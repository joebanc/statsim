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
#include "DocHtmlHelp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDocHtmlHelp

IMPLEMENT_DYNCREATE(CDocHtmlHelp, CDocument)

CDocHtmlHelp::CDocHtmlHelp()
{
}

BOOL CDocHtmlHelp::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CDocHtmlHelp::~CDocHtmlHelp()
{
}


BEGIN_MESSAGE_MAP(CDocHtmlHelp, CDocument)
	//{{AFX_MSG_MAP(CDocHtmlHelp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDocHtmlHelp diagnostics

#ifdef _DEBUG
void CDocHtmlHelp::AssertValid() const
{
	CDocument::AssertValid();
}

void CDocHtmlHelp::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDocHtmlHelp serialization

void CDocHtmlHelp::Serialize(CArchive& ar)
{
   char ch;

	if (ar.IsStoring())
	{      
      FILE* pFile = fopen(BDGetHtmlHelp(),"r");
      if (pFile != NULL)
      {         
         while ((ch = getc(pFile)) != EOF)
         {
            ar << ch;
         }
         fclose(pFile);
      };		
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDocHtmlHelp commands
