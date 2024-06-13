#include "parser.hpp"
using namespace GUMLANG;

Parser::Parser(const std::string& filename)
    : lexer(""), currentToken({TokenType::TOKEN_UNKNOWN, "", 0, 0})
{
    file.open(filename);

    if (!file.is_open()) {
        std::cerr << "Cannot open source file. Try opening from a different directory." << std::endl;
    } else {
        if (!hasGumExtension(filename)) {
            std::cerr << filename + " is not a GUM sourcefile." << std::endl;
            isGumSourceFile = false;
        }
        std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        lexer = Lexer(source);
        advanceToken();
    }
}

void Parser::InterpretFile() {
    if (!file.is_open()) return;
    if (!isGumSourceFile) return;

    while (currentToken.type != TokenType::TOKEN_EOF) {
        parseLine();
    }
}

void Parser::parseBlock() {
    while (currentToken.type != TokenType::TOKEN_RBRACE) {
        if (currentToken.type == TokenType::TOKEN_EOF) {
            std::cerr << "Syntax error: unexpected end of file in block." << std::endl;
            exit(1);
        }
        parseLine();
    }
    advanceToken(); // consume '}'
}

void Parser::advanceToken() {
    currentToken = lexer.getNextToken();
}

void Parser::expectToken(TokenType type) {
    if (currentToken.type != type) {
        std::cerr << "Syntax error: expected token type " << static_cast<int>(type)
                  << ", but got " << static_cast<int>(currentToken.type) << std::endl;
        exit(1);
    }
    advanceToken();
}

void Parser::parseIfStatement() {
    advanceToken(); // consume 'if'
    
    std::string condition;
    while (currentToken.type != TokenType::TOKEN_THEN && currentToken.type != TokenType::TOKEN_LBRACE && currentToken.type != TokenType::TOKEN_EOL) {
        condition += currentToken.value + " ";
        advanceToken();
    }
    
    if (currentToken.type == TokenType::TOKEN_THEN)
        advanceToken(); // consume 'then'

    bool conditionResult = evaluateCondition(condition);

    if (currentToken.type == TokenType::TOKEN_EOL)
        advanceToken();

    if (currentToken.type == TokenType::TOKEN_LBRACE) {
        advanceToken(); // consume '{'
        if (conditionResult) {
            parseBlock();
        } else {
            skipBlock(); // skip if block
        }
    } else {
        // Handle single-line if statement
        if (conditionResult) {
            parseSingleLineStatement();
        } else {
            // Skip to the end of the single-line statement
            while (currentToken.type != TokenType::TOKEN_EOF && currentToken.type != TokenType::TOKEN_EOL) {
                advanceToken();
            }
            if (currentToken.type == TokenType::TOKEN_EOL) {
                advanceToken(); // consume EOL if present
            }
        }
    }
    handleElseIfOrElse(); // Process else if and else blocks
}

void Parser::handleElseIfOrElse() {
    bool conditionMet = false;

    while (currentToken.type == TokenType::TOKEN_ELSEIF || currentToken.type == TokenType::TOKEN_ELSE) {
        if (currentToken.type == TokenType::TOKEN_ELSEIF) {
            advanceToken(); // consume 'else if'

            std::string condition;
            while (currentToken.type != TokenType::TOKEN_THEN && currentToken.type != TokenType::TOKEN_LBRACE && currentToken.type != TokenType::TOKEN_EOL) {
                condition += currentToken.value + " ";
                advanceToken();
            }

            if (currentToken.type == TokenType::TOKEN_THEN)
                advanceToken(); // consume 'then'

            bool conditionResult = evaluateCondition(condition);

            if (currentToken.type == TokenType::TOKEN_EOL)
                advanceToken();

            if (currentToken.type == TokenType::TOKEN_LBRACE) {
                advanceToken(); // consume '{'
                if (conditionResult && !conditionMet) {
                    parseBlock();
                    conditionMet = true;
                } else {
                    skipBlock(); // skip this else if block
                }
            }
        } else if (currentToken.type == TokenType::TOKEN_ELSE) {
            advanceToken(); // consume 'else'
            if (currentToken.type == TokenType::TOKEN_LBRACE) {
                advanceToken(); // consume '{'
                if (!conditionMet) {
                    parseBlock(); // parse else block
                } else {
                    skipBlock(); // skip else block
                }
            } else {
                std::cerr << "Syntax error: else must be followed by a block enclosed in braces." << std::endl;
                exit(1);
            }
        }
    }
}

