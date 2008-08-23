/*  xbmemo.cpp

    Xbase64 project source code

    This file contains the basic Xbase64 routines for handling
    dBASE III+ and dBASE IV style memo .dbt files


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

#ifdef __WIN32__
#include <xbase64/xbwincfg.h>
#else
#include <xbase64/xbconfig.h>
#endif

#include <xbase64/xbase64.h>
#ifdef XB_MEMO_FIELDS

#include <stdio.h>
//#include <xbase64/xbexcept.h>

#ifdef HAVE_IO_H
#include <io.h>
#endif


/*! \file xbmemo.cpp
*/

/************************************************************************/
//! Short description
/*!
*/
xbLong xbDbf::CalcLastDataBlock()
{
  if( _fseek( mfp, 0, SEEK_END ) != 0 )
    return XB_SEEK_ERROR;
  return ( _ftell( mfp ) / MemoHeader.BlockSize );
}
/************************************************************************/
//! Short description
/*!
  \param BlocksNeeded
  \param Location
  \param PrevNode
*/
xbShort xbDbf::GetBlockSetFromChain( xbLong BlocksNeeded,
   xbLong Location, xbLong PrevNode )

/* this routine grabs a set of blocks out of the free block chain  */  
{
  xbShort rc;
  xbLong NextFreeBlock2, NewFreeBlocks, SaveNextFreeBlock;

  if(( rc = ReadMemoBlock( Location, 2 )) != XB_NO_ERROR )
    return rc;

  if( BlocksNeeded == FreeBlockCnt ){  /* grab this whole set of blocks */
    if( PrevNode == 0 ){    /* first in the chain */
      MemoHeader.NextBlock = NextFreeBlock;
      if(( rc = UpdateHeadNextNode()) != XB_NO_ERROR )
        return rc;
    }
    else  /* remove out of the middle or end */
    {
      NextFreeBlock2 = NextFreeBlock;
      if(( rc = ReadMemoBlock( PrevNode, 2 )) != XB_NO_ERROR )
        return rc;
      NextFreeBlock = NextFreeBlock2;
      if(( rc = WriteMemoBlock( PrevNode, 2 )) != XB_NO_ERROR )
        return rc;
    }
  }

  else  /* only take a portion of this set */
  {
    if( PrevNode == 0 ){  /* first in the set */
      MemoHeader.NextBlock = Location + BlocksNeeded;
      if(( rc = UpdateHeadNextNode()) != XB_NO_ERROR )
        return rc;
      FreeBlockCnt -= BlocksNeeded;
      if(( rc = WriteMemoBlock( MemoHeader.NextBlock, 2 )) != XB_NO_ERROR )
        return rc;
    }
    else  /* remove out of the middle or end */
    {
      NewFreeBlocks = FreeBlockCnt - BlocksNeeded;
      SaveNextFreeBlock = NextFreeBlock; 
      NextFreeBlock2= Location + BlocksNeeded;
      if(( rc = ReadMemoBlock( PrevNode, 2 )) != XB_NO_ERROR )
        return rc;
      NextFreeBlock = NextFreeBlock2;
      if(( rc = WriteMemoBlock( PrevNode, 2 )) != XB_NO_ERROR )
        return rc;
      FreeBlockCnt = NewFreeBlocks;
      NextFreeBlock = SaveNextFreeBlock;
      if(( rc = WriteMemoBlock( NextFreeBlock2, 2 )) != XB_NO_ERROR )
        return rc;
    }
  }
  return 0;
}   
/************************************************************************/
//! Short description
/*!
  \param BlocksNeeded
  \param LastDataBlock
  \param Location
  \param PreviousNode
*/
xbShort xbDbf::FindBlockSetInChain( xbLong BlocksNeeded,
    xbLong LastDataBlock, xbLong &Location, xbLong &PreviousNode )

