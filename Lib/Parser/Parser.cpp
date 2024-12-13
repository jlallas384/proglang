#include "Parser.h"
#include "Utils/ErrorReporter.h"
#include "AST/Decl.h"
#include "AST/Stmt.h"
#include <format>

#include "AST/Module.h"

void Parser::parseFile(std::string FilePath) {
    const auto Source = SourceFile::fromPath(std::move(FilePath));
    ErrorReporter Reporter;
    Parser P(Source, Reporter);
}

Parser::Parser(SourceFile Source, ErrorReporter& Reporter) : Reporter(Reporter), Source(std::move(Source)), Lex(this->Source), CurTok(Lex.nextToken()) {

}

template <typename ... T>
std::optional<Token> Parser::consumeToken(T... Args) {
    if (CurTok.is(Args...)) {
        auto Ret = CurTok;
        CurTok = Lex.nextToken();
        return Ret;
    }
    return std::nullopt;
}

Token Parser::expectToken(const TokenKind &Kind) {
    auto Tok = advanceToken();
    if (!Tok.is(Kind)) {
        const auto Loc = Tok.getLoc();
        const auto Message = std::format("Expected '{}', found '{}'", kindToString(Kind), kindToString(Tok.getKind()));
        Reporter.error(Source, Loc, Message);
    }
    return Tok;
}

std::string Parser::expectIdentifier() {
    const auto Tok = expectToken(TokenKind::Identifier);
    return Tok.getValue();
}

Token Parser::advanceToken() {
    auto Tok = CurTok;
    CurTok = Lex.nextToken();
    return Tok;
}

AstPtr<Module> Parser::parseModule() {
    std::vector<AstPtr<Declaration>> Nodes;
    while (!CurTok.is(TokenKind::Eof)) {
        if (CurTok.is(TokenKind::Function)) {
            Nodes.push_back(parseFunctionDecl());
        }
    }
    return std::make_unique<Module>(std::move(Nodes));
}

AstPtr<Declaration> Parser::parseFunctionDecl() {
    expectToken(TokenKind::Function);
    auto Name = expectIdentifier();
    expectToken(TokenKind::LeftParen);
    std::vector<std::string> ParamNames;
    std::vector<const Type*> ParamTypes;
    if (!CurTok.is(TokenKind::RightParen)) {
        Name = expectIdentifier();
        ParamNames.push_back(Name);
        ParamTypes.push_back(parseTypeAnnotation());
    }
    while (!consumeToken(TokenKind::RightParen)) {
        expectToken(TokenKind::Comma);
        Name = expectIdentifier();
        ParamNames.push_back(Name);
        ParamTypes.push_back(parseTypeAnnotation());
    }
    const Type* RetType = parseTypeAnnotation();
    const FunctionType* FuncType = Types.getFunctionType(RetType, ParamTypes);
    AstPtr<Statement> Body = parseCompoundStmt();

    return std::make_unique<FunctionDecl>(Name, ParamNames, FuncType, std::move(Body));
}

AstPtr<Statement> Parser::parseStmt() {
    if (CurTok.is(TokenKind::LeftBrace)) {
        return parseCompoundStmt();
    }
    if (CurTok.is(TokenKind::If)) {
        return parseIfStmt();
    }
    if (CurTok.is(TokenKind::Let)) {
        return parseLetStmt();
    }
    if (CurTok.is(TokenKind::Return)) {
        return parseReturnStmt();
    }
    return nullptr;
}

AstPtr<Statement> Parser::parseCompoundStmt() {
    expectToken(TokenKind::LeftBrace);
    std::vector<AstPtr<Statement>> Body;
    while (!consumeToken(TokenKind::RightBrace)) {
        Body.push_back(parseStmt());
    }
    return std::make_unique<CompoundStmt>(std::move(Body));
}

AstPtr<Statement> Parser::parseIfStmt() {
    expectToken(TokenKind::If);
    expectToken(TokenKind::LeftParen);
    auto Cond = parseExpr();
    expectToken(TokenKind::RightParen);
    auto TrueBlock = parseStmt();
    AstPtr<Statement> FalseBlock = nullptr;
    if (consumeToken(TokenKind::Else)) {
        FalseBlock = parseStmt();
    }
    return std::make_unique<IfStmt>(std::move(Cond), std::move(TrueBlock), std::move(FalseBlock));
}

AstPtr<Statement> Parser::parseLetStmt() {
    expectToken(TokenKind::Let);
    auto Name = expectIdentifier();
    const Type* Type = parseTypeAnnotation();
    expectToken(TokenKind::Equal);
    auto Value = parseExpr();
    expectToken(TokenKind::Semicolon);
    return std::make_unique<LetStmt>(std::move(Name), Type, std::move(Value));
}

AstPtr<Statement> Parser::parseReturnStmt() {
    expectToken(TokenKind::Return);
    AstPtr<Expression> Expr = nullptr;
    if (!CurTok.is(TokenKind::Semicolon)) {
        Expr = parseExpr();
    }
    expectToken(TokenKind::Semicolon);
    return std::make_unique<ReturnStmt>(std::move(Expr));
}

AstPtr<Expression> Parser::parseExpr() {
    return parseBinaryExpr();
}

namespace {
    constexpr auto binOpPrecedence(TokenKind Kind) {
        switch (Kind) {
            case TokenKind::PipePipe:
                return 0;
            case TokenKind::AmpAmp:
                return 1;
            case TokenKind::Pipe:
                return 2;
            case TokenKind::Caret:
                return 3;
            case TokenKind::EqualEqual:
            case TokenKind::NotEqual:
                return 4;
            case TokenKind::LessEqual:
            case TokenKind::Less:
            case TokenKind::Greater:
            case TokenKind::GreaterEqual:
                return 5;
            case TokenKind::LessLess:
            case TokenKind::GreaterGreater:
                return 6;
            case TokenKind::Plus:
            case TokenKind::Minus:
                return 7;
            case TokenKind::Star:
            case TokenKind::Slash:
            case TokenKind::Percent:
                return 8;
            default:
                return -1;
        }
    }
}

AstPtr<Expression> Parser::parseBinaryExpr(int Prec) {
    if (Prec == 9) {
        return parseCastExpr();
    }
    auto Expr = parseBinaryExpr(Prec + 1);
    while (binOpPrecedence(CurTok.getKind()) == Prec) {
        const auto Tok = advanceToken();
        auto Right = parseBinaryExpr(Prec + 1);
        Expr = std::make_unique<BinaryOpExpr>(Tok.getKind(), std::move(Expr), std::move(Right));
    }
    return Expr;
}

AstPtr<Expression> Parser::parseCastExpr() {
    AstPtr<Expression> Expr = parseUnaryExpr();
    if (consumeToken(TokenKind::As)) {
        auto Type = parseType();
        Expr = std::make_unique<CastExpr>(Type, std::move(Expr));
    }
    return Expr;
}

AstPtr<Expression> Parser::parseUnaryExpr() {
    const auto Tok = consumeToken(TokenKind::Plus, TokenKind::Minus, TokenKind::Tilde);
    if (Tok) {
        auto Expr = parsePostFixExpr();
        return std::make_unique<UnaryOpExpr>(Tok->getKind(), std::move(Expr));
    }
    return parsePostFixExpr();
}

AstPtr<Expression> Parser::parsePostFixExpr() {
    auto Expr = parsePrimaryExpr();
    switch (CurTok.getKind()) {
        case TokenKind::Dot: {
            advanceToken();
            auto Identifier = expectIdentifier();
            Expr = std::make_unique<DotExpr>(std::move(Expr), Identifier);
            break;
        }
        case TokenKind::LeftParen: {
            break;
        }
        default:
            break;
    }
    return Expr;
}

AstPtr<Expression> Parser::parsePrimaryExpr() {
    AstPtr<Expression> Expr = nullptr;
    switch (CurTok.getKind()) {
        case TokenKind::Integer:
            Expr = std::make_unique<LiteralExpr>(std::stoi(CurTok.getValue()));
            advanceToken();
            break;
        case TokenKind::Identifier:
            Expr = std::make_unique<NamedExpr>(CurTok.getValue());
            advanceToken();
            break;
        case TokenKind::LeftParen:
            advanceToken();
            Expr = parseExpr();
            expectToken(TokenKind::RightParen);
            break;
    }
    return Expr;
}

const Type* Parser::parseTypeAnnotation() {
    expectToken(TokenKind::Colon);
    return parseType();
}

const Type* Parser::parseType() {
    const auto Tok = expectToken(TokenKind::Identifier);
    const auto TypeName = Tok.getValue();
    const Type* Type = Types.findTypeByName(TypeName);

    if (!Type) {
        auto Message = std::format("Type '{}' not found", TypeName);
        Reporter.error(Source, Tok.getLoc(), Message);
    }
    return Type;

}
