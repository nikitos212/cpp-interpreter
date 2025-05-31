#pragma once
#include "symbol_table/SymbolTable.h"
#include "tokens/tokens.h"
#include <memory>

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual Value get(SymbolTable& symbols, std::ostream& out) = 0;
};

class NumberNode : public ASTNode {
    Value value;
public:
    NumberNode(int v);
    NumberNode(double v);
    Value get(SymbolTable&, std::ostream& out) override;
};

class VariableNode : public ASTNode {
    std::string name;
public:
    VariableNode(const std::string& n);
    Value get(SymbolTable& symbols, std::ostream& out) override;
};

class AssignmentNode : public ASTNode {
    std::string var_name;
    std::unique_ptr<ASTNode> value;
public:
    AssignmentNode(const std::string& name, std::unique_ptr<ASTNode> val);
    Value get(SymbolTable& symbols, std::ostream& out) override;
};

class BinOpNode : public ASTNode {
public:
    BinOpNode(std::unique_ptr<ASTNode> l, TokenType o, std::unique_ptr<ASTNode> r);

    Value get(SymbolTable& symbols, std::ostream& out) override;

private:
    std::unique_ptr<ASTNode> left, right;
    TokenType op;

    template<typename T>
    Value apply_operator(T l, T r) {
        switch (op) {
            case TokenType::PLUS: return l + r;
            case TokenType::MINUS: return l - r;
            case TokenType::MULTIPLY: return l * r;
            case TokenType::DIVIDE:
                if (r == 0) throw std::runtime_error("Division by zero");
                return l / r;
            case TokenType::EQUAL_EQUAL: return l == r;
            case TokenType::NOT_EQUAL:   return l != r;
            case TokenType::LESS:        return l < r;
            case TokenType::GREATER:     return l > r;
            case TokenType::LESS_EQUAL:  return l <= r;
            case TokenType::GREATER_EQUAL: return l >= r;
            default: throw std::runtime_error("Invalid operator");
        }
    }
};

class PrintNode : public ASTNode {
    std::unique_ptr<ASTNode> expr;
public:
    PrintNode(std::unique_ptr<ASTNode> e);
    Value get(SymbolTable& symbols, std::ostream& out) override;
};

class IfNode : public ASTNode {
public:
    struct ElseIfBranch {
        std::unique_ptr<ASTNode> condition;
        std::vector<std::unique_ptr<ASTNode>> body;
    };

private:
    std::unique_ptr<ASTNode> condition;
    std::vector<std::unique_ptr<ASTNode>> then_branch;
    std::vector<ElseIfBranch> else_if_branches;
    std::vector<std::unique_ptr<ASTNode>> else_branch;
    
public:
    IfNode(std::unique_ptr<ASTNode> cond, 
           std::vector<std::unique_ptr<ASTNode>> then_exprs,
           std::vector<ElseIfBranch> else_ifs,
           std::vector<std::unique_ptr<ASTNode>> else_exprs);
    
    Value get(SymbolTable& symbols, std::ostream& out) override;
};

class StringNode : public ASTNode {
    std::string value;
public:
    StringNode(const std::string& val);
    Value get(SymbolTable&, std::ostream&) override;
};