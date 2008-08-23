/*  xbexpprc.cpp

    Xbase64 project source code

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

#ifdef XB_EXPRESSIONS

#include <ctype.h>
#include <math.h>

/*! \file xbexpprc.cpp
*/

/*************************************************************************/
//! Short description
/*!
  \param e
*/
xbExpNode * xbExpn::GetFirstTreeNode( xbExpNode * e )
{
  xbExpNode * WorkNode;
  if( !e ) return e;
  WorkNode = e;
  while( WorkNode->Sibling1 )
    WorkNode = WorkNode->Sibling1;
  return WorkNode;
}
/*************************************************************************/
//! Short description
/*!
  \param Operand
  \param Op1
  \pamam Op2
*/
xbShort xbExpn::ValidOperation( char * Operand, char Op1, char Op2 )
{
   /*  Valid operation table

       operator  Op1      Op2             operator   Op1      Op2

        **        N        N                 =        N        N
        *         N        N                 =        C        C
        /         N        N                 =        D        D     
        +         N        N                 <>,#     N        N 
        +         C        C                 <>,#     C        C 
        +         D        N                 <>,#     D        D 
        -         N        N                 <=       N        N 
        -         C        C                 <=       D        D 
        -         D        D                 <=       C        C 
        -         D        N                 >=       N        N 
        <         N        N                 >=       D        D
        <         C        C                 >=       C        C
        <         D        D                 $        C        C
        >         N        N               
        >         C        C
        >         D        D
        
       C = Character
       D = Date
       N = Numeric

    Maybe reversed to what you are thinking ==>  think OP2 - OP1

    */

  // check for **
  if( Operand[0] == '*' && Operand[1] == '*' && Op1 == 'N' && Op2 == 'N' )
    return 1;

  // check for !=
  if(( Operand[0] == '!' && Operand[1] == '=' ) && 
    (( Op1 == 'N' && Op2 == 'N' ) || 
     ( Op1 == 'C' && Op2 == 'C' ) ||
     ( Op1 == 'D' && Op2 == 'D' )))
    return 1;

  switch( Operand[0] ) {
    case '*': 
    case '/':
      if( Op1 == 'N' && Op2 == 'N' ) 
        return 1; 
      else 
        return 0;

    case '+':
      if(( Op1 == 'N' && Op2 == 'N' ) || 
         ( Op1 == 'C' && Op2 == 'C' ) ||
         ( Op1 == 'N' && Op2 == 'D' ))
        return 1;
      else
        return 0;

   case '-':
     if(( Op1 == 'N' && Op2 == 'N' ) || 
        ( Op1 == 'C' && Op2 == 'C' ) ||
        ( Op1 == 'D' && Op2 == 'D' ) ||
        ( Op1 == 'N' && Op2 == 'D' ))
       return 1;
     else
      return 0;

    case '<':
    case '>':
    case '=':
    case '#':
      if(( Op1 == 'N' && Op2 == 'N' ) || 
         ( Op1 == 'C' && Op2 == 'C' ) ||
         ( Op1 == 'D' && Op2 == 'D' ))
        return 1;
      else
        return 0;

    case '$': 
      if( Op1 == 'C' && Op2 == 'C' ) 
        return 1;
      else
        return 0;
             
      case '.' : 
        if( (strncmp( Operand, ".AND.", 5 ) == 0 ) ||
            (strncmp( Operand, ".OR.", 4 ) == 0 )  ||
            (strncmp( Operand, ".NOT.", 5 ) == 0 ))
          return 1;
        else
          return 0;

    default:
      return 0;
  }
}
/*************************************************************************/
//! Short description
/*!
  \param e
*/
xbExpNode * xbExpn::GetNextTreeNode( xbExpNode * e )
{
  if( !e->Node ) return NULL;

  /* sibling 1 && sibling 2 exists */
  if( e == e->Node->Sibling1 && e->Node->Sibling2 )
    return GetFirstTreeNode( e->Node->Sibling2 );

  /* sibling2 && sibling3 exists */
  else if( e == e->Node->Sibling2 && e->Node->Sibling3 )
    return GetFirstTreeNode( e->Node->Sibling3 );

  else
    return e->Node;
}
/*************************************************************************/
//! Short description
/*!
  \param e
*/
xbShort xbExpn::ProcessExpression( xbExpNode * e )
{
  return ProcessExpression( e, 0 );
}
/*************************************************************************/
//! Short description
/*!
  \param Wtree
  \param RecBufSw
*/
xbShort xbExpn::ProcessExpression( xbExpNode * Wtree, xbShort RecBufSw )
{
  xbExpNode * WorkNode;
  xbShort rc;
  if( Wtree == 0 )
    Wtree = Tree;
  memset(WorkBuf, 0x00, WorkBufMaxLen+1 );
  /* initialize the stack - free any expnodes */
  while( GetStackDepth() > 0 ) {
    WorkNode = (xbExpNode *) Pop();
    if( !WorkNode->InTree )
      delete WorkNode;
  }   
  if(( WorkNode = GetFirstTreeNode( Wtree )) == NULL )
    return XB_NO_DATA;

  while( WorkNode ) {
    Push(WorkNode);
    if( WorkNode->Type == 'D' && WorkNode->dbf ) {
        WorkNode->dbf->GetField( WorkNode->FieldNo, WorkNode->StringResult, RecBufSw );
        if( WorkNode->dbf->GetFieldType( WorkNode->FieldNo ) == 'N' || 
            WorkNode->dbf->GetFieldType( WorkNode->FieldNo ) == 'F' )
            WorkNode->DoubResult = WorkNode->dbf->GetDoubleField( WorkNode->FieldNo, RecBufSw );
    } else if( WorkNode->Type == 'O' ) {
      if(( rc = ProcessOperator( RecBufSw )) != XB_NO_ERROR )
        return rc;
    } else if( WorkNode->Type == 'F' )
      if(( rc = ProcessFunction( WorkNode->NodeText )) != XB_NO_ERROR )
        return rc;
      WorkNode = GetNextTreeNode( WorkNode );
  }
  if( GetStackDepth() != 1 )    /* should only have result left in stack */
    return XB_PARSE_ERROR;
  return XB_NO_ERROR;
}
/*************************************************************************/
//! Short description
/*!
  \param e
*/
char xbExpn::GetOperandType( xbExpNode * e )
{
  /* this routine returns
       L - logical
       N - Numeric
       C - Character
       0 - error
  */
  char WorkType;
  if( e->Type == 'd' || e->Type == 'N' || e->Type == 'i' ) return 'N';
  if( e->Type == 'l' ) return 'L';
  if( e->Type == 's' ) return 'C';
  if( e->Type == 'C' ) {
    if(e->NodeText[0]=='-' || e->NodeText[0]=='+' || 
      (isdigit(e->NodeText[0]) &&
      !(e->NodeText[e->DataLen] == '\'' || e->NodeText[e->DataLen] == '"')))
      return 'N';
    else
      return 'C';
  } else if( e->Type == 'D' && e->dbf ){
    WorkType = e->dbf->GetFieldType( e->FieldNo );
    if( WorkType == 'C' ) return 'C';
    else if( WorkType == 'F' || WorkType == 'N' ) return 'N';
    else if( WorkType == 'L' ) return 'L';
    else if( WorkType == 'D' ) return 'D';
    else return 0;
  } else
    return 0;
}
/*************************************************************************/
//! Short description
/*!
  \param RecBufSw
*/
xbShort xbExpn::ProcessOperator( xbShort RecBufSw )
{
  xbExpNode * WorkNode;
  char Operator[6]; 
  char t;
  if( GetStackDepth() < 3 ) 
    return XB_PARSE_ERROR;
  WorkNode = (xbExpNode *) Pop();
  if( WorkNode->Len > 5 )
    return XB_PARSE_ERROR;

  memset( Operator, 0x00, 6 );
  strncpy( Operator, WorkNode->NodeText, WorkNode->Len );
  if( !WorkNode->InTree ) 
    delete WorkNode;

  /* load up operand 1 */
  WorkNode = (xbExpNode *) Pop();
  if(( OpType1 = GetOperandType( WorkNode )) == 0 )
    return XB_PARSE_ERROR;

  if( OpLen1 < WorkNode->DataLen+1 && WorkNode->Type != 'd' ) {
    if( OpLen1 > 0 ) free( Op1 );
    if(( Op1 = (char *) malloc( WorkNode->DataLen+1 )) == NULL ) {
      return XB_NO_MEMORY;
    }
    OpLen1 = WorkNode->DataLen+1;
  }
  OpDataLen1 = WorkNode->DataLen;
  memset( Op1, 0x00, WorkNode->DataLen+1 );
  if( WorkNode->Type == 'D' && WorkNode->dbf ) {    /* database field  */
    WorkNode->dbf->GetField( WorkNode->FieldNo, Op1, RecBufSw );
    t = WorkNode->dbf->GetFieldType( WorkNode->FieldNo );
    if( t == 'N' || t == 'F' )
      Opd1 = strtod( WorkNode->StringResult, 0 );
    else if( t == 'D' ){   // date field
      xbDate d;
      Opd1 = d.JulianDays( WorkNode->StringResult );
    }
  }
  else if( WorkNode->Type == 'C' )     /* constant        */
    memcpy( Op1, WorkNode->NodeText, WorkNode->DataLen );
  else if( WorkNode->Type == 's' )     /* previous result */
    memcpy( Op1, WorkNode->StringResult, WorkNode->DataLen+1 );
  else if( WorkNode->Type == 'd' )     /* previous numeric result */   
    Opd1 = WorkNode->DoubResult;
  else if( WorkNode->Type == 'N' )     /* previous numeric result */   
    Opd1 = strtod( WorkNode->StringResult, 0 );
  else if(WorkNode->Type == 'l')       /* previous logical result 3/26/00 dtb */
    Opd1 = WorkNode->IntResult;
  if( !WorkNode->InTree ) 
    delete WorkNode;

  /* load up operand 2 */
  WorkNode = (xbExpNode *) Pop();
  if(( OpType2 = GetOperandType( WorkNode )) == 0 )
    return XB_PARSE_ERROR;

  if( OpLen2 < WorkNode->DataLen+1 && WorkNode->Type != 'd' ) {
    if( OpLen2 > 0 ) free( Op2 );
    if(( Op2 = (char *) malloc( WorkNode->DataLen+1 )) == NULL ) {
      return XB_NO_MEMORY;
    }
    OpLen2 = WorkNode->DataLen+1;
  }
  OpDataLen2 = WorkNode->DataLen;
  memset( Op2, 0x00, WorkNode->DataLen+1 );
  if( WorkNode->Type == 'D' && WorkNode->dbf ) {    /* database field  */
    WorkNode->dbf->GetField( WorkNode->FieldNo, Op2, RecBufSw );
    t = WorkNode->dbf->GetFieldType( WorkNode->FieldNo );
    if( t == 'N' || t == 'F' )
      Opd2 = strtod( WorkNode->StringResult, 0 );
    else if( t == 'D' ){   // date field
      xbDate d;
      Opd2 = d.JulianDays( WorkNode->StringResult );
    }
  }
  else if( WorkNode->Type == 'C' )     /* constant        */
    memcpy( Op2, WorkNode->NodeText, WorkNode->DataLen );
  else if( WorkNode->Type == 's' )     /* previous result */
    memcpy( Op2, WorkNode->StringResult, WorkNode->DataLen+1 );
  else if( WorkNode->Type == 'd' )     /* previous numeric result */
    Opd2 = WorkNode->DoubResult;
  else if( WorkNode->Type == 'N' )     /* previous numeric result */   
    Opd2 = strtod( WorkNode->StringResult, 0 );
  else if(WorkNode->Type == 'l')       /* previous logical result 3/26/00 dtb*/
    Opd2 = WorkNode->IntResult;
  if( !WorkNode->InTree )
    delete WorkNode;
  if( !ValidOperation( Operator, OpType1, OpType2 ))
    return XB_PARSE_ERROR;

  if( OpType1 == 'N' || OpType1 == 'L' || OpType1 == 'D' ) /* numeric procesing */
    return NumericOperation( Operator );
  else                    /* must be character */
    return  AlphaOperation( Operator );
}
/*************************************************************************/
//! Short description
/*!
  \param Operator
*/
xbShort xbExpn::NumericOperation( char * Operator )
{
  xbDouble  Operand1, Operand2;
  xbExpNode * WorkNode;
  xbShort   ResultLen;
  char      SaveType;
  ResultLen = 0;

/* This function assumes a valid operation coming in */

  if( Operator[0] == '=' || Operator[0] == '<' || 
      Operator[0] == '>' || Operator[0] == '#'  ||
      Operator[0] == '.' || (strncmp( Operator, "!=", 2 ) == 0 ))
    SaveType = 'l';
  else
    SaveType = 'd';

  WorkNode = new xbExpNode;
   
  if( !WorkNode )
    return XB_PARSE_ERROR;
  WorkNode->ResultLen = ResultLen;
  WorkNode->Type = SaveType;
  WorkNode->DataLen = ResultLen;

  if( OpType1 == 'd' || OpType1 == 'N' || OpType2 == 'D' )
    Operand1 = Opd1;
  else
    Operand1 = strtod( Op1, NULL );

  if( OpType2 == 'd' || OpType2 == 'N' || OpType2 == 'D' )
    Operand2 =  Opd2;
  else
    Operand2 = strtod( Op2, NULL );

  if( Operator[0] == '*' && Operator[1] == '*' )
    WorkNode->DoubResult = pow( Operand2, Operand1 );
  else if( Operator[0] == '*' )
    WorkNode->DoubResult = Operand2 * Operand1;
  else if( Operator[0] == '/') 
    WorkNode->DoubResult = Operand2 / Operand1;
  else if( Operator[0] == '+' ){
    WorkNode->DoubResult = Operand2 + Operand1;
    xbDate d;
    WorkNode->StringResult = d.JulToDate8((xbLong) WorkNode->DoubResult );
  } else if( Operator[0] == '-' ){
    WorkNode->DoubResult = Operand2 - Operand1;
    xbDate d;
    WorkNode->StringResult = d.JulToDate8((xbLong) WorkNode->DoubResult );
  }

  /* = */
  else if( Operator[0]== '=' && Operand1 == Operand2 )
    WorkNode->IntResult = 1;
  else if( Operator[0] == '=' )
    WorkNode->IntResult = 0;
  /* not = */
  else if(( Operator[0] == '<' && Operator[1] == '>' )||
          ( Operator[0] == '!' && Operator[1] == '=' )||
            Operator[0] == '#' || (strncmp( Operator, "!=", 2 ) == 0 )) 
    WorkNode->IntResult = ( Operand1 != Operand2 ) ? 1 : 0;
  /* less than */
  else if( Operator[0] == '<' ) 
    WorkNode->IntResult = ( Operand2 < Operand1 ) ? 1 : 0;
  /* greater than */
  else if( Operator[0] == '>' ) 
    WorkNode->IntResult = ( Operand2 > Operand1 ) ? 1 : 0;
  else if(Operator[0] == '.'){ // logical operators, added 3/26/00 dtb
    switch(Operator[1]){
     case 'A' : // and
      WorkNode->IntResult = (Opd1 && Opd2) ? 1 : 0;
      break;
       
     case 'N' : // not
      WorkNode->IntResult = (!(Opd1 && Opd2)) ? 1 : 0;
      break;
       
     case 'O' : // or
      WorkNode->IntResult = (Opd1 || Opd2) ? 1 : 0;
      break;
       
     default :
      return XB_PARSE_ERROR;
    }
  } else
    return XB_PARSE_ERROR;

  Push(WorkNode);
  return 0;
}
/*************************************************************************/
//! Short description
/*!
  \param Operator
*/
xbShort xbExpn::AlphaOperation( char * Operator )
{
  xbShort ResultLen, i;
  char SaveType;
  xbExpNode * WorkNode;

  if( Operator[0] == '=' || Operator[0] == '<'  || 
      Operator[0] == '>' || Operator[0] == '#'  ||
      (strncmp( Operator, "!=", 2 ) == 0 )      ||
      Operator[0] == '$'){
    ResultLen = 0;
    SaveType = 'l';
  } else {
    ResultLen = OpDataLen1 + OpDataLen2 + 1;
    SaveType = 's';
  }
   
  WorkNode = new xbExpNode;
  if( !WorkNode )
    return XB_PARSE_ERROR;
  WorkNode->ResultLen = ResultLen;
  WorkNode->Type = SaveType;
  if( WorkNode->Type == 'l' )
    WorkNode->DataLen = 0;
  else
    WorkNode->DataLen = ResultLen - 1;

  if( Operator[0] == '+' ){
    WorkNode->StringResult = Op2;
    WorkNode->StringResult += Op1;
  } else if( Operator[0] == '-' ) {
    WorkNode->StringResult = RTRIM( Op2 );
    WorkNode->StringResult += Op1;
    i = WorkNode->StringResult.len();
    for( ; i < ResultLen-1; i++)
      WorkNode->StringResult += " ";
  }
  /* == */
  else if(( strncmp( Operator, "==", 2 ) == 0 ) && strcmp(Op1,Op2) == 0)
    WorkNode->IntResult = 1;

  else if(( strncmp( Operator, "==", 2 ) == 0 ))
    WorkNode->IntResult = 0;

  /* = */
  else if( Operator[0] == '=' && strcmp(Op1,Op2) == 0 )
    WorkNode->IntResult = 1;

  else if( Operator[0] == '=' )
    WorkNode->IntResult = 0;

  /* not = */
  else if(( strncmp( Operator, "<>", 2 ) == 0 ) ||
            Operator[0] == '#' ||
            strncmp( Operator, "!=", 2 ) == 0 ) 
    WorkNode->IntResult = ( strcmp( Op1, Op2 ) != 0 ) ? 1 : 0;
  /* less than */
  else if( Operator[0] == '<' )
    WorkNode->IntResult = ( strcmp( Op2, Op1 ) < 0 ) ? 1 : 0;
  /* greater than */
  else if( Operator[0] == '>' )
    WorkNode->IntResult = ( strcmp( Op2, Op1 ) > 0 ) ? 1 : 0;
  else if(Operator[0] == '$')
    WorkNode->IntResult = (strstr(Op1,Op2)) ? 1 : 0;
  else
    return XB_PARSE_ERROR;

  Push(WorkNode);
  return XB_NO_ERROR;
}
/*************************************************************************/
#endif   // XB_EXPRESSIONS
