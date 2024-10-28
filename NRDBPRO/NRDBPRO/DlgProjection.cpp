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
#include "DlgProjection.h"
#include "viewmap.h"
#include "docmap.h"
#include "dlgutm.h"
#include "projctns.h"
#include "definitions.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

#define ELLIPSOIDDP 7
#define ACCURACY 1e-7

/////////////////////////////////////////////////////////////////////////////

inline void swap(double& d1, double& d2)
{
   double dTemp = d1;
   d1 = d2;
   d2 = dTemp;
}

/////////////////////////////////////////////////////////////////////////////
// CDlgProjection dialog


CDlgProjection::CDlgProjection(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProjection::IDD, pParent)
{
   m_lMaxId = 0;
   m_lDefaultId = 0;
   m_bLatLon = FALSE;
   m_bUpdate = FALSE;
   m_bUTMTip = FALSE;

	//{{AFX_DATA_INIT(CDlgProjection)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CDlgProjection::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgProjection)	
	DDX_Control(pDX, IDC_COORDASLATLON, m_ckCoordAsLatLon);
	DDX_Control(pDX, IDC_ELLIPSOID, m_cbEllipsoid); 
	DDX_Control(pDX, IDC_MINNORTHING, m_eMinNorthing);
	DDX_Control(pDX, IDC_MINEASTING, m_eMinEasting);
	DDX_Control(pDX, IDC_MAXNORTHING, m_eMaxNorthing);
	DDX_Control(pDX, IDC_MAXEASTING, m_eMaxEasting);
	DDX_Control(pDX, IDC_SEMIMINORAXIS, m_eSemiMinorAxis);
	DDX_Control(pDX, IDC_SEMIMAJORAXIS, m_eSemiMajorAxis);
	DDX_Control(pDX, IDC_SCALEFACTOR, m_eScaleFactor);
	DDX_Control(pDX, IDC_LONGITUDEORIGIN, m_eLongitudeOrigin);
	DDX_Control(pDX, IDC_LATITUDEORIGIN, m_eLatitudeOrigin);
	DDX_Control(pDX, IDC_FALSENORTHING, m_eFalseNorthing);
	DDX_Control(pDX, IDC_FALSEEASTING, m_eFalseEasting);
	DDX_Control(pDX, IDC_EDITNAME, m_eName);
	DDX_Control(pDX, IDC_NAME, m_cbProjections);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgProjection, CDialog)
	//{{AFX_MSG_MAP(CDlgProjection)
	ON_CBN_SELCHANGE(IDC_NAME, OnSelchangeName)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_UPDATE, OnEdit)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
   ON_BN_CLICKED(IDC_SETEXTENT, OnSetExtent)
	ON_CBN_SELCHANGE(IDC_ELLIPSOID, OnSelchangeEllipsoid)
	ON_EN_CHANGE(IDC_EDITNAME, OnChange)
	ON_EN_CHANGE(IDC_FALSEEASTING, OnChange)
	ON_EN_CHANGE(IDC_FALSENORTHING, OnChange)
	ON_EN_CHANGE(IDC_LATITUDEORIGIN, OnChange)
	ON_EN_CHANGE(IDC_LONGITUDEORIGIN, OnChange)
	ON_EN_CHANGE(IDC_MAXEASTING, OnChange)
	ON_EN_CHANGE(IDC_MAXNORTHING, OnChange)
	ON_EN_CHANGE(IDC_MINEASTING, OnChange)
	ON_EN_CHANGE(IDC_MINNORTHING, OnChange)
	ON_EN_CHANGE(IDC_SCALECOORDS, OnChange)
	ON_EN_CHANGE(IDC_SCALEFACTOR, OnChange)
	ON_EN_CHANGE(IDC_SEMIMAJORAXIS, OnChange)
	ON_EN_CHANGE(IDC_SEMIMINORAXIS, OnChange)
   ON_MESSAGE(WM_UTMTIP, OnUTMTip)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgProjection message handlers

