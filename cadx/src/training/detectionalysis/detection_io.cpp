//=============================================================================
// File: detection_io.cpp
// Description: The code in this file implements functions for storing and
// manipulating detections. The idea is to store detections in a linked
// list and to keep track of name and value pairs for each detection. This
// scheme allows searches to identify subsets of detections that match
// certain criteria such as all of those that come from some image or all
// of those that have a probability (of detection) greater than or equal
// to some value. Programmers (users) can implement complex searches of
// criteria to get just the subset of detections they desire. Detections
// may actually be read from different file formats, but once they are stored
// using this scheme, other functions such as those that evaluate detection
// algorithm performance or those that plot detections on images can use
// detections in a common format.
// Name: Michael Heath, Eastman Kodak Company
// Date: 06/26/2003
// Copyright: Eastman Kodak Company
//=============================================================================
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "misc.h"
#include "detection_io.h"
#include "can_io.h"
#include "ftr_io.h"

//=============================================================================
// Function: print_detections
// Purpose: This function prints the detections that are stored in a linked
// list of detections. The detection number, the detction point coordinates
// and all name, value pairs are printed for each detection.
// Name: Michael Heath, Eastman Kodak Company
// Date: 06/26/2003
//=============================================================================
void print_detections(struct Detection *detections, FILE *fptr)
{
	struct Detection *detectionptr = NULL;
	struct NVP *nvp = NULL;
	int d = 0;

	detectionptr = detections;
	while(detectionptr != NULL){
		fprintf(fptr, "Det=%5d", d);
		fprintf(fptr, " (%d %d)", detectionptr->point.x, detectionptr->point.y);
		nvp = detectionptr->nvp;
		while(nvp != NULL){
			if(strcmp(nvp->type, "float") == 0) fprintf(fptr, " %s %f", nvp->name, nvp->value.floatvalue);
			if(strcmp(nvp->type, "int") == 0) fprintf(fptr, " %s %d", nvp->name, nvp->value.intvalue);
			if(strcmp(nvp->type, "string") == 0) fprintf(fptr, " %s %s", nvp->name, nvp->value.string_value);
			nvp = nvp->nextnvp;
		}
		fprintf(fptr, "\n");
		d++;
		detectionptr = detectionptr->nextdetection;
	}
}

//=============================================================================
// Function: print_detections
// Purpose: This function prints the detections that are stored in a linked
// list of detections. The detection number, the detction point coordinates
// and all name, value pairs are printed for each detection.
// Name: Michael Heath, Eastman Kodak Company
// Date: 06/26/2003
//=============================================================================
void print_detectionarray(struct Detection **detectionarray, struct IndexLink *index,
	FILE *fptr)
{
	struct IndexLink *indexptr = NULL;
	struct Detection *detectionptr = NULL;
	struct NVP *nvp = NULL;
	int d = 0;

	indexptr = index;
	while(indexptr != NULL){
		d = indexptr->index;
		fprintf(fptr, "Det=%5d", d);
		fprintf(fptr, " (%d %d)", detectionarray[d]->point.x, detectionarray[d]->point.y);
		nvp = detectionarray[d]->nvp;
		while(nvp != NULL){
			if(strcmp(nvp->type, "float") == 0) fprintf(fptr, " %s %f", nvp->name, nvp->value.floatvalue);
			if(strcmp(nvp->type, "int") == 0) fprintf(fptr, " %s %d", nvp->name, nvp->value.intvalue);
			if(strcmp(nvp->type, "string") == 0) fprintf(fptr, " %s %s", nvp->name, nvp->value.string_value);
			nvp = nvp->nextnvp;
		}
		fprintf(fptr, "\n");
		d++;
		indexptr = indexptr->nextindex;
	}
}

//=============================================================================
// Procedure: detections_to_detectionarray
// Purpose: This procedure creates an array of pointers to detections for
// detections that are stored in a linked list. This assigns indices to each
// detection which can be used to reference specific detections.
// Name: Michael Heath, Eastman Kodak Company
// Date: 06/26/2003
//=============================================================================
void detections_to_detectionarray(struct Detection *detections,
	struct Detection ***detectionarray, int *numdetections)
{
	struct Detection *detectionptr = NULL;
	int i = 0;

