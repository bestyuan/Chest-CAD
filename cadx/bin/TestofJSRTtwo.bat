@echo on

SET OUTDIR=E:\图像数据\胸片\JSRTout\
SET INDIR=E:\图像数据\胸片\JSRT\
SET INDIRRIBSUP=E:\图像数据\胸片\JSRT\
SET IMAGELIST=E:\图像数据\胸片\JSRT\jsrt_nodules.lst
SET TRUTHDIR=E:\图像数据\胸片\TruthofJPRT\

FOR /F "tokens=1" %%i IN (%IMAGELIST%) DO ChestCADfortwo.bat %INDIR%/%%i %INDIRRIBSUP%/%%i %OUTDIR% %TRUTHDIR%



PAUSE
