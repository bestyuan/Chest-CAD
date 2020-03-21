//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#include "Kespr.h" 
#include "Util.h"
  
using namespace CADX_SEG;


//const long Kespr::HEIGHT_OFFSET = 608;
//const long Kespr::WIDTH_OFFSET = 624;  
//const long Kespr::SPACING_OFFSET = 640; 
//const long Kespr::HEADER_SIZE = 2048; 

const long Kespr::HEIGHT_OFFSET = 608;
const long Kespr::WIDTH_OFFSET = 624;  
const long Kespr::SPACING_OFFSET = 640; 
const long Kespr::HEADER_SIZE = 0; 
     
     
     
Kespr::Kespr() { 
	
	height = width = 0;
	bands = 1;
	xSpacing = ySpacing = 0;
}


IemTImage<short> Kespr::Read(const char* inFileName) {

	char s[1024];

	FILE *fp = fopen(inFileName, "rb"); 
		
	// Read image height.	
//	fseek(fp, Kespr::HEIGHT_OFFSET, SEEK_SET);
//	if(fgets(s, 16, fp) == NULL) Util::fatalError("Kespr::Read()- height not read."); 
//	height = atoi(s);  
	height=500;
	
	// Read image width.	
//	fseek(fp, Kespr::WIDTH_OFFSET, SEEK_SET);
//	if(fgets(s, 16, fp) == NULL) Util::fatalError("Kespr::Read()- width not read."); 
//	width = atoi(s);
	width=500;
	
	// Read image spacing.	
//	fseek(fp, Kespr::SPACING_OFFSET, SEEK_SET);
//	if(fgets(s, 16, fp) == NULL) Util::fatalError("Kespr::Read()- spacing not read.");      
	
//	xSpacing = atof(strtok(s, "\\"));  
// 	ySpacing = atof(strtok(NULL, "\\"));
    xSpacing=0.175;
	ySpacing=0.175;
	
		
//	cout << "Image height, width, spacing: " << height << "\t" << width
//	  << "\t" << xSpacing << "\t" << ySpacing << endl;
	       
	       
	// Read image pixels into IEM image.  
	long rowBytes = sizeof(short) * width;      
	
	unsigned char *buffer = new unsigned char[rowBytes];    
	if(buffer == NULL) Util::fatalError("Kespr::Read()- memory not allocated.");   
	
	IemTImage<short> img(bands, height, width);	 
	
	for(long r = 0; r < height; r++) {
		                                        
		fseek(fp, rowBytes * r + Kespr::HEADER_SIZE, SEEK_SET);
		fread(buffer, sizeof(unsigned char), rowBytes, fp);
		
		for(long c = 0, k = 0; c < width; c++, k++) { 
			
			long v1 = buffer[k];
			long v2 = buffer[++k];

			img[0][r][c] = (short)((v1 << 8) + v2);
//			img[0][r][c] = (1023 - img[0][r][c]);
//			img[0][r][c] = (img[0][r][c])*4;
//			img[0][r][c]=(short)((v1<<7)+(v2>>1));
//			img[0][r][c] = (short)((v1 << 10) + (v2<<2));
//			img[0][r][c] = (v1*256+v2);
//			short chen=img[0][r][c];
//			if (chen<0)
//				img[0][r][c]=0;
		}
	}
	
	delete[] buffer;
	fclose(fp);

	return img;
}

