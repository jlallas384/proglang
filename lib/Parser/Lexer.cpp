#include "Lexer.h"
#include "Token.h"
#include <string>
#include <ranges>
#include <cctype>
#include <utility>

namespace {
    constexpr auto kindRange(TokenKind Front, TokenKind Back) {
        return std::views::iota(std::to_underlying(Front), std::to_underlying(Back) + 1)
            | std::views::transform([](auto E) { return static_cast<TokenKind>(E); });
    };
    constexpr auto Keywords = kindRange(TokenKind::While, TokenKind::Let);
    constexpr auto OperatorDelimiters = kindRange(TokenKind::AmpAmp, TokenKind::Equal);
}

Token Lexer::nextToken() {
    if (isEof()) {
        return makeToken(TokenKind::Eof);
    }
    for (const auto Kind: OperatorDelimiters) {
        const auto Str = kindToString(Kind);
        size_t Matched = 0;
        while (Matched < Str.size() && Matched + At < Source.length() && Str[Matched] == Source.at(At + Matched)) {
            Matched++;
        }
        if (Matched == Str.size()) {
            Buffer = Str;
            At += Str.size();
            return makeToken(Kind);
        }
    }
    switch (Source.at(At)) {
        case '\n': {
            LineNum++;
            Column = 0;
            At++;
            return nextToken();
        }
        case ' ':
        case '\t': {
            At++;
            Column++;
            return nextToken();
        }
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j':
        case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't':
        case 'u': case 'v': case 'w': case 'x': case 'y': case 'z': case 'A': case 'B': case 'C': case 'D':
        case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
        case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
        case 'Y': case 'Z': case '_': {
            return readIdentifierOrKeyword();
        }
        case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
            return readInteger();
        default:
            return makeToken(TokenKind::Eof);
    }
}

Token Lexer::makeToken(TokenKind Kind) {
    Token Ret = { Kind, {LineNum, Column}, Buffer };
    Column += Buffer.size();
    Buffer = "";
    return Ret;
}

Token Lexer::readIdentifierOrKeyword() {
    while (!isEof() && (std::isalnum(Source.at(At)) || Source.at(At) == '_')) {
        Buffer += Source.at(At++);
    }
    const auto Iter = std::ranges::find_if(Keywords, [&](auto Kind) {
        return Buffer == kindToString(Kind);
    });
    if (Iter != Keywords.end()) {
        return makeToken(*Iter);
    }
    return makeToken(TokenKind::Identifier);
}

Token Lexer::readInteger() {
    while (!isEof() && std::isdigit(Source.at(At))) {
        Buffer += Source.at(At++);
    }
    return makeToken(TokenKind::Integer);
}
