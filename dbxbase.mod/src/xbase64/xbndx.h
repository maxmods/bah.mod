/*  xbndx.h

    Xbase64 project source code

    This file contains a header file for the xbNdx object, which is used
    for handling NDX type indices.

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

#ifndef __XB_NDX_H__
#define __XB_NDX_H__

#ifdef __GNU_LesserG__
#pragma interface
#endif

#include <xbase64/xbase64.h>
#include <string.h>

/*! \file xbndx.h
*/

//
// Define the following to use inline versions of the respective methods.
//
#define XB_INLINE_GETDBFNO

#define XB_NDX_NODE_BASESIZE            24      // size of base header data

#define XB_VAR_NODESIZE                 // define to enable variable node sizes

#ifndef XB_VAR_NODESIZE
#define XB_NDX_NODE_SIZE 2048
//#define XB_NDX_NODE_SIZE 512          // standard dbase node size
#else
#define XB_DEFAULT_NDX_NODE_SIZE        512
#define XB_MAX_NDX_NODE_SIZE            4096
#define XB_NDX_NODE_SIZE                NodeSize
#define XB_NDX_NODE_MULTIPLE            512
#endif // XB_VAR_NODESIZE

//! xbNdxHeadnode struct
/*!
*/

struct XBDLLEXPORT xbNdxHeadNode {        /* ndx header on disk */
   xbLong   StartNode;                    /* header node is node 0 */
   xbLong   TotalNodes;                   /* includes header node */
   xbLong   NoOfKeys;                     /* actual count + 1 */
                                          /* not updated by borland dbe? */
   xbUShort KeyLen;                       /* length of key data */
   xbUShort KeysPerNode;
   xbUShort KeyType;                      /* 00 = Char, 01 = Numeric */
   xbLong   KeySize;                      /* key len + 8 bytes */
   char     Unknown2;
   char     Unique;
// char     KeyExpression[488];
#ifndef XB_VAR_NODESIZE
   char     KeyExpression[XB_NDX_NODE_SIZE - 24];
#else
   char     KeyExpression[XB_MAX_NDX_NODE_SIZE - 24];
#endif // XB_VAR_NODESIZE
};

//! xbNdxLeafNode struct
/*!
*/

struct XBDLLEXPORT xbNdxLeafNode {        /* ndx node on disk */
   xbLong   NoOfKeysThisNode;
#ifndef XB_VAR_NODESIZE
   char   KeyRecs[XB_NDX_NODE_SIZE-4];
#else
   char   KeyRecs[XB_MAX_NDX_NODE_SIZE - 4];
#endif // XB_VAR_NODESIZE
};

//! xbNdxNodeLink struct
/*!
*/

struct XBDLLEXPORT xbNdxNodeLink {        /* ndx node memory */
   xbNdxNodeLink * PrevNode;
   xbNdxNodeLink * NextNode;
   xbLong       CurKeyNo;                 /* 0 - KeysPerNode-1 */
   xbLong       NodeNo;
   struct xbNdxLeafNode Leaf;
};

//! xbNdx class
/*!
*/

class XBDLLEXPORT xbNdx : public xbIndex
{
 public:
   xbNdx();
   xbNdx(xbDbf *);
   virtual ~xbNdx();

/* don't uncomment next line - it causes seg faults for some undiagnosed reason*/
//   ~NDX() { if( NdxStatus ) CloseIndex(); }

   xbShort  CreateIndex( const char *IxName, const char *Exp,
                         xbShort Unique, xbShort OverLay );
   xbLong   GetTotalNodes();
   xbULong  GetCurDbfRec() { return CurDbfRec; }
   xbShort  CreateKey( xbShort, xbShort );
   xbShort  GetCurrentKey(char *key);
   xbShort  AddKey( xbLong );
   xbShort  UniqueIndex() { return HeadNode.Unique; }
   xbShort  DeleteKey( xbLong );
   xbShort  KeyWasChanged();
   xbShort  FindKey( const char *Key );
   xbShort  FindKey();
   xbShort  FindKey( xbDouble );
#ifdef XBASE_DEBUG
   void     DumpHdrNode( xbShort Option );
   void     DumpNodeRec( xbLong NodeNo );
   void     DumpNodeChain();
   xbShort  CheckIndexIntegrity( xbShort Option );
#endif
   //! Short description.
   /*!
   */
   xbShort  GetNextKey()  { return GetNextKey( 1 ); }
   //! Short description.
   /*!
   */
   xbShort  GetLastKey()  { return GetLastKey( 0, 1 ); }
   //! Short description.
   /*!
   */
   xbShort  GetFirstKey() { return GetFirstKey( 1 ); }
   //! Short description.
   /*!
   */
   xbShort  GetPrevKey()  { return GetPrevKey( 1 ); }
   xbShort  ReIndex(void (*statusFunc)(xbLong itemNum, xbLong numItems) = 0);
   xbShort  KeyExists( const char * Key ) { return FindKey( Key, strlen( Key ), 0 ); }
   xbShort  KeyExists( xbDouble );

   virtual void SetNodeSize(xbShort size);

   virtual void GetExpression(char *buf, int len);
   virtual const char* GetExtWithDot(bool lower);

  protected:
    virtual xbUShort GetKeyLen();
    virtual const char* GetKeyExpression();
    virtual void FreeNodesMemory();

