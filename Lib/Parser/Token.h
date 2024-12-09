#pragma once
#include "Utils/SourceLoc.h"
#include <variant>
#include <string>
#include <string_view>
#include <vector>

class TokenKind {
public:
    explicit TokenKind(const char* Str) : Str(Str), Id(Count++) {
    }

    bool operator==(const TokenKind& Oth) const { return Id == Oth.Id; }

    std::string toString() const {
        return static_cast<std::string>(Str);
    }

private:
    inline static int Count = 0;
    std::string_view Str;
    int Id;
};

inline std::vector<TokenKind> Keywords, OperatorDelimiters;


class Token {
public:
    Token(const TokenKind& Kind, SourceLoc Loc, std::string Value) : Kind(Kind), Loc(Loc), Value(std::move(Value)) {
    }

    template <typename... T>
    [[nodiscard]] bool is(const TokenKind& TargetKind, T... Args) const {
        return Kind == TargetKind || is(Args...);
    }

    [[nodiscard]] bool is(const TokenKind& TargetKind) const {
        return Kind == TargetKind;
    }

    [[nodiscard]] TokenKind getKind() const { return Kind;  }
    [[nodiscard]] SourceLoc getLoc() const { return Loc;  }
    [[nodiscard]] auto getValue() const { return Value; }
private:
    TokenKind Kind;
    SourceLoc Loc;
    std::string Value;
};


#define MAKE_TOKEN_KIND(STR, TYPE) \
    ([](const char* Str, int Type) { \
        TokenKind Kind(Str); \
        if (Type == 0) { \
            Keywords.push_back(Kind); \
        } else if (Type == 1) { \
            OperatorDelimiters.push_back(Kind); \
        } \
        return Kind; \
    }(STR, TYPE))

#define KEYWORD(NAME, STRING) inline TokenKind NAME = MAKE_TOKEN_KIND(STRING, 0);
#define OPERATOR_DELIMITER(NAME, STRING) inline TokenKind NAME = MAKE_TOKEN_KIND(STRING, 1);
#define SPECIAL(NAME, STRING) inline TokenKind NAME = MAKE_TOKEN_KIND(STRING, 2);

KEYWORD(While, "while");
KEYWORD(If, "if");
KEYWORD(Else, "else");
KEYWORD(Function, "fn");
KEYWORD(Import, "import");
KEYWORD(Let, "let");

OPERATOR_DELIMITER(AmpAmp, "&&");
OPERATOR_DELIMITER(PipePipe, "||");
OPERATOR_DELIMITER(EqualEqual, "==");
OPERATOR_DELIMITER(Plus, "+");
OPERATOR_DELIMITER(Minus, "-");
OPERATOR_DELIMITER(Star, "*");
OPERATOR_DELIMITER(Slash, "/");
OPERATOR_DELIMITER(LeftParen, "(");
OPERATOR_DELIMITER(RightParen, ")");
OPERATOR_DELIMITER(LeftBrace, "{");
OPERATOR_DELIMITER(RightBrace, "}");
OPERATOR_DELIMITER(LeftSqrBrace, "[");
OPERATOR_DELIMITER(RightSqrBrace, "]");
OPERATOR_DELIMITER(Semicolon, ";");
OPERATOR_DELIMITER(Comma, ",");
OPERATOR_DELIMITER(Colon, ":");
OPERATOR_DELIMITER(Dot, ".");
OPERATOR_DELIMITER(Caret, "^");
OPERATOR_DELIMITER(Tilee, "~");
OPERATOR_DELIMITER(Equal, "=");


SPECIAL(Identifier, "identififer");
SPECIAL(Integer, "integer");
SPECIAL(FloatingPoint, "");
SPECIAL(Eof, "");

#undef SPECIAL
#undef OPERATOR_DELIMITER
#undef KEYWORD
#undef MAKE_TOKEN_KIND
