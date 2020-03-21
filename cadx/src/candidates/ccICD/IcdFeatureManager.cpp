#include "IcdFeatureManager.h"
#include <iostream>
using namespace std;


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

//
// register_feature: registers a feature with IcdFeatureManager, assigning it a name
//
// feature_name : a name to give this feature, in the features data member of Candidate
// icdf : a pointer to an IcdFeature object that can compute this feature.
//
// This method must be called exactly once after IcdFeatureManager is constructed, 
// for each feature you wish to compute. Basically it controls the mapping between
// feature names (strings) and feature objects (that do the computation).
//
void IcdFeatureManager::register_feature(const std::string &feature_name, IcdFeature *icdf)
{
    features.insert(std::pair<std::string, IcdFeature *>(feature_name, icdf));
}


// 
// compute_features: computes all registered features for a given candidate.
//
// This method computes all features which have been registered by register_feature
// for the given candidate, and fills the features member of Candidate with the
// computed features.
//
// img : x-ray image
// mask : lung mask
// peak : candidate for which to compute features
// scale_factor : relationship between coordinate system of img, and the coordinates
//                in the row and column members of peak.
//
void IcdFeatureManager::compute_features(IcdXrayImage &img, const LungMask &mask, 
                                         Candidate &peak, int scale_factor)
{
    std::map<std::string, IcdFeature *>::iterator feature_iter;

    int i=0;
    for(feature_iter = features.begin(); feature_iter != features.end(); ++feature_iter)
    {
        std::cout << "Feature #" << i++ << std::endl;
        double val = feature_iter->second->compute(img, mask, 
                      Point(peak.row * scale_factor + scale_factor / 2 - 1, 
                            peak.col * scale_factor + scale_factor / 2 - 1));
        peak.features[feature_iter->first] = val;
    }

}


// 
// compute_features: computes all registered features for a given candidate list.
//
// This method computes all features which have been registered by register_feature
// for all candidates in peaks, and fills the features member of each Candidate with the
// computed features.
//
// img : x-ray image
// mask : lung mask
// peaks : candidate list for which to compute features
// scale_factor : relationship between coordinate system of img, and the coordinates
//                in the row and column members of peak.
//
void IcdFeatureManager::compute_features(IcdXrayImage &img, const LungMask &mask, 
                                         CandidateList &peaks, int scale_factor)
{
    std::map<std::string, IcdFeature *>::iterator feature_iter;

    int i=0;
    for(feature_iter = features.begin(); feature_iter != features.end(); ++feature_iter)
    {
        std::cout << "Feature #" << i++ << std::endl;
        std::vector<Point> point_vect;

        CandidateList::iterator cand_iter;
        for(cand_iter = peaks.begin(); cand_iter != peaks.end(); ++cand_iter)
            if(scale_factor != 1)
                point_vect.push_back(Point(cand_iter->row * scale_factor + scale_factor / 2 - 1,
                                           cand_iter->col * scale_factor + scale_factor / 2 - 1));
            else
                point_vect.push_back(Point(cand_iter->row, cand_iter->col));

        std::vector<double> results = feature_iter->second->compute(img, mask, point_vect);

        CandidateList::iterator peak_iter;
        int i=0;
        for(peak_iter = peaks.begin(); peak_iter != peaks.end(); ++peak_iter, i++)  
            peak_iter->features.insert(std::pair<std::string, double>(feature_iter->first, results[i]));
    }  
}
