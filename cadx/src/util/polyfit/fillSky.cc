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
#include "labelRegions.h"
#include "fillIn.h"
#include "fillIn2.h"

void help()
{


     printf("fillSky -i ImageName.tif -oMask outputMask.tif -iMask inputMask -coords x1 x2 y1 y2 -tolerance [3.0] -sample [1]\n");
    printf("\n");
     printf("\n");
    exit(0);
}
	

// *******************************************************************
int main(int argc, char *argv[])
{
int     k=0;
char    *ifile1 = "in.tif" ,*imask= NULL, *ofile= NULL, *wireMap=NULL, *regionMap=NULL;
int     ro,co,c;
double 	tolerance = 3.0; 
double 	threshold =0.3; 
double 	noiseFactor = 0.5; 

int	x1=0,x2=0,y1=0,y2=0; 
int	dimension_problem_flag =0; 
int	statsFlag = 0;
double 	factorR, factorC; 
int	xSum=0, ySum = 0;
int	sample =1; 
int 	window = 5; 
int 	fillInStyle = 1; 
double	xMean=0, yMean=0; 

     if (argc < 2)	help();
    else		if (!strcmp(argv[1], "-h"))  help();
    while (argc > ++k)
      {		
 		if (!strcmp( argv[k], "-i" ))		ifile1 = argv[++k];
	    else if (!strcmp( argv[k], "-iMask" )) 	imask = argv[++k];	  
	    else if (!strcmp( argv[k], "-o" )) 		ofile = argv[++k];	
	    else if (!strcmp( argv[k], "-regionMap" )) 		regionMap = argv[++k];	
    	    else if (!strcmp( argv[k], "-wireMap" )) 		wireMap = argv[++k];	
	    else if (!strcmp( argv[k], "-threshold" ))	threshold  =atof( argv[++k] );
	    else if (!strcmp( argv[k], "-noiseFactor" ))	noiseFactor  =atof( argv[++k] );
		
	    else    if (!strcmp( argv[k], "-tolerance" ))	tolerance  =atof( argv[++k] );
	    else    if (!strcmp( argv[k], "-stats" ))	statsFlag =1;
    	    else    if (!strcmp( argv[k], "-fi2" ))	fillInStyle =2;
	    else    if (!strcmp(argv[k],"-sample")) 	sample = atoi(argv[++k]);
	    else if (!strcmp(argv[k],"-win")) 	window = atoi(argv[++k]);
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

	int	integerThreshold = (int)(threshold*255+.5); 
	IemImage imgua = iemRead(ifile1);			//READ IN IMAGE, BYTE OR SHORT
 	IemTImage <short> imga;  				//INITIALIZE SHORT IMAGE
		
	IemType input_type =imgua.type();			//GET INPUT IMAGE TYPE
	if(input_type == IemShort) 	imga = imgua;		//MAKE IMGA A SHORT VERSION OF IMGUA
	else 				imga = convert(imgua, IemShort); 

		
	IemImage inMaska = iemRead(imask);			//READ IN IMAGE, BYTE OR SHORT
 	IemTPlane <short> inMask;  				//INITIALIZE SHORT PLANE

  	c = imga.chans();
   	ro = imga.rows();
    	co = imga.cols();
	IemTImage <byte> outputMask(1,ro,co);			//this is the output mask



	IemMatrix	vandermonde;
	IemMatrix	listR;
	IemMatrix	listG;
	IemMatrix	listB;

	if(imask)		//sky pixels are identified by mask
	{

		
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
			if (inMask[ii][jj]>=integerThreshold)
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
			if (inMask[ii][jj]>=integerThreshold)
			{ 
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


	printf("Computed COeficients\n");
	for (int ii =0; ii<6;ii++)
	{
		printf("%.6f\t%.6f\t%.6f\t\n",redCoefs[ii][0], grnCoefs[ii][0],bluCoefs[ii][0]);
	}	
 

	printf("Errors:\t%.2f\t%.2f\t%.2f\n",redErr,grnErr,bluErr); 
	

	IemTPlane <short> labelMaskRegions(ro,co);
	labelMaskRegions = 0; 
	labelRegions(inMask,labelMaskRegions,integerThreshold); 
	if (regionMap) iemWrite(labelMaskRegions,regionMap );
	
	// Now I need to do the growing operation
	IemTPlane <byte> wires(ro,co);
	wires = 0; 	
	if (fillInStyle==1) fillIn(labelMaskRegions, wires, window); 
	else fillIn2(labelMaskRegions, wires, window);
	if (wireMap) iemWrite(wires*255,wireMap );
	
	
	// Now, replace wire pixels with sky pixels
	IemMatrix vanderOnePixel( 1,6 );
	IemMatrix codeValue(1,1); 
	for (int ii = 0 ; ii< ro; ii++)
	{
	for (int jj = 0; jj<co; jj++)
	{
		if (wires[ii][jj])  //detected wire pixel
		{ 
			vanderOnePixel[0][0] = (jj-xMean)*(jj-xMean);		//xx
			vanderOnePixel[0][1] = (jj-xMean)*(yMean-ii);		//xy
			vanderOnePixel[0][2] = (yMean-ii)*(yMean-ii);		//yy
			vanderOnePixel[0][3] =	(jj-xMean);			//x
			vanderOnePixel[0][4] =	(yMean-ii);			//y
			vanderOnePixel[0][5] = 1;
			
			//noise can also be added
			codeValue =  matrixMultiply(vanderOnePixel, redCoefs);
			imga[0][ii][jj] = (int)(codeValue[0][0]); 
			codeValue =  matrixMultiply(vanderOnePixel, grnCoefs);
			imga[1][ii][jj] = (int)(codeValue[0][0]); 
			codeValue =  matrixMultiply(vanderOnePixel, bluCoefs);
			imga[2][ii][jj] = (int)(codeValue[0][0]); 
		}
	}}	
	
	
	
		iemWrite(convert(imga, input_type),ofile);

	

	
   }
   catch(IemError &err)
   {
	cerr << "Caught IEM Error: " << (const char*)err << endl;
	exit(1); 
   }
   exit(0); 


}
