#include <iostream>
#include "parser.hpp"

using namespace std;

string trim(string s)
{
    string chars_to_remove = " \t";
    s.erase(0, s.find_first_not_of(chars_to_remove));
    s.erase(s.find_last_not_of(chars_to_remove) + 1);
    return s;
}

Token::Token(TokenType t, string v) :
    type(t), value(std::move(v))
{}

const string VAR_NAME_REG_EXP_STR = "[a-zA-Z_]\\w*";
const regex VAR_NAME_REG_EXP = regex(VAR_NAME_REG_EXP_STR);
const regex UNARY_REG_EXP = regex( VAR_NAME_REG_EXP_STR + R"(\([^\(\)]+\))");
const regex BINARY_OPERATOR_REG_EXP = regex("[*+-/]");
const string EXIT_STRING = "EXIT";

Expression::Expression(bool correct, ExpressionType type, vector<Token> parts) :
        correct(correct), type(type), parts(std::move(parts))
{}

bool is_correct_var_name(const string& var_name)
{
    smatch var_name_match;
    return (regex_match(var_name, var_name_match, VAR_NAME_REG_EXP) && var_name != Context::TEMP_VAR);
}

Expression parse_expression(string expression)
{
    smatch binary_operator_match, unary_match;
    string expression_copy = expression;
    if (is_correct_var_name(expression))
    {
        vector<Token> parts(1);
        parts[0] = Token(TOKEN_VARIABLE, expression);
        return Expression(true, VARIABLE, parts);
    }
    else if (RationalNumber::is_correct_str(expression))
    {
        vector<Token> parts(1);
        parts[0] = Token(TOKEN_RATIONAL, expression);
        return Expression(true, VALUE, parts);
    }
    else if (Matrix::is_correct_str(expression))
    {
        vector<Token> parts(1);
        parts[0] = Token(TOKEN_MATRIX, expression);
        return Expression(true, VALUE, parts);
    }
    else if (expression[0] == '-' || regex_match(expression, unary_match, UNARY_REG_EXP))
    {
        string unary_function, argument;
        vector<Token> parts(2);

        if (expression[0] != '-')
        {
            string::size_type first_par_pos = expression.find('(');
            unary_function = expression.substr(0, first_par_pos);
            argument = expression.substr(first_par_pos + 1, expression.size() - first_par_pos - 2);
        }
        else
        {
            unary_function = "-";
            argument = expression.substr(1);
        }
        parts[0] = Token(TOKEN_UNARY, unary_function);

        if (is_correct_var_name(argument))
            parts[1] = Token(TOKEN_VARIABLE,  argument);
        else if (RationalNumber::is_correct_str(argument))
            parts[1] = Token(TOKEN_RATIONAL,  argument);
        else if (Matrix::is_correct_str(argument))
            parts[1] = Token(TOKEN_MATRIX, argument);
        else
            return Expression(false, UNARY, parts);
        return Expression(true, UNARY, parts);
    }
    else if (regex_search(expression_copy, binary_operator_match, BINARY_OPERATOR_REG_EXP))
    {
        Token op(TOKEN_BINARY, binary_operator_match[0]);

        string::size_type op_pos = binary_operator_match.position(0);
        int matches_number = 1;
        expression_copy = binary_operator_match.suffix().str();
        while (regex_search(expression_copy, binary_operator_match, BINARY_OPERATOR_REG_EXP))
        {
            matches_number++;
            expression_copy = binary_operator_match.suffix().str();
        }
        if (matches_number != 1)
            return Expression(false, BINARY, vector<Token>());

        string first_operand = trim(expression.substr(0, op_pos));
        string second_operand = trim(expression.substr(op_pos + 1));

        Token first, second;
        if (is_correct_var_name(first_operand))
            first = Token(TOKEN_VARIABLE, first_operand);
        else if (RationalNumber::is_correct_str(first_operand))
            first = Token(TOKEN_RATIONAL, first_operand);
        else if (Matrix::is_correct_str(first_operand))
            first = Token(TOKEN_MATRIX, first_operand);
        else
            return Expression(false, BINARY, vector<Token>());

        if (is_correct_var_name(second_operand))
            second = Token(TOKEN_VARIABLE, second_operand);
        else if (RationalNumber::is_correct_str(second_operand))
            second = Token(TOKEN_RATIONAL, second_operand);
        else if (Matrix::is_correct_str(second_operand))
            second = Token(TOKEN_MATRIX, second_operand);
        else
            return Expression(false, BINARY, vector<Token>());

        vector<Token> parts(3);
        parts[0] = op;
        parts[1] = first;
        parts[2] = second;
        return Expression(true, BINARY, parts);
    }
    else
        return Expression(false, UNRECOGNIZED, vector<Token>());
}

Command* parse_command(string command_string)
{
    command_string = trim(command_string);
    if (command_string.empty())
        return new Command(true, EMPTY);
    else if (command_string == EXIT_STRING)
        return new Command(true, EXIT);

    string::size_type eq_pos = command_string.find('=');
    if (eq_pos != string::npos)
    {
        string variable_name = command_string.substr(0, eq_pos);
        variable_name = trim(variable_name);

        string expression = command_string.substr(eq_pos + 1);
        expression = trim(expression);
        if (expression.empty())
        {
            cout << "... ";
            getline(cin, expression);
            expression = trim(expression);
        }

        Expression exp = parse_expression(expression);

        if (!exp.is_correct())
        {
            cerr << "This assignment has invalid syntax." << endl;
            return new Command(false, ASSIGN);
        }
        else
            return new Assignment(true, variable_name, exp);
    }
    else
    {
        Expression exp = parse_expression(command_string);
        if (!exp.is_correct())
        {
            cerr << "This output command has invalid syntax." << endl;
            return new Command(false, OUTPUT);
        }
        else
            return new Output(true, exp);
    }
}