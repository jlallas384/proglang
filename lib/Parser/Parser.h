#pragma once
#include "Utils/SourceFile.h"
#include "Lexer.h"
#include "AST/ASTBase.h"
#include "AST/Type.h"
#include "AST/TypeContext.h"
#include "AST/Identifier.h"
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
    static AstPtr<Module> parseSourceFile(SourceFile& Source, ErrorReporter& Reporter, std::shared_ptr<TypeContext> TyContext = std::make_shared<TypeContext>());
    explicit Parser(SourceFile& Source, ErrorReporter& Reporter);
    AstPtr<Module> parseModule(std::shared_ptr<TypeContext> TypeContext);
private:
    template <typename... T>
    std::optional<Token> consumeToken(T... Args);
    Token expectToken(TokenKind Kind);
    IdentifierSymbol expectIdentifier();
    Token advanceToken();

    AstPtr<Declaration> parseFunctionDecl();
    AstPtr<Declaration> parseStructDecl();

    AstPtr<Statement> parseStmt();
    AstPtr<Statement> parseCompoundStmt();
    AstPtr<Statement> parseWhileStmt();
    AstPtr<Statement> parseIfStmt();
    AstPtr<Statement> parseLetStmt();
    AstPtr<Statement> parseReturnStmt();
    AstPtr<Statement> parseExpressionOrAssignStmt();
    AstPtr<Statement> parseAssignStmt(AstPtr<Expression> Left);

    AstPtr<Expression> parseExpr();

    AstPtr<Expression> parseBinaryExpr(int Prec = 0);
    AstPtr<Expression> parseCastExpr();
    AstPtr<Expression> parseUnaryExpr();
    AstPtr<Expression> parsePostFixExpr();
    std::vector<AstPtr<Expression>> parseCallArgs();
    AstPtr<Expression> parsePrimaryExpr();

    TypeInfo parseTypeAnnotation();
    TypeInfo parseArrayType();
    TypeInfo parseType();

    ErrorReporter& Reporter;
    std::shared_ptr<TypeContext> TyContext;
    SourceFile &Source;
    Lexer Lex;
    Token CurTok;
};
