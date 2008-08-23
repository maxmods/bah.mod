/*  xbstring.cpp
 
    Xbase64 project source code

    This file contains the xbString object methods

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
  #pragma implementation "xbstring.h"
#endif

#ifdef __WIN32__
#include <xbase64/xbwincfg.h>
#else
#include <xbase64/xbconfig.h>
#endif

#include <xbase64/xbase64.h>

#include <stdlib.h>
#include <stdio.h>

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#ifdef STDC_HEADERS
#include <stdarg.h>
#endif

#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif

#include <xbase64/xbstring.h>
//#include <xbase64/xbexcept.h>

//#define free(x)

/*! \file xbstring.cpp
*/

const char * xbString::NullString = "";

//! Short description.
/*!
*/
xbString::xbString() {
  ctor(NULL);
}

//! Short description.
/*!
  \param size
*/
xbString::xbString(size_t size) {
  data = (char *)calloc(1, size);
  this->size = size;
}

//! Short description.
/*!
  \param c
*/
xbString::xbString(char c) {
  ctor(NULL);
  *this = c;
}

//! Short description.
/*!
  \param s
*/
xbString::xbString(const char *s) {
  ctor(s);
}

//! Short description.
/*!
  \param s
*/
xbString::xbString(const xbString &s) {
  ctor((const char *)s);
}

//! Short description.
/*!
  \param s
  \param maxlen
*/
xbString::xbString(const char *s, size_t maxlen) {
#if 0
  size_t len = strlen(s);
  
  if(len < maxlen)
    maxlen = len;
#endif    

  size = maxlen + 1;
  data = (char *)calloc(1, size);
  strncpy(data, s, maxlen);
  data[maxlen] = 0;
}

//! Short description.
/*!
*/
xbString::~xbString() {
  if (data != NULL)
    free(data);
}

//! Short description.
/*!
  \param s
*/
void xbString::ctor(const char *s) {
  if (s == NULL) {
    data = NULL;
    size = 0;
    return;
  }

  size = strlen(s) + 1;

  data = (char *)calloc(1, size);
  strcpy(data, s);
}

//! Short description.
/*!
  \param s
  \param maxlen
*/
void xbString::ctor(const char *s, size_t maxlen) {

  if (s == NULL) {
    data = NULL;
    size =0;
    return;
  }
  size = maxlen + 1;
  data = (char *)calloc(1, size);
  strncpy(data, s, maxlen);
  data[maxlen] = 0;
}

//! Short description.
/*!
*/
xbString &xbString::operator=(char c) {
  if (data != NULL)
    free(data);

  data = (char *)calloc(1, 2);
  data[0] = c;
  data[1] = 0;

  size = 2;

  return (*this);
}

//! Short description.
/*!
*/
xbString &xbString::operator=(const xbString &s) {
  if (data != NULL)
    free(data);

  const char *sd = s;
  if (sd == NULL) {
    data = NULL;
    size = 0;
    return (*this);
  }

  data = (char *)calloc(1, strlen(s) + 1);
  strcpy(data, s);

  size = strlen(data)+1;
  
  return (*this);
}

//! Short description.
/*!
*/
xbString &xbString::operator=(const char *s) {
  if(data != NULL)
    free(data);

  if(s == NULL) {
    data = NULL;
    size = 0;
    return (*this);
  }
  data = (char *)calloc(1, strlen(s) + 1);
  strcpy(data, s);
  size = strlen(data) + 1;
  return (*this);
}

//! Short description.
/*!
  \param size
*/
void xbString::resize(size_t size) {
  data = (char *)realloc(data, size);
  if( size > 0 )
    data[size-1] = 0;
  this->size = size;
}
     
//! Short description.
/*!
*/
xbBool xbString::isNull() const {
  return( data == NULL );
}

//! Short description.
/*!
*/
xbBool xbString::isEmpty() const {
  if( data == NULL )
    return true;
  if( data[0] == 0 )
    return true;
  return false;
}

//! Short description.
/*!
*/
size_t xbString::len() const {
  return( data ? strlen(data) : 0 );
}

//! Short description.
/*!
*/
size_t xbString::length() const {
  return len();
}

