#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "groundtruth.h"
#include "kespr_io.h"

void print_groundtruth(struct GroundTruth *groundtruth, FILE *fptr)
{
	struct GroundTruth *groundtruthptr = NULL;
	struct NVP *nvp = NULL;
	int d = 0;

	groundtruthptr = groundtruth;
	while(groundtruthptr != NULL){
		fprintf(fptr, "GT=%5d", d);
		// printf(" (%d %d)", detectionptr->point.x, detectionptr->point.y);
		nvp = groundtruthptr->nvp;
		while(nvp != NULL){
			if(strcmp(nvp->type, "float") == 0) fprintf(fptr, "\t%s\t%f", nvp->name, nvp->value.floatvalue);
			if(strcmp(nvp->type, "int") == 0) fprintf(fptr, "\t%s\t%d", nvp->name, nvp->value.intvalue);
			if(strcmp(nvp->type, "string") == 0) fprintf(fptr, "\t%s\t%s", nvp->name, nvp->value.string_value);
			nvp = nvp->nextnvp;
		}
		fprintf(fptr, "\n");
		d++;
		groundtruthptr = groundtruthptr->nextgroundtruth;
	}
}

void print_groundtrutharray(struct GroundTruth **groundtrutharray, struct IndexLink *index, FILE *fptr)
{
	struct IndexLink *indexptr = NULL;
	struct GroundTruth *groundtruthptr = NULL;
	struct NVP *nvp = NULL;
	int d = 0;

	indexptr = index;
	while(indexptr != NULL){
		d = indexptr->index;
		fprintf(fptr, "GT=%5d", d);
		// printf(" (%d %d)", groundtrutharray[d]->point.x, detectionarray[d]->point.y);
		nvp = groundtrutharray[d]->nvp;
		while(nvp != NULL){
			if(strcmp(nvp->type, "float") == 0) fprintf(fptr, "\t%s\t%f", nvp->name, nvp->value.floatvalue);
			if(strcmp(nvp->type, "int") == 0) fprintf(fptr, "\t%s\t%d", nvp->name, nvp->value.intvalue);
			if(strcmp(nvp->type, "string") == 0) fprintf(fptr, "\t%s\t%s", nvp->name, nvp->value.string_value);
			nvp = nvp->nextnvp;
		}
		fprintf(fptr, "\n");
		d++;
		indexptr = indexptr->nextindex;
	}
}

void groundtruth_to_groundtrutharray(struct GroundTruth *groundtruth,
	struct GroundTruth ***groundtrutharray, int *numgroundtruth)
{
	struct GroundTruth *groundtruthptr = NULL;
	int i = 0;

	if(groundtruth == NULL){
		*groundtrutharray = NULL;
		*numgroundtruth = 0;
		return;
	}

	i = 0;
	groundtruthptr = groundtruth;
	while(groundtruthptr != NULL){
		i++;
		groundtruthptr = groundtruthptr->nextgroundtruth;
	}

	*groundtrutharray = (struct GroundTruth **) calloc(i, sizeof(struct GroundTruth *));
	*numgroundtruth = i;	

	i = 0;
	groundtruthptr = groundtruth;
	while(groundtruthptr != NULL){
		(*groundtrutharray)[i] = groundtruthptr;
		i++;
		groundtruthptr = groundtruthptr->nextgroundtruth;
	}
}

struct IndexLink *where_groundtrutharray(struct GroundTruth **groundtrutharray, int numgroundtruth,
	struct IndexLink *index, char *name, char *op, float value, int startwithall)
{
	struct IndexLink *localindex = NULL, *newlocalindex = NULL, *lastlocalindex = NULL, 
		*indexptr = NULL;
	int d = 0;

	if(startwithall){
		for(d=0;d<numgroundtruth;d++){
			if(nvp_match(groundtrutharray[d]->nvp, name, op, value) == 1){
				newlocalindex = (struct IndexLink *) calloc(1, sizeof(struct IndexLink));
				if(localindex == NULL) localindex = newlocalindex;
				else lastlocalindex->nextindex = newlocalindex;
				newlocalindex->index = d;
				lastlocalindex = newlocalindex;
			}
		}
	}
	else{
		indexptr = index;
		while(indexptr != NULL){
			d = indexptr->index;
			if(nvp_match(groundtrutharray[d]->nvp, name, op, value) == 1){
				newlocalindex = (struct IndexLink *) calloc(1, sizeof(struct IndexLink));
				if(localindex == NULL) localindex = newlocalindex;
				else lastlocalindex->nextindex = newlocalindex;
				newlocalindex->index = d;
				lastlocalindex = newlocalindex;
			}
			indexptr = indexptr->nextindex;
		}
	}

