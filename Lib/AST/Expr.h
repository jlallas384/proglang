#pragma once
#include "ASTBase.h"
#include <variant>
#include <cstdint>
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

    void accept(AstVisitor& Visitor) override;

private:
    std::variant<
        long long,
        double,
        bool,
        std::string
    > Value;
};

class BinaryOpExpr : public Expression {
public:
    enum class Kind : std::uint8_t {
        Plus, Minus, Star, Slash, Percent,
        Less, LEq, Greater, GEq, Equals, NEquals
    };

    BinaryOpExpr(Kind Kind, AstPtr<Expression> Left, AstPtr<Expression> Right) : Kind(Kind), Left(std::move(Left)),
        Right(std::move(Right)) {
    }

    [[nodiscard]] Kind getKind() const { return Kind; }
    [[nodiscard]] const Expression& getLeft() const { return *Left; }
    [[nodiscard]] const Expression& getRight() const { return *Right; }
    void accept(AstVisitor& Visitor) override;

private:
    Kind Kind;
    AstPtr<Expression> Left, Right;
};

class UnaryOpExpr : public Expression {
public:
    enum class Kind : std::uint8_t {
        Plus, Minus, Tilde
    };

    UnaryOpExpr(Kind Kind, AstPtr<Expression> Value) : Kind(Kind), Value(std::move(Value)) {
    }

    [[nodiscard]] Kind getKind() const { return Kind; }
    [[nodiscard]] const Expression& getValue() const { return *Value; }
    void accept(AstVisitor& Visitor) override;

private:
    Kind Kind;
    AstPtr<Expression> Value;
};

class FunctionCallExpr : public Expression {
public:
    FunctionCallExpr(AstPtr<Expression> Function, std::vector<AstPtr<Expression>> Args) : Function(std::move(Function)),
        Args(std::move(Args)) {
    }

    [[nodiscard]] const Expression& getFunction() const { return *Function; }
    [[nodiscard]] const std::vector<AstPtr<Expression>>& getArgs() const { return Args; }
    void accept(AstVisitor& Visitor) override;

private:
    AstPtr<Expression> Function;
    std::vector<AstPtr<Expression>> Args;
};

class NamedExpr : public Expression {
public:
    explicit NamedExpr(std::string Name) : Name(std::move(Name)) {
    }

    [[nodiscard]] const std::string& getName() const { return Name; }
    void accept(AstVisitor& Visitor) override;

private:
    std::string Name;
};

class Type;

class CastExpr : public Expression {
public:
    CastExpr(Type* Type, AstPtr<Expression> Value) : CastType(Type), Value(std::move(Value)) {
    }

    void accept(AstVisitor& Visitor) override;
    [[nodiscard]] Type* getType() const { return CastType; }
    [[nodiscard]] const Expression& getValue() const { return *Value; }

private:
    Type* CastType;
    AstPtr<Expression> Value;
};
