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
#include <io.h>
#include <strstream>

#include "nrdb.h"
#include "editplus.h"
#include "definitions.h"

///////////////////////////////////////////////////////////////////////////////
//
// void CEditPlus::SetValue()
//

void CEditPlus::SetValue(double dValue, int nDP)
{
   std::strstream sValue;                                                          
           
   if (dValue != NULL_DOUBLE)
   {      
      if (nDP != -1) sValue.precision(nDP);       
      sValue.flags(std::ios::fixed);
      sValue << dValue << std::ends;
      SetWindowText(sValue.str());      
      sValue.rdbuf()->freeze(0);      
   } else
   {  
      SetWindowText("");
   }
}

void CEditPlus::SetValue(long lValue)
{
   std::strstream sValue;                                                          
      
   if (lValue != NULL_LONG)
   {
      sValue.precision(10);
      sValue << lValue << std::ends;                                                  
      SetWindowText(sValue.str());
      sValue.rdbuf()->freeze(0);
   } else
   {
      SetWindowText("");
   }
}

void CEditPlus::SetValue(CDateTime datetime)
{
   CString sDate;
   datetime.DateAsString(sDate);
   SetWindowText(sDate);   
};

void CEditPlus::SetValue(int nValue)
{
   std::strstream sValue;                                                          
      
   if (nValue != NULL_INT)
   {
      sValue.precision(10);
      sValue << nValue << std::ends;  
      SetWindowText(sValue.str());
      sValue.rdbuf()->freeze(0);
   } else
   {
      SetWindowText("");
   }
}

///////////////////////////////////////////////////////////////////////////////
//
// BOOL CEditPlus::GetValue()
//


BOOL CEditPlus::GetValue(double& dValue, double dMin, double dMax, BOOL bNull)                       
{
   CString str;                                                                  
   BOOL bOK = TRUE;
   char ch;
 
// If window is disabled then do not retrieve value

   if (!IsWindowEnabled())
   {
      return bOK;
   }

   if (IsEmpty())
   {
      if (!bNull)
      {
         SetFocus();                                                                         
         SetSel(0,-1);                                   
         AfxMessageBox(BDString(IDS_NOVALUE) + ": " + GetLabel());
         bOK = FALSE;
      } else
      {
         dValue = NULL_DOUBLE;
         return TRUE;
      }
   } 

 // Convert the string to an integer value

   else
   {
      GetWindowText(str);                                       
           
      if (sscanf(str,"%lf%c",&dValue,&ch) != 1)
      {
         SetFocus();                                                                         
         SetSel(0,-1);                                   
         AfxMessageBox(BDString(IDS_INVALIDVALUE) + ": " + str);
         bOK = FALSE;
      }
   }
   
 // Check if value within range
 
   if (bOK && (dValue <= dMin || dValue >= dMax))
   {  
      std::strstream str;
      str.precision(10);
      SetFocus();                                                                         
      SetSel(0,-1);                                   
      bOK = FALSE;

      if (dMax == DBL_MAX)
      {         
         str << BDString(IDS_GREATERTHAN) + " " << dMin << std::ends;         
      } else
      {
         str << BDString(IDS_INRANGE) + " " << dMin << " to " << dMax << std::ends;
      }  
      AfxMessageBox(str.str());      
      str.rdbuf()->freeze(0);
   };
                        
   return bOK;
}

///////////////////////////////////////////////////////////////////////////////
//
// BOOL CEditPlus::GetValue(long&, long lMin, long lMax, BOOL bNull)
//

BOOL CEditPlus::GetValue(long& lValue, long lMin, long lMax, BOOL bNull)
{
   CString str;                                                               
   BOOL bOK = TRUE;
   char ch;

   // If window is disabled then do not retrieve value

   if (!IsWindowEnabled())
   {
      return bOK;
   }
 
   if (IsEmpty())
   {
      if (!bNull)
      {
         SetFocus();                                                                         
         SetSel(0,-1);                                   
         AfxMessageBox(BDString(IDS_NOVALUE) + ": " + GetLabel());
         bOK = FALSE;
      } else
      {
         lValue = NULL_LONG;
         return TRUE;
      }
   } 

 // Convert the string to an integer value

   else
   {
      GetWindowText(str);                                             
      if (sscanf(str,"%li%c",&lValue,&ch) != 1)
      {
         SetFocus();  
         SetSel(0,-1);                                   
         AfxMessageBox(BDString(IDS_INVALIDVALUE) + ": " + str);
         bOK = FALSE;
      }

   }
                                                                                 
 // Check if value within range
 
   if (bOK && (lValue < lMin || lValue > lMax))
   {          
      std::strstream str;
      str.precision(10);
      SetFocus();                                                                         
      SetSel(0,-1);                                   

      if (lMax == LONG_MAX)
      {
         str << BDString(IDS_GREATEREQUAL) + " " << lMin << std::ends;         
      } else
      {
         str << BDString(IDS_INRANGE) + " " << lMin << " " + BDString(IDS_TO) + " " << lMax << std::ends;
      }
            
      AfxMessageBox(str.str());      
      str.rdbuf()->freeze(0);
      bOK = FALSE;
   };
   
         
   return bOK;
}

