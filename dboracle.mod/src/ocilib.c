/*
   +----------------------------------------------------------------------+   
   |                 OCILIB - C Wrapper for Oracle OCI                    |
   +----------------------------------------------------------------------+
   |              Website : http://orclib.sourceforge.net                 |
   +----------------------------------------------------------------------+
   |               Copyright (c) 2007-2008 Vincent ROGIER                 |
   +----------------------------------------------------------------------+
   | This library is free software; you can redistribute it and/or        |
   | modify it under the terms of the GNU Library General Public          |
   | License as published by the Free Software Foundation; either         |
   | version 2 of the License, or (at your option) any later version.     |
   |                                                                      |
   | This library is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU    |
   | Library General Public License for more details.                     |
   |                                                                      |
   | You should have received a copy of the GNU Library General Public    |
   | License along with this library; if not, write to the Free           |
   | Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.   |
   +----------------------------------------------------------------------+
   |          Author: Vincent ROGIER <vince.rogier@gmail.com>             |
   +----------------------------------------------------------------------+ 
*/

/* ------------------------------------------------------------------------ *
 * $Id: ocilib.c, v 2.5.1 00:21 24/07/2008 Vince $
 * ------------------------------------------------------------------------ */

/* public declarations */

#include "ocilib.h"

/*
    those lines are unfortunatly necessary to avoid the redefinition of the 
    boolean macro type on Microsoft Windows platform's !
*/

#ifdef _WINDOWS
  #ifdef boolean
    #undef boolean
  #endif  
  #include <windows.h>
#endif

/* private import of oracle declarations */

#include "oci_import.h"

/* ------------------------------------------------------------------------ *
 * Guess Oracle version
 * ------------------------------------------------------------------------ */

#ifdef OCI_IMPORT_RUNTIME

  /* for runtime loading, set compile time version to the highest minimum
     version needed by OCILIB encapsulation of OCI */
  #define OCI_VERSION_COMPILE OCI_10
  /* set runtime version to unknown, it will be guessed from symbols loading */
  #define OCI_VERSION_RUNTIME OCI_UNKNOWN

#else

  /* oracle 10g introduced in oci.h this constant, so it means >= 10g */
  #ifdef OCI_MAJOR_VERSION

    #define OCI_VERSION_COMPILE OCI_MAJOR_VERSION
    #define OCI_VERSION_RUNTIME OCI_MAJOR_VERSION

  #elif defined(SQLT_PNTY)

    /* if version < 10g and if SQLT_PNTY is defined, it means 9i */
    #define OCI_VERSION_COMPILE OCI_9
    #define OCI_VERSION_RUNTIME OCI_9

  #else

    /* so it must be 8i */
    #define OCI_VERSION_COMPILE OCI_8
    #define OCI_VERSION_RUNTIME OCI_8

  #endif

#endif

/* tries to enable Oracle 10g support for lobs > 4Go with OCILobxxx2() calls */

#if defined(OCI_BIG_UINT_ENABLED) && defined(ORAXB8_DEFINED)
  #define OCI_LOB2_API_ENABLED
#endif

/* ------------------------------------------------------------------------ *
 * Setup internal miscellaneous defines for handling various character sizes
 * ------------------------------------------------------------------------ */

/* mtext and dtext are public character types for meta and user string types
   We need to handle as well internal string types because :
   
   - wchar_t is not the same type on all platforms (that is such a pain !),
   - OCI, in Unicode mode, uses Fixed length UTF16 encoding (2 bytes)
   
   So, omtext and odtext were added to represent internal meta and user string
   types.

   The following checks find out the real types and sizes of omtext and odtext
*/
   
#ifdef OCI_CHARSET_ANSI

  /* easy ! */
  #define omtext mtext
  #define odtext dtext

#else

  #define WCHAR_2_BYTES 0xFFFF
  #define WCHAR_4_BYTES 0x7FFFFFFF

  /* 
     Actually, the only need to use string conversion is when using wchar_t
     on unixes systems. This test will probably change in future releases to
     handle internally UTF8, by example
  */

  #if WCHAR_MAX == WCHAR_4_BYTES
    /* so, input/output conversion will be needed */
    #define OCI_CHECK_STRINGS
  #endif

  #ifdef OCI_METADATA_UNICODE

    #ifdef OCI_CHECK_STRINGS
      /* conversion for meta string needed */
      #define OCI_CHECK_METASTRINGS
    #endif

    /* internal meta string type is UTF16 (2 bytes) */
    #define omtext unsigned short

  #else

    /* internal meta string type is char */
    #define omtext char

  #endif

  #ifdef OCI_USERDATA_UNICODE

    #ifdef OCI_CHECK_STRINGS
      /* conversion for data string needed */
      #define OCI_CHECK_DATASTRINGS
    #endif

    /* internal data string type is UTF16 (2 bytes) */
    #define odtext unsigned short

  #else

    /* internal data string type is char */
    #define odtext char

  #endif

#endif

/* ------------------------------------------------------------------------ *
 * Local constants
 * ------------------------------------------------------------------------ */

/* statement status */

#define OCI_STMT_CLOSED                 0
#define OCI_STMT_PREPARED               1
#define OCI_STMT_EXECUTED               2

/* connection states */

#define OCI_CONN_ALLOCATED              0
#define OCI_CONN_ATTACHED               1
#define OCI_CONN_LOGGED                 2

/* memory status */

#define OCI_OBJECT_ALLOCATED            0
#define OCI_OBJECT_FETCHED_CLEAN        1
#define OCI_OBJECT_FETCHED_DIRTY        2

/* bind type */

#define OCI_BIND_INPUT                  0
#define OCI_BIND_OUTPUT                 1

/* integer type */


#define OCI_NUM_UNSIGNED               2
#define OCI_NUM_SHORT                  4
#define OCI_NUM_INT                    8
#define OCI_NUM_BIGINT                 16
#define OCI_NUM_NUMBER                 32

#define OCI_NUM_USHORT                 OCI_NUM_SHORT  | OCI_NUM_UNSIGNED
#define OCI_NUM_UINT                   OCI_NUM_INT    | OCI_NUM_UNSIGNED
#define OCI_NUM_BIGUINT                OCI_NUM_BIGINT | OCI_NUM_UNSIGNED

/* output buffer server line size */

#define OCI_OUPUT_LSIZE     255
#define OCI_OUPUT_LSIZE_10G 32767

/* number of server version fields */

#define OCI_NB_ARG_VERSION              3

/* OCI unicode flag */

#ifndef OCI_UTF16ID
  #define OCI_UTF16ID                   OCI_UCS2ID
#endif

/* local mapping macros */

#ifdef OCI_METADATA_UNICODE
  #define mttoupper           towupper
  #define mtisdigit           iswdigit
  #define mtsscanf            swscanf
#else
  #define mttoupper           toupper
  #define mtisdigit           isdigit
  #define mtsscanf            sscanf
#endif

/* unicode constants */

#define UNI_SHIFT             ((int) 10 )
#define UNI_BASE              ((unsigned long) 0x0010000UL)
#define UNI_MASK              ((unsigned long) 0x3FFUL)
#define UNI_REPLACEMENT_CHAR  ((unsigned long) 0x0000FFFD)
#define UNI_MAX_BMP           ((unsigned long) 0x0000FFFF)
#define UNI_MAX_UTF16         ((unsigned long) 0x0010FFFF)
#define UNI_MAX_UTF32         ((unsigned long) 0x7FFFFFFF)
#define UNI_MAX_LEGAL_UTF32   ((unsigned long) 0x0010FFFF)
#define UNI_SUR_HIGH_START    ((unsigned long) 0xD800)
#define UNI_SUR_HIGH_END      ((unsigned long) 0xDBFF)
#define UNI_SUR_LOW_START     ((unsigned long) 0xDC00)
#define UNI_SUR_LOW_END       ((unsigned long) 0xDFFF)

/* ------------------------------------------------------------------------ *
 * Local types
 * ------------------------------------------------------------------------ */

/*
 * OCI_Item : Internal list entry.
 *
 * The library needs to manage internal list of objects in order to be able to
 * free them if the application doest not.
 * 
 * @note
 * Internal lists are using mutexes for resource locking in multithreaded
 * environments
 * 
 */

typedef struct OCI_Item {
   void             *data;  /* pointer to external data */
   struct OCI_Item  *next;  /* next element in list */
} OCI_Item;

/*
 * OCI_List : Internal list object.
 * 
 * The OCI_List object is used to maintain a collection of handles allocated
 * by user programs.
 * 
 * Those handles are freed when the collection owner is destroyed.
 * So, we make sure that if OCI_Cleanup() is called, all allocated handles will
 * be destroyed even if the program does not free them.
 * 
 */

typedef struct OCI_List {
    OCI_Item    *head;   /* pointer to first item */
    OCI_Mutex   *mutex;  /* mutex handle */
    int          count;  /* number of elements in list */
} OCI_List;

/* 
 * Server ouput object used to retreive server dbms.output buffers 
 * 
 */

typedef struct OCI_ServerOutput {
    ub1                *arrbuf;    /* array buffer */
    unsigned int        arrsize;   /* array size */
    unsigned int        cursize;   /* number of filled items in the array */
    unsigned int        curpos;    /* current position in the array */
    unsigned int        lnsize;    /* line size */
    OCI_Statement      *stmt;      /* pointer to statement object (dbms_output calls) */
} OCI_ServerOutput;

/* 
 * Error object
 * 
 */

struct OCI_Error {
    OCI_Connection  *con;                   /* pointer to connection object */
    OCI_Statement   *stmt;                  /* pointer to statement object */
    int              oci_code;              /* Oracle OCI error code */
    int              int_code;              /* OCILIB internal error code */ 
    mtext            str[OCI_SIZE_BUFFER];  /* error message */
    int              type;                  /* OCILIB error type */
};

/* 
 * Mutex object 
 *
 * Mutexes have their own error handle to avoid conflict using OCIErrorGet()
 * from differents threads
 *
 */

struct OCI_Mutex {
    OCIThreadMutex  *handle;  /* OCI Mutex handle */
    OCIError        *err;     /* OCI Error handle */
    sword            status;  /* current error status */
};

/*
 * Thread object
 *
 * Threads have their own error handle to avoid conflict using OCIErrorGet()
 *
 */

struct OCI_Thread {
    OCIThreadHandle *handle;    /* OCI Thread handle */
    OCIThreadId     *id;        /* OCI Thread ID */
    OCIError        *err;       /* OCI Error handle */
    void            *arg;       /* thread routine argument */
    POCI_THREAD      proc;      /* thread routine */
    sword            status;    /* current error status */
};

/* 
 * Thread key object 
 *
 * Thread keys have their own error handle to avoid conflict using OCIErrorGet()
 * from differents threads
 *
 */

typedef struct OCI_ThreadKey {
    OCIThreadKey    *handle;  /* OCI Thread key handle */
    OCIError        *err;     /* OCI Error handle */
    sword            status;  /* current error status */
} OCI_ThreadKey;

/* 
 * OCI_Library : Internal OCILIB library encapsulation.
 *
 * It's a static, local and unique object that collects all the global variables
 * needed by the library
 * 
 */

typedef struct OCI_Library {
    OCI_List       *cons;                   /* list of connection objects */
    OCI_List       *pools;                  /* list of pools objects */
    OCIEnv         *env;                    /* OCI environnement handle */
    OCIError       *err;                    /* OCI error handle */
    POCI_ERROR      error_handler;          /* user defined error handler */
    mtext           path[OCI_SIZE_BUFFER];  /* Oracle shared lib path */
    int             ver_compile;            /* OCI version used at compile time */
    int             ver_runtime;            /* OCI version used at runtime */
    int             env_mode;               /* default environnement mode */
    boolean         use_lob_ub8;            /* use 64 bits integers for lobs ? */
    OCI_HashTable  *key_map;                /* hash table for mapping name/key */
#ifdef OCI_IMPORT_RUNTIME
    LIB_HANDLE      lib_handle;             /* handle of runtime shared library */
#endif
} OCI_Library;

/* 
 * Connection Pool object 
 *
 */

struct OCI_ConnPool {
    OCI_List        *cons;      /* list of connection objects */
#if OCI_VERSION_COMPILE >= OCI_9
    OCICPool        *handle;    /* OCI pool handle */
#else
    void            *handle;    /* fake handle for aligment */
#endif
    OCIError        *err;       /* OCI context handle */
    mtext           *name;      /* pool name */
    mtext           *db;        /* database */
    mtext           *user;      /* user */
    mtext           *pwd;       /* password */
    OCI_Mutex       *mutex;     /* mutex handle */
    int              mode;      /* session mode */
    int              min;       /* minimum of connections */
    int              max;       /* maximum of connections */
    int              incr;      /* increment step of connections */
    sword            status;    /* current error status */
    int              nb_busy;   /* number of busy connections */
    int              nb_opened; /* number of opened connections */
    int              timeout;   /* connection idle timeout */
    boolean          nowait;    /* wait for new connection */
};

/* 
 * Connection object 
 *
 */

struct OCI_Connection {
    mtext              *db;        /* database */
    mtext              *user;      /* user */
    mtext              *pwd;       /* password */
    mtext              *version;   /* server version */
    OCI_List           *stmts;     /* list of statements */
    OCI_List           *trsns;     /* list of transactions */
    OCI_List           *sobjs;     /* list of schema objects */
    OCI_Transaction    *trs;       /* pointer to current transaction object */
    OCI_ConnPool       *pool;      /* pointer to connection pool parent */
    OCI_ServerOutput   *svopt;     /* Pointer to server output object */
    OCIServer          *svr;       /* OCI server  handle */
    OCIError           *err;       /* OCI context handle */
    OCISession         *ses;       /* OCI session handle */
    OCISvcCtx          *cxt;       /* OCI context handle */
    sword               status;    /* current error status */
    boolean             autocom;   /* auto commit mode */
    int                 nb_files;  /* number of OCI_File opened by the connection */
    int                 ver_maj;   /* server version major number */
    int                 ver_min;   /* server version minor number */
    int                 ver_rev;   /* server version revision number */
    int                 mode;      /* session mode */
    boolean             available; /* pooling availabality */
    int                 cstate;    /* connection state */
    const void         *usrdata;   /* user data */
};

/* 
 * Transaction object
 *
 */

struct OCI_Transaction
{
    OCI_Connection  *con;       /* pointer to connection object */
    OCITrans        *htr;       /* OCI transaction handle */
    int              timeout;   /* timeout */
    int              mode;      /* transaction mode */
    boolean          local;     /* is local transaction ? */
    OCI_XID          xid;       /* global transaction identitifer */
};

/* 
 * Column object
 *
 */

struct OCI_Column {

    /* 0racle infos */ 
    ub2              ocode;     /* Oracle SQL code */
    ub2              icode;     /* Internal translated SQL code */
    ub2              size;      /* SQL Size */
    ub1              prec;      /* SQL precision 1 (number prec, leading prec) */
    ub1              prec2;     /* SQL precision 2 (fractionnal prec) */
    sb1              scale;     /* SQL scale */
    ub1              type;      /* internal datatype */
    boolean          null;      /* is nullable */
    boolean          charused;  /* is column size expressed in characters */
    ub2              charsize;  /* SQL Size in character */
    mtext           *name;      /* column name */
    ub1              csfrm;     /* charset form */

    /* OCILIB infos */ 
    ub1              subtype;   /* object type */
    ub1              dtype;     /* oracle handle type */
    ub4              bufsize;   /* element size */
    OCI_Schema       *nty;      /* user type descriptor */ 
};

/* 
 * OCI_Buffer : Internal input/output buffer
 *
 */

typedef struct OCI_Buffer {
    void            *handle;   /* OCI handle (bind or define) */
    void           **data;     /* data / array of data */
    void            *inds;     /* array of indicators */
    void            *lens;     /* array of lengths */
    dtext           *temp;     /* temporary buffer for string conversion */
    int              count;    /* number of elements in the buffer */
    int              sizelen;  /* size of an element in the lens array */
} OCI_Buffer;

/* 
 * OCI_Bind : Internal bind representation.
 *
 * A bind object is an object that holds all information about an Oracle
 * statement binding operation
 *
 */

typedef struct OCI_Bind {
    OCI_Statement   *stmt;      /* pointer to statement object */
    void           **input;     /* input values */
    mtext           *name;      /* name of the bind */
    ub1              mode;      /* in / out mode */
    ub1              type;      /* internal datatype */
    ub1              subtype;   /* internal subtype */
    ub2              code;      /* SQL datatype code */
    ub4              size;      /* data size */
    ub1              alloc;     /* is buffer allocated or mapped to input */
    OCI_Buffer       buf;       /* place holder */
    ub2              dynpos;    /* index of the bind for dynamic binds */
    ub2             *plsizes;   /* PL/SQL tables element sizes */
    ub2             *plrcds;    /* PL/SQL tables return codes */
    ub4              plnbelem;  /* PL/SQL tables nb elements */
} OCI_Bind;

/*
 * OCI_Define : Internal Resultset column data implementation
 *
 */

typedef struct OCI_Define {
    OCI_Resultset   *rs;           /* pointer to resultset object */
    void            *obj;          /* current OCILIB object instance */
    OCI_Column       col;          /* column object */
    OCI_Buffer       buf;          /* placeholder */
} OCI_Define;

/* 
 * Resultset object
 *
 */

struct OCI_Resultset {
    OCI_Statement   *stmt;          /* pointer to statement object */
    OCI_HashTable   *map;           /* hash table handle for mapping name/index */
    OCI_Define      *defs;          /* array of define objects */
    int              nb_defs;       /* number of elements */
    ub4              row_mem;       /* actual position in the array of rows */
    ub4              row_abs;       /* absolute position in the resulset */
    ub4              row_count;     /* number of rows fetched so far */
    boolean          eof;           /* end of resultset reached ?  */
    int              fetch_size;    /* internal array size */
};

/*
 * Statement object 
 *
 */

struct OCI_Statement {
    OCIStmt         *stmt;          /* OCI statement handle */
    ub1              hstate;        /* object variable state */
    OCI_Resultset  **rsts;          /* pointer to resultset list */
    OCI_Connection  *con;           /* pointer to connection object */
    mtext           *sql;           /* SQL statement */
    mtext           *fmt_date;      /* format date for conversion */
    OCI_Bind       **binds;         /* array of bind objects */
    int              nb_binds;      /* number of elememts in the bind array */
    int              nb_outbinds;   /* number of output binds */
    sword            bind_mode;     /* type of binding */
    ub2              fetch_mode;    /* type of fetch */
    ub4              fetch_size;    /* fetch array size */
    ub4              prefetch_size; /* prefetch size */
    ub4              prefetch_mem;  /* prefetch memory */
    ub4              long_size;     /* default size for LONG columns */
    ub4              long_mode;     /* LONG datatype handling mode */
    int              status;        /* statement status */
    ub2              type;          /* type of SQL statement */
    ub4              nb_iters;      /* number of iterations for execution */
    ub4              nb_rs;         /* number of resulsets */
    ub4              cur_rs;        /* index of the current resulset */
    ub1              piecewise;     /* piecewise fetching ? */ 
    ub2              dynidx;        /* bind index counter for dynamic exec */
};

/*
 * Internal Large object 
 *
 */

struct OCI_Lob {
    OCILobLocator   *handle;    /* OCI handle */
    ub1              hstate;    /* object variable state */
    OCI_Connection  *con;       /* pointer to connection object */
    ub1              type;      /* type of lob */
    big_uint         offset;    /* current offset for R/W */
};

/* 
 * External Large object 
 *
 */

struct OCI_File {
    OCILobLocator   *handle;    /* OCI handle */
    ub1              hstate;    /* object variable state */
    OCI_Connection  *con;       /* pointer to connection object */
    ub1              type;      /* type if file */
    big_uint         offset;    /* current offset for read */
    mtext           *dir;       /* directory name */
    mtext           *name;      /* file name */
};

/* 
 * Long object 
 *
 */ 

struct OCI_Long {
    OCI_Statement   *stmt;      /* pointer to statement object */
    ub1              hstate;    /* object variable state */
    OCI_Define      *def;       /* pointer to resultset define object */
    ub1             *buffer;    /* fetched buffer */
    ub4              size;      /* size of the buffer read / written */
    int              type;      /* type of long */
    ub4              offset;    /* current offset for R/W */
    ub4              piecesize; /* size of current fetched piece */
};

/*
 * Date object 
 *
 */

struct OCI_Date {
    OCIDate         *value;     /* OCI handle */
    ub1              hstate;    /* object variable state */
    ub1              alloc;     /* is value allocated */ 
    OCI_Connection  *con;       /* pointer to connection object */
    sword            status;    /* current error status */
    OCIError        *err;       /* OCI context handle */

};

/* 
 * Timestamp object 
 *
 */

struct OCI_Timestamp {
#if OCI_VERSION_COMPILE >= OCI_9
    OCIDateTime     *handle;    /* OCI handle */
#else
    void            *handle;    /* fake handle for alignment */
#endif
    ub1              hstate;    /* object variable state */
    OCI_Connection  *con;       /* pointer to connection object */
    ub1              type;      /* sub type */
    sword            status;    /* current error status */
    OCIError        *err;       /* OCI context handle */
};

/* 
 * Interval object 
 *
 */

struct OCI_Interval {
#if OCI_VERSION_COMPILE >= OCI_9
    OCIInterval     *handle;    /* OCI handle */
#else
    void            *handle;    /* fake handle for alignment */
#endif
    ub1              hstate;    /* object variable state */
    OCI_Connection  *con;       /* pointer to connection object */
    ub1              type;      /* sub type */
    sword            status;    /* current error status */
    OCIError        *err;       /* OCI context handle */
};

/*
 * Oracle Named type object 
 *
 */

struct OCI_Object { 
    void            *handle;   /* OCI handle */
    ub1              hstate;   /* object variable state */
    OCI_Connection  *con;      /* pointer to connection object */
    OCI_Schema      *nty;      /* pointer schema object type */
    void           **objs;     /* array of sub objects handles */
    void            *buf;      /* buffer to store converted out string attribute */ 
};

/* 
 * Schema object 
 *
 */

struct OCI_Schema {
    OCIDescribe     *handle;    /* OCI handle */
    OCI_Connection  *con;       /* pointer to connection object */
    OCI_Variant      items;     /* array of sub items */
    mtext           *name;      /* name of the schema object */
    ub2              nb_items;  /* number of items in the array */
    OCIType         *tdo;       /* datatype object type */
    int              type;      /* type of schema handle */
};

/* 
 * Hash table object 
 *
 */

struct OCI_HashTable {
    OCI_HashEntry    **items;     /* array of slots */
    unsigned int       size;      /* size of the slots array */
    unsigned int       count;     /* number of used slots */
    int                type;      /* type of data */
};

/*
 * OCI_Datatype : fake dummy structure for casting object with 
 * handles for more compact code
 *
 */

typedef struct OCI_Datatype
{
    void *handle;   /* OCI handle */
    ub1   hstate;   /* object variable state */
} OCI_Datatype;

/* ------------------------------------------------------------------------ *
 * Local helper macros
 * ------------------------------------------------------------------------ */

/* check OCI status */

#define OCI_NO_ERROR(obj)   (obj->status == OCI_SUCCESS ||  \
                             obj->status == OCI_SUCCESS_WITH_INFO)

/* check OCI calls */

#define OCI_CALL(obj , fct)      \
                                 \
    {                            \
        if( OCI_NO_ERROR(obj))   \
            (obj)->status = fct; \
    }

#define OCI_DIRECT(obj , fct)    \
                                 \
    {                            \
        (obj)->status = fct;     \
    }

/* various check macros */

#define OCI_CHECK(exp, ret)             if ((exp) == 1   ) return (ret);
#define OCI_CHECK_PARAM(p)              if ((p  ) == NULL) return;

#define OCI_CHECK_SUCCESS(exp, ret)     OCI_CHECK(((exp) != OCI_SUCCESS), ret)
#define OCI_CHECK_NUM(val, ret)         OCI_CHECK(((val) == 0          ), ret)
#define OCI_CHECK_PTR(val, ret)         OCI_CHECK(((val) == NULL       ), ret)
#define OCI_CHECK_BOOL(val, ret)        OCI_CHECK(((val) == FALSE      ), ret)
#define OCI_CHECK_STRING(p)             ((p != NULL) && ((p)[0]) != 0)

#define OCI_CHECK_VERSION(con, ver, ret)                                       \
                                                                               \
    if (OCILib.ver_runtime < ver || (((con) != NULL) && (con)->ver_maj < ver)) \
    {                                                                          \
        OCI_ErrorVersion(con, NULL, ver);                                      \
        return ret;                                                            \
    }

#define OCI_CHECK_ERROR(con, stmt, ret)         \
                                                \
    if((OCI_NO_ERROR(con)) == FALSE)            \
    {                                           \
        OCI_ErrorAPI((con)->err, con, stmt);    \
        return ret;                             \
    }

#define OCI_CHECK_ERROR2(obj, con, stmt, ret)   \
                                                \
    if((OCI_NO_ERROR(obj)) == FALSE)            \
    {                                           \
        OCI_ErrorAPI((obj)->err, con, stmt);    \
        return ret;                             \
    }

#define OCI_CHECK_STATUS(obj, ret)              \
                                                \
    if((OCI_NO_ERROR(obj)) == FALSE)            \
    {                                           \
        OCI_ErrorAPI((obj)->err, NULL, NULL);   \
        return ret;                             \
    }

/* this should please our C++ friends ! */

#ifdef __cplusplus
  #define OCI_CAST(t) static_cast<t> 
#else
  #define OCI_CAST(t) 
#endif
  
/* memory management helpers */

#define OCI_ALLOC(type, ptr, size, bytes)      \
                                               \
    {                                          \
        (size) = (bytes);                      \
        (ptr)  = OCI_CAST(type) (malloc(size));\
        if ((ptr))                             \
             memset((ptr), 0, (size));         \
    }

#define OCI_REALLOC(type, ptr, size, bytes)          \
                                                     \
    {                                                \
        type tmp;                                    \
        (size) = (bytes);                            \
        (tmp)  = OCI_CAST(type) (realloc(ptr, size));\
         if ((tmp))                                  \
              (ptr) = (type) tmp;                    \
         else                                        \
         {                                           \
             OCI_FREE(ptr);                          \
         }                                           \
    }

#define OCI_FREE(ptr)                   free(ptr), ptr = NULL;

/* indicator and nullity handlers */

#define OCI_IND(exp)                    (sb2) ((exp) ? 0 : -1)

#define OCI_NOT_NULL(def)  \
    ((sb2) ((sb2*)def->buf.inds)[rs->row_mem] != -1)

#define OCI_NOT_USED(p)                 (p) = (p);

/* versions checks */

#define OCI_VERSION_LIB_OK(ver)         (OCILib.ver_runtime >= ver)                    
                                 
#define OCI_VERSION_CON_OK(con, ver)    (OCILib.ver_runtime >= ver &&          \
                                         con->ver_maj       >= ver)              

#define OCI_LIB_THREADED                (OCILib.env_mode & OCI_ENV_THREADED)

#define OCI_RUNTIME_LOB2_API            (OCI_VERSION_LIB_OK(OCI_10) &&         \
                                         sizeof(big_uint) > sizeof(int)

/* ------------------------------------------------------------------------ *
 * Local variables
 * ------------------------------------------------------------------------ */

/* static and unique OCI_Library object */

static OCI_Library OCILib;

/* ------------------------------------------------------------------------ *
 * Local functions
 * ------------------------------------------------------------------------ */

/* library */

static void OCI_KeyMapFree(void);

/* lists */

static OCI_List * OCI_ListCreate(void);
static boolean    OCI_ListFree(OCI_List *list);
static OCI_Item * OCI_ListCreateItem(int size);
static OCI_Item * OCI_ListAppend(OCI_List *list, int size);
static boolean    OCI_ListClear(OCI_List *list);
static boolean    OCI_ListForEach(OCI_List *list, boolean (*proc)(void *));
static boolean    OCI_ListRemove(OCI_List *list, void *data);

/* binds */

static boolean OCI_BindFreeAll(OCI_Statement *stmt);
static boolean OCI_BindCheck(OCI_Statement *stmt);
static boolean OCI_BindReset(OCI_Statement *stmt);
static boolean OCI_BindData(OCI_Statement *stmt, void *data, ub4 size,
                            const mtext *name, ub1 type, int code, int mode,
                            int subtype, void *extra, unsigned int nbelem);

/* connections */


static OCI_Connection * OCI_ConnectionAllocate(OCI_ConnPool *pool,
                                               const mtext *db, 
                                               const mtext *user,
                                               const mtext *pwd, 
                                               int mode);

static boolean OCI_ConnectionDeallocate(OCI_Connection *con);
static boolean OCI_ConnectionAttach(OCI_Connection *con);
static boolean OCI_ConnectionLogon(OCI_Connection *con);
static boolean OCI_ConnectionDetach(OCI_Connection *con);
static boolean OCI_ConnectionLogOff(OCI_Connection *con);
static boolean OCI_ConnectionClose(OCI_Connection *con);

/* connection pools */

static boolean OCI_ConnPoolClose(OCI_ConnPool *pool);

/* transactions */

static boolean OCI_TransactionClose(OCI_Transaction * trans);

/* error handling */


static void OCI_ErrorGeneric(OCI_Connection *con, OCI_Statement *stmt, 
                             int type, int code, mtext *format, ...);
static void OCI_ErrorAPI(OCIError *p_err, OCI_Connection *con,
                         OCI_Statement *stmt);
static void OCI_ErrorMemory(OCI_Connection *con, OCI_Statement *stmt,
                            size_t nb_bytes);
static void OCI_ErrorVersion(OCI_Connection *con, OCI_Statement *stmt,
                             int version);

/* statements */

static boolean OCI_StatementReset(OCI_Statement *stmt);
static boolean OCI_StatementClose(OCI_Statement *stmt);

static OCI_Resultset * OCI_ResultsetCreate(OCI_Statement *stmt, int size);

/* resultsets */

static boolean OCI_ResultsetFree(OCI_Resultset *rs);
static boolean OCI_FetchData(OCI_Resultset *rs);
static boolean OCI_FetchIntoUserVariables(OCI_Statement *stmt, va_list args);

#ifdef OCI_CHECK_DATASTRINGS 

static boolean      OCI_ResultsetExpandStrings(OCI_Resultset *rs);

#endif

static OCI_Define * OCI_GetDefine(OCI_Resultset *rs, int index);
static int          OCI_GetDefineIndex(OCI_Resultset *rs, const mtext *name);

/* defines */

static boolean OCI_DefineAlloc(OCI_Define *def);
static boolean OCI_DefineDef(OCI_Define *def);
static void *  OCI_DefineGetData(OCI_Define *def);

/* columns */

static boolean OCI_ColumnMap(OCI_Column *col, OCI_Statement *stmt);
static boolean OCI_ColumnDescribe(OCI_Column *col, OCI_Connection *con,
                                  OCI_Statement *stmt, void *handle, 
                                  int index, boolean check_null);

/* OCILIB objects initializers */

static OCI_Statement * OCI_StatementInit(OCI_Connection *con, 
                                         OCI_Statement **pstmt,
                                         OCIStmt *handle,  
                                         OCI_Define *def);

static OCI_Lob       * OCI_LobInit(OCI_Connection *con, OCI_Lob **plob,
                                   OCILobLocator *handle, ub1 type);
static OCI_Date      * OCI_DateInit(OCI_Connection *con, OCI_Date **pdate,
                                    OCIDate *buffer, boolean alloc, 
                                    boolean is_ansi_date);
static OCI_Long      * OCI_LongInit(OCI_Statement *stmt, OCI_Long **plg,
                                    OCI_Define *def, int type);
static OCI_File      * OCI_FileInit(OCI_Connection *con, OCI_File **pfile,
                                    OCILobLocator *handle, ub1 type);
static OCI_Object    * OCI_ObjectInit(OCI_Connection *con, OCI_Object **pobj,
                                      void *handle, OCI_Schema *schema);
static OCI_Interval  * OCI_IntervalInit(OCI_Connection *con, OCI_Interval **pitv,
                                        OCIInterval *buffer, ub1 type);
static OCI_Timestamp * OCI_TimestampInit(OCI_Connection *con, 
                                         OCI_Timestamp **ptmsp,
                                         OCIDateTime *buffer, ub1 type);

/* Thread keys */

static boolean OCI_ThreadKeyFree(OCI_ThreadKey *key);

/* hash tables */

static unsigned int OCI_HashCompute(OCI_HashTable *table, const mtext *str);
static boolean      OCI_HashAdd(OCI_HashTable *table, const mtext *key, 
                                OCI_Variant value, int type);

/* dynamic bind callbacks */

static sb4 OCI_ProcInBind(dvoid *ictxp, OCIBind *bindp, ub4 iter, ub4 index,
                          dvoid **bufpp, ub4 *alenp,  ub1 *piecep, dvoid **indp);
static sb4 OCI_ProcOutBind(dvoid *octxp, OCIBind *bindp, ub4 iter, ub4 index,
                           dvoid **bufpp, ub4 **alenp, ub1 *piecep, dvoid **indp,
                           ub2 **rcodep);

/* Threads */

static void OCI_ThreadProc(dvoid *arg);

/* schemas */

boolean OCI_SchemaClose(OCI_Schema *schema);

/* files */

static boolean OCI_FileGetInfo(OCI_File *file);

/* Objects (Named types) */

static boolean OCI_ObjectGetAttr(OCI_Object *obj, const mtext *attr, 
                                 void ** p_value, OCIInd* p_ind, 
                                 OCIType **p_tdo);
static boolean OCI_ObjectSetAttr(OCI_Object *obj, const mtext *attr, 
                                 void * value,  OCIInd ind);
static boolean OCI_ObjectSetNumber(OCI_Object *obj, const mtext *attr, 
                                   void *value, int size, ub1 flag);
static boolean OCI_ObjectGetNumber(OCI_Object *obj, const mtext *attr, 
                                   void *value, int size, ub1 flag);
static int     OCI_ObjectGetIndex(OCI_Object *obj, const mtext *attr, int type);

/* numbers */

static boolean OCI_NumberGet(OCI_Connection *con,  OCINumber *data,
                             void *value, int size, uword flag);
static boolean OCI_NumberSet(OCI_Connection *con,  OCINumber *data,
                             void *value, int size, uword flag);

static void *  OCI_IntegerGet(OCI_Connection *con, 
                              void *src_ptr, ub1 src_type, int src_size,
                              void *dst_ptr, ub1 dst_type, int dst_size);

/* unicode / multibytes string handlers */

static long   OCI_StringCopy4to2bytes(const unsigned long* src, long src_size, 
                                      unsigned short* dst, long dst_size);
static long   OCI_StringCopy2to4bytes(const unsigned short* src, long src_size, 
                                      unsigned long* dst, long dst_size);


static void * OCI_GetInputString(void *src, int *size, int size_char_in, 
                                 int size_char_out);
static void   OCI_GetOutputString(void *src, void *dest, int *size,
                                  int size_char_in, int size_char_out);
static void   OCI_ConvertString(void *src, int char_count, int size_char_in,
                                int size_char_out);
static void   OCI_CopyString(void *src, void *dest, int *size, int size_char_in,
                             int size_char_out);
static void   OCI_ReleaseMetaString(void *ptr);
static void   OCI_ReleaseDataString(void *ptr);
static int    OCI_StringLength(void *ptr, int size_elem);


#define OCI_GetInputMetaString(s, n)     OCI_GetInputString((void *) s, n,     \
                                                             sizeof(mtext),    \
                                                             sizeof(omtext))

#define OCI_GetOutputMetaString(s, d, n) OCI_GetOutputString((void *) s, d, n, \
                                                             sizeof(omtext),   \
                                                             sizeof(mtext)) 

#define OCI_GetInputDataString(s, n)     OCI_GetInputString((void *) s, n,     \
                                                             sizeof(dtext),    \
                                                             sizeof(odtext))

#define OCI_GetOutputDataString(s, d, n) OCI_GetOutputString((void *) s, d, n, \
                                                              sizeof(odtext),  \
                                                              sizeof(dtext))
/* formatting */

static int OCI_ParseSqlFmt(OCI_Statement *stmt, mtext *buffer, 
                           const mtext *format, va_list *pargs);

/* ------------------------------------------------------------------------ *
 *                    Charset functions
 * ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ *
 * ocistrdup
 * ------------------------------------------------------------------------ */

char * ocistrdup(const char * src)
{
    char *dst;

    OCI_CHECK_PTR(src, NULL);

    dst = (char *) malloc((strlen(src)+1) * sizeof(char));

    if (dst != NULL)
        strcpy(dst, src);

    return dst;
}

/* ------------------------------------------------------------------------ *
 * ocistrcasecmp
 * ------------------------------------------------------------------------ */

int ocistrcasecmp(const char *str1, const char *str2)
{
    if (str1 == NULL && str2 == NULL)
        return 0;

    if (str1 == NULL)
        return 1;

    if (str2 == NULL)
        return -1;

    while (((*str1) != 0) && 
           ((*str2) != 0) &&
           (tolower((int)(*str1)) == tolower((int)(*str2))))
    {
        str1++;
        str2++;
    }

    return (tolower((int) (*str1)) - tolower((int) (*str2)));
}

/* ------------------------------------------------------------------------ *
 * ocisprintf
 * ------------------------------------------------------------------------ */

int ocisprintf(char *str, int size, const char *format, ...)
{
    va_list args;
    int n;

    va_start(args, format);

    n = vsnprintf(str, size, format, args);

    va_end(args);

    return n;
}

/* ------------------------------------------------------------------------ *
 *                         Numbers functions
 * ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ *
 * OCI_NumberGet
 * ------------------------------------------------------------------------ */

boolean OCI_NumberGet(OCI_Connection *con,  OCINumber *data,
                      void *value, int size, uword flag)
{
    OCI_CHECK_PTR(con,   FALSE);
    OCI_CHECK_PTR(value, FALSE);
    OCI_CHECK_PTR(data,  FALSE);

    OCI_CALL(con, OCINumberToInt(con->err, data, size, flag, value))
            
    OCI_CHECK_ERROR(con, NULL, FALSE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_NumberSet
 * ------------------------------------------------------------------------ */

boolean OCI_NumberSet(OCI_Connection *con,  OCINumber *data,
                      void *value, int size, uword flag)
{
    OCI_CHECK_PTR(con,   FALSE);
    OCI_CHECK_PTR(value, FALSE);
    OCI_CHECK_PTR(data,  FALSE);

    OCI_CALL(con, OCINumberFromInt(con->err, value, size, flag, data))
            
    OCI_CHECK_ERROR(con, NULL, FALSE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_IntegerGet
 * ------------------------------------------------------------------------ */

void * OCI_IntegerGet(OCI_Connection *con, 
                      void *src_ptr, ub1 src_type, int src_size,
                      void *dst_ptr, ub1 dst_type, int dst_size)
{
    /* no parameter checks until this function get reused in other contexts */

    if (src_type == dst_type)
    {
        /* non conversion */

        return src_ptr;
    }
    else if (src_type == OCI_NUM_NUMBER)
    {  
        uword flag = OCI_NUMBER_SIGNED;
        
        if (dst_type & OCI_NUM_UNSIGNED)
            flag = OCI_NUMBER_UNSIGNED;

       /* OCINumber to any integer types */

        OCI_NumberGet(con, src_ptr, dst_ptr, dst_size, flag);

        return dst_ptr;
    }
    else
    {
        /* integer type to another integer type through OCINumber */

        OCINumber num;

        uword src_flag = OCI_NUMBER_SIGNED;
        uword dst_flag = OCI_NUMBER_SIGNED;

        if (src_type & OCI_NUM_UNSIGNED)
            src_flag = OCI_NUMBER_UNSIGNED;
        
        if (dst_type & OCI_NUM_UNSIGNED)
            dst_flag = OCI_NUMBER_UNSIGNED;

        OCI_NumberSet(con, &num, src_ptr, src_size, src_flag);
        OCI_NumberGet(con, &num, dst_ptr, dst_size, dst_flag);

        return dst_ptr;
    }
}

/* ------------------------------------------------------------------------ *
 *                         Parsing functions
 * ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ *
 * OCI_ParseSqlFmt
 * ------------------------------------------------------------------------ */

int OCI_ParseSqlFmt(OCI_Statement *stmt, mtext *buf, const mtext *format,
                    va_list *pargs)
{
    int size        = 0;
    int len         = 0;
    boolean quote   = FALSE;
    mtext *pb       = buf;
    const mtext *pf = format;

    OCI_CHECK_PTR(format, 0);

    for (; *pf != 0; pf++)
    {
        if (*pf != '%')
        {
            if (buf != NULL) 
                *(pb++) = *pf;

            size++;
            continue;
        }
        else
        {
            quote = TRUE;
            len   = 0;

            if ( *(++pf) == '%')
            {
                if (buf != NULL)
                    *pb = *pf;

                quote = FALSE;
                len   = 1;
            }
        }

        switch (*pf)
        {
            case 's':
            case 'm':
            {
                const mtext *str = va_arg(*pargs, const mtext *);

                if (str != NULL && str[0] != 0)
                {
                    len = (int) ((str!= NULL) ? mtslen(str) : OCI_SIZE_NULL);

                    if ((quote == TRUE) && (*pf != 'm'))
                    {
                        if (buf != NULL)
                        {
                            *pb = 39;
                            mtscpy(pb+1, str);
                            *(pb+len+1) = 39;
                        }

                        len+=2;
                    }
                    else if (buf != NULL) 
                        mtscpy(pb, str);
                }
                else
                {
                    if (*pf != 'm')
                    {
                        len = OCI_SIZE_NULL;
                        
                        if (buf != NULL) 
                            mtscpy(pb, OCI_STRING_NULL);
                    }
                }
                break;
            }
            case 't':
            {
                OCI_Date *date = (OCI_Date *) va_arg(*pargs, OCI_Date *);

                if (buf != NULL)
                {
                    if (date != NULL)
                    {
                        mtsprintf(pb, OCI_SIZE_DATE,
                                  MT("to_date(\'%02i%02i%04i%02i%02i%02i\',")
                                  MT("\'DDMMYYYYHH24MISS\')"),
                                  date->value->OCIDateDD,
                                  date->value->OCIDateMM,
                                  date->value->OCIDateYYYY ,
                                  date->value->OCIDateTime.OCITimeHH,
                                  date->value->OCIDateTime.OCITimeMI,
                                  date->value->OCIDateTime.OCITimeSS);
                    }
                    else
                    {
                        mtscpy(pb, OCI_STRING_NULL);
                    }
                }

                len = ((date != NULL) ? OCI_SIZE_DATE : OCI_SIZE_NULL);

                break;
            }
            case 'p':
            {
                OCI_Timestamp *tmsp = (OCI_Timestamp *) va_arg(*pargs, OCI_Timestamp *);

                if (buf != NULL)
                {
                    if (tmsp != NULL)
                    {
                        mtext str_ff[12];
                        int yy, mm, dd, hh, mi, ss, ff;

                        yy = mm = dd = mi = hh = ss = ff = 0;

                        OCI_TimestampGetDate(tmsp, &yy, &mm, &dd);
                        OCI_TimestampGetTime(tmsp, &hh, &mi, &ss, &ff);

                        if (ff > 0)
                            mtsprintf(str_ff, msizeof(str_ff)-1, MT("%i"), ff);
                        else
                            mtscpy(str_ff, MT("00"));

                        str_ff[2] = 0;

                        len = mtsprintf(pb, OCI_SIZE_TIMESTAMP,
                                        MT("to_timestamp(\'%02i%02i%04i%02i%02i%02i%s\',")
                                        MT("\'DDMMYYYYHH24MISSFF\')"),
                                        dd, mm, yy, hh, mi, ss, str_ff);
                    }
                    else
                    {
                        mtscpy(pb, OCI_STRING_NULL);
                    }
                }
                else
                    len = ((tmsp != NULL) ? OCI_SIZE_TIMESTAMP : OCI_SIZE_NULL);

                break;
            }
            case 'v':
            {
                mtext temp[128];

                OCI_Interval *itv = (OCI_Interval *) va_arg(*pargs, OCI_Interval *);

                temp[0] = 0;

                if (itv != NULL)
                {
                    OCI_IntervalToText(itv, 3, 3, msizeof(temp)-1, temp);
                    
                    len = (int) mtslen(temp);

                    if ((buf != NULL) && (len > 0)) 
                        mtscpy(pb, temp);
                }
                else
                {
                    len = OCI_SIZE_NULL;
                    
                    if ((buf != NULL) && (len > 0)) 
                        mtscpy(pb, OCI_STRING_NULL);
                }
  
                break;
            }
            case 'i':
            {
                mtext temp[64];
       
                temp[0] = 0;

                len = mtsprintf(temp, msizeof(temp)-1, MT("%i"),
                                va_arg(*pargs, int));

                if ((buf != NULL) && (len > 0)) 
                    mtscpy(pb, temp);

                break;
            }
            case 'u':
            {
                mtext temp[64];
  
                temp[0] = 0;

                len = mtsprintf(temp, msizeof(temp)-1, MT("%u"),
                                va_arg(*pargs, unsigned int));

                if ((buf != NULL) && (len > 0)) 
                    mtscpy(pb, temp);

                break;
            }
            case 'l':
            {
                mtext temp[64];

                temp[0] = 0;

                pf++;

                if (*pf == 'i')
                {
                    len = mtsprintf(temp, msizeof(temp)-1, MT("%lld"),
                                    va_arg(*pargs, big_int));
                }
                else if (*pf == 'u')
                {
                    len = mtsprintf(temp, msizeof(temp)-1, MT("%llu"),
                                    va_arg(*pargs, big_uint));
                }
                else
                    len = 0;

                if ((buf != NULL) && (len > 0)) 
                    mtscpy(pb, temp);

                break;
            }
            case 'h':
            {
                mtext temp[64];
   
                temp[0] = 0;

                pf++;

                /* short ints pust be passed as int to va_args */

                if (*pf == 'i')
                {
                    len = mtsprintf(temp, msizeof(temp)-1, MT("%hd"),
                                    va_arg(*pargs, int));
                }
                else if (*pf == 'u')
                {
                    len = mtsprintf(temp, msizeof(temp)-1, MT("%hu"),
                                    va_arg(*pargs, unsigned int));
                }
                else
                    len = 0;

                if ((buf != NULL) && (len > 0)) 
                    mtscpy(pb, temp);

                break;
            }
            case 'g':
            {
                mtext temp[128];
      
                temp[0] = 0;

                len = mtsprintf(temp, msizeof(temp)-1, MT("%f"),
                                va_arg(*pargs, double));

                if ((buf != NULL) && (len > 0)) 
                    mtscpy(pb, temp);

                break;
            }
            default:
            {
                OCI_ErrorGeneric(stmt->con, stmt, OCI_ERR_SQL_PARSING, 0,
                                 MT("Parsing SQL : unknown identifier '%c'"), *pf);

                return 0;
            }

        }

        if (buf != NULL)
            pb += len;

        size += len;
    }

    if (buf != NULL) 
        *pb = 0;

    return size;
}

#ifdef OCI_INCLUDE_WCHAR

/* ------------------------------------------------------------------------ *
 * ociwcsdup
 * ------------------------------------------------------------------------ */

wchar_t * ociwcsdup(const wchar_t * src)
{
    wchar_t *dst;

    OCI_CHECK_PTR(src, NULL);

    dst = (wchar_t *) malloc((wcslen(src)+1) * sizeof(wchar_t));

    if (dst != NULL)
        wcscpy(dst, src);

    return dst;
}

/* ------------------------------------------------------------------------ *
 * ociwcscasecmp
 * ------------------------------------------------------------------------ */

int ociwcscasecmp(const wchar_t *str1, const wchar_t *str2)
{
     if (str1 == NULL && str2 == NULL)
        return 0;

    if (str1 == NULL)
        return 1;

    if (str2 == NULL)
        return -1;

    while ((*str1 != 0) && (*str2 != 0) &&
           (towlower((wint_t)*str1) == towlower((wint_t)*str2)))
    {
        str1++;
        str2++;
    }

    return (towlower((wint_t) *str1) - towlower((wint_t) *str2));
}

#endif

/* ------------------------------------------------------------------------ *
 *                    Widechar/Multibytes String functions
 * ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ *
 * OCI_StringCopy4to2bytes
 * ------------------------------------------------------------------------ */

long OCI_StringCopy4to2bytes(const unsigned long* src, long src_size, 
                             unsigned short* dst, long dst_size)
{
    long cp_size = 0;

    const unsigned long*  src_end = NULL;
    const unsigned short* dst_end = NULL;
    
    OCI_CHECK_PTR(src, 0);
    OCI_CHECK_PTR(dst, 0);

    src_end = src + src_size;
    dst_end = dst + dst_size;

    while (src < src_end)
    {
        unsigned long c;
    
        if (dst >= dst_end) return -1; 
    
        c = *src++;
        
        if (c <= UNI_MAX_BMP) 
        { 
            if (c >= UNI_SUR_HIGH_START && c < UNI_SUR_LOW_END)
                *dst++ = UNI_REPLACEMENT_CHAR;
            else 
                *dst++ = (unsigned short) c;

            cp_size++;
        }
        else if (c > UNI_MAX_LEGAL_UTF32)
        {
            *dst++ = UNI_REPLACEMENT_CHAR;
            
            cp_size++;
        } 
        else
        {
            if ((dst + 1) >= dst_end) return -2; 

            c -= UNI_BASE;
            
            if (dst)
            {
                *dst++ = (unsigned short)((c >> UNI_SHIFT) + UNI_SUR_HIGH_START);
                *dst++ = (unsigned short)((c &  UNI_MASK ) + UNI_SUR_LOW_START );
            }

            cp_size++;
            cp_size++;
        }
    }

    return cp_size;
}

/* ------------------------------------------------------------------------ *
 * OCI_StringCopy2to4bytes
 * ------------------------------------------------------------------------ */

long OCI_StringCopy2to4bytes(const unsigned short* src, long src_size, 
                             unsigned long* dst, long dst_size)
{
    long cp_size = 0;

    const unsigned short* src_end = NULL;
    const unsigned long*  dst_end = NULL;

    unsigned long c1, c2;

    OCI_CHECK_PTR(src, 0);
    OCI_CHECK_PTR(dst, 0);

    src_end = src + src_size;
    dst_end = dst + dst_size;

    while (src < src_end)
    {
        c1 = *src++;

        if (c1 >= UNI_SUR_HIGH_START && c1 <= UNI_SUR_HIGH_END)
        {
            if (src < src_end)
            {
                c2 = *src;

                if (c2 >= UNI_SUR_LOW_START && c2 <= UNI_SUR_LOW_END)
                {
                    c1 = ((c1 - UNI_SUR_HIGH_START) << UNI_SHIFT) + 
                          (c1 - UNI_SUR_LOW_START )  + UNI_BASE;
                    
                    ++src;
                }
            } 
            else 
                return -1;
        } 

        if (dst >= dst_end) return -2;

        *dst++ = c1;

        cp_size++;
    }

    return cp_size;
}

/* ------------------------------------------------------------------------ *
 * OCI_StringLength
 * ------------------------------------------------------------------------ */

int OCI_StringLength(void *ptr, int size_elem)
{
    char *s = (char *) ptr;

    OCI_CHECK_PTR(s, 0);

    while (*s != 0) 
        s += size_elem;

    return (((int) (s - ((char *) ptr))) / size_elem);    
}

/* ------------------------------------------------------------------------ *
 * OCI_GetInputString
 * ------------------------------------------------------------------------ */

void * OCI_GetInputString(void *src, int *size, int size_char_in,
                          int size_char_out)
{
    OCI_CHECK_PTR(src,  NULL);
    OCI_CHECK_PTR(size, NULL);

    if (size_char_in == size_char_out) 
    {
        /* in/out type sizes are equal, so no conversion ! */
       
        if (*size == -1)
            *size = OCI_StringLength(src, size_char_in) * size_char_in;

        return src;
    }
    else
    {
        /* in/out type size are not equal, so conversion needed ! */
     
        long char_count = 0;
        void *dest      = NULL;

        if (*size == -1)
            char_count = OCI_StringLength(src, size_char_in);
        else
            char_count = *size / size_char_in;

        *size = 0;

        dest = malloc((char_count+1)*sizeof(size_char_out));

        if (dest != NULL)
        {
            if ((*(char*) src) != 0)
            {
                if (size_char_in > size_char_out)
                {
                    char_count = OCI_StringCopy4to2bytes
                                 (
                                    (const unsigned long  *)  src, char_count,
                                    (      unsigned short *) dest, char_count
                                 );
                }
                else
                {                
                    char_count = OCI_StringCopy2to4bytes
                                 (
                                    (const unsigned short *) src,  char_count, 
                                    (      unsigned long  *) dest, char_count
                                 );
                }
           }
                
            *size = char_count * size_char_out;

            memset(((char*)dest) + *size, 0, sizeof(size_char_out));
        }

        return dest;
    }
}

/* ------------------------------------------------------------------------ *
 * OCI_GetOutputString
 * ------------------------------------------------------------------------ */

void OCI_GetOutputString(void *src, void *dest, int *size, int size_char_in,
                         int size_char_out)
{
    OCI_CHECK_PARAM(src);
    OCI_CHECK_PARAM(dest);
    OCI_CHECK_PARAM(size);

    /* do something only if in/out type sizes are not equal ! */

    if (size_char_in != size_char_out) 
    {        
        long char_count = 0;
     
        if (*size == -1)
            char_count = OCI_StringLength((void *) src, size_char_in);
        else
            char_count = *size / size_char_in;
           
        if (size_char_in > size_char_out)
        {
            char_count = OCI_StringCopy4to2bytes
                         (
                            (const unsigned long  *)  src, char_count,
                            (      unsigned short *) dest, char_count
                         );

        }
        else
        {
            char_count = OCI_StringCopy2to4bytes
                         (
                            (const unsigned short *)  src, char_count,
                            (      unsigned long  *) dest, char_count
                         );
        }

        *size = char_count * size_char_out;
    }
}

/* ------------------------------------------------------------------------ *
 * OCI_ConvertString
 * ------------------------------------------------------------------------ */

void OCI_ConvertString(void *str, int char_count, int size_char_in, 
                       int size_char_out)
{
    OCI_CHECK_PARAM(str);

    if (size_char_out > size_char_in)
    {
        /* expand string */
        
        unsigned long  *wcstr  = (unsigned long  *) str;
        unsigned short *ustr   = (unsigned short *) str;
 
        if (*ustr == 0)
            return;

        while (char_count--)
            wcstr[char_count] = (unsigned long) ustr[char_count];

    }
    else if (size_char_out < size_char_in)
    {
        /* pack string */
        
        unsigned long  *ustr  = (unsigned long  *) str;
        unsigned short *wcstr = (unsigned short *) str;
        int i = 0;

        if (*wcstr == 0)
            return;

        while (++i < char_count)
            ustr[char_count] = (unsigned long) wcstr[char_count];

    }
}

/* ------------------------------------------------------------------------ *
 * OCI_CopyString
 * ------------------------------------------------------------------------ */

void OCI_CopyString(void *src, void *dest, int *size, int size_char_in,
                    int size_char_out)
{
    OCI_CHECK_PARAM(src);
    OCI_CHECK_PARAM(dest);
    OCI_CHECK_PARAM(size);

    if (size_char_out == size_char_in)
        memcpy(dest, src, *size);
    else
        OCI_GetOutputString(src, dest, size, size_char_in, size_char_out);
}

/* ------------------------------------------------------------------------ *
 * OCI_ReleaseMetaString
 * ------------------------------------------------------------------------ */

void OCI_ReleaseMetaString(void *str)
{
    OCI_CHECK_PARAM(str);

#ifdef OCI_CHECK_METASTRINGS 

    free(str);

#endif
}

/* ------------------------------------------------------------------------ *
 * OCI_ReleaseDataString
 * ------------------------------------------------------------------------ */

void OCI_ReleaseDataString(void *str)
{
    OCI_CHECK_PARAM(str);

#ifdef OCI_CHECK_DATASTRINGS 

    free(str);

#endif
}

/* ------------------------------------------------------------------------ *
 *                    OCI_Item functions
 * ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ *
 * OCI_ListCreate
 * ------------------------------------------------------------------------ */

OCI_List * OCI_ListCreate(void)
{
    OCI_List *list = NULL;
    size_t msize   = 0;

    OCI_ALLOC(OCI_List *, list, msize, sizeof(OCI_List))

    /* create mutex on multithreaded environments */

    if (list != NULL && OCI_LIB_THREADED)
    {
        list->mutex = OCI_MutexCreate();

        if (list->mutex == NULL)
            OCI_FREE(list);
    }

    return list;
}

/* ------------------------------------------------------------------------ *
 * OCI_ListFree
 * ------------------------------------------------------------------------ */

boolean OCI_ListFree(OCI_List *list)
{
    OCI_CHECK_PTR(list, FALSE);

    OCI_ListClear(list);

    OCI_MutexFree(list->mutex);

    OCI_FREE(list);
    
    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_ListCreateItem
 * ------------------------------------------------------------------------ */

OCI_Item * OCI_ListCreateItem(int size)
{
    OCI_Item *item  = NULL;
    size_t    msize = 0;

    /* allocate list item entry */

    OCI_ALLOC(OCI_Item *, item, msize, sizeof(OCI_Item));

    if (item != NULL)
    {
        /* allocate item data buffer */

        OCI_ALLOC(void *, item->data, msize, size);

        if (item->data == NULL)
        {
            OCI_ErrorMemory(NULL, NULL, msize);
            OCI_FREE(item);
        }
    }
    else
        OCI_ErrorMemory(NULL, NULL, msize);

    return item;
}

/* ------------------------------------------------------------------------ *
 * OCI_ListAppend
 * ------------------------------------------------------------------------ */

OCI_Item * OCI_ListAppend(OCI_List *list, int size)
{
    OCI_Item *item = NULL;
    OCI_Item *temp = NULL;

    OCI_CHECK_PTR(list, FALSE);

    item = OCI_ListCreateItem(size);

    OCI_CHECK_PTR(item, FALSE);

    OCI_MutexAcquire(list->mutex);

    temp = list->head;

    while (temp != NULL && temp->next)
    {
        temp = temp->next;
    }

    if (temp != NULL)
        temp->next = item;
    else
        list->head = item;

    list->count++;

    OCI_MutexRelease(list->mutex);

    return item;
}

/* ------------------------------------------------------------------------ *
 * OCI_ListClear
 * ------------------------------------------------------------------------ */

boolean OCI_ListClear(OCI_List *list)
{
    OCI_Item *item = NULL;
    OCI_Item *temp = NULL;

    OCI_CHECK_PTR(list, FALSE);

    OCI_MutexAcquire(list->mutex);

    /* walk along the list to free item's buffer */

    item = list->head;

    while (item != NULL)
    {
        temp  = item;
        item  = item->next;

        /* free data */

        OCI_FREE(temp->data);
        OCI_FREE(temp);
    }

    list->head  = NULL;
    list->count = 0;

    OCI_MutexRelease(list->mutex);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_ListForEach
 * ------------------------------------------------------------------------ */

boolean OCI_ListForEach(OCI_List *list, boolean (*proc)(void *))
{
    OCI_Item *item = NULL;

    OCI_CHECK_PTR(list, FALSE);

    OCI_MutexAcquire(list->mutex);

    item = list->head;

    /* for each item in the list, execute the given callback */

    while (item != NULL)
    {
        proc(item->data);
        item = item->next;
    }

    OCI_MutexRelease(list->mutex);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_ListRemove
 * ------------------------------------------------------------------------ */

boolean OCI_ListRemove(OCI_List *list, void *data)
{
    OCI_Item *item = NULL;
    OCI_Item *temp = NULL;

    OCI_CHECK_PTR(list, FALSE);
    OCI_CHECK_PTR(data, FALSE);

    OCI_MutexAcquire(list->mutex);

    item = list->head;

    while (item != NULL)
    {
        if (item->data == data)
        {
            if (temp) temp->next = item->next;

            /* if item was the first entry, reajust the first list
               entry to next element */

            if (item == list->head) list->head = item->next;

            OCI_FREE(item);

            break;
        }

        temp = item;
        item = item->next;
    }

    list->count--;

    OCI_MutexRelease(list->mutex);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 *                    OCI_Bind functions
 * ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ *
 * OCI_BindFreeAll
 * ------------------------------------------------------------------------ */

boolean OCI_BindFreeAll(OCI_Statement *stmt)
{
    int i;
    OCI_Bind *bnd;

    OCI_CHECK_PTR(stmt, FALSE)
    OCI_CHECK_PTR(stmt->binds, FALSE);

    for(i = 0; i < stmt->nb_binds; i++)
    {
        bnd = stmt->binds[i];

        if (bnd->alloc == TRUE)
            OCI_FREE(bnd->buf.data);

        OCI_FREE(bnd->buf.inds);
        OCI_FREE(bnd->buf.lens);
        OCI_FREE(bnd->buf.temp);

        OCI_FREE(bnd->name);
        OCI_FREE(bnd);
    }

    OCI_FREE(stmt->binds);

    stmt->nb_binds    = 0;
    stmt->nb_outbinds = 0;

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_BindCheck
 * ------------------------------------------------------------------------ */

boolean OCI_BindCheck(OCI_Statement *stmt)
{
    int i, j;
    OCI_Bind *bnd;
    sb2 *ind;

    OCI_CHECK_PTR(stmt, FALSE)
    OCI_CHECK_PTR(stmt->binds, FALSE);

    for(i = 0; i < stmt->nb_binds; i++)
    {
        bnd = stmt->binds[i];
        ind = (sb2 *) bnd->buf.inds;

        /* no check for output binds */

        if (bnd->mode == OCI_BIND_OUTPUT)
            continue;

        if (bnd->alloc == TRUE)
        {
            if (bnd->buf.count == 1)
            {
                /* - For big integer (64 bits), we use an OCINumber.

                   - Oracle date/time type is the only non scalar type 
                     implemented by oracle through a public structure instead 
                     of using a handle. So we need to copy the value
                */

                if (bnd->type == OCI_CDT_INTEGER)
                {
                    OCI_CALL(stmt->con, 
                             OCINumberFromInt(stmt->con->err, 
                                              (CONST void *) bnd->input,
                                              sizeof(big_int), 
                                              bnd->subtype, 
                                              (OCINumber *) bnd->buf.data))

                    OCI_CHECK_ERROR(stmt->con, stmt, FALSE);
                }
                else if (bnd->type == OCI_CDT_DATETIME)
                {
                    memcpy(bnd->buf.data,
                           ((OCI_Date *) bnd->input)->value,
                           sizeof(OCIDate));
                }

#ifdef OCI_CHECK_DATASTRINGS

                else if (bnd->type == OCI_CDT_TEXT)
                {
                    /* need conversion if bind buffer was allocated */
 
                    int osize = -1;

                    OCI_GetOutputString(bnd->input, bnd->buf.data, &osize, 
                                        sizeof(dtext), sizeof(odtext));

                }

 #endif

                else
                {
                    bnd->buf.data[0] = ((OCI_Datatype *) bnd->input)->handle;

                    /* for handles, check anyway the value for null data */

                    if (ind != NULL && *ind != -1)
                        *ind = OCI_IND(bnd->buf.data);
                }
            }
            else
            {
                for (j = 0; j < bnd->buf.count; j++, ind++)
                {

                    /* - For big integer (64 bits), we use an OCINumber.

                       - Oracle date/time type is the only non scalar type 
                         implemented by oracle through a public structure instead 
                         of using a handle. So we need to copy the value
                    */

                    if (bnd->type == OCI_CDT_INTEGER)
                    {
                        OCINumber *num = (OCINumber *) ((ub1 *) bnd->buf.data + (j*bnd->size));

                        big_int *value = (big_int *) (((ub1 *) bnd->input) + (j*sizeof(big_int)));

                        OCI_CALL(stmt->con, 
                                 OCINumberFromInt(stmt->con->err, 
                                                  (CONST void *) value,
                                                  sizeof(big_int), 
                                                  bnd->subtype, 
                                                  num))

                        OCI_CHECK_ERROR(stmt->con, stmt, FALSE);
                    }
                    else  if (bnd->type == OCI_CDT_DATETIME)
                    {
                       memcpy(((ub1 *) bnd->buf.data) + (j*bnd->size),
                              ((OCI_Date *) bnd->input[j])->value,
                               sizeof(OCIDate));
                    }

#ifdef OCI_CHECK_DATASTRINGS

                    else if (bnd->type == OCI_CDT_TEXT)
                    {
                        /* need conversion if bind buffer was allocated */

                        int osize   = -1;
                        int offset1 = (bnd->size/sizeof(odtext))*sizeof(dtext);
                        int offset2 = bnd->size;

                        OCI_GetOutputString(((ub1 *) bnd->input)    + (j*offset1), 
                                            ((ub1 *) bnd->buf.data) + (j*offset2),
                                            &osize, sizeof(dtext), sizeof(odtext));

                    }

#endif

                    else
                    {
                        bnd->buf.data[j] = ((OCI_Datatype *) bnd->input[j])->handle;

                        /* for handles, check anyway the value for null data */

                        if (ind != NULL && *ind != -1)
                            *ind = OCI_IND(bnd->buf.data[j]);
                    }
                }
            }
        }
    }

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_BindReset
 * ------------------------------------------------------------------------ */

boolean OCI_BindReset(OCI_Statement *stmt)
{
    int i,j;

    OCI_CHECK_PTR(stmt, FALSE)
    OCI_CHECK_PTR(stmt->binds, FALSE);

    /* avoid unused param warning from compiler */
    
    i = j = 0;

    for(i = 0; i < stmt->nb_binds; i++)
    {
        OCI_Bind *bnd = stmt->binds[i];

        if (bnd->mode == OCI_BIND_OUTPUT)
            continue;

        memset(bnd->buf.inds, 0, bnd->buf.count * sizeof(sb2));

#ifdef OCI_CHECK_DATASTRINGS

        if (stmt->binds[i]->type == OCI_CDT_TEXT)
        {
            for (j = 0; j < bnd->buf.count; j++)
            {
                /* need conversion if bind buffer was allocated */

                int osize   = -1;
                int offset1 = (bnd->size/sizeof(odtext))*sizeof(dtext);
                int offset2 = bnd->size;

                OCI_GetOutputString(((ub1 *) bnd->buf.data) + (j*offset2), 
                                    ((ub1 *) bnd->input)    + (j*offset1),
                                    &osize, sizeof(odtext), sizeof(dtext));
            }
        }

#endif

    }

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_BindData
 * ------------------------------------------------------------------------ */

boolean OCI_BindData(OCI_Statement *stmt, void *data, ub4 size, 
                     const mtext *name, ub1 type, int code, int mode, 
                     int subtype, void *extra, unsigned int nbelem)
{
    OCI_Bind *bnd   = NULL;
    int exec_mode   = OCI_DEFAULT;
    size_t msize    = 0;
    ub4 *pnbelem    = NULL;
    ub4 i;

    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(name, FALSE);
    OCI_CHECK(stmt->nb_binds >= OCI_BIND_MAX, FALSE);
    OCI_CHECK(data == NULL && mode == OCI_BIND_INPUT, FALSE);

    if (nbelem == 0)
        nbelem = stmt->nb_iters;

    /* allocate bind array if necessary */

    if(stmt->binds == NULL)
    {
        OCI_ALLOC(OCI_Bind **, stmt->binds, msize,
                  OCI_BIND_MAX * sizeof(OCI_Bind *));
    }

    if(stmt->binds == NULL)
    {
        OCI_ErrorMemory(stmt->con, stmt, msize);
        return FALSE;
    }

    /* allocate bind object */
    
    OCI_ALLOC(OCI_Bind *, bnd, msize, sizeof(OCI_Bind));

    if (bnd == NULL)
    {
        OCI_ErrorMemory(stmt->con, stmt, msize);
        return FALSE;
    }

    /* allocate indicators array */
    
    OCI_ALLOC(void *, bnd->buf.inds, msize, nbelem * sizeof(sb2));

    if (bnd->buf.inds == NULL)
    {
        OCI_ErrorMemory(stmt->con, stmt, msize);
        OCI_FREE(bnd);
        return FALSE;
    }

    /* check for PL/SQL table */

    if (nbelem > 1 && (stmt->type == OCI_CST_BEGIN || 
                       stmt->type == OCI_CST_DECLARE))
    {
        /* allocate array of element sizes */
     
        OCI_ALLOC(ub2 *, bnd->plsizes, msize, nbelem * sizeof(ub2));
    
        if (bnd->plsizes == NULL)
        {
            OCI_ErrorMemory(stmt->con, stmt, msize);
            OCI_FREE(bnd->buf.inds);
            return FALSE;
        }

        for(i = 0; i < nbelem; i++)
            bnd->plsizes[i] = (ub2) size;
      
        /* allocate array of returned codes */
    
        OCI_ALLOC(ub2 *, bnd->plrcds, msize, nbelem * sizeof(ub2));
      
        if (bnd->plrcds == NULL)
        {
            OCI_ErrorMemory(stmt->con, stmt, msize);
            OCI_FREE(bnd->buf.inds);
            OCI_FREE(bnd->plsizes);
            return FALSE;
        }

        pnbelem = &bnd->plnbelem;
    }

    /* for handle based datatypes, we need to allocate an array of handles for
       bind calls because OCILIB uses external arrays of OCILIB Objects */

    if (mode == OCI_BIND_INPUT)
    {
        if (type != OCI_CDT_RAW      && 
            type != OCI_CDT_LONG     && 
            type != OCI_CDT_CURSOR   &&
            type != OCI_CDT_DOUBLE   && 

#ifndef OCI_CHECK_DATASTRINGS
            
            type != OCI_CDT_TEXT     && 

#endif
            (type != OCI_CDT_INTEGER || code == SQLT_VNU)
        )
        {
            bnd->alloc = TRUE;

            OCI_ALLOC(void **, bnd->buf.data, msize, nbelem * size);

            if (bnd->buf.data == NULL)
            {
                OCI_ErrorMemory(stmt->con, stmt, msize);
                OCI_FREE(stmt->binds);
                return FALSE;
            }
        }
        else
            bnd->buf.data = (void **) data;
    }

    /* initialize bind object */

    bnd->stmt      = stmt;
    bnd->input     = (void **) data;
    bnd->type      = type;
    bnd->size      = size;
    bnd->name      = mtsdup(name);
    bnd->code      = (ub2) code;
    bnd->mode      = (ub1) mode;
    bnd->subtype   = (ub1) subtype;

    /* initialize buffer */

    bnd->buf.count   = nbelem;
    bnd->buf.sizelen = sizeof(ub2);

    /* if we bind an OCILong or any outut bind, we need to change the execution
       mode to provide data at execute time */

    if (bnd->type == OCI_CDT_LONG)
    {
        stmt->long_size = size;
        exec_mode       = OCI_DATA_AT_EXEC;
    }
    else if (bnd->mode == OCI_BIND_OUTPUT)
    {
        exec_mode = OCI_DATA_AT_EXEC;
    }

   /* OCI binding */

    if(stmt->bind_mode == OCI_BIND_BY_POS)
    {
        int index = (int) mtstol(&bnd->name[1], NULL, 10);

        OCI_CALL(stmt->con, OCIBindByPos(stmt->stmt,
                                         (OCIBind **) &bnd->buf.handle,
                                         stmt->con->err, index,
                                         bnd->buf.data, bnd->size,
                                         bnd->code, bnd->buf.inds,
                                         bnd->plsizes, bnd->plrcds, 
                                         nbelem == 1 ? 0 : nbelem, 
                                         pnbelem, exec_mode))
    }
    else
    {
        void * ostr = NULL;
        int osize   = -1;

        ostr = OCI_GetInputMetaString(bnd->name, &osize);

        OCI_CALL(stmt->con, OCIBindByName(stmt->stmt,
                                          (OCIBind **) &bnd->buf.handle,
                                          stmt->con->err,
                                          (CONST OraText *) ostr, (sb4) osize,
                                          bnd->buf.data, bnd->size,
                                          bnd->code, bnd->buf.inds,
                                          bnd->plsizes, bnd->plrcds, 
                                          nbelem == 1 ? 0 : nbelem, 
                                          pnbelem, exec_mode))
        
        OCI_ReleaseMetaString(ostr);
    }

    if (code == SQLT_NTY)
    {                                          
        OCI_CALL(stmt->con, OCIBindObject((OCIBind *) bnd->buf.handle, 
                                          stmt->con->err, 
                                          (CONST OCIType *) extra,
                                          (void **) bnd->buf.data,
                                          NULL, NULL,
                                          (ub4 *) bnd->buf.inds))
    }

    if (bnd->mode == OCI_BIND_OUTPUT)
    {
        /* register output placeholder */

        OCI_CALL(stmt->con, OCIBindDynamic((OCIBind *) bnd->buf.handle,
                                           stmt->con->err,
                                           (dvoid *) bnd,
                                           OCI_ProcInBind,
                                           (dvoid *) bnd,
                                           OCI_ProcOutBind))
    }

    if ((bnd->type == OCI_CDT_LOB && bnd->subtype == OCI_NCLOB)
#ifdef OCI_USERDATA_UNICODE        
        || (bnd->type == OCI_CDT_TEXT && OCILib.ver_runtime >= OCI_9 && bnd->plsizes == NULL)
#endif
        )
    {
        ub1 csfrm = SQLCS_NCHAR;

        OCI_CALL(bnd->stmt->con, OCIAttrSet((dvoid *) bnd->buf.handle,
                                            (ub4) OCI_HTYPE_BIND,
                                            (dvoid *) &csfrm, 
                                            sizeof(csfrm),
                                            (ub4) OCI_ATTR_CHARSET_FORM,
                                            bnd->stmt->con->err));
    }

#ifdef OCI_CHARSET_MIXED

    /* setup Unicode mode for user data in mixed builds */
    {

        ub2 csid = OCI_UTF16ID;

        OCI_CALL(stmt->con, OCIAttrSet((dvoid *) bnd->buf.handle,
                                       (ub4) OCI_HTYPE_BIND,
                                       (dvoid *) &csid,
                                       (ub4) 0,
                                       (ub4) OCI_ATTR_CHARSET_ID,
                                       stmt->con->err))
    }

#endif

    OCI_CHECK_ERROR(stmt->con, stmt, FALSE);

    /* ok - binding done */

    stmt->binds[stmt->nb_binds++] = bnd;

    /* this will allow resultset creation for statement with returning clause */
    
    if (bnd->mode == OCI_BIND_OUTPUT)
        stmt->nb_outbinds++;

    return TRUE;
}

/* ------------------------------------------------------------------------ *
                    OCI_Error functions
 * ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ *
 * OCI_ErrorGetString
 * ------------------------------------------------------------------------ */

const mtext * OCI_API OCI_ErrorGetString(OCI_Error *err)
{
    OCI_CHECK_PTR(err, NULL);

    return err->str;
}

/* ------------------------------------------------------------------------ *
 * OCI_ErrorGetType
 * ------------------------------------------------------------------------ */

int OCI_API OCI_ErrorGetType(OCI_Error *err)
{
    OCI_CHECK_PTR(err, OCI_UNKNOWN);

    return err->type;
}

/* ------------------------------------------------------------------------ *
 * OCI_ErrorGetOCICode
 * ------------------------------------------------------------------------ */

int OCI_API OCI_ErrorGetOCICode(OCI_Error *err)
{
    OCI_CHECK_PTR(err, 0);

    return err->oci_code;
}

/* ------------------------------------------------------------------------ *
 * OCI_ErrorGetInternalCode
 * ------------------------------------------------------------------------ */

int OCI_API OCI_ErrorGetInternalCode(OCI_Error *err)
{
    OCI_CHECK_PTR(err, 0);

    return err->int_code;
}

/* ------------------------------------------------------------------------ *
 * OCI_ErrorGetConnection
 * ------------------------------------------------------------------------ */

OCI_Connection * OCI_API OCI_ErrorGetConnection(OCI_Error *err)
{
    OCI_CHECK_PTR(err, NULL);

    return err->con;
}

/* ------------------------------------------------------------------------ *
 * OCI_ErrorGetStatement
 * ------------------------------------------------------------------------ */

OCI_Statement * OCI_API OCI_ErrorGetStatement(OCI_Error *err)
{
    OCI_CHECK_PTR(err, NULL);

    return err->stmt;
}

/* ------------------------------------------------------------------------ *
                    OCI_Library functions
 * ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ *
 * OCI_Initialize
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_Initialize(POCI_ERROR err_handler, const mtext *home,
                               int mode)
{
    boolean res = TRUE;

 #if defined(OCI_IMPORT_RUNTIME) && defined(OCI_CHARSET_UNICODE) && !defined(_WINDOWS)

    char temp[OCI_SIZE_BUFFER];

#endif

    memset(&OCILib, 0, sizeof(OCI_Library));

    OCILib.error_handler  = err_handler;
    OCILib.ver_compile    = OCI_VERSION_COMPILE;
    OCILib.ver_runtime    = OCI_VERSION_RUNTIME;
    OCILib.env_mode       = OCI_ENV_MODE | OCI_OBJECT | mode;

#ifdef OCI_IMPORT_LINKAGE

    OCI_NOT_USED(home);

#if defined(OCI_BIG_UINT_ENABLED)

    OCILib.use_lob_ub8 = TRUE;

#endif

#else

    if (home != NULL && home[0] != 0)
    {
        mtsprintf(OCILib.path, msizeof(OCILib.path)-1, MT("%s//%s"), home, OCI_DL_NAME);
    }
    else
        mtscpy( OCILib.path, OCI_DL_NAME);

#if defined(OCI_CHARSET_UNICODE) && !defined(_WINDOWS)

   temp[0] = 0;

   wcstombs(temp, OCILib.path, sizeof(temp)-1);

   OCILib.lib_handle = LIB_OPEN(temp);

#else

   OCILib.lib_handle = LIB_OPEN(OCILib.path);

#endif

    if (OCILib.lib_handle == NULL)
    {
        /* OCI shared library not found ....*/

        OCI_ErrorGeneric(NULL, NULL, OCI_ERR_API, errno, 
                         MT("Cannot load OCI shared library"));

        return FALSE;
    }

    /* Now loading all symbols - no check is performed on each function,
       Basic checks will be done to ensure we're loading an
       Oracle and compatible library ...
    */

    LIB_SYMBOL(OCILib.lib_handle, "OCIEnvCreate", OCIEnvCreate,
               OCIENVCREATE);

    LIB_SYMBOL(OCILib.lib_handle, "OCIServerAttach", OCIServerAttach,
               OCISERVERATTACH);
    LIB_SYMBOL(OCILib.lib_handle, "OCIServerDetach", OCIServerDetach,
               OCISERVERDETACH);

    LIB_SYMBOL(OCILib.lib_handle, "OCIHandleAlloc", OCIHandleAlloc,
               OCIHANDLEALLOC);
    LIB_SYMBOL(OCILib.lib_handle, "OCIHandleFree",  OCIHandleFree,
               OCIHANDLEFREE);

    LIB_SYMBOL(OCILib.lib_handle, "OCIDescriptorAlloc", OCIDescriptorAlloc,
               OCIDESCRIPTORALLOC);
    LIB_SYMBOL(OCILib.lib_handle, "OCIDescriptorFree", OCIDescriptorFree,
               OCIDESCRIPTORFREE);

    LIB_SYMBOL(OCILib.lib_handle, "OCIAttrSet", OCIAttrSet,
               OCIATTRSET);
    LIB_SYMBOL(OCILib.lib_handle, "OCIAttrGet", OCIAttrGet,
               OCIATTRGET);

    LIB_SYMBOL(OCILib.lib_handle, "OCIParamSet", OCIParamSet,
               OCIPARAMSET);
    LIB_SYMBOL(OCILib.lib_handle, "OCIParamGet", OCIParamGet,
               OCIPARAMGET);

    LIB_SYMBOL(OCILib.lib_handle, "OCISessionBegin", OCISessionBegin,
               OCISESSIONBEGIN);
    LIB_SYMBOL(OCILib.lib_handle, "OCISessionEnd", OCISessionEnd,
               OCISESSIONEND);

    LIB_SYMBOL(OCILib.lib_handle, "OCIPasswordChange", OCIPasswordChange,
               OCIPASSWORDCHANGE);

    LIB_SYMBOL(OCILib.lib_handle, "OCITransStart", OCITransStart,
               OCITRANSSTART);
    LIB_SYMBOL(OCILib.lib_handle, "OCITransDetach", OCITransDetach,
               OCITRANSDETACH);
    LIB_SYMBOL(OCILib.lib_handle, "OCITransPrepare", OCITransPrepare,
               OCITRANSPREPARE);
    LIB_SYMBOL(OCILib.lib_handle, "OCITransForget", OCITransForget,
               OCITRANSFORGET);
    LIB_SYMBOL(OCILib.lib_handle, "OCITransCommit", OCITransCommit,
               OCITRANSCOMMIT);
    LIB_SYMBOL(OCILib.lib_handle, "OCITransRollback", OCITransRollback,
               OCITRANSROLLBACK);

    LIB_SYMBOL(OCILib.lib_handle, "OCIErrorGet",  OCIErrorGet,
               OCIERRORGET);
    LIB_SYMBOL(OCILib.lib_handle, "OCIServerVersion", OCIServerVersion,
               OCISERVERVERSION);
    LIB_SYMBOL(OCILib.lib_handle, "OCIBreak", OCIBreak,
               OCIBREAK);

    LIB_SYMBOL(OCILib.lib_handle, "OCIBindByPos", OCIBindByPos,
               OCIBINDBYPOS);
    LIB_SYMBOL(OCILib.lib_handle, "OCIBindByName", OCIBindByName,
               OCIBINDBYNAME);
    LIB_SYMBOL(OCILib.lib_handle, "OCIBindDynamic", OCIBindDynamic,
               OCIBINDDYNAMIC);
    LIB_SYMBOL(OCILib.lib_handle, "OCIBindObject", OCIBindObject,
               OCIBINDOBJECT);

    LIB_SYMBOL(OCILib.lib_handle, "OCIDefineByPos", OCIDefineByPos,
               OCIDEFINEBYPOS);
    LIB_SYMBOL(OCILib.lib_handle, "OCIDefineObject", OCIDefineObject,
               OCIDEFINEOBJECT);

    LIB_SYMBOL(OCILib.lib_handle, "OCIStmtPrepare", OCIStmtPrepare,
               OCISTMTPREPARE);
    LIB_SYMBOL(OCILib.lib_handle, "OCIStmtExecute", OCIStmtExecute,
               OCISTMTEXECUTE);
    LIB_SYMBOL(OCILib.lib_handle, "OCIStmtFetch", OCIStmtFetch,
               OCISTMTFETCH);

    LIB_SYMBOL(OCILib.lib_handle, "OCIStmtGetPieceInfo", OCIStmtGetPieceInfo,
               OCISTMTGETPIECEINFO);
    LIB_SYMBOL(OCILib.lib_handle, "OCIStmtSetPieceInfo", OCIStmtSetPieceInfo,
               OCISTMTSETPIECEINFO);

    LIB_SYMBOL(OCILib.lib_handle, "OCILobCreateTemporary", OCILobCreateTemporary,
               OCILOBCREATETEMPORARY);
    LIB_SYMBOL(OCILib.lib_handle, "OCILobFreeTemporary", OCILobFreeTemporary,
               OCILOBFREETEMPORARY);
    LIB_SYMBOL(OCILib.lib_handle, "OCILobIsTemporary", OCILobIsTemporary,
               OCILOBISTEMPORARY);
    LIB_SYMBOL(OCILib.lib_handle, "OCILobRead", OCILobRead,
               OCILOBREAD);
    LIB_SYMBOL(OCILib.lib_handle, "OCILobWrite", OCILobWrite,
               OCILOBWRITE);
    LIB_SYMBOL(OCILib.lib_handle, "OCILobCopy", OCILobCopy,
               OCILOBCOPY);
    LIB_SYMBOL(OCILib.lib_handle, "OCILobTrim", OCILobTrim,
               OCILOBTRIM);
    LIB_SYMBOL(OCILib.lib_handle, "OCILobErase", OCILobErase,
               OCILOBERASE);
    LIB_SYMBOL(OCILib.lib_handle, "OCILobAppend", OCILobAppend,
               OCILOBAPPEND);
    LIB_SYMBOL(OCILib.lib_handle, "OCILobGetLength", OCILobGetLength,
               OCILOBGETLENGTH);
    LIB_SYMBOL(OCILib.lib_handle, "OCILobOpen", OCILobOpen,
               OCILOBOPEN);
    LIB_SYMBOL(OCILib.lib_handle, "OCILobClose", OCILobClose,
               OCILOBCLOSE);

    LIB_SYMBOL(OCILib.lib_handle, "OCILobCopy2", OCILobCopy2,
               OCILOBCOPY2);
    LIB_SYMBOL(OCILib.lib_handle, "OCILobErase2", OCILobErase2,
               OCILOBERASE2);
    LIB_SYMBOL(OCILib.lib_handle, "OCILobGetLength2", OCILobGetLength2,
               OCILOBGETLENGTH2);
    LIB_SYMBOL(OCILib.lib_handle, "OCILobLoadFromFile2", OCILobLoadFromFile2,
               OCILOBLOADFROMFILE2);
    LIB_SYMBOL(OCILib.lib_handle, "OCILobRead2", OCILobRead2,
               OCILOBREAD2);
    LIB_SYMBOL(OCILib.lib_handle, "OCILobTrim2", OCILobTrim2,
               OCILOBTRIM2);
    LIB_SYMBOL(OCILib.lib_handle, "OCILobWrite2", OCILobWrite2,
               OCILOBWRITE2);
    LIB_SYMBOL(OCILib.lib_handle, "OCILobWriteAppend2", OCILobWriteAppend2,
               OCILOBWRITEAPPEND2);
 
    LIB_SYMBOL(OCILib.lib_handle, "OCILobFileOpen", OCILobFileOpen,
               OCILOBFILEOPEN);
    LIB_SYMBOL(OCILib.lib_handle, "OCILobFileClose", OCILobFileClose,
               OCILOBFILECLOSE);
    LIB_SYMBOL(OCILib.lib_handle, "OCILobFileCloseAll", OCILobFileCloseAll,
               OCILOBFILECLOSEALL);
    LIB_SYMBOL(OCILib.lib_handle, "OCILobFileIsOpen", OCILobFileIsOpen,
               OCILOBFILEISOPEN);
    LIB_SYMBOL(OCILib.lib_handle, "OCILobFileExists", OCILobFileExists,
               OCILOBFILEEXISTS);
    LIB_SYMBOL(OCILib.lib_handle, "OCILobFileGetName", OCILobFileGetName,
               OCILOBFIELGETNAME);
    LIB_SYMBOL(OCILib.lib_handle, "OCILobFileSetName", OCILobFileSetName,
               OCILOBFILESETNAME);
    LIB_SYMBOL(OCILib.lib_handle, "OCILobLoadFromFile", OCILobLoadFromFile,
               OCILOBLOADFROMFILE);
    LIB_SYMBOL(OCILib.lib_handle, "OCILobWriteAppend", OCILobWriteAppend,
               OCILOBWRITEAPPEND);
    LIB_SYMBOL(OCILib.lib_handle, "OCILobIsEqual", OCILobIsEqual,
               OCILOBISEQUAL);
    LIB_SYMBOL(OCILib.lib_handle, "OCILobAssign", OCILobAssign,
               OCILOBASSIGN);
    LIB_SYMBOL(OCILib.lib_handle, "OCILobLocatorAssign", OCILobLocatorAssign,
               OCILOBLOCATORASSIGN);

    LIB_SYMBOL(OCILib.lib_handle, "OCIDateAssign", OCIDateAssign,
               OCIDATEASSIGN);
    LIB_SYMBOL(OCILib.lib_handle, "OCIDateToText", OCIDateToText,
               OCIDATETOTEXT);
    LIB_SYMBOL(OCILib.lib_handle, "OCIDateFromText", OCIDateFromText,
               OCIDATEFROMTEXT);
    LIB_SYMBOL(OCILib.lib_handle, "OCIDateCompare", OCIDateCompare,
               OCIDATECOMPARE);
    LIB_SYMBOL(OCILib.lib_handle, "OCIDateAddMonths", OCIDateAddMonths,
               OCIDATEADDMONTHS);
    LIB_SYMBOL(OCILib.lib_handle, "OCIDateAddDays", OCIDateAddDays,
               OCIDATEADDDAYS);
    LIB_SYMBOL(OCILib.lib_handle, "OCIDateLastDay", OCIDateLastDay,
               OCIDATELASTDAY);
    LIB_SYMBOL(OCILib.lib_handle, "OCIDateDaysBetween", OCIDateDaysBetween,
               OCIDATEDAYSBETWEEN);
    LIB_SYMBOL(OCILib.lib_handle, "OCIDateZoneToZone", OCIDateZoneToZone,
               OCIDATEZONETOZONE);
    LIB_SYMBOL(OCILib.lib_handle, "OCIDateNextDay", OCIDateNextDay,
               OCIDATENEXTDAY);
    LIB_SYMBOL(OCILib.lib_handle, "OCIDateCheck", OCIDateCheck,
               OCIDATECHECK);
    LIB_SYMBOL(OCILib.lib_handle, "OCIDateSysDate", OCIDateSysDate,
               OCIDATESYSDATE);
    LIB_SYMBOL(OCILib.lib_handle, "OCIDescribeAny", OCIDescribeAny,
               OCIDESCRIBEANY);

    LIB_SYMBOL(OCILib.lib_handle, "OCIIntervalAssign", OCIIntervalAssign,
               OCIINTERVALASSIGN);
    LIB_SYMBOL(OCILib.lib_handle, "OCIIntervalCheck", OCIIntervalCheck,
               OCIINTERVALCHECK);
    LIB_SYMBOL(OCILib.lib_handle, "OCIIntervalCompare", OCIIntervalCompare,
               OCIINTERVALCOMPARE);
    LIB_SYMBOL(OCILib.lib_handle, "OCIIntervalFromText", OCIIntervalFromText,
               OCIINTERVALFROMTEXT);
    LIB_SYMBOL(OCILib.lib_handle, "OCIIntervalToText", OCIIntervalToText,
               OCIINTERVALTOTEXT);
    LIB_SYMBOL(OCILib.lib_handle, "OCIIntervalFromTZ", OCIIntervalFromTZ,
               OCIINTERVALFROMTZ);
    LIB_SYMBOL(OCILib.lib_handle, "OCIIntervalGetDaySecond", OCIIntervalGetDaySecond,
               OCIINTERVALGETDAYSECOND);
    LIB_SYMBOL(OCILib.lib_handle, "OCIIntervalGetYearMonth", OCIIntervalGetYearMonth,
               OCIINTERVALGETYEARMONTH);
    LIB_SYMBOL(OCILib.lib_handle, "OCIIntervalSetDaySecond", OCIIntervalSetDaySecond,
               OCIINTERVALSETDAYSECOND);
    LIB_SYMBOL(OCILib.lib_handle, "OCIIntervalSetYearMonth", OCIIntervalSetYearMonth,
               OCIINTERVALSETYEARMONTH);
    LIB_SYMBOL(OCILib.lib_handle, "OCIIntervalSubtract", OCIIntervalSubtract,
               OCIINTERVALSUBTRACT);
    LIB_SYMBOL(OCILib.lib_handle, "OCIIntervalAdd", OCIIntervalAdd,
               OCIINTERVALADD);

    LIB_SYMBOL(OCILib.lib_handle, "OCIDateTimeAssign", OCIDateTimeAssign,
               OCIDATETIMEASSIGN);
    LIB_SYMBOL(OCILib.lib_handle, "OCIDateTimeCheck", OCIDateTimeCheck,
               OCIDATETIMECHECK);
    LIB_SYMBOL(OCILib.lib_handle, "OCIDateTimeCompare", OCIDateTimeCompare,
               OCIDATETIMECOMPARE);
    LIB_SYMBOL(OCILib.lib_handle, "OCIDateTimeConstruct", OCIDateTimeConstruct,
               OCIDATETIMECONSTRUCT);
    LIB_SYMBOL(OCILib.lib_handle, "OCIDateTimeConvert", OCIDateTimeConvert,
               OCIDATETIMECONVERT);
    LIB_SYMBOL(OCILib.lib_handle, "OCIDateTimeFromArray", OCIDateTimeFromArray,
               OCIDATETIMEFROMARRAY);
    LIB_SYMBOL(OCILib.lib_handle, "OCIDateTimeToArray", OCIDateTimeToArray,
               OCIDATETIMETOARRAY);
    LIB_SYMBOL(OCILib.lib_handle, "OCIDateTimeFromText", OCIDateTimeFromText,
               OCIDATETIMEFROMTEXT);
    LIB_SYMBOL(OCILib.lib_handle, "OCIDateTimeToText", OCIDateTimeToText,
               OCIDATETIMETOTEXT);
    LIB_SYMBOL(OCILib.lib_handle, "OCIDateTimeGetDate", OCIDateTimeGetDate,
               OCIDATETIMEGETDATE);
    LIB_SYMBOL(OCILib.lib_handle, "OCIDateTimeGetTime", OCIDateTimeGetTime,
               OCIDATETIMEGETTIME);
    LIB_SYMBOL(OCILib.lib_handle, "OCIDateTimeGetTimeZoneName", OCIDateTimeGetTimeZoneName,
               OCIDATETIMEGETTIMEZONENAME);
    LIB_SYMBOL(OCILib.lib_handle, "OCIDateTimeGetTimeZoneOffset", OCIDateTimeGetTimeZoneOffset,
               OCIDATETIMEGETTIMEZONEOFFSET);
    LIB_SYMBOL(OCILib.lib_handle, "OCIDateTimeIntervalAdd", OCIDateTimeIntervalAdd,
               OCIDATETIMEINTERVALADD);
    LIB_SYMBOL(OCILib.lib_handle, "OCIDateTimeIntervalSub", OCIDateTimeIntervalSub,
               OCIDATETIMEINTERVALSUB);
    LIB_SYMBOL(OCILib.lib_handle, "OCIDateTimeSubtract", OCIDateTimeSubtract,
               OCIDATETIMESUBTRACT);
    LIB_SYMBOL(OCILib.lib_handle, "OCIDateTimeSysTimeStamp", OCIDateTimeSysTimeStamp,
               OCIDATETIMESYSTIMESTAMP);

    LIB_SYMBOL(OCILib.lib_handle, "OCITypeByName", OCITypeByName,
               OCITYPEBYNAME);

    LIB_SYMBOL(OCILib.lib_handle, "OCINumberToInt", OCINumberToInt,
               OCINUMBERTOINT);
    LIB_SYMBOL(OCILib.lib_handle, "OCINumberFromInt", OCINumberFromInt,
               OCINUMBERFROMINT);
 
    LIB_SYMBOL(OCILib.lib_handle, "OCINumberToReal", OCINumberToReal,
               OCINUMBERTOREAL);
    LIB_SYMBOL(OCILib.lib_handle, "OCINumberFromReal", OCINumberFromReal,
               OCINUMBERFROMREAL);

    LIB_SYMBOL(OCILib.lib_handle, "OCINumberToText", OCINumberToText,
               OCINUMBERTOTEXT);

    LIB_SYMBOL(OCILib.lib_handle, "OCIStringPtr", OCIStringPtr,
               OCISTRINGPTR);
    LIB_SYMBOL(OCILib.lib_handle, "OCIStringAssignText", OCIStringAssignText,
               OCISTRINGASSIGNTEXT);
    
    LIB_SYMBOL(OCILib.lib_handle, "OCIRawPtr", OCIRawPtr,
               OCIRAWPTR);
    LIB_SYMBOL(OCILib.lib_handle, "OCIRawAssignBytes", OCIRawAssignBytes,
               OCIRAWASSIGNBYTES);
    LIB_SYMBOL(OCILib.lib_handle, "OCIRawAllocSize", OCIRawAllocSize,
               OCIRAWALLOCSIZE);
    
    LIB_SYMBOL(OCILib.lib_handle, "OCIObjectNew", OCIObjectNew,
               OCIOBJECTNEW);
    LIB_SYMBOL(OCILib.lib_handle, "OCIObjectFree", OCIObjectFree,
               OCIOBJECTFREE);
    LIB_SYMBOL(OCILib.lib_handle, "OCIObjectSetAttr", OCIObjectSetAttr,
               OCIOBJECTSETATTR);
    LIB_SYMBOL(OCILib.lib_handle, "OCIObjectGetAttr", OCIObjectGetAttr,
               OCIOBJECTGETATTR);

    LIB_SYMBOL(OCILib.lib_handle, "OCIArrayDescriptorFree", OCIArrayDescriptorFree,
               OCIARRAYDESCRIPTORFREE);

    LIB_SYMBOL(OCILib.lib_handle, "OCIClientVersion", OCIClientVersion,
               OCICLIENTVERSION);

    LIB_SYMBOL(OCILib.lib_handle, "OCIThreadProcessInit", OCIThreadProcessInit,
               OCITHREADPROCESSINIT);
    LIB_SYMBOL(OCILib.lib_handle, "OCIThreadInit", OCIThreadInit,
               OCITHREADINIT);
    LIB_SYMBOL(OCILib.lib_handle, "OCIThreadTerm", OCIThreadTerm,
               OCITHREADTERM);

    LIB_SYMBOL(OCILib.lib_handle, "OCIThreadIdInit", OCIThreadIdInit,
               OCITHREADIDINIT);
    LIB_SYMBOL(OCILib.lib_handle, "OCIThreadIdDestroy", OCIThreadIdDestroy,
               OCITHREADIDDESTROY);
    LIB_SYMBOL(OCILib.lib_handle, "OCIThreadHndInit", OCIThreadHndInit,
               OCITHREADHNDINIT);
    LIB_SYMBOL(OCILib.lib_handle, "OCIThreadHndDestroy", OCIThreadHndDestroy,
               OCITHREADHNDDESTROY);
    LIB_SYMBOL(OCILib.lib_handle, "OCIThreadCreate", OCIThreadCreate,
               OCITHREADCREATE);
    LIB_SYMBOL(OCILib.lib_handle, "OCIThreadJoin", OCIThreadJoin,
               OCITHREADJOIN);
    LIB_SYMBOL(OCILib.lib_handle, "OCIThreadClose", OCIThreadClose,
               OCITHREADCLOSE);

    LIB_SYMBOL(OCILib.lib_handle, "OCIThreadMutexInit", OCIThreadMutexInit,
               OCITHREADMUTEXINIT);
    LIB_SYMBOL(OCILib.lib_handle, "OCIThreadMutexDestroy", OCIThreadMutexDestroy,
               OCITHREADMUTEXDESTROY);
    LIB_SYMBOL(OCILib.lib_handle, "OCIThreadMutexAcquire", OCIThreadMutexAcquire,
               OCITHREADMUTEXACQUIRE);
    LIB_SYMBOL(OCILib.lib_handle, "OCIThreadMutexRelease", OCIThreadMutexRelease,
               OCITHREADMUTEXRELEASE);

    LIB_SYMBOL(OCILib.lib_handle, "OCIThreadKeyInit", OCIThreadKeyInit,
               OCITHREADKEYINIT);
    LIB_SYMBOL(OCILib.lib_handle, "OCIThreadKeyDestroy", OCIThreadKeyDestroy,
               OCITHREADKEYDESTROY);
    LIB_SYMBOL(OCILib.lib_handle, "OCIThreadKeySet", OCIThreadKeySet,
               OCITHREADKEYSET);
    LIB_SYMBOL(OCILib.lib_handle, "OCIThreadKeyGet", OCIThreadKeyGet,
               OCITHREADKEYGET);


    LIB_SYMBOL(OCILib.lib_handle, "OCIConnectionPoolCreate", OCIConnectionPoolCreate,
               OCICONNECTIONPOOLCREATE);
    LIB_SYMBOL(OCILib.lib_handle, "OCIConnectionPoolDestroy", OCIConnectionPoolDestroy,
               OCICONNECTIONPOOLDESTROY);

    /* API Version checking */

    if (OCIArrayDescriptorFree != NULL)
    {
        OCILib.ver_runtime = OCI_11;
    }
    else if (OCIClientVersion != NULL)
    {
        OCILib.ver_runtime = OCI_10;
    }
    else if (OCIDateTimeGetTimeZoneName != NULL)
    {
        OCILib.ver_runtime = OCI_9;
    }
    else if (OCIEnvCreate != NULL)
    {
        OCILib.ver_runtime = OCI_8;
    }
    else
    {
        LIB_CLOSE(OCILib.lib_handle);

        OCI_ErrorGeneric(NULL, NULL, OCI_ERR_API, 0, 
                         MT("Cannot load OCI symbols from shared library"));

        res = FALSE;
    }

#if defined(OCI_BIG_UINT_ENABLED)

    if ((OCILib.ver_runtime >= 10) && (OCILobCopy2 != NULL))
    {
        OCILib.use_lob_ub8 = TRUE;
    }

#endif

#endif

    /* Oracle 8i does not support full unicode mode */

#if defined(OCI_CHARSET_UNICODE)

    if (OCILib.ver_runtime == OCI_8)
    {
        OCI_ErrorGeneric(NULL, NULL, OCI_ERR_API, 0, 
                         MT("Oracle 8i does not support full Unicode mode"));

        res = FALSE;
    }

#endif

    /* create environment on success */

    res = res && (OCI_SUCCESS == OCIEnvCreate(&OCILib.env, OCILib.env_mode, 
                                              NULL, NULL, NULL, NULL, 0, NULL));

    /*  allocate error handle */
    
    res = res && (OCI_SUCCESS == OCIHandleAlloc(OCILib.env, 
                                                (dvoid **) (void *) &OCILib.err,
                                                OCI_HTYPE_ERROR, 0, 
                                                (dvoid **) NULL));

    if (res == TRUE)
    {
        if (OCI_LIB_THREADED)
        {
            OCIThreadProcessInit();

            res = (OCI_SUCCESS == OCIThreadInit(OCILib.env, OCILib.err));
        }

        OCILib.cons  = OCI_ListCreate();    
        OCILib.pools = OCI_ListCreate();    
    }

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_Cleanup
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_Cleanup(void)
{
    boolean res = TRUE;

    /* free all connections */

    OCI_ListForEach(OCILib.cons, (boolean (*)(void *)) OCI_ConnectionClose);
    OCI_ListClear(OCILib.cons);

    /* free all pools */

    OCI_ListForEach(OCILib.pools, (boolean (*)(void *)) OCI_ConnPoolClose);
    OCI_ListClear(OCILib.pools);

    /* free objects */

    OCI_KeyMapFree();       

    OCI_ListFree(OCILib.cons);
    OCI_ListFree(OCILib.pools);

    OCILib.cons    = NULL;
    OCILib.pools   = NULL;
    OCILib.key_map = NULL;

    if (OCI_LIB_THREADED)
    {
        if (OCI_SUCCESS != OCIThreadTerm(OCILib.env, OCILib.err))
        {
            OCI_ErrorAPI(OCILib.err, NULL, NULL);
            res = FALSE;
        }
    }

    /* close error handle */
    
    if (OCILib.err != NULL)
        OCIHandleFree(OCILib.err, OCI_HTYPE_ERROR);

    /* close environnement handle */
    
    if (OCILib.env != NULL)
        OCIHandleFree(OCILib.env, OCI_HTYPE_ENV);

#ifdef OCI_IMPORT_RUNTIME

    if (OCILib.lib_handle != NULL)
        LIB_CLOSE(OCILib.lib_handle);

#endif

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_KeyMapFree
 * ------------------------------------------------------------------------ */

void OCI_KeyMapFree(void)
{
    OCI_HashEntry *e;
    OCI_HashValue *v;
    int i, n;

    if (OCILib.key_map == NULL)
        return;

    n = OCI_HashGetSize(OCILib.key_map);

    for (i = 0; i < n; i++)
    {
        e = OCI_HashGetEntry(OCILib.key_map, i);

        while (e != NULL)
        {
            v = e->values;

            while (v != NULL)
            {
                OCI_ThreadKeyFree((OCI_ThreadKey *) v->value.p_void);
                v = v->next;
            }

            e = e->next;
        }
    }

    OCI_HashFree(OCILib.key_map);

    OCILib.key_map = NULL;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetOCICompileVersion
 * ------------------------------------------------------------------------ */

int OCI_API OCI_GetOCICompileVersion(void)
{
    return OCILib.ver_compile;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetOCIRuntimeVersion
 * ------------------------------------------------------------------------ */

int OCI_API OCI_GetOCIRuntimeVersion(void)
{
    return OCILib.ver_runtime;
}

/* ------------------------------------------------------------------------ *
 *                         Error Handling functions
 * ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ *
 * OCI_ErrorAPI
 * ------------------------------------------------------------------------ */

void OCI_ErrorAPI(OCIError *p_err, OCI_Connection *con, OCI_Statement *stmt)
{
    if (OCILib.error_handler != NULL)
    {
        OCI_Error err;

        int osize  = -1;
        void *ostr = NULL;

        memset(&err, 0, sizeof(OCI_Error));

        err.con  = con;
        err.stmt = stmt;
        err.type = OCI_ERR_API;

        /* get oracle description */
     
        osize = msizeof(err.str) - sizeof(mtext);

        ostr  = OCI_GetInputMetaString(err.str, &osize);

        OCIErrorGet(p_err, 1, NULL, &err.oci_code,  (text *) ostr, 
                    (ub4) osize, OCI_HTYPE_ERROR);
           
        OCI_GetOutputMetaString(ostr, err.str, &osize);
        OCI_ReleaseMetaString(ostr);

        /* clear internal error flag before the call to the error handler 
           (necessary for handling  correctly errors with C++ exceptions) */

        if (con != NULL)
            con->status = OCI_SUCCESS;

        OCILib.error_handler(&err);
    }
}

/* ------------------------------------------------------------------------ *
 * OCI_ErrorGeneric
 * ------------------------------------------------------------------------ */

static void OCI_ErrorGeneric(OCI_Connection *con, OCI_Statement *stmt, 
                             int type, int code, mtext *format, ...)
{
    if (OCILib.error_handler != NULL)
    {
        OCI_Error err;
        va_list args;

        memset(&err, 0, sizeof(OCI_Error));

        err.type     = type;
        err.con      = con;
        err.stmt     = stmt;
        err.int_code = code;

        va_start(args, format);

        mtsvprintf(err.str,  msizeof(err.str)-1, format, args);

        va_end(args);

        OCILib.error_handler(&err);
    }
}

/* ------------------------------------------------------------------------ *
 * OCI_ErrorMemory
 * ------------------------------------------------------------------------ */

void OCI_ErrorMemory(OCI_Connection *con, OCI_Statement *stmt, size_t nb_bytes)
{
    OCI_ErrorGeneric(con, stmt, OCI_ERR_MEMORY, errno, 
                     MT("Cannot allocate %d bytes (Error:%d)"), nb_bytes, errno);
}

/* ------------------------------------------------------------------------ *
 * OCI_ErrorVersion
 * ------------------------------------------------------------------------ */

void OCI_ErrorVersion(OCI_Connection *con, OCI_Statement *stmt, int version)
{
    OCI_ErrorGeneric(con, stmt, OCI_ERR_NOT_AVAILABLE, 0, 
                     MT("Oracle %d feature not available"), version);
}

/* ------------------------------------------------------------------------ *
 *                         OCI_ConnPool functions
 * ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ *
 * OCI_ConnPoolCreate
 * ------------------------------------------------------------------------ */

OCI_ConnPool * OCI_API OCI_ConnPoolCreate(const mtext *db, const mtext *user, 
                                          const mtext *pwd, int mode, 
                                          int min_con, int max_con, int incr_con)
{
    OCI_ConnPool *pool = NULL;
    OCI_Item *item     = NULL;

#if OCI_VERSION_COMPILE >= OCI_9
   
    int osize_name  = -1;
    int osize_db    = -1;
    int osize_user  = -1;
    int osize_pwd   = -1;

    void *ostr_name = NULL;
    void *ostr_db   = NULL;
    void *ostr_user = NULL;
    void *ostr_pwd  = NULL;

#endif

    boolean res     = TRUE;

    OCI_CHECK(min_con  < 0, FALSE);
    OCI_CHECK(incr_con < 0, FALSE);
    OCI_CHECK(max_con  < 1, FALSE);

    /* let's be sure OCI_Initialize() has been called */

    OCI_CHECK_NUM(OCILib.ver_runtime, NULL);

    /* create connection pool object */

    item = OCI_ListAppend(OCILib.pools, sizeof(OCI_ConnPool));

    OCI_CHECK_PTR(item, FALSE);

    pool = (OCI_ConnPool *) item->data;

    pool->cons = OCI_ListCreate();    

    if (OCI_LIB_THREADED)
    {
        /* create mutex for OCI_ConnPoolGetConnection() */

        pool->mutex = OCI_MutexCreate();

        if (pool->mutex == NULL)
        {
            OCI_ConnPoolFree(pool);
            return NULL;
        }
    }

    /* set attributes */

    pool->mode = mode;
    pool->min  = min_con;
    pool->max  = max_con;
    pool->incr = incr_con;

    pool->db   = mtsdup(db   != NULL ? db   : MT(""));
    pool->user = mtsdup(user != NULL ? user : MT(""));
    pool->pwd  = mtsdup(pwd  != NULL ? pwd  : MT(""));

    /* create the pool */

#if OCI_VERSION_COMPILE >= OCI_9

    if (OCILib.ver_runtime >= OCI_9)
    {
         /*  allocate error handle */
        
        if (OCI_SUCCESS != OCIHandleAlloc(OCILib.env, (dvoid **) (void *) &pool->err,
                                          OCI_HTYPE_ERROR, 0, (dvoid **) NULL))
        {    
            OCI_ConnPoolFree(pool);
            return NULL;
        }

       /*  allocate connection pool handle */
        
        if (OCI_SUCCESS != OCIHandleAlloc(OCILib.env, (dvoid **) (void *) &pool->handle,
                                          OCI_HTYPE_CPOOL, 0, (dvoid **) NULL))
        {    
            OCI_ConnPoolFree(pool);
            return NULL;
        }

        ostr_db    = OCI_GetInputMetaString(pool->db,   &osize_db);
        ostr_user  = OCI_GetInputMetaString(pool->user, &osize_user);
        ostr_pwd   = OCI_GetInputMetaString(pool->pwd,  &osize_pwd);

        OCI_CALL(pool, OCIConnectionPoolCreate((OCIEnv *) OCILib.env, 
                                               (OCIError *) pool->err,
                                               (OCICPool *) pool->handle,
                                               (OraText **) &ostr_name,
                                               (sb4*) &osize_name, 
                                               (CONST OraText *)ostr_db,
                                               (sb4) osize_db,
                                               (ub4)  min_con,  (ub4) max_con,
                                               (ub4) 1,
                                               (CONST OraText *) ostr_user, 
                                               (sb4) osize_user,
                                               (CONST OraText *) ostr_pwd,  
                                               (sb4) osize_pwd, 
                                               (ub4) OCI_DEFAULT))

        OCI_ReleaseMetaString(ostr_db);
        OCI_ReleaseMetaString(ostr_user);
        OCI_ReleaseMetaString(ostr_pwd);

        if (ostr_name != NULL)
        {
            size_t msize = 0;

            OCI_ALLOC(mtext *, pool->name, msize,
                      ((osize_name/sizeof(omtext))+ 1) * sizeof(mtext));
      
            if (pool->name != NULL)
            {
                OCI_CopyString(ostr_name, pool->name, &osize_name, 
                               sizeof(omtext), sizeof(mtext));
            }
            else
            {
                OCI_ErrorMemory(NULL, NULL, msize);
                res = FALSE;
            }
         }
        else
            res = FALSE;
    }

#endif

    /* handle errors */
    
    if (OCI_NO_ERROR(pool) == FALSE)
    {
        OCI_ErrorAPI(pool->err, NULL, NULL);
        res = FALSE;
    }

    if(res == FALSE)
    {
        OCI_ConnPoolFree(pool);
        pool = NULL;
    }
    else
    {
       OCI_Connection *cn;

       while ((min_con--) > 0)
       {
            cn = OCI_ConnectionAllocate(pool, pool->db, pool->user, 
                                        pool->pwd, pool->mode);

            OCI_ConnectionAttach(cn);
       }
    }

    return pool;
}

/* ------------------------------------------------------------------------ *
 * OCI_ConnPoolFree
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ConnPoolFree(OCI_ConnPool *pool)
{
    OCI_CHECK_PTR(pool, FALSE);

    OCI_ConnPoolClose(pool);
    
    OCI_ListRemove(OCILib.pools, pool);

    OCI_FREE(pool);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_ConnPoolGetConnection
 * ------------------------------------------------------------------------ */

OCI_Connection * OCI_API OCI_ConnPoolGetConnection(OCI_ConnPool *pool)
{
    OCI_Connection *con = NULL;
    OCI_Item *item      = NULL;
    boolean found       = FALSE;

    OCI_CHECK_PTR(pool, FALSE);

    if (OCI_LIB_THREADED)
        OCI_MutexAcquire(pool->mutex);

    /* fist, try to find an unsued OCI_Connection in list */

    item = pool->cons->head;

    while (item != NULL)
    {
        con = (OCI_Connection *) item->data;

        if (con->cstate == OCI_CONN_ATTACHED)
        {
            found = TRUE;
            break;
        }

        item = item->next;
    }

    if (found == FALSE)
    {
        /* no available connection found !
           try to allocate a new one... */

        if (OCILib.ver_runtime >= OCI_9 || pool->cons->count < pool->max)
        {
            int i, nb;
            OCI_Connection *c = NULL;
            
            nb = pool->nb_opened + pool->incr;

            if (nb > pool->max)
                nb = pool->max;

            for (i = 0; i < nb; i++)
            {
                c = OCI_ConnectionAllocate(pool, pool->db, pool->user, 
                                           pool->pwd, pool->mode);

                if (i == 0 && c != NULL)
                   con = c;
            }
        }
    }

    if (con != NULL)
    {
        boolean res = TRUE;

        if (con->cstate == OCI_CONN_ALLOCATED)
            res = res && OCI_ConnectionAttach(con);

        res  = res &&  OCI_ConnectionLogon(con);

        if (res == FALSE)
        {
            OCI_ConnectionFree(con);
            con = NULL;
        }
    }

    if (OCI_LIB_THREADED)
        OCI_MutexRelease(pool->mutex);

    return con;
}

/* ------------------------------------------------------------------------ *
 * OCI_ConnPoolClose
 * ------------------------------------------------------------------------ */

boolean OCI_ConnPoolClose(OCI_ConnPool *pool)
{
    OCI_CHECK_PTR(pool, FALSE);

    /* free all connections */
    
    OCI_ListForEach(pool->cons, (boolean (*)(void *)) OCI_ConnectionClose);
    OCI_ListClear(pool->cons);

    OCI_ListFree(pool->cons);

    pool->cons = NULL;

    if (OCI_LIB_THREADED)
        OCI_MutexFree(pool->mutex);

#if OCI_VERSION_COMPILE >= OCI_9

    if (OCILib.ver_runtime >= OCI_9)
    {
        /* close connection pool handle */

        if (pool->handle != NULL)
            OCIConnectionPoolDestroy(pool->handle, pool->err, OCI_DEFAULT);
   
        /* close error handle */
        
        if (pool->err != NULL)
            OCIHandleFree(pool->err, OCI_HTYPE_ERROR);
    }

#endif
 
    pool->err    = NULL;
    pool->handle = NULL;

    /* free strings */
    
    OCI_FREE(pool->name);
    OCI_FREE(pool->db);
    OCI_FREE(pool->user);
    OCI_FREE(pool->pwd);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_ConnPoolGetTimeout
 * ------------------------------------------------------------------------ */

int OCI_API OCI_ConnPoolGetTimeout(OCI_ConnPool *pool)
{
    OCI_CHECK_PTR(pool, 0);

    return pool->timeout;
}

/* ------------------------------------------------------------------------ *
 * OCI_ConnPoolSetTimeout
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ConnPoolSetTimeout(OCI_ConnPool *pool, int value)
{
    OCI_CHECK_PTR(pool, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    if (OCILib.ver_runtime >= OCI_9)
    {
        ub4 timeout = (ub4) value;

        OCI_CALL(pool, OCIAttrSet(pool->handle, OCI_HTYPE_CPOOL,
                                  &timeout, sizeof(timeout),
                                  OCI_ATTR_CONN_TIMEOUT,
                                  pool->err))

        if (OCI_NO_ERROR(pool) == FALSE)
        {
            OCI_ErrorAPI(pool->err, NULL, NULL);
            return FALSE;
        }
    }

#endif

    pool->timeout = value;

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_ConnPoolGetlGetNoWait
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ConnPoolGetlGetNoWait(OCI_ConnPool *pool)
{
    OCI_CHECK_PTR(pool, FALSE);

    return pool->nowait;
}

/* ------------------------------------------------------------------------ *
 * OCI_ConnPoolSetNoWait
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ConnPoolSetNoWait(OCI_ConnPool *pool, boolean value)
{
    OCI_CHECK_PTR(pool, 0);

#if OCI_VERSION_COMPILE >= OCI_9

    if (OCILib.ver_runtime >= OCI_9)
    {
        ub1 nowait = (ub1) value;

        OCI_CALL(pool, OCIAttrSet(pool->handle, OCI_HTYPE_CPOOL,
                                  &nowait, sizeof(nowait),
                                  OCI_ATTR_CONN_NOWAIT,
                                  pool->err))

        if (OCI_NO_ERROR(pool) == FALSE)
        {
            OCI_ErrorAPI(pool->err, NULL, NULL);
            return FALSE;
        }
    }

#endif

    pool->nowait = value;

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_ConnPoolGetBusyCount
 * ------------------------------------------------------------------------ */

int OCI_API OCI_ConnPoolGetBusyCount(OCI_ConnPool *pool)
{
    OCI_CHECK_PTR(pool, 0);

#if OCI_VERSION_COMPILE >= OCI_9

    if (OCILib.ver_runtime >= OCI_9)
    {
        ub4 value = 0;

        OCI_CALL(pool, OCIAttrGet(pool->handle, OCI_HTYPE_CPOOL, &value, 
                                  NULL, OCI_ATTR_CONN_BUSY_COUNT, pool->err))

        if (OCI_NO_ERROR(pool) == TRUE)
            pool->nb_busy = value;
        else
            OCI_ErrorAPI(pool->err, NULL, NULL);
    }

#endif

    return pool->nb_busy;
}

/* ------------------------------------------------------------------------ *
 * OCI_ConnPoolGetOpenedCount
 * ------------------------------------------------------------------------ */

int OCI_API OCI_ConnPoolGetOpenedCount(OCI_ConnPool *pool)
{
    OCI_CHECK_PTR(pool, 0);

#if OCI_VERSION_COMPILE >= OCI_9

    if (OCILib.ver_runtime >= OCI_9)
    {
        ub4 value = 0;

        OCI_CALL(pool, OCIAttrGet(pool->handle, OCI_HTYPE_CPOOL, &value, 
                                  NULL, OCI_ATTR_CONN_OPEN_COUNT, pool->err))

        if (OCI_NO_ERROR(pool) == TRUE)
            pool->nb_opened = value;
        else
            OCI_ErrorAPI(pool->err, NULL, NULL);
    }

#endif

    return pool->nb_opened;
}

/* ------------------------------------------------------------------------ *
 * OCI_ConnPoolGetMin
 * ------------------------------------------------------------------------ */

int OCI_API OCI_ConnPoolGetMin(OCI_ConnPool *pool)
{
    OCI_CHECK_PTR(pool, 0);

    return pool->min;
}

/* ------------------------------------------------------------------------ *
 * OCI_ConnPoolGetMax
 * ------------------------------------------------------------------------ */

int OCI_API OCI_ConnPoolGetMax(OCI_ConnPool *pool)
{
    OCI_CHECK_PTR(pool, 0);

    return pool->max;
}

/* ------------------------------------------------------------------------ *
 * OCI_ConnPoolGetIncrement
 * ------------------------------------------------------------------------ */

int OCI_API OCI_ConnPoolGetIncrement(OCI_ConnPool *pool)
{
    OCI_CHECK_PTR(pool, 0);

    return pool->incr;
}

/* ------------------------------------------------------------------------ *
 *                         OCI_Connection functions
 * ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ *
 * OCI_ConnectionCreate
 * ------------------------------------------------------------------------ */

OCI_Connection * OCI_API OCI_ConnectionCreate(const mtext *db, 
                                              const mtext *user,
                                              const mtext *pwd, int mode)
{
    OCI_Connection * con;
    
    /* let's be sure OCI_Initialize() has been called */

    OCI_CHECK_NUM(OCILib.ver_runtime, NULL);

    con = OCI_ConnectionAllocate(NULL, db, user, pwd, mode);

    if (con != NULL)
    {
        if (OCI_ConnectionAttach(con) == FALSE ||
            OCI_ConnectionLogon(con)  == FALSE)
        {
            OCI_ConnectionFree(con);
            con = NULL;
        }
    }

    return con;
}

/* ------------------------------------------------------------------------ *
 * OCI_ConnectionAttach
 * ------------------------------------------------------------------------ */

boolean OCI_ConnectionAttach(OCI_Connection *con)
{
    void *ostr  = NULL;
    int osize   = -1;
    boolean res = TRUE;
    ub4 cmode   = OCI_DEFAULT;

    OCI_CHECK_PTR(con, FALSE);
    OCI_CHECK(con->cstate != OCI_CONN_ALLOCATED, FALSE);

    /* attach server handle to service name */

#if OCI_VERSION_COMPILE >= OCI_9

    if (OCILib.ver_runtime >= OCI_9 && con->pool != NULL)
    {
        ostr  = OCI_GetInputMetaString(con->pool->name, &osize);
        cmode = OCI_CPOOL;
    }
    else

#endif

    {
        ostr  = OCI_GetInputMetaString(con->db, &osize);
    }

    OCI_CALL(con, OCIServerAttach(con->svr, con->err, (text*) ostr,
                                  (sb4) osize, (ub4) cmode))

    OCI_ReleaseMetaString(ostr);

    /* handle errors */
    
    if (OCI_NO_ERROR(con) == FALSE)
    {
        OCI_ErrorAPI(con->err, con, NULL);
        res = FALSE;
    }
    else
    {        
        if (OCILib.ver_runtime < OCI_9 && con->pool != NULL)
            con->pool->nb_opened++;

        con->cstate = OCI_CONN_ATTACHED;
    }

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ConnectionDetach
 * ------------------------------------------------------------------------ */

boolean OCI_ConnectionDetach(OCI_Connection *con)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(con, FALSE);
    OCI_CHECK(con->cstate != OCI_CONN_ATTACHED, FALSE);

    con->status = OCI_SUCCESS;

    /* detach from the oracle server */
    
    OCI_CALL(con, OCIServerDetach(con->svr, con->err, OCI_DEFAULT))

    if (OCI_NO_ERROR(con) == FALSE)
    {
        OCI_ErrorAPI(con->err, con, NULL);
        res = FALSE;
    }

    if (OCILib.ver_runtime < OCI_9 && con->pool != NULL)
        con->pool->nb_opened--;

    con->cstate = OCI_CONN_ALLOCATED;

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ConnectionLogon
 * ------------------------------------------------------------------------ */

boolean OCI_ConnectionLogon(OCI_Connection *con)
{
    void *ostr  = NULL;
    int osize   = -1;
    boolean res = TRUE;
    
    OCI_CHECK_PTR(con, FALSE);

   /* set context server attribute */
    
    OCI_CALL(con, OCIAttrSet(con->cxt, OCI_HTYPE_SVCCTX, con->svr,
                             sizeof (con->svr), OCI_ATTR_SERVER, con->err))

    /* set session login attribute */

   if (OCI_NO_ERROR(con) == TRUE && OCI_CHECK_STRING(con->user) == TRUE)
    {
        osize = -1;
        ostr  = OCI_GetInputMetaString(con->user, &osize);

        OCI_CALL(con, OCIAttrSet(con->ses, OCI_HTYPE_SESSION, (text*) ostr,
                                 (sb4) osize, OCI_ATTR_USERNAME,
                                 con->err))

        OCI_ReleaseMetaString(ostr);
    }

    /* set session password attribute */

    if (OCI_NO_ERROR(con) == TRUE && OCI_CHECK_STRING(con->pwd) == TRUE)
    {
        osize = -1;
        ostr  = OCI_GetInputMetaString(con->pwd, &osize);

        OCI_CALL(con, OCIAttrSet(con->ses, OCI_HTYPE_SESSION, (text*) ostr,
                                (sb4) osize, OCI_ATTR_PASSWORD,
                                con->err))

        OCI_ReleaseMetaString(ostr);
    }

    /* start session */
    
    if (OCI_NO_ERROR(con) == TRUE)
    {
        ub4 credt = OCI_CRED_RDBMS;

        if  ((OCI_CHECK_STRING(con->user) == FALSE) &&
             (OCI_CHECK_STRING(con->pwd ) == FALSE))
        {
            credt = OCI_CRED_EXT;
        }

        OCI_CALL(con, OCISessionBegin(con->cxt, con->err, con->ses, credt,
                                      con->mode))
    }

    /* This call has moved after OCISessionBegin() call to enable connection
       pooling (an error ORA-24324 was thrown is the session handle was set to
       the service context handle before OCISessionBegin() */
    
    OCI_CALL(con, OCIAttrSet(con->cxt, OCI_HTYPE_SVCCTX, con->ses,
                             sizeof(con->ses), OCI_ATTR_SESSION, con->err))


    /* create default transaction object */
    
    if (OCI_NO_ERROR(con) == TRUE)
    {
        con->trs  = OCI_TransactionCreate(con, 1, OCI_TRANS_READWRITE, NULL);

        /* start transaction */
        
        if (con->trs != NULL)
            OCI_CALL(con, OCI_TransactionStart(con->trs))
        else
            con->status = OCI_ERROR;
    }

    /* handle errors */
    
    if (OCI_NO_ERROR(con) == FALSE)
    {
        OCI_ErrorAPI(con->err, con, NULL);
        res = FALSE;
    }
    else
    {
        if (OCILib.ver_runtime < OCI_9 && con->pool != NULL)
            con->pool->nb_busy++;

        con->cstate = OCI_CONN_LOGGED;

        /* added check for server version initialization */

        OCI_GetVersionServer(con);
    }

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ConnectionLogOff
 * ------------------------------------------------------------------------ */

boolean OCI_ConnectionLogOff(OCI_Connection *con)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(con, FALSE);
    OCI_CHECK(con->cstate != OCI_CONN_LOGGED, FALSE);

    /* free all statements */
    
    OCI_ListForEach(con->stmts, (boolean (*)(void *)) OCI_StatementClose);
    OCI_ListClear(con->stmts);

    /* free all transactions */
    
    OCI_ListForEach(con->trsns, (boolean (*)(void *)) OCI_TransactionClose);
    OCI_ListClear(con->trsns);

    /* free all schema objects */
    
    OCI_ListForEach(con->sobjs, (boolean (*)(void *)) OCI_SchemaClose);
    OCI_ListClear(con->sobjs);

   /* close any server files not explicitly closed */
   
    if (con->nb_files > 0)
    {
        OCI_DIRECT(con, OCILobFileCloseAll(con->cxt, con->err))
    }

    con->status = OCI_SUCCESS;

    /* close session */
   
    if (con->cxt != NULL && con->err != NULL && con->ses != NULL)
    {
         OCI_CALL(con, OCISessionEnd(con->cxt, con->err, con->ses,
                                     OCI_DEFAULT))
    }

    if (OCI_NO_ERROR(con) == FALSE)
    {
        OCI_ErrorAPI(con->err, con, NULL);
        res = FALSE;
    }
    else
        con->cstate = OCI_CONN_ATTACHED;

    if (OCILib.ver_runtime < OCI_9 && con->pool != NULL)
        con->pool->nb_busy--;


    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ConnectionAllocate
 * ------------------------------------------------------------------------ */

OCI_Connection * OCI_ConnectionAllocate(OCI_ConnPool *pool,
                                        const mtext *db, 
                                        const mtext *user,
                                        const mtext *pwd, 
                                        int mode)
{
    OCI_Connection *con = NULL;
    OCI_List *list      = NULL;
    OCI_Item *item      = NULL;

    /* create connection object */

    if (pool != NULL)
        list  = pool->cons;
    else
        list  = OCILib.cons;

    item = OCI_ListAppend(list, sizeof(OCI_Connection));
 
    OCI_CHECK_PTR(item, FALSE);

    con = (OCI_Connection *) item->data;

    /* create internal lists */

    con->stmts = OCI_ListCreate();    
    con->sobjs = OCI_ListCreate();    
    con->trsns = OCI_ListCreate();
        
    if (con->stmts == NULL || OCILib.cons == con->sobjs || con->trsns == NULL)
    {
        OCI_ConnectionFree(con);
        return NULL;
    }

    /* set attributes */

    con->mode = mode;
    con->pool = pool;

    if (con->pool != NULL)
    {
        con->db     = (mtext *) db;
        con->user   = (mtext *) user;
        con->pwd    = (mtext *) pwd;
    }
    else
    {
        con->db   = mtsdup(db   != NULL ? db   : MT(""));
        con->user = mtsdup(user != NULL ? user : MT(""));
        con->pwd  = mtsdup(pwd  != NULL ? pwd  : MT(""));
    }

    /*  allocate error handle */
    
    OCI_DIRECT(con, OCIHandleAlloc(OCILib.env, (dvoid **) (void *) &con->err,
                                 OCI_HTYPE_ERROR, 0, (dvoid **) NULL))

    /* allocate server handle */
    
    OCI_CALL(con, OCIHandleAlloc(OCILib.env, (dvoid **) (void *) &con->svr,
                                 OCI_HTYPE_SERVER, 0, (dvoid **) NULL))

    /* allocate context handle */
    
    OCI_CALL(con, OCIHandleAlloc(OCILib.env, (dvoid **) (void *) &con->cxt,
                                 OCI_HTYPE_SVCCTX, 0, (dvoid **) NULL))

    /* allocate session handle */
    
    OCI_CALL(con, OCIHandleAlloc(OCILib.env, (dvoid **) (void *) &con->ses,
                                 OCI_HTYPE_SESSION, 0, (dvoid **) NULL))


    /* handle errors */
    
    if (OCI_NO_ERROR(con) == FALSE)
    {
        OCI_ErrorAPI(con->err, con, NULL);
        OCI_ConnectionFree(con);
        con = NULL;
    }

    con->cstate = OCI_CONN_ALLOCATED;

    return con;
}

/* ------------------------------------------------------------------------ *
 * OCI_ConnectionDeallocate
 * ------------------------------------------------------------------------ */

boolean OCI_ConnectionDeallocate(OCI_Connection *con)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(con, FALSE);
    OCI_CHECK(con->cstate != OCI_CONN_ALLOCATED, FALSE);

    con->status = OCI_SUCCESS;

    /* close context handle */
    
    if(con->cxt != NULL)
        OCI_DIRECT(con, OCIHandleFree(con->cxt, OCI_HTYPE_SVCCTX))

    /* close session handle */
    
    if (con->ses != NULL)
        OCI_DIRECT(con, OCIHandleFree(con->ses, OCI_HTYPE_SESSION))

    /* close server handle */
    if (con->svr != NULL)
        OCI_DIRECT(con, OCIHandleFree(con->svr, OCI_HTYPE_SERVER))

     /* close error handle */
    
    if (con->err != NULL)
        OCI_DIRECT(con, OCIHandleFree(con->err, OCI_HTYPE_ERROR))

    if (OCI_NO_ERROR(con) == FALSE)
    {
        OCI_ErrorAPI(con->err, con, NULL);
        res = FALSE;
    }

    con->cxt = NULL;
    con->ses = NULL;
    con->svr = NULL;
    con->err = NULL;

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ConnectionClose
 * ------------------------------------------------------------------------ */

boolean OCI_ConnectionClose(OCI_Connection *con)
{
    OCI_CHECK_PTR(con, FALSE);

    /* clear server output ressources */

    OCI_ServerDisableOutput(con);

    /* lofoff and detatch form server */

    OCI_ConnectionLogOff(con);
    OCI_ConnectionDetach(con);
    OCI_ConnectionDeallocate(con);

    /* free internal lists */

    OCI_ListFree(con->stmts);
    OCI_ListFree(con->trsns);
    OCI_ListFree(con->sobjs);

    if (con->pool == NULL)
    {
        /* free strings */
        
        OCI_FREE(con->db);
        OCI_FREE(con->user);
        OCI_FREE(con->pwd);
    }

    OCI_FREE(con->version);

    con->stmts = NULL;
    con->trsns = NULL;
    con->sobjs = NULL;

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_ConnectionFree
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ConnectionFree(OCI_Connection *con)
{
    OCI_CHECK_PTR(con, FALSE);
   
    if (con->pool != NULL)
    {
        OCI_ConnectionLogOff(con);
    
        if (OCILib.ver_runtime >= OCI_9)
            OCI_ConnectionDetach(con);
    }
    else
    {
        OCI_ConnectionClose(con);
        OCI_ListRemove(OCILib.cons, con);
        OCI_FREE(con);
    }

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_Commit
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_Commit(OCI_Connection *con)
{
    OCI_CHECK_PTR(con, FALSE);

    con->status = OCI_SUCCESS;

    OCI_CALL(con, OCITransCommit(con->cxt, con->err, OCI_DEFAULT))

    OCI_CHECK_ERROR(con, NULL, FALSE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_Rollback
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_Rollback(OCI_Connection *con)
{
    OCI_CHECK_PTR(con, FALSE);

    con->status = OCI_SUCCESS;

    OCI_CALL(con, OCITransRollback(con->cxt, con->err, OCI_DEFAULT))

    OCI_CHECK_ERROR(con, NULL, FALSE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_SetAutoCommit
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_SetAutoCommit(OCI_Connection *con, boolean enable)
{
    OCI_CHECK_PTR(con, FALSE);

    con->autocom = enable;

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetAutoCommit
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_GetAutoCommit(OCI_Connection *con)
{
    OCI_CHECK_PTR(con, FALSE);

    return con->autocom;
}


/* ------------------------------------------------------------------------ *
 * OCI_IsConnected
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_IsConnected(OCI_Connection *con)
{
    ub4 status = 0;

    OCI_CHECK_PTR(con, FALSE);


    OCI_CALL(con, OCIAttrGet(con->svr, OCI_HTYPE_SERVER, &status, NULL, 
                             OCI_ATTR_SERVER_STATUS, con->err))

    OCI_CHECK_ERROR(con, NULL, FALSE);

    return (status == OCI_SERVER_NORMAL);
}

/* ------------------------------------------------------------------------ *
 * OCI_GetUserData
 * ------------------------------------------------------------------------ */

const void * OCI_API OCI_GetUserData(OCI_Connection *con)
{
    OCI_CHECK_PTR(con, FALSE);

    return con->usrdata;  
}

/* ------------------------------------------------------------------------ *
 * OCI_SetSetData
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_SetSetData(OCI_Connection *con, const void * data)
{
    OCI_CHECK_PTR(con, FALSE);

    con->usrdata = data;  

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetDatabase
 * ------------------------------------------------------------------------ */

const mtext * OCI_API OCI_GetDatabase(OCI_Connection *con)
{
    OCI_CHECK_PTR(con, NULL);

    return con->db;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetUserName
 * ------------------------------------------------------------------------ */

const mtext * OCI_API OCI_GetUserName(OCI_Connection *con)
{
    OCI_CHECK_PTR(con, NULL);

    return con->user;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetPassword
 * ------------------------------------------------------------------------ */

const mtext * OCI_API OCI_GetPassword(OCI_Connection *con)
{
    OCI_CHECK_PTR(con, NULL);

    return con->pwd;
}

/* ------------------------------------------------------------------------ *
 * OCI_SetPassword
 * ------------------------------------------------------------------------ */

OCI_EXPORT boolean OCI_API OCI_SetPassword(OCI_Connection *con, 
                                           const mtext *password)
{
    OCI_CHECK_PTR(con, FALSE);
    OCI_CHECK_PTR(password, FALSE);

    OCI_CALL(con, OCIPasswordChange(con->cxt, con->err, 
                                    (CONST OraText *) con->user, 
                                    (ub4) mtextsize(con->user),
                                    (CONST OraText *) con->pwd,  
                                    (ub4) mtextsize(con->pwd),
                                    (CONST OraText *) password,  
                                    (ub4) mtextsize(password),
                                    OCI_DEFAULT))

    OCI_CHECK_ERROR(con, NULL, FALSE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetSessionMode
 * ------------------------------------------------------------------------ */

int OCI_API OCI_GetSessionMode(OCI_Connection *con)
{
    OCI_CHECK_PTR(con, 0);

    return con->mode;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetVersionServer
 * ------------------------------------------------------------------------ */

const mtext * OCI_API OCI_GetVersionServer(OCI_Connection *con)
{
    OCI_CHECK_PTR(con, NULL);

    if (con->version == NULL)
    {
        size_t msize = 0;

        OCI_ALLOC(mtext *, con->version, msize, 
                  (OCI_SIZE_BUFFER+1) * sizeof(mtext));

        if (con->version != NULL)
        {
            int osize  = OCI_SIZE_BUFFER * sizeof(mtext);
            void *ostr = NULL;
            mtext *p   = NULL;
 
            ostr = OCI_GetInputMetaString(con->version, &osize);

            OCI_CALL(con, OCIServerVersion(con->cxt, con->err,
                                           (text*) ostr, (ub4) osize,
                                           OCI_HTYPE_SVCCTX))

            OCI_GetOutputMetaString(ostr, con->version, &osize);
            OCI_ReleaseMetaString(ostr);

            OCI_CHECK_ERROR(con, NULL, NULL);

            /* parse server version string to find the version information */

            for (p = con->version; (p != NULL) && (*p != 0); p++)
            {
                if (mtisdigit(*p) && (*(p+1) != 0) &&
                    (*(p+1) == MT('.') || (*(p+2) == MT('.') )))
                {
                    if (OCI_NB_ARG_VERSION != mtsscanf(p, MT("%i.%i.%i"),
                                                       &con->ver_maj,
                                                       &con->ver_min,
                                                       &con->ver_rev))
                    {
                        /* extracting version info failed ! */

                        con->ver_maj = 0;
                        con->ver_min = 0;
                        con->ver_rev = 0;
                    }

                    break;
                }
            }
        }
        else
        {
            OCI_ErrorMemory(con, NULL, msize);
        }
    }

    return con->version;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetServerMajorVersion
 * ------------------------------------------------------------------------ */

int OCI_API OCI_GetServerMajorVersion(OCI_Connection *con)
{
    OCI_CHECK_PTR(con, 0);

    if (con->ver_maj == 0)
        OCI_GetVersionServer(con);

    return con->ver_maj;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetServerMinorVersion
 * ------------------------------------------------------------------------ */

int OCI_API OCI_GetServerMinorVersion(OCI_Connection *con)
{
    OCI_CHECK_PTR(con, 0);

    if (con->ver_min == 0)
        OCI_GetVersionServer(con);

    return con->ver_min;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetServerRevisionVersion
 * ------------------------------------------------------------------------ */

int OCI_API OCI_GetServerRevisionVersion(OCI_Connection *con)
{
    OCI_CHECK_PTR(con, 0);

    if (con->ver_rev == 0)
        OCI_GetVersionServer(con);

    return con->ver_rev;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetTransaction
 * ------------------------------------------------------------------------ */

OCI_Transaction * OCI_API OCI_GetTransaction(OCI_Connection *con)
{
    OCI_CHECK_PTR(con, NULL);

    return con->trs;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetTransaction
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_SetTransaction(OCI_Connection *con, OCI_Transaction *trans)
{
    OCI_CHECK_PTR(con, FALSE);
    OCI_CHECK_PTR(trans, FALSE);

    if (OCI_TransactionStop(con->trs) == TRUE)
    {
        con->trs = trans;
        return TRUE;
    }

    return FALSE;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetVersionConnection
 * ------------------------------------------------------------------------ */

int OCI_API OCI_GetVersionConnection(OCI_Connection *con)
{
    int v1, v2;

    OCI_CHECK_PTR(con, OCI_UNKNOWN);

    v1 = OCI_GetOCIRuntimeVersion();
    v2 = OCI_GetServerMajorVersion(con);

    /* return the minimum supported version */

    return (v1 > v2) ? v2 : v1;
}

/* ------------------------------------------------------------------------ *
 * OCI_Break
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_Break(OCI_Connection *con)
{
    OCI_CHECK_PTR(con, FALSE);

    OCI_DIRECT(con, OCIBreak(con->cxt, con->err))

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_ServerEnableOutput
 * ------------------------------------------------------------------------ */

boolean OCI_ServerEnableOutput(OCI_Connection *con, 
                               unsigned int bufsize,
                               unsigned int arrsize,
                               unsigned int lnsize)
{
    boolean res = TRUE;
    int msize   = 0;

    OCI_CHECK_PTR(con, FALSE);

    /* initialize the output buffer on server side */

    if (con->svopt == NULL)
    {
        OCI_ALLOC(OCI_ServerOutput *, con->svopt, msize, sizeof(OCI_ServerOutput));

        if (con->svopt == NULL)
        {
            OCI_ErrorMemory(con, NULL, msize);
            return FALSE;
        }

        /* check params ranges */

        if (con->ver_maj > OCI_10 || (con->ver_maj == OCI_10 && con->ver_min >= 2))
        {
            if (lnsize < OCI_OUPUT_LSIZE)
                lnsize = OCI_OUPUT_LSIZE;
            else if (lnsize > OCI_OUPUT_LSIZE_10G)
                lnsize = OCI_OUPUT_LSIZE_10G;      
        }
        else
        {
            if (lnsize > OCI_OUPUT_LSIZE)
                lnsize = OCI_OUPUT_LSIZE;
        }

        con->svopt->arrsize = arrsize;
        con->svopt->lnsize  = lnsize;

        OCI_ALLOC(ub1 *, con->svopt->arrbuf, msize, 
                  (con->svopt->lnsize + sizeof(dtext)) * con->svopt->arrsize);

        if (con->svopt->arrbuf == NULL)
        {
            OCI_ErrorMemory(con, NULL, msize);
            OCI_FREE(con->svopt);
            return FALSE;
        }

        con->svopt->stmt = OCI_StatementCreate(con);
        
        if (con->svopt->stmt == NULL)
        {
            OCI_ServerDisableOutput(con);
            return FALSE;
        }

        res = res && OCI_Prepare(con->svopt->stmt, 
                                 MT("BEGIN DBMS_OUTPUT.ENABLE(:n); END;"));

        res = res && OCI_BindUnsignedInt(con->svopt->stmt, MT(":n"), &bufsize);

        if (bufsize == 0)
            OCI_SetNull(con->svopt->stmt, 1);

        res = res && OCI_Execute(con->svopt->stmt);
    }

    /* prepare the retreival statement call */

    if (res == TRUE)
    {
        con->svopt->cursize = con->svopt->arrsize;

        res = res && OCI_Prepare(con->svopt->stmt, 
                                 MT("BEGIN DBMS_OUTPUT.GET_LINES(:s, :i); END;"));
        
        res = res && OCI_BindArrayOfStrings(con->svopt->stmt,
                                            MT(":s"), 
                                            (dtext*) con->svopt->arrbuf,
                                            con->svopt->lnsize, 
                                            con->svopt->arrsize);

        res = res && OCI_BindUnsignedInt(con->svopt->stmt, 
                                         MT(":i"),
                                         &con->svopt->cursize);
    }

    if (res == FALSE)
        OCI_ServerDisableOutput(con);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ServerDisableOutput
 * ------------------------------------------------------------------------ */

boolean OCI_ServerDisableOutput(OCI_Connection *con)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(con, FALSE);

    if (con->svopt != NULL)
    {
        res = OCI_ExecuteStmt(con->svopt->stmt, 
                              MT("BEGIN DBMS_OUTPUT.DISABLE(); END;"));

        OCI_StatementFree(con->svopt->stmt);  
        OCI_FREE(con->svopt->arrbuf);
        OCI_FREE(con->svopt);

    }

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ServerGetOutput
 * ------------------------------------------------------------------------ */

const dtext * OCI_API OCI_ServerGetOutput(OCI_Connection *con)
{
   dtext *res = NULL;

    OCI_CHECK_PTR(con, FALSE);
    OCI_CHECK_PTR(con->svopt, FALSE);

    if (con->svopt->curpos == 0 || con->svopt->curpos >= con->svopt->cursize)
        OCI_Execute(con->svopt->stmt);

    if (con->svopt->cursize > 0)
        res = (dtext*) (con->svopt->arrbuf + 
                        (((con->svopt->lnsize + 1) * sizeof(dtext)) * con->svopt->curpos++));

    return (const dtext *) res;
}

/* ------------------------------------------------------------------------ *
 *                         OCI_Transaction functions
 * ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ *
 * OCI_TransactionCreate
 * ------------------------------------------------------------------------ */

OCI_Transaction * OCI_API OCI_TransactionCreate(OCI_Connection *con,
                                                int timeout, int mode,
                                                OCI_XID *pxid)
{
    OCI_Item *item;
    OCI_Transaction *trans;

    OCI_CHECK_PTR(con, NULL);

    /* create transaction object */

    item = OCI_ListAppend(con->trsns, sizeof(OCI_Transaction));

    OCI_CHECK_PTR(item, NULL);

    trans = (OCI_Transaction *) item->data;

    trans->con      = con;
    trans->mode     = mode;
    trans->timeout  = timeout;
    trans->local    = (pxid == NULL);

    /* allocate transaction handle */

    OCI_CALL(trans->con, OCIHandleAlloc(OCILib.env,
                                        (dvoid **) (void *) &trans->htr,
                                        OCI_HTYPE_TRANS, 0,
                                        (dvoid **) NULL))

    /* set context transaction attribute */

    OCI_CALL(con, OCIAttrSet(trans->con->cxt,
                             OCI_HTYPE_SVCCTX,
                             trans->htr, 
                             sizeof(trans->htr),
                             OCI_ATTR_TRANS,
                             trans->con->err))

    /* set XID attribute for global transaction */

    if (pxid != NULL)
    {
        memcpy(&trans->xid , pxid, sizeof(trans->xid));

        OCI_CALL(con, OCIAttrSet((dvoid *) trans->htr,
                                  OCI_HTYPE_TRANS,
                                  (dvoid *) &trans->xid,
                                  sizeof(trans->xid),
                                  OCI_ATTR_XID,
                                  trans->con->err))
    }

   /* handle errors */

    if (trans->con->status != OCI_SUCCESS)
    {
        OCI_ErrorAPI(trans->con->err, trans->con, NULL);
        OCI_TransactionFree(trans);
        trans = NULL;
    }

    return trans;
}

/* ------------------------------------------------------------------------ *
 * OCI_TransactionClose
 * ------------------------------------------------------------------------ */

boolean OCI_TransactionClose(OCI_Transaction * trans)
{
    return OCI_TransactionStop(trans);
}

/* ------------------------------------------------------------------------ *
 * OCI_TransactionFree
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TransactionFree(OCI_Transaction * trans)
{
    OCI_Connection *con = NULL;

    OCI_CHECK_PTR(trans, FALSE);

    con = trans->con;

    OCI_TransactionStop(trans);

    /* close transaction handle */

    if (trans->htr != NULL)
        OCI_DIRECT(con, OCIHandleFree(trans->htr, OCI_HTYPE_TRANS))

    /* remove transaction from internal list */

    OCI_ListRemove(trans->con->trsns, trans);

    OCI_FREE(trans);

    /* handle errors */

    OCI_CHECK_ERROR(con, NULL, FALSE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_TransactionStart
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TransactionStart(OCI_Transaction * trans)
{
    OCI_CHECK_PTR(trans, FALSE);

    /* start transaction */

    OCI_CALL(trans->con, OCITransStart(trans->con->cxt, trans->con->err,
                                       trans->timeout, trans->mode))

    /* handle errors */

    OCI_CHECK_ERROR(trans->con, NULL, FALSE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_TransactionStop
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TransactionStop(OCI_Transaction * trans)
{
    OCI_CHECK_PTR(trans, FALSE);

    /* commit or rollback upon auto commit mode */

    if (trans->con->autocom == TRUE)
        OCI_Commit(trans->con);
    else
        OCI_Rollback(trans->con);

    if (trans->local == FALSE)
    {
        OCI_CALL(trans->con, OCITransDetach(trans->con->cxt,
                                            trans->con->err,
                                            OCI_DEFAULT))
    }

    /* handle errors */

    OCI_CHECK_ERROR(trans->con, NULL, FALSE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_TransactionResume
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TransactionResume(OCI_Transaction * trans)
{
    OCI_CHECK_PTR(trans, FALSE);

    /* resume transaction */

    OCI_CALL(trans->con, OCITransStart(trans->con->cxt, trans->con->err,
                                       trans->timeout, OCI_TRANS_RESUME))

    /* handle errors */

    OCI_CHECK_ERROR(trans->con, NULL, FALSE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_TransactionPrepare
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TransactionPrepare(OCI_Transaction * trans)
{
    OCI_CHECK_PTR(trans, FALSE);

    trans->con->status = OCI_SUCCESS;

    OCI_CALL(trans->con, OCITransPrepare(trans->con->cxt,
                                         trans->con->err,
                                         OCI_DEFAULT))

    OCI_CHECK_ERROR(trans->con, NULL, FALSE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_TransactionForget
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TransactionForget(OCI_Transaction * trans)
{
    OCI_CHECK_PTR(trans, FALSE);

    trans->con->status = OCI_SUCCESS;

    OCI_CALL(trans->con, OCITransForget(trans->con->cxt,
                                        trans->con->err,
                                        OCI_DEFAULT))

    OCI_CHECK_ERROR(trans->con, NULL, FALSE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_TransactionGetMode
 * ------------------------------------------------------------------------ */

int OCI_API OCI_TransactionGetMode(OCI_Transaction * trans)
{
    OCI_CHECK_PTR(trans, OCI_ERROR);

    return trans->mode;
}

/* ------------------------------------------------------------------------ *
 * OCI_TransactionGetTimeout
 * ------------------------------------------------------------------------ */

int OCI_API OCI_TransactionGetTimeout(OCI_Transaction * trans)
{
    OCI_CHECK_PTR(trans, OCI_ERROR);

    return trans->timeout;
}

/* ------------------------------------------------------------------------ *
 *                         OCI_Statement functions
 * ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ *
 * OCI_ReleaseResultsets
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ReleaseResultsets(OCI_Statement *stmt)
{
    ub4 i;

    OCI_CHECK_PTR(stmt, FALSE);

    if (stmt->rsts != NULL)
    {
        for (i = 0; i  < stmt->nb_rs; i++)
        {
            if (stmt->rsts[i] != NULL)
               OCI_ResultsetFree(stmt->rsts[i]);
        }

        OCI_FREE(stmt->rsts);
    }

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobInit
 * ------------------------------------------------------------------------ */

OCI_Statement * OCI_StatementInit(OCI_Connection *con, OCI_Statement **pstmt,
                                  OCIStmt *handle, OCI_Define *def)
{
    OCI_Statement * stmt = NULL;
    size_t msize  = 0;

    OCI_CHECK_PTR(con, NULL);
    OCI_CHECK_PTR(pstmt, NULL);

    if((*pstmt) == NULL)
        OCI_ALLOC(OCI_Statement *, *pstmt, msize, sizeof(OCI_Statement));

    stmt = *pstmt;

    if (stmt != NULL)
    {
        stmt->con           = con;
        stmt->stmt          = handle;

        stmt->long_size     = OCI_SIZE_LONG;
        stmt->bind_mode     = OCI_BIND_BY_NAME;
        stmt->long_mode     = OCI_LONG_EXPLICIT;
    }
    else
        OCI_ErrorMemory(con, NULL, msize);

    OCI_CHECK_PTR(stmt, NULL);

    /* reset statement */

    OCI_StatementReset(stmt);

    if (def == NULL)
    {
        /* allocate handle for non fetched lob (temporay lob) */

        stmt->hstate = OCI_OBJECT_ALLOCATED;

        /* allocate handle */

        OCI_CALL(stmt->con, OCIHandleAlloc(OCILib.env,
                                       (dvoid **) (void *) &stmt->stmt,
                                       OCI_HTYPE_STMT, 0, (dvoid **) NULL))

        if (stmt->con->status != OCI_SUCCESS)
        {
            OCI_ErrorAPI(stmt->con->err, stmt->con, stmt);
            OCI_FREE(stmt);
        }
    }
    else
    {
        stmt->hstate = OCI_OBJECT_FETCHED_CLEAN;
        stmt->status = OCI_STMT_EXECUTED;
        stmt->type   = OCI_CST_SELECT;
     
        /* not really perfect, but better than nothing */

        if (def->col.name != NULL)
            stmt->sql = mtsdup(def->col.name);
    }

    /* set statement default attributes */

    if (stmt != NULL)
    {
        OCI_SetPrefetchSize(stmt, OCI_PREFETCH_SIZE);
        OCI_SetFetchSize(stmt, OCI_FETCH_SIZE);
    }

    return stmt; 
}

/* ------------------------------------------------------------------------ *
 * OCI_StatementReset
 * ------------------------------------------------------------------------ */

boolean OCI_StatementReset(OCI_Statement *stmt)
{
    OCI_CHECK_PTR(stmt, FALSE);

    OCI_FREE(stmt->sql);

    /* free resultsets */

    OCI_ReleaseResultsets(stmt);

    /* free in/out binds */

    OCI_BindFreeAll(stmt);

    stmt->rsts        = NULL;
    stmt->sql         = NULL;

    stmt->status      = OCI_STMT_CLOSED;
    stmt->type        = OCI_UNKNOWN;
    stmt->fetch_mode  = OCI_DEFAULT;
    stmt->piecewise   = FALSE;

    stmt->nb_iters     = 1;
    stmt->dynidx       = 0;

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_StatementCreate
 * ------------------------------------------------------------------------ */

OCI_Statement * OCI_API OCI_StatementCreate(OCI_Connection *con)
{
    OCI_Item *item;
    OCI_Statement *stmt;

    OCI_CHECK_PTR(con, NULL);

    /* create statement object */

    item = OCI_ListAppend(con->stmts, sizeof(OCI_Statement));

    OCI_CHECK_PTR(item, NULL);
 
    stmt = (OCI_Statement *) item->data;

    OCI_StatementInit(con, &stmt, NULL, FALSE);

    /* remove item from list if statement handle has not been allocated */

    if (stmt != NULL && stmt->stmt == NULL)
        OCI_ListRemove(con->stmts, stmt);

    return stmt;
}

/* ------------------------------------------------------------------------ *
 * OCI_StatementClose
 * ------------------------------------------------------------------------ */

boolean OCI_StatementClose(OCI_Statement *stmt)
{
    OCI_CHECK_PTR(stmt, FALSE);

    /* reset data */

    OCI_StatementReset(stmt);

    if (stmt->stmt != NULL && stmt->hstate == OCI_OBJECT_ALLOCATED)
        OCI_DIRECT(stmt->con, OCIHandleFree(stmt->stmt, OCI_HTYPE_STMT))

    OCI_FREE(stmt->fmt_date);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_StatementFree
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_StatementFree(OCI_Statement *stmt)
{
    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK(stmt->hstate != OCI_OBJECT_ALLOCATED, FALSE);

    OCI_StatementClose(stmt);

    OCI_ListRemove(stmt->con->stmts, stmt);

    OCI_FREE(stmt);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_Prepare
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_Prepare(OCI_Statement *stmt, const mtext *sql)
{
    void *ostr = NULL;
    int  osize = -1;

    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(sql, FALSE);

    /* reset statement */

    OCI_StatementReset(stmt);

    /* store SQL */

    stmt->sql = mtsdup(sql);

    ostr = OCI_GetInputMetaString(stmt->sql, &osize);

    /* prepare SQL */
    OCI_CALL(stmt->con, OCIStmtPrepare(stmt->stmt,stmt->con->err,
                                       (text*) ostr, (sb4) osize,
                                       OCI_NTV_SYNTAX, OCI_DEFAULT))

    OCI_ReleaseMetaString(ostr);

    /* get statement type */

    OCI_CALL(stmt->con, OCIAttrGet(stmt->stmt, OCI_HTYPE_STMT, &stmt->type,
                                   NULL, OCI_ATTR_STMT_TYPE, stmt->con->err))

    /* handle errors */

    if (stmt->con->status != OCI_SUCCESS)
    {
        OCI_ErrorAPI(stmt->con->err, stmt->con, stmt);
        stmt->status = OCI_STMT_CLOSED;

        return FALSE;
    }

   stmt->status = OCI_STMT_PREPARED;

   return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_Execute
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_Execute(OCI_Statement *stmt)
{
    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK(stmt->status == OCI_STMT_CLOSED, FALSE);
    OCI_CHECK(stmt->con->status != OCI_SUCCESS, FALSE);

    /* check bind objects for updating their null indicator status */

    OCI_BindCheck(stmt);

    /* free previous resulsets in case of re-execution of the same SQL order */

    OCI_ReleaseResultsets(stmt);

    /* Oracle execute call */

    OCI_CALL(stmt->con, OCIStmtExecute(stmt->con->cxt, stmt->stmt,
                                       stmt->con->err,
                                       (stmt->type == OCI_CST_SELECT) ? 0 : stmt->nb_iters,
                                       0, NULL, NULL, stmt->fetch_mode))

    /* reset input binds indicators status */

    OCI_BindReset(stmt);

    /* handle errors */

    if (!(stmt->con->status == OCI_SUCCESS ||
          stmt->con->status == OCI_NEED_DATA))
    {
        OCI_ErrorAPI(stmt->con->err, stmt->con, stmt);
        return FALSE;
    }

    stmt->status = OCI_STMT_EXECUTED;

    /* commit if necessary */

    if (stmt->con->autocom == TRUE) OCI_Commit(stmt->con);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_ExecuteStmt
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ExecuteStmt(OCI_Statement *stmt, const mtext *sql)
{
    return (OCI_Prepare(stmt, sql) && OCI_Execute(stmt));
}

/* ------------------------------------------------------------------------ *
 * OCI_PrepareFmt
 * ------------------------------------------------------------------------ */

boolean OCI_PrepareFmt(OCI_Statement *stmt, const mtext *sql, ...)
{
    va_list args;
    mtext *sql_fmt;
    int msize, size;
    boolean res;

    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(sql, FALSE);

    res = FALSE;

    /* first, get buffer size */

    va_start(args, sql);

    size = OCI_ParseSqlFmt(stmt, NULL, sql, &args);

    va_end(args);

    if (size > 0)
    {
        /* allocate buffer */

        OCI_ALLOC(mtext *, sql_fmt, msize, (size+1) * sizeof(mtext));

        if (sql_fmt != NULL)
        {
            /* format buffer */

            va_start(args, sql);

            if (OCI_ParseSqlFmt(stmt, sql_fmt, sql, &args) > 0)
            {
               /* parse buffer */

               res = OCI_Prepare(stmt, sql_fmt);
            }

            va_end(args);

            OCI_FREE(sql_fmt);
        }
        else
            OCI_ErrorMemory(stmt->con, NULL, msize);
    }

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ExecuteStmtFmt
 * ------------------------------------------------------------------------ */

boolean OCI_ExecuteStmtFmt(OCI_Statement *stmt, const mtext *sql, ...)
{
    va_list args;
    mtext *sql_fmt;
    int msize, size;
    boolean res;

    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(sql, FALSE);

    res = FALSE;

    /* first, get buffer size */

    va_start(args, sql);

    size = OCI_ParseSqlFmt(stmt, NULL, sql, &args);

    va_end(args);

    if (size > 0)
    {
        /* allocate buffer */

        OCI_ALLOC(mtext *, sql_fmt, msize, (size+1) * sizeof(mtext));

        if (sql_fmt != NULL)
        {
            /* format buffer */

            va_start(args, sql);

            if (OCI_ParseSqlFmt(stmt, sql_fmt, sql, &args) > 0)
            {
                /* prepare and execute SQL buffer */

                res = (OCI_Prepare(stmt, sql_fmt) &&  OCI_Execute(stmt));
            }

            va_end(args);

            OCI_FREE(sql_fmt);
        }
        else
            OCI_ErrorMemory(stmt->con, NULL, msize);
    }

    return res;
}


/* ------------------------------------------------------------------------ *
 * OCI_FetchIntoUserVariables
 * ------------------------------------------------------------------------ */

boolean OCI_FetchIntoUserVariables(OCI_Statement *stmt, va_list args)
{
    OCI_Resultset *rs;
    int i, n;
    boolean res;

    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(args, FALSE);

    /* get resultset */

    rs  = OCI_GetResultset(stmt);

    /* fetch data */
   
    res = OCI_FetchNext(rs);

    OCI_CHECK_BOOL(res, FALSE);

    /* loop on column list for updating user given placeholders */

    for (i = 1, n = OCI_GetColumnCount(rs); i <= n && res == TRUE; i++)
    {
        OCI_Column *col = OCI_GetColumn(rs, i);

        int type = va_arg(args, int);

        switch (type)
        {
            case OCI_ARG_SHORT:
            {
                short src, *dst;

                src = OCI_GetShort(rs, i);
                dst = va_arg(args, short *);
                
                if (dst != NULL)
                    *dst = src;

                break;        
            }
            case OCI_ARG_USHORT:
            {
               unsigned short src, *dst;

                src = OCI_GetUnsignedShort(rs, i);
                dst = va_arg(args, unsigned short *);
                
                if (dst != NULL)
                    *dst = src;

                break;        
            }
            case OCI_ARG_INT:
            {
                int src, *dst;

                src = OCI_GetInt(rs, i);
                dst = va_arg(args, int *);
                                
                if (dst != NULL)
                    *dst = src;

                break;        
            }
            case OCI_ARG_UINT:
            {
               unsigned int src, *dst;

                src = OCI_GetUnsignedInt(rs, i);
                dst = va_arg(args, unsigned int *);
                
                if (dst != NULL)
                    *dst = src;

                break;        
            }
            case OCI_ARG_BIGINT:
            {
                big_int src, *dst;

                src = OCI_GetBigInt(rs, i);
                dst = va_arg(args, big_int *);
                
                if (dst != NULL)
                    *dst = src;

                break;        
            }
            case OCI_ARG_BIGUINT:
            {
               big_uint src, *dst;

                src = OCI_GetUnsignedBigInt(rs, i);
                dst = va_arg(args, big_uint *);
                
                if (dst != NULL)
                    *dst = src;

                break;        
            }
            case OCI_ARG_DOUBLE:
            {
                double src, *dst;

                src = OCI_GetDouble(rs, i);
                dst = va_arg(args, double *);
                
                if (dst != NULL)
                    *dst = src;

                break;
            }
            case OCI_ARG_DATETIME:
            {
                OCI_Date *src, *dst;

                src = OCI_GetDate(rs, i);
                dst = (OCI_Date *) va_arg(args, OCI_Date *);

                if (src != NULL && dst != NULL)
                    OCI_DateAssign(dst, src);

                break;
            }
            case OCI_ARG_TEXT:
            {
                const dtext *src;
                dtext *dst;

                src = OCI_GetString(rs, i);
                dst = va_arg(args, dtext *);
                
                if (dst != NULL)
                    dst[0] = 0;

                if (dst != NULL && src != NULL)
                    dtscpy(dst, src);

                break;
            }
            case OCI_ARG_RAW:
            {
                OCI_GetRaw(rs, i, va_arg(args, dtext *), col->bufsize);
                break;
            }
            case OCI_ARG_LOB:
            {
                OCI_Lob *src, *dst;

                src = OCI_GetLob(rs, i);
                dst = (OCI_Lob *) va_arg(args, OCI_Lob *);

                if (src != NULL && dst != NULL)
                    OCI_LobAssign(dst, src);

                break;
            }
            case OCI_ARG_FILE:
            {
                OCI_File *src, *dst;

                src = OCI_GetFile(rs, i);
                dst = (OCI_File *) va_arg(args, OCI_File *);

                if (src != NULL && dst != NULL)
                    OCI_FileAssign(dst, src);

                break;
            }
            case OCI_ARG_TIMESTAMP:
            {
                OCI_Timestamp *src, *dst;

                src = OCI_GetTimestamp(rs, i);
                dst = (OCI_Timestamp *) va_arg(args, OCI_Timestamp *);

                if (src != NULL && dst != NULL)
                    OCI_TimestampAssign(dst, src);

                break;
            }
            case OCI_ARG_INTERVAL:
            {
                OCI_Interval *src, *dst;

                src = OCI_GetInterval(rs, i);
                dst = (OCI_Interval *) va_arg(args, OCI_Interval *);

                if (src != NULL && dst != NULL)
                    OCI_IntervalAssign(dst, src);

                break;
            }
            default:
            {
                OCI_ErrorGeneric(stmt->con, stmt, OCI_ERR_ARG_INVALID, 0, 
                                 MT("Invalid argument datatype (%d)"), type);

                res = FALSE;

                break;
            }
        }
    }

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_Immediate
 * ------------------------------------------------------------------------ */

boolean OCI_Immediate(OCI_Connection *con, const mtext *sql, ...)
{
    OCI_Statement *stmt;
    va_list args;
    boolean res;

    OCI_CHECK_PTR(con, FALSE);
    OCI_CHECK_PTR(sql, FALSE);

    res = FALSE;

    /* First, execute SQL */

    stmt = OCI_StatementCreate(con);

    OCI_CHECK_PTR(stmt, FALSE);

    if (OCI_ExecuteStmt(stmt, sql))
    {
        res = TRUE;

        /* get resultset and set up variables */

        if (OCI_GetStatementType(stmt) == OCI_CST_SELECT)
        {
            va_start(args, sql);

            res = OCI_FetchIntoUserVariables(stmt, args);

            va_end(args);
        }
    }

    OCI_StatementFree(stmt);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ImmediateFmt
 * ------------------------------------------------------------------------ */

boolean OCI_ImmediateFmt(OCI_Connection *con, const mtext *sql, ...)
{
    va_list args;
    mtext *sql_fmt;
    int msize, size;
    OCI_Statement *stmt;
    boolean res;

    OCI_CHECK_PTR(con, FALSE);
    OCI_CHECK_PTR(sql, FALSE);

    res = FALSE;

    stmt = OCI_StatementCreate(con);

    OCI_CHECK_PTR(stmt, FALSE);

    /* first, get buffer size */

    va_start(args, sql);

    size = OCI_ParseSqlFmt(stmt, NULL, sql, &args);

    va_end(args);

    if (size > 0)
    {
        /* allocate buffer */

        OCI_ALLOC(mtext *, sql_fmt, msize, (size+1) * sizeof(mtext));

        if (sql_fmt != NULL)
        {
            /* format buffer */

            va_start(args, sql);

            if (OCI_ParseSqlFmt(stmt, sql_fmt, sql, &args) > 0)
            {
                /* prepare and execute SQL buffer */

                res = (OCI_Prepare(stmt, sql_fmt) &&  OCI_Execute(stmt));

                /* get resultset and set up variables */

                if (res && (OCI_GetStatementType(stmt) == OCI_CST_SELECT))
                    res = OCI_FetchIntoUserVariables(stmt, args);
            }

            va_end(args);

            OCI_FREE(sql_fmt);
        }
        else
            OCI_ErrorMemory(con, NULL, msize);
    }

    OCI_StatementFree(stmt);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_BindArraySetSize
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindArraySetSize(OCI_Statement *stmt, int size)
{
    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_NUM(size, FALSE);
    OCI_CHECK_BOOL(stmt->status == OCI_STMT_PREPARED, FALSE);

    stmt->nb_iters = size;

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_BindArrayGetSize
 * ------------------------------------------------------------------------ */

int OCI_API OCI_BindArrayGetSize(OCI_Statement *stmt)
{
    OCI_CHECK_PTR(stmt, FALSE);

    return stmt->nb_iters;
}

/* ------------------------------------------------------------------------ *
 * OCI_BindShort
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindShort(OCI_Statement *stmt, const mtext *name, short *data)
{
    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(data, FALSE);

    return OCI_BindData(stmt, data, sizeof(short), name, OCI_CDT_INTEGER, 
                        SQLT_INT, OCI_BIND_INPUT, OCI_NUM_SHORT, NULL, 1);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindArrayOfShorts
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindArrayOfShorts(OCI_Statement *stmt, const mtext *name, 
                                    short *data, unsigned int nbelem)
{
    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(data, FALSE);

    return OCI_BindData(stmt, data, sizeof(short), name, OCI_CDT_INTEGER,
                        SQLT_INT, OCI_BIND_INPUT, OCI_NUM_SHORT, NULL, nbelem);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindUnsignedShort
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindUnsignedShort(OCI_Statement *stmt, const mtext *name,
                                    unsigned short *data)
{
    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(data, FALSE);

    return OCI_BindData(stmt, data, sizeof(unsigned short), name, OCI_CDT_INTEGER, 
                        SQLT_UIN, OCI_BIND_INPUT, OCI_NUM_USHORT, NULL, 1);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindArrayOfUnsignedShorts
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindArrayOfUnsignedShorts(OCI_Statement *stmt, 
                                            const mtext *name, 
                                            unsigned short *data,
                                            unsigned int nbelem)
{
    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(data, FALSE);

    return OCI_BindData(stmt, data, sizeof(unsigned short), name, OCI_CDT_INTEGER,
                        SQLT_UIN, OCI_BIND_INPUT, OCI_NUM_USHORT, NULL, nbelem);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindInt
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindInt(OCI_Statement *stmt, const mtext *name, int *data)
{
    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(data, FALSE);

    return OCI_BindData(stmt, data, sizeof(int), name, OCI_CDT_INTEGER, 
                        SQLT_INT, OCI_BIND_INPUT, OCI_NUM_INT, NULL, 1);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindArrayOfInts
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindArrayOfInts(OCI_Statement *stmt, const mtext *name, 
                                    int *data, unsigned int nbelem)
{
    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(data, FALSE);

    return OCI_BindData(stmt, data, sizeof(int), name, OCI_CDT_INTEGER,
                        SQLT_INT, OCI_BIND_INPUT, OCI_NUM_INT, NULL, nbelem);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindUnsignedInt
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindUnsignedInt(OCI_Statement *stmt, const mtext *name,
                                    unsigned int *data)
{
    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(data, FALSE);

    return OCI_BindData(stmt, data, sizeof(unsigned int), name, OCI_CDT_INTEGER, 
                        SQLT_UIN, OCI_BIND_INPUT, OCI_NUM_UINT, NULL, 1);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindArrayOfUnsignedInts
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindArrayOfUnsignedInts(OCI_Statement *stmt, const mtext *name, 
                                            unsigned int *data, unsigned int nbelem)
{
    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(data, FALSE);

    return OCI_BindData(stmt, data, sizeof(unsigned int), name, OCI_CDT_INTEGER,
                        SQLT_UIN, OCI_BIND_INPUT, OCI_NUM_UINT, NULL, nbelem);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindBigInt
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindBigInt(OCI_Statement *stmt, const mtext *name,
                               big_int *data)
{
    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(data, FALSE);

    return OCI_BindData(stmt, data, sizeof(OCINumber), name, OCI_CDT_INTEGER, 
                        SQLT_VNU, OCI_BIND_INPUT, OCI_NUM_BIGINT, NULL, 1);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindArrayOfBigInts
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindArrayOfBigInts(OCI_Statement *stmt, const mtext *name, 
                                       big_int *data, unsigned int nbelem)
{
    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(data, FALSE);

    return OCI_BindData(stmt, data, sizeof(OCINumber), name, OCI_CDT_INTEGER,
                        SQLT_VNU, OCI_BIND_INPUT, OCI_NUM_BIGINT, NULL, nbelem);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindUnsignedBigInt
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindUnsignedBigInt(OCI_Statement *stmt, const mtext *name,
                                      big_uint *data)
{
    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(data, FALSE);

    return OCI_BindData(stmt, data, sizeof(OCINumber), name, OCI_CDT_INTEGER, 
                        SQLT_VNU, OCI_BIND_INPUT, OCI_NUM_BIGUINT, NULL, 1);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindArrayOfUnsignedInts
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindArrayOfUnsignedBigInts(OCI_Statement *stmt, 
                                               const mtext *name, 
                                               big_uint *data, 
                                               unsigned int nbelem)
{
    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(data, FALSE);

    return OCI_BindData(stmt, data, sizeof(OCINumber), name, OCI_CDT_INTEGER,
                        SQLT_VNU, OCI_BIND_INPUT, OCI_NUM_BIGUINT, NULL, nbelem);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindString
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindString(OCI_Statement *stmt, const mtext *name, 
                               dtext *data, int len)
{
    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(data, FALSE);

    if (len <= 0)
        len = (int) dtslen(data);

    return OCI_BindData(stmt, data, (len+1)*sizeof(odtext), name, OCI_CDT_TEXT,
                        SQLT_STR, OCI_BIND_INPUT, 0, NULL, 1);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindArrayOfStrings
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindArrayOfStrings(OCI_Statement *stmt, const mtext *name,
                                       dtext *data, int len, unsigned int nbelem)
{
    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(data, FALSE);
    OCI_CHECK(len <= 0, FALSE);

    return OCI_BindData(stmt, data, (len+1)*sizeof(odtext), name, OCI_CDT_TEXT,
                        SQLT_STR, OCI_BIND_INPUT, 0, NULL, nbelem);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindRaw
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindRaw(OCI_Statement *stmt, const mtext *name, void *data,
                               int len)
{
    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(data, FALSE);
    OCI_CHECK(len <= 0, FALSE);

    return OCI_BindData(stmt, data, len, name, OCI_CDT_RAW, SQLT_BIN,
                        OCI_BIND_INPUT, 0, NULL, 1);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindArrayOfRaws
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindArrayOfRaws(OCI_Statement *stmt, const mtext *name,
                                       void  *data, int len, unsigned int nbelem)
{
    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(data, FALSE);
    OCI_CHECK(len <= 0, FALSE);

    return OCI_BindData(stmt, data, len, name, OCI_CDT_RAW, SQLT_BIN,
                        OCI_BIND_INPUT, 0, NULL, nbelem);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindDouble
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindDouble(OCI_Statement *stmt, const mtext *name, 
                               double *data)
{
    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(data, FALSE);

    return OCI_BindData(stmt, data, sizeof(double), name,
                        OCI_CDT_DOUBLE, SQLT_FLT, OCI_BIND_INPUT, 0, NULL, 1);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindArrayOfDoubles
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindArrayOfDoubles(OCI_Statement *stmt, const mtext *name,
                                       double *data, unsigned int nbelem)
{
    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(data, FALSE);

    return OCI_BindData(stmt, data, sizeof(double), name, OCI_CDT_DOUBLE, 
                        SQLT_FLT, OCI_BIND_INPUT, 0, NULL, nbelem);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindDate
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindDate(OCI_Statement *stmt, const mtext *name, 
                             OCI_Date *data)
{
    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(data, FALSE);

    return OCI_BindData(stmt, data, sizeof(OCIDate), name, OCI_CDT_DATETIME,
                        SQLT_ODT, OCI_BIND_INPUT, 0, NULL, 1);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindArrayOfDates
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindArrayOfDates(OCI_Statement *stmt, const mtext *name,
                                     OCI_Date **data, unsigned int nbelem)
{
    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(data, FALSE);

    return OCI_BindData(stmt, data, sizeof(OCIDate), name, OCI_CDT_DATETIME,
                        SQLT_ODT, OCI_BIND_INPUT, 0, NULL, nbelem);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindTimestamp
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindTimestamp(OCI_Statement *stmt, const mtext *name,
                                  OCI_Timestamp *data)
{
    int code    = 0;
    boolean res = FALSE;

    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(data, FALSE);
    OCI_CHECK_VERSION(stmt->con, OCI_9, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    /* map oracle internal type */

    if (data->type == OCI_TIMESTAMP_TZ)
        code = SQLT_TIMESTAMP_TZ;
    else if (data->type == OCI_TIMESTAMP_LTZ)
        code = SQLT_TIMESTAMP_LTZ;
    else
        code = SQLT_TIMESTAMP;

    res = OCI_BindData(stmt, data, sizeof(OCIDateTime *), name,
                       OCI_CDT_TIMESTAMP, code, OCI_BIND_INPUT,
                       data->type, NULL, 1);

#else

    OCI_NOT_USED(name);
    OCI_NOT_USED(code);

#endif

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_BindArrayOfTimestamps
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindArrayOfTimestamps(OCI_Statement *stmt, const mtext *name,
                                          OCI_Timestamp **data, int type,
                                          unsigned int nbelem)
{
    int code    = 0;
    boolean res = FALSE;

    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(data, FALSE);
    OCI_CHECK_VERSION(stmt->con, OCI_9, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    /* map oracle internal type */

    if (type == OCI_TIMESTAMP_TZ)
        code = SQLT_TIMESTAMP_TZ;
    else if (type == OCI_TIMESTAMP_LTZ)
        code = SQLT_TIMESTAMP_LTZ;
    else
        code = SQLT_TIMESTAMP;

    res =  OCI_BindData(stmt, data, sizeof(OCIDateTime *), name,
                        OCI_CDT_TIMESTAMP, code, OCI_BIND_INPUT,
                        type, NULL, nbelem);

#else

    OCI_NOT_USED(name);
    OCI_NOT_USED(type);
    OCI_NOT_USED(code);

#endif

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_BindInterval
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindInterval(OCI_Statement *stmt, const mtext *name,
                                  OCI_Interval *data)
{
    int code    = 0;
    boolean res = FALSE;

    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(data, FALSE);
    OCI_CHECK_VERSION(stmt->con, OCI_9, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    /* map oracle internal type */

    if (data->type == OCI_INTERVAL_YM)
        code = SQLT_INTERVAL_YM;
    else if (data->type == OCI_INTERVAL_DS)
        code = SQLT_INTERVAL_DS;

    res = OCI_BindData(stmt, data, sizeof(OCIInterval *), name,
                       OCI_CDT_INTERVAL, code, OCI_BIND_INPUT,
                       data->type, NULL, 1);

#else

    OCI_NOT_USED(name);
    OCI_NOT_USED(code);

#endif

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_BindArrayOfIntervals
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindArrayOfIntervals(OCI_Statement *stmt, const mtext *name,
                                         OCI_Interval **data, int type,
                                         unsigned int nbelem)
{
    int code    = 0;
    boolean res = FALSE;

    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(data, FALSE);

    OCI_CHECK_VERSION(stmt->con, OCI_9, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    /* map oracle internal type */

    if (type == OCI_INTERVAL_YM)
        code = SQLT_INTERVAL_YM;
    else if (type == OCI_INTERVAL_DS)
        code = SQLT_INTERVAL_DS;

    res = OCI_BindData(stmt, data, sizeof(OCIInterval *), name,
                       OCI_CDT_INTERVAL, code, OCI_BIND_INPUT,
                       type, NULL, nbelem);

#else

    OCI_NOT_USED(name);
    OCI_NOT_USED(type);
    OCI_NOT_USED(code);

#endif

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_BindObject
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindObject(OCI_Statement *stmt, const mtext *name,
                               OCI_Object *data)
{
    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(data, FALSE);

    return OCI_BindData(stmt, data, sizeof(void*), name,
                        OCI_CDT_OBJECT, SQLT_NTY, 
                        OCI_BIND_INPUT, 0, data->nty->tdo, 1);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindArrayOfObjects
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindArrayOfObjects(OCI_Statement *stmt, const mtext *name,
                                       OCI_Object **data, OCI_Schema *schema,
                                       unsigned int nbelem)
{
    OCI_CHECK_PTR(stmt,   FALSE);
    OCI_CHECK_PTR(data,   FALSE);
    OCI_CHECK_PTR(schema, FALSE);

    return OCI_BindData(stmt, data, sizeof(void *), name,
                        OCI_CDT_OBJECT, SQLT_NTY, OCI_BIND_INPUT,
                        0, schema->tdo, nbelem);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindLob
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindLob(OCI_Statement *stmt, const mtext *name, 
                            OCI_Lob *data)
{
    int code = 0;

    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(data, FALSE);

    /* map oracle internal type */

    if (data->type == OCI_CLOB || data->type == OCI_NCLOB)
        code = SQLT_CLOB;
    else
        code = SQLT_BLOB;

    return OCI_BindData(stmt, data, sizeof(OCILobLocator*), name, OCI_CDT_LOB,
                        code, OCI_BIND_INPUT, data->type, NULL, 1);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindArrayOfLobs
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindArrayOfLobs(OCI_Statement *stmt, const mtext *name,
                                    OCI_Lob **data, int type,
                                    unsigned int nbelem)
{
    int code = 0;

    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(data, FALSE);

    /* map oracle internal type */

    if (type == OCI_CLOB || type == OCI_NCLOB)
        code = SQLT_CLOB;
    else
        code = SQLT_BLOB;

    return OCI_BindData(stmt, data, sizeof(OCILobLocator*), name,
                        OCI_CDT_LOB, code, OCI_BIND_INPUT, type, NULL, nbelem);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindFile
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindFile(OCI_Statement *stmt, const mtext *name, 
                             OCI_File *data)
{
    int code = 0;

    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(data, FALSE);

    /* map oracle internal type */

    if (data->type == OCI_CFILE)
        code = SQLT_CFILE;
    else
        code = SQLT_BFILE;

    return OCI_BindData(stmt, data, sizeof(OCILobLocator*), name,
                        OCI_CDT_FILE, code, OCI_BIND_INPUT, data->type, NULL, 1);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindArrayOfFiles
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindArrayOfFiles(OCI_Statement *stmt, const mtext *name,
                                     OCI_File **data, int type,
                                     unsigned int nbelem)
{
    int code = 0;

    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(data, FALSE);

    /* map oracle internal type */

    if (type == OCI_CFILE)
        code = SQLT_CFILE;
    else
        code = SQLT_BFILE;

    return OCI_BindData(stmt, data, sizeof(OCILobLocator*), name,
                        OCI_CDT_FILE, code, OCI_BIND_INPUT, type, NULL, nbelem);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindStatement
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindStatement(OCI_Statement *stmt, const mtext *name,
                                  OCI_Statement *data)
{
    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(data, FALSE);

    OCI_StatementReset(data);

    if (OCI_BindData(stmt, &data->stmt, sizeof(OCIStmt*), name,
                     OCI_CDT_CURSOR, SQLT_RSET, OCI_BIND_INPUT, 0, NULL, 1))
    {
        /* Once stmt is exectuted, Oracle provides a statemment handle
           ready to be fetched  */

        data->status  = OCI_STMT_EXECUTED;
        data->type    = OCI_CST_SELECT;

        return TRUE;
    }

    return FALSE;
}

/* ------------------------------------------------------------------------ *
 * OCI_BindLong
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindLong(OCI_Statement *stmt, const mtext *name, 
                             OCI_Long *data, unsigned int size)
{
    int code = 0;

    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(data, FALSE);

    /* map oracle internal type */

    if (data->type == OCI_CLONG)
        code = SQLT_LNG;
    else
        code = SQLT_LBI;

    if (data->type == OCI_CLONG)
        size *= sizeof(dtext);

    return OCI_BindData(stmt, data, size, name, OCI_CDT_LONG,
                        code, OCI_BIND_INPUT, data->type, NULL, 1);
}

/* ------------------------------------------------------------------------ *
 * OCI_RegisterShort
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_RegisterShort(OCI_Statement *stmt, const mtext *name)
{
    OCI_CHECK_PTR(stmt, FALSE);

    return OCI_BindData(stmt, NULL, sizeof(short), name,
                        OCI_CDT_INTEGER, SQLT_INT, OCI_BIND_OUTPUT, 
                        OCI_NUM_SHORT, NULL, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_RegisterUnsignedShort
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_RegisterUnsignedShort(OCI_Statement *stmt, const mtext *name)
{
    OCI_CHECK_PTR(stmt, FALSE);

    return OCI_BindData(stmt, NULL, sizeof(unsigned short), name,
                        OCI_CDT_INTEGER, SQLT_UIN, OCI_BIND_OUTPUT, 
                        OCI_NUM_USHORT, NULL, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_RegisterInt
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_RegisterInt(OCI_Statement *stmt, const mtext *name)
{
    OCI_CHECK_PTR(stmt, FALSE);

    return OCI_BindData(stmt, NULL, sizeof(int), name,
                        OCI_CDT_INTEGER, SQLT_INT, OCI_BIND_OUTPUT,
                        OCI_NUM_INT, NULL, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_RegisterUnsignedInt
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_RegisterUnsignedInt(OCI_Statement *stmt, const mtext *name)
{
    OCI_CHECK_PTR(stmt, FALSE);

    return OCI_BindData(stmt, NULL, sizeof(unsigned int), name,
                        OCI_CDT_INTEGER, SQLT_UIN, OCI_BIND_OUTPUT, 
                        OCI_NUM_UINT, NULL, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_RegisterBigInt
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_RegisterBigInt(OCI_Statement *stmt, const mtext *name)
{
    OCI_CHECK_PTR(stmt, FALSE);

    return OCI_BindData(stmt, NULL, sizeof(OCINumber), name,
                        OCI_CDT_INTEGER, SQLT_VNU, OCI_BIND_OUTPUT,
                        OCI_NUM_BIGINT, NULL, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_RegisterUnsignedBigInt
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_RegisterUnsignedBigInt(OCI_Statement *stmt, const mtext *name)
{
    OCI_CHECK_PTR(stmt, FALSE);

    return OCI_BindData(stmt, NULL, sizeof(OCINumber), name,
                        OCI_CDT_INTEGER, SQLT_VNU, OCI_BIND_OUTPUT, 
                        OCI_NUM_BIGUINT, NULL, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_RegisterString
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_RegisterString(OCI_Statement *stmt, const mtext *name, 
                                   int len)
{
    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK(len <= 0, FALSE);

    return OCI_BindData(stmt, NULL, (len+1)*sizeof(odtext), name, OCI_CDT_TEXT,
                        SQLT_STR, OCI_BIND_OUTPUT, 0, NULL, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_RegisterRaw
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_RegisterRaw(OCI_Statement *stmt, const mtext *name, int len)
{
    OCI_CHECK_PTR(stmt, FALSE);

    return OCI_BindData(stmt, NULL, len, name, OCI_CDT_RAW, SQLT_BIN,
                        OCI_BIND_OUTPUT, 0, NULL, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_RegisterDouble
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_RegisterDouble(OCI_Statement *stmt, const mtext *name)
{
    OCI_CHECK_PTR(stmt, FALSE);

    return OCI_BindData(stmt, NULL, sizeof(double), name,
                        OCI_CDT_DOUBLE, SQLT_FLT, OCI_BIND_OUTPUT, 0, NULL, 
                        0);
}

/* ------------------------------------------------------------------------ *
 * OCI_RegisterDate
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_RegisterDate(OCI_Statement *stmt, const mtext *name)
{
    int code = SQLT_ODT;
    int size = sizeof(OCIDate);

    OCI_CHECK_PTR(stmt, FALSE);

    /* versions of OCI (< 10.2) crashes if SQLT_ODT is passed for output 
       data with returning clause. 
       It's an Oracle known bug #3269146 */

    if (OCI_GetVersionConnection(stmt->con) < OCI_11)
    {
        code = SQLT_DAT;
        size = 7;
    }

    return OCI_BindData(stmt, NULL, size, name, OCI_CDT_DATETIME,
                        code, OCI_BIND_OUTPUT, 0, NULL, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_RegisterTimestamp
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_RegisterTimestamp(OCI_Statement *stmt, const mtext *name, 
                                      int type)
{
    int code    = 0;
    boolean res = FALSE;

    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_VERSION(stmt->con, OCI_9, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    /* map oracle internal type */

    if (type == OCI_TIMESTAMP_TZ)
        code = SQLT_TIMESTAMP_TZ;
    else if (type == OCI_TIMESTAMP_LTZ)
        code = SQLT_TIMESTAMP_LTZ;
    else
        code = SQLT_TIMESTAMP;

    res = OCI_BindData(stmt, NULL, sizeof(OCIDateTime *), name,
                       OCI_CDT_TIMESTAMP, code, OCI_BIND_OUTPUT, type, NULL, 
                       0);

#else

    OCI_NOT_USED(name);
    OCI_NOT_USED(type);
    OCI_NOT_USED(code);

#endif

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_RegisterInterval
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_RegisterInterval(OCI_Statement *stmt, const mtext *name, 
                                     int type)
{
    int code    = 0;
    boolean res = FALSE;

    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_VERSION(stmt->con, OCI_9, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    /* map oracle internal type */

    if (type == OCI_INTERVAL_YM)
        code = SQLT_INTERVAL_YM;
    else if (type == OCI_INTERVAL_DS)
        code = SQLT_INTERVAL_DS;

    res =  OCI_BindData(stmt, NULL, sizeof(OCIInterval *), name,
                        OCI_CDT_INTERVAL, code, OCI_BIND_OUTPUT, type, NULL, 
                        0);

#else

    OCI_NOT_USED(name);
    OCI_NOT_USED(type);
    OCI_NOT_USED(code);

#endif

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_RegisterObject
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_RegisterObject(OCI_Statement *stmt, const mtext *name,
                                   OCI_Schema *schema)
{
    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(schema, FALSE);

    return OCI_BindData(stmt, NULL, sizeof(OCIInterval *), name,
                        OCI_CDT_OBJECT, SQLT_NTY, OCI_BIND_OUTPUT,
                        0, schema->tdo, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_RegisterLob
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_RegisterLob(OCI_Statement *stmt, const mtext *name, int type)
{
    int code = 0;

    OCI_CHECK_PTR(stmt, FALSE);

    /* map oracle internal type */

    if (type == OCI_CLOB || type == OCI_NCLOB)
        code = SQLT_CLOB;
    else
        code = SQLT_BLOB;

    return OCI_BindData(stmt, NULL, sizeof(OCILobLocator*), name,
                        OCI_CDT_LOB, code, OCI_BIND_OUTPUT, type, NULL, 
                        0);
}

/* ------------------------------------------------------------------------ *
 * OCI_RegisterFile
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_RegisterFile(OCI_Statement *stmt, const mtext *name, 
                                 int type)
{
    int code;

    OCI_CHECK_PTR(stmt, FALSE);

    /* map oracle internal type */

    if (type == OCI_CFILE)
        code = SQLT_CFILE;
    else
        code = SQLT_BFILE;

    return OCI_BindData(stmt, NULL, sizeof(OCILobLocator*), name,
                         OCI_CDT_FILE, code, OCI_BIND_OUTPUT, type, NULL, 
                         0);
}

/* ------------------------------------------------------------------------ *
 * OCI_SetNullAtPos
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_SetNullAtPos(OCI_Statement *stmt, int index, int position)
{
    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(stmt->binds, FALSE);
    OCI_CHECK(index < 0, FALSE);
    OCI_CHECK(index >= stmt->nb_binds, FALSE);
    OCI_CHECK(position < 0, FALSE);
    OCI_CHECK(position >= (int) stmt->nb_iters, FALSE);

    if (stmt->binds[index]->buf.inds != NULL)
        ((sb2*) stmt->binds[index]->buf.inds)[position] = -1;

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_SetNull
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_SetNull(OCI_Statement *stmt, int index)
{
  return OCI_SetNullAtPos(stmt, index, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_GetStatementType
 * ------------------------------------------------------------------------ */

int OCI_API OCI_GetStatementType(OCI_Statement *stmt)
{
    OCI_CHECK_PTR(stmt, OCI_UNKNOWN);

    return stmt->type;
}

/* ------------------------------------------------------------------------ *
 * OCI_SetBindMode
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_SetBindMode(OCI_Statement *stmt, int mode)
{
    OCI_CHECK_PTR(stmt, FALSE);

    stmt->bind_mode = mode;

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetBindMode
 * ------------------------------------------------------------------------ */

int OCI_API OCI_GetBindMode(OCI_Statement *stmt)
{
    OCI_CHECK_PTR(stmt, 0);

    return stmt->bind_mode;
}

/* ------------------------------------------------------------------------ *
 * OCI_SetFetchSize
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_SetFetchSize(OCI_Statement *stmt, int size)
{
     OCI_CHECK_PTR(stmt, FALSE);
     OCI_CHECK(size <= 0, FALSE);

     stmt->fetch_size = size;

     return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetFetchSize
 * ------------------------------------------------------------------------ */

int OCI_API OCI_GetFetchSize(OCI_Statement *stmt)
{
    OCI_CHECK_PTR(stmt, 0);

    return stmt->fetch_size;
}

/* ------------------------------------------------------------------------ *
 * OCI_SetPrefetchSize
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_SetPrefetchSize(OCI_Statement *stmt, int size)
{
    OCI_CHECK_PTR(stmt,  FALSE);
    OCI_CHECK(size < 0,  FALSE);


    OCI_CALL(stmt->con, OCIAttrSet(stmt->stmt, OCI_HTYPE_STMT,
                               &stmt->prefetch_size, 
                               sizeof(stmt->prefetch_size),
                               OCI_ATTR_PREFETCH_ROWS,
                               stmt->con->err))

    OCI_CHECK_ERROR(stmt->con, stmt, FALSE);

    stmt->prefetch_size = size;

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetPrefetchSize
 * ------------------------------------------------------------------------ */

int OCI_API OCI_GetPrefetchSize(OCI_Statement *stmt)
{
    OCI_CHECK_PTR(stmt, 0);

    return stmt->prefetch_size;
}

/* ------------------------------------------------------------------------ *
 * OCI_SetPrefetchMemory
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_SetPrefetchMemory(OCI_Statement *stmt, int size)
{
    OCI_CHECK_PTR(stmt,  FALSE);
    OCI_CHECK(size < 0, FALSE);

    OCI_CALL(stmt->con, OCIAttrSet(stmt->stmt, OCI_HTYPE_STMT,
                               &stmt->prefetch_mem, 
                               sizeof(stmt->prefetch_mem),
                               OCI_ATTR_PREFETCH_MEMORY,
                               stmt->con->err))

    OCI_CHECK_ERROR(stmt->con, stmt, FALSE);

    stmt->prefetch_mem = size;

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetPrefetchMemory
 * ------------------------------------------------------------------------ */

int OCI_API OCI_GetPrefetchMemory(OCI_Statement *stmt)
{
    OCI_CHECK_PTR(stmt, 0);

    return stmt->prefetch_mem;
}

/* ------------------------------------------------------------------------ *
 * OCI_SetLongMaxSize
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_SetLongMaxSize(OCI_Statement *stmt, int size)
{
    OCI_CHECK_PTR(stmt,  FALSE);
    OCI_CHECK(size <= 0, FALSE);

    stmt->long_size = size;

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetLongMaxSize
 * ------------------------------------------------------------------------ */

int OCI_API OCI_GetLongMaxSize(OCI_Statement *stmt)
{
    OCI_CHECK_PTR(stmt, 0);

    return stmt->long_size;
}

/* ------------------------------------------------------------------------ *
 * OCI_SetLongMode
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_SetLongMode(OCI_Statement *stmt, int mode)
{
    OCI_CHECK_PTR(stmt, FALSE);

    stmt->long_mode = mode;

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetLongMode
 * ------------------------------------------------------------------------ */

int OCI_API OCI_GetLongMode(OCI_Statement *stmt)
{
    OCI_CHECK_PTR(stmt, 0);

    return stmt->long_mode;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetSql
 * ------------------------------------------------------------------------ */

const mtext * OCI_API OCI_GetSql(OCI_Statement *stmt)
{
    OCI_CHECK_PTR(stmt, NULL);

    return stmt->sql;
}

/* ------------------------------------------------------------------------ *
 * OCI_SetFormatDate
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_SetFormatDate(OCI_Statement *stmt, const mtext *format)
{
    OCI_CHECK_PTR(stmt, FALSE);

    OCI_FREE(stmt->fmt_date);

    stmt->fmt_date = mtsdup(format ? format : OCI_STRING_FORMAT_DATE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetFormatDate
 * ------------------------------------------------------------------------ */

const mtext * OCI_API OCI_GetFormatDate(OCI_Statement *stmt)
{
    OCI_CHECK_PTR(stmt, NULL);

    if(!stmt->fmt_date) OCI_SetFormatDate(stmt, NULL);

    return (stmt->fmt_date);
}

/* ------------------------------------------------------------------------ *
 * OCI_GetAffecteddRows
 * ------------------------------------------------------------------------ */

int OCI_API OCI_GetAffectedRows(OCI_Statement *stmt)
{
    ub4 count = 0;

    OCI_CHECK_PTR(stmt, 0);

    OCI_CALL(stmt->con, OCIAttrGet(stmt->stmt, OCI_HTYPE_STMT, &count,
                                   NULL, OCI_ATTR_ROW_COUNT,
                                   stmt->con->err))

    OCI_CHECK_ERROR(stmt->con, stmt, 0);

    return count;
}

/* ------------------------------------------------------------------------ *
 *                         OCI_Resultset functions
 * ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ *
 * OCI_ResultsetCreate
 * ------------------------------------------------------------------------ */

OCI_Resultset * OCI_ResultsetCreate(OCI_Statement *stmt, int size)
{
    OCI_Resultset* rs = NULL;
    boolean res       = TRUE;
    size_t msize      = 0;
    int nb            = 0;
    int i;
  
    OCI_CHECK_PTR(stmt, NULL);
    OCI_CHECK(stmt->status != OCI_STMT_EXECUTED, NULL);
    OCI_CHECK(stmt->type   != OCI_CST_SELECT && stmt->nb_outbinds == 0, NULL);

    /* allocate resulset structure */

    OCI_ALLOC(OCI_Resultset *, rs, msize, sizeof(OCI_Resultset));

    if (rs == NULL)
    {
        OCI_ErrorMemory(stmt->con, stmt, msize);
        return NULL;
    }

    rs->eof        = FALSE;
    rs->fetch_size = size;
    rs->row_count  = 0;
    rs->row_mem    = 0;
    rs->row_abs    = 0;


    /* is the resultset created from a SQL select statement ? */

    if (stmt->type == OCI_CST_SELECT)
    {
        OCI_CALL(stmt->con, OCIAttrGet(stmt->stmt, OCI_HTYPE_STMT,
                                       (ub4 *) &nb, NULL,
                                       OCI_ATTR_PARAM_COUNT, stmt->con->err))
    }
    else
    {
        nb = stmt->nb_outbinds;
    }

    /* allocate columns array */

    OCI_ALLOC(OCI_Define *, rs->defs, msize, nb * sizeof(OCI_Define));

    if (rs->defs == NULL)
    {
        OCI_ErrorMemory(stmt->con, stmt, msize);
        OCI_FREE(rs);
        return NULL;
    }

    rs->stmt = stmt;

    if (stmt->type == OCI_CST_SELECT)
    {
        /* describe select list */

        for (i=0; i < nb; i++)
        {
            OCI_Define *def  = &rs->defs[i];

            def->buf.count   = size;
            def->buf.sizelen = sizeof(ub2);
        
            def->rs = rs;

            rs->nb_defs++;

            res = OCI_ColumnDescribe(&def->col, rs->stmt->con,
                                     rs->stmt,  rs->stmt->stmt,
                                     rs->nb_defs, TRUE);
            
            if (res == TRUE)
            {
                 /* map column : failure means unsupported type */

                if (OCI_ColumnMap(&def->col, rs->stmt) == FALSE)
                {
                    OCI_ResultsetFree(rs);
                    return NULL;
                }
            }

            res = res && OCI_DefineAlloc(def) &&  OCI_DefineDef(def);
            
            if (res == FALSE) break;
        }
    }
    else
    {
        /* get info from input binds */

        for (i=0; i < stmt->nb_binds; i++)
        {
            OCI_Bind   *bnd  = stmt->binds[i];
            OCI_Define *def  = &rs->defs[bnd->dynpos];

            def->buf.count   = size;
            def->buf.sizelen = sizeof(ub4);

            def->rs = rs;

            if (bnd->mode == OCI_BIND_OUTPUT)
            {
                rs->nb_defs++;

                /* columns info */

                def->col.ocode   = bnd->code;
                def->col.name    = mtsdup(bnd->name);
                def->col.size    = (ub2) bnd->size;
                def->col.type    = bnd->type;
                def->col.subtype = bnd->subtype;

                /* check national attribute for nclobs */

                if (bnd->type == OCI_CDT_LOB && bnd->subtype == OCI_NCLOB)
                    def->col.csfrm = SQLCS_NCHAR;

                /* ajdust colum size from bind atrributes */

                if (def->col.type == OCI_CDT_TEXT)
                    def->col.size = (ub2) (def->col.size / (sizeof(dtext)) - 1);

                /* for integer types, set the bufsize here in order to 
                   retreive later the integer type (short, integer, big_int)
                   depending on the integer size */
                
                if (def->col.type == OCI_CDT_INTEGER)
                    def->col.bufsize = def->col.size;

                /* preset bufsize here to let OCI_ColumnMap() know we don't 
                   want the column to mapped to SQLT_ODT */

                if (def->col.ocode == SQLT_DAT)
                    def->col.bufsize = def->col.size;

                /* map column : failure means unsupported type */

                if (OCI_ColumnMap(&def->col, rs->stmt) == FALSE)
                {
                    OCI_ResultsetFree(rs);
                    return NULL;
                }

                /* allocate storage */
                
                res = OCI_DefineAlloc(def);
            
                if (res == FALSE) break;
           }
        }
    }

    /* handle errors */

    if (res == FALSE)
    {
        OCI_ErrorAPI(stmt->con->err, stmt->con, stmt);
        OCI_ResultsetFree(rs);
        rs = NULL;
    }

    return rs;
}

#ifdef OCI_CHECK_DATASTRINGS 

/* ------------------------------------------------------------------------ *
 * OCI_ResultsetExpandStrings
 * ------------------------------------------------------------------------ */

boolean OCI_ResultsetExpandStrings(OCI_Resultset *rs)
{
    int i, j;

    OCI_CHECK_PTR(rs, FALSE)

    for (i = 0; i < rs->nb_defs; i++)
    {
        OCI_Define *def = &rs->defs[i];

        if (def->col.type == OCI_CDT_TEXT)
        {
            for (j = 0; j < def->buf.count; j++)
            {
                OCI_ConvertString(((ub1*)def->buf.data) + (def->col.bufsize  * j),
                                  def->col.bufsize / sizeof(dtext),
                                  sizeof(odtext), sizeof(dtext));
            }
        }
    }

    return TRUE;
}

#endif

/* ------------------------------------------------------------------------ *
 * OCI_GetResultset
 * ------------------------------------------------------------------------ */

OCI_Resultset * OCI_API OCI_GetResultset(OCI_Statement *stmt)
{
    size_t msize;

    OCI_CHECK_PTR(stmt, NULL);
    OCI_CHECK(stmt->status != OCI_STMT_EXECUTED, NULL);
    OCI_CHECK(stmt->type   != OCI_CST_SELECT && stmt->nb_outbinds == 0, NULL);

    /* allocate resultset for select statements */

    if (stmt->type == OCI_CST_SELECT)
    {
        /* allocate memory for one resultset handle */

        OCI_ALLOC(OCI_Resultset **, stmt->rsts, msize, sizeof(OCI_Resultset *));

        if (stmt->rsts == NULL)
        {
            OCI_ErrorMemory(stmt->con, stmt, msize);
            return NULL;
        }
 
        if (stmt->rsts[0] == NULL)
        {
            stmt->nb_rs   = 1;
            stmt->cur_rs  = 0;
            
            /* create resultset object */

            stmt->rsts[0] = OCI_ResultsetCreate(stmt, stmt->fetch_size);
        }
    }

    /* extra checks because allocation migth have failed within
       the callbacks for returning DML statements)  */

    OCI_CHECK_PTR(stmt->rsts, NULL);

    /* everything's ok */

    stmt->cur_rs = 0;

    return stmt->rsts[0];
}

/* ------------------------------------------------------------------------ v*
 * OCI_GetNextResultset
 * ------------------------------------------------------------------------ */

OCI_Resultset * OCI_API OCI_GetNextResultset(OCI_Statement *stmt)
{
    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK(stmt->cur_rs > (stmt->nb_rs -1), NULL);

    return stmt->rsts[++stmt->cur_rs];
}

/* ------------------------------------------------------------------------ *
 * OCI_ResultsetFree
 * ------------------------------------------------------------------------ */

boolean OCI_ResultsetFree(OCI_Resultset *rs)
{
    int i, j;

    OCI_CHECK_PTR(rs, FALSE);

    for (i = 0; i < rs->nb_defs; i++)
    {
        OCI_Define *def = &(rs->defs[i]);

        /* free buffer objects */

        if (def->obj != NULL)
        {
            /* handy cast to set object state flag */

           ((OCI_Datatype *) def->obj)->hstate = OCI_OBJECT_FETCHED_DIRTY;

           switch (def->col.type)
            {
                case OCI_CDT_DATETIME:

                    OCI_DateFree((OCI_Date *) def->obj);
                    break;

                case OCI_CDT_LOB:

                    OCI_LobFree((OCI_Lob *) def->obj);
                    break;

               case OCI_CDT_FILE:

                    OCI_FileFree((OCI_File *) def->obj);
                    break;

               case OCI_CDT_CURSOR:

                    OCI_StatementClose((OCI_Statement *) def->obj);
                    OCI_FREE(def->obj);
                    break;

              case OCI_CDT_OBJECT:

                    OCI_ObjectFree((OCI_Object *) def->obj);
                    break;

               case OCI_CDT_TIMESTAMP:

                    OCI_TimestampFree((OCI_Timestamp *) def->obj);
                    break;

               case OCI_CDT_INTERVAL:

                    OCI_IntervalFree((OCI_Interval *) def->obj);
                    break;
            }

            def->obj = NULL;
        }

        /* free OCI handles */

        if (def->col.dtype != 0)
        {
            for(j=0; j < def->buf.count; j++)
            {
                if (def->col.type == OCI_CDT_CURSOR)

                    OCI_DIRECT(rs->stmt->con,
                               OCIHandleFree(def->buf.data[j],
                               def->col.dtype))
                else

                    OCI_DIRECT(rs->stmt->con,
                               OCIDescriptorFree(def->buf.data[j],
                               def->col.dtype))
            }
        }

        /* free OCI long buffers */
               
        if (def->col.type == OCI_CDT_LONG && def->buf.data != NULL)
        {
            for(j=0; j < def->buf.count; j++)
            {         
                if (def->buf.data[j] != NULL)
                {
                   ((OCI_Datatype *) def->buf.data[j])->hstate = OCI_OBJECT_FETCHED_DIRTY;

                    OCI_LongFree((OCI_Long *) def->buf.data[j]);
                }
            }
        }

        /* free column pointers */

        OCI_FREE(def->col.name);

        /* free buffer pointers */

        OCI_FREE(def->buf.data);
        OCI_FREE(def->buf.inds);
        OCI_FREE(def->buf.lens);
        OCI_FREE(def->buf.temp);
   }

    /* free column map */

    OCI_HashFree(rs->map);

    /* free defines (column array) */

    OCI_FREE(rs->defs);

    OCI_FREE(rs);

    return TRUE;
}


/* ------------------------------------------------------------------------ *
 * OCI_FetchData
 * ------------------------------------------------------------------------ */

boolean OCI_FetchData(OCI_Resultset *rs)
{
    OCI_CHECK_PTR(rs, FALSE);
    OCI_CHECK(rs->eof == TRUE, FALSE);
    OCI_CHECK(rs->stmt->con->status == OCI_NO_DATA, FALSE);

    /* internal fetch */

    OCI_CALL(rs->stmt->con, OCIStmtFetch(rs->stmt->stmt,
                                         rs->stmt->con->err,
                                         rs->fetch_size,
                                         OCI_FETCH_NEXT, OCI_DEFAULT))
    
    /* do we need to do a piecewise fetch */

    if (rs->stmt->piecewise && rs->stmt->con->status == OCI_NEED_DATA)
    {
        size_t msize;
        ub4 type, iter, dx;
        ub1 in_out, piece;
        void *handle;
        int i, j;

        /* reset long objects */

        for (i = 0; i < rs->nb_defs; i++)
        {
            OCI_Define *def = &rs->defs[i];
             
            if (def->col.type == OCI_CDT_LONG)
            {
                for (j = 0; j < def->buf.count; j++)
                {
                    OCI_LongInit(rs->stmt, (OCI_Long **) &def->buf.data[j],
                                 def, def->col.subtype);
                }
            }
        }

        /* dynamic fetch */

        while (rs->stmt->con->status == OCI_NEED_DATA) 
        {      
            piece  = OCI_NEXT_PIECE;
            iter   = 0;
            handle = NULL;

            /* get piece information */

            rs->stmt->con->status = OCIStmtGetPieceInfo(rs->stmt->stmt,
                                                        rs->stmt->con->err,
                                                        &handle, &type,
                                                        &in_out, &iter,
                                                        &dx, &piece);

            /* search for the given column */

            for (i = 0; i < rs->nb_defs; i++)
            {
                OCI_Define *def = &rs->defs[i];
                 
                if (def->col.type == OCI_CDT_LONG && def->buf.handle == handle)
                {
                    /* get the long object for the given internal row */

                    OCI_Long *lg = (OCI_Long *) def->buf.data[iter];

                    /* setup up piece size */

                    ub4 bufsize = rs->stmt->long_size;

                    if (lg->type == OCI_CLONG)
                        bufsize += sizeof(dtext);

                    /* check buffer */

                    if (lg->buffer == NULL)
                    {
                        OCI_ALLOC(ub1 *, lg->buffer, msize, bufsize);
                    }
                    else 
                    {
                        OCI_REALLOC(ub1 *, lg->buffer, msize, 
                                    lg->size + bufsize);

                        if (lg->buffer == NULL)
                        {
                            OCI_ErrorMemory(rs->stmt->con, rs->stmt, msize);
                            return FALSE;
                        }
                    }
                
                    /* update piece infos */

                    lg->piecesize = bufsize;

                    OCI_CALL(lg->stmt->con, 
                             OCIStmtSetPieceInfo(handle,
                                                 OCI_HTYPE_DEFINE,
                                                 lg->stmt->con->err,
                                                 lg->buffer + lg->size,
                                                 &lg->piecesize,
                                                 (ub1) piece,
                                                 NULL, NULL))
                    
                     break;
                }
            }

            /* fetch data */

            OCI_CALL(rs->stmt->con, OCIStmtFetch (rs->stmt->stmt, 
                                                  rs->stmt->con->err,
                                                  rs->fetch_size,
                                                  OCI_FETCH_NEXT, OCI_DEFAULT))

            /* search for the given column */

            for (i = 0; i < rs->nb_defs; i++)
            {
                OCI_Define *def = &rs->defs[i];

                if (def->col.type == OCI_CDT_LONG && def->buf.handle == handle)
                {
                    /* get the long object for the given internal row */
              
                    OCI_Long *lg = (OCI_Long *) def->buf.data[iter];
                    
                    lg->size += lg->piecesize;

                    break;
                }
            }
        }

        /* for LONG columns, set the zero terminal string */

        for (i = 0; i < rs->nb_defs; i++)
        {
            OCI_Define *def = &rs->defs[i];

            if (def->col.type == OCI_CDT_LONG && def->col.subtype == OCI_CLONG)
            {
                for (j = 0; j < def->buf.count; j++)
                {
                    OCI_Long *lg = (OCI_Long *) def->buf.data[j];
                    
                    if (lg->buffer != NULL)
                        lg->buffer[lg->size] = 0;                    
                }
            }
        }
    }

    /* check string buffer for unicode builds that need buffer expansion */

#ifdef OCI_CHECK_DATASTRINGS 
 
    OCI_ResultsetExpandStrings(rs);

#endif

    /* update internal fecth status and variables */

    if ((rs->stmt->con->status == OCI_SUCCESS)    ||
        (rs->stmt->con->status == OCI_NO_DATA)    ||
        (rs->stmt->con->status == OCI_SUCCESS_WITH_INFO))
    {

        if ((rs->stmt->con->status == OCI_NO_DATA) && (rs->fetch_size  == 1))
        {
            rs->eof = TRUE;
        }
        else
        {
            ub4 old_count = rs->row_count;

            rs->stmt->con->status = OCI_SUCCESS;

            /* this might be in the future replaced by a using
               the attribute OCI_ATTR_ROWS_FETCHED instead.

               But for now, Oracle 8i does not support this attribute, so let's
               use our alternative method */

            rs->row_count = OCI_GetAffectedRows(rs->stmt);

            if ((rs->row_count - old_count) != (ub4) rs->fetch_size)
            {
                rs->eof = TRUE;

                if (rs->row_count == old_count)
                    return FALSE;
            }
        }

        rs->stmt->con->status = OCI_SUCCESS;

        return TRUE;
    }

    OCI_CHECK_ERROR(rs->stmt->con, rs->stmt, FALSE);

    return FALSE;
}

/* ------------------------------------------------------------------------ *
 * OCI_FetchNext
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_FetchNext(OCI_Resultset *rs)
{
    int res = TRUE;

    OCI_CHECK_PTR(rs, FALSE);
  
    if (rs->stmt->con->status == OCI_NO_DATA)
    {
        rs->stmt->con->status = OCI_SUCCESS;
        return FALSE;
    }

    /* update current absolute row */
    rs->row_abs++;

     /* get internal current row in the internal fecth array */
    rs->row_mem = (ub4) ((rs->row_abs-1) % rs->fetch_size);

    if (rs->stmt->nb_outbinds > 0)
    {
        /* for resulset created from output binds (DML with returning clause),
           there is no real fetch, just a check for resultset EOF flag to
           find out is buffer expansion has already being made on platforms
           that need it */
    
        if (rs->eof == FALSE)
        {
            /* check string buffer for unicode build */
        
        #ifdef OCI_CHECK_DATASTRINGS 
            
            OCI_ResultsetExpandStrings(rs);
            
        #endif

            rs->eof = TRUE;
        }

        return (rs->row_abs <= rs->row_count);
    }
    
    /* fecth more rows if needed */

    if (rs->row_abs > rs->row_count)
        res = OCI_FetchData(rs);
 
    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetRowCount
 * ------------------------------------------------------------------------ */

int OCI_API OCI_GetRowCount(OCI_Resultset *rs)
{
    OCI_CHECK_PTR(rs, 0);

    return rs->row_count;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetColumnCount
 * ------------------------------------------------------------------------ */

int OCI_API OCI_GetColumnCount(OCI_Resultset *rs)
{
    OCI_CHECK_PTR(rs, 0);

    return rs->nb_defs;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetColumn
 * ------------------------------------------------------------------------ */

OCI_Column * OCI_API OCI_GetColumn(OCI_Resultset *rs, int index)
{
    OCI_Define *def = OCI_GetDefine(rs, index);

    OCI_CHECK_PTR(def, NULL);

    return &def->col;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetColumn2
 * ------------------------------------------------------------------------ */

OCI_Column * OCI_API OCI_GetColumn2(OCI_Resultset *rs, const mtext *name)
{
    OCI_Define *def = OCI_GetDefine(rs, OCI_GetDefineIndex(rs, name));

    OCI_CHECK_PTR(def, NULL);

    return &def->col;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetColumnIndex
 * ------------------------------------------------------------------------ */

int OCI_API OCI_GetColumnIndex(OCI_Resultset *rs, const mtext *name)
{
   return OCI_GetDefineIndex(rs, name);
}

/* ------------------------------------------------------------------------ *
 * OCI_GetColumnName
 * ------------------------------------------------------------------------ */

const mtext * OCI_API OCI_GetColumnName(OCI_Column *col)
{
    OCI_CHECK_PTR(col, NULL);

    return col->name;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetColumnType
 * ------------------------------------------------------------------------ */

int OCI_API OCI_GetColumnType(OCI_Column *col)
{
    OCI_CHECK_PTR(col, OCI_UNKNOWN);

    return col->type;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetColumnCharsetForm
 * ------------------------------------------------------------------------ */

int OCI_API OCI_GetColumnCharsetForm(OCI_Column *col)
{
    OCI_CHECK_PTR(col, OCI_CSF_NONE);

    if (col->csfrm == SQLCS_NCHAR)
        return OCI_CSF_NATIONAL;
    else if (col->csfrm == SQLCS_IMPLICIT)
        return OCI_CSF_CHARSET;
    else
        return OCI_CSF_NONE;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetColumnSize
 * ------------------------------------------------------------------------ */

int OCI_API OCI_GetColumnSize(OCI_Column *col)
{
    OCI_CHECK_PTR(col, 0);

    /* Oracle 9i introduced CHAR attribute on string columns to indicate the 
       size of the column is not in bytes (default) but in chars
       OCI_ColumnDescribe() already dealed with the Oracle compatibily
       version, so if col->charsize is zero it means :
       - the column is not a string column 
       - the size is not in char
       - client does not support the OCI_ATTR_CHAR_SIZE attribute */
       
    if (col->charused == TRUE && col->charsize > 0)
        return col->charsize;
    else
        return col->size;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetColumnScale
 * ------------------------------------------------------------------------ */

int OCI_API OCI_GetColumnScale(OCI_Column *col)
{
    OCI_CHECK_PTR(col, 0);

    return col->scale;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetColumnPrecision
 * ------------------------------------------------------------------------ */

int OCI_API OCI_GetColumnPrecision(OCI_Column *col)
{
    OCI_CHECK_PTR(col, 0);

    if (col->type == OCI_CDT_INTEGER  || col->type == OCI_CDT_DOUBLE)
        return col->prec;
    else
        return 0;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetColumnFractionnalPrecision
 * ------------------------------------------------------------------------ */

int OCI_API OCI_GetColumnFractionnalPrecision(OCI_Column *col)
{
    OCI_CHECK_PTR(col, 0);

    if (col->type == OCI_CDT_TIMESTAMP)
        return col->prec;
    else if (col->type == OCI_CDT_INTERVAL)
        return col->prec2;
    else
        return 0;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetColumnLeadingPrecision
 * ------------------------------------------------------------------------ */

int OCI_API OCI_GetColumnLeadingPrecision(OCI_Column *col)
{
    OCI_CHECK_PTR(col, 0);

    if (col->type == OCI_CDT_INTERVAL)
        return col->prec;
    else
        return 0;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetColumnPrecision
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_GetColumnNullable(OCI_Column *col)
{
    OCI_CHECK_PTR(col, FALSE);

    return (col->null == TRUE);
}

/* ------------------------------------------------------------------------ *
 * OCI_GetColumnCharUsed
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_GetColumnCharUsed(OCI_Column *col)
{
    OCI_CHECK_PTR(col, FALSE);

    return (boolean) col->charused;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetColumnSQLType
 * ------------------------------------------------------------------------ */

const mtext * OCI_API OCI_GetColumnSQLType(OCI_Column *col)
{
    OCI_CHECK_PTR(col, NULL);

    /* VARCHAR type will not be returned because Oracle does not make any 
       difference with VARCHAR2. If a column is created with VARCHAR, it is
       internally created as VARCHAR2
    */       

    switch(col->ocode)
    {
        case SQLT_AFC:

            if (col->csfrm == SQLCS_NCHAR)
                return MT("NCHAR");
            else
                return MT("CHAR");

        case SQLT_AVC:
        case SQLT_STR:
        case SQLT_CHR:

            if (col->csfrm == SQLCS_NCHAR)
                return MT("NVARCHAR2");
            else
                return MT("VARCHAR2");

        case SQLT_NUM:

            if (col->scale == -127 && col->prec > 0)
                 return MT("FLOAT");
             else
                 return MT("NUMBER");

        case SQLT_INT:

             return MT("INTEGER");

        case SQLT_FLT:

             return MT("FLOAT");

#if OCI_VERSION_COMPILE >= OCI_10

        case SQLT_BFLOAT:
        case SQLT_IBFLOAT:

             return MT("BINARY FLOAT");

        case SQLT_BDOUBLE:
        case SQLT_IBDOUBLE:

             return MT("BINARY DOUBLE");

 #endif

        case SQLT_LNG:

             return MT("LONG");

        case SQLT_DAT:
        case SQLT_ODT:
        case SQLT_DATE:

            return MT("DATE");

        case SQLT_RDD:
        case SQLT_RID:

            return MT("ROWID");

        case SQLT_BIN:

            return MT("RAW");

        case SQLT_LBI:

            return MT("LONG RAW");

        case SQLT_RSET:

            return MT("RESULTSET");

        case SQLT_CUR:

            return MT("CURSOR");

        case SQLT_CLOB:

            if (col->subtype == OCI_NCLOB)
                return MT("NCLOB");
            else
                return MT("CLOB");

        case SQLT_BLOB:

            return MT("BLOB");

        case SQLT_BFILE:

            return MT("BINARY FILE LOB");

        case SQLT_CFILE:

            return MT("CFILE");

#if OCI_VERSION_COMPILE >= OCI_9

        case SQLT_TIMESTAMP:

            return MT("TIMESTAMP");

        case SQLT_TIMESTAMP_TZ:

            return MT("TIMESTAMP WITH TIME ZONE");

        case SQLT_TIMESTAMP_LTZ:

            return MT("TIMESTAMP WITH LOCAL TIME ZONE");

        case SQLT_INTERVAL_YM:

            return MT("INTERVAL YEAR TO MONTH");

        case SQLT_INTERVAL_DS:

            return MT("INTERVAL DAY TO SECOND");

#endif

        case SQLT_REF:

            return MT("REF");

#if OCI_VERSION_COMPILE >= OCI_9

        case SQLT_PNTY:
#endif

        case SQLT_NTY:

            if (col->nty != NULL)
                return col->nty->name;
            else
                return MT("NAMED TYPE");

       default:

            /* for all other types not supported */
            return MT("?");
    }
}


/* ------------------------------------------------------------------------ *
 * OCI_GetColumnFullSQLType
 * ------------------------------------------------------------------------ */

int OCI_API OCI_GetColumnFullSQLType(OCI_Column *col, mtext *buffer, int len)
{
    OCI_CHECK_PTR(col, 0);

    /* ISO C functions are supposed to be "standard", but we still see specific
       implementations that make some usage not portable and worse not compatible.
       MS Windows is implementing string conversion characters (%s/%ls) of the 
       printf/wprintf family diffently from unixes !
    */

    /* This function returns the same strings as Sql*Plus DESC command */

    switch(col->ocode)
    {
        case SQLT_AFC:

#if defined(OCI_METADATA_UNICODE) && !defined(_WINDOWS)
            len = mtsprintf(buffer, len, MT("%lsCHAR(%i%ls)"), 
#else
            len = mtsprintf(buffer, len, MT("%sCHAR(%i%s)"), 
#endif
                            col->csfrm    == SQLCS_NCHAR ? MT("N") : MT(""),
                            col->charused == TRUE ? col->charsize : col->size,
                            col->charused == TRUE &&
                            col->csfrm    != SQLCS_NCHAR ? MT(" CHAR") : MT(""));
            break;
 
        case SQLT_AVC:
        case SQLT_STR:
        case SQLT_CHR:
 
#if defined(OCI_METADATA_UNICODE) && !defined(_WINDOWS)
            len = mtsprintf(buffer, len, MT("%lsVARCHAR(%i%ls)"), 
#else
            len = mtsprintf(buffer, len, MT("%sVARCHAR(%i%s)"), 
#endif
                            col->csfrm    == SQLCS_NCHAR ? MT("N") : MT(""),
                            col->charused == TRUE ? col->charsize : col->size,
                            col->charused == TRUE &&
                            col->csfrm    != SQLCS_NCHAR ? MT(" CHAR") : MT(""));
            break;

        case SQLT_NUM:
 
            if (col->scale == -127 && col->prec > 0)
                len = mtsprintf(buffer, len,  MT("FLOAT(%i)"), col->prec);
            else if (col->scale > 0 && col->prec > 0)
                len = mtsprintf(buffer, len,  MT("NUMBER(%i,%i)"), 
                                col->prec, col->scale);
            else if (col->prec > 0)
                len = mtsprintf(buffer, len,  MT("NUMBER(%i)"), col->prec);
            else
                len = mtsprintf(buffer, len,  MT("NUMBER"));
        
            break;
 
        case SQLT_INT:

            len = mtsprintf(buffer, len,  MT("NUMBER"));
            break;

        case SQLT_FLT:

            len = mtsprintf(buffer, len,  MT("FLOAT(%i)"), col->prec);
            break;

#if OCI_VERSION_COMPILE >= OCI_10

        case SQLT_BFLOAT:
        case SQLT_IBFLOAT:

            len = mtsprintf(buffer, len,  MT("BINARY FLOAT"));
            break;

        case SQLT_BDOUBLE:
        case SQLT_IBDOUBLE:

             len = mtsprintf(buffer, len,  MT("BINARY DOUBLE"));
            break;

#endif

        case SQLT_LNG:

            len = mtsprintf(buffer, len, MT("LONG"));
            break;

        case SQLT_DAT:
        case SQLT_ODT:
        case SQLT_DATE:

            len = mtsprintf(buffer, len, MT("DATE"));
            break;

        case SQLT_RDD:
        case SQLT_RID:

            len = mtsprintf(buffer, len,  MT("ROWID"));
            break;

        case SQLT_BIN:
            len = mtsprintf(buffer, len, MT("RAW(%i)"), col->size);
            break;

        case SQLT_LBI:

            len = mtsprintf(buffer, len, MT("LONG RAW(%i)"), col->size);
            break;

        case SQLT_RSET:

            len = mtsprintf(buffer, len,  MT("RESULTSET"));
            break;

        case SQLT_CUR:

            len = mtsprintf(buffer, len,  MT("CURSOR"));
            break;

        case SQLT_CLOB:

            if (col->subtype == OCI_NCLOB)
                len = mtsprintf(buffer, len,  MT("NCLOB"));
            else
                len = mtsprintf(buffer, len,  MT("CLOB"));
            break;

        case SQLT_BLOB:

            len = mtsprintf(buffer, len,  MT("BLOB"));
            break;

        case SQLT_BFILE:

            len = mtsprintf(buffer, len,  MT("BINARY FILE LOB"));
            break;
 
        case SQLT_CFILE:
 
            len = mtsprintf(buffer, len,  MT("CFILE"));
            break;
 
#if OCI_VERSION_COMPILE >= OCI_9

        case SQLT_TIMESTAMP:

            len = mtsprintf(buffer, len,  MT("TIMESTAMP(%i)"), col->prec);
            break;
 
        case SQLT_TIMESTAMP_TZ:
 
            len = mtsprintf(buffer, len,  MT("TIMESTAMP(%i) WITH TIME ZONE"),
                            col->prec);
            break;
 
        case SQLT_TIMESTAMP_LTZ:
 
            len = mtsprintf(buffer, len,  MT("TIMESTAMP(%i) WITH LOCAL TIME ZONE"),
                            col->prec);
            break;
 
        case SQLT_INTERVAL_YM:
 
           len = mtsprintf(buffer, len,  MT("INTERVAL(%i) YEAR TO MONTH(%i)"),
                            col->prec, col->prec2);
            break;
 
       case SQLT_INTERVAL_DS:
 
            len = mtsprintf(buffer, len,  MT("INTERVAL(%i) DAY TO SECOND(%i)"),
                            col->prec, col->prec2);
            break;

#endif
       
       case SQLT_REF:

            len = mtsprintf(buffer, len,  MT("REF"));
            break;

#if OCI_VERSION_COMPILE >= OCI_9

        case SQLT_PNTY:
#endif

        case SQLT_NTY:
 
            if (col->nty != NULL)
                len = mtsprintf(buffer, len, col->nty->name);
            else
                len = mtsprintf(buffer, len, MT("NAMED TYPE"));
            break;

        default:

            mtscpy(buffer, MT("?"));
    }

    return len;
}
    
/* ------------------------------------------------------------------------ *
 * OCI_GetShort
 * ------------------------------------------------------------------------ */

short OCI_API OCI_GetShort(OCI_Resultset *rs, int index)
{
    OCI_Define *def = OCI_GetDefine(rs, index);
    short value = 0;

    OCI_CHECK_PTR(def, 0);

    if (OCI_NOT_NULL(def) == TRUE) 
    {
        void *data = OCI_DefineGetData(def);

        switch (def->col.type)
        {
            case OCI_CDT_INTEGER:

                value = *(short *) OCI_IntegerGet(def->rs->stmt->con,
                                                  data, 
                                                  def->col.subtype,
                                                  def->col.bufsize,
                                                  &value,
                                                  OCI_NUM_SHORT,
                                                  sizeof(value));
                break;

            case OCI_CDT_DOUBLE:

                value = (short) *((double *) (data));

                break;
        }
    }

    return value;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetShort2
 * ------------------------------------------------------------------------ */

short OCI_API OCI_GetShort2(OCI_Resultset *rs, const mtext *name)
{
    return OCI_GetShort(rs, OCI_GetDefineIndex(rs, name));
}
    
/* ------------------------------------------------------------------------ *
 * OCI_GetUnsignedShort
 * ------------------------------------------------------------------------ */

unsigned short OCI_API OCI_GetUnsignedShort(OCI_Resultset *rs, int index)
{
    OCI_Define *def = OCI_GetDefine(rs, index);
    unsigned short value = 0;

    OCI_CHECK_PTR(def, 0);

    if (OCI_NOT_NULL(def) == TRUE) 
    {
        void *data = OCI_DefineGetData(def);

        switch (def->col.type)
        {
            case OCI_CDT_INTEGER:

                value = *(unsigned short *) OCI_IntegerGet(def->rs->stmt->con,
                                                           data, 
                                                           def->col.subtype,
                                                           def->col.bufsize,
                                                           &value,
                                                           OCI_NUM_USHORT,
                                                           sizeof(value));
                break;

            case OCI_CDT_DOUBLE:

                value = (unsigned short) *((double*) (data));

                break;
        }
    }

    return value;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetUnsignedShort2
 * ------------------------------------------------------------------------ */

unsigned short OCI_API OCI_GetUnsignedShort2(OCI_Resultset *rs, const mtext *name)
{
    return OCI_GetUnsignedShort(rs, OCI_GetDefineIndex(rs, name));
}

/* ------------------------------------------------------------------------ *
 * OCI_GetInt
 * ------------------------------------------------------------------------ */

int OCI_API OCI_GetInt(OCI_Resultset *rs, int index)
{
    OCI_Define *def = OCI_GetDefine(rs, index);
    int value = 0;

    OCI_CHECK_PTR(def, 0);

    if (OCI_NOT_NULL(def) == TRUE) 
    {
        void *data = OCI_DefineGetData(def);

        switch (def->col.type)
        {
            case OCI_CDT_INTEGER:

                value = *(int *) OCI_IntegerGet(def->rs->stmt->con,
                                                data, 
                                                def->col.subtype,
                                                def->col.bufsize,
                                                &value,
                                                OCI_NUM_INT,
                                                sizeof(value));
                break;

            case OCI_CDT_DOUBLE:

                value = (int) *((double*) (data));

                break;
        }
    }

    return value;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetInt2
 * ------------------------------------------------------------------------ */

int OCI_API OCI_GetInt2(OCI_Resultset *rs, const mtext *name)
{
    return OCI_GetInt(rs, OCI_GetDefineIndex(rs, name));
}

/* ------------------------------------------------------------------------ *
 * OCI_GetUnsignedInt
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_GetUnsignedInt(OCI_Resultset *rs, int index)
{
    OCI_Define *def = OCI_GetDefine(rs, index);
    unsigned int value = 0;

    OCI_CHECK_PTR(def, 0);

    if (OCI_NOT_NULL(def) == TRUE) 
    {
        void *data = OCI_DefineGetData(def);

        switch (def->col.type)
        {
            case OCI_CDT_INTEGER:

                value = *(unsigned int *) OCI_IntegerGet(def->rs->stmt->con,
                                                         data, 
                                                         def->col.subtype,
                                                         def->col.bufsize,
                                                         &value,
                                                         OCI_NUM_UINT,
                                                         sizeof(value));
                break;

            case OCI_CDT_DOUBLE:

                value = (unsigned int) *((double*) (data));

                break;
        }
    }

    return value;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetUnsignedInt2
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_GetUnsignedInt2(OCI_Resultset *rs, const mtext *name)
{
    return OCI_GetUnsignedInt(rs, OCI_GetDefineIndex(rs, name));
}

/* ------------------------------------------------------------------------ *
 * OCI_GetBigInt
 * ------------------------------------------------------------------------ */

big_int OCI_API OCI_GetBigInt(OCI_Resultset *rs, int index)
{
    OCI_Define *def = OCI_GetDefine(rs, index);
    big_int value = 0;

    OCI_CHECK_PTR(def, 0);

    if (OCI_NOT_NULL(def) == TRUE) 
    {
        void *data = OCI_DefineGetData(def);

        switch (def->col.type)
        {
            case OCI_CDT_INTEGER:

                value = *(big_int *) OCI_IntegerGet(def->rs->stmt->con,
                                                    data, 
                                                    def->col.subtype,
                                                    def->col.bufsize,
                                                    &value,
                                                    OCI_NUM_BIGINT,
                                                    sizeof(value));
                break;

            case OCI_CDT_DOUBLE:

                value = (big_int) *((double*) (data));

                break;
        }
    }

    return value;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetBigInt2
 * ------------------------------------------------------------------------ */

big_int OCI_API OCI_GetBigInt2(OCI_Resultset *rs, const mtext *name)
{
    return OCI_GetBigInt(rs, OCI_GetDefineIndex(rs, name));
}

/* ------------------------------------------------------------------------ *
 * OCI_GetUnsignedBigInt
 * ------------------------------------------------------------------------ */

big_uint OCI_API OCI_GetUnsignedBigInt(OCI_Resultset *rs, int index)
{
    OCI_Define *def = OCI_GetDefine(rs, index);
    big_uint value = 0;

    OCI_CHECK_PTR(def, 0);

    if (OCI_NOT_NULL(def) == TRUE) 
    {
        void *data = OCI_DefineGetData(def);

        switch (def->col.type)
        {
            case OCI_CDT_INTEGER:

                value = *(big_uint *) OCI_IntegerGet(def->rs->stmt->con,
                                                     data, 
                                                     def->col.subtype,
                                                     def->col.bufsize,
                                                     &value,
                                                     OCI_NUM_BIGUINT,
                                                     sizeof(value));
                break;

            case OCI_CDT_DOUBLE:

                value = (big_uint) *((double*) (data));

                break;
        }
    }

    return value;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetUnsignedInt2
 * ------------------------------------------------------------------------ */

big_uint OCI_API OCI_GetUnsignedBigInt2(OCI_Resultset *rs, const mtext *name)
{
    return OCI_GetUnsignedBigInt(rs, OCI_GetDefineIndex(rs, name));
}

/* ------------------------------------------------------------------------ *
 * OCI_GetString
 * ------------------------------------------------------------------------ */

const dtext * OCI_API OCI_GetString(OCI_Resultset *rs, int index)
{
    OCI_Define *def = OCI_GetDefine(rs, index);

    OCI_CHECK_PTR(def, NULL);

    if (OCI_NOT_NULL(def) == TRUE)
    {
        void *data   = OCI_DefineGetData(def);
        size_t msize = 0;

        if (def->col.type == OCI_CDT_TEXT)
            return (const dtext *) data;

        /* tries an implicit conversion if possible */

        if (def->buf.temp == NULL)
            OCI_ALLOC(mtext *, def->buf.temp, msize, 
                      (OCI_SIZE_BUFFER+1)*sizeof(dtext));

        if (def->buf.temp == NULL)
        {
            OCI_ErrorMemory(rs->stmt->con, rs->stmt, msize);
            return NULL;
        }

        def->buf.temp[0] = 0;

        switch (def->col.type)
        {
            case OCI_CDT_INTEGER:

                if (def->col.icode == SQLT_INT)
                {
                    dtsprintf(def->buf.temp, OCI_SIZE_BUFFER,
                              DT("%d"), OCI_GetInt(rs, index));
                }
                else if (def->col.icode == SQLT_UIN)
                {
                    dtsprintf(def->buf.temp, OCI_SIZE_BUFFER,
                              DT("%u"), OCI_GetUnsignedInt(rs, index));
                }
                else
                {
                    ub4 len = OCI_SIZE_BUFFER;

                    OCI_CALL(rs->stmt->con,
                             OCINumberToText(rs->stmt->con->err, data,
                                            (CONST text *) OCI_STRING_FORMAT_NUM, 
                                            OCI_SIZE_FORMAT_NUM, 
                                            (CONST text *) NULL, 0, &len,
                                            (text*) def->buf.temp))
          
                    OCI_CHECK_ERROR(rs->stmt->con, rs->stmt, NULL);
                }

                break;

            case OCI_CDT_DOUBLE:

                dtsprintf(def->buf.temp, OCI_SIZE_BUFFER,
                          DT("%g"), OCI_GetDouble(rs, index));
                break;

            case OCI_CDT_DATETIME:

#ifndef OCI_CHARSET_MIXED

                 OCI_DateToText(OCI_GetDate(rs, index),
                                OCI_GetFormatDate(rs->stmt),
                                OCI_SIZE_BUFFER, (mtext *) def->buf.temp);
#else

                /* mixed mode... hum conversion needed ! */

                {
                    mtext temp[OCI_SIZE_BUFFER+1];

                    temp[0] = 0;

                    OCI_DateToText(OCI_GetDate(rs, index),
                                   OCI_GetFormatDate(rs->stmt),
                                   OCI_SIZE_BUFFER, temp);

                    mbstowcs(def->buf.temp, temp, strlen(temp));
                }
 #endif
                break;

            case OCI_CDT_TIMESTAMP:

#ifndef OCI_CHARSET_MIXED

                 OCI_TimestampToText(OCI_GetTimestamp(rs, index),
                                     OCI_GetFormatDate(rs->stmt),
                                     OCI_SIZE_BUFFER,
                                     (mtext *) def->buf.temp,
                                     0);
#else

                /* mixed mode... hum conversion needed ! */

                {
                    mtext temp[OCI_SIZE_BUFFER+1];

                    temp[0] = 0;

                    OCI_TimestampToText(OCI_GetTimestamp(rs, index),
                                        OCI_GetFormatDate(rs->stmt),
                                        OCI_SIZE_BUFFER,
                                        (mtext *) def->buf.temp,
                                        0);

                    mbstowcs(def->buf.temp, temp, strlen(temp));
                }
 #endif
                break;

            case OCI_CDT_INTERVAL:

#ifndef OCI_CHARSET_MIXED

                 OCI_IntervalToText(OCI_GetInterval(rs, index),                            
                                    OCI_STRING_DEFAULT_PREC,
                                    OCI_STRING_DEFAULT_PREC,
                                    OCI_SIZE_BUFFER, 
                                    (mtext *) def->buf.temp);
#else

                /* mixed mode... hum conversion needed ! */

                {
                    mtext temp[OCI_SIZE_BUFFER+1];

                    temp[0] = 0;

                    OCI_IntervalToText(OCI_GetInterval(rs, index),                                      
                                       OCI_STRING_DEFAULT_PREC,
                                       OCI_STRING_DEFAULT_PREC,
                                       OCI_SIZE_BUFFER, (mtext *) temp);
                    
                    mbstowcs(def->buf.temp, temp, strlen(temp));
                }
 #endif
                break;
        }

        return def->buf.temp;
    }

    return NULL;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetString2
 * ------------------------------------------------------------------------ */

const dtext * OCI_API OCI_GetString2(OCI_Resultset *rs, const mtext *name)
{
    return OCI_GetString(rs, OCI_GetDefineIndex(rs, name));
}

/* ------------------------------------------------------------------------ *
 * OCI_GetRaw
 * ------------------------------------------------------------------------ */

int OCI_API OCI_GetRaw(OCI_Resultset *rs, int index, void *buffer, int len)
{
    ub4 count = (ub4) len;

    OCI_Define *def = OCI_GetDefine(rs, index);

    OCI_CHECK_PTR(def, 0);
    OCI_CHECK_PTR(buffer, 0);

    if ((OCI_NOT_NULL(def) == TRUE) && (def->col.type == OCI_CDT_RAW))
    {
        if (count > def->col.size)
            count = def->col.size;

        /* for RAWs, we copy the data in the destination buffer instead of 
           returning internal buffer as we do for strings */

        memcpy(buffer, OCI_DefineGetData(def), count);

        return count;
    }

    return 0;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetRaw2
 * ------------------------------------------------------------------------ */

int OCI_API OCI_GetRaw2(OCI_Resultset *rs, const mtext *name, void *buffer, 
                        int len)
{
    return OCI_GetRaw(rs, OCI_GetDefineIndex(rs, name), buffer, len);
}

/* ------------------------------------------------------------------------ *
 * OCI_GetDouble
 * ------------------------------------------------------------------------ */

double OCI_API OCI_GetDouble(OCI_Resultset *rs, int index)
{
    OCI_Define *def = OCI_GetDefine(rs, index);

    OCI_CHECK_PTR(def, 0.0);

    if (OCI_NOT_NULL(def) == TRUE)
    {
        void *data = OCI_DefineGetData(def);

        /* tries an implicit conversion if possible */

        switch (def->col.type)
        {
            case OCI_CDT_INTEGER:
            {
                double value = 0.0;
                OCINumber num;
        
                if (def->col.subtype != OCI_NUM_NUMBER)
                {
                    uword flag = OCI_NUMBER_SIGNED;

                    if (def->col.subtype & OCI_NUM_UNSIGNED)
                        flag = OCI_NUMBER_UNSIGNED;

                    OCI_NumberSet(rs->stmt->con, &num, data, def->col.bufsize,
                                  flag);

                    data = &num;
                }        

                OCI_CALL(rs->stmt->con, OCINumberToReal(rs->stmt->con->err, 
                                                        data, 
                                                        sizeof(value), 
                                                        &value))

                OCI_CHECK_ERROR(rs->stmt->con, rs->stmt, 0.0);

                return value;
            }

            case OCI_CDT_DOUBLE:

                return *((double*) (data));

            case OCI_CDT_TEXT:

                return dtstod((const dtext *) data, NULL);
        }
    }

    return 0.0;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetDouble2
 * ------------------------------------------------------------------------ */

double OCI_API OCI_GetDouble2(OCI_Resultset *rs, const mtext *name)
{
    return OCI_GetDouble(rs, OCI_GetDefineIndex(rs, name));
}

/* ------------------------------------------------------------------------ *
 * OCI_GetDate
 * ------------------------------------------------------------------------ */

OCI_Date * OCI_API OCI_GetDate(OCI_Resultset *rs, int index)
{
    OCI_Define *def = OCI_GetDefine(rs, index);

    OCI_CHECK_PTR(def, NULL);

    if (OCI_NOT_NULL(def) == TRUE)
    {
        /* no implicit conversion */

        if (def->col.type == OCI_CDT_DATETIME)
        {
             return OCI_DateInit(rs->stmt->con,
                                 (OCI_Date **) &def->obj,
                                 (OCIDate *) OCI_DefineGetData(def), FALSE,
                                 (def->col.icode == SQLT_DAT));
        }
    }

    return NULL;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetDate2
 * ------------------------------------------------------------------------ */

OCI_Date * OCI_API OCI_GetDate2(OCI_Resultset *rs, const mtext *name)
{
    return OCI_GetDate(rs, OCI_GetDefineIndex(rs, name));
}

/* ------------------------------------------------------------------------ *
 * OCI_GetTimestamp
 * ------------------------------------------------------------------------ */

OCI_Timestamp * OCI_API OCI_GetTimestamp(OCI_Resultset *rs, int index)
{
    OCI_Define *def = OCI_GetDefine(rs, index);

    OCI_CHECK_PTR(def, NULL);

    if (OCI_NOT_NULL(def) == TRUE)
    {
        /* no implicit conversion */

        if (def->col.type == OCI_CDT_TIMESTAMP)
        {
            return OCI_TimestampInit(rs->stmt->con,
                                     (OCI_Timestamp **) &def->obj,
                                     (OCIDateTime *) OCI_DefineGetData(def),
                                     def->col.subtype);
        }
    }

    return NULL;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetTimestamp2
 * ------------------------------------------------------------------------ */

OCI_Timestamp * OCI_API OCI_GetTimestamp2(OCI_Resultset *rs, const mtext *name)
{
    return OCI_GetTimestamp(rs, OCI_GetDefineIndex(rs, name));
}

/* ------------------------------------------------------------------------ *
 * OCI_GetInterval
 * ------------------------------------------------------------------------ */

OCI_Interval * OCI_API OCI_GetInterval(OCI_Resultset *rs, int index)
{
    OCI_Define *def = OCI_GetDefine(rs, index);

    OCI_CHECK_PTR(def, NULL);

    if (OCI_NOT_NULL(def) == TRUE)
    {
        /* no implicit conversion */

        if (def->col.type == OCI_CDT_INTERVAL)
        {
            return OCI_IntervalInit(rs->stmt->con,
                                    (OCI_Interval **) &def->obj,
                                    (OCIInterval *) OCI_DefineGetData(def),
                                    def->col.subtype);
        }
    }

    return NULL;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetInterval2
 * ------------------------------------------------------------------------ */

OCI_Interval * OCI_API OCI_GetInterval2(OCI_Resultset *rs, const mtext *name)
{
    return OCI_GetInterval(rs, OCI_GetDefineIndex(rs, name));
}

/* ------------------------------------------------------------------------ *
 * OCI_Object
 * ------------------------------------------------------------------------ */

OCI_Object * OCI_API OCI_GetObject(OCI_Resultset *rs, int index)
{
    OCI_Define *def = OCI_GetDefine(rs, index);

    OCI_CHECK_PTR(def, 0);

    if (OCI_NOT_NULL(def) == TRUE)
    {
        /* no implicit conversion */
      
        if (def->col.ocode == SQLT_NTY)
        {
            return OCI_ObjectInit(rs->stmt->con,
                                  (OCI_Object **) &def->obj,
                                  OCI_DefineGetData(def), def->col.nty);
        }
   }

    return NULL;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetObject2
 * ------------------------------------------------------------------------ */

OCI_Object * OCI_API OCI_GetObject2(OCI_Resultset *rs, const mtext *name)
{
    return OCI_GetObject(rs, OCI_GetDefineIndex(rs, name));
}

/* ------------------------------------------------------------------------ *
 * OCI_GetStatement
 * ------------------------------------------------------------------------ */

OCI_Statement * OCI_API OCI_GetStatement(OCI_Resultset *rs, int index )
{
    OCI_Define *def = OCI_GetDefine(rs, index);

    OCI_CHECK_PTR(def, NULL);

    if (OCI_NOT_NULL(def) == TRUE)
    {
        /* no implicit conversion */

        if (def->col.type == OCI_CDT_CURSOR)
        {
            return OCI_StatementInit(rs->stmt->con,
                                     (OCI_Statement **) &def->obj,
                                     (OCIStmt *) OCI_DefineGetData(def), def);
        }
    }

    return NULL;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetStatement2
 * ------------------------------------------------------------------------ */

OCI_Statement * OCI_API OCI_GetStatement2(OCI_Resultset *rs, const mtext *name)
{
    return OCI_GetStatement(rs, OCI_GetDefineIndex(rs, name));
}

/* ------------------------------------------------------------------------ *
 * OCI_GetLob
 * ------------------------------------------------------------------------ */

OCI_Lob * OCI_API OCI_GetLob(OCI_Resultset *rs, int index)
{
    OCI_Define *def = OCI_GetDefine(rs, index);

    OCI_CHECK_PTR(def, NULL);

    if (OCI_NOT_NULL(def) == TRUE)
    {
        /* no implicit conversion */

        if (def->col.type == OCI_CDT_LOB)
        {
            return OCI_LobInit(rs->stmt->con,
                               (OCI_Lob **) &def->obj,
                               (OCILobLocator *) OCI_DefineGetData(def),
                               def->col.subtype);
        }
    }

    return NULL;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetLob2
 * ------------------------------------------------------------------------ */

OCI_Lob * OCI_API OCI_GetLob2(OCI_Resultset *rs, const mtext *name)
{
    return OCI_GetLob(rs, OCI_GetDefineIndex(rs, name));
}

/* ------------------------------------------------------------------------ *
 * OCI_GetFile
 * ------------------------------------------------------------------------ */

OCI_File * OCI_API OCI_GetFile(OCI_Resultset *rs, int index)
{
    OCI_Define *def = OCI_GetDefine(rs, index);

    OCI_CHECK_PTR(def, NULL);

    if (OCI_NOT_NULL(def) == TRUE)
    {
        /* no implicit conversion */

        if (def->col.type == OCI_CDT_FILE)
        {
            return OCI_FileInit(rs->stmt->con,
                                (OCI_File **) &def->obj,
                                (OCILobLocator *) OCI_DefineGetData(def),
                                def->col.subtype);
        }
    }

    return NULL;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetFile2
 * ------------------------------------------------------------------------ */

OCI_File * OCI_API OCI_GetFile2(OCI_Resultset *rs, const mtext *name)
{
    return OCI_GetFile(rs, OCI_GetDefineIndex(rs, name));
}

/* ------------------------------------------------------------------------ *
 * OCI_GetLong
 * ------------------------------------------------------------------------ */

OCI_Long * OCI_API OCI_GetLong(OCI_Resultset *rs, int index)
{
    OCI_Define *def = OCI_GetDefine(rs, index);

    OCI_CHECK_PTR(def, NULL);

    if (OCI_NOT_NULL(def) == TRUE)
    {
        /* no implicit conversion */

        if (def->col.type == OCI_CDT_LONG)
        {
            return (OCI_Long *) OCI_DefineGetData(def);
        }
    }

    return NULL;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetLong2
 * ------------------------------------------------------------------------ */

OCI_Long * OCI_API OCI_GetLong2(OCI_Resultset *rs, const mtext *name)
{
    return OCI_GetLong(rs, OCI_GetDefineIndex(rs, name));
}

/* ------------------------------------------------------------------------ *
 * OCI_IsNull
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_IsNull(OCI_Resultset *rs, int index)
{
    OCI_Define *def = OCI_GetDefine(rs, index);

    OCI_CHECK_PTR(def, TRUE);

    return (OCI_NOT_NULL(def) == FALSE);
}

/* ------------------------------------------------------------------------ *
 *                         OCI Dynamic callback functions
 * ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ *
 * OCI_GetDefine
 * ------------------------------------------------------------------------ */

sb4 OCI_ProcInBind(dvoid *ictxp, OCIBind *bindp, ub4 iter, ub4 index,
                   dvoid **bufpp, ub4 *alenp, ub1 *piecep, dvoid **indp)
{
    OCI_Bind * bnd = (OCI_Bind *) ictxp;
    sb2 *ind = (sb2 *) bnd->buf.inds;
    int i;

    /* those checks may be not necessary but they keep away compilers warning
       away if the warning level is set to maximum !
    */

    OCI_NOT_USED(index);
    OCI_NOT_USED(bindp);

    /* check objects and bounds */

    OCI_CHECK_PTR(bnd, OCI_ERROR);
    OCI_CHECK(iter > (ub4) bnd->buf.count, OCI_ERROR);

    /* indicators must be set to -1 depending on datatype,
       so let's do it for all */

    for (i = 0; i < bnd->buf.count; i++, ind++)
        *ind = -1;

    /* setup bind index because OCI_RegisterXXX() might not have been called
       in the same order than the variables in the returning clause */

    if (iter == 0)
        bnd->dynpos = bnd->stmt->dynidx++;

    /* we do not need to do anything here except setting indicators */

    *bufpp  = (dvoid *) 0;
    *alenp  = 0;
    *indp   = (dvoid *) bnd->buf.inds;
    *piecep = OCI_ONE_PIECE;

    return OCI_CONTINUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetDefine
 * ------------------------------------------------------------------------ */

sb4 OCI_ProcOutBind(dvoid *octxp, OCIBind *bindp, ub4 iter, ub4 index,
                    dvoid **bufpp, ub4 **alenp, ub1 *piecep, dvoid **indp,
                    ub2 **rcodep)
{
    static ub4  rows = 0;
    OCI_Bind * bnd = (OCI_Bind *) octxp;
    OCI_Define *def;
    OCI_Resultset *rs;
    size_t msize;

    /* those checks may be not necessary but they keep away compilers warning
       away if the warning level is set to maximum !
    */

    OCI_NOT_USED(bindp);

    /* check objects and bounds */

    OCI_CHECK_PTR(bnd, OCI_ERROR);
    OCI_CHECK(iter > (ub4) bnd->buf.count, OCI_ERROR);

    /* update statmement status */

    bnd->stmt->status = OCI_STMT_EXECUTED;

    /* create resultset on the first row processed for each iteration */

    if (index == 0)
    {
        bnd->stmt->nb_rs   = bnd->stmt->nb_iters;
        bnd->stmt->cur_rs  = 0;

        /* allocate resultset handles array */

        if (bnd->stmt->rsts == NULL)
        {
            OCI_ALLOC(OCI_Resultset **, bnd->stmt->rsts, msize,
                      sizeof(OCI_Resultset) * bnd->stmt->nb_rs);

            if (bnd->stmt->rsts == NULL)
            {
                OCI_ErrorMemory(bnd->stmt->con, bnd->stmt, msize);
                return OCI_ERROR;
            }
        }
        
        /* create resultset as needed */

        if (bnd->stmt->rsts[iter] == NULL)
        {
            OCI_CALL(bnd->stmt->con, OCIAttrGet(bnd->buf.handle,
                                            OCI_HTYPE_BIND,
                                            &rows,
                                            NULL,
                                            OCI_ATTR_ROWS_RETURNED,
                                            bnd->stmt->con->err))

            bnd->stmt->rsts[iter] = OCI_ResultsetCreate(bnd->stmt, rows);
        }
    }

    /* check for errors to stop Oracle to call this function if an error occurs */

    OCI_CHECK_ERROR(bnd->stmt->con, bnd->stmt, OCI_ERROR);

    /* should not happen... but let's think safe */

    OCI_CHECK_PTR(bnd->stmt->rsts, OCI_ERROR);
    OCI_CHECK_PTR(bnd->stmt->rsts[iter], OCI_ERROR);

    /* ok.. let's Oracle update its buffers */

    rs = bnd->stmt->rsts[iter];

    rs->row_count = rows;

    /* update pointers contents */

    def = &rs->defs[bnd->dynpos];

    switch (def->col.type)
    {

        case OCI_CDT_CURSOR:
        case OCI_CDT_TIMESTAMP:
        case OCI_CDT_INTERVAL:
        case OCI_CDT_LOB:
        case OCI_CDT_FILE:

            *bufpp = def->buf.data[index];
            break;

        default:

            *bufpp = (((ub1*)def->buf.data) + (def->col.bufsize * index));
    }

    *alenp  = (ub4   *) (((ub1 *) def->buf.lens) + (def->buf.sizelen * index));
    *indp   = (dvoid *) (((ub1 *) def->buf.inds) + (sizeof(sb2) * index));
    *piecep = OCI_ONE_PIECE;
    *rcodep = NULL;

    return OCI_CONTINUE;
}

/* ------------------------------------------------------------------------ *
 *                         OCI_Define functions
 * ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ *
 * OCI_GetDefine
 * ------------------------------------------------------------------------ */

OCI_Define * OCI_GetDefine(OCI_Resultset *rs, int index)
{
    OCI_CHECK_PTR(rs, NULL);
    OCI_CHECK(index <= 0, NULL);
    OCI_CHECK(index > (int) rs->nb_defs, NULL);

    return &rs->defs[index-1];
}

/* ------------------------------------------------------------------------ *
 * OCI_GetDefineIndex
 * ------------------------------------------------------------------------ */

int OCI_GetDefineIndex(OCI_Resultset *rs, const mtext *name)
{
    OCI_CHECK_PTR(rs, -1);

    if (rs->map == NULL)
    {
        int i;

        /* create the map at the first call to OCI_Getxxxxx2() to save
           time and memory when it's not needed */

        rs->map = OCI_HashCreate(OCI_HASH_DEFAULT_SIZE, OCI_HASH_INTEGER);

        for (i = 0; i < rs->nb_defs; i++)
            OCI_HashAddInt(rs->map, rs->defs[i].col.name, (i+1));
    }

    OCI_CHECK_PTR(rs->map, -1);

    return OCI_HashGetInt(rs->map, name);
}

/* ------------------------------------------------------------------------ *
 * OCI_DefineGetData
 * ------------------------------------------------------------------------ */

void * OCI_DefineGetData(OCI_Define *def)
{
    OCI_CHECK_PTR(def, NULL);

    switch (def->col.type)
    {
        case OCI_CDT_LONG:
        case OCI_CDT_CURSOR:
        case OCI_CDT_TIMESTAMP:
        case OCI_CDT_INTERVAL:
        case OCI_CDT_LOB:
        case OCI_CDT_FILE:
        case OCI_CDT_OBJECT:
 
            /* handle based types */

            return def->buf.data[def->rs->row_mem];

        default:

            /* scalar types */

            return (((ub1*)def->buf.data) + (def->col.bufsize  * def->rs->row_mem));
    }
}

/* ------------------------------------------------------------------------ *
 * OCI_shcemaribe
 * ------------------------------------------------------------------------ */

boolean OCI_ColumnDescribe(OCI_Column *col, OCI_Connection *con,
                          OCI_Statement *stmt, void *handle, int index,
                          boolean check_null)
{
    void       *param    = NULL;
    void *ostr           = NULL;
    boolean     res      = TRUE;
    int         osize    = 0;
    ub4         htype    = 0;
    size_t      msize    = 0;
     
    if (stmt != NULL)
        htype = OCI_HTYPE_STMT;
    else
        htype = OCI_DTYPE_PARAM;

    /* allocate param handle */

    OCI_CALL(con, OCIParamGet((dvoid *) handle, htype,
                              con->err, (void**) &param, index))

    /* sql code */

    OCI_CALL(con, OCIAttrGet(param, OCI_DTYPE_PARAM, &col->ocode,
                             NULL, OCI_ATTR_DATA_TYPE, con->err))
    /* size */

    OCI_CALL(con, OCIAttrGet(param, OCI_DTYPE_PARAM, &col->size,
                             NULL, OCI_ATTR_DATA_SIZE, con->err))

    /* scale */

    OCI_CALL(con, OCIAttrGet(param, OCI_DTYPE_PARAM, &col->scale,
                             NULL, OCI_ATTR_SCALE, con->err))

    /* precision */

    OCI_CALL(con, OCIAttrGet(param, OCI_DTYPE_PARAM, &col->prec,
                             NULL, OCI_ATTR_PRECISION, con->err))

   /* charset form */

    OCI_CALL(con, OCIAttrGet(param, OCI_DTYPE_PARAM, &col->csfrm,
                             NULL, OCI_ATTR_CHARSET_FORM, con->err))
        
    /* check nullable only for table based column */

   if (check_null)
   {
       OCI_CALL(con, OCIAttrGet(param, OCI_DTYPE_PARAM, &col->null,
                                NULL, OCI_ATTR_IS_NULL, con->err))
   
   }
   else
       col->null = TRUE;

#if OCI_VERSION_COMPILE >= OCI_9 

    if (OCI_VERSION_CON_OK(con, OCI_9))
    {
        /* char used - no error checking because the on Oracle 9.0, querying
                       this param taht is not char/varchar based will cause an 
                       error */

        OCIAttrGet(param, OCI_DTYPE_PARAM, &col->charused,
                   NULL, OCI_ATTR_CHAR_USED, con->err);
    }

    /* char size */
    
    if (col->charused == TRUE)
    {
        OCI_CALL(con, OCIAttrGet(param, OCI_DTYPE_PARAM, &col->charsize,
                                 NULL, OCI_ATTR_CHAR_SIZE, con->err))
    }

    if (OCI_VERSION_CON_OK(con, OCI_9))
    {
        /* fractionnal time precision for timestamps */

         if (col->ocode == SQLT_TIMESTAMP    ||
             col->ocode == SQLT_TIMESTAMP_TZ ||
             col->ocode == SQLT_TIMESTAMP_LTZ)
        {
            OCI_CALL(con, OCIAttrGet(param, OCI_DTYPE_PARAM, &col->prec,
                                     NULL, OCI_ATTR_FSPRECISION, con->err))
        }

        /* leading and fractionnal precision for interval */

        if (col->ocode == SQLT_INTERVAL_DS ||
            col->ocode == SQLT_INTERVAL_YM)
        {
            OCI_CALL(con, OCIAttrGet(param, OCI_DTYPE_PARAM, &col->prec,
                                     NULL, OCI_ATTR_LFPRECISION, con->err))

            OCI_CALL(con, OCIAttrGet(param, OCI_DTYPE_PARAM, &col->prec2,
                                     NULL, OCI_ATTR_FSPRECISION, con->err))
        }
    }

#endif

    /* name */

    OCI_CALL(con, OCIAttrGet(param, OCI_DTYPE_PARAM, &ostr, (ub4*) &osize, 
                             OCI_ATTR_NAME, con->err))
    if (ostr != NULL)
    {
        OCI_ALLOC(mtext *, col->name, msize,
                  ((osize/sizeof(omtext))+ 1) * sizeof(mtext));
  
        if (col->name != NULL)
        {
            OCI_CopyString(ostr, col->name, &osize, 
                           sizeof(omtext), sizeof(mtext));
        }
        else
        {
            OCI_ErrorMemory(con, stmt, msize);
            res = FALSE;
        }
     }

    /* user type descriptor */

    if (col->ocode == SQLT_NTY)
    {
        OCI_CALL(con, OCIAttrGet(param, OCI_DTYPE_PARAM,  &ostr, (ub4*) &osize, 
                                 OCI_ATTR_TYPE_NAME, con->err))
      
        if (ostr != NULL)
        {   
            mtext *tmp;
            
            OCI_ALLOC(mtext *, tmp, msize, 
                      ((osize/sizeof(omtext))+ 1) * sizeof(mtext));
      
            if (tmp != NULL)
            {
               OCI_CopyString(ostr, tmp, &osize, sizeof(omtext), sizeof(mtext));
               col->nty = OCI_SchemaGet(con, tmp, OCI_SCHEMA_TYPE);
            }
            else
            {
                OCI_ErrorMemory(con, stmt, msize);
                res = FALSE;
            }
            
            OCI_FREE(tmp);
        }
     }

    /* free param handle */

    if (param != NULL)
    {
        OCI_CALL(con, OCIDescriptorFree (param, OCI_DTYPE_PARAM))
    }

    return (res && con->status == OCI_SUCCESS);
}
    
/* ------------------------------------------------------------------------ *
 * OCI_DefineMap
 * ------------------------------------------------------------------------ */

boolean OCI_ColumnMap(OCI_Column *col, OCI_Statement *stmt)
{
    OCI_CHECK_PTR(col, FALSE);

   /* sometimes, float are reported as number ... */

    if (col->ocode == SQLT_NUM && (col->scale == -127 && col->prec > 0))
        col->ocode = SQLT_FLT;

    /* map Oracle SQL code to OCILIB types and setup of internal buffer size */

    col->icode = col->ocode;

    switch (col->icode)
    {
        case SQLT_INT:

            col->type    = OCI_CDT_INTEGER;
            
            /* set bufsize only if it's not a "returning into" placeholder */
         
            if (col->bufsize == 0) 
            {
                col->subtype = OCI_NUM_INT;
                col->bufsize = sizeof(int);
            }

            break;

       case SQLT_UIN:

            col->type    = OCI_CDT_INTEGER;
           
            /* set bufsize only if it's not a "returning into" placeholder */

            if (col->bufsize == 0) 
            {
                col->subtype = OCI_NUM_UINT;
                col->bufsize = sizeof(unsigned int);
            }

            break;

#if OCI_VERSION_COMPILE >= OCI_10

        case SQLT_BFLOAT:
        case SQLT_BDOUBLE:
        case SQLT_IBFLOAT:
        case SQLT_IBDOUBLE:

            col->icode   = SQLT_BDOUBLE;
            col->type    = OCI_CDT_DOUBLE;
            col->bufsize = sizeof(double);
            break;

#endif

        case SQLT_FLT:

            col->type    = OCI_CDT_DOUBLE;
            col->bufsize = sizeof(double);
            break;

        case SQLT_VNU:
        case SQLT_PDN:
        case SQLT_NUM:

            /* OCILIB tries to map few NUMBER based columns (INT, INTEGER, ...)
               to C int types instead of double for performance.
               So, it tries to detect decimal columns from columns attributes */

            if (col->scale == 0 && col->prec != 0 && col->prec <= 38)
            {                
                col->type    = OCI_CDT_INTEGER;
                col->subtype = OCI_NUM_NUMBER;
                col->icode   = SQLT_VNU;
                col->bufsize = sizeof(OCINumber);
            }
            else
            {
                col->icode   = SQLT_FLT;
                col->type    = OCI_CDT_DOUBLE;
                col->bufsize = sizeof(double);
            }

            break;

        case SQLT_DAT:
        case SQLT_ODT:

            col->type    = OCI_CDT_DATETIME;

            /* We map to SQLT_ODT only it the column is not part of a 
               "returning into" clause (workaround for Oracle 
               known bug #3269146 
            */

            if (col->bufsize == 0)
            {
                col->icode   = SQLT_ODT;
                col->bufsize = sizeof(OCIDate);
            }

            break;

        case SQLT_CUR:
        case SQLT_RSET:

            col->type     = OCI_CDT_CURSOR;
            col->bufsize  = sizeof(OCIStmt *);
            col->dtype    = OCI_HTYPE_STMT;
            break;

        case SQLT_RDD:
        case SQLT_RID:

            col->icode   = SQLT_STR;
            col->type    = OCI_CDT_TEXT;
            col->bufsize = (OCI_SIZE_ROWID+1) * sizeof(dtext);
            break;

        case SQLT_BIN:

            col->type    = OCI_CDT_RAW;
            col->bufsize = col->size;
            break;

        case SQLT_BLOB:

            col->type    = OCI_CDT_LOB;
            col->subtype = OCI_BLOB;
            col->dtype   = OCI_DTYPE_LOB;
            col->bufsize = sizeof(OCILobLocator *);
            break;

        case SQLT_CLOB:

            col->type    = OCI_CDT_LOB;
            col->dtype   = OCI_DTYPE_LOB;
            col->bufsize = sizeof(OCILobLocator *);
           
            if (col->csfrm == SQLCS_NCHAR)
                col->subtype = OCI_NCLOB;
            else
                col->subtype = OCI_CLOB;

            break;

        case SQLT_BFILE:

            col->type    = OCI_CDT_FILE;
            col->subtype = OCI_BFILE;
            col->dtype   = OCI_DTYPE_LOB;
            col->bufsize = sizeof(OCILobLocator *);
            break;

        case SQLT_CFILE:

            col->type    = OCI_CDT_FILE;
            col->subtype = OCI_CFILE;
            col->bufsize = sizeof(OCILobLocator *);
            col->dtype   = OCI_DTYPE_LOB;
            break;

        case SQLT_LNG:
        case SQLT_LVC:
        case SQLT_LBI:
        case SQLT_LVB:
        case SQLT_VBI:

            if ((col->icode == SQLT_LNG || col->icode == SQLT_LVC) &&
                (stmt != NULL && stmt->long_mode == OCI_LONG_IMPLICIT))
            {
                 col->type = OCI_CDT_TEXT;
                 col->bufsize = (OCI_SIZE_LONG+1);
            }
            else
            {
                if (stmt != NULL)
                {
                    stmt->fetch_mode = OCI_DYNAMIC_FETCH;
                    stmt->piecewise  = TRUE;
                }

                col->type    = OCI_CDT_LONG;
                col->bufsize = INT_MAX;
                
                if (col->icode == SQLT_LBI ||
                    col->icode == SQLT_LVB ||
                    col->icode == SQLT_VBI)
                {
                    col->subtype = OCI_BLONG;
                }
                else
                {
                    col->subtype = OCI_CLONG;
                }

            }

            break;

#if OCI_VERSION_COMPILE >= OCI_9

        case SQLT_TIMESTAMP:

            col->type    = OCI_CDT_TIMESTAMP;
            col->subtype = OCI_TIMESTAMP;
            col->dtype   = OCI_DTYPE_TIMESTAMP;
            col->bufsize = sizeof(OCIDateTime *);
            break;

        case SQLT_TIMESTAMP_TZ:

            col->type    = OCI_CDT_TIMESTAMP;
            col->subtype = OCI_TIMESTAMP_TZ;
            col->dtype   = OCI_DTYPE_TIMESTAMP_TZ;
            col->bufsize = sizeof(OCIDateTime *);
            break;

        case SQLT_TIMESTAMP_LTZ:

            col->type    = OCI_CDT_TIMESTAMP;
            col->subtype = OCI_TIMESTAMP_LTZ;
            col->dtype   = OCI_DTYPE_TIMESTAMP_LTZ;
            col->bufsize = sizeof(OCIDateTime *);
            break;

        case SQLT_INTERVAL_YM:

            col->type    = OCI_CDT_INTERVAL;
            col->subtype = OCI_INTERVAL_YM;
            col->dtype   = OCI_DTYPE_INTERVAL_YM;
            col->bufsize = sizeof(OCIInterval *);
            break;

        case SQLT_INTERVAL_DS:

            col->type    = OCI_CDT_INTERVAL;
            col->subtype = OCI_INTERVAL_DS;
            col->dtype   = OCI_DTYPE_INTERVAL_DS;
            col->bufsize = sizeof(OCIInterval *);
            break;

#endif

#if OCI_VERSION_COMPILE >= OCI_9

        case SQLT_PNTY:

#endif

        case SQLT_NTY:
        {
            col->icode   = SQLT_NTY;
            col->type    = OCI_CDT_OBJECT;                    
            col->bufsize = sizeof(void *);
            break;
        }
        case SQLT_REF:

            /* not supported datatypes */

            OCI_ErrorGeneric(stmt->con, stmt, OCI_ERR_NOT_SUPPORTED, 0,
                             MT("Oracle REF Datatype is not supported"));

            return FALSE;

        case SQLT_CHR:
        case SQLT_STR:
        case SQLT_VCS:
        case SQLT_AFC:
        case SQLT_AVC:
        case SQLT_VST:
        case SQLT_LAB:
        case SQLT_OSL:
        case SQLT_SLS:
        default:

            col->icode   = SQLT_STR;
            col->type    = OCI_CDT_TEXT;
            col->bufsize = (col->size + 1) * sizeof(dtext);
            break;
    }

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_DefineAlloc
 * ------------------------------------------------------------------------ */

boolean OCI_DefineAlloc(OCI_Define *def)
{
    size_t msize   = 0;
    size_t bufsize = 0;
    size_t indsize = 0;
    int i;

    /*
       this function allocates internal buffers, handles, indicators, arrays, ...
       for the given outpout define handle
    */

    OCI_CHECK_PTR(def, FALSE);

    /* Allocate null indicators array */

    if (def->col.icode == SQLT_NTY)
        indsize = sizeof(void*);
    else
        indsize = sizeof(sb2);

    OCI_ALLOC(void *, def->buf.inds, msize, def->buf.count*indsize);

    if (def->buf.inds == NULL)
    {
        OCI_ErrorMemory(def->rs->stmt->con, def->rs->stmt, msize);
        return FALSE;
    }

    /* Allocate row data sizes array */

    OCI_ALLOC(void *, def->buf.lens, msize, def->buf.count*def->buf.sizelen);

    if (def->buf.lens == NULL)
    {
        OCI_ErrorMemory(def->rs->stmt->con, def->rs->stmt, msize);
        return FALSE;
    }

    /* initialize length array with buffer default size. 
       But, Oracle uses different sizes for static fetch and callback fetch....*/

    for (i=0; i < def->buf.count; i++)
    {
        if (def->buf.sizelen == sizeof(ub2))
            *(ub2*)(((ub1 *)def->buf.lens) + def->buf.sizelen*(i)) = (ub2)def->col.bufsize;
        else if (def->buf.sizelen == sizeof(ub4))
            *(ub4*)(((ub1 *)def->buf.lens) + def->buf.sizelen*(i)) = (ub4)def->col.bufsize;
   }

    /* Allocate buffer array */

    if (def->col.type == OCI_CDT_LONG)
        bufsize = sizeof(OCI_Long *);
    else
        bufsize = def->col.bufsize;
    
    OCI_ALLOC(void **, def->buf.data, msize, def->buf.count * bufsize)

    if (def->buf.data == NULL)
    {
        OCI_ErrorMemory(def->rs->stmt->con, def->rs->stmt, msize);
        return FALSE;
    }

    /* Allocate descriptor for cursor, lob and file, interval and timestamp  */

    if (def->col.dtype != 0)
    {
        for (i = 0; i < def->buf.count; i++)
        {
            if (def->col.type == OCI_CDT_CURSOR)

                OCI_CALL(def->rs->stmt->con,
                         OCIHandleAlloc(OCILib.env,
                                        (dvoid **) &(def->buf.data[i]),
                                        def->col.dtype, 0, (dvoid **) NULL))
            else
            {
                OCI_CALL(def->rs->stmt->con,
                         OCIDescriptorAlloc(OCILib.env,
                                            (dvoid **) &(def->buf.data[i]),
                                            def->col.dtype, 0, (dvoid **) NULL))

                if (def->col.type == OCI_CDT_LOB)
                {
                    ub4 empty = 0;

                    OCI_CALL(def->rs->stmt->con, 
                             OCIAttrSet(def->buf.data[i], def->col.dtype,
                             &empty, sizeof(empty), OCI_ATTR_LOBEMPTY,
                             def->rs->stmt->con->err))

                }
            }

            OCI_CHECK_SUCCESS(def->rs->stmt->con->status, FALSE);
        }
    }

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_DefineDef
 * ------------------------------------------------------------------------ */

boolean OCI_DefineDef(OCI_Define *def)
{
    OCI_CHECK_PTR(def, FALSE);

    /* oracle defining */

    OCI_CALL(def->rs->stmt->con, OCIDefineByPos(def->rs->stmt->stmt,
                                                (OCIDefine **) &def->buf.handle,
                                                def->rs->stmt->con->err,
                                                def->rs->nb_defs,
                                                (void *) def->buf.data,
                                                (sb4)    def->col.bufsize,
                                                (ub2)    def->col.icode,
                                                (void *) def->buf.inds,
                                                (ub2 *)  def->buf.lens,
                                                (ub2 *)  NULL,
                                                def->rs->stmt->fetch_mode))

    if (def->col.type == OCI_CDT_OBJECT)
    {
        OCI_CALL(def->rs->stmt->con, OCIDefineObject((OCIDefine *) def->buf.handle,    
                                                     def->rs->stmt->con->err, 
                                                     def->col.nty->tdo, 
                                                     (void **) def->buf.data, 
                                                     NULL,
                                                     (void **) def->buf.inds,
                                                     NULL))
    }

    if (def->col.csfrm == SQLCS_NCHAR
#ifdef OCI_USERDATA_UNICODE        
        || (def->col.type == OCI_CDT_TEXT && OCILib.ver_runtime >= OCI_9)
#endif
        )
    {
        ub1 csfrm = SQLCS_NCHAR;

        OCI_CALL(def->rs->stmt->con, OCIAttrSet((dvoid *) def->buf.handle,
                                                (ub4) OCI_HTYPE_DEFINE,
                                                (dvoid *) &csfrm, 
                                                sizeof(csfrm),
                                                (ub4) OCI_ATTR_CHARSET_FORM,
                                                def->rs->stmt->con->err));
    }

#ifdef OCI_CHARSET_MIXED

    /* setup Unicode mode for user data on mixed builds */
    {
        ub2 csid = OCI_UTF16ID;

        OCI_CALL(def->rs->stmt->con, OCIAttrSet((dvoid *) def->buf.handle,
                                                (ub4) OCI_HTYPE_DEFINE,
                                                (dvoid *) &csid, 
                                                sizeof(csid),
                                                (ub4) OCI_ATTR_CHARSET_ID,
                                                def->rs->stmt->con->err));
    }

#endif

    return def->rs->stmt->con->status == OCI_SUCCESS;
}

/* ------------------------------------------------------------------------ *
 *                         OCI_Lob functions
 * ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ *
 * OCI_LobInit
 * ------------------------------------------------------------------------ */

 OCI_Lob * OCI_LobInit(OCI_Connection *con, OCI_Lob **plob,
                       OCILobLocator *handle, ub1 type)
{
    ub2 csid      = OCI_DEFAULT;
    ub1 csfrm     = OCI_DEFAULT;
    OCI_Lob * lob = NULL;
    ub1 lobtype   = 0;
    size_t msize  = 0;

    OCI_CHECK_PTR(con, NULL);
    OCI_CHECK_PTR(plob, NULL);

    if((*plob) == NULL)
        OCI_ALLOC(OCI_Lob *, *plob, msize, sizeof(OCI_Lob));

    lob = *plob;

    if (lob)
    {
        lob->type   = type;
        lob->con    = con;
        lob->handle = handle;
        lob->offset = 1;
    }
    else
        OCI_ErrorMemory(con, NULL, msize);

    if (lob != NULL && lob->handle == NULL)
    {
        ub4 empty = 0;

        /* allocate handle for non fetched lob (temporay lob) */

        lob->hstate = OCI_OBJECT_ALLOCATED;

        if (lob->type == OCI_NCLOB)
        {
            csfrm   = SQLCS_NCHAR;
            lobtype = OCI_TEMP_CLOB;
        }
        else if (lob->type == OCI_CLOB)
        {
            csfrm   = SQLCS_IMPLICIT;
            lobtype = OCI_TEMP_CLOB;
        }
        else
            lobtype = OCI_TEMP_BLOB;

        OCI_CALL(lob->con,
                 OCIDescriptorAlloc(OCILib.env,
                                    (dvoid **) (void *) &lob->handle,
                                    OCI_DTYPE_LOB, 0, (dvoid **) NULL))

        OCI_CALL(lob->con, OCIAttrSet(lob->handle, OCI_DTYPE_LOB,
                                      &empty, sizeof(empty), 
                                      OCI_ATTR_LOBEMPTY,
                                      lob->con->err))

        OCI_CALL(lob->con,
                 OCILobCreateTemporary(lob->con->cxt,
                                       lob->con->err,
                                       lob->handle,
                                       csid,
                                       csfrm,
                                       lobtype,
                                       FALSE,
                                       (OCIDuration) OCI_DURATION_SESSION))

        if (lob->con->status != OCI_SUCCESS)
        {
            OCI_ErrorAPI(lob->con->err, con, NULL);
            OCI_FREE(lob);
        }
    }
    else
        lob->hstate = OCI_OBJECT_FETCHED_CLEAN;

    return lob;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobCreate
 * ------------------------------------------------------------------------ */

OCI_Lob * OCI_API OCI_LobCreate(OCI_Connection *con, int type)
{
    OCI_Lob *lob = NULL;

    OCI_CHECK_PTR(con, NULL);

    return OCI_LobInit(con, &lob, NULL, (ub1) type);
}

/* ------------------------------------------------------------------------ *
 * OCI_LobFree
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_LobFree(OCI_Lob *lob)
{
    OCI_CHECK_PTR(lob, FALSE);
    OCI_CHECK(lob->hstate == OCI_OBJECT_FETCHED_CLEAN, FALSE);

    if (OCI_LobIsTemporary(lob))
    {
        OCI_CALL(lob->con, OCILobFreeTemporary(lob->con->cxt, lob->con->err,
                                               lob->handle))

        if (lob->con->status != OCI_SUCCESS)
            OCI_ErrorAPI(lob->con->err, lob->con, NULL);
    }

    if (lob->hstate == OCI_OBJECT_ALLOCATED)
        OCI_DIRECT(lob->con, OCIDescriptorFree(lob->handle, OCI_DTYPE_LOB))

    OCI_FREE(lob);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobGetType
 * ------------------------------------------------------------------------ */

int OCI_API OCI_LobGetType(OCI_Lob *lob)
{
    OCI_CHECK_PTR(lob, OCI_UNKNOWN);

    return lob->type;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobSeek
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_LobSeek(OCI_Lob *lob, big_uint offset, int mode)
{
    big_uint size = 0;

    OCI_CHECK_PTR(lob, FALSE);

    size = OCI_LobGetLength(lob);

    if ((mode == OCI_SEEK_CUR && (offset + lob->offset-1) > size))
        return FALSE;

    if(mode == OCI_SEEK_SET)
        lob->offset  = offset + 1;
    else if(mode == OCI_SEEK_END)
        lob->offset  = size-offset + 1;
    else if(mode == OCI_SEEK_CUR)
        lob->offset += offset;

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobGetOffset
 * ------------------------------------------------------------------------ */

big_uint OCI_API OCI_LobGetOffset(OCI_Lob *lob)
{
    OCI_CHECK_PTR(lob, 0);

    return lob->offset-1;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobRead
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_LobRead(OCI_Lob *lob, void *buffer, unsigned int len)
{
    ub4 size_in  = 0;
    ub4 size_out = 0;
    ub2 csid     = 0;
    ub1 csfrm    = 0;

    OCI_CHECK_PTR(lob, 0);

    size_out = size_in = len;

    if (lob->type != OCI_BLOB)
    {

#ifndef OCI_CHARSET_ANSI

        csid = OCI_UTF16ID;

#endif        
        
        size_in *= sizeof(odtext);
    }

    if (lob->type == OCI_NCLOB)
        csfrm = SQLCS_NCHAR;
    else
        csfrm = SQLCS_IMPLICIT;

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        ub8 size_char = (ub8) len;
        ub8 size_byte = (ub8) size_in;
        
        OCI_CALL(lob->con, OCILobRead2(lob->con->cxt,
                                       lob->con->err,
                                       lob->handle,
                                       &size_byte,
                                       &size_char, 
                                       lob->offset, 
                                       buffer, size_in, 
                                       OCI_ONE_PIECE, 0, 0,
                                       csid, csfrm))

        if (lob->type == OCI_BLOB)
            size_out = (ub4) size_byte;
        else
            size_out = (ub4) size_char;
    }

    else

#endif

    {
        ub4 offset = (ub4) lob->offset;

        OCI_CALL(lob->con, OCILobRead(lob->con->cxt, 
                                      lob->con->err,
                                      lob->handle,
                                      &size_out, 
                                      offset, 
                                      buffer, size_in,
                                      0, 0, csid, csfrm))
    }


    OCI_CHECK_ERROR(lob->con, NULL, 0);

    if (lob->type != OCI_BLOB)
        OCI_ConvertString(buffer, (int) size_out, sizeof(odtext), sizeof(dtext));

    lob->offset += (big_uint) size_out;

    return size_out;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobWrite
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_LobWrite(OCI_Lob *lob, void *buffer, unsigned int len)
{
    ub4 size_in  = 0;
    ub4 size_out = 0;
    ub2 csid     = 0;
    ub1 csfrm    = 0;
    void *obuf   = NULL;

    OCI_CHECK_PTR(lob, 0);

    size_out = size_in = len;
    
    if (lob->type != OCI_BLOB)
    {

#ifndef OCI_CHARSET_ANSI

        csid = OCI_UTF16ID;

#endif

        size_in *= sizeof(dtext);
        obuf     = OCI_GetInputDataString(buffer, (int *) &size_in);
    }
    else
       obuf = buffer;

    if (lob->type == OCI_NCLOB)
        csfrm = SQLCS_NCHAR;
    else
        csfrm = SQLCS_IMPLICIT;


#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        ub8 size_char = (ub8) len;
        ub8 size_byte = (ub8) size_in;
        
        OCI_CALL(lob->con, OCILobWrite2(lob->con->cxt,
                                        lob->con->err, 
                                        lob->handle,
                                        &size_byte,
                                        &size_char, 
                                        lob->offset, 
                                        buffer, size_in, 
                                        OCI_ONE_PIECE, 0, 0, 
                                        csid, csfrm))

        if (lob->type == OCI_BLOB)
            size_out = (ub4) size_byte;
        else
            size_out = (ub4) size_char;
    }

    else

#endif

    {
        ub4 offset = (ub4) lob->offset;
   
        OCI_CALL(lob->con, OCILobWrite(lob->con->cxt, lob->con->err,
                                       lob->handle, &size_out,
                                       offset, obuf, size_in, 
                                       OCI_ONE_PIECE, 0, 0,
                                       csid, csfrm))
    }

    if (lob->type != OCI_BLOB)
        OCI_ReleaseDataString(obuf);

    OCI_CHECK_ERROR(lob->con, NULL, 0);

    lob->offset += (big_uint) size_out;

    return size_out;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobTruncate
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_LobTruncate(OCI_Lob *lob, big_uint size)
{
    OCI_CHECK_PTR(lob, FALSE);

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
         OCI_CALL(lob->con, OCILobTrim2(lob->con->cxt, lob->con->err,
                                        lob->handle, size))
    }
    else

#endif
 
    {
       ub4 size32 = (ub4) size;

       OCI_CALL(lob->con, OCILobTrim(lob->con->cxt, lob->con->err,
                                      lob->handle, size32))
    }

    OCI_CHECK_ERROR(lob->con, NULL, FALSE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobErase
 * ------------------------------------------------------------------------ */

big_uint OCI_API OCI_LobErase(OCI_Lob *lob, big_uint offset, big_uint size)
{
    OCI_CHECK_PTR(lob, 0);
    OCI_CHECK_NUM(size, 0);

 #ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        OCI_CALL(lob->con, OCILobErase2(lob->con->cxt, lob->con->err, 
                                        lob->handle, &size, offset+1))
    }
    else

#endif
 
    {
        ub4 size32   = (ub4) size;
        ub4 offset32 = (ub4) offset;

        OCI_CALL(lob->con, OCILobErase(lob->con->cxt, lob->con->err,
                                       lob->handle, &size32, offset32+1))

        size = (big_uint) size32;
    }
                            
    OCI_CHECK_ERROR(lob->con, NULL, 0);

    return size;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobGetLength
 * ------------------------------------------------------------------------ */

big_uint OCI_API OCI_LobGetLength(OCI_Lob *lob)
{
    big_uint size = 0;

    OCI_CHECK_PTR(lob, 0);
 
#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        OCI_CALL(lob->con, OCILobGetLength2(lob->con->cxt, lob->con->err,
                                            lob->handle, &size))
    }
    else

#endif
 
    {
        ub4 size32 = (ub4) size;
   
        OCI_CALL(lob->con, OCILobGetLength(lob->con->cxt, lob->con->err,
                                           lob->handle, &size32))

        size = (big_uint) size32;
    }

    OCI_CHECK_ERROR(lob->con, NULL, 0);

    return size;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobCopy
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_LobCopy(OCI_Lob *lob, OCI_Lob *lob_src,
                            big_uint offset_dst,
                            big_uint offset_src,
                            big_uint count)
{
    OCI_CHECK_PTR(lob, FALSE);
    OCI_CHECK_PTR(lob_src, FALSE);

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
 
        OCI_CALL(lob->con, OCILobCopy2(lob->con->cxt, lob->con->err, lob->handle,
                                       lob_src->handle, count, offset_dst+1,
                                       offset_src+1))
    }
    else

#endif
 
    {
       ub4 count32      = (ub4) count;
       ub4 offset_src32 = (ub4) offset_src;
       ub4 offset_dst32 = (ub4) offset_dst;


       OCI_CALL(lob->con, OCILobCopy(lob->con->cxt, lob->con->err, lob->handle,
                                     lob_src->handle, count32, offset_dst32+1,
                                     offset_src32+1))

    }
    
    OCI_CHECK_ERROR(lob->con, NULL, FALSE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobCopyFromFile
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_LobCopyFromFile(OCI_Lob *lob, OCI_File *file,
                                    big_uint offset_dst,
                                    big_uint offset_src,
                                    big_uint count)
{
    OCI_CHECK_PTR(lob, FALSE);
    OCI_CHECK_PTR(file, FALSE);

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        OCI_CALL(lob->con, OCILobLoadFromFile2(lob->con->cxt, lob->con->err,
                                               lob->handle, file->handle, count,
                                               offset_dst+1, offset_src+1))
    }
    else

#endif
 
    {
       ub4 count32      = (ub4) count;
       ub4 offset_src32 = (ub4) offset_src;
       ub4 offset_dst32 = (ub4) offset_dst;


        OCI_CALL(lob->con, OCILobLoadFromFile(lob->con->cxt, lob->con->err,
                                              lob->handle, file->handle, count32,
                                              offset_dst32+1, offset_src32+1))
    }

    OCI_CHECK_ERROR(lob->con, NULL, FALSE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobAppend
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_LobAppend(OCI_Lob *lob, void *buffer, unsigned int len)
{ 
    ub4 size_in  = 0;
    ub4 size_out = 0;
    ub2 csid     = 0;
    ub1 csfrm    = 0;
    void *obuf   = NULL;

    OCI_CHECK_PTR(lob, 0);

#ifndef OCI_CHARSET_ANSI
    csid = OCI_UTF16ID;
#endif

    /* OCILobWriteAppend() seems to cause problems on Oracle client 8.1 and 9.0 
       It's an Oracle known bug #886191
       So we do e OCI_LobWrite(lob, bufferxplicitly seek and write calls instead */

    if (OCILib.ver_runtime < OCI_10)
    {
       return OCI_LobSeek(lob, OCI_LobGetLength(lob), OCI_SEEK_SET) &&
              OCI_LobWrite(lob, buffer, len);
    }        

    size_out = size_in = len;
    
    if (lob->type != OCI_BLOB)
    {
        size_in *= sizeof(dtext);
        obuf  = OCI_GetInputDataString(buffer, (int *) &size_in);
    }
    else
       obuf = buffer;

    if (lob->type == OCI_NCLOB)
        csfrm = SQLCS_NCHAR;
    else
        csfrm = SQLCS_IMPLICIT;


#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        ub8 size_char = (ub8) len;
        ub8 size_byte = (ub8) size_in;
        
        OCI_CALL(lob->con, OCILobWriteAppend2(lob->con->cxt,
                                              lob->con->err, 
                                              lob->handle,
                                              &size_byte,
                                              &size_char, 
                                              buffer, size_in, 
                                              OCI_ONE_PIECE, 0, 0, 
                                              csid, csfrm))
        if (lob->type == OCI_BLOB)
            size_out = (ub4) size_byte;
        else
            size_out = (ub4) size_char;
    }

    else

#endif

    {
         OCI_CALL(lob->con, OCILobWriteAppend(lob->con->cxt, lob->con->err,
                                                 lob->handle, &size_out,
                                                 obuf, size_in, 
                                                 OCI_ONE_PIECE, 0, 0,
                                                 csid, csfrm))
    }

    if (lob->type != OCI_BLOB)
        OCI_ReleaseDataString(obuf);

    OCI_CHECK_ERROR(lob->con, NULL, 0);

    lob->offset += (big_uint) size_out;

    return size_out;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobAppendLob
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_LobAppendLob(OCI_Lob *lob, OCI_Lob *lob_src)
{
    OCI_CHECK_PTR(lob, FALSE);
    OCI_CHECK_PTR(lob_src, FALSE);

    /* 
       this might cause an ORA-24805 on Oracle 8.1.x only !
       I could'nt find a bug ID on Metalink, but Oracle 9i had many fixes for
       lobs ! 
    */

    OCI_CALL(lob->con, OCILobAppend(lob->con->cxt, lob->con->err, lob->handle,
                                    lob_src->handle))

    OCI_CHECK_ERROR(lob->con, NULL, FALSE);

    lob->offset = OCI_LobGetLength(lob);

    return TRUE;
 }

/* ------------------------------------------------------------------------ *
 * OCI_LobIsTemporary
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_LobIsTemporary(OCI_Lob *lob)
{
    boolean res = FALSE;

    OCI_CHECK_PTR(lob, FALSE);

    OCI_CALL(lob->con, OCILobIsTemporary(OCILib.env, lob->con->err,
                                         lob->handle, &res))

    OCI_CHECK_ERROR(lob->con, NULL, FALSE);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobOpen
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_LobOpen(OCI_Lob *lob, int mode)
{
    OCI_CHECK_PTR(lob, FALSE);

    OCI_CALL(lob->con, OCILobOpen(lob->con->cxt, lob->con->err, lob->handle,
                                  (ub1) mode))

    OCI_CHECK_ERROR(lob->con, NULL, FALSE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobClose
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_LobClose(OCI_Lob *lob)
{
    OCI_CHECK_PTR(lob, FALSE);

    OCI_CALL(lob->con, OCILobClose(lob->con->cxt, lob->con->err, lob->handle))

    OCI_CHECK_ERROR(lob->con, NULL, FALSE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobIsEqual
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_LobIsEqual(OCI_Lob *lob, OCI_Lob *lob2)
{
    boolean res = FALSE;

    OCI_CHECK_PTR(lob, FALSE);
    OCI_CHECK_PTR(lob2, FALSE);

    OCI_CALL(lob->con, OCILobIsEqual(OCILib.env,
                                     lob->handle,
                                     lob2->handle,
                                     &res))

    OCI_CHECK_ERROR(lob->con, NULL, FALSE);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobAssign
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_LobAssign(OCI_Lob *lob, OCI_Lob *lob_src)
{
    OCI_CHECK_PTR(lob,     FALSE);
    OCI_CHECK_PTR(lob_src, FALSE);

    if (lob->hstate == OCI_OBJECT_ALLOCATED)
    {
        OCI_CALL(lob->con, OCILobLocatorAssign(lob->con->cxt, lob->con->err,
                                               lob_src->handle, &lob->handle))
  
    }
    else
    {
        OCI_CALL(lob->con, OCILobAssign(OCILib.env, lob->con->err,
                                        lob_src->handle, &lob->handle))
    }

    OCI_CHECK_ERROR(lob->con, NULL, FALSE);
  
    return TRUE;
}

/* ------------------------------------------------------------------------ *
 *                         OCI_File functions
 * ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ *
 * OCI_FileInit
 * ------------------------------------------------------------------------ */

 OCI_File * OCI_FileInit(OCI_Connection *con, OCI_File **pfile,
                         OCILobLocator *handle, ub1 type)
{
    OCI_File *file = NULL;
    size_t msize   = 0;

    OCI_CHECK_PTR(con, NULL);
    OCI_CHECK_PTR(pfile, NULL);

    if((*pfile) == NULL)
        OCI_ALLOC(OCI_File *, *pfile, msize, sizeof(OCI_File));

    file = *pfile;

    if (file)
    {
        file->type   = type;
        file->con    = con;
        file->handle = handle;
        file->offset = 1;
    }
    else
        OCI_ErrorMemory(con, NULL, msize);

    if (file != NULL)
    {
        /* reset file info */

        if (file)
        {
           if (file->dir != NULL)
               file->dir[0] = 0;

           if (file->name != NULL)
               file->name[0] = 0;
        }

        if (file->handle == NULL)
        {
            /* allocate handle for non fetched file (local file object) */

            file->hstate = OCI_OBJECT_ALLOCATED;


            OCI_CALL(file->con,
                     OCIDescriptorAlloc(OCILib.env,
                                        (dvoid **) (void *) &file->handle,
                                        OCI_DTYPE_LOB, 0, (dvoid **) NULL))

            if (file->con->status != OCI_SUCCESS)
            {
                OCI_ErrorAPI(file->con->err, con, NULL);
                OCI_FREE(file);
            }
        }
        else
            file->hstate = OCI_OBJECT_FETCHED_CLEAN;
    }

    return file;
}

/* ------------------------------------------------------------------------ *
 * OCI_FileGetInfo
 * ------------------------------------------------------------------------ */

boolean OCI_FileGetInfo(OCI_File *file)
{
    void *ostr1  = NULL;
    void *ostr2  = NULL;
    int  osize1  = 0;
    int  osize2  = 0;
    ub2  usize1  = 0;
    ub2  usize2  = 0;
    size_t msize = 0;

    OCI_CHECK_PTR(file, FALSE);

    /* directory name */

    if (file->dir == NULL)
    {
        OCI_ALLOC(mtext *, file->dir, msize,
                  (OCI_SIZE_DIRECTORY+1)*sizeof(mtext));

        if (file->dir == NULL)
        {
            OCI_ErrorMemory(file->con, NULL, msize);
            return FALSE;
        }
    }
    else
        file->dir[0] = 0;

    /* file name */

    if (file->name == NULL)
    {
        OCI_ALLOC(mtext *, file->name, msize, 
                  (OCI_SIZE_FILENAME+1)*sizeof(mtext));

        if (file->name == NULL)
        {
            OCI_FREE(file->dir);
            OCI_ErrorMemory(file->con, NULL, msize);
            return FALSE;
         }
    }
    else
        file->name[0] = 0;

    /* retrieve name */

    osize1 = OCI_SIZE_DIRECTORY  * sizeof(mtext);
    ostr1  = OCI_GetInputMetaString(file->dir, &osize1);

    osize2 = OCI_SIZE_FILENAME  * sizeof(mtext);
    ostr2  = OCI_GetInputMetaString(file->name, &osize1);
 
    usize1 = (ub2) osize1;
    usize2 = (ub2) osize2;

    OCI_CALL(file->con, OCILobFileGetName(OCILib.env,
                                          file->con->err,
                                          file->handle,
                                          (text *) ostr1,
                                          (ub2*) &usize1,
                                          (text *) ostr2,
                                          (ub2*) &usize2))

    osize1 = (int) usize1;
    osize2 = (int) usize2;

    OCI_GetOutputMetaString(ostr1, file->dir,  &osize1);
    OCI_GetOutputMetaString(ostr2, file->name, &osize2);

    OCI_ReleaseMetaString(ostr1);
    OCI_ReleaseMetaString(ostr2);

    OCI_CHECK_ERROR(file->con, NULL, FALSE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_FileCreate
 * ------------------------------------------------------------------------ */

OCI_File * OCI_API OCI_FileCreate(OCI_Connection *con, int type)
{
    OCI_File *file = NULL;

    OCI_CHECK_PTR(con, NULL);

    return OCI_FileInit(con, &file, NULL, (ub1) type);
}

/* ------------------------------------------------------------------------ *
 * OCI_FileSeek
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_FileSeek(OCI_File *file, big_uint offset, int mode)
{
    big_uint size = 0;

    OCI_CHECK_PTR(file, FALSE);

    size = OCI_FileGetSize(file);

    if ((mode == OCI_SEEK_CUR && (offset + file->offset-1) > size))
        return FALSE;

    if(mode == OCI_SEEK_SET)
        file->offset  = offset + 1;
    else if(mode == OCI_SEEK_END)
        file->offset  = size-offset + 1;
    else if(mode == OCI_SEEK_CUR)
        file->offset += offset;

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_FileGetOffset
 * ------------------------------------------------------------------------ */

big_uint OCI_API OCI_FileGetOffset(OCI_File *file)
{
    OCI_CHECK_PTR(file, 0);

    return file->offset-1;
}

/* ------------------------------------------------------------------------ *
 * OCI_FileRead
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_FileRead(OCI_File *file, void *buffer, unsigned int len)
{
    ub4 size_in  = 0;
    ub4 size_out = 0;

    OCI_CHECK_PTR(file, 0);

    size_out = size_in = len;

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        ub8 size_char = (ub8) len;
        ub8 size_byte = (ub8) size_in;
        
        OCI_CALL(file->con, OCILobRead2(file->con->cxt,
                                       file->con->err,
                                       file->handle,
                                       &size_byte,
                                       &size_char, 
                                       file->offset, 
                                       buffer, size_in, 
                                       OCI_ONE_PIECE, 0, 0,
                                       0, SQLCS_IMPLICIT))
    }

    else

#endif

    {
        ub4 offset = (ub4) file->offset;

        OCI_CALL(file->con, OCILobRead(file->con->cxt, 
                                      file->con->err,
                                      file->handle,
                                      &size_out, 
                                      offset, 
                                      buffer, size_in,
                                      0, 0, 0, SQLCS_IMPLICIT))
    }

    OCI_CHECK_ERROR(file->con, NULL, 0);

    file->offset += (big_uint) size_out;

    return size_out;
}

/* ------------------------------------------------------------------------ *
 * OCI_FileGetType
 * ------------------------------------------------------------------------ */

int OCI_API OCI_FileGetType(OCI_File *file)
{
    OCI_CHECK_PTR(file, 0);

    return file->type;
}

/* ------------------------------------------------------------------------ *
 * OCI_FileGetSize
 * ------------------------------------------------------------------------ */

big_uint OCI_API OCI_FileGetSize(OCI_File *file)
{
    big_uint size = 0;

    OCI_CHECK_PTR(file, 0);
 
#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        OCI_CALL(file->con, OCILobGetLength2(file->con->cxt, file->con->err,
                                             file->handle, &size))

    }
    else

#endif
 
    {
        ub4 size32 = (ub4) size;
   
        OCI_CALL(file->con, OCILobGetLength(file->con->cxt, file->con->err,
                                            file->handle, &size32))

        size = (big_uint) size32;
    }

    OCI_CHECK_ERROR(file->con, NULL, 0);

    return size;
}

/* ------------------------------------------------------------------------ *
 * OCI_FileFree
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_FileFree(OCI_File *file)
{
    OCI_CHECK_PTR(file, FALSE);
    OCI_CHECK(file->hstate == OCI_OBJECT_FETCHED_CLEAN, FALSE);

    OCI_FREE(file->dir);
    OCI_FREE(file->name);

    if (file->hstate == OCI_OBJECT_ALLOCATED)
        OCI_DIRECT(file->con, OCIDescriptorFree(file->handle, OCI_DTYPE_LOB))

    OCI_FREE(file);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobFileExists
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_FileExists(OCI_File *file)
{
    boolean res = FALSE;

    OCI_CHECK_PTR(file, FALSE);

    OCI_CALL(file->con, OCILobFileExists(file->con->cxt, file->con->err,
                                         file->handle, &res))

    OCI_CHECK_ERROR(file->con, NULL, FALSE);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_FileSetName
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_FileSetName(OCI_File *file, const mtext *dir, 
                                const mtext *name)
{
    void *ostr1 = NULL;
    void *ostr2 = NULL;
    int osize1  = -1;
    int osize2  = -1;

    OCI_CHECK_PTR(file, FALSE);

    ostr1 = OCI_GetInputMetaString(dir,  &osize1);
    ostr2 = OCI_GetInputMetaString(name, &osize2);

    OCI_CALL(file->con, OCILobFileSetName(OCILib.env,
                                          file->con->err,
                                          &file->handle,
                                          (CONST text *) ostr1, (ub2) osize1,
                                          (CONST text *) ostr2, (ub2) osize2))

    OCI_ReleaseMetaString(ostr1);
    OCI_ReleaseMetaString(ostr2);

    OCI_CHECK_ERROR(file->con, NULL, FALSE);
 
    OCI_FileGetInfo(file);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_FileGetDirectory
 * ------------------------------------------------------------------------ */

const mtext * OCI_API OCI_FileGetDirectory(OCI_File *file)
{
    OCI_CHECK_PTR(file, NULL);

    if (file->name == NULL || file->name[0] == 0)
        OCI_FileGetInfo(file);

    return file->dir;
}

/* ------------------------------------------------------------------------ *
 * OCI_FileGetName
 * ------------------------------------------------------------------------ */

const mtext * OCI_API OCI_FileGetName(OCI_File *file)
{
    OCI_CHECK_PTR(file, NULL);

    if (file->name == NULL || file->name[0] == 0)
        OCI_FileGetInfo(file);

    return file->name;
}

/* ------------------------------------------------------------------------ *
 * OCI_FileOpen
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_FileOpen(OCI_File *file)
{
    OCI_CHECK_PTR(file, FALSE);

    OCI_CALL(file->con, OCILobFileOpen(file->con->cxt, file->con->err,
                                       file->handle, OCI_LOB_READONLY))

    OCI_CHECK_ERROR(file->con, NULL, FALSE);

    file->con->nb_files++;

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_LobFileIsOpen
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_FileIsOpen(OCI_File *file)
{
    boolean res = FALSE;

    OCI_CHECK_PTR(file, FALSE);

    OCI_CALL(file->con, OCILobFileIsOpen(file->con->cxt, file->con->err,
                                         file->handle, &res))

    OCI_CHECK_ERROR(file->con, NULL, FALSE);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_FileClose
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_FileClose(OCI_File *file)
{
    OCI_CHECK_PTR(file, FALSE);

    OCI_CALL(file->con, OCILobFileClose(file->con->cxt, file->con->err,
                                        file->handle))

    OCI_CHECK_ERROR(file->con, NULL, FALSE);

    file->con->nb_files--;

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_FileIsEqual
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_FileIsEqual(OCI_File *file, OCI_File *file2)
{
    boolean res = FALSE;

    OCI_CHECK_PTR(file, FALSE);
    OCI_CHECK_PTR(file2, FALSE);

    OCI_CALL(file->con, OCILobIsEqual(OCILib.env,
                                      file->handle,
                                      file2->handle,
                                      &res))

    OCI_CHECK_ERROR(file->con, NULL, FALSE);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_FileAssign
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_FileAssign(OCI_File *file, OCI_File *file_src)
{
    OCI_CHECK_PTR(file,     FALSE);
    OCI_CHECK_PTR(file_src, FALSE);

    if (file->hstate == OCI_OBJECT_ALLOCATED)
    {
        OCI_CALL(file->con, OCILobLocatorAssign(file->con->cxt, file->con->err,
                                                file_src->handle, &file->handle))
  
    }
    else
    {
        OCI_CALL(file->con, OCILobAssign(OCILib.env, file->con->err,
                                         file_src->handle, &file->handle))
    }

    OCI_CHECK_ERROR(file->con, NULL, FALSE);
  
    OCI_FileGetInfo(file);

    OCI_CHECK_ERROR(file->con, NULL, FALSE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 *                         OCI_Long functions
 * ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ *
 * OCI_LongInit
 * ------------------------------------------------------------------------ */

OCI_Long * OCI_LongInit(OCI_Statement *stmt, OCI_Long **plg,
                        OCI_Define *def, int type)
{
    OCI_Long *lg = NULL;
    size_t msize = 0;

    OCI_CHECK_PTR(stmt, FALSE);
    OCI_CHECK_PTR(plg, FALSE);

    if((*plg) == NULL)
        OCI_ALLOC(OCI_Long *, *plg, msize, sizeof(OCI_Long));

    lg = *plg;

    if (lg)
    {
        lg->size = 0;
        lg->stmt = stmt;
        lg->def  = def;
        lg->type = type;

        if (def != NULL)
        {
            lg->hstate = OCI_OBJECT_FETCHED_CLEAN;
        }
        else
        {
            lg->hstate = OCI_OBJECT_ALLOCATED;
        }
    }
    else
        OCI_ErrorMemory(stmt->con, stmt, msize);

    return lg;
}

/* ------------------------------------------------------------------------ *
 * OCI_LongCreate
 * ------------------------------------------------------------------------ */

OCI_Long * OCI_API OCI_LongCreate(OCI_Statement *stmt, int type)
{
    OCI_Long *lg = NULL;

    OCI_CHECK_PTR(stmt, NULL);

    return OCI_LongInit(stmt, &lg, NULL, type);
}

/* ------------------------------------------------------------------------ *
 * OCI_LongFree
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_LongFree(OCI_Long *lg)
{
    OCI_CHECK_PTR(lg, FALSE);
    OCI_CHECK(lg->hstate == OCI_OBJECT_FETCHED_CLEAN, FALSE);

    OCI_FREE(lg->buffer);
    OCI_FREE(lg);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_LongGetType
 * ------------------------------------------------------------------------ */

int OCI_API OCI_LongGetType(OCI_Long *lg)
{
    OCI_CHECK_PTR(lg, OCI_UNKNOWN);

    return lg->type;
}

/* ------------------------------------------------------------------------ *
 * OCI_LongRead
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_LongRead(OCI_Long *lg, void *buffer,
                                  unsigned int len)
{
    OCI_CHECK_PTR(buffer, 0);
    OCI_CHECK_PTR(lg, 0);
    OCI_CHECK_NUM(len, 0);
    OCI_CHECK_BOOL(lg->offset < lg->size, 0);

    if (lg->type == OCI_CLONG)
        len *= sizeof(dtext);

   /* check buffer size to read */

   if ((len + lg->offset) > lg->size)
        len = lg->size - lg->offset;

   /* copy buffer */

    memcpy(buffer, lg->buffer + lg->offset, len);

    lg->offset += len;

    if (lg->type == OCI_CLONG)
        len /= sizeof(dtext);

    return len;
}

/* ------------------------------------------------------------------------ *
 * OCI_LongWrite
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_LongWrite(OCI_Long *lg, void *buffer,
                                   unsigned int len)
{
    ub4 type, iter, dx, count = len;
    ub1 in_out, piece;
    void *handle;
    void *obuf = NULL;

    OCI_CHECK_PTR(buffer, 0);
    OCI_CHECK_PTR(lg, 0);
    OCI_CHECK_NUM(len, 0);

    if (lg->stmt->con->status != OCI_ERROR)
    {
        if (lg->type == OCI_CLONG)
            len *= sizeof(odtext);

        if (lg->type == OCI_CLONG)
            obuf = OCI_GetInputDataString(buffer, (int *) &len);
        else
            obuf = buffer;

       /* get piece info */

        lg->stmt->con->status = OCIStmtGetPieceInfo(lg->stmt->stmt,
                                                    lg->stmt->con->err,
                                                    &handle, &type,
                                                    &in_out, &iter,
                                                    &dx, &piece);

        /* set up piece type */

        if (len > 0)
            piece = (ub1) ((lg->size > 0) ? OCI_NEXT_PIECE : OCI_FIRST_PIECE);
        else
            piece = (ub1) OCI_LAST_PIECE;

        /* correct size to read for last piece */

        if ((lg->size + len) >= lg->stmt->long_size)
        {
            piece = OCI_LAST_PIECE;
            count = lg->stmt->long_size - lg->size;
        }

        /* set up info for writing */

        OCI_CALL(lg->stmt->con, OCIStmtSetPieceInfo(handle, type,
                                                    lg->stmt->con->err,
                                                    obuf, &count,  piece,
                                                    NULL, NULL))

        /* perform write call */

        OCI_CALL(lg->stmt->con, OCIStmtExecute(lg->stmt->con->cxt,
                                               lg->stmt->stmt,
                                               lg->stmt->con->err, 1, 0,
                                               NULL, NULL, 0))

        /* update size */

        lg->size += count;

        if (lg->type == OCI_CLONG)
            OCI_ReleaseDataString(obuf);

        if (lg->stmt->con->status != OCI_SUCCESS &&
            lg->stmt->con->status != OCI_NEED_DATA)
        {
            OCI_ErrorAPI(lg->stmt->con->err, lg->stmt->con, lg->stmt);
        }

        if (lg->type == OCI_CLONG)
            count /= sizeof(odtext);

        return count;
    }

    return 0;
}

/* ------------------------------------------------------------------------ *
 * OCI_LongGetSize
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_LongGetSize(OCI_Long *lg)
{
    OCI_CHECK_PTR(lg, 0);

    if (lg->type == OCI_CLONG)
        return lg->size/sizeof(dtext);
    else
        return lg->size;
}

/* ------------------------------------------------------------------------ *
 * OCI_LongGetBuffer
 * ------------------------------------------------------------------------ */

void * OCI_API OCI_LongGetBuffer(OCI_Long *lg)
{
    OCI_CHECK_PTR(lg, NULL);

    return (void *) lg->buffer;
}

/* ------------------------------------------------------------------------ *
 *                         OCI_Date functions
 * ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ *
 * OCI_DateInit
 * ------------------------------------------------------------------------ */

OCI_Date * OCI_DateInit(OCI_Connection *con,  OCI_Date **pdate,
                        OCIDate *buffer, boolean alloc, boolean is_ansi_date)
{
    OCI_Date *date = NULL;
    size_t msize   = 0;

    OCI_CHECK_PTR(pdate, NULL);

    if((*pdate) == NULL)
        OCI_ALLOC(OCI_Date *, *pdate, msize, sizeof(OCI_Date));

    date = *pdate;

    if (date != NULL)
    {
        date->con = con;

        if (con != NULL)
            date->err = con->err;
        else
            date->err = OCILib.err;

        if (alloc == TRUE || is_ansi_date == TRUE)
        {
            date->alloc = TRUE;

            if (alloc == TRUE)
                date->hstate = OCI_OBJECT_ALLOCATED;

            OCI_ALLOC(OCIDate *, date->value, msize, sizeof(OCIDate));

            if (date->value == NULL)
            {
                OCI_ErrorMemory(con, NULL, msize);
                OCI_FREE(date);
            }
        }
        else
        {
            date->hstate = OCI_OBJECT_FETCHED_CLEAN;
            date->value  = buffer;
        }

        if (is_ansi_date == TRUE && buffer)
        {
            unsigned char *d = (unsigned char *) buffer;
            
            date->value->OCIDateYYYY = (sb2) (((d[0] - 100) * 100) + (d[1] - 100));
            date->value->OCIDateMM   = (ub1) d[2];
            date->value->OCIDateDD   = (ub1) d[3];

            date->value->OCIDateTime.OCITimeHH = (ub1) (d[4] - 1);
            date->value->OCIDateTime.OCITimeMI = (ub1) (d[5] - 1);
            date->value->OCIDateTime.OCITimeSS = (ub1) (d[6] - 1);
        }
    }
    else
        OCI_ErrorMemory(con, NULL, msize);


    return date;
}

/* ------------------------------------------------------------------------ *
 * OCI_DateCreate
 * ------------------------------------------------------------------------ */

OCI_Date * OCI_API OCI_DateCreate(OCI_Connection *con)
{
    OCI_Date *date = NULL;

    return OCI_DateInit(con, &date, NULL, TRUE, FALSE);
}

/* ------------------------------------------------------------------------ *
 * OCI_DateFree
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DateFree(OCI_Date *date)
{
    OCI_CHECK_PTR(date, FALSE);
    OCI_CHECK(date->hstate == OCI_OBJECT_FETCHED_CLEAN, FALSE);

    if (date->alloc == TRUE)
        OCI_FREE(date->value);

    OCI_FREE(date);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_DateAddDays
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DateAddDays(OCI_Date *date, sb4 nb)
{
    OCI_CHECK_PTR(date, FALSE);

    OCI_CALL(date, OCIDateAddDays(date->err, date->value, nb, date->value))

    OCI_CHECK_ERROR2(date, date->con, NULL, FALSE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_DateAddMonths
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DateAddMonths(OCI_Date *date, int nb)
{
    OCI_CHECK_PTR(date, FALSE);

    OCI_CALL(date, OCIDateAddMonths(date->err, date->value, (sb4) nb, date->value))

    OCI_CHECK_ERROR2(date, date->con, NULL, FALSE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_DateAssign
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DateAssign(OCI_Date *date, OCI_Date *date_src)
{
    OCI_CHECK_PTR(date,     FALSE);
    OCI_CHECK_PTR(date_src, FALSE);

    OCI_CALL(date, OCIDateAssign(date->err, date_src->value, date->value))

    OCI_CHECK_ERROR2(date, date->con, NULL, FALSE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_DateCheck
 * ------------------------------------------------------------------------ */

int OCI_API OCI_DateCheck(OCI_Date *date)
{
    uword res = 0;

    OCI_CHECK_PTR(date, OCI_ERROR);

    OCI_CALL(date, OCIDateCheck(date->err, date->value, &res))

    OCI_CHECK_ERROR2(date, date->con, NULL, OCI_ERROR);

    return (int) res;
}

/* ------------------------------------------------------------------------ *
 * OCI_DateCompare
 * ------------------------------------------------------------------------ */

int OCI_API OCI_DateCompare(OCI_Date *date, OCI_Date *date2)
{
    sword res = -1;

    OCI_CHECK_PTR(date, -1);

    OCI_CALL(date, OCIDateCompare(date->err, date->value, date2->value, &res))

    OCI_CHECK_ERROR2(date, date->con, NULL, -1);

    return (int) res;
}

/* ------------------------------------------------------------------------ *
 * OCI_DateDaysBetween
 * ------------------------------------------------------------------------ */

int OCI_API OCI_DateDaysBetween(OCI_Date *date, OCI_Date *date2)
{
    sb4 nb = 0;

    OCI_CHECK_PTR(date,  OCI_ERROR);
    OCI_CHECK_PTR(date2, OCI_ERROR);

    OCI_CALL(date, OCIDateDaysBetween(date->err, date->value, date2->value, &nb))

    OCI_CHECK_ERROR2(date, date->con, NULL, OCI_ERROR);

    return nb;
}

/* ------------------------------------------------------------------------ *
 * OCI_DateFromText
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DateFromText(OCI_Date *date, const mtext *str, 
                                 const mtext *fmt)
{
    void *ostr1 = NULL;
    void *ostr2 = NULL;
    int  osize1 = -1;
    int  osize2 = -1;

    OCI_CHECK_PTR(date, FALSE);
    OCI_CHECK_PTR(str,  FALSE);
    OCI_CHECK_PTR(fmt,  FALSE);

    ostr1 = OCI_GetInputMetaString(str, &osize1);
    ostr2 = OCI_GetInputMetaString(fmt, &osize2);

    OCI_CALL(date, OCIDateFromText(date->err,
                                   (CONST text *) ostr1, (ub4) osize1,
                                   (CONST text *) ostr2, (ub1) osize2,
                                   NULL, 0, date->value))

    OCI_ReleaseMetaString(ostr1);
    OCI_ReleaseMetaString(ostr2);

    OCI_CHECK_ERROR2(date, date->con, NULL, FALSE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_DateGetDate
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DateGetDate(OCI_Date *date, int *year, int *month, int *day)
{
    OCI_CHECK_PTR(date, FALSE);

    OCIDateGetDate(date->value, (sb2 *) year, (ub1 *) month, (ub1 *) day);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_DateGetTime
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DateGetTime(OCI_Date *date, int *hour, int *min, int *sec)
{
    OCI_CHECK_PTR(date, FALSE);

    OCIDateGetTime(date->value, (ub1 *) hour, (ub1 *) min, (ub1 *) sec);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_DateGetDateTime
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DateGetDateTime(OCI_Date *date, int *year, int *month, 
                                    int *day, int *hour, int *min, int *sec)
{
    OCI_CHECK_PTR(date, FALSE);

    OCI_DateGetDate(date, year, month, day);
    OCI_DateGetTime(date, hour, min, sec);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_DateLastDay
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DateLastDay(OCI_Date *date)
{
    OCI_CHECK_PTR(date, FALSE);

    OCI_CALL(date, OCIDateLastDay(date->err, date->value,
                                       date->value))

    OCI_CHECK_ERROR2(date, date->con, NULL, FALSE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_DateNextDay
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DateNextDay(OCI_Date *date, const mtext *day)
{
    OCI_CHECK_PTR(date, FALSE);
    OCI_CHECK_PTR(day,  FALSE);

    OCI_CALL(date, OCIDateNextDay(date->err, date->value,
                                       (CONST text *) day,
                                       (ub4) (day ? mtextsize(day) : 0),
                                       date->value))

    OCI_CHECK_ERROR2(date, date->con, NULL, FALSE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_DateSetDate
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DateSetDate(OCI_Date *date, int year, int month, int day)
{
    OCI_CHECK_PTR(date, FALSE);

    OCIDateSetDate(date->value, (sb2) year, (ub1) month, (ub1) day);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_DateSetTime
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DateSetTime(OCI_Date *date, int hour, int min, int sec)
{
    OCI_CHECK_PTR(date, FALSE);

    OCIDateSetTime(date->value, (ub1) hour, (ub1) min, (ub1) sec);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_DateSetDateTime
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DateSetDateTime(OCI_Date *date, int year, int month, 
                                    int day, int hour, int min, int sec)
{
    OCI_CHECK_PTR(date, FALSE);

    OCI_DateSetDate(date, year, month, day);
    OCI_DateSetTime(date, hour, min, sec);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_DateSysDate
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DateSysDate(OCI_Date *date)
{
    OCI_CHECK_PTR(date, FALSE);

    OCI_CALL(date, OCIDateSysDate(date->err, date->value))

    OCI_CHECK_ERROR2(date, date->con, NULL, FALSE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_DateToText
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DateToText(OCI_Date *date, const mtext *fmt, int size, 
                               mtext *str)
{
    void *ostr1 = NULL;
    void *ostr2 = NULL;
    int  osize1 = size*sizeof(mtext);
    int  osize2 = -1;

    OCI_CHECK_PTR(date, FALSE);
    OCI_CHECK_PTR(str,  FALSE);
    OCI_CHECK_PTR(fmt,  FALSE);

    /* init output buffer in case of OCI failure */
 
    str[0] = 0;

    ostr1 = OCI_GetInputMetaString(str, &osize1);
    ostr2 = OCI_GetInputMetaString(fmt, &osize2);

    OCI_CALL(date, OCIDateToText(date->err, date->value,
                                 (CONST text *) ostr2, (ub1) osize2,
                                 (CONST text *) NULL, 0,
                                 (ub4*) &osize1, (text *) ostr1))

    OCI_GetOutputMetaString(ostr1, str, &osize1);

    OCI_ReleaseMetaString(ostr1);
    OCI_ReleaseMetaString(ostr2);

    /* set null string terminator*/

    str[osize1/sizeof(mtext)] = 0;

    OCI_CHECK_ERROR2(date, date->con, NULL, FALSE);

    return (osize1 > 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_DateZoneToZone
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DateZoneToZone(OCI_Date *date, const mtext *zone1, 
                                   const mtext *zone2)
{
    void *ostr1 = NULL;
    void *ostr2 = NULL;
    int osize1  = -1;
    int osize2  = -1;

    OCI_CHECK_PTR(date,  FALSE);
    OCI_CHECK_PTR(zone1, FALSE);
    OCI_CHECK_PTR(zone2, FALSE);

    ostr1 = OCI_GetInputMetaString(zone1, &osize1);
    ostr2 = OCI_GetInputMetaString(zone2, &osize2);
   
    OCI_CALL(date, OCIDateZoneToZone(date->err,
                                     date->value,
                                     (CONST text *) ostr1, (ub4) osize1,
                                     (CONST text *) ostr2, (ub4) osize2,
                                     date->value))

    OCI_ReleaseMetaString(ostr1);
    OCI_ReleaseMetaString(ostr2);

    OCI_CHECK_ERROR2(date, date->con, NULL, FALSE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_DateToCTime
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DateToCTime(OCI_Date *date, struct tm *ptm, time_t *pt)
{
    time_t time = -1;
    struct tm t;

    OCI_CHECK_PTR(date, FALSE);
    OCI_CHECK_PTR(ptm,  FALSE);

    t.tm_year  = date->value->OCIDateYYYY - 1900;
    t.tm_mon   = date->value->OCIDateMM - 1;
    t.tm_mday  = date->value->OCIDateDD;

    t.tm_hour  = date->value->OCIDateTime.OCITimeHH;
    t.tm_min   = date->value->OCIDateTime.OCITimeMI;
    t.tm_sec   = date->value->OCIDateTime.OCITimeSS;

    t.tm_wday  = 0;
    t.tm_yday  = 0;
    t.tm_isdst = -1;

    time = mktime(&t);

    if (ptm != NULL)
        memcpy(ptm, &t, sizeof(t));

    if (pt != NULL)
        *pt = time;

    return (time != -1);
}

/* ------------------------------------------------------------------------ *
 * OCI_DateFromCTime
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DateFromCTime(OCI_Date *date, struct tm *ptm, time_t t)
{
    OCI_CHECK_PTR(date, FALSE);

    if (ptm == NULL && t == 0)
        return FALSE;

    if (ptm == NULL)
        ptm = localtime(&t);

    date->value->OCIDateYYYY = (sb2) ptm->tm_year + 1900;
    date->value->OCIDateMM   = (ub1) ptm->tm_mon  + 1;
    date->value->OCIDateDD   = (ub1) ptm->tm_mday;

    date->value->OCIDateTime.OCITimeHH = (ub1) ptm->tm_hour;
    date->value->OCIDateTime.OCITimeMI = (ub1) ptm->tm_min;
    date->value->OCIDateTime.OCITimeSS = (ub1) ptm->tm_sec;

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 *                         OCI_Timestamp functions
 * ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ *
 * OCI_TimestampInit
 * ------------------------------------------------------------------------ */

OCI_Timestamp * OCI_TimestampInit(OCI_Connection *con, OCI_Timestamp **ptmsp,
                                      OCIDateTime *buffer, ub1 type)
{
    OCI_Timestamp *tmsp = NULL;
    size_t msize        = 0;

    OCI_CHECK_PTR(ptmsp, NULL);
    OCI_CHECK_VERSION(con, OCI_9, NULL);

#if OCI_VERSION_COMPILE >= OCI_9

    if((*ptmsp) == NULL)
        OCI_ALLOC(OCI_Timestamp *, *ptmsp, msize, sizeof(OCI_Timestamp));

    tmsp = *ptmsp;

    if (tmsp)
    {
        tmsp->con    = con;
        tmsp->handle = buffer;
        tmsp->type   = type;

        if (con != NULL)
            tmsp->err = con->err;
        else
            tmsp->err = OCILib.err;

    }
    else
        OCI_ErrorMemory(con, NULL, msize);

    if (tmsp != NULL && tmsp->handle == NULL)
    {
        ub4 htype = 0;

        tmsp->hstate = OCI_OBJECT_ALLOCATED;

        if (tmsp->type == OCI_TIMESTAMP)
            htype = OCI_DTYPE_TIMESTAMP;
        else if (tmsp->type == OCI_TIMESTAMP_TZ)
            htype = OCI_DTYPE_TIMESTAMP_TZ;
        else if (tmsp->type == OCI_TIMESTAMP_LTZ)
            htype = OCI_DTYPE_TIMESTAMP_LTZ;

        OCI_CALL(tmsp,
                 OCIDescriptorAlloc(OCILib.env,
                                    (dvoid **) (void *) &tmsp->handle,
                                    htype, 0, (dvoid **) NULL))

        if (tmsp->status != OCI_SUCCESS)
        {
            OCI_ErrorAPI(tmsp->err, con, NULL);
            OCI_FREE(tmsp);
        }
    }
    else
        tmsp->hstate = OCI_OBJECT_FETCHED_CLEAN;

#else

    OCI_NOT_USED(type);
    OCI_NOT_USED(buffer);
    OCI_NOT_USED(msize);

#endif

    return tmsp;
}

/* ------------------------------------------------------------------------ *
 * OCI_TimestampCreate
 * ------------------------------------------------------------------------ */

OCI_Timestamp * OCI_API OCI_TimestampCreate(OCI_Connection *con, int type)
{
    OCI_Timestamp *tmsp = NULL;

    OCI_CHECK_VERSION(con, OCI_9, NULL);

#if OCI_VERSION_COMPILE >= OCI_9

    tmsp = OCI_TimestampInit(con, &tmsp, NULL, (ub1) type);

#else

    OCI_NOT_USED(type);

#endif

    return tmsp;
}

/* ------------------------------------------------------------------------ *
 * OCI_TimestampFree
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TimestampFree(OCI_Timestamp *tmsp)
{
    boolean res = FALSE;

    OCI_CHECK_PTR(tmsp, FALSE);
    OCI_CHECK_VERSION(tmsp->con, OCI_9, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    OCI_CHECK(tmsp->hstate == OCI_OBJECT_FETCHED_CLEAN, FALSE);

    if (tmsp->hstate == OCI_OBJECT_ALLOCATED)
    {   
        ub4 htype  = 0;

        if (tmsp->type == OCI_TIMESTAMP)
            htype = OCI_DTYPE_TIMESTAMP;
        else if (tmsp->type == OCI_TIMESTAMP_TZ)
            htype = OCI_DTYPE_TIMESTAMP_TZ;
        else if (tmsp->type == OCI_TIMESTAMP_LTZ)
            htype = OCI_DTYPE_TIMESTAMP_LTZ;

        OCI_DIRECT(tmsp, OCIDescriptorFree(tmsp->handle, htype))
    }

    OCI_FREE(tmsp);

    res = TRUE;

#endif

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_TimestampGetType
 * ------------------------------------------------------------------------ */

int OCI_API OCI_TimestampGetType(OCI_Timestamp *tmsp)
{
    OCI_CHECK_PTR(tmsp, OCI_UNKNOWN);

    return tmsp->type;
}

/* ------------------------------------------------------------------------ *
 * OCI_DateZoneToZone
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TimestampAssign(OCI_Timestamp *tmsp, OCI_Timestamp *tmsp_src)
{
    boolean res = FALSE;

    OCI_CHECK_PTR(tmsp,     FALSE);
    OCI_CHECK_PTR(tmsp_src, FALSE);

    OCI_CHECK_VERSION(tmsp->con, OCI_9, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    OCI_CALL(tmsp, OCIDateTimeAssign(OCILib.env, tmsp->err,
                                     tmsp_src->handle, tmsp->handle))

    OCI_CHECK_ERROR2(tmsp, tmsp->con, NULL, FALSE);

    res = TRUE;

#endif

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_TimestampCheck
 * ------------------------------------------------------------------------ */

int OCI_API OCI_TimestampCheck(OCI_Timestamp *tmsp)
{
    ub4 res = (ub4) OCI_ERROR;

    OCI_CHECK_PTR(tmsp, OCI_ERROR);
    OCI_CHECK_VERSION(tmsp->con, OCI_9, OCI_ERROR);

#if OCI_VERSION_COMPILE >= OCI_9

    OCI_CALL(tmsp, OCIDateTimeCheck(OCILib.env, tmsp->err,
                                    tmsp->handle, &res))

    OCI_CHECK_ERROR2(tmsp, tmsp->con, NULL, OCI_ERROR);

#endif

    return (int) res;
}

/* ------------------------------------------------------------------------ *
 * OCI_TimestampCompare
 * ------------------------------------------------------------------------ */

int OCI_API OCI_TimestampCompare(OCI_Timestamp *tmsp, OCI_Timestamp *tmsp2)
{
    sword res = OCI_ERROR;

    OCI_CHECK_PTR(tmsp,  res);
    OCI_CHECK_PTR(tmsp2, res);
    OCI_CHECK_VERSION(tmsp->con, OCI_9, res);

#if OCI_VERSION_COMPILE >= OCI_9

    OCI_CALL(tmsp,
             OCIDateTimeCompare(OCILib.env, tmsp->err,
                                tmsp2->handle, tmsp2->handle, &res))

    OCI_CHECK_ERROR2(tmsp, tmsp->con, NULL, OCI_ERROR);

#endif

    return (int) res;
}

/* ------------------------------------------------------------------------ *
 * OCI_TimestampConstruct
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TimestampConstruct(OCI_Timestamp *tmsp, int year,int month,
                                      int day, int hour,  int min, int sec,
                                      int fsec, const mtext *timezone)
{
    boolean res = FALSE;

    OCI_CHECK_PTR(tmsp, FALSE);
    OCI_CHECK_VERSION(tmsp->con, OCI_9, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    OCI_CALL(tmsp,
             OCIDateTimeConstruct(OCILib.env, tmsp->err,
                                  tmsp->handle,
                                  (sb2) year, (ub1) month, (ub1) day,
                                  (ub1) hour, (ub1) min,(ub1) sec, (ub4) fsec,
                                  (OraText *) timezone,
                                  (size_t) (timezone ? mtextsize(timezone) : 0)))

    OCI_CHECK_ERROR2(tmsp, tmsp->con, NULL, FALSE);

    res = TRUE;

#else

    OCI_NOT_USED(year);
    OCI_NOT_USED(month);
    OCI_NOT_USED(day);
    OCI_NOT_USED(hour);
    OCI_NOT_USED(min);
    OCI_NOT_USED(sec);
    OCI_NOT_USED(fsec);
    OCI_NOT_USED(timezone);

#endif

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_TimestampConvert
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TimestampConvert(OCI_Timestamp *tmsp, OCI_Timestamp *tmsp2)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(tmsp,  FALSE);
    OCI_CHECK_PTR(tmsp2, FALSE);
    OCI_CHECK_VERSION(tmsp->con, OCI_9, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    OCI_CALL(tmsp,
             OCIDateTimeConvert(OCILib.env, tmsp->err,
                                tmsp->handle, tmsp2->handle))

    OCI_CHECK_ERROR2(tmsp, tmsp->con, NULL, FALSE);

    res = TRUE;

#endif

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_TimestampFromText
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TimestampFromText(OCI_Timestamp *tmsp, const mtext *str, 
                                      const mtext *fmt)
{
    boolean res = FALSE;
    void *ostr1 = NULL;
    void *ostr2 = NULL;
    int  osize1 = -1;
    int  osize2 = -1;

    OCI_CHECK_PTR(tmsp, FALSE);
    OCI_CHECK_PTR(str,  FALSE);
    OCI_CHECK_PTR(fmt,  FALSE);

    OCI_CHECK_VERSION(tmsp->con, OCI_9, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    ostr1 = OCI_GetInputMetaString(str, &osize1);
    ostr2 = OCI_GetInputMetaString(fmt, &osize2);

    OCI_CALL(tmsp, OCIDateTimeFromText(OCILib.env,
                                       tmsp->err,
                                       (CONST OraText *) ostr1, (ub4) osize1,
                                       (CONST OraText *) ostr2, (ub1) osize2,
                                       NULL, 0, tmsp->handle))

    OCI_ReleaseMetaString(ostr1);
    OCI_ReleaseMetaString(ostr2);

    OCI_CHECK_ERROR2(tmsp, tmsp->con, NULL, FALSE);

    res = TRUE;

#else

    OCI_NOT_USED(ostr1);
    OCI_NOT_USED(ostr2);
    OCI_NOT_USED(osize1);
    OCI_NOT_USED(osize2);

#endif

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_TimestampToText
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TimestampToText(OCI_Timestamp *tmsp, const mtext *fmt, 
                                    int size, mtext *str, int precision)
{
    boolean res = FALSE;
    void *ostr1 = NULL;
    void *ostr2 = NULL;
    int  osize1 = size*sizeof(mtext);
    int  osize2 = -1;

    OCI_CHECK_PTR(tmsp, FALSE);
    OCI_CHECK_PTR(str,  FALSE);
    OCI_CHECK_PTR(fmt,  FALSE);

    /* init output buffer in case of OCI failure */
 
    str[0] = 0;

    OCI_CHECK_VERSION(tmsp->con, OCI_9, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    ostr1 = OCI_GetInputMetaString(str, &osize1);
    ostr2 = OCI_GetInputMetaString(fmt, &osize2);

    OCI_CALL(tmsp, OCIDateTimeToText(OCILib.env,
                                     tmsp->err,
                                     tmsp->handle,
                                     (CONST OraText *) ostr2,
                                     (ub1) osize2,
                                     (ub1) precision,
                                     (CONST OraText *) NULL, 0,
                                     (ub4*) &osize1,
                                     (OraText *) ostr1))

    OCI_GetOutputMetaString(ostr1, str, &osize1);

    OCI_ReleaseMetaString(ostr1);
    OCI_ReleaseMetaString(ostr2);

    /* set null string terminator */

    str[osize1/sizeof(mtext)] = 0;

    OCI_CHECK_ERROR2(tmsp, tmsp->con, NULL, FALSE);

    res = (osize1 > 0);

#else

    OCI_NOT_USED(ostr1);
    OCI_NOT_USED(ostr2);
    OCI_NOT_USED(osize1);
    OCI_NOT_USED(osize2);
    OCI_NOT_USED(precision);

#endif

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_TimestampGetDate
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TimestampGetDate(OCI_Timestamp *tmsp, int *year, int *month,
                                    int *day)
{
    boolean res = FALSE;

    OCI_CHECK_PTR(tmsp, FALSE);
    OCI_CHECK_VERSION(tmsp->con, OCI_9, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    OCI_CALL(tmsp, OCIDateTimeGetDate(OCILib.env,
                                      tmsp->err,
                                      tmsp->handle,
                                      (sb2*) year,
                                      (ub1*) month,
                                      (ub1*) day))

    OCI_CHECK_ERROR2(tmsp, tmsp->con, NULL, FALSE);

    res = TRUE;

#else

    OCI_NOT_USED(year);
    OCI_NOT_USED(month);
    OCI_NOT_USED(day);

#endif

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_TimestampGetTime
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TimestampGetTime(OCI_Timestamp *tmsp, int *hour, int *min,
                                    int *sec, int *fsec)
{
    boolean res = FALSE;

    OCI_CHECK_PTR(tmsp, FALSE);
    OCI_CHECK_VERSION(tmsp->con, OCI_9, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    OCI_CALL(tmsp, OCIDateTimeGetTime(OCILib.env,
                                      tmsp->err,
                                      tmsp->handle,
                                      (ub1*) hour,
                                      (ub1*) min,
                                      (ub1*) sec,
                                      (ub4*) fsec))

    OCI_CHECK_ERROR2(tmsp, tmsp->con, NULL, FALSE);

    res = TRUE;

#else

    OCI_NOT_USED(hour);
    OCI_NOT_USED(min);
    OCI_NOT_USED(sec);
    OCI_NOT_USED(fsec);

#endif

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_TimestampGetTimeZoneName
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TimestampGetTimeZoneName(OCI_Timestamp *tmsp, int size,
                                             mtext *str)
{
    boolean res = FALSE;
    void *ostr  = NULL;
    int osize   = size*sizeof(mtext);

    OCI_CHECK_PTR(tmsp, FALSE);
    OCI_CHECK_PTR(str, FALSE);
    OCI_CHECK_VERSION(tmsp->con, OCI_9, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    ostr = OCI_GetInputMetaString(str, &osize);

    OCI_CALL(tmsp,
             OCIDateTimeGetTimeZoneName(OCILib.env,
                                        tmsp->err,
                                        tmsp->handle,
                                        (ub1*) ostr,
                                        (ub4*)&osize))

    OCI_GetOutputMetaString(ostr, str, &osize);

    OCI_ReleaseMetaString(ostr);

    /* set null string terminator */

    str[osize/sizeof(mtext)] = 0;

    OCI_CHECK_ERROR2(tmsp, tmsp->con, NULL, FALSE);

    res = (osize > 0);

#else

    OCI_NOT_USED(str);
    OCI_NOT_USED(size);
    OCI_NOT_USED(ostr);
    OCI_NOT_USED(osize);

#endif

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_TimestampGetTimeZoneOffset
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TimestampGetTimeZoneOffset(OCI_Timestamp *tmsp,
                                              int *hour, int *min)
{
    boolean res = FALSE;

    OCI_CHECK_PTR(tmsp, FALSE);
    OCI_CHECK_VERSION(tmsp->con, OCI_9, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    OCI_CALL(tmsp,
             OCIDateTimeGetTimeZoneOffset(OCILib.env,
                                          tmsp->err,
                                          tmsp->handle,
                                          (sb1*) hour,
                                          (sb1*) min))

    OCI_CHECK_ERROR2(tmsp, tmsp->con, NULL, FALSE);

    res = TRUE;

#else

    OCI_NOT_USED(hour);
    OCI_NOT_USED(min);

#endif

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_TimestampIntervalAdd
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TimestampIntervalAdd(OCI_Timestamp *tmsp,
                                         OCI_Interval *itv)
{
    boolean res  = FALSE;
    ub1 old_type = OCI_TIMESTAMP_TZ;

    OCI_CHECK_PTR(tmsp, FALSE);
    OCI_CHECK_PTR(itv,  FALSE);
    OCI_CHECK_VERSION(tmsp->con, OCI_9, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    /* OCIDateTimeIntervalAdd()  fails if timestamps is not OCI_TIMESTAMP_TZ */

    if (tmsp->type != OCI_TIMESTAMP_TZ)    
    {
        OCI_Timestamp *tmp = OCI_TimestampCreate(tmsp->con, OCI_TIMESTAMP_TZ);

        old_type = tmsp->type;

        OCI_TimestampAssign(tmp, tmsp);
        OCI_TimestampConvert(tmp, tmsp);

        OCI_TimestampFree(tmp);
    }

    OCI_CALL(tmsp, OCIDateTimeIntervalAdd(OCILib.env,
                                          tmsp->err,
                                          tmsp->handle,
                                          itv->handle,
                                          tmsp->handle))

    OCI_CHECK_ERROR2(tmsp, tmsp->con, NULL, FALSE);

    /* restoring old timestamp type */

    if (old_type != OCI_TIMESTAMP_TZ)    
    {
        OCI_Timestamp *tmp = OCI_TimestampCreate(tmsp->con, old_type);

        OCI_TimestampAssign(tmp, tmsp);
        OCI_TimestampConvert(tmp, tmsp);

        OCI_TimestampFree(tmp);
    }

    res = TRUE;

#endif

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_TimestampIntervalSub
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TimestampIntervalSub(OCI_Timestamp *tmsp,
                                         OCI_Interval *itv)
{
    boolean res  = FALSE;
    ub1 old_type = OCI_TIMESTAMP_TZ;


    OCI_CHECK_PTR(tmsp, FALSE);
    OCI_CHECK_PTR(itv,  FALSE);
    OCI_CHECK_VERSION(tmsp->con, OCI_9, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    /* OCIDateTimeIntervalSub() fails if timestamps is not OCI_TIMESTAMP_TZ */

    if (tmsp->type != OCI_TIMESTAMP_TZ)    
    {
        OCI_Timestamp *tmp = OCI_TimestampCreate(tmsp->con, OCI_TIMESTAMP_TZ);

        old_type = tmsp->type;

        OCI_TimestampConvert(tmp, tmsp);
        OCI_TimestampAssign(tmp, tmsp);

        OCI_TimestampFree(tmp);
    }

    OCI_CALL(tmsp, OCIDateTimeIntervalSub(OCILib.env,
                                          tmsp->err,
                                          tmsp->handle,
                                          itv->handle,
                                          tmsp->handle))

    OCI_CHECK_ERROR2(tmsp, tmsp->con, NULL, FALSE);

    /* restoring old timestamp type */

    if (old_type != OCI_TIMESTAMP_TZ)    
    {
        OCI_Timestamp *tmp = OCI_TimestampCreate(tmsp->con, old_type);

        OCI_TimestampConvert(tmp, tmsp);
        OCI_TimestampAssign(tmp, tmsp);

        OCI_TimestampFree(tmp);
    }

    res = TRUE;

#endif

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_TimestampSubtract
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TimestampSubtract(OCI_Timestamp *tmsp, OCI_Timestamp *tmsp2,
                                     OCI_Interval *itv)
{
    OCI_CHECK_PTR(tmsp,  FALSE);
    OCI_CHECK_PTR(tmsp2, FALSE);
    OCI_CHECK_PTR(itv, FALSE);
    OCI_CHECK_VERSION(tmsp->con, OCI_9, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    OCI_CALL(tmsp, OCIDateTimeSubtract(OCILib.env,
                                        tmsp->err,
                                        tmsp->handle,
                                        tmsp2->handle,
                                        itv->handle))

    OCI_CHECK_ERROR2(tmsp, tmsp->con, NULL, FALSE);

#endif

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_TimestampSysTimeStamp
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TimestampSysTimeStamp(OCI_Timestamp *tmsp)
{
    boolean res = FALSE;

    OCI_CHECK_PTR(tmsp, FALSE);
    OCI_CHECK_VERSION(tmsp->con, OCI_9, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    OCI_CALL(tmsp, OCIDateTimeSysTimeStamp(OCILib.env,
                                           tmsp->err,
                                           tmsp->handle))

    OCI_CHECK_ERROR2(tmsp, tmsp->con, NULL, FALSE);

    res = TRUE;

#endif

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_TimestampToCTime
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TimestampToCTime(OCI_Timestamp *tmsp, struct tm *ptm,
                                     time_t *pt)
{
    boolean res = TRUE;
    time_t time = -1;
    int msec    = 0;
    struct tm t;

    OCI_CHECK_PTR(tmsp, FALSE);
    OCI_CHECK_PTR(ptm,  FALSE);

    res = OCI_TimestampGetDate(tmsp, &t.tm_year, &t.tm_mon, &t.tm_mday) &&
          OCI_TimestampGetTime(tmsp, &t.tm_hour, &t.tm_min, &t.tm_sec, &msec);

    if (res == TRUE)
    {
        t.tm_wday  = 0;
        t.tm_yday  = 0;
        t.tm_isdst = -1;

        time = mktime(&t);

        if (ptm != NULL)
            memcpy(ptm, &t, sizeof(t));

        if (pt != NULL)
            *pt = time;
    }

    return (time != -1);
}

/* ------------------------------------------------------------------------ *
 * OCI_TimestampFromCTime
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TimestampFromCTime(OCI_Timestamp *tmsp, struct tm *ptm,
                                       time_t t)
{
    OCI_CHECK_PTR(tmsp, FALSE);

    if (ptm == NULL && t == 0)
        return FALSE;

    if (ptm == NULL)
        ptm = localtime(&t);

    return OCI_TimestampConstruct(tmsp, 
                                  ptm->tm_year + 1900,
                                  ptm->tm_mon  + 1,
                                  ptm->tm_mday,
                                  ptm->tm_hour,
                                  ptm->tm_min,
                                  ptm->tm_sec,
                                  0,NULL);
}

/* ------------------------------------------------------------------------ *
 *                         OCI_Interval functions
 * ------------------------------------------------------------------------ */

OCI_Interval * OCI_IntervalInit(OCI_Connection *con, OCI_Interval **pitv,
                                OCIInterval *buffer, ub1 type)
{
    OCI_Interval *itv = NULL;
    size_t msize      = 0;

    OCI_CHECK_PTR(pitv, NULL);
    OCI_CHECK_VERSION(con, OCI_9, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    if((*pitv) == NULL)
        OCI_ALLOC(OCI_Interval *, *pitv, msize, sizeof(OCI_Interval));

    itv = *pitv;

    if (itv)
    {
        itv->con    = con;
        itv->handle = buffer;
        itv->type   = type;

        if (con != NULL)
            itv->err = con->err;
        else
            itv->err = OCILib.err;
    }
    else
        OCI_ErrorMemory(con, NULL, msize);

    if (itv != NULL && itv->handle == NULL)
    {
        ub4 htype = 0;

        itv->hstate = OCI_OBJECT_ALLOCATED;

        if (itv->type == OCI_INTERVAL_YM)
            htype = OCI_DTYPE_INTERVAL_YM;
        else if (itv->type == OCI_INTERVAL_DS)
            htype = OCI_DTYPE_INTERVAL_DS;

        OCI_CALL(itv,
                 OCIDescriptorAlloc(OCILib.env,
                                    (dvoid **) (void *) &itv->handle,
                                    htype, 0, (dvoid **) NULL))

        if (itv->status != OCI_SUCCESS)
        {
            OCI_ErrorAPI(itv->err, con, NULL);
            OCI_FREE(itv);
        }
    }
    else
        itv->hstate = OCI_OBJECT_FETCHED_CLEAN;

#else

    OCI_NOT_USED(type);
    OCI_NOT_USED(buffer);
    OCI_NOT_USED(msize);

#endif

    return itv;
}

/* ------------------------------------------------------------------------ *
 * OCI_IntervalCreate
 * ------------------------------------------------------------------------ */

OCI_Interval * OCI_API OCI_IntervalCreate(OCI_Connection *con, int type)
{
    OCI_Interval *itv = NULL;

    OCI_CHECK_VERSION(con, OCI_9, NULL);

#if OCI_VERSION_COMPILE >= OCI_9

    itv = OCI_IntervalInit(con, &itv, NULL, (ub1) type);

#else

    OCI_NOT_USED(type);

#endif

    return itv;
}

/* ------------------------------------------------------------------------ *
 * OCI_IntervalFree
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_IntervalFree(OCI_Interval *itv)
{
    boolean res = FALSE;

    OCI_CHECK_PTR(itv, FALSE);
    OCI_CHECK_VERSION(itv->con, OCI_9, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    OCI_CHECK(itv->hstate == OCI_OBJECT_FETCHED_CLEAN, FALSE);

    if (itv->hstate == OCI_OBJECT_ALLOCATED)
    {
        ub4 htype = 0;

        if (itv->type == OCI_INTERVAL_YM)
            htype = OCI_DTYPE_INTERVAL_YM;
        else if (itv->type == OCI_INTERVAL_DS)
            htype = OCI_DTYPE_INTERVAL_DS;

        OCI_DIRECT(itv, OCIDescriptorFree(itv->handle, htype))
    }

    OCI_FREE(itv);

    res = TRUE;
   
#endif
   
   return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_IntervalGetType
 * ------------------------------------------------------------------------ */

int OCI_API OCI_IntervalGetType(OCI_Interval *itv)
{
    OCI_CHECK_PTR(itv, OCI_UNKNOWN);

    return itv->type;
}

/* ------------------------------------------------------------------------ *
 * OCI_IntervalAssign
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_IntervalAssign(OCI_Interval *itv,  OCI_Interval *itv_src)
{
    boolean res = FALSE;

    OCI_CHECK_PTR(itv,  FALSE);
    OCI_CHECK_PTR(itv_src, FALSE);
    OCI_CHECK_VERSION(itv->con, OCI_9, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    OCI_CALL(itv, OCIIntervalAssign(OCILib.env, itv->err,
                                    itv_src->handle, itv->handle))

    OCI_CHECK_ERROR2(itv, itv->con,  NULL, FALSE);

    res = TRUE;
   
#endif
   
   return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_IntervalCheck
 * ------------------------------------------------------------------------ */

int OCI_API OCI_IntervalCheck(OCI_Interval *itv)
{
    ub4 res = (ub4) OCI_ERROR;

    OCI_CHECK_PTR(itv, OCI_ERROR);
    OCI_CHECK_VERSION(itv->con, OCI_9, OCI_ERROR);

#if OCI_VERSION_COMPILE >= OCI_9

    OCI_CALL(itv, OCIIntervalCheck(OCILib.env, itv->err, itv->handle, &res))

    OCI_CHECK_ERROR2(itv, itv->con,  NULL, OCI_ERROR);

#endif

    return (int) res;

}

/* ------------------------------------------------------------------------ *
 * OCI_IntervalCompare
 * ----------------------------------------------------------------------- */

int OCI_API OCI_IntervalCompare(OCI_Interval *itv, OCI_Interval *itv2)
{
    sword res = OCI_ERROR;

    OCI_CHECK_PTR(itv,  res);
    OCI_CHECK_PTR(itv2, res);
    OCI_CHECK_VERSION(itv->con, OCI_9, res);

#if OCI_VERSION_COMPILE >= OCI_9

    OCI_CALL(itv,
             OCIIntervalCompare(OCILib.env, itv->err,
                                itv2->handle, itv2->handle, &res))

    OCI_CHECK_ERROR2(itv, itv->con,  NULL, OCI_ERROR);

#endif

    return (int) res;
}

/* ------------------------------------------------------------------------ *
 * OCI_IntervalFromText
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_IntervalFromText(OCI_Interval *itv, const mtext * str)
{
    boolean res = FALSE;
    void *ostr = NULL;
    int  osize = -1;

    OCI_CHECK_PTR(itv, FALSE);
    OCI_CHECK_VERSION(itv->con, OCI_9, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    ostr = OCI_GetInputMetaString(str, &osize);
 
    OCI_CALL(itv,
             OCIIntervalFromText(OCILib.env, itv->err,
                                 (CONST OraText *) ostr, osize,
                                 itv->handle))
    
    OCI_ReleaseMetaString(ostr);

    OCI_CHECK_ERROR2(itv, itv->con,  NULL, FALSE);

    res = TRUE;

#else

    OCI_NOT_USED(str);
    OCI_NOT_USED(ostr);
    OCI_NOT_USED(osize);    

#endif
   
   return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_IntervalToText
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_IntervalToText(OCI_Interval *itv, int leading_prec,
                                    int fraction_prec, int size, mtext * str)
{
    boolean res = FALSE;
    void *ostr  = NULL;
    int osize   = size*sizeof(mtext);

    OCI_CHECK_PTR(itv, FALSE);
    OCI_CHECK_PTR(str, FALSE);
 
    /* init output buffer in case of OCI failure */
 
    str[0] = 0;

    OCI_CHECK_VERSION(itv->con, OCI_9, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    ostr = OCI_GetInputMetaString(str, &osize);
 
    OCI_CALL(itv, OCIIntervalToText(OCILib.env,
                                    itv->err,
                                    itv->handle,
                                    (ub1) leading_prec,
                                    (ub1) fraction_prec,
                                    (OraText *) ostr, osize, 
                                    (size_t *) &osize))
 
    OCI_GetOutputMetaString(ostr, str, &osize);
    OCI_ReleaseMetaString(ostr);

    /* set null string terminator */

    str[osize/sizeof(mtext)] = 0;

    OCI_CHECK_ERROR2(itv, itv->con,  NULL, FALSE);

    res = (osize > 0);

#else

    OCI_NOT_USED(str);
    OCI_NOT_USED(ostr);
    OCI_NOT_USED(size);    
    OCI_NOT_USED(osize);    
    OCI_NOT_USED(leading_prec);    
    OCI_NOT_USED(fraction_prec);    

#endif
   
   return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_IntervalFromTimeZone
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_IntervalFromTimeZone(OCI_Interval *itv, const mtext * str)
{
    boolean res = FALSE;
    void *ostr  = NULL;
    int osize   = -1;

    OCI_CHECK_PTR(itv, FALSE);
    OCI_CHECK_PTR(str, FALSE);
    OCI_CHECK_VERSION(itv->con, OCI_9, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    ostr = OCI_GetInputMetaString(str, &osize);
  
    OCI_CALL(itv,
             OCIIntervalFromTZ(OCILib.env, itv->err,
                               (CONST OraText *) ostr, osize,
                               itv->handle))

    OCI_ReleaseMetaString(ostr);

    OCI_CHECK_ERROR2(itv, itv->con,  NULL, FALSE);

    res = TRUE;

#else

    OCI_NOT_USED(str);
    OCI_NOT_USED(ostr);
    OCI_NOT_USED(osize);    

#endif

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_IntervalGetDaySecond
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_IntervalGetDaySecond(OCI_Interval *itv, int *day, int *hour,
                                         int *min, int *sec, int *fsec)
{
    boolean res = FALSE;

    OCI_CHECK_PTR(itv, FALSE);
    OCI_CHECK_VERSION(itv->con, OCI_9, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    OCI_CALL(itv,
             OCIIntervalGetDaySecond(OCILib.env, itv->err,
                                     (sb4 *) day, (sb4 *) hour, (sb4 *) min,
                                     (sb4 *) sec, (sb4 *) fsec, itv->handle))

    OCI_CHECK_ERROR2(itv, itv->con,  NULL, FALSE);

    res = TRUE;
    
#else

    OCI_NOT_USED(day);
    OCI_NOT_USED(hour);
    OCI_NOT_USED(min);    
    OCI_NOT_USED(sec);    
    OCI_NOT_USED(fsec);    

#endif

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_IntervalGetYearMonth
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_IntervalGetYearMonth(OCI_Interval *itv, int *year, int *month)
{
    boolean res = FALSE;

    OCI_CHECK_PTR(itv, FALSE);
    OCI_CHECK_VERSION(itv->con, OCI_9, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    OCI_CALL(itv,
             OCIIntervalGetYearMonth(OCILib.env, itv->err,
                                     (sb4 *) year, (sb4 *) month, itv->handle))

    OCI_CHECK_ERROR2(itv, itv->con,  NULL, FALSE);

    res = TRUE;
    
#else

    OCI_NOT_USED(year);
    OCI_NOT_USED(month);

#endif

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_IntervalSetDaySecond
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_IntervalSetDaySecond(OCI_Interval *itv, int day,int hour,
                                         int min, int sec, int fsec)
{
    boolean res = FALSE;

    OCI_CHECK_PTR(itv, FALSE);
    OCI_CHECK_VERSION(itv->con, OCI_9, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    OCI_CALL(itv,
             OCIIntervalSetDaySecond(OCILib.env, itv->err,
                                     (sb4) day, (sb4) hour, (sb4) min,
                                     (sb4) sec, (sb4) fsec, itv->handle))

    OCI_CHECK_ERROR2(itv, itv->con,  NULL, FALSE);

    res = TRUE;
    
#else

    OCI_NOT_USED(day);
    OCI_NOT_USED(hour);
    OCI_NOT_USED(min);    
    OCI_NOT_USED(sec);    
    OCI_NOT_USED(fsec);    

#endif

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_IntervalSetYearMonth
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_IntervalSetYearMonth(OCI_Interval *itv, int year, int month)
{
    boolean res = FALSE;

    OCI_CHECK_PTR(itv, FALSE);
    OCI_CHECK_VERSION(itv->con, OCI_9, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    OCI_CALL(itv,
             OCIIntervalSetYearMonth(OCILib.env, itv->err,
                                     (sb4) year, (sb4) month, itv->handle))

    OCI_CHECK_ERROR2(itv, itv->con,  NULL, FALSE);

    res = TRUE;
    
#else

    OCI_NOT_USED(year);
    OCI_NOT_USED(month);

#endif

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_IntervalAdd
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_IntervalAdd(OCI_Interval *itv, OCI_Interval *itv2)
{
    boolean res = FALSE;

    OCI_CHECK_PTR(itv,  FALSE);
    OCI_CHECK_PTR(itv2, FALSE);
    OCI_CHECK_VERSION(itv->con, OCI_9, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    OCI_CALL(itv,
             OCIIntervalAdd(OCILib.env, itv->err,
                            itv->handle, itv2->handle, itv->handle))

    OCI_CHECK_ERROR2(itv, itv->con,  NULL, FALSE);

    res = TRUE;

#endif

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_IntervalSubtract
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_IntervalSubtract(OCI_Interval *itv, OCI_Interval *itv2)
{
    boolean res = FALSE;

    OCI_CHECK_PTR(itv,  FALSE);
    OCI_CHECK_PTR(itv2, FALSE);
    OCI_CHECK_VERSION(itv->con, OCI_9, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    OCI_CALL(itv, OCIIntervalSubtract(OCILib.env, itv->err,
                                      itv->handle, itv2->handle, itv->handle))

    OCI_CHECK_ERROR2(itv, itv->con,  NULL, FALSE);

    res = TRUE;

#endif

    return res;
}

/* ------------------------------------------------------------------------ *
 *                         OCI_Object functions
 * ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ *
 * OCI_ObjectInit
 * ------------------------------------------------------------------------ */

OCI_Object * OCI_ObjectInit(OCI_Connection *con, OCI_Object **pobj,
                            void *handle, OCI_Schema *schema)
{
    OCI_Object * obj = NULL;
    size_t msize     = 0;
    
    OCI_CHECK_PTR(con,    NULL);
    OCI_CHECK_PTR(pobj,   NULL);
    OCI_CHECK_PTR(schema, NULL);

    if((*pobj) == NULL)
    {
        OCI_ALLOC(OCI_Object *, *pobj, msize, sizeof(OCI_Object));
    }

    obj = *pobj;

    if (obj)
    {
        obj->con    = con;
        obj->handle = handle;
        obj->nty    = schema; 

        OCI_ALLOC(void **, obj->objs, msize,
                  sizeof(void*) * schema->nb_items);

        if (obj->objs == NULL)
        {
            OCI_ErrorMemory(con, NULL, msize);
            OCI_FREE(obj);            
            return NULL;
        }
    }
    else
        OCI_ErrorMemory(con, NULL, msize);

    if (obj != NULL && obj->handle == NULL)
    {
        /* allocates handle for non fetched object */

        obj->hstate = OCI_OBJECT_ALLOCATED;

        OCI_CALL(con,  OCIObjectNew(OCILib.env,
                                    con->err,
                                    con->cxt,
                                    SQLT_NTY,
                                    obj->nty->tdo,
                                    NULL,
                                    OCI_DURATION_SESSION,
                                    TRUE,
                                    (dvoid **) &obj->handle))
             
        if (obj->con->status != OCI_SUCCESS)    
        {
            OCI_ErrorAPI(obj->con->err, con, NULL);
            OCI_FREE(obj);
        }
    }
    else
        obj->hstate = OCI_OBJECT_FETCHED_CLEAN;

    return obj;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetIndex
 * ------------------------------------------------------------------------ */

int OCI_ObjectGetIndex(OCI_Object *obj, const mtext *attr, int type)
{
    ub2 i;
 
    OCI_CHECK_PTR(obj, -1);

    for (i = 0; i < obj->nty->nb_items; i++)
    {
        OCI_Column *col = &obj->nty->items.p_col[i];

        if (col->type == type  &&  mtscasecmp(col->name, attr) == 0)
        {
           return i;
        }
    }

    return -1;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetAttr
 * ------------------------------------------------------------------------ */

boolean OCI_ObjectGetAttr(OCI_Object *obj, const mtext *attr, void ** p_value,
                          OCIInd* p_ind,  OCIType **p_tdo)
{
    void *ostr = NULL;
    int osize  = -1;

    OCI_CHECK_PTR(obj, FALSE);
    OCI_CHECK_PTR(attr, FALSE);
    OCI_CHECK_PTR(p_value, FALSE);
    OCI_CHECK_PTR(p_ind, FALSE);

    ostr = OCI_GetInputMetaString(attr, &osize);

    OCI_CALL(obj->con, OCIObjectGetAttr(OCILib.env,
                                        obj->con->err,
                                        obj->handle, 
                                        NULL,  obj->nty->tdo,
                                        (CONST text**) &ostr, 
                                        (ub4 *) &osize,
                                        1, NULL, 0,  p_ind, 
                                        NULL, p_value, 
                                        p_tdo))

    OCI_ReleaseMetaString(ostr);

    OCI_CHECK_ERROR(obj->con, NULL, FALSE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectSetAttr
 * ------------------------------------------------------------------------ */

boolean OCI_ObjectSetAttr(OCI_Object *obj, const mtext *attr, void * value, 
                          OCIInd ind)
{
    void *ostr = NULL;
    int osize  = -1;

    OCI_CHECK_PTR(obj, FALSE);
    OCI_CHECK_PTR(attr, FALSE);
    OCI_CHECK_PTR(value, FALSE);

    ostr = OCI_GetInputMetaString(attr, &osize);

    OCI_CALL(obj->con, OCIObjectSetAttr(OCILib.env,
                                        obj->con->err,
                                        obj->handle, 
                                        NULL, obj->nty->tdo,
                                        (CONST text**) &ostr, 
                                        (ub4 *) &osize,
                                        1, NULL, 0, ind, 
                                        NULL, value))

    OCI_ReleaseMetaString(ostr);

    OCI_CHECK_ERROR(obj->con, NULL, FALSE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectSetNumber
 * ------------------------------------------------------------------------ */

boolean OCI_ObjectSetNumber(OCI_Object *obj, const mtext *attr, 
                            void *value, int size, ub1 flag)
{
    boolean res = FALSE;
    int index   = 0;
        
    OCI_CHECK_PTR(obj, FALSE);
        
    index = OCI_ObjectGetIndex(obj, attr, OCI_CDT_INTEGER);
  
    /* if field was not of type integer, try to get it as double */

    if (index < 0)
        index = OCI_ObjectGetIndex(obj, attr, OCI_CDT_DOUBLE);
    
    if(index >= 0)
    {
        OCINumber num;
     
        res = OCI_NumberSet(obj->con, &num, value, size, flag) &&
              OCI_ObjectSetAttr(obj, attr, (void *) &num, 0);
    }

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetNumber
 * ------------------------------------------------------------------------ */

boolean OCI_ObjectGetNumber(OCI_Object *obj, const mtext *attr, 
                            void *value, int size, ub1 flag)
{
    boolean res = FALSE;
    int index   = 0;
    
    OCI_CHECK_PTR(obj, FALSE);
    
    index = OCI_ObjectGetIndex(obj, attr, OCI_CDT_INTEGER);
  
    /* if field was not of type integer, try to get it as double */

    if (index < 0)
        index = OCI_ObjectGetIndex(obj, attr, OCI_CDT_DOUBLE);
    
    if(index >= 0)
    {
        OCIInd ind      = 0;
        OCIType *tdo    = NULL;
        OCINumber *num  = NULL;

        if (OCI_ObjectGetAttr(obj, attr, (void **) &num, &ind, &tdo) && num)
        {
            res = OCI_NumberGet(obj->con, num, value, size, flag);
        }
    }

    return FALSE;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectCreate
 * ------------------------------------------------------------------------ */

OCI_Object * OCI_API OCI_ObjectCreate(OCI_Connection *con, OCI_Schema *schema)
{
    OCI_Object *obj = NULL;

    OCI_CHECK_PTR(con, NULL);

    return OCI_ObjectInit(con, &obj, NULL, schema);
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectFree
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ObjectFree(OCI_Object *obj)
{
    OCI_CHECK_PTR(obj, FALSE);
    OCI_CHECK(obj->hstate == OCI_OBJECT_FETCHED_CLEAN, FALSE);

    /* if the object has sub-objects that have been fetched, we need to free
       these objects */

    if (obj->objs != NULL)
    {
        ub2 i;

        for (i = 0; i < obj->nty->nb_items; i++)
        {
            if (obj->objs[i] != NULL)
            {
               ((OCI_Datatype *) obj->objs[i])->hstate = OCI_OBJECT_FETCHED_DIRTY;

                switch (obj->nty->items.p_col[i].type)
                {
                    case OCI_CDT_DATETIME:

                        OCI_DateFree((OCI_Date *) obj->objs[i]);
                        break;

                    case OCI_CDT_LOB:

                        OCI_LobFree((OCI_Lob *) obj->objs[i]);
                        break;

                    case OCI_CDT_FILE:

                        OCI_FileFree((OCI_File *) obj->objs[i]);
                        break;

                    case OCI_CDT_OBJECT:

                        OCI_ObjectFree((OCI_Object *) obj->objs[i]);
                        break;

                    case OCI_CDT_TIMESTAMP:

                        OCI_TimestampFree((OCI_Timestamp *) obj->objs[i]);
                        break;

                    case OCI_CDT_INTERVAL:

                        OCI_IntervalFree((OCI_Interval *) obj->objs[i]);
                        break;
                }
            }
        }

        OCI_FREE(obj->objs);
    }

    if (obj->hstate == OCI_OBJECT_ALLOCATED)
    {
        OCI_CALL(obj->con, OCIObjectFree(OCILib.env,
                                         obj->con->err,
                                         obj->handle, 
                                         OCI_OBJECTFREE_NONULL))
    }

    OCI_FREE(obj->buf);
    OCI_FREE(obj);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetInt
 * ------------------------------------------------------------------------ */

int OCI_API OCI_ObjectGetInt(OCI_Object *obj, const mtext *attr)
{
    int value = 0;

    OCI_ObjectGetNumber(obj, attr, &value, sizeof(value), OCI_NUMBER_SIGNED);
    
    return value;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetUnsignedInt
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_ObjectGetUnsignedInt(OCI_Object *obj, const mtext *attr)
{
    unsigned int value = 0;

    OCI_ObjectGetNumber(obj, attr, &value, sizeof(value), OCI_NUMBER_UNSIGNED);

    return value;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetBigInt
 * ------------------------------------------------------------------------ */

big_int OCI_API OCI_ObjectGetBigInt(OCI_Object *obj, const mtext *attr)
{
    big_int value = 0;

    OCI_ObjectGetNumber(obj, attr, &value, sizeof(value), OCI_NUMBER_SIGNED);
    
    return value;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetUnsignedBigInt
 * ------------------------------------------------------------------------ */

big_uint OCI_API OCI_ObjectGetUnsignedBigInt(OCI_Object *obj, const mtext *attr)
{
    big_uint value = 0;

    OCI_ObjectGetNumber(obj, attr, &value, sizeof(value), OCI_NUMBER_UNSIGNED);
    
    return value;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetDouble
 * ------------------------------------------------------------------------ */

double OCI_API OCI_ObjectGetDouble(OCI_Object *obj, const mtext *attr)
{
    int index = OCI_ObjectGetIndex(obj, attr, OCI_CDT_DOUBLE);
  
    /* if field was not of type double, try to get it as integer */

    if (index < 0)
        index = OCI_ObjectGetIndex(obj, attr, OCI_CDT_INTEGER);
    
    if(index >= 0)
    {
        double ret       = 0.0;
        OCIInd ind       = 0;
        OCIType *tdo     = NULL;
        OCINumber *value = NULL;

        if (OCI_ObjectGetAttr(obj, attr, (void **) &value, &ind, &tdo) && value)
        {
            OCI_CALL(obj->con, OCINumberToReal(obj->con->err, 
                                              value, sizeof(ret), &ret))

            OCI_CHECK_ERROR(obj->con, NULL, 0);

            return ret;
        }
    }

    return 0.0;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetString
 * ------------------------------------------------------------------------ */

const mtext * OCI_API OCI_ObjectGetString(OCI_Object *obj, const mtext *attr)
{
    int index = OCI_ObjectGetIndex(obj, attr, OCI_CDT_TEXT);

    if (index >= 0)
    {
        OCIInd ind        = 0;
        OCIType *tdo      = NULL;
        OCIString **value = NULL;
        void *ret         = NULL;
        
        if (OCI_ObjectGetAttr(obj, attr, (void **) &value, &ind, &tdo) && value)
        {
            if (value != NULL)
            {
                ret = (void *) OCIStringPtr(OCILib.env, *value);

#ifdef OCI_CHECK_DATASTRINGS 

                {
                    size_t msize = 0;
                    int olen     = OCI_StringLength(ret, sizeof(omtext));
                    int osize    = olen * sizeof(omtext);

                    OCI_FREE(obj->buf);
                    OCI_ALLOC(obj->buf, msize, osize + sizeof(omtext));

                    if (obj->buf == NULL)
                        OCI_ErrorMemory(obj->con, NULL, msize);
                    else
                        OCI_GetOutputDataString(ret, obj->buf, &osize);

                   return obj->buf;
                }

#else
                return (const mtext *) ret;
#endif
            }
        }
    }

    return NULL;    
}


/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetRaw
 * ------------------------------------------------------------------------ */

int OCI_API OCI_ObjectGetRaw(OCI_Object *obj, const mtext *attr, void *buffer, 
                             int len)
{
    int index = OCI_ObjectGetIndex(obj, attr, OCI_CDT_RAW);

    if (index >= 0)
    {
        OCIInd ind     = 0;
        OCIType *tdo   = NULL;
        OCIRaw **value = NULL;

        if (OCI_ObjectGetAttr(obj, attr, (void **) &value, &ind, &tdo) && value)
        {
            if (value != NULL)
            {
                int raw_len = 0;

                OCIRawAllocSize(OCILib.env, obj->con->err, 
                                 *value, (ub4*) &raw_len);

                if (len > raw_len)
                    len = raw_len;

                memcpy(buffer, OCIRawPtr(OCILib.env, *value), len);

                return len;
            }
        }
    }

    return 0;    
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetDate
 * ------------------------------------------------------------------------ */

OCI_Date * OCI_API OCI_ObjectGetDate(OCI_Object *obj, const mtext *attr)
{
    int index = OCI_ObjectGetIndex(obj, attr, OCI_CDT_DATETIME);

    if (index >= 0)
    {
        OCIInd ind      = 0;
        OCIType *tdo    = NULL;
        OCI_Date *date  = NULL;

        date = OCI_DateInit(obj->con, (OCI_Date **) &obj->objs[index], 
                            NULL, FALSE, FALSE);

        if (OCI_ObjectGetAttr(obj, attr, (void **) &date->value, &ind, &tdo))
        {
            return date;
        }
    }

    return NULL;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetTimeStamp
 * ------------------------------------------------------------------------ */

OCI_Timestamp * OCI_API OCI_ObjectGetTimeStamp(OCI_Object *obj, 
                                               const mtext *attr)
{
    int index = OCI_ObjectGetIndex(obj, attr, OCI_CDT_TIMESTAMP);

    if (index >= 0)
    {
        OCIInd ind          = 0;
        OCIType *tdo        = NULL;
        OCIDateTime **value = NULL;

       if (OCI_ObjectGetAttr(obj, attr, (void **) &value, &ind, &tdo) && value)
        {
           return OCI_TimestampInit(obj->con, 
                                    (OCI_Timestamp **) &obj->objs[index], 
                                    (OCIDateTime *) *value, 
                                    obj->nty->items.p_col[index].subtype);
        }
    }

    return NULL;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetInterval
 * ------------------------------------------------------------------------ */

OCI_Interval * OCI_API OCI_ObjectGetInterval(OCI_Object *obj, const mtext *attr)
{
    int index = OCI_ObjectGetIndex(obj, attr, OCI_CDT_INTERVAL);

    if (index >= 0)
    {
        OCIInd ind          = 0;
        OCIType *tdo        = NULL;
        OCIInterval **value = NULL;
        
        if (OCI_ObjectGetAttr(obj, attr, (void **) &value, &ind, &tdo) && value)
        {
            return OCI_IntervalInit(obj->con, 
                                    (OCI_Interval **) &obj->objs[index], 
                                    (OCIInterval *) *value, 
                                    obj->nty->items.p_col[index].subtype);
        }
    }

    return NULL;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetInt
 * ------------------------------------------------------------------------ */

OCI_Object * OCI_API OCI_ObjectGetObject(OCI_Object *obj, const mtext *attr)
{
    int index = OCI_ObjectGetIndex(obj, attr, OCI_CDT_OBJECT);

    if (index >= 0)
    {     
        OCIInd ind    = 0;
        OCIType *tdo  = NULL;
        void *value   = NULL;

        if (OCI_ObjectGetAttr(obj, attr, (void **) &value, &ind, &tdo) && value)
        {
            return OCI_ObjectInit(obj->con, (OCI_Object **) &obj->objs[index], 
                                  value, obj->nty->items.p_col[index].nty);
        }
    }

    return NULL;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetInterval
 * ------------------------------------------------------------------------ */

OCI_Lob * OCI_API OCI_ObjectGetLob(OCI_Object *obj, const mtext *attr)
{
    int index = OCI_ObjectGetIndex(obj, attr, OCI_CDT_LOB);

    if (index >= 0)
    {    
        OCIInd ind            = 0;
        OCIType *tdo          = NULL;
        OCILobLocator **value = NULL;

        if (OCI_ObjectGetAttr(obj, attr, (void **) &value, &ind, &tdo) && value)
        {
            return OCI_LobInit(obj->con, (OCI_Lob **) &obj->objs[index], 
                               *value, obj->nty->items.p_col[index].subtype);
        }
    }

    return NULL;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetFile
 * ------------------------------------------------------------------------ */

OCI_File * OCI_API OCI_ObjectGetFile(OCI_Object *obj, const mtext *attr)
{
    int index = OCI_ObjectGetIndex(obj, attr, OCI_CDT_FILE);

    if (index >= 0)
    {     
        OCIInd ind            = 0;
        OCIType *tdo          = NULL;
        OCILobLocator **value = NULL;

        if (OCI_ObjectGetAttr(obj, attr, (void **) &value, &ind, &tdo) && value)
        {
            return OCI_FileInit(obj->con, (OCI_File **) &obj->objs[index], 
                                  *value, obj->nty->items.p_col[index].subtype);
        }
    }

    return NULL;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectSetShort
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ObjectSetShort(OCI_Object *obj, const mtext *attr, short value)
{
    return OCI_ObjectSetNumber(obj, attr, &value, sizeof(value), 
                               OCI_NUMBER_SIGNED);
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectSetUnsignedShort
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ObjectSetUnsignedShort(OCI_Object *obj, const mtext *attr, 
                                           unsigned short value)
{
    return OCI_ObjectSetNumber(obj, attr, &value, sizeof(value), 
                               OCI_NUMBER_UNSIGNED);
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectSetInt
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ObjectSetInt(OCI_Object *obj, const mtext *attr, int value)
{
    return OCI_ObjectSetNumber(obj, attr, &value, sizeof(value), 
                               OCI_NUMBER_SIGNED);
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectSetUnsignedInt
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ObjectSetUnsignedInt(OCI_Object *obj, const mtext *attr, 
                                        unsigned int value)
{
    return OCI_ObjectSetNumber(obj, attr, &value, sizeof(value), 
                               OCI_NUMBER_UNSIGNED);
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectSetBigInt
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ObjectSetBigInt(OCI_Object *obj, const mtext *attr,
                                    big_int value)
{
    return OCI_ObjectSetNumber(obj, attr, &value, sizeof(value), 
                               OCI_NUMBER_SIGNED);
}


/* ------------------------------------------------------------------------ *
 * OCI_ObjectSetUnsignedBigInt
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ObjectSetUnsignedBigInt(OCI_Object *obj, const mtext *attr,
                                            big_uint value)
{
    return OCI_ObjectSetNumber(obj, attr, &value, sizeof(value), 
                               OCI_NUMBER_UNSIGNED);
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectSetDouble
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ObjectSetDouble(OCI_Object *obj, const mtext *attr, 
                                    double value)
{
    int index = OCI_ObjectGetIndex(obj, attr, OCI_CDT_DOUBLE);
  
    /* if field was not of type double, try to get it as integer */

    if (index < 0)
        index = OCI_ObjectGetIndex(obj, attr, OCI_CDT_INTEGER);
    
    if(index >= 0)
    {
        OCINumber num;

        memset(&num, 0, sizeof(num));

        OCI_CALL(obj->con, OCINumberFromReal(obj->con->err, &value,
                                             sizeof(value), &num))

        OCI_CHECK_ERROR(obj->con, NULL, FALSE);
        
        return OCI_ObjectSetAttr(obj, attr, (void *) &num, 0);
    }

    return FALSE;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectSetString
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ObjectSetString(OCI_Object *obj, const mtext *attr,
                                    const dtext *value)
{
    int index = OCI_ObjectGetIndex(obj, attr, OCI_CDT_TEXT);
  
    if (index >= 0)
    {
        OCIString *str = NULL;
        void *ostr     = NULL;
        int osize      = -1;
        boolean ret    = FALSE;

        if (value == NULL)
            return OCI_ObjectSetNull(obj, attr);

        ostr = OCI_GetInputDataString(value, &osize);
    
        OCI_CALL(obj->con, OCIStringAssignText(OCILib.env, 
                                               obj->con->err, 
                                               (CONST oratext *) ostr,
                                               (ub4) osize, &str))

        OCI_CHECK_ERROR(obj->con, NULL, FALSE);
 
        ret = OCI_ObjectSetAttr(obj, attr, (void *) str, 0);

        OCI_ReleaseDataString(ostr);

        return ret;
    }

    return FALSE;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectSetRaw
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ObjectSetRaw(OCI_Object *obj, const mtext *attr, 
                                 void* value, int len)
{
    int index = OCI_ObjectGetIndex(obj, attr, OCI_CDT_RAW);
  
    if (index >= 0)
    {
        OCIRaw *raw=NULL;
     
        if (value == NULL)
            return OCI_ObjectSetNull(obj, attr);

        OCI_CALL(obj->con, OCIRawAssignBytes(OCILib.env, 
                                             obj->con->err, 
                                             (CONST ub1*) value,
                                             len, &raw))

        OCI_CHECK_ERROR(obj->con, NULL, FALSE);
        
        return OCI_ObjectSetAttr(obj, attr, (void *) raw, 0);
    }

    return FALSE;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectSetNull
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ObjectSetNull(OCI_Object *obj, const mtext *attr)
{
    void *ostr = NULL;
    int osize  = -1;

    OCI_CHECK_PTR(obj, FALSE);
    OCI_CHECK_PTR(attr, FALSE);

    ostr = OCI_GetInputMetaString(attr, &osize);
    
    OCI_CALL(obj->con, OCIObjectSetAttr(OCILib.env,
                                        obj->con->err,
                                        obj->handle, 
                                        NULL, obj->nty->tdo,
                                        (CONST text**) &ostr, 
                                        (ub4 *) &osize,
                                        1, NULL, 0, -1, 
                                        NULL, NULL))

    OCI_ReleaseMetaString(ostr);

    OCI_CHECK_ERROR(obj->con, NULL, FALSE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 *                         OCI_Schema functions
 * ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ *
 * OCI_SchemaClose
 * ------------------------------------------------------------------------ */

boolean OCI_SchemaClose(OCI_Schema *schema)
{
    ub2 i;

    OCI_CHECK_PTR(schema, FALSE);

    for (i=0; i < schema->nb_items; i++)
    {
        OCI_FREE(schema->items.p_col[i].name);
    }

    OCI_DIRECT(schema->con, OCIHandleFree(schema->handle, OCI_HTYPE_DESCRIBE))

    OCI_FREE(schema->items.p_bytes);
    OCI_FREE(schema->name);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_SchemaGet
 * ------------------------------------------------------------------------ */

OCI_Schema * OCI_API OCI_SchemaGet(OCI_Connection *con, const mtext *name, 
                                   int type)
{
    OCI_Schema *schema = NULL;
    void *ostr         = NULL;
    OCI_Item *item     = NULL;
    OCIParam *hattr    = NULL;
    OCIParam *hlist    = NULL;
    mtext *str         = NULL;
    int osize          = -1;
    size_t msize       = 0;
    ub1 desc_type      = 0;
    ub4 attr_type      = 0;
    ub4 num_type       = 0;
    ub2 i;

    OCI_CHECK_PTR(con, NULL);
    OCI_CHECK_PTR(name, NULL);

    if (type == OCI_SCHEMA_TABLE)
        desc_type = OCI_PTYPE_TABLE;
    else if (type == OCI_SCHEMA_VIEW)
        desc_type = OCI_PTYPE_VIEW;
    else if (type == OCI_SCHEMA_TYPE)
        desc_type = OCI_PTYPE_TYPE;
    else
        return NULL;

    /* first try to find it in list */

    item = con->sobjs->head;

    /* walk along the list to find the type */
    while (item != NULL)
    {
        schema = (OCI_Schema *) item->data;
     
        if (schema != NULL && mtscasecmp(schema->name, name) == 0)
        {
            if (schema->type == type)
                return schema;
        }

        item = item->next;
    } 

    /* Not found, so create type object */

    item = OCI_ListAppend(con->sobjs, sizeof(OCI_Schema));

    OCI_CHECK_PTR(item, NULL);

    schema = (OCI_Schema *) item->data;

    schema->type = type;
    schema->con  = con;
    schema->name = mtsdup(name);

    /* type name have to be upper case */
    for (str = schema->name; *str != 0; str++)
        *str = (mtext) mttoupper((short)*str);

    OCI_CALL(con, OCIHandleAlloc(OCILib.env, (dvoid **) (void *) &schema->handle, 
                                 OCI_HTYPE_DESCRIBE, 0, (dvoid **) NULL))

    ostr = OCI_GetInputMetaString(schema->name, &osize);

    if (type == OCI_SCHEMA_TYPE)
    {            
        attr_type = OCI_ATTR_LIST_TYPE_ATTRS;
        num_type  = OCI_ATTR_NUM_TYPE_ATTRS;

        OCI_CALL(con, OCITypeByName(OCILib.env, con->err, con->cxt,
                                    (CONST text *) NULL, (ub4) 0, 
                                    (CONST text *) ostr, (ub4) osize, 
                                    (CONST text *) NULL, (ub4) 0, 
                                    OCI_DURATION_SESSION, OCI_TYPEGET_ALL,
                                    &schema->tdo))

        OCI_CALL(con, OCIDescribeAny(con->cxt, con->err, (void *) schema->tdo,
                                     0, OCI_OTYPE_PTR, OCI_DEFAULT, 
                                     OCI_PTYPE_TYPE, schema->handle))
    }
    else
    {
        attr_type = OCI_ATTR_LIST_COLUMNS;
        num_type  = OCI_ATTR_NUM_COLS;

        OCI_CALL(con, OCIDescribeAny(con->cxt, con->err, (dvoid *) ostr, 
                                     (ub4) osize, OCI_OTYPE_NAME, 
                                     OCI_DEFAULT, desc_type, schema->handle))
    }
          
    OCI_ReleaseMetaString(ostr);


    OCI_CALL(con, OCIAttrGet(schema->handle, OCI_HTYPE_DESCRIBE, &hattr, 
                             NULL, OCI_ATTR_PARAM, con->err))


    OCI_CALL(con, OCIAttrGet(hattr, OCI_DTYPE_PARAM, &schema->nb_items,
                                 NULL, num_type, con->err))

    OCI_CALL(con, OCIAttrGet(hattr, OCI_DTYPE_PARAM, &hlist, 
                             NULL, attr_type, con->err))

    OCI_ALLOC(unsigned char *, schema->items.p_bytes, msize, 
              sizeof(OCI_Column)*schema->nb_items);

    if (schema->items.p_bytes == NULL)
    {
        OCI_ErrorMemory(con, NULL, msize);
        OCI_SchemaFree(schema);
        return NULL;
    }

    for (i = 0; i < schema->nb_items; i++)
    {
        OCI_Column *col = &schema->items.p_col[i];
  
        if (OCI_ColumnDescribe(col, con, NULL, (void *) hlist, i+1, 
                               schema->type != OCI_SCHEMA_TYPE))
        {
            OCI_ColumnMap(col, NULL);
        }
        else
            break;
    }

   /* handle errors */

    if (OCI_NO_ERROR(con) == FALSE)
    {
       OCI_ErrorAPI(con->err, con, NULL);
       OCI_SchemaFree(schema);        
       schema = NULL;
    }

    return schema; 
}

/* ------------------------------------------------------------------------ *
 * OCI_SchemaFree
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_SchemaFree(OCI_Schema *schema)
{
    OCI_CHECK_PTR(schema, FALSE);

    OCI_ListRemove(schema->con->sobjs, schema);

    OCI_SchemaClose(schema);

    OCI_FREE(schema);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_SchemaGetType
 * ------------------------------------------------------------------------ */

int OCI_API OCI_SchemaGetType(OCI_Schema *schema)
{
    OCI_CHECK_PTR(schema, OCI_UNKNOWN);

    return schema->type;
}

/* ------------------------------------------------------------------------ *
 * OCI_SchemaGetColumnCount
 * ------------------------------------------------------------------------ */

int OCI_API OCI_SchemaGetColumnCount(OCI_Schema *schema)
{
    OCI_CHECK_PTR(schema, 0);
  
    return schema->nb_items;
}

/* ------------------------------------------------------------------------ *
 * OCI_SchemaGetColumn
 * ------------------------------------------------------------------------ */

OCI_Column * OCI_API OCI_SchemaGetColumn(OCI_Schema *schema, int index)
{
    OCI_CHECK_PTR(schema, NULL);
    OCI_CHECK(index <= 0, NULL);
    OCI_CHECK(index > schema->nb_items, NULL);

    return &(schema->items.p_col[index-1]);
}

/* ------------------------------------------------------------------------ *
 * OCI_SchemaGetName
 * ------------------------------------------------------------------------ */

const mtext * OCI_API OCI_SchemaGetName(OCI_Schema *schema)
{
    OCI_CHECK_PTR(schema, NULL);

    return schema->name;
}

/* ------------------------------------------------------------------------ *
 *                        Thread functions
 * ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ *
 * OCI_ThreadProc
 * ------------------------------------------------------------------------ */

void OCI_ThreadProc(dvoid *arg)
{
    OCI_Thread *thread = (OCI_Thread *) arg;

    if (thread != NULL)
        thread->proc(thread, thread->arg);
}

/* ------------------------------------------------------------------------ *
 * OCI_ThreadCreate
 * ------------------------------------------------------------------------ */

OCI_Thread * OCI_API OCI_ThreadCreate(void)
{
    OCI_Thread *thread = NULL;
    size_t msize =  0;

    OCI_CHECK_BOOL(OCI_LIB_THREADED, FALSE);

    /* allocate thread structure  */

    OCI_ALLOC(OCI_Thread *, thread, msize, sizeof(OCI_Thread));

    OCI_CHECK_PTR(thread, NULL);

    /* allocate error handle */

    OCI_DIRECT(thread, OCIHandleAlloc(OCILib.env, (dvoid **) (void *) &thread->err,
                                      OCI_HTYPE_ERROR, 0, (dvoid **) NULL));
    /* allocate thread ID */

    OCI_CALL(thread, OCIThreadIdInit(OCILib.env, thread->err, &thread->id));

    /* allocate thread handle */

    OCI_CALL(thread, OCIThreadHndInit(OCILib.env, thread->err, &thread->handle));

    /* Handle errors */

    if (OCI_NO_ERROR(thread) == FALSE)
    {
        OCI_ErrorAPI(thread->err, NULL, NULL);
        OCI_ThreadFree(thread);
        thread = NULL;
    }

    return thread;
}

/* ------------------------------------------------------------------------ *
 * OCI_ThreadFree
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ThreadFree(OCI_Thread *thread)
{
    OCI_CHECK_PTR(thread, FALSE);
    OCI_CHECK_BOOL(OCI_LIB_THREADED, FALSE);

    /* close thread handle */

    if (thread->handle != NULL)
    {
        OCI_DIRECT(thread, OCIThreadClose(OCILib.env, 
                                          thread->err, 
                                          thread->handle))

        OCI_DIRECT(thread, OCIThreadHndDestroy(OCILib.env,
                                               thread->err, 
                                               &thread->handle))
    }

    /* close thread id */

    if (thread->id != NULL)
        OCI_DIRECT(thread, OCIThreadIdDestroy(OCILib.env, thread->err, 
                                              &thread->id))    

    /* check for errors */

    if (OCI_NO_ERROR(thread) == FALSE)
        OCI_ErrorAPI(thread->err, NULL, NULL);

    /* close error handle */
    
    if (thread->err != NULL)
       OCIHandleFree(thread->err, OCI_HTYPE_ERROR);

    /* free mutex structure */

    OCI_FREE(thread);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_ThreadRun
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ThreadRun(OCI_Thread *thread, POCI_THREAD proc, void *arg)
{
    OCI_CHECK_PTR(thread, FALSE);
    OCI_CHECK_PTR(proc, FALSE);
    OCI_CHECK_BOOL(OCI_LIB_THREADED, FALSE);

    thread->proc = proc;
    thread->arg  = arg;

    OCI_DIRECT(thread, OCIThreadCreate(OCILib.env, thread->err, OCI_ThreadProc,
                                       thread, thread->id, thread->handle))

    OCI_CHECK_STATUS(thread, FALSE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_ThreadJoin
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ThreadJoin(OCI_Thread *thread)
{
    OCI_CHECK_PTR(thread, FALSE);
    OCI_CHECK_BOOL(OCI_LIB_THREADED, FALSE);

    OCI_DIRECT(thread, OCIThreadJoin(OCILib.env, thread->err, thread->handle))

    OCI_CHECK_STATUS(thread, FALSE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 *                        Mutex functions
 * ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ *
 * OCI_MutexCreate
 * ------------------------------------------------------------------------ */

OCI_Mutex * OCI_API OCI_MutexCreate(void)
{
    OCI_Mutex *mutex = NULL;
    size_t msize =  0;

    /* allocate mutex structure  */

    OCI_ALLOC(OCI_Mutex *, mutex, msize, sizeof(OCI_Mutex));

    OCI_CHECK_PTR(mutex, NULL);

    /* allocate error handle */

    OCI_DIRECT(mutex, OCIHandleAlloc(OCILib.env, (dvoid **) (void *) &mutex->err,
                                      OCI_HTYPE_ERROR, 0, (dvoid **) NULL))

    /* allocate mutex handle */

    OCI_CALL(mutex, OCIThreadMutexInit(OCILib.env, mutex->err, &mutex->handle))

    /* Handle errors */

    if (OCI_NO_ERROR(mutex) == FALSE)
    {
        OCI_ErrorAPI( mutex->err, NULL, NULL);
        OCI_MutexFree(mutex);
        mutex = NULL;
    }

    return mutex;
}

/* ------------------------------------------------------------------------ *
 * OCI_MutexFree
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_MutexFree(OCI_Mutex *mutex)
{
    OCI_CHECK_PTR(mutex, FALSE);

    /* close mutext handle */

    if (mutex->handle != NULL)
        OCI_DIRECT(mutex, OCIThreadMutexDestroy(OCILib.env, mutex->err, 
                                                &mutex->handle))
    /* check for errors */

    if (OCI_NO_ERROR(mutex) == FALSE)
        OCI_ErrorAPI(mutex->err, NULL, NULL);

    /* close error handle */
    
    if (mutex->err != NULL)
        OCIHandleFree(mutex->err, OCI_HTYPE_ERROR);

    /* free mutex structure */

    OCI_FREE(mutex);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_MutexAcquire
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_MutexAcquire(OCI_Mutex *mutex)
{
    OCI_CHECK_PTR(mutex, FALSE);

    OCI_DIRECT(mutex, OCIThreadMutexAcquire(OCILib.env, mutex->err, mutex->handle))
    
    OCI_CHECK_STATUS(mutex, FALSE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_MutexRelease
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_MutexRelease(OCI_Mutex *mutex)
{
    OCI_CHECK_PTR(mutex, FALSE);

    OCI_DIRECT(mutex, OCIThreadMutexRelease(OCILib.env, mutex->err, mutex->handle))
  
    OCI_CHECK_STATUS(mutex, FALSE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 *                        ThreadKey functions
 * ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ *
 * OCI_ThreadKeyCreate
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ThreadKeyCreate(mtext *name, POCI_THREADKEYDEST destfunc)
{
    OCI_ThreadKey *key = NULL;
    size_t msize =  0;

    OCI_CHECK_PTR(name, FALSE);

    if (OCILib.key_map == NULL)
    {
        /* create the map at the first call to OCI_ThreadKeyCreate to save
           time and memory when it's not needed */

        OCILib.key_map = OCI_HashCreate(OCI_HASH_DEFAULT_SIZE, 
                                        OCI_HASH_POINTER);

    }

    OCI_CHECK_PTR(OCILib.key_map, FALSE);

    /* allocate key structure  */

    OCI_ALLOC(OCI_ThreadKey *, key, msize, sizeof(OCI_ThreadKey));

    OCI_CHECK_PTR(key, FALSE);

    /* allocate error handle */

    OCI_DIRECT(key, OCIHandleAlloc(OCILib.env, (dvoid **) (void *) &key->err,
                                      OCI_HTYPE_ERROR, 0, (dvoid **) NULL))

    /* allocate key handle */

    OCI_CALL(key, OCIThreadKeyInit(OCILib.env, key->err, &key->handle, destfunc))

    /* Handle errors */

    if (OCI_NO_ERROR(key) == FALSE)
    {
        OCI_ErrorAPI(key->err, NULL, NULL);
        OCI_ThreadKeyFree(key);
    }
    else
    {
        OCI_HashAddPointer(OCILib.key_map, name, key);
    }

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_ThreadKeyFree
 * ------------------------------------------------------------------------ */

boolean OCI_ThreadKeyFree(OCI_ThreadKey *key)
{
   OCI_CHECK_PTR(key, FALSE);

    /* close mutext handle */

    if (key->handle != NULL)
        OCI_DIRECT(key, OCIThreadKeyDestroy(OCILib.env, key->err, &key->handle))

    /* check for errors */

    if (OCI_NO_ERROR(key) == FALSE)
        OCI_ErrorAPI(key->err, NULL, NULL);

    /* close error handle */
    
    if (key->err != NULL)
        OCIHandleFree(key->err, OCI_HTYPE_ERROR);

    /* free key structure */

    OCI_FREE(key);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_ThreadKeySetValue
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ThreadKeySetValue(mtext *name, void *value)
{
    OCI_ThreadKey* key;

    OCI_CHECK_PTR(name, FALSE);

    key = OCI_HashGetPointer(OCILib.key_map, name);

    OCI_CHECK_PTR(key, FALSE);

    OCI_DIRECT(key, OCIThreadKeySet(OCILib.env, key->err, key->handle, value))
    
    OCI_CHECK_STATUS(key, FALSE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_ThreadKeyGetValue
 * ------------------------------------------------------------------------ */

void * OCI_API OCI_ThreadKeyGetValue(mtext *name)
{
    void * value = NULL;
    OCI_ThreadKey* key;

    OCI_CHECK_PTR(name, FALSE);

    key = OCI_HashGetPointer(OCILib.key_map, name);

    OCI_CHECK_PTR(key, NULL);

    OCI_DIRECT(key, OCIThreadKeyGet(OCILib.env, key->err, key->handle, &value))
    
    OCI_CHECK_STATUS(key, NULL);

    return value;
}

/* ------------------------------------------------------------------------ *
 *                        Handle functions
 * ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ *
 * OCI_HandleGetEnvironment
 * ------------------------------------------------------------------------ */

const void * OCI_API OCI_HandleGetEnvironment(void)
{
    return OCILib.env;
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleGetContext
 * ------------------------------------------------------------------------ */

const void * OCI_API OCI_HandleGetContext(OCI_Connection *con)
{
    OCI_CHECK_PTR(con, NULL);

    return con->cxt;
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleGetServer
 * ------------------------------------------------------------------------ */

const void * OCI_API OCI_HandleGetServer(OCI_Connection *con)
{
    OCI_CHECK_PTR(con, NULL);

    return con->svr;
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleGetError
 * ------------------------------------------------------------------------ */

const void * OCI_API OCI_HandleGetError(OCI_Connection *con)
{
    OCI_CHECK_PTR(con, NULL);

    return con->err;
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleGetSession
 * ------------------------------------------------------------------------ */

const void * OCI_API OCI_HandleGetSession(OCI_Connection *con)
{
    OCI_CHECK_PTR(con, NULL);

    return con->ses;
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleGetTransaction
 * ------------------------------------------------------------------------ */

const void * OCI_API OCI_HandleGetTransaction(OCI_Transaction *trans)
{
    OCI_CHECK_PTR(trans, NULL);

    return trans->htr;
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleGetStatement
 * ------------------------------------------------------------------------ */

const void * OCI_API OCI_HandleGetStatement(OCI_Statement *stmt)
{
    OCI_CHECK_PTR(stmt, NULL);

    return stmt->stmt;
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleGetLob
 * ------------------------------------------------------------------------ */

const void * OCI_API OCI_HandleGetLob(OCI_Lob *lob)
{
    OCI_CHECK_PTR(lob, NULL);

    return lob->handle;
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleGetFile
 * ------------------------------------------------------------------------ */

const void * OCI_API OCI_HandleGetFile(OCI_File *file)
{
    OCI_CHECK_PTR(file, NULL);

    return file->handle;
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleGetDate
 * ------------------------------------------------------------------------ */

const void * OCI_API OCI_HandleGetDate(OCI_Date *date)
{
    OCI_CHECK_PTR(date, NULL);

    return date->value;
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleGetTimestamp
 * ------------------------------------------------------------------------ */

const void * OCI_API OCI_HandleGetTimestamp(OCI_Timestamp *tmsp)
{
    OCI_CHECK_PTR(tmsp, NULL);

    return tmsp->handle;
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleGetInterval
 * ------------------------------------------------------------------------ */

const void * OCI_API OCI_HandleGetInterval(OCI_Interval *itv)
{
    OCI_CHECK_PTR(itv, NULL);

    return itv->handle;
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleGetObject
 * ------------------------------------------------------------------------ */

const void * OCI_API OCI_HandleGetObject(OCI_Object *obj)
{
    OCI_CHECK_PTR(obj, NULL);

    return obj->handle;
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleGetMutex
 * ------------------------------------------------------------------------ */

const void * OCI_API OCI_HandleGetMutex(OCI_Mutex *mutex)
{
    OCI_CHECK_PTR(mutex, NULL);

    return mutex->handle;
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleGetThreadID
 * ------------------------------------------------------------------------ */

const void * OCI_API OCI_HandleGetThreadID(OCI_Thread *thread)
{
    OCI_CHECK_PTR(thread, NULL);

    return thread->id;
}

/* ------------------------------------------------------------------------ *
 * OCI_HandleGetThread
 * ------------------------------------------------------------------------ */

const void * OCI_API OCI_HandleGetThread(OCI_Thread *thread)
{
    OCI_CHECK_PTR(thread, NULL);

    return thread->handle;
}


/* ------------------------------------------------------------------------ *
                    OCI_HashTable functions
 * ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ *
 * OCI_HashCompute
 * ------------------------------------------------------------------------ */

unsigned int OCI_HashCompute(OCI_HashTable *table, const mtext *str)
{
    unsigned int h;
    mtext *p;
    mtext c;

    OCI_CHECK_PTR(table, 0);
    OCI_CHECK_PTR(str, 0);

    for(h = 0, p = (mtext *) str; (*p) != 0; p++)
    {
        c = *p;

        h = 31 * h + mttoupper(c);
    }

    return (h % table->size);
}

/* ------------------------------------------------------------------------ *
 * OCI_HashCreate
 * ------------------------------------------------------------------------ */

OCI_HashTable * OCI_API OCI_HashCreate(int size, int type)
{
    OCI_HashTable *table = NULL;
    size_t msize = 0;

    OCI_ALLOC(OCI_HashTable *, table, msize, sizeof(OCI_HashTable));

    if(table == NULL)
    {
        OCI_ErrorMemory(NULL, NULL, msize);
        return NULL;
    }

    OCI_ALLOC(OCI_HashEntry **, table->items, msize, size * sizeof(OCI_HashEntry *));

    if (table->items == NULL)
    {
        OCI_ErrorMemory(NULL, NULL, msize);
        OCI_FREE(table);
        return NULL;
    }

    table->size     = size;
    table->type     = type;
    table->count    = 0;

    return table;
}

/* ------------------------------------------------------------------------ *
 * OCI_HashFree
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_HashFree(OCI_HashTable *table)
{
    unsigned int i;

    OCI_HashEntry *e1, *e2;
    OCI_HashValue *v1, *v2;

    OCI_CHECK_PTR(table, FALSE);

    for (i = 0; i < table->size; i++)
    {
        e1 = table->items[i];

        while (e1 != NULL)
        {
            e2 = e1;
            e1 = e1->next;

            v1 = e2->values;

            while (v1 != NULL)
            {
                v2 = v1;
                v1 = v1->next;

                if (table->type == OCI_HASH_STRING)
                    OCI_FREE(v2->value.p_mtext);

                OCI_FREE(v2);
            }

            if (e2->key)
                OCI_FREE(e2->key);

            if (e2)
                OCI_FREE(e2);
        }
    }

    if (table->items)
    {
        OCI_FREE(table->items);
    }

    OCI_FREE(table);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_HashGetSize
 * ------------------------------------------------------------------------ */

int OCI_API OCI_HashGetSize(OCI_HashTable *table)
{
    OCI_CHECK_PTR(table, 0);

    return table->size;
}

/* ------------------------------------------------------------------------ *
 * OCI_HashGetType
 * ------------------------------------------------------------------------ */

int OCI_API OCI_HashGetType(OCI_HashTable *table)
{
    OCI_CHECK_PTR(table, OCI_UNKNOWN);

    return table->type;
}

/* ------------------------------------------------------------------------ *
 * OCI_HashGetValue
 * ------------------------------------------------------------------------ */

OCI_HashValue * OCI_API OCI_HashGetValue(OCI_HashTable *table, const mtext *key)
{
    OCI_HashEntry *e=NULL;

    OCI_CHECK_PTR(table, NULL);

    e = OCI_HashLookup(table, key, FALSE);

    return ((e != NULL) ? e->values : NULL);
}

/* ------------------------------------------------------------------------ *
 * OCI_HashGetEntry
 * ------------------------------------------------------------------------ */

OCI_HashEntry * OCI_API OCI_HashGetEntry(OCI_HashTable *table, int index)
{
    OCI_CHECK_PTR(table, NULL);
    OCI_CHECK(index < 0, NULL);
    OCI_CHECK(index > (int) table->size, NULL);

    return table->items[index];
}

/* ------------------------------------------------------------------------ *
 * OCI_HashGetString
 * ------------------------------------------------------------------------ */

const mtext * OCI_API OCI_HashGetString(OCI_HashTable *table, const mtext *key)
{
    OCI_HashValue *v = NULL;

    OCI_CHECK_PTR(table, NULL);
    OCI_CHECK(table->type != OCI_HASH_STRING, NULL);

    v = OCI_HashGetValue(table, key);

    return ((v != NULL) ? v->value.p_mtext : NULL);
}

/* ------------------------------------------------------------------------ *
 * OCI_HashGetInt
 * ------------------------------------------------------------------------ */

int OCI_API OCI_HashGetInt(OCI_HashTable *table, const mtext *key)
{
    OCI_HashValue *v = NULL;

    OCI_CHECK_PTR(table, 0);
    OCI_CHECK(table->type != OCI_HASH_INTEGER, 0);

    v = OCI_HashGetValue(table, key);

    return ((v != NULL) ? v->value.num : 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_HashGetPointer
 * ------------------------------------------------------------------------ */

void * OCI_API OCI_HashGetPointer(OCI_HashTable *table, const mtext *key)
{
    OCI_HashValue *v = NULL;

    OCI_CHECK_PTR(table, NULL);
    OCI_CHECK(table->type != OCI_HASH_POINTER, NULL);

    v = OCI_HashGetValue(table, key);

    return ((v != NULL) ? v->value.p_void : NULL);
}

/* ------------------------------------------------------------------------ *
 * OCI_HashAdd
 * ------------------------------------------------------------------------ */

boolean OCI_HashAdd(OCI_HashTable *table, const mtext *key, OCI_Variant value, 
                    int type)
{
    OCI_HashEntry * e;
    OCI_HashValue * v, *v1, *v2;
    size_t msize = 0;

    OCI_CHECK_PTR(table, FALSE);
    OCI_CHECK_PTR(key, FALSE);
    OCI_CHECK(table->type != type, FALSE);

    e = OCI_HashLookup(table, key, TRUE);

    if (e != NULL)
    {
          OCI_ALLOC(OCI_HashValue *, v, msize, sizeof(OCI_HashValue));

          if(v == NULL)
          {
              OCI_ErrorMemory(NULL, NULL, msize);
              return FALSE;
          }

          if (table->type == OCI_HASH_STRING && value.p_mtext != NULL)
          {
              v->value.p_mtext = mtsdup(value.p_mtext);
          }
          else if (table->type == OCI_HASH_INTEGER)
          {
              v->value.num = value.num;
          }
          else
              v->value.p_void = value.p_void;

        v1 = v2 = e->values;

        while (v1 != NULL)
        {
            v2 = v1;
            v1 = v1->next;
        }

        if (v2 != NULL)
            v2->next  = v;
        else
            e->values = v;
    }

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_HashAddString
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_HashAddString(OCI_HashTable *table, const mtext *key, 
                                  const mtext *value)
{
    OCI_Variant v;

    OCI_CHECK_PTR(table, FALSE);

    v.p_mtext = (mtext *) value;

    return OCI_HashAdd(table, key, v, OCI_HASH_STRING);
}

/* ------------------------------------------------------------------------ *
 * OCI_HashAddInt
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_HashAddInt(OCI_HashTable *table, const mtext *key,
                               int value)
{
    OCI_Variant v;

    OCI_CHECK_PTR(table, FALSE);

    v.num = value;

    return OCI_HashAdd(table, key, v, OCI_HASH_INTEGER);
}

/* ------------------------------------------------------------------------ *
 * OCI_HashAddPointer
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_HashAddPointer(OCI_HashTable *table, const mtext *key, 
                                   void *value)
{
    OCI_Variant v;

    OCI_CHECK_PTR(table, FALSE);

    v.p_void = value;

    return OCI_HashAdd(table, key, v, OCI_HASH_POINTER);
}

/* ------------------------------------------------------------------------ *
 * OCI_HashLookup
 * ------------------------------------------------------------------------ */

OCI_HashEntry * OCI_API OCI_HashLookup(OCI_HashTable *table, const mtext *key,
                                       boolean create)
{
    unsigned int i;
    size_t msize;
    OCI_HashEntry *e, *e1, *e2;

    OCI_CHECK_PTR(table, NULL);
    OCI_CHECK_PTR(key, NULL);

    i = OCI_HashCompute(table, key);

    if (i >= table->size)
        return NULL;

    for(e = table->items[i]; e != NULL; e = e->next)
    {
        if (mtscasecmp(e->key, key) == 0)
            return e;
    }

    if (create == TRUE)
    {
        OCI_ALLOC(OCI_HashEntry *, e, msize, sizeof(OCI_HashEntry));

        if(e == NULL)
        {
            OCI_ErrorMemory(NULL, NULL, msize);
            return NULL;
        }

        e->key = mtsdup(key);

        e1 = e2 = table->items[i];

        while (e1 != NULL)
        {
            e2 = e1;
            e1 = e1->next;
        }

        if (e2 != NULL)
            e2->next = e;
        else
            table->items[i] = e;
    }

    return e;
}
