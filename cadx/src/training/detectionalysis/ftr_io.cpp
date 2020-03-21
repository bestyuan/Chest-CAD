//=============================================================================
// File: ftr_io.cpp
// Description: The code in this file implements functions for reading FTR
// format files. These files 
// Name: Michael Heath, Eastman Kodak Company
// Date: 06/26/2003
// Copyright: Eastman Kodak Company
//=============================================================================
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ftr_io.h"

//#^FTR V1.0
//title: NewYorkHospital
//nclasses: 2
//clabels: P N 
//nsamples: 524 2461 
//apriori: 0.239999 0.760001 
//nfeatures: 14
//flabels: coin afum rdens grad vloc hloc lvar Shape.area Shape.aspectRatio Shape.fit Region.avgCodeValueRatio Region.sigmaCodeValueRatio Region.avgCodeValueRatio Region.sigmaCodeValueRatio 
//129.200000 111.000000 0.149800 48.370000 0.138200 0.246000 0.004950 0.019779 3.849790 0.875671 1.334270 0.490805 1.097670 0.450117 P EK0002_0_PA_P_CR400_CO.img canID:_0 
//94.720000 74.000000 0.049690 117.000000 0.041160 0.144600 0.036620 0.099949 3.177950 0.630953 1.468420 0.333826 1.147130 0.460577 N EK0002_0_PA_P_CR400_CO.img canID:_1 
//93.550000 98.000000 0.072640 38.600000 0.113200 0.016860 0.008764 0.057510 5.145910 0.823745 1.204530 0.522021 1.079190 0.553164 N EK0002_0_PA_P_CR400_CO.img canID:_2 
//92.690000 60.000000 0.235000 42.760000 0.034390 0.003670 0.019790 0.050907 5.036250 0.810463 1.246600 0.545309 1.066110 0.473976 N EK0002_0_PA_P_CR400_CO.img canID:_3 
//92.610000 63.000000 0.039030 75.170000 0.070010 0.245700 0.032080 0.025459 1.700300 0.825223 1.166450 0.419956 1.049530 0.474060 N EK0002_0_PA_P_CR400_CO.img canID:_4 
//89.560000 49.000000 0.051770 29.150000 0.056900 0.348100 0.007725 0.099934 4.180550 0.625482 1.491460 0.321165 1.146110 0.474869 N EK0002_0_PA_P_CR400_CO.img canID:_5 
//85.920000 66.000000 0.039670 66.570000 0.078240 0.414000 0.005956 0.094849 3.886890 0.642218 1.473750 0.342740 1.163500 0.465910 N EK0002_0_PA_P_CR400_CO.img canID:_6 
//84.550000 102.000000 0.030000 88.600000 0.019160 0.345900 0.006099 0.016655 5.377770 0.801567 1.420600 0.541647 1.188400 0.486308 N EK0002_0_PA_P_CR400_CO.img canID:_7 
//82.960000 62.000000 0.061030 87.050000 0.097780 0.028330 0.005537 0.014740 2.002930 0.955743 0.957587 0.180503 1.058550 0.204787 N EK0002_0_PA_P_CR400_CO.img canID:_8 
//81.420000 124.000000 0.013740 19.030000 0.063460 0.097260 0.015250 0.005375 1.980560 0.920623 1.409450 0.636496 1.150990 0.628787 P EK0002_0_PA_P_CR400_CO.img canID:_9_nodID:_0 

void print_ftrdata(struct FTR *ftrdata)
{
	struct FTR *ftrptr = NULL;
	struct FTRcandidate *ftrcandidateptr = NULL;
	int fn = 0;

	ftrptr = ftrdata;
	while(ftrptr != NULL){
		printf("TITLE: %s\n", ftrptr->title);
		printf("NUMCLASSES: %d\n", ftrptr->numclasses);
		printf("CLASSES:");
		for(fn=0;fn<ftrptr->numclasses;fn++) printf(" %s", ftrptr->classlabels[fn]);
		printf("\n");
		printf("SAMPLESPERCLASS:");
		for(fn=0;fn<ftrptr->numclasses;fn++) printf(" %d", ftrptr->samplesperclass[fn]);
		printf("\n");
		printf("APRIORI:");
		for(fn=0;fn<ftrptr->numclasses;fn++) printf(" %f", ftrptr->apriori[fn]);
		printf("\n");
		printf("NUMFEATURES: %d\n", ftrptr->numfeatures);
		for(fn=0;fn<ftrptr->numfeatures;fn++) printf("%s\n", ftrptr->featurelabels[fn]);
		ftrcandidateptr = ftrdata->ftrcandidate;
		fn = 0;
		while(ftrcandidateptr != NULL){
			if((fn < 1) || (ftrcandidateptr->nextftrcandidate == NULL)){
				printf("%s %s %d\n", ftrcandidateptr->classlabel, ftrcandidateptr->image, ftrcandidateptr->candidatenumber);
			}
			fn++;
			ftrcandidateptr = ftrcandidateptr->nextftrcandidate;
		}
		ftrptr = ftrptr->nextftr;
	}
}

