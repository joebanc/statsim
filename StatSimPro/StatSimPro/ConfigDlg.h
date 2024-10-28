#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "StatSimList.h"
#include "StatSimTree.h"
#include "StatSimConn.h"


// CConfigDlg dialog

class CConfigDlg : public CDialog
{
	DECLARE_DYNAMIC(CConfigDlg)

public:
	CConfigDlg(CStatSimConn *pSrcDB, CStatSimConn *pToDB, CWnd* pParent = NULL);   // standard constructor
	virtual ~CConfigDlg();

// Dialog Data
	enum { IDD = IDD_CONFIGDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

// attributes
public:
	
	CButton m_Cancel;
	CComboBox m_ComboLevel;
	CButton m_Deselect;
	CButton m_DeselectAll;

	// list of selected items
	CStatSimList m_ListSel;
	
	// list of items not yet selected
	CStatSimList m_ListUnsel;
	
	CButton m_Select;
	CButton m_SelectAll;
	CButton m_Ok;

	// view button
	CButton m_View;
	// change selection
	CButton m_Change;

	// tree of the levels that will be selected
	CStatSimTree* m_TreeLevel;

	// frame that enclose the selection
	CStatic m_FrameSelect;

private:
	DWORD m_hlimit;
	CStatSimConn *m_pSrcDB, *m_pToDB;

	//number of rows, columns
	int m_nListI;
	long m_nListJ;
	//handler of column names;
	LPCSTR* m_sListCols;
	//element handler
	CStatSimElt* m_pElt;

// functions
public:
	
	//Recorset operations
	// Checked records
	void RefreshCheckList();


	afx_msg void OnBnClickedSelect();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnSelchangeCombolevel();
	afx_msg void OnBnClickedCancel();

public:
	afx_msg void OnBnClickedView();
public:
	afx_msg void OnBnClickedDeselect();
public:
	afx_msg void OnBnClickedSelectall();
public:
	afx_msg void OnBnClickedDeselectall();
public:
	afx_msg void OnBnClickedChange();
public:
	afx_msg void OnNMDblclkListunsel(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnNMDblclkListsel(NMHDR *pNMHDR, LRESULT *pResult);
};
