#pragma once
#include "StatSimList.h"


// CProcSelDlg dialog

class CProcSelDlg : public CDialog
{
	DECLARE_DYNAMIC(CProcSelDlg)

public:
	CProcSelDlg(int elt_lim = HH, CString sCaption = _T("StatSim: Select levels to process"),
		CWnd* pParent = NULL);   // standard constructor
	virtual ~CProcSelDlg();

//attributes
	int* elementIDArray;
	int nElement;
	BOOL wHH;

// Dialog Data
	enum { IDD = IDD_PROCSELDLG };
	CButton	m_OK;
	CStatSimList	m_ListUnselect;
	CStatSimList	m_ListSelect;
	CButton	m_ButtonSelectAll;
	CButton	m_ButtonSelect;
	CButton	m_ButtonDeselectAll;
	CButton	m_ButtonDeselect;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSelect();
	afx_msg void OnBnClickedDeselect();
	afx_msg void OnBnClickedSelectall();
	afx_msg void OnBnClickedDeselectall();
//	afx_msg void OnLvnItemchangedListunsel(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListunsel(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListsel(NMHDR *pNMHDR, LRESULT *pResult);

private:
	int m_elt_lim;
	CString m_sCaption;
};

