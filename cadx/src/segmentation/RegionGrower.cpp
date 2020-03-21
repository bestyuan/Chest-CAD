//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//
//


#include "RegionGrower.h"

#include "Util.h"   
#include "Vector.h" 
#include "CadxParm.h" 
#include "Histogram.h"  
#include "Iem/IemImageIO.h"
#include "Iem/maximumFxns.h"


using namespace CADX_SEG;


RegionGrower::RegionGrower() {
	initialize();
}   

RegionGrower::RegionGrower(IemTImage<short>& _img, IemTImage<unsigned char>& _imgExclude,
 long _colSeed, long _rowSeed) {
	
	initialize();

	colSeed = _colSeed;  
	rowSeed = _rowSeed;
	
	img = _img.copy();        
	maxArea = 0.1 * img.rows() * img.cols();

	distNorm = sqrt(Util::square(img.cols()) + Util::square(img.rows()));
		                 	
	cvSeed = img[0][rowSeed][colSeed];

	imgMask = IemTImage<unsigned char>(1, img.rows(), img.cols());
	imgMask = 0;
	
	imgExclude = _imgExclude;

	neighborsList.Create(1, MaxNeighbors, 3);             
	neighborsList.Set0(); 

	memberList.Create(1, maxArea, 3);             
	memberList.Set0();       
	
	/*
	
	Histogram histogram(img, CadxParm::getInstance().getMinCodeValue(),
	 CadxParm::getInstance().getMaxCodeValue());  
	 	 
	threshold = histogram.getUpperPenetration(0.20);   
	*/
	         
	threshold = CadxParm::getInstance().getRegionGrowThreshold();

//	CadxParm::getInstance().getLogFile() << "\n\n RegionGrower.threshold "
//	 << threshold << ", seed " << cvSeed << endl;
	
}

void RegionGrower::initialize() {   
	
	// short data    
	debug = 0;
 	Status = 0;

	// long data
	nNeighbors = Area = 0;

	maxArea = 0;
 	stopArea = 0;
 
	cvSeed = cvSum = cvSum2 = cvSigma = 0.0;
   
	aspectRatio = Fit = c_sum = r_sum = cr_sum =
	c_center = r_center = c2_sum = r2_sum =
	c2_center = r2_center = cr_center =
	MomentRR = MomentCC = MomentRC = 0.0;

	initializeStats();
 
	RegionValue = 255;
	NeighborValue = 128;
	MaxNeighbors = 5000;  
	
	threshold = 0;
	distNorm = 1.0;
	
	minAllowedRegionCodeValue = 1;
}


void RegionGrower::initializeStats() {    
	
	cvAvg = cvSum = cvSum2 = cvSigma = 0;	
}


void RegionGrower::addPixelToRegion(long c, long r, double score)
{
	Area++;

	memberList.Val(Area, 1) = c;
	memberList.Val(Area, 2) = r;
	memberList.Val(Area, 3) = 1000.0 * score;

	imgMask[0][r][c] = RegionValue; 

	short cv = img[0][r][c];
                      
	updateRegionStats(cv);
	updateRegionShape(c, r);

	for(long n = -1; n <= 1; n++) {
		for(long m = -1; m <= 1; m++) {
			if(n || m) {  
       	
				long cc = c + m; 
				long rr = r + n;

				if(cc < 0 || cc >= img.cols()) continue;
				if(rr < 0 || rr >= img.rows()) continue;

				if(imgMask[0][rr][cc] == 0) {
					addToNeighborList(cc, rr);
 					imgMask[0][rr][cc] = (unsigned char)NeighborValue; 
				}
	}}}
}


void RegionGrower::addToNeighborList(long c, long r) {
                                         	
	double score = calculateScore(c, r);
	
	// If code value in exclude image is zero pixel is not added to list.
	if(imgExclude[0][r][c] == 0) return;

	// If score is zero do not add the pixel to the neighbor list
	if(score <= 0.0) return;

	// Size of neighborsList exceeded.
	if(nNeighbors >= MaxNeighbors) return;

	// Add neighbor to end of list  
	JEntry<long> entry(3);
	entry.Val(1) = c;
	entry.Val(2) = r;
	entry.Val(3) = 1000.0 * score; 

	neighborsList.mergeDescending(3, 1, nNeighbors, entry);

	nNeighbors++;
}


double RegionGrower::calculateScore(long c, long r) { 
	
	double score = 1.0;

	short cv = img[0][r][c];
	
	if(cv < minAllowedRegionCodeValue) {return 0.0;}
	
	return (double)cv / (double)cvSeed;
	
//	if(cv > cvSeed) score = 1.0;
//   	else score = 1.0 - Util::abs(cv - cvSeed) / CadxParm::getInstance().getMaxCodeValue();   
//   	if(score < 0.9) score = 0; 
   	 

//   	if(cv >= threshold) score = (cv - threshold) / (CadxParm::getInstance().getMaxCodeValue() - threshold); 
//   	else score = 0.0;   

	double delta = Util::abs((double)(cv - cvSeed) / (double)cvSeed);
	
	
	score *= 1.0 - Util::abs(cv - cvSeed) / (double)cvSeed;
	
	double a = sqrt(Util::square(c - colSeed) + Util::square(r - rowSeed));	
	
	score *= 1.0 - a / distNorm;
	
   if(score < threshold) score = 0.0;
   	 

	return score;   
}


