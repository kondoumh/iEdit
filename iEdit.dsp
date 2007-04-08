# Microsoft Developer Studio Project File - Name="iEdit" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=iEdit - Win32 Debug
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "iEdit.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "iEdit.mak" CFG="iEdit - Win32 Debug"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "iEdit - Win32 Release" ("Win32 (x86) Application" 用)
!MESSAGE "iEdit - Win32 Debug" ("Win32 (x86) Application" 用)
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "iEdit - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 msimg32.lib imm32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "iEdit - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "iEdit - Win32 Release"
# Name "iEdit - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\EditorView.cpp
# End Source File
# Begin Source File

SOURCE=.\iEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\iEdit.rc
# End Source File
# Begin Source File

SOURCE=.\iEditDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\iLink.cpp
# End Source File
# Begin Source File

SOURCE=.\iNode.cpp
# End Source File
# Begin Source File

SOURCE=.\InpcatDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\InpcnDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LinkInfo2Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LinkInfoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LinkView.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MfSizer.cpp
# End Source File
# Begin Source File

SOURCE=.\NetView.cpp
# End Source File
# Begin Source File

SOURCE=.\nodePropDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\nodeSrchDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OnProcDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OutLineView.cpp
# End Source File
# Begin Source File

SOURCE=.\PageFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\PageLink.cpp
# End Source File
# Begin Source File

SOURCE=.\PageNode.cpp
# End Source File
# Begin Source File

SOURCE=.\PageOther.cpp
# End Source File
# Begin Source File

SOURCE=.\PropertyDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\randAreaDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RelaxThrd.cpp
# End Source File
# Begin Source File

SOURCE=.\SelExportDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SelFileDropDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SelImportDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetAlphaDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\setFoldUpDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetMFSizeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\shapeInsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\shapesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Splash.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SvgWriter.cpp
# End Source File
# Begin Source File

SOURCE=.\TabSeet.cpp
# End Source File
# Begin Source File

SOURCE=.\Token.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\EditorView.h
# End Source File
# Begin Source File

SOURCE=.\iEdit.h
# End Source File
# Begin Source File

SOURCE=.\iEditDoc.h
# End Source File
# Begin Source File

SOURCE=.\iLink.h
# End Source File
# Begin Source File

SOURCE=.\iNode.h
# End Source File
# Begin Source File

SOURCE=.\InpcatDlg.h
# End Source File
# Begin Source File

SOURCE=.\InpcnDlg.h
# End Source File
# Begin Source File

SOURCE=.\LinkInfo2Dlg.h
# End Source File
# Begin Source File

SOURCE=.\LinkInfoDlg.h
# End Source File
# Begin Source File

SOURCE=.\LinkView.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\MfSizer.h
# End Source File
# Begin Source File

SOURCE=.\NetView.h
# End Source File
# Begin Source File

SOURCE=.\nodePropDlg.h
# End Source File
# Begin Source File

SOURCE=.\nodeSrchDlg.h
# End Source File
# Begin Source File

SOURCE=.\OnProcDlg.h
# End Source File
# Begin Source File

SOURCE=.\OutLineView.h
# End Source File
# Begin Source File

SOURCE=.\PageFrame.h
# End Source File
# Begin Source File

SOURCE=.\PageLink.h
# End Source File
# Begin Source File

SOURCE=.\PageNode.h
# End Source File
# Begin Source File

SOURCE=.\PageOther.h
# End Source File
# Begin Source File

SOURCE=.\PropertyDlg.h
# End Source File
# Begin Source File

SOURCE=.\randAreaDlg.h
# End Source File
# Begin Source File

SOURCE=.\RelaxThrd.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SelExportDlg.h
# End Source File
# Begin Source File

SOURCE=.\SelFileDropDlg.h
# End Source File
# Begin Source File

SOURCE=.\SelImportDlg.h
# End Source File
# Begin Source File

