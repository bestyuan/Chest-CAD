#ifndef _CanIo_
#define _CanIo_

#include "misc.h"

struct Candidate{
	int index;
	struct Point centroid;
	float *ftrvector;
	char *ftrvector_line;
	float probability;
	int ngroups;						// Added to support groups in CAN V1.2
	int *groups;						// Added to support groups in CAN V1.2
	struct Candidate *nextcandidate;
};

struct CanData{
	char *sourceimage;
	int cols, rows;
	int numcandidates;
	int numfeatures;
	char **featurelabels;
	struct Candidate *candidate;
	struct Candidate **candidatearray;
	char regiontype;
	struct CanData *nextcandata;
};

int read_canset_v1point0(char *filename, struct CanData **candata);
struct CanData *read_can_v1point0(char *filename, struct CanData *candata);
struct CanData *read_can_v1point1(char *filename, struct CanData *candata);
struct CanData *read_can_v1point2(char *filename, struct CanData *candata);
int read_canset_v1point1(char *filename, struct CanData **candata);
int read_canset_v1point2(char *filename, struct CanData **candata);
struct CanData *read_can_v1point1(char *filename, struct CanData *candata);
void free_candata(struct CanData *candata);
void free_candata(struct CanData **candata);
float get_cansetversion(char *filename);

#endif

