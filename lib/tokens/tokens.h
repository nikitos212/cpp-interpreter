#pragma once
#include <string>

enum class TokenType {
    INTEGER,
    EQUAL,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    EQUAL_EQUAL, 
    NOT_EQUAL,
    LESS,
    GREATER,
    LESS_EQUAL,
    GREATER_EQUAL, 
    LPAREN,
    RPAREN,
    VAR,
    PRINT,
    IF,
    THEN,
    ELSE,
    END_IF,
    STRING,
    END
};

inline std::string token_type_to_string(TokenType type) {
    switch (type) {
        case TokenType::INTEGER: return "INTEGER";
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::MULTIPLY: return "MULTIPLY";
        case TokenType::DIVIDE: return "DIVIDE";
        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";
        case TokenType::VAR: return "VAR";
        case TokenType::PRINT: return "PRINT";
        case TokenType::END: return "END";
        default: return "UNKNOWN";
    }
}

struct Token {
    Token();
    TokenType type;
    std::string value;
    Token(TokenType t, const std::string& v = "");
};

