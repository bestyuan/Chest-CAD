//
//  (c) Copyright Eastman Kodak Company
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef UTIL_H
#define UTIL_H      


#include <stdlib.h>  
#include <stdio.h>
#include <string>
#include <limits>  
#include <iostream>  
#include <fstream>  
#include <iomanip> 
using namespace std;  
   

namespace CADX_SEG {


class Util 
{
	public:
	static const double PI;	
	static const long NODULE_LABEL;
	static const long SURROUND_LABEL;
	static const long BACKGROUND_LABEL;
	static const long EXCLUDE_LABEL;
	static const long GROUNDTRUTH_UNKNOWN;	   
	static const long GROUNDTRUTH_NEGATIVE;	 
	static const long GROUNDTRUTH_DEFINITE;	   
	static const long GROUNDTRUTH_PROBABLE;	
	static const long GROUNDTRUTH_POSSIBLE;	
	static const long TRUE_POSITIVE;
	static const long TRUE_NEGATIVE;
	static const long FALSE_POSITIVE;
	static const long FALSE_NEGATIVE;

	static const double RAD_TO_DEG; 
	static const double DEG_TO_RAD; 
	
	
	// Call when a fatal error occurs.
	static void fatalError(char *message);    

	// Call when a warning occurs.
	static void warning(char *message);    
	                                           
	// Removes directory portion from a path name.                                         
	static void stripDirectory(char* name);
     
	// Removes extension portion from a path name.
	static void stripExtension(char* name); 
	
	static char* getExtension(char* name);
	
	static double max(double a, double b) {if(a >= b) return a; else return b;}	  
	
	static double min(double a, double b) {if(a <= b) return a; else return b;}	
	
	static double abs(double a) {if(a <= 0) return -a; else return a;}
	
	static double square(double a) {return a * a;}
	
	static long round(double a) {if(a >= 0.0) return floor(a + 0.5); else return ceil(a - 0.5);}

	                             
	// Returns a textual description for a groundtruth label.                             
	static char* getGtDescription(long gt);
   
   static void copyFile(char* src, char* des);
	
};

} // end namespace




#endif
