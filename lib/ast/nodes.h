#pragma once
#include "tokens/tokens.h"
#include <memory>
#include <unordered_map>
#include <variant>
#include <vector>

class ASTNode;

struct FunctionValue {
    std::vector<std::string> params;
    std::vector<std::shared_ptr<ASTNode>> body;

    bool operator==(const FunctionValue& other) { return false; }
    bool operator!=(const FunctionValue& other) { return false; }
};

using List = std::vector<std::variant<int, double, std::string, bool, FunctionValue>>;

using ListValue = std::vector<std::variant<int, double, std::string, bool, FunctionValue, List>>;

using Value = std::variant<int, double, std::string, bool, FunctionValue, ListValue>;

class SymbolTable {
    struct Scope {
        std::unordered_map<std::string, Value> variables;
    };
    std::vector<Scope> scopes;

public:
    SymbolTable();

    SymbolTable create_child();

    void add_variable(const std::string& name, Value value);

    Value get_variable(const std::string& name) const ;

    void push_scope();
    
    void pop_scope();

};

inline SymbolTable::SymbolTable() {
        scopes.emplace_back();
}
    
inline void SymbolTable::push_scope() {
    scopes.emplace_back();
}
    
inline void SymbolTable::pop_scope() {
    if (scopes.size() > 1) {
        scopes.pop_back();
    }
}

inline void SymbolTable::add_variable(const std::string& name, Value value) {
    scopes.back().variables[name] = value;
}

inline Value SymbolTable::get_variable(const std::string& name) const {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        if (it->variables.count(name)) return it->variables.at(name);
    }
    throw std::runtime_error("Undefined variable: " + name);
}

inline SymbolTable SymbolTable::create_child() {
    SymbolTable child;
    child.scopes = this->scopes; 
    child.push_scope();
    return child;
}

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
            case TokenType::POW: return pow(l, r);
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

class BoolNode : public ASTNode {
    bool value;
public:
    BoolNode(const std::string& val);
    Value get(SymbolTable&, std::ostream&) override;
};

class ForNode : public ASTNode {
    std::string var_name;
    std::unique_ptr<ASTNode> start_expr;
    std::unique_ptr<ASTNode> end_expr;
    std::unique_ptr<ASTNode> step_expr;
    std::vector<std::unique_ptr<ASTNode>> body;

public:
    ForNode(std::string var,
            std::unique_ptr<ASTNode> start,
            std::unique_ptr<ASTNode> end,
            std::unique_ptr<ASTNode> step,
            std::vector<std::unique_ptr<ASTNode>> body_nodes)
        : var_name(std::move(var)),
          start_expr(std::move(start)),
          end_expr(std::move(end)),
          step_expr(std::move(step)),
          body(std::move(body_nodes)) {}

    Value get(SymbolTable& symbols, std::ostream& out) override;
};

class LenNode : public ASTNode {
    std::unique_ptr<ASTNode> expr;
public:
    LenNode(std::unique_ptr<ASTNode> e) : expr(std::move(e)) {}
    Value get(SymbolTable& symbols, std::ostream& out) override;
};

class WhileNode : public ASTNode {
    std::unique_ptr<ASTNode> condition;
    std::vector<std::unique_ptr<ASTNode>> body;
public:
    WhileNode(std::unique_ptr<ASTNode> cond,
              std::vector<std::unique_ptr<ASTNode>> body_nodes)
        : condition(std::move(cond)),
          body(std::move(body_nodes)) {}
    Value get(SymbolTable& symbols, std::ostream& out) override;
};

class FunctionNode : public ASTNode {
public:
    std::vector<std::string> params;
    std::vector<std::shared_ptr<ASTNode>> body;

    FunctionNode(std::vector<std::string> p,
                 std::vector<std::shared_ptr<ASTNode>> b);
    Value get(SymbolTable& symbols, std::ostream& out) override;
};


class CallNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> funcExpr;
    std::vector<std::unique_ptr<ASTNode>> args;

    CallNode(std::unique_ptr<ASTNode> f,
             std::vector<std::unique_ptr<ASTNode>> a);
    Value get(SymbolTable& symbols, std::ostream& out) override;
};

class ReturnNode : public ASTNode {
    std::unique_ptr<ASTNode> expr;
public:
    ReturnNode(std::unique_ptr<ASTNode> e);
    Value get(SymbolTable& symbols, std::ostream& out) override;
};

struct ReturnException {
    Value value;
    ReturnException(Value v) : value(std::move(v)) {}
};

inline bool is_truthy(const Value& val) {
    if (std::holds_alternative<bool>(val)) {
        return std::get<bool>(val);
    }
    if (std::holds_alternative<int>(val)) {
        return std::get<int>(val) != 0;
    }
    if (std::holds_alternative<double>(val)) {
        return std::get<double>(val) != 0.0;
    }
    if (std::holds_alternative<std::string>(val)) {
        return !std::get<std::string>(val).empty();
    }
    return true;
}

class ListNode : public ASTNode {
    std::vector<std::unique_ptr<ASTNode>> elements;
public:
    ListNode(std::vector<std::unique_ptr<ASTNode>> elems)
      : elements(std::move(elems)) {}
    Value get(SymbolTable& symbols, std::ostream& out) override;
};

class IndexNode : public ASTNode {
    std::unique_ptr<ASTNode> container;
    std::unique_ptr<ASTNode> index;
public:
    IndexNode(std::unique_ptr<ASTNode> c, std::unique_ptr<ASTNode> i)
      : container(std::move(c)), index(std::move(i)) {}
    Value get(SymbolTable& symbols, std::ostream& out) override;
};

class SliceNode : public ASTNode {
    std::unique_ptr<ASTNode> container;
    std::unique_ptr<ASTNode> start;
    std::unique_ptr<ASTNode> end;
public:
    SliceNode(std::unique_ptr<ASTNode> c,
              std::unique_ptr<ASTNode> s,
              std::unique_ptr<ASTNode> e)
      : container(std::move(c)), start(std::move(s)), end(std::move(e)) {}
    Value get(SymbolTable& symbols, std::ostream& out) override;
};