/*  xbfilter.cpp

    Xbase project source code
  
    This file conatains logic for the xbfilter class.

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
  #pragma implementation "xbfilter.h"
#endif

#ifdef __WIN32__
#include <xbase64/xbwincfg.h>
#else
#include <xbase64/xbconfig.h>
#endif

#include <xbase64/xbase64.h>
//#include <xbase64/xbexcept.h>

/*! \file xbfilter.cpp
*/

#ifdef XB_FILTERS
/************************************************************************/
//! Constructor.
/*!
  \param dbf
  \param index
  \param exp
*/
xbFilter::xbFilter( xbDbf * dbf, xbIndex * index, char * exp )
{
  xbShort rc;
  Status = 0;
  CurFilterRecNo = 0L;
  d = dbf;
  i = index;
//  e = 0;


  flExpn = new xbExpn( d->xbase );
  if(( rc = flExpn->ParseExpression( exp, d )) != XB_NO_ERROR )
    Status = rc;
  else{
    if( flExpn->GetExpressionResultType() != 'L' )
      Status = XB_PARSE_ERROR;
  }
}

/***********************************************************************/
//! Destructor.
/*!
*/
xbFilter::~xbFilter()
{
  if( flExpn )
    delete flExpn;
}

/***********************************************************************/
//! Short description.
/*!
*/
xbShort xbFilter::GetFirstFilterRec()
{
  xbShort rc;

  if( Status )
    return Status;

  if( i )
    rc = i->GetFirstKey();
  else
    rc = d->GetFirstRecord();

  while( rc == XB_NO_ERROR ){
    if(( rc = flExpn->ProcessExpression()) != XB_NO_ERROR )
      return rc;

    if( flExpn->GetIntResult() )
    {
      CurFilterRecNo = d->GetCurRecNo();
      return XB_NO_ERROR;
    }
    if( i )
      rc = i->GetNextKey();
    else
      rc = d->GetNextRecord();
  }
  return rc;
}
/***********************************************************************/
//! Short description.
/*!
*/
xbShort xbFilter::GetLastFilterRec()
{
  xbShort rc;

  if( Status )
    return Status;

  if( i )
    rc = i->GetLastKey();
  else
    rc = d->GetLastRecord();

  while( rc == XB_NO_ERROR ){
    if(( rc = flExpn->ProcessExpression()) != XB_NO_ERROR )
      return rc;

    if( flExpn->GetIntResult() )
    {
      CurFilterRecNo = d->GetCurRecNo();
      return XB_NO_ERROR;
    }
    if( i )
      rc = i->GetPrevKey();
    else
      rc = d->GetPrevRecord();
  }
  return rc;
}
/***********************************************************************/
//! Short description.
/*!
*/
xbShort xbFilter::GetNextFilterRec()
{
  xbShort rc;

  if( Status )
    return Status;

  if( !CurFilterRecNo )
    return GetFirstFilterRec();

  if( i ){
    rc = i->GetNextKey();
  }
  else
    rc = d->GetNextRecord();

  while( rc == XB_NO_ERROR ){
    if(( rc = flExpn->ProcessExpression()) != XB_NO_ERROR )
      return rc;

    if( flExpn->GetIntResult())
    {
      CurFilterRecNo = d->GetCurRecNo();
      return XB_NO_ERROR;
    }
    if( i )
      rc = i->GetNextKey();
    else
      rc = d->GetNextRecord();
  }
  return rc;
}
/***********************************************************************/
//! Short description.
/*!
*/
xbShort xbFilter::GetPrevFilterRec()
{
  xbShort rc;

  if( Status )
    return Status;

  if( !CurFilterRecNo )
    return GetLastFilterRec();

  if( i ){
    rc = i->GetPrevKey();
  }
  else
    rc = d->GetPrevRecord();

  while( rc == XB_NO_ERROR ){
    if(( rc = flExpn->ProcessExpression()) != XB_NO_ERROR )
      return rc;

    if( flExpn->GetIntResult())
    {
      CurFilterRecNo = d->GetCurRecNo();
      return XB_NO_ERROR;
    }
    if( i )
      rc = i->GetPrevKey();
    else
      rc = d->GetPrevRecord();
  }
  return rc;
}
/***********************************************************************/
#endif  // XB_FILTERS_ON
