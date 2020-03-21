//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef CULLER_H
#define CULLER_H


#include "Roi.h"
#include "UserAgent.h"


namespace CADX_SEG {


class Group {

	private:
	UserAgent userAgent;

	public:
	long index;
	long minX, maxX, minY, maxY;

	long nMembers;
	Roi *memberList[128];

	Group() {initialize();}

	~Group() {}

	void initialize();

	void addROI(Roi& roi);
   
	short belongsToGroup(Roi& roi);
   
	void setIndex(long i) {index = i;}
	
	long getIndex() {return index;}
	
	long getNMembers() {return nMembers;}
	
	Roi* getMember(long i) {return memberList[i];}
   
	static short sameAs(Roi& roi1, Roi& roi2);
	
	void setUserAgent(UserAgent& agent) {userAgent = agent;}

	// Returns a pointer to the Roi in the group with the highest probability.
	// If the goup has no members NULL is returned.
	Roi* getBestGroupMember();

};



class Culler {

	private:

	long nCandidates;
	long maxGroups;

	Roi *roiArray;

	long nGroups;
	Group* groupArray;
   
	UserAgent userAgent;

	public:
	
	Culler(Roi* roiArray, long nCandidates);

	~Culler();

	void assignGroups();  
	
	void setUserAgent(UserAgent& agent) {userAgent = agent;}

	// If candidates belong to the same group (boundaries overlap)
	// remove all but the candidate with highest probability.
	// A candidate is removed by setting its probability to a 
	// negative number.
	void removeDuplicates();

   
	protected:
   
	void initialize();
	




};

} // End namespace

#endif
