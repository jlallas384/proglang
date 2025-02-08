#pragma once
#include "Expr.h"
#include "Stmt.h"
#include "Decl.h"
#include "Module.h"

class AstVisitor {
public:
    virtual ~AstVisitor() = default;
    virtual void visit(const LiteralExpr&);
    virtual void visit(const UnaryOpExpr&);
    virtual void visit(const BinaryOpExpr&);
    virtual void visit(const FunctionCallExpr&);
    virtual void visit(const NamedExpr&);
    virtual void visit(const DotExpr&);
    virtual void visit(const CastExpr&);
    virtual void visit(const SubscriptExpr&);
    virtual void visit(const CompoundExpr&);

    virtual void visit(const IfStmt&);
    virtual void visit(const WhileStmt&);
    virtual void visit(const CompoundStmt&);
    virtual void visit(const ReturnStmt&);
    virtual void visit(const LetStmt&);
    virtual void visit(const ExpressionStmt&);
    virtual void visit(const AssignStmt&);

    virtual void visit(const FunctionDecl&);
    virtual void visit(const StructDecl&);
    virtual void visit(const Module&);
};
