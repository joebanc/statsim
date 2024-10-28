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

#ifndef _MAPDOC_H_
#define _MAPDOC_H_

#include "maplayer.h"

class CDocMap : public CDocument
{
protected: // create from serialization only
	CDocMap();
	DECLARE_DYNCREATE(CDocMap)

   CMapLayerArray* GetLayers() {return &m_aMapObject;}

   class CViewLegend* GetViewLegend();
   class CViewMap* GetViewMap();
   class CViewLocator* GetViewLocator();

   void Open(LPCSTR sFile);

   void Activate();
   void OnLayers() {OnViewLayers();}

   int GetScale() {return m_nScale;}
   void ResetScale() {m_nScale = 0;}
   
public:
   static void DetermineScale(double dMin, double dMax, double *pdScaleMin, double *pdScaleMax, 
                              int *pnSteps = NULL, BOOL bLatLon = FALSE);
   
// Attributes
protected:
   CMapLayerArray m_aMapObject;

   int m_nScale;      
    
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDocMap)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	virtual void OnCloseDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDocMap();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
public:
	//{{AFX_MSG(CDocMap)
	afx_msg void OnViewLayers();
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileOpen();
   afx_msg void OnMapScale();
	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePrintPreview(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePrint(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMapZoom(CCmdUI* pCmdUI);	
	afx_msg void OnExportShapefile();
   afx_msg void OnUpdateExportShapefile(CCmdUI* pCmdUI);
	afx_msg void OnToolsProjections();
	afx_msg void OnMapLayout();
	afx_msg void OnUpdateMapLayout(CCmdUI* pCmdUI);
	afx_msg void OnMapGrid();		
	afx_msg void OnUpdateMapGrid(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif
