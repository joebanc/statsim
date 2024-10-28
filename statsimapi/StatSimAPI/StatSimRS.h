#pragma once

//Define StatSimRS
#ifndef STATSIMRS
	#define STATSIMRS
#endif
///////////////////

#ifndef MYSQLPP_MYSQLPP_H
	#include <mysql++.h>
#endif

#ifndef STATSIMDAELIB
	#include "statsimdaelib.h"
#endif


#define RS_BOF 0
#define RS_EOF -1

//sometimes a dummy!
enum SSRSOpenOptions
{
		none =                      0x0,
		readOnly =                  0x0004,
		appendOnly =                0x0008,
		skipDeletedRecords =        0x0010, // turn on skipping of deleted records, Will slow Move(n).
		noDirtyFieldCheck =         0x0020, // disable automatic dirty field checking
		useBookmarks =              0x0100, // turn on bookmark support
		useMultiRowFetch =          0x0200, // turn on multi-row fetch model
		userAllocMultiRowBuffers =  0x0400, // if multi-row fetch on, user will alloc memory for buffers
		useExtendedFetch =          0x0800, // use SQLExtendedFetch with forwardOnly type recordsets
		executeDirect =             0x2000, // Directly execute SQL rather than prepared execute
		optimizeBulkAdd =           0x4000, // Use prepared HSTMT for multiple AddNews, dirty fields must not change.
		firstBulkAdd =              0x8000, // INTERNAL to MFC, don't specify on Open.
};
CString sCritSQL(CString Table, CString critFld, CString critValue,
				 CString labelFld, CString IDFld, CString attrFld, BOOL ltd = FALSE);	//return SQL with criteria


//MySQL Version

class CStatSimConn;

class __declspec(dllexport) CSSMySQLrs
{
	
public:

	CSSMySQLrs(mysqlpp::Connection* pConn);
	CSSMySQLrs(mysqlpp::Connection* pConn, LPCSTR table, BOOL bldArray = FALSE,
		SSRSOpenOptions dbOpenOpt = none);
	CSSMySQLrs(mysqlpp::Connection* pConn, CString sSQL, BOOL bldArray = FALSE,
		SSRSOpenOptions dbOpenOpt = none);
	virtual ~CSSMySQLrs();

	//methods

	//navigation
	void SetRowPos(int pos);
	void MoveNext();	//just like CRecordset::MoveNext()
	void MovePrevious();
	void MoveFirst();

	short RunSQL(CString sSQL, SSRSOpenOptions dbOpenOpt = readOnly,
		BOOL bldArray = FALSE, UINT scale = 0, LPCSTR **strFill = NULL);

	//Fields
	LPCSTR* GetFieldNames();
	LPCSTR GetFieldName(USHORT col);
	short GetFieldSQLType(USHORT col);
	unsigned long GetFieldPrecision(USHORT col);
	short GetFieldScale(USHORT col);
	short GetFieldNullability(USHORT col);
	//CDBVariant GetFldVal(USHORT col);	//overloaded get field value function

	CString SQLFldType(UINT col, BOOL conservative = TRUE);
	CString SQLFldValue(UINT col, CString strEnclose = _T(""), 
		BOOL bsReplace = TRUE, BOOL eReplace = TRUE);
	CString SQLFldValue(LPCSTR strFld, CString strEnclose = _T(""),
		BOOL bsReplace = TRUE, BOOL eReplace = TRUE);

	//hierarchical
	varvec2d* GetRSArray(CString Table, CString critFld, CString critValue,
		CString labelFld, CString IDFld, CString attrFld, 
		UINT scale = 1, BOOL ltd = TRUE, LPCSTR **strFill = NULL);

	varvec2d* GetRSArray();

	void GetHierRS(CString Table, CString critFld, CString critValue,
		CString labelFld, CString IDFld, CString attrFld, 
		CComboBox *pComboBox = NULL, BOOL ltd = FALSE);

	void GetHierRS(CString Table, CString critFld, CString critValue,
		CString labelFld, CString IDFld, CString attrFld, 
		CListCtrl *pListCtrl = NULL, BOOL ltd = FALSE);

	int PutValuesTo(CComboBox *pComboBox, 
		LPCSTR labelFld, LPCSTR IDFld, LPCSTR attrFld, 
		LPCSTR sSQL = NULL);

	HTREEITEM* GetHierRS(CString Table, CString critFld, CString critValue,
		CString labelFld, CString IDFld, CString attrFld, DWORD itemData,
		CTreeCtrl *pTreeCtrl = NULL, HTREEITEM hParent = NULL, BOOL ltd = FALSE);

	int GetFldIndex(LPCSTR sFld);

	//returns true if either query or result set is still not null
	bool IsOpen();

	//if past the last record
	bool IsEOF();
	long GetRecordCount();
	int GetFieldCount();

	// test if a string exists in a field
	BOOL Exists(LPCSTR sSearchItem, LPCSTR sSearchField);
	BOOL Exists(LPCSTR sSearchItem, int idxField);

	void Close();


private:

