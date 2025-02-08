#include "Lexer.h"
#include "Token.h"
#include <string>
#include <ranges>
#include <cctype>
#include <utility>
#include <cassert>

namespace {
    constexpr auto kindRange(TokenKind Front, TokenKind Back) {
        return std::views::iota(std::to_underlying(Front), std::to_underlying(Back) + 1)
            | std::views::transform([](auto E) { return static_cast<TokenKind>(E); });
    };
    constexpr auto Keywords = kindRange(TokenKind::While, TokenKind::Let);
}

Token Lexer::nextToken() {
    Size = 0;
    auto Kind = TokenKind::Eof;
    switch (nextChar()) {
        case '\0':
            break;
        case '\r':
            return nextToken();
        case '\n': {
            LineNum++;
            Column = 0;
            return nextToken();
        }
        case ' ':
        case '\t': {
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
        case '{':
            Kind = TokenKind::LeftBrace;
            break;
        case '}':
            Kind = TokenKind::RightBrace;
            break;
        case '[':
            Kind = TokenKind::LeftSqrBrace;
            break;
        case ']':
            Kind = TokenKind::RightSqrBrace;
            break;
        case '&':
            Kind = TokenKind::Amp;
            if (tryConsume('&')) {
                Kind = TokenKind::AmpAmp;
            }
            break;
        case '|':
            Kind = TokenKind::Pipe;
            if (tryConsume('|')) {
                Kind = TokenKind::PipePipe;
            }
            break;
        case '<':
            Kind = TokenKind::Less;
            if (tryConsume('<')) {
                Kind = TokenKind::LessLess;
            } else if (tryConsume('=')) {
                Kind = TokenKind::LessEqual;
            }
            break;
        case '>':
            Kind = TokenKind::Greater;
            if (tryConsume('>')) {
                return makeToken(TokenKind::GreaterGreater);
            }
            if (tryConsume('=')) {
                return makeToken(TokenKind::GreaterEqual);
            }
            break;
        case ';':
            Kind = TokenKind::Semicolon;
            break;
        case ':':
            Kind = TokenKind::Colon;
            break;
        case '=':
            Kind = TokenKind::Equal;
            if (tryConsume('=')) {
                Kind = TokenKind::EqualEqual;
            }
            break;
        case '!':
            Kind = TokenKind::ExclMark;
            if (tryConsume('=')) {
                Kind = TokenKind::NotEqual;
            }
            break;
        case '.':
            Kind = TokenKind::Dot;
            break;
        case '+':
            Kind = TokenKind::Plus;
            break;
        case '-':
            Kind = TokenKind::Minus;
            break;
        case '*':
            Kind = TokenKind::Star;
            break;
        case '/':
            Kind = TokenKind::Slash;
            break;
        case '%':
            Kind = TokenKind::Amp;
            break;
        case '^':
            Kind = TokenKind::Caret;
            break;
        case '(':
            Kind = TokenKind::LeftParen;
            break;
        case ')':
            Kind = TokenKind::RightParen;
            break;
        case ',':
            Kind = TokenKind::Comma;
            break;
        default:
            assert(false);
    }
    return makeToken(Kind);
}

bool Lexer::tryConsume(char Chr) {
    if (Source[At] == Chr) {
        Size++;
        At++;
        return true;
    }
    return false;
}

char Lexer::nextChar() {
    Size++;
    if (isEof()) {
        return 0;
    }
    return Source[At++];
}


Token Lexer::makeToken(TokenKind Kind) {
    Token Ret = { Kind, {LineNum, Column}, Source.substr(At - Size, Size)};
    Column += Size;
    return Ret;
}

Token Lexer::readIdentifierOrKeyword() {
    while (!isEof() && (std::isalnum(Source[At]) || Source[At] == '_')) {
        nextChar();
    }
    auto Buffer = Source.substr(At - Size, Size);
    const auto Iter = std::ranges::find_if(Keywords, [&](auto Kind) {
        return Buffer == kindToString(Kind);
    });
    if (Iter != Keywords.end()) {
        return makeToken(*Iter);
    }
    return makeToken(TokenKind::Identifier);
}

Token Lexer::readInteger() {
    while (!isEof() && std::isdigit(Source[At])) {
        nextChar();
    }
    return makeToken(TokenKind::Integer);
}
