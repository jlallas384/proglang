#include "NameResolver.h"
#include "Seman.h"
#include "TypeResolver.h"
#include <format>
#include <map>

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
    AstConstVisitor::visit(Module);
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

    for (const auto& Param : FunctionDecl.getParams()) {
        CurrentScope->insert(Param.getName(), &Param);
        if (Param.ParamType == nullptr) {
            const auto Msg = "function parameter requires explicit type annotation";
            SemanInfo.error(Param.getIdentifier().getRange(), Msg);
            continue;
        }
        if (auto ResolvedType = tryResolveType(*Param.ParamType)) {
            SemanInfo.setType(Param, ResolvedType);

            if (ResolvedType->isArrayType()) {
                const auto ElemTy = ResolvedType->as<ArrayType>().getElementType();
                ResolvedType = SemanInfo.getTyContext().getPointerType(ElemTy);
            }

            ParamTypes.push_back(ResolvedType);
        }
    }

    if (const auto ResolvedReturnType = tryResolveType(FunctionDecl.getRetType())) {
        const auto FunctionTy = SemanInfo.getTyContext().getFunctionType(ResolvedReturnType, ParamTypes);
        SemanInfo.setType(FunctionDecl, FunctionTy);
    }

    FunctionDecl.getBody().accept(*this);
}

void NameResolver::visit(const LetStmt& Node) {
    auto& Identifier = Node.getIdentifier();
    auto& Name = Identifier.getName();
    if (CurrentScope->find(Name)) {
        const auto Msg = std::format("'{}' is already defined", Name);
        SemanInfo.error(Identifier.getRange(), Msg);
    }
    CurrentScope->insert(Name, &Node);

    if (const auto ResolvedType = tryResolveType(*Node.getTypeInfo().getType())) {
        SemanInfo.setType(Node, ResolvedType);
    }

    AstConstVisitor::visit(Node);
}

void NameResolver::visit(const CompoundStmt& CompoundStmt) {
    ScopeGuard Guard(CurrentScope);
    AstConstVisitor::visit(CompoundStmt);
}

void NameResolver::visit(const NamedExpr& NamedExpr) {
    const auto& Name = NamedExpr.getIdentifier().getName();
    const auto Sym = CurrentScope->find(Name);
    if (Sym == nullptr) {
        const auto Msg = std::format("Symbol '{}' not found", Name);
        SemanInfo.error(NamedExpr.getIdentifier().getRange(), Msg);
    } else {
        SemanInfo.setReferencedName(NamedExpr.getIdentifier(), *Sym);
    }
}

void NameResolver::visit(const CastExpr& CastExpr) {
    const auto& CastTypeInfo = CastExpr.getTypeInfo();
    const auto CastType = CastTypeInfo.getType();

    TypeResolver TyResolver(SemanInfo.getTyContext(), Types);

    const auto [ResolvedCastType, FailedResolve] = TyResolver.resolve(*CastType);

    if (FailedResolve) {
        SemanInfo.error(CastTypeInfo.getRange(), "fail");
    } else {

    }
    AstConstVisitor::visit(CastExpr);
}

void NameResolver::visit(const StructDecl& StructDecl) {
    for (auto& Field : StructDecl.getFields()) {
        if (const auto ResolvedType = tryResolveType(*Field.FieldType)) {
            SemanInfo.setType(Field, ResolvedType);
        }
    }
}

const Type* NameResolver::tryResolveType(const Type& Ty) {
    TypeResolver TyResolver(SemanInfo.getTyContext(), Types);
    const auto [ResolvedType, FailedResolve] = TyResolver.resolve(Ty);

    if (FailedResolve) {
        const auto Msg = std::format("type '{}' not found", FailedResolve->toString());
        SemanInfo.error(FailedResolve->getIdentifier().getRange(), Msg);
        return nullptr;
    }

    return ResolvedType;
}