	return(localindex);
}

struct IndexLink *where_groundtrutharray(struct GroundTruth **groundtrutharray, int numgroundtruth,
	struct IndexLink *index, char *name, char *op, int value, int startwithall)
{
	struct IndexLink *localindex = NULL, *newlocalindex = NULL, *lastlocalindex = NULL, 
		*indexptr = NULL;
	int d = 0;

	if(startwithall){
		for(d=0;d<numgroundtruth;d++){
			if(nvp_match(groundtrutharray[d]->nvp, name, op, value) == 1){
				newlocalindex = (struct IndexLink *) calloc(1, sizeof(struct IndexLink));
				if(localindex == NULL) localindex = newlocalindex;
				else lastlocalindex->nextindex = newlocalindex;
				newlocalindex->index = d;
				lastlocalindex = newlocalindex;
			}
		}
	}
	else{
		indexptr = index;
		while(indexptr != NULL){
			d = indexptr->index;
			if(nvp_match(groundtrutharray[d]->nvp, name, op, value) == 1){
				newlocalindex = (struct IndexLink *) calloc(1, sizeof(struct IndexLink));
				if(localindex == NULL) localindex = newlocalindex;
				else lastlocalindex->nextindex = newlocalindex;
				newlocalindex->index = d;
				lastlocalindex = newlocalindex;
			}
			indexptr = indexptr->nextindex;
		}
	}

	return(localindex);
}

struct IndexLink *where_groundtrutharray(struct GroundTruth **groundtrutharray, int numgroundtruth,
	struct IndexLink *index, char *name, char *op, char *value, int startwithall)
{
	struct IndexLink *localindex = NULL, *newlocalindex = NULL, *lastlocalindex = NULL, 
		*indexptr = NULL;
	int d = 0;

	if(startwithall){
		for(d=0;d<numgroundtruth;d++){
			if(nvp_match(groundtrutharray[d]->nvp, name, op, value) == 1){
				newlocalindex = (struct IndexLink *) calloc(1, sizeof(struct IndexLink));
				if(localindex == NULL) localindex = newlocalindex;
				else lastlocalindex->nextindex = newlocalindex;
				newlocalindex->index = d;
				lastlocalindex = newlocalindex;
			}
		}
	}
	else{
		indexptr = index;
		while(indexptr != NULL){
			d = indexptr->index;
			if(nvp_match(groundtrutharray[d]->nvp, name, op, value) == 1){
				newlocalindex = (struct IndexLink *) calloc(1, sizeof(struct IndexLink));
				if(localindex == NULL) localindex = newlocalindex;
				else lastlocalindex->nextindex = newlocalindex;
				newlocalindex->index = d;
				lastlocalindex = newlocalindex;
			}
			indexptr = indexptr->nextindex;
		}
	}

	return(localindex);
}

// area2D_Polygon(): computes the area of a 2D polygon
//    Input:  int n = the number of vertices in the polygon
//            Point* V = an array of n+2 vertices 
//                       with V[n]=V[0] and V[n+1]=V[1]
//    Return: the (float) area of the polygon
float area2D_Polygon(struct ChainPoint *cp)
{
	struct ChainPoint *cpptr = NULL;
    float area = 0.0;
	struct Point *i = NULL, *j = NULL, *k = NULL;

//    for (i=1, j=2, k=0; i<=n; i++, j++, k++) {
//        area += V[i].x * (V[j].y - V[k].y);
//    }
//    return area / 2.0;

	cpptr = cp;
	while(cpptr->nextpoint->nextpoint != NULL){
		k = &(cpptr->point);
		i = &(cpptr->nextpoint->point);
		j = &(cpptr->nextpoint->nextpoint->point);
		area += (float)(i->x) * ((float)(j->y) - (float)(k->y));
//		if(cpptr == cp){
//			printf("(%d,%d) (%d,%d) (%d,%d)  ", k->x,k->y,i->x,i->y,j->x,j->y);
//		}
		cpptr = cpptr->nextpoint;
	}
	k = i;
	i = j;
	j = &(cp->nextpoint->point);
	area += (float)(i->x) * ((float)(j->y) - (float)(k->y));
	//printf("(%d,%d) (%d,%d) (%d,%d)\n", k->x,k->y,i->x,i->y,j->x,j->y);
    return((float)(area/2.0));
}

