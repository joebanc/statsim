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
#include "DlgFTypeAttr.h"
#include "dlgseldictionary.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CAttrTypes CAttrTypes::m_aAttrType[] = 
{{BDTEXT, IDS_TEXT},
{BDNUMBER, IDS_NUMBER}, 
{BDCOORD, IDS_COORDINATES}, 
{BDMAPLINES, IDS_POLYLINES}, 
{BDLINK, IDS_LINK}, 
{BDBOOLEAN, IDS_BOOLEAN},
{BDHOTLINK, IDS_HOTLINK},
{BDLONGTEXT, IDS_LONGTEXT},
{BDIMAGE, IDS_IMAGE},
{BDFILE, IDS_FILE},
};

/////////////////////////////////////////////////////////////////////////////

CString CAttrTypes::GetDesc(int nId)
{
   int i = 0; for (i = 0; i < sizeof(m_aAttrType)/ sizeof(CAttrTypes); i++)
   {
      if (nId == m_aAttrType[i].m_nId) return BDString(m_aAttrType[i].m_nStringId);
   }
   return "";
}
   
/////////////////////////////////////////////////////////////////////////////
// CDlgFTypeAttr dialog


CDlgFTypeAttr::CDlgFTypeAttr(long lId, long lParentId, long lType, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFTypeAttr::IDD, pParent)
{
   m_lFTypeId = lId;
   m_bEdit = m_lFTypeId != 0;
   m_bUpdate = FALSE; 
   m_lType = lType;
   m_lParentId = lParentId;
   m_bShowAttr = FALSE;

	//{{AFX_DATA_INIT(CDlgFTypeAttr)
	//}}AFX_DATA_INIT
}

/////////////////////////////////////////////////////////////////////////////

CDlgFTypeAttr::~CDlgFTypeAttr()
{
}

/////////////////////////////////////////////////////////////////////////////

void CDlgFTypeAttr::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFTypeAttr)
	DDX_Control(pDX, IDC_DICTIONARY, m_cbDictionary);		
	DDX_Control(pDX, IDC_HIERARCHY, m_cbManyToOne);
	DDX_Control(pDX, IDC_PARENTFYTPE, m_cbParentFType);
	DDX_Control(pDX, IDC_FTYPELINK, m_cbFTypeLink);
	DDX_Control(pDX, IDC_ATTRTYPE, m_cbAttrType);
	DDX_Control(pDX, IDC_ATTRIBUTES, m_lbAttributes);
	DDX_Control(pDX, IDC_FTYPENAME, m_eFTypeName);
	DDX_Control(pDX, IDC_ATTRNAME, m_eAttrName);
	//}}AFX_DATA_MAP

   DDX_Control(pDX, IDC_PRIMARYKEY, m_cbPrimaryKey);
}


BEGIN_MESSAGE_MAP(CDlgFTypeAttr, CDialog)
	//{{AFX_MSG_MAP(CDlgFTypeAttr)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_LBN_SELCHANGE(IDC_ATTRIBUTES, OnSelchangeAttributes)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_CBN_SELCHANGE(IDC_ATTRTYPE, OnSelchangeAttrtype)
	ON_CBN_SELCHANGE(IDC_FTYPELINK, OnSelchangeFtypelink)
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN, OnDeltaposUpdown)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_PRIMARYKEY, OnPrimarykey)
	ON_CBN_SELCHANGE(IDC_PARENTFYTPE, OnSelchangeParentfytpe)
	ON_BN_CLICKED(IDC_UPDATE, OnUpdate)
	ON_EN_CHANGE(IDC_ATTRNAME, OnChangeAttrname)
	ON_BN_CLICKED(IDC_SECTOR, OnSector)
	ON_EN_KILLFOCUS(IDC_FTYPENAME, OnKillfocusFtypename)
	ON_CBN_SELCHANGE(IDC_DICTIONARY, OnSelchangeDictionary)
	ON_CBN_KILLFOCUS(IDC_DICTIONARY, OnKillfocusDictionary)
	ON_CBN_KILLFOCUS(IDC_PARENTFYTPE, OnKillfocusFtypename)
	ON_CBN_KILLFOCUS(IDC_HIERARCHY, OnKillfocusFtypename)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgFTypeAttr message handlers

BOOL CDlgFTypeAttr::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Initialise available types
  	
   int i = 0; for (i = 0; i < sizeof(CAttrTypes::m_aAttrType)/sizeof(CAttrTypes); i++)
   {
      int index = m_cbAttrType.AddString(BDString(CAttrTypes::m_aAttrType[i].m_nStringId));
      m_cbAttrType.SetItemData(index, CAttrTypes::m_aAttrType[i].m_nId);
   }  

   // Initialise available feature types for links and parent types

   CFeatureType ftype;

   int index = m_cbParentFType.AddString(BDString(IDS_NONE));
   m_cbParentFType.SetItemData(index, 0);
   m_cbParentFType.SetCurSel(index); // Set as default

   // Default type of link is [new...] i.e. auto create

   index = m_cbFTypeLink.AddString(BDString(IDS_ADDNEW));
   m_cbFTypeLink.SetItemData(index, -1);
   m_cbFTypeLink.SetCurSel(index);

   BOOL bFound = BDFeatureType(BDHandle(), &ftype, BDGETINIT); 
   while (bFound)
   {
	  int index = m_cbFTypeLink.AddString(ftype.m_sDesc);
	  m_cbFTypeLink.SetItemData(index, ftype.m_lId);

     index = m_cbParentFType.AddString(ftype.m_sDesc);
	  m_cbParentFType.SetItemData(index, ftype.m_lId);

     // Restore parent
     if (ftype.m_lId == m_lParentId) m_cbParentFType.SetCurSel(index);

	  bFound = BDGetNext(BDHandle());
   }
   BDEnd(BDHandle());
   if (m_cbFTypeLink.GetCurSel() == CB_ERR) m_cbFTypeLink.SetCurSel(0);

   m_cbManyToOne.SetCurSel(TRUE);   
   m_cbPrimaryKey.SetCurSel(FALSE);


   // Initialise the dictionary settings

   InitSectors();   

   // Restrict the length of controls
   
   m_eFTypeName.SetLimitText(BD_NAMESTR);
   m_eAttrName.SetLimitText(BD_NAMESTR);

   // If editing an existing feature type then retrieve the data

   if (m_bEdit)
   {
      OnInitEdit();   
   } else
   {  
      OnInitAdd();
   }

   OnSelchangeParentfytpe();
   OnSelchangeAttributes();
   OnSelchangeAttrtype();

   // Display name and date attributes

   UpdateAttr();

   // Hide rest of window

   if (!m_bEdit)
   {
	   ResizeWindow(IDS_FTYPE);
   } else
   {
      m_bShowAttr = TRUE;
   }
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////

void CDlgFTypeAttr::OnKillfocusFtypename() 
{
   UpdateAttr();
}


/////////////////////////////////////////////////////////////////////////////
//
// Include attributes feature type [name] and date so that user is aware of
// them
//

void CDlgFTypeAttr::UpdateAttr()
{
   CString sName;

   // Search for an attribute of type BDFTYPE and remove it

   int i = 0; for (i = 0; i < m_lbAttributes.GetCount(); i++)
   {
      CFTypeAttr* pFTypeAttr = (CFTypeAttr*)m_lbAttributes.GetItemDataPtr(i);

      if (pFTypeAttr->GetDataType() == BDFTYPE)
      {
         m_lbAttributes.DeleteString(i);
         i--;
      }
      else if (pFTypeAttr->GetDataType() == BDDATE)
      {
         m_lbAttributes.DeleteString(i);
         i--;
      }
   }

   // Insert the new string
   
   m_eFTypeName.GetWindowText(sName);
   sName.TrimRight();
   if (!sName.IsEmpty())
   {
      // Determine parent name for one to one relationships

      if (!m_cbManyToOne.GetCurSel())
      {
         CFeatureType ftype;
         long lFTypeI = m_cbParentFType.GetItemData(m_cbParentFType.GetCurSel());
         if (lFTypeI > 0) 
         {
            BDFTypeI(BDHandle(), lFTypeI, &ftype);
            sName = ftype.m_sDesc;
         };
      }

      sName += " [" + BDString(IDS_NAME) + "]";
      int index = m_lbAttributes.InsertString(0, sName);
   

      CFTypeAttr* pFTypeAttr = new CFTypeAttr();
      pFTypeAttr->m_lDataType = BDFTYPE;

      m_lbAttributes.SetItemDataPtr(index, pFTypeAttr);

      // Insert the date

      index = m_lbAttributes.InsertString(1, BDString(IDS_DATE));
      pFTypeAttr = new CFTypeAttr();
      pFTypeAttr->m_lDataType = BDDATE;
      m_lbAttributes.SetItemDataPtr(index, pFTypeAttr);
   };   
}

