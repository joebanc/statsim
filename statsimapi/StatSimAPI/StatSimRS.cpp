// StatSimRS.cpp : implementation file
//

#include "stdafx.h"
#include "StatSimAPI.h"
#include "StatSimRS.h"

#ifndef STATSIMCONN
	#include "statsimconn.h"
#endif

#include "SSVariants.h"
#include "StatSimWnd.h"
#include "null.h"	//mysql nulls


// CStatSimRS
CString sCritSQL(CString Table, CString critFld, CString critValue,
		CString labelFld, CString IDFld, CString attrFld, BOOL ltd)
{

	CString sSQL;
		
	critFld.TrimLeft(); critFld.TrimRight();
	critValue.TrimLeft(); critValue.TrimRight();

	if (ltd)
		sSQL.Format(_T("SELECT `%s`, `%s` FROM `%s`"), labelFld, attrFld, Table);
	else 
		sSQL.Format(_T("SELECT * FROM `%s`"), Table);


	if (critFld.GetLength() > 0 && critValue.GetLength() > 0) {
		sSQL = sSQL + _T(" WHERE ") +
			critFld + _T("=") + critValue + _T(";");
	}
	else if (critFld.GetLength() == 0 && critValue.GetLength() == 0) {
		sSQL = sSQL + _T(";");
	}
	else {
		AfxMessageBox( _T("Illegal criteria!") );
		sSQL = "";
	}

	//AfxMessageBox(sSQL);

	return sSQL;


}

CStatSimRS::CStatSimRS(CStatSimConn* pStatSimConn)
{
	m_IsODBC = pStatSimConn->IsODBC();
	m_IsMySQL = pStatSimConn->IsMySQL();

	if (m_IsODBC) {
		pMySQLrs = 0;
		pODBCrs = new CSSODBCrs( pStatSimConn->GetODBCdb() );
	}
	else {
		pODBCrs = 0;
		pMySQLrs = new CSSMySQLrs( pStatSimConn->GetMySQLdb() );
	}
}

CStatSimRS::CStatSimRS(CStatSimConn* pStatSimConn, LPCSTR table, BOOL bldArray,
					   SSRSOpenOptions dbOpenOpt)
{
	m_wArray = bldArray;

	m_IsODBC = pStatSimConn->IsODBC();
	m_IsMySQL = pStatSimConn->IsMySQL();
	if (m_IsODBC) {
		pMySQLrs = 0;
		pODBCrs = new CSSODBCrs( pStatSimConn->GetODBCdb(), table, bldArray, (CRecordset::OpenOptions) dbOpenOpt );
	}
	else {
		pODBCrs = 0;
		pMySQLrs = new CSSMySQLrs( pStatSimConn->GetMySQLdb(), table, bldArray, dbOpenOpt );
	}

}

CStatSimRS::CStatSimRS(CStatSimConn* pStatSimConn, CString sSQL, BOOL bldArray,
					   SSRSOpenOptions dbOpenOpt)
{
	m_wArray = bldArray;

	m_IsODBC = pStatSimConn->IsODBC();
	m_IsMySQL = pStatSimConn->IsMySQL();
	if (m_IsODBC) {
		pMySQLrs = 0;
		pODBCrs = new CSSODBCrs( pStatSimConn->GetODBCdb(), sSQL, bldArray, (CRecordset::OpenOptions) dbOpenOpt );
	}
	else {
		pODBCrs = 0;
		pMySQLrs = new CSSMySQLrs( pStatSimConn->GetMySQLdb(), sSQL, bldArray, dbOpenOpt );
	}

}

CStatSimRS::CStatSimRS(CDatabase* pStatSimConn)
{
	m_IsODBC = true;
	m_IsMySQL = false;
	pMySQLrs = 0;
	pODBCrs = new CSSODBCrs( pStatSimConn );

}

CStatSimRS::CStatSimRS(CDatabase* pStatSimConn, LPCSTR table, BOOL bldArray,
					   CRecordset::OpenOptions dbOpenOpt)
{
	m_IsODBC = true;
	m_IsMySQL = false;
	pMySQLrs = 0;
	pODBCrs = new CSSODBCrs( pStatSimConn, table, bldArray, (CRecordset::OpenOptions) dbOpenOpt );
}

CStatSimRS::CStatSimRS(CDatabase* pStatSimConn, CString sSQL, BOOL bldArray,
					   CRecordset::OpenOptions dbOpenOpt)
{
	m_IsODBC = true;
	m_IsMySQL = false;
	pMySQLrs = 0;
	pODBCrs = new CSSODBCrs( pStatSimConn, sSQL, bldArray, (CRecordset::OpenOptions) dbOpenOpt );

}


CStatSimRS::CStatSimRS(mysqlpp::Connection* pStatSimConn)
{
	m_IsODBC = false;
	m_IsMySQL = true;
	pODBCrs = 0;
	pMySQLrs = new CSSMySQLrs( pStatSimConn );

}

CStatSimRS::CStatSimRS(mysqlpp::Connection* pStatSimConn, LPCSTR table, BOOL bldArray,
					   SSRSOpenOptions dbOpenOpt)
{
	m_IsODBC = false;
	m_IsMySQL = true;
	pODBCrs = 0;
	pMySQLrs = new CSSMySQLrs( pStatSimConn, table, bldArray, dbOpenOpt );

}

CStatSimRS::CStatSimRS(mysqlpp::Connection* pStatSimConn, CString sSQL, BOOL bldArray,
					   SSRSOpenOptions dbOpenOpt)
{
	m_IsODBC = false;
	m_IsMySQL = true;
	pODBCrs = 0;
	pMySQLrs = new CSSMySQLrs( pStatSimConn, sSQL, bldArray, dbOpenOpt );

}


CStatSimRS::~CStatSimRS()
{

	delete pODBCrs; pODBCrs = NULL;
	delete pMySQLrs; pMySQLrs = NULL;

}

void CStatSimRS::SetRowPos(int pos)
{

	if (m_IsODBC) {
		//ODBC
		pODBCrs->SetRowPos(pos);
	}
	else {
		//MySQL
		pMySQLrs->SetRowPos(pos);
	}

}
void CStatSimRS::MoveNext()
{

	if (m_IsODBC) {
		//ODBC
		pODBCrs->MoveNext();
	}
	else {
		//MySQL
		pMySQLrs->MoveNext();
	}

}
void CStatSimRS::MovePrevious()
{

	if (m_IsODBC) {
		//ODBC
		pODBCrs->MovePrev();
	}
	else {
		//MySQL
		pMySQLrs->MovePrevious();
	}

}
void CStatSimRS::MoveFirst()
{

	if (m_IsODBC) {
		//ODBC
		pODBCrs->MoveFirst();
	}
	else {
		//MySQL
		pMySQLrs->MoveFirst();
	}

}
void CStatSimRS::Refresh()
{

	if (m_IsODBC) {
		//ODBC
		pODBCrs->Move(0);
	}
	else {
		//MySQL - not applicable?
		return;
	}

}
bool CStatSimRS::IsOpen()
{

	if (m_IsODBC) {
		//ODBC
		return pODBCrs->IsOpen();
	}
	else {
		//MySQL
		return pMySQLrs->IsOpen();
	}

}
bool CStatSimRS::IsEOF()
{

	if (m_IsODBC) {
		//ODBC
		return pODBCrs->IsEOF();
	}
	else {
		//MySQL
		return pMySQLrs->IsEOF();
	}

}
long CStatSimRS::GetRecordCount()
{

	if (m_IsODBC) {
		//ODBC
		return pODBCrs->GetRecordCount();
	}
	else {
		//MySQL
		return pMySQLrs->GetRecordCount();
	}

}

