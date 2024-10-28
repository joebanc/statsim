#pragma once


// CSelDataDlg dialog

class CSelDataDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelDataDlg)

public:
	CSelDataDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelDataDlg();

	LPCSTR m_sTable;
// Dialog Data
	//{{AFX_DATA(CSelDataDlg)
	enum { IDD = IDD_SELDATADLG };
	CComboBox	m_ComboData;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelDataDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelDataDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