  protected:
   xbNdxHeadNode HeadNode;
   xbNdxLeafNode LeafNode;
   xbLong xbNodeLinkCtr;
   xbLong ReusedxbNodeLinks;

#ifndef XB_VAR_NODESIZE
   char  Node[XB_NDX_NODE_SIZE];
#else
   char  Node[XB_MAX_NDX_NODE_SIZE];
#endif // XB_VAR_NODESIZE

   xbNdxNodeLink * NodeChain;     /* pointer to node chain of index nodes */
   xbNdxNodeLink * FreeNodeChain; /* pointer to chain of free index nodes */
   xbNdxNodeLink * CurNode;       /* pointer to current node              */
   xbNdxNodeLink * DeleteChain;   /* pointer to chain to delete           */
//   xbNdxNodeLink * CloneChain;    /* pointer to node chain copy (add dup) */

/* private functions */
   xbLong     GetLeftNodeNo( xbShort, xbNdxNodeLink * );


   // in line functions for performance reasons
   //! Short description.
   /*!
   */
   inline xbShort    CompareKey( const char *Key1, const char *Key2, xbShort Klen )
   {
     xbDouble d1, d2;
     int c;

     if(!( Key1 && Key2 )) return -1;

     if( Klen > HeadNode.KeyLen ) Klen = HeadNode.KeyLen;

     if( HeadNode.KeyType == 0 )
     {
       c = memcmp(Key1, Key2, Klen);
       if(c < 0)
         return 2;
       else if(c > 0)
         return 1;
       return 0;
     }
     else      /* key is numeric */
     {
        d1 = dbf->xbase->GetDouble( Key1 );
        d2 = dbf->xbase->GetDouble( Key2 );
        if( d1 == d2 ) return 0;
        else if( d1 > d2 ) return 1;
        else return 2;
     }
   }

#ifndef XB_INLINE_GETDBFNO
   xbLong     GetDbfNo( xbShort, xbNdxNodeLink * );
#else
   //! Short description.
   /*!
   */
   inline xbLong     GetDbfNo( xbShort RecNo, xbNdxNodeLink *n )
   {
     xbNdxLeafNode *temp;
     char *p;
     if( !n ) return 0L;
     temp = &n->Leaf;
     if( RecNo < 0 || RecNo > ( temp->NoOfKeysThisNode - 1 )) return 0L;
     p = temp->KeyRecs + 4;
     p += RecNo * ( 8 + HeadNode.KeyLen );
     return( dbf->xbase->GetLong( p ));
   }
#endif
   char *     GetKeyData( xbShort, xbNdxNodeLink * );
   xbUShort   GetKeysPerNode();
   virtual xbShort    GetHeadNode();
   xbShort    GetLeafNode( xbLong, xbShort );
   xbNdxNodeLink * GetNodeMemory();
   void       ReleaseNodeMemory(xbNdxNodeLink *n, xbBool doFree = false);
   xbShort    BSearchNode(const char *key, xbShort klen,
                         const xbNdxNodeLink *node, xbShort *comp);
   xbLong     GetLeafFromInteriorNode( const char *Tkey, xbShort Klen );
   xbShort    CalcKeyLen();
   xbShort    PutKeyData( xbShort, xbNdxNodeLink * );
   xbShort    PutLeftNodeNo( xbShort, xbNdxNodeLink *, xbLong );
   xbShort    PutLeafNode( xbLong, xbNdxNodeLink * );
   xbShort    PutHeadNode( xbNdxHeadNode *, FILE *, xbShort );
   xbShort    PutDbfNo( xbShort, xbNdxNodeLink *, xbLong );
   xbShort    PutKeyInNode( xbNdxNodeLink *, xbShort, xbLong, xbLong, xbShort );
   xbShort    SplitLeafNode( xbNdxNodeLink *, xbNdxNodeLink *, xbShort, xbLong );
   xbShort    SplitINode( xbNdxNodeLink *, xbNdxNodeLink *, xbLong );
   xbShort    AddToIxList();
   xbShort    RemoveFromIxList();
   xbShort    RemoveKeyFromNode( xbShort, xbNdxNodeLink * );
   xbShort    FindKey( const char *Tkey, xbShort Klen, xbShort RetrieveSw );
   xbShort    UpdateParentKey( xbNdxNodeLink * );
   xbShort    GetFirstKey( xbShort );
   xbShort    GetNextKey( xbShort );
   xbShort    GetLastKey( xbLong, xbShort );
   xbShort    GetPrevKey( xbShort );
   void       UpdateDeleteList( xbNdxNodeLink * );
   void       ProcessDeleteList();
   xbNdxNodeLink * LeftSiblingHasSpace( xbNdxNodeLink * );
   xbNdxNodeLink * RightSiblingHasSpace( xbNdxNodeLink * );
   xbShort    DeleteSibling( xbNdxNodeLink * );
   xbShort    MoveToLeftNode( xbNdxNodeLink *, xbNdxNodeLink * );
   xbShort    MoveToRightNode( xbNdxNodeLink *, xbNdxNodeLink * );
   xbShort    FindKey( const char *Tkey, xbLong DbfRec );   /* for a specific dbf no */
};
#endif      /* __XB_NDX_H__ */
