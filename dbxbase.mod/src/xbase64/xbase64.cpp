/*  xbase64.cpp

    Xbase64 project source code

    This file contains logic for the basic Xbase class.

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
  #pragma implementation "xbase64.h"
#endif

#ifdef __WIN32__
#include <xbase64/xbwincfg.h>
#else
#include <xbase64/xbconfig.h>
#include <errno.h>
#endif

#include <xbase64/xbase64.h>
#include <ctype.h>
#include <string.h>

//#include <xbase64/xbexcept.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_IO_H
          // windows locking
#include <io.h>
#endif

#ifdef HAVE_DOS_H
         // windows _sleep
#include <dos.h>
#endif


/*! \file xbase64.cpp
*/

/*************************************************************************/
//! Constructor.
/*!
*/
xbXBase::xbXBase()
{
  xbShort e = 1;
  EndianType = *(char *) &e;
  if( EndianType )
    EndianType = 'L';
  else
    EndianType = 'B';
  DbfList = NULL;
  FreeDbfList = NULL;

#ifdef XB_LOCKING_ON
  LockRetryCount = 5;
  LockMode       = XB_SINGLE_USER_MODE;
#endif

  DefaultDateFormat = "MM/DD/YY";
}
/*************************************************************************/
//! Get pointer to named dbf.
/*!
  Looks up an open DBF file by Name.
  
  \param Name
  \returns A pointer to the xbDbf class instance if found or NULL if
    not found.
*/
xbDbf *xbXBase::GetDbfPtr(const char *Name) {
  xbDbList *t;

  t = DbfList;
  xbShort len = strlen(Name);

  /* check for -> embedded in the name */
  for( xbShort i = 0; i < (len-1); i++ )
    if( Name[i] == '-' && Name[i+1] == '>' )
      len = i-1;

  while (t) {
    if (strncmp(Name, t->DbfName, len) == 0 )
      return t->dbf;
    t = t->NextDbf;
  }
  return NULL;
}

/*************************************************************************/
//! Destructor.
/*!
*/
xbXBase::~xbXBase()
{
  xbDbList *i = FreeDbfList;
  while (i) {
    xbDbList *t = i->NextDbf;
    if (i->DbfName) {
       free(i->DbfName);
    }
    free(i);
    i = t;
  }
}
/*************************************************************************/
//! Add dbf to dbf list.
/*!
  Adds an xbDbf class instance to the list of dbf's.
  
  \param d the xbDbf instance to be added
  \param DatabaseName name of the database
  
  \returns One of the following return codes:
    \htmlonly
      <p>
      <table border=2><tr><th>Return Code</th><th>Description</th></tr>
        <tr><td>XB_NO_ERROR</td><td>No error</td></tr>
        <tr><td>XB_NO_MEMORY</td><td>Out of memory</td></tr>
      </table>
    \endhtmlonly
    \latexonly
      \\
      \\
      \begin{tabular}{|l|l|} \hline
        \textbf{Return Code} & \textbf{Description} \\ \hline \hline
        XB\_NO\_ERROR & No Error \\ \hline
        XB\_NO\_MEMORY & Out of memory \\ \hline
      \end{tabular}
    \endlatexonly
*/
xbShort xbXBase::AddDbfToDbfList(xbDbf *d, const char *DatabaseName) {
   xbDbList *i, *s, *t;

  if(!FreeDbfList) {
    if((i = (xbDbList *)malloc(sizeof(xbDbList))) == NULL) {
      return XB_NO_MEMORY;
    }
  } else {
    i = FreeDbfList;
    FreeDbfList = i->NextDbf;
  }
  memset(i, 0x00, sizeof(xbDbList));

  i->DbfName  = strdup(DatabaseName);
  i->dbf      = d;

  /* insert new dbf into the list of open dbf files, sorted by dbf name */
  s = NULL;
  t = DbfList;
  while(t && strcmp(t->DbfName, DatabaseName) < 0) {
    s = t;
    t = t->NextDbf;
  }
  i->NextDbf = t;
  if (s == NULL)
    DbfList = i;
  else
    s->NextDbf = i;

  return 0;
}
/***********************************************************************/
//!  Remove dbf from dbf list.
/*!
  Removes the specified xbDbf class instance from the list of dbf's.
  
  \param d xbDbf to be removed
  
  \returns One of the following return codes:
    \htmlonly
      <p>
      <table border=2><tr><th>Return Code</th><th>Description</th></tr>
        <tr><td>XB_NO_ERROR</td><td>No error</td></tr>
      </table>
    \endhtmlonly
    \latexonly
      \\
      \\
      \begin{tabular}{|l|l|} \hline
        \textbf{Return Code} & \textbf{Description} \\ \hline \hline
        XB\_NO\_ERROR & No Error \\ \hline
      \end{tabular}
    \endlatexonly
*/
xbShort xbXBase::RemoveDbfFromDbfList(xbDbf *d) {
   xbDbList *i, *s;

  i = DbfList;
  s = NULL;

  while (i) {
    if(i->dbf == d) {
      /* remove it from current chain */
      if(s)
        s->NextDbf = i->NextDbf;
      else
        DbfList = i->NextDbf;

      /* add i to the current free chain */
      i->NextDbf = FreeDbfList;
      FreeDbfList = i;
      free(FreeDbfList->DbfName);
      FreeDbfList->DbfName = NULL;
      break;
    } else {
      s = i;
      i = i->NextDbf;
    }
  }
  return XB_NO_ERROR;
} 

