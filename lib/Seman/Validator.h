#pragma once

#include "AST/ASTVisitor.h"
#include "Utils/VisitorBase.h"
/* Checks for:
 * struct redefintion
 * parameters with same name
 * struct member with same name
 * all break and continue are inside a loop
 */

class StructType;
class TypeContext;

class Validator : public VisitorBase<Validator, const AstBase, bool>, public AstVisitor {
public:
    Validator(TypeContext& TyContext);
    void visit(const FunctionDecl&) override;
    void visit(const StructDecl&) override;
    void visit(const WhileStmt&) override;
    auto getStructTypes() { return StructTypes; }
private:
    int LoopDepth = 0;
    TypeContext &TyContext;
    std::vector<const StructType*> StructTypes;
};