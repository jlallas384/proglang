#pragma once
#include "AST/TypeVisitor.h"
#include <map>

class Seman;

class TypeValidator : public TypeVisitor {
public:
    TypeValidator(Seman& SemanInfo) : SemanInfo(SemanInfo) {}
    void visit(const StructType& Ty) override;
    void visit(const UnresolvedType& Ty) override;
    void visit(const ArrayType& Ty) override;
    void validate(const Type& Type) {
        if (!isVisited(Type)) {
            Type.accept(*this);
        }
    }
private:
    bool isVisiting(const Type& Type) const {
        return IsVisited.contains(&Type) && !IsVisited.at(&Type);
    }
    bool isVisited(const Type& Type) const {
        return IsVisited.contains(&Type) && IsVisited.at(&Type);
    }
    void setVisiting(const Type& Type) { IsVisited[&Type] = false; }
    void setVisited(const Type& Type) { IsVisited[&Type] = true; }
    std::map<const Type*, bool> IsVisited;
    Seman& SemanInfo;
};