//! Short description.
/*!
*/
xbString xbString::copy() const {
  return( *this );
}

//! Short description.
/*!
  \param format
*/
xbString &xbString::sprintf(const char *format, ...) {
  va_list ap;
  va_start(ap, format);

  if (size < 256)
    resize(256);              // make string big enough

#ifdef HAVE_VSNPRINTF
  if (vsnprintf(data, size, format, ap) == -1)
    data[size-1] = 0;
#else
#  if HAVE_VSPRINTF
  vsprintf(data, format, ap);
#  else
#      error "You have neither vsprintf nor vsnprintf!!!"
#  endif
#endif

  resize(strlen(data)+1);               // truncate
  va_end(ap);
  return (*this);
}

//! Short description.
/*!
*/
xbString::operator const char *() const {
  return (data != NULL) ? data : NullString;
}

//! Short description.
/*!
*/
xbString &xbString::operator-=(const char *s) {
  if( s == NULL ) return (*this);
  int len = strlen(s);
  int oldlen = this->len();

  data = (char *)realloc(data, oldlen+len+1);
  if( oldlen == 0 ) data[0] = 0;

  // looking for an occurence of space in the first string
  char *lftspc = strchr(data,' ');
  if( lftspc==NULL ) { // left string has no spaces
    strcat(data,s);
  } else { // left string has one or more spaces
    int numspc = strlen(lftspc);
    strcpy(lftspc,s);
    while( numspc-- > 0 ) strcat(lftspc," ");
  }

  size += len;
  return (*this);
}

//! Short description.
/*!
*/
xbString &xbString::operator+=(const char *s) {
  if (s == NULL)
    return (*this);
  int len = strlen(s);
  int oldlen = this->len();

  data = (char *)realloc(data, oldlen+len+1);
   if (oldlen == 0)
      data[0] = 0;
  strcat(data, s);

  size += len;
  return (*this);
}

//! Short description.
/*!
*/
xbString &xbString::operator+=(char c) {
  int len = 1;
  int oldlen = this->len();

  data = (char *)realloc(data, oldlen+len+1);
  data[oldlen] = c;
  data[oldlen+1] = 0;

  size++;

  return (*this);
}

//! Short description.
/*!
*/
const char *xbString::getData() const {
  return data ? data : NullString;
}

//! Short description.
/*!
*/
const char *xbString::c_str() const {
  return data ? data : NullString;
}

//! Short description.
/*!
*/
void xbString::toLowerCase() {
  int len = this->len();
  for (int i=0;i<len;i++)
    data[i] = (char)tolower(data[i]);
}


//! Short description.
/*!
*/
void xbString::toUpperCase() {
  int len = this->len();
  for (int i=0;i<len;i++)
    data[i] = (char)toupper(data[i]);
}


//! Short description.
/*!
  \param c
*/
int xbString::pos(char c) {
  if (data == NULL)
    return (-1);

  const char *p = strchr(data, c);

  if (p == NULL)
    return (-1);

  return p-data;
}

//! Short description.
/*!
  \param s
*/
int xbString::pos(const char* s) {
  if (data == NULL)
    return (-1);

  const char *p = strstr(data, s);

  if (p == NULL)
    return (-1);

  return p-data;
}

//! Short description.
/*!
  \param num
*/
void xbString::setNum(long num) {
  sprintf("%ld", num);
}

//! Short description.
/*!
  \param fmt
  \param num
*/

void xbString::setNum( char * fmt, double num) {
  xbString f;
  f = "%";
  f += fmt;
  f += "f";
  sprintf( f.getData(), num);
}


//! Short description.
/*!
*/
XBDLLEXPORT xbBool operator==(const xbString &s1, const char *s2) {
  if (s2 == NULL) {
    if (s1.getData() == NULL)
      return true;
    return false;
  }

   if ((s2[0] == 0) && s1.getData() == NULL)
      return true;

  if (s1.getData() == NULL)
    return false;

  return (strcmp(s1, s2) == 0);
}

