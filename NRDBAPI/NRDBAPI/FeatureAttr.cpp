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
#include "nrdbapi.h"
#include "FeatureAttr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFeatureAttrSet

IMPLEMENT_DYNAMIC(CFeatureAttrSet, CRecordsetExt)

CFeatureAttrSet::CFeatureAttrSet(CAttrArray* pArray, CNRDBase* pdb)
	: CRecordsetExt(pdb)
{
	//{{AFX_FIELD_INIT(CFeatureAttrSet)
	m_FEATURE_ID = 0;
	m_LDATE = 0;
	m_ORDER = 0;	
	m_nFields = 3;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;

   m_pArray = pArray;
   
   // Adjust to take into account types with more than one field

   if (m_pArray != NULL)
   {
	   for (int i = 0; i < m_pArray->GetSize(); i++)
	   {
	      m_nFields += m_pArray->GetAt(i)->GetNumFields();
	   };
   };
}

CFeatureAttrSet::~CFeatureAttrSet()
{   
};

CString CFeatureAttrSet::GetDefaultConnect()
{
	return _T("ODBC;DSN=BOHOL");
}

///////////////////////////////////////////////////////////////////////////////
//
// Instead of returning table name as 'from' part of SQL, perform a subquery
// that implements the date condition using where and group by, e.g.
// latest/earliest (between dates) or 
// after or before dates
//
// select * from municipality_data as table1 join (select feature_id, 
// max(ldate) as ldate from municipality_data group by feature_id) as table2 on 
// table1.feature_id = table2.feature_id and table1.ldate = table2.ldate;
//

CString CFeatureAttrSet::GetDefaultSQL()
{
   CString sTable;
   CString sStart, sEnd;

#ifdef NRDBMYSQL
   CString sIF = "if";
#else
   CString sIF = "iif";
#endif

   if (m_iFlag != BDADD && m_iFlag != BDUPDATE && m_iFlag != BDDELETE)
   {
      // Convert year or year/month to eight bytes

      CString sDate = sIF + "(ldate<10000,ldate*10000,"+sIF+"(ldate<1000000,ldate*100,ldate))";
      CString sDate1 = sIF + "(table1.ldate<10000,table1.ldate*10000,"+sIF+"(table1.ldate<1000000,table1.ldate*100,table1.ldate))";
      CString sDate2 = sDate + " as ldate2";
   
   
      // Min/max dates
 
      if (m_nDateCond == CFeatureAttrSet::earliest) sDate2 = "min(" + sDate + ") as ldate2";
      else if (m_nDateCond == CFeatureAttrSet::latest) sDate2 = "max(" + sDate + ") as ldate2";

      // Join tables based on feature_id and ldate
   
      sTable = "(select * from `" + m_sFType + "` as table1 inner join (select `FEATURE_ID` as fid2, " + sDate2 + " from ";
      sTable += "`" + m_sFType += "` ";

      // Where clause

      if (m_lStartDate != 0 && m_lEndDate != 0)
      {
         long lStartDate = m_lStartDate < 10000 ? m_lStartDate * 10000 : (m_lStartDate < 1000000 ? m_lStartDate * 100 : m_lStartDate);
         long lEndDate = m_lEndDate < 10000 ? m_lEndDate * 10000 : (m_lEndDate < 1000000 ? m_lEndDate * 100 : m_lEndDate);

         sStart.Format("%d",lStartDate);
         sEnd.Format("%d",lEndDate);

         sTable += " where ";
         if (m_lStartDate != 0) sTable += sDate + " >= " + sStart;
         if (m_lStartDate != 0 && m_lEndDate) sTable += " and ";
         if (m_lEndDate != 0) sTable += sDate+ " <= " + sEnd;
      }

      // Group by

      if (m_nDateCond == CFeatureAttrSet::earliest || m_nDateCond == CFeatureAttrSet::latest) sTable += " group by `FEATURE_ID`";

      sTable += ") as table2 on table1.feature_id = table2.fid2 and " + sDate1 + "= table2.ldate2) as table3";
   }

   // Not a select query

   else 
   {
      sTable = "`" + m_sFType + "`";   
   }
   
	return sTable;
}

///////////////////////////////////////////////////////////////////////////////

void CFeatureAttrSet::DoFieldExchange(CFieldExchange* pFX)
{
   CString sCol;   

	//{{AFX_FIELD_MAP(CFeatureAttrSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Long(pFX, _T("`FEATURE_ID`"), m_FEATURE_ID);
	RFX_Long(pFX, _T("`LDATE`"), m_LDATE);	
	//}}AFX_FIELD_MAP

   if (m_pDBase->GetDBVersion() < 3)
   {
      RFX_Long(pFX, _T("`ORDER`"), m_ORDER);   	
   } else
   {
      RFX_Long(pFX, _T("`NRDB_ORDER`"), m_ORDER);   	
   }

   // Perform data exchange on the bound variables

   for (int i = 0; m_pArray != NULL && i < m_pArray->GetSize(); i++)
   {
      CAttribute* pAttr = m_pArray->GetAt(i);           
      sCol.Format("[%s]", pAttr->m_sColName);   
      switch (pAttr->GetDataType())
      {                  
         case BDNUMBER :  RFX_Double(pFX, sCol, pAttr->GetDoubleInt());
            break;
		   case BDLINK : RFX_Long(pFX, sCol, pAttr->GetLinkInt());
            break;
         case BDTEXT :             
               RFX_Text(pFX, sCol, pAttr->GetStringInt());
            break;
         case BDMAPLINES : case BDIMAGE : case BDLONGTEXT : case BDFILE : 
            RFX_LongBinary(pFX, sCol, pAttr->GetLongBinaryInt());            
               
               if ((m_iFlag == BDUPDATE && pFX->m_nOperation == CFieldExchange::MarkForUpdate) ||
				   (m_iFlag == BDADD && pFX->m_nOperation == CFieldExchange::MarkForAddNew))
               {              
			         SetFieldDirty(&pAttr->GetLongBinaryInt(), TRUE);                  			   
                  SetFieldNull(&pAttr->GetLongBinaryInt(), !pAttr->GetLongBinaryInt().m_hData);
               };            
			break;
		  case BDCOORD :
           RFX_Double(pFX, "`"+pAttr->m_sColName+"_x`", pAttr->GetCoordInt().x);   
			  RFX_Double(pFX, "`"+pAttr->m_sColName+"_y`", pAttr->GetCoordInt().y);   
			break;
		  case BDBOOLEAN :
			  RFX_Bool(pFX, sCol, pAttr->GetBooleanInt());
			  break;
        case BDDATE :
           RFX_Long(pFX, sCol, pAttr->GetDateInt());
           break;
        case BDHOTLINK : 
           RFX_Text(pFX, sCol, pAttr->GetFileInt());
           break;
		 default: ASSERT(FALSE);

      }
   }
   DoDataExchange(GetVar);
}

