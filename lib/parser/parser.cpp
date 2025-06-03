#include "parser.h"
#include "tokens/tokens.h"

void Parser::eat(TokenType type) {
    if (current_token.type == type) {
        current_token = lexer.get_next_token();
    } else {
        std::string msg = "Expected " + token_type_to_string(type) +
                          ", got " + token_type_to_string(current_token.type) +
                          " (" + current_token.value + ")";
        throw std::runtime_error(msg);
    }
}

std::unique_ptr<ASTNode> Parser::factor() {
    Token token = current_token;
    
    if (token.type == TokenType::INTEGER) {
        eat(TokenType::INTEGER);
        return std::make_unique<NumberNode>(std::stoi(token.value));
    }
    else if (token.type == TokenType::BOOL) {
        eat(TokenType::BOOL);
        return std::make_unique<BoolNode>(token.value);
    }
    else if (token.type == TokenType::STRING) {
        eat(TokenType::STRING);
        return std::make_unique<StringNode>(token.value);
    }
    else if (token.type == TokenType::NOT) {
        eat(TokenType::NOT);
        auto node = factor();
        auto falseNode = std::make_unique<NumberNode>(0);
        return std::make_unique<BinOpNode>(std::move(node), TokenType::EQUAL_EQUAL, std::move(falseNode));
    }
    else if (token.type == TokenType::LPAREN) {
        eat(TokenType::LPAREN);
        auto node = comparison();
        eat(TokenType::RPAREN);
        return node;
    }
    else if (token.type == TokenType::VAR) {
        std::string name = token.value;
        eat(TokenType::VAR);

        if (name == "len") {
            eat(TokenType::LPAREN);
            auto arg = comparison();
            eat(TokenType::RPAREN);
            return std::make_unique<LenNode>(std::move(arg));
        } else {
            return std::make_unique<VariableNode>(name);
        }
    }
    
    throw std::runtime_error("Unexpected token in factor: " + 
                            token_type_to_string(token.type) + " (" + token.value + ")");
}

std::unique_ptr<ASTNode> Parser::term() {
    auto node = factor();
    while (current_token.type == TokenType::MULTIPLY || current_token.type == TokenType::DIVIDE || current_token.type == TokenType::REM || current_token.type == TokenType::POW) {
        Token token = current_token;
        eat(token.type);
        node = std::make_unique<BinOpNode>(std::move(node), token.type, factor());
    }
    return node;
}

std::unique_ptr<ASTNode> Parser::comparison() {
    auto node = expr(); 
    while (current_token.type == TokenType::EQUAL_EQUAL ||
           current_token.type == TokenType::NOT_EQUAL ||
           current_token.type == TokenType::LESS ||
           current_token.type == TokenType::GREATER ||
           current_token.type == TokenType::LESS_EQUAL ||
           current_token.type == TokenType::GREATER_EQUAL) {
        Token op = current_token;
        eat(op.type);
        node = std::make_unique<BinOpNode>(std::move(node), op.type, expr());
    }
    
    return node;
}

std::unique_ptr<ASTNode> Parser::logic_and() {
    auto node = comparison();
    while (current_token.type == TokenType::AND) {
        TokenType op = current_token.type;
        eat(op);
        node = std::make_unique<BinOpNode>(std::move(node), op, comparison());
    }
    return node;
}


std::unique_ptr<ASTNode> Parser::logic_or() {
    auto node = logic_and();
    while (current_token.type == TokenType::OR) {
        TokenType op = current_token.type;
        eat(op);
        node = std::make_unique<BinOpNode>(std::move(node), op, logic_and());
    }
    return node;
}

std::unique_ptr<ASTNode> Parser::expr() {
    auto node = term();
    
    while (current_token.type == TokenType::PLUS || current_token.type == TokenType::MINUS) {
        Token op = current_token;
        eat(op.type);
        node = std::make_unique<BinOpNode>(std::move(node), op.type, term());
    }
    
    return node;
}


Parser::Parser(const std::string& text) : lexer(text), current_token(lexer.get_next_token()) {}

