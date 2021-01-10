#pragma once
#include <string>
#include <regex>

enum ValueType
{
    RATIONAL_NUMBER,
    MATRIX
};

// ==== Value types base class declaration ====

struct GenericValue
{
    explicit GenericValue(ValueType object_type);
    virtual ~GenericValue() = default;

    virtual GenericValue* clone();

    inline ValueType get_type() { return type; }

    virtual std::string to_string() const { return std::string(); };
private:
    ValueType type;
};

// ==== Rational number class declaration ====

struct RationalNumber : GenericValue
{
    static bool is_correct_str(std::string str_num);
    static const std::string REG_EXP_STR;
    static const std::regex REG_EXP;

    explicit RationalNumber(std::string str_num);
    RationalNumber(const RationalNumber& other);
    RationalNumber(int num, int den);
    RationalNumber();
    ~RationalNumber() override = default;

    GenericValue* clone() override;

    RationalNumber& operator=(const RationalNumber& other);

    RationalNumber operator+(const RationalNumber& other) const;
    RationalNumber operator-() const;
    RationalNumber operator-(const RationalNumber& other) const;
    RationalNumber operator*(const RationalNumber& other) const;
    RationalNumber operator/(const RationalNumber& other) const;

    inline int num() const { return numerator; }
    inline int den() const { return denominator; }

    std::string to_string() const override;
private:
    void simplify();
    int numerator;
    int denominator;
};

// ==== Matrix of rational numbers class declaration ====

struct Matrix : GenericValue
{
    static bool is_correct_str(std::string str_matrix);
    static const std::regex ROW_REG_EXP;
    static const std::regex REG_EXP;

    explicit Matrix(std::string str_matrix);
    Matrix(int rows, int cols);
    Matrix();
    Matrix(const Matrix& other);
    Matrix& operator=(const Matrix& other);
    ~Matrix() override;

    GenericValue* clone() override;

    void transpose();
    Matrix operator+(const Matrix& other) const;
    Matrix operator-() const;
    Matrix operator-(const Matrix& other) const;
    Matrix operator*(const Matrix& other) const;
    Matrix operator*(const RationalNumber& multiplier) const;
    friend Matrix operator*(const RationalNumber& multiplier, const Matrix& m);

    bool inline has_same_size(const Matrix& other) const { return (rows_ == other.rows_ && cols_ == other.cols_); }
    bool inline is_multipliable_with(const Matrix& other) const { return (cols_ == other.rows_); }

    std::string to_string() const override;
private:
    void clear();
    RationalNumber** contents;
    int rows_;
    int cols_;
};

// ==== Binary operations declaration ====

bool add(GenericValue* left, GenericValue* right, GenericValue** result);
bool subtract(GenericValue* left, GenericValue* right, GenericValue** result);
bool multiply(GenericValue* left, GenericValue* right, GenericValue** result);
bool divide(GenericValue* left, GenericValue* right, GenericValue** result);

// ==== Unary operations declarations ====

bool T(GenericValue* argument, GenericValue** result);
bool unary_minus(GenericValue* argument, GenericValue** result);