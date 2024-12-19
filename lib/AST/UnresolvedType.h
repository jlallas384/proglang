#pragma once
#include <memory>
#include <string>

class UnresolvedType {
public:
    virtual ~UnresolvedType() = default;
    virtual std::string toString() const = 0;
};

class NamedType : public UnresolvedType {
public:
    NamedType(std::string Name) : Name(std::move(Name)) {}
    std::string toString() const override { return Name; }
private:
    std::string Name;
};