//=============================================================================
// File: can_io.cpp
// Description: The code in this file implements reading .can files that
// contain candidate detections of lung nodules. The file format was created
// by the chest CAD algorithm development team. Documents exist that define
// the file format. They were written by Joseph Revelli from the ISD of
// Eastman Kodak. This code implments readers for V1 and V1.1 versions of the
// format. Please note that the boundary or mask information is not read from
// the .can files by this code. Rather, it is just skipped. Candidates are
// read into a linked list of candidate information.
// Name: Michael Heath, Eastman Kodak Company
// Date: 06/26/2003
// Copyright: Eastman Kodak Company
//=============================================================================
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "can_io.h"
#include "detection_io.h"

// Here is a portion from an example V1 .can file.
//#^CAN V1
//# Senn Features V1.1
//xraySourceID: EK0002_0_PA_P_CR400_CO.img
//imageSize: 2048 2500
//numCandidates: 50
//typeRegion: b
//numFeatures: 7
//flabels: coin afum rdens grad vloc hloc lvar
//candidateIndex: 0
//ftrVector: 1.292E2 1.11E2 1.498E-1 4.837E1 1.382E-1 2.46E-1 4.95E-3
//centroid: 409 1721
//nPoints: 1
//409 1721
//candidateIndex: 1
//ftrVector: 9.472E1 7.4E1 4.969E-2 1.17E2 4.116E-2 1.446E-1 3.662E-2
//centroid: 617 457
//nPoints: 1
//617 457
struct CanData *read_can_v1point0(char *filename, struct CanData *candata)
{
	FILE *fptr = NULL;
	struct CanData *newcandata = NULL, *candataptr = NULL;
	char aline[1002];
	int i = 0, numcandidates = 0, numfeatures = 0, numpoints = 0;
	char tempstring[300] = {'\0'};
	char *thisfunctionname = "read_can_v1point0";
	struct Candidate *newcandidate = NULL, *lastcandidate = NULL;
	int canid = 0;
	char regiontype = '\0';
	char *seperator = " ";
	char *cptr = NULL;

	if((fptr = fopen(filename, "r")) == NULL){
		fprintf(stderr, "Error! Candidate file [%s] could not be opened\n", filename);
		return(NULL);
	}

	i = 0;
	while((fgets(aline, 1000, fptr) != NULL) && (!feof(fptr))){
		if(i == 0){
			if(strncmp(aline, "#^CAN V1", strlen("#^CAN V1")) != 0){
				fprintf(stderr, "Error! The file %s is not a valid version 1 can file!\n", filename);
				fclose(fptr);
				return(NULL);
			}
			else newcandata = (struct CanData *) calloc(1, sizeof(struct CanData));
			i++;
			continue;
		}
		cptr = strchr(aline, '\n');
		if(cptr != NULL) *cptr = '\0';
		if((aline[0] == '#') || (aline[0] == ' ')) continue;
		if(strncmp(aline, "xraySourceID:", strlen("xraySourceID:")) == 0){
			sscanf(aline, "%*s%s", tempstring);
			newcandata->sourceimage = (char *) calloc(strlen(tempstring)+1, sizeof(char));
			strcpy(newcandata->sourceimage, tempstring);
		}
		else if(strncmp(aline, "imageSize:", strlen("imageSize:")) == 0){
			sscanf(aline, "%*s%d%d", &(newcandata->cols), &(newcandata->rows));
		}
		else if(strncmp(aline, "numCandidates:", strlen("numCandidates:")) == 0){
			sscanf(aline, "%*s%d", &numcandidates);
			newcandata->numcandidates = numcandidates;
			newcandata->candidatearray = (struct Candidate **) calloc(numcandidates, sizeof(struct Candidate *));
		}
		else if(strncmp(aline, "numFeatures:", strlen("numFeatures:")) == 0){
			sscanf(aline, "%*s%d", &numfeatures);
			newcandata->numfeatures = numfeatures;
		}
		else if(strncmp(aline, "flabels:", strlen("flabels:")) == 0){
			int fn = 0;
			cptr = strtok(aline, seperator);
			newcandata->featurelabels = (char **) calloc(numfeatures, sizeof(char *));
			fn = 0;
			while(cptr != NULL){
				cptr = strtok((char *)NULL, seperator);
				if(cptr != NULL){
					sscanf(cptr, "%s", tempstring);
					newcandata->featurelabels[fn] = (char *) calloc(strlen(tempstring)+1, sizeof(char));
					strcpy(newcandata->featurelabels[fn], tempstring);
					fn++;
				}
			}
			// for(fn=0;fn<numfeatures;fn++) printf("%s\n", (*candata)->featurelabels[fn]);
			continue;
		}
		else if(strncmp(aline, "ftrVector:", strlen("ftrVector:")) == 0){
			if(numfeatures != 0){
				newcandidate->ftrvector_line = (char *) calloc(strlen(aline)+1-strlen("ftrVector: "), sizeof(char));
				strcpy(newcandidate->ftrvector_line, aline+strlen("ftrVector: "));
				newcandidate->ftrvector = (float *) calloc(numfeatures, sizeof(float));
				int fn = 0;
				cptr = strtok(aline, seperator);
				fn = 0;
				while(cptr != NULL){
					cptr = strtok((char *)NULL, seperator);
					if(cptr != NULL){
						sscanf(cptr, "%f", &(newcandidate->ftrvector[fn]));
						fn++;
					}
				}
				// for(fn=0;fn<numfeatures;fn++) printf("%f ", newcandidate->ftrvector[fn]);
				// printf("\n");
			}
			continue;
		}
		else if(strncmp(aline, "typeRegion:", strlen("typeRegion:")) == 0){
			sscanf(aline, "%*s %c", &regiontype);
			// printf("regiontype = %c\n", regiontype);
			newcandata->regiontype = regiontype;
		}
		else if(strncmp(aline, "candidateIndex:", strlen("candidateIndex:")) == 0){
			newcandidate = (struct Candidate *) calloc(1, sizeof(struct Candidate));
			if(newcandata->candidate == NULL) newcandata->candidate = newcandidate;
			else lastcandidate->nextcandidate = newcandidate;
			lastcandidate = newcandidate;
			sscanf(aline, "%*s %d", &(newcandidate->index));
			newcandata->candidatearray[canid] = newcandidate;
			canid++;
		}
		else if(strncmp(aline, "centroid:", strlen("centroid:")) == 0){
			sscanf(aline, "%*s %d %d", &(newcandidate->centroid.x), &(newcandidate->centroid.y));
		}
		else if(strncmp(aline, "nPoints:", strlen("nPoints:")) == 0){
         sscanf(aline, "%*s%d", &numpoints);
		}
//		else{
//			newchainpoint = (struct ChainPoint *) calloc(1, sizeof(struct ChainPoint));
//   		if(newtruth->boundary == NULL) newtruth->boundary = newchainpoint;
//			else lastchainpoint->nextpoint = newchainpoint;
//			lastchainpoint = newchainpoint;
//			sscanf(aline, "%d %d", &(newchainpoint->point.x), &(newchainpoint->point.y));
//		}
	}

	fclose(fptr);

	if(candata == NULL) return(newcandata);

	candataptr = candata;
	while(candataptr->nextcandata != NULL) candataptr = candataptr->nextcandata;
	candataptr->nextcandata = newcandata;

	return(candata);

}

// Here is a portion from an example V1.1 .can file.
//#^CAN V1.1
//xraySourceID: EK0002_0_PA_P_CR400_CO.img
//imageSize: 2048 2500
//numCandidates: 50
//typeRegion: m
//numFeatures: 14
//flabels: coin afum rdens grad vloc hloc lvar Shape.area Shape.aspectRatio Shape.fit Region.avgCodeValueRatio Region.sigmaCodeValueRatio Region.avgCodeValueRatio Region.sigmaCodeValueRatio 
//endHeader:
//######
//###############################################
//###############################################
//###############################################
//###############################################
//###############################################
//###############################################
//###############################################
//###############################################
//###############################################
//###############################################
//###############################################
//###############################################
//###############################################
//###############################################
//###############################################
//###############################################
//###############################################
//###############################################
//###############################################
//###############################################
//###############################################
//###############################################
//candidateIndex: 0
//groundtruth: unknown
//probability: 0.5
//ftrVector: 129.2 111 0.1498 48.37 0.1382 0.246 0.00495 0.0197792 3.84979 0.875671 1.33427 0.490805 1.09767 0.450117 
//centroid: 409 1721
//nPoints: 5185

