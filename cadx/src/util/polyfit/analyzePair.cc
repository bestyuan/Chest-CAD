#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>
#include <fstream.h>
#include <math.h>
#include "Iem.h" 
#include "IemIO.h" 
#include "IemLUT.h"

#include "CCA.h"
#include "geoDistance.h"


void analyzePair(int eyePair, int hist [],int  world [], int histbins, IemTImage <byte> & mask)
{
	int 	row = mask.rows(), col = mask.cols();

	int	pixelsLeft;
	double	xcentroidLeft, ycentroidLeft; 	
	int	pixelsRight;
	double	xcentroidRight, ycentroidRight; 
	double	normdist;
	int	indexedDist; 	
	int	leftEyeIndex = 2*eyePair;
	int	rightEyeIndex = 2*eyePair+1; 
	int	distanceBetween; 
	int	leftValid, rightValid; 


	leftValid = CCA(mask[1], leftEyeIndex,xcentroidLeft,ycentroidLeft,pixelsLeft);
	rightValid = CCA(mask[1], rightEyeIndex,xcentroidRight,ycentroidRight,pixelsRight);
	if(leftValid&&rightValid)
	{
		distanceBetween = (int)(geoDistance(xcentroidLeft,ycentroidLeft,xcentroidRight,ycentroidRight)+.5); 

		cout<<"LEFT : "<<xcentroidLeft<<"\t"<<ycentroidLeft<<"\t"<<pixelsLeft<<"\n";
		cout<<"RIGHT: "<<xcentroidRight<<"\t"<<ycentroidRight<<"\t"<<pixelsRight<<"\n";
		cout<<"DIST:  "<<distanceBetween<<"\n"; 

		//update histograms
		//left eye
		// set box limits
		int left = (int)(xcentroidLeft-distanceBetween/2) ; 
		int right =  (int)(xcentroidLeft +distanceBetween/2) ; 
		int top =   (int)(ycentroidLeft - distanceBetween/2) ;
		int bottom =  (int)(ycentroidLeft +distanceBetween/2) ;

		if(left < 0 ) left = 0;
		if(right >= col) right = col-1; 
		if(top<0) top = 0;
		if(bottom>=row) bottom = row-1;


		// now update!!!
		for (int ii = top; ii<=bottom; ii++)
		{
		for (int jj = left; jj<=right;jj++)
		{
			normdist = sqrt((ii-ycentroidLeft)*(ii-ycentroidLeft) +(jj-xcentroidLeft)*(jj-xcentroidLeft) )/ distanceBetween;
			indexedDist = (int)(normdist*2*histbins);	//not rounded on purpose
			//increment 
			//if(indexedDist<0) indexedDist=0;
			//else if (indexedDist >=histbins) indexedDist = histbins-1; 	

			if((indexedDist>=0)&&(indexedDist <histbins))world[indexedDist] ++;
			if(indexedDist ==0) cout<<ii<<"\t"<<jj<<"\t"<<ycentroidRight<<"\t"<<xcentroidRight<<"\t"<<distanceBetween<<"\n";

			if(mask[1][ii][jj]==leftEyeIndex) hist[indexedDist] ++;
 
		}}
 

		//right eye
		left = (int)(xcentroidRight-distanceBetween/2) ; 
	 	right =  (int)(xcentroidRight +distanceBetween/2) ; 
	 	top =   (int)(ycentroidRight - distanceBetween/2) ;
	 	bottom =  (int)(ycentroidRight +distanceBetween/2) ;

		if(left < 0 ) left = 0;
		if(right >= col) right = col-1; 
		if(top<0) top = 0;
		if(bottom>=row) bottom = row-1;


		// now update!!!
		for (int ii = top; ii<=bottom; ii++)
		{
		for (int jj = left; jj<=right;jj++)
		{
			normdist = sqrt((ii-ycentroidRight)*(ii-ycentroidRight) +(jj-xcentroidRight)*(jj-xcentroidRight) )/ distanceBetween;
			indexedDist = (int)(normdist*2*histbins);	//not rounded on purpose
			//increment 
		//if(indexedDist<0) indexedDist=0;
			//else if (indexedDist >=histbins) indexedDist = histbins-1; 

			if((indexedDist>=0)&&(indexedDist <histbins))world[indexedDist] ++;
			if(indexedDist ==0) cout<<ii<<"\t"<<jj<<"\t"<<ycentroidRight<<"\t"<<xcentroidRight<<"\t"<<distanceBetween<<"\n";

			if(mask[1][ii][jj]==rightEyeIndex) hist[indexedDist] ++;
 
		}}
 
	}
}

    
