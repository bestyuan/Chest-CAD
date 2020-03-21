#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "misc.h"

char *get_basename(char *string, char *extension)
{
	char pathsep = 92, *newstring = NULL, *cptr1 = NULL, *cptr2 = NULL;
	int i = 0;

	if(string == NULL){
		newstring = (char *) calloc(1, sizeof(char));
		return(newstring);
	}
	newstring = (char *) calloc(strlen(string)+1, sizeof(char));
	cptr1 = newstring;
	cptr2 = string;
	while(cptr2 < (string + strlen(string))){
		if(*cptr2 == pathsep){
			memset(newstring, strlen(string), 0);
			cptr1 = newstring;
			cptr2++;
		}
		else{
			*cptr1 = *cptr2;
			cptr1++;
			cptr2++;
		}
	}
	if((extension == NULL) || (strlen(extension) == 0) || (strlen(extension) > strlen(newstring))) return(newstring);

	cptr1 = newstring + strlen(newstring) - 1;
	cptr2 = extension + strlen(extension) - 1;
	for(i=0;i<(int)strlen(extension);i++){
		if((*cptr1) != (*cptr2)) return(newstring);
		cptr1--;
		cptr2--;
	}

	*(cptr1 + 1) = '\0';
	return(newstring);
}

struct IndexLink *free_index_once(struct IndexLink *index)
{
	if(index == NULL) return(NULL);
	if(index->nextindex != NULL) free_index(index->nextindex);
	free(index);
	return(NULL);
}

struct IndexLink *free_index(struct IndexLink *index)
{
	// I modified the code to remove a stack overflow
	// that was occurring due to too many recursive calls to the function.
	// Mike Heath 3/2/2004
	//	if(index == NULL) return(NULL);
	//	if(index->nextindex != NULL) free_index(index->nextindex);
	//	free(index);
	//	return(NULL);

	struct IndexLink *indexptr = NULL, **indexptr_to_free = NULL;
	int n = 0;

	if(index == NULL) return(NULL);

	while(1){
		indexptr = index;
		indexptr_to_free = &index;
		n = 0;
		while(indexptr != NULL){
			n++;
			if(n >= 1000) indexptr_to_free = &((*indexptr_to_free)->nextindex);
			indexptr = indexptr->nextindex;
		}
		if((*indexptr_to_free) == index){
			// Free all remaining indices in the linked list.
			*indexptr_to_free = free_index_once(*indexptr_to_free);
			return(NULL);
		}
		else{
			// Free the last 1000 indices in the linked list.
			*indexptr_to_free = free_index_once(*indexptr_to_free);
		}
	}

}

struct IndexLink *copy_index(struct IndexLink *index)
{
	struct IndexLink *indexptr = NULL, *newindex = NULL,
		*lastindex = NULL, *topindex = NULL;

	indexptr = index;
	while(indexptr != NULL){
		newindex = (struct IndexLink *) calloc(1, sizeof(struct IndexLink));
		if(topindex == NULL) topindex = newindex;
		else lastindex->nextindex = newindex;
		lastindex = newindex;
		newindex->index = indexptr->index;
		indexptr = indexptr->nextindex;
	}

	return(topindex);
}

struct IndexLink *whereindex(struct IndexLink *index, int index_value)
{
	struct IndexLink *indexptr = NULL;

	// printf("[Looking for %d] ", index_value);
	indexptr = index;
	while(indexptr != NULL){
		// printf("<Trying %d>", indexptr->index);
		if(indexptr->index == index_value){
			// printf("\n------------------------ Found in list ---------------------------\n"); fflush(stdout);
			return(indexptr);
		}
		indexptr = indexptr->nextindex;
	}
    // printf("\n");
	return(NULL);
}

int indexlistlength(struct IndexLink *index)
{
	struct IndexLink *indexptr = NULL;
	int length = 0;

	indexptr = index;
	while(indexptr != NULL){
		length++;
		indexptr = indexptr->nextindex;
	}

	return(length);
}

int compare_int(const void *a, const void *b)
{
	return((*(int *)a) - (*(int *)b));
}

struct IndexLink *unionindex(struct IndexLink *index1, struct IndexLink *index2)
{
	struct IndexLink *unionindexptr = NULL, *indexptr = NULL, *lastindex = NULL, *newindex = NULL;
	int len1 = 0, len2 = 0, i = 0, *array = NULL;

	len1 = indexlistlength(index1);
	len2 = indexlistlength(index2);

