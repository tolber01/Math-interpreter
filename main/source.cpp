#include "interpreter.hpp"

// Project build from math_interpreter root directory:
// c++ parsing/*.cpp types/*.cpp execution/*.cpp main/*.cpp -o interpreter.exe

int main(int argc, char const* argv[])
{
    if (argc > 1)
    {
        Interpreter interpreter(argv[1]);
        interpreter.run();
    }
    else
    {
        Interpreter interpreter;
        interpreter.run();
    }

    return 0;
}