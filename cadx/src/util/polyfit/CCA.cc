#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>
#include <fstream.h>
#include <math.h>
#include "Iem.h" 
#include "IemIO.h" 
#include "IemLUT.h"




int CCA ( IemTPlane <byte> &xy,int index, double &xcentroid, double &ycentroid, int &REpixels)
{

	int 	row = xy.rows(), col = xy.cols();
	IemTPlane <byte> x(row,col); 
	x = xy.copy();

	IemTPlane <byte> map(row,col); 
	map = x.copy();
	int	i=0,j=0,k,l,stackindex = 0;
	int	newi, newj; 
	long 	N=0, blobnumber = 0;
	int	bucket;
	int 	stacksize = 100*((int)sqrt(row*row+col*col));
	int  	istack[stacksize], jstack[stacksize];
 
	int	xsum = 0, ysum = 0; 
	REpixels = 0;

	// This loop identifies each belief blob and
	// records the scale factor for each blob in the 
	// array list_of_scale_factors.
	for(i=0; (i<row); i++)
	{
	for(j=0; (j<col); j++)
	{
		N = 0; 
		if(map[i][j] ==index)  //found a start of a blob
		{
			//cout<<"FOUND\n"; 
			bucket = map[i][j];
			N++;
			blobnumber++; 
			xsum +=j;
			ysum +=i; 		 			

			//place location onto the stack 
			istack[stackindex] = i;
			jstack[stackindex] = j;
			stackindex++; 
			map[i][j] = 0; 
			while(stackindex)
			{
				stackindex--; 
				if(stackindex<0)
				{
					printf("STACK UNDERFLO\n");
					exit(1);
				}
				newi = istack[stackindex];
				newj = jstack[stackindex];
				for (k = -1 ; k <2; k++)
				{
				for (l = - 1; l<2; l++)
				{
 		
					if((newi+k>=0)&&(newi+k<row)&&(newj+l>=0)&&(newj+l<col))
					{	
						if(map[newi+k][newj+l]==index)
						{
							//printf("%d\n",N); 
							N++;
							xsum +=j;
							ysum +=i;
 			
							istack[stackindex] = newi+k;
							jstack[stackindex] = newj+l;
							stackindex++;
							if(stackindex>=stacksize) 
							{
									printf("STACK OVERFLO %d %d \n", stackindex, stacksize);
								exit(1);
							}
							map[newi+k][newj+l] = 0;			
						}
					}
					
				}} 
				//this located one belief blob	
			}
			REpixels = N; 
		}

	}}
	 
	xcentroid = xsum /(double)REpixels;
	ycentroid = ysum/(double)REpixels; 
	//cout <<REpixels<<"\n"; 
	if(REpixels) return 1;
	else return 0; 
}


    
