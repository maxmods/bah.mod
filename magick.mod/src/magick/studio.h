/*
  Copyright (C) 2003, 2007 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  GraphicsMagick Application Programming Interface declarations.
*/
#ifndef _MAGICK_STUDIO_H
#define _MAGICK_STUDIO_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Note that the WIN32 and WIN64 definitions are provided by the build
  configuration rather than the compiler.  Definitions available from
  the Windows compiler are _WIN32 and _WIN64.
*/
#if defined(WIN32) || defined(WIN64)
#  define MSWINDOWS
#endif /* defined(WIN32) || defined(WIN64) */

#if !defined(MSWINDOWS)
#  define POSIX
#endif /* !defined(MSWINDOWS) */

/*
  Private functions and types which are not part of the published API
  should only be exposed when MAGICK_IMPLEMENTATION is defined.
*/
#define MAGICK_IMPLEMENTATION 1

#include "magick/magick_config.h"
#if defined(__cplusplus) || defined(c_plusplus)
#  undef inline
#endif /* defined(__cplusplus) || defined(c_plusplus) */

/*
  Support library symbol prefixing
*/
#if defined(PREFIX_MAGICK_SYMBOLS)
#  include "magick/symbols.h"
#endif /* defined(PREFIX_MAGICK_SYMBOLS) */

#if !defined(const)
  /*
    For some stupid reason the zlib headers define 'const' to nothing
    under AIX unless STDC is defined.
  */
#  define STDC
#endif /* !defined(const) */

/**
 ** Borland C++ Builder DLL compilation defines
 **/
#if defined(__BORLANDC__) && defined(_DLL)
#  pragma message("BCBMagick lib DLL export interface")
#  define _MAGICKDLL_
#  define _MAGICKLIB_
#  undef BuildMagickModules
#  define SupportMagickModules
#endif 

#if defined(MSWINDOWS) && !defined(__CYGWIN__)
#  if defined(_MT) && defined(_DLL) && !defined(_MAGICKDLL_) && !defined(_LIB)
#    define _MAGICKDLL_
#  endif
#  if defined(_MAGICKDLL_)
#    if defined(_VISUALC_)
#      pragma warning( disable: 4273 )  /* Disable the dll linkage warnings */
#    endif
#    if !defined(_MAGICKLIB_)
#      define MagickExport  __declspec(dllimport)
#      if defined(_VISUALC_)
#        pragma message( "Magick lib DLL import interface" )
#      endif
#    else
#      define MagickExport  __declspec(dllexport)
#      if defined(_VISUALC_)
#         pragma message( "Magick lib DLL export interface" )
#      endif
#    endif
#  else
#    define MagickExport
#    if defined(_VISUALC_)
#      pragma message( "Magick lib static interface" )
#    endif
#  endif
#  if defined(_DLL) && !defined(_LIB)
#    define ModuleExport  __declspec(dllexport)
#    if defined(_VISUALC_)
#      pragma message( "Magick module DLL export interface" ) 
#    endif
#  else
#    define ModuleExport
#    if defined(_VISUALC_)
#      pragma message( "Magick module static interface" ) 
#    endif
#  endif
#  define MagickGlobal __declspec(thread)
#  if defined(_VISUALC_)
#    pragma warning(disable : 4018)
#    pragma warning(disable : 4244)
#    pragma warning(disable : 4142)
#    pragma warning(disable : 4800)
#    pragma warning(disable : 4786)
#    pragma warning(disable : 4996) /* function deprecation warnings */
#  endif
#else
#  define MagickExport
#  define ModuleExport
#  define MagickGlobal
#endif

/*
  Enable use of numeric message IDs and a translation table in order
  to support multiple locales.
 */
#define MAGICK_IDBASED_MESSAGES 1
#if defined(MAGICK_IDBASED_MESSAGES)
#  include "magick/locale_c.h"
#endif

