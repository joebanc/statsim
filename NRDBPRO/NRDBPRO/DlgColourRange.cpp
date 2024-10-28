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
#include "nrdbpro.h"
#include "DlgColourRange.h"
#include "pagemaplayerprop.h"
#include "viewmap.h"
#include "docmap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__; 
#endif

/////////////////////////////////////////////////////////////////////////////

#define ITERATIONS 100 

double inline square(double d) {return d*d;}
static int compare(const void *elem1, const void *elem2 ) 
{   
   if (*(double*)elem1 == *(double*)elem2) return 0;
   else if (*(double*)elem1 < *(double*)elem2) return -1;
   else return 1;
}

/////////////////////////////////////////////////////////////////////////////
// CDlgColourRange dialog


CDlgColourRange::CDlgColourRange(CMapLayer *pMapLayer, CMapProperties *pMapProp, BOOL bDefault, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgColourRange::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgColourRange)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

   m_pMapProp = pMapProp;
   m_pMapLayer = pMapLayer;
   m_bDefault = bDefault;
}


void CDlgColourRange::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgColourRange)
	DDX_Control(pDX, IDC_BUTTON1, m_pbColour);
	DDX_Control(pDX, IDC_ROUND, m_cbRound);
	DDX_Control(pDX, IDC_NUMINTERVALS, m_eIntervals);
	DDX_Control(pDX, IDC_METHOD, m_cbMethod);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgColourRange, CDialog)
	//{{AFX_MSG_MAP(CDlgColourRange)
	ON_CBN_SELCHANGE(IDC_METHOD, OnSelchangeMethod)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgColourRange message handlers

BOOL CDlgColourRange::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
   // Initialise the list of methods

	int index = m_cbMethod.AddString(BDString(IDS_EQUALINTERVALS));
   m_cbMethod.SetItemData(index, equalintervals);   
   index = m_cbMethod.AddString(BDString(IDS_PERCENTILES));
   m_cbMethod.SetItemData(index, percentiles);
   index = m_cbMethod.AddString(BDString(IDS_DEFAULT));
   m_cbMethod.SetItemData(index, defaultbreaks);
   m_cbMethod.SetCurSel(index);

   // Initialise the rounding values

   index = m_cbRound.AddString(BDString(IDS_NONE));
   m_cbRound.SetCurSel(index);
   m_cbRound.AddString("1000");
   m_cbRound.AddString("100");
   m_cbRound.AddString("10");
   m_cbRound.AddString("1");
   m_cbRound.AddString("0.1");
   m_cbRound.AddString("0.01");
   m_cbRound.AddString("0.001");

   // Set current number of intervals

   m_eIntervals.SetValue(m_pMapProp->m_aRangeColour.GetSize());

   if (m_pMapProp->m_aRangeColour.GetSize() > 0)
   {
      m_pbColour.SetColour(m_pMapProp->m_aRangeColour[m_pMapProp->m_aRangeColour.GetSize()-1].m_crFill);
   } else
   {
      if (m_pMapProp->m_bPolygon) m_pbColour.SetColour(m_pMapProp->m_crFill);
      else m_pbColour.SetColour(m_pMapProp->m_crLine);
      
   }

   // Initialise controls

   OnSelchangeMethod();

   // If default then close automatically

   if (m_bDefault) OnOK();
     	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

///////////////////////////////////////////////////////////////////////////////

