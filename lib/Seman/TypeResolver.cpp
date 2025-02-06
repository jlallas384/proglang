#include "TypeResolver.h"
#include "AST/TypeContext.h"

TypeResolver::TypeResolver(TypeContext& TyContext, std::map<std::string, const Type*>& ResolvedTypes) : TyContext(TyContext), ResolvedTypes(ResolvedTypes) {

}

std::pair<const Type*, const UnresolvedType*> TypeResolver::resolve(const Type& Ty) {
    FailedToResolve = nullptr;
    return { doVisit(Ty), FailedToResolve };
}

void TypeResolver::visit(const UnresolvedType& Ty) {
    const auto TypeName = Ty.toString();
    const auto Iter = ResolvedTypes.find(TypeName);
    if (Iter == ResolvedTypes.end()) {
        FailedToResolve = &Ty;
    } else {
        returnValue(Iter->second);
    }
}

void TypeResolver::visit(const PointerType& Ty) {
    const auto Res = doVisit(*Ty.getElementType());
    returnValue(TyContext.getPointerType(Res));
}

void TypeResolver::visit(const ArrayType& Ty) {
    const auto Res = doVisit(*Ty.getElementType());
    returnValue(TyContext.getArrayType(Res, Ty.getSize()));
}
