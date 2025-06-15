#include "lexer.h"
#include "tokens/tokens.h"
#include <stdexcept>

void Lexer::step() {
    pos++;
    current_char = (pos < text.size()) ? text[pos] : '\0';
}

char Lexer::peek(size_t ahead) const {
    return (pos + ahead < text.size()) ? text[pos + ahead] : '\0';
}

void Lexer::skip_whitespace() {
    while (current_char != '\0' && isspace(current_char)) step();
}

std::string Lexer::number() {
    std::string result;
    bool has_dot = false;
    while (current_char != '\0' && (isdigit(current_char) || current_char == '.')) {
        if (current_char == '.') {
            if (has_dot) break;
            has_dot = true;
        }
        result += current_char;
        step();
    }

    if (current_char == 'e' || current_char == 'E') {
        result += current_char;
        step();
        if (current_char == '+' || current_char == '-') {
            result += current_char;
            step();
        }
        if (!isdigit(current_char)) {
            throw std::runtime_error("Malformed exponent in number");
        }
        while (isdigit(current_char)) {
            result += current_char;
            step();
        }
    }

    return result;
}


Lexer::Lexer(const std::string& t) : text(t), current_char(t.empty() ? '\0' : t[0]) {}

Token Lexer::get_next_token() {
    while (current_char != '\0') {
        if (isspace(current_char)) {
            skip_whitespace();
            continue;
        }

        if (isdigit(current_char)) {
            return Token(TokenType::INTEGER, number());
        }

        if (current_char == '"') {
            step();
            std::string str;
            while (current_char != '"' && current_char != '\0') {
                if (current_char == '\\') {
                    step();
                    switch (current_char) {
                        case 'n':  str += '\n'; break;
                        case 't':  str += '\t'; break;
                        case 'r':  str += '\r'; break;
                        case '\\': str += '\\'; break;
                        case '"':  str += '"';  break;
                        case '\'': str += '\''; break;
                        case '0':  str += '\0'; break;
                        default:
                            throw std::runtime_error(std::string("Unknown escape sequence: \\") + current_char);
                    }
                    step();
                } else {
                    str += current_char;
                    step();
                }
            }
            if (current_char != '"')
                throw std::runtime_error("Unterminated string literal");
            step();
            return Token(TokenType::STRING, str);
        }


        if (isalpha(current_char)) {
            std::string word;
            while (isalnum(current_char) || current_char == '_') {
                word += current_char;
                step();
            }

            if (word == "end") {
                while (isspace(current_char)) step();
                
                if (current_char == 'i' && peek(1) == 'f') {
                    step();
                    step(); 
                    return Token(TokenType::END_IF);
                }
                if (current_char == 'f' && peek(1) == 'o' && peek(2) == 'r') {
                    step(); step(); step();
                    return Token(TokenType::END_FOR);
                }
                if (current_char == 'w' && peek(1) == 'h' && peek(2) == 'i' &&
                    peek(3) == 'l' && peek(4) == 'e') {
                    step(); step(); step(); step(); step();
                    return Token(TokenType::END_WHILE);
                }
                if (current_char == 'f' && peek(1) == 'u' && peek(2) == 'n' && peek(3) == 'c' && peek(4) == 't' && peek(5) == 'i' && peek(6) == 'o' && peek(7) == 'n') {
                    step(); step(); step(); step(); step(); step(); step(); step();
                    return Token(TokenType::END_FUNCTION);
                }

                return Token(TokenType::END);
            }
            
            if (word == "print") return Token(TokenType::PRINT);
            if (word == "println") return Token(TokenType::PRINTLN);
            if (word == "read") return Token(TokenType::READ);
            if (word == "stacktrace") return Token(TokenType::STACKTRACE);
            if (word == "if") return Token(TokenType::IF);
            if (word == "then") return Token(TokenType::THEN);
            if (word == "else") return Token(TokenType::ELSE);
            if (word == "true") return Token(TokenType::BOOL, "true");
            if (word == "false") return Token(TokenType::BOOL, "false");
            if (word == "for") return Token(TokenType::FOR);
            if (word == "in") return Token(TokenType::IN);
            if (word == "while") return Token(TokenType::WHILE);
            if (word == "and") return Token(TokenType::AND);
            if (word == "or") return Token(TokenType::OR);
            if (word == "not") return Token(TokenType::NOT);
            if (word == "function") return Token(TokenType::FUNCTION);
            if (word == "return") return Token(TokenType::RETURN);
            if (word == "push") return Token(TokenType::PUSH);
            if (word == "pop") return Token(TokenType::POP);
            if (word == "insert") return Token(TokenType::INSERT);
            if (word == "remove") return Token(TokenType::REMOVE);
            if (word == "sort") return Token(TokenType::SORT);
            if (word == "len") return Token(TokenType::LEN);
            if (word == "MAX") return Token(TokenType::MAX);
            if (word == "MIN") return Token(TokenType::MIN);
            if (word == "nil") return Token(TokenType::NIL);
            if (word == "ceil") return Token(TokenType::CEIL);
            if (word == "abs") return Token(TokenType::ABS);
            if (word == "floor") return Token(TokenType::FLOOR);
            if (word == "round") return Token(TokenType::ROUND);
            if (word == "sqrt") return Token(TokenType::SQRT);
            if (word == "rnd") return Token(TokenType::RND);
            if (word == "parse_num") return Token(TokenType::PARSE_NUM);
            if (word == "to_string") return Token(TokenType::TO_STRING);
            if (word == "lower") return Token(TokenType::LOWER);
            if (word == "upper") return Token(TokenType::UPPER);
            if (word == "split") return Token(TokenType::SPLIT);
            if (word == "join") return Token(TokenType::JOIN);
            if (word == "replace") return Token(TokenType::REPLACE);
            if (word == "break")    return Token(TokenType::BREAK);
            if (word == "continue") return Token(TokenType::CONTINUE);
            
            return Token(TokenType::VAR, word);
        }

        if (current_char == '+') {
            if (peek(1) == '=') { step(); step(); return Token(TokenType::PLUS_EQUAL); }
            step(); return Token(TokenType::PLUS);
        }
        if (current_char == '-') {
            if (peek(1) == '=') { step(); step(); return Token(TokenType::MINUS_EQUAL); }
            if (isdigit(peek(1))) { step(); return Token(TokenType::INTEGER, '-' + number()); }
            step(); return Token(TokenType::MINUS);
        }
        if (current_char == '*') {
            if (peek(1) == '=') { step(); step(); return Token(TokenType::MULTIPLY_EQUAL); }
            step(); return Token(TokenType::MULTIPLY);
        }
        if (current_char == '/') {
            if (peek(1) == '=') { step(); step(); return Token(TokenType::DIVIDE_EQUAL); }
            step(); return Token(TokenType::DIVIDE);
        }
        if (current_char == '%') {
            if (peek(1) == '=') { step(); step(); return Token(TokenType::MOD_EQUAL); }
            step(); return Token(TokenType::REM); 
        }
        if (current_char == '^') {
            if (peek(1) == '=') { step(); step(); return Token(TokenType::POWER_EQUAL); }
            step(); return Token(TokenType::POW); 
        }

        if (current_char == '=') {
            step();
            if (current_char == '=') {
                step();
                return Token(TokenType::EQUAL_EQUAL);
            }
            return Token(TokenType::EQUAL);
        }

        if (current_char == '!') {
            step();
            if (current_char == '=') {
                step();
                return Token(TokenType::NOT_EQUAL);
            }
            return Token(TokenType::NOT);
        }

        if (current_char == '<') {
            step();
            if (current_char == '=') {
                step();
                return Token(TokenType::LESS_EQUAL);
            }
            return Token(TokenType::LESS);
        }

        if (current_char == '>') {
            step();
            if (current_char == '=') {
                step();
                return Token(TokenType::GREATER_EQUAL);
            }
            return Token(TokenType::GREATER);
        }

        switch (current_char) {
            case '(': step(); return Token(TokenType::LPAREN);
            case ')': step(); return Token(TokenType::RPAREN);
            case ',': step(); return Token(TokenType::COMMA);
            case '[': step(); return Token(TokenType::LBRACKET);
            case ']': step(); return Token(TokenType::RBRACKET);
            case ':': step(); return Token(TokenType::COLON);
            default:
                throw std::runtime_error(std::string("Unexpected character: ") + current_char);
        }
    }
    return Token(TokenType::END);
}