struct CanData *read_can_v1point1(char *filename, struct CanData *candata)
{
	FILE *fptr = NULL;
	struct CanData *newcandata = NULL, *candataptr = NULL;
	char aline[1002];
	int i = 0, numcandidates = 0, numfeatures = 0, numpoints = 0;
	char tempstring[300] = {'\0'};
	char *thisfunctionname = "read_can_v1point1";
	struct Candidate *newcandidate = NULL, *lastcandidate = NULL;
	int canid = 0;
	char regiontype = '\0';
	char *seperator = " ";
	char *cptr = NULL;

	if((fptr = fopen(filename, "r")) == NULL){
		fprintf(stderr, "Error! Candidate file [%s] could not be opened\n", filename);
		return(NULL);
	}

	i = 0;
	while((fgets(aline, 1000, fptr) != NULL) && (!feof(fptr))){
		if(i == 0){
			if(strncmp(aline, "#^CAN V1.1", strlen("#^CAN V1.1")) != 0){
				fprintf(stderr, "Error! The file %s is not a valid version 1.1 can file!\n", filename);
				fclose(fptr);
				return(NULL);
			}
			else newcandata = (struct CanData *) calloc(1, sizeof(struct CanData));
			i++;
			continue;
		}
		cptr = strchr(aline, '\n');
		if(cptr != NULL) *cptr = '\0';
		if((aline[0] == '#') || (aline[0] == ' ')) continue;
		if(strncmp(aline, "xraySourceID:", strlen("xraySourceID:")) == 0){
			sscanf(aline, "%*s%s", tempstring);
			newcandata->sourceimage = (char *) calloc(strlen(tempstring)+1, sizeof(char));
			strcpy(newcandata->sourceimage, tempstring);
		}
		else if(strncmp(aline, "imageSize:", strlen("imageSize:")) == 0){
			sscanf(aline, "%*s%d%d", &(newcandata->cols), &(newcandata->rows));
		}
		else if(strncmp(aline, "numCandidates:", strlen("numCandidates:")) == 0){
			sscanf(aline, "%*s%d", &numcandidates);
			newcandata->numcandidates = numcandidates;
			newcandata->candidatearray = (struct Candidate **) calloc(numcandidates, sizeof(struct Candidate *));
		}
		else if(strncmp(aline, "numFeatures:", strlen("numFeatures:")) == 0){
			sscanf(aline, "%*s%d", &numfeatures);
			newcandata->numfeatures = numfeatures;
		}
		else if(strncmp(aline, "flabels:", strlen("flabels:")) == 0){
			int fn = 0;
			cptr = strtok(aline, seperator);
			newcandata->featurelabels = (char **) calloc(numfeatures, sizeof(char *));
			fn = 0;
			while(cptr != NULL){
				cptr = strtok((char *)NULL, seperator);
				if(cptr != NULL){
					sscanf(cptr, "%s", tempstring);
					newcandata->featurelabels[fn] = (char *) calloc(strlen(tempstring)+1, sizeof(char));
					strcpy(newcandata->featurelabels[fn], tempstring);
					fn++;
				}
			}
			// for(fn=0;fn<numfeatures;fn++) printf("%s\n", (*candata)->featurelabels[fn]);
			continue;
		}
		else if(strncmp(aline, "ftrVector:", strlen("ftrVector:")) == 0){
			if(numfeatures != 0){
				newcandidate->ftrvector_line = (char *) calloc(strlen(aline)+1-strlen("ftrVector: "), sizeof(char));
				strcpy(newcandidate->ftrvector_line, aline+strlen("ftrVector: "));
				newcandidate->ftrvector = (float *) calloc(numfeatures, sizeof(float));
				int fn = 0;
				cptr = strtok(aline, seperator);
				fn = 0;
				while(cptr != NULL){
					cptr = strtok((char *)NULL, seperator);
					if(cptr != NULL){
						sscanf(cptr, "%f", &(newcandidate->ftrvector[fn]));
						fn++;
					}
				}
				// for(fn=0;fn<numfeatures;fn++) printf("%f ", newcandidate->ftrvector[fn]);
				// printf("\n");
			}
			continue;
		}
		else if(strncmp(aline, "typeRegion:", strlen("typeRegion:")) == 0){
			sscanf(aline, "%*s %c", &regiontype);
			// printf("regiontype = %c\n", regiontype);
			newcandata->regiontype = regiontype;
		}
		else if(strncmp(aline, "candidateIndex:", strlen("candidateIndex:")) == 0){
			newcandidate = (struct Candidate *) calloc(1, sizeof(struct Candidate));
			if(newcandata->candidate == NULL) newcandata->candidate = newcandidate;
			else lastcandidate->nextcandidate = newcandidate;
			lastcandidate = newcandidate;
			sscanf(aline, "%*s %d", &(newcandidate->index));
			newcandata->candidatearray[canid] = newcandidate;
			canid++;
		}
		else if(strncmp(aline, "centroid:", strlen("centroid:")) == 0){
			sscanf(aline, "%*s %d %d", &(newcandidate->centroid.x), &(newcandidate->centroid.y));
		}
		else if(strncmp(aline, "probability:", strlen("probability:")) == 0){
			sscanf(aline, "%*s %f", &(newcandidate->probability));
		}
		else if(strncmp(aline, "nPoints:", strlen("nPoints:")) == 0){
			sscanf(aline, "%*s%d", &numpoints);
		}
//		else{
//			newchainpoint = (struct ChainPoint *) calloc(1, sizeof(struct ChainPoint));
//   		if(newtruth->boundary == NULL) newtruth->boundary = newchainpoint;
//			else lastchainpoint->nextpoint = newchainpoint;
//			lastchainpoint = newchainpoint;
//			sscanf(aline, "%d %d", &(newchainpoint->point.x), &(newchainpoint->point.y));
//		}
	}

