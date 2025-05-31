#include "SymbolTable.h"
#include <stdexcept>

SymbolTable::SymbolTable() {
        scopes.emplace_back();
}
    
void SymbolTable::push_scope() {
    scopes.emplace_back();
}
    
void SymbolTable::pop_scope() {
    if (scopes.size() > 1) {
        scopes.pop_back();
    }
}

void SymbolTable::add_variable(const std::string& name, Value value) {
    scopes.back().variables[name] = value;
}

Value SymbolTable::get_variable(const std::string& name) const {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        if (it->variables.count(name)) return it->variables.at(name);
    }
    throw std::runtime_error("Undefined variable: " + name);
}

SymbolTable SymbolTable::create_child() {
    SymbolTable child;
    child.scopes = this->scopes; 
    child.push_scope();
    return child;
}