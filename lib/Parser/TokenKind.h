#pragma once
#include <cstdint>
#include <string_view>

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

inline std::string_view kindToString(TokenKind Kind) {
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