//! Short description.
/*!
*/
XBDLLEXPORT xbBool operator!=(const xbString &s1, const char *s2) {
  if (s2 == NULL) {
    if (s1.getData() == NULL)
      return false;
    return true;
  }

   if ((s2[0] == 0) && s1.getData() == NULL)
      return false;

  if (s1.getData() == NULL)
    return true;

  return (strcmp(s1, s2) != 0);
}

//! Short description.
/*!
*/
xbBool xbString::operator==( const xbString &s2 ) const {
  if( data == NULL || data[0] == 0 ) {
    if( s2.data == NULL || s2.data[0] == 0 ) return true; // NULL == NULL
    return false; // NULL == !NULL
  } else {
    if( s2.data == NULL || s2.data[0] == 0 ) return false; // !NULL == NULL
    return strcmp(data,s2.data) == 0; //!NULL == !NULL
  }
}

//! Short description.
/*!
*/
xbBool xbString::operator!=( const xbString &s2 ) const {
  if( data == NULL || data[0] == 0 ) {
    if( s2.data == NULL || s2.data[0] == 0 ) return false; // NULL != NULL
    return true; // NULL != !NULL
  } else {
    if( s2.data == NULL || s2.data[0] == 0 ) return true; // !NULL != NULL
    return strcmp(data,s2.data) != 0; //!NULL != !NULL
  }
}

//! Short description.
/*!
*/
xbBool xbString::operator< ( const xbString &s2 ) const {
  if( data == NULL || data[0] == 0 ) {
    if( s2.data == NULL || s2.data[0] == 0 ) return false; // NULL < NULL
    return true; // NULL < !NULL
  } else {
    if( s2.data == NULL || s2.data[0] == 0 ) return false; // !NULL < NULL
    return strcmp(data,s2.data) < 0; //!NULL < !NULL
  }
}

//! Short description.
/*!
*/
xbBool xbString::operator> ( const xbString &s2 ) const {
  if( data == NULL || data[0] == 0 ) {
    if( s2.data == NULL || s2.data[0] == 0 ) return false; // NULL > NULL
    return false; // NULL > !NULL
  } else {
    if( s2.data == NULL || s2.data[0] == 0 ) return true; // !NULL > NULL
    return strcmp(data,s2.data) > 0; //!NULL > !NULL
  }
}

//! Short description.
/*!
*/
xbBool xbString::operator<=( const xbString &s2 ) const {
  if( data == NULL || data[0] == 0 ) {
    if( s2.data == NULL || s2.data[0] == 0 ) return true; // NULL <= NULL
    return true; // NULL <= !NULL
  } else {
    if( s2.data == NULL || s2.data[0] == 0 ) return false; // !NULL <= NULL
    return strcmp(data,s2.data) <= 0; //!NULL <= !NULL
  }
}

//! Short description.
/*!
*/
xbBool xbString::operator>=( const xbString &s2 ) const {
  if( data == NULL || data[0] == 0 ) {
    if( s2.data == NULL || s2.data[0] == 0 ) return true; // NULL >= NULL
    return false; // NULL >= !NULL
  } else {
    if( s2.data == NULL || s2.data[0] == 0 ) return true; // !NULL >= NULL
    return strcmp(data,s2.data) >= 0; //!NULL >= !NULL
  }
}

//! Short description.
/*!
*/
XBDLLEXPORT std::ostream& operator<< ( std::ostream& os,
                                       const xbString& xbs ) {
  return os << xbs.data;
}

//! Short description.
/*!
*/
XBDLLEXPORT xbString operator-(const xbString &s1, const xbString &s2) {
   xbString tmp(s1.getData());
   tmp -= s2;
   return tmp;
}

//! Short description.
/*!
*/
XBDLLEXPORT xbString operator+(const xbString &s1, const xbString &s2) {
   xbString tmp(s1.getData());
   tmp += s2;
   return tmp;
}

//! Short description.
/*!
*/
XBDLLEXPORT xbString operator+(const xbString &s1, const char *s2) {
   xbString tmp(s1.getData());
   tmp += s2;
   return tmp;
}

//! Short description.
/*!
*/
XBDLLEXPORT xbString operator+(const char *s1, const xbString &s2) {
   xbString tmp(s1);
   tmp += s2;
   return tmp;
}

