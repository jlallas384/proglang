#pragma once
#include "ASTBase.h"
#include "Decl.h"
#include "TypeContext.h"
#include <vector>
#include <memory>

class SourceFile;

class Module : public AstBase {
public:
    Module(std::vector<AstPtr<Declaration>> Declarations, std::shared_ptr<TypeContext> TyContext, SourceFile& Source) :
        Declarations(std::move(Declarations)), TyContext(std::move(TyContext)), Source(Source) {}
    const auto& getDeclarations() const { return Declarations; }
    SourceFile& getSourceFile() const { return Source; }
    void accept(AstConstVisitor& Visitor) const override;
    void accept(AstVisitor& Visitor) override;
public:
    std::vector<AstPtr<Declaration>> Declarations;
    std::shared_ptr<TypeContext> TyContext;
    SourceFile& Source;
};
