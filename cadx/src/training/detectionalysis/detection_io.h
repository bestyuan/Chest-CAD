#ifndef _DetIo_
#define _DetIo_

#include "misc.h"
#include "can_io.h"

struct Detection{
	struct NVP *nvp;
	int cols, rows;
	struct Point point;
	struct Detection *nextdetection;
};

struct Detection *free_detections(struct Detection *detection);
struct Detection *can_v1point0_candata_to_detections(struct CanData *candata,
	int *numfeatures, char ***featurelabels, char ***featuredata, int *totalnumcandidates);
struct Detection *can_v1point1_candata_to_detections(struct CanData *candata,
	int *numfeatures, char ***featurelabels, char ***featuredata, int *totalnumcandidates);
void print_detectionarray(struct Detection **detectionarray, struct IndexLink *index,
	FILE *fptr);
void print_detections(struct Detection *detections, FILE *fptr);
void detections_to_detectionarray(struct Detection *detections,
	struct Detection ***detectionarray, int *numdetections);
struct IndexLink *where_detectionarray(struct Detection **detectionarray, int numdetections,
	struct IndexLink *index, char *name, char *op, float value, int startwithall);
struct IndexLink *where_detectionarray(struct Detection **detectionarray, int numdetections,
	struct IndexLink *index, char *name, char *op, int value, int startwithall);
struct IndexLink *where_detectionarray(struct Detection **detectionarray, int numdetections,
	struct IndexLink *index, char *name, char *op, char *value, int startwithall);
void print_detectionarray(struct Detection **detectionarray, struct IndexLink *index);
struct IndexLink *free_index(struct IndexLink *index);
struct VUnionLL *sorted_unique_values_for_name(struct Detection **detectionarray,
	int numdetections, char *name, char *type);
struct Detection *ftr_v1point1_ftrdata_to_detections(struct FTR *ftrdata,
	struct Detection **detectionarray, int numdetections);
struct Detection *can_v1point2_candata_to_detections(struct CanData *candata,
	int *numfeatures, char ***featurelabels, char ***featuredata, int *totalnumcandidates);

#endif