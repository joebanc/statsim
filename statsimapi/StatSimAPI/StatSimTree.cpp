// StatSimTree.cpp : implementation file
//

#include "stdafx.h"
#include "StatSimAPI.h"
#include "StatSimTree.h"
#include "StatSimElt.h"
#include "StatSimWnd.h"

// CStatSimTree

IMPLEMENT_DYNAMIC(CStatSimTree, CTreeCtrl)
/////////////////////////////////////////////////////////////////////////////
// CStatSimTree

CStatSimTree::CStatSimTree(BOOL wDrag)
{
	m_wDrag = wDrag;

	phItemArray = 0;
	phParentItemArray = 0;
	hSizeArray = 0;

	pImageList = 0;
	pStateImage = 0;

	m_pDragTargetList = 0;
}

CStatSimTree::~CStatSimTree()
{
	for (int i=0; i<hLevels; i++) {
		delete [] phItemArray[i]; phItemArray[i] = 0;
		delete [] phParentItemArray[i]; phParentItemArray[i] = 0;
	}
	delete [] phItemArray; phItemArray = 0;
	delete [] phParentItemArray; phParentItemArray = 0;
	delete [] hSizeArray; hSizeArray = 0;

}


BEGIN_MESSAGE_MAP(CStatSimTree, CTreeCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, &CStatSimTree::OnTvnBegindrag)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_PAINT()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemExpanding)
	ON_WM_ERASEBKGND()
	ON_WM_QUERYNEWPALETTE()
	ON_WM_PALETTECHANGED()
	ON_NOTIFY_REFLECT(NM_CLICK, &CStatSimTree::OnNMClick)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatSimTree message handlers
void CStatSimTree::Initialize(CStatSimConn* pStatSimConn, LPTSTR rootlb, DWORD limit, 
		CStatSimWnd* pLabel, CStatSimBar* pProgBar)
{

	if (!this) {
		AfxMessageBox(_T("The tree is not yet created!"));
		return;
	}

	// pass the limit size to member limit
	nlimit = limit;
	// pass the database
	m_pStatSimConn = pStatSimConn;

	CStatSimRS *phRS, *phelemRS;
	
	ULONG *htSizeArray;
	CStringArray htTableArray, htLabelArray, htIDfldArray, 
		htItmDtaArr, htDigitArray;
	DWORD hIDno = 0, hparID = 0; 
	hLevels = 0;	//initialize levels

	hSizeArray = new ULONG[hLevels];
	htSizeArray = new ULONG[hLevels];

	hTableArray.SetSize(hLevels); htTableArray.SetSize(hLevels);
	hLabelArray.SetSize(hLevels); htLabelArray.SetSize(hLevels);
	hIDFldArray.SetSize(hLevels); htIDfldArray.SetSize(hLevels);
	hItmDtaArr.SetSize(hLevels); htItmDtaArr.SetSize(hLevels);
	hDigitArray.SetSize(hLevels); htDigitArray.SetSize(hLevels);

	while (hIDno != limit) {

		CString hSQL, hID, htable, hlabel, hIDFld, hItmDta, hdigit;
		phRS = new CStatSimRS( pStatSimConn );
		hSQL.Format( _T("SELECT * FROM `~hElement` WHERE parent=%d AND elementID<=%d"), hIDno, limit );

		phRS->RunSQL(hSQL);
		
		if (phRS->GetRecordCount() > 0) {
			phRS->MoveFirst();
			hLevels++;
			hID = phRS->SQLFldValue( _MBCS("elementID") );
			hparID = hIDno; hIDno = _ttol( ( hID ) );
			htable = phRS->SQLFldValue( _MBCS("element") );	//get table	
			hlabel = phRS->SQLFldValue( _MBCS("name") );
			hIDFld = phRS->SQLFldValue( _MBCS("IDField") );
			hItmDta = hID;
			
			//for digits of parent
			phRS->Close(); delete phRS; phRS = 0;
			phRS = new CStatSimRS( pStatSimConn );
			hSQL.Format( _T("SELECT * FROM `~hElement` WHERE `elementID`=%d"), hparID);
			
			phRS->RunSQL(hSQL);
			if (phRS->GetRecordCount() > 0) {
				phRS->MoveFirst();
				hdigit = phRS->SQLFldValue( _MBCS("digit") );
				
			}

			phelemRS = new CStatSimRS( pStatSimConn );	//handler for the current table
			hSQL.Format( _T("SELECT * FROM `%s`;"), htable);
			phelemRS->RunSQL(hSQL);

			//reassign pointer
			delete [] hSizeArray; hSizeArray = 0;
			hSizeArray = new ULONG[hLevels];

			//resize table array
			hTableArray.SetSize(hLevels);
			hLabelArray.SetSize(hLevels);
			hIDFldArray.SetSize(hLevels);
			hItmDtaArr.SetSize(hLevels);
			hDigitArray.SetSize(hLevels);
			
			ULONG i;

			//transfer values
			for(i=0; i<hLevels-1; i++) {
				hSizeArray[i] = htSizeArray[i];
				hTableArray[i] = htTableArray[i];
				hLabelArray[i] = htLabelArray[i];
				hIDFldArray[i] = htIDfldArray[i];
				hItmDtaArr[i] = htItmDtaArr[i];
				hDigitArray[i] = htDigitArray[i];
			}
			
			//reassign pointer for temp
			delete [] htSizeArray; htSizeArray = 0;
			htSizeArray = new ULONG[hLevels];

			//resize temp table array
			htTableArray.SetSize(hLevels);
			htLabelArray.SetSize(hLevels);
			htIDfldArray.SetSize(hLevels);
			htItmDtaArr.SetSize(hLevels);
			htDigitArray.SetSize(hLevels);
		
			//reassign values again - reversal
			for(i=0; i<hLevels-1; i++) {
				htSizeArray[i] = hSizeArray[i];
				htTableArray[i] = hTableArray[i];
				htLabelArray[i] = hLabelArray[i];
				htIDfldArray[i] = hIDFldArray[i];
				htItmDtaArr[i] = hItmDtaArr[i];
				htDigitArray[i] = hDigitArray[i];
			}
			
			//assign latest value
			htSizeArray[hLevels-1] = phelemRS->GetRecordCount();
			hSizeArray[hLevels-1] = htSizeArray[hLevels-1];

			htTableArray[hLevels-1] = htable;
			hTableArray[hLevels-1] = htTableArray[hLevels-1];

			htLabelArray[hLevels-1] = hlabel;
			hLabelArray[hLevels-1] = htLabelArray[hLevels-1];

			htIDfldArray[hLevels-1] = hIDFld;
			hIDFldArray[hLevels-1] = htIDfldArray[hLevels-1];

			htItmDtaArr[hLevels-1] = hItmDta;
			hItmDtaArr[hLevels-1] = htItmDtaArr[hLevels-1];

			htDigitArray[hLevels-1] = hdigit;
			hDigitArray[hLevels-1] = htDigitArray[hLevels-1];
		
		}
	}

	//delete the temporary
	delete [] htSizeArray; htSizeArray = 0;

	//initialize Item Array pointer and array of booleans
	phItemArray = new HTREEITEM*[hLevels];
	phParentItemArray = new HTREEITEM*[hLevels];

	for (UINT i=0; i<hLevels; i++) {
		if (i==0) {	//one country muna
			phItemArray[i] = new HTREEITEM[1];
			phParentItemArray[i] = new HTREEITEM[1];
		}
		else {
			phItemArray[i] = new HTREEITEM[hSizeArray[i]];
			phParentItemArray[i] = new HTREEITEM[hSizeArray[i]];
		}
	}

	if (phRS->IsOpen() && phRS!=NULL) {
		phRS->Close(); delete phRS; phRS = 0;
	}
	
	if (phelemRS->IsOpen() && phelemRS!=NULL) {
		phelemRS->Close(); delete phelemRS; phelemRS = 0;
	}


	Populate(pStatSimConn, rootlb, 9, pLabel, pProgBar);

}

