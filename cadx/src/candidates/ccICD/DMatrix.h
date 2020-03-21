#include <iostream>

// DMatrix : Dave's straightforward matrix class
//
// This class provides basic matrix arithmetic. 
// You can access a given matrix element by using [][] operators,
// e.g. matrix[row][col].
//
// crandall, 5/2000
//
class DMatrix
{
    public:
        DMatrix(int _rows, int _cols);
        DMatrix(int _rows, int _cols, const double *array);
        DMatrix();
        DMatrix(const DMatrix &other);

        ~DMatrix();

          // matrix multiplication
        DMatrix operator*(const DMatrix &other) const;

          // matrix addition
        DMatrix operator+(const DMatrix &other) const;

          // matrix subtraction
        DMatrix operator-(const DMatrix &other) const;

          // matrix division
        DMatrix operator/(const DMatrix &other) const;

          // matrix assignment
        DMatrix &operator=(const DMatrix &other);

          // matrix + scalar 
        DMatrix operator+(double value) const;

          // matrix - scalar
        DMatrix operator-(double value) const;

          // matrix * scalar
        DMatrix operator*(double value) const;

          // return pointer to a given row of matrix
        double *operator[](int row) const;

          // return transpose of current matrix
        DMatrix transpose() const;

          // return # of rows or columns in matrix
        int rows() const { return _rows; }
        int cols() const { return _cols; }

          // auto-casts a singular (1x1) matrix to a scalar (double)
        operator double() const;

          // scalar + matrix
        friend DMatrix operator+(double value, const DMatrix &other);

          // scalar - matrix
        friend DMatrix operator-(double value, const DMatrix &other);

          // scalar * matrix
        friend DMatrix operator*(double value, const DMatrix &other);

          // i/o routines
        friend std::istream &operator>>(std::istream &is, DMatrix &matrix);
        friend std::ostream &operator<<(std::ostream &is, const DMatrix &matrix);

          // return true iff m1 and m2 have the same dimensions
        friend bool same_size(const DMatrix &m1, const DMatrix &m2);

    protected:
          // allocation, deallocation routines
        void deallocate_storage();
        void initialize_storage();

          // array of pointers, to beginning of each matrix row
        double **data;

          // pointer to data area, where matrix is stored
        double *data_area;

          // # rows and cols in matrix
        int _rows, _cols;
};



