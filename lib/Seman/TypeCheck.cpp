#include "TypeCheck.h"
#include "Seman.h"
#include <format>
#include <cassert>
#include <iostream>

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

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
        auto Msg = std::format("operator {} on type '{}' and '{}' is not allowed", kindToString(BinaryOpExpr.getKind()),
            LeftTy->toString(), RightTy->toString());
        SemanInfo.error(BinaryOpExpr.getRange(), Msg);
    } else {
        returnValue(BinTy);
    }
}

void TypeCheck::visit(const NamedExpr& NamedExpr) {
    const auto SymRef = SemanInfo.get(NamedExpr).SymRef;
    std::visit(overloaded{
        [&](auto Arg) {
            returnValue(getType(*Arg));
        }
    }, SymRef);
}

void TypeCheck::visit(const FunctionCallExpr& FunctionCallExpr) {
    auto CalleeSymRef = SemanInfo.get(FunctionCallExpr.getFunction()).SymRef;
    std::visit(overloaded{
        [&](const FunctionDecl* FnDecl) {
            const auto FuncTy = checkFunctionCall(FnDecl, FunctionCallExpr);
            returnValue(FuncTy->getReturnType());
        },
        [&](const LetStmt* Arg) {
            std::cout << Arg->getIdentifier() << " " << Arg->getType()->toString() << " is not callable";
        },
        [&](FunctionParam Arg) {
            std::cout << Arg->Identifier.getName() << "FUCKER\n";
        }
    }, CalleeSymRef);
}

template <typename T>
const Type* TypeCheck::getType(const T& Node) const {
    assert(SemanInfo.has(Node));
    return SemanInfo.get(Node).Ty;
}

const FunctionType* TypeCheck::checkFunctionCall(const FunctionDecl* FnDecl, const FunctionCallExpr& FunctionCallExpr) {
    std::vector<const Type*> ArgTypes;
    for (auto& Arg : FunctionCallExpr.getArgs()) {
        ArgTypes.push_back(doVisit(*Arg));
    }
    const auto FuncTy = dynamic_cast<const FunctionType*>(getType(*FnDecl));
    assert(FuncTy);

    const auto& ParamTypes = FuncTy->getParamTypes();
    if (ArgTypes.size() != ParamTypes.size()) {
        auto Msg = std::format("function expects {} arguments, found {}", ParamTypes.size(), ArgTypes.size());
        SemanInfo.error(FunctionCallExpr.getRange(), Msg);
    } else {
        for (unsigned Index = 0; Index < ArgTypes.size(); Index++) {
            if (ArgTypes[Index] != ParamTypes[Index]) {
                auto Msg = std::format("expects '{}', found '{}'", ParamTypes[Index]->toString(), ArgTypes[Index]->toString());
                SemanInfo.error(FunctionCallExpr.getArg(Index).getRange(), Msg);
            }
        }
    }
    return FuncTy;
}