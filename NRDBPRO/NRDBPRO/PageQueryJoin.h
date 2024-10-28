#if !defined(AFX_PAGEQUERYJOIN_H__6C522B7E_2291_4478_AC97_455C6741B196__INCLUDED_)
#define AFX_PAGEQUERYJOIN_H__6C522B7E_2291_4478_AC97_455C6741B196__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageQueryJoin.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPageQueryJoin dialog

class CPageQueryJoin : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageQueryJoin)

// Construction
public:
   CPageQueryJoin(CQuery* pQuery, CQuery* pQueryPrev, int nType);
	CPageQueryJoin();
	~CPageQueryJoin();

protected:
   CQuery* m_pQuery;
   CQuery* m_pQueryPrev;
   int m_nType;

   void Update();

// Dialog Data
	//{{AFX_DATA(CPageQueryJoin)
	enum { IDD = IDD_QUERYJOIN };
	CListBox	m_lbJoins;
	CComboBoxFType	m_cbFTypeTo;
	CEditPlus	m_eDistance;
	CBDComboBox	m_cbCondition;
	CBDComboBox	m_cbAttrTo;
	CBDComboBox	m_cbAttrFrom;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageQueryJoin)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageQueryJoin)
	afx_msg void OnSector();
	afx_msg void OnSelchangeFeatureto();
	afx_msg void OnSelchangeAttrto();
	afx_msg void OnSelchangeCondition();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEQUERYJOIN_H__6C522B7E_2291_4478_AC97_455C6741B196__INCLUDED_)
