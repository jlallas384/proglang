#pragma once
#include "Expr.h"
#include "ASTBase.h"
#include "Type.h"

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
    [[nodiscard]] const Statement* getTrueBlock() const { return TrueBlock.get(); }
    [[nodiscard]] const Statement* getFalseBlock() const { return FalseBlock.get(); }
    void accept(AstVisitor& Visitor) const override;

private:
    AstPtr<Expression> Condition;
    AstPtr<Statement> TrueBlock, FalseBlock;
};

class LetStmt : public Statement {
public:
    LetStmt(std::string Identifier, const Type* VarType, AstPtr<Expression> Value) :
        Identifier(std::move(Identifier)), VarType(VarType), Value(std::move(Value)) {}
    const std::string& getIdentifier() const { return Identifier; }
    const Type* getType() const { return VarType; }
    const Expression* getValue() const { return Value.get();  }
    void accept(AstVisitor& Visitor) const override;
private:
    std::string Identifier;
    const Type* VarType;
    AstPtr<Expression> Value;
};

class WhileStmt : public Statement {
public:
    WhileStmt(AstPtr<Expression> Condition, AstPtr<Statement> Body) : Condition(std::move(Condition)),
                                                                      Body(std::move(Body)) {
    }

    [[nodiscard]] const Expression& getCondition() const { return *Condition; }
    [[nodiscard]] const Statement& getBody() const { return *Body; }
    void accept(AstVisitor& Visitor) const override;

private:
    AstPtr<Expression> Condition;
    AstPtr<Statement> Body;
};

class ExpressionStmt : public Statement {
public:
    explicit ExpressionStmt(AstPtr<Expression> Expr) : Expr(std::move(Expr)) {}
    const Expression& getExpr() const { return *Expr; }
    void accept(AstVisitor& Visitor) const override;
private:
    AstPtr<Expression> Expr;
};

class CompoundStmt : public Statement {
public:
    explicit CompoundStmt(std::vector<AstPtr<Statement>> Body) : Body(std::move(Body)) {
    }

    [[nodiscard]] const auto& getBody() const { return Body; }
    void accept(AstVisitor& Visitor) const override;

private:
    std::vector<AstPtr<Statement>> Body;
};

class ReturnStmt : public Statement {
public:
    explicit ReturnStmt(AstPtr<Expression> Value) : Value(std::move(Value)) {
    }

    [[nodiscard]] const Expression* getValue() const { return Value.get(); }
    void accept(AstVisitor& Visitor) const override;
private:
    AstPtr<Expression> Value;
};
