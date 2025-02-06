#pragma once
#include "ASTBase.h"
#include "Stmt.h"
#include "Type.h"
#include <vector>

class Declaration : public AstBase {
};


class FunctionDecl final : public Declaration, public Nameable {
public:
    struct Param : Nameable {
        Param(IdentifierSymbol Identifier, const Type* ParamType) : Nameable(std::move(Identifier)), ParamType(ParamType) {}
        const Type* ParamType;
    };
    FunctionDecl(IdentifierSymbol Identifier, const Type* RetType, std::vector<Param> Params, AstPtr<Statement> Body) :
        Nameable(std::move(Identifier)), RetType(RetType), Params(std::move(Params)), Body(std::move(Body)) {}
    [[nodiscard]] const Statement& getBody() const {
        return *Body;
    }
    const Type& getRetType() const { return *RetType; }
    const auto& getParams() const { return Params; }
    void accept(AstVisitor& Visitor) const override;
private:
    const Type* RetType;
    std::vector<Param> Params;
    AstPtr<Statement> Body;
};


class StructDecl final : public Declaration, public Nameable {
public:
    struct Field : Nameable {
        Field(IdentifierSymbol Identifier, const Type* FieldType) : Nameable(std::move(Identifier)), FieldType(FieldType) {}
        const Type* FieldType;
    };
    StructDecl(IdentifierSymbol Identifier, std::vector<Field> Fields) : Nameable(std::move(Identifier)), Fields(std::move(Fields)) {}
    auto& getFields() const { return Fields; }
    void accept(AstVisitor& Visitor) const override;
private:
    std::vector<Field> Fields;
};