void RegionGrower::updateRegionStats(short cv) { 

	cvSum += cv;  
 
	cvAvg = cvSum / (double)Area;  
 
	cvSum2 += cv * cv;  

	cvSigma = sqrt(cvSum2 / (double)Area - Util::square(cvAvg));      
}



void RegionGrower::updateRegionShape(long c, long r) {   
	
   c_sum += (double)c;
   r_sum += (double)r;

   c_center = c_sum / (double)Area;
   r_center = r_sum / (double)Area;

   c2_sum += c * c;
   r2_sum += r * r;
   cr_sum += c * r;

   c2_center = c2_sum / (double)Area;
   r2_center = r2_sum / (double)Area;
   cr_center = cr_sum / (double)Area;

   MomentRR = r2_center - r_center * r_center;
   MomentCC = c2_center - c_center * c_center;
   MomentRC = cr_center - c_center * r_center;
}


void RegionGrower::start() {  
	
	long k;     
/*
	if(cvSeed < threshold) {
		
	 	CadxParm::getInstance().getLogFile() << "\n\n RegionGrower.cvSeed " << cvSeed 
	 	 << " is smaller than threshold." << endl;
	  
		return; 
	}
*/

	if(cvSeed < minAllowedRegionCodeValue) {Status = 1; return;}
   
	addPixelToRegion(colSeed, rowSeed, 1);


	if(0) {
		cout << "\n\n neighborsList";
		for(k = 1; k <= nNeighbors; k++)
		cout << "\n" << setw(9) << k << setw(9) << neighborsList.Val(k, 1) 
			<< setw(9) << neighborsList.Val(k, 2) << setw(9) << neighborsList.Val(k, 3);
	}

	while(nNeighbors > 0) {

		if(Area == maxArea) {
			if(debug) cout << "\n\n Redeye maxArea exceeded" << endl;
			break;
		}

		Iterate();

		if(0) {

			cout << "\n\n memberList";
			for(k = 1; k <= Area; k++)
			cout << "\n" << setw(9) << k << setw(9) << memberList.Val(k, 1) 
					<< setw(9) << memberList.Val(k, 2) << setw(9) << memberList.Val(k, 3);

			cout << "\n\n neighborsList";
			for(k = 1; k <= nNeighbors; k++)
				cout << "\n" << setw(9) << k << setw(9) << neighborsList.Val(k, 1) 
					<< setw(9) << neighborsList.Val(k, 2) << setw(9) << neighborsList.Val(k, 3); 
		}

	//	if(debug) cout << "\n* " << *this;
	}

	stopArea = Area;

	calculateFit();
	setMask();

//	if(1) cout << "\n\n* " << *this << endl;    
	     
}


void RegionGrower::Iterate() {

   long c = neighborsList.Val(1, 1);
   long r = neighborsList.Val(1, 2);    
   double score = (double)neighborsList.Val(1, 3) / 1000.0;

   if(debug)
     cout << "\n Pixel " << Area  << " added at " << setw(9) << c
      << setw(9) << r << ", score " << setw(9) << score;

   shiftNeighborsListDown();

   addPixelToRegion(c, r, score); 
}


void RegionGrower::shiftNeighborsListDown() {
   neighborsList.Move_Block(2, 1, --nNeighbors);
}


double RegionGrower::calculateFit(long A) {  
	
   long inCount = 0, outCount = 0, k;
   double d, e, f, r = 0.0, ellipseArea, temp, u, x0, x1, inPixels, C1, C2;

   if(A == 0) A = Area;

   if(A == 0){Fit = 0.0; aspectRatio = 0.0; return 0.0;}

   temp = sqrt(Util::square(MomentCC - MomentRR) + 4.0 * Util::square(MomentRC));

   C1 = MomentCC + MomentRR + temp;
   C2 = MomentCC + MomentRR - temp;

   if(C2 == 0) aspectRatio = 0.0;
   else aspectRatio = sqrt(C1 / C2);

   u = MomentRR * MomentCC - Util::square(MomentRC);
   temp = 4.0 * u;

   if(temp == 0.0){Fit = 0.0; return 0.0;}

   d = MomentCC / temp;
   e = -MomentRC / temp;
   f = MomentRR / temp;

   ellipseArea = 4.0 * Util::PI * sqrt(u);
   
   for(k = 1; k <= A; k++)
   {
     x1 = (double)memberList.Val(k, 1) - c_center;  
     x0 = (double)memberList.Val(k, 2) - r_center;

     r = d * Util::square(x0) + 2.0 * e * x0 * x1 + f * Util::square(x1);

     if(r <= 1.0) inCount++; 
     else outCount++; 
   }      

   inPixels = Util::min(inCount, ellipseArea);
     
   if(ellipseArea) 
    Fit = 0.5 * (2.0 - (double)outCount / (double)Area - 
     (ellipseArea - inPixels) / ellipseArea);

   else Fit = 0.0;

   return r;
}


