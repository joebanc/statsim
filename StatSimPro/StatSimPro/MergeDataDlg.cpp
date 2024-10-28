// MergeDataDlg.cpp : implementation file
//

#include "stdafx.h"
#include "StatSimPro.h"
#include "MergeDataDlg.h"
#include "StatSimHTML.h"
#include "StatSimProView.h"

extern CStatSimConn* pGlobalConn;

extern CMFCRibbonStatusBarPane  *g_wndStatusBarPane;//I have transferred this from main to global to be accessible
extern CProgressCtrl  m_ProgStatBar;	//I have transferred this from main to global to be accessible

extern CStatSimProView *g_pSSHTMLView;

extern short country_dig = 0, 
	regn_dig = 2, 
	prov_dig = 4, 
	mun_dig = 6,
	brgy_dig = 9,
	purok_dig = 11,
	hh_dig = 17,
	mem_dig = 19,
	id_dig = 9;

// CMergeDataDlg dialog

IMPLEMENT_DYNAMIC(CMergeDataDlg, CDialog)

CMergeDataDlg::CMergeDataDlg(LPCSTR sUsingPath, CWnd* pParent /*=NULL*/)
	: CDialog(CMergeDataDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMergeDataDlg)
	//}}AFX_DATA_INIT
	m_sUsingPath = sUsingPath;
	
	CString sFile;
	int bsPos, len, dotPos;

	//assign file name and path
	sFile = sUsingPath;
	
	//assign folder path
	bsPos = sFile.ReverseFind('\\');
	len = sFile.GetLength();
	m_sFolderPath = ConstChar( sFile.Left(bsPos) );


	//assign file name
	sFile = sFile.Right(len-bsPos-1);
	m_sFileName = ConstChar(sFile);

	//assign file title
	dotPos = sFile.ReverseFind('.');
	m_sFileTitle = ConstChar( sFile.Left(dotPos) );

	m_nKeys = 0;



}


void CMergeDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMergeDataDlg)
	DDX_Control(pDX, IDC_LIST_MERGE, m_ListMerge);
	DDX_Control(pDX, IDC_LIST_USING, m_ListUsing);
	DDX_Control(pDX, IDC_LIST_MASTER, m_ListMaster);
	DDX_Control(pDX, IDC_OPT_SELECT, m_OptSelect);
	DDX_Control(pDX, IDC_OPT_KEY, m_OptKey);
	DDX_Control(pDX, IDC_COMBO_USING, m_ComboUsing);
	DDX_Control(pDX, IDC_COMBO_MASTER, m_ComboMaster);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMergeDataDlg, CDialog)
	//{{AFX_MSG_MAP(CMergeDataDlg)
	ON_BN_CLICKED(IDC_OPT_KEY, OnOptKey)
	ON_BN_CLICKED(IDC_OPT_SELECT, OnOptSelect)
	ON_BN_CLICKED(IDCLOSE, OnClose)
	ON_CBN_SELCHANGE(IDC_COMBO_MASTER, OnSelchangeComboMaster)
	ON_CBN_SELCHANGE(IDC_COMBO_USING, OnSelchangeComboUsing)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMergeDataDlg message handlers

BOOL CMergeDataDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	ResizeWindow(IDOK, this, 1);

	CStatSimRS *pMasterRS, *pUsingRS, *pRS;
	
	CString sCaption, sSQL, sConn;
	
	sCaption.Format(_T("CBMS StatSim - Match data with %s"), (CString) m_sFileTitle);
	
	SetWindowText(sCaption);

	// build using connection
	sConn.Format(_T("Driver={Microsoft Access Driver (*.mdb)}; Dbq=%s; DefaultDir=%s;"), (CString) m_sFileName, (CString) m_sFolderPath);
	m_pUsingDB = new CStatSimConn((sConn));

	//////////////////////////////////////////////////

	//populate using combo box
	sSQL.Format(_T("SELECT * FROM `BDFeature_Types` WHERE `ManyToOne`=-1 ORDER BY `ID`;"));
	pUsingRS = new CStatSimRS( m_pUsingDB->GetODBCdb(), sSQL);
	pUsingRS->PutValuesTo(&m_ComboUsing, _MBCS("description"), _MBCS("ID"), _MBCS("description"));

	//populate master combo box
	sSQL.Format(_T("SELECT * FROM `~hElement` ORDER BY `elementID`"));

	pMasterRS = new CStatSimRS( pGlobalConn, sSQL);
	pMasterRS->PutValuesTo(&m_ComboMaster, _MBCS("label"), _MBCS("elementID"), _MBCS("label"));
	
	m_OptKey.SetButtonStyle(BS_AUTORADIOBUTTON);
	m_OptSelect.SetButtonStyle(BS_AUTORADIOBUTTON);

	//default is key option
	m_OptKey.SetCheck(BST_CHECKED);
	OnOptKey();

	m_ComboMaster.SetCurSel(GetItemIndex(&m_ComboMaster, HH_COREIND));
	m_ComboUsing.SetCurSel(GetItemIndex(&m_ComboUsing, HOUSEHOLD));

	//return true;
	OnSelchangeComboUsing();
	OnSelchangeComboMaster();
	
	TCHAR *sTxt;
	sTxt = new TCHAR[100];
	CString sTxt2;

	m_ComboUsing.GetWindowText(sTxt, 100);

	sTxt2 = sTxt;
	sTxt2.TrimLeft();
	sTxt2.TrimRight();
	
	if (sTxt2!="Household") {
		m_ComboUsing.EnableWindow();
	}

