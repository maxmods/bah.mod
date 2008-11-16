/* src/raptor_config.h.  Generated from raptor_config.h.in by configure.  */
/* src/raptor_config.h.in.  Generated from configure.ac by autoheader.  */

/* does expat crash when it sees an initial UTF8 BOM? */
/* #undef EXPAT_UTF8_BOM_CRASH */

/* vsnprint has C99 compatible return value */
#define HAVE_C99_VSNPRINTF 1

/* Have curl/curl.h */
#define HAVE_CURL_CURL_H 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <dmalloc.h> header file. */
/* #undef HAVE_DMALLOC_H */

/* Define to 1 if you have the <errno.h> header file. */
#define HAVE_ERRNO_H 1

/* Define to 1 if you have the <expat.h> header file. */
#define HAVE_EXPAT_H 1

/* Define to 1 if you have the <fetch.h> header file. */
/* #undef HAVE_FETCH_H */

/* Define to 1 if you have the `getopt' function. */
#define HAVE_GETOPT 1

/* Define to 1 if you have the <getopt.h> header file. */
#define HAVE_GETOPT_H 1

/* Define to 1 if you have the `getopt_long' function. */
#define HAVE_GETOPT_LONG 1

/* INN parsedate function present */
/* #undef HAVE_INN_PARSEDATE */

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the `isascii' function. */
#define HAVE_ISASCII 1

/* Define to 1 if you have the <libxml/hash.h> header file. */
#define HAVE_LIBXML_HASH_H 1

/* Define to 1 if you have the <libxml/HTMLparser.h> header file. */
#define HAVE_LIBXML_HTMLPARSER_H 1

/* Define to 1 if you have the <libxml/nanohttp.h> header file. */
#define HAVE_LIBXML_NANOHTTP_H 1

/* Define to 1 if you have the <libxml/parser.h> header file. */
#define HAVE_LIBXML_PARSER_H 1

/* Define to 1 if you have the <libxml/SAX2.h> header file. */
#define HAVE_LIBXML_SAX2_H 1

/* Define to 1 if you have the <libxslt/xslt.h> header file. */
#define HAVE_LIBXSLT_XSLT_H 1

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define to 1 if you have the <math.h> header file. */
#define HAVE_MATH_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Raptor raptor_parse_date available */
/* #undef HAVE_RAPTOR_PARSE_DATE */

/* have round() in libm */
#define HAVE_ROUND 1

/* Define to 1 if you have the `setjmp' function. */
#define HAVE_SETJMP 1

/* Define to 1 if you have the <setjmp.h> header file. */
#define HAVE_SETJMP_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strcasecmp' function. */
#define HAVE_STRCASECMP 1

/* Define to 1 if you have the `stricmp' function. */
/* #undef HAVE_STRICMP */

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* have trunc() in libm */
#define HAVE_TRUNC 1

/* Define to 1 if the system has the type `u16'. */
/* #undef HAVE_U16 */

/* Define to 1 if the system has the type `u8'. */
/* #undef HAVE_U8 */

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the `vsnprintf' function. */
#define HAVE_VSNPRINTF 1

/* Define to 1 if you have the `xmlCtxtUseOptions' function. */
#define HAVE_XMLCTXTUSEOPTIONS 1

/* Define to 1 if you have the <xmlparse.h> header file. */
/* #undef HAVE_XMLPARSE_H */

/* Define to 1 if you have the `xmlSAX2InternalSubset' function. */
#define HAVE_XMLSAX2INTERNALSUBSET 1

/* need 'extern int optind' declaration? */
/* #undef NEED_OPTIND_DECLARATION */

/* Name of package */
#define PACKAGE "raptor"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "http://bugs.librdf.org/"

/* Define to the full name of this package. */
#define PACKAGE_NAME "Raptor RDF Parser"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "Raptor RDF Parser 1.4.18"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "raptor"

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.4.18"

/* does libxml struct xmlEntity have a field etype */
#define RAPTOR_LIBXML_ENTITY_ETYPE 1

/* does libxml struct xmlEntity have a field name_length */
/* #undef RAPTOR_LIBXML_ENTITY_NAME_LENGTH */

/* does libxml have HTML_PARSE_NONET */
#define RAPTOR_LIBXML_HTML_PARSE_NONET 1

/* does libxml xmlSAXHandler have externalSubset field */
#define RAPTOR_LIBXML_XMLSAXHANDLER_EXTERNALSUBSET 1

/* does libxml xmlSAXHandler have initialized field */
#define RAPTOR_LIBXML_XMLSAXHANDLER_INITIALIZED 1

/* does libxml have XML_PARSE_NONET */
#define RAPTOR_LIBXML_XML_PARSE_NONET 1

/* Provide a Unicode NFC check */
#define RAPTOR_NFC_CHECK 1

/* Building GRDDL parser */
#define RAPTOR_PARSER_GRDDL 1

/* Building guess parser */
#define RAPTOR_PARSER_GUESS 1

/* Building Notation 3 parser */
/* #undef RAPTOR_PARSER_N3 */

/* Building N-Triples parser */
#define RAPTOR_PARSER_NTRIPLES 1

/* Building RDFA parser */
#define RAPTOR_PARSER_RDFA 1

/* Building RDF/XML parser */
#define RAPTOR_PARSER_RDFXML 1

/* Building RSS Tag Soup parser */
#define RAPTOR_PARSER_RSS 1

/* Building TRiG parser */
#define RAPTOR_PARSER_TRIG 1

/* Building Turtle parser */
#define RAPTOR_PARSER_TURTLE 1

/* Building Atom 1.0 serializer */
#define RAPTOR_SERIALIZER_ATOM 1

/* Building GraphViz DOT serializer */
#define RAPTOR_SERIALIZER_DOT 1

/* Building JSON serializer */
#define RAPTOR_SERIALIZER_JSON 1

/* Building N-Triples serializer */
#define RAPTOR_SERIALIZER_NTRIPLES 1

/* Building RDF/XML serializer */
#define RAPTOR_SERIALIZER_RDFXML 1

/* Building RDF/XML-abbreviated serializer */
#define RAPTOR_SERIALIZER_RDFXML_ABBREV 1

/* Building RSS 1.0 serializer */
#define RAPTOR_SERIALIZER_RSS_1_0 1

/* Building Turtle serializer */
#define RAPTOR_SERIALIZER_TURTLE 1

/* Release version as a decimal */
#define RAPTOR_VERSION_DECIMAL 10418

/* Major version number */
#define RAPTOR_VERSION_MAJOR 1

/* Minor version number */
#define RAPTOR_VERSION_MINOR 4

/* Release version number */
#define RAPTOR_VERSION_RELEASE 18

/* Have libcurl WWW library */
#define RAPTOR_WWW_LIBCURL 1

/* Have libfetch WWW library */
/* #undef RAPTOR_WWW_LIBFETCH */

/* Have libxml available as a WWW library */
/* #undef RAPTOR_WWW_LIBXML */

/* No WWW library */
/* #undef RAPTOR_WWW_NONE */

/* Check XML 1.1 Names */
/* #undef RAPTOR_XML_1_1 */

/* Use expat XML parser */
/* #undef RAPTOR_XML_EXPAT */

/* Use libxml XML parser */
#define RAPTOR_XML_LIBXML 1

/* The size of `unsigned char', as computed by sizeof. */
#define SIZEOF_UNSIGNED_CHAR 1

/* The size of `unsigned int', as computed by sizeof. */
#define SIZEOF_UNSIGNED_INT 4

/* The size of `unsigned long', as computed by sizeof. */
#define SIZEOF_UNSIGNED_LONG 4

/* The size of `unsigned short', as computed by sizeof. */
#define SIZEOF_UNSIGNED_SHORT 2

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#define TIME_WITH_SYS_TIME 1

/* Version number of package */
#define VERSION "1.4.18"

/* Define to 1 if your processor stores words with the most significant byte
   first (like Motorola and SPARC, unlike Intel and VAX). */
/* #undef WORDS_BIGENDIAN */

/* Define to 1 if `lex' declares `yytext' as a `char *' by default, not a
   `char[]'. */
#define YYTEXT_POINTER 1

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */
