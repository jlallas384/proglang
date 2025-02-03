#pragma once
#include "AST/ASTVisitor.h"
#include "Scope.h"
#include "Symbol.h"
#include <map>

class Seman;

class NameResolver : public AstVisitor {
public:
    NameResolver(Seman& SemanInfo, const std::vector<const StructType*> &StructTypes);
    void visit(const Module&) override;
    void visit(const FunctionDecl&) override;
    void visit(const LetStmt&) override;
    void visit(const CompoundStmt&) override;
    void visit(const NamedExpr&) override;
private:
    Seman& SemanInfo;
    std::map<std::string, const Type*> Types;
    std::unique_ptr<Scope<Symbol>> CurrentScope = nullptr;
}; 