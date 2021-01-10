#include <iostream>
#include "../parsing/parser.hpp"

using namespace std;

Command::Command(bool correct, CommandCode c) :
        correct(correct), c(c)
{}

bool Command::run(Context* context)
{
    return false;
}

Assignment::Assignment(bool correct, string variable, Expression value) :
        Command(correct, ASSIGN), variable_name(std::move(variable)), value(std::move(value))
{}

bool Assignment::run(Context* context)
{
    if (context->expression_to_TEMP(value))
    {
        context->copy_to(Context::TEMP_VAR, variable_name);
        return true;
    }
    return false;
}

Output::Output(bool correct, Expression value) :
    Command(correct, OUTPUT), value(std::move(value))
{}

bool Output::run(Context* context)
{
    if (context->expression_to_TEMP(value))
    {
        GenericValue* to_print = context->get_variable(Context::TEMP_VAR);
        cout << to_print->to_string() << endl;
        return true;
    }
    else
    {
        cerr << "Incorrect expression" << endl;
        return false;
    }
}