// FIXME: byte reverse methods are awful, compared to bitwise shifts  -- willy

/************************************************************************/
//! Get a portable short value.
/*!
  Converts a short (16 bit integer) value stored at p from a portable 
  format to the machine format.
  
  \param p pointer to memory containing the portable short value
  
  \returns the short value.
*/
/* This routine returns a short value from a 2 byte character stream */
xbShort xbXBase::GetShort(const char *p) {
   xbShort s, i;
   const char *sp;
   char *tp;

   s = 0;
   tp = (char *) &s;
   sp = p;
   if( EndianType == 'L' )
      for( i = 0; i < 2; i++ ) *tp++ = *sp++;
   else
   {
      sp++;
      for( i = 0; i < 2; i++ ) *tp++ = *sp--;
   }  
   return s;
}
/*************************************************************************/
//! Get a portable long value.
/*!
  Converts a long (32 bit integer) value stored at p from a portable 
  format to the machine format.
  
  \param p pointer to memory containing the portable long value
  
  \returns the long value.
*/
/* This routine returns a long value from a 4 byte character stream */
xbLong xbXBase::GetLong( const char *p )
{
   xbLong l;
   const char *sp;
   char *tp;
   xbShort i;

   tp = (char *) &l;
   sp = p;
   if( EndianType == 'L' )
      for( i = 0; i < 4; i++ ) *tp++ = *sp++;
   else
   {
      sp+=3;
      for( i = 0; i < 4; i++ )  *tp++ = *sp--;
   }  
   return l;
}
/*************************************************************************/
//! Get a portable unsigned long value.
/*!
  Converts an unsigned long (32 bit integer) value stored at p from a portable 
  format to the machine format.
  
  \param p pointer to memory containing the portable unsigned long value
  
  \returns the unsigned long value.
*/
/* This routine returns a long value from a 4 byte character stream */
xbULong xbXBase::GetULong( const char *p )
{
  xbULong l;
  char *tp;
  xbShort i;
  
  tp = (char *) &l;
  if( EndianType == 'L' )
    for( i = 0; i < 4; i++ ) *tp++ = *p++;
  else{
    p+=3;
    for( i = 0; i < 4; i++ ) *tp++ = *p--;
  }
  return l;
}

/************************************************************************/
//! Get a high byte first short value.
/*!
  Converts a short (16 bit integer) value stored at p from a high byte first
  format to the machine format.

  \param p pointer to memory containing the high byte first short value

  \returns the short value.
*/
/* This routine returns a short value from a 2 byte character stream */
xbShort xbXBase::GetHBFShort(const char *p) {
   xbShort s, i;
   const char *sp;
   char *tp;

   s = 0;
   tp = (char *) &s;
   sp = p;
   if( EndianType == 'B' )
      for( i = 0; i < 2; i++ ) *tp++ = *sp++;
   else
   {
      sp++;
      for( i = 0; i < 2; i++ ) *tp++ = *sp--;
   }
   return s;
}

/*************************************************************************/
//! Get a high byte first unsigned long value.
/*!
  Converts an unsigned long (32 bit integer) value stored at p from a high byte first
  format to the machine format.

  \param p pointer to memory containing the high byte first unsigned long value

  \returns the unsigned long value.
*/
/* This routine returns a long value from a 4 byte character stream */
xbULong xbXBase::GetHBFULong( const char *p )
{
  xbULong l;
  char *tp;
  xbShort i;

  tp = (char *) &l;
  if( EndianType == 'B' )
    for( i = 0; i < 4; i++ ) *tp++ = *p++;
  else{
    p+=3;
    for( i = 0; i < 4; i++ ) *tp++ = *p--;
  }
  return l;
}
/*************************************************************************/
//! Get a portable double value.
/*!
  Converts a double (64 bit floating point) value stored at p from a portable 
  format to the machine format.
  
  \param p pointer to memory containing the portable double value
  
  \returns the double value.
*/
/* This routine returns a double value from an 8 byte character stream */
xbDouble xbXBase::GetDouble( const char *p )
{
   xbDouble d;
   const char *sp;
   char *tp;
   xbShort i;

   tp = (char *) &d;
   sp = p;
   if( EndianType == 'L' )
      for( i = 0; i < 8; i++ ) *tp++ = *sp++;
   else
   {
      sp+=7;
      for( i = 0; i < 8; i++ )  *tp++ = *sp--;
   } 

   return d;
}
/*************************************************************************/
//! Put a portable short value.
/*!
  Converts a short (16 bit integer) value from machine format to a
  portable format and stores the converted value in the memory referenced
  by c.
  
  \param c pointer to memory to hold converted value
  \param s value to be converted
*/
/* This routine puts a short value to a 2 byte character stream */
void xbXBase::PutShort( char * c, xbShort s )
{
   const char *sp;
   char *tp;
   xbShort i;

   tp = c;
   sp = (const char *) &s;

   if( EndianType == 'L' )
   {
      for( i = 0; i < 2; i++ ) *tp++ = *sp++;
   }
   else      /* big endian */
   {
      sp++;
      for( i = 0; i < 2; i++ ) *tp++ = *sp--;
   }
   return;
}

