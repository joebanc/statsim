// SSTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "StatSimAPI.h"
#include "SSTreeCtrl.h"


// CSSTreeCtrl.cpp : implementation file
/////////////////////////////////////////////////////////////////////////////
// CSSTreeCtrl
IMPLEMENT_DYNAMIC(CSSTreeCtrl, CTreeCtrl)

CSSTreeCtrl::CSSTreeCtrl()
{
	memDC_bgColor_bitmap = RGB(255,255,255);

	m_pDragImage = NULL;
	m_bLDragging = FALSE; 

	isImageTiled = true ;

	SetDefaultCursor() ;
}

CSSTreeCtrl::~CSSTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CSSTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CSSTreeCtrl)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnTvnBegindrag)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemExpanding)
	ON_WM_ERASEBKGND()
	ON_WM_QUERYNEWPALETTE()
	ON_WM_PALETTECHANGED()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSSTreeCtrl message handlers

HTREEITEM CSSTreeCtrl::AddGroup(const CString & group_name)
{
 	HTREEITEM added_group = 0;
	HTREEITEM groupItem ; 

		if ( (groupItem = GetGroupTreeItem(group_name)) != 0 )  return 0;

	HTREEITEM lastAddedGroup = 0;
		
		//insert the group into the tree
		added_group = InsertItem(group_name, 1,1,0,TVI_FIRST);
	    //Associates 32 bit number with this item
	//	SetItemData(added_group,(DWORD)newGroup);
	
		Expand(added_group,TVE_EXPAND);


	return added_group ;
}

HTREEITEM CSSTreeCtrl::GetGroupTreeItem(CString grp_name)
{

	HTREEITEM answer = 0;
	
	HTREEITEM groupItem = GetRootItem()/*(TVI_ROOT, TVGN_NEXT)*/;
	while (groupItem != NULL && !answer)	//while there is still something in the tree
	{
	
		TVITEM item;
		TCHAR szText[1024];
		//CString szText = name;
		item.hItem = groupItem;
		//only get the text and the handle of the item
		item.mask = TVIF_TEXT | TVIF_HANDLE;	
		item.pszText = szText;
		item.cchTextMax = 1024;

		GetItem(&item);

		CString thisGroupName = item.pszText;

		if (thisGroupName == grp_name)
		{
			answer = groupItem;
			break ;
		}
		
		//get the next item for the sake of the while loop ending
		groupItem = GetNextItem(groupItem,TVGN_NEXT);
	}
	return answer;
}

bool CSSTreeCtrl::DeleteGroup(CString group_name)
{
	if (group_name.IsEmpty() ) return false ;

	HTREEITEM group_item ;
	if ( (group_item = GetGroupTreeItem(group_name)) == 0 ) return false ;


		//delete every child of this group
		HTREEITEM currentItem = GetNextItem(group_item ,TVGN_CHILD);

		while (currentItem != NULL)
		{
			DeleteItem(currentItem);

			//get the next item for the sake of the while loop ending
			currentItem = GetNextItem(currentItem,TVGN_NEXT);
		}

		//delete the actual group now
		DeleteItem(group_item);

	return true ;
}

HTREEITEM CSSTreeCtrl::AddChild(CString buddy_name, CString group_name)
{
 
	HTREEITEM buddy_that_was_added;

	//check if the buddy already exists
	if (GetBuddyTreeItem(buddy_name) != 0) return 0;


		HTREEITEM tree_group = GetGroupTreeItem(group_name);

		//if the group actually exists add the buddy to it
		if (tree_group != 0)
		{
			buddy_that_was_added = InsertItem(buddy_name,2, 2,tree_group,TVI_SORT);

		  //	SetItemData(buddy_that_was_added,(DWORD)newBuddy);
		}
		//create the group then add the buddy
		else	
		{
			tree_group = AddGroup(group_name);
			buddy_that_was_added = InsertItem(buddy_name, 2, 2,tree_group,TVI_SORT);
	
		//	SetItemData(buddyWasAdded,(DWORD)newBuddy);
		}
	

		//this just expands the group the buddy was added to
		HTREEITEM hParent = GetParentItem(buddy_that_was_added);
	
		if (hParent != NULL)	Expand(hParent, TVE_EXPAND);

	
	
	return buddy_that_was_added;
}

