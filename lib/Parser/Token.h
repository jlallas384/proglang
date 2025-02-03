#pragma once
#include "Utils/SourceLoc.h"
#include <cstdint>
#include <string>

enum class TokenKind : std::int8_t;

class Token {
public:
    Token(TokenKind Kind, SourceLoc Start, std::string Value) : Kind(Kind), Range({Start, Value.size()}), Value(std::move(Value)) {
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
    std::string Value;
};

enum class TokenKind : std::int8_t {
    While,
    If,
    Else,
    Function,
    Import,
    As,
    Return,
    Struct,
    True,
    False,
    Let,
    AmpAmp,
    PipePipe,
    NotEqual,
    EqualEqual,
    GreaterEqual,
    LessEqual,
    LessLess,
    GreaterGreater,
    Greater,
    Less,
    Plus,
    Minus,
    Star,
    Slash,
    Percent,
    LeftParen,
    RightParen,
    LeftBrace,
    RightBrace,
    LeftSqrBrace,
    RightSqrBrace,
    Semicolon,
    Comma,
    Colon,
    Dot,
    Caret,
    Tilde,
    Pipe,
    Amp,
    ExclMark,
    Equal,
    Identifier,
    Integer,
    FloatingPoint,
    Eof
};

inline std::string kindToString(TokenKind Kind) {
    switch (Kind) {
        case TokenKind::While: return "while";
        case TokenKind::If: return "if";
        case TokenKind::Else: return "else";
        case TokenKind::Function: return "fn";
        case TokenKind::Import: return "import";
        case TokenKind::As: return "as";
        case TokenKind::Return: return "return";
        case TokenKind::Struct: return "struct";
        case TokenKind::True: return "true";
        case TokenKind::False: return "false";
        case TokenKind::Let: return "let";
        case TokenKind::AmpAmp: return "&&";
        case TokenKind::PipePipe: return "||";
        case TokenKind::NotEqual: return "!=";
        case TokenKind::EqualEqual: return "==";
        case TokenKind::GreaterEqual: return ">=";
        case TokenKind::LessLess: return "<<";
        case TokenKind::GreaterGreater: return ">>";
        case TokenKind::Greater: return ">";
        case TokenKind::Less: return "<";
        case TokenKind::LessEqual: return "<=";
        case TokenKind::Plus: return "+";
        case TokenKind::Minus: return "-";
        case TokenKind::Star: return "*";
        case TokenKind::Slash: return "/";
        case TokenKind::Percent: return "%";
        case TokenKind::LeftParen: return "(";
        case TokenKind::RightParen: return ")";
        case TokenKind::LeftBrace: return "{";
        case TokenKind::RightBrace: return "}";
        case TokenKind::LeftSqrBrace: return "[";
        case TokenKind::RightSqrBrace: return "]";
        case TokenKind::Semicolon: return ";";
        case TokenKind::Comma: return ",";
        case TokenKind::Colon: return ":";
        case TokenKind::Dot: return ".";
        case TokenKind::Caret: return "^";
        case TokenKind::Tilde: return "~";
        case TokenKind::Pipe: return "|";
        case TokenKind::Amp: return "&";
        case TokenKind::ExclMark: return "!";
        case TokenKind::Equal: return "=";
        case TokenKind::Identifier: return "identifier";
        case TokenKind::Integer: return "integer";
        case TokenKind::FloatingPoint: return "floating point";
        case TokenKind::Eof: return "eof";
    }
    return "unknown";
}