void Parser::parseForLoop() {
    advanceToken(); // consume 'for'
    if (currentToken.type != TokenType::TOKEN_NUMBER) {
        std::cerr << "Syntax error: expected a number of cycles for 'for' loop, but got: " << currentToken.value << std::endl;
        exit(1);
    }

    int cycles = 0;
    try {
        cycles = std::stoi(currentToken.value);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Syntax error: invalid number of cycles for 'for' loop: " << currentToken.value << std::endl;
        exit(1);
    }

    advanceToken(); // consume the cycle amount

    if (currentToken.type == TokenType::TOKEN_LBRACE) {
        advanceToken(); // consume '{'
        std::vector<std::string> loopBody;
        std::string loopLine;

        while (currentToken.type != TokenType::TOKEN_RBRACE) {
            if (currentToken.type == TokenType::TOKEN_EOF) {
                std::cerr << "Syntax error: unexpected end of file in 'for' loop." << std::endl;
                exit(1);
            }
            if (currentToken.type == TokenType::TOKEN_EOL) {
                loopBody.push_back(loopLine);
                loopLine.clear();
            } else {
                loopLine += currentToken.value + " ";
            }
            advanceToken();
        }
        advanceToken(); // consume '}'

        for (int i = 0; i < cycles; ++i) {
            for (const auto& line : loopBody) {
                std::istringstream iss(line);
                std::string token;
                while (iss >> token) {
                    // Reset lexer and currentToken for each line
                    lexer = Lexer(line);
                    advanceToken();
                    // Parse the line as a complete statement
                    parseLine();
                }
            }
        }

        if (currentToken.type == TokenType::TOKEN_EOL) {
            advanceToken(); // consume EOL if present
        }
    } else {
        // Handle single-line for loop
        std::string singleLineStatement;
        while (currentToken.type != TokenType::TOKEN_EOL && currentToken.type != TokenType::TOKEN_EOF) {
            singleLineStatement += currentToken.value + " ";
            advanceToken();
        }
        for (int i = 0; i < cycles; ++i) {
            std::istringstream iss(singleLineStatement);
            lexer = Lexer(singleLineStatement);
            advanceToken();
            parseLine();
        }
        if (currentToken.type == TokenType::TOKEN_EOL) {
            advanceToken(); // consume EOL if present
        }
    }
}

void Parser::parsePrintStatement() {
    advanceToken(); // consume 'print'
    std::string content;
    while (currentToken.type != TokenType::TOKEN_EOF && currentToken.type != TokenType::TOKEN_EOL) {
        content += currentToken.value + " ";
        advanceToken();
    }
    handleExpression(content);

    if (currentToken.type == TokenType::TOKEN_EOL) {
        advanceToken(); // consume EOL if present
    }
}

void Parser::parseLine() {
    if (currentToken.type == TokenType::TOKEN_IF) {
        parseIfStatement();
    } else if (currentToken.type == TokenType::TOKEN_PRINT) {
        parsePrintStatement();
    } else if (currentToken.type == TokenType::TOKEN_FOR) {
        parseForLoop();
    } else if (currentToken.type == TokenType::TOKEN_RANDOM) {
        parseRandom();
    } else if (currentToken.type == TokenType::TOKEN_IDENTIFIER) {
        std::string varName = currentToken.value;
        advanceToken();
        if (currentToken.type == TokenType::TOKEN_ASSIGN) {
            advanceToken(); // consume '='
            parseAssignment(varName);
        } else if (currentToken.type == TokenType::TOKEN_OPERATOR_PLUSEQUAL) {
            advanceToken(); // consume '+='
            parseAdditionAssignment(varName);
        } else if (currentToken.type == TokenType::TOKEN_OPERATOR_MINUSEQUAL) {
            advanceToken(); // consume '-='
            parseSubtractionAssignment(varName);
        } else if (currentToken.type == TokenType::TOKEN_OPERATOR_STAREQUAL) {
            advanceToken(); // consume '*='
            parseMultiplicationAssignment(varName);
        } else if (currentToken.type == TokenType::TOKEN_OPERATOR_SLASHEQUAL) {
            advanceToken(); // consume '/='
            parseDivisionAssignment(varName);
        } else if (currentToken.type == TokenType::TOKEN_OPERATOR_INCREMENT) {
            advanceToken(); // consume '++'
            parseIncrement(varName);
        } else if (currentToken.type == TokenType::TOKEN_OPERATOR_DECREMENT) {
            advanceToken(); // consume '--'
            parseDecrement(varName);
        } else if (currentToken.type == TokenType::TOKEN_NUMBER || currentToken.type == TokenType::TOKEN_STRING) {
            parseVariableDeclaration(varName);
        } else if (currentToken.type == TokenType::TOKEN_EOL) {
            parseVariableDeclaration(varName);
        } else {
            std::cerr << "Syntax error: invalid line format: " << varName << std::endl;
        }
    } else if (currentToken.type == TokenType::TOKEN_EOL) {
        advanceToken(); // consume EOL and continue
    } else {
        std::cerr << "Syntax error: unexpected token: " << currentToken.value << std::endl;
        advanceToken(); // consume the unexpected token and continue
    }
}

void Parser::parseSingleLineStatement() {
    if (currentToken.type == TokenType::TOKEN_PRINT) {
        parsePrintStatement();
    } else if (currentToken.type == TokenType::TOKEN_IDENTIFIER) {
        std::string varName = currentToken.value;
        advanceToken();
        if (currentToken.type == TokenType::TOKEN_ASSIGN) {
            advanceToken(); // consume '='
            parseAssignment(varName);
        } else if (currentToken.type == TokenType::TOKEN_OPERATOR_PLUSEQUAL) {
            advanceToken(); // consume '+='
            parseAdditionAssignment(varName);
        } else if (currentToken.type == TokenType::TOKEN_OPERATOR_MINUSEQUAL) {
            advanceToken(); // consume '-='
            parseSubtractionAssignment(varName);
        } else if (currentToken.type == TokenType::TOKEN_OPERATOR_STAREQUAL) {
            advanceToken(); // consume '*='
            parseMultiplicationAssignment(varName);
        } else if (currentToken.type == TokenType::TOKEN_OPERATOR_SLASHEQUAL) {
            advanceToken(); // consume '/='
            parseDivisionAssignment(varName);
        } else if (currentToken.type == TokenType::TOKEN_OPERATOR_INCREMENT) {
            advanceToken(); // consume '++'
            parseIncrement(varName);
        } else if (currentToken.type == TokenType::TOKEN_OPERATOR_DECREMENT) {
            advanceToken(); // consume '--'
            parseDecrement(varName);
        } else if (currentToken.type == TokenType::TOKEN_NUMBER || currentToken.type == TokenType::TOKEN_STRING) {
            parseVariableDeclaration(varName);
        } else {
            std::cerr << "Syntax error: invalid line format: " << varName << std::endl;
        }
    } else {
        std::cerr << "Syntax error: unexpected token: " << currentToken.value << std::endl;
        advanceToken(); // consume the unexpected token and continue
    }
}

void Parser::parseVariableDeclaration(const std::string& varName) {
    if (currentToken.type == TokenType::TOKEN_NUMBER) {
        variables[varName] = Variable(varName, std::stod(currentToken.value));
    } else if (currentToken.type == TokenType::TOKEN_STRING) {
        // Remove surrounding quotes if present
        std::string value = currentToken.value;
        if (value.front() == '"' && value.back() == '"') {
            value = value.substr(1, value.size() - 2);
        }
        variables[varName] = Variable(varName, value);
    } else {
        std::cerr << "Syntax error: expected a number or string for variable declaration." << std::endl;
    }
    advanceToken();
}

