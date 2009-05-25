' Copyright (c) 2007-2009 Bruce A Henderson
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
bbdoc: libcurlSSL
End Rem
Module BaH.libcurlSSL

ModuleInfo "Version: 1.02"
ModuleInfo "Author: Bruce A Henderson"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: (libcurl) 1998 - 2007, Daniel Stenberg"
ModuleInfo "Copyright: (c-ares) 1998 Massachusetts Institute of Technology."
ModuleInfo "Copyright: (Wrapper) 2007-2009 Bruce A Henderson"
ModuleInfo "Modserver: BRL"

ModuleInfo "History: 1.02"
ModuleInfo "History: Added ssh support. Now requires BaH.libssh2."
ModuleInfo "History: Update to c-ares 1.6.0"
ModuleInfo "History: Fixed ResponseCode() not returning correct codes."
ModuleInfo "History: 1.01"
ModuleInfo "History: Update to libcurl 7.18.0"
ModuleInfo "History: Update to c-ares 1.5.1"
ModuleInfo "History: Now nulls internal slist."
ModuleInfo "History: 1.00 Initial Release (libcurl 7.16.4, c-ares 1.4.0)"

?linux
ModuleInfo "CC_OPTS: -DHAVE_CONFIG_H"
?macos
ModuleInfo "CC_OPTS: -DHAVE_CONFIG_H"
?win32
ModuleInfo "LD_OPTS: -L%PWD%/ssl/lib"
ModuleInfo "CC_OPTS: -DHAVE_GETTIMEOFDAY -DCURL_DISABLE_LDAP"
?
ModuleInfo "CC_OPTS: -DCURL_STATICLIB"

' NOTES :
'
' Renamed ares/inet_ntop.c to area/ares_inet_ntop.c because of name clash with libcurl
'
' Added extra options to config_win32.h
' Added __GNUC__ test to ares/setup.h - for GCC 4+ compilation
'

Import "curlmain.bmx"
