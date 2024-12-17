#pragma once
#include "type.h"
#include <vector>
#include <string_view>
#include <memory>

class TypesManager {
public:
    TypesManager();
    [[nodiscard]] const Type* findTypeByName(std::string_view Name) const;
    const FunctionType* getFunctionType(const Type* RetType, const std::vector<const Type*>& ArgTypes);
private:
    std::vector<Type*> Types;
    std::vector<std::unique_ptr<FunctionType>> FunctionTypes;
};