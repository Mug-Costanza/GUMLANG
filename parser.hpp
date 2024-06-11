#ifndef PARSER_HPP
#define PARSER_HPP

#include "lexer.hpp"
#include "variable.hpp"
#include <fstream>
#include <ostream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

namespace GUMLANG {

class Parser {
public:
    Parser(const std::string& filename);
    void InterpretFile();

private:
    void parseIfStatement();
    void parseSingleLineStatement();
    void parsePrintStatement();
    void parseLine();
    void parseBlock();
    void skipBlock();
    bool evaluateCondition(const std::string& condition);
    Variable evaluateExpression(const std::string& expression);
    void handleExpression(const std::string& content);

    void parseVariableDeclaration(const std::string& varName);
    void parseAssignment(const std::string& varName);

    bool isNumber(const std::string& s);
    bool hasGumExtension(const std::string& filename);

    std::ifstream file;
    Lexer lexer;
    Token currentToken;
    std::unordered_map<std::string, Variable> variables;
    bool isGumSourceFile = true;

    void advanceToken();
    void expectToken(TokenType type);
};

} // namespace GUMLANG

#endif // PARSER_HPP


