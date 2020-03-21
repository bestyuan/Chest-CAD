@echo on


SET TMP=%3
SET BASENAME=%~n1
SET IMAGENAME=%1
SET IMAGEDIR=%2
SET OUTDIR=%3
SET MASKDIR=%4
SET HITDIR=%5


REM Make a tmp directory if on does not already exist.
IF NOT EXIST %TMP% MD %TMP%


REM Convert the rendered image to a TIFF file.
convert.exe -i %IMAGEDIR%/%IMAGENAME% -o %TMP%/%BASENAME%_8bit.tif

REM Create lung mask.
AsmTest.exe -i %TMP%/%BASENAME%_8bit.tif -p AsmTest/asmsearch.txt -l AsmTest/lungL.asm -r AsmTest/lungR.asm -mask %OUTDIR%/%BASENAME%_mask.png -overlay %TMP%/%BASENAME%_lung_overlay.tif -subsample 4 -noorient

REM Initial candidate detection.
rem IcdSegApp.exe -log -parm ../data/cadx5.parm -img %IMAGEDIR%/%IMAGENAME% -hit %HITDIR%/%BASENAME%.hit -mask %OUTDIR%/%BASENAME%_mask.png -outCan %TMP%/%BASENAME%.can -outDir %TMP%
IcdSegApp.exe -log -parm ../data/cadx5.parm -img %IMAGEDIR%/%IMAGENAME% -hit %HITDIR%/%BASENAME%.hit -mask %OUTDIR%/%BASENAME%_mask.png -outCan %TMP%/%BASENAME%.can -outDir %TMP%

REM Classify all candidates.
rem ClassifierApp.exe -i %TMP%/%BASENAME%.can -o %TMP%/%BASENAME%.can -data ../data/August272004_AutoLungMask_AllCRandJSRT.gml

REM Annotate the rendered image.
AnnotateApp2.exe -parm ../data/cadx5.parm -i %IMAGEDIR%/%IMAGENAME% -o %OUTDIR%/%BASENAME%_annotated.tif -can %TMP%/%BASENAME%.can -hit %HITDIR%/%BASENAME%.hit -report %OUTDIR%/%BASENAME%_report.txt -minProbability 0.0


REM erase /Q %TMP%/*.*

REM pause
