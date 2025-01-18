#pragma once
#include "Symbol.h"
#include <map>

class AstBase;
class Type;
class LetStmt;

struct Info {
    const Type* Ty;
    Symbol SymRef;
};

class SemanInfo {
public:
    template <typename T>
    Info& get(const T& Node) {
        return SMap[&Node];
    }
private:
    std::map<const void*, Info> SMap;
};