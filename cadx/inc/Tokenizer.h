#ifndef TOKENIZER_H
#define TOKENIZER_H
          

#include <string>
#include <stdlib.h>    
#include <fstream>  
using namespace std;


namespace CADX_SEG {



class Tokenizer
{
	protected:

   	istream *pSrc;
   
	char Last[1024], Current[1024];
   
	long Count;    // The number of numbers or strings that where
                  	// returned by getNumber() and getString() functions.

	short eof;


	public:

	Tokenizer();

	Tokenizer(istream &s);

	short EndOfFile(){return pSrc->eof();}

	char* NextToken();
   
   	double getNumber();
   
   	char* getCurrent(){return Current;}
   
   	char* getLast(){return Last;}
     
   	char* getString();
   
  	long getCount(){return Count;}  
   
	private:
   
	void Initialize();
  
  	short Comment();     

};



} // End namespace

#endif
