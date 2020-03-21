#ifndef _HitIo_
#define _HitIo_

#include "misc.h"

struct Truth{
	char *status;
	int index;
	struct Point centroid;
	struct ChainPoint *boundary;
	float equivelant_diameter_cm;
	struct Truth *nexttruth;
};

struct ImageTruth{
	char *sourceimage;
	int cols, rows, numtruth;
	struct Truth *truth;
	struct Truth **trutharray;
	struct ImageTruth *nextimagetruth;
};

int read_hitset_v1point0(char *filename, struct ImageTruth **imagetruth);
struct ImageTruth *read_hit_v1point0(char *filename, struct ImageTruth *imagetruth);
struct ImageTruth *free_imagetruth(struct ImageTruth *imagetruth);
struct VUnionLL *sorted_unique_values_for_name(struct GroundTruth **groundtrutharray,
	int numgroundtruths, char *name, char *type);

#endif