#include "Expr.h"
#include "ASTVisitor.h"

void LiteralExpr::accept(AstConstVisitor& Visitor) const {
    Visitor.visit(*this);
}

void LiteralExpr::accept(AstVisitor& Visitor) {
    return Visitor.visit(*this);
}

void BinaryOpExpr::accept(AstConstVisitor& Visitor) const {
    Visitor.visit(*this);
}

void BinaryOpExpr::accept(AstVisitor& Visitor) {
    return Visitor.visit(*this);
}

void UnaryOpExpr::accept(AstConstVisitor& Visitor) const {
    Visitor.visit(*this);
}

void UnaryOpExpr::accept(AstVisitor& Visitor) {
    return Visitor.visit(*this);
}

void FunctionCallExpr::accept(AstConstVisitor& Visitor) const {
    Visitor.visit(*this);
}

void FunctionCallExpr::accept(AstVisitor& Visitor) {
    return Visitor.visit(*this);
}

void NamedExpr::accept(AstConstVisitor& Visitor) const {
    Visitor.visit(*this);
}

void NamedExpr::accept(AstVisitor& Visitor) {
    return Visitor.visit(*this);
}

void DotExpr::accept(AstConstVisitor& Visitor) const {
    Visitor.visit(*this);
}

void DotExpr::accept(AstVisitor& Visitor) {
    return Visitor.visit(*this);
}

void CastExpr::accept(AstConstVisitor& Visitor) const {
    Visitor.visit(*this);
}

void CastExpr::accept(AstVisitor& Visitor) {
    return Visitor.visit(*this);
}

void SubscriptExpr::accept(AstConstVisitor& Visitor) const {
    Visitor.visit(*this);
}

void SubscriptExpr::accept(AstVisitor& Visitor) {
    return Visitor.visit(*this);
}

void CompoundExpr::accept(AstConstVisitor& Visitor) const {
    Visitor.visit(*this);
}

void CompoundExpr::accept(AstVisitor& Visitor) {
    return Visitor.visit(*this);
}
