// Afum : Perform Average Fraction Under the Minimum (AFUM) filtering
//
// This class implements M. Heath's AFUM filter. Please see Afum.cpp for 
// further information on Afum.
//
// ported to C++ by crandall, 5/2003
//
// original comments on Mike's code:
// /*******************************************************************************
// * File: afumfunction.c
// * Purpose: This file contains code to locate candidate mass regions in a
// * mammogram using Average Fraction Under the Minimum (AFUM) filtering. This
// * filter was developed by Michael Heath in the Computer Vision Lab and the
// * University of South Florida. This source code file contains just the
// * function for computing the AFUM feature image. It was extracted from the
// * afumfeature.c source code file from Mike Heaths Appendix B CDROM in his
// * dissertation.
// * Name: Michael Heath, Eastman Kodak Company
// * Date: 10/18/02
// * Note: Originally, the code contained the following Copyright information.
// * Copyright: Michael Heath and Dr. Kevin Bowyer 2000
// *******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "Iem.h"
#include "Afum.h"
#include <vector>
#include <algorithm>

#ifdef WIN32
// This hideously ugly hack is required because Microsoft Visual C++ 6.0
// does not properly implement the ANSI C++ standard regarding variables
// declared within 'for' statements.
#define for if(0) ; else for
#endif

void Afum::cleanup()
{
    if(num_at_radius)
        delete[] num_at_radius;
    
    if(num_below_min)
        delete[] num_below_min;
    
    if(min_in_disk)
        delete[] min_in_disk;
}

void Afum::setup_filter(int radius)
{
    cleanup();

    //printf("in setup_filter\n");

    num_at_radius = new int[radius+1];
    num_below_min = new int[radius+1];
    min_in_disk = new unsigned short[radius + 1];

    for(int i=0; i<radius+1; i++)
        num_at_radius[i] = num_below_min[i] = min_in_disk[i] = 0;

      // Compute the coordinates and distance for points inside the filter.
    int pix_count = 0;
    for(int r=(-radius);r<=radius;r++)
        for(int c=(-radius);c<=radius;c++)
        {
            int d = (int) floor(sqrt((double)(r*r+c*c)));
            if(d <= radius)
            {
                afum_filter.push_back(AfumFilter(r, c, d));

                num_at_radius[d]++;
                pix_count++;
//                printf("%d %d %d %d\n", r, c, d, radius);
            }
        }

      // djc - sort was done here in original code, but stl takes care of this automatically now
      // (cuz sets are kept sorted during item insertion).
		std::sort(afum_filter.begin(), afum_filter.end());

		std::vector<AfumFilter>::iterator iter;
    //printf("%d %d\n", afum_filter.size(), pix_count);

    assert(afum_filter.size() == (unsigned int)pix_count);

//    printf("%d\n", afum_filter.size());
//    for(iter = afum_filter.begin(); iter != afum_filter.end(); ++iter)
//        printf("%d %d %d\n", iter->row, iter->col, iter->radius);
}



