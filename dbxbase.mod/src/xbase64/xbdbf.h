/*  xbdbf.h

    Xbase64 project source code

    This file contains the Class definition for a xbDBF object.

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

#ifndef __XB_DBF_H__
#define __XB_DBF_H__

#ifdef __GNU_LesserG__
#pragma interface
#endif

#ifdef __WIN32__
#include <xbase64/xbwincfg.h>
#else
#include <xbase64/xbconfig.h>
#endif

#include <xbase64/xbtypes.h>
#include <xbase64/xbdate.h>
#include <xbase64/xbfile.h>

#include <iostream>
#include <stdio.h>

/*! \file xbdbf.h
*/

#if defined(XB_INDEX_ANY)
   class XBDLLEXPORT xbIndex;
   class XBDLLEXPORT xbNdx;
   class XBDLLEXPORT xbNtx;
#endif

/*****************************/
/* Field Types               */

#define XB_CHAR_FLD      'C'
#define XB_LOGICAL_FLD   'L'
#define XB_NUMERIC_FLD   'N'
#define XB_DATE_FLD      'D'
#define XB_MEMO_FLD      'M'
#define XB_FLOAT_FLD     'F'

/*****************************/
/* File Status Codes         */

#define XB_CLOSED  0
#define XB_OPEN    1
#define XB_UPDATED 2

/*****************************/
/* Other defines             */

#define XB_OVERLAY     1
#define XB_DONTOVERLAY 0

#define XB_CHAREOF  '\x1A'         /* end of DBF        */
#define XB_CHARHDR  '\x0D'         /* header terminator */

//! Used to define the fields in a database (DBF file).
/*!
  Generally one would define an xbSchema array to be passed
  to xbDbf::CreateDatabase() to define the fields in the database.

  For example, one might create a declaration as follows:
  
  \code  
  xbSchema MyRecord[] = 
  {
    { "FIRSTNAME", XB_CHAR_FLD,     15, 0 },
    { "LASTNAME",  XB_CHAR_FLD,     20, 0 },
    { "BIRTHDATE", XB_DATE_FLD,      8, 0 },
    { "AMOUNT",    XB_NUMERIC_FLD,   9, 2 },
    { "SWITCH",    XB_LOGICAL_FLD,   1, 0 },
    { "FLOAT1",    XB_FLOAT_FLD,     9, 2 },
    { "FLOAT2",    XB_FLOAT_FLD,     9, 1 },
    { "FLOAT3",    XB_FLOAT_FLD,     9, 2 },
    { "FLOAT4",    XB_FLOAT_FLD,     9, 3 },
    { "MEMO1",     XB_MEMO_FLD,     10, 0 },
    { "ZIPCODE",   XB_NUMERIC_FLD,   5, 0 },      
    { "",0,0,0 }
  };
  \endcode
  
  Note that the last xbSchema in an array must be a "null" entry like the 
  one above:
  
  \code
    { "",0,0,0 }
  \endcode
  
  To indicate the end of the array.
*/
struct XBDLLEXPORT xbSchema {
  char      FieldName[11];
  char      Type;
// xbUShort  FieldLen;       /* does not work */
// xbUShort  NoOfDecs;       /* does not work */
  unsigned  char FieldLen;  /* fields are stored as one byte on record*/
  unsigned  char NoOfDecs;
};

//! Defines a field in an XBase file header (DBF file header)
/*!
  This structure is only used internally by the xbDbf class.
*/
struct XBDLLEXPORT xbSchemaRec {
  char     FieldName[11];
  char     Type;            /* field type */
  char     *Address;        /* pointer to field in record buffer 1 */
// xbUShort FieldLen;        /* does not work */
// xbUShort NoOfDecs;        /* does not work */
  unsigned char FieldLen;   /* fields are stored as one byte on record */
  unsigned char NoOfDecs;
  char     *Address2;       /* pointer to field in record buffer 2 */
  char     *fp;             /* pointer to null terminated buffer for field */
                             /* see method GetString */
  xbShort  LongFieldLen;    /* to handle long field lengths */
};

