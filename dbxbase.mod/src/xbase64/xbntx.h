/*  xbntx.h

    Xbase64 project source code

    This file contains a header file for the xbNdx object, which is used
    for handling xbNdx type indices.

    Copyright (C) 1997,2003  Bob Cotton
    
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

#ifndef __XB_NTX_H__
#define __XB_NTX_H__

#ifdef __GNU_LesserG__
#pragma interface
#endif

#include <xbase64/xbase64.h>
#include <string.h>

/*! \file xbntx.h
*/

#define XB_NTX_NODE_SIZE 1024

//! xbNtxHeadNode struct
/*!
*/

struct NtxHeadNode {        /* ntx header on disk */
  xbUShort Signature;       /* Clipper 5.x or Clipper 87 */
  xbUShort Version;         /* Compiler Version */
                            /* Also turns out to be a last modified counter */
  xbLong   StartNode;       /* Offset in file for first index */
  xbULong  UnusedOffset;    /* First free page offset */
  xbUShort KeySize;         /* Size of items (KeyLen + 8) */
  xbUShort KeyLen;          /* Size of the Key */
  xbUShort DecimalCount;    /* Number of decimal positions */
  xbUShort KeysPerNode;     /* Max number of keys per page */
  xbUShort HalfKeysPerNode; /* Min number of keys per page */
  char KeyExpression[256];  /* Null terminated key expression */
  unsigned  Unique;         /* Unique Flag */
  char NotUsed[745];
};

//! xbNtxLeafNode struct
/*!
*/

struct NtxLeafNode {       /* ndx node on disk */
  xbUShort NoOfKeysThisNode;
  char     KeyRecs[XB_NTX_NODE_SIZE];
};


//! xbNtxItem struct
/*!
*/

struct NtxItem
{
  xbULong Node;
  xbULong RecordNumber;
  char Key[256];
};

//! xbNtxNodeLink struct
/*!
*/

struct xbNodeLink {             /* ndx node memory */
  xbNodeLink * PrevNode;
  xbNodeLink * NextNode;
  xbUShort     CurKeyNo;        /* 0 - KeysPerNode-1 */
  xbLong       NodeNo;
  struct       NtxLeafNode Leaf;
  xbUShort   * offsets;
};

//! xbNtx class
/*!
*/

class XBDLLEXPORT xbNtx : public xbIndex
{
protected:
  NtxHeadNode  HeadNode;
  NtxLeafNode  LeafNode;
  xbLong       NodeLinkCtr;
  xbLong       ReusedNodeLinks;
  char         Node[XB_NTX_NODE_SIZE];
  xbNodeLink * NodeChain;        /* pointer to node chain of index nodes */
  xbNodeLink * FreeNodeChain;    /* pointer to chain of free index nodes */
  xbNodeLink * CurNode;          /* pointer to current node              */
  xbNodeLink * DeleteChain;      /* pointer to chain to delete           */
//  xbNodeLink * CloneChain;       /* pointer to node chain copy (add dup) */
  NtxItem PushItem;

/* private functions */
  xbLong     GetLeftNodeNo( xbShort, xbNodeLink * );
  xbShort    CompareKey( const char *, const char *, xbShort );
  xbShort    CompareKey( const char *, const char * );
  xbLong     GetDbfNo( xbShort, xbNodeLink * );
  char *     GetKeyData( xbShort, xbNodeLink * );
  xbUShort   GetItemOffset ( xbShort, xbNodeLink *, xbShort );
  xbUShort   InsertKeyOffset ( xbShort, xbNodeLink * );
  xbUShort   GetKeysPerNode();
  virtual xbShort    GetHeadNode();
  xbShort    GetLeafNode( xbLong, xbShort );
  xbNodeLink * GetNodeMemory();
  xbLong     GetNextNodeNo();
  void       ReleaseNodeMemory(xbNodeLink *n, xbBool doFree = false);
  xbULong    GetLeafFromInteriorNode( const char *, xbShort );
  xbShort    CalcKeyLen();
  xbShort    PutKeyData( xbShort, xbNodeLink * );
  xbShort    PutLeftNodeNo( xbShort, xbNodeLink *, xbLong );
  xbShort    PutLeafNode( xbLong, xbNodeLink * );
  xbShort    PutHeadNode( NtxHeadNode *, FILE *, xbShort );
  xbShort    TouchIndex();
  xbShort    PutDbfNo( xbShort, xbNodeLink *, xbLong );
  xbShort    PutKeyInNode( xbNodeLink *, xbShort, xbLong, xbLong, xbShort );
  xbShort    SplitLeafNode( xbNodeLink *, xbNodeLink *, xbShort, xbLong );
  xbShort    SplitINode( xbNodeLink *, xbNodeLink *, xbLong );
  xbShort    AddToIxList();
  xbShort    RemoveFromIxList();
  xbShort    RemoveKeyFromNode( xbShort, xbNodeLink * );
  xbShort    DeleteKeyFromNode( xbShort, xbNodeLink * );
  xbShort    JoinSiblings(xbNodeLink *, xbShort, xbNodeLink *, xbNodeLink *);
  xbUShort   DeleteKeyOffset( xbShort, xbNodeLink *);
  xbShort    FindKey( const char *, xbShort, xbShort );
  xbShort    UpdateParentKey( xbNodeLink * );
  xbShort    GetFirstKey( xbShort );
  xbShort    GetNextKey( xbShort );
  xbShort    GetLastKey( xbLong, xbShort );
  xbShort    GetPrevKey( xbShort );
  void       UpdateDeleteList( xbNodeLink * );
  void       ProcessDeleteList();
  xbShort    FindKey( const char *, xbLong );   /* for a specific dbf no */

public:
  xbNtx();
  xbNtx(xbDbf *);
  virtual ~xbNtx();

/* note to gak - don't uncomment next line - it causes seg faults */
//   ~NTX() { if( NtxStatus ) CloseIndex(); }

  void     DumpHdrNode  ( xbShort Option );
  void     DumpNodeRec  ( xbLong );
  xbShort  CreateIndex( const char *, const char *, xbShort, xbShort );
  xbLong   GetTotalNodes();
  xbULong  GetCurDbfRec() { return CurDbfRec; }
  void     DumpNodeChain();
  xbShort  CreateKey( xbShort, xbShort );
  xbShort  GetCurrentKey(char *key);
  xbShort  AddKey( xbLong );
  xbShort  UniqueIndex() { return HeadNode.Unique; }
  xbShort  DeleteKey( xbLong DbfRec );
  xbShort  KeyWasChanged();
  xbShort  FindKey( const char * );
  xbShort  FindKey();
  xbShort  FindKey( xbDouble );
  xbShort  GetNextKey()  { return GetNextKey( 1 ); }
  xbShort  GetLastKey()  { return GetLastKey( 0, 1 ); }
  xbShort  GetFirstKey() { return GetFirstKey( 1 ); }
  xbShort  GetPrevKey()  { return GetPrevKey( 1 ); }
  xbShort  ReIndex(void (*statusFunc)(xbLong itemNum, xbLong numItems) = 0) ;
  xbShort  KeyExists( char * Key ) { return FindKey( Key, strlen( Key ), 0 ); }
  xbShort  KeyExists( xbDouble );
  virtual  void GetExpression(char *buf, int len);
#ifdef XBASE_DEBUG
  xbShort  CheckIndexIntegrity( xbShort Option );
#endif

  virtual const char* GetExtWithDot(bool lower);
  
  protected:
    virtual xbUShort GetKeyLen();
    virtual const char* GetKeyExpression();
    virtual void FreeNodesMemory();
};
#endif      /* __XB_NTX_H__ */
