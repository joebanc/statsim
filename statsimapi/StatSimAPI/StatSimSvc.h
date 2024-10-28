//  Service.cpp : Defines the entry point for the console application.
//
#pragma once

#include "stdafx.h"
#include <windows.h>
#include <tlhelp32.h>

#define		MAX_NUM_OF_PROCESS		4
/** Window Service **/
VOID DLLEXPORT ServiceMainProc();
VOID DLLEXPORT Install(TCHAR* pPath, TCHAR* pName, TCHAR *pInstLog = NULL);
VOID DLLEXPORT UnInstall(TCHAR* pName);
VOID DLLEXPORT WriteLog(TCHAR* pFile, TCHAR* pMsg);
BOOL DLLEXPORT KillService(TCHAR* pName);
BOOL DLLEXPORT RunService(TCHAR* pName);
VOID DLLEXPORT ExecuteSubProcess();
VOID DLLEXPORT ProcMonitorThread(VOID *);
BOOL DLLEXPORT StartProcess(int ProcessIndex);
VOID DLLEXPORT EndProcess(int ProcessIndex);
VOID DLLEXPORT AttachProcessNames();

VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv);
VOID WINAPI ServiceHandler(DWORD fdwControl);


/** Window Service **/
const int nBufferSize = 500;
TCHAR pServiceName[nBufferSize+1];
TCHAR pExeFile[nBufferSize+1];
TCHAR lpCmdLineData[nBufferSize+1];
TCHAR pLogFile[nBufferSize+1];
BOOL ProcessStarted = TRUE;

CRITICAL_SECTION		myCS;
SERVICE_TABLE_ENTRY		lpServiceStartTable[] = 
{
	{pServiceName, ServiceMain},
	{NULL, NULL}
};

/*LPTSTR ProcessNames[] = { "C:\\n.exe",
						 "C:\\a.exe",
						 "C:\\b.exe",
						 "C:\\c.exe" };*/
LPTSTR ProcessNames[MAX_NUM_OF_PROCESS];

SERVICE_STATUS_HANDLE   hServiceStatusHandle; 
SERVICE_STATUS          ServiceStatus; 
PROCESS_INFORMATION	pProcInfo[MAX_NUM_OF_PROCESS];

int _tmain(int argc, _TCHAR* argv[])
{
	if(argc >= 2)
		_tcscpy(lpCmdLineData, argv[1]);
	ServiceMainProc();
	return 0;
}

VOID ServiceMainProc()
{
	::InitializeCriticalSection(&myCS);
	// initialize variables for .exe and .log file names
	TCHAR pModuleFile[nBufferSize+1];
	DWORD dwSize = GetModuleFileName(NULL, pModuleFile, nBufferSize);
	pModuleFile[dwSize] = 0;
	if(dwSize>4 && pModuleFile[dwSize-4] == '.')
	{
		_stprintf(pExeFile,_T("%s"),pModuleFile);
		pModuleFile[dwSize-4] = 0;
		_stprintf(pLogFile,_T("%s.log"),pModuleFile);
	}
	_tcscpy(pServiceName,_T("Sundar_Service"));

	if(_tcsicmp(_T("-i"),lpCmdLineData) == 0 || _tcsicmp(_T("-I"),lpCmdLineData) == 0)
		Install(pExeFile, pServiceName);
	else if(_tcsicmp(_T("-k"),lpCmdLineData) == 0 || _tcsicmp(_T("-K"),lpCmdLineData) == 0)
		KillService(pServiceName);
	else if(_tcsicmp(_T("-u"),lpCmdLineData) == 0 || _tcsicmp(_T("-U"),lpCmdLineData) == 0)
		UnInstall(pServiceName);
	else if(_tcsicmp(_T("-s"),lpCmdLineData) == 0 || _tcsicmp(_T("-S"),lpCmdLineData) == 0)
		RunService(pServiceName);
	else
		ExecuteSubProcess();
}

VOID Install(TCHAR* pPath, TCHAR* pName, TCHAR* pInstLog)
{  
	SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_CREATE_SERVICE); 
	if (schSCManager==0) 
	{
		long nError = GetLastError();
		TCHAR pTemp[121];
		_stprintf(pTemp, _T("OpenSCManager failed, error code = %d\n"), nError);
		//if(pLogFile)
		//	WriteLog(pLogFile, pTemp);
		//AfxMessageBox(pTemp);
	}
	else
	{
		SC_HANDLE schService = CreateService
		( 
			schSCManager,	/* SCManager database      */ 
			pName,			/* name of service         */ 
			pName,			/* service name to display */ 
			SERVICE_ALL_ACCESS,        /* desired access          */ 
			SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS , /* service type            */ 
			SERVICE_AUTO_START,      /* start type              */ 
			SERVICE_ERROR_NORMAL,      /* error control type      */ 
			pPath,			/* service's binary        */ 
			NULL,                      /* no load ordering group  */ 
			NULL,                      /* no tag identifier       */ 
			NULL,                      /* no dependencies         */ 
			NULL,                      /* LocalSystem account     */ 
			NULL
		);                     /* no password             */ 
		if (schService==0) 
		{
			long nError =  GetLastError();
			TCHAR pTemp[121];
			_stprintf(pTemp, _T("Failed to create service %s, error code = %d\n"), pName, nError);
			//AfxMessageBox(pTemp);
			//WriteLog(pLogFile, pTemp);
	
		}
		else
		{
			TCHAR pTemp[121];
			_stprintf(pTemp, _T("Service %s installed\n"), pName);
			//WriteLog(pLogFile, pTemp);
			//AfxMessageBox(pTemp);
			CloseServiceHandle(schService); 
		}
		CloseServiceHandle(schSCManager);
	}	
}

