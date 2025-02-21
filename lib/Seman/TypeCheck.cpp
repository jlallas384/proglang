#include "TypeCheck.h"
#include "Seman.h"
#include <format>
#include <cassert>

void TypeCheck::visit(LiteralExpr& LiteralExpr) {
    if (LiteralExpr.is<IntLiteral>()) {
        returnValue({ SemanInfo.getTyContext().getI32Type() });
    } else if (LiteralExpr.is<FloatLiteral>()) {
        returnValue({ SemanInfo.getTyContext().getF32Type() });
    } else if (LiteralExpr.is<BoolLiteral>()) {
        returnValue({ SemanInfo.getTyContext().getBoolType() });
    }
}

void TypeCheck::visit(BinaryOpExpr& BinaryOpExpr) {
    const auto LeftRes = check(BinaryOpExpr.getLeft());
    const auto RightRes = check(BinaryOpExpr.getRight());

    if (LeftRes.isInvalid() || RightRes.isInvalid()) {
        return typecheckFail();
    }

    const auto LeftTy = LeftRes.getType();
    const auto RightTy = RightRes.getType();
    const auto Range = BinaryOpExpr.getRange();

    ExprResult Res{};
    switch (const auto Kind = BinaryOpExpr.getKind()) {
        case TokenKind::Plus:
        case TokenKind::Minus:
            Res = checkBinaryAddSubOp(LeftTy, RightTy, Range, Kind);
            break;
        case TokenKind::AmpAmp:
        case TokenKind::PipePipe:
            Res = checkBinaryLogicalOp(LeftTy, RightTy, Range, Kind);
            break;
        case TokenKind::Star:
        case TokenKind::Slash:
            Res = checkBinaryMulDivOp(LeftTy, RightTy, Range, Kind);
            break;
        case TokenKind::Less:
        case TokenKind::LessEqual:
        case TokenKind::Greater:
        case TokenKind::GreaterEqual:
            Res = checkBinaryInequalityOp(LeftTy, RightTy, Range, Kind);
            break;
        case TokenKind::EqualEqual:
        case TokenKind::NotEqual:
            Res = checkBinaryEqualityOp(LeftTy, RightTy, Range, Kind);
            break;
        default:
            assert(false);
            break;

    }

    returnValue(Res);
}

void TypeCheck::visit(NamedExpr& NamedExpr) {
    const auto& NameRef = *NamedExpr.getRefedName();
    auto Ty = SemanInfo.getType(NameRef);

    if (Ty->isArrayType()) {
        const auto& ArrTy = Ty->as<ArrayType>();
        Ty = SemanInfo.getTyContext().getPointerType(ArrTy.getElementType());
    }

    returnValue({ Ty, true });
}

void TypeCheck::visit(FunctionCallExpr& FunctionCallExpr) {
    const auto Res = check(FunctionCallExpr.getFunction());
    if (Res.isInvalid()) {
        return typecheckFail();
    }

    const Type* InnerTy = Res.getType();
    if (InnerTy->isFunctionType()) {
        const auto& FunctionTy = InnerTy->as<FunctionType>();
        validateCallArgs(FunctionTy, FunctionCallExpr);
        returnValue(FunctionTy.getReturnType());
    } else {
        const auto Msg = std::format("type '{}' is not callable", InnerTy->toString());
        SemanInfo.error(FunctionCallExpr.getFunction().getRange(), Msg);
    }
}

void TypeCheck::visit(ReturnStmt& ReturnStmt) {
    auto RetValueType = SemanInfo.getTyContext().getVoidType();
    auto ReturnVal = ReturnStmt.getValue();
    if (ReturnVal) {
        const auto Res = check(*ReturnVal);
        if (Res.isInvalid()) {
            return typecheckFail();
        }
        RetValueType = Res.getType();
    }

    auto FuncRetType = SemanInfo.getType(*CurrentFunction)->as<FunctionType>().getReturnType();

    if (RetValueType != FuncRetType) {
        const auto Msg = std::format("expected return type '{}', found '{}'", FuncRetType->toString(), RetValueType->toString());
        SemanInfo.error(ReturnVal->getRange(), Msg); // TODO empty return
    }
}

void TypeCheck::visit(FunctionDecl& FunctionDecl) {
    CurrentFunction = &FunctionDecl;
    AstVisitor::visit(FunctionDecl);
}

void TypeCheck::visit(LetStmt& LetStmt) {
    const auto Value = LetStmt.getValue();
    const auto LetTy = SemanInfo.getType(LetStmt);
    if (Value) {
        const auto Res = check(*Value);
        if (Res.isInvalid()) {
            return typecheckFail();
        }

        const auto ExprTy = Res.getType();

        if (LetTy->isArrayType() && ExprTy->isArrayType()) {
            const auto& ArrLetTy = LetTy->as<ArrayType>();
            const auto& ArrExprTy = ExprTy->as<ArrayType>();

            if (ArrLetTy.getElementType() != ArrExprTy.getElementType() || 
                ArrLetTy.getSize() < ArrExprTy.getSize()) {

                const auto Msg = std::format("incompatible array types", LetTy->toString(), ExprTy->toString());
                SemanInfo.error(LetStmt.getValue()->getRange(), Msg);
                return typecheckFail();
            }
            return;
        }
        if (LetTy != ExprTy) {
            const auto Msg = std::format("expected type '{}', found '{}'", LetTy->toString(), ExprTy->toString());
            SemanInfo.error(LetStmt.getValue()->getRange(), Msg);
        }
    }

}

void TypeCheck::visit(WhileStmt& WhileStmt) {
    auto& Cond = WhileStmt.getCondition();
    const auto Res = check(Cond);

    if (!Res.isInvalid()) {
        const auto CondTy = Res.getType();
        if (CondTy != SemanInfo.getTyContext().getBoolType()) {
            const auto Msg = std::format("while condition expects a bool type, found '{}'", CondTy->toString());
            SemanInfo.error(Cond.getRange(), Msg);
        }
    }

    WhileStmt.getBody().accept(*this);
}

void TypeCheck::visit(IfStmt& IfStmt) {
    auto& Cond = IfStmt.getCondition();
    const auto Res = check(Cond);

    if (!Res.isInvalid()) {
        const auto CondTy = Res.getType();
        if (CondTy != SemanInfo.getTyContext().getBoolType()) {
            const auto Msg = std::format("if condition expects a bool type, found '{}'", CondTy->toString());
            SemanInfo.error(Cond.getRange(), Msg);
        }
    }

    IfStmt.getTrueBlock()->accept(*this);
    if (const auto FalseBlock = IfStmt.getFalseBlock()) {
        FalseBlock->accept(*this);
    }
}

void TypeCheck::visit(UnaryOpExpr& UnaryOpExpr) {
    auto Res = check(UnaryOpExpr.getValue());
    if (Res.isInvalid()) {
        return typecheckFail();
    }

    const auto Range = UnaryOpExpr.getRange();
    switch (const auto Kind = UnaryOpExpr.getKind()) {
        case TokenKind::Star:
            Res = checkDereferenceOp(Res, Range);
            break;
        case TokenKind::Amp:
            Res = checkAddressofOp(Res, Range);
            break;
        case TokenKind::Plus:
        case TokenKind::Minus:
            Res = checkUnaryPlusMinusOp(Res, Range, Kind);
            break;
        case TokenKind::ExclMark:
            Res = checkLogicalNotOp(Res, Range);
            break;
        default:
            assert(false);
            break;
    }
    returnValue(Res);
}

void TypeCheck::visit(StructDecl& StructDecl) {
    TyValidator.validate(*SemanInfo.getType(StructDecl));
}

void TypeCheck::visit(SubscriptExpr& SubscriptExpr) {
    auto& Expr = SubscriptExpr.getExpr();
    const auto ExprRes = check(Expr);
    const auto ExprTy = ExprRes.getType();

    bool Fail = false;

    if (!ExprRes.isInvalid()) {
        if (!ExprTy->isArrayType() && !ExprTy->isPointerType()) {
            SemanInfo.error(Expr.getRange(), "subscripted value must be array or pointer type");
            Fail = true;
        }
    }

    auto& Subscript = SubscriptExpr.getSubscript();
    const auto SubscriptRes = check(Subscript);
    const auto SubscriptTy = SubscriptRes.getType();

    if (!SubscriptRes.isInvalid()) {
        if (!SubscriptTy->isIntegerType()) {
            SemanInfo.error(Subscript.getRange(), "subscript must be of type integer");
            Fail = true;
        }
    }

    if (!Fail) {
        const auto& PointerTy = ExprTy->as<PointerType>();
        auto ElemTy = PointerTy.getElementType();

        if (ElemTy->isArrayType()) {
            const auto& ArrTy = ElemTy->as<ArrayType>();
            ElemTy = SemanInfo.getTyContext().getPointerType(ArrTy.getElementType());
        }

        return returnValue({ ElemTy, true });
    }
    return typecheckFail();
}

