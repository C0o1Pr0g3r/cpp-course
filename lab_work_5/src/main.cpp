#include <iostream>

#include "matrix.h"

int main(int, char**) {
    using ElementType = int;

    Matrix<ElementType> matrix1 {
        {31, 24, 43, 24, 68},
        {46, 32, 38, 44, 50},
        {39, 62, 33, 58, 42},
        {39, 62, 33, 25, 25},
        {39, 62, 33, 25, 25},
    };
    Matrix<ElementType> matrix2 {
        {15, 12, 19, 12, 22, 13},
        {18, 14, 20, 15, 35, 54},
        {17, 22, 30, 31, 12, 93},
    };

    std::cout << "First matrix:" << std::endl;
    std::cout << matrix1 << std::endl;
    std::cout << "Second matrix:" << std::endl;
    std::cout << matrix2 << std::endl;

    std::cout << "1. Trying to add matrices of different sizes" << std::endl;
    try {
        matrix1 + matrix2;
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
    std::cout << std::endl;

    std::cout
        << "2. Trying to get an arbitrary element, change it and get it again"
        << std::endl;
    std::cout << "Before change: " << matrix1[std::make_pair(2, 3)] << std::endl;
    matrix1[std::make_pair(2, 3)] = 67;
    std::cout << "After change: " << matrix1[std::make_pair(2, 3)] << std::endl;
    std::cout << std::endl;

    const std::size_t row_number_to_sort = 3;
    std::cout << "3. Trying to sort row number "
        << row_number_to_sort << std::endl;
    std::cout << "Matrix before sorting row number "
        << row_number_to_sort << std::endl;
    std::cout << matrix2 << std::endl;
    matrix2.sort(Matrix<ElementType>::Dimension::ROW, row_number_to_sort - 1);
    std::cout << "Matrix after sorting row number "
        << row_number_to_sort << std::endl;
    std::cout << matrix2 << std::endl;

    const std::size_t column_number_to_sort = 5;
    std::cout
        << "4. Trying to sort column " << column_number_to_sort
        << " with custom comparator" << std::endl;
    std::cout
        << "Matrix before sorting column number " << column_number_to_sort
        << std::endl;
    std::cout << matrix2 << std::endl;
    matrix2.sort(Matrix<ElementType>::Dimension::COLUMN, column_number_to_sort - 1,
        [](const ElementType& a, const ElementType& b) { return a > b; });
    std::cout << "Matrix after sorting column number " << column_number_to_sort << std::endl;
    std::cout << matrix2 << std::endl;

    std::cout << "5. Trying to subtract matrices of the same size" << std::endl;
    std::cout
        << "Size of matrix number 1 before resizing: "
        << matrix1.size(Matrix<ElementType>::Dimension::ROW)
        << "x"
        << matrix1.size(Matrix<ElementType>::Dimension::COLUMN)
        << std::endl;
    matrix1.resize(Matrix<ElementType>::Dimension::ROW, 3);
    matrix1.resize(Matrix<ElementType>::Dimension::COLUMN, 6);
    std::cout
        << "Size of matrix number 1 after resizing: "
        << matrix1.size(Matrix<ElementType>::Dimension::ROW)
        << "x"
        << matrix1.size(Matrix<ElementType>::Dimension::COLUMN)
        << std::endl;
    std::cout << std::endl;

    std::cout << "6. Trying to subtract matrices of the same size" << std::endl;
    std::cout << "First matrix:" << std::endl;
    std::cout << matrix1 << std::endl;
    std::cout << "-" << std::endl << std::endl;
    std::cout << "Second matrix:" << std::endl;
    std::cout << matrix2 << std::endl;
    std::cout << "=" << std::endl << std::endl;
    std::cout << matrix1 - matrix2 << std::endl;

    Matrix<std::string> string_matrix {
        {"Hello", "world", "!  "},
        {"How  ", "are  ", "you", "?"},
    };
    
    std::cout << "String matrix:" << std::endl;
    std::cout << string_matrix <<std::endl;

    std::cout
        << "7. Trying to get the rank of a matrix: " << string_matrix.rank
        << std::endl;
}
