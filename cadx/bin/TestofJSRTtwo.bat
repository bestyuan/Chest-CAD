@echo on

SET OUTDIR=E:\ͼ������\��Ƭ\JSRTout\
SET INDIR=E:\ͼ������\��Ƭ\JSRT\
SET INDIRRIBSUP=E:\ͼ������\��Ƭ\JSRT\
SET IMAGELIST=E:\ͼ������\��Ƭ\JSRT\jsrt_nodules.lst
SET TRUTHDIR=E:\ͼ������\��Ƭ\TruthofJPRT\

FOR /F "tokens=1" %%i IN (%IMAGELIST%) DO ChestCADfortwo.bat %INDIR%/%%i %INDIRRIBSUP%/%%i %OUTDIR% %TRUTHDIR%



PAUSE
