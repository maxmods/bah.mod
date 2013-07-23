/* config.h.in.  Generated from configure.ac by autoheader.  */

/*#ifndef WIN32
#cmakedefine _DEBUG
#endif*/

/* Define to 1 to use CyaSSL as a replacement for OpenSSL. 
 * LWS_OPENSSL_SUPPORT needs to be set also for this to work. */
/*#cmakedefine USE_CYASSL*/

/* The Libwebsocket version */
#define LWS_LIBRARY_VERSION "1.3"

/* The current git commit hash that we're building from */
#define LWS_BUILD_HASH "6c58228577306c023b072b5c7c7a2b044a94f12a"

/* Build with OpenSSL support */
/*#cmakedefine LWS_OPENSSL_SUPPORT*/

/* Sets the path where the client certs should be installed. */
/*#cmakedefine LWS_OPENSSL_CLIENT_CERTS "${LWS_OPENSSL_CLIENT_CERTS}"*/

/* Turn off websocket extensions */
/*#cmakedefine LWS_NO_EXTENSIONS*/

/* Turn on latency measuring code */
/*#cmakedefine LWS_LATENCY*/

/* Don't build the daemonizeation api */
#define LWS_NO_DAEMONIZE

/* Build without server support */
/*#cmakedefine LWS_NO_SERVER*/

/* Build without client support */
/*#cmakedefine LWS_NO_CLIENT*/

/* If we should compile with MinGW support */
#ifdef WIN32
#define LWS_MINGW_SUPPORT
#endif

/* Use the BSD getifaddrs that comes with libwebsocket, for uclibc support */
#define LWS_BUILTIN_GETIFADDRS 1

/* Define to 1 if you have the `bzero' function. */
#define HAVE_BZERO 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the `fork' function. */
#define HAVE_FORK 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1
 
/* Define to 1 if you have the `ssl' library (-lssl). */
//#cmakedefine HAVE_LIBSSL

/* Define to 1 if your system has a GNU libc compatible `malloc' function, and
   to 0 otherwise. */
#define HAVE_MALLOC 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `memset' function. */
#define HAVE_MEMSET 1

/* Define to 1 if you have the <netinet/in.h> header file. */
#define HAVE_NETINET_IN_H 1

/* Define to 1 if your system has a GNU libc compatible `realloc' function,
   and to 0 otherwise. */
#define HAVE_REALLOC 1

/* Define to 1 if you have the `socket' function. */
#define HAVE_SOCKET 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strerror' function. */
#define HAVE_STRERROR 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/prctl.h> header file. */
/*#define HAVE_SYS_PRCTL_H 1*/

/* Define to 1 if you have the <sys/socket.h> header file. */
#define HAVE_SYS_SOCKET_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the `vfork' function. */
#define HAVE_VFORK 1

/* Define to 1 if you have the <vfork.h> header file. */
#define HAVE_VFORK_H 1

/* Define to 1 if `fork' works. */
#define HAVE_WORKING_FORK 1

/* Define to 1 if `vfork' works. */
#define HAVE_WORKING_VFORK 1

/* Define to 1 if you have the <zlib.h> header file. */
#define HAVE_ZLIB_H 1

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#undef LT_OBJDIR // We're not using libtool

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION 1

/* Define to rpl_malloc if the replacement function should be used. */
/*#define malloc*/

/* Define to `int' if <sys/types.h> does not define. */
/*#define pid_t*/

/* Define to rpl_realloc if the replacement function should be used. */
/*#define realloc*/

/* Define to `unsigned int' if <sys/types.h> does not define. */
/*#define size_t*/

/* Define to 1 if we have getifaddrs */
/*#define HAVE_GETIFADDRS*/

/* Define as `fork' if `vfork' does not work. */
//#cmakedefine vfork

/* Define if the inline keyword doesn't exist. */
/*#define inline*/
