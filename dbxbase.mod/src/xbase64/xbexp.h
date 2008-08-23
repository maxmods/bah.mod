/*  xbexp.h

    Xbase64 project source code 

    This file contains a header file for the EXP object, which is
    used for expression processing.

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

#ifndef __XB_EXP_H__
#define __XB_EXP_H__

#ifdef __GNU_LesserG__
#pragma interface
#endif

#include <xbase64/xbase64.h>

#ifdef XB_EXPRESSIONS             /* compile if expression logic on */
#include <xbase64/xbtypes.h>

/*! \file xbexp.h
*/

#undef ABS
#undef MIN
#undef MAX

class XBDLLEXPORT xbDbf;

/************************************************************************/
//! xbFuncDtl struct
/*!  This structure defines function information
*/

struct XBDLLEXPORT xbFuncDtl {
  const   char * FuncName;          /* function name               */
  xbShort ParmCnt;                  /* no of parms it needs        */
  char    ReturnType;               /* return type of function     */
  void    (*ExpFuncPtr)();          /* pointer to function routine */
};

/************************************************************************/
//! xbExpNode struct
/*! This class defines a node within a tree of nodes, each token
    in an expression gets placed onto its own node
*/

class XBDLLEXPORT xbExpNode {
  public:
   xbExpNode();
   virtual ~xbExpNode();

  public:
   char      * NodeText;      /* expression text                 */
   char      Type;            /* same as TokenType below         */
   xbShort   Len;             /* length of expression text       */
   xbShort   InTree;          /* this node in the tree? 1=yes    */
   xbExpNode * Node;          /* pointer to parent               */
   xbExpNode * Sibling1;      /* pointer to sibling 1            */
   xbExpNode * Sibling2;      /* pointer to sibling 2            */
   xbExpNode * Sibling3;      /* pointe/r to sibling 3           */
   xbShort   DataLen;         /* length of data in result buffer */
   xbShort   ResultLen;       /* length of result buffer         */
   xbString  StringResult;    /* string result                   */
   xbDouble  DoubResult;      /* Numeric Result                  */
   xbShort   IntResult;       /* logical result                  */   
   xbDbf *   dbf;             /* pointer to datafile             */
   xbShort   FieldNo;         /* field no if DBF field           */
   char      ExpressionType;  /* used in head node C,N,L or D    */
};
/************************************************************************/
//! xbStackElement class
/*!
*/

class XBDLLEXPORT xbStackElement
{
  public:
   xbStackElement();
   ~xbStackElement();

  friend class xbExpn;

  private:
   xbStackElement *Previous;
   xbStackElement *Next;
   xbExpNode      *NodePtr;
};
/************************************************************************/
//! xbExpn class
/*! This class is used for processing expressions
*/

/* Expression handler */

class XBDLLEXPORT xbExpn{
  public:
   xbExpn( xbXBase * );
   virtual ~xbExpn();

   xbShort GetNextToken( const char *s, xbShort MaxLen );
   xbShort ProcessExpression( xbExpNode *n, xbShort );
   xbShort ProcessExpression(  xbShort opt )
            { return ProcessExpression( Tree, opt ); }

   xbExpNode * GetTree() { return Tree; }
   void SetTreeToNull() { Tree = NULL; }
   xbExpNode * GetFirstTreeNode( xbExpNode * );
   xbExpNode * GetFirstTreeNode()
               { return GetFirstTreeNode( Tree ); }
   xbShort   ProcessExpression( const char *exp, xbDbf * d );
   xbShort   ParseExpression( const char *exp, xbDbf * d );
   xbExpNode * GetExpressionHandle();
   char      GetExpressionResultType( xbExpNode * );
   char      GetExpressionResultType() 
              { return GetExpressionResultType( Tree ); }
   char *    GetCharResult();
   xbString  & GetStringResult();
   xbDouble  GetDoubleResult();
   xbLong    GetIntResult();
   xbShort   ProcessExpression( xbExpNode * );
   xbShort   ProcessExpression() { return ProcessExpression( Tree ); }
   xbShort   BuildExpressionTree( const char * Expression, xbShort MaxTokenLen,
              xbDbf *d );

   /* stack functions */
   void      InitStack();
   xbExpNode * Pop();
   xbShort   Push(xbExpNode *);
   xbShort   GetStackDepth() { return StackDepth; }
   void      DumpStack();
   const char * GetValidFuncName( xbShort funcNo )
     { return XbaseFuncList[funcNo].FuncName; }

#ifdef XBASE_DEBUG
   void DumpExpressionTree( xbShort printOption )
         { DumpExpressionTree( Tree, printOption ); }
   void DumpExpressionTree( xbExpNode *, xbShort printOption );
   void DumpExpNode( xbExpNode *, xbShort printOption );
#endif