VOID UnInstall(TCHAR* pName)
{
	SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS); 
	if (schSCManager==0) 
	{
		long nError = GetLastError();
		char pTemp[121];
		sprintf(pTemp, "OpenSCManager failed, error code = %d\n", nError);
		//WriteLog(pLogFile, pTemp);
	}
	else
	{
		SC_HANDLE schService = OpenService( schSCManager, pName, SERVICE_ALL_ACCESS);
		if (schService==0) 
		{
			long nError = GetLastError();
			char pTemp[121];
			sprintf(pTemp, "OpenService failed, error code = %d\n", nError);
			//WriteLog(pLogFile, pTemp);
		}
		else
		{
			if(!DeleteService(schService)) 
			{
				char pTemp[121];
				sprintf(pTemp, "Failed to delete service %s\n", pName);
				//WriteLog(pLogFile, pTemp);
			}
			else 
			{
				char pTemp[121];
				sprintf(pTemp, "Service %s removed\n",pName);
				//WriteLog(pLogFile, pTemp);
			}
			CloseServiceHandle(schService); 
		}
		CloseServiceHandle(schSCManager);	
	}
	DeleteFile(pLogFile);
}

VOID WriteLog(TCHAR* pFile, TCHAR* pMsg)
{
	// write error or other information into log file
	::EnterCriticalSection(&myCS);
	try
	{
		SYSTEMTIME oT;
		::GetLocalTime(&oT);
		FILE* pLog = _tfopen(pFile, _T("a"));
		#if defined(UNICODE) || defined(_UNICODE)
		fwprintf(pLog,L"%02d/%02d/%04d, %02d:%02d:%02d\n    %s",oT.wMonth,oT.wDay,oT.wYear,oT.wHour,oT.wMinute,oT.wSecond,pMsg); 
		#else
		fwprintf(pLog,"%02d/%02d/%04d, %02d:%02d:%02d\n    %s",oT.wMonth,oT.wDay,oT.wYear,oT.wHour,oT.wMinute,oT.wSecond,pMsg); 
		#endif
		fclose(pLog);
	} catch(...) {}
	::LeaveCriticalSection(&myCS);
}

BOOL KillService(TCHAR* pName) 
{ 
	// kill service with given name
	SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS); 
	if (schSCManager==0) 
	{
		long nError = GetLastError();
		char pTemp[121];
		sprintf(pTemp, "OpenSCManager failed, error code = %d\n", nError);
		//WriteLog(pLogFile, pTemp);
	}
	else
	{
		// open the service
		SC_HANDLE schService = OpenService( schSCManager, pName, SERVICE_ALL_ACCESS);
		if (schService==0) 
		{
			long nError = GetLastError();
			char pTemp[121];
			sprintf(pTemp, "OpenService failed, error code = %d\n", nError);
			//WriteLog(pLogFile, pTemp);
		}
		else
		{
			// call ControlService to kill the given service
			SERVICE_STATUS status;
			if(ControlService(schService,SERVICE_CONTROL_STOP,&status))
			{
				CloseServiceHandle(schService); 
				CloseServiceHandle(schSCManager); 
				return TRUE;
			}
			else
			{
				long nError = GetLastError();
				char pTemp[121];
				sprintf(pTemp, "ControlService failed, error code = %d\n", nError);
				//WriteLog(pLogFile, pTemp);
			}
			CloseServiceHandle(schService); 
		}
		CloseServiceHandle(schSCManager); 
	}
	return FALSE;
}

