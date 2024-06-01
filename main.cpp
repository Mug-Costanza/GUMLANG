/*
    WHAT A PROGRAMMING LANGUAGE NEEDS:
     * VARIABLES
     * LANGUAGE INTERPRETOR
     
    GOALS:
     * CREATE A READABLE SYNTAX
     * MAKE A THE LANGUAGE SIMPLE AND EASY
     * MAYBE SOME SIMPLE FORM OF MEMORY MANAGEMENT
*/

//std
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

//Project Files
#include "interpreter.cpp"

int main()
{
    std::cout << "Enter source file directory." << std::endl;
    
    std::string input;
    
    std::cin >> input;

    const char *source = input.c_str();
    
    ReadFile(source);
    
    return 0;
}
