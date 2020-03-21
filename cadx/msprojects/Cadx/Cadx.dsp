# Microsoft Developer Studio Project File - Name="Cadx" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Cadx - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Cadx.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Cadx.mak" CFG="Cadx - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Cadx - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Cadx - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "Cadx"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Cadx - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\lib\tnt" /I "..\..\lib\ASMLibrary\inc" /I "..\..\lib\dicom\kfc_i3r2\kfc\Dependant_Libs\Repertoire" /I "..\..\lib\dicom\kdt_i13\Kodak DICOM Toolkit" /I "..\..\lib\dicom\kdt_i13\Kodak DICOM Toolkit\dicom_toolkit" /I "..\kfc_i3r2\kfc\Dependant_Libs\Repertoire" /I "../../lib/dicom/DicomMRIIOLibrary" /I "../../lib/AlgoUtils/inc" /I "../../lib/ImageUtils/inc" /I "../../lib/EquivList/inc" /I "../../lib/RegionLabel/inc" /I "../../src/util/decomposition" /I "../../inc" /I "../../src/util/polyfit" /I "../../lib/png/zlib" /I "../../lib/png/lpng125" /I "../../lib/IEM/version-233/inc" /I "../../lib/EKC/inc" /I "../../lib/NR/inc" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_AFXDLL" /FR /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../lib/Cadx.lib"

!ELSEIF  "$(CFG)" == "Cadx - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\lib\tnt" /I "..\..\lib\ASMLibrary\inc" /I "..\..\lib\dicom\kfc_i3r2\kfc\Dependant_Libs\Repertoire" /I "..\..\lib\dicom\kdt_i13\Kodak DICOM Toolkit" /I "..\..\lib\dicom\kdt_i13\Kodak DICOM Toolkit\dicom_toolkit" /I "..\kfc_i3r2\kfc\Dependant_Libs\Repertoire" /I "../../lib/dicom/DicomMRIIOLibrary" /I "../../lib/AlgoUtils/inc" /I "../../lib/ImageUtils/inc" /I "../../lib/EquivList/inc" /I "../../lib/RegionLabel/inc" /I "../../src/util/decomposition" /I "../../inc" /I "../../src/util/polyfit" /I "../../lib/png/zlib" /I "../../lib/png/lpng125" /I "../../lib/IEM/version-233/inc" /I "../../lib/EKC/inc" /I "../../lib/NR/inc" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_AFXDLL" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../lib/CadxD.lib"

!ENDIF 

# Begin Target

# Name "Cadx - Win32 Release"
# Name "Cadx - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\features\AfumFeature.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\util\Annotator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\preprocessing\BlurFilter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\util\Boundary.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\data\CadxParm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\features\CalcFeatures.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\data\Can.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\util\Canny.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\util\Chain.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\util\ChainMaker.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\lungmask\Chest_PA_AP.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\segmentation\Cluster.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\segmentation\Clusterer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\data\Cnl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\util\decomposition\contour_pt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\util\decomposition\contours.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\util\ConvexHull.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\util\Cooccurrence.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\features\Culler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\segmentation\Decomposition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\preprocessing\EdgeDetector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\features\Feature.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\features\FeatureSet.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\util\polyfit\getError.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\util\gfft.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\cppclassifier\GmlClassifier.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\data\GmlData.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\cppclassifier\GmlProb.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\preprocessing\Gradient.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\features\GradientFeature.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\util\Histogram.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\data\Hit.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ICD\Icd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ICD\IcdFeatures.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\util\IemDicom.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\util\ImageReader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\util\ImageUtil.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\util\Kespr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\lungmask\LungMaskFunctions.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\lungmask\LungMaskMaker.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\util\polyfit\matrixMultiply.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\util\polyfit\matrixTranspose.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ICD\MhacFeature.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\preprocessing\MorphFilter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\preprocessing\MorphPreprocessor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\segmentation\MorphSegmenter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\preprocessing\MultiMorphFilter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\features\OverlapFeature.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\util\PeakDetector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\util\PNGWriter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\util\PolyFit.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\util\Polynomial.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\preprocessing\Preprocessor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\preprocessing\Preprocessor2.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\features\RegionFeature.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\util\RegionLabel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\segmentation\RegionSupport.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\data\Report.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\features\Roi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\segmentation\ScaleSelector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\preprocessing\ScalingFilter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\segmentation\SegFeatures.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\segmentation\SegmenterA.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\features\ShapeFeature.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\features\SurfaceFeature.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\features\TextureFeature.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\util\Tokenizer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\util\UserAgent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\util\Util.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\segmentation\Watershed.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
