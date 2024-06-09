#include <iostream>
#include <string>
#include "parser.hpp"

using namespace GUMLANG;

int main()
{
    // std::cout << "Enter source file directory." << std::endl;
    // std::string input;
    // std::cin >> input;
    
    std::string input = "/Users/zacharybunch/Documents/GUMLANG/GUMLANG/hello.gum";

    Parser parser(input);
    parser.InterpretFile();

    return 0;
}
