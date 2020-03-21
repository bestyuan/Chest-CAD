#ifndef __CANDIDATELIST_H_
#define __CANDIDATELIST_H_

#include "Candidate.h"

//
// CandidateList : encapsulates a list (vector) of candidates, plus some methods
//                 that operate on the list.
//
// Note that this class is derived from an stl vector, hence any vector-related methods
// can be applied to this class.
//

class CandidateList : public std::vector<Candidate>
{
    public:
          // constructor
        CandidateList() {}

          // merges candidates that are very close to one another
        CandidateList remove_overlapping_peaks() const;

          // draws candidates (with cross hairs indiciating position and a number indicating
          // rank) on the specified image
        void OverlayCandidates(IemTPlane<short> &orig_img, int scale_factor, int max_count) const;

          // truncates the size of the CandidateList to a certain number of candidates.
        CandidateList truncate_size(int max_count) const;

          // merges together peak_list1 and peak_list2, joining candidates that are very close together
        friend CandidateList resolve_multiple_peak_sets(CandidateList &peak_list1, CandidateList &peak_list2);
};

#endif
