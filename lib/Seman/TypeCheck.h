#pragma once
#include "TypeValidator.h"
#include "AST/ASTVisitor.h"
#include "Utils/VisitorBase.h"
#include <utility>

class Seman;

class ExprResult {
public:
    ExprResult(const Type* ExprTy = nullptr, bool IsLValue = false) : ExprTy(ExprTy), IsLValue(IsLValue) {
    }

    bool isLValue() const { return IsLValue; }
    bool isInvalid() const { return ExprTy == nullptr; }
    const Type* getType() const { return ExprTy; }

private:
    const Type* ExprTy;
    bool IsLValue;
};

class TypeCheck : public VisitorBase<TypeCheck, AstBase, ExprResult>, public AstVisitor {
public:
    TypeCheck(Seman& SemanInfo) : SemanInfo(SemanInfo), TyValidator(SemanInfo) {
    }

    void visit(LiteralExpr&) override;
    void visit(BinaryOpExpr&) override;
    void visit(NamedExpr&) override;
    void visit(FunctionCallExpr&) override;
    void visit(ReturnStmt&) override;
    void visit(FunctionDecl&) override;
    void visit(LetStmt&) override;
    void visit(WhileStmt&) override;
    void visit(IfStmt&) override;
    void visit(UnaryOpExpr&) override;
    void visit(StructDecl&) override;
    void visit(SubscriptExpr&) override;
    void visit(AssignStmt&) override;
    void visit(DotExpr&) override;
    void visit(CompoundExpr&) override;
    void visit(CastExpr&) override;

private:
    ExprResult check(Expression& Node);

    void typecheckFail() { returnValue({}); }

    void validateCallArgs(const FunctionType& FunctionTy, const FunctionCallExpr& FunctionCallExpr);
    std::pair<const StructDeclField*, const Type*> getStructField(const StructType& StructTy,
                                                                  const std::string& FieldName) const;
    static bool checkCast(const Type* From, const Type* To);
    ExprResult checkDereferenceOp(ExprResult Res, const SourceRange& Range) const;
    ExprResult checkAddressofOp(ExprResult Res, const SourceRange& Range) const;
    ExprResult checkUnaryPlusMinusOp(ExprResult Res, const SourceRange& Range, TokenKind Op) const;
    ExprResult checkLogicalNotOp(ExprResult Res, const SourceRange& Range) const;

    ExprResult checkBinaryAddSubOp(const Type* Left, const Type* Right, const SourceRange& Range, TokenKind Op) const;
    ExprResult checkBinaryLogicalOp(const Type* Left, const Type* Right, const SourceRange& Range, TokenKind Op) const;
    ExprResult checkBinaryMulDivOp(const Type* Left, const Type* Right, const SourceRange& Range, TokenKind Op) const;
    ExprResult checkBinaryInequalityOp(const Type* Left, const Type* Right, const SourceRange& Range, TokenKind Op) const;
    ExprResult checkBinaryEqualityOp(const Type* Left, const Type* Right, const SourceRange& Range, TokenKind Op) const;

    Seman& SemanInfo;
    TypeValidator TyValidator;

    const FunctionDecl* CurrentFunction = nullptr;
};
