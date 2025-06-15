#include "interpreter/interpreter.h"
#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename.is>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    
    if (filename.size() < 3 || filename.substr(filename.size() - 3) != ".is") {
        std::cerr << "Error: Input file must have .is extension" << std::endl;
        return 1;
    }

    std::ifstream input_file(filename);
    if (!input_file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return 1;
    }

    try {
        interpret(input_file, std::cout);
    } catch (const std::exception& e) {
        std::cerr << "Interpretation error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}