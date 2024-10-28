#pragma once

#include "stdafx.h"
//Define StatSimConn
#ifndef STATSIMCONN
	#define STATSIMCONN
#endif
/////////////////

//mysql: include mysql++ headers
#ifndef MYSQLPP_MYSQLPP_H
	#include <mysql++.h>
#endif
///////////////////

#define COL_NAME 0
#define COL_TYPE 1
#define COL_NUM 2
#define NUMERIC _MBCS("numeric")
#define ALPHA _MBCS("alpha")

class daedata;
class daedict;
class CStatSimRS;

class CStatSimWnd;
class CStatSimBar;
// CSSODBCconn command target
// handler of odbc database

class __declspec(dllexport) CSSODBCconn : public CDatabase
{
public:
	//construction
	CSSODBCconn();
	CSSODBCconn(CString custUser, CString custPwd,
		CString custHost = (CString) "localhost", CString custPort = (CString) "3306", LPCSTR custDB = _MBCS("StatSimCBMS"), bool IsMySQL = FALSE);	//custom user and pwd - default connection
	CSSODBCconn(CString strDriver, CString strUser, CString strPwd,
		CString strServer,  CString strDB,  CString strOpt = (CString) "", bool IsMySQL = FALSE);	//general connection 1
	CSSODBCconn(CString strConn, bool IsMySQL = FALSE);	//general connection 2
	virtual ~CSSODBCconn();

	LPCSTR m_sProgress;
	float m_nPercent;

	//methods
	
	LPCSTR sSQLCreateTable(CStatSimRS * pFromRS, CString TableName, 
		BOOL conservative = TRUE, BOOL testExist = TRUE, BOOL encloseFld = TRUE);	//template function for creating tables in both classes
	void CreateTable(CStatSimRS * pFromRS, CString TableName, 
		BOOL conservative = TRUE, BOOL testExist = TRUE, BOOL encloseFld = TRUE);
	void CreateTable(daedata *pFromDta, LPCSTR* TableName, 
		LPCSTR* pType = NULL, USHORT nTypes = 1, CString strIndex = (CString) "");
	
	void UpdateRecords(CStatSimRS * pFromRS, CString TargetTable, LPCSTR sWhereFld = NULL,
		CStatSimWnd* pLabel = NULL, CStatSimBar* pProgBar = NULL);

	void CreateTable(LPCSTR TargetTable, int nCols, LPCSTR** sColInfo, BOOL testExist = TRUE);

	void InsertRecords(LPCSTR** sArray, LPCSTR TargetTable, 
		long nRows, int nCols, LPCSTR** sColInfo, 
		CStatSimWnd* pLabel = NULL, CStatSimBar* pProgBar = NULL, 
		BOOL fresh = FALSE, BOOL single = FALSE, BOOL ignore = FALSE);

	void InsertRecords(CStatSimRS * pFromRS, CString TargetTable,
		CStatSimWnd* pLabel = NULL, CStatSimBar* pProgBar = NULL, 
		BOOL fresh = FALSE, BOOL single = FALSE, BOOL ignore = FALSE,
		LPCSTR sCritFld = NULL, LPCSTR* sCritVals = NULL, int nItems = 0);

	void InsertRecords(daedata * pFromDta, LPCSTR* TargetTable, 
		LPCSTR* pType = NULL, USHORT nTypes = 1, 
		CStatSimWnd* pLabel = NULL, CStatSimBar* pProgBar = NULL, 
		float limitMB = 1, CString strIndex = (CString) "");	//with rtype restriction

	//void InsertRecords(CStatSimStream * pFromStream, CStatSimRS * pDictRS,
	//	LPCSTR* TargetTable, LPCSTR* pType = NULL, USHORT nTypes = 1, 
	//	CStatSimWnd* pLabel = NULL, CStatSimBar* pProgBar = NULL, 
	//	float limitMB = 1, CString strIndex = "");	//directly from stream with dictionary

	CString CreateSQL(USHORT index);	//returner of sSQL
	short ExecuteSQL(CString sSQL, BOOL prompt = TRUE);	//returner of sSQL; with MySQL version already
	bool IsConnected();
	

private:

	//attributes
	CStringArray sSQL;

	//methods
	void CreateSQL(daedata *pFromDta, LPCSTR* TableName, 
		LPCSTR* pType = NULL, USHORT nTypes = 1, CString strIndex = (CString) "");

	int ConnectMySQL(CString currUser, CString currPwd,
		CString custHost, CString custPort, CString custDB); //MySQL
	int Connect(CString strConn);	//general
	BOOL Connected, m_IsMySQL;

};


//MySQL specific database 
using namespace std;

