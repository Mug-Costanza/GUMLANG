// token.hpp
#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

enum class TokenType {
    TOKEN_UNKNOWN,
    TOKEN_EOF,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_ASSIGN,
    TOKEN_IF,
    TOKEN_THEN,
    TOKEN_ELSE,
    TOKEN_PRINT,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_OPERATOR,
    TOKEN_EOL
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};

#endif // TOKEN_HPP

