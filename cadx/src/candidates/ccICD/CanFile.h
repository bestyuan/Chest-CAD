#include "ImageTruth.h"
#include "CandidateList.h"

//
// CanFile : encapsulates I/O of .can files
//
// Currently this class only supports output of CAN files, not input, since
// ICD has no use for CAN file input, at least now. 
//

class CanFile
{
    public:
          // write a CandidateList out to a can file.
          // Note: all these extra parameters (argc, argv, etc) are just for including
          // debugging info in the canfile
        void CanFile::write_can_file(char *filename, CandidateList *peaks, bool have_gt, ImageTruth *truth, 
                                     std::string image_filename, const IemImage &img, int down_sample, int argc, 
                                     char *argv[], int max_can_count);

};
