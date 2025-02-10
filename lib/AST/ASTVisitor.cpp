#include "ASTVisitor.h"

void AstConstVisitor::visit(const LiteralExpr& Node) {
}

void AstConstVisitor::visit(const UnaryOpExpr& Node) {
    Node.getValue().accept(*this);
}

void AstConstVisitor::visit(const BinaryOpExpr& Node) {
    Node.getLeft().accept(*this);
    Node.getRight().accept(*this);
}

void AstConstVisitor::visit(const FunctionCallExpr& Node) {
    Node.getFunction().accept(*this);
    for (auto &Arg : Node.getArgs()) {
        Arg->accept(*this);
    }
}

void AstConstVisitor::visit(const NamedExpr& Node) {
}

void AstConstVisitor::visit(const DotExpr& Node) {
    Node.getExpr().accept(*this);
}

void AstConstVisitor::visit(const CastExpr& Node) {
    Node.getValue().accept(*this);
}

void AstConstVisitor::visit(const SubscriptExpr& Node) {
    Node.getExpr().accept(*this);
    Node.getSubscript().accept(*this);
}

void AstConstVisitor::visit(const CompoundExpr& Node) {
    for (const auto& Expr : Node.getExprs()) {
        Expr->accept(*this);
    }
}

void AstConstVisitor::visit(const IfStmt& Node) {
    Node.getCondition().accept(*this);
    Node.getTrueBlock()->accept(*this);
    if (const auto FalseBlock = Node.getFalseBlock()) {
        FalseBlock->accept(*this);
    }
}

void AstConstVisitor::visit(const WhileStmt& Node) {
    Node.getCondition().accept(*this);
    Node.getBody().accept(*this);
}

void AstConstVisitor::visit(const CompoundStmt& Node) {
    for (auto &Stmt : Node.getBody()) {
        Stmt->accept(*this);
    }
}

void AstConstVisitor::visit(const ReturnStmt& Node) {
    if (const auto Value = Node.getValue()) {
        Value->accept(*this);
    }
}

void AstConstVisitor::visit(const LetStmt& Node) {
    if (const auto Value = Node.getValue()) {
        Value->accept(*this);
    }
}

void AstConstVisitor::visit(const ExpressionStmt& Node) {
    Node.getExpr().accept(*this);
}

void AstConstVisitor::visit(const AssignStmt& Node) {
    Node.getLeft().accept(*this);
    Node.getRight().accept(*this);
}

void AstConstVisitor::visit(const FunctionDecl& Node) {
    Node.getBody().accept(*this);
}

void AstConstVisitor::visit(const StructDecl& Node) {
}

void AstConstVisitor::visit(const Module& Node) {
    for (auto &Decl : Node.getDeclarations()) {
        Decl->accept(*this);
    }
}

void AstVisitor::visit(LiteralExpr& Node) {

}

void AstVisitor::visit(UnaryOpExpr& Node) {
    Node.getValue().accept(*this);
}

void AstVisitor::visit(BinaryOpExpr& Node) {
    Node.getLeft().accept(*this);
    Node.getRight().accept(*this);
}

void AstVisitor::visit(FunctionCallExpr& Node) {
    Node.getFunction().accept(*this);
    for (auto& Arg : Node.getArgs()) {
        Arg->accept(*this);
    }
}

void AstVisitor::visit(NamedExpr& Node) {
}

void AstVisitor::visit(DotExpr& Node) {
    Node.getExpr().accept(*this);
}

void AstVisitor::visit(CastExpr& Node) {
    Node.getValue().accept(*this);
}

void AstVisitor::visit(SubscriptExpr& Node) {
    Node.getExpr().accept(*this);
    Node.getSubscript().accept(*this);
}

void AstVisitor::visit(CompoundExpr& Node) {
    for (const auto& Expr : Node.getExprs()) {
        Expr->accept(*this);
    }
}

void AstVisitor::visit(IfStmt& Node) {
    Node.getCondition().accept(*this);
    Node.getTrueBlock()->accept(*this);
    if (const auto FalseBlock = Node.getFalseBlock()) {
        FalseBlock->accept(*this);
    }
}

void AstVisitor::visit(WhileStmt& Node) {
    Node.getCondition().accept(*this);
    Node.getBody().accept(*this);
}

void AstVisitor::visit(CompoundStmt& Node) {
    for (auto& Stmt : Node.getBody()) {
        Stmt->accept(*this);
    }
}

void AstVisitor::visit(ReturnStmt& Node) {
    if (const auto Value = Node.getValue()) {
        Value->accept(*this);
    }
}

void AstVisitor::visit(LetStmt& Node) {
    if (const auto Value = Node.getValue()) {
        Value->accept(*this);
    }
}

void AstVisitor::visit(ExpressionStmt& Node) {
    Node.getExpr().accept(*this);
}

void AstVisitor::visit(AssignStmt& Node) {
    Node.getLeft().accept(*this);
    Node.getRight().accept(*this);
}

void AstVisitor::visit(FunctionDecl& Node) {
    Node.getBody().accept(*this);
}

void AstVisitor::visit(StructDecl& Node) {
}

void AstVisitor::visit(Module& Node) {
    for (auto& Decl : Node.getDeclarations()) {
        Decl->accept(*this);
    }
}
