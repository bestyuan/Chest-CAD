

CalcFeaturesListApp -log -parm V:/schildkr_CADX/ISTL/cadx/data/cadx4_rendered.parm -list V:/schildkr_CADX/ISTL/cadx/lst/jsrt_nodules_tif.lst -imgDir L:/schildkraut/cadx/JSRT/rendered -inCanDir O:/cadx/JSRT/IntermediateResults/InitialCandidates_20040711 -outDir Q:/schildkr/cadx/JSRT/run -hitDir L:/singhal/LungSegImages/JSRT/GroundTruth -maskDir O:/cadx/JSRT/IntermediateResults/LungSegmentation_20040711



rem CalcFeaturesListApp -log -parm V:/schildkr_CADX/ISTL/cadx/data/cadx4.parm -list V:/schildkr_CADX/ISTL/cadx/lst/jsrt_nodules.lst -imgDir L:/singhal/LungSegImages/JSRT/Images/CorrectOrientation -inCanDir O:/cadx/JSRT/IntermediateResults/InitialCandidates_20040711 -outDir Q:/schildkr/cadx/JSRT/run -hitDir L:/singhal/LungSegImages/JSRT/GroundTruth -maskDir O:/cadx/JSRT/IntermediateResults/LungSegmentation_20040711



FeatureAnalysisApp -parm V:/schildkr_CADX/ISTL/cadx/data/cadx4.parm -list V:/schildkr_CADX/ISTL/cadx/lst/jsrt_nodules.lst -canDir Q:/schildkr/cadx/JSRT/run -hitDir L:/singhal/LungSegImages/JSRT/GroundTruth -o Q:/schildkr/cadx/JSRT/run/cadx.stat -plot Q:/schildkr/cadx/JSRT/run/cadx.plot
