#pragma once
#include "Token.h"
#include "SourceInfo.h"
#include <string>

class Lexer {
public:
    explicit Lexer(SourceInfo Source) : Source(std::move(Source)) {}
    Token nextToken();
private:
    [[nodiscard]] Token makeToken(const TokenKind &Kind, size_t Offset, std::string Value) const {
        return { Kind, {LineNum, Column - Offset}, std::move(Value) };
    }
    [[nodiscard]] Token makeToken(const TokenKind &Kind, size_t Offset) const {
        return makeToken(Kind, Offset, "");
    }
    [[nodiscard]] Token readIdentifierOrKeyword();
    SourceInfo Source;
    size_t LineNum = 0, Column = 0, At = 0;
};