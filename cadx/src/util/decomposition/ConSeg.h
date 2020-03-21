#ifndef _CONSEG_H_
#define _CONSEG_H_

#include <vector>
#include <algorithm>
#include <functional>
#include "Iem/Iem.h"
#include "contour_pt.h"
#define M_PI            3.14159265358979323846
#ifdef WIN32
// this is really annoying. It is required because MSVC++ apparently does not adhere
// to the C++ standard for variable scoping in a for statement.
// I tried disabling the Language Extensions, as per Microsoft Knowledge Base 
// article #Q167748, but this causes Microsoft's macros to break. This is the
// alternate solution proposed by the above article. 
// Not that I'm bitter, but since when is following the C++ standard a bad thing???
#define for if(0); else for 
#endif

class candidate {
    public:
        int x, y;
        double angle, vector;
        double distance;
        int index;
        candidate(int ind, double dist, int x1, int y1) {
            index=ind;
            distance=dist;
            x=x1;
            y=y1;
        }

        candidate(int ind, contour_pt &this_pt, contour_pt &ncp_pt) {
            index=ind;
            distance=(this_pt.get_x()-ncp_pt.get_x())*(this_pt.get_x()-ncp_pt.get_x()) +
                (this_pt.get_y()-ncp_pt.get_y())*(this_pt.get_y()-ncp_pt.get_y());
            x=this_pt.get_x();
            y=this_pt.get_y();
        }

        bool operator==(const int &_index) {
            return(_index==index);
        }
};

typedef vector< candidate > candidate_vector;
typedef vector< candidate_vector > candidate_matrix;


class ConSeg
{
    public:
        void process_region(short codeval);
        IemTPlane<short> contour_segmentation(IemTPlane<short> &inImage,
          char *debug_name=0);
        void filter_by_tangent_angle(contour_vector &sm_contour, vector<int> &ncps, candidate_matrix &in, candidate_matrix &out);
        void smooth_contour(contour_vector &in, contour_vector &out);

        // _width is the filter width. And, why isn't it an odd number?
        ConSeg(int _width=50, double _omega=0.1, double _crit_thresh=60, int _crit_method=1, bool _debug=false)
        {
            conv_width=_width;
            omega=_omega;
            crit_thresh=_crit_thresh;
            crit_method=_crit_method;
            debug=_debug;

            // angle threshold (first number is in degrees, rest converts to radians)
            // these are the margins on *either side* of the ideal angle, so for a setting
            // of 90, there is an acceptable range of 180 degrees.
            usual_ang_thresh = 90.0 * 2.0*M_PI/360.0;
            hi_curv_ang_thresh = 135.0 * 2.0*M_PI/360.0;
            hi_curve_thresh=0.2;
			min_contour_length=30;
        }

		~ConSeg()
        {
		}

        int  conv_width;
        bool debug;
        
    protected:
        IemTPlane<short> contourbuf;
        IemTPlane<short> cuts;
        IemTPlane<short> segseg;

          // used for debugging
        IemTImage<unsigned char> shapebuf;

        double   omega;
        double   crit_thresh;
        int      crit_method;
		unsigned min_contour_length;

        void select_by_euclidean_distance(contour_vector &cv, vector<int> &ncps, vector<int> &cps, candidate_matrix &nearby_pts);
        void filter_by_normal_angle(contour_vector &sm_contour, vector<int> &ncps, candidate_matrix &in, candidate_matrix &out);
        vector<int> perform_local_nonmax_suppression(contour_vector &sm_contour, vector<int> &ncps, vector<int> &cps);

        double dist_threshold, dist_threshold_sqrt, usual_ang_thresh, hi_curv_ang_thresh, hi_curve_thresh;
        double dist_threshold2;
};

int sign(double d);
#endif
