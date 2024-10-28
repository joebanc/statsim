#pragma once
#include "StatSimList.h"

#define DT_ID 0
#define DT_DESC 1

// CMergeDataDlg dialog

class CMergeDataDlg : public CDialog
{
	DECLARE_DYNAMIC(CMergeDataDlg)

	CMergeDataDlg(LPCSTR sUsingPath, CWnd* pParent = NULL);   // standard constructor

	LPCTSTR m_sRptPath;
// Dialog Data
	//{{AFX_DATA(CMergeDataDlg)
	enum { IDD = IDD_MERGEDATADLG };
	CListBox	m_ListMerge;
	CListBox	m_ListUsing;
	CListBox	m_ListMaster;
	CButton	m_OptSelect;
	CButton	m_OptKey;
	CComboBox	m_ComboUsing;
	CComboBox	m_ComboMaster;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMergeDataDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

private:

	LPCSTR m_sUsingPath, m_sFileTitle, m_sFolderPath, m_sFileName;
	bool m_IsKey;
	CStatSimConn* m_pUsingDB;

	int m_nDataTypes;
	LPCSTR** m_pMapDataTypes;

	int m_nKeys,
		m_nMasterVars, m_nUsingVars,
		m_nMasterSrc, m_nUsingSrc;
	CStringArray m_sMasterKeys, m_sUsingKeys,
		m_sMasterVars, m_sUsingVars,
		m_sMasterSrc, m_sUsingSrc,
		m_sSelMasterSrc, m_sSelUsingSrc,
		m_sSelMasterVars, m_sSelUsingVars;
	CString m_sSQLFromU, m_sSQLFromM,
		m_sSQLSelectU, m_sSQLSelectM;
	
	//methods
	LPCSTR GetsDataType(int id);
	void RefreshVars(LPCSTR sMasterVar, LPCSTR sUsingVar);
	
protected:

	// Generated message map functions
	//{{AFX_MSG(CMergeDataDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnOptKey();
	afx_msg void OnOptSelect();
	afx_msg void OnClose();
	afx_msg void OnSelchangeComboMaster();
	afx_msg void OnSelchangeComboUsing();
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
