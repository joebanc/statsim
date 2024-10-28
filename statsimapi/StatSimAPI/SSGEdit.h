#pragma once


// CSSGEdit

class CSSGEdit : public CEdit
{
	DECLARE_DYNAMIC(CSSGEdit)

public:
	CSSGEdit();
	void SetWindowText(LPCSTR lpszString);

// Attributes
protected:
   CString m_sDefault;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSSGEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSSGEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSSGEdit)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
