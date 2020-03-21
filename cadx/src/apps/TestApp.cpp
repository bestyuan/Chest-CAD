//
//  (c) Copyright Eastman Kodak Company
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//



#include "Iem/Iem.h"   
#include "Iem/IemImageIO.h" 
#include "Iem/IemCrop.h"   
#include "Iem/rasterOps.h" 
#include "Iem/IemInterpolate.h"
#include "Iem/IemMorphological.h"  
#include "Iem/IemAggregate.h"  






int main(int argc, char* argv[]) {  
	

	long subsample = 4;

	IemTImage<unsigned char> imgMask;
	imgMask << "C:/send/NYPH02152005_C11_CO_Preprocessor_Mask1.tif";


 	imgMask >> "C:/send/LittleMask.png";
	IemTImage<unsigned char> imgMaskBig = iemUpsample(imgMask, subsample, subsample);
	imgMaskBig >> "C:/send/BigMask.png";


	return 0;
}



