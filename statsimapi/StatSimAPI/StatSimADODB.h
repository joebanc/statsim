#pragma once

// StatSimADODB.h: interface for the CStatSimADODB class.
//
//////////////////////////////////////////////////////////////////////
#import "C:\Program Files\Common Files\System\ado\msado15.dll" \
	rename( "EOF", "adoEOF" ) 

using namespace ADODB;

class CStatSimADODB  
{
public:
	CStatSimADODB();
	CStatSimADODB(LPCSTR dbTable);
	CStatSimADODB(LPCSTR dbTable, ADODB::LockTypeEnum);
	CStatSimADODB(LPCSTR custConn, LPCSTR custUser, LPCSTR custPwd);

	virtual ~CStatSimADODB();
	void Connect();
	void Connect(LPCSTR TargetTable, ADODB::LockTypeEnum dbLockType);
	void Connect(LPCSTR currConn, LPCSTR currUser, LPCSTR currPwd);
	//void ConstructDB(CString fileExt);		//constructor of necessary tables
	//void ConfigDB();
	void OpenQuery(LPCSTR sSQL);
	void OpenQuery(LPCSTR sSQL, ADODB::LockTypeEnum dbLockType);
	void OpenQuery(CString TargetTable, CString CritFld, CString CritVal);
	void ExecQuery(LPCSTR sSQL);

	void CreateTable(_RecordsetPtr fromRS, CString TableName);
	void AddHHRecords(CString fromPath);
	void AddMemRecords(CString fromPath);
	void AddRecords(_RecordsetPtr fromRS);
	void AddRecords(_RecordsetPtr fromRS, CString TargetTable);
	void DeleteRecords(CString TargetTable, CString CritFld, CString CritVal);
	void DeleteRecords(CString TargetTable);
	void ExportRecords(CString sSQL, CString targetPath);

	//void CompactDB();

public:
	//My generated attributes
	//_ptr pJE;
	_RecordsetPtr pDBRS;
	_ConnectionPtr pDBConn;
	_CommandPtr pDBCmd;
	int RecCount;
	int FldCount;
	CWnd *pWnd;
	CProgressCtrl *pProgCtrl;

//private methods
private:
	void ConnectToDict(CString TableName);	//connector to dict table

//protected methods
protected:
	void TransferTable(CString fromTable, CString toTable);	//Transfer table from dict table

private:
	_RecordsetPtr pDictRS;
	_ConnectionPtr pDictConn;
	_CommandPtr pDictCmd;
	int dictRecCount;		//dict table record count
	int dictFldCount;		//dict table field count

};
