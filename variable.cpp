#include "variable.hpp"

using namespace GUMLANG;

Variable::Variable(std::string id)
{
    if(value == (INT32_C || float_t))
    {
        type = 1;
    }
    else if(value == BOOL)
    {
        type = 2;
    }
    else if(value == STRING)
    {
        type = 3;
    }
    else
    {
        std::cout << "Variable has no defined type." << std::endl;
        type = 0;
    } 
}

void Variable::Delete()
{
    //~Variable();
}

Variable::~Variable()
{
	    
}

bool Variable::canCombine(Variable a, Variable b)
{
    switch(a.type)
    {
        case(1): // NUM
        
        switch(b.type)
        {
             case(1): // NUM
             
             return true;
             
             break;
             
             case(2): // BOOL
             
             return false;
             
             break;
             
             case(3): // STRING
             
             return false;
             
             break;
        }
        
        break;
        
        case(2): // BOOL
        
        return false;
        
        break;
        
        case(3): // STRING
        
        switch(b.type)
        {
            case(1): // NUM
            
            return true;
            
            break;
            
            case(2): // BOOL
            
            return true;
            
            break;
            
            case(3): // STRING
            
            return true;
            
            break;
        }
        
        break;
    }
    
    return false;
}
        
void Variable::Add(Variable a, Variable b)
{
    if(!canCombine(a, b))
    {
        std::cout << a.id << " and " << b.id << " cannot be combined." << std::endl;
        
        return NULL;
    }
    
    a = a + b;
}

void Variable::Subtract(Variable a, Variable b)
{
    if(!canCombine(a, b))
    {
        std::cout << a.id << " and " << b.id << " cannot be combined." << std::endl;
        
        return NULL;
    }
    
    a = a + b;
}

void Variable::Multiply(Variable a, Variable b)
{
    if(!canCombine(a, b))
    {
        std::cout << a.id << " and " << b.id << " cannot be combined." << std::endl;
        
        return NULL;
    }
    
    a = a + b;
}

void Variable::Add(Variable a, Variable b)
{
    if(!canCombine(a, b))
    {
        std::cout << a.id << " and " << b.id << " cannot be combined." << std::endl;
        
        return NULL;
    }
    
    a = a + b;
}
