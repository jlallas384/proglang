#pragma once
#include "AST/ASTVisitor.h"
#include "Symbol.h"
#include "Utils/ErrorReporter.h"

class TypeContext;

struct Info {
    const Type* Ty = nullptr;
    Symbol SymRef;
};

template <typename T>
concept NotPointerType = !std::is_pointer_v<T>;

class Seman : public AstVisitor {
public:
    Seman(TypeContext& TyContext, ErrorReporter& Reporter);
    void visit(const Module&) override;

    TypeContext& getTyContext() const { return TyContext; }

    template <NotPointerType T>
    Info& get(const T& Node) {
        return SMap[&Node];
    }
    template <NotPointerType T>
    bool has(const T& Node) const {
        return SMap.contains(&Node);
    }

    void error(const SourceRange &Loc, const std::string& Message) const {
        Reporter.error(*CurrentSource, Loc, Message);
    }
private:
    std::map<const void*, Info> SMap;
    SourceFile* CurrentSource = nullptr;
    TypeContext& TyContext;
    ErrorReporter& Reporter;
};
