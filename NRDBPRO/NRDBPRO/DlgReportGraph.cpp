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
#include "DlgReportGraph.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgReportGraph dialog


CDlgReportGraph::CDlgReportGraph(int nType, CWnd* pParent /*=NULL*/)
	: CDlgReport(nType, IDD)
{
	//{{AFX_DATA_INIT(CDlgReportGraph)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

   m_nType = nType;
}


void CDlgReportGraph::DoDataExchange(CDataExchange* pDX)
{
	CDlgReport::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgReportGraph)
	DDX_Control(pDX, IDC_LABEL, m_lbLabel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgReportGraph, CDlgReport)
	//{{AFX_MSG_MAP(CDlgReportGraph)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL CDlgReportGraph::OnInitDialog() 
{
	CDlgReport::OnInitDialog();
	
   if (m_nType & Timeseries) m_lbLabel.EnableWindow(FALSE);

   if (m_nType & Report) SetWindowText(BDString(IDS_REPORT));

   RetrieveSel();
	
	return TRUE; 
}

/////////////////////////////////////////////////////////////////////////////
// CDlgReportGraph message handlers

void CDlgReportGraph::SelChangeFType() 
{
   CFeatureType ftype;
   CFeature feature;
  
   m_lbAttr.ResetContent();
   m_lbLabel.ResetContent();
   
   int index =m_cbFTypes.GetCurSel();
   if (index != CB_ERR)
   {
      long lFType = m_cbFTypes.GetItemData(index);      

      InitFeatures(lFType);

      // Get the name of the feature type (or parent where one to one)

      BDFTypeI(BDHandle(), lFType, &ftype);

	   // Initialise list of attributes corresponding to selected ftype

      index = m_lbLabel.AddString(ftype.m_sDesc + " [" + BDString(IDS_NAME) + "]");
      m_lbLabel.SetItemData(index, BDFEATURE);

      // Add the date

      index = m_lbLabel.AddString(BDString(IDS_DATE));
      m_lbLabel.SetItemData(index, BDDATE);
      if (m_nType & Timeseries) m_lbLabel.SetSel(index);

      // Add remaining attributes

      feature.m_lFeatureTypeId = lFType;
	   BDFTypeAttrInit(BDHandle(), feature.m_lFeatureTypeId, &m_aAttr);
      BDEnd(BDHandle());

	   int i = 0; for (i = 0; i < m_aAttr.GetSize(); i++)
	   {
		   long lDataType = m_aAttr[i]->GetDataType();         
         if (lDataType != BDMAPLINES)
         {
            if (m_aAttr[i]->GetDataType() == BDNUMBER)
            {
		         index = m_lbAttr.AddString(m_aAttr[i]->GetDesc());
			      m_lbAttr.SetItemDataPtr(index, m_aAttr[i]);
            } else
            {
               index = m_lbLabel.AddString(m_aAttr[i]->GetDesc());
			      m_lbLabel.SetItemDataPtr(index, m_aAttr[i]);
            }
         };
	   }
   }
   OnSelchangeFeatures();	
   OnSelchangeAttr();	
}

///////////////////////////////////////////////////////////////////////////////

void CDlgReportGraph::RetrieveSel()
{
   long lFType;
   if (m_cbFTypes.GetItemDataX(lFType) != CB_ERR && lFType == BDFTypeSel())
   {
      int i = 0; for (i = 0; i < m_lbAttr.GetCount(); i++)
      {
         CFTypeAttr* pAttr = (CFTypeAttr*)m_lbAttr.GetItemDataPtr(i);
         if (BDIsAttrSel(pAttr->GetFTypeId(), pAttr->GetAttrId()))
         {
            m_lbAttr.SetSel(i);
         }
      }
      for (i = 0; i < m_lbLabel.GetCount(); i++)
      {
         if (m_lbLabel.GetItemData(i) != BDDATE &&
             m_lbLabel.GetItemData(i) != BDFEATURE)
         {  
            CFTypeAttr* pAttr = (CFTypeAttr*)m_lbLabel.GetItemDataPtr(i);
            if (BDIsAttrSel(pAttr->GetFTypeId(), pAttr->GetAttrId()))
            {
               m_lbLabel.SetSel(i);
            }
         } else
         {
            if (BDIsAttrSel(lFType, m_lbLabel.GetItemData(i)))
            {
               m_lbLabel.SetSel(i);
            }
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void CDlgReportGraph::RetrieveAttrs()
{
   // Retrieve numeric attributes

   int i = 0; for (i = 0; i < m_lbAttr.GetCount(); i++)
	{
		if (m_lbAttr.GetSel(i))
		{                    
         CFTypeAttr* pAttr = (CFTypeAttr*)m_lbAttr.GetItemDataPtr(i);
         ASSERT (pAttr->GetDataType() == BDNUMBER);
			m_alAttr.Add(pAttr->GetAttrId());
  		}
	}

   for (i = 0; i < m_lbLabel.GetCount(); i++)
	{
		if (m_lbLabel.GetSel(i))
		{
         if (m_lbLabel.GetItemData(i) != BDDATE &&
             m_lbLabel.GetItemData(i) != BDFEATURE)
         {
            CFTypeAttr* pAttr = (CFTypeAttr*)m_lbLabel.GetItemDataPtr(i);
				m_alAttr.Add(pAttr->GetAttrId());
         } else
         {
            m_alAttr.Add(m_lbLabel.GetItemData(i));
         }
		}
	}

}