	fclose(fptr);

	if(candata == NULL) return(newcandata);

	candataptr = candata;
	while(candataptr->nextcandata != NULL) candataptr = candataptr->nextcandata;
	candataptr->nextcandata = newcandata;

	return(candata);

}

//=============================================================================
// Function: read_can_v1point2
// Purpose to read the new CAN file format that incorporates group information.
// Name: Michael Heath, Eastman Kodak Company
// Date: 01/30/2004
//=============================================================================
struct CanData *read_can_v1point2(char *filename, struct CanData *candata)
{
	FILE *fptr = NULL;
	struct CanData *newcandata = NULL, *candataptr = NULL;
	char aline[1002];
	int i = 0, numcandidates = 0, numfeatures = 0, numpoints = 0;
	char tempstring[300] = {'\0'};
	char *thisfunctionname = "read_can_v1point1";
	struct Candidate *newcandidate = NULL, *lastcandidate = NULL;
	int canid = 0;
	char regiontype = '\0';
	char *seperator = " ";
	char *cptr = NULL;

	if((fptr = fopen(filename, "r")) == NULL){
		fprintf(stderr, "Error! Candidate file [%s] could not be opened\n", filename);
		return(NULL);
	}

	i = 0;
	while((fgets(aline, 1000, fptr) != NULL) && (!feof(fptr))){
		if(i == 0){
			if(strncmp(aline, "#^CAN V1.2", strlen("#^CAN V1.2")) != 0){
				fprintf(stderr, "Error! The file %s is not a valid version 1.2 can file!\n", filename);
				fclose(fptr);
				return(NULL);
			}
			else newcandata = (struct CanData *) calloc(1, sizeof(struct CanData));
			i++;
			continue;
		}
		cptr = strchr(aline, '\n');
		if(cptr != NULL) *cptr = '\0';
		if((aline[0] == '#') || (aline[0] == ' ')) continue;
		if(strncmp(aline, "xraySourceID:", strlen("xraySourceID:")) == 0){
			sscanf(aline, "%*s%s", tempstring);
			newcandata->sourceimage = (char *) calloc(strlen(tempstring)+1, sizeof(char));
			strcpy(newcandata->sourceimage, tempstring);
		}
		else if(strncmp(aline, "imageSize:", strlen("imageSize:")) == 0){
			sscanf(aline, "%*s%d%d", &(newcandata->cols), &(newcandata->rows));
		}
		else if(strncmp(aline, "numCandidates:", strlen("numCandidates:")) == 0){
			sscanf(aline, "%*s%d", &numcandidates);
			newcandata->numcandidates = numcandidates;
			newcandata->candidatearray = (struct Candidate **) calloc(numcandidates, sizeof(struct Candidate *));
		}
		else if(strncmp(aline, "numFeatures:", strlen("numFeatures:")) == 0){
			sscanf(aline, "%*s%d", &numfeatures);
			newcandata->numfeatures = numfeatures;
		}
		else if(strncmp(aline, "flabels:", strlen("flabels:")) == 0){
			int fn = 0;
			cptr = strtok(aline, seperator);
			newcandata->featurelabels = (char **) calloc(numfeatures, sizeof(char *));
			fn = 0;
			while(cptr != NULL){
				cptr = strtok((char *)NULL, seperator);
				if(cptr != NULL){
					sscanf(cptr, "%s", tempstring);
					newcandata->featurelabels[fn] = (char *) calloc(strlen(tempstring)+1, sizeof(char));
					strcpy(newcandata->featurelabels[fn], tempstring);
					fn++;
				}
			}
			// for(fn=0;fn<numfeatures;fn++) printf("%s\n", (*candata)->featurelabels[fn]);
			continue;
		}
		else if(strncmp(aline, "ftrVector:", strlen("ftrVector:")) == 0){
			if(numfeatures != 0){
				newcandidate->ftrvector_line = (char *) calloc(strlen(aline)+1-strlen("ftrVector: "), sizeof(char));
				strcpy(newcandidate->ftrvector_line, aline+strlen("ftrVector: "));
				newcandidate->ftrvector = (float *) calloc(numfeatures, sizeof(float));
				int fn = 0;
				cptr = strtok(aline, seperator);
				fn = 0;
				while(cptr != NULL){
					cptr = strtok((char *)NULL, seperator);
					if(cptr != NULL){
						sscanf(cptr, "%f", &(newcandidate->ftrvector[fn]));
						fn++;
					}
				}
				// for(fn=0;fn<numfeatures;fn++) printf("%f ", newcandidate->ftrvector[fn]);
				// printf("\n");
			}
			continue;
		}
		else if(strncmp(aline, "typeRegion:", strlen("typeRegion:")) == 0){
			sscanf(aline, "%*s %c", &regiontype);
			// printf("regiontype = %c\n", regiontype);
			newcandata->regiontype = regiontype;
		}
		else if(strncmp(aline, "candidateIndex:", strlen("candidateIndex:")) == 0){
			newcandidate = (struct Candidate *) calloc(1, sizeof(struct Candidate));
			if(newcandata->candidate == NULL) newcandata->candidate = newcandidate;
			else lastcandidate->nextcandidate = newcandidate;
			lastcandidate = newcandidate;
			sscanf(aline, "%*s %d", &(newcandidate->index));
			newcandata->candidatearray[canid] = newcandidate;
			canid++;
		}
		else if(strncmp(aline, "nGroups:", strlen("nGroups:")) == 0){   // New in CAN V1.2
			sscanf(aline, "%*s %d", &(newcandidate->ngroups));
			newcandidate->groups = (int *) calloc(newcandidate->ngroups, sizeof(int));
		}
        else if(strncmp(aline, "groups:", strlen("groups:")) == 0){    // New in CAN V1.2
			int fn = 0;
			cptr = strtok(aline, seperator);
			fn = 0;
			while(cptr != NULL){
				cptr = strtok((char *)NULL, seperator);
				if(cptr != NULL){
					sscanf(cptr, "%d", &(newcandidate->groups[fn]));
					//printf("<%d>\n", newcandidate->groups[fn]);
					fn++;
				}
			}
		}
		else if(strncmp(aline, "centroid:", strlen("centroid:")) == 0){
			sscanf(aline, "%*s %d %d", &(newcandidate->centroid.x), &(newcandidate->centroid.y));
		}
		else if(strncmp(aline, "probability:", strlen("probability:")) == 0){
			sscanf(aline, "%*s %f", &(newcandidate->probability));
		}
		else if(strncmp(aline, "nPoints:", strlen("nPoints:")) == 0){
			sscanf(aline, "%*s%d", &numpoints);
		}
//		else{
//			newchainpoint = (struct ChainPoint *) calloc(1, sizeof(struct ChainPoint));
//   		if(newtruth->boundary == NULL) newtruth->boundary = newchainpoint;
//			else lastchainpoint->nextpoint = newchainpoint;
//			lastchainpoint = newchainpoint;
//			sscanf(aline, "%d %d", &(newchainpoint->point.x), &(newchainpoint->point.y));
//		}
	}

