@echo on

SET OUTDIR=E:\workingforUofC\newwork\output0.2\
SET INDIR=E:\workingforUofC\newwork\Imagesup0.2\
SET IMAGELIST=E:\workingforUofC\newwork\Imagesup0.2\jsrt_nodules.lst
SET TRUTHDIR=E:\workingforUofC\newwork\TruthofJPRT\

FOR /F "tokens=1" %%i IN (%IMAGELIST%) DO ChestCAD.bat %INDIR%/%%i %OUTDIR% %TRUTHDIR%



PAUSE
