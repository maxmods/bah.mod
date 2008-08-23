/*  xbndx.cpp

    Xbase64 project source code

    NDX indexing routines for X-Base

    Copyright (C) 1997,2003,2004  Gary A Kunkel
    
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
  #pragma implementation "xbndx.h"
#endif

#ifdef __WIN32__
#include <xbase64/xbwincfg.h>
#else
#include <xbase64/xbconfig.h>
#endif

#include <xbase64/xbase64.h>
#include <iostream>

#ifdef XB_INDEX_NDX

#ifdef HAVE_IO_H
#include <io.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include <xbase64/xbexcept.h>

/*! \file xbndx.cpp
*/

/***********************************************************************/
//! Short description
/*!
*/
/* This routine dumps the node chain to stdout                         */
#ifdef XBASE_DEBUG
void xbNdx::DumpNodeChain()
{
  xbNdxNodeLink  *n;
  std::cout << std::endl << "*************************" << std::endl;
  std::cout << "xbNodeLinkCtr = " << xbNodeLinkCtr      << std::endl;
  std::cout << "Reused        = " << ReusedxbNodeLinks  << std::endl;

  n = NodeChain;
  while(n){
    std::cout << "xbNodeLink Chain ->" << n->NodeNo   << std::endl;
    std::cout << "        CurKeyNo ->" << n->CurKeyNo << std::endl;
    n = n->NextNode;
  }
  n = FreeNodeChain;
  while(n){
    std::cout << "FreexbNodeLink Chain " << n->NodeNo << std::endl;
    n = n->NextNode;
  }
  n = DeleteChain;
  while(n){
    std::cout << "DeleteLink Chain " << n->NodeNo << std::endl;
    n = n->NextNode;
  }
}
#endif
/***********************************************************************/
//! Short description
/*!
  \param n
*/
/* This routine returns a chain of one or more index nodes back to the */
/* free node chain                                                     */

void xbNdx::ReleaseNodeMemory(xbNdxNodeLink *n, xbBool doFree)
{
  xbNdxNodeLink *temp;

  if(doFree){
    while(n){
      temp = n->NextNode;
      free(n);
      n = temp;
    }
  } else {
    if( !FreeNodeChain )
      FreeNodeChain = n;
    else {   /* put this list at the end */
      temp = FreeNodeChain;
      while( temp->NextNode )
         temp = temp->NextNode;
      temp->NextNode = n;
    }
  }
}
/***********************************************************************/
//! Short description
/*!
*/
/* This routine returns a node from the free chain if available,       */
/* otherwise it allocates new memory for the requested node             */

xbNdxNodeLink * xbNdx::GetNodeMemory()
{
  xbNdxNodeLink * temp;
  if( FreeNodeChain ){
    temp = FreeNodeChain;
    FreeNodeChain = temp->NextNode;
    ReusedxbNodeLinks++;
  } else {
    temp = (xbNdxNodeLink *) malloc( sizeof( xbNdxNodeLink ));
    xbNodeLinkCtr++;
  }
  memset( temp, 0x00, sizeof( xbNdxNodeLink ));
  return temp;
}
/***********************************************************************/
//! Short description
/*!
*/
#ifdef XBASE_DEBUG
void xbNdx::DumpHdrNode( xbShort opt )
{
  if( opt ){
    FILE * log;
    if(( log = fopen( "xbase64.log", "a+t" )) == NULL ) return;
    fprintf( log, "Index Header Node for %s\n", GetFileName().getData());
    fprintf( log, "--------------------------------\n" );
    fprintf( log, "Start node    = %ld\n",  HeadNode.StartNode );
    fprintf( log, "Total nodes   = %ld\n",  HeadNode.TotalNodes );
    fprintf( log, "No of keys    = %ld\n",  HeadNode.NoOfKeys );
    fprintf( log, "Key Length    = %d\n",   HeadNode.KeyLen );
    fprintf( log, "Keys Per Node = %d\n",   HeadNode.KeysPerNode );
    fprintf( log, "Key type      = %d\n",   HeadNode.KeyType );
    fprintf( log, "Key size      = %ld\n",  HeadNode.KeySize );
    fprintf( log, "Unknown 2     = %d\n",   HeadNode.Unknown2 );
    fprintf( log, "Unique        = %d\n",   HeadNode.Unique );
    fprintf( log, "KeyExpression = %s\n",   HeadNode.KeyExpression );
    fclose( log );
  }
  else
  {
    std::cout << "Start node    = " << HeadNode.StartNode     << std::endl;
    std::cout << "Total nodes   = " << HeadNode.TotalNodes    << std::endl;
    std::cout << "No of keys    = " << HeadNode.NoOfKeys      << std::endl;
    std::cout << "Key Length    = " << HeadNode.KeyLen        << std::endl;
    std::cout << "Keys Per Node = " << HeadNode.KeysPerNode   << std::endl;
    std::cout << "Key type      = " << HeadNode.KeyType       << std::endl;
    std::cout << "Key size      = " << HeadNode.KeySize       << std::endl;
    std::cout << "Unknown 2     = " << HeadNode.Unknown2      << std::endl;
    std::cout << "Unique        = " << HeadNode.Unique        << std::endl;
    std::cout << "KeyExpression = " << HeadNode.KeyExpression << std::endl;
#ifdef XB_VAR_NODESIZE
    std::cout << "NodeSize      = " << NodeSize               << std::endl;
#endif // XB_VAR_NODESIZE
    std::cout << std::endl;
  }
}
#endif

/***********************************************************************/
//! Constructor
/*!
  \param pdbf
*/
xbNdx::xbNdx() : xbIndex()
{
}

/***********************************************************************/
//! Constructor
/*!
  \param pdbf
*/
xbNdx::xbNdx(xbDbf *pdbf) : xbIndex(pdbf) {
#ifndef XB_VAR_NODESIZE
  memset( Node, 0x00, XB_NDX_NODE_SIZE );
#else
  memset( Node, 0x00, XB_MAX_NDX_NODE_SIZE );
#endif
  memset( &HeadNode, 0x00, sizeof( xbNdxHeadNode ));
  NodeChain       = NULL;
  FreeNodeChain   = NULL;
  DeleteChain     = NULL;
  CurNode         = NULL;
  xbNodeLinkCtr     = 0L;
  ReusedxbNodeLinks = 0L;
#ifndef XB_VAR_NODESIZE
  NodeSize = XB_NDX_NODE_SIZE;
#else
  NodeSize = XB_DEFAULT_NDX_NODE_SIZE;
#endif // XB_VAR_NODESIZE
}

/***********************************************************************/
//! Destructor
/*!
*/
xbNdx::~xbNdx()
{
  CloseIndex();
}

/***********************************************************************/
//! Short description
/*!
*/
xbShort xbNdx::GetHeadNode( void )
{
  char *p, *q;
  xbShort i;

  if( !IsOpen() )
    return XB_NOT_OPEN;

  if( _fseek( indexfp, 0, SEEK_SET ))
    return XB_SEEK_ERROR;

  if(( fread( Node, XB_NDX_NODE_SIZE, 1, indexfp )) != 1 )
    return XB_READ_ERROR;

  /* load the head node structure */
  p = Node;
  HeadNode.StartNode   = dbf->xbase->GetLong ( p ); p+=4;
  HeadNode.TotalNodes  = dbf->xbase->GetLong ( p ); p+=4;
  HeadNode.NoOfKeys    = dbf->xbase->GetLong ( p ); p+=4;
  HeadNode.KeyLen      = dbf->xbase->GetShort( p ); p+=2;
  HeadNode.KeysPerNode = dbf->xbase->GetShort( p ); p+=2;
  HeadNode.KeyType     = dbf->xbase->GetShort( p ); p+=2;
  HeadNode.KeySize     = dbf->xbase->GetLong ( p ); p+=4;
  HeadNode.Unknown2    = *p++;
  HeadNode.Unique      = *p++;
  
#ifdef XB_VAR_NODESIZE
   //
   //  Automagically determine the node size.  Note the (2 * sizeof(xbLong))
   //  is taken directly from CreateIndex().  I don't understand it exactly,
   //  but this is the value used to calculate the number of keys per node.
   //  DTB.
   //
  NodeSize = (2 * sizeof(xbLong)) + HeadNode.KeySize * HeadNode.KeysPerNode;
  if(NodeSize % XB_NDX_NODE_MULTIPLE)
    NodeSize = ((NodeSize + XB_NDX_NODE_MULTIPLE) / XB_NDX_NODE_MULTIPLE) * 
                 XB_NDX_NODE_MULTIPLE;
#endif

  q = HeadNode.KeyExpression;
  for( i = XB_NDX_NODE_BASESIZE; i < XB_NDX_NODE_SIZE && *p; i++ )
    *q++ = *p++;

  return 0;
}
/***********************************************************************/
//! Short description
/*!
  \param NodeNo
  \param SetNodeChain
*/
/* This routine reads a leaf node from disk                            */
/*                                                                     */
/*  If SetNodeChain 2, then the node is not appended to the node chain */
/*                     but the CurNode pointer points to the node read */
/*  If SetNodeChain 1, then the node is appended to the node chain     */
/*  If SetNodeChain 0, then record is only read to Node memory         */

