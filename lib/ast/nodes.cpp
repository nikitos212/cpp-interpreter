#include "nodes.h"
#include "tokens/tokens.h"
#include <cstddef>
#include <iostream>
#include <memory>

std::ostream& operator<<(std::ostream& os, const Value& v) {
    std::visit([&](auto&& x){
        using T = std::decay_t<decltype(x)>;
        if constexpr (std::is_same_v<T, Nil>) {
            os << "nil";
        } else if constexpr (std::is_same_v<T, bool>) {
            if (x == true) os << "true";
            else os << "false";
        } else {
            os << x;
        }
    }, v);
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

    if (op == TokenType::POW) {
        if (std::holds_alternative<int>(lval) && std::holds_alternative<int>(rval)) {
            int base = std::get<int>(lval);
            int exponent = std::get<int>(rval);
            int result = 1;
            for (int i = 0; i < exponent; ++i) {
                result = result * base;
            }
            return result;
        }
        double base_d, exp_d;
        if (std::holds_alternative<int>(lval))
            base_d = static_cast<double>(std::get<int>(lval));
        else if (std::holds_alternative<double>(lval))
            base_d = std::get<double>(lval);
        else
            throw std::runtime_error("Bad types for '^' operator");

        if (std::holds_alternative<int>(rval))
            exp_d = static_cast<double>(std::get<int>(rval));
        else if (std::holds_alternative<double>(rval))
            exp_d = std::get<double>(rval);
        else
            throw std::runtime_error("Bad types for '^' operator");

        return pow(base_d, exp_d);
    }

    if (op == TokenType::MULTIPLY) {
        if (std::holds_alternative<std::string>(lval)) {
            std::string a = std::get<std::string>(lval);
            if (std::holds_alternative<int>(rval)) {
                return a * std::get<int>(rval);
            }
            if (std::holds_alternative<bool>(rval)) {
                return a * std::get<bool>(rval);
            }
        }
        if (std::holds_alternative<int>(lval)) {
            int a = std::get<int>(lval);
            if (std::holds_alternative<int>(rval)) {
                return a * std::get<int>(rval);
            }
            if (std::holds_alternative<double>(rval)) {
                return static_cast<double>(a) * std::get<double>(rval);
            }
            if (std::holds_alternative<std::string>(rval)) {
                return a * std::get<std::string>(rval);
            }
            if (std::holds_alternative<bool>(rval)) {
                return a * std::get<bool>(rval);
            }
        }
        if (std::holds_alternative<double>(lval)) {
            double a = std::get<double>(lval);
            if (std::holds_alternative<int>(rval)) {
                return a * static_cast<double>(std::get<int>(rval));
            }
            if (std::holds_alternative<double>(rval)) {
                return a * std::get<double>(rval);
            }
            if (std::holds_alternative<bool>(rval)) {
                return a * std::get<bool>(rval);
            }
        }
        if (std::holds_alternative<bool>(lval)) {
            bool a = std::get<bool>(lval);
            if (std::holds_alternative<int>(rval)) {
                return a * std::get<int>(rval);
            }
            if (std::holds_alternative<double>(rval)) {
                return a * std::get<double>(rval);
            }
            if (std::holds_alternative<bool>(rval)) {
                return a * std::get<bool>(rval);
            }
        }
        throw std::runtime_error("Bad types for '*'");
    }

    if (op == TokenType::AND) {
        bool a = is_truthy(lval);
        bool b = is_truthy(rval);
        return a && b;
    }
    if (op == TokenType::OR) {
        bool a = is_truthy(lval);
        bool b = is_truthy(rval);
        return a || b;
    }


    if (op == TokenType::PLUS) {
        if (std::holds_alternative<std::string>(lval) && std::holds_alternative<std::string>(rval)) {
            return std::get<std::string>(lval) + std::get<std::string>(rval);
        }
        if (std::holds_alternative<int>(lval)) {
            int a = std::get<int>(lval);
            if (std::holds_alternative<int>(rval)) {
                return a + std::get<int>(rval);
            }
            if (std::holds_alternative<double>(rval)) {
                return static_cast<double>(a) + std::get<double>(rval);
            }
        }
        if (std::holds_alternative<double>(lval)) {
            double a = std::get<double>(lval);
            if (std::holds_alternative<int>(rval)) {
                return a + static_cast<double>(std::get<int>(rval));
            }
            if (std::holds_alternative<double>(rval)) {
                return a + std::get<double>(rval);
            }
        }
        throw std::runtime_error("Bad types for '+'");
    }

    if (op == TokenType::MINUS) {
        if (std::holds_alternative<int>(lval)) {
            int a = std::get<int>(lval);
            if (std::holds_alternative<int>(rval)) {
                return a - std::get<int>(rval);
            }
            if (std::holds_alternative<double>(rval)) {
                return static_cast<double>(a) - std::get<double>(rval);
            }
        }
        if (std::holds_alternative<double>(lval)) {
            double a = std::get<double>(lval);
            if (std::holds_alternative<int>(rval)) {
                return a - static_cast<double>(std::get<int>(rval));
            }
            if (std::holds_alternative<double>(rval)) {
                return a - std::get<double>(rval);
            }
        }
        throw std::runtime_error("Bad types for '-'");
    }

    if (op == TokenType::DIVIDE) {
        if (std::holds_alternative<int>(lval)) {
            int a = std::get<int>(lval);
            if (std::holds_alternative<int>(rval)) {
                int b = std::get<int>(rval);
                if (b == 0) throw std::runtime_error("Division by zero");
                return a / b;
            }
            if (std::holds_alternative<double>(rval)) {
                double b = std::get<double>(rval);
                if (b == 0.0) throw std::runtime_error("Division by zero");
                return static_cast<double>(a) / b;
            }
        }
        if (std::holds_alternative<double>(lval)) {
            double a = std::get<double>(lval);
            if (std::holds_alternative<int>(rval)) {
                int b = std::get<int>(rval);
                if (b == 0) throw std::runtime_error("Division by zero");
                return a / static_cast<double>(b);
            }
            if (std::holds_alternative<double>(rval)) {
                double b = std::get<double>(rval);
                if (b == 0.0) throw std::runtime_error("Division by zero");
                return a / b;
            }
        }
        throw std::runtime_error("Bad types for '/'");
    }

    if (op == TokenType::REM) {
        if (std::holds_alternative<int>(lval)) {
            int a = std::get<int>(lval);
            if (std::holds_alternative<int>(rval)) {
                int b = std::get<int>(rval);
                if (b == 0) throw std::runtime_error("Division by zero");
                return a % b;
            }
            if (std::holds_alternative<bool>(rval)) {
                bool b = std::get<bool>(rval);
                if (b == false) throw std::runtime_error("Division by zero");
                return a % b;
            }
        }
        if (std::holds_alternative<bool>(lval)) {
            int a = std::get<int>(lval);
            if (std::holds_alternative<int>(rval)) {
                int b = std::get<int>(rval);
                if (b == 0) throw std::runtime_error("Division by zero");
                return a % b;
            }
            if (std::holds_alternative<bool>(rval)) {
                bool b = std::get<bool>(rval);
                if (b == false) throw std::runtime_error("Division by zero");
                return a % b;
            }
        }
        
        throw std::runtime_error("Bad types for '/'");
    }

    if (op == TokenType::MOD_EQUAL) {
        if (std::holds_alternative<int>(lval) && std::holds_alternative<int>(rval)) {
            int a = std::get<int>(lval);
            int b = std::get<int>(rval);
            if (b == 0) throw std::runtime_error("Modulo by zero");
            return a % b;
        }
        throw std::runtime_error("Bad types for '%'");
    }

    if (op == TokenType::EQUAL_EQUAL) {
        if (std::holds_alternative<int>(lval) && std::holds_alternative<int>(rval)) {
        return std::get<int>(lval) == std::get<int>(rval);
        }
        if (std::holds_alternative<double>(lval) && std::holds_alternative<double>(rval)) {
            return std::get<double>(lval) == std::get<double>(rval);
        }
        if (std::holds_alternative<int>(lval) && std::holds_alternative<double>(rval)) {
            return static_cast<double>(std::get<int>(lval)) == std::get<double>(rval);
        }
        if (std::holds_alternative<double>(lval) && std::holds_alternative<int>(rval)) {
            return std::get<double>(lval) == static_cast<double>(std::get<int>(rval));
        }
        if (std::holds_alternative<bool>(lval) && std::holds_alternative<bool>(rval)) {
            return std::get<bool>(lval) == std::get<bool>(rval);
        }
        if (std::holds_alternative<std::string>(lval) && std::holds_alternative<std::string>(rval)) {
            return std::get<std::string>(lval) == std::get<std::string>(rval);
        }
        if (std::holds_alternative<FunctionValue>(lval) && std::holds_alternative<FunctionValue>(rval)) {
            throw std::runtime_error("Cannot compare functions with == ");
        }
        if (std::holds_alternative<Nil>(lval) && std::holds_alternative<Nil>(rval)) {
            return true;
        }
        if (std::holds_alternative<Nil>(lval) && !std::holds_alternative<Nil>(rval)) {
            return false;
        }
        if (!std::holds_alternative<Nil>(lval) && std::holds_alternative<Nil>(rval)) {
            return false;
        }
        throw std::runtime_error("Type mismatch in '==' operation");
    }
    if (op == TokenType::NOT_EQUAL) {
         if (std::holds_alternative<int>(lval) && std::holds_alternative<int>(rval)) {
        return std::get<int>(lval) != std::get<int>(rval);
        }
        if (std::holds_alternative<double>(lval) && std::holds_alternative<double>(rval)) {
            return std::get<double>(lval) != std::get<double>(rval);
        }
        if (std::holds_alternative<int>(lval) && std::holds_alternative<double>(rval)) {
            return static_cast<double>(std::get<int>(lval)) != std::get<double>(rval);
        }
        if (std::holds_alternative<double>(lval) && std::holds_alternative<int>(rval)) {
            return std::get<double>(lval) != static_cast<double>(std::get<int>(rval));
        }
        if (std::holds_alternative<bool>(lval) && std::holds_alternative<bool>(rval)) {
            return std::get<bool>(lval) != std::get<bool>(rval);
        }
        if (std::holds_alternative<std::string>(lval) && std::holds_alternative<std::string>(rval)) {
            return std::get<std::string>(lval) != std::get<std::string>(rval);
        }
        if (std::holds_alternative<FunctionValue>(lval) && std::holds_alternative<FunctionValue>(rval)) {
            throw std::runtime_error("Cannot compare functions with == ");
        }
        throw std::runtime_error("Type mismatch in '==' operation");
    }
    if (op == TokenType::LESS) {
        if (std::holds_alternative<int>(lval) && std::holds_alternative<int>(rval)) {
            return std::get<int>(lval) < std::get<int>(rval);
        }
        if (std::holds_alternative<double>(lval) && std::holds_alternative<double>(rval)) {
            return std::get<double>(lval) < std::get<double>(rval);
        }
        throw std::runtime_error("Bad types for '<'");
    }
    if (op == TokenType::GREATER) {
        if (std::holds_alternative<int>(lval) && std::holds_alternative<int>(rval)) {
            return std::get<int>(lval) > std::get<int>(rval);
        }
        if (std::holds_alternative<double>(lval) && std::holds_alternative<double>(rval)) {
            return std::get<double>(lval) > std::get<double>(rval);
        }
        throw std::runtime_error("Bad types for '>'");
    }
    if (op == TokenType::LESS_EQUAL) {
        if (std::holds_alternative<int>(lval) && std::holds_alternative<int>(rval)) {
            return std::get<int>(lval) <= std::get<int>(rval);
        }
        if (std::holds_alternative<double>(lval) && std::holds_alternative<double>(rval)) {
            return std::get<double>(lval) <= std::get<double>(rval);
        }
        throw std::runtime_error("Bad types for '<='");
    }
    if (op == TokenType::GREATER_EQUAL) {
        if (std::holds_alternative<int>(lval) && std::holds_alternative<int>(rval)) {
            return std::get<int>(lval) >= std::get<int>(rval);
        }
        if (std::holds_alternative<double>(lval) && std::holds_alternative<double>(rval)) {
            return std::get<double>(lval) >= std::get<double>(rval);
        }
        throw std::runtime_error("Bad types for '>='");
    }

    throw std::runtime_error("Unknown binary operator or bad types");
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
        for (auto& expr : then_branch) {
            expr->get(symbols, out);
        }
        return {};
    }
    
    for (auto& elif : else_if_branches) {
        Value elif_val = elif.condition->get(symbols, out);
        if (is_truthy(elif_val)) {
            for (auto& expr : elif.body) {
                expr->get(symbols, out);
            }
            return {};
        }
    }
    
    if (!else_branch.empty()) {
        for (auto& expr : else_branch) {
            expr->get(symbols, out);
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

Value NilNode::get(SymbolTable&, std::ostream&) {
    return Nil{};
}

Value ForNode::get(SymbolTable& symbols, std::ostream& out) {
    if (iterable_expr == nullptr) {
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
    } else {
        Value iter = iterable_expr->get(symbols, out);
        if (auto p = std::get_if<std::shared_ptr<ListValue>>(&iter)) {
        for (auto& v : (*p)->items) {
            symbols.add_variable(var_name, v);
            for (auto& stmt : body) {
                stmt->get(symbols, out);
            }
        }
        return {};
    }

        if (std::holds_alternative<std::string>(iter)) {
            const auto& s = std::get<std::string>(iter);
            for (char c : s) {
                SymbolTable child = symbols.create_child();
                child.add_variable(var_name, std::string(1, c));
                for (auto& stmt : body) {
                    stmt->get(child, out);
                }
            }
            return {};
        }

        throw std::runtime_error("Cannot iterate over non-list/string value in for-loop");
    }
}

Value LenNode::get(SymbolTable& symbols, std::ostream& out) {
    Value v = expr->get(symbols, out);
    if (std::holds_alternative<std::string>(v)) {
        const std::string& s = std::get<std::string>(v);
        return static_cast<int>(s.size());
    } else if (std::holds_alternative<std::shared_ptr<ListValue>>(v)) {
        return static_cast<int>(std::get<std::shared_ptr<ListValue>>(v)->items.size());
    }
    throw std::runtime_error("len() argument must be a string");
    
}

Value MaxNode::get(SymbolTable& symbols, std::ostream& out) {
    Value v = expr->get(symbols, out);
    if (std::holds_alternative<std::shared_ptr<ListValue>>(v)) {
        auto& lst = std::get<std::shared_ptr<ListValue>>(v);
        int max = INT_MIN;
        for (auto& i : lst->items) {
            if (std::holds_alternative<int>(i) && max < std::get<int>(i)) max = std::get<int>(i);
        }

        return max;
    }

    throw std::runtime_error("max() argument must be a list");
}

Value MinNode::get(SymbolTable& symbols, std::ostream& out) {
    Value v = expr->get(symbols, out);
    if (std::holds_alternative<std::shared_ptr<ListValue>>(v)) {
        auto& lst = std::get<std::shared_ptr<ListValue>>(v);
        int min = INT_MAX;
        for (auto& i : lst->items) {
            if (std::holds_alternative<int>(i) && min > std::get<int>(i)) min = std::get<int>(i);
        }

        return min;
    }

    throw std::runtime_error("max() argument must be a list");
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

ReturnNode::ReturnNode(std::unique_ptr<ASTNode> e) : expr(std::move(e)) {}
Value ReturnNode::get(SymbolTable& symbols, std::ostream& out) {
    Value v = expr->get(symbols, out);
    throw ReturnException(std::move(v));
}

FunctionNode::FunctionNode(std::vector<std::string> p,
                           std::vector<std::shared_ptr<ASTNode>> b)
    : params(std::move(p)), body(std::move(b)) {}

Value FunctionNode::get(SymbolTable& symbols, std::ostream& out) {
    FunctionValue fv;
    fv.params = params;
    fv.body   = body;
    return fv;
}

CallNode::CallNode(std::unique_ptr<ASTNode> f,
                   std::vector<std::unique_ptr<ASTNode>> a)
    : funcExpr(std::move(f)), args(std::move(a)) {}

Value CallNode::get(SymbolTable& symbols, std::ostream& out) {
    Value fval = funcExpr->get(symbols, out);

    if (!std::holds_alternative<FunctionValue>(fval)) {
        throw std::runtime_error("Attempt to call a non-function value");
    }
    FunctionValue fv = std::get<FunctionValue>(std::move(fval));

    if (args.size() != fv.params.size()) {
        throw std::runtime_error("Function called with wrong number of arguments");
    }

    SymbolTable local = symbols.create_child();

    for (size_t i = 0; i < args.size(); ++i) {
        Value aval = args[i]->get(symbols, out);
        local.add_variable(fv.params[i], std::move(aval));
    }

    try {
        for (auto& stmt_ptr : fv.body) {
            stmt_ptr->get(local, out);
        }
    } catch (const ReturnException& rex) {
        return rex.value;
    }

    return Value{0};
}

static int to_int(const Value& v) {
    if (std::holds_alternative<int>(v))       return std::get<int>(v);
    if (std::holds_alternative<double>(v))    return static_cast<int>(std::get<double>(v));
    if (std::holds_alternative<bool>(v))      return std::get<bool>(v) ? 1 : 0;
    if (std::holds_alternative<std::string>(v)) return std::stoi(std::get<std::string>(v));
    throw std::runtime_error("Cannot convert to int");
}

Value ListNode::get(SymbolTable& symbols, std::ostream& out) {
    auto list = std::make_shared<ListValue>();
    for (auto& elem : elements) {
        list->items.push_back(elem->get(symbols, out));
    }
    return list;
}


Value IndexNode::get(SymbolTable& symbols, std::ostream& out) {
    Value container_val = container->get(symbols, out);
    Value idx_val = index->get(symbols, out);
    int idx = to_int(idx_val);

    if (std::holds_alternative<std::shared_ptr<ListValue>>(container_val)) {
        const auto& lv = std::get<std::shared_ptr<ListValue>>(container_val);

        if (idx < 0 || idx >= static_cast<int>(lv->items.size()))
            throw std::runtime_error("List index out of range");

        const auto& element = lv->items[idx];

        if (std::holds_alternative<int>(element)) {
            return std::get<int>(element);
        }
        if (std::holds_alternative<double>(element)) {
            return std::get<double>(element);
        }
        if (std::holds_alternative<std::string>(element)) {
            return std::get<std::string>(element);
        }
        if (std::holds_alternative<bool>(element)) {
            return std::get<bool>(element);
        }
        if (std::holds_alternative<FunctionValue>(element)) {
            return std::get<FunctionValue>(element);
        }

        throw std::runtime_error("IndexNode: unexpected variant alternative");
    }

    if (std::holds_alternative<std::string>(container_val)) {
        const std::string& s = std::get<std::string>(container_val);
        if (idx < 0 || idx >= static_cast<int>(s.size()))
            throw std::runtime_error("String index out of range");
        return std::string(1, s[idx]);
    }

    throw std::runtime_error("Indexing non-list/string value");
}


Value SliceNode::get(SymbolTable& symbols, std::ostream& out) {
    Value container_val = container->get(symbols, out);
    int start_idx = to_int(start->get(symbols, out));
    int end_idx   = to_int(  end->get(symbols, out) );

    if (std::holds_alternative<std::shared_ptr<ListValue>>(container_val)) {
        auto& lst = std::get<std::shared_ptr<ListValue>>(container_val);
        if (start_idx < 0) start_idx = 0;
        if (end_idx > (int)lst->items.size()) end_idx = lst->items.size();
        if (start_idx > end_idx) start_idx = end_idx;
        auto slice = std::make_shared<ListValue>();
        for (int i = start_idx; i < end_idx; ++i) {
            slice->items.push_back(lst->items[i]);
        }

        return slice;
    }
    if (std::holds_alternative<std::string>(container_val)) {
        auto& s = std::get<std::string>(container_val);
        if (start_idx < 0) start_idx = 0;
        if (end_idx > (int)s.size()) end_idx = s.size();
        if (start_idx > end_idx) start_idx = end_idx;
        return s.substr(start_idx, end_idx - start_idx);
    }
    throw std::runtime_error("Slicing non-list/string value");
}