UINT CStatSimTree::hItemIndex(HTREEITEM* phItem, CString critVal)
{
	return 0;
}

void CStatSimTree::PopulateChild(CStatSimConn* pStatSimConn, HTREEITEM hselItem, USHORT right)
{
	//HTREEITEM* hChildItem;	
	

	if (ItemHasChildren(hselItem)) {

		HTREEITEM hChildItem = GetChildItem(hselItem);
		HTREEITEM hNextItem;

		while (hChildItem != NULL) {
		
			ConferChild(pStatSimConn, hChildItem);
			
			hNextItem = GetNextItem(hChildItem, TVGN_NEXT);		//get next
			hChildItem = hNextItem;	//pass on to next child
		}
	}

	else {
		AfxMessageBox( _T("Item still has no children!") );
	}

}
void CStatSimTree::Populate(CStatSimConn* pStatSimConn, LPTSTR rootlb, USHORT right, 
						  CStatSimWnd* pLabel, CStatSimBar* pProgBar)
{
	//Watch out for case sensitivity

	CStatSimRS *phRS;
	CString hSQL, prCaption, prInitCaption;
	
	for (UINT j=0; j<hLevels; j++) {
		phRS = new CStatSimRS( pStatSimConn );
		hSQL.Format( _T("SELECT * FROM `%s`;"), hTableArray[j] );		
		phRS->RunSQL(hSQL);	

		if (j == 0) {	//one country muna
			TVINSERTSTRUCT tvInsert;
			tvInsert.hParent = NULL;
			tvInsert.hInsertAfter = NULL;
			tvInsert.item.mask = TVIF_TEXT;
			tvInsert.item.pszText = rootlb;
			HTREEITEM hroot = CTreeCtrl::InsertItem(&tvInsert);
			SetItemState( hroot, INDEXTOSTATEIMAGEMASK(1), TVIS_STATEIMAGEMASK );
			SetItemData(hroot, 1);

			phItemArray[0][0] = hroot;	//0 - 1st level; 0 - 1st country
			phParentItemArray[0][0] = NULL;	//0 - 1st level; 0 - 1st country

		}
		else {
			if (phRS->GetRecordCount() > 0) {
				phRS->MoveFirst();
			}
			CString lvlLabel, lvlID, lvlItem, lvlAttr, critVal;
			
			if (pProgBar!=NULL)
				pProgBar->SetRange(0, 100);
			
			prInitCaption.Format( _T("StatSim: Scanning %s:"), hLabelArray[j] );
		
			for (ULONG i=0; i<phRS->GetRecordCount(); i++) {			

				lvlLabel = phRS->SQLFldValue( ConstChar(hLabelArray[j]) );
				lvlID = phRS->SQLFldValue( ConstChar(hIDFldArray[j]) );
				lvlAttr = phRS->SQLFldValue(hTableArray[j] + "Path");
				lvlItem = lvlLabel + " - " + lvlID;
				
				critVal = lvlID.Left( _ttoi( ( hDigitArray[j] ) ) );

				HTREEITEM hparent = GetParent(i, j, critVal, _ttoi( ( hDigitArray[j] ) ) );

				phItemArray[j][i] = this->InsertItem(TVIF_TEXT,
					lvlItem, 0, 0, 0, 0, 0, hparent, NULL);
				SetItemState( phItemArray[j][i], INDEXTOSTATEIMAGEMASK(1), TVIS_STATEIMAGEMASK );
				phParentItemArray[j][i] = hparent;

				if (!this->SetItemData(phItemArray[j][i], _ttol( ( hItmDtaArr[j] ) ) ) ) {
					CString msg; msg.Format( _T("Cannot assign data to %s"), lvlID );
					AfxMessageBox(msg);
				}
				
				prCaption.Format( _T("%s Populating with %d of %d"), prInitCaption, 
					i+1, phRS->GetRecordCount() );
				m_sProgress = ConstChar(prCaption);		
				m_nPercent = ((float) (i+1)/(float) phRS->GetRecordCount())*100;
				
				if (pLabel!=NULL) {
					pLabel->SetText( ConstChar(prCaption) );
				}				
				
				if (pProgBar!=NULL) {
					if (pProgBar->GetSafeHwnd()) pProgBar->SetPos((int) m_nPercent);
				}

				phRS->MoveNext();

			}

		}

		delete phRS; phRS = 0;
	}

}
void CStatSimTree::Populate(CStatSimConn* pStatSimConn, HTREEITEM hselItem, DWORD limit, USHORT right)
{

	HTREEITEM hrunItem = hselItem;	//initialize
	DWORD hIDno;

	hIDno = GetItemData(hselItem);	//get id
	
	ConferChild(pStatSimConn, hrunItem);
	PopulateChild(pStatSimConn, hrunItem);

}