	if(detections == 0){
		*detectionarray = NULL;
		*numdetections = 0;
		return;
	}

	i = 0;
	detectionptr = detections;
	while(detectionptr != NULL){
		i++;
		detectionptr = detectionptr->nextdetection;
	}

	*detectionarray = (struct Detection **) calloc(i, sizeof(struct Detection *));
	*numdetections = i;	

	i = 0;
	detectionptr = detections;
	while(detectionptr != NULL){
		(*detectionarray)[i] = detectionptr;
		i++;
		detectionptr = detectionptr->nextdetection;
	}
}

//=============================================================================
// Function: where_detectionarray
// Purpose: This function locates detections that meet a specific name value
// pair criteria. Each detection referenced in the array of pointers to
// detections that matches the search criteria gets placed in a linked list of
// array indices. Note that this function is overloaded with different
// value types.
// Name: Michael Heath, Eastman Kodak Company
// Date: 06/26/2003
//=============================================================================
struct IndexLink *where_detectionarray(struct Detection **detectionarray, int numdetections,
	struct IndexLink *index, char *name, char *op, float value, int startwithall)
{
	struct IndexLink *localindex = NULL, *newlocalindex = NULL, *lastlocalindex = NULL, 
		*indexptr = NULL;
	int d = 0;

	if(startwithall){
		for(d=0;d<numdetections;d++){
			if(nvp_match(detectionarray[d]->nvp, name, op, value) == 1){
				newlocalindex = (struct IndexLink *) calloc(1, sizeof(struct IndexLink));
				if(localindex == NULL) localindex = newlocalindex;
				else lastlocalindex->nextindex = newlocalindex;
				newlocalindex->index = d;
				lastlocalindex = newlocalindex;
			}
		}
	}
	else{
		indexptr = index;
		while(indexptr != NULL){
			d = indexptr->index;
			if(nvp_match(detectionarray[d]->nvp, name, op, value) == 1){
				newlocalindex = (struct IndexLink *) calloc(1, sizeof(struct IndexLink));
				if(localindex == NULL) localindex = newlocalindex;
				else lastlocalindex->nextindex = newlocalindex;
				newlocalindex->index = d;
				lastlocalindex = newlocalindex;
			}
			indexptr = indexptr->nextindex;
		}
	}

	return(localindex);
}

//=============================================================================
// Function: where_detectionarray
// Purpose: This function locates detections that meet a specific name value
// pair criteria. Each detection referenced in the array of pointers to
// detections that matches the search criteria gets placed in a linked list of
// array indices. Note that this function is overloaded with different
// value types.
// Name: Michael Heath, Eastman Kodak Company
// Date: 06/26/2003
//=============================================================================
struct IndexLink *where_detectionarray(struct Detection **detectionarray, int numdetections,
	struct IndexLink *index, char *name, char *op, int value, int startwithall)
{
	struct IndexLink *localindex = NULL, *newlocalindex = NULL, *lastlocalindex = NULL, 
		*indexptr = NULL;
	int d = 0;

	if(startwithall){
		for(d=0;d<numdetections;d++){
			if(nvp_match(detectionarray[d]->nvp, name, op, value) == 1){
				newlocalindex = (struct IndexLink *) calloc(1, sizeof(struct IndexLink));
				if(localindex == NULL) localindex = newlocalindex;
				else lastlocalindex->nextindex = newlocalindex;
				newlocalindex->index = d;
				lastlocalindex = newlocalindex;
			}
		}
	}
	else{
		indexptr = index;
		while(indexptr != NULL){
			d = indexptr->index;
			if(nvp_match(detectionarray[d]->nvp, name, op, value) == 1){
				newlocalindex = (struct IndexLink *) calloc(1, sizeof(struct IndexLink));
				if(localindex == NULL) localindex = newlocalindex;
				else lastlocalindex->nextindex = newlocalindex;
				newlocalindex->index = d;
				lastlocalindex = newlocalindex;
			}
			indexptr = indexptr->nextindex;
		}
	}

