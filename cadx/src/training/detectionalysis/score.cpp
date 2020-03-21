#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "misc.h"
#include "groundtruth.h"
#include "detection_io.h"
#include "score.h"

//===================================================================
// Copyright 2001, softSurfer (www.softsurfer.com)
// This code may be freely used and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from its use.
// Users of this code must verify correctness for their application.
// http://geometryalgorithms.com/Archive/algorithm_0103/algorithm_0103.htm#wn_PinPolygon()
//===================================================================
int isLeft(struct Point p0, struct Point p1, struct Point p2)
{
	return ( (p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y) );
}

//===================================================================
// Copyright 2001, softSurfer (www.softsurfer.com)
// This code may be freely used and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from its use.
// Users of this code must verify correctness for their application.
//===================================================================
int point_inside_chain(int x, int y, struct ChainPoint *chain)
{
	struct ChainPoint *chainpoint = NULL;
	struct Point previouspoint, p;
	int    wn = 0;    // the winding number counter

	int isLeft(struct Point p0, struct Point p1, struct Point p2);

	p.x = x;
	p.y = y;

	// loop through all edges of the polygon
	previouspoint = chain->point;
	chainpoint = chain->nextpoint;
	while(chainpoint != NULL){
		if(previouspoint.y <= p.y) {         // start y <= P.y
			if(chainpoint->point.y > p.y)      // an upward crossing
				if(isLeft(previouspoint, chainpoint->point, p) > 0)  // P left of edge
					++wn;            // have a valid up intersect
		}
		else{                       // start y > P.y (no test needed)
			if(chainpoint->point.y <= p.y)     // a downward crossing
				if(isLeft(previouspoint, chainpoint->point, p) < 0)  // P right of edge
					--wn;            // have a valid down intersect
		}
		previouspoint = chainpoint->point;
		chainpoint = chainpoint->nextpoint;
   }

   // Now do the last one that links the last point to the first one.
   if(previouspoint.y <= p.y) {         // start y <= P.y
		if(chain->point.y > p.y)      // an upward crossing
			if(isLeft(previouspoint, chain->point, p) > 0)  // P left of edge
				++wn;            // have a valid up intersect
	}
	else{                       // start y > P.y (no test needed)
		if(chain->point.y <= p.y)     // a downward crossing
			if(isLeft(previouspoint, chain->point, p) < 0)  // P right of edge
				--wn;            // have a valid down intersect
	}

	if(wn == 0) return 0;
	else return 1;
}

/*
void print_detscore(struct DetScore *detscore, struct GroundTruth **groundtrutharray,
	struct Detection **detectionarray)
{
	struct DetScore *detscoreptr = NULL;
	char *imagename = NULL;

	detscoreptr = detscore;
	while(detscoreptr != NULL){
		if(detscore->detindex_tp != NULL){
			imagename = get_nvp_value_for_name(detectionarray[detscore->detindex_tp->index]->nvp, "IMAGE");
			if(imagename == NULL) imagename = "N/A";
			printf("[%s] TP=%d FP=%d FN=%d WASH=%d\n", imagename, detscore->num_tp,
				detscore->num_fp, detscore->num_fn, detscore->num_wash);
		}
		else if(detscore->detindex_fp != NULL){
			imagename = get_nvp_value_for_name(detectionarray[detscore->detindex_fp->index]->nvp, "IMAGE");
			if(imagename == NULL) imagename = "N/A";
			printf("[%s] TP=%d FP=%d FN=%d WASH=%d\n", imagename, detscore->num_tp,
				detscore->num_fp, detscore->num_fn, detscore->num_wash);
		}
		else if(detscore->gtindex_tp != NULL){
			imagename = get_nvp_value_for_name(groundtrutharray[detscore->gtindex_tp->index]->nvp, "IMAGE");
			if(imagename == NULL) imagename = "N/A";
			printf("[%s] TP=%d FP=%d FN=%d WASH=%d\n", imagename, detscore->num_tp,
				detscore->num_fp, detscore->num_fn, detscore->num_wash);
		}
		else if(detscore->gtindex_fn != NULL){
			imagename = get_nvp_value_for_name(groundtrutharray[detscore->gtindex_fn->index]->nvp, "IMAGE");
			if(imagename == NULL) imagename = "N/A";
			printf("[%s] TP=%d FP=%d FN=%d WASH=%d\n", imagename, detscore->num_tp,
				detscore->num_fp, detscore->num_fn, detscore->num_wash);
		}
		detscoreptr = detscoreptr->nextdetscore;
	}
}
*/