HTREEITEM* CStatSimTree::ConferChild(CStatSimConn* pStatSimConn, HTREEITEM hselItem, USHORT right)
{

	
	CStatSimRS *phRS, *ptreeRS;

	UINT hChildCount = 0;
	DWORD hIDno; 
	CString strID, strItem, critFld, critVal, strAttr, 
		htable, hSQL, hlabel, hID, hIDFld, hdigit;

	if (ItemHasChildren(hselItem)) {
		ClearChild(hselItem);	//delete children if there are any
	}

	phRS = new CStatSimRS( pStatSimConn );
	ptreeRS = new CStatSimRS( pStatSimConn );

	strItem = GetItemText(hselItem);	//get text
	strID = strItem.Right(right);

	hIDno = GetItemData(hselItem);	//get id

	//for characteristics of current
	hSQL.Format( _T("SELECT * FROM `~hElement` WHERE parent=%d"), hIDno );
	phRS->RunSQL(hSQL);
	
	if (phRS->GetRecordCount() > 0) {
		phRS->MoveFirst();
		htable = phRS->SQLFldValue( _MBCS("element") );
		hlabel = phRS->SQLFldValue( _MBCS("name") );
		hID = phRS->SQLFldValue( _MBCS("elementID") );
		hIDFld = phRS->SQLFldValue( _MBCS("IDField") );
	}
	
	//for digits of parent
	phRS->Close(); delete phRS; phRS = 0;
	phRS = new CStatSimRS( pStatSimConn );
	hSQL.Format( _T("SELECT * FROM `~hElement` WHERE `elementID`=%d"), hIDno );

	phRS->RunSQL(hSQL);
	if (phRS->GetRecordCount() > 0) {
		phRS->MoveFirst();
		hdigit = phRS->SQLFldValue( _MBCS("digit") );
		
	}
	
	int intdigit = _ttoi( ( hdigit ) );
	if ( intdigit > 0 ) {
		critFld.Format( _T("Left(%s, %s)"), hIDFld, hdigit);
		critVal.Format( _T("%d"), _ttol( ( strID.Left(intdigit) ) ) );
	}
	else {
		critFld = "country";
		critVal = "608";
	}
	
	strAttr.Format( _T("%sPath"), htable );

	HTREEITEM* hChildItem;	
	
	hChildItem = ptreeRS->GetHierRS(htable,  critFld, critVal, 
		hlabel, hIDFld, strAttr, _ttol( (hID) ),
		this, hselItem);
	
	phRS->Close(); delete phRS; phRS = 0;
	ptreeRS->Close(); delete ptreeRS; ptreeRS = 0;

	return hChildItem;

}

