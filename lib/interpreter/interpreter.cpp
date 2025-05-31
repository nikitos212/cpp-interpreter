#include "interpreter.h"
#include "symbol_table/SymbolTable.h"
#include "parser/parser.h"
#include "ast/nodes.h"

class Interpreter {
    SymbolTable symbol_table;
    std::ostream& output;

public:
    Interpreter(std::ostream& out) : output(out) {}

    Value interpr(const std::string& text) {
        Parser parser(text);
        auto ast = parser.parse();
        return ast->get(symbol_table, output);
    }
};

bool interpret(std::istream& input, std::ostream& output) {
    Interpreter interpreter(output);
    std::string line;

    while (std::getline(input, line)) {
        if (line.find_first_not_of(" \t\n\r") == std::string::npos)
            continue;

        try {
            std::string trimmed = line;
            trimmed.erase(0, trimmed.find_first_not_of(" \t"));
            interpreter.interpr(line);
        } catch (const std::exception& e) {
            output << "Error: " << e.what() << std::endl;
        }
    }

    return true;
}

