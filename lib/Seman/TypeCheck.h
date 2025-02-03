#pragma once
#include "AST/ASTVisitor.h"
#include "Utils/VisitorBase.h"

class Seman;

class TypeCheck : public VisitorBase<TypeCheck, const AstBase, const Type*>, public AstVisitor {
public:
    TypeCheck(Seman& SemanInfo) : SemanInfo(SemanInfo) {}
    void visit(const LiteralExpr&) override;
    void visit(const BinaryOpExpr&) override;
    void visit(const NamedExpr&) override;
    void visit(const FunctionCallExpr&) override;
private:
    template <typename T>
    const Type* getType(const T& Node) const;
    const FunctionType* checkFunctionCall(const FunctionDecl* FnDecl, const FunctionCallExpr& FunctionCallExpr);
    Seman& SemanInfo;
};

