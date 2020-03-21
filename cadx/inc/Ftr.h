#ifndef FTR_H
#define FTR_H


#include "Util.h"
#include "Roi.h" 
 
#define MAX_CLASSES 16
#define MAX_FEATURES 256


class Record {

   private:

   long cnumber;
   char clabel[1024];
   char source[1024];    
   char comment[1024];
   double* fvalues;

   public:
   
   Record() {;}

   void allocate(long nfeatures) {
       fvalues = new double[nfeatures];
   }
   
   long getCNumber() {return cnumber;}
   void setCNumber(long v) {cnumber = v;}
   
   double getFValue(long k) {return fvalues[k];}
   void setFValue(double v, long k) {fvalues[k] = v;}
   
   char* getCLabel() {return clabel;}
   void setCLabel(char* v) {strcpy(clabel, v);}

   char* getSource() {return source;}
   void setSource(char* v) {strcpy(source, v);}
   
   char* getComment() {return comment;}
   void setComment(char* v) {strcpy(comment, v);}

};


class Ftr {   

	private:  
	char title[1024];
	long nclasses;
	char clabels[MAX_CLASSES][1024];
	long nsamples[MAX_CLASSES];
	double apriori[MAX_CLASSES];
	long nfeatures;
	char flabels[MAX_FEATURES][1024];
	char fileName[1024];
	
	Record currentRecord;
	
	long valid;

	fstream file;

	long nsamplesPos;

	public:  
	Ftr();
	
	Ftr(char* fileName);
	
	Ftr(char* fileName, char* _title, long _nclasses, char** _clabels,
	 long* _nsamples, double* _apriori, long nfeatures, char** flabels);

	// Destructor.
	~Ftr();     
	
	void writeHeader();

	void readHeader();

	void writeHeader(char* _title, long _nclasses, char** _clabels,
	 long* _nsamples, double* _apriori, long nfeatures, char** flabels);

	void updateHeader();

	void writeCandidate(Roi& roi, long classNum);
	
	void readNextSample();

	void write(ostream& s);
	void read(istream& s);  
	
	void openForReading();
	       
	void openForWriting();
	
	void reset() {file.seekg(0, ios_base::beg); file.clear();}

	short isValid() {return valid;}   

	void setValid(short v) {valid = v;}    
        
   long getNSamples(long i) {return nsamples[i];}

   long getSampleCNumber() {return currentRecord.getCNumber();}
   
   char* getSampleCLabel() {return currentRecord.getCLabel();}
   
   char* getSampleSource() {return currentRecord.getSource();}
   
   char* getSampleComment() {return currentRecord.getComment();}

   double getSampleFValue(long k) {return currentRecord.getFValue(k);}
   
   double getSampleFValue(char* name) {
                                  	
      for(long k = 0; k < nfeatures; k++) {
         if(strcmp(name, flabels[k]) == 0) return getSampleFValue(k);
      }
      
      return -1;
   }

   char* getFLabel(long k) {return flabels[k];}

	private:
	void initialize();


};





#endif
