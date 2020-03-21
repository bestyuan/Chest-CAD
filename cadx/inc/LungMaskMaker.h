//
//  (c) Copyright Eastman Kodak Company
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//           



#ifndef LUNGMASKMAKER_H
#define LUNGMASKMAKER_H


#include "Iem/Iem.h"
#include "UserAgent.h"




namespace CADX_SEG {


class LungMaskMaker {

	char paramFileName[1024];
	char rightLungFileName[1024];
	char leftLungFileName[1024];

	int subsample;
	bool reorient;
	bool verbose;

	// Max code value in input image.
	short maxCodeValue;

	public:

	LungMaskMaker();

	LungMaskMaker(char* paramFileName, char* rightLungFileName, char* leftLungFileName, int subsample = 4);

	IemTImage<unsigned char> makeMask(IemTImage<short>& img);

	IemImage convertTo8Bit(IemTImage<short>& img);
	
	void setMaxCodeValue(short v) {maxCodeValue = v;}


	private:
	
	void initialize();
	
	
	vector<byte *> Around_the_Points(IemTPlane<byte> &img, vector<byte *> points);
	
	void FillShape(IemPlane output, int x0, int y0); 
	
	void DrawShape(IemPlane &output, vector<float> &ShapeV, int factor);
















};


} // End namespace


#endif