	return(localindex);
}

//=============================================================================
// Function: where_detectionarray
// Purpose: This function locates detections that meet a specific name value
// pair criteria. Each detection referenced in the array of pointers to
// detections that matches the search criteria gets placed in a linked list of
// array indices. Note that this function is overloaded with different
// value types.
// Name: Michael Heath, Eastman Kodak Company
// Date: 06/26/2003
//=============================================================================
struct IndexLink *where_detectionarray(struct Detection **detectionarray, int numdetections,
	struct IndexLink *index, char *name, char *op, char *value, int startwithall)
{
	struct IndexLink *localindex = NULL, *newlocalindex = NULL, *lastlocalindex = NULL, 
		*indexptr = NULL;
	int d = 0;

	if(startwithall){
		for(d=0;d<numdetections;d++){
			if(nvp_match(detectionarray[d]->nvp, name, op, value) == 1){
				newlocalindex = (struct IndexLink *) calloc(1, sizeof(struct IndexLink));
				if(localindex == NULL) localindex = newlocalindex;
				else lastlocalindex->nextindex = newlocalindex;
				newlocalindex->index = d;
				lastlocalindex = newlocalindex;
			}
		}
	}
	else{
		indexptr = index;
		while(indexptr != NULL){
			d = indexptr->index;
			if(nvp_match(detectionarray[d]->nvp, name, op, value) == 1){
				newlocalindex = (struct IndexLink *) calloc(1, sizeof(struct IndexLink));
				if(localindex == NULL) localindex = newlocalindex;
				else lastlocalindex->nextindex = newlocalindex;
				newlocalindex->index = d;
				lastlocalindex = newlocalindex;
			}
			indexptr = indexptr->nextindex;
		}
	}

	return(localindex);
}

//=============================================================================
// Function: can_v1point0_candata_to_detections
// Purpose: This function creates a linked list of detections from detection
// data that came from .can files. Please note that at the present time, only
// the centroid of the candidates, the IMAGE filename and the CANDIDATE indices
// are stored in the detection linked list. The candidates do have feature
// vectors that could be stored as well.
// Name: Michael Heath, Eastman Kodak Company
// Date: 06/26/2003
//=============================================================================
struct Detection *can_v1point0_candata_to_detections(struct CanData *candata,
	int *numfeatures, char ***featurelabels, char ***featuredata, int *totalnumcandidates)
{
	struct Detection *detection = NULL, *newdetection = NULL, *lastdetection = NULL;
	struct CanData *candataptr = NULL;
	struct Candidate *candidateptr = NULL;
    int c = 0, d = 0, f = 0;
	int cnum = 0;

	*totalnumcandidates = 0;
	candataptr = candata;
	while(candataptr != NULL){
		*totalnumcandidates += candataptr->numcandidates;
		candataptr = candataptr->nextcandata;
	}
	*featuredata = (char **) calloc(*totalnumcandidates, sizeof(char *));
	candataptr = candata;
	cnum = 0;
	while(candataptr != NULL){
		for(c=0;c<candataptr->numcandidates;c++){
			(*featuredata)[cnum] = (char *) calloc(strlen(candataptr->candidatearray[c]->ftrvector_line)+1, sizeof(char));
			strcpy((*featuredata)[cnum], candataptr->candidatearray[c]->ftrvector_line);
			cnum++;
		}
		candataptr = candataptr->nextcandata;
	}

	cnum = 0;
	candataptr = candata;
	while(candataptr != NULL){
		for(c=0;c<candataptr->numcandidates;c++){
			newdetection = (struct Detection *) calloc(1, sizeof(struct Detection));
			if(d == 0){
				detection = newdetection;
				lastdetection = newdetection;
				*numfeatures = candataptr->numfeatures;
				*featurelabels = (char **) calloc(candataptr->numfeatures, sizeof(char *));
				for(f=0;f<candataptr->numfeatures;f++){
					(*featurelabels)[f] = (char *) calloc(strlen(candataptr->featurelabels[f])+1, sizeof(char));
					strcpy((*featurelabels)[f], candataptr->featurelabels[f]);
				}
			}
			else{
				lastdetection->nextdetection = newdetection;
				lastdetection = newdetection;
			}
			newdetection->point = candataptr->candidatearray[c]->centroid;
			newdetection->cols = candataptr->cols;
			newdetection->rows = candataptr->rows;
			newdetection->nvp = create_nvp(newdetection->nvp, "IMAGE", candataptr->sourceimage);
			newdetection->nvp = create_nvp(newdetection->nvp, "CANDIDATE", (int)(candataptr->candidatearray[c]->index)); //(int)c);
			newdetection->nvp = create_nvp(newdetection->nvp, "CANNUMBER", (int)c);
			f = 0;
			for(f=0;f<candataptr->numfeatures;f++){
				if(strcmp(candataptr->featurelabels[f], "coin") == 0){
					newdetection->nvp = create_nvp(newdetection->nvp, "COIN",
						(float)floor((candataptr->candidatearray[c]->ftrvector[f])));
					break;
				}
			}
			newdetection->nvp = create_nvp(newdetection->nvp, "FEATUREINDEX", (int)cnum);
			cnum++;
			d++;
		}
		candataptr = candataptr->nextcandata;
	}

	return(detection);
}

