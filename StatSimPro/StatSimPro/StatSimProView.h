// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// StatSimProView.h : interface of the CStatSimProView class
//


#pragma once


class CStatSimProView : public CHtmlView
{
protected: // create from serialization only
	CStatSimProView();
	DECLARE_DYNCREATE(CStatSimProView)

// Attributes
private:
	LPCTSTR m_sNavPath;

public:
	//CStatSimProDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CStatSimProView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	DECLARE_MESSAGE_MAP()
};

//#ifndef _DEBUG  // debug version in StatSimProView.cpp
//inline CStatSimProDoc* CStatSimProView::GetDocument() const
//   { return reinterpret_cast<CStatSimProDoc*>(m_pDocument); }
//#endif

