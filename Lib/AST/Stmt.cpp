#include "Stmt.h"
#include "Visitor.h"

void IfStmt::accept(AstVisitor& Visitor) {
    return Visitor.visit(*this);
}

void CompoundStmt::accept(AstVisitor& Visitor) {
    return Visitor.visit(*this);
}
