#include "Module.h"
#include "Visitor.h"

void Module::accept(AstVisitor& Visitor) const {
    Visitor.visit(*this);
}
