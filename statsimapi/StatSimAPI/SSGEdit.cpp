// SSGEdit.cpp : implementation file
//

#include "stdafx.h"
#include "StatSimAPI.h"
#include "SSGEdit.h"


// CSSGEdit

IMPLEMENT_DYNAMIC(CSSGEdit, CEdit)
/////////////////////////////////////////////////////////////////////////////
// CSSGEdit

CSSGEdit::CSSGEdit()
{
}

CSSGEdit::~CSSGEdit()
{
}


BEGIN_MESSAGE_MAP(CSSGEdit, CEdit)
	//{{AFX_MSG_MAP(CSSGEdit)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_CHAR()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//
// Store default so that it can be reset if escape is pressed

void CSSGEdit::SetWindowText(LPCSTR lpszString)
{
   m_sDefault = lpszString;
   CEdit::SetWindowText((CString) lpszString);
}

/////////////////////////////////////////////////////////////////////////////
// CSSGEdit message handlers

void CSSGEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{	
   // Allow parent to handle tab/return key etc.

   if (nChar == VK_TAB || nChar == VK_RETURN || nChar == VK_ESCAPE)
   {
      GetParent()->PostMessage(WM_KEYDOWN, nChar, MAKELPARAM(nRepCnt, nFlags));
   } else
   {
	   CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
   };
}

void CSSGEdit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_SHIFT)
   {
      GetParent()->PostMessage(WM_KEYUP, nChar, MAKELPARAM(nRepCnt, nFlags));
   } else
   {	
	   CEdit::OnKeyUp(nChar, nRepCnt, nFlags);
   };
}

void CSSGEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// Remove to prevent annoying beep
	
   if (nChar == VK_RETURN || nChar == VK_ESCAPE || nChar == VK_TAB)
   {
   } else
   {
	   CEdit::OnChar(nChar, nRepCnt, nFlags);
   };
}

///////////////////////////////////////////////////////////////////////////////

void CSSGEdit::OnKillFocus(CWnd* pNewWnd) 
{
	CEdit::OnKillFocus(pNewWnd);
	
   // Inform parent if losing focus whilst in edit mode

      GetParent()->PostMessage(WM_SETFOCUS,(WPARAM)this, 0);

}

///////////////////////////////////////////////////////////////////////////////

BOOL CSSGEdit::PreTranslateMessage(MSG* pMsg) 
{	   
   // Necessary for keys such as tab      
   
   if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
   {
	   if (pMsg->message == WM_KEYDOWN)      
      {  
         if (pMsg->wParam == VK_ESCAPE)
         {
            CEdit::SetWindowText(m_sDefault);
         }
         OnKeyDown(pMsg->wParam, LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));                                       
         GetParent()->SetFocus();
         return TRUE;
      } 
      else if (pMsg->message == WM_KEYUP)
      {                
         OnKeyUp(pMsg->wParam, LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));                        
         GetParent()->SetFocus();
         return TRUE;
      };                 
   };       
	
	return CEdit::PreTranslateMessage(pMsg);
}


