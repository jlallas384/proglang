#include "Validator.h"
#include "Seman.h"
#include "AST/TypeContext.h"
#include <ranges>
#include <set>
#include <string>
#include <format>


Validator::Validator(Seman& SemanInfo, TypeContext& TyContext) : SemanInfo(SemanInfo), TyContext(TyContext) {
}

void Validator::visit(const FunctionDecl& FunctionDecl) {
    std::set<std::string> ParamNames;
    for (auto& Param : FunctionDecl.getParams()) {
        auto& ParamName = Param.getName();
        if (ParamNames.contains(ParamName)) {
            const auto Msg = std::format("parameter name '{}' is already used", ParamName);
            SemanInfo.error(Param.getIdentifier().getRange(), Msg);
            return returnValue(true);
        }
        ParamNames.insert(ParamName);
    }
}

void Validator::visit(const StructDecl& StructDecl) {
    std::set<std::string> FieldNames;
    for (auto& Field : StructDecl.getFields()) {
        auto& FieldName = Field.getName();
        if (FieldNames.contains(FieldName)) {
            const auto Msg = std::format("field name '{}' is already used", FieldName);
            SemanInfo.error(Field.getIdentifier().getRange(), Msg);
            return returnValue(true);
        }
        FieldNames.insert(FieldName);
    }

    const auto Iter = std::ranges::find_if(StructTypes, [&StructDecl](auto& StructTy) {
        return StructDecl.getIdentifier().getName() == StructTy->getName();
    });

    if (Iter != StructTypes.end()) {
        return;
    }

    const auto Ty = TyContext.createStructType(StructDecl.getIdentifier().getName(), StructDecl);
    StructTypes.push_back(Ty);
    SemanInfo.setType(StructDecl, Ty);
}

void Validator::visit(const WhileStmt& WhileStmt) {
    LoopDepth++;
    AstVisitor::visit(WhileStmt);
    LoopDepth--;
}