	//data members
	ULONG m_nRecords;
	UINT m_nFields;

	//methods
	int Init(mysqlpp::Connection* pConn);

	//attributes
	//zero based current index of the recordset
	long curr_i;
	mysqlpp::Connection* SSCon;
	mysqlpp::Query* SSQuery;
	
	mysqlpp::StoreQueryResult::iterator i;
	mysqlpp::Row* SSRow;
	mysqlpp::StoreQueryResult* SSResult;

	UINT m_Scale;
	LPCSTR* FieldName;
	mysqlpp::mysql_type_info* FieldSQLType;

	varvec2d* value_array;

	void BuildArray(UINT scale = 0, LPCSTR **strFill = NULL);

};

//ODBC version
class __declspec(dllexport) CSSODBCrs : public CRecordset
{

public:
	//construction
	CSSODBCrs(CDatabase* pStatSimConn);
	CSSODBCrs(CDatabase* pStatSimConn, LPCSTR table, BOOL bldArray = FALSE,
		CRecordset::OpenOptions dbOpenOpt = none);	
	CSSODBCrs(CDatabase* pStatSimConn, CString sSQL, BOOL bldArray = FALSE, 
		CRecordset::OpenOptions dbOpenOpt = none);
		
	virtual ~CSSODBCrs();

	//methods
	virtual CString GetDefaultConnect();    // Default connection string
	virtual CString GetDefaultSQL();    // Default SQL for Recordset

	short RunSQL(CString sSQL, CRecordset::OpenOptions dbOpenOpt = readOnly,
		BOOL bldArray = FALSE, UINT scale = 0, LPCSTR **strFill = NULL);
	void Close();
	void AddRecords(CString TargetTable);

	CString SQLFldType(USHORT col, BOOL conservative = TRUE);
	CString SQLFldValue(USHORT col, CString strEnclose = (CString) "", 
		BOOL bsReplace = TRUE, BOOL eReplace = TRUE);
	CString SQLFldValue(LPCSTR strFld, CString strEnclose = (CString) "",
		BOOL bsReplace = TRUE, BOOL eReplace = TRUE);
	//CLongBinary, etc

	//Fields
	LPCSTR* GetFieldNames();

	LPCSTR GetFieldName(USHORT col);
	SWORD GetFieldSQLType(USHORT col);
	UDWORD GetFieldPrecision(USHORT col);
	SWORD GetFieldScale(USHORT col);
	SWORD GetFieldNullability(USHORT col);
	int GetFldVal(USHORT col, CDBVariant varVal);	//overloaded get field value function

	//hierarchical
	varvec2d* GetRSArray(CString Table, CString critFld, CString critValue,
		CString labelFld, CString IDFld, CString attrFld, 
		UINT scale = 1, BOOL ltd = TRUE, LPCSTR **strFill = NULL);

	varvec2d* GetRSArray();

	void GetHierRS(CString Table, CString critFld, CString critValue,
		CString labelFld, CString IDFld, CString attrFld, 
		CComboBox *pComboBox = NULL, BOOL ltd = FALSE);

	void GetHierRS(CString Table, CString critFld, CString critValue,
		CString labelFld, CString IDFld, CString attrFld, 
		CListCtrl *pListCtrl = NULL, BOOL ltd = FALSE);

	int PutValuesTo(CComboBox *pComboBox, 
		LPCSTR labelFld, LPCSTR IDFld, LPCSTR attrFld, 
		LPCSTR sSQL = NULL);

	HTREEITEM* GetHierRS(CString Table, CString critFld, CString critValue,
		CString labelFld, CString IDFld, CString attrFld, DWORD itemData,
		CTreeCtrl *pTreeCtrl = NULL, HTREEITEM hParent = NULL, BOOL ltd = FALSE);

	int GetFldIndex(LPCSTR sFld);
	long GetRecordCount();
	int GetFieldCount();


	//recordset navigation
	void SetRowPos(int pos);

	//test existence
	BOOL Exists(LPCSTR sSearchItem, LPCSTR sSearchField);
	BOOL Exists(LPCSTR sSearchItem, int idxField);

private:

	//data members
	ULONG m_nRecords;
	UINT m_nFields;
	CODBCFieldInfo FieldInfo;

	UINT m_Scale;

	LPCSTR* FieldName;
	SWORD* FieldSQLType;
	UDWORD* FieldPrecision;
	SWORD* FieldScale;
	SWORD* FieldNullability;
	_variant_t* FieldValue;

	varvec2d* value_array;

	//methods

	_variant_t GetFieldVarVal(USHORT col);
	void BuildArray(UINT scale = 0, LPCSTR **strFill = NULL);
	void InitmPtrs();
	//short GetFieldValue(int i, CDBVariant fldValue, BOOL prompt = TRUE);
	//short GetFieldValue(LPCSTR sFld, CDBVariant fldValue, BOOL prompt = TRUE);
	//short GetODBCFieldInfo(int i, CODBCFieldInfo fldInfo, BOOL prompt = TRUE);
	//short GetODBCFieldInfo(LPCSTR sFld, CODBCFieldInfo fldInfo, BOOL prompt = TRUE);
};

