#pragma once
#include "Expr.h"
#include "Stmt.h"
#include "Decl.h"
#include "Module.h"

class AstConstVisitor {
public:
    virtual ~AstConstVisitor() = default;
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

class AstVisitor {
public:
    virtual ~AstVisitor() = default;
    virtual void visit(LiteralExpr&);
    virtual void visit(UnaryOpExpr&);
    virtual void visit(BinaryOpExpr&);
    virtual void visit(FunctionCallExpr&);
    virtual void visit(NamedExpr&);
    virtual void visit(DotExpr&);
    virtual void visit(CastExpr&);
    virtual void visit(SubscriptExpr&);
    virtual void visit(CompoundExpr&);

    virtual void visit(IfStmt&);
    virtual void visit(WhileStmt&);
    virtual void visit(CompoundStmt&);
    virtual void visit(ReturnStmt&);
    virtual void visit(LetStmt&);
    virtual void visit(ExpressionStmt&);
    virtual void visit(AssignStmt&);

    virtual void visit(FunctionDecl&);
    virtual void visit(StructDecl&);
    virtual void visit(Module&);
};
