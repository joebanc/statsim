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
#include "DocMapLayout.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDocMapLayout

IMPLEMENT_DYNCREATE(CDocMapLayout, CDocument)

CDocMapLayout::CDocMapLayout()
{
   m_pMapLayout = NULL;
}

BOOL CDocMapLayout::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

   // Set title  
 
   SetTitle(BDGetApp()->GetLayout().GetDefault().m_sName);

	return TRUE;
}

CDocMapLayout::~CDocMapLayout()
{
}


BEGIN_MESSAGE_MAP(CDocMapLayout, CDocument)
	//{{AFX_MSG_MAP(CDocMapLayout)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDocMapLayout diagnostics

#ifdef _DEBUG
void CDocMapLayout::AssertValid() const
{
	CDocument::AssertValid();
}

void CDocMapLayout::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDocMapLayout serialization

void CDocMapLayout::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDocMapLayout commands

void CDocMapLayout::SetTitle(LPCTSTR lpszTitle) 
{
   CDocument::SetTitle(lpszTitle);
}

///////////////////////////////////////////////////////////////////////////////

void CDocMapLayout::OnUpdateFileSaveAs(CCmdUI* pCmdUI) 
{
   // No functionality for saving map layouts to file 

	pCmdUI->Enable(FALSE);
	
}
