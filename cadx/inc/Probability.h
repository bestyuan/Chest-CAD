//
//  (c) Copyright Eastman Kodak Company
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef PROBABILITY_H
#define PROBABILITY_H



#include "Util.h"



namespace CADX_SEG {


class Probability {

	public:
	// The index of the object this the probability.
	long index;

	// The probability value.
	double value;


	
	public:
	Probability() {initialize();}

	Probability(long _index, double _value) {
		initialize();
		index = _index;
		value = _value;
	}
	
	Probability(const Probability& rhs) {
		*this = rhs;
	}

	Probability& operator = (const Probability& rhs) {
		index = rhs.index;
		value = rhs.value;
		return *this;
	} 
	

	bool operator < (const Probability& rhs) {
		if(value < rhs.value) return true;
		return false;
	}


	bool operator > (const Probability& rhs) {
		if(value > rhs.value) return true;
		return false;
	}


	bool operator == (const Probability& rhs) {
		if(value == rhs.value) return true;
		return false;
	} 
	

	void write(ostream& s) {
		s << "index= " << index << ", value= " << value;
	}
	
	friend ostream& operator << (ostream& s, Probability& rhs) {
		rhs.write(s);
		return s;
	}

  

	protected:

	void initialize() {
		index = 0;
		value = 0.0;
	}

};



} // Namespace CADX





#endif

