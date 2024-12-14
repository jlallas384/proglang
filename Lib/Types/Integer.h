#pragma once
#include "Type.h"
#include <cstdint>

class IntegerType final : public PrimitiveType {
public:
    IntegerType(const std::string& Name, std::uint8_t Width, bool Sign) : PrimitiveType(Name), Width(Width), Sign(Sign) {}
private:
    std::uint8_t Width;
    bool Sign;
};

inline IntegerType I8Type("i8", 8, true);
inline IntegerType I16Type("i16", 16, true);
inline IntegerType I32Type("i32", 32, true);
inline IntegerType I64Type("i64", 64, true);

inline IntegerType U8Type("u8", 8, false);
inline IntegerType U16Type("u16", 16, false);
inline IntegerType U32Type("u32", 32, false);
inline IntegerType U64Type("u64", 64, false);