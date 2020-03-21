#ifndef CONTOUR_PT_H
#define CONTOUR_PT_H
#include <vector>
#include <algorithm>
#include <functional>
#include <math.h>

#ifdef WIN32
using namespace std;
#endif

// why doesn't the following line work in VC++??
typedef vector<class contour_pt> contour_vector;
// have to use this instead:
//#define contour_vector vector<class contour_pt>

class contour_pt {
    public:
        contour_pt(int _x, int _y) {
            set_xy(_x,_y);
        }
        contour_pt(double _angle, double _radius) {
            set_ar(_angle, _radius);
        }
        contour_pt(void) {
            cart_valid=false;
            polar_valid=false;
            deriv_valid=false;
            tangent_valid=false;
        }

        void set_xy(int _x, int _y) {
            x=_x, y=_y;
            cart_valid=true;
            polar_valid=false;
            deriv_valid=false;
            tangent_valid=false;
        }
        void set_ar(double _angle, double _radius) {
            angle=_angle, radius=_radius;
            cart_valid=false;
            polar_valid=true;
            deriv_valid=false;
            tangent_valid=false;
        }


        
        int get_x() {
            if(!cart_valid)
                update_cart();
            return x;
        }
        int get_y() {
            if(!cart_valid)
                update_cart();
            return y;
        }
        double get_angle() {
            if(!polar_valid)
                update_polar();
            return angle;
        }
        double get_radius() {
            if(!polar_valid)
                update_polar();
            return radius;
        }

        double get_tangent_angle(contour_vector &v, int index);

        void get_derivatives(contour_vector &vector, int index, 
                             pair<double,double> &_first_deriv, pair<double,double> &_second_deriv) {
            if(!deriv_valid) {
                compute_derivatives(vector,index);
                deriv_valid=true;
            }
            _first_deriv=first_deriv;
            _second_deriv=second_deriv;
        }

        double get_curvature(contour_vector &v, int index)
        {
            if(!deriv_valid) {
                compute_derivatives(v,index);
                deriv_valid=true;
            }

            return((first_deriv.first*second_deriv.second-second_deriv.first*first_deriv.second)/
                pow(first_deriv.first*first_deriv.first+first_deriv.second*first_deriv.second,3.0/2.0));

        }


        double contour_dist;
        int orig_index;

        bool operator==(contour_pt &other_pt) {
            return(get_x()==other_pt.get_x() && get_y()==other_pt.get_y());
                   
        }

        bool operator!=(contour_pt &other_pt) {
            return(!(operator==(other_pt)));
        }

    protected:
        int x, y;
        double angle, radius, tangent_angle;
        bool cart_valid, polar_valid, deriv_valid, tangent_valid;
        pair<double,double> first_deriv, second_deriv;

        void compute_derivatives(contour_vector &contour, int index);

        void update_polar() {
            radius=sqrt(x*x+y*y);
            if(radius==0)
                angle=0;
            else
                angle=asin(y/radius);
            polar_valid=true;
        }

        void update_cart() {
            x=int(radius*cos(angle));
            y=int(radius*sin(angle));
            cart_valid=true;
        }
};


#endif
