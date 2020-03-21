#include "IcdFeature.h"
#include "LungMask.h"
#include "CandidateList.h"

//
// IcdFeatureManager : responsible for computing and keeping track of all features
//
// To use this class, an application calls the register_feature() method for every
// feature it would like to have computed. This should be done only once, right
// after IcdFeatureManager is constructed, and before any calls to compute_features.
//
// Then, the application calls compute_features() on a candidate or list of candidates.
// IcdFeatureManager takes care of the dirty work of computing all of the features,
// and placing them in the correct slots in the features data member of Candidate.
//

class IcdFeatureManager
{
    public:
        IcdFeatureManager() {}

          // register a new feature
        void register_feature(const std::string &feature_name, IcdFeature *icdf);

          // compute features for a set of candidates
        void compute_features(IcdXrayImage &img, const LungMask &mask, CandidateList &peaks,
                              int scale_factor = 1);

          // compute features for a single candidate
        void compute_features(IcdXrayImage &img, const LungMask &mask, Candidate &peak,
                              int scale_factor = 1);

    protected:
        std::map<std::string, IcdFeature *> features;
};