xbShort xbNdx::GetLeafNode( xbLong NodeNo, xbShort SetNodeChain )
{
  xbNdxNodeLink *n;

  if( !IsOpen() )
    return XB_NOT_OPEN;

  if( _fseek( indexfp, (xbOffT)NodeNo * XB_NDX_NODE_SIZE, SEEK_SET ))
    return XB_SEEK_ERROR;

  if(( fread( Node, XB_NDX_NODE_SIZE, 1, indexfp )) != 1 )
    return XB_READ_ERROR;

  if( !SetNodeChain ) return 0;

  if(( n = GetNodeMemory()) == NULL )
    return XB_NO_MEMORY;

  n->NodeNo = NodeNo;
  n->CurKeyNo = 0L;
  n->NextNode = NULL;
  n->Leaf.NoOfKeysThisNode = dbf->xbase->GetLong( Node );
  memcpy( n->Leaf.KeyRecs, Node+4, XB_NDX_NODE_SIZE - 4 );

  /* put the node in the chain */
  if( SetNodeChain == 1 ){
    if( NodeChain == NULL ){      /* first one ? */
      NodeChain = n;
      CurNode = n;
      CurNode->PrevNode = NULL;
    } else {
      n->PrevNode = CurNode;
      CurNode->NextNode = n;
      CurNode = n;
    }
  }
  else
    CurNode = n;
  return 0;
}
/***********************************************************************/
//! Short description
/*!
  \param n
*/
#ifdef XBASE_DEBUG
void xbNdx::DumpNodeRec( xbLong n )
{
  char *p;
  xbLong NoOfKeys, LeftBranch, RecNo, NodeType;
  xbShort i,j;
  FILE * log;

  if(( log = fopen( "xbase64.log", "a+t" )) == NULL ) return;
  GetLeafNode( n, 0 );
  NoOfKeys = dbf->xbase->GetLong( Node );
  p = Node + 4;        /* go past no of keys */
 
  fprintf( log, "----------------------------------------------------\n" );
  fprintf( log, "Node # %ld\n", n );
  fprintf( log, "Number of keys = %ld\n", NoOfKeys );
  fprintf( log, " Key      Left     Dbf Rec    Key\n" );
  fprintf( log, "Number   Branch     Number  Data\n" );

  NodeType = 0;
  for( i = 0; i < (NoOfKeys+NodeType); i++ ){
    LeftBranch = dbf->xbase->GetLong( p );
    if( i == 0 && LeftBranch ){
      NodeType = 1;        /* print one extra entry for interior nodes */
      fprintf( log, "Interior node\n" );
    }

    p+=4;
    RecNo = dbf->xbase->GetLong( p );
    p+=4;

    fprintf( log, " %3d  %9ld  %9ld  ", i, LeftBranch, RecNo );

    if( NodeType == 1 && i == NoOfKeys )
      fprintf( log, "...\n" );

    else if( !HeadNode.KeyType ){
      for( j = 0; j < HeadNode.KeyLen; j++ ) 
        fputc( *p++, log );
      fputc( '\n', log );
    }

    else {
      fprintf( log, "??????\n" /*, dbf->xbase->GetDouble( p )*/  );
      p += 8;
    }
  }
  fclose( log );
}
#endif
/***********************************************************************/
#ifndef XB_INLINE_GETDBFNO
xbLong xbNdx::GetDbfNo( xbShort RecNo, xbNdxNodeLink * n )
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
/***********************************************************************/
//! Short description
/*!
  \param RecNo
  \param n
*/
xbLong xbNdx::GetLeftNodeNo( xbShort RecNo, xbNdxNodeLink * n )
{
  xbNdxLeafNode *temp;
  char *p;
  if( !n ) return 0L;
  temp = &n->Leaf;
  if( RecNo < 0 || RecNo > temp->NoOfKeysThisNode ) return 0L;
  p = temp->KeyRecs;
  p += RecNo * ( 8 + HeadNode.KeyLen );
  return( dbf->xbase->GetLong( p ));
}
/***********************************************************************/
//! Short description
/*!
  \param RecNo
  \param n
*/
char * xbNdx::GetKeyData( xbShort RecNo, xbNdxNodeLink * n )
{
  xbNdxLeafNode *temp;
  char *p;
  if( !n ) return 0L;
  temp = &n->Leaf;
  if( RecNo < 0 || RecNo > ( temp->NoOfKeysThisNode - 1 )) return 0L;
  p = temp->KeyRecs + 8;
  p += RecNo * ( 8 + HeadNode.KeyLen );
  return( p );
}
/***********************************************************************/
//! Short description
/*!
*/
xbLong xbNdx::GetTotalNodes( void ) 
{
  if( &HeadNode )
    return HeadNode.TotalNodes;
  else
    return 0L;
}
/***********************************************************************/
//! Short description
/*!
*/
xbUShort xbNdx::GetKeysPerNode( void ) 
{
  if( &HeadNode )
    return HeadNode.KeysPerNode;
  else
    return 0L;
}
/***********************************************************************/
//! Short description
/*!
  \param RetrieveSw
*/
xbShort xbNdx::GetFirstKey( xbShort RetrieveSw )
{
/* This routine returns 0 on success and sets CurDbfRec to the record  */
/* corresponding to the first index pointer                            */

  xbLong TempNodeNo;
  xbShort rc;
   
   /* initialize the node chain */
  if( NodeChain ){
    ReleaseNodeMemory( NodeChain );
    NodeChain = NULL;
  }

  if(( rc = GetHeadNode()) != 0 ){
    CurDbfRec = 0L;
    return rc;
  }

  /* get a node and add it to the link */
  if(( rc = GetLeafNode( HeadNode.StartNode, 1 )) != 0 ){
    return rc;
  }      

/* traverse down the left side of the tree */
  while( GetLeftNodeNo( 0, CurNode )){
    TempNodeNo = GetLeftNodeNo( 0, CurNode );     
    if(( rc = GetLeafNode( TempNodeNo, 1 )) != 0 ){
      CurDbfRec = 0L;
      return rc;     
    }
    CurNode->CurKeyNo = 0;
  }
  CurDbfRec = GetDbfNo( 0, CurNode );
  if( RetrieveSw )
    return dbf->GetRecord( CurDbfRec );
  else
    return XB_NO_ERROR;
}
/***********************************************************************/
//! Short description
/*!
  \param RetrieveSw
*/
xbShort xbNdx::GetNextKey( xbShort RetrieveSw )
{
/* This routine returns 0 on success and sets CurDbfRec to the record  */
/* corresponding to the next index pointer                             */

  xbNdxNodeLink * TempxbNodeLink;
  xbLong TempNodeNo;
  xbShort rc;
    
  if( !IsOpen() ){
    CurDbfRec = 0L;
    return XB_NOT_OPEN;
  }
  
  if( !CurNode ){
    rc = GetFirstKey( RetrieveSw );
    return rc;
  }

  /* more keys on this node ? */
  if(( CurNode->Leaf.NoOfKeysThisNode-1) > CurNode->CurKeyNo ){
    CurNode->CurKeyNo++;
    CurDbfRec = GetDbfNo( CurNode->CurKeyNo, CurNode );
    if( RetrieveSw )
      return dbf->GetRecord( CurDbfRec );
    else
      return XB_NO_ERROR;
  }

   /* if head node we are at eof */
  if( CurNode->NodeNo == HeadNode.StartNode ) {
    return XB_EOF;
  }

   /* this logic assumes that interior nodes have n+1 left node no's where */
   /* n is the number of keys in the node                                  */

   /* pop up one node to the interior node level & free the leaf node      */

  TempxbNodeLink = CurNode;
  CurNode = CurNode->PrevNode;
  CurNode->NextNode = NULL;
  ReleaseNodeMemory( TempxbNodeLink );

   /* while no more right keys && not head node, pop up one node */
  while(( CurNode->CurKeyNo >= CurNode->Leaf.NoOfKeysThisNode ) &&
          ( CurNode->NodeNo != HeadNode.StartNode )){
    TempxbNodeLink = CurNode;
    CurNode = CurNode->PrevNode;
    CurNode->NextNode = NULL;
    ReleaseNodeMemory( TempxbNodeLink );
  }

  /* if head node && right most key, return end-of-file */
  if(( HeadNode.StartNode == CurNode->NodeNo ) &&
      ( CurNode->CurKeyNo >= CurNode->Leaf.NoOfKeysThisNode )) {
     return XB_EOF;
  }

   /* move one to the right */
  CurNode->CurKeyNo++;
  TempNodeNo = GetLeftNodeNo( CurNode->CurKeyNo, CurNode );

  if(( rc = GetLeafNode( TempNodeNo, 1 )) != 0 ){
    return rc;
  }

/* traverse down the left side of the tree */
  while( GetLeftNodeNo( 0, CurNode )){
    TempNodeNo = GetLeftNodeNo( 0, CurNode );     
    if(( rc = GetLeafNode( TempNodeNo, 1 )) != 0 ){
      CurDbfRec = 0L;
      return rc;
    }
    CurNode->CurKeyNo = 0;
  }
  CurDbfRec = GetDbfNo( 0, CurNode );
  if( RetrieveSw )
    return dbf->GetRecord( CurDbfRec );
  else
    return XB_NO_ERROR;
}
/***********************************************************************/
//! Short description
/*!
  \param NodeNo
  \param RetrieveSw
*/
xbShort xbNdx::GetLastKey( xbLong NodeNo, xbShort RetrieveSw )
{
/* This routine returns 0 on success and sets CurDbfRec to the record  */
/* corresponding to the last index pointer                             */

/* If NodeNo = 0, start at head node, otherwise start at NodeNo        */

  xbLong TempNodeNo;
  xbShort rc;
  
  if( NodeNo < 0 || NodeNo > HeadNode.TotalNodes )
    return XB_INVALID_NODE_NO;

  /* initialize the node chain */
  if( NodeChain ){
    ReleaseNodeMemory( NodeChain );
    NodeChain = NULL;
  }
  if( NodeNo == 0L )
    if(( rc = GetHeadNode()) != 0 ){ 
      CurDbfRec = 0L;
      return rc;
    }

   /* get a node and add it to the link */
  if( NodeNo == 0L ){
    if(( rc = GetLeafNode( HeadNode.StartNode, 1 )) != 0 ){
      CurDbfRec = 0L;
      return rc;
    }
  } else {
    if(( rc = GetLeafNode( NodeNo, 1 )) != 0 ) {
      CurDbfRec = 0L;
      return rc;
    }
  }
  CurNode->CurKeyNo = CurNode->Leaf.NoOfKeysThisNode;

/* traverse down the right side of the tree */
  while( GetLeftNodeNo( CurNode->Leaf.NoOfKeysThisNode, CurNode )){
    TempNodeNo = GetLeftNodeNo( CurNode->Leaf.NoOfKeysThisNode, CurNode );     
    if(( rc = GetLeafNode( TempNodeNo, 1 )) != 0 ){
      CurDbfRec = 0L;
      return rc;
    }
    CurNode->CurKeyNo = CurNode->Leaf.NoOfKeysThisNode;
  }
  CurNode->CurKeyNo--;           /* leaf node has one fewer ix recs */
  CurDbfRec = GetDbfNo( CurNode->Leaf.NoOfKeysThisNode-1, CurNode );
  if( RetrieveSw )
    return dbf->GetRecord( CurDbfRec );
  else
    return XB_NO_ERROR;
}
/***********************************************************************/
//! Short description
/*!
  \param RetrieveSw
*/
xbShort xbNdx::GetPrevKey( xbShort RetrieveSw )
{
/* This routine returns 0 on success and sets CurDbfRec to the record  */
/* corresponding to the previous index pointer                         */

  xbNdxNodeLink * TempxbNodeLink;
  xbLong TempNodeNo;
  xbShort rc;
    
  if( !IsOpen() ){
    CurDbfRec = 0L;
    return XB_NOT_OPEN;
  }

  if( !CurNode ){
    CurDbfRec = 0L;
    return GetFirstKey( RetrieveSw );
  }

   /* more keys on this node ? */
  if( CurNode->CurKeyNo > 0 ){
    CurNode->CurKeyNo--;
    CurDbfRec = GetDbfNo( CurNode->CurKeyNo, CurNode );
    if( RetrieveSw )
      return dbf->GetRecord( CurDbfRec );
    else
      return XB_NO_ERROR;
  }

  /* this logic assumes that interior nodes have n+1 left node no's where */
  /* n is the number of keys in the node                                  */
  /* pop up one node to the interior node level & free the leaf node      */

  if( !CurNode->PrevNode ) {      /* michael - make sure prev node exists */
    return XB_EOF;
  }

  TempxbNodeLink = CurNode;
  CurNode = CurNode->PrevNode;
  CurNode->NextNode = NULL;
  ReleaseNodeMemory( TempxbNodeLink );

  /* while no more left keys && not head node, pop up one node */
  while(( CurNode->CurKeyNo == 0 ) && 
          ( CurNode->NodeNo != HeadNode.StartNode )) {
    TempxbNodeLink = CurNode;
    CurNode = CurNode->PrevNode;
    CurNode->NextNode = NULL;
    ReleaseNodeMemory( TempxbNodeLink );
  }

   /* if head node && left most key, return beginning-of-file */
  if(( HeadNode.StartNode == CurNode->NodeNo ) &&
      ( CurNode->CurKeyNo == 0 )) {
    return XB_BOF;
  }

   /* move one to the left */
  CurNode->CurKeyNo--;
  TempNodeNo = GetLeftNodeNo( CurNode->CurKeyNo, CurNode );

  if(( rc = GetLeafNode( TempNodeNo, 1 )) != 0 ) {
    return rc;
  }

  if( GetLeftNodeNo( 0, CurNode )) /* if interior node */
    CurNode->CurKeyNo = CurNode->Leaf.NoOfKeysThisNode;
  else              /* leaf node */
    CurNode->CurKeyNo = CurNode->Leaf.NoOfKeysThisNode - 1;

/* traverse down the right side of the tree */
  while( GetLeftNodeNo( 0, CurNode )){    /* while interior node */
    TempNodeNo = GetLeftNodeNo( CurNode->Leaf.NoOfKeysThisNode, CurNode );
    if(( rc = GetLeafNode( TempNodeNo, 1 )) != 0 ){
      CurDbfRec = 0L;
      return rc;
    }
    if( GetLeftNodeNo( 0, CurNode )) /* if interior node */
      CurNode->CurKeyNo = CurNode->Leaf.NoOfKeysThisNode;
    else              /* leaf node */
      CurNode->CurKeyNo = CurNode->Leaf.NoOfKeysThisNode - 1;
  }
  CurDbfRec = GetDbfNo( CurNode->Leaf.NoOfKeysThisNode - 1, CurNode );
  if( RetrieveSw )
    return dbf->GetRecord( CurDbfRec );
  else
    return XB_NO_ERROR;
}
/**************************************************************************/
//! Short description
/*!
  \param key
  \param klen
  \param node
  \param comp
*/
/*
**  This is a pretty basic binary search with two exceptions:  1) it will
**  find the first of duplicate key values and 2) will return the index
**  and the value of the last comparision even if it doesn't find a 
**  match.
*/
xbShort
xbNdx::BSearchNode(const char *key, xbShort klen, const xbNdxNodeLink *node, 
                   xbShort *comp)
{
  xbShort c, p, start = 0, end = node->Leaf.NoOfKeysThisNode - 1;
      
  if(start > end){
    *comp = 2;
    return 0;
  }
  
  do {
    p = (start + end) / 2;
    c = CompareKey(key, GetKeyData(p, (xbNdxNodeLink *)node), klen);
    switch(c){
    
      case 1 : /* greater than */
        start = p + 1;
      break;

      case 2 : /* less than */
        end = p - 1;
      break;
    }
  } while(start <= end && c);

  
  if(c == 1)
    while(p < node->Leaf.NoOfKeysThisNode &&
          (c = CompareKey(key, GetKeyData(p, (xbNdxNodeLink *)node), klen)) == 1)
      p++;
    
  *comp = c;
  
  if(!c)
    while(p > 0 && !CompareKey(key, GetKeyData(p - 1, (xbNdxNodeLink *)node), klen))
      p--;
    
  return p;
}
/***********************************************************************/
//! Short description
/*!
  \param Tkey
  \param Klen
*/
xbLong xbNdx::GetLeafFromInteriorNode( const char * Tkey, xbShort Klen )
{
  /* This function scans an interior node for a key and returns the   */
  /* correct interior leaf node no                                    */

  xbShort p, c;

   /* if Tkey > any keys in node, return right most key */
  p = CurNode->Leaf.NoOfKeysThisNode - 1;
  if( CompareKey( Tkey, GetKeyData( p, CurNode ), Klen ) == 1 ) {
    CurNode->CurKeyNo = CurNode->Leaf.NoOfKeysThisNode;
    return GetLeftNodeNo( CurNode->Leaf.NoOfKeysThisNode, CurNode );
  }

  p = BSearchNode(Tkey, Klen, CurNode, &c);
  CurNode->CurKeyNo = p;
  return GetLeftNodeNo( p, CurNode );
}
/***********************************************************************/
//! Short description
/*!
  \param d
*/
xbShort xbNdx::KeyExists( xbDouble d )
{
  char buf[9];
  memset( buf, 0x00, 9 );
  dbf->xbase->PutDouble( buf, d );
  return FindKey( buf, 8, 0 );
}
/***********************************************************************/
//! Short description
/*!
  \param d
*/
xbShort xbNdx::FindKey( xbDouble d )
{
  char buf[9];
  memset( buf, 0x00, 9 );
  dbf->xbase->PutDouble( buf, d );
  return FindKey( buf, 8, 1 );
}
/***********************************************************************/
//! Short description
/*!
  \param Key
*/
xbShort xbNdx::FindKey( const char * Key )
{
  return FindKey( Key, strlen( Key ), 1 );
}
/***********************************************************************/
//! Short description
/*!
  \param Tkey
  \param DbfRec
*/
xbShort xbNdx::FindKey( const char * Tkey, xbLong DbfRec )
{
  /* find a key with a specifc DBF record number */
  xbShort rc;

  xbLong CurDbfRecNo;
   xbLong CurNdxDbfNo;

  /* if we are already on the correct key, return XB_FOUND */
  if( CurNode ) {
    CurDbfRecNo = dbf->GetCurRecNo();
    CurNdxDbfNo = GetDbfNo( CurNode->CurKeyNo, CurNode );
    if( CurDbfRecNo == CurNdxDbfNo && 
      (strncmp(Tkey, GetKeyData( CurNode->CurKeyNo, CurNode ),
         HeadNode.KeyLen ) == 0 )) {
          return XB_FOUND;
    }
  }
  rc =  FindKey( Tkey, HeadNode.KeyLen, 0 );

  while( rc == 0 || rc == XB_FOUND ) {
    if( strncmp( Tkey, GetKeyData( CurNode->CurKeyNo, CurNode ),
        HeadNode.KeyLen ) == 0 ){
      if( DbfRec == GetDbfNo( CurNode->CurKeyNo, CurNode )) {
        return XB_FOUND;
      }
      else
        rc = GetNextKey( 0 );
    } else {
       return XB_NOT_FOUND;
     }
   }
  return XB_NOT_FOUND;
}
/***********************************************************************/
//! Short description
/*!
*/
xbShort xbNdx::FindKey( void )
{
   /* if no paramaters given, use KeyBuf */
  return( FindKey( KeyBuf, HeadNode.KeyLen, 0 ));
}
/***********************************************************************/
//! Short description
/*!
  \param Tkey
  \param Klen
  \param RetrieveSw
*/
xbShort xbNdx::FindKey( const char * Tkey, xbShort Klen, xbShort RetrieveSw )
{
  /* This routine sets the current key to the found key */
  /* if RetrieveSw is true, the method positions the dbf record */
  xbShort rc,i;
  xbLong TempNodeNo;

  if( NodeChain ) {
    ReleaseNodeMemory( NodeChain );
    NodeChain = NULL;
  }

  if(( rc = GetHeadNode()) != 0 ){
    CurDbfRec = 0L;
    return rc;
  }

   /* load first node */
  if(( rc = GetLeafNode( HeadNode.StartNode, 1 )) != 0 ){
    CurDbfRec = 0L;
    return rc;
  }

  /* traverse down the tree until it hits a leaf */
  while( GetLeftNodeNo( 0, CurNode )){ /* while interior node */
    TempNodeNo = GetLeafFromInteriorNode( Tkey, Klen );
    if(( rc = GetLeafNode( TempNodeNo, 1 )) != 0 ){
      CurDbfRec = 0L;
      return rc;
    }
  }

  i = BSearchNode(Tkey, Klen, CurNode, &rc);
  switch(rc) {
    case 0 : /* found! */
      CurNode->CurKeyNo = i;
      CurDbfRec = GetDbfNo( i, CurNode );
      if( RetrieveSw )
        dbf->GetRecord(CurDbfRec);
        return XB_FOUND;
     
    case 1 : /* less than */
//       if(i < CurNode->Leaf.NoOfKeysThisNode)
      break;
//       i++;
     
    case 2 : /* greater than */
      CurNode->CurKeyNo = i;
      CurDbfRec = GetDbfNo( i, CurNode );
      if( RetrieveSw )
        dbf->GetRecord(CurDbfRec);
      return XB_NOT_FOUND;
   }

   CurNode->CurKeyNo = i;
   if(i >= CurNode->Leaf.NoOfKeysThisNode){
     CurDbfRec = 0;
     return XB_EOF;
   }
   
  CurDbfRec = GetDbfNo( i, CurNode );
  if((RetrieveSw) && (CurDbfRec > 0))
    dbf->GetRecord( CurDbfRec );
   
  return XB_NOT_FOUND;
}
/***********************************************************************/
//! Short description
/*!
*/
xbShort xbNdx::CalcKeyLen()
{
  xbShort rc;
  xbExpNode * TempNode;
  char FieldName[11];
  char Type;

  TempNode = IxExp->GetFirstTreeNode();

  if( !TempNode )
    return 0;

  if( TempNode->Type == 'd' ) return -8;
  if( TempNode->Type == 'D' ){
    memset( FieldName, 0x00, 11 );
    memcpy( FieldName, TempNode->NodeText, TempNode->Len );
    Type = dbf->GetFieldType( dbf->GetFieldNo( FieldName ));
    if( Type == 'N' || Type == 'F' )
      return -8;
  }

  if(( rc = IxExp->ProcessExpression()) != XB_NO_ERROR )
    return 0;
      
  TempNode = (xbExpNode *) IxExp->Pop();
  if( !TempNode )
    return 0;
  rc = TempNode->DataLen;

  if( !TempNode->InTree )
    delete TempNode;

   return rc;
}
/***********************************************************************/
//! Short description
/*!
  \param IxName
  \param Exp
  \param Unique
  \param Overlay
*/
xbShort xbNdx::CreateIndex(const char * IxName, const char * Exp,
         xbShort Unique, xbShort Overlay )
{
  xbShort i, KeyLen, rc;

  if( IsOpen()) CloseIndex();
  if( strlen( Exp ) > 488 )
    return XB_INVALID_KEY_EXPRESSION;

  if( dbf->GetDbfStatus() == 0 )
    return XB_NOT_OPEN;

  /* Get the index file name and store it in the class */
  SetFileName(IxName);

  /* check if the file already exists */
  if (((indexfp = fopen( GetFileName(), "r" )) != NULL ) && !Overlay ) {
    fclose( indexfp );
    return XB_FILE_EXISTS;
  }

  if (indexfp)
    fclose(indexfp);

  if(( indexfp = fopen( GetFileName(), "w+b" )) == NULL )
    return XB_OPEN_ERROR;

#ifdef XB_LOCKING_ON
   /*
   **  Must turn off buffering when multiple programs may be accessing
   **  index files.
   */
   setbuf( indexfp, NULL );
#endif   

  /* parse the expression */
  IxExp = new xbExpn( dbf->xbase );
  if(( rc = IxExp->BuildExpressionTree( Exp, strlen( Exp ), dbf )) != XB_NO_ERROR )
    return rc;

  /* build the header record */
  memset( &HeadNode, 0x00, sizeof( xbNdxHeadNode ));
  HeadNode.StartNode  = 1L;
  HeadNode.TotalNodes = 2L;
  HeadNode.NoOfKeys   = 1L;
  KeyLen = CalcKeyLen();

  if( KeyLen == 0 || KeyLen > 100 )       /* 100 byte key length limit */
    return XB_INVALID_KEY;
  else if( KeyLen == -8 ){ 
    HeadNode.KeyType = 1;                /* numeric key */
    HeadNode.KeyLen = 8; 
  } else { 
    HeadNode.KeyType = 0;                /* character key */
    HeadNode.KeyLen = KeyLen; 
  }

//   HeadNode.KeysPerNode = (xbUShort) ( XB_NDX_NODE_SIZE - (2*sizeof( xbLong ))) /
//      (HeadNode.KeyLen + 8 );
//   HeadNode.KeySize = HeadNode.KeyLen + 8;
//   while(( HeadNode.KeySize % 4 ) != 0 ) HeadNode.KeySize++;  /* multiple of 4*/

/* above code replaced with following by Paul Koufalis pkoufalis@cogicom.com */
//   while(( HeadNode.KeyLen % 4 ) != 0 ) HeadNode.KeyLen++;  /* multiple of 4*/
//   HeadNode.KeySize = HeadNode.KeyLen + 8;


/* above two lines commented out by gary 4/14/99 and replaced w/ following 
    For compatibility with other Xbase tools
      KeyLen is the length of the key data
      KeySize = KeyLen+8, rounded up until divisible by 4
*/

  HeadNode.KeySize = HeadNode.KeyLen + 8;
  while(( HeadNode.KeySize % 4 ) != 0 ) HeadNode.KeySize++;  /* multiple of 4*/

  HeadNode.KeysPerNode = (xbUShort)
    (XB_NDX_NODE_SIZE - (2*sizeof( xbLong ))) / HeadNode.KeySize;

  HeadNode.Unique = Unique;
  strncpy( HeadNode.KeyExpression, Exp, 488 );
  KeyBuf  = (char *) malloc( HeadNode.KeyLen + 1 ); 
  KeyBuf2 = (char *) malloc( HeadNode.KeyLen + 1 ); 
  memset( KeyBuf,  0x00, HeadNode.KeyLen + 1 );
  memset( KeyBuf2, 0x00, HeadNode.KeyLen + 1 );

  if(( rc = PutHeadNode( &HeadNode, indexfp, 0 )) != 0 ){
    return rc;
  }
   /* write node #1 all 0x00 */
  for( i = 0; i < XB_NDX_NODE_SIZE; i++ ){
    if ((fwrite("\x00", 1, 1, indexfp)) != 1){
      fclose( indexfp );
      return XB_WRITE_ERROR;
    }
  }
//  IndexStatus = XB_OPEN;
  return dbf->AddIndexToIxList( index, GetFileName() );  
}
/***********************************************************************/
//! Short description
/*!
  \param RecNo
  \param n
  \param NodeNo
*/
xbShort xbNdx::PutLeftNodeNo( xbShort RecNo, xbNdxNodeLink *n, xbLong NodeNo )
{
   /* This routine sets n node's leftnode number */
  xbNdxLeafNode *temp;
  char *p;
  if( !n )
    return XB_INVALID_NODELINK;

  temp = &n->Leaf;
  if( RecNo < 0 || RecNo > HeadNode.KeysPerNode)
    return XB_INVALID_KEY;

  p = temp->KeyRecs;
  p+= RecNo * ( 8 + HeadNode.KeyLen );
  dbf->xbase->PutLong( p, NodeNo );
  return XB_NO_ERROR;
}
/***********************************************************************/
//! Short description
/*!
  \param RecNo
  \param n
  \param DbfNo
*/
xbShort xbNdx::PutDbfNo( xbShort RecNo, xbNdxNodeLink *n, xbLong DbfNo )
{
  /* This routine sets n node's dbf number */
  xbNdxLeafNode *temp;
  char *p;
  if( !n )
    return XB_INVALID_NODELINK;

  temp = &n->Leaf;
  if( RecNo < 0 || RecNo > (HeadNode.KeysPerNode-1))
    return XB_INVALID_KEY;

  p = temp->KeyRecs + 4;
  p+= RecNo * ( 8 + HeadNode.KeyLen );
  dbf->xbase->PutLong( p, DbfNo );
  return XB_NO_ERROR;
}
/************************************************************************/
//! Short description
/*!
  \param l
  \param n
*/
xbShort xbNdx::PutLeafNode( xbLong l, xbNdxNodeLink *n )
{
  if ((_fseek(indexfp, (xbOffT)l * XB_NDX_NODE_SIZE , SEEK_SET)) != 0) {
    fclose( indexfp );
    return XB_SEEK_ERROR;
  }
  dbf->xbase->PutLong( Node, n->Leaf.NoOfKeysThisNode );

  if(( fwrite( Node, 4, 1, indexfp )) != 1 ){
    fclose( indexfp );
    return XB_WRITE_ERROR;
  }
  if(( fwrite( &n->Leaf.KeyRecs, XB_NDX_NODE_SIZE-4, 1, indexfp )) != 1 ){
    fclose( indexfp );
    return XB_WRITE_ERROR;
  }
  return 0;   
}
/************************************************************************/
//! Short description
/*!
  \param Head
  \param f
  \param UpdateOnly
*/
xbShort xbNdx::PutHeadNode( xbNdxHeadNode * Head, FILE * f, xbShort UpdateOnly )
{
  char buf[4];
 
  if(( _fseek( f, 0L, SEEK_SET )) != 0 ){
    fclose( f );
    return XB_SEEK_ERROR;
  }
  memset( buf, 0x00, 4 );
  dbf->xbase->PutLong( buf, Head->StartNode );
  if(( fwrite( &buf, 4, 1, f )) != 1 ){
    fclose( f );
    return XB_WRITE_ERROR;
  }
  memset( buf, 0x00, 4 );
  dbf->xbase->PutLong( buf, Head->TotalNodes );
  if(( fwrite( &buf, 4, 1, f )) != 1 ){
    fclose( f );
    return XB_WRITE_ERROR;
  }
  memset( buf, 0x00, 4 );
  dbf->xbase->PutLong( buf, Head->NoOfKeys );
  if(( fwrite( &buf, 4, 1, f )) != 1 ){
    fclose( f );
    return XB_WRITE_ERROR;
  }
  if( UpdateOnly )
    return XB_NO_ERROR;
  memset( buf, 0x00, 2 );
  dbf->xbase->PutLong( buf, Head->KeyLen );
  if(( fwrite( &buf, 2, 1, f )) != 1 ){
    fclose( f );
    return XB_WRITE_ERROR;
  }
  memset( buf, 0x00, 2 );
  dbf->xbase->PutLong( buf, Head->KeysPerNode );
  if(( fwrite( &buf, 2, 1, f )) != 1 ){
    fclose( f );
    return XB_WRITE_ERROR;
  }
  memset( buf, 0x00, 2 );
  dbf->xbase->PutLong( buf, Head->KeyType );
  if(( fwrite( &buf, 2, 1, f )) != 1 ){
    fclose( f );
    return XB_WRITE_ERROR;
  }
  memset( buf, 0x00, 4 );
  dbf->xbase->PutLong( buf, Head->KeySize );
  if(( fwrite( &buf, 4, 1, f )) != 1 ){
    fclose( f );
    return XB_WRITE_ERROR;
  }
  if(( fwrite( &Head->Unknown2, XB_NDX_NODE_SIZE - 22, 1, f )) != 1 ){
    fclose( f );
    return XB_WRITE_ERROR;
  }
  return 0;   
}
/************************************************************************/
//! Short description
/*!
  \param RecNo
  \param n
*/
xbShort xbNdx::PutKeyData( xbShort RecNo, xbNdxNodeLink *n )
{
   /* This routine copies the KeyBuf data into xbNdxNodeLink n */
  xbNdxLeafNode *temp;
  char *p;
  xbShort i;
  if( !n )
    return XB_INVALID_NODELINK;

  temp = &n->Leaf;
  if( RecNo < 0 || RecNo > (HeadNode.KeysPerNode-1))
    return XB_INVALID_KEY;

  p = temp->KeyRecs + 8;
  p+= RecNo * ( 8 + HeadNode.KeyLen );
  for( i = 0; i < HeadNode.KeyLen; i++ ) {
    *p = KeyBuf[i];
    p++;
  }
  return XB_NO_ERROR;
}
/************************************************************************/
//! Short description
/*!
  \param n
  \param pos
  \param d
  \param l
  \param w
*/
xbShort xbNdx::PutKeyInNode( xbNdxNodeLink * n, xbShort pos, xbLong d, 
    xbLong l, xbShort w )
{
  xbShort i;

   /* check the node */
  if (!n)
    return XB_INVALID_NODELINK;

  if(pos < 0 || pos > HeadNode.KeysPerNode)
    return XB_INVALID_RECORD;

  if(n->Leaf.NoOfKeysThisNode >= HeadNode.KeysPerNode)
    return XB_NODE_FULL;

  /* if key movement, save the original key */
  if( pos < n->Leaf.NoOfKeysThisNode )
    memcpy( KeyBuf2, KeyBuf, HeadNode.KeyLen + 1);

  /* if interior node, handle the right most left node no */
  if( GetLeftNodeNo( 0, n ))
    PutLeftNodeNo( n->Leaf.NoOfKeysThisNode+1, n,
     GetLeftNodeNo( n->Leaf.NoOfKeysThisNode, n ));

  for( i = n->Leaf.NoOfKeysThisNode; i > pos; i-- ){
    memcpy( KeyBuf, GetKeyData(i-1,n), HeadNode.KeyLen );  
    PutKeyData( i, n );
    PutDbfNo( i, n, GetDbfNo(i-1,n));
    PutLeftNodeNo(i, n, GetLeftNodeNo(i-1,n));
  }
  
  /* put new key in node */
  if( pos < n->Leaf.NoOfKeysThisNode )
    memcpy( KeyBuf, KeyBuf2, HeadNode.KeyLen + 1);

  PutKeyData( pos, n );
  PutDbfNo( pos, n, d );
  PutLeftNodeNo( pos, n, l );
  n->Leaf.NoOfKeysThisNode++;
  if( w )
    return PutLeafNode( n->NodeNo, n );
  else
    return 0;
}
/************************************************************************/
//! Short description
/*!
  \param curNode  Current Node
  \param newNode  New Empty Node
  \param pos      Position of new key in current node
  \param d        dbf record number
*/
/* This function splits a full index leaf node into two parts
   as of 2/13/04, this split logic was modified to cause an
   even split in order to keep the index tree balanced 

*/

