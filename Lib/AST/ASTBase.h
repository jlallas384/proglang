#pragma once
#include <memory>

class AstVisitor;

class AstBase {
public:
    AstBase() = default;
    virtual ~AstBase() = default;
    AstBase(const AstBase&) = delete;
    AstBase& operator=(const AstBase&) = delete;
    AstBase(const AstBase&&) = delete;
    AstBase& operator=(const AstBase&&) = delete;
    virtual void accept(AstVisitor& Visitor) = 0;
};

template <typename T>
using AstPtr = std::unique_ptr<T>;