/*	//array of data types;
	int i, j;
	sSQL.Format(_T("SELECT * FROM `BDDictionary`;");
	pRS = new CStatSimRS( m_pUsingDB->GetODBCdb(), sSQL);
	if ( pRS->GetRecordCount() > 0 )
		pRS->MoveFirst();

	m_nDataTypes = pRS->GetRecordCount();
	
	VERIFY( m_pMapDataTypes = (LPCSTR **)malloc(pRS->GetRecordCount() * sizeof(LPCSTR *)) );
	for (i=0; i<pRS->GetRecordCount(); i++) {
		if (AfxCheckMemory()) {
			
			VERIFY( m_pMapDataTypes[i] = (LPCSTR *)malloc(pRS->GetFieldCount() * sizeof(LPCSTR)) );

		}
	}
	
	for (i=0; i<pRS->GetRecordCount(); i++) {
		for (j=0; j<pRS->GetFieldCount(); j++) {

			m_pMapDataTypes[i][j] = ConstChar(pRS->SQLFldValue(j));

		}
		pRS->MoveNext();
	}
	delete pRS; pRS = 0;

*/
	m_nDataTypes = 9;
	int i, j, nCols = 2;
	VERIFY( m_pMapDataTypes = (LPCSTR **)malloc(m_nDataTypes * sizeof(LPCSTR *)) );
	for (i=0; i<m_nDataTypes; i++) {
		if (AfxCheckMemory()) {
			
			VERIFY( m_pMapDataTypes[i] = (LPCSTR *)malloc(nCols * sizeof(LPCSTR)) );

		}
	}
	
	m_pMapDataTypes[0][0] = _MBCS("1");	m_pMapDataTypes[0][1] = _MBCS("Float");
	m_pMapDataTypes[1][0] = _MBCS("2");	m_pMapDataTypes[1][1] = _MBCS("String");
	m_pMapDataTypes[2][0] = _MBCS("3");	m_pMapDataTypes[2][1] = _MBCS("Coordinate");
	m_pMapDataTypes[3][0] = _MBCS("4");	m_pMapDataTypes[3][1] = _MBCS("Map Lines");
	m_pMapDataTypes[5][0] = _MBCS("6");	m_pMapDataTypes[5][1] = _MBCS("Picture");
	m_pMapDataTypes[6][0] = _MBCS("7");	m_pMapDataTypes[6][1] = _MBCS("Link");
	m_pMapDataTypes[7][0] = _MBCS("8");	m_pMapDataTypes[7][1] = _MBCS("Boolean");
	m_pMapDataTypes[8][0] = _MBCS("9");	m_pMapDataTypes[8][1] = _MBCS("Date");


	delete pUsingRS; pUsingRS = 0;
	delete pMasterRS; pMasterRS = 0;

	//configure merge list
	//m_ListMerge.InsertColumn(0, _T("Master [*]", LVCFMT_LEFT, 75);
	//m_ListMerge.InsertColumn(1, _T("Using [+]", LVCFMT_LEFT, 75);
	//m_ListMerge.InsertColumn(2, _T("Type", LVCFMT_LEFT, 75);
	//m_ListMerge.InsertColumn(3, _T("Master Source", LVCFMT_LEFT, 75);
	//m_ListMerge.InsertColumn(4, _T("Using Source", LVCFMT_LEFT, 75);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMergeDataDlg::OnOptKey() 
{
	m_IsKey = true;
	
}

void CMergeDataDlg::OnOptSelect() 
{
	m_IsKey = FALSE;
	
}

void CMergeDataDlg::OnClose() 
{
	CDialog::OnCancel();	
	
}

