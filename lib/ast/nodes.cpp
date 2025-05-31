#include "nodes.h"

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
    if (std::holds_alternative<int>(lval)) {
        int l = std::get<int>(lval);
        if (std::holds_alternative<int>(rval)) return apply_operator(l, std::get<int>(rval));
        if (std::holds_alternative<double>(rval)) return apply_operator(static_cast<double>(l), std::get<double>(rval));
    }
    if (std::holds_alternative<double>(lval)) {
        double l = std::get<double>(lval);
        if (std::holds_alternative<int>(rval)) return apply_operator(l, static_cast<double>(std::get<int>(rval)));
        if (std::holds_alternative<double>(rval)) return apply_operator(l, std::get<double>(rval));
    }
    throw std::runtime_error("Type mismatch in binary operation");
}


PrintNode::PrintNode(std::unique_ptr<ASTNode> e) : expr(std::move(e)) {}
Value PrintNode::get(SymbolTable& symbols, std::ostream& out) {
    Value val = expr->get(symbols, out);
    out << val;
    
    return val;
}

bool is_truthy(const Value& val) {
    if (std::holds_alternative<bool>(val)) {
        return std::get<bool>(val);
    }
    if (std::holds_alternative<int>(val)) {
        return std::get<int>(val) != 0;
    }
    if (std::holds_alternative<double>(val)) {
        return std::get<double>(val) != 0.0;
    }
    return true;
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