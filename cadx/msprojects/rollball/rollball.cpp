// rollball.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "kespr_io.h"

unsigned short int median(unsigned short int *arr, int n);
void median_cross(unsigned short int *image, int rows, int cols, unsigned short int **medianimage);
void rollball(unsigned short int *image, int rows, int cols, int radius,
	float **imagelow, float **imagehigh);
void rollball_float(float *image, int rows, int cols, int radius,
	float **imagelow, float **imagehigh);

int main(int argc, char* argv[])
{
	FILE *fptr = NULL;
	int rows = 0, cols = 0;
	unsigned short int *image = NULL, *medianimage=NULL;
	float *imageconvexlow = NULL, *imageconvexhigh = NULL,
		*imageconcavelow=NULL, *imageconcavehigh=NULL, *junkimage = NULL,
		*imagelow=NULL, *imagehigh=NULL;
	int r, c;
	unsigned short int *imageptr = NULL;
	unsigned long int val, lb, hb;
	struct KESPRHEADER *kheader = NULL;

	if(argc < 5) return(1);

	kheader = read_kespr_header(argv[1], (struct KESPRHEADER *)NULL);
	rows = get_kespr_rows(kheader);
	cols = get_kespr_cols(kheader);
	printf("rows = %d   cols=%d\n", rows, cols);

//	cols = 2048;
//	rows = 2500;

	image = (unsigned short int *) calloc(rows*cols, sizeof(unsigned short int));
	fptr = fopen(argv[1], "rb");
	fread(image, 2048, 1, fptr);
	fread(image, rows*cols, sizeof(unsigned short int), fptr);
	fclose(fptr);

	imageptr = image;
	for(r=0;r<rows;r++){
		for(c=0;c<cols;c++,imageptr++){
			val = (unsigned long int)(*imageptr);
			lb = val % 256;
			hb = val / 256;
			*imageptr = (unsigned short int)(lb*256+hb);
		}
	}

	/***************************************************************************
	* Apply a cross type median filter.
	***************************************************************************/
	printf("Starting median filtering!\n"); fflush(stdout);
	median_cross(image, rows, cols, &medianimage);
	printf("Finished median filtering!\n"); fflush(stdout);

	fptr = fopen(argv[2], "wb");
	fwrite(medianimage, rows*cols, sizeof(unsigned short int), fptr);
	fclose(fptr);

	/***************************************************************************
	* Compute a gray scale morphological filtering to produce a locally
	* convex surfaces above and below the image.
	***************************************************************************/
	printf("Started convex low & high rollball filtering!\n");
	rollball(medianimage, rows, cols, 39, &imageconvexlow, &imageconvexhigh);
	printf("Finished rollball filtering!\n");

	/***************************************************************************
	* Compute a gray scale morphological filtering to produce a locally
	* concave surfaces below the image.
	***************************************************************************/
	printf("Started concave low rollball filtering!\n");
	rollball_float(imageconvexlow, rows, cols, 39, &junkimage, &imageconcavelow);
	printf("Finished rollball filtering!\n");

	free(junkimage);
	junkimage = NULL;
   free(imageconvexlow);
	imageconvexlow = NULL;

	/***************************************************************************
	* Compute a gray scale morphological filtering to produce a locally
	* concave surfaces above the image.
	***************************************************************************/
	printf("Started concave high rollball filtering!\n");
	rollball_float(imageconvexhigh, rows, cols, 39, &imageconcavehigh, &junkimage);
	printf("Finished rollball filtering!\n");

   free(junkimage);
	junkimage = NULL;
	free(imageconvexhigh);
	imageconvexhigh = NULL;

	/***************************************************************************
	* Compute a gray scale morphological filtering to produce a more locally
	* convex surface below the image.
	***************************************************************************/
	printf("Started final rollball low filtering!\n");
	rollball_float(imageconcavelow, rows, cols, 11, &imagelow, &junkimage);
	printf("Finished rollball filtering!\n");

	free(junkimage);
	junkimage = NULL;

	/***************************************************************************
	* Compute a gray scale morphological filtering to produce a more locally
	* convex surface above the image.
	***************************************************************************/
	printf("Started final rollball high filtering!\n");
	rollball_float(imageconcavehigh, rows, cols, 11, &junkimage, &imagehigh);
	printf("Finished rollball filtering!\n");

	free(junkimage);
	junkimage = NULL;

	fptr = fopen(argv[3], "wb");
	fwrite(imagelow, rows*cols, sizeof(float), fptr);
	fclose(fptr);

	fptr = fopen(argv[4], "wb");
	fwrite(imagehigh, rows*cols, sizeof(float), fptr);
	fclose(fptr);

	return 0;
}

