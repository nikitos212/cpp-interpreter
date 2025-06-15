#include "parser.h"
#include "ast/nodes.h"
#include "tokens/tokens.h"
#include <climits>
#include <memory>

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
        std::string s = token.value;
        eat(TokenType::INTEGER);
        if (s.find_first_of("eE.") != std::string::npos) {
            return std::make_unique<NumberNode>(std::stod(s));
        } else {
            return std::make_unique<NumberNode>(std::stoi(s));
        }
    }

    if (token.type == TokenType::BOOL) {
        eat(TokenType::BOOL);
        return std::make_unique<BoolNode>(token.value);
    }

    if (token.type == TokenType::STRING) {
        eat(TokenType::STRING);
        return std::make_unique<StringNode>(token.value);
    }

    if (token.type == TokenType::NIL) {
        eat(TokenType::NIL);
        return std::make_unique<NilNode>();
    }

    if (token.type == TokenType::NOT) {
        eat(TokenType::NOT);
        auto inner = factor();
        auto falseNode = std::make_unique<NumberNode>(0);
        return std::make_unique<BinOpNode>(std::move(inner), TokenType::EQUAL_EQUAL, std::move(falseNode));
    }

    if (token.type == TokenType::READ) {
        eat(TokenType::READ);
        eat(TokenType::LPAREN);
        eat(TokenType::RPAREN);
        return std::make_unique<ReadNode>();
    }

    if (token.type == TokenType::LEN) {
        eat(TokenType::LEN);
        eat(TokenType::LPAREN);
        auto inside = expr();
        eat(TokenType::RPAREN);
        return std::make_unique<LenNode>(std::move(inside));
    }

    if (token.type == TokenType::MAX) {
        eat(TokenType::MAX);
        eat(TokenType::LPAREN);
        auto inside = expr();
        eat(TokenType::RPAREN);
        return std::make_unique<MaxNode>(std::move(inside));
    }

    if (token.type == TokenType::MIN) {
        eat(TokenType::MIN);
        eat(TokenType::LPAREN);
        auto inside = expr();
        eat(TokenType::RPAREN);
        return std::make_unique<MinNode>(std::move(inside));
    }

    if (token.type == TokenType::ABS) {
        eat(TokenType::ABS);
        eat(TokenType::LPAREN);
        auto inside = expr();
        eat(TokenType::RPAREN);
        return std::make_unique<AbsNode>(std::move(inside));
    }

    if (token.type == TokenType::CEIL) {
        eat(TokenType::CEIL);
        eat(TokenType::LPAREN);
        auto inside = expr();
        eat(TokenType::RPAREN);
        return std::make_unique<CeilNode>(std::move(inside));
    }

    if (token.type == TokenType::FLOOR) {
        eat(TokenType::FLOOR);
        eat(TokenType::LPAREN);
        auto inside = expr();
        eat(TokenType::RPAREN);
        return std::make_unique<FloorNode>(std::move(inside));
    }

    if (token.type == TokenType::ROUND) {
        eat(TokenType::ROUND);
        eat(TokenType::LPAREN);
        auto inside = expr();
        eat(TokenType::RPAREN);
        return std::make_unique<RoundNode>(std::move(inside));
    }

    if (token.type == TokenType::SQRT) {
        eat(TokenType::SQRT);
        eat(TokenType::LPAREN);
        auto inside = expr();
        eat(TokenType::RPAREN);
        return std::make_unique<SqrtNode>(std::move(inside));
    }

    if (token.type == TokenType::RND) {
        eat(TokenType::RND);
        eat(TokenType::LPAREN);
        auto inside = expr();
        eat(TokenType::RPAREN);
        return std::make_unique<RndNode>(std::move(inside));
    }

    if (token.type == TokenType::PARSE_NUM) {
        eat(TokenType::PARSE_NUM);
        eat(TokenType::LPAREN);
        auto inside = expr();
        eat(TokenType::RPAREN);
        return std::make_unique<ParseNumNode>(std::move(inside));
    }

    if (token.type == TokenType::TO_STRING) {
        eat(TokenType::TO_STRING);
        eat(TokenType::LPAREN);
        auto inside = expr();
        eat(TokenType::RPAREN);
        return std::make_unique<ToStringNode>(std::move(inside));
    }

    if (token.type == TokenType::LOWER) {
        eat(TokenType::LOWER);
        eat(TokenType::LPAREN);
        auto inside = expr();
        eat(TokenType::RPAREN);
        return std::make_unique<LowerNode>(std::move(inside));
    }

    if (token.type == TokenType::UPPER) {
        eat(TokenType::UPPER);
        eat(TokenType::LPAREN);
        auto inside = expr();
        eat(TokenType::RPAREN);
        return std::make_unique<UpperNode>(std::move(inside));
    }

    if (token.type == TokenType::SPLIT) {
        eat(TokenType::SPLIT);
        eat(TokenType::LPAREN);
        auto s = expr();
        eat(TokenType::COMMA);
        auto delim = expr();
        eat(TokenType::RPAREN);
        return std::make_unique<SplitNode>(std::move(s), std::move(delim));
    }

    if (token.type == TokenType::JOIN) {
        eat(TokenType::JOIN);
        eat(TokenType::LPAREN);
        auto s = expr();
        eat(TokenType::COMMA);
        auto delim = expr();
        eat(TokenType::RPAREN);
        return std::make_unique<JoinNode>(std::move(s), std::move(delim));
    }

    if (token.type == TokenType::REPLACE) {
        eat(TokenType::REPLACE);
        eat(TokenType::LPAREN);
        auto s = expr();
        eat(TokenType::COMMA);
        auto old = expr();
        eat(TokenType::COMMA);
        auto new_s = expr();
        eat(TokenType::RPAREN);
        return std::make_unique<ReplaceNode>(std::move(s), std::move(old), std::move(new_s));
    }

    if (token.type == TokenType::PUSH) {
        eat(TokenType::PUSH);
        eat(TokenType::LPAREN);
        auto list = expr();
        eat(TokenType::COMMA);
        auto elem = expr();
        eat(TokenType::RPAREN);
        return std::make_unique<PushNode>(std::move(list), std::move(elem));
    }

    if (token.type == TokenType::POP) {
        eat(TokenType::POP);
        eat(TokenType::LPAREN);
        auto inside = expr();
        eat(TokenType::RPAREN);
        return std::make_unique<PopNode>(std::move(inside));
    }

    if (token.type == TokenType::SORT) {
        eat(TokenType::SORT);
        eat(TokenType::LPAREN);
        auto inside = expr();
        eat(TokenType::RPAREN);
        return std::make_unique<SortNode>(std::move(inside));
    }

    if (token.type == TokenType::REMOVE) {
        eat(TokenType::REMOVE);
        eat(TokenType::LPAREN);
        auto list = expr();
        eat(TokenType::COMMA);
        auto ind = expr();
        eat(TokenType::RPAREN);
        return std::make_unique<RemoveNode>(std::move(list), std::move(ind));
    }

    if (token.type == TokenType::INSERT) {
        eat(TokenType::INSERT);
        eat(TokenType::LPAREN);
        auto list = expr();
        eat(TokenType::COMMA);
        auto ind = expr();
        eat(TokenType::COMMA);
        auto x = expr();
        eat(TokenType::RPAREN);
        return std::make_unique<InsertNode>(std::move(list), std::move(ind), std::move(x));
    }

    if (token.type == TokenType::FUNCTION) {
        auto functionNode = parse_function();

        if (current_token.type == TokenType::LPAREN) {
            eat(TokenType::LPAREN);
            std::vector<std::unique_ptr<ASTNode>> args;
            if (current_token.type != TokenType::RPAREN) {
                args.push_back(expr());
                while (current_token.type == TokenType::COMMA) {
                    eat(TokenType::COMMA);
                    args.push_back(expr());
                }
            }
            eat(TokenType::RPAREN);
            return std::make_unique<CallNode>(
                std::move(functionNode),
                std::move(args)
            );
        }

        return functionNode;
    }

    if (token.type == TokenType::RETURN) {
        return parse_return();
    }

    if (token.type == TokenType::VAR) {
        std::string name = token.value;
        eat(TokenType::VAR);

        if (current_token.type == TokenType::LPAREN) {
            eat(TokenType::LPAREN);
            std::vector<std::unique_ptr<ASTNode>> args;
            if (current_token.type != TokenType::RPAREN) {
                args.push_back(expr());
                while (current_token.type == TokenType::COMMA) {
                    eat(TokenType::COMMA);
                    args.push_back(expr());
                }
            }
            eat(TokenType::RPAREN);
            auto varNode = std::make_unique<VariableNode>(name);
            return std::make_unique<CallNode>(
                std::move(varNode),
                std::move(args)
            );
        }

        if (current_token.type == TokenType::LBRACKET) {
            std::unique_ptr<ASTNode> idx;
            std::unique_ptr<ASTNode> endExpr;

            eat(TokenType::LBRACKET);

            if (current_token.type == TokenType::COLON) {
                eat(TokenType::COLON);
                idx = std::make_unique<NumberNode>(-1);
                if (current_token.type == TokenType::RBRACKET) {
                    eat(TokenType::RBRACKET);
                    endExpr = std::make_unique<NumberNode>(INT_MAX);
                    return std::make_unique<SliceNode>(
                    std::move(std::make_unique<VariableNode>(name)),
                    std::move(idx),
                    std::move(endExpr)
                    );
                }
                endExpr = expr();
                eat(TokenType::RBRACKET);
                return std::make_unique<SliceNode>(
                    std::move(std::make_unique<VariableNode>(name)),
                    std::move(idx),
                    std::move(endExpr)
                );
            }

            idx = expr();
            if (current_token.type == TokenType::COLON) {
                eat(TokenType::COLON);
                if (current_token.type == TokenType::RBRACKET) {
                    eat(TokenType::RBRACKET);
                    endExpr = std::make_unique<NumberNode>(INT_MAX);
                    return std::make_unique<SliceNode>(
                    std::move(std::make_unique<VariableNode>(name)),
                    std::move(idx),
                    std::move(endExpr)
                    );
                }
                endExpr = expr();
                eat(TokenType::RBRACKET);
                return std::make_unique<SliceNode>(
                    std::move(std::make_unique<VariableNode>(name)),
                    std::move(idx),
                    std::move(endExpr)
                );
            } else {
                auto node = std::unique_ptr<ASTNode>();
                eat(TokenType::RBRACKET);
                node = std::make_unique<IndexNode>(
                    std::make_unique<VariableNode>(name),
                    std::move(idx)
                );

                while (current_token.type == TokenType::LPAREN) {
                    eat(TokenType::LPAREN);
                    std::vector<std::unique_ptr<ASTNode>> args;
                    if (current_token.type != TokenType::RPAREN) {
                        args.push_back(expr());
                        while (current_token.type == TokenType::COMMA) {
                            eat(TokenType::COMMA);
                            args.push_back(expr());
                        }
                    }
                    eat(TokenType::RPAREN);
                    node = std::make_unique<CallNode>(std::move(node), std::move(args));
                }

                return node;
            }
        }

        return std::make_unique<VariableNode>(name);
    }

    if (token.type == TokenType::LPAREN) {
        eat(TokenType::LPAREN);
        auto node = expr();
        eat(TokenType::RPAREN);
        return node;
    }

    if (token.type == TokenType::LBRACKET) {
        eat(TokenType::LBRACKET);

        std::vector<std::unique_ptr<ASTNode>> elements;
        if (current_token.type != TokenType::RBRACKET) {
            while (true) {
                elements.push_back(expr());
                if (current_token.type == TokenType::COMMA) {
                    eat(TokenType::COMMA);
                    if (current_token.type == TokenType::RBRACKET)
                        break;
                } else {
                    break;
                }
            }
        }
        eat(TokenType::RBRACKET);

        auto node = std::make_unique<ListNode>(std::move(elements));

        return node;
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

std::unique_ptr<ASTNode> Parser::arith_expr() {
    auto node = term();
    
    while (current_token.type == TokenType::PLUS || current_token.type == TokenType::MINUS) {
        Token op = current_token;
        eat(op.type);
        node = std::make_unique<BinOpNode>(std::move(node), op.type, term());
    }
    
    return node;
}

std::unique_ptr<ASTNode> Parser::comparison() {
    auto node = arith_expr(); 
    while (current_token.type == TokenType::EQUAL_EQUAL ||
           current_token.type == TokenType::NOT_EQUAL ||
           current_token.type == TokenType::LESS ||
           current_token.type == TokenType::GREATER ||
           current_token.type == TokenType::LESS_EQUAL ||
           current_token.type == TokenType::GREATER_EQUAL) {
        Token op = current_token;
        eat(op.type);
        node = std::make_unique<BinOpNode>(std::move(node), op.type, arith_expr());
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
    return logic_or();
}


Parser::Parser(const std::string& text) : lexer(text), current_token(lexer.get_next_token()) {}

std::unique_ptr<ASTNode> Parser::parse() {
    if (current_token.type == TokenType::BREAK) {
        eat(TokenType::BREAK);
        return std::make_unique<BreakNode>();
    }
    if (current_token.type == TokenType::CONTINUE) {
        eat(TokenType::CONTINUE);
        return std::make_unique<ContinueNode>();
    }
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
        auto arg = expr();
        eat(TokenType::RPAREN); 
        return std::make_unique<PrintNode>(std::move(arg));
    }
    else if (current_token.type == TokenType::PRINTLN) {
        eat(TokenType::PRINTLN);
        eat(TokenType::LPAREN);
        auto arg = expr();
        eat(TokenType::RPAREN); 
        return std::make_unique<PrintlnNode>(std::move(arg));
    } 
    if (current_token.type == TokenType::VAR) {
        std::string var_name = current_token.value;
        eat(TokenType::VAR);

        if (current_token.type == TokenType::PLUS_EQUAL
         || current_token.type == TokenType::MINUS_EQUAL
         || current_token.type == TokenType::MULTIPLY_EQUAL
         || current_token.type == TokenType::DIVIDE_EQUAL
         || current_token.type == TokenType::MOD_EQUAL
         || current_token.type == TokenType::POWER_EQUAL) {
            TokenType op = current_token.type;
            eat(op);

            auto right = expr();
            TokenType binOp;
            switch (op) {
                case TokenType::PLUS_EQUAL:     binOp = TokenType::PLUS;     break;
                case TokenType::MINUS_EQUAL:    binOp = TokenType::MINUS;    break;
                case TokenType::MULTIPLY_EQUAL: binOp = TokenType::MULTIPLY; break;
                case TokenType::DIVIDE_EQUAL:   binOp = TokenType::DIVIDE;   break;
                case TokenType::MOD_EQUAL:      binOp = TokenType::MOD_EQUAL; break;
                case TokenType::POWER_EQUAL:    binOp = TokenType::POW;    break;
                default:
                    throw std::runtime_error("Unknown compound assignment operator");
            }
            auto varNode = std::make_unique<VariableNode>(var_name);
            auto bin = std::make_unique<BinOpNode>(std::move(varNode), binOp, std::move(right));
            return std::make_unique<AssignmentNode>(var_name, std::move(bin));
        }

        if (current_token.type == TokenType::LPAREN) {
            eat(TokenType::LPAREN);
            std::vector<std::unique_ptr<ASTNode>> args;
            if (current_token.type != TokenType::RPAREN) {
                args.push_back(expr());
                while (current_token.type == TokenType::COMMA) {
                    eat(TokenType::COMMA);
                    args.push_back(expr());
                }
            }
            eat(TokenType::RPAREN);
            auto varNode = std::make_unique<VariableNode>(var_name);
            return std::make_unique<CallNode>(
                std::move(varNode),
                std::move(args)
            );
        }

        if (current_token.type == TokenType::EQUAL) {
            eat(TokenType::EQUAL);
            if (current_token.type == TokenType::FUNCTION) {
                auto fnNode = parse_function();
                return std::make_unique<AssignmentNode>(var_name, std::move(fnNode));
            }
            if (current_token.type == TokenType::LBRACKET) {
                eat(TokenType::LBRACKET);

                std::vector<std::unique_ptr<ASTNode>> elements;
                if (current_token.type != TokenType::RBRACKET) {
                    while (true) {
                        elements.push_back(expr());
                        if (current_token.type == TokenType::COMMA) {
                            eat(TokenType::COMMA);
                            if (current_token.type == TokenType::RBRACKET) {
                                break;
                            }
                            continue;
                        }
                        break;
                    }
                }

                eat(TokenType::RBRACKET);
                auto listNode = std::make_unique<ListNode>(std::move(elements));
                return std::make_unique<AssignmentNode>(var_name, std::move(listNode));
            }
            auto right = expr();
            return std::make_unique<AssignmentNode>(var_name, std::move(right));
        }

        return std::make_unique<VariableNode>(var_name);
    }

    return expr();
}

std::unique_ptr<ASTNode> Parser::parse_if() {
    eat(TokenType::IF);
    auto condition = expr();
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
            auto elif_cond = expr();
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

    if ((current_token.type == TokenType::VAR && current_token.value == "range")) {
        eat(TokenType::VAR);
        eat(TokenType::LPAREN);

        std::unique_ptr<ASTNode> start_node;
        std::unique_ptr<ASTNode> end_node;
        std::unique_ptr<ASTNode> step_node;

        start_node = expr();
        if (current_token.type == TokenType::RPAREN) {
            eat(TokenType::RPAREN);
            end_node = std::move(start_node);
            start_node = std::make_unique<NumberNode>(0);
            step_node = std::make_unique<NumberNode>(1);
        } else {
            eat(TokenType::COMMA);
            end_node = expr();
            if (current_token.type == TokenType::RPAREN) {
                eat(TokenType::RPAREN);
                step_node = std::make_unique<NumberNode>(1);
            } else {
                eat(TokenType::COMMA);
                step_node = expr();
                eat(TokenType::RPAREN);
            }
        }

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
    } else {
        auto iterable_expr = expr();

        std::vector<std::unique_ptr<ASTNode>> body;
        while (current_token.type != TokenType::END_FOR &&
            current_token.type != TokenType::END) {
            body.push_back(parse());
        }
        if (current_token.type == TokenType::END_FOR) {
            eat(TokenType::END_FOR);
        } else {
            throw std::runtime_error("Expected 'end for' at end of for-statement");
        }

        return std::make_unique<ForNode>(
            var_name,
            std::move(iterable_expr),
            std::move(body)
        );
    }
}

std::unique_ptr<ASTNode> Parser::parse_while() {
    eat(TokenType::WHILE);
    auto condition_node = expr();

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

std::unique_ptr<ASTNode> Parser::parse_function() {
    eat(TokenType::FUNCTION);
    std::vector<std::string> paramsList;
    eat(TokenType::LPAREN);
    if (current_token.type != TokenType::RPAREN) {
        paramsList.push_back(current_token.value);
        eat(TokenType::VAR);
        while (current_token.type == TokenType::COMMA) {
            eat(TokenType::COMMA);
            if (current_token.type != TokenType::VAR) {
                throw std::runtime_error("Expected parameter name after comma");
            }
            paramsList.push_back(current_token.value);
            eat(TokenType::VAR);
        }
    }
    eat(TokenType::RPAREN);

    std::vector<std::shared_ptr<ASTNode>> bodyNodesShared;
    while (current_token.type != TokenType::END_FUNCTION)
    {
        auto stmt = parse();
        std::shared_ptr<ASTNode> sharedStmt = std::move(stmt);
        bodyNodesShared.push_back(std::move(sharedStmt));
    }
    eat(TokenType::END_FUNCTION);
    return std::make_unique<FunctionNode>(
        std::move(paramsList),
        std::move(bodyNodesShared)
    );
}

std::unique_ptr<ASTNode> Parser::parse_return() {
    eat(TokenType::RETURN);
    auto node = expr();
    return std::make_unique<ReturnNode>(std::move(node));
}