#pragma once
#include "statsimlist.h"
#include "StatSimConn.h"


// CDataDefnDlg dialog

class CDataDefnDlg : public CDialog
{
	DECLARE_DYNAMIC(CDataDefnDlg)

public:
	CDataDefnDlg(CStatSimConn *pSrcDB, CStatSimConn *pToDB, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDataDefnDlg();

// Dialog Data
	enum { IDD = IDD_DATADEFNDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	// List of data dictionaries
	CStatSimList m_ListDataDefn;
public:
	virtual BOOL OnInitDialog();

private:
	CStatSimConn *m_pSrcDB, *m_pToDB;

	afx_msg void OnBnClickedOk();
};
