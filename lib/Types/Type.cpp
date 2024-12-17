#include "Type.h"

std::string FunctionType::getName() const {
    std::string Ret = ReturnType->getName() + "(";
    if (!ParamTypes.empty()) {
        for (const auto Param : ParamTypes) {
            Ret += Param->getName() + ",";
        }
        Ret.pop_back();
    }
    Ret += ")";
    return Ret;
}
