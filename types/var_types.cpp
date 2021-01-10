#include "../execution/context.hpp"
#include "../parsing/parser.hpp"

using namespace std;

// ==== GenericValue implementation ====

GenericValue::GenericValue(ValueType object_type) :
        type(object_type)
{}

GenericValue* GenericValue::clone() { return new GenericValue(*this); }

// ==== RationalNumber implementation ====

const string RationalNumber::REG_EXP_STR = R"((?:-?\d+\s*/\s*\d+|-?\d+))";
const regex RationalNumber::REG_EXP = regex(RationalNumber::REG_EXP_STR);

bool RationalNumber::is_correct_str(std::string str_num)
{
    str_num = trim(str_num);
    smatch number_match;
    if (!regex_match(str_num, number_match, REG_EXP))
        return false;
    string::size_type slash_pos = str_num.find('/');
    int numerator, denominator;
    if (slash_pos == string::npos)
    {
        try
        {
            numerator = stoi(str_num);
        }
        catch (invalid_argument& e) { return false; }
        return true;
    }
    else
    {
        try
        {
            numerator = stoi(str_num.substr(0, slash_pos));
            denominator = stoi(str_num.substr(slash_pos + 1));
            if (denominator == 0)
                throw invalid_argument("Zero division error");
        }
        catch (invalid_argument& e) { return false; }
        return true;
    }
}

void RationalNumber::simplify()
{
    for (int d = 2; d <= denominator; d++)
        while (numerator % d == 0 && denominator % d == 0)
        {
            numerator /= d;
            denominator /= d;
        }
}

RationalNumber::RationalNumber(string str_num) :
        GenericValue(RATIONAL_NUMBER)
{
    str_num = trim(str_num);
    string::size_type slash_pos = str_num.find('/');
    if (slash_pos != string::npos)
    {
        numerator = stoi(str_num.substr(0, slash_pos));
        denominator = stoi(str_num.substr(slash_pos + 1));
    }
    else
    {
        numerator = stoi(str_num);
        denominator = 1;
    }
    simplify();
}

RationalNumber::RationalNumber() :
        GenericValue(RATIONAL_NUMBER), numerator(0), denominator(1)
{}

RationalNumber::RationalNumber(int num, int den) :
        GenericValue(RATIONAL_NUMBER), numerator(num), denominator(den)
{
    simplify();
}

RationalNumber::RationalNumber(const RationalNumber& other) :
        GenericValue(RATIONAL_NUMBER), numerator(other.numerator), denominator(other.denominator)
{}

GenericValue* RationalNumber::clone() { return new RationalNumber(*this); }

RationalNumber& RationalNumber::operator=(const RationalNumber& other)
{
    numerator = other.numerator;
    denominator = other.denominator;
    return *this;
}

string RationalNumber::to_string() const
{
    if (denominator == 1)
        return std::to_string(numerator);
    else
        return std::to_string(numerator) + "/" + std::to_string(denominator);
}

RationalNumber RationalNumber::operator+(const RationalNumber& other) const
{
    return RationalNumber(
        numerator*other.denominator + denominator*other.numerator,
        denominator*other.denominator);
}

RationalNumber RationalNumber::operator-() const
{
    return RationalNumber(-numerator, denominator);
}

RationalNumber RationalNumber::operator-(const RationalNumber& other) const
{
    return *this + (-other);
}

RationalNumber RationalNumber::operator*(const RationalNumber& other) const
{
    return RationalNumber(numerator*other.numerator, denominator*other.denominator);
}

RationalNumber RationalNumber::operator/(const RationalNumber& other) const
{
    return RationalNumber(numerator*other.denominator, denominator*other.numerator);
}


// ==== Matrix implementation ====

const regex Matrix::ROW_REG_EXP = regex(
        "(?:" + RationalNumber::REG_EXP_STR + "\\s*)+"
);
const regex Matrix::REG_EXP = regex(
        "\\[\\s*(?:(?:" + RationalNumber::REG_EXP_STR + "\\s+)*" + RationalNumber::REG_EXP_STR + "\\s*;\\s*)*(?:"
        + RationalNumber::REG_EXP_STR + "\\s+)*" + RationalNumber::REG_EXP_STR + "\\s*\\]"
);

bool Matrix::is_correct_str(string str_matrix)
{
    str_matrix = trim(str_matrix);

    cmatch matrix_match_result;

    if (!regex_match(str_matrix.c_str(), matrix_match_result, Matrix::REG_EXP))
        return false;
    else
    {
        smatch rows_match, columns_match, elements_match;

        if (!regex_search(str_matrix, rows_match, Matrix::ROW_REG_EXP))
            return false;
        string row = rows_match[0];

        int columns_number = 0, c;
        while (regex_search(row, columns_match, RationalNumber::REG_EXP))
        {
            if (!RationalNumber::is_correct_str(columns_match[0]))
                return false;
            columns_number++;
            row = columns_match.suffix().str();
        }
        str_matrix = rows_match.suffix().str();

        while (regex_search(str_matrix, rows_match, Matrix::ROW_REG_EXP))
        {
            row = rows_match[0];
            c = 0;
            while (regex_search(row, columns_match, RationalNumber::REG_EXP))
            {
                if (!RationalNumber::is_correct_str(columns_match[0]))
                    return false;
                c++;
                row = columns_match.suffix().str();
            }
            if (c != columns_number)
                return false;
            str_matrix = rows_match.suffix().str();
        }

        return true;
    }    
}

