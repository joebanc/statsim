#pragma once

//#include "StatSimStream.h"
#include "SSVariants.h"

#pragma warning(disable:4311)
#pragma warning(disable:4312)
#pragma comment(lib, "gdiplus.lib")
#include <gdiplus.h>
using namespace Gdiplus;

#ifndef CS_DROPSHADOW
#define CS_DROPSHADOW 0x00020000
#endif
#ifndef LWA_ALPHA
#define LWA_ALPHA 0x00000002
#endif
#ifndef WS_EX_LAYERED
#define WS_EX_LAYERED 0x00080000
#endif

#define WM_PROGRESSUPDATE WM_USER + 100

//A helper class that constructs a modal dialog from a template in memory
class CSphereMem
{
public:
	CSphereMem(const wchar_t* szClassName, DWORD classStyle, DWORD style, DWORD dwExtendedStyle, void* pParam, DLGPROC dlgProc, HWND hParent)
	{
		HLOCAL hDlgTemplate = NULL;
		void FAR* lpDlgTemplate = NULL;
		DLGTEMPLATE dlgTemplate;
   		UINT nMenuNameLength = NULL;
		UINT nClassNameLength = NULL;
		void* pMenuName = NULL;
		void* pClassName = NULL;

		WNDCLASS wc;
		GetClassInfo(AfxGetInstanceHandle(), _MBCS("#32770"), &wc);
		_cochar str(szClassName); //conversion
		wc.lpszClassName = str;
		//wc.lpszClassName = szClassName;
		wc.style = CS_HREDRAW | CS_VREDRAW | classStyle;
		AfxRegisterClass(&wc);

		memset(&dlgTemplate, 0, sizeof(dlgTemplate));

		//Menu
		nMenuNameLength = sizeof(WORD);
		pMenuName = new BYTE[nMenuNameLength];
		*((WORD*)pMenuName) = 0x0000;

		//Class
		nClassNameLength = sizeof(WORD);
		nClassNameLength += ((UINT)wcslen(szClassName)) * sizeof(WCHAR);
		pClassName = new BYTE[nClassNameLength];
		*((WORD*) pClassName) = 0x0000;
		wcscpy((WCHAR*)pClassName, szClassName);

		dlgTemplate.style = style | DS_CENTER | DS_MODALFRAME;
		dlgTemplate.dwExtendedStyle = dwExtendedStyle;
		dlgTemplate.x = 0;
		dlgTemplate.y = 0;
		dlgTemplate.cx = 0;
		dlgTemplate.cy = 0;

		hDlgTemplate = LocalAlloc(LHND, (((sizeof(DLGTEMPLATE) +  nMenuNameLength + nClassNameLength) + 3) & ~3));
		lpDlgTemplate = LocalLock(hDlgTemplate);

		BYTE* pMain = (BYTE*)lpDlgTemplate;
		DLGTEMPLATE* pTemplate = (DLGTEMPLATE*)lpDlgTemplate;
		*pTemplate = dlgTemplate;

		pMain += sizeof(dlgTemplate);

		int nLenght = nMenuNameLength;
		BYTE* p = (BYTE*) pMenuName;
		for(; nLenght--; *pMain++ = *p++);

		nLenght = nClassNameLength;
		p = (BYTE*) pClassName;
		for(; nLenght--; *pMain++ = *p++);

		DialogBoxIndirectParam(NULL, (LPCDLGTEMPLATE)lpDlgTemplate, hParent, dlgProc,(LPARAM)pParam);

		LocalUnlock(hDlgTemplate);
		LocalFree(hDlgTemplate);

		delete pClassName;
		delete pMenuName;
	}

	~CSphereMem()
	{
	}
};

class CSphereProgress
{
	typedef bool (*PROGRESSFUNC)(CSphereProgress*);

	HWND m_hWnd;
	bool m_bAllowCancel;
	CWinThread*	m_hThread;
	PROGRESSFUNC m_hThreadProc;
	ULONG_PTR m_gdiplusToken;
	double m_dPos;
	wchar_t* m_strTitle;
	bool m_bContinue;
	
