//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//




#ifndef FIT_H
#define FIT_H



#include "nr.h"   
 


class Fit
{   
	
	void function(float, float [], int);     

	static void lfit(float x[], float y[], float sig[], int ndat, float a[], int ia[],
	 int ma, float **covar, float *chisq, void (*funcs)(float, float [], int));
	 
	static void covsrt(float **covar, int ma, int ia[], int mfit);

};




#endif