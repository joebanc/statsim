// StatSimList.cpp : implementation file
//

#include "stdafx.h"
#include "StatSimAPI.h"
#include "StatSimList.h"
#include "StatSimTree.h"
#include "StatSimWnd.h"


// CStatSimList

IMPLEMENT_DYNAMIC(CStatSimList, CListCtrl)
/////////////////////////////////////////////////////////////////////////////
// CStatSimList
BEGIN_MESSAGE_MAP(CStatSimList, CListCtrl)
	//{{AFX_MSG_MAP(CStatSimList)
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

CStatSimList::CStatSimList(BOOL wProgressBars, int PBCtrlID): 
							m_wProgressBars(wProgressBars), m_ProgressColumn(0), m_PercentColumn(1)
{
	nRows = 0; nCols = 0; //initialize but nCols is fixed
	
	m_PBCtrlID = PBCtrlID;
	
	InitmPtrs();

}

void CStatSimList::InitmPtrs()
{
	VERIFY(m_Font.CreateFont(
		14,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Arial")));                 // lpszFacename
	
	mItemArray = NULL;
	//sColArray = NULL;
	//pArrBldr = NULL;
	dwIDArray = NULL;

	m_ProgressBars = NULL;
	m_ProgressLabels = NULL;
	m_nPositions = NULL;
	m_sCaptions = NULL;

	// the source tree
	m_pDragSourceTree = 0;
	
	// the column names
	//sColArray = new CArray<LPCSTR, LPCSTR>;
	sColArray = new tstrvec1d;

}

CStatSimList::~CStatSimList()
{
	//return;
	//delete pArrBldr; pArrBldr = NULL;
	free(mItemArray);
	//free(sColArray);
	free(dwIDArray);

	if (m_wProgressBars && m_ProgressBars!=NULL) {
		DestroyProgressBars();	
	}

	delete [] m_nPositions; m_nPositions = 0;
	delete [] m_sCaptions; m_sCaptions = 0;

	if(sColArray){
		//delete sColArray; sColArray = 0;
	}

	if(_heapmin()<0)
		 AfxMessageBox(_T("Cannot give back memory to OS."));

}

void CStatSimList::SetImage(UINT bmpID, int cx, int nGrow, COLORREF colorRef, int nILT)
{

	pImageList = new CImageList;
	pImageList->Create(bmpID, cx, nGrow, colorRef); 
	SetImageList(pImageList, nILT);


}
void CStatSimList::CleanUp(LPCSTR* sCols, int nJ, 
						 BOOL enable, REFRESH_OPT refresh, 
						 int* widthArray, short l, short r)
{

	nCols = nJ;

	int i, j, nColItems;

	//sColArray = sCols; // pass the values
	(*sColArray).clear();
	//(*sColArray).RemoveAll();
	for (j=0; j<nJ; j++) {
		(*sColArray).push_back((sCols[j]));
	}

	switch (refresh) {

	case FRESH:	//as in no contents
		
		DeleteAllItems();
		
		nColItems = GetHeaderCtrl()->GetItemCount();
		
		for (i=0; i < nColItems; i++) {
			DeleteColumn(0);
		}
		delete mItemArray; mItemArray = 0;

		break;

	
	case EMPTY: //no contents but with column labels

		DeleteAllItems();

		nColItems = GetHeaderCtrl()->GetItemCount();
		
		for (i=0; i < nColItems; i++) {
			DeleteColumn(0);
		}
		
		for (j=0 + l; j < nJ - r; j++) {
			int width; 
			
			if (widthArray==NULL)
				width = 100; //my default width
			else
				width = widthArray[j];

			// this is still very erroneous since the introduction of the new feature vector::scolarray
			// some are still being adjusted
			CStatSimList::InsertColumn(j-l, sCols[j], LVCFMT_LEFT, width, -1);
		}
		
		//reinitialize data members
		nRows = 0;
		//free(mItemArray);
		//mItemArray = 0;
		delete mItemArray; mItemArray = 0;

		break;

	case NOACTION:  //leave as it is

		break;
	
	}	

	EnableWindow(enable);

}

void CStatSimList::InsertBulk(LPCSTR* sCols, int nJ, 
		LPCSTR** sItems, int nI, 
		BOOL enable, REFRESH_OPT refresh, 
		int* widthArray)

{
	CleanUp(sCols, nJ, enable, refresh, widthArray);

	int i, j;
	for (i=0; i < nI; i++) {
		
		if (sItemExists(sItems[i][0])) {
			CString msg;
			msg.Format( _T("%s already exists in the list."), sItems[i][0]);
			AfxMessageBox(msg);
		}

		else {
			
			InsertItem(i, (CString) sItems[i][0]);	//rows in column 1
			SetItemData(nRows + i, nRows + i);				//sets the index as the item data

			for (j=1; j < nJ; j++) {					
				
				SetItemText(i, j, (CString) sItems[i][j]);
				
			}
		}
	}

	RefreshItemArray();

}

DWORD* CStatSimList::GetIDArray() 
{

	dwIDArray = (DWORD*) malloc(nRows*sizeof(DWORD));

	for (int i=0; i<nRows; i++) {
		dwIDArray[i] = GetItemData(i);

		if (dwIDArray[i]<NULL)
			return NULL;
	}

	return dwIDArray;


}

BOOL CStatSimList::Exists(DWORD dwID, DWORD *dwConst, UINT nDW) 
{
	UINT i;
	
	for (i=0; i<nDW; i++) {
		if (dwConst[i] == dwID) {
			return TRUE;
		}
	}

	return FALSE;

}

void CStatSimList::InsertBulk(CStatSimRS *pRS, CStatSimWnd* pLabel, CStatSimBar* pProgBar, 
							  BOOL enable, REFRESH_OPT refresh, 
							  int* widthArray, bool dwFld, DWORD *dwConst, UINT nDW)

{
	CString prCaption;

	LPCSTR* sCols = pRS->GetFieldNames();
	UINT nJ = pRS->GetFieldCount();

	CleanUp(sCols, nJ, enable, refresh, widthArray, dwFld);

	UINT i, j;

	if (pProgBar!=NULL) {
		pProgBar->SetRange(0, 100);
	}

	long ct=pRS->GetRecordCount();

	if (ct>0) {
		pRS->MoveFirst();
		for (i=0; i < ct; i++) {
			
			prCaption.Format( _T("StatSim: Reading and fetching record %d of %d..."), 
				i+1, ct);
			m_sProgress = ConstChar(prCaption);		
			m_nPercent = ((float) (i+1)/(float) ct)*100;

			if (pLabel!=NULL) {
				if (pLabel->GetSafeHwnd()) pLabel->SetText(ConstChar(prCaption));
			}				
			
			if (pProgBar!=NULL) {
				if (pProgBar->GetSafeHwnd()) pProgBar->SetPos((int) m_nPercent);
			}

			LPCSTR sItem;
			DWORD dwDta;
			int nItems = 0;

			if ( (dwConst!=NULL) && (dwFld==TRUE) ) {
			
				dwDta = (DWORD) _ttol((pRS->SQLFldValue((USHORT) 0)));
				if ( !Exists(dwDta, dwConst, nDW) ) {
					nItems++;

					sItem = ConstChar(pRS->SQLFldValue((USHORT) 0 + dwFld));
					InsertItem(nItems-1, (CString) sItem);	//rows in column 1
					AfxMessageBox((CString) sItem);

					if ( !SetItemData(nItems-1, dwDta) ) {
						CString msg; msg.Format( _T("Can't set item data for %s."), sItem);
						AfxMessageBox(msg);
					}
					
					for (j=1; j < nJ - dwFld; j++) {	
						sItem = ConstChar(pRS->SQLFldValue((USHORT) j+dwFld));		
						SetItemText(nItems-1, j, (CString) sItem);
					}
				}
			}
			else {
				if (dwFld) {
					dwDta = (DWORD) _ttol((pRS->SQLFldValue((USHORT) 0)));
				}
				
				sItem = ConstChar(pRS->SQLFldValue((USHORT) 0 + dwFld));
				InsertItem(i, (CString) sItem);	//rows in column 1
				
				if ( dwFld )
					if ( !SetItemData(i, dwDta) ) {
						CString msg; msg.Format( _T("Can't set item data for %s."), sItem);
						AfxMessageBox(msg);
					}
				
				for (j=1; j < nJ - dwFld; j++) {	
					sItem = ConstChar(pRS->SQLFldValue((USHORT) j+dwFld));		
					SetItemText(i, j, (CString) sItem);
				}
			}
			
			pRS->MoveNext();
		}
	}

	RefreshItemArray();

}

BOOL CStatSimList::sItemExists(LPCSTR sItem)
{

	CString currStr, critStr;
	critStr.Format(_T("%s"), sItem);

	for (int i=0; i < nRows; i++) {	

		currStr = mItemArray[i][0];
	
		if (currStr == critStr) {
			return TRUE;
		}
	
	}

	return FALSE;



}

void CStatSimList::RefreshItemArray()
{

	nRows = GetItemCount();
	free(mItemArray);

	mItemArray = (LPCSTR**) malloc(nRows * sizeof(LPCSTR*));

	for (int i=0; i<nRows; i++) {
		
		mItemArray[i] = (LPCSTR*) malloc(nCols * sizeof(LPCSTR));
		
		for (int j=0; j<nCols; j++) {

			mItemArray[i][j] = ConstChar(GetItemText(i, j));

		}
	}

}

LPCSTR** CStatSimList::GetItemArray()
{

	return mItemArray;

}

int CStatSimList::GetnRows()
{
	return nRows;

}
int CStatSimList::GetSelectedItem()
{
	int nRowCount = GetItemCount();
	
	for (int i=0; i<nRowCount; i++) {
		if (GetItemState(i, LVIS_SELECTED)) {
			return i;
		}
	}

	return -1;

}

LPCSTR* CStatSimList::GetSelectedArray(LPCSTR critVal, int critRow, int critCol)
{	
	LPCSTR* itemArr;

	int nColumnCount = GetHeaderCtrl()->GetItemCount(), 
		nRowCount = GetItemCount(), 
		nSel = GetSelectedCount();

	if (nRowCount<=0) 
		return NULL;

	if (nSel!=1) {
		AfxMessageBox(_T("Function is for single selection only."));
		return NULL;
	}

	itemArr = (LPCSTR*) malloc(nColumnCount * sizeof(LPCSTR));
	
	for (int i=0; i<nRowCount; i++) {
		if (GetItemState(i, LVIS_SELECTED)) {
			for (int j=0;j < nColumnCount;j++) {
				itemArr[j] = ConstChar(GetItemText(i, j));
				//CString sVal, sItem;
				//sVal.Format( _T("%s"), critVal); sVal.TrimLeft(); sVal.TrimRight();
				//sItem.Format( _T("%s"), GetItemText(i, j));	sVal.TrimLeft(); sVal.TrimRight();
				//if ((critVal!=NULL && sVal==sItem) && critRow==i && critCol==j){
				//	return NULL;
				//}
			}
			return itemArr;
		}
	}

	AfxMessageBox(_T("Nothing selected."));

	return NULL;
					
}
void CStatSimList::SwitchContents(CStatSimList* pToList, BOOL bldArr)
{
	//if (sColArray = NULL) {
	//	AfxMessageBox(_T("Cannot proceed without column items!"));
	//	return;
	//}

	if ((*sColArray).size()== NULL) {
		AfxMessageBox(_T("Cannot proceed without column items!"));
		return;
	}

	CString strItem, strAttr;
	int nColumnCount, nRowCount, k, nSel;
	DWORD dwID = 0;

	if (pToList!=NULL) {

		nColumnCount = GetHeaderCtrl()->GetItemCount();	//column count from this list
		k = pToList->GetItemCount();

		if (nColumnCount > pToList->GetHeaderCtrl()->GetItemCount()) {
			AfxMessageBox(_T("Warning: There is a possible incompatibility between two lists"));
		}

		if (this == pToList) {
			AfxMessageBox(_T("Cannot transfer value to itself!"));
			return;
		}

	}

	nRowCount = GetItemCount();		
	nSel = 0;
			
	try	{		
		for (int i=0;i < nRowCount;i++)
		{	
			if (nSel == 0) {
				if (GetItemState(i, LVIS_SELECTED)) {				

					if (pToList!=NULL) {
						strItem = GetItemText(i, 0);
						dwID = GetItemData(i);
						pToList->InsertItem( k, (strItem) );
						pToList->SetItemData(k, dwID);
						for (int j=1;j < nColumnCount;j++) {					
							strAttr = GetItemText(i, j);
							pToList->SetItemText(k, j, (strAttr));
						}
					}
					
					DeleteItem(i);
					nSel++;
					k++;
				}
			}
			else {
				if (GetItemState((i-nSel), LVIS_SELECTED)) {				
					
					if (pToList!=NULL) {
						strItem = GetItemText((i-nSel), 0);
						dwID = GetItemData(i-nSel);
						pToList->InsertItem(k, (strItem));
						pToList->SetItemData(k, dwID);
						for (int j=1;j < nColumnCount;j++) {
							strAttr = GetItemText((i-nSel), j);
							pToList->SetItemText(k, j, (strAttr));
						}
					}
					
					DeleteItem((i-nSel));
					nSel++;
					k++;
				}
			}			
		}

		RefreshItemArray();
		
		if (pToList!=NULL) {
			pToList->RefreshItemArray();
		}

		
	} 
	
	catch(_com_error &e) 
	{
		_bstr_t bstrSource (e.Source());
		_bstr_t bstrDescription (e.Description());
		TRACE ( "Exception thrown for classes generated by #import" );
		TRACE ( "\tCode = %08lx\n", e.Error ());
		TRACE ( "\tCode meaning = %s\n", e.ErrorMessage ());
		TRACE ( "\tSource = %s\n", (LPCSTR) bstrSource);
		TRACE ( "\tDescription = %s\n", (LPCSTR) bstrDescription);
		
		AfxMessageBox ((LPCTSTR) bstrDescription);
	}
}

void CStatSimList::TransferContents(CStatSimList* pToList)
{

	if (this == pToList) {
		AfxMessageBox(_T("Cannot transfer value(s) to itself!"));
		return;
	}

	CStringArray strItem;
	int nColumnCount = pToList->GetHeaderCtrl()->GetItemCount();		
	int k = pToList->GetItemCount();

	strItem.SetSize(nColumnCount);
			
	try	{
		if (pToList!=NULL) {
			for (int i=0;i < GetItemCount();i++)
			{		
				strItem[0] = GetItemText(i, 0);
				pToList->InsertItem(k, (strItem[0]));
				
				for (int j=1;j < nColumnCount;j++) {					
					strItem[j] = GetItemText(i, j);
					pToList->SetItemText(k, j, (strItem[j]));
				}
				k++;
			}
		}
		
		DeleteAllItems();

		RefreshItemArray();
		
		if (pToList!=NULL) {
			pToList->RefreshItemArray();
		}
		
	}
	
	catch(_com_error &e) 
	{
		_bstr_t bstrSource (e.Source());
		_bstr_t bstrDescription (e.Description());
		TRACE ( "Exception thrown for classes generated by #import" );
		TRACE ( "\tCode = %08lx\n", e.Error ());
		TRACE ( "\tCode meaning = %s\n", e.ErrorMessage ());
		TRACE ( "\tSource = %s\n", (LPCSTR) bstrSource);
		TRACE ( "\tDescription = %s\n", (LPCSTR) bstrDescription);
		
		AfxMessageBox ((LPCTSTR) bstrDescription);
	}

}

void CStatSimList::OnPaint()
{
	// TODO: Add your message handler code here
	// Do not call CListCtrl::OnPaint() for painting messages
	
	// proceed making progress bars if specified
	if (!m_wProgressBars || !m_ProgressBars || !m_ProgressLabels) {	
		CListCtrl::OnPaint();
		return;
	}

	int Top=GetTopIndex();
	int Total=GetItemCount();
	int PerPage=GetCountPerPage();
	int LastItem=((Top+PerPage)>Total)?Total:Top+PerPage; //if Top+PerPage exceeds total, total, else Top+PerPage

	CHeaderCtrl* pHeader=GetHeaderCtrl();
	
	CRect pbRect, lbRect;
	pHeader->GetItemRect(m_ProgressColumn, &pbRect);
	pHeader->GetItemRect(m_ProgressColumn + 1, &lbRect);
	
	int pbWidth=pbRect.Width(),
		lbWidth=lbRect.Width();

	for(int i=0; i<nRows; i++)
	{
		m_nPositions[i] = m_ProgressBars[i]->GetPos();
		m_ProgressBars[i]->DestroyWindow();

		CString buff; m_ProgressLabels[i]->GetWindowText(buff);
		m_sCaptions[i] = ConstChar(buff);
		m_ProgressLabels[i]->DestroyWindow();

		if (i<Top || i>LastItem) {
			continue;
		}
	
		// get the rect
		CRect pbRT, lbRT;
		GetItemRect(i,&pbRT,LVIR_LABEL);
		lbRT = pbRT;
		
		pbRT.top+=1;
		pbRT.bottom-=1;
		pbRT.left+=pbRect.left;
		pbRT.right=pbRT.left+pbWidth-4;
		
		// Redraw
		m_ProgressBars[i]->Create(PBS_SMOOTH | WS_CHILD | WS_VISIBLE, pbRT, this, m_PBCtrlID+i);
		m_ProgressBars[i]->SetRange(0, 100);
		m_ProgressBars[i]->SetPos(m_nPositions[i]);
		m_ProgressBars[i]->ShowWindow(SW_SHOWNORMAL);

		lbRT.top+=1;
		lbRT.bottom-=1;
		lbRT.left+=lbRect.left;
		lbRT.right=lbRT.left+lbWidth-4;

		// Redraw
		m_ProgressLabels[i]->Create(NULL, WS_CHILD|WS_VISIBLE|SS_CENTER, lbRT, this);
		m_ProgressLabels[i]->SetFont(&m_Font);
		m_ProgressLabels[i]->ShowWindow(SW_SHOWNORMAL);
		m_ProgressLabels[i]->SetWindowText((CString) m_sCaptions[i]);

	}

	CListCtrl::OnPaint();
}

void CStatSimList::InitProgressColumn(int ColNum/*=0*/)
{
	m_ProgressColumn=ColNum;
}
void CStatSimList::InitPercentColumn(int ColNum/*=0*/)
{
	m_PercentColumn=ColNum;
}

CImageList* CStatSimList::SetImageListEx( HICON* pIcons, int nIcons, int nILT,
		int cx, int cy, UINT nFlags)
{
	//set the image list - assign as state images
	pImageList = new CImageList();
	pImageList->Create(cx, cy, ILC_COLOR32, 0, nIcons);
	for (int i=0; i<nIcons; i++) {
		pImageList->Add(pIcons[i]);
	}
	return SetImageList(pImageList, nILT);
}

void CStatSimList::CreateProgress(int Index, int SubIndex)
{
	//refresh nRows
	nRows = GetItemCount();
	// can only create progress for an existing item
	if (Index >= nRows)
		return;

	// can only create if progressbar array is already initiated
	if (!m_ProgressBars)
		m_ProgressBars = new CProgressCtrl*[nRows];
	
	m_ProgressBars[Index] = new CProgressCtrl();
	CRect ItemRect;
	GetSubItemRect(Index, SubIndex, LVIR_BOUNDS, ItemRect);
	
	int left = ItemRect.left;
	int top = ItemRect.top;
	int right = ItemRect.right;
	int bottom = ItemRect.bottom;
	
	m_ProgressBars[Index]->Create(PBS_SMOOTH | WS_CHILD | WS_VISIBLE, CRect(left, top, right, bottom), this, m_PBCtrlID + Index);
	m_ProgressBars[Index]->SetRange(0, 100);
	m_ProgressBars[Index]->SetPos(0);
	m_ProgressBars[Index]->ShowWindow(SW_SHOWNORMAL);

}
void CStatSimList::DestroyProgressBars()
{
	int nRows = GetItemCount();
	
	for (int i=0; i<nRows; i++) {
		m_ProgressBars[i]->DestroyWindow();
		m_ProgressLabels[i]->DestroyWindow();
		delete m_ProgressBars[i]; m_ProgressBars[i] = 0;
		delete m_ProgressLabels[i]; m_ProgressLabels[i] = 0;
	}

	delete [] m_ProgressBars; m_ProgressBars = 0;
	delete [] m_ProgressLabels; m_ProgressLabels = 0;
}
void CStatSimList::InitProgressBars()
{
	//refresh nRows
	nRows = GetItemCount();

	if (!m_wProgressBars) {
		AfxMessageBox(_T("Please initiate the list with progress bar property."));
		return;
	}

	if (!nRows) {
		AfxMessageBox(_T("Empty list."));
		return;
	}

	if (!m_ProgressBars) {
		m_ProgressBars = new CProgressCtrl*[nRows];
	}

	if (!m_ProgressLabels) {
		m_ProgressLabels = new CStatic*[nRows];
	}

	if (!m_nPositions) {
		m_nPositions = new int[nRows];
	}

	if (!m_sCaptions) {
		m_sCaptions = new LPCSTR[nRows];
	}

	CHeaderCtrl* pHeader=GetHeaderCtrl();
	
	CRect pbRect, lbRect;
	pHeader->GetItemRect(m_ProgressColumn, &pbRect);
	pHeader->GetItemRect(m_ProgressColumn + 1, &lbRect);
	
	int pbWidth=pbRect.Width(),
		lbWidth=lbRect.Width();

	for (int i=0; i<nRows; i++)
	{
		//initialize data
		m_sCaptions[i] = _MBCS("");
		m_nPositions[i] = 0;

		// get the rect
		CRect pbRT, lbRT;
		GetItemRect(i,&pbRT,LVIR_LABEL);
		lbRT = pbRT;
		
		pbRT.top+=1;
		pbRT.bottom-=1;
		pbRT.left+=pbRect.left;
		pbRT.right=pbRT.left+pbWidth-4;

		// create the progress control and set their position
		CreateProgress(i, m_ProgressColumn);

		lbRT.top+=1;
		lbRT.bottom-=1;
		lbRT.left+=lbRect.left;
		lbRT.right=lbRT.left+lbWidth-4;

		// create the label control and set their position
		m_ProgressLabels[i] = new CStatic();
		m_ProgressLabels[i]->Create(NULL, WS_CHILD|WS_VISIBLE|SS_CENTER, lbRT, this);
		m_ProgressLabels[i]->SetFont(&m_Font);
		m_ProgressLabels[i]->ShowWindow(SW_SHOW);

	}

}

// extended

void CStatSimList::ResizeProgress(int Index, int SubIndex)
{
	//refresh nRows
	nRows = GetItemCount();
	// can only create progress for an existing item
	if (Index >= nRows)
		return;
	
	if (!m_ProgressBars[Index])
		return;
	
	CRect ItemRect;
	GetSubItemRect(Index, SubIndex, LVIR_BOUNDS, ItemRect);
	
	int left = ItemRect.left;
	int top = ItemRect.top;
	int right = ItemRect.right;
	int bottom = ItemRect.bottom;
	
  	m_ProgressBars[Index]->MoveWindow(left, top, (right - left), (bottom - top));

}
void CStatSimList::ResizeProgress()
{
	//refresh nRows
	nRows = GetItemCount();
	for (int i=0; i<nRows; i++) {
		ResizeProgress(i, m_ProgressColumn);
	}

}
void CStatSimList::SetProgress(int Index, int prog)
{
	if (m_ProgressBars[Index])
		m_ProgressBars[Index]->SetPos(prog);

}
void CStatSimList::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	//ResizeProgress();
	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CStatSimList::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	//ResizeProgress();	
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CStatSimList::OnSize(UINT nType, int cx, int cy) 
{
	CListCtrl::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	//ResizeProgress();
}
void CStatSimList::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CListCtrl::OnWindowPosChanged(lpwndpos);
	
	// TODO: Add your message handler code here
	//ResizeProgress();
}
BOOL CStatSimList::SetCheckEx( int index, CheckState nCheck )
{
	if( index < 0 )
		return FALSE;

	if (nCheck == NOSTATE)
		return FALSE;

	return SetItemState( index, INDEXTOSTATEIMAGEMASK(nCheck),
					LVIS_STATEIMAGEMASK );

}

void CStatSimList::OnLButtonDown(UINT nFlags, CPoint point)
{
	UINT uFlags=0;
	int lviItem = HitTest(point, &uFlags),
		hParentItem;

	if( uFlags & LVHT_ONITEMSTATEICON )
	{	
		UINT nState = GetItemState( lviItem, LVIS_STATEIMAGEMASK ) >> 12;
		switch (nState)
		{
		case CHECKED:
			//uncheck the items
			SetCheckEx( lviItem, UNCHECKED );
			break;

		case UNCHECKED:
			//check the items
			SetCheckEx( lviItem, CHECKED );
			break;

		}
	}

	CListCtrl::OnLButtonDown(nFlags, point);
}

void CStatSimList::SetTreeCtrl(CStatSimTree *pTreeCtrl)
{

	m_pDragSourceTree = pTreeCtrl;

}



/*
void CStatSimList::InitProgressBars()
{
	int Top=GetTopIndex();
	int Total=GetItemCount();
	int PerPage=GetCountPerPage();
	int LastItem=((Top+PerPage)>Total)?Total:Top+PerPage; //if Top+PerPage exceeds total, total, else Top+PerPage

	// if the count in the list os nut zero delete all the progress controls and them procede
	{
		int Count=(int)m_ProgressBars.GetCount();
		for(int i=0;i<Count;i++)
		{
			CProgressCtrl* pControl=m_ProgressBars.GetAt(0);
			pControl->DestroyWindow();
			m_ProgressBars.RemoveAt(0);
		}
	}

	CHeaderCtrl* pHeader=GetHeaderCtrl();
	for(int i=Top;i<LastItem;i++)
	{
		CRect ColRt;
		pHeader->GetItemRect(m_ProgressColumn, &ColRt);
		// get the rect
		CRect rt;
		GetItemRect(i,&rt,LVIR_LABEL);
		rt.top+=1;
		rt.bottom-=1;
		rt.left+=ColRt.left;
		int Width=ColRt.Width();
		rt.right=rt.left+Width-4;
		
		//rt.left=ColRt.left+1;
		//rt.right=ColRt.right-1;

		// create the progress control and set their position
		CProgressCtrl* pControl=new CProgressCtrl();
		pControl->Create(NULL,rt,this,m_PBCtrlID+i);

		CString Data=GetItemText(i, m_PercentColumn);
		int Percent=_ttoi(Data);

		// set the position on the control
		pControl->SetPos(Percent);
		pControl->ShowWindow(SW_SHOWNORMAL);
		// add them to the list
		m_ProgressBars.Add(pControl);

	}

	nRows = GetItemCount();

}
*/


void CStatSimList::OnLButtonUp(UINT nFlags, CPoint point)
{
	UINT uFlags=0;
	//int lItem = HitTest(point, &uFlags);	
	int loc = OnNcHitTest(point);

	//if( lItem != -1 && (uFlags &  LVHT_ONITEM) )
	if( loc != HTNOWHERE )
	{
		AfxMessageBox(_T("Dragged over"));
		if (m_pDragSourceTree) {
			//AfxMessageBox(m_pDragSourceTree->GetItemText(m_pDragSourceTree->m_hitemDrag);
			AfxMessageBox(_T("on statsimlist"));

		}
	}


	CListCtrl::OnLButtonUp(nFlags, point);
}
void CStatSimList::RefreshColArray()
{
	(*sColArray).clear();

}

int CStatSimList::InsertColumn(int nCol, LPCSTR lpszColumnHeading, 
		int nFormat, int nWidth, int nSubItem)
{

	//std::vector<LPCSTR>::iterator it = (*sColArray).begin() + nCol;
	// insert the item
	//(*sColArray).insert(it, lpszColumnHeading);
	if (!(*sColArray).size())
		(*sColArray).push_back((lpszColumnHeading));
	else
		(*sColArray).insert((*sColArray).begin() + nCol, (lpszColumnHeading));


	return CListCtrl::InsertColumn(nCol, (CString) lpszColumnHeading, nFormat, nWidth, nSubItem);

}


LPCSTR CStatSimList::GetColString(int nCol)
{

	LPCSTR sCol;

	if ((*sColArray).size()<=NULL)
		sCol = NULL;
	else {
		CString sbuff((*sColArray).at(nCol));
		sCol = ConstChar(sbuff);
	}
	return sCol;

}


int CStatSimList::GetColumnCount()
{

	return (*sColArray).size();
}