struct FTR *read_ftr_v1point0(char *filename, struct FTR *ftrdata)
{
	FILE *fptr = NULL;
	struct FTR *newftrdata = NULL, *ftrdataptr = NULL;
	char aline[1002];
	int i = 0, numcandidates = 0, numfeatures = 0, numpoints = 0;
	char tempstring[300] = {'\0'};
	char *thisfunctionname = "read_ftr_v1point0";
	struct FTRcandidate *newftrcandidate = NULL, *lastftrcandidate = NULL;

	char *seperator = " ";
	char *cptr = NULL;
	int ingest_candidates = 0;
	int fn = 0;

	if((fptr = fopen(filename, "r")) == NULL){
		fprintf(stderr, "Error! Ftr file [%s] could not be opened\n", filename);
		return(NULL);
	}

	i = 0;
	while((fgets(aline, 1000, fptr) != NULL)){ //  && (!feof(fptr))){
		// printf("%s", aline);
		if(i == 0){
			if(strncmp(aline, "#^FTR V1.0", strlen("#^FTR V1.0")) != 0){
				fprintf(stderr, "Error! The file %s is not a valid version 1.0 ftr file!\n", filename);
				fclose(fptr);
				return(NULL);
			}
			else newftrdata = (struct FTR *) calloc(1, sizeof(struct FTR));
			i++;
			continue;
		}
		cptr = strchr(aline, '\n');
		if(cptr != NULL) *cptr = '\0';
		if((aline[0] == '#') || (aline[0] == ' ')) continue;
		if(strncmp(aline, "title:", strlen("title:")) == 0){
			sscanf(aline, "%*s%s", tempstring);
			newftrdata->title = (char *) calloc(strlen(tempstring)+1, sizeof(char));
			strcpy(newftrdata->title, tempstring);
		}
		else if(strncmp(aline, "nclasses:", strlen("nclasses:")) == 0){
			sscanf(aline, "%*s %d", &(newftrdata->numclasses));
		}
		else if(strncmp(aline, "clabels:", strlen("clabels:")) == 0){
			cptr = strtok(aline, seperator);
			newftrdata->classlabels = (char **) calloc(newftrdata->numclasses, sizeof(char *));
			fn = 0;
			while(cptr != NULL){
				cptr = strtok((char *)NULL, seperator);
				if(cptr != NULL){
					sscanf(cptr, "%s", tempstring);
					newftrdata->classlabels[fn] = (char *) calloc(strlen(tempstring)+1, sizeof(char));
					strcpy(newftrdata->classlabels[fn], tempstring);
					fn++;
				}
			}
			// for(fn=0;fn<newftrdata->numclasses;fn++) printf("%s\n", (*newftrdata)->classlabels[fn]);
		}
		else if(strncmp(aline, "nsamples:", strlen("nsamples:")) == 0){
			cptr = strtok(aline, seperator);
			newftrdata->samplesperclass = (int *) calloc(newftrdata->numclasses, sizeof(int));
			fn = 0;
			while(cptr != NULL){
				cptr = strtok((char *)NULL, seperator);
				if(cptr != NULL){
					sscanf(cptr, "%d", &(newftrdata->samplesperclass[fn]));
					fn++;
				}
			}
			// for(fn=0;fn<newftrdata->numclasses;fn++) printf("%d\n", (*newftrdata)->samplesperclass[fn]);
		}
		else if(strncmp(aline, "apriori:", strlen("apriori:")) == 0){
			cptr = strtok(aline, seperator);
			newftrdata->apriori = (float *) calloc(newftrdata->numclasses, sizeof(float));
			fn = 0;
			while(cptr != NULL){
				cptr = strtok((char *)NULL, seperator);
				if(cptr != NULL){
					sscanf(cptr, "%f", &(newftrdata->apriori[fn]));
					fn++;
				}
			}
			// for(fn=0;fn<newftrdata->numclasses;fn++) printf("%f\n", (*newftrdata)->apriori[fn]);
		}
		else if(strncmp(aline, "nfeatures:", strlen("nfeatures:")) == 0){
			sscanf(aline, "%*s %d", &(newftrdata->numfeatures));
		}
		else if(strncmp(aline, "flabels:", strlen("flabels:")) == 0){
			cptr = strtok(aline, seperator);
			newftrdata->featurelabels = (char **) calloc(newftrdata->numfeatures, sizeof(char *));
			fn = 0;
			while(cptr != NULL){
				cptr = strtok((char *)NULL, seperator);
				if(cptr != NULL){
					sscanf(cptr, "%s", tempstring);
					newftrdata->featurelabels[fn] = (char *) calloc(strlen(tempstring)+1, sizeof(char));
					strcpy(newftrdata->featurelabels[fn], tempstring);
					fn++;
				}
			}
			// for(fn=0;fn<newftrdata->numfeatures;fn++) printf("%s\n", (*newftrdata)->featurelabels[fn]);
			ingest_candidates = 1;
		}
		else{
			if(ingest_candidates){
				newftrcandidate = (struct FTRcandidate *) calloc(1, sizeof(FTRcandidate));

				newftrcandidate->feature = (float *) calloc(newftrdata->numfeatures, sizeof(float));
				if(newftrdata->ftrcandidate == NULL) newftrdata->ftrcandidate = newftrcandidate;
				else lastftrcandidate->nextftrcandidate = newftrcandidate;
				lastftrcandidate = newftrcandidate;

				for(fn=0;fn<newftrdata->numfeatures;fn++){
					if(fn == 0) cptr = strtok(aline, seperator);
					else cptr = strtok((char *)NULL, seperator);
					if(cptr != NULL) sscanf(cptr, "%f", &(newftrcandidate->feature[fn]));
				}

				cptr = strtok((char *)NULL, seperator);
				sscanf(cptr, "%s", tempstring);
				newftrcandidate->classlabel = (char *) calloc(strlen(tempstring)+1, sizeof(char));
				strcpy(newftrcandidate->classlabel, tempstring);

				cptr = strtok((char *)NULL, seperator);
				sscanf(cptr, "%s", tempstring);
				newftrcandidate->image = (char *) calloc(strlen(tempstring)+1, sizeof(char));
				strcpy(newftrcandidate->image, tempstring);

				cptr = strtok((char *)NULL, "_");
				cptr = strtok((char *)NULL, "_");
				sscanf(cptr, "%d", &(newftrcandidate->candidatenumber));

			}
		}
	}

