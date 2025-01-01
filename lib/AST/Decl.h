#pragma once
#include "ASTBase.h"
#include "Stmt.h"
#include "Type.h"
#include <string>
#include <vector>

class Declaration : public AstBase {
};


class FunctionDecl : public Declaration {
public:
    struct Param {
        std::string Name;
        const Type* ParamType;
    };
    FunctionDecl(std::string Name, const Type* RetType, std::vector<Param> Params, AstPtr<Statement> Body) :
        Name(std::move(Name)), RetType(RetType), Params(std::move(Params)), Body(std::move(Body)) {}
    [[nodiscard]] const Statement& getBody() const {
        return *Body;
    }
    const std::string& getName() const { return Name; }
    const Type& getRetType() const { return *RetType; }
    const auto& getParams() const { return Params; }
    void accept(AstVisitor& Visitor) const override;
private:
    std::string Name;
    const Type* RetType;
    std::vector<Param> Params;
    AstPtr<Statement> Body;
};


class StructDecl final : public Declaration {
public:
    struct Field {
        std::string Name;
        const Type* FieldType;
    };
    StructDecl(std::string Name, std::vector<Field> Fields) : Name(std::move(Name)), Fields(std::move(Fields)) {}
    const std::string& getName() const { return Name; }
    auto& getFields() const { return Fields; }
    void accept(AstVisitor& Visitor) const override;
private:
    std::string Name;
    std::vector<Field> Fields;
};