/////////////////////////////////////////////////////////////////////////////

void CDlgFTypeAttr::OnInitAdd()
{
   // Determine feature type id
   
   m_iFlag = BDADD;
   
   BDNextId(BDHandle(), BDFTYPE, 0, &m_lFTypeId);

   // Add attributes for points, polylines or polygons types

   if (m_lType != 0)
   {
      CFTypeAttr ftypeattr;
      ftypeattr.m_lFType = m_lFTypeId;	   
      ftypeattr.m_lAttrId = 1;
      ftypeattr.m_lDataType = m_lType;
   
      if (m_lType == BDCOORD)
      {      
         ftypeattr.m_sDesc = ftypeattr.m_sColName = BDString(IDS_COORDTYPE);      	   		   
      }
      else if (m_lType == BDMAPLINES)
      {
         ftypeattr.m_sDesc = ftypeattr.m_sColName = BDString(IDS_POLYLINESTYPE);      	   		   
      }
      int index = m_lbAttributes.AddString(ftypeattr.m_sDesc);
      m_lbAttributes.SetItemDataPtr(index, new CFTypeAttr(ftypeattr));	   
   };
}

/////////////////////////////////////////////////////////////////////////////

void CDlgFTypeAttr::OnInitEdit()
{   
   CFeatureType ftype;   

   m_iFlag = BDUPDATE;

  // Retrieve the feature name

   ftype.m_lId = m_lFTypeId;
   
   if (BDFeatureType(BDHandle(), &ftype, BDSELECT))
   {
      m_ftypeEdit = ftype;

	   m_eFTypeName.SetWindowText(ftype.m_sDesc);      	   
      m_cbParentFType.EnableWindow(FALSE);
      m_cbManyToOne.EnableWindow(FALSE);            

	   m_cbManyToOne.SetCurSel(ftype.m_bManyToOne);

	  // Set the parent fype

	   int i = 0; for (i = 0; i < m_cbParentFType.GetCount(); i++)
	   {
		  if (m_cbParentFType.GetItemData(i) == (DWORD)ftype.m_lParentFType)
		  {
			  m_cbParentFType.SetCurSel(i);
			  break;
		  }
	   }

      // Set the dictionary type

      for (i = 0; i < m_cbDictionary.GetCount(); i++)
      {
         if (m_cbDictionary.GetItemData(i) == (DWORD)ftype.m_lDictionary)
         {
            m_cbDictionary.SetCurSel(i);
            break;
         }
      }

	  // Retrieve attributes for the feature type

      CFTypeAttr ftypeattr;
      ftypeattr.m_lFType = ftype.m_lId;	   

	   BOOL bFound = BDFTypeAttr(BDHandle(), &ftypeattr, BDSELECT2);
	   while (bFound)
	   {
		  int index = m_lbAttributes.AddString(ftypeattr.m_sDesc);
        m_lbAttributes.SetItemDataPtr(index, new CFTypeAttr(ftypeattr));

		  bFound = BDGetNext(BDHandle());
	   }
   };
   m_lbAttributes.SetCurSel(0);
   BDEnd(BDHandle());
}

/////////////////////////////////////////////////////////////////////////////

void CDlgFTypeAttr::OnSelchangeAttributes() 
{   
   CFTypeAttr* pFTypeAttr = NULL;
   
   // Restores the selected attributes values to the controls

   int index = m_lbAttributes.GetCurSel();
   if (index != LB_ERR)
   {
      pFTypeAttr = (CFTypeAttr*)m_lbAttributes.GetItemDataPtr(index); 

      if (pFTypeAttr->GetDataType() == BDFTYPE ||
          pFTypeAttr->GetDataType() == BDDATE)
      {
         GetDlgItem(IDC_UPDATE)->EnableWindow(FALSE);
         GetDlgItem(IDC_DELETE)->EnableWindow(FALSE);
         m_cbFTypeLink.EnableWindow(FALSE);
         m_eAttrName.SetWindowText("");
         m_cbAttrType.SetCurSel(-1);
         return;
      } else
      {
         GetDlgItem(IDC_UPDATE)->EnableWindow(TRUE);
         GetDlgItem(IDC_DELETE)->EnableWindow(TRUE);
         m_cbFTypeLink.EnableWindow(FALSE);
      }

      // Restore name

      m_eAttrName.SetWindowText(pFTypeAttr->m_sDesc);
   
      // Restore data type

      int i = 0; for (i = 0; i < m_cbAttrType.GetCount(); i++)
      {
         if ((long)m_cbAttrType.GetItemData(i) == pFTypeAttr->GetDataType()) 
         {
            m_cbAttrType.SetCurSel(i);
            m_eAttrName.SetWindowText(pFTypeAttr->GetDesc());
         };
      }

      // Restore feature link

      if (pFTypeAttr->GetDataType() == BDLINK)
      {
         for (i = 0; i < m_cbFTypeLink.GetCount(); i++)
         {
	         if ((long)m_cbFTypeLink.GetItemData(i) == pFTypeAttr->GetFTypeLink())
	         {
		         m_cbFTypeLink.SetCurSel(i);
		         break;
	         }
         }
      };

      // Restore primary key

      m_cbPrimaryKey.SetCurSel(pFTypeAttr->IsPrimaryKey() == TRUE);
   };

   // Update selections

   OnSelchangeAttrtype();

   m_bUpdate = FALSE;
}

/////////////////////////////////////////////////////////////////////////////

void CDlgFTypeAttr::OnPrimarykey() 
{
	OnSelchangeAttrtype();
   m_bUpdate = TRUE;
}

/////////////////////////////////////////////////////////////////////////////

void CDlgFTypeAttr::OnAdd() 
{
   CFTypeAttr ftypeattr;
	
   m_eAttrName.GetWindowText(ftypeattr.m_sDesc);      

   // Validate name

   /*if (!IsNameValid(ftypeattr.m_sDesc))
   {
      return;
   }*/  
   
   ftypeattr.m_sColName = ftypeattr.m_sDesc;
   RemoveInvalidChar(ftypeattr.m_sColName);
   CheckNameUnique(ftypeattr.m_sColName);         

   // Retrieve the data type

   int index = m_cbAttrType.GetCurSel();
   if (index != CB_ERR)
   {
	   ftypeattr.m_lDataType = m_cbAttrType.GetItemData(index);
   } else
   {
      AfxMessageBox(BDString(IDS_NODATATYPE));
      return;
   }

   // Retrieve the link

   if (m_cbFTypeLink.IsWindowEnabled())
   {
      int i = m_cbFTypeLink.GetCurSel();
      if (i != CB_ERR)
      {
         ftypeattr.m_lFTypeLink = m_cbFTypeLink.GetItemData(i);
      } else
      {
         AfxMessageBox(BDString(IDS_NOLINK));
         return;
      }
   };   

   // Retrieve the primary key

   ftypeattr.m_bPrimaryKey = m_cbPrimaryKey.GetCurSel();
    
   // Check the name is unique and if so add it to the list

	if (!ftypeattr.m_sDesc.IsEmpty() && 
       m_lbAttributes.FindStringExact(-1,ftypeattr.m_sDesc) == LB_ERR)
	{
      ftypeattr.m_lFType = m_lFTypeId;
      int index = m_lbAttributes.AddString(ftypeattr.m_sDesc);
      m_lbAttributes.SetItemDataPtr(index, new CFTypeAttr(ftypeattr));	
      m_lbAttributes.SetCurSel(index);

      // Retrieve the remaining attributes

      OnSelchangeAttrtype();

      // Reset

      m_eAttrName.SetWindowText("");
      m_bUpdate = FALSE;
	}	   
}