/* this routine searches thru the free node chain in a dbase IV type   
   memo file searching for a place to grab some free blocks for reuse 

   LastDataBlock- is the last data block in the file, enter 0
                  for the routine to calculate it.
   BlocksNeeded - is the size to look in the chain for
   Location     - is the location it finds
   PreviousNode - is the block number of the node imediately previous
                  to this node in the chain - 0 if header node
   returns - 0 if no spot in chain found
             1 if spot in chain is found
*/
{
  xbShort rc;
  xbLong LDB, PrevNode, CurNode;

  if( LastDataBlock == 0 )
    LDB = CalcLastDataBlock();
  else
    LDB = LastDataBlock;

  if( MemoHeader.NextBlock < LDB ){
    PrevNode = 0L;
    CurNode = MemoHeader.NextBlock;
    if(( rc = ReadMemoBlock( MemoHeader.NextBlock, 2 )) != XB_NO_ERROR )
      return rc;
    while( BlocksNeeded > FreeBlockCnt && NextFreeBlock < LDB ){
      PrevNode = CurNode;
      CurNode = NextFreeBlock;
      if(( rc = ReadMemoBlock( NextFreeBlock, 2 )) != XB_NO_ERROR )
        return rc;
    }
    if( BlocksNeeded <= FreeBlockCnt ){
      Location = CurNode;
      PreviousNode = PrevNode;
      return 1;
    }
    else{   /* no data found and at end of chain */
      PreviousNode = CurNode;
      return 0;
    }
  }
  else{
    PreviousNode = 0;
    return 0;
  }
}
/************************************************************************/
//! Short description
/*!
  \param BlockSize
*/
xbShort xbDbf::SetMemoBlockSize( xbShort BlockSize )
{
  if(IsType3Dbt())
    return XB_NO_ERROR;    // not applicable for type 3
  if( BlockSize % 512 != 0 )
    return XB_INVALID_BLOCK_SIZE;

  MemoHeader.BlockSize = BlockSize;
  return XB_NO_ERROR;
}
/***********************************************************************/
//! Short description
/*!
  \param Option
*/
xbShort xbDbf::GetDbtHeader( xbShort Option )
{
  char *p;
  xbShort i;
  char MemoBlock[24];

  /*  Option = 0  -->  read only first four bytes
               1  -->  read the entire thing  */

  if( !mfp )
    return XB_NOT_OPEN;

  if( _fseek( mfp, 0, SEEK_SET ))
    return XB_SEEK_ERROR;

  if(( fread( MemoBlock, 24, 1, mfp )) != 1 )
    return XB_READ_ERROR;

  p = MemoBlock;
  MemoHeader.NextBlock = xbase->GetLong( p ); 
  if(IsType3Dbt() || Option == 0)
    return XB_NO_ERROR;
 
  /* version IV stuff follows */
  p+=8;
  for( i = 0; i < 8; i++, p++ ) 
    MemoHeader.FileName[i] = *p;
  MemoHeader.Version  = *p;
  p+=4;
  MemoHeader.BlockSize = xbase->GetShort( p );
  return XB_NO_ERROR;
}

/***********************************************************************/
xbShort xbDbf::OpenFPTFile() 
{
  if (GetFileName().len() < 3)
    return XB_INVALID_NAME;

  xbShort len  = GetFileName().len() - 1;
  xbString ext = GetFileName().mid(len-2, 3);
  MemofileName = GetFileName().mid(0, len-2);
  if (ext == "DBF")
    MemofileName += "FPT";
  else
    if (ext = "dbf")
      MemofileName += "fpt";
    else
      return XB_INVALID_NAME;
  if ((mfp = fopen(MemofileName, "r+b" )) == NULL){
    //
    //  Try to open read only if can't open read/write
    //
    if ((mfp = fopen(MemofileName, "rb" )) == NULL)
      return XB_OPEN_ERROR;
  }
  char header[8];
  if ((fread(header, 8, 1, mfp)) != 1)
    return XB_READ_ERROR;

  char *p = header;
  MemoHeader.NextBlock = xbase->GetHBFULong(p);
  p += 6;
  MemoHeader.BlockSize = xbase->GetHBFShort(p);
  return XB_NO_ERROR;
}
/***********************************************************************/
//! Short description
/*!
*/
xbShort xbDbf::OpenMemoFile()
{
  if (Version == (char)0xf5 || Version == (char)0x30) 
    return OpenFPTFile();

  xbShort len, rc;
  xbOffT Size, NewSize, l;
  MemofileName = GetFileName();
  len = GetFileName().len() - 1;
  if( MemofileName[len] == 'F' )
    MemofileName.putAt(len, 'T');
  else if( MemofileName[len] == 'f' )
    MemofileName.putAt(len, 't');
  else
    return XB_INVALID_NAME;

  if(( mfp = fopen( MemofileName, "r+b" )) == NULL ){
    //
    //  Try to open read only if can't open read/write
    //
    if(( mfp = fopen( MemofileName, "rb" )) == NULL )
      return XB_OPEN_ERROR;
  }
#ifdef XB_LOCKING_ON
  setbuf( mfp, NULL );
#endif
  if(( rc = GetDbtHeader(1)) != 0 ){
    fclose( mfp );
    return rc;
  }

  len = GetMemoBlockSize();
  if( len == 0 || ((len % 512) != 0 )){
    fclose( mfp );
    return XB_INVALID_BLOCK_SIZE;
  }

  /* logic to verify file size is a multiple of block size */
  if(( rc = _fseek( mfp, 0, SEEK_END )) != 0 ){
    fclose( mfp );
    return XB_SEEK_ERROR;
  }

  /* if the file is not a multiple of block size, fix it, append nulls */
  Size = _ftell( mfp );
  if(( Size % MemoHeader.BlockSize ) != 0 ){
    NewSize = ( Size / MemoHeader.BlockSize + 1) * MemoHeader.BlockSize;
    for( l = Size; l < NewSize; l++ )
      fputc( 0x00, mfp );
  }

  if(( mbb = (void *) malloc(len)) == NULL ){
    fclose( mfp );
    return XB_NO_MEMORY;
  }
  return XB_NO_ERROR;
}
/***********************************************************************/
//! Short description
/*!
*/
xbShort xbDbf::CreateMemoFile( void )
{
  xbShort len,i;
  char  *sp;
  char  buf[4];

  len = GetMemoBlockSize();
  if( len == 0 || len % 512 != 0 )
    return XB_INVALID_BLOCK_SIZE;

  if(( sp = (char*)strrchr(GetFileName(), PATH_SEPARATOR)) != NULL )
    sp++;
  else
    sp = MemoHeader.FileName;

  memset( MemoHeader.FileName, 0x00, 8 );

  for( i = 0; i < 8 && *sp != '.'; i++ )
    MemoHeader.FileName[i] = *sp++;

  MemofileName = GetFileName();

  len = GetFileName().len() - 1;
  if( MemofileName[len] == 'F' )
    MemofileName.putAt(len, 'T');
  else if( MemofileName[len] == 'f' )
    MemofileName.putAt(len, 't');
  else
    return XB_INVALID_NAME;

  /* Initialize the variables */
  MemoHeader.NextBlock = 1L;

  if(( mfp = fopen( MemofileName, "w+b" )) == NULL )
    return XB_OPEN_ERROR;
#ifdef XB_LOCKING_ON
  setbuf( mfp, NULL );
#endif

  if(( _fseek( mfp, 0, SEEK_SET )) != 0 ){
    fclose( mfp );
    return XB_SEEK_ERROR;
  }

  memset( buf, 0x00, 4 );
  xbase->PutLong( buf, MemoHeader.NextBlock );
  if(( fwrite( &buf, 4, 1, mfp )) != 1 ){
    fclose( mfp );
    return XB_WRITE_ERROR;
  }

  if( IsType3Dbt() ){   /* dBASE III+ */
    for( i = 0; i < 12; i++ )  fputc( 0x00, mfp );
    fputc( 0x03, mfp );
    for( i = 0; i < 495; i++ ) fputc( 0x00, mfp );
  }
  else
  {
    for( i = 0; i < 4; i++ )  fputc( 0x00, mfp );
    fwrite( &MemoHeader.FileName,  8, 1, mfp );
    for( i = 0; i < 4; i++ )  fputc( 0x00, mfp );
    memset( buf, 0x00, 2 );
    xbase->PutShort( buf, MemoHeader.BlockSize );
    if(( fwrite( &buf, 2, 1, mfp )) != 1 ){
      fclose( mfp );
      return XB_WRITE_ERROR;
    }
    for( i = 22; i <  MemoHeader.BlockSize; i++ ) fputc( 0x00, mfp );
  }
        
  if(( mbb = (void *) malloc( MemoHeader.BlockSize )) == NULL ){
    fclose( mfp );
    return XB_NO_MEMORY;
  }
  return XB_NO_ERROR;
}
/***********************************************************************/
//! Short description
/*!
  \param BlockNo
  \param Option
*/

/* Option = 0 - 1st Block of a set of valid data blocks, load buckets  */
/* Option = 1 - subsequant block of data in a multi block set or db III*/
/* Option = 2 - 1st block of a set of free blocks, load buckets        */                     
/* Option = 3 - read 8 bytes of a block, don't load any buckets        */
/* Option = 4 - read 8 bytes of a block, load data buckets             */

