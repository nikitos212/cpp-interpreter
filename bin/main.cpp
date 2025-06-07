#include "interpreter/interpreter.h"
#include <iostream>
#include <sstream>

int main(int argc, char** argv) {
    std::string code = R"(
        funcs = [
            function() return 1 end function,
            function() return 2 end function,
            function() return 3 end function
        ]

        print(funcs[0]())
        print(funcs[1]())
        print(funcs[2]())
    )";

    std::istringstream stream(code);

    interpret(stream, std::cout);
    return 0;
}