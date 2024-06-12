// token.hpp
#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

enum class TokenType {
    TOKEN_EOF,
    TOKEN_EOL,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_ASSIGN,
    TOKEN_OPERATOR,
    TOKEN_OPERATOR_PLUSEQUAL, // Added this line
    TOKEN_OPERATOR_MINUSEQUAL, // Added this line
    TOKEN_OPERATOR_STAREQUAL, // Added this line
    TOKEN_OPERATOR_SLASHEQUAL, // Added this line
    TOKEN_IF,
    TOKEN_THEN,
    TOKEN_ELSE,
    TOKEN_PRINT,
    TOKEN_FOR,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};

#endif // TOKEN_HPP

