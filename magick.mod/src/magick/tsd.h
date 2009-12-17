/*
  Copyright (C) 2005 GraphicsMagick Group
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
*/

#ifndef _MAGICK_TSD_H
#define _MAGICK_TSD_H

/*
  Thread specific data support.
*/

#if defined(HAVE_PTHREAD)
typedef pthread_key_t MagickTsdKey_t;
#elif defined(MSWINDOWS)
typedef DWORD MagickTsdKey_t;
#else
/* FIXME: This type should be based on magick_uintptr_t */
typedef unsigned long * MagickTsdKey_t;
#endif

extern MagickExport MagickPassFail
  MagickTsdKeyCreate(MagickTsdKey_t *key),
  MagickTsdKeyDelete(MagickTsdKey_t key),
  MagickTsdSetSpecific(MagickTsdKey_t key, const void *value);

extern MagickExport void
  *MagickTsdGetSpecific(MagickTsdKey_t key);

#endif /* _MAGICK_TSD_H */