	if((len1 + len2) == 0) return(NULL);
	array = (int *) calloc(len1+len2, sizeof(int));

	indexptr = index1;
	i = 0;
	while(indexptr != NULL){
		array[i] = indexptr->index;
		indexptr = indexptr->nextindex;
		i++;
	}

	indexptr = index2;
	while(indexptr != NULL){
		array[i] = indexptr->index;
		indexptr = indexptr->nextindex;
		i++;
	}

	qsort((void *)array, (size_t)(len1+len2), sizeof(int), compare_int);

	for(i=0;i<(len1+len2);i++){
		if((i == 0) || (array[i] != array[i-1])){
			newindex = (struct IndexLink *) calloc(1, sizeof(struct IndexLink));
			if(unionindexptr == NULL) unionindexptr = newindex;
			else lastindex->nextindex = newindex;
			lastindex = newindex;
			newindex->index = array[i];
		}
	}

	free(array);
	return(unionindexptr);
}

struct IndexLink *concatenateindex(struct IndexLink *index1, struct IndexLink *index2)
{
	struct IndexLink *unionindexptr = NULL, *indexptr = NULL, *lastindex = NULL, *newindex = NULL;
	int len1 = 0, len2 = 0, i = 0, *array = NULL;

	len1 = indexlistlength(index1);
	len2 = indexlistlength(index2);

	if((len1 + len2) == 0) return(NULL);
	array = (int *) calloc(len1+len2, sizeof(int));

	indexptr = index1;
	i = 0;
	while(indexptr != NULL){
		array[i] = indexptr->index;
		indexptr = indexptr->nextindex;
		i++;
	}

	indexptr = index2;
	while(indexptr != NULL){
		array[i] = indexptr->index;
		indexptr = indexptr->nextindex;
		i++;
	}

	for(i=0;i<(len1+len2);i++){
		newindex = (struct IndexLink *) calloc(1, sizeof(struct IndexLink));
		if(unionindexptr == NULL) unionindexptr = newindex;
		else lastindex->nextindex = newindex;
		lastindex = newindex;
		newindex->index = array[i];
	}

	free(array);
	return(unionindexptr);
}

struct IndexLink *intersectionindex(struct IndexLink *index1, struct IndexLink *index2)
{
	struct IndexLink *intersectionindexptr = NULL, *indexptr = NULL, *lastindex = NULL, *newindex = NULL;
	int len1 = 0, len2 = 0, i = 0, *array = NULL;

	len1 = indexlistlength(index1);
	len2 = indexlistlength(index2);

	if((len1 == 0) || (len2 == 0)) return(NULL);
	array = (int *) calloc(len1+len2, sizeof(int));

	indexptr = index1;
	i = 0;
	while(indexptr != NULL){
		array[i] = indexptr->index;
		indexptr = indexptr->nextindex;
		i++;
	}

	indexptr = index2;
	while(indexptr != NULL){
		array[i] = indexptr->index;
		indexptr = indexptr->nextindex;
		i++;
	}

	qsort((void *)array, (size_t)(len1+len2), sizeof(int), compare_int);

	for(i=0;i<(len1+len2)-1;i++){
		if((array[i] == array[i+1])){
			newindex = (struct IndexLink *) calloc(1, sizeof(struct IndexLink));
			if(intersectionindexptr == NULL) intersectionindexptr = newindex;
			else lastindex->nextindex = newindex;
			lastindex = newindex;
			newindex->index = array[i];
		}
	}

	free(array);
	return(intersectionindexptr);
}

struct IndexLink *removeindex(struct IndexLink **index, int index_value)
{
	struct IndexLink *indexptr = NULL, *lastindexptr = NULL;

	if(*index == NULL) return(NULL);

	indexptr = lastindexptr = *index;
	while(indexptr != NULL){
		if(indexptr->index == index_value){
			// printf("Found it in the list to remove it!\n"); fflush(stdout);
			if(indexptr == *index){
				*index = (*index)->nextindex;
				indexptr->nextindex = NULL;
				return(indexptr);
			}
			else{
				lastindexptr->nextindex = indexptr->nextindex;
				indexptr->nextindex = NULL;
				return(indexptr);
			}
		}
		lastindexptr = indexptr;
		indexptr = indexptr->nextindex;
	}

	return(NULL);
}

