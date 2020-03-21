//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//



#include "Polynomial.h"

using namespace CADX_SEG;


Polynomial::Polynomial() {
	initialize();
}


Polynomial::Polynomial(long _order) {
	initialize();
	allocate(_order);
	setTermNames();
}


void Polynomial::create(long _order) {
	initialize();
	allocate(_order);
	setTermNames();
}


Polynomial& Polynomial::operator=(const Polynomial& p) {
	free();

	x = p.x;
	y = p.y;
	xc = p.xc;
	yc = p.yc;
	order = p.order;
	   
	allocate(order);

	for(long i = 0; i < nTerms; i++) {
		coef[i] = p.coef[i];
		term[i] = p.term[i];
		strcpy(termName[i], p.termName[i]);
	} 

	return *this;
}


Polynomial::~Polynomial() {
	free();
}


void Polynomial::allocate(long _order) {
	order = _order;
	nTerms = (order + 1) * (order + 2) / 2;

	coef = new double[nTerms];
	term = new double[nTerms];
	termName = new char*[nTerms];

	for(long i = 0; i < nTerms; i++) termName[i] = new char[256];
}


void Polynomial::initialize() {
	x = y = 0;
	xc = yc = 0;
	order = 0;
	nTerms = 0;
	coef = NULL;
	term = NULL;
	termName = NULL;
}


void Polynomial::free() {
	if(coef != NULL) delete[] coef;
	if(term != NULL) delete[] term;
	
	if(termName != NULL) {
		for(long i = 0; i < nTerms; i++) delete[] termName[i];
		delete[] termName;
	}
} 


double Polynomial::getValue() {
	double sum = 0.0;
	
	for(long i = 0; i < nTerms; i++) {
		sum += coef[i] * term[i];
	}

	return sum;
}


double Polynomial::getFirstDer() {
	double dx = getdX();
	double dy = getdY();

	return sqrt(dx * dx + dy * dy);
}
   

void Polynomial::write(ostream& s) {

	s << "\nPolynomial: order= " << order << ", xc= " << xc << ", yc= " << yc << endl;

	for(long i = 0; i < nTerms; i++) {
		s << getTermName(i) << "= " << coef[i] << " ";
	}

	s << flush;
}
 

void Polynomial::setTermNames() {

	if(order >= 1) {
		strcpy(termName[0], "constant");
 		strcpy(termName[1], "X");
 		strcpy(termName[2], "Y");
	}
	if(order >= 2) {
	 	strcpy(termName[3], "XY");
	 	strcpy(termName[4], "XX");
	 	strcpy(termName[5], "YY");
	}
	if(order >= 3) {
		strcpy(termName[6], "XXY");
 		strcpy(termName[7], "YYX");
 		strcpy(termName[8], "XXX");
 		strcpy(termName[9], "YYY");
	}
	if(order >= 4) {
 		strcpy(termName[10], "YYYX");
		strcpy(termName[11], "XXXY");
 		strcpy(termName[12], "YYXX");
 		strcpy(termName[13], "XXXX");
 		strcpy(termName[14], "YYYY");
	}

}


void Polynomial::setTerms() {

	double xx, yy, xxx, yyy;

	if(order >= 1) {
		term[0] = 1.0;
		term[1] = x;
		term[2] = y;
	}
	if(order >= 2) {
		xx = x * x;
		yy = y * y;

		term[3] = x * y;
		term[4] = xx;
		term[5] = yy;
	}
	if(order >= 3) {
		term[6] = xx * y;
		term[7] = yy * x;
		term[8] = xx * x;
		term[9] = yy * y;
	}
	if(order >= 4) {
		xxx = xx * x;
		yyy = yy * y;

		term[10] = yyy * x;
		term[11] = xxx * y;
		term[12] = yy * xx;
		term[13] = xx * xx;
		term[14] = yy * yy;
	}

}


double Polynomial::getdX() {

	double df = 0.0;

	if(order >= 1) {
		df += coef[1];
	}
	if(order >= 2) {
		df += coef[3] * term[2] + 2.0 * coef[4] * term[1];
	}
	if(order >= 3) {
		df += 2.0 * coef[6] * term[3] + coef[7] * term[5] + 3.0 * coef[8] * term[4];
	}
	if(order >= 4) {
		df += coef[10] * term[9] + 3.0 * coef[11] * term[6] + 2.0 * coef[12] * term[7]
	 	 + 4.0 * coef[13] * term[8];
	}

	return df;
}


double Polynomial::getdXX() {

	double df = 0.0;

	if(order >= 2) {
		df += 2 * coef[4];
	}
	if(order >= 3) {
		df += 2.0 * coef[6] * term[2] + 6.0 * coef[8] * term[1];
	}
	if(order >= 4) {
		df += 6.0 * coef[11] * term[3] + 2.0 * coef[12] * term[5]
		 + 12.0 * coef[13] * term[4];
	}

	return df;
}


double Polynomial::getdXY() {

	double df = 0.0;

	if(order >= 2) {
		df += coef[3];
	}
	if(order >= 3) {
		df += 2.0 * coef[6] * term[1] + 2.0 * coef[7] * term[2];
	}
	if(order >= 4) {
		df += 3.0 * coef[10] * term[5] + 3.0 * coef[11] * term[4]
		 + 4.0 * coef[12] * term[3];
	}

	return df;
}


double Polynomial::getdY() {

	double df = 0.0;

	if(order >= 1) {
		df += coef[2];
	}
	if(order >= 2) {
		df += coef[3] * term[1] + 2.0 * coef[5] * term[2];
	}
	if(order >= 3) {
		df += coef[6] * term[4] + 2.0 * coef[7] * term[3] + 3.0 * coef[9] * term[5];
	}
	if(order >= 4) {
		df += 3.0 * coef[10] * term[7] + coef[11] * term[8] + 2.0 * coef[12] * term[6]
	 	 + 4.0 * coef[14] * term[9];
	}

	return df;
}


double Polynomial::getdYY() {

	double df = 0.0;

	if(order >= 2) {
		df += 2.0 * coef[5];
	}
	if(order >= 3) {
		df += 2.0 * coef[7] * term[1] + 6.0 * coef[9] * term[2];
	}
	if(order >= 4) {
		df += 6.0 * coef[10] * term[3] + 2.0 * coef[12] * term[4] + 12.0 * coef[14] * term[5];
	}

	return df;
}





 






 
 
 
 
 

 