///////////////////////////////////////////////////////////////////////////////
//
// BOOL CEditPlus::GetValue(int& nValue, int nMin, int nMax, BOOL bNull)
//

BOOL CEditPlus::GetValue(int& nValue, int nMin, int nMax, BOOL bNull)
{
   CString str;                                                               
   BOOL bOK = TRUE;
   char ch;
 
   // If window is disabled then do not retrieve value

   if (!IsWindowEnabled())
   {
      return bOK;
   }

   if (IsEmpty())
   {
      if (!bNull)
      {
         SetFocus();                                                                         
         SetSel(0,-1);                                   
         AfxMessageBox(BDString(IDS_NOVALUE) + ": " + GetLabel());
         bOK = FALSE;
      } else
      {
         nValue = NULL_INT;
         return TRUE;
      }
   } 

 // Convert the string to an integer value

   else
   {
      GetWindowText(str);                                             
      if (sscanf(str,"%d%c",&nValue,&ch) != 1)
      {
         SetFocus();  
         SetSel(0,-1);                                   
         AfxMessageBox(BDString(IDS_INVALIDVALUE) + ": " + str);
         bOK = FALSE;
      }

   }
                                                                                 
 // Check if value within range
 
   if (bOK && (nValue < nMin || nValue > nMax))
   {          
      std::strstream str;
      str.precision(10);
      SetFocus();                                                                         
      SetSel(0,-1);                                   

      if (nMax == INT_MAX)
      {
         str << BDString(IDS_GREATEREQUAL) + " " << nMin << std::ends;         
      } else
      {
         str << BDString(IDS_INRANGE) + " " << nMin << " to " << nMax << std::ends;
      }                  
      AfxMessageBox(str.str());      
      str.rdbuf()->freeze(0);
      bOK = FALSE;
   };
   
         
   return bOK;
}

///////////////////////////////////////////////////////////////////////////////
//
// BOOL CEditPlus::GetValue(CDateTime&)
//

BOOL CEditPlus::GetValue(CDateTime& rDateTime, CDateTime dtMin, BOOL bNull)
{
   CString str;
   char sDate[32], sTime[32];
   BOOL bOK = TRUE;
   
   GetWindowText(str);
   
   sscanf(str, "%32s %32s",sDate,sTime);

   CDateTime datetime;

// If window is disabled then do not retrieve value

   if (!IsWindowEnabled())
   {
      return bOK;
   }

// Determine if date is valid and if an empty date is allowed   

   if (IsEmpty())
   {
      if (!bNull)
      {
         SetFocus();                                                                         
         SetSel(0,-1);                                   
         AfxMessageBox(BDString(IDS_NOVALUE) + ": " + GetLabel());
         bOK = FALSE;
      } else
      {
         rDateTime = CDateTime();                 
         return TRUE;
      }
   } 

// Convert the string to a date

   else if (!datetime.StringAsDateTime(sDate,sTime))
   {  
      SetFocus();      
      SetSel(0,-1);         
      AfxMessageBox(BDString(IDS_DATEFORMAT) + ": " + str);      
      bOK = FALSE;
   }      

// Check range

   if (bOK && datetime < dtMin)
   {
      CString sDate;
      dtMin.DateAsString(sDate);
      AfxMessageBox(BDString(IDS_DATEAFTER) + ": " + sDate);
      return FALSE;
   };

   if (bOK)
   {
      rDateTime = datetime;
   }
   return bOK;
}

///////////////////////////////////////////////////////////////////////////////
//
// BOOL CEditPlus::GetValue(CString&)
//