   /* expression methods */
   xbDouble ABS( xbDouble );
   xbLong   ASC( const char * );
   xbLong   AT( const char *, const char * );
   char *   CDOW( const char * );
   char *   CHR( xbLong );
   char *   CMONTH( const char * );
   char *   CTOD( const char * );
   char *   DATE();
   xbLong   DAY( const char * );
   char *   DESCEND( const char * );
   xbLong   DESCEND( const xbDate & );
   xbDouble DESCEND( xbDouble );
   xbLong   DOW( const char * );
   char *   DTOC( const char * );
   char *   DTOS( const char * );
   xbDouble EXP( xbDouble );
   char *   IIF( xbShort, const char *, const char * );
   xbLong   INT( xbDouble );
   xbLong   ISALPHA( const char * );
   xbLong   ISLOWER( const char * );
   xbLong   ISUPPER( const char * );
   char *   LEFT( const char *, xbShort );
   xbLong   LEN( const char * );
   xbDouble LOG( xbDouble );
   char *   LOWER( const char * );
   char *   LTRIM( const char * );
   xbDouble MAX( xbDouble, xbDouble );
   xbLong   MONTH( const char * );         /* MONTH() */
   xbDouble MIN( xbDouble, xbDouble );
   xbLong   RECNO( xbDbf * );
   char *   REPLICATE( const char *, xbShort );
   char *   RIGHT( const char *, xbShort );
   char *   RTRIM( const char * );
   char *   SPACE( xbShort );   
   xbDouble SQRT( xbDouble );
   char *   STR( const char * );
   char *   STR( const char *, xbShort );
   char *   STR( const char *, xbShort, xbShort );
   char *   STR( xbDouble );
   char *   STR( xbDouble, xbShort );
   char *   STR(xbDouble, xbUShort length, xbShort numDecimals );
   char *   STRZERO( const char * );
   char *   STRZERO( const char *, xbShort );
   char *   STRZERO( const char *, xbShort, xbShort );
   char *   STRZERO( xbDouble );
   char *   STRZERO( xbDouble, xbShort );
   char *   STRZERO( xbDouble, xbShort, xbShort );
   char *   SUBSTR( const char *, xbShort, xbShort );
   char *   TRIM( const char * );
   char *   UPPER( const char * );
   xbLong   VAL( const char * );
   xbLong   YEAR( const char * );  

 protected:
  xbShort   IsWhiteSpace( char );
  char      IsSeparator( char );
  xbExpNode * LoadExpNode( const char * ENodeText, const char EType,
             const xbShort ELen, const xbShort BufLen );
  xbShort   OperatorWeight( const char *Oper, xbShort len );
  xbShort   ReduceComplexExpression( const char * NextToken, xbShort Len,
             xbExpNode * cn, xbDbf *d );
  xbShort   GetFunctionTokenLen( const char *s );
  xbShort   ReduceFunction( const char *NextToken, xbExpNode *cn, xbDbf *d );
  xbExpNode * GetNextTreeNode( xbExpNode * );
  xbShort   ProcessOperator( xbShort );
  xbShort   ProcessFunction( char * );
  xbShort   ValidOperation( char *, char, char );
  char      GetOperandType( xbExpNode * );
  xbShort   AlphaOperation( char * );
  xbShort   NumericOperation( char * );
  xbShort   GetFuncInfo( const char *Function, xbShort Option );
  xbDouble  GetDoub( xbExpNode * );
  xbLong    GetInt( xbExpNode * );

 private:
  xbXBase   *xbase;
  xbFuncDtl *XbaseFuncList;   /* pointer to list of Xbase functions    */
  xbExpNode *Tree;            /* pointer to tree of parsed nodes       */
  xbShort   LogicalType;      /* set to 1 for logical type nodes       */
  char      TokenType;        /* E - Expression, not in simplest form  */
                              /* C - Constant                          */
                              /* N - Numeric Constant                  */
                              /* O - Operator                          */
                              /* F - Function                          */
                              /* D - Database Field                    */
                              /* s - character string result           */
                              /* l - logical or short int result       */
                              /* d - double result                     */
  char      PreviousType;     /* used to see if "-" follows operator     */
  char      * Op1;            /* pointer to operand 1                    */
  char      * Op2;            /* pointer to operand 2                    */
  xbDouble  Opd1;             /* double result 1                         */
  xbDouble  Opd2;             /* double result 2                         */
  xbShort   OpLen1;           /* length of memory allocated to operand 1 */
  xbShort   OpLen2;           /* length of memory allocated to operand 2 */
  xbShort   OpDataLen1;       /* length of data in op1                   */
  xbShort   OpDataLen2;       /* length of data in op2                   */
  char      OpType1;          /* type of operand 1                       */
  char      OpType2;          /* type of operand 2                       */
  xbShort   TokenLen;         /* length of token                         */

//   static xbString DefaultDateFormat;  /*default date format for DTOC func*/
  enum { WorkBufMaxLen = 200 };
  char  WorkBuf[WorkBufMaxLen+1];
  
  /* stack variables */
  xbShort StackDepth;
  xbStackElement *First;
  xbStackElement *Last;   
};

#endif        // XB_EXPRESSIONS
#endif        // __XB_EXP_H__


