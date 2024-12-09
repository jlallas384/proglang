#pragma once
#include "Expr.h"
#include "Stmt.h"
#include "Decl.h"

class AstVisitor {
public:
    virtual void visit(LiteralExpr&) = 0;
    virtual void visit(UnaryOpExpr&) = 0;
    virtual void visit(BinaryOpExpr&) = 0;
    virtual void visit(FunctionCallExpr&) = 0;
    virtual void visit(NamedExpr&) = 0;
    virtual void visit(CastExpr&) = 0;

    virtual void visit(IfStmt&) = 0;
    virtual void visit(WhileStmt&) = 0;
    virtual void visit(AssignStmt&) = 0;
    virtual void visit(CompoundStmt&) = 0;
    virtual void visit(ReturnStmt&) = 0;

    virtual void visit(VariableDecl&) = 0;
    virtual void visit(FunctionDecl&) = 0;
};