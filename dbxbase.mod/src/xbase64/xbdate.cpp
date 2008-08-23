/*  xbdate.cpp

    Xbase64 project source code

    These functions are used for processing dates.
    All functions assume a standard date format of CCYYMMDD
    for Century,Year,Month and Day

    Copyright (C) 1997,2003  Gary A Kunkel
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


    Contact:
    
     Email:
    
      xdb-devel@lists.sourceforge.net
      xdb-users@lists.sourceforge.net
      
      
     Regular Mail:
     
       XBase Support
       149C South Main St
       Keller Texas, 76248     
       USA

*/

#ifdef __GNU LesserG__
  #pragma implementation "xbdate.h"
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>  
#include <string.h>
#include <time.h>
#ifdef __WIN32__
#include <xbase64/xbwincfg.h>
#else
#include <xbase64/xbconfig.h>
#endif
#include <xbase64/xbase64.h>
#include <xbase64/xbdate.h>
//#include <xbase64/retcodes.h>

/*! \file xbdate.cpp
*/

int xbDate::DaysInMonths[2][13];
int xbDate::AggregatedDaysInMonths[2][13];

#define EPOCH_MIN 100
#define EPOCH_MAX 3000
#define DAYS_AD(year) ((year) *365L + (year) / 4 - (year) / 100 + (year) / 400)

/***************************************************************/
//! Short description.
/*!
  \param Date8
*/
xbDate::xbDate( const xbString & Date8 ) {
  if( DateIsValid( Date8 ))
    cDate8 = Date8;
  else
    Sysdate();
  SetDateTables();
}
/***************************************************************/
//! Short description.
/*!
  \param Date8
*/
xbDate::xbDate( const char * Date8 ) {
  if( DateIsValid( Date8 ))
    cDate8 = Date8;
  else
    Sysdate();        /* if invalid date, set class to sysdate */
  SetDateTables();
}
/***************************************************************/
//! Short description.
/*!
*/
xbDate::xbDate()
{
  Sysdate();
  SetDateTables();
}

/***************************************************************/
//! Destructor
/*!
*/
xbDate::~xbDate()
{
}
/***************************************************************/
//! Short description.
/*!
*/
void xbDate::SetDateTables() {
 if( AggregatedDaysInMonths[1][12] != 366 ){    /* first time called ? */
  AggregatedDaysInMonths[0][0]  = 0;
  AggregatedDaysInMonths[0][1]  = 31;
  AggregatedDaysInMonths[0][2]  = 59;
  AggregatedDaysInMonths[0][3]  = 90;
  AggregatedDaysInMonths[0][4]  = 120;
  AggregatedDaysInMonths[0][5]  = 151;
  AggregatedDaysInMonths[0][6]  = 181;
  AggregatedDaysInMonths[0][7]  = 212;
  AggregatedDaysInMonths[0][8]  = 243;
  AggregatedDaysInMonths[0][9]  = 273;
  AggregatedDaysInMonths[0][10] = 304;
  AggregatedDaysInMonths[0][11] = 334;
  AggregatedDaysInMonths[0][12] = 365;
  AggregatedDaysInMonths[1][0]  = 0; 
  AggregatedDaysInMonths[1][1]  = 31;
  AggregatedDaysInMonths[1][2]  = 60;
  AggregatedDaysInMonths[1][3]  = 91;
  AggregatedDaysInMonths[1][4]  = 121;
  AggregatedDaysInMonths[1][5]  = 152;
  AggregatedDaysInMonths[1][6]  = 182;
  AggregatedDaysInMonths[1][7]  = 213;
  AggregatedDaysInMonths[1][8]  = 244;
  AggregatedDaysInMonths[1][9]  = 274;
  AggregatedDaysInMonths[1][10] = 305;
  AggregatedDaysInMonths[1][11] = 335;
  AggregatedDaysInMonths[1][12] = 366;
  
  DaysInMonths[0][0]  = 0; 
  DaysInMonths[0][1]  = 31;
  DaysInMonths[0][2]  = 28;
  DaysInMonths[0][3]  = 31;
  DaysInMonths[0][4]  = 30;
  DaysInMonths[0][5]  = 31;
  DaysInMonths[0][6]  = 30;
  DaysInMonths[0][7]  = 31;
  DaysInMonths[0][8]  = 31;
  DaysInMonths[0][9]  = 30;
  DaysInMonths[0][10] = 31;
  DaysInMonths[0][11] = 30;
  DaysInMonths[0][12] = 31;
  DaysInMonths[1][0]  = 0; 
  DaysInMonths[1][1]  = 31;
  DaysInMonths[1][2]  = 29;
  DaysInMonths[1][3]  = 31;
  DaysInMonths[1][4]  = 30;
  DaysInMonths[1][5]  = 31;
  DaysInMonths[1][6]  = 30;
  DaysInMonths[1][7]  = 31;
  DaysInMonths[1][8]  = 31;
  DaysInMonths[1][9]  = 30;
  DaysInMonths[1][10] = 31;
  DaysInMonths[1][11] = 30;
  DaysInMonths[1][12] = 31;
 }
}

