#pragma once
#include "TokenKind.h"
#include "Utils/SourceLoc.h"
#include <string>

class Token {
public:
    Token(TokenKind Kind, SourceLoc Start, std::string_view Value) : Kind(Kind), Range({Start, Value.size()}), Value(Value) {
    }

    template <typename... T>
    [[nodiscard]] bool is(TokenKind TargetKind, T... Args) const {
        return Kind == TargetKind || is(Args...);
    }

    [[nodiscard]] bool is(TokenKind TargetKind) const {
        return Kind == TargetKind;
    }

    [[nodiscard]] TokenKind getKind() const { return Kind; }
    [[nodiscard]] SourceRange getRange() const { return Range; }
    [[nodiscard]] SourceLoc getStart() const { return Range.Start;  }
    [[nodiscard]] SourceLoc getEnd() const { return Range.End; }
    [[nodiscard]] auto getValue() const { return Value; }
private:
    TokenKind Kind;
    SourceRange Range;
    std::string_view Value;
};