void CStatSimTree::ClearChild(HTREEITEM hselItem)
{
	if (ItemHasChildren(hselItem)) {

		HTREEITEM hNextItem;
		HTREEITEM hChildItem = GetChildItem(hselItem);
		UINT hChildCount = 0;
		
		while (hChildItem != NULL) {

			hChildCount++;

			hNextItem = GetNextItem(hChildItem, TVGN_NEXT);		//get next

			if (GetItemData(hChildItem) != 0)
			{
				DeleteItem(hChildItem);
				
			}
			
			hChildItem = hNextItem;	//pass on to next child
		}
		
	}
}

CString CStatSimTree::hTable(UINT i) {
	
	return hTableArray[i];

}

CString CStatSimTree::hLabel(UINT i) {
	
	return hLabelArray[i];

}

CString CStatSimTree::hIDFld(UINT i) {
	
	return hIDFldArray[i];

}

CString CStatSimTree::hDigit(UINT i) {
	
	return hDigitArray[i];

}

CString CStatSimTree::hItmDta(UINT i) {
	
	return hItmDtaArr[i];

}

UINT CStatSimTree::hIndex(DWORD itmDta) {

	UINT index = 0;
	for (UINT k=0; k<hLevels; k++) {
		if ( (ULONG) _ttol( ( hItmDtaArr[k] ) ) == itmDta ) 
			index = k;
	}

	return index;
	
}

HTREEITEM CStatSimTree::GetParent(ULONG i, ULONG j,
								CString critVal, USHORT left, USHORT right)
{

	CString strCrit;
	BOOL wparent = FALSE;
	HTREEITEM hparent;
	ULONG k;

	for (k=0; k<hSizeArray[j-1]; k++) {
		
		strCrit = GetItemText(phItemArray[j-1][k]);
		strCrit.TrimLeft();strCrit.TrimRight();
		strCrit = strCrit.Right(right);
		strCrit = strCrit.Left(left);

		if (j==1) {
			hparent = this->GetRootItem();
			wparent = TRUE;
		}

		else {
			if (critVal==strCrit) {
				hparent = phItemArray[j-1][k];
				wparent = TRUE;
			}

		}
	
	}

	if (!wparent) {	//revise criteria
		for (k=0; k<hSizeArray[j-1]; k++) {
			strCrit = this->GetItemText(phItemArray[j-2][k]);
			strCrit.TrimLeft();strCrit.TrimRight();
			strCrit = strCrit.Right(right);

			int intdigit = _ttoi( ( hDigitArray[j-1] ) );
			strCrit = strCrit.Left(intdigit);
			critVal = critVal.Left(intdigit);
			
			if (critVal==strCrit) {
				hparent = phItemArray[j-2][k];
				wparent = TRUE;
			}
		}
	}

	if(wparent)
		return hparent;
	else 
		return this->GetRootItem();
	

}

HTREEITEM** CStatSimTree::GetItemArray()
{
	return phItemArray;
}
HTREEITEM** CStatSimTree::GetParentItemArray()
{
	return phParentItemArray;
}

ULONG* CStatSimTree::GetSizeArray()
{
	return hSizeArray;
}
UINT CStatSimTree::nLevels()
{
	return hLevels;
}

void CStatSimTree::SetParentChecks(HTREEITEM hselItem)
{

	HTREEITEM hParentItem = GetParentItem(hselItem);
	if ( hParentItem!=NULL ) {
		if (GetItemData(hParentItem) != 0) {

			SetCheckEx( hParentItem, ChildrenCheckStatus(hParentItem) );
			// set check of further children
			if (GetParentItem(hParentItem)!=NULL) {
				SetParentChecks( hParentItem );
			}
		}	
		
		else {
			AfxMessageBox(_T("The tree has null item data!"));
		}
	}
}


void CStatSimTree::SetChildChecks(HTREEITEM hselItem, CheckState nCheck)
{
	if (ItemHasChildren(hselItem)) {

		HTREEITEM hNextItem;
		HTREEITEM hChildItem = GetChildItem(hselItem);
		UINT hChildCount = 0;
		
		while (hChildItem != NULL) {

			hChildCount++;

			hNextItem = GetNextItem(hChildItem, TVGN_NEXT);		//get next

			if (GetItemData(hChildItem) != 0)
			{
				SetCheckEx(hChildItem, nCheck);
				// set check of further children
				if (GetChildItem(hChildItem)!=NULL) {
					SetChildChecks(hChildItem, nCheck);
				}
			}
			
			else {
				AfxMessageBox(_T("The tree has null item data!"));
			}
			
			hChildItem = hNextItem;	//pass on to next child
		}
		
	}
}