xbShort xbNdx::SplitLeafNode( xbNdxNodeLink *curNode, 
    xbNdxNodeLink *newNode, xbShort pos, xbLong d )
{
  xbShort curNodeNewSize;
  xbShort newNodeSize;
  xbShort i,j,rc,startPos;

  curNodeNewSize = (curNode->Leaf.NoOfKeysThisNode + 1) / 2;
  newNodeSize    = curNode->Leaf.NoOfKeysThisNode + 1 - curNodeNewSize;

  /* save off the current key buffer */
  memcpy( KeyBuf2, KeyBuf, HeadNode.KeyLen + 1 );
  if( pos < curNodeNewSize ){   /* new key goes in current node */

    /* copy second half of current node to beginning of new node */
    /* memcpy( dst, src, len ); */
    startPos = curNode->Leaf.NoOfKeysThisNode - newNodeSize;

    for( i = startPos, j = 0; i < CurNode->Leaf.NoOfKeysThisNode; i++, j++){
      memcpy( KeyBuf, GetKeyData( i, curNode ), HeadNode.KeyLen );
      PutKeyData( j, newNode );
      PutDbfNo( j, newNode, GetDbfNo( i, curNode ));
    }

    /* make a hole for the new key */
    for( i = curNodeNewSize - 1; i > pos; i-- ){
      memcpy( KeyBuf, GetKeyData( i-1, curNode ), HeadNode.KeyLen );
      PutKeyData( i, curNode );
      PutDbfNo( i, curNode, GetDbfNo( i-1, curNode ));
    }

    /* insert key appropriately */
    memcpy( KeyBuf, KeyBuf2, HeadNode.KeyLen + 1 );
    PutKeyData( pos, curNode );
    PutDbfNo( pos, curNode, d );
  }
  else
  {
    pos -= curNodeNewSize;

    /* do part one of the key migration */
    if( pos ){

/*  was originally

      startPos = curNode->Leaf.NoOfKeysThisNode - curNodeNewSize + 1;


    then was changed to 
*/

/*
      if( ((pos + curNodeNewSize) == HeadNode.KeysPerNode) &&
           (pos == newNodeSize) ){  // off the right end
        startPos = curNode->Leaf.NoOfKeysThisNode - curNodeNewSize;
      }
      else
      {
        startPos = curNode->Leaf.NoOfKeysThisNode - curNodeNewSize + 1;
      }

*/

/*
    and this didn't work


      startPos = curNode->Leaf.NoOfKeysThisNode - curNodeNewSize;
  
*/

      startPos = curNodeNewSize;
      for( i = startPos, j = 0; 
           j < pos && i < curNode->Leaf.NoOfKeysThisNode; i++, j++){
        memcpy( KeyBuf, GetKeyData( i, curNode ), HeadNode.KeyLen );
        PutKeyData( j, newNode );
        PutDbfNo( j, newNode, GetDbfNo( i, curNode ));
      }
    }

    /* insert new key appropriately */
    memcpy( KeyBuf, KeyBuf2, HeadNode.KeyLen + 1 );
    PutKeyData( pos, newNode );
    PutDbfNo( pos, newNode, d );

    /* Load the remainder of the keys on the new node past the new key */
    if( pos < (newNodeSize-1) ){

//      startPos = curNode->Leaf.NoOfKeysThisNode - curNodeNewSize + pos + 1;

      startPos = curNodeNewSize + pos;
      for( i = startPos, j = pos+1; j < newNodeSize; i++, j++){
        memcpy( KeyBuf, GetKeyData( i, curNode ), HeadNode.KeyLen );
        PutKeyData( j, newNode );
        PutDbfNo( j, newNode, GetDbfNo( i, curNode ));
      }
    }
  }

  curNode->Leaf.NoOfKeysThisNode = curNodeNewSize;
  newNode->Leaf.NoOfKeysThisNode = newNodeSize;

  /* write the new nodes to disk */  
  if(( rc = PutLeafNode( curNode->NodeNo, curNode )) != 0 )
    return rc;
  if(( rc = PutLeafNode( newNode->NodeNo, newNode )) != 0 )
    return rc;

  return 0;
}
/************************************************************************/
//! Short description
/*!
  \param nodeToSplit   Interior node to split
  \param newNode       New empty node to use
  \param dscNodeNo     Descendant node number
*/
/* This routine splits an interior node */

