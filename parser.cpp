#include "parser.hpp"

using namespace GUMLANG;

Parser::Parser(const std::string& filename)
{
    file.open(filename);

    if (!file.is_open())
    {
        std::cerr << "Cannot open source file. Try opening from a different directory." << std::endl;
    }
    else
    {
        if (!hasGumExtension(filename))
        {
            std::cerr << filename + " is not a GUM sourcefile." << std::endl;
            isGumSourceFile = false;
        }
    }
}

void Parser::InterpretFile()
{
    if (!file.is_open()) return;
    if (!isGumSourceFile) return;

    std::string line;
    bool inBlockComment = false;

    while (std::getline(file, line))
    {
        // Trim any leading or trailing whitespace
        line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
        line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);

        // Check for block comments
        if (inBlockComment)
        {
            if (line.find("*/") != std::string::npos)
            {
                inBlockComment = false;
                line = line.substr(line.find("*/") + 2);
                line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
            }
            else
            {
                continue;
            }
        }
        if (line.find("/*") != std::string::npos)
        {
            inBlockComment = true;
            line = line.substr(0, line.find("/*"));
            line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
            line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
        }

        // Remove inline comments
        size_t commentPos = line.find("//");
        if (commentPos != std::string::npos)
        {
            line = line.substr(0, commentPos);
            // Trim the line again after removing comment
            line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
            line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
        }

        // Skip empty lines or lines that were comments
        if (line.empty())
        {
            continue;
        }

        if (!line.empty())
        {
            if (line.find("print") == 0)
            {
                parseLine(line);
            }
            else if (line.find("+=") != std::string::npos || line.find("-=") != std::string::npos ||
                     line.find("*=") != std::string::npos || line.find("/=") != std::string::npos)
            {
                parseArithmetic(line);
            }
            else if (line.find(" += ") != std::string::npos) // Special case for concatenation
            {
                parseConcatenation(line);
            }
            else
            {
                parseLine(line);
            }
        }
    }
}

void Parser::parseLine(const std::string& line)
{
    if (line.find("print") == 0)
    {
        std::string content = line.substr(6); // extract the content to print
        content.erase(0, content.find_first_not_of(" \t\n\r\f\v")); // trim leading whitespace
        content.erase(content.find_last_not_of(" \t\n\r\f\v") + 1); // trim trailing whitespace

        // Evaluate the expression
        handleExpression(content);
    }
    else
    {
        // Check for variable initialization
        size_t pos = line.find(" ");
        if (pos != std::string::npos)
        {
            std::string varName = line.substr(0, pos);
            std::string value = line.substr(pos + 1);

            varName.erase(0, varName.find_first_not_of(" \t\n\r\f\v")); // trim leading whitespace
            varName.erase(varName.find_last_not_of(" \t\n\r\f\v") + 1); // trim trailing whitespace
            value.erase(0, value.find_first_not_of(" \t\n\r\f\v")); // trim leading whitespace
            value.erase(value.find_last_not_of(" \t\n\r\f\v") + 1); // trim trailing whitespace

            if (isNumber(value))
            {
                variables[varName] = Variable(varName, std::stod(value));
            }
            else
            {
                // Remove surrounding quotes if present
                if (value.front() == '"' && value.back() == '"') {
                    value = value.substr(1, value.size() - 2);
                }
                variables[varName] = Variable(varName, value);
            }
        }
        else
        {
            std::cerr << "Syntax error: invalid line format: " << line << std::endl;
        }
    }
}

void Parser::handleExpression(const std::string& content)
{
    std::istringstream iss(content);
    std::string token;
    std::vector<std::string> tokens;
    while (std::getline(iss, token, ' '))
    {
        if (!token.empty())
        {
            tokens.push_back(token);
        }
    }

    Variable result;
    bool first = true;
    std::string op;

    for (const auto& token : tokens)
    {
        if (token == "+" || token == "-" || token == "*" || token == "/")
        {
            op = token;
        }
        else
        {
            Variable var;
            if (isNumber(token))
            {
                var = Variable(token, std::stod(token));
            }
            else if (token.front() == '"' && token.back() == '"')
            {
                // Handle string literals
                var = Variable(token, token.substr(1, token.size() - 2));
            }
            else
            {
                auto it = variables.find(token);
                if (it != variables.end())
                {
                    var = it->second;
                }
                else
                {
                    std::cerr << "Undefined variable: " << token << std::endl;
                    return;
                }
            }

            if (first)
            {
                result = var;
                first = false;
            }
            else
            {
                if (op == "+")
                {
                    if (result.type == VariableType::NUMBER && var.type == VariableType::NUMBER)
                    {
                        result.numberValue += var.numberValue;
                    }
                    else
                    {
                        std::string leftValue = (result.type == VariableType::STRING) ? result.value : std::to_string(result.numberValue);
                        std::string rightValue = (var.type == VariableType::STRING) ? var.value : std::to_string(var.numberValue);

                        // Remove trailing zeros from numbers before concatenating
                        if (result.type == VariableType::NUMBER) {
                            leftValue.erase(leftValue.find_last_not_of('0') + 1, std::string::npos);
                            leftValue.erase(leftValue.find_last_not_of('.') + 1, std::string::npos);
                        }
                        if (var.type == VariableType::NUMBER) {
                            rightValue.erase(rightValue.find_last_not_of('0') + 1, std::string::npos);
                            rightValue.erase(rightValue.find_last_not_of('.') + 1, std::string::npos);
                        }

                        result = Variable("", leftValue + rightValue);
                    }
                }
                else if (op == "-")
                {
                    if (result.type == VariableType::NUMBER && var.type == VariableType::NUMBER)
                    {
                        result.numberValue -= var.numberValue;
                    }
                    else
                    {
                        std::cerr << "Unsupported operation for non-numeric types: " << result.value << " - " << var.value << std::endl;
                        return;
                    }
                }
                else if (op == "*")
                {
                    if (result.type == VariableType::NUMBER && var.type == VariableType::NUMBER)
                    {
                        result.numberValue *= var.numberValue;
                    }
                    else
                    {
                        std::cerr << "Unsupported operation for non-numeric types: " << result.value << " * " << var.value << std::endl;
                        return;
                    }
                }
                else if (op == "/")
                {
                    if (result.type == VariableType::NUMBER && var.type == VariableType::NUMBER)
                    {
                        if (var.numberValue != 0)
                        {
                            result.numberValue /= var.numberValue;
                        }
                        else
                        {
                            std::cerr << "Division by zero error" << std::endl;
                            return;
                        }
                    }
                    else
                    {
                        std::cerr << "Unsupported operation for non-numeric types: " << result.value << " / " << var.value << std::endl;
                        return;
                    }
                }
            }
        }
    }

    if (result.type == VariableType::NUMBER)
    {
        std::cout << result.numberValue << std::endl;
    }
    else
    {
        std::cout << result.value << std::endl;
    }
}

