//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//

#ifndef FFT_H
#define FFT_H


#include "Iem/Iem.h" 


void gfft1dc (   
	double	*re,	// In/Out - real data
	double	*im,	// In/Out - imaginary data
	long	n,		// In - number of samples 
	short	dir		// In - 1 = forward, -1 = inverse 
);


class FFT {

	public:

	// Returns an image that can be used to view the mag. of the FFT.
	static IemTImage<float> magFFT(IemTImage<float>& imgFFT);

	// Calculates the fast Fourier transform of an image. 
	// The real (imag) part is placed in band 0 (1) of the returned image.
	static IemTImage<float> calcFFT(IemTImage<short>& img);
	
	static IemTImage<float> filterFFT(IemTImage<float>& imgFFT);

	// Calculates the inverse fast Fourier transform of an image. 
	static IemTImage<short> calcInverseFFT(IemTImage<float>& imgFFT);

};



#endif
