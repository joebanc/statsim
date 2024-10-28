// Array.h: interface for the CArray class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARRAY_H__25B89495_095C_43ED_A17D_555474CD7A52__INCLUDED_)
#define AFX_ARRAY_H__25B89495_095C_43ED_A17D_555474CD7A52__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPointArray : public CArray <POINT, POINT>
{
public:
    
   void Initialise(int nMaxSize) 
   {
      if (m_pData != NULL) delete [] (POINT*)m_pData;
		m_pData = new POINT[nMaxSize];
		m_nSize = 0;
		m_nMaxSize = nMaxSize;
   }
};

///////////////////////////////////////////////////////////////////////////////

class CFeatureArray : public CArray <CFeature, CFeature>
{
public:
	CFeatureArray() {m_bSorted = FALSE;}

	BOOL IsFeatureSel(long lFType, long lFeature);
	int Add(CFeature feature) {m_bSorted = FALSE; return CArray <CFeature,CFeature>::Add(feature);}
	void Sort();

protected:
	BOOL m_bSorted;
	void QSort(int start, int end);
	
};


#endif // !defined(AFX_ARRAY_H__25B89495_095C_43ED_A17D_555474CD7A52__INCLUDED_)