struct DetScore *free_detscore(struct DetScore *detscore)
{
	if(detscore == NULL) return(NULL);
	if(detscore->nextdetscore != NULL) free_detscore(detscore->nextdetscore);
	detscore->detindex_tp = free_index(detscore->detindex_tp);
	detscore->detindex_fp = free_index(detscore->detindex_fp);
	detscore->detindex_wash = free_index(detscore->detindex_wash);
	detscore->gtindex_tp = free_index(detscore->gtindex_tp);
	detscore->gtindex_fn = free_index(detscore->gtindex_fn);
	detscore->gtindex_match = free_index(detscore->gtindex_match);
	detscore->detindex_match = free_index(detscore->detindex_match);
	free(detscore);
	return(detscore);
}

//=============================================================================
// Function: score_detection_groundtruth
// Purpose: This function performs a scoring of detections against groundtruth.
// The detections are used as point coordinates and the groundtruth is used
// as polygons of points. A point inside match criteria is used. Each
// groundtruth may not generate more than one TP. Detections are considered
// in the order in which they are provided in the list of detections. When
// a detection matches more than one truth it can generate up to one true
// positive for each truth it matches. Any detection that matches at least
// one already detected truth is considered a wash unless it also matches
// an (as yet) undetected truth. Lists of indices are provided for
// both TP and FN truths and for TP, FP and WASH detections. This allows
// an analysis of the results.
// Name: Michael Heath, Eastman Kodak Company
// Date: 6/19/2003
//=============================================================================
struct DetScore *score_detection_groundtruth(struct GroundTruth **groundtrutharray, struct IndexLink *gtindex,
	struct Detection **detectionarray, struct IndexLink *detindex)
{
	struct IndexLink *gtindexptr = NULL, *detindexptr = NULL,
		*gtindex_tp_ptr = NULL, *tmpindexptr = NULL;
	struct DetScore *detscore = NULL;
	int isinside = 0, numinside = 0, numtimestp = 0, num_truth = 0, num_detections = 0;

	num_truth = indexlistlength(gtindex);
	num_detections = indexlistlength(detindex);

	detscore = (struct DetScore *) calloc(1, sizeof(struct DetScore));

	detscore->num_tp = 0;
	detscore->num_fp = 0;
	detscore->num_fn = 0;
	detscore->num_wash = 0;
	detscore->detindex_tp = NULL;
	detscore->detindex_fp = NULL;
	detscore->detindex_wash = NULL;
	detscore->gtindex_tp = NULL;
	detscore->gtindex_fn = NULL;
	detscore->gtindex_match = NULL;
	detscore->detindex_match = NULL;
	detscore->nextdetscore = NULL;

	//-------------------------------------------------------------------------
	// With no detections and no truth, there is nothing to do.
	//-------------------------------------------------------------------------
	if((gtindex == NULL) && (detindex == NULL)) return(detscore);

	//-------------------------------------------------------------------------
	// With no truth (but with detections), every detection is a false positive.
	//-------------------------------------------------------------------------
	if(gtindex == NULL){
		detscore->detindex_fp = copy_index(detindex);
		detscore->num_fp = indexlistlength(detscore->detindex_fp);
		return(detscore);
	}

	//-------------------------------------------------------------------------
	// With no detections (but with truth), every truth is a false negative.
	//-------------------------------------------------------------------------
	if(detindex == NULL){
		detscore->gtindex_fn = copy_index(gtindex);
		detscore->num_fn = indexlistlength(detscore->gtindex_fn);
		return(detscore);
	}

