/*  xblock.h

    Xbase project source code
  
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

/*! \file xblock.h
*/

#ifndef __XB_XBLOCK_H__
#define __XB_XBLOCK_H__

#ifdef __GNU_LesserG__
#pragma interface
#endif

#ifdef XB_LOCKING_ON

//! xbLock class
/*!
*/

class XBDLLEXPORT xbLock
{
public:
  xbLock( xbDbf * dbf );
  virtual ~xbLock();
  virtual xbShort LockTableHeader( xbShort LockType ) = 0;
  virtual xbShort LockTable( xbShort LockType ) = 0;
  virtual xbShort LockRecord( xbShort LockType, xbULong RecNo, xbOffT len ) = 0;
  virtual xbShort LockMemo( xbShort LockType ) = 0;
  virtual xbShort LockIndex( xbShort LockType ) = 0;
  virtual xbShort UnlockAll() = 0;
  virtual xbShort LockInit() { return XB_NO_ERROR; }

protected:
  xbDbf   *dbf;
  xbShort HdrLockCnt;
  xbShort TableLockCnt;
  xbShort MemoLockCnt;
  xbShort IndexLockCnt;
  xbShort LockFile( int fn, xbShort LockType, xbOffT lockLen  );
};

class XBDLLEXPORT xbaseLock : xbLock
{
public:
  xbaseLock( xbDbf * pdbf );
  virtual ~xbaseLock() {}
  virtual xbShort LockTableHeader( xbShort LockType );
  virtual xbShort LockTable( xbShort LockType );
  virtual xbShort LockRecord( xbShort LockType, xbULong RecNo, xbOffT len );
  virtual xbShort LockMemo( xbShort LockType ); 
  virtual xbShort LockIndex( xbShort LockType ); 
  virtual xbShort UnlockAll(); 
  virtual xbShort LockInit();
private:
  FILE    *lfh;                    /* lock file handle */

};

class XBDLLEXPORT dbaseLock : xbLock
{
public:
  dbaseLock( xbDbf * pdbf );
  virtual ~dbaseLock() {}
  virtual xbShort LockTableHeader( xbShort LockType );
  virtual xbShort LockTable( xbShort LockType );
  virtual xbShort LockRecord( xbShort LockType, xbULong RecNo, xbOffT len );
  virtual xbShort LockMemo( xbShort LockType ); 
  virtual xbShort LockIndex( xbShort LockType ); 
  virtual xbShort UnlockAll(); 
};


class XBDLLEXPORT clipperLock : xbLock
{
public:
  clipperLock( xbDbf * pdbf );
  virtual ~clipperLock() {}
  virtual xbShort LockTableHeader( xbShort LockType );
  virtual xbShort LockTable( xbShort LockType );
  virtual xbShort LockRecord( xbShort LockType, xbULong RecNo, xbOffT len );
  virtual xbShort LockMemo( xbShort LockType ); 
  virtual xbShort LockIndex( xbShort LockType ); 
  virtual xbShort UnlockAll(); 
};

class XBDLLEXPORT foxproLock : xbLock
{
public:
  foxproLock( xbDbf * pdbf );
  virtual ~foxproLock() {}
  virtual xbShort LockTableHeader( xbShort LockType );
  virtual xbShort LockTable( xbShort LockType );
  virtual xbShort LockRecord( xbShort LockType, xbULong RecNo, xbOffT len );
  virtual xbShort LockMemo( xbShort LockType ); 
  virtual xbShort LockIndex( xbShort LockType ); 
  virtual xbShort UnlockAll(); 
};

class XBDLLEXPORT noLock : xbLock
{
public:
  noLock( xbDbf * pdbf ) : xbLock( pdbf ) {};
  virtual ~noLock() {}
  virtual xbShort LockTableHeader( xbShort LockType )
            { return XB_NO_ERROR; }
  virtual xbShort LockTable( xbShort LockType )
            { return XB_NO_ERROR; }
  virtual xbShort LockRecord( xbShort LockType, xbULong RecNo )
            { return XB_NO_ERROR; }
  virtual xbShort LockMemo( xbShort LockType )
            { return XB_NO_ERROR; }
  virtual xbShort LockIndex( xbShort LockType )
            { return XB_NO_ERROR; }
  virtual xbShort UnlockAll()
            { return XB_NO_ERROR; }
};




#endif   // XB_LOCKING_ON
#endif   // __XB_XBLOCK_H__
