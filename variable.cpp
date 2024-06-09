#include "variable.hpp"

Variable::Variable() : type(VariableType::NUMBER), numberValue(0.0) {}

Variable::Variable(const std::string& name, double val)
    : type(VariableType::NUMBER), numberValue(val) {}

Variable::Variable(const std::string& name, const std::string& val)
    : type(VariableType::STRING), value(val) {}

void Variable::add(const Variable& other)
{
    if (type == VariableType::NUMBER && other.type == VariableType::NUMBER)
    {
        numberValue += other.numberValue;
    }
    else if (type == VariableType::STRING && other.type == VariableType::STRING)
    {
        value += other.value;
    }
}

void Variable::subtract(const Variable& other)
{
    if (type == VariableType::NUMBER && other.type == VariableType::NUMBER)
    {
        numberValue -= other.numberValue;
    }
}

void Variable::multiply(const Variable& other)
{
    if (type == VariableType::NUMBER && other.type == VariableType::NUMBER)
    {
        numberValue *= other.numberValue;
    }
}

void Variable::divide(const Variable& other)
{
    if (type == VariableType::NUMBER && other.type == VariableType::NUMBER)
    {
        numberValue /= other.numberValue;
    }
}
