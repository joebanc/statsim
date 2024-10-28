// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// StatSimProDoc.cpp : implementation of the CStatSimProDoc class
//

//NOTE: Always call CDocument::SetModifiedFlag() after changing your document data. This will ensure that the framework prompts the user to save before shutdown. If you need more extensive control over the shutdown procedure, you can override CDocument::SaveModified(). 

#include "stdafx.h"
#include "StatSimPro.h"

#include "StatSimProDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CStatSimProDoc

IMPLEMENT_DYNCREATE(CStatSimProDoc, CDocument)

BEGIN_MESSAGE_MAP(CStatSimProDoc, CDocument)
END_MESSAGE_MAP()


// CStatSimProDoc construction/destruction

CStatSimProDoc::CStatSimProDoc()
{
	// TODO: add one-time construction code here

}

CStatSimProDoc::~CStatSimProDoc()
{
}

BOOL CStatSimProDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	//my serialization method
	//this->SetTitle(_T("Welcome"));

	// (SDI documents will reuse this document)

	return TRUE;
}




// CStatSimProDoc serialization

void CStatSimProDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar<<3;

		// TODO: add storing code here
	}
	else
	{
		ar<<30;
		// TODO: add loading code here
	}
}


// CStatSimProDoc diagnostics

#ifdef _DEBUG
void CStatSimProDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CStatSimProDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CStatSimProDoc commands
