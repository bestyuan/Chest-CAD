#include "Tokenizer.h"


using namespace CADX_SEG;


Tokenizer::Tokenizer() {
   Initialize();
}


Tokenizer::Tokenizer(istream &s) {
   pSrc = &s;

   Initialize();
}


void Tokenizer::Initialize() {
   eof = 0;
   Count = 0;

   strcpy(Last, "\0");
   strcpy(Current, "\0");
}


char* Tokenizer::NextToken() {
	strcpy(Last, Current);

	*pSrc >> Current;

	if(Comment()) NextToken();
	   
	//if(pSrc->eof() != 0){eof = 1; return "\0";}

	return Current;
}



double Tokenizer::getNumber() {
   Count++;	
 	
   return atof(NextToken()); 	
}

 
 
char* Tokenizer::getString() {
   Count++;	
 	
   return NextToken(); 	
}



short Tokenizer::Comment() {

	if(Current[0] == '#') {    
		
		char c;

		do {
			c = pSrc->get();  
		} while(c != '\n');
  	
		return 1; 
	}

	return 0;
}


