//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//

#include "ScaleSelector.h" 
#include "RegionFeature.h"



using namespace CADX_SEG; 


ScaleSelector::ScaleSelector() {

}


double ScaleSelector::getScore(IemTImage<short>& img, IemTImage<unsigned char>& imgRegionMap,
 IemTImage<unsigned char>& imgLungMask) {
	RegionFeature feature("Nodule-Rib", img, imgRegionMap, imgLungMask);
 	feature.calculate();
	return feature.getScore();
}

