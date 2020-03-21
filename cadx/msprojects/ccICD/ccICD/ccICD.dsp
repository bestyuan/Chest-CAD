# Microsoft Developer Studio Project File - Name="ccICD" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=ccICD - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ccICD.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ccICD.mak" CFG="ccICD - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ccICD - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "ccICD - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "ccICD"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ccICD - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "\\rl-fs02\ISD-ISRLDISK\inhouse\IEM\current\inc\iem" /I "\\rl-fs02\ISD-ISRLDISK\inhouse\ima\current\fw\ekc\inc" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# SUBTRACT CPP /X
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib iem.lib ekc.lib iemviaio.lib via.lib /nologo /subsystem:console /machine:I386 /libpath:"\\rl-fs02\ISD-ISRLDISK\inhouse\IEM\current\Release" /libpath:"\\rl-fs02\ISD-ISRLDISK\inhouse\Ima\current\fw\ekc\lib" /libpath:"\\rl-fs02\ISD-ISRLDISK\inhouse\via\v13beta\lib\release"

!ELSEIF  "$(CFG)" == "ccICD - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "\\rl-fs02\ISD-ISRLDISK\inhouse\IEM\current\inc\iem" /I "\\rl-fs02\ISD-ISRLDISK\inhouse\ima\current\fw\ekc\inc" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Iemd.lib viad.lib ekcd.lib iemviaiod.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"msvcrt" /pdbtype:sept /libpath:"\\rl-fs02\ISD-ISRLDISK\inhouse\IEM\current\Debug" /libpath:"\\rl-fs02\ISD-ISRLDISK\inhouse\Ima\current\fw\ekc\lib" /libpath:"\\rl-fs02\ISD-ISRLDISK\inhouse\via\v13beta\lib\debug"

!ENDIF 

# Begin Target

# Name "ccICD - Win32 Release"
# Name "ccICD - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\src\candidates\ccICD\Afum.cpp

!IF  "$(CFG)" == "ccICD - Win32 Release"

!ELSEIF  "$(CFG)" == "ccICD - Win32 Debug"

# ADD CPP /I "\\rl-fs02\ISD-ISRLDISK\inhouse\ima\current\fw\ekc\inc"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\candidates\ccICD\CandidateList.cpp

!IF  "$(CFG)" == "ccICD - Win32 Release"

!ELSEIF  "$(CFG)" == "ccICD - Win32 Debug"

# ADD CPP /I "\\rl-fs02\ISD-ISRLDISK\inhouse\ima\current\fw\ekc\inc"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\candidates\ccICD\CanFile.cpp

!IF  "$(CFG)" == "ccICD - Win32 Release"

!ELSEIF  "$(CFG)" == "ccICD - Win32 Debug"

# ADD CPP /I "\\rl-fs02\ISD-ISRLDISK\inhouse\ima\current\fw\ekc\inc"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\candidates\ccICD\ccICD.cpp

!IF  "$(CFG)" == "ccICD - Win32 Release"

!ELSEIF  "$(CFG)" == "ccICD - Win32 Debug"

# ADD CPP /I "\\rl-fs02\ISD-ISRLDISK\inhouse\ima\current\fw\ekc\inc"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\candidates\ccICD\Classifier.cpp

!IF  "$(CFG)" == "ccICD - Win32 Release"

!ELSEIF  "$(CFG)" == "ccICD - Win32 Debug"

# ADD CPP /I "\\rl-fs02\ISD-ISRLDISK\inhouse\ima\current\fw\ekc\inc"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\candidates\ccICD\DMatrix.cpp

!IF  "$(CFG)" == "ccICD - Win32 Release"

!ELSEIF  "$(CFG)" == "ccICD - Win32 Debug"

# ADD CPP /I "\\rl-fs02\ISD-ISRLDISK\inhouse\ima\current\fw\ekc\inc"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\candidates\ccICD\GmlClassifier.cpp

!IF  "$(CFG)" == "ccICD - Win32 Release"

!ELSEIF  "$(CFG)" == "ccICD - Win32 Debug"

# ADD CPP /I "\\rl-fs02\ISD-ISRLDISK\inhouse\ima\current\fw\ekc\inc"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\candidates\ccICD\IcdFeatureManager.cpp

!IF  "$(CFG)" == "ccICD - Win32 Release"

!ELSEIF  "$(CFG)" == "ccICD - Win32 Debug"

# ADD CPP /I "\\rl-fs02\ISD-ISRLDISK\inhouse\ima\current\fw\ekc\inc"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\candidates\ccICD\IcdImgProc.cpp

!IF  "$(CFG)" == "ccICD - Win32 Release"

!ELSEIF  "$(CFG)" == "ccICD - Win32 Debug"

# ADD CPP /I "\\rl-fs02\ISD-ISRLDISK\inhouse\ima\current\fw\ekc\inc"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\candidates\ccICD\IcdUtils.cpp

!IF  "$(CFG)" == "ccICD - Win32 Release"

!ELSEIF  "$(CFG)" == "ccICD - Win32 Debug"

# ADD CPP /I "\\rl-fs02\ISD-ISRLDISK\inhouse\ima\current\fw\ekc\inc"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\candidates\ccICD\IcdXrayImage.cpp

!IF  "$(CFG)" == "ccICD - Win32 Release"

!ELSEIF  "$(CFG)" == "ccICD - Win32 Debug"

# ADD CPP /I "\\rl-fs02\ISD-ISRLDISK\inhouse\ima\current\fw\ekc\inc"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\candidates\ccICD\ImageTruth.cpp

!IF  "$(CFG)" == "ccICD - Win32 Release"

!ELSEIF  "$(CFG)" == "ccICD - Win32 Debug"

# ADD CPP /I "\\rl-fs02\ISD-ISRLDISK\inhouse\ima\current\fw\ekc\inc"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\candidates\ccICD\KesprImage.cpp

!IF  "$(CFG)" == "ccICD - Win32 Release"

!ELSEIF  "$(CFG)" == "ccICD - Win32 Debug"

# ADD CPP /I "\\rl-fs02\ISD-ISRLDISK\inhouse\ima\current\fw\ekc\inc"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\candidates\ccICD\LungMask.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\candidates\ccICD\Mhac.cpp

!IF  "$(CFG)" == "ccICD - Win32 Release"

!ELSEIF  "$(CFG)" == "ccICD - Win32 Debug"

# ADD CPP /I "\\rl-fs02\ISD-ISRLDISK\inhouse\ima\current\fw\ekc\inc"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\candidates\ccICD\MultiscaleMhac.cpp

!IF  "$(CFG)" == "ccICD - Win32 Release"

!ELSEIF  "$(CFG)" == "ccICD - Win32 Debug"

# ADD CPP /I "\\rl-fs02\ISD-ISRLDISK\inhouse\ima\current\fw\ekc\inc"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\candidates\ccICD\Rect.cpp

!IF  "$(CFG)" == "ccICD - Win32 Release"

!ELSEIF  "$(CFG)" == "ccICD - Win32 Debug"

# ADD CPP /I "\\rl-fs02\ISD-ISRLDISK\inhouse\ima\current\fw\ekc\inc"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\candidates\ccICD\TextGen.cpp

!IF  "$(CFG)" == "ccICD - Win32 Release"

!ELSEIF  "$(CFG)" == "ccICD - Win32 Debug"

# ADD CPP /I "\\rl-fs02\ISD-ISRLDISK\inhouse\ima\current\fw\ekc\inc"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\candidates\ccICD\TrueNodule.cpp

!IF  "$(CFG)" == "ccICD - Win32 Release"

!ELSEIF  "$(CFG)" == "ccICD - Win32 Debug"

# ADD CPP /I "\\rl-fs02\ISD-ISRLDISK\inhouse\ima\current\fw\ekc\inc"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
