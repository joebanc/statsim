# Microsoft Developer Studio Generated NMAKE File, Based on GetVersion.dsp
!IF "$(CFG)" == ""
CFG=GetVersion - Win32 Debug
!MESSAGE Keine Konfiguration angegeben. GetVersion - Win32 Debug wird als Standard verwendet.
!ENDIF 

!IF "$(CFG)" != "GetVersion - Win32 Release" && "$(CFG)" != "GetVersion - Win32 Debug"
!MESSAGE UngÅltige Konfiguration "$(CFG)" angegeben.
!MESSAGE Sie kînnen beim AusfÅhren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "GetVersion.mak" CFG="GetVersion - Win32 Debug"
!MESSAGE 
!MESSAGE FÅr die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "GetVersion - Win32 Release" (basierend auf  "Win32 (x86) Application")
!MESSAGE "GetVersion - Win32 Debug" (basierend auf  "Win32 (x86) Application")
!MESSAGE 
!ERROR Eine ungÅltige Konfiguration wurde angegeben.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GetVersion - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\GetVersion.exe"


CLEAN :
	-@erase "$(INTDIR)\GetVersion.obj"
	-@erase "$(INTDIR)\GetVersion.pch"
	-@erase "$(INTDIR)\GetVersion.res"
	-@erase "$(INTDIR)\GlobalFunctions.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\GetVersion.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\GetVersion.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\GetVersion.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\GetVersion.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=version.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\GetVersion.pdb" /machine:I386 /out:"$(OUTDIR)\GetVersion.exe" 
LINK32_OBJS= \
	"$(INTDIR)\GetVersion.obj" \
	"$(INTDIR)\GlobalFunctions.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\GetVersion.res"

"$(OUTDIR)\GetVersion.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "GetVersion - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\GetVersion.exe"


CLEAN :
	-@erase "$(INTDIR)\GetVersion.obj"
	-@erase "$(INTDIR)\GetVersion.pch"
	-@erase "$(INTDIR)\GetVersion.res"
	-@erase "$(INTDIR)\GlobalFunctions.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\GetVersion.exe"
	-@erase "$(OUTDIR)\GetVersion.ilk"
	-@erase "$(OUTDIR)\GetVersion.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\GetVersion.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\GetVersion.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\GetVersion.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=version.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\GetVersion.pdb" /debug /machine:I386 /out:"$(OUTDIR)\GetVersion.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\GetVersion.obj" \
	"$(INTDIR)\GlobalFunctions.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\GetVersion.res"

"$(OUTDIR)\GetVersion.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("GetVersion.dep")
!INCLUDE "GetVersion.dep"
!ELSE 
!MESSAGE Warning: cannot find "GetVersion.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "GetVersion - Win32 Release" || "$(CFG)" == "GetVersion - Win32 Debug"
SOURCE=.\GetVersion.cpp

"$(INTDIR)\GetVersion.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\GetVersion.pch"


SOURCE=.\GetVersion.rc

"$(INTDIR)\GetVersion.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\GlobalFunctions.cpp

"$(INTDIR)\GlobalFunctions.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\GetVersion.pch"


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "GetVersion - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\GetVersion.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\GetVersion.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "GetVersion - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\GetVersion.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\GetVersion.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