//=============================================================================
// Function: can_v1point1_candata_to_detections
// Purpose: This function creates a linked list of detections from detection
// data that came from .can files. Please note that at the present time, only
// the centroid of the candidates, the IMAGE filename, the CANDIDATE indices
// and the detection probabilities (quantized to the hundedths place)
// are stored in the detection linked list. The candidates do have feature
// vectors that could be stored as well.
// Name: Michael Heath, Eastman Kodak Company
// Date: 06/26/2003
//=============================================================================
struct Detection *can_v1point1_candata_to_detections(struct CanData *candata,
	int *numfeatures, char ***featurelabels, char ***featuredata, int *totalnumcandidates)
{
	struct Detection *detection = NULL, *newdetection = NULL, *lastdetection = NULL;
	struct CanData *candataptr = NULL;
	struct Candidate *candidateptr = NULL;
    int c = 0, d = 0, f = 0;
	int cnum = 0;

	*totalnumcandidates = 0;

	candataptr = candata;
	while(candataptr != NULL){
		*totalnumcandidates += candataptr->numcandidates;
		candataptr = candataptr->nextcandata;
	}
	*featuredata = (char **) calloc(*totalnumcandidates, sizeof(char *));
	candataptr = candata;
	cnum = 0;
	while(candataptr != NULL){
		for(c=0;c<candataptr->numcandidates;c++){
			(*featuredata)[cnum] = (char *) calloc(strlen(candataptr->candidatearray[c]->ftrvector_line)+1, sizeof(char));
			strcpy((*featuredata)[cnum], candataptr->candidatearray[c]->ftrvector_line);
			cnum++;
		}
		candataptr = candataptr->nextcandata;
	}

	cnum = 0;
	candataptr = candata;
	while(candataptr != NULL){
		for(c=0;c<candataptr->numcandidates;c++){
			newdetection = (struct Detection *) calloc(1, sizeof(struct Detection));
			if(d == 0){
				detection = newdetection;
				lastdetection = newdetection;
				*numfeatures = candataptr->numfeatures;
				*featurelabels = (char **) calloc(candataptr->numfeatures, sizeof(char *));
				for(f=0;f<candataptr->numfeatures;f++){
					(*featurelabels)[f] = (char *) calloc(strlen(candataptr->featurelabels[f])+1, sizeof(char));
					strcpy((*featurelabels)[f], candataptr->featurelabels[f]);
				}
			}
			else{
				lastdetection->nextdetection = newdetection;
				lastdetection = newdetection;
			}
			newdetection->point = candataptr->candidatearray[c]->centroid;
			newdetection->cols = candataptr->cols;
			newdetection->rows = candataptr->rows;
			newdetection->nvp = create_nvp(newdetection->nvp, "IMAGE", candataptr->sourceimage);
			newdetection->nvp = create_nvp(newdetection->nvp, "CANDIDATE", (int)(candataptr->candidatearray[c]->index)); //(int)c);
			newdetection->nvp = create_nvp(newdetection->nvp, "CANNUMBER", (int)c);
			newdetection->nvp = create_nvp(newdetection->nvp, "PROBABILITY",
				(float)(floor((100.0 * candataptr->candidatearray[c]->probability)) / 100.0));
			f = 0;
			for(f=0;f<candataptr->numfeatures;f++){
				if(strcmp(candataptr->featurelabels[f], "coin") == 0){
					newdetection->nvp = create_nvp(newdetection->nvp, "COIN",
						(float)floor((candataptr->candidatearray[c]->ftrvector[f])));
					break;
				}
			}
			newdetection->nvp = create_nvp(newdetection->nvp, "FEATUREINDEX", (int)cnum);
			cnum++;
			d++;
		}
		candataptr = candataptr->nextcandata;
	}

	return(detection);
}

