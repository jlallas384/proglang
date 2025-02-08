#pragma once

#include "Parser/TokenKind.h"
#include "Identifier.h"
#include <string>
#include <vector>
#include <cstdint>

class TypeContext;

class TypeVisitor;

enum class TypeTag : std::uint8_t {
    Primitive,
    Integer,
    FloatingPoint,
    Function,
    Pointer,
    Array,
    Struct,
    Unresolved
};

class Type {
public:
    virtual ~Type() = default;
    Type(const Type&) = delete;
    Type(const Type&&) = delete;
    auto& operator=(const Type&) = delete;
    auto& operator=(const Type&&) = delete;

    bool isIntegerType() const { return getTag() == TypeTag::Integer; }
    bool isFloatingPointType() const { return getTag() == TypeTag::FloatingPoint; }
    bool isArithmeticType() const { return isIntegerType() || isFloatingPointType(); }
    bool isBoolType() const;
    bool isVoidType() const;
    bool isPointerType() const { return getTag() == TypeTag::Pointer; }
    bool isArrayType() const { return getTag() == TypeTag::Array; }
    bool isFunctionType() const { return getTag() == TypeTag::Function; }
    bool isStructType() const { return getTag() == TypeTag::Struct; }

    template <typename T>
    const T& as() const { return static_cast<const T&>(*this); }

    [[nodiscard]] virtual std::string toString() const = 0;

    virtual TypeTag getTag() const = 0;
    virtual void accept(TypeVisitor& Visitor) const = 0;

protected:
    Type(TypeContext& Context);
    TypeContext& Context;
};

class PrimitiveType : public Type {
public:
    PrimitiveType(TypeContext& TyContext, std::string Name) : Type(TyContext), Name(std::move(Name)) {
    }

    std::string toString() const override { return Name; }
    TypeTag getTag() const override { return TypeTag::Primitive; }

    void accept(TypeVisitor& Visitor) const override;

private:
    std::string Name;
};

class IntegerType final : public PrimitiveType {
public:
    IntegerType(TypeContext& TyContext, const std::string& Name, std::uint8_t Width, bool Sign) :
        PrimitiveType(TyContext, Name), Width(Width), Sign(Sign) {
    }

    std::uint8_t getWidth() const { return Width; }
    bool getSign() const { return Sign; }
    TypeTag getTag() const override { return TypeTag::Integer; }

    void accept(TypeVisitor& Visitor) const override;

private:
    std::uint8_t Width;
    bool Sign;
};

class FloatingPointType final : public PrimitiveType {
public:
    FloatingPointType(TypeContext& TyContext, const std::string& Name, std::uint8_t Width) : PrimitiveType(
        TyContext, Name), Width(Width) {
    }

    std::uint8_t getWidth() const { return Width; }
    TypeTag getTag() const override { return TypeTag::FloatingPoint; }

    void accept(TypeVisitor& Visitor) const override;

private:
    std::uint8_t Width;
};

class FunctionType final : public Type {
public:
    FunctionType(TypeContext& TyContext, const Type* ReturnType, std::vector<const Type*> ParamTypes) : Type(TyContext),
        ReturnType(ReturnType), ParamTypes(std::move(ParamTypes)) {
    }

    std::string toString() const override;
    const Type* getReturnType() const { return ReturnType; }
    TypeTag getTag() const override { return TypeTag::Function; }

    const std::vector<const Type*>& getParamTypes() const { return ParamTypes; }
    void accept(TypeVisitor& Visitor) const override;

private:
    const Type* ReturnType;
    std::vector<const Type*> ParamTypes;
};

class PointerType : public Type {
public:
    PointerType(TypeContext& TyContext, const Type* ElementType) : Type(TyContext), ElementType(ElementType) {
    }

    std::string toString() const override { return "*" + ElementType->toString(); }
    const Type* getElementType() const { return ElementType; }
    TypeTag getTag() const override { return TypeTag::Pointer; }

    void accept(TypeVisitor& Visitor) const override;

private:
    const Type* ElementType;
};

class ArrayType final : public PointerType {
public:
    ArrayType(TypeContext& TyContext, const Type* ElementType, std::uint32_t Size) :
        PointerType(TyContext, ElementType), Size(Size) {
    }

    std::string toString() const override;
    std::uint32_t getSize() const { return Size; }
    TypeTag getTag() const override { return TypeTag::Array; }

    void accept(TypeVisitor& Visitor) const override;

private:
    std::uint32_t Size;
};

class UnresolvedType final : public Type {
public:
    UnresolvedType(TypeContext& TyContext, IdentifierSymbol Identifier) : Type(TyContext),
                                                                          Identifier(std::move(Identifier)) {
    }

    std::string toString() const override { return Identifier.getName(); }
    const auto& getIdentifier() const { return Identifier; }
    TypeTag getTag() const override { return TypeTag::Unresolved; }

    void accept(TypeVisitor& Visitor) const override;

private:
    IdentifierSymbol Identifier;
};

class StructDecl;
struct StructDeclField;

class StructType final : public Type {
public:
    StructType(TypeContext& TyContext, std::string Name, const StructDecl& Decl)
        : Type(TyContext), Name(std::move(Name)), Decl(Decl) {
    }

    std::string toString() const override { return Name; }
    const StructDeclField* getField(const std::string& FieldName) const;
    std::string getName() const { return Name; }
    const auto& getDecl() const { return Decl; }
    TypeTag getTag() const override { return TypeTag::Struct; }

    void accept(TypeVisitor& Visitor) const override;

private:
    std::string Name;
    const StructDecl& Decl;
};

class TypeInfo {
public:
    explicit TypeInfo(const Type* Type = nullptr, const SourceRange& Range = {}) : TypePtr(Type), Range(Range) {
    }

    const Type* getType() const { return TypePtr; }
    const SourceRange& getRange() const { return Range; }

private:
    const Type* TypePtr;
    SourceRange Range;
};