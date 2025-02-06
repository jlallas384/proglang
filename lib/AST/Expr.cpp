#include "Expr.h"
#include "ASTVisitor.h"

void LiteralExpr::accept(AstVisitor& Visitor) const {
    Visitor.visit(*this);
}

void BinaryOpExpr::accept(AstVisitor& Visitor) const {
    Visitor.visit(*this);
}

void UnaryOpExpr::accept(AstVisitor& Visitor) const {
    Visitor.visit(*this);
}

void FunctionCallExpr::accept(AstVisitor& Visitor) const {
    Visitor.visit(*this);
}

void NamedExpr::accept(AstVisitor& Visitor) const {
    Visitor.visit(*this);
}

void DotExpr::accept(AstVisitor& Visitor) const {
    Visitor.visit(*this);
}

void CastExpr::accept(AstVisitor& Visitor) const {
    Visitor.visit(*this);
}

void SubscriptExpr::accept(AstVisitor& Visitor) const {
    Visitor.visit(*this);
}
