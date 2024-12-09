#include "Lexer.h"
#include "Token.h"
#include <string>
#include <ranges>
#include <cctype>

Token Lexer::nextToken() {
    if (At >= Source.length()) {
        return makeToken(Eof, 0);
    }
    for (const auto& Kind: OperatorDelimiters) {
        const auto Str = Kind.toString();
        size_t Matched = 0;
        while (Matched < Str.size() && Matched + At < Source.length() && Str[Matched] == Source.at(At + Matched)) {
            Matched++;
        }
        if (Matched == Str.size()) {
            At += Str.size();
            Column += Str.size();
            return makeToken(Kind, Str.size());
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
        default:
            return makeToken(Eof, 0);
    }
}

Token Lexer::readIdentifierOrKeyword() {
    std::string Str;
    while (At < Source.length() && (std::isalnum(Source.at(At)) || Source.at(At) == '_')) {
        Str += Source.at(At++);
        Column++;
    }
    const auto Iter = std::ranges::find_if(Keywords, [&Str](const TokenKind& Kind) {
        return Str == Kind.toString();
    });
    if (Iter != Keywords.end()) {
        return makeToken(*Iter, Str.size());
    }
    return makeToken(Identifier, Str.size(), Str);
}