///////////////////////////////////////////////////////////
//
// void CDlgFTypeAttr::OnDelete() 
//

void CDlgFTypeAttr::OnDelete() 
{
   int index = m_lbAttributes.GetCurSel();
   if (index != LB_ERR)
   {
      CFTypeAttr* pFTypeAttr = (CFTypeAttr*)m_lbAttributes.GetItemDataPtr(index);

      ASSERT(pFTypeAttr != NULL);
      delete pFTypeAttr;

      m_lbAttributes.DeleteString(index);
	   m_eAttrName.SetWindowText("");

      m_bUpdate = FALSE;
   }	
}

///////////////////////////////////////////////////////////

void CDlgFTypeAttr::OnSelchangeAttrtype() 
{
   CFTypeAttr* pFTypeAttr = NULL;
   CString sName;   

   // Enable ftype link only for links

   m_cbFTypeLink.EnableWindow(FALSE);
   int index = m_cbAttrType.GetCurSel();
   if (index != CB_ERR)
   {
      if (m_cbAttrType.GetItemData(index) == BDLINK)
      {
         m_cbFTypeLink.EnableWindow(TRUE);
      }
   }   
}

///////////////////////////////////////////////////////////////////////////////

void CDlgFTypeAttr::OnSelchangeFtypelink() 
{
   OnSelchangeAttrtype();	
   m_bUpdate = TRUE;
}

///////////////////////////////////////////////////////////

void CDlgFTypeAttr::OnSelchangeParentfytpe() 
{
   m_cbManyToOne.EnableWindow(!m_bEdit);

	int index = m_cbParentFType.GetCurSel();
   if (index != LB_ERR)
   {
      if (m_cbParentFType.GetItemData(index) == 0)
      {
         m_cbManyToOne.EnableWindow(FALSE);
      }
   }	
}

///////////////////////////////////////////////////////////