void TypeCheck::visit(AssignStmt& AssignStmt) {
    const auto LeftRes = check(AssignStmt.getLeft());

    auto& Right = AssignStmt.getRight();
    const auto RightRes = check(Right);

    if (LeftRes.isInvalid() || RightRes.isInvalid()) {
        return;
    }

    const auto LeftTy = LeftRes.getType();

    if (LeftTy->isArrayType()) {
        const auto Msg = std::format("can't assign to array type '{}'", LeftTy->toString());
        SemanInfo.error(AssignStmt.getRight().getRange(), Msg);
        return typecheckFail();
    }

    if (LeftRes.isLValue()) {
        const auto RightTy = RightRes.getType();
        if (LeftTy != RightTy) {
            const auto Msg = std::format("expected type '{}', found '{}'", LeftTy->toString(), RightTy->toString());
            SemanInfo.error(Right.getRange(), Msg);
        }
    } else {
        const auto Msg = std::format("left hand side of assignment must be an lvalue");
        SemanInfo.error(AssignStmt.getLeft().getRange(), Msg);
    }
}

void TypeCheck::visit(DotExpr& DotExpr) {
    const auto LeftRes = check(DotExpr.getExpr());
    if (LeftRes.isInvalid()) {
        return typecheckFail();
    }

    const auto& Identifier = DotExpr.getIdentifier();

    if (LeftRes.getType()->isStructType()) {
        const auto& StructTy = LeftRes.getType()->as<StructType>();
        const auto [Field, FieldTy] = getStructField(StructTy, Identifier.getName());

        if (!Field) {
            const auto Msg = std::format("type '{}' has no field named '{}'", StructTy.toString(), Identifier.getName());
            SemanInfo.error(Identifier.getRange(), Msg);
        } else {
            return returnValue({ FieldTy, LeftRes.isLValue() });
        }
    } else {
        const auto Msg = std::format("left hand side must be of a struct type");
        SemanInfo.error(DotExpr.getExpr().getRange(), Msg);
    }

    return typecheckFail();
}

void TypeCheck::visit(CompoundExpr& CompoundExpr) {
    std::vector<const Type*> Types;
    for (const auto& Expr : CompoundExpr.getExprs()) {
        const auto Res = check(*Expr);
        if (Res.isInvalid()) {
            return typecheckFail();
        }
        Types.push_back(Res.getType());
    }
    for (const auto Ty : Types) {
        if (Ty != Types[0]) {
            const auto Msg = "array initializer must have homogenous types";
            SemanInfo.error(CompoundExpr.getRange(), Msg);
            return typecheckFail();
        }
    }
    const auto ArrayTy = SemanInfo.getTyContext().getArrayType(Types[0], Types.size());
    returnValue({ ArrayTy });
}

void TypeCheck::visit(CastExpr& CastExpr) {
    const auto CastTo = CastExpr.getType();
    if (!CastTo) {
        return typecheckFail();
    }
    const auto CastFrom = check(CastExpr.getValue()).getType();
    if (!CastFrom) {
        return typecheckFail();
    }

    if (checkCast(CastFrom, CastTo)) {
        return returnValue({ CastTo });
    }
    const auto Msg = std::format("can't cast from '{}' to '{}'", CastFrom->toString(), CastTo->toString());
    SemanInfo.error(CastExpr.getRange(), Msg);
    return typecheckFail();
}


ExprResult TypeCheck::check(Expression& Node) {
    returnValue({});
    const auto Res = doVisit(Node);
    Node.setType(Res.getType());
    return Res;
}

