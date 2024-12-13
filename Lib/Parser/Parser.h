#pragma once
#include "Utils/SourceFile.h"
#include "Lexer.h"
#include "Types/TypesManager.h"
#include "AST/ASTBase.h"
#include <string>
#include <optional>

class ErrorReporter;
class Declaration;
class Statement;
class Expression;
class Module;

class Parser {
public:
    static void parseFile(std::string FilePath);
    explicit Parser(SourceFile Source, ErrorReporter& Reporter);
    AstPtr<Module> parseModule();
public:

    template <typename... T>
    std::optional<Token> consumeToken(T... Args);
    Token expectToken(const TokenKind &Kind);
    std::string expectIdentifier();
    Token advanceToken();

    AstPtr<Declaration> parseFunctionDecl();

    AstPtr<Statement> parseStmt();
    AstPtr<Statement> parseCompoundStmt();
    AstPtr<Statement> parseIfStmt();
    AstPtr<Statement> parseLetStmt();
    AstPtr<Statement> parseReturnStmt();

    AstPtr<Expression> parseExpr();

    AstPtr<Expression> parseBinaryExpr(int Prec = 0);
    AstPtr<Expression> parseCastExpr();
    AstPtr<Expression> parseUnaryExpr();
    AstPtr<Expression> parsePostFixExpr();
    AstPtr<Expression> parsePrimaryExpr();

    const Type* parseTypeAnnotation();
    const Type* parseType();

    ErrorReporter& Reporter;
    TypesManager Types;
    SourceFile Source;
    Lexer Lex;
    Token CurTok;
};
