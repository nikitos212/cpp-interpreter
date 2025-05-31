#pragma once
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

using Value = std::variant<int, double, std::string, bool>;

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

