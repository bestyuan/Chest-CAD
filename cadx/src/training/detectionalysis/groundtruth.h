#ifndef _GroundTruth_
#define _GroundTruth_

#include "misc.h"
#include "hit_io.h"

struct GroundTruth{
	struct NVP *nvp;
	int cols, rows;
	struct Point centroid;
	struct ChainPoint *boundary;
	struct GroundTruth *nextgroundtruth;
};

struct GroundTruth *hit_v1point0_imagetruth_to_groundtruth(struct ImageTruth *imagetruth, struct KESPRHEADER *kh);
struct GroundTruth *free_groundtruth(struct GroundTruth *groundtruth);
void print_groundtruth(struct GroundTruth *groundtruth, FILE *fptr);
void print_groundtrutharray(struct GroundTruth **groundtrutharray, struct IndexLink *index,
	FILE *fptr);
void groundtruth_to_groundtrutharray(struct GroundTruth *groundtruth,
	struct GroundTruth ***groundtruthnarray, int *numgroundtruth);
struct IndexLink *where_groundtrutharray(struct GroundTruth **groundtrutharray, int numgroundtruth,
	struct IndexLink *index, char *name, char *op, float value, int startwithall);
struct IndexLink *where_groundtrutharray(struct GroundTruth **groundtrutharray, int numgroundtruth,
	struct IndexLink *index, char *name, char *op, int value, int startwithall);
struct IndexLink *where_groundtrutharray(struct GroundTruth **groundtrutharray, int numgroundtruth,
	struct IndexLink *index, char *name, char *op, char *value, int startwithall);

#endif