//! xbIxList struct
/*!
  Internal use only.
*/
struct XBDLLEXPORT xbIxList {
  xbIxList * NextIx;
  xbString IxName;
#if defined(XB_INDEX_ANY)
  xbIndex  * index;
  xbShort  Unique;
  xbShort  KeyUpdated;
#endif
};

//! xbMH struct
/*!
  Internal use only.
*/

#ifdef XB_MEMO_FIELDS
struct XBDLLEXPORT xbMH{                      /* memo header                    */
  xbLong  NextBlock;             /* pointer to next block to write */
  char    FileName[8];           /* name of dbt file               */
  char    Version;               /* not sure                       */
  xbShort BlockSize;             /* memo file block size           */
};
#endif

//! xbDbf class
/*!
  The xbDbf class encapsulates an xbase DBF database file.  It includes
  all dbf access, field access, and locking methods.
*/
class XBDLLEXPORT xbDbf : protected xbFile{

public:
  xbDbf( xbXBase * );
  virtual ~xbDbf();

  xbXBase  *xbase;               /* linkage to main base class */

/* datafile methods */
#if defined(XB_INDEX_ANY)
  xbShort   AddIndexToIxList(xbIndex *, const char *IndexName);
  xbShort   RemoveIndexFromIxList( xbIndex * );
#endif
  xbShort   AppendRecord();
  xbShort   BlankRecord();
  xbShort   CloseDatabase( xbBool deleteIndexes = 0 );
  xbShort   CopyDbfStructure( const char *, xbShort );
  xbShort   CreateDatabase( const char * Name, xbSchema *, xbShort Overlay );
  //! Delete all records
  /*!
  */
  xbShort   DeleteAllRecords() { return DeleteAll(0); }
  xbShort   DeleteRecord();
#ifdef XBASE_DEBUG
  xbShort   DumpHeader( xbShort );
#endif
  xbShort   DumpRecord( xbULong );
  //! Return number of fields
  /*!
  */
  xbLong    FieldCount() { return NoOfFields; }
  //! Return Dbf name
  /*!
  */
  const xbString& GetDbfName() { return GetFileName(); }
  //! Return status
  /*!
  */
  xbShort   GetDbfStatus() { return DbfStatus; }
  xbShort   GetFirstRecord();
  xbShort   GetLastRecord();
  xbShort   GetNextRecord();
  xbShort   GetPrevRecord();
  //! Return current record number
  /*!
  */
  xbLong    GetCurRecNo() { return CurRec; }
  xbShort   GetRecord( xbULong );
  //! Return a pointer to the record buffer
  /*!
  */
  char *    GetRecordBuf() { return RecBuf; }
  //! Return record length
  /*!
  */
  xbShort   GetRecordLen() { return RecordLen; }
  xbShort   NameSuffixMissing( xbShort, const char * );
  xbLong    GetRecCnt() { return NoOfRecords(); }
  xbLong    NoOfRecords();
  xbLong    PhysicalNoOfRecords();
  xbShort   OpenDatabase( const char * );
  xbShort   PackDatabase(xbShort LockWaitOption,
        void (*packStatusFunc)(xbLong itemNum, xbLong numItems) = 0,
        void (*indexStatusFunc)(xbLong itemNum, xbLong numItems) = 0);
  xbShort   PutRecord(); // Put record to current position
  xbShort   PutRecord(xbULong);
  xbShort   RebuildAllIndices(
      void (*statusFunc)(xbLong itemNum, xbLong numItems) = 0);
  xbShort   RecordDeleted();
  //! Set number of records to zero????
  /*!
  */
  void      ResetNoOfRecs() { NoOfRecs = 0L; }
  xbShort   SetVersion( xbShort );
  //! Undelete all records
  /*!
  */
  xbShort   UndeleteAllRecords() { return DeleteAll(1); }
  xbShort   UndeleteRecord();
  xbShort   Zap( xbShort );

/* field methods */
  const char *GetField(xbShort FieldNo) const; // Using internal static buffer
  const char *GetField(const char *Name) const;
  xbShort   GetField( xbShort FieldNo, char *Buf) const;
  xbShort   GetRawField( xbShort FieldNo, char *Buf) const;
  xbShort   GetField( xbShort FieldNo, char *Buf, xbShort RecBufSw) const;
  xbShort   GetField( const char *Name, char *Buf) const;
  xbShort   GetRawField(const char *Name, char *Buf) const;
  xbShort   GetField( const char *Name, char *Buf, xbShort RecBufSw) const;
  xbShort   GetField(xbShort FieldNo, xbString&, xbShort RecBufSw ) const;
  xbShort   GetFieldDecimal( xbShort );
  xbShort   GetFieldLen( xbShort );
  char *    GetFieldName( xbShort );
  xbShort   GetFieldNo( const char * FieldName ) const;
  char      GetFieldType( xbShort FieldNo ) const;
  xbShort   GetLogicalField( xbShort FieldNo );
  xbShort   GetLogicalField( const char * FieldName );
  char *    GetStringField( xbShort FieldNo );
  char *    GetStringField( const char * FieldName );
  xbShort   PutField( xbShort, const char * );
  xbShort   PutRawField( xbShort FieldNo, const char *buf );
  xbShort   PutField( const char *Name, const char *buf);
  xbShort   PutRawField( const char *Name, const char *buf );
  xbShort   ValidLogicalData( const char * );
  xbShort   ValidNumericData( const char * );

  xbLong    GetLongField( const char *FieldName) const;
  xbLong    GetLongField( const xbShort FieldNo) const;
  xbShort   PutLongField( const xbShort, const xbLong );
  xbShort   PutLongField( const char *, const xbLong);

  xbFloat   GetFloatField( const char * FieldName );
  xbFloat   GetFloatField( xbShort FieldNo );
  xbShort   PutFloatField( const char *, const xbFloat);
  xbShort   PutFloatField( const xbShort, const xbFloat);

  xbDouble  GetDoubleField( const char *);
  xbDouble  GetDoubleField( xbShort, xbShort RecBufSw = 0);
  xbShort   PutDoubleField( const char *, xbDouble);
  xbShort   PutDoubleField( const xbShort, xbDouble);

#ifdef XB_LOCKING_ON
  xbShort   GetLockMode() { return LockMode; }
  xbShort   SetLockMode( xbShort );
//  xbShort   OpenXbLockFile();
//  xbShort   GetTableLockCnt() { return TableLockCnt; }
//  xbShort   LockIndex( xbShort LockType );    /* for XB_XBASE_LOCK_MODE */
  int       GetDbfFileNo() { return fileno( fp ); }
  int       GetMemoFileNo() { return fileno( mfp ); }

#ifdef XB_MEMO_FIELDS
//  xbShort   GetMemoLockCnt() { return MemoLockCnt; }
#endif
  
/*
  xbShort   LockTable( xbShort LockType );
  xbShort   LockXbaseTable( xbShort LockType );
  xbShort   LockClipperTable( xbShort LockType );
  xbShort   LockFoxproTable( xbShort LockType );
  xbShort   LockDbaseTable( xbShort LockType );
  
  xbShort   LockRecord( xbShort LockType, xbULong RecNo, xbULong RecCnt );
  xbShort   LockXbaseRecord( xbShort LockType, xbULong RecNo, xbULong RecCnt );
  xbShort   LockClipperRecord( 
               xbShort LockType, xbULong RecNo, xbULong RecCnt );
  xbShort   LockFoxproRecord( xbShort LockType, xbULong RecNo, xbULong RecCnt );
  xbShort   LockDbaseRecord( xbShort LockType, xbULong RecNo, xbULong RecCnt );

  xbShort   LockDatabase( xbShort, xbShort, xbULong );
  xbShort   ExclusiveLock( xbShort );
  xbShort   ExclusiveUnlock();
  xbShort   LockDatabase( xbShort cmd, xbULong recNo ) { return 0; }
*/

#ifndef HAVE_FCNTL
  xbShort   UnixToDosLockCommand( xbShort WaitOption,
              xbShort LockType ) const;
#endif

#else
  xbShort   LockDatabase( xbShort, xbShort, xbLong )
     { return XB_NO_ERROR; }
  xbShort   ExclusiveLock( xbShort ) { return XB_NO_ERROR; };
  xbShort   ExclusiveUnlock()      { return XB_NO_ERROR; };
#endif

