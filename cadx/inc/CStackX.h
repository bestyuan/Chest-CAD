/*Permission to use, copy, modify, distribute and sell this software
* and its documentation for any purpose is hereby granted without fee,
* provided that the above copyright notice appear in all copies and
* that both that copyright notice and this permission notice appear
* in supporting documentation.
*It is provided "as is" without express or implied warranty.
* kings_oz@yahoo.com
*/

// StackX.h: interface for the CStackX class.
//
/////////////////////////////////////////////

#ifndef _TREX_
#define _TREX_


#include <stdlib.h>


#define ARRAY_MAX_SIZE 10000

template <class TRex>
class CStackX
{

public:
    CStackX();
    CStackX(long val);
    ~CStackX();
    
    void push(TRex);
    TRex pop();
    TRex peek(long i);
    void setMaxSize(long);
    long getSize();

private:
   // ArrayOutOfBounds *ex ;

    
    
    long arr_size;
    TRex *vect ;
    long top;


};

#if defined(__GNUC__) || defined(WIN32)
#include "CStackX.impl"
#endif

#endif


