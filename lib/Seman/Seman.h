#pragma once
#include "AST/ASTVisitor.h"
#include "Utils/ErrorReporter.h"
#include <map>

// TODO
// cyclic structs
class Seman : public AstVisitor {
public:
    Seman(TypeContext& TyContext, ErrorReporter& Reporter);
    void visit(Module&) override;

    TypeContext& getTyContext() const { return TyContext; }

    void error(const SourceRange &Loc, const std::string& Message) const {
        Reporter.error(*CurrentSource, Loc, Message);
    }

    const Type* getType(const Nameable& Name) const {
        const auto Iter = NamesResolvedTypes.find(&Name);
        if (Iter == NamesResolvedTypes.end()) {
            return nullptr;
        }
        return Iter->second;
    }

    void setType(const Nameable& Name, const Type* Type) {
        NamesResolvedTypes[&Name] = Type;
    }

private:
    std::map<const IdentifierSymbol*, const Nameable*> ReferencedNames;
    std::map<const Nameable*, const Type*> NamesResolvedTypes;
    SourceFile* CurrentSource = nullptr;
    TypeContext& TyContext;
    ErrorReporter& Reporter;
};
