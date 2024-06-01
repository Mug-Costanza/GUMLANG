#include "interpreter.hpp"

int ReadFile(const char *source)
{
    std::ifstream file(source);
    file.open(source);
    bool isValidFile = file.is_open();
    
    if(!isValidFile)
    {
        std::cout << "Cannot open source file. Try opening from a different directory." << std::endl;
        file.close();
        return -1;
    }
    
    return 0;
}

std::string getErr(const char *source)
{
    if(ReadFile(source))
    {
        return "\n";
    }
    else
    {
        return "There were build errors.\n";
    }
}
