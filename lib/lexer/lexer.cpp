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
    return result;
}

std::string Lexer::variable() {
    std::string name;
    while (current_char != '\0' && (isalnum(current_char) || current_char == '_')) {
        name += current_char;
        step();
    }
    return name;
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
                str += current_char;
                step();
            }
            if (current_char != '"') {
                throw std::runtime_error("Unterminated string literal");
            }
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
                return Token(TokenType::END);
            }
            
            if (word == "print") return Token(TokenType::PRINT);
            if (word == "if") return Token(TokenType::IF);
            if (word == "then") return Token(TokenType::THEN);
            if (word == "else") return Token(TokenType::ELSE);
            
            return Token(TokenType::VAR, word);
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
            throw std::runtime_error("Expected !=, got !");
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
            case '+': step(); return Token(TokenType::PLUS);
            case '-': step(); return Token(TokenType::MINUS);
            case '*': step(); return Token(TokenType::MULTIPLY);
            case '/': step(); return Token(TokenType::DIVIDE);
            case '(': step(); return Token(TokenType::LPAREN);
            case ')': step(); return Token(TokenType::RPAREN);
            default:
                throw std::runtime_error(std::string("Unexpected character: ") + current_char);
        }
    }
    return Token(TokenType::END);
}