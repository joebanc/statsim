
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by NRDB.HPJ. >"hlp\nrdb.hm"
echo. >>"hlp\nrdb.hm"
echo // Commands (ID_* and IDM_*) >>"hlp\nrdb.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\nrdb.hm"
echo. >>"hlp\nrdb.hm"
echo // Prompts (IDP_*) >>"hlp\nrdb.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\nrdb.hm"
echo. >>"hlp\nrdb.hm"
echo // Resources (IDR_*) >>"hlp\nrdb.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\nrdb.hm"
echo. >>"hlp\nrdb.hm"
echo // Dialogs (IDD_*) >>"hlp\nrdb.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\nrdb.hm"
echo. >>"hlp\nrdb.hm"
echo // Frame Controls (IDW_*) >>"hlp\nrdb.hm"
makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\nrdb.hm"
REM -- Make help for Project nrdb


echo Building Win32 Help files
start /wait hcrtf -x "hlp\nrdbpro.hpj"