// CStatSimRS command target

class __declspec(dllexport) CStatSimRS
{
public:
	//construction
	CStatSimRS(CStatSimConn* pStatSimConn);
	CStatSimRS(CStatSimConn* pStatSimConn, LPCSTR table, BOOL bldArray = FALSE,
		SSRSOpenOptions dbOpenOpt = none);	
	CStatSimRS(CStatSimConn* pStatSimConn, CString sSQL, BOOL bldArray = FALSE, 
		SSRSOpenOptions dbOpenOpt = none);

	//overloaded ODBC constructor
	CStatSimRS(CDatabase* pStatSimConn);
	CStatSimRS(CDatabase* pStatSimConn, LPCSTR table, BOOL bldArray = FALSE,
		CRecordset::OpenOptions dbOpenOpt = (CRecordset::OpenOptions) none);	
	CStatSimRS(CDatabase* pStatSimConn, CString sSQL, BOOL bldArray = FALSE, 
		CRecordset::OpenOptions dbOpenOpt = (CRecordset::OpenOptions) none);
		
	//overloaded MySQL constructor
	CStatSimRS(mysqlpp::Connection* pStatSimConn);
	CStatSimRS(mysqlpp::Connection* pStatSimConn, LPCSTR table, BOOL bldArray = FALSE,
		SSRSOpenOptions dbOpenOpt = none);	
	CStatSimRS(mysqlpp::Connection* pStatSimConn, CString sSQL, BOOL bldArray = FALSE, 
		SSRSOpenOptions dbOpenOpt = none);

	virtual ~CStatSimRS();

	//methods
	virtual CString GetDefaultConnect();    // Default connection string
	virtual CString GetDefaultSQL();    // Default SQL for Recordset

	short RunSQL(CString sSQL, SSRSOpenOptions dbOpenOpt = readOnly,
		BOOL bldArray = FALSE, UINT scale = 0, LPCSTR **strFill = NULL);
	void Close();
	void AddRecords(CString TargetTable);

	CString SQLFldType(USHORT col, BOOL conservative = TRUE);
	CString SQLFldValue(USHORT col, CString strEnclose = (CString) "", 
		BOOL bsReplace = TRUE, BOOL eReplace = TRUE);
	CString SQLFldValue(LPCSTR strFld, CString strEnclose = (CString) "",
		BOOL bsReplace = TRUE, BOOL eReplace = TRUE);
	//CLongBinary, etc

	//Fields
	LPCSTR* GetFieldNames();


	LPCSTR GetFieldName(USHORT col);
	SWORD GetFieldSQLType(USHORT col);
	UDWORD GetFieldPrecision(USHORT col);
	SWORD GetFieldScale(USHORT col);
	SWORD GetFieldNullability(USHORT col);

	//hierarchical
	varvec2d* GetRSArray(CString Table, CString critFld, CString critValue,
		CString labelFld, CString IDFld, CString attrFld, 
		UINT scale = 1, BOOL ltd = TRUE, LPCSTR **strFill = NULL);

	varvec2d* GetRSArray();

	void GetHierRS(CString Table, CString critFld, CString critValue,
		CString labelFld, CString IDFld, CString attrFld, 
		CComboBox *pComboBox = NULL, BOOL ltd = FALSE);

	void GetHierRS(CString Table, CString critFld, CString critValue,
		CString labelFld, CString IDFld, CString attrFld, 
		CListCtrl *pListCtrl = NULL, BOOL ltd = FALSE);

	int PutValuesTo(CComboBox *pComboBox, 
		LPCSTR labelFld, LPCSTR IDFld, LPCSTR attrFld, 
		LPCSTR sSQL = NULL);

	HTREEITEM* GetHierRS(CString Table, CString critFld, CString critValue,
		CString labelFld, CString IDFld, CString attrFld, DWORD itemData,
		CTreeCtrl *pTreeCtrl = NULL, HTREEITEM hParent = NULL, BOOL ltd = FALSE);

	int GetFldIndex(LPCSTR sFld);

	bool IsOpen();
	bool IsEOF();
	long GetRecordCount();
	int GetFieldCount();


	//recordset navigation
	void SetRowPos(int pos);
	void MoveNext();
	void MovePrevious();
	void MoveFirst();

	void Refresh();

	// test existence
	BOOL Exists(LPCSTR sSearchItem, LPCSTR sSearchField);
	BOOL Exists(LPCSTR sSearchItem, int idxField);

private:

	//data members
	ULONG m_nRecords;
	UINT m_nFields;

	bool m_IsODBC, m_IsMySQL;
	CSSODBCrs* pODBCrs;
	CSSMySQLrs* pMySQLrs;

	UINT m_Scale;

	LPCSTR* FieldName;
	SWORD* FieldSQLType;
	UDWORD* FieldPrecision;
	SWORD* FieldScale;
	SWORD* FieldNullability;
	_variant_t* FieldValue;

	BOOL m_wArray;

};