class __declspec(dllexport) CSSMySQLconn : public mysqlpp::Connection
{

public:
	CSSMySQLconn(bool IsMySQL);	//default
	CSSMySQLconn(CString custUser, CString custPwd,
		CString custHost = (CString) "localhost", CString custPort = (CString) "3306", LPCSTR custDB = _MBCS("StatSimCBMS"),
		bool IsMySQL = true);	//custom user and pwd - default connection
	virtual ~CSSMySQLconn();

	LPCSTR m_sProgress;
	float m_nPercent;

	//methods

	LPCSTR sSQLCreateTable(CStatSimRS * pFromRS, CString TableName, 
		BOOL conservative = TRUE, BOOL testExist = TRUE, BOOL encloseFld = TRUE);	//template function for creating tables in both classes
	short ExecuteSQL(CString sSQL, BOOL prompt = TRUE);	//returner of sSQL
	mysqlpp::StoreQueryResult* OpenSQL(CString sSQL, BOOL prompt = TRUE);	//returner of sSQL

	bool IsConnected();	//tells if a successfull connection has been made

	void CreateTable(CStatSimRS * pFromRS, CString TableName, 
		BOOL conservative = TRUE, BOOL testExist = TRUE, BOOL encloseFld = TRUE);	//create a table from a recordset of dictionary

	void UpdateRecords(CString sFromTab, CString TargetTable, LPCSTR sWhereFld = NULL);
	void UpdateRecords(CStatSimRS * pFromRS, CString TargetTable, LPCSTR sWhereFld = NULL,
		CStatSimWnd* pLabel = NULL, CStatSimBar* pProgBar = NULL);

	void InsertFromSQL(CString sFromSQL, CString TargetTable);
	void InsertRecords(CStatSimRS * pFromRS, CString TargetTable,
		CStatSimWnd* pLabel = NULL, CStatSimBar* pProgBar = NULL, 
		BOOL fresh = FALSE, BOOL single = FALSE, BOOL ignore = FALSE,
		LPCSTR sCritFld = NULL, LPCSTR* sCritVals = NULL, int nItems = 0);	//insert records from a recordset

	void InsertRecords(daedata * pFromDta, LPCSTR* TargetTable, 
		LPCSTR* pType = NULL, USHORT nTypes = 1, 
		CStatSimWnd* pLabel = NULL, CStatSimBar* pProgBar = NULL, 
		float limitMB = 1, CString strIndex = (CString) "");	//with rtype restriction

	void CreateTable(LPCSTR TargetTable, int nCols, LPCSTR** sColInfo, BOOL testExist = TRUE);
	void CreateTable(daedata *pFromDta, LPCSTR* TableName, 
		LPCSTR* pType = NULL, USHORT nTypes = 1, CString strIndex = (CString) "");

	void CreateTable(daedict **pDCT, LPCSTR* pTables,
		LPCSTR* pType = NULL, USHORT nTypes = 1, CString strIndex = (CString) "");

	void InsertRecords(LPCSTR** sArray, LPCSTR TargetTable, 
		long nRows, int nCols, LPCSTR** sColInfo, 
		CStatSimWnd* pLabel = NULL, CStatSimBar* pProgBar = NULL, 
		BOOL fresh = FALSE, BOOL single = FALSE, BOOL ignore = FALSE);

	void InsertRecords(LPCSTR sPath, daedict **pDCT, LPCSTR* pTables, int rtLen,
		LPCSTR* pType = NULL, USHORT nTypes = 1, 
		CStatSimWnd* pLabel = NULL, CStatSimBar* pProgBar = NULL, 
		float limitMB = 1, CString strIndex = (CString) "");	//with rtype restriction

	CString CreateSQL(USHORT index);	//returner of sSQL


private:

	//attributes
	CStringArray sSQL;

	mysqlpp::Query* SSQuery;
	
	const bool m_IsMySQL;

	mysqlpp::Row* SSRow;
	mysqlpp::StoreQueryResult* SSResult;

	//methods
	int Connect(CString currUser, CString currPwd,
		CString custHost, CString custPort, CString custDB); //MySQL

	void CreateSQL(daedata *pFromDta, LPCSTR* TableName, 
		LPCSTR* pType = NULL, USHORT nTypes = 1, CString strIndex = (CString) "");
	void CreateSQL(daedict **pDCT, LPCSTR* pTables, 
		LPCSTR* pType = NULL, USHORT nTypes = 1, CString strIndex = (CString) "");


};

