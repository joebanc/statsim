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
#include "BDView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

#define POINTS_PER_INCH 72

/////////////////////////////////////////////////////////////////////////////

BOOL CBDView::m_bCopy;

/////////////////////////////////////////////////////////////////////////////
// CBDView


IMPLEMENT_DYNCREATE(CBDView, CView)

CBDView::CBDView()
{
   m_bCopy = FALSE;
}

CBDView::~CBDView()
{
}


BEGIN_MESSAGE_MAP(CBDView, CView)
	//{{AFX_MSG_MAP(CBDView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBDView drawing

void CBDView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CBDView diagnostics

#ifdef _DEBUG
void CBDView::AssertValid() const
{
	CView::AssertValid();
}

void CBDView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBDView message handlers

void CBDView::OnEditCopy()
{      
   // Copy to clipboard using a bitmap

    CBitmap     cBmp;
    CClientDC   cWndDC(this);   // View is an hWnd, so we can use "this"
    CDC         cMemDC;         // Handle to a memory DC
    CRect     rect;             // For storing the size of the window

    cMemDC.CreateCompatibleDC(&cWndDC); // Create the memory DC.

    GetClientRect(rect);         // Get the size of the window
    
    // Clear the background for copying

    cWndDC.FillSolidRect(&rect, RGB(255,255,255));   
    
    // Draw the contents of the window
    m_bCopy = TRUE;    

    OnDraw(&cWndDC);   
    m_bCopy = FALSE;

    //cBmp.CreateCompatibleBitmap(&cMemDC, rect.Width(),rect.Height() );
    cBmp.CreateCompatibleBitmap(GetDC(), rect.Width(),rect.Height() );
    // Keep the old bitmap
    CBitmap* pOldBitmap = cMemDC.SelectObject(&cBmp);

    cMemDC.BitBlt(0, 0, rect.Width(),rect.Height(), &cWndDC, 0, 0,SRCCOPY); 

    // here are the actual clipboard functions.
    AfxGetApp()->m_pMainWnd->OpenClipboard() ;
    EmptyClipboard() ;
    SetClipboardData (CF_BITMAP, cBmp.GetSafeHandle() ) ;
    CloseClipboard () ;
        // next we select the old bitmap back into the memory DC
        // so that our bitmap is not deleted when cMemDC is destroyed.
        // Then we detach the bitmap handle from the cBmp object so that
        // the bitmap is not deleted when cBmp is destroyed.
    cMemDC.SelectObject(pOldBitmap);
    cBmp.Detach();

    // Redraw the window
    
    Invalidate();
    RedrawWindow();

    return;
}

/////////////////////////////////////////////////////////////////////////////

void CBDView::ScaleFont(CDC* pDC, LOGFONT* pLogfont)
{
   int nPixelsX = pDC->GetDeviceCaps(LOGPIXELSX);
   int nPixelsY = pDC->GetDeviceCaps(LOGPIXELSY);      

   // Layout, make font smaller, equally for large jpeg exports make
   // the font large

   if (pDC->IsPrinting() && pDC->GetDeviceCaps(PHYSICALWIDTH) == 0)   
   {  
      int nDefaultWidth = pDC->GetDeviceCaps(HORZRES);
      int nDefaultHeight = pDC->GetDeviceCaps(VERTRES);     

      nPixelsX = (nPixelsX * m_nWidth) / nDefaultWidth;
      nPixelsY = (nPixelsY * m_nHeight) / nDefaultHeight;      

      // Arbitary adjustment
      nPixelsX = (nPixelsX * 3) / 4;
      nPixelsY = (nPixelsY * 3) / 4;

   }

   pLogfont->lfWidth =  MulDiv(nPixelsX, -pLogfont->lfWidth, POINTS_PER_INCH);      
   pLogfont->lfHeight = MulDiv(nPixelsY,-pLogfont->lfHeight,POINTS_PER_INCH);      


}

