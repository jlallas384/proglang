#include "Parser.h"
#include <iostream>
#include <format>

void Parser::parseFile(std::string FilePath) {
    const auto Source = SourceInfo::fromPath(std::move(FilePath));
    Parser P(Source);
}

Parser::Parser(SourceInfo Source) : Source(std::move(Source)), Lex(this->Source), CurTok(Lex.nextToken()) {

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
        auto Loc = Tok.getLoc();
        auto Message = std::format("{}:{}: Expected '{}', found '{}'", Loc.LineNum, Loc.Column, Kind.toString(), Tok.getKind().toString());
        std::cout << Message << '\n';
        exit(0);
    }
    return Tok;
}

std::string Parser::expectIdentifier() {
    const auto Tok = expectToken(Identifier);
    return Tok.getValue();
}

Token Parser::advanceToken() {
    auto Tok = CurTok;
    CurTok = Lex.nextToken();
    return Tok;
}

void Parser::parseModule() {
    std::vector<AstPtr<Declaration>> Nodes;
    while (!CurTok.is(Eof)) {
        if (CurTok.is(Function)) {
            Nodes.push_back(parseFunctionDecl());
        }
    }
}

AstPtr<FunctionDecl> Parser::parseFunctionDecl() {
    expectToken(Function);
    auto Name = expectIdentifier();
    expectToken(LeftParen);
    std::vector<std::string> ArgNames;
    std::vector<const Type*> ArgTypes;
    if (!CurTok.is(RightParen)) {
        Name = expectIdentifier();
        ArgNames.push_back(Name);
        ArgTypes.push_back(parseTypeAnnotation());
    }
    while (!CurTok.is(RightParen)) {
        expectToken(Comma);
        Name = expectIdentifier();
        ArgNames.push_back(Name);
        ArgTypes.push_back(parseTypeAnnotation());
    }
    expectToken(RightParen);
    const Type* RetType = parseTypeAnnotation();
    const FunctionType* FuncType = Types.getFunctionType(RetType, ArgTypes);
    AstPtr<CompoundStmt> Body = parseCompoundStmt();

    return std::make_unique<FunctionDecl>(Name, ArgNames, FuncType, std::move(Body));
}

AstPtr<Statement> Parser::parseStmt() {
    if (CurTok.is(LeftBrace)) {
        return parseCompoundStmt();
    }
    if (CurTok.is(If)) {
        return parseIfStmt();
    }

    return nullptr;
}

AstPtr<CompoundStmt> Parser::parseCompoundStmt() {
    expectToken(LeftBrace);
    std::vector<AstPtr<Statement>> Body;
    while (!consumeToken(RightBrace)) {
        Body.push_back(parseStmt());
    }
    return std::make_unique<CompoundStmt>(std::move(Body));
}

AstPtr<IfStmt> Parser::parseIfStmt() {
    expectToken(If);
    expectToken(LeftParen);

    expectToken(RightParen);
    auto TrueBlock = parseStmt();
    AstPtr<Statement> FalseBlock = nullptr;
    if (consumeToken(Else)) {
        FalseBlock = parseStmt();
    }
    return std::make_unique<IfStmt>(nullptr, std::move(TrueBlock), std::move(FalseBlock));
}

const Type* Parser::parseTypeAnnotation() {
    expectToken(Colon);
    return parseType();
}

const Type* Parser::parseType() {
    const Type* Type = nullptr;
    const auto TypeName = expectIdentifier();
    Type = Types.findTypeByName(TypeName);
    return Type;

}
