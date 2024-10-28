#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include "StatSimList.h"
#include "StatSimConn.h"

#include "StatSimElt.h"

enum UD {UP = 0, DOWN};
#define STR_EQUAL 0

// CEditLevelDlg dialog

class CEditLevelDlg : public CDialog
{
	DECLARE_DYNAMIC(CEditLevelDlg)

public:
	CEditLevelDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditLevelDlg();

// Dialog Data
	enum { IDD = IDD_EDITLEVELDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	// List of level names to be edited
	CStatSimList m_ListLevel;
	// Navigational list of levels
	CStatSimList m_ListLevelNav;
	// Display text information of current level
	CEdit m_TextSelect;
	CButton m_Cancel;
	CButton m_Ok;

// Implementation
protected:

	void PopulateNav(BOOL wroot = FALSE, BOOL navend = FALSE, 
		LPCSTR navFld = _MBCS(""), LPCSTR navVal = _MBCS(""));
	void PopulateEdt(LPCSTR edtFld = _MBCS(""), LPCSTR edtVal = _MBCS(""));

	void Populate(LPCSTR* selArray, 
		BOOL wNav = TRUE, BOOL pass = TRUE);

	void Pass(ELEMENT, UD);

	BOOL WRoot(ELEMENT);
	BOOL NavEnd(ELEMENT);

	void UpdateFill();
	void UpdateSelLables(LPCSTR sLabel, UD);
	
	void EditLevelLabel();
	void UpdateAndChange();

//Attributes
private:
	
	CEdit *pEditCtrl;

	//tstrvec2d m_NavItmArray;	//	LPCSTR** mNavItemArr;	//array of items in navigation
	LPCSTR* m_NavColArray;	//	LPCSTR* sNavCols;	//array of column labels in navigation
	//tstrvec2d m_EdtItmArray;	//	LPCSTR** mEditItemArr;	//array of items in edit list
	LPCSTR* m_EdtColArray;	//	LPCSTR* sEditCols;	//array of column labels in edit list

	CComboBox* pComboLevel;
	CStatSimRS *plevelRS, *peditRS;

	CStatSimElt *pnavElt, *pedtElt;

	CString sNavElt, sEdtElt, sEdtEltLB, sEdtEltIDFld;
	ELEMENT navElt, edtElt, parElt, chiElt;
	USHORT nNavAttr, nEditAttr;	//rows, columns
	ULONG nNavItem,	nEditItem;	//rows, columns
	UINT crdigit;

	LPCSTR Region, Province, Municipality, Barangay, 
		regnID, provID, munID,
		m_sLName0, m_sLName1;

	LPCSTR** sFill;

	int selIndex;	//index of selected

	CMenu menu;   
	CMenu* pMenu; 

	BOOL m_hasChanged;

	virtual BOOL OnInitDialog();
	afx_msg void OnNMDblclkListlevelnav(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnNMClickListlevelnav(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListlevel(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeleditListlevel(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedEdit();
	afx_msg void OnNMRclickListlevel(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnBeginlabeleditListlevel(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLevelEdit();
	afx_msg void OnLevelDelete();
	afx_msg void OnNMClickListlevelnav(NMHDR *pNMHDR, LRESULT *pResult);
};