//=============================================================================
// Function: can_v1point2_candata_to_detections
// Purpose: This function creates a linked list of detections from detection
// data that came from .can files. Please note that at the present time, only
// the centroid of the candidates, the IMAGE filename, the CANDIDATE indices
// and the detection probabilities (quantized to the hundedths place)
// are stored in the detection linked list. The candidates do have feature
// vectors that could be stored as well.
// Name: Michael Heath, Eastman Kodak Company
// Date: 06/26/2003
// Written on 01/30/2004 by modifying the can_v1point2_candata_to_detections
// code to process CAN V1.2 data.
//=============================================================================
struct Detection *can_v1point2_candata_to_detections(struct CanData *candata,
	int *numfeatures, char ***featurelabels, char ***featuredata, int *totalnumcandidates)
{
	struct Detection *detection = NULL, *newdetection = NULL, *lastdetection = NULL;
	struct CanData *candataptr = NULL;
	struct Candidate *candidateptr = NULL;
    int c = 0, d = 0, f = 0, q = 0, startpos = 0, incrementstartpos = 0;
	int cnum = 0;
	char groupstring[400] = {'\0'};

	*totalnumcandidates = 0;

	candataptr = candata;
	while(candataptr != NULL){
		*totalnumcandidates += candataptr->numcandidates;
		candataptr = candataptr->nextcandata;
	}
	*featuredata = (char **) calloc(*totalnumcandidates, sizeof(char *));
	candataptr = candata;
	cnum = 0;
	while(candataptr != NULL){
		for(c=0;c<candataptr->numcandidates;c++){
			(*featuredata)[cnum] = (char *) calloc(strlen(candataptr->candidatearray[c]->ftrvector_line)+1, sizeof(char));
			strcpy((*featuredata)[cnum], candataptr->candidatearray[c]->ftrvector_line);
			cnum++;
		}
		candataptr = candataptr->nextcandata;
	}

	cnum = 0;
	candataptr = candata;
	while(candataptr != NULL){
		for(c=0;c<candataptr->numcandidates;c++){
			newdetection = (struct Detection *) calloc(1, sizeof(struct Detection));
			if(d == 0){
				detection = newdetection;
				lastdetection = newdetection;
				*numfeatures = candataptr->numfeatures;
				*featurelabels = (char **) calloc(candataptr->numfeatures, sizeof(char *));
				for(f=0;f<candataptr->numfeatures;f++){
					(*featurelabels)[f] = (char *) calloc(strlen(candataptr->featurelabels[f])+1, sizeof(char));
					strcpy((*featurelabels)[f], candataptr->featurelabels[f]);
				}
			}
			else{
				lastdetection->nextdetection = newdetection;
				lastdetection = newdetection;
			}
			newdetection->point = candataptr->candidatearray[c]->centroid;
			newdetection->cols = candataptr->cols;
			newdetection->rows = candataptr->rows;
			newdetection->nvp = create_nvp(newdetection->nvp, "IMAGE", candataptr->sourceimage);
			newdetection->nvp = create_nvp(newdetection->nvp, "CANDIDATE", (int)(candataptr->candidatearray[c]->index)); //(int)c);
			newdetection->nvp = create_nvp(newdetection->nvp, "CANNUMBER", (int)c);
			newdetection->nvp = create_nvp(newdetection->nvp, "PROBABILITY",
				(float)(floor((100.0 * candataptr->candidatearray[c]->probability)) / 100.0));
			f = 0;
			for(f=0;f<candataptr->numfeatures;f++){
				if(strcmp(candataptr->featurelabels[f], "coin") == 0){
					newdetection->nvp = create_nvp(newdetection->nvp, "COIN",
						(float)floor((candataptr->candidatearray[c]->ftrvector[f])));
					break;
				}
			}
			newdetection->nvp = create_nvp(newdetection->nvp, "FEATUREINDEX", (int)cnum);

			// Place the groups information into the dataset.
			memset(groupstring, 400, '\0');
			startpos = 0;
			//printf("%d\n", candataptr->candidate->ngroups);
			for(q=0;q<candataptr->candidatearray[c]->ngroups;q++){
				if(q != 0){
					sprintf(groupstring+startpos, " %n", &incrementstartpos);
					startpos += incrementstartpos;
				}
				sprintf(groupstring+startpos, "%d%n", candataptr->candidatearray[c]->groups[q], &incrementstartpos);
				startpos += incrementstartpos;
			}
            newdetection->nvp = create_nvp(newdetection->nvp, "GROUPS", (char *)groupstring);

			cnum++;
			d++;
		}
		candataptr = candataptr->nextcandata;
	}

	return(detection);
}

