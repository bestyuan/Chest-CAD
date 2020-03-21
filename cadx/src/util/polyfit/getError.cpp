#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>
#include <fstream.h>
#include <math.h>
#include "Iem\Iem.h" 
#include "Iem\IemIO.h" 
#include "getError.h"


double getError(const IemMatrix & est, const IemMatrix & reference)
{
	long	lhRows, lhCols; 
	long	rhRows, rhCols;
	double	squareErr = 0, sumErr=0, err; 
	double	outputError;
	long	samples;

	lhRows = est.rows();	
	lhCols = est.cols();

	rhRows = reference.rows();	
	rhCols = reference.cols();

	samples = lhRows*lhCols; 

	if((lhRows!=rhRows)||(lhCols!=rhCols))
	{
		printf("getError iemMatrices are not the same size\n");
		exit(1); 
	}

	for (int ii=0; ii<lhRows; ii++)
	{
	for (int jj = 0; jj<lhCols; jj++)
	{
		err = est[ii][jj]-reference[ii][jj];
		sumErr+=err;
		squareErr += err*err;		

	}}

	double meanErr = sumErr / (double)samples;

	outputError = sqrt(squareErr / (double)samples - meanErr * meanErr); 

	return outputError; 
}

 
