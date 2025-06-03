#pragma once
#include "tokens/tokens.h"
#include <string>

class Lexer {
    std::string text;
    size_t pos = 0;
    char current_char;

    void step();

    char peek(size_t ahead = 1) const;

    void skip_whitespace();

    std::string number();

public:
    Lexer(const std::string& t);

    Token get_next_token();
};