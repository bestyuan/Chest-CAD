#include "Candidate.h"
#include <math.h>
#include "TextGen.h"
#include "ImageTruth.h"
#include "IemMorphological.h"
#include "CanFile.h"
#ifndef WIN32
#include <sys/times.h>
#endif
#include "IemLinearArray.h"
#include "IemLinearArrayFxns.h"
#include "IemHistogram.h"
#include "GmlClassifier.h"
#include <algorithm>
#include "Afum.h"
#include "Mhac.h"
#include "IcdUtils.h"
#include "IcdImgProc.h"
#include "IcdFeatureManager.h"
#include <iostream>
#include "CandidateList.h"
#include "MultiscaleMhac.h"
using namespace std;

// I am terribly, terribly sorry, but I have the annoying habit of making
// command line options be global variables in research code. 
// At least I always put a _g suffix on them so that global variables
// can be quickly found and eliminated.
bool debug_g=0;
bool multiscale_g = false;
bool norevisemask_g = false;
bool nobug_fix_g = false;
bool noclassify_g = false;
bool loo_g = true;
int maxcancount_g = 0;
int multiscale_count_g = 1;
std::vector<int> multiscale_scales_g;

#ifdef WIN32
// This hideously ugly hack is required because Microsoft Visual C++ 6.0
// does not properly implement the ANSI C++ standard regarding variables
// declared within 'for' statements.
#define for if(0) ; else for
#endif


//using namespace std;

const char *version_info[] = {"This is an IEM/C++ version of Bob Senn's MHAC algorithm, ",
                              " with the following additions: ",
                              "   - Code to reduce false alarms around lung mask boundaries",
                              "   - Lung mask growing to detect nodules on or near lung mask",
                              "     boundaries",
                              "   - Additional features and a built-in GML classifier to",
                              "     produce true positive probabilities for each nodule",
                              "   - (optional) multi-resolution processing to detect small",
                              "     nodules, only enabled if -multiscale option is given",
                              0};

ostream &operator<<(ostream &os, const Candidate &pk)
{
    os << pk.row * pk.scale_factor + pk.scale_factor / 2 - 1 << " " 
       << pk.col * pk.scale_factor + pk.scale_factor / 2 - 1 << " " << pk.coin_val;

    return os;
}



#ifndef WIN32
struct tms beg_proc_times;
#endif


void help()
{
    std::cout << std::endl;
    std::cout << "usage: ccICD [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "  where " << std::endl;
    std::cout << std::endl;
    std::cout << " required parameters: " << std::endl;
    std::cout << "   -image [filename]        specify input x-ray image (KESPR format)" << std::endl;
    std::cout << "   -lungmask [filename]     specify lung mask image file" << std::endl;
    std::cout << std::endl;
    std::cout << " operating modes: " << std::endl;
    std::cout << "      [these are just shortcuts that invoke combinations of " << std::endl;
    std::cout << "       the parameters below] " << std::endl;
    std::cout << "   -dec2002                 replicate the results of the java ICD code" << std::endl;
    std::cout << "                              of December 2002, expands to: " << std::endl;
    std::cout << "                              -noclassify -norevisemask -nobugfix  " << std::endl;
    std::cout << "                              -maxcancount 50" << std::endl;   
    std::cout << "   -rec0703                 recommended best parameter set for July 2003 release" << std::endl;
    std::cout << "                              to Health Imaging, expands to: " << std::endl;
    std::cout << "                              -noclassify -norevisemask -ms 2 15 7 " << std::endl;
    std::cout << "                              -maxcancount 70" << std::endl;
    std::cout << std::endl;
    std::cout << " optional parameters: " << std::endl;
    std::cout << "   -debug                   enable debugging (output of status messages " << std::endl;
    std::cout << "                              and intermediate images)" << std::endl;
    std::cout << "   -ms [num] [s1] [s2] ...  enable multiscale processing (experimental) " << std::endl;
    std::cout << "                              'num' indicates number of scales that are " << std::endl;
    std::cout << "                              to be considered. This must be followed by" << std::endl;
    std::cout << "                              'num' integers (s1, s2, s3...) indicating " << std::endl;
    std::cout << "                              the COIN/MHAC radius that should be used" << std::endl;
    std::cout << "                              at each scale " << std::endl;
    std::cout << "   -gt [filename]           specify ground truth (.hit) file" << std::endl;
    std::cout << "                              this is required for testing/training and" << std::endl;
    std::cout << "                              if you want gt info output in the .can file" << std::endl;
    std::cout << "   -norevisemask            turns off lung mask 'revising' code that tries" << std::endl;
    std::cout << "                              to improve localization accuracy of lung masks" << std::endl;
    std::cout << "   -gml [filename]          specifies name of file containing trained parameters" << std::endl;
    std::cout << "                              for internal GML classifier (default: trained.gml)" << std::endl;
    std::cout << "   -nobugfix                emulates the bug found in the original java ICD" << std::endl;
    std::cout << "                              code which caused false positives along lung" << std::endl;
    std::cout << "                              boundaries (useful for replicating prior results)" << std::endl;
    std::cout << "   -noclassify              turns 'off' internal classifier" << std::endl;
    std::cout << "   -maxcancount [n]         output at most n candidates per image in .can files" << std::endl;
    std::cout << "                              (0 = infinite, which is the default)" << std::endl;
    std::cout << "   -noloo                   don\'t operate internal classifier in \'leave one out\'" << std::endl;
    std::cout << "                              testing mode ('L method' of Fukunaga), which is" << std::endl;
    std::cout << "                              most fair and hence the default, instead using the" << std::endl;
    std::cout << "                              replacement ('R method')" << std::endl;
    std::cout << std::endl;

}

