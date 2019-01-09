# Microsoft Developer Studio Generated NMAKE File, Based on LogBook.dsp
!IF "$(CFG)" == ""
CFG=LogBook - Win32 Debug
!MESSAGE No configuration specified. Defaulting to LogBook - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "LogBook - Win32 Release" && "$(CFG)" != "LogBook - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LogBook.mak" CFG="LogBook - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LogBook - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "LogBook - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LogBook - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\LogBook.exe" "$(OUTDIR)\LogBook.tlb"


CLEAN :
	-@erase "$(INTDIR)\DlgProxy.obj"
	-@erase "$(INTDIR)\LogBook.obj"
	-@erase "$(INTDIR)\LogBook.pch"
	-@erase "$(INTDIR)\LogBook.res"
	-@erase "$(INTDIR)\LogBook.tlb"
	-@erase "$(INTDIR)\LogBookDlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\LogBook.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\LogBook.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\LogBook.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\LogBook.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\LogBook.pdb" /machine:I386 /out:"$(OUTDIR)\LogBook.exe" 
LINK32_OBJS= \
	"$(INTDIR)\LogBook.obj" \
	"$(INTDIR)\LogBookDlg.obj" \
	"$(INTDIR)\DlgProxy.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\LogBook.res"

"$(OUTDIR)\LogBook.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "LogBook - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\LogBook.exe" "$(OUTDIR)\LogBook.tlb"


CLEAN :
	-@erase "$(INTDIR)\DlgProxy.obj"
	-@erase "$(INTDIR)\LogBook.obj"
	-@erase "$(INTDIR)\LogBook.pch"
	-@erase "$(INTDIR)\LogBook.res"
	-@erase "$(INTDIR)\LogBook.tlb"
	-@erase "$(INTDIR)\LogBookDlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\LogBook.exe"
	-@erase "$(OUTDIR)\LogBook.ilk"
	-@erase "$(OUTDIR)\LogBook.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\LogBook.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ   /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\LogBook.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\LogBook.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\LogBook.pdb" /debug /machine:I386 /out:"$(OUTDIR)\LogBook.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\LogBook.obj" \
	"$(INTDIR)\LogBookDlg.obj" \
	"$(INTDIR)\DlgProxy.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\LogBook.res"

"$(OUTDIR)\LogBook.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("LogBook.dep")
!INCLUDE "LogBook.dep"
!ELSE 
!MESSAGE Warning: cannot find "LogBook.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "LogBook - Win32 Release" || "$(CFG)" == "LogBook - Win32 Debug"
SOURCE=.\DlgProxy.cpp

"$(INTDIR)\DlgProxy.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\LogBook.pch"


SOURCE=.\LogBook.cpp

"$(INTDIR)\LogBook.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\LogBook.pch"


SOURCE=.\LogBook.odl

!IF  "$(CFG)" == "LogBook - Win32 Release"

MTL_SWITCHES=/nologo /D "NDEBUG" /tlb "$(OUTDIR)\LogBook.tlb" /mktyplib203 /win32 

"$(OUTDIR)\LogBook.tlb" : $(SOURCE) "$(OUTDIR)"
	$(MTL) @<<
  $(MTL_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "LogBook - Win32 Debug"

MTL_SWITCHES=/nologo /D "_DEBUG" /tlb "$(OUTDIR)\LogBook.tlb" /mktyplib203 /win32 

"$(OUTDIR)\LogBook.tlb" : $(SOURCE) "$(OUTDIR)"
	$(MTL) @<<
  $(MTL_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\LogBook.rc

!IF  "$(CFG)" == "LogBook - Win32 Release"


"$(INTDIR)\LogBook.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\LogBook.res" /i "Release" /d "NDEBUG" /d "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "LogBook - Win32 Debug"


"$(INTDIR)\LogBook.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\LogBook.res" /i "Debug" /d "_DEBUG" /d "_AFXDLL" $(SOURCE)


!ENDIF 

SOURCE=.\LogBookDlg.cpp

"$(INTDIR)\LogBookDlg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\LogBook.pch"


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "LogBook - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\LogBook.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\LogBook.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "LogBook - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\LogBook.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ   /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\LogBook.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

