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
// Homepage: http://www.nrdb.co.uk/// 

#include "stdafx.h"
#include "nrdb.h"
#include "nrdbpro.h"
#include "BDHtmlView.h"
#include "docbdreport.h"
#include "doctsreport.h"
#include "mainfrm.h"
#include "fileobj.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

#define WM_SETMESSAGE WM_USER

/////////////////////////////////////////////////////////////////////////////
// CBDHtmlView

IMPLEMENT_DYNCREATE(CBDHtmlView, CHtmlView)

CBDHtmlView::CBDHtmlView()
{
	//{{AFX_DATA_INIT(CBDHtmlView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

   m_pDoc = NULL;
   m_bForward = FALSE;
   m_bBack = FALSE;
}

CBDHtmlView::~CBDHtmlView()
{
}

void CBDHtmlView::DoDataExchange(CDataExchange* pDX)
{
	CHtmlView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBDHtmlView)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBDHtmlView, CHtmlView)
	//{{AFX_MSG_MAP(CBDHtmlView)
	ON_COMMAND(ID_GOBACK, OnGoback)
	ON_COMMAND(ID_GOFORWARD, OnGoforward)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_UPDATE_COMMAND_UI(ID_GOBACK, OnUpdateGoback)
	ON_MESSAGE(WM_SETMESSAGE, OnSetMessage)   
	ON_UPDATE_COMMAND_UI(ID_GOFORWARD, OnUpdateGoforward)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBDHtmlView diagnostics

#ifdef _DEBUG
void CBDHtmlView::AssertValid() const
{
	CHtmlView::AssertValid();
}

void CBDHtmlView::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBDHtmlView message handlers

void CBDHtmlView::OnInitialUpdate() 
{
   CString s;
   CDocBDReport* m_pDoc = (CDocBDReport*)GetDocument();

  // Ensure that the file can be opened

   if (m_pDoc->IsKindOf(RUNTIME_CLASS(CDocBDReport)) 
      || m_pDoc->IsKindOf(RUNTIME_CLASS(CDocTSReport)))

   {
      FILE* pFile = fopen(m_pDoc->GetFileName(), "r");
      if (pFile != NULL)
      {
         fclose(pFile);

         COleVariant vtEmpty;
         Navigate(m_pDoc->GetFileName());         
      } else
      {
         s.LoadString(IDS_ERRORREAD);
         AfxMessageBox(s + CString(m_pDoc->GetFileName()));
      }  
  
      PostMessage(WM_SETMESSAGE);
   } else

   // If not attached to a CDocBDReport then open default html file
   {
      Navigate2(BDGetHtmlHelp());

      if (BDGetApp()->IsHtmlHelpTemp()) m_asTempFile.Add(BDGetHtmlHelp());      
   }
}

/////////////////////////////////////////////////////////////////////////////

void CBDHtmlView::OnGoback() 
{
	GoBack();
}

/////////////////////////////////////////////////////////////////////////////

void CBDHtmlView::OnUpdateGoback(CCmdUI* pCmdUI) 
{  
   pCmdUI->Enable(m_bBack);
}

/////////////////////////////////////////////////////////////////////////////

void CBDHtmlView::OnGoforward() 
{    
	GoForward();	
}

/////////////////////////////////////////////////////////////////////////////

void CBDHtmlView::OnUpdateGoforward(CCmdUI* pCmdUI) 
{     
   pCmdUI->Enable(m_bForward);
}

/////////////////////////////////////////////////////////////////////////////

void CBDHtmlView::OnEditCopy() 
{
   m_wndBrowser.SetFocus();
   ExecWB(OLECMDID_SELECTALL, OLECMDEXECOPT_DONTPROMPTUSER, NULL, NULL); 
   ExecWB(OLECMDID_COPY, OLECMDEXECOPT_DONTPROMPTUSER, NULL, NULL); 
}

/////////////////////////////////////////////////////////////////////////////

LRESULT CBDHtmlView::OnSetMessage(WPARAM, LPARAM)
{     
   CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
   m_pDoc = (CDocBDReport*)GetDocument();

   if (m_pDoc->IsKindOf(RUNTIME_CLASS(CDocBDReport)))
   {
      pMainFrame->GetMessageBar()->SetWindowText(m_pDoc->GetStatusText());   
   };

   return 0;
}

