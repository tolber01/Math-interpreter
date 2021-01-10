#pragma once
#include "../execution/commands.hpp"

std::string trim(std::string s);

extern const std::string VAR_NAME_REG_EXP_STR;
extern const std::regex VAR_NAME_REG_EXP;
extern const std::regex UNARY_REG_EXP;
extern const std::regex BINARY_OPERATOR_REG_EXP;
extern const std::string EXIT_STRING;

bool is_correct_var_name(const std::string& var_name);
Command* parse_command(std::string command_string);
Expression parse_expression(std::string expression);