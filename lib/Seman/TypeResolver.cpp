#include "TypeResolver.h"
#include "AST/TypeContext.h"
#include <iostream>
#include <format>

TypeResolver::TypeResolver(TypeContext& TyContext, std::map<std::string, const Type*>& ResolvedTypes) : TyContext(TyContext), ResolvedTypes(ResolvedTypes) {

}

void TypeResolver::visit(const UnresolvedType& Ty) {
    const auto TypeName = Ty.toString();
    const auto Iter = ResolvedTypes.find(TypeName);
    if (Iter == ResolvedTypes.end()) {
        std::cout << std::format("type '{}' not found\n", TypeName);
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
