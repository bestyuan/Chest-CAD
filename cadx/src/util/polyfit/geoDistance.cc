#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iostream.h>
#include <fstream.h>

 
double geoDistance(float x1, float y1, float x2, float y2)
{
	float d; 
	d = sqrt( (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) );
	return d; 
}

