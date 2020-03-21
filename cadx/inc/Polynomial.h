//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H



#include "Util.h"



namespace CADX_SEG {


class Polynomial {

	protected:
	double x;
	double y;
	
	double xc;
	double yc;

	long order;
	long nTerms;
	long maxOrder;

	double* coef;
	
	double* term;
	char** termName;

	public:
	Polynomial();
	Polynomial(long _order);
	
	Polynomial(const Polynomial& p) {
		*this = p;
	}
	
	~Polynomial();

	void setCoef(long i, double _coef) {coef[i] = _coef;}
	
	void setCoef(double* _coef) {for(long i = 0; i < nTerms; i++) coef[i] = _coef[i];}
	
	void setPosition(double _x, double _y) {x = _x - xc; y = _y - yc; setTerms();}
	
	void setCenter(double _xc, double _yc) {xc = _xc; yc = _yc;}

	double* getTerms() {return term;}

	void setTerms();
	
	double getCoef(long i) {return coef[i];}
	
	double getdX();
	double getdY();
	
	double getdXY();
	double getdYX() {return getdXY();}

	double getdXX();
	double getdYY();

	double getFirstDer();

	char* getTermName(long i) {return termName[i];}

	long getNTerms() {return nTerms;}

	long getOrder() {return order;}

	double getTerm(long i) {return term[i];}

//	double getMonomial(long i, long x, long y) {return coef[i] * getTerm(i, x, y);}
	
	double getValue();

	double getValue(double x, double y) {setPosition(x, y); return getValue();}
	
	void create(long order);
	
	void write(ostream& s);

	Polynomial& operator=(const Polynomial& p);

	private:
	
	void free();
	
	void initialize();
	
	void setTermNames();
	
	void allocate(long order);
};



} // Namespace CADX_SEG





#endif

