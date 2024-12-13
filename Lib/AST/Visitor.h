#pragma once
#include "Expr.h"
#include "Stmt.h"
#include "Decl.h"
#include "Module.h"

class AstVisitor {
public:
    virtual ~AstVisitor() = default;
    virtual void visit(const LiteralExpr&) = 0;
    virtual void visit(const UnaryOpExpr&) = 0;
    virtual void visit(const BinaryOpExpr&) = 0;
    virtual void visit(const FunctionCallExpr&) = 0;
    virtual void visit(const NamedExpr&) = 0;
    virtual void visit(const DotExpr&) = 0;
    virtual void visit(const CastExpr&) = 0;

    virtual void visit(const IfStmt&) = 0;
    virtual void visit(const WhileStmt&) = 0;
    virtual void visit(const AssignStmt&) = 0;
    virtual void visit(const CompoundStmt&) = 0;
    virtual void visit(const ReturnStmt&) = 0;
    virtual void visit(const LetStmt&) = 0;

    virtual void visit(const FunctionDecl&) = 0;

    virtual void visit(const Module&) = 0;
};
