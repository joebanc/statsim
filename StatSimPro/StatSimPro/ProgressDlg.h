#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CProgressDlg dialog

class CProgressDlg : public CDialog
{
	//DECLARE_DYNAMIC(CProgressDlg)
	DECLARE_DYNCREATE(CProgressDlg);

public:
	CProgressDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProgressDlg();

// Dialog Data
	enum { IDD = IDD_PROGRESSDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	// Caption in progress
	CStatic m_ProgressLabel;
	// Progress bar
	CProgressCtrl m_ProgressBar;
	CButton m_Cancel;

private:
    CView* m_pView;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCancel();

	// attributes
	BOOL m_IsCancel;
};