BOOL CStatSimTree::ChildrenAllChecked(HTREEITEM hselItem, CheckState nCheck)
{
	BOOL IsTrue = -1;

	if (ItemHasChildren(hselItem)) {

		IsTrue = TRUE;

		HTREEITEM hNextItem;
		HTREEITEM hChildItem = GetChildItem(hselItem);
		UINT hChildCount = 0;
		
		while (hChildItem != NULL) {

			hChildCount++;

			hNextItem = GetNextItem(hChildItem, TVGN_NEXT);		//get next

			if (GetItemData(hChildItem) != 0)
			{
				UINT nStatus = GetItemState( hChildItem, TVIS_STATEIMAGEMASK ) >> 12;							
				// get check status of children, if not the same return false
				if ( nStatus != nCheck ) {
					IsTrue = FALSE;
				}

				// get check status of further children
				if (GetChildItem(hChildItem)!=NULL) {
					if ( !ChildrenAllChecked( GetChildItem(hChildItem), nCheck ) ) {
						IsTrue = FALSE;
					}
				}

			}
			
			else {
				AfxMessageBox(_T("The tree has null item data!"));
				return -1;
			}
			
			hChildItem = hNextItem;	//pass on to next child
		}
		
	}

	return IsTrue;
}

CStatSimTree::CheckState CStatSimTree::ChildrenCheckStatus(HTREEITEM hselItem)
{
	BOOL AllChecked, AllUnChecked;

	// all children were checked
	AllChecked = ChildrenAllChecked(hselItem);
	// all children were unchecked
	AllUnChecked = ChildrenAllChecked(hselItem, UNCHECKED);

	if ( AllChecked == -1 || AllUnChecked == -1 ) {
		AfxMessageBox( _T("Cannot retrieve check status.") );
		return NOSTATE;
	}

	if ( AllChecked && !AllUnChecked ) {
		return CHECKED;
	}
	else if ( AllUnChecked && !AllChecked ) {
		return UNCHECKED;
	}
	else {
		return PARTIALLY;
	}


}


BOOL CStatSimTree::SetCheckEx( HTREEITEM hItem, CheckState nCheck )
{
	if( hItem == NULL )
		return FALSE;

	if (nCheck == NOSTATE)
		return FALSE;

	return SetItemState( hItem, INDEXTOSTATEIMAGEMASK(nCheck),
					TVIS_STATEIMAGEMASK );

}

CImageList* CStatSimTree::SetImageListEx( HICON* pIcons, int nIcons, int nILT,
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
void CStatSimTree::SetListCtrl(CStatSimList *pListCtrl)
{

	m_pDragTargetList = pListCtrl;

}
void CStatSimTree::OnRButtonDown(UINT nFlags, CPoint point)
{

	UINT uFlags=0;
	HTREEITEM hItem = HitTest(point, &uFlags);

	if ((hItem != NULL) && (uFlags & TVHT_ONITEM))
	{
		Select(hItem, TVGN_CARET);
	}


	CTreeCtrl::OnRButtonDown(nFlags, point);
}



void CStatSimTree::OnLButtonDown(UINT nFlags, CPoint point)
{

	UINT uFlags=0;
	HTREEITEM hItem = HitTest(point, &uFlags),
		hParentItem;	

	if( hItem != NULL && (uFlags & TVHT_ONITEMSTATEICON) )
	{
		
		UINT nState = GetItemState( hItem, TVIS_STATEIMAGEMASK ) >> 12;
		switch (nState)
		{
		case CHECKED:
			//uncheck the items
			SetCheckEx( hItem, UNCHECKED );
			//set child check state
			SetChildChecks( hItem, UNCHECKED );
			break;

		case UNCHECKED:
			//check the items
			SetCheckEx( hItem, CHECKED );
			//set child check state
			SetChildChecks( hItem, CHECKED );
			break;
		case PARTIALLY:
			//check the items
			SetCheckEx( hItem, CHECKED );
			//set child check state
			SetChildChecks( hItem, CHECKED );
			break;
		}
		
		//update the parent
		hParentItem = GetParentItem(hItem);
		SetParentChecks( hItem );

		//hilight the checked 
		Select(hItem, TVGN_CARET);

		//expand
		Expand(hItem, TVE_EXPAND);

		return;
	}
	
	else if ((hItem != NULL) && (uFlags & TVHT_ONITEM))
	{
		Select(hItem, TVGN_CARET);
	}


	CTreeCtrl::OnLButtonDown(nFlags, point);
}


void CStatSimTree::OnTvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	if (!m_wDrag)
		return;

	m_hitemDrag = pNMTreeView->itemNew.hItem;
	m_hitemDrop = NULL;

	m_pDragImage = CreateDragImage(m_hitemDrag);  // get the image list for dragging
	// CreateDragImage() returns NULL if no image list
	// associated with the tree view control
	if( !m_pDragImage )
		return;

	Select(m_hitemDrag, TVGN_DROPHILITE);

	m_bLDragging = TRUE;
	m_pDragImage->BeginDrag(0, CPoint(-15,-15));
	POINT pt = pNMTreeView->ptDrag;
	ClientToScreen( &pt );
	m_pDragImage->DragEnter(NULL, pt);
	SetCapture();

}