void appendindex_value(struct IndexLink **index1, int index_value)
{
	struct IndexLink *indexptr = NULL;

	if((*index1) == NULL){
		*index1 = (struct IndexLink *) calloc(1, sizeof(struct IndexLink));
		(*index1)->index = index_value;
		return;
	}

	indexptr = *index1;
	while(indexptr != NULL){
		if(indexptr->nextindex == NULL){
			indexptr->nextindex = (struct IndexLink *) calloc(1, sizeof(struct IndexLink));
			indexptr->nextindex->index = index_value;
			return;
		}
		indexptr = indexptr->nextindex;
	}
}

struct NVP *free_nvp(struct NVP *nvp)
{
	if(nvp == NULL) return(NULL);
	if(nvp->nextnvp != NULL) free_nvp(nvp->nextnvp);
	if(nvp->name != NULL) free(nvp->name);
	if(nvp->type != NULL){
		if(strcmp(nvp->type, "string") == 0){
			if(nvp->value.string_value != NULL) free(nvp->value.string_value);
		}
	}
	free(nvp);
	return(NULL);
}

//=============================================================================
// Function: create_nvp
// Purpose: To create a linked list or an element of a linked list and append
// it to the input linked list that contains a name and value pair. The
// name and value are stored internally as strings in the linked list but
// the type of the data (float) is stored. Knowledge of the type allows
// operations on the data by logical functions that are possible with
// numberic data (as opposed to string data). If nvp is passed a NULL then
// a new linked list is created, otherwise, a new link is added to the end
// of the input linked list.
// Name: Michael Heath, Eastman Kodak Company
// Date: 6/17/2003
//=============================================================================
struct NVP *create_nvp(struct NVP *nvp, char *name, float value)
{
	struct NVP *newnvp = NULL, *nvpptr = NULL;
	static char *type = "float";

	newnvp = (struct NVP *) calloc(1, sizeof(struct NVP));
	newnvp->name = (char *) calloc(strlen(name)+1, sizeof(char));
	strcpy(newnvp->name, name);
	newnvp->type = type;
	newnvp->value.floatvalue = value;

	if(nvp == NULL) return(newnvp);

	nvpptr = nvp;
	while(nvpptr->nextnvp != NULL) nvpptr = nvpptr->nextnvp;
	nvpptr->nextnvp = newnvp;

	return(nvp);
}

//=============================================================================
// Function: create_nvp
// Purpose: To create a linked list or an element of a linked list and append
// it to the input linked list that contains a name and value pair. The
// name and value are stored internally as strings in the linked list but
// the type of the data (integer) is stored. Knowledge of the type allows
// operations on the data by logical functions that are possible with
// numberic data (as opposed to string data). If nvp is passed a NULL then
// a new linked list is created, otherwise, a new link is added to the end
// of the input linked list.
// Name: Michael Heath, Eastman Kodak Company
// Date: 6/17/2003
//=============================================================================
struct NVP *create_nvp(struct NVP *nvp, char *name, int value)
{
	struct NVP *newnvp = NULL, *nvpptr = NULL;
	static char *type = "int";

	newnvp = (struct NVP *) calloc(1, sizeof(struct NVP));
	newnvp->name = (char *) calloc(strlen(name)+1, sizeof(char));
	strcpy(newnvp->name, name);
	newnvp->type = type;
	newnvp->value.intvalue = value;

	if(nvp == NULL) return(newnvp);

	nvpptr = nvp;
	while(nvpptr->nextnvp != NULL) nvpptr = nvpptr->nextnvp;
	nvpptr->nextnvp = newnvp;

	return(nvp);
}

//=============================================================================
// Function: create_nvp
// Purpose: To create a linked list or an element of a linked list and append
// it to the input linked list that contains a name and value pair. The
// name and value are stored internally as strings in the linked list but
// the type of the data (string) is stored. Knowledge of the type allows
// operations on the data by logical functions that are useful with strings.
// If nvp is passed a NULL then a new linked list is created, otherwise,
// a new link is added to the end
// of the input linked list.
// Name: Michael Heath, Eastman Kodak Company
// Date: 6/17/2003
//=============================================================================
struct NVP *create_nvp(struct NVP *nvp, char *name, char *value)
{
	struct NVP *newnvp = NULL, *nvpptr = NULL;
	static char *type = "string";

	newnvp = (struct NVP *) calloc(1, sizeof(struct NVP));
	newnvp->name = (char *) calloc(strlen(name)+1, sizeof(char));
	strcpy(newnvp->name, name);
	newnvp->type = type;
	newnvp->value.string_value = (char *) calloc(strlen(value)+1, sizeof(char));
	strcpy(newnvp->value.string_value, value);

	if(nvp == NULL) return(newnvp);

	nvpptr = nvp;
	while(nvpptr->nextnvp != NULL) nvpptr = nvpptr->nextnvp;
	nvpptr->nextnvp = newnvp;

	return(nvp);
}

int nvp_match(struct NVP *nvp, char *name, char *op, float value)
{
	struct NVP *nvpptr = NULL;

	if(strcmp(op, "eq") == 0){
		nvpptr = nvp;
		while(nvpptr != NULL){
			if(strcmp(nvpptr->name, name) == 0){
				if(nvpptr->value.floatvalue == value) return 1;
			}
			nvpptr = nvpptr->nextnvp;
		}
	}

	else if(strcmp(op, "lt") == 0){
		nvpptr = nvp;
		while(nvpptr != NULL){
			if(strcmp(nvpptr->name, name) == 0){
				if(nvpptr->value.floatvalue < value) return 1;
			}
			nvpptr = nvpptr->nextnvp;
		}
	}

	else if(strcmp(op, "gt") == 0){
		nvpptr = nvp;
		while(nvpptr != NULL){
			if(strcmp(nvpptr->name, name) == 0){
				if(nvpptr->value.floatvalue > value) return 1;
			}
			nvpptr = nvpptr->nextnvp;
		}
	}

	else if(strcmp(op, "le") == 0){
		nvpptr = nvp;
		while(nvpptr != NULL){
			if(strcmp(nvpptr->name, name) == 0){
				if(nvpptr->value.floatvalue <= value) return 1;
			}
			nvpptr = nvpptr->nextnvp;
		}
	}

	else if(strcmp(op, "ge") == 0){
		nvpptr = nvp;
		while(nvpptr != NULL){
			if(strcmp(nvpptr->name, name) == 0){
				if(nvpptr->value.floatvalue >= value) return 1;
			}
			nvpptr = nvpptr->nextnvp;
		}
	}

	else if(strcmp(op, "ne") == 0){
		nvpptr = nvp;
		while(nvpptr != NULL){
			if(strcmp(nvpptr->name, name) == 0){
				if(nvpptr->value.floatvalue != value) return 1;
			}
			nvpptr = nvpptr->nextnvp;
		}
	}

	return 0;
}

int nvp_match(struct NVP *nvp, char *name, char *op, int value)
{
	struct NVP *nvpptr = NULL;

	if(strcmp(op, "eq") == 0){
		nvpptr = nvp;
		while(nvpptr != NULL){
			if(strcmp(nvpptr->name, name) == 0){
				if(nvpptr->value.intvalue== value) return 1;
			}
			nvpptr = nvpptr->nextnvp;
		}
	}

	else if(strcmp(op, "lt") == 0){
		nvpptr = nvp;
		while(nvpptr != NULL){
			if(strcmp(nvpptr->name, name) == 0){
				if(nvpptr->value.intvalue < value) return 1;
			}
			nvpptr = nvpptr->nextnvp;
		}
	}

	else if(strcmp(op, "gt") == 0){
		nvpptr = nvp;
		while(nvpptr != NULL){
			if(strcmp(nvpptr->name, name) == 0){
				if(nvpptr->value.intvalue > value) return 1;
			}
			nvpptr = nvpptr->nextnvp;
		}
	}

	else if(strcmp(op, "le") == 0){
		nvpptr = nvp;
		while(nvpptr != NULL){
			if(strcmp(nvpptr->name, name) == 0){
				if(nvpptr->value.intvalue <= value) return 1;
			}
			nvpptr = nvpptr->nextnvp;
		}
	}

	else if(strcmp(op, "ge") == 0){
		nvpptr = nvp;
		while(nvpptr != NULL){
			if(strcmp(nvpptr->name, name) == 0){
				if(nvpptr->value.intvalue >= value) return 1;
			}
			nvpptr = nvpptr->nextnvp;
		}
	}

	else if(strcmp(op, "ne") == 0){
		nvpptr = nvp;
		while(nvpptr != NULL){
			if(strcmp(nvpptr->name, name) == 0){
				if(nvpptr->value.intvalue != value) return 1;
			}
			nvpptr = nvpptr->nextnvp;
		}
	}

	return 0;
}

union VUnion *get_nvp_value_for_name(struct NVP *nvp, char *name)
{
	struct NVP *nvpptr = NULL;

	nvpptr = nvp;
	while(nvpptr != NULL){
		if(strcmp(nvpptr->name, name) == 0) return(&(nvpptr->value));
		nvpptr = nvpptr->nextnvp;
	}