/***************************************************************/
//! Short description.
/*!
  \param Date8
*/
/* this function returns century and year from a CCYYMMDD date */
int xbDate::CenturyOf( const char * Date8 ) const
{
  char Century[3];
  Century[0] = Date8[0];
  Century[1] = Date8[1];
  Century[2] = 0x00;
  return( atoi( Century ));
}

/***************************************************************/
//! Short description.
/*!
  \param Date8
*/
/* this function returns century and year from a CCYYMMDD date */
int xbDate::YearOf( const char * Date8 ) const
{
  char year[5];
  year[0] = Date8[0];
  year[1] = Date8[1];
  year[2] = Date8[2];
  year[3] = Date8[3];
  year[4] = 0x00;
  return( atoi( year ));
}
/***************************************************************/
//! Short description.
/*!
  \param Date8
*/
/* this function returns the month from a CCYYMMDD date        */
int xbDate::MonthOf( const char * Date8 ) const
{
   char month[3];
   month[0] = Date8[4];
   month[1] = Date8[5];
   month[2] = 0x00;
   return( atoi( month ));
}
/***************************************************************/
//! Short description.
/*!
  \param Date8
*/
/* this function returns TRUE if a CCYYMMDD date is a leap year*/

int xbDate::IsLeapYear( const char * Date8 ) const
{
   int year;
   year = YearOf( Date8 );
   if(( year % 4 == 0 && year % 100 != 0 ) || year % 400 == 0 )
      return 1;
   else
      return 0;
}

/***************************************************************/
//! Short description.
/*!
  \param CalcYear
*/
/* this function returns TRUE if a CCYYMMDD date is a leap year*/

int xbDate::CalcRollingCenturyForYear( int CalcYear ) const
{
  /* this routine calculates a century for a year - it uses
     an 80/20 rolling date window to calculate the century    */

  xbDate d;
  int ThisYear = YearOf( d.Sysdate() );
  int ThisCentury = CenturyOf( d.Sysdate() );

  ThisYear -= (ThisCentury * 100);

  if( ThisYear < 80 && CalcYear < (ThisYear+20) )
    return ThisCentury;

  else if( ThisYear >= 80 && 
           CalcYear <  ThisYear && 
           CalcYear >= (ThisYear-80))
    return ThisCentury;

  else
    return ThisCentury - 1;
}
/***************************************************************/
//! Short description.
/*!
  \param Format
  \param Date8
*/
/* this function returns the "day of" from a CCYYMMDD date     */

/*  format = XB_FMT_WEEK       Number of day in WEEK  0-6 ( Sun - Sat )
    format = XB_FMT_MONTH      Number of day in MONTH 1-31 
    format = XB_FMT_YEAR       Number of day in YEAR  1-366
*/

int xbDate::DayOf( int Format, const char * Date8 ) const
{
   char day[3];
   int  iday, imonth, iyear, iday2;

   /* check for valid format switch */

   if( Format!=XB_FMT_WEEK && Format!=XB_FMT_MONTH && Format!=XB_FMT_YEAR )
      return XB_INVALID_OPTION;
      
   if( Format == XB_FMT_WEEK )
   {
      iday =   DayOf( XB_FMT_MONTH, Date8 );  
      imonth = MonthOf( Date8 );
      iyear  = YearOf ( Date8 );

      /* The following formula uses Zeller's Congruence to determine
         the day of the week */

      if( imonth > 2 )           /* init to February */
         imonth -= 2;
      else
      {
         imonth += 10;
         iyear--;
      }

      iday2 = ((13 * imonth - 1) / 5) +iday + ( iyear % 100 ) +
              (( iyear % 100 ) / 4) + ((iyear /100 ) / 4 ) - 2 *
              ( iyear / 100 ) + 77 ;

      return( iday2 - 7 * ( iday2 / 7 ));
   }

   else if( Format == XB_FMT_MONTH )
   {
      day[0] = Date8[6];
      day[1] = Date8[7];
      day[2] = 0x00;
      return( atoi( day ));
   }
   else
    return(
      AggregatedDaysInMonths[IsLeapYear(Date8)][MonthOf(Date8)-1]+
      DayOf(XB_FMT_MONTH, Date8));
}
/**********************************************************************/
//! Short description.
/*!
*/
/* this method sets the class date & returns the system date */