	friend INT_PTR CALLBACK ProgressDlgProc(HWND, UINT, WPARAM, LPARAM);
	friend UINT ThreadProc(LPVOID pData);

public:
	bool Continue()
	{
		return m_bContinue;
	}

	HWND GetWindowHandle()
	{
		return m_hWnd;
	}

	CSphereProgress(HWND hParentWnd, PROGRESSFUNC hThreadProc, const wchar_t* strTitle, bool bAllowCancel = false)
	{
		GdiplusStartupInput gdiplusStartupInput;
		GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

		m_hThread = NULL;
		m_bAllowCancel = bAllowCancel;
		m_hThreadProc = hThreadProc;

		m_strTitle = new wchar_t[wcslen(strTitle) + 1];
		wcscpy(m_strTitle, strTitle);
		m_strTitle[wcslen(strTitle)] = 0;	

		CSphereMem dlg(L"CSphereProgress", CS_DROPSHADOW, WS_POPUP | WS_DISABLED, WS_EX_LAYERED, this, ProgressDlgProc, hParentWnd);
	}

	virtual ~CSphereProgress(void)
	{
		GdiplusShutdown(m_gdiplusToken);
		delete m_strTitle;
	}

	inline void SetProgress(CString strText, double dPos)
	{
		m_dPos = dPos;
		::SendMessage(m_hWnd, WM_PROGRESSUPDATE, (WPARAM)strText.GetBuffer(), 0);
	}
};

static UINT ThreadProc(LPVOID pData)
{
	CSphereProgress* pSphereProgress = (CSphereProgress*) pData;
	bool bRet = pSphereProgress->m_hThreadProc(pSphereProgress);

	EndDialog(pSphereProgress->m_hWnd, 2);

	return bRet;
}

static INT_PTR CALLBACK ProgressDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch(Message)
	{
		case WM_INITDIALOG:
		{
			CSphereProgress *pProgressDialog = (CSphereProgress*) lParam;

			typedef BOOL (WINAPI* lpfnSetLayeredWindowAttributes)(HWND hWnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);
			
			HMODULE hDll = ::LoadLibrary(_MBCS("USER32.dll"));

			if(hDll)
			{
				lpfnSetLayeredWindowAttributes pFn = NULL;
				pFn = (lpfnSetLayeredWindowAttributes)GetProcAddress(hDll, "SetLayeredWindowAttributes");
				
				if(pFn)
					pFn(hDlg, NULL, 200, LWA_ALPHA);

				::FreeLibrary(hDll);
			}

			pProgressDialog->m_hWnd = hDlg;

			SetWindowLongPtr(hDlg, GWL_USERDATA,(LONG)pProgressDialog);
			SendMessage(hDlg, WM_PROGRESSUPDATE, 0, 0);

			RECT rc;
			GetWindowRect(GetParent(hDlg), &rc);

			if(rc.right - rc.left < 400 || rc.bottom - rc.top < 200)
				GetWindowRect(GetDesktopWindow(), &rc);

			rc.left = rc.left + (rc.right - rc.left - 400) / 2;
			rc.top = rc.top  + (rc.bottom - rc.top  - 200) / 2;
			rc.right = rc.left + 400;
			rc.bottom = rc.top  + 200;

			SetWindowPos(hDlg, NULL, rc.left, rc.top, 400, 200, SWP_NOZORDER | SWP_SHOWWINDOW);
			pProgressDialog->m_hThread = AfxBeginThread(ThreadProc, pProgressDialog);
		}
		case WM_COMMAND:
		{
			if(IDCANCEL == LOWORD(wParam))
			{
				CSphereProgress* pProgressDialog = (CSphereProgress*)GetWindowLongPtr(hDlg, GWL_USERDATA);
				
				if(pProgressDialog->m_bAllowCancel)
				{
					pProgressDialog->m_bContinue = false;

					if(pProgressDialog->m_hThread)
						WaitForSingleObject(pProgressDialog->m_hThread, 5000);

					EndDialog(hDlg, 2);
				}
				
				return TRUE;
			}

			break;
		}		
		case WM_PROGRESSUPDATE:
		{
			CSphereProgress* pProgressDialog = (CSphereProgress*)GetWindowLongPtr(hDlg, GWL_USERDATA);
			wchar_t* strStatus = (wchar_t*)wParam;
			double dProgress = pProgressDialog->m_dPos;

			HDC dc = GetDC(hDlg);
			HDC dcMem;
			HBITMAP bm;
			
			bm = CreateCompatibleBitmap(dc, 400, 200);
			dcMem = CreateCompatibleDC(dc);
			SelectObject(dcMem, bm);

			//////////////////////////
			Graphics graphics(dcMem);
			graphics.SetSmoothingMode(SmoothingModeAntiAlias);

			GraphicsPath path;
			path.AddEllipse(25, 25, 150, 150);
			INT num = 1;

			PathGradientBrush pthGrBrush(&path);
			pthGrBrush.SetGammaCorrection(TRUE);
			pthGrBrush.SetSurroundColors(&Color(255, 64, 64, 64), &num);
			pthGrBrush.SetCenterColor(Color(255, 128, 128, 128));

			PathGradientBrush pthGrBrush2(&path);
			pthGrBrush2.SetGammaCorrection(TRUE);
			pthGrBrush2.SetSurroundColors(&Color(255, 64, 64, 64), &num);
			pthGrBrush2.SetCenterColor(Color(255, 255, 128, 64));

			graphics.FillRectangle(&SolidBrush(Color(255, 128, 128, 128)), -5, -5, 410, 210);
			graphics.FillPie(&pthGrBrush, 25, 25, 150, 150, 0, 360);
			graphics.FillPie(&pthGrBrush2, 25, 25, 150, 150, 0, (365 * (REAL)dProgress) / 100);
			//graphics.DrawRectangle(&Pen(Color(255, 255, 255, 255), 2), 0, 0, 399, 199);
			graphics.DrawPie(&Pen(Color(255, 100, 100, 100)), 25, 25, 150, 150, 0, 360);
			graphics.DrawPie(&Pen(Color(255, 100, 100, 100)), 25, 25, 150, 150, 0, (365 * (REAL)dProgress) / 100);

			StringFormat format;
			format.SetAlignment(StringAlignmentNear);
			Font fontBold(L"Tahoma", 10, FontStyleBold);
			Font fontNormal(L"Tahoma", 8);

			graphics.DrawString(pProgressDialog->m_strTitle, -1, &fontBold, RectF(201, 26, 176, 151), &format, &SolidBrush(Color(255, 0, 0, 0)));
			graphics.DrawString(pProgressDialog->m_strTitle, -1, &fontBold, RectF(200, 25, 175, 150), &format, &SolidBrush(Color(255, 255, 255, 255)));
			graphics.DrawString(strStatus, -1, &fontNormal, RectF(200, 70, 175, 105), &format, &SolidBrush(Color(255, 255, 255, 255)));
			//////////////////////////////

			BitBlt(dc, 0, 0, 400, 200, dcMem, 0, 0, SRCCOPY);
			
			DeleteObject(bm);
			DeleteDC(dcMem);
			ReleaseDC(hDlg, dc); 

			return TRUE;
		}
	}

	return FALSE;
}

#pragma warning(default:4311)
#pragma warning(default:4312)

bool ProgressFunction(CSphereProgress* pSphereProgress)
{
	for(int i = 0; i < 5000; i++)
	{
		if(!pSphereProgress->Continue())
			return 1;

		CString strMessage;
		strMessage.Format(_MBCS("Current number is %i"), i);
		pSphereProgress->SetProgress(strMessage, (double)i/50.0f);
	}

	return 0;
}