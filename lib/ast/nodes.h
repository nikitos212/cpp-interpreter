#pragma once
#include "tokens/tokens.h"
#include "interpreter/call_stack.h"
#include <memory>
#include <unordered_map>
#include <variant>
#include <vector>

struct ASTNode;
struct ListValue;
struct Nil;
struct FunctionValue;  

struct Nil { };

using Value = std::variant<
    int,
    double,
    std::shared_ptr<std::string>,
    bool,
    FunctionValue,
    std::shared_ptr<ListValue>,
    Nil
>;

struct FunctionValue {
    std::vector<std::string> params;
    std::vector<std::shared_ptr<ASTNode>> body;

    bool operator==(const FunctionValue& other) { return false; }
    bool operator!=(const FunctionValue& other) { return false; }
};

struct ListValue;

struct ListValue {
    std::vector<Value> items;
};

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
    std::string& get_name();
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

class PrintlnNode : public ASTNode {
    std::unique_ptr<ASTNode> expr;
public:
    PrintlnNode(std::unique_ptr<ASTNode> e);
    Value get(SymbolTable& symbols, std::ostream& out) override;
};

class ReadNode : public ASTNode {
    std::string expr;
public:
    ReadNode();
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

class NilNode : public ASTNode {
public:
    NilNode() {};
    Value get(SymbolTable&, std::ostream&) override;
};

class ForNode : public ASTNode {
    std::string var_name;
    std::unique_ptr<ASTNode> start_expr;
    std::unique_ptr<ASTNode> end_expr;
    std::unique_ptr<ASTNode> step_expr;
    std::unique_ptr<ASTNode> iterable_expr;
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
          
    ForNode(std::string var,
            std::unique_ptr<ASTNode> iterable_node,
            std::vector<std::unique_ptr<ASTNode>> body_nodes)
        : var_name(std::move(var)),
          iterable_expr(std::move(iterable_node)),
          body(std::move(body_nodes)) {}

    Value get(SymbolTable& symbols, std::ostream& out) override;
};

class LenNode : public ASTNode {
    std::unique_ptr<ASTNode> expr;
public:
    LenNode(std::unique_ptr<ASTNode> e) : expr(std::move(e)) {}
    Value get(SymbolTable& symbols, std::ostream& out) override;
};

class MaxNode : public ASTNode {
    std::unique_ptr<ASTNode> expr;
public:
    MaxNode(std::unique_ptr<ASTNode> e) : expr(std::move(e)) {}
    Value get(SymbolTable& symbols, std::ostream& out) override;
};

class MinNode : public ASTNode {
    std::unique_ptr<ASTNode> expr;
public:
    MinNode(std::unique_ptr<ASTNode> e) : expr(std::move(e)) {}
    Value get(SymbolTable& symbols, std::ostream& out) override;
};

class AbsNode : public ASTNode {
    std::unique_ptr<ASTNode> expr;
public:
    AbsNode(std::unique_ptr<ASTNode> e) : expr(std::move(e)) {}
    Value get(SymbolTable& symbols, std::ostream& out) override;
};

class CeilNode : public ASTNode {
    std::unique_ptr<ASTNode> expr;
public:
    CeilNode(std::unique_ptr<ASTNode> e) : expr(std::move(e)) {}
    Value get(SymbolTable& symbols, std::ostream& out) override;
};

class FloorNode : public ASTNode {
    std::unique_ptr<ASTNode> expr;
public:
    FloorNode(std::unique_ptr<ASTNode> e) : expr(std::move(e)) {}
    Value get(SymbolTable& symbols, std::ostream& out) override;
};

class RoundNode : public ASTNode {
    std::unique_ptr<ASTNode> expr;
public:
    RoundNode(std::unique_ptr<ASTNode> e) : expr(std::move(e)) {}
    Value get(SymbolTable& symbols, std::ostream& out) override;
};

class SqrtNode : public ASTNode {
    std::unique_ptr<ASTNode> expr;
public:
    SqrtNode(std::unique_ptr<ASTNode> e) : expr(std::move(e)) {}
    Value get(SymbolTable& symbols, std::ostream& out) override;
};

class RndNode : public ASTNode {
    std::unique_ptr<ASTNode> expr;
public:
    RndNode(std::unique_ptr<ASTNode> e) : expr(std::move(e)) {}
    Value get(SymbolTable& symbols, std::ostream& out) override;
};

class ParseNumNode : public ASTNode {
    std::unique_ptr<ASTNode> expr;
public:
    ParseNumNode(std::unique_ptr<ASTNode> e) : expr(std::move(e)) {}
    Value get(SymbolTable& symbols, std::ostream& out) override;
};

class ToStringNode : public ASTNode {
    std::unique_ptr<ASTNode> expr;
public:
    ToStringNode(std::unique_ptr<ASTNode> e) : expr(std::move(e)) {}
    Value get(SymbolTable& symbols, std::ostream& out) override;
};

class LowerNode : public ASTNode {
    std::unique_ptr<ASTNode> expr;
public:
    LowerNode(std::unique_ptr<ASTNode> e) : expr(std::move(e)) {}
    Value get(SymbolTable& symbols, std::ostream& out) override;
};

class UpperNode : public ASTNode {
    std::unique_ptr<ASTNode> expr;
public:
    UpperNode(std::unique_ptr<ASTNode> e) : expr(std::move(e)) {}
    Value get(SymbolTable& symbols, std::ostream& out) override;
};

class SplitNode : public ASTNode {
    std::unique_ptr<ASTNode> expr;
    std::unique_ptr<ASTNode> delim;
public:
    SplitNode(std::unique_ptr<ASTNode> e, std::unique_ptr<ASTNode> d) : expr(std::move(e)), delim(std::move(d)) {}
    Value get(SymbolTable& symbols, std::ostream& out) override;
};

class JoinNode : public ASTNode {
    std::unique_ptr<ASTNode> expr;
    std::unique_ptr<ASTNode> delim;
public:
    JoinNode(std::unique_ptr<ASTNode> e, std::unique_ptr<ASTNode> d) : expr(std::move(e)), delim(std::move(d)) {}
    Value get(SymbolTable& symbols, std::ostream& out) override;
};

class ReplaceNode : public ASTNode {
    std::unique_ptr<ASTNode> expr;
    std::unique_ptr<ASTNode> old;
    std::unique_ptr<ASTNode> new_s;
public:
    ReplaceNode(std::unique_ptr<ASTNode> e, std::unique_ptr<ASTNode> o, std::unique_ptr<ASTNode> n) : expr(std::move(e)), old(std::move(o)), new_s(std::move(n)) {}
    Value get(SymbolTable& symbols, std::ostream& out) override;
};

class PushNode : public ASTNode {
    std::unique_ptr<ASTNode> list;
    std::unique_ptr<ASTNode> expr;
public:
    PushNode(std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> e) : list(std::move(l)), expr(std::move(e)) {}
    Value get(SymbolTable& symbols, std::ostream& out) override;
};

class PopNode : public ASTNode {
    std::unique_ptr<ASTNode> expr;
public:
    PopNode(std::unique_ptr<ASTNode> e) : expr(std::move(e)) {}
    Value get(SymbolTable& symbols, std::ostream& out) override;
};

class SortNode : public ASTNode {
    std::unique_ptr<ASTNode> expr;
public:
    SortNode(std::unique_ptr<ASTNode> e) : expr(std::move(e)) {}
    Value get(SymbolTable& symbols, std::ostream& out) override;
};

class RemoveNode : public ASTNode {
    std::unique_ptr<ASTNode> expr;
    std::unique_ptr<ASTNode> ind;
public:
    RemoveNode(std::unique_ptr<ASTNode> e, std::unique_ptr<ASTNode> i) : expr(std::move(e)), ind(std::move(i)) {}
    Value get(SymbolTable& symbols, std::ostream& out) override;
};

class InsertNode : public ASTNode {
    std::unique_ptr<ASTNode> expr;
    std::unique_ptr<ASTNode> ind;
    std::unique_ptr<ASTNode> value;
public:
    InsertNode(std::unique_ptr<ASTNode> e, std::unique_ptr<ASTNode> i, std::unique_ptr<ASTNode> v) : expr(std::move(e)), ind(std::move(i)), value(std::move(v)) {}
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

struct BreakException {};

class BreakNode : public ASTNode {
public:
    Value get(SymbolTable&, std::ostream&) override {
        throw BreakException();
    }
};

struct ContinueException {};

class ContinueNode : public ASTNode {
public:
    Value get(SymbolTable&, std::ostream&) override {
        throw ContinueException();
    }
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
    if (std::holds_alternative<std::shared_ptr<std::string>>(val)) {
        return !std::get<std::shared_ptr<std::string>>(val)->empty();
    }
    if (std::holds_alternative<Nil>(val)) {
        return false;
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

class StackTraceNode : public ASTNode {
public:
    StackTraceNode () {}
    Value get(SymbolTable&, std::ostream&) override {
        auto list = std::make_shared<ListValue>();
        for (auto& fn : call_stack) {
            list->items.push_back(std::make_shared<std::string>(fn));
        }
        return list;
    }
};