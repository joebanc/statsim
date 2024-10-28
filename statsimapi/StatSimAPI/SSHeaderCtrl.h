#pragma once

#ifndef SSHEADERCTRL
	#define SSHEADERCTRL
#endif
// CSSHeaderCtrl

class __declspec(dllexport) CSSHeaderCtrl : public CHeaderCtrl
{
	DECLARE_DYNAMIC(CSSHeaderCtrl)

public:
	CSSHeaderCtrl();
	virtual ~CSSHeaderCtrl();
	BOOL Init(CHeaderCtrl *pHeader);

protected:
	void OnEndTrack(NMHDR * pNMHDR, LRESULT* pResult);
	int	 m_nSavedImage;			// Index of saved image	
	CImageList m_cImageList;	// Image list for this header control

	DECLARE_MESSAGE_MAP()
};