bool parse_command_line(int argc, char *argv[], std::string &img_fname, std::string &gt_fname, std::string &lungmask_fname,
                        std::string &gml_fname)
{
    int index = 1;

    if(argc <= 1)
    {
        help();
        return false;
    }

    while(index < argc)
    {
        std::string parm(argv[index]);

        if(parm == "-image")
        {
            index++;
            img_fname = argv[index++];
        }
        else if(parm == "-gt")
        {
            index++;
            gt_fname = argv[index++];
        }
        else if(parm == "-lungmask")
        {
            index++;
            lungmask_fname = argv[index++];
        }
        else if(parm == "-debug")
        {
            index++;
            debug_g = true;
        }
        else if(parm == "-ms")
        {
            index++;
            multiscale_g = true;
            multiscale_count_g = atoi(argv[index++]);
            for(int i=0; i<multiscale_count_g; i++)
                multiscale_scales_g.push_back(atoi(argv[index++]));
        }
        else if(parm == "-norevisemask")
        {
            index++;
            norevisemask_g = true;
        }
        else if(parm == "-nobugfix")
        {
            index++;
            nobug_fix_g = true;
        }
        else if(parm == "-noclassify")
        {
            index++;
            noclassify_g = true;
        }
        else if(parm == "-gml")
        {
            index++;
            gml_fname = argv[index++];
        }
        else if(parm == "-maxcancount")
        {
            index++;
            maxcancount_g = atoi(argv[index++]);
        }
        else if(parm == "-noloo")
        {
            index++;
            loo_g = false;
        }
        else if(parm == "-dec2002")
        {
            index++;
            norevisemask_g = true;
            nobug_fix_g = true;
            noclassify_g = true;
            maxcancount_g = 50;
            multiscale_g = false;
        }
        else if(parm == "-rec0703")
        {
            index++;
            norevisemask_g = true;
            nobug_fix_g = false;
            noclassify_g = true;
            maxcancount_g = 70;
            multiscale_g = true;

            multiscale_count_g = 2;
            multiscale_scales_g.push_back(15);
            multiscale_scales_g.push_back(7);
        }
        else
        {
            std::cout << std::endl;
            std::cout << "unexpected parameter: " << parm << std::endl;
            help();
            return false;
        }
    }
    
    return true;

}
                        

