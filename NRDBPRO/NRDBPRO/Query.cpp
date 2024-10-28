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
#include "query.h"
#include "bdimportexport.h"
#include "bdattribute.h"
#include "projctns.h"
#include "maplayer.h"
#include "spatial.h"

///////////////////////////////////////////////////////////////////////////////

#define SQUARE(x) x*x
#define GROUP -100
#define STATISTIC -101
#define COUNT -102
#define COMPDOUBLE 1e-10
#define MAXFILTERLEN 4096
#define MAXQUERYIDS 1024

///////////////////////////////////////////////////////////////////////////////

CQueryDate CQuery::m_aQueryDates[] = {{AllDates, "All data"},
                           {FirstDate, "Earliest data"}, 
                           {LastDate, "Latest data"},
                           {FirstDateRange, "First data in range"}, 
                           {LastDateRange, "Last data in range"}, 
                           {AllDatesRange, "All data in range"}, 
                           {AllDatesAfter, "All data after"},
                           {AllDatesBefore, "All data before"},{-1,""}};

///////////////////////////////////////////////////////////////////////////////

CQuery::CQuery()
{
   m_lFType = 0;
   SetDateCond(LastDate);
   SetShowAllFeatures(FALSE);
}

///////////////////////////////////////////////////////////////////////////////

CQuery::~CQuery()
{
   CQueryElement::Delete(m_pNext);   
   CQueryJoin::Delete(m_pQueryJoin);
}

///////////////////////////////////////////////////////////////////////////////

CQuery::CQuery(CQuery& rSrc) : CQueryElement(rSrc)
{   
   *this = rSrc;
}

///////////////////////////////////////////////////////////////////////////////
//
// Initialise a query using the results from a standard report dialog
//

CQuery::CQuery(long lFType, CLongArray& alAttr, CLongArray& alFeatures, int nSortBy)
{
   // Convert the list of attributes to a list of attribute selections (includes
   // feature type)

   CArrayAttrSel aAttr;
   int i = 0; for (i = 0; i < alAttr.GetSize(); i++)
   {
      CQueryAttrSel attrsel;
      attrsel.m_lFType = lFType;
      attrsel.m_lAttr = alAttr[i];
      aAttr.Add(attrsel);
   }

   *this = CQuery(lFType, aAttr, alFeatures, nSortBy);
}

///////////////////////////////////////////////////////////////////////////////

CQuery::CQuery(long lFType, CArrayAttrSel& aAttr, CLongArray& alFeatures,  int nSortBy)
{   
  // Initialise query
   
   Initialise(lFType);   

   // Set date condition

   BOOL bDate = FALSE;
   int i = 0; for (i = 0; i < aAttr.GetSize(); i++)
   {
      if (aAttr[i].m_lAttr == BDDATE)
	  {
		  bDate = TRUE;
		  break;
	  }
   }
   if (bDate) SetDateCond(CQuery::AllDates);
   else SetDateCond(CQuery::LastDate);

   // Select attributes to be displayed

   CQueryElement* pQueryElement = this;   
   while (pQueryElement != NULL)
   {      
        // Select feature

		 int i = 0; for (i = 0; i < aAttr.GetSize(); i++)
		 {         			 
          if (pQueryElement->GetFTypeId() == aAttr[i].m_lFType)
          {                   
             if ((pQueryElement->GetAttrId() == aAttr[i].m_lAttr &&
                 pQueryElement->GetDataType() != BDFEATURE) ||
                  (pQueryElement->GetDataType() == BDFTYPE && 
                   aAttr[i].m_lAttr == BDFEATURE)) 
			    {
				   pQueryElement->SetSelected();				
			    }
          
		       if (pQueryElement->GetAttrId() == nSortBy)
			    {
			       pQueryElement->SetSortBy(1);
			    };
          };
		 }
      
      pQueryElement = pQueryElement->GetNextQuery();
	}

   // Sort the list of features to optimise searching

   alFeatures.Sort();

   // Update the selected features

   CQueryElement* pElement = this;
   while (pElement != NULL)
   {
      if (pElement->GetDataType() == BDFEATURE)
      {
         CQueryLink* pLink = (CQueryLink*)pElement;
         int i = 0; for (i = 0; i < pLink->m_aFeatures.GetSize(); i++)
         {
            if (alFeatures.FindSorted(pLink->m_aFeatures[i].m_lId) != -1)
			{
				pLink->m_aFeatures[i].SetSelected(TRUE);
			} else
			{
				pLink->m_aFeatures[i].SetSelected(FALSE);
			}
         }

      }
      pElement = pElement->GetNextQuery();
   }
}

///////////////////////////////////////////////////////////////////////////////