	fclose(fptr);

	if(candata == NULL) return(newcandata);

	candataptr = candata;
	while(candataptr->nextcandata != NULL) candataptr = candataptr->nextcandata;
	candataptr->nextcandata = newcandata;

	return(candata);

}

//=============================================================================
// Function: free_candidate
// Purpose: This recursive function frees a linked list of candidates.
// Name: Michael Heath, Eastman Kodak Company
// Date: 06/26/2003
// Modified for CAN V1.2 on 01-30-2004 by Mike Heath
//=============================================================================
void free_candidate(struct Candidate *candidate)
{
	if(candidate == NULL) return;
	if(candidate->nextcandidate != NULL) free_candidate(candidate->nextcandidate);
	if(candidate->ftrvector != NULL) free(candidate->ftrvector);
	if(candidate->groups != NULL) free(candidate->groups);   // Added for V1.2
	if(candidate->ftrvector_line != NULL) free(candidate->ftrvector_line);
	free(candidate);
}

//=============================================================================
// Function: free_candata
// Purpose: This recursive function frees a linked list of candidate data from
// .can files. Each link in the list represents the data from one .can file.
// Each link itself contains a linked list of candidates which are freed be
// calling the free_candidate function.
// Name: Michael Heath, Eastman Kodak Company
// Date: 06/26/2003
//=============================================================================
void free_candata(struct CanData *candata)
{
	int fn = 0;
	if(candata == NULL) return;
	if(candata->nextcandata != NULL) free_candata(candata->nextcandata);
	free_candidate(candata->candidate);
    if(candata->candidatearray != NULL) free(candata->candidatearray);
	if(candata->sourceimage != NULL) free(candata->sourceimage);
	if(candata->featurelabels != NULL)
		for(fn=0;fn<candata->numfeatures;fn++)
			if(candata->featurelabels[fn] != NULL)
				free(candata->featurelabels[fn]);
	free(candata);
	candata = NULL; 
}

