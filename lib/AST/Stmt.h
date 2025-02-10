#pragma once
#include "Expr.h"
#include "ASTBase.h"
#include "Type.h"

class AstConstVisitor;

class Statement : public AstBase {
};

class IfStmt : public Statement {
public:
    IfStmt(AstPtr<Expression> Condition, AstPtr<Statement> TrueBlock, AstPtr<Statement> FalseBlock) :
        Condition(std::move(Condition)),
        TrueBlock(std::move(TrueBlock)), FalseBlock(std::move(FalseBlock)) {
    }

    [[nodiscard]] Expression& getCondition() const { return *Condition; }
    [[nodiscard]] Statement* getTrueBlock() const { return TrueBlock.get(); }
    [[nodiscard]] Statement* getFalseBlock() const { return FalseBlock.get(); }
    void accept(AstConstVisitor& Visitor) const override;
    void accept(AstVisitor& Visitor) override;
private:
    AstPtr<Expression> Condition;
    AstPtr<Statement> TrueBlock, FalseBlock;
};

class LetStmt : public Statement, public Nameable {
public:
    LetStmt(IdentifierSymbol Identifier, const TypeInfo &TyInfo, AstPtr<Expression> Value) :
        Nameable(std::move(Identifier)), TyInfo(TyInfo), Value(std::move(Value)) {}
    const auto& getTypeInfo() const { return TyInfo; }
    Expression* getValue() const { return Value.get();  }
    void accept(AstConstVisitor& Visitor) const override;
    void accept(AstVisitor& Visitor) override;
private:
    TypeInfo TyInfo;
    AstPtr<Expression> Value;
};

class WhileStmt : public Statement {
public:
    WhileStmt(AstPtr<Expression> Condition, AstPtr<Statement> Body) : Condition(std::move(Condition)),
                                                                      Body(std::move(Body)) {
    }

    [[nodiscard]] Expression& getCondition() const { return *Condition; }
    [[nodiscard]] Statement& getBody() const { return *Body; }
    void accept(AstConstVisitor& Visitor) const override;
    void accept(AstVisitor& Visitor) override;
private:
    AstPtr<Expression> Condition;
    AstPtr<Statement> Body;
};

class ExpressionStmt : public Statement {
public:
    explicit ExpressionStmt(AstPtr<Expression> Expr) : Expr(std::move(Expr)) {}
    Expression& getExpr() const { return *Expr; }
    void accept(AstConstVisitor& Visitor) const override;
    void accept(AstVisitor& Visitor) override;
private:
    AstPtr<Expression> Expr;
};

class CompoundStmt : public Statement {
public:
    explicit CompoundStmt(std::vector<AstPtr<Statement>> Body) : Body(std::move(Body)) {
    }

    [[nodiscard]] auto& getBody() const { return Body; }
    void accept(AstConstVisitor& Visitor) const override;
    void accept(AstVisitor& Visitor) override;
private:
    std::vector<AstPtr<Statement>> Body;
};

class ReturnStmt : public Statement {
public:
    explicit ReturnStmt(AstPtr<Expression> Value) : Value(std::move(Value)) {
    }

    [[nodiscard]] Expression* getValue() const { return Value.get(); }
    void accept(AstConstVisitor& Visitor) const override;
    void accept(AstVisitor& Visitor) override;
private:
    AstPtr<Expression> Value;
};

class AssignStmt : public Statement {
public:
    AssignStmt(AstPtr<Expression> Left, AstPtr<Expression> Right) : Left(std::move(Left)), Right(std::move(Right)) {}
    auto& getLeft() const { return *Left; }
    auto& getRight() const { return *Right; }
    void accept(AstConstVisitor& Visitor) const override;
    void accept(AstVisitor& Visitor) override;
private:
    AstPtr<Expression> Left, Right;
};