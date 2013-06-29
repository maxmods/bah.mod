/*
% Copyright (C) 2003 - 2010 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%        DDDD   EEEEE  PPPP   RRRR   EEEEE   CCCC   AAA   TTTTT  EEEEE        %
%        D   D  E      P   P  R   R  E      C      A   A    T    E            %
%        D   D  EEE    PPPPP  RRRR   EEE    C      AAAAA    T    EEE          %
%        D   D  E      P      R R    E      C      A   A    T    E            %
%        DDDD   EEEEE  P      R  R   EEEEE   CCCC  A   A    T    EEEEE        %
%                                                                             %
%                                                                             %
%                      GraphicsMagick Deprecated Methods                      %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                October 2002                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/constitute.h"
#include "magick/pixel_cache.h"
#include "magick/utility.h"
#include "magick/deprecate.h"

#undef LoadImageText
#undef SaveImageText
#undef LoadImagesText
#undef SaveImagesText

extern MagickExport const char
  *LoadImageText,
  *LoadImagesText,
  *SaveImageText,
  *SaveImagesText;

const char
  *LoadImageText = "[%s] Loading image: %lux%lu...  ",
  *LoadImagesText = "[%s] Loading images...  ",
  *SaveImageText = "[%s] Saving image: %lux%lu...  ",
  *SaveImagesText = "[%s] Saving images...  ";

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A c q u i r e C a c h e V i e w                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method AcquireCacheView gets pixels from the in-memory or disk pixel cache
%  as defined by the geometry parameters for read-only access.   A pointer to
%  the pixels is returned if the pixels are transferred, otherwise NULL is
%  returned.
%
%  The format of the AcquireCacheView method is:
%
%      const PixelPacket *AcquireCacheView(const ViewInfo *view,const long x,
%        const long y,const unsigned long columns,const unsigned long rows,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o pixels: Method AcquireCacheView returns a null pointer if an error
%      occurs, otherwise a pointer to the view pixels.
%
%    o view: The address of a structure of type ViewInfo.
%
%    o x,y,columns,rows:  These values define the perimeter of a region of
%      pixels.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport const PixelPacket *
AcquireCacheView(const ViewInfo *view,
                 const long x,const long y,const unsigned long columns,
                 const unsigned long rows,ExceptionInfo *exception)
{
  return AcquireCacheViewPixels(view,x,y,columns,rows,exception);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A c q u i r e M e m o r y                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AcquireMemory() returns a pointer to a block of memory of at least size
%  bytes suitably aligned for any use.  NULL is returned if insufficient
%  memory is available or the requested size is zero.
%
%  The format of the AcquireMemory method is:
%
%      void *AcquireMemory(const size_t size)
%
%  A description of each parameter follows:
%
%    o size: The size of the memory in bytes to allocate.
%
%
*/
MagickExport void *AcquireMemory(const size_t size)
{
  if (IsEventLogging())
    (void) LogMagickEvent(DeprecateEvent,GetMagickModule(),
                          "Method has been deprecated");

  return MagickAllocateMemory(void *,size);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e M e m o r y                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CloneMemory() copies size bytes from memory area source to the
%  destination.  Copying between objects that overlap will take place
%  correctly.  It returns destination.
%
%  The format of the CloneMemory method is:
%
%      void *CloneMemory(void *destination,const void *source,const size_t size)
%
%  A description of each parameter follows:
%
%    o size: The size of the memory in bytes to allocate.
%
%
*/
MagickExport void *CloneMemory(void *destination,const void *source,
  const size_t size)
{
  if (IsEventLogging())
    (void) LogMagickEvent(DeprecateEvent,GetMagickModule(),
                          "Method has been deprecated");

  return MagickCloneMemory(destination,source,size);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t C a c h e V i e w                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetCacheView() gets writeable pixels from the in-memory or disk pixel
%  cache as defined by the geometry parameters.   A pointer to the pixels
%  is returned if the pixels are transferred, otherwise a NULL is returned.
%
%  The format of the GetCacheView method is:
%
%      PixelPacket *GetCacheView(ViewInfo *view,const long x,const long y,
%        const unsigned long columns,const unsigned long rows)
%
%  A description of each parameter follows:
%
%    o pixels: Method GetCacheView returns a null pointer if an error
%      occurs, otherwise a pointer to the view pixels.
%
%    o view: The address of a structure of type ViewInfo.
%
%    o x,y,columns,rows:  These values define the perimeter of a region of
%      pixels.
%
%
*/
MagickExport PixelPacket *
GetCacheView(ViewInfo *view,const long x,const long y,
             const unsigned long columns,const unsigned long rows)
{
  return GetCacheViewPixels(view,x,y,columns,rows,
                            &GetCacheViewImage(view)->exception);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L i b e r a t e M e m o r y                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  LiberateMemory() frees memory that has already been allocated, and
%  NULLs the pointer to it.
%
%  The format of the LiberateMemory method is:
%
%      void LiberateMemory(void **memory)
%
%  A description of each parameter follows:
%
%    o memory: A pointer to a block of memory to free for reuse.
%
%
*/
MagickExport void LiberateMemory(void **memory)
{
  assert(memory != (void **) NULL);

  if (IsEventLogging())
    (void) LogMagickEvent(DeprecateEvent,GetMagickModule(),
                          "Method has been deprecated");

  MagickFreeMemory(*memory);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P o p I m a g e P i x e l s                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PopImagePixels() transfers one or more pixel components from the image pixel
%  cache to a user supplied buffer.   True is returned if the pixels are
%  successfully transferred, otherwise False.
%
%  The format of the PopImagePixels method is:
%
%      unsigned int PopImagePixels(const Image *,const QuantumType quantum,
%        unsigned char *destination)
%
%  A description of each parameter follows:
%
%    o status: Method PopImagePixels returns True if the pixels are
%      successfully transferred, otherwise False.
%
%    o image: The image.
%
%    o quantum: Declare which pixel components to transfer (RGB, RGBA, etc).
%
%    o destination:  The components are transferred to this buffer.
%
%
*/
MagickExport unsigned int PopImagePixels(const Image *image,
  const QuantumType quantum_type,unsigned char *destination)
{
  unsigned int
    quantum_size;
  
  quantum_size=image->depth;

  if (quantum_size <= 8)
    quantum_size=8;
  else if (quantum_size <= 16)
    quantum_size=16;
  else
    quantum_size=32;

  if ( (quantum_type == IndexQuantum) || (quantum_type == IndexAlphaQuantum) )
  {
    if (image->colors <= 256)
      quantum_size=8;
    else if (image->colors <= 65536L)
      quantum_size=16;
    else
      quantum_size=32;
  }

  if (image->logging)
    (void) LogMagickEvent(DeprecateEvent,GetMagickModule(),
                          "Method has been deprecated");
  
  return ExportImagePixelArea(image,quantum_type,quantum_size,destination,0,0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P u s h I m a g e P i x e l s                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PushImagePixels() transfers one or more pixel components from a user
%  supplied buffer into the image pixel cache of an image.  It returns True if
%  the pixels are successfully transferred, otherwise False.
%
%  The format of the PushImagePixels method is:
%
%      unsigned int PushImagePixels(Image *image,
%        const QuantumType quantum_type,
%        const unsigned char *source)
%
%  A description of each parameter follows:
%
%    o status: Method PushImagePixels returns True if the pixels are
%      successfully transferred, otherwise False.
%
%    o image: The image.
%
%    o quantum_type: Declare which pixel components to transfer (red, green,
%      blue, opacity, RGB, or RGBA).
%
%    o source:  The pixel components are transferred from this buffer.
%
*/
MagickExport unsigned int PushImagePixels(Image *image,
  const QuantumType quantum_type,const unsigned char *source)
{
  unsigned int
    quantum_size;

  quantum_size=image->depth;

  if (quantum_size <= 8)
    quantum_size=8;
  else if (quantum_size <= 16)
    quantum_size=16;
  else
    quantum_size=32;

  if ( (quantum_type == IndexQuantum) || (quantum_type == IndexAlphaQuantum) )
  {
    if (image->colors <= 256)
      quantum_size=8;
    else if (image->colors <= 65536L)
      quantum_size=16;
    else
      quantum_size=32;
  }

  if (image->logging)
    (void) LogMagickEvent(DeprecateEvent,GetMagickModule(),
                          "Method has been deprecated");

  return ImportImagePixelArea(image,quantum_type,quantum_size,source,0,0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a c q u i r e M e m o r y                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReacquireMemory() changes the size of the memory and returns a
%  pointer to the (possibly moved) block.  The contents will be unchanged
%  up to the lesser of the new and old sizes.
%
%  The format of the ReacquireMemory method is:
%
%      void ReacquireMemory(void **memory,const size_t size)
%
%  A description of each parameter follows:
%
%    o memory: A pointer to a memory allocation.  On return the pointer
%      may change but the contents of the original allocation will not.
%
%    o size: The new size of the allocated memory.
%
%
*/
MagickExport void ReacquireMemory(void **memory,const size_t size)
{
  assert(memory != (void **) NULL);

  if (IsEventLogging())
    (void) LogMagickEvent(DeprecateEvent,GetMagickModule(),
                          "Method has been deprecated");

  MagickReallocMemory(void*,*memory,size);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t C a c h e V i e w                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetCacheView() gets pixels from the in-memory or disk pixel cache as
%  defined by the geometry parameters.   A pointer to the pixels is returned
%  if the pixels are transferred, otherwise a NULL is returned.
%
%  The format of the SetCacheView method is:
%
%      PixelPacket *SetCacheView(ViewInfo *view,const long x,const long y,
%        const unsigned long columns,const unsigned long rows)
%
%  A description of each parameter follows:
%
%    o view: The address of a structure of type ViewInfo.
%
%    o x,y,columns,rows:  These values define the perimeter of a region of
%      pixels.
%
%
*/
MagickExport PixelPacket *
SetCacheView(ViewInfo *view,const long x,const long y,
             const unsigned long columns,const unsigned long rows)
{
  return SetCacheViewPixels(view,x,y,columns,rows,
                            &GetCacheViewImage(view)->exception);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S y n c C a c h e V i e w                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SyncCacheView() saves the view pixels to the in-memory or disk cache.
%  The method returns MagickPass if the pixel region is synced, otherwise
%  MagickFail.
%
%  The format of the SyncCacheView method is:
%
%      MagickPassFail SyncCacheView(ViewInfo *view)
%
%  A description of each parameter follows:
%
%    o view: The address of a structure of type ViewInfo.
%
%
*/
MagickExport MagickPassFail
SyncCacheView(ViewInfo *view)
{
  return SyncCacheViewPixels(view,&GetCacheViewImage(view)->exception);
}
