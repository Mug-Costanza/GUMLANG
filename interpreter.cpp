#include "interpreter.hpp"

int ReadFile(const char *source)
{
    std::string filename(source);
    
    std::ifstream file(source);
    if (!file.is_open())
    {
        std::cerr << "Cannot open source file. Try opening from a different directory." << std::endl;
        return -1;
    }

    file.close();
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
