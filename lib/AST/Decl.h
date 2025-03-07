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
    [[nodiscard]] Statement& getBody() const {
        return *Body;
    }
    const Type& getRetType() const { return *RetType; }
    auto& getParams() const { return Params; }
    void accept(AstConstVisitor& Visitor) const override;
    void accept(AstVisitor& Visitor) override;
private:
    const Type* RetType;
    std::vector<Param> Params;
    AstPtr<Statement> Body;
};

struct StructDeclField : Nameable {
    StructDeclField(IdentifierSymbol Identifier, const Type* FieldType) : Nameable(std::move(Identifier)), FieldType(FieldType) {}
    const Type* FieldType;
};

class StructDecl final : public Declaration, public Nameable {
public:

    StructDecl(IdentifierSymbol Identifier, std::vector<StructDeclField> Fields) : Nameable(std::move(Identifier)), Fields(std::move(Fields)) {}
    auto& getFields() const { return Fields; }
    void accept(AstConstVisitor& Visitor) const override;
    void accept(AstVisitor& Visitor) override;
private:
    std::vector<StructDeclField> Fields;
};