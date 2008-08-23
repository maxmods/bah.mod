/*  xblock.cpp

    Xbase64 project source code
    written at 35000 feet on SWA
   
    This file contains the implementation of the xbLock.

    This file conatains a header file for the xbLock virtual objects which
    is used for controlling file and record locking.  Record and file
    locking has been rewritten in version 3.

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
  #pragma implementation "xblock.h"
#endif

#ifdef __WIN32__
#include <xbase64/xbwincfg.h>
#else
#include <xbase64/xbconfig.h>
#include <errno.h>
#endif

#include <xbase64/xbase64.h>

#ifdef HAVE_IO_H     // windows locking
#include <io.h>
#endif

#ifdef HAVE_DOS_H    // _sleep
#include <dos.h>
#endif

//#include <stdio.h>
//#include <stdlib.h>

/*! \file xblock.cpp
*/
#ifdef XB_LOCKING_ON

//! Constructor
/*!
  \param pdbf
*/

/*************************************************************************/
xbLock::xbLock(xbDbf * pdbf)
{
  dbf          = pdbf;
  HdrLockCnt   = 0;
  TableLockCnt = 0;
  MemoLockCnt  = 0;
  IndexLockCnt = 0;
  std::cout << "xbLock constructor" << std::cout;
}
/*************************************************************************/
xbLock::~xbLock()
{
  std::cout << "xbLock destructor" << std::endl;
}
/*************************************************************************/

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
xbShort xbLock::LockFile( int fn, xbShort LockType, xbOffT lockLen)
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
  } while( rc == -1 && tries < dbf->xbase->GetLockRetryCount());
  
  if( rc )
    return XB_LOCK_FAILED;
  
  return 0;
}

#elif HAVE_FCNTL_H

xbShort xbLock::LockFile( int fn, xbShort LockType, xbOffT lockLen )
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
  } while( rc == -1 && tries < dbf->xbase->GetLockRetryCount());
    
  if( rc )
    return XB_LOCK_FAILED;
    
  return XB_NO_ERROR;
}
#endif     // HAVE_FCNTL
/*************************************************************************/
/*************************************************************************/
xbaseLock::xbaseLock( xbDbf * pdbf ) : xbLock( pdbf )
{
  std::cout << "xbaseLock constructor" << std::cout;
}
/*************************************************************************/
xbShort xbaseLock::LockTableHeader( xbShort LockType )
{
  if( LockType == XB_UNLOCK )
    return XB_NO_ERROR;
  return XB_INVALID_LOCK_OPTION;
}
/*************************************************************************/
xbShort xbaseLock::LockTable( xbShort LockType )
{
  if(( LockType == XB_LOCK || LockType == XB_LOCK_HOLD ) && TableLockCnt ){
    TableLockCnt++;
    return XB_NO_ERROR;
  }
  if( LockType == XB_UNLOCK && TableLockCnt > 1 ){
    TableLockCnt--;
    return XB_NO_ERROR;
  }

#ifdef _LARGEFILE64_SOURCE
  if( lseek64( fileno( lfh ), 1, SEEK_SET ) != 1 )
    return XB_LOCK_FAILED;

  if( LockFile( fileno( lfh ), LockType, 4294967295LL ) != XB_NO_ERROR )
    return XB_LOCK_FAILED;
#else
  if( lseek( fileno( lfh ), 1, SEEK_SET ) != 1 )
    return XB_LOCK_FAILED;

  if( LockFile( fileno( lfh ), LockType, 4294967295L ) != XB_NO_ERROR )
    return XB_LOCK_FAILED;
#endif
  
  if( LockType == XB_UNLOCK )
    TableLockCnt--;
  else
    TableLockCnt++;

  return XB_NO_ERROR;
}
/*************************************************************************/
xbShort xbaseLock::LockRecord(xbShort LockType,xbULong RecNo,xbOffT RecCnt)
{

#ifdef _LARGEFILE64_SOURCE
  if( lseek64( fileno( lfh ), 100L + RecNo, SEEK_SET ) == -1 ){
    return XB_LOCK_FAILED;
  }
#else
  if( lseek( fileno( lfh ), 100L + RecNo, SEEK_SET ) == -1 ){
    return XB_LOCK_FAILED;
  }
#endif

  return LockFile( fileno( lfh ), LockType, (xbOffT) RecCnt );
}
/*************************************************************************/
xbShort xbaseLock::LockMemo( xbShort LockType )
{
  xbShort rc;

  if(( LockType == XB_LOCK || LockType == XB_LOCK_HOLD ) && MemoLockCnt ){
    MemoLockCnt++;
    return XB_NO_ERROR;
  }
  else if ( LockType == XB_UNLOCK && MemoLockCnt > 1 ){
    MemoLockCnt--;
    return XB_NO_ERROR;
  }
  
#ifdef _LARGEFILE64_SOURCE
  if( lseek64( fileno( lfh ), 2, SEEK_SET ) != 2 )
    return XB_LOCK_FAILED;
#else
  if( lseek( fileno( lfh ), 2, SEEK_SET ) != 2 )
    return XB_LOCK_FAILED;
#endif
  rc = LockFile( fileno( lfh ), LockType, 1 );

  if( rc == XB_NO_ERROR ){
    if( LockType == XB_UNLOCK )
      MemoLockCnt--;
    else
      MemoLockCnt++;
  }
  return rc;
}
/*************************************************************************/
//! Lock Index
/*!
  Locks all indices for a table when using lock mode XB_XBASE_LOCK_MODE
  
  \param LockType  is one of XB_LOCK, XB_LOCK_HOLD or XB_UNLOCK
*/

xbShort xbaseLock::LockIndex( xbShort LockType )
{
  xbShort rc;

//  if( !NdxList )
//    printf( "no index\n" );

  if(( LockType == XB_LOCK || LockType == XB_LOCK_HOLD ) && IndexLockCnt ){
    IndexLockCnt++;
    return XB_NO_ERROR;
  }
  if( LockType == XB_UNLOCK && IndexLockCnt > 1 ){
    IndexLockCnt--;
    return XB_NO_ERROR;
  }

#ifdef _LARGEFILE64_SOURCE
  if( lseek64( fileno( lfh ), 3, SEEK_SET ) == -1 ){
    printf( "here cp1\n");
    return XB_LOCK_FAILED;
  }
#else
  if( lseek( fileno( lfh ), 3, SEEK_SET ) == -1 ){
    printf( "here cp2\n" );
    return XB_LOCK_FAILED;
  }
#endif

  rc = LockFile( fileno( lfh ), LockType, 1 );
  
  if( rc == XB_NO_ERROR )
    if( LockType == XB_UNLOCK )
      IndexLockCnt--;
    else
      IndexLockCnt++;

  return rc; 
}
/*************************************************************************/
xbShort xbaseLock::UnlockAll()
{
  return XB_INVALID_LOCK_OPTION;
}
/************************************************************************/
xbShort xbaseLock::LockInit()
{
  xbShort  len;
  xbString lfn;

  lfn = dbf->GetDbfName();
  lfn.resize( lfn.len() - 3 );

  lfn += ".lck";

#ifdef _LARGEFILE_SOURCE
  if(( lfh = fopen64( lfn.getData(), "w+b" )) == NULL )
    return XB_OPEN_ERROR;
#else
  if(( lfh = fopen( lfn.getData(), "w+b" )) == NULL )
    return XB_OPEN_ERROR;
#endif
        
  else
    return XB_NO_ERROR;
}

/*************************************************************************/
/*************************************************************************/
dbaseLock::dbaseLock( xbDbf * pdbf ) : xbLock( pdbf )
{
  std::cout << "dbaseLock constructor" << std::cout;
}
/*************************************************************************/
xbShort dbaseLock::LockTableHeader( xbShort LockType )
{
  if( LockType == XB_UNLOCK )
    return XB_NO_ERROR;
  return XB_INVALID_LOCK_OPTION;
}
/*************************************************************************/
xbShort dbaseLock::LockTable( xbShort LockType )
{

  if(( LockType == XB_LOCK || LockType == XB_LOCK_HOLD ) && TableLockCnt ){
    TableLockCnt++;
    return XB_NO_ERROR;
  }
  if( LockType == XB_UNLOCK && TableLockCnt > 1 ){
    TableLockCnt--;
    return XB_NO_ERROR;
  }

#ifdef _LARGEFILE64_SOURCE
  if( lseek64( dbf->GetDbfFileNo(), 4026531838LL, SEEK_SET ))
    return XB_LOCK_FAILED;

  if( LockFile( dbf->GetDbfFileNo(), LockType, 1 ) != XB_NO_ERROR )
    return XB_LOCK_FAILED;

  if( LockType == XB_LOCK || LockType == XB_LOCK_HOLD ){
    if( LockRecord( XB_LOCK, 1, 4294967295LL ) != XB_NO_ERROR )
      return LockTable( XB_UNLOCK );
    LockRecord( XB_UNLOCK, 1, 4294967295LL );
  }

  if( LockType == XB_UNLOCK )
    TableLockCnt--;
  else
    TableLockCnt++;

  return XB_NO_ERROR;
#else
  /* I couldn't figure out how Dbase locks a file at offset 4026531838 
     for a 32 bit platform - if you know how, please let me know
     
     Gary - gkunkel@zhsac.com
     
  */

  return XB_INVALID_LOCK_OPTION;
#endif

}

/*************************************************************************/
xbShort dbaseLock::LockRecord( xbShort LockType, xbULong RecNo, xbOffT RecCnt )
{
#ifdef _LARGEFILE64_SOURCE

  if( lseek64( dbf->GetDbfFileNo(), 4026531838LL - (RecNo+RecCnt-1), SEEK_SET ) == -1 )
    return XB_LOCK_FAILED;

  return LockFile( dbf->GetDbfFileNo(), LockType, RecCnt );

#else

  /* I couldn't figure out how dbase locks a file at offset 4026531838 
     for a 32 bit platform - if you know how, please let me know
     
     Gary - gkunkel@zhsac.com
     
  */

  return XB_INVALID_LOCK_OPTION;
#endif
}

