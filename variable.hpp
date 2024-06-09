#ifndef VARIABLE_HPP
#define VARIABLE_HPP

#include <string>

enum class VariableType
{
    NUMBER,
    STRING
};

class Variable
{
public:
    VariableType type;
    double numberValue;
    std::string value;

    Variable();
    Variable(const std::string& name, double val);
    Variable(const std::string& name, const std::string& val);

    void add(const Variable& other);
    void subtract(const Variable& other);
    void multiply(const Variable& other);
    void divide(const Variable& other);
};

#endif // VARIABLE_HPP
