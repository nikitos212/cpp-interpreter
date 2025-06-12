#include "interpreter/interpreter.h"
#include <iostream>
#include <sstream>

int main(int argc, char** argv) {
    std::string code = R"(
        
    )";

    std::istringstream stream(code);

    interpret(stream, std::cout);
    return 0;
}