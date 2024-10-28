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

#ifndef _DATETIME_H_
#define _DATETIME_H_

#include <math.h>


class CDateTime
{ 

// Dates are stored as long integers in the format YYYYMMDD and time as long
// integers as the number of seconds into the day.
    
protected:    

   long m_lDate;
   long m_lTime;
   const static int m_anMonthLen[13];
   const static int m_anMonthJul[13];
   static CString m_sMonth;

//    
// Operations
//
  
public:  

//
// Constructors
//

    CDateTime() {m_lDate = 0; m_lTime = 0;};
      
    CDateTime(int nYear, int nMonth, int nDay,
              int nHour = 0, int nMinutes = 0, int nSeconds = 0);
    CDateTime(long lDate, long lTime);    

//
// Data manipulation
//              
    BOOL IsValid();        
    
    BOOL DateAsString(CString&);
	 BOOL TimeAsString(CString&);    
	 BOOL StringAsDate(CString);
    BOOL StringAsTime(CString);   
	 BOOL StringAsDateTime(LPCSTR sDate, LPCSTR sTime);

    
    static LPCSTR GetMonthName(int nMonth) {m_sMonth.LoadString(IDS_JAN-1+nMonth);return m_sMonth;}
         
    void AsSystemDate();
        
    CDateTime& Advance(long lDays, long lSecs);
    int MonthDisp(CDateTime);
    void AdvanceYear();    
    void AdvanceMonth();
    
    CDateTime operator+(int nDays);
    CDateTime operator-(int nDays);        
    CDateTime operator+(double);
    CDateTime operator-(double);
    
    double operator-(CDateTime& rSrc);
//
// Data conversion functions
//  
    long GetDateLong()
       {ASSERT(IsValid());
        return m_lDate;}
    long GetTimeLong()
       {ASSERT(IsValid());
        return m_lTime;}
    
    CDateTime AsDate()
    {       
       CDateTime dt(m_lDate,0);
       return dt;
    }
    CDateTime& AsRef()
    {
      ASSERT(IsValid());
      return *this;
    }
//
// Assignment
//    
    CDateTime& operator=(const CDateTime&);
//    
// Access functions
//
    int GetYear();       
    int GetMonth();       
    int GetDay();       
    int GetHour();       
    int GetMinute();
    int GetSecond();   
        
//
// Comparison operands
//                    
   BOOL operator==(CDateTime rdt);
   BOOL operator!=(CDateTime rdt);
   BOOL operator<(CDateTime rdt);
   BOOL operator>(CDateTime rdt);
   BOOL operator>=(CDateTime rdt);
   BOOL operator<=(CDateTime rdt);
        
   static BOOL IsDateValid(const CString&);
   static BOOL IsTimeValid(const CString&);   
   static BOOL IsTimeValid(int nHour, int nMinute, int nSecond);

   static BOOL GetDaysMonth(int nMonth, int nYear);   
   static BOOL IsDateValid(int nYear, int nMonth, int nDay);
   static inline int CDateTime::DaysInYear(int nYear);
   static inline BOOL CDateTime::IsLeapYear(int nYear);    
   static inline int CDateTime::LeapDays(int nYear);


//
// Internal functions
//

protected:
             
   BOOL LongAsTime(long lTime, int* pnHour, int* pnMinute, int* pnSecond);   
   BOOL TimeAsLong(int nHour, int nMinute, int nSecond, long* pnTime);            
   long DateCompare(long lDate0, long lDate1, long lTime0, long lTime1);   
   BOOL ExcelYear(LPCSTR sDate, int* nYear);

   static void JulAsDate(int nYear, int nJul, int &nMonth, int &nDay);
   static int GetJulDay(int nYear, int nMonth, int nDay);   
   static BOOL LongAsDate(long lDate, int* pnYear, int* pnMonth, int* pnDay);   
   static BOOL DateAsLong(int nYear, int nMonth, int nDay, long* pnDate);
   static BOOL DateAddDays(long* plDate, long lDays);
   static long DateDiff(long lDate0, long lDate1);         

};

#endif
                   
///////////////////////////////////////////////////////////////////////////////
