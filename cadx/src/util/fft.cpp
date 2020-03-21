#include "fft.h"
#include "util.h"



IemTImage<float> FFT::magFFT(IemTImage<float>& imgFFT) {

	IemTImage<float> imgFFTView(imgFFT.chans() / 2, imgFFT.rows(), imgFFT.cols());
	
	for(long ib = 0; ib < imgFFTView.chans(); ib++) {
		for(long l = 0; l < imgFFTView.rows(); l++) {
			for(long p = 0; p < imgFFTView.cols(); p++) {
			
				long imag_offset = imgFFTView.chans() + ib;
				
				float mag = Util::square(imgFFT[ib][l][p]) + Util::square(imgFFT[imag_offset][l][p]);
				mag = sqrt(mag);
				
				imgFFTView[ib][l][p] = mag;
	}}}

   	return imgFFTView;
}


IemTImage<float> FFT::calcFFT(IemTImage<short>& img) {

	// Assumes all planes are real.

	// Check if image dims are power of 2.

	IemTImage<float> imgFFT(2 * img.chans(), img.rows(), img.cols());

	long max_dim = Util::max(img.rows(), img.cols());

	double real[16384];
	double imag[16384];

	long l,p;

	for(long ib = 0 ; ib < img.chans(); ib++) {

		long imag_offset = img.chans() + ib;

		// Horizontal
		long r = 1, c = 1;

		for(l=0; l < img.rows(); l++) {
		
			for(p = 0; p < img.cols();p++) {
				real[p] = (r * c) * img[ib][l][p];
				c *= -1;
			}

			r *= -1;

			memset(imag, 0, sizeof(double) * max_dim);

			gfft1dc(real, imag, img.cols(), 1);

			for(p = 0; p < img.cols(); p++) {
				imgFFT[ib][l][p] = (float)real[p];
				imgFFT[imag_offset][l][p] = (float)imag[p];
			}

		}


		// Vertical
		for(p = 0 ; p < img.cols(); p++) {

			for(l = 0; l < img.rows(); l++) {
				real[l] = imgFFT[ib][l][p];
				imag[l] = imgFFT[imag_offset][l][p];
			}

			gfft1dc(real, imag, img.rows(), 1);

			for(l = 0; l < img.rows(); l++) {
				imgFFT[ib][l][p] = (float)real[l];
				imgFFT[imag_offset][l][p] = (float)imag[l];
			}
		}
	}

	return imgFFT;
}


IemTImage<short> FFT::calcInverseFFT(IemTImage<float>& imgFFT) {

	// Assumes all planes are real.

	// Check if image dims are power of 2.

	IemTImage<short> img(imgFFT.chans() / 2, imgFFT.rows(), imgFFT.cols());
	IemTImage<float> imgBuf(2, imgFFT.rows(), imgFFT.cols());

	double real[16384];
	double imag[16384];

	long l,p;

	for(long ib = 0 ; ib < img.chans(); ib++) {

		long imag_offset = img.chans() + ib;
		
		// Horizontal


		for(l = 0; l < img.rows(); l++) {
		
			for(p = 0; p < img.cols(); p++) {
				real[p] = imgFFT[ib][l][p];
				imag[p] = imgFFT[imag_offset][l][p];
			}

			gfft1dc(real, imag, img.cols(), -1);

			for(p = 0; p < img.cols(); p++) {
				imgBuf[0][l][p] = (float)real[p];
				imgBuf[1][l][p] = (float)imag[p];
			}
		}
		
		// Vertical
		long r = 1, c = 1;
				
		for(p = 0; p < img.cols(); p++) {

			for(l = 0; l < img.rows(); l++) {
				real[l] = imgBuf[0][l][p];
				imag[l] = imgBuf[1][l][p];
			}

			gfft1dc(real, imag, img.rows(), -1);

			for(l = 0; l < img.rows(); l++) {
				img[ib][l][p] = r * c * (short)real[l];
				c *= -1;
			}
			r *= -1;
		}


	}

	return img;
}


IemTImage<float> FFT::filterFFT(IemTImage<float>& imgFFT) {

	IemTImage<float> imgFFTFiltered(imgFFT.chans(), imgFFT.rows(), imgFFT.cols());
	
	IemTImage<float> imgMask(1, imgFFT.rows(), imgFFT.cols());
	imgMask = 1.0;


	double fxc = imgFFT.cols() / 2;
	double fyc = imgFFT.rows() / 2;


	for(long ib = 0 ; ib < imgFFT.chans() / 2; ib++) {
		for(long l = 0; l < imgFFT.rows(); l++) {
			for(long p = 0; p < imgFFT.cols(); p++) {

				double fx = (p - fxc);
				double fy = (l - fyc);

				double f = sqrt(fx * fx + fy * fy);
			
			//	if(f == 4.0) imgMask[0][l][p] = 0;

				if(p == 257 && l == 253) imgMask[0][l][p] = 0;
				if(p == 255 && l == 259) imgMask[0][l][p] = 0;

	}}}




	for(ib = 0 ; ib < imgFFT.chans() / 2; ib++) {
		for(long l = 0; l < imgFFT.rows(); l++) {
			for(long p = 0; p < imgFFT.cols(); p++) {
			
				long imag_offset = imgFFT.chans() / 2 + ib;

				imgFFTFiltered[ib][l][p] = imgMask[0][l][p] * imgFFT[ib][l][p];
				imgFFTFiltered[imag_offset][l][p] = imgMask[0][l][p] * imgFFT[imag_offset][l][p];
	}}}

	return imgFFTFiltered;
}

