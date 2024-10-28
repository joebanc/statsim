#if !defined(AFX_DLGVIEWATTR_H__4A73CD09_A0CD_4BC8_8124_34C7F524673C__INCLUDED_)
#define AFX_DLGVIEWATTR_H__4A73CD09_A0CD_4BC8_8124_34C7F524673C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgViewAttr.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgViewAttr dialog

class CDlgViewAttr : public CDialog
{
// Construction
public:
	CDlgViewAttr(CWnd* pParent = NULL);   // standard constructor

   void Update(long lFeatureId, long lFType);

protected:
   

public:

// Dialog Data
	//{{AFX_DATA(CDlgViewAttr)
	enum { IDD = IDD_VIEWATTR };
	CListBox	m_lbAttr;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgViewAttr)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgViewAttr)
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGVIEWATTR_H__4A73CD09_A0CD_4BC8_8124_34C7F524673C__INCLUDED_)
