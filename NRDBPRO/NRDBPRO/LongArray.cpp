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

#include "stdafx.h"
#include "nrdb.h"
#include "LongArray.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLongArray::CLongArray()
{

}

CLongArray::~CLongArray()
{

}

///////////////////////////////////////////////////////////////////////////////

int CLongArray::Find(long l)
{
   int i = 0; for (i = 0; i < GetSize(); i++)
   {
      if (GetAt(i) == l) return i;
   }
   return -1;
}

/////////////////////////////////////////////////////////////////////////////

int compare(const void *elem1, const void* elem2)
{
	return *(long*)elem1 - *(long*)elem1;
}

/////////////////////////////////////////////////////////////////////////////

void q_sort(long numbers[], int left, int right)
{
  int pivot, l_hold, r_hold;

  ASSERT(left <= right);

  l_hold = left;
  r_hold = right;
  pivot = numbers[left];
  while (left < right)
  {
    while ((numbers[right] >= pivot) && (left < right))
      right--;
    if (left != right)
    {
      numbers[left] = numbers[right];
      left++;
    }
    while ((numbers[left] <= pivot) && (left < right))
      left++;
    if (left != right)
    {
      numbers[right] = numbers[left];
      right--;
    }
  }
  numbers[left] = pivot;
  pivot = left;
  left = l_hold;
  right = r_hold;
  if (left < pivot)
    q_sort(numbers, left, pivot-1);
  if (right > pivot)
    q_sort(numbers, pivot+1, right);
}

///////////////////////////////////////////////////////////////////////////////

void CLongArray::Sort()
{
   //qsort(GetData(), GetSize(), sizeof(long), compare);
   if (GetSize() > 0) q_sort(GetData(), 0, GetSize()-1);
};

///////////////////////////////////////////////////////////////////////////////

int CLongArray::FindSorted(long l)
{
	int nHigh = GetSize()-1;
	int nLow = 0;
	int nMiddle;

	while (nLow <= nHigh)
	{
	   nMiddle = (nLow+nHigh)/2;

	   if (l > GetAt(nMiddle)) nLow = nMiddle+1;
	   else if (l < GetAt(nMiddle)) nHigh = nMiddle-1;
	   else return nMiddle;
       
	}
	return -1;
	
}