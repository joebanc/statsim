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
#include <math.h>
#include <strstream>

#include "cdate.h"

///////////////////////////////////////////////////////////////////////////////
//
// Static variable declarations
//
// Declare the number of days in each month, also the date values for
// specific decades as an optimisation
//

const int CDateTime::m_anMonthLen[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
const int CDateTime::m_anMonthJul[] = {0,0,31,59,90,120,151,181,212,243,273,304,334};
CString CDateTime::m_sMonth;

///////////////////////////////////////////////////////////////////////////////

inline int CDateTime::DaysInYear(int nYear) {return IsLeapYear(nYear) ? 366 : 365;}

inline int CDateTime::LeapDays(int nYear) {return nYear/4 - nYear/100 + nYear/400;}

inline BOOL CDateTime::IsLeapYear(int nYear)
{
   return !(nYear % 4) && ((nYear % 100) || (!(nYear % 400)));
};            


///////////////////////////////////////////////////////////////////////////////

void CDateTime::JulAsDate(int nYear, int nJul, int &nMonth, int &nDay)
{  
   for (int i = 1; i <= 12; i++)
   {   
      if (nJul > GetDaysMonth(i, nYear))
      {
         nJul -= GetDaysMonth(i, nYear);
      } else
      {
         nMonth = i;
         nDay = nJul;
         break;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
//
// int GetJulDay(int nYear, nMonth, nDay)
//

int CDateTime::GetJulDay(int nYear, int nMonth, int nDay)
{
   int nJDay = nDay + m_anMonthJul[nMonth]; 
   
   if (nMonth > 2 && IsLeapYear(nYear))
   {
      nJDay++;
   };         
   
   return nJDay;
};

///////////////////////////////////////////////////////////////////////////////
//
// Constructor for CDateTime
//
  
CDateTime::CDateTime(int nYear, int nMonth, int nDay,
                     int nHours, int nMinutes, int nSeconds)
{   
   DateAsLong(nYear,nMonth,nDay,&m_lDate);
   TimeAsLong(nHours,nMinutes,nSeconds, &m_lTime);  

   ASSERT(IsValid());   
}   

///////////////////////////////////////////////////////////////////////////////
//
// CDateTime::CDateTime()
//
// Constructor for CDateTime
//

CDateTime::CDateTime(long lDate, long lTime)
{  
   m_lDate = lDate;
   m_lTime = lTime;
   ASSERT(IsValid() || (m_lDate == 0 && m_lTime == 0));
}        

///////////////////////////////////////////////////////////////////////////////
//
// BOOL CDateTime::DateAsString(CString&)
//
// Copies the date in the format DD/MM/YYYY to the given string
//

BOOL CDateTime::DateAsString(CString& strDate)
{                                  
   int nDay, nMonth, nYear;
   CString sMonth;
        
   LongAsDate(m_lDate,&nYear,&nMonth,&nDay);      
   
   if (IsDateValid(nYear, nMonth, nDay))
   {  
	   if (nMonth == 0 && nDay == 0)
      {
		  strDate.Format("%d",nYear);                     
      }
	   else 
      {         
         
	      sMonth.LoadString(IDS_JAN+nMonth-1);
         sMonth = sMonth.Left(3);

         if (nDay == 0)
         {
		      strDate.Format("%s %d",(LPCSTR)sMonth, nYear);                     
         }       
	      else
         {
            strDate.Format("%d %s %d",nDay, (LPCSTR)sMonth, nYear);                     
         }
      }
   } else
   {
      return FALSE;
   }   
   return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//
// BOOL CDateTime::TimeAsString(CString&)
//
// Copies the time in the format HH:MM to the given string
//

BOOL CDateTime::TimeAsString(CString& strTime)
{
  BOOL bOK = TRUE;

  // Null string indicated by -1

  if (m_lTime == -1)
  {
	  strTime = "";
	  return TRUE;
  }
    
  // Extract components of time
   
   int nHour, nMinute, nSecond;
   
   bOK = LongAsTime(m_lTime, &nHour, &nMinute, &nSecond);
            
  // Write the text to the string
    
    if (bOK)
    {
       strTime.Format("%02i:%02i",nHour, nMinute);       
    };      
   
   return bOK;  
}
 
///////////////////////////////////////////////////////////////////////////////
//
// BOOL CDateTime::StringAsDate()
//
// Converts a string in the format DD-MMM-YYYY or MMM-YYYY or YYYY
// to a datetime structure
//

BOOL CDateTime::StringAsDate(CString sDate)
{                                                
   BOOL bOK = TRUE;
   int nDay = 0, nMonth = 0, nYear = 0;
   CString sMonth, sDay, sYear, s;   
   int nTmp;   

   sDate.TrimLeft();
   sDate.TrimRight();

  // First scan for Excel and dBase formats

   if (sscanf(sDate,"%d-%d-%d %d:%d:%d", &nYear,&nMonth,&nDay,&nTmp,&nTmp,&nTmp) != 6 &&
       (sscanf(sDate,"%d-%d-%d", &nYear,&nMonth,&nDay) != 3 || nYear < 100) &&
       !ExcelYear(sDate, &nYear))
   {
      
     // Determine format of string

      sDate.TrimLeft(" ");
	   sDate.TrimRight(" ");

      int i = sDate.FindOneOf(".,-/ ");
      for (int j = sDate.GetLength()-1; j >= 0 && CString(" -/").Find(sDate[j]) == -1; j--);	   
	   if (i == -1 && j == -1) sYear = sDate;
	   else if (i == j)
	   { 
		   sMonth = sDate.Left(i);
		   sYear = sDate.Mid(i+1);
	   } else
	   {	
	      sDay = sDate.Left(i);
	      sMonth = sDate.Mid(i+1, j-i-1);
	      sYear = sDate.Mid(j+1);
	   };
      sDay.TrimLeft();
      sDay.TrimRight();
      sMonth.TrimLeft();
      sMonth.TrimRight();
      sYear.TrimLeft();
      sYear.TrimRight();
      
      // If the month is numeric and the day contains characters then swap

      if (sMonth.FindOneOf("0123456789") != -1 && sDay.FindOneOf("0123456789") == -1)
      {
         s = sDay;
         sDay = sMonth;
         sMonth = s;
      }

	   // Determine day

	   if (!sDay.IsEmpty())
	   {
		   bOK = sscanf(sDay, "%d", &nDay);
	   };
	   
	   // Determine month
  
      if (bOK && !sMonth.IsEmpty())
      {           
         for (int i = 1; i <= 12 && nMonth == 0; i++)
         {
           s.LoadString(IDS_JAN+i-1);           
           if (strnicmp(sMonth, s, sMonth.GetLength())== 0 &&
               sMonth.GetLength() >= 3)

           {          
              nMonth = i;
           }; 
         };     
     
        if (nMonth == 0)
        { 
           bOK = FALSE;
        };    
     }

      // Determine year

      if (bOK)
      {
	      bOK = sscanf(sYear,"%d", &nYear);
      }
   };
  
  // Construct the new datetime if the values are valid
   
   if (bOK && IsDateValid(nYear,nMonth,nDay))
   {        
      DateAsLong(nYear,nMonth,nDay,&m_lDate);
	  m_lTime = 0;

   } else
   {
      bOK = FALSE;
   } 
      
   return bOK;
}

///////////////////////////////////////////////////////////////////////////////
//
// Check for dates of the form 1999.0 as imported from Excel
//

BOOL CDateTime::ExcelYear(LPCSTR psDate, int* pnYear)
{
   *pnYear = 0;

   // Skip space

   while (isspace(*psDate)) pnYear++;

   // Create number

   while (isdigit(*psDate))
   {
      *pnYear = *pnYear*10 + *psDate-'0';;
      psDate++;
   }

   // Ensure that the last two digits are .0

   if (psDate[0] != '.' || psDate[1] != '0') return FALSE;

   return IsDateValid(*pnYear,0,0);
}

///////////////////////////////////////////////////////////////////////////////
//
// Converts a string in the format DD-MM-YYYY or DD MMM YYYY 
// to a datetime structure
//                        

BOOL CDateTime::StringAsDateTime(LPCSTR sDate, LPCSTR sTime)
{  
   return StringAsDate(sDate) && 
          StringAsTime(sTime);           
};

///////////////////////////////////////////////////////////////////////////////
//
// CDateTime& CDateTime::operator=(const CDateTime& rSrcDateTime)
//
// Assignment operator                                           
//

CDateTime& CDateTime::operator=(const CDateTime& rSrcDateTime)
{
   //ASSERT(rSrcDateTime.IsValid());

   m_lDate = rSrcDateTime.m_lDate;
   m_lTime = rSrcDateTime.m_lTime;   
   
   return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
// CDateTime& CDateTime::Advance()
//
// Advance the date for the number of days (or fraction there of) provided
//

CDateTime& CDateTime::Advance(long lDays, long lSecs)
{
   ASSERT(IsValid());
   
   DateAddDays(&m_lDate, lDays);
   m_lTime += lSecs;        
   
   if (m_lTime >= 86400)
   {      
      DateAddDays(&m_lDate, m_lTime / 86400);
      m_lTime -= (m_lTime / 86400) * 86400; 
   }
   
   else if (m_lTime < 0)
   {                           
      long lDays = ((m_lTime+1) / 86400)-1;
      DateAddDays(&m_lDate, lDays);
      m_lTime -= lDays * 86400;
   };   
   
   return *this;
} 

///////////////////////////////////////////////////////////////////////////////
// 
// Access functions
//

///////////////////////////////////////////////////////////////////////////////
//        
// int CDateTime::GetYear()
//

int CDateTime::GetYear()
{            
   int nDay, nMonth, nYear;
      
   LongAsDate(m_lDate,&nYear,&nMonth,&nDay);
   
   return nYear;
}

///////////////////////////////////////////////////////////////////////////////
//        
// int CDateTime::GetMonth()
//

int CDateTime::GetMonth()
{  
   int nDay, nMonth, nYear;             
   
   LongAsDate(m_lDate,&nYear,&nMonth,&nDay);   
   
   return nMonth;
}

///////////////////////////////////////////////////////////////////////////////
//        
// int CDateTime::GetDay()
//

int CDateTime::GetDay()
{            
   int nDay, nMonth, nYear;   
   
   LongAsDate(m_lDate,&nYear,&nMonth,&nDay);   
   
   return nDay;
}

///////////////////////////////////////////////////////////////////////////////
//        
// int CDateTime::GetHour()
//

int CDateTime::GetHour()
{             
   int nHour, nMinute, nSecond;   
   
   LongAsTime(m_lTime,&nHour,&nMinute,&nSecond);
   
   return nHour;
}

///////////////////////////////////////////////////////////////////////////////
//        
// int CDateTime::GetMinute()
//

int CDateTime::GetMinute()
{            
   int nHour, nMinute, nSecond;   
   
   LongAsTime(m_lTime,&nHour,&nMinute,&nSecond);
   
   return nMinute;
}
                 
///////////////////////////////////////////////////////////////////////////////
//        
// int CDateTime::GetSecond()
//

int CDateTime::GetSecond()
{            
   int nHour, nMinute, nSecond;   
   
   LongAsTime(m_lTime,&nHour,&nMinute,&nSecond);
   
   return nSecond;
}
                  
/////////////////////////////////////////////////////////////////////////////////////////
//
// BOOL CDateTime::IsTimeValid(CString&)
//
// Determines if a time, in string format is valid e.g. 15:30:12
// returns true if it is.  Will also allow the seconds value to be missed
// in which case this would default to zero
//

BOOL CDateTime::IsTimeValid(const CString& strTime)
{   
   UINT nHour, nMinute, nSecond = 0;
   
   BOOL bValid = TRUE;
  
  // Extract values from the string
   
   if (sscanf(strTime,"%u%*c%u%*c%u",&nHour, &nMinute, &nSecond) < 2)
   {
      bValid = FALSE;
   }                 
   
   if (bValid && IsTimeValid(nHour,nMinute,nSecond) == FALSE)
   {
      bValid = FALSE;
   }                 
   
   return bValid;
}

///////////////////////////////////////////////////////////////////////////////
//
// BOOL CDateTime::GetDaysMonth(int nMonth, int nYear)
//
// Return the number of days in the month for the current year
//

BOOL CDateTime::GetDaysMonth(int nMonth, int nYear)
{                      
  ASSERT(nMonth >= 1 && nMonth <= 12);
  
  if (nMonth != 2)
  { 
    return m_anMonthLen[nMonth];
    
  } else if (IsLeapYear(nYear))
  { 
    return 29;       
  } else
  {
    return 28;
  }   
};

///////////////////////////////////////////////////////////////////////////////
// 
// CDateTime& CDateTime::operator+(int nDays)
//
// Addition/Subtraction operators 
//

CDateTime CDateTime::operator+(int nDays)
{ 
   long lDate = m_lDate;
   DateAddDays(&lDate,nDays);   
   return CDateTime(lDate,m_lTime);   
};

CDateTime CDateTime::operator-(int nDays)
{ 
   long lDate = m_lDate;
   DateAddDays(&lDate,-nDays);
   return CDateTime(lDate,m_lTime);   
};

///////////////////////////////////////////////////////////////////////////////

CDateTime CDateTime::operator+(double dDays)
{  
   double dTmp;
   long lDate = m_lDate;
   long lTime = m_lTime;
   
   DateAddDays(&lDate,(long)dDays);
   lTime += (long)(modf(dDays, &dTmp)*86400);   
   
   if (lTime < 0)
   { 
      lTime += 86400;
      DateAddDays(&lDate,-1);
   } 
   else if (lTime >= 86400)
   { 
      lTime -= 86400;
      DateAddDays(&lDate,1);
   };         
   
   return CDateTime(lDate,lTime);
};

///////////////////////////////////////////////////////////////////////////////

CDateTime CDateTime::operator-(double dValue)
{ 
   return *this + (-dValue);    
};

///////////////////////////////////////////////////////////////////////////////
//
// double CDateTime::operator-(CDateTime& rSrc)
//
// Determine number of whole and fractional parts between two dates
//
   
double CDateTime::operator-(CDateTime& rSrc)
{ 
  // First convert dates to YYYYMMDD form

   long lDate1 = rSrc.m_lDate;
   long lDate2 = m_lDate;

   if (lDate1 < 10000) lDate1 *= 100;
   if (lDate1 < 1000000) lDate1 *= 100;
   if (lDate2 < 10000) lDate2 *= 100;
   if (lDate2 < 1000000) lDate2 *= 100;

   // Nb. Not sure how DateDiff will cope with zero months and years
   
   long lDays = DateDiff(lDate1, lDate2);
   
   long lTime = m_lTime - rSrc.m_lTime;
   
   if (lTime < 0)
   { 
      lTime += 86400;
      lDays--;
   } 
   else if (lTime >= 86400)
   { 
      lTime -= 86400;
      lDays++;
   };         
   
   return lDays + lTime/86400.0;
};

///////////////////////////////////////////////////////////////////////////////
//
// int CDateTime::MonthDisp(CDateTime)                       
//
// Returns the number of inclusive months between two dates
// E.G. MAR 93 to JAN 94 would be 11 months
//
int CDateTime::MonthDisp(CDateTime dateTo)                       
{ 
   int nMonthDisp;
   int nMonth = GetMonth();
   int nYear = GetYear();

   nMonthDisp = 0;

   while (nYear < dateTo.GetYear())
     {
      nMonthDisp += 13 - nMonth;
      nMonth = 1;
      nYear++;
     };

   nMonthDisp += dateTo.GetMonth() - nMonth;
   return nMonthDisp;
};

///////////////////////////////////////////////////////////////////////////////
//
// void CDateTime::AdvanceYear()
//
// Advances a date forward by one year
//

void CDateTime::AdvanceYear()
{
    int nYear, nMonth, nDay;
    LongAsDate(m_lDate,&nYear,&nMonth,&nDay);
    DateAsLong(nYear+1,nMonth,nDay,&m_lDate);
};

void CDateTime::AdvanceMonth()
{     
    int nYear, nMonth, nDay;
    LongAsDate(m_lDate,&nYear,&nMonth,&nDay); 
    
    nMonth++;
    if (nMonth > 12)
    {
       nYear++;
       nMonth = 1;
    }
    DateAsLong(nYear,nMonth,nDay,&m_lDate);     
};

///////////////////////////////////////////////////////////////////////////////
//
// BOOL CDateTime::IsValid()
//

BOOL CDateTime::IsValid()
{                                     

   int nYear, nMonth, nDay, nHour, nMinute, nSecond;   
   return LongAsDate(m_lDate, &nYear, &nMonth, &nDay) && 
          LongAsTime(m_lTime,  &nHour, &nMinute, &nSecond);    
};

///////////////////////////////////////////////////////////////////////////////
//
// void CDateTime::AsSystemDate()
//
// Sets the date and time to that of the current system time
//

void CDateTime::AsSystemDate()
{
   time_t time_tDate;          // Current system time and date   
   struct tm *pTmDate;         // Current system time and date
   
   time(&time_tDate); 
   pTmDate = localtime(&time_tDate); 
   
   *this = CDateTime(pTmDate->tm_year+1900, pTmDate->tm_mon+1, pTmDate->tm_mday,
                    pTmDate->tm_hour, pTmDate->tm_min, pTmDate->tm_sec);
}

///////////////////////////////////////////////////////////////////////////////
//
// BOOL CDateTime::IsDateValid(int nYear, int nMonth, int nDay)
//
// Nb. Zero day or zero month and day are allowed e.g. 1999 or Jun 1999
//

BOOL CDateTime::IsDateValid(int nYear, int nMonth, int nDay)
{  
   if (nYear < 100)
   {      
      return FALSE;          
   }

   if (nMonth == 0 && nDay == 0) return TRUE;

   if( nMonth < 1 || nMonth > 12 )
   {     
      return FALSE;
   }   

   if (nDay == 0) return TRUE;

   if( nDay < 1 || nDay > GetDaysMonth(nMonth,nYear) )
   {                            
      return FALSE;     
   } else
   {
     return TRUE;
   }
};

///////////////////////////////////////////////////////////////////////////////
//
// BOOL IsTimeValid(int nHour, int nMinute, int nSecond)
//

BOOL CDateTime::IsTimeValid(int nHour, int nMinute, int nSecond)
{
   BOOL bOK = TRUE;
      
  // Validate
   
   if (bOK)
   {      
      if (!(nHour >= 0 && nHour < 24 &&
          nMinute >= 0 && nMinute < 60 &&
          nSecond >= 0 && nSecond < 60))
      {                                
         bOK = FALSE;
      };
   };
   
   return bOK;
};


///////////////////////////////////////////////////////////////////////////////
//
// Dates are stored as YYYYMMDD or YYYYMM or YYYY
//

BOOL CDateTime::DateAsLong(int nYear, int nMonth, int nDay, long* plDate)
{
	if (nDay == 0 && nMonth == 0) *plDate = nYear;
	else if (nDay == 0) *plDate = nYear * 100 + nMonth;
	else *plDate = nYear*10000 + nMonth*100 + nDay;

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CDateTime::TimeAsLong(int nHour, int nMinute, int nSecond, long* plTime)
{
	*plTime = nHour * 3600l + nMinute * 60l + nSecond;

	return TRUE;
}



///////////////////////////////////////////////////////////////////////////////

BOOL CDateTime::LongAsDate(long lDate, int* pnYear, int* pnMonth, int* pnDay)
{
   *pnYear = 0;
   *pnMonth = 0;
   *pnDay = 0;

	if (lDate >= 10000000)
	{
      *pnYear = lDate/10000;
      *pnMonth = (lDate - *pnYear*10000)/100;
      *pnDay = (lDate - *pnYear*10000) - *pnMonth * 100;         
	} 
	else if (lDate >= 100000)
	{
      *pnYear =  lDate/100;
      *pnMonth = lDate - *pnYear*100;
	}
	else
	{
       *pnYear = lDate;
	   *pnMonth = 0;
	   *pnDay = 0;
	}

    return IsDateValid(*pnYear, *pnMonth, *pnDay);

}

///////////////////////////////////////////////////////////////////////////////

BOOL CDateTime::LongAsTime(long lTime, int* pnHour, int* pnMinute, int* pnSecond)
{
   if (lTime == -1)
   {
	   *pnHour = 0;
	   *pnMinute = 0;
	   *pnSecond = 0;
	   return TRUE;
   }

   *pnHour = (int)(lTime / 3600l);
   *pnMinute = (int)((lTime - *pnHour*3600l)/60l);
   *pnSecond = (int)((lTime - *pnHour*3600l) - *pnMinute * 60l);  

   return IsTimeValid(*pnHour, *pnMinute, *pnSecond);
}



///////////////////////////////////////////////////////////////////////////////
//
// BOOL CDateTime::StringAsTime()
//
// Converts a string to time, allowing for the water day
//

BOOL CDateTime::StringAsTime(CString sTime)
{
   UINT nHour = 0;
   UINT nMinute = 0;
   UINT nSecond = 0;
   
   BOOL bOK = TRUE;
   
  // No time is indicated by -1

   sTime.TrimLeft();
   if (sTime.IsEmpty())
   {
	  m_lTime = -1;
	  return TRUE;
   }
      
  // Extract values from the string   

   if (sscanf(sTime,"%d%*c%d%*c%d",&nHour, &nMinute, &nSecond) < 2)
   {
	  bOK = FALSE;
   }
     
  // Construct the new datetime if the values are valid
   
   if (bOK)
   {    
      bOK = TimeAsLong(nHour,nMinute,nSecond,&m_lTime);                               
   };
      
   return bOK;
};

////////////////////////////////////////////////////////////////////////////////
//
// Adds lDays onto date plDate in format YYYYMMDD
//

BOOL CDateTime::DateAddDays(long* plDate, long lDays)
{
   int nYear, nMonth, nDay;
   BOOL bOK = TRUE;

   if (LongAsDate(*plDate, &nYear, &nMonth, &nDay))
   {
      // Convert to a julian date

      int nJul = GetJulDay(nYear, nMonth, nDay);
      
      // Add year

      int nYear2 = nYear + lDays/365;

      // Add days

      nJul += lDays%365;
      nJul = nJul - LeapDays(nYear2-1) + LeapDays(nYear-1);

      if (nJul > DaysInYear(nYear2))
      {
         nJul -= DaysInYear(nYear2++);         
      }
      else if (nJul < 1)
      {
         nJul += DaysInYear(--nYear2);         
      }      

      // Convert back to a gregorian date

      JulAsDate(nYear2, nJul, nMonth, nDay);      


      DateAsLong(nYear2, nMonth, nDay, plDate);       

   } else
   {
      bOK = FALSE;
   }

   return bOK;



}

////////////////////////////////////////////////////////////////////////////////
//
// Returns the difference between two dates in days (lDate2-lDate1)
//
// lDate1 and lDate2 are of the format YYYYMMDD
//

long CDateTime::DateDiff(long lDate1, long lDate2)
{
   int nYear1, nMonth1, nDay1;
   int nYear2, nMonth2, nDay2;

   LongAsDate(lDate1, &nYear1, &nMonth1, &nDay1);
   LongAsDate(lDate2, &nYear2, &nMonth2, &nDay2);

   long lDays = (GetJulDay(nYear2, nMonth2, nDay2) + nYear2*365 +  LeapDays(nYear2-1)) - 
                (GetJulDay(nYear1, nMonth1, nDay1) + nYear1*365 +  LeapDays(nYear1-1));
 
   return lDays;
}

///////////////////////////////////////////////////////////////////////////////
//
// Compares two dates, allowing for the fact that one may be a year only and
// the other may be a full date etc.
// Returns 0 if equal or -ve if first is earlier, positive if later
//
// For times, if either is set to null (-1) then assume they are equal
//

long CDateTime::DateCompare(long lDate0, long lDate1, long lTime0, long lTime1)
{    
   if (lDate0 > 1000000 && lDate1 < 1000000) lDate0 /= 100;
   if (lDate0 < 1000000 && lDate1 > 1000000) lDate1 /= 100;
   if (lDate0 > 10000 && lDate1 < 10000) lDate0 /= 100;
   if (lDate0 < 10000 && lDate1 > 10000) lDate1 /= 100;

   long lDiff = lDate0 - lDate1;
   if (lDiff != 0) return lDiff;

   // If comparing months or years then time is irrelevant

   if (lDate0 < 1000000) return 0;

   // Compare times
   
   if (lTime0 == -1 || lTime1 == -1) return 0;
   else return lTime0 - lTime1;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CDateTime::operator==(CDateTime rdt)
{
   ASSERT(IsValid() && rdt.IsValid());
   return DateCompare(m_lDate, rdt.m_lDate, m_lTime, rdt.m_lTime) == 0;   
}

BOOL CDateTime::operator!=(CDateTime rdt)
{
   ASSERT(IsValid() && rdt.IsValid());
   return DateCompare(m_lDate, rdt.m_lDate, m_lTime, rdt.m_lTime) != 0; 
}

BOOL CDateTime::operator<(CDateTime rdt)
{ 
   ASSERT(IsValid() && rdt.IsValid());
   return DateCompare(m_lDate, rdt.m_lDate, m_lTime, rdt.m_lTime) < 0;       
}    
BOOL CDateTime::operator>(CDateTime rdt)
{
   ASSERT(IsValid() && rdt.IsValid());
   return DateCompare(m_lDate, rdt.m_lDate, m_lTime, rdt.m_lTime) > 0;         
}          
BOOL CDateTime::operator>=(CDateTime rdt)
{
   ASSERT(IsValid() && rdt.IsValid());
   return DateCompare(m_lDate, rdt.m_lDate, m_lTime, rdt.m_lTime) >= 0;        
}  
                            
BOOL CDateTime::operator<=(CDateTime rdt)
{
   ASSERT(IsValid() && rdt.IsValid());
   return DateCompare(m_lDate, rdt.m_lDate, m_lTime, rdt.m_lTime) <= 0;       
}                                            
