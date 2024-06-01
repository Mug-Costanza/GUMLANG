#include <stdio.h>
#include <iostream>

/*
    OPERATOR TYPES:
     * 1 - ADD - '+'
     * 2 - SUB - '-'
     * 3 - MUL - '*'
     * 4 - DIV - '/'

*/

namespace GUMLANG
{
    struct Operator
    {
        Operator(char oper);
    
        int type;
    
        bool isValidOperator(char op);
        void defineOperator(char op);
        
    private:
        char op;
    };
}