BOOL RunService(TCHAR* pName) 
{ 
	// run service with given name
	SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS); 
	if (schSCManager==0) 
	{
		long nError = GetLastError();
		TCHAR pTemp[121];
		_stprintf(pTemp, L"OpenSCManager failed, error code = %d\n", nError);
		//WriteLog(pLogFile, pTemp);
		//AfxMessageBox(pTemp);
	}
	else
	{
		// open the service
		SC_HANDLE schService = OpenService( schSCManager, pName, SERVICE_ALL_ACCESS);
		if (schService==0) 
		{
			long nError = GetLastError();
			TCHAR pTemp[121];
			_stprintf(pTemp, L"OpenService failed, error code = %d\n", nError);
			//WriteLog(pLogFile, pTemp);
			//AfxMessageBox(pTemp);
		}
		else
		{
			// call StartService to run the service
			if(StartService(schService, 0, (const TCHAR**)NULL))
			{
				CloseServiceHandle(schService); 
				CloseServiceHandle(schSCManager); 
				return TRUE;
			}
			else
			{
				long nError = GetLastError();
				TCHAR pTemp[121];
				_stprintf(pTemp, L"StartService failed, error code = %d\n", nError);
				//WriteLog(pLogFile, pTemp);
				//AfxMessageBox(pTemp);
			}
			CloseServiceHandle(schService); 
		}
		CloseServiceHandle(schSCManager); 
	}
	return FALSE;
}


VOID ExecuteSubProcess()
{
	if(_beginthread(ProcMonitorThread, 0, NULL) == -1)
	{
		long nError = GetLastError();
		char pTemp[121];
		sprintf(pTemp, "StartService failed, error code = %d\n", nError);
		//WriteLog(pLogFile, pTemp);
	}
	if(!StartServiceCtrlDispatcher(lpServiceStartTable))
	{
		long nError = GetLastError();
		char pTemp[121];
		sprintf(pTemp, "StartServiceCtrlDispatcher failed, error code = %d\n", nError);
		//WriteLog(pLogFile, pTemp);
	}
	::DeleteCriticalSection(&myCS);
}

VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
	DWORD   status = 0; 
    DWORD   specificError = 0xfffffff; 
 
    ServiceStatus.dwServiceType        = SERVICE_WIN32; 
    ServiceStatus.dwCurrentState       = SERVICE_START_PENDING; 
    ServiceStatus.dwControlsAccepted   = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE; 
    ServiceStatus.dwWin32ExitCode      = 0; 
    ServiceStatus.dwServiceSpecificExitCode = 0; 
    ServiceStatus.dwCheckPoint         = 0; 
    ServiceStatus.dwWaitHint           = 0; 
 
    hServiceStatusHandle = RegisterServiceCtrlHandler(pServiceName, ServiceHandler); 
    if (hServiceStatusHandle==0) 
    {
		long nError = GetLastError();
		char pTemp[121];
		sprintf(pTemp, "RegisterServiceCtrlHandler failed, error code = %d\n", nError);
		//WriteLog(pLogFile, pTemp);
        return; 
    } 
 
    // Initialization complete - report running status 
    ServiceStatus.dwCurrentState       = SERVICE_RUNNING; 
    ServiceStatus.dwCheckPoint         = 0; 
    ServiceStatus.dwWaitHint           = 0;  
    if(!SetServiceStatus(hServiceStatusHandle, &ServiceStatus)) 
    { 
		long nError = GetLastError();
		char pTemp[121];
		sprintf(pTemp, "SetServiceStatus failed, error code = %d\n", nError);
		//WriteLog(pLogFile, pTemp);
    } 

	AttachProcessNames();
	for(int iLoop = 0; iLoop < MAX_NUM_OF_PROCESS; iLoop++)
	{
		pProcInfo[iLoop].hProcess = 0;
		StartProcess(iLoop);
	}
}

VOID AttachProcessNames()
{
	LPTSTR lpszpath;
	lpszpath = new TCHAR[nBufferSize];
	memset(lpszpath,0x00,sizeof(lpszpath));
	DWORD dwSize = GetModuleFileName(NULL, lpszpath, nBufferSize);
	lpszpath[dwSize] = 0;
	while(lpszpath[dwSize] != '\\' && dwSize != 0)
	{
		lpszpath[dwSize] = 0; dwSize--;
	}
	for(int iLoop = 0; iLoop < MAX_NUM_OF_PROCESS; iLoop++)
	{
		ProcessNames[iLoop] = (TCHAR*) malloc(nBufferSize);
		memset(ProcessNames[iLoop], 0x00, nBufferSize);
		_tcscpy(ProcessNames[iLoop],lpszpath); 
	}
	_tcscat(ProcessNames[0], _T("1.exe"));
	_tcscat(ProcessNames[1], _T("2.exe"));
	_tcscat(ProcessNames[2], _T("3.exe"));
	_tcscat(ProcessNames[3], _T("4.exe"));
}

