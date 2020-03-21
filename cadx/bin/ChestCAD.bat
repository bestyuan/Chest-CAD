@echo on



SET IMAGENAME=%1
SET BASENAME=%~n1
SET OUTDIR=%2
SET TRUTHDIR=%3


REM Make the output directory if on does not already exist.
IF NOT EXIST %OUTDIR% MD %OUTDIR%


ChestCADApp -showAll -log -classData ../data/beta1p2_all.gml -lungParm ../data/lungmask/asmsearch.txt -leftLung ../data/lungmask/lungL.asm -rightLung ../data/lungmask/lungR.asm -parm ../data/cadx5.parm -i %IMAGENAME% -outDir %OUTDIR% -truth %TRUTHDIR%

