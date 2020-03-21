/*****************************************************************************
*************          include file of crhdr.h         ***********************
******************************************************************************/
/*
*MH****************************************************************************
*MH
*MH                COPYRIGHT EASTMAN KODAK COMPANY 1992
*MH                        ALL RIGHTS RESERVED
*MH    SCCS Information : @(#)kesprio.h	2.1 8/17/95 14:19:00
*MH****************************************************************************
*MH
*MHMN  Module Name  : crhdr.h
*MH
*MH    Purpose      :
*MH
*MH
*MH    Ref  Revision Date  By    Revision Notes
*MH    ---  -------------  ---   ----------------------------------------------
*MHMR    0  yy/mm/dd             Original coding
*MHMR    1  yy/mm/dd             First change
*MH     .7  7/16/92         rw   add HdrSize=2048, hdr in BigCrLut
*MH     1.0 12/8/93         rw   add exam_class
*MH****************************************************************************
*/
 
/*symbolic constant #defines*/
 
#define CrHdrVersion "V1.1"     /* 12/8/93 */
/***************************************************************************
    cr.h  V0.4  2/27/92
 
    Description:
    Modifications:
 
        rw      v0.3    add transition zone width (on a whim)
 
        rw      v0.4    add RelAnalogGain, CalScreenSpeed, SceenSizeScaleFactor
                                        for patient exposure
 
        ra      v0.5    add Position for
        rw      v0.6    add lut/patient exposure/ window&level...
        rw      v0.7    6/16/92  add lut, usm, patient exposure stuff
        rw      v1.1    add DrcLoBoost, HiBoost
 
    COPYRIGHT (c) 1992 BY
    EASTMAN KODAK COMPANY, ROCHESTER, NEW YORK.
    ALL RIGHTS RESERVED.
****************************************************************************/
#ifndef Cr_Hdr          /* for 1 level deep includes */
#define Cr_Hdr
#define HdrSize 2048
#define FL      16
#define FL2     (2*FL)
#define FieldLength     (FL-1)
#define FieldLength2 (2*FL-1)
 
/************************************************* AcrNemaTable (V0.6) *******/
typedef struct {
  char  CurveDimensions[FL];/* 50xx,0005 */
  char  nPoints[FL];        /* 50xx,0010 (4096 for 12 bit pixels */
  char  DataType[FL];       /* 50xx,0020 TSM LUT (tonescale mapping lut) */
  char  RefInstanceUID[FL]; /* 50xx,0050 UID of image to which this applies*/
  char  RefClassUID[FL];    /* 50xx,0055 Class = CR Image. */
  char  DataRep[FL];        /* 50xx,0103 0 = unsigned short integer */
  char  MinCoordValue[FL];  /* 50xx,0104         */
  char  MaxCoordValue[FL];  /* 50xx,0105         */
} AcrNemaTable;
 
 
 
