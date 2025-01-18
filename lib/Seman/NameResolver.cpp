#include "NameResolver.h"
#include "SemanInfo.h"
#include <iostream>
#include <cassert>

NameResolver::NameResolver(SemanInfo& Info) : Info(Info) {
    
}

void NameResolver::visit(const Module& Module) {
    ScopeGuard Guard(CurrentScope);
    AstVisitor::visit(Module);
}

void NameResolver::visit(const FunctionDecl& FunctionDecl) { //TODO take all function first in an initial pass
    const auto &Name = FunctionDecl.getName();
    if (!CurrentScope->find(Name)) {
        CurrentScope->insert(Name, FunctionOverloads{});
    }

    const auto Overloads = std::get_if<FunctionOverloads>(CurrentScope->find(Name));
    assert(Overloads);
    Overloads->push_back(&FunctionDecl);

    ScopeGuard Guard(CurrentScope);
    for (const auto& Param : FunctionDecl.getParams()) {
        CurrentScope->insert(Param.Name, &Param);
    }
    FunctionDecl.getBody().accept(*this);
}

void NameResolver::visit(const LetStmt& Node) {
    if (CurrentScope->find(Node.getIdentifier())) {
        std::cout << "multiple symbol" << '\n';
    }
    CurrentScope->insert(Node.getIdentifier(), &Node);
}

void NameResolver::visit(const CompoundStmt& CompoundStmt) {
    ScopeGuard Guard(CurrentScope);
    AstVisitor::visit(CompoundStmt);
}

void NameResolver::visit(const NamedExpr& NamedExpr) {
    const auto &Name = NamedExpr.getName();
    const auto Sym = CurrentScope->find(Name);
    if (Sym == nullptr) {
        std::cout << "symbol not found" << " " << Name << '\n';
    } else {
        Info.get(NamedExpr).SymRef = *Sym;
    }
}
