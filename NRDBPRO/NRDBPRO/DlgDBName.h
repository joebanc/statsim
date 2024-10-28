#if !defined(AFX_DLGDBNAME_H__F66C83FD_027D_4E88_9C07_809B94E43387__INCLUDED_)
#define AFX_DLGDBNAME_H__F66C83FD_027D_4E88_9C07_809B94E43387__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgDBName.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgDBName dialog

class CDlgDBName : public CDialog
{
// Construction
public:
	CDlgDBName(CWnd* pParent = NULL);   // standard constructor

   LPCSTR GetDBName() {return m_sDBName;};
protected:
   CString m_sDBName;

// Dialog Data
	//{{AFX_DATA(CDlgDBName)
	enum { IDD = IDD_DBNAME };
	CEdit	m_eDatabase;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgDBName)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgDBName)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDBNAME_H__F66C83FD_027D_4E88_9C07_809B94E43387__INCLUDED_)
