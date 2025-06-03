#include "nodes.h"
#include "tokens/tokens.h"
#include <iostream>

using Value = std::variant<int, double, std::string, bool>;

std::ostream& operator<<(std::ostream& os, const Value& value) {
    std::visit([&os](auto&& arg) { os << arg; }, value);
    return os;
}


NumberNode::NumberNode(int v) : value(v) {}
NumberNode::NumberNode(double v) : value(v) {}
Value NumberNode::get(SymbolTable&, std::ostream& out)  { return value; }


VariableNode::VariableNode(const std::string& n) : name(n) {}
Value VariableNode::get(SymbolTable& symbols, std::ostream& out) { return symbols.get_variable(name); }


AssignmentNode::AssignmentNode(const std::string& name, std::unique_ptr<ASTNode> val) : var_name(name), value(std::move(val)) {}
Value AssignmentNode::get(SymbolTable& symbols, std::ostream& out) {
    Value val = value->get(symbols, out);
    symbols.add_variable(var_name, val);
    return val;
}

BinOpNode::BinOpNode(std::unique_ptr<ASTNode> l, TokenType o, std::unique_ptr<ASTNode> r)
    : left(std::move(l)), op(o), right(std::move(r)) {}

Value BinOpNode::get(SymbolTable& symbols, std::ostream& out) {
    Value lval = left->get(symbols, out);
    Value rval = right->get(symbols, out);
    if (std::holds_alternative<int>(lval) && std::holds_alternative<int>(rval) && op == TokenType::REM) {
        return std::get<int>(lval) % std::get<int>(rval);
    }
    if (std::holds_alternative<int>(lval)) {
        if (std::holds_alternative<int>(rval)) return apply_operator(std::get<int>(lval), std::get<int>(rval));
        if (std::holds_alternative<double>(rval)) return apply_operator(static_cast<double>(std::get<int>(lval)), std::get<double>(rval));
    }
    if (std::holds_alternative<double>(lval)) {
        if (std::holds_alternative<int>(rval)) return apply_operator(std::get<double>(lval), static_cast<double>(std::get<int>(rval)));
        if (std::holds_alternative<double>(rval)) return apply_operator(std::get<double>(lval), std::get<double>(rval));
    }
    if (std::holds_alternative<int>(lval) && std::holds_alternative<std::string>(rval) && op == TokenType::MULTIPLY) {
        return std::get<std::string>(rval) * std::get<int>(lval);
    }
    if (std::holds_alternative<int>(rval) && std::holds_alternative<std::string>(lval) && op == TokenType::MULTIPLY) {
        return std::get<std::string>(lval) * std::get<int>(rval);
    }

    throw std::runtime_error("Type mismatch in binary operation");
}


PrintNode::PrintNode(std::unique_ptr<ASTNode> e) : expr(std::move(e)) {}
Value PrintNode::get(SymbolTable& symbols, std::ostream& out) {
    Value val = expr->get(symbols, out);
    out << val;
    
    return val;
}


IfNode::IfNode(std::unique_ptr<ASTNode> cond, 
               std::vector<std::unique_ptr<ASTNode>> then_exprs,
               std::vector<ElseIfBranch> else_ifs,
               std::vector<std::unique_ptr<ASTNode>> else_exprs)
    : condition(std::move(cond)),
      then_branch(std::move(then_exprs)),
      else_if_branches(std::move(else_ifs)),
      else_branch(std::move(else_exprs)) 
{}

Value IfNode::get(SymbolTable& symbols, std::ostream& out) {
    Value cond_val = condition->get(symbols, out);
    
    if (is_truthy(cond_val)) {
        SymbolTable then_scope = symbols.create_child();
        for (auto& expr : then_branch) {
            expr->get(then_scope, out);
        }
        return {};
    }
    
    for (auto& elif : else_if_branches) {
        Value elif_val = elif.condition->get(symbols, out);
        if (is_truthy(elif_val)) {
            SymbolTable elif_scope = symbols.create_child();
            for (auto& expr : elif.body) {
                expr->get(elif_scope, out);
            }
            return {};
        }
    }
    
    if (!else_branch.empty()) {
        SymbolTable else_scope = symbols.create_child();
        for (auto& expr : else_branch) {
            expr->get(else_scope, out);
        }
    }
    
    return {};
}

StringNode::StringNode(const std::string& val) : value(val) {}
Value StringNode::get(SymbolTable&, std::ostream&) {
    return value;
}

BoolNode::BoolNode(const std::string& val) {
    if (val == "true") value = true;
    else value = false;
}
Value BoolNode::get(SymbolTable&, std::ostream&) {
    return value;
}

Value ForNode::get(SymbolTable& symbols, std::ostream& out) {
    Value s_val = start_expr->get(symbols, out);
    Value e_val = end_expr->get(symbols, out);
    Value t_val = step_expr->get(symbols, out);

    int s, e, st;
    if (std::holds_alternative<int>(s_val)) {
        s = std::get<int>(s_val);
    } else if (std::holds_alternative<double>(s_val)) {
        s = static_cast<int>(std::get<double>(s_val));
    } else {
        throw std::runtime_error("Range start is not a number");
    }

    if (std::holds_alternative<int>(e_val)) {
        e = std::get<int>(e_val);
    } else if (std::holds_alternative<double>(e_val)) {
        e = static_cast<int>(std::get<double>(e_val));
    } else {
        throw std::runtime_error("Range end is not a number");
    }

    if (std::holds_alternative<int>(t_val)) {
        st = std::get<int>(t_val);
    } else if (std::holds_alternative<double>(t_val)) {
        st = static_cast<int>(std::get<double>(t_val));
    } else {
        throw std::runtime_error("Range step is not a number");
    }
    if (st == 0) {
        throw std::runtime_error("Range step cannot be zero");
    }

    if (st > 0) {
        for (int i = s; i < e; i += st) {
            symbols.add_variable(var_name, i);
            for (auto& stmt : body) {
                stmt->get(symbols, out);
            }
        }
    } else {
        for (int i = s; i > e; i += st) {
            symbols.add_variable(var_name, i);
            for (auto& stmt : body) {
                stmt->get(symbols, out);
            }
        }
    }
    return Value{}; 
}

Value LenNode::get(SymbolTable& symbols, std::ostream& out) {
    Value v = expr->get(symbols, out);
    if (!std::holds_alternative<std::string>(v)) {
        throw std::runtime_error("len() argument must be a string");
    }
    const std::string& s = std::get<std::string>(v);
    return static_cast<int>(s.size());
}

Value WhileNode::get(SymbolTable& symbols, std::ostream& out) {
    Value cond_val = condition->get(symbols, out);
    while (is_truthy(cond_val)) {
        for (auto& stmt : body) {
            stmt->get(symbols, out);
        }
        cond_val = condition->get(symbols, out);
    }
    return Value{};
}