xbShort xbDbf::ReadMemoBlock( xbLong BlockNo, xbShort Option )
{
  size_t ReadSize;
  CurMemoBlockNo = -1;

  if( BlockNo < 1L )
    return XB_INVALID_BLOCK_NO;

  if( _fseek( mfp,((xbOffT)BlockNo*MemoHeader.BlockSize), SEEK_SET ))
    return XB_SEEK_ERROR;


  if( Option ==  0 || Option == 1 )
    ReadSize = MemoHeader.BlockSize;
  else
    ReadSize = 8L;

  if(fread( mbb, ReadSize, 1, mfp ) != 1 )
    return XB_READ_ERROR;

  if( Option == 0 || Option == 4){ // 1st block of a set of valid data blocks
    mfield1   = xbase->GetShort( (char *) mbb );
    MStartPos = xbase->GetShort( (char *) mbb+2 );
    MFieldLen = xbase->GetLong ( (char *) mbb+4 );
  }
  else if( Option == 2 ){    // 1st block of a set of free blocks
    NextFreeBlock = xbase->GetLong( (char *) mbb );
    FreeBlockCnt  = xbase->GetLong( (char *) mbb+4 );
  }
   
  if( Option ==  0 || Option == 1 )
    CurMemoBlockNo = BlockNo;

  return XB_NO_ERROR;
}
/************************************************************************/
//! Short description
/*!
  \param BlockNo
  \param Option
*/
xbShort xbDbf::WriteMemoBlock( xbLong BlockNo, xbShort Option )
{
/* Option = 0 - 1st Block of a set of valid data blocks, set buckets    */
/* Option = 1 - subsequant block of data in a multi block set or db III */
/* Option = 2 - 1st block of a set offree blocks, set buckets           */                     

  xbLong WriteSize;

  if( BlockNo < 1L )
    return XB_INVALID_BLOCK_NO;

  CurMemoBlockNo = -1;

  if( Option == 0 ){
    xbase->PutShort( (char *) mbb, mfield1 );
    xbase->PutShort( (char *) mbb+2, MStartPos );
    xbase->PutLong ( (char *) mbb+4, MFieldLen );
    WriteSize = MemoHeader.BlockSize;
  }
  else if( Option == 2 ){
    xbase->PutLong((char *) mbb, NextFreeBlock );
    xbase->PutLong((char *) mbb+4, FreeBlockCnt );
    WriteSize = 8L;
  }
  else
    WriteSize = MemoHeader.BlockSize;

  if( _fseek( mfp,((xbOffT)BlockNo*MemoHeader.BlockSize), SEEK_SET ))
    return XB_SEEK_ERROR;

  if(( fwrite( mbb, WriteSize, 1, mfp )) != 1 ) 
    return XB_WRITE_ERROR;

  if( Option == 0 || Option == 1 ) 
    CurMemoBlockNo = BlockNo;

  return XB_NO_ERROR;
}
/***********************************************************************/
//! Short description
/*!
  \param FieldNo
  \param len
  \param Buf
  \param LockOpt
*/
xbShort xbDbf::GetFPTField(xbShort FieldNo, xbLong len,
                           char * Buf, xbShort LockOpt) {

 if (FieldNo < 0 || FieldNo > (NoOfFields - 1))
   return XB_INVALID_FIELDNO;

 if (GetFieldType(FieldNo) != 'M')
   return XB_NOT_MEMO_FIELD;

#ifdef XB_LOCKING_ON
//  if( LockOpt != -1 )
//    if( LockMemoFile( XB_LOCK ) != XB_NO_ERROR )
//      return XB_LOCK_FAILED;
#endif

  xbLong BlockNo;
  char buf[18];

  if( Version == (char)0x30 ) {
    memset( buf, 0x00, 18 ) ;
    GetField( FieldNo, buf );
    BlockNo = xbase->GetLong((char*) buf);
  } else {
    BlockNo = GetLongField(FieldNo);
  }  

  if ( BlockNo == 0L )
    return 0L;

  // Seek to start_of_block + 4


// FIXME LOCK

#ifdef XB_LOCKING_ON
//  try {
#endif
    if (_fseek(mfp, ((xbOffT)BlockNo * MemoHeader.BlockSize + 4), SEEK_SET) != 0)
      return XB_SEEK_ERROR;
    char h[4];
    if ((fread(h, 4, 1, mfp)) != 1)
     return XB_READ_ERROR;

    xbULong fLen = xbase->GetHBFULong(h);

    xbULong l = (fLen < (xbULong)len) ? fLen : len;
    if ((fread(Buf, l, 1, mfp)) != 1)
     return XB_READ_ERROR;
    Buf[l]=0;
#ifdef XB_LOCKING_ON
//  }
// catch (...) {
//     if (LockOpt != -1)
//       LockMemoFile( XB_UNLOCK );
//     throw;
//  }
#endif

#ifdef XB_LOCKING_ON
//  if (LockOpt != -1)
//    LockMemoFile( XB_UNLOCK );
#endif

  return XB_NO_ERROR;
}
/***********************************************************************/
//! Short description
/*!
  \param FieldNo
  \param len
  \param Buf
  \param LockOpt
*/
xbShort xbDbf::GetMemoField( xbShort FieldNo, xbLong len,
        char * Buf, xbShort LockOpt )
{
  if( Version == (char)0xf5 || Version == (char)0x30 )
    return GetFPTField(FieldNo, len, Buf, LockOpt);

  xbLong BlockNo, Tcnt, Scnt;
  char *tp, *sp;           /* target and source pointers */
  xbShort rc;
  xbShort Vswitch;
  xbLong  MemoLen;

  if( FieldNo < 0 || FieldNo > ( NoOfFields - 1 ))
    return XB_INVALID_FIELDNO;

  if( GetFieldType( FieldNo ) != 'M' )
    return XB_NOT_MEMO_FIELD;

#ifdef XB_LOCKING_ON
//  if( LockOpt != -1 )
//    if(( rc = LockMemoFile( LockOpt, XB_LOCK )) != XB_NO_ERROR )
//      return XB_LOCK_FAILED;
#endif

  if(( BlockNo = GetLongField( FieldNo )) == 0 ){
#ifdef XB_LOCKING_ON
//    if( LockOpt != -1 )
//      LockMemoFile( XB_UNLOCK );
#endif
    return XB_NO_MEMO_DATA;
  }

  if( IsType3Dbt() )
    Vswitch = 1;
  else
    Vswitch = 0;

  if(( rc = ReadMemoBlock( BlockNo, Vswitch )) != 0 ){
#ifdef XB_LOCKING_ON
//    if( LockOpt != -1 )
//      LockMemoFile( XB_UNLOCK );
#endif
    return rc;
  }

  tp = Buf;
  sp = (char *) mbb;

  if( IsType4Dbt() ){
    sp+=8;
    Scnt = 8L;  
  }
  else
    Scnt = 0L;

  Tcnt = 0L;
  MemoLen = GetMemoFieldLen( FieldNo );
  while( Tcnt < len && Tcnt < MemoLen ){
    *tp++ = *sp++;
    Scnt++;
    Tcnt++;
    if( Scnt >= MemoHeader.BlockSize ){
      BlockNo++;
      if(( rc = ReadMemoBlock( BlockNo, 1 )) != 0 )
        return rc;
      Scnt = 0;
      sp = (char *) mbb;
    }
  } 
#ifdef XB_LOCKING_ON
  //if( LockOpt != -1 )
//    LockMemoFile( XB_LOCK );
#endif

  return XB_NO_ERROR;
}
/***********************************************************************/
//! Short description
/*!
  \param FieldNo
*/
xbLong xbDbf::GetFPTFieldLen( xbShort FieldNo )
{
  xbLong  BlockNo;
  if(( BlockNo = GetLongField(FieldNo)) == 0L )
    return 0L;
  // Seek to start_of_block + 4
  if(_fseek(mfp, ((xbOffT)BlockNo * MemoHeader.BlockSize + 4), SEEK_SET) != 0)
    return XB_SEEK_ERROR;
  char h[4];
  if((fread(h, 4, 1, mfp)) != 1)
    return XB_READ_ERROR;

  return xbase->GetHBFULong(h);
}
/***********************************************************************/
//! Short description
/*!
  \param FieldNo
*/
xbLong xbDbf::GetMemoFieldLen( xbShort FieldNo ) {
  if (Version == (char)0xf5 || Version == (char)0x30 )
    return GetFPTFieldLen(FieldNo);

  xbLong  BlockNo, ByteCnt;
  xbShort scnt, NotDone;
  char *sp, *spp;

  if(( BlockNo = GetLongField( FieldNo )) == 0L )
    return 0L;

  if( IsType4Dbt()){   /* dBASE IV */
    if( BlockNo == CurMemoBlockNo && CurMemoBlockNo != -1 )
      return MFieldLen - MStartPos;
    if( ReadMemoBlock( BlockNo, 0 ) != XB_NO_ERROR )
      return 0L;
    return MFieldLen - MStartPos;
  } else { /* version 0x03 dBASE III+ */
    ByteCnt = 0L;
    spp = NULL;
    NotDone = 1;
    while( NotDone ){
      if( ReadMemoBlock( BlockNo++, 1 ) != XB_NO_ERROR )
        return 0L;
      scnt = 0;
      sp = (char *) mbb;
      while( scnt < 512 && NotDone ){
        if( *sp == 0x1a && *spp == 0x1a )
          NotDone = 0;
        else{
          ByteCnt++; scnt++; spp = sp; sp++;
        }
      }
    }
    if( ByteCnt > 0 ) ByteCnt--;
    return ByteCnt;
  }
}
/***********************************************************************/
//! Short description
/*!
*/
xbShort xbDbf::MemoFieldsPresent() const
{
  xbShort i;
  for( i = 0; i < NoOfFields; i++ )
    if( GetFieldType( i ) == 'M' )
      return 1;

  return 0;
}
/***********************************************************************/
//! Short description
/*!
  \param FieldNo
*/
xbShort xbDbf::DeleteMemoField( xbShort FieldNo )
{
  xbLong SBlockNo, SNoOfBlocks, SNextBlock;
  xbLong LastFreeBlock, LastFreeBlockCnt, LastDataBlock;
  xbShort rc;

  NextFreeBlock    = 0L;
  LastFreeBlockCnt = 0L;
  LastFreeBlock    = 0L;

  if( IsType3Dbt() ){    /* type III */
    PutField( FieldNo, "          " );
    return XB_NO_ERROR;
  }

  /* Get Block Number */
  if(( SBlockNo = GetLongField( FieldNo )) == 0 )
    return XB_INVALID_BLOCK_NO;

  /* Load the first block */

  if(( rc = ReadMemoBlock( SBlockNo, 4 )) != XB_NO_ERROR )
    return rc;

  if( (MFieldLen+2) % MemoHeader.BlockSize )
    SNoOfBlocks = (MFieldLen+2)/MemoHeader.BlockSize+1L;
  else
    SNoOfBlocks = (MFieldLen+2)/MemoHeader.BlockSize;

  /* Determine last good data block */
  LastDataBlock = CalcLastDataBlock();

  /* position to correct location in chain */
  NextFreeBlock = MemoHeader.NextBlock;
  while( SBlockNo > NextFreeBlock && SBlockNo < LastDataBlock ){
    LastFreeBlock    = NextFreeBlock;
    if(( rc = ReadMemoBlock( NextFreeBlock, 2 )) != XB_NO_ERROR )
      return rc;
    LastFreeBlockCnt = FreeBlockCnt;
  }


  /* if next block should be concatonated onto the end of this set */

  if((SBlockNo+SNoOfBlocks) == NextFreeBlock && NextFreeBlock < LastDataBlock )
  {
    if(( rc = ReadMemoBlock( NextFreeBlock, 2 )) != XB_NO_ERROR )
      return XB_NO_ERROR;
    SNoOfBlocks += FreeBlockCnt;
    SNextBlock = NextFreeBlock;
  } else if( LastFreeBlock == 0L )
    SNextBlock = MemoHeader.NextBlock;
  else
    SNextBlock = NextFreeBlock;

  /* if this is the first set of free blocks */
  if( LastFreeBlock == 0L ){
    /* 1 - write out the current block */
    /* 2 - update header block         */
    /* 3 - write header block          */
    /* 4 - update data field           */

    NextFreeBlock = SNextBlock;
    FreeBlockCnt = SNoOfBlocks;
    if(( rc = WriteMemoBlock( SBlockNo, 2 )) != XB_NO_ERROR )
      return rc;

    MemoHeader.NextBlock = SBlockNo;
    if(( rc = UpdateHeadNextNode()) != XB_NO_ERROR )
      return rc;
    PutField( FieldNo, "          " );
    return XB_NO_ERROR;
  }

/* determine if this block set should be added to the previous set */

  if(( LastFreeBlockCnt + LastFreeBlock ) == SBlockNo ){
    if(( rc = ReadMemoBlock( LastFreeBlock, 2 )) != XB_NO_ERROR )
      return rc;
    NextFreeBlock = SNextBlock;
    FreeBlockCnt += SNoOfBlocks;
    if(( rc = WriteMemoBlock( LastFreeBlock, 2 )) != XB_NO_ERROR )
      return rc;
    PutField( FieldNo, "          " );
    return XB_NO_ERROR;
  }

  /* insert into the chain */
  /* 1 - set the next bucket on the current node         */
  /* 2 - write this node                                 */
  /* 3 - go to the previous node                         */
  /* 4 - insert this nodes id into the previous node set */
  /* 5 - write previous node                             */

  FreeBlockCnt = SNoOfBlocks;
  if(( rc = WriteMemoBlock( SBlockNo, 2 )) != XB_NO_ERROR )
    return rc;
  if(( rc = ReadMemoBlock( LastFreeBlock, 2 )) != XB_NO_ERROR )
    return rc;
  NextFreeBlock = SBlockNo;
  if(( rc = WriteMemoBlock( LastFreeBlock, 2 )) != XB_NO_ERROR )
    return rc;
  PutField( FieldNo, "          " );

  return XB_NO_ERROR;
}
/***********************************************************************/
//! Short description
/*!
  \param FieldNo
  \param DataLen
  \param Buf
*/

