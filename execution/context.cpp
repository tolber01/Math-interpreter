#include "../execution/context.hpp"

using namespace std;

string const Context::TEMP_VAR = "TEMP";

Context::Context() :
    variables(unordered_map<string, GenericValue*>()),
    unary_functions(unordered_map<string, bool (*)(GenericValue*, GenericValue**)>())
{
    unary_functions.emplace("T", T);
    unary_functions.emplace("-", unary_minus);
}

Context::~Context()
{
    for_each(variables.begin(), variables.end(),
             [](const pair<string, GenericValue*>& element) { delete element.second; });
}

void Context::update_variable(const string& var_name, GenericValue* value)
{
    if (has_variable(var_name))
    {
        delete variables.at(var_name);
        variables.erase(var_name);
    }
    variables.emplace(var_name, value);
}

bool Context::expression_to_TEMP(Expression expression)
{
    if (expression.get_type() == VARIABLE)
    {
        if (!has_variable(expression[0].get_value()))
            return false;
        copy_to(expression[0].get_value(), TEMP_VAR);
        return true;
    }
    else if (expression.get_type() == VALUE)
    {
        if (expression[0].get_type() == TOKEN_RATIONAL)
        {
            GenericValue* value = new RationalNumber(expression[0].get_value());
            update_variable(TEMP_VAR, value);
            return true;
        }
        else if (expression[0].get_type() == TOKEN_MATRIX)
        {
            GenericValue* value = new Matrix(expression[0].get_value());
            update_variable(TEMP_VAR, value);
            return true;
        }
        else
            return false;
    }
    else if (expression.get_type() == UNARY)
    {
        string function_name = expression[0].get_value();
        if (!has_unary_function(function_name))
            return false;

        GenericValue* object_copy;
        auto result = new GenericValue*();

        if (expression[1].get_type() == TOKEN_VARIABLE)
        {
            if (!has_variable(expression[1].get_value()))
                return false;
            object_copy = variables.at(expression[1].get_value())->clone();
        }
        else if (expression[1].get_type() == TOKEN_RATIONAL)
            object_copy = new RationalNumber(expression[1].get_value());
        else if (expression[1].get_type() == TOKEN_MATRIX)
            object_copy = new Matrix(expression[1].get_value());
        else
            return false;

        if (!unary_functions.at(function_name)(object_copy, result))
            return false;
        update_variable(TEMP_VAR, *result);
        delete object_copy; delete result;
        return true;
    }
    else if (expression.get_type() == BINARY)
    {
        GenericValue* left, *right;
        if (expression[1].get_type() == TOKEN_VARIABLE)
        {
            if (!has_variable(expression[1].get_value()))
                return false;
            left = variables.at(expression[1].get_value())->clone();
        }
        else if (expression[1].get_type() == TOKEN_RATIONAL)
            left = new RationalNumber(expression[1].get_value());
        else if (expression[1].get_type() == TOKEN_MATRIX)
            left = new Matrix(expression[1].get_value());

        if (expression[2].get_type() == TOKEN_VARIABLE)
        {
            if (!has_variable(expression[2].get_value()))
                return false;
            right = variables.at(expression[2].get_value())->clone();
        }
        else if (expression[2].get_type() == TOKEN_RATIONAL)
            right = new RationalNumber(expression[2].get_value());
        else if (expression[2].get_type() == TOKEN_MATRIX)
            right = new Matrix(expression[2].get_value());

        auto result = new GenericValue*();
        *result = nullptr;
        switch (expression[0].get_value()[0])
        {
            case '+':
                if (!add(left, right, result))
                    return false;
                break;
            case '-':
                if (!subtract(left, right, result))
                    return false;
                break;
            case '*':
                if (!multiply(left, right, result))
                    return false;
                break;
            case '/':
                if (!divide(left, right, result))
                    return false;
                break;
        }
        update_variable(TEMP_VAR, *result);
        delete left; delete right; delete result;
        return true;
    }
    else
        return false;
}

void Context::copy_to(const string& from_var, const string& to_var)
{
    update_variable(to_var, variables.at(from_var)->clone());
}