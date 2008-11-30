/*
  Copyright (C) 2003 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  Methods to lock and unlock semaphores.
*/
#ifndef _MAGICK_SEMAPHORE_H
#define _MAGICK_SEMAPHORE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

typedef struct _SemaphoreInfo SemaphoreInfo;

extern MagickExport SemaphoreInfo
  *AllocateSemaphoreInfo(void);

extern MagickExport unsigned int
  LockSemaphoreInfo(SemaphoreInfo *),
  UnlockSemaphoreInfo(SemaphoreInfo *);

extern MagickExport void
  AcquireSemaphoreInfo(SemaphoreInfo **),
  DestroySemaphore(void),
  DestroySemaphoreInfo(SemaphoreInfo **),
  InitializeSemaphore(void),
  LiberateSemaphoreInfo(SemaphoreInfo **);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
