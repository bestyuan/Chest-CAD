#include "Rect.h"


// Rect : a simple Rectangle class
//
// crandall, 10/2001
//

  // function to output rectangle coordinates to a stream
std::ostream &operator<<(std::ostream &os, const Rect &rect)
{
    os << "[" << rect.top << " " << rect.left << " " << rect.height << " " << rect.width << "]";

    return os;
}
