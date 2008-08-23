/*  xbtypes.h
  
    Xbase64 project source code

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

#ifndef __XB_XTYPES_H__
#define __XB_XTYPES_H__

#include <stdio.h>

/*! \file xbtypes.h
*/

//! xbULong type
/*!
*/
typedef unsigned long  int xbULong;

//! xbUShort type
/*!
*/
typedef unsigned short int xbUShort;

//! xbShort type
/*!
*/
typedef short int xbShort;
typedef long  xbLong;


//! xbFloat type
/*!
*/
typedef float              xbFloat;


//! xbDouble type
/*!
*/
typedef double             xbDouble;

//! xbBool type
/*!
*/
typedef short int          xbBool;

//! xbOffT type
/*!
*/
#ifdef XB_LOCKING_ON
#ifdef __WIN32__
#else
#endif
#endif      // XB_LOCKING_ON

// 64 bit file processing
#if defined(HAVE_FSEEKO) && defined(HAVE_FTELLO) && defined(XB_LARGEFILE_SUPPORT)
 #define _ftell ftello
 #define _fseek fseeko
 typedef off_t xbOffT;
#else
 #define _ftell ftell
 #define _fseek fseek
 typedef long xbOffT;
#endif

#endif      // __XB_XTYPES_H__
