#pragma once
#include "Type.h"

class FloatingPointType final : public PrimitiveType {
public:
    FloatingPointType(const std::string& Name, std::uint8_t Width) : PrimitiveType(Name), Width(Width) {}
private:
    std::uint8_t Width;
};

inline FloatingPointType F32Type("f32", 32);
inline FloatingPointType F64Type("f64", 64);