xbString& xbDate::Sysdate()
{
   char dt[9];
   time_t timer;
   struct tm *tblock;
   timer = time( NULL );
   tblock = localtime( &timer );
   tblock->tm_year += 1900;
   tblock->tm_mon++;
   sprintf( dt,"%4d%02d%02d",tblock->tm_year,tblock->tm_mon,tblock->tm_mday );
   dt[8] = 0x00;
   cDate8 = dt;
   return cDate8;
}
/***************************************************************/
//! Short description.
/*!
  \param Date8
*/
/* this function checks a date for validity - returns 1 if OK  */

int xbDate::DateIsValid( const char * Date8 ) const
{
   int year, month, day;

   if(!isdigit( Date8[0] ) || !isdigit( Date8[1] ) || !isdigit( Date8[2] ) ||
      !isdigit( Date8[3] ) || !isdigit( Date8[4] ) || !isdigit( Date8[5] ) ||
      !isdigit( Date8[6] ) || !isdigit( Date8[7] ) )
         return 0;

   year  = YearOf ( Date8 );
   month = MonthOf( Date8 );
   day   = DayOf  ( XB_FMT_MONTH, Date8 );
   
   /* check the basics */
   if( year == 0 || month < 1 || month > 12 || day < 1 || day > 31 )
      return 0;

   /* April, June, September and November have 30 days */
   if(( month==4 || month==6 || month==9 || month==11 )&& day > 30 )
      return 0;

   /* check for February with leap year */
   if( month == 2 )
     if( IsLeapYear( Date8 ))
     {
       if( day > 29 )
         return 0;
     }
     else
     {
       if( day > 28 )
         return 0;
     }
   return 1;
}

/***************************************************************/
//! Short description.
/*!
  \param Date8
*/
int xbDate::SetDate( const char * Date8 )
{
  if( DateIsValid( Date8 ))
  {
    cDate8 = Date8;
    return 1;
  }
  return 0;
}

/***************************************************************/
//! Short description.
/*!
  \param Date8
*/
/* this returns the number of days since 1/1/EPOCH_MIN        */
long xbDate::JulianDays( const char * Date8 ) const
{
   int year = YearOf( Date8 );
   if(( year < EPOCH_MIN ) || (year >= EPOCH_MAX))
     return XB_INVALID_DATE;

   long days = 0;
   for (long y = EPOCH_MIN; y < year; y++ )
     days += 365 + ( ( ( y%4==0 && y%100!=0 ) || y%400==0 ) ? 1 : 0 );

   days += (long) DayOf( XB_FMT_YEAR, Date8 ) -1;

   return days;
}
/***************************************************************/
//! Short description.
/*!
  \param days
*/
/* this function does the opposite of the JulianDays function  */
/* it converts a julian based date into a Date8 format         */

xbString& xbDate::JulToDate8( long days )
{
   char Date8[9];
   int year, leap, month;

   year = EPOCH_MIN;
   leap = 0;               /* EPOCH_MIN of 100 is not a leap year */

/* this while loop calculates the year of the date by incrementing
   the years counter as it decrements the days counter */

   while( days > ( 364+leap ))
   {
      days -= 365+leap;
      year++;
      if(( year % 4 == 0 && year % 100 != 0 ) || year % 400 == 0 )
         leap = 1;
      else 
         leap = 0;
   }    

/* this for loop calculates the month and day of the date by
   comparing the number of days remaining to one of the tables   */

   for( month = 12; month >= 1; month-- )
     if( days >= (long)AggregatedDaysInMonths[leap][month] ) {
       days -= AggregatedDaysInMonths[leap][month];
       break;
     }

   sprintf( Date8, "%4d%02d%02ld", year, month+1, days+1 );

   Date8[8] = 0x00;
   cDate8 = Date8;
   return cDate8;
}
/***************************************************************/
//! Short description.
/*!
  \param Date8
*/
/* this routine returns a pointer to the day of the week(Sun-Sat)*/
xbString& xbDate::CharDayOf( const char * Date8 )
{
  struct tm tblock;
  char buf[25];

  tblock.tm_year = YearOf( Date8 ) - 1900;
  tblock.tm_mon  = MonthOf( Date8 ) - 1;
  tblock.tm_mday = DayOf( XB_FMT_MONTH, Date8 );
  tblock.tm_hour = 0;
  tblock.tm_min  = 0;
  tblock.tm_sec  = 1;
  tblock.tm_isdst = -1;
  if( mktime( &tblock ) == -1 )
    fDate = "????";
  else
  {
    strftime( buf, 25, "%A", &tblock );
    fDate = buf;
  }
  return fDate;
}
/***************************************************************/
//! Short description.
/*!
  \param Date8
*/
/* this routine returns a pointer to the month                 */

