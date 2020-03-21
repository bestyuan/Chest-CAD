#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hit_io.h"
#include "groundtruth.h"

//-----------------------------------------------------------------------------
// Function: read_hit_v1point0
// Purpose: To read the contents of a hit file into a data structure. This
// code was written to read hit files that are in the format of version 1.
// All of the comments are ignored.
// Name: Michael Heath, Eastman Kodak Company
// Date: 6/13/2003
//-----------------------------------------------------------------------------
struct ImageTruth *read_hit_v1point0(char *filename, struct ImageTruth *imagetruth)
{
	FILE *fptr = NULL;
	struct ImageTruth *newimagetruth = NULL, *imagetruthptr = NULL;
	char aline[302];
	int i = 0, numtruths = 0, numpoints = 0;
	char tempstring[300] = {'\0'};
	char *thisfunctionname = "read_hit_v1point0";
	struct Truth *newtruth = NULL, *lasttruth = NULL;
	struct ChainPoint *newchainpoint = NULL, *lastchainpoint = NULL;
	int truthid = 0;
	char *cptr = NULL;

	if((fptr = fopen(filename, "r")) == NULL){
		fprintf(stderr, "Error! Truth file %s could not be opened\n", filename);
		return(NULL);
	}

// #^HIT V1
// # Date/Time Generated: Tue Jan 07 09:22:13 2003
// # Source Image: L:\chestCad\TorontoGeneral\Cases\TGH-EK0025\TGH-EK0025.img
// # Image: rotate 90
// # Image: scale 17492 19199
// # Image: flip
// # Source CAD: L:\chestCad\TorontoGeneral\Cases\TGH-EK0025\TGH-EK0025.cad
// # CAD: rotate 90
// # CAD: flip
// # Source Truth_Type: BOUNDARY
// # Source Truth: L:\chestCad\TorontoGeneral\Cases\TGH-EK0025\TGH-EK0025_boundary_1.txt
// # Truth: rotate 90
// # Truth: flip
// xraySourceID: TGH-EK0025_CO.img
// imageSize: 2500 2048
// numNodules: 1
// noduleIndex: 0
// status: definite
// centroid: 1582 880
// nPoints: 281
// 1550 915
// 1549 915

	i = 0;
	while((fgets(aline, 300, fptr) != NULL) && (!feof(fptr))){
		if(i == 0){
			if(strncmp(aline, "#^HIT V1", strlen("#^HIT V1")) != 0){
				fprintf(stderr, "Error! The file %s is not a valid version 1 hit file!\n", filename);
				fclose(fptr);
				return(NULL);
			}
			else newimagetruth = (struct ImageTruth *) calloc(1, sizeof(struct ImageTruth));
			i++;
			continue;
		}
		if((aline[0] == '#') || (aline[0] == ' ')) continue;
		cptr = strchr(aline, '\n');
		if(cptr != NULL) *cptr = '\0';
		if(strncmp(aline, "xraySourceID:", strlen("xraySourceID:")) == 0){
			sscanf(aline, "%*s%s", tempstring);
			newimagetruth->sourceimage = (char *) calloc(strlen(tempstring)+1, sizeof(char));
			strcpy(newimagetruth->sourceimage, tempstring);
		}
		else if(strncmp(aline, "imageSize:", strlen("imageSize:")) == 0){
			sscanf(aline, "%*s%d%d", &(newimagetruth->cols), &(newimagetruth->rows));
		}
		else if(strncmp(aline, "numNodules:", strlen("numNodules:")) == 0){
			sscanf(aline, "%*s%d", &numtruths);
			newimagetruth->numtruth = numtruths;
			newimagetruth->trutharray = (struct Truth **) calloc(numtruths, sizeof(struct Truth *));
		}
		else if(strncmp(aline, "noduleIndex:", strlen("noduleIndex:")) == 0){
			newtruth = (struct Truth *) calloc(1, sizeof(struct Truth));
			sscanf(aline, "%*s%d", &(newtruth->index));
			if(newimagetruth->truth == NULL) newimagetruth->truth = newtruth;
			else lasttruth->nexttruth = newtruth;
			lasttruth = newtruth;
			newimagetruth->trutharray[truthid] = newtruth;
			truthid++;
		}
		else if(strncmp(aline, "status:", strlen("status:")) == 0){
			sscanf(aline, "%*s%s", tempstring);
			newtruth->status = (char *) calloc(strlen(tempstring)+1, sizeof(char));
			strcpy(newtruth->status, tempstring);
		}
		else if(strncmp(aline, "centroid:", strlen("centroid:")) == 0){
			sscanf(aline, "%*s %d %d", &(newtruth->centroid.x), &(newtruth->centroid.y));
		}
		else if(strncmp(aline, "nPoints:", strlen("nPoints:")) == 0){
			sscanf(aline, "%*s%d", &numpoints);
		}
		else{
			newchainpoint = (struct ChainPoint *) calloc(1, sizeof(struct ChainPoint));
			if(newtruth->boundary == NULL) newtruth->boundary = newchainpoint;
			else lastchainpoint->nextpoint = newchainpoint;
			lastchainpoint = newchainpoint;
			sscanf(aline, "%d %d", &(newchainpoint->point.x), &(newchainpoint->point.y));
		}
	}

	fclose(fptr);

	if(imagetruth == NULL) return(newimagetruth);

	imagetruthptr = imagetruth;
	while(imagetruthptr->nextimagetruth != NULL) imagetruthptr = imagetruthptr->nextimagetruth;
	imagetruthptr->nextimagetruth = newimagetruth;

	return(imagetruth);
}

void free_chain(struct ChainPoint *chainpoint)
{
	if(chainpoint == NULL) return;
	if(chainpoint->nextpoint != NULL) free_chain(chainpoint->nextpoint);
	free(chainpoint);
}

void free_truth(struct Truth *truth)
{
	if(truth == NULL) return;
	if(truth->nexttruth != NULL) free_truth(truth->nexttruth);
	if(truth->boundary != NULL) free_chain(truth->boundary);
	if(truth->status != NULL) free(truth->status);
	free(truth);
}

struct ImageTruth *free_imagetruth(struct ImageTruth *imagetruth)
{
	if(imagetruth == NULL) return(NULL);
	if(imagetruth->nextimagetruth != NULL) free_imagetruth(imagetruth->nextimagetruth);
	if(imagetruth->truth != NULL) free_truth(imagetruth->truth);
	if(imagetruth->sourceimage != NULL) free(imagetruth->sourceimage);
	if(imagetruth->trutharray != NULL) free(imagetruth->trutharray);
	free(imagetruth);
	return(NULL);
}

int read_hitset_v1point0(char *filename, struct ImageTruth **imagetruth)
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
			if((*imagetruth = read_hit_v1point0(aline, NULL)) == NULL) return 1;
		}
		else{
			if((*imagetruth = read_hit_v1point0(aline, *imagetruth)) == NULL) return 1;
		}
		i++;
	}

	fclose(fptr);

	return 0;
}

struct VUnionLL *sorted_unique_values_for_name(struct GroundTruth **groundtrutharray,
	int numgroundtruths, char *name, char *type)
{
	struct VUnionLL *topvunion = NULL, *newvunion = NULL, *lastvunion = NULL, *vunionptr = NULL;
	union VUnion *cptr = NULL;
	char *tptr = NULL;
	int d = 0;

	for(d=0;d<numgroundtruths;d++){

		cptr = get_nvp_value_for_name(groundtrutharray[d]->nvp, name);

		tptr = get_nvp_type_for_name(groundtrutharray[d]->nvp, name);

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
