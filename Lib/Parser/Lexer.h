#pragma once
#include "Token.h"
#include "Utils/SourceFile.h"
#include <string>

class Lexer {
public:
    explicit Lexer(SourceFile Source) : Source(std::move(Source)) {}
    Token nextToken();
private:
    [[nodiscard]] Token makeToken(TokenKind Kind);
    [[nodiscard]] Token readIdentifierOrKeyword();
    [[nodiscard]] Token readInteger();
    [[nodiscard]] bool isEof() const { return At >= Source.length(); }
    std::string Buffer;
    SourceFile Source;
    size_t LineNum = 0, Column = 0, At = 0;
};