BOOL CEditPlus::GetValue(CString& rString, BOOL bNull)
{
   BOOL bOK = TRUE;

// If window is disabled then do not retrieve value

   if (!IsWindowEnabled())
   {
      return bOK;
   }

   GetWindowText(rString);
   if (IsEmpty() && !bNull)
   {
      SetFocus();      
      SetSel(0,-1);    
      AfxMessageBox(BDString(IDS_NOVALUE) + ": " + GetLabel());
      bOK = FALSE;
   }

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////
//
// BOOL CEditPlus::IsEmpty()
//

BOOL CEditPlus::IsEmpty()
{
   CString str;
   BOOL bEmpty = TRUE;

   GetWindowText(str);
            
   int i = 0;

   while (i < str.GetLength() && bEmpty)
   {
     if (str[i] != ' ')
         bEmpty = FALSE;
          
     i++;
   } // end while            
   return bEmpty;
};

///////////////////////////////////////////////////////////////////////////////
//
// BOOL CEditPlus::GetPath(CString&)
//

BOOL CEditPlus::GetPath(CString& rString)
{
   GetWindowText(rString);
   BOOL bOK = TRUE;
   
   rString.TrimRight();

    // Check if string is empty

   if (IsEmpty())
   {
      SetFocus();      
      SetSel(0,-1);    
      AfxMessageBox(BDString(IDS_NOVALUE) + ": " + GetLabel());
      return FALSE;
   }

   // Nb. For handling paths longer than MAX_PATH use GetFileAttributesW

   // Check for files existence, this is necessary as GetFileAttributes 
   // seems to think that everything is a directory!

   if (_access(rString, 00) != 0)
   {
      AfxMessageBox(IDS_NODIR + ": " + rString);     
      bOK = FALSE;
   };

   // If the file exists then check whether it is a directory

   if (bOK)
   {      
      DWORD dwRet = GetFileAttributes(rString);
      if (dwRet != -1 && !(dwRet & FILE_ATTRIBUTE_DIRECTORY))
      {
         AfxMessageBox(BDString(IDS_NODIR) + ": " + rString);     
         bOK = FALSE;      
      }
   };

  // Display error message if it does not exist

   if (!bOK)
   {
      SetFocus();      
      SetSel(0,-1);          
   };

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////
//
// BOOL CEditPlus::GetFullPath(CString&)
//
// Returns true if the path exists and is not a directory
//

BOOL CEditPlus::GetFullPath(CString& rString)
{
   GetWindowText(rString);
   BOOL bOK = TRUE;
   
   rString.TrimRight();

    // Check if string is empty

   if (IsEmpty())
   {
      SetFocus();      
      SetSel(0,-1);    
      AfxMessageBox(BDString(IDS_NOVALUE) + ": " + GetLabel());
      return FALSE;
   }

   // Nb. For handling paths longer than MAX_PATH use GetFileAttributesW

   // Check for files existence, this is necessary as GetFileAttributes 
   // seems to think that everything is a directory!

   if (_access(rString, 00) != 0)
   {
      AfxMessageBox(BDString(IDS_NOFILE) + ": " + rString);
      bOK = FALSE;
   };

   // If the file exists then check whether it is a directory

   if (bOK)
   {      
      DWORD dwRet = GetFileAttributes(rString);
      if (dwRet != -1 && dwRet & FILE_ATTRIBUTE_DIRECTORY)
      {
         AfxMessageBox(BDString(IDS_NOFILE) + rString);     
         bOK = FALSE;      
      }
   };

  // Display error message if it does not exist

   if (!bOK)
   {
      SetFocus();      
      SetSel(0,-1);          
   };

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////
//
// BOOL CEditPlus::GetFile()
//

BOOL CEditPlus::GetFile(CString& rString, BOOL bNull)
{  
 // Check if string is empty

   if (IsEmpty() && !bNull)
   {
      SetFocus();      
      SetSel(0,-1);    
      AfxMessageBox(BDString(IDS_NOVALUE) + ": " + GetLabel());
      return FALSE;
   }

// Check if no illegal characters for a file name exist

   GetWindowText(rString); 
   if (rString.FindOneOf("\"/\\[]:;=,") == -1)
   {
      return TRUE;
   } else
   {
      SetFocus();      
      SetSel(0,-1);    
      AfxMessageBox(BDString(IDS_NOFILE) + ": " + rString);
      return FALSE;
   }

}

///////////////////////////////////////////////////////////////////////////////
//
// CString CEditPlus::GetLabel()
//
// Retrieves the label for associated with the edit control.  This relies on
// the tab order being set so that labels preceed edit controls
//

CString CEditPlus::GetLabel()
{
   CString sRet;
   CString sLabel;

   CWnd* pWnd = GetWindow(GW_HWNDPREV); 

   char sClassName[128];
   if (GetClassName(pWnd->GetSafeHwnd(), sClassName, sizeof(sClassName)) &&
       strcmp(sClassName, "Static") == 0)
   {
       pWnd->GetWindowText(sRet);
   }

  // Remove asterisks from names

   int i = 0; for (i = 0; i < sRet.GetLength(); i++)
   {
      if (sRet[i] != '*' && sRet[i] != ':') 
      {
         sLabel += sRet[i];
      }
   }
   
   return sLabel;
};