/*************************************************************************/
//! Put a portable long value.
/*!
  Converts a long (32 bit integer) value from machine format to a
  portable format and stores the converted value in the memory referenced
  by c.
  
  \param c pointer to memory to hold converted value
  \param l value to be converted
*/
/* This routine puts a long value to a 4 byte character stream */
void xbXBase::PutLong( char * c, xbLong l )
{
   const char *sp;
   char *tp;
   xbShort i;

   tp = c;
   sp = (const char *) &l;
   if( EndianType == 'L' )
      for( i = 0; i < 4; i++ ) *tp++ = *sp++;
   else
   {
      sp+=3;
      for( i = 0; i < 4; i++ ) *tp++ = *sp--;
   }
   return;
}
/*************************************************************************/
//! Put a portable unsigned short value.
/*!
  Converts an unsigned long (16 bit integer) value from machine format to a
  portable format and stores the converted value in the memory referenced
  by c.
  
  \param c pointer to memory to hold converted value
  \param s value to be converted
*/
/* This routine puts a short value to a 2 byte character stream */
void xbXBase::PutUShort( char * c, xbUShort s )
{
   const char *sp;
   char *tp;
   xbShort i;

   tp = c;
   sp = (const char *) &s;
   if( EndianType == 'L' )
      for( i = 0; i < 2; i++ ) *tp++ = *sp++;
   else
   {
      sp++;
      for( i = 0; i < 2; i++ ) *tp++ = *sp--;
   }
   return;
}
/*************************************************************************/
//! Put a portable unsigned long value.
/*!
  Converts an unsigned long (32 bit integer) value from machine format to a
  portable format and stores the converted value in the memory referenced
  by c.
  
  \param c pointer to memory to hold converted value
  \param l value to be converted
*/
/* This routine puts a long value to a 4 byte character stream */
void xbXBase::PutULong( char * c, xbULong l )
{
   const char *sp;
   char *tp;
   xbShort i;

   tp = c;
   sp = (const char *) &l;
   if( EndianType == 'L' )
      for( i = 0; i < 4; i++ ) *tp++ = *sp++;
   else
   {
      sp+=3;
      for( i = 0; i < 4; i++ ) *tp++ = *sp--;
   }
   return;
}
/*************************************************************************/
//! Put a portable double value.
/*!
  Converts a double (64 floating point) value from machine format to a
  portable format and stores the converted value in the memory referenced
  by c.
  
  \param c pointer to memory to hold converted value
  \param d value to be converted
*/
/* This routine puts a double value to an 8 byte character stream */
void xbXBase::PutDouble( char * c, xbDouble d )
{
   const char *sp;
   char *tp;
   xbShort i;

   tp = c;
   sp = (const char *) &d;
   if( EndianType == 'L' )
      for( i = 0; i < 8; i++ ) *tp++ = *sp++;
   else
   {
      sp+=7;
      for( i = 0; i < 8; i++ ) *tp++ = *sp--;
   }
   return;
}
/************************************************************************/
//! Get offset of last PATH_SEPARATOR in Name.
/*!
  Scans the specified Name for the last occurance of PATH_SEPARATOR.
  
  \param Name string to be scanned.
  
  \returns offset of last occurance of PATH_SEPARATOR
*/
xbShort xbXBase::DirectoryExistsInName( const char * Name )
{
   /* returns the offset in the string of the last directory slash */

   xbShort Count, Mark;
   char  Delim;
   const char  *p;

   Delim = PATH_SEPARATOR;

   Count = Mark = 0;
   p = Name;

   while( *p )
   {
      Count++;
      if( *p++ == Delim ) Mark = Count;
   }
   return Mark;
}

/************************************************************************/
//! Display description of error code.
/*!
  Displays a text description of an XBase error code.
  
  \param ErrorCode error to be displayed
*/
void xbXBase::DisplayError( xbShort ErrorCode ) const
{
  std::cout << GetErrorMessage( ErrorCode ) << std::endl;
}
/************************************************************************/
//! Get description of error code.
/*!
  Returns a pointer to string containing a text description of an
  error code.
  
  \param ErrorCode error number of description to be returned
*/
const char* xbXBase::GetErrorMessage( xbShort ErrorCode )
{
  switch( ErrorCode ) {
    case    0: return "No Error";
    case -100: return "End Of File";
    case -101: return "Beginning Of File";
    case -102: return "No Memory";
    case -103: return "File Already Exists";
    case -104: return "Database or Index Open Error";
    case -105: return "Error writing to disk drive";
    case -106: return "Unknown Field Type";
    case -107: return "Database already open";
    case -108: return "Not an Xbase type database";
    case -109: return "Invalid Record Number";
    case -110: return "Invalid Option";
    case -111: return "Database not open";
    case -112: return "Disk Drive Seek Error";
    case -113: return "Disk Drive Read Error";
    case -114: return "Search Key Not Found";
    case -115: return "Search Key Found";
    case -116: return "Invalid Key";
    case -117: return "Invalid Node Link";
    case -118: return "Key Not Unique";
    case -119: return "Invalid Key Expression";
    case -120: return "DBF File Not Open";
    case -121: return "Invalid Key Type";
    case -122: return "Invalid Node No";
    case -123: return "Node Full";
    case -124: return "Invalid Field Number";
    case -125: return "Invalid Data";
    case -126: return "Not a leaf node";
    case -127: return "Lock Failed";
    case -128: return "Close Error";
    case -129: return "Invalid Schema";
    case -130: return "Invalid Name";
    case -131: return "Invalid Block Size";
    case -132: return "Invalid Block Number";
    case -133: return "Not a Memo field";
    case -134: return "No Memo Data";    
    case -135: return "Expression syntax error";
    case -136: return "Parse Error";
    case -137: return "No Data";
    case -138: return "Unknown Token Type";
    case -140: return "Invalid Field";
    case -141: return "Insufficient Parms";
    case -142: return "Too Many Parms";
    case -143: return "Invalid or Undefined Function";
    case -144: return "Invalid Field Length";
    case -145: return "Harvest Node";
    case -146: return "Invalid Date";
    case -147: return "Invalid Lock Option";
    default:   return "Unknown error code";
  }
}
/************************************************************************/
#ifdef XB_LOCKING_ON

//! File lock routine
/*!
  Lowest level lock routine
  Locks/unlocks a database,memo or index file.
  This function assumes the file position has been correctly set
  
  \param fn        file to lock/unlock
  \param LockType  lock type, one of: XB_LOCK or XB_UNLOCK  
  \param lockLen   byte count to lock
*/

#ifdef __WIN32__
xbShort xbXBase::LockFile( int fn, xbShort LockType, xbOffT lockLen)
{

  int mode;
  int rc;
  int tries = 0;

  /* convert the xbase locking command into a windows locking command */
  if( LockType == XB_UNLOCK )
    mode = LK_UNLCK;
  else if( LockType == XB_LOCK || LockType == XB_LOCK_HOLD )
    mode = LK_NBLCK;
  else
    return XB_INVALID_LOCK_OPTION;

  do{
    rc = locking( fn, mode, lockLen );
    if( rc )
      _sleep( 1 );
  } while( rc == -1 && tries++ < GetLockRetryCount());
  
  if( rc )
    return XB_LOCK_FAILED;
  
  return 0;
}

#elif HAVE_FCNTL_H

xbShort xbXBase::LockFile( int fn, xbShort LockType, xbOffT lockLen )
{
  xbShort cmd, rc;
  xbShort tries = 0;
  
/* convert cross platform xbase lock type to unix lock type */  
  if( LockType == XB_UNLOCK ) 
    cmd = F_ULOCK;
  else if( LockType == XB_LOCK || LockType == XB_LOCK_HOLD )
    cmd = F_TLOCK;
  else
    return XB_INVALID_LOCK_OPTION;

/* do the actual lock */
  do{     
    #ifdef _LARGEFILE64_SOURCE
      rc = lockf64( fn, cmd, lockLen );
    #else
      rc = lockf( fn, cmd, lockLen );
    #endif
    if( rc == -1 && errno != EINTR ){    
      tries++; 
      sleep(1);
    }
  } while( rc == -1 && tries < GetLockRetryCount());
    
  if( rc )
    return XB_LOCK_FAILED;
    
  return XB_NO_ERROR;
}
#endif     // HAVE_FCNTL
#endif     // XB_LOCKING_ON

/************************************************************************/
#ifdef XB_LOCKING_ON

//! Set high level lock mode
/*!
  
  \param nlm       New lock mode
*/

xbShort xbXBase::SetLockMode( xbShort nlm )
{
  if( nlm != XB_SINGLE_USER_MODE  &&  nlm != XB_XBASE_LOCK_MODE     &&
      nlm != XB_DBASE5_LOCK_MODE  &&  nlm != XB_CLIPPER5_LOCK_MODE &&
      nlm != XB_FOXPRO3_LOCK_MODE )
    return XB_INVALID_LOCK_OPTION;
   
  LockMode = nlm;
  return XB_NO_ERROR;
}

#endif     // XB_LOCKING_ON



