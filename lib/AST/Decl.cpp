#include "Decl.h"
#include "Visitor.h"

void FunctionDecl::accept(AstVisitor& Visitor) const {
    Visitor.visit(*this);
}
