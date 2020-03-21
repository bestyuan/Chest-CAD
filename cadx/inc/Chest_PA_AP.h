#ifndef __CHEST_PA_AP_H__
#define __CHEST_PA_AP_H__

#include "Iem/Iem.h"
#include "Iem/IemLinearArray.h"


namespace CADX_SEG {

class Chest_PA_AP  {

    public:
        Chest_PA_AP();
        void processing(IemPlane src);
        IemPlane getMarkedImage();
        void setVerbose(bool option) { _verbose = option; }
        void setSubsample(bool option) { _subsample = option; }
		void setBoostFactor(double option) { _boost_factor =option; }
        IemReorientOp getReorientOp() { return _ReorientationOp; }
        IemLinearArray getSpineLine() { return _spine_line; }
        IemLinearArray getLeftLungLine() { return _left_line; }
        IemLinearArray getRightLungLine() { return _right_line; }
        bool isAP() { return _AP; }
		double getSpineTopV() { return _spine_top_V; }
		double getSpineBottomV() { return _spine_bottom_V; }
		void setReorientation(bool option) { _reorient = option; }

    private:
        int _min_side;
        int _thH, _thL;
        double _fraction1, _fraction2, _fraction3, _fraction4;
        double _std_Factor, _to_edge;
		double _spine_top_V,_spine_bottom_V;
        bool _verbose;
		double _boost_factor;
        bool _subsample, _reorient;
        IemPlane _src,_Oimg;
        IemType _type;
        IemReorientOp _ReorientationOp;
        IemLinearArray _spine_line,_left_line,_right_line;
        bool _AP;
};



} // End namespace


#endif // __CHEST_PA_AP_H__
