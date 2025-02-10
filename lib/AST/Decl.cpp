#include "Decl.h"
#include "ASTVisitor.h"

void FunctionDecl::accept(AstConstVisitor& Visitor) const {
    Visitor.visit(*this);
}

void FunctionDecl::accept(AstVisitor& Visitor) {
    Visitor.visit(*this);
}

void StructDecl::accept(AstConstVisitor& Visitor) const {
    Visitor.visit(*this);
}

void StructDecl::accept(AstVisitor& Visitor) {
    Visitor.visit(*this);
}
