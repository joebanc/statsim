// GetVersionDlg.h : Header-Datei
//

#if !defined(AFX_GETVERSIONDLG_H__3C0D3378_6407_41E3_A607_AB6B940DFC9A__INCLUDED_)
#define AFX_GETVERSIONDLG_H__3C0D3378_6407_41E3_A607_AB6B940DFC9A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CGetVersionDlg Dialogfeld

class CGetVersionDlg : public CDialog
{
// Konstruktion
public:
	CGetVersionDlg(CWnd* pParent = NULL);	// Standard-Konstruktor

// Dialogfelddaten
	//{{AFX_DATA(CGetVersionDlg)
	enum { IDD = IDD_GETVERSION_DIALOG };
		// HINWEIS: der Klassenassistent fügt an dieser Stelle Datenelemente (Members) ein
	//}}AFX_DATA

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CGetVersionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	HICON m_hIcon;

	// Generierte Message-Map-Funktionen
	//{{AFX_MSG(CGetVersionDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_GETVERSIONDLG_H__3C0D3378_6407_41E3_A607_AB6B940DFC9A__INCLUDED_)
