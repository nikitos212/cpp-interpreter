#include "tokens.h"

Token::Token() = default;
Token::Token(TokenType t, const std::string& v) : type(t), value(v) {}

