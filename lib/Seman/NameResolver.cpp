#include "NameResolver.h"
#include "Seman.h"
#include "TypeResolver.h"
#include <iostream>
#include <cassert>
#include <format>

NameResolver::NameResolver(Seman& SemanInfo, const std::vector<const StructType*>& StructTypes) : SemanInfo(SemanInfo) {
    for (const auto Ty : SemanInfo.getTyContext().getBuiltinTypes()) {
        Types[Ty->toString()] = Ty;
    }
    for (const auto Ty : StructTypes) {
        Types[Ty->toString()] = Ty;
    }
}

void NameResolver::visit(const Module& Module) {
    ScopeGuard Guard(CurrentScope);
    AstVisitor::visit(Module);
}

void NameResolver::visit(const FunctionDecl& FunctionDecl) { //TODO take all function first in an initial pass
    const auto& Name = FunctionDecl.getIdentifier().getName();
    if (!CurrentScope->find(Name)) {
        CurrentScope->insert(Name, &FunctionDecl);
    } else {
        // TODO redefinition error
    }


    ScopeGuard Guard(CurrentScope);

    std::vector<const Type*> ParamTypes;
    TypeResolver TyResolver(SemanInfo.getTyContext(), Types);

    for (const auto& Param : FunctionDecl.getParams()) {
        CurrentScope->insert(Param.Identifier.getName(), &Param);

        auto ResolvedType = TyResolver.doVisit(*Param.ParamType);
        assert(ResolvedType);
        SemanInfo.get(Param).Ty = ResolvedType;
        ParamTypes.push_back(ResolvedType);
    }

    const auto RetType = TyResolver.doVisit(FunctionDecl.getRetType());
    const auto FunctionTy = SemanInfo.getTyContext().getFunctionType(RetType, ParamTypes);
    SemanInfo.get(FunctionDecl).Ty = FunctionTy;

    FunctionDecl.getBody().accept(*this);
}

void NameResolver::visit(const LetStmt& Node) {
    if (CurrentScope->find(Node.getIdentifier())) {
        std::cout << "multiple symbol" << '\n';
    }
    CurrentScope->insert(Node.getIdentifier(), &Node);

    TypeResolver TyResolver(SemanInfo.getTyContext(), Types);
    const auto ResolvedType = TyResolver.doVisit(*Node.getType());
    SemanInfo.get(Node).Ty = ResolvedType;

    Node.getValue()->accept(*this);
}

void NameResolver::visit(const CompoundStmt& CompoundStmt) {
    ScopeGuard Guard(CurrentScope);
    AstVisitor::visit(CompoundStmt);
}

void NameResolver::visit(const NamedExpr& NamedExpr) {
    const auto& Name = NamedExpr.getIdentifier().getName();
    const auto Sym = CurrentScope->find(Name);
    if (Sym == nullptr) {
        auto Msg = std::format("Symbol '{}' not found", Name);
        SemanInfo.error(NamedExpr.getIdentifier().getRange(), Msg);
    } else {
        SemanInfo.get(NamedExpr).SymRef = *Sym;
    }
}
