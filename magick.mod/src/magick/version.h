/*
  Copyright (C) 2003 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
  Copyright 1991-1999 E. I. du Pont de Nemours and Company
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  GraphicsMagick version and copyright.
*/
#ifndef _MAGICK_VERSION_H
#define _MAGICK_VERSION_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Define declarations.

  MagickLibVersion and MagickLibVersionNumber are defined differently
  than they are in ImageMagick. The three fields are based on library
  interface versioning.  Each field in MagickLibVersion is one byte.
  The most significant field (third byte from the right) defines the
  library major interface, which is incremented whenever the library
  ABI changes incompatibly with preceding versions. The second field
  identifies an interface in a series of upward-compatible interfaces
  with the same major interface (such as when only new functions have)
  been added. The least significant field specifies the revision across
  100% compatible interfaces.

  MagickLibVersionText provides a simple human-readable string for
  identifying the release.
*/
#define MagickPackageName "GraphicsMagick"
#define MagickCopyright  "Copyright (C) 2002-2008 GraphicsMagick Group.\nAdditional copyrights and licenses apply to this software.\nSee http://www.GraphicsMagick.org/www/Copyright.html for details."
#define MagickLibVersion  0x040000
#define MagickLibVersionText  "1.4"
#define MagickLibVersionNumber 4,0,0
#define MagickChangeDate   "20081128"
#define MagickReleaseDate  "unreleased"


#if (QuantumDepth == 8)
#define MagickQuantumDepth  "Q8"
#elif (QuantumDepth == 16)
#define MagickQuantumDepth  "Q16"
#elif (QuantumDepth == 32)
#define MagickQuantumDepth  "Q32"
#else
# error Unsupported quantum depth.
#endif

#define MagickVersion MagickPackageName " " MagickLibVersionText " " \
  MagickReleaseDate " " MagickQuantumDepth " " MagickWebSite
#define MagickWebSite  "http://www." MagickPackageName ".org/"

/*
  Method declarations.
*/
extern MagickExport const char
  *GetMagickCopyright(void),
  *GetMagickVersion(unsigned long *),
  *GetMagickWebSite(void);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
