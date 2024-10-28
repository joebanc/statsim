#pragma once

//class CProgressExDlg{};

#ifndef STATSIMTREE
	#include "StatSimTree.h"
#endif

#ifndef STATSIMLIST
	#include "StatSimList.h"
#endif

class CProgressExDlg;
class CStatSimElt;

// CRemoveAutoSelDlg dialog

class CRemoveAutoSelDlg : public CDialog
{
	DECLARE_DYNAMIC(CRemoveAutoSelDlg)

public:
	CRemoveAutoSelDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRemoveAutoSelDlg();

// Dialog Data
	enum { IDD = IDD_REMOVEAUTOSEL };

	CButton	m_FrameSelect;
	CStatSimTree m_TreeLevel;
	CButton	m_OK;
	CButton	m_Cancel;
	CButton	m_ButtonDeselect;
	CButton	m_ButtonDeselectAll;
	CButton	m_ButtonSelect;
	CButton	m_ButtonSelectAll;
	CStatSimList	m_ListUnselect;
	CStatSimList	m_ListSelect;
	CButton	m_ButtonSpecPath;

	// view the current selection
	CButton m_View;
	// change the current selection
	CButton m_Change;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSpecifypath();
//	afx_msg void OnTvnSelchangedTreelevel(NMHDR *pNMHDR, LRESULT *pResult);


private:

	CProgressExDlg *m_pDlg;
	void FillUnSel();
	int AutoImport(CStatSimList* pSrcList);

	//void SwitchContents(CSimList* pFrList, CSimList* pToList = NULL);

	afx_msg void OnBnClickedSelect();
	afx_msg void OnBnClickedDeselect();
	afx_msg void OnBnClickedSelectall();
	afx_msg void OnBnClickedDeselectall();
	afx_msg void OnBnClickedOk();
	afx_msg void OnNMDblclkListunsel(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListsel(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();

	// element handler
	CStatSimElt *m_pElt;
	//number of rows, columns
	int m_nListI;
	long m_nListJ;
	//handler of column names;
	LPCTSTR* m_sListCols;

	afx_msg void OnBnClickedView();
	afx_msg void OnBnClickedChange();
	afx_msg void OnNMDblclkTreelevel(NMHDR *pNMHDR, LRESULT *pResult);
};
