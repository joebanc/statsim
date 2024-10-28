#pragma once
#ifndef STATSIMWND
	#define STATSIMWND
#endif

//#include "afxribbonbutton.h"

//class CMFCRibbonStatusBarPane;

//wrapper of window labels
class __declspec(dllexport) CStatSimWnd
{

public:

	//normal window derivatives
	CStatSimWnd(CWnd* pWnd = NULL);
	//list controls
	CStatSimWnd(CListCtrl* pListCtrl, int i, int j);

	//string only
	CStatSimWnd(LPCSTR* sText);

	//MFC Ribbon Button
	//CStatSimWnd(CMFCRibbonStatusBarPane* pMFCRibButt = NULL);

	virtual ~CStatSimWnd(void);

	BOOL SetText(LPCSTR sTxt);

	HWND GetSafeHwnd();
	BOOL DestroyWindow();
	BOOL m_IsDestroyed;

	BOOL Create( LPCSTR lpszClassName, LPCSTR lpszWindowName,
		DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
		UINT nID, CCreateContext* pContext = NULL );

	operator LPCSTR*()
	{
		return m_sText;
	}

	CWnd* m_pWnd;
	CListCtrl* m_pListCtrl;
	//CMFCRibbonStatusBarPane* m_pMFCRibButt;
	LPCSTR* m_sText;
	int m_i, m_j;

};

class __declspec(dllexport) CStatSimBar
{

public:

	//normal window derivatives
	CStatSimBar(CProgressCtrl* pPB, int *nProg=0, int *min=0, int *max=0);

	//integer only
	CStatSimBar(int *nProg, int *min=0, int *max=0);
    
	// list control item
	CStatSimBar(int Index, int SubIndex);

	int m_Index;
    int m_SubIndex;

	virtual ~CStatSimBar(void);

	void SetRange(int min, int max);
	void SetPos(int nProg);

	HWND GetSafeHwnd();
	BOOL DestroyWindow();
	BOOL m_IsDestroyed;
	
	BOOL Create( DWORD dwStyle,
		const RECT& rect,
		CWnd* pParentWnd,
		UINT nID );
	
	CProgressCtrl* m_pPB;

	operator int*()	{ return m_nProg;}

private:
	BOOL delPB, delprog, delmin, delmax;
	int *m_nProg, *m_nMin, *m_nMax;

};
