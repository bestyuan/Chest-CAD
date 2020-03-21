#ifndef _Score_
#define _Score_

#include "misc.h"
#include "groundtruth.h"
#include "detection_io.h"

struct DetScore{
	int num_tp, num_fp, num_fn, num_wash;
	struct IndexLink *detindex_tp, *detindex_fp, *detindex_wash, *detindex_match;
	struct IndexLink *gtindex_tp, *gtindex_fn, *gtindex_match;
	struct DetScore *nextdetscore;
};

struct DetScore *score_detection_groundtruth(struct GroundTruth **groundtrutharray, struct IndexLink *gtindex,
	struct Detection **detectionarray, struct IndexLink *detindex);
struct DetScore *free_detscore(struct DetScore *detscore);

#endif