VOID WINAPI ServiceHandler(DWORD fdwControl)
{
	switch(fdwControl) 
	{
		case SERVICE_CONTROL_STOP:
		case SERVICE_CONTROL_SHUTDOWN:
			ProcessStarted = FALSE;
			ServiceStatus.dwWin32ExitCode = 0; 
			ServiceStatus.dwCurrentState  = SERVICE_STOPPED; 
			ServiceStatus.dwCheckPoint    = 0; 
			ServiceStatus.dwWaitHint      = 0;
			// terminate all processes started by this service before shutdown
			{
				for(int i = MAX_NUM_OF_PROCESS - 1 ; i >= 0; i--)
				{
					EndProcess(i);
					delete ProcessNames[i];
				}			
			}
			break; 
		case SERVICE_CONTROL_PAUSE:
			ServiceStatus.dwCurrentState = SERVICE_PAUSED; 
			break;
		case SERVICE_CONTROL_CONTINUE:
			ServiceStatus.dwCurrentState = SERVICE_RUNNING; 
			break;
		case SERVICE_CONTROL_INTERROGATE:
			break;
		default:
			if(fdwControl>=128&&fdwControl<256)
			{
				int nIndex = fdwControl&0x7F;
				// bounce a single process
				if(nIndex >= 0 && nIndex < MAX_NUM_OF_PROCESS)
				{
					EndProcess(nIndex);
					StartProcess(nIndex);
				}
				// bounce all processes
				else if(nIndex==127)
				{
					int i=0;
					for(i = MAX_NUM_OF_PROCESS-1; i >= 0; i--)
					{
						EndProcess(i);
					}
					for(i = 0; i < MAX_NUM_OF_PROCESS; i++)
					{
						StartProcess(i);
					}
				}
			}
			else
			{
				char pTemp[121];
				sprintf(pTemp,  "Unrecognized opcode %d\n", fdwControl);
				//WriteLog(pLogFile, pTemp);
			}
	};
    if (!SetServiceStatus(hServiceStatusHandle,  &ServiceStatus)) 
	{ 
		long nError = GetLastError();
		char pTemp[121];
		sprintf(pTemp, "SetServiceStatus failed, error code = %d\n", nError);
		//WriteLog(pLogFile, pTemp);
    } 
}
BOOL StartProcess(int ProcessIndex)
{
	STARTUPINFO startUpInfo = { sizeof(STARTUPINFO),NULL,_T(""),NULL,0,0,0,0,0,0,0,STARTF_USESHOWWINDOW,0,0,NULL,0,0,0};  
	startUpInfo.wShowWindow = SW_SHOW;
	startUpInfo.lpDesktop = NULL;
	if(CreateProcess(NULL, ProcessNames[ProcessIndex],NULL,NULL,FALSE,NORMAL_PRIORITY_CLASS,\
					 NULL,NULL,&startUpInfo,&pProcInfo[ProcessIndex]))
	{
		Sleep(1000);
		return TRUE;
	}
	else
	{
		long nError = GetLastError();
		char pTemp[256];
		sprintf(pTemp,"Failed to start program '%s', error code = %d\n", ProcessNames[ProcessIndex], nError); 
		//WriteLog(pLogFile, pTemp);
		return FALSE;
	}
}


VOID EndProcess(int ProcessIndex)
{
	if(ProcessIndex >=0 && ProcessIndex <= MAX_NUM_OF_PROCESS)
	{
		if(pProcInfo[ProcessIndex].hProcess)
		{
			// post a WM_QUIT message first
			PostThreadMessage(pProcInfo[ProcessIndex].dwThreadId, WM_QUIT, 0, 0);
			Sleep(1000);
			// terminate the process by force
			TerminateProcess(pProcInfo[ProcessIndex].hProcess, 0);
		}
	}
}
VOID ProcMonitorThread(VOID *)
{
	while(ProcessStarted == TRUE)
	{
		DWORD dwCode;
		for(int iLoop = 0; iLoop < MAX_NUM_OF_PROCESS; iLoop++)
		{
			if(::GetExitCodeProcess(pProcInfo[iLoop].hProcess, &dwCode) && pProcInfo[iLoop].hProcess != NULL)
			{
				if(dwCode != STILL_ACTIVE)
				{
					if(StartProcess(iLoop))
					{
						char pTemp[121];
						sprintf(pTemp, "Restarted process %d\n", iLoop);
						//WriteLog(pLogFile, pTemp);
					}
				}
			}
		}
	}
}

DWORD FindProcessId(const std::wstring& processName)
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);

	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if ( processesSnapshot == INVALID_HANDLE_VALUE )
		return 0;

	Process32First(processesSnapshot, &processInfo);
	if ( !processName.compare(processInfo.szExeFile) )
	{
		CloseHandle(processesSnapshot);
		return processInfo.th32ProcessID;
	}

	while ( Process32Next(processesSnapshot, &processInfo) )
	{
		if ( !processName.compare(processInfo.szExeFile) )
		{
			CloseHandle(processesSnapshot);
			return processInfo.th32ProcessID;
		}
	}
	
	CloseHandle(processesSnapshot);
	return 0;
}