void Parser::parseAssignment(const std::string& varName) {
    std::string value;
    while (currentToken.type != TokenType::TOKEN_EOF && currentToken.type != TokenType::TOKEN_EOL &&
           currentToken.type != TokenType::TOKEN_LBRACE && currentToken.type != TokenType::TOKEN_RBRACE &&
           currentToken.type != TokenType::TOKEN_IF && currentToken.type != TokenType::TOKEN_THEN &&
           currentToken.type != TokenType::TOKEN_ELSE && currentToken.type != TokenType::TOKEN_PRINT && currentToken.type != TokenType::TOKEN_FOR) {
        value += currentToken.value + " ";
        advanceToken();
    }
    // Remove surrounding quotes if present
    if (value.front() == '"' && value.back() == '"') {
        value = value.substr(1, value.size() - 2);
    }
    if (isNumber(value)) {
        variables[varName] = Variable(varName, std::stod(value));
    } else {
        variables[varName] = Variable(varName, value);
    }
}

void Parser::parseAdditionAssignment(const std::string& varName) {
    if (variables.find(varName) == variables.end()) {
        std::cerr << "Undefined variable: " << varName << std::endl;
        return;
    }

    std::string value;
    while (currentToken.type != TokenType::TOKEN_EOF && currentToken.type != TokenType::TOKEN_EOL) {
        value += currentToken.value + " ";
        advanceToken();
    }

    Variable right = evaluateExpression(value);

    if (variables[varName].type == VariableType::NUMBER && right.type == VariableType::NUMBER) {
        variables[varName].numberValue += right.numberValue;
    } else if (variables[varName].type == VariableType::STRING && right.type == VariableType::STRING) {
        variables[varName].value += right.value;
    } else {
        std::cerr << "Type error: incompatible types for += operation." << std::endl;
    }
}

void Parser::parseSubtractionAssignment(const std::string& varName) {
    if (variables.find(varName) == variables.end()) {
        std::cerr << "Undefined variable: " << varName << std::endl;
        return;
    }

    std::string value;
    while (currentToken.type != TokenType::TOKEN_EOF && currentToken.type != TokenType::TOKEN_EOL) {
        value += currentToken.value + " ";
        advanceToken();
    }

    Variable right = evaluateExpression(value);

    if (variables[varName].type == VariableType::NUMBER && right.type == VariableType::NUMBER) {
        variables[varName].numberValue -= right.numberValue;
    } else {
        std::cerr << "Type error: incompatible types for -= operation." << std::endl;
    }
}

void Parser::parseMultiplicationAssignment(const std::string& varName) {
    if (variables.find(varName) == variables.end()) {
        std::cerr << "Undefined variable: " << varName << std::endl;
        return;
    }

    std::string value;
    while (currentToken.type != TokenType::TOKEN_EOF && currentToken.type != TokenType::TOKEN_EOL) {
        value += currentToken.value + " ";
        advanceToken();
    }

    Variable right = evaluateExpression(value);

    if (variables[varName].type == VariableType::NUMBER && right.type == VariableType::NUMBER) {
        variables[varName].numberValue *= right.numberValue;
    } else {
        std::cerr << "Type error: incompatible types for *= operation." << std::endl;
    }
}

void Parser::parseDivisionAssignment(const std::string& varName) {
    if (variables.find(varName) == variables.end()) {
        std::cerr << "Undefined variable: " << varName << std::endl;
        return;
    }

    std::string value;
    while (currentToken.type != TokenType::TOKEN_EOF && currentToken.type != TokenType::TOKEN_EOL) {
        value += currentToken.value + " ";
        advanceToken();
    }

    Variable right = evaluateExpression(value);

    if (variables[varName].type == VariableType::NUMBER && right.type == VariableType::NUMBER) {
        if (right.numberValue != 0) {
            variables[varName].numberValue /= right.numberValue;
        } else {
            std::cerr << "Division by zero error" << std::endl;
        }
    } else {
        std::cerr << "Type error: incompatible types for /= operation." << std::endl;
    }
}

void Parser::parseIncrement(const std::string& varName) {
    if (variables.find(varName) == variables.end()) {
        std::cerr << "Undefined variable: " << varName << std::endl;
        return;
    }
    if (variables[varName].type == VariableType::NUMBER) {
        variables[varName].numberValue += 1;
    } else {
        std::cerr << "Type error: ++ operation only supports numeric types." << std::endl;
    }
}