/*
  For the Windows Visual C++ DLL build, use a Windows resource based
  message lookup table (i.e. use FormatMessage()).
 */
#if 0
  /*
    Currently disabled since feature only seems to work from
    a DLL
  */
#  if ((defined(MSWINDOWS) && defined(_DLL)) && !defined(__MINGW32__))
#    define MAGICK_WINDOWS_MESSAGE_TABLES 1
#  endif
#endif

#define MagickSignature  0xabacadabUL
#define MaxTextExtent  2053

#include <stdarg.h>
#include <stdio.h>
#if defined(MSWINDOWS) && defined(_DEBUG)
#  define _CRTDBG_MAP_ALLOC
#endif
#include <stdlib.h>
#if !defined(MSWINDOWS)
#  include <unistd.h>
#else
#  include <direct.h>
#  if !defined(HAVE_STRERROR)
#    define HAVE_STRERROR
#  endif
#endif

/*
  Use fseeko() and ftello() if they are available since they use
  'off_t' rather than 'long'.  It is wrong to use fseeko() and
  ftello() only on systems with special LFS support since some systems
  (e.g. FreeBSD) support a 64-bit off_t by default.
*/
#if defined(HAVE_FSEEKO)
#  define fseek  fseeko
#  define ftell  ftello
#endif

#if !defined(ExtendedSignedIntegralType)
#  define ExtendedSignedIntegralType magick_int64_t
#endif
#if !defined(ExtendedUnsignedIntegralType)
#  define ExtendedUnsignedIntegralType magick_uint64_t
#endif

#define MagickPassFail unsigned int
#define MagickPass     1
#define MagickFail     0

#define MagickBool     unsigned int
#define MagickTrue     1
#define MagickFalse    0

#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <signal.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>

#if defined(HAVE_FTIME)
#  include <sys/timeb.h>
#endif

#if defined(POSIX)
#  if defined(HAVE_SYS_NDIR_H) || defined(HAVE_SYS_DIR_H) || defined(HAVE_NDIR_H)
#    define dirent direct
#    define NAMLEN(dirent) (dirent)->d_namlen
#    if defined(HAVE_SYS_NDIR_H)
#      include <sys/ndir.h>
#    endif
#    if defined(HAVE_SYS_DIR_H)
#      include <sys/dir.h>
#    endif
#    if defined(HAVE_NDIR_H)
#      include <ndir.h>
#    endif
#  else
#    include <dirent.h>
#    define NAMLEN(dirent) strlen((dirent)->d_name)
#  endif
#  include <sys/wait.h>
#  include <pwd.h>
#endif

#if !defined(S_ISDIR)
#  define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
#endif

#if !defined(S_ISREG)
#  define S_ISREG(mode) (((mode) & S_IFMT) == S_IFREG)
#endif

#include "magick/magick_types.h"
#include "magick/image.h"
#include "magick/list.h"
#include "magick/memory.h"

#if !defined(MSWINDOWS)
#  include <sys/time.h>
#  if defined(HAVE_SYS_TIMES_H)
#    include <sys/times.h>
#  endif
#endif


#if defined(POSIX)
# include "magick/unix_port.h"
#endif /* defined(POSIX) */

#if defined(MSWINDOWS)
# include "magick/nt_base.h"
#endif /* defined(MSWINDOWS) */

#if defined(HAVE_MMAP_FILEIO) && !defined(MSWINDOWS)
# include <sys/mman.h>
#endif

#if defined(HAVE_PTHREAD)
# include <pthread.h>
#endif

#if defined(HAVE_POLL)
# include <sys/poll.h>
#endif

/*
  OpenMP support requires version 2.0 (March 2002) or later.
*/
#if defined(_OPENMP) && (_OPENMP >= 200203)
#  include <omp.h>
#  define HAVE_OPENMP 1
#endif

#undef index
#undef pipe

