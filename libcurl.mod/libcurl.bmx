' Copyright (c) 2007-2013 Bruce A Henderson
' 
' Permission is hereby granted, free of charge, to any person obtaining a copy
' of this software and associated documentation files (the "Software"), to deal
' in the Software without restriction, including without limitation the rights
' to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
' copies of the Software, and to permit persons to whom the Software is
' furnished to do so, subject to the following conditions:
' 
' The above copyright notice and this permission notice shall be included in
' all copies or substantial portions of the Software.
' 
' THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
' IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
' FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
' AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
' LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
' OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
' THE SOFTWARE.

SuperStrict

Rem
bbdoc: libcurl
End Rem
Module BaH.libcurl

ModuleInfo "Version: 1.04"
ModuleInfo "Author: Bruce A Henderson"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: (libcurl) 1996 - 2013, Daniel Stenberg"
ModuleInfo "Copyright: (c-ares) 1998 Massachusetts Institute of Technology, 2004 - 2011 by Daniel Stenberg et al"
ModuleInfo "Copyright: (Wrapper) 2007-2013 Bruce A Henderson"
ModuleInfo "Modserver: BRL"

ModuleInfo "History: 1.04"
ModuleInfo "History: Update to libcurl 7.31.0"
ModuleInfo "History: Update to c-ares 1.10.0"
ModuleInfo "History: Fix for Win32 blocked select()."
ModuleInfo "History: Do not build acountry.c."
ModuleInfo "History: 1.03"
ModuleInfo "History: Update to libcurl 7.28.1"
ModuleInfo "History: Update to c-ares 1.9.1"
ModuleInfo "History: Fixed ResponseCode() not returning correct codes."
ModuleInfo "History: ReadStream now uses Read() instead of ReadBytes()."
ModuleInfo "History: 1.02"
ModuleInfo "History: Update to libcurl 7.18.0"
ModuleInfo "History: Update to c-ares 1.5.1"
ModuleInfo "History: Now nulls internal slist."
ModuleInfo "History: 1.01"
ModuleInfo "History: Added c-ares (1.4.0) for asynchronous DNS resolving."
ModuleInfo "History: 1.00 Initial Release (libcurl 7.16.4)"

?Not win32
ModuleInfo "CC_OPTS: -DHAVE_CONFIG_H -DSIZEOF_INT=4 -DSIZEOF_SHORT=2 -DSIZEOF_SIZE_T=4"
?win32
ModuleInfo "CC_OPTS: -DHAVE_GETTIMEOFDAY -DCURL_DISABLE_LDAP"
?
ModuleInfo "CC_OPTS: -DCURL_STATICLIB -DCARES_STATICLIB"

' NOTES :
'
' Renamed ares/inet_ntop.c to area/ares_inet_ntop.c because of name clash with libcurl
'
' Added extra options to config_win32.h
' Added __GNUC__ test to ares/setup.h - for GCC 4+ compilation
'

Import "curlmain.bmx"
