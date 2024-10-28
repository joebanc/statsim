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
#include "DlgFeatures.h"
#include "dlgeditfeature.h"
#include "dlgeditattributes.h"
#include "dlgselectfeatures.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgFeatures dialog


CDlgFeatures::CDlgFeatures(long lFType, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFeatures::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgFeatures)
	//}}AFX_DATA_INIT

   m_lFType = lFType;
}


void CDlgFeatures::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFeatures)
	DDX_Control(pDX, IDC_FTYPE, m_cbFTypes);	
	//}}AFX_DATA_MAP

   DDX_Control(pDX, IDC_FEATURESM, m_lbFeatures);
}


BEGIN_MESSAGE_MAP(CDlgFeatures, CDialog)
	//{{AFX_MSG_MAP(CDlgFeatures)
	ON_CBN_SELCHANGE(IDC_FTYPE, OnSelchangeFtype)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_EDITDATA, OnEditData)
	ON_BN_CLICKED(IDC_SELECT, OnSelect)
	ON_LBN_SELCHANGE(IDC_FEATURES, OnSelchangeFeatures)
	ON_BN_CLICKED(IDC_DICTIONARY, OnDictionary)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP

   ON_LBN_SELCHANGE(IDC_FEATURESM, OnSelchangeFeatures)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgFeatures message handlers

