#pragma once
#include "SourceInfo.h"
#include "Lexer.h"
#include "AST/Decl.h"
#include "Types/TypesManager.h"
#include <string>
#include <optional>

class Parser {
public:
    static void parseFile(std::string FilePath);
    explicit Parser(SourceInfo Source);
    void parseModule();
private:

    template <typename... T>
    std::optional<Token> consumeToken(T... Args);
    Token expectToken(const TokenKind &Kind);
    std::string expectIdentifier();
    Token advanceToken();

    AstPtr<FunctionDecl> parseFunctionDecl();

    AstPtr<Statement> parseStmt();
    AstPtr<CompoundStmt> parseCompoundStmt();
    AstPtr<IfStmt> parseIfStmt();

    const Type* parseTypeAnnotation();
    const Type* parseType();

    TypesManager Types;
    SourceInfo Source;
    Lexer Lex;
    Token CurTok;
};
