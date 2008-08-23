#    This file is part of the xbase64 libraries
#    Copyright (C) 1998 Denis Pershin (dyp@inetlab.com)
#
#
#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with this library; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
#    Contact:
#
#      Email:
#
#        xbase64-dev@lists.sourceforge.net
#        xbase64-users@lists.sourceforge.net
#
#

INCLUDES = -I$(topdir)

lib_LTLIBRARIES = libxbase64.la

pkginclude_HEADERS = xbdbf.h xbexp.h xbndx.h xbretcod.h xbase64.h xbdate.h \
	xbtypes.h xbstring.h xbindex.h xbntx.h xbconfig.h xbfilter.h            \
	xblock.h xbfile.h xbcdx.h xbwincfg.h xbmindex.h xbnode.h

#install-data-hook:
#	(cd $(includedir); rm -f xbase64.h; ln -s xbase64/xbase64.h xbase64.h)

libxbase64_la_SOURCES = xbdbf.cpp xbexp.cpp xbexpfnc.cpp xbexpprc.cpp      \
	xbfields.cpp xbmemo.cpp xbndx.cpp xbase64.cpp xbdate.cpp                \
	xbstring.cpp xbindex.cpp xbntx.cpp xbfilter.cpp xblock.cpp              \
	xbfile.cpp xbcdx.cpp xbnode.cpp

EXTRA_DIST = makebcc.bat
#	makefile.g95 \
#	makebcc.bat \
#	xbase.ide


libxbase64_la_LDFLAGS = -version-info 1:0:0 
libxbase64_la_LIBADD = 

MAINTAINERCLEANFILES = Makefile.in stamp-h.in
CLEANFILES = *.obj *.BAK *.bak *.tds *.lib compout