double RegionGrower::getRegionQuality() {
	
	
	return Fit * ((double)cvAvg / (double)CadxParm::getInstance().getMaxCodeValue());		
}


void RegionGrower::setMask(long A) {
 
	if(A == 0) A = Area;

	imgMask = 0; 

	for(long k = 1; k <= A; k++) { 
   	
		long c = memberList.Val(k, 1);  
		long r = memberList.Val(k, 2);

		imgMask[0][r][c] = RegionValue;
	}
}     


IemTImage<unsigned char> RegionGrower::getRegionView() {
 
  	IemTImage<unsigned char> img_display(3, img.rows(), img.cols()); 
  	
  	double max = iemMaximum(img);
  	
  	double scale = (255.0 / max);
  	
  	for(long r = 0; r < img.rows(); r++) {		                                        
		for(long c = 0; c < img.cols(); c++) {  
			
			short v =  img[0][r][c] * scale; 
			
			if(v < 0) v = 0;
			
			img_display[0][r][c] = v;
			img_display[1][r][c] = v;	
			img_display[2][r][c] = v;		
		}
	}	

	for(long k = 1; k <= Area; k++) { 
   	
		long c = memberList.Val(k, 1);  
		long r = memberList.Val(k, 2);

		short cv = img_display[0][r][c];

		cv += 255.0 / 5.0;  
		
		img_display[0][r][c] = Util::min(cv, 255);
	}     

	return img_display;
}

/*
long RegionGrower::Find_Best_Area() {
   long c, r;
   double R, G, B, Y, H, Sat;
   char FileName[1024];
   ofstream Out;

   if(debug) {
   //  sprintf(FileName, "%s/%s.dat", "", pI->Get_Name());
    // Out.open(FileName);
    // Out << "\n\n\n RegionGrower::Find_Best_Area()" << flush;
   }

   Initialize_Sums();

   Vector<double> Ratio(1, stopArea);
   Vector<double> Radius(1, stopArea);
   
   for(Area = 1; Area <= stopArea; Area++)
   {
     c = (short)memberList.Val(Area, 1);  // krd changed cast from (double) to (short)
     r = (short)memberList.Val(Area, 2);

     Ratio.Val(Area) = Calculate_Color_Distance_Ratio(c, r);

	updateRegionStats(p, l);

     updateRegionShape(c, r);

     Radius.Val(Area) = calculateFit(Area);

     R = img[1][r][c]; 

     if(debug)
     {
       Out << "\n" << setw(7) << Area << ", pos " << setw(7) << c << setw(7) << r
        << ", Ratio " << setw(12) << Ratio.Val(Area) 
        << ", Radius " << setw(12) << Radius.Val(Area) 
        << ", YHS " << setw(9) << Y << setw(9) << H << setw(9) << Sat << flush;
     }

   } 

   Ratio.Smooth();
   Radius.Smooth();


   for(Area = 1; Area <= stopArea; Area++)
   {
//     if(Radius.Val(Area) > 1.5 && Ratio.Val(Area) > GrowRatio) break;
     if(Ratio.Val(Area) > GrowRatio){Area--; break;}
   }

  

// Find the largest area for which the ratio of
// color distance from skin to the redeye seed
// pixel is greater than GrowRatio and the aspectRatio
// ratio does not exceed the maximum allowed.
   for(Area = stopArea; Area >= 1; Area--)
   {
     if(Ratio.Val(Area) < GrowRatio && Radius.Val(Area) < 2.0) break;
   }


   if(debug)
   {
     Out << "\n\n Area " << setw(7) << Area << endl;
     Out.close();
   }

   return Area;     
}
*/

void RegionGrower::write(ostream& out) {
	
	out << "\nRegionGrower:"
		<< "\n colSeed= " << colSeed << ", rowSeed= " << rowSeed << ", cvSeed= " << cvSeed
		<< "\n Area " << Area << ", Mean CV=" << setw(12) << cvAvg << ", Sigma=" << setw(12) << cvSigma
		<< "\n Ellipse center= " << setw(12) << c_center << setw(12) << r_center
		<< "\n Last pixel added at " << setw(12) << memberList.Val(Area, 1) << setw(12) << memberList.Val(Area, 2)
		<< ", aspectRatio= " << setw(12) << aspectRatio << "\n Fit= " << setw(12) << Fit
		<< ", Quality= " << getRegionQuality() << endl;
}
