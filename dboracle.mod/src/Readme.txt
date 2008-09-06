****************************************************************************
 
                 OCILIB Copyright (c) 2007-2008 Vincent Rogier

                           (WINDOWS PACKAGE)

****************************************************************************

DESCRIPTION 
===========

OCILIB is a ISO C encapsulation of the Oracle OCI API

This library encapsulates OCI complexity around a more simple, readable and reusable API.
 
VERSION 
========

2.5.1


FEATURES 
========
   
 * - Data binding
 * - Integrated smart define and fetch mecanisms
 * - FULL Unicode support on all platorms
 * - Multi row fetching
 * - Binding array Interface for fast and massive bulk operations
 * - Reusable Statements
 * - Global Transactions
 * - Connection Pooling
 * - Returning DML feature support
 * - ROWIDs support
 * - Named Types (Object types) support (User or Builtin)
 * - Cursors
 * - PL/SQL Tables
 * - PL/SQL blocks
 * - PL/SQL Ref cursors and Nested tables
 * - LOB (BLOBs/ FILEs)
 * - Supports lobs > 4Go
 * - Long datatype (piecewise operations)
 * - Provides "All in one" Formatted functions (printf's like)
 * - Smallest memory usage possible
 * - Date/time management
 * - Timestamps and Intervals support 
 * - Error handling
 * - Describe database schema objects
 * - Access columns by index or name
 * - Hash tables API
 * - Portable Threads and mutexes API
 * - Supports static / shared oracle linkage
 * - Can be compile as a static lib or a shared lib
 * - Support runtime loading (no OCI libs required at compile / time)
 * - Great performances (straight OCI encapsulation)

DOWNLOAD
========

Available at Sourceforge.net:

URL : http://orclib.sourceforge.net

LICENSE
=======

The source code is free source code.

It's licensed under GNU LESSER GENERAL PUBLIC LICENSE (LGPL).

(see the file lgpl-license.txt)

COMPATIBILITY 
=============

This current source is pure ISO C and should compile with any C90 Compliant compiler
for ansi version and C99 for unicode versions.
 
 Validated Platforms : 
 
 - Windows 
 - UP/UX 
 - Linux 
 - Solaris 
 - AIX 
 - Mac OSX
 - OpenVMS

 Validated Compilers : 

 - GCC / MinGW
 - MS Compilers 
 - XLC  
 - CCs
 - LabView 

 Validated Oracle versions : 
 
 - Oracle 8i 
 - Oracle 9i 
 - Oracle 10g 
 - Oracle 11g 


INSTALLATION (WINDOWS)
============

Precompiled OCILIB libraries are provided for Microsoft Windows :

     - ocilib\lib32 : 32bits binairies
     - ocilib\lib64 : 64bits binairies

Two methods :

1/
     - add the ocilib\src directory to your include's path
     - add the ocilib\lib32|64 directory to your libraries path
     - add the ocilib\lib32|64 directory to your windows path

2/
     - copy  ocilib\inc\ocilib.h to any place located in your include's path
     - copy  ocilib\lib32|64\ocilib[x].lib to any place located in your libraries path
     - copy  ocilib\lib32|64\ocilib[x].dll to any place located in your windows path
 
LIBRARY BUILDS (WINDOWS)
==============

OCILIB supports 3 builds options :

1/ oracle import (linkage or runtime loading)

On win32, the provided build were compiled using is runtime loading.
If you want to import Oracle OCI at linkage time , you need to rebuild the libraries

Defines :

     - OCI_IMPORT_LINKAGE for linkage at compile time
     - OCI_IMPORT_RUNTIME for runtime loading

if no import mode is set, the library sets the default to OCI_IMPORT_LINKAGE

2/ charset (ansi, unicode, mixed)

this pakcages provides 3 builds of OCILIB :

      - ociliba.dll (ANSI version)
      - ocilibw.dll (UNICODE/UFT16/USC2/wchar_t version)
      - ocilibm.dll (ANSI for meta data, UNICODE for user data - often used with Oracle 8i)     

Defines :

     - OCI_CHARSET_ANSI for ansi
     - OCI_CHARSET_UNICODE for unicode
     - OCI_CHARSET_MIXED for third mixed mode (often used with Oracle 8i)

if no charset mode is set, the library sets the default to OCI_CHARSET_ANSI 

3/ calling convention

On win32, the default calling convention is __sdtcall.
So the OCILIB function using a variable number or parameters (OCI_xxxxFmt) won't work
If you need them, modify the preprocessor options to change OCI_API to __cdecl and rebuild !

Defines :

     - OCI_API =  __sdtcall to link OCILIB to any langage independant module
     - OCI_API = __cdecl (or no value) for C/C++ only !

if no charset mode is set, the library sets the default to no value


To reduild the libraries :

      - open the dll project ocilib\proj\dll\ocilib_dll_vs200x.sln
      - change preprocessors options in project settings
      - do a batch build

BUILD DEMO (WINDOWS) 
==========

1/ run an already built demo

     - run ocilib_demo.exe located in ocilib\proj\test\Win32|64\release [build]

2/ build a demo

     - open the test project ocilib\proj\test\ocilib_demo_vs200x.sln
     - choose an configuration
     - build

MAKE ANY PROGRAM WITH OCILIB (WINDOWS)
============================

1/ use the libraries

     - add ocilib[x].lib to your linking options (linker option or by a pragma - see demo source code)
     - add OCI_API macro definition to your preprocessor options (usually __stdcall)

2/ link source code

     - add ocilib.c to your project and defines 3 build options

FEED BACK, BUG REPORTS 
======================

URL : http://sourceforge.net/projects/orclib

DOCUMENTATION (WIN32)
=============

Included in the archive  : ocilib\doc\html folder (index.html)

Online URL : http://orclib.sourceforge.net/doc/index.html

USE OF OCILIB
=============

Please, Let me know about projects using OCILIB.

Send me a mail at <vince.rogier@gmail.com> with a brief description of your project

Thanks.



