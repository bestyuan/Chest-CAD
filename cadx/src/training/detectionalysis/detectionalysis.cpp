//=============================================================================
// File: detectionalysis.cpp
// Description: The code in this file implements a tool for the performance
// analysis of a Chest CAD algorithm using
// Free-response-Receiver-Operating-Characteristic analysis. The chest CAD
// algorithm has multiple stages at which the FROC analysis may be performed.
// This tool requires input of a set of Health Imaging Truth (.hit) groundtruth
// files and a set of Candidate Nodule (.can) files. The FROC analysis can
// then be done using a number of candidates per image or using a threshold
// of the candidate prabability (available for V1.1 .can files). The
// performance of the complete system may also be assessed by including a
// set of FTR files that contain classification markings for each candidate
// nodule at a set of APRIORI values for the classifier. Only definite and
// probable groundtruth markings are considered as truth, possible markings
// are discarded. In scoring detections, any detections above and beyond one
// that correspond to a truth region are not considered as false positive
// detections. A correspondence is attained when a detection (just a point
// coordinate) falls inside a ground truth region (a polygon of points).
// A single detection could cause multiple ground truths to generate true
// positives if correspondences are found. All images referenced by .hit,
// .can and optionally by .ftr files are considered in the performance
// evaluation whether or not all two (or three) sources of information exist
// for that image.
// Name: Michael Heath, Eastman Kodak Company
// Date: 06/26/2003
// Copyright: Eastman Kodak Company
//=============================================================================
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "hit_io.h"
#include "can_io.h"
#include "detection_io.h"
#include "groundtruth.h"
#include "score.h"
#include "ftr_io.h"
#include "kespr_io.h"
#include "misc.h"

int VERBOSE = 0;

int create_training_file(char *trainingfilename, struct Detection **detectionarray,
	int numdetections, struct GroundTruth **groundtrutharray, int numgroundtruth,
	int numfeatures, char **featurelabels, char **features, struct IndexLink *gtindexbase);

//=============================================================================
// Procedure: print_usage
// Purpose: This procedue provides a user with the command line arguments
// for the program.
// Name: Michael Heath, Eastman Kodak Company
// Date: 06/26/2003
//=============================================================================
static void print_usage(char *programname, char *version)
{
	printf("\n\n<USAGE> %s -truth filename.txt -can filename.txt\n", programname);
	printf("\t\t-img filename.txt [-h] [-v] [-ftr filename.txt]\n");
	printf("\t\t[-criteria VALUE] [-can_tpf VALUE] | [-can_fp VALUE]]\n");
	printf("\t\t[-o filename.txt]\n\n");

	printf("-truth filename.txt\tA text file containing an unordered\n");
	printf("\t\t\tlist of .hit files, one per line.\n");
	printf("-can filename.txt\tA text file containing an unordered\n");
	printf("\t\t\tlist of .can files, one per line.\n");
	printf("\t\t\tAll must be in either V1 or V1.1 format.\n");
	printf("-img filename.txt\tA text file containing an unordered\n");
	printf("\t\t\tlist of .img files, one per line.\n");
	printf("-ftr filename.txt\tA text file containing an unordered\n");
	printf("\t\t\tlist of .ftr files, one per line.\n");
	printf("-criteria VALUE\t\tThis option specifies the criteria for\n");
	printf("\t\t\tsweeping out the FROC curve(s). When the\n");
	printf("\t\t\t-ftr option is used the VALUE is automatically\n");
	printf("\t\t\tforced to be APRIORI. Otherwise a user can\n");
	printf("\t\t\toverride the default VALUE of CANNUMBER with\n");
	printf("\t\t\ta VALUE of COIN or PROBABILITY.\n");
	printf("-o filename.txt\t\tThe name of an output file which will\n");
	printf("\t\t\tcontain the performance evaluation\n");
	printf("\t\t\tresults in tab separated format.\n");
	printf("-h\t\t\tPrint usage information.\n");
	printf("-v\t\t\tRun in verbose mode.\n");
	printf("\nVersion: %s [%s]\n\n", programname, version);
	exit(1);
}



