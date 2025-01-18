#include "TypeResolver.h"
#include "AST/TypeContext.h"

TypeResolver::TypeResolver(TypeContext& TyContext, const Type* ResolvedType) : TyContext(TyContext), ResolvedType(ResolvedType) {

}

void TypeResolver::visit(const UnresolvedType& Ty) {
    returnValue(ResolvedType);
}

void TypeResolver::visit(const PointerType& Ty) {
    const auto Res = doVisit(*Ty.getElementType());
    returnValue(TyContext.getPointerType(Res));
}

void TypeResolver::visit(const ArrayType& Ty) {
    const auto Res = doVisit(*Ty.getElementType());
    returnValue(TyContext.getArrayType(Res, Ty.getSize()));
}
