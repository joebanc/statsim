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
#include "nrdbpro.h"
#include "projctns.h"
#include "bdimportexport.h"
#include "dlgprojection.h"
#include "definitions.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////
//
// Internal parameters
//
#undef PI
#define PI 3.14159265358979323844
#define EARTHRADIUS 6378137 // radius at equator in metres
#define UNITSFILE "units.xml"
#define UNIT_LENGTH "length"
#define UNIT_AREA "area"

////////////////////////////////////////////////////////////////////////////////
//
// Additional, non intrinsic/library maths functions
//
#define cotan(x) (1.0/tan(x))

inline double square(double n) {return n*n;};
inline double pow2(double n) {return n*n;};
inline double pow3(double n) {return pow2(n)*n;}
inline double pow4(double n) {return pow2(n)*pow2(n);}
inline double pow5(double n) {return pow2(n)*pow3(n);}
inline double pow6(double n) {return pow3(n)*pow3(n);}
inline double pow7(double n) {return pow3(n)*pow4(n);}

/////////////////////////////////////////////////////////////////////////////

CString g_sRegCode;

///////////////////////////////////////////////////////////////////////////////

ELLIPSOID CEllipsoid::m_aEllipsoids[] = 
{   
   {"UNIVERSAL TRANSVERSE MERCATOR (UTM)",6378206.400, 294.9786982},
   {"LATITUDE / LONGITUDE",LATLON_ID,LATLON_ID},
   {"USER DEFINED",0,0},
   {"Airy (1830)",6377563.396,299.3249647},
   {"US/UK - Modified Airy",6377340.189,299.3249647},
   {"Australian National (1966)",6378160.000,298.2500000},
   {"APL 4.5 (1968)",6378144.000,298.2300000},
   {"Average Terrestrial System 1977", 6378135.000, 298.2570000},
   {"Airy (War Office)",6377542.178, 299.3250000},
   {"Bessel (Modified)",6377492.018, 299.1528000},
   {"Bessel 1841 (Namibia)", 6377483.865, 299.1528128},
   {"US - Bessel 1841 (Ethiopia, Indonesia, Japan, Korea)",6377397.155, 299.1528128},
   {"UK - Bessel (1841) Revised",6377397.155, 299.1528128},
   {"Clarke 1858",6378235.600, 294.2606768},
   {"Clarke 1858 (Modified)",6378293.645, 294.26000000},
   {"Clarke 1866",6378206.400, 294.9786982},
   {"US - Clarke 1880",6378249.145, 293.4650000},
   {"Clarke 1880 (Cape)",6378249.145, 293.4663077},
   {"Clarke 1880 (Palestine)",6378300.782, 293.4663077},
   {"Clarke 1880 (IGN)",6378249.200, 293.4660208},
   {"Clarke 1880 (Syria)",6378247.842, 293.4663517},
   {"Clarke 1880 (Fiji)",6378301.000, 293.4650000},
   {"Danish (1876) or Andrae",6377104.430, 300.0000000},
   {"Delambre 1810",6376985.228, 308.6400000},
   {"Delambre (Carte de France)",6376985.000, 308.6400000},
   {"US - Everest (India 1830)",6377276.345, 300.8017000},
   {"UK - Everest (1830)",6377276.345, 300.8017000},
   {"US - Everest (Brunei and E. Malaysia (Sabah and Sarawak))",6377298.556, 300.8017000},
   {"UK - Everest (Borneo)",6377298.556, 300.8017000},
   {"US - Everest (India 1956)",6377301.243, 300.8017000},
   {"UK - Everest (India)",6377301.243,300.8017255},
   {"US - Everest (W. Malaysia 1969)",6377295.664, 300.8017000},
   {"UK - Everest (Malaya RSO)",6377295.664, 300.8017000},
   {"US - Everest (W. Malaysia and Singapore 1948)",6377304.063, 300.8017000},
   {"UK - Everest (Malaya RKT)",6377304.063, 300.8017000},
   {"Everest (Pakistan)",6377309.613, 300.8017000},
   {"US - Modified Fischer 1960 (South Asia)",6378155.000, 298.3000000},
   {"UK - Fischer 1960 (South Asia)",6378155.000, 298.3000000},
   {"Fischer 1968",6378150.000, 298.3000000},
   {"Fischer 1960 (Mercury)",6378166.000, 298.3000000},
   {"Germaine (Djibouti)",6378284.000, 294.0000000},
   {"Hayford 1909",6378388.000, 296.9592630},
   {"Helmert 1906",6378200.000, 298.3000000},
   {"Hough 1960",6378270.000, 297.0000000},
   {"IAG Best Estimate 1975",6378140.000, 298.2570000},
   {"Indonesian National (1974)",6378160.000, 298.2470000},
   {"US - International 1924",6378388.000, 297.0000000},
   {"UK - International",6378388.000, 297.0000000},
   {"Krassovsky (1940)",6378245.000, 298.3000000},
   {"Krayenhoff 1827",6376950.400, 309.6500000},
   {"NWL-8E",6378145.000, 298.2500000},
   {"Plessis Modified",6376523.000, 308.6400000},
   {"Plessis Reconstituted",6376523.994, 308.6248070},
   {"Geodetic Reference System 1967",6378160.000, 298.2471674},
   {"Geodetic Reference System 1980",6378137.000, 298.2572221},
   {"South American",6378160.000, 298.2500000},
   {"Soviet Geodetic System 1985",6378136.000, 298.2570000},
   {"Soviet Geodetic System 1990",6378136.000, 298.2578393},
   {"Struve 1860",6378298.300, 294.7300000},
   {"Svanberg",6376797.000, 304.2506000},
   {"Walbeck 1819 (Planheft 1942)",6376895.000, 302.7821565},
   {"Walbeck 1819 (AMS 1963)",6376896.000, 302.7800000},
   {"World Geodetic System 1966",6378145.000, 298.2500000},
   {"World Geodetic System 1972",6378135.000, 298.2600000},
   {"World Geodetic System 1984",6378137.000, 298.2572236},
   {"US - War Office 1924 (McCaw)",6378300.000, 296.0000000},
   {"UK - War Office 1924",6378300.000, 296.0000000},   
   {"World Geodetic System 1960",6378165.000, 298.3000000}    
};

////////////////////////////////////////////////////////////////////////////////

double CEllipsoid::GetSemiMinorAxis()
{
   double F, ESQ, EPS;
      
   double ER = m_dSemiMajorAxis;
   double RF = m_dRF;

   F=1.0/RF;
   ESQ=(F+F-pow(F,2));
   EPS=ESQ/(1.0-ESQ);
   return (1.0-F)*ER;      
}

///////////////////////////////////////////////////////////////////////////////

BOOL CProjection::IsUTMOld() 
{
   return fabs(m_dSemiMajorAxis - UTM_ID_OLD) < 0.001;
}

///////////////////////////////////////////////////////////////////////////////

BOOL CProjection::IsLatLon()
{
   return fabs(m_dSemiMajorAxis - LATLON_ID) < 0.001;
}

///////////////////////////////////////////////////////////////////////////////

CProjection::CProjection()
{
   m_lScaleCoords = 1;
}

///////////////////////////////////////////////////////////////////////////////
//
// Reloads the projection from the database
//

BOOL CProjection::InitialiseProjection(BDHANDLE hConnect)
{        
   BOOL bDefault = FALSE;
     
   if (hConnect == NULL) 
   {
      bDefault = TRUE;
      hConnect = BDHandle();
   };

   do
   {

      // Determine the default projection

      CBDProjection projection;

      BOOL bFound = BDProjection(hConnect, &projection, BDGETINIT);
      while (bFound)
      {   
         if (projection.m_bDefault) 
         {                        
            (CBDProjection&)*this = projection;

            // If projection is UTM then update parameters (backwards compatibility)

            if (((CProjection&)projection).IsUTMOld())
            {
               projection.m_dSemiMajorAxis = CEllipsoid::m_aEllipsoids[UTM_INDEX].m_dSemiMajorAxis;               
               projection.m_dSemiMinorAxis = ((CEllipsoid&)(CEllipsoid::m_aEllipsoids[UTM_INDEX])).GetSemiMinorAxis();
               projection.m_dFalseEasting = 0;
               projection.m_dFalseEasting = 500000;
               projection.m_dScaleFactorAtOrigin = UTM_SCALEFACTOR;

               // Update

               BDEnd(hConnect);
               BDProjection(hConnect, &projection, BDUPDATE);
            }

            break;
         };
         bFound = BDGetNext(hConnect);
      }
      BDEnd(hConnect);   

      // If no default projection is defined then display dialog

      if (m_nID == 0)
      {
         if (bDefault)
         {
            if (AfxMessageBox(IDS_NOPROJECTION, MB_YESNO|MB_DEFBUTTON1) == IDYES)
            {
               CDlgProjection dlg;
               dlg.DoModal();	   

               // If the projection is latitude/longitude then set the default display

               if (IsLatLon())
               {
                  BDGetSettings().m_bCoordAsLatLon = TRUE;
               }
               
               // Prevent message re-appearing
               bDefault = FALSE; 
            } else
            {
               return FALSE;
            }
         } else
         {
            return FALSE;
         }
      }
   } while (m_nID == 0);

   // Optimisation: Pre-calculate parameters

   if (m_nID != 0)
   {
      m_a = m_dSemiMajorAxis * m_dScaleFactorAtOrigin;
      m_b = m_dSemiMinorAxis *  m_dScaleFactorAtOrigin;   
      m_e2 = ((m_a * m_a) - (m_b * m_b)) / (m_a * m_a);
      m_N = (m_a - m_b) / (m_a + m_b);
      m_pow2N = m_N*m_N;
      m_pow3N = m_N*m_pow2N;
   }


   return TRUE;
};

