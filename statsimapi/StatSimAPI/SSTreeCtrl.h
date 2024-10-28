#pragma once


// CSSTreeCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSSTreeCtrl window

class __declspec(dllexport) CSSTreeCtrl : public CTreeCtrl
{
	DECLARE_DYNAMIC(CSSTreeCtrl)
// Construction
public:
	CSSTreeCtrl();


// Attributes
public:
	void SetDefaultCursor() ;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSSTreeCtrl)
	//}}AFX_VIRTUAL

//*********** CURSORS  *********************
	HCURSOR cursor_hand ;
	HCURSOR cursor_arr	;
	HCURSOR cursor_no	;
	bool	isCursorArrow;
//******************************************


//*************  DRAG & DROP ******************
	protected:
	CImageList*	m_pDragImage;
	BOOL		m_bLDragging;
	HTREEITEM	m_hitemDrag,m_hitemDrop;

//*********************************************

// Implementation
public:
	COLORREF memDC_bgColor_bitmap;
	bool	 isImageTiled;

	CString		RemoveBuddyCountFromName(CString name);
	int			GetChildCountInGroup(CString group_name);
		
	bool		DeleteChild(CString buddy_name);
	HTREEITEM	MoveChildItem(HTREEITEM hItem, HTREEITEM htiNewParent, HTREEITEM htiAfter);
	HTREEITEM	GetBuddyTreeItem(CString buddy_name);
	HTREEITEM	AddChild(CString buddy_name, CString group_name);
	bool		DeleteGroup(CString group_name);
	HTREEITEM	GetGroupTreeItem( CString  grp_name);
	HTREEITEM	AddGroup(const CString & group_name);
	virtual		~CSSTreeCtrl();

	BOOL SetBkImage(UINT nIDResource) ;
	BOOL SetBkImage(LPCSTR lpszResourceName) ;

	// Generated message map functions
protected:
	//{{AFX_MSG(cBuddyTree)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTvnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnQueryNewPalette();
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
	CPalette m_pal;
	CBitmap m_bitmap;
	int m_cxBitmap, m_cyBitmap ;
};

