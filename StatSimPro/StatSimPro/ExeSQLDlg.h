#pragma once
#include "afxwin.h"


// CExeSQLDlg dialog

class CExeSQLDlg : public CDialog
{
	DECLARE_DYNAMIC(CExeSQLDlg)

public:
	CExeSQLDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CExeSQLDlg();

// Dialog Data
	enum { IDD = IDD_EXESQLDLG };
	LPCTSTR m_sRptPath;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CFont m_Font;
	CEdit m_EditSQL;
	afx_msg void OnBnClickedRun();
	afx_msg void OnBnClickedClose();
	virtual BOOL OnInitDialog();
};
