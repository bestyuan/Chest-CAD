#include <Iem.h>

// This is a very simple character generator class, for putting
// text on IemImage planes.
//
// (only numbers [integers] are currently supported)
//
// crandall, 9/2002

class TextGen 
{
    public:
          // output an integer at a specified position
        void draw_number(IemPlane &img, int number, int digits, int row, int col, int value=255, 
                          bool erase_background = true) const;
        template<class T>
        void draw_number(IemTPlane<T> &img, int number, int digits, int row, int col, int value, bool erase_bkgrnd) const;

    protected:
        static const int digits[10][120];
        static const int chr_rows, chr_cols;
};



