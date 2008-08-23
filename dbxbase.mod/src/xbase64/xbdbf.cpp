/*  xbdbf.cpp
 
    Xbase64 project source code
   
    This file contains the basic Xbase routines for reading and writing
    Xbase .DBF files.

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
  #pragma implementation "xbdbf.h"
#endif

#ifdef __WIN32__
#include <xbase64/xbwincfg.h>
#else
#include <xbase64/xbconfig.h>
#endif

#include <xbase64/xbase64.h>

#ifdef HAVE_IO_H
#include <io.h>
#endif
#include <errno.h>

/*! \file xbdbf.cpp
*/

/************************************************************************/
//! Constructor
/*!
  \param x pointer to the global xbXbase class
*/
xbDbf::xbDbf( xbXBase * x )
{
  xbase = x;
  InitVars();
}
/************************************************************************/
//! Destructor
/*!
*/
xbDbf::~xbDbf()
{
  CloseDatabase(true);
}
/************************************************************************/
//! Initialize private data members.
/*!
  Internal use only.
*/
void xbDbf::InitVars()
{
  SetFileName(NULL);
  NoOfFields   = 0;
  DbfStatus    = XB_CLOSED;
  fp           = NULL;
  CurRec       = 0L;
  SchemaPtr    = NULL;
  RecBuf       = NULL;
  RecBuf2      = NULL;
  Version      = 0x00;
  UpdateYY     = 0x00;
  UpdateMM     = 0x00;
  UpdateDD     = 0x00;
  NoOfRecs     = 0L;
  HeaderLen    = 0x00;
  RecordLen    = 0x00;
  NdxList      = NULL;
  FreeIxList   = NULL;
  XFV          = 3;            /* Xbase file version */

#ifdef XB_LOCKING_ON
  xblfh        = NULL;         /* lock file for XB_XBASE_LOCK_MODE */
  LockMode     = xbase->GetLockMode();
  TableLockCnt = 0;
  IndexLockCnt = 0;
#ifdef XB_MEMO_FIELDS
  MemoLockCnt  = 0;
#endif
  
  AutoLock         = 1;
  CurLockType      = -1;
  CurLockCount     = 0;
  CurLockedRecNo   = 0L;
  CurRecLockType   = -1;
  CurRecLockCount  = 0;
  CurMemoLockType  = -1;
  CurMemoLockCount = 0;
#else
  AutoLock         = 0;
#endif

#ifdef XB_MEMO_FIELDS
  MemofileName          = "";
  MemoHeader.BlockSize  = XB_DBT_BLOCK_SIZE;
  MemoHeader.Version    = 0x03;
  mfp                   = NULL;
  mbb                   = NULL;
  CurMemoBlockNo        = -1;
  mfield1               = 0;
  MStartPos             = 0;
  MFieldLen             = 0;
  NextFreeBlock         = 0L;
  FreeBlockCnt          = 0L;
  MNextBlockNo          = 0L;
  MNoOfFreeBlocks       = 0L;
#endif

//#ifdef XB_REAL_DELETE
  RealDelete = 0;
  FirstFreeRec = 0L;
  RealNumRecs = 0L;
//#endif
}
/************************************************************************/
//! Set dbase version for the dbf file.
/*!
  Set dbase version.  Should only be used before creating a database with
  xbDbf::CreateDatabase().
  
  \param v version, either 3 or 4.
*/
xbShort xbDbf::SetVersion(xbShort v) {
  if (v == 0)
    return XFV;
  else
    if(v == 3) {
      XFV = 3;
#ifdef XB_MEMO_FIELDS
      MemoHeader.Version = 0x03;
#endif
      return XFV;
   } else
     if (v == 4) {
       XFV = 4;
#ifdef XB_MEMO_FIELDS
       MemoHeader.Version = 0x00;
#endif
       return XFV;
   }

  return XB_INVALID_OPTION;
}
/************************************************************************/
//! Write the dbf header
/*!
  Internal use only.

  \param PositionOption flag that indicates whether file postition should
  be moved.  non-zero if so, zero if not.
*/
xbShort xbDbf::WriteHeader( xbShort PositionOption )
{
   char buf[32];
   memset(buf, 0, 32);
   if(PositionOption)
     rewind(fp);

   memcpy(&buf[0], &Version, 4);
   xbase->PutLong(&buf[4], NoOfRecs);
   xbase->PutShort(&buf[8], HeaderLen );
   xbase->PutShort(&buf[10], RecordLen );

#ifdef XB_REAL_DELETE
   if(RealDelete){
     xbase->PutULong(&buf[12], FirstFreeRec);
     xbase->PutULong(&buf[16], RealNumRecs);
   }
#endif
   if(fwrite(buf, 32, 1, fp) != 1)
     return XB_WRITE_ERROR;

   return XB_NO_ERROR;
}
/************************************************************************/
//! Read the dbf header.
/*!
  Internal use only.

  \param PositionOption
*/
xbShort xbDbf::ReadHeader( xbShort PositionOption )
{
#if 0
   char buf[4];
   if (PositionOption)
       rewind(fp);
   if (fread(&Version, 4, 1, fp) != 1)
     xb_error(XB_READ_ERROR);

   if (fread(buf, 4, 1, fp ) != 1)
     xb_error(XB_READ_ERROR);

   NoOfRecs = xbase->GetLong( buf );
   if(fread(buf, 2, 1, fp) != 1)
     xb_error(XB_READ_ERROR);

   HeaderLen = xbase->GetShort( buf );
   if(fread(buf, 2, 1, fp) != 1)
     xb_error(XB_READ_ERROR);

   RecordLen = xbase->GetShort(buf);

#ifdef XB_REAL_DELETE
   if(RealDelete)
   {
     if (fread(buf, 4, 1, fp ) != 1)
       xb_error(XB_READ_ERROR);
     FirstFreeRec = xbase->GetULong( buf );
     
     if (fread(buf, 4, 1, fp ) != 1)
       xb_error(XB_READ_ERROR);
     RealNumRecs = xbase->GetULong( buf );
   }
#endif
#else
   char buf[32];

   if(PositionOption)
     rewind(fp);

   if(fread(buf, 32, 1, fp) != 1)
     return XB_READ_ERROR;

   memcpy(&Version, buf, 4);
   NoOfRecs = xbase->GetLong(&buf[4]);
   HeaderLen = xbase->GetShort(&buf[8]);
   RecordLen = xbase->GetShort(&buf[10]);

#ifdef XB_REAL_DELETE
   if(RealDelete)
   {
     FirstFreeRec = xbase->GetULong(&buf[12]);
     RealNumRecs = xbase->GetULong(&buf[16]);
   }
#endif
#endif

   return XB_NO_ERROR;
}
/************************************************************************/
//! Determine if file name suffix is missing
/*!
  Internal use only.
*/
xbShort xbDbf::NameSuffixMissing( xbShort type, const char * name )
{
  /*  type 1 is DBF check
      type 2 is NDX check
      type 3 is MDX check
      type 4 is NTX check

      Returns 0 if suffix found
              1 if suffix not found, lower case
              2 is suffix not found, upper, case
*/

  xbShort len;

  len = strlen( name );
  if( len <= 4 )
    if( name[len-1] >= 'A' && name[len-1] <= 'Z' )
      return 2;
    else
      return 1;

  if( type == 1          && name[len-4] == '.' &&
     ( name[len-3] == 'd' || name[len-3] == 'D' ) && 
     ( name[len-2] == 'b' || name[len-2] == 'B' ) && 
     ( name[len-1] == 'f' || name[len-1] == 'F' )
    )
      return 0;

  if(  type == 2          && name[len-4] == '.' &&
     ( name[len-3] == 'n' || name[len-3] == 'N' ) && 
     ( name[len-2] == 'd' || name[len-2] == 'D' ) && 
     ( name[len-1] == 'x' || name[len-1] == 'X' )
     )
      return 0; 

  if(  type == 4          && name[len-4] == '.' &&
     ( name[len-3] == 'n' || name[len-3] == 'N' ) && 
     ( name[len-2] == 't' || name[len-2] == 'T' ) && 
     ( name[len-1] == 'x' || name[len-1] == 'X' )
    )
     return 0;

  if( name[len-5] >= 'A' && name[len-5] <= 'Z' )
    return 2;
  else
    return 1;
}
/************************************************************************/
//! Create the dbf file.
/*!
  This method attempts to create the DBF file with the specified
  name (TableName) and schema (xbSchema s).  The OverLay switch is used to determine
  if an existing file should be overwritten or an error flagged if the
  file already exists.  The  record buffer is blanked (set to spaces).
  
  \param TableName name of the table
  \param s xbSchema
  \param Overlay One of the following:
    \htmlonly
      <p>
      <table border=2><tr><th>OverLay</th><th>Description</th></tr>
        <tr><td>XB_OVERLAY</td><td>Overwrite existing file if it exists</td></tr>
        <tr><td>XB_DONTOVERLAY</td><td>Report an error if file exists</td></tr>
      </table>
    \endhtmlonly
    \latexonly
      \\
      \\
      \begin{tabular}{|l|l|} \hline
        \textbf{OverLay} & \textbf{Description} \\ \hline \hline
        XB\_OVERLAY      & Overwrite existing file if it exists \\ \hline
        XB\_DONTOVERLAY  & Report an error if file exists \\ \hline
      \end{tabular}
    \endlatexonly
  \returns One of the following return codes:
    \htmlonly
      <p>
      <table border=2><tr><th>Return Code</th><th>Description</th></tr>
        <tr><td>XB_NO_ERROR</td><td>No error</td></tr>
        <tr><td>XB_FILE_EXISTS</td><td>If the file exists and OverLay is XB_DONTOVERLAY</td></tr>
        <tr><td>XB_OPEN_ERROR</td><td>Couldn't open the file</td></tr>            <tr><td>XB_NO_MEMORY</td><td>Memory allocation error</td></tr>
        <tr><td>XB_WRITE_ERROR</td><td>Couldn't write to disk</td><tr>
      </table>
    \endhtmlonly
    \latexonly
      \\
      \\
      \begin{tabular}{|l|l|} \hline
        \textbf{Return Code} & \textbf{Description} \\ \hline \hline
        XB\_NO\_ERROR & No Error \\ \hline
        XB\_FILE\_EXISTS & If the file exists and OverLay is XB\_DONTOVERAY \\ \hline
        XB\_OPEN\_ERROR & Couldn't open the file \\ \hline
        XB\_WRITE\_ERROR & Couldn't write to disk \\ \hline
      \end{tabular}
    \endlatexonly
*/
xbShort xbDbf::CreateDatabase( const char * TableName, xbSchema * s,
    const xbShort Overlay )
{
  xbShort    i, j, k, k2, rc;  /* , count; */

#ifdef XB_MEMO_FIELDS
  xbShort MemoSw = 0;
#endif

  DbfStatus = XB_CLOSED;                 
  SetFileName( TableName );

  /* check if the file already exists */
  if((( fp = fopen( GetFileName(), "r" )) != NULL ) && !Overlay ){
    fclose( fp );
    return XB_FILE_EXISTS;
  }
  else if( fp ) fclose( fp );

  if(( fp = fopen( GetFileName(), "w+b" )) == NULL )
    return XB_OPEN_ERROR;

#ifdef XB_LOCKING_ON
  /* no buffering in multi user mode */
  setbuf( fp, NULL );
    
  /* open the lock file if XB_XBASE_LOCK_FLAVOR */
//  if( LockMode == XB_XBASE_LOCK_MODE )
//    if(( rc = OpenXbLockFile()) != XB_NO_ERROR )
//      return rc;
#endif

  /* count the number of fields and check paramaters */
  i = 0;
  while( s[i].Type != 0 ){
    NoOfFields++;

#ifdef XB_MEMO_FIELDS
    if(s[i].Type == 'M'){
      s[i].FieldLen = 10;
      s[i].NoOfDecs = 0;
    }
#endif /* XB_MEMO_FIELDS */

    if(s[i].Type == 'D'){
      s[i].FieldLen = 8;
      s[i].NoOfDecs = 0;
    }

    if(s[i].Type == 'C')
      s[i].NoOfDecs = 0;

    RecordLen += s[i].FieldLen;

    if( s[i].Type != 'C' &&
        s[i].Type != 'N' &&
        s[i].Type != 'F' &&
        s[i].Type != 'D' &&
#ifdef XB_MEMO_FIELDS
        s[i].Type != 'M' &&
#endif /* XB_MEMO_FIELDS */
        s[i].Type != 'L' )
    {
      fclose( fp );
      InitVars();
      return XB_UNKNOWN_FIELD_TYPE;
    }

#ifdef XB_MEMO_FIELDS
//  8/18/03 types B and O dont exist yet - gkunkel
//      if( !MemoSw && ( s[i].Type=='M' || s[i].Type=='B' || s[i].Type=='O'))
    if( !MemoSw && ( s[i].Type=='M' ))
       MemoSw++;
#endif

// check for numeric fields which are too long
    if((s[i].Type == 'N' || s[i].Type == 'F') && s[i].FieldLen > 19 ){
      fclose( fp );
      InitVars();
      return XB_INVALID_FIELD_LEN;
    }
    i++;
  }
  RecordLen++;                  /* add one byte for 0x0D    */

  if(( RecBuf = (char *) malloc( RecordLen )) == NULL ){
    fclose( fp );
    InitVars();
    return XB_NO_MEMORY;
  }

  if(( RecBuf2 = (char *) malloc( RecordLen )) == NULL ){
    free( RecBuf );
    fclose( fp );
    InitVars();
    return XB_NO_MEMORY;
  }

  /* BlankRecord(); */
  memset( RecBuf, 0x20, RecordLen );
  memset( RecBuf2, 0x20, RecordLen );

  /* set class variables */
  Version = XFV & 0x7;            // file version - bit 0-2
#ifdef XB_MEMO_FIELDS
  if(MemoSw){
    if((XFV & 0x7) == 3)
      Version |= 0x80;    // memo presence - bit 7
    else
      Version = (char) 0x8b;
  }
#endif

  CurRec  = 0L;
  HeaderLen = 33 + NoOfFields * 32;
  xbDate d;
  UpdateYY = (d.YearOf() - 1900);
  if((XFV & 0x7) == 3)
    UpdateYY %= 100;   // dBASE III seems to do this, but IV does not.  DTB

  UpdateMM = d.MonthOf();
  UpdateDD = d.DayOf( XB_FMT_MONTH );

  /* write the header prolog */
  if(( rc = WriteHeader( 0 )) != XB_NO_ERROR ){
    free( RecBuf );
    free( RecBuf2 );
    fclose( fp );
    InitVars();
    return XB_WRITE_ERROR;
  }

  if((SchemaPtr=(xbSchemaRec *)malloc(NoOfFields*sizeof(xbSchemaRec)))==NULL){
    free( RecBuf );
    free( RecBuf2 );
    fclose( fp );
    InitVars();
    return XB_NO_MEMORY;
  }
  memset( SchemaPtr, 0x00, ( NoOfFields * sizeof(xbSchemaRec)));

  /* write the field information into the header */
  for( i = 0, k = 1; i < NoOfFields; i++ ){
    memset( SchemaPtr[i].FieldName, 0x00, 11 );
    strncpy( SchemaPtr[i].FieldName, s[i].FieldName, 10 );

    SchemaPtr[i].Type = s[i].Type;
    SchemaPtr[i].FieldLen = s[i].FieldLen; 
    SchemaPtr[i].NoOfDecs = s[i].NoOfDecs;
    
    if( SchemaPtr[i].NoOfDecs > SchemaPtr[i].FieldLen ) {
      fclose( fp );
      free( SchemaPtr );
      free( RecBuf );
      free( RecBuf2 );
      InitVars(); 
      return XB_INVALID_SCHEMA;
    }

    k2 = k;
    k += SchemaPtr[i].FieldLen;

    if(( fwrite( &SchemaPtr[i], 1, 18, fp )) != 18 ) {
      fclose( fp );
      free( SchemaPtr );
      free( RecBuf );
      free( RecBuf2 );
      InitVars(); 
      return XB_WRITE_ERROR;
    }

    for( j = 0; j < 14; j++ ) {
      if(( fwrite( "\x00", 1, 1, fp )) != 1 ) {
        free( SchemaPtr );
        free( RecBuf );
        free( RecBuf2 );
        fclose( fp );
        InitVars(); 
        return XB_WRITE_ERROR;
      }
    }
    SchemaPtr[i].Address  = RecBuf  + k2;
    SchemaPtr[i].Address2 = RecBuf2 + k2;
  }

  /* write the header terminator */
  if(( fputc( XB_CHARHDR, fp )) != XB_CHARHDR ){
    fclose( fp );
    free( SchemaPtr );
    free( RecBuf );
    free( RecBuf2 );
    InitVars(); 
    return XB_WRITE_ERROR;
  }

#ifdef XB_MEMO_FIELDS
  if( MemoSw )
    if((rc = CreateMemoFile()) != XB_NO_ERROR){
      fclose(fp);
      free(RecBuf);
      free(RecBuf2);
      InitVars();
      return rc;
    }
#endif

  DbfStatus = XB_OPEN;
  return xbase->AddDbfToDbfList(this, GetFileName());
}
/************************************************************************/
//! Close the dbf file.
/*!
  This method attempts to close the DBF file which was previously
  opened with either CreateDatabase() or OpenDatabase().  Deletes any
  memory allocated.  Automatically closes any open indexes associated
  with this data file.
  
  \param deleteIndexes if TRUE, the indexes (xbIndex instances) will also 
    be deleted (index files will not be deleted)
  \returns One of the following:
    \htmlonly
      <p>
      <table border=2><tr><th>Return Code</th><th>Description</th></tr>
        <tr><td>XB_NO_ERROR</td><td>No error</td></tr>
        <tr><td>XB_NOT_OPEN</td><td>File was not open</td></tr>
      </table>
    \endhtmlonly
    \latexonly
      \\
      \\
      \begin{tabular}{|l|l|} \hline
        \textbf{Return Code} & \textbf{Description} \\ \hline \hline
        XB\_NO\_ERROR & No Error \\ \hline
        XB\_NOT\_OPEN\_ERROR & File was not open \\ \hline
      \end{tabular}
    \endlatexonly
*/
xbShort xbDbf::CloseDatabase( xbBool deleteIndexes )
{
#if defined(XB_INDEX_ANY)
  xbIxList *i, *ti;
#endif

  if(DbfStatus == XB_CLOSED)
    return XB_NO_ERROR;


#if defined(XB_INDEX_ANY)
  i = NdxList;
  while (i){
    i->index->CloseIndex();
    if(deleteIndexes)
      delete i->index;
    i = NdxList;
  }
/* free up unused nodes */
  i = FreeIxList;
  while( i ) {
    ti = i;
    i = i->NextIx;
    free(ti);
  }
#endif

  if(SchemaPtr){
    for( int j = 0; j < NoOfFields; j++ )
      if( SchemaPtr[j].fp ) delete SchemaPtr[j].fp;
    free( SchemaPtr );
  }
  if(RecBuf)
    free( RecBuf );
  if(RecBuf2)
    free( RecBuf2 );

#ifdef XB_MEMO_FIELDS
  if( mbb )
    free( mbb );         /* memo block buffer */
  if( mfp )
    fclose( mfp );       /* memo file pointer */
#endif

#ifdef XB_LOCKING_ON
  if( xblfh ){
    fclose( xblfh );
    xblfh = NULL;
  }
#endif

  xbase->RemoveDbfFromDbfList( this );
  if(fp)
    fclose( fp );
  InitVars();
  return XB_NO_ERROR;
}
/************************************************************************/
/* options  1 = Print header only
            2 = Field data only
            3 = Header and Field data */

