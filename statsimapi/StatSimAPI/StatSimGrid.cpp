// StatSimGrid.cpp : implementation file
//
#include "stdafx.h"
#include "StatSimAPI.h"
#include "StatSimGrid.h"

// CStatSimGrid

IMPLEMENT_DYNAMIC(CStatSimGrid, CButton)
/////////////////////////////////////////////////////////////////////////////

void inline swap(long& a, long& b)
{
   long c = a;
   a = b;
   b = c;
}

COLORREF inline InverseColor(COLORREF cr)
{
   return RGB(255-GetRValue(cr), 255-GetGValue(cr), 255 - GetBValue(cr));
};

/////////////////////////////////////////////////////////////////////////////
// CStatSimGrid

CStatSimGrid::CStatSimGrid()
{
   m_nActiveRow = 1;
   m_nActiveCol = 1;
   m_nFocusRow = 1;
   m_nFocusCol = 1;

   m_nFirstVisibleRow = 1;
   m_nFirstVisibleCol = 1;
   m_nVisibleColumns = 0;
   m_nVisibleRows = 0;

   m_celltopleft.Row = -2;
   m_celltopleft.Col = -2;
   m_cellbottomright.Col = -2;
   m_cellbottomright.Row = -2;

   m_nRows = 0;
   m_nCols = 0;

   m_dColWidth = 1;

   m_hFont = NULL;

   m_bShift = FALSE;
   m_bCtrl = FALSE;   
   m_bLButton = FALSE;
   
   m_nUndoRow = 0;
   m_nUndoCol = 0;

   m_hDragCursor = LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_DRAGHORZ));
   //m_hDefaultCursor = ::GetCursor();

   m_nDragCol = 0;
   m_bDragCursor = FALSE;

   m_bEditMode = FALSE;
   
}

CStatSimGrid::~CStatSimGrid()
{
   if (m_hDragCursor != NULL) DestroyCursor(m_hDragCursor);
}


BEGIN_MESSAGE_MAP(CStatSimGrid, CButton)
	//{{AFX_MSG_MAP(CStatSimGrid)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CHAR()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_KEYUP()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()   
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP

   ON_MESSAGE(WM_SETEDITMODE, OnSetEditMode)
   ON_MESSAGE(WM_COPY, OnCopy)
   ON_MESSAGE(WM_CUT, OnCut)
   ON_MESSAGE(WM_PASTE, OnPaste)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatSimGrid message handlers

CSSGRow::CSSGRow(CSSGRow& rSrc)
{
   *this = rSrc;
}

