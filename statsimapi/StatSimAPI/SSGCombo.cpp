// SSGCombo.cpp : implementation file
//

#include "stdafx.h"
#include "StatSimAPI.h"
#include "SSGCombo.h"
#include "StatSimGrid.h"


// CSSGCombo

IMPLEMENT_DYNAMIC(CSSGCombo, CComboBox)
/////////////////////////////////////////////////////////////////////////////
// CSSGCombo

CSSGCombo::CSSGCombo()
{
   m_iDefault = -1;
}

CSSGCombo::~CSSGCombo()
{
}


BEGIN_MESSAGE_MAP(CSSGCombo, CComboBox)
	//{{AFX_MSG_MAP(CSSGCombo)
	ON_WM_KEYDOWN()
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseup)
	ON_WM_KEYUP()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CSSGCombo::SetCurSel(int index)
{
   m_iDefault = index;
   CComboBox::SetCurSel(index);
}

/////////////////////////////////////////////////////////////////////////////
// CSSGCombo message handlers

void CSSGCombo::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{      
   CComboBox::OnKeyDown(nChar, nRepCnt, nFlags);
}

///////////////////////////////////////////////////////////////////////////////

void CSSGCombo::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{	
   CComboBox::OnKeyUp(nChar, nRepCnt, nFlags);
}

///////////////////////////////////////////////////////////////////////////////

void CSSGCombo::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	  if (nChar == VK_RETURN || nChar == VK_ESCAPE || nChar == VK_TAB)
   {
   } else
   {
	   CComboBox::OnChar(nChar, nRepCnt, nFlags);
   };
}

///////////////////////////////////////////////////////////////////////////////

void CSSGCombo::OnCloseup() 
{
   // Send rather than post so has immediate effect 
   GetParent()->PostMessage(WM_SETEDITMODE, FALSE);		
}


///////////////////////////////////////////////////////////////////////////////

BOOL CSSGCombo::PreTranslateMessage(MSG* pMsg) 
{
  // Necessary for keys such as tab      
   
   UINT& nChar = (UINT&) pMsg->wParam;

   if (pMsg->message == WM_KEYDOWN)      
   {  
      // When escaping, restore the previous selection

      if (nChar == VK_ESCAPE)
      {
         SetCurSel(m_iDefault);
      }

      if (nChar == VK_TAB || nChar == VK_RETURN || nChar == VK_ESCAPE || 
          nChar == VK_LEFT || nChar == VK_RIGHT || nChar == VK_SHIFT)
      {	                        
         OnKeyDown(pMsg->wParam, LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));                                       
         GetParent()->SetFocus();
         return TRUE;
      };
   } 
    
   else if (pMsg->message == WM_KEYUP)
   {  
      if (nChar == VK_SHIFT)
      {         
         OnKeyUp(pMsg->wParam, LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));                        
         GetParent()->SetFocus();
         return TRUE;
      };                 
   }; 
   
	
	return CComboBox::PreTranslateMessage(pMsg);
}