void CDlgFTypeAttr::OnOK() 
{   
   BOOL bOK = TRUE;   
   CAttrArray aAttr;
   CFeatureType ftype;
   CFTypeAttr ftypeattr;

   // Make sure whole window is visible

   if (!m_bShowAttr && m_lType == 0)
   {
      OnSelchangeDictionary();
      return;
   }
      
   // If editing then obtain a copy of the existing attributes

   if (bOK && m_bEdit)
   {       
	   bOK = BDFTypeAttrInit(BDHandle(), m_lFTypeId, &aAttr);
   }
  
   BDBeginTrans(BDHandle());

   TRY
   {

   // If there is a name in the attribute edit control and it doesn't exist
   // in the list box then add it
   
   if (m_bUpdate)      
   {
      if (AfxMessageBox(BDString(IDS_NOTSAVED), MB_YESNO|MB_DEFBUTTON2) == IDNO)
      {
         BDRollBack(BDHandle());
         return;
      }
   }
   
	// Retrieve name of feature type

   if (!m_bEdit) 
   {
      m_eFTypeName.GetWindowText(ftype.m_sInternal);
   } else
   {
      ftype.m_sInternal = m_ftypeEdit.m_sInternal;
   }
   m_eFTypeName.GetWindowText(ftype.m_sDesc);

   // Retrieve feature type

   int index = m_cbParentFType.GetCurSel();
   if (index != CB_ERR)
   {
      ftype.m_lParentFType = m_cbParentFType.GetItemData(index);	   
	   ftype.m_bManyToOne = m_cbManyToOne.GetCurSel();
   } else
   {
      AfxMessageBox(BDString(IDS_NOPARENTFTYPE));
	   bOK = FALSE;
   }

   // Retrieve dictionary

   if (bOK)
   {
      index = m_cbDictionary.GetCurSel();
      if (index != CB_ERR)
      {
         ftype.m_lDictionary = m_cbDictionary.GetItemData(index);
      } else
      {
         AfxMessageBox(BDString(IDS_NOSECTOR));
         bOK = FALSE;
      }
   };

   // Save if okay

   if (bOK && ftype.m_sDesc[0] != '\0')
   {      
      // Save feature type

	  ftype.m_lId = m_lFTypeId;

      if (BDFeatureType(BDHandle(), &ftype, m_iFlag))
      {
		  BDEnd(BDHandle());

		 // Delete existing attributes

        CFTypeAttr ftypeattr;
        ftypeattr.m_lFType = ftype.m_lId;		  
		  BDFTypeAttr(BDHandle(), &ftypeattr, BDDELETEALL);
		  BDEnd(BDHandle());

         // Save corresponding attributes
               
         int i = 0; for (i = 0; bOK && i < m_lbAttributes.GetCount(); i++)
         {                        
            CFTypeAttr* pFTypeAttr = (CFTypeAttr*)m_lbAttributes.GetItemDataPtr(i);
            
            if (pFTypeAttr->GetDataType() != BDFTYPE && 
                pFTypeAttr->GetDataType() != BDDATE)
            {
               m_lbAttributes.GetText(i, pFTypeAttr->m_sDesc);
               pFTypeAttr->m_lAttrId = i+1;
      
               // Create auto create link

               if (pFTypeAttr->m_lDataType == BDLINK && pFTypeAttr->m_lFTypeLink == -1)
               {
                  CFeatureType ftypeL;
                  BDNextId(BDHandle(), BDFTYPE, 0, &ftypeL.m_lId);
                  ftypeL.m_sDesc = ftype.m_sDesc + " " + pFTypeAttr->m_sDesc;
                  ftypeL.m_sInternal = ftypeL.m_sDesc;
                  ftypeL.m_lParentFType = 0;
                  ftypeL.m_bManyToOne = FALSE;
                  ftypeL.m_lDictionary = ftype.m_lDictionary;
                  if (BDFeatureType(BDHandle(), &ftypeL, BDADD) && 
                      BDFTypeCreate(BDHandle(), ftypeL.m_lId))
                  {
                     pFTypeAttr->m_lFTypeLink = ftypeL.m_lId;
                  } 
                  BDEnd(BDHandle());
               }

			      if (pFTypeAttr->m_lDataType != BDLINK || 
                   (pFTypeAttr->m_lDataType == BDLINK && pFTypeAttr->m_lFTypeLink > 0))
			      {
				      bOK = BDFTypeAttr(BDHandle(), pFTypeAttr, BDADD);				
			      } else
			      {               
				      AfxMessageBox(BDString(IDS_LINKSNOTDEFINE));
				      bOK = FALSE;
			      }
            };                       
         };
         BDEnd(BDHandle());

         // Create the corresponding table

         if (bOK)
         {
			 if (m_bEdit)
			 {
                bOK = BDFTypeUpdate(BDHandle(), m_lFTypeId, &aAttr);
			 } else
			 {
                bOK = BDFTypeCreate(BDHandle(), m_lFTypeId);
			 };
         };
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
	   CDialog::OnOK();
   } else
   {
      BDRollBack(BDHandle());
   }
}

///////////////////////////////////////////////////////////////////////////////
//
// Allow the changing of the order of attributes
//

void CDlgFTypeAttr::OnDeltaposUpdown(NMHDR* pNMHDR, LRESULT* pResult) 
{
   CString s;   

   NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
     
   int iSwap = -1;
	int i = m_lbAttributes.GetCurSel();
   

   // Determine which items to swap

   if (pNMUpDown->iDelta == -1 && i > 0)
   {
      iSwap = i-1;
      
   }    
   else if (pNMUpDown->iDelta == 1 && i != LB_ERR && i+1 < m_lbAttributes.GetCount())
   {
      iSwap = i+1;
   };

   // Disable move of ftype and date
   if (i <=1 || iSwap <=1 ) return;

   // Swap the items
   if (iSwap != -1)
   {
      m_lbAttributes.GetText(i, s);
      CFTypeAttr* pFTypeAttr = (CFTypeAttr*)m_lbAttributes.GetItemDataPtr(i);      

      m_lbAttributes.DeleteString(i);
      m_lbAttributes.InsertString(iSwap, s);
      m_lbAttributes.SetItemDataPtr(iSwap, pFTypeAttr);

      m_lbAttributes.SetCurSel(iSwap);
   };
	
	*pResult = 0;
}

///////////////////////////////////////////////////////////////////////////////

void CDlgFTypeAttr::OnClose() 
{	
	CDialog::OnClose();
}

///////////////////////////////////////////////////////////////////////////////

void CDlgFTypeAttr::OnUpdate() 
{   
   CFTypeAttr* pFTypeAttr = NULL;
   CString sName;

   // Retrieve the currently selected attribute
     
	int index = m_lbAttributes.GetCurSel();
	if (index != LB_ERR)
	{                 
      pFTypeAttr = (CFTypeAttr*)m_lbAttributes.GetItemDataPtr(index);      
	}   
     
   if (pFTypeAttr != NULL)
   {

      // Retrieve name

      m_eAttrName.GetWindowText(sName);      
      
      // Validate name

      /*if (!IsNameValid(sName))
      {
         return;
      }*/     

      pFTypeAttr->m_sDesc = sName;

      // Ensure column name is unique

      if (!m_bEdit)
      {         
         RemoveInvalidChar(sName);
         pFTypeAttr->m_sColName = sName;
      }

      // Retrieve the data type

      int iType = m_cbAttrType.GetCurSel();
      if (iType != CB_ERR)
      {         
         long lDataType = m_cbAttrType.GetItemData(iType);

        // Re-insert the item with the new name

         m_lbAttributes.DeleteString(index);
         index = m_lbAttributes.InsertString(index, pFTypeAttr->m_sDesc);
         m_lbAttributes.SetItemDataPtr(index, pFTypeAttr);
         m_lbAttributes.SetCurSel(index);

        // If editing, then warn of losing data

        if (lDataType != pFTypeAttr->m_lDataType &&
           AfxMessageBox(BDString(IDS_DATATYPECHANGE), MB_YESNO) != IDYES)
        {
           return;
        }

	      pFTypeAttr->m_lDataType = lDataType;
       
	      // Retrieve the link

	      m_cbFTypeLink.EnableWindow(pFTypeAttr->m_lDataType == BDLINK);

	      if (pFTypeAttr->m_lDataType == BDLINK)
	      {
		     int index = m_cbFTypeLink.GetCurSel();
		     pFTypeAttr->m_lFTypeLink = m_cbFTypeLink.GetItemData(index);
	      }      

         // Retrieve the primary key

         pFTypeAttr->m_bPrimaryKey = m_cbPrimaryKey.GetCurSel();
      };
   };	

   m_bUpdate = FALSE;
}

///////////////////////////////////////////////////////////////////////////////

void CDlgFTypeAttr::OnChangeAttrname() 
{
   CString(s);
   m_eAttrName.GetWindowText(s);
   GetDlgItem(IDC_ADD)->EnableWindow(!s.IsEmpty());
   
 	m_bUpdate = TRUE;	
}

///////////////////////////////////////////////////////////////////////////////

BOOL CDlgFTypeAttr::DestroyWindow() 
{
	int i = 0; for (i = 0; i < m_lbAttributes.GetCount(); i++)
   {
      CFTypeAttr* p = (CFTypeAttr*)m_lbAttributes.GetItemDataPtr(i);
      ASSERT(p != NULL);
      delete p;
   }
   m_lbAttributes.ResetContent();
	
	return CDialog::DestroyWindow();
}

///////////////////////////////////////////////////////////////////////////////

/*BOOL CDlgFTypeAttr::IsNameValid(CString& sName)
{
   sName.TrimLeft();
   sName.TrimRight();

   if (sName.FindOneOf(".[]'") != -1)
   {
      AfxMessageBox(BDString(IDS_ATTRNAMEINVALID) + " .[]'");
      return FALSE;
   }

   if (sName == "")
   {
      AfxMessageBox(BDString(IDS_NOTBLANK));
      return FALSE;
   }
   return TRUE;

}*/

///////////////////////////////////////////////////////////////////////////////
//
// Remove characters from the string that are not valid database column names
//

void CDlgFTypeAttr::RemoveInvalidChar(CString& s)
{
   s.Replace("\'","");
   s.Replace("[","");
   s.Replace("]","");
   s.Replace(".","");
   s.Replace("`","");
}

///////////////////////////////////////////////////////////////////////////////
//
//  Ensure that all internal names are unique
//

void CDlgFTypeAttr::CheckNameUnique(CString& sName)
{
   int iCount = 0;
   CString s;
   int i = 0;

   do 
   {      
      if (iCount > 0) s.Format("%s%i", sName, iCount);
      else s = sName;
   
      for (i = 0; i < m_lbAttributes.GetCount(); i++)
      {
          CFTypeAttr* pFTypeAttr = (CFTypeAttr*)m_lbAttributes.GetItemDataPtr(i);      
          if (pFTypeAttr->m_sColName == s)
          {          
             break;
          }
      };
      iCount++;
   } while (i != m_lbAttributes.GetCount());

   sName = s;
};

///////////////////////////////////////////////////////////////////////////////
//
// Allow users to add new sectors here
//

void CDlgFTypeAttr::OnSector() 
{
   CComboBoxFType cbFType;   
   cbFType.InitDictionary();

   cbFType.ShowDictionary();   

   // Initialise the dictionary settings

   InitSectors();
}

///////////////////////////////////////////////////////////////////////////////
//
// Update list of sectors, retaining old list

void CDlgFTypeAttr::InitSectors()
{
   long lId = 0;
   int index = m_cbDictionary.GetCurSel();
   if (index != CB_ERR) lId = m_cbDictionary.GetItemData(index);
   
   m_cbDictionary.ResetContent();
   CDictionary dictionary;
   BOOL bFound = BDDictionary(BDHandle(), &dictionary, BDGETINIT);
   while (bFound)
   {
      int index = m_cbDictionary.AddString(dictionary.m_sDesc);
      m_cbDictionary.SetItemData(index, dictionary.m_lId);
      if (lId == dictionary.m_lId)
      {
         m_cbDictionary.SetCurSel(index);
      }

      bFound = BDGetNext(BDHandle());
   }
   BDEnd(BDHandle());

   if (m_cbDictionary.GetCurSel() == CB_ERR) m_cbDictionary.SetCurSel(0);
}

///////////////////////////////////////////////////////////////////////////////

void CDlgFTypeAttr::OnSelchangeDictionary() 
{
   // Display whole window

	ResizeWindow(IDS_ATTR);
   m_bShowAttr = TRUE;

   m_eAttrName.SetFocus();
	
}

///////////////////////////////////////////////////////////////////////////////

void CDlgFTypeAttr::OnKillfocusDictionary() 
{
	OnSelchangeDictionary();	
}

///////////////////////////////////////////////////////////////////////////////

void CDlgFTypeAttr::ResizeWindow(int nControl)
{
   CRect rectW, rectC;   
         
   GetWindowRect(rectW);
   GetDlgItem(nControl)->GetWindowRect(rectC);   
   
   int nSpace = GetDC()->GetTextExtent("ABC").cy;

   SetWindowPos(NULL,rectW.left,rectW.left,
                rectW.Width(), rectC.bottom - rectW.top +nSpace/2, 
                SWP_NOZORDER);

   CenterWindow();
};



