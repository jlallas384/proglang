#pragma once
#include "ASTBase.h"
#include "Expr.h"
#include "Stmt.h"
#include "Types/Type.h"
#include <string>
#include <vector>

class Declaration : public AstBase {
public:
    Declaration() = default;
};

class VariableDecl : public Declaration {
public:
    VariableDecl(Type* Type, std::string Identifier, AstPtr<Expression> Value = nullptr);
private:
    Type* VarType;
    std::string Identifier;
    AstPtr<Expression> Value;
};

class FunctionDecl : public Declaration {
public:
    FunctionDecl(std::string Name, std::vector<std::string> ArgNames, const FunctionType* FuncType, AstPtr<CompoundStmt> Body) :
        Name(std::move(Name)), ArgNames(std::move(ArgNames)), FuncType(FuncType), Body(std::move(Body)) {}
    [[nodiscard]] const CompoundStmt* getBody() const {
        return Body.get();
    }
    void accept(AstVisitor& Visitor) override;
private:
    std::string Name;
    std::vector<std::string> ArgNames;
    const FunctionType* FuncType;
    AstPtr<CompoundStmt> Body;
};