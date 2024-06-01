#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

//#define NUM     1
//#define BOOL    2
//#define STRING  3

namespace GUMLANG
{

    /*
        VARIABLE TYPES
        * NUM : CAN MERGE WITH NUMS
        * BOOL : CAN MERGE WITH BOOLS
        * STRING : CAN MERGE W/ ALL TYPES
        
        VARIABLE FUNCTIONS
        * MERGE [ADD, SUB, MUL, DIV]
    */
    
    struct Variable
    {
        Variable(std::string id); // Constructor
        ~Variable(); // Destructor
        
        int type;
        
        char *value;
        
        bool canCombine(Variable a, Variable b);
        bool combine(Variable a, Variable b);
        
        void Delete();
        
    private:
        std::string id;
    };
}