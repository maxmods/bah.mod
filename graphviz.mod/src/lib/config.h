/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Define to one of `_getb67', `GETB67', `getb67' for Cray-2 and Cray-YMP
   systems. This function is required for `alloca.c' support on those systems.
   */
#undef CRAY_STACKSEG_END

/* Define to 1 if using `alloca.c'. */
#undef C_ALLOCA

/* Define for Darwin-style shared library names. */
#undef DARWIN_DYLIB

/* Default DPI. */
#define DEFAULT_DPI 96

/* Path to TrueType fonts. */
#ifndef WIN32
#ifndef _APPLE_
#define DEFAULT_FONTPATH "/usr/X11R6/lib/X11/fonts/TrueType:/usr/X11R6/lib/X11/fonts/truetype:/usr/X11R6/lib/X11/fonts/TTF:/usr/share/fonts/TrueType:/usr/share/fonts/truetype:/usr/openwin/lib/X11/fonts/TrueType:/usr/X11R6/lib/X11/fonts/Type1"
#   define PATHSEPARATOR ":"
#else
#endif
/*apple*/
#define DEFAULT_FONTPATH "~/Library/Fonts:/Library/Fonts:/Network/Library/Fonts:/System/Library/Fonts"
#   define PATHSEPARATOR ":"
#else
/*win32*/
#   define DEFAULT_FONTPATH    "C:/WINDOWS/FONTS;C:/WINNT/Fonts;C:/winnt/fonts"
#   define PATHSEPARATOR ";"
#endif

/* Define if you want DIGCOLA */
#undef DIGCOLA

/* Define if you want on-demand plugin loading */
#undef ENABLE_LTDL

/* always defined to indicate that i18n is enabled */
#undef ENABLE_NLS

/* Define if you want shared lib executables */
#undef ENABLE_SHARED

/* Define if you want statically linked executables */
#define ENABLE_STATIC 1

/* Gettext package. */
#undef GETTEXT_PACKAGE

/* Compatibility version number for plugins. */
#undef GVPLUGIN_VERSION

/* Define to 1 if you have `alloca', as a function or macro. */
#define HAVE_ALLOCA 1

/* Define to 1 if you have <alloca.h> and it should be used (not on Ultrix).
   */
#define HAVE_ALLOCA_H 1

/* Define to 1 if you have the `bind_textdomain_codeset' function. */
#undef HAVE_BIND_TEXTDOMAIN_CODESET

/* Define to 1 if compiler supports bool */
#ifndef WIN32
#define HAVE_BOOL 1
#endif

/* Define to 1 if you have the `cbrt' function. */
#ifdef __APPLE__
#define HAVE_CBRT 1
#endif

/* Define to 1 if you have the `dcgettext' function. */
#undef HAVE_DCGETTEXT

/* Define to 1 if you have the <dirent.h> header file, and it defines `DIR'.
   */
#define HAVE_DIRENT_H 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the `drand48' function. */
#ifndef WIN32
#define HAVE_DRAND48 1
#endif

/* Define if errno externs are declared */
#define HAVE_ERRNO_DECL 1

/* Define to 1 if you have the <errno.h> header file. */
#define HAVE_ERRNO_H 1

/* Define to 1 if you have the `feenableexcept' function. */
#ifndef __APPLE__
#define HAVE_FEENABLEEXCEPT 1
#endif

/* Define to 1 if you have the <fenv.h> header file. */
#define HAVE_FENV_H 1

/* Define to 1 if you have the `fesetenv' function. */
#define HAVE_FESETENV 1

/* Define if FILE structure provides _cnt */
#undef HAVE_FILE_CNT

/* Define if FILE structure provides _IO_read_end */
#ifndef __APPLE__
#define HAVE_FILE_IO_READ_END 1
#endif

/* Define if FILE structure provides _next */
#undef HAVE_FILE_NEXT

/* Define if FILE structure provides _r */
#ifdef __APPLE__
#define HAVE_FILE_R 1
#endif

/* Define to 1 if you have the <float.h> header file. */
#define HAVE_FLOAT_H 1

/* Define if you have the fontconfig library */
#undef HAVE_FONTCONFIG

/* Define to 1 if you have the <fontconfig/fontconfig.h> header file. */
#undef HAVE_FONTCONFIG_FONTCONFIG_H

/* Define to 1 if you have the <fpu_control.h> header file. */
#undef HAVE_FPU_CONTROL_H