/*
  If TRIO library is used, remap snprintf and vsnprintf to TRIO equivalents.
*/
#if defined(HasTRIO)
#  include <trio.h>
#  define snprintf trio_snprintf
#  define HAVE_SNPRINTF 1
#  define vsnprintf trio_vsnprintf
#  define HAVE_VSNPRINTF 1
#endif

/*
  Provide prototypes for several functions which are detected to be
  available, but which do not provide a prototype due to interface
  standards conformance (or a bug).
*/

#if defined(HAVE_PREAD) && defined(HAVE_DECL_PREAD) && !HAVE_DECL_PREAD
ssize_t pread(int fildes, void *buf, size_t nbyte, off_t offset);
#endif

#if defined(HAVE_PWRITE) && defined(HAVE_DECL_PWRITE) && !HAVE_DECL_PWRITE
ssize_t pwrite(int fildes, const void *buf, size_t nbyte, off_t offset);
#endif

#if defined(HAVE_STRLCPY) && defined(HAVE_DECL_STRLCPY) && !HAVE_DECL_STRLCPY
extern size_t strlcpy(char *dst, const char *src, size_t dstsize);
#endif

#if defined(HAVE_VSNPRINTF) && defined(HAVE_DECL_VSNPRINTF) && !HAVE_DECL_VSNPRINTF
extern int vsnprintf(char *s, size_t n, const char *format, va_list ap);
#endif

/*
  Some 64-bit int support.
*/
#if defined(HAVE_STRTOLL) && (SIZEOF_SIGNED_LONG < 8)
#  define MagickStrToL64(str,endptr,base) (strtoll(str,endptr,base))
#else
#  define MagickStrToL64(str,endptr,base) ((magick_int64_t) strtol(str,endptr,base))
#endif
#if defined(HAVE_ATOLL) && (SIZEOF_SIGNED_LONG < 8)
#  define MagickAtoL64(str) (atoll(str))
#else
#  define MagickAtoL64(str) ((magick_int64_t) atol(str))
#endif

/*
  Review these platform specific definitions.
*/
#if defined(POSIX)
#  define DirectorySeparator  "/"
#  define DirectoryListSeparator  ':'
#  define EditorOptions  " -title \"Edit Image Comment\" -e vi"
#  define Exit  exit
#  define IsBasenameSeparator(c)  ((c) == '/')
#  define PreferencesDefaults  "~/."
#  define ProcessPendingEvents(text)
#  define ReadCommandlLine(argc,argv)
#  define SetNotifyHandlers
#endif

#if defined(MSWINDOWS)
#  define DirectorySeparator  "\\"
#  define DirectoryListSeparator  ';'
#  define EditorOptions ""
#  define IsBasenameSeparator(c)  (((c) == '/') || ((c) == '\\'))
#  define ProcessPendingEvents(text)
#  if !defined(PreferencesDefaults)
#     define PreferencesDefaults  "~\\."
#  endif /* PreferencesDefaults */
#  define ReadCommandlLine(argc,argv)
#  define SetNotifyHandlers \
    SetErrorHandler(NTErrorHandler); \
    SetWarningHandler(NTWarningHandler)
#  undef sleep
#  define sleep(seconds)  Sleep(seconds*1000)
#  if !defined(HAVE_TIFFCONF_H)
#    define HAVE_TIFFCONF_H
#  endif
#endif /* defined(MSWINDOWS) */


/*
  Define declarations.
*/
#define AbsoluteValue(x)  ((x) < 0 ? -(x) : (x))
#define False  0
#define DegreesToRadians(x) (MagickPI*(x)/180.0)
#define MagickIncarnate(x)  InitializeMagick(x)
#define MagickEpsilon  1.0e-12
#define MagickPI  3.14159265358979323846264338327950288419716939937510
#define MagickSQ2PI 2.50662827463100024161235523934010416269302368164062
#define Max(x,y)  (((x) > (y)) ? (x) : (y))
#define Min(x,y)  (((x) < (y)) ? (x) : (y))
#define QuantumTick(i,span) \
  ((((i) % ((Max(101,span)-1)/100)) == 0) || \
    ((magick_int64_t) (i) == ((magick_int64_t) (span)-1)))
