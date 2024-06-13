#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>
#include <cctype>
#include <iostream>
#include "token.hpp"

class Lexer {
public:
    Lexer(const std::string& source);
    Token getNextToken();

private:
    std::string source;
    size_t index;
    int line;
    int column;
    char currentChar;

    void advance();
    void skipWhitespace();
    void skipSingleLineComment();
    void skipMultiLineComment();
    Token identifier();
    Token number();
    Token string();
    Token makeToken(TokenType type, const std::string& value);
};

#endif // LEXER_HPP

