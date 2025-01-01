#pragma once
#include <cstdint>
#include <string>
#include <variant>

struct IntLiteral {
    std::uint64_t Value;
};

struct FloatLiteral {
    double Value;
};

struct BoolLiteral {
    bool Value;
};

class StringLiteral {
    std::string Value;
};

using Literal = std::variant<IntLiteral, FloatLiteral, BoolLiteral, StringLiteral>;