#pragma once
#include "ASTVisitor.h"
#include <string_view>
#include <vector>

class AstPrinter : public AstConstVisitor {
public:
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
    void visit(const ExpressionStmt&) override;
    void visit(const StructDecl&) override;
    void visit(const SubscriptExpr&) override;
    void visit(const AssignStmt&) override;
    void visit(const CompoundExpr&) override;
private:
    struct AstScopeGuard {
        AstScopeGuard(AstPrinter& Printer, std::string_view Name);
        ~AstScopeGuard();
        AstPrinter& Printer;
        bool AtLastChild = false;
    };

    void printNodeName(std::string_view Name) const;
    std::vector<AstScopeGuard*> Parents;
};