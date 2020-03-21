#include "TextGen.h"
#include "CandidateList.h"
#include <algorithm>

//
// CandidateList : encapsulates a list (vector) of candidates, plus some methods
//                 that operate on the list.
//
// Note that this class is derived from an stl vector, hence any vector-related methods
// can be applied to this class.
//


//
// remove_overlapping_peaks
//
// analyzes the candidate list, looking for candidates that are very close together,
// and returns a new CandidateList in which the close-together candidates have been
// "joined".
//
// Currently this is accomplished by looking for candidates no more than 5 pixels
// apart, and choosing the "best" one according to coin value (or however
// the CandidateList has been sorted).
//
CandidateList CandidateList::remove_overlapping_peaks() const
{
    CandidateList new_peak_list;

    CandidateList::const_iterator peak_iter = begin();
    for(; peak_iter != end(); peak_iter++)
    {
        CandidateList::iterator peak_iter2 = new_peak_list.begin();
        bool overlap=false;
        for(; peak_iter2 != new_peak_list.end() && !overlap; peak_iter2++)
            if(peak_iter->distance_to_candidate(*peak_iter2) <= 25)
                overlap = true;

        if(!overlap)
            new_peak_list.push_back(*peak_iter);
    }

    return new_peak_list;
}

//
// OverlayCandidates
//
// Draws the candidates in the CandidateList on the x-ray image, along with a number
// indicating rank within the image.
//
// orig_img : image to draw on
// scale_factor : scale factor between subsampled image coordinates and orig_img
//                coordinates
// max_count : maximum number of candidates to draw, or -1 for infinite.
//
void CandidateList::OverlayCandidates(IemTPlane<short> &orig_img, int scale_factor, int max_count) const
{
    TextGen textgen;

    IemTPlane<short> results = orig_img;

    CandidateList::const_iterator set_iterator;

    int j=1;
    for(set_iterator = begin(); set_iterator != end(); ++set_iterator, ++j)
    {
        for(int i=-5; i<=5; i++)
        {
            if(set_iterator->row * scale_factor + i > 0 && set_iterator->col * scale_factor > 0 &&
               set_iterator->row * scale_factor + i < orig_img.rows()-1 && 
               set_iterator->col * scale_factor < orig_img.cols()-1)
                results[set_iterator->row*scale_factor+i][set_iterator->col*scale_factor] = 2048;
            if(set_iterator->row * scale_factor > 0 && set_iterator->col * scale_factor + i > 0 &&
               set_iterator->row * scale_factor < orig_img.rows()-1 && 
               set_iterator->col * scale_factor + i < orig_img.cols()-1)
            results[set_iterator->row*scale_factor][set_iterator->col*scale_factor+i] = 2048;
        }

        textgen.draw_number(results, j, 2, set_iterator->row * scale_factor - 6, 
                            set_iterator->col * scale_factor + 7, 2048);
                    
        if(j >= max_count && max_count != -1)
            break;
    }
}


//
// truncate_size
//
// returns a new list, consisting of the first max_count candidates of this
// CandidateList.
//
CandidateList CandidateList::truncate_size(int max_count) const
{
    CandidateList new_peak_list;

    CandidateList::const_iterator peak_iter;
    int i=0;
    for(peak_iter = begin(); peak_iter != end() && i < max_count; ++peak_iter, i++)
        new_peak_list.push_back(*peak_iter);

    return new_peak_list;
}


//
// resolve_multiple_peak_sets 
//
// analyzes peak_list1 and peak_list2, looking for candidates in one list that are nearly
// overlapping candidates in the other, and return a third list without the overlaps.
//
// This is currently accomplsihed by looking for candidates no more than 4 pixels apart.
// When overlapping candidates are found, the one with the higher coin value is chosen, 
// and the other is discarded.
//
CandidateList resolve_multiple_peak_sets(CandidateList &peak_list1, CandidateList &peak_list2)
{
    CandidateList::iterator peak_iter1 = peak_list1.begin();
    CandidateList new_peak_list;

	std::sort(peak_list1.begin(), peak_list1.end(), compare_candidates_by_coin());
	std::sort(peak_list2.begin(), peak_list2.end(), compare_candidates_by_coin());

    const int overlap_dist_thresh = 16;

    for( ; peak_iter1 != peak_list1.end(); ++peak_iter1)
    {
          // see whether this peak is within dist<=3 of any other peak
          // from the other list
        CandidateList::iterator peak_iter2 = peak_list2.begin();
        bool overlap = false;
        for( ; peak_iter2 != peak_list2.end(); ++peak_iter2)
            if(peak_iter1->distance_to_candidate(*peak_iter2) <= overlap_dist_thresh)
            {
                overlap = true;
                break;
            }
        
        if(overlap)
        {
            if(peak_iter1->coin_val > peak_iter2->coin_val)
            {
                Candidate peak(*peak_iter1);

                new_peak_list.push_back(peak);
            }
            else
            {
                Candidate peak(*peak_iter2);

                new_peak_list.push_back(peak);
            }

            std::cout << "abc "<<  peak_iter1->coin_val << " " << peak_iter2->coin_val << std::endl;
        }
        else
        {
            Candidate peak(*peak_iter1);

            new_peak_list.push_back(peak);
        }
    }

      // now add any from peak_list2 that are left over
    CandidateList::iterator peak_iter2 = peak_list2.begin();
    for( ; peak_iter2 != peak_list2.end(); ++peak_iter2)
    {
          // see whether this peak is within dist<=3 of any peak in the new list
        CandidateList::iterator new_peak_iter = new_peak_list.begin();
        bool overlap = false;
        for( ; new_peak_iter != new_peak_list.end(); ++new_peak_iter)
            if(new_peak_iter->distance_to_candidate(*peak_iter2) <= overlap_dist_thresh)
            {
                overlap = true;
                break;
            }

        if(!overlap)
        {
            Candidate peak(*peak_iter2);
            new_peak_list.push_back(peak);
        }

    }

      // keep new list sorted by COIN value
    std::sort(new_peak_list.begin(), new_peak_list.end(), compare_candidates_by_coin());

    return new_peak_list;
}
