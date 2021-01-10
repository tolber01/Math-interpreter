#include <iostream>
#include "interpreter.hpp"

using namespace std;

Interpreter::Interpreter() :
    from_file(false), file(nullptr), context(new Context())
{}

Interpreter::Interpreter(char const* path) :
    from_file(true), file(new ifstream(path)), context(new Context())
{}

Interpreter::~Interpreter()
{
    delete context;
    delete file;
}

void Interpreter::run()
{
    if (from_file)
        run_from_file();
    else
        run_from_console();
}

void Interpreter::run_from_console()
{
    Command* command;
    string command_string;

    cout << "<===| Simple math interpreter |===>" << endl;
    cout << "=> ";
    while (getline(cin, command_string))
    {
        command = parse_command(command_string);
        if (command->code() == EXIT)
            break;
        command->run(context);
        delete command;
        cout << "=> ";
    }
}

void Interpreter::run_from_file()
{
    if (!file->good())
    {
        cerr << "Error with reading file. Turning on console mode." << endl;
        run_from_console();
    }
    else
    {
        Command* command;
        string command_string;
        while (getline(*file, command_string))
        {
            command = parse_command(command_string);
            if (!command->is_correct() || command->code() == EXIT)
                break;
            if (!command->run(context))
            {
                cout << "Error with running command!\n";
                delete command;
                break;
            }
            delete command;
        }
        file->close();
    }
}