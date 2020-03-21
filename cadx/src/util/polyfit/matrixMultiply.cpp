#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>
#include <fstream.h>
#include <math.h>
#include "Iem\Iem.h" 
#include "Iem\IemIO.h" 
#include "Iem\IemLUT.h"
#include "matrixMultiply.h"


IemMatrix matrixMultiply(const IemMatrix &lhMat,const IemMatrix &rhMat)
{
	int	lhRows, lhCols; 
	int	rhRows, rhCols; 
	int	outRows, outCols; 

	lhRows = lhMat.rows();	
	lhCols = lhMat.cols();

	rhRows = rhMat.rows();	
	rhCols = rhMat.cols();

	if (lhCols != rhRows )
	{
		printf("DIMENSION MISMATCH IN matrixMultiply\n\n"); 
		exit(1); 
	} 


	outRows = lhRows;
	outCols = rhCols;
 
	IemMatrix 	outMat(outRows, outCols); 
	double		currentProduct; 

	//printf("Rows COls Out %d %d \n", outRows, outCols); 
	for (int ii = 0; ii<outRows; ii++)
	{
	for (int jj = 0; jj<outCols; jj++)
	{	
		
		currentProduct = 0.0; 
		for ( int kk = 0; kk <lhCols; kk++)
		{
			currentProduct += lhMat[ii][kk] * rhMat[kk][jj];
			//if(currentProduct<0.0) printf("NEG \t");  
		}

		outMat[ii][jj] = currentProduct;
		//if(currentProduct<0.0) printf("NEG pos  %d %d Val %.2f\t",ii,jj,currentProduct);
	}}
	return outMat; 
}



//pre-allocated output (Jeff Snyder Version) 
int matrixMultiply(const IemMatrix &lhMat,const IemMatrix &rhMat, IemMatrix &preAllocatedOutput)
{
	int	lhRows, lhCols; 
	int	rhRows, rhCols; 
	int	expectedOutRows, expectedOutCols; 
	int	outRows, outCols; 

	lhRows = lhMat.rows();	
	lhCols = lhMat.cols();

	rhRows = rhMat.rows();	
	rhCols = rhMat.cols();

	if (lhCols != rhRows )
	{
		printf("DIMENSION MISMATCH IN matrixMultiply\n\n"); 
		exit(1); 
		// Exception thrown?
	} 


	expectedOutRows = lhRows;
	expectedOutCols = rhCols;
	outRows = preAllocatedOutput.rows();
	outCols = preAllocatedOutput.cols();

	if((expectedOutCols!=outCols)||(expectedOutRows!=outRows))
	{
		printf("output Matrix of matrixMultiply is not correct size\n"); 
		return(0); 
		//(Exception thrown? ); 
	}
 
	double		currentProduct; 

	for (int ii = 0; ii<outRows; ii++)
	{
	for (int jj = 0; jj<outCols; jj++)
	{	
		
		currentProduct = 0; 
		for ( int kk = 0; kk <lhCols; kk++)
		{
			currentProduct += lhMat[ii][kk] * rhMat[kk][jj]; 
		}

		preAllocatedOutput[ii][jj] = currentProduct;
	}}
	return 1; 
}


