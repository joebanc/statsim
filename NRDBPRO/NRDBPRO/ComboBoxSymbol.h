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

#ifndef _CCOMBOBOXSYMBOL_H_
#define _CCOMBOBOXSYMBOL_H_

#include "imagefile.h"
#include "maplayer.h"

/////////////////////////////////////////////////////////////////////////////

struct CCustomSymbol
{
   int m_nId;
   CImageFile m_imagefile;
};

struct CCustomSymArray : public CArray <CCustomSymbol, CCustomSymbol> 
{
   BOOL m_bInit;
};

/////////////////////////////////////////////////////////////////////////////

class CComboBoxSymbol : public CComboBox
{
// Construction
public:
	CComboBoxSymbol();

   friend class CDlgCustomSymbol;

// Operations
public:
   void Initialise(const CMapStyle&);   

   CMapStyle GetStyle(int index);

   static void DrawSymbol(CDC* pDC, double dX, double dY, const CMapStyle& mapstyle);   
   static CSize GetSymSize(CDC* pDC, double dSize);

   static void InitCustomSym();

   BOOL AddSymbol();
   BOOL DeleteSymbol();

   enum {none=0, triangle=1, square=2, circle=3, diamond=4, arrow=5, cross=6, inversetriangle=7, times=8, user=100};
   
protected:

   static DWORD m_anSymbols[];   
   static CCustomSymArray m_aCustomSym;

// Overrides

   virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
   virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComboBoxSymbol)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CComboBoxSymbol();

	// Generated message map functions
protected:
	//{{AFX_MSG(CComboBoxSymbol)
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif
