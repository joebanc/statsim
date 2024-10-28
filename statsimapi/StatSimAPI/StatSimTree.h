#pragma once

#ifndef STATSIMLIST
	#include "StatSimList.h"
#endif

class CStatSimConn;

// CStatSimTree
#ifndef STATSIMTREE
	#define STATSIMTREE
#endif

class __declspec(dllexport) CStatSimTree : public CTreeCtrl
{
	DECLARE_DYNAMIC(CStatSimTree)
// Construction
public:
	CStatSimTree(BOOL wDrag = FALSE);

// Attributes
	enum CheckState{ NOSTATE = 0, UNCHECKED, CHECKED, PARTIALLY };

public:

// Operations
	void Initialize(CStatSimConn* pStatSimConn, LPTSTR rootlb, DWORD limit, 
		CStatSimWnd* pLabel = NULL, CStatSimBar* pProgBar = NULL);
	HTREEITEM* ConferChild(CStatSimConn* pStatSimConn, HTREEITEM hselItem, USHORT right = 9);	//populate children of hselItem
	void Populate(CStatSimConn* pStatSimConn, HTREEITEM hselItem, DWORD limit, USHORT right = 9);	//populate children of hselItem
	void PopulateChild(CStatSimConn* pStatSimConn, HTREEITEM hselItem, USHORT right = 9);	//populate children of hselItem
	void Populate(CStatSimConn* pStatSimConn, LPTSTR rootlb, USHORT right = 9, 
		CStatSimWnd* pLabel = NULL, CStatSimBar* pProgBar = NULL);	//populate children of hselItem
	UINT hIndex(DWORD itmDta);
	CString hTable(UINT i);
	CString hLabel(UINT i);
	CString hIDFld(UINT i);
	CString hItmDta(UINT i);
	CString hDigit(UINT i);

	//returns a pointer to the two dimensional tree items array
	HTREEITEM** GetItemArray();
	HTREEITEM** GetParentItemArray();
	//returns a pointer to the size array
	ULONG* GetSizeArray();
	//returns number of levels
	UINT nLevels();

	//tree state check
	BOOL SetCheckEx( HTREEITEM hItem, CheckState nCheck );
	CImageList* SetImageListEx( HICON* pIcons, int nIcon, 
		int nILT = TVSIL_STATE,
		int cx = 13, int cy = 13, 
		UINT nFlags = ILC_COLOR32
		);

	// set the object list
	void SetListCtrl(CStatSimList *pListCtrl);
	HTREEITEM MoveChildItem(HTREEITEM hItem, HTREEITEM htiNewParent, HTREEITEM htiAfter);

private:
// Attributes

	BOOL m_wDrag;

	CStatSimConn* m_pStatSimConn;
	CImageList *pImageList, *pStateImage;
	HTREEITEM** phItemArray;	//[j] - level; [i] - case
	HTREEITEM** phParentItemArray;	//[j] - level; [i] - case
	UINT hLevels;
	ULONG* hSizeArray;
	CStringArray hTableArray, hLabelArray, hIDFldArray, 
		hItmDtaArr, hDigitArray;

	int nlimit;

	LPCSTR m_sProgress;
	float m_nPercent;

	//drag and drop routines
	CImageList*	m_pDragImage;
	BOOL		m_bLDragging;
	HTREEITEM	m_hitemDrag,m_hitemDrop;
	// the tree's object list
	CStatSimList* m_pDragTargetList;
	CPoint m_DropOffPoint;
	CWnd* m_DropOffWnd;

	CPalette m_pal;
	CBitmap m_bitmap;
	int m_cxBitmap, m_cyBitmap ;

// Operations
	void ClearChild(HTREEITEM hselItem);
	
	//sets whether children items are checked or not
	void SetChildChecks(HTREEITEM hselItem, CheckState nCheck = CHECKED);
	//sets whether parents are checked or not
	void SetParentChecks(HTREEITEM hselItem);
	//determines if all children follows the same status as nCheck
	BOOL ChildrenAllChecked(HTREEITEM hselItem, CheckState nCheck = CHECKED);
	//determines the status of all the children
	CheckState ChildrenCheckStatus(HTREEITEM hselItem);


	UINT hItemIndex(HTREEITEM* phItem, CString critVal);
	HTREEITEM GetParent(ULONG i, ULONG j,
		CString critVal, USHORT left = 0, USHORT right = 9);	//Get parent of item [j][i]

public:
	void SetDefaultCursor() ;
//*********** CURSORS  *********************
	HCURSOR cursor_hand ;
	HCURSOR cursor_arr	;
	HCURSOR cursor_no	;
	bool	isCursorArrow;
//******************************************

	COLORREF memDC_bgColor_bitmap;
	bool	 isImageTiled;

	BOOL SetBkImage(UINT nIDResource) ;
	BOOL SetBkImage(LPCSTR lpszResourceName) ;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatSimTree)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStatSimTree();

	// Generated message map functions
protected:
	//{{AFX_MSG(CStatSimTree)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnPaint();

	afx_msg void OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnQueryNewPalette();
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
public:
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
};

/////////////////////////////////////////////////////////////////////////////

