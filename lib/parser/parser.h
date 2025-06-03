#pragma once
#include "lexer/lexer.h"
#include "ast/nodes.h"

class Parser {
    Lexer lexer;
    Token current_token;

    void eat(TokenType type);

    std::unique_ptr<ASTNode> factor();

    std::unique_ptr<ASTNode> term();

    std::unique_ptr<ASTNode> arith_expr(); 

    std::unique_ptr<ASTNode> expr();

    std::unique_ptr<ASTNode> logic_and();
    
    std::unique_ptr<ASTNode> logic_or();  

    std::unique_ptr<ASTNode> comparison();

    std::unique_ptr<ASTNode> parse_statement();

    std::unique_ptr<ASTNode> parse_if();

    std::unique_ptr<ASTNode> parse_for();

    std::unique_ptr<ASTNode> parse_while();

public:
    Parser(const std::string& text);

    std::unique_ptr<ASTNode> parse();
};