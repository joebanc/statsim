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

#include "stdafx.h"
#include "mysqldbase.h"
#include "nrdbase.h"

///////////////////////////////////////////////////////////////////////////////

#ifdef NRDBMYSQL

IMPLEMENT_DYNAMIC(CDBException, CException)

#define BUFFER_SIZE 8096

///////////////////////////////////////////////////////////////////////////////

void AFXAPI RFX_Text(CFieldExchange* pFX, LPCTSTR szName, CNRDBString& value,
	int nMaxLength, int nColumnType, short nScale)
{  
   
   if (pFX->m_nOperation == CFieldExchange::BindParam)
   {
      value.SetLength(BD_SHORTSTR);      
   }   
   else if (pFX->m_nOperation == CFieldExchange::StoreField)
   {
	  // Quote any quotes!

	  //CString s = value;
	  //s.Replace("'","\\'");
	  //value = s;

      // For writing, set the length of the data to be written

      *pFX->m_pBind[pFX->m_iField].length = strlen((LPCSTR)value);  
   }    
   else if(pFX->m_nOperation == CFieldExchange::LoadField)
   {
      if (*pFX->m_pBind[pFX->m_iField].is_null)
      {
         value = "";
      }
   }
   pFX->Default(szName, (void*)(LPCSTR)value, MYSQL_TYPE_VAR_STRING, BD_SHORTSTR);         
}

void AFXAPI RFX_Long(CFieldExchange* pFX, LPCTSTR szName, long& value)
{
   if (pFX->m_nOperation == CFieldExchange::StoreField)
   {
      if (value == AFX_RFX_LONG_PSEUDO_NULL)
      {
         *pFX->m_pBind[pFX->m_iField].is_null = TRUE;
      }
   }
   pFX->Default(szName, &value, MYSQL_TYPE_LONG, sizeof(value));
}

void AFXAPI RFX_Bool(CFieldExchange* pFX, LPCTSTR szName, BOOL& value)
{
   pFX->Default(szName, &value, MYSQL_TYPE_TINY, sizeof(value));

}

void AFXAPI RFX_Int(CFieldExchange* pFX, LPCTSTR szName, int& value)
{   
   pFX->Default(szName, &value, MYSQL_TYPE_LONG, sizeof(value));
}

void AFXAPI RFX_Double(CFieldExchange* pFX, LPCTSTR szName, double& value)
{
   pFX->Default(szName, &value, MYSQL_TYPE_DOUBLE, sizeof(value));
}

///////////////////////////////////////////////////////////////////////////////

void AFXAPI RFX_LongBinary(CFieldExchange* pFX, LPCTSTR szName, CLongBinary& value)
{     
   void* pData = NULL;  
  
   if (pFX->m_nOperation == CFieldExchange::LoadField)
   {
      // Now that length has been retrieved, allocate memory

      int nLength = *pFX->m_pBind[pFX->m_iField].length;
      if (value.m_hData != NULL)
      {
         GlobalFree(value.m_hData);
         value.m_hData = NULL;
      }   
      
      if (nLength > 0)
      {
         value.m_hData = GlobalAlloc(GMEM_MOVEABLE|GMEM_DISCARDABLE, nLength);     
         value.m_dwDataLength = nLength;
         pData = GlobalLock(value.m_hData);
         
         if (pData == NULL) 
		 {
			 AfxThrowMemoryException();
		 }

         long lOffset = 0;
         while (lOffset < nLength)
         {
            pFX->m_pBind[pFX->m_iField].buffer = ((BYTE*)pData)+lOffset;
            pFX->m_pBind[pFX->m_iField].buffer_length = min(BUFFER_SIZE, nLength-lOffset);

            if (mysql_stmt_fetch_column(pFX->m_prs->m_stmt, &pFX->m_pBind[pFX->m_iField], pFX->m_iField, lOffset) != 0)
            {
               AfxThrowDBException(0, pFX->m_prs->m_pDBase, pFX->m_prs->m_stmt);
            }
            lOffset += BUFFER_SIZE;            
         };
         GlobalUnlock(value.m_hData);
      };
   }

   // Pass data to database in chunks

   else if (pFX->m_nOperation == CFieldExchange::StoreField)
   {
      if (value.m_dwDataLength > 0)
      {
         BYTE *pData = (BYTE*)GlobalLock(value.m_hData);
         if (pData != NULL)
         {
             UINT lOffset = 0;
             while (lOffset < value.m_dwDataLength)
             {
                int nLength = min(BUFFER_SIZE, value.m_dwDataLength-lOffset);
                if (mysql_stmt_send_long_data(pFX->m_prs->m_stmt, pFX->m_iField, (const char*)(pData+lOffset), nLength) != 0)
                {
                   AfxThrowDBException(0, pFX->m_prs->m_pDBase, pFX->m_prs->m_stmt);
                }
                lOffset += BUFFER_SIZE;
             };
             GlobalFree(value.m_hData);
			 value.m_hData = NULL;
         } else
         {
            AfxThrowMemoryException();
         }
      }
   }

   // Set size of field to zero so that only the length is returned
   pFX->Default(szName, NULL, MYSQL_TYPE_BLOB, 0);
   
}

