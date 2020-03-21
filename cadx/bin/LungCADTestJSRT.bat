@echo on

SET OUTDIR=L:/LungCAD/results/jsrt/run
rem SET OUTDIR=c:/tmp
rem SET IMAGEDIR=L:/singhal/LungSegImages/JSRT/Images/CorrectOrientation
SET IMAGEDIR=L:/LungCAD/jsrt/images/rendered_EDEP
rem SET MASKDIR=L:/schildkraut/cadx/JSRT/IntermediateResults/LungSegmentation_20040711
SET MASKDIR=L:/singhal/LungSegImages/JSRT/LungMaskGtTiff
SET HITDIR=L:/singhal/LungSegImages/JSRT/GroundTruth
rem SET IMAGELIST=../lst/JSRT_nodules.lst
SET IMAGELIST=../lst/JSRT.lst

FOR /F "tokens=1" %%i IN (%IMAGELIST%) DO LungCAD.bat %%i %IMAGEDIR% %OUTDIR% %MASKDIR% %HITDIR%

FeatureAnalysisApp -parm ../data/cadx5.parm -list %IMAGELIST% -canDir %OUTDIR% -hitDir %HITDIR% -o %OUTDIR%/cadx.stat -plot %OUTDIR%/cadx.plot

PAUSE
