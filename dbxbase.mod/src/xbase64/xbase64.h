/*  xbase64.h  

    Xbase project source code

    This file contains a header file for the xbXBase class, which is the
    base class for using the Xbase DBMS library.

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

#ifndef __XB_XBASE_H__
#define __XB_XBASE_H__

#ifdef __GNU_LesserG__
#pragma interface
#endif

#ifdef __WIN32__
#include <xbase64/xbwincfg.h>
#else
#include <xbase64/xbconfig.h>
#endif

#include <string.h>

#if defined(__WIN32__)
#include "windows.h"

// ripped from wxWindows

// _declspec works in BC++ 5 and later, as well as VC++
#if defined(__VISUALC__) || defined(__BORLANDC__) || defined(__GNU_LesserC__)
#  ifdef XBMAKINGDLL
#    define XBDLLEXPORT __declspec( dllexport )
#    define XBDLLEXPORT_DATA(type) __declspec( dllexport ) type
#    define XBDLLEXPORT_CTORFN
#  elif defined(XBUSINGDLL)
#    define XBDLLEXPORT __declspec( dllimport )
#    define XBDLLEXPORT_DATA(type) __declspec( dllimport ) type
#    define XBDLLEXPORT_CTORFN
#  else
#    define XBDLLEXPORT
#    define XBDLLEXPORT_DATA(type) type
#    define XBDLLEXPORT_CTORFN
#  endif

#else

#  define XBDLLEXPORT
#  define XBDLLEXPORT_DATA(type) type
#  define XBDLLEXPORT_CTORFN
#endif

#else // !Windows
#  define XBDLLEXPORT
#  define XBDLLEXPORT_DATA(type) type
#  define XBDLLEXPORT_CTORFN
#endif // Win/!Win


#define XB_SINGLE_USER_MODE 0
#define XB_UNLOCK           200
#define XB_LOCK             201
#define XB_LOCK_HOLD        202

#ifdef XB_LOCKING_ON

 #ifdef HAVE_SYS_LOCKING_H
  #include <sys/locking.h>
  #ifdef __MINGW32__
    #defibe locking _locking
  #endif
 #endif

 #ifdef HAVE_FCNTL_H
  #include <fcntl.h>
 #endif

 #ifdef HAVE_UNISTD_H
  #include <unistd.h>
 #endif

 #define XB_XBASE_LOCK_MODE     200
 #define XB_DBASE5_LOCK_MODE    201
 #define XB_CLIPPER5_LOCK_MODE  202
 #define XB_FOXPRO3_LOCK_MODE   203

#endif          /* XB_LOCKING_ON */

#include "xbtypes.h"
#include "xbretcod.h"
#include "xbdate.h"
#include "xbstring.h"

#ifndef XB_MIN
#define XB_MIN(a, b)	(((a) < (b)) ? (a) : (b))
#endif /* XB_MIN */

// 3/18/04 next macro isn't currently used in the library - GK
//#ifndef XB_MAX
//#define XB_MAX(a, b)	(((a) < (b)) ? (b) : (a))
//#endif /* XB_MAX */

/*! \file xbase64.h
*/

class XBDLLEXPORT xbDbf;

//! xbDbList struct
/*!
*/
struct XBDLLEXPORT xbDbList{
  xbDbList * NextDbf;
  char * DbfName;
  xbDbf  * dbf;
};

//! xbXBase class
/*!
*/
class XBDLLEXPORT xbXBase {
 public:
  ~xbXBase();
  xbXBase();
  xbShort  AddDbfToDbfList(xbDbf *d, const char *DatabaseName);
  xbDbf *  GetDbfPtr( const char *Name );
  xbShort  DirectoryExistsInName( const char *Name );
  xbShort  GetEndianType() { return EndianType; }
  void     DisplayError( xbShort ErrorCode ) const;
  static const char* GetErrorMessage( xbShort ErrorCode );
  xbString & GetDefaultDateFormat() { return DefaultDateFormat; }
  void     SetDefaultDateFormat( const xbString & f ){ DefaultDateFormat = f; }

  /* next 6 routines handle both big endian and little endian machines */
  xbDouble GetDouble( const char *p );
  xbLong   GetLong  ( const char *p );
  xbULong  GetULong ( const char *p );
  xbShort  GetShort ( const char *p );
  xbULong GetHBFULong( const char *p );
  xbShort GetHBFShort ( const char *p );

  void   PutLong  ( char *p, const xbLong   l );
  void   PutShort ( char *p, const xbShort  s );
  void   PutULong ( char *p, const xbULong  l );
  void   PutUShort( char *p, const xbUShort s );
  void   PutDouble( char *p, const xbDouble d );

  xbShort  RemoveDbfFromDbfList( xbDbf * );

#ifdef XB_LOCKING_ON
  xbShort  GetLockRetryCount(){ return LockRetryCount; }
  void     SetLockRetryCount( xbShort lrc ) { LockRetryCount = lrc; }
  xbShort  LockFile( int fn, xbShort type, xbOffT len );
  xbShort  GetLockMode() { return LockMode; }
  xbShort  SetLockMode( xbShort nlm );
#endif

protected:
  xbDbList * DbfList;
  xbDbList * FreeDbfList;
  xbShort    EndianType;          /* B = Big Endian, L = Little Endian */

private:
  xbString DefaultDateFormat;
  
#ifdef XB_LOCKING_ON
  xbShort LockRetryCount;
  xbShort LockMode;
#endif
};

#include "xbdbf.h"

#if defined(XB_EXPRESSIONS)
#include "xbexp.h"
#endif

#if defined(XB_INDEX_ANY)
#include "xbindex.h"
#include "xbmindex.h"
#endif

#ifdef XB_LOCKING_ON
#include "xblock.h"
#endif

#ifdef XB_INDEX_NDX
#include "xbndx.h"
#endif

#ifdef XB_INDEX_NTX
#include "xbntx.h"
#endif

#ifdef XB_INDEX_CDX
#include "xbcdx.h"
#endif

#if defined(XB_FILTERS) && !defined(XB_INDEX_ANY)
#error XB_FILTERS cant be used without index support
#elif defined(XB_FILTERS)
#include "xbfilter.h"
#endif

#endif		// __XB_XBASE_H__



