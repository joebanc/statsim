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
#include "nrdb.h"
#include "DlgImportCreate2.h"
#include "importtextfile.h"
#include "shapefile.h"
#include "comboboxftype.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

#define SQL_SPATIAL 0x1000

/////////////////////////////////////////////////////////////////////////////

BOOL inline IsNumDataType(int nDataType) {return 
                  nDataType == SQL_NUMERIC || nDataType == SQL_DECIMAL || 
                  nDataType == SQL_SMALLINT || nDataType == SQL_FLOAT || 
                  nDataType == SQL_REAL || nDataType == SQL_DOUBLE ||
                  nDataType == SQL_INTEGER;};

BOOL inline IsTextDataType(int nDataType) {return 
            nDataType == SQL_CHAR || nDataType == SQL_VARCHAR;};

/////////////////////////////////////////////////////////////////////////////
// CDlgImportCreate2 dialog


CDlgImportCreate2::CDlgImportCreate2(CDatabase* pDatabase,  CImportTable* pImportTable, int nShape, int nFileType, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgImportCreate2::IDD, pParent)
{
   m_pImportTable = pImportTable;
   m_pDatabase = pDatabase;
   m_nShape = nShape;
   m_nFileType = nFileType;   

	//{{AFX_DATA_INIT(CDlgImportCreate2)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

///////////////////////////////////////////////////////////////////////////////

CDlgImportCreate2::~CDlgImportCreate2()
{   
}

///////////////////////////////////////////////////////////////////////////////

void CDlgImportCreate2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgImportCreate2)
	DDX_Control(pDX, IDC_DATE, m_cbDate);
   DDX_Control(pDX, IDC_LABEL, m_cbLabel);
	DDX_Control(pDX, IDC_FTYPENAME, m_eFType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgImportCreate2, CDialog)
	//{{AFX_MSG_MAP(CDlgImportCreate2)	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgImportCreate2 message handlers