xbShort xbNdx::SplitINode( xbNdxNodeLink *nodeToSplit, 
        xbNdxNodeLink *newNode, xbLong dscNodeNo )
{
  xbShort i,j,rc;
  xbNdxNodeLink * SaveNodeChain;
  xbNdxNodeLink * SaveCurNode;
  xbLong newNodeToSplitSize;
  xbLong newNodeSize;
  xbShort pos, startPos, offset;

  newNodeToSplitSize = (nodeToSplit->Leaf.NoOfKeysThisNode + 2 ) / 2;
  newNodeSize = nodeToSplit->Leaf.NoOfKeysThisNode + 2 - newNodeToSplitSize;
  pos = nodeToSplit->CurKeyNo;

  if( pos < (newNodeToSplitSize-1) ){
    /* copy second half of nodeToSplit to newNode */
    startPos = nodeToSplit->Leaf.NoOfKeysThisNode - newNodeSize +1;
    for(i=startPos, j=0; i <= nodeToSplit->Leaf.NoOfKeysThisNode; i++, j++ ){
      if( i < nodeToSplit->Leaf.NoOfKeysThisNode ){
        memcpy( KeyBuf, GetKeyData( i, nodeToSplit ), HeadNode.KeyLen );
        PutKeyData( j, newNode );
      }
      PutLeftNodeNo( j, newNode, GetLeftNodeNo( i, nodeToSplit ));
    }

    /* make a hole for the new key */
    for( i = newNodeToSplitSize; i > pos; i-- ){
      memcpy( KeyBuf, GetKeyData( i-1, nodeToSplit ), HeadNode.KeyLen );
      PutKeyData( i, nodeToSplit );
      PutLeftNodeNo( i, nodeToSplit, GetLeftNodeNo( i-1, nodeToSplit ));
    }

    /* load new high key value into current position on nodeToSplit */
    if( pos < (newNodeToSplitSize - 1 )){
      SaveNodeChain = NodeChain;
      NodeChain = NULL;
      SaveCurNode = CurNode;
      GetLastKey( GetLeftNodeNo( pos, nodeToSplit ), 0 );
      memcpy( KeyBuf, GetKeyData( CurNode->CurKeyNo, CurNode ), HeadNode.KeyLen );
      PutKeyData( pos, nodeToSplit );
      ReleaseNodeMemory( NodeChain );
      NodeChain = SaveNodeChain;
      CurNode = SaveCurNode;
    }
    PutLeftNodeNo( pos+1, nodeToSplit, dscNodeNo );
  }

/*************/
/*   part b  */
/*************/

  else
  {
    pos -= newNodeToSplitSize-1;
    /* do part one of the key migration */
    if( pos ){
//      startPos = nodeToSplit->Leaf.NoOfKeysThisNode - newNodeToSplitSize + 2;
  
// 5/29/04 gak changed the following line for index probs
//      startPos = nodeToSplit->Leaf.NoOfKeysThisNode - newNodeToSplitSize + 1;

      if( HeadNode.KeysPerNode % 2 )
        offset = 2;
      else
        offset = 1;

      startPos = nodeToSplit->Leaf.NoOfKeysThisNode - newNodeToSplitSize + offset;

      for( i = startPos, j = 0; j < pos; i++, j++ ){
        if( i < nodeToSplit->Leaf.NoOfKeysThisNode && j < (pos-1)){
          memcpy( KeyBuf, GetKeyData( i, nodeToSplit ), HeadNode.KeyLen );
          PutKeyData( j, newNode );
        }
        else
        {
          SaveNodeChain = NodeChain;
          NodeChain = NULL;
          SaveCurNode = CurNode;
          GetLastKey( GetLeftNodeNo( i, nodeToSplit ), 0 );
          memcpy(KeyBuf,GetKeyData(CurNode->CurKeyNo,CurNode),HeadNode.KeyLen);
          PutKeyData( j, newNode );
          ReleaseNodeMemory( NodeChain );
          NodeChain = SaveNodeChain;
          CurNode = SaveCurNode;
        }
        PutLeftNodeNo( j, newNode, GetLeftNodeNo( i, nodeToSplit ));
      }
    }

    /* insert new key appropriately */
    if( pos < (newNodeSize - 1)){
      SaveNodeChain = NodeChain;
      NodeChain = NULL;
      SaveCurNode = CurNode;
      GetLastKey( dscNodeNo, 0 );
      memcpy(KeyBuf,GetKeyData(CurNode->CurKeyNo,CurNode),HeadNode.KeyLen);
      PutKeyData( pos, newNode );
      ReleaseNodeMemory( NodeChain );
      NodeChain = SaveNodeChain;
      CurNode = SaveCurNode;
    }
    PutLeftNodeNo( pos, newNode, dscNodeNo );

    /* load remainder of the keys */
    if( pos < (newNodeSize - 1)){


// startPos=nodeToSplit->Leaf.NoOfKeysThisNode-newNodeToSplitSize+pos+2;
// 5/29/04 gak changed the following line for index probs

      startPos=nodeToSplit->Leaf.NoOfKeysThisNode-newNodeToSplitSize+pos+offset;

      for( i = startPos, j = pos+1; j < newNodeSize; i++, j++ ){
        if( i < nodeToSplit->Leaf.NoOfKeysThisNode ){
          memcpy( KeyBuf, GetKeyData( i, nodeToSplit ), HeadNode.KeyLen );
          PutKeyData( j, newNode );
        }
        PutLeftNodeNo( j, newNode, GetLeftNodeNo( i, nodeToSplit ));
      }
    }
  }

  nodeToSplit->Leaf.NoOfKeysThisNode = newNodeToSplitSize - 1;
  newNode->Leaf.NoOfKeysThisNode = newNodeSize - 1;

  if((rc = PutLeafNode( nodeToSplit->NodeNo, nodeToSplit )) != 0) return rc;
  if((rc = PutLeafNode( newNode->NodeNo, newNode )) != 0) return rc;
  return 0;
}
/************************************************************************/
//! Short description
/*!
  \param RecBufSw
  \param KeyBufSw
*/
xbShort xbNdx::CreateKey( xbShort RecBufSw, xbShort KeyBufSw )
{ 
   /* RecBufSw   0   Use RecBuf    */
   /*            1   Use RecBuf2   */
   /* KeyBufSw   0   Use KeyBuf    */
   /*            1   Use KeyBuf2   */

  xbShort rc;
  xbExpNode * TempNode;

  if(( rc = IxExp->ProcessExpression( RecBufSw )) != XB_NO_ERROR )
    return rc;
  TempNode = (xbExpNode *) IxExp->Pop();
  if( !TempNode )
    return XB_INVALID_KEY;

  if( KeyBufSw ){
    if( HeadNode.KeyType == 1 )    /* numeric key   */
      dbf->xbase->PutDouble( KeyBuf2, TempNode->DoubResult );
    else{                           /* character key */
      memset( KeyBuf2, 0x00, HeadNode.KeyLen + 1 );
       memcpy( KeyBuf2, TempNode->StringResult, XB_MIN(HeadNode.KeyLen + 1, TempNode->DataLen) );
     }
  } else {
    if( HeadNode.KeyType == 1 )    /* numeric key   */
      dbf->xbase->PutDouble( KeyBuf, TempNode->DoubResult );
    else {                           /* character key */
      memset( KeyBuf, 0x00, HeadNode.KeyLen + 1 );
      memcpy( KeyBuf, TempNode->StringResult.c_str(), XB_MIN(HeadNode.KeyLen + 1, TempNode->DataLen) );
    }
  }
//   if( !TempNode->InTree ) dbf->xbase->FreeExpNode( TempNode );
  if( !TempNode->InTree ) delete TempNode;
  return 0;
}
/************************************************************************/
//! Short description
/*!
  \param key
*/
xbShort
xbNdx::GetCurrentKey(char *key)
{
  CreateKey(0, 0);
  if(HeadNode.KeyType == 1)
    memcpy(key, KeyBuf, 8);
  else
    memcpy(key, KeyBuf, HeadNode.KeyLen + 1);
  return 0;
}
/************************************************************************/
//! Short description
/*!
  \param DbfRec
*/
xbShort xbNdx::AddKey( xbLong DbfRec )
{
 /* This routine assumes KeyBuf contains the contents of the index to key */
   
  char *p;
  xbShort i,rc;
  xbNdxNodeLink * TempNode;   
  xbNdxNodeLink * Tparent;
  xbLong TempNodeNo;              /* new, unattached leaf node no */
  xbNdxNodeLink * SaveNodeChain;
  xbNdxNodeLink * SaveCurNode;

  /* find node key belongs in */
  rc = FindKey( KeyBuf, HeadNode.KeyLen, 0 );
  if( rc == XB_FOUND && HeadNode.Unique )
    return XB_KEY_NOT_UNIQUE;

  if( CurNode->Leaf.NoOfKeysThisNode > 0 && rc == XB_FOUND ){
    rc = 0;
    while( rc == 0 ){
      if(( p = GetKeyData( CurNode->CurKeyNo, CurNode )) == NULL )  
        rc = -1;
      else {
        rc = CompareKey( KeyBuf, p, HeadNode.KeyLen );
        if( rc == 0 && DbfRec >= GetDbfNo( CurNode->CurKeyNo, CurNode )){
          if((rc = GetNextKey(0)) == XB_EOF) {
            if((rc = GetLastKey(0, 0)) != XB_NO_ERROR)
              return rc;
            CurNode->CurKeyNo++;
          }
        }
        else 
          rc = -1;
      }
    }
  }

  /* update header node */
  HeadNode.NoOfKeys++;
  /************************************************/
  /* section A - if room in node, add key to node */
  /************************************************/

  if( CurNode->Leaf.NoOfKeysThisNode < HeadNode.KeysPerNode ){
    if(( rc = PutKeyInNode( CurNode,CurNode->CurKeyNo,DbfRec,0L,1)) != 0)
      return rc;
     if(( rc = PutHeadNode( &HeadNode, indexfp, 1 )) != 0) 
       return rc;
     return XB_NO_ERROR;
  }   

  /***********************************************************************/
  /* section B - split leaf node if full and put key in correct position */
  /***********************************************************************/

  TempNode = GetNodeMemory();
  TempNode->NodeNo = HeadNode.TotalNodes++;
  rc = SplitLeafNode( CurNode, TempNode, CurNode->CurKeyNo, DbfRec );
  if( rc ) 
    return rc;

  TempNodeNo = TempNode->NodeNo;
  ReleaseNodeMemory( TempNode );

  /*****************************************************/
  /* section C go up tree splitting nodes as necessary */
  /*****************************************************/
  Tparent = CurNode->PrevNode;

  while( Tparent && 
      Tparent->Leaf.NoOfKeysThisNode >= HeadNode.KeysPerNode-1) {

    TempNode = GetNodeMemory();

    if( !TempNode ) 
      return XB_NO_MEMORY;
    TempNode->NodeNo = HeadNode.TotalNodes++;      

    rc = SplitINode( Tparent, TempNode, TempNodeNo );
    if( rc ) return rc;

    TempNodeNo = TempNode->NodeNo;
    ReleaseNodeMemory( TempNode );
    ReleaseNodeMemory( CurNode );
    CurNode = Tparent;
    CurNode->NextNode = NULL;
    Tparent = CurNode->PrevNode;
  }

  /************************************************************/
  /* Section D  if CurNode is split root, create new root     */
  /************************************************************/

  /* at this point
   CurNode = The node that was just split
   TempNodeNo = The new node split off from CurNode */

  if(CurNode->NodeNo == HeadNode.StartNode ){
    TempNode = GetNodeMemory();
    if( !TempNode ) 
      return XB_NO_MEMORY;
      
    SaveNodeChain = NodeChain;
    NodeChain = NULL;
    SaveCurNode = CurNode;
    GetLastKey( CurNode->NodeNo, 0 );
    memcpy( KeyBuf, GetKeyData( CurNode->CurKeyNo,CurNode ),HeadNode.KeyLen );
    ReleaseNodeMemory( NodeChain );
    NodeChain = SaveNodeChain;
    CurNode = SaveCurNode;
    PutKeyData( 0, TempNode );
    PutLeftNodeNo( 0, TempNode, CurNode->NodeNo );
    PutLeftNodeNo( 1, TempNode, TempNodeNo );
    TempNode->NodeNo = HeadNode.TotalNodes++;
    TempNode->Leaf.NoOfKeysThisNode++;
    HeadNode.StartNode = TempNode->NodeNo;
    rc = PutLeafNode( TempNode->NodeNo, TempNode );
    if( rc ) return rc;
    rc = PutHeadNode( &HeadNode, indexfp, 1 );
    if( rc ) return rc;
    ReleaseNodeMemory( TempNode );
    return XB_NO_ERROR;
  }
  /**********************************/
  /* Section E  make room in parent */
  /**********************************/
  for( i = Tparent->Leaf.NoOfKeysThisNode; i > Tparent->CurKeyNo; i-- ){
    memcpy( KeyBuf, GetKeyData( i-1, Tparent ), HeadNode.KeyLen );
    PutKeyData( i, Tparent );
    PutLeftNodeNo( i+1, Tparent, GetLeftNodeNo( i, Tparent ));
  }

  /* put key in parent */
  SaveNodeChain = NodeChain;
  NodeChain = NULL;
  SaveCurNode = CurNode;
  GetLastKey( CurNode->NodeNo, 0 );
  memcpy( KeyBuf,GetKeyData( CurNode->CurKeyNo, CurNode ), HeadNode.KeyLen );
  ReleaseNodeMemory( NodeChain );
  NodeChain = SaveNodeChain;
  CurNode = SaveCurNode;
  PutKeyData( i, Tparent );
  PutLeftNodeNo( i+1, Tparent, TempNodeNo );
  Tparent->Leaf.NoOfKeysThisNode++;
  rc = PutLeafNode( Tparent->NodeNo, Tparent );
  if( rc ) return rc;
  rc = PutHeadNode( &HeadNode, indexfp, 1 );
  if( rc ) return rc;
  return XB_NO_ERROR;
}
/***********************************************************************/
//! Short description
/*!
  \param pos
  \param n
*/
xbShort xbNdx::RemoveKeyFromNode( xbShort pos, xbNdxNodeLink *n )
{
  xbShort i;
  /* check the node */
  if( !n )
    return XB_INVALID_NODELINK;

  if( pos < 0 || pos > HeadNode.KeysPerNode )
    return XB_INVALID_KEY;

  for( i = pos; i < n->Leaf.NoOfKeysThisNode-1; i++ ){
    memcpy( KeyBuf, GetKeyData( i+1, n), HeadNode.KeyLen );
    PutKeyData( i, n );
    PutDbfNo( i, n, GetDbfNo( i+1, n ));
    PutLeftNodeNo( i, n, GetLeftNodeNo( i+1, n ));
  }   
  PutLeftNodeNo( i, n, GetLeftNodeNo( i+1, n ));
  n->Leaf.NoOfKeysThisNode--;
  /* if last key was deleted, decrement CurKeyNo */
  if( n->CurKeyNo > n->Leaf.NoOfKeysThisNode )
    n->CurKeyNo--;
  return PutLeafNode( n->NodeNo, n );
}   
/***********************************************************************/
//! Short description
/*!
  \param n
*/
xbShort xbNdx::UpdateParentKey( xbNdxNodeLink * n )
{
/* this routine goes backwards thru the node chain looking for a parent
   node to update */

  xbNdxNodeLink * TempNode;
  if( !n )
    return XB_INVALID_NODELINK;
    
  if( !GetDbfNo( 0, n ))
    return XB_NOT_LEAFNODE;

  TempNode = n->PrevNode;
  while( TempNode ){
    if( TempNode->CurKeyNo < TempNode->Leaf.NoOfKeysThisNode ){
      memcpy(KeyBuf,GetKeyData(n->Leaf.NoOfKeysThisNode-1,n),HeadNode.KeyLen);
       PutKeyData( TempNode->CurKeyNo, TempNode );
       return PutLeafNode( TempNode->NodeNo, TempNode );
    }
    TempNode = TempNode->PrevNode;
  }
  return XB_NO_ERROR;
}
/***********************************************************************/
//! Short description
/*!
  \param n
*/
/* This routine queues up a list of nodes which have been emptied      */
void xbNdx::UpdateDeleteList( xbNdxNodeLink *n )
{
  n->NextNode = DeleteChain;
  DeleteChain = n;
}
/***********************************************************************/
//! Short description
/*!
*/
/* Delete nodes from the node list - for now we leave the empty nodes  */
/* dangling in the file. Eventually we will remove nodes from the file */

