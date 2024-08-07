#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

enum class TokenType {
    TOKEN_EOF,
    TOKEN_EOL,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_IF,
    TOKEN_THEN,
    TOKEN_ELSE,
    TOKEN_ELSEIF,
    TOKEN_PRINT,
    TOKEN_FOR,
    TOKEN_ASSIGN,
    TOKEN_OPERATOR,
    TOKEN_OPERATOR_PLUSEQUAL,
    TOKEN_OPERATOR_MINUSEQUAL,
    TOKEN_OPERATOR_STAREQUAL,
    TOKEN_OPERATOR_SLASHEQUAL,
    TOKEN_OPERATOR_INCREMENT,
    TOKEN_OPERATOR_DECREMENT,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_COMMA,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_RANDOM,
    TOKEN_UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};

#endif // TOKEN_HPP