CQuery& CQuery::operator=(CQuery& rSrc)
{
   CQueryElement::Delete(m_pNext);
   CQueryJoin::Delete(m_pQueryJoin);

   m_lFType = rSrc.m_lFType;
   m_nDateCond = rSrc.m_nDateCond;
   m_bShowAllFeatures = rSrc.m_bShowAllFeatures;
   m_dtStart = rSrc.m_dtStart;
   m_dtEnd = rSrc.m_dtEnd;

   (CQueryElement&)*this = (CQueryElement&)rSrc;

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

void CQuery::Initialise(long lFType)
{
   BOOL bInitFeatures = FALSE;
   BOOL bInitDates = FALSE;
   Initialise(lFType, bInitFeatures, bInitDates);
}

///////////////////////////////////////////////////////////////////////////////

void CQuery::Initialise(long lFType, BOOL& bInitFeatures, BOOL& bInitDates)
{
   CAttrArray aAttr;
   CFeatureType ftype;
   CQueryElement* pPrevElement = NULL;

   // Reset dates parameters

   SetDateCond(LastDate);
   SetShowAllFeatures(FALSE);

   // Loads the elements according to the feature type

   CQueryElement::Delete(m_pNext);
   CQueryJoin::Delete(m_pQueryJoin);
   m_lFType = lFType;

   // Retrieve feature type description

   ftype.m_lId = lFType;
   BDFeatureType(BDHandle(), &ftype, BDSELECT);
   BDEnd(BDHandle());
     
   // Retrieve the attributes for the feature type

   if (BDFTypeAttrInit(BDHandle(), lFType, &aAttr))
   {      
      // First element is the feature type
           
      SetDesc(aAttr.GetFTypeName()  + CString(" ["+ BDString(IDS_NAME) + "]"));
      SetDataType(BDFTYPE);      
	   SetFTypeId(lFType);
	   SetAttrId(BDFEATURE);
      pPrevElement = this;
      CQueryElement* pElement = NULL;

      // Next element is the features of the feature type, only provide one
      // list of features

      if (ftype.m_bManyToOne && !bInitFeatures)
      {
         pElement = new CQueryLink(lFType);
         pElement->SetDesc(aAttr.GetFTypeName());         
         pElement->SetDataType(BDFEATURE);                  
         pElement->SetAttrId(BDFEATURE);
         pElement->SetFTypeId(lFType);
         pPrevElement->SetNextQuery(pElement);
         pPrevElement = pElement;         
         bInitFeatures = TRUE;
      };

      // Initialise dates

      if (!bInitDates)
      {
         pElement = new CQueryElement;
         pElement->SetDesc(BDString(IDS_DATE));
         pElement->SetDataType(BDDATE);
         pElement->SetAttrId(BDDATE);
         pElement->SetFTypeId(lFType);
         pPrevElement->SetNextQuery(pElement);
         pPrevElement = pElement;
         bInitDates = TRUE;
      };

      
      // Add all of the attributes

      int i = 0; for (i = 0; i < aAttr.GetSize(); i++)
      {                  
         // Allocate memory

         CAttribute* pAttr = aAttr.GetAt(i);
         switch (pAttr->GetDataType())
         {
         case BDFTYPE : 
            pElement = new CQuery;
            break;
         case BDLINK : 
            pElement = new CQueryLink(pAttr->GetFTypeLink());
            break;
         case BDNUMBER :
            pElement = new CQueryNumber;
            break;
         case BDBOOLEAN : 
            pElement = new CQueryBoolean;
            break;
         case BDTEXT: case BDCOORD : case BDMAPLINES : case BDHOTLINK : case BDIMAGE : 
         case BDLONGTEXT : case BDFILE :
            pElement = new CQueryElement;         
            break;
         default : 
            ASSERT(FALSE);
            
         }

         // Initialise

         CString sDesc = pAttr->GetDesc();
         if (pAttr->GetDataType() == BDMAPLINES) sDesc += " [" + BDString(IDS_POLYLINESTYPE)+"]";
         if (pAttr->GetDataType() == BDCOORD) sDesc += " [" + BDString(IDS_COORDTYPE) + "]";
         pElement->SetDesc(sDesc);
         pElement->SetColName(pAttr->m_sColName);
         pElement->SetDataType(pAttr->GetDataType());
         pElement->SetAttrId(pAttr->GetAttrId());
         pElement->SetFTypeId(lFType);

         // Add the list
      
         pPrevElement->SetNextQuery(pElement);
         pPrevElement = pElement;
      };
   };
   BDEnd(BDHandle());

   // Initialise the parent type   

   if (ftype.m_lParentFType != 0)
   {
      CQuery* pQuery = new CQuery;         
      pQuery->Initialise(ftype.m_lParentFType, bInitFeatures, bInitDates);                           
      pPrevElement->SetNextQuery(pQuery);
   }      
}


///////////////////////////////////////////////////////////////////////////////

/*
void CQuery::Write(FILE* pFile)
{
   fprintf(pFile,"FType=%li\n",GetFTypeId());
   fprintf(pFile,"DateCond=%i\n",GetDateCond());   
   if (GetDateCond() == FirstDateRange || GetDateCond() == LastDateRange || 
       GetDateCond() == AllDatesRange)
   {
      fprintf(pFile,"DateStart=%li\n",GetStartDate().GetDateLong());
      fprintf(pFile,"DateEnd=%li\n",GetEndDate().GetDateLong());
   };
   fprintf(pFile,"ShowAllFeatures=%i\n",IsShowAllFeatures());

   CQueryElement::Write(pFile);
   fprintf(pFile,"end\n");
}
*/

///////////////////////////////////////////////////////////////////////////////
//
// Saves the query object as an XML object so that it can be saved to file
//

void CQuery::AsXML(CXMLObj* pXMLObj)
{
   pXMLObj->SetValue("", "ftype", GetFTypeId());
   pXMLObj->SetValue("", "datecond", GetDateCond());
   if (GetDateCond() == FirstDateRange || GetDateCond() == LastDateRange || 
       GetDateCond() == AllDatesRange)
   {
      pXMLObj->SetValue("", "startdate", GetStartDate());
      pXMLObj->SetValue("", "enddate", GetEndDate());
   }
   if (GetDateCond() == AllDatesAfter || GetDateCond() == AllDatesBefore)
   {
        pXMLObj->SetValue("", "startdate", GetStartDate());
   }

   CQueryElement::AsXML(pXMLObj->AddChild("queryelement"));
   
}

///////////////////////////////////////////////////////////////////////////////
//
// Reconstructs CQuery object from XML (loaded from file)
//


BOOL CQuery::XMLAs(CXMLObj* pXMLObj)
{
   CXMLObj *pXMLChild = NULL;
   CQueryElement* pElement = NULL;
   BOOL bOK = TRUE;
   BOOL bContinue = FALSE;

   // First retrieve the feature type and initiase the CQuery structure from
   // the database

    long lFType = pXMLObj->GetInteger("", "ftype");
    Initialise(lFType);

    // Now retrieve the conditions that apply to the whole query

    SetDateCond(pXMLObj->GetInteger("", "datecond"));

    // Now iterate through each queryelement

    BOOL bFound = pXMLObj->GetFirst("", "queryelement", pXMLChild);
    while (bFound && bOK)
    {
      // To better handle structure changes in the data dictionary, e.g. addition of new
      // attributes, no longer assume that the query elements are in the same positions as in XML

       lFType = pXMLChild->GetInteger("","ftypeid");
       long lAttr = pXMLChild->GetInteger("", "attrid");
       long lDataType = pXMLChild->GetInteger("", "datatype");

       pElement = this;
       while (pElement != NULL)
       {

          // If found then initialise CQueryElement from XML

          if (pElement->GetFTypeId() == lFType && pElement->GetAttrId() == lAttr && pElement->GetDataType() == lDataType)
          {
             if (!pElement->CQueryElement::XMLAs(pXMLChild)) 
             {
                bOK = FALSE;
             }
             break;
          }

          pElement = pElement->GetNextQuery();
       }

       // If not found then warn user and offer opportunity to continue

       if (pElement == NULL && !bContinue)
       {
          if (AfxMessageBox(IDS_CHANGEDATADICT, MB_YESNO) == IDYES)
          {
             bContinue = TRUE;
          } else
          {
             bOK = FALSE;
          }
       }

       bFound = pXMLObj->GetNext("", "queryelement", pXMLChild);
    }

    // Now search through for calculations

    bFound = pXMLObj->GetFirst("", "querycalc", pXMLChild);
    while (bFound && bOK)
    {
       CQueryCalc* pQueryCalc = new CQueryCalc;
       pQueryCalc->XMLAs(pXMLChild);
        
       // Append to the end of the query

       CQueryElement* pElement = this;
       while (pElement->GetNextQuery() != NULL)
       {
          pElement = pElement->GetNextQuery();
       }
       pElement->SetNextQuery(pQueryCalc);

       bFound = pXMLObj->GetNext("", "querycalc", pXMLChild);
    };

   
    return bOK;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CQuery::Read(FILE* pFile)
{  
   BOOL bOK = TRUE;

   CString s = BDNextItem(pFile);
   if (s != "FType") bOK = FALSE;

   if (bOK)
   {
      long lFType = BDNextInt(pFile, FALSE);
      if (GetFTypeId() == 0)
      {
         SetFTypeId(lFType);
         Initialise(GetFTypeId());
      };

      SetDateCond(BDNextInt(pFile));
      if (GetDateCond() == FirstDateRange || GetDateCond() == LastDateRange ||
          GetDateCond() == AllDatesRange)
      {
         SetStartDate(CDateTime(BDNextInt(pFile),0));
         SetEndDate(CDateTime(BDNextInt(pFile), 0));      
      };
      SetShowAllFeatures(BDNextInt(pFile));   
      bOK = CQueryElement::Read(pFile);
   };

   // Add new attribute to the end
      
   while ((s = BDNextItem(pFile)) == "Statistic" && bOK)
   {
      CQueryCalc* pStats = new CQueryCalc;
      CQueryElement* pElement = this;
      pStats->Read(pFile);
      while (pElement->GetNextQuery() != NULL && bOK)
      {
         pElement = pElement->GetNextQuery();
      }
      pElement->SetNextQuery(pStats);
   }

   if (s != "end")
   {
      ASSERT(FALSE);
      bOK = FALSE;
   }

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////
//
// Removes square brackets and their contents from a string
//

CString CQuery::StripBrackets(CString s)
{
   CString sName;
   BOOL bBracket = FALSE;
   int i = 0; for (i = 0; i < s.GetLength(); i++)
   {
      if (s[i] == '[') bBracket = TRUE;
      else if (s[i] == ']') bBracket = FALSE;
      else if (!bBracket) sName += s[i];
   }
   sName.TrimRight();
   return sName;
}

///////////////////////////////////////////////////////////////////////////////
// 
// Read files from version 2.0
//

BOOL CQueryElement::Read(FILE* pFile)
{
   BOOL bOK = TRUE;

   SetDataType(BDNextInt(pFile));
   SetAttrId(BDNextInt(pFile));
   SetFTypeId(BDNextInt(pFile));
   SetSelected(BDNextInt(pFile));
   SetSortBy(BDNextInt(pFile));
   SetGroupBy(BDNextInt(pFile));
   SetGroupStat(BDNextInt(pFile));

   // Retrieve conditions

   CQueryCond condition;
   condition.SetCond(BDNextInt(pFile, "Cond"));
   condition.SetCondValue(BDNextDouble(pFile, "CondValue", 0));
   condition.SetCondValueStr(BDNextStr(pFile, "CondValueStr"));

   m_aCondition.RemoveAll();
   if (condition.GetCond() > CQueryElement::none) m_aCondition.Add(condition);
         
   CQueryElement* pQueryElement = GetNextQuery();
   if (pQueryElement != NULL)
   {
      bOK = pQueryElement->Read(pFile);
   }

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////

/*void CQueryElement::Write(FILE* pFile)
{  
   fprintf(pFile,"DataType=%i\n",GetDataType());
   fprintf(pFile,"AttrId=%i\n",GetAttrId());
   fprintf(pFile,"FtypeId=%i\n",GetFTypeId());         
   fprintf(pFile,"Selected=%i\n",GetSelected());
   fprintf(pFile,"SortBy=%i\n", GetSortBy());
   fprintf(pFile,"GroupBy=%i\n", GetGroupBy());
   fprintf(pFile,"StatGroup=%i\n", GetGroupStat());

   fprintf(pFile,"Cond=%i\n", GetCond());
   fprintf(pFile,"CondValue=%lf\n", GetCondValue());
   fprintf(pFile,"CondValueStr=%s\n",GetCondValueStr());
   
   CQueryElement* pQueryElement = GetNextQuery();
   if (pQueryElement != NULL)
   {
      pQueryElement->Write(pFile);
   }

   // TODO write join
}*/

///////////////////////////////////////////////////////////////////////////////
//
// Convert a query element objects attributes to XML
//

void CQueryElement::AsXML(CXMLObj* pXMLObj)
{
   // Add the attributes

   pXMLObj->SetValue("", "datatype", GetDataType());
   pXMLObj->SetValue("", "attrid", GetAttrId());
   pXMLObj->SetValue("", "ftypeid", GetFTypeId());
   pXMLObj->SetValue("", "selected", GetSelected());
   pXMLObj->SetValue("", "sortby", GetSortBy());
   pXMLObj->SetValue("", "groupby", GetGroupBy());
   pXMLObj->SetValue("", "statgroup", GetGroupStat());

   int i = 0; for (i = 0; i < m_aCondition.GetSize(); i++)
   {
      if (m_aCondition[i].GetCond() != CQueryCalc::none)
      {
         CXMLObj *pXMLChild = pXMLObj->AddChild("condition");
         pXMLChild->SetValue("", "type", m_aCondition[i].GetCond());
         pXMLChild->SetValue("", "value", m_aCondition[i].GetCondValue());
         pXMLChild->SetValue("", "string", m_aCondition[i].GetCondValueStr());
      }
   }

   // Add query link

   if (GetDataType() == BDLINK || GetDataType() == BDFEATURE)
   {
      ((CQueryLink*)this)->CQueryLink::AsXML(pXMLObj); 
   }

   // Add any query join

   if (m_pQueryJoin != NULL)
   {
      m_pQueryJoin->AsXML(pXMLObj->AddChild("queryjoin"));
   }

   // Now add next query element

   if (m_pNext != NULL)
   {
      if (m_pNext->GetDataType() == BDQUERYSTATS)
      {
         ((CQueryCalc*)m_pNext)->CQueryCalc::AsXML(pXMLObj->AddNext("querycalc"));
      } else
      {
         m_pNext->CQueryElement::AsXML(pXMLObj->AddNext("queryelement"));
      }
   }

}

///////////////////////////////////////////////////////////////////////////////
//
// Initialise CQueryElement from XML object
//

BOOL CQueryElement::XMLAs(CXMLObj* pXMLObj)
{
   BOOL bOK = TRUE;
   CXMLObj *pXMLChild = NULL;

   SetDataType(pXMLObj->GetInteger("", "datatype"));
   SetAttrId(pXMLObj->GetInteger("", "attrid"));
   SetFTypeId(pXMLObj->GetInteger("", "ftypeid"));
   SetSelected(pXMLObj->GetInteger("", "selected"));
   SetSortBy(pXMLObj->GetInteger("", "sortby"));
   SetGroupBy(pXMLObj->GetInteger("", "groupby"));
   SetGroupStat(pXMLObj->GetInteger("", "statgroup"));

   if (GetDataType() == BDLINK || GetDataType() == BDFEATURE)
   {
      ((CQueryLink*)this)->CQueryLink::XMLAs(pXMLObj->GetXMLObj("", "querylink", TRUE));
   }

   // Check for conditions

   CQueryCond condition;
   BOOL bFound = pXMLObj->GetFirst("", "condition", pXMLChild);
   while (bFound)
   {
      condition.SetCond(pXMLChild->GetInteger("", "type"));
      condition.SetCondValue(pXMLChild->GetDouble("", "value"));
      condition.SetCondValueStr(pXMLChild->GetString("", "string"));

      m_aCondition.Add(condition);
      
      bFound = pXMLObj->GetNext("", "condition", pXMLChild);
   }

   // Add any query join

   pXMLChild = pXMLObj->GetXMLObj("", "queryjoin");
   if (pXMLChild != NULL)
   {
      m_pQueryJoin = new CQueryJoin;
      m_pQueryJoin->XMLAs(pXMLChild);
   }

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////

CString CQuery::DateCondAsString()
{
   CString sDate;

   int i = 0; for (i = 0; m_aQueryDates[i].m_nID != GetDateCond(); i++);
   
   CString sDesc = m_aQueryDates[i].m_psDescription;
   switch (GetDateCond())
   {
      case FirstDateRange : case LastDateRange : case AllDatesRange :
      case AllDatesAfter : case AllDatesBefore : 
         GetStartDate().DateAsString(sDate);
         sDesc += " " + sDate;
   }
   switch (GetDateCond())
   {
      case FirstDateRange : case LastDateRange : case AllDatesRange : 
         GetEndDate().DateAsString(sDate);
         sDesc += " " + BDString(IDS_TO) + " " + sDate;         
   }
   return sDesc;
   
}

///////////////////////////////////////////////////////////////////////////////

/*void CQueryLink::Write(FILE* pFile)
{  
   fprintf(pFile, "Link=");
   int c = 0;
   int i = 0; for (i = 0; i < m_aFeatures.GetSize(); i++)
   {
      if (m_aFeatures.GetAt(i).GetSelected())
      {
         if (c > 0) fprintf(pFile,",");
         fprintf(pFile,"%li",m_aFeatures.GetAt(i).m_lId);
         c++;
      }      
   }   
   fprintf(pFile,"\n");

   CQueryElement::Write(pFile);
}*/ 

///////////////////////////////////////////////////////////////////////////////

BOOL CQueryLink::Read(FILE* pFile)
{
   CDWordArray adw;
   
   CString sHeader = BDNextItem(pFile);
   if (sHeader != "Link") return FALSE;

   BDNext(pFile, adw, FALSE);  
 
   int i = 0; for (i = 0; i < m_aFeatures.GetSize(); i++)
   {
      CQueryFeature feature = m_aFeatures.GetAt(i);

      feature.SetSelected(FALSE);      
      int j = 0; for (j = 0; j < adw.GetSize(); j++)
      {
         if (m_aFeatures.GetAt(i).m_lId == (long)adw[j])
         {
            feature.SetSelected(TRUE);      
         }
      }	  
      m_aFeatures.SetAt(i, feature);
   }
 
   return CQueryElement::Read(pFile);   
}

///////////////////////////////////////////////////////////////////////////////

void CQueryLink::AsXML(CXMLObj* pXMLObj)
{
   // Produce a string containing the list of selected feature id
   CString sLink, s;
   int i = 0; for (i = 0; i < m_aFeatures.GetSize(); i++)
   {
      if (m_aFeatures.GetAt(i).GetSelected())
      {
         if (sLink != "") sLink = sLink + ",";
         s.Format("%d", m_aFeatures.GetAt(i).m_lId);
         sLink += s;
         
      }      
   }   

   // Save to xml object

   pXMLObj->SetValue("", "querylink", sLink);
   
   // Convert parent object

   //CQueryElement::AsXML(pXMLObj);
}

///////////////////////////////////////////////////////////////////////////////

BOOL CQueryLink::XMLAs(CXMLObj* pXMLObj)
{
   long l;
   CString s;
   int i = 0;
   BOOL bOK = TRUE;

   CString sLink = pXMLObj->m_sValue;

   // Sort features for faster searching

   Sort();
   for (i = 0; i < m_aFeatures.GetSize(); i++)
   {
      m_aFeatures[i].SetSelected(FALSE);
   }

   // Read each number until comma or end of string

   i = 0;
   while (i <= sLink.GetLength() && bOK)
   {
      if (i == sLink.GetLength() || sLink[i] == ',')
      {
         if (sscanf(s, "%d", &l) != 1) bOK = FALSE;
         else 
         {
            int index = Find(l);          
            if (index >= 0)
            {
               m_aFeatures[index].SetSelected(TRUE);
            }
         }

         s = "";
      } else
      {
         s = s + sLink[i];
      }
      i++;
   }

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////

void CQueryLink::Sort(int left, int right)
{
   int pivot,l_hold, r_hold;

   if (left == -1 && right == -1)
   {
      left = 0;
      right = m_aFeatures.GetSize()-1;
      if (right < 0) return;
   }

  
  CQueryFeature queryfeature;

  l_hold = left;
  r_hold = right;
  queryfeature = m_aFeatures.GetAt(left);
  while (left < right)
  {
    while (m_aFeatures.GetAt(right) >= queryfeature && left < right)
      right--;
    if (left != right)
    {
	  m_aFeatures.SetAt(left,m_aFeatures.GetAt(right));
      left++;
    }
    while (m_aFeatures.GetAt(left) <= queryfeature && left < right)
      left++;
    if (left != right)
    {
	  m_aFeatures.SetAt(right,m_aFeatures.GetAt(left));
      right--;
    }
  }
  m_aFeatures.SetAt(left, queryfeature);
  pivot = left;
  left = l_hold;
  right = r_hold;
  if (left < pivot)
    Sort(left, pivot-1);
  if (right > pivot)
    Sort(pivot+1, right);
}

///////////////////////////////////////////////////////////////////////////////

int CQueryLink::Find(long lId)
{
   int nHigh = m_aFeatures.GetSize()-1;
	int nLow = 0;
	int nMiddle;

	while (nLow <= nHigh)
	{
	   nMiddle = (nLow+nHigh)/2;

	   if (lId > m_aFeatures.GetAt(nMiddle).m_lId) nLow = nMiddle+1;
	   else if (lId < m_aFeatures.GetAt(nMiddle).m_lId) nHigh = nMiddle-1;
	   else return nMiddle;
   
	}
	return -1;	
}

///////////////////////////////////////////////////////////////////////////////

/*void CQueryCalc::Write(FILE* pFile)
{
   fprintf(pFile,"Statistic\n");
   fprintf(pFile,"AttrId2=%i\n",GetAttrId2());
   fprintf(pFile,"FTypeId2=%i\n",GetFTypeId2());
   fprintf(pFile,"Qualifier=%i\n",GetQualifier());
   fprintf(pFile,"Qualifier2=%i\n",GetQualifier2());
   fprintf(pFile,"Statistic=%i\n",GetStatistic());
   fprintf(pFile,"DecPlaces=%i\n",GetDecPlaces());
   fprintf(pFile,"Value1=%lf\n",GetValue1());
   fprintf(pFile,"Value2=%lf\n",GetValue2());

   CQueryElement::Write(pFile);
}*/

///////////////////////////////////////////////////////////////////////////////

BOOL CQueryCalc::Read(FILE* pFile)
{
   SetAttrId2(BDNextInt(pFile));
   SetFTypeId2(BDNextInt(pFile));
   SetQualifier(BDNextInt(pFile, "Qualifier",0));
   SetQualifier2(BDNextInt(pFile, "Qualifier2",0));
   SetStatistic(BDNextInt(pFile));   
   SetDecPlaces(BDNextInt(pFile, "DecPlaces", 0));
   SetValue1(BDNextDouble(pFile, "Value1", 0));
   SetValue2(BDNextDouble(pFile, "Value2", 0));
   

   return CQueryElement::Read(pFile);
}

///////////////////////////////////////////////////////////////////////////////

void CQueryCalc::AsXML(CXMLObj* pXMLObj)
{
   CXMLObj *pXMLChild = pXMLObj->AddChild("querycalc");
   pXMLChild->SetValue("", "attrid2", GetAttrId());
   pXMLChild->SetValue("", "ftypeid2", GetFTypeId2());
   pXMLChild->SetValue("", "qualifier", GetQualifier());
   pXMLChild->SetValue("", "qualifier2", GetQualifier2());
   pXMLChild->SetValue("", "statistic", GetStatistic());
   pXMLChild->SetValue("", "decplaces", GetDecPlaces());
   if (GetAttrId() == 0) pXMLChild->SetValue("", "value1", GetValue1());
   if (GetAttrId2() == 0) pXMLChild->SetValue("", "value2", GetValue2());

   // Convert parent object

   CQueryElement::AsXML(pXMLChild);
}

///////////////////////////////////////////////////////////////////////////////

BOOL CQueryCalc::XMLAs(CXMLObj* pXMLObj)
{
   BOOL bOK = TRUE;

   CXMLObj *pXMLChild = pXMLObj->GetXMLObj("", "querycalc");
   SetAttrId(pXMLChild->GetInteger("", "attrid2"));
   SetFTypeId2(pXMLChild->GetInteger("", "ftypeid2"));
   SetQualifier(pXMLChild->GetInteger("", "qualifier"));
   SetQualifier2(pXMLChild->GetInteger("", "qualifier2"));
   SetStatistic(pXMLChild->GetInteger("", "statistic"));
   SetDecPlaces(pXMLChild->GetInteger("", "decplaces"));
   if (GetAttrId() == 0) SetValue1(pXMLChild->GetDouble("", "value1"));
   if (GetAttrId2() == 0) SetValue2(pXMLChild->GetDouble("", "value2"));

   // Convert parent object

   CQueryElement::XMLAs(pXMLChild);

   return bOK;
}
///////////////////////////////////////////////////////////////////////////////

CString CArrayAttrSel::GetAttrDesc(CAttrArray& aAttr)
{
      CString sAttrName;

   int i = 0; for (i = 0; i < GetSize(); i++)
   {   
      // Attributes
      
      int j = 0; for (j = 0; j < aAttr.GetSize(); j++)
      {
         if (GetAt(i).m_lAttr == aAttr.GetAt(j)->GetAttrId() &&
             GetAt(i).m_lFType == aAttr.GetAt(j)->GetFTypeId())
         {
            CAttribute* pAttr = aAttr.GetAt(j);
            if (pAttr->GetDataType() != BDCOORD && 
                pAttr->GetDataType() != BDMAPLINES)
            {
               if (sAttrName.GetLength() > 0) sAttrName+= " ";

               CBDAttribute* pAttr = (CBDAttribute*)aAttr[j];
               sAttrName += pAttr->AsString();      
               sAttrName.TrimRight();
            };
         }
      }            
   };

     // Filter out anything in square brackets

   return CQuery::StripBrackets(sAttrName);   
}


///////////////////////////////////////////////////////////////////////////////

CQueryElement::CQueryElement()
{
   m_nDataType = 0;
   m_nAttrId = 0;
   m_lFType = 0;   
   m_nStatGroup= none; 
   m_nSortBy = none;  
   m_nGroupBy = none;   
   m_bSelected = FALSE;

   m_pNext = NULL;
   m_pQueryJoin = NULL;
   

}

///////////////////////////////////////////////////////////////////////////////

CQueryElement::CQueryElement(CQueryElement& rSrc)
{
   *this = rSrc;
}

CQueryElement& CQueryElement::operator=(CQueryElement& rSrc)
{
   m_nDataType = rSrc.m_nDataType;
   m_nAttrId = rSrc.m_nAttrId;
   m_lFType = rSrc.m_lFType;
   m_nStatGroup = rSrc.m_nStatGroup;
   m_nSortBy = rSrc.m_nSortBy;
   m_nGroupBy = rSrc.m_nGroupBy;
   m_bSelected = rSrc.m_bSelected;
   m_sDesc = rSrc.m_sDesc;
   m_sColName = rSrc.m_sColName;

   m_aCondition.Copy(rSrc.m_aCondition);

   m_pNext = NULL;   
   m_pQueryJoin = NULL;
   if (rSrc.m_pNext != NULL)
   {   
      CQueryElement* pNext = rSrc.m_pNext;
      switch (pNext->GetDataType())
      {
         case BDFTYPE : 
            m_pNext = new CQuery((CQuery&)*pNext); break;                       
         case BDLINK : case BDFEATURE : 
            m_pNext = new CQueryLink((CQueryLink&)*pNext); break;               
         case BDNUMBER : 
            m_pNext = new CQueryNumber((CQueryNumber&)*pNext); break;               
         case BDBOOLEAN : 
            m_pNext = new CQueryBoolean((CQueryBoolean&)*pNext); break;               
         case BDQUERYSTATS : 
            m_pNext = new CQueryCalc((CQueryCalc&)*pNext); break;
         default : 
            m_pNext = new CQueryElement(*pNext);                     
      }
      ASSERT(m_pNext != NULL);
   }   
   if (rSrc.m_pQueryJoin != NULL) 
   {
      m_pQueryJoin = new CQueryJoin(*rSrc.m_pQueryJoin);
   }
   return *this;
}

///////////////////////////////////////////////////////////////////////////////

CQueryElement::~CQueryElement()
{
   Delete(m_pNext);
   CQueryJoin::Delete(m_pQueryJoin);
}

///////////////////////////////////////////////////////////////////////////////

void CQueryElement::Delete(CQueryElement*& pElement)
{   
   if (pElement != NULL)
   {
      if (pElement->m_pNext != NULL)
      {
         Delete(pElement->m_pNext);
      }

      switch (pElement->GetDataType())
      {
         case BDFTYPE : 
            delete ((CQuery*)pElement);
            break;       
         case BDLINK : case BDFEATURE : 
            delete ((CQueryLink*)pElement);
            break;         
         case BDNUMBER : delete ((CQueryNumber*)pElement);
            break;
         case BDBOOLEAN : delete ((CQueryBoolean*)pElement);
            break;
         case BDDATE : case BDTEXT : case BDCOORD : case BDHOTLINK : case BDLONGTEXT :
         case BDMAPLINES : case BDIMAGE : case BDFILE :
            delete pElement;
            break;
         case BDQUERYSTATS : delete ((CQueryCalc*)pElement);
            break;
         case BDJOIN: 
            delete ((CQueryJoin*)pElement);
            break;
         default: 
            ASSERT(FALSE);
      }      
   }
   pElement = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void CQueryJoin::Delete(CQueryJoin*& pElement)
{
   // CQuery Join doesn't have any attributes so only need to call for parent

   CQueryElement::Delete((CQueryElement*&)pElement);   
}

////////////////////////////////////////////////////////////////////////////////
//
// Save query joins as
//
// <queryjoin>
//    <ftype/>...
//    <condition/>
//       <query/>
//

void CQueryJoin::AsXML(CXMLObj* pXMLObj)
{
   CQueryElement::AsXML(pXMLObj);

   CXMLObj *pXMLChild = pXMLObj->AddChild("query");
   ((CQuery*)m_pNext)->AsXML(pXMLChild);
}

///////////////////////////////////////////////////////////////////////////////

BOOL CQueryJoin::XMLAs(CXMLObj* pXMLObj)
{
   BOOL bOK = TRUE;

   // Initialise join attributes 

   CQueryElement::XMLAs(pXMLObj);

   // Now initialise a query object

   ASSERT(m_lFType);
   if (m_lFType != 0)
   {
      CQuery* pQuery = new CQuery;
      m_pNext = pQuery;

      CXMLObj *pXMLChild = pXMLObj->GetXMLObj("", "query", TRUE);
      if (pXMLChild != NULL)
      {
         if (!pQuery->XMLAs(pXMLChild))
         {
            bOK = FALSE;
         }
      }

   }

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////

CQueryFeature::CQueryFeature()
{
   m_bSelected = FALSE;
}

///////////////////////////////////////////////////////////////////////////////

CQueryLink::CQueryLink()
{

}

///////////////////////////////////////////////////////////////////////////////

CQueryLink::CQueryLink(CQueryLink& rSrc) : CQueryElement(rSrc)
{
   m_aFeatures.Copy(rSrc.m_aFeatures);
}

///////////////////////////////////////////////////////////////////////////////

CQueryLink::CQueryLink(long lFType)
{
   CFeature feature;
   feature.m_lFeatureTypeId = lFType;

   BOOL bFound = BDFeature(BDHandle(), &feature, BDSELECT2);
   while (bFound)
   {
      CQueryFeature qf = feature;
      qf.m_sName.TrimRight();

      m_aFeatures.Add(qf);      

      bFound = BDGetNext(BDHandle());
   }
   BDEnd(BDHandle());
}

///////////////////////////////////////////////////////////////////////////////

CQueryBoolean::CQueryBoolean()
{
   CQueryFeature feature;
   feature.m_lId = no;
   feature.m_sName = BDString(IDS_NO);
   feature.SetSelected();
   m_aFeatures.Add(feature);
   feature.m_lId = yes;
   feature.m_sName = BDString(IDS_YES);
   m_aFeatures.Add(feature);   
}

///////////////////////////////////////////////////////////////////////////////

CQueryCalc::CQueryCalc()
{
   SetAttrId2(0);
   SetFTypeId2(0);
   SetStatistic(0);
   SetDecPlaces(0);
   SetValue2(0);
   SetQualifier(0);
   SetQualifier2(0);

}

///////////////////////////////////////////////////////////////////////////////

CQueryCalc::CQueryCalc(CQueryCalc& rSrc) : CQueryElement(rSrc)
{
   m_nAttrId2 = rSrc.m_nAttrId2;
   m_lFType2 = rSrc.m_lFType2;
   m_lQualifier = rSrc.m_lQualifier;
   m_lQualifier2 = rSrc.m_lQualifier2;
   m_nStatistic = rSrc.m_nStatistic;
   m_nDecPlaces = rSrc.m_nDecPlaces;
   m_dValue1 = rSrc.m_dValue1;
   m_dValue2 = rSrc.m_dValue2;
}

///////////////////////////////////////////////////////////////////////////////

CQueryAttrArray::CQueryAttrArray(LPCSTR sFeature)
{      
   m_sFeature = sFeature;
   m_bCondMet = FALSE;
   m_lFeatureActive = 0;
   m_lParentFeatureActive = 0;
}

///////////////////////////////////////////////////////////////////////////////

CQueryAttrArray::CQueryAttrArray(CQueryAttrArray& rSrc) : 
    CAttrArray(rSrc)
{
   m_sFeature = rSrc.m_sFeature;
   m_bCondMet = rSrc.m_bCondMet;
   m_lFeatureActive = rSrc.m_lFeatureActive;
   m_sFeatureActive = rSrc.m_sFeatureActive;
   m_lParentFeatureActive = rSrc.m_lParentFeatureActive;
   m_dateActive = rSrc.m_dateActive;
}

///////////////////////////////////////////////////////////////////////////////

CQueryAttrArray::~CQueryAttrArray()
{   
}

///////////////////////////////////////////////////////////////////////////////

CQueryAttrSel::CQueryAttrSel()
{
   m_lAttr = 0;
   m_lFType = 0;
   m_lDataType = 0;
   m_nStatGroup = 0;
   m_nGroupBy = 0;
   m_nSortBy = 0;
}



///////////////////////////////////////////////////////////////////////////////

CQueryResult::CQueryResult()
{
   m_pQuery = NULL;
   m_bBaseFType = FALSE;
   m_pQueryResult = NULL;
}

CQueryResult::~CQueryResult()
{
   if (m_pQueryResult  != NULL) delete m_pQueryResult;
   RemoveAllX();
}

///////////////////////////////////////////////////////////////////////////////

void CQueryResult::RemoveAllX()
{
   int i = 0; for (i = 0; i < GetSize(); i++)
   {
      delete GetAt(i);
   }
   RemoveAll();
}

///////////////////////////////////////////////////////////////////////////////

BOOL CQueryResult::Initialise(CQuery* pQuery)
{
   BOOL bOK = TRUE;

   // Pointer to the header of the list

   m_pQuery = pQuery;

   // Initialise list of features

   AfxGetApp()->BeginWaitCursor();
   if (InitFeatures(pQuery))
   {
      // For each feature type retrieve data, ensure it has attributes selected

      bOK = RetrieveData(pQuery);

      // Retrieve data for each feature that is joined

      if (bOK) 
      {
         CQueryResultArray aQueryResult;
         bOK = RetrieveJoinData(pQuery, aQueryResult);

         // Perform (spatial) joins
         if (bOK)
         {
            bOK = JoinQueryResult(pQuery, aQueryResult);
         };

      };      

      // Perform calculations

      if (bOK)
      {
         CalcRowBasedStats();

         // Sort the results of the query

         SortQueryResults();

         // Calculate statistics
            
         CalculateStatistics();
      };      
   } else
   {
      bOK = FALSE;
      RemoveAllX();
   }
   AfxGetApp()->EndWaitCursor();
      
   return bOK;
}

///////////////////////////////////////////////////////////////////////////////
// 
// Determines the description of the query from the selected feature type and
// attributes
//

CString CQuery::GetQueryName()
{
	CString sFeature, sAttr, sName;
	int iFeature = 0;
	int iAttr = 0;
   long lParentFeature = 0;
   CFeature feature;
   CFeatureType ftype;   
    
	// Feature type name

	CQueryElement* pElement = this;
	while (pElement != NULL)
	{
	   // Determine how many features are selected, if only one then include
	   // in name

       if (pElement->GetDataType() == BDFEATURE)
	   {
		  CQueryLink* pLink = (CQueryLink*)pElement;
		  int i = 0; for (i = 0; i < pLink->m_aFeatures.GetSize(); i++)
		  {
			  if (pLink->m_aFeatures.GetAt(i).GetSelected())
			  {
				  iFeature++;
				  sFeature = pLink->m_aFeatures.GetAt(i).m_sName;

              if (lParentFeature == 0)
                 lParentFeature = pLink->m_aFeatures.GetAt(i).m_lParentFeature;
              else if (lParentFeature != pLink->m_aFeatures.GetAt(i).m_lParentFeature)
                 lParentFeature = -1;
			  }           
		  }		  
	   }

	   // Determine how many attributes are selected, if only one then include in name

	   if (pElement->GetSelected() && (pElement->GetDataType() == BDNUMBER ||
          pElement->GetDataType() == BDTEXT || 
		  pElement->GetDataType() == BDLONGTEXT ||
          pElement->GetDataType() == BDLINK ||
          pElement->GetDataType() == BDBOOLEAN))
	   {
		   iAttr++;
		   sAttr = pElement->GetDesc();
	   }

      if (pElement->GetDataType() == BDQUERYSTATS)
      {
         iAttr++;
         sAttr = pElement->GetDesc();
      }
	          
	   pElement = pElement->GetNextQuery();
	}

	// Create name
   
	if (iAttr == 1)
	{
	   sName = sAttr;
	}

   // Add feature name

   if (iFeature == 1)
   {
      sFeature.TrimRight();
      sName = sFeature;
   }

   if (sName == "") sName = GetDesc();

   // If only one parent feature then include in the name

   if (lParentFeature > 0)
   {  
      BDFTypeParentI(BDHandle(), GetFType(), &ftype);
      feature.m_lId = lParentFeature;
      feature.m_lFeatureTypeId = ftype.m_lId;
      BDFeature(BDHandle(), &feature, BDSELECT);
      BDEnd(BDHandle());
      feature.m_sName.TrimRight();
      sName += ", " + feature.m_sName;
   }	

   // Strip square brackets
   
   CString str = CQuery::StripBrackets(sName);   

   return str;
}

///////////////////////////////////////////////////////////////////////////////
//
// For each feature in the query add an attribute array to the list
//

BOOL CQueryResult::InitFeatures(CQuery* pQuery)
{
   CQueryElement* pElement = pQuery;
   while (pElement != NULL)
   {
      if (pElement->GetDataType() == BDFEATURE)
      {
         CQueryLink* pLink = (CQueryLink*)pElement;
         int i = 0; for (i = 0; i < pLink->m_aFeatures.GetSize(); i++)
         {
            CQueryFeature *pFeature = &pLink->m_aFeatures[i];
            if (pFeature->GetSelected())
            {
               CQueryAttrArray* pQAA = new CQueryAttrArray(pFeature->m_sName);
               pQAA->m_lFeature = pFeature->m_lId;
               pQAA->m_lFeatureActive = pFeature->m_lId;
               pQAA->m_sFeature = pFeature->m_sName;
               pQAA->m_sFeatureActive = pFeature->m_sName;                
               pQAA->m_lParentFeatureActive = pFeature->m_lParentFeature;

               Add(pQAA);
            }
         }
         break;
      };
      pElement = pElement->GetNextQuery();
   };

   return pElement != NULL;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CQueryResult::RetrieveData(CQuery* pQuery)
{
   BOOL bOK = TRUE;   
   CFeature feature;
   CFeatureType ftype;

   m_bBaseFType = TRUE;
 
   // Firstly determine which attributes are selected for all feature types
   
   CQueryElement* pElement = pQuery;
   int iAttr = -1;
   while (pElement != NULL)
   {      
      CQueryAttrSel attrsel;

      if (pElement->GetSelected() == TRUE) // -1 indicates join
      {                         
         attrsel.m_lAttr = pElement->GetAttrId();
         attrsel.m_lFType = pElement->GetFTypeId();
         attrsel.m_lDataType = pElement->GetDataType();                        
      };      
      
      if (pElement->GetDataType() == BDQUERYSTATS)
      {
         attrsel.m_lAttr = iAttr--;
         attrsel.m_lFType = m_pQuery->GetFTypeId();
         attrsel.m_lDataType = BDNUMBER;
      }

      if (pElement->GetSelected() || pElement->GetDataType() == BDQUERYSTATS)
      {
         attrsel.m_nGroupBy = pElement->GetGroupBy();
         attrsel.m_nSortBy = pElement->GetSortBy();
         attrsel.m_nStatGroup = pElement->GetGroupStat();      

         m_aAttrSel.Add(attrsel);               
      };      

      pElement = pElement->GetNextQuery();      
   };

   // Retrieve the data for these attributes, even if no attributes are
   // selected as the feature may be removed if conditions are not met

      bOK = LoadDataAll(pQuery, pQuery->GetFType());

    // Set the date

      int i = 0; for (i = 0; i < GetSize(); i++)
      {         
         if (GetAt(i)->m_dateActive.IsValid())
         {
            GetAt(i)->m_lDate = GetAt(i)->m_dateActive.GetDateLong();         
         };
      };

   // Retrieve attributes for remaining feature types   

   m_bBaseFType = FALSE;

   while (bOK && pQuery != NULL)
   {     
     // Determine if the feature is one to one or one to many

      ftype.m_lId = pQuery->GetFType();
      BDFeatureType(BDHandle(), &ftype, BDSELECT);
      BDEnd(BDHandle());

     // Determine next element

      pQuery = (CQuery*)pQuery->GetNextQuery();
      while (pQuery != NULL && pQuery->GetDataType() != BDFTYPE)
      {
         pQuery = (CQuery*)pQuery->GetNextQuery();
      }

      if (pQuery != NULL)
      {
         // Reset dates

         for (i = 0; i < GetSize(); i++)
         {
            GetAt(i)->m_dateActive = CDateTime(0,0);
         };       

        // Change the active feature to the parent feature for the type     

         if (ftype.m_bManyToOne)
         {
           // Update active features

            int i = 0; for (i = 0; i < GetSize(); i++)
            {
               GetAt(i)->m_lFeatureActive = GetAt(i)->m_lParentFeatureActive;               
            }

			// Reset the parent names incase parent feature cannot be found!

		   for (i = 0; i < GetSize(); i++)
		   {
			   GetAt(i)->m_sFeatureActive = "";
		   }

            // Update name and parent of features

            feature.m_lFeatureTypeId = pQuery->GetFType();
            BOOL bFound = BDFeature(BDHandle(), &feature, BDSELECT2);
            while (bFound)
            {
               int i = FindSorted(feature.m_lId);
               while (i != -1)
               {
                     GetAt(i)->m_lParentFeatureActive = feature.m_lParentFeature;
                     GetAt(i)->m_sFeatureActive = feature.m_sName;

                     // Check next
                    
                     if (i+1 < GetSize() && GetAt(i+1)->m_lFeatureActive == feature.m_lId) i++;
                     else i = -1;
               }
               bFound = BDGetNext(BDHandle());
            }
            BDEnd(BDHandle());
         };
         
         // Retrieve the data for the parent feature type
         
         ASSERT(pQuery->GetDataType() == BDFTYPE);
         bOK = LoadDataAll(pQuery, pQuery->GetFType());      
      };
   };

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////
//
// This is a faster way of retrieving large numbers of values.  
// Retrieves the attributes for all features rather than querying for every one
//

BOOL CQueryResult::LoadDataAll(CQuery* pQuery, long lFType)
{
   CAttrArray aAttr;   
   CFeature feature;   
   BOOL bOK = TRUE;
   long lFeatureMin, lFeatureMax;
   int iCount = 0;

   // Display name of ftype

   CString s = pQuery->GetDesc();
   BDProgressText("Loading " + s.Left(s.Find('[')) + "...");

   // No need to load if no features

   if (GetSize() == 0) return FALSE;

    // Retrieve the selected feature names for the query

   int i = 0; for (i = 0; i < GetSize(); i++)
   {
      RetrieveFeatures(pQuery, GetAt(i));      
   }

   // If no attributes or conditions selected then no need to load data
   // This also handles where parent features have no attributes set

   int nRet = IsFTypeSelected(pQuery, lFType);
   if (!m_bBaseFType && nRet != yes)
   {
      return TRUE;
   }

   // Re-initialise the 'conditions met' flags

   for (i = 0; i < GetSize(); i++)
   {
      CQueryAttrArray* pAttrArray = GetAt(i);
      pAttrArray->SetCondMet(FALSE);
   }

   // Determine the maximum and minimum feature ids

   ASSERT(GetSize() > 0);
   lFeatureMax = lFeatureMin = GetAt(0)->GetActiveFeature();
   for (i = 0; i < GetSize(); i++)
   {           
      lFeatureMin = min(lFeatureMin, GetAt(i)->GetActiveFeature());
      lFeatureMax = max(lFeatureMax, GetAt(i)->GetActiveFeature());    
   }

   // Sort the query results for fast searching

   SortFeature(0, GetSize()-1);

   // Initialise attribute array 

   BDFTypeAttrInit(BDHandle(), lFType, &aAttr);

   // Retrieve all data and then extract what is needed   
   
   BDProgressRange(0, GetSize());
   
    CString sFilter;
    long lDateStart = 0, lDateEnd = 0;
    sFilter = GetQueryFilter(pQuery, aAttr);
    int iFlag = BDGETINIT;
    if (m_pQuery->GetDateCond() == CQuery::FirstDate || m_pQuery->GetDateCond() == CQuery::FirstDateRange) iFlag = BDSELECTEARLIEST;
    if (m_pQuery->GetDateCond() == CQuery::LastDate || m_pQuery->GetDateCond() == CQuery::LastDateRange) iFlag = BDSELECTLATEST;
    if (m_pQuery->GetDateCond() == CQuery::FirstDateRange || m_pQuery->GetDateCond() == CQuery::LastDateRange || m_pQuery->GetDateCond() == CQuery::AllDatesRange)
    {
       lDateStart = m_pQuery->GetStartDate().GetDateLong();
       lDateEnd = m_pQuery->GetEndDate().GetDateLong();

    }


    BOOL bFound = BDAttributeCond(BDHandle(), &aAttr, lFeatureMin, lFeatureMax, lDateStart, lDateEnd, sFilter, iFlag);
   
   while (bFound && bOK)
   {
     // For each record, determine whether the feature is selected

	  int i = FindSorted(aAttr.m_lFeature);

     // Make sure this is the first one

     while (i > 0 && aAttr.m_lFeature == GetAt(i-1)->GetActiveFeature()) i--;

     // Loop through as additional features may be added where there is more than one results (e.g. multiple dates)

     while (i != -1 && i < GetSize() && aAttr.m_lFeature == GetAt(i)->GetActiveFeature())
     {                 
		 CQueryAttrArray* pAttrArray = GetAt(i);

        // Check the conditions imposed are valid
        
        if (CheckConditions(pQuery, aAttr)
            && CheckValueCond(pQuery, aAttr)
           )
        {             
           // Check the conditions are met

           pAttrArray->SetCondMet();

           // If so determine whether the data is to replace the existing
           // data, or to be created as a new feature

           int nStatus = UpdateDates(pQuery, aAttr, pAttrArray, i);
           if (nStatus == Replace)
           {                              
              RetrieveAttr(pQuery, aAttr, pAttrArray);                            
           }

           // If a new reading then retrieve the attributes for this and then 
           // skip remaining feature types

           else if (nStatus == New)
           {
              //i = GetSize()-2;
              RetrieveAttr(pQuery, aAttr, GetAt(i+1));
              break;
           }

           // Where the required data already exists, fill in missing attributes

           else if (nStatus == Keep)
           {
              RetrieveMissing(pQuery, aAttr, pAttrArray);
           }

           if (!BDProgressPos(iCount++)) bOK = FALSE;
           if (iCount >= GetSize()) 
           {                  
              iCount = 0;
              BDProgressRange(0,GetSize());
           };
        }
        i++;
      }

      bFound = BDGetNext(BDHandle());      
   }
   BDEnd(BDHandle());   
   BDProgressRange(0,0);
   
   // Remove all features with no data
   
   for (i = 0; i < GetSize(); i++)
   {       
      CQueryAttrArray* pAttrArray = GetAt(i);
      if (!pAttrArray->GetCondMet())
      {           
         if (!m_pQuery->IsShowAllFeatures())
         {
            delete pAttrArray; 
            RemoveAt(i);
            i--;         
         } else
         {
            InitNullAttr(pQuery, aAttr, pAttrArray);
         }
      };
   };
   
   return bOK;
}

///////////////////////////////////////////////////////////////////////////////
//
// Retrieves a queryresul object for each query joined to the main query. 
// This is appended to the array
//

BOOL CQueryResult::RetrieveJoinData(CQuery* pQuery, CQueryResultArray& aQueryResult)
{   
   BOOL bOK = TRUE;   

   // Find each query join

   CQueryElement* pElement = pQuery;   
   while (pElement != NULL && bOK)
   {
      if (pElement->GetQueryJoin() != NULL)
      {
         CQueryJoin *pQueryJoin = pElement->GetQueryJoin();

         CQuery *pQuery1 = (CQuery*)pQueryJoin->GetNextQuery();
         
         // Flag the attribute that is included in the join so that it will be selected

         CQueryElement* pElement1 = pQuery1;   
         while (pElement1 != NULL)
         {
            if (pElement1->GetAttrId() == pQueryJoin->GetAttrId())
            {
               if (!pElement1->GetSelected())
               {
                  pElement1->SetSelected(-1);
               }
               break;
            };
            pElement1 = pElement1->GetNextQuery();
         }

         // TODO need to ensure that joined from attributes are loaded (before call to this
         // function)

         // TODO if conditions, stats etc. in main query can include attributes from 
         // the joined query then these features need to be included

         CQueryResult *pQueryResult = new CQueryResult;

         // Date conditions from parent query apply to joined queries

         pQueryResult->m_pQuery = pQuery;

         if (pQueryResult->InitFeatures(pQuery1) &&          
            pQueryResult->RetrieveData(pQuery1))
         {
            aQueryResult.Add(pQueryResult);            
         } else
         {
            bOK = FALSE;
            delete pQueryResult;
         }
      }
      pElement = pElement->GetNextQuery();
   }
   
   return bOK;
}

///////////////////////////////////////////////////////////////////////////////
//
// Performs a join on the query results and those joined to it.  Identifies 
// individual joins and calls subfunction
//
//

BOOL CQueryResult::JoinQueryResult(CQuery* pQuery, CQueryResultArray& aQueryResult)
{
   int iJoin = 0;
   BOOL bOK = TRUE;

   // Find the query joins

   CQueryElement *pElement = pQuery;
   while (pElement != NULL)
   {
      if (pElement->GetQueryJoin() != NULL)
      {
         CQueryJoin *pQueryJoin = pElement->GetQueryJoin();
         bOK = JoinQueryResult(pElement, pQueryJoin, *(aQueryResult[iJoin]));
         iJoin++;
      }

      pElement = pElement->GetNextQuery();
   }
   return bOK;
}

///////////////////////////////////////////////////////////////////////////////
//
// Performs a join between a queryresult and a joined query result
//

BOOL CQueryResult::JoinQueryResult(CQueryElement* pJoinAttr, CQueryJoin* pQueryJoin, CQueryResult& queryresult)
{   
   CQueryResult queryresultoutput;

   // For each object in the query result, check if it joins with each object in the 
   // joined query.  If so combine their attributes into a new result!

   BDProgressRange(0, GetSize());
   BDProgressText("Applying spatial joins...");

  // Determine the join condition

   BOOL bJoin = FALSE;
   int nCond = 0;
   double dCondValue = 0;

   if (pQueryJoin->GetConditions().GetSize() > 0) 
   {
      nCond = pQueryJoin->GetConditions().GetAt(0).GetCond();
      dCondValue = pQueryJoin->GetConditions().GetAt(0).GetCondValue();
   }

 // Re-initialise the 'conditions met' flags for exlusive query

   int i = 0; for (i = 0; i < GetSize(); i++)
   {
      CQueryAttrArray* pAttrArray = GetAt(i);
      pAttrArray->SetCondMet(TRUE);
   }

  // Compare all attributes

   for (i = 0; i < GetSize(); i++)
   {
      int j = 0; for (j = 0; j < queryresult.GetSize(); j++)
      {
         // Find the attribute joined from

         CQueryAttrArray *pQueryAttrFrom = GetAt(i);
         CAttribute *pAttrFrom = NULL;
         int k = 0; for (k = 0; k < pQueryAttrFrom->GetSize(); k++)
         {
            if (pQueryAttrFrom->GetAt(k)->GetFTypeId() == pJoinAttr->GetFTypeId() && 
                pQueryAttrFrom->GetAt(k)->GetAttrId() == pJoinAttr->GetAttrId())
            {
               pAttrFrom = pQueryAttrFrom->GetAt(k);
               break;
            }
         }

         // Find the attribute joined to 

         CQueryAttrArray *pQueryAttrTo = queryresult.GetAt(j);
         CAttribute *pAttrTo = NULL;
         for (k = 0; k < pQueryAttrTo->GetSize(); k++)
         {
            if (pQueryAttrTo->GetAt(k)->GetFTypeId() == pQueryJoin->GetFTypeId() && 
                pQueryAttrTo->GetAt(k)->GetAttrId() == pQueryJoin->GetAttrId())
            {
               pAttrTo = pQueryAttrTo->GetAt(k);
               break;
            }
         }

         ASSERT(pAttrFrom != NULL && pAttrTo != NULL);
         if (pAttrFrom == NULL || pAttrTo == NULL)
         {
            return FALSE;
         }

         // Inside

         if (nCond & CQueryJoin::inside)
         {
            if (pAttrFrom->GetDataType() == BDCOORD && pAttrTo->GetDataType() == BDMAPLINES)
            {
               CCoord *pCoord = pAttrFrom->GetCoord();
               CLongLines longlines(*pAttrTo->GetLongBinary());                  

               bJoin = CSpatial::Inside(CLongCoord((long)pCoord->x,(long)pCoord->y), longlines);
            }
            if (pAttrFrom->GetDataType() == BDMAPLINES && pAttrTo->GetDataType() == BDMAPLINES)
            {
               CLongLines longlines1(*pAttrFrom->GetLongBinary());                  
               CLongLines longlines2(*pAttrTo->GetLongBinary());                  

               bJoin = CSpatial::Inside(longlines1, longlines2);
            }          
         }

         // Centroid inside

         if (nCond & CQueryJoin::centroidinside)
         {           
            if (pAttrFrom->GetDataType() == BDMAPLINES && pAttrTo->GetDataType() == BDMAPLINES)
            {  
               CSpatialPoly polygon1(*pAttrFrom->GetLongBinary());                  
               CSpatialPoly polygon2(*pAttrTo->GetLongBinary());                  

               CLongCoord coord1;
               polygon1.GetCentroid(coord1);

               bJoin = CSpatial::Inside(coord1, polygon2);
            }          
         }

         // Intersects

         if (nCond & CQueryJoin::intersects)
         {
             if (pAttrFrom->GetDataType() == BDMAPLINES && pAttrTo->GetDataType() == BDMAPLINES)
             {
                CLongLines longlines1(*pAttrFrom->GetLongBinary());                  
                CLongLines longlines2(*pAttrTo->GetLongBinary());                  

                bJoin = CSpatial::Intersects(longlines1, longlines2);
             }
         };
         
         // Within

         if (nCond & CQueryJoin::within)
         {
            if (pAttrFrom->GetDataType() == BDCOORD && pAttrTo->GetDataType() == BDCOORD)
            {
               CCoord *pCoord1 = pAttrFrom->GetCoord();
               CCoord *pCoord2 = pAttrTo->GetCoord();

               bJoin = CSpatial::Within(CLongCoord((long)pCoord1->x,(long)pCoord1->y), CLongCoord((long)pCoord2->x,(long)pCoord2->y), (int)dCondValue);
            }
            if (pAttrFrom->GetDataType() == BDCOORD && pAttrTo->GetDataType() == BDMAPLINES)
            {
               CCoord *pCoord = pAttrFrom->GetCoord();
               CLongLines longlines(*pAttrTo->GetLongBinary());                  

               bJoin = CSpatial::Within(CLongCoord((long)pCoord->x,(long)pCoord->y), longlines, (int)dCondValue);
            }
            if (pAttrFrom->GetDataType() == BDMAPLINES && pAttrTo->GetDataType() == BDMAPLINES)
            {
               CLongLines longlines1(*pAttrFrom->GetLongBinary());                  
               CLongLines longlines2(*pAttrTo->GetLongBinary());                  

               bJoin = CSpatial::Within(longlines1, longlines2, (int)dCondValue);
            }          
         }

         // TODO other comparisons

         // Now merge results

         if (!(nCond & CQueryJoin::exclusive))
         {
            if (bJoin)
            {
               CQueryAttrArray *pQueryAttrArray = new CQueryAttrArray(pQueryAttrFrom->GetFName());

               // Create new objects not pointers (i.e. don't use copy and append)

               for (k = 0; k < pQueryAttrFrom->GetSize(); k++)
               {
                  // Check if attribute appears in attribute list. 

                  for (int l = 0; l < m_aAttrSel.GetSize(); l++)
                  {
                      if (m_aAttrSel[l].m_lAttr == pQueryAttrFrom->GetAt(k)->m_lAttrId && 
                          m_aAttrSel[l].m_lFType == pQueryAttrFrom->GetAt(k)->m_lFType)
                      {
                         pQueryAttrArray->Add(new CAttribute(*pQueryAttrFrom->GetAt(k)));
                      };
                  };
               }
               for (k = 0; k < pQueryAttrTo->GetSize(); k++)
               {
                  // Check if attribute appears in attribute list. 

                  for (int l = 0; l < queryresult.m_aAttrSel.GetSize(); l++)
                  {
                     if (queryresult.m_aAttrSel[l].m_lAttr == pQueryAttrTo->GetAt(k)->m_lAttrId && 
                         queryresult.m_aAttrSel[l].m_lFType == pQueryAttrTo->GetAt(k)->m_lFType)
                     {
                         pQueryAttrArray->Add(new CAttribute(*pQueryAttrTo->GetAt(k)));
                         break;
                     };
                  };
               }            

               queryresultoutput.Add(pQueryAttrArray);
            }
         }
         // Exclusive
         else
         {
            if (bJoin)
            {
               pQueryAttrFrom->SetCondMet(FALSE);
            }
         }
      }
      BDProgressPos(i);
   }
  
   // Inclusive

   if (!(nCond & CQueryJoin::exclusive))
   {
      // Append attribute selection
      m_aAttrSel.Append(queryresult.m_aAttrSel);
         
      // Replace results

      RemoveAllX();
      Copy(queryresultoutput);

      // Prevent objects being deleted
      queryresultoutput.RemoveAll();
   } 

   // Exclusive
   else
   {
      // Copy all features where conditions are met

      int i = 0; for (i = 0; i < GetSize(); i++)
      {
         if (!GetAt(i)->GetCondMet())
         {
            delete GetAt(i);
            RemoveAt(i);
            i--;
         }
      }
   
   }



   return TRUE;
};

///////////////////////////////////////////////////////////////////////////////
//
// Creates a filter for the SQL query to optimise the retrieval of data 
// accordinate to conditions
//

CString CQueryResult::GetQueryFilter(CQuery* pQuery, CAttrArray& aAttr)
{
   CString sFilter;   

   // Optimisation, specify feature ids in query
   
   if (GetSize() < MAXQUERYIDS)
   {
	   CString s;
	   sFilter = "`FEATURE_ID` in (";
	   int i = 0; for (i = 0; i < GetSize(); i++)
	   {           
		  if (i > 0) sFilter += ",";   
		  s.Format("%d",GetAt(i)->GetActiveFeature());      
		  sFilter += s;
	   }
	   sFilter += ")";


	   // Do not use this filter if SQL is going to be too long, will still
	   // retrieve between max and min ids

	   if (sFilter.GetLength() > MAXFILTERLEN) sFilter = "";
   }

   // Retrieve conditions from filter

   CQueryElement* pElement = pQuery->GetNextQuery();
   while (pElement != NULL)
   {
      int j = 0; for (j = 0; j < pElement->GetConditions().GetSize(); j++)
      {        
         int nCond = pElement->GetConditions().GetAt(j).GetCond();
         double dCondValue = pElement->GetConditions().GetAt(j).GetCondValue();
         CString sCondValue = pElement->GetConditions().GetAt(j).GetCondValueStr();

         // Find the corresponding attribute

         int i = 0; for (i = 0; i < aAttr.GetSize(); i++)
         {
            if (aAttr.GetAt(i)->GetAttrId() == pElement->GetAttrId() && 
                aAttr.GetAt(i)->GetFTypeId() == pElement->GetFTypeId())
            {           
               // Check numerical conditions

               if (aAttr.GetAt(i)->GetDataType() == BDNUMBER)
               {
                  double dValue = *aAttr.GetAt(i)->GetDouble();
                  //double dCondValue = pElement->GetCondValue();

                  // Column name

                  if (sFilter != "") sFilter += " and ";
                  sFilter += "`" + CString(pElement->GetColName()) + "`";

                  // Condition

                  switch (nCond)
                  {
                     case CQueryElement::greater : sFilter += " > "; break;
                     case CQueryElement::less : sFilter += " < "; break;
                     case CQueryElement::greaterequal : sFilter += " >= "; break;
                     case CQueryElement::lessequal : sFilter += " <= "; break;
                     case CQueryElement::equal : sFilter += " = "; break;
                     case CQueryElement::notequal : sFilter += " <> "; break;
                  }

                  // Value

                  CString sValue;
                  sValue.Format("%lf", dCondValue);
                  sFilter += sValue;
               }

               // Text based

               else if (aAttr.GetAt(i)->GetDataType() == BDTEXT || 
                        aAttr.GetAt(i)->GetDataType() == BDLONGTEXT)
               {
                 // Note: memo field is only included for version 3 databases as version 2 
                 // stores memo as longbinary fields which cannot be searched

                  ASSERT(aAttr.GetAt(i)->GetDataType() != BDLONGTEXT || 
                      BDGetVersion(BDHandle()) >= 3);

                  // Column name

                  if (sFilter != "") sFilter += " and ";
                  sFilter += "`" + CString(pElement->GetColName()) + "`";

                  // Condition

                  switch (nCond)
                  {                     
                     case CQueryElement::equal : sFilter += " = "; break;
                     case CQueryElement::notequal : sFilter += " <> "; break;
                     case CQueryElement::contains : sFilter += " like "; break;                        
                     case CQueryElement::notcontains : sFilter += " not like "; break;
                     case CQueryElement::like : sFilter += " like "; break;
                  }

                  // Value

                  if (nCond == CQueryElement::contains ||
                      nCond == CQueryElement::notcontains)
                  {
                     sFilter += "'%" + CString(sCondValue) + "%'";
                  } 

                  // Replace wildcard asterisks (*) with percent (%)

                  else if (nCond == CQueryElement::like)
                  {
                     sFilter += "'" + sCondValue + "'";
                     for (int i =0; i < sFilter.GetLength(); i++)
                     {
                        if (sFilter[i] == '*') sFilter.SetAt(i, '%');
                     }
                  }
                  else
                  {
                     sFilter += "'" + sCondValue + "'";
                  };
                  

               }
            }            
            
         }
      }
      pElement = pElement->GetNextQuery();
   }   
   return sFilter;
}

///////////////////////////////////////////////////////////////////////////////
//
// Validate that boolean or only data with selected links are retrieved
//

BOOL CQueryResult::CheckConditions(CQuery* pQuery, CAttrArray& aAttr)
{
   BOOL bOK = TRUE;   

  // Validate dates, if not valid ignore data but not an error
    
   if (m_bBaseFType)
   {
      CDateTime datetime(aAttr.m_lDate, 0);
      if (m_pQuery->GetDateCond() == CQuery::FirstDateRange ||
          m_pQuery->GetDateCond() == CQuery::LastDateRange ||
          m_pQuery->GetDateCond() == CQuery::AllDatesRange)
      {      
         if (datetime < m_pQuery->GetStartDate() || 
             datetime > m_pQuery->GetEndDate())
         {         
            bOK = FALSE;
         }
      }

      if (m_pQuery->GetDateCond() == CQuery::AllDatesAfter &&
          datetime < m_pQuery->GetStartDate())
      {
         bOK = FALSE;
      }

      if (m_pQuery->GetDateCond() == CQuery::AllDatesBefore &&
          datetime > m_pQuery->GetStartDate())
      {
         bOK = FALSE;
      }
   };

   // Validate attributes

   CQueryElement* pElement = pQuery->GetNextQuery();
   while (bOK && pElement != NULL && pElement->GetDataType() != BDFTYPE &&
          pElement->GetDataType() != BDQUERYSTATS)
   {                                                  
      // Attributes

      int j = 0; for (j = 0; j < aAttr.GetSize(); j++)
      {         
         if (aAttr[j]->GetAttrId() == pElement->GetAttrId())
         {
            long lValue = 0;
            long lDataType = aAttr[j]->GetDataType();
            
            if (lDataType == BDBOOLEAN) lValue = *aAttr[j]->GetBoolean();
            if (lDataType == BDLINK) lValue = *aAttr[j]->GetLink();                 

            if (lDataType == BDLINK || lDataType == BDBOOLEAN)
            {                                       
              // Search for the selected attributes

               BOOL bFound = FALSE;
               CQueryLink* pLink = (CQueryLink*)pElement;               
			   int nSelected = 0;			   
			  
			   // Search for the link among selected links in query

               int i = 0; for (i = 0; i < pLink->m_aFeatures.GetSize() && !bFound; i++)
               {      
				  if (pLink->m_aFeatures.GetAt(i).GetSelected())
				  {	
					  nSelected++;
					  if (pLink->m_aFeatures.GetAt(i).m_lId == lValue)						  
					  {
						 bFound = TRUE;
					  };
				  };
               }

			   // If all links are selected and the link contains the value 'null' then
			   // accept it

			   if (lDataType == BDLINK && lValue == AFX_RFX_LONG_PSEUDO_NULL && 
				   nSelected == pLink->m_aFeatures.GetSize())
			   {
				   bFound = TRUE;
			   }
               else if (lDataType == BDBOOLEAN && lValue == AFX_RFX_BOOL_PSEUDO_NULL && 
				   nSelected == 2)
			   {
				   bFound = TRUE;
			   }			   

               if (!bFound) 
               {                  
                  bOK = FALSE;
               };
            };            
                           
         };
      };                     

      // Get next element

      pElement = pElement->GetNextQuery();
   }                           

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////


BOOL CQueryResult::CheckValueCond(CQuery* pQuery, CAttrArray& aAttr)
{  
  // Search for value based conditions in the query

   CQueryElement* pElement = pQuery->GetNextQuery();
   while (pElement != NULL)
   {
      int j = 0; for (j = 0; j < pElement->GetConditions().GetSize(); j++)
      {
         int nCond = pElement->GetConditions().GetAt(j).GetCond();
         double dCondValue = pElement->GetConditions().GetAt(j).GetCondValue();
         CString sCondValue = pElement->GetConditions().GetAt(j).GetCondValueStr();

         // Find the corresponding attribute

         int i = 0; for (i = 0; i < aAttr.GetSize(); i++)
         {
            if (aAttr.GetAt(i)->GetAttrId() == pElement->GetAttrId() && 
                aAttr.GetAt(i)->GetFTypeId() == pElement->GetFTypeId())
            {  
               BOOL bOK = TRUE;


               // Check text based conditions for links as the SQL code for this would be
               // messy as the functionality is currently spit across the database API and the
               // NRDB Pro application

               if (aAttr.GetAt(i)->GetDataType() == BDLINK)
               {               
                  CString sValue = aAttr.GetAt(i)->AsString();
                  //CString sCondValue = pElement->GetCondValueStr();

                  // None case sensitive comparison (CString::Find is case sensitive?)

                  sValue.MakeUpper();
                  sCondValue.MakeUpper();

                  switch (nCond)
                  {
                     case CQueryElement::equal : bOK = (sValue == sCondValue); break;
                     case CQueryElement::notequal : bOK = (sValue != sCondValue); break;
                     case CQueryElement::contains: bOK = sValue.Find(sCondValue) != -1; break;                       
                     case CQueryElement::notcontains: bOK = sValue.Find(sCondValue) == -1; break;                       
                  }
               }
               if (!bOK) return FALSE;
            }               
         }
      }


      pElement = pElement->GetNextQuery();
   }
   return TRUE;   
}


///////////////////////////////////////////////////////////////////////////////
//
// Determines if any attributes or conditions of the ftype are selected in the
// query
//

BOOL CQueryResult::IsFTypeSelected(CQuery* pQuery, long lFType)
{
   BOOL bFType = FALSE;

   CQueryElement* pElement = pQuery;
   while (pElement != NULL)
   {
      if (pElement->GetFTypeId() == lFType)
      {
         // Check if attribute selected

         if (pElement->GetSelected())
         {
            if (pElement->GetDataType() == BDFTYPE || 
                pElement->GetDataType() == BDFEATURE)
            {
                bFType = TRUE;
            } else
            {
               return yes;
            };
         };

         // Check if conditional

         if (pElement->GetDataType() == BDLINK ||
             pElement->GetDataType() == BDBOOLEAN)
         {
            CQueryLink* pLink = (CQueryLink*)pElement;

            int i = 0; for (i = 0; i < pLink->m_aFeatures.GetSize(); i++)
            {
               if (!pLink->m_aFeatures[i].GetSelected()) return yes;
            }
         }

         // Check for numerical conditions

         if (pElement->GetConditions().GetSize() > 0)
         {
            return yes;
         }
      }

      // Check if used as part of a join

      if (pElement->GetQueryJoin() != NULL)
      {
         return yes;
      }

      // Check if statistics require the feature type

      if (pElement->GetDataType() == BDQUERYSTATS)
      {
         CQueryCalc* pStats = (CQueryCalc*)pElement;
         if (pStats->GetFTypeId() == lFType || 
             pStats->GetFTypeId2() == lFType)
         {
            return yes;
         }
      }
   
      pElement = pElement->GetNextQuery();
   }
   if (bFType) return ftype;
   else return no;
}

///////////////////////////////////////////////////////////////////////////////
//
// CQueryResult::UpdateDates
//
// This function, according to the date condition, will replace the existing
// attributes retrieved.  If the date condition allows multiple dates then a 
// query attribute array is inserted into the list at the index (i) position, retaining sort order

int CQueryResult::UpdateDates(CQuery* pQuery, CAttrArray& aAttr, CQueryAttrArray* pAttrArray, int index)
{   

   BOOL nStatus = Replace;   

   // Check the dates

   if (pAttrArray->GetSize() > 0 && pAttrArray->m_dateActive.IsValid())
   {      
      CDateTime dtC = pAttrArray->m_dateActive;

      CDateTime dtN = CDateTime(aAttr.m_lDate, 0);

      switch (m_pQuery->GetDateCond())
      {
         case CQuery::FirstDate : case CQuery::FirstDateRange :
            if (dtN > dtC) nStatus = Keep;
            if (dtN == dtC) nStatus = New;
            break;
         case CQuery::LastDate : case CQuery::LastDateRange :
            if (dtN < dtC) nStatus = Keep;
            if (dtN == dtC) nStatus = New;
            break;
         default : 
            if (m_bBaseFType) nStatus = New;            
            break;
      };                    
   }

   // If okay then delete the existing attributes with this feature
   // type
   
   if (nStatus == Replace)
   {
      // Remove all attributes for this ftype except BDFEATURE

      int i = 0; for (i = 0; i < pAttrArray->GetSize(); i++)
      {
         CAttribute* pAttr = pAttrArray->GetAt(i);
         if (pAttr->GetFTypeId() == pQuery->GetFType() &&
             pAttr->GetDataType() != BDFEATURE)
         {
            delete pAttr;
            pAttrArray->RemoveAt(i);
            i--;
         }
      };      
   };

   // When dates are the same create a 2nd feature, this will be added to the
   // end of the queryresult list and so will have its attributes updated later
   // in the list

   if (nStatus == New)
   {
       CQueryAttrArray* pQAA = new CQueryAttrArray(*pAttrArray);
       pQAA->m_lDate = 0; // Causes data to be overwritten
       pQAA->m_dateActive = CDateTime(0,0);
       
       int i = 0; for (i = 0; i < pQAA->GetSize(); i++)
       {
          CAttribute* pAttr = pQAA->GetAt(i);
          if (pAttr->GetDataType() != BDFEATURE)
          {
             delete pAttr;
             pQAA->RemoveAt(i);
             i--;
          }
       }
       InsertAt(index+1, pQAA);
       //Add(pQAA);
   }

   return nStatus;
}

///////////////////////////////////////////////////////////////////////////////
//
// Retrieve the features named in the query.  This is separate from data as
// some parent attributes may have no attributes set
//

void CQueryResult::RetrieveFeatures(CQuery* pQuery, CQueryAttrArray* pAttrArray)
{
   // Ensure that the feature does not already exist 
   // (one to one or both BDFTYPE & BDFEATURE selected)

   int i = 0; for (i = 0; i < pAttrArray->GetSize(); i++)
   {
      if (pAttrArray->GetAt(i)->GetAttrId() == BDFEATURE &&
          pAttrArray->GetAt(i)->GetFTypeId() == pQuery->GetFType())
      {
         return;
      }
   }

   // Searh to see if the ftype is selected

   CQueryElement* pElement = pQuery;
   while (pElement != NULL)
   {
      if ((pElement->GetDataType() == BDFTYPE || 
          pElement->GetDataType() == BDFEATURE) &&
          pElement->GetSelected())
      {
         
         // Insert the feature at the beginning

         CAttribute* pAttr = new CAttribute(BDFEATURE);            
         pAttr->SetAttrId(BDFEATURE);
         pAttr->SetFTypeId(pQuery->GetFType());
         pAttr->SetDesc(pAttrArray->m_sFeatureActive);
         pAttrArray->InsertAt(0, pAttr);
         break;   
      }

      // Get next element

      pElement = pElement->GetNextQuery();
      if (pElement != NULL && pElement->GetDataType() == BDFTYPE) break;
   }
}

///////////////////////////////////////////////////////////////////////////////

BOOL CQueryResult::RetrieveAttr(CQuery* pQuery, CAttrArray& aAttr, 
                                CQueryAttrArray* pAttrArray)
{
   BOOL bOK = TRUE;
   BOOL bFound = FALSE;
   CString sDate, sTime;   

   // Determine which attributes are required
   
   CQueryElement* pElement = pQuery;   

   ASSERT(pElement != NULL);   
   do
   {                            
      if (pElement->GetSelected())
      {
         bFound = FALSE;

          // Feature name
                  
         if ((long)pElement->GetDataType() == BDFEATURE ||
             (long)pElement->GetDataType() == BDFTYPE)
         {
            bFound = TRUE;
         }          

         // Date

         if ((long)pElement->GetDataType() == BDDATE)
         {
            CAttribute* pAttr = new CAttribute(BDDATE);         
            pAttr->SetAttrId(BDDATE);            
            pAttr->SetFTypeId(pQuery->GetFType());
            pAttr->SetDate(aAttr.m_lDate);
            CDateTime date(aAttr.m_lDate, 0);            
            date.DateAsString(sDate);            
            pAttr->SetDesc(sDate.GetBuffer(0));
            pAttrArray->Add(pAttr);
            bFound = TRUE;
         }

         // Other attributes
         
         int j = 0; for (j = 0; !bFound && j < aAttr.GetSize(); j++)
         {
            if (aAttr[j]->GetAttrId() == pElement->GetAttrId())
            {
               CAttribute* pAttr = new CAttribute(aAttr[j]->GetDataType());
               *pAttr = *aAttr[j];
            
               pAttrArray->Add(pAttr);                              
               bFound = TRUE;
            };
         };                     
      }   
      // Store the date of the data
      
      pAttrArray->m_dateActive = CDateTime(aAttr.m_lDate, 0);      

      // Get next element

      pElement = pElement->GetNextQuery();
   }
   while (pElement != NULL && pElement->GetDataType() != BDFTYPE);

   // Retrieve data for statistics

   pElement = m_pQuery;
   while (pElement != NULL)
   {
      if (pElement->GetDataType() == BDQUERYSTATS)
      {
         CQueryCalc* pStats = (CQueryCalc*)pElement;
         int j = 0; for (j = 0; j < aAttr.GetSize(); j++)
         {
            if ((aAttr[j]->GetAttrId() == pStats->GetAttrId() &&
                aAttr[j]->GetFTypeId() == pStats->GetFTypeId()) ||
                (aAttr[j]->GetAttrId() == pStats->GetAttrId2() &&
                 aAttr[j]->GetFTypeId() == pStats->GetFTypeId2()))
            {
               CAttribute* pAttr = new CAttribute(aAttr[j]->GetDataType());
               *pAttr = *aAttr[j];               
            
               pAttrArray->Add(pAttr);                              
               bFound = TRUE;
            };
         };                     
      }
      pElement = pElement->GetNextQuery();
   };

   // Retrieve data for joins where attribute isn't selected

   pElement = m_pQuery;
   while (pElement != NULL)
   {
      if (pElement->GetQueryJoin() != NULL && !pElement->GetSelected())
      {
         int j = 0; for (j = 0; j < aAttr.GetSize(); j++)
         {
            if (aAttr[j]->GetAttrId() == pElement->GetAttrId())
            {
                CAttribute* pAttr = new CAttribute(aAttr[j]->GetDataType());
                *pAttr = *aAttr[j];            
                pAttrArray->Add(pAttr);                              
                break;
            }
         };
      }
      pElement = pElement->GetNextQuery();
   }
 

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////
//
// For each attribute, if it has no value assigned then assign its value can
// be replaced with another attribute meeting the required conditions
//

BOOL CQueryResult::RetrieveMissing(CQuery* pQuery, CAttrArray& aAttr, 
                                CQueryAttrArray* pAttrArray)
{
   BOOL bOK = TRUE;

   int i = 0; for (i = 0; i < aAttr.GetSize(); i++)
   {
      int j = 0; for (j = 0; j < pAttrArray->GetSize(); j++)
      {
         if (aAttr[i]->GetFTypeId() == pAttrArray->GetAt(j)->GetFTypeId() &&
             aAttr[i]->GetAttrId() == pAttrArray->GetAt(j)->GetAttrId())
         {
            CAttribute* pAttr = pAttrArray->GetAt(j);
            if (pAttr->IsNull() && !aAttr[i]->IsNull())
            {
             // Only do this for coordinate and maplines, otherwise
             // user may be confused when value is presented with wrong date

               if (pAttr->GetDataType() == BDMAPLINES || pAttr->GetDataType() == BDCOORD)
               {
                  delete pAttr;
                  pAttr = new CAttribute(aAttr[i]->GetDataType());
                  *pAttr = *aAttr[i];            
                  pAttrArray->SetAt(j, pAttr);
               };
            }
         }             
      }
   }

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////
//
// Retrieves the feature names if selected
//

void CQueryResult::InitNullAttr(CQuery* pQuery, CAttrArray& aAttr, CQueryAttrArray* pAttrArray)
{
   // Determine which attributes are required
   
   CQueryElement* pElement = pQuery;   

   ASSERT(pElement != NULL);   
   do
   {                            
      if (pElement->GetSelected())
      {
          // Feature name

         if ((long)pElement->GetDataType() == BDFEATURE ||
             (long)pElement->GetDataType() == BDFTYPE)
         {
			 // Skip
         }        
         else if ((long)pElement->GetDataType() == BDDATE)
         {
            // Add blank date

            CAttribute* pAttr = new CAttribute(BDDATE);         
            pAttr->SetAttrId(BDDATE);            
            pAttr->SetFTypeId(pQuery->GetFType());            
            pAttr->SetDesc("");
            pAttrArray->Add(pAttr);            
         }
         else
         {
            // Add blank attribute

            int j = 0; for (j = 0; j < aAttr.GetSize(); j++)
            {
               if (aAttr[j]->GetAttrId() == pElement->GetAttrId())
               {
                  CAttribute* pAttr = new CAttribute(aAttr[j]->GetDataType());                                
                  pAttr->SetDesc(aAttr[j]->GetDesc());
                  pAttrArray->Add(pAttr);                              
                  break;
               };
            };                     
         };
      };
       pElement = pElement->GetNextQuery();
   }
   while (pElement != NULL && pElement->GetDataType() != BDFTYPE);

}

///////////////////////////////////////////////////////////////////////////////
//
// Performs calculations whereby two attributes for the same feature will
// be combined using a mathematic operator
//

void CQueryResult::CalcRowBasedStats()
{
   int iAttr = -1;

   // Find whether statistics are required

   CQueryElement* pElement = m_pQuery;
   while (pElement != NULL)
   {
      if (pElement->GetDataType() == BDQUERYSTATS)
      {
         int nDP = 0;

         CQueryCalc* pStats = (CQueryCalc*)pElement;

         BDProgressRange(0, GetSize());
         BDProgressText("Performing calculations...");
         
         int i = 0; for (i = 0; i < GetSize(); i++)
         {
            CalcRowBasedStats(pStats, *GetAt(i), iAttr, nDP);

            BDProgressPos(i);
         }
         iAttr--; 
      }        
      pElement = pElement->GetNextQuery();
   }
}

///////////////////////////////////////////////////////////////////////////////

void CQueryResult::CalcRowBasedStats(CQueryCalc* pStats, CQueryAttrArray& aAttr, int iAttr, int nDP)
{
   CAttribute *pAttr1 = NULL, *pAttr2 = NULL;   
   double d1 = AFX_RFX_DOUBLE_PSEUDO_NULL, d2 = AFX_RFX_DOUBLE_PSEUDO_NULL;
   int iAttr1 = -1, iAttr2 = -1;
   CLongCoord coord;

  // Find attributes used for the statistic

    int i = 0; for (i = 0; i < aAttr.GetSize(); i++)
    {
       if (aAttr.GetAt(i)->GetFTypeId() == pStats->GetFTypeId() &&
           aAttr.GetAt(i)->GetAttrId() == pStats->GetAttrId())
       {
          pAttr1 = aAttr.GetAt(i);          
          iAttr1 = i;
       }
       if (aAttr.GetAt(i)->GetFTypeId() == pStats->GetFTypeId2() &&
           aAttr.GetAt(i)->GetAttrId() == pStats->GetAttrId2())
       {
          pAttr2 = aAttr.GetAt(i);          
          iAttr2 = i;
       }
    }    

    // Determine if a value is to be used

    if (pStats->GetAttrId() == 0) d1 = pStats->GetValue1();
    if (pStats->GetAttrId2() == 0 && pStats->GetStatistic() != CQueryCalc::none) d2 = pStats->GetValue2();

    // Perform spatial calculations

    if (pStats->GetQualifier() == BDAREA)
    {       
       CLongLines longlines(*pAttr1->GetLongBinary());  
	   CSpatialPoly polygon(&longlines);
       if (!polygon.CalcArea(d1)) d1 = AFX_RFX_DOUBLE_PSEUDO_NULL;       
    };
    if (pStats->GetQualifier2() == BDAREA)
    {
       CLongLines longlines(*pAttr2->GetLongBinary());       
	   CSpatialPoly polygon(&longlines);
       if (!polygon.CalcArea(d2)) d2 = AFX_RFX_DOUBLE_PSEUDO_NULL;       
    }
    if (pStats->GetQualifier() == BDLENGTH)
    {       
       CLongLines longlines(*pAttr1->GetLongBinary());       
       d1 = CSpatial::CalcLength(&longlines);
    };
    if (pStats->GetQualifier2() == BDLENGTH)
    {       
       CLongLines longlines(*pAttr2->GetLongBinary());       
       d2 = CSpatial::CalcLength(&longlines);
    };
    if (pStats->GetQualifier() == BDCOORD)
    {
       CSpatialPoly polygon(*pAttr1->GetLongBinary());
       polygon.GetCentroid(coord);
    }

    // Perform calculation

    double dResult = AFX_RFX_DOUBLE_PSEUDO_NULL;

    // Numeric statistic

    if (pStats->GetAttrId() == 0 || pAttr1->GetDataType() == BDNUMBER ||
        !IsNullDouble(d1))
    {
       if (pAttr1 != NULL && IsNullDouble(d1)) d1 = *pAttr1->GetDouble();
       if (pAttr2 != NULL && IsNullDouble(d2)) d2 = *pAttr2->GetDouble();

       // Check for divide by zero

       if ((pStats->GetStatistic() == CQueryCalc::divide || 
           pStats->GetStatistic() == CQueryCalc::percent) && 
           d1 == 0)
       {
          // Ignore
       }
       else if (!IsNullDouble(d1) && 
                !IsNullDouble(d2))
       {       
          switch(pStats->GetStatistic())
          {
             case CQueryCalc::add : dResult = d1+d2; break;
             case CQueryCalc::subtract : dResult = d1-d2; break;
             case CQueryCalc::multiply : dResult = d1*d2; break;
             case CQueryCalc::divide : if (d2 != 0) dResult = d1/d2; break;
             case CQueryCalc::percent : if (d2 != 0) dResult = d1/d2*100; break;
          }
       } else if (!IsNullDouble(d1) && pStats->GetStatistic() == CQueryCalc::none)
       {
          dResult = d1;
       }
    }

    // Allow user to apply operations to coordinates

    if (pStats->GetQualifier() == BDCOORD && pStats->GetStatistic() > CQueryCalc::none)
    {
       if (pAttr2 != NULL && IsNullDouble(d2)) d2 = *pAttr2->GetDouble();
       if (!IsNullDouble(d2))
       {
          switch(pStats->GetStatistic())
          {
              case CQueryCalc::add : coord.x = (long)(coord.x + d2); coord.y = (long)(coord.y + d2); break;
              case CQueryCalc::subtract : coord.x = (long)(coord.x - d2); coord.y = (long)(coord.y - d2); break;
              case CQueryCalc::multiply : coord.x = (long)(coord.x * d2); coord.y = (long)(coord.y * d2); break;
              case CQueryCalc::divide : if (d2 != 0) {coord.x = (long)(coord.x * d2); coord.y = (long)(coord.y * d2);} else coord.SetNull(); break;
              case CQueryCalc::percent : if (d2 != 0) coord.SetNull(); break;
          }
       }
    }
    
    dResult = SetDP(dResult, pStats->GetDecPlaces());
  
    // Substitute values for result

    if (pStats->GetQualifier() == BDCOORD)
    {
       CAttribute* pAttr = new CAttribute(BDCOORD);
       pAttr->SetCoord(CCoord(coord.x,coord.y));
       pAttr->SetDesc(pStats->GetDesc());
       pAttr->SetAttrId(iAttr);
       pAttr->SetFTypeId(m_pQuery->GetFType());
       aAttr.Add(pAttr);    
    } 
    else
    {
       CAttribute* pAttr = new CAttribute(BDNUMBER);
       pAttr->SetDouble(dResult);
       pAttr->SetDesc(pStats->GetDesc());
       pAttr->SetAttrId(iAttr);
       pAttr->SetFTypeId(m_pQuery->GetFType());
       aAttr.Add(pAttr);    
    }

  // Remove the attributes from the list 
    
   if (pAttr1 != NULL)
   {
     aAttr.RemoveAt(iAttr1);
     delete pAttr1;      
   };

   if (pAttr1 != pAttr2 && pAttr2 != NULL)
   {
     if (iAttr2 > iAttr1 && iAttr1 != -1) iAttr2--;
     aAttr.RemoveAt(iAttr2);
     delete pAttr2;       
   };   
}


///////////////////////////////////////////////////////////////////////////////

double CQueryResult::SetDP(double dResult, int nDP)
{
   int n = 1;
   int i = 0; for (i = 0; i < nDP; i++) n *= 10;
   
   return floor(dResult * n + 0.5) / n;
}

///////////////////////////////////////////////////////////////////////////////
//
// This routine will sort the results of the query according to the selected
// elements
//

void CQueryResult::SortQueryResults()
{
   CQueryResult aSorted;

   BDProgressRange(0, GetSize());
   BDProgressText("Sorting...");

   if (GetSize() > 0)
   {
	   SortQueryResults(0, GetSize()-1);
   }
}

///////////////////////////////////////////////////////////////////////////////

void CQueryResult::SortQueryResults(int left, int right)
{
  int pivot,l_hold, r_hold;
  CQueryAttrArray *pQueryAttr;

  l_hold = left;
  r_hold = right;
  pQueryAttr = GetAt(left);
  while (left < right)
  {
    while (Compare(GetAt(right), pQueryAttr) >= 0 && left < right)
      right--;
    if (left != right)
    {
	  SetAt(left,GetAt(right));
      left++;
    }
    while (Compare(GetAt(left), pQueryAttr) <= 0 && left < right)
      left++;
    if (left != right)
    {
	  SetAt(right,GetAt(left)); 
      right--;
    }
  }
  SetAt(left, pQueryAttr);
  pivot = left;
  left = l_hold;
  right = r_hold;
  if (left < pivot)
    SortQueryResults(left, pivot-1);
  if (right > pivot)
    SortQueryResults(pivot+1, right);
}


///////////////////////////////////////////////////////////////////////////////
//
// Compares two arrays of attributes according to the items selection in CQuery
// returns 0 if they are equal, or negative if A occurs first or positive if
// B occurs first.
//

int CQueryResult::Compare(CQueryAttrArray* pAttrA, CQueryAttrArray* pAttrB, int nSortId)
{
   int nCompare = 0;   
   int nAttrStats = 0;
   BOOL bFound = FALSE;

   // Sort by multiple attributes from 1 upwards until none found

   int i = 0; for (i = 0; i < m_aAttrSel.GetSize(); i++)
   {
      if (m_aAttrSel[i].m_nSortBy == nSortId)
      {
         bFound = TRUE;

         // Search for attribute

         int j = 0; for (j = 0; j < pAttrA->GetSize(); j++)
         {
            CAttribute* pA = pAttrA->GetAt(j);
            CAttribute* pB = pAttrB->GetAt(j);

            if (pA->GetAttrId() == m_aAttrSel[i].m_lAttr &&
                pA->GetFTypeId() == m_aAttrSel[i].m_lFType)
            {
               nCompare = Compare(pA, pB);
               break;
            }
         };
         break;            
      }      
   }

   // If the results are the same then check if there is another sort attribute

   if (nCompare == 0)
   {
      int i = 0; for (i = 0; i < m_aAttrSel.GetSize(); i++)
      {
         if (m_aAttrSel[i].m_nSortBy == nSortId+1)
         {
            nCompare = Compare(pAttrA, pAttrB, nSortId+1);
            break;
         };
      }
   };

   return nCompare;
}

///////////////////////////////////////////////////////////////////////////////
//
// Compares two attributes

int CQueryResult::Compare(CAttribute* pA, CAttribute* pB)
{
   int nCompare = 0;   

   // Numbers

   if (pA->GetDataType() == BDNUMBER)
   {
      if (!IsNullDouble(*pA->GetDouble()) && 
          !IsNullDouble(*pB->GetDouble()))
      {
         if (*pA->GetDouble() < *pB->GetDouble()) nCompare = -1;
         else if (*pA->GetDouble() > *pB->GetDouble()) nCompare = 1;
      }
      else if (IsNullDouble(*pA->GetDouble()) &&
               !IsNullDouble(*pB->GetDouble()))
      {
         nCompare = -1;
      }
      else if (!IsNullDouble(*pA->GetDouble()) &&
               IsNullDouble(*pB->GetDouble()))
      {
         nCompare = 1;
      }

   }

   // Dates
   else if (pA->GetDataType() == BDDATE)
   {
      if (CDateTime(*pA->GetDate(),0) < CDateTime(*pB->GetDate(),0)) nCompare = -1;
      else if (CDateTime(*pA->GetDate(),0) > CDateTime(*pB->GetDate(),0)) nCompare = 1;                  

   }

   // Compare map lines

   else if (pA->GetDataType() == BDMAPLINES)
   {
      CLongBinary *pLongBin1 = pA->GetLongBinary();
      CLongBinary *pLongBin2 = pB->GetLongBinary();

      if (pLongBin1->m_dwDataLength == pLongBin2->m_dwDataLength)
      {
         CLongLines maplines1(*pLongBin1);
         CLongLines maplines2(*pLongBin2);

         if (maplines1.GetSize() == maplines2.GetSize())
         {         
            int i = 0; for (i = 0; i < maplines1.GetSize() && nCompare == 0; i++)
            {
               if (maplines1.GetAt(i) != maplines2.GetAt(i)) 
			   {
				   nCompare = 1; // Not equal
				   break;
			   }
            }         
         } else
         {
            nCompare = 1; // Not equal
         }
      } 
      else
      {
         nCompare = 1; // Not equal
      }
   }

   // Text

   else 
   {
      if (pA->AsString() < pB->AsString()) nCompare = -1;
      else if (pA->AsString() > pB->AsString()) nCompare = 1;            
   }

   return nCompare;

}

///////////////////////////////////////////////////////////////////////////////
//
// Calculate the results of statistics
//

void CQueryResult::CalculateStatistics()
{
   CQueryResult aGroupAttr;      
   CQueryAttrSel attrselgroup, attrselstat;;

   // Determine the attribute to group by

   int i = 0; for (i = 0; i < m_aAttrSel.GetSize(); i++)
   {
      if (m_aAttrSel[i].m_nGroupBy)
      {
         attrselgroup = m_aAttrSel[i];
         m_nGroupBy = m_aAttrSel[i].m_nGroupBy;
      }
      if (m_aAttrSel[i].m_nStatGroup)
      {
         attrselstat = m_aAttrSel[i];
      }
   }

   // Test if data is to be sorted into groups

   if (attrselstat.m_lFType != 0)
   {
      // Create groups

      CalculateStatistics(aGroupAttr, attrselgroup, attrselstat);

      // Copy the query result into the queryresult

      m_pQueryResult = new CQueryResult;
      m_pQueryResult->Copy(*this);
      m_pQueryResult->m_aAttrSel.Copy(m_aAttrSel);
      RemoveAll(); // Remove without deleting data
   
      // Replace the current result set with the statistical one
      
      Copy(aGroupAttr);

      // Remove elements without freeing memory as pointers have been transfered

      aGroupAttr.RemoveAll();
   };
}

///////////////////////////////////////////////////////////////////////////////

void CQueryResult::CalculateStatistics(CQueryResult& aGroupAttr, CQueryAttrSel& attrselgroup, 
                                       CQueryAttrSel& attrselstat)
{
   CAttribute* pAttrG = NULL;
   CAttribute* pAttrS = NULL;
   CQueryAttrArray* paAttrG = NULL;

   // Search through the list of existing attributes and determine if the
   // group by attribute already exists

   BDProgressRange(0, GetSize());

   int i = 0; for (i = 0; i < GetSize(); i++)
   {
      pAttrG = NULL;
      pAttrS = NULL;
      CQueryAttrArray& aAttr = *GetAt(i);
      int j = 0; for (j = 0; j < aAttr.GetSize(); j++)
      {
         // Search for the attribute corresponding to the sorted and the
         // group attribute

         CAttribute* pAttr = aAttr.GetAt(j);
         if (pAttr->GetFTypeId() == attrselgroup.m_lFType &&
             pAttr->GetAttrId() == attrselgroup.m_lAttr)
         {
            pAttrG = pAttr;
         }
         if (pAttr->GetFTypeId() == attrselstat.m_lFType &&
             pAttr->GetAttrId() == attrselstat.m_lAttr)
         {
            pAttrS = pAttr;
         }
      };      
 
       if (pAttrS == NULL) attrselstat.m_nStatGroup = CQuery::count;          

       DetermineGroup(aGroupAttr,paAttrG, pAttrG, pAttrS, attrselstat.m_nStatGroup);

       // Update the statitics
                    
       UpdateStatistics(paAttrG, pAttrS, attrselstat.m_nStatGroup);              

       BDProgressPos(i);
   }

   // For unique count it is necessary to remove the temporary data

   for (i = 0; i < aGroupAttr.GetSize(); i++)
   {
      CQueryAttrArray* paAttrG = aGroupAttr.GetAt(i);
      int j = 0; for (j = 0; j < paAttrG->GetSize(); j++)
      {
         CAttribute* pAttr = paAttrG->GetAt(j);
         if (pAttr->GetAttrId() != GROUP && 
             pAttr->GetAttrId() != COUNT && 
             pAttr->GetAttrId() != STATISTIC)
         {
            delete pAttr;
            paAttrG->RemoveAt(j);
            j--;
         }
      }      
   }

   // Update the attribute selected array, changing the data type of 
   // the statistical link to number

   CQueryAttrSel attrsel;
   m_aAttrSel.RemoveAll();
   CQueryAttrSel attrselN;      
   if (pAttrG != NULL) attrsel.m_lFType = pAttrG->GetFTypeId(); 
   else attrsel.m_lFType = m_pQuery->GetFType();
   attrsel.m_lAttr = GROUP;
   if (pAttrG != NULL) attrsel.m_lDataType = pAttrG->GetDataType();
   else attrsel.m_lDataType = BDTEXT;
   m_aAttrSel.Add(attrsel);

   if (pAttrS != NULL) attrsel.m_lFType = pAttrS->GetFTypeId();
   else attrsel.m_lFType = m_pQuery->GetFTypeId();

   attrsel.m_lAttr = STATISTIC;
   attrsel.m_lDataType = BDNUMBER;   
   m_aAttrSel.Add(attrsel);

   BDProgressRange(0,0);
}

///////////////////////////////////////////////////////////////////////////////
//
// Creates a group for the storage of statistics
//

void CQueryResult::DetermineGroup(CQueryResult& aGroupAttr, CQueryAttrArray*& paAttrG, 
                                  CAttribute* pAttrG, CAttribute* pAttrS, int nStat)
{
   CString s;
   BOOL bFound = FALSE;  
   CAttribute* pAttrN = NULL;

      // Allow comparison of year and month as well as date
      // Nb. Leave m_sDesc as this is used in reports
      
      if (pAttrG != NULL && pAttrG->GetDataType() == BDDATE)
      {     
         CDateTime  dtG = CDateTime(*pAttrG->GetDate(), 0);
   
         if (m_nGroupBy == CQuery::month) 
         {
            dtG = CDateTime(dtG.GetYear(), dtG.GetMonth(), 0);    
         }
         if (m_nGroupBy == CQuery::year) 
         {
            dtG = CDateTime(dtG.GetYear(), 0, 0);      
         }      
         pAttrG->SetDate(dtG.GetDateLong());                        
      };      
   
   paAttrG = NULL;
   int k = 0; for (k = 0; k < aGroupAttr.GetSize() && !bFound; k++)
   {                
      paAttrG = aGroupAttr.GetAt(k);
      CAttribute* pGroup = paAttrG->GetAt(0);

      // No groups, create universal group if it doesn't exist

      if (pAttrG == NULL)
      {
         if (paAttrG->GetSize() == 0) break;
         else bFound = TRUE;
      }
      else if (Compare(pGroup, pAttrG) == 0) bFound = TRUE;
   };
             
   if (!bFound)
   {
      if (pAttrG != NULL) paAttrG = new CQueryAttrArray(pAttrG->GetDesc());
      else paAttrG = new CQueryAttrArray("");
      
      paAttrG->m_lFeature = 0;
      paAttrG->m_lFType = 0;

      if (pAttrG != NULL)
      {                  
         // Initialise the link name
                  
         pAttrN = new CAttribute(*pAttrG);                  
         pAttrN->SetAttrId(GROUP);
         pAttrN->SetFTypeId(pAttrG->GetFTypeId());      

         if (pAttrN->GetDataType() == BDDATE)
         {
            CDateTime dtN = CDateTime(*pAttrN->GetDate(),0);
            dtN.DateAsString(s);
            pAttrN->SetDesc(s);
         }
         
         paAttrG->Add(pAttrN);
      };

      // Initialise the value to hold each value
      
      if (nStat == CQuery::count)  s = BDString(IDS_COUNT);
      else if (nStat == CQuery::total)  s = BDString(IDS_TOTAL);
      else if (nStat == CQuery::countunique) s = BDString(IDS_COUNTUNIQUE);
      else if (nStat == CQuery::mean)  s = BDString(IDS_MEAN);
      else if (nStat == CQuery::max)  s = BDString(IDS_MAX);
      else if (nStat == CQuery::min)  s = BDString(IDS_MIN);      

      pAttrN = new CAttribute(BDNUMBER);         
      pAttrN->SetAttrId(STATISTIC);

      if (pAttrS != NULL) pAttrN->SetFTypeId(pAttrS->GetFTypeId()); 
      else (pAttrN->SetFTypeId(m_pQuery->GetFType()));

      // Create name for attribute, avoid repeating statistic name e.g. Total Total

      if (nStat == CQuery::count)
      { 
          if (pAttrG != NULL)
          {
             s += " " + CString(m_pQuery->GetDesc()).Left(CString(m_pQuery->GetDesc()).Find('['));             
          };
          pAttrN->SetDesc(s); 
      } else
      {
         if (pAttrS != NULL) 
         {
            if (pAttrS->GetDesc().Find(s) == -1) pAttrN->SetDesc(s + " " + pAttrS->GetDesc());          
            else pAttrN->SetDesc(pAttrS->GetDesc());
         }
         else pAttrN->SetDesc(s); 
      };
      
      pAttrN->SetDouble(0);
      paAttrG->Add(pAttrN);

      // Initialise the value to hold each value

      if (nStat != CQuery::count)
      {
         pAttrN = new CAttribute(BDNUMBER);         
         pAttrN->SetAttrId(COUNT);
         pAttrN->SetFTypeId(m_pQuery->GetFType());
         pAttrN->SetDesc("Count"); 
         pAttrN->SetDouble(0);
         paAttrG->Add(pAttrN);
      };

      aGroupAttr.Add(paAttrG);                         
   };
}

///////////////////////////////////////////////////////////////////////////////
//
// Update the statistics
//

void CQueryResult::UpdateStatistics(CQueryAttrArray*& paAttrG, CAttribute* pAttr, int nStat)
{    
   CAttribute *pAttr1 = NULL, *pAttr2 = NULL;   

   int i = 0; for (i = 0; paAttrG->GetAt(i)->GetAttrId() != STATISTIC; i++) {};
   pAttr1 = paAttrG->GetAt(i);
          
   if (nStat != CQuery::count)
   {
      int i = 0; for (i = 0; paAttrG->GetAt(i)->GetAttrId() != COUNT; i++) {};
      pAttr2 = paAttrG->GetAt(i);
   };

   // Update statistic

   if (nStat == CQuery::count)
   {
      pAttr1->SetDouble(*pAttr1->GetDouble()+1);   
   } 

   // For unique count the attributes are added to the end of the group

   else if (nStat == CQuery::countunique)
   {
      // First search to see if the attribute already exists for the group
      // Skip first column

      for (int i = 1; i < paAttrG->GetSize(); i++)
      {
         CAttribute* pAttrG = paAttrG->GetAt(i);

         if (pAttrG->GetAttrId() != STATISTIC && pAttrG->GetAttrId() != COUNT)
         {
            if (Compare(pAttrG,  pAttr) == 0) break;         
         };
      }
      // Add to list if new and update count
      if (i == paAttrG->GetSize())
      {
         pAttr1->SetDouble(*pAttr1->GetDouble()+1);
         paAttrG->Add(new CAttribute(*pAttr)); 
      }

      // Update count

      pAttr2->SetDouble(*pAttr2->GetDouble()+1);
   }

   // Numeric statistics

   else if (!IsNullDouble(*pAttr->GetDouble()))
   {      
      
      if (nStat == CQuery::total)
      {
         pAttr1->SetDouble(*pAttr1->GetDouble() + *pAttr->GetDouble());
      } 
      else if (nStat == CQuery::mean)
      {
         double dTotal = (*pAttr1->GetDouble()) * (*pAttr2->GetDouble());
         pAttr1->SetDouble((dTotal + *pAttr->GetDouble()) / (*pAttr2->GetDouble()+1));
      }
      else if (nStat == CQuery::max)
      {
         if (*pAttr->GetDouble() > *pAttr1->GetDouble())
         {
            pAttr1->SetDouble(*pAttr->GetDouble());
         }
      }
      else if (nStat == CQuery::min)
      {
         if (*pAttr->GetDouble() < *pAttr1->GetDouble())
         {
            pAttr1->SetDouble(*pAttr->GetDouble());
         }
      }
   
      // Update Count
   
      if (nStat != CQuery::count)
      {
         pAttr2->SetDouble(*pAttr2->GetDouble()+1);   
      };
   }
}

///////////////////////////////////////////////////////////////////////////////
//
// Sorts the array of query attributes by feature for fast searching
//

void CQueryResult::SortFeature(int left, int right)
{
  int pivot,l_hold, r_hold;
  CQueryAttrArray *pQueryAttr;

  l_hold = left;
  r_hold = right;
  pQueryAttr = GetAt(left);
  while (left < right)
  {
    while (GetAt(right)->GetActiveFeature() >= pQueryAttr->GetActiveFeature() && left < right)
      right--;
    if (left != right)
    {
	  SetAt(left,GetAt(right));
      left++;
    }
    while (GetAt(left)->GetActiveFeature() <= pQueryAttr->GetActiveFeature() && left < right)
      left++;
    if (left != right)
    {
	  SetAt(right,GetAt(left)); 
      right--;
    }
  }
  SetAt(left, pQueryAttr);
  pivot = left;
  left = l_hold;
  right = r_hold;
  if (left < pivot)
    SortFeature(left, pivot-1);
  if (right > pivot)
    SortFeature(pivot+1, right);
}

///////////////////////////////////////////////////////////////////////////////
//
// Binary search of sorted list of query results for specific feature
//

int CQueryResult::FindSorted(long lFeature)
{
	int nHigh = GetSize()-1;
	int nLow = 0;
	int nMiddle;

	while (nLow <= nHigh)
	{
	   nMiddle = (nLow+nHigh)/2;

	   if (lFeature > GetAt(nMiddle)->GetActiveFeature()) nLow = nMiddle+1;
	   else if (lFeature < GetAt(nMiddle)->GetActiveFeature()) nHigh = nMiddle-1;
	   else return nMiddle;
   
	}
	return -1;	
}
