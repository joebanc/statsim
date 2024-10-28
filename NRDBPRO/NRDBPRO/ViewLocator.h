#if !defined(AFX_VIEWLOCATOR_H__6D37C6B9_7C84_414B_A735_81F596770120__INCLUDED_)
#define AFX_VIEWLOCATOR_H__6D37C6B9_7C84_414B_A735_81F596770120__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ViewLocator.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CViewLocator view

class CViewLocator : public CView
{
protected:
	CViewLocator();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CViewLocator)

// Attributes
protected:   

// Operations
public:
   void DrawLocator(CDC *pDC) {OnDraw(pDC);};

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewLocator)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CViewLocator();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CViewLocator)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWLOCATOR_H__6D37C6B9_7C84_414B_A735_81F596770120__INCLUDED_)
