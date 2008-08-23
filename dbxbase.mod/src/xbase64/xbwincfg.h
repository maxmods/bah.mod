/* config file for windows environments */

/* Name of package */
#define PACKAGE "xbase64"

/* Version number of package */
#define VERSION "3.1.2"

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define if you have io.h */
#define HAVE_IO_H 1

/* Define if you need to have .ndx indexes */
#define XB_INDEX_NDX 1

/* Define if you need to have .ntx indexes */
#define XB_INDEX_NTX 1

/* Define if you need to have .cdx indexes */
#define XB_INDEX_CDX 1

/* Define if you need to support memo fields */
#define XB_MEMO_FIELDS 1

/* Define if you need expressions */
#define XB_EXPRESSIONS 1

/* Define if you need locking support */
#undef XB_LOCKING_ON 

/* Define if you need to turn on XBase specific debug */
#define XBASE_DEBUG 1

/* Define if using real deletes */
#define XB_REAL_DELETE 1

/* Define if need filters */
#define XB_FILTERS 1

/* Define if you have the fcntl function.  */
#define HAVE_FCNTL 1

/* Define if you have the vsnprintf function.  */
//#define HAVE_VSNPRINTF 1

/* Define if you have the vsprintf function.  */
#define HAVE_VSPRINTF 1

/* Define if you have the <ctype.h> header file.  */
#define HAVE_CTYPE_H 1

/* Define if you have the <fcntl.h> header file.  */
#define HAVE_FCNTL_H 1

/* Define if you have the <string.h> header file.  */
#define HAVE_STRING_H 1

/* Should we include generic index support? */
#if defined(XB_INDEX_NDX) || defined(XB_INDEX_NTX)
#define  XB_INDEX_ANY 1
#endif

/* expressions required for indexes */
#if defined(XB_INDEX_ANY) && !defined(XB_EXPRESSIONS)
#define XB_EXPRESSIONS 1
#endif

/* default memo block size */
#define XB_DBT_BLOCK_SIZE  512

/* filename path separator */
#define PATH_SEPARATOR '/'

/* MS uses WIN32, Borland uses __WIN32__ */
#ifdef WIN32
 #ifndef __WIN32__
  #define __WIN32__
 #endif 
#endif

