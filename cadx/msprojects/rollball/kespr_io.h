#ifndef _KesprIo_
#define _KesprIo_

struct KESPRHEADER{
	char rawheader[2048];
	char *sourceimage, *image;
	char *names[58];
	int num;
	int sizes[58];
	char *values[58];
	char HeaderType[16];				// "Image Header"
	char Version[16];					// "V0.0"
	char Length[16];					// 2048 + image size
	char InstanceUID[16];				// 0008,0014 Unique object instance.
	char ClassUID[16];					// 008,0016 Identifies class/type of object.
	char StudyDate[16];					// 0008,0020 The date the study is started.
	char DateOfExposure[16];			// 0008,0022 Date of patient x-ray exposure.
	char StudyTime[16];					// 0008,0030 The time the study is started.
	char TimeOfExposure[16];			// 0008,0032 Time of patient x-ray exposure.
	char Modality[16];					// 0008,0060 i.e. CR
	char Manufacturer[16];				// 0008,0070 i.e. KODAK
	char InstitutionID[16];				// 0008,0080 Name of hospital/institution
	char RefPhysician[16];				// 0008,0090 Patients referring physician(s).
	char StationID[16];					// 0008,1010 CR1, CR2 (hospital's ID number)
	char ScannerModel[16];				// 0008,1090 i.e. Single cassette / stacker
	char OperatorID1[16];				// 0008,1070 i.e. Tech ID
	char OperatorID2[16];				// 0008,1070 i.e. Tech ID
	char PatientName[32];				// 0010,0010 from KESPR for Pittsburgh.
	char PatientID[16];					// 0010,0020 hospital ID number for patient.
	char Birthdate[16];					// 0010,0030
	char PatientSex[16];				// 0010,0040
	char BodyPart[16];					// 0018,0015 body part examined.  at,ev
										//       skull,cspine,tspine,lspine,chest,
										//       clavicle,breast,abdomen,pelvis,hip,
										//       hand,foot,other
	char DeviceSN[16];					// 0018,1000
	char PlateID[16];					// 0018,1002 bar coded number on CR plate
	char SoftwareVersion[16];			// 0018,1020 Software version
	char CalibrationDate[16];			// 0018,1200 Date last change to device.
	char CalibrationTime[16];			// 0018,1201 Time of last change to device...
	char ScreenType[16];				// 0018,1260 label on detector screen.
	char PhosphorType[16];				// 018,1261 type of phosphor on screen.
	char Projection[16];				// 0018,5110 AP,PA,RL,LL,RLD,LLD,RLO,LLO,?x tbl
	char ExamComments[32];				// 0018,4000
	char Study[16];						// 0020,0010
	char Series[16];					// 0020,0011
	char Acquisition[16];				// 0020,0012 From single plate,
										//      unique per Patient ID
	char Image[16];						// 0020,0013 More than one per acquisition
										//      is allowed.
	char Orientation[16];				// 0020,0020 red/green (portrait/landscape)
										//      corresponding to stripe on cassette
										//      which will be top row of image on
										//      CRT and film).; (L,R,H,F,A,P)
	char Rows[16];						// 0028,0010
	char Columns[16];					// 0028,0011
	char PixelSpacing[16];				// 0028,0030 mm between pixel centers
	char BitsStored[16];				// 0028,0101 Bits per pixel
	char HighBit[16];					// 0028,0102 MSB.
	char DataRep[16];					// 0028,0103 Data rep (2's compl, unsigned)
	char KVP[16];						// 0018,0060 Portables: 50, 60, 70, 80, 85
	char Distance[16];					// 0018,1110 source to detector (in mm.)
	char mAs[16];						// 0018,1152 Portables: 1.25,1.5,2.5,3.2,50,80
	char ReadoutDate[16];				// i.e. Date plate was scanned by this KESPR.
	char ReadoutTime[16];				// i.e. Time plate was scanned.
	char Unused1[16];					// patient exposure 2
	char Unused2[16];					// cfg_get_ip_curve_mult()
	char ExamClass[16];					// exam_class
	char Unused4[16];					//
	char Unused5[16];					// ts_version
	char Position[16];					// Exposure index
	char RelAnalogGain[16];				// Exposure index
	char CalibrationScreenSpeed[16];	// Exposure index
	char ScreenSizeScaleFactor[16];		// Exposure index
	char DrcLoBoost[16];				// low density DRC boost factor
	char DrcHiBoost[16];				// hi  density DRC boost factor

	struct KESPRHEADER *nextkesprheader;
};

struct KESPRHEADER *read_kespr_header(char *filename, struct KESPRHEADER *khold);
float get_kespr_pixelspacing(struct KESPRHEADER *kh);
int read_kesprimgset(char *filename, struct KESPRHEADER **kh);
struct KESPRHEADER *free_kesprheader(struct KESPRHEADER *kh);
int get_kespr_rows(struct KESPRHEADER *kh);
int get_kespr_cols(struct KESPRHEADER *kh);
#endif