#pragma once
#include "Utils/SourceFile.h"
#include "Lexer.h"
#include "AST/ASTBase.h"
#include "AST/UnresolvedType.h"
#include <string>
#include <optional>
#include <memory>
#include <vector>

class ErrorReporter;
class Declaration;
class Statement;
class Expression;
class Module;

class Parser {
public:
    static AstPtr<Module> parseFile(std::string FilePath);
    explicit Parser(SourceFile Source, ErrorReporter& Reporter);
    AstPtr<Module> parseModule();
private:
    template <typename... T>
    std::optional<Token> consumeToken(T... Args);
    Token expectToken(const TokenKind &Kind);
    std::string expectIdentifier();
    Token advanceToken();

    AstPtr<Declaration> parseFunctionDecl();
    AstPtr<Declaration> parseStructDecl();

    AstPtr<Statement> parseStmt();
    AstPtr<Statement> parseCompoundStmt();
    AstPtr<Statement> parseWhileStmt();
    AstPtr<Statement> parseIfStmt();
    AstPtr<Statement> parseLetStmt();
    AstPtr<Statement> parseReturnStmt();
    AstPtr<Statement> parseExpressionStmt();

    AstPtr<Expression> parseExpr();

    AstPtr<Expression> parseBinaryExpr(int Prec = 0);
    AstPtr<Expression> parseCastExpr();
    AstPtr<Expression> parseUnaryExpr();
    AstPtr<Expression> parsePostFixExpr();
    std::vector<AstPtr<Expression>> parseCallArgs();
    AstPtr<Expression> parsePrimaryExpr();

    std::unique_ptr<UnresolvedType> parseTypeAnnotation();
    std::unique_ptr<UnresolvedType> parseType();

    ErrorReporter& Reporter;

    SourceFile Source;
    Lexer Lex;
    Token CurTok;
};
