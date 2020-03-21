#include "stdafx.h"
#include "kespr_io.h"
#include "misc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct KESPRHEADER *read_kespr_header(char *filename, struct KESPRHEADER *khold)
{
	FILE *fptr = NULL;
	struct KESPRHEADER *kh = NULL, *khptr = NULL;
	static char *names[58] = {"HeaderType","Version","Length","InstanceUID","ClassUID","StudyDate",\
		"DateOfExposure","StudyTime","TimeOfExposure","Modality","Manufacturer","InstitutionID",\
		"RefPhysician","StationID","ScannerModel","OperatorID1","OperatorID2","PatientName",\
		"PatientID","Birthdate","PatientSex","BodyPart","DeviceSN","PlateID","SoftwareVersion",\
		"CalibrationDate","CalibrationTime","ScreenType","PhosphorType","Projection",\
		"ExamComments","Study","Series","Acquisition","Image","Orientation","Rows",\
		"Columns","PixelSpacing","BitsStored","HighBit","DataRep","KVP","Distance","mAs",\
		"ReadoutDate","ReadoutTime","Unused1","Unused2","ExamClass","Unused4","Unused5",\
		"Position","RelAnalogGain","CalibrationScreenSpeed","ScreenSizeScaleFactor",\
		"DrcLoBoost","DrcHiBoost"};
	int len = 58, i, sp = 0;
	int sizes[58] = {sizeof(kh->HeaderType),sizeof(kh->Version),sizeof(kh->Length),sizeof(kh->InstanceUID),\
		sizeof(kh->ClassUID),sizeof(kh->StudyDate),sizeof(kh->DateOfExposure),sizeof(kh->StudyTime),\
		sizeof(kh->TimeOfExposure),sizeof(kh->Modality),sizeof(kh->Manufacturer),sizeof(kh->InstitutionID),\
		sizeof(kh->RefPhysician),sizeof(kh->StationID),sizeof(kh->ScannerModel),sizeof(kh->OperatorID1),\
		sizeof(kh->OperatorID2),sizeof(kh->PatientName),sizeof(kh->PatientID),sizeof(kh->Birthdate),\
		sizeof(kh->PatientSex),sizeof(kh->BodyPart),sizeof(kh->DeviceSN),sizeof(kh->PlateID),\
		sizeof(kh->SoftwareVersion),sizeof(kh->CalibrationDate),sizeof(kh->CalibrationTime),\
		sizeof(kh->ScreenType),sizeof(kh->PhosphorType),sizeof(kh->Projection),sizeof(kh->ExamComments),\
		sizeof(kh->Study),sizeof(kh->Series),sizeof(kh->Acquisition),sizeof(kh->Image),sizeof(kh->Orientation),\
		sizeof(kh->Rows),sizeof(kh->Columns),sizeof(kh->PixelSpacing),sizeof(kh->BitsStored),sizeof(kh->HighBit),\
		sizeof(kh->DataRep),sizeof(kh->KVP),sizeof(kh->Distance),sizeof(kh->mAs),sizeof(kh->ReadoutDate),\
		sizeof(kh->ReadoutTime),sizeof(kh->Unused1),sizeof(kh->Unused2),sizeof(kh->ExamClass),sizeof(kh->Unused4),\
		sizeof(kh->Unused5),sizeof(kh->Position),sizeof(kh->RelAnalogGain),sizeof(kh->CalibrationScreenSpeed),\
		sizeof(kh->ScreenSizeScaleFactor),sizeof(kh->DrcLoBoost),sizeof(kh->DrcHiBoost)};
	char outstring[100] = {'\0'};

	kh = (struct KESPRHEADER *) calloc(1, sizeof(struct KESPRHEADER));
	if((fptr = fopen(filename, "rb")) == NULL){
		fprintf(stderr, "Error opening the file <%s> for reading!\n", filename);
		return(NULL);
	}
	fread(kh->rawheader, sizeof(kh->rawheader), 1, fptr);
	fclose(fptr);

	kh->num = len;
	for(i=0;i<len;i++){
		kh->names[i] = names[i];
		kh->sizes[i] = sizes[i];
		memset(outstring, 100, 0);
		memcpy(outstring, kh->rawheader + sp, sizes[i]);
		kh->values[i] = kh->rawheader + sp;
		//printf("%d %s %s\n", sp, names[i], outstring);
		sp += sizes[i];
	}

//	for(i=0;i<len;i++){
//		if(strcmp(kh->names[i], "PixelSpacing") == 0){
//			printf("pixelspacing = %f\n", atof(kh->values[i]));
//			break;
//		}
//	}

	kh->sourceimage = (char *) calloc(strlen(filename)+1, sizeof(char));
	strcpy(kh->sourceimage, filename);
	kh->image = get_basename(filename, NULL);
	if(khold == NULL) return(kh);

	khptr = khold;
	while(khptr != NULL){
		if(khptr->nextkesprheader == NULL){
			khptr->nextkesprheader = kh;
			return(khold);
		}
		khptr = khptr->nextkesprheader;
	}

	return(NULL);
}

struct KESPRHEADER *free_kesprheader(struct KESPRHEADER *kh)
{
	if(kh == NULL) return(NULL);
	if(kh->nextkesprheader != NULL) kh->nextkesprheader = free_kesprheader(kh->nextkesprheader);
	if(kh->rawheader != NULL) free(kh->rawheader);
	if(kh->sourceimage != NULL) free(kh->sourceimage);
	if(kh->image != NULL) free(kh->image);
	free(kh);
	return(NULL);
}

float get_kespr_pixelspacing(struct KESPRHEADER *kh)
{
	int i = 0;

	for(i=0;i<kh->num;i++){
		if(strcmp(kh->names[i], "PixelSpacing") == 0){
			return((float)atof(kh->values[i]));
			break;
		}
	}
	return(-1.0f);
}

int read_kesprimgset(char *filename, struct KESPRHEADER **kh)
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
			if((*kh = read_kespr_header(aline, NULL)) == NULL) return 1;
		}
		else{
			if((*kh = read_kespr_header(aline, *kh)) == NULL) return 1;
		}
		i++;
	}

	fclose(fptr);

	return 0;
}

/*
	HeaderType
	Version
	Length
	InstanceUID
	ClassUID
	StudyDate
	DateOfExposure
	StudyTime
	TimeOfExposure
	Modality
	Manufacturer
	InstitutionID
	RefPhysician
	StationID
	ScannerModel
	OperatorID1
	OperatorID2
	PatientName
	PatientID
	Birthdate
	PatientSex
	BodyPart
	DeviceSN
	PlateID
	SoftwareVersion
	CalibrationDate
	CalibrationTime
	ScreenType
	PhosphorType
	Projection
	ExamComments
	Study
	Series
	Acquisition
	Image
	Orientation
	Rows
	Columns
	PixelSpacing
	BitsStored
	HighBit
	DataRep
	KVP
	Distance
	mAs
	ReadoutDate
	ReadoutTime
	Unused1
	Unused2
	ExamClass
	Unused4
	Unused5
	Position
	RelAnalogGain
	CalibrationScreenSpeed
	ScreenSizeScaleFactor
	DrcLoBoost
	DrcHiBoost
*/