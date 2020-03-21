//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//




#ifndef REGIONLABEL_H
#define REGIONLABEL_H



#include "Iem/Iem.h" 
#include "Lut.h"
#include "Util.h"   

namespace CADX_SEG {

class Label;


class RegionLabel
{          
	private:	 
	
	IemTImage<unsigned char> img;	
	
	IemTImage<short> T;	  
	
	Label* L; 
	
	long bk;   	// Background code value that is mapped to 
			// 0 in labeled image.
	
	public:
	
	RegionLabel(); 
	~RegionLabel();
	
	      
	// 8-Connected regions of the same code value are assigned labels.
   	long Label_Connected_Regions(IemTImage<unsigned char>& I, long MinNoPixels, 
    	 long MaxNoRegions, long MaxNoLabels);	
   		
	IemTImage<unsigned char> getLabeledImage();
	
	
};




class Label
{
	private:
	long Value;		// Value of label.
	long SameAs;     // Value of an equivalent label.
	long N;		// Number of pixels with this label.
	long Rank;		// Rank of label.

	public:
	// Constructors.
	Label(){Initialize();}

	// Returns value of members.
	long Get_Value(){return Value;}
 	long Get_Equivalent(){return SameAs;}

	// Initialize;
	void Initialize(long k = 0) {
 		Value = k;
		SameAs = k;
		N = 0;
		Rank = 0;
 	}


// The i0 and j0 label and all equivalent labels
// have SameAs set to the value of the equivalent 
// label with the lowest value.
   static void Equate(Label* L, long i0, long j0);

// Set SameAs for all labels to the value of the equivalent
// label with the lowest value.
   static void Find_Smallest_Equivalent_Value(Label* L, long NoLabels);

// The labels are ranked according to the number of pixels that
// have the label (or an equivalent label). The label with the most
// pixels has Rank = 1. If the label has less than MinNoPixels
// Rank = 0. Only the largest MaxNoRegions labels are ranked. 
// The rest have Rank = 0;
   static void Assign_Rank(Label *L, long& NoLabels, long MinNoPixels,
    long MaxNoRegions);

// Assign a pixel to this label.
   void Assign(long *Ptr){*Ptr = Value; N++;}   
   
   long getValueInc() {N++; return Value;}   
   
   long getRank() {return Rank;}    

   void write(ostream& Out);
};

} // Namespace CADX


#endif 


