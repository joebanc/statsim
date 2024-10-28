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
#include "ButtonCredits.h"
#include "buttonanimate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

#define LOGO_WIDTH 299
#define LOGO_HEIGHT 284

#define DELAY 100

/////////////////////////////////////////////////////////////////////////////

CFont CButtonCredits::m_font;

/////////////////////////////////////////////////////////////////////////////

inline void swap(BYTE& n1, BYTE& n2)
{
   int n = n1;
   n1 = n2;
   n2 = n;
}

/////////////////////////////////////////////////////////////////////////////
// CButtonCredits

CButtonCredits::CButtonCredits()
{  
   // Create default font
   
   if (m_font.m_hObject == NULL)
   {
      LOGFONT lf;
      memset(&lf,0,sizeof(LOGFONT));
      lf.lfHeight = -11;
      lf.lfPitchAndFamily = 12;   
      lf.lfWeight = 0;
      lf.lfCharSet = NRDB_CHARSET;
      strcpy(lf.lfFaceName, "MS Sans Serif");         
      m_font.CreateFontIndirect(&lf);        
   };   

   // Retrieve logos etc.

   char s[256] = "";

   HKEY hKey;
   RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\Natural Resources Database\\NRDB Pro",  &hKey);      

   DWORD l = sizeof(s);
   DWORD nType;
   RegQueryValueEx(hKey, "Description", 0, &nType, (BYTE*)s, &l);                        
   m_sDescription = s;
   RegCloseKey(hKey);
}

///////////////////////////////////////////////////////////////////////////////

CButtonCredits::~CButtonCredits()
{
   ReleaseCapture();
}


BEGIN_MESSAGE_MAP(CButtonCredits, CButton)
	//{{AFX_MSG_MAP(CButtonCredits)	
	ON_WM_TIMER()
	ON_WM_DRAWITEM()	   
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CButtonCredits message handlers

void CButtonCredits::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
   CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
   
   Credits(pDC);	
}

/////////////////////////////////////////////////////////////////////////////

void CButtonCredits::Credits(CDC* pDC)
{
   CRect rect;
   GetClientRect(&rect);

   // Prerelease version

   /*LOGFONT lf;
   memset(&lf,0,sizeof(lf));
   lf.lfHeight = 40;
   lf.lfPitchAndFamily = 18;   
   lf.lfCharSet = NRDB_CHARSET;   
   strcpy(lf.lfFaceName, BDString(IDS_DEFAULTFONT));      
   CFont font;
   font.CreateFontIndirect(&lf);       
   CFont *pFontOld = pDC->SelectObject(&font);
   pDC->SetBkMode(TRANSPARENT);          
   pDC->SetTextColor(RGB(192,192,192));
   pDC->DrawText("NRDB Pro development release", &rect, DT_CENTER|DT_WORDBREAK);    
   pDC->SetTextColor(RGB(0,0,0));
   pDC->SelectObject(pFontOld);*/

   // Draw credits

   CString s;
                    
	s += "Professional version\r\n(NRDB Pro)\r\n\r\n";
	  
	  
      s += "\r\nNRDB Std and NRDB Pro were developed by Richard D. Alexander\r\n\r\n";       

      if (m_sDescription == "")
      {               
         s += "NRDB was originally developed for the Bohol Environment Management Office, Philippines, "
              "through the assistance of Voluntary Service Overseas (V.S.O)";       
      };      
      
      
   CFont *pFontOld = pDC->SelectObject(&m_font);
   pDC->DrawText(s, &rect, DT_CENTER|DT_WORDBREAK);
   pDC->SelectObject(pFontOld);   
   

   
}


///////////////////////////////////////////////////////////////////////////////

void CButtonCredits::OnTimer(UINT nIDEvent) 
{   	
	CButton::OnTimer(nIDEvent);
}


BOOL CButtonCredits::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CButton::PreCreateWindow(cs);
}
