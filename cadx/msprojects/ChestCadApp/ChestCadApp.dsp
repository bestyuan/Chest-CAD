# Microsoft Developer Studio Project File - Name="ChestCadApp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=ChestCadApp - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ChestCadApp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ChestCadApp.mak" CFG="ChestCadApp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ChestCadApp - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "ChestCadApp - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "ChestCadApp"
# PROP Scc_LocalPath "..\.."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ChestCadApp - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../inc" /I "../../lib/IEM/version-233/inc" /I "../../lib/EKC/inc" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /YX /FD /c
# SUBTRACT CPP /Z<none>
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 Cadx.lib ..\..\lib\ASMLibrary\Release\ASMLibrary.lib ..\..\lib\Via\lib\Release\Via.lib ..\..\lib\Ekc\lib\Ekc.lib Iem.lib IemViaIO.lib DicomMRIIOLibrary.lib coding.vc6{i13r0}.lib element.vc6{i13r0}.lib KdtNTWrappers.lib types.vc6{i13r0}.lib "Dlugosz Repertoire Classics.vc6{pb6+6}.lib" /nologo /subsystem:console /profile /debug /machine:I386 /force /out:"../../bin/ChestCadApp.exe" /libpath:"../../lib/" /libpath:"../../lib/IEM/Version-233/lib" /libpath:"..\..\lib\dicom\DicomMRIIOLibrary\Release" /libpath:"..\..\lib\dicom\kfc_i3r2\kfc\Dependant_Libs\Repertoire\compile.vc6\Release" /libpath:"..\..\lib\dicom\kdt_i13\Kodak DICOM Toolkit\dicom_toolkit\compile.vc6\bin_Release"

!ELSEIF  "$(CFG)" == "ChestCadApp - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../inc" /I "../../lib/IEM/version-233/inc" /I "../../lib/EKC/inc" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 CadxD.lib ..\..\lib\ASMLibrary\Release\ASMLibrary.lib ..\..\lib\Via\lib\Release\Via.lib ..\..\lib\Ekc\lib\Ekc.lib Iem.lib IemViaIO.lib DicomMRIIOLibrary.lib acse.vc6.debug{i13r0}.lib coding.vc6.debug{i13r0}.lib Communication.vc6.debug{i13r0}.lib element.vc6.debug{i13r0}.lib KdtNTWrappers.lib StoreService.vc6.debug{i13r0}.lib types.vc6.debug{i13r0}.lib "Dlugosz Repertoire Classics.vc6.debug{pb6+6}.lib" /nologo /subsystem:console /debug /machine:I386 /force /out:"../../bin/ChestCadAppD.exe" /pdbtype:sept /libpath:"../../lib/" /libpath:"../../lib/IEM/Version-233/lib" /libpath:"..\..\lib\dicom\DicomMRIIOLibrary\Debug" /libpath:"..\..\lib\dicom\kfc_i3r2\kfc\Dependant_Libs\Repertoire\compile.vc6\Debug" /libpath:"..\..\lib\dicom\kdt_i13\Kodak DICOM Toolkit\dicom_toolkit\compile.vc6\bin_Debug"

!ENDIF 

# Begin Target

# Name "ChestCadApp - Win32 Release"
# Name "ChestCadApp - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\apps\ChestCadApp.cpp
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