// Further modified 5/7/2003 by crandall
// Making changes to allow use with IEM
//
void Afum::process_pixel(const IemTPlane<short> & image_rev, const IemTPlane<byte> &mask,
                             int r, int c, const short *img_cp, const byte *mask_cp, byte *afum_cp, int delta_r)
{
	//printf("%d %d\n", r, c);
	
	int rows = image_rev.rows();
    int cols = image_rev.cols();

      // initialize
    for(int n=0; n<=radius; n++)
    {
        num_below_min[n] = 0;
        min_in_disk[n] = 65535;
    }
    
      // at radius 0, minimum is the current point!
    min_in_disk[0] = *img_cp;
    int lastr = -1;
    //printf("1\n");    
	//printf("2\n"); 
	std::vector<AfumFilter>::iterator af_cp;
//	    printf("2\n"); 
      // skip over first (radius = 0) entry
    af_cp = afum_filter.begin();
	  //  printf("3\n"); 
    af_cp++;
	  //  printf("4\n"); 
    for( ; af_cp != afum_filter.end() ; af_cp++)
    {
        //printf("%d \n", qqq++);
          // if, on the last iteration, we were looking at a different radius than
          // the current one, set min_in_disk of the *current* radius to the min_in_disk
          // of the *prior* radius (i.e. it's a recursive thing - the minimum value
          // in a disk of radius r is the minimum of the minimum of a disk of radius r-1
          // and the minimum of a ring with radius r.)
        if(lastr != af_cp->radius)
        {
            if(af_cp->radius != 0) 
                min_in_disk[af_cp->radius] = min_in_disk[af_cp->radius-1];
            lastr = af_cp->radius;
        }
        
        int rr = r + af_cp->row;
        int cc = c + af_cp->col;
        
        if(rr >= 0 && rr < rows && cc >= 0 && cc < cols)
        {
            const byte mask_val = mask[rr][cc];
            
              // make sure *that* point is within the mask, too.
            if(mask_val)
            {
                const short img_val = image_rev[rr][cc];
                
                int r2 = lastr;
                if(img_val < min_in_disk[r2]) 
                    min_in_disk[r2] = img_val;
                int r1 = r2 - delta_r;
                
                if((r1 >= 0) && (img_val < min_in_disk[r1]))
                    num_below_min[r2]++;
                
            }
        }
    }
    //printf("2\n");    
    double afum = 0;
    for(int n = delta_r; n <= radius; n++)
    {
          // djc changed this from an "if" + error message into an assert.
          // this condition should never happen.
        assert(num_below_min[n] <= num_at_radius[n]);
        
        afum += (double)num_below_min[n] / (double)num_at_radius[n];
        
    }
    //printf("3\n");    
      // djc asks: what do we do if > 1.0?
      // I think this is a sanity check. If it's > 1.0, something bad has happened.
      // so change it to an assert
      // if((afum/(double)((radius-delta_r) + 1)) < 1.0)
    assert((afum/(double)((radius-delta_r) + 1)) < 1.0);
    *afum_cp = (unsigned char)floor(255.0 * afum / (double)((radius-delta_r) + 1));

    return;
}

std::vector<double> Afum::apply_filter(const IemTPlane<short> & image, int delta_r, const IemTPlane<byte> &mask,
                                       const std::vector<Point> &points, double factor)
{
    std::vector<double> results;

    assert(image.rows() == mask.rows() && image.cols() == mask.cols());

      // Apply the filter to the image.

    IemTPlane<short> rev_image = image;

    std::vector<Point>::const_iterator peak_iter;
    for(peak_iter=points.begin(); peak_iter != points.end(); peak_iter++)
    {
        int r=int(peak_iter->first / factor);
        int c=int(peak_iter->second / factor);

        byte result;
        process_pixel(rev_image, mask, r, c, &(rev_image[r][c]), &(mask[r][c]),
                      &result, delta_r);

        results.push_back(double(result));
    }
    
    return results;
}

IemPlane Afum::apply_filter(const IemTPlane<short> & image, int delta_r, const IemTPlane<byte> &mask)
{
    int rows = image.rows();
    int cols = image.cols();

    IemTPlane<byte> afum_result(rows, cols);
    afum_result = 0;

    //printf("%d,%d\n%d,%d\n",image.rows(), image.cols(), mask.rows(), mask.cols());

    assert(image.rows() == mask.rows() && image.cols() == mask.cols());

      // Apply the filter to the image.

    IemTPlane<short> rev_image = 32767 - image;

    const byte *mask_cp = mask[0];
    const short *img_cp = rev_image[0];
    byte *afum_cp = afum_result[0];

    for(int r=0; r < rows; r++)
    {
        for(int c=0; c<cols; c++, mask_cp++, afum_cp++, img_cp++)
        {
              // if point is part of the lung, according to the lung field mask
            if(*mask_cp)
                process_pixel(rev_image, mask, r, c, img_cp, mask_cp, afum_cp, delta_r);

        }
    }

    //printf("afum filter done\n");

    return afum_result;
}


