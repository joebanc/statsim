#pragma once


// CSSGCombo

class CSSGCombo : public CComboBox
{
	DECLARE_DYNAMIC(CSSGCombo)
// Construction
public:
	CSSGCombo();

   void SetCurSel(int index);                  

// Attributes
protected:
   int m_iDefault;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSSGCombo)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSSGCombo();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSSGCombo)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnCloseup();
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