unsigned short int median(unsigned short int *arr, int n)
{
   int nn, l, h, m;
   unsigned short int tv;

	if(n >= 3){
		nn = n/2;
		for(l=0;l<=nn;l++){
			m = l;
			for(h=l+1;h<n;h++) if(arr[h] < arr[m]) m = h;
			if(m != l){
				tv = arr[l];
				arr[l] = arr[m];
				arr[m] = tv;
			}
		}
		if(n % 2) return(arr[nn]);
		return((arr[nn-1]+arr[nn])/2);
	}
	else if(n < 2) return(arr[0]);
	else return((arr[0]+arr[1])/2);
}

void median_cross(unsigned short int *image, int rows, int cols, unsigned short int **medianimage)
{
	int r, c;
	unsigned short int *imageptr = NULL, *imagemedianptr=NULL;
	unsigned short int cnt, arr[13];

	imagemedianptr = (unsigned short int *) calloc(rows*cols, sizeof(unsigned short int));
	*medianimage = imagemedianptr;

	imageptr = image;
   for(r=0;r<rows;r++){
      for(c=0;c<cols;c++,imageptr++,imagemedianptr++){
			if((r < 3) || (r > (rows-4)) || (c < 3) || (c > (cols-4))){
				arr[0] = *imageptr;
            cnt = 1;
				if((c-3) >= 0){
					arr[cnt] = *(imageptr - 3);
					cnt++;
				}
				if((c-2) >= 0){
					arr[cnt] = *(imageptr - 2);
					cnt++;
				}
				if((c-1) >= 0){
					arr[cnt] = *(imageptr - 1);
					cnt++;
				}
				if((c+1) < cols){
					arr[cnt] = *(imageptr + 1);
					cnt++;
				}
				if((c+2) < cols){
					arr[cnt] = *(imageptr + 2);
					cnt++;
				}
				if((c+3) < cols){
					arr[cnt] = *(imageptr + 3);
					cnt++;
				}
				if((r-3) >= 0){
					arr[cnt] = *(imageptr - 3*cols);
					cnt++;
				}
				if((r-2) >= 0){
					arr[cnt] = *(imageptr - 2*cols);
					cnt++;
				}
				if((r-1) >= 0){
					arr[cnt] = *(imageptr - cols);
					cnt++;
				}
				if((r+1) < rows){
					arr[cnt] = *(imageptr + cols);
					cnt++;
				}
				if((r+2) < rows){
					arr[cnt] = *(imageptr + 2*cols);
					cnt++;
				}
				if((r+3) < rows){
					arr[cnt] = *(imageptr + 3*cols);
					cnt++;
				}
			}
         else{
				arr[0] = *imageptr;
				arr[1] = *(imageptr - 3);
				arr[2] = *(imageptr - 2);
				arr[3] = *(imageptr - 1);
				arr[4] = *(imageptr + 1);
				arr[5] = *(imageptr + 2);
				arr[6] = *(imageptr + 3);
				arr[7] = *(imageptr - 3*cols);
				arr[8] = *(imageptr - 2*cols);
				arr[9] = *(imageptr - cols);
				arr[10] = *(imageptr + cols);
				arr[11] = *(imageptr + 2*cols);
				arr[12] = *(imageptr + 3*cols);
				cnt = 13;
         }

         *imagemedianptr = median(arr, cnt);

      }
   }
}

