// ViewLocator.cpp : implementation file
//

#include "stdafx.h"
#include "nrdbpro.h"
#include "ViewLocator.h"
#include "docmap.h"
#include "viewmap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewLocator

IMPLEMENT_DYNCREATE(CViewLocator, CView)

CViewLocator::CViewLocator()
{

}

CViewLocator::~CViewLocator()
{
}


BEGIN_MESSAGE_MAP(CViewLocator, CView)
	//{{AFX_MSG_MAP(CViewLocator)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewLocator drawing

void CViewLocator::OnDraw(CDC* pDC)
{
   CRect rect;
   GetClientRect(&rect);

   CDocMap* pDoc = (CDocMap*)GetDocument();

  // If printing then determine where the locator map is to be displayed

   if (pDC->IsPrinting() )
   {
      rect = CRect(0,0,0,0);

      if (!BDGetApp()->GetLayout().IsAuto())
      {
         CMapLayoutObj layoutobj = pDoc->GetLayers()->GetMapLayout().GetLayoutObj(CMapLayout::locator);
         CRect rectLocator = layoutobj.m_rect;   

         if (rectLocator.Width() != 0)
         {         
            rect = CMapLayout::RectFromPercent(pDoc->GetViewMap()->GetRectPrint(), rectLocator);      
         }
      };
   } 

   // Draw the preview window

	if (rect.Width() != 0)
   {
      pDoc->GetViewMap()->DrawRect(pDC, rect, TRUE);
   };
	
}

/////////////////////////////////////////////////////////////////////////////
// CViewLocator diagnostics

#ifdef _DEBUG
void CViewLocator::AssertValid() const
{
	CView::AssertValid();
}

void CViewLocator::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CViewLocator message handlers
