//
//  (c) Copyright Eastman Kodak Company 
//  $Date: 2002/06/11 21:09:24 $
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//
//  RCS: $Id: jlabel.cpp,v 1.1.1.1 2002/06/11 21:09:24 L615076 Exp $
//


#include "RegionLabel.h"  
#include "Iem/IemImageIO.h" 


CADX_SEG::RegionLabel::RegionLabel() {

	bk = 0;	
	L = NULL;  	
}  


CADX_SEG::RegionLabel::~RegionLabel() {
	
	if(L != NULL) delete [] L;  	
}



IemTImage<unsigned char> CADX_SEG::RegionLabel::getLabeledImage() {
	
	IemTImage<unsigned char> imgLabeled(1, img.rows(), img.cols());
		
	for(long c = 0; c < img.cols(); c++) {
		for(long r = 0; r < img.rows(); r++) {    
					
			imgLabeled[0][r][c] = L[T[0][r][c]].getRank();
	}} 	 
	
	return imgLabeled;  
}





long CADX_SEG::RegionLabel::Label_Connected_Regions(IemTImage<unsigned char>& I, long MinNoPixels,
 long MaxNoRegions, long MaxNoLabels) { 
 	
	short p, l, n, e, w, NoPixels, NoLines, NE, N, NW, W, Cur, Assigned;
	long k, NoLabels = 1, V_n, V_ne, V_nw, V_w, MaxLabel = 0;

	NoPixels = I.cols(); 
	NoLines = I.rows();    
	
	img = I.copy();

 
	T = IemTImage<short>(1, NoLines, NoPixels);
	T = 0;

	L = new Label[MaxNoLabels];
	
	// Reserve the label 0 for background pixels.
  	for(k = 0; k < MaxNoLabels; k++) L[k].Initialize(k); 

   
	T[0][0][0] = L[NoLabels++].getValueInc();

	for(p = 1, w = 0; p < NoPixels; p++, w++) {   
		if(img[0][0][p] == img[0][0][w]) T[0][0][p] = L[T[0][0][w]].getValueInc();
		else T[0][0][p] =  L[NoLabels++].getValueInc();
		if(NoLabels == MaxNoLabels) return MaxNoLabels;
	}

	for(l = 1; l < NoLines; l++) {
		for(p = 0; p < NoPixels; p++) {    
     	
			NE = 0; N = 0; NW = 0; W = 0;
			V_n = 0; V_ne = 0; V_nw = 0; V_w = 0;
			Assigned = 0; 
			
			n = l - 1;
			w = p - 1;
			e = p + 1;

			Cur = img[0][l][p];

			// cout << "\n Location " << setw(6) << p << setw(6) << l << setw(8) << Cur;

			if(p > 0 && Cur == img[0][n][w]) {
				NW = 1; 
				if(!Assigned) {             
					T[0][l][p] = L[T[0][n][w]].getValueInc();
					Assigned = 1; 
					//  cout << ", Assigning label NW " << setw(9) << L[T.Val(w,n,1)];
				}
			}

			if(Cur == img[0][n][p]) {
				N = 1; 
				if(!Assigned) {
					T[0][l][p] = L[T[0][n][p]].getValueInc();
					Assigned = 1; 
					//  cout << ", Assigning label N " << setw(9) << L[T.Val(p,n,1)];
				}
			}

			if(p < NoPixels - 1 && Cur == img[0][n][e]) {            	
				NE = 1; 
				if(!Assigned) {  
					T[0][l][p] =  L[T[0][n][e]].getValueInc(); 
					Assigned = 1; 
						//   cout << ", Assigning label NE " << setw(9) << L[T.Val(e,n,1)];
				}
			}

			if(p > 0 && Cur == img[0][l][w]) {
				W = 1; 
				if(!Assigned) {
					T[0][l][p] = L[T[0][l][w]].getValueInc();
					Assigned = 1; 
					// cout << ", Assigning label W " << setw(9) << L[T.Val(w,l,1)];
				}
			}

			if(!Assigned) {
				// cout << ", Need new label " << setw(9) << NoLabels << flush;

				T[0][l][p] = L[NoLabels++].getValueInc();
				if(NoLabels == MaxNoLabels){delete L; return MaxNoLabels;}
			}

			if(NE&&NW) {
				// cout << ", NE same as NW";
				if(T[0][n][e] != T[0][n][w]) Label::Equate(L, T[0][n][e], T[0][n][w]);
			}
			if(NE&&W) {
				// cout << ", NE same as W";
				if(T[0][l][w] != T[0][n][e]) Label::Equate(L, T[0][l][w], T[0][n][e]);
			} 
 
	}}


	Label::Find_Smallest_Equivalent_Value(L, NoLabels);

	Label::Assign_Rank(L, NoLabels, MinNoPixels, MaxNoRegions);


//	Label::Label_Image(T, I, L);


	return NoLabels;
}