//! Dump header information.
/*!
  \param Option One of the following:
     \htmlonly
       <p>
       <table border=2><tr><th>Option</th><th>Description</th></tr>
         <tr><td>1</td><td>Print header only</td></tr>
         <tr><td>2</td><td>Field data only</td></tr>
         <tr><td>3</td><td>Header and field data</td></tr>
       </table>
     \endhtmlonly
    \latexonly
      \\
      \\
      \begin{tabular}{|l|l|} \hline
        \textbf{Option} & \textbf{Description} \\ \hline \hline
        1 & Header only \\ \hline
        2 & Field data only \\ \hline
        3 & Header and field data \\ \hline
      \end{tabular}
    \endlatexonly
*/
#ifdef XBASE_DEBUG
xbShort xbDbf::DumpHeader( xbShort Option )
{
  int i;

  if( Option < 1 || Option > 3 )
    return XB_INVALID_OPTION;

  if( DbfStatus == XB_CLOSED )
    return XB_NOT_OPEN;

  std::cout << "\nDatabase file " << GetFileName() << std::endl << std::endl;

  if( Option != 2 ){
    std::cout << "File header data:" << std::endl;
    if( Version == 3 )
      std::cout << "Dbase III file" << std::endl;
    else if ( Version == 83 )
      std::cout << "Dbase III file with memo fields" << std::endl << std::endl;

    std::cout << "Last update date = " 
      << (int) UpdateMM << "/" << (int) UpdateDD << "/" << (int) UpdateYY % 100 << std::endl;  

    std::cout << "Header length    = " << HeaderLen << std::endl;
    std::cout << "Record length    = " << RecordLen << std::endl;
    std::cout << "Records in file  = " << NoOfRecs << std::endl << std::endl;
#ifdef XB_REAL_DELETE
    std::cout << "First Free Rec   = " << FirstFreeRec << std::endl << std::endl;
#endif      
  }
  if( Option != 1 ){
    std::cout << "Field Name   Type  Length  Decimals" << std::endl;
    std::cout << "----------   ----  ------  --------" << std::endl;
    for( i = 0; i <NoOfFields; i++ ){
      if( SchemaPtr[i].Type == 'C' && SchemaPtr[i].NoOfDecs > 0 )
        printf( "%10s    %1c     %4d    %4d\n", SchemaPtr[i].FieldName,
               SchemaPtr[i].Type, SchemaPtr[i].FieldLen, 0 );
      else    
        printf( "%10s    %1c     %4d    %4d\n", SchemaPtr[i].FieldName,
               SchemaPtr[i].Type, SchemaPtr[i].FieldLen, SchemaPtr[i].NoOfDecs );
    }
  }
  std::cout << std::endl;
  return XB_NO_ERROR;
}
#endif
/************************************************************************/
//! Open the DBF file.
/*!
  This method attempts to open the DBF file with the specified
  name (TableName).  This method does not position to any particular
  record in the file.  The record buffer is blanked (set to spaces).

  \param TableName Name of table to open
  \returns One of the following:  
    \htmlonly
      <p>
      <table border=2><tr><th>Return Code</th><th>Description</th></tr>
        <tr><td>XB_NO_ERROR</td><td>No error</td></tr>
        <tr><td>XB_OPEN_ERROR</td><td>Couldn't open file</td></tr>
        <tr><td>XB_NO_MEMORY</td><td>Memory allocation error</td></tr>
        <tr><td>XB_NOT_XBASE</td><td>Not an DBF file</td></tr>
      </table>
    \endhtmlonly
    \latexonly
      \\
      \\
      \begin{tabular}{|l|l|} \hline
        \textbf{Return Code} & \textbf{Description} \\ \hline \hline
        XB\_NO\_ERROR & No error \\ \hline
        XB\_OPEN\_ERROR & Couldn't open file \\ \hline
        XB\_NO\_MEMORY & Memory allocation error \\ \hline
        XB\_NOT\_XBASE & Not an DBF file \\ \hline
      \end{tabular}
    \endlatexonly
*/
xbShort xbDbf::OpenDatabase( const char * TableName )
{
  xbShort i, j, rc;
  char buf[33];
  char *p;

#ifdef XB_MEMO_FIELDS
  xbShort MemoSw = 0;
#endif

  /* verify the file is not already open */
  if( DbfStatus != XB_CLOSED )
    return XB_ALREADY_OPEN;

  /* copy the file name to the class variable */
  SetFileName( TableName );

  /* open the file */
  if(( fp = fopen(GetFileName(), "r+b")) == NULL ){
    //
    //  Try to open read only if failed to open read/write
    //
    if(( fp = fopen(GetFileName(), "rb")) == NULL )
      return XB_OPEN_ERROR;
  }

#ifdef XB_LOCKING_ON
  /* no buffering in multi user mode - may not see what others have updated */
  setbuf( fp, NULL );
  
  /* open the lock file if XB_XBASE_LOCK_MODE */
//  if( LockMode == XB_XBASE_LOCK_MODE )
//    if(( rc = OpenXbLockFile()) != XB_NO_ERROR )
//      return rc;
#endif

#ifdef XB_LOCKING_ON
//  if( AutoLock )
//    if(( rc = LockDatabase( XB_LOCK, 0L )) != XB_NO_ERROR)
//      return rc;
#endif

  /* copy the header into memory */
  if(( rc = ReadHeader( 1 )) != XB_NO_ERROR ){
    InitVars();
    return rc;
  }

  /* check the version */
  if( Version == 3 || Version == (char)0x83 ){ /* dBASE III+ */
    XFV = 3;
#ifdef XB_MEMO_FIELDS
    MemoHeader.Version = 0x03;
#endif
  }
  else if( Version == 4 || Version == (char)0x8B ){     /* dBASE IV */
    XFV = 4;
#ifdef XB_MEMO_FIELDS
    MemoHeader.Version = 0x00;
#endif
  }
  else if( Version == (char)0xf5 ){    /* FoxPro */
    XFV = 4;
#ifdef XB_MEMO_FIELDS
    MemoHeader.Version = 0x00;
#endif
  }
  else if( Version == (char)0x30 ){  /* Visual Foxpro */
    XFV = 4;
#ifdef XB_MEMO_FIELDS
    MemoHeader.Version = 0x00;
#endif
  } else {
    InitVars();
    return XB_NOT_XBASE;
  }

  // it would seem that dBASE III+ generates an UpdateYY value
  // of 0 for 2000 and dBASE IV uses 100, so I have removed the
  // check for UpdateYY being 0 (which might be valid).  DTB

  // Not all flavors of database tools use these fields
  // Found a month set to 0 in  valid dbf file 
  // Commented out this check 2/11/06 - GAK

  //  if( UpdateMM == 0 || UpdateDD == 0 ){
  //    InitVars();
  //    return XB_NOT_XBASE;
  //  }

  /* calculate the number of fields */
  if( Version == (char)0x30 ) {
    NoOfFields = ( HeaderLen - 296 ) / 32 ;
  } else {
    NoOfFields = ( HeaderLen - 33 ) / 32;
  }

  if(( RecBuf = (char *) malloc( RecordLen )) == NULL ) {
    fclose( fp );
    InitVars();
    return XB_NO_MEMORY;
  }
  if(( RecBuf2 = (char *) malloc( RecordLen )) == NULL ) {
    fclose( fp );
    free( RecBuf );
    InitVars();
    return XB_NO_MEMORY;
  }

  if((SchemaPtr=(xbSchemaRec *)malloc(NoOfFields*sizeof(xbSchemaRec)))==NULL){
    free( RecBuf );
    free( RecBuf2 );
    fclose( fp );
    InitVars();
    return XB_NO_MEMORY;
  }
  memset( SchemaPtr, 0x00, ( NoOfFields * sizeof(xbSchemaRec)));

  /* copy field info into memory */
  for( i = 0, j = 1; i < NoOfFields; i++ ){
    _fseek( fp,((xbOffT)i*32+32), 0 );
    fread( &buf, 1, 32, fp );
    p = buf;
    strncpy( SchemaPtr[i].FieldName, p, 10 );
    p += 11;
    SchemaPtr[i].Type = *p++;

    SchemaPtr[i].Address  = RecBuf + j;
    SchemaPtr[i].Address2 = RecBuf2 + j;

    SchemaPtr[i].FieldLen = *( p + 4 );
    SchemaPtr[i].NoOfDecs = *( p + 5 );

    if( SchemaPtr[i].Type == 'C' && SchemaPtr[i].NoOfDecs > 0 ){
      SchemaPtr[i].LongFieldLen = xbase->GetShort( p + 4 );
      j += SchemaPtr[i].LongFieldLen;
    }
    else
      j += SchemaPtr[i].FieldLen;
#ifdef XB_MEMO_FIELDS
      if( !MemoSw && (SchemaPtr[i].Type == 'M' || 
        SchemaPtr[i].Type == 'B' || SchemaPtr[i].Type == 'O' ))
        MemoSw++;
#endif
  } 
  CurRec = 0L;
  BlankRecord();
  DbfStatus = XB_OPEN;

#ifdef XB_MEMO_FIELDS
  if( MemoSw )   /* does this table have memo fields ? */
    if(( rc = OpenMemoFile()) != XB_NO_ERROR ){
      free( RecBuf );
      free( RecBuf2 );
      free( SchemaPtr );
      fclose( fp );
      InitVars(); 
      return rc;
    }
#endif

#ifdef XB_LOCKING_ON
//  if( AutoLock )
//    LockDatabase( XB_UNLOCK, 0L );
#endif               /* XB_LOCKING_ON */

  return xbase->AddDbfToDbfList( this, GetFileName() );
}
/************************************************************************/
//! Blank the record buffer.
/*!
  Sets the record to spaces.
*/
xbShort xbDbf::BlankRecord()
{
  if( DbfStatus == XB_CLOSED ) 
    return XB_NOT_OPEN;

  if( DbfStatus != XB_UPDATED ){
    DbfStatus = XB_UPDATED;
    memcpy( RecBuf2, RecBuf, RecordLen );
  }
  
  memset( RecBuf, 0x20, RecordLen );
  return XB_NO_ERROR;
}
/************************************************************************/
//! Append the current record to the data file
/*!
  This method attempts to append the contents of the current record buffer
  to the end of the DBF file and updates the file date and number of
  records in the file.  Also updates any open indices associated with
  this data file.
  
  \returns One of the following:
    \htmlonly
      <p>
      <table border=2><tr><th>Return Code</th><th>Description</th></tr>
        <tr><td>XB_NO_ERROR</td><td>No error</td></tr>
        <tr><td>XB_LOCK_FAILED</td><td>Couldn't lock file</td></tr>
        <tr><td>XB_WRITE_ERROR</td><td>Error writing to file</td></tr>
      </table>
    \endhtmlonly
    \latexonly
      \\
      \\
      \begin{tabular}{|l|l|} \hline
        \textbf{Return Code} & \textbf{Description} \\ \hline \hline
        XB\_NO\_ERROR & No error \\ \hline
   XB\_LOCK\_FAILED & Couldn't lock file \\ \hline
   XB\_WRITE\_ERROR & Error writing to file \\ \hline
      \end{tabular}
    \endlatexonly
*/
xbShort xbDbf::AppendRecord()
{
  xbShort rc;      
  xbULong nextRecNo;

#if defined(XB_INDEX_ANY)
  xbIxList *i;
#endif

/* lock the database */
#ifdef XB_LOCKING_ON
//  if( AutoLock )
//    if(( rc = LockDatabase( XB_LOCK, 0L )) != XB_NO_ERROR) 
//      return rc;

  rc = ReadHeader(1);

//  if(AutoLock)
//    LockDatabase( XB_UNLOCK, 0L );

  if( rc )
    return rc;

#endif

/* lock any indexes */
#if defined(XB_INDEX_ANY)
#ifdef XB_LOCKING_ON
  i = NdxList;
  while( i && AutoLock ){
//    if(( rc = i->index->LockIndex( XB_LOCK )) != XB_NO_ERROR ) 
//      return rc;
    i = i->NextIx;
  }
#endif               /* XB_LOCKING_ON */
#endif

// if there are no duplicates, and no records set the CurRec to the
// last record + 1.  This is for EXP::RECNO()

/* check for any duplicate keys */
#if defined(XB_INDEX_ANY)
  i = NdxList;
  while( i ){
    if( i->index->UniqueIndex() ){
      i->index->CreateKey( 0, 0 );
      if( i->index->FindKey() == XB_FOUND )
      return XB_KEY_NOT_UNIQUE;
    }
    i = i->NextIx;
  }
#endif

#ifdef XB_REAL_DELETE
  if(RealDelete && FirstFreeRec)
    nextRecNo = FirstFreeRec;
  else
    nextRecNo = NoOfRecs + 1;
#else
  nextRecNo = NoOfRecs + 1;
#endif

  CurRec = NoOfRecs + 1;

#if defined(XB_INDEX_ANY)
/* update the indexes */
  i = NdxList;
  while( i ){
    if( !i->index->UniqueIndex() )          /* if we didn't prepare the key */
      if(( rc = i->index->CreateKey( 0, 0 )) != XB_NO_ERROR ) /* then do it before the add    */
        return rc;
    if(( rc =  i->index->AddKey(nextRecNo)) != XB_NO_ERROR )
      return rc;
    i->index->TouchIndex();
    i = i->NextIx;
  }
#endif              /* XB_INDEX_ANY */

#ifdef XB_REAL_DELETE
  char buf[4];

  if(RealDelete && FirstFreeRec){
    /*
    **  Grab the next free rec no and put it in FirstFreeRec
    */
    if(_fseek(fp, (HeaderLen+(((xbOffT)FirstFreeRec-1)*RecordLen)+1), 0) != 0)
      return XB_SEEK_ERROR;
       
    if(fread(buf, 4, 1, fp) != 1)
      return XB_READ_ERROR;
       
    FirstFreeRec = xbase->GetULong(buf);
  }
   
  /*
  **  Okay, seek and write the record out
  */
  if(_fseek(fp, (HeaderLen+(((xbOffT)nextRecNo-1)*RecordLen)), 0) != 0)
    return XB_SEEK_ERROR;
     
  if(fwrite( RecBuf, RecordLen, 1, fp) != 1)
    return XB_WRITE_ERROR;

  /*
  **  If we just appended the record to the file, then write the EOF char
  */
  if(nextRecNo == NoOfRecs + 1){
    if( fputc( XB_CHAREOF, fp ) != XB_CHAREOF )
      return XB_WRITE_ERROR;
  }
#else
  /* write the last record */
  if( _fseek( fp,(HeaderLen+((xbOffT)NoOfRecs*RecordLen)), 0 ) != 0 )
    return XB_SEEK_ERROR;

  if( fwrite( RecBuf, RecordLen, 1, fp ) != 1 )
    return XB_WRITE_ERROR;

   /* write the end of file marker */
  if( fputc( XB_CHAREOF, fp ) != XB_CHAREOF )
    return XB_WRITE_ERROR;
#endif

  /* calculate the latest header information */
  xbDate d; 
  UpdateYY = d.YearOf() - 1900;
  if(XFV == 3)
    UpdateYY %= 100;  // dBASE III seems to do this, IV does not.  DTB
  UpdateMM = d.MonthOf();
  UpdateDD = d.DayOf( XB_FMT_MONTH );
#ifndef XB_REAL_DELETE
  NoOfRecs++;
#else
  if(RealDelete){
    if(nextRecNo == NoOfRecs + 1)
      NoOfRecs++;
    RealNumRecs++;
  }
  else
    NoOfRecs++;
#endif
  CurRec = nextRecNo;

  /* rewrite the header record */
  if(( rc = WriteHeader( 1 )) != XB_NO_ERROR )
     return rc;

#ifdef XB_LOCKING_ON
//  if( AutoLock )
//    LockDatabase( XB_UNLOCK, 0L );

#if defined(XB_INDEX_ANY)
  i = NdxList;
  while( i && AutoLock ){
//    i->index->LockIndex( XB_UNLOCK );
    i = i->NextIx;
  }
#endif               /* XB_INDEX_ANY  */
#endif               /* XB_LOCKING_ON */

  DbfStatus = XB_OPEN;
  return XB_NO_ERROR;
}
/************************************************************************/
//! Get a record from the data file
/*!
  This method attempts to retrieve the record specified by RecNo from the
  data file into the record buffer.  
  
  \param RecNo Record number to retrieve
  \returns One of the following:
    \htmlonly
      <p>
      <table border=2><tr><th>Return Code</th><th>Description</th></tr>
        <tr><td>XB_NO_ERROR</td><td>No error</td></tr>
        <tr><td>XB_LOCK_FAILED</td><td>Couldn't lock file</td></tr>
        <tr><td>XB_NOT_OPEN</td><td>File is not open</td></tr>
        <tr><td>XB_INVALID_RECORD</td><td>Invalid record number</td></tr>
        <tr><td>XB_WRITE_ERROR</td><td>Error writing to file</td></tr>
      </table>
    \endhtmlonly
    \latexonly
      \\
      \\
      \begin{tabular}{|l|l|} \hline
        \textbf{Return Code} & \textbf{Description} \\ \hline \hline
        XB\_NO\_ERROR & No error \\ \hline
   XB\_LOCK\_FAILED & Couldn't lock file \\ \hline
   XB\_NOT\_OPEN & File is not open \\ \hline
   XB\_INVALID\_RECORD & Invalid record number \\ \hline
   XB\_WRITE\_ERROR & Error writing to file \\ \hline
      \end{tabular}
    \endlatexonly
*/
xbShort xbDbf::GetRecord( xbULong RecNo )
{
  xbShort rc;
  if( DbfStatus == XB_CLOSED )
    return XB_NOT_OPEN;

#ifdef XB_LOCKING_ON
//  if( AutoLock )
//    if(( rc = LockDatabase( XB_LOCK, RecNo )) != 0 ) return rc;

  rc = ReadHeader(1);

//  if(AutoLock)
//    LockDatabase( XB_UNLOCK, RecNo );

  if( rc )
    return rc;

#endif

  if( RecNo > NoOfRecs || RecNo == 0L )
    return XB_INVALID_RECORD;

  if( _fseek( fp, (HeaderLen+(((xbOffT)RecNo-1L)*RecordLen)), SEEK_SET )){
#ifdef XB_LOCKING_ON
//    LockDatabase( XB_UNLOCK, RecNo );
#endif
    return XB_SEEK_ERROR;
  }

  if( fread( RecBuf, RecordLen, 1, fp ) != 1 ){
#ifdef XB_LOCKING_ON
//    LockDatabase( XB_UNLOCK, RecNo );
#endif
    return XB_READ_ERROR;
  }

#ifdef XB_LOCKING_ON
//  if( AutoLock )
//    LockDatabase( XB_LOCK, RecNo );
#endif

  DbfStatus = XB_OPEN;
  CurRec = RecNo;
  return XB_NO_ERROR;
}
/************************************************************************/
//! Get the first physical record in the data file
/*!
  Attempts to retrieve the first physical record from the data file into
  the record buffer.
  
  \returns One of the following:
    \htmlonly
      <p>
      <table border=2><tr><th>Return Code</th><th>Description</th></tr>
        <tr><td>XB_NO_ERROR</td><td>No error</td></tr>
        <tr><td>XB_LOCK_FAILED</td><td>Couldn't lock file</td></tr>
        <tr><td>XB_NOT_OPEN</td><td>File is not open</td></tr>
        <tr><td>XB_INVALID_RECORD</td><td>Invalid record number</td></tr>
        <tr><td>XB_SEEK_ERROR</td><td>Error seeking file</td></tr>
        <tr><td>XB_WRITE_ERROR</td><td>Error writing to file</td></tr>
      </table>
    \endhtmlonly
    \latexonly
      \\
      \\
      \begin{tabular}{|l|l|} \hline
        \textbf{Return Code} & \textbf{Description} \\ \hline \hline
        XB\_NO\_ERROR & No error \\ \hline
   XB\_LOCK\_FAILED & Couldn't lock file \\ \hline
   XB\_NOT\_OPEN & File is not open \\ \hline
   XB\_INVALID\_RECORD & Invalid record number \\ \hline
   XB\_SEEK\_ERROR & Error seeking file \\ \hline
   XB\_WRITE\_ERROR & Error writing to file \\ \hline
      \end{tabular}
    \endlatexonly
*/
xbShort xbDbf::GetFirstRecord()
{
  xbShort rc;
  if( NoOfRecs == 0 )
    return XB_INVALID_RECORD;

  rc = GetRecord( 1L );
#ifdef XB_REAL_DELETE
  if(!rc && RealDelete && RecordDeleted())
    rc = GetNextRecord();
#endif

  return rc;
}
/************************************************************************/
//! Get the last phyiscal record in the data file
/*!
  Attempts to retrieve the last physical record from the data file into
  the record buffer.
  
  \returns One of the following:
    \htmlonly
      <p>
      <table border=2><tr><th>Return Code</th><th>Description</th></tr>
        <tr><td>XB_NO_ERROR</td><td>No error</td></tr>
        <tr><td>XB_LOCK_FAILED</td><td>Couldn't lock file</td></tr>
        <tr><td>XB_EOF</td><td>At end of file</td></tr>
        <tr><td>XB_NOT_OPEN</td><td>File is not open</td></tr>
        <tr><td>XB_INVALID_RECORD</td><td>Invalid record number</td></tr>
        <tr><td>XB_SEEK_ERROR</td><td>Error seeking file</td></tr>
        <tr><td>XB_WRITE_ERROR</td><td>Error writing to file</td></tr>
      </table>
    \endhtmlonly
    \latexonly
      \\
      \\
      \begin{tabular}{|l|l|} \hline
        \textbf{Return Code} & \textbf{Description} \\ \hline \hline
        XB\_NO\_ERROR & No error \\ \hline
   XB\_LOCK\_FAILED & Couldn't lock file \\ \hline
   XB\_EOF & At end of file \\ \hline
   XB\_NOT\_OPEN & File is not open \\ \hline
   XB\_INVALID\_RECORD & Invalid record number \\ \hline
   XB\_SEEK\_ERROR & Error seeking file \\ \hline
   XB\_WRITE\_ERROR & Error writing to file \\ \hline
      \end{tabular}
    \endlatexonly
*/
xbShort xbDbf::GetLastRecord()
{
  xbShort rc;
  if( NoOfRecs == 0 )
    return XB_INVALID_RECORD;

  rc = GetRecord( NoOfRecs );
#ifdef XB_REAL_DELETE
  if(!rc && RealDelete && RecordDeleted())
    rc = GetPrevRecord();
#endif

  return rc;
}
/************************************************************************/
//! Get the next physical record in the data file
/*!
  Attempts to retrieve the next physical record from the data file into
  the record buffer.

  \returns One of the following:
    \htmlonly
      <p>
      <table border=2><tr><th>Return Code</th><th>Description</th></tr>
        <tr><td>XB_NO_ERROR</td><td>No error</td></tr>
        <tr><td>XB_LOCK_FAILED</td><td>Couldn't lock file</td></tr>
        <tr><td>XB_EOF</td><td>At end of file</td></tr>
        <tr><td>XB_NOT_OPEN</td><td>File is not open</td></tr>
        <tr><td>XB_INVALID_RECORD</td><td>Invalid record number</td></tr>
        <tr><td>XB_SEEK_ERROR</td><td>Error seeking file</td></tr>
        <tr><td>XB_WRITE_ERROR</td><td>Error writing to file</td></tr>
      </table>
    \endhtmlonly
    \latexonly
      \\
      \\
      \begin{tabular}{|l|l|} \hline
        \textbf{Return Code} & \textbf{Description} \\ \hline \hline
        XB\_NO\_ERROR & No error \\ \hline
   XB\_LOCK\_FAILED & Couldn't lock file \\ \hline
   XB\_EOF & At end of file \\ \hline
   XB\_NOT\_OPEN & File is not open \\ \hline
   XB\_INVALID\_RECORD & Invalid record number \\ \hline
   XB\_SEEK\_ERROR & Error seeking file \\ \hline
   XB\_WRITE\_ERROR & Error writing to file \\ \hline
      \end{tabular}
    \endlatexonly
*/
xbShort xbDbf::GetNextRecord()
{
  xbShort rc;
  if( NoOfRecs == 0 ) 
    return XB_INVALID_RECORD;
  else if( CurRec >= NoOfRecs ) 
    return XB_EOF;
  
  rc = GetRecord( ++CurRec );

#ifdef XB_REAL_DELETE
  while(!rc && RealDelete && RecordDeleted())
    rc = GetRecord(++CurRec);
#endif
  
  return rc;
}
/************************************************************************/
//! Get the previous physical record in the data file
/*!
  Attempts to retrieve the previous physical record from the data file into
  the record buffer.
  
  \returns One of the following:
    \htmlonly
      <p>
      <table border=2><tr><th>Return Code</th><th>Description</th></tr>
        <tr><td>XB_NO_ERROR</td><td>No error</td></tr>
        <tr><td>XB_LOCK_FAILED</td><td>Couldn't lock file</td></tr>
        <tr><td>XB_BOF</td><td>At beginning of file</td></tr>
        <tr><td>XB_NOT_OPEN</td><td>File is not open</td></tr>
        <tr><td>XB_INVALID_RECORD</td><td>Invalid record number</td></tr>
        <tr><td>XB_SEEK_ERROR</td><td>Error seeking file</td></tr>
        <tr><td>XB_WRITE_ERROR</td><td>Error writing to file</td></tr>
      </table>
    \endhtmlonly
    \latexonly
      \\
      \\
      \begin{tabular}{|l|l|} \hline
        \textbf{Return Code} & \textbf{Description} \\ \hline \hline
        XB\_NO\_ERROR & No error \\ \hline
   XB\_LOCK\_FAILED & Couldn't lock file \\ \hline
   XB\_BOF & At beginning of file \\ \hline
   XB\_NOT\_OPEN & File is not open \\ \hline
   XB\_INVALID\_RECORD & Invalid record number \\ \hline
   XB\_SEEK\_ERROR & Error seeking file \\ \hline
   XB\_WRITE\_ERROR & Error writing to file \\ \hline
      \end{tabular}
    \endlatexonly
*/
xbShort xbDbf::GetPrevRecord()
{
  xbShort rc;
  if( NoOfRecs == 0 ) 
    return XB_INVALID_RECORD;
  else if( CurRec <= 1L ) 
    return XB_EOF;
  
  rc = GetRecord( --CurRec );

#ifdef XB_REAL_DELETE
  while(!rc && RealDelete && RecordDeleted())
    rc = GetRecord(--CurRec);
#endif

  return rc;
}
/************************************************************************/
//! Dump record
/*!
  Dump the contents of the specified record to stdout.

  \param RecNo Record number of record to be dumped.
  \returns An error code (same as GetRecord()).
*/
xbShort xbDbf::DumpRecord( xbULong RecNo )
{
  int i, rc;
  char buf[4096];

  if( RecNo == 0 || RecNo > NoOfRecs )
    return XB_INVALID_RECORD;

  rc = GetRecord( RecNo );
  if( rc != XB_NO_ERROR )
    return rc;

  std::cout << "\nREC NUMBER " << RecNo << "\n";

  if( RecordDeleted() )
    std::cout << "\nRecord deleted...\n";

  for( i = 0; i < NoOfFields; i++ ){
#ifdef XB_MEMO_FIELDS
    if(SchemaPtr[i].Type == 'M'){
      if( MemoFieldExists( i )){
        std::cout << SchemaPtr[i].Type << " " << SchemaPtr[i].FieldName 
                  << " len = " << GetMemoFieldLen( i ) << std::endl;
        memset( buf, 0x00, 4095 );
        rc = GetMemoField(i, 4095, buf, 0);
        if(rc != XB_NO_ERROR)
          return rc;
      } else {
         buf[0] = 0x00;
      }    
    }
    else
      GetField( i, buf );
    std::cout << SchemaPtr[i].Type << " " << SchemaPtr[i].FieldName << " = '" << buf << "'\n";
#else
    GetField( i, buf );
    std::cout << SchemaPtr[i].FieldName << " = '" << buf << "'\n";
#endif
  }
  std::cout << std::endl;
  return XB_NO_ERROR;
}
/************************************************************************/
//! Write the current record buffer to the current record in the data file.
/*!
  Attempts to write the contents of the record buffer to the current
  record in the data file.  Updates any open indexes.

  \sa PutRecord(xbULong RecNo)
  \returns One of the following:
    \htmlonly
      <p>
      <table border=2><tr><th>Return Code</th><th>Description</th></tr>
        <tr><td>XB_NO_ERROR</td><td>No error</td></tr>
        <tr><td>XB_LOCK_FAILED</td><td>Couldn't lock file</td></tr>
        <tr><td>XB_NOT_OPEN</td><td>File is not open</td></tr>
        <tr><td>XB_INVALID_RECORD</td><td>Invalid record number</td></tr>
        <tr><td>XB_SEEK_ERROR</td><td>Error seeking file</td></tr>
        <tr><td>XB_WRITE_ERROR</td><td>Error writing to file</td></tr>
      </table>
    \endhtmlonly
    \latexonly
      \\
      \\
      \begin{tabular}{|l|l|} \hline
        \textbf{Return Code} & \textbf{Description} \\ \hline \hline
        XB\_NO\_ERROR & No error \\ \hline
   XB\_LOCK\_FAILED & Couldn't lock file \\ \hline
   XB\_NOT\_OPEN & File is not open \\ \hline
   XB\_INVALID\_RECORD & Invalid record number \\ \hline
   XB\_SEEK\_ERROR & Error seeking file \\ \hline
   XB\_WRITE\_ERROR & Error writing to file \\ \hline
      \end{tabular}
    \endlatexonly
*/

