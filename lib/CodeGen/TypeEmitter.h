#pragma once
#include "AST/TypeVisitor.h"
#include "Utils/VisitorBase.h"
#include <map>

class Seman;

namespace llvm {
    class LLVMContext;
    class Type;
}

class TypeEmitter : public VisitorBase<TypeEmitter, const Type, llvm::Type*>, public TypeVisitor {
public:
    TypeEmitter(llvm::LLVMContext& Context, Seman& SemanInfo) : Context(Context), SemanInfo(SemanInfo) {}
    void visit(const IntegerType& Ty) override;
    void visit(const FloatingPointType& Ty) override;
    void visit(const FunctionType& Ty) override;
    void visit(const ArrayType& Ty) override;
    void visit(const PointerType& Ty) override;
    void visit(const StructType& Ty) override;
    void visit(const PrimitiveType& Ty) override;
    void visit(const UnresolvedType& Ty) override;

    llvm::Type* emit(const Type& Ty);
private:
    llvm::LLVMContext& Context;
    Seman& SemanInfo;
    std::map<const Type*, llvm::Type*> Types;
};