BOOL CDlgImportCreate2::OnInitDialog() 
{
   CFeatureType ftype;

	CDialog::OnInitDialog();
   
  // Set the name of the new feature type

   m_eFType.SetWindowText(m_pImportTable->m_sTableImport);

   OnSelect();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


///////////////////////////////////////////////////////////////////////////////

void CDlgImportCreate2::OnSelect() 
{
   HSTMT hstmt;      
   UCHAR sAttr[BD_NAMESTR];   
   short n;
   long lType;
   CString s;
   CFeatureType ftype;      

   // Validate existing entries

   m_eFType.GetWindowText(s);
   s.TrimRight();
   if (s.IsEmpty())
   {
      m_eFType.SetFocus();
      return;
   }

   // Determine columns in the file to be imported
   
   // Retrieve the names and types of the columns

   BOOL bOK = SQLAllocStmt(m_pDatabase->m_hdbc, &hstmt) == SQL_SUCCESS;

   CString sTable = m_pImportTable->m_sTableImport;
   if (m_nFileType == CImportDB::shapefile || m_nFileType == CImportDB::dbase)
   {
      CString sPath = GetFilePath(".dbf");      
      sTable = CImportTextFile::GetShortFileTitle(sPath);
   }

   CString sSQL = "select * from [" + sTable + "]";
   if (bOK) bOK = SQLExecDirect(hstmt, (UCHAR*)(LPCSTR)sSQL, sSQL.GetLength()) == SQL_SUCCESS;

   if (bOK)
   {
      int i = 1;
      BOOL bRet = 0;
      do
      {
          bRet = SQLColAttributes(hstmt,  i, SQL_COLUMN_NAME, sAttr, BD_SHORTSTR, &n, &lType);
          
          bRet = SQLColAttributes(hstmt,  i, SQL_COLUMN_TYPE, sAttr, BD_SHORTSTR, &n, &lType);          

          if (bRet == SQL_SUCCESS)
          {
             CColumnType coltype;
             coltype.m_sColName = sAttr;
             coltype.m_nDataType = lType;             
             m_aImportCol.Add(coltype);

             // Add labels

             m_cbLabel.AddString((char*)sAttr);             

             // Add dates

             int index = m_cbDate.AddString((char*)sAttr);
             if (lType == SQL_DATE && m_cbDate.GetCurSel() == CB_ERR) m_cbDate.SetCurSel(index);
          };

          i++;
      } while (bRet == SQL_SUCCESS);
   }

   SQLFreeStmt(hstmt,SQL_DROP);        	

   // Add null option for dates

   int index = m_cbDate.InsertString(0, BDString(IDS_NONE));   
   if (m_cbDate.GetCurSel() == CB_ERR) m_cbDate.SetCurSel(index);

   m_cbLabel.SetCurSel(index);

   // For shapefiles, add the corresponding type

   if (m_nShape == SHPPoint)
   {
       CColumnType coltype;
       coltype.m_sColName = SHAPEFILE_POINTS;
       coltype.m_nDataType = BDCOORD + SQL_SPATIAL;
       m_aImportCol.Add(coltype);       
   }
   if (m_nShape == SHPPolyLine || m_nShape == SHPPolygon)
   {
       CColumnType coltype;
       coltype.m_sColName = SHAPEFILE_POLYLINES;
       coltype.m_nDataType = BDMAPLINES + SQL_SPATIAL;
       m_aImportCol.Add(coltype);             
   };  
}

///////////////////////////////////////////////////////////////////////////////
//
// Determine the file being imported
//

CString CDlgImportCreate2::GetFilePath(LPCSTR sExt)
{
   CString sPath;

   int i = m_pDatabase->GetConnect().Find("DefaultDir=");      
   sPath = m_pDatabase->GetConnect().Mid(i+11);
   sPath = sPath.Left(sPath.Find(";"));

   if (sPath[sPath.GetLength()-1] != '\\') sPath += '\\';   
   sPath += m_pImportTable->m_sTableImport + sExt;

   return sPath;
}

///////////////////////////////////////////////////////////////////////////////

void CDlgImportCreate2::OnOK() 
{
   if (CreateFType())
   {
      UpdateImportList();

      // Don't ask any questions

      m_pImportTable->m_bAutoCreate = TRUE;

      CDialog::OnOK();
   }
};

///////////////////////////////////////////////////////////////////////////////

BOOL CDlgImportCreate2::CreateFType()
{
   BOOL bOK = TRUE;   
   CAttrArray aAttr;
   CFeatureType ftype;
   CFTypeAttr ftypeattr;
   CString s;

   // Start creating ftype definition
   
   BDBeginTrans(BDHandle());

   TRY
   {
   
	// Retrieve name of feature type

   m_eFType.GetWindowText(ftype.m_sInternal);   
   m_eFType.GetWindowText(ftype.m_sDesc);

   // Retrieve column contain date field

   CString sDate;
   int index = m_cbDate.GetCurSel();
   if (index != CB_ERR && index > 0)
   {
      m_cbDate.GetLBText(index, sDate);
      sDate = CQuery::StripBrackets(sDate);
   };

   // Retrieve name of feature name column

   CString sFeature;
   index = m_cbLabel.GetCurSel();
   if (index != CB_ERR)
   {
      m_cbLabel.GetLBText(index, sFeature);      
   };

   ftype.m_lParentFType = 0;
   ftype.m_bManyToOne = 0;

   // Set sector

   ftype.m_lDictionary = 1;

   // Must set this as a feature containing names 

   ftype.m_bManyToOne = 1;
      
      // Save feature type

	BDNextId(BDHandle(), BDFTYPE, 0, &ftype.m_lId);

   if (BDFeatureType(BDHandle(), &ftype, BDADD))
   {
      // Create list of attributes

      int nId = 1;
      int i = 0; for (i = 0; i < m_aImportCol.GetSize(); i++)
      {         
         ftypeattr.m_lFType = ftype.m_lId;

         s = m_aImportCol[i].m_sColName;            
         s = CQuery::StripBrackets(s);
         ftypeattr.m_sColName = s;
         ftypeattr.m_sDesc = s;

         long wType = m_aImportCol[i].m_nDataType;
         if (s == sDate) ftypeattr.m_lDataType = BDDATE;
         else if (wType == SQL_DATE) ftypeattr.m_lDataType = BDTEXT;
         else if (s == sFeature) ftypeattr.m_lDataType = BDFEATURE;
         else if (IsTextDataType(wType)) ftypeattr.m_lDataType = BDTEXT;
         else if (IsNumDataType(wType)) ftypeattr.m_lDataType = BDNUMBER;                     
         else if (wType == SQL_BIT) ftypeattr.m_lDataType = BDBOOLEAN;
         else if (wType & SQL_SPATIAL) ftypeattr.m_lDataType = wType ^ SQL_SPATIAL;
         else ftypeattr.m_lDataType = 0;
         
         if (ftypeattr.m_lDataType > 0)
         {
            ftypeattr.m_lAttrId = nId++;
            bOK = BDFTypeAttr(BDHandle(), &ftypeattr, BDADD);				
         }         
      }
	
      // Create the corresponding table

      if (bOK)
      {
         bOK = BDFTypeCreate(BDHandle(), ftype.m_lId);
      };
   } else
   {
      bOK = FALSE;
   }
   
   }
   CATCH (CDBException, pEx)
   {
      AfxMessageBox(pEx->m_strError);
      bOK = FALSE;
   }
   END_CATCH

   if (bOK)
   {
      BDCommit(BDHandle());
   } else
   {
      BDRollBack(BDHandle());
   }
	
   return bOK;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CDlgImportCreate2::UpdateImportList()
{
   BOOL bOK = TRUE;
   CString s;

   // Store the feature type name

   m_eFType.GetWindowText(m_pImportTable->m_sFType);

   int i = m_cbLabel.GetCurSel();
   if (i != CB_ERR) 
   {
      m_cbLabel.GetLBText(i, s);      
      m_pImportTable->m_sFeature = s;
   };   

   // Get the date column name

   CString sDate;
   int index = m_cbDate.GetCurSel();
   if (index != CB_ERR && index > 0)
   {
      m_cbDate.GetLBText(index, sDate);
      sDate = CQuery::StripBrackets(sDate);
   };

   m_pImportTable->m_aColumn.RemoveAll();

   CImportColumn column;
   for (i = 0; i < m_aImportCol.GetSize(); i++)
   {       
      // Store its name and the name of the attribute into which it
      // will be imported

         s = m_aImportCol[i].m_sColName;         
         s.TrimRight();

         column.m_sNameImport = s;         

      // Match against the feature name
       
         if (s == m_pImportTable->m_sFeature) s += IMPORT_NAME;

      // Store the date column
         
         if (sDate == s) s = IMPORT_DATE;

      // Indicate if the column contains shapefile data

         int wDataType = m_aImportCol[i].m_nDataType;
         column.m_bShapeFile = wDataType > 0 ? !!(wDataType & SQL_SPATIAL) : 0;

      // Set the attribute name
         
         if (column.m_bShapeFile)
         {
            s = CQuery::StripBrackets(s);
            if (wDataType ^ SQL_SPATIAL == BDCOORD) s += IMP_COORDS;
            else if (wDataType ^ SQL_SPATIAL == BDMAPLINES) s += IMP_MAPLINES;
         };

         column.m_sNameAttr = s;

         m_pImportTable->m_aColumn.Add(column);
   }

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////

void CDlgImportCreate2::PostNcDestroy() 
{
   BDHwndDialog() = NULL;
   BDToolTip() = NULL;	

	CDialog::PostNcDestroy();
}

///////////////////////////////////////////////////////////////////////////////

void CDlgImportCreate2::OnMouseMove(UINT nFlags, CPoint point) 
{
	CDialog::OnMouseMove(nFlags, point);
}
