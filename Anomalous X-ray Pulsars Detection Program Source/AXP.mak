# Microsoft Developer Studio Generated NMAKE File, Based on AXP.dsp
!IF "$(CFG)" == ""
CFG=test4 - Win32 Debug
!MESSAGE No configuration specified. Defaulting to test4 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "test4 - Win32 Release" && "$(CFG)" != "test4 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AXP.mak" CFG="test4 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "test4 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "test4 - Win32 Debug" (based on "Win32 (x86) Application")
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

!IF  "$(CFG)" == "test4 - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\AXP.exe"


CLEAN :
	-@erase "$(INTDIR)\AXP.obj"
	-@erase "$(INTDIR)\AXP.pch"
	-@erase "$(INTDIR)\AXP.res"
	-@erase "$(INTDIR)\DSP.OBJ"
	-@erase "$(INTDIR)\GraphBuf.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\SystemSettings.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\AXP.exe"
	-@erase "$(OUTDIR)\AXP.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\AXP.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\AXP.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AXP.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  Wsock32.lib MSVCRT.LIB LIBCMT.LIB /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\AXP.pdb" /machine:I386 /out:"$(OUTDIR)\AXP.exe" 
LINK32_OBJS= \
	"$(INTDIR)\AXP.obj" \
	"$(INTDIR)\DSP.OBJ" \
	"$(INTDIR)\GraphBuf.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\SystemSettings.obj" \
	"$(INTDIR)\AXP.res" \
	".\daq_lib\nidaq32.lib" \
	".\daq_lib\nidex32.lib"

"$(OUTDIR)\AXP.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "test4 - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\AXP.exe"


CLEAN :
	-@erase "$(INTDIR)\AXP.obj"
	-@erase "$(INTDIR)\AXP.pch"
	-@erase "$(INTDIR)\AXP.res"
	-@erase "$(INTDIR)\DSP.OBJ"
	-@erase "$(INTDIR)\GraphBuf.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\SystemSettings.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\AXP.exe"
	-@erase "$(OUTDIR)\AXP.ilk"
	-@erase "$(OUTDIR)\AXP.map"
	-@erase "$(OUTDIR)\AXP.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\AXP.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\AXP.res" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AXP.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Wsock32.lib MSVCRT.LIB LIBCMT.LIB /nologo /subsystem:windows /verbose /incremental:yes /pdb:"$(OUTDIR)\AXP.pdb" /map:"$(INTDIR)\AXP.map" /debug /machine:I386 /nodefaultlib /out:"$(OUTDIR)\AXP.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\AXP.obj" \
	"$(INTDIR)\DSP.OBJ" \
	"$(INTDIR)\GraphBuf.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\SystemSettings.obj" \
	"$(INTDIR)\AXP.res" \
	".\daq_lib\nidaq32.lib" \
	".\daq_lib\nidex32.lib"

"$(OUTDIR)\AXP.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("AXP.dep")
!INCLUDE "AXP.dep"
!ELSE 
!MESSAGE Warning: cannot find "AXP.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "test4 - Win32 Release" || "$(CFG)" == "test4 - Win32 Debug"
SOURCE=.\AXP.cpp

"$(INTDIR)\AXP.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AXP.pch"


SOURCE=.\AXP.rc

"$(INTDIR)\AXP.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\DSP.CPP

"$(INTDIR)\DSP.OBJ" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AXP.pch"


SOURCE=.\GraphBuf.cpp

"$(INTDIR)\GraphBuf.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AXP.pch"


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "test4 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\AXP.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\AXP.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "test4 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\AXP.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\AXP.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\SystemSettings.cpp

"$(INTDIR)\SystemSettings.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AXP.pch"



!ENDIF 

