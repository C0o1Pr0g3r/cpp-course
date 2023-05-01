#ifndef MATRIX_H_INCLUDED
#define MATRIX_H_INCLUDED

#include <vector>
#include <algorithm>

#include "utilities.h"


template<class T = int, bool Placeholder = true>
class Matrix {
public:
    using value_type = T;
    using reference = value_type&;
    using const_reference = const value_type&;
    using size_type = std::size_t;
    using index_type = std::pair<size_type, size_type>;
    using storage_type = std::vector<std::vector<value_type>>;

    std::integral_constant<size_type, 2> rank;

    enum class Dimension {
        ROW,
        COLUMN
    };

    size_type size(Dimension dimension) const {
        switch (dimension) {
            case Dimension::ROW:
                return this->_elements.size();
            break;
            case Dimension::COLUMN:
                return this->_elements[0].size();
            break;
            default:
                throw std::invalid_argument("An invalid value was passed for parameter 'dimension'");
        } 
    }

    void resize(Dimension dimension, size_type value) {
        switch (dimension) {
            case Dimension::ROW:
                this->_elements.resize(value);
            break;
            case Dimension::COLUMN:
                for (auto& row: this->_elements) {
                    row.resize(value);
                }
            break;
            default:
                throw std::invalid_argument("An invalid value was passed for parameter 'dimension'");
        }
    }

    template<class Compare = std::less<const_reference>>
    void sort(Dimension dimension, size_type index, Compare comp = std::less<const_reference>()) {
        switch (dimension) {
            case Dimension::ROW:
                std::sort(this->_elements[index].begin(), this->_elements[index].end(), comp);
            break;
            case Dimension::COLUMN: {
                std::vector<value_type> tmp(this->_elements.size());

                for (size_type i = 0; i < this->_elements.size(); ++i) {
                    tmp[i] = this->_elements[i][index];
                }

                std::sort(tmp.begin(), tmp.end(), comp);

                for (size_type i = 0; i < this->_elements.size(); ++i) {
                    this->_elements[i][index] = tmp[i];
                }
            }
            break;
            default:
                throw std::invalid_argument("An invalid value was passed for parameter 'dimension'");
        }
    }
    
    reference operator[](index_type index) {
        return this->_elements[index.first][index.second];
    }

    const_reference operator[](index_type index) const {
        return this->_elements[index.first][index.second];
    }

    Matrix operator+(const Matrix& other) const {
        return _perform_mathematical_operation_with_matrix(other, sum<value_type>);
    }

    Matrix operator-(const Matrix& other) const {
        return _perform_mathematical_operation_with_matrix(other, difference<value_type>);
    }

    Matrix()
    :
        _elements()
    {}

    Matrix(std::initializer_list<std::vector<value_type>> init)
    :
        _elements(init)
    {}

    template<class U>
    friend void print_matrix(std::ostream& out, const Matrix<U>& matrix, std::string delimiter);

private:
    template<class U>
    Matrix _perform_mathematical_operation_with_matrix(const Matrix& other, value_type operation(const U& a, const U& b)) const {
        if (
            this->_elements.size() != other._elements.size()
            ||
            this->_elements[0].size() != other._elements[0].size()
        ) {
            throw std::length_error("Matrix sizes do not match");
        }

        Matrix result;
        result._elements.resize(this->_elements.size());
        
        for (size_type i = 0; i < this->_elements.size(); ++i) {
            result._elements[i].resize(this->_elements[i].size());
            for (size_type j = 0; j < this->_elements[0].size(); ++j) {
                result._elements[i][j] = operation(this->_elements[i][j], other._elements[i][j]);
            }
        }

        return result;
    }

    storage_type _elements;
};

template<class U>
void print_matrix(std::ostream& out, const Matrix<U>& matrix, std::string delimiter) {
    for (const auto& row: matrix._elements) {
        for (std::size_t i = 0; i < row.size(); ++i) {
            out << row[i];
            if (i < row.size() - 1) {
                out << delimiter;
            }
        }
        out << std::endl;
    }
}

template<class U, std::enable_if_t<!std::is_integral<U>::value, bool> = true>
std::ostream& operator<<(std::ostream& out, const Matrix<U>& matrix) {
    print_matrix(out, matrix, " | ");

    return out;
}

template<class U, std::enable_if_t<std::is_integral<U>::value, bool> = true>
std::ostream& operator<<(std::ostream& out, const Matrix<U>& matrix) {
    print_matrix(out, matrix, "; ");

    return out;
}

// Explicit specialization
template<>
Matrix<std::string> Matrix<std::string>::operator-(const Matrix<std::string>& other) const = delete;

#endif // MATRIX_H_INCLUDED
