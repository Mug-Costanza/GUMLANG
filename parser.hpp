#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include "variable.hpp"

namespace GUMLANG
{
    class Parser
    {
    public:
        Parser(const std::string& filename);
        void InterpretFile();
    private:
        std::ifstream file;
        std::unordered_map<std::string, Variable> variables;
        void parseLine(const std::string& line);
        void parseArithmetic(const std::string& line);
        void parseConcatenation(const std::string& line);
        void handleExpression(const std::string& content); // Corrected declaration
        bool isNumber(const std::string& s);
        bool hasGumExtension(const std::string& filename);
        bool isGumSourceFile = true;
    };
}
