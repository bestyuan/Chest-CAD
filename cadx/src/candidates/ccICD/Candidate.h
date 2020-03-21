#include <Iem.h>
#include <IemIO.h>
#include <set>
#include "IcdXrayImage.h"
#include "LungMask.h"

#ifndef __CCICD_H_
#define __CCICD_H_

//
// Candidate : encapsulate information about a single candidate
//
// This class stores the row and column, coin value, features, and classified
// probability for a single point (nodule candidate).
//
// Note that by current convention, row and column are with respect to the
// subsampled x-ray image. scale_factor specifies the subsampling scale factor.
// So to obtain coordinates with respect to the original image, you must multiple
// row and column by scale_factor. The software could be changed to remove
// this piece of awkwardness.
//
class Candidate
{
    public:
          // constructor, specifying row and column coordinates (in subsampled space, by convention),
          // coin value, scale factor between subsampled image and original image.
        Candidate(int _row, int _col, double _coin_val, int _scale_factor) : 
                row(_row), col(_col), coin_val(_coin_val), scale_factor(_scale_factor) 
        {
            features.insert(std::pair<std::string, double>("CoinNorm", _coin_val));
        }

          // coordinates
        int row, col;
        
          // coin value
        double coin_val;

          // scale factor between subsampled image and original image
        int scale_factor;

          // probability, after classification
        double nodule_prob;

          // features. The key is a string, the name of the feature.
          // Hence features["afum"] refers to the afum feature value, etc.
        std::map<std::string, double> features; 

          // returns our hypothesis of the probability that this is a nodule
        double get_probability() const
        {
            return nodule_prob;
        }

          // computes distance-squared from this candidate to another
        double distance_to_candidate(const Candidate &pk1) const
        {
            return(pow(pk1.row - row, 2) + pow(pk1.col - col, 2));
        }
};

// output basic info about the candidate
std::ostream &operator<<(std::ostream &os, const Candidate &pk);

// compare two candidates, according to classified probability. Return true if a is greater than b.
class compare_candidates_by_prob : public std::binary_function<const Candidate &, const Candidate &, bool> {
    public:
        bool operator()(const Candidate &a, const Candidate &b)
        {
            return(a.nodule_prob > b.nodule_prob || 
                   a.nodule_prob == b.nodule_prob && a.coin_val > b.coin_val);
        }
};

// compare two candidates, according to coin value. Return true if a is greater than b.
class compare_candidates_by_coin : public std::binary_function<const Candidate &, const Candidate &, bool> {
    public:
        bool operator()(const Candidate &a, const Candidate &b)
        {
            return(a.coin_val > b.coin_val ||
                   a.coin_val == b.coin_val && a.nodule_prob > b.nodule_prob);
        }
};


#endif