/************************************************* ImageHdr ****************/
typedef struct {
  char  HeaderType[FL];     /* "Image Header" */
  char  Version[FL];        /* "V0.0"   */
  char  Length[FL];         /* 2048 + image size */
  char  InstanceUID[FL];    /* 0008,0014 Unique object instance. */
  char  ClassUID[FL];       /* 0008,0016 Identifies class/type of object. */
  char  StudyDate[FL];      /* 0008,0020 The date the study is started. */
  char  DateOfExposure[FL]; /* 0008,0022 Date of patient x-ray exposure.*/
  char  StudyTime[FL];      /* 0008,0030 The time the study is started. */
  char  TimeOfExposure[FL]; /* 0008,0032 Time of patient x-ray exposure.*/
  char  Modality[FL];       /* 0008,0060 i.e. CR */
  char  Manufacturer[FL];   /* 0008,0070 i.e. KODAK */
  char  InstitutionID[FL];  /* 0008,0080 Name of hospital/institution */
  char  RefPhysician[FL];   /* 0008,0090 Patients referring physician(s). */
  char  StationID[FL];      /* 0008,1010 CR1, CR2 (hospital's ID number) */
  char  ScannerModel[FL];   /* 0008,1090 i.e. Single cassette / stacker */
  char  OperatorID1[FL];    /* 0008,1070 i.e. Tech ID */
  char  OperatorID2[FL];    /* 0008,1070 i.e. Tech ID */
  char  PatientName[FL2];   /* 0010,0010 from KESPR for Pittsburgh. */
  char  PatientID[FL];      /* 0010,0020 hospital ID number for patient. */
  char  Birthdate[FL];      /* 0010,0030 */
  char  PatientSex[FL];     /* 0010,0040 */
  char  BodyPart[FL];       /* 0018,0015 body part examined.  at,ev
                                         skull,cspine,tspine,lspine,chest,
                                         clavicle,breast,abdomen,pelvis,hip,
                                         hand,foot,other */
  char  DeviceSN[FL];       /* 0018,1000 */
  char  PlateID[FL];        /* 0018,1002 bar coded number on CR plate */
  char  SoftwareVersion[FL];/* 0018,1020 Software version    */
  char  CalibrationDate[FL];/* 0018,1200 Date last change to device. */
  char  CalibrationTime[FL];/* 0018,1201 Time of last change to device... */
  char  ScreenType[FL];     /* 0018,1260 label on detector screen. */
  char  PhosphorType[FL];   /* 0018,1261 type of phosphor on screen. */
  char  Projection[FL];     /* 0018,5110 AP,PA,RL,LL,RLD,LLD,RLO,LLO,?x tbl*/
  char  ExamComments[FL2];  /* 0018,4000 */
  char  Study[FL];          /* 0020,0010 */
  char  Series[FL];         /* 0020,0011 */
  char  Acquisition[FL];    /* 0020,0012 From single plate,
                                         unique per Patient ID */
  char  Image[FL];          /* 0020,0013 More than one per acquisition
                                         is allowed. */
  char  Orientation[FL];    /* 0020,0020 red/green (portrait/landscape)
                                         corresponding to stripe on cassette
                                         which will be top row of image on
                                         CRT and film).; (L,R,H,F,A,P) */
  char  Rows[FL];           /* 0028,0010 */
  char  Columns[FL];        /* 0028,0011 */
  char  PixelSpacing[FL];   /* 0028,0030 mm between pixel centers */
  char  BitsStored[FL];     /* 0028,0101 Bits per pixel */
  char  HighBit[FL];        /* 0028,0102 MSB. */
  char  DataRep[FL];        /* 0028,0103 Data rep (2's compl, unsigned) */
 
  char  KVP[FL];            /* 0018,0060 Portables: 50, 60, 70, 80, 85 */
  char  Distance[FL];       /* 0018,1110 source to detector (in mm.) */
  char  mAs[FL];            /* 0018,1152 Portables: 1.25,1.5,2.5,3.2,50,80 */
  char  ReadoutDate[FL];    /* i.e. Date plate was scanned by this KESPR. */
  char  ReadoutTime[FL];    /* i.e. Time plate was scanned. */
 
  char  Unused1[FL];    /* patient exposure 2 */
  char  Unused2[FL];    /* cfg_get_ip_curve_mult() */
  char  ExamClass[FL];  /* exam_class */
  char  Unused4[FL];
  char  Unused5[FL];    /* ts_version */
 
  char  Position[FL];                           /* Exposure index */
  char  RelAnalogGain[FL];                      /* Exposure index */
  char  CalibrationScreenSpeed[FL];             /* Exposure index */
  char  ScreenSizeScaleFactor[FL];              /* Exposure index */
 
  char  DrcLoBoost[FL];       /* low density DRC boost factor */
  char  DrcHiBoost[FL];       /* hi  density DRC boost factor */
 
} ImageHdr;
 
