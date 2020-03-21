#include "IcdXrayImage.h"
#include "LungMask.h"
#include <vector>
#include "Mhac.h"
#include <algorithm>

//
// MultiscaleMhac : perform Mhac filtering at multiple window sizes
//

class MultiscaleMhac
{
    public:
          // constructor, constructs a MultiscaleMhac object that applies the scales listed in _scales
        MultiscaleMhac(const std::vector<int> &_scales);

          // run multiscale mhac on an image
        CandidateList run_multiscale_mhac(IcdXrayImage &input_image, const LungMask &mask, int down_sample);

    protected:
        std::vector<int> scales;
        Mhac *mhacs;
        CandidateList *peak_lists;
};