void Parser::parseArithmetic(const std::string& line)
{
    size_t pos;
    std::string varName, operand;

    if ((pos = line.find("+=")) != std::string::npos)
    {
        varName = line.substr(0, pos);
        operand = line.substr(pos + 2);
    }
    else if ((pos = line.find("-=")) != std::string::npos)
    {
        varName = line.substr(0, pos);
        operand = line.substr(pos + 2);
    }
    else if ((pos = line.find("*=")) != std::string::npos)
    {
        varName = line.substr(0, pos);
        operand = line.substr(pos + 2);
    }
    else if ((pos = line.find("/=")) != std::string::npos)
    {
        varName = line.substr(0, pos);
        operand = line.substr(pos + 2);
    }
    else
    {
        std::cerr << "Syntax error: invalid arithmetic operation in line: " << line << std::endl;
        return;
    }

    // Trim whitespace from variable names and operand
    varName.erase(0, varName.find_first_not_of(" \t\n\r\f\v"));
    varName.erase(varName.find_last_not_of(" \t\n\r\f\v") + 1);
    operand.erase(0, operand.find_first_not_of(" \t\n\r\f\v"));
    operand.erase(operand.find_last_not_of(" \t\n\r\f\v") + 1);

    auto it = variables.find(varName);
    if (it == variables.end())
    {
        std::cerr << "Undefined variable: " << varName << std::endl;
        return;
    }

    auto operandIt = variables.find(operand);
    if (operandIt == variables.end())
    {
        std::cerr << "Undefined operand variable: " << operand << std::endl;
        return;
    }

    if (line.find("+=") != std::string::npos)
    {
        it->second.add(operandIt->second);
    }
    else if (line.find("-=") != std::string::npos)
    {
        it->second.subtract(operandIt->second);
    }
    else if (line.find("*=") != std::string::npos)
    {
        it->second.multiply(operandIt->second);
    }
    else if (line.find("/=") != std::string::npos)
    {
        it->second.divide(operandIt->second);
    }
}

void Parser::parseConcatenation(const std::string& line)
{
    size_t pos = line.find(" += ");
    if (pos == std::string::npos)
    {
        std::cerr << "Syntax error: invalid concatenation operation in line: " << line << std::endl;
        return;
    }

    std::string varName = line.substr(0, pos);
    std::string leftOperand, rightOperand;
    std::string operands = line.substr(pos + 4);

    // Trim whitespace from variable names and operands
    varName.erase(0, varName.find_first_not_of(" \t\n\r\f\v"));
    varName.erase(varName.find_last_not_of(" \t\n\r\f\v") + 1);
    operands.erase(0, operands.find_first_not_of(" \t\n\r\f\v"));
    operands.erase(operands.find_last_not_of(" \t\n\r\f\v") + 1);

    size_t spacePos = operands.find(" ");
    if (spacePos == std::string::npos)
    {
        std::cerr << "Syntax error: invalid concatenation operation in operands: " << operands << std::endl;
        return;
    }

    leftOperand = operands.substr(0, spacePos);
    rightOperand = operands.substr(spacePos + 1);

    // Trim whitespace from operands
    leftOperand.erase(0, leftOperand.find_first_not_of(" \t\n\r\f\v"));
    leftOperand.erase(leftOperand.find_last_not_of(" \t\n\r\f\v") + 1);
    rightOperand.erase(0, rightOperand.find_first_not_of(" \t\n\r\f\v"));
    rightOperand.erase(rightOperand.find_last_not_of(" \t\n\r\f\v") + 1);

    auto leftIt = variables.find(leftOperand);
    auto rightIt = variables.find(rightOperand);

    if (leftIt == variables.end() || rightIt == variables.end())
    {
        std::cerr << "Undefined variable(s) in concatenation: " << leftOperand << " or " << rightOperand << std::endl;
        return;
    }

    if (leftIt->second.type == VariableType::STRING && rightIt->second.type == VariableType::STRING)
    {
        variables[varName] = Variable(varName, leftIt->second.value + rightIt->second.value);
    }
    else
    {
        std::cerr << "Concatenation error: Both operands must be strings." << std::endl;
    }
}

bool Parser::isNumber(const std::string& s)
{
    char* end = nullptr;
    double val = std::strtod(s.c_str(), &end);
    return end != s.c_str() && *end == '\0' && val != HUGE_VAL;
}

bool Parser::hasGumExtension(const std::string& filename)
{
    return filename.size() >= 5 && filename.substr(filename.size() - 4) == ".gum";
}

