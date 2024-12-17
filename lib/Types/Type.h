#pragma once
#include <string>
#include <vector>

class Type {
public:
    virtual ~Type() = default;
    [[nodiscard]] bool isSameType(const Type &Type) const {
        return this == &Type;
    }
    [[nodiscard]] virtual std::string getName() const = 0;
};

class PrimitiveType : public Type {
public:
    PrimitiveType(std::string Name) : Name(std::move(Name)) {}
    std::string getName() const override {
        return Name;
    }
private:
    std::string Name;
};

inline PrimitiveType VoidType("void");
inline PrimitiveType BoolType("bool");


class FunctionType final : public Type {
public:
    FunctionType(const Type* ReturnType, std::vector<const Type*> ParamTypes) : ReturnType(ReturnType), ParamTypes(std::move(ParamTypes)) {}
    std::string getName() const override;
    const Type* getReturnType() const { return ReturnType; }
    const std::vector<const Type*>& getArgTypes() const { return ParamTypes; }
private:
    const Type* ReturnType;
    std::vector<const Type*> ParamTypes;
};