/*  xbstring.h

    Xbase64 project source code

    This file contains the Class definition for a xbString object.

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

#ifndef __XBSTRING_H__
#define __XBSTRING_H__

#ifdef __GNU_LesserG__
#pragma interface
#endif

#ifdef __WIN32__
#include <xbase64/xbwincfg.h>
#else
#include <xbase64/xbconfig.h>
#endif

#include <stdlib.h>
#include <iostream>

/*! \file xbstring.h
*/

//! xbString class
/*!
*/
class XBDLLEXPORT xbString {
public:
  enum {npos = -1};

  xbString();
  xbString(size_t size);
  xbString(char c);
  xbString(const char *s);
  xbString(const char *s, size_t maxlen);
  xbString(const xbString &s);
  virtual ~xbString();

  operator const char *() const;
  char operator[](int n) { return data[n]; }

  xbString &operator=(const xbString &s);
  xbString &operator=(const char *s);
  xbString &operator=(char c);
  xbString &operator+=(const char *s);
  xbString &operator+=(char c);
  xbString &operator-=(const char *s);

  xbBool operator == ( const xbString& ) const;
  xbBool operator != ( const xbString& ) const;
  xbBool operator < ( const xbString& ) const;
  xbBool operator > ( const xbString& ) const;
  xbBool operator <= ( const xbString& ) const;
  xbBool operator >= ( const xbString& ) const;

  friend XBDLLEXPORT std::ostream& operator << ( std::ostream&,
                                             const xbString& );
  void     addBackSlash( char c );
  xbString &assign(const xbString& str, size_t pos = 0, int n = npos);
  xbString &assign(char* str, int n);
  xbString copy() const;
  const char *c_str() const;
  int      countChar( char c ) const;
  int      cvtHexChar( char & out );
  int      cvtHexString( xbString & out );
  char     getCharacter( int n ) const { return data[n]; }
  const char *getData() const;
  xbBool   hasAlphaChars() const;
  xbBool   isEmpty() const;
  xbBool   isNull() const;
  size_t   len() const;
  size_t   length() const;
  xbString mid(size_t pos = 0, int n = npos) const;
  void     lTrunc( size_t cnt );
  int      pos(char c);
  int      pos(const char* s);
  void     putAt(size_t pos, char c);
  xbString &remove(size_t pos = 0, int n = npos);
  void     resize(size_t size);  
  void     setNum(long num);
  void     setNum(char * fmt, double num);
  xbString &sprintf(const char *format, ...);
  void     swapChars( char from, char to );
  void     toLowerCase();
  void     toUpperCase();
  void     trim();
  void     zapChar( char c );
  void     zapLeadingChar( char c );
  int      setFromDelimitedInput(const char *,char, int, int );

protected:
  void ctor(const char *s);
  void ctor(const char *s, size_t maxlen);
  char *data;
  size_t size;
  static const char * NullString;
};

XBDLLEXPORT xbString operator-(const xbString &s1, const xbString &s2);
XBDLLEXPORT xbString operator+(const xbString &s1, const xbString &s2);
XBDLLEXPORT xbString operator+(const xbString &s1, const char *s2);
XBDLLEXPORT xbString operator+(const char *s1, const xbString &s2);
XBDLLEXPORT xbString operator+(const xbString &s1, char c2);
XBDLLEXPORT xbString operator+(char c1, const xbString &s2);
XBDLLEXPORT xbBool operator==(const xbString &s1, const char *s2);
XBDLLEXPORT xbBool operator!=(const xbString &s1, const char *s2);

#endif

