#pragma once
#include "AST/ASTVisitor.h"
#include "Scope.h"
#include <map>

class Seman;

class NameResolver : public AstVisitor {
public:
    NameResolver(Seman& SemanInfo, const std::vector<const StructType*> &StructTypes);
    void visit(Module&) override;
    void visit(FunctionDecl&) override;
    void visit(LetStmt&) override;
    void visit(CompoundStmt&) override;
    void visit(NamedExpr&) override;
    void visit(CastExpr&) override;
    void visit(StructDecl&) override;
private:
    const Type* tryResolveType(const Type& Ty);
    Seman& SemanInfo;
    std::map<std::string, const Type*> Types;
    std::unique_ptr<Scope<const Nameable*>> CurrentScope = nullptr;
}; 