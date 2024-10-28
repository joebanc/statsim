#pragma once

#include "SSGEdit.h"
#include "SSGCombo.h"

/////////////////////////////////////////////////////////////////////////////
// CStatSimGrid window

#define SSM_LEAVECELL WM_USER+1
#define SSM_DATACHANGE WM_USER+2
#define SSM_KEYDOWN WM_USER+3
#define SSM_COMBOSELCHANGE WM_USER+4

#define WM_SETEDITMODE WM_USER+5 

#define SS_USERRESIZE_COL 1
#define SS_ENTERACTION_NEXT 1
#define ES_STATIC 1
#define SS_CHRSET_CHR 1
#define SS_CASE_NOCASE 1
#define SS_CB_DROPDOWN 1

#define SSB_PROCESSTAB 2
#define SSB_EDITMODEREPLACE 3

///////////////////////////////////////////////////////////////////////////////

enum {Edit=1, Combo, Number};

///////////////////////////////////////////////////////////////////////////////

struct SS_CELLTYPE
{
   BYTE  m_nType;
   short m_nLength; // Edit controls

   CString m_sItems; // Combobox


   SS_CELLTYPE() {m_nType = Edit; m_nLength = 0;}
};

typedef long SS_COORD;
typedef long* LPSS_COORD;
typedef double* LPDOUBLE;
typedef SS_CELLTYPE* LPSS_CELLTYPE;

struct SS_CELLCOORD
{
   SS_COORD Col;
   SS_COORD Row;

   SS_CELLCOORD() {Col = -2; Row = -2;}
};

typedef SS_CELLCOORD* LPSS_CELLCOORD;

///////////////////////////////////////////////////////////////////////////////

class CStatSimCell
{
public:

friend class CStatSimGrid;

   CStatSimCell() {m_dwItemData = 0; m_nIndex = -1;}

protected:
   
   CString m_sText;   
   int m_nIndex;
   DWORD m_dwItemData;      
};

class CSSGRow : public CArray <CStatSimCell, CStatSimCell>
{
friend class CStatSimGrid;

public:
   CSSGRow& operator=(CSSGRow&);
   CSSGRow() {m_dHeight = 1;}
   CSSGRow(CSSGRow& );

protected:
   double m_dHeight;
};

struct CGridColumn
{
   long m_lData;
   double m_dWidth;
   BOOL m_bLocked;
   BOOL m_bVisible;

   SS_CELLTYPE m_celltype;

   CGridColumn() {m_lData = 0; m_dWidth = -1; m_bLocked = FALSE; m_bVisible=TRUE;}
};

///////////////////////////////////////////////////////////////////////////////

class __declspec(dllexport) CStatSimGrid : public CButton
{
	DECLARE_DYNAMIC(CStatSimGrid)
// Construction
public:
	CStatSimGrid();

// Operations
public:

   CString GetValue(SS_COORD Col, SS_COORD Row) 
   {return m_aGrid[Row][Col].m_sText;};
   BOOL GetActiveCell (LPSS_COORD Col, LPSS_COORD Row) 
   {*Col = m_nActiveCol; *Row = m_nActiveRow; return TRUE;}
   BOOL GetColUserData(SS_COORD Col,LPLONG lplUserData)
   {*lplUserData = m_aColumns[Col].m_lData; return TRUE;}
   BOOL GetInteger(SS_COORD Col, SS_COORD Row, LPLONG lplValue)
   {*lplValue = m_aGrid[Row][Col].m_dwItemData; return TRUE;}   
   int GetIndex(SS_COORD Col, SS_COORD Row)
   {return m_aGrid[Row][Col].m_nIndex;};   
   void SetIndex(SS_COORD Col, SS_COORD Row, int index)
   {m_aGrid[Row][Col].m_nIndex = index;}
   BOOL SetInteger(SS_COORD Col, SS_COORD Row, long lValue)
   {m_aGrid[Row][Col].m_dwItemData = lValue; return TRUE;}
   BOOL SetColor(SS_COORD Col, SS_COORD Row, COLORREF Background,
                COLORREF Foreground);
   BOOL SetValue(SS_COORD Col, SS_COORD Row, LPCSTR lpData);   
   WORD SetUserResize(WORD wUserResize) 
   {return 0;}
   BOOL SetBool(short nIndex, BOOL bNewVal)
   {return 0;}
   WORD SetEditEnterAction(WORD wAction)
   {return 0;}
   BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);   
   LRESULT ComboBoxSendMessage (SS_COORD Col, SS_COORD Row, UINT Msg, WPARAM wParam, LPARAM lParam);   
   void SetMaxRows(SS_COORD MaxRows);
   BOOL SetFont(SS_COORD Col, SS_COORD Row, HFONT hFont, BOOL fDeleteFont)
   {m_hFont = hFont; return TRUE;}
   BOOL SetColWidth(SS_COORD Col, double Width);      
   BOOL GetRowHeight(SS_COORD Row, LPDOUBLE lpHeight)
   {*lpHeight = m_aGrid[Row].m_dHeight; return TRUE;}
   BOOL SetRowHeight(SS_COORD Row, double Height)
   {m_aGrid[Row].m_dHeight = Height; return TRUE;}
   BOOL SetColUserData(SS_COORD Col, long lUserData)
   {m_aColumns[Col].m_lData = lUserData; return TRUE;}
   LPSS_CELLTYPE SetTypeEdit(LPSS_CELLTYPE CellType, long Style, short Len,
                            short ChrSet, short ChrCase);   
   BOOL SetCellType(SS_COORD Col, SS_COORD Row, LPSS_CELLTYPE CellType);
   BOOL SetLock(SS_COORD Col, SS_COORD Row, BOOL Lock);   
   LPSS_CELLTYPE SetTypeComboBox(LPSS_CELLTYPE CellType, long Style,
                                LPCSTR lpItems);   
   LPSS_CELLTYPE SetTypeInteger(LPSS_CELLTYPE CellType, long Min, long Max);   
   void SetMaxCols(SS_COORD MaxCols);   
   SS_COORD GetMaxRows() {return m_nRows-1;}
   SS_COORD GetMaxCols() {return m_nCols-1;}
   BOOL DelRow(SS_COORD Row);   
   BOOL GetSelectBlock(LPSS_CELLCOORD CellUL, LPSS_CELLCOORD CellLR)
      {*CellUL = m_celltopleft; *CellLR = m_cellbottomright; return m_celltopleft.Row != -2;};
   BOOL ClearDataRange(SS_COORD Col,  SS_COORD Row, 
                      SS_COORD Col2, SS_COORD Row2);  
   BOOL SetActiveCell(SS_COORD Col, SS_COORD Row, int nFlags = save);   
   BOOL InsRow(SS_COORD nRow);
   BOOL SetEditMode(BOOL fEditModeOn, int nChar = -1);
   BOOL GetEditMode(void);
   BOOL ShowCol(SS_COORD Col, BOOL bShow);

protected:
   double GetColWidth(SS_COORD Col);    
   COLORREF GetCellColor(SS_COORD Col,  SS_COORD Row);
   BOOL GetCellPoint(CPoint point, SS_COORD* Col, SS_COORD* Row);
   int GetRectCell(SS_COORD Col, SS_COORD Row,  CRect* pRect);   

// Attributes
protected:

   CSSGRow m_aCols;
   CArray <CSSGRow, CSSGRow> m_aGrid;
   CArray <CGridColumn, CGridColumn> m_aColumns;
   
   long m_nActiveRow;
   long m_nActiveCol;
   long m_nFocusRow;
   long m_nFocusCol;

   long m_nRows;
   long m_nCols;

   long m_nFirstVisibleRow;
   long m_nFirstVisibleCol;
   long m_nVisibleColumns;
   long m_nVisibleRows;   

   CArray <CRect, CRect> m_aRectCols, m_aRectRows;

   double m_dColWidth;

   HFONT m_hFont;

   CSSGEdit m_edit;
   CSSGCombo m_combo;
   BOOL m_bEditMode;

   SS_CELLCOORD m_celltopleft;
   SS_CELLCOORD m_cellbottomright;      

   CWnd* m_pParent;

   BOOL m_bShift;
   BOOL m_bLButton;
   BOOL m_bCtrl;
   
   int m_nDragCol;

   HCURSOR m_hDragCursor;
   HCURSOR m_hDefaultCursor;
   BOOL m_bDragCursor;

   // Undo

   CString m_sUndo;
   int m_nUndoRow, m_nUndoCol;

   enum {save=1, clearsel=2};

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatSimGrid)
	public:
   virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStatSimGrid();

	// Generated message map functions
protected:
	//{{AFX_MSG(CStatSimGrid)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);      
	afx_msg void OnEditUndo();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
   
   LRESULT OnCopy(WPARAM, LPARAM);
   LRESULT OnCut(WPARAM, LPARAM);
   LRESULT OnPaste(WPARAM, LPARAM);
   LRESULT OnSetEditMode(WPARAM, LPARAM);
   
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
