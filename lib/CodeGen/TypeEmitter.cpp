#include "TypeEmitter.h"
#include "AST/Decl.h"
#include "Seman/Seman.h"
#include <llvm/Support/Casting.h>
#include <llvm/IR/DerivedTypes.h>
#include <ranges>

void TypeEmitter::visit(const IntegerType& Ty) {
    returnValue(llvm::IntegerType::get(Context, Ty.getWidth()));
}

void TypeEmitter::visit(const FloatingPointType& Ty) {
    if (Ty.getWidth() == 32) {
        returnValue(llvm::Type::getFloatTy(Context));
    } else {
        returnValue(llvm::Type::getDoubleTy(Context));
    }
}

void TypeEmitter::visit(const FunctionType& Ty) {
    const auto RetTy = emit(*Ty.getReturnType());
    std::vector<llvm::Type*> Params;
    std::ranges::transform(Ty.getParamTypes(), std::back_inserter(Params), [&](const auto& Param) {
        return emit(*Param);
    });

    const auto FuncTy = llvm::FunctionType::get(RetTy, Params, false);
    returnValue(FuncTy);
}

void TypeEmitter::visit(const ArrayType& Ty) {
    const auto InnerTy = emit(*Ty.getElementType());
    const auto ArrayTy = llvm::ArrayType::get(InnerTy, Ty.getSize());
    returnValue(ArrayTy);
}

void TypeEmitter::visit(const PointerType& Ty) {
    const auto InnerTy = emit(*Ty.getElementType());
    const auto PointerTy = llvm::PointerType::get(InnerTy, 0);
    returnValue(PointerTy);
}

void TypeEmitter::visit(const StructType& Ty) {
    const auto& Decl = Ty.getDecl();
    std::vector<llvm::Type*> FieldTys;

    for (const auto& Field : Decl.getFields()) {
        FieldTys.push_back(emit(*SemanInfo.getType(Field)));
    }

    returnValue(llvm::StructType::create(Context, FieldTys));
}

void TypeEmitter::visit(const PrimitiveType& Ty) {
    if (Ty.isBoolType()) {
        return returnValue(llvm::IntegerType::get(Context, 1));
    }
    if (Ty.isVoidType()) {
        return returnValue(llvm::Type::getVoidTy(Context));
    }
    assert(false);
}

void TypeEmitter::visit(const UnresolvedType& Ty) {
    assert(false && "Unresolved Type");
}

llvm::Type* TypeEmitter::emit(const Type& Ty) {
    if (Types.contains(&Ty)) {
        return Types[&Ty];
    }
    return Types[&Ty] = doVisit(Ty);
}