void Parser::parseDecrement(const std::string& varName) {
    if (variables.find(varName) == variables.end()) {
        std::cerr << "Undefined variable: " << varName << std::endl;
        return;
    }
    if (variables[varName].type == VariableType::NUMBER) {
        variables[varName].numberValue -= 1;
    } else {
        std::cerr << "Type error: -- operation only supports numeric types." << std::endl;
    }
}

void Parser::skipBlock() {
    int braceCount = 1; // Starting from the opening brace

    while (braceCount > 0) {
        advanceToken();
        if (currentToken.type == TokenType::TOKEN_LBRACE) {
            braceCount++;
        } else if (currentToken.type == TokenType::TOKEN_RBRACE) {
            braceCount--;
        } else if (currentToken.type == TokenType::TOKEN_EOF) {
            std::cerr << "Syntax error: unexpected end of file while skipping block." << std::endl;
            exit(1);
        }
    }
    advanceToken(); // consume the last '}'
}

bool Parser::evaluateCondition(const std::string& condition) {
    std::istringstream iss(condition);
    std::string token;
    std::vector<std::string> tokens;
    while (std::getline(iss, token, ' ')) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }

    if (tokens.size() != 3) {
        std::cerr << "Syntax error: invalid condition: " << condition << std::endl;
        return false;
    }

    Variable left = evaluateExpression(tokens[0]);
    std::string op = tokens[1];
    Variable right = evaluateExpression(tokens[2]);

    if (left.type == VariableType::NUMBER && right.type == VariableType::NUMBER) {
        if (op == "==") return left.numberValue == right.numberValue;
        if (op == "!=") return left.numberValue != right.numberValue;
        if (op == "<") return left.numberValue < right.numberValue;
        if (op == "<=") return left.numberValue <= right.numberValue;
        if (op == ">") return left.numberValue > right.numberValue;
        if (op == ">=") return left.numberValue >= right.numberValue;
    } else if (left.type == VariableType::STRING && right.type == VariableType::STRING) {
        if (op == "==") return left.value == right.value;
        if (op == "!=") return left.value != right.value;
        std::cerr << "Unsupported operation for string types: " << condition << std::endl;
    } else {
        std::cerr << "Type error: incompatible types in condition: " << condition << std::endl;
    }

    return false;
}

void Parser::handleExpression(const std::string& content)
{
    Variable result = evaluateExpression(content);

    if (result.type == VariableType::NUMBER)
    {
        // Check if the number is an integer
        if (result.numberValue == static_cast<int>(result.numberValue))
        {
            std::cout << static_cast<int>(result.numberValue) << std::endl;
        }
        else
        {
            std::cout << result.numberValue << std::endl;
        }
    }
    else
    {
        std::cout << result.value << std::endl;
    }
}

Variable Parser::evaluateExpression(const std::string& expression) {
    std::istringstream iss(expression);
    return parseExpression(iss);
}

Variable Parser::parseExpression(std::istringstream& iss) {
    Variable left = parseTerm(iss);
    while (true) {
        std::string token;
        iss >> token;
        if (token == "+" || token == "-") {
            Variable right = parseTerm(iss);
            if (left.type == VariableType::NUMBER && right.type == VariableType::NUMBER) {
                if (token == "+") {
                    left.numberValue += right.numberValue;
                } else {
                    left.numberValue -= right.numberValue;
                }
            } else {
                std::string leftValue = (left.type == VariableType::STRING) ? left.value : formatNumber(left.numberValue);
                std::string rightValue = (right.type == VariableType::STRING) ? right.value : formatNumber(right.numberValue);
                left = Variable("", leftValue + rightValue);
            }
        } else {
            iss.putback(token[0]);
            break;
        }
    }
    return left;
}

Variable Parser::parseTerm(std::istringstream& iss) {
    Variable left = parseFactor(iss);
    while (true) {
        std::string token;
        iss >> token;
        if (token == "*" || token == "/") {
            Variable right = parseFactor(iss);
            if (left.type == VariableType::NUMBER && right.type == VariableType::NUMBER) {
                if (token == "*") {
                    left.numberValue *= right.numberValue;
                } else {
                    if (right.numberValue != 0) {
                        left.numberValue /= right.numberValue;
                    } else {
                        std::cerr << "Division by zero error" << std::endl;
                        return Variable("", 0.0);
                    }
                }
            } else {
                std::cerr << "Unsupported operation for non-numeric types: " << left.value << " " << token << " " << right.value << std::endl;
                return Variable("", 0.0);
            }
        } else {
            iss.putback(token[0]);
            break;
        }
    }
    return left;
}