int main(int argc, char *argv[])
{
    std::string image_fname, gt_fname, lungmask_fname, gml_fname;

    gml_fname = "trained.gml";

    if(!parse_command_line(argc, argv, image_fname, gt_fname, lungmask_fname, gml_fname))
        return 1;


#ifndef WIN32
    times(&beg_proc_times);
#endif

    try {
        const int down_sample = 4;

          // load in the x-ray image
        IcdXrayImage big_input_image(image_fname.c_str());
        IemImage img = IemImage(big_input_image.get_xray_image()).copy();

          // down sample image (currently to 1/4 the size)
        IcdImgProc<short> iip;
        for(int ds=down_sample; ds != 1; ds /= 2)
            img = iip.downSample2x(IemTImage<short>(img));

          // make an IcdXrayImage out of the subsampled version
        IcdXrayImage input_image(img[0]);

          // load in the lung mask
        LungMask lung_mask_orig;
        lung_mask_orig = iemRead(lungmask_fname.c_str())[0];

          // resize the lung mask, to the same size as the *subsampled* x-ray
          // This could involve either subsampling or upsampling. In our (ISD) environment,
          // it involves upsampling, but M. Heath uses masks already upsampled to the original
          // x-ray size, hence his masks need to be downsampled to the subsampled x-ray size.
          // Understand? :)
        LungMask lung_mask = lung_mask_orig.copy();
        double mask_factor = input_image.rows()/double(lung_mask.rows());
        if(mask_factor > 1.5)
            lung_mask = iemReplicate(IemImage(lung_mask_orig), mask_factor, mask_factor)[0];
        else if(mask_factor < 0.8)
            lung_mask = iemSubsample(IemImage(lung_mask_orig), int(1.0/mask_factor), int(1.0/mask_factor))[0];
        lung_mask = iemPad(lung_mask, input_image.rows(), input_image.cols());

          // Do the lung mask revision step, if requested
        LungMask revised_lung_mask;
        if(!norevisemask_g)
            revised_lung_mask = lung_mask.revise_lung_mask(input_image.get_xray_image());
        else
            revised_lung_mask = lung_mask.copy();

          // set up debugging images
        int overlaid_count = 50;
        IemTImage<short> out_img, out_img_sub;
        if(debug_g)
        {
            out_img = IemTImage<short>(3, big_input_image.rows(), big_input_image.cols());
            out_img[0] = (2048 - big_input_image.get_xray_image());
            out_img[1] = (2048 - big_input_image.get_xray_image());
            out_img[2] = (2048 - big_input_image.get_xray_image());
            out_img_sub = IemTImage<short>(3, input_image.rows(), input_image.cols());
            out_img_sub[0] = (2048 - input_image.get_xray_image());
            out_img_sub[1] = (2048 - input_image.get_xray_image());
            out_img_sub[2] = (2048 - input_image.get_xray_image());
        }

          // load ground truth (from .hit file), if we have it
        ImageTruth gt;
        bool have_gt = false;
        if(gt_fname!="")
        {
            have_gt = true;
            gt.LoadHitFile(gt_fname);
            if(debug_g)
            {
                gt.OverlayTruth(out_img);
                gt.OverlayTruth(out_img_sub, down_sample);
            }
        }
        
          // Register the features we want to compute with the feature manager.
          // Note: We use two feature managers. This is because the original 7 features from
          // 2002 were computed on the original x-ray image, while the features developed
          // during 2003 (up to 7/2003) operate on the subsampled x-rays. Hence we use
          // two different feature managers, so that we can pass one the subampled images
          // and the other the original images.
        IcdFeatureManager ifm_original7;
        IcdFeatureManager ifm_newfeatures;
        
        std::cout << "registering features..." << std::endl;
        ifm_original7.register_feature("rdens", new icdfRelativeDensity);
        ifm_original7.register_feature("lvar", new icdfBSLocalVar(7));
        ifm_original7.register_feature("hloc", new icdfHorizontalLocation);
        ifm_original7.register_feature("vloc", new icdfVerticalLocation);
        ifm_original7.register_feature("afum", new icdfAfum(30, 10));
        ifm_original7.register_feature("grad", new icdfWinGrad(1));
        
        ifm_newfeatures.register_feature("LocalVar11", new icdfLocalVar(11));
        ifm_newfeatures.register_feature("LocalVar21", new icdfLocalVar(21));
        ifm_newfeatures.register_feature("GMVar11", new icdfGMVar(11));
        ifm_newfeatures.register_feature("GMVar21", new icdfGMVar(21));
        ifm_newfeatures.register_feature("YDiff", new icdfYDiff(10));
        ifm_newfeatures.register_feature("PtGrad", new icdfWinGrad(1));
        ifm_newfeatures.register_feature("WinGrad", new icdfWinGrad(3));
        ifm_newfeatures.register_feature("ProSymMean", new icdfProfileSymMean(15));
        ifm_newfeatures.register_feature("ProSymVar", new icdfProfileSymVar(15));
        ifm_newfeatures.register_feature("GProSymMean", new icdfGradProfileSymMean(15));
        ifm_newfeatures.register_feature("GProSymVar", new icdfGradProfileSymVar(15));
        ifm_newfeatures.register_feature("NormYPos", new icdfNormYPos);


        int i=0;
        
        
        if(multiscale_scales_g.size() == 0)
            multiscale_scales_g.push_back(15);
        
        MultiscaleMhac mmhac(multiscale_scales_g);
        CandidateList peak_list = mmhac.run_multiscale_mhac(input_image, revised_lung_mask, down_sample);


          // Compute the features. Again, we pass the original ("big") x-ray image to
          // the original 7 features feature manager, and the subsampled image to
          // the newfeatures manager.
        std::cout << "computing features..." << std::endl;
        LungMask big_mask = iemReplicate(revised_lung_mask, down_sample, down_sample);
        ifm_newfeatures.compute_features(input_image, revised_lung_mask, peak_list);
        ifm_original7.compute_features(big_input_image, big_mask, peak_list, down_sample);


        if(debug_g)
        {
            lung_mask.overlay_lung_boundaries(out_img_sub);
            revised_lung_mask.overlay_lung_boundaries(out_img_sub);
        }

          // Load the classifier, if requested.
        GmlClassifier gml;
        if(!noclassify_g)
            gml.loadFromFile(gml_fname.c_str());

        int hit_count = 0;
        int fa_count = 0;
        std::vector<TrueNodule> hits;
        
        CandidateList::iterator peak_iter;
        for(i=0, peak_iter = peak_list.begin(); peak_iter != peak_list.end(); ++peak_iter, i++)
        {
            double prob = 0;
            
            if(!noclassify_g)
            {
                double features[10];

                features[0] = peak_iter->features["CoinNorm"];
                features[1] = peak_iter->features["GMVar11"];
                features[2] = peak_iter->features["GMVar21"];
                features[3] = peak_iter->features["LocalVar11"];
                features[4] = peak_iter->features["LocalVar21"];
                features[5] = peak_iter->features["NormYPos"];
                features[6] = peak_iter->features["ProSymMean"];
                features[7] = peak_iter->features["ProSymVar"];
                features[8] = peak_iter->features["rdens"];
                features[9] = peak_iter->features["vloc"];

                if(loo_g)
                {
                    if(!have_gt)
                        throw std::string("-gt parameter must be specified if -loo is used");
                    
                    const TrueNodule *tn = gt.hit_test(Point(peak_iter->row * down_sample, peak_iter->col * down_sample));
                    unsigned int class_label = 1; // default to non-nodule
                    if(tn && (tn->get_status() == probable || tn->get_status() == definite))
                        class_label = 0;
                    
                    double *beliefs = gml.getBeliefs_L_method(features, class_label);
                    prob = beliefs[0];
                }
                else
                {
                    double *beliefs = gml.getBeliefs(features);
                    prob = beliefs[0];
                }
            }
            
            peak_iter->nodule_prob = prob;
        }
        
        std::sort(peak_list.begin(), peak_list.end(), compare_candidates_by_coin());
        if(maxcancount_g != 0)
            peak_list = peak_list.truncate_size(maxcancount_g);
        std::sort(peak_list.begin(), peak_list.end(), compare_candidates_by_prob());
        
          // do this stuff only if we have gt available
        if(have_gt)
        {
            CandidateList::iterator peak_iter;
            for(i=0, peak_iter = peak_list.begin(); peak_iter != peak_list.end(); ++peak_iter, i++)
            {
                const TrueNodule *tn;
                
                tn = gt.hit_test(Point(peak_iter->row * down_sample, peak_iter->col * down_sample));
            
                bool already_hit = false;
                  // have we already hit this one? if so, don't count again.
                if(tn && find(hits.begin(), hits.end(), *tn) != hits.end())
                    already_hit = true;
                
                if(!already_hit && tn)
                    hits.push_back(*tn);
                
                Status stat;
                if(tn) 
                    stat = tn->get_status();
                else
                    stat = falsealarm;
                
                if(!already_hit)
                {
                    if((stat == probable || stat == definite))
                        hit_count++;
                    else if(stat != possible)
                        fa_count++;
                }
                
                std::cout << "training_data " << *peak_iter << " " << int(stat) << " " ;
                std::map<std::string, double>::iterator feature_iter;
                for(feature_iter = peak_iter->features.begin(); feature_iter != peak_iter->features.end(); feature_iter++)
                    std::cout << feature_iter->second << " ";
                std::cout << "\n";
        
                if(!already_hit)
                {
                    std::cout << "testing_data " << peak_iter->row << " " << peak_iter->col << " " << peak_iter->nodule_prob
                         << " " << int(stat) << std::endl;
                    std::cout << "training_fortest_data " << *peak_iter << " " << int(stat) << " " ;
                    std::map<std::string, double>::iterator feature_iter;
                    for(feature_iter = peak_iter->features.begin(); feature_iter != peak_iter->features.end(); feature_iter++)
                        std::cout << feature_iter->second << " ";
                    std::cout << "\n";
                }
            
            }
        }
        

        IemTImage<short> out_img_sub2;
        if(debug_g)
        {
            out_img_sub2 = out_img_sub.copy();

            peak_list.OverlayCandidates(out_img_sub[1], 1, -1); //, overlaid_count);
            peak_list.OverlayCandidates(out_img_sub2[1], 1, -1); //, overlaid_count);
        }

        std::cout << "Candidate count: " << peak_list.size() << "\n";
        if(have_gt)
        {
            std::cout << "Hit count: " << hit_count << "\n";
            std::cout << "GT true count: " << gt.count_by_status(probable) + gt.count_by_status(definite) << "\n";
            std::cout << "False neg count: " << gt.count_by_status(probable) + gt.count_by_status(definite) - hit_count << "\n";
        }
        
        char temp[500], temp2[500];
        
        IcdUtils icdu;
        if(debug_g)
        {
            sprintf(temp, "%s_outimg.tif", icdu.basename(image_fname, false, true).c_str());
            iemWrite(out_img, temp);
            sprintf(temp, "%s_outimg_sub.tif", icdu.basename(image_fname, false, true).c_str());
            iemWrite(out_img_sub, temp);
            sprintf(temp, "%s_outimg_sub2.tif", icdu.basename(image_fname, false, true).c_str());
            iemWrite(out_img_sub2, temp);
        }
        
        strcpy(temp2, icdu.basename(image_fname, true, true).c_str());

          // Output the can file
        sprintf(temp, "%s.can", temp2);
        CanFile cf;
        cf.write_can_file(temp, &peak_list, have_gt, &gt, icdu.basename(image_fname, false, true), 
                          big_input_image.get_xray_image(), down_sample, argc, argv, maxcancount_g);
        
        
    }
    catch(IemError &err)
    {
        std::cout << err << std::endl;
    }
    catch(std::string &str)
    {
        std::cout << str << std::endl;
    }
    catch(...)
    {
        std::cout << "caught an exception\n";
    }

    return 0;
}
