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

/////////////////////////////////////////////////////////////////////////////
// CComboBoxLines window

#ifndef _COMBOBOXLINES_H_
#define _COMBOBOXLINES_H_

class CComboBoxLines : public CComboBox
{
// Construction
public:
	CComboBoxLines();

// Operations
public:
   void Initialise(int nLineStyle, int nLineWidth);

   int GetStyle(int index) {return LOWORD(GetItemData(index));}
   int GetWidth(int index) {return HIWORD(GetItemData(index));}

      

protected:

   static DWORD m_anLines[];

// Overrides

   virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComboBoxLines)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CComboBoxLines();

	// Generated message map functions
protected:
	//{{AFX_MSG(CComboBoxLines)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif
