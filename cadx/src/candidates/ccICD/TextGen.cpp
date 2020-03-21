#include "TextGen.h"


// This is a very simple character generator class, for putting
// text on IemImage planes.
//
// (only numbers [integers] are currently supported)
//
// crandall, 9/2002
//

// Bitmaps of each digit
//
const int TextGen::digits[10][120] = {
    {0, 0, 0, 1, 1, 0, 0, 0,
     0, 0, 1, 1, 1, 1, 0, 0,
     0, 1, 0, 0, 0, 0, 1, 0,
     0, 1, 0, 0, 0, 0, 1, 0,
     0, 1, 0, 0, 0, 0, 1, 0,
     0, 1, 0, 0, 0, 0, 1, 0,
     0, 1, 0, 0, 0, 0, 1, 0,
     0, 1, 0, 0, 0, 0, 1, 0,
     0, 1, 0, 0, 0, 0, 1, 0,
     0, 1, 0, 0, 0, 0, 1, 0,
     0, 1, 0, 0, 0, 0, 1, 0,
     0, 0, 1, 1, 1, 1, 0, 0,
     0, 0, 0, 1, 1, 0, 0, 0},

    {0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 1, 0, 0, 0,
     0, 0, 0, 0, 1, 0, 0, 0,
     0, 0, 0, 0, 1, 0, 0, 0,
     0, 0, 0, 0, 1, 0, 0, 0,
     0, 0, 0, 0, 1, 0, 0, 0,
     0, 0, 0, 0, 1, 0, 0, 0,
     0, 0, 0, 0, 1, 0, 0, 0,
     0, 0, 0, 0, 1, 0, 0, 0,
     0, 0, 0, 0, 1, 0, 0, 0,
     0, 0, 0, 0, 1, 0, 0, 0,
     0, 0, 0, 0, 1, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0},

    {0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 1, 1, 1, 0, 0, 0,
     0, 1, 0, 0, 0, 1, 0, 0,
     0, 0, 0, 0, 0, 1, 0, 0,
     0, 0, 0, 0, 0, 1, 0, 0,
     0, 0, 0, 0, 0, 1, 0, 0,
     0, 0, 0, 0, 1, 0, 0, 0,
     0, 0, 0, 1, 0, 0, 0, 0,
     0, 0, 1, 0, 0, 0, 0, 0,
     0, 1, 0, 0, 0, 0, 0, 0,
     0, 1, 0, 0, 0, 0, 0, 0,
     0, 1, 1, 1, 1, 1, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0},

    {0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 1, 1, 1, 0, 0, 0,
     0, 0, 0, 0, 0, 1, 0, 0,
     0, 0, 0, 0, 0, 1, 0, 0,
     0, 0, 0, 0, 0, 1, 0, 0,
     0, 0, 0, 0, 1, 0, 0, 0,
     0, 0, 0, 1, 0, 0, 0, 0,
     0, 0, 0, 0, 1, 0, 0, 0,
     0, 0, 0, 0, 0, 1, 0, 0,
     0, 0, 0, 0, 0, 1, 0, 0,
     0, 1, 0, 0, 0, 1, 0, 0,
     0, 0, 1, 1, 1, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0},

    {0, 0, 0, 0, 0, 0, 0, 0,
     0, 1, 0, 0, 0, 1, 0, 0,
     0, 1, 0, 0, 0, 1, 0, 0,
     0, 1, 0, 0, 0, 1, 0, 0,
     0, 1, 0, 0, 0, 1, 0, 0,
     0, 1, 0, 0, 0, 1, 0, 0,
     0, 1, 1, 1, 1, 1, 0, 0,
     0, 0, 0, 0, 0, 1, 0, 0,
     0, 0, 0, 0, 0, 1, 0, 0,
     0, 0, 0, 0, 0, 1, 0, 0,
     0, 0, 0, 0, 0, 1, 0, 0,
     0, 0, 0, 0, 0, 1, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0},

    {0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 1, 1, 1, 1, 1, 0,
     0, 0, 1, 0, 0, 0, 0, 0,
     0, 0, 1, 0, 0, 0, 0, 0,
     0, 0, 1, 0, 0, 0, 0, 0,
     0, 0, 1, 1, 1, 1, 1, 0,
     0, 0, 0, 0, 0, 0, 1, 0,
     0, 0, 0, 0, 0, 0, 1, 0,
     0, 0, 0, 0, 0, 0, 1, 0,
     0, 0, 0, 0, 0, 0, 1, 0,
     0, 0, 0, 0, 0, 0, 1, 0,
     0, 0, 1, 1, 1, 1, 1, 0,
     0, 0, 0, 0, 0, 0, 0, 0},

    {0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 1, 1, 1, 1, 0, 0,
     0, 0, 1, 0, 0, 0, 0, 0,
     0, 0, 1, 0, 0, 0, 0, 0,
     0, 0, 1, 0, 0, 0, 0, 0,
     0, 0, 1, 0, 0, 0, 0, 0,
     0, 0, 1, 1, 1, 1, 1, 0,
     0, 0, 1, 0, 0, 0, 1, 0,
     0, 0, 1, 0, 0, 0, 1, 0,
     0, 0, 1, 0, 0, 0, 1, 0,
     0, 0, 1, 0, 0, 0, 1, 0,
     0, 0, 1, 1, 1, 1, 1, 0,
     0, 0, 0, 0, 0, 0, 0, 0},

    {0, 0, 0, 0, 0, 0, 0, 0,
     0, 1, 1, 1, 1, 1, 0, 0,
     0, 0, 0, 0, 0, 1, 0, 0,
     0, 0, 0, 0, 0, 1, 0, 0,
     0, 0, 0, 0, 0, 1, 0, 0,
     0, 0, 0, 0, 0, 1, 0, 0,
     0, 0, 0, 0, 0, 1, 0, 0,
     0, 0, 0, 0, 0, 1, 0, 0,
     0, 0, 0, 0, 0, 1, 0, 0,
     0, 0, 0, 0, 0, 1, 0, 0,
     0, 0, 0, 0, 0, 1, 0, 0,
     0, 0, 0, 0, 0, 1, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0},

    {0, 0, 0, 0, 0, 0, 0, 0,
     0, 1, 1, 1, 1, 1, 1, 0,
     0, 1, 0, 0, 0, 0, 1, 0,
     0, 1, 0, 0, 0, 0, 1, 0,
     0, 1, 0, 0, 0, 0, 1, 0,
     0, 1, 0, 0, 0, 0, 1, 0,
     0, 1, 1, 1, 1, 1, 1, 0,
     0, 1, 0, 0, 0, 0, 1, 0,
     0, 1, 0, 0, 0, 0, 1, 0,
     0, 1, 0, 0, 0, 0, 1, 0,
     0, 1, 0, 0, 0, 0, 1, 0,
     0, 1, 1, 1, 1, 1, 1, 0,
     0, 0, 0, 0, 0, 0, 0, 0},

    {0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 1, 1, 1, 1, 1, 0,
     0, 1, 0, 0, 0, 0, 1, 0,
     0, 1, 0, 0, 0, 0, 1, 0,
     0, 1, 0, 0, 0, 0, 1, 0,
     0, 1, 0, 0, 0, 0, 1, 0,
     0, 1, 1, 1, 1, 1, 1, 0,
     0, 0, 0, 0, 0, 0, 1, 0,
     0, 0, 0, 0, 0, 0, 1, 0,
     0, 0, 0, 0, 0, 0, 1, 0,
     0, 0, 0, 0, 0, 0, 1, 0,
     0, 0, 0, 0, 0, 0, 1, 0,
     0, 0, 0, 0, 0, 0, 0, 0}};

