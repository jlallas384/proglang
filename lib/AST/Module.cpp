#include "Module.h"
#include "ASTVisitor.h"

void Module::accept(AstVisitor& Visitor) const {
    Visitor.visit(*this);
}