/************************************************************************/
xbShort xbDbf::PutRecord() {
   return PutRecord(CurRec);
}

//! Write the current record buffer to the specified record in the data file.
/*!
  Attempts to write the contents of the record buffer to the record specified
  by RecNo.  Updates any open indexes.
  
  \param RecNo Record number to which data should be written
  \returns One of the following:
    \htmlonly
      <p>
      <table border=2><tr><th>Return Code</th><th>Description</th></tr>
        <tr><td>XB_NO_ERROR</td><td>No error</td></tr>
        <tr><td>XB_LOCK_FAILED</td><td>Couldn't lock file</td></tr>
        <tr><td>XB_NOT_OPEN</td><td>File is not open</td></tr>
        <tr><td>XB_INVALID_RECORD</td><td>Invalid record number</td></tr>
        <tr><td>XB_SEEK_ERROR</td><td>Error seeking file</td></tr>
        <tr><td>XB_WRITE_ERROR</td><td>Error writing to file</td></tr>
      </table>
    \endhtmlonly
    \latexonly
      \\
      \\
      \begin{tabular}{|l|l|} \hline
        \textbf{Return Code} & \textbf{Description} \\ \hline \hline
        XB\_NO\_ERROR & No error \\ \hline
   XB\_LOCK\_FAILED & Couldn't lock file \\ \hline
   XB\_NOT\_OPEN & File is not open \\ \hline
   XB\_INVALID\_RECORD & Invalid record number \\ \hline
   XB\_SEEK\_ERROR & Error seeking file \\ \hline
   XB\_WRITE\_ERROR & Error writing to file \\ \hline
      \end{tabular}
    \endlatexonly
*/
xbShort xbDbf::PutRecord(xbULong RecNo) 
{
  xbShort  rc;

#if defined(XB_INDEX_ANY)
  xbIxList *i;
#endif

  if( DbfStatus == XB_CLOSED )
    return XB_NOT_OPEN;

/* lock the database */
#ifdef XB_LOCKING_ON
  if( AutoLock ){
//    if(( rc = LockDatabase( XB_LOCK, RecNo )) != XB_NO_ERROR ) 
//      return rc;
//    if(( rc = LockDatabase( XB_LOCK, 0L )) != XB_NO_ERROR ){
//      LockDatabase( XB_UNLOCK, RecNo );
//      return rc;
//    }
     
    if((rc = ReadHeader(1)) != XB_NO_ERROR){
//      if(AutoLock){
//        LockDatabase( XB_UNLOCK, RecNo );
//        LockDatabase( XB_UNLOCK, 0L );
//      }
      return rc;
    }
  }
#endif

  if( RecNo > NoOfRecs || RecNo == 0L )
    return XB_INVALID_RECORD;

/* lock the indexes */
#if defined(XB_INDEX_ANY)
#ifdef XB_LOCKING_ON
  i = NdxList;
  while( i && AutoLock ){
//    if(( rc = i->index->LockIndex( XB_LOCK )) != XB_NO_ERROR ) 
//      return rc;
    i = i->NextIx;
  }
#endif               /* XB_LOCKING_ON */
#endif

#if defined(XB_INDEX_ANY)
  /* for any unique indexes that were updated, verify no unique keys exist */
  i = NdxList;
  while( i ){
    if( i->index->UniqueIndex() ){
      if(( i->KeyUpdated = i->index->KeyWasChanged()) == 1 ){
        i->index->CreateKey(0, 0);
         if( i->index->FindKey() == XB_FOUND && i->index->GetCurDbfRec() != RecNo)
           return XB_KEY_NOT_UNIQUE;
      }
    }
    i = i->NextIx;
  }
#endif

#if defined(XB_INDEX_ANY)
   /* loop through deleting old index keys and adding new index keys */
  i = NdxList;
  while( i ){
    if( !i->index->UniqueIndex() )
      i->KeyUpdated = i->index->KeyWasChanged();
    if( i->KeyUpdated ){
      i->index->CreateKey( 1, 0 );      /* load key buf w/ old values */
      if((rc = i->index->DeleteKey( CurRec )) != XB_NO_ERROR){
#ifdef XB_LOCKING_ON
//        if( AutoLock ){
//          LockDatabase( XB_UNLOCK, RecNo );
//          LockDatabase( XB_UNLOCK, 0L );
//        }
#if defined(XB_INDEX_ANY)
        i = NdxList;
        while( i && AutoLock ){
//          i->index->LockIndex( XB_UNLOCK );
          i = i->NextIx;
        }
#endif               /* XB_INDEX_ANY  */
#endif               /* XB_LOCKING_ON */
      return rc;
    }

    i->index->CreateKey( 0, 0 );
    if(( rc = i->index->AddKey(CurRec)) != XB_NO_ERROR ){
#ifdef XB_LOCKING_ON
//      if( AutoLock ){
//        LockDatabase( XB_UNLOCK, RecNo );
//        LockDatabase( XB_UNLOCK, 0L );
//      }
#if defined(XB_INDEX_ANY)
      i = NdxList;
      while( i && AutoLock ){
//        i->index->LockIndex( XB_UNLOCK );
        i = i->NextIx;
      }
#endif               /* XB_INDEX_ANY  */
#endif               /* XB_LOCKING_ON */
      return rc;
    }
    i->index->TouchIndex();
   }
   i = i->NextIx;
 }
#endif                        /* XB_INDEX_ANY */

 if( _fseek( fp, (HeaderLen+(((xbOffT)RecNo-1L)*RecordLen)),0 ))
   return XB_SEEK_ERROR;

 if( fwrite( RecBuf, RecordLen, 1, fp ) != 1 )
   return XB_WRITE_ERROR;

 /* calculate the latest header information */
 xbDate d;
 UpdateYY = d.YearOf() - 1900;
 if(XFV == 3)
    UpdateYY %= 100;  // dBASE III seems to do this, IV does not.  DTB
 UpdateMM = d.MonthOf();
 UpdateDD = d.DayOf( XB_FMT_MONTH );

 /* rewrite the header record */
 if(( rc = WriteHeader( 1 )) != XB_NO_ERROR )
   return rc;

#ifdef XB_LOCKING_ON
//  if( AutoLock ){
//    LockDatabase( XB_UNLOCK, RecNo );
//    LockDatabase( XB_UNLOCK, 0L );
//  }

#if defined(XB_INDEX_ANY)
  i = NdxList;
  while( i && AutoLock ){
//    i->index->LockIndex( XB_UNLOCK );
    i = i->NextIx;
  }
#endif               /* XB_INDEX_ANY  */
#endif               /* XB_LOCKING_ON */

  CurRec = RecNo;
  DbfStatus = XB_OPEN;
  return XB_NO_ERROR;
}
/************************************************************************/
//! Delete the current record
/*!
  Marks the current record as deleted or if "real" deletes are turned
  on (xbDbf::RealDeleteOn()) will delete the record and add it to the
  free record list.  Normal dBase behavior is to simply mark the record
  as deleted; the record will actually be deleted when the the DBF file 
  "packed" (xbDbf::PackDatabase()).  If "real" deletes are not on, a 
  record may be undeleted using xbDbf::UndeleteRecord().
  
  \returns One of the following:
    \htmlonly
      <p>
      <table border=2><tr><th>Return Code</th><th>Description</th></tr>
        <tr><td>XB_NO_ERROR</td><td>No error</td></tr>
        <tr><td>XB_INVALID_RECORD</td><td>Invalid record number</td></tr>
      </table>
    \endhtmlonly
    \latexonly
      \\
      \\
      \begin{tabular}{|l|l|} \hline
        \textbf{Return Code} & \textbf{Description} \\ \hline \hline
        XB\_NO\_ERROR & No error \\ \hline
   XB\_INVALID\_RECORD & Invalid record number \\ \hline
      \end{tabular}
    \endlatexonly
*/
xbShort xbDbf::DeleteRecord()
{
  xbULong newCurRec = 0;
  xbShort rc = XB_NO_ERROR;

#if defined(XB_INDEX_ANY)
  xbIxList *i;
#endif

  if(!RecBuf)   
    return XB_INVALID_RECORD;
     
  if(CurRec < 1 || CurRec > NoOfRecs)
    return XB_INVALID_RECORD;
          
/* lock the database */
#ifdef XB_LOCKING_ON

  if( AutoLock ){
/*
    if(( rc = LockDatabase( XB_LOCK, CurRec )) != XB_NO_ERROR ) 
      return rc;
    if(( rc = LockDatabase( XB_LOCK, 0L )) != XB_NO_ERROR ){
      LockDatabase( XB_UNLOCK, CurRec );
      return rc;
    }
 */
    if((rc = ReadHeader(1)) != XB_NO_ERROR){
//      if(AutoLock){
//        LockDatabase( XB_UNLOCK, CurRec );
//        LockDatabase( XB_UNLOCK, 0L );
//      }
      return rc;
    }
  }
#endif

/* lock the indexes */
#if defined(XB_INDEX_ANY) && defined(XB_LOCKING_ON) && defined(XB_REAL_DELETE)
  i = NdxList;
  while( i && AutoLock ){
//    if(( rc = i->index->LockIndex( XB_LOCK )) != XB_NO_ERROR )
//       return rc;
    i = i->NextIx;
  }
#endif

/* remove keys from indexes */
#if defined(XB_REAL_DELETE) && defined(XB_INDEX_ANY)

  if(RealDelete){
    i = NdxList;
    while(i){
      i->index->CreateKey(0, 0);      /* load key buf */
      if(i->index->GetCurDbfRec() == (xbULong)CurRec){
        i->index->DeleteKey(CurRec);
        newCurRec = i->index->GetCurDbfRec();
      }
      else
        i->index->DeleteKey(CurRec);
      i->index->TouchIndex();
      i = i->NextIx;
    }
  }

#endif

  RecBuf[0] = 0x2a;


#ifdef XB_REAL_DELETE
  if(RealDelete){
#ifdef XB_MEMO_FIELDS
    //
    //  Delete memo data for memo fields.
    //
    for(int f = 0; f < NoOfFields; f++ )
      if(GetFieldType(f) == 'M' && MemoFieldExists(f))
        UpdateMemoData(f, 0, 0, XB_LOCK);
#endif
    xbase->PutULong(&RecBuf[1], FirstFreeRec);
    FirstFreeRec = CurRec;
    RealNumRecs--;
    WriteHeader(1);
 }
#endif

 if(!RealDelete){
   if( DbfStatus != XB_UPDATED ){
     DbfStatus = XB_UPDATED;
     memcpy( RecBuf2, RecBuf, RecordLen );
   }
   rc = PutRecord( CurRec );
 }
 else
 {
   if(_fseek( fp, (HeaderLen+(((xbOffT)CurRec-1L)*RecordLen)), 0))
     return XB_SEEK_ERROR;
   if(fwrite( RecBuf, RecordLen, 1, fp ) != 1 )
     return XB_WRITE_ERROR;

   //
   //  Attempt to read in the record for the current location
   //  in the active index.
   //
   CurRec = newCurRec;
   if(CurRec)
     rc = GetRecord(CurRec);
   else
     BlankRecord();
 }
      
#ifdef XB_LOCKING_ON
//  if(AutoLock){
//    LockDatabase( XB_UNLOCK, CurRec );
//    LockDatabase( XB_UNLOCK, 0L );
//  }

#if defined(XB_INDEX_ANY) && defined(XB_REAL_DELETE)
  i = NdxList;
  while( i && AutoLock ){
//    i->index->LockIndex( XB_UNLOCK );
    i = i->NextIx;
  }
#endif               /* XB_INDEX_ANY  */
#endif               /* XB_LOCKING_ON */

  return rc;
}
/************************************************************************/
//! Undelete the current record
/*!
  Marks the currect record as not deleted (i.e. removes the flag indicating
  the record is deleted).  This method may not be used (and will return
  an error code) if "real" deletes are on.
  
  \returns One of the following:
    \htmlonly
      <p>
      <table border=2><tr><th>Return Code</th><th>Description</th></tr>
        <tr><td>XB_NO_ERROR</td><td>No error</td></tr>
        <tr><td>XB_INVALID_RECORD</td><td>Invalid record number</td></tr>
      </table>
    \endhtmlonly
    \latexonly
      \\
      \\
      \begin{tabular}{|l|l|} \hline
        \textbf{Return Code} & \textbf{Description} \\ \hline \hline
        XB\_NO\_ERROR & No error \\ \hline
   XB\_INVALID\_RECORD & Invalid record number \\ \hline
      \end{tabular}
    \endlatexonly
*/
xbShort xbDbf::UndeleteRecord()
{
  xbShort rc;
   
#ifdef XB_REAL_DELETE
  if(RealDelete)
    return XB_INVALID_RECORD;
#endif
  if( RecBuf ){
    if( DbfStatus != XB_UPDATED ){
      DbfStatus = XB_UPDATED;
      memcpy( RecBuf2, RecBuf, RecordLen );
    }
  
    RecBuf[0] = 0x20;
    if(( rc = PutRecord( CurRec )) != 0 )
      return rc;
  }
  else
    return XB_INVALID_RECORD;

  return 0;
}
/************************************************************************/
//! Determine if current record is deleted
/*!
  \returns TRUE (1) if the current record is marked as deleted or FALSE
    (0) if not.
*/
xbShort xbDbf::RecordDeleted()
{
  if( RecBuf && RecBuf[0] == 0x2a )
    return 1;
  else
    return 0;
}
/************************************************************************/
//! Create a unique file name 
/*!
*/
xbShort xbDbf::CreateUniqueDbfName( xbString & sDbfn, xbString & sDbtn )
{
  xbShort dnf;    /* directory in name flag */
  xbShort unique = 0;
  xbLong l = 1;
  char dbfn[13];
  char dbtn[13];
  
  dnf = xbase->DirectoryExistsInName( GetFileName() );
  sprintf( dbfn, "xb%06d.dbf", l );
  sprintf( dbtn, "xb%06d.dbt", l++ );
  
  if( dnf ){
    sDbfn.assign( GetFileName(), 0, dnf );
    sDbfn += dbfn;
    sDbtn.assign( GetFileName(), 0, dnf );
    sDbtn += dbtn;
  } else  {
    sDbfn  = dbfn;
    sDbtn  = dbtn;
  }
    
  while( !unique ){
    if( access( sDbfn.getData(), 0 ) == -1 &&
        access( sDbtn.getData(), 0 ) == -1 )
      unique++;
    else{
      sprintf( dbfn, "xb%06d.dbf", l );
      sprintf( dbtn, "xb%06d.dbt", l++ );
    
      if( dnf ){
        sDbfn.assign( GetFileName(), 0, dnf );
        sDbfn += dbfn;
        sDbtn.assign( GetFileName(), 0, dnf );
        sDbtn += dbtn;
      } else  {
        sDbfn  = dbfn;
        sDbtn  = dbtn;
      }
    }
  }
  return 0;
}

/************************************************************************/
//! Pack data file
/*!
*/
xbShort xbDbf::PackDatafiles(void (*statusFunc)(xbLong itemNum, xbLong numItems))
{
  xbShort rc, i;
  FILE *t;
  xbLong l;
  char *target, *source;
  xbString TempDbfName;
  xbString TempDbtName;
  char   * Buf = 0; 

#ifdef XB_MEMO_FIELDS
  char tbuf[4];
#endif

#ifdef XB_MEMO_FIELDS
  xbLong   len, BufSize;
  xbShort  MemoFields;
#endif  /* XB_MEMO_FIELDS */

  xbDbf Temp( xbase );
  CreateUniqueDbfName( TempDbfName, TempDbtName );

  if(( t = fopen( TempDbfName, "w+b" )) == NULL )
    return XB_OPEN_ERROR;

  /* copy file header */
  if(( rc = _fseek( fp, 0, SEEK_SET )) != 0 )
    return XB_SEEK_ERROR;

  for( i = 0; i < HeaderLen; i++ )
    fputc( fgetc( fp ), t );
  fputc( 0x1a, t );

  if( fclose( t ) != 0 )
    return XB_CLOSE_ERROR;

#ifdef XB_MEMO_FIELDS
  if(( MemoFields = MemoFieldsPresent()) > 0 ){

    if((t = fopen( TempDbtName, "w+b" )) == NULL)
       return XB_OPEN_ERROR;

    l = 1L;
    memset( tbuf, 0x00, 4 );
    xbase->PutLong( tbuf, l );

    if((fwrite(&tbuf, 4, 1, t)) != 1)
      return XB_WRITE_ERROR;

    if( MemoHeader.Version == 0x03 ){
      for( i = 0; i < 12; i++ ) fputc( 0x00, t );
        fputc( 0x03, t );
      for( i = 0; i < 495; i++ ) fputc( 0x00, t );
    } else {
      for( i = 0; i < 4; i++ ) fputc( 0x00, t );
      if ((fwrite(&MemoHeader.FileName, 8, 1, t)) != 1)
        return XB_WRITE_ERROR;
      for( i = 0; i < 4; i++ ) fputc( 0x00, t );
        memset( tbuf, 0x00, 2 );
      xbase->PutShort( tbuf, MemoHeader.BlockSize );
      if ((fwrite(&tbuf, 2, 1, t)) != 1)
        return XB_WRITE_ERROR;

      for( i = 22; i < MemoHeader.BlockSize; i++ ) fputc( 0x00, t );
    }

    if( fclose( t ) != 0 )
      return XB_CLOSE_ERROR;
   }
#endif   /* XB_MEMO_FIELDS */

   /* reopen as database */
   if(( rc = Temp.OpenDatabase( TempDbfName )) != XB_NO_ERROR )
     return rc;

#ifdef XB_REAL_DELETE
   if(RealDelete)
     Temp.RealDeleteOn();
   Temp.FirstFreeRec = 0;
   Temp.RealNumRecs = 0;
#endif   
   Temp.ResetNoOfRecs();
   Temp.WriteHeader(2);          // flush NoOfRecs=0 to disk
   target = Temp.GetRecordBuf();
   source = GetRecordBuf();

   for( l = 1; l <= PhysicalNoOfRecords(); l++ ){
     if(statusFunc && (l == 1 || !(l % 100) || l == PhysicalNoOfRecords()))
       statusFunc(l, PhysicalNoOfRecords());

     if(( rc = GetRecord( l )) != XB_NO_ERROR )
       return rc;

     if( !RecordDeleted() ){
       memcpy( target, source, GetRecordLen());

#ifdef XB_MEMO_FIELDS
       BufSize = 0L;
//     Buf = NULL;   Already set to 0, this statement flags as memory leak

       for( i = 0; i < NoOfFields; i++ ){
         if( GetFieldType( i ) == 'M' && MemoFieldExists( i )){
           Temp.PutLongField(i, 0L);
           len = GetMemoFieldLen( i );
           if( len > BufSize ){
             if( Buf )
               free( Buf );
             if((Buf = (char *)malloc(len)) == NULL)
               return XB_NO_MEMORY;
             BufSize = len;
           }
           GetMemoField( i, len, Buf, -1 );
           Temp.UpdateMemoData( i, len, Buf, -1 );
         }
       }
#endif
       if(( rc = Temp.AppendRecord()) != XB_NO_ERROR ){
         if(Buf) free(Buf);
         return rc;
       }
     }
   }
   if( Buf ) free( Buf );
   Temp.CloseDatabase();

   if(fclose(fp) != 0)
     return XB_CLOSE_ERROR;

   if(remove(GetFileName()) != 0)
     return XB_WRITE_ERROR;

   if(rename(TempDbfName, GetFileName()) != 0)
     return XB_WRITE_ERROR;

#ifdef XB_MEMO_FIELDS
   if( MemoFields ){
     if(fclose(mfp) != 0)       
       return XB_CLOSE_ERROR;

     if(remove(MemofileName) != 0)
       return XB_WRITE_ERROR;     
     if( rename( TempDbtName, MemofileName ) != 0 )
       return XB_WRITE_ERROR;
     if(( mfp = fopen( MemofileName, "r+b" )) == NULL )
       return XB_OPEN_ERROR;
     if(( rc = GetDbtHeader(1)) != 0 ){
       fclose( mfp );
       return rc;
     }
#ifdef XB_LOCKING_ON
     /* no buffering in multi user mode */
     setbuf( mfp, NULL );
#endif
  }

#endif /* XB_MEMO_FIELDS */

  if(( fp = fopen( GetFileName(), "r+b" )) == NULL )
    return XB_OPEN_ERROR;

#ifdef XB_LOCKING_ON
  /* no buffering in multi user mode */
  setbuf( fp, NULL );
#endif

  return XB_NO_ERROR;
}
/************************************************************************/
//! Pack the database
/*!
  This method removes all records marked for deletion from an Xbase (.DBF)
  file, reindexes any open index files, and also reorganizes any memo fields
  stored in a .DBT memo file.

  \param packStatusFunc status function
  \param indexStatusFunc index status function

  \param LockWaitOption One of the following:
    \htmlonly
      <p>
      <table border=2><tr><th>LockWaitOption</th><th>Description</th></tr>
        <tr><td>F_SETLK</td><td>Return immediately if the DBF file cannot be locked</td></tr>
        <tr><td>XB_LOCK</td><td>Wait for lock on DBF file to succeed</td></tr>
      </table>
    \endhtmlonly
    \latexonly
      \\
      \\
      \begin{tabular}{|l|l|} \hline
        \textbf{LockWaitOption} & \textbf{Description} \\ \hline \hline
        F\_SETLK & Return immediately if DBF file cannot be locked \\ \hline
        F\_SETLKW  & Wait for lock on DBF file to succeed \\ \hline
      \end{tabular}
    \endlatexonly
    
  \returns One of the following return codes:
    \htmlonly
      <p>
      <table border=2><tr><th>Return Code</th><th>Description</th></tr>
        <tr><td>XB_NO_ERROR</td><td>No error</td></tr>
   <tr><td>XB_CLOSE_ERROR</td><td>Unable to close intermediate work file</td></tr>
   <tr><td>XB_OPEN_ERROR</td><td>Could not open file</td></tr>
   <tr><td>XB_NO_MEMORY</td><td>Memory allocation error</td></tr>
        <tr><td>XB_WRITE_ERROR</td><td>Couldn't write to disk</td></tr>
   <tr><td>XB_SEEK_ERROR</td><td>Error seeking file</td></tr>
   <tr><td>XB_LOCK_FAILED</td><td>Unable to lock file or index</td></tr>
      </table>
    \endhtmlonly
    \latexonly
      \\
      \\
      \begin{tabular}{|l|l|} \hline
        \textbf{Return Code} & \textbf{Description} \\ \hline \hline
        XB\_NO\_ERROR & No Error \\ \hline
   XB\_CLOSE\_ERROR & Unable to close intermediate work file \\ \hline
        XB\_OPEN\_ERROR & Couldn't open the file \\ \hline
   XB\_NO\_MEMORY & Memory allocation error \\ \hline
        XB\_WRITE\_ERROR & Couldn't write to disk \\ \hline
   XB\_SEEK\_ERROR & Error seeking file \\ \hline
   XB\_LOCK\_FAILED & Unable to lock file or index \\ \hline
      \end{tabular}
    \endlatexonly
*/
xbShort xbDbf::PackDatabase(xbShort LockWaitOption,
            void (*packStatusFunc)(xbLong itemNum, xbLong numItems),
            void (*indexStatusFunc)(xbLong itemNum, xbLong numItems))
{
  xbShort rc;

  /* lock all open files and indexes */
//  if(( rc = ExclusiveLock( LockWaitOption )) != XB_NO_ERROR ) return rc;

  if(( rc = PackDatafiles(packStatusFunc)) != XB_NO_ERROR ){
//    ExclusiveUnlock();
    return rc;
  }

  /* refresh file header */
  if(( rc = ReadHeader(1)) != XB_NO_ERROR )
    return rc;

  if(( rc = RebuildAllIndices(indexStatusFunc)) != XB_NO_ERROR ) 
    return rc;

//  ExclusiveUnlock();
  return XB_NO_ERROR;
}
/************************************************************************/
//! Copy DBF structure
/*!
*/
xbShort xbDbf::CopyDbfStructure(const char *NewFileName, xbShort Overlay) {

  xbShort rc, i;
  xbString ndfn;       /* new dbf file name */
  char  ch;

#ifdef XB_MEMO_FIELDS
  char buf[9];
  xbShort ct, NameLen;
  xbString MemoName;
#endif
  FILE  *t;

  /* build the new file name */
  rc = NameSuffixMissing( 1, NewFileName );
  ndfn = NewFileName;
  if( rc == 1 )
    ndfn += ".dbf";
  else if( rc == 2 )
    ndfn += ".DBF";

  /* check if the file exists and Overlay is on */
  if(((t = fopen( ndfn, "r" )) != NULL ) && !Overlay) {
    fclose(t);
    return XB_FILE_EXISTS;
  }

  /* open new file */
  if((t = fopen(ndfn, "w+b")) == NULL)
    return XB_OPEN_ERROR;

  /* copy the file header */
  if(( rc = _fseek( fp, 0, SEEK_SET )) != 0 )
    return XB_SEEK_ERROR;

  fputc( fgetc( fp ), t );

  /* do the date */
  xbDate d;
  ch = d.YearOf() - 1900;
  if(XFV == 3)
    ch %= 100;          // dBASE III+ does this, dBASE IV does not.
  fputc( ch, t );
  ch = d.MonthOf();
  fputc( ch, t );
  ch = d.DayOf( XB_FMT_MONTH );
  fputc( ch, t );

  /* record count */
  for( i = 0; i < 4; i++ ) fputc( 0x00, t );

  if((rc = _fseek(fp, 7, SEEK_CUR)) != 0) {
    fclose( t );
    return XB_SEEK_ERROR;
  }
  for( i = 0; i < 4; i++ )
    fputc( fgetc( fp ), t );

  for( i = 0; i < 17; i++ )
    fputc( 0x00, t );

  if((rc = _fseek( fp, 17, SEEK_CUR )) != 0) {
    fclose( t );
    return XB_SEEK_ERROR;
  }

  for( i = 29; i < HeaderLen; i++ )
    fputc( fgetc( fp ), t );

  fputc( 0x1a, t );
  fclose( t );

#ifdef XB_MEMO_FIELDS
  if( MemoFieldsPresent()){
     MemoName = ndfn;

     NameLen = MemoName.len();
     NameLen--;
     if( MemoName.getCharacter( NameLen ) == 'F' )
       MemoName.putAt(NameLen, 'T');
     else
       MemoName.putAt(NameLen, 't');

     if(( t = fopen( MemoName, "w+b" )) == NULL )
       return XB_OPEN_ERROR;

     memset( buf, 0x00, 4 );
     xbase->PutLong( buf, 1L );
     if(( fwrite( &buf, 4, 1, t )) != 1 ){
       fclose( t );
       return XB_WRITE_ERROR;
     }
     if( MemoHeader.Version == 0x03 ){
       for( i = 0; i < 12; i++ ) fputc( 0x00, t );
       fputc( 0x03, t );
       for( i = 0; i < 495; i++ ) fputc( 0x00, t );
     }
     else
     {
       for( i = 0; i < 4; i++ ) fputc( 0x00, t );  // put 4 bytes 0x00
       memset( buf, 0x00, 9 );
       NameLen = ndfn.len();
       for( i = 0, ct = 0; i < NameLen; i++ )
         if( ndfn.getCharacter( i ) == PATH_SEPARATOR ){
           ct = i;
           ct++;
         }

       for( i = 0; i < 8 && ndfn[i+ct] != '.'; i++ )
         buf[i] = ndfn[i+ct];

       fwrite( &buf, 8, 1, t );
       for( i = 0; i < 4; i++ ) fputc( 0x00, t );
       memset( buf, 0x00, 2 );
       xbase->PutShort( buf, MemoHeader.BlockSize );
       if(( fwrite( &buf, 2, 1, t )) != 1 ){
         fclose(t);
         return XB_WRITE_ERROR;
       }
       for( i = 22; i < MemoHeader.BlockSize; i++ ) fputc( 0x00, t );
     }
   }
   fclose( t );
#endif   // XB_MEMO_FIELDS
   return XB_NO_ERROR;
}
/************************************************************************/
//! Add index to list
/*!
  Adds the specified index to the list of indexes maintained by the 
  dbf.
  
  \param n index to add
  \param IndexName name of index
*/
#if defined(XB_INDEX_ANY)
xbShort xbDbf::AddIndexToIxList(xbIndex * n, const char *IndexName)
{
  xbIxList *i, *s, *t;

  if( !FreeIxList ){
    if((i = (xbIxList *) malloc(sizeof(xbIxList))) == NULL) 
      return XB_NO_MEMORY;
  }
  else
  {
    i = FreeIxList;
    FreeIxList = i->NextIx;
  } 
  memset(i, 0x00, sizeof(xbIxList));

  i->IxName  = IndexName;
  i->index   = n;

  s = NULL;
  t = NdxList;
  while( t && strcmp( t->IxName, IndexName ) < 0 ){
    s = t;
    t = t->NextIx;
  }
  i->NextIx = t;
  if( s == NULL )
    NdxList = i;
  else
    s->NextIx = i;
  return 0;
}
#endif
/************************************************************************/
//! Rebuild all index files
/*!
*/
xbShort xbDbf::RebuildAllIndices(void (*statusFunc)(xbLong itemNum, xbLong numItems))
{
#if defined(XB_INDEX_ANY)
  xbShort rc;
  xbIxList *n;

  n = NdxList;
  while( n ){
    if(( rc = n->index->ReIndex(statusFunc)) != XB_NO_ERROR ){
//      ExclusiveUnlock();
      return rc;
    }
    n = n->NextIx;
  }
#endif
  return XB_NO_ERROR;
}
/************************************************************************/
//! Delete all records
/*!
*/
xbShort xbDbf::DeleteAll( xbShort Option )
{
  xbShort rc;

  if(( NoOfRecords()) == 0 ) 
    return XB_NO_ERROR;
  if(( rc = GetFirstRecord()) != XB_NO_ERROR )
    return rc;

  if( Option == 0 ){   /* delete all option */
    while( 1 ){
      if( !RecordDeleted())
        if(( rc = DeleteRecord()) != XB_NO_ERROR )
          return rc;
      if(( rc = GetNextRecord()) != XB_NO_ERROR )
        break;
    }
  }
  else   /* undelete all option */
  {
    while( 1 ){
      if( RecordDeleted())
        if(( rc = UndeleteRecord()) != XB_NO_ERROR )
          return rc;
        if(( rc = GetNextRecord()) != XB_NO_ERROR )
          break;
    }
  }
  if( rc == XB_EOF )
    return XB_NO_ERROR;
  else
    return rc;
}
/************************************************************************/
//! Delete all records and pack data file
/*!
*/
xbShort xbDbf::Zap( xbShort WaitOption )
{
  xbShort rc;
  xbString TempDbfName, TempDbtName;
  
  CreateUniqueDbfName( TempDbfName, TempDbtName );
  if(( rc = CopyDbfStructure( TempDbfName, 1 )) != XB_NO_ERROR) {
    return rc;
  }

  if( fp ){
    fclose( fp );
    fp = 0;
  }

  if(( rc = remove( GetFileName() )) != 0 )
    return XB_WRITE_ERROR;

  if(( rc = rename( TempDbfName, GetFileName() )) != 0 ) 
    return XB_WRITE_ERROR;
  
  if((fp = fopen( GetFileName(), "r+b" )) == NULL)
    return XB_OPEN_ERROR;

#ifdef XB_LOCKING_ON
  setbuf( fp, NULL );
#endif
  ReadHeader( 1 );

#ifdef XB_MEMO_FIELDS
  if( MemoFieldsPresent() ){
    fclose( mfp );

  if(( rc = remove( MemofileName )) != 0 )
    return XB_WRITE_ERROR;

  if(( rc = rename( TempDbtName, MemofileName )) != 0 ) 
    return XB_WRITE_ERROR;
  
  if(( mfp = fopen( MemofileName, "r+b" )) == NULL)
    return XB_OPEN_ERROR;

  }
#endif   // XB_MEMO_FIELDS

  if(( rc = RebuildAllIndices()) != XB_NO_ERROR )
    return rc;
  
  return XB_NO_ERROR;
}
/************************************************************************/
//! Remove an index from the list
/*!
*/
#if defined(XB_INDEX_ANY)
xbShort xbDbf::RemoveIndexFromIxList(xbIndex * n) {
  xbIxList *i, *s;

  i = NdxList;
  s = NULL;
  while( i ){
    if( i->index == n ){
      /* remove it from current chain */
      if( s )
        s->NextIx = i->NextIx;
      else
        NdxList = i->NextIx;

      /* add i to the current free chain */
      i->NextIx = FreeIxList;
      FreeIxList = i;
      FreeIxList->IxName = (const char *)NULL;
      FreeIxList->index = NULL;
      break;
    }
    else
    {
      s = i;
      i = i->NextIx;
    }
  }
  return XB_NO_ERROR;
} 
#endif

/************************************************************************/
//! Gets the number of records in the data file
/*!
*/
xbLong xbDbf::NoOfRecords()
{
  xbLong numRecs;

/* lock the database */
#ifdef XB_LOCKING_ON
  xbShort rc;

  if( AutoLock ){
//    if(( rc = LockDatabase( XB_LOCK, 0L )) != XB_NO_ERROR )
//      return rc;

    if((rc = ReadHeader(1)) != XB_NO_ERROR){
//      if(AutoLock)
//        LockDatabase( XB_UNLOCK, 0L );
      return rc;
    }
  }
#endif

#ifndef XB_REAL_DELETE
  numRecs = NoOfRecs;
#else
  numRecs = RealDelete ? RealNumRecs : NoOfRecs;
#endif

#ifdef XB_LOCKING_ON
//  if(AutoLock)
//    LockDatabase( XB_UNLOCK, 0L );
#endif

  return numRecs;
}
/************************************************************************/
//! Get the physical number of records in the data file
/*!
*/
xbLong xbDbf::PhysicalNoOfRecords()
{
  xbShort rc;
    
/* lock the database */
#ifdef XB_LOCKING_ON
//  if( AutoLock )
//    if(( rc = LockDatabase( XB_LOCK, 0L )) != XB_NO_ERROR )
//       return rc;
#endif

  rc = ReadHeader(1);

#ifdef XB_LOCKING_ON
//  if(AutoLock)
//    if(( rc = LockDatabase( XB_UNLOCK, 0L )) != XB_NO_ERROR )
//      return rc;
#endif

  if( rc )
    return rc;
 
  return NoOfRecs;
}

/************************************************************************/
#if defined(XB_INDEX_ANY)
//! Get the number of currently open indexes for data file
/*!
*/
xbShort xbDbf::IndexCount()
{
  xbShort count;
  xbIxList *i;

  for(count = 0, i = NdxList; i; i = i->NextIx, count++) ;

  return count;
}
/************************************************************************/
//! Get a specific index
/*!
*/
xbIndex * xbDbf::GetIndex(xbShort indexNum)
{
  xbIxList *i;

  i = NdxList;
  while(indexNum && i){
    indexNum--;
    i = i->NextIx;
  }

  if(i)
    return i->index;

  return 0;
}

#endif // XB_INDEX_ANY
/************************************************************************/
void xbDbf::Flush()
{
  if(fp)
    fflush(fp);

#ifdef XB_MEMO_FIELDS
  if(mfp)
    fflush(mfp);
#endif

#if defined(XB_INDEX_ANY)
  xbIxList
    *i;

  i = NdxList;
  while(i) {
    i->index->Flush();
    i = i->NextIx;
  }
#endif
}
/************************************************************************/
#ifdef XB_LOCKING_ON
xbShort xbDbf::SetLockMode( xbShort nlm )
{
/*
  xbShort rc;
  if( LockMode != XB_XBASE_LOCK_MODE &&
      nlm == XB_XBASE_LOCK_MODE      &&
      !xblfh ){
    rc = OpenXbLockFile();
    if( rc )
      return rc;
  }
*/  
  LockMode = nlm;
  return XB_NO_ERROR;   
}
#endif
/************************************************************************/
const char  * xbDbf::GetExtWithDot( bool lower )
{
  return lower ? ".dbf" : ".DBF";
}
