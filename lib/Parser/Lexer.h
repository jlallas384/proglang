#pragma once
#include "Token.h"
#include "Utils/SourceFile.h"
#include <string_view>

class Lexer {
public:
    explicit Lexer(std::string_view Source) : Source(Source) {}
    Token nextToken();
    [[nodiscard]] bool isEof() const { return At >= Source.length(); }
private:
    bool tryConsume(char Chr);
    char nextChar();
    [[nodiscard]] Token makeToken(TokenKind Kind);
    [[nodiscard]] Token readIdentifierOrKeyword();
    [[nodiscard]] Token readInteger();
    size_t Size;
    std::string_view Source;
    size_t LineNum = 1, Column = 0, At = 0;
};