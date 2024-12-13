#include "ASTPrinter.h"
#include <iostream>
#include <format>

void ASTPrinter::visit(const AssignStmt&) {
}

void ASTPrinter::visit(const BinaryOpExpr& Node) {
    AstScopeGuard Scoper(*this, "BinaryOpExpr");
    printAttrib("Kind", kindToString(Node.getKind()));
    print("\n");
    Node.getLeft().accept(*this);
    Node.getRight().accept(*this);
}

void ASTPrinter::visit(const CastExpr&) {
}

void ASTPrinter::visit(const CompoundStmt& Node) {
    AstScopeGuard Scoper(*this, "CompoundStmt");
    print("\n");
    for (const auto& Stmt : Node.getBody()) {
        Stmt->accept(*this);
    }
}

void ASTPrinter::visit(const DotExpr& Node) {
    AstScopeGuard Scoper(*this, "DotExpr");
    printAttrib("Identifier", Node.getIdentifier());
    print("\n");
    Node.getExpr().accept(*this);
}

void ASTPrinter::visit(const FunctionCallExpr&) {
}

void ASTPrinter::visit(const FunctionDecl& Node) {
    AstScopeGuard Scoper(*this, "FunctionDecl");
    printAttrib("Identifier", Node.getName());
    print("\n");
    Node.getBody().accept(*this);
}

void ASTPrinter::visit(const IfStmt& Node) {
    AstScopeGuard Scoper(*this, "IfStmt");
    print("\n");
    Node.getCondition().accept(*this);
    Node.getTrueBlock()->accept(*this);
    auto FalseBlock = Node.getFalseBlock();
    if (FalseBlock) {
        FalseBlock->accept(*this);
    }
}

void ASTPrinter::visit(const LetStmt& Node) {
    AstScopeGuard Scoper(*this, "LetStmt");
    printAttrib("Identifier", Node.getIdentifier());
    auto Value = Node.getValue();
    print("\n");
    if (Value) {
        Value->accept(*this);
    }
}

void ASTPrinter::visit(const LiteralExpr& Node) {
    AstScopeGuard Scoper(*this, "LiteralExpr");
    printAttrib("Value", Node.as<long long>());
    print("\n");
}

void ASTPrinter::visit(const NamedExpr& Node) {
    AstScopeGuard Scoper(*this, "NamedExpr");
    printAttrib("Name", Node.getName());
    print("\n");
}

void ASTPrinter::visit(const ReturnStmt& Node) {
    AstScopeGuard Scoper(*this, "ReturnStmt");
    print("\n");
    auto Value = Node.getValue();
    if (Value) {
        Value->accept(*this);
    }
}

void ASTPrinter::visit(const UnaryOpExpr& Node) {
    AstScopeGuard Scoper(*this, "UnaryOpExpr");
    printAttrib("Kind", kindToString(Node.getKind()));
    print("\n");
    Node.getValue().accept(*this);
}

void ASTPrinter::visit(const WhileStmt&) {
}

void ASTPrinter::visit(const Module& Node) {
    AstScopeGuard Scoper(*this, "Module");
    print("\n");
    for (const auto& Decl : Node.getDeclarations()) {
        Decl->accept(*this);
    }
}

void ASTPrinter::print(const std::string &Str, bool Indented) const {
    if (Indented) {
        std::cout << std::string(Indent * 2, ' ');
    }
    std::cout << Str;
}

ASTPrinter::AstScopeGuard::AstScopeGuard(ASTPrinter& Printer, const std::string &Name) : Printer(Printer) {
    Printer.Indent++;
    Printer.print(Name + ": ", true);
}

ASTPrinter::AstScopeGuard::~AstScopeGuard() {
    Printer.Indent--;
}
