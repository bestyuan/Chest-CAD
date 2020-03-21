#include "DMatrix.h"
#include <string>

// DMatrix : Dave's straightforward matrix class
//
// crandall, 5/2000
//

using namespace std;

// cleans up memory areas
//
void DMatrix::deallocate_storage()
{
    delete[] data;
    delete[] data_area;
}

// initializes memory areas
//
void DMatrix::initialize_storage()
{
    if(data)
        deallocate_storage();

    data = new double *[_rows];
    data_area = new double[_rows * _cols];

    for(int i=0; i<_rows; i++)
        data[i] = &(data_area[i*_cols]);
}

DMatrix::DMatrix()
{
    data = 0;
    data_area = 0;
    _rows = _cols = 0;
}


DMatrix::DMatrix(int __rows, int __cols)
{
    _rows = __rows;
    _cols = __cols;

    data = 0;
    data_area = 0;

    initialize_storage();
}

DMatrix::DMatrix(int __rows, int __cols, const double *array)
{
    _rows = __rows;
    _cols = __cols;

    data = 0;
    data_area = 0;

    initialize_storage();

    memcpy(data_area, array, _rows * _cols * sizeof(double));
}


bool same_size(const DMatrix &m1, const DMatrix &m2)
{
    return(m1.rows() == m2.rows() && m1.cols() == m2.cols());
}

DMatrix DMatrix::operator+(const DMatrix &other) const
{
    DMatrix result(_rows, _cols);

    if(!same_size(*this, other))
        throw string("Size mismatch in DMatrix operator +");

    double *cp1 = data_area, *cp2 = other.data_area, *cp_out = result.data_area;
    for(int i=0; i<_rows; i++)
        for(int j=0; j<_cols; j++, cp1++, cp2++, cp_out++)
            *cp_out = *cp1 + * cp2;

    return result;
}

DMatrix DMatrix::operator+(double value) const
{
    DMatrix result(_rows, _cols);

    double *cp1 = data_area, *cp_out = result.data_area;
    for(int i=0; i<_rows; i++)
        for(int j=0; j<_cols; j++, cp1++, cp_out++)
            *cp_out = *cp1 + value;

    return result;
}


DMatrix DMatrix::operator-(const DMatrix &other) const
{
    DMatrix result(_rows, _cols);

    if(!same_size(*this, other))
        throw string("Size mismatch in DMatrix operator -");

    double *cp1 = data_area, *cp2 = other.data_area, *cp_out = result.data_area;
    for(int i=0; i<_rows; i++)
        for(int j=0; j<_cols; j++, cp1++, cp2++, cp_out++)
            *cp_out = *cp1 - * cp2;

    return result;
}

DMatrix DMatrix::operator-(double value) const
{
    DMatrix result(_rows, _cols);

    double *cp1 = data_area, *cp_out = result.data_area;
    for(int i=0; i<_rows; i++)
        for(int j=0; j<_cols; j++, cp1++, cp_out++)
            *cp_out = *cp1 - value;

    return result;
}

DMatrix DMatrix::operator*(const DMatrix &other) const
{
    if(_cols != other._rows)
        throw string("Size mismatch in DMatrix operator -");

    DMatrix result(_rows, other._cols);

    for(int i=0; i<_rows; i++)
        for(int j=0; j<other._cols; j++)
        {
            double res=0;

            for(int k=0; k<other._rows; k++)
                res += data[i][k] * other.data[k][j];

            result.data[i][j] = res;
        }

    return result;
}

DMatrix DMatrix::operator*(double value) const
{
    DMatrix result(_rows, _cols);

    double *cp1 = data_area, *cp_out = result.data_area;
    for(int i=0; i<_rows; i++)
        for(int j=0; j<_cols; j++, cp1++, cp_out++)
            *cp_out = *cp1 * value;

    return result;
}

DMatrix operator*(double value, const DMatrix &other)
{
    return(other * value);
}

double *DMatrix::operator[](int row) const
{
    return data[row];
}

DMatrix::~DMatrix()
{
    deallocate_storage();
}

DMatrix &DMatrix::operator=(const DMatrix &other)
{
    _rows = other.rows();
    _cols = other.cols();

    data = 0;
    data_area = 0;

    initialize_storage();

    memcpy(data_area, other.data_area, _rows * _cols * sizeof(double));

    return *this;
}

DMatrix::DMatrix(const DMatrix &other)
{
    *this = other;
}

DMatrix DMatrix::transpose() const
{
    DMatrix result(_cols, _rows);

    for(int i=0; i<_rows; i++)
        for(int j=0; j<_cols; j++)
            result.data[j][i] = data[i][j];

    return result;
}

DMatrix::operator double() const
{
    if(_rows != 1 || _cols != 1)
        throw std::string("Conversion to scalar attempted on matrix with dimensions larger than 1x1Z");

    return(data[0][0]);
}


DMatrix operator+(double value, const DMatrix &other)
{
    return(other + value);
}

DMatrix operator-(double value, const DMatrix &other)
{
    DMatrix result(other.rows(), other.cols());

    double *cp1 = other.data_area, *cp_out = result.data_area;
    for(int i=0; i<other.rows(); i++)
        for(int j=0; j<other.cols(); j++, cp1++, cp_out++)
            *cp_out = -*cp1 + value;

    return result;
}

istream &operator>>(istream &is, DMatrix &matrix)
{
    for(int i=0; i<matrix.rows(); i++)
        for(int j=0; j<matrix.cols(); j++)
            is >> matrix.data[i][j];

    return is;
}

ostream &operator<<(ostream &os, const DMatrix &matrix)
{
    for(int i=0; i<matrix.rows(); i++)
    {
        for(int j=0; j<matrix.cols(); j++)
            os << matrix.data[i][j] << " ";
        os << endl;
    }

    return os;
}