void xbNdx::ProcessDeleteList( void )
{
  if( DeleteChain ){
    ReleaseNodeMemory( DeleteChain );
    DeleteChain = NULL;
  }
}
/***********************************************************************/
//! Short description
/*!
*/
xbShort xbNdx::KeyWasChanged( void )
{
  CreateKey( 0, 0 );            /* use KeyBuf,  RecBuf    */
  CreateKey( 1, 1 );            /* use KeyBuf2, RecBuf2   */
  if( CompareKey( KeyBuf, KeyBuf2, HeadNode.KeyLen ) != 0 )
    return 1;
  else
    return 0;
}
/***********************************************************************/
//! Short description
/*!
  \param n
*/
xbNdxNodeLink * xbNdx::LeftSiblingHasSpace( xbNdxNodeLink * n )
{
  xbNdxNodeLink * TempNode;
  xbNdxNodeLink * SaveCurNode;

  /* returns a Nodelink to xbNdxNodeLink n's left sibling if it has space */
  /* if left most node in parent return NULL */
  if( n->PrevNode->CurKeyNo == 0 )  
    return NULL;

  SaveCurNode = CurNode;
  GetLeafNode( GetLeftNodeNo( n->PrevNode->CurKeyNo-1, n->PrevNode ), 2 ); 
  if( CurNode->Leaf.NoOfKeysThisNode < HeadNode.KeysPerNode ){
    TempNode = CurNode;  
    CurNode = SaveCurNode;
    TempNode->PrevNode = n->PrevNode;
    return TempNode;
  } else {  /* node is already full */
    ReleaseNodeMemory( CurNode );
    CurNode = SaveCurNode;
    return NULL;
  }
}
/***********************************************************************/
//! Short description
/*!
  \param n
*/
xbNdxNodeLink * xbNdx::RightSiblingHasSpace( xbNdxNodeLink * n )
{
 /* returns a Nodelink to xbNdxNodeLink n's right sibling if it has space */

  xbNdxNodeLink * TempNode;
  xbNdxNodeLink * SaveCurNode;

   /* if left most node in parent return NULL */
  if( n->PrevNode->CurKeyNo >= n->PrevNode->Leaf.NoOfKeysThisNode )  
    return NULL;
  SaveCurNode = CurNode;
  /* point curnode to right sib*/
  GetLeafNode( GetLeftNodeNo( n->PrevNode->CurKeyNo+1, n->PrevNode ), 2 ); 
  if( CurNode->Leaf.NoOfKeysThisNode < HeadNode.KeysPerNode ){
    TempNode = CurNode;  
    CurNode = SaveCurNode;
    TempNode->PrevNode = n->PrevNode;
    return TempNode;
  } else { /* node is already full */
    ReleaseNodeMemory( CurNode );
    CurNode = SaveCurNode;
    return NULL;
  }
}
/*************************************************************************/
//! Short description
/*!
  \param n
  \param Right
*/
xbShort xbNdx::MoveToRightNode( xbNdxNodeLink * n, xbNdxNodeLink * Right )
{
  xbShort j;
  xbNdxNodeLink * TempNode;
  xbNdxNodeLink * SaveCurNode;
  xbNdxNodeLink * SaveNodeChain;

  if( n->CurKeyNo == 0 ){
    j = 1;
    SaveNodeChain = NodeChain;
    SaveCurNode = CurNode;
    NodeChain = NULL;
    GetLastKey( n->NodeNo, 0 ); 
    memcpy( KeyBuf, GetKeyData( CurNode->CurKeyNo, CurNode),HeadNode.KeyLen);
    ReleaseNodeMemory( NodeChain );
    NodeChain = SaveNodeChain;
    CurNode = SaveCurNode;
  } else {
    j = 0;
    memcpy( KeyBuf, GetKeyData( j, n ), HeadNode.KeyLen);
  }
  PutKeyInNode( Right, 0, 0L, GetLeftNodeNo( j, n ), 1 );
  ReleaseNodeMemory( Right );
  TempNode = n;
  CurNode = n->PrevNode;
  n = n->PrevNode;
  n->NextNode = NULL;
  UpdateDeleteList( TempNode );
  DeleteSibling( n );
  return XB_NO_ERROR;
}
/***********************************************************************/
//! Short description
/*!
  \param n
  \param Left
*/
xbShort xbNdx::MoveToLeftNode( xbNdxNodeLink * n, xbNdxNodeLink * Left )
{
  xbShort j, rc;
  xbNdxNodeLink * SaveNodeChain;
  xbNdxNodeLink * TempNode;

  if( n->CurKeyNo == 0 )
    j = 1;
  else
    j = 0;
  
  /* save the original node chain */
  SaveNodeChain = NodeChain;
  NodeChain = NULL;

  /* determine new right most key for left node */
  GetLastKey( Left->NodeNo, 0 );
  memcpy( KeyBuf, GetKeyData( CurNode->CurKeyNo, CurNode ), HeadNode.KeyLen);
  ReleaseNodeMemory( NodeChain );
  NodeChain = NULL;       /* for next GetLastKey */
  PutKeyData( Left->Leaf.NoOfKeysThisNode, Left); 
  PutLeftNodeNo( Left->Leaf.NoOfKeysThisNode+1, Left, GetLeftNodeNo( j,n ));
  Left->Leaf.NoOfKeysThisNode++;
  Left->CurKeyNo = Left->Leaf.NoOfKeysThisNode;
  if(( rc = PutLeafNode( Left->NodeNo, Left )) != 0 )
    return rc;
  n->PrevNode->NextNode = NULL;
  UpdateDeleteList( n );

  /* get the new right most key for left to update parents */
   GetLastKey( Left->NodeNo, 0 );
   
  /* assemble the chain */
  TempNode = Left->PrevNode;
  TempNode->CurKeyNo--;
  NodeChain->PrevNode = Left->PrevNode;
  UpdateParentKey( CurNode );
  ReleaseNodeMemory( NodeChain );
  ReleaseNodeMemory( Left );
  CurNode = TempNode;
  NodeChain = SaveNodeChain;
  TempNode->CurKeyNo++;    
  DeleteSibling( TempNode );
  return XB_NO_ERROR;
}
/***********************************************************************/
//! Short description
/*!
  \param n
*/
xbShort xbNdx::DeleteSibling( xbNdxNodeLink * n )
{
  xbNdxNodeLink * Left;
  xbNdxNodeLink * Right;
  xbNdxNodeLink * SaveCurNode;
  xbNdxNodeLink * SaveNodeChain;
  xbNdxNodeLink * TempNode;
  xbShort  rc;

  /* this routine deletes sibling CurRecNo out of xbNodeLink n */
  if( n->Leaf.NoOfKeysThisNode > 1 ){  
    RemoveKeyFromNode( n->CurKeyNo, n );
    if( n->CurKeyNo == n->Leaf.NoOfKeysThisNode ){
      SaveNodeChain = NodeChain;
      SaveCurNode = CurNode;
      NodeChain = NULL;
      GetLastKey( n->NodeNo, 0 );
      /* assemble the node chain */
      TempNode = NodeChain->NextNode;
      NodeChain->NextNode = NULL;
      ReleaseNodeMemory( NodeChain );
      TempNode->PrevNode = n;
      UpdateParentKey( CurNode );
      /* take it back apart */
      ReleaseNodeMemory( TempNode );
      NodeChain = SaveNodeChain;
      CurNode = SaveCurNode;
    }
  } else if( n->NodeNo == HeadNode.StartNode ) {
    /* get here if root node and only one child remains */
    /* make remaining node the new root */
    if( n->CurKeyNo == 0 )   
      HeadNode.StartNode = GetLeftNodeNo( 1, n );
    else
      HeadNode.StartNode = GetLeftNodeNo( 0, n );
    UpdateDeleteList( n );
    NodeChain = NULL;
    CurNode = NULL;
  } 
  else if (( Left = LeftSiblingHasSpace( n )) != NULL )
    return MoveToLeftNode( n, Left );
  else if (( Right = RightSiblingHasSpace( n )) != NULL )
    return MoveToRightNode( n, Right );
  /* else if left sibling exists */   
  else if( n->PrevNode->CurKeyNo > 0 ) {
    /* move right branch from left sibling to this node */  
    SaveCurNode = CurNode;
    SaveNodeChain = NodeChain;
    NodeChain = NULL;
    GetLeafNode( GetLeftNodeNo( n->PrevNode->CurKeyNo-1, n->PrevNode ), 2 );
    Left = CurNode;
    Left->PrevNode = SaveCurNode->PrevNode;
    GetLastKey( Left->NodeNo, 0 );
    strncpy( KeyBuf, GetKeyData( CurNode->CurKeyNo,CurNode),HeadNode.KeyLen );
    if( n->CurKeyNo == 1 )
      PutLeftNodeNo( 1, n, GetLeftNodeNo( 0, n ));
    PutKeyData( 0, n );
    PutLeftNodeNo( 0, n, GetLeftNodeNo( Left->Leaf.NoOfKeysThisNode, Left ));
    if(( rc = PutLeafNode( n->NodeNo, n )) != XB_NO_ERROR ) return rc;
    SaveCurNode = n->PrevNode;
    SaveCurNode->NextNode = NULL;
    ReleaseNodeMemory( n );
    Left->Leaf.NoOfKeysThisNode--;
    if(( rc = PutLeafNode( Left->NodeNo, Left )) != XB_NO_ERROR ) return rc;
    /* rebuild left side of tree */
    GetLastKey( Left->NodeNo, 0 );
    NodeChain->PrevNode = SaveCurNode;
    SaveCurNode->CurKeyNo--;
    UpdateParentKey( CurNode );
    ReleaseNodeMemory( NodeChain );
    ReleaseNodeMemory( Left );
    CurNode = SaveCurNode;
    NodeChain = SaveNodeChain;
  }
  /* right sibling must exist */
  else if( n->PrevNode->CurKeyNo <= n->PrevNode->Leaf.NoOfKeysThisNode ){
    /* move left branch from left sibling to this node */   
    SaveCurNode = CurNode;
    SaveNodeChain = NodeChain;
    NodeChain = NULL;

    /* move the left node number one to the left if necessary */
    if( n->CurKeyNo == 0 ){
      PutLeftNodeNo( 0, n, GetLeftNodeNo( 1, n ));
      GetLastKey( GetLeftNodeNo( 0, n ), 0 );
      memcpy(KeyBuf,GetKeyData(CurNode->CurKeyNo,CurNode),HeadNode.KeyLen);
      PutKeyData( 0, n );
      ReleaseNodeMemory( NodeChain );
      NodeChain = NULL;
    }
    GetLeafNode( GetLeftNodeNo( n->PrevNode->CurKeyNo+1, n->PrevNode ), 2 );
    /* put leftmost node number from right node in this node */
    PutLeftNodeNo( 1, n, GetLeftNodeNo( 0, CurNode ));      
    if(( rc = PutLeafNode( n->NodeNo, n )) != XB_NO_ERROR ) return rc;

    /* remove the key from the right node */
    RemoveKeyFromNode( 0, CurNode );
    if(( rc = PutLeafNode( CurNode->NodeNo, CurNode )) != XB_NO_ERROR ) 
      return rc;
    ReleaseNodeMemory( CurNode );

    /* update new parent key value */
    GetLastKey( n->NodeNo, 0 );
    NodeChain->PrevNode = n->PrevNode;
    UpdateParentKey( CurNode );
    ReleaseNodeMemory( NodeChain );
    NodeChain = SaveNodeChain;
    CurNode = SaveCurNode;
  } else {
    /* this should never be true-but could be if 100 byte limit is ignored*/
    std::cout << "Fatal index error" << std::endl;
    exit(0);
  }
  return XB_NO_ERROR;   
}
/***********************************************************************/
//! Short description
/*!
  \param DbfRec
*/
xbShort xbNdx::DeleteKey( xbLong DbfRec )
{
/* this routine assumes the key to be deleted is in KeyBuf */

  xbNdxNodeLink * TempNode;
  xbShort rc;

#if 0
   //  Not sure why this check is here, but it prevents numeric keys
   //  from being deleted (and thus index updates will also fail).
   //  I have removed it for now.  Derry Bryson
   if( HeadNode.KeyType != 0x00 )
     xb_error(XB_INVALID_KEY_TYPE);
#endif     

  if(( rc = FindKey( KeyBuf, DbfRec )) != XB_FOUND )
    return rc;

  /* found the record to delete at this point */
  HeadNode.NoOfKeys--;

  /* delete the key from the node                                    */
  if(( rc = RemoveKeyFromNode( CurNode->CurKeyNo, CurNode )) != 0 )
    return rc;

  /* if root node, we are done */
  if( !( CurNode->NodeNo == HeadNode.StartNode )){
    /* if leaf node now empty */
    if( CurNode->Leaf.NoOfKeysThisNode == 0 ){
      TempNode = CurNode->PrevNode;
      TempNode->NextNode = NULL;
      UpdateDeleteList( CurNode );
      CurNode = TempNode;
      DeleteSibling( CurNode );
      ProcessDeleteList();
    }

    /* if last key of leaf updated, update key in parent node */
    /* this logic updates the correct parent key              */

    else if( CurNode->CurKeyNo == CurNode->Leaf.NoOfKeysThisNode )
      UpdateParentKey( CurNode );
  }
   
  if(CurNode)
    CurDbfRec = GetDbfNo( CurNode->CurKeyNo, CurNode );
  else
    CurDbfRec = 0;

  if(( rc = PutHeadNode( &HeadNode, indexfp, 1 )) != 0 )
    return rc;
  return XB_NO_ERROR;
}
/************************************************************************/
//! Short description
/*!
  \param option
*/
#ifdef XBASE_DEBUG
xbShort xbNdx::CheckIndexIntegrity( const xbShort option )
{
  /* if option = 1, print out some stats */
  xbShort rc;
  xbLong ctr = 1L;
  while( ctr <= dbf->NoOfRecords()){
    if( option ) std::cout << "Checking Record " << ctr << std::endl;
    if(( rc = dbf->GetRecord(ctr++)) != XB_NO_ERROR )
      return rc;
    if(!dbf->RecordDeleted()){
      CreateKey( 0, 0 );
      rc = FindKey( KeyBuf, dbf->GetCurRecNo());
      if( rc != XB_FOUND ){
        if( option ){
          std::cout << "Record number " << dbf->GetCurRecNo()
                    <<  " Not Found"    << std::endl;
          std::cout << "Key = " << KeyBuf << std::endl;
        }
        return rc;
      }
    }
  }
  if( option )
    std::cout << std::endl << "Total records checked = " 
              << ctr - 1 << std::endl;
  return XB_NO_ERROR;
}
#endif
/***********************************************************************/
//! Short description
/*!
  \param statusFunc
*/
xbShort xbNdx::ReIndex(void (*statusFunc)(xbLong itemNum, xbLong numItems))
{
  /* this method assumes the index has been locked in exclusive mode */
  xbLong l;
  xbShort rc, i, saveAutoLock;
  xbNdxHeadNode TempHead;
  FILE *t;
  xbString TempName;
  memcpy( &TempHead, &HeadNode, sizeof( struct xbNdxHeadNode ));
  TempHead.NoOfKeys = 1L;
  TempHead.TotalNodes = 2L;
  TempHead.StartNode = 1L;
  rc = dbf->xbase->DirectoryExistsInName( GetFileName() );
  if( rc ){
    TempName.assign(GetFileName(), 0, rc);
    TempName += "TEMPFILE.NDX";
  } else
    TempName = "TEMPFILE.NDX";
 
  if(( t = fopen( TempName, "w+b" )) == NULL )
    return XB_OPEN_ERROR;

  if(( rc = PutHeadNode( &TempHead, t, 0 )) != 0 ){
    fclose( t );
    remove(TempName);
    return rc;
  }

  for( i = 0; i < XB_NDX_NODE_SIZE; i++ ){
    if(( fwrite( "\x00", 1, 1, t )) != 1 ){
      fclose( t );
      remove(TempName);
      return XB_WRITE_ERROR;
    }
  }
  if( fclose( indexfp ) != 0 )
    return XB_CLOSE_ERROR;
  if( fclose( t ) != 0 )
    return XB_CLOSE_ERROR;
  if( remove( GetFileName() ) != 0 )
    return XB_CLOSE_ERROR;
  if( rename(TempName, GetFileName() ) != 0 )
    return XB_WRITE_ERROR;
  if(( indexfp = fopen( GetFileName(), "r+b" )) == NULL )
    return XB_OPEN_ERROR;

  saveAutoLock = dbf->GetAutoLock();
  dbf->AutoLockOff();
  for( l = 1; l <= dbf->PhysicalNoOfRecords(); l++ ){
    if(statusFunc && (l == 1 || !(l % 100) || l == dbf->PhysicalNoOfRecords()))
       statusFunc(l, dbf->PhysicalNoOfRecords());
    if(( rc = dbf->GetRecord(l)) != XB_NO_ERROR ){
      if(saveAutoLock)
        dbf->AutoLockOn();
       return rc;
    }

    if(!dbf->GetRealDelete() || !dbf->RecordDeleted()){
      /* Create the key */
      CreateKey( 0, 0 );
      /* add key to index */
      if(( rc = AddKey( l )) != XB_NO_ERROR ){
        if(saveAutoLock)
          dbf->AutoLockOn();
         return rc;
      }
    } 
  }
  return rc;
}

/***********************************************************************/
//! Short description
/*!
  \param size
*/
void xbNdx::SetNodeSize(xbShort size)
{
#ifdef XB_VAR_NODESIZE
  if(size >= XB_DEFAULT_NDX_NODE_SIZE)
  {
    if(size % XB_NDX_NODE_MULTIPLE)
      NodeSize = ((size + XB_NDX_NODE_MULTIPLE) / XB_NDX_NODE_MULTIPLE) *
                 XB_NDX_NODE_MULTIPLE;
    else
      NodeSize = size;
  }
  else
    NodeSize = XB_DEFAULT_NDX_NODE_SIZE;
#endif
}

/***********************************************************************/
//! Short description
/*!
  \param buf
  \param len
*/
void xbNdx::GetExpression(char *buf, int len)
{
  memcpy(buf, HeadNode.KeyExpression, 
         len < XB_NDX_NODE_SIZE ? len : XB_NDX_NODE_SIZE - XB_NDX_NODE_BASESIZE);
}

const char* xbNdx::GetExtWithDot(bool lower)
{
  return lower? ".ndx": ".NDX";
}

xbUShort xbNdx::GetKeyLen()
{
  return HeadNode.KeyLen;
}

const char* xbNdx::GetKeyExpression()
{
  return HeadNode.KeyExpression;
}

void xbNdx::FreeNodesMemory()
{
  ReleaseNodeMemory(NodeChain, true);
  NodeChain = 0;
//  ReleaseNodeMemory(CloneChain, true);
//  CloneChain = 0;
  ReleaseNodeMemory(FreeNodeChain, true);
  FreeNodeChain = 0;
  ReleaseNodeMemory(DeleteChain, true);
  DeleteChain = 0;
}

#endif     /* XB_INDEX_NDX */
