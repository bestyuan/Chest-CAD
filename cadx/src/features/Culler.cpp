//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#include "Culler.h"
#include "Roi.h"
#include "Can.h"
#include "CadxParm.h"
#include "Util.h"


using namespace CADX_SEG;


Culler::Culler(Roi* _roiArray, long _nCandidates) {
	initialize();

	nCandidates = _nCandidates;
	roiArray = _roiArray;
}


void Culler::initialize() {
	nGroups = 0;
	maxGroups = 1024;
	groupArray = NULL;

	groupArray = new Group[maxGroups];
}


Culler::~Culler() {
	if(groupArray != NULL) delete[] groupArray;
}


void Culler::assignGroups() { 


	for(long i = 0; i < nCandidates; i++) {

		long addedToGroup = 0;

		// Determine if ROI belongs to an existing group.
		// g is set to the number of the group that the ROI should
		// be added to. If no group is found it is set to -1.
		for(long k = 0; k < nGroups; k++) {
			if(groupArray[k].belongsToGroup(roiArray[i])) {
				groupArray[k].addROI(roiArray[i]);
				roiArray[i].addToGroup(groupArray[k].getIndex());
				addedToGroup = 1;
				break;
			}
		}

		// If ROI does not belong to an existing group add it to a new group.
		if(!addedToGroup) {
			groupArray[nGroups].setIndex(nGroups);
			groupArray[nGroups].addROI(roiArray[i]);
			roiArray[i].addToGroup(groupArray[nGroups].getIndex());
			nGroups++;

			if(nGroups >= maxGroups) return;
		}
 	                                      	
	}


}


void Culler::removeDuplicates() {

	// Add all roi to a group.
	for(long i = 0; i < nCandidates; i++) {

		// The number of groups that the Roi belongs to.
		long nRoiGroups = roiArray[i].getNGroups();

		// Add the roi to each group that it belongs to.
		for(long k = 0; k < nRoiGroups; k++) {
			 long groupNum = roiArray[i].getGroup(k);
			 if(groupNum >= maxGroups) continue;
			 groupArray[groupNum].addROI(roiArray[i]);
		}
	}

	for(long j = 0; j < maxGroups; j++) {
	                                    	
	     long nMembers = groupArray[j].getNMembers();
	     if(nMembers == 0) continue;

		// Get the roi that belongs to the group with highest probability.
		Roi* pBestRoi = groupArray[j].getBestGroupMember();

		// The probability of all roi in the group are set to zero
		// except for the roi with highest probabity.
		for(long l = 0; l < nMembers; l++) {
			Roi* pRoi = groupArray[j].getMember(l);	
			if(pRoi != pBestRoi) pRoi->setProbability(0.0);
		}


	}




}
















void Group::initialize() {
	index = 0;
	maxX = maxY = LONG_MIN;
	nMembers = 0;
}


short Group::belongsToGroup(Roi& roi) {

	for(long k = 0; k < nMembers; k++) {
		if(sameAs(*memberList[k], roi)) return 1;
	}

	return 0;
}


void Group::addROI(Roi& roi) {

	if(nMembers >= 128) return;

	memberList[nMembers] = &roi;
	nMembers++;
   
//	roi.addToGroup(index);
}


short Group::sameAs(Roi& roi1, Roi& roi2) {

	if(roi1.getArea() == 0 || roi2.getArea() == 0) return 0;

	long minX1 = roi1.getAbsMinBoundaryCol();
	long minX2 = roi2.getAbsMinBoundaryCol();
	long maxX1 = roi1.getAbsMaxBoundaryCol();
	long maxX2 = roi2.getAbsMaxBoundaryCol();
	
	long minY1 = roi1.getAbsMinBoundaryRow();
	long minY2 = roi2.getAbsMinBoundaryRow();
	long maxY1 = roi1.getAbsMaxBoundaryRow();
	long maxY2 = roi2.getAbsMaxBoundaryRow();

	long overlapMinX = Util::max(minX1, minX2);
	long overlapMaxX = Util::min(maxX1, maxX2);
	long overlapMinY = Util::max(minY1, minY2);
	long overlapMaxY = Util::min(maxY1, maxY2);

	long w = overlapMaxX - overlapMinX;
	long h = overlapMaxY - overlapMinY;
   
	if(w <= 0 || h <= 0) return 0;

	long overlapArea = 0;

	for(long x = overlapMinX; x <= overlapMaxX; x++) {
		for(long y = overlapMinY; y <= overlapMaxY; y++) {
		   if(roi1.isInsideAbsBoundary(x, y) && roi2.isInsideAbsBoundary(x, y)) overlapArea++;
	}}

	double overlap1 = (double)overlapArea / (double)roi1.getArea();
	double overlap2 = (double)overlapArea / (double)roi2.getArea();
/*
	userAgent.getLogFile() << "\n\nOverlapping regions: "
	 << "\noverlapMinX= " <<  overlapMinX << "\noverlapMaxX= " <<  overlapMaxX
	 << "\noverlapMinY= " <<  overlapMinY << "\noverlapMaxY= " <<  overlapMaxY
	 << "\nindex= " << roi1.getIndex() << ", area= " << roi1.getArea()
	 << "\nindex= " << roi2.getIndex() << ", area= " << roi2.getArea()
	 << "\noverlapArea= " << overlapArea
	 << "\noverlap1= " << overlap1 << ", overlap2= " << overlap2;
*/   
	if(overlap1 > CadxParm::getInstance().getMaxOverlap()
	 && overlap2 > CadxParm::getInstance().getMaxOverlap()) return 1;

	return 0;
}


Roi* Group::getBestGroupMember() {

	if(nMembers == 0) return NULL;

	double highestProb = DBL_MIN;
	long	highestK = 0;

	for(long k = 0; k < nMembers; k++) {
		if(memberList[k]->getProbability() > highestProb) { 
			highestProb = memberList[k]->getProbability();
			highestK = k;
		}
	}

	return memberList[highestK]; 
}




















