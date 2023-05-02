#include <iostream>

#include "matrix.h"


// Explicit instantiation
template
class Matrix<std::string>;

template<class T>
void test_addition_of_matrices_of_different_sizes(
    const Matrix<T>& a, const Matrix<T>& b
);

template<class T>
void test_access_to_matrix_elements(Matrix<T>& matrix);

template<class T>
void test_sorting_of_matrix_row(Matrix<T>& matrix);

template<class T>
void test_sorting_of_matrix_column(Matrix<T>& matrix);

template<class T>
void test_matrix_resizing(Matrix<T>& matrix);

template<class T>
void test_subtraction_of_matrices_of_the_same_size(
    const Matrix<T>& a, const Matrix<T>& b
);

template<class T>
void test_getting_matrix_rank(const Matrix<T>& matrix);


int main() {
    // Implicit instantiation
    Matrix<int> matrix1 {
        {31, 24, 43, 24, 68},
        {46, 32, 38, 44, 50},
        {39, 62, 33, 58, 42},
        {39, 62, 33, 25, 25},
        {39, 62, 33, 25, 25},
    };
    Matrix<int> matrix2 {
        {15, 12, 19, 12, 22, 13},
        {18, 14, 20, 15, 35, 54},
        {17, 22, 30, 31, 12, 93},
    };

    std::cout << "First matrix:" << std::endl;
    std::cout << matrix1 << std::endl;
    std::cout << "Second matrix:" << std::endl;
    std::cout << matrix2 << std::endl;
    
    test_addition_of_matrices_of_different_sizes(matrix1, matrix2);
    test_access_to_matrix_elements(matrix1);
    test_sorting_of_matrix_row(matrix2);
    test_sorting_of_matrix_column(matrix2);
    test_matrix_resizing(matrix1);
    test_subtraction_of_matrices_of_the_same_size(matrix1, matrix2);
    
    Matrix<std::string> string_matrix {
        {"Hello", "world", "!  "},
        {"How  ", "are  ", "you", "?"},
    };
    
    std::cout << "String matrix:" << std::endl;
    std::cout << string_matrix <<std::endl;

    test_getting_matrix_rank(string_matrix);

    return 0;
}


template<class T>
void test_addition_of_matrices_of_different_sizes(
    const Matrix<T>& a, const Matrix<T>& b
) {
    std::cout << "1. Trying to add matrices of different sizes"
        << " (deliberate attempt to catch an exception)"
        << std::endl;
    try {
        a + b;
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
    std::cout << std::endl;
}

template<class T>
void test_access_to_matrix_elements(Matrix<T>& matrix) {
    std::cout
        << "2. Trying to get an arbitrary element, change it and get it again"
        << std::endl;
    std::cout << "Before change: " << matrix[std::make_pair(2, 3)] << std::endl;
    matrix[std::make_pair(2, 3)] = 67;
    std::cout << "After change: " << matrix[std::make_pair(2, 3)] << std::endl;
    std::cout << std::endl;
}

template<class T>
void test_sorting_of_matrix_row(Matrix<T>& matrix) {
    const std::size_t row_number_to_sort = 3;
    std::cout << "3. Trying to sort row number "
        << row_number_to_sort << std::endl;
    std::cout << "Matrix before sorting row number "
        << row_number_to_sort << std::endl;
    std::cout << matrix << std::endl;
    matrix.sort(Matrix<T>::Dimension::ROW, row_number_to_sort - 1);
    std::cout << "Matrix after sorting row number "
        << row_number_to_sort << std::endl;
    std::cout << matrix << std::endl;
}

template<class T>
void test_sorting_of_matrix_column(Matrix<T>& matrix) {
    const std::size_t column_number_to_sort = 5;
    std::cout
        << "4. Trying to sort column " << column_number_to_sort
        << " with custom comparator (in descending order)" << std::endl;
    std::cout
        << "Matrix before sorting column number " << column_number_to_sort
        << std::endl;
    std::cout << matrix << std::endl;
    matrix.sort(Matrix<T>::Dimension::COLUMN, column_number_to_sort - 1,
        [](const T& a, const T& b) { return a > b; });
    std::cout << "Matrix after sorting column number "
        << column_number_to_sort << std::endl;
    std::cout << matrix << std::endl;
}

template<class T>
void test_matrix_resizing(Matrix<T>& matrix) {
std::cout << "5. Trying to resize a matrix" << std::endl;
    std::cout
        << "Size of matrix before resizing: "
        << matrix.size(Matrix<T>::Dimension::ROW)
        << "x"
        << matrix.size(Matrix<T>::Dimension::COLUMN)
        << std::endl;
    matrix.resize(Matrix<T>::Dimension::ROW, 3);
    matrix.resize(Matrix<T>::Dimension::COLUMN, 6);
    std::cout
        << "Size of matrix after resizing: "
        << matrix.size(Matrix<T>::Dimension::ROW)
        << "x"
        << matrix.size(Matrix<T>::Dimension::COLUMN)
        << std::endl;
    std::cout << std::endl;
}

template<class T>
void test_subtraction_of_matrices_of_the_same_size(
    const Matrix<T>& a, const Matrix<T>& b
) {
    std::cout << "6. Trying to subtract matrices of the same size" << std::endl;
    std::cout << "First matrix:" << std::endl;
    std::cout << a << std::endl;
    std::cout << "-" << std::endl << std::endl;
    std::cout << "Second matrix:" << std::endl;
    std::cout << b << std::endl;
    std::cout << "=" << std::endl << std::endl;
    std::cout << "Resulting matrix:" << std::endl;
    std::cout << a - b << std::endl;
}

template<class T>
void test_getting_matrix_rank(const Matrix<T>& matrix) {
    std::cout << "7. Trying to get the matrix rank" << std::endl;
    std::cout << "Matrix rank: " << matrix.rank << std::endl;
};
