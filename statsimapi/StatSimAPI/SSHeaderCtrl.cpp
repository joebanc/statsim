// SSHeaderCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "StatSimAPI.h"
#include "SSHeaderCtrl.h"
#include "StatSimList.h"


// CSSHeaderCtrl

IMPLEMENT_DYNAMIC(CSSHeaderCtrl, CHeaderCtrl)

CSSHeaderCtrl::CSSHeaderCtrl()
{

}

CSSHeaderCtrl::~CSSHeaderCtrl()
{
}


BEGIN_MESSAGE_MAP(CSSHeaderCtrl, CHeaderCtrl)
	//{{AFX_MSG_MAP(CSSHeaderCtrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(HDN_ENDTRACKW, OnEndTrack)
//	ON_NOTIFY_REFLECT(HDN_BEGINTRACKW, OnBeginTrack)
	ON_NOTIFY_REFLECT(HDN_ENDTRACKA, OnEndTrack)
//	ON_NOTIFY_REFLECT(HDN_BEGINTRACKA, OnBeginTrack)
END_MESSAGE_MAP()


// CSSHeaderCtrl message handlers

BOOL CSSHeaderCtrl::Init(CHeaderCtrl *pHeader)
{	
	ASSERT(pHeader && pHeader->GetSafeHwnd());
	if (!SubclassWindow(pHeader->GetSafeHwnd()))
	{
		OutputDebugString(_T("Unable to subclass existing header!\n"));
		return FALSE;
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
/*
	OnEndTrack
	
	Params:
		pNMHDR	pointer to NMHDR structure
		pResult	pointer to LRESULT code

	Returns:
		None
*/
/////////////////////////////////////////////////////////////////////////////
void CSSHeaderCtrl::OnEndTrack(NMHDR * pNMHDR, LRESULT* pResult)
{
	NMHEADER *pHdr = (NMHEADER*)pNMHDR;
	CStatSimList* pList = (CStatSimList*) GetParent();
	pList->ResizeProgress();
	*pResult = 0;
}