/* Define to 1 if you have the <ft2build.h> header file. */
#undef HAVE_FT2BUILD_H

/* Define if the GD library has the GD_FONTCONFIG feature */
#undef HAVE_GD_FONTCONFIG

/* Define if the GD library has the GD_FREETYPE feature */
#undef HAVE_GD_FREETYPE

/* Define if the GD library has the GD_GIF feature */
#undef HAVE_GD_GIF

/* Define if the GD library has the GD_JPEG feature */
#undef HAVE_GD_JPEG

/* Define if the GD library has the GD_PNG feature */
#undef HAVE_GD_PNG

/* Define to 1 if you have the `getenv' function. */
#define HAVE_GETENV 1

/* Define if getopt externs are declared */
#undef HAVE_GETOPT_DECL

/* Define to 1 if you have the <getopt.h> header file. */
#undef HAVE_GETOPT_H

/* Define to 1 if you have the `getrusage' function. */
#undef HAVE_GETRUSAGE

/* Define if the GNU gettext() function is already present or preinstalled. */
#undef HAVE_GETTEXT

/* Define if you have the gnomeui library */
#undef HAVE_GNOMEUI

/* Define if you have the gtk+ library */
#undef HAVE_GTK

/* Define if you have the iconv() function. */
#undef HAVE_ICONV

/* Define to 1 if you have the <iconv.h> header file. */
#undef HAVE_ICONV_H

/* Define if <iconv.h> defines iconv_t. */
#undef HAVE_ICONV_T_DEF

/* Define if intptr_t is declared */
#define HAVE_INTPTR_T 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define if your <locale.h> file defines LC_MESSAGES. */
#define HAVE_LC_MESSAGES 1

/* Define if you have the EXPAT library */
#define HAVE_LIBEXPAT 1

/* Define if you have the FONTCONFIG library */
#undef HAVE_LIBFONTCONFIG

/* Define if you have the FREETYPE2 library */
#undef HAVE_LIBFREETYPE

/* Define if either internal or external GD library is availabel */
#undef HAVE_LIBGD

/* Define if the LIBGEN library has the basename feature */
#undef HAVE_LIBGEN

/* Define if you have the JPEG library */
#undef HAVE_LIBJPEG

/* Define if you have the PNG library */
#undef HAVE_LIBPNG

/* Define if you have the XPM library */
#undef HAVE_LIBXPMFORLEFTY

/* Define if you have the Z library */
#undef HAVE_LIBZ

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define to 1 if you have the <locale.h> header file. */
#ifdef __APPLE_
#define HAVE_LOCALE_H 1

/* Define to 1 if you have the `lrand48' function. */
#define HAVE_LRAND48 1
#endif

/* Define to 1 if you have the `lsqrt' function. */
#undef HAVE_LSQRT

/* Define to 1 if you have the <malloc.h> header file. */
#ifndef __APPLE__
#define HAVE_MALLOC_H 1
#endif

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <ndir.h> header file, and it defines `DIR'. */
#undef HAVE_NDIR_H

/* Define if you have the pango library with cairo support */
#undef HAVE_PANGOCAIRO

/* Define to 1 if you have the <pthread.h> header file. */
#undef HAVE_PTHREAD_H

/* Define to 1 if you have the <search.h> header file. */
#define HAVE_SEARCH_H 1

/* Define to 1 if you have the `setenv' function. */
#ifndef WIN32
#define HAVE_SETENV 1
#endif

/* Define to 1 if you have the `setmode' function. */
#undef HAVE_SETMODE

/* Define if libm provides a *working* sincos function */
#undef HAVE_SINCOS

/* Define to 1 if you have the `srand48' function. */
#ifdef __APPLE__
#undef HAVE_SRAND48
#endif

/* Define to 1 if you have the <stdarg.h> header file. */
#define HAVE_STDARG_H 1

/* Define to 1 if stdbool.h conforms to C99. */
#define HAVE_STDBOOL_H 1

/* Define to 1 if you have the <stddef.h> header file. */
#define HAVE_STDDEF_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strcasecmp' function. */
#define HAVE_STRCASECMP 1

/* Define to 1 if you have the `strerror' function. */
#define HAVE_STRERROR 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strncasecmp' function. */
#define HAVE_STRNCASECMP 1

/* Define to 1 if you have the `strtoll' function. */
#define HAVE_STRTOLL 1

/* Define to 1 if you have the `strtoul' function. */
#define HAVE_STRTOUL 1

/* Define to 1 if you have the `strtoull' function. */
#define HAVE_STRTOULL 1

/* Define to 1 if you have struct dioattr */
#undef HAVE_STRUCT_DIOATTR

/* Define to 1 if you have the <sys/dir.h> header file, and it defines `DIR'.
   */
#undef HAVE_SYS_DIR_H

/* Define to 1 if you have the <sys/fpu.h> header file. */
#undef HAVE_SYS_FPU_H

/* Define to 1 if you have the <sys/inotify.h> header file. */
#undef HAVE_SYS_INOTIFY_H

/* Define to 1 if you have the <sys/ioctl.h> header file. */
#ifndef WIN32
#define HAVE_SYS_IOCTL_H 1

/* Define to 1 if you have the <sys/mman.h> header file. */
#define HAVE_SYS_MMAN_H 1
#endif

/* Define to 1 if you have the <sys/ndir.h> header file, and it defines `DIR'.
   */
#undef HAVE_SYS_NDIR_H

/* Define to 1 if you have the <sys/select.h> header file. */
#define HAVE_SYS_SELECT_H 1

/* Define to 1 if you have the <sys/socket.h> header file. */
#define HAVE_SYS_SOCKET_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/times.h> header file. */
#define HAVE_SYS_TIMES_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <termios.h> header file. */
#define HAVE_TERMIOS_H 1

/* Define to 1 if you have the <time.h> header file. */
#define HAVE_TIME_H 1

/* Define to 1 if you have the <tkInt.h> header file. */
#undef HAVE_TKINT_H

/* Define to 1 if you have the <tk.h> header file. */
#undef HAVE_TK_H

/* Define to 1 if you have the `uname' function. */
#define HAVE_UNAME 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the <values.h> header file. */
#define HAVE_VALUES_H 1

/* Define to 1 if you have the `vsnprintf' function. */
#define HAVE_VSNPRINTF 1

/* Define to 1 if you have the <X11/Intrinsic.h> header file. */
#undef HAVE_X11_INTRINSIC_H

/* Define to 1 if you have the <X11/Xaw/Text.h> header file. */
#undef HAVE_X11_XAW_TEXT_H

/* Define to 1 if the system has the type `_Bool'. */
#ifdef __APPLE__
#define HAVE__BOOL 1
#endif

/* Define to 1 if you have the `_sysconf' function. */
#undef HAVE__SYSCONF

/* Define to 1 if you have the `__freadable' function. */
#undef HAVE___FREADABLE

/* Define as const if the declaration of iconv() needs const. */
#undef ICONV_CONST

/* Define if you want IPSEPCOLA */
#undef IPSEPCOLA

/* Define if no fpu error exception handling is required. */
#undef NO_FPERR

/* Alias PostScript font names to URW type 1 fonts */
#undef NO_POSTSCRIPT_ALIAS

/* Name of package */
#define PACKAGE "graphviz"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME "graphviz"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "graphviz 2.12"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "graphviz"

/* Define to the version of this package. */
#define PACKAGE_VERSION "2.12"

/* Path separator character. */
#define PATHSEPARATOR ":"

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run-time.
	STACK_DIRECTION > 0 => grows toward higher addresses
	STACK_DIRECTION < 0 => grows toward lower addresses
	STACK_DIRECTION = 0 => direction of growth unknown */
/* #undef STACK_DIRECTION */

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#define TIME_WITH_SYS_TIME 1

/* Version number of package */
#define VERSION "2.12"

/* Define if you want the old codegens */
#undef WITH_CODEGENS

/* Define to 1 if the X Window System is missing or not being used. */
/* #undef X_DISPLAY_MISSING */

/* Define to 1 if `lex' declares `yytext' as a `char *' by default, not a
   `char[]'. */
#define YYTEXT_POINTER 1

/* Define to `int' if <sys/types.h> doesn't define. */
/* #undef gid_t */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef mode_t */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef pid_t */

/* Define to `unsigned' if <sys/types.h> does not define. */
/* #undef size_t */

/* Define to unsigned if socklet_t is missing */
/* #undef socklen_t */

/* Define to `int' if <sys/types.h> doesn't define. */
/* #undef uid_t */


#ifdef WIN32
#define F_GETFL 3
#define F_SETFL 4
#endif
