#pragma once
#include "Type.h"
#include <vector>
#include <memory>

class TypeContext {
public:
    TypeContext();
    const PointerType* getPointerType(const Type* ElementType);
    const ArrayType* getArrayType(const Type* ElementType, std::uint32_t Size);
    const FunctionType* getFunctionType(const Type* ReturnType, const std::vector<const Type*> &ParamTypes);
    const StructType* createStructType(std::string Name, std::map<std::string, const Type*> Fields);

    const Type* getBoolType() const { return &BoolTy; }
private:
    IntegerType I8Ty, I16Ty, I32Ty, I64Ty;
    FloatingPointType F32Ty, F64Ty;
    PrimitiveType BoolTy;
    std::vector<std::unique_ptr<PointerType>> PointerTypes;
    std::vector<std::unique_ptr<ArrayType>> ArrayTypes;
    std::vector<std::unique_ptr<FunctionType>> FunctionTypes;
    std::vector<std::unique_ptr<StructType>> StructTypes;
    std::vector<const Type*> BuiltinTypes;
};