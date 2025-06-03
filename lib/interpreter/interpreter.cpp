#include "interpreter.h"
#include "symbol_table/SymbolTable.h"
#include "parser/parser.h"
#include "ast/nodes.h"
#include <cstddef>

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

    while (true) {
        if (!std::getline(input, line))
            break;
        
        std::string trimmed = line;
        trimmed.erase(0, trimmed.find_first_not_of(" \t"));

        if (trimmed.empty() || (trimmed.size() >= 2 && trimmed[0] == '/' && trimmed[1] == '/')) {
            continue;
        }

        if (trimmed.rfind("if ", 0) != 0 && trimmed.rfind("for ", 0) != 0 && trimmed.rfind("while ", 0) != 0) {
            try {
                interpreter.interpr(trimmed);
            } catch (const std::exception& e) {
                output << "Error: " << e.what() << std::endl;
                return false;
            }
            continue;
        }

        std::string block = trimmed + "\n";
        
        if (trimmed.rfind("if ", 0) == 0 && trimmed.find("end if") != std::string::npos) {
            try {
                interpreter.interpr(block);
            } catch (const std::exception& e) {
                output << "Error: " << e.what() << std::endl;
                return false;
            }
            continue;
        }

        if (trimmed.rfind("for ", 0) == 0 && trimmed.find("end for") != std::string::npos) {
            try {
                interpreter.interpr(block);
            } catch (const std::exception& e) {
                output << "Error: " << e.what() << std::endl;
                return false;
            }
            continue;
        }

        if (trimmed.rfind("while ", 0) == 0 && trimmed.find("end while") != std::string::npos) {
            try {
                interpreter.interpr(block);
            } catch (const std::exception& e) {
                output << "Error: " << e.what() << std::endl;
                return false;
            }
            continue;
        }

        size_t depth = 1;

        while (depth > 0) {
            if (!std::getline(input, line)) {
                output << "Error: unclosed block starting with: " << trimmed << std::endl;
                return false;
            }
            
            std::string t = line;
            t.erase(0, t.find_first_not_of(" \t"));
            
            if (t.rfind("if ", 0) == 0 || t.rfind("for ", 0) == 0 || t.rfind("while ", 0) == 0) {
                depth++; 
            }
            else if (t.rfind("end if", 0) == 0 || t.rfind("end for", 0) == 0 || t.rfind("end while", 0) == 0 || t == "end") {
                depth--;
            }

            block += t + "\n";
        }

        try {
            interpreter.interpr(block);
        } catch (const std::exception& e) {
            output << "Error: " << e.what() << std::endl;
            return false;
        }
    }

    return true;
}