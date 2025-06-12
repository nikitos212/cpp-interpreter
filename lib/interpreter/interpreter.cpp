#include "interpreter.h"
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

    while (true) {
        if (!std::getline(input, line))
            break;

        std::string trimmed = line;
        trimmed.erase(0, trimmed.find_first_not_of(" \t"));

        if (trimmed.empty() || (trimmed.size() >= 2 && trimmed[0] == '/' && trimmed[1] == '/'))
        {
            continue;
        }

        bool isIf    = (trimmed.rfind("if ", 0)    == 0);
        bool isFor   = (trimmed.rfind("for ", 0)   == 0);
        bool isWhile = (trimmed.rfind("while ", 0) == 0);
        bool isList = (trimmed.find("= [") != std::string::npos);
        bool isFunctionLiteral = (trimmed.find("= function") != std::string::npos);

        if (!isIf && !isFor && !isWhile && !isFunctionLiteral && !isList) {
            try {
                interpreter.interpr(trimmed);
            } catch (const std::exception& e) {
                output << "Error: " << e.what() << std::endl;
                return false;
            }
            continue;
        }

        std::string block = trimmed + "\n";

        int depth = 1;

        bool oneLineIf    = (isIf    && (trimmed.find("end if")    != std::string::npos));
        bool oneLineFor   = (isFor   && (trimmed.find("end for")   != std::string::npos));
        bool oneLineWhile = (isWhile && (trimmed.find("end while") != std::string::npos));
        bool oneLineFunc  = (isFunctionLiteral && (trimmed.find("end function") != std::string::npos));
        bool oneLineList  = (isList && (trimmed.find("]") != std::string::npos));

        if (oneLineIf || oneLineFor || oneLineWhile || oneLineFunc || oneLineList) {
            try {
                interpreter.interpr(block);
            } catch (const std::exception& e) {
                output << "Error: " << e.what() << std::endl;
                return false;
            }
            continue;
        }

        while (depth > 0) {
            if (!std::getline(input, line)) {
                output << "Error: unclosed block starting with: " << trimmed << std::endl;
                return false;
            }

            std::string t = line;
            t.erase(0, t.find_first_not_of(" \t"));

            if (t.rfind("if ", 0) == 0) {
                depth++;
            }
            if (t.rfind("for ", 0) == 0) {
                depth++;
            }
            if (t.rfind("while ", 0) == 0) {
                depth++;
            }
            if (t.find("= function") != std::string::npos) {
                depth++;
            }
            if (t.find("= [") != std::string::npos) {
                depth++;
            }

            if (t.find("end if") != std::string::npos ||
                t.find("end for") != std::string::npos ||
                t.find("end while") != std::string::npos ||
                t.rfind("end function", 0) == 0 ||
                t.rfind("]", 0) == 0)
            {
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