//=============================================================================
// Procedure: main
// Purpose: This is the main forcedure for the program. The description of
// the program is provided at the top of this file. The basic approach to the
// code is to read in Ground Truth information from .hit files and convert it
// to an abstract representation of groundtruth. Candidate detections are then
// read from .can files and are converted to abstract representations of
// detections. Optionally, FTR data (output from a classifier run at different
// APRIORI values) is read and used to expand the detections to include
// detections at different APRIORI values. The set of detections and truth
// is then quieried to find the composite set of images used in the performance
// evaluation. The desired criteria for sweeping out the FROC analysis are then
// identified. The data is then processed at each FROC criteria, each image is
// considered and the FROC data is generated. Finally, the FROC data is output
// to a screen or to a file.
// Name: Michael Heath, Eastman Kodak Company
// Date: 06/26/2003
// Modified for handling CAN V1.2 files on 01-30-2004 by Mike Heath
//=============================================================================
int main(int argc, char* argv[])
{
	FILE *fptr = NULL, *fptrtraining = NULL;
	struct ImageTruth *imagetruth = NULL;
    struct CanData *candata = NULL;
	struct Detection *detections = NULL, *justcandetections;
	struct Detection **detectionarray = NULL, **justcandetectionarray = NULL;
	int numdetections = 0, justcannumdetections = 0;
	struct IndexLink *detindex = NULL, *gtindex = NULL, *detindexbase = NULL,
		*gtindex1 = NULL, *gtindex2 = NULL, *gtindexbase = NULL, *detindexbase0 = NULL,
		*templink = NULL, *detected_definite = NULL, *detected_probable = NULL,
		*missed_definite = NULL, *missed_probable = NULL;
	char *canfilelist = NULL, *hitfilelist = NULL, *ftrfilelist = NULL, *criteria = NULL,
		*outputfilename = NULL, *imgfilelist = NULL;
	struct GroundTruth *groundtruth = NULL, **groundtrutharray = NULL;
	int numgroundtruth = 0;
	struct VUnionLL *unique_detection_filenames = NULL, *vunionptr = NULL,
		*unique_candidates = NULL, *vunionptr2 = NULL,
		*unique_groundtruth_filenames = NULL, *unique_filenames = NULL,
		*oldvunionptr2 = NULL, *holdvunionptr = NULL;
	int candidatenumber = 0, sum_tp = 0, sum_fp = 0, sum_fn = 0, num = 0, sum_wash = 0;
	struct FTR *ftrdata = NULL;
	float apriorinumber = 0.0;
	struct DetScore *detscore = NULL, *summarydetscore = NULL, *lastdetscore = NULL, *newdetscore = NULL;
	int total_detections = 0, i = 0;
	int numadditionalcriteria = 0;
	char additionalname[300] = {'\0'};
	char additionaltype[20] = {'\0'};
	char additionalop[20] = {'\0'};
	char additionalvalue[300] = {'\0'};
	char frocname[300] = {'\0'};
	char froctype[20] = {'\0'};
	char *op = NULL;
	float canversion = 0.0;
	char *version = "1.1 - 01.30.2004";
	struct KESPRHEADER *kh = NULL, *khptr = NULL;

	struct IndexLink *indexptr = NULL, *thisgtindex = NULL;
	union VUnion *vunion;
	int numtruthfields = 4;
	char *vunionptrtype = NULL;
	char *truthfields[4] = {"IMAGE", "TRUTHINDEX", "TRUTHSTATUS", "TRUTHDIAMETERCM"};
	char *thisresult = NULL;

	int num_size_distribution = 12;
	int gt_tp_size_distribution[12];
	int gt_all_size_distribution[12];
	float size_distribution[11] = {0.0f, 0.5f, 1.0f, 1.5f, 2.0f, 2.5f, 3.0f, 3.5f, 4.0f, 4.5f, 5.0f};
	char *size_distribution_names[12] = {"s<=0","0<s<=0.5","0.5<s<=1.0","1.0<s<=1.5","1.5<s<=2.0",\
		"2.0<s<=2.5","2.5<s<=3.0","3.0<s<=3.5","3.5<s<=4.0","4.0<s<=4.5","4.5<s<=5.0","s>5.0"};
	float tpf = 0.0, avgfp = 0.0, cancriteriavalue = -1.0;
	char *cancriteria = NULL;

	int numfeatures = 0;
	char **featurelabels = NULL;
	char **features = NULL;
	int totalnumcandidates = 0;

	char *trainingfilename = NULL;

	// Extract the command line parameters.
	for(i=1;i<argc;i++){
		if((strncmp(argv[i], "-truth", strlen("-truth")) == 0) && (i < (argc-1))){
			hitfilelist = argv[i+1];
			i++;
		}
		else if((strcmp(argv[i], "-can") == 0) && (i < (argc-1))){
			canfilelist = argv[i+1];
			i++;
		}
		else if((strncmp(argv[i], "-ftr", strlen("-ftr")) == 0) && (i < (argc-1))){
			ftrfilelist = argv[i+1];
			i++;
		}
		else if((strncmp(argv[i], "-img", strlen("-img")) == 0) && (i < (argc-1))){
			imgfilelist = argv[i+1];
			i++;
		}
		else if((strncmp(argv[i], "-criteria", strlen("-criteria")) == 0) && (i < (argc-1))){
			criteria = argv[i+1];
			i++;
		}
		else if((strncmp(argv[i], "-can_tpf", strlen("-can_tpf")) == 0) && (i < (argc-1))){
			cancriteria = "TPF";
			cancriteriavalue = (float)atof(argv[i+1]);
			i++;
		}
		else if((strncmp(argv[i], "-can_fp", strlen("-can_fp")) == 0) && (i < (argc-1))){
			cancriteria = "FP";
			cancriteriavalue = (float)atof(argv[i+1]);
			i++;
		}
		else if((strncmp(argv[i], "-o", strlen("-o")) == 0) && (i < (argc-1))){
			outputfilename = argv[i+1];
			i++;
		}
		else if((strncmp(argv[i], "-v", strlen("-v")) == 0)){
			VERBOSE = 1;
		}
		else if((strcmp(argv[i], "-trn") == 0) && (i < (argc-1))){
			trainingfilename = argv[i+1];
			i++;
		}
		else if((strncmp(argv[i], "-h", strlen("-h")) == 0)){
			print_usage(argv[0], version);
		}

	}
	if(hitfilelist == NULL) print_usage(argv[0], version);
	if(canfilelist == NULL) print_usage(argv[0], version);
	if(imgfilelist == NULL) print_usage(argv[0], version);

	//-------------------------------------------------------------------------
	// Read in the headers of the KESPR images.
	//-------------------------------------------------------------------------
	if(VERBOSE) printf("Reading in the KESPR image headers\n");
	if(read_kesprimgset(imgfilelist, &kh) == 1) return 1;
//	khptr = kh;
//	while(khptr != NULL){
//		printf("%s %f\n", khptr->image, get_kespr_pixelspacing(khptr));
//		khptr = khptr->nextkesprheader;
//	}

	//-------------------------------------------------------------------------
	// Read in the ground truth from all of the files whose names are
	// stored in the file named hitfilelist.
	//-------------------------------------------------------------------------
	if(VERBOSE) printf("Reading the HIT files...\n");
	if(read_hitset_v1point0(hitfilelist, &imagetruth) != 0) return 1;
	groundtruth = hit_v1point0_imagetruth_to_groundtruth(imagetruth, kh);
//	if(VERBOSE) printf("Freeing the KESPR image headers...\n");
//	kh = free_kesprheader(kh);
	imagetruth = free_imagetruth(imagetruth);
	groundtruth_to_groundtrutharray(groundtruth, &groundtrutharray, &numgroundtruth);
	printf("numgroundtruth = %d\n", numgroundtruth);

	if(VERBOSE) printf("Finding definite and probable truth regions...\n");
	gtindex1 = where_groundtrutharray(groundtrutharray, numgroundtruth, NULL, "TRUTHSTATUS", "eq", "definite", 1);
	gtindex2 = where_groundtrutharray(groundtrutharray, numgroundtruth, NULL, "TRUTHSTATUS", "eq", "probable", 1);
	gtindexbase = unionindex(gtindex1, gtindex2);
	gtindex1 = free_index(gtindex1);
	gtindex2 = free_index(gtindex2);

//	printf("Using definite and probable AND POSSIBLE truth regions...\n");
//	gtindexbase = where_groundtrutharray(groundtrutharray, numgroundtruth, NULL, "TRUTHSTATUS", "ne", "nothing", 1);

	//-------------------------------------------------------------------------
	// Read in all of the candidates from all of the files whose names
	// are stored in the file named canfilelist.
	//-------------------------------------------------------------------------
	if(VERBOSE) printf("Reading the CAN files...\n");
	canversion = get_cansetversion(canfilelist);
	if(VERBOSE) printf("The CAN files are version %.1f\n", canversion);
	if(canversion == 1.0f){
		if(read_canset_v1point0(canfilelist, &candata) != 0) return 1;
		if(VERBOSE) printf("Converting the candidates to detections...\n");
		justcandetections = can_v1point0_candata_to_detections(candata,
			&numfeatures, &featurelabels, &features, &totalnumcandidates);
		if(VERBOSE) printf("Deallocating the candidate data...\n");
		free_candata(&candata);
		detections_to_detectionarray(justcandetections, &justcandetectionarray, &justcannumdetections);
	}
	else if(canversion == 1.1f){
		if(read_canset_v1point1(canfilelist, &candata) != 0) return 1;
		if(VERBOSE) printf("Converting the candidates to detections...\n");
		justcandetections = can_v1point1_candata_to_detections(candata,
			&numfeatures, &featurelabels, &features, &totalnumcandidates);
		if(VERBOSE) printf("Deallocating the candidate data...\n");
		free_candata(&candata);
		detections_to_detectionarray(justcandetections, &justcandetectionarray, &justcannumdetections);
	}
	else if(canversion == 1.2f){
		if(read_canset_v1point2(canfilelist, &candata) != 0) return 1;
		if(VERBOSE) printf("Converting the candidates to detections...\n");
		justcandetections = can_v1point2_candata_to_detections(candata,
			&numfeatures, &featurelabels, &features, &totalnumcandidates);
		if(VERBOSE) printf("Deallocating the candidate data...\n");
		free_candata(&candata);
		detections_to_detectionarray(justcandetections, &justcandetectionarray, &justcannumdetections);
	}
	else return 1;

	if(trainingfilename != NULL){
		if(VERBOSE) printf("Creating the training file...\n");
		create_training_file(trainingfilename, justcandetectionarray, justcannumdetections,
			groundtrutharray, numgroundtruth, numfeatures, featurelabels, features, gtindexbase);
		return 0;
	}

	//=========================================================================
	// If we do not have an FTR file then we will use the justcandetections
	// as the detections and will set up the frocname that specifies the
	// criteria to threshold to sweep out the FROC curve.
	//=========================================================================
	if(ftrfilelist == NULL){

		char *st = NULL;

		if(criteria != NULL) strcpy(frocname, criteria);
		else strcpy(frocname, "CANNUMBER");

		make_uppercase(frocname);

		if(VERBOSE) printf("The FROC curve will be swept out by thresholding %s.\n", frocname);
		st = get_nvp_type_for_name(justcandetectionarray[0]->nvp, frocname);
		if(st == NULL){
			fprintf(stderr, "Error! The FROC search criteria (%s) is not valid!\n", frocname);
			return(1);
		}
		strcpy(froctype, st);

		numadditionalcriteria = 0;

		detections = justcandetections;
		numdetections = justcannumdetections;

		justcandetections = NULL;
		justcannumdetections = 0;
		detectionarray = justcandetectionarray;
		justcandetectionarray = NULL;
	}

	//=========================================================================
	// If we have an FTR file then we have labels for candidates and we need to
	// process the detections accordingly. In this case the FROC will be swept
	// out using the APRIORI values from the FTR files. Furthermore, only
	// detections with a "P" class for the "CLASS" attribute will be counted
	// as detections.
	//=========================================================================
	if(ftrfilelist != NULL){

		char *st = NULL;

		// Try to read an FTR file
		if(VERBOSE) printf("Reading the FTR files...\n");
		if(read_ftrset_v1point0(ftrfilelist, &ftrdata) != 0) return 1;

		if(VERBOSE) printf("Expanding the detection candidates with FTR data...\n");
		detections = ftr_v1point1_ftrdata_to_detections(ftrdata, justcandetectionarray, justcannumdetections);
		detections_to_detectionarray(detections, &detectionarray, &numdetections);

		if(VERBOSE) printf("Deallocating the FTR data...\n");
		ftrdata = free_ftr(ftrdata);

		justcandetections = free_detections(justcandetections);
		justcannumdetections = 0;
		free(justcandetectionarray);
		justcandetectionarray = NULL;

		strcpy(frocname, "APRIORI");
		st = get_nvp_type_for_name(detectionarray[0]->nvp, frocname);
		if(st == NULL){
			printf("Error! The search criteria (%s) is not valid!\n", frocname);
			return(1);
		}
		strcpy(froctype, st);

		numadditionalcriteria = 1;
		strcpy(additionalname, "CLASS");
		strcpy(additionalvalue, "P");

		st = get_nvp_type_for_name(detectionarray[0]->nvp, additionalname);
		if(st == NULL){
			printf("Error! The search criteria (%s) is not valid!\n", additionalname);
			return(1);
		}
		strcpy(additionaltype, st);

		strcpy(additionalop, "eq");

		if(VERBOSE) printf("The FROC curve will be swept out by thresholding %s\n", frocname);
		if(VERBOSE)
			printf("Only candidates with %s equal to %s will be used.\n", additionalname, additionalvalue);

	}

	//-------------------------------------------------------------------------
	// Determine the list of images that will be used to compute each FROC
	// point. Images references in both .can and .hit files will be used.
	// Thus, if a can file does not have a corresponding hit file, any
	// detections that are used from the can file will be false positive
	// detections. If a .hit file does not have a .can file than any truths that
	// are used from that file will be counted as false negatives.
	//-------------------------------------------------------------------------
	if(VERBOSE) printf("Finding the list of unique detection (can) filenames.\n");
	unique_detection_filenames = sorted_unique_values_for_name(detectionarray,
		numdetections, "IMAGE", "string");

	if(VERBOSE) printf("Finding the list of unique truth filenames.\n");
	unique_groundtruth_filenames = sorted_unique_values_for_name(groundtrutharray,
		numgroundtruth, "IMAGE", "string");

	if(VERBOSE) printf("Combining the detection (can) and truth filenames.\n");
	unique_filenames = unionvunions(unique_detection_filenames, unique_groundtruth_filenames, "string");

	// Free the linked list of vunions that stored the detection image filenames
	unique_detection_filenames = free_vunion(unique_detection_filenames, "string");
	unique_detection_filenames = NULL;

	// Free the linked list of vunions that stored the truth image filenames
	unique_groundtruth_filenames = free_vunion(unique_groundtruth_filenames, "string");
	unique_groundtruth_filenames = NULL;

	//-------------------------------------------------------------------------
	// Determine the list of values at which FROC evaluations will be done.
	//-------------------------------------------------------------------------
	if(VERBOSE) printf("Finding the list of unique FROC points.\n");
	unique_candidates = sorted_unique_values_for_name(detectionarray,
		numdetections, frocname, froctype);

	//-------------------------------------------------------------------------
	// Loop throuch each FROC criteria. Inside that loop, the images are looped
	// through to calculate each FROC point.
	//-------------------------------------------------------------------------
	if(VERBOSE) printf("Starting to loop through FROC criteria\n");

	vunionptr2 = unique_candidates;
	unique_candidates = NULL;

	if(strcmp(frocname, "CANNUMBER") == 0) op = "le";
	if(strcmp(frocname, "PROBABILITY") == 0) op = "ge";
	if(strcmp(frocname, "COIN") == 0) op = "ge";
	if(strcmp(frocname, "APRIORI") == 0) op = "eq";

	holdvunionptr = vunionptr2;

	if((strcmp(op, "ge") == 0) || (strcmp(op, "gt") == 0)){
		vunionptr2 = NULL;
		vunionptr2 = reversevunions(holdvunionptr, froctype);
		holdvunionptr = free_vunion(holdvunionptr, froctype);
		holdvunionptr = vunionptr2;
	}

	vunionptr2 = holdvunionptr;
	while(vunionptr2 != NULL){

		if(strcmp(froctype, "string") == 0){
			detindexbase = where_detectionarray(detectionarray, numdetections, NULL,
				frocname, op, vunionptr2->vunion.string_value, 1);
		}
		if(strcmp(froctype, "float") == 0){
			detindexbase = where_detectionarray(detectionarray, numdetections, NULL,
				frocname, op, vunionptr2->vunion.floatvalue, 1);
		}
		if(strcmp(froctype, "int") == 0){
			detindexbase = where_detectionarray(detectionarray, numdetections, NULL,
				frocname, op, vunionptr2->vunion.intvalue, 1);
		}

		if(numadditionalcriteria == 1){

			detindexbase0 = detindexbase;
			detindexbase = NULL;

			detindexbase = where_detectionarray(detectionarray, numdetections, detindexbase0,
					additionalname, additionalop, additionalvalue, 0);
			detindexbase0 = free_index(detindexbase0);

		}

		total_detections = indexlistlength(detindexbase);

		num = sum_tp = sum_fp = sum_fn = sum_wash = 0;

		newdetscore = (struct DetScore *) calloc(1, sizeof(struct DetScore));
		if(summarydetscore == NULL) summarydetscore = newdetscore;
		else lastdetscore->nextdetscore = newdetscore;
		lastdetscore = newdetscore;

		vunionptr = unique_filenames;

		while(vunionptr != NULL){

			gtindex = where_groundtrutharray(groundtrutharray, numgroundtruth, gtindexbase, "IMAGE", "eq", vunionptr->vunion.string_value, 0);

			detindex = where_detectionarray(detectionarray, numdetections, detindexbase, "IMAGE", "eq", vunionptr->vunion.string_value, 0);

			detscore = score_detection_groundtruth(groundtrutharray, gtindex, detectionarray, detindex);

			// Save the indices of the ground truth true positives.
			templink = unionindex(newdetscore->gtindex_tp, detscore->gtindex_tp);
			newdetscore->gtindex_tp = free_index(newdetscore->gtindex_tp);
			newdetscore->gtindex_tp = templink;
			templink = NULL;

			// Save the indices of the ground truth false negatives.
			templink = unionindex(newdetscore->gtindex_fn, detscore->gtindex_fn);
			newdetscore->gtindex_fn = free_index(newdetscore->gtindex_fn);
			newdetscore->gtindex_fn = templink;
			templink = NULL;

			// Save the indices of the detection true positives.
			templink = unionindex(newdetscore->detindex_tp, detscore->detindex_tp);
			newdetscore->detindex_tp = free_index(newdetscore->detindex_tp);
			newdetscore->detindex_tp = templink;
			templink = NULL;

			// Save the indices of the detection false positives.
			templink = unionindex(newdetscore->detindex_fp, detscore->detindex_fp);
			newdetscore->detindex_fp = free_index(newdetscore->detindex_fp);
			newdetscore->detindex_fp = templink;
			templink = NULL;

			// Save the indices of the detection washes.
			templink = unionindex(newdetscore->detindex_wash, detscore->detindex_wash);
			newdetscore->detindex_wash = free_index(newdetscore->detindex_wash);
			newdetscore->detindex_wash = templink;
			templink = NULL;

			sum_tp += detscore->num_tp;
			sum_fp += detscore->num_fp;
			sum_fn += detscore->num_fn;
			sum_wash += detscore->num_wash;
			num++;

			detindex = free_index(detindex);
			gtindex = free_index(gtindex);

			detscore = free_detscore(detscore);

			vunionptr = vunionptr->nextvunion;

		}
		vunionptr = NULL;

		if(total_detections != (sum_tp+sum_fp+sum_wash)) return 1;

		vunionptr2 = vunionptr2->nextvunion;
	}
	vunionptr2 = NULL;

	//=========================================================================
	// Write the results out to either the screen or to a file.
	//=========================================================================
	if(outputfilename != NULL){
		if((fptr = fopen(outputfilename, "a")) == NULL){
			fprintf(stderr, "Error opening the file %s for writing!\n\n", outputfilename);
			exit(1);
		}
	}
	else fptr = stdout;

	// Echo the command line to the output.
	for(i=0;i<argc;i++) fprintf(fptr, "%s ", argv[i]);
	fprintf(fptr, "\n");

	// Print the column headings for the output statistics.
	fprintf(fptr, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s", frocname,
		"SUM_TP", "SUM_FP", "SUM_FN", "SUM_WASH", "FP/IM", "TPF", "DET_definite", "DET_probable",
		"MISS_definite", "MISS_probable");
	for(i=0;i<num_size_distribution;i++) fprintf(fptr, "\tTP:%s", size_distribution_names[i]);
	for(i=0;i<num_size_distribution;i++) fprintf(fptr, "\tNUM:%s", size_distribution_names[i]);
	fprintf(fptr, "\n");

	// Print the output statistics for each FROC point.
	newdetscore = summarydetscore;
	vunionptr2 = holdvunionptr;
	while(newdetscore != NULL){

		char *frocstring = NULL;

		frocstring = vunion_to_string(vunionptr2, froctype);

		// Determine the number of probable and the number of definite truth regions that were detected and missed.
		detected_definite = where_groundtrutharray(groundtrutharray, numgroundtruth, newdetscore->gtindex_tp, "TRUTHSTATUS", "eq", "definite", 0);
		detected_probable = where_groundtrutharray(groundtrutharray, numgroundtruth, newdetscore->gtindex_tp, "TRUTHSTATUS", "eq", "probable", 0);
		missed_definite = where_groundtrutharray(groundtrutharray, numgroundtruth, newdetscore->gtindex_fn, "TRUTHSTATUS", "eq", "definite", 0);
		missed_probable = where_groundtrutharray(groundtrutharray, numgroundtruth, newdetscore->gtindex_fn, "TRUTHSTATUS", "eq", "probable", 0);
		int ii = 0;
		// Determine the hits and misses as a function of ground truth size.
		for(i=0;i<num_size_distribution;i++) gt_tp_size_distribution[i] = 0;
		indexptr = newdetscore->gtindex_tp;
		while(indexptr != NULL){
			vunion = get_nvp_value_for_name(groundtrutharray[indexptr->index]->nvp, "TRUTHDIAMETERCM");
			vunionptrtype = get_nvp_type_for_name(groundtrutharray[indexptr->index]->nvp, "TRUTHDIAMETERCM");
			if(strcmp(vunionptrtype, "float") == 0){
				for(i=0;i<num_size_distribution;i++){
					if(i < (num_size_distribution-1)){
						if(vunion->floatvalue <= size_distribution[i]){
							gt_tp_size_distribution[i]++;
							ii++;
							break;
						}
					}
					else{
						if(vunion->floatvalue > size_distribution[i-1]){
							gt_tp_size_distribution[i]++;
							ii++;
							break;
						}
					}
				}
			}
			indexptr = indexptr->nextindex;
		}
		indexptr = NULL;
		templink = unionindex(newdetscore->gtindex_tp, newdetscore->gtindex_fn);

		printf("[TP=%d FN=%d, UNION=%d ii=%d]\n", indexlistlength(newdetscore->gtindex_tp),
			indexlistlength(newdetscore->gtindex_fn),
			indexlistlength(templink), ii);

		for(i=0;i<num_size_distribution;i++) gt_all_size_distribution[i] = 0;
		indexptr = templink;
		while(indexptr != NULL){
			vunion = get_nvp_value_for_name(groundtrutharray[indexptr->index]->nvp, "TRUTHDIAMETERCM");
			vunionptrtype = get_nvp_type_for_name(groundtrutharray[indexptr->index]->nvp, "TRUTHDIAMETERCM");
			if(strcmp(vunionptrtype, "float") == 0){
				for(i=0;i<num_size_distribution;i++){
					if(i < (num_size_distribution-1)){
						if(vunion->floatvalue <= size_distribution[i]){
							gt_all_size_distribution[i]++;
							break;
						}
					}
					else{
						if(vunion->floatvalue > size_distribution[i-1]){
							gt_all_size_distribution[i]++;
							break;
						}
					}
				}
			}
			indexptr = indexptr->nextindex;
		}
		indexptr = NULL;
		templink = free_index(templink);

		fprintf(fptr, "%s\t%d\t%d\t%d\t%d\t%f\t%f\t%d\t%d\t%d\t%d", frocstring, indexlistlength(newdetscore->gtindex_tp),
			indexlistlength(newdetscore->detindex_fp), indexlistlength(newdetscore->gtindex_fn),
			indexlistlength(newdetscore->detindex_wash),
			float(indexlistlength(newdetscore->detindex_fp)) / float(num),
			float(indexlistlength(newdetscore->gtindex_tp)) /
			(float(indexlistlength(newdetscore->gtindex_tp)) + float(indexlistlength(newdetscore->gtindex_fn))),
			indexlistlength(detected_definite), indexlistlength(detected_probable),
			indexlistlength(missed_definite), indexlistlength(missed_probable));

		for(i=0;i<num_size_distribution;i++) fprintf(fptr, "\t%d", gt_tp_size_distribution[i]);
		for(i=0;i<num_size_distribution;i++) fprintf(fptr, "\t%d", gt_all_size_distribution[i]);
		fprintf(fptr, "\n");

		detected_definite = free_index(detected_definite);
		detected_probable = free_index(detected_probable);
		missed_definite = free_index(missed_definite);
		missed_probable = free_index(missed_probable);

		newdetscore = newdetscore->nextdetscore;
		vunionptr2 = vunionptr2->nextvunion;

		free(frocstring);
		frocstring = NULL;
	}
	vunionptr2 = NULL;
	newdetscore = NULL;

	// Write the groundtruth to the file.
//	fprintf(fptr, "The groundtruth regions that were used in the FROC analysis are...\n");
//	print_groundtrutharray(groundtrutharray, gtindexbase, fptr);


	// Write out a comprehensive list of FROC component data.
	for(i=0;i<numtruthfields;i++){
		if(i != 0) fprintf(fptr, "\t");
		fprintf(fptr, "%s", truthfields[i]);
	}
	vunionptr2 = holdvunionptr;
	while(vunionptr2 != NULL){
		if(strcmp(froctype, "string") == 0) fprintf(fptr, "\t%s", vunionptr2->vunion.string_value);
		if(strcmp(froctype, "float") == 0) fprintf(fptr, "\t%f", vunionptr2->vunion.floatvalue);
		if(strcmp(froctype, "int") == 0) fprintf(fptr, "\t%d", vunionptr2->vunion.intvalue);
		vunionptr2 = vunionptr2->nextvunion;
	}
	fprintf(fptr, "\n");
	vunionptr2 = NULL;

	// Go through all of the truths, and through all of the FROC operating points for each to
	// determine if the truth generated a TP or a FP at that FROC point.
	indexptr = gtindexbase;
	while(indexptr != NULL){
		for(i=0;i<numtruthfields;i++){
			if(i != 0) fprintf(fptr, "\t");
			vunion = get_nvp_value_for_name(groundtrutharray[indexptr->index]->nvp, truthfields[i]);
			vunionptrtype = get_nvp_type_for_name(groundtrutharray[indexptr->index]->nvp, truthfields[i]);
			if(vunionptrtype != NULL){
				if(strcmp(vunionptrtype, "float") == 0) fprintf(fptr, "%f", vunion->floatvalue);
				if(strcmp(vunionptrtype, "int") == 0) fprintf(fptr, "%d", vunion->intvalue);
				if(strcmp(vunionptrtype, "string") == 0) fprintf(fptr, "%s", vunion->string_value);
			}
		}
		// Go through all of the FROC operating points.
		newdetscore = summarydetscore;
		while(newdetscore != NULL){
			thisgtindex = newdetscore->gtindex_tp;
			thisresult = "UNKNOWN";
			// Determine if this truth was a TP at this FROC operating point.
			while(thisgtindex != NULL){
				if(thisgtindex->index == indexptr->index){
					thisresult = "TP";
					break;
				}
				thisgtindex = thisgtindex->nextindex;
			}
			// Determine if this truth was a FN at this operating point.
			if(strcmp(thisresult, "UNKNOWN") == 0){
				thisgtindex = newdetscore->gtindex_fn;
				thisresult = "UNKNOWN";
				while(thisgtindex != NULL){
					if(thisgtindex->index == indexptr->index){
						thisresult = "FN";
						break;
					}
					thisgtindex = thisgtindex->nextindex;
				}
			}
			fprintf(fptr, "\t%s", thisresult);
			thisresult = NULL;
			newdetscore = newdetscore->nextdetscore;
		}
		fprintf(fptr, "\n");
		indexptr = indexptr->nextindex;
	}

	// Close the output file.
	if(fptr != stdout) fclose(fptr);

	/**************************************************************************
	* Create CAN files for each image. There will be one CAN file produced for
	* the true positives, another for the false positives, and yet another for
	* the false negatives.
	**************************************************************************/
	if((cancriteria != NULL) && (cancriteriavalue > 0.0)){

		char output_filename[300] = {'\0'};
		struct IndexLink *templink_tp = NULL, *templink_fn = NULL, *templink_fp = NULL, *templinkptr = NULL;
		int numtowrite, rows, cols;
		int summary_tp = 0, summary_fp = 0, summary_fn = 0, summarycount = 0;

		// Find the actual operating point that is closest to the desired operating point.
		newdetscore = summarydetscore;
		detscore = newdetscore;
		while(newdetscore != NULL){
			tpf = (float)indexlistlength(newdetscore->gtindex_tp) /
				((float)indexlistlength(newdetscore->gtindex_tp) +
				(float)indexlistlength(newdetscore->gtindex_fn));
			avgfp = (float)indexlistlength(newdetscore->detindex_fp) / (float)num;

			printf("Trying TPF = %f, FP = %f (%f)\n", tpf, avgfp, cancriteriavalue);

			if(strcmp(cancriteria, "TPF") == 0){
				if(tpf < cancriteriavalue) detscore = newdetscore;
			}
			else if(strcmp(cancriteria, "FP") == 0){
				if(avgfp < cancriteriavalue) detscore = newdetscore;
			}
			newdetscore = newdetscore->nextdetscore;
		}
		tpf = (float)indexlistlength(detscore->gtindex_tp) /
				((float)indexlistlength(detscore->gtindex_tp) +
				(float)indexlistlength(detscore->gtindex_fn));
		avgfp = (float)indexlistlength(detscore->detindex_fp) / (float)num;

		printf("The FROC point for CAN file generation is TPF=%f FP=%f and the target was (%s = %f)\n",
			tpf, avgfp, cancriteria, cancriteriavalue);

		// Go through all of the filenames.
		vunionptr = unique_filenames;
		while(vunionptr != NULL){

			// * Find all of the ground truth and all of the detections associated with this image.
			gtindex = where_groundtrutharray(groundtrutharray, numgroundtruth, NULL, "IMAGE", "eq", vunionptr->vunion.string_value, 1);
			detindex = where_detectionarray(detectionarray, numdetections, detscore->detindex_fp, "IMAGE", "eq", vunionptr->vunion.string_value, 0);
			if(detindex == NULL)
				detindex = where_detectionarray(detectionarray, numdetections, NULL, "IMAGE", "eq", vunionptr->vunion.string_value, 1);

			templink_tp = intersectionindex(gtindex, detscore->gtindex_tp);
			templink_fn = intersectionindex(gtindex, detscore->gtindex_fn);
			templink_fp = intersectionindex(detindex, detscore->detindex_fp);

			printf("TP = %d  FN = %d  FP = %d\n",
				indexlistlength(templink_tp), indexlistlength(templink_fn), indexlistlength(templink_fp));

			rows = 0;
			cols = 0;

			if(templink_tp != NULL){
				if(groundtrutharray[templink_tp->index]->rows != 0) rows = groundtrutharray[templink_tp->index]->rows;
				if(groundtrutharray[templink_tp->index]->rows != 0) cols = groundtrutharray[templink_tp->index]->cols;
			}
			if(templink_fn != NULL){
				if(groundtrutharray[templink_fn->index]->rows != 0) rows = groundtrutharray[templink_fn->index]->rows;
				if(groundtrutharray[templink_fn->index]->rows != 0) cols = groundtrutharray[templink_fn->index]->cols;
			}
			if(templink_fp != NULL){
				if(detectionarray[templink_fp->index]->rows != 0) rows = detectionarray[templink_fp->index]->rows;
				if(detectionarray[templink_fp->index]->rows != 0) cols = detectionarray[templink_fp->index]->cols;
			}

			// Write the list of true positives to a CAN format file.
			numtowrite = indexlistlength(templink_tp);
			summary_tp += numtowrite;
			strcpy(output_filename, vunionptr->vunion.string_value);
			sprintf(output_filename + strlen(output_filename)-4, "_CANSCORE_%s%f_TPLIST.can",
				cancriteria, cancriteriavalue);
			fptr = fopen(output_filename, "w");

			fprintf(fptr, "#^CAN V1.1\n");
			fprintf(fptr, "xraySourceID: %s\n", vunionptr->vunion.string_value);
			fprintf(fptr, "imageSize: %d %d\n", cols, rows);
			fprintf(fptr, "numCandidates: %d\n", numtowrite);
			fprintf(fptr, "typeRegion: m\n");
			fprintf(fptr, "numFeatures: 0\n");
			fprintf(fptr, "flabels: \n");
			fprintf(fptr, "endHeader:\n");

			templinkptr = templink_tp;
			i = 0;
			while(templinkptr != NULL){
				fprintf(fptr, "candidateIndex: %d\n", i);
				fprintf(fptr, "groundtruth: unknown\n");
				fprintf(fptr, "probability: 0\n");
				fprintf(fptr, "ftrVector: \n");
				fprintf(fptr, "centroid: %d %d\n", (int)groundtrutharray[templinkptr->index]->centroid.x,
					(int)groundtrutharray[templinkptr->index]->centroid.y);
				fprintf(fptr, "nPoints: 0\n");
				fprintf(fptr, "endCandidate:\n");
				i++;
				templinkptr = templinkptr->nextindex;
			}
			if(templink_tp != NULL) templink_tp = free_index(templink_tp);
			fclose(fptr);

			// Write the list of false negatives to a CAN format file.
			numtowrite = indexlistlength(templink_fn);
			summary_fn += numtowrite;
			strcpy(output_filename, vunionptr->vunion.string_value);
			sprintf(output_filename + strlen(output_filename)-4, "_CANSCORE_%s%f_FNLIST.can",
				cancriteria, cancriteriavalue);
			fptr = fopen(output_filename, "w");

			fprintf(fptr, "#^CAN V1.1\n");
			fprintf(fptr, "xraySourceID: %s\n", vunionptr->vunion.string_value);
			fprintf(fptr, "imageSize: %d %d\n", cols, rows);
			fprintf(fptr, "numCandidates: %d\n", numtowrite);
			fprintf(fptr, "typeRegion: m\n");
			fprintf(fptr, "numFeatures: 0\n");
			fprintf(fptr, "flabels: \n");
			fprintf(fptr, "endHeader:\n");

			templinkptr = templink_fn;
			i = 0;
			while(templinkptr != NULL){
				fprintf(fptr, "candidateIndex: %d\n", i);
				fprintf(fptr, "groundtruth: unknown\n");
				fprintf(fptr, "probability: 0\n");
				fprintf(fptr, "ftrVector: \n");
				fprintf(fptr, "centroid: %d %d\n", (int)groundtrutharray[templinkptr->index]->centroid.x,
					(int)groundtrutharray[templinkptr->index]->centroid.y);
				fprintf(fptr, "nPoints: 0\n");
				fprintf(fptr, "endCandidate:\n");
				i++;
				templinkptr = templinkptr->nextindex;
			}
			if(templink_fn != NULL) templink_fn = free_index(templink_fn);
			fclose(fptr);

			// Write the list of false positives to a CAN format file.
			numtowrite = indexlistlength(templink_fp);
			summary_fp += numtowrite;
			strcpy(output_filename, vunionptr->vunion.string_value);
			sprintf(output_filename + strlen(output_filename)-4, "_CANSCORE_%s%f_FPLIST.can",
				cancriteria, cancriteriavalue);
			fptr = fopen(output_filename, "w");

			fprintf(fptr, "#^CAN V1.1\n");
			fprintf(fptr, "xraySourceID: %s\n", vunionptr->vunion.string_value);
			fprintf(fptr, "imageSize: %d %d\n", cols, rows);
			fprintf(fptr, "numCandidates: %d\n", numtowrite);
			fprintf(fptr, "typeRegion: m\n");
	        fprintf(fptr, "numFeatures: 0\n");
			fprintf(fptr, "flabels: \n");
			fprintf(fptr, "endHeader:\n");

			templinkptr = templink_fp;
			i = 0;
			while(templinkptr != NULL){
				fprintf(fptr, "candidateIndex: %d\n", i);
				fprintf(fptr, "groundtruth: unknown\n");
				fprintf(fptr, "probability: 0\n");
				fprintf(fptr, "ftrVector: \n");
				fprintf(fptr, "centroid: %d %d\n", (int)detectionarray[templinkptr->index]->point.x,
					(int)detectionarray[templinkptr->index]->point.y);
				fprintf(fptr, "nPoints: 0\n");
				fprintf(fptr, "endCandidate:\n");
				i++;
				templinkptr = templinkptr->nextindex;
			}
			if(templink_fp != NULL) templink_fp = free_index(templink_fp);
			fclose(fptr);

			summarycount++;
			vunionptr = vunionptr->nextvunion;
		}

		printf("Observed TPF = %f  FP = %f\n", (float)summary_tp / (float)(summary_tp + summary_fn),
			(float)summary_fp / (float)summarycount);
	}


	// Free the array of feature vectors (array of strings)
	for(i=0;i<totalnumcandidates;i++) if(features[i] != NULL) free(features[i]);
	free(features);
	features = NULL;
	for(i=0;i<numfeatures;i++) if(featurelabels[i] != NULL) free(featurelabels[i]);
	free(featurelabels);
	featurelabels = NULL;
	numfeatures = 0;

	// Free the linked list of vunions that stored the FROC criteria
	holdvunionptr = free_vunion(holdvunionptr, froctype);
	holdvunionptr = NULL;

	// Free the linked list of vunions that stored the image filenames
	unique_filenames = free_vunion(unique_filenames, "string");
	unique_filenames = NULL;

	// Free the linked list of detection scores.
	if(VERBOSE) printf("Deallocating the FROC data...\n");
	summarydetscore = free_detscore(summarydetscore);

	// Free the groundtruth.
	if(VERBOSE) printf("Deallocating the ground truth data...\n");
	groundtruth = free_groundtruth(groundtruth);
	free(groundtrutharray);
	groundtrutharray = NULL;

	// Free the detections.
    detections = free_detections(detections);
	if(VERBOSE) printf("Deallocating the detection data...\n");
	free(detectionarray);
	detectionarray = NULL;

	detindexbase = free_index(detindexbase);
	gtindexbase = free_index(gtindexbase);

	if(VERBOSE) printf("Finished\n");

	return 0;
}

