//
//  (c) Copyright Eastman Kodak Company
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef REGIONGROWER_H
#define REGIONGROWER_H


#include "Iem/Iem.h" 
#include "Lut.h" 
#include "Util.h"


class RegionGrower
{
	protected:

	IemTImage<short> img;				// Color Image
	IemTImage<unsigned char> imgMask;		// Region mask
	
	// Zero code values in this image denode pixels to be excluded from the region.
	IemTImage<unsigned char> imgExclude;	// Exclusion mask

	short debug;
	long maxArea;				// Max area of region
	long stopArea;				// Area at which growth stopped.
	long MaxNeighbors;			// Size of SSL.
	short RegionValue;			// Value assigned to region pixels in mask
	short NeighborValue;		// Value assigned to neighbor pixels in mask
	double GrowRatio;			// Used for stop condition     
	
	short minAllowedRegionCodeValue;
	
	long colSeed;  
	long rowSeed;  
	
	short cvSeed;   

	double cvAvg, cvSum, cvSum2, cvSigma;	
	

	double aspectRatio, Fit;
	double distNorm;
	double c_sum, r_sum, cr_sum;
	double c_center, r_center;
	double c2_sum, r2_sum;
	double c2_center, r2_center, cr_center;
	double MomentRR, MomentCC, MomentRC;
	short Status;			        // Set to one if region growing failed


	long nNeighbors;			// Number of neighbors
	Lut<long> neighborsList; 	// Sorted list of neighbor positions and color differences

	long Area;					// Number of pixels in region
	Lut<long> memberList;		// List of region member positions  
	
	double threshold;


	public:

	RegionGrower();

	RegionGrower(IemTImage<short>& _img, IemTImage<unsigned char>& _imgExclude,
	 long _colSeed, long _rowSeed);

	long getArea() {return Area;}
	long getStopArea() {return stopArea;}
	double getColCenter() {return c_center;}
	double getRowCenter() {return r_center;}
	double getAspectRatio() {return aspectRatio;}
	short getStatus() {return Status;}
	double getFit() {return Fit;}
	double getMomentRR() {return MomentRR;}
	double getMomentCC() {return MomentCC;}
	double getMomentRC() {return MomentRC;}   
	double getAvg() {return cvAvg;}   
	double getSigma() {return cvSigma;}
	double getSeedCodeValue() {return cvSeed;}
	Lut<long>& getMemberList() {return memberList;}
	IemTImage<unsigned char> getMask() {return imgMask;}	 
	
	short hasGrowingFailed() {return Status;}
	
	double getDeff() {return 2.0 * sqrt((double)Area / 3.14159265358979);}
	
	double getRegionQuality();
	
	void setMinAllowedRegionCodeValue(short v) {minAllowedRegionCodeValue = v;}


	void initialize();

	void initializeStats();

	void addPixelToRegion(long c, long r, double score);

	void start();

	void addToNeighborList(long c, long r);

   	double calculateScore(long c, long r);

   	void updateRegionStats(short v);

  	void updateRegionShape(long c, long r);

	// Calculates the region's fit to an ellipse for the first A pixels.
	// in the region. If A = 0 then Area pixels are used.
   	double calculateFit(long A = 0);

	void shiftNeighborsListDown();      
	
	IemTImage<unsigned char> getRegionView();

   	void Iterate();

// 	Sets the mask to show the first A pixels in the region.
// 	If A = 0 then Area pixels are shown.
	void setMask(long A = 0);

	// Returns the optimal area for the defect;
   	long Find_Best_Area();

	void write(ostream& out);       
   


};

#endif // RegionGrower

