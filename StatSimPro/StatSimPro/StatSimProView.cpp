// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// StatSimProView.cpp : implementation of the CStatSimProView class
//

#include "stdafx.h"
#include "StatSimPro.h"

#include "StatSimProDoc.h"
#include "StatSimProView.h"

extern CStatSimProView *g_pSSHTMLView;
extern CStatSimConn* pGlobalConn;
extern CString userName, userPwd, sHost, sPort, sDB;
extern long hpq_id;

extern bool isNewReport;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CStatSimProView

IMPLEMENT_DYNCREATE(CStatSimProView, CHtmlView)

BEGIN_MESSAGE_MAP(CStatSimProView, CHtmlView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CHtmlView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CHtmlView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CHtmlView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CStatSimProView construction/destruction

CStatSimProView::CStatSimProView()
{
	// TODO: add construction code here
	g_pSSHTMLView = this;

}

CStatSimProView::~CStatSimProView()
{
}

BOOL CStatSimProView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CHtmlView::PreCreateWindow(cs);
}

void CStatSimProView::OnInitialUpdate()
{
	CHtmlView::OnInitialUpdate();
	
	//bool isMDC = true; //temporary MDC qualifier

	//try cbms portal
	if (!isNewReport) {
		Navigate2(_T("http://cbms.dlsu.edu.ph/portal/"),NULL,NULL);
		return;
		//this->SetWindowTextW(_T("CBMS Portal"));
	}

	CreateDirectory(_T("C:\\CBMSDatabase\\System\\Reports"), NULL);
	CreateDirectory(_T("C:\\CBMSDatabase\\System\\Reports\\Temp"), NULL);
	m_sNavPath = _T("C:\\CBMSDatabase\\System\\Reports\\Temp\\StatSimRpt.htm");

	CString sSQL, sFld = _T("freq");
	CStatSimRS* pRS = 0;
	int n=7;
	int* nrec = new int[n];
	CStringArray sFreqArr, sFreqNull,
		sFrNullVarsHH, sFrNullVarsMem;

	sFreqArr.SetSize(n);
	sFreqNull.SetSize(n);
	sFrNullVarsHH.SetSize(2);
	sFrNullVarsMem.SetSize(2);

	sSQL = _T("SELECT count(*) AS `freq` FROM `regn`;");
	pRS = new CStatSimRS(pGlobalConn, sSQL);
	sFreqArr[0] = pRS->SQLFldValue(ConstChar(sFld));
	sFreqNull[0] = "0";
	delete pRS; pRS = 0;

	sSQL = _T("SELECT count(*) AS `freq` FROM `prov`;");
	pRS = new CStatSimRS(pGlobalConn, sSQL);
	sFreqArr[1] = pRS->SQLFldValue(ConstChar(sFld));
	sFreqNull[1] = "0";
	delete pRS; pRS = 0;

	sSQL = _T("SELECT count(*) AS `freq` FROM `mun`;");
	pRS = new CStatSimRS(pGlobalConn, sSQL);
	sFreqArr[2] = pRS->SQLFldValue(ConstChar(sFld));
	sFreqNull[2] = "0";
	delete pRS; pRS = 0;

	sSQL = _T("SELECT count(*) AS `freq` FROM `brgy`;");
	pRS = new CStatSimRS(pGlobalConn, sSQL);
	sFreqArr[3] = pRS->SQLFldValue(ConstChar(sFld));
	sFreqNull[3] = "0";
	delete pRS; pRS = 0;

	if(TableExists(_T("purok"), pGlobalConn)) {
		sSQL = _T("SELECT count(*) AS `freq` FROM `purok`;");
		pRS = new CStatSimRS(pGlobalConn, sSQL);
		sFreqArr[4] = pRS->SQLFldValue(ConstChar(sFld));
		delete pRS; pRS = 0;

		sSQL = _T("SELECT count(*) AS `freq` FROM `purok` where isnull(`purok`)=TRUE;");
		pRS = new CStatSimRS(pGlobalConn, sSQL);
		sFreqNull[4] = pRS->SQLFldValue(ConstChar(sFld));
		delete pRS; pRS = 0;
	}
	else {
		sFreqArr[4] = "Empty";
		sFreqNull[4] = "N/A";
	}

	if(TableExists(_T("hpq_hh"), pGlobalConn) ) { //&& !isMDC) {
		sSQL = _T("SELECT count(*) AS `freq` FROM `hpq_hh`;");
		pRS = new CStatSimRS(pGlobalConn, sSQL);
		sFreqArr[5] = pRS->SQLFldValue(ConstChar(sFld));
		delete pRS; pRS = 0;

		sSQL = _T("SELECT count(*) AS `freq` FROM `hpq_hh` where isnull(`hcn`)=TRUE;");
		pRS = new CStatSimRS(pGlobalConn, sSQL);
		sFreqNull[5] = pRS->SQLFldValue(ConstChar(sFld));
		delete pRS; pRS = 0;

		if (FieldExists(_T("int_yy"),  _T("hpq_hh"), pGlobalConn) ) {
			sSQL = _T("SELECT count(*) AS `freq` FROM `hpq_hh` where isnull(`int_yy`)=TRUE;");
			pRS = new CStatSimRS(pGlobalConn, sSQL);
			sFrNullVarsHH[0] = pRS->SQLFldValue(ConstChar(sFld));
			delete pRS; pRS = 0;
		}

		if (FieldExists(_T("int_mm"),  _T("hpq_hh"), pGlobalConn) ) {
			sSQL = _T("SELECT count(*) AS `freq` FROM `hpq_hh` where isnull(`int_mm`)=TRUE;");
			pRS = new CStatSimRS(pGlobalConn, sSQL);
			sFrNullVarsHH[1] = pRS->SQLFldValue(ConstChar(sFld));
			delete pRS; pRS = 0;
		}
	}
	else {
		sFreqArr[5] = "Empty";
		sFreqNull[5] = "N/A";

		sFrNullVarsHH[0] = "N/A";
		sFrNullVarsHH[1] = "N/A";
	}

	if(TableExists(_T("hpq_mem"), pGlobalConn) ) { //&& !isMDC){
		sSQL = _T("SELECT count(*) AS `freq` FROM `hpq_mem`;");
		pRS = new CStatSimRS(pGlobalConn, sSQL);
		sFreqArr[6] = pRS->SQLFldValue(ConstChar(sFld));
		delete pRS; pRS = 0;

		sSQL = _T("SELECT count(*) AS `freq` FROM `hpq_mem` where isnull(`memno`)=TRUE;");
		pRS = new CStatSimRS(pGlobalConn, sSQL);
		sFreqNull[6] = pRS->SQLFldValue(ConstChar(sFld));
		delete pRS; pRS = 0;

		sSQL = _T("SELECT count(*) AS `freq` FROM `hpq_mem` where isnull(`age_yr`)=TRUE;");
		pRS = new CStatSimRS(pGlobalConn, sSQL);
		sFrNullVarsMem[0] = pRS->SQLFldValue(ConstChar(sFld));
		delete pRS; pRS = 0;

		sSQL = _T("SELECT count(*) AS `freq` FROM `hpq_mem` where isnull(`sex`)=TRUE;");
		pRS = new CStatSimRS(pGlobalConn, sSQL);
		sFrNullVarsMem[1] = pRS->SQLFldValue(ConstChar(sFld));
		delete pRS; pRS = 0;

	}
	else {
		sFreqArr[6] = "Empty";
		sFreqNull[6] = "N/A";

		sFrNullVarsMem[0] = "N/A";
		sFrNullVarsMem[1] = "N/A";

	}

	FILE * pFile = _tfopen(m_sNavPath, _T("w"));

	CString sHtmlTag, sQnrID;
	sQnrID.Format(_T("%d"), hpq_id);

	sHtmlTag = _T("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=ISO-8859-1\"> \n\
		<title>Welcome StatSimPro 5</title>\n\
		</head>\n\
		<body>\n\
		<hr /><hr />\n\
		<p><font face=\"Calibri\"><b><font size=\"5\" color=\"#990000\">CBMS StatSimPro 6.0<br>\n\
		</font><font size=\"4\" color=\"#000080\">\n\
		<img border=\"0\" src=\".\\images\\logo.jpg\" align=\"left\" width=\"86\" height=\"110\">\n\
		</font></b></font></p>\n\
		<p><font face=\"Calibri\"><b><font size=\"4\" color=\"#000080\">The PEP-Asia CBMS Network <br>\n\
		</font></b><i>Developed by the PEP-Asia CBMS Network Team \n\
		with the financial support \n\
		from the government of Canada provided \n\
		through the International Development Research Centre (IDRC) \n\
		and the Canadian International Development Agency (CIDA).<br>\n\
		</i><br>&nbsp;</font></p>\n\
		<br /><hr />\n\
		<p><font face=\"Calibri\"><b><font size=\"4\" color=\"#000080\">") + sDB + _T(" Database Content Summary <br>\n\
		</font></b>\n\
		<i><table>\n\
		<tr>\n\
		<td width='100'></td><td width='200'>HPQ VN: ") + sQnrID +_T("\n\
		</td></tr>\n\
		<tr>\n\
		<td width='100'></td><td width='200'>Region(s): ") + sFreqArr[0] +_T("\n\
		</td></tr>\n\
		<tr>\n\
		<td width='100'></td><td width='200'>Province(s): ") + sFreqArr[1] +_T("\n\
		</td></tr> \n\
		<tr>\n\
		<td width='100'></td><td width='200'>Municipality(ies): ") + sFreqArr[2] +_T("\n\
		</td></tr>\n\
		<tr>\n\
		<td width='100'></td><td width='200'>Barangay(s): ") + sFreqArr[3] +_T("\n\
		</td></tr>\n\
		<tr>\n\
		<td width='100'></td><td width='200'>Purok(s): ") + sFreqArr[4] +_T("</td>\n\
		<td width='10'></td><td width='200'>Missing ID(s): ") + sFreqNull[4] +_T("\n\
		</td></tr>\n\
		<tr>\n\
		<td></td><td colspan=3><hr/></td> \n\
		</tr>\n\
		<tr>\n\
		<td width='100'></td><td width='200'>Household(s): ") + sFreqArr[5] +_T("</td>\n\
		<td width='10'></td><td width='200'>Missing ID(s): ") + sFreqNull[5] +_T("\n\
		</td></tr>\n\
		<tr>\n\
		<td width='100'></td><td width='200'></td>\n\
		<td width='10'></td><td width='200'>Missing Interview Year / Month: ") + sFrNullVarsHH[0] +_T(" / ") + sFrNullVarsHH[1] + _T("\n\
		</td></tr>\n\
		<tr>\n\
		<td></td><td colspan=3><hr/></td> \n\
		</td></tr>\n\
		<tr>\n\
		<td width='100'></td><td width='200'>Member(s): ") + sFreqArr[6] +_T("</td>\n\
		<td width='10'></td><td width='200'>Missing ID(s): ") + sFreqNull[6] +_T("\n\
		</td></tr>\n\
		<tr>\n\
		<td width='100'></td><td width='200'></td>\n\
		<td width='10'></td><td width='200'>Missing Age / Sex: ") + sFrNullVarsMem[0] +_T(" / ") + sFrNullVarsMem[1] + _T("\n\
		</td></tr>\n\
		</table>\n\
		<br /> <br>\n\
		</i><br>&nbsp;</font></p>\n\
		</body></html>");


	_ftprintf(pFile, (sHtmlTag));
	fclose(pFile);

	Navigate2(m_sNavPath, NULL, NULL);

}


// CStatSimProView printing

BOOL CStatSimProView::OnPreparePrinting(CPrintInfo* pInfo)
{
	pInfo->SetMaxPage(2);   // the document is two pages long:
							// the first page is the title page
							// the second is the drawing
	BOOL bRet = DoPreparePrinting(pInfo);	// default preparation
	pInfo->m_nNumPreviewPages = 2;  // Preview 2 pages at a time
	// Set this value after calling DoPreparePrinting to override
	// value read from .INI file
	return bRet;
}
void CStatSimProView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}


void CStatSimProView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CStatSimProView::OnContextMenu(CWnd* pWnd, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}

void CStatSimProView::OnDraw(CDC* /*pDC*/)
{
	CStatSimProDoc* pDoc = (CStatSimProDoc*) GetDocument();
	ASSERT_VALID(pDoc);
}

// CStatSimProView diagnostics

#ifdef _DEBUG
void CStatSimProView::AssertValid() const
{
	CHtmlView::AssertValid();
}

void CStatSimProView::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}

CStatSimProDoc* CStatSimProView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CStatSimProDoc)));
	return (CStatSimProDoc*)m_pDocument;
}
#endif //_DEBUG


// CStatSimProView message handlers
