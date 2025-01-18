#pragma once
#include "ASTBase.h"
#include "Parser/Token.h"
#include "Type.h"
#include "Utils/Literal.h"
#include <string>
#include <vector>

class Expression : public AstBase {
};

class LiteralExpr : public Expression {
public:
    template <typename T>
    [[nodiscard]] bool is() const { return std::holds_alternative<T>(Value); }

    template <typename T>
    explicit LiteralExpr(T Value) : Value(Value) {
    }

    template <typename T>
    T as() const { return std::get<T>(Value); }
    void accept(AstVisitor& Visitor) const override;

private:
    Literal Value;
};

class BinaryOpExpr : public Expression {
public:
    BinaryOpExpr(TokenKind Kind, AstPtr<Expression> Left, AstPtr<Expression> Right) : Kind(Kind), Left(std::move(Left)),
        Right(std::move(Right)) {
    }

    [[nodiscard]] TokenKind getKind() const { return Kind; }
    [[nodiscard]] const Expression& getLeft() const { return *Left; }
    [[nodiscard]] const Expression& getRight() const { return *Right; }
    void accept(AstVisitor& Visitor) const override;

private:
    TokenKind Kind;
    AstPtr<Expression> Left, Right;
};

class UnaryOpExpr : public Expression {
public:
    UnaryOpExpr(TokenKind Kind, AstPtr<Expression> Value) : Kind(Kind), Value(std::move(Value)) {
    }

    [[nodiscard]] TokenKind getKind() const { return Kind; }
    [[nodiscard]] const Expression& getValue() const { return *Value; }
    void accept(AstVisitor& Visitor) const override;

private:
    TokenKind Kind;
    AstPtr<Expression> Value;
};

class FunctionCallExpr : public Expression {
public:
    FunctionCallExpr(AstPtr<Expression> Function, std::vector<AstPtr<Expression>> Args) : Function(std::move(Function)),
        Args(std::move(Args)) {
    }

    [[nodiscard]] const Expression& getFunction() const { return *Function; }
    [[nodiscard]] const std::vector<AstPtr<Expression>>& getArgs() const { return Args; }
    void accept(AstVisitor& Visitor) const override;

private:
    AstPtr<Expression> Function;
    std::vector<AstPtr<Expression>> Args;
};

class NamedExpr : public Expression {
public:
    explicit NamedExpr(std::string Name) : Name(std::move(Name)) {
    }

    [[nodiscard]] const std::string& getName() const { return Name; }
    void accept(AstVisitor& Visitor) const override;

private:
    std::string Name;
};

class DotExpr : public Expression {
public:
    DotExpr(AstPtr<Expression> Expr, std::string Identifier) : Expr(std::move(Expr)), Identifier(std::move(Identifier)) {}
    const std::string& getIdentifier() const { return Identifier; }
    const Expression& getExpr() const { return *Expr;  }
    void accept(AstVisitor& Visitor) const override;
private:
    AstPtr<Expression> Expr;
    std::string Identifier;
};

class CastExpr : public Expression {
public:
    CastExpr(const Type* CastType, AstPtr<Expression> Value) : CastType(CastType), Value(std::move(Value)) {
    }

    void accept(AstVisitor& Visitor) const override;
    [[nodiscard]] const Type& getType() const { return *CastType; }
    [[nodiscard]] const Expression& getValue() const { return *Value; }

private:
    const Type* CastType;
    AstPtr<Expression> Value;
};
