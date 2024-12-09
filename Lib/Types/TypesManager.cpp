#include "TypesManager.h"
#include "Integer.h"
#include "FloatingPoint.h"
#include <algorithm>

TypesManager::TypesManager() {
    Types.push_back(&I8Type);
    Types.push_back(&I16Type);
    Types.push_back(&I32Type);
    Types.push_back(&I64Type);
    Types.push_back(&U8Type);
    Types.push_back(&U16Type);
    Types.push_back(&U32Type);
    Types.push_back(&U64Type);
    Types.push_back(&F32Type);
    Types.push_back(&F64Type);
}

const Type* TypesManager::findTypeByName(std::string_view Name) const {
    const auto Iter = std::ranges::find_if(Types, [Name](const auto &Type) {
        return Type->getName() == Name;
    });
    return Iter != Types.end() ? *Iter : nullptr;
}

const FunctionType* TypesManager::getFunctionType(const Type* RetType, const std::vector<const Type*>& ArgTypes) {
    const auto Iter = std::ranges::find_if(FunctionTypes, [&](const auto& Type) {
        return Type->getReturnType() == RetType && Type->getArgTypes() == ArgTypes;
    });
    if (Iter == FunctionTypes.end()) {
        FunctionTypes.push_back(std::make_unique<FunctionType>(RetType, ArgTypes));
        return FunctionTypes.back().get();
    }
    return Iter->get();
}
