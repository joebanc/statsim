#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "StatSimList.h"
#include "StatSimConn.h"

// progress handlers
class _hprogress
{
public:
	_hprogress();
	virtual ~_hprogress();

	enum proctype { IMPORT = 0 };
	vector<LPCSTR> m_sObj;
	vector<LPCSTR> m_sDesc;
	proctype m_proctype;

};


// CProgressExDlg dialog

class CProgressExDlg : public CDialog
{
	DECLARE_DYNAMIC(CProgressExDlg)

public:
	CProgressExDlg(_hprogress *phprogress = NULL,
		CWnd* pParent = NULL);   // standard constructor
	virtual ~CProgressExDlg();

// Dialog Data
	enum { IDD = IDD_PROGRESSEXDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	// the object database
	CStatSimConn *m_pDB;
	CFont m_Font;
	daedata *m_pDta;
	daestream *m_pSSS;

	int curr_i;

public:

	_hprogress *m_phprogress;
    CView* m_pView;

	CButton m_Cancel;
	CButton m_Ok;
	// list progress control
	CStatSimList *m_ProgressList;
	// frame of the list control
	CStatic m_FrameList;

	// overall progressbar
	CProgressCtrl m_ProgressBar;

	BOOL m_bOK;


// functions
	void SetDB(CStatSimConn* pDB);
	void SetProgress(int bar, int pos);
	afx_msg void OnBnClickedOk();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCancel();

public:
	afx_msg void OnLvnItemchangedProgresslist(NMHDR *pNMHDR, LRESULT *pResult);
};
