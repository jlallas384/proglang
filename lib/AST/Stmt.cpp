#include "Stmt.h"
#include "ASTVisitor.h"

void IfStmt::accept(AstVisitor& Visitor) const {
    return Visitor.visit(*this);
}

void LetStmt::accept(AstVisitor& Visitor) const {
    return Visitor.visit(*this);
}

void WhileStmt::accept(AstVisitor& Visitor) const {
    return Visitor.visit(*this);
}

void ExpressionStmt::accept(AstVisitor& Visitor) const {
    return Visitor.visit(*this);
}

void CompoundStmt::accept(AstVisitor& Visitor) const {
    return Visitor.visit(*this);
}

void ReturnStmt::accept(AstVisitor& Visitor) const {
    return Visitor.visit(*this);
}
