#pragma once
#include <fstream>
#include "../execution/context.hpp"
#include "../parsing/parser.hpp"

struct Interpreter
{
    Interpreter();
    explicit Interpreter(char const* path);
    ~Interpreter();
    void run();
private:
    void run_from_console();
    void run_from_file();

    bool from_file;
    std::ifstream* file;
    Context* context;
};