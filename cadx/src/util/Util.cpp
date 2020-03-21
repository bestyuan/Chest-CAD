//
//  (c) Copyright Eastman Kodak Company
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#include "Util.h"

using namespace CADX_SEG;


const double Util::PI = 3.14159265358979323846;  
const double Util::RAD_TO_DEG = 180.0 / PI;
const double Util::DEG_TO_RAD = PI / 180.0;
  
const long Util::NODULE_LABEL = 255;
const long Util::SURROUND_LABEL = 128;
const long Util::BACKGROUND_LABEL = 0;
const long Util::EXCLUDE_LABEL = 64;

const long Util::GROUNDTRUTH_UNKNOWN = 0;
const long Util::GROUNDTRUTH_NEGATIVE = 1;	  	
const long Util::GROUNDTRUTH_POSSIBLE = 2;	 
const long Util::GROUNDTRUTH_PROBABLE = 3;
const long Util::GROUNDTRUTH_DEFINITE = 4;   

const long Util::TRUE_POSITIVE = 1;
const long Util::TRUE_NEGATIVE = 2;
const long Util::FALSE_POSITIVE = 3;
const long Util::FALSE_NEGATIVE = 4;
               
               
               

char* Util::getGtDescription(long gt) {
	if(gt == Util::GROUNDTRUTH_UNKNOWN) return "unknown";
	else if(gt == Util::GROUNDTRUTH_NEGATIVE) return "negative"; 
	else if(gt == Util::GROUNDTRUTH_POSSIBLE) return "possible"; 		
	else if(gt == Util::GROUNDTRUTH_PROBABLE) return "probable"; 	
	else if(gt == Util::GROUNDTRUTH_DEFINITE) return "definite"; 
	else return "";		
}	


void Util::fatalError(char *message) {   
	
	cerr << "\n" << message << endl; 
	exit(1);
}  


void Util::warning(char *message) {   
	
	cerr << "\n" << message << endl; 
	exit(1);
}  


void Util::stripDirectory(char* name) { 
	
	long n, k, start = 0;
	char buffer[1024];

	n = strlen(name); 

	for(k = n - 1; k >= 0; k--) {
		
		if(name[k] == '/' || name[k] == '\\') {start = k + 1; break;}  
	}

	n = n - start + 1;
   
	for (k = 0; k < n; k++) buffer[k] = name[k + start];

	strcpy(name, buffer); 
} 


void Util::stripExtension(char* name) { 
	
	long n, k;
                    
	n = strlen(name); 

	for(k = n - 1; k >= 0; k--) { 
		
		if(name[k] == '.') {name[k] = '\0'; break;}
	}
}


char* Util::getExtension(char* name) { 

	static char extension[1024];
	
	char *p = strrchr(name, '.');
	
	if(p == NULL) return "";
	
	strcpy(extension, p);
	
	return extension;
}


void Util::copyFile(char* src, char* des) {

   FILE *in, *out;
   char buffer[1024];
   int n;
   
   in = fopen(src, "r");
   out = fopen(des, "w");
   
   while((n = fread(buffer, 1, 1024, in)) > 0) fwrite(buffer, 1, n, out);

   fclose(in);
   fclose(out);
}


















