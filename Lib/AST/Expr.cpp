#include "Expr.h"
#include "Visitor.h"

void LiteralExpr::accept(AstVisitor& Visitor) {
    Visitor.visit(*this);
}

void BinaryOpExpr::accept(AstVisitor& Visitor) {
    Visitor.visit(*this);
}

void UnaryOpExpr::accept(AstVisitor& Visitor) {
    Visitor.visit(*this);
}

void FunctionCallExpr::accept(AstVisitor& Visitor) {
    Visitor.visit(*this);
}

void NamedExpr::accept(AstVisitor& Visitor) {
    Visitor.visit(*this);
}

void CastExpr::accept(AstVisitor& Visitor) {
    Visitor.visit(*this);
}
