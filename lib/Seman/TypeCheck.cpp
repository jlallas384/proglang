#include "TypeCheck.h"
#include "Seman.h"
#include <format>

void TypeCheck::visit(const LiteralExpr& LiteralExpr) {
    if (LiteralExpr.is<IntLiteral>()) {
        returnValue({ SemanInfo.getTyContext().getI32Type() });
    } else if (LiteralExpr.is<FloatLiteral>()) {
        returnValue({ SemanInfo.getTyContext().getF32Type() });
    }
}

void TypeCheck::visit(const BinaryOpExpr& BinaryOpExpr) {
    const Type* LeftTy = check(BinaryOpExpr.getLeft());
    const Type* RightTy = check(BinaryOpExpr.getRight());

    const auto BinTy = LeftTy->applyBinaryOp(BinaryOpExpr.getKind(), RightTy);
    if (BinTy == nullptr) {
        const auto Msg = std::format("operator {} on type '{}' and '{}' is not allowed", kindToString(BinaryOpExpr.getKind()),
            LeftTy->toString(), RightTy->toString());
        SemanInfo.error(BinaryOpExpr.getRange(), Msg);
    } else {
        returnValue({ BinTy });
    }
}

void TypeCheck::visit(const NamedExpr& NamedExpr) {
    const auto& NameRef = *SemanInfo.getReferencedName(NamedExpr.getIdentifier());
    returnValue({ SemanInfo.getType(NameRef), true });
}

void TypeCheck::visit(const FunctionCallExpr& FunctionCallExpr) {
    const Type* InnerTy = check(FunctionCallExpr.getFunction());

    if (InnerTy->getTag() == TypeTag::Function) {
        const auto& FunctionTy = InnerTy->as<FunctionType>();
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
        RetValueType = check(*ReturnVal);
    }

    auto FuncRetType = SemanInfo.getType(*CurrentFunction)->as<FunctionType>().getReturnType();

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
    const auto Value = LetStmt.getValue();
    const auto LetTy = SemanInfo.getType(LetStmt);
    if (Value) {
        const Type* ExprTy = check(*LetStmt.getValue()); // TODO empty rhs
        if (LetTy != ExprTy) {
            const auto Msg = std::format("expected type '{}', found '{}'", LetTy->toString(), ExprTy->toString());
            SemanInfo.error(LetStmt.getValue()->getRange(), Msg);
        }
    }

}

void TypeCheck::visit(const WhileStmt& WhileStmt) {
    const auto& Cond = WhileStmt.getCondition();
    const Type* CondTy = check(Cond);
    if (CondTy != SemanInfo.getTyContext().getBoolType()) {
        const auto Msg = std::format("while condition expects a bool type, found '{}' instead", CondTy->toString());
        SemanInfo.error(Cond.getRange(), Msg);
    }

    WhileStmt.getBody().accept(*this);
}

void TypeCheck::visit(const IfStmt& IfStmt) {
    const auto& Cond = IfStmt.getCondition();
    const Type* CondTy = check(Cond);
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
    const Type* ExprTy = check(UnaryOpExpr.getValue());
    const auto ResultTy = ExprTy->applyUnaryOp(UnaryOpExpr.getKind());

    returnValue({ ResultTy });
}

void TypeCheck::visit(const StructDecl& StructDecl) {
    TyValidator.validate(*SemanInfo.getType(StructDecl));
}

void TypeCheck::visit(const SubscriptExpr& SubscriptExpr) {
    const auto& Expr = SubscriptExpr.getExpr();
    const Type* ExprTy = check(Expr);

    bool Fail = false;
    if (ExprTy->getTag() != TypeTag::Array) {
        SemanInfo.error(Expr.getRange(), "must be array type");
        Fail = true;
    }

    const auto& Subscript = SubscriptExpr.getSubscript();
    const Type* SubscriptTy = check(Subscript);

    if (SubscriptTy->getTag() != TypeTag::Integer) {
        SemanInfo.error(Subscript.getRange(), "subscript must be of type integer");
        Fail = true;
    }

    if (!Fail) {
        const auto& ArrayTy = ExprTy->as<ArrayType>();
        returnValue({ ArrayTy.getElementType(), true });
    }
}

void TypeCheck::visit(const AssignStmt& AssignStmt) {
    const auto LeftRes = check(AssignStmt.getLeft());

    const auto& Right = AssignStmt.getRight();
    const Type* RightTy = check(Right);

    if (LeftRes.isLValue()) {
        const auto LeftTy = LeftRes.getType();
        if (LeftTy != RightTy) {
            const auto Msg = std::format("expected type '{}', found '{}'", LeftTy->toString(), RightTy->toString());
            SemanInfo.error(Right.getRange(), Msg);
        }
    } else {
        const auto Msg = std::format("left hand side of assignment must be an lvalue");
        SemanInfo.error(AssignStmt.getLeft().getRange(), Msg);
    }
}

void TypeCheck::visit(const DotExpr& DotExpr) {
    const auto LeftRes = check(DotExpr.getExpr());
    const auto& Identifier = DotExpr.getIdentifier();

    if (LeftRes.getType()->getTag() == TypeTag::Struct) {
        const auto& StructTy = LeftRes.getType()->as<StructType>();
        const auto [Field, FieldTy] = getStructField(StructTy, Identifier.getName());

        if (!Field) {
            const auto Msg = std::format("type '{}' has no field named '{}'", StructTy.toString(), Identifier.getName());
            SemanInfo.error(Identifier.getRange(), Msg);
        } else {
            returnValue({ FieldTy, LeftRes.isLValue() });
        }
    } else {
        const auto Msg = std::format("left hand side must be of a struct type");
        SemanInfo.error(DotExpr.getExpr().getRange(), Msg);
    }
}

ExprResult TypeCheck::check(const AstBase& Node) {
    returnValue({});
    return doVisit(Node);
}

void TypeCheck::validateCallArgs(const FunctionType& FunctionTy, const FunctionCallExpr& FunctionCallExpr) {
    std::vector<const Type*> ArgTypes;
    for (auto& Arg : FunctionCallExpr.getArgs()) {
        ArgTypes.push_back(check(*Arg));
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

std::pair<const StructDeclField*, const Type*> TypeCheck::getStructField(
    const StructType& StructTy, const std::string& FieldName) const {
    const auto Field = StructTy.getField(FieldName);
    if (!Field) {
        return { nullptr, nullptr };
    }
    return { Field, SemanInfo.getType(*Field) };
}

