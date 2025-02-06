#include "TypeValidator.h"
#include "Seman.h"
#include <cassert>

void TypeValidator::visit(const StructType& Ty) {
    setVisiting(Ty);
    const auto& Decl = Ty.getDecl();
    for (auto& Field : Decl.getFields()) {
        const auto FieldTy = SemanInfo.getType(Field);
        if (isVisiting(*FieldTy)) {
            SemanInfo.error(Field.getIdentifier().getRange(), "Cyclic");
        } else {
            validate(*FieldTy);
        }

    }
    setVisited(Ty);
}

void TypeValidator::visit(const UnresolvedType& Ty) {
    assert(false && "Type still unresolved");
}

void TypeValidator::visit(const ArrayType& Ty) {
    setVisiting(Ty);
    const auto ElemTy = Ty.getElementType();
    if (isVisiting(*ElemTy)) {
        // TODO
    } else {
        validate(*ElemTy);
    }
    setVisited(Ty);
}
