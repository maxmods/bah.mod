/*
  Copyright (C) 2003, 2008 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  Magic methods.
*/
#ifndef _MAGICK_MAGIC_H
#define _MAGICK_MAGIC_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Method declarations.
*/
extern MagickExport MagickPassFail
  GetMagickFileFormat(const unsigned char *header,const size_t header_length,
     char *format,const size_t format_length,ExceptionInfo *exception),
  InitializeMagicInfo(ExceptionInfo *exception),
  ListMagicInfo(FILE *file,ExceptionInfo *exception);

extern MagickExport void
  DestroyMagicInfo(void);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
