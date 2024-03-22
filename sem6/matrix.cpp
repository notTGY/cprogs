#include <iostream>

class Matrix {
public:
    int rows;
    int columns;
    double* data;

    Matrix(int _rows = 0, int _columns = 0) : rows(_rows), columns(_columns) {
      uint64_t bigRow = rows;
      uint64_t bigColumn = columns;
      uint64_t bigSize = bigRow * bigColumn;
      uint64_t smallSize = 10000000000;
      if (rows <= 0 || columns <= 0 || bigSize >= smallSize) {
          std::cout << "Wrong size of matrix" << std::endl;
          exit(1);
      }
      data = new double[rows * columns];
      if (data == nullptr) {
          std::cout << "Wrong size of matrix" << std::endl;
          exit(1);
      }
    }

    Matrix(const Matrix& other) : rows(other.rows), columns(other.columns) {
      data = new double[rows * columns];
      if (data == nullptr) {
          std::cout << "Wrong size of matrix" << std::endl;
          exit(1);
      }
      std::copy(other.data, other.data + other.rows * other.columns, data);
    }

    ~Matrix() { delete[] data; }

    const double * operator[](int row) const { return data + row * columns; }

    friend std::ostream& operator<<(std::ostream& os, const Matrix& mat) {
        for (int i = 0; i < mat.rows; ++i) {
            for (int j = 0; j < mat.columns; ++j) {
                os << mat[i][j] << ' ';
            }
            os << '\n';
        }
        return os;
    }
};