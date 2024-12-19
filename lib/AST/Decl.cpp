#include "Decl.h"
#include "Visitor.h"

void FunctionDecl::accept(AstVisitor& Visitor) const {
    Visitor.visit(*this);
}

void StructDecl::accept(AstVisitor& Visitor) const {
    Visitor.visit(*this);
}