//! Short description.
/*!
*/
XBDLLEXPORT xbString operator+(const xbString &s1, char c2) {
   xbString tmp(s1.getData());
   tmp += c2;
   return tmp;
}

//! Short description.
/*!
*/
XBDLLEXPORT xbString operator+(char c1, const xbString &s2) {
   xbString tmp(c1);
   tmp += s2;
   return tmp;
}

//! Short description.
/*!
  \param pos
  \param c
*/
void xbString::putAt(size_t pos, char c) {
   if (pos>len())
      return;

   data[pos] = c;
}

//! Short description.
/*!
  \param str
  \param pos
  \param n
*/
xbString& xbString::assign(const xbString& str, size_t pos, int n)
{
  if(data){
    free(data);
    data = 0;
  }

  if(str.len() <= pos){
    size = 0;
    return (*this);
  }

  if(str.len() < pos + n){
    n = str.len() - pos;
  }

  const char *d = str;

  if (n == -1){
//        data = (char *)malloc(str.len()-pos+1); ms win/nt bug fix
    data = (char *)calloc(str.len()-pos+1, sizeof( char ));
    strcpy(data, d+pos);
    size = str.len()-pos+1;
  }
  else
  {
//   data = (char *)malloc(n);  ms win/nt bug fix
// boundschecker flags the next line as a memory leak
// but this is a valid memory allocation
    data = (char *)calloc(n + 1, sizeof(char));
    strncpy(data, d + pos, n);
    data[n] = '\0';
    size = n + 1;
  }

  return (*this);
}

//! Short description.
/*!
  \param str
  \param n
*/
xbString& xbString::assign(char* str, int n)
{
  if(data)
  {
    free(data);
    data = 0;
  }

  data = (char *)calloc(n + 1, sizeof(char));
  strncpy(data, str, n);
  data[n] = 0;
  size = n + 1;

  return (*this);
}

//! Short description.
/*!
*/
void xbString::trim() {
  int l = len()-1;

   for (;;) {
      if (data[l] != ' ')
         break;
      data[l] = 0;
      if (l == 0)
         break;
      l--;
   }
}

//! Short description.
/*!
  \param pos
  \param n
*/
xbString &xbString::remove(size_t pos, int n) {
  if (data == NULL)
    return (*this);
  if (data[0] == 0)
    return (*this);

  size_t l = len();

  if (pos>l)
    return (*this);
  if (n == 0)
    return (*this);
  if (n > int(l-pos))
    n = l-pos;
  if (n<0)
    n = l-pos;
  memcpy(data+pos, data+pos+n, l-pos-n+1);

  return (*this);
}

//! Short description.
/*!
  \param pos
  \param n
*/
xbString xbString::mid(size_t pos, int n) const {
  if (data == NULL)
    return (*this);
  if (data[0] == 0)
    return (*this);

  size_t l = len();

  if (pos>l)
    return (*this);
  if (n == 0)
    return (*this);
  if (n > int(l-pos))
    n = l-pos;
  if (n<0)
    n = l-pos;

  xbString s;
  s.data = (char *)malloc(n+1);
  strncpy(s.data, data+pos, n);
  s.data[n] = 0;

  return s;
}

//! Short description.
/*!
  \param from
  \param to
*/
void xbString::swapChars( char from, char to )
{
  size_t i;
  for( i = 0; i < size; i++ )
    if( data[i] == from )
       data[i] = to;
} 

//! Short description.
/*!
  \param c
*/
void xbString::zapChar( char c )
{
  /* routine zaps every occurrence of a given character from the string */
  int p;
  size_t s;
  p = pos( c );
  while( p != -1 ){
    for( s = (size_t) p; s < size; s++ )
      putAt( s, data[s+1]);
    resize( size-1 );
    p = pos( c );
  }
}

//! Short description.
/*!
  \param c
*/
int xbString::countChar( char c ) const
{
  int i,j;
  
  for( i = 0,j = 0; i < (int) size; i++ )
    if( data[i] == c )
      j++;

  return j;
}


