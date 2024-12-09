#include "Decl.h"
#include "Visitor.h"

void FunctionDecl::accept(AstVisitor& Visitor) {
    Visitor.visit(*this);
}
