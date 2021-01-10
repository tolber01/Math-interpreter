#pragma once
#include <string>
#include <vector>

enum ExpressionType
{
    UNRECOGNIZED,
    VARIABLE,
    VALUE,
    UNARY,
    BINARY
};

enum TokenType
{
    TOKEN_VARIABLE,
    TOKEN_MATRIX,
    TOKEN_RATIONAL,
    TOKEN_UNARY,
    TOKEN_BINARY
};

struct Token
{
    Token(TokenType t, std::string v);
    Token() = default;
    inline TokenType get_type() { return type; }
    inline std::string get_value() { return value; }
private:
    TokenType type;
    std::string value;
};

struct Expression
{
    Expression(bool correct, ExpressionType type, std::vector<Token> parts);
    inline bool is_correct() const { return correct; }
    inline ExpressionType get_type() { return type; }
    Token inline operator[](int i) { return parts[i]; }
private:
    bool correct;
    ExpressionType type;
    std::vector<Token> parts;
};