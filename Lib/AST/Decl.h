#pragma once
#include "ASTBase.h"
#include "Stmt.h"
#include "Types/Type.h"
#include <string>
#include <vector>

class Declaration : public AstBase {
};

class FunctionDecl : public Declaration {
public:
    FunctionDecl(std::string Name, std::vector<std::string> ParamNames, const FunctionType* FuncType, AstPtr<Statement> Body) :
        Name(std::move(Name)), ParamNames(std::move(ParamNames)), FuncType(FuncType), Body(std::move(Body)) {}
    [[nodiscard]] const Statement& getBody() const {
        return *Body;
    }
    const std::string& getName() const { return Name; }
    void accept(AstVisitor& Visitor) const override;
private:
    std::string Name;
    std::vector<std::string> ParamNames;
    const FunctionType* FuncType;
    AstPtr<Statement> Body;
};