struct GroundTruth *hit_v1point0_imagetruth_to_groundtruth(struct ImageTruth *imagetruth, struct KESPRHEADER *kh)
{
	struct GroundTruth *groundtruth = NULL, *newgroundtruth = NULL, *lastgroundtruth = NULL;
	struct ImageTruth *imagetruthptr = NULL;
	struct Truth *truthptr = NULL;
    int t = 0, d = 0;
	struct KESPRHEADER *khptr = NULL;
	double pixarea = 0.0, pixelspacingmm = 0.0, diametercm = 0.0;
	char *junkstring1 = NULL, *junkstring2 = NULL;

//	struct ChainPoint p0, p1, p2, p3, p4;

//	p0.nextpoint = &p1;
//	p1.nextpoint = &p2;
//	p2.nextpoint = &p3;
//	p3.nextpoint = &p4;
//	p4.nextpoint = NULL;

//	p0.point.x = 0;
//	p0.point.y = 0;
//	p1.point.x = 10;
//	p1.point.y = 0;
//	p2.point.x = 10;
//	p2.point.y = 10;
//	p3.point.x = 0;
//	p3.point.y = 10;
//	p4.point.x = 0;
//	p4.point.y = 0;

//	printf("%f\n", area2D_Polygon(&p0));
//	exit(1);

	imagetruthptr = imagetruth;
	d = 0;
	while(imagetruthptr != NULL){
		for(t=0;t<imagetruthptr->numtruth;t++){
			newgroundtruth = (struct GroundTruth *) calloc(1, sizeof(struct GroundTruth));
			if(d == 0){
				groundtruth = newgroundtruth;
				lastgroundtruth = newgroundtruth;
			}
			else{
				lastgroundtruth->nextgroundtruth = newgroundtruth;
				lastgroundtruth = newgroundtruth;
			}
			newgroundtruth->boundary = copy_chainpoint(imagetruthptr->trutharray[t]->boundary);
			newgroundtruth->centroid = imagetruthptr->trutharray[t]->centroid;
			newgroundtruth->cols = imagetruthptr->cols;
			newgroundtruth->rows = imagetruthptr->rows;
			newgroundtruth->nvp = create_nvp(newgroundtruth->nvp, "IMAGE", imagetruthptr->sourceimage);
			newgroundtruth->nvp = create_nvp(newgroundtruth->nvp, "TRUTHINDEX", (int)(imagetruthptr->trutharray[t]->index)); // (int)t);
			newgroundtruth->nvp = create_nvp(newgroundtruth->nvp, "TRUTHSTATUS", imagetruthptr->trutharray[t]->status);
			newgroundtruth->nvp = create_nvp(newgroundtruth->nvp, "TRUTHPIXELAREA", (float)fabs((double)area2D_Polygon(imagetruthptr->trutharray[t]->boundary)));
			if(kh != NULL){
				khptr = kh;
				while(khptr != NULL){
					junkstring1 = (char *) calloc(strlen(khptr->image)+1, sizeof(char));
					strcpy(junkstring1, khptr->image);
					make_uppercase(junkstring1);
					junkstring2 = (char *) calloc(strlen(imagetruthptr->sourceimage)+1, sizeof(char));
					strcpy(junkstring2, imagetruthptr->sourceimage);
					make_uppercase(junkstring2);
					if(strcmp(junkstring1, junkstring2) == 0){
						pixarea = fabs((double)area2D_Polygon(imagetruthptr->trutharray[t]->boundary));
						pixelspacingmm = (double)get_kespr_pixelspacing(khptr);
						diametercm = sqrt((pixarea * (pixelspacingmm / 10.0) * (pixelspacingmm / 10.0)) / 3.14159265) * 2.0;
						newgroundtruth->nvp = create_nvp(newgroundtruth->nvp, "TRUTHDIAMETERCM", (float)diametercm);
						printf("%s %d %d %f %s\n", junkstring2, t, imagetruthptr->trutharray[t]->index, diametercm,
							imagetruthptr->trutharray[t]->status);
						free(junkstring1);
						junkstring1 = NULL;
						free(junkstring2);
						junkstring2 = NULL;
						break;
					}
					khptr = khptr->nextkesprheader;
				}
			}
			d++;
		}
		imagetruthptr = imagetruthptr->nextimagetruth;
	}
	return(groundtruth);
}

struct GroundTruth *free_groundtruth(struct GroundTruth *groundtruth)
{
   if(groundtruth == NULL) return(NULL);
   if(groundtruth->nextgroundtruth != NULL) free_groundtruth(groundtruth->nextgroundtruth);
   if(groundtruth->boundary != NULL) free_chainpoint(groundtruth->boundary);
   if(groundtruth->nvp != NULL) free_nvp(groundtruth->nvp);
   free(groundtruth);
   return(NULL);
}