void CStatSimTree::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_wDrag) {
		CTreeCtrl::OnMouseMove(nFlags, point);
		return;
	}

	HTREEITEM	hitem;
	UINT		flags;

	if (m_bLDragging)
	{
		POINT pt = point;
		ClientToScreen( &pt );
		CImageList::DragMove(pt);
		if ((hitem = HitTest(point, &flags)) != NULL)
		{
			CImageList::DragShowNolock(FALSE);
			// don't highlight tree items if bounded to a target list
			if (!m_pDragTargetList)
				SelectDropTarget(hitem);
			m_hitemDrop = hitem;
			CImageList::DragShowNolock(TRUE);
		}
	}

	CTreeCtrl::OnMouseMove(nFlags, point);
}

void CStatSimTree::OnLButtonUp(UINT nFlags, CPoint point)
{
	CTreeCtrl::OnLButtonUp(nFlags, point);
	if (!m_wDrag) {
		return;
	}

	if (m_bLDragging)
	{
		m_bLDragging = FALSE;
		CImageList::DragLeave(this);
		CImageList::EndDrag();
		ReleaseCapture();

		delete m_pDragImage;

		// Remove drop target highlighting
		if (!m_pDragTargetList)
			SelectDropTarget(NULL); // don't highlight tree items if bounded to a target list

		// if drag and drop item are the same, return
		if( m_hitemDrag == m_hitemDrop )
			return;

		// If Drag item is an ancestor of Drop item then return
		HTREEITEM htiParent = m_hitemDrop;
		while( (htiParent = GetParentItem( htiParent )) != NULL )
		{
			if( htiParent == m_hitemDrag ) return;
		}

		m_DropOffPoint = point;
		ClientToScreen(&m_DropOffPoint);
		m_DropOffWnd = WindowFromPoint(m_DropOffPoint);
		
		if ( m_DropOffWnd->IsKindOf(RUNTIME_CLASS(CListCtrl)) )
        {
			if (m_pDragTargetList) {
				m_pDragTargetList->InsertColumn( 0, ConstChar(GetItemText(m_hitemDrag)) );
				m_pDragTargetList->SetColumnWidth(0, 100);
				return;
			}

        }
		else if (m_DropOffWnd->IsKindOf(RUNTIME_CLASS(CHeaderCtrl)) )
        {
            AfxMessageBox(_T("target is a Header!"), MB_OK);
			return;
        }

		if (m_pDragTargetList)
			return;

		Expand( m_hitemDrop, TVE_EXPAND ) ;

		HTREEITEM htiNew = MoveChildItem( m_hitemDrag, m_hitemDrop, TVI_LAST );
		DeleteItem(m_hitemDrag);
		SelectItem( htiNew );

	}
}
HTREEITEM CStatSimTree::MoveChildItem(HTREEITEM hItem, HTREEITEM htiNewParent, HTREEITEM htiAfter)
{

	TV_INSERTSTRUCT tvstruct;
	HTREEITEM hNewItem;
    CString sText;

    // get information of the source item
    tvstruct.item.hItem = hItem;
    tvstruct.item.mask = TVIF_CHILDREN | TVIF_HANDLE |     TVIF_IMAGE | TVIF_SELECTEDIMAGE;
    GetItem(&tvstruct.item);  
    sText = GetItemText( hItem );
        
    tvstruct.item.cchTextMax = sText.GetLength();
    tvstruct.item.pszText = sText.LockBuffer();

    //insert the item at proper location
    tvstruct.hParent = htiNewParent;
    tvstruct.hInsertAfter = htiAfter;
    tvstruct.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
    hNewItem = InsertItem(&tvstruct);
    sText.ReleaseBuffer();

    //now copy item data and item state.
    SetItemData(hNewItem,GetItemData(hItem));
    SetItemState(hNewItem,GetItemState(hItem,TVIS_STATEIMAGEMASK),TVIS_STATEIMAGEMASK);

	//now delete the old item
	DeleteItem(hItem);

    return hNewItem;
}



