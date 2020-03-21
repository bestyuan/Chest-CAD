#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>
#include <fstream.h>
#include <math.h>
#include "Iem.h" 
#include "IemIO.h" 
#include "IemLUT.h"
#include "labelRegions.h"



			
int labelRegions ( IemTPlane <short> &xy, IemTPlane <short> &label ,int threshold)
{

	int 	row = xy.rows(), col = xy.cols();


	int	i=0,j=0,k,l,stackindex = 0;
	int	newi, newj; 
	long  	blobnumber = 0;
	int	bucket;
	int 	stacksize = 100*((int)sqrt(row*row+col*col));
	int  	istack[stacksize], jstack[stacksize];
 


	for(i=0; (i<row); i++)
	{
	for(j=0; (j<col); j++)
	{
	 
		if(xy[i][j] >=threshold)  //found a start of a blob
		{ 
			bucket = xy[i][j];
			blobnumber++; 	
printf("blobnumber %d \tbucket %d\n", blobnumber,bucket); 
 

			//place location onto the stack 
			istack[stackindex] = i;
			jstack[stackindex] = j;
			stackindex++; 

			label[i][j] = blobnumber;
			xy[i][j] = 0; 

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
						if(xy[newi+k][newj+l]==bucket)
						{
 			
							istack[stackindex] = newi+k;
							jstack[stackindex] = newj+l;
							stackindex++;
							if(stackindex>=stacksize) 
							{
								printf("STACK OVERFLO %d %d \n", stackindex, stacksize);
								exit(1);
							}
							xy[newi+k][newj+l] = 0;
							label[newi+k][newj+l] = blobnumber;			
						}
					}
					
				}} 
				//this located one belief blob	
			}	 
		}
	}}
	return 0;  
}


    
