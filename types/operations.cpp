#include "var_types.hpp"

// ==== Binary operations implementation ====

bool add(GenericValue* left, GenericValue* right, GenericValue** result)
{
    if (left->get_type() == RATIONAL_NUMBER)
    {
        if (right->get_type() != RATIONAL_NUMBER)
            return false;
        auto first = dynamic_cast<RationalNumber*>(left);
        auto second = dynamic_cast<RationalNumber*>(right);
        *result = new RationalNumber(*first + *second);
        return true;
    }
    else if (left->get_type() == MATRIX)
    {
        if (right->get_type() != MATRIX)
            return false;
        auto first = dynamic_cast<Matrix*>(left);
        auto second = dynamic_cast<Matrix*>(right);
        if (!first->has_same_size(*second))
            return false;
        *result = new Matrix(*first + *second);
        return true;
    }
    else
        return false;
}

bool subtract(GenericValue* left, GenericValue* right, GenericValue** result)
{
    if (left->get_type() == RATIONAL_NUMBER)
    {
        if (right->get_type() != RATIONAL_NUMBER)
            return false;
        auto first = dynamic_cast<RationalNumber*>(left);
        auto second = dynamic_cast<RationalNumber*>(right);
        *result = new RationalNumber(*first - *second);
        return true;
    }
    else if (left->get_type() == MATRIX)
    {
        if (right->get_type() != MATRIX)
            return false;
        auto first = dynamic_cast<Matrix*>(left);
        auto second = dynamic_cast<Matrix*>(right);
        if (!first->has_same_size(*second))
            return false;
        *result = new Matrix(*first - *second);
        return true;
    }
    else
        return false;
}

bool multiply(GenericValue* left, GenericValue* right, GenericValue** result)
{
    if (left->get_type() == RATIONAL_NUMBER)
    {
        auto first = dynamic_cast<RationalNumber*>(left);
        if (right->get_type() == RATIONAL_NUMBER)
        {
            auto second = dynamic_cast<RationalNumber*>(right);
            *result = new RationalNumber(*second * *first);
            return true;
        }
        else if (right->get_type() == MATRIX)
        {
            auto second = dynamic_cast<Matrix*>(right);
            *result = new Matrix(*second * *first);
            return true;
        }
        else
            return false;
    }
    else if (left->get_type() == MATRIX)
    {
        auto first = dynamic_cast<Matrix*>(left);
        if (right->get_type() == RATIONAL_NUMBER)
        {
            auto second = dynamic_cast<RationalNumber*>(right);
            *result = new Matrix(*first * *second);
            return true;
        }
        else if (right->get_type() == MATRIX)
        {
            auto second = dynamic_cast<Matrix*>(right);
            if (!first->is_multipliable_with(*second))
                return false;
            *result = new Matrix(*first * *second);
            return true;
        }
        else
            return false;
    }
    else
        return false;
}

bool divide(GenericValue* left, GenericValue* right, GenericValue** result)
{
    if (left->get_type() == RATIONAL_NUMBER)
    {
        auto first = dynamic_cast<RationalNumber*>(left);
        if (right->get_type() != RATIONAL_NUMBER)
            return false;
        auto second = dynamic_cast<RationalNumber*>(right);
        if (second->num() == 0)
            return false;
        *result = new RationalNumber(*first / *second);
        return true;
    }
    else
        return false;
}

// ==== Unary operations implementation ====

bool T(GenericValue* argument, GenericValue** result)
{
    if (argument->get_type() != MATRIX)
        return false;
    auto matrix_argument = dynamic_cast<Matrix*>(argument);
    auto copy = new Matrix(*matrix_argument);
    copy->transpose();
    *result = copy;
    return true;
}

bool unary_minus(GenericValue* argument, GenericValue** result)
{
    if (argument->get_type() == MATRIX)
    {
        auto matrix_argument = dynamic_cast<Matrix*>(argument);
        *result = new Matrix(-(*matrix_argument));
    }
    else if (argument->get_type() == RATIONAL_NUMBER)
    {
        auto rational_argument = dynamic_cast<RationalNumber*>(argument);
        *result = new RationalNumber(-(*rational_argument));
    }
    return true;
}
