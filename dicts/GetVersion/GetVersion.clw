; CLW-Datei enthält Informationen für den MFC-Klassen-Assistenten

[General Info]
Version=1
LastClass=CGetVersionDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "GetVersion.h"

ClassCount=4
Class1=CGetVersionApp
Class2=CGetVersionDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_GETVERSION_DIALOG

[CLS:CGetVersionApp]
Type=0
HeaderFile=GetVersion.h
ImplementationFile=GetVersion.cpp
Filter=N

[CLS:CGetVersionDlg]
Type=0
HeaderFile=GetVersionDlg.h
ImplementationFile=GetVersionDlg.cpp
Filter=D

[CLS:CAboutDlg]
Type=0
HeaderFile=GetVersionDlg.h
ImplementationFile=GetVersionDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_GETVERSION_DIALOG]
Type=1
Class=CGetVersionDlg
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352