///////////////////////////////////////////////////////////////////////////////

CFieldExchange::CFieldExchange(int nOp, CMySQLRecordset *pRecordset)
{
   m_nOperation = nOp;
   m_prs = pRecordset;

   m_pBind = new MYSQL_BIND[m_prs->m_nFields];
   memset(m_pBind, 0, sizeof(MYSQL_BIND)*m_prs->m_nFields);

   m_pIsNull = new my_bool[m_prs->m_nFields];
   m_pLength = new unsigned long[m_prs->m_nFields];

   for (int i = 0; i < m_prs->m_nFields; i++)
   {
      m_pIsNull[i] = FALSE;
      m_pLength[i] = 0;
   }
};

CFieldExchange::~CFieldExchange()
{
   if (m_pBind != NULL) delete [] m_pBind;
   if (m_pIsNull != NULL) delete [] m_pIsNull;
   if (m_pLength != NULL) delete [] m_pLength;

}

void CFieldExchange::SetFieldType(UINT nFieldType)
{
   m_iField = 0;
}

///////////////////////////////////////////////////////////////////////////////

void CFieldExchange::Default(LPCTSTR szName,	void* pv, enum_field_types nType, SIZE_T nSize)
{
   switch (m_nOperation)
   {
      case Name:            
         m_asParams.Add(szName);
         break;

      case CFieldExchange::BindParam : 
         m_pBind[m_iField].buffer_type = nType;         
         m_pBind[m_iField].buffer = (char*)pv;
         m_pBind[m_iField].is_null = &m_pIsNull[m_iField];                          
         m_pBind[m_iField].length = &m_pLength[m_iField];
         m_pBind[m_iField].buffer_length = nSize;                          
         break;
   };
   m_iField++;
}

///////////////////////////////////////////////////////////////////////////////

CDBException::CDBException(RETCODE nRetCode)
{
   this->m_nRetCode = nRetCode;
}

void AFXAPI AfxThrowDBException(RETCODE nRetCode, CMySQLDatabase* pdb, MYSQL_STMT *pStmt)
{
	CDBException* pException = new CDBException(nRetCode);

   if (pStmt != NULL)
   {
      pException->m_strError = mysql_stmt_error(pStmt);
   }
   else if (pdb != NULL)
   {
      pException->m_strError = mysql_error(pdb->GetMySQL());
   };
   ((CNRDBase*)pdb)->m_sError = pException->m_strError;
	
	THROW(pException);
}

BOOL CDBException::GetErrorMessage(LPTSTR lpszError, UINT nMaxError, PUINT pnHelpContext)
{
   if (m_strError != "")
   {
      strncpy(lpszError, m_strError, nMaxError);
      return TRUE;
   } else
   {
      return FALSE;
   }  
}

/////////////////////////////////////////////////////////////////////

CMySQLDatabase::CMySQLDatabase()
{
   m_bConnect = FALSE;
}

/////////////////////////////////////////////////////////////////////
//
// Retrieves list of MySQL databases
//

BOOL BDGetDataSourceNames(HANDLE &handle, CString &sDSN, LPCSTR sUser, LPCSTR sPassword, LPCSTR sHost)
{       
   MYSQL_ROW row;
   struct CMySQL
   {
      MYSQL m_mysql;
      MYSQL_RES *m_result;
   };

   if (handle == NULL)
   {
      CMySQL *pMySQL = new CMySQL;

      mysql_init(&pMySQL->m_mysql);
      if (mysql_real_connect(&pMySQL->m_mysql, sHost, sUser, sPassword, NULL, 0, NULL, 0))
      {
         pMySQL->m_result = mysql_list_dbs(&pMySQL->m_mysql, NULL);      
         handle = pMySQL;
      } else
      {      
         delete pMySQL;  
         return FALSE;
      }            
   } 

   CMySQL *pMySQL = (CMySQL*)handle;    
   if (row = mysql_fetch_row(pMySQL->m_result))
   {
      sDSN = row[0];      
      return TRUE;
   }
   mysql_free_result(pMySQL->m_result);
   mysql_close(&pMySQL->m_mysql);      
   delete pMySQL;

   return FALSE;
   
}

///////////////////////////////////////////////////////////////////////////////

BOOL BDCreateDatasource(LPCSTR sDSN, LPCSTR sPath, LPCSTR sUser, LPCSTR sPassword, LPCSTR sHost)
{
   BOOL bOK = FALSE;

   MYSQL mysql;
   mysql_init(&mysql);
   if (mysql_real_connect(&mysql, sHost, sUser, sPassword, NULL, 0, NULL, 0))
   {
      if (mysql_query(&mysql, "create database " + CString(sDSN)) == 0)
      {
         bOK = TRUE;
      } else
      {
#ifdef _DEBUG
         AfxMessageBox("Debug: " + CString(mysql_error(&mysql)));
#endif
      }
      mysql_close(&mysql);      
   }              
   return bOK;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CMySQLDatabase::IsOpen() const
{
   return m_bConnect;
}

///////////////////////////////////////////////////////////////////////////////

void CMySQLDatabase::Close( )
{
   mysql_close(&m_mysql);      
   m_bConnect = FALSE;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CMySQLDatabase::Open( LPCTSTR lpszDSN, LPCSTR sUser, LPCSTR sPassword, LPCSTR sHost)
{
   if (!IsOpen())
   {
      mysql_init(&m_mysql);
      if (mysql_real_connect(&m_mysql, sHost, sUser, sPassword, lpszDSN, 0, NULL, 0))
      {
         m_bConnect = TRUE;
         return TRUE;
      }
      
   } else
   {
      return FALSE;
   }
   
   return FALSE;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CMySQLDatabase::CanTransact( ) const
{
   //return FALSE; // Transactions cannot undo 'create table; which is the primary use in NRDB
   return mysql_get_client_version() >= 40100;
}

BOOL CMySQLDatabase::BeginTrans( )
{   
   //return FALSE;
   mysql_commit(&m_mysql);
   return mysql_autocommit(&m_mysql,0);
};

BOOL CMySQLDatabase::CommitTrans( )
{
   //return FALSE;
   return mysql_commit(&m_mysql);
}

BOOL CMySQLDatabase::Rollback( )
{
   //return FALSE;
   return mysql_rollback(&m_mysql);
}

BOOL CMySQLDatabase::GetDriver(CString& sVersion)
{
   sVersion = CString("MySQL ") + mysql_get_client_info();
   return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

void CMySQLDatabase::ExecuteSQL( LPCSTR lpszSQL )
{
   if (mysql_query(&m_mysql, lpszSQL) != 0)
   {
      AfxThrowDBException(0, this, NULL);
   }
}

///////////////////////////////////////////////////////////////////////////////
//
// Executes an SQL statement with an integer return value
//

BOOL BDExecRetSQL(BDHANDLE hConnect, LPSTR sSQL, LPLONG plRet)
{
   CNRDBase* pDBase = GetNRDBase(hConnect); 
   if (pDBase != NULL)
   {
      return pDBase->ExecRetSQL(sSQL, plRet);
   } else
   {
      return FALSE;
   }   
}

BOOL CMySQLDatabase::ExecRetSQL(LPSTR sSQL, LPLONG plRet)
{
   BOOL bOK = FALSE;
   MYSQL_BIND bind[1];
   memset(&bind, 0, sizeof(bind));
   my_bool is_null[1] = {FALSE};
   unsigned long length[1] = {0};
   
   MYSQL_STMT *stmt;
   if (stmt = mysql_stmt_init(&m_mysql))
   {               
      if (mysql_stmt_prepare(stmt, sSQL, strlen(sSQL)) == 0)
      {                       
          if (mysql_stmt_execute(stmt) == 0)
          {             
             bind[0].buffer_type = MYSQL_TYPE_LONG;
             bind[0].buffer = (char*)plRet;
             bind[0].is_null = &is_null[0];             
             bind[0].length = &length[0];
            
             if (mysql_stmt_bind_result(stmt, bind) == 0)
             {
                if (mysql_stmt_fetch(stmt) == NULL)
                {
                   bOK = TRUE;
                }
             };
          }
      }
#ifdef _DEBUG
      else
      {
         CString sError = mysql_error(&m_mysql);
         ASSERT(FALSE);
      }
#endif


      mysql_stmt_close(stmt);
   }

  return bOK;
};

///////////////////////////////////////////////////////////////////////////////

CMySQLRecordset::CMySQLRecordset(CNRDBase* pDBase)
{
   m_pDBase = pDBase;
   m_pMySQL = pDBase->GetMySQL();   
   m_stmt = NULL;
   m_pfx = NULL;
}

CMySQLRecordset::~CMySQLRecordset()
{
   if (m_pfx != NULL) delete m_pfx;
   m_pfx = NULL;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CMySQLRecordset::IsOpen( ) const
{
   return m_stmt != NULL;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CMySQLRecordset::Open()
{  
   int i;
   m_bEOF = TRUE;

     // If an existing statement is open then close it

   if (m_stmt != NULL) Close(); // Note deletes field exchange

   // Create field exchange
      
   if (m_pfx != NULL) delete m_pfx;
   m_pfx = new CFieldExchange(CFieldExchange::Name, this);   
      
  // First create the SQL statement

    m_sSQL = "select ";
   DoFieldExchange(m_pfx);

   for (i = 0; i < m_pfx->m_asParams.GetSize(); i++)
   {
      if (i > 0) m_sSQL += ", ";
      m_sSQL += m_pfx->m_asParams[i];
   };
   m_sSQL += " from " + GetDefaultSQL();

   // add any filter

   if (m_strFilter != "") m_sSQL += " where " + m_strFilter;
   if (m_strGroupBy != "") m_sSQL += " group by " + m_strGroupBy;
   if (m_strSort != "") m_sSQL += " order by " + m_strSort;

   // Substitute square brackets for ``
   
   while ((i = m_sSQL.Find('[')) >= 0) m_sSQL.SetAt(i,'`');
   while ((i = m_sSQL.Find(']')) >= 0) m_sSQL.SetAt(i,'`');
     
   BOOL bOK = FALSE;
   
   if (m_stmt = mysql_stmt_init(m_pMySQL))
   {     
      if (mysql_stmt_prepare(m_stmt, m_sSQL, m_sSQL.GetLength()) == 0)
      {                       
          if (mysql_stmt_execute(m_stmt) == 0)
          {
             // Now create bindings

             m_pfx->m_nOperation = CFieldExchange::BindParam;
             DoFieldExchange(m_pfx);

             if (mysql_stmt_bind_result(m_stmt, m_pfx->m_pBind) != 0)
             {
               AfxThrowDBException(0, m_pDBase, m_stmt);
             }                      

             // Retrieve first values
             int nRet = mysql_stmt_fetch(m_stmt);
             m_bEOF = nRet == MYSQL_NO_DATA;
             bOK = nRet != 1;             

             // Now copy fields across
             
             m_pfx->m_nOperation = CFieldExchange::LoadField;
             DoFieldExchange(m_pfx);
             
             
          }
      };
   }

   if (!bOK)
   {
      AfxThrowDBException(0, this->m_pDBase, NULL);
   };
   return FALSE;
};

///////////////////////////////////////////////////////////////////////////////

void CMySQLRecordset::Close( )
{
   if (m_stmt != NULL)
   {
      mysql_stmt_free_result(m_stmt); 
      mysql_stmt_close(m_stmt);
      m_stmt = NULL;      
   }

   if (m_pfx != NULL) delete m_pfx;
   m_pfx = NULL;   
};

///////////////////////////////////////////////////////////////////////////////

void CMySQLRecordset::SetFieldDirty( void* pv, BOOL bDirty)
{
}

void CMySQLRecordset::SetFieldNull( void* pv, BOOL bNull)
{   
}

BOOL CMySQLRecordset::IsEOF( ) const
{
   return m_bEOF;
}

void CMySQLRecordset::MoveNext( )
{
   //throw( CDBException, CMemoryException );   

   if (mysql_stmt_fetch(m_stmt) == NULL)
   {   
      m_pfx->m_nOperation = CFieldExchange::LoadField;
      DoFieldExchange(m_pfx);
   }
   else
   {
      m_bEOF = TRUE;
   };
   
};

///////////////////////////////////////////////////////////////////////////////

void CMySQLRecordset::AddNew( )
{
   int i;

   if (m_stmt != NULL) Close();     
 
   if (m_pfx != NULL) delete m_pfx;
   m_pfx = new CFieldExchange(CFieldExchange::Name, this);   

   // First create the SQL statement

   m_sSQL = "insert into " + GetDefaultSQL() + " ";
   DoFieldExchange(m_pfx);

   m_sSQL += "(";
   for (i = 0; i < m_pfx->m_asParams.GetSize(); i++)
   {
      if (i > 0) m_sSQL += ", ";
      m_sSQL += m_pfx->m_asParams[i];
   }
   m_sSQL += ")";

   m_sSQL += " VALUES(";
   for (i = 0; i < m_nFields; i++)
   { 
      if (i > 0) m_sSQL += ",";
      m_sSQL += "?";
   }
   m_sSQL += " )";

   // Substitute square brackets for ``
   
   while ((i = m_sSQL.Find('[')) >= 0) m_sSQL.SetAt(i,'`');
   while ((i = m_sSQL.Find(']')) >= 0) m_sSQL.SetAt(i,'`');

   // Initialise new statement

   if (m_stmt = mysql_stmt_init(m_pMySQL))
   {
      if (mysql_stmt_prepare(m_stmt, m_sSQL, m_sSQL.GetLength()) == 0)
      {
         // Now create bindings

          m_pfx->m_nOperation = CFieldExchange::BindParam;
          DoFieldExchange(m_pfx);

          if (mysql_stmt_bind_param(m_stmt, m_pfx->m_pBind) != 0)
          {
             AfxThrowDBException(0, m_pDBase, m_stmt);
          }
      } else
      {
          AfxThrowDBException(0, m_pDBase, m_stmt);
      }
   } else
   {
       AfxThrowDBException(0, m_pDBase, NULL);
   }
};

///////////////////////////////////////////////////////////////////////////////

BOOL CMySQLRecordset::CanUpdate( ) const
{
   return !m_bEOF;
}

///////////////////////////////////////////////////////////////////////////////

void CMySQLRecordset::Edit( )
{   
   int i;

   if (m_stmt != NULL) Close();     
 
   if (m_pfx != NULL) delete m_pfx;
   m_pfx = new CFieldExchange(CFieldExchange::Name, this);   

   // First create the SQL statement

   m_sSQL = "update " + GetDefaultSQL() + " set ";
   DoFieldExchange(m_pfx);

   // TODO check if field is dirty before including in update

   for (i = 0; i < m_pfx->m_asParams.GetSize(); i++)
   {
      if (i > 0) m_sSQL += ", ";
      m_sSQL += m_pfx->m_asParams[i] + " = ? ";
   }
   if (m_strFilter != "") m_sSQL += " where " + m_strFilter;

   // Substitute square brackets for ``
   
   while ((i = m_sSQL.Find('[')) >= 0) m_sSQL.SetAt(i,'`');
   while ((i = m_sSQL.Find(']')) >= 0) m_sSQL.SetAt(i,'`');

   // Initialise new statement

   if (m_stmt = mysql_stmt_init(m_pMySQL))
   {
      if (mysql_stmt_prepare(m_stmt, m_sSQL, m_sSQL.GetLength()) == 0)
      {
         // Now create bindings

          m_pfx->m_nOperation = CFieldExchange::BindParam;
          DoFieldExchange(m_pfx);

          if (mysql_stmt_bind_param(m_stmt, m_pfx->m_pBind) != 0)
          {
             AfxThrowDBException(0, m_pDBase, m_stmt);
          }
      } else
      {
               AfxThrowDBException(0, m_pDBase, m_stmt);
      }
   } else
   {
       AfxThrowDBException(0, m_pDBase, NULL);
   }
};

///////////////////////////////////////////////////////////////////////////////

BOOL CMySQLRecordset::Update( )
{  
   ASSERT(m_pfx != NULL);

  // Perform any additional operations prior to executing

   m_pfx->m_nOperation = CFieldExchange::StoreField;
   DoFieldExchange(m_pfx);

   if (mysql_stmt_execute(m_stmt) != 0)
   {
      AfxThrowDBException(0, m_pDBase, m_stmt);
   }

   my_ulonglong nRows = mysql_stmt_affected_rows(m_stmt);

   mysql_stmt_close(m_stmt);
   m_stmt = NULL;

   return nRows == 1;
};

///////////////////////////////////////////////////////////////////////////////

void CMySQLRecordset::Delete( )
{
   int i;

   if (m_stmt != NULL) Close();     

   m_sSQL = "delete from " + GetDefaultSQL() + " where " + m_strFilter;

     // Substitute square brackets for ``
   
   while ((i = m_sSQL.Find('[')) >= 0) m_sSQL.SetAt(i,'`');
   while ((i = m_sSQL.Find(']')) >= 0) m_sSQL.SetAt(i,'`');

   if (m_stmt = mysql_stmt_init(m_pMySQL))
   {
      if (mysql_stmt_prepare(m_stmt, m_sSQL, m_sSQL.GetLength()) == 0)
      {
         if (mysql_stmt_execute(m_stmt) != 0)
         {
            AfxThrowDBException(0, m_pDBase, m_stmt);
         }
      }
      
   };
};

void CMySQLRecordset::Dump( CDumpContext& dc ) const
{
}

#endif