	return(NULL);
}

char *get_nvp_type_for_name(struct NVP *nvp, char *name)
{
	struct NVP *nvpptr = NULL;

	nvpptr = nvp;
	while(nvpptr != NULL){
		if(strcmp(nvpptr->name, name) == 0) return(nvpptr->type);
		nvpptr = nvpptr->nextnvp;
	}

	return(NULL);
}

int nvp_match(struct NVP *nvp, char *name, char *op, char *value)
{
	struct NVP *nvpptr = NULL;

	if(strcmp(op, "eq") != 0) return 1;

	nvpptr = nvp;
	while(nvpptr != NULL){
		if(strcmp(nvpptr->name, name) == 0){
			if(strcmp(value, nvpptr->value.string_value) == 0) return 1;
		}
		nvpptr = nvpptr->nextnvp;
	}
	return 0;
}

struct ChainPoint *copy_chainpoint(struct ChainPoint *chainpoint)
{
	struct ChainPoint *firstchainpoint = NULL, *newchainpoint = NULL,
		*lastchainpoint = NULL, *chainpointptr = NULL;

	chainpointptr = chainpoint;
	while(chainpointptr != NULL){
		newchainpoint = (struct ChainPoint *) calloc(1, sizeof(struct ChainPoint));
		if(firstchainpoint == NULL) firstchainpoint = newchainpoint;
		else lastchainpoint->nextpoint = newchainpoint;
		lastchainpoint = newchainpoint;
		newchainpoint->point = chainpointptr->point;
		chainpointptr = chainpointptr->nextpoint;
	}
	return(firstchainpoint);
}

struct ChainPoint *free_chainpoint(struct ChainPoint *chainpoint)
{
	if(chainpoint == NULL) return(NULL);
	if(chainpoint->nextpoint != NULL) free_chainpoint(chainpoint->nextpoint);
	free(chainpoint);
	return(NULL);
}

char *vunion_to_string(struct VUnionLL *vunion, char *type)
{
	char *newstring = NULL;

	if(strcmp(type, "string") == 0){
		newstring = (char *) calloc(strlen(vunion->vunion.string_value)+1, sizeof(char));
		strcpy(newstring, vunion->vunion.string_value);
	}
	if(strcmp(type, "float") == 0){
		newstring = (char *) calloc(12, sizeof(char));
		sprintf(newstring, "%f", vunion->vunion.floatvalue);
	}
	if(strcmp(type, "int") == 0){
		newstring = (char *) calloc(12, sizeof(char));
		sprintf(newstring, "%d", vunion->vunion.intvalue);
	}
	return newstring;
}

struct VUnionLL *free_vunion(struct VUnionLL *vunion, char *type)
{
	if(vunion == NULL) return(NULL);
	if(vunion->nextvunion != NULL) vunion->nextvunion = free_vunion(vunion->nextvunion, type);
	if((strcmp(type, "string") == 0) && (vunion->vunion.string_value != NULL))
		free(vunion->vunion.string_value);
	free(vunion);
	return(NULL);
}

struct VUnionLL *reversevunions(struct VUnionLL *vunion, char *type)
{
	struct VUnionLL *newvunion = NULL, *topvunion = NULL, *vunionptr = NULL;

	vunionptr = vunion;
	while(vunionptr != NULL){
		newvunion = (struct VUnionLL *) calloc(1, sizeof(struct VUnionLL));
		if(strcmp(type, "float") == 0){
			newvunion->vunion.floatvalue = vunionptr->vunion.floatvalue;
		}
		if(strcmp(type, "int") == 0){
			newvunion->vunion.intvalue = vunionptr->vunion.intvalue;
		}
		if(strcmp(type, "string") == 0){
			newvunion->vunion.string_value = (char *) calloc(strlen(vunionptr->vunion.string_value)+1, sizeof(char));
			strcpy(newvunion->vunion.string_value, vunionptr->vunion.string_value);
		}
		newvunion->nextvunion = topvunion;
		topvunion = newvunion;
		vunionptr = vunionptr->nextvunion;
	}

	return(topvunion);
}

int compare_vunion_float(const void *a, const void *b)
{
	struct VUnionLL vu1, vu2;

	vu1 = **((struct VUnionLL **)a);
	vu2 = **((struct VUnionLL **)b);

	if(vu1.vunion.floatvalue > vu2.vunion.floatvalue) return 1;
	if(vu1.vunion.floatvalue < vu2.vunion.floatvalue) return -1;
	return 0;
}

