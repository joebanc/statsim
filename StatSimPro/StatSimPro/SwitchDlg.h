#pragma once
//#include "afxwin.h"
#include "ImportSelPathDlg.h"
#include "ImportAutoSelDlg.h"


enum SELOPT { AUTOSEL = 0, SPECPATH, SELCAN };
enum SWITCHTYPE { IMPORTMETHOD = 0, PROCMETHOD };

// CSwitchDlg dialog

class CSwitchDlg : public CDialog
{
	DECLARE_DYNAMIC(CSwitchDlg)

public:
	CSwitchDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSwitchDlg();

// Dialog Data
	enum { IDD = IDD_SWITCH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_Cancel;
	CButton m_Ok;
	// Autoselection of levels
	CButton m_Opt0;
	// Specify path
	CButton m_Opt1;
	CButton m_OptCan;

	SELOPT currSelOpt;
	CImportAutoSelDlg *pImportAutoSelDlg;
	CImportSelPathDlg *pImportSelPathDlg;
	CString m_sCanPath;

public:
	afx_msg void OnBnClickedSelcan();
	afx_msg void OnBnClickedAutoselectopt();
	afx_msg void OnBnClickedSpecpathopt();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	void ImportCan();
	// Combo box of questionnaires
	CComboBox m_ComboQnr;
	afx_msg void OnCbnSelchangeCombolevel();
private:
	
	
};

std::string getdelimcol(ifstream & in, char delim = ',', char enc = '"', char newline = '\n', char defenc = '\'', bool sqlform = true );
int col_sql_form(daestring& scol, char defenc = '\'');
int samp();
class streamcol
{
public:
	streamcol();   // standard constructor
	virtual ~streamcol();
};