void CStatSimTree::OnPaint()
{
	CPaintDC dc(this); // device context for painting
		// Remove comments from next five lines if you don't need any 
	// specialization beyond adding a background image
//	if( m_bitmap.m_hObject == NULL )
//	{
//		CTreeCtrl::OnPaint();
//		return;
//	}

	CRect rcClip, rcClient;
	dc.GetClipBox( &rcClip );
	GetClientRect(&rcClient);

	// Create a compatible memory DC 
	CDC memDC;
	memDC.CreateCompatibleDC( &dc );
	
	// Select a compatible bitmap into the memory DC
	CBitmap bitmap, bmpImage;
	bitmap.CreateCompatibleBitmap( &dc, rcClient.Width(), rcClient.Height() );
	memDC.SelectObject( &bitmap );

	
	// First let the control do its default drawing.
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)memDC.m_hDC, 0 );

	// Draw bitmap in the background if one has been set
	if( m_bitmap.m_hObject != NULL )
	{
		// Now create a mask
		CDC maskDC;
		maskDC.CreateCompatibleDC(&dc);
		CBitmap maskBitmap;

		// Create monochrome bitmap for the mask
		maskBitmap.CreateBitmap( rcClient.Width(), rcClient.Height(), 
						1, 1, NULL );
		maskDC.SelectObject( &maskBitmap );
		memDC.SetBkColor(RGB(255,255,255) /*::GetSysColor( COLOR_WINDOW )*/ );

		// Create the mask from the memory DC
		maskDC.BitBlt( 0, 0, rcClient.Width(), rcClient.Height(), &memDC, 
					rcClient.left, rcClient.top, SRCCOPY );

		
		CDC tempDC;
		tempDC.CreateCompatibleDC(&dc);
		tempDC.SelectObject( &m_bitmap );

		CDC imageDC;
		CBitmap bmpImage;
		imageDC.CreateCompatibleDC( &dc );
		bmpImage.CreateCompatibleBitmap( &dc, rcClient.Width(), 
						rcClient.Height() );
		imageDC.SelectObject( &bmpImage );

		if( dc.GetDeviceCaps(RASTERCAPS) & RC_PALETTE && m_pal.m_hObject != NULL )
		{
			dc.SelectPalette( &m_pal, FALSE );
			dc.RealizePalette();

			imageDC.SelectPalette( &m_pal, FALSE );
		}

		// Get x and y offset
		CRect rcRoot;
		GetItemRect( GetRootItem(), rcRoot, FALSE );
		rcRoot.left = -GetScrollPos( SB_HORZ );

	
		if (isImageTiled)
		{
			// Draw bitmap in tiled manner to imageDC
			for( int i = rcRoot.left; i < rcClient.right; i += m_cxBitmap )
			for( int j = rcRoot.top; j < rcClient.bottom; j += m_cyBitmap )
				imageDC.BitBlt( i, j, m_cxBitmap, m_cyBitmap, &tempDC, 
							0, 0, SRCCOPY );
		}else
		{
			int x=0,y=0 ;
			(m_cxBitmap > rcClient.right) ? x=0:x=(rcClient.right - m_cxBitmap);
			(m_cyBitmap > rcClient.bottom)? y=0:y=(rcClient.bottom - m_cyBitmap);
			imageDC.BitBlt( x, y, m_cxBitmap, m_cyBitmap, &tempDC, 
							0, 0, SRCCOPY );
		}

		// Set the background in memDC to black. Using SRCPAINT with black and any other
		// color results in the other color, thus making black the transparent color
		memDC.SetBkColor( RGB(0,0,0)/*memDC_bgColor_bitmap*/);        
		memDC.SetTextColor(RGB(255,255,255));
		memDC.BitBlt(rcClip.left, rcClip.top, rcClip.Width(), rcClip.Height(), &maskDC, 
				rcClip.left, rcClip.top, SRCAND);

		// Set the foreground to black. See comment above.
		imageDC.SetBkColor(RGB(255,255,255));
		imageDC.SetTextColor(RGB(0,0,0));
		imageDC.BitBlt(rcClip.left, rcClip.top, rcClip.Width(), rcClip.Height(), &maskDC, 
				rcClip.left, rcClip.top, SRCAND);

		// Combine the foreground with the background
		imageDC.BitBlt(rcClip.left, rcClip.top, rcClip.Width(), rcClip.Height(), 
					&memDC, rcClip.left, rcClip.top,SRCPAINT);

		//*****************************
			/*	for( int yy = 0; yy < rcClient.Height(); yy++)
				for( int xx = 0; xx < rcClient.Width(); xx++ )
			{
				if (imageDC.GetPixel(CPoint(xx,yy)) == RGB(0,0,0))
					imageDC.SetPixel(CPoint(xx,yy),RGB(255,255,255));
			}
			 Create a compatible memory DC 48068
			CDC whiteDC;
			whiteDC.CreateCompatibleDC( &dc );
	
			// Select a compatible bitmap into the memory DC
				CBitmap cBmp;
			blankBmp.CreateCompatibleBitmap( &dc, rcClient.Width(), rcClient.Height() );
			whiteDC.SelectObject( &blankBmp );*/
		//*****************************

		// Draw the final image to the screen		
		dc.BitBlt( rcClip.left, rcClip.top, rcClip.Width(), rcClip.Height(), 
					&imageDC, rcClip.left, rcClip.top, SRCCOPY );
	}
	else
	{
		dc.BitBlt( rcClip.left, rcClip.top, rcClip.Width(), 
				rcClip.Height(), &memDC, 
				rcClip.left, rcClip.top, SRCCOPY );
	}

}

