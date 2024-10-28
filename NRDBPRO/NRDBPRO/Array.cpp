// Array.cpp: implementation of the CArray class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "nrdbpro.h"
#include "Array.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////

void CFeatureArray::Sort()
{
	if (GetSize() > 0)
	{
	   QSort(0, GetSize()-1);
	};
}

///////////////////////////////////////////////////////////////////////////////

void CFeatureArray::QSort(int left, int right)
{
  int pivot,l_hold, r_hold;
  CFeature feature;

  l_hold = left;
  r_hold = right;
  feature = GetAt(left);
  while (left < right)
  {
    while ((GetAt(right).m_lFeatureTypeId > feature.m_lFeatureTypeId || 
		(GetAt(right).m_lFeatureTypeId == feature.m_lFeatureTypeId && GetAt(right).m_lId >= feature.m_lId)) && (left < right))
      right--;
    if (left != right)
    {
      SetAt(left,GetAt(right));
      left++;
    }
    while ((GetAt(left).m_lFeatureTypeId < feature.m_lFeatureTypeId || 
		(GetAt(left).m_lFeatureTypeId == feature.m_lFeatureTypeId && GetAt(left).m_lId <= feature.m_lId)) && (left < right))
      left++;
    if (left != right)
    {
      SetAt(right,GetAt(left));
      right--;
    }
  }
  SetAt(left,feature);
  pivot = left;
  left = l_hold;
  right = r_hold;
  if (left < pivot)
    QSort(left, pivot-1);
  if (right > pivot)
    QSort(pivot+1, right);
}

///////////////////////////////////////////////////////////////////////////////

BOOL CFeatureArray::IsFeatureSel(long lFType, long lFeature)
{
	// If unsorted then first sort

	if (!m_bSorted) Sort();
	m_bSorted = TRUE;

	// Now search for the feature type and feature

	int nHigh = GetSize()-1;
	int nLow = 0;
	int nMiddle;

	while (nLow <= nHigh)
	{
	   nMiddle = (nLow+nHigh)/2;

	   if (lFType > GetAt(nMiddle).m_lFeatureTypeId || (lFType == GetAt(nMiddle).m_lFeatureTypeId && lFeature > GetAt(nMiddle).m_lId)) nLow = nMiddle+1;
	   else if (lFType < GetAt(nMiddle).m_lFeatureTypeId || (lFType == GetAt(nMiddle).m_lFeatureTypeId && lFeature < GetAt(nMiddle).m_lId)) nHigh = nMiddle-1;
	   else return TRUE;
       
	}
	return FALSE;

}