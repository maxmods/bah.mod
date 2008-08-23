/*  xbindex.cpp

    Xbase64 project source code
   
    This file contains the implementation of the xbIndex class.

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
  #pragma implementation "xbindex.h"
#endif

#ifdef __WIN32__
#include <xbase64/xbwincfg.h>
#else
#include <xbase64/xbconfig.h>
#endif

#include <xbase64/xbase64.h>

#include <stdio.h>
#include <stdlib.h>

/*! \file xbindex.cpp
*/

#ifdef XB_INDEX_ANY
//! Constructor
/*!
  \param pdbf
*/
xbIndex::xbIndex(xbDbf * pdbf)
{
  index          = this;
  dbf            = pdbf;
//  ExpressionTree = NULL;
  IxExp          = NULL;
  indexfp        = NULL;
//  IndexStatus    = 0;
  CurDbfRec      = 0L;
  KeyBuf         = NULL;
  KeyBuf2        = NULL;
#ifdef XB_LOCKING_ON
  LockCnt        = 0;

  CurLockCount   = 0;
  CurLockType    = -1;
#endif // XB_LOCKING_ON
}

/*************************************************************************/

//! Destructor
/*!
  \param pdbf
*/
xbIndex::~xbIndex()
{
  if( IxExp ){
    delete IxExp;
    IxExp = NULL;
  }
}
/*************************************************************************/

void xbIndex::Flush()
{
  if(indexfp) fflush(indexfp);
}

/*************************************************************************/
xbShort xbIndex::OpenIndex(const char* FileName)
{
  if (IsOpen()) return XB_ALREADY_OPEN;

  int rc;

  SetFileName(FileName);

  /* open the file */
  if(( indexfp = fopen( GetFileName(), "r+b" )) == NULL ){
  //
  //  Try to open read only if can't open read/write
  //
    if(( indexfp = fopen( GetFileName(), "rb" )) == NULL )
      return XB_OPEN_ERROR;
  }

#ifdef XB_LOCKING_ON
   /*
   **  Must turn off buffering when multiple programs may be accessing
   **  index files.
   */
  setbuf( indexfp, NULL );
#endif

//  IndexStatus = 1;
  if(( rc = GetHeadNode()) != 0){
    fclose( indexfp );
    return rc;
  }

  /* parse the expression */
/* pre 3.0
  if(( rc = dbf->xbase->BuildExpressionTree( HeadNode.KeyExpression,
      strlen( HeadNode.KeyExpression ), dbf )) != XB_NO_ERROR ){
    return rc;
  }
  ExpressionTree = dbf->xbase->GetTree();
  dbf->xbase->SetTreeToNull();
*/
  IxExp = new xbExpn( dbf->xbase );
  if(( rc = IxExp->BuildExpressionTree( GetKeyExpression(),
      strlen( GetKeyExpression() ), dbf )) != XB_NO_ERROR ){
    fclose( indexfp );
    return rc;
  }  
  
  rc=AllocKeyBufs();
  if(rc){
#ifdef XB_LOCKING_ON
//      if( dbf->GetAutoLock() )
//         LockIndex( XB_UNLOCK );
#endif
    fclose(indexfp);
    return rc;     
  }

#ifdef XBASE_DEBUG
//   CheckIndexIntegrity( 0 );
#endif

#ifdef XB_LOCKING_ON
  //if( dbf->GetAutoLock() )
//    LockIndex( XB_UNLOCK );
#endif
  
  rc = dbf->AddIndexToIxList( index, GetFileName() );
  return rc;
}
/*************************************************************************/
//! Short description.
/*!
*/
xbShort xbIndex::AllocKeyBufs()
{
  KeyBuf  = (char *) malloc( GetKeyLen() + 1 );
  if(KeyBuf==NULL) {
    return XB_NO_MEMORY;
  };
  KeyBuf2 = (char *) malloc( GetKeyLen() + 1);
  if(KeyBuf2==NULL) {
    free(KeyBuf);
    return XB_NO_MEMORY;    
  };
  memset( KeyBuf,  0x00, GetKeyLen() + 1 );
  memset( KeyBuf2, 0x00, GetKeyLen() + 1 );
  return XB_NO_ERROR;
}
/***********************************************************************/

xbShort xbIndex::CloseIndex( void )
{
  if(KeyBuf){
    free(KeyBuf);
    KeyBuf = NULL;
  }
  if(KeyBuf2){
    free(KeyBuf2);
    KeyBuf2 = NULL;
  }

  dbf->RemoveIndexFromIxList( index );  // why not 'this'?
  FreeNodesMemory();
  if( IxExp ){
    delete IxExp;
    IxExp = 0;
  }
  
  if(indexfp){
    fclose( indexfp );
    indexfp = NULL;
  }
//  IndexStatus = 0;
  return 0;
}
/***********************************************************************/

#endif    // XB_INDEX_ANY