std::unique_ptr<ASTNode> Parser::parse() {
    if (current_token.type == TokenType::IF) {
        return parse_if();
    }
    else if (current_token.type == TokenType::FOR) {
        return parse_for();
    }
    else if (current_token.type == TokenType::WHILE) {
        return parse_while(); 
    }
    else if (current_token.type == TokenType::PRINT) {
        eat(TokenType::PRINT);
        eat(TokenType::LPAREN);
        auto arg = comparison();
        eat(TokenType::RPAREN); 
        return std::make_unique<PrintNode>(std::move(arg));
    } 
    else if (current_token.type == TokenType::VAR) {
        std::string var_name = current_token.value;
        eat(TokenType::VAR);
        if (current_token.type == TokenType::EQUAL) { 
            eat(TokenType::EQUAL);
            return std::make_unique<AssignmentNode>(var_name, comparison());
        } else {
            return std::make_unique<VariableNode>(var_name);
        }
    } 
    else {
        return comparison();
    }
}

std::unique_ptr<ASTNode> Parser::parse_if() {
    eat(TokenType::IF);
    auto condition = comparison();
    eat(TokenType::THEN);

    std::vector<std::unique_ptr<ASTNode>> then_branch;
    while (current_token.type != TokenType::ELSE && 
           current_token.type != TokenType::END_IF &&
           current_token.type != TokenType::END) {
        then_branch.push_back(parse());
    }

    std::vector<IfNode::ElseIfBranch> else_ifs;
    std::vector<std::unique_ptr<ASTNode>> else_branch;

    while (current_token.type == TokenType::ELSE) {
        eat(TokenType::ELSE);
        
        if (current_token.type == TokenType::IF) {
            eat(TokenType::IF);
            auto elif_cond = comparison();
            eat(TokenType::THEN);
            
            std::vector<std::unique_ptr<ASTNode>> elif_body;
            while (current_token.type != TokenType::ELSE && 
                   current_token.type != TokenType::END_IF &&
                   current_token.type != TokenType::END) {
                elif_body.push_back(parse());
            }
            
            else_ifs.push_back({std::move(elif_cond), std::move(elif_body)});
        } 
        else {
            while (current_token.type != TokenType::END_IF &&
                   current_token.type != TokenType::END) {
                else_branch.push_back(parse());
            }
            break;
        }
    }

    if (current_token.type == TokenType::END_IF) {
        eat(TokenType::END_IF);
    } else if (current_token.type == TokenType::END) {
        eat(TokenType::END);
    } else {
        throw std::runtime_error("Expected 'end if' or end of input");
    }
    
    return std::make_unique<IfNode>(
        std::move(condition),
        std::move(then_branch),
        std::move(else_ifs),
        std::move(else_branch)
    );
}

std::unique_ptr<ASTNode> Parser::parse_for() {
    eat(TokenType::FOR); 
    if (current_token.type != TokenType::VAR) {
        throw std::runtime_error("Expected loop variable after 'for'");
    }
    std::string var_name = current_token.value;
    eat(TokenType::VAR);

    if (current_token.type != TokenType::IN) {
        throw std::runtime_error("Expected 'in' after loop variable in for-statement");
    }
    eat(TokenType::IN);

    if (!(current_token.type == TokenType::VAR && current_token.value == "range")) {
        throw std::runtime_error("Expected 'range' after 'in' in for-statement");
    }
    eat(TokenType::VAR);

    eat(TokenType::LPAREN);

    auto start_node = comparison();
    eat(TokenType::COMMA);

    auto end_node = comparison();
    eat(TokenType::COMMA);

    auto step_node = comparison();
    eat(TokenType::RPAREN);

    std::vector<std::unique_ptr<ASTNode>> body_nodes;
    while (current_token.type != TokenType::END_FOR && 
           current_token.type != TokenType::END) {
        body_nodes.push_back(parse());
    }

    if (current_token.type == TokenType::END_FOR) {
        eat(TokenType::END_FOR);
    } else {
        throw std::runtime_error("Expected 'end for' at end of for-statement");
    }

    return std::make_unique<ForNode>(
        var_name,
        std::move(start_node),
        std::move(end_node),
        std::move(step_node),
        std::move(body_nodes)
    );
}

std::unique_ptr<ASTNode> Parser::parse_while() {
    eat(TokenType::WHILE);
    auto condition_node = comparison();

    std::vector<std::unique_ptr<ASTNode>> body_nodes;
    while (current_token.type != TokenType::END_WHILE &&
           current_token.type != TokenType::END) {
        body_nodes.push_back(parse());
    }

    if (current_token.type == TokenType::END_WHILE) {
        eat(TokenType::END_WHILE);
    } else {
        throw std::runtime_error("Expected 'end while' at end of while-statement");
    }

    return std::make_unique<WhileNode>(std::move(condition_node),
                                       std::move(body_nodes));
}