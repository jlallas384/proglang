#pragma once
#include "AST/ASTVisitor.h"
#include "Scope.h"
#include "Symbol.h"

class SemanInfo;

class NameResolver : public AstVisitor {
public:
    NameResolver(SemanInfo& Info);
    void visit(const Module&) override;
    void visit(const FunctionDecl&) override;
    void visit(const LetStmt&) override;
    void visit(const CompoundStmt&) override;
    void visit(const NamedExpr&) override;
private:
    SemanInfo& Info;
    std::unique_ptr<Scope<Symbol>> CurrentScope = nullptr;
}; 