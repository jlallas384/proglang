#include "Type.h"
#include "TypeContext.h"
#include "TypeVisitor.h"
#include "Decl.h"
#include <format>
#include <ranges>

bool Type::isBoolType() const {
    return Context.getBoolType() == this;
}

bool Type::isVoidType() const {
    return Context.getVoidType() == this;
}

Type::Type(TypeContext& Context) : Context(Context) {
}

void PrimitiveType::accept(TypeVisitor& Visitor) const {
    Visitor.visit(*this);
}

void IntegerType::accept(TypeVisitor& Visitor) const {
    Visitor.visit(*this);
}

void FloatingPointType::accept(TypeVisitor& Visitor) const {
    Visitor.visit(*this);
}

std::string FunctionType::toString() const {
    std::string Ret = ReturnType->toString() + "(";
    if (!ParamTypes.empty()) {
        for (const auto Param : ParamTypes) {
            Ret += Param->toString() + ",";
        }
        Ret.pop_back();
    }
    Ret += ")";
    return Ret;
}

void FunctionType::accept(TypeVisitor& Visitor) const {
    Visitor.visit(*this);
}

void PointerType::accept(TypeVisitor& Visitor) const {
    Visitor.visit(*this);
}

std::string ArrayType::toString() const {
    return std::format("[{}, {}]", getElementType()->toString(), Size);
}

void ArrayType::accept(TypeVisitor& Visitor) const {
    Visitor.visit(*this);
}

void UnresolvedType::accept(TypeVisitor& Visitor) const {
    Visitor.visit(*this);
}

const StructDeclField* StructType::getField(const std::string& FieldName) const {
    const auto& Fields = Decl.getFields();
    const auto Iter = std::ranges::find_if(Fields, [&](auto &Field) {
        return Field.getName() == FieldName;
    });
    if (Iter != Fields.end()) {
        return &*Iter;
    }
    return nullptr;
}

void StructType::accept(TypeVisitor& Visitor) const {
    Visitor.visit(*this);
}