//=============================================================================
// Function: free_detection
// Purpose: This function frees a linked list of detections. Please note that
// detection lists may be very long and this recursive function has and may
// overflow the stack. The free_detections function may be called instead and
// has some protection to limit the number of recursive calls of this function
// while still freeing all detections.
// Name: Michael Heath, Eastman Kodak Comapny
// Date: 06/26/2003
//=============================================================================
static struct Detection *free_detection(struct Detection *detection)
{
   if(detection == NULL) return(NULL);
   if(detection->nextdetection != NULL) free_detection(detection->nextdetection);
   free_nvp(detection->nvp);
   free(detection);
   return(NULL);
}

//=============================================================================
// Function: free_detections
// Purpose: This procedure is used to free a linked list of detections. It
// was written to overcome a run time error that was encountered when the
// function free_detection was called with too long on a linked list. The
// stack overflowed. This routine calls free_detection as many times as
// is necessary to free a linked list. Each call made to free_detection
// frees no more than 1000 links in the list.
// Name: Michael Heath, Eastman Kodak Company
// Date: 06/26/2003
//=============================================================================
struct Detection *free_detections(struct Detection *detection)
{
	struct Detection *detectionptr = NULL, **detectionptr_to_free = NULL;
	int n = 0;

	while(1){
		detectionptr = detection;
		detectionptr_to_free = &detection;
		n = 0;
		while(detectionptr != NULL){
			n++;
			if(n >= 1000) detectionptr_to_free = &((*detectionptr_to_free)->nextdetection);
			detectionptr = detectionptr->nextdetection;
		}
		if((*detectionptr_to_free) == detection){
			// Free all remaining detections in the linked list.
			*detectionptr_to_free = free_detection(*detectionptr_to_free);
			return(NULL);
		}
		else{
			// Free the last 1000 detections in the linked list.
			*detectionptr_to_free = free_detection(*detectionptr_to_free);
		}
	}
}

