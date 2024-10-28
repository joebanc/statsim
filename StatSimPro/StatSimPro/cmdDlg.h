#pragma once
#include "afxwin.h"


// CCmdDlg dialog

class CCmdDlg : public CDialog
{
	DECLARE_DYNAMIC(CCmdDlg)

public:
	CCmdDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCmdDlg();

// Dialog Data
	enum { IDD = IDD_CMDDLG };
	CString m_sRptPath, m_sTable;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CFont m_Font;
	CEdit m_EditCmd;
	afx_msg void OnBnClickedRun();
	afx_msg void OnBnClickedClose();
	virtual BOOL OnInitDialog();
};