void CStatSimTree::OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	if( m_bitmap.m_hObject != NULL ) InvalidateRect(NULL);
	
	*pResult = 0;

}

BOOL CStatSimTree::OnEraseBkgnd(CDC* pDC) 
{
	if( m_bitmap.m_hObject != NULL )	return TRUE;

	return CTreeCtrl::OnEraseBkgnd(pDC);

}
BOOL CStatSimTree::OnQueryNewPalette() 
{

	CClientDC dc(this);
	if( dc.GetDeviceCaps(RASTERCAPS) & RC_PALETTE && m_pal.m_hObject != NULL )
	{
		dc.SelectPalette( &m_pal, FALSE );
		BOOL result = dc.RealizePalette();
		if( result )
			Invalidate();
		return result;
	}
	
	return CTreeCtrl::OnQueryNewPalette();
}
void CStatSimTree::OnPaletteChanged(CWnd* pFocusWnd) 
{
	CTreeCtrl::OnPaletteChanged(pFocusWnd);
	
	if( pFocusWnd == this )
		return;

	OnQueryNewPalette();

}

BOOL CStatSimTree::SetBkImage(UINT nIDResource)
{
	return SetBkImage( (LPCSTR)nIDResource );
}

BOOL CStatSimTree::SetBkImage(LPCSTR lpszResourceName)
{

	// If this is not the first call then Delete GDI objects
	if( m_bitmap.m_hObject != NULL )
		m_bitmap.DeleteObject();
	if( m_pal.m_hObject != NULL )
		m_pal.DeleteObject();
	
	
	HBITMAP hBmp = (HBITMAP)::LoadImage( AfxGetInstanceHandle(), 
			(CString) lpszResourceName, IMAGE_BITMAP, 0,0, LR_CREATEDIBSECTION );

	if( hBmp == NULL ) 
		return FALSE;

	m_bitmap.Attach( hBmp );
	BITMAP bm;
	m_bitmap.GetBitmap( &bm );
	m_cxBitmap = bm.bmWidth;
	m_cyBitmap = bm.bmHeight;


	// Create a logical palette for the bitmap
	DIBSECTION ds;
	BITMAPINFOHEADER &bmInfo = ds.dsBmih;
	m_bitmap.GetObject( sizeof(ds), &ds );

	int nColors = bmInfo.biClrUsed ? bmInfo.biClrUsed : 1 << bmInfo.biBitCount;

	// Create a halftone palette if colors > 256. 
	CClientDC dc(NULL);			// Desktop DC
	if( nColors > 256 )
		m_pal.CreateHalftonePalette( &dc );
	else
	{
		// Create the palette

		RGBQUAD *pRGB = new RGBQUAD[nColors];
		CDC memDC;
		memDC.CreateCompatibleDC(&dc);

		memDC.SelectObject( &m_bitmap );
		::GetDIBColorTable( memDC, 0, nColors, pRGB );

		UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * nColors);
		LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];

		pLP->palVersion = 0x300;
		pLP->palNumEntries = nColors;

		for( int i=0; i < nColors; i++)
		{
			pLP->palPalEntry[i].peRed = pRGB[i].rgbRed;
			pLP->palPalEntry[i].peGreen = pRGB[i].rgbGreen;
			pLP->palPalEntry[i].peBlue = pRGB[i].rgbBlue;
			pLP->palPalEntry[i].peFlags = 0;
		}

		m_pal.CreatePalette( pLP );

		delete[] pLP;
		delete[] pRGB;
	}
	Invalidate();

	return TRUE;
}

void CStatSimTree::SetDefaultCursor()
{
	   // Get the windows directory
        CString strWndDir;
        GetWindowsDirectory(strWndDir.GetBuffer(MAX_PATH), MAX_PATH);
        strWndDir.ReleaseBuffer();

        strWndDir += _MBCS("\\winhlp32.exe");
        // This retrieves cursor #106 from winhlp32.exe, which is a hand pointer
        HMODULE hModule = LoadLibrary(strWndDir);
        if (hModule) {
            HCURSOR hHandCursor = ::LoadCursor(hModule, MAKEINTRESOURCE(106));
            if (hHandCursor)
			{
                cursor_hand = CopyCursor(hHandCursor);
			}
			      
        }
        FreeLibrary(hModule);

		cursor_arr	= ::LoadCursor(NULL, IDC_ARROW);
		cursor_no	= ::LoadCursor(NULL, IDC_NO) ;
}



void CStatSimTree::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{

	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	Select(pNMTreeView->itemNew.hItem, TVGN_CARET);

	*pResult = 0;

}