xbString& xbDate::CharMonthOf( const char * Date8 )
{
  struct tm tblock;
  char buf[25];

  tblock.tm_year = YearOf( Date8 ) - 1900;
  tblock.tm_mon  = MonthOf( Date8 ) - 1;
  tblock.tm_mday = DayOf( XB_FMT_MONTH, Date8 );
  tblock.tm_hour = 0;
  tblock.tm_min  = 0;
  tblock.tm_sec  = 1;
  tblock.tm_isdst = -1;
  if( mktime( &tblock ) == -1 )
    fDate = "????";
  else
  {
    strftime( buf, 25, "%B", &tblock );
    fDate = buf;
  }
  return fDate;
}

/***************************************************************/
//! Short description.
/*!
  \param indate in the format of MM/DD/YY
*/
/* This function formats a date and returns a pointer to a     */
/* static buffer containing the date                           */

xbString& xbDate::FormatCTODdate( const char * indate )
{
  xbDate d;
  char cbuf[3];
  char odate[9];
  fDate = "";
  if( indate[0] == ' ' || indate[1] == ' ' )   // empty date 
    return fDate;

  sprintf( cbuf, "%02d", 
           d.CalcRollingCenturyForYear( atoi( indate+6 )));
  odate[0] = cbuf[0];
  odate[1] = cbuf[1];
  odate[2] = indate[6];
  odate[3] = indate[7];
  odate[4] = indate[0];
  odate[5] = indate[1];
  odate[6] = indate[3];
  odate[7] = indate[4];
  odate[8] = 0x00;

  fDate = odate;
  return fDate;
}
/***************************************************************/
//! Short description.
/*!
  \param Format
  \param Date8
*/
/* This function formats a date and returns a pointer to a     */
/* static buffer containing the date                           */

xbString& xbDate::FormatDate( const char * Format, const char * Date8 )
{
   const char *FmtPtr;     /* format pointer */
   char *BufPtr;           /* buffer pointer */
   char type;
   char cbuf[10];
   int  type_ctr, i;
   char buf[50];
   xbString s;

   memset( buf, 0x00, 50 );
   if( strstr( Format, "YYDDD" ))
   {
      buf[0] = Date8[2];
      buf[1] = Date8[3];
      sprintf( buf+2, "%03d", DayOf( XB_FMT_YEAR, Date8 ));
   }
   else
   {
      BufPtr = buf;
      FmtPtr = Format;
      memset( cbuf, 0x00, 10 );
      while( *FmtPtr )
      {
         if( *FmtPtr != 'D' && *FmtPtr != 'M' && *FmtPtr != 'Y' )
         {
            *BufPtr = *FmtPtr;
            BufPtr++;
            FmtPtr++;
         }
         else
         { 
            type = *FmtPtr;
            type_ctr = 0;
            while( *FmtPtr == type )
            {
               type_ctr++;
               FmtPtr++;
            }
            switch( type )
            {
               case 'D':         
                  if( type_ctr == 1 )
                  {
                     sprintf( cbuf, "%d", DayOf( XB_FMT_MONTH, Date8 ));
                     strcat( buf, cbuf );
                     BufPtr += strlen( cbuf );
                  }
                  else if( type_ctr == 2 )
                  {
                     cbuf[0] = Date8[6];
                     cbuf[1] = Date8[7];
                     cbuf[2] = 0x00;
                     strcat( buf, cbuf );
                     BufPtr += 2;
                  }
                  else
                  {
                     s = CharDayOf( Date8 );
                     if( type_ctr == 3 )
                     {
                        strncat( buf, s.getData(), 3 );
                        BufPtr += 3;   
                     }
                     else
                     {
                        strcpy( cbuf, CharDayOf( Date8 ));
                        for( i = 0; i < 9; i++ )
                           if( cbuf[i] == 0x20 ) cbuf[i] = 0x00;
                        strcat( buf, cbuf );
                        BufPtr += strlen( cbuf );
                     }
                  }         
                  break;

               case 'M':
                  if( type_ctr == 1 )
                  {
                     sprintf( cbuf, "%d", MonthOf( Date8 ));
                     strcat( buf, cbuf );
                     BufPtr += strlen( cbuf );
                  }
                  else if( type_ctr == 2 )
                  {
                     cbuf[0] = Date8[4];
                     cbuf[1] = Date8[5];
                     cbuf[2] = 0x00;
                     strcat( buf, cbuf );
                     BufPtr += 2;
                  }
                  else
                  {
                     s = CharMonthOf( Date8 );
                     if( type_ctr == 3 )
                     {
                        strncat( buf, s.getData(), 3 );
                        BufPtr += 3;
                     }
                     else
                     {
                        strcpy( cbuf, CharMonthOf( Date8 ));
                        for( i = 0; i < 9; i++ )
                           if( cbuf[i] == 0x20 ) cbuf[i] = 0x00;
                        strcat( buf, cbuf );
                        BufPtr += strlen( cbuf );
                     }
                  }
                  break;
            
               case 'Y':
                  if( type_ctr == 2 )
                  {
                     cbuf[0] = Date8[2];
                     cbuf[1] = Date8[3];
                     cbuf[2] = 0x00;
                     strcat( buf, cbuf );
                     BufPtr += 2;
                  }
                  else if( type_ctr == 4 )
                  {
                     cbuf[0] = Date8[0];
                     cbuf[1] = Date8[1];
                     cbuf[2] = Date8[2];
                     cbuf[3] = Date8[3];
                     cbuf[4] = 0x00;
                     strcat( buf, cbuf );
                     BufPtr += 4;
                  }
                  break;

               default:
                  break;
            }
         }
      }
   }
   fDate = buf;
   return fDate;
}
/***************************************************************/
//! Short description.
/*!
  \param Date8
*/
/* this routine returns the Date8 format of the last day of the
   month for the given input Date8 */
 
