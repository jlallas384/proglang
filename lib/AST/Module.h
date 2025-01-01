#pragma once
#include "ASTBase.h"
#include "Decl.h"
#include "TypeContext.h"
#include <vector>
#include <memory>

class Module : public AstBase {
public:
    Module(std::vector<AstPtr<Declaration>> Declarations, std::shared_ptr<TypeContext> TyContext) :
        Declarations(std::move(Declarations)), TyContext(std::move(TyContext)) {}
    const auto& getDeclarations() const { return Declarations; }
    void accept(AstVisitor& Visitor) const override;
private:
    std::vector<AstPtr<Declaration>> Declarations;
    std::shared_ptr<TypeContext> TyContext;
};
