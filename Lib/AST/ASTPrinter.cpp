#include "ASTPrinter.h"
#include <iostream>
#include <format>
#include <algorithm>

void ASTPrinter::visit(const BinaryOpExpr& Node) {
    AstScopeGuard Scoper(*this, "BinaryOpExpr");
    printAttrib("Kind", kindToString(Node.getKind()));
    print("\n");
    Node.getLeft().accept(*this);
    Scoper.AtLastChild = true;
    Node.getRight().accept(*this);
}

void ASTPrinter::visit(const CastExpr&) {
}

void ASTPrinter::visit(const CompoundStmt& Node) {
    AstScopeGuard Scoper(*this, "CompoundStmt");
    print("\n");
    std::ranges::for_each(Node.getBody(), [&, Ind = 0](auto& Stmt) {
        if (Ind == Node.getBody().size() - 1) Scoper.AtLastChild = true;
        Stmt->accept(*this);
    });
}

void ASTPrinter::visit(const DotExpr& Node) {
    AstScopeGuard Scoper(*this, "DotExpr");
    printAttrib("Identifier", Node.getIdentifier());
    print("\n");
    Scoper.AtLastChild = true;
    Node.getExpr().accept(*this);
}

void ASTPrinter::visit(const FunctionCallExpr& Node) {
    AstScopeGuard Scoper(*this, "FunctionCallExpr");
    print("\n");
    Node.getFunction().accept(*this);
    std::ranges::for_each(Node.getArgs(), [&, Ind = 0](auto& Arg) {
        if (Ind == Node.getArgs().size() - 1) Scoper.AtLastChild = true;
        Arg->accept(*this);
    });
}

void ASTPrinter::visit(const FunctionDecl& Node) {
    AstScopeGuard Scoper(*this, "FunctionDecl");
    printAttrib("Identifier", Node.getName());
    std::string ParamNames;
    for (const auto &Param : Node.getParamNames()) {
        ParamNames += Param + ",";
    }
    if (ParamNames.size()) {
        ParamNames.pop_back();
    }
    printAttrib("ParamNames", ParamNames);
    print("\n");
    Scoper.AtLastChild = true;
    Node.getBody().accept(*this);
}

void ASTPrinter::visit(const IfStmt& Node) {
    AstScopeGuard Scoper(*this, "IfStmt");
    print("\n");
    Node.getCondition().accept(*this);
    auto FalseBlock = Node.getFalseBlock();
    if (!FalseBlock) {
        Scoper.AtLastChild = true;
    }
    Node.getTrueBlock()->accept(*this);
    if (FalseBlock) {
        Scoper.AtLastChild = true;
        FalseBlock->accept(*this);
    }
}

void ASTPrinter::visit(const LetStmt& Node) {
    AstScopeGuard Scoper(*this, "LetStmt");
    printAttrib("Identifier", Node.getIdentifier());
    auto Value = Node.getValue();
    print("\n");
    if (Value) {
        Scoper.AtLastChild = true;
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
        Scoper.AtLastChild = true;
        Value->accept(*this);
    }
}

void ASTPrinter::visit(const UnaryOpExpr& Node) {
    AstScopeGuard Scoper(*this, "UnaryOpExpr");
    printAttrib("Kind", kindToString(Node.getKind()));
    print("\n");
    Scoper.AtLastChild = true;
    Node.getValue().accept(*this);
}

void ASTPrinter::visit(const WhileStmt& Node) {
    AstScopeGuard Scoper(*this, "WhileStmt");
    print("\n");
    Node.getCondition().accept(*this);
    Scoper.AtLastChild = true;
    Node.getBody().accept(*this);
}

void ASTPrinter::visit(const Module& Node) {
    AstScopeGuard Scoper(*this, "Module");
    print("\n");
    for (const auto& Decl : Node.getDeclarations()) {
        Decl->accept(*this);
    }
}

void ASTPrinter::visit(const ExpressionStmt& Node) {
    AstScopeGuard Scoper(*this, "ExpressionStmt");
    print("\n");
    Scoper.AtLastChild = true;
    Node.getExpr().accept(*this);
}

void ASTPrinter::print(const std::string &Str, bool Indented) const {
    if (Indented) {
        for (int Ind = 0; Ind < Indent - 1; Ind++) {
            std::string Str = "  ";
            if (!Parents[Ind]->AtLastChild) Str = "| ";
            std::cout << Str;
        }
        if (Indent) {
            if (Parents.back()->AtLastChild) std::cout << "`-";
            else std::cout << "|-";
        }
        //std::cout << std::string(Indent * 2, ' ');
    }
    std::cout << Str;
}

ASTPrinter::AstScopeGuard::AstScopeGuard(ASTPrinter& Printer, const std::string &Name) : Printer(Printer) {
    Printer.Indent++;
    Printer.print(Name + ": ", true);
    Printer.Parents.push_back(this);
}

ASTPrinter::AstScopeGuard::~AstScopeGuard() {
    Printer.Parents.pop_back();
    Printer.Indent--;
}