BOOL CDlgProjection::OnInitDialog() 
{
	CDialog::OnInitDialog();   

   if (BDGetTransLevel(BDHandle()) > 0) BDCommit(BDHandle());
   ASSERT(BDGetTransLevel(BDHandle()) == 0);

   // Fill list of projections (skip UTM)

   for (int i = 1; i < sizeof(CEllipsoid::m_aEllipsoids)/sizeof(ELLIPSOID); i++)
   {
       int index = m_cbEllipsoid.AddString(CEllipsoid::m_aEllipsoids[i].m_psName);
       m_cbEllipsoid.SetItemData(index, i);
   }

   Update();

      // If no projection then set to UTM scale factor to 1

   if (m_lDefaultId == 0)
   {      
      if (!m_bUTMTip) PostMessage(WM_UTMTIP);
   }

   // Display the default format for coordinates

   m_ckCoordAsLatLon.SetCheck(BDGetSettings().m_bCoordAsLatLon);
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////

void CDlgProjection::Update(BOOL bInit)
{
   m_cbProjections.ResetContent();

     // List available projections

   CBDProjection projection;

   BOOL bFound = BDProjection(BDHandle(), &projection, BDGETINIT);
   while (bFound)
   {
      int index = m_cbProjections.AddString(projection.m_sName);
      m_cbProjections.SetItemData(index, projection.m_nID);

      if (projection.m_bDefault) 
      {
         if (bInit)
         {
            m_lDefaultId = projection.m_nID;
         };
         m_cbProjections.SetCurSel(index);
      };
      m_lMaxId = max(m_lMaxId, projection.m_nID);

      bFound = BDGetNext(BDHandle());
   }
   BDEnd(BDHandle());   

   OnSelchangeName();	
}

/////////////////////////////////////////////////////////////////////////////

void CDlgProjection::OnSelchangeName() 
{
   CBDProjection projection;

   // Determine selected value

   int index = m_cbProjections.GetCurSel();
   if (index != CB_ERR)
   {
      // Retrieve value from the database

      projection.m_nID = m_cbProjections.GetItemData(index);

      if (BDProjection(BDHandle(), &projection, BDSELECT))
      {
         // Determine if the elipsoid parameters match any of the defaults

         for (index = 0; index < m_cbEllipsoid.GetCount(); index++)
         {
            int i = m_cbEllipsoid.GetItemData(index);
            CEllipsoid* pEllipsoid = (CEllipsoid*)(&CEllipsoid::m_aEllipsoids[i]);               
            
            // Match Ellipsoid

            if (fabs(projection.m_dSemiMajorAxis - pEllipsoid->GetSemiMajorAxis()) < ACCURACY &&
                fabs(projection.m_dSemiMinorAxis - pEllipsoid->GetSemiMinorAxis()) < ACCURACY)
            {
               m_cbEllipsoid.SetCurSel(index);
               OnSelchangeEllipsoid();
               break;
            }
            

            // Latitude / Longitude

            if (fabs(projection.m_dSemiMajorAxis - pEllipsoid->GetSemiMajorAxis()) < ACCURACY && 
                fabs(projection.m_dSemiMajorAxis - LATLON_ID) < ACCURACY)
            {
               m_cbEllipsoid.SetCurSel(index);
               OnSelchangeEllipsoid();
               break;
            }

            // Default
            
            if (fabs(pEllipsoid->GetSemiMajorAxis()) < ACCURACY)
            {
               m_cbEllipsoid.SetCurSel(index);               
            }            
         }

      // Update controls

         m_eName.SetWindowText(projection.m_sName);	
         m_eSemiMajorAxis.SetValue(projection.m_dSemiMajorAxis, ELLIPSOIDDP);	      
         m_eSemiMinorAxis.SetValue(projection.m_dSemiMinorAxis, ELLIPSOIDDP);	      
         m_eLatitudeOrigin.SetValue(projection.m_dLatitudeOrigin, 1);
	      m_eLongitudeOrigin.SetValue(projection.m_dLongitudeOrigin, 1);	      
	      m_eFalseNorthing.SetValue(projection.m_dFalseNorthing, 1);
	      m_eFalseEasting.SetValue(projection.m_dFalseEasting, 1);
         m_eScaleFactor.SetValue(projection.m_dScaleFactorAtOrigin, 10);         

         // Display min/max easting/northing as latitude/longitude

         double dMinEasting = projection.m_dMinEasting;
         double dMaxEasting = projection.m_dMaxEasting;
         double dMinNorthing = projection.m_dMinNorthing;
         double dMaxNorthing = projection.m_dMaxNorthing;

         int nDP = 0;
         if (m_bLatLon)
         {
            BDProjection()->TransMercatorToLatLon(dMinEasting, dMinNorthing, &dMinNorthing, &dMinEasting);
            BDProjection()->TransMercatorToLatLon(dMaxEasting, dMaxNorthing, &dMaxNorthing, &dMaxEasting);
            nDP = -1;
         }

         m_eMinEasting.SetValue(dMinEasting,nDP);
         m_eMaxEasting.SetValue(dMaxEasting,nDP);
         m_eMinNorthing.SetValue(dMinNorthing,nDP);
         m_eMaxNorthing.SetValue(dMaxNorthing,nDP);	                     
      }
      BDEnd(BDHandle());
   }        

   // Update the default conversion

   UpdateDefault();

   m_bUpdate = FALSE;
}

/////////////////////////////////////////////////////////////////////////////

LRESULT CDlgProjection::OnUTMTip(WPARAM, LPARAM)
{
   CDlgUTM dlg;
   if (dlg.DoModal() == IDOK)
   {
      CString s; 
      s.Format("%i", -177+(abs(dlg.GetZone())-1)*6);   

      CProjection projection;

      projection.m_bDefault = TRUE;
      projection.m_nID = 1;
      projection.m_dFalseEasting = 500000;
      if (dlg.GetZone() > 0) projection.m_dFalseNorthing = 0; // Northern hemisphere
      else projection.m_dFalseNorthing = 10000000; // Southern hemisphere
      projection.m_dLatitudeOrigin = 0;
      projection.m_dLongitudeOrigin = -177+(abs(dlg.GetZone())-1)*6;
      projection.m_dScaleFactorAtOrigin = UTM_SCALEFACTOR;
      projection.m_dSemiMajorAxis = CEllipsoid::m_aEllipsoids[UTM_INDEX].m_dSemiMajorAxis;
      CEllipsoid ellipsoid = CEllipsoid::m_aEllipsoids[UTM_INDEX];
      projection.m_dSemiMinorAxis = ellipsoid.GetSemiMinorAxis();
      projection.m_lScaleCoords = 1;
      projection.m_dMaxEasting = AFX_RFX_DOUBLE_PSEUDO_NULL;
      projection.m_dMinEasting = AFX_RFX_DOUBLE_PSEUDO_NULL;
      projection.m_dMaxNorthing = AFX_RFX_DOUBLE_PSEUDO_NULL;
      projection.m_dMinNorthing = AFX_RFX_DOUBLE_PSEUDO_NULL;
      projection.m_sName = CEllipsoid::m_aEllipsoids[UTM_INDEX].m_psName;
                 

      BDEnd(BDHandle());
      if (BDProjection(BDHandle(), &projection, BDADD))
      {         
         BDEnd(BDHandle());
         CDialog::OnOK();                  
      }
            
      
   }
   m_bUTMTip = TRUE;
   return 0;
}

/////////////////////////////////////////////////////////////////////////////

void CDlgProjection::OnAdd() 
{
   if (AddEdit(TRUE, m_lMaxId+1))
   {
      m_lMaxId++;
   }
   m_bUpdate = FALSE;
}

/////////////////////////////////////////////////////////////////////////////

void CDlgProjection::OnEdit() 
{
   int index = m_cbProjections.GetCurSel();
   if (index != CB_ERR)
   {
      // Retrieve value from the database

      AddEdit(FALSE, m_cbProjections.GetItemData(index));           
   };
   m_bUpdate = FALSE;
}

/////////////////////////////////////////////////////////////////////////////

void CDlgProjection::OnDelete() 
{
   CBDProjection projection;

   int index = m_cbProjections.GetCurSel();
   if (index != CB_ERR)
   {
      projection.m_nID = m_cbProjections.GetItemData(index);
      
      BDProjection(BDHandle(), &projection, BDDELETE);
      BDEnd(BDHandle());      

      Update(FALSE);      

      m_cbProjections.SetCurSel(max(0,index-1));
      
      // Store the new default
      UpdateDefault();      

      // Update attributes
      OnSelchangeName();	
      
   }   
   m_bUpdate = FALSE;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CDlgProjection::AddEdit(BOOL bAdd, long lId)
{
   BOOL bOK = FALSE;
   
   CBDProjection projection;

   // Retrieve values from controls

  if (m_eName.GetValue(projection.m_sName, FALSE) && 
      m_eSemiMajorAxis.GetValue(projection.m_dSemiMajorAxis) &&
      m_eSemiMinorAxis.GetValue(projection.m_dSemiMinorAxis) &&
      m_eLatitudeOrigin.GetValue(projection.m_dLatitudeOrigin, -90, 90) &&
      m_eLongitudeOrigin.GetValue(projection.m_dLongitudeOrigin,-180,180) &&
      m_eFalseEasting.GetValue(projection.m_dFalseEasting) &&
      m_eFalseNorthing.GetValue(projection.m_dFalseNorthing) &&	      
	   m_eScaleFactor.GetValue(projection.m_dScaleFactorAtOrigin) &&       
      m_eMinEasting.GetValue(projection.m_dMinEasting, -1e20, 1e20, TRUE) &&
      m_eMaxEasting.GetValue(projection.m_dMaxEasting, projection.m_dMinEasting, 1e20, TRUE) &&
      m_eMinNorthing.GetValue(projection.m_dMinNorthing, -1e20, 1e20, TRUE) &&
      m_eMaxNorthing.GetValue(projection.m_dMaxNorthing, projection.m_dMinNorthing, 1e20, TRUE))      
  {
     projection.m_bDefault = TRUE;
     projection.m_nID = lId;
     

     // Latitude/longitude

     if (m_bLatLon)
     {
        projection.m_dSemiMajorAxis = LATLON_ID;
        projection.m_dSemiMinorAxis = LATLON_ID;
        projection.m_dLatitudeOrigin = 0;        
        projection.m_dFalseEasting = 0;
        projection.m_dFalseNorthing = 0;
	     projection.m_dScaleFactorAtOrigin = 0;

        BDProjection()->LatLonToTransMercator(projection.m_dMinNorthing, projection.m_dMinEasting, &projection.m_dMinEasting, &projection.m_dMinNorthing);
        BDProjection()->LatLonToTransMercator(projection.m_dMaxNorthing, projection.m_dMaxEasting, &projection.m_dMaxEasting, &projection.m_dMaxNorthing);        
     }
 
     int iFlag = BDADD;
     if (!bAdd) iFlag = BDUPDATE;
     
     if (BDProjection(BDHandle(), &projection, iFlag))
     {
        bOK = TRUE;
     }
     BDEnd(BDHandle());
  }	     	     	     	     	      	

  Update(FALSE);

  return bOK;
}

/////////////////////////////////////////////////////////////////////////////

void CDlgProjection::UpdateDefault()
{
   CBDProjection projection;

   // Remove previous default

   projection.m_nID = m_lDefaultId;
   if (BDProjection(BDHandle(), &projection, BDSELECT))  
   {
      projection.m_bDefault = FALSE;
      BDEnd(BDHandle());
      BDProjection(BDHandle(), &projection, BDUPDATE);
   };
   BDEnd(BDHandle());


   // Set new default

   int index = m_cbProjections.GetCurSel();
   if (index != CB_ERR)
   {
      // Retrieve new default

      projection.m_nID = m_cbProjections.GetItemData(index);
      m_lDefaultId = projection.m_nID;

      // Update controls

      if (BDProjection(BDHandle(), &projection, BDSELECT))  
      {
         projection.m_bDefault = TRUE;
         BDEnd(BDHandle());
         BDProjection(BDHandle(), &projection, BDUPDATE);
      };
      BDEnd(BDHandle());
   };
}

/////////////////////////////////////////////////////////////////////////////

void CDlgProjection::OnSelchangeEllipsoid() 
{
   m_bLatLon = FALSE;

   //m_eName.SetWindowText("");	

   int index = m_cbEllipsoid.GetCurSel();
   if (index != CB_ERR)
   {
      int i = m_cbEllipsoid.GetItemData(index);
      CEllipsoid* pEllipsoid = (CEllipsoid*)(&CEllipsoid::m_aEllipsoids[i]);

      if (fabs(pEllipsoid->GetSemiMajorAxis()) > 0.001)
      {
         m_eSemiMajorAxis.SetValue(pEllipsoid->GetSemiMajorAxis(),ELLIPSOIDDP);
         m_eSemiMinorAxis.SetValue(pEllipsoid->GetSemiMinorAxis(),ELLIPSOIDDP);
      }      
      else 
      {
         m_eSemiMajorAxis.SetWindowText("");
         m_eSemiMinorAxis.SetWindowText("");
      }         
      
      // Determine if LatLon

      if (fabs(pEllipsoid->GetSemiMajorAxis() - LATLON_ID) < 0.001)
      {       
         m_bLatLon = TRUE;         
      }      
   }	

   // Disable controls if latlon

   m_eSemiMajorAxis.EnableWindow(!m_bLatLon);
   m_eSemiMinorAxis.EnableWindow(!m_bLatLon);
   m_eLatitudeOrigin.EnableWindow(!m_bLatLon);  
   m_eFalseEasting.EnableWindow(!m_bLatLon);
   m_eFalseNorthing.EnableWindow(!m_bLatLon);
	m_eScaleFactor.EnableWindow(!m_bLatLon);
   m_eLongitudeOrigin.EnableWindow(!m_bLatLon);

   m_bUpdate = TRUE;	
}


/////////////////////////////////////////////////////////////////////////////

void CDlgProjection::OnOK() 
{
   if (m_bUpdate)
   {
      if (AfxMessageBox(BDString(IDS_NOTSAVED), MB_YESNO|MB_DEFBUTTON2) == IDNO)
      {
         return;
      }
   }   

   // Store coordinate display preference

   CBDMain main;
	BDMain(BDHandle(), &main, BDGETINIT);
   main.m_bCoordAsLatLon = m_ckCoordAsLatLon.GetCheck();
   BDMain(BDHandle(), &main, BDUPDATE);
   BDGetSettings() = main; 

	CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////

void CDlgProjection::OnChange() 
{
	m_bUpdate = TRUE;	
}

/////////////////////////////////////////////////////////////////////////////
//
// Set the default extent to the current view in the map

void CDlgProjection::OnSetExtent()
{
   CRect rect;
   CDocMap* pDocMap = BDGetDocMap();


   if (pDocMap != NULL)
   {
      // Get Extent

      CViewMap* pViewMap = pDocMap->GetViewMap();

      pViewMap->GetClientRect(&rect);

      double dMinX = pViewMap->GetfXInv(rect.left);
      double dMaxX = pViewMap->GetfXInv(rect.right);
      double dMinY = pViewMap->GetfYInv(rect.top);
      double dMaxY = pViewMap->GetfYInv(rect.bottom);

      if (m_bLatLon)
      {
         BDProjection()->TransMercatorToLatLon(dMinX, dMinY, &dMinY, &dMinX);
         BDProjection()->TransMercatorToLatLon(dMaxX, dMaxY, &dMaxY, &dMaxX);         
      };      

      if (dMinY > dMaxY) swap(dMinY, dMaxY);

      m_eMinEasting.SetValue(dMinX);
      m_eMaxEasting.SetValue(dMaxX);

      m_eMinNorthing.SetValue(dMinY);
      m_eMaxNorthing.SetValue(dMaxY);

      OnEdit();
   }
}
