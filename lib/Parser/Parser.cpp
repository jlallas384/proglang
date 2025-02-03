#include "Parser.h"
#include "Utils/ErrorReporter.h"
#include "AST/Decl.h"
#include "AST/Stmt.h"
#include "AST/Module.h"
#include <format>

AstPtr<Module> Parser::parseSourceFile(SourceFile& Source, ErrorReporter& Reporter, std::shared_ptr<TypeContext> TyContext) {
    Parser P(Source, Reporter);
    return P.parseModule(std::move(TyContext));
}

Parser::Parser(SourceFile& Source, ErrorReporter& Reporter) : Reporter(Reporter), Source(Source),
                                                             Lex(this->Source), CurTok(Lex.nextToken()) {
}

template <typename... T>
std::optional<Token> Parser::consumeToken(T... Args) {
    if (CurTok.is(Args...)) {
        auto Ret = CurTok;
        CurTok = Lex.nextToken();
        return Ret;
    }
    return std::nullopt;
}

Token Parser::expectToken(TokenKind Kind) {
    auto Tok = advanceToken();
    if (!Tok.is(Kind)) {
        const auto Message = std::format("Expected '{}', found '{}'", kindToString(Kind), kindToString(Tok.getKind()));
        Reporter.error(Source, Tok.getRange(), Message);
    }
    return Tok;
}

IdentifierSymbol Parser::expectIdentifier() {
    const auto Tok = expectToken(TokenKind::Identifier);
    return { Tok.getValue(), Tok.getRange() };
}

Token Parser::advanceToken() {
    auto Tok = CurTok;
    CurTok = Lex.nextToken();
    return Tok;
}

AstPtr<Module> Parser::parseModule(std::shared_ptr<TypeContext> TypeContext) {
    TyContext = std::move(TypeContext);
    std::vector<AstPtr<Declaration>> Nodes;
    while (!CurTok.is(TokenKind::Eof)) {
        if (CurTok.is(TokenKind::Function)) {
            Nodes.push_back(parseFunctionDecl());
        } else if (CurTok.is(TokenKind::Struct)) {
            Nodes.push_back(parseStructDecl());
        }
    }
    return std::make_unique<Module>(std::move(Nodes), std::move(TyContext), Source);
}

AstPtr<Declaration> Parser::parseFunctionDecl() {
    expectToken(TokenKind::Function);
    auto Name = expectIdentifier();
    expectToken(TokenKind::LeftParen);
    std::vector<FunctionDecl::Param> Params;
    if (!CurTok.is(TokenKind::RightParen)) {
        const auto ParamName = expectIdentifier();
        Params.emplace_back(ParamName, parseTypeAnnotation());
    }
    while (!consumeToken(TokenKind::RightParen)) {
        expectToken(TokenKind::Comma);
        const auto ParamName = expectIdentifier();
        Params.emplace_back(ParamName, parseTypeAnnotation());
    }
    auto RetType = parseTypeAnnotation();
    AstPtr<Statement> Body = parseCompoundStmt();
    return std::make_unique<FunctionDecl>(Name, RetType, std::move(Params), std::move(Body));
}

AstPtr<Declaration> Parser::parseStructDecl() {
    expectToken(TokenKind::Struct);
    auto Name = expectIdentifier();
    expectToken(TokenKind::LeftBrace);
    std::vector<StructDecl::Field> Fields;
    StructDecl::Field Field = {
        expectIdentifier(),
        parseTypeAnnotation()
    };
    Fields.push_back(std::move(Field));
    while (consumeToken(TokenKind::Comma)) {
        auto FieldName = expectIdentifier();
        Fields.emplace_back(FieldName, parseTypeAnnotation());
    }
    expectToken(TokenKind::RightBrace);
    return std::make_unique<StructDecl>(Name, std::move(Fields));
}

AstPtr<Statement> Parser::parseStmt() {
    switch (CurTok.getKind()) {
        case TokenKind::LeftBrace:
            return parseCompoundStmt();
        case TokenKind::If:
            return parseIfStmt();
        case TokenKind::Let:
            return parseLetStmt();
        case TokenKind::Return:
            return parseReturnStmt();
        case TokenKind::While:
            return parseWhileStmt();
        default:
            return parseExpressionStmt();
    }
}

AstPtr<Statement> Parser::parseCompoundStmt() {
    expectToken(TokenKind::LeftBrace);
    std::vector<AstPtr<Statement>> Body;
    while (!consumeToken(TokenKind::RightBrace)) {
        Body.push_back(parseStmt());
    }
    return std::make_unique<CompoundStmt>(std::move(Body));
}

AstPtr<Statement> Parser::parseWhileStmt() {
    expectToken(TokenKind::While);
    expectToken(TokenKind::LeftParen);
    auto Cond = parseExpr();
    expectToken(TokenKind::RightParen);
    auto Body = parseStmt();
    return std::make_unique<WhileStmt>(std::move(Cond), std::move(Body));
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
    auto Type = parseTypeAnnotation();
    expectToken(TokenKind::Equal);
    auto Value = parseExpr();
    expectToken(TokenKind::Semicolon);
    return std::make_unique<LetStmt>(std::move(Name).getName(), Type, std::move(Value)); // TODO 
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

AstPtr<Statement> Parser::parseExpressionStmt() {
    auto Expr = parseExpr();
    // TODO other assign
    if (consumeToken(TokenKind::Equal)) {
        Expr = std::make_unique<BinaryOpExpr>(TokenKind::Equal, std::move(Expr), parseExpr());
    }
    expectToken(TokenKind::Semicolon);
    return std::make_unique<ExpressionStmt>(std::move(Expr));
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
    if (Prec > binOpPrecedence(TokenKind::Star)) {
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
    const auto Tok = consumeToken(TokenKind::Plus, TokenKind::Minus, TokenKind::Tilde, TokenKind::Star, TokenKind::Amp, TokenKind::ExclMark);
    if (Tok) {
        auto Expr = parsePostFixExpr();
        return std::make_unique<UnaryOpExpr>(Tok->getStart(), Tok->getKind(), std::move(Expr));
    }
    return parsePostFixExpr();
}

AstPtr<Expression> Parser::parsePostFixExpr() {
    auto Expr = parsePrimaryExpr(); //TODO loop
    switch (CurTok.getKind()) {
        case TokenKind::Dot: {
            advanceToken();
            auto Identifier = expectIdentifier();
            Expr = std::make_unique<DotExpr>(std::move(Expr), Identifier);
            break;
        }
        case TokenKind::LeftParen: {
            auto Args = parseCallArgs();
            const auto RParen = expectToken(TokenKind::RightParen);
            Expr = std::make_unique<FunctionCallExpr>(std::move(Expr), std::move(Args), RParen.getEnd());
            break;
        }
        default:
            break;
    }
    return Expr;
}

std::vector<AstPtr<Expression>> Parser::parseCallArgs() {
    expectToken(TokenKind::LeftParen);
    std::vector<AstPtr<Expression>> Args;
    if (!CurTok.is(TokenKind::RightParen)) {
        Args.push_back(parseExpr());
    }
    while (!CurTok.is(TokenKind::RightParen)) {
        expectToken(TokenKind::Comma);
        Args.push_back(parseExpr());
    }
    return Args;
}

AstPtr<Expression> Parser::parsePrimaryExpr() {
    AstPtr<Expression> Expr = nullptr;
    switch (CurTok.getKind()) {
        case TokenKind::Integer:
            Expr = std::make_unique<LiteralExpr>(IntLiteral{ std::stoull(CurTok.getValue()) }, CurTok.getRange());
            advanceToken();
            break;
        case TokenKind::True:
            Expr = std::make_unique<LiteralExpr>(BoolLiteral{ true }, CurTok.getRange());
            advanceToken();
            break;
        case TokenKind::False:
            Expr = std::make_unique<LiteralExpr>(BoolLiteral{ false }, CurTok.getRange());
            advanceToken();
            break;
        case TokenKind::Identifier:
            Expr = std::make_unique<NamedExpr>(IdentifierSymbol{ CurTok.getValue(), CurTok.getRange() });
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

const Type* Parser::parseArrayType() {
    expectToken(TokenKind::LeftSqrBrace);
    auto Inner = parseType();
    expectToken(TokenKind::Comma);
    auto SizeTok = expectToken(TokenKind::Integer);
    auto Size = std::stoi(SizeTok.getValue());
    expectToken(TokenKind::RightSqrBrace);
    return TyContext->getArrayType(Inner, Size);
}

const Type* Parser::parseType() {
    if (CurTok.is(TokenKind::LeftSqrBrace)) {
        return parseArrayType();
    }
    const auto Name = expectIdentifier();
    return TyContext->createUnresolvedType(Name);
}
