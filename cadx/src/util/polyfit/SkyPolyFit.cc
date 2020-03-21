#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iostream.h>
#include <fstream.h>
 
#include "Iem.h" 
#include "IemIO.h" 
#include "IemLUT.h"
#include "analyzePair.h"
#include "CCA.h"
#include "geoDistance.h"
#include "matrixTranspose.h"
#include "matrixMultiply.h"
#include "getError.h"


void help()
{


     printf("SkyPolyFit -i ImageName.tif -oMask outputMask.tif -iMask inputMask -coords x1 x2 y1 y2 -tolerance [3.0] -sample [1]\n");
    printf("\n");
     printf("\n");
    exit(0);
}
	

// *******************************************************************
int main(int argc, char *argv[])
{
int     k=0;
char    *ifile1 = "in.tif" ,*imask= NULL, *omask= NULL;
int     ro,co,c;
double 	tolerance = 3.0; 
int	x1=0,x2=0,y1=0,y2=0; 
int	dimension_problem_flag =0; 
int	statsFlag = 0;
double 	factorR, factorC; 
int	xSum=0, ySum = 0;
int	sample =1; 
double	xMean=0, yMean=0; 

     if (argc < 2)	help();
    else		if (!strcmp(argv[1], "-h"))  help();
    while (argc > ++k)
      {		
 		if (!strcmp( argv[k], "-i" ))		ifile1 = argv[++k];
	    else if (!strcmp( argv[k], "-iMask" )) 	imask = argv[++k];	  
	    else if (!strcmp( argv[k], "-oMask" )) 	omask = argv[++k];	
		
	    else    if (!strcmp( argv[k], "-tolerance" ))	tolerance  =atof( argv[++k] );
	    else    if (!strcmp( argv[k], "-stats" ))	statsFlag =1;
	    else    if (!strcmp(argv[k],"-sample")) 	sample = atoi(argv[++k]);
            else if (!strcmp( argv[k], "-coords" ))	
		{
			x1 =  atoi(argv[++k]);
			x2 =  atoi(argv[++k]);
			y1 =  atoi(argv[++k]);
			y2 =  atoi(argv[++k]);
		}
	  
	else
	  {
	    fprintf( stderr, "Unable to interpret parameter {%s}!\n\n", argv[k] );
		exit(1); 
	  }
      }


try{

	IemImage imgua = iemRead(ifile1);			//READ IN IMAGE, BYTE OR SHORT
 	IemTImage <short> imga;  				//INITIALIZE SHORT IMAGE
		
	IemType input_type =imgua.type();			//GET INPUT IMAGE TYPE
	if(input_type == IemShort) 	imga = imgua;		//MAKE IMGA A SHORT VERSION OF IMGUA
	else 				imga = convert(imgua, IemShort); 

  	c = imga.chans();
   	ro = imga.rows();
    	co = imga.cols();
	IemTImage <byte> outputMask(1,ro,co);			//this is the output mask

/*
	double matdata[] = { 1, 2, 3, 4 ,5 ,6,
			     10 , 11 , 12 ,12 , 13 ,16,
			 21,21,21,21,21,21,
			31,30,31,30,31,30,
			41,42,42,41,41,42,
			51,51,51,54,54,54,
			61,64,62,64,63,60,
			76,75,73,72,74,72,
			82,83,84,82,83,84,
			91,91,93,93,94,94};
				

	IemMatrix	vandermonde(10,6,matdata);
	cout<<vandermonde[7][4]<<"\n"; 
	cout<<vandermonde[4][7]<<"\n";
	cout<<vandermonde[7][4]<<"\n";
*/


	IemMatrix	vandermonde;
	IemMatrix	listR;
	IemMatrix	listG;
	IemMatrix	listB;

	if(imask)		//sky pixels are identified by mask
	{
		// READ IN THE MASK, MAKE IT A SHORT
		IemImage inMaska = iemRead(imask);			//READ IN IMAGE, BYTE OR SHORT
 		IemTPlane <short> inMask;  				//INITIALIZE SHORT PLANE
		
		IemType input_type =inMaska.type();			//GET INPUT IMAGE TYPE
		if(input_type == IemShort) 	inMask = inMaska[0];		//MAKE IMGA A SHORT VERSION OF IMGUA
		else 				inMask = convert(inMaska[0], IemShort); 


			if( inMask.rows() != ro ) 
			{
				printf("DIMENSION MISMATCH (ROWS)!\n");
				factorR = ro/inMask.rows();
				dimension_problem_flag = 1;
				 
			}
			if( inMask.cols() != co ) 
			{
				printf("DIMENSION MISMATCH (COLS)\n");
				factorC = co/inMask.cols();
				dimension_problem_flag = 1;  
			}
			if(dimension_problem_flag ) 
			{
				if (factorR == factorC) //then interpolate the image
				{
					inMask = interpolate(inMask, ro, co);
					printf("Interpolating Belief Map by %.2f\n",factorC); 
				}
				else
				{
			 		printf("EXITING\n\n");
					exit(1); 
				}
			}
		int	numberSetPixels = 0;
		for (int ii = 0 ; ii< ro; ii+=sample)
		{
		for (int jj = 0; jj<co; jj+=sample)
		{
			if (inMask[ii][jj])
			{ 
				numberSetPixels++;
				xSum+= jj;			//these will be the center of my coordinate system
				ySum += ii;
			}
		}}		
		xMean = xSum /(double)numberSetPixels; 
		yMean = ySum /(double)numberSetPixels;	
		printf("Set Pixels %d out of %d (%.2f%%)\n", numberSetPixels,ro*co, 100*double(numberSetPixels)/ro/co); 
		printf("The xMean is:\t%.2f and the yMean is:\t%.2f\n", xMean, yMean); 

		int	index =0; 
		IemMatrix vanderTemp( numberSetPixels,6 ); 
		IemMatrix rTemp( numberSetPixels,1 );
		IemMatrix gTemp( numberSetPixels,1 );
		IemMatrix bTemp( numberSetPixels,1 );
		for (int ii = 0 ; ii< ro; ii+=sample)
		{
		for (int jj = 0; jj<co; jj+=sample)
		{
			if (inMask[ii][jj])
			{ 
				outputMask[0][ii][jj] = 254; 				//marking an originally known pixel
				vanderTemp[index][0] = (jj-xMean)*(jj-xMean);		//xx
				vanderTemp[index][1] = (jj-xMean)*(yMean-ii);		//xy
				vanderTemp[index][2] = (yMean-ii)*(yMean-ii);		//yy
				vanderTemp[index][3] =	(jj-xMean);			//x
				vanderTemp[index][4] =	(yMean-ii);			//y
				vanderTemp[index][5] = 1;

				rTemp[index][0] = imga[0][ii][jj];
				gTemp[index][0] = imga[1][ii][jj];
				bTemp[index][0] = imga[2][ii][jj];

				index ++; 
			}
		}}	
		vandermonde = vanderTemp; 
		listR=rTemp;
		listG=gTemp; 
		listB=bTemp; 	

	}
	else if (x1||x2||y1||y2)	//sky pixels identified by oposite corners of a rectangular region
	{	
		int xMin = x1;
		int yMin = y1;
		int xMax = x2;
		int yMax = y2;
		if(yMax<yMin) 
		{
			yMax = y1;
			yMin = y2;
		}
		if(xMax<xMin) 
		{
			xMax = x1;
			xMin = x2;
		}
		if(xMin < 0  || yMin<0 ||yMax>ro-1 ||xMax>co-1)
		{
			printf("Invalid Coordinates\n");
			exit(1); 
		}
		xMean = (xMax+xMin)/2.0;
		yMean = (yMax+yMin)/2.0; 

		int	numberSetPixels = (yMax-yMin+1)*(xMax-xMin+1);

		printf("Set Pixels %d out of %d (%.2f%%)\n", numberSetPixels,ro*co, 100*double(numberSetPixels)/ro/co); 
		printf("The xMean is:\t%.2f and the yMean is:\t%.2f\n", xMean, yMean); 

		int	index =0; 
		IemMatrix vanderTemp( numberSetPixels,6 ); 
		IemMatrix rTemp( numberSetPixels,1 );
		IemMatrix gTemp( numberSetPixels,1 );
		IemMatrix bTemp( numberSetPixels,1 );
		for (int ii = yMin; ii<=yMax; ii+=sample)
		{
		for (int jj = xMin; jj<=xMax; jj+=sample)
		{
			outputMask[0][ii][jj] = 254; 				//marking an originally known pixel
			vanderTemp[index][0] = (jj-xMean)*(jj-xMean);		//xx
			vanderTemp[index][1] = (jj-xMean)*(yMean-ii);		//xy
			vanderTemp[index][2] = (yMean-ii)*(yMean-ii);		//yy
			vanderTemp[index][3] =	(jj-xMean);			//x
			vanderTemp[index][4] =	(yMean-ii);			//y
			vanderTemp[index][5] = 1;

			//vanderTemp[index][0] = (jj)*(jj);		//xx
			//vanderTemp[index][1] = (jj)*(ii);		//xy
			//vanderTemp[index][2] = (ii)*(ii);		//yy
			//vanderTemp[index][3] =	(jj);			//x
			//vanderTemp[index][4] =	(ii);			//y
			//vanderTemp[index][5] = 1;

			rTemp[index][0] = imga[0][ii][jj];
			gTemp[index][0] = imga[1][ii][jj];
			bTemp[index][0] = imga[2][ii][jj];
			index ++; 
			
		}}	
		vandermonde = vanderTemp; 
		listR=rTemp;
		listG=gTemp; 
		listB=bTemp; 	
		
	}
	else
	{
		printf("No pixels have been identified\n");
		exit(1); 
	}


	// Compute the pseudo Inverse of the vandermonde

	IemMatrix 	transposeVander = matrixTranspose(vandermonde);
	IemMatrix	vanderTVander = matrixMultiply(transposeVander, vandermonde);
	IemMatrix	inverseVanderTVander = vanderTVander.inverse(); 
	IemMatrix 	pseudoInverse =matrixMultiply(inverseVanderTVander, transposeVander); 
	IemMatrix	redCoefs = matrixMultiply(pseudoInverse,listR);
	IemMatrix 	grnCoefs = matrixMultiply(pseudoInverse,listG);
	IemMatrix	bluCoefs = matrixMultiply(pseudoInverse,listB);

	//Free up some memory
	//delete &transposeVander;
	//delete &pseudoInverse;

	IemMatrix	estimate = matrixMultiply(vandermonde, redCoefs); 
	double		redErr = getError(estimate, listR);
	estimate = matrixMultiply(vandermonde, grnCoefs); 
	double		grnErr = getError(estimate, listG);
	estimate = matrixMultiply(vandermonde, bluCoefs); 
	double		bluErr = getError(estimate, listB);

	// FIt the polynomial to the entire image and see what happens!!

	int rMin, gMin, bMin;
	double	xC, yC; 

	for (int ii = 0; ii <ro; ii++)
	{
	for (int jj = 0; jj<co; jj++)
	{
		yC = yMean-ii;
		xC = jj-xMean;

		//yC = ii;
		//xC = jj;

		rMin = (int)(redCoefs[0][0]* xC*xC + redCoefs[1][0]*xC*yC + redCoefs[2][0]*yC*yC + redCoefs[3][0]*xC + redCoefs[4][0]*yC + redCoefs[5][0] - redErr*tolerance +.5); 

		gMin =(int)( grnCoefs[0][0]* xC*xC + grnCoefs[1][0]*xC*yC + grnCoefs[2][0]*yC*yC + grnCoefs[3][0]*xC + grnCoefs[4][0]*yC + grnCoefs[5][0] - grnErr*tolerance +.5) ;

		bMin = (int)(	bluCoefs[0][0]* xC*xC + bluCoefs[1][0]*xC*yC + bluCoefs[2][0]*yC*yC + bluCoefs[3][0]*xC + bluCoefs[4][0]*yC + bluCoefs[5][0] - bluErr*tolerance +.5);


		if(outputMask[0][ii][jj]!= 254)
		{
			outputMask[0][ii][jj] = 0;

			if( ( imga[0][ii][jj]  >rMin) &&(   imga[1][ii][jj]>gMin) &&( imga[2][ii][jj] >bMin) &&( imga[0][ii][jj]  <rMin+2* tolerance*redErr) &&(  imga[1][ii][jj] <gMin+ 2*tolerance*grnErr) &&( imga[2][ii][jj]  <bMin+ 2*tolerance*bluErr) ) 		outputMask[0][ii][jj] = 255;
		}	
	}}



	if (omask) iemWrite(outputMask,omask );




	printf("Computed COeficients\n");
	for (int ii =0; ii<6;ii++)
	{
		printf("%.6f\t%.6f\t%.6f\t\n",redCoefs[ii][0], grnCoefs[ii][0],bluCoefs[ii][0]);
	}	
 

	printf("Errors:\t%.2f\t%.2f\t%.2f\n",redErr,grnErr,bluErr); 
   }
   catch(IemError &err)
   {
	cerr << "Caught IEM Error: " << (const char*)err << endl;
	exit(1); 
   }
   exit(0); 


}
