#pragma once
#include "ASTBase.h"
#include "Stmt.h"
#include "Type.h"
#include <string>
#include <vector>

class Declaration : public AstBase {
};


class FunctionDecl final : public Declaration {
public:
    struct Param {
        IdentifierSymbol Identifier;
        const Type* ParamType;
    };
    FunctionDecl(IdentifierSymbol Identifier, const Type* RetType, std::vector<Param> Params, AstPtr<Statement> Body) :
        Identifier(std::move(Identifier)), RetType(RetType), Params(std::move(Params)), Body(std::move(Body)) {}
    [[nodiscard]] const Statement& getBody() const {
        return *Body;
    }
    const auto& getIdentifier() const { return Identifier; }
    const Type& getRetType() const { return *RetType; }
    const auto& getParams() const { return Params; }
    void accept(AstVisitor& Visitor) const override;
private:
    IdentifierSymbol Identifier;
    const Type* RetType;
    std::vector<Param> Params;
    AstPtr<Statement> Body;
};


class StructDecl final : public Declaration {
public:
    struct Field {
        IdentifierSymbol Identifier;
        const Type* FieldType;
    };
    StructDecl(IdentifierSymbol Identifier, std::vector<Field> Fields) : Identifier(std::move(Identifier)), Fields(std::move(Fields)) {}
    const auto& getIdentifier() const { return Identifier; }
    auto& getFields() const { return Fields; }
    void accept(AstVisitor& Visitor) const override;
private:
    IdentifierSymbol Identifier;
    std::vector<Field> Fields;
};