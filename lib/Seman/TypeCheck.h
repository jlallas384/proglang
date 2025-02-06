#pragma once
#include "TypeValidator.h"
#include "AST/ASTVisitor.h"
#include "Utils/VisitorBase.h"
#include <cassert>
#include <utility>

class Seman;

class ExprResult {
public:
    ExprResult(const Type* ExprTy = nullptr, bool IsLValue = false) : ExprTy(ExprTy), IsLValue(IsLValue) {}
    bool isLValue() const { return IsLValue; }
    bool isInvalid() const { return ExprTy == nullptr; }
    const Type* getType() const { return ExprTy; }
    operator const Type* () const { return ExprTy; }
private:
    const Type* ExprTy;
    bool IsLValue;
};

class TypeCheck : public VisitorBase<TypeCheck, const AstBase, ExprResult>, public AstVisitor {
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
    void visit(const AssignStmt&) override;
    void visit(const DotExpr&) override;

private:
    ExprResult check(const AstBase& Node);
    void validateCallArgs(const FunctionType& FunctionTy, const FunctionCallExpr& FunctionCallExpr);
    std::pair<const StructDeclField*, const Type*> getStructField(const StructType& StructTy,
                                                                      const std::string& FieldName) const;

    Seman& SemanInfo;
    TypeValidator TyValidator;

    const FunctionDecl* CurrentFunction = nullptr;
};