SOURCE=.\SetAlphaDlg.h
# End Source File
# Begin Source File

SOURCE=.\setFoldUpDlg.h
# End Source File
# Begin Source File

SOURCE=.\SetMFSizeDlg.h
# End Source File
# Begin Source File

SOURCE=.\shapeInsDlg.h
# End Source File
# Begin Source File

SOURCE=.\shapesDlg.h
# End Source File
# Begin Source File

SOURCE=.\Splash.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\SvgWriter.h
# End Source File
# Begin Source File

SOURCE=.\TabSeet.h
# End Source File
# Begin Source File

SOURCE=.\Token.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\arc.cur
# End Source File
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cur00001.cur
# End Source File
# Begin Source File

SOURCE=.\res\cur00002.cur
# End Source File
# Begin Source File

SOURCE=.\res\cur00003.cur
# End Source File
# Begin Source File

SOURCE=.\res\cur00004.cur
# End Source File
# Begin Source File

SOURCE=.\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\res\cursor2.cur
# End Source File
# Begin Source File

SOURCE=.\res\hand_clo.cur
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\iEdit.ico
# End Source File
# Begin Source File

SOURCE=.\res\iEdit.rc2
# End Source File
# Begin Source File

SOURCE=.\res\iEditDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\line1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\link.cur
# End Source File
# Begin Source File

SOURCE=.\res\linkdl.bmp
# End Source File
# Begin Source File

SOURCE=.\res\linkfile.bmp
# End Source File
# Begin Source File

SOURCE=.\res\links.bmp
# End Source File
# Begin Source File

SOURCE=.\res\linksl.bmp
# End Source File
# Begin Source File

SOURCE=.\res\linktree.cur
# End Source File
# Begin Source File

SOURCE=.\res\linkurl.bmp
# End Source File
# Begin Source File

SOURCE=.\res\logo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\palette.bmp
# End Source File
# Begin Source File

SOURCE=.\res\pointer_.cur
# End Source File
# Begin Source File

SOURCE=.\res\ptr_move.cur
# End Source File
# Begin Source File

SOURCE=.\res\rect.cur
# End Source File
# Begin Source File

SOURCE=.\Splsh16.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tree.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\iEdit.reg
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
# Section iEdit : {D076B643-16F0-11D3-8084-00A0C9B72FDD}
# 	2:9:TabSeet.h:TabSeet.h
# 	2:11:TabSeet.cpp:TabSeet.cpp
# 	2:15:CLASS: CTabSeet:CTabSeet
# 	2:19:Application Include:iEdit.h
# End Section
# Section iEdit : {FE9DB921-28A6-11D3-A0F1-000000000000}
# 	1:9:IDD_INPCN:102
# 	2:16:Resource Include:resource.h
# 	2:16:CLASS: CInpcnDlg:CInpcnDlg
# 	2:10:ENUM: enum:enum
# 	2:12:InpcnDlg.cpp:InpcnDlg.cpp
# 	2:9:IDD_INPCN:IDD_INPCN
# 	2:10:InpcnDlg.h:InpcnDlg.h
# 	2:19:Application Include:iEdit.h
# End Section
# Section iEdit : {C56C8800-7A46-11D3-B4D3-00A0C9B72FDD}
# 	1:10:IDD_ONPROC:104
# 	2:16:Resource Include:resource.h
# 	2:17:CLASS: COnProcDlg:COnProcDlg
# 	2:10:IDD_ONPROC:IDD_ONPROC
# 	2:13:OnProcDlg.cpp:OnProcDlg.cpp
# 	2:10:ENUM: enum:enum
# 	2:19:Application Include:iEdit.h
# 	2:11:OnProcDlg.h:OnProcDlg.h
# End Section
# Section iEdit : {72ADFD78-2C39-11D0-9903-00A0C91BC942}
# 	1:10:IDB_SPLASH:103
# 	2:21:SplashScreenInsertKey:4.0
# End Section
