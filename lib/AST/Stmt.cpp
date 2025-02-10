#include "Stmt.h"
#include "ASTVisitor.h"

void IfStmt::accept(AstConstVisitor& Visitor) const {
    return Visitor.visit(*this);
}

void IfStmt::accept(AstVisitor& Visitor) {
    return Visitor.visit(*this);
}

void LetStmt::accept(AstConstVisitor& Visitor) const {
    return Visitor.visit(*this);
}

void LetStmt::accept(AstVisitor& Visitor) {
    return Visitor.visit(*this);
}

void WhileStmt::accept(AstConstVisitor& Visitor) const {
    return Visitor.visit(*this);
}

void WhileStmt::accept(AstVisitor& Visitor) {
    return Visitor.visit(*this);
}

void ExpressionStmt::accept(AstConstVisitor& Visitor) const {
    return Visitor.visit(*this);
}

void ExpressionStmt::accept(AstVisitor& Visitor) {
    return Visitor.visit(*this);
}

void CompoundStmt::accept(AstConstVisitor& Visitor) const {
    return Visitor.visit(*this);
}

void CompoundStmt::accept(AstVisitor& Visitor) {
    return Visitor.visit(*this);
}

void ReturnStmt::accept(AstConstVisitor& Visitor) const {
    return Visitor.visit(*this);
}

void ReturnStmt::accept(AstVisitor& Visitor) {
    return Visitor.visit(*this);
}

void AssignStmt::accept(AstConstVisitor& Visitor) const {
    return Visitor.visit(*this);
}

void AssignStmt::accept(AstVisitor& Visitor) {
    return Visitor.visit(*this);
}
