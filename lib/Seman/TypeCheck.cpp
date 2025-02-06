#include "TypeCheck.h"
#include "AST/TypeVisitor.h"
#include "Seman.h"
#include <format>

void TypeCheck::visit(const LiteralExpr& LiteralExpr) {
    if (LiteralExpr.is<IntLiteral>()) {
        returnValue(SemanInfo.getTyContext().getI32Type());
    } else if (LiteralExpr.is<FloatLiteral>()) {
        returnValue(SemanInfo.getTyContext().getF32Type());
    }
}

void TypeCheck::visit(const BinaryOpExpr& BinaryOpExpr) {
    const auto LeftTy = doVisit(BinaryOpExpr.getLeft());
    const auto RightTy = doVisit(BinaryOpExpr.getRight());

    const auto BinTy = LeftTy->applyBinaryOp(BinaryOpExpr.getKind(), RightTy);
    if (BinTy == nullptr) {
        const auto Msg = std::format("operator {} on type '{}' and '{}' is not allowed", kindToString(BinaryOpExpr.getKind()),
            LeftTy->toString(), RightTy->toString());
        SemanInfo.error(BinaryOpExpr.getRange(), Msg);
    } else {
        returnValue(BinTy);
    }
}

void TypeCheck::visit(const NamedExpr& NamedExpr) {
    const auto& NameRef = *SemanInfo.getReferencedName(NamedExpr.getIdentifier());
    returnValue(SemanInfo.getType(NameRef));
}

void TypeCheck::visit(const FunctionCallExpr& FunctionCallExpr) {
    const auto InnerTy = doVisit(FunctionCallExpr.getFunction());

    if (InnerTy->getTag() == TypeTag::Function) {
        const auto& FunctionTy = InnerTy->as<const FunctionType>();
        validateCallArgs(FunctionTy, FunctionCallExpr);
        returnValue(FunctionTy.getReturnType());
    } else {
        const auto Msg = std::format("type '{}' is not callable", InnerTy->toString());
        SemanInfo.error(FunctionCallExpr.getFunction().getRange(), Msg);
    }
}

void TypeCheck::visit(const ReturnStmt& ReturnStmt) {
    auto RetValueType = SemanInfo.getTyContext().getVoidType();
    auto ReturnVal = ReturnStmt.getValue();
    if (ReturnVal) {
        RetValueType = doVisit(*ReturnVal);
    }

    auto FuncRetType = SemanInfo.getType(*CurrentFunction)->as<const FunctionType>().getReturnType();

    if (RetValueType != FuncRetType) {
        const auto Msg = std::format("expected return type '{}', found '{}'", FuncRetType->toString(), RetValueType->toString());
        SemanInfo.error(ReturnVal->getRange(), Msg);
    }
}

void TypeCheck::visit(const FunctionDecl& FunctionDecl) {
    CurrentFunction = &FunctionDecl;
    AstVisitor::visit(FunctionDecl);
}

void TypeCheck::visit(const LetStmt& LetStmt) {
    const auto ExprTy = doVisit(*LetStmt.getValue()); // TODO empty rhs
    const auto LetTy = SemanInfo.getType(LetStmt);
    if (LetTy != ExprTy) {
        const auto Msg = std::format("expected type '{}', found '{}'", ExprTy->toString(), LetTy->toString());
        SemanInfo.error(LetStmt.getValue()->getRange(), Msg);
    }
}

void TypeCheck::visit(const WhileStmt& WhileStmt) {
    const auto& Cond = WhileStmt.getCondition();
    const auto CondTy = doVisit(Cond);
    if (CondTy != SemanInfo.getTyContext().getBoolType()) {
        const auto Msg = std::format("while condition expects a bool type, found '{}' instead", CondTy->toString());
        SemanInfo.error(Cond.getRange(), Msg);
    }

    WhileStmt.getBody().accept(*this);
}

void TypeCheck::visit(const IfStmt& IfStmt) {
    const auto& Cond = IfStmt.getCondition();
    const auto CondTy = doVisit(Cond);
    if (CondTy != SemanInfo.getTyContext().getBoolType()) {
        const auto Msg = std::format("if condition expects a bool type, found '{}' instead", CondTy->toString());
        SemanInfo.error(Cond.getRange(), Msg);
    }

    IfStmt.getTrueBlock()->accept(*this);
    if (const auto FalseBlock = IfStmt.getFalseBlock()) {
        FalseBlock->accept(*this);
    }
}

void TypeCheck::visit(const UnaryOpExpr& UnaryOpExpr) {
    const auto ExprTy = doVisit(UnaryOpExpr.getValue());
    const auto ResultTy = ExprTy->applyUnaryOp(UnaryOpExpr.getKind());

    returnValue(ResultTy);
}

void TypeCheck::visit(const StructDecl& StructDecl) {
    TyValidator.validate(*SemanInfo.getType(StructDecl));
}

void TypeCheck::visit(const SubscriptExpr& SubscriptExpr) {
    const auto& Expr = SubscriptExpr.getExpr();
    const auto ExprTy = doVisit(Expr);

    if (ExprTy->getTag() != TypeTag::Array) {
        SemanInfo.error(Expr.getRange(), "must be array type");
    } else {
        const auto& ArrayTy = ExprTy->as<const ArrayType&>();
        returnValue(ArrayTy.getElementType());
    }

    const auto& Subscript = SubscriptExpr.getSubscript();
    const auto SubscriptTy = doVisit(Subscript);

    if (SubscriptTy->getTag() != TypeTag::Integer) {
        SemanInfo.error(Subscript.getRange(), "subscript must be of type integer");
    }
}

void TypeCheck::validateCallArgs(const FunctionType& FunctionTy, const FunctionCallExpr& FunctionCallExpr) {
    std::vector<const Type*> ArgTypes;
    for (auto& Arg : FunctionCallExpr.getArgs()) {
        ArgTypes.push_back(doVisit(*Arg));
    }

    const auto& ParamTypes = FunctionTy.getParamTypes();
    if (ArgTypes.size() != ParamTypes.size()) {
        const auto Msg = std::format("function expects {} arguments, found {}", ParamTypes.size(), ArgTypes.size());
        SemanInfo.error(FunctionCallExpr.getRange(), Msg);
    } else {
        for (unsigned Index = 0; Index < ArgTypes.size(); Index++) {
            if (ArgTypes[Index] != ParamTypes[Index]) {
                const auto Msg = std::format("expects '{}', found '{}'", ParamTypes[Index]->toString(), ArgTypes[Index]->toString());
                SemanInfo.error(FunctionCallExpr.getArg(Index).getRange(), Msg);
            }
        }
    }
}

