#pragma once
#include "ASTBase.h"
#include "Decl.h"
#include "Types/TypesManager.h"
#include <vector>

class Module : public AstBase {
public:
    Module(std::vector<AstPtr<Declaration>> Declarations, std::unique_ptr<TypesManager> Types) :
        Declarations(std::move(Declarations)), Types(std::move(Types)) {}
    const auto& getDeclarations() const { return Declarations; }
    void accept(AstVisitor& Visitor) const override;
private:
    std::vector<AstPtr<Declaration>> Declarations;
    std::unique_ptr<TypesManager> Types;
};
