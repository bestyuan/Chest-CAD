////////////////////////////////////////////////////////////////////////////////
//
// Eastman Kodak Company CONFIDENTIAL - for internal use only
// 
// File Name: 
//	$Workfile: $
// 
// Author: 
//	$Author: $
// 
// History:
//	$History: $
// 
// Copyright 2002, Eastman Kodak Company, All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////
// 
// Description: 
//
// This file contains the C implementation of a generic Fast Fourier Transform
// which should be portable to any platform.
// 
////////////////////////////////////////////////////////////////////////////////

// includes
#include <assert.h>
#include <math.h>

// 
// This inline is used to quickly swap two double array elements.
//
static inline void
fswap (double &a, double &b)
{
	register double temp = a; 
	a = b; 
	b = temp;
}

//
// Name:	
//	gfft1dc
//
// Purpose:
//	This computes an in-place complex-to-complex FFT using a radix-2
//	Cooley-Tukey computational method.  The input and output values
//	are both in "norma" order.  A forward FFT is done if dir = 1, an
//	inverse FFT is done if dir = -1.  References in comments refer to
//	"Digital Signal Processing", A.V. Oppenheim and R.W. Shafer, 1975.
//
// Returns:	
//	NA
//
void 
gfft1dc (
	   
	double	*re,	// In/Out - real data
	double	*im,	// In/Out - imaginary data
	long	n,		// In - number of samples 
	short	dir		// In - 1 = forward, -1 = inverse 
)

{
	register long	i, j, l;	// loop indices

	// Get exponent as an integer - the .1 is used to insure that
	// we get the correct truncation.
	register long m = (long)(log ((double)n) / log (2.0) + 0.1);

	// Step 1. - The FFT code below requires input values in bit-reversed 
	// order; the first and last elements are always the same in bit-reversed 
	// vs. normal order [Oppenheim p. 316].
	register double *pr = re;
	register double *pi = im;
	for (i = 1; i < n-1; ++i)
	{
		// create bit-reverse x of i
		register int s = m - 1;
		register long x = 0;
		for (j = 0; j < m; ++j, s -= 2)
			x |= s > 0 ? ((i & (1 << j)) << s) : ((i & (1 << j)) >> -s);

		// swap contents at x and i
		++pr;
		++pi;
		if (i < x)
		{
			fswap (*pr, re[x]);
			fswap (*pi, im[x]);
		}
	}

	// Step 2. Radix-2 Cooley-Tukey FFT Computation on bit-reversed data
	// [similar to Oppenheim fig. 6.21].
	//
	// The loop below is *somewhat* optimized, but further speed 
	// improvements probably could be made.  It might also help to use
	// an olgorithm which takes AND produces normal order arrays.
	//
	// This code is adapted from algorithms found on the web page
	// "DFT, Discrete Fourier Transform", at the web site
	// http://astronomy.swin.edu.au/~pbourke/analysis/dft/
	//
	register double c1 = -1.0; 
	register double c2 = 0.0;
	register long l2 = 1;
	for (l = 0; l < m; ++l) 
	{
		register long l1 = l2;
		l2 <<= 1;
		register double u1 = 1.0; 
		register double u2 = 0.0;
		for (j = 0; j < l1; ++j) 
		{
			for (i = j; i < n; i += l2) 
			{
				register long i1 = i + l1;
				register double t1 = u1 * re[i1] - u2 * im[i1];
				register double t2 = u1 * im[i1] + u2 * re[i1];
				re[i1] = re[i] - t1; 
				im[i1] = im[i] - t2;
				re[i] += t1;
				im[i] += t2;
			}

			register double z =  u1 * c1 - u2 * c2;
			u2 = u1 * c2 + u2 * c1;
			u1 = z;
		}

		c2 = -dir * sqrt ((1.0 - c1) / 2.0);
		c1 = sqrt ((1.0 + c1) / 2.0);
	}

	// Step 2a. Scaling is needed for forward transform
	if (dir > 0) 
	{
		register int count = n;
		register double *pr = re;
		register double *pi = im;
		do
		{
			*pr++ /= n;
			*pi++ /= n;
		}
		while (--count > 0);
	}

	// Step 3. Results are currently in reverse normal order, except for 
	// first (DC) element
	register int count = n / 2;
	register double *pr2 = &re[n-1];
	register double *pi2 = &im[n-1];
	pr = re + 1;
	pi = im + 1;
	do
	{
		fswap (*pr++, *pr2--);
		fswap (*pi++, *pi2--);
	}
	while (--count > 0);
}