///////////////////////////////////////////////////////////////////////////////
//
// Projects latlong using transmercator projection, or for data stored as lat
// long multiplies by a scale factor (to retain decimal placed in integer format)
//
//

void CProjection::LatLonToTransMercator(double latitude, double longitude, 
                              long *easting, long *northing)
{
   double dEasting, dNorthing;
   LatLonToTransMercator(latitude, longitude, &dEasting, &dNorthing);
   *easting = (long)(dEasting+0.5);
   *northing = (long)(dNorthing+0.5);
}

void CProjection::LatLonToTransMercator(double latitude, double longitude, 
                              double *easting, double *northing)
{
   if (IsNullDouble(latitude) || IsNullDouble(longitude))
   {
      *easting = NULL_DOUBLE;
      *northing = NULL_DOUBLE;
   }
   else if (IsLatLon())
   {
      *easting = (long)(longitude*LATLONSCALE);
      *northing = (long)(latitude*LATLONSCALE);
   } 
   else
   {
      LatLonToTransMercatorInt(latitude, longitude, easting, northing);
   }
}

///////////////////////////////////////////////////////////////////////////////
//
// Converts from latitude longitude in decimal degrees to easting and northing
// using a tranverse mercator projection with the parameters defined in this
// class

void CProjection::LatLonToTransMercatorInt(double latitude, double longitude, 
                           double *easting, double *northing)
{
   double phi,v,rho,n2,phi1,M,P,IV,VI,I,II,III,IIIA;   
   double sinphi, cosphi, tanphi;
   double pow2sinphi, pow2cosphi, pow3cosphi, pow5cosphi, pow2tanphi, pow4tanphi;
   double pow2P, pow3P, pow4P, pow5P, pow6P;

// Initialise
      
   phi = latitude * PI / 180.0;

   // Optimisations

   sinphi = sin(phi);
   cosphi = cos(phi);
   tanphi = tan(phi);
   pow2sinphi = sinphi*sinphi;
   pow2cosphi = cosphi*cosphi;
   pow3cosphi = pow2cosphi*cosphi;
   pow5cosphi = pow3cosphi*pow2cosphi;
   pow2tanphi = tanphi*tanphi;
   pow4tanphi = pow2tanphi*pow2tanphi;
   
   v = m_a / sqrt(1.0 - m_e2 * (pow2sinphi));
   rho = (v * (1.0 - m_e2)) / (1.0 - m_e2 * pow2sinphi);
   n2 = (v / rho) - 1.0;
   phi1 = m_dLatitudeOrigin * PI / 180.0;   
   M = m_b * (((1.0 + m_N + ((m_pow2N) * 5.0 / 4.0) + ((m_pow3N) * 5.0 / 4.0)) * (phi - phi1)) 
        - ((3.0 * m_N + 3.0 * (m_pow2N) + (m_pow3N) * 21.0 / 8.0) * sin(phi - phi1) * cos(phi + phi1)) 
        + ((((m_pow2N) * 15.0 / 8.0) + ((m_pow3N) * 15.0 / 8.0)) * (sin(2.0 * (phi - phi1))) * (cos(2.0 * (phi + phi1)))) 
        - (((m_pow3N) * 35.0 / 24.0) * (sin(3.0 * (phi - phi1))) * (cos(3.0 * (phi + phi1)))));
   P = (longitude - m_dLongitudeOrigin) * PI / 180;

   // Optimisations

   pow2P = P*P;
   pow3P = pow2P*P;
   pow4P = pow2P*pow2P;
   pow5P = pow4P*P;
   pow6P = pow2P*pow4P;
  
    // Calculate northing
                            
   I = M + m_dFalseNorthing;
   II = (v / 2.0) * sinphi * cosphi;
   III = (v / 24.0) * sinphi * (pow3cosphi) * (5.0 - (pow2tanphi) + 9.0 * n2);
   IIIA = (v / 720.0) * sinphi * (pow5cosphi) * (61.0 - 58.0 * (pow2tanphi) + (pow4tanphi));

   *northing = I + ((pow2(P)) * II) + ((pow4P) * III) + ((pow6P) * IIIA);

   // Calculating easting

   IV = v * cosphi;
   v = (v / 6.0) * (pow3cosphi) * ((v / rho) - (pow2tanphi));
   VI = (v / 120.0) * (pow5cosphi) * (5.0 - 18.0 * (pow2tanphi) + (pow4tanphi) + 14.0 * n2 - 58.0 * (pow2tanphi) * n2);
   VI = (v / 120.0) * (pow5cosphi) * (5.0 - 18.0 * (pow2tanphi) + (pow4tanphi) + 14 * n2 - 58.0 * (pow2tanphi) * n2);
 
   *easting = m_dFalseEasting + (P * IV) + ((pow3P) * v) + ((pow5P) * VI);

}