void CDlgColourRange::OnOK() 
{
   // Determine the new intervals based on the parameters entered

   int index = m_cbMethod.GetCurSel();
   DWORD dw = m_cbMethod.GetItemData(index);

   COLORREF cr = m_pbColour.GetColour();

   // Retrieve rounding factor

   double dRound = 0;
   CString s;
   m_cbRound.GetLBText(m_cbRound.GetCurSel(), s);
   sscanf(s, "%lf", &dRound);

   // Retrieve intervals

   int nIntervals;
   if (m_eIntervals.GetValue(nIntervals, 1))
   {

      // Convert the current colour range to an array of values

      CArray <double, double> aRange; 
      if (dw == equalintervals)
      {
         EqualIntervals(aRange, nIntervals);
      }
      else if (dw == percentiles)
      {
         Percentiles(aRange, nIntervals);
      }
      else 
      {
         DefaultBreaks(aRange, nIntervals);
      }

        // Now copy values back

      m_pMapProp->m_aRangeColour.RemoveAll();

      // Add zero separately as white

      CColourRange rangecolor;
      if (aRange.GetSize() > 0 && aRange[0] == 0)
      {
         rangecolor.m_dMin = 0;
         rangecolor.m_dMax = 0;
         (CMapStyle&)rangecolor = *m_pMapProp;
         if (!m_pMapProp->m_bPolygon) rangecolor.m_crLine = RGB(255,255,255);
         else rangecolor.m_crFill = RGB(255,255,255);        

         m_pMapProp->m_aRangeColour.Add(rangecolor);   
      }        

      // Round the values      
      
      int i = 0; for (i = 0; i < aRange.GetSize()-1; i++)
      {                  
         if (dRound != 0) rangecolor.m_dMin = floor(aRange[i] / dRound)*dRound;    
         else rangecolor.m_dMin = aRange[i];

         if (i+1 < aRange.GetSize()-1)
         {
            if (dRound != 0) rangecolor.m_dMax = floor(aRange[i+1] / dRound)*dRound;     
            else rangecolor.m_dMax = aRange[i+1];
         } else
         {
            if (dRound != 0) rangecolor.m_dMax = ceil(aRange[i+1] / dRound)*dRound;     
            else rangecolor.m_dMax = aRange[i+1];
         }         

         (CMapStyle&)rangecolor = *m_pMapProp;

         if (!m_pMapProp->m_bPolygon) rangecolor.m_crLine = CViewMap::GetColour(i+1,0,nIntervals, cr);
         else rangecolor.m_crFill = CViewMap::GetColour(i+1,0,nIntervals, cr);        

         

         m_pMapProp->m_aRangeColour.Add(rangecolor);   
      }

      // Store rounding value

      m_pMapProp->m_aRangeColour.m_dRound = dRound;      

      CDialog::OnOK();
   };
}

///////////////////////////////////////////////////////////////////////////////

void CDlgColourRange::EqualIntervals(CArray <double, double>& aRange, int nIntervals)
{
   double dMin, dMax;
   CPageMapLayerProp::GetMinMax(m_pMapLayer, dMin, dMax);

   aRange.RemoveAll();

   aRange.Add(dMin);
   int i = 0; for (i = 0; i < nIntervals; i++)
   {
      aRange.Add(dMin + ((dMax-dMin) * (i+1)) / nIntervals);                             
   };
}

///////////////////////////////////////////////////////////////////////////////
//
// Calculate the breaks such that each interval has approximately the same 
// number of values

void CDlgColourRange::Percentiles(CArray <double, double>& aRange, int nIntervals)
{
   CArray <double, double> aData;

   // Retrieve the values into an array

   int i = 0; for (i = 0; i < m_pMapLayer->GetSize(); i++)
   {
     CMapLayerObj* m_pMapLayerObj = m_pMapLayer->GetAt(i);

     if (!IsNullDouble(m_pMapLayerObj->GetValue()))
     {
        aData.Add(m_pMapLayerObj->GetValue());
     }      
   }

   // Now sort the list

   qsort(aData.GetData(), aData.GetSize(), sizeof(double), &compare);

   // Now add the values

   aRange.Add(aData[0]);
   for (i = 1; i < nIntervals; i++)
   {
      aRange.Add(aData[(i*aData.GetSize())/nIntervals]);
   }   
   aRange.Add(aData[aData.GetSize()-1]);
}

///////////////////////////////////////////////////////////////////////////////

void CDlgColourRange::DefaultBreaks(CArray <double, double>& aRange, int& nIntervals)
{   
   double dMin, dMax, dSMin, dSMax;

   if (CPageMapLayerProp::GetMinMax(m_pMapLayer, dMin, dMax))
   {
      CDocMap::DetermineScale(dMin, dMax, &dSMin, &dSMax, &nIntervals);
   
      int i = 0; for (i = 0; i <= nIntervals; i++)
      { 
         aRange.Add(dSMin + ((dSMax-dSMin) * i) / nIntervals);
      };              
   } else
   {
      nIntervals = 1;
   }
}

///////////////////////////////////////////////////////////////////////////////

void CDlgColourRange::OnSelchangeMethod() 
{
	int index = m_cbMethod.GetCurSel();
   DWORD dw = m_cbMethod.GetItemData(index);

   m_eIntervals.EnableWindow(dw != defaultbreaks);
	
}
