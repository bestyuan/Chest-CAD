//
//  (c) Copyright Eastman Kodak Company
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef IMAGEUTIL_H
#define IMAGEUTIL_H

 
#include "Iem/Iem.h"  

namespace CADX_SEG {


class ImageUtil {


	public:    
	static IemTImage<short> invert(IemTImage<short>& img, short min, short max);  
	
	static IemTImage<short> subtract(IemTImage<short>& img1, IemTImage<short>& img2);
	
	static IemTImage<short> pad(IemTImage<short>& img, short tpad, short lpad, short rpad, short bpad);
	
	static IemTImage<unsigned char> pad(IemTImage<unsigned char>& img, short tpad, short lpad, short rpad, short bpad);
	
	static IemTImage<unsigned char> imageToSet(IemTImage<short>& img, short threshold);
	
	static IemTImage<unsigned char> imageToSetUpperHist(IemTImage<short>& img, double upperPenetration);
	
	static IemTImage<short> min(IemTImage<short>& img1, IemTImage<short>& img2, long b);
	
	static IemTImage<short> max(IemTImage<short>& img1, IemTImage<short>& img2, long b);
	
	static IemTImage<unsigned char> mapToBoundary(IemTImage<unsigned char>& img, long* &xPoints, long* &yPoints, long& nPoints);

	static IemTImage<unsigned char> dilateMap(IemTImage<unsigned char>& _imgMap, short d, short aggregation = 2);

	static IemTImage<unsigned char> erodeMap(IemTImage<unsigned char>& _imgMap, short d, short aggregation = 2);

	// If the mask image equals maskValue set image band to setValue.
	static IemTImage<short> applyMask(IemTImage<short>& img, long band, 
	 IemTImage<unsigned char>& imgMask, unsigned char maskValue, short setValue);
	 
	static void threshold(IemTImage<unsigned char>& img, long b,
	short threshold, short lowCv, short highCv);
	
	// If pixel = codeValue output pixel is set to maskCodeValue otherwise set to 0.
	// Arguments meanX and meanY are set to mean position of pixel = codeValue.
	static IemTImage<unsigned char> codevalueToMask(IemTImage<unsigned char>& img,
 	 short codeValue, short maskCodeValue, double& meanX, double& meanY);

	static void makeMark(IemTImage<short>& img, long col, long row, short r, short g, short b, short size);
	static void makeMark(IemTImage<unsigned char>& img, long col, long row, short r, short g, short b, short size);
	static IemTImage<unsigned char> mapResize(IemTImage<unsigned char>& imgMap, long cols, long rows);

	static IemTImage<short> applyDoubleThreshold(IemTImage<short>& img, long b, short min, short max);

};

} // end namespace


#endif
