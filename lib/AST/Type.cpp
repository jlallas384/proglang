#include "Type.h"
#include "TypeContext.h"
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

std::string ArrayType::toString() const {
    return std::format("[{}, {}]", getElementType()->toString(), Size);
}

const Type* StructType::getField(const std::string& Name) const {
    const auto Iter = Fields.find(Name);
    return Iter == Fields.end() ? nullptr : Iter->second;
}

const Type* StructType::applyUnaryOp(TokenKind OpKind, const Type* Other) const {
    if (OpKind == TokenKind::Amp) {
        return Context.getPointerType(this);
    }
    return nullptr;
}
