#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>
#include <fstream.h>
#include <math.h>
#include "Iem/Iem.h" 
#include "Iem/IemIO.h" 
#include "Iem/IemLUT.h"
#include "matrixTranspose.h"


IemMatrix matrixTranspose(const IemMatrix &inMat)
{
	int	inRows, inCols; 
	int	outRows, outCols; 

	inRows = inMat.rows();	
	inCols = inMat.cols();
	outRows = inCols;
	outCols = inRows;
 
	IemMatrix 	outMat(outRows, outCols); 

	for (int ii = 0; ii<inRows; ii++)
	{
	for (int jj = 0; jj<inCols; jj++)
	{	
		outMat[jj][ii] = inMat[ii][jj];
	}}
	return outMat; 
}






// this is the "jeff Snyder" version of the function. 
int	 matrixTranspose(const IemMatrix &inMat,  IemMatrix &preAllocatedOutput)
{
	int	inRows, inCols,outRows, outCols; 
	int	outRowsExpect, outColsExpect; 

	inRows = inMat.rows();	
	inCols = inMat.cols();

	outRows = preAllocatedOutput.rows();
	outCols = preAllocatedOutput.cols();

	outRowsExpect = inCols;
	outColsExpect = inRows;

	if((outRowsExpect!=outRows )||(outColsExpect!=outCols ))
	{
		printf("Dimension Mismatch in preallocated transpose\n");
		return(1); 
		//  {THROW EXCEPTION ?}
	}
 
	for (int ii = 0; ii<inRows; ii++)
	{
	for (int jj = 0; jj<inCols; jj++)
	{	
		preAllocatedOutput[jj][ii] = inMat[ii][jj];
	}}

	return(0); 		//success

}
