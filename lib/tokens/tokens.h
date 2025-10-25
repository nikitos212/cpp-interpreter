#pragma once
#include <string>

enum class TokenType {
    INTEGER,
    BOOL,
    NIL,
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
    FUNCTION,
    END_FUNCTION,
    RETURN,
    LBRACKET,
    RBRACKET,
    COLON, 
    RANGE,
    LEN,
    PUSH,
    POP,
    REMOVE,
    SORT,
    INSERT,
    MAX,
    MIN,
    ABS,
    CEIL,
    FLOOR,
    ROUND,
    SQRT,
    RND,
    PARSE_NUM,
    TO_STRING,
    LOWER,
    UPPER,
    SPLIT,
    JOIN,
    REPLACE,
    PRINTLN,
    READ,
    STACKTRACE,
    BREAK,
    CONTINUE,
    END
};

inline std::string token_type_to_string(TokenType type) {
    switch (type) {
        case TokenType::INTEGER:       return "INTEGER";
        case TokenType::BOOL:          return "BOOL";
        case TokenType::STRING:        return "STRING";
        case TokenType::NIL:           return "NIL";
        case TokenType::LPAREN:        return "LPAREN";
        case TokenType::RPAREN:        return "RPAREN";
        case TokenType::LBRACKET:      return "LBRACKET";
        case TokenType::RBRACKET:      return "RBRACKET";
        case TokenType::COLON:         return "COLON";
        case TokenType::COMMA:         return "COMMA";
        case TokenType::EQUAL:         return "EQUAL";
        case TokenType::PLUS_EQUAL:    return "PLUS_EQUAL";
        case TokenType::MINUS_EQUAL:   return "MINUS_EQUAL";
        case TokenType::MULTIPLY_EQUAL:return "MULTIPLY_EQUAL";
        case TokenType::DIVIDE_EQUAL:  return "DIVIDE_EQUAL";
        case TokenType::MOD_EQUAL:     return "MOD_EQUAL";
        case TokenType::POWER_EQUAL:   return "POWER_EQUAL";
        case TokenType::PLUS:          return "PLUS";
        case TokenType::MINUS:         return "MINUS";
        case TokenType::MULTIPLY:      return "MULTIPLY";
        case TokenType::DIVIDE:        return "DIVIDE";
        case TokenType::REM:           return "REM";
        case TokenType::POW:           return "POW";
        case TokenType::EQUAL_EQUAL:   return "EQUAL_EQUAL";
        case TokenType::NOT_EQUAL:     return "NOT_EQUAL";
        case TokenType::LESS:          return "LESS";
        case TokenType::GREATER:       return "GREATER";
        case TokenType::LESS_EQUAL:    return "LESS_EQUAL";
        case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
        case TokenType::AND:           return "AND";
        case TokenType::OR:            return "OR";
        case TokenType::NOT:           return "NOT";
        case TokenType::VAR:           return "VAR";
        case TokenType::PRINT:         return "PRINT";
        case TokenType::IF:            return "IF";
        case TokenType::THEN:          return "THEN";
        case TokenType::ELSE:          return "ELSE";
        case TokenType::END_IF:        return "END_IF";
        case TokenType::FOR:           return "FOR";
        case TokenType::IN:            return "IN";
        case TokenType::WHILE:         return "WHILE";
        case TokenType::FUNCTION:      return "FUNCTION";
        case TokenType::RETURN:        return "RETURN";
        case TokenType::RANGE:         return "RANGE";
        case TokenType::PUSH:          return "PUSH";
        case TokenType::POP:           return "POP";
        case TokenType::INSERT:        return "INSERT";
        case TokenType::REMOVE:        return "REMOVE";
        case TokenType::SORT:          return "SORT";
        case TokenType::END:           return "END";
        default: return "UNKNOWN";
    }
}

struct Token {
    Token();
    TokenType type;
    std::string value;
    Token(TokenType t, const std::string& v = "");
};

inline int pow(int l, int r) {
    int res = 1;
    if (r == 0) return 1;
    if (r > 0)  for (int i = 0; i < r; ++i) res *= l;
    else for (int i = 0; i < r; ++i) res /= l;

    return res;
}





