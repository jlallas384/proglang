#include "ASTVisitor.h"

void AstVisitor::visit(const LiteralExpr& Node) {
}

void AstVisitor::visit(const UnaryOpExpr& Node) {
    Node.getValue().accept(*this);
}

void AstVisitor::visit(const BinaryOpExpr& Node) {
    Node.getLeft().accept(*this);
    Node.getRight().accept(*this);
}

void AstVisitor::visit(const FunctionCallExpr& Node) {
    Node.getFunction().accept(*this);
    for (auto &Arg : Node.getArgs()) {
        Arg->accept(*this);
    }
}

void AstVisitor::visit(const NamedExpr& Node) {
}

void AstVisitor::visit(const DotExpr& Node) {
    Node.getExpr().accept(*this);
}

void AstVisitor::visit(const CastExpr& Node) {
    Node.getValue().accept(*this);
}

void AstVisitor::visit(const SubscriptExpr& Node) {
    Node.getExpr().accept(*this);
    Node.getSubscript().accept(*this);
}

void AstVisitor::visit(const IfStmt& Node) {
    Node.getCondition().accept(*this);
    Node.getTrueBlock()->accept(*this);
    if (const auto FalseBlock = Node.getFalseBlock()) {
        FalseBlock->accept(*this);
    }
}

void AstVisitor::visit(const WhileStmt& Node) {
    Node.getCondition().accept(*this);
    Node.getBody().accept(*this);
}

void AstVisitor::visit(const CompoundStmt& Node) {
    for (auto &Stmt : Node.getBody()) {
        Stmt->accept(*this);
    }
}

void AstVisitor::visit(const ReturnStmt& Node) {
    if (const auto Value = Node.getValue()) {
        Value->accept(*this);
    }
}

void AstVisitor::visit(const LetStmt& Node) {
    //TODO empty initializer
    Node.getValue()->accept(*this);
}

void AstVisitor::visit(const ExpressionStmt& Node) {
    Node.getExpr().accept(*this);
}

void AstVisitor::visit(const FunctionDecl& Node) {
    Node.getBody().accept(*this);
}

void AstVisitor::visit(const StructDecl& Node) {
}

void AstVisitor::visit(const Module& Node) {
    for (auto &Decl : Node.getDeclarations()) {
        Decl->accept(*this);
    }
}