void CMergeDataDlg::OnSelchangeComboMaster() 
{
	m_ListMaster.ResetContent();

	CString sTable, sSQL;
	DWORD id;
	int index, nCols, nItems, i, j, k;
	LPCSTR *sCols, **sItems;

	CStatSimRS *pRS, *pToolRS;

	//get the id of the current table
	index = m_ComboMaster.GetCurSel();
	id = m_ComboMaster.GetItemData(index);

	sSQL.Format(_T("SELECT * FROM `~hElement` WHERE `elementID`=%d;"), id);
	pRS = new CStatSimRS( pGlobalConn, sSQL);

	if ( pRS->GetRecordCount() > 0 )
		pRS->MoveFirst();

	//get the source table
	sTable = pRS->SQLFldValue(_MBCS("element"));
	sTable.MakeLower();
	sSQL = _T("SHOW TABLES;");
	pToolRS = new CStatSimRS( pGlobalConn, sSQL );

	if (!pToolRS->Exists(ConstChar(sTable), 0))
	{
		CString msg;
		msg.Format(_T("Table %s does not exist.  Reprocessing may solve this problem."), sTable);
		AfxMessageBox(msg);
		return;
	}

	sSQL.Format(_T("SELECT * FROM `%s` LIMIT 0;"), sTable);
	pRS = new CStatSimRS( pGlobalConn, sSQL);

	nCols = 2;
	nItems = pRS->GetFieldCount();
	
	//assign column values for list
	VERIFY( sCols = (LPCSTR *)malloc(nCols * sizeof(LPCSTR)) );
	sCols[0] = _MBCS("Column");	sCols[1] = _MBCS("Type");


	//assign item values [row][col]
	VERIFY( sItems = (LPCSTR **)malloc(nItems * sizeof(LPCSTR *)) );
	for (i=0; i<nItems; i++) {
		if (AfxCheckMemory()) {
			
			VERIFY( sItems[i] = (LPCSTR *)malloc(nCols * sizeof(LPCSTR)) );

		}
	}

	//column name
	for (i=0; i<nItems; i++) {

		m_sMasterSrc.SetSize(nItems);
		m_sMasterVars.SetSize(nItems);

		sItems[i][0] = pRS->GetFieldName(i);
		sItems[i][1] = ConstChar(pRS->SQLFldType(i));

		m_sMasterSrc[i] = sTable;
		m_sMasterVars[i] = sItems[i][0];

		m_ListMaster.AddString((CString) sItems[i][0]);
	}

	m_nMasterVars = nItems;
	m_nMasterSrc = nItems;

	//m_ListMasterVar.InsertBulk(sCols, nCols, sItems, nItems, TRUE, EMPTY);
	
}

void CMergeDataDlg::OnSelchangeComboUsing() 
{
	m_ListUsing.ResetContent();

	CString sTable, sSQL, sItem;
	DWORD id;
	int index, nCols, nItems, sctr, vctr, i, j, k;
	LPCSTR *sCols, **sItems;

	CStatSimRS *pRS=0, *pColRS=0;

	//get the id of the current table
	index = m_ComboUsing.GetCurSel();
	id = m_ComboUsing.GetItemData(index);
	
	//initialize and clean up
	nCols = 2;
	VERIFY( sCols = (LPCSTR *)malloc(nCols * sizeof(LPCSTR)) );
	sCols[0] = _MBCS("Column");	sCols[1] = _MBCS("Type");
	//m_ListUsingVar.CleanUp(sCols, nCols, TRUE, EMPTY);

	nItems=1;
	vctr=0;
	sctr=0;
	while (nItems>0) {

		m_sUsingSrc.SetSize(vctr+1);
		m_sUsingVars.SetSize(vctr+1);
		

		sSQL.Format(_T("SELECT * FROM `BDFeature_Types` WHERE `ID`=%d;"), id);
		pRS = new CStatSimRS( m_pUsingDB->GetODBCdb(), sSQL);
		nItems = pRS->GetRecordCount();

		if ( nItems > 0 ) {
			pRS->MoveFirst();
			
			//get the source table and add to the control
			sTable = pRS->SQLFldValue(_MBCS("internal"));
			//m_ListUsingVar.InsertItem(ctr, sTable);
			//m_ListUsingVar.SetItemText(ctr, 1, _T("Text");
			
			m_ListUsing.AddString( (sTable + _T(" [name]")) );
			m_sUsingSrc[vctr] = sTable;
			m_sUsingVars[vctr] = _T("Description");
			m_sUsingSrc[vctr].TrimLeft(); m_sUsingSrc[vctr].TrimRight();
			m_sUsingVars[vctr].TrimLeft(); m_sUsingVars[vctr].TrimRight();
			
		
			//increment
			vctr++;
			sctr++;

		}
		//columns
		sSQL.Format(_T("SELECT * FROM `BDFtype_Attr` WHERE `Feature_Type_ID`=%d;"), id);
		pColRS = new CStatSimRS( m_pUsingDB->GetODBCdb(), sSQL);
		nItems = pColRS->GetRecordCount();

		if ( nItems > 0 ) 
			pColRS->MoveFirst();

		for (i=0; i<nItems; i++) {
			//sctr++;
			//vctr++;
			
			m_sUsingSrc.SetSize(vctr+1);
			m_sUsingVars.SetSize(vctr+1);

			sItem = pColRS->SQLFldValue(_MBCS("Col_Name"));
			//m_ListUsingVar.InsertItem(ctr, sItem);
			int id = _ttoi( ( pColRS->SQLFldValue(_MBCS("Data_Type_ID")) ) );
			//m_ListUsingVar.SetItemText( ctr, 1, GetsDataType(id) );

			m_ListUsing.AddString( (sItem) );

			//m_sUsingSrc[vctr] = sTable;
			//m_sUsingVars[vctr] = sItem;
			//m_sUsingSrc[vctr].TrimLeft(); m_sUsingSrc[vctr].TrimRight();
			//m_sUsingVars[vctr].TrimLeft(); m_sUsingVars[vctr].TrimRight();

			pColRS->MoveNext();

		}
		
		//pass the new id
		if ( nItems > 0 ) {
			id = _ttol( ( pRS->SQLFldValue(_MBCS("Parent_FType")) ) );
		}
				
		delete pRS; pRS = 0;
		delete pColRS; pColRS = 0;
	}

	m_nUsingVars = vctr;
	m_nUsingSrc = vctr;


}

LPCSTR CMergeDataDlg::GetsDataType(int id) 
{

	for (int i=0; i<m_nDataTypes; i++) {
		int currID = atoi(m_pMapDataTypes[i][DT_ID]);
		if (currID==id) {
			return m_pMapDataTypes[i][DT_DESC];
		}

	}


}

void CMergeDataDlg::OnAdd() 
{
	
	int idxm, idxu;
	DWORD dwm, dwu;
	CString sMasterKeys, sUsingKeys,
		sMasterVars, sUsingVars, sItem, sRemark;

	idxm = m_ListMaster.GetCurSel();
	idxu = m_ListUsing.GetCurSel();

	m_sSelMasterSrc.SetSize(m_nKeys);
	m_sSelUsingSrc.SetSize(m_nKeys);
	m_sSelMasterVars.SetSize(m_nKeys);
	m_sSelUsingVars.SetSize(m_nKeys);
	
	if (idxm != LB_ERR && idxu != LB_ERR)
	{
		
		if (m_IsKey) {
			m_ListMaster.GetText(idxm, sMasterKeys); 
			m_ListUsing.GetText(idxu, sUsingKeys); 
			sRemark = _T("KEY");
		}
		else {
			m_ListMaster.GetText(idxm, sMasterVars); 
			m_ListUsing.GetText(idxu, sUsingVars);  
			sRemark = _T("SELECT");
		}
		
		dwm = m_ListMaster.GetItemData(idxm);
		dwu = m_ListUsing.GetItemData(idxu);
		
		// Remove the items from the lists
		m_ListMaster.DeleteString(idxm);
		m_ListUsing.DeleteString(idxu);
		
		sItem = sMasterKeys + _T("[*] <<>>") + sUsingKeys + _T("[+]");
		m_ListMerge.AddString( (sItem) );

		//m_sSelMasterSrc[m_nKeys] = m_sMasterSrc[idxm];
		//m_sSelUsingSrc[m_nKeys] = m_sUsingSrc[idxu];
		//m_sSelMasterVars[m_nKeys] = m_sMasterVars[idxm];
		//m_sSelUsingVars[m_nKeys] = m_sUsingVars[idxu];

		//m_ListMerge.InsertItem(m_nKeys, sMasterKeys);
		//m_ListMerge.SetItemText(m_nKeys, 1, sUsingKeys);
		//m_ListMerge.SetItemText(m_nKeys, 2, sRemark);
		m_nKeys++;
  
   }   
}
void CMergeDataDlg::RefreshVars(LPCSTR sMasterVar, LPCSTR sUsingVar)
{

//	if (m_nKeys>0) {
//		m_nKeys++;
//	}

//	m_sMasterKeys.SetSize(m_nKeys);

}

void CMergeDataDlg::OnRemove() 
{

/*	int idx;
	DWORD dw;
	idx = m_ListMerge.GetCurSel();
	
	if (idx != LB_ERR)
	{
		m_ListMerge.DeleteItem(idx);
	}   
*/	
}

void CMergeDataDlg::OnOK() 
{

	CString sSQL, sTable, sTableU, sTableM, sP = _T(""), sMsg;
	CStatSimRS *pToolRS;
	int idxm, idxu, i, msgResult;

	idxm = m_ComboMaster.GetCurSel();
	idxu = m_ComboUsing.GetCurSel();

	// test if there are selected sources
	if (idxm==LB_ERR || idxu==LB_ERR) {
		AfxMessageBox(_T("Please select item from both sources."));
		return;
	}
	////////////////

	//test records of hh in NRDB
	sSQL.Format(_T("SELECT `internal` FROM `BDFeature_Types` WHERE `ID`=%d;"), m_ComboUsing.GetItemData(idxu));
	pToolRS = new CStatSimRS( m_pUsingDB->GetODBCdb(), sSQL);
	pToolRS->MoveFirst();
	sTable = pToolRS->SQLFldValue(_MBCS("internal"));
	delete pToolRS; pToolRS = 0;
	sSQL.Empty();

	// This method is set assuming household matching
	sSQL.Format(_T("SELECT * FROM `%s`;"), sTable);
	pToolRS  = new CStatSimRS( m_pUsingDB->GetODBCdb(), sSQL);
	if (pToolRS->GetRecordCount()<=0) {
		AfxMessageBox(_T("There are no digitized data to match!"));
		delete pToolRS; pToolRS = 0;
		delete m_pUsingDB; m_pUsingDB = 0;
		OnClose();
		return;
	}
	//////////////////////////////////////////////

	// test existence of hh_coreind
	sSQL.Format(_T("SELECT `element` FROM `~helement` WHERE `elementID`=%d;"), m_ComboMaster.GetItemData(idxm));
	pToolRS = new CStatSimRS(pGlobalConn, sSQL);
	pToolRS->MoveFirst();
	short col = 0;
    sTable = pToolRS->SQLFldValue( col );
	sTable.MakeLower();
	delete pToolRS;  pToolRS = 0;

	sSQL.Format(_T("SHOW TABLES;"));
    pToolRS = new CStatSimRS(pGlobalConn, sSQL);
	pToolRS->MoveFirst();
	if (!pToolRS->Exists(ConstChar(sTable), 0))
	{
		CString msg;
		msg.Format(_T("Table %s does not exist.  Reprocessing may solve this problem."), sTable);
		AfxMessageBox(msg);
		delete pToolRS; pToolRS = 0;
		OnClose();
		return;
	}
	////////////////

	BeginWaitCursor();
	g_wndStatusBarPane->SetText(_T("Performing matching..."));

	//create a directory first
	CreateDirectory(_T("C:\\CBMSDatabase\\System\\Output"), NULL);

	for (i=0; i<m_nUsingVars-1; i++) {
			sP = sP + _T("(");
	}

	//select households, barangays, etc. from nrdb
	sTable = m_sUsingSrc[0] + _T("_NRDB");
	
	m_sSQLFromU = _T("FROM ") + sP;
	m_sSQLSelectU = _T("SELECT");
	for (i=0; i<m_nUsingVars; i++) {
		if (m_nUsingVars>=2 && i>=1) {
			if (m_sUsingVars[i]=="Description") {
				m_sSQLSelectU = m_sSQLSelectU + _T(", ") + m_sUsingSrc[i] + _T(".") + m_sUsingVars[i] + _T(" AS ") + m_sUsingSrc[i];
			}
		}
		else {
			if (m_sUsingVars[i]=="Description") {
				m_sSQLSelectU = m_sSQLSelectU + _T(" ") + m_sUsingSrc[i] + _T(".") + m_sUsingVars[i] + _T(" AS ") + m_sUsingSrc[i];
			}
		}

		if (m_nUsingVars>=2 && i>=1) {
			if (m_sUsingVars[i]=="Description") {
				m_sSQLFromU = m_sSQLFromU + _T(" INNER JOIN ") + m_sUsingSrc[i] + _T(" ON ") + m_sUsingSrc[i-1] + _T(".Parent_Feature=") + m_sUsingSrc[i] + _T(".ID)") ;
			}
		}
		else {
			if (m_sUsingVars[i]=="Description") {
				m_sSQLFromU = m_sSQLFromU + _T(" ") + m_sUsingSrc[i];
			}
		}
	}

	sSQL = m_sSQLSelectU + _T(" ") + m_sSQLFromU;

	CStatSimRS *pRS;
	pRS = new CStatSimRS( m_pUsingDB->GetODBCdb(), sSQL );

	sTableU = sTable;
	sTableM = _T("hh_CoreInd");
	sSQL = _T("DROP TABLE IF EXISTS ") + sTable;
	pGlobalConn->ExecuteSQL(sSQL);
	TransferRecords(pRS, ConstChar(sTable), pGlobalConn);

	sSQL = _T("ALTER TABLE ") + sTable + _T(" ") +
			_T("MODIFY ") + m_sUsingSrc[0] + _T(" VARCHAR(9), ") +
			_T("MODIFY ") + m_sUsingSrc[1] + _T(" VARCHAR(100), ") +
			_T("MODIFY ") + m_sUsingSrc[2] + _T(" VARCHAR(100), ") +
			_T("MODIFY ") + m_sUsingSrc[3] + _T(" VARCHAR(100), ") +
			_T("MODIFY ") + m_sUsingSrc[4] + _T(" VARCHAR(100), ") +
			_T("ADD INDEX `hhIndex`(`") + m_sUsingSrc[0] + _T("`, `") + m_sUsingSrc[1] + _T("`, `") + m_sUsingSrc[2] + _T("`, `") + m_sUsingSrc[3] + _T("`, `") + m_sUsingSrc[4] + _T("`)");
	
	//FILE* p2File = fopen("C:\\mycmd.sql", "w");
	//fprintf(p2File, ConstChar(sSQL));
	//fclose(p2File);
	pGlobalConn->ExecuteSQL(sSQL);

	//count duplicates - encoded
	sSQL = _T("SELECT prov, mun, brgy, purok, `Household Instance`, \n\
			  COUNT(`Household Instance`) AS `Frequency` \n\
			  FROM (SELECT prov, mun, brgy, purok, hcn, COUNT(`hcn`) AS `Household Instance` \n\
			  FROM `hpq_hh` GROUP BY prov, mun, brgy, purok, hcn) AS `Instance_Temp` \n\
			  GROUP BY prov, mun, brgy, purok, `Household Instance`;");

	delete pRS; pRS = 0;
	pRS = new CStatSimRS(pGlobalConn, sSQL);

	//build report
	CStatSimHTML *pHTML;

	CreateDirectory(_T("C:\\CBMSDatabase\\System\\Reports"), NULL);
	CreateDirectory(_T("C:\\CBMSDatabase\\System\\Reports\\Temp"), NULL);
	
	LPCTSTR sTitle, sSubTitle;
	m_sRptPath = _T("C:\\CBMSDatabase\\System\\Reports\\Temp\\StatSimRpt.htm");
	sTitle = _T("CBMS StatSim Merge/Match Report");
	sSubTitle = _T("Checking Duplicates in Encoded Households");

	FILE* pFile = _tfopen(m_sRptPath, _T("w"));
	pHTML = new CStatSimHTML(pRS, pFile);	//generic or core
	pHTML->DrawGeneric(sTitle, sSubTitle);
	
	//list of duplicates - encoded
	sSQL = _T("SELECT * FROM (SELECT prov, mun, brgy, purok, hcn, COUNT(`hcn`) AS `Household Instance` FROM `hpq_hh` GROUP BY prov, mun, brgy, purok, hcn) AS Instance_Temp WHERE `Household Instance`>=2;");
	delete pRS; pRS = 0;
	pRS = new CStatSimRS(pGlobalConn, sSQL);
	
	if (pRS->GetRecordCount()>0) {
		pHTML->SetRS(pRS);
		sTitle = _T("CBMS StatSim Merge/Match Report");
		sSubTitle = _T("Households with more than one instance in encoded");
		pHTML->DrawGeneric(sTitle, sSubTitle);
	}

	CString sPreClause = _T("");
	for (i=1; i<m_nUsingVars; i++) {
		if (m_nUsingVars>=3 && i>=2) {
			if (m_sUsingVars[m_nUsingVars-i]=="Description") {
				sPreClause = sPreClause + _T(", ") + m_sUsingSrc[i];
			}
		}
		else {
			if (m_sUsingVars[i]=="Description") {
				sPreClause = sPreClause + _T(" ") + m_sUsingSrc[i];
			}
		}
	}
	//count duplicates - using
	sSQL = _T("SELECT ") + sPreClause + _T(", `") + m_sUsingSrc[0] + _T(" Instance`, COUNT(`") + m_sUsingSrc[0] + _T(" Instance`) AS `Frequency` FROM (SELECT *, COUNT(`") + m_sUsingSrc[0] + _T("`) AS `") + m_sUsingSrc[0] + _T(" Instance` FROM ") + sTable + _T(" GROUP BY ") + sPreClause + _T(", ") + m_sUsingSrc[0] + _T(") AS `Instance_Temp` GROUP BY ") + sPreClause + _T(", `") + m_sUsingSrc[0] + _T(" Instance`;");
	delete pRS; pRS = 0;
	pRS = new CStatSimRS(pGlobalConn, sSQL);
	
	pHTML->SetRS(pRS);
	sTitle = _T("CBMS StatSim Merge/Match Report");
	sSubTitle = _T("Checking Duplicates in CBMS-NRDB Households");
	pHTML->DrawGeneric(sTitle, sSubTitle);

	//list of duplicates - digitized/using
	sSQL = _T("SELECT * FROM (SELECT *, COUNT(`") + m_sUsingSrc[0] + _T("`) AS `") 
		+ m_sUsingSrc[0] + _T(" Instance` FROM ") + sTable + _T(" GROUP BY ") 
		+ sPreClause + _T(", ") + m_sUsingSrc[0] + _T(") AS Instance_Temp WHERE `") 
		+ m_sUsingSrc[0] + _T(" Instance`>=2;");
	delete pRS; pRS = 0;
	pRS = new CStatSimRS(pGlobalConn, sSQL);
	
	if (pRS->GetRecordCount()>0) {
		pHTML->SetRS(pRS);
		sTitle = _T("CBMS StatSim Merge/Match Report");
		sSubTitle = _T("Households with more than one instance in digitized");
		pHTML->DrawGeneric(sTitle, sSubTitle);
	}

	//modify hh core ind
	sSQL.Format(_T("ALTER TABLE `hh_CoreInd` MODIFY `hcn_NRDB` int(%d);"), hh_dig-purok_dig);
	pGlobalConn->ExecuteSQL(sSQL, FALSE);
	
	sSQL.Format( _T("ALTER TABLE `hh_CoreInd` MODIFY `hcn_NRDB` varchar(%d);"), hh_dig-purok_dig);
	pGlobalConn->ExecuteSQL(sSQL, FALSE);

	//////////////////////////////////////
	//matching results - temporary
	CString sSQLU, sSQLM, sSQLI;
/*	sSQLU = _T("SELECT ") + sTableU + _T(".Province, ") + sTableU + _T(".Municipality, ") + sTableU + _T(".Barangay, " 
		+ sTableU + _T(".Purok, ") + sTableU + _T(".Household, ") +	sTableM + _T(".Province, ") + sTableM + _T(".Municipality, " 
		+ sTableM + _T(".Barangay, ") + sTableM + _T(".PurokName, ") + sTableM + _T(".hcn_NRDB, -1 AS `~match` \n" 
		+ _T("FROM ") + sTableU + _T(" LEFT JOIN ") + sTableM + _T(" ON " 
		+ sTableU + _T(".Province=") + sTableM + _T(".Province AND " 
		+ sTableU + _T(".Municipality=") + sTableM + _T(".Municipality AND " 
		+ sTableU + _T(".Barangay=") + sTableM + _T(".Barangay AND " 
		+ sTableU + _T(".Purok=") + sTableM + _T(".PurokName AND "
		+ sTableU + _T(".Household=") + sTableM + _T(".hcn_NRDB "
		+ _T("WHERE ") + sTableM + _T(".hcn_NRDB IS NULL");
*/

	sSQLU = _T("SELECT ") + sTableU + _T(".") + m_sUsingSrc[4] + _T(", ") + sTableU + _T(".") + m_sUsingSrc[3] + _T(", ") + sTableU + _T(".") + m_sUsingSrc[2] + _T(", ") 
		+ sTableU + _T(".") + m_sUsingSrc[1] + _T(", ") + sTableU + _T(".") + m_sUsingSrc[0] + _T(", -1 AS `~match` \n") 
		+ _T("FROM ") + sTableU + _T(" LEFT JOIN ") + sTableM + _T(" ON ") 
		+ sTableU + _T(".") + m_sUsingSrc[4] + _T("=") + sTableM + _T(".Province AND ") 
		+ sTableU + _T(".") + m_sUsingSrc[3] + _T("=") + sTableM + _T(".Municipality AND ")
		+ sTableU + _T(".") + m_sUsingSrc[2] + _T("=") + sTableM + _T(".Barangay AND ") 
		+ sTableU + _T(".") + m_sUsingSrc[1] + _T("=") + sTableM + _T(".PurokName AND ")
		+ sTableU + _T(".") + m_sUsingSrc[0] + _T("=") + sTableM + _T(".hcn_NRDB ")
		+ _T("WHERE ") + sTableM + _T(".hcn_NRDB IS NULL");

/*	sSQLM = _T("SELECT ") + sTableM + _T(".Province, ") + sTableM + _T(".Municipality, ") + sTableM + _T(".Barangay, " 
		+ sTableM + _T(".PurokName, ") + sTableM + _T(".hcn_NRDB, ") +	sTableU + _T(".Province, ") + sTableU + _T(".Municipality, " 
		+ sTableU + _T(".Barangay, ") + sTableU + _T(".Purok, ") + sTableU + _T(".Household, 1 AS `~match` \n" 
		+ _T("FROM ") + sTableM + _T(" LEFT JOIN ") + sTableU + _T(" ON " 
		+ sTableM + _T(".Province=") + sTableU + _T(".Province AND " 
		+ sTableM + _T(".Municipality=") + sTableU + _T(".Municipality AND " 
		+ sTableM + _T(".Barangay=") + sTableU + _T(".Barangay AND " 
		+ sTableM + _T(".PurokName=") + sTableU + _T(".Purok AND "
		+ sTableM + _T(".hcn_NRDB=") + sTableU + _T(".Household "
		+ _T("WHERE ") + sTableU + _T(".Household IS NULL");
*/	
	sSQLM = _T("SELECT ") + sTableM + _T(".Province, ") + sTableM + _T(".Municipality, ") + sTableM + _T(".Barangay, ") 
		+ sTableM + _T(".PurokName, ") + sTableM + _T(".hcn_NRDB, 1 AS `~match` \n") 
		+ _T("FROM ") + sTableM + _T(" LEFT JOIN ") + sTableU + _T(" ON ") 
		+ sTableM + _T(".Province=") + sTableU + _T(".") + m_sUsingSrc[4] + _T(" AND ") 
		+ sTableM + _T(".Municipality=") + sTableU + _T(".") + m_sUsingSrc[3] + _T(" AND ") 
		+ sTableM + _T(".Barangay=") + sTableU + _T(".") + m_sUsingSrc[2] + _T(" AND ") 
		+ sTableM + _T(".PurokName=") + sTableU + _T(".") + m_sUsingSrc[1] + _T(" AND ")
		+ sTableM + _T(".hcn_NRDB=") + sTableU + _T(".") + m_sUsingSrc[0] + _T(" ")
		+ _T("WHERE ") + sTableU + _T(".") + m_sUsingSrc[0] + _T(" IS NULL");


/*	sSQLI = _T("SELECT ") + sTableM + _T(".Province, ") + sTableM + _T(".Municipality, ") + sTableM + _T(".Barangay, " 
		+ sTableM + _T(".PurokName, ") + sTableM + _T(".hcn_NRDB, ") +	sTableU + _T(".Province, ") + sTableU + _T(".Municipality, " 
		+ sTableU + _T(".Barangay, ") + sTableU + _T(".Purok, ") + sTableU + _T(".Household, 0 AS `~match` \n" 
		+ _T("FROM ") + sTableM + _T(" INNER JOIN ") + sTableU + _T(" ON " 
		+ sTableM + _T(".Province=") + sTableU + _T(".Province AND " 
		+ sTableM + _T(".Municipality=") + sTableU + _T(".Municipality AND " 
		+ sTableM + _T(".Barangay=") + sTableU + _T(".Barangay AND " 
		+ sTableM + _T(".PurokName=") + sTableU + _T(".Purok AND "
		+ sTableM + _T(".hcn_NRDB=") + sTableU + _T(".Household");
*/

	sSQLI = _T("SELECT ") + sTableM + _T(".Province, ") + sTableM + _T(".Municipality, ") + sTableM + _T(".Barangay, ") 
		+ sTableM + _T(".PurokName, ") + sTableM + _T(".hcn_NRDB, 0 AS `~match` \n") 
		+ _T("FROM ") + sTableM + _T(" INNER JOIN ") + sTableU + _T(" ON ") 
		+ sTableM + _T(".Province=") + sTableU + _T(".") + m_sUsingSrc[4] + _T(" AND ") 
		+ sTableM + _T(".Municipality=") + sTableU + _T(".") + m_sUsingSrc[3] + _T(" AND ") 
		+ sTableM + _T(".Barangay=") + sTableU + _T(".") + m_sUsingSrc[2] + _T(" AND ") 
		+ sTableM + _T(".PurokName=") + sTableU + _T(".") + m_sUsingSrc[1] + _T(" AND ")
		+ sTableM + _T(".hcn_NRDB=") + sTableU + _T(".") + m_sUsingSrc[0] + _T("");

	delete pRS; pRS = 0;
	sTable = sTableM + _T("_") + sTableU;
	sSQL = _T("DROP TABLE IF EXISTS ") + sTable;
	pGlobalConn->ExecuteSQL(sSQL);

	sSQL = _T("CREATE TABLE ") + sTable + _T(" ")
		+ sSQLM + _T(" UNION ") + sSQLU + _T(" UNION ") + sSQLI + _T(";");
	//sSQL = sSQLI + _T(" UNION ") + sSQLM + _T(" UNION ") + sSQLU + _T(";");
	pGlobalConn->ExecuteSQL(sSQL);

	//count matching
	sSQL = _T("SELECT `~match`, `Household Instance`, COUNT(`Household Instance`) AS `Frequency` FROM (SELECT *, COUNT(`~match`) AS `Household Instance` FROM ") + sTable + _T(" GROUP BY Province, Municipality, Barangay, PurokName, hcn_NRDB, `~match`) AS `Instance_Temp` GROUP BY `Household Instance`, `~match`;");
	
	//draw report
	delete pRS; pRS = 0;
	pRS = new CStatSimRS(pGlobalConn, sSQL);
	
	pHTML->SetRS(pRS);
	sTitle = _T("~match: -1=Found in digitized but not in encoded; 1=Found in encoded but not in digitized; 0=Matched");
	sSubTitle = _T("Household Instance: Number of occurence of household, Frequency: Instances of occurence");
	pHTML->DrawGeneric(sTitle, sSubTitle);

	//draw report on matching
	delete pRS; pRS = 0;
	sSQL.Format(_T("SELECT 'C:\\\\CBMSDatabase\\\\System\\\\Output\\\\%s.csv' AS `Open File`;"), sTable);
	pRS = new CStatSimRS(pGlobalConn, sSQL);
	pHTML->SetRS(pRS);
	sTitle = _T("~match: -1=Found in digitized but not in encoded; 1=Found in encoded but not in digitized; 0=Matched");
	sSubTitle = _T("Checks whether a household came from the encoded or digitized or both");
	pHTML->DrawGeneric(sTitle, sSubTitle);
	delete pRS; pRS = 0;

	//output match report
	//sSQL.Format(_T("SELECT * INTO OUTFILE 'C:\\CBMSDatabase\\System\\Output\\%s.csv' FIELDS TERMINATED BY ',' FROM `%s`;", sTable);
	//pGlobalConn->ExecuteSQL(sSQL);
	
	fclose(pFile);

	CString sPath, sVal;
	
	sPath.Format(_T("C:\\CBMSDatabase\\System\\Output\\%s.csv"), sTable);
	pFile = _tfopen( (sPath), _T("w"));

	sMsg = _T("Please close ") + sPath + _T(" first.  Do you want to continue?");

	//loop till the file is opened or the user decides to discontinue
	while (pFile==NULL) {
		msgResult = AfxMessageBox(sMsg, MB_YESNO);
		
		if (msgResult==IDYES) {
			pFile = _tfopen( (sPath), _T("w"));
		}

		else {
			return;
		}

	}

	sSQL.Format(_T("SELECT * FROM `%s`;"), sTable);	
	pRS = new CStatSimRS(pGlobalConn, sSQL);
	
	if (pRS->GetRecordCount()>0) {
		pRS->MoveFirst();
	}
	
	//field names first
	for (int j=0; j<pRS->GetFieldCount(); j++) {
		fprintf( pFile, pRS->GetFieldName(j) );
		fprintf( pFile, _MBCS(",") );
	}
	fprintf( pFile, "\n" );
	
	//then the records
	for (i=0; i<pRS->GetRecordCount(); i++) {			
		for (int j=0; j<pRS->GetFieldCount(); j++) {
			
			//handle commas
			sVal = pRS->SQLFldValue(j);
			sVal.Replace(_T(","), _T(";"));
			_ftprintf( pFile, ( sVal ) );
			_ftprintf( pFile, _T(",") );
		}
		_ftprintf( pFile, _T("\n") );
		pRS->MoveNext();
	}

	delete pRS; pRS = NULL;
		
	fclose(pFile);

	g_pSSHTMLView->Navigate2((m_sRptPath), NULL, NULL);
	//open
	ShellExecute(*this, _T("open"), sPath, NULL, NULL, SW_SHOWNORMAL);	

	EndWaitCursor();

	g_wndStatusBarPane->SetText(_T("Ready"));

	CDialog::OnOK();
}
