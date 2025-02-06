#pragma once
#include <string>
#include <cassert>
#include <memory>
#include <map>

template <typename Value>
class Scope {
public:
    Scope(Scope* Parent) : Parent(Parent) {
        
    }
    void insert(const std::string& Name, Value Val) {
        assert(!Values.contains(Name));
        Values[Name] = Val;
    }
    Value* find(const std::string& Name) {
        const auto Iter = Values.find(Name);
        if (Iter != Values.end()) {
            return &Iter->second;
        }
        return Parent == nullptr ? nullptr : Parent->find(Name);
    }
private:
    std::map<std::string, Value> Values;
    Scope* Parent;
};

template <typename Value>
class ScopeGuard {
public:
    explicit ScopeGuard(std::unique_ptr<Scope<Value>>& CurrentScope) : CurrentScope(CurrentScope), Previous(std::move(CurrentScope)) {
        CurrentScope = std::make_unique<Scope<Value>>(Previous.get());
    }
    ScopeGuard(ScopeGuard&) = delete;
    auto& operator=(ScopeGuard&) = delete;
    ScopeGuard(ScopeGuard&&) = delete;
    auto& operator=(ScopeGuard&&) = delete;
    ~ScopeGuard() {
        CurrentScope = std::move(Previous);
    }
private:
    std::unique_ptr<Scope<Value>>& CurrentScope;
    std::unique_ptr<Scope<Value>> Previous;
};