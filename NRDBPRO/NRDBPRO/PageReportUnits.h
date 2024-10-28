#if !defined(AFX_PAGEREPORTUNITS_H__4A4C2DE6_12D1_4D8B_845C_7A44FAE5779F__INCLUDED_)
#define AFX_PAGEREPORTUNITS_H__4A4C2DE6_12D1_4D8B_845C_7A44FAE5779F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageReportUnits.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPageReportUnits dialog

class CPageReportUnits : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageReportUnits)

// Construction
public:
	CPageReportUnits();
	~CPageReportUnits();

// Dialog Data
	//{{AFX_DATA(CPageReportUnits)
	enum { IDD = IDD_REPORTUNITS };
	CComboBox	m_cbArea;
	CComboBox	m_cbLength;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageReportUnits)
	public:
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageReportUnits)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEREPORTUNITS_H__4A4C2DE6_12D1_4D8B_845C_7A44FAE5779F__INCLUDED_)
