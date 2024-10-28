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
#include "DlgAddLayer.h"
#include "maplayer.h"
#include "dlgselectfeatures.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

COLORREF CDlgAddLayer::m_crSel = DEFAULT_COLOUR;

/////////////////////////////////////////////////////////////////////////////
// CDlgAddLayer dialog


CDlgAddLayer::CDlgAddLayer(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAddLayer::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAddLayer)
	//}}AFX_DATA_INIT

   m_pTooltip = NULL;
   m_bMapLines = FALSE;
}

CDlgAddLayer::~CDlgAddLayer()
{
   if (m_pTooltip != NULL) delete m_pTooltip;
};

void CDlgAddLayer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAddLayer)
	DDX_Control(pDX, IDC_LABEL, m_cbLabel);
	DDX_Control(pDX, IDC_MAPOBJ, m_cbMapObj);
	DDX_Control(pDX, IDC_SELECTALL, m_pbSelectAll);
	DDX_Control(pDX, IDC_COLOR, m_pbColour);		
	DDX_Control(pDX, IDC_FTYPE, m_cbFType);
	DDX_Control(pDX, IDC_FEATURE, m_lbFeature);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAddLayer, CDialog)
	//{{AFX_MSG_MAP(CDlgAddLayer)
	ON_CBN_SELCHANGE(IDC_FTYPE, OnSelchangeFtype)	
	ON_BN_CLICKED(IDC_SELECTALL, OnSelectall)
	ON_BN_CLICKED(IDC_SELECT, OnSelect)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_DICTIONARY, OnDictionary)
	ON_LBN_SELCHANGE(IDC_FEATURE, OnSelchangeFeature)
	ON_WM_MOUSEMOVE()	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CDlgAddLayer::Reset()
{
   CString s = AfxGetApp()->GetProfileString(BDGetDataSource(), "DefaultColour");

   int r,g,b;
   if (sscanf(s, "%i,%i,%i", &r, &g, &b))
   {
      m_crSel = RGB(r,g,b);      
   }
}

/////////////////////////////////////////////////////////////////////////////
// CDlgAddLayer message handlers

BOOL CDlgAddLayer::OnInitDialog() 
{
	CFeatureType ftype;

	CDialog::OnInitDialog();

	BeginWaitCursor();

   long lFTypeSel = BDFTypeSel();
   m_bInit = TRUE;

  // Load in list of feature types
   
   m_cbFType.Init(lFTypeSel);  

   OnSelchangeFtype();

   OnSelchangeFeature();   

   // Set previous color

   m_pbColour.SetColour(m_crSel);

   // Enable tooltips

   BDHwndDialog() = m_hWnd;

   m_bInit = FALSE;

   EndWaitCursor();
      
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
//
// Remove feature types that don't have map attributes
//

void CDlgAddLayer::RemoveNonMapFTypes()
{
   BeginWaitCursor();

   // Produce a list of feature types with map attributes

   int i = 0; for (i = 0; i < m_cbFType.GetCount(); i++)
   {    
      if (!IsMapFType(m_cbFType.GetItemData(i)))
      {
         m_cbFType.DeleteString(i);
         i--;
      }
   }
   
   // Set selection

   if (m_cbFType.GetCurSel() == CB_ERR) m_cbFType.SetCurSel(0);

   EndWaitCursor();
}

///////////////////////////////////////////////////////////////////////////////
//
// Determines if a ftype or its 1:1 parent has map attributes
//

BOOL CDlgAddLayer::IsMapFType(long lFType)
{
   CAttrArray aAttr;
   CFeatureType ftype;

   BOOL bMapFType = FALSE;

   // Optimization, store list of ftypes which have already been checked

   int i = 0; for (i = 0; i < m_aFTypeMap.GetSize(); i++)
   {
      if (m_aFTypeMap[i].m_lFType == lFType) return m_aFTypeMap[i].m_bMap;      
   }

   // Determine if ftype inherits polylines or coordinates attr one to one

   if (BDFTypeAttrInit(BDHandle(), lFType, &aAttr))
   {
      int i = 0; for (i = 0; i < aAttr.GetSize(); i++)
      {
         if (aAttr.GetAt(i)->GetDataType() == BDMAPLINES ||
             aAttr.GetAt(i)->GetDataType() == BDCOORD ||
             aAttr.GetAt(i)->GetDataType() == BDIMAGE)
         {
            bMapFType = TRUE;
            break;
         }
      }
      // Check parent

      if (!bMapFType && BDFTypeI(BDHandle(), lFType, &ftype) &&
          lFType != ftype.m_lId)
      {
         return IsMapFType(ftype.m_lId);
      }
   }

   // Add to optimization list

   CFTypeMap ftypemap;
   ftypemap.m_lFType = lFType;
   ftypemap.m_bMap = bMapFType;
   m_aFTypeMap.Add(ftypemap);

   return bMapFType;
}

/////////////////////////////////////////////////////////////////////////////

void CDlgAddLayer::OnSelchangeFtype() 
{   
// Remove feature types that don't have map attributes

   RemoveNonMapFTypes();

 // Update list of features and attributes

	CFeature feature;

   m_lbFeature.ResetContent();

   int index = m_cbFType.GetCurSel();
   if (index != LB_ERR)
   {
      feature.m_lFeatureTypeId = m_cbFType.GetItemData(index);

	// Update list of features

      BOOL bFound = BDFeature(BDHandle(), &feature, BDSELECT2);
      while (bFound)
      {
         index = m_lbFeature.AddString(feature.m_sName);
         m_lbFeature.SetItemData(index, feature.m_lId);

         // Previous selection

         if (m_bInit && BDIsFeatureSel(feature.m_lFeatureTypeId, feature.m_lId)) 
         {
            m_lbFeature.SetSel(index);
         }
         
         bFound = BDGetNext(BDHandle());
      }
      BDEnd(BDHandle());
   };   

   if (m_lbFeature.GetSelCount() == 0)
   {
      OnSelectall();
   }

   OnSelchangeFeature();

   InitAttr();
   if (m_cbMapObj.GetCount() == 0)
   {
      InitParentAttr();
   }
}

/////////////////////////////////////////////////////////////////////////////

void CDlgAddLayer::InitAttr()
{
   CAttrArray aAttr;   
   CAttribute* pAttr;      
   CString sFType;
   CFeatureType ftype;
   CString s;

   RemoveAttr();   

   // Retrieve selected feature and type
   
   int iFType = m_cbFType.GetCurSel();
   
   if (iFType != CB_ERR)
   {      
      long lFType = m_cbFType.GetItemData(iFType);

  // Retrieve attribute values for the feature

     BOOL bFound = BDFTypeAttrInit(BDHandle(), lFType, &aAttr);

  // Add the default 'blank' label

     int index = m_cbLabel.AddString(BDString(IDS_NONE));        
     m_cbLabel.SetItemDataPtr(index, NULL);        

   // Create a list of attributes with spatial data
                  
     int i = 0; for (i = 0; i < aAttr.GetSize(); i++)
     {
        pAttr = aAttr[i];                    

        s = pAttr->GetDesc();
        if (pAttr->GetDataType() == BDMAPLINES) 
        {
           s += " [" + BDString(IDS_POLYLINESTYPE) + "]";           
        };
        if (pAttr->GetDataType() == BDCOORD) 
        {           
           s += " [" + BDString(IDS_COORDTYPE) + "]";                   
        };

        if (pAttr->GetDataType() == BDIMAGE)
        {
           s += " [" + BDString(IDS_IMAGETYPE) + "]";        
        }

        if (pAttr->GetDataType() == BDMAPLINES || pAttr->GetDataType() == BDCOORD || 
            pAttr->GetDataType() == BDIMAGE) 
        {
           int index = m_cbMapObj.AddString(s);        
           m_cbMapObj.SetItemDataPtr(index, new CFTypeAttr(*(CFTypeAttr*)pAttr));    

           if (m_bInit && BDIsAttrSel(pAttr->GetFTypeId(), pAttr->GetAttrId()))
           {
              m_cbMapObj.SetCurSel(index);
           }
        } else
        {
           int index = m_cbLabel.AddString(s);        
           m_cbLabel.SetItemDataPtr(index, new CFTypeAttr(*(CFTypeAttr*)pAttr));    
           if (m_bInit && BDIsAttrSel(pAttr->GetFTypeId(), pAttr->GetAttrId()))
           {
              m_cbLabel.SetCurSel(index);
           }
        }
     }
     if (m_cbMapObj.GetCurSel() == CB_ERR) m_cbMapObj.SetCurSel(0);     
     
     // Add the parent feature name
     
      if (BDFTypeI(BDHandle(), lFType, &ftype))
      {
         int index = m_cbLabel.AddString(ftype.m_sDesc + " ["+BDString(IDS_NAME)+"]");
         CFTypeAttr ftypeattr;
         ftypeattr.m_lAttrId = BDFEATURE;
         ftypeattr.m_lFType = ftype.m_lId;
         m_cbLabel.SetItemDataPtr(index, new CFTypeAttr(ftypeattr));             
         if (m_bInit && BDIsAttrSel(ftypeattr.m_lFType, ftypeattr.m_lAttrId))
         {
            m_cbLabel.SetCurSel(index);
         }

         if (m_cbLabel.GetCurSel() == LB_ERR)
         {
            m_cbLabel.SetCurSel(index);
         }

      }
   };	
   BDEnd(BDHandle());
   
}

/////////////////////////////////////////////////////////////////////////////
//
// Removes attributes and the corresponding allocated memory
//

void CDlgAddLayer::RemoveAttr()
{
   int i = 0; for (i = 0; i < m_cbMapObj.GetCount(); i++)
   {
      CFTypeAttr *pAttr = (CFTypeAttr*)m_cbMapObj.GetItemDataPtr(i);
      if (pAttr != NULL)
      {
         delete pAttr;
      };
   }   
   m_cbMapObj.ResetContent();

	for (i = 0; i < m_cbLabel.GetCount(); i++)
   {
      CFTypeAttr *pAttr = (CFTypeAttr*)m_cbLabel.GetItemDataPtr(i);
      if (pAttr != NULL)
      {
         delete pAttr;
      };
   }   
   m_cbLabel.ResetContent();

}

/////////////////////////////////////////////////////////////////////////////
//
// Add map attributes for 1:1 parent features

void CDlgAddLayer::InitParentAttr()
{
   CFeatureType ftypeP;
   CAttrArray aAttr;   
   CAttribute* pAttr;      
   CString s;

   int iFType = m_cbFType.GetCurSel();
   
   if (m_lbFeature.GetCount() > 0 && iFType != CB_ERR)
   {      
      long lFType = m_cbFType.GetItemData(iFType);

      BDFTypeI(BDHandle(), lFType, &ftypeP);
      if (ftypeP.m_lId != lFType)
      {
     // Retrieve attribute values for the feature

        BOOL bFound = BDFTypeAttrInit(BDHandle(), ftypeP.m_lId, &aAttr);

      // Create a list of attributes with spatial data
                  
        int i = 0; for (i = 0; i < aAttr.GetSize(); i++)
        {
           pAttr = aAttr[i];                    

           // Create string

           s = pAttr->GetDesc();
           if (pAttr->GetDataType() == BDMAPLINES) 
           {
              s += " [" + BDString(IDS_POLYLINESTYPE) + "]";
             
           };
           if (pAttr->GetDataType() == BDCOORD) 
           {           
              s += " [" + BDString(IDS_COORDTYPE) + "]";                     
           };

           if (pAttr->GetDataType() == BDIMAGE) 
           {           
              s += " [" + BDString(IDS_IMAGETYPE) + "]";                     
           };

           // Add string

           if (pAttr->GetDataType() == BDMAPLINES || pAttr->GetDataType() == BDCOORD ||
               pAttr->GetDataType() == BDIMAGE) 
           {
              int index = m_cbMapObj.AddString(s);        
              m_cbMapObj.SetItemDataPtr(index, new CFTypeAttr(*(CFTypeAttr*)pAttr));    

              if (m_bInit && BDIsAttrSel(ftypeP.m_lId, pAttr->GetAttrId()))
              {
                 m_cbMapObj.SetCurSel(index);
              }
           };
        };
      };
   }
   if (m_cbMapObj.GetCurSel() == CB_ERR) m_cbMapObj.SetCurSel(0);     
}

/////////////////////////////////////////////////////////////////////////////

void CDlgAddLayer::OnOK() 
{
   CString sDateTime, sDate, sTime;   
   CFeature feature;
   CFeatureType ftype;

	int iFType = m_cbFType.GetCurSel();   
   int iMapObj = m_cbMapObj.GetCurSel();
   int iLabel = m_cbLabel.GetCurSel();
   int nMapLines = 0, nCoord = 0;
   
   // Retrieve data

   if (iFType != CB_ERR)
   {      
      if (m_lbFeature.GetSelCount() > 0 ||
          AfxMessageBox(IDS_NOFEATURECONT, MB_YESNO) == IDYES)          
      {
         if (iMapObj == CB_ERR)
         {
            AfxMessageBox(BDString(IDS_SELMAPOBJ));
            return;
         }

		 BeginWaitCursor();

         m_lFType = m_cbFType.GetItemData(iFType);            

         BDFTypeSel(m_lFType);
         BDFeatureSel().RemoveAll();

         // Retrieve features

         BDFTypeI(BDHandle(), m_lFType, &ftype);
         int i = 0; for (i = 0; i < m_lbFeature.GetCount(); i++)
         {
            if (m_lbFeature.GetSel(i))
            {               
               m_alFeatures.Add(m_lbFeature.GetItemData(i));         
               
               feature.m_lFeatureTypeId = ftype.m_lId;
               feature.m_lId = m_lbFeature.GetItemData(i);
               BDFeatureSel().Add(feature);
            };
         }      

         // Add map object attribute

         BDAttrSel().RemoveAll();

         CFTypeAttr* pAttr = (CFTypeAttr*)m_cbMapObj.GetItemDataPtr(iMapObj);     
         
         m_aAttr.Add(*pAttr);
         if (pAttr->GetDataType() == BDMAPLINES) m_bMapLines = TRUE;
                  
         BDAttrSel().Add(*pAttr); 

         // Retrieve attributes
         
         if (iLabel != CB_ERR)
         {
            CFTypeAttr* pAttr = (CFTypeAttr*)m_cbLabel.GetItemDataPtr(iLabel);
            if (pAttr != NULL)
            {
               m_aAttr.Add(*pAttr);    
               BDAttrSel().Add(*pAttr);
            };            
         }      

         // Retrieve the color

         m_color = m_pbColour.GetColour();
         m_crSel = m_color;

		 EndWaitCursor();
      
         CDialog::OnOK();
      }
      
   } else
   {
      AfxMessageBox(BDString(IDS_NOFTYPESEL));
   }
}

///////////////////////////////////////////////////////////////////////////////

void CDlgAddLayer::OnSelectall() 
{
   m_lbFeature.SelectAll();
   m_pbSelectAll.Update(m_lbFeature);
}

///////////////////////////////////////////////////////////////////////////////

void CDlgAddLayer::OnSelchangeFeature() 
{
   m_pbSelectAll.Update(m_lbFeature);
}

///////////////////////////////////////////////////////////////////////////////

void CDlgAddLayer::OnSelect() 
{   
   long lFType = 0; 

   // Determine the parent feature type of that selected

   int index = m_cbFType.GetCurSel();
   if (index != LB_ERR)
   {      
      lFType = m_cbFType.GetItemData(index);
     
   // Create a dialog allowing a sub-set of the features to be selected
   
      CDlgSelectFeatures dlg(lFType);      
      if (dlg.DoModal() == IDOK)
      {
         dlg.Initialise(m_lbFeature);         
      }
   };	
}

///////////////////////////////////////////////////////////////////////////////

void CDlgAddLayer::OnClose() 
{	
	CDialog::OnClose();
}

///////////////////////////////////////////////////////////////////////////////

BOOL CDlgAddLayer::DestroyWindow() 
{
   // Tidy up

   RemoveAttr();
   	
	return CDialog::DestroyWindow();   
}

///////////////////////////////////////////////////////////////////////////////

void CDlgAddLayer::OnDictionary() 
{
   m_cbFType.OnClickDictionary();   
}

///////////////////////////////////////////////////////////////////////////////

int CDlgAddLayer::OnToolHitTest( CPoint point, TOOLINFO* pTI ) const
{
   CWnd::OnToolHitTest(point, pTI);
   CString s = BDString(IDS_SELMAPOBJ);
   pTI->lpszText = (LPSTR)malloc(s.GetLength()+1);
   strcpy(pTI->lpszText, s);

   return 1;
}

///////////////////////////////////////////////////////////////////////////////

void CDlgAddLayer::OnMouseMove(UINT nFlags, CPoint point) 
{
   CDialog::OnMouseMove(nFlags, point);     	
}

///////////////////////////////////////////////////////////////////////////////

void CDlgAddLayer::PostNcDestroy() 
{
   BDHwndDialog() = NULL;
   BDToolTip() = NULL;
	
	CDialog::PostNcDestroy();
}