///////////////////////////////////////////////////////////////////////////////

void CProjection::TransMercatorToLatLon(double easting, double northing,
                              double *latitude, double *longitude)
{
   if (IsNullDouble(easting) || IsNullDouble(northing))
   {
      *latitude = NULL_DOUBLE;
      *longitude = NULL_DOUBLE;
   }
   else if (IsLatLon())
   {
      *latitude = northing/LATLONSCALE;
      *longitude = easting/LATLONSCALE;
   } else
   {
      TransMercatorToLatLonInt(easting,northing,latitude,longitude);
   }
}

///////////////////////////////////////////////////////////////////////////////
//
// Converts from easting and northings to latitude longitude in decimal degrees 
// using a tranverse mercator projection with the parameters defined in this
// class


void CProjection::TransMercatorToLatLonInt(double easting, double northing,
                           double *latitude, double *longitude)
{
   double phiDash, phi, phi1, M;
   double v, rho, n2, VII, VIII, IX, Et;
   double X, XI, XII, XIIA;

   // Initialise parameters
      
   // Calculate latitude

   phiDash = ((northing - m_dFalseNorthing) / m_a) + (m_dLatitudeOrigin * PI / 180);
   phi = phiDash;
   phi1 = (m_dLatitudeOrigin * PI / 180.0);
            
   M = m_b * (((1.0 + m_N + ((m_pow2N) * 5.0 / 4) + ((m_pow3N) * 5.0 / 4.0)) * (phi - phi1))
       - ((3.0 * m_N + 3.0 * (m_pow2N) + (m_pow3N) * 21.0 / 8.0) * sin(phi - phi1) * cos(phi + phi1))
       + ((((m_pow2N) * 15.0 / 8.0) + ((m_pow3N) * 15.0 / 8.0)) * (sin(2 * (phi - phi1))) * (cos(2.0 * (phi + phi1))))
       - (((m_pow3N) * 35.0 / 24.0) * (sin(3.0 * (phi - phi1))) * (cos(3.0 * (phi + phi1)))));
      
   while ((northing - m_dFalseNorthing - M) > 0.001)
   {
       phi = phi + ((northing - m_dFalseNorthing - M) / m_a);
       M = m_b * (((1 + m_N + ((m_pow2N) * 5.0 / 4.0) + ((m_pow3N) * 5.0 / 4.0)) * (phi - phi1)) 
           - ((3.0 * m_N + 3.0 * (m_pow2N) + (m_pow3N) * 21.0 / 8.0) * sin(phi - phi1) * cos(phi + phi1)) 
           + ((((m_pow2N) * 15.0 / 8.0) + ((m_pow3N) * 15.0 / 8.0)) * (sin(2.0 * (phi - phi1))) * (cos(2 * (phi + phi1)))) 
           - (((m_pow3N) * 35.0 / 24.0) * (sin(3.0 * (phi - phi1))) * (cos(3.0 * (phi + phi1)))));
   };

   v = m_a / sqrt(1.0 - m_e2 * (pow2((sin(phi)))));
   rho = (v * (1.0 - m_e2)) / (1 - m_e2 * (pow2(sin(phi))));
   n2 = (v / rho) - 1.0;
   VII = (tan(phi)) / (2.0 * rho * v);
   VIII = ((tan(phi)) / (24.0 * rho * (pow3(v)))) * (5.0 + (3.0 * (pow2(tan(phi)))) + n2 - (9.0 * n2 * (pow2(tan(phi)))));
   IX = (tan(phi) / (720.0 * rho * (pow5(v)))) * (61.0 + (90.0 * (pow2(tan(phi)))) + (45.0 * (pow4(tan(phi)))));
   Et = easting - m_dFalseEasting;
   *latitude = (phi - ((pow2(Et)) * VII) + ((pow4(Et)) * VIII) - ((pow6(Et)) * IX)) * 180.0 / PI;

   // Calculate longitude
 
    X = ((1 / cos(phi)) / v);
    XI = ((1 / cos(phi)) / (6.0 * (pow3(v)))) * ((v / rho) + 2.0 * (pow2(tan(phi))));
    XII = ((1 / cos(phi)) / (120.0 * (pow5(v)))) * (5.0 + (28.0 * (pow2(tan(phi)))) + (24.0 * (pow4(tan(phi)))));
    XIIA = ((1.0 / cos(phi)) / (5040.0 * (pow7(v)))) * (61.0 + (662.0 * (pow2(tan(phi)))) + (1320.0 * (pow4(tan(phi)))) + (720 * (pow6(tan(phi)))));    
    *longitude = m_dLongitudeOrigin + ((Et * X) - ((pow3(Et)) * XI) + ((pow5(Et)) * XII) - ((pow7(Et)) * XIIA)) * 180.0 / PI;
}

////////////////////////////////////////////////////////////////////////////////
//
// Converts a string of the format DD::MM::SS NSEW to a decimal number.  The 
// flag indicates if the value is a longitude or latitude value
//

BOOL CProjection::StringAsLatLong(LPCSTR sText, double* pdValue, int iFlag)
{                                                                    
   int nDeg, nMin;
   double dDeg = 0;
   double dMin = 0, dSec = 0;
   long lSign = 1;   
   char cDir = '\0';
   char* sLatLon;
   CString s;
   
   // Retrieve values from string, nb. because the last number may be
   // a decimal any following 'e' will be interpreted as an exponent

   sLatLon = (LPSTR)sText;
   while (*sLatLon == ' ') sLatLon++;

   if (!BDNext(sLatLon, nDeg) || !BDNext(sLatLon, nMin) ||
       !BDNext(sLatLon, dSec))
   {
      sLatLon = (LPSTR)sText;
      while (*sLatLon == ' ') sLatLon++;

      if (!BDNext(sLatLon, nDeg) || !BDNext(sLatLon, dMin))
      {
        // Add support for decimal degrees on import only

         sLatLon = (LPSTR)sText;
         while (*sLatLon == ' ') sLatLon++;

         if (((CString)sLatLon).Find('.') > 0 && BDNext(sLatLon, dDeg))
         {
            nDeg = (int)dDeg;
            dMin = (dDeg - nDeg)*60;
         } else
         {
            return FALSE;
         }
      }

      nMin = (int)dMin;
      dSec = (dMin-nMin)*60;
   }         
   
   
   // Check direction

   if (BDNext(sLatLon, cDir))
   {
      if (nDeg < 0) return FALSE; // Can't have a sign and a direction

      s = cDir;
      s.MakeUpper();   
      if (s == BDString(IDS_N) && iFlag & latitude) lSign = 1;
      else if (s == BDString(IDS_S) && iFlag & latitude) lSign = -1;
      else if (s == BDString(IDS_E) && iFlag & longitude) lSign = 1;
      else if (s == BDString(IDS_W) && iFlag & longitude) lSign = -1;
      else return FALSE;
   };   

   // Validate

   if (nMin > 60.0 || dSec > 60.0) return FALSE;
   
   // Calculate decimal

   *pdValue = nDeg + nMin/60.0 + dSec/3600.0;      
   *pdValue = *pdValue * lSign;   

   // Validate range

   if (iFlag & latitude == latitude && (double)fabs((double)(*pdValue > 90.0))) return FALSE;
   if (iFlag & longitude == longitude && (double)fabs((double)(*pdValue > 180.0))) return FALSE;
   
   return TRUE;
};