CSSGRow& CSSGRow::operator=(CSSGRow& rSrc )
{
   RemoveAll();
   Copy(rSrc);
   return *this;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CStatSimGrid::ClearDataRange(SS_COORD Col,  SS_COORD Row, 
                      SS_COORD Col2, SS_COORD Row2)
{
   for (int i = Col; i <= Col2; i++)
   {
      for (int j = Row; j <= Row2; j++)
      {
         SetValue(i, j, _MBCS(""));         
         SetInteger(i, j, 0);
         SetIndex(i, j, -1);
      }
   }
   return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//
// Resize the grid to reflect the number of columns
//

void CStatSimGrid::SetMaxCols(SS_COORD MaxCols)
{
   m_nCols = MaxCols+1;
   for (int iRow = 0; iRow < m_nRows; iRow++)
   {
      int nSize = m_aGrid[iRow].GetSize();
      m_aGrid[iRow].SetSize(m_nCols);      
   };

   m_aColumns.SetSize(m_nCols);
}   

///////////////////////////////////////////////////////////////////////////////
//
// Resize the grid to reflect the number of rows
//

void CStatSimGrid::SetMaxRows(SS_COORD MaxRows) 
{   
   m_nRows = MaxRows+1;
   m_aGrid.SetSize(m_nRows);   
   
   // Update the column size for the new columns

   SetMaxCols(m_nCols-1);
};

///////////////////////////////////////////////////////////////////////////////

BOOL CStatSimGrid::DelRow(SS_COORD Row) 
{
   if (m_nRows > 2)
   {
      m_aGrid.RemoveAt(Row); 
      SetMaxRows(GetMaxRows()-1);      

      // Create a blank row

      if (m_nRows == 1)
      {
         InsRow(1);
      }
   };
     
   return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CStatSimGrid::InsRow(SS_COORD nRow) 
{
   CSSGRow row; 
   m_aGrid.InsertAt(nRow, row); 
   SetMaxRows(GetMaxRows()+1);
   return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//
// Set value, update number of columns if necessary
//

BOOL CStatSimGrid::SetValue(SS_COORD Col, SS_COORD Row, LPCSTR lpData)
{
   if (Row >= m_nRows) SetMaxRows(Row);
   if (Col >= m_nCols) SetMaxCols(Col);   

   m_aGrid[Row][Col].m_sText = lpData; 
   m_aGrid[Row][Col].m_sText.TrimRight();
   
   Invalidate(FALSE);
      
   return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CStatSimGrid::SetColWidth(SS_COORD Col, double Width)
{ 
   if (Col == -1)
   {
      for (int i = 0; i < m_nCols; i++)
      {
         m_aColumns[i].m_dWidth = Width; 
      }

      // Set default column width
      m_dColWidth = Width;
   } else
   {
      m_aColumns[Col].m_dWidth = Width; 
   }
   return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

double CStatSimGrid::GetColWidth(SS_COORD Col)
{
   // If no column width set then use default

   double dWidth = m_aColumns[Col].m_dWidth;

   // Comparing with -1 should be ok because no arithmetic performed

   if (Col == 0) return 4;
   else if (dWidth == -1) return m_dColWidth;
   else return dWidth;

   return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CStatSimGrid::SetColor(SS_COORD Col, SS_COORD Row, COLORREF Background,
                COLORREF Foreground)
{
   // 
   Invalidate(FALSE);

   return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

COLORREF CStatSimGrid::GetCellColor(SS_COORD Col,  SS_COORD Row)
{
   if (Col == 0 || Row == 0) return RGB(192, 192, 192);

   long n;
   if (GetInteger(Col,  Row, &n) && n == 0) return RGB(255,255,255);
   else return RGB(0,0,250);
}

///////////////////////////////////////////////////////////////////////////////

void CStatSimGrid::PreSubclassWindow() 
{   
	CButton::PreSubclassWindow();
}


///////////////////////////////////////////////////////////////////////////////

void CStatSimGrid::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{      
   CString s;

   // Determine characer width

   CDC* pDC = CDC::FromHandle(lpDIS->hDC);
   CSize sz = pDC->GetTextExtent(_T("A"));   

   CRect rectC;
   CRect rect;
   GetClientRect(&rectC);

   pDC->SetBkMode(TRANSPARENT);

   // Initialise

   m_aRectCols.RemoveAll();
   m_aRectRows.RemoveAll();

   // Use the font

   CFont* pFont = CFont::FromHandle(m_hFont);

   CFont* pFontOld = pDC->SelectObject(pFont);
   
   // Draw the grid

   int y = 0;
   int x = 0;
   int xMax = 0;

   for (int iRow = 0; iRow < m_nRows; iRow++)
   {
      x = 0;
      double dHeight;
      GetRowHeight(iRow, &dHeight);
      int nHeight  = (int)(sz.cy * dHeight * 1.3);

      // Allow for scolling

      if (iRow != 0 && iRow < m_nFirstVisibleRow) iRow = m_nFirstVisibleRow;
      if (y > rectC.bottom) break;      

      for (int iCol = 0; iCol < m_nCols; iCol++)
      {   
         // Allow for scrolling

          if (iCol != 0 && iCol < m_nFirstVisibleCol) iCol = m_nFirstVisibleCol; 

          // Don't draw if not visible

          if (x > rectC.right) break;          

          if (m_aColumns[iCol].m_bVisible)
		  {

			  // Determine column width

			  int nWidth = (int)(sz.cx * GetColWidth(iCol) * 1.2);
   
			  // Draw rectangle

			  rect.left = x;
			  rect.right = x + nWidth;
			  rect.top = y;
			  rect.bottom = y + nHeight;

			  COLORREF cr = GetCellColor(iCol, iRow);
			  COLORREF crT = RGB(0,0,0);

			   // Check if selected

			  if ((m_celltopleft.Row <= iRow && iRow <= m_cellbottomright.Row ||
				  m_celltopleft.Row == -1) && 
				  (m_celltopleft.Col <= iCol && iCol <= m_cellbottomright.Col ||
				  m_celltopleft.Col == -1) && !(iRow == m_nActiveRow && 
				  iCol == m_nActiveCol))
			  {
				 // Inverse colour
				 cr = InverseColor(cr);
				 crT = InverseColor(crT);
			  }


			  pDC->FillSolidRect(rect.left,rect.top, rect.Width(), rect.Height(), cr);

			  if (iCol == 0 || iRow == 0)
			  {                  
				 pDC->Draw3dRect(rect.left, rect.top, rect.Width()+1, rect.Height()+1, 
					   0,0);             
				 pDC->Draw3dRect(rect.left+1, rect.top+1, rect.Width()-1, rect.Height()-1, 
					   RGB(255,255,255), RGB(128,128,128));
			  } else
			  {
				 pDC->Draw3dRect(rect.left, rect.top, rect.Width()+1, rect.Height()+1, 
					   RGB(192,192,192), RGB(192,192,192));             
			  }

			  // If cell is active then indicate this

			  if (iCol == m_nActiveCol && iRow == m_nActiveRow && iCol > 0 && iRow > 0)
			  {
				 cr = InverseColor(cr);
				 pDC->Draw3dRect(rect.left+1, rect.top+1, rect.Width()-2, rect.Height()-2, 
					   cr, cr);                          
			  }
          
			  // Draw the text - for numbers do not display

			  if (iRow == 0 || m_aColumns[iCol].m_celltype.m_nType == Edit || 
				  m_aColumns[iCol].m_celltype.m_nType == Combo)
			  {
				 s = GetValue(iCol, iRow);

				 int nFlags = DT_LEFT|DT_NOPREFIX;
				 if (iRow == 0 || iCol == 0) 
				 {
					if (pDC->GetTextExtent(s).cx < rect.Width())
					{
						nFlags = DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX|DT_CENTER;
					} else
					{
						nFlags = DT_WORDBREAK|DT_NOPREFIX|DT_CENTER;                
					};
				 };

				 // Display row numbers

				 if (iCol == 0 && iRow != 0) s.Format(_T("%i"), iRow);

				 CRect rectT = rect;
				 rectT.left += 2;
				 rectT.top += 2;

				 pDC->SetTextColor(crT);
				 pDC->DrawText(s, &rectT, nFlags);
			  };
                    
			  // Update the position

			  x += nWidth;

			  // Determine approximate number of columns from last drawn

			  m_nVisibleColumns = (long)(ceil( (double) rectC.Width() / nWidth));
		  } else
		  {
			  rect = CRect();
		  }


          // Store the position of the cell

          if (iRow == 0) m_aRectCols.Add(rect);
      };      
      
      y += nHeight;
      xMax = max(x, xMax);

      m_nVisibleRows = (long)ceil( (double) rectC.Height() / nHeight);

      m_aRectRows.Add(rect);
   }  
   
   // Clear space to right (more smooth than repainting)

   pDC->FillSolidRect(xMax, 0, rectC.right, y, RGB(192,192,192));
   pDC->FillSolidRect(0, y, rectC.right, rectC.bottom, RGB(192,192,192));


   // Set scroll position
   
   SetScrollRange(SB_HORZ, 1, m_nCols-1, FALSE);
   SetScrollPos(SB_HORZ, m_nFirstVisibleCol, TRUE); 

   SetScrollRange(SB_VERT, 1, m_nRows-1, FALSE);
   SetScrollPos(SB_VERT, m_nFirstVisibleRow, TRUE); 

   // Tidy up

   pDC->SelectObject(pFontOld);
}

///////////////////////////////////////////////////////////////////////////////

void CStatSimGrid::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
   int nCol = 0;

   switch(nSBCode)
   {     
      case SB_LINELEFT:                          
         if (m_nFirstVisibleCol > 1)
         {
            m_nFirstVisibleCol--;                  
            while (m_nFirstVisibleCol > 1 && !m_aColumns[m_nFirstVisibleCol].m_bVisible) m_nFirstVisibleCol--;
         }
         else return;
      break;

      case SB_LINERIGHT:        
         // Skip hidden         
         if (m_nFirstVisibleCol+1 < m_nCols) 
         {
            m_nFirstVisibleCol++;        
            while (m_nFirstVisibleCol+1 < m_nCols && !m_aColumns[m_nFirstVisibleCol].m_bVisible) m_nFirstVisibleCol++;
         }
         else return;
      break;

  // page left
         
      case SB_PAGELEFT:        
         nCol = max(1, m_nFirstVisibleCol - m_nVisibleColumns);
         if (nCol != m_nFirstVisibleCol) m_nFirstVisibleCol = nCol;
         else return;         
      break;            
  
  // page right
      
      case SB_PAGERIGHT:          
         nCol = min(m_nCols-1, m_nFirstVisibleCol + m_nVisibleColumns);
         if (nCol != m_nFirstVisibleCol) m_nFirstVisibleCol = nCol;
         else return;
      break;         
      
      case SB_THUMBPOSITION:         
          m_nFirstVisibleCol = nPos;
      break;
     
      default:        
         CButton::OnHScroll(nSBCode, nPos, pScrollBar);
         return;
   } 	   

   SetEditMode(FALSE);
   Invalidate(FALSE);  
}

///////////////////////////////////////////////////////////////////////////////

void CStatSimGrid::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
   int nRow = 0;

	switch(nSBCode)
   {     
      case SB_LINEUP:        
         if (m_nFirstVisibleRow > 1) m_nFirstVisibleRow --;
         else return;
      break;

      case SB_LINEDOWN:        
         if (m_nFirstVisibleRow+1 < m_nRows) m_nFirstVisibleRow ++;        
      break;

  // page left
         
      case SB_PAGEUP:        
        nRow = max(1, m_nFirstVisibleRow - m_nVisibleRows);
         if (nRow != m_nFirstVisibleRow) m_nFirstVisibleRow = nRow;
         else return;         
      break;            
  
  // page right
      
      case SB_PAGEDOWN:                            
         nRow = min(m_nRows-1, m_nFirstVisibleRow + m_nVisibleRows);
         if (nRow != m_nFirstVisibleRow) m_nFirstVisibleRow = nRow;
         else return;
      break;         
      
      case SB_THUMBPOSITION:         
         m_nFirstVisibleRow = nPos;        
      break;
     
      default:        
         CButton::OnHScroll(nSBCode, nPos, pScrollBar);
         return;
   } 	   
   
   SetEditMode(FALSE);
   Invalidate(FALSE);  
}

///////////////////////////////////////////////////////////////////////////////

void CStatSimGrid::OnLButtonDown(UINT nFlags, CPoint point) 
{
   CButton::OnLButtonDown(nFlags, point);   

   // Under windows 98, upon gaining focus point is 0,0

   if (point.x == 0 && point.y == 0) return;

   // Clear any previous selection

   if (!m_bShift)
   {
      SetActiveCell(m_nActiveCol, m_nActiveRow, clearsel|save);
   };
  
   long lCol, lRow;
   if (GetCellPoint(point, &lCol, &lRow))
   {      
      if (lCol > 0 && lRow > 0)
      {
         CRect rect;
         GetRectCell(lCol, lRow, &rect);            

         // If the cell is a combobox then create it and fill list

         if (m_aColumns[lCol].m_celltype.m_nType == Combo)
         {
            m_nActiveRow = lRow;
            m_nActiveCol = lCol;
            SetEditMode(TRUE);
         } else
         {
            SetActiveCell(lCol, lRow);
         }
         Invalidate(FALSE);
      }

      // Select entire column

      else
      {
         if (lRow == 0) lRow = -1;
         if (lCol == 0) lCol = -1;
         SetActiveCell(lCol, lRow);
      }
   }            

   // Set after processing so that active cell is changed

   if (!IsWindow(m_combo))
   {
      m_bLButton = TRUE;
      SetCapture();
   };
}

///////////////////////////////////////////////////////////////////////////////

void CStatSimGrid::OnLButtonUp(UINT nFlags, CPoint point) 
{
   // End drag selection

   if (m_bLButton) ReleaseCapture();
   m_bLButton = FALSE;
   m_nDragCol = 0;
   
	CButton::OnLButtonUp(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////

void CStatSimGrid::OnMouseMove(UINT nFlags, CPoint point) 
{
   long lCol, lRow;

   m_bDragCursor = FALSE;

   CRect rect;
   GetWindowRect(&rect);

   // Detect drag position

   if (m_nDragCol == 0)
   {      
      if (GetCellPoint(point, &lCol, &lRow) && lRow == 0)
      {
         long lDragCol = 0;
         for (int i = 1; i < m_aRectCols.GetSize(); i++)
         {
            if (m_aRectCols[i].right -2 <= point.x && point.x < m_aRectCols[i].right+2)
            {                  
               lDragCol = i + m_nFirstVisibleCol-1;     
               break;
            }
         }
         if (lDragCol != 0)
         {            
            m_bDragCursor = TRUE;

            if (m_bLButton)
            {
               m_nDragCol = lDragCol;            
            };         
         };
      }
   } 

   // Drag current column

   else
   {
      // Not sure how this occurs but causes error      
      if (point.x > rect.Width()-4) return;
     
      m_bDragCursor = TRUE;

      // Get width of current column

      double dWidth = m_aColumns[m_nDragCol].m_dWidth;

      // Get the rectangle corresponding to the current column

      CRect rectC = m_aRectCols[m_nDragCol-m_nFirstVisibleCol+1];

      // Calculate new width

      dWidth = max(1, (dWidth * (point.x - rectC.left)) / rectC.Width());

      m_aColumns[m_nDragCol].m_dWidth = dWidth;

      Invalidate(FALSE);
   }

   // Check for selection of cells

   if (m_bLButton)
   {
      if (GetCellPoint(point, &lCol, &lRow))
      {
         if (lCol > 0 && lRow > 0)
         {
            if (lCol != m_nFocusCol || lRow != m_nFocusRow)
            {
               m_nFocusCol = lCol;
               m_nFocusRow = lRow;

               SetActiveCell(lCol, lRow);

               Invalidate(FALSE);  
            };
         } 
      } else 
      {
         // If over the edge of the control then scroll

         CPoint point2 = point;
         ClientToScreen(&point2);
         if (point2.x > rect.right) OnKeyDown(VK_RIGHT, 0, 0);
         if (point2.x < rect.left) OnKeyDown(VK_LEFT, 0, 0);
         if (point2.y < rect.top) OnKeyDown(VK_UP, 0, 0);
         if (point2.y > rect.bottom) OnKeyDown(VK_DOWN, 0,0);        
      }
   };      

  // Set the drag cursor

   if (m_bDragCursor) 
   {
      SetCursor(m_hDragCursor);
   };
	
	CButton::OnMouseMove(nFlags, point);
}


///////////////////////////////////////////////////////////////////////////////

BOOL CStatSimGrid::SetActiveCell(SS_COORD Col, SS_COORD Row, int nFlags) 
{
   CString s;      

   BOOL bChanged = m_nActiveCol != Col || m_nActiveRow != Row;

   if (bChanged) m_bEditMode = FALSE;

   // If editing then save data

   if (IsWindow(m_edit))
   {           
      m_edit.GetWindowText(s);
      BOOL bChanged = GetValue(m_nActiveCol, m_nActiveRow) != s;
      if (nFlags & save) SetValue(m_nActiveCol, m_nActiveRow, ConstChar(s));
      m_edit.DestroyWindow();

      // If data has changed then send message

      if (nFlags & save && bChanged) m_pParent->PostMessage(SSM_DATACHANGE);
   }

   if (IsWindow(m_combo))
   {
      int index = m_combo.GetCurSel();
      if (index != CB_ERR)
      {
         m_combo.GetLBText(index, s);         
         if (nFlags & save) SetValue(m_nActiveCol, m_nActiveRow, ConstChar(s));

         long index1 = GetIndex(m_nActiveCol, m_nActiveRow);         
         BOOL bChanged = index1 != index;
         if (nFlags & save) SetIndex(m_nActiveCol, m_nActiveRow, index);

         if (bChanged && nFlags & save) m_pParent->PostMessage(SSM_COMBOSELCHANGE);
      } 
      m_combo.DestroyWindow();               
   }

   // Set window

   if (!m_bShift && !m_bLButton)
   {
      if (Col == -1) m_nActiveCol = 1;
      else m_nActiveCol = Col; 

      if (Row == -1) m_nActiveRow = 1; 
      else m_nActiveRow = Row; 
      
      m_nFocusCol = m_nActiveCol;
      m_nFocusRow = m_nActiveRow;

    // Store undo information

      if (bChanged)
      {
         m_sUndo = GetValue(m_nActiveCol, m_nActiveRow);
         m_nUndoRow = Row;
         m_nUndoCol = Col;
      }
   };
       
   // Inform parent

   if (bChanged && Col > 0 && Row > 0)
   {              
      // Open combobox for new cell

      if (m_aColumns[Col].m_celltype.m_nType == Combo && !m_bShift && 
          !m_bLButton)
      {
         SetEditMode(TRUE);
      }
      m_pParent->PostMessage(SSM_LEAVECELL);
   };

   // Update selection
   
   if ((m_bShift || m_bLButton || Col == -1 || Row == -1) && !(nFlags & clearsel))
   {
      m_nFocusRow = Row;
      m_nFocusCol = Col;

      m_celltopleft.Col = Col;
      m_celltopleft.Row = Row;

      if (m_bShift || m_bLButton)
      {
         m_cellbottomright.Col = m_nActiveCol;
         m_cellbottomright.Row = m_nActiveRow;
      } else
      {
         m_cellbottomright = m_celltopleft;
      }
      
      if (m_celltopleft.Col > m_cellbottomright.Col) swap(m_celltopleft.Col, m_cellbottomright.Col);
      if (m_celltopleft.Row > m_cellbottomright.Row) swap(m_celltopleft.Row, m_cellbottomright.Row);      
   }
   else
   {               
      if (bChanged || nFlags & clearsel)
      {
         m_celltopleft.Row = -2;
         m_celltopleft.Col = -2;
         m_cellbottomright = m_celltopleft;
      };
   }
   
   return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CStatSimGrid::GetCellPoint(CPoint point, SS_COORD* pCol, SS_COORD* pRow)
{
   *pCol = -1;
   *pRow = -1;
   int i;

   // First determine the column 

   for (i = 0; i < m_aRectCols.GetSize(); i++)
   {
      if (m_aRectCols[i].left <= point.x && point.x < m_aRectCols[i].right)
      {
         if (i == 0) *pCol = 0;
         else *pCol = i + m_nFirstVisibleCol-1;     
         break;
      }
   }

   for (i = 0; i < m_aRectRows.GetSize(); i++)
   {
      if (m_aRectRows[i].top <= point.y && point.y < m_aRectRows[i].bottom)
      {
         if (i == 0) *pRow = 0;
         else *pRow = i + m_nFirstVisibleRow-1;                 
         break;
      }
   }

   return *pRow != -1 && *pCol != -1;
}

///////////////////////////////////////////////////////////////////////////////
//
// Returns the recangle for a given cell, if available
//
// Returns 1 if cell is visible, 
// -1 if cell is in first row or column
// -2 if the cell is partially visible
// otherwise 0
//

int CStatSimGrid::GetRectCell(SS_COORD Col, SS_COORD Row, CRect* pRect)
{      
   int iCol = 0;
   if (Col != 0) iCol = Col - m_nFirstVisibleCol+1;

   if (iCol < 0 || iCol >= m_aRectCols.GetSize()) return FALSE;

   pRect->left = m_aRectCols[iCol].left;
   pRect->right = m_aRectCols[iCol].right;

   int iRow = 0;
   if (Row != 0) iRow = Row - m_nFirstVisibleRow+1;

   if (iRow < 0 || iRow >= m_aRectRows.GetSize()) return FALSE;

   pRect->top = m_aRectRows[iRow].top;
   pRect->bottom = m_aRectRows[iRow].bottom;

   if (iCol == 0 || iRow == 0) return -1;

   // Cell is partially visible
   CRect rectC;
   GetClientRect(&rectC);
   if (pRect->right > rectC.right) return -2;

   else return 1;
}

///////////////////////////////////////////////////////////////////////////////

void CStatSimGrid::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
   CRect rect;   
   int nActiveRow = m_nFocusRow;
   int nActiveCol = m_nFocusCol;
   int nFirstVisibleCol = m_nFirstVisibleCol;
   int nFirstVisibleRow = m_nFirstVisibleRow;   

   // Check if shift is pressed down

   if (nChar == VK_SHIFT)
   {
      m_bShift = TRUE;
   }
   if (nChar == VK_CONTROL)
   {
      m_bCtrl = TRUE;
   }
   
   BOOL bShift = m_bShift;

   // Copy, cut and paste
   
   if ((nChar == VK_INSERT || nChar == 'C') && m_bCtrl)
   {
      OnCopy(0,0);
   }

   if ((nChar == VK_INSERT && m_bShift) || (nChar == 'V' && m_bCtrl))
   {
      OnPaste(0,0);
   }

   if ((nChar == VK_DELETE && m_bShift) || (nChar == 'X' && m_bCtrl))
   {
      OnCut(0,0);
   }

   if (nChar == 'Z' && m_bCtrl)
   {
      OnEditUndo();
   }

   // Next cell

   if (nChar == VK_RETURN || (nChar == VK_TAB && !m_bShift)) 
   {
     // Prevent dragging of selection
      m_bShift = FALSE;

      nActiveCol++;

      // If column is hidden then advance

      while (nActiveCol < m_nCols && !m_aColumns[nActiveCol].m_bVisible)
      {
         nActiveCol++;
      }


      if (nActiveCol >= m_nCols)
      {
         if (nActiveRow+1 < m_nRows)
         {
            nActiveCol = 1;
            nActiveRow++;
         } else
         {
            nActiveCol = m_nCols-1;
         }
      } 
   }   

   // Previous cell
   
   else if (nChar == VK_TAB && m_bShift)
   {
      // Prevent dragging of selection
      m_bShift = FALSE;

      nActiveCol--;

      // If column is hidden then advance

      while (nActiveCol >= 0 && !m_aColumns[nActiveCol].m_bVisible)
      {
         nActiveCol--;
      }

      if (nActiveCol == 0)
      {
         if (nActiveRow > 1)
         {
            nActiveRow--;
            nActiveCol = m_nCols-1;
         } else
         {
            nActiveCol = 1;
         }
      }
      
   }

   else if (nChar == VK_F2)
   {
      SetEditMode(TRUE);
   }

   else if (nChar == VK_ESCAPE)
   {
      // Quit without saving 
      SetActiveCell(nActiveCol, nActiveRow, clearsel);
      SetEditMode(FALSE);      
   }

   // Quit if combobox so not as to interfere with its options
      
   if (nChar == VK_RIGHT)
   {  
      // Find next visible column

      for (int i = nActiveCol+1; i < m_nCols; i++)
      {
         if (m_aColumns[i].m_bVisible)
         {
            nActiveCol = i;
            break;
         }
      };         
   }
   else if (nChar == VK_LEFT)
   {      
     for (int i = nActiveCol-1; i > 0; i--)
     {
         if (m_aColumns[i].m_bVisible)
         {
            nActiveCol = i;
            break;
         }
     };         
   }

  // Don't process up/down if combobox is visible
   
   if (!IsWindow(m_combo)) 
   {   
      if (nChar == VK_UP) 
      {      
         nActiveRow = max(1, nActiveRow-1);                     
      }
      else if (nChar == VK_DOWN)
      { 
         nActiveRow = min(m_nRows-1, nActiveRow+1);                     
      }      
   };

   // Pageup/down

   if (nChar == VK_PRIOR)
   {
      nActiveRow = max(1, nActiveRow- m_nVisibleRows+1);
   }
   
   else if (nChar == VK_NEXT)
   {
      nActiveRow = min(m_nRows-1, nActiveRow + m_nVisibleRows-1);
   }

   // Home/end

   else if (nChar == VK_HOME)
   {
      nActiveCol = 1;
   }

   else if (nChar == VK_END)
   {
      nActiveCol = m_nCols-1;
   }

   // Set visible cells

   if (nActiveRow != m_nFocusRow || nActiveCol != m_nFocusCol || 
       nFirstVisibleCol != m_nFirstVisibleCol || nFirstVisibleRow != m_nFirstVisibleRow)
   {       

      if (GetRectCell(nActiveCol, nActiveRow, &rect) <= 0)
      {    
         // Horizontal

         if (nActiveCol == 1) m_nFirstVisibleCol = 1;
         //else if (nActiveCol == m_nCols-1) m_nFirstVisibleCol = m_nCols-1;      
         else
         {
            if (nActiveCol > m_nFirstVisibleCol) OnHScroll(SB_LINERIGHT, 0, NULL);
            else if (nActiveCol < m_nFirstVisibleCol) OnHScroll(SB_LINELEFT, 0, NULL);
         };   

         // Vertical

         // Line-up / down
         if (GetRectCell(nActiveCol, nActiveRow, &rect) <= 0)
         {
            if (nActiveRow > m_nFirstVisibleRow) OnVScroll(SB_LINEDOWN, 0, NULL);
            else if (nActiveRow < m_nFirstVisibleRow) OnVScroll(SB_LINEUP, 0, NULL);

            // Still not visible, set cell

            if (GetRectCell(nActiveCol, nActiveRow, &rect) <= 0)
            {
               m_nFirstVisibleRow = max(1, nActiveRow);            
            };
         };   
      }
   
   // Redraw if cell has changed
      
      SetActiveCell(nActiveCol, nActiveRow);      
      Invalidate(FALSE);  
   };

   m_bShift = bShift;

   CButton::OnKeyDown(nChar, nRepCnt, nFlags);
}

///////////////////////////////////////////////////////////////////////////////

void CStatSimGrid::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
   if (nChar == VK_SHIFT) 
   {      
      m_bShift = FALSE;   
   };
   if (nChar == VK_CONTROL)
   {
      m_bCtrl = FALSE;
   }   
	
	CButton::OnKeyUp(nChar, nRepCnt, nFlags);
}

///////////////////////////////////////////////////////////////////////////////
//
// Start editing

void CStatSimGrid::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
   long lCol, lRow;

   if (GetCellPoint(point, &lCol, &lRow) && lCol > 0 && lRow > 0)
   {
       SetActiveCell(lCol, lRow);

       if (m_aColumns[lCol].m_celltype.m_nType == Edit && !m_aColumns[lCol].m_bLocked)
       {
          SetEditMode(TRUE);              
       }        
     };
	
	CButton::OnLButtonDblClk(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
//
// Handle message from combobox when closed up

LRESULT CStatSimGrid::OnSetEditMode(WPARAM wParam, LPARAM lParam)
{
   SetEditMode(wParam);
   return 0;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CStatSimGrid::SetEditMode(BOOL fEditModeOn, int nChar) 
{    
   CString s;
   USES_CONVERSION;

   // Destroys current edit control if necessary

   SetActiveCell(m_nActiveCol, m_nActiveRow);

   if (fEditModeOn)
   {
      CRect rect;
      GetRectCell(m_nActiveCol, m_nActiveRow,  &rect);
      rect.top++;
      rect.bottom--;
      rect.left++;
      rect.right--;

      // Edit control

       if (m_aColumns[m_nActiveCol].m_celltype.m_nType == Edit && 
          !IsWindow(m_edit))
       {
          
          m_edit.Create(ES_AUTOHSCROLL|WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS, rect, this, 1);          

          // Create maximum length according to cell type

          m_edit.SetLimitText(m_aColumns[m_nActiveCol].m_celltype.m_nLength);

          // Select font

          CFont* pFont = CFont::FromHandle(m_hFont);
          m_edit.SetFont(pFont);

          if (nChar != -1) 
			  s.Format(_T("%d"), nChar);	//modified by JEB from [s = nChar]
          else 
			  s = GetValue(m_nActiveCol, m_nActiveRow);
          s.TrimRight();
          m_edit.SetWindowText(ConstChar(s));       

          m_edit.SetFocus();    
          m_edit.SetSel(s.GetLength(), s.GetLength(), TRUE);
       } 

       // Combo box

       else if (m_aColumns[m_nActiveCol].m_celltype.m_nType == Combo && 
                !IsWindow(m_combo))
       {

         long lCol = m_nActiveCol;
         long lRow = m_nActiveRow;

         CRect rectI = rect;         
         rectI.bottom = rectI.top + rectI.Height() * 10;
   
         m_combo.Create(CBS_DROPDOWNLIST|WS_VSCROLL|WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS, rectI, this, 1);
         CFont* pFont = CFont::FromHandle(m_hFont);
         m_combo.SetFont(pFont);      

         // Fill list 

         CString s = m_aColumns[lCol].m_celltype.m_sItems;
         while (!s.IsEmpty())
         {
            int i = s.Find('\t');
            if (i == -1) i = s.GetLength();

            int index = m_combo.AddString(s.Left(i));            

            if (s.GetLength() > i) s = s.Mid(i+1);                                   
            else break;
            
         };               
      // Set current selection

         long index = GetIndex(lCol, lRow);
         m_combo.SetCurSel(index);                  
         m_combo.ShowDropDown();
         m_combo.SetFocus();
       }       
   };
   return 0;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CStatSimGrid::GetEditMode(void) 
{
   return IsWindow(m_edit);
} 

///////////////////////////////////////////////////////////////////////////////

void CStatSimGrid::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{   
   if (nChar != VK_RETURN && nChar != VK_TAB && nChar != VK_ESCAPE && 
       !m_bCtrl)
   {
      if (m_aColumns[m_nActiveCol].m_celltype.m_nType == Edit && 
          !m_aColumns[m_nActiveCol].m_bLocked)
      {
	      SetEditMode(TRUE, nChar);
      };
   };   
	
	CButton::OnChar(nChar, nRepCnt, nFlags);
}

///////////////////////////////////////////////////////////////////////////////

BOOL CStatSimGrid::SetLock(SS_COORD Col, SS_COORD Row, BOOL Lock)
{
   if (Row == -1)
   {      
      // Set default for column

      m_aColumns[Col].m_bLocked = Lock;
   } else
   {
      ASSERT(FALSE);
   };
   return TRUE;
};

///////////////////////////////////////////////////////////////////////////////

BOOL CStatSimGrid::ShowCol(SS_COORD Col, BOOL bShow)
{
	SetMaxCols(max(GetMaxCols(), Col));
	m_aColumns[Col].m_bVisible = bShow;
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CStatSimGrid::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
   // Subclass dialog (removes need to include DDX_Control entry in parent dialog

   SubclassDlgItem(nID, pParentWnd);   

   // Store copy of parent (GetParent may not work because window does not
   // have WS_CHILD flag)

   m_pParent = pParentWnd;
   
   
   return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//
// When control loses focus make it a child window so that mouse and keyboard
// messages are received by the parent
//

void CStatSimGrid::OnKillFocus(CWnd* pNewWnd) 
{	
   m_bShift = FALSE;
   m_bCtrl = FALSE;   
   m_nDragCol = 0;
   if (m_bLButton) ReleaseCapture();
   m_bLButton = FALSE;
   
   if (pNewWnd != &m_edit && pNewWnd != &m_combo)
   {
      m_bEditMode = IsWindow(m_edit);
      SetEditMode(FALSE);
   };

   CButton::OnKillFocus(pNewWnd);
}

//////////////////////////////////////////////////////////////////////////////
//
// When the control has focus make it a pop-up window so that it recieves all
// keyboard commands

void CStatSimGrid::OnSetFocus(CWnd* pOldWnd) 
{  
	CButton::OnSetFocus(pOldWnd);		
   
   SetEditMode(m_bEditMode);
   
}

///////////////////////////////////////////////////////////////////////////////

BOOL CStatSimGrid::SetCellType(SS_COORD Col, SS_COORD Row, LPSS_CELLTYPE CellType)
{
   if (Row == -1)
   {      
      m_aColumns[Col].m_celltype = *CellType;      
   } else
   {
      ASSERT(FALSE);
   }
   return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

LPSS_CELLTYPE CStatSimGrid::SetTypeEdit(LPSS_CELLTYPE pCellType, long Style, short Len,
                            short ChrSet, short ChrCase)
{
   pCellType->m_nType = Edit;
   pCellType->m_nLength = Len;
   return pCellType; 
}

///////////////////////////////////////////////////////////////////////////////
   
LPSS_CELLTYPE CStatSimGrid::SetTypeComboBox(LPSS_CELLTYPE pCellType, long Style,
                                         LPCSTR lpItems)
{
   pCellType->m_nType = Combo;
   pCellType->m_sItems = lpItems;
   return pCellType; 
};

///////////////////////////////////////////////////////////////////////////////

LPSS_CELLTYPE CStatSimGrid::SetTypeInteger(LPSS_CELLTYPE pCellType, long Min, long Max)
{
   pCellType->m_nType = Number;   
   return pCellType; 
};

///////////////////////////////////////////////////////////////////////////////

LRESULT CStatSimGrid::ComboBoxSendMessage (SS_COORD Col, SS_COORD Row, UINT Msg, WPARAM wParam, LPARAM lParam)
{
   ASSERT(m_aColumns[Col].m_celltype.m_nType == Combo);

   if (Msg == CB_GETCURSEL)
   {           
      return GetIndex(Col, Row);
   }

   if (Msg == CB_SETCURSEL)
   {
      SetIndex(Col, Row, wParam);
      return 0;
   }

   if (Msg == CB_ADDSTRING)
   {
      CString s;
	  s.Format(_T("%d"), lParam);
      ASSERT(Row == -1);

      CString sItems = m_aColumns[Col].m_celltype.m_sItems;
      if (!sItems.IsEmpty()) s = '\t' + s;
      sItems += s;
      m_aColumns[Col].m_celltype.m_sItems = sItems;      
      return 0;
   }
   return 0; 
}

///////////////////////////////////////////////////////////////////////////////

LRESULT CStatSimGrid::OnCopy(WPARAM wParam, LPARAM)
{

	USES_CONVERSION;

   CRect rect;
   CString s;

   // Copy current cell by creating a temporary edit control
   
   m_edit.Create(WS_CHILD, rect, this, 1);
   s = GetValue(m_nActiveCol, m_nActiveRow);
   m_edit.SetWindowText( ConstChar(s) );
   m_edit.SetSel(0,-1);
   m_edit.SendMessage(WM_COPY);
   m_edit.DestroyWindow();   

   // Redraw (if not called by cut)

   if (wParam == 0) Invalidate(FALSE);  
   return 0;
}

///////////////////////////////////////////////////////////////////////////////

LRESULT CStatSimGrid::OnCut(WPARAM, LPARAM)
{   
   // Same as copy but deletes

   OnCopy(1,0);
   SetValue(m_nActiveCol, m_nActiveRow, _MBCS(""));
   SetInteger(m_nActiveCol, m_nActiveRow, 0);
   SetIndex(m_nActiveCol, m_nActiveRow, -1);   
   
   Invalidate(FALSE);  
   return 0;
}

///////////////////////////////////////////////////////////////////////////////

LRESULT CStatSimGrid::OnPaste(WPARAM, LPARAM)
{
   CRect rect;
   CString s;

   // Paste into a temporary edit control then copy to the cell

   m_edit.Create(ES_AUTOHSCROLL|WS_CHILD, rect, this, 1);
   m_edit.SendMessage(WM_PASTE);
   m_edit.GetWindowText(s);
   SetValue(m_nActiveCol, m_nActiveRow, ConstChar(s));   
   //m_edit.SendMessage(WM_COPY);
   m_edit.DestroyWindow();   

   Invalidate(FALSE);  
   return 0;
}

///////////////////////////////////////////////////////////////////////////////

void CStatSimGrid::OnEditUndo() 
{
   SetValue(m_nUndoCol, m_nUndoRow, ConstChar(m_sUndo));
   Invalidate(FALSE);	
}

///////////////////////////////////////////////////////////////////////////////

BOOL CStatSimGrid::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{  
   if (m_bDragCursor) 
   {
      SetCursor(m_hDragCursor);
      return TRUE;
   };
     
	return CButton::OnSetCursor(pWnd, nHitTest, message);    
}

///////////////////////////////////////////////////////////////////////////////

BOOL CStatSimGrid::PreTranslateMessage(MSG* pMsg) 
{   
   // By default child buttons do not get keyboard messages

   if (GetFocus() == this)
   {
      // Necessary for keys such as tab
            
      if (pMsg->wParam == VK_TAB || pMsg->wParam == VK_ESCAPE ||
          pMsg->wParam == VK_LEFT || pMsg->wParam == VK_RIGHT ||
          pMsg->wParam == VK_UP || pMsg->wParam == VK_DOWN || 
          pMsg->wParam == VK_PRIOR || pMsg->wParam == VK_NEXT || 
          pMsg->wParam == VK_HOME || pMsg->wParam == VK_END || 
          pMsg->wParam == VK_RETURN)
      {
	      if (pMsg->message == WM_KEYDOWN)      
         {                  
            OnKeyDown(pMsg->wParam, LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));                                       
            return TRUE;
         } 
         else if (pMsg->message == WM_KEYUP)
         {       
            OnKeyUp(pMsg->wParam, LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));                        
            return TRUE;
         };                 
      };
      
      // Ensure all keys are passed OnChar

      if (pMsg->message == WM_CHAR)
      {
         OnChar(pMsg->wParam, LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));                  
         return TRUE;
      }                 
   };
   
	return CButton::PreTranslateMessage(pMsg);
      
}