int CStatSimRS::GetFieldCount()
{

	if (m_IsODBC) {
		//ODBC
		return pODBCrs->GetFieldCount();
	}
	else {
		//MySQL
		return pMySQLrs->GetFieldCount();
	}

}
CString CStatSimRS::GetDefaultConnect()
{
	CString sDefConn;

	if (m_IsODBC) {
		//ODBC
		sDefConn = _MBCS("ODBC;");
	}
	else {
		//MySQL
		sDefConn = _MBCS("MySQL;");
	}

	return sDefConn;
}

CString CStatSimRS::GetDefaultSQL()
{
	ASSERT(FALSE);

	CString sDefSQL;
	
	if (m_IsODBC) {
		//ODBC
		sDefSQL = _MBCS("!");
	}
	else {
		//MySQL
		sDefSQL = _MBCS("!");
	}

	return sDefSQL;
}
void CStatSimRS::Close()
{

	if (m_IsODBC) {
		//ODBC
		pODBCrs->Close();
	}
	else {
		//MySQL
		pMySQLrs->Close();
	}

}
short CStatSimRS::RunSQL(CString sSQL, SSRSOpenOptions dbOpenOpt, 
					   BOOL bldArray, UINT scale, LPCSTR **strFill)
{
	m_wArray = bldArray;

	if (m_IsODBC) {
		//ODBC
		return pODBCrs->RunSQL(sSQL, (CRecordset::OpenOptions) dbOpenOpt, bldArray, scale, strFill);
	}
	else {
		//MySQL
		return pMySQLrs->RunSQL(sSQL, (SSRSOpenOptions) dbOpenOpt, bldArray, scale, strFill);
	}

}

CString CStatSimRS::SQLFldType(USHORT j, BOOL c)
{
	if (m_IsODBC) {
		//ODBC
		return pODBCrs->SQLFldType(j, c);
	}
	else {
		//MySQL
		return pMySQLrs->SQLFldType(j, c);
	}

}

int CStatSimRS::GetFldIndex(LPCSTR sFld)
{
	if (m_IsODBC) {
		//ODBC
		return pODBCrs->GetFldIndex(sFld);
	}
	else {
		//MySQL
		return pMySQLrs->GetFldIndex(sFld);
	}

}

CString CStatSimRS::SQLFldValue(LPCSTR strFld, CString strEnclose,
		BOOL bsReplace, BOOL eReplace)
{

	if (m_IsODBC) {
		//ODBC
		return pODBCrs->SQLFldValue(strFld, strEnclose, bsReplace, eReplace);
	}
	else {
		//MySQL
		return pMySQLrs->SQLFldValue(strFld, strEnclose, bsReplace, eReplace);
	}

}

CString CStatSimRS::SQLFldValue(USHORT j, CString strEnclose,
		BOOL bsReplace, BOOL eReplace)
{

	
	if (m_IsODBC) {
		//ODBC
		return pODBCrs->SQLFldValue(j, strEnclose, bsReplace, eReplace);
	}
	else {
		//MySQL
		return pMySQLrs->SQLFldValue(j, strEnclose, bsReplace, eReplace);
	}
}


LPCSTR* CStatSimRS::GetFieldNames()
{

	if (m_IsODBC) {
		//ODBC
		return pODBCrs->GetFieldNames();
	}
	else {
		//MySQL
		return pMySQLrs->GetFieldNames();
	}

}

LPCSTR CStatSimRS::GetFieldName(USHORT j)
{

	if (m_IsODBC) {
		//ODBC
		return pODBCrs->GetFieldName(j);
	}
	else {
		//MySQL
		return pMySQLrs->GetFieldName(j);
	}

}

SWORD CStatSimRS::GetFieldSQLType(USHORT j)
{

	if (m_IsODBC) {
		//ODBC
		return pODBCrs->GetFieldSQLType(j);
	}
	else {
		//MySQL
		return pMySQLrs->GetFieldSQLType(j);
	}

}

UDWORD CStatSimRS::GetFieldPrecision(USHORT j)
{

	if (m_IsODBC) {
		//ODBC
		return pODBCrs->GetFieldPrecision(j);
	}
	else {
		//MySQL
		return pMySQLrs->GetFieldPrecision(j);
	}

}

SWORD CStatSimRS::GetFieldScale(USHORT j)
{

	if (m_IsODBC) {
		//ODBC
		return pODBCrs->GetFieldScale(j);
	}
	else {
		//MySQL
		return pMySQLrs->GetFieldScale(j);
	}

}

SWORD CStatSimRS::GetFieldNullability(USHORT j)
{

	if (m_IsODBC) {
		//ODBC
		return pODBCrs->GetFieldNullability(j);
	}
	else {
		//MySQL
		return pMySQLrs->GetFieldNullability(j);
	}

}

HTREEITEM* CStatSimRS::GetHierRS(CString Table, CString critFld, CString critValue,
		CString labelFld, CString IDFld, CString attrFld, DWORD itemData,
		CTreeCtrl *pTreeCtrl, HTREEITEM hParent, BOOL ltd)
{

	if (m_IsODBC) {
		//ODBC
		return pODBCrs->GetHierRS(Table, critFld, critValue, labelFld, IDFld, attrFld, itemData,
			pTreeCtrl, hParent, ltd);
	}
	else {
		//MySQL
		return pMySQLrs->GetHierRS(Table, critFld, critValue, labelFld, IDFld, attrFld, itemData,
			pTreeCtrl, hParent, ltd);
	}


}

int CStatSimRS::PutValuesTo(CComboBox *pComboBox, 
							 LPCSTR labelFld, LPCSTR IDFld, LPCSTR attrFld, 
							 LPCSTR sSQL)
{
	if (m_IsODBC) {
		//ODBC
		return pODBCrs->PutValuesTo(pComboBox, labelFld, IDFld, attrFld, sSQL);
	}
	else {
		//MySQL
		return pMySQLrs->PutValuesTo(pComboBox, labelFld, IDFld, attrFld, sSQL);
	}


}
void CStatSimRS::GetHierRS(CString Table, CString critFld, CString critValue,
			   CString labelFld, CString IDFld, CString attrFld, 
			   CComboBox *pComboBox, BOOL ltd)
{

	if (m_IsODBC) {
		//ODBC
		return pODBCrs->GetHierRS(Table, critFld, critValue, labelFld, IDFld,
			attrFld, pComboBox, ltd);
	}
	else {
		//MySQL
		return pMySQLrs->GetHierRS(Table, critFld, critValue, labelFld, IDFld,
			attrFld, pComboBox, ltd);
	}
}


void CStatSimRS::GetHierRS(CString Table, CString critFld, CString critValue,
		CString labelFld, CString IDFld, CString attrFld, 
		CListCtrl *pListCtrl, BOOL ltd)
{
	if (m_IsODBC) {
		//ODBC
		return pODBCrs->GetHierRS(Table, critFld, critValue, labelFld, IDFld,
			attrFld, pListCtrl, ltd);
	}
	else {
		//MySQL
		return pMySQLrs->GetHierRS(Table, critFld, critValue, labelFld, IDFld,
			attrFld, pListCtrl, ltd);
	}

}

varvec2d* CStatSimRS::GetRSArray()
{
	if(!m_wArray) {
		AfxMessageBox(L"Array was not built!");
		return 0;
	}
	varvec2d* rArray;

	if (m_IsODBC) {
		//ODBC
		//AfxMessageBox(L"ODBC");
		rArray = pODBCrs->GetRSArray();
	}
	else {
		//MySQL
		//AfxMessageBox(L"MySQL");
		rArray = pMySQLrs->GetRSArray();
	}

	return rArray;

}

varvec2d* CStatSimRS::GetRSArray(CString Table, CString critFld, CString critValue,
		CString labelFld, CString IDFld, CString attrFld, 
		UINT scale, BOOL ltd, LPCSTR **strFill)
{

	if (m_IsODBC) {
		//ODBC
		return pODBCrs->GetRSArray(Table, critFld, critValue, labelFld, IDFld,
			attrFld, scale, ltd, strFill);
	}
	else {
		//MySQL
		return pMySQLrs->GetRSArray(Table, critFld, critValue, labelFld, IDFld,
			attrFld, scale, ltd, strFill);
	}

}
BOOL CStatSimRS::Exists(LPCSTR sSearchItem, LPCSTR sSearchField)
{
	if (m_IsODBC) {
		//ODBC
		return pODBCrs->Exists(sSearchItem, sSearchField);
	}
	else {
		//MySQL
		return pMySQLrs->Exists(sSearchItem, sSearchField);
	}

}
BOOL CStatSimRS::Exists(LPCSTR sSearchItem, int idxField)
{
	if (m_IsODBC) {
		//ODBC
		return pODBCrs->Exists(sSearchItem, idxField);
	}
	else {
		//MySQL
		return pMySQLrs->Exists(sSearchItem, idxField);
	}

}

//MySQL Version
CSSMySQLrs::CSSMySQLrs(mysqlpp::Connection* pConn)
{
	SSQuery = NULL;
	SSRow = NULL;
	SSResult = NULL;
	FieldSQLType = NULL;

	value_array = NULL;

	Init(pConn);

}

CSSMySQLrs::CSSMySQLrs(mysqlpp::Connection* pConn, LPCSTR table, BOOL bldArray,
					   SSRSOpenOptions Opt)
{
	SSQuery = NULL;
	SSRow = NULL;
	SSResult = NULL;
	FieldSQLType = NULL;

	value_array = NULL;

	Init(pConn);

	CString sSQL;
	sSQL.Format(_T("SELECT * FROM `%s`"), (CString) table);

	//AfxMessageBox(sSQL);
		
	RunSQL(sSQL, Opt, bldArray);

}

CSSMySQLrs::CSSMySQLrs(mysqlpp::Connection* pConn, CString sSQL, BOOL bldArray,
					   SSRSOpenOptions Opt)
{
	SSQuery = NULL;
	SSRow = NULL;
	SSResult = NULL;
	FieldSQLType = NULL;

	value_array = NULL;

	Init(pConn);

	RunSQL(sSQL, Opt, bldArray);

}

CSSMySQLrs::~CSSMySQLrs()
{
	Close();

	//delete value_array;

	//I still don't know why this doesn't work
	//destroyvec(value_array);

}

void CSSMySQLrs::Close()
{
	if (SSQuery!=NULL) {
		SSQuery->reset();
		delete SSQuery; SSQuery = NULL;
	}
	if (SSRow!=NULL) {
		delete SSRow; SSRow = NULL;
	}
	if (SSResult!=NULL) {
		SSResult->clear();
		delete SSResult; SSResult = NULL;
	}
	if (FieldSQLType!=NULL) {
		delete [] FieldSQLType; FieldSQLType = NULL;
	}
	if (FieldName!=NULL) {
		delete [] FieldName; FieldName = NULL;
	}
}
int CSSMySQLrs::Init(mysqlpp::Connection* pConn)
{

	int nRetCode = 0;
	CString strMsg, str1, str2;
		
	try {

		SSCon = pConn;
		
		SSQuery = new mysqlpp::Query(SSCon);
		// This creates a query object that is bound to SSCon.
		SSResult = new mysqlpp::StoreQueryResult;
		// This creates a result object
		SSRow = new mysqlpp::Row;
	}
	
	catch (mysqlpp::BadQuery er){ 
		
		str1 = er.what();			
		strMsg = _T(/*"Error: "*/ "") + str1;
		AfxMessageBox(strMsg);
		
		nRetCode = -1;  
	} 
	
	catch (mysqlpp::BadConversion er) {

		str1 = er.data.c_str();
		str2 = er.type_name;
		strMsg = _T("Error: Tried to convert ") + str1 + _T(" to a ") + str2 + _T(".");
		
		AfxMessageBox(strMsg);
		
		nRetCode = -1;
	}
	
	return nRetCode;

}
short CSSMySQLrs::RunSQL(CString sSQL, SSRSOpenOptions dbOpenOpt, 
					   BOOL bldArray, UINT scale, LPCSTR **strFill)
{
	int nRetCode = 0;
	CString strMsg, str1, str2;
	curr_i = 0;	//initialize current position of record pointer
		
	try {

		//reset if necessary
		if (SSQuery) {
			SSQuery->reset();
		}
		//_tochar sql(ConstChar(sSQL));
		*SSQuery << ConstChar(sSQL);
		//Assign the sql

		//reset if necessary
		if (SSResult) {
			SSResult->clear();
		}
		*SSResult = SSQuery->store();
		//run and store the query

		m_nFields = SSResult->num_fields();
		m_nRecords= SSResult->num_rows();

		FieldName = new LPCSTR[m_nFields];
		FieldSQLType = new mysqlpp::mysql_type_info[m_nFields];

		for (UINT j=0; j<m_nFields; j++) {
			CString sFld(SSResult->field_name(j).c_str());
			FieldName[j] = ConstChar(sFld);
			FieldSQLType[j] = SSResult->field_type(j);
		}		
		
		//AfxMessageBox(L"Query was run!");
		if (bldArray)
			BuildArray(scale, strFill);
	
	}
	
	catch (mysqlpp::BadQuery er){ 
		
		str1 = er.what();			
		strMsg = _T(/*"Error: "*/ "") + str1;
		AfxMessageBox(strMsg);
		
		nRetCode = -1;  
	} 
	
	catch (mysqlpp::BadConversion er) {

		str1 = er.data.c_str();
		str2 = er.type_name;
		strMsg = _T("Error: Tried to convert ") + str1 + _T(" to a ") + str2 + _T(".");
		
		AfxMessageBox(strMsg);
		
		nRetCode = -1;
	}
	
	return nRetCode;
	
}

long CSSMySQLrs::GetRecordCount()
{
	return m_nRecords;
}

int CSSMySQLrs::GetFieldCount()
{

	return m_nFields;

}
LPCSTR* CSSMySQLrs::GetFieldNames()
{

	return FieldName;

}
LPCSTR CSSMySQLrs::GetFieldName(USHORT j)
{

	return FieldName[j];

}
SWORD CSSMySQLrs::GetFieldSQLType(USHORT j)
{
	USES_CONVERSION;
	AfxMessageBox( A2CT(FieldSQLType[j].c_type().raw_name()) );
	return -1;

}

UDWORD CSSMySQLrs::GetFieldPrecision(USHORT j)
{

	//return FieldPrecision[j];
	return -1;

}
SWORD CSSMySQLrs::GetFieldScale(USHORT j)
{

	return -1;

}

SWORD CSSMySQLrs::GetFieldNullability(USHORT j)
{

	return -1;

}

CString CSSMySQLrs::SQLFldType(UINT j, BOOL c)
{
	CString strType;

	//varchar is converted to text, can't retrieve field length because of construction
	if (FieldSQLType[j].c_type()==typeid(mysqlpp::sql_varchar)) {
		strType.Format(_T("TEXT NOT NULL"));
	}
	else if (FieldSQLType[j].c_type()==typeid(mysqlpp::Null<mysqlpp::sql_varchar>)) {
		strType.Format(_T("TEXT NULL"));
	}
	else {
		CString str(FieldSQLType[j].sql_name());
		strType = str;
	}

	return strType;					

}
HTREEITEM* CSSMySQLrs::GetHierRS(CString Table, CString critFld, CString critValue,
		CString labelFld, CString IDFld, CString attrFld, DWORD itemData,
		CTreeCtrl *pTreeCtrl, HTREEITEM hParent, BOOL ltd)
{

	CString sSQL = sCritSQL(Table, critFld, critValue, labelFld, IDFld, attrFld, ltd);
		
	RunSQL(sSQL);

	if ( m_nRecords > 0 ) {
		MoveFirst();
	}

	HTREEITEM* hlvl = new HTREEITEM[m_nRecords];
	//0 alloted for the parent


	CString lvlLabel, lvlID, lvlItem, lvlAttr;

	for (ULONG i=0; i<m_nRecords; i++)
	{			
		lvlLabel = SQLFldValue( ConstChar(labelFld) );
		lvlID = SQLFldValue( ConstChar(IDFld) );
		lvlAttr = SQLFldValue( ConstChar(attrFld) );
		lvlItem = lvlLabel + " - " + lvlID;

		if (pTreeCtrl!=NULL && hParent!=NULL) {
			hlvl[i] = pTreeCtrl->InsertItem(TVIF_TEXT,
				(lvlItem), 0, 0, 0, 0, 0, hParent, NULL);

			pTreeCtrl->SetItemData(hlvl[i], itemData);		

		}
		else {
			hlvl[i] = NULL;
		}
		
		MoveNext();

	}

	return hlvl;

}

void CSSMySQLrs::GetHierRS(CString Table, CString critFld, CString critValue,
			   CString labelFld, CString IDFld, CString attrFld, 
			   CComboBox *pComboBox, BOOL ltd)
{


	CString sSQL = sCritSQL(Table, critFld, critValue, labelFld, IDFld, attrFld, ltd);
	RunSQL(sSQL);

	if (pComboBox!=NULL) 
		pComboBox->ResetContent();

	if ( m_nRecords > 0 ) {
		MoveFirst();
	}

	CString itmLabel, itmID, itmAttr;

	for (UINT i=0; i<m_nRecords; i++)
	{		
		itmLabel = SQLFldValue( ConstChar(labelFld) );
		itmID = SQLFldValue( ConstChar(IDFld) );

		if (pComboBox!=NULL) {
			pComboBox->AddString(itmLabel);
			int cbIndex = pComboBox->SetItemData(i, _ttol((itmID) ) );
		}

		MoveNext();
	}

}


void CSSMySQLrs::GetHierRS(CString Table, CString critFld, CString critValue,
		CString labelFld, CString IDFld, CString attrFld, 
		CListCtrl *pListCtrl, BOOL ltd)
{
	CString sSQL = sCritSQL(Table, critFld, critValue, labelFld, IDFld, attrFld, ltd);
		
	RunSQL(sSQL);

	if ( m_nRecords > 0 ) {
		MoveFirst();
	}

	int nCols = 0;

	if (pListCtrl!=NULL) {

		nCols = pListCtrl->GetHeaderCtrl()->GetItemCount();		

	}
	
	CString lvlLabel, lvlID, lvlItem, lvlAttr;
	for (ULONG i=0; i<m_nRecords; i++)
	{			
		
		lvlLabel = SQLFldValue( ConstChar(labelFld) );
		lvlID = SQLFldValue( ConstChar(IDFld) );
		lvlAttr = SQLFldValue( ConstChar(attrFld) );
		lvlItem = lvlLabel + " - " + lvlID;
	
		if (pListCtrl!=NULL) {
			pListCtrl->InsertItem( i, (lvlLabel) );
			
			for (int j=1; j < nCols; j++) {
				
				pListCtrl->SetItemText(i, j, (lvlAttr) );
			}
		}

		MoveNext();
	}

}
varvec2d* CSSMySQLrs::GetRSArray()
{
	return value_array;
}
varvec2d* CSSMySQLrs::GetRSArray(CString Table, CString critFld, CString critValue,
		CString labelFld, CString IDFld, CString attrFld, 
		UINT scale, BOOL ltd, LPCSTR **strFill)
{

	CString sSQL = sCritSQL(Table, critFld, critValue, labelFld, IDFld, attrFld, ltd);

	RunSQL(sSQL, readOnly, TRUE, scale, strFill);
	
	return value_array;

}
void CSSMySQLrs::SetRowPos(int pos)
{
	curr_i = pos;
}

void CSSMySQLrs::MoveNext()
{
	if (curr_i < m_nRecords) {
		++curr_i;
	}
	else {
		AfxMessageBox( _T("End of recordset reached!") );
	}
	
}

void CSSMySQLrs::MovePrevious()
{

	if (curr_i > 0) {
		--curr_i;
	}
	else {
		AfxMessageBox( _T("Beginning of recordset reached!") );
	}

}

void CSSMySQLrs::MoveFirst()
{
	curr_i = 0;
}

int CSSMySQLrs::GetFldIndex(LPCSTR sFld)
{

	std::string sCurrFld;
	_cochar str(sFld);
	sCurrFld = str;

	return SSResult->field_num(sCurrFld);


}
bool CSSMySQLrs::IsOpen()
{
	if (SSQuery || SSResult) {
		return TRUE;
	}
	else {
		return false;
	}




}
bool CSSMySQLrs::IsEOF()
{

	if ( curr_i>=m_nRecords ) {
		return TRUE;
	}
	else {
		return FALSE;
	}



}

CString CSSMySQLrs::SQLFldValue(LPCSTR strFld, CString strEnclose,
		BOOL bsReplace, BOOL eReplace)
{

	int iFld = GetFldIndex(strFld);
	return SQLFldValue( iFld, strEnclose, bsReplace, eReplace);


}

CString CSSMySQLrs::SQLFldValue(UINT j, CString strEnclose,
		BOOL bsReplace, BOOL eReplace)
{

	CString strVal, sFind, sReplace;
	LPCTSTR str;

	if ( SSResult->empty() ) {
		AfxMessageBox(_T("Error retrieving record values."));
		return _T("");
	}

	*SSRow = SSResult->at(curr_i);

	if ( SSRow->empty() ) {
		AfxMessageBox(_T("Error retrieving row values."));
		return _T("");
	}

	std::string s1( SSRow->at(j) );
	CString s( s1.c_str() );
	str = s;

	if ( strEnclose.TrimLeft().TrimRight().GetLength()>0 ) {
		sReplace= _T("\\") + strEnclose.Left(1);
		sFind = strEnclose.Left(1);
	}

	using namespace mysqlpp;

	//numerics
	if ( 
		FieldSQLType[j] == typeid(sql_tinyint) ||
		FieldSQLType[j] == typeid(sql_tinyint_unsigned) ||
		FieldSQLType[j] == typeid(sql_smallint) ||
		FieldSQLType[j] == typeid(sql_smallint_unsigned) ||
		FieldSQLType[j] == typeid(sql_int) ||
		FieldSQLType[j] == typeid(sql_int_unsigned) ||
		FieldSQLType[j] == typeid(sql_mediumint) ||
		FieldSQLType[j] == typeid(sql_mediumint_unsigned) ||
		FieldSQLType[j] == typeid(sql_bigint) ||
		FieldSQLType[j] == typeid(sql_bigint_unsigned) ||
		FieldSQLType[j] == typeid(sql_float) ||
		FieldSQLType[j] == typeid(sql_double) ||
		FieldSQLType[j] == typeid(sql_decimal) ||
		FieldSQLType[j] == typeid(Null<sql_tinyint>) ||	//nullable types
		FieldSQLType[j] == typeid(Null<sql_tinyint_unsigned>) ||
		FieldSQLType[j] == typeid(Null<sql_smallint>) ||
		FieldSQLType[j] == typeid(Null<sql_smallint_unsigned>) ||
		FieldSQLType[j] == typeid(Null<sql_int>) ||
		FieldSQLType[j] == typeid(Null<sql_int_unsigned>) ||
		FieldSQLType[j] == typeid(Null<sql_mediumint>) ||
		FieldSQLType[j] == typeid(Null<sql_mediumint_unsigned>) ||
		FieldSQLType[j] == typeid(Null<sql_bigint>) ||
		FieldSQLType[j] == typeid(Null<sql_bigint_unsigned>) ||
		FieldSQLType[j] == typeid(Null<sql_float>) ||
		FieldSQLType[j] == typeid(Null<sql_double>) ||
		FieldSQLType[j] == typeid(Null<sql_decimal>) 
		)
	{
		strVal.Format(_T("%s"), str);
	}
	//strings / time
	else if (
		FieldSQLType[j] == typeid(sql_enum) ||
		FieldSQLType[j] == typeid(sql_blob) ||
		FieldSQLType[j] == typeid(sql_tinyblob) ||
		FieldSQLType[j] == typeid(sql_mediumblob) ||
		FieldSQLType[j] == typeid(sql_longblob) ||
		FieldSQLType[j] == typeid(sql_char) ||
		FieldSQLType[j] == typeid(sql_varchar) ||
		FieldSQLType[j] == typeid(sql_date) ||
		FieldSQLType[j] == typeid(sql_time) ||
		FieldSQLType[j] == typeid(sql_timestamp) ||
		FieldSQLType[j] == typeid(sql_datetime) ||
		FieldSQLType[j] == typeid(Null<sql_enum>) ||	//nullable types
		FieldSQLType[j] == typeid(Null<sql_blob>) ||
		FieldSQLType[j] == typeid(Null<sql_tinyblob>) ||
		FieldSQLType[j] == typeid(Null<sql_mediumblob>) ||
		FieldSQLType[j] == typeid(Null<sql_longblob>) ||
		FieldSQLType[j] == typeid(Null<sql_char>) ||
		FieldSQLType[j] == typeid(Null<sql_varchar>) ||
		FieldSQLType[j] == typeid(Null<sql_date>) ||
		FieldSQLType[j] == typeid(Null<sql_time>) ||
		FieldSQLType[j] == typeid(Null<sql_timestamp>) ||
		FieldSQLType[j] == typeid(Null<sql_datetime>)
		) 
	{ 
		strVal.Format(_T("%s"), str);

		if (bsReplace)
			strVal.Replace(_T("\\"), _T("\\\\"));

		if (eReplace)
			strVal.Replace(sFind, sReplace);

		strVal = strEnclose.Left(1) + strVal + strEnclose.Right(1);
	}
	else if ( FieldSQLType[j] == typeid(Null<void>) ) {

		strVal = _MBCS("null");
		if (bsReplace)
			strVal.Replace(_T("\\"), _T("\\\\"));

		if (eReplace)
			strVal.Replace(sFind, sReplace);

		strVal = strEnclose.Left(1) + strVal + strEnclose.Right(1);

	}
	else {
		AfxMessageBox(_T("Error retrieving data type."));
		return _T("");
	}

	//AfxMessageBox(strVal);
	return strVal;					

}

/*
void CSSMySQLrs::BuildArray(UINT scale, LPCSTR **strFill)
{
	pRSArrBldr = new CSSArrayBuilder(STR_ARR, 2);

	ULONG i;
	UINT j, k;

	if (m_nRecords > 0) {
		pRSArrBldr->SetDim(DIM1, m_nRecords + scale);
		pRSArrBldr->SetDim(DIM2, m_nFields);

		MoveFirst();
		
		for (i=scale; i<m_nRecords + scale; i++) {
			for (j=0; j<m_nFields; j++) {
				pRSArrBldr->PutVal(i, j, ConstChar(SQLFldValue(j)));
			}
			MoveNext();
		}
		
		for (k=0; k<scale; k++) {
			for (j=0; j<m_nFields; j++) {
				if (strFill==NULL) 
					pRSArrBldr->PutVal(k, j, "...");
				else
					pRSArrBldr->PutVal(k, j, strFill[k][j]);
			}
		}
	}


}
*/
int CSSMySQLrs::PutValuesTo(CComboBox *pComboBox, 
							 LPCSTR labelFld, LPCSTR IDFld, LPCSTR attrFld, 
							 LPCSTR sSQL)
{

	if (sSQL!=NULL) {
		CString sRunSQL;
		sRunSQL.Format(_T("%s"), (CString) sSQL);
		RunSQL(sRunSQL);
	}

	pComboBox->ResetContent();

	int iCB, iDW;

	if ( m_nRecords > 0 ) {
		MoveFirst();
	}

	CString itmLabel, itmID, itmAttr;

	for (UINT i=0; i<m_nRecords; i++)
	{		
		itmLabel = SQLFldValue(labelFld);
		itmID = SQLFldValue(IDFld);

		
		if (pComboBox!=NULL) {
			iCB = pComboBox->AddString(itmLabel);
			iDW = pComboBox->SetItemData(iCB, _ttol((itmID)));
		}

		MoveNext();
	}

	return iDW;


}
BOOL CSSMySQLrs::Exists(LPCSTR sSearchItem, int idxField)
{
	BOOL found = FALSE;

	if (m_nRecords<=0)
		return FALSE;

	MoveFirst();

	for (long i=0; i<m_nRecords; i++) {
		CString sCrit(sSearchItem);
		if (sCrit==SQLFldValue(idxField))
			return TRUE;
		
		MoveNext();

	}

	return found;
}
BOOL CSSMySQLrs::Exists(LPCSTR sSearchItem, LPCSTR sSearchField)
{
	int iFld = GetFldIndex(sSearchField);
	return Exists( sSearchItem, iFld );

}
void CSSMySQLrs::BuildArray(UINT scale, LPCSTR **strFill)
{

	value_array = new varvec2d;

	ULONG i;
	UINT j, k;

	if (m_nRecords > 0) {
		(*value_array).resize(m_nRecords + scale);
		//pRSArrBldr->SetDim(DIM1, m_nRecords + scale);
		//pRSArrBldr->SetDim(DIM2, m_nFields);

		MoveFirst();
				

		for (i=scale; i<m_nRecords + scale; i++) {
			(*value_array)[i].resize(m_nFields);
			for (j=0; j<m_nFields; j++) {
				(*value_array)[i][j] = ConstChar(SQLFldValue(j));
				//pRSArrBldr->PutVal(i, j, ConstChar(SQLFldValue(j)));
			}
			MoveNext();
		}

		for (k=0; k<scale; k++) {
			for (j=0; j<m_nFields; j++) {
				if (strFill==NULL) {
					//pRSArrBldr->PutVal(k, j, _MBCS("..."));
					(*value_array)[k][j] = "...";
				}
				else {
					//pRSArrBldr->PutVal(k, j, strFill[k][j]);
					//(*value_array)[k][j] = ConstChar( strFill[k][j] ); //not this because row k's cols has not been initialized
					(*value_array)[k].push_back(( strFill[k][j] ));
				}

			}
		}


	}

}

CSSODBCrs::CSSODBCrs(CDatabase* pStatSimConn = 0):CRecordset(pStatSimConn)
{
	InitmPtrs();

	m_nDefaultType = snapshot;
}

CSSODBCrs::CSSODBCrs(CDatabase* pStatSimConn, LPCSTR table, BOOL bldArray,
					   CRecordset::OpenOptions dbOpenOpt):CRecordset(pStatSimConn)
{

	InitmPtrs();

	CString sSQL;
	sSQL.Format( _T("SELECT * FROM `%s`;"), (CString) table );

	RunSQL(sSQL, dbOpenOpt, bldArray);

	m_nDefaultType = snapshot;

}

CSSODBCrs::CSSODBCrs(CDatabase* pStatSimConn, CString sSQL, BOOL bldArray,
					   CRecordset::OpenOptions dbOpenOpt):CRecordset(pStatSimConn)
{
	InitmPtrs();

	RunSQL(sSQL, dbOpenOpt, bldArray);

	m_nDefaultType = snapshot;

}


void CSSODBCrs::SetRowPos(int pos)
{

	SetAbsolutePosition(pos);

}

void CSSODBCrs::InitmPtrs()
{
	
	FieldName = NULL;
	FieldSQLType = NULL; 
	FieldPrecision = NULL;
	FieldScale = NULL;
	FieldNullability = NULL;	
	FieldValue = NULL;	

	value_array = NULL;
	//pRSArrBldr = NULL;	

}

CSSODBCrs::~CSSODBCrs()
{
	/*
	if (FieldName!=NULL)
		delete FieldName; FieldName = 0;

	if (FieldSQLType!=NULL)
		delete FieldSQLType; FieldSQLType = 0;

	if (FieldPrecision!=NULL)
		delete FieldPrecision; FieldPrecision = 0;

	if (FieldScale!=NULL)
		delete FieldScale; FieldScale = 0;

	if (FieldNullability!=NULL)
		delete FieldNullability; FieldNullability = 0;
	
	if (FieldValue!=NULL)
		delete FieldValue; FieldValue = 0;

	*/
	//destruct array builders and initialized pointers

	//I still don't know
	//destroyvec(value_array);
	//AfxMessageBox(L"OK destroy");
	
	delete [] FieldName;
	delete [] FieldSQLType;
	delete [] FieldPrecision;
	delete [] FieldScale;
	delete [] FieldNullability;

}

long CSSODBCrs::GetRecordCount()
{
	return m_nRecords;
}

int CSSODBCrs::GetFieldCount()
{

	return m_nFields;

}

CString CSSODBCrs::GetDefaultConnect()
{
	CString sDefConn("ODBC;");
	return sDefConn;
}

CString CSSODBCrs::GetDefaultSQL()
{
	ASSERT(FALSE);
	CString sDefSQL("!");
	return sDefSQL;
}
void CSSODBCrs::Close()
{

	CRecordset::Close();

}
short CSSODBCrs::RunSQL(CString sSQL, CRecordset::OpenOptions dbOpenOpt, 
					   BOOL bldArray, UINT scale, LPCSTR **strFill)
{

	TRY
	{
		if (!Open(CRecordset::snapshot, sSQL, dbOpenOpt)) {
			AfxMessageBox( _T("Error running SQL.") );
			return SQL_ERROR;
		}
		
		else {
			
			while (!IsEOF()) {
				MoveNext();
			}
			
			m_nRecords = CRecordset::GetRecordCount();
			m_nFields = GetODBCFieldCount();

			FieldName = new LPCSTR[m_nFields];
			FieldSQLType = new SWORD[m_nFields];
			FieldPrecision = new UDWORD[m_nFields];
			FieldScale = new SWORD[m_nFields];
			FieldNullability = new SWORD[m_nFields];
			
			for (UINT j=0; j<m_nFields; j++) {
				GetODBCFieldInfo(j, FieldInfo);
				FieldName[j] = ConstChar( FieldInfo.m_strName );
				FieldSQLType[j] = FieldInfo.m_nSQLType;
				FieldPrecision[j] = FieldInfo.m_nPrecision;
				FieldScale[j] = FieldInfo.m_nScale;
				FieldNullability[j] = FieldInfo.m_nNullability;
			}
			
			if (bldArray)
				BuildArray(scale, strFill);
			
			return SQL_SUCCESS;
		}
	}	
	
	CATCH(CDBException, e)
	{
		AfxMessageBox(e->m_strError);
		return e->m_nRetCode;
	}
	
	END_CATCH

}

CString CSSODBCrs::SQLFldType(USHORT j, BOOL c)
{
	CString strType;
	
	//lengths are not defined hence chars are text!	
	switch(FieldSQLType[j]) {					
						
	case SQL_BIT:					
		strType = "BIT";				
		break;				
						
	case SQL_TINYINT:					
		if (c)
			strType = "TINYINT";
		else
			strType = "INTEGER";
		break;				
						
	case SQL_SMALLINT:					
		if (c)
			strType = "SMALLINT";
		else
			strType = "INTEGER";
		break;				
						
	case SQL_INTEGER:					
		strType = "INTEGER";				
		break;				
						
	case SQL_REAL:					
		strType = "FLOAT";				
		break;				
						
	case SQL_FLOAT:					
		strType = "FLOAT";				
		break;				
						
	case SQL_DOUBLE:					
		strType = "DOUBLE";				
		break;				
						
	case SQL_DATE:					
		strType = "DATETIME";				
		break;				
						
	case SQL_TIME:					
		strType = "DATETIME";				
		break;				
						
	case SQL_TIMESTAMP:					
		strType = "TIMESTAMP";				
		break;				
						
	case SQL_NUMERIC:					
		strType = "DOUBLE";				
		break;				
						
	case SQL_DECIMAL:					
		strType = "DECIMAL";				
		break;				
						
	case SQL_BIGINT:					
		strType = "BIGINT";				
		break;				
						
	case SQL_CHAR:
		if (c)
			strType = "TINYTEXT";
		else
			strType = "TEXT";
		break;				
						
	case SQL_VARCHAR:					
		if (c)
			strType = "TINYTEXT";
		else
			strType = "TEXT";
		break;				
						
	case SQL_LONGVARCHAR:					
		strType = "LONGTEXT";				
		break;				
						
	case SQL_BINARY:					
		strType = "BLOB";				
		break;				
						
	case SQL_VARBINARY:					
		strType = "MEDIUMBLOB";				
		break;				
						
	case SQL_LONGVARBINARY:					
		strType = "LONGBLOB";				
		break;				
						
	default:					
		strType = "TEXT";				
		break;				
						
	}					
	return strType;					

}

int CSSODBCrs::GetFldIndex(LPCSTR sFld)
{
	CString sCritFld, sCurFld;
	sCritFld = NormStr(sFld);

	for (int j=0; j<(int) m_nFields; j++) {
		sCurFld	= NormStr( FieldName[j] );
		
		if ( sCurFld==sCritFld )
			return j;

	}

	return -1;

}

CString CSSODBCrs::SQLFldValue(LPCSTR strFld, CString strEnclose,
		BOOL bsReplace, BOOL eReplace)
{

	int iFld = GetFldIndex(strFld);
	return SQLFldValue( iFld, strEnclose, bsReplace, eReplace);
}

CString CSSODBCrs::SQLFldValue(USHORT j, CString strEnclose,
		BOOL bsReplace, BOOL eReplace)
{
	TRY
	{
		
		CString strVal, sFind, sReplace;
		CDBVariant fldValue;
		SWORD fldType;
		
		GetFieldValue(j, fldValue);
		fldType = FieldSQLType[j];
		
		ULONG dwType = fldValue.m_dwType;
		
		CString sCrit = NormStr( ConstChar(strEnclose) );
		
		if ( sCrit.GetLength()>0 ) {
			sReplace= _T("\\") + strEnclose.Left(1);
			sFind = strEnclose.Left(1);
		}

		switch(dwType) {

		case DBVT_NULL:		
			strVal = "NULL";	
				
			break;	
				
		case DBVT_BOOL:		
				
			strVal.Format(_T("%d"), fldValue.m_boolVal);	
			break;	
				
		case DBVT_UCHAR:		
			strVal.Format(_T("%d"), fldValue.m_chVal);
				
			if (bsReplace)	
				strVal.Replace(_T("\\"), _T("\\\\"));
				
			if (eReplace)	
				strVal.Replace(sFind, sReplace);
				
			strVal = strEnclose.Left(1) + strVal + strEnclose.Right(1);	
			break;	
				
		case DBVT_SHORT:	//0-255: 1 byte so char Value	
			strVal.Format(_T("%d"), fldValue.m_iVal);	
			break;	
				
		case DBVT_LONG:		
			strVal.Format(_T("%d"), fldValue.m_lVal);	
			break;	
				
		case DBVT_SINGLE:		
			strVal.Format(_T("%f"), fldValue.m_fltVal);	
			break;	
				
		case DBVT_DOUBLE:		
			strVal.Format(_T("%f"), fldValue.m_dblVal);	
			break;	
				
		case DBVT_DATE:		
			strVal.Format(_T("%s"), *fldValue.m_pdate);	
				
			if (bsReplace)	
				strVal.Replace(_T("\\"), _T("\\\\"));
				
			if (eReplace)	
				strVal.Replace(sFind, sReplace);
				
			strVal = strEnclose.Left(1) + strVal + strEnclose.Right(1);	
			break;	
				
				
		case DBVT_STRING:		
			strVal.Format(_T("%s"), *fldValue.m_pstring);	

			if (bsReplace)	
				strVal.Replace(_T("\\"), _T("\\\\"));
				
			if (eReplace) {	
				strVal.Replace(sFind, sReplace);
			}	
			strVal = strEnclose.Left(1) + strVal + strEnclose.Right(1);	
			break;	
				
		case DBVT_BINARY:		
			strVal.Format(_T("%s"), *fldValue.m_pbinary);	
			
			if (bsReplace)	
				strVal.Replace(_T("\\"), _T("\\\\"));
				
			if (eReplace)	
				strVal.Replace(sFind, sReplace);
			strVal = strEnclose.Left(1) + strVal + strEnclose.Right(1);	
			break;	
				
		case DBVT_ASTRING:		
			strVal.Format(_T("%s"), (CString) *fldValue.m_pstringA);	

			if (bsReplace)	
				strVal.Replace(_T("\\"), _T("\\\\"));
				
			if (eReplace)	
				strVal.Replace(sFind, sReplace);
			strVal = strEnclose.Left(1) + strVal + strEnclose.Right(1);	
				
			break;	
				
		case DBVT_WSTRING:		
			strVal.Format(_T("%s"), (CString) *fldValue.m_pstringW);	

			if (bsReplace)	
				strVal.Replace(_T("\\"), _T("\\\\"));
				
			if (eReplace)	
				strVal.Replace(sFind, sReplace);
			strVal = strEnclose.Left(1) + strVal + strEnclose.Right(1);	
			break;	
				
		default:		
			strVal.Format(_T("%s"), *fldValue.m_pstring);	

			if (bsReplace)	
				strVal.Replace(_T("\\"), _T("\\\\"));
				
			if (eReplace)	
				strVal.Replace(sFind, sReplace);
			strVal = strEnclose.Left(1) + strVal + strEnclose.Right(1);	
			break;	
		}
		
		return strVal;
	
	}
	
	CATCH(CDBException, e)
	{
		return e->m_strError;
	}
	
	END_CATCH

}	

int CSSODBCrs::GetFldVal(USHORT j, CDBVariant varVal)
{
	TRY
	{
		CRecordset::GetFieldValue(j, varVal);

		return 1;
	}
	
	CATCH(CDBException, e)
	{
		AfxMessageBox(e->m_strError);
		return e->m_nRetCode;
	}
	
	END_CATCH

}

_variant_t CSSODBCrs::GetFieldVarVal(USHORT j)
{
	_variant_t retVal;

	CDBVariant currValue;
	CRecordset::GetFieldValue(j, currValue);

	switch(currValue.m_dwType) {
	
	case DBVT_NULL:
		retVal = (short) NULL;
		break;

	case DBVT_BOOL:
		retVal = (short) currValue.m_boolVal;
		break;

	case DBVT_UCHAR:
		retVal = (unsigned char) currValue.m_chVal;
		break;

	case DBVT_SHORT:
		retVal = (short) currValue.m_iVal;
		break;

	case DBVT_LONG:
		retVal = (long) currValue.m_lVal;
		break;

	case DBVT_SINGLE:
		retVal = (float) currValue.m_fltVal;
		break;

	case DBVT_DOUBLE:
		retVal = (double) currValue.m_dblVal;
		break;

	case DBVT_DATE:
		retVal = (IDispatch*) currValue.m_pdate;
		break;

	case DBVT_STRING:
		retVal = (IDispatch*) currValue.m_pstring;
		break;

	case DBVT_BINARY:
		retVal = (IDispatch*) currValue.m_pbinary;
		break;

	default:					
		retVal = (short) NULL;
		break;				
						
	}
	
	return retVal;


}

LPCSTR* CSSODBCrs::GetFieldNames()
{

	return FieldName;

}

LPCSTR CSSODBCrs::GetFieldName(USHORT j)
{

	return FieldName[j];

}

SWORD CSSODBCrs::GetFieldSQLType(USHORT j)
{

	return FieldSQLType[j];

}

UDWORD CSSODBCrs::GetFieldPrecision(USHORT j)
{

	return FieldPrecision[j];

}

SWORD CSSODBCrs::GetFieldScale(USHORT j)
{

	return FieldScale[j];

}

SWORD CSSODBCrs::GetFieldNullability(USHORT j)
{

	return FieldNullability[j];

}

HTREEITEM* CSSODBCrs::GetHierRS(CString Table, CString critFld, CString critValue,
		CString labelFld, CString IDFld, CString attrFld, DWORD itemData,
		CTreeCtrl *pTreeCtrl, HTREEITEM hParent, BOOL ltd)
{

	CString sSQL = sCritSQL(Table, critFld, critValue, labelFld, IDFld, attrFld, ltd);
		
	RunSQL(sSQL);

	if ( m_nRecords > 0 ) {
		MoveFirst();
	}

	HTREEITEM* hlvl = new HTREEITEM[m_nRecords];
	//0 alloted for the parent


	CString lvlLabel, lvlID, lvlItem, lvlAttr;

	for (ULONG i=0; i<m_nRecords; i++)
	{			
		lvlLabel = SQLFldValue( ConstChar(labelFld) );
		lvlID = SQLFldValue( ConstChar(IDFld) );
		lvlAttr = SQLFldValue( ConstChar(attrFld) );
		lvlItem = lvlLabel + " - " + lvlID;

		if (pTreeCtrl!=NULL && hParent!=NULL) {
			hlvl[i] = pTreeCtrl->InsertItem(TVIF_TEXT,
				(lvlItem), 0, 0, 0, 0, 0, hParent, NULL);

			pTreeCtrl->SetItemData(hlvl[i], itemData);		

		}
		else {
			hlvl[i] = NULL;
		}
		
		MoveNext();

	}

	return hlvl;

}

int CSSODBCrs::PutValuesTo(CComboBox *pComboBox, 
							 LPCSTR labelFld, LPCSTR IDFld, LPCSTR attrFld, 
							 LPCSTR sSQL)
{
	if (sSQL!=NULL) {
		CString sRunSQL;
		sRunSQL.Format(_T("%s"), (CString) sSQL);
		RunSQL(sRunSQL);
	}

	pComboBox->ResetContent();

	int iCB, iDW;

	if ( m_nRecords > 0 ) {
		MoveFirst();
	}

	CString itmLabel, itmID, itmAttr;

	for (UINT i=0; i<m_nRecords; i++)
	{		
		itmLabel = SQLFldValue(labelFld);
		itmID = SQLFldValue(IDFld);

		if (pComboBox!=NULL) {
			iCB = pComboBox->AddString(itmLabel);
			iDW = pComboBox->SetItemData(iCB, _ttol((itmID)));
		}

		MoveNext();
	}

	return iDW;


}
void CSSODBCrs::GetHierRS(CString Table, CString critFld, CString critValue,
			   CString labelFld, CString IDFld, CString attrFld, 
			   CComboBox *pComboBox, BOOL ltd)
{

	CString sSQL = sCritSQL(Table, critFld, critValue, labelFld, IDFld, attrFld, ltd);
	RunSQL(sSQL);

	if (pComboBox!=NULL) 
		pComboBox->ResetContent();

	if ( m_nRecords > 0 ) {
		MoveFirst();
	}

	CString itmLabel, itmID, itmAttr;

	for (UINT i=0; i<m_nRecords; i++)
	{		
		itmLabel = SQLFldValue( ConstChar(labelFld) );
		itmID = SQLFldValue( ConstChar(IDFld) );

		if (pComboBox!=NULL) {
			pComboBox->AddString(itmLabel);
			int cbIndex = pComboBox->SetItemData(i, _ttol((itmID) ) );
		}

		MoveNext();
	}

}


void CSSODBCrs::GetHierRS(CString Table, CString critFld, CString critValue,
		CString labelFld, CString IDFld, CString attrFld, 
		CListCtrl *pListCtrl, BOOL ltd)
{

	CString sSQL = sCritSQL(Table, critFld, critValue, labelFld, IDFld, attrFld, ltd);
		
	RunSQL(sSQL);

	if ( m_nRecords > 0 ) {
		MoveFirst();
	}

	int nCols = 0;

	if (pListCtrl!=NULL) {

		nCols = pListCtrl->GetHeaderCtrl()->GetItemCount();		

	}
	
	CString lvlLabel, lvlID, lvlItem, lvlAttr;
	for (ULONG i=0; i<m_nRecords; i++)
	{			
		
		lvlLabel = SQLFldValue( ConstChar(labelFld) );
		lvlID = SQLFldValue( ConstChar(IDFld) );
		lvlAttr = SQLFldValue( ConstChar(attrFld) );
		lvlItem = lvlLabel + " - " + lvlID;
	
		if (pListCtrl!=NULL) {
			pListCtrl->InsertItem( i, (lvlLabel) );
			
			for (int j=1; j < nCols; j++) {
				
				pListCtrl->SetItemText(i, j, (lvlAttr) );
			}
		}

		MoveNext();
	}

}

varvec2d* CSSODBCrs::GetRSArray()
{
	return value_array;
}

varvec2d* CSSODBCrs::GetRSArray(CString Table, CString critFld, CString critValue,
		CString labelFld, CString IDFld, CString attrFld, 
		UINT scale, BOOL ltd, LPCSTR **strFill)
{

	CString sSQL = sCritSQL(Table, critFld, critValue, labelFld, IDFld, attrFld, ltd);

	RunSQL(sSQL, readOnly, TRUE, scale, strFill);
	
	return value_array;

}

void CSSODBCrs::BuildArray(UINT scale, LPCSTR **strFill)
{
	value_array = new varvec2d;
	//pRSArrBldr = new CSSArrayBuilder(STR_ARR, 2);

	ULONG i;
	UINT j, k;

	if (m_nRecords > 0) {
		//pRSArrBldr->SetDim(DIM1, m_nRecords + scale);
		//pRSArrBldr->SetDim(DIM2, m_nFields);
		(*value_array).resize(m_nRecords + scale);

		MoveFirst();
		
		for (i=scale; i<m_nRecords + scale; i++) {
			(*value_array)[i].resize(m_nFields);
			for (j=0; j<m_nFields; j++) {
				(*value_array)[i][j] = ConstChar(SQLFldValue(j));
				//pRSArrBldr->PutVal(i, j, ConstChar(SQLFldValue(j)));
			}
			MoveNext();
		}
		
		for (k=0; k<scale; k++) {
			for (j=0; j<m_nFields; j++) {
				if (strFill==NULL) {
					(*value_array)[k][j] = "...";
					//pRSArrBldr->PutVal(k, j, _MBCS("..."));
				}
				else {
					//pRSArrBldr->PutVal(k, j, strFill[k][j]);
					//(*value_array)[k][j] = ConstChar( strFill[k][j] ); //not this because row k's cols has not been initialized
					(*value_array)[k].push_back(( strFill[k][j] ));
				}
			}
		}
	}


}
BOOL CSSODBCrs::Exists(LPCSTR sSearchItem, int idxField)
{
	BOOL found = FALSE;
	
	if (m_nRecords<=0)
		return FALSE;

	MoveFirst();

	for (long i=0; i<m_nRecords; i++) {
		CString sCrit(sSearchItem);
		if (sCrit==SQLFldValue(idxField))
			return TRUE;

		MoveNext();

	}

	return found;
}
BOOL CSSODBCrs::Exists(LPCSTR sSearchItem, LPCSTR sSearchField)
{
	int iFld = GetFldIndex(sSearchField);
	return Exists( sSearchItem, iFld );

}
