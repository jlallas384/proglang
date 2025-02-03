#pragma once

#include "Identifier.h"
#include <string>
#include <vector>
#include <map>

class TypeContext;
enum class TokenKind : std::int8_t;

class TypeVisitor;

class Type {
public:
    virtual ~Type() = default;
    Type(const Type&) = delete;
    Type(const Type&&) = delete;
    auto& operator=(const Type&) = delete;
    auto& operator=(const Type&&) = delete;
    [[nodiscard]] virtual std::string toString() const = 0;
    virtual const Type* applyBinaryOp(TokenKind OpKind, const Type* Other) const { return nullptr; }
    virtual const Type* applyUnaryOp(TokenKind OpKind, const Type* Other) const { return nullptr; }
    virtual void accept(TypeVisitor& Visitor) const = 0;
protected:
    Type(TypeContext& Context);
    TypeContext& Context;
};

class PrimitiveType : public Type {
public:
    PrimitiveType(TypeContext& TyContext, std::string Name) : Type(TyContext), Name(std::move(Name)) {}
    std::string toString() const override { return Name; }
    const Type* applyBinaryOp(TokenKind OpKind, const Type* Other) const override;
    void accept(TypeVisitor& Visitor) const override;
private:
    std::string Name;
};

class IntegerType final : public PrimitiveType {
public:
    IntegerType(TypeContext& TyContext, const std::string& Name, std::uint8_t Width, bool Sign) : PrimitiveType(TyContext, Name), Width(Width), Sign(Sign) {}
    std::uint8_t getWidth() const { return Width; }
    bool getSign() const { return Sign; }
    const Type* applyBinaryOp(TokenKind OpKind, const Type* Other) const override;
    const Type* applyUnaryOp(TokenKind OpKind, const Type* Other) const override;
    void accept(TypeVisitor& Visitor) const override;
private:
    std::uint8_t Width;
    bool Sign;
};

class FloatingPointType final : public PrimitiveType {
public:
    FloatingPointType(TypeContext& TyContext, const std::string& Name, std::uint8_t Width) : PrimitiveType(TyContext, Name), Width(Width) {}
    std::uint8_t getWidth() const { return Width; }
    const Type* applyBinaryOp(TokenKind OpKind, const Type* Other) const override;
    const Type* applyUnaryOp(TokenKind OpKind, const Type* Other) const override;
    void accept(TypeVisitor& Visitor) const override;
private:
    std::uint8_t Width;
};

class FunctionType final : public Type {
public:
    FunctionType(TypeContext& TyContext, const Type* ReturnType, std::vector<const Type*> ParamTypes) : Type(TyContext), ReturnType(ReturnType), ParamTypes(std::move(ParamTypes)) {}
    std::string toString() const override;
    const Type* getReturnType() const { return ReturnType; }
    const std::vector<const Type*>& getParamTypes() const { return ParamTypes; }
    void accept(TypeVisitor& Visitor) const override;
private:
    const Type* ReturnType;
    std::vector<const Type*> ParamTypes;
};

class PointerType : public Type {
public:
    PointerType(TypeContext& TyContext, const Type* ElementType) : Type(TyContext), ElementType(ElementType) {}
    std::string toString() const override { return "*" + ElementType->toString(); }
    const Type* getElementType() const { return ElementType; }
    const Type* applyBinaryOp(TokenKind OpKind, const Type* Other) const override;
    const Type* applyUnaryOp(TokenKind OpKind, const Type* Other) const override;
    void accept(TypeVisitor& Visitor) const override;
private:
    const Type* ElementType;
};

class ArrayType final : public PointerType {
public:
    ArrayType(TypeContext& TyContext, const Type* ElementType, std::uint32_t Size) : PointerType(TyContext, ElementType), Size(Size) {}
    std::string toString() const override;
    std::uint32_t getSize() const { return Size; }
    const Type* applyBinaryOp(TokenKind OpKind, const Type* Other) const override { return nullptr; }
    const Type* applyUnaryOp(TokenKind OpKind, const Type* Other) const override { return nullptr; }
    void accept(TypeVisitor& Visitor) const override;
private:
    std::uint32_t Size;
};

class UnresolvedType final : public Type {
public:
    UnresolvedType(TypeContext& TyContext, IdentifierSymbol Identifier) : Type(TyContext), Identifier(std::move(Identifier)) {}
    std::string toString() const override { return Identifier.getName(); }
    void accept(TypeVisitor& Visitor) const override;
private:
    IdentifierSymbol Identifier;
};

class StructType final : public Type {
public:
    StructType(TypeContext &TyContext, std::string Name, std::map<std::string, const Type*> Fields)
        : Type(TyContext), Name(std::move(Name)), Fields(std::move(Fields)) {}
    std::string toString() const override { return Name; }
    const Type* getField(const std::string& FieldName) const;
    std::string getName() const { return Name; }
    const Type* applyUnaryOp(TokenKind OpKind, const Type* Other) const override;
    void accept(TypeVisitor& Visitor) const override;
private:
    std::string Name;
    std::map<std::string, const Type*> Fields;
};