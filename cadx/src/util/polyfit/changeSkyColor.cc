#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iostream.h>
#include <fstream.h>
 
#include "Iem.h" 
#include "IemIO.h" 
#include "IemLUT.h"

void help()
{


     printf("changeSkyColor -i ImageName.tif  -iMask inputMask -satScale -bal -hueRot\n");
    printf("\n");
     printf("\n");
    exit(0);
}
	

// *******************************************************************
int main(int argc, char *argv[])
{
int     k=0;
char    *ifile1 = "in.tif" ,*imask= NULL, *ofile= NULL;
int     ro,co,c;
double 	threshold =0.3,satScale =1.0; 

int	r,g,b,nr,ng,nb,sr=0,sg=0,sb=0,nn; 
int	factorR, factorC; 
int	dimension_problem_flag =0; 

     if (argc < 2)	help();
    else		if (!strcmp(argv[1], "-h"))  help();
    while (argc > ++k)
      {		
 		if (!strcmp( argv[k], "-i" ))		ifile1 = argv[++k];
	    else if (!strcmp( argv[k], "-iMask" )) 	imask = argv[++k];	  
	    else if (!strcmp( argv[k], "-o" )) 		ofile = argv[++k];	
	    else if (!strcmp( argv[k], "-threshold" ))	threshold  =atof( argv[++k] );
	    else if (!strcmp(argv[k],"-satScale")) 	satScale = atof(argv[++k]);
            else if (!strcmp( argv[k], "-bal" ))	
		{
			sr =  atoi(argv[++k]);
			sg =  atoi(argv[++k]);
			sb =  atoi(argv[++k]);
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

	IemType mask_type =inMaska.type();			//GET INPUT IMAGE TYPE
	if(mask_type == IemShort) 	inMask = inMaska[0];		//MAKE IMGA A SHORT VERSION OF IMGUA

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


	for (int ii = 0 ; ii< ro; ii++)
	{
	for (int jj = 0; jj<co; jj++)
	{

		if (inMask[ii][jj]>=integerThreshold)
		{ 
			r = imga[0][ii][jj];
			g = imga[1][ii][jj];
			b = imga[2][ii][jj];
			nn = (int)(.375*r + .5*g + .125*b + .5);			

			nr = (int)((r - nn) *satScale + nn +sr+.5) ;
			ng = (int)((g - nn) *satScale + nn +sg +.5);
			nb = (int)((b - nn) *satScale + nn +sb + .5) ;
		 	imga[0][ii][jj]=nr;
			imga[1][ii][jj]=ng;
			imga[2][ii][jj]=nb;


		}
		//do at less strength at neighbors
		else if ((ii>0) && (jj>0) &&(ii<ro-1) &&(jj<co-1))
		{
			if ((inMask[ii][jj-1]>=integerThreshold) ||(inMask[ii][jj+1]>=integerThreshold) ||(inMask[ii-1][jj]>=integerThreshold) ||(inMask[ii+1][jj]>=integerThreshold) ) 
			{
				r = imga[0][ii][jj];
				g = imga[1][ii][jj];
				b = imga[2][ii][jj];
				nn = (int)(.375*r + .5*g + .125*b + .5);			

				nr = (int)((r - nn) *((satScale-1)*.5+1.0) + nn +.5*sr+.5) ;
				ng = (int)((g - nn) *((satScale-1)*.5+1.0) + nn +.5* sg +.5);
				nb = (int)((b - nn) *((satScale-1)*.5+1.0) + nn +.5*sb + .5) ;
			 	imga[0][ii][jj]=nr;
				imga[1][ii][jj]=ng;
				imga[2][ii][jj]=nb;


			}

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