xbString & xbDate::LastDayOfMonth( const char * Date8 )
{
  char tmp[9];
  sprintf( tmp, "%4.4d%2.2d%2.2d", 
     YearOf( Date8 ), MonthOf( Date8 ), 
     DaysInMonths[IsLeapYear(Date8)][MonthOf(Date8)]);
  cDate8 = tmp;
  return cDate8;
}
/**********************************************************************/
//! Short description.
/*!
*/
xbString &xbDate::operator+=( int count )
{
  JulToDate8( JulianDays() + count );
  return cDate8;
}
/**********************************************************************/
//! Short description.
/*!
*/
xbString &xbDate::operator-=( int count )
{
  JulToDate8( JulianDays() - count );
  return cDate8;
}
/**********************************************************************/
//! Short description.
/*!
*/
xbString &xbDate::operator++( int )
{
  *this+=1;
  return cDate8;
}
/**********************************************************************/
//! Short description.
/*!
*/
xbString &xbDate::operator--( int )
{
  *this-=1;
  return cDate8;
}
/**********************************************************************/
//! Short description.
/*!
*/
xbString &xbDate::operator+( int count )
{
  xbDate d( GetDate() );
  d+=count;
  fDate = d.GetDate();
  return fDate;
}
/**********************************************************************/
//! Short description.
/*!
*/
xbString &xbDate::operator-( int count )
{
  xbDate d( GetDate() );
  d-=count;
  fDate = d.GetDate();
  return fDate;
}
/**********************************************************************/
//! Short description.
/*!
*/
long xbDate::operator-( const xbDate & d ) const 
{
  return JulianDays() - d.JulianDays();
}
/**********************************************************************/
//! Short description.
/*!
*/
int xbDate::operator==( const xbDate & d ) const 
{
  if( JulianDays() == d.JulianDays() )
    return 1;
  else
    return 0;
}
/**********************************************************************/
//! Short description.
/*!
*/
int xbDate::operator!=( const xbDate & d ) const 
{
  if( JulianDays() != d.JulianDays() ) 
    return 1;
  else 
    return 0;
}
/**********************************************************************/
//! Short description.
/*!
*/
int xbDate::operator<( const xbDate & d ) const 
{
  if( JulianDays() < d.JulianDays() ) 
    return 1;
  else 
    return 0;
}
/**********************************************************************/
//! Short description.
/*!
*/
int xbDate::operator>( const xbDate & d ) const 
{
  if( JulianDays() > d.JulianDays() ) 
    return 1;
  else 
    return 0;
}
/**********************************************************************/
//! Short description.
/*!
*/
int xbDate::operator<=( const xbDate & d ) const 
{
  if( JulianDays() <= d.JulianDays() ) 
    return 1;
  else 
    return 0;
}
/**********************************************************************/
//! Short description.
/*!
*/
int xbDate::operator>=( const xbDate & d ) const 
{
  if( JulianDays() >= d.JulianDays() ) 
    return 1;
  else 
    return 0;
}
/**********************************************************************/
