#ifndef _Misc_
#define _Misc_

struct IndexLink{
	int index;
	struct IndexLink *nextindex;
};

struct Point{
	int x, y;
};

struct ChainPoint{
	struct Point point;
	struct ChainPoint *nextpoint;
};

union VUnion{
	int intvalue;
	float floatvalue;
	char *string_value;
};

struct NVP{
	char *name;
	char *type;
	union VUnion value;
	struct NVP *nextnvp;
};

struct VUnionLL{
	union VUnion vunion;
	struct VUnionLL *nextvunion;
};

struct IndexLink *free_index(struct IndexLink *index);
struct NVP *create_nvp(struct NVP *nvp, char *name, int value);
struct NVP *create_nvp(struct NVP *nvp, char *name, float value);
struct NVP *create_nvp(struct NVP *nvp, char *name, char *value);
int nvp_match(struct NVP *nvp, char *name, char *op, float value);
int nvp_match(struct NVP *nvp, char *name, char *op, int value);
int nvp_match(struct NVP *nvp, char *name, char *op, char *value);
struct ChainPoint *copy_chainpoint(struct ChainPoint *chainpoint);
struct ChainPoint *free_chainpoint(struct ChainPoint *chainpoint);
struct NVP *free_nvp(struct NVP *nvp);
struct IndexLink *copy_index(struct IndexLink *index);
struct IndexLink *whereindex(struct IndexLink *index, int index_value);
int indexlistlength(struct IndexLink *index);
void appendindex_value(struct IndexLink **index1, int index_value);
struct IndexLink *concatenateindex(struct IndexLink *index1, struct IndexLink *index2);
struct IndexLink *removeindex(struct IndexLink **index, int index_value);
union VUnion *get_nvp_value_for_name(struct NVP *nvp, char *name);
char *get_nvp_type_for_name(struct NVP *nvp, char *name);
struct IndexLink *unionindex(struct IndexLink *index1, struct IndexLink *index2);
struct IndexLink *intersectionindex(struct IndexLink *index1, struct IndexLink *index2);
struct VUnionLL *unionvunions(struct VUnionLL *vunion1, struct VUnionLL *vunion2, char *type);
struct VUnionLL *reversevunions(struct VUnionLL *vunion, char *type);
char *vunion_to_string(struct VUnionLL *vunion, char *type);
struct VUnionLL *free_vunion(struct VUnionLL *vunion, char *type);
char *get_basename(char *string, char *extension);
void make_uppercase(char *string);
#endif