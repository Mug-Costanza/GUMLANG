#include "operator.hpp"

using namespace GUMLANG;

Operator::Operator(char oper)
{
    this->op = oper;
    
    if(!isValidOperator(op))
    {
        std::cout << op << " is not a valid operator." << std::endl;
    }
    else
    {
        defineOperator(op);
    }
}

bool Operator::isValidOperator(char op)
{
    if(op == '+')
    {
        type = 1;
    }
    else if(op == '-')
    {
        type = 2;
    }
    else if(op == '*')
    {
        type = 3;
    }
    else if(op == '/')
    {
        type = 4;
    }
    else
    {
        std::cout << "Operator has no defined type." << std::endl;
        type = 0;
    }
    
    return true;
}

void defineOperator(char op)
{
    
}