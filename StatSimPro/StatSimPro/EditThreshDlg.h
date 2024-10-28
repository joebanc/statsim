#pragma once


// CEditThreshDlg dialog

class CEditThreshDlg : public CDialog
{
// Construction

	DECLARE_DYNAMIC(CEditThreshDlg)

private:
	float m_PovThresh0, m_PovThresh1, m_SubThresh0, m_SubThresh1;
	DWORD m_idUrb, m_idYear, m_idProv, m_idMun;
	CString m_spovthrfld, m_ssubthrfld;

public:
	CEditThreshDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditThreshDlg)
	enum { IDD = IDD_EDITTHRESHDLG };
	CComboBox	m_ComboMunicipality;
	CComboBox	m_ComboUrbanity;
	CEdit	m_EditSubThresh;
	CEdit	m_EditPovThresh;
	CComboBox	m_ComboYear;
	CComboBox	m_ComboProvince;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditThreshDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void UpdateDisplay();

	// Generated message map functions
	//{{AFX_MSG(CEditThreshDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboProvince();
	afx_msg void OnSelchangeComboYear();
	afx_msg void OnSelchangeComboUrbanity();
	virtual void OnOK();
	afx_msg void OnSelchangeComboMunicipality();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

