#include "ASTPrinter.h"
#include <iostream>
#include <format>
#include <ranges>
#include <string_view>

namespace {
    auto dropLast(auto&& Cont) {
        return Cont | std::views::take(Cont.size() - 1);
    }
    void printNodeInfo() {
        std::cout << '\n';
    }
    template <typename T, typename... Ts>
    void printNodeInfo(std::string_view Key, const T& Value, Ts&&... Args) {
        std::cout << std::format("{} = '{}'", Key, Value);
        if constexpr (sizeof...(Args) > 0) {
            std::cout << " ";
            printNodeInfo(std::forward<Ts>(Args)...);
        } else {
            std::cout << '\n';
        }
    }
}

void AstPrinter::visit(const BinaryOpExpr& Node) {
    AstScopeGuard Scoper(*this, "BinaryOpExpr");
    printNodeInfo("Kind", kindToString(Node.getKind()));
    Node.getLeft().accept(*this);
    Scoper.AtLastChild = true;
    Node.getRight().accept(*this);
}

void AstPrinter::visit(const CastExpr& Node) {
    AstScopeGuard Scoper(*this, "CastExpr");
    auto& CastType = Node.getType();
    printNodeInfo("Type", CastType.toString());
    Scoper.AtLastChild = true;
    Node.getValue().accept(*this);
}

void AstPrinter::visit(const CompoundStmt& Node) {
    AstScopeGuard Scoper(*this, "CompoundStmt");
    printNodeInfo();
    auto &Body = Node.getBody();
    if (!Body.empty()) {
        for (auto& Stmt : dropLast(Body)) {
            Stmt->accept(*this);
        }
        Scoper.AtLastChild = true;
        Body.back()->accept(*this);
    }
}

void AstPrinter::visit(const DotExpr& Node) {
    AstScopeGuard Scoper(*this, "DotExpr");
    printNodeInfo("Identifier", Node.getIdentifier());
    Scoper.AtLastChild = true;
    Node.getExpr().accept(*this);
}

void AstPrinter::visit(const FunctionCallExpr& Node) {
    AstScopeGuard Scoper(*this, "FunctionCallExpr");
    printNodeInfo();
    Node.getFunction().accept(*this);
    auto& Args = Node.getArgs();
    if (!Args.empty()) {
        for (auto& Arg : dropLast(Args)) {
            Arg->accept(*this);
        }
        Scoper.AtLastChild = true;
        Args.back()->accept(*this);
    }
}

void AstPrinter::visit(const FunctionDecl& Node) {
    AstScopeGuard Scoper(*this, "FunctionDecl");
    std::string ParamNames;
    for (const auto &Param : Node.getParams()) {
        ParamNames += Param.Name + ",";
    }
    if (!ParamNames.empty()) {
        ParamNames.pop_back();
    }
    printNodeInfo("Identifier", Node.getName(), "ParamNames", ParamNames);
    Scoper.AtLastChild = true;
    Node.getBody().accept(*this);
}

void AstPrinter::visit(const IfStmt& Node) {
    AstScopeGuard Scoper(*this, "IfStmt");
    printNodeInfo();
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

void AstPrinter::visit(const LetStmt& Node) {
    AstScopeGuard Scoper(*this, "LetStmt");
    printNodeInfo("Identifier", Node.getIdentifier());
    auto Value = Node.getValue();
    if (Value) {
        Scoper.AtLastChild = true;
        Value->accept(*this);
    }
}

void AstPrinter::visit(const LiteralExpr& Node) {
    AstScopeGuard Scoper(*this, "LiteralExpr");
    printNodeInfo("Value", Node.as<long long>());
}

void AstPrinter::visit(const NamedExpr& Node) {
    AstScopeGuard Scoper(*this, "NamedExpr");
    printNodeInfo("Name", Node.getName());
}

void AstPrinter::visit(const ReturnStmt& Node) {
    AstScopeGuard Scoper(*this, "ReturnStmt");
    printNodeInfo();
    auto Value = Node.getValue();
    if (Value) {
        Scoper.AtLastChild = true;
        Value->accept(*this);
    }
}

void AstPrinter::visit(const UnaryOpExpr& Node) {
    AstScopeGuard Scoper(*this, "UnaryOpExpr");
    printNodeInfo("Kind", kindToString(Node.getKind()));
    Scoper.AtLastChild = true;
    Node.getValue().accept(*this);
}

void AstPrinter::visit(const WhileStmt& Node) {
    AstScopeGuard Scoper(*this, "WhileStmt");
    printNodeInfo();
    Node.getCondition().accept(*this);
    Scoper.AtLastChild = true;
    Node.getBody().accept(*this);
}

void AstPrinter::visit(const Module& Node) {
    AstScopeGuard Scoper(*this, "Module");
    printNodeInfo();
    auto& Decls = Node.getDeclarations();
    if (!Decls.empty()) {
        for (auto &Decl : dropLast(Decls)) {
            Decl->accept(*this);
        }
        Scoper.AtLastChild = true;
        Decls.back()->accept(*this);
    }
}

void AstPrinter::visit(const ExpressionStmt& Node) {
    AstScopeGuard Scoper(*this, "ExpressionStmt");
    printNodeInfo();
    Scoper.AtLastChild = true;
    Node.getExpr().accept(*this);
}

void AstPrinter::visit(const StructDecl& Node) {
    AstScopeGuard Scoper(*this, "StructDecl");
    std::string Fields;
    for (const auto &Field : Node.getFields()) {
        Fields += Field.Name + ": " + Field.FieldType->toString() + ",";
    }
    printNodeInfo("Name", Node.getName(), "Fields", Fields);
}

AstPrinter::AstScopeGuard::AstScopeGuard(AstPrinter& Printer, std::string_view Name) : Printer(Printer) {
    Printer.printNodeName(Name);
    Printer.Parents.push_back(this);
}

AstPrinter::AstScopeGuard::~AstScopeGuard() {
    Printer.Parents.pop_back();
}

void AstPrinter::printNodeName(std::string_view Name) const {
    std::string Hierarchy;
    if (!Parents.empty()) {
        for (const auto P : dropLast(Parents)) {
            if (!P->AtLastChild) Hierarchy += "| ";
            else Hierarchy += "  ";
        }
        if (Parents.back()->AtLastChild) Hierarchy += "`-";
        else Hierarchy += "|-";
    }
    std::cout << std::format("{}{}: ", Hierarchy, Name);
}