//=============================================================================
// Function: free_candata
// Purpose: This function frres a linked list of candidate data. It merely
// calls the free_candata function to do this and then sets the top linked list
// pointer to NULL to properly clean up the reference to the memory.
// Name: Michael Heath, Eastman Kodak Company
// Date: 06/26/2003
//=============================================================================
void free_candata(struct CanData **candata)
{
   if(candata == NULL) return;

   free_candata(*candata);

   *candata = NULL;
}

//=============================================================================
// Function: get_cansetversion
// Purpose: Given the name of a file that contains a list of .can filenames
// written one per line, this function determines the version of the .can
// files by finding the version of the first .can file in the list.
// Name: Michael Heath, Eastman Kodak Company
// Date: 06/26/2003
// Modified for CAN V1.2 on 01-30-2004 by Mike Heath
//=============================================================================
float get_cansetversion(char *filename)
{
	FILE *fptr = NULL;
	char *cptr = NULL, aline[302] = {'\0'};
	float version = 0.0;

	if((fptr = fopen(filename, "r")) == NULL){
		fprintf(stderr, "Error! Cannot open the file [%s]\n\n", filename);
		return(1);
	}

	if(fgets(aline, 300, fptr) == NULL){
		fclose(fptr);
		return version;
	}
	fclose(fptr);

	cptr = strchr(aline, '\n');
	if(cptr != NULL) *cptr = '\0';

	if((fptr = fopen(aline, "r")) == NULL){
		fprintf(stderr, "Error! Cannot open the file [%s]\n\n", aline);
		return(1);
	}

	cptr = strchr(aline, '\n');
	if(cptr != NULL) *cptr = '\0';

	if(fgets(aline, 300, fptr) == NULL){
		fclose(fptr);
		return version;
	}
	fclose(fptr);

	// Check for V1.1 first because V1 is a substring of V1.1. Also note
	// that is V1.0 is listed in the file (as it should be) it will still
	// register as a V1 file.
	if(strncmp(aline, "#^CAN V1.2", strlen("#^CAN V1.2")) == 0) version = 1.2f;
	else if(strncmp(aline, "#^CAN V1.1", strlen("#^CAN V1.1")) == 0) version = 1.1f;
	else if(strncmp(aline, "#^CAN V1", strlen("#^CAN V1")) == 0) version = 1.0f;

	return version;
}

