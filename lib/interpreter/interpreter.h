#pragma once
#include <iostream>
#include <cctype>
#include "ast/nodes.h"


class Interpreter;

class Interpreter {
    SymbolTable symbol_table;
    std::ostream& output;

public:
    Interpreter(std::ostream& out);

    Value interpr(const std::string& text);
};

bool interpret(std::istream& input, std::ostream& output);