	//-------------------------------------------------------------------------
	// With both detections and truth, figure out the scoring.
	//-------------------------------------------------------------------------
	detscore->gtindex_fn = copy_index(gtindex);
	detindexptr = detindex;
	while(detindexptr != NULL){
		gtindexptr = gtindex;
		numinside = 0;
		numtimestp = 0;
		while(gtindexptr != NULL){

			isinside = point_inside_chain(detectionarray[detindexptr->index]->point.x,
				detectionarray[detindexptr->index]->point.y, groundtrutharray[gtindexptr->index]->boundary);

			if(isinside){

				numinside++;

				//-------------------------------------------------------------
				// Note that this detection and this groundthruth matched. This
				// simply means that this detection point is inside the truth.
				// It does not necessarily mean it was a true positive.
				//-------------------------------------------------------------
				appendindex_value(&(detscore->gtindex_match), gtindexptr->index);
				appendindex_value(&(detscore->detindex_match), detindexptr->index);

				//-------------------------------------------------------------
				// Check to see if this truth is currently undetected which
				// can be determined by finding it in the list of false
				// negative detections.
				//-------------------------------------------------------------
				tmpindexptr = whereindex(detscore->gtindex_fn, gtindexptr->index);
				//printf("TMPINDEXPTR = %p\n", tmpindexptr);

				//-------------------------------------------------------------
				// The groundtruth was not previously detected so count this
				// detection as a true positive and move the truth from the
				// false negative to the true positive list.
				//-------------------------------------------------------------
				if(tmpindexptr != NULL){

					//---------------------------------------------------------
					// Remove the truth from the list of false negatives.
					// Place the truth in the list of true positives.
					//---------------------------------------------------------
					tmpindexptr = removeindex(&(detscore->gtindex_fn), gtindexptr->index);

					//---------------------------------------------------------
					// Place the detection in the list of true positives if
					// it is not already there.
					//---------------------------------------------------------
					appendindex_value(&(detscore->gtindex_tp), tmpindexptr->index);
					free(tmpindexptr);

					appendindex_value(&(detscore->detindex_tp), detindexptr->index);

					numtimestp++;
				}
			}

			gtindexptr = gtindexptr->nextindex;
		}
		if(numinside == 0){ // We have a false positive detection.

			//-----------------------------------------------------------------
			// Place the detection in the list of false positives.
			//-----------------------------------------------------------------
			appendindex_value(&(detscore->detindex_fp), detindexptr->index);
		}
		else{
			if(numtimestp == 0){ // We have a wash.

				//-------------------------------------------------------------
				// Place the detection in the list of washes.
				// printf("Found a Wash\n"); fflush(stdout);
				//-------------------------------------------------------------
				appendindex_value(&(detscore->detindex_wash), detindexptr->index);
			}
		}
		detindexptr = detindexptr->nextindex;
	}

	//-------------------------------------------------------------------------
	// Count the number of true positives.
	//-------------------------------------------------------------------------
	detscore->num_tp = indexlistlength(detscore->gtindex_tp);

	//-------------------------------------------------------------------------
	// Cound the number of false positives.
	//-------------------------------------------------------------------------
	detscore->num_fp = indexlistlength(detscore->detindex_fp);

	//-------------------------------------------------------------------------
	// Count the number of false negatives.
	//-------------------------------------------------------------------------
	detscore->num_fn = indexlistlength(detscore->gtindex_fn);

	//-------------------------------------------------------------------------
	// Count the number of washes.
	//-------------------------------------------------------------------------
	detscore->num_wash = indexlistlength(detscore->detindex_wash);

	if(((detscore->num_tp + detscore->num_fn) != num_truth) ||
		((detscore->num_tp + detscore->num_fp + detscore->num_wash) != num_detections)){
		printf("Error!\n");
		exit(1);
	}

	return(detscore);
}