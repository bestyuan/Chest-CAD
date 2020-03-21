

CalcFeaturesListApp -parm V:/schildkr_CADX/ISTL/cadx/data/cadx3.parm -list V:/schildkr_CADX/ISTL/cadx/lst/short.lst -log C:/cadx/cadx.log -imgDir L:/image/CADX/testAll/Images -inCanDir O:/cadx/testAll/IntermediateResults/initial_candidates_hand_masks -outDir C:/cadx -hitDir L:/image/CADX/testAll/GroundTruth -maskDir O:/cadx/testAll/IntermediateResults/LungSegmentation_hand

FeatureAnalysisApp -parm V:/schildkr_CADX/ISTL/cadx/data/cadx3.parm -list V:/schildkr_CADX/ISTL/cadx/lst/short.lst -canDir C:/cadx -hitDir L:/image/CADX/testAll/GroundTruth -o C:/cadx/cadx.stat -plot C:/cadx/cadx.plot