	fclose(fptr);

	if(ftrdata == NULL) return(newftrdata);

	ftrdataptr = ftrdata;
	while(ftrdataptr->nextftr != NULL) ftrdataptr = ftrdataptr->nextftr;
	ftrdataptr->nextftr = newftrdata;

	return(ftrdata);
}

int read_ftrset_v1point0(char *filename, struct FTR **ftrdata)
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
			if((*ftrdata = read_ftr_v1point0(aline, NULL)) == NULL) return 1;
		}
		else{
			if((*ftrdata = read_ftr_v1point0(aline, *ftrdata)) == NULL) return 1;
		}
		i++;
	}

	fclose(fptr);

	return 0;
}

static struct FTRcandidate *free_ftrcandidate(struct FTRcandidate *ftrcandidate)
{
	if(ftrcandidate == NULL) return(NULL);
	if(ftrcandidate->nextftrcandidate != NULL)
		ftrcandidate->nextftrcandidate = free_ftrcandidate(ftrcandidate->nextftrcandidate);
	if(ftrcandidate->feature != NULL) free(ftrcandidate->feature);
	if(ftrcandidate->classlabel != NULL) free(ftrcandidate->classlabel);
	if(ftrcandidate->image != NULL) free(ftrcandidate->image);
	free(ftrcandidate);
	return(NULL);
}

struct FTR *free_ftr(struct FTR *ftrdata)
{
	int i = 0;

	if(ftrdata == NULL) return(NULL);
	if(ftrdata->nextftr != NULL) ftrdata->nextftr = free_ftr(ftrdata->nextftr);
	if(ftrdata->ftrcandidate != NULL) ftrdata->ftrcandidate = free_ftrcandidate(ftrdata->ftrcandidate);
	if(ftrdata->title != NULL) free(ftrdata->title);
	if(ftrdata->classlabels != NULL){
		for(i=0;i<ftrdata->numclasses;i++)
			if(ftrdata->classlabels[i] != NULL) free(ftrdata->classlabels[i]);
		free(ftrdata->classlabels);
	}
	if(ftrdata->samplesperclass != NULL) free(ftrdata->samplesperclass);
	if(ftrdata->apriori != NULL) free(ftrdata->apriori);
	if(ftrdata->featurelabels != NULL){
		for(i=0;i<ftrdata->numfeatures;i++)
			if(ftrdata->featurelabels[i] != NULL) free(ftrdata->featurelabels[i]);
		free(ftrdata->featurelabels);
	}
	return(NULL);
}