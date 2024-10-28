#if !defined(AFX_DLGCONNECT_H__D47C6B79_6AE1_45FE_83CA_F8DECA989363__INCLUDED_)
#define AFX_DLGCONNECT_H__D47C6B79_6AE1_45FE_83CA_F8DECA989363__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgConnect.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgConnect dialog

class CDlgConnect : public CDialog
{
// Construction
public:
	CDlgConnect(CWnd* pParent = NULL);   // standard constructor

   CString GetUserName() {return m_sUserName;}
   CString GetPassword() {return m_sPassword;}
   CString GetHost() {return m_sHost;}
    
protected:
   CString m_sPassword;
   CString m_sUserName;
   CString m_sHost;

// Dialog Data
	//{{AFX_DATA(CDlgConnect)
	enum { IDD = IDD_CONNECT };
	CEdit	m_eUserName;
	CEdit	m_ePassword;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgConnect)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgConnect)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCONNECT_H__D47C6B79_6AE1_45FE_83CA_F8DECA989363__INCLUDED_)
