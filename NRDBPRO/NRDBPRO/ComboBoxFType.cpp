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
#include "ComboBoxFType.h"
#include "dlgseldictionary.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CComboBoxFType
//
// Static variables
//

CDictSelArray CComboBoxFType::m_aDictSel;
BOOL CComboBoxFType::m_bIncLinks;

/////////////////////////////////////////////////////////////////////////////

CComboBoxFType::CComboBoxFType()
{
}

CComboBoxFType::~CComboBoxFType()
{
}


BEGIN_MESSAGE_MAP(CComboBoxFType, CComboBox)
	//{{AFX_MSG_MAP(CComboBoxFType)
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComboBoxFType message handlers

void CComboBoxFType::Init(long lFTypeSel, BOOL bManyToOne, BOOL bRecurse)
{
   InitDictionary();

   CFeatureType ftype;
   CFTypeAttr attr;
   CArray <long,long> aFTypeAttr;

   ResetContent();
   BeginWaitCursor();

   // Produce a list of feature types with attributes

  if (!m_bIncLinks)
  {
     BOOL bFound = BDFTypeAttr(BDHandle(), &attr, BDGETINIT);
     while (bFound)
     {
        int i = 0; for (i = 0; i < aFTypeAttr.GetSize(); i++)
        {
           if (aFTypeAttr[i] == attr.GetFTypeId()) break;
        }
        if (i == aFTypeAttr.GetSize()) aFTypeAttr.Add(attr.GetFTypeId());
        
        bFound = BDGetNext(BDHandle());
     }
     BDEnd(BDHandle());
  }

  // Fill the combobox with available feature types

   BOOL bFound = BDFeatureType(BDHandle(), &ftype, BDGETINIT);
   while (bFound)
   {
      // Only include subsets

      int i = 0; for (i = 0; i < m_aDictSel.GetSize(); i++)
      {
         // For summary reports ignore sector

         if (ftype.m_lDictionary == m_aDictSel.GetAt(i).m_lId && 
             (m_aDictSel.GetAt(i).m_bSel || bManyToOne))
         {
            // Check that the value has attributes

            if (!m_bIncLinks)
            {
                 int j = 0; for (j = 0; j < aFTypeAttr.GetSize(); j++)
                 {
                    if (aFTypeAttr[j] == ftype.m_lId) break;
                 }
                 if (j == aFTypeAttr.GetSize()) break;
            }
            
            // For summary reports only parent ftypes are shown
            
            if (ftype.m_bManyToOne || ftype.m_lParentFType == 0 || !bManyToOne)
            {
               // Add the feature type

               int index = AddString(ftype.m_sDesc);
               SetItemData(index, ftype.m_lId);

               if (ftype.m_lId == lFTypeSel) SetCurSel(index);
               break;
            };            
         };
      }      

      bFound = BDGetNext(BDHandle());
   }

   // If selected feature type is not found then reset dictionary and try again

   /*if (lFTypeSel != 0 && GetCurSel() == CB_ERR && !bRecurse)
   {
      InitDictionary(TRUE);
      Init(lFTypeSel, bManyToOne, TRUE);
   };*/
   

   BDEnd(BDHandle());

   EndWaitCursor();

   if (GetCurSel() == CB_ERR) SetCurSel(0);
}

/////////////////////////////////////////////////////////////////////////////

void CComboBoxFType::InitDictionary(BOOL bReset)
{
   CDictionary dictionary;
   CDictSel dictsel;

   if (m_aDictSel.GetSize() == 0 || bReset)
   {
      m_aDictSel.RemoveAll();
      BOOL bFound = BDDictionary(BDHandle(), &dictionary, BDGETINIT);
      while (bFound)
      {         
         dictsel.m_lId = dictionary.m_lId;
         dictsel.m_bSel = TRUE;
         dictsel.m_sDesc = dictionary.m_sDesc;
         m_aDictSel.Add(dictsel);

         bFound = BDGetNext(BDHandle());
      }
      BDEnd(BDHandle());
   }
}

/////////////////////////////////////////////////////////////////////////////

void CComboBoxFType::OnRButtonDown(UINT nFlags, CPoint point) 
{
   OnClickDictionary();

   CComboBox::OnRButtonDown(nFlags, point);
};

/////////////////////////////////////////////////////////////////////////////

void CComboBoxFType::OnClickDictionary(BOOL bManyToOne)
{

   ShowDictionary();

      // Save current feature type

   long lFTypeSel = 0;
   int index = GetCurSel();
   if (index != 0) lFTypeSel = GetItemData(index);
   Init(lFTypeSel, bManyToOne);	

   // Inform parent of change

   HWND hWndParent = GetParent()->GetSafeHwnd();
   ::PostMessage(hWndParent, WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(),CBN_SELCHANGE),(LPARAM)GetSafeHwnd());   
}

/////////////////////////////////////////////////////////////////////////////

void CComboBoxFType::ShowDictionary()
{
   
	CDlgSelDictionary dlg(&m_aDictSel, &m_bIncLinks);
   dlg.DoModal();   
}
