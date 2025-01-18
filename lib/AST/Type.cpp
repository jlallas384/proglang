#include "Type.h"
#include "TypeContext.h"
#include "TypeVisitor.h"
#include "Parser/Token.h"
#include <format>

Type::Type(TypeContext& Context) : Context(Context) {
}

const Type* PrimitiveType::applyBinaryOp(TokenKind OpKind, const Type* Other) const {
    auto BoolTy = Context.getBoolType();
    if (this == BoolTy) {
        switch (OpKind) {
            case TokenKind::AmpAmp:
            case TokenKind::PipePipe:
                return Other == BoolTy ? BoolTy : nullptr;
            default:
                return nullptr;
        }
    }
    return nullptr;
}

void PrimitiveType::accept(TypeVisitor& Visitor) const {
    Visitor.visit(*this);
}

const Type* IntegerType::applyBinaryOp(TokenKind OpKind, const Type* Other) const {
    if (this == Other) {
        return this;
    }
    if (dynamic_cast<const PointerType*>(Other)) {
        if (OpKind == TokenKind::Plus || OpKind == TokenKind::Minus) {
            return Other;
        }
    }
    return nullptr;
}

const Type* IntegerType::applyUnaryOp(TokenKind OpKind, const Type* Other) const {
    if (OpKind == TokenKind::Amp) {
        return Context.getPointerType(this);
    }
    if (OpKind == TokenKind::ExclMark) {
        return Context.getBoolType();
    }
    return this;
}

void IntegerType::accept(TypeVisitor& Visitor) const {
    Visitor.visit(*this);
}

const Type* FloatingPointType::applyBinaryOp(TokenKind OpKind, const Type* Other) const {
    if (this == Other) {
        return this;
    }
    return nullptr;
}

const Type* FloatingPointType::applyUnaryOp(TokenKind OpKind, const Type* Other) const {
    if (OpKind == TokenKind::Amp) {
        return Context.getPointerType(this);
    }
    return this;
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

const Type* PointerType::applyBinaryOp(TokenKind OpKind, const Type* Other) const {
    if (const auto Ptr = dynamic_cast<const IntegerType*>(Other)) {
        return Ptr->applyBinaryOp(OpKind, Other);
    }
    return nullptr;
}

const Type* PointerType::applyUnaryOp(TokenKind OpKind, const Type* Other) const {
    if (OpKind == TokenKind::Star) {
        return ElementType;
    }
    if (OpKind == TokenKind::Amp) {
        return Context.getPointerType(this);
    }
    return nullptr;
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

const Type* StructType::getField(const std::string& FieldName) const {
    const auto Iter = Fields.find(FieldName);
    return Iter == Fields.end() ? nullptr : Iter->second;
}

const Type* StructType::applyUnaryOp(TokenKind OpKind, const Type* Other) const {
    if (OpKind == TokenKind::Amp) {
        return Context.getPointerType(this);
    }
    return nullptr;
}

void StructType::accept(TypeVisitor& Visitor) const {
    Visitor.visit(*this);
}
