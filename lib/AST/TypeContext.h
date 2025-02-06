#pragma once
#include "Type.h"
#include <vector>
#include <memory>

class TypeContext {
public:
    TypeContext();
    const PointerType* getPointerType(const Type* ElementType);
    const ArrayType* getArrayType(const Type* ElementType, std::uint32_t Size);
    const FunctionType* getFunctionType(const Type* ReturnType, const std::vector<const Type*>& ParamTypes);
    const UnresolvedType* createUnresolvedType(IdentifierSymbol Identifier);
    const StructType* createStructType(std::string Name, const StructDecl& Decl);
    auto& getBuiltinTypes() const { return BuiltinTypes; }
    const Type* getBoolType() const { return &BoolTy; }
    const Type* getI32Type() const { return &I32Ty; }
    const Type* getF32Type() const { return &F32Ty; }
    const Type* getVoidType() const { return &VoidTy; }
private:
    IntegerType I8Ty, I16Ty, I32Ty, I64Ty;
    FloatingPointType F32Ty, F64Ty;
    PrimitiveType BoolTy, VoidTy;
    std::vector<std::unique_ptr<PointerType>> PointerTypes;
    std::vector<std::unique_ptr<ArrayType>> ArrayTypes;
    std::vector<std::unique_ptr<FunctionType>> FunctionTypes;
    std::vector<std::unique_ptr<StructType>> StructTypes;
    std::vector<std::unique_ptr<UnresolvedType>> UnresolvedTypes;
    std::vector<const Type*> BuiltinTypes;
};