/*************************************************************************/
xbShort dbaseLock::LockMemo( xbShort LockType )
{

  xbShort rc;

  if(( LockType == XB_LOCK || LockType == XB_LOCK_HOLD ) && MemoLockCnt ){
    MemoLockCnt++;
    return XB_NO_ERROR;
  }
  else if ( LockType == XB_UNLOCK && MemoLockCnt > 1 ){
    MemoLockCnt--;
    return XB_NO_ERROR;
  }
  
#ifdef _LARGEFILE_SOURCE
  if( lseek64( dbf->GetMemoFileNo(), 4026531838LL, SEEK_SET ) == -1 )
    return XB_LOCK_FAILED;
  rc = LockFile( dbf->GetMemoFileNo(), LockType, 1 );
#else
  rc = XB_INVALID_OPTION;
#endif

  if( rc == XB_NO_ERROR ){
    if( LockType == XB_UNLOCK )
      MemoLockCnt--;
    else
      MemoLockCnt++;
  }
  return rc;
}
/*************************************************************************/
xbShort dbaseLock::LockIndex( xbShort LockType )
{
  if( LockType == XB_NO_ERROR )
    return XB_NO_ERROR;
  return XB_INVALID_LOCK_OPTION;
}
/*************************************************************************/
xbShort dbaseLock::UnlockAll()
{
  return XB_INVALID_LOCK_OPTION;
}
/*************************************************************************/
/*************************************************************************/
clipperLock::clipperLock( xbDbf * pdbf ) : xbLock( pdbf )
{
  std::cout << "clipperLock constructor" << std::cout;
}
/*************************************************************************/
xbShort clipperLock::LockTableHeader( xbShort LockType )
{
  return XB_INVALID_LOCK_OPTION;
}
/*************************************************************************/
xbShort clipperLock::LockTable( xbShort LockType )
{
  if(( LockType == XB_LOCK || LockType == XB_LOCK_HOLD ) && TableLockCnt ){
    TableLockCnt++;
    return XB_NO_ERROR;
  }
  if( LockType == XB_UNLOCK && TableLockCnt > 1 ){
    TableLockCnt--;
    return XB_NO_ERROR;
  }

  if( LockRecord( LockType, 1L, 1000000000L ) != XB_NO_ERROR )
    return XB_LOCK_FAILED;
  
  if( LockType == XB_UNLOCK )
    TableLockCnt--;
  else
    TableLockCnt++;

  return XB_NO_ERROR;
}
/*************************************************************************/
xbShort clipperLock::LockRecord(
     xbShort LockType, xbULong RecNo, xbOffT RecCnt )
{

#ifdef _LARGEFILE64_SOURCE
  if( lseek64( dbf->GetDbfFileNo(), 1000000000L + RecNo, SEEK_SET ))
    return XB_LOCK_FAILED;
#else
  if( lseek( dbf->GetDbfFileNo(), 1000000000L + RecNo, SEEK_SET ))
    return XB_LOCK_FAILED; 
#endif

  return LockFile( dbf->GetDbfFileNo(), LockType, RecCnt );
}
/*************************************************************************/
xbShort clipperLock::LockMemo( xbShort LockType )
{
  return XB_NO_ERROR;
}
/*************************************************************************/
xbShort clipperLock::LockIndex( xbShort LockType )
{
  return XB_INVALID_LOCK_OPTION;
}
/*************************************************************************/
xbShort clipperLock::UnlockAll()
{
  return XB_INVALID_LOCK_OPTION;
}
/*************************************************************************/
/*************************************************************************/
foxproLock::foxproLock( xbDbf * pdbf ) : xbLock( pdbf )
{
  std::cout << "foxproLock constructor" << std::cout;
}
/*************************************************************************/
xbShort foxproLock::LockTableHeader( xbShort LockType )
{

  return XB_INVALID_LOCK_OPTION;
}
/*************************************************************************/
xbShort foxproLock::LockTable( xbShort LockType )
{

  if(( LockType == XB_LOCK || LockType == XB_LOCK_HOLD ) && TableLockCnt ){
    TableLockCnt++;
    return XB_NO_ERROR;
  }
  if( LockType == XB_UNLOCK && TableLockCnt > 1 ){
    TableLockCnt--;
    return XB_NO_ERROR;
  }


// something goes in here


  if( LockType == XB_UNLOCK )
    TableLockCnt--;
  else
    TableLockCnt++;

  return XB_NO_ERROR;

}
/*************************************************************************/
xbShort foxproLock::LockRecord( xbShort LockType, xbULong RecNo, xbOffT len )
{
  return XB_INVALID_LOCK_OPTION;
}
/*************************************************************************/
xbShort foxproLock::LockMemo( xbShort LockType )
{
  return XB_INVALID_LOCK_OPTION;
}
/*************************************************************************/
xbShort foxproLock::LockIndex( xbShort LockType )
{
  return XB_INVALID_LOCK_OPTION;
}
/*************************************************************************/
xbShort foxproLock::UnlockAll()
{
  return XB_INVALID_LOCK_OPTION;
}
/*************************************************************************/

#endif  // XB_LOCKING_ON

