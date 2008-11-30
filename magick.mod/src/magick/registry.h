/*
  Copyright (C) 2003 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  Magick registry methods.
*/
#ifndef _MAGICK_REGISTRY_H
#define _MAGICK_REGISTRY_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Enum declarations.
*/
typedef enum
{
  UndefinedRegistryType,
  ImageRegistryType,
  ImageInfoRegistryType
} RegistryType;

/*
  Typedef declarations.
*/
typedef struct _RegistryInfo
{
  long
    id;

  RegistryType
    type;

  void
    *blob;

  size_t
    length;

  unsigned long
    signature;

  struct _RegistryInfo
    *previous,
    *next;
} RegistryInfo;

/*
  Magick registry methods.
*/
extern MagickExport Image
  *GetImageFromMagickRegistry(const char *,long *id,ExceptionInfo *);

extern MagickExport long
  SetMagickRegistry(const RegistryType,const void *,const size_t,
    ExceptionInfo *);

extern MagickExport unsigned int
  DeleteMagickRegistry(const long);

extern MagickExport void
  DestroyMagickRegistry(void),
  *GetMagickRegistry(const long,RegistryType *,size_t *,ExceptionInfo *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
