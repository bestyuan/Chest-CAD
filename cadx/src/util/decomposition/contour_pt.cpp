#include "contour_pt.h"
#include "ConSeg.h"
#include "math.h"


void contour_pt::compute_derivatives(contour_vector &contour, int index)
{
      // compute 1st and 2nd derivatives of x and y signals
    double x[5], y[5], x1[3], y1[3];
    int k=5; // sampling scale for 1st and 2nd derivative computations

    for(int i=0; i<5; i++) {
        int t=index-(2-i)*k;
        if(t<0)
            t+=(int)contour.size();
        else if(t>=(int)contour.size())
            t-=(int)contour.size();
        
        x[i]=contour[t].get_x();
        y[i]=contour[t].get_y();
    }

    x1[0]=(-x[0]+x[2])/2.0; y1[0]=(-y[0]+y[2])/2.0;
    x1[1]=(-x[1]+x[3])/2.0; y1[1]=(-y[1]+y[3])/2.0;
    x1[2]=(-x[2]+x[4])/2.0; y1[2]=(-y[2]+y[4])/2.0;
    
    second_deriv=pair<double,double>((x1[2]-x1[0])/2.0, (y1[2]-y1[0])/2.0);
    first_deriv=pair<double,double>(x1[1],y1[1]);
}

// computes the angle of the tangent of the curve at the specified point
//
// the tangent angle is guaranteed to be in the range [-pi/2,3pi/2]

double contour_pt::get_tangent_angle(contour_vector &contour, int index)
{
    if(!tangent_valid) {
          // compute derivatives at the point
        pair<double,double> first_deriv, second_deriv;

        get_derivatives(contour, index, first_deriv, second_deriv);

          // find slope of tangent line
        double m;
        if(first_deriv.first==0)
            m=10000000*-sign(first_deriv.second); // a Big Number
        else if(first_deriv.second==0)
            m=0;
        else
            m=-(first_deriv.second/first_deriv.first);
        
          // figure out the angle of the tangent
        tangent_angle=atan(m);
        if(first_deriv.first < 0)
            tangent_angle+=M_PI;

        tangent_valid=true;
    }

    return(tangent_angle);
}