//handler/wrapper of the two databases
class __declspec(dllexport) CStatSimConn
{

public:
	CStatSimConn();	//default
	CStatSimConn(CString custUser, CString custPwd,
		CString custHost = (CString) "localhost", CString custPort = (CString) "3306", LPCSTR custDB = _MBCS("StatSimCBMS"),
		bool IsODBC = TRUE, bool IsMySQL = TRUE);	//custom user and pwd - default connection
	CStatSimConn(CString strConn, bool IsMySQL = FALSE);	//string connection version
	virtual ~CStatSimConn();

	CSSODBCconn* GetODBCdb();
	CSSMySQLconn* GetMySQLdb();

	//methods

	LPCSTR sSQLCreateTable(CStatSimRS * pFromRS, CString TableName, 
		BOOL conservative = TRUE, BOOL testExist = TRUE, BOOL encloseFld = TRUE);	//template function for creating tables in both classes
	void CreateTable(CStatSimRS * pFromRS, CString TableName, 
		BOOL conservative = TRUE, BOOL testExist = TRUE, BOOL encloseFld = TRUE);
	void CreateTable(daedata *pFromDta, LPCSTR* TableName, 
		LPCSTR* pType = NULL, USHORT nTypes = 1, CString strIndex = (CString) "");
	void CreateTable(daedict **pDCT, LPCSTR* pTables,
		LPCSTR* pType = NULL, USHORT nTypes = 1, CString strIndex = (CString) "");
	
	void UpdateRecords(CStatSimRS * pFromRS, CString TargetTable, LPCSTR sWhereFld = NULL,
		CStatSimWnd* pLabel = NULL, CStatSimBar* pProgBar = NULL);

	void UpdateRecords(CString sFromTab, CString TargetTable, LPCSTR sWhere);

	void CreateTable(LPCSTR TargetTable, int nCols, LPCSTR** sColInfo, BOOL testExist = TRUE);

	void InsertRecords(LPCSTR** sArray, LPCSTR TargetTable, 
		long nRows, int nCols, LPCSTR** sColInfo, 
		CStatSimWnd* pLabel = NULL, CStatSimBar* pProgBar = NULL, 
		BOOL fresh = FALSE, BOOL single = FALSE, BOOL ignore = FALSE);

	//sCriFld, sCritVals, nItems are the set of arguments that will test if a record will be imported or not
	void InsertFromSQL(CString sFromSQL, CString TargetTable);

	void InsertRecords(CStatSimRS * pFromRS, CString TargetTable,
		CStatSimWnd* pLabel = NULL, CStatSimBar* pProgBar = NULL, 
		BOOL fresh = FALSE, BOOL single = FALSE, BOOL ignore = FALSE,
		LPCSTR sCritFld = NULL, LPCSTR* sCritVals = NULL, int nItems = 0);

	void InsertRecords(daedata * pFromDta, LPCSTR* TargetTable, 
		LPCSTR* pType = NULL, USHORT nTypes = 1, 
		CStatSimWnd* pLabel = NULL, CStatSimBar* pProgBar = NULL, 
		float limitMB = 1, CString strIndex = (CString) "");	//with rtype restriction

	void InsertRecords(LPCSTR sPath, daedict **pDCT, LPCSTR* pTables, int rtLen,
		LPCSTR* pType = NULL, USHORT nTypes = 1, 
		CStatSimWnd* pLabel = NULL, CStatSimBar* pProgBar = NULL, 
		float limitMB = 1, CString strIndex = (CString) "");	//with rtype restriction

	bool IsODBC();
	bool IsMySQL();

	//Get progress attributes
	LPCSTR GetCurProgress()
	{
		if (m_IsODBC) {
			return pODBCdb->m_sProgress;
		}
		else {
			return pMySQLdb->m_sProgress;
		}
	}
	float GetCurPercent()
	{
		if (m_IsODBC) {
			return pODBCdb->m_nPercent;
		}
		else {
			return pMySQLdb->m_nPercent;
		}
	}

	//void InsertRecords(CStatSimStream * pFromStream, CStatSimRS * pDictRS,
	//	LPCSTR* TargetTable, LPCSTR* pType = NULL, USHORT nTypes = 1, 
	//	CStatSimWnd* pLabel = NULL, CStatSimBar* pProgBar = NULL, 
	//	float limitMB = 1, CString strIndex = "");	//directly from stream with dictionary

	CString CreateSQL(USHORT index);	//returner of sSQL
	short ExecuteSQL(CString sSQL, BOOL prompt = TRUE);	//returner of sSQL; with MySQL version already
	BOOL IsConnected();	//tells success in connection

	bool GetTableList(std::vector<LPCSTR> &tables);

private:
	bool m_IsODBC, m_IsMySQL;
	CSSODBCconn* pODBCdb;
	CSSMySQLconn* pMySQLdb;

};


