#pragma once
#include "Visitor.h"
#include <string>
#include <format>

class ASTPrinter : public AstVisitor {
public:
    void visit(const AssignStmt&) override;
    void visit(const BinaryOpExpr&) override;
    void visit(const CastExpr&) override;
    void visit(const CompoundStmt&) override;
    void visit(const DotExpr&) override;
    void visit(const FunctionCallExpr&) override;
    void visit(const FunctionDecl&) override;
    void visit(const IfStmt&) override;
    void visit(const LetStmt&) override;
    void visit(const LiteralExpr&) override;
    void visit(const NamedExpr&) override;
    void visit(const ReturnStmt&) override;
    void visit(const UnaryOpExpr&) override;
    void visit(const WhileStmt&) override;
    void visit(const Module&) override;
private:
    struct AstScopeGuard {
        AstScopeGuard(ASTPrinter& Printer, const std::string& Name);
        ~AstScopeGuard();
        ASTPrinter& Printer;
    };
    void print(const std::string& Str, bool Indented = false) const;
    template <typename T>
    void printAttrib(const std::string& Attrib, T Value) const {
        print(std::format("{} = '{}'", Attrib, Value));
    }
    int Indent = -1;

};