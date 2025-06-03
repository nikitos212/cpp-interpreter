#include "interpreter/interpreter.h"
#include <iostream>
#include <sstream>

int main(int argc, char** argv) {
    std::string code = R"(
    s = "ITMO"
    k = 0
    while  s != "ITMOITMO"
        s = s * 2
        k += 1
    end while
    print(s)
)";

    std::istringstream stream(code);

    interpret(stream, std::cout);
    return 0;
}