xbShort xbDbf::AddMemoData( xbShort FieldNo, xbLong DataLen,
         const char * Buf )
{
  xbShort rc;
  xbLong  BlocksNeeded, LastDataBlock;
  xbLong  PrevNode, HeadBlock;
  xbLong  TotalLen;       /* total length of needed area for memo field */

  TotalLen = DataLen+2;
  LastDataBlock = CalcLastDataBlock();

  if( IsType3Dbt() ||                          /* always append to end */
    ( LastDataBlock == MemoHeader.NextBlock )){ /* no free space */
    if( TotalLen % MemoHeader.BlockSize )
      BlocksNeeded = TotalLen / MemoHeader.BlockSize + 1;
    else
      BlocksNeeded = TotalLen / MemoHeader.BlockSize;

    MemoHeader.NextBlock = LastDataBlock + BlocksNeeded;  /* reset to eof */
    if(( rc = PutMemoData( LastDataBlock, BlocksNeeded, DataLen, Buf ))
         != XB_NO_ERROR )
      return rc;
    HeadBlock = LastDataBlock;
    if(( rc = UpdateHeadNextNode()) != XB_NO_ERROR )
      return rc;
  }else{
    TotalLen += 8;
    if( TotalLen % MemoHeader.BlockSize )
      BlocksNeeded = TotalLen / MemoHeader.BlockSize + 1;
    else
      BlocksNeeded = TotalLen / MemoHeader.BlockSize;

    if(( rc = FindBlockSetInChain( BlocksNeeded, LastDataBlock,
              HeadBlock, PrevNode )) == 1 ){
      if(( rc = GetBlockSetFromChain( BlocksNeeded, HeadBlock, PrevNode ))
             != XB_NO_ERROR )
        return rc;
      if(( rc = PutMemoData( HeadBlock, BlocksNeeded, DataLen, Buf )) 
             != XB_NO_ERROR )
        return rc;
    } else { /* append to the end */
      /* if header block needed updated, already done by here */
      if(( rc = PutMemoData( LastDataBlock, BlocksNeeded, DataLen, Buf ))
              != XB_NO_ERROR )
        return rc;
      HeadBlock = LastDataBlock;
      if(( rc = ReadMemoBlock( PrevNode, 2 )) != XB_NO_ERROR )
        return rc;
      NextFreeBlock += BlocksNeeded;
      if(( rc = WriteMemoBlock( PrevNode, 2 )) != XB_NO_ERROR )
        return rc;
    }
  }
  PutLongField( FieldNo, HeadBlock );
  return XB_NO_ERROR;
}
/***********************************************************************/
//! Short description
/*!
*/
xbShort xbDbf::UpdateHeadNextNode() const
{
  char buf[4];
  memset( buf, 0x00, 4 );
  xbase->PutLong( buf, MemoHeader.NextBlock );
  if(( _fseek( mfp, 0, SEEK_SET )) != 0 )
    return XB_SEEK_ERROR;

  if(( fwrite( &buf, 4, 1, mfp )) != 1 )
    return XB_WRITE_ERROR;

  return XB_NO_ERROR;
}
/***********************************************************************/
//! Short description
/*!
  \param StartBlock    First block to write
  \param BlocksNeeded  Total number of blocks needed
  \param DataLen       Length of data to write
  \param Buf           Actual data
*/
xbShort xbDbf::PutMemoData( xbLong StartBlock, xbLong BlocksNeeded, 
   xbLong DataLen, const char *Buf )
{
  xbShort i, rc, Tctr;
  xbShort BytesProcessed;          // bytes processed so far
  xbShort TotalLen;                // total length of data
  xbLong  CurBlock;
  char *tp;
  const char *sp;

  TotalLen = DataLen + 2;
  CurBlock = StartBlock;
  memset( (char *) mbb, 0x00, MemoHeader.BlockSize );
  tp = (char *) mbb;
  sp = Buf;
  BytesProcessed = 0;   /* total length processed */


  if( IsType3Dbt() )
    Tctr = 0;
  else{  /* dBASE IV */
    tp += 8;
    Tctr = 8;
  }

  for( i = 0; i < BlocksNeeded; i++ ){
    while( Tctr < MemoHeader.BlockSize && BytesProcessed < TotalLen ){
      if( BytesProcessed >= DataLen )
        *tp++ = 0x1a;    /* end of data marker */
      else
        *tp++ = *sp++;   /* copy data to memo block buffer */
      Tctr++; 
      BytesProcessed++;
    }

    /* if incomplete block, finish it out with 0x00 */
    while( Tctr++ < MemoHeader.BlockSize )
      *tp++ = 0x00;

    if( i == 0 && IsType4Dbt() ){
      mfield1 = -1;
      MStartPos = 8;
      MFieldLen = DataLen + MStartPos;
      if(( rc = WriteMemoBlock( CurBlock++, 0 )) != XB_NO_ERROR )
        return rc;
    } else {
      if(( rc = WriteMemoBlock( CurBlock++, 1 )) != XB_NO_ERROR )
        return rc;
    }
    Tctr = 0;
    tp = (char *) mbb;
  }
  return XB_NO_ERROR;
}
/***********************************************************************/
//! Short description
/*!
  \param FieldNo
  \param DataLen
  \param Buf
  \param LockOpt
*/
xbShort xbDbf::UpdateMemoData( xbShort FieldNo, xbLong DataLen, 
     const char * Buf, xbShort LockOpt )
{
  xbShort rc;
  xbLong  TotalLen;
  xbLong  BlocksNeeded, BlocksAvailable;

  #ifdef XB_LOCKING_ON
  if( LockOpt != -1 )
//    if(( rc = LockMemoFile( XB_LOCK )) != XB_NO_ERROR )
//      return XB_LOCK_FAILED;
  #endif

  if( DataLen ){
    TotalLen = DataLen + 2;              //  add 2 eod 0x1a chars
    if( IsType4Dbt()) TotalLen += 8;     //  leading fields for dbase iv
  }
  else
    TotalLen = 0;

  if( DataLen == 0L ){   /* handle delete */
    if( MemoFieldExists( FieldNo )){
      if(( rc = DeleteMemoField( FieldNo )) != XB_NO_ERROR ){
         #ifdef XB_LOCKING_ON
//         LockMemoFile( XB_UNLOCK );
         #endif
         return rc;
      }
    }
  } else if((IsType3Dbt() || GetMemoFieldLen(FieldNo)==0L)){
    if(( rc = AddMemoData( FieldNo, DataLen, Buf )) != XB_NO_ERROR ){
      #ifdef XB_LOCKING_ON
//      LockMemoFile( XB_UNLOCK );
      #endif
      return rc;
    }
  } else {   /* version IV type files, reuse unused space */
    if( TotalLen % MemoHeader.BlockSize )
      BlocksNeeded = TotalLen / MemoHeader.BlockSize + 1;
    else
      BlocksNeeded = TotalLen / MemoHeader.BlockSize;

    if(( rc = ReadMemoBlock( GetLongField( FieldNo ), 4 )) != XB_NO_ERROR ){
      #ifdef XB_LOCKING_ON
//      LockMemoFile( XB_UNLOCK );
      #endif
      return rc;
    }

    if( (MFieldLen+2) % MemoHeader.BlockSize )
      BlocksAvailable = (MFieldLen+2) / MemoHeader.BlockSize + 1;
    else
      BlocksAvailable = (MFieldLen+2) / MemoHeader.BlockSize;

    if( BlocksNeeded == BlocksAvailable ){
      if(( rc = PutMemoData( GetLongField( FieldNo ), BlocksNeeded,
           DataLen, Buf )) != XB_NO_ERROR ){
        #ifdef XB_LOCKING_ON
//        LockMemoFile( XB_UNLOCK );
        #endif
        return rc;
      }
    } else {
      if(( rc = DeleteMemoField( FieldNo )) != XB_NO_ERROR ){
        #ifdef XB_LOCKING_ON
//        LockMemoFile( XB_UNLOCK );
        #endif
        return rc;
       }
       if(( rc = AddMemoData( FieldNo, DataLen, Buf )) != XB_NO_ERROR ){
         #ifdef XB_LOCKING_ON
//         LockMemoFile( XB_UNLOCK );
         #endif
         return rc;
       }
     }
  }


  #ifdef XB_LOCKING_ON
//  if( LockOpt != -1 )
//    if(( rc = LockMemoFile( XB_UNLOCK )) != XB_NO_ERROR )
//      return XB_LOCK_FAILED;
  #endif
  return XB_NO_ERROR;
}
/***********************************************************************/
//! Short description
/*!
  \param FieldNo
*/
xbShort xbDbf::MemoFieldExists( xbShort FieldNo ) const
{
  if( GetLongField( FieldNo ) == 0L )
     return 0;
  else
     return 1;
}
/***********************************************************************/
//! Short description
/*!
*/
#ifdef XBASE_DEBUG
void xbDbf::DumpMemoHeader() const
{
  xbShort i;
  std::cout << "\n*********************************";
  std::cout << "\nMemo header data...";
  std::cout << "\nNext Block " << MemoHeader.NextBlock;
  if( IsType4Dbt() ){
    std::cout << "\nFilename   ";
    for( i = 0; i < 8; i++ )
      std::cout << MemoHeader.FileName[i];
  }
  std::cout << "\nBlocksize  " << MemoHeader.BlockSize;
  return;
}
/***********************************************************************/
//! Short description
/*!
*/
xbShort xbDbf::DumpMemoFreeChain() 
{
  xbShort rc;
  xbLong  CurBlock, LastDataBlock;

  if(( rc = GetDbtHeader(1)) != XB_NO_ERROR )
    return rc;
  LastDataBlock = CalcLastDataBlock();
  CurBlock = MemoHeader.NextBlock;
  std::cout << "Total blocks in file = " << LastDataBlock << std::endl;
  std::cout << "Head Next Block      = " << CurBlock      << std::endl;;
  while( CurBlock < LastDataBlock ){
    if(( rc = ReadMemoBlock( CurBlock, 2 )) != XB_NO_ERROR )
      return rc;
    std::cout << "**********************************"     << std::endl;
    std::cout << "This Block = "   << CurBlock            << std::endl;
    std::cout << "Next Block = "   << NextFreeBlock       << std::endl;
    std::cout << "No Of Blocks = " << FreeBlockCnt        << std::endl;
    CurBlock = NextFreeBlock;
  }
  return XB_NO_ERROR;
}
/***********************************************************************/
//! Short description
/*!
*/
void xbDbf::DumpMemoBlock() const
{
  xbShort i;
  char  *p;
  p = (char *) mbb;
  if( IsType3Dbt() ){
    for( i = 0; i < 512; i++ )
      std::cout << *p++;
  } else {
    std::cout << "\nField1     => " << mfield1;
    std::cout << "\nStart Pos  => " << MStartPos;
    std::cout << "\nField Len  => " << MFieldLen;
    std::cout << "\nBlock data => ";
    p += 8;
    for( i = 8; i < MemoHeader.BlockSize; i++ )
      std::cout << *p++;
  }
  return;
}
#endif  /* XBASE_DEBUG */
#endif  /* MEMO_FIELD */