void rollball(unsigned short int *image, int rows, int cols, int radius,
	float **imagelow, float **imagehigh)
{
	int pr, pc, prr, pcc, p;
	double radius_sq, a, b, c, x, y, z, x0, y0, det, sqrtdet;
	float tl, th;
	unsigned short int *imageptr = NULL;
	float *imagelowptr = NULL, *imagehighptr = NULL;

   imagelowptr = (float *) calloc(rows*cols, sizeof(float));
	*imagelow = imagelowptr;
	imagehighptr = (float *) calloc(rows*cols, sizeof(float));
   *imagehigh = imagehighptr;

	for(pr=0,p=0;pr<rows;pr++){
		for(pc=0;pc<cols;pc++,p++){
			imagelowptr[p] = (float)(image[p]);
			imagehighptr[p] = (float)(image[p]);
		}
	}

   radius_sq = (double)radius * (double)radius;

   a = 1.0;

	imageptr = image;
	for(pr=0;pr<rows;pr++){
		printf("%d ", pr); fflush(stdout);
		y = (double)pr;
		for(pc=0;pc<cols;pc++,imageptr++){
			x = (double)pc;
			z = (double)(*imageptr);
			for(prr=(pr-radius);prr<=(pr+radius);prr++){
				if((prr >= 0) && (prr < rows)){
					y0 = (double)prr;
					for(pcc=(pc-radius);pcc<=(pc+radius);pcc++){
						if((pcc >= 0) && (pcc < cols)){
							x0 = (double)pcc;
							b = (double)(-2.0 * z);
							c = (x0-x)*(x0-x) + (y0-y)*(y0-y) + z*z - radius_sq;
							det = (b * b) - (4.0 * a * c);
							if(det >= 0.0){
								if(det > 0.0) sqrtdet = sqrt(det);
								else sqrtdet = 0.0;
								tl = (float)(((-1.0 * b) - sqrtdet) / 2.0);
								th = (float)(((-1.0 * b) + sqrtdet) / 2.0);
								if(imagelowptr[prr*cols+pcc] > tl) imagelowptr[prr*cols+pcc] = tl;
								if(imagehighptr[prr*cols+pcc] < th) imagehighptr[prr*cols+pcc] = th;
							}
						}
					}
				}
			}
		}
	}
}

void rollball_float(float *image, int rows, int cols, int radius,
	float **imagelow, float **imagehigh)
{
	int pr, pc, prr, pcc, p;
	double radius_sq, a, b, c, x, y, z, x0, y0, det, sqrtdet;
	float tl, th;
	float *imageptr = NULL;
	float *imagelowptr = NULL, *imagehighptr = NULL;

   imagelowptr = (float *) calloc(rows*cols, sizeof(float));
	*imagelow = imagelowptr;
	imagehighptr = (float *) calloc(rows*cols, sizeof(float));
   *imagehigh = imagehighptr;

	for(pr=0,p=0;pr<rows;pr++){
		for(pc=0;pc<cols;pc++,p++){
			imagelowptr[p] = image[p];
			imagehighptr[p] = image[p];
		}
	}

   radius_sq = (double)radius * (double)radius;

   a = 1.0;

	imageptr = image;
	for(pr=0;pr<rows;pr++){
		printf("%d ", pr); fflush(stdout);
		y = (double)pr;
		for(pc=0;pc<cols;pc++,imageptr++){
			x = (double)pc;
			z = (double)(*imageptr);
			for(prr=(pr-radius);prr<=(pr+radius);prr++){
				if((prr >= 0) && (prr < rows)){
					y0 = (double)prr;
					for(pcc=(pc-radius);pcc<=(pc+radius);pcc++){
						if((pcc >= 0) && (pcc < cols)){
							x0 = (double)pcc;
							b = (double)(-2.0 * z);
							c = (x0-x)*(x0-x) + (y0-y)*(y0-y) + z*z - radius_sq;
							det = (b * b) - (4.0 * a * c);
							if(det >= 0.0){
								if(det > 0.0) sqrtdet = sqrt(det);
								else sqrtdet = 0.0;
								tl = (float)(((-1.0 * b) - sqrtdet) / 2.0);
								th = (float)(((-1.0 * b) + sqrtdet) / 2.0);
								if(imagelowptr[prr*cols+pcc] > tl) imagelowptr[prr*cols+pcc] = tl;
								if(imagehighptr[prr*cols+pcc] < th) imagehighptr[prr*cols+pcc] = th;
							}
						}
					}
				}
			}
		}
	}
}