//! Short description.
/*!
  \param c
*/

void xbString::addBackSlash( char c )
{
 /* prefixes all char "c" with a backslash */
 int i, t, cnt;
 xbString ws;
  
  cnt = countChar( c );
  if( !cnt )
    return;
    
  ws.resize( size+cnt );
  for( i = 0, t = 0; i < (int)size; i++ ){
    if( data[i] == c )
      ws.putAt( t++, '\\' );
    ws.putAt( t++, data[i] );
  }
  ws.putAt( t, 0 );
  *this = ws.getData();
}

//! Short description.
/*!
  \param cnt
*/
void xbString::lTrunc( size_t cnt )
{
  /* left truncate cnt butes */

  char * ndata;
  char * p;
  if( cnt >= size ){
    ctor(0);
    return;
  }
  ndata = (char *) malloc( size - cnt );
  p = data;
  p += cnt;
  strcpy( ndata, p );
  free( data );
  data = ndata;
  size = size - cnt;
}


//! Short description.
/*!
  \param c
*/
void xbString::zapLeadingChar( char c )
{
  /* left truncate all of character c */

  int len = 0;
  char *p;
  p = data;
  while( *p && *p == c ){
    len++;
    p++;
  }
  if( len )
    lTrunc( len );
}


//! Short description.
/*!
*/
xbBool xbString::hasAlphaChars() const
{
  for( int i = 0; i < (int) size; i++ )
    if( isalpha( data[i] ))
      return 1;
  return 0;
}


//! Short description.
/*!
  \param out
*/

int xbString::cvtHexChar( char & out )
{
  /* this routine converts a four byte string in the format of 0x00 
     to a one byte char value 
     
     the first four bytes of the string must be in the format 0x00
     anything past the first four bytes is disregarded
     
     returns -1 on error
              0 on success 
  */

  int  j, k;
  char c;
    
  if( len() < 4 || data[0] != '0' || (data[1]!='X' && data[1]!='x' ))
    return -1;

  c = toupper( data[2] );
  j = ( c > '9' ? c - 'A' + 10 : c - '0' );
  c = toupper( data[3] );
  k = ( c > '9' ? c - 'A' + 10 : c - '0' );
  j = ( j << 4 ) + k;
    
  out = ( char ) j; 
  return 0;
} 

//! Short description.
/*!
  \param out
*/
int xbString::cvtHexString( xbString & out )
{
  /* this routine converts a string of four byte format of 0x00 
     to a string of one byte chars 
     
     returns -1 on error
              0 on success 
  */

  char c;
  xbString ws;
  ws = data;
  out = "";
  while( ws.len()){
    if( ws.cvtHexChar( c ))
      return -1;
    out += c;
    ws.lTrunc( 4 );
  }
  return 0;
}
//! Short description.
/*!
  \param src
  \param delim
  \param skipcnt
  \param opt
*/
int xbString::setFromDelimitedInput( const char * src, 
                char delim, int skipcnt, int opt )
{
  /* opt values

     1 - ignore delimiters between quotes
     2 - treat crlf characters as delimters
     3 - both options 1 and 2 
  */
 
  int len;
  int curpos = 0;
  int quotesw = 0;
  const char * s;
  const char * anchor;

  /* skip past skipcnt delimiters */
  s = src;
  while( *s && curpos < skipcnt ){ 
    if( *s == delim && !quotesw )
      curpos++;
    else if (( opt == 1 || opt == 3 ) &&  *s == '"' )
      quotesw = (quotesw) ? 0 : 1;
    s++;
  }
  /* at the beginning of the field */
  anchor = s;
  while( *s && ( *s != delim || ( *s == delim && quotesw ))){
    if( *s == '"' )
      quotesw = (quotesw) ? 0 : 1;
    s++;
  }
  len = s - anchor;

  /* copy data */
  data = (char *) realloc( data, len+1 );
  memcpy( data, anchor, len );
  data[len] = 0;
  this->size = len+1;

  if( opt == 2 || opt == 3 ){
    zapChar( 0x0a );
    zapChar( 0x0c );
    zapChar( 0x0d );
  }

  return len;
}