///////////////////////////////////////////////////////////////////////////////

void CProjection::LatLongAsString(double deg, CString& s, int iFlag)
{
   int hour,min;
   double dSec;

   char dir;
   hour = abs((int)deg);
   min = abs((int)((fabs(deg)-hour)*60));
   dSec = fabs((((fabs(deg)-hour)*60)-min)*60);

   if (fabs(dSec - 60) < 0.000001) 
   {
      min += 1;
      dSec = 0;
   }

   if (deg >= 0 && (iFlag & latitude) == latitude) dir = 'N';
   else if (deg < 0 && (iFlag & latitude) == latitude) dir = 'S';
   else if (deg >= 0 && (iFlag & longitude) == longitude) dir = 'E';
   else if (deg < 0 && (iFlag & longitude) == longitude) dir = 'W';
   else ASSERT(FALSE);

   if (iFlag & legend)
   {
      if (iFlag & seconds) s.Format("%d'%d'%.0lf",hour,min,dSec);         
      else s.Format("%d'%d",hour,min);         
   } else
   {
      s.Format("%d'%02d\'%.1lf %c",hour,min,dSec,dir);   
   };
}

///////////////////////////////////////////////////////////////////////////////
//
// Converts a string of the format X,Y where X and Y are either latitude and
// longitude or X and Y coordinates.  If the former then a tranformation 
// is performed
//

int CProjection::StringAsCoord(CString s, CCoord* pCoord)
{
  CLongCoord coord;
  int ret = StringAsCoord(s, &coord);
  if (ret)
  {
     pCoord->x = coord.x;
     pCoord->y = coord.y;
  }
  return ret;
}

///////////////////////////////////////////////////////////////////////////////

