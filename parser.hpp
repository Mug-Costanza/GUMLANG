#ifndef PARSER_HPP
#define PARSER_HPP

#include "lexer.hpp"
#include "variable.hpp"
#include <fstream>
#include <ostream>
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <unordered_map>
#include <random>

namespace GUMLANG {

class Parser {
public:
    Parser(const std::string& filename);
    void InterpretFile();

private:
    void parseIfStatement();
    void handleElseIfOrElse();
    void parseSingleLineStatement();
    void parseForLoop();
    void parsePrintStatement();
    void parseAdditionAssignment(const std::string& varName);
    void parseSubtractionAssignment(const std::string& varName);
    void parseMultiplicationAssignment(const std::string& varName);
    void parseDivisionAssignment(const std::string& varName);
    void parseIncrement(const std::string& varName);
    void parseDecrement(const std::string& varName);
    void parseLine();
    void parseBlock();
    void skipBlock();
    bool evaluateCondition(const std::string& condition);
    Variable evaluateExpression(const std::string& expression);
    Variable parseExpression(std::istringstream& iss);
    Variable parseTerm(std::istringstream& iss);
    Variable parseFactor(std::istringstream& iss);
    void handleExpression(const std::string& content);

    void parseVariableDeclaration(const std::string& varName);
    void parseAssignment(const std::string& varName);
    void skipRemainingBlocks();

    std::string formatNumber(double number);
    int generateRandomNumber(int minValue, int maxValue);
    void parseRandom();
    Variable parseStringLiteral(std::istringstream& iss);
    Variable parseRandomFunction(std::istringstream& iss);
    bool isNumber(const std::string& s);
    bool hasGumExtension(const std::string& filename);
    
    Variable parseVariable();
    Variable parseExpression();
    Variable parse();

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