void TypeCheck::validateCallArgs(const FunctionType& FunctionTy, const FunctionCallExpr& FunctionCallExpr) {
    std::vector<ExprResult> ArgRes;
    for (auto& Arg : FunctionCallExpr.getArgs()) {
        ArgRes.push_back(check(*Arg));
    }

    const auto& ParamTypes = FunctionTy.getParamTypes();
    if (ArgRes.size() != ParamTypes.size()) {
        const auto Msg = std::format("function expects {} arguments, found {}", ParamTypes.size(), ArgRes.size());
        SemanInfo.error(FunctionCallExpr.getRange(), Msg);
    } else {
        for (unsigned Index = 0; Index < ArgRes.size(); Index++) {
            if (ArgRes[Index].isInvalid()) continue;
            if (ArgRes[Index].getType() != ParamTypes[Index]) {
                const auto Msg = std::format("expects '{}', found '{}'", ParamTypes[Index]->toString(), ArgRes[Index].getType()->toString());
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

bool TypeCheck::checkCast(const Type* From, const Type* To) {
    if (From->isArithmeticType() && (To->isArithmeticType() || To->isBoolType())) {
        return true;
    }
    if (From->isBoolType() && To->isArithmeticType()) {
        return true;
    }
    return false;
}

ExprResult TypeCheck::checkDereferenceOp(ExprResult Res, const SourceRange& Range) const {
    const auto ResTy = Res.getType();

    if (!ResTy->isPointerType()) {
        const auto Msg = std::format("operand to a dereference operator must be a pointer type, found '{}' instead", ResTy->toString());
        SemanInfo.error(Range, Msg);
        return {};
    }

    const auto& PointerTy = ResTy->as<PointerType>();
    return { PointerTy.getElementType(), true };
}

ExprResult TypeCheck::checkAddressofOp(ExprResult Res, const SourceRange& Range) const {
    if (!Res.isLValue()) {
        const auto Msg = std::format("operand to an address-of operator must be an l value");
        SemanInfo.error(Range, Msg);
        return {};
    }

    return { SemanInfo.getTyContext().getPointerType(Res.getType()) };
}

ExprResult TypeCheck::checkUnaryPlusMinusOp(ExprResult Res, const SourceRange& Range, TokenKind Op) const {
    const auto ResTy = Res.getType();
    if (!ResTy->isArithmeticType()) {
        const auto Msg = std::format("operand to unary '{}' must be a signed integer or floating point type, found '{}' instead", 
            kindToString(Op), ResTy->toString());
        SemanInfo.error(Range, Msg);
        return {};
    }

    return { ResTy };
}

ExprResult TypeCheck::checkLogicalNotOp(ExprResult Res, const SourceRange& Range) const {
    const auto ResTy = Res.getType();

    if (!ResTy->isBoolType()) {
        const auto Msg = std::format("operand to '!' must be a bool type, found '{}' instead", 
            ResTy->toString());
        SemanInfo.error(Range, Msg);
        return {};
    }

    return { ResTy };
}

ExprResult TypeCheck::checkBinaryAddSubOp(const Type* Left, const Type* Right, const SourceRange& Range, TokenKind Op) const {
    if (Left->isArithmeticType() && Left == Right) {
        return { Left };
    }

    bool IsSwapped = false;
    if (Right->isPointerType()) {
        IsSwapped = true;
        std::swap(Left, Right);
    }

    if (Left->isPointerType() && Right->isIntegerType()) {
        return { Left };
    }

    if (IsSwapped) {
        std::swap(Left, Right);
    }

    const auto Msg = std::format("invalid operands of type '{}' and '{}' to operator '{}'", 
        Left->toString(), Right->toString(), kindToString(Op));
    SemanInfo.error(Range, Msg);
    return {};
}

ExprResult TypeCheck::checkBinaryLogicalOp(const Type* Left, const Type* Right, const SourceRange& Range,
                                           TokenKind Op) const {
    if (!Left->isBoolType() || !Right->isBoolType()) {
        const auto Msg = std::format("operand to '{}' must be a bool type, found '{}' and '{}' instead", 
                kindToString(Op), Left->toString(), Right->toString());

        SemanInfo.error(Range, Msg);
        return {};
    }
    return { SemanInfo.getTyContext().getBoolType() };
}

ExprResult TypeCheck::checkBinaryMulDivOp(const Type* Left, const Type* Right, const SourceRange& Range,
    TokenKind Op) const {

    if (Left != Right) {
        const auto Msg = std::format("operands to '{}' must be integer or floating point type, found '{}' and '{}' instead",
            kindToString(Op), Left->toString(), Right->toString());
        SemanInfo.error(Range, Msg);
        return {};
    }
    return { Left };
}

ExprResult TypeCheck::checkBinaryInequalityOp(const Type* Left, const Type* Right, const SourceRange& Range,
    TokenKind Op) const {
    if (Left == Right && (Left->isArithmeticType() || Left->isPointerType())) {
        return { SemanInfo.getTyContext().getBoolType() };
    }
    const auto Msg = std::format("invalid operands of type '{}' and '{}' to operator '{}'",
        Left->toString(), Right->toString(), kindToString(Op));
    SemanInfo.error(Range, Msg);
    return {};
}

ExprResult TypeCheck::checkBinaryEqualityOp(const Type* Left, const Type* Right, const SourceRange& Range,
    TokenKind Op) const {
    if (Left == Right && (Left->isArithmeticType() || Left->isPointerType()) || Left->isBoolType()) {
        return { SemanInfo.getTyContext().getBoolType() };
    }
    const auto Msg = std::format("invalid operands of type '{}' and '{}' to operator '{}'",
        Left->toString(), Right->toString(), kindToString(Op));
    SemanInfo.error(Range, Msg);
    return {};
}