   //! Turn autolock on
   /*!
   */
   void    AutoLockOn()  { AutoLock = 1; }
   //! Turn autolock off
   /*!
   */
   void    AutoLockOff() { AutoLock = 0; }
   //! Return whether or not autolocking is on or off
   /*!
   */
  xbShort GetAutoLock() { return AutoLock; }

#ifdef XB_MEMO_FIELDS
  xbShort   GetMemoField( xbShort FieldNo, xbLong len,
             char * Buf, xbShort LockOption );
  xbLong    GetMemoFieldLen( xbShort FieldNo );
  xbShort   GetFPTField( xbShort FieldNo, xbLong len,
              char * Buf, xbShort LockOption );
  xbLong    GetFPTFieldLen( xbShort FieldNo );
  xbShort   UpdateMemoData( xbShort FieldNo, xbLong len,
              const char * Buf, xbShort LockOption );
  xbShort   MemoFieldExists( xbShort FieldNo ) const;
  xbShort   LockMemoFile( xbShort WaitOption, xbShort LockType );

  xbShort   MemoFieldsPresent() const;
  xbLong    CalcLastDataBlock();
  xbShort   FindBlockSetInChain( xbLong BlocksNeeded, xbLong
               LastDataBlock, xbLong & Location, xbLong &PreviousNode );
  xbShort   GetBlockSetFromChain( xbLong BlocksNeeded, xbLong
               Location, xbLong PreviousNode );
  xbString & GetDbtName() { return MemofileName; }

#ifdef XBASE_DEBUG
  xbShort   DumpMemoFreeChain();
  void      DumpMemoHeader() const;
  void      DumpMemoBlock() const;
#endif
#endif

  //! Turn on "real" deletes
  /*!
     This should be done before creating a database (with 
    xbDbf::CreateDatatabase()) and thereafter before opening
    a database with xbDbfCreateDatabase().
    
    You cannot "turn on" real deletes once a database has been created
    and records added.
  */
  void      RealDeleteOn() { RealDelete = 1; if(fp) ReadHeader(1); }
  /*! Turn off "real" deletes
  */
  void      RealDeleteOff() { RealDelete = 0; if(fp) ReadHeader(1); }
  //! Return whether "real" deletes are on or off
  /*!
 Use this to determine if "real deletes" are being used with
the database.
  */
  xbShort   GetRealDelete() { return RealDelete; }

#if defined(XB_INDEX_ANY)
  xbShort   IndexCount();
  xbIndex   *GetIndex(xbShort indexNum);
#endif

  void      Flush();
  virtual const char* GetExtWithDot( bool lower );

 private:

  xbShort   DeleteAll( xbShort );
  void      InitVars();
  xbShort   PackDatafiles(void (*statusFunc)(xbLong itemNum, xbLong numItems) = 0);
  xbShort   ReadHeader( xbShort );
  xbShort   WriteHeader( xbShort );
  
#ifdef XB_MEMO_FIELDS
  xbShort   AddMemoData( xbShort FieldNo, xbLong Len, const char * Buf );
  xbShort   CreateMemoFile();
  xbShort   DeleteMemoField( xbShort FieldNo );
  xbShort   GetDbtHeader( xbShort Option );
  xbShort   GetMemoBlockSize() { return MemoHeader.BlockSize; }
  xbShort   OpenMemoFile();
  xbShort   OpenFPTFile();
  xbShort   PutMemoData( xbLong StartBlock, xbLong BlocksNeeded,
          xbLong Len, const char * Buf );
  xbShort   ReadMemoBlock( xbLong BlockNo, xbShort Option);
  xbShort   SetMemoBlockSize( xbShort );
  xbShort   UpdateHeadNextNode() const;
  xbShort   WriteMemoBlock( xbLong BlockNo, xbShort Option );
  xbShort   IsType3Dbt() const { return( Version==(char)0x83 ? 1:0 ); }
  xbShort   IsType4Dbt() const
        {return (( Version==(char)0x8B || Version==(char)0x8E ) ? 1:0 );}
  xbShort   CreateUniqueDbfName( xbString &, xbString & );
#endif


//  xbString DatabaseName;
  xbShort  XFV;                  /* xBASE file version            */
  xbShort  NoOfFields;
  char     DbfStatus;            /* 0 = closed
                                    1 = open
                                    2 = updates pending           */
  FILE   *fp;                    /* file pointer                  */
  xbSchemaRec *SchemaPtr;        /* Pointer to field data         */
  char   *RecBuf;                /* Pointer to record buffer      */
  char   *RecBuf2;               /* Pointer to original rec buf   */

#ifdef XB_MEMO_FIELDS
  xbString MemofileName;         /* memo file name                */
  FILE    *mfp;                  /* memo file pointer             */
  void    *mbb;                  /* memo block buffer             */
  xbMH     MemoHeader;           /* memo header structure         */
  xbShort  mfield1;              /* memo block field one FF       */
  xbShort  MStartPos;            /* memo start pos of data        */
  xbLong   MFieldLen;            /* memo length of data           */
  xbLong   NextFreeBlock;        /* next free block in free chain */
  xbLong   FreeBlockCnt;         /* count of free blocks this set */
  xbLong   MNextBlockNo;         /* free block chain              */
  xbLong   MNoOfFreeBlocks;      /* free block chain              */
  xbLong   CurMemoBlockNo;       /* Current block no loaded       */
#endif

/* Next seven variables are read directly off the database header */
/* Don't change the order of the following seven items            */
  char   Version;
  char   UpdateYY;
  char   UpdateMM;
  char   UpdateDD;
//   xbLong   NoOfRecs;
//   xbShort  HeaderLen;
//   xbShort  RecordLen;

  xbULong  NoOfRecs;
  xbUShort HeaderLen;
  xbUShort RecordLen;

//#ifdef XB_REAL_DELETE
  xbULong  FirstFreeRec;
  xbULong  RealNumRecs;
//#endif

//  xbIxList * MdxList;
  xbIxList * NdxList;
  xbIxList * FreeIxList;
  xbULong  CurRec;               /* Current record or zero   */
  xbShort  AutoLock;             /* Auto update option 0 = off  */

//#ifdef XB_REAL_DELETE
  xbShort  RealDelete;           /* real delete option 0 = off */
//#endif

#ifdef XB_LOCKING_ON
  FILE     *xblfh;                /* xbase lock file pointer for xbase locking */
  xbShort  LockMode;              /* lock mode for this table */  
  xbString lfn;                   /* xbase lock file name for xbase locking */
  xbShort  TableLockCnt;          /* number of table locks  */
  xbShort  IndexLockCnt;          /* no of index locks XB_XBASE_LOCK_MODE only */

#ifdef XB_MEMO_FIELDS
  xbShort  MemoLockCnt;           /* number of memo file locks */
#endif
  
  /* old locking stuff */
  xbShort  CurLockType;           /* current type of file lock */
  xbShort  CurLockCount;          /* number of current file locks */
  xbULong  CurLockedRecNo;        /* currently locked record no */
  xbShort  CurRecLockType;        /* current type of rec lock held (F_RDLOCK or F_WRLCK) */
  xbShort  CurRecLockCount;       /* number of current record locks */
  xbShort  CurMemoLockType;       /* current type of memo lock */
  xbShort  CurMemoLockCount;      /* number of current memo locks */
#endif

};
#endif      // __XB_DBF_H__


