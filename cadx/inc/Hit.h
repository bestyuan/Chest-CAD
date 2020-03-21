#ifndef HIT_H
#define HIT_H



#include "Util.h"


namespace CADX_SEG {
 
 

class Hit {        

	private:  
	char xraySourceID[1024];
	long imageWidth;
	long imageHeight;
	long numNodules;

	long *noduleIndex;
	long *status;
	long *centroidRow;
	long *centroidCol;
	long *nOutlinePoints;
	long **outlineRow; 
	long **outlineCol;   
	
	short valid;
	
	bool *detected;


	public:  
	 
	// Default contructor.
	Hit();
	    
	Hit(const char *hitFileName);

	// Destructor.
	~Hit();     
	
	void open(const char *hitFileName);
	
	long getGroundtruth(long col, long row); 
	
	// index is set to the index of the entry in the hit file
	// that determined the groundtruth.
	long getGroundtruth(long col, long row, long& index);
	
	long getNumNodules() {return numNodules;}      
	long getNoduleStatus(long i) {return status[i];}  
	long getNoduleIndex(long i) {return noduleIndex[i];}  
	long getNoduleCol(long i) {return centroidCol[i];}     
	long getNoduleRow(long i) {return centroidRow[i];} 
	long getNOutlinePoints(long i) {return nOutlinePoints[i];}  
	long getOutlineRow(long i, long j) {return outlineRow[i][j];}  	
	long getOutlineCol(long i, long j) {return outlineCol[i][j];} 
	long* getOutlineRowArray(long i) {return outlineRow[i];}  	
	long* getOutlineColArray(long i) {return outlineCol[i];}  
	short isValid() {return valid;}
	long getNumWithGroundtruth(long gt);
	
	void setDetected(bool b, long i) {detected[i] = b;}
	bool isDetected(long i) {return detected[i];}

	bool isInsideOutline(long i, long col, long row);    	

	long getNoduleArea(long i);	


	// Returns the index of the nodule that contains
	// the specified pixel. If no nodule is found
	// -1 is returned.	                                            
	long getEnclosingNodule(long col, long row);         
	
	void write(ostream& s);
	void read(istream& s);  
	                          
            
	private:                     
                  
	void initialize();
	void allocateNodulesArrays(); 
	void allocateNoduleArrays(long i);  

	                                            




};



} // End namespace


#endif
