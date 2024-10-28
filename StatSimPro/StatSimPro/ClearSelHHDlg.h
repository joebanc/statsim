#pragma once
#include "statsimlist.h"
#include "StatSimConn.h"


// CClearSelHHDlg dialog

class CClearSelHHDlg : public CDialog
{
	DECLARE_DYNAMIC(CClearSelHHDlg)

public:
	CClearSelHHDlg( CWnd* pParent = NULL);   // standard constructor
	virtual ~CClearSelHHDlg();

// Dialog Data
	enum { IDD = IDD_CLEARSELHH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	// List of data dictionaries
	CStatSimList m_ListGrp;
public:
	virtual BOOL OnInitDialog();

private:

	afx_msg void OnBnClickedOk();
};