Variable Parser::parseFactor(std::istringstream& iss) {
    std::string token;
    iss >> token;
    if (isNumber(token)) {
        return Variable(token, std::stod(token));
    } else if (token == "random") {
        return parseRandomFunction(iss);
    } else if (token == "(") {
        Variable result = parseExpression(iss);
        iss >> token; // Expecting ')'
        if (token != ")") {
            std::cerr << "Syntax error: expected ')' but got " << token << std::endl;
            return Variable("", 0.0);
        }
        return result;
    } else {
        auto it = variables.find(token);
        if (it != variables.end()) {
            return it->second;
        } else {
            std::cerr << "Undefined variable: " << token << std::endl;
            return Variable("", 0.0);
        }
    }
}

std::string Parser::formatNumber(double number) {
    std::ostringstream oss;
    // Check if the number is an integer
    if (number == static_cast<int>(number)) {
        oss << static_cast<int>(number);
    } else {
        oss << std::fixed << std::setprecision(2) << number;
    }
    return oss.str();
}

bool Parser::isNumber(const std::string& s) {
    char* end = nullptr;
    double val = std::strtod(s.c_str(), &end);
    return end != s.c_str() && *end == '\0' && val != HUGE_VAL;
}

void Parser::parseRandom() {
    advanceToken(); // consume 'random'
    if (currentToken.type != TokenType::TOKEN_NUMBER) {
        std::cerr << "Syntax error: expected a number as the first argument to 'random'" << std::endl;
        exit(1);
    }

    int minValue = 0;
    int maxValue = 0;

    // Parse min value
    if (currentToken.type == TokenType::TOKEN_NUMBER) {
        minValue = std::stoi(currentToken.value);
        advanceToken(); // consume the min value
    } else {
        std::cerr << "Syntax error: expected a number as the first argument to 'random'" << std::endl;
        exit(1);
    }

    if (currentToken.type != TokenType::TOKEN_NUMBER) {
        std::cerr << "Syntax error: expected a number as the second argument to 'random'" << std::endl;
        exit(1);
    }

    // Parse max value
    if (currentToken.type == TokenType::TOKEN_NUMBER) {
        maxValue = std::stoi(currentToken.value);
        advanceToken(); // consume the max value
    } else {
        std::cerr << "Syntax error: expected a number as the second argument to 'random'" << std::endl;
        exit(1);
    }

    // Generate the random number
    int randomNumber = generateRandomNumber(minValue, maxValue);
    std::cout << randomNumber << std::endl;

    if (currentToken.type == TokenType::TOKEN_EOL) {
        advanceToken(); // consume EOL if present
    }
}

Variable Parser::parseRandomFunction(std::istringstream& iss) {
    int minValue = 0;
    int maxValue = 0;
    std::string token;

    // Parse min value
    iss >> token;
    if (isNumber(token)) {
        minValue = std::stoi(token);
    } else {
        std::cerr << "Syntax error: expected a number as the first argument to 'random'" << std::endl;
        return Variable("", 0.0);
    }

    // Parse max value
    iss >> token;
    if (isNumber(token)) {
        maxValue = std::stoi(token);
    } else {
        std::cerr << "Syntax error: expected a number as the second argument to 'random'" << std::endl;
        return Variable("", 0.0);
    }

    // Generate the random number
    int randomNumber = generateRandomNumber(minValue, maxValue);
    return Variable("", static_cast<double>(randomNumber));
}

// Helper function to generate random numbers
int Parser::generateRandomNumber(int minValue, int maxValue) {
    std::random_device rd; // Obtain a random number from hardware
    std::mt19937 eng(rd()); // Seed the generator
    std::uniform_int_distribution<> distr(minValue, maxValue); // Define the range

    return distr(eng);
}

bool Parser::hasGumExtension(const std::string& filename)
{
    return filename.size() >= 5 && filename.substr(filename.size() - 4) == ".gum";
}
