#pragma once
#include <unordered_map>
#include "../types/var_types.hpp"
#include "../parsing/expression.hpp"

struct Context
{
    static std::string const TEMP_VAR;
    Context();
    ~Context();

    void update_variable(const std::string& var_name, GenericValue* value);

    bool expression_to_TEMP(Expression expression);

    inline bool has_variable(const std::string& var_name) { return (variables.find(var_name) != variables.end()); };
    inline bool has_unary_function(const std::string& func_name)
    { return (unary_functions.find(func_name) != unary_functions.end()); }
    inline GenericValue* get_variable(const std::string& var_name) { return variables.at(var_name); }

    void copy_to(const std::string& from_var, const std::string& to_var);
private:
    std::unordered_map<std::string, GenericValue*> variables;
    std::unordered_map<std::string, bool (*)(GenericValue*, GenericValue**)> unary_functions;
};