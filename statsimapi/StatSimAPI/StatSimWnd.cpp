#include "StdAfx.h"
#include "StatSimWnd.h"

CStatSimWnd::CStatSimWnd(CWnd* pWnd)
{
	if (!pWnd)
		m_pWnd = new CWnd();
	else
		m_pWnd = pWnd;

	if (!m_pWnd) {
		AfxMessageBox(_T("Failed to create object StatSimWnd."));
	}

	//m_pMFCRibButt = NULL;
	m_sText = NULL;
	m_pListCtrl = NULL;
	m_i = -1, m_j = -1;

	m_IsDestroyed = FALSE;

}

/*CStatSimWnd::CStatSimWnd(CMFCRibbonStatusBarPane* pMFCRibButt)
{
	////m_pMFCRibButt = pMFCRibButt;
	m_pWnd = NULL;
	m_sText = NULL;
	m_pListCtrl = NULL;
	m_i = -1, m_j = -1;

	m_IsDestroyed = FALSE;

}

*/
CStatSimWnd::CStatSimWnd(CListCtrl* pListCtrl, int i, int j)
{
	////m_pMFCRibButt = NULL;

	m_pListCtrl = pListCtrl;
	if (!m_pListCtrl) {
		AfxMessageBox(_T("Failed to create object StatSimWnd."));
	}
	m_i = i, m_j = j;
	m_pWnd = NULL;
	m_sText = NULL;
	m_IsDestroyed = FALSE;

}
CStatSimWnd::CStatSimWnd(LPCSTR *sText)
{
	//m_pMFCRibButt = NULL;
	m_sText = sText;
	if (!m_sText) {
		AfxMessageBox(_T("Failed to create object StatSimWnd."));
	}
	m_pWnd = NULL;
	m_pListCtrl = NULL;
	m_i = -1, m_j = -1;
	m_IsDestroyed = -1;
}

CStatSimWnd::~CStatSimWnd()
{

}

BOOL CStatSimWnd::SetText(LPCSTR sTxt) 
{
	if (m_pWnd) {
		m_pWnd->SetWindowText((CString) sTxt);
		return TRUE;
	}
	
	if (m_pListCtrl) {
		m_pListCtrl->SetItemText(m_i, m_j, (CString) sTxt);			
		return TRUE;		
	}
	
	if (m_sText) {
		*m_sText = sTxt;
		return TRUE;		
	}

	return FALSE;

}
 
HWND CStatSimWnd::GetSafeHwnd() 
{
	if (m_pWnd) {
		return m_pWnd->GetSafeHwnd();
	}
	
	if (m_pListCtrl) {
		return m_pListCtrl->GetSafeHwnd();
	}
	
	if (m_sText) {
		return NULL;
	}
	
	return NULL;
}

BOOL CStatSimWnd::DestroyWindow() 
{
	if (m_pWnd) {
		m_IsDestroyed = m_pWnd->DestroyWindow();
		return m_IsDestroyed;
	}
	if (m_pListCtrl) {
		m_IsDestroyed = m_pListCtrl->DestroyWindow();
		return m_IsDestroyed;
	}
	
	if (m_sText) {
		m_IsDestroyed = -1;
		return m_IsDestroyed;
	}

	return FALSE;

}
BOOL CStatSimWnd::Create( LPCSTR lpszClassName, LPCSTR lpszWindowName,
		DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
		UINT nID, CCreateContext* pContext)
{
	m_IsDestroyed = FALSE;
	m_pListCtrl = NULL;
	m_sText = NULL;
	return m_pWnd->Create((CString) lpszClassName, (CString) lpszWindowName,
		dwStyle, rect, pParentWnd, nID, pContext);

}

CStatSimBar::CStatSimBar(CProgressCtrl* pPB, int *nProg, int *min, int *max)
{
	m_pPB = pPB;
	delPB = FALSE;
	m_IsDestroyed = FALSE;
	
	if (!m_pPB) {
		AfxMessageBox(_T("Failed to create object CStatSimBar."));
	}

	if (!nProg){
		m_nProg = new int;
		(*m_nProg) = (int) 0;
		delprog = TRUE;
	}
	else {
		m_nProg = nProg;
		delprog = FALSE;
	}

	if (!min){
		m_nMin = new int;
		(*m_nMin) = (int) 0;
		delmin = TRUE;
	}
	else {
		m_nMin = min;
		delmin = FALSE;
	}

	if (!max){
		m_nMax = new int;
		(*m_nMax) = (int) 100;
		delmax = TRUE;
	}
	else {
		m_nMax = max;
		delmax = FALSE;
	}

	if (m_pPB)
	{
		m_pPB->SetRange32( (int) (*m_nMin), (int) (*m_nMax) );
		m_pPB->SetPos( (int) (*m_nProg) );
	}

}

CStatSimBar::CStatSimBar(int *nProg, int *min, int *max)
{

	m_nProg = nProg;
	delprog = FALSE;
	delPB = FALSE;

	m_IsDestroyed = -1;

	if (!min){
		m_nMin = new int;
		(*m_nMin) = (int) 0;
		delmin = TRUE;
	}
	else {
		m_nMin = min;
		delmin = FALSE;
	}

	if (!max){
		m_nMax = new int;
		(*m_nMax) = (int) 100;
		delmax = TRUE;
	}
	else {
		m_nMax = max;
		delmax = FALSE;
	}

	m_pPB = NULL;

}
CStatSimBar::CStatSimBar(int Index, int SubIndex):
				m_Index(Index), m_SubIndex(SubIndex)
{
	m_pPB = new CProgressCtrl();
	delPB = TRUE;
	m_IsDestroyed = FALSE;

	m_nProg = new int;
	(*m_nProg) = (int) 0;
	delprog = TRUE;

	m_nMin = new int;
	(*m_nMin) = (int) 0;
	delmin = TRUE;

	m_nMax = new int;
	(*m_nMax) = (int) 100;
	delmax = TRUE;

}
				
CStatSimBar::~CStatSimBar()
{
	if (delPB)
		delete m_pPB; m_pPB = 0;
	if (delprog)
		delete m_nProg; m_nProg = 0;
	if (delmin)
		delete m_nMin; m_nMin = 0;
	if (delmax)
		delete m_nMax; m_nMax = 0;
}

void CStatSimBar::SetRange(int min, int max)
{
	*m_nMin = (int) min;
	*m_nMax = (int) max;

	if (m_pPB)
	{
		m_pPB->SetRange32(min, max);

	}
}

void CStatSimBar::SetPos(int nProg)
{
	*m_nProg = (int) nProg;

	if (m_pPB)
	{
		m_pPB->SetPos(nProg);

	}
}

HWND CStatSimBar::GetSafeHwnd() 
{
	if (m_pPB) {
		return m_pPB->GetSafeHwnd();
	}
	
	if (m_nProg) {
		return NULL;
	}

	return NULL;

}

BOOL CStatSimBar::DestroyWindow() 
{
	if (m_pPB) {
		m_IsDestroyed = m_pPB->DestroyWindow();
		return m_IsDestroyed;
	}
	
	if (m_nProg) {
		m_IsDestroyed = -1;
		return m_IsDestroyed;
	}

	return FALSE;

}

BOOL CStatSimBar::Create( DWORD dwStyle,
		const RECT& rect,
		CWnd* pParentWnd,
		UINT nID )
{
	m_IsDestroyed = FALSE;
	m_nProg = NULL;
	return m_pPB->Create(dwStyle, rect,
		pParentWnd, nID);

}