/************************************************* LutHdr ****************/
typedef struct {
  char  HeaderType[FL];     /* "Lut Header" */
  char  Version[FL];        /* "V0.0"       */
  char  Length[FL];         /* 2048+ lut size */
  char  InstanceUID[FL];    /* 0008,0014 Unique object instance. */
  char  ClassUID[FL];       /* 0008,0016 Identifies class/type of object. */
 
  char  LeftPoint[FL];      /* start of region of interest */
  char  LPointOut[FL];      /* for hi-contrast tonescale */
  char  ModLPoint[FL];      /* hi_contrast and black_bone tonescale */
  char  ModLPointOut[FL];   /* hi_contrast tonescale */
  char  RightPoint[FL];     /* end of ROI */
  char  RPointOut[FL];      /* lin_cdf value @ RightPoint */
  char  EndPoint[FL];       /* hi_contrast tonescale, rejects bkgrnd direct x-rays */
 
  char  Threshold[FL];      /* to generate boost table for unsharp masking */
  char  ThreshOut[FL];      /* for hi-contrast tonescale */
  char  TransWidth[FL];     /* transition zone width of boost table for USM */
  char  LutStart[FL];       /* configurable tonescale value (from imagproc.dat */
  char  CdfPoint[FL];       /* black_bone tonescale */
 
  char  AverageDensity[FL]; /* configurable tonescale value (from imagproc.dat */
  char  ExposureIndex[FL];  /* X-ray exposure reaching phosphor screen */
  char  Beta[FL];           /* used to convert from log-to-linear space and back */
  char  Delta[FL];          /*   "" */
 
  char  Unused1[FL];
  char  Unused2[FL];
  char  Unused3[FL];
  char  Unused4[FL];
  char  Unused5[FL];
 
 
/* this is an AcrNemaTable.... */
  char  CurveDimensions[FL];/* 50xx,0005 */
  char  nPoints[FL];        /* 50xx,0010 (4096 for 12 bit pixels */
  char  DataType[FL];       /* 50xx,0020 TSM LUT (tonescale mapping lut) */
  char  RefInstanceUID[FL]; /* 50xx,0050 UID of image to which this applies*/
  char  RefClassUID[FL];    /* 50xx,0055 Class = CR Image. */
  char  DataRep[FL];        /* 50xx,0103 0 = unsigned short integer */
  char  MinCoordValue[FL];  /* 50xx,0104         */
  char  MaxCoordValue[FL];  /* 50xx,0105         */
 
/* V0.4 updates for patient exposure */
  char  ScreenType[FL];     /* 0018,1260 label on detector screen. */
  char  PhosphorType[FL];   /* 0018,1261 type of phosphor on screen. */
  char  RelAnalogGain[FL];           /* Exposure index */
  char  CalibrationScreenSpeed[FL];  /* Exposure index */
  char  ScreenSizeScaleFactor[FL];   /* Exposure index */
 
/* V0.6 */
  char  ByteOrder[FL];       /* SUN or PC */
  char  KernelSize[FL];      /* usm kernel size (int, 1...301, odd) */
  char  LoBoost[FL];         /* lo boost (float, 0...10)  */
  char  HiBoost[FL];         /* hi boost (float, 0...10)  */
  char  WindowWidth[FL];     /* user specified, (int, 0...4095) */
  char  WindowLevel[FL];     /* user specified, (int, 0...4095) */
  char  LutPreference[FL];   /* TSMLUT, hi-con, blk bone or linear */
  AcrNemaTable Tables[2];    /* filled in DataType => existence of trailer header */
} LutHdr;
 
/************************************************* CrHdr ****************/
typedef struct {
  ImageHdr  img;
  ImageHdr  mini;
  LutHdr    lut;
} CrHdr;                /* note: never used??? */
 
 
/******************************************** BigCrHdr (V0.6) ****************/
typedef union {
  ImageHdr  img;
  struct {
    char    LutOffset[1024];
    LutHdr  lut;
  } big;
  char  hdr[HdrSize];
} BigCrHdr;
 
#endif          /* Cr_Hdr */
/****************************************************************************
***********************       include file for hsd_typedefs.h ***************
*****************************************************************************/
/*
*MH****************************************************************
*MH
*MH             COPYRIGHT EASTMAN KODAK COMPANY 1992
*MH                     ALL RIGHTS RESERVED
*MH
*MH****************************************************************
*MH
*MHMN   Module Name:    hsd_typedefs.h
*MH
*MH     Purpose:    	Define HSD typedefs.
*MH			The macros and typedefs contained within should
*MH			not be product specific.  The intent of this
*MH			header file is that it can be used intact from
*MH			product to product without modification.
*MH
*MH
*MH     Revision Date   By      Revision Notes
*MH     -------------   ---     -----------------------------------
*MHMR	07/10/92	ajs	moved from dqcw_general.h
*MHMR	07/21/92	JER	moved OS_ERROR from errors.h
*MHMR	07/22/92	JER	added RETRY
*MH
*MH***************************************************************
*/

#ifndef HSD_TYPEDEFS_H
#define HSD_TYPEDEFS_H

#ifndef TRUE
#define TRUE    1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL    0
#endif

#ifndef NULLCHAR
#define NULLCHAR        '\0'
#endif

#ifndef EOL
#define EOL     "\n"
#endif

#ifndef SUCCESS
#define SUCCESS 0
#endif
#ifndef FAILURE
#define FAILURE 1
#endif
#ifndef RETRY
#define RETRY 	2
#endif

#ifndef ON
#define ON      1
#endif
#ifndef OFF
#define OFF     0
#endif

#ifndef YES
#define YES     1
#endif
#ifndef NO
#define NO      0
#endif


/*
 * file i/o constants
 */

#ifndef IO_ERROR
#define IO_ERROR        -1      /* open(), read(), write(), or close() error */
#endif

#ifndef OS_ERROR
#define OS_ERROR        (-1)    /* open(), read(), write(), or close() error */
#endif

#ifndef SEEK_END
#define SEEK_END        2       /* seek to end of file */
#endif

#ifndef READ_MODE
#define READ_MODE               "r"
#endif

#ifndef WRITE_MODE
#define WRITE_MODE              "w"
#endif

/*
 *
 * System Defines
 *
 */
 
#ifndef CHAR
#define	CHAR			char		/* 8 bit character 	*/
#endif

#ifndef BOOLEAN
#define	BOOLEAN			char		/* 8 bit int, boolean	*/
#endif

#ifndef BOOLFUNC
#define	BOOLFUNC		short		/* for function returns	*/
#endif

#ifndef UINT8
#define	UINT8			unsigned char	/* unsigned 8 bit int	*/
#endif

#ifndef INT8
#define	INT8			char		/* signed 8 bit int	*/
#endif

#ifndef UINT16
#define	UINT16			unsigned short	/* unsigned 16 bit int	*/
#endif

#ifndef INT16
#define	INT16			short		/* signed 16 bit int	*/
#endif

#ifndef UINT32
#define	UINT32			unsigned long	/* unsigned 32 bit int	*/
#endif

#ifndef INT32
#define	INT32			long		/* signed 32 bit int	*/
#endif

#ifndef FLOAT
#define	FLOAT			float		/* single precision	*/
#endif

#ifndef DOUBLE
#define	DOUBLE			double		/* double precision	*/
#endif

#ifndef VOID
#define	VOID			void		/* for void functions	*/
#endif

/* these are for compatibility with Xlib */

#ifndef XINT
#define	XINT			int		
#endif

#ifndef XUINT
#define	XUINT			unsigned int
#endif

#endif

/****************************************************************************
***********************       include file dqcw_general.h   *****************
*****************************************************************************/
/*
*MH****************************************************************
*MH
*MH             COPYRIGHT EASTMAN KODAK COMPANY 1992
*MH                     ALL RIGHTS RESERVED
*MH
*MH****************************************************************
*MH
*MHMN   Module Name:    dqcw_general.h
*MH
*MH     Purpose:    	Define general macros needed for DQCW
*MH
*MH
*MH     Revision Date   By      Revision Notes
*MH     -------------   ---     -----------------------------------
*MH
*MH***************************************************************
*/

#ifndef DQCW_GENERAL_H
#define DQCW_GENERAL_H

#define DQCW_MAX_EXAM_NAME_LEN    50
#define DQCW_ONE 1                    /* Self explanatory */
#define MAX_FILE_NAMELEN        255   /* max file namelen        */
#define TS_SS_FACTOR            9
#define DISPLAY_SS_FACTOR       3

/***************************************************************/
typedef enum{
        ROUTE_NORMAL,
        ROUTE_STAT
} Route_Priority;


/***************************************************************/
typedef enum{
        ACQUIRED,
        QC,
        REPROCESS,
        TURF_REP,
        ROUTE,
        DELETED
} Process_Mode;


/* 
 *  FIRST and LAST _BODY_PARTS must be first and last.  
 *  They can be used for range checking, but are not valid exam types.
*/
/***************************************************************/
typedef enum{                   /* new field, V5 and beyond... */
        FIRST_EXAM_CLASS = 15799,
        ADULT,
        PEDIATRIC,
        LAST_EXAM_CLASS
} Exam_Class;

