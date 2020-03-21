@echo on

rem SET OUTDIR=Q:/schildkr/cadx/AllCR/run
SET OUTDIR=L:/LungCAD/results/AllCR/run
rem SET OUTDIR=c:/tmp
rem SET IMAGEDIR=L:/singhal/LungSegImages/All_CR/Images/CorrectOrientation
SET IMAGEDIR=L:/LungCAD/All_CR/images/rendered_EDEP
SET MASKDIR=L:/singhal/LungSegImages/All_CR/GroundTruth/lungGT
SET HITDIR=L:/singhal/LungSegImages/All_CR/GroundTruth
SET IMAGELIST=../lst/AllCR.lst


rem FOR /F "tokens=1" %%i IN (%IMAGELIST%) DO LungCAD.bat %%i %IMAGEDIR% %OUTDIR% %MASKDIR% %HITDIR%

FeatureAnalysisApp -parm ../data/cadx5.parm -list %IMAGELIST% -canDir %OUTDIR% -hitDir %HITDIR% -o %OUTDIR%/cadx.stat -plot %OUTDIR%/cadx.plot

PAUSE
