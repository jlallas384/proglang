#include "TypeContext.h"
#include <ranges>

TypeContext::TypeContext() :
    I8Ty(*this, "i8", 8, true),
    I16Ty(*this, "i16", 16, true),
    I32Ty(*this, "i32", 32, true),
    I64Ty(*this, "i64", 64, true),
    F32Ty(*this, "f32", 32),
    F64Ty(*this, "f64", 64),
    BoolTy(*this, "bool"),
    VoidTy(*this, "void"),
    BuiltinTypes({ &I8Ty, &I16Ty, &I32Ty, &I64Ty, &F32Ty, &F64Ty, &BoolTy, &VoidTy })
{
}

const PointerType* TypeContext::getPointerType(const Type* ElementType) {
    const auto Iter = std::ranges::find_if(PointerTypes, [&](const auto& Ptr) {
        return Ptr->getElementType() == ElementType;
    });
    if (Iter != PointerTypes.end()) {
        return Iter->get();
    }
    auto Ptr = std::make_unique<PointerType>(*this, ElementType);
    PointerTypes.push_back(std::move(Ptr));
    return PointerTypes.back().get();
}

const ArrayType* TypeContext::getArrayType(const Type* ElementType, std::uint32_t Size) {
    const auto Iter = std::ranges::find_if(ArrayTypes, [&](const auto& Ptr) {
        return Ptr->getElementType() == ElementType && Ptr->getSize() == Size;
    });
    if (Iter != ArrayTypes.end()) {
        return Iter->get();
    }
    auto Ptr = std::make_unique<ArrayType>(*this, ElementType, Size);
    ArrayTypes.push_back(std::move(Ptr));
    return ArrayTypes.back().get();
}

const FunctionType* TypeContext::getFunctionType(const Type* ReturnType, const std::vector<const Type*>& ParamTypes) {
    const auto Iter = std::ranges::find_if(FunctionTypes, [&](const auto& Ptr) {
        return Ptr->getReturnType() == ReturnType && Ptr->getParamTypes() == ParamTypes;
    });
    if (Iter != FunctionTypes.end()) {
        return Iter->get();
    }
    auto Ptr = std::make_unique<FunctionType>(*this, ReturnType, ParamTypes);
    FunctionTypes.push_back(std::move(Ptr));
    return FunctionTypes.back().get();
}

const UnresolvedType* TypeContext::createUnresolvedType(IdentifierSymbol Identifier) {
    UnresolvedTypes.push_back(std::make_unique<UnresolvedType>(*this, std::move(Identifier)));
    return UnresolvedTypes.back().get();
}

const StructType* TypeContext::createStructType(std::string Name, const StructDecl& Decl) {
    StructTypes.push_back(std::make_unique<StructType>(*this, std::move(Name), Decl));
    return StructTypes.back().get();
}
