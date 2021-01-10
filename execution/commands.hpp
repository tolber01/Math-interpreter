#pragma once
#include "context.hpp"

enum CommandCode
{
    EMPTY,
    EXIT,
    ASSIGN,
    OUTPUT,
};

// ==== Command base class declaration ====

struct Command
{
    Command(bool correct, CommandCode c);
    virtual bool run(Context* context);
    inline bool is_correct() const { return correct; }
    inline CommandCode code() const { return c; }
protected:
    bool correct;
    CommandCode c;
};

// ==== Assignment command class declaration ====

struct Assignment : Command
{
    Assignment(bool correct, std::string variable, Expression value);
    bool run(Context* context) override;
private:
    std::string variable_name;
    Expression value;
};

// ==== Output command class declaration ====

struct Output : Command
{
    Output(bool correct, Expression value);
    bool run(Context* context) override;
private:
    Expression value;
};