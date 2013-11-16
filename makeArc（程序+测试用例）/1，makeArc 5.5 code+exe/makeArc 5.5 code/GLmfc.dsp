# Microsoft Developer Studio Project File - Name="GLmfc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=GLmfc - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GLmfc.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GLmfc.mak" CFG="GLmfc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GLmfc - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "GLmfc - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GLmfc - Win32 Release"

# PROP BASE Use_MFC 2
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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "GLmfc - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "GLmfc - Win32 Release"
# Name "GLmfc - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\GLmfc.cpp
# End Source File
# Begin Source File

SOURCE=.\GLmfc.rc
# End Source File
# Begin Source File

SOURCE=.\mainDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\setingDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\class.h
# End Source File
# Begin Source File

SOURCE=.\func.h
# End Source File
# Begin Source File

SOURCE=.\GLmfc.h
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\VC98\Include\GL\glut.h"
# End Source File
# Begin Source File

SOURCE=.\groundBlock.h
# End Source File
# Begin Source File

SOURCE=.\groundgrid.h
# End Source File
# Begin Source File

SOURCE=.\mainDlg.h
# End Source File
# Begin Source File

SOURCE=.\mathEx.h
# End Source File
# Begin Source File

SOURCE=.\my_Delaunay.h
# End Source File
# Begin Source File

SOURCE=.\qTree.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\setingDlg.h
# End Source File
# Begin Source File

SOURCE=.\simpleClass.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\text.h
# End Source File
# Begin Source File

SOURCE=.\texture.h
# End Source File
# Begin Source File

SOURCE=.\WF_cal_avrgH_for_each_cloud.h
# End Source File
# Begin Source File

SOURCE=.\WF_cal_bottomH_for_each_cloud.h
# End Source File
# Begin Source File

SOURCE=.\WF_cal_minH_and_rid_cloud_too_low.h
# End Source File
# Begin Source File

SOURCE=.\WF_cal_minRect_for_each_cloud.h
# End Source File
# Begin Source File

SOURCE=.\WF_cal_texCoord_for_each_roof.h
# End Source File
# Begin Source File

SOURCE=.\WF_cal_texCoord_for_groundgrid.h
# End Source File
# Begin Source File

SOURCE=.\WF_cloud_data_read_in.h
# End Source File
# Begin Source File

SOURCE=.\WF_cloud_segmentation.h
# End Source File
# Begin Source File

SOURCE=.\WF_cloud_settlement.h
# End Source File
# Begin Source File

SOURCE=.\WF_load_model.h
# End Source File
# Begin Source File

SOURCE=.\WF_load_raw_head_file.h
# End Source File
# Begin Source File

SOURCE=.\WF_make_ground_grid_and_remove_ground_point.h
# End Source File
# Begin Source File

SOURCE=.\WF_make_roof_and_wall.h
# End Source File
# Begin Source File

SOURCE=.\WF_mark_step_point.h
# End Source File
# Begin Source File

SOURCE=.\WF_outline_extraction.h
# End Source File
# Begin Source File

SOURCE=.\WF_outline_regulation.h
# End Source File
# Begin Source File

SOURCE=.\WF_outline_smooth.h
# End Source File
# Begin Source File

SOURCE=.\WF_remove_vegetation_point.h
# End Source File
# Begin Source File

SOURCE=.\WF_remove_wall_point.h
# End Source File
# Begin Source File

SOURCE=.\WF_roughly_cal_minH_for_each_cloud.h
# End Source File
# Begin Source File

SOURCE=.\WF_save_model.h
# End Source File
# Begin Source File

SOURCE=.\WF_scene_management.h
# End Source File
# Begin Source File

SOURCE=".\z_备忘.h"
# End Source File
# Begin Source File

SOURCE=".\z_参数.h"
# End Source File
# Begin Source File

SOURCE=".\z_模型文件格式.h"
# End Source File
# Begin Source File

SOURCE=".\z_整理原则.h"
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
