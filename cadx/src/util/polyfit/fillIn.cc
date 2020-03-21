#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iostream.h>
#include <fstream.h>
#include "Iem.h" 
#include "IemIO.h" 
#include "IemLUT.h"
#include "IemCore.h"
#include "IemImageIO.h"
#include "IemConvert.h"
#include "IemOperators.h"
#include "fillIn.h"


int  fillIn(IemTPlane <short> &inplane, IemTPlane <byte> &fillMap,int size)
{

	int ro = inplane.rows();
	int co = inplane.cols();
	//IemTPlane <short> pRows(ro,co);
	//pRows = 0; 
	//IemTPlane <short> pCols(ro,co);
	//pCols = 0; 

	int pixelCounter = 0;  
	int currentPixel;
	int influenceCounter = 0;
	int currentMax = 0;
  
	int 	index, tag; 


	//first do each row
	for (int ii =0; ii<ro; ii++)
	{
		pixelCounter = 0; 
		influenceCounter = 0; 
		currentMax = 0; 
		tag = 0;
	 
	for (int jj = 0; jj<co; jj++)
	{
 		//moving across each row...

		currentPixel= inplane[ii][jj];
		if ((influenceCounter) && (currentPixel) &&(currentPixel != tag) )
		{
		
			//then we need to back up and fill in the zeros as detected gaps
			for (int kk = 1;kk<= size-influenceCounter; kk++)
			{
				index = jj-kk ;
				if (index>=0) fillMap[ii][index] = 1; 				
			}
			influenceCounter = size+1; 
			tag = currentPixel; 
		}
		else if(currentPixel)
		{
			
			influenceCounter = size+1; 
			tag = currentPixel; 
		}		
		//if(ii == 130) 
		//{
		//	printf("%d\t%d\t%d\t%d\n",jj,currentPixel, tag ,influenceCounter  );
		//}

	
		//get ready for next pixel  
		if(influenceCounter) influenceCounter--; 

	}



	}




	//next do each row
	for (int ii =0; ii<co; ii++)
	{
		pixelCounter = 0; 
		influenceCounter = 0; 
		currentMax = 0; 
		tag = 0;
	 
	for (int jj = 0; jj<ro; jj++)
	{
 		//moving down each column each row...

		currentPixel= inplane[jj][ii];
		if ((influenceCounter) && (currentPixel) &&(currentPixel != tag) )
		{
		
			//then we need to back up and fill in the zeros as detected gaps
			for (int kk = 1;kk<= size-influenceCounter; kk++)
			{
				index = jj-kk ;
				if (index>=0) fillMap[index][ii] = 1; 				
			}
			influenceCounter = size+1; 
			tag = currentPixel; 
		}
		else if(currentPixel)
		{
			
			influenceCounter = size+1; 
			tag = currentPixel; 
		}		
		//if(ii == 130) 
		//{
		//	printf("%d\t%d\t%d\t%d\n",jj,currentPixel, tag ,influenceCounter  );
		//}

	
		//get ready for next pixel  
		if(influenceCounter) influenceCounter--; 

	}



	}


	return 1;  
}
