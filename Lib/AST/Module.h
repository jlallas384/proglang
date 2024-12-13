#pragma once
#include "ASTBase.h"
#include "Decl.h"
#include <vector>

class Module : public AstBase {
public:
    Module(std::vector<AstPtr<Declaration>> Declarations) : Declarations(std::move(Declarations)) {}
    const auto& getDeclarations() const { return Declarations; }
    void accept(AstVisitor& Visitor) const override;
private:
    std::vector<AstPtr<Declaration>> Declarations;
};
