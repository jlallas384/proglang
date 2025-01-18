#include "Validator.h"
#include "AST/TypeContext.h"
#include <set>
#include <string>
#include <ranges>

Validator::Validator(TypeContext& TyContext) : TyContext(TyContext) {
}

void Validator::visit(const FunctionDecl& FunctionDecl) {
    std::set<std::string> ParamNames;
    for (auto &[Name, _] : FunctionDecl.getParams()) {
        if (ParamNames.contains(Name)) {
            
        } else {
            ParamNames.insert(Name);
        }
    }
}

void Validator::visit(const StructDecl& StructDecl) {
    std::set<std::string> FieldNames;
    bool Valid = true;
    for (auto &[Name, _] : StructDecl.getFields()) {
        if (FieldNames.contains(Name)) {
            Valid = false;
        } else {
            FieldNames.insert(Name);
        }
    }
    if (!Valid) {
        return;
    }

    auto Iter = std::ranges::find_if(StructTypes, [&StructDecl](auto& StructTy) {
        return StructDecl.getName() == StructTy->getName();
    });

    if (Iter != StructTypes.end()) {
        return;
    }

    std::map<std::string, const Type*> Fields;
    for (auto &[Name, Ty] : StructDecl.getFields()) {
        Fields.emplace(Name, Ty);
    }
    const auto Ty = TyContext.createStructType(StructDecl.getName(), Fields);
    StructTypes.push_back(Ty);
}

void Validator::visit(const WhileStmt& WhileStmt) {
    LoopDepth++;
    AstVisitor::visit(WhileStmt);
    LoopDepth--;
}
