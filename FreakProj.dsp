# Microsoft Developer Studio Project File - Name="FreakProj" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=FreakProj - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FreakProj.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FreakProj.mak" CFG="FreakProj - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FreakProj - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "FreakProj - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "FreakProj - Win32 Thomas" (based on "Win32 (x86) Application")
!MESSAGE "FreakProj - Win32 MarieLaure" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "FreakProj"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FreakProj - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1009 /d "NDEBUG"
# ADD RSC /l 0x1009 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 ddraw.lib gdi32.lib user32.lib /nologo /subsystem:windows /debug /machine:I386

!ELSEIF  "$(CFG)" == "FreakProj - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /Gm /GX /ZI /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /YX /FD /GZ /c
# SUBTRACT CPP /O<none>
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1009 /d "_DEBUG"
# ADD RSC /l 0x1009 /fo"script2.res" /i "c:\PROG\CppProj\VS6\FreakProj\bitmaps" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ddraw.lib gdi32.lib user32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /verbose /incremental:no /nodefaultlib

!ELSEIF  "$(CFG)" == "FreakProj - Win32 Thomas"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "FreakProj___Win32_Thomas"
# PROP BASE Intermediate_Dir "FreakProj___Win32_Thomas"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Win32_Thomas"
# PROP Intermediate_Dir "Win32_Thomas"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /Ob0 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "THOMAS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1009 /d "NDEBUG"
# ADD RSC /l 0x1009 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib c:\mssdk\lib\ddraw.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 ddraw.lib gdi32.lib user32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Win32_Thomas\ThomasGoonOut.exe"

!ELSEIF  "$(CFG)" == "FreakProj - Win32 MarieLaure"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "FreakProj___Win32_MarieLaure"
# PROP BASE Intermediate_Dir "FreakProj___Win32_MarieLaure"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Win32_MarieLaure"
# PROP Intermediate_Dir "Win32_MarieLaure"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "MARIE_LAURE" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1009 /d "NDEBUG"
# ADD RSC /l 0x1009 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib c:\mssdk\lib\ddraw.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 ddraw.lib gdi32.lib user32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Win32_MarieLaure\MarieLaureBoopOut.exe"

!ENDIF 

# Begin Target

# Name "FreakProj - Win32 Release"
# Name "FreakProj - Win32 Debug"
# Name "FreakProj - Win32 Thomas"
# Name "FreakProj - Win32 MarieLaure"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Src\blackbox.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\freakMain.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\freakout.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\Script2.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Src\blackbox.h
# End Source File
# Begin Source File

SOURCE=.\Src\freakout.h
# End Source File
# Begin Source File

SOURCE=.\Src\FreakProj.h
# End Source File
# Begin Source File

SOURCE=.\Src\resource.h
# End Source File
# End Group
# Begin Group "Info"

# PROP Default_Filter ".info .txt"
# Begin Source File

SOURCE=.\Info\bitMapInfo.txt

!IF  "$(CFG)" == "FreakProj - Win32 Release"

!ELSEIF  "$(CFG)" == "FreakProj - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FreakProj - Win32 Thomas"

!ELSEIF  "$(CFG)" == "FreakProj - Win32 MarieLaure"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Info\problems.info

!IF  "$(CFG)" == "FreakProj - Win32 Release"

!ELSEIF  "$(CFG)" == "FreakProj - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FreakProj - Win32 Thomas"

!ELSEIF  "$(CFG)" == "FreakProj - Win32 MarieLaure"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Info\todo.info

!IF  "$(CFG)" == "FreakProj - Win32 Release"

!ELSEIF  "$(CFG)" == "FreakProj - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FreakProj - Win32 Thomas"

!ELSEIF  "$(CFG)" == "FreakProj - Win32 MarieLaure"

!ENDIF 

# End Source File
# End Group
# Begin Group "bitmaps"

# PROP Default_Filter ".bmp"
# Begin Source File

SOURCE=.\bitmaps\happy_bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\Src\happy_bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmaps\happy_bitmap2.bmp
# End Source File
# Begin Source File

SOURCE=.\Src\happy_bitmap2.bmp
# End Source File
# Begin Source File

SOURCE=".\Src\happy_bitmap256-1.bmp"
# End Source File
# Begin Source File

SOURCE=".\bitmaps\happy_bitmap256-2.bmp"
# End Source File
# Begin Source File

SOURCE=".\bitmaps\happy_bitmap256-3.bmp"
# End Source File
# Begin Source File

SOURCE=".\bitmaps\happy_bitmap256-4.bmp"
# End Source File
# Begin Source File

SOURCE=".\bitmaps\happy_bitmap256-5.bmp"
# End Source File
# Begin Source File

SOURCE=".\bitmaps\happy_bitmap256-6.bmp"
# End Source File
# Begin Source File

SOURCE=".\bitmaps\happy_bitmap256-7.bmp"
# End Source File
# Begin Source File

SOURCE=".\bitmaps\happy_bitmap256-8.bmp"
# End Source File
# Begin Source File

SOURCE=.\bitmaps\happy_bitmap3.bmp
# End Source File
# Begin Source File

SOURCE=.\Src\happy_bitmap3.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmaps\happy_bitmap4.bmp
# End Source File
# Begin Source File

SOURCE=.\Src\happy_bitmap4.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmaps\happy_bitmap5.bmp
# End Source File
# Begin Source File

SOURCE=.\Src\happy_bitmap5.bmp
# End Source File
# End Group
# End Target
# End Project
