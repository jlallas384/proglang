#pragma once
#include "Expr.h"
#include "ASTBase.h"

class AstVisitor;

class Statement : public AstBase {
};

class IfStmt : public Statement {
public:
    IfStmt(AstPtr<Expression> Condition, AstPtr<Statement> TrueBlock, AstPtr<Statement> FalseBlock) :
        Condition(std::move(Condition)),
        TrueBlock(std::move(TrueBlock)), FalseBlock(std::move(FalseBlock)) {
    }

    [[nodiscard]] const Expression& getCondition() const { return *Condition; }
    [[nodiscard]] const Statement& getTrueBlock() const { return *TrueBlock; }
    [[nodiscard]] const Statement& getFalseBlock() const { return *FalseBlock; }
    void accept(AstVisitor& Visitor) override;

private:
    AstPtr<Expression> Condition;
    AstPtr<Statement> TrueBlock, FalseBlock;
};

class WhileStmt : public Statement {
public:
    WhileStmt(AstPtr<Expression> Condition, AstPtr<Statement> Body) : Condition(std::move(Condition)),
                                                                      Body(std::move(Body)) {
    }

    [[nodiscard]] const Expression& getCondition() const { return *Condition; }
    [[nodiscard]] const Statement& getBody() const { return *Body; }
    void accept(AstVisitor& Visitor) override;

private:
    AstPtr<Expression> Condition;
    AstPtr<Statement> Body;
};

class AssignStmt : public Statement {
public:
    AssignStmt(AstPtr<Expression> Left, AstPtr<Expression> Right) : Left(std::move(Left)), Right(std::move(Right)) {
    }

    [[nodiscard]] const Expression& getLeft() const { return *Left; }
    [[nodiscard]] const Expression& getRight() const { return *Right; }
    void accept(AstVisitor& Visitor) override;

private:
    AstPtr<Expression> Left, Right;
};

class CompoundStmt : public Statement {
public:
    explicit CompoundStmt(std::vector<AstPtr<Statement>> Body) : Body(std::move(Body)) {
    }

    [[nodiscard]] const auto& getBody() const { return Body; }
    void accept(AstVisitor& Visitor) override;

private:
    std::vector<AstPtr<Statement>> Body;
};

class ReturnStmt : public Statement {
public:
    explicit ReturnStmt(AstPtr<Expression> Value) : Value(std::move(Value)) {
    }

    [[nodiscard]] const Expression& getValue() const { return *Value; }
    void accept(AstVisitor& Visitor) override;

private:
    AstPtr<Expression> Value;
};