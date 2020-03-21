#include "MultiscaleMhac.h"

//
// MultiscaleMhac : perform Mhac filtering at multiple window sizes
//

// construct a MultiscaleMhac object, specify scales of interest
// 
// _scales : vector of integers, specifying radii of interest
//
// Note: This class currently treats the first scale differently than others,
// in that all candidates detected at the first scale are kept, but only the top
// n (currently 50) are kept for other scales. Hence you should list your 
// "best" scale "first", i.e. _scales[0] should probably be 15.
//
MultiscaleMhac::MultiscaleMhac(const std::vector<int> &_scales)
{
    scales = _scales;

    mhacs = new Mhac[scales.size()];
    peak_lists = new CandidateList[scales.size()];

}

// run multiscale mhac
//
// input_image : IcdXrayImage object containing (probably sub-sampled) xray image
// mask : lung segmentation mask
// down_sample : sub-sampling factor that was used to sub-sample input_image
//
// Note that currently this method only supports "biscale" processing (2 scales), 
// although it could be easily modified to handle true multi-scale.
//
CandidateList MultiscaleMhac::run_multiscale_mhac(IcdXrayImage &input_image, const LungMask &mask, int down_sample)
{
      // loop through each scale (radius), performing MHAC on each
    for(int i=0; i<scales.size(); i++)
    {
        mhacs[i].set_radius(scales[i]);
        
          // run MHAC
        peak_lists[i] = mhacs[i].generate_initial_candidates(input_image, mask, down_sample);

          // remove nearly-overlapping candidates from the mhac output at each radius
        peak_lists[i] = peak_lists[i].remove_overlapping_peaks();
        
          // and sort by coin value
        std::sort(peak_lists[i].begin(), peak_lists[i].end(), compare_candidates_by_coin());
        std::cout << "peak count " << i << " " << peak_lists[i].size() << std::endl;
    }
    
    CandidateList peak_list;
      // if one scale, just use peak_lists[0]
    if(scales.size() == 1)
        peak_list = peak_lists[0];
      // if two scales, truncate the second scale at 50 (arbitrary), then merge lists together
    else if(scales.size() == 2)
    {
        peak_lists[1] = peak_lists[1].truncate_size(50);
        
        peak_list = resolve_multiple_peak_sets(peak_lists[0], peak_lists[1]); 
    }
    else
        throw(std::string("only 2 scales currently supported\n"));
        
    return peak_list;
}
