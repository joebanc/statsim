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

// CImportAutoSelDlg dialog

class CImportAutoSelDlg : public CDialog
{
	DECLARE_DYNAMIC(CImportAutoSelDlg)

public:
	CImportAutoSelDlg(long curr_qnrID, CWnd* pParent = NULL);   // standard constructor
	virtual ~CImportAutoSelDlg();

// Dialog Data
	enum { IDD = IDD_IMPORTAUTOSEL };

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
	LPCSTR* m_sListCols;

	long m_qnrID;

	afx_msg void OnBnClickedView();
	afx_msg void OnBnClickedChange();
	afx_msg void OnNMDblclkTreelevel(NMHDR *pNMHDR, LRESULT *pResult);
};
