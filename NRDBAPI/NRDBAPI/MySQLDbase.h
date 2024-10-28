//////////////////////////////////////////////////////
//
// NRDB Pro - Spatial database and mapping application
//
// Copyright (c) 1989-2004 Richard D. Alexander
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// NRDB Pro is part of the Natural Resources Database Project 
// 
// Homepage: http://www.nrdb.co.uk/
// 

#ifndef _MYSQLDBASE_H_
#define _MYSQLDBASE_H_

#include "datatype.h"

class CMySQLRecordset;
class CNRDBase;
class CMySQLDatabase;
class CFieldExchange;

// Database constants defined in MFC

#ifdef NRDBMYSQL

#define RETCODE INT

#define SQL_SUCCESS                0

// SQL data type codes 
#define	SQL_UNKNOWN_TYPE	0
#define SQL_CHAR            1
#define SQL_NUMERIC         2
#define SQL_DECIMAL         3
#define SQL_INTEGER         4
#define SQL_SMALLINT        5
#define SQL_FLOAT           6
#define SQL_REAL            7
#define SQL_DOUBLE          8
#define SQL_VARCHAR        12

#define SQL_PARAM_TYPE_UNKNOWN           0
#define SQL_PARAM_INPUT                  1
#define SQL_PARAM_INPUT_OUTPUT           2
#define SQL_RESULT_COL                   3
#define SQL_PARAM_OUTPUT                 4
#define SQL_RETURN_VALUE                 5

///////////////////////////////////////////////////////////////////////////////

void Hex2Bin(BYTE*, BYTE*, int len);
void Bin2Hex(BYTE *dest, BYTE *src, int len);

///////////////////////////////////////////////////////////////////////////////

class DLLEXPORT CMySQLDatabase : public CObject
{
public:
   CMySQLDatabase();

   void ExecuteSQL( LPCSTR lpszSQL );
   BOOL IsOpen() const;
   virtual void Close( );
   virtual BOOL Open( LPCTSTR lpszDSN, LPCSTR sUser = NULL, LPCSTR sPassword = NULL, LPCSTR sHost = NULL); 
   BOOL CanTransact( ) const;
   BOOL BeginTrans( );
   BOOL CommitTrans( );
   BOOL Rollback( );
   BOOL GetDriver(CString& sVersion);
   BOOL ExecRetSQL(LPSTR sSQL, LPLONG plRet);
   MYSQL* GetMySQL() {return &m_mysql;}

    DECLARE_DYNAMIC(CMySQLDatabase); 

protected:
   MYSQL m_mysql;
   BOOL m_bConnect;

};

///////////////////////////////////////////////////////////////////////////////

class DLLEXPORT CMySQLRecordset : public CObject
{
public:

   CMySQLRecordset();
   CMySQLRecordset(CNRDBase* pDBase);
   ~CMySQLRecordset();

   void SetFieldDirty( void* pv, BOOL bDirty = TRUE );
   void SetFieldNull( void* pv, BOOL bNull = TRUE );
   BOOL IsEOF( ) const;
   void MoveNext( );//throw( CDBException, CMemoryException );
   BOOL IsOpen( ) const;
   virtual BOOL Open(); //throw( CDBException, CMemoryException );
   virtual void Close( );
   virtual void AddNew( ); // throw( CDBException );
   BOOL CanUpdate( ) const;
   virtual void Edit( ); // throw( CDBException, CMemoryException );
   virtual BOOL Update( ); // throw( CDBException );
   virtual void Delete( ); // throw( CDBException );

   virtual void DoFieldExchange(CFieldExchange* pFX) = 0;
   virtual CString GetDefaultSQL() = 0;    // Default SQL for Recordset
   
   virtual void Dump( CDumpContext& dc ) const;
   DECLARE_DYNAMIC(CMySQLRecordset);

   enum {snapshot};
   enum {none};

   int m_nFields;
   int m_nDefaultType;
   CString m_strFilter;
   CString m_strSort;
   CString m_strGroupBy;
   MYSQL* m_pMySQL;
   MYSQL_STMT *m_stmt;      
   CMySQLDatabase *m_pDBase;
   CString m_sSQL;
   BOOL m_bEOF;
   CFieldExchange *m_pfx;
};

///////////////////////////////////////////////////////////////////////////////

class DLLEXPORT CDBException : public CException
{
	DECLARE_DYNAMIC(CDBException)

// Attributes
public:
	RETCODE m_nRetCode;
	CString m_strError;
	CString m_strStateNativeOrigin;

// Implementation (use AfxThrowDBException to create)
public:
	CDBException(RETCODE nRetCode = SQL_SUCCESS);
	
	//virtual ~CDBException();

	virtual BOOL GetErrorMessage(LPTSTR lpszError, UINT nMaxError,
		PUINT pnHelpContext = NULL);
};

class DLLEXPORT CFieldExchange
{
public:
   CFieldExchange(int nOp, CMySQLRecordset *pRecordset);
   ~CFieldExchange();

// Attributes
public:
	enum RFX_Operation
	{
		BindParam,          // register users parameters with ODBC SQLBindParameter
		RebindParam,        //  migrate param values to proxy array before Requery
		BindFieldToColumn,  // register users fields with ODBC SQLBindCol
		BindFieldForUpdate, // temporarily bind columns before update (via SQLSetPos)
		UnbindFieldForUpdate,   // unbind columns after update (via SQLSetPos)
		Fixup,              // Set string lengths, clear status bits
		MarkForAddNew,      // Prepare fields and flags for addnew operation
		MarkForUpdate,      // Prepare fields and flags for update operation
		Name,               // append dirty field name
		NameValue,          // append dirty name=value
		Value,              // append dirty value or parameter marker
		SetFieldNull,       // Set status bit for null value
		StoreField,         // archive values of current record
		LoadField,          // reload archived values into current record
		AllocCache,         // allocate cache used for dirty field check
		AllocMultiRowBuffer,    // allocate buffer holding multi rows of data
		DeleteMultiRowBuffer,   // delete buffer holding multi rows of data
	};

   enum FieldType
	{
		noFieldType     = -1,
		outputColumn    = 0,
		param           = SQL_PARAM_INPUT,
		inputParam      = param,
		outputParam     = SQL_PARAM_OUTPUT,
		inoutParam      = SQL_PARAM_INPUT_OUTPUT,
	};

	UINT m_nOperation;  // Type of exchange operation
	CMySQLRecordset* m_prs;  // recordset handle

   CStringArray m_asParams;
   MYSQL_BIND *m_pBind;
   my_bool *m_pIsNull;
   unsigned long *m_pLength;
   int m_iField;

   void Default(LPCTSTR szName,	void* pv, enum_field_types nType, SIZE_T nSize);
   void SetFieldType(UINT nFieldType);

   
};



void AFXAPI RFX_Text(CFieldExchange* pFX, LPCTSTR szName, CNRDBString& value,
	// Default max length for char and varchar, default datasource type
	int nMaxLength = 255, int nColumnType = SQL_VARCHAR, short nScale = 0);

void AFXAPI RFX_Long(CFieldExchange* pFX, LPCTSTR szName, long& value);
void AFXAPI RFX_Bool(CFieldExchange* pFX, LPCTSTR szName, BOOL& value);
void AFXAPI RFX_Int(CFieldExchange* pFX, LPCTSTR szName, int& value);
void AFXAPI RFX_Double(CFieldExchange* pFX, LPCTSTR szName, double& value);
void AFXAPI RFX_LongBinary(CFieldExchange* pFX, LPCTSTR szName, CLongBinary& value);

void AFXAPI AfxThrowDBException(RETCODE nRetCode, CMySQLDatabase* pdb, MYSQL_STMT *pStmt);

#endif
#endif