//=============================================================================
// Function: sorted_unique_values_for_name
// Purpose: Given an array of detections and the name (and type) of a name,
// value pair associated with detections, this function returns a sorted list
// of unique values for the name. These are returned in a linked list. This
// function can be used, for example, to find the list of all images referenced
// by the detections. Note that this function is overloaded with a similar
// function for grountruths.
// Name: Michael Heath, Eastman Kodak Company
// Date: 06/26/2003
//=============================================================================
struct VUnionLL *sorted_unique_values_for_name(struct Detection **detectionarray,
	int numdetections, char *name, char *type)
{
	struct VUnionLL *topvunion = NULL, *newvunion = NULL, *lastvunion = NULL, *vunionptr = NULL;
	union VUnion *cptr = NULL;
	char *tptr = NULL;
	int d = 0;

	for(d=0;d<numdetections;d++){

		cptr = get_nvp_value_for_name(detectionarray[d]->nvp, name);

		tptr = get_nvp_type_for_name(detectionarray[d]->nvp, name);

		if(cptr == NULL) continue;

		if(topvunion == NULL){
			topvunion = (struct VUnionLL *) calloc(1, sizeof(struct VUnionLL));
			if(strcmp(type, "float") == 0) topvunion->vunion.floatvalue = cptr->floatvalue;
			if(strcmp(type, "int") == 0) topvunion->vunion.intvalue = cptr->intvalue;
			if(strcmp(type, "string") == 0){
				topvunion->vunion.string_value = (char *) calloc(strlen(cptr->string_value)+1, sizeof(char));
				strcpy(topvunion->vunion.string_value, cptr->string_value);
			}
			continue;
		}

		vunionptr = topvunion;
		lastvunion = topvunion;

		if(strcmp(tptr, "string") == 0){
			while(vunionptr != NULL){
				if(strcmp(vunionptr->vunion.string_value, cptr->string_value) == 0) break;
				if(strcmp(vunionptr->vunion.string_value, cptr->string_value) > 0){
					newvunion = (struct VUnionLL *) calloc(1, sizeof(struct VUnionLL));
					newvunion->vunion.string_value = (char *) calloc(strlen(cptr->string_value)+1, sizeof(char));
					strcpy(newvunion->vunion.string_value, cptr->string_value);

					if(vunionptr == topvunion){
						newvunion->nextvunion = topvunion;
						topvunion = newvunion;
						break;
					}
					newvunion->nextvunion = vunionptr;
					lastvunion->nextvunion = newvunion;
					break;
				}
				lastvunion = vunionptr;
				vunionptr = vunionptr->nextvunion;
			}
			if(vunionptr == NULL){  // Got to the end... so add it here...
				lastvunion->nextvunion = (struct VUnionLL *) calloc(1, sizeof(struct VUnionLL));
				lastvunion->nextvunion->vunion.string_value = (char *) calloc(strlen(cptr->string_value)+1, sizeof(char));
				strcpy(lastvunion->nextvunion->vunion.string_value, cptr->string_value);
			}
		}

		if(strcmp(tptr, "int") == 0){
			while(vunionptr != NULL){
				if(vunionptr->vunion.intvalue == cptr->intvalue) break;
				if(vunionptr->vunion.intvalue > cptr->intvalue){
					newvunion = (struct VUnionLL *) calloc(1, sizeof(struct VUnionLL));
					newvunion->vunion.intvalue = cptr->intvalue;

					if(vunionptr == topvunion){
						newvunion->nextvunion = topvunion;
						topvunion = newvunion;
						break;
					}
					newvunion->nextvunion = vunionptr;
					lastvunion->nextvunion = newvunion;
					break;
				}
				lastvunion = vunionptr;
				vunionptr = vunionptr->nextvunion;
			}
			if(vunionptr == NULL){  // Got to the end... so add it here...
				lastvunion->nextvunion = (struct VUnionLL *) calloc(1, sizeof(struct VUnionLL));
				lastvunion->nextvunion->vunion.intvalue= cptr->intvalue;
			}
		}

		if(strcmp(tptr, "float") == 0){
			while(vunionptr != NULL){
				if(vunionptr->vunion.floatvalue == cptr->floatvalue) break;
				if(vunionptr->vunion.floatvalue > cptr->floatvalue){
					newvunion = (struct VUnionLL *) calloc(1, sizeof(struct VUnionLL));
					newvunion->vunion.floatvalue = cptr->floatvalue;

					if(vunionptr == topvunion){
						newvunion->nextvunion = topvunion;
						topvunion = newvunion;
						break;
					}
					newvunion->nextvunion = vunionptr;
					lastvunion->nextvunion = newvunion;
					break;
				}
				lastvunion = vunionptr;
				vunionptr = vunionptr->nextvunion;
			}
			if(vunionptr == NULL){  // Got to the end... so add it here...
				lastvunion->nextvunion = (struct VUnionLL *) calloc(1, sizeof(struct VUnionLL));
				lastvunion->nextvunion->vunion.floatvalue= cptr->floatvalue;
			}
		}
	}

//	stringptr = topstring;
//	while(stringptr != NULL){
//		printf("%s\n", stringptr->string);
//		stringptr = stringptr->nextstring;
//	}

	return(topvunion);
}

