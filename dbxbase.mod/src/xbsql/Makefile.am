DEBUG			= -g
CC			= g++
CFLAGS			= -UNO_READLINE -I/usr/local/include $(DEBUG)
CXXFLAGS		= -UNO_READLINE -I/usr/local/include $(DEBUG)
LDFLAGS			= $(DEBUG)
INCLUDE			= -I/usr/local/include

lib_LTLIBRARIES		= libxbsql.la
bin_PROGRAMS		= xql

libxbsql_la_SOURCES	= xb_value.cpp xb_elist.cpp xb_enode.cpp \
			  xb_queryset.cpp xb_tablelist.cpp \
			  xb_select.cpp xb_table.cpp xbsql.cpp \
			  xb_tableset.cpp xb_fieldset.cpp \
			  xb_query.cpp xb_insert.cpp xb_fieldlist.cpp \
			  xb_field.cpp xb_update.cpp xb_delete.cpp \
			  xb_assignlist.cpp xb_create.cpp \
			  xbsql.tab.c xb_datetime.cpp

libxbsql_la_LDFLAGS	= -lxbase $(DEBUG)

xql_SOURCES		= xql.cpp
xql_LDADD		= -lxbase -lreadline -l$(CURSES) ./libxbsql.la

#
#
xbsql.tab.o:		xbsql.tab.c lex.yy.c
			$(CC) $(CFLAGS) -c xbsql.tab.c

xbsql.tab.c:		xbsql.y
			bison -p xbsql_ xbsql.y

lex.yy.c:		xbsql.l
			flex xbsql.l

#install-data-hook:
#			$(mkinstalldirs) $(includedir)
#			$(INSTALL_DATA) xbsql.h $(includedir)

include_HEADERS	= xbsql.h
