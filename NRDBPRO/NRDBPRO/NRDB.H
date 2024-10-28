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

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#ifndef _NRDB_H_
#define _NRDB_H_

#include "resource.h"       // main symbols
#include "array.h"

/////////////////////////////////////////////////////////////////////////////

class CNRDB
{
public:
   enum {reportwizard, maplayer, mapquery, standardreport, reportquery, 
      summaryreport, histogram, histogramquery, timeseries, 
      timeseriesquery, piechart, piechartquery, tsreport, tsreportquery};
};

/////////////////////////////////////////////////////////////////////////////
// 
// Functions requiring BDGetApp() have been moved to this header file to 
// prevent application being rebuild everytime a change is made to an included
// header file e.g. CMapLayer
//

extern BDHANDLE BDHandle();
extern BDHANDLE& BDHandleRef();
extern void BDProgressRange(int a, int b);
extern BOOL BDProgressPos(int a);
extern void BDProgressText(LPCSTR s);
extern CString BDString(int nId);
extern class CProjection* BDProjection();
extern class CDocMap* BDGetDocMap();
extern void BDSetProgressBar(class CBDProgressBar* p);
extern class CBDProgressBar* BDGetProgressBar(); 
extern CBDMain& BDGetSettings();
extern LPCSTR BDGetDataSource();
extern void BDGetPrinterInfo(DEVMODE* pDevMode);
extern CMultiDocTemplate* BDGetDocTemplate(LPCSTR pDocName);
extern int BDOnHelpNrdbhomepage();
extern long BDFTypeSel();
extern void BDFTypeSel(long lFType);
extern void BDFTypeInit();
extern LPCSTR BDGetHtmlHelp();
extern void BDSetHtmlHelp(LPCSTR s, BOOL bTemp = FALSE);
extern int BDGetPreviousReport();
extern void BDSetPreviousReport(int nType);
extern void BDOnReportSummary();
extern CFeatureArray& BDFeatureSel();
extern CFTypeAttrArray& BDAttrSel();
extern BOOL BDIsAttrSel(long lFType, long lAttr);
extern BOOL BDIsFeatureSel(long lFType, long lFeature);
extern void BDSetProgressText(CStatic* pEdit);
extern CStatic* BDGetProgressText();
extern HWND& BDHwndDialog();
extern CToolTipCtrl*& BDToolTip();
extern CString BDGetAppPath();
extern void BDSetUserPassword(LPCSTR sUser, LPCSTR sPassword);
extern LPCSTR BDGetUser();
extern LPCSTR BDGetPassword();
extern LPCSTR BDGetHost();
extern BOOL BDIsMySQL(BDHANDLE);

typedef CArray <CFTypeAttr, CFTypeAttr> CFTypeAttrArray;

/////////////////////////////////////////////////////////////////////////////

#define NRDB_CHARSET ANSI_CHARSET

#endif