HTREEITEM CSSTreeCtrl::GetBuddyTreeItem(CString buddy_name)
{
  	HTREEITEM answer = 0;
	//get the root group
	HTREEITEM groupItem = GetRootItem();
	while (groupItem != NULL && !answer)	//while there is still something in the tree
	{
	HTREEITEM currentItem = GetNextItem(groupItem,TVGN_CHILD);
	while (currentItem != NULL && !answer)	//while there is still something in the tree
	{
		TVITEM item;
		TCHAR szText[1024];
		//CString szText = name;
		item.hItem = currentItem;
		//only get the text and the handle of the item
		item.mask = TVIF_TEXT | TVIF_HANDLE;	
		item.pszText = szText;
		item.cchTextMax = 1024;

		/*BOOL answer = */
		GetItem(&item);

		CString thisBuddyName = item.pszText;
		if (thisBuddyName == buddy_name)
			answer = currentItem ;
		else
			answer = 0;

		//get the next item for the sake of the while loop ending
		currentItem = GetNextItem(currentItem,TVGN_NEXT);
	}
		groupItem = GetNextItem(groupItem,TVGN_NEXT);
	}
	return answer;
}

void CSSTreeCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CTreeCtrl::OnLButtonDblClk(nFlags, point);
	
	HTREEITEM hItem = GetSelectedItem();
	
	// If this is a root node, return
	if (GetParentItem(hItem) == NULL) return ;

}

void CSSTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{

	CTreeCtrl::OnRButtonDown(nFlags, point);
}

void CSSTreeCtrl::OnTvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;
	
	// So user cant drag root node
	if (GetParentItem(pNMTreeView->itemNew.hItem) == NULL) return ; 

	// Item user started dragging ...
	m_hitemDrag = pNMTreeView->itemNew.hItem;
	m_hitemDrop = NULL;



	m_pDragImage = CreateDragImage(m_hitemDrag);  // get the image list for dragging
	// CreateDragImage() returns NULL if no image list
	// associated with the tree view control
	if( !m_pDragImage )
		return;

	m_bLDragging = TRUE;
	m_pDragImage->BeginDrag(0, CPoint(-15,-15));
	POINT pt = pNMTreeView->ptDrag;
	ClientToScreen( &pt );
	m_pDragImage->DragEnter(NULL, pt);
	SetCapture();

}

void CSSTreeCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
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
				
			// Tests if dragged item is over another child !
			  if ( (GetParentItem(hitem) != NULL) && (cursor_no != ::GetCursor())) 
			  {
				  ::SetCursor(cursor_no);
				   // Dont want last selected target highlighted after mouse
				   // has moved off of it, do we now ?
				   SelectDropTarget(NULL);
			  }
			// Is item we're over a root node and not parent root node ?
			if ( (GetParentItem(hitem) == NULL) && (GetParentItem(m_hitemDrag) != hitem ) ) 
			{
				if (cursor_arr != ::GetCursor()) ::SetCursor(cursor_arr); 
				SelectDropTarget(hitem);
			}

			m_hitemDrop = hitem;
			CImageList::DragShowNolock(TRUE);
		}
	}
	else 
	{
		// Set cursor to arrow if not dragged
		// Otherwise, cursor will stay hand or arrow depen. on prev setting
		::SetCursor(cursor_arr);
	}

	CTreeCtrl::OnMouseMove(nFlags, point);

}

void CSSTreeCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{

	CTreeCtrl::OnLButtonUp(nFlags, point);

	if (m_bLDragging)
	{
		m_bLDragging = FALSE;
		CImageList::DragLeave(this);
		CImageList::EndDrag();
		ReleaseCapture();

		if(m_pDragImage != NULL) 
		{ 
		delete m_pDragImage; 
		m_pDragImage = NULL; 
		} 

		// Remove drop target highlighting
		SelectDropTarget(NULL);

		if( m_hitemDrag == m_hitemDrop )
			return;

		HTREEITEM	hitem;
		// Make sure pt is over some item
		if ( ((hitem = HitTest(point, &nFlags)) == NULL)  ) return ;
		// Make sure dropped item isnt a child
		if (GetParentItem(hitem) != NULL) return ;

		// If Drag item is an ancestor of Drop item then return
		HTREEITEM htiParent = m_hitemDrop;
		while( (htiParent = GetParentItem( htiParent )) != NULL )
		{
			if( htiParent == m_hitemDrag ) return;
		}

		Expand( m_hitemDrop, TVE_EXPAND ) ;

		HTREEITEM htiNew = MoveChildItem( m_hitemDrag, m_hitemDrop, TVI_LAST );
		DeleteItem(m_hitemDrag);
		SelectItem( htiNew );
	}

}

HTREEITEM CSSTreeCtrl::MoveChildItem(HTREEITEM hItem, HTREEITEM htiNewParent, HTREEITEM htiAfter)
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


bool CSSTreeCtrl::DeleteChild(CString buddy_name)
{
	if (buddy_name.IsEmpty()) return false;


	HTREEITEM buddyItem = GetBuddyTreeItem(buddy_name);

	if (buddyItem != 0)
	{
		DeleteItem(buddyItem);
		return true ; 
	}

	return false;
}

int CSSTreeCtrl::GetChildCountInGroup(CString group_name)
{

	HTREEITEM group_item = GetGroupTreeItem(group_name);	//get the group
	
	if (group_item == 0)					return -1 ;
	if (!ItemHasChildren(group_item))		return  0 ;

		int total_in_group =0 ;
		
		//delete every child of this group
		HTREEITEM currentItem = GetNextItem(group_item ,TVGN_CHILD);

		while (currentItem != NULL)
		{
			++total_in_group ;

			//get the next item for the sake of the while loop ending
			currentItem = GetNextItem(currentItem,TVGN_NEXT);
		}

	return total_in_group ;
}

CString CSSTreeCtrl::RemoveBuddyCountFromName(CString name)
{
	if (name.Find(_T("(")) > 0)
	{
		name = name.Left(name.Find(_T("(")) - 1);
	}
	else return name;

	return name;
}

/*
void CSSTreeCtrl::CollapseAll()
{
        HTREEITEM hti = GetRootItem();
        do{
             Expand( hti, TVE_COLLAPSE ); //     CollapseBranch( hti );
        }while( (hti = GetNextSiblingItem( hti )) != NULL );
}

  */

void CSSTreeCtrl::OnPaint() 
{

	// Remove comments from next five lines if you don't need any 
	// specialization beyond adding a background image
//	if( m_bitmap.m_hObject == NULL )
//	{
//		CTreeCtrl::OnPaint();
//		return;
//	}

	// temp
	CTreeCtrl::OnPaint();
	return;
	///

	CPaintDC dc(this);

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

void CSSTreeCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if( m_bitmap.m_hObject != NULL ) InvalidateRect(NULL);

	CTreeCtrl::OnHScroll(nSBCode, nPos, pScrollBar);

}

void CSSTreeCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if( m_bitmap.m_hObject != NULL )	InvalidateRect(NULL);

	CTreeCtrl::OnVScroll(nSBCode, nPos, pScrollBar);

}

void CSSTreeCtrl::OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	if( m_bitmap.m_hObject != NULL ) InvalidateRect(NULL);
	
	*pResult = 0;

}

BOOL CSSTreeCtrl::OnEraseBkgnd(CDC* pDC) 
{
	if( m_bitmap.m_hObject != NULL )	return TRUE;

	return CTreeCtrl::OnEraseBkgnd(pDC);

}

BOOL CSSTreeCtrl::OnQueryNewPalette() 
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

void CSSTreeCtrl::OnPaletteChanged(CWnd* pFocusWnd) 
{
	CTreeCtrl::OnPaletteChanged(pFocusWnd);
	
	if( pFocusWnd == this )
		return;

	OnQueryNewPalette();

}

BOOL CSSTreeCtrl::SetBkImage(UINT nIDResource)
{
	return SetBkImage( (LPCSTR)nIDResource );
}

BOOL CSSTreeCtrl::SetBkImage(LPCSTR lpszResourceName)
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



void CSSTreeCtrl::SetDefaultCursor()
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