/***************************************************************/
typedef enum{
	FIRST_BODY_PART = 15000,
        BP_OTHER,
	CHEST,
	SKULL,
	ABDOMEN,
	CSPINE,
	PELVIS,
        TSPINE,
	CLAVICLE,
        BREAST,
        LSPINE,
        HIP,
        EXTREMITY,              /* get rid of this */

        FOREARM,                /* new body parts V5 and beyond... */
        ANKLE,
        HUMERUS,
        HAND,
        WRIST,
        ELBOW,
        SHOULDER,
        FEMUR,
        FIBULA_TIBIA,
        FOOT,
        KNEE,
	PATTERN,
        PED_CHEST,
        PED_ABDOMEN,

	PORTABLE_CHEST,
	FACIAL_BONES,
	NASAL_BONES,		
	CRANIUM,
	THORAX,
	SPINE,
	ABDOMEN_2,
	JOINT,
	LONGBONE,

        EX1,			/* experimental body parts */
        EX2,
        EX3,
        EX4,


	LAST_BODY_PART
} Body_Part;
 
/***************************************************************/
typedef enum{
        PROJ_OTHER=15100,
        AP, 
        LATERAL, 
        RLD, 
        LLD, 
        XTABLE,
        PA,
        RL,
        LL,
        RPO,                            /* Right Posterior Oblique */
        LPO,                            /* Left Posterior Oblique  */
        RAO,                            /* Right Anterior Oblique  */
        LAO,                             /* Left Anterior Oblique   */
	SWIMMERS,
	SMV,
	ORBIT,
	WATERS,
	OBLIQUE				/* LLB added oblique handling */
} Projection; 

#define STR_PROJ_UNKNOWN "UNKNOWN PROJECTION"
#define	STR_PROJ_OTHER	"OTHER"
#define	STR_AP		"AP"
#define	STR_LATERAL 	"LATERAL"
#define	STR_RLD 	"RLD"
#define	STR_LLD		"LLD"
#define	STR_XTABLE	"XTABLE"
#define	STR_PA		"PA"
#define	STR_RL		"RL"
#define	STR_LL		"LL"
#define	STR_RLO		"RLO"
#define	STR_LLO		"LLO"


 
/***************************************************************/
typedef enum{ 
        LONGITUDINAL, 
        TRANSVERSAL 
} Plate_Orientation; 

#define	STR_ORIENT_LONG	"LONGITUDINAL"
#define STR_ORIENT_TRAN	"TRANSVERSE"

/***************************************************************/
typedef enum{
	POS_OTHER=15200,
        SUPINE,
        SEMI_ERECT,
        ERECT
} PatientPosition;

#define	STR_POS_OTHER	"OTHER"
#define	STR_SUPINE	"SUPINE"
#define	STR_SEMI_ERECT	"SEMIERECT"
#define	STR_ERECT	"ERECT"
#define STR_UNKNOWN     "UNKNOWN"             


/***************************************************************/
typedef enum{
        REGULAR,
        BLACKBONE,
        HIGH_CONTRAST,
        LINEAR,
        TONESCALE_OTHER
} TonescaleType;

#define	STR_REGULAR		"REGULAR"
#define	STR_BLACKBONE		"BLACKBONE"
#define	STR_HIGH_CONTRAST	"HIGH CONTRAST"
#define	STR_LINEAR		"LINEAR"
#define	STR_TONESCALE_OTHER	"OTHER"

/***************************************************************/
typedef enum{
	NO_CHANGE,
	RIGHT_1,
	RIGHT_2,
	RIGHT_3,
	BACK,
	RIGHT_1B,
	RIGHT_2B,
	RIGHT_3B
} Orient;

/***************************************************************/
/*
 * different configurations that an
 * image can be flipped
 */
typedef	enum{
	HORIZONTAL,		/* along the y axis */
	VERTICAL		/* along the x axis */
} Flip_Dir;

typedef enum{
	EXAM_TYPE_CHANGE,
	FLIP_HORIZONTAL,
	FLIP_VERTICAL,
	ROTATE_RIGHT,
	ROTATE_LEFT,
	USM_CHANGE,
	TONESCALE_CHANGE,
	WIDTH_LEVEL_CHANGE,
	REVERT_TONESCALE,
	QC_COMPLETE
} Turf_Request;

/*
 * Exam Information Record -
 * 	use this type for database access
 */
#define	CASSETTE_ID_LEN		16
#define	DQCW_IMAGE_KEY_LEN	64
#define	DQCW_EXAM_FIELD_LEN	16
#define	DQCW_EXAM_COMMENTS_LEN	32

