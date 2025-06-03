#pragma once
#include <string>

enum class TokenType {
    INTEGER,
    BOOL,
    EQUAL,
    PLUS_EQUAL,
    MINUS_EQUAL,
    MULTIPLY_EQUAL,
    DIVIDE_EQUAL,
    MOD_EQUAL,
    POWER_EQUAL,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    POW,
    REM,
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
    FOR,
    IN,
    END_FOR,
    COMMA,
    WHILE,
    END_WHILE,
    AND,
    OR,
    NOT,
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

inline std::string operator*(const std::string& str, size_t n) {
    std::string result;
    result.reserve(str.size() * n);
    while (n--) result += str;
    return result;
}


inline int pow(int l, int r) {
    int res = 1;
    if (r == 0) return 1;
    if (r > 0)  for (int i = 0; i < r; ++i) res *= l;
    else for (int i = 0; i < r; ++i) res /= l;

    return res;
}
