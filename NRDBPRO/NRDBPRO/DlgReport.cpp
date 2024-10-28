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
#include "DlgReport.h"
#include "dlgselectfeatures.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgReport dialog


CDlgReport::CDlgReport(int nType, int nIDD, CWnd* pParent /*=NULL*/)
	: CDialog(nIDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgReport)
	//}}AFX_DATA_INIT

	m_nType = nType;
   m_pTooltip = NULL;
}

/////////////////////////////////////////////////////////////////////////////

CDlgReport::~CDlgReport()
{
   if (m_pTooltip != NULL) delete m_pTooltip;
};


void CDlgReport::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgReport)
	DDX_Control(pDX, IDC_SELECTALLATTR, m_pbSelectAllAttr);
	DDX_Control(pDX, IDC_SELECTALL, m_pbSelectAll);
	DDX_Control(pDX, IDC_FTYPE, m_cbFTypes);
	DDX_Control(pDX, IDC_FEATURES, m_lbFeatures);	
	//}}AFX_DATA_MAP

   if (m_nType == PieChart) DDX_Control(pDX, IDC_ATTRS, m_lbAttr);
   else DDX_Control(pDX, IDC_ATTR, m_lbAttr);
}


BEGIN_MESSAGE_MAP(CDlgReport, CDialog)
	//{{AFX_MSG_MAP(CDlgReport)
	ON_CBN_SELCHANGE(IDC_FTYPE, OnSelchangeFtype)
	ON_BN_CLICKED(IDC_QUERY, OnQuery)
	ON_BN_CLICKED(IDC_SELECTALL, OnSelectall)
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN, OnDeltaposUpdown)
	ON_BN_CLICKED(IDC_SELECTALLATTR, OnSelectallAttr)
	ON_BN_CLICKED(IDC_DICTIONARY, OnDictionary)
	ON_LBN_SELCHANGE(IDC_FEATURES, OnSelchangeFeatures)
	ON_LBN_SELCHANGE(IDC_ATTR, OnSelchangeAttr)
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_SELECT, OnSelect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgReport message handlers

BOOL CDlgReport::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
   m_cbFTypes.Init(BDFTypeSel());

	SelChangeFType();

   // Set title

   if (m_nType == Report) SetWindowText(BDString(IDS_STANDARDREPORT));
   else if (m_nType == Histogram) SetWindowText(BDString(IDS_HISTOGRAMGRAPH));
   else if (m_nType == Timeseries) SetWindowText(BDString(IDS_TIMESERIESGRAPH));
   else if (m_nType == PieChart) SetWindowText(BDString(IDS_PIECHART));

   // Retrieve previous selections

   RetrieveSel();

   // Show windows

   if (m_nType == PieChart) 
   {
      GetDlgItem(IDC_ATTRS)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_ATTR)->ShowWindow(SW_HIDE);
   }

   // Enable tooltips

   BDHwndDialog() = m_hWnd;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

///////////////////////////////////////////////////////////////////////////////

void CDlgReport::OnSelchangeFtype()
{
   SelChangeFType();
}

void CDlgReport::SelChangeFType()
{         
   CFeature feature;  
   CFeatureType ftype;
  
   m_lbAttr.ResetContent();
   
   int index =m_cbFTypes.GetCurSel();
   if (index != CB_ERR)
   {
      long lFType = m_cbFTypes.GetItemData(index);      

      InitFeatures(lFType);

      // Get the name of the feature type (or parent where one to one)

      BDFTypeI(BDHandle(), lFType, &ftype);
      
	   // Initialise list of attributes corresponding to selected ftype

      index = m_lbAttr.AddString(ftype.m_sDesc + " [" + BDString(IDS_NAME) + "]");
      m_lbAttr.SetItemData(index, BDFEATURE);

      // Add the date

      index = m_lbAttr.AddString(BDString(IDS_DATE));
      m_lbAttr.SetItemData(index, BDDATE);
      if (m_nType == Timeseries) m_lbAttr.SetSel(index);

      // Add remaining attributes

      feature.m_lFeatureTypeId = lFType;
	   BDFTypeAttrInit(BDHandle(), feature.m_lFeatureTypeId, &m_aAttr);
      BDEnd(BDHandle());

	   int i = 0; for (i = 0; i < m_aAttr.GetSize(); i++)
	   {
		   long lDataType = m_aAttr[i]->GetDataType();         
         if (lDataType != BDMAPLINES)
         {
		     index = m_lbAttr.AddString(m_aAttr[i]->GetDesc());
			  m_lbAttr.SetItemDataPtr(index, m_aAttr[i]);
         };
	   }
   }
   OnSelchangeFeatures();	
   OnSelchangeAttr();
}

///////////////////////////////////////////////////////////////////////////////

void CDlgReport::InitFeatures(long lFType)
{
   CFeature feature;

   m_lbFeatures.ResetContent();

   // Initialise list of features corresponding to selected ftype

	   feature.m_lFeatureTypeId = lFType;
	   m_aAttr.m_lFType = feature.m_lFeatureTypeId;	         

      BOOL bFound = BDFeature(BDHandle(), &feature, BDSELECT2);
      while (bFound)
	   {
		   int index = m_lbFeatures.AddString(feature.m_sName);
		   m_lbFeatures.SetItemData(index, feature.m_lId);
		   bFound = BDGetNext(BDHandle());
	   }
	   BDEnd(BDHandle());
}

///////////////////////////////////////////////////////////////////////////////

void CDlgReport::RetrieveSel()
{
   long lFType;
   if (m_cbFTypes.GetItemDataX(lFType) != CB_ERR && lFType == BDFTypeSel())
   {
      int i = 0; for (i = 0; i < m_lbFeatures.GetCount(); i++)
      {
         if (BDIsFeatureSel(lFType,m_lbFeatures.GetItemData(i)))
         {
            m_lbFeatures.SetSel(i);
         }
      }
      for (i = 0; i < m_lbAttr.GetCount(); i++)
      {
         long l = m_lbAttr.GetItemData(i);
         if (l > 0) l = ((CAttribute*)l)->m_lAttrId;

         if (BDIsAttrSel(lFType, l))
         {
            if (m_nType == PieChart)
            {
               m_lbAttr.SetCurSel(i);
            } else
            {
               m_lbAttr.SetSel(i);
            };
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void CDlgReport::OnOK() 
{   	
   CString s;
   BOOL bDate = FALSE, bNumber = FALSE;
   CFeature feature;   

	if (m_lbFeatures.GetSelCount() > 0)
	{
	   if (m_lbAttr.GetSelCount() > 0 ||
          (m_nType == PieChart && m_lbAttr.GetCurSel() != LB_ERR))
	   {
			// Retrieve feature type

			m_lFType = m_cbFTypes.GetItemData(m_cbFTypes.GetCurSel());

		  // Retrieve selected features
			
		   int i = 0; for (i = 0; i < m_lbFeatures.GetCount(); i++)
		   {
			  if (m_lbFeatures.GetSel(i))
			  {
				  m_alFeatures.Add(m_lbFeatures.GetItemData(i));
				  m_lbFeatures.GetText(i, s);
				  s.TrimRight();
				  m_asFeatures.Add(s);
			  }
		   }

		   // Retrieve the selected attributes

         RetrieveAttrs();
		   
		   // Check that attributes of the correct types have been selected
	   		 
		   if (m_nType == Timeseries || m_nType == Histogram || m_nType == PieChart)
		   {

			   for (i = 0; i < m_alAttr.GetSize(); i++)
			   {
				   if (m_alAttr[i] == BDDATE) bDate = TRUE;
				   int j = 0; for (j = 0; j < m_aAttr.GetSize(); j++)
				   {
					   if (m_aAttr[j]->GetAttrId() == (long)m_alAttr[i])
					   {						   
						   if (m_aAttr[j]->GetDataType() == BDNUMBER) bNumber = TRUE;
					   }
				   }
			   }
			   if (m_nType == Timeseries && !bDate) 
			   {				   
               ASSERT(FALSE);
				   return;
			   }
			   if (bNumber == FALSE)
			   {
               ASSERT(FALSE);				  
				   return;
			   } 
		   };

		   m_cbFTypes.SaveFTypes();

         // Store default features

         BDFeatureSel().RemoveAll();
         for (i = 0; i < m_alFeatures.GetSize(); i++)
         {
            feature.m_lFeatureTypeId = m_lFType;
            feature.m_lId = m_alFeatures[i];
            BDFeatureSel().Add(feature);
         }         

         // Store default attributes

         BDAttrSel().RemoveAll();
         for (i = 0; i < m_alAttr.GetSize(); i++)
         {
            CFTypeAttr ftypeattr;
            ftypeattr.m_lFType = m_lFType;
            ftypeattr.m_lAttrId = m_alAttr[i];
            BDAttrSel().Add(ftypeattr);
         }         
		   DetermineTitle();

		   CDialog::OnOK();
	   } else
	   {
		   AfxMessageBox(BDString(IDS_NOATTRSEL));
	   }
	} 
	else
	{
		AfxMessageBox(BDString(IDS_NONAMESEL));
	}

}

///////////////////////////////////////////////////////////////////////////////

void CDlgReport::RetrieveAttrs()
{
   int i = 0; for (i = 0; i < m_lbAttr.GetCount(); i++)
	{
      if (m_lbAttr.GetSel(i) ||
          (m_nType == PieChart && m_lbAttr.GetCurSel() == i))
		{
         if (m_lbAttr.GetItemData(i) != BDDATE &&
             m_lbAttr.GetItemData(i) != BDFEATURE)
         {
            CFTypeAttr* pAttr = (CFTypeAttr*)m_lbAttr.GetItemDataPtr(i);
				m_alAttr.Add(pAttr->GetAttrId());
         } else
         {
            m_alAttr.Add(m_lbAttr.GetItemData(i));
         }
		}
	}

}

///////////////////////////////////////////////////////////////////////////////

void CDlgReport::DetermineTitle()
{
   CString sFeature, sAttr;
   int iAttr = 0;
   CString sTitle;

   // Get feature type

   m_cbFTypes.GetWindowText(sTitle);

   // Get features, if only one

   if (m_lbFeatures.GetSelCount() == 1)
   {
      int i = 0; for (i = 0; i < m_lbFeatures.GetCount(); i++)
      {
         if (m_lbFeatures.GetSel(i))
         {
            m_lbFeatures.GetText(i, sFeature);
			   sFeature.TrimRight();
            sTitle += " " + sFeature;
         }
      }
   }

   // Get attributes if only one

   int i = 0; for (i = 0; i < m_lbAttr.GetCount(); i++)
   {
      if (m_lbAttr.GetSel(i) && m_lbAttr.GetItemData(i) != BDDATE &&
          m_lbAttr.GetItemData(i) != BDFEATURE)
      {
         iAttr++;
         m_lbAttr.GetText(i, sAttr);            
      }
   }   

   if (iAttr == 1)
   {
      sTitle += " " + sAttr;
   }

   // Filter out anything in square brackets

   BOOL bBracket = FALSE;
   for (i = 0; i < sTitle.GetLength(); i++)
   {
      if (sTitle[i] == '[') bBracket = TRUE;
      else if (sTitle[i] == ']') bBracket = FALSE;
      else if (!bBracket) m_sTitle += sTitle[i];
   }



}

///////////////////////////////////////////////////////////////////////////////

void CDlgReport::OnQuery() 
{
	EndDialog(IDC_QUERY);
	
}

///////////////////////////////////////////////////////////////////////////////

void CDlgReport::OnSelectall() 
{
   m_lbFeatures.SelectAll();	
   m_pbSelectAll.Update(m_lbFeatures);
}

///////////////////////////////////////////////////////////////////////////////

void CDlgReport::OnDeltaposUpdown(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

   m_lbAttr.UpDown(pNMUpDown->iDelta);	
		
	*pResult = 0;
}

///////////////////////////////////////////////////////////////////////////////

void CDlgReport::OnSelectallAttr() 
{
   m_lbAttr.SelectAll();	
   m_pbSelectAllAttr.Update(m_lbAttr);
}

///////////////////////////////////////////////////////////////////////////////

void CDlgReport::OnDictionary() 
{
	m_cbFTypes.OnClickDictionary();
	
}

///////////////////////////////////////////////////////////////////////////////

void CDlgReport::OnSelchangeFeatures() 
{
   m_pbSelectAll.Update(m_lbFeatures);
}

///////////////////////////////////////////////////////////////////////////////

void CDlgReport::OnSelchangeAttr() 
{
   m_pbSelectAllAttr.Update(m_lbAttr);	
}

///////////////////////////////////////////////////////////////////////////////

void CDlgReport::OnMouseMove(UINT nFlags, CPoint point) 
{
	CDialog::OnMouseMove(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////

void CDlgReport::PostNcDestroy() 
{
   BDHwndDialog() = NULL;
   BDToolTip() = NULL;
	
	CDialog::PostNcDestroy();
}

///////////////////////////////////////////////////////////////////////////////

void CDlgReport::OnSelect() 
{
   long lFType = 0; 

   // Determine the parent feature type of that selected

   int index = m_cbFTypes.GetCurSel();
   if (index != LB_ERR)
   {      
      lFType = m_cbFTypes.GetItemData(index);
     
   // Create a dialog allowing a sub-set of the features to be selected
   
      CDlgSelectFeatures dlg(lFType);      
      if (dlg.DoModal() == IDOK)
      {
         dlg.Initialise(m_lbFeatures);         
      }
   };		
}
