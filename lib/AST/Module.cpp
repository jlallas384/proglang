#include "Module.h"
#include "ASTVisitor.h"

void Module::accept(AstConstVisitor& Visitor) const {
    Visitor.visit(*this);
}

void Module::accept(AstVisitor& Visitor) {
    Visitor.visit(*this);
}