void CADX_SEG::Label::Equate(Label* L, long i0, long j0)
{
   long i, j, Root, i_next, j_next;

//   cout << "\n\n Equate " << endl;
  

   i = i0; j = j0;

//   cout << "\n Branch 1";

   Root = L[i].Value;

   do {  
//     cout << "\n  Label " << L[i];

     i = L[i].SameAs;

     Root = Util::min(Root, L[i].Value);
 
   }while(L[i].SameAs != L[i].Value);

//   cout << "\n Branch 2" << flush;

   Root = Util::min(Root, L[j].Value);

   do { 
//     cout << "\n  Label " << L[j];

     j = L[j].SameAs;

     Root = Util::min(Root, L[j].Value); 
 
   } while(L[j].SameAs != L[j].Value);

//   cout << "\n Root " << Root << endl;


   i = i0; j = j0;

   do { 
     i_next = L[i].SameAs;

     L[i].SameAs = Root;

     i = i_next;

   } while(L[i].SameAs != L[i].Value);

   L[i].SameAs = Root;

   do { 
     j_next = L[j].SameAs;

     L[j].SameAs = Root;

     j = j_next;
 
   }while(L[j].SameAs != L[j].Value);

   L[j].SameAs = Root;

}


void CADX_SEG::Label::Find_Smallest_Equivalent_Value(Label* L, long NoLabels)
{
	long k, j;

	for(k = 0; k < NoLabels; k++) {
		if(L[k].SameAs != L[k].Value) {
			j = L[k].SameAs;

			while(L[j].SameAs != L[j].Value) { 
				j = L[j].SameAs;    
			}
     
 		L[j].N += L[k].N;

		L[k].N = 0;

		L[k].SameAs = j;
	}}
}


void CADX_SEG::Label::Assign_Rank(Label *L, long& NoLabels, long MinNoPixels, long MaxNoRegions)
{
   long k, j, n = 1, LabelsUsed = 0, InLut;

   for(k = 1; k < NoLabels; k++) {
     if(L[k].N >= MinNoPixels) LabelsUsed++;
   }

   Lut<long> lut(1, LabelsUsed, 2);

   for(k = 1; k < NoLabels; k++) {
     if(L[k].N >= MinNoPixels) {
       lut.Val(n, 1) = k;
       lut.Val(n++, 2) = L[k].N;
   }}

   lut.Sort(2);    // Lut.Write_File("lut_sorted.lut");

   for(k = 1; k < NoLabels; k++) {  
   	
     InLut = 0;

     for(j = 1; j < LabelsUsed; j++) {
       
       if(L[k].SameAs == lut.Val(j, 1)) {
         if(j < MaxNoRegions) L[k].Rank = j;
         else L[k].Rank = 0; 

         InLut = 1;
         break;
       }
      
     }

     if(!InLut) L[k].Rank = 0;
   }

}


void CADX_SEG::Label::write(ostream& Out) {

   Out << " Value " << setw(8) << Value <<  ", SameAs " 
    << setw(8) << SameAs << ", N " << setw(8) << N 
    << ", Rank " << setw(8) << Rank << flush;
}


