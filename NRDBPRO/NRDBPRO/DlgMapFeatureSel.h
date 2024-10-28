#if !defined(AFX_DLGMAPFEATURESEL_H__FB35BAD9_DFE2_4C5C_911E_A0D7CB8D8DD6__INCLUDED_)
#define AFX_DLGMAPFEATURESEL_H__FB35BAD9_DFE2_4C5C_911E_A0D7CB8D8DD6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgMapFeatureSel.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgMapFeatureSel dialog

class CDlgMapFeatureSel : public CDialog
{
// Construction
public:
	CDlgMapFeatureSel(long lFtype, CWnd* pParent = NULL);   // standard constructor

   long GetFeatureId() {return m_lFeature;}

protected:

   long m_lFType;
   long m_lFeature;

// Dialog Data
	//{{AFX_DATA(CDlgMapFeatureSel)
	enum { IDD = IDD_MAPFEATURESEL };
	CListBox	m_lbFeatures;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMapFeatureSel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgMapFeatureSel)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelect();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMAPFEATURESEL_H__FB35BAD9_DFE2_4C5C_911E_A0D7CB8D8DD6__INCLUDED_)
