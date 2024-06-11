#include "lexer.hpp"
#include <cctype>
#include <iostream>

Lexer::Lexer(const std::string& source)
    : source(source), index(0), line(1), column(1), currentChar(source[index]) {}

void Lexer::advance() {
    if (currentChar != '\0') {
        if (currentChar == '\n') {
            line++;
            column = 0;
        }
        index++;
        if (index >= source.size()) {
            currentChar = '\0';
        } else {
            currentChar = source[index];
        }
        column++;
    }
}

void Lexer::skipWhitespace() {
    while (isspace(currentChar) && currentChar != '\n') {
        advance();
    }
}

Token Lexer::identifier() {
    std::string value;
    while (isalnum(currentChar) || currentChar == '_') {
        value += currentChar;
        advance();
    }
    if (value == "if") return makeToken(TokenType::TOKEN_IF, value);
    if (value == "then") return makeToken(TokenType::TOKEN_THEN, value);
    if (value == "else") return makeToken(TokenType::TOKEN_ELSE, value);
    if (value == "print") return makeToken(TokenType::TOKEN_PRINT, value);
    return makeToken(TokenType::TOKEN_IDENTIFIER, value);
}

Token Lexer::number() {
    std::string value;
    while (isdigit(currentChar) || currentChar == '.') {
        value += currentChar;
        advance();
    }
    return makeToken(TokenType::TOKEN_NUMBER, value);
}

Token Lexer::string() {
    std::string value;
    advance(); // Skip the opening quote
    while (currentChar != '"' && currentChar != '\0') {
        value += currentChar;
        advance();
    }
    advance(); // Skip the closing quote
    return makeToken(TokenType::TOKEN_STRING, value);
}

Token Lexer::makeToken(TokenType type, const std::string& value) {
    return {type, value, line, column};
}

Token Lexer::getNextToken() {
    skipWhitespace();

    if (currentChar == '\0') return makeToken(TokenType::TOKEN_EOF, "");

    if (currentChar == '\n') {
        advance();
        return makeToken(TokenType::TOKEN_EOL, "\\n");
    }

    if (isalpha(currentChar) || currentChar == '_') return identifier();

    if (isdigit(currentChar)) return number();

    if (currentChar == '"') return string();

    if (currentChar == '=') {
        advance();
        if (currentChar == '=') {
            advance();
            return makeToken(TokenType::TOKEN_OPERATOR, "==");
        }
        return makeToken(TokenType::TOKEN_ASSIGN, "=");
    }

    if (currentChar == '{') {
        advance();
        return makeToken(TokenType::TOKEN_LBRACE, "{");
    }

    if (currentChar == '}') {
        advance();
        return makeToken(TokenType::TOKEN_RBRACE, "}");
    }

    if (currentChar == '>' || currentChar == '<' || currentChar == '!' || currentChar == '+' || currentChar == '-' || currentChar == '*' || currentChar == '/') {
        char op = currentChar;
        advance();
        if (currentChar == '=') {
            std::string value(1, op);
            value += '=';
            advance();
            return makeToken(TokenType::TOKEN_OPERATOR, value);
        }
        return makeToken(TokenType::TOKEN_OPERATOR, std::string(1, op));
    }

    std::cerr << "Unexpected character: " << currentChar << " at line " << line << ", column " << column << std::endl;
    advance();
    return makeToken(TokenType::TOKEN_UNKNOWN, std::string(1, currentChar));
}