//=============================================================================
// Function: read_canset_v1point0
// Purpose: This function reads the data from a set of .can files and places
// the data in a linked list. The set of .can files are specified one per line
// in a text file whose name is passed into the function in the filename
// argument. The .can file must all be version V1.
// Name: Michael Heath, Eastman Kodak Company
// Date: 06/26/2003
//=============================================================================
int read_canset_v1point0(char *filename, struct CanData **candata)
{
	FILE *fptr = NULL;
	char *cptr = NULL, aline[302];
    int i = 0;

	if((fptr = fopen(filename, "r")) == NULL){
		fprintf(stderr, "Error! Cannot open the file [%s]\n\n", filename);
		return(1);
	}

	while((fgets(aline, 300, fptr) != NULL) && !feof(fptr)){

		cptr = strchr(aline, '\n');
		if(cptr != NULL) *cptr = '\0';
		if(i == 0){
			if((*candata = read_can_v1point0(aline, NULL)) == NULL) return 1;
		}
		else{
			if((*candata = read_can_v1point0(aline, *candata)) == NULL) return 1;
		}
		i++;
	}

	fclose(fptr);

	return 0;
}

//=============================================================================
// Function: read_canset_v1point1
// Purpose: This function reads the data from a set of .can files and places
// the data in a linked list. The set of .can files are specified one per line
// in a text file whose name is passed into the function in the filename
// argument. The .can files must all be version V1.1.
// Name: Michael Heath, Eastman Kodak Company
// Date: 06/26/2003
//=============================================================================
int read_canset_v1point1(char *filename, struct CanData **candata)
{
	FILE *fptr = NULL;
	char *cptr = NULL, aline[302];
    int i = 0;

	if((fptr = fopen(filename, "r")) == NULL){
		fprintf(stderr, "Error! Cannot open the file [%s]\n\n", filename);
		return(1);
	}

	while((fgets(aline, 300, fptr) != NULL) && !feof(fptr)){

		cptr = strchr(aline, '\n');
		if(cptr != NULL) *cptr = '\0';
		if(i == 0){
			if((*candata = read_can_v1point1(aline, NULL)) == NULL) return 1;
		}
		else{
			if((*candata = read_can_v1point1(aline, *candata)) == NULL) return 1;
		}
		i++;
	}

	fclose(fptr);

	return 0;
}

//=============================================================================
// Function: read_canset_v1point2
// Purpose: This function reads the data from a set of .can files and places
// the data in a linked list. The set of .can files are specified one per line
// in a text file whose name is passed into the function in the filename
// argument. The .can files must all be version V1.2.
// Name: Michael Heath, Eastman Kodak Company
// Date: 01/30/2004
//=============================================================================
int read_canset_v1point2(char *filename, struct CanData **candata)
{
	FILE *fptr = NULL;
	char *cptr = NULL, aline[302];
    int i = 0;

	if((fptr = fopen(filename, "r")) == NULL){
		fprintf(stderr, "Error! Cannot open the file [%s]\n\n", filename);
		return(1);
	}

	while((fgets(aline, 300, fptr) != NULL) && !feof(fptr)){

		cptr = strchr(aline, '\n');
		if(cptr != NULL) *cptr = '\0';
		if(i == 0){
			if((*candata = read_can_v1point2(aline, NULL)) == NULL) return 1;
		}
		else{
			if((*candata = read_can_v1point2(aline, *candata)) == NULL) return 1;
		}
		i++;
	}

	fclose(fptr);

	return 0;
}