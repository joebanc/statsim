//////////////////////////////////////////////////////
//
// NRDB Pro - Spatial database and mapping application
//
// Copyright (c) 1989-2004 Richard D. Alexander
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// NRDB Pro is part of the Natural Resources Database Project 
// 
// Homepage: http://www.nrdb.co.uk/
// 

#include "stdafx.h"
#include "nrdb.h"
#include "ButtonAnimate.h"
#include "docmap.h"
#include "viewmap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////

#define PI 3.141592653

CImageFile CButtonAnimate::m_imagefile;

/////////////////////////////////////////////////////////////////////////////
// CButtonAnimate

CButtonAnimate::CButtonAnimate()
{
   m_nImage = 0;
}

CButtonAnimate::~CButtonAnimate()
{
   if (m_imagefile.IsOpen())
   {
      m_imagefile.Close();
   };
}


BEGIN_MESSAGE_MAP(CButtonAnimate, CButton)
	//{{AFX_MSG_MAP(CButtonAnimate)
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CButtonAnimate message handlers

void CButtonAnimate::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
   CRect rect;
   GetClientRect(&rect);
   CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
     
   CDocMap* pDocMap = BDGetDocMap();
   if (pDocMap != NULL)
   {
      pDocMap->GetViewMap()->DrawRect(pDC, rect);
   } else
   {
      CRect rect;
      GetClientRect(&rect);

    // If a logo is defined in the registry
            
      if (!m_imagefile.IsOpen())
      {
         char sLogoFile[256] = "";
         HKEY hKey;
         RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\Natural Resources Database\\NRDB Pro",  &hKey);      
         DWORD cb = sizeof(sLogoFile);
         DWORD nType;
         RegQueryValueEx(hKey, "LogoFile", 0, &nType, (BYTE*)sLogoFile, &cb);                              
         RegCloseKey(hKey);

          m_imagefile.Open(BDGetAppPath() + sLogoFile, FALSE);

          // Must draw once to create device context
          m_imagefile.OnDraw(pDC, rect);         
      };

      if (m_imagefile.IsOpen())
      {
         m_imagefile.OnDraw(pDC, rect);         
         return;
      } else
      {

    // Draw default bitmap

         CBitmap bitmap;
         bitmap.LoadBitmap(IDB_LOGO);      
         DrawBitmap(pDC, bitmap, rect, 110, 102);   
      };
   }

   CString s = "NRDB Pro";
   int nHeight = -24;

   CFont font;
   LOGFONT lf;
   memset(&lf,0,sizeof(LOGFONT));
   lf.lfHeight = nHeight;
   lf.lfPitchAndFamily = 12;   
   lf.lfCharSet = NRDB_CHARSET;
   strcpy(lf.lfFaceName, BDString(IDS_DEFAULTFONT));         
   font.CreateFontIndirect(&lf);        

   CFont* pFontOld = pDC->SelectObject(&font);
   pDC->SetBkMode(TRANSPARENT);
   pDC->SetTextColor(RGB(192,192,192));
   pDC->DrawText(s, &rect, DT_CENTER|DT_WORDBREAK);  	
   pDC->SetTextColor(RGB(0,0,192));
   rect.left-=1;
   rect.right-=1;
   pDC->DrawText(s, &rect, DT_CENTER|DT_WORDBREAK);  	
   pDC->SelectObject(pFontOld);
}

///////////////////////////////////////////////////////////////////////////////
//
// Microsoft Knowledge Base routine for transparent bitmaps

void CButtonAnimate::DrawBitmap(CDC* pDC, CBitmap& bitmap, CRect& rect, int nWidth, int nHeight)
{
   CDC dc;
   dc.CreateCompatibleDC(pDC);
   CBitmap* pBitmapOld = dc.SelectObject(&bitmap);
   
   pDC->StretchBlt(0,0, rect.Width(), rect.Height(), &dc, 0,0, nWidth, nHeight, 
                   SRCCOPY);

   dc.SelectObject(pBitmapOld);
}


/////////////////////////////////////////////////////////////////////////////

BOOL CButtonAnimate::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{   	
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

///////////////////////////////////////////////////////////////////////////////

void CButtonAnimate::Init(int nBitmap)
{                
   m_nTimer = 0;
   //SetTimer(0,1000/6,NULL);   
}

///////////////////////////////////////////////////////////////////////////////

void CButtonAnimate::OnTimer(UINT nIDEvent) 
{
   CRect rect;   

	m_nTimer++;
   
   m_nImage = m_nTimer%NUMIMAGES;

   Invalidate(TRUE);
   UpdateWindow();   
		
	CButton::OnTimer(nIDEvent);
}

///////////////////////////////////////////////////////////////////////////////

void CButtonAnimate::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// Post message to parent

   GetParent()->PostMessage(WM_LBUTTONDOWN);
	
	CButton::OnLButtonDown(nFlags, point);
}