int compare_vunion_int(const void *a, const void *b)
{
	struct VUnionLL vu1, vu2;

	vu1 = **((struct VUnionLL **)a);
	vu2 = **((struct VUnionLL **)b);

	if(vu1.vunion.intvalue > vu2.vunion.intvalue) return 1;
	if(vu1.vunion.intvalue < vu2.vunion.intvalue) return -1;
	return 0;
}

int compare_vunion_string(const void *a, const void *b)
{
	struct VUnionLL vu1, vu2;

	vu1 = **((struct VUnionLL **)a);
	vu2 = **((struct VUnionLL **)b);

	return(strcmp(vu1.vunion.string_value, vu2.vunion.string_value));
}

struct VUnionLL *unionvunions(struct VUnionLL *vunion1, struct VUnionLL *vunion2, char *type)
{
	struct VUnionLL *unionvunion = NULL, *vunionptr = NULL, *lastvunion = NULL, *newvunion = NULL;
	int len1 = 0, len2 = 0, i = 0;
	struct VUnionLL **array = NULL;

	len1 = 0;
	vunionptr = vunion1;
	while(vunionptr != NULL){
		len1++;
		vunionptr = vunionptr->nextvunion;
	}

	len2 = 0;
	vunionptr = vunion2;
	while(vunionptr != NULL){
		len2++;
		vunionptr = vunionptr->nextvunion;
	}

	if((len1 + len2) == 0) return(NULL);
	array = (struct VUnionLL **) calloc(len1+len2, sizeof(struct VUnionLL *));

	vunionptr = vunion1;
	i = 0;
	while(vunionptr != NULL){
		array[i] = vunionptr;
		vunionptr = vunionptr->nextvunion;
		i++;
	}

	vunionptr = vunion2;
	while(vunionptr != NULL){
		array[i] = vunionptr;
		vunionptr = vunionptr->nextvunion;
		i++;
	}

	if(strcmp(type, "float") == 0){
		qsort((void *)array, (size_t)(len1+len2), sizeof(struct VUnionLL *), compare_vunion_float);
		for(i=0;i<(len1+len2);i++){
			if((i == 0) || (array[i]->vunion.floatvalue != array[i-1]->vunion.floatvalue)){
				newvunion = (struct VUnionLL *) calloc(1, sizeof(struct VUnionLL));
				if(unionvunion == NULL) unionvunion = newvunion;
				else lastvunion->nextvunion = newvunion;
				lastvunion = newvunion;
				newvunion->vunion.floatvalue = array[i]->vunion.floatvalue;
			}
		}
	}
	if(strcmp(type, "int") == 0){
		qsort((void *)array, (size_t)(len1+len2), sizeof(struct VUnionLL *), compare_vunion_int);
		for(i=0;i<(len1+len2);i++){
			if((i == 0) || (array[i]->vunion.intvalue != array[i-1]->vunion.intvalue)){
				newvunion = (struct VUnionLL *) calloc(1, sizeof(struct VUnionLL));
				if(unionvunion == NULL) unionvunion = newvunion;
				else lastvunion->nextvunion = newvunion;
				lastvunion = newvunion;
				newvunion->vunion.intvalue = array[i]->vunion.intvalue;
			}
		}
	}
	if(strcmp(type, "string") == 0){
		qsort((void *)array, (size_t)(len1+len2), sizeof(struct VUnionLL *), compare_vunion_string);
		for(i=0;i<(len1+len2);i++){
			if((i == 0) || (strcmp(array[i]->vunion.string_value,array[i-1]->vunion.string_value) != 0)){
				newvunion = (struct VUnionLL *) calloc(1, sizeof(struct VUnionLL));
				if(unionvunion == NULL) unionvunion = newvunion;
				else lastvunion->nextvunion = newvunion;
				lastvunion = newvunion;
				newvunion->vunion.string_value = (char *) calloc(strlen(array[i]->vunion.string_value)+1, sizeof(char));
				strcpy(newvunion->vunion.string_value, array[i]->vunion.string_value);
			}
		}
	}

	free(array);
	return(unionvunion);
}

//=============================================================================
// Procedure: make_uppercase
// Purpose: This is a simple utility procedure to convert a string to upper
// case.
// Name: Michael Heath, Eastman Kodak Company
// Date: 06/26/20003
//=============================================================================
void make_uppercase(char *string)
{
	char *cptr = NULL;

	cptr = string;
	while(*cptr != '\0'){
		*cptr = toupper((int)(*cptr));
		cptr++;
	}
}