#pragma once
#include "afxwin.h"
#ifndef STATSIMPRO_H
	#include "StatSimPro.h"
#endif

#include <vector>

// CLoginDlg dialog

class CLoginDlg : public CDialog
{
	DECLARE_DYNAMIC(CLoginDlg)

private:
	CFont m_Font;
	CStringArray m_sDB;
	int m_nDB;

public:
	CLoginDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLoginDlg();

// Dialog Data
	enum { IDD = IDD_LOGIN };

	// Drop down of databases
	CComboBox m_ComboDB;
	CButton m_OK;
	CEdit m_TextHost;
	CEdit m_TextPort;
	CEdit m_TextUser;
	CEdit m_TextPwd;

	void PopulateDB();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
