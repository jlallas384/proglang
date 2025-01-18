#pragma once
#include "Type.h"

class TypeVisitor {
public:
    virtual void visit(const PrimitiveType& Ty) {}
    virtual void visit(const IntegerType& Ty) {}
    virtual void visit(const FloatingPointType& Ty) {}
    virtual void visit(const FunctionType& Ty) {}
    virtual void visit(const PointerType& Ty) {}
    virtual void visit(const ArrayType& Ty) {}
    virtual void visit(const UnresolvedType& Ty) {}
    virtual void visit(const StructType& Ty) {}
};