Matrix::Matrix(string str_matrix) :
        GenericValue(MATRIX)
{
    string str_copy = str_matrix, row;
    str_matrix = trim(str_matrix);
    smatch elements_match, columns_match, rows_match;

    regex_search(str_copy, rows_match, Matrix::ROW_REG_EXP);
    row = rows_match[0];
    cols_ = 0, rows_ = 1;
    while (regex_search(row, columns_match, RationalNumber::REG_EXP))
    {
        cols_++;
        row = columns_match.suffix().str();
    }
    str_copy = rows_match.suffix().str();
    while (regex_search(str_copy, rows_match, Matrix::ROW_REG_EXP))
    {
        rows_++;
        str_copy = rows_match.suffix().str();
    }

    contents = new RationalNumber*[rows_];
    contents[0] = new RationalNumber[rows_ * cols_];
    for (int i = 1; i != rows_; i++)
        contents[i] = contents[i - 1] + cols_;

    int k = 0;
    while (regex_search(str_matrix, elements_match, RationalNumber::REG_EXP))
    {
        contents[k / cols_][k % cols_] = RationalNumber(elements_match[0].str());
        str_matrix = elements_match.suffix().str();
        k++;
    }
}

Matrix::Matrix(int rows, int cols) :
        GenericValue(MATRIX), contents(nullptr), rows_(rows), cols_(cols)
{
    contents = new RationalNumber*[rows_];
    contents[0] = new RationalNumber[rows_ * cols_];
    for (int i = 1; i != rows_; i++)
        contents[i] = contents[i - 1] + cols_;
}

Matrix::Matrix() :
        GenericValue(MATRIX), contents(nullptr), rows_(0), cols_(0)
{}

Matrix::Matrix(const Matrix& other) :
        GenericValue(MATRIX)
{
    rows_ = other.rows_;
    cols_ = other.cols_;
    contents = new RationalNumber*[rows_];
    contents[0] = new RationalNumber[rows_ * cols_];
    for (int i = 1; i != rows_; i++)
        contents[i] = contents[i - 1] + cols_;

    for (int i = 0; i != rows_; i++)
        for (int j = 0; j != cols_; j++)
            contents[i][j] = other.contents[i][j];
}

void Matrix::clear()
{
    if (contents != nullptr) { delete [] contents[0]; delete [] contents; }
    contents = nullptr;
    rows_ = 0;
    cols_ = 0;
}

Matrix::~Matrix()
{
    clear();
}

GenericValue* Matrix::clone() { return new Matrix(*this); }

Matrix& Matrix::operator=(const Matrix& other)
{
    if (this != &other)
    {
        clear();
        rows_ = other.rows_;
        cols_ = other.rows_;
        contents = new RationalNumber*[rows_];
        contents[0] = new RationalNumber[rows_ * cols_];
        for (int i = 1; i != rows_; i++)
            contents[i] = contents[i - 1] + cols_;

        for (int i = 0; i != rows_; i++)
            for (int j = 0; j != cols_; j++)
                contents[i][j] = other.contents[i][j];
    }
    return *this;
}

void Matrix::transpose()
{
    int new_rows = cols_, new_cols = rows_;

    auto new_contents = new RationalNumber*[new_rows];
    new_contents[0] = new RationalNumber[new_rows * new_cols];
    for (int i = 1; i != new_rows; i++)
        new_contents[i] = new_contents[i - 1] + new_cols;

    for (int i = 0; i != new_rows; i++)
        for (int j = 0; j != new_cols; j++)
            new_contents[i][j] = contents[j][i];

    clear();
    contents = new_contents;
    rows_ = new_rows;
    cols_ = new_cols;
}

Matrix Matrix::operator+(const Matrix& other) const
{
    Matrix result(rows_, cols_);
    for (int i = 0; i != rows_; i++)
        for (int j = 0; j != cols_; j++)
            result.contents[i][j] = contents[i][j] + other.contents[i][j];
    return result;
}

Matrix Matrix::operator-() const
{
    Matrix result(rows_, cols_);
    for (int i = 0; i != rows_; i++)
        for (int j = 0; j != cols_; j++)
            result.contents[i][j] = -contents[i][j];
    return result;
}

Matrix Matrix::operator-(const Matrix& other) const
{
    Matrix result(rows_, cols_);
    for (int i = 0; i != rows_; i++)
        for (int j = 0; j != cols_; j++)
            result.contents[i][j] = contents[i][j] - other.contents[i][j];
    return result;
}

Matrix Matrix::operator*(const Matrix& other) const
{
    Matrix result(rows_, other.cols_);
    for (int i = 0; i != rows_; i++)
        for (int j = 0; j != other.cols_; j++)
        {
            RationalNumber element(0, 1);
            for (int k = 0; k != cols_; k++)
                element = element + contents[i][k] * other.contents[k][j];
            result.contents[i][j] = element;
        }
    return result;
}

Matrix Matrix::operator*(const RationalNumber& multiplier) const
{
    Matrix result(rows_, cols_);
    for (int i = 0; i != result.rows_; i++)
        for (int j = 0; j != result.cols_; j++)
            result.contents[i][j] = multiplier * contents[i][j];
    return result;
}

Matrix operator*(const RationalNumber& multiplier, const Matrix& m)
{
    Matrix result(m.rows_, m.cols_);
    for (int i = 0; i != result.rows_; i++)
        for (int j = 0; j != result.cols_; j++)
            result.contents[i][j] = multiplier * m.contents[i][j];
    return result;
}

std::string Matrix::to_string() const
{

    if (contents == nullptr || rows_ == 0 || cols_ == 0)
        return "( Empty matrix )";
    else
    {
        string result = "(\n";
        for (int i = 0; i != rows_; i++)
        {
            result += "\t";
            for (int j = 0; j < cols_; j++)
                result += contents[i][j].to_string() + " ";
            result += "\n";
        }
        result += ")";
        return result;
    }
}