typedef	struct{
	INT32	exam_id_key;
	CHAR	exam_name[ DQCW_MAX_EXAM_NAME_LEN ];

	INT32	scan_date_time;
	INT32	exposure_date_time;
	CHAR	cassette_id[ CASSETTE_ID_LEN + 1 ];
	CHAR	operator_id[ DQCW_EXAM_FIELD_LEN + 1 ];
	CHAR	exam_comments[ DQCW_EXAM_COMMENTS_LEN + 1 ];
	INT32	kvp;
	INT32	distance;
	FLOAT	mAs;
	CHAR	requisition_no[ DQCW_EXAM_FIELD_LEN +1 ];
	CHAR	scanner_id[ DQCW_EXAM_FIELD_LEN + 1 ];
	CHAR	patient_id[ DQCW_EXAM_FIELD_LEN + 1 ];

	CHAR	spare1[ DQCW_EXAM_FIELD_LEN + 1 ];
	CHAR	spare2[ DQCW_EXAM_FIELD_LEN + 1 ];
	CHAR	spare3[ DQCW_EXAM_FIELD_LEN + 1 ];
	CHAR	spare4[ DQCW_EXAM_FIELD_LEN + 1 ];
	CHAR	spare5[ DQCW_EXAM_FIELD_LEN + 1 ];
        CHAR    body_part_name [ DQCW_EXAM_FIELD_LEN + 1 ];
	
	Body_Part	body_part;
	Projection	projection;
	Plate_Orientation	plate_orientation;
	PatientPosition		position;
        INT32   print_format;

	INT32	sc_qc_version;
	Orient	orientation;
	Process_Mode	process_mode;
	INT32	version_count;
	CHAR	exam_lock[ DQCW_ONE ];
	
	INT32	plate_size;
	INT32	bits_pixel;
	INT32	bytes_pixel;
	INT32	image_transfer_length;	/* size of raw data in bytes */
	INT32	print_copies;
	Route_Priority	route_priority;

	INT32	ave_density;
	INT32	l_point_out;
	INT32	threshold;
	INT32	threshold_out;
	INT32	width;
	INT32	histo_min;
	INT32	histo_max;
	INT32	l_point;
	INT32	r_point;
	INT32	r_point_out;
	INT32	mod_l_point;
	INT32	mod_l_point_out;
	INT32	end_point;
	INT32	cdf_point;
	INT32	exposure_index;
	FLOAT	beta;
	FLOAT	delta;
	INT32	screen_speed;
	INT32	cal_screen_speed;
	FLOAT	analog_gain;
	FLOAT	screen_size_sf;
	FLOAT	sys_gain_factor;
	INT32	rows;
	INT32	columns;
	INT32	ssee_rows;
	INT32	ssee_cols;
	INT32	print_rows;
	INT32	print_cols;
	/*
	 * fill in as we go
	 */
}	Exam_Info;
#endif

/****************************************************************************
***************   Exam Stuff for intrepeting param files *******************
****************************************************************************/
#ifndef Lab_Exam_Hdr          /* for 1 level deep includes */
#define Lab_Exam_Hdr    

#define MaxNameLength   20
#define MaxBodyParts    (LAST_BODY_PART-FIRST_BODY_PART)
/*
#define MaxProjections  (LAO-PROJ_OTHER+1)
*/
#define MaxProjections  (OBLIQUE-PROJ_OTHER+1)
#define MaxExamClasses  2
#define MaxOrientations 2
#define MaxPositions	(ERECT-POS_OTHER+1)
/*
char ExamClasses[MaxExamClasses][MaxNameLength] = {
        "ADULT", "PEDIATRIC"};
char BodyParts[MaxBodyParts][MaxNameLength] = {
        "OTHER", "CHEST", "SKULL", "ABDOMEN", "CSPINE", "PELVIS",
        "TSPINE", "CLAVICLE", "BREAST", "LSPINE", "HIP", "EXTREMITY",
        "FOREARM", "ANKLE", "HUMERUS", "HAND", "WRIST", "ELBOW", "SHOULDER",
        "FEMUR", "FIBULA", "FOOT", "KNEE", "PATTERN",
        "PED_CHEST", "PED_ABDOMEN" , "PORTABLE_CHEST", "FACIAL_BONES", 
	"NASAL_BONES", "CRANIUM", "THORAX", "SPINE", "ABDOMEN_2",
	"JOINT", "LONGBONE"};
char Projections[MaxProjections][MaxNameLength] = {
        "OTHER", "AP", "LATERAL", "RLD", "LLD", "XTABLE",  "PA", "RL", "LL",
	"RPO", "LPO", "RAO", "LAO"};
*/
#endif