BOOL CDlgFeatures::OnInitDialog() 
{
	CDialog::OnInitDialog();

   if (BDGetTransLevel(BDHandle()) > 0) BDCommit(BDHandle());
   ASSERT(BDGetTransLevel(BDHandle()) == 0);
	//BDBeginTrans(BDHandle());   
   //GetDlgItem(IDCANCEL)->EnableWindow(FALSE);

   long lFTypeSel = BDFTypeSel();
   if (m_lFType != 0) lFTypeSel = m_lFType;
	
	// Load in list of feature types

   m_cbFTypes.Init(lFTypeSel);   
   OnSelchangeFtype();

   GetDlgItem(IDC_FEATURES)->ShowWindow(SW_HIDE);
   GetDlgItem(IDC_FEATURESM)->ShowWindow(SW_SHOW);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////

void CDlgFeatures::OnSelchangeFtype() 
{
   InitFeatures(0);
}

/////////////////////////////////////////////////////////////////////////////

void CDlgFeatures::InitFeatures(long lIdSel)
{
	CFeature feature;

   m_lbFeatures.ResetContent();
  
   int index = m_cbFTypes.GetCurSel();
   if (index != LB_ERR)
   {
      feature.m_lFeatureTypeId = m_cbFTypes.GetItemData(index);

	// Enable controls accoring to the selected feature type

	  EnableControls(feature.m_lFeatureTypeId);

	// Update list of features

      BOOL bFound = BDFeature(BDHandle(), &feature, BDSELECT2);
      while (bFound)
      {
         index = m_lbFeatures.AddString(feature.m_sName);
         m_lbFeatures.SetItemData(index, feature.m_lId);

         if (feature.m_lId == lIdSel)
         { 
            BDFTypeSel(lIdSel);

			m_lbFeatures.SetSel(index, TRUE);
         };

         bFound = BDGetNext(BDHandle());
      }
      BDEnd(BDHandle());
   };

   if (m_lbFeatures.GetCurSel() == LB_ERR) m_lbFeatures.SetCurSel(0);
   OnSelchangeFeatures();	
}

/////////////////////////////////////////////////////////////////////////////

void CDlgFeatures::OnAdd() 
{
   int index = m_cbFTypes.GetCurSel();
   if (index != LB_ERR)
   {
      long lFTypeId = m_cbFTypes.GetItemData(index);
      CDlgEditFeature dlg(lFTypeId);
      if (dlg.DoModal() == IDOK)
      {
         InitFeatures(dlg.GetId());         
      }	      
   };


  //if (BDGetTransLevel(BDHandle()) == 0)  BDBeginTrans(BDHandle());

}

/////////////////////////////////////////////////////////////////////////////

void CDlgFeatures::OnEdit() 
{
   int index = m_cbFTypes.GetCurSel();
   if (index != LB_ERR)
   {
      long lFTypeId = m_cbFTypes.GetItemData(index);

      index = m_lbFeatures.GetCurSel();
      if (index != LB_ERR)
      {
         long lId = m_lbFeatures.GetItemData(index);
         CDlgEditFeature dlg(lFTypeId, lId);
         if (dlg.DoModal() == IDOK)
         {
            InitFeatures(dlg.GetId());
         }	
      };
   };	

   //if (BDGetTransLevel(BDHandle()) == 0)  BDBeginTrans(BDHandle());

}

/////////////////////////////////////////////////////////////////////////////

void CDlgFeatures::OnDelete() 
{
   CFeature feature, featureC;
   CFeatureType ftype;
   CLongArray alFType121, alFType12M;      
   CAttrArray aAttr;

   int index1 = m_cbFTypes.GetCurSel();
   if (index1 != CB_ERR)
   {      


	  
	  if (m_lbFeatures.GetCount() > 0 && 
		  AfxMessageBox(BDString(IDS_CONFIRMDELETE) + " " + feature.m_sName , MB_YESNO) == IDYES)
	  {
		  // Check to see if the feature has children
                     
		  feature.m_lFeatureTypeId = m_cbFTypes.GetItemData(index1);
		  GetChildren(feature.m_lFeatureTypeId, alFType121, alFType12M);

		  for (int index2 = 0; index2 < m_lbFeatures.GetCount(); index2++)
		  {
			 if (m_lbFeatures.GetSel(index2))
         
      {                  
         feature.m_lId = m_lbFeatures.GetItemData(index2);         
         feature.m_lFeatureTypeId = m_cbFTypes.GetItemData(index1);
 		   GetChildren(feature.m_lFeatureTypeId, alFType121, alFType12M);

         m_lbFeatures.GetText(index2, feature.m_sName);         
         feature.m_sName.TrimRight(); 
        
       
           BeginWaitCursor();

      // Determine if child ftypes have child features

		     CString s;		   
           int i = 0; for (i = 0; i < alFType12M.GetSize(); i++)
           {
              featureC.m_lFeatureTypeId = alFType12M[i];
              featureC.m_lParentFeature = feature.m_lId;
			     BOOL bOK = BDFeature(BDHandle(), &featureC, BDSELECT4);
              while (bOK )
              {                 
                 featureC.m_sName.TrimRight();
				 
                 if (!s.IsEmpty()) s += ", ";
				     s += featureC.m_sName;				    
                 bOK = BDGetNext(BDHandle());

                 if (s.GetLength() > 640) 
                 {
                    s+="...";
                    goto end;
                 }
              }
           }
end:

           BDEnd(BDHandle());
		     EndWaitCursor();                 

		   if (!s.IsEmpty())
		   {
			 AfxMessageBox(BDString(IDS_CANNOTDEL) + "\r\n\r\n" + s);
             return;
		   };
       
           // Determine if child features of 1:1 contain data

           for (i = 0; i < alFType121.GetSize(); i++)
           {
              aAttr.m_lFType = alFType121[i];
              aAttr.m_lFeature = feature.m_lId;

              if (BDAttribute(BDHandle(), &aAttr, BDSELECT3))
              {
                 BDEnd(BDHandle());
                 ftype.m_lId = alFType121[i];
                 BDFeatureType(BDHandle(), &ftype, BDSELECT);
                 BDEnd(BDHandle());

                 EndWaitCursor();
                 AfxMessageBox(BDString(IDS_CANNOTDELDATA) + ": " + ftype.m_sDesc + "."); 
                 return;
              }
              BDEnd(BDHandle());
           }      

           EndWaitCursor();

                       
      // Delete the feature

           BDFeature(BDHandle(), &feature, BDDELETE);           	           
        };
      

      };

      };
     
      InitFeatures(0);

   };	
}

///////////////////////////////////////////////////////////////////////////////

void CDlgFeatures::GetChildren(long lFType, CLongArray& alFType121, CLongArray& alFType12M)
{
   BeginWaitCursor();

   CFeatureType ftype;
   int n121 = alFType121.GetSize();

   // Produce separate lists of child ftypes with one to one and one to many 
   // relationships

   BOOL bFound = BDFeatureType(BDHandle(), &ftype, BDGETINIT);
   while (bFound)
   {
      if (ftype.m_lParentFType == lFType)
      {
         if (ftype.m_bManyToOne)
         {
            alFType12M.Add(ftype.m_lId);
         }
         else
         {
            alFType121.Add(ftype.m_lId);                      
         };
      }
      bFound = BDGetNext(BDHandle());
   }
   BDEnd(BDHandle());   

   // For 1:1 get subclasses

   for (int i = n121; i < alFType121.GetSize(); i++)
   {
      GetChildren(alFType121[i], alFType121, alFType12M);
   };

   EndWaitCursor();
}

///////////////////////////////////////////////////////////////////////////////
//
// If features are inherited from parent feature type (1 to 1) then they 
// cannot be edited
//

void CDlgFeatures::EnableControls(long lIdSel)
{
   CFeatureType ftype;

   ftype.m_lId = lIdSel;

   BDFeatureType(BDHandle(), &ftype, BDSELECT);
   BDEnd(BDHandle());

   BOOL bEnable = ftype.m_bManyToOne || ftype.m_lParentFType == 0 ;

   GetDlgItem(IDC_ADD)->EnableWindow(bEnable);
   GetDlgItem(IDC_EDIT)->EnableWindow(bEnable);
   GetDlgItem(IDC_DELETE)->EnableWindow(bEnable);
}

///////////////////////////////////////////////////////////////////////////////

void CDlgFeatures::OnEditData() 
{
   CString sFeature;


   int iFType = m_cbFTypes.GetCurSel();
  
   if (iFType != CB_ERR)
   {
      CLongArray alFeatures;

      long lFType = m_cbFTypes.GetItemData(iFType);
      
      int i = 0; for (i = 0; i < m_lbFeatures.GetCount(); i++)
      {
         if (m_lbFeatures.GetSel(i)) alFeatures.Add(m_lbFeatures.GetItemData(i));
      }
      
      if (alFeatures.GetSize() > 0)
      {
         CDlgEditAttributes dlg(alFeatures, lFType);
         int nRet = dlg.DoModal();		 
      };
      
      //if (BDGetTransLevel(BDHandle()) == 0)  BDBeginTrans(BDHandle());
   }	

}

///////////////////////////////////////////////////////////////////////////////

void CDlgFeatures::OnOK() 
{
   int iFType = m_cbFTypes.GetCurSel();
   if (iFType != CB_ERR)
   {
      long lFType = m_cbFTypes.GetItemData(iFType);
	   BDFTypeSel(lFType);
   };

	CDialog::OnOK();
}

///////////////////////////////////////////////////////////////////////////////   

void CDlgFeatures::OnCancel()
{

}

///////////////////////////////////////////////////////////////////////////////

void CDlgFeatures::OnSelect() 
{
   long lFType = 0; 

   // Determine the parent feature type of that selected

   int index = m_cbFTypes.GetCurSel();
   if (index != LB_ERR)
   {      
      lFType = m_cbFTypes.GetItemData(index);
     
   // Create a dialog allowing a sub-set of the features to be selected
   
      CDlgSelectFeatures dlg(lFType, FALSE);      

      if (dlg.DoModal() == IDOK)
      {
         dlg.Initialise(m_lbFeatures);         
         OnSelchangeFeatures();
      }
   };	
}

///////////////////////////////////////////////////////////////////////////////

void CDlgFeatures::OnSelchangeFeatures() 
{
   CFeature feature;
   CFeatureType ftype;

  // Display name of parent feature

   GetDlgItem(IDC_PARENTFEATURE)->SetWindowText("");

   int index1 = m_cbFTypes.GetCurSel();
   int index2 = m_lbFeatures.GetCurSel();
   
   if (index1 != CB_ERR && index2 != LB_ERR)
   {  
      feature.m_lFeatureTypeId = m_cbFTypes.GetItemData(index1);
      feature.m_lId = m_lbFeatures.GetItemData(index2);      

      // Determine the parent feature type id
      
      if (BDFeature(BDHandle(), &feature, BDSELECT) && feature.m_lParentFeature != 0)
      {
         // Determine the parent feature type
         
         if (BDFTypeParentI(BDHandle(), feature.m_lFeatureTypeId, &ftype))
         {
            feature.m_lId = feature.m_lParentFeature;
            feature.m_lFeatureTypeId = ftype.m_lId;

            if (BDFeature(BDHandle(), &feature, BDSELECT))
            {
               GetDlgItem(IDC_PARENTFEATURE)->SetWindowText(feature.m_sName);         
            }
         }
      };
      BDEnd(BDHandle());
   }

	
}

///////////////////////////////////////////////////////////////////////////////

void CDlgFeatures::OnDictionary() 
{
   m_cbFTypes.OnClickDictionary();	
}

///////////////////////////////////////////////////////////////////////////////

void CDlgFeatures::OnClose() 
{
   OnCancel();	
}