int CProjection::StringAsCoord(CString s, CLongCoord* pCoord)
{
   BOOL bOK = TRUE;
   int nRet = 0;

   CString s1, s2;   
   double dLat, dLong;
   char ch;

   // Search for comma or space

   s.TrimLeft();
   int i = s.Find(",");
   if (i == -1) i = s.Find(" ");

   if (i != -1)
   {
      s1 = s.Left(i);
      s2 = s.Mid(i+1);
      s1.TrimRight();
      s2.TrimRight();

      // Check for latitude, longitude

      if (StringAsLatLong(s1, &dLat, latitude) && 
          StringAsLatLong(s2, &dLong, longitude))
      {
         LatLonToTransMercator(dLat, dLong, &pCoord->x, &pCoord->y);         
         nRet = PR_LATLON;
      }
      else
      {
         bOK = sscanf(s1,"%d%c",&pCoord->x, &ch) == 1 &&
               sscanf(s2,"%d%c",&pCoord->y, &ch) == 1;
         nRet = PR_COORD;
      }
   }
   else
   {
      bOK = sscanf(s,"%d %d",&pCoord->x, &pCoord->y) == 2;               
   }

   if (bOK) return nRet;
   else return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
//
// Converts coordinates to latitude longitude and outputs as a string
//

void CProjection::CoordAsString(CCoord coord, CString& s)
{
   double dLat, dLong;
   CString sLat, sLong;

   if (BDGetSettings().m_bCoordAsLatLon)
   {
      TransMercatorToLatLon(coord.x, coord.y, &dLat, &dLong);   
      LatLongAsString(dLat, sLat, latitude);
      LatLongAsString(dLong, sLong, longitude);
      s = sLat + ", " + sLong;
   } else
   {
      s.Format("%.0lf, %.0lf", coord.x, coord.y);
   }
}

///////////////////////////////////////////////////////////////////////////////
//
// Converts a bearing in the form:
// 12N 37E 123.7 
// To a relative coordinate in metres
//

BOOL CProjection::StringAsBearing(CString sT, CLongCoord &coordL)
{
   CCoord coord;
   if (StringAsBearing(sT, coord))
   {
      coordL.x = (long)(coord.x + 0.5); 
      coordL.y = (long)(coord.y + 0.5);

      return TRUE;
   } else
   {
      return FALSE;
   }
}

///////////////////////////////////////////////////////////////////////////////

BOOL CProjection::StringAsBearing(CString s, CCoord &coord)
{
   BOOL bOK = TRUE;

   s.TrimLeft();
   int nDeg, nMin;
   double dD, dAngle;
   char cN, cE, c;
   BOOL bN = FALSE;
   BOOL bE = FALSE;

   if (sscanf(s, "%d%c%d %c%c %lf", &nDeg, &c, &nMin, &cN, &cE, &dD) == 6 && 
       (cN == 'N' || cN == 'n' || cN == 's' || cN == 'S'))
   {            
      // Determine if +ve or -ve

      if (cN == 'N' || cN == 'n')
      {
         bN = TRUE;
      }
      else if (cN == 'S' || cN == 's')
      {
         bN = FALSE;
      }
      else 
      {
         bOK = FALSE;
      }

      // Determine if +ve or -ve

      if (cE == 'E' || cE == 'e')
      {
         bE = TRUE;
      }
      else if (cE == 'W' || cE == 'w')
      {
         bE = FALSE;
      }
      else 
      {
         bOK = FALSE;
      }

      // Determine angle

      dAngle = nDeg + nMin/60;
      if (!bN && bE) dAngle = 90 + 90 - dAngle;
      if (!bN && !bE) dAngle = 180 + dAngle;
      if (bN && !bE) dAngle = 270 + 90 - dAngle;
      dAngle = dAngle/90.0 * PI/2;

      // Determine bearing

      coord.y = cos(dAngle) * dD;
      coord.x = sin(dAngle) * dD;
           
   } else
   {
      bOK = FALSE;
   }

   return bOK;
}

///////////////////////////////////////////////////////////////////////////////

CString CProjection::GetProjectionName()
{
   return this->m_sName;
}

CBDProjection& CProjection::GetProjection()
{
   return (CBDProjection&)*this;
}

///////////////////////////////////////////////////////////////////////////////
//
// Return distance between coordinates in metres
//
// www.mathforum.org
//
// cos(AOB) = cos(dLatA)cos(dLatB)cos(lonB-lonA)+sin(dLatA)sin(dLatB)
//
// This gives AOB, and the great circle distance between A and
// B will be
//               R*(AOB)     with AOB in radians.
//
// NOTE: Treats earth as sphere not elipsoid!
//

double CProjection::GetDistance(CLongCoord coord1, CLongCoord coord2, BOOL bDefaultUnits)
{
   // For tranverse mercator this is simple as in metres

  if (!IsLatLon())
  {     
     // Force use of floating point arithmetic to remove rounding errors

     double dDist = sqrt(square(coord2.x - coord1.x) + square(coord2.y - coord1.y));
     if (!bDefaultUnits) dDist *= BDGetApp()->GetUnits().GetLengthUnit().m_dScaleFactor;
     return dDist;
  } 
  
  // Calculate for lat/long coordinates

  else
  {  
    // Convert to latitude/longitude
     
     double dLat1, dLon1, dLat2, dLon2;
     TransMercatorToLatLon(coord1.x, coord1.y, &dLat1, &dLon1);
     TransMercatorToLatLon(coord2.x, coord2.y, &dLat2, &dLon2);

     // Convert to radians

     dLat1 = dLat1 / 180 * PI;
     dLon1 = dLon1 / 180 * PI;
     dLat2 = dLat2 / 180 * PI;
     dLon2 = dLon2 / 180 * PI;

     double c = acos( (cos(dLon1-dLon2)-1)*cos(dLat1)*cos(dLat2) + cos(dLat1-dLat2) );

     // Determine radius for this latitude

     double r = EARTHRADIUS * (1 - 0.0033493 * square(sin((dLat1+dLat2)/2)));
          
     double dDist = (r * c);
     if (!bDefaultUnits) dDist *= BDGetApp()->GetUnits().GetLengthUnit().m_dScaleFactor;
     return dDist;

  }
}

///////////////////////////////////////////////////////////////////////////////

void CUnits::LoadUnits()
{
   CXMLFile xmlfile;

   if (xmlfile.Read(BDGetAppPath() + UNITSFILE))
   {
      XMLAs(&xmlfile);
   }
};

///////////////////////////////////////////////////////////////////////////////

void CUnits::SaveUnits()
{
   CXMLFile xmlfile;
   AsXML(&xmlfile);

   xmlfile.Write(BDGetAppPath() + UNITSFILE);
}

///////////////////////////////////////////////////////////////////////////////

void CUnits::XMLAs(CXMLObj* pXMLObj)
{
   CXMLObj *pXMLChild;

   RemoveAll();
   BOOL bFound = pXMLObj->GetFirst("units", "unit", pXMLChild);
   while (bFound)
   {
      CUnit unit;
      unit.m_bDefault = pXMLChild->GetAttr("default") == "1";
      unit.m_dScaleFactor = pXMLChild->GetDouble("", "scalefactor");
      unit.m_sAbbr = pXMLChild->GetString("", "abbreviation");
      unit.m_sName = pXMLChild->GetString("", "name");
      if (pXMLChild->GetString("", "type") == "length") unit.m_nType = CUnits::length;
      if (pXMLChild->GetString("", "type") == "area") unit.m_nType = CUnits::area;

      Add(unit);

      bFound = pXMLObj->GetNext("units", "unit", pXMLChild);
   }
}

///////////////////////////////////////////////////////////////////////////////

void CUnits::AsXML(CXMLObj* pXMLObj)
{
   pXMLObj->SetName("units");

   int i = 0; for (i = 0; i < GetSize(); i++)
   {
      CUnit &unit = GetAt(i);
      CXMLObj *pXMLChild = pXMLObj->AddChild("unit");

      if (unit.m_bDefault) pXMLChild->SetAttr("default", "1");
      pXMLChild->SetValue("", "scalefactor", unit.m_dScaleFactor);
      pXMLChild->SetValue("", "abbreviation", unit.m_sAbbr);
      pXMLChild->SetValue("", "name", unit.m_sName);
      if (unit.m_nType == CUnits::length)  pXMLChild->SetValue("", "type", "length");
      else if (unit.m_nType == CUnits::area) pXMLChild->SetValue("", "type", "area");

   };
}

///////////////////////////////////////////////////////////////////////////////
//
// Set the default unit for either length or area
//


void CUnits::SetDefault(int nType, LPCSTR sName)
{
   int i = 0; for (i = 0; i < GetSize(); i++)
   {
      if (GetAt(i).m_nType == nType)
      {
         if (GetAt(i).m_sName == sName)
         {
            m_pData[i].m_bDefault = TRUE;
         } else
         {
            m_pData[i].m_bDefault = FALSE;
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

CUnit CUnits::GetAreaUnit()
{
   // Search through list of units for default one

   int i = 0; for (i = 0; i < GetSize(); i++)
   {
      if (GetAt(i).m_bDefault && GetAt(i).m_nType == area) return GetAt(i);
   }

   // If no default area unit found then create one

   CUnit unit;
   unit.m_sAbbr = BDString(IDS_SQMETERS);
   return unit;
}

///////////////////////////////////////////////////////////////////////////////

CUnit CUnits::GetLengthUnit()
{
   // Search through list of units for default one

   int i = 0; for (i = 0; i < GetSize(); i++)
   {
      if (GetAt(i).m_bDefault && GetAt(i).m_nType == length) return GetAt(i);
   }

   // If no default length unit found then create one

   CUnit unit;
   unit.m_sAbbr = BDString(IDS_METERS);
   return unit;
}
