#pragma once
#include "TypeValidator.h"
#include "AST/ASTVisitor.h"
#include "Utils/VisitorBase.h"

class Seman;

class TypeCheck : public VisitorBase<TypeCheck, const AstBase, const Type*>, public AstVisitor {
public:
    TypeCheck(Seman& SemanInfo) :  SemanInfo(SemanInfo), TyValidator(SemanInfo) {}
    void visit(const LiteralExpr&) override;
    void visit(const BinaryOpExpr&) override;
    void visit(const NamedExpr&) override;
    void visit(const FunctionCallExpr&) override;
    void visit(const ReturnStmt&) override;
    void visit(const FunctionDecl&) override;
    void visit(const LetStmt&) override;
    void visit(const WhileStmt&) override;
    void visit(const IfStmt&) override;
    void visit(const UnaryOpExpr&) override;
    void visit(const StructDecl&) override;
    void visit(const SubscriptExpr&) override;

private:
    void validateCallArgs(const FunctionType& FunctionTy, const FunctionCallExpr& FunctionCallExpr);
    Seman& SemanInfo;
    TypeValidator TyValidator;

    const FunctionDecl* CurrentFunction = nullptr;
};

