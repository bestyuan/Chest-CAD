#ifndef _FtrIo_
#define _FtrIo_

struct FTRcandidate{
	float *feature;
	char *classlabel;
	char *image;
	int candidatenumber;
	struct FTRcandidate *nextftrcandidate;
};

struct FTR{
	char *title;
	int numclasses;
	char **classlabels;
	int *samplesperclass;
	float *apriori;
	int numfeatures;
	char **featurelabels;
	struct FTRcandidate *ftrcandidate;
	struct FTR *nextftr;
};

struct FTR *read_ftr_v1point0(char *filename, struct FTR *ftrdata);
int read_ftrset_v1point0(char *filename, struct FTR **ftrdata);
void print_ftrdata(struct FTR *ftrdata);
struct FTR *free_ftr(struct FTR *ftrdata);

#endif