#define RadiansToDegrees(x) (180.0*(x)/MagickPI)
#define ScaleColor5to8(x)  (((x) << 3) | ((x) >> 2))
#define ScaleColor6to8(x)  (((x) << 2) | ((x) >> 4))
#define Swap(x,y) ((x)^=(y), (y)^=(x), (x)^=(y))
#define True  1

#define DefineNameToString(value) #value
#define DefineValueToString(define) DefineNameToString(define)

/*
  3D effects.
*/
#define AccentuateModulate  ScaleCharToQuantum(80)
#define HighlightModulate  ScaleCharToQuantum(125)
#define ShadowModulate  ScaleCharToQuantum(135)
#define DepthModulate  ScaleCharToQuantum(185)
#define TroughModulate  ScaleCharToQuantum(110)

/*
  Define system symbols if not already defined.
*/
#if !defined(STDIN_FILENO)
#  define STDIN_FILENO  0x00
#endif

#if !defined(O_BINARY)
#  define O_BINARY  0x00
#endif

#if !defined(MAP_FAILED)
#  define MAP_FAILED      ((void *) -1)
#endif

#if defined(HasLTDL) || ( defined(MSWINDOWS) && defined(_DLL) )
#  define SupportMagickModules
#endif

#if defined(_MAGICKMOD_)
#  undef BuildMagickModules
#  define BuildMagickModules
#endif


/*
  I/O defines.
*/
#if defined(MSWINDOWS) && !defined(Windows95) && !defined(__BORLANDC__)
  /* Windows '95 and Borland C do not support _lseeki64 */
#  define MagickSeek(file,offset,whence)  _lseeki64(file,offset,whence)
#  define MagickTell(file) _telli64(file)
#else
#  define MagickSeek(file,offset,whence)  lseek(file,offset,whence)
#  define MagickTell(file) tell(file)
#endif

#if !defined(HAVE_POPEN) && defined(HAVE__POPEN)
#  define HAVE_POPEN 1
#  define popen _popen
#endif /* !defined(HAVE_POPEN) && defined(HAVE__POPEN) */

#if !defined(HAVE_PCLOSE) && defined(HAVE__PCLOSE)
#  define HAVE_PCLOSE 1
#  define pclose _pclose
#endif /* !defined(HAVE_PCLOSE) && defined(HAVE__PCLOSE) */

#if defined(HAVE__EXIT)
#  define SignalHandlerExit _exit
#else
#  define SignalHandlerExit Exit
#endif /* defined(HAVE__EXIT) */

/*
  OpenMP function null replacements if not using OpenMP.
*/
#if !defined(HAVE_OPENMP)
#  undef omp_get_max_threads
#  define omp_get_max_threads() 1
#  undef omp_get_num_threads
#  define omp_get_num_threads() 1
#  undef omp_get_thread_num
#  define omp_get_thread_num() 0
#  undef omp_set_num_threads
#  define omp_set_num_threads(nthreads)
#endif /* !defined(HAVE_OPENMP) */


/*
  Image const declarations.
*/
extern MagickExport const char
  *BackgroundColor,
  *BorderColor,
  *DefaultTileFrame,
  *DefaultTileGeometry,
  *DefaultTileLabel,
  *ForegroundColor,
  *HighlightColor,
  *LoadImageText,
  *LoadImagesText,
  *MatteColor,
  *PSDensityGeometry,
  *PSPageGeometry,
  *SaveImageText,
  *SaveImagesText;

extern MagickExport const unsigned long
  DefaultCompressionQuality;

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#endif /* ifndef _MAGICK_STUDIO_H */