int create_training_file(char *trainingfilename, struct Detection **detectionarray,
	int numdetections, struct GroundTruth **groundtrutharray, int numgroundtruth,
	int numfeatures, char **featurelabels, char **features, struct IndexLink *gtindexbase)
{
	FILE *fptrtraining = NULL;

	struct VUnionLL *unique_detection_filenames = NULL,
		*unique_groundtruth_filenames = NULL,
		*unique_filenames = NULL, *vunionptr = NULL;
	struct IndexLink *detindex = NULL, *gtindex = NULL, *templink = NULL, *templink_det = NULL, *templink_gt = NULL;
	struct DetScore *detscore = NULL, *newdetscore = NULL;
	int sum_tp = 0, sum_fp = 0, sum_fn = 0, sum_wash = 0, num = 0, i = 0;
	int cols, rows, xcoord, ycoord, candidatenumber, featureindex, num_p = 0, num_n = 0, truthindex = 0;
	char *statusarray = NULL, *filename = NULL;
	union VUnion *vunion = NULL, *vuniongroups = NULL;
	int numreleventgroundtruth = 0;

	//-------------------------------------------------------------------------
	// Determine the list of images that will be used to compute the matches
	// between detection and truth regions.
	//-------------------------------------------------------------------------
	if(VERBOSE) printf("Finding the list of unique detection (can) filenames.\n");
	unique_detection_filenames = sorted_unique_values_for_name(detectionarray,
		numdetections, "IMAGE", "string");

	if(VERBOSE) printf("Finding the list of unique truth filenames.\n");
	unique_groundtruth_filenames = sorted_unique_values_for_name(groundtrutharray,
		numgroundtruth, "IMAGE", "string");

	if(VERBOSE) printf("Combining the detection (can) and truth filenames.\n");
	unique_filenames = unionvunions(unique_detection_filenames, unique_groundtruth_filenames, "string");

	// Free the linked list of vunions that stored the detection image filenames
	unique_detection_filenames = free_vunion(unique_detection_filenames, "string");
	unique_detection_filenames = NULL;

	// Free the linked list of vunions that stored the truth image filenames
	unique_groundtruth_filenames = free_vunion(unique_groundtruth_filenames, "string");
	unique_groundtruth_filenames = NULL;

	vunionptr = unique_filenames;
	newdetscore = (struct DetScore *) calloc(1, sizeof(struct DetScore));

	while(vunionptr != NULL){

		printf("processing %s\n", vunionptr->vunion.string_value);

		gtindex = where_groundtrutharray(groundtrutharray, numgroundtruth, gtindexbase, "IMAGE", "eq", vunionptr->vunion.string_value, 0);

		detindex = where_detectionarray(detectionarray, numdetections, NULL, "IMAGE", "eq", vunionptr->vunion.string_value, 1);

		printf("GT = %d  DT = %d\n", indexlistlength(gtindex), indexlistlength(detindex));

		detscore = score_detection_groundtruth(groundtrutharray, gtindex, detectionarray, detindex);

		// Save the indices of the ground truth true positives.
		templink = unionindex(newdetscore->gtindex_tp, detscore->gtindex_tp);
		newdetscore->gtindex_tp = free_index(newdetscore->gtindex_tp);
		newdetscore->gtindex_tp = templink;
		templink = NULL;

		// Save the indices of the ground truth false negatives.
		templink = unionindex(newdetscore->gtindex_fn, detscore->gtindex_fn);
		newdetscore->gtindex_fn = free_index(newdetscore->gtindex_fn);
		newdetscore->gtindex_fn = templink;
		templink = NULL;

		// Save the indices of the detection true positives.
		templink = unionindex(newdetscore->detindex_tp, detscore->detindex_tp);
		newdetscore->detindex_tp = free_index(newdetscore->detindex_tp);
		newdetscore->detindex_tp = templink;
		templink = NULL;

		// Save the indices of the detection false positives.
		templink = unionindex(newdetscore->detindex_fp, detscore->detindex_fp);
		newdetscore->detindex_fp = free_index(newdetscore->detindex_fp);
		newdetscore->detindex_fp = templink;
		templink = NULL;

		// Save the indices of the detection washes.
		templink = unionindex(newdetscore->detindex_wash, detscore->detindex_wash);
		newdetscore->detindex_wash = free_index(newdetscore->detindex_wash);
		newdetscore->detindex_wash = templink;
		templink = NULL;

		// Save the indices of the detection matches.
		templink = concatenateindex(newdetscore->detindex_match, detscore->detindex_match);
		newdetscore->detindex_match = free_index(newdetscore->detindex_match);
		newdetscore->detindex_match = templink;
		templink = NULL;

		// Save the indices of the ground truth matches.
		templink = concatenateindex(newdetscore->gtindex_match, detscore->gtindex_match);
		newdetscore->gtindex_match = free_index(newdetscore->gtindex_match);
		newdetscore->gtindex_match = templink;
		templink = NULL;

		sum_tp += detscore->num_tp;
		sum_fp += detscore->num_fp;
		sum_fn += detscore->num_fn;
		sum_wash += detscore->num_wash;
		num++;

		detindex = free_index(detindex);
		gtindex = free_index(gtindex);

		detscore = free_detscore(detscore);

		vunionptr = vunionptr->nextvunion;

	}

	printf("sum_tp = %d  sum_fp = %d  sum_fn = %d  sum_wash = %d\n",
		sum_tp, sum_fp, sum_fn, sum_wash);
	fflush(stdout);

	// Create a file that combines the candidates, their pixel coordinates, the candidate
	// ids, and whether or not they correspond to a truth region. Please note that detections
	// that are scored as washes do not contribute to TP's but do indicate a correspondence.

	//#^FTR V1.0
	//title: NewYorkHospital
	//nclasses: 2
	//clabels: P N 
	//nsamples: 137 4063 
	//apriori: 0.500000 0.500000 
	//nfeatures: 36
	//flabels: CoinNorm 

	if(trainingfilename != NULL){
		statusarray = (char *) calloc(numdetections, sizeof(char));
		for(i=0;i<numdetections;i++) statusarray[i] = 'N';
		templink = newdetscore->detindex_tp;
		while(templink != NULL){
			statusarray[templink->index] = 'P';
			templink = templink->nextindex;
		}
		templink = newdetscore->detindex_wash;
		while(templink != NULL){
			statusarray[templink->index] = 'P';
			templink = templink->nextindex;
		}
		num_p = 0;
		num_n = 0;
		for(i=0;i<numdetections;i++){
			if(statusarray[i] == 'P') num_p++;
			if(statusarray[i] == 'N') num_n++;
		}
		fptrtraining = fopen(trainingfilename, "w");

		// If we have groups information then we will write a V1.1 training
		// file and include the groups information. If there is no groups
		// information present, then we will write a V1.0 trainign file.
		vuniongroups = get_nvp_value_for_name(detectionarray[0]->nvp, "GROUPS");
		if(vuniongroups == (VUnion *)NULL) fprintf(fptrtraining, "#^TRN V1.0\n");
		else fprintf(fptrtraining, "#^TRN V1.1\n");

		fprintf(fptrtraining, "title: none\n");
		fprintf(fptrtraining, "nclasses: 2\n");
		fprintf(fptrtraining, "clabels: P N\n");
		fprintf(fptrtraining, "nsamples: %d %d\n", num_p, num_n);
		fprintf(fptrtraining, "apriori: 0.500000 0.500000\n");
		fprintf(fptrtraining, "nfeatures: %d\n", numfeatures);
		fprintf(fptrtraining, "flabels:");
		for(i=0;i<numfeatures;i++) fprintf(fptrtraining, " %s", featurelabels[i]);
		fprintf(fptrtraining, "\n");
		
		for(i=0;i<numdetections;i++){
			cols = detectionarray[i]->cols;
			rows = detectionarray[i]->rows;
			xcoord = detectionarray[i]->point.x;
			ycoord = detectionarray[i]->point.y;
			vunion = get_nvp_value_for_name(detectionarray[i]->nvp, "IMAGE");
			filename = vunion->string_value;
			vunion = get_nvp_value_for_name(detectionarray[i]->nvp, "CANDIDATE");
			candidatenumber = vunion->intvalue;
			vunion = get_nvp_value_for_name(detectionarray[i]->nvp, "FEATUREINDEX");
			featureindex = vunion->intvalue;
			//fprintf(fptrtraining, "%s %d %d %d %d %d %c %s", filename, 
			//	rows, cols, xcoord, ycoord, candidatenumber, statusarray[i], features[featureindex]);
			fprintf(fptrtraining, "%s%c %s canID:_%d", features[featureindex], statusarray[i], filename, candidatenumber);
			templink_det = newdetscore->detindex_match;
			templink_gt = newdetscore->gtindex_match; 
			if((templink_det == NULL) || (templink_gt == NULL)){
				printf("Error! No matches were found!\n");
				fflush(stdout);
				exit(1);
			}
			while(templink_det != NULL){
				if(templink_det->index == i){
					vunion = get_nvp_value_for_name(groundtrutharray[templink_gt->index]->nvp, "TRUTHINDEX");
					truthindex = vunion->intvalue;
					fprintf(fptrtraining, "_nodID:_%d", truthindex);
				}
				templink_det = templink_det->nextindex;
				templink_gt = templink_gt->nextindex;
			}
			filename = NULL;

			// Write the Group information if it is stored and we are writing a V1.1 file.
			if(vuniongroups != (VUnion *)NULL){
				int fn = 0, tempint = 0;
				char *cptr = NULL;

				vuniongroups = get_nvp_value_for_name(detectionarray[i]->nvp, "GROUPS");
				fprintf(fptrtraining, " Groups:");

				sscanf(vuniongroups->string_value, "%d", &tempint);
                fprintf(fptrtraining, "_%d", tempint);

			    cptr = strtok(vuniongroups->string_value, " " );
			    fn = 0;
			    while(cptr != NULL){
		    		cptr = strtok((char *)NULL, " ");
				    if(cptr != NULL){
					    sscanf(cptr, "%d", &(tempint));
						fprintf(fptrtraining, "_%d", tempint);
					    //printf("<%d>\n", newcandidate->groups[fn]);
					    fn++;
					}
				}

				//fprintf(fptrtraining, "%s", vuniongroups->string_value);
			}

			fprintf(fptrtraining, "\n");
		}

		free(statusarray);

		templink_gt = gtindexbase;
		numreleventgroundtruth = 0;
		while(templink_gt != NULL){
			numreleventgroundtruth++;
			templink_gt = templink_gt->nextindex;
		}
		fprintf(fptrtraining, "numgroundtruth: %d\n", numreleventgroundtruth);
		templink_gt = gtindexbase;
		while(templink_gt != NULL){
			vunion = get_nvp_value_for_name(groundtrutharray[templink_gt->index]->nvp, "IMAGE");
			filename = vunion->string_value;
			vunion = get_nvp_value_for_name(groundtrutharray[templink_gt->index]->nvp, "TRUTHINDEX");
			truthindex = vunion->intvalue;
			fprintf(fptrtraining, "%s %d\n", filename, truthindex);
			templink_gt = templink_gt->nextindex;
		}

		fclose(fptrtraining);
	}

	newdetscore = free_detscore(newdetscore);

	// Free the linked list of vunions that stored the truth image filenames
	unique_filenames = free_vunion(unique_filenames, "string");
	unique_filenames = NULL;


	return(0);
}