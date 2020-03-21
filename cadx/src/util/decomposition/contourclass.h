#ifndef _COUNTOUR_CLASS_H_DEFINED_
#define _COUNTOUR_CLASS_H_DEFINED_ 1

#include "Iem/IemImageIO.h"
#include "Iem/Iem.h"

template<class T>
class contourclass
{
    public:
          //    dir: 0, 90, 180, 270
        
        contourclass(IemTPlane<T> &ccbufinit,
          IemTPlane<T> &ccbufinit_filled, T ccnuminit, int dirinit, 
          int startrow, int startcol)
        {
            ccbuf=ccbufinit;
            ccbuf_filled=ccbufinit_filled;
            ccnum=ccnuminit;
            width=ccbufinit.cols();
            height=ccbufinit.rows();
            dir=dirinit;
            row=startrow;
            col=startcol;
            left_count=right_count=0;
        }

        void TurnLeft()
        {
            dir = ((dir == 0) ? 270 : dir - 90);
        }

        void TurnRight()
        {
            dir = ((dir==270) ? 0 : dir + 90);
        }

        void GoStraight() 
        {
#if 0 // KRD
            if (dir==0)
            {
                row--;

                if (row >= 0)
                {
                    if (col>=3 && ccbuf_filled[row][col-3]==ccnum)
                        left_count++;

                    if (col<ccbuf_filled.cols()-3 &&
                      ccbuf_filled[row][col+3]==ccnum)
                        right_count++;
                }
            }

            if (dir==90)
            {
                col++;

                if (col < ccbuf_filled.cols())
                {
                    if (row>=3 && ccbuf_filled[row-3][col]==ccnum)
                        left_count++;

                    if (row<ccbuf_filled.rows()-3 &&
                      ccbuf_filled[row+3][col]==ccnum)
                        right_count++;
                }
            }

            if (dir==180)
            {
                row++;

                if (row < ccbuf_filled.rows())
                {
                    if (col<ccbuf_filled.cols()-3 &&
                      ccbuf_filled[row][col+3]==ccnum)
                        left_count++;
                    if (col>=3 && ccbuf_filled[row][col-3]==ccnum)
                        right_count++;
                }
            }

            if (dir==270)
            {
                col--;
                
                if (col >= 0)
                {
                    if (row<ccbuf_filled.rows()-3 &&
                      ccbuf_filled[row+3][col]==ccnum)
                        left_count++;
                    if (row>=3 && ccbuf_filled[row-3][col]==ccnum)
                        right_count++;
                }
            }
#else
            int nCols = (int)ccbuf_filled.cols();
            int nRows = (int)ccbuf_filled.rows();

            //
            // 6/25/2002 -- KRD adding more bounds checks after
            // we found an image where "row" got to 257 for case 0 below.
            // That's unfortunate, because row-- took it to 256, and then we
            // used it as an index into an image that's only 256 high. Yes,
            // we were out of bounds (indices range 0-255 in an image 256 high)
            //
            switch (dir)
            {
                case 0:
                {
                    row--;
                    if (row >= 0 && row < nRows)
                    {
                        //
                        // KRD
                        // We really shouldn't see col going to -3, nor
                        // should we see col - 3 >= nCols? Check anyway...
                        //
                        if (col >= 3 && col - 3 < nCols &&
                          ccbuf_filled[row][col - 3] == ccnum)
                            left_count++;

                        if (col + 3 < nCols && col >= -3 &&
                          ccbuf_filled[row][col + 3] == ccnum)
                            right_count++;
                    }
                }
                break;

                case 90:
                {
                    col++;

                    if (col < nCols && col >= 0)
                    {
                        if (row >= 3 && row - 3 < nRows &&
                          ccbuf_filled[row - 3][col] == ccnum)
                            left_count++;

                        if (row + 3 < nRows && row + 3 >= 0  &&
                          ccbuf_filled[row + 3][col] == ccnum)
                            right_count++;
                    }
                }
                break;

                case 180:
                {
                    row++;

                    if (row < nRows && row >= 0)
                    {
                        if (col + 3 < nCols && col + 3 >= 0 &&
                          ccbuf_filled[row][col + 3] == ccnum)
                            left_count++;

                        if (col >= 3 && col - 3  < nCols &&
                          ccbuf_filled[row][col - 3] == ccnum)
                            right_count++;
                    }
                }
                break;

                case 270:
                {
                    col--;
                    
                    if (col >= 0 && col < nCols)
                    {
                        if (row + 3 < nRows && row >= -3 &&
                          ccbuf_filled[row + 3][col] == ccnum)
                            left_count++;

                        if (row >= 3 && row - 3 < nRows &&
                          ccbuf_filled[row - 3][col] == ccnum)
                            right_count++;
                    }
                }
                break;

                default:
                    break;
            } // switch (dir)
#endif // KRD
        }

        char *Get4Block()
        {
            if(row < height && col < width)
                block4[3]=(ccbuf[row][col]==ccnum);
            else
                block4[3]=0;
            if(col > 0 && row < height)
                block4[2]=(ccbuf[row][col-1]==ccnum);
            else
                block4[2]=0;
            if(row > 0 && col < width)
                block4[1]=(ccbuf[row-1][col]==ccnum);
            else
                block4[1]=0;
            if(col > 0 && row > 0)
                block4[0]=(ccbuf[row-1][col-1]==ccnum);
            else
                block4[0]=0;
            
            if(dir==90)
            {
                int temp=block4[0];
                block4[0]=block4[1];
                block4[1]=block4[3];
                block4[3]=block4[2];
                block4[2]=temp;
            }
            else if(dir==270)
            {
                int temp=block4[0];
                block4[0]=block4[2];
                block4[2]=block4[3];
                block4[3]=block4[1];
                block4[1]=temp;
            }
            else if(dir==180)
            {
                int temp=block4[0];
                block4[0]=block4[3];
                block4[3]=temp;
                
                temp=block4[1];
                block4[1]=block4[2];
                block4[2]=temp;
            }
            return(block4);
        }
 
        int GetRow() { return row; }
        int GetCol() { return col; }
        int GetDir() { return dir; }

        int left_count, right_count;
        
    protected:
        char block4[4];
        int dir, height, width;
        
        T ccnum;
        IemTPlane<T> ccbuf, ccbuf_filled;
        char *borderbuf;
        int row,col;
};

#endif // _COUNTOUR_CLASS_H_DEFINED_ 1