//=============================================================================
// Function: ftr_v1point1_ftrdata_to_detections
// Purpose: FTR files were created for the chest CAD project. They store the
// output from classification applied to a set of images with a particular
// APRIORI probability of detection. Amoung other information, each candidate
// in a set of .can files is listed along with its source image and candidate
// number (index) for that image. This function builds a linked list of
// detections that expands the detections from a set of can files (stored as
// detections in a detectionarray) to have a copy for each ftr file. The
// APRIORI values from the FTR file along with the classification labels are
// added as name value pairs to the new detections augmenting the previous
// name value pair linked list for each detection. Among other things, this
// recovers the detection coordinated for each detection that was classified
// by the classifier that produced the FTR files. Please note, this can
// create really a really long linked list of detections (e.g. 80 images x
// 50 candidates/image x 30 APRIORI values = 120,000 detections) each
// with a linked list of name value pairs.
// Name: Michael Heath, Eastman Kodak Company
// Date: 06/26/2003
//=============================================================================
struct Detection *ftr_v1point1_ftrdata_to_detections(struct FTR *ftrdata,
	struct Detection **detectionarray, int numdetections)
{
	struct IndexLink *detindexbase = NULL, *detindex = NULL;
	struct Detection *detection = NULL, *newdetection = NULL, *lastdetection = NULL;
	struct FTR *ftrdataptr = NULL;
	struct FTRcandidate *ftrcandidateptr = NULL;
    int c = 0, d = 0;

	ftrdataptr = ftrdata;
	while(ftrdataptr != NULL){
		ftrcandidateptr = ftrdataptr->ftrcandidate;
		while(ftrcandidateptr != NULL){
			newdetection = (struct Detection *) calloc(1, sizeof(struct Detection));
			if(d == 0){
				detection = newdetection;
				lastdetection = newdetection;
			}
			else{
				lastdetection->nextdetection = newdetection;
				lastdetection = newdetection;
			}

			detindexbase = where_detectionarray(detectionarray, numdetections, NULL, "CANDIDATE", "eq",
				ftrcandidateptr->candidatenumber, 1);
			detindex = where_detectionarray(detectionarray, numdetections, detindexbase, "IMAGE", "eq",
				ftrcandidateptr->image, 0);

			if(detindex != NULL){
				newdetection->point = detectionarray[detindex->index]->point;
				newdetection->cols = detectionarray[detindex->index]->cols;
				newdetection->rows = detectionarray[detindex->index]->rows;
				newdetection->nvp = create_nvp(newdetection->nvp, "IMAGE", ftrcandidateptr->image);
				newdetection->nvp = create_nvp(newdetection->nvp, "CANDIDATE", ftrcandidateptr->candidatenumber);
				newdetection->nvp = create_nvp(newdetection->nvp, "APRIORI", ftrdataptr->apriori[0]);
				newdetection->nvp = create_nvp(newdetection->nvp, "CLASS", ftrcandidateptr->classlabel);
			}

			ftrcandidateptr = ftrcandidateptr->nextftrcandidate;
			d++;
		}
		ftrdataptr = ftrdataptr->nextftr;
	}

	return(detection);
}
