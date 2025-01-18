#pragma once
#include "AST/TypeVisitor.h"
#include "AST/Type.h"
#include "Utils/VisitorBase.h"

class TypeContext;


class TypeResolver : public VisitorBase<TypeResolver, const Type, const Type*>, public TypeVisitor {
public:
    TypeResolver(TypeContext& TyContext, const Type* ResolvedType);
    void visit(const UnresolvedType& Ty) override;
    void visit(const PointerType& Ty) override;
    void visit(const ArrayType& Ty) override;
private:
    TypeContext& TyContext;
    const Type* ResolvedType;
};