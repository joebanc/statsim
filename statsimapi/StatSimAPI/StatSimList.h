#pragma once
#ifndef STATSIMRS
	#include "StatSimRS.h"
#endif

//Define STATSIMLIST
#ifndef STATSIMLIST
	#define STATSIMLIST
#endif
///////////////////

#ifndef SSHEADERCTRL
	#include "SSHeaderCtrl.h"
#endif

enum REFRESH_OPT { FRESH = 0, 
	EMPTY, NOACTION };

// CStatSimList

class CStatSimWnd;
class CStatSimBar;
class CStatSimTree;


class __declspec(dllexport) CStatSimList : public CListCtrl
{
	DECLARE_DYNAMIC(CStatSimList)
// Construction
public:
	CStatSimList(BOOL wProgressBars = FALSE, int PBCtrlID = WM_USER);

// Attributes
	enum CheckState{ NOSTATE = 0, UNCHECKED, CHECKED, PARTIALLY };

public:

	// the array of the Progress Controls in the list control
	//CArray<CProgressCtrl*,CProgressCtrl*> m_ProgressBars;
	CProgressCtrl** m_ProgressBars;
	CStatic** m_ProgressLabels;
	// array of handles to windows
	int *m_nPositions;
	LPCSTR *m_sCaptions;
	
	// the column which should contain the progress bars
	int m_ProgressColumn, m_PercentColumn;
	//the identifier of the control
	int m_PBCtrlID;

// Operations
public:

	//overloaded member functions
	int InsertColumn(int nCol, LPCSTR lpszColumnHeading, 
		int nFormat = LVCFMT_LEFT,
		int nWidth = -1,
		int nSubItem = -1
		);

	// Extended functions
	BOOL SetCheckEx( int index, CheckState nCheck );

	void ResizeProgress(int Index, int SubIndex = 0);
	void ResizeProgress();
	void SetProgress(int Index, int prog);
//	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void CreateProgress(int Index, int SubIndex = 0);

	// new set image list
	CImageList* SetImageListEx( HICON* pIcons, int nIcon, 
		int nILT = LVSIL_SMALL,
		int cx = 13, int cy = 13, 
		UINT nFlags = ILC_COLOR32
		);

	// initialize the column containing the bars
	void InitProgressColumn(int ColNum=0);
	void InitPercentColumn(int ColNum=0);

	void CleanUp(LPCSTR* sColumns, int nJ,
		BOOL enable = FALSE, REFRESH_OPT refresh = EMPTY, 
		int* widthArray = NULL, short ltrim = 0, short rtrim = 0);

	void InsertBulk(LPCSTR* sCols, int nJ, LPCSTR** sItems, int nI, 
		BOOL enable = TRUE, REFRESH_OPT refresh = NOACTION,
		int* widthArray = NULL);

	void InsertBulk(CStatSimRS *pRS, CStatSimWnd* pLabel = NULL, CStatSimBar* pProgBar = NULL,
		BOOL enable = TRUE, REFRESH_OPT refresh = EMPTY, int* widthArray = NULL, 
		bool dwFld = FALSE, DWORD* dwConst = NULL, UINT nDW = 0);

	LPCSTR** GetItemArray();
	LPCSTR* GetSelectedArray(LPCSTR critVal = NULL, int critRow = 0, int critCol = 0);
	int GetnRows();
	int GetSelectedItem();

	void RefreshColArray();
	LPCSTR GetColString(int nCol);

	BOOL Exists(DWORD dwID, DWORD *dwConst, UINT nDW);
	DWORD* GetIDArray();

	void SwitchContents(CStatSimList* pToList = NULL, BOOL bldArr = FALSE);	//Switch with other list
	void TransferContents(CStatSimList* pToList = NULL);	//Transfer to other list

	void SetImage(UINT bmpID, int cx = 16, int nGrow = 0, COLORREF colorRef = 1,
		int nILT = LVSIL_SMALL);

	//the one painting the progress bars
	void InitProgressBars();
	void DestroyProgressBars();

	//set the source tree
	void SetTreeCtrl(CStatSimTree *pTreeCtrl);

	int GetColumnCount();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatSimList)
	//}}AFX_VIRTUAL

private:
// Attributes
	CFont m_Font;
	CImageList* pImageList;

	const BOOL m_wProgressBars;

	LPCSTR m_sProgress;
	float m_nPercent;

	int nRows, nCols;
	DWORD* dwIDArray;
	//LPCSTR* sColArray;	//array of column lables
	tstrvec1d* sColArray;	//array of column lables
	//CArray<LPCSTR, LPCSTR> *sColArray;	//array of column lables
	
	LPCSTR** mItemArray;	//array of items
	//CSSArrayBuilder* pArrBldr;
	tstrvec2d itemArray;

	CStatSimTree* m_pDragSourceTree;
// Implementation
	BOOL sItemExists(LPCSTR sItem);	//item i,0 exists?

	void InitmPtrs();

protected:
	void RefreshItemArray();	//Refresh item array


public:
	virtual ~CStatSimList();

	// Generated message map functions
protected:
	//{{AFX_MSG(CStatSimList)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	//the trick
	afx_msg void OnPaint();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////