/////////////////////////////////////////////////////////////////////////////

void CBDHtmlView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{	
   PostMessage(WM_SETMESSAGE);   

   CHtmlView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

/////////////////////////////////////////////////////////////////////////////
//
// BUG: CHtmlView Does Not Repaint Correctly in an AppWizard-Generated 
// MDI Application

BOOL CBDHtmlView::PreCreateWindow(CREATESTRUCT& cs) 
{				
	return CHtmlView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
//
// Printing with customized header and footer
//
// http://support.microsoft.com/support/kb/articles/Q267/2/40.ASP
// 
void CBDHtmlView::OnFilePrint() 
{
	SAFEARRAYBOUND psabBounds[1];
	SAFEARRAY *psaHeadFoot;
	HRESULT hr = S_OK;         

	// Variables needed to send IStream header to print operation.
	HGLOBAL hG = 0;
	IStream *pStream= NULL;
	IUnknown *pUnk = NULL;
	ULONG lWrote = 0;
	LPSTR sMem = NULL;
	   
	// Initialize header and footer parameters to send to ExecWB().
	psabBounds[0].lLbound = 0;
	psabBounds[0].cElements = 3;
	psaHeadFoot = SafeArrayCreate(VT_VARIANT, 1, psabBounds);
	if (NULL == psaHeadFoot) {
		// Error handling goes here.
		goto cleanup;
	}
	VARIANT vHeadStr, vFootStr, vHeadTxtStream;
	long rgIndices;
	VariantInit(&vHeadStr);
	VariantInit(&vFootStr);
	VariantInit(&vHeadTxtStream);

	// Argument 1: Header
	vHeadStr.vt = VT_BSTR;
	vHeadStr.bstrVal = SysAllocString(L" ");
	if (vHeadStr.bstrVal == NULL) {
		goto cleanup;
	}
	
	// Argument 2: Footer	
	vFootStr.vt = VT_BSTR;
	vFootStr.bstrVal = SysAllocString(L" ");
	if (vFootStr.bstrVal == NULL) {
		ATLTRACE(_T("DoPrint: Could not allocate memory in %s: Line %d\n"), __FILE__, __LINE__);
		goto cleanup;
	}

	// Argument 3: IStream containing header text. Outlook and Outlook 
         // Express use this to print out the mail header. 	
	if ((sMem = (LPSTR)CoTaskMemAlloc(512)) == NULL) {
		ATLTRACE(_T("DoPrint: Could not allocate memory in %s: Line %d\n"), __FILE__, __LINE__);
		goto cleanup;
	}
	// We must pass in a full HTML file here, otherwise this 
         // gets turned into garbage corrupted when we print.
	sprintf(sMem, "<html><body> </body></html>\0");

	// Allocate an IStream for the LPSTR that we just created.
	hG = GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, strlen(sMem));
	if (hG == NULL) {
		ATLTRACE(_T("DoPrint: Could not allocate memory in %s: Line %d\n"), __FILE__, __LINE__);
		goto cleanup;
	}
	hr = CreateStreamOnHGlobal(hG, TRUE, &pStream);
	if (FAILED(hr)) {
		ATLTRACE(_T("OnPrint::Failed to create stream from HGlobal: %lX\n"), hr);
		goto cleanup;
	}
	hr = pStream->Write(sMem, strlen(sMem), &lWrote);
	if (SUCCEEDED(hr)) {
	    // Set the stream back to its starting position.
		LARGE_INTEGER pos;
		pos.QuadPart = 0;
		pStream->Seek((LARGE_INTEGER)pos, STREAM_SEEK_SET, NULL);
		hr = pStream->QueryInterface(IID_IUnknown, reinterpret_cast<void **>(&pUnk));
		vHeadTxtStream.vt = VT_UNKNOWN;
		vHeadTxtStream.punkVal = pUnk;
	}

	rgIndices = 0;
	SafeArrayPutElement(psaHeadFoot, &rgIndices, static_cast<void *>(&vHeadStr));
	rgIndices = 1;
	SafeArrayPutElement(psaHeadFoot, &rgIndices, static_cast<void *>(&vFootStr));
	rgIndices = 2;
	SafeArrayPutElement(psaHeadFoot, &rgIndices, static_cast<void *>(&vHeadTxtStream));
		
	// SAFEARRAY must be passed ByRef or else MSHTML transforms it into NULL.
	VARIANT vArg;
	VariantInit(&vArg);
	vArg.vt = VT_ARRAY | VT_BYREF;
	vArg.parray = psaHeadFoot;
	   

// get the HTMLDocument


	if (m_pBrowserApp != NULL)
	{
		LPOLECOMMANDTARGET lpTarget = NULL;
		LPDISPATCH lpDisp = GetHtmlDocument();

		if (lpDisp != NULL)
		{
			// the control will handle all printing UI

			if (SUCCEEDED(lpDisp->QueryInterface(IID_IOleCommandTarget,
					(LPVOID*) &lpTarget)))
			{
				lpTarget->Exec(NULL, OLECMDID_PRINT, 0, &vArg, NULL);
				lpTarget->Release();
			}
			lpDisp->Release();
		}
	}

	return; //WebBrowser control will clean up the SAFEARRAY after printing.

cleanup:
	VariantClear(&vHeadStr);
	VariantClear(&vFootStr);
	VariantClear(&vHeadTxtStream);
	if (psaHeadFoot) {
		SafeArrayDestroy(psaHeadFoot);
	}
	if (sMem) {
		CoTaskMemFree(sMem);	
	}
	if (hG != NULL) {
		GlobalFree(hG);
	}
	if (pStream != NULL) {
		pStream->Release();
		pStream = NULL;
	}
	return;

	
}

///////////////////////////////////////////////////////////////////////////////

void CBDHtmlView::OnClose() 
{	   
   
	CHtmlView::OnClose();
}

///////////////////////////////////////////////////////////////////////////////

void CBDHtmlView::OnDestroy() 
{
   // Delete temporary files

   int i = 0; for (i = 0; i < m_asTempFile.GetSize(); i++)
   {
      remove(m_asTempFile[i]);
   }

	CHtmlView::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

///////////////////////////////////////////////////////////////////////////////
//
// Go back to html document to ensure any linked files are closed with window

BOOL CBDHtmlView::PreCloseView()
{
   while (m_bBack)
   {
      GoBack();
   }
    
   return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

void CBDHtmlView::OnCommandStateChange(long nCommand, BOOL bEnable)
{
   if (nCommand == 1) m_bForward = bEnable;
   if (nCommand == 2) m_bBack = bEnable;   


}


void CBDHtmlView::OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags, LPCTSTR lpszTargetFrameName, CByteArray& baPostedData, LPCTSTR lpszHeaders, BOOL* pbCancel) 
{
   CString sURL = lpszURL;

   // Determine if a request has been made to open a document
   
   int index = sURL.Find('?');
   if (index != -1)
   {
      int i,j;
      if (sscanf(sURL.Mid(index+1),"queryresult=%i&attrid=%i",&i,&j) == 2)
      {
         // Open document as a temporary file

         CQueryResult *pQueryResult = m_pDoc->GetQueryResult();
         CQueryAttrArray* pAttrArray = pQueryResult->GetAt(i);

         CAttribute* pAttr = pAttrArray->GetAt(j);

         if (pAttr->GetDataType() == BDFILE)
         {
            CFileObj fileobj;
            if (fileobj.Initialise(*pAttr->GetLongBinary()))
            {
               // Save the file to the temporary directory

               char sTempDir[MAX_PATH];
               GetTempPath(sizeof(sTempDir), sTempDir);
               CString sFile = CString(sTempDir) + fileobj.GetFileName();

               FILE* pFile = fopen(sFile, "wb");
               if (pFile != NULL)
               {
                  fwrite(fileobj.GetData(), fileobj.GetSize(), 1, pFile);
                  fclose(pFile);
               }

               // Change the URL to point to this file

               sURL = "file://" + sFile;               
               int i = 0; for (i = 0; i < sURL.GetLength(); i++)
               {
                  if (sURL[i] == '\\') sURL.SetAt(i,'/');
               }

               // Add the file to a list of temporary files to delete

               m_asTempFile.Add(sFile);

               // Cancel this operation

               *pbCancel = TRUE;
               Navigate2(sURL);

            };
         }
      };
   }
	
	CHtmlView::OnBeforeNavigate2(lpszURL, nFlags,	lpszTargetFrameName, baPostedData, lpszHeaders, pbCancel);
}