/////////////////////////////////////////////////////////////////////////////

void CFeatureAttrSet::DoDataExchange(int iFlag)
{   

   DATA_EXCHANGE(m_LDATE, m_plDate, iFlag);
   DATA_EXCHANGE(m_ORDER, m_plOrder, iFlag);
   DATA_EXCHANGE(m_FEATURE_ID, m_plFeature, iFlag);

   // Exchange between bound database values and values pointed to

   for (int i = 0; m_pArray != NULL && i < m_pArray->GetSize(); i++)
   {    
      CAttribute* pAttr = m_pArray->GetAt(i);
      switch (pAttr->GetDataType())
      {        
         case BDNUMBER :                                     
            DATA_EXCHANGE(pAttr->GetDoubleInt(), pAttr->GetDouble(), iFlag);                    
            break;
		 case BDLINK : 
            DATA_EXCHANGE(pAttr->GetLinkInt(), pAttr->GetLink(), iFlag);            
			break;
         case BDTEXT : 
            DATA_EXCHANGE_STR(pAttr->GetStringInt(), pAttr->GetString(), BD_SHORTSTR, iFlag);            
            break;		 
         case BDMAPLINES : case BDIMAGE : case BDFILE :  
            DATA_EXCHANGE_LONGBIN(pAttr->GetLongBinaryInt(), pAttr->GetLongBinary(), iFlag);            
			break;
         case BDLONGTEXT : 
			DATA_EXCHANGE_LONGTEXT(pAttr->GetLongBinaryInt(), pAttr->GetLongText(), iFlag);            
			break;
		 case BDCOORD:
			   DATA_EXCHANGE(pAttr->GetCoordInt(), pAttr->GetCoord(), iFlag);
			   break;
		 case BDBOOLEAN:
            DATA_EXCHANGE(pAttr->GetBooleanInt(), pAttr->GetBoolean(), iFlag);
			 break;
       case BDDATE : 
            DATA_EXCHANGE(pAttr->GetDateInt(), pAttr->GetBoolean(), iFlag);
			   break;
       case BDHOTLINK : 
            DATA_EXCHANGE(pAttr->GetFileInt(), pAttr->GetFile(), iFlag);
            break;
		 default:
			 ASSERT(FALSE);

			break;                        
      }
   }   
}

/////////////////////////////////////////////////////////////////////////////

CString CFeatureAttrSet::GetFilter()
{
   CString sFilter, s;

   if (m_iFlag == BDSELECT3)
   {
      sFilter.Format("`FEATURE_ID` = %li", *m_plFeature);
   } 
   else if (m_iFlag == BDSELECT2)
   {
      sFilter.Format("`FEATURE_ID` = %li `LDATE` >= %li", *m_plFeature, *m_plDate);
   }  
   else if (m_iFlag == BDSELECT4)
   {
      sFilter.Format("`FEATURE_ID` >= %li and `FEATURE_ID` <= %li", m_lFeatureMin, m_lFeatureMax);
   }

   // Primary key attributes must match
 
   else 
   {
	   sFilter.Format("`FEATURE_ID` = %li and `LDATE` = %li", *m_plFeature, *m_plDate);
	   for (int i = 0; i < m_pArray->GetSize(); i++)
	   {
		   CAttribute* pAttr = m_pArray->GetAt(i);
		   if (pAttr->IsPrimaryKey() && !pAttr->IsNull())
		   {
			  if (pAttr->GetDataType() == BDNUMBER)
			  {
                 s.Format(" and `%s` = %lf", pAttr->m_sColName, *pAttr->GetDouble());
				 sFilter +=s;
			  };			               
		   }
	   }
   }

   // Add conditions for filters

   if (m_sFilter != "")
   {
      if (sFilter != "") sFilter += " and ";
      sFilter += "(" + m_sFilter + ")";
   }
      
   return sFilter;
}

/////////////////////////////////////////////////////////////////////////////

CString CFeatureAttrSet::GetSortOrder()
{
   if (m_pDBase->GetDBVersion() < 3)
   {
      return "`FEATURE_ID`, `LDATE`, `ORDER`";
   } else
   {
      return "`FEATURE_ID`, `LDATE`, `NRDB_ORDER`";
   }
}

/////////////////////////////////////////////////////////////////////////////
// CFeatureAttrSet diagnostics

#ifdef _DEBUG
void CFeatureAttrSet::AssertValid() const
{
	CRecordsetExt::AssertValid();
}

void CFeatureAttrSet::Dump(CDumpContext& dc) const
{
	CRecordsetExt::Dump(dc);
}
#endif //_DEBUG