const int TextGen::chr_rows=13;
const int TextGen::chr_cols=8;

// output an integer at a specified position
//
// img      : image upon which to write numbers
// row, col : starting (upper-left) coordinate where text should be displayed
// digit_count   : # of digits to display
// value    : integer to display
// erase_bkgrnd : true if background should be erased before writing text, 
//                false if not
//
template<class T>
void TextGen::draw_number(IemTPlane<T> &img, int number, int digit_count, int row, int col, int value, bool erase_bkgrnd) const
{
      // loop through each digit of the number
    for(int d=digit_count; d>0; d--)
    {
          // pick off the highest-order remaining digit
        int num = (number % int(pow(10,d))) / int(pow(10,d-1));

          // bitblt it to the img
        for(int i=0; i<chr_rows; i++)
            for(int j=0; j<chr_cols; j++)
                if(row+i >= 0 && col+j >=0 && row+i < img.rows() && col+j < img.cols())
                    if(!erase_bkgrnd)
                        img[row+i][col+j] = int(img[row+i][col+j])/2 | int(digits[num][i*chr_cols + j] * value);
                    else
                        img[row+i][col+j] = int(digits[num][i*chr_cols + j] * value);

        col += chr_cols;
    }

    return;
}

void TextGen::draw_number(IemPlane &plane, int number, int digit_count, int row, int col, int value, bool erase_bkgrnd) const
{
    PLANE_DISPATCH(plane, planeT, draw_number(planeT, number, digit_count, row, col, value, erase_bkgrnd));
}
