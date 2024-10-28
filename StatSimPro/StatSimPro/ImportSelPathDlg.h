#pragma once

#include "Resource.h"

#ifndef STATSIMLIST
	#include "StatSimList.h"
#endif
//#include "SSArrayBuilder.h"



// CImportSelPathDlg dialog

class CImportSelPathDlg : public CDialog
{
	DECLARE_DYNAMIC(CImportSelPathDlg)

public:
	CImportSelPathDlg(long curr_qnrID, CWnd* pParent = NULL);   // standard constructor
	virtual ~CImportSelPathDlg();

// Dialog Data
	enum { IDD = IDD_IMPORTSELPATH };

	CButton	m_ButtonClear;
	CButton	m_Cancel;
	CButton	m_OK;
	CButton	m_ButtonRemove;
	CStatSimList	m_ListPath;
	CButton	m_ButtonAutoSelect;
	CButton	m_ButtonAdd;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedAutosel();
	afx_msg void OnBnClickedAdd();
	//the trick
	afx_msg void OnTimer(UINT nIDEvent);

private:
//functions
	void GetSelPaths(CFileDialog* pFileDlg);
//attributes

	long m_qnrID;
	USHORT nPaths, nAttr;	//rows, columns

	std::vector<std::vector<CString>> m_itemArray;	//LPCSTR** mItemArray;	//array of items
	
	LPCSTR* sCols;		//array of column labels;
	int* width;			//array of widths

	afx_msg void OnBnClickedRemove();
	afx_msg void OnBnClickedClear();
	virtual BOOL OnInitDialog();
};
