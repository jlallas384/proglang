#include "Validator.h"
#include "AST/TypeContext.h"
#include <set>
#include <string>
#include <ranges>
#include <iostream>

Validator::Validator(TypeContext& TyContext) : TyContext(TyContext) {
}

void Validator::visit(const FunctionDecl& FunctionDecl) {
    std::set<std::string> ParamNames;
    for (auto &[Identifier, _] : FunctionDecl.getParams()) {
        if (ParamNames.contains(Identifier.getName())) {
            std::cout << "multiple param names\n";
            return returnValue(true);
        }
        ParamNames.insert(Identifier.getName());
    }
}

void Validator::visit(const StructDecl& StructDecl) {
    std::set<std::string> FieldNames;
    for (auto &[Identifier, _] : StructDecl.getFields()) {
        if (FieldNames.contains(Identifier.getName())) {
            std::cout << "multiple field names\n";
            return returnValue(true);
        }
        FieldNames.insert(Identifier.getName());
    }

    const auto Iter = std::ranges::find_if(StructTypes, [&StructDecl](auto& StructTy) {
        return StructDecl.getIdentifier().getName() == StructTy->getName();
    });

    if (Iter != StructTypes.end()) {
        return;
    }

    std::map<std::string, const Type*> Fields;
    for (auto &[Identifier, Ty] : StructDecl.getFields()) {
        Fields.emplace(Identifier.getName(), Ty);
    }
    const auto Ty = TyContext.createStructType(StructDecl.getIdentifier().getName(), Fields);
    StructTypes.push_back(Ty);
}

void Validator::visit(const WhileStmt& WhileStmt) {
    LoopDepth++;
    AstVisitor::visit(WhileStmt);
    LoopDepth--;
}
