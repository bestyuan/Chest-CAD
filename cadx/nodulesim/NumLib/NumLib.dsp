# Microsoft Developer Studio Project File - Name="NumLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=NumLib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "NumLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "NumLib.mak" CFG="NumLib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "NumLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "NumLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/NumLib", VKLAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "NumLib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "NumLib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\NumLib" /I "\WinBarco_09_25_00" /I "..\VSS\NumLib" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "NumLib - Win32 Release"
# Name "NumLib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\GSDF.cpp
# End Source File
# Begin Source File

SOURCE=.\NL2DFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\NL2DPolySurface.cpp
# End Source File
# Begin Source File

SOURCE=.\NLBioWav.cpp
# End Source File
# Begin Source File

SOURCE=.\NLConversions.cpp
# End Source File
# Begin Source File

SOURCE=.\NLDyadWav.cpp
# End Source File
# Begin Source File

SOURCE=.\NLDydPyr.cpp
# End Source File
# Begin Source File

SOURCE=.\NLFFT.cpp
# End Source File
# Begin Source File

SOURCE=.\NLFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\NLFit.cpp
# End Source File
# Begin Source File

SOURCE=.\NLHist.cpp
# End Source File
# Begin Source File

SOURCE=.\NLKespr.cpp
# End Source File
# Begin Source File

SOURCE=.\NLMRA.cpp
# End Source File
# Begin Source File

SOURCE=.\NLNoiseEst.cpp
# End Source File
# Begin Source File

SOURCE=.\NLOrientMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\NLPyramid.cpp
# End Source File
# Begin Source File

SOURCE=.\NLRan.cpp
# End Source File
# Begin Source File

SOURCE=.\NLScatterRed.cpp
# End Source File
# Begin Source File

SOURCE=.\NLSigFilt.cpp
# End Source File
# Begin Source File

SOURCE=.\NLSolve.cpp
# End Source File
# Begin Source File

SOURCE=.\NLSort.cpp
# End Source File
# Begin Source File

SOURCE=.\NLSpecFct.cpp
# End Source File
# Begin Source File

SOURCE=.\NLTonescale.cpp
# End Source File
# Begin Source File

SOURCE=.\NLWavNoiseRed.cpp
# End Source File
# Begin Source File

SOURCE=.\NLWireWheel.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\GSDF.h
# End Source File
# Begin Source File

SOURCE=.\NL2DArr.h
# End Source File
# Begin Source File

SOURCE=.\NL2DFilter.h
# End Source File
# Begin Source File

SOURCE=.\NLArray.h
# End Source File
# Begin Source File

SOURCE=.\NLBdyExt.h
# End Source File
# Begin Source File

SOURCE=.\NLComplex.h
# End Source File
# Begin Source File

SOURCE=.\NLConstants.h
# End Source File
# Begin Source File

SOURCE=.\NLConversions.h
# End Source File
# Begin Source File

SOURCE=.\NLFFT.h
# End Source File
# Begin Source File

SOURCE=.\NLFilter.h
# End Source File
# Begin Source File

SOURCE=.\NLFit.h
# End Source File
# Begin Source File

SOURCE=.\NLHist.h
# End Source File
# Begin Source File

SOURCE=.\NLInternal.h
# End Source File
# Begin Source File

SOURCE=.\NLInterpolate.h
# End Source File
# Begin Source File

SOURCE=.\NLKespr.h
# End Source File
# Begin Source File

SOURCE=.\NLLinSolve.h
# End Source File
# Begin Source File

SOURCE=.\NLLut.h
# End Source File
# Begin Source File

SOURCE=.\NLMem.h
# End Source File
# Begin Source File

SOURCE=.\NLMRA.h
# End Source File
# Begin Source File

SOURCE=.\NLNoiseEst.h
# End Source File
# Begin Source File

SOURCE=.\NLNoiseRed.h
# End Source File
# Begin Source File

SOURCE=.\NLOrientMgr.h
# End Source File
# Begin Source File

SOURCE=.\NLRan.h
# End Source File
# Begin Source File

SOURCE=.\NLResize.h
# End Source File
# Begin Source File

SOURCE=.\NLScatterRed.h
# End Source File
# Begin Source File

SOURCE=.\NLSolve.h
# End Source File
# Begin Source File

SOURCE=.\NLSort.H
# End Source File
# Begin Source File

SOURCE=.\NLSpecFct.h
# End Source File
# Begin Source File

SOURCE=.\NLStructs.h
# End Source File
# Begin Source File

SOURCE=.\NLTonescale.h
# End Source File
# Begin Source File

SOURCE=.\NLTypes.h
# End Source File
# Begin Source File

SOURCE=.\NLUtil.h
# End Source File
# Begin Source File

SOURCE=.\NumLib.h
# End Source File
# End Group
# End Target
# End Project
