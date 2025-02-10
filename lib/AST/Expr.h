#pragma once
#include "ASTBase.h"
#include "Type.h"
#include "Utils/Literal.h"
#include "Identifier.h"
#include <vector>

class Expression : public AstBase {
public:
    virtual SourceRange getRange() const = 0;
    SourceLoc getStart() const { return getRange().Start; }
    SourceLoc getEnd() const { return getRange().End; }
};

class LiteralExpr : public Expression {
public:
    template <typename T>
    [[nodiscard]] bool is() const { return std::holds_alternative<T>(Value); }

    template <typename T>
    explicit LiteralExpr(T Value, const SourceRange& Range) : Value(Value), Range(Range) {
    }

    template <typename T>
    T as() const { return std::get<T>(Value); }

    void accept(AstVisitor& Visitor) const override;
    SourceRange getRange() const override { return Range; }

private:
    Literal Value;
    SourceRange Range;
};

class BinaryOpExpr : public Expression {
public:
    BinaryOpExpr(TokenKind Kind, AstPtr<Expression> Left, AstPtr<Expression> Right) : Kind(Kind), Left(std::move(Left)),
        Right(std::move(Right)) {
    }
    void fuck() {}
    [[nodiscard]] TokenKind getKind() const { return Kind; }
    [[nodiscard]] const Expression& getLeft() const { return *Left; }
    [[nodiscard]] const Expression& getRight() const { return *Right; }
    void accept(AstVisitor& Visitor) const override;

    SourceRange getRange() const override {
        return {Left->getStart(), Right->getEnd()};
    }

private:
    TokenKind Kind;
    AstPtr<Expression> Left, Right;
};

class UnaryOpExpr : public Expression {
public:
    UnaryOpExpr(SourceLoc StartLoc, TokenKind Kind, AstPtr<Expression> Value) : StartLoc(StartLoc), Kind(Kind),
        Value(std::move(Value)) {
    }

    [[nodiscard]] TokenKind getKind() const { return Kind; }
    [[nodiscard]] const Expression& getValue() const { return *Value; }
    void accept(AstVisitor& Visitor) const override;

    SourceRange getRange() const override {
        return {StartLoc, Value->getEnd()};
    }

private:
    SourceLoc StartLoc;
    TokenKind Kind;
    AstPtr<Expression> Value;
};

class FunctionCallExpr : public Expression {
public:
    FunctionCallExpr(AstPtr<Expression> Function, std::vector<AstPtr<Expression>> Args, SourceLoc EndLoc) :
        Function(std::move(Function)),
        Args(std::move(Args)), EndLoc(EndLoc) {
    }

    [[nodiscard]] const Expression& getFunction() const { return *Function; }
    [[nodiscard]] const std::vector<AstPtr<Expression>>& getArgs() const { return Args; }
    const Expression& getArg(unsigned Index) const { return *getArgs()[Index]; }
    void accept(AstVisitor& Visitor) const override;

    SourceRange getRange() const override {
        return {Function->getStart(), EndLoc};
    }

private:
    AstPtr<Expression> Function;
    std::vector<AstPtr<Expression>> Args;
    SourceLoc EndLoc;
};

class NamedExpr : public Expression {
public:
    explicit NamedExpr(IdentifierSymbol Identifier) : Identififer(std::move(Identifier)) {
    }

    [[nodiscard]] const auto& getIdentifier() const { return Identififer; }
    void accept(AstVisitor& Visitor) const override;

    SourceRange getRange() const override {
        return Identififer.getRange();
    }

private:
    IdentifierSymbol Identififer;
};

class DotExpr : public Expression {
public:
    DotExpr(AstPtr<Expression> Expr, IdentifierSymbol Identifier) : Expr(std::move(Expr)),
                                                                    Identifier(std::move(Identifier)) {
    }

    const auto& getIdentifier() const { return Identifier; }
    const Expression& getExpr() const { return *Expr; }
    void accept(AstVisitor& Visitor) const override;

    SourceRange getRange() const override {
        return {Expr->getStart(), Identifier.getRange().End};
    }

private:
    AstPtr<Expression> Expr;
    IdentifierSymbol Identifier;
};

class CastExpr : public Expression {
public:
    CastExpr(const TypeInfo& TyInfo, AstPtr<Expression> Value) : TyInfo(TyInfo), Value(std::move(Value)) {
    }

    void accept(AstVisitor& Visitor) const override;
    [[nodiscard]] auto& getTypeInfo() const { return TyInfo; }
    [[nodiscard]] const Expression& getValue() const { return *Value; }

    SourceRange getRange() const override {
        return {Value->getStart(), TyInfo.getRange().End};
    }

private:
    TypeInfo TyInfo;
    AstPtr<Expression> Value;
};

class SubscriptExpr : public Expression {
public:
    SubscriptExpr(AstPtr<Expression> Expr, AstPtr<Expression> Subscript, SourceLoc EndLoc) :
        Expr(std::move(Expr)), Subscript(std::move(Subscript)), EndLoc(EndLoc) {
        
    }

    void accept(AstVisitor& Visitor) const override;
    const auto& getExpr() const { return *Expr; }
    const auto& getSubscript() const { return *Subscript; }
    SourceRange getRange() const override {
        return { Expr->getStart(), EndLoc };
    }

private:
    AstPtr<Expression> Expr, Subscript;
    SourceLoc EndLoc;
};

class CompoundExpr : public Expression {
public:
    CompoundExpr(std::vector<AstPtr<Expression>> Exprs, SourceLoc StartLoc, SourceLoc EndLoc) :
        Exprs(std::move(Exprs)), StartLoc(StartLoc), EndLoc(EndLoc) {}
    void accept(AstVisitor& Visitor) const override;
    const auto& getExprs() const { return Exprs; }
    SourceRange getRange() const override {
        return { StartLoc, EndLoc };
    }
private:
    std::vector<AstPtr<Expression>> Exprs;
    SourceLoc StartLoc, EndLoc;
};