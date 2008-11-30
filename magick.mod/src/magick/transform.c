/*
% Copyright (C) 2003 - 2008 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
% Copyright 1991-1999 E. I. du Pont de Nemours and Company
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%       TTTTT  RRRR    AAA   N   N  SSSSS  FFFFF   OOO   RRRR   M   M         %
%         T    R   R  A   A  NN  N  SS     F      O   O  R   R  MM MM         %
%         T    RRRR   AAAAA  N N N   SSS   FFF    O   O  RRRR   M M M         %
%         T    R R    A   A  N  NN     SS  F      O   O  R R    M   M         %
%         T    R  R   A   A  N   N  SSSSS  F       OOO   R  R   M   M         %
%                                                                             %
%                                                                             %
%                   GraphicsMagick Image Transform Methods                    %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
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
#include "magick/color.h"
#include "magick/composite.h"
#include "magick/monitor.h"
#include "magick/pixel_cache.h"
#include "magick/resize.h"
#include "magick/transform.h"
#include "magick/utility.h"
#include "magick/log.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C h o p I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Chop() removes a region of an image and collapses the image to occupy the
%  removed portion.
%
%  The format of the ChopImage method is:
%
%      Image *ChopImage(const Image *image,const RectangleInfo *chop_info
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o chop_info: Define the region of the image to chop.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *ChopImage(const Image *image,const RectangleInfo *chop_info,
                              ExceptionInfo *exception)
{
#define ChopImageText  "[%s] Chop image..."

  Image
    *chop_image;

  unsigned long
    row_count=0;

  long
    y;

  RectangleInfo
    clone_info;

  MagickPassFail
    status=MagickPass;

  /*
    Check chop geometry.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  assert(chop_info != (RectangleInfo *) NULL);
  if (((chop_info->x+(long) chop_info->width) < 0) ||
      ((chop_info->y+(long) chop_info->height) < 0) ||
      (chop_info->x > (long) image->columns) ||
      (chop_info->y > (long) image->rows))
    ThrowImageException3(OptionError,GeometryDoesNotContainImage,
                         UnableToChopImage);
  clone_info=(*chop_info);
  if ((clone_info.x+(long) clone_info.width) > (long) image->columns)
    clone_info.width=(unsigned long) ((long) image->columns-clone_info.x);
  if ((clone_info.y+(long) clone_info.height) > (long) image->rows)
    clone_info.height=(unsigned long) ((long) image->rows-clone_info.y);
  if (clone_info.x < 0)
    {
      clone_info.width-=(unsigned long) (-clone_info.x);
      clone_info.x=0;
    }
  if (clone_info.y < 0)
    {
      clone_info.height-=(unsigned long) (-clone_info.y);
      clone_info.y=0;
    }
  /*
    Initialize chop image attributes.
  */
  chop_image=CloneImage(image,image->columns-clone_info.width,
                        image->rows-clone_info.height,False,exception);
  if (chop_image == (Image *) NULL)
    return((Image *) NULL);

  /*
    Extract chop image.
  */
#if defined(HAVE_OPENMP)  && !defined(DisableSlowOpenMP)
#  pragma omp parallel for schedule(dynamic,4) shared(row_count, status)
#endif
  for (y=0; y < (long) clone_info.y; y++)
    {
      register const PixelPacket
        *p;
    
      register const IndexPacket
        *indexes;
    
      register IndexPacket
        *chop_indexes;
    
      register long
        x;
    
      register PixelPacket
        *q;

      MagickBool
        thread_status;

      thread_status=status;
      if (thread_status == MagickFail)
        continue;

      p=AcquireImagePixels(image,0,y,image->columns,1,exception);
      q=SetImagePixelsEx(chop_image,0,y,chop_image->columns,1,exception);
      if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
        thread_status=MagickFail;

      if (thread_status != MagickFail)
        {
          indexes=AccessImmutableIndexes(image);
          chop_indexes=AccessMutableIndexes(chop_image);
          for (x=0; x < (long) image->columns; x++)
            {
              if ((x < clone_info.x) || (x >= (long) (clone_info.x+clone_info.width)))
                {
                  if ((indexes != (const IndexPacket *) NULL) &&
                      (chop_indexes != (IndexPacket *) NULL))
                    *chop_indexes++=indexes[x];
                  *q=(*p);
                  q++;
                }
              p++;
            }
          if (!SyncImagePixelsEx(chop_image,exception))
            thread_status=MagickFail;
        }
#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  pragma omp critical
#endif
      {
        row_count++;
        if (QuantumTick(row_count,chop_image->rows))
          if (!MagickMonitorFormatted(row_count,chop_image->rows,exception,
                                      ChopImageText,image->filename))
            thread_status=MagickFail;
          
        if (thread_status == MagickFail)
          status=MagickFail;
      }
    }
  /*
    Extract chop image.
  */
#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  pragma omp parallel for schedule(dynamic,4) shared(row_count, status)
#endif
  for (y=0; y < (long) (image->rows-(clone_info.y+clone_info.height)); y++)
    {
      register const PixelPacket
        *p;
    
      register const IndexPacket
        *indexes;
    
      register IndexPacket
        *chop_indexes;
    
      register long
        x;
    
      register PixelPacket
        *q;

      MagickBool
        thread_status;

      thread_status=status;
      if (thread_status == MagickFail)
        continue;

      p=AcquireImagePixels(image,0,clone_info.y+clone_info.height+y,image->columns,1,exception);
      q=SetImagePixelsEx(chop_image,0,clone_info.y+y,chop_image->columns,1,exception);
      if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
        thread_status=MagickFail;

      if (thread_status != MagickFail)
        {
          indexes=AccessImmutableIndexes(image);
          chop_indexes=AccessMutableIndexes(chop_image);
          for (x=0; x < (long) image->columns; x++)
            {
              if ((x < clone_info.x) || (x >= (long) (clone_info.x+clone_info.width)))
                {
                  if ((indexes != (const IndexPacket *) NULL) &&
                      (chop_indexes != (IndexPacket *) NULL))
                    *chop_indexes++=indexes[x];
                  *q=(*p);
                  q++;
                }
              p++;
            }
          if (!SyncImagePixelsEx(chop_image,exception))
            thread_status=MagickFail;
        }
#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  pragma omp critical
#endif
      {
        row_count++;
        if (QuantumTick(row_count,chop_image->rows))
          if (!MagickMonitorFormatted(row_count,chop_image->rows,exception,
                                      ChopImageText,image->filename))
            thread_status=MagickFail;
          
        if (thread_status == MagickFail)
          status=MagickFail;
      }
    }
  if (row_count < chop_image->rows)
    {
      DestroyImage(chop_image);
      return((Image *) NULL);
    }
  chop_image->is_grayscale=image->is_grayscale;
  return(chop_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     C o a l e s c e I m a g e s                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CoalesceImages() composites a set of images while respecting any page
%  offsets and disposal methods.  GIF, MIFF, and MNG animation sequences
%  typically start with an image background and each subsequent image
%  varies in size and offset.  CoalesceImages() returns a new sequence
%  where each image in the sequence is the same size as the first and
%  composited with the next image in the sequence.
%
%  The format of the CoalesceImages method is:
%
%      Image *CoalesceImages(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image sequence.
%
%    o exception: Return any errors or warnings in this structure.
%
*/
MagickExport Image *CoalesceImages(const Image *image,ExceptionInfo *exception)
{
  Image
    *coalesce_image,
    *previous_image;

  register const Image
    *next;

  /*
    Coalesce the image sequence.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if (image->next == (Image *) NULL)
    ThrowImageException3(ImageError,ImageSequenceIsRequired,
      UnableToCoalesceImage);
  /*
    Clone first image in sequence.
  */
  coalesce_image=CloneImage(image,0,0,True,exception);
  if (coalesce_image == (Image *) NULL)
    return((Image *) NULL);
  (void) memset(&coalesce_image->page,0,sizeof(RectangleInfo));
  previous_image=coalesce_image;
  /*
    Coalesce image.
  */
  for (next=image->next; next != (Image *) NULL; next=next->next)
  {
    switch (next->dispose)
    {
      case UndefinedDispose:
      case NoneDispose:
      {
        coalesce_image->next=CloneImage(coalesce_image,0,0,True,exception);
        if (coalesce_image->next != (Image *) NULL)
          previous_image=coalesce_image->next;
        break;
      }
      case BackgroundDispose:
      {
        coalesce_image->next=CloneImage(coalesce_image,0,0,True,exception);
        if (coalesce_image->next != (Image *) NULL)
          (void) SetImage(coalesce_image->next,OpaqueOpacity);
        break;
      }
      case PreviousDispose:
      default:
      {
        coalesce_image->next=CloneImage(previous_image,0,0,True,exception);
        break;
      }
    }
    if (coalesce_image->next == (Image *) NULL)
      {
        DestroyImageList(coalesce_image);
        return((Image *) NULL);
      }
    coalesce_image->next->previous=coalesce_image;
    coalesce_image=coalesce_image->next;
    coalesce_image->delay=next->delay;
    coalesce_image->start_loop=next->start_loop;
    (void) CompositeImage(coalesce_image,next->matte ? OverCompositeOp :
      CopyCompositeOp,next,next->page.x,next->page.y);
  }
  while (coalesce_image->previous != (Image *) NULL)
    coalesce_image=coalesce_image->previous;
  return(coalesce_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C r o p I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Use CropImage() to extract a region of the image starting at the offset
%  defined by geometry.  As a special feature, if the geometry "0x0" is
%  is passed, GetImageBoundingBox() is used to locate the edges of the
%  image and the image is cropped ("trimmed") to that boundary.
%
%  The format of the CropImage method is:
%
%      Image *CropImage(const Image *image,const RectangleInfo *geometry,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o geometry: Define the region of the image to crop with members
%      x, y, width, and height.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *CropImage(const Image *image,const RectangleInfo *geometry,
                              ExceptionInfo *exception)
{
  Image
    *crop_image;

  unsigned long
    row_count=0;

  long
    y;

  RectangleInfo
    page;

  MagickPassFail
    status=MagickPass;

  /*
    Check crop geometry.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(geometry != (const RectangleInfo *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if ((geometry->width != 0) || (geometry->height != 0))
    {
      if (((geometry->x+(long) geometry->width) < 0) ||
          ((geometry->y+(long) geometry->height) < 0) ||
          (geometry->x >= (long) image->columns) ||
          (geometry->y >= (long) image->rows))
        ThrowImageException(OptionError,GeometryDoesNotContainImage,
                            MagickMsg(ResourceLimitError,UnableToCropImage));
    }
  page=(*geometry);
  if ((page.width != 0) || (page.height != 0))
    {
      if ((page.x+(long) page.width) > (long) image->columns)
        page.width=image->columns-page.x;
      if ((page.y+(long) page.height) > (long) image->rows)
        page.height=image->rows-page.y;
      if (page.x < 0)
        {
          page.width+=page.x;
          page.x=0;
        }
      if (page.y < 0)
        {
          page.height+=page.y;
          page.y=0;
        }
    }
  else
    {
      /*
        Set bounding box to the image dimensions.
      */
      page=GetImageBoundingBox(image,exception);
      page.width+=geometry->x*2;
      page.height+=geometry->y*2;
      page.x-=geometry->x;
      if (page.x < 0)
        page.x=0;
      page.y-=geometry->y;
      if (page.y < 0)
        page.y=0;
      if ((((long) page.width+page.x) > (long) image->columns) ||
          (((long) page.height+page.y) > (long) image->rows))
        ThrowImageException(OptionError,GeometryDoesNotContainImage,
                            MagickMsg(ResourceLimitError,UnableToCropImage));
    }
  if ((page.width == 0) || (page.height == 0))
    ThrowImageException(OptionError,GeometryDimensionsAreZero,
                        MagickMsg(ResourceLimitError,UnableToCropImage));
  if ((page.width == image->columns) && (page.height == image->rows) &&
      (page.x == 0) && (page.y == 0))
    return(CloneImage(image,0,0,True,exception));
  /*
    Initialize crop image attributes.
  */
  crop_image=CloneImage(image,page.width,page.height,True,exception);
  if (crop_image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Extract crop image.
  */
  crop_image->page=page;
  if ((geometry->width == 0) || (geometry->height == 0))
    (void) memset(&crop_image->page,0,sizeof(RectangleInfo));
#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  pragma omp parallel for schedule(dynamic,4) shared(row_count, status)
#endif
  for (y=0; y < (long) crop_image->rows; y++)
    {
      const PixelPacket
        *p;

      const IndexPacket
        *indexes;

      IndexPacket
        *crop_indexes;

      PixelPacket
        *q;

      MagickBool
        thread_status;

      thread_status=status;
      if (thread_status == MagickFail)
        continue;

      p=AcquireImagePixels(image,page.x,page.y+y,crop_image->columns,1,exception);
      q=SetImagePixelsEx(crop_image,0,y,crop_image->columns,1,exception);
      if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
        thread_status=MagickFail;

      if (thread_status != MagickFail)
        {
          (void) memcpy(q,p,crop_image->columns*sizeof(PixelPacket));
          indexes=AccessImmutableIndexes(image);
          crop_indexes=AccessMutableIndexes(crop_image);
          if ((indexes != (const IndexPacket *) NULL) &&
              (crop_indexes != (IndexPacket *) NULL))
            (void) memcpy(crop_indexes,indexes,crop_image->columns*
                          sizeof(IndexPacket));
          if (!SyncImagePixelsEx(crop_image,exception))
            thread_status=MagickFail;
        }
#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  pragma omp critical
#endif
      {
        row_count++;
        if (QuantumTick(row_count,crop_image->rows))
          if (!MagickMonitorFormatted(row_count,crop_image->rows-1,exception,
                                      "[%s] Crop %lux%lu+%ld+%ld image...",
                                      crop_image->filename,
                                      crop_image->columns,crop_image->rows,
                                      page.x,page.y))
            thread_status=MagickFail;
          
        if (thread_status == MagickFail)
          status=MagickFail;
      }
    }
  if (row_count < crop_image->rows)
    {
      DestroyImage(crop_image);
      return((Image *) NULL);
    }
  crop_image->is_grayscale=image->is_grayscale;
  return(crop_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     D e c o n s t r u c t I m a g e s                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DeconstructImages() compares each image with the next in a sequence and
%  returns the maximum bounding region of any pixel differences it discovers.
%
%  The format of the DeconstructImages method is:
%
%      Image *DeconstructImages(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *DeconstructImages(const Image *image,
  ExceptionInfo *exception)
{
  Image
    *crop_image,
    *crop_next,
    *deconstruct_image;

  long
    y;

  RectangleInfo
    *bounds;

  register const Image
    *next;

  register const PixelPacket
    *p;

  register long
    i,
    x;

  register PixelPacket
    *q;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if (image->next == (Image *) NULL)
    ThrowImageException3(ImageError,ImageSequenceIsRequired,
      UnableToDeconstructImageSequence);
  /*
    Ensure the image are the same size.
  */
  for (next=image; next != (Image *) NULL; next=next->next)
  {
    if ((next->columns != image->columns) || (next->rows != image->rows))
      ThrowImageException(OptionError,ImagesAreNotTheSameSize,
        MagickMsg(ImageError,UnableToDeconstructImageSequence));
  }
  /*
    Allocate memory.
  */
  bounds=MagickAllocateMemory(RectangleInfo *,
    GetImageListLength(image)*sizeof(RectangleInfo));
  if (bounds == (RectangleInfo *) NULL)
    ThrowImageException(ResourceLimitError,MemoryAllocationFailed,
      MagickMsg(ImageError,UnableToDeconstructImageSequence));
  /*
    Compute the bounding box for each next in the sequence.
  */
  i=0;
  for (next=image->next; next != (const Image *) NULL; next=next->next)
  {
    /*
      Set bounding box to the next dimensions.
    */
    for (x=0; x < (long) next->columns; x++)
    {
      p=AcquireImagePixels(next,x,0,1,next->rows,exception);
      q=GetImagePixels(next->previous,x,0,1,next->previous->rows);
      if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
        break;
      for (y=0; y < (long) next->rows; y++)
      {
        if (!FuzzyColorMatch(p,q,next->fuzz))
          break;
        p++;
        q++;
      }
      if (y < (long) next->rows)
        break;
    }
    bounds[i].x=x;
    for (y=0; y < (long) next->rows; y++)
    {
      p=AcquireImagePixels(next,0,y,next->columns,1,exception);
      q=GetImagePixels(next->previous,0,y,next->previous->columns,1);
      if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
        break;
      for (x=0; x < (long) next->columns; x++)
      {
        if (!FuzzyColorMatch(p,q,next->fuzz))
          break;
        p++;
        q++;
      }
      if (x < (long) next->columns)
        break;
    }
    bounds[i].y=y;
    for (x=(long) next->columns-1; x >= 0; x--)
    {
      p=AcquireImagePixels(next,x,0,1,next->rows,exception);
      q=GetImagePixels(next->previous,x,0,1,next->previous->rows);
      if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
        break;
      for (y=0; y < (long) next->rows; y++)
      {
        if (!FuzzyColorMatch(p,q,next->fuzz))
          break;
        p++;
        q++;
      }
      if (y < (long) next->rows)
        break;
    }
    bounds[i].width=x-bounds[i].x+1;
    for (y=(long) next->rows-1; y >= 0; y--)
    {
      p=AcquireImagePixels(next,0,y,next->columns,1,exception);
      q=GetImagePixels(next->previous,0,y,next->previous->columns,1);
      if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
        break;
      for (x=0; x < (long) next->columns; x++)
      {
        if (!FuzzyColorMatch(p,q,next->fuzz))
          break;
        p++;
        q++;
      }
      if (x < (long) next->columns)
        break;
    }
    bounds[i].height=y-bounds[i].y+1;
    i++;
  }
  /*
    Clone first image in sequence.
  */
  deconstruct_image=CloneImage(image,0,0,True,exception);
  if (deconstruct_image == (Image *) NULL)
    {
      MagickFreeMemory(bounds);
      return((Image *) NULL);
    }
  /*
    Deconstruct the image sequence.
  */
  i=0;
  for (next=image->next; next != (Image *) NULL; next=next->next)
  {
    crop_image=CloneImage(next,0,0,True,exception);
    if (crop_image == (Image *) NULL)
      break;
    crop_next=CropImage(crop_image,&bounds[i++],exception);
    DestroyImage(crop_image);
    if (crop_next == (Image *) NULL)
      break;
    deconstruct_image->next=crop_next;
    crop_next->previous=deconstruct_image;
    deconstruct_image=deconstruct_image->next;
  }
  MagickFreeMemory(bounds);
  while (deconstruct_image->previous != (Image *) NULL)
    deconstruct_image=deconstruct_image->previous;
  if (next != (Image *) NULL)
    {
      DestroyImageList(deconstruct_image);
      return((Image *) NULL);
    }
  return(deconstruct_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     F l a t t e n I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method FlattenImage merges a sequence of images.  This is useful for
%  combining Photoshop layers into a single image.
%
%  The format of the FlattenImage method is:
%
%      Image *FlattenImage(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image sequence.
%
%    o exception: Return any errors or warnings in this structure.
%
*/
MagickExport Image *FlattenImages(const Image *image,ExceptionInfo *exception)
{
  Image
    *flatten_image;

  register const Image
    *next;

  /*
    Flatten the image sequence.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if (image->next == (Image *) NULL)
    ThrowImageException3(ImageError,ImageSequenceIsRequired,
      UnableToFlattenImage);
  /*
    Clone first image in sequence.
  */
  flatten_image=CloneImage(image,0,0,True,exception);
  if (flatten_image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Flatten image.
  */
  for (next=image->next; next != (Image *) NULL; next=next->next)
    (void) CompositeImage(flatten_image,next->compose,next,next->page.x,
      next->page.y);
  return(flatten_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   F l i p I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  FlipImage() creates a vertical mirror image by reflecting the pixels
%  around the central x-axis.
%
%  The format of the FlipImage method is:
%
%      Image *FlipImage(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *FlipImage(const Image *image,ExceptionInfo *exception)
{
#define FlipImageText  "[%s] Flip image..."

  Image
    *flip_image;

  unsigned long
    row_count=0;

  long
    y;

  MagickPassFail
    status=MagickPass;

  /*
    Initialize flip image attributes.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  flip_image=CloneImage(image,image->columns,image->rows,True,exception);
  if (flip_image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Flip each row.
  */
#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  pragma omp parallel for schedule(dynamic,4) shared(row_count, status)
#endif
  for (y=0; y < (long) flip_image->rows; y++)
    {
      const PixelPacket
        *p;

      const IndexPacket
        *indexes;

      IndexPacket
        *flip_indexes;

      PixelPacket
        *q;

      MagickBool
        thread_status;

      thread_status=status;
      if (thread_status == MagickFail)
        continue;

      p=AcquireImagePixels(image,0,y,image->columns,1,exception);
      q=SetImagePixelsEx(flip_image,0,(long) (flip_image->rows-y-1),
                         flip_image->columns,1,exception);
      if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
        thread_status=MagickFail;

      if (thread_status != MagickFail)
        {
          (void) memcpy(q,p,flip_image->columns*sizeof(PixelPacket));
          indexes=AccessImmutableIndexes(image);
          flip_indexes=AccessMutableIndexes(flip_image);
          if ((indexes != (IndexPacket *) NULL) &&
              (flip_indexes != (IndexPacket *) NULL))
            (void) memcpy(flip_indexes,indexes,image->columns*sizeof(IndexPacket));
          if (!SyncImagePixelsEx(flip_image,exception))
            thread_status=MagickFail;
        }
#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  pragma omp critical
#endif
      {
        row_count++;
        if (QuantumTick(row_count,flip_image->rows))
          if (!MagickMonitorFormatted(row_count,flip_image->rows,exception,
                                      FlipImageText,image->filename))
            thread_status=MagickFail;
          
        if (thread_status == MagickFail)
          status=MagickFail;
      }
    }
  if (row_count < flip_image->rows)
    {
      DestroyImage(flip_image);
      return((Image *) NULL);
    }
  flip_image->is_grayscale=image->is_grayscale;
  return(flip_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   F l o p I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  FlopImage() creates a horizontal mirror image by reflecting the pixels
%  around the central y-axis.
%
%  The format of the FlopImage method is:
%
%      Image *FlopImage(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *FlopImage(const Image *image,ExceptionInfo *exception)
{
#define FlopImageText  "[%s] Flop image..."

  Image
    *flop_image;

  unsigned long
    row_count=0;

  long
    y;

  MagickPassFail
    status=MagickPass;

  /*
    Initialize flop image attributes.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  flop_image=CloneImage(image,image->columns,image->rows,True,exception);
  if (flop_image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Flop each row.
  */
#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  pragma omp parallel for schedule(dynamic,4) shared(row_count, status)
#endif
  for (y=0; y < (long) flop_image->rows; y++)
    {
      register const IndexPacket
        *indexes;

      register IndexPacket
        *flop_indexes;

      register const PixelPacket
        *p;

      register long
        x;

      register PixelPacket
        *q;

      MagickBool
        thread_status;

      thread_status=status;
      if (thread_status == MagickFail)
        continue;

      p=AcquireImagePixels(image,0,y,image->columns,1,exception);
      q=SetImagePixelsEx(flop_image,0,y,flop_image->columns,1,exception);
      if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
        thread_status=MagickFail;

      if (thread_status != MagickFail)
        {
          indexes=AccessImmutableIndexes(image);
          flop_indexes=AccessMutableIndexes(flop_image);
          q+=flop_image->columns;
          for (x=0; x < (long) flop_image->columns; x++)
            {
              if ((indexes != (const IndexPacket *) NULL) &&
                  (flop_indexes != (IndexPacket *) NULL))
                flop_indexes[flop_image->columns-x-1]=indexes[x];
              q--;
              *q=(*p);
              p++;
            }
          if (!SyncImagePixelsEx(flop_image,exception))
            thread_status=MagickFail;
        }
#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  pragma omp critical
#endif
      {
        row_count++;
        if (QuantumTick(row_count,flop_image->rows))
          if (!MagickMonitorFormatted(row_count,flop_image->rows,exception,
                                      FlopImageText,image->filename))
            thread_status=MagickFail;
          
        if (thread_status == MagickFail)
          status=MagickFail;
      }
    }
  if (row_count < flop_image->rows)
    {
      DestroyImage(flop_image);
      return((Image *) NULL);
    }
  flop_image->is_grayscale=image->is_grayscale;
  return(flop_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%     M o s a i c I m a g e s                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MosaicImages() inlays an image sequence to form a single coherent picture.
%  It returns a single image with each image in the sequence composited at
%  the location defined by the page member of the image structure.
%
%  The format of the MosaicImage method is:
%
%      Image *MosaicImages(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *MosaicImages(const Image *image,ExceptionInfo *exception)
{
#define MosaicImageText  "[%s] Create image mosaic..."

  Image
    *mosaic_image;

  RectangleInfo
    page;

  register const Image
    *next;

  unsigned int
    scene,
    status;

  /*
    Determine mosaic bounding box.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if (image->next == (Image *) NULL)
    ThrowImageException3(ImageError,ImageSequenceIsRequired,
      UnableToCreateImageMosaic);
  page.width=image->columns;
  page.height=image->rows;
  page.x=0;
  page.y=0;
  for (next=image; next != (Image *) NULL; next=next->next)
  {
    page.x=next->page.x;
    page.y=next->page.y;
    if ((next->columns+page.x) > page.width)
      page.width=next->columns+page.x;
    if (next->page.width > page.width)
      page.width=next->page.width;
    if ((next->rows+page.y) > page.height)
      page.height=next->rows+page.y;
    if (next->page.height > page.height)
      page.height=next->page.height;
  }
  /*
    Allocate mosaic image.
  */
  mosaic_image=AllocateImage((ImageInfo *) NULL);
  if (mosaic_image == (Image *) NULL)
    return((Image *) NULL);
  mosaic_image->columns=page.width;
  mosaic_image->rows=page.height;
  (void) SetImage(mosaic_image,OpaqueOpacity);
  /*
    Initialize colormap.
  */
  scene=0;
  for (next=image; next != (Image *) NULL; next=next->next)
  {
    (void) CompositeImage(mosaic_image,CopyCompositeOp,next,next->page.x,
      next->page.y);
    status=MagickMonitorFormatted(scene++,GetImageListLength(image),
                                  exception,MosaicImageText,image->filename);
    if (status == False)
      break;
  }
  return(mosaic_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R o l l I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  RollImage() offsets an image as defined by x_offset and y_offset.
%
%  The format of the RollImage method is:
%
%      Image *RollImage(const Image *image,const long x_offset,
%        const long y_offset,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o x_offset: The number of columns to roll in the horizontal direction.
%
%    o y_offset: The number of rows to roll in the vertical direction.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *RollImage(const Image *image,const long x_offset,
                              const long y_offset,ExceptionInfo *exception)
{
  Image
    *roll_image;

  RectangleInfo
    offset;

  /*
    Initialize roll image attributes.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  roll_image=CloneImage(image,image->columns,image->rows,True,exception);
  if (roll_image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Roll image.
  */
  offset.x=x_offset;
  offset.y=y_offset;
  while (offset.x < 0)
    offset.x+=image->columns;
  while (offset.x >= (long) image->columns)
    offset.x-=image->columns;
  while (offset.y < 0)
    offset.y+=image->rows;
  while (offset.y >= (long) image->rows)
    offset.y-=image->rows;

  /* Top left quadrant */
  (void) CompositeImageRegion(CopyCompositeOp,0,offset.x,offset.y,image,
                              image->columns-offset.x,image->rows-offset.y,
                              roll_image,0,0,exception);

  /* Top right quadrant */
  (void) CompositeImageRegion(CopyCompositeOp,0,image->columns-offset.x,offset.y,image,
                              0,image->rows-offset.y,
                              roll_image,offset.x,0,exception);

  /* Bottom left quadrant */
  (void) CompositeImageRegion(CopyCompositeOp,0,offset.x,image->rows-offset.y,image,
                              image->columns-offset.x,0,
                              roll_image,0,offset.y,exception);

  /* Bottom right quadrant */
  (void) CompositeImageRegion(CopyCompositeOp,0,image->columns-offset.x,image->rows-offset.y,image,
                              0,0,
                              roll_image,offset.x,offset.y,exception);

  roll_image->is_grayscale=image->is_grayscale;
  return(roll_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S h a v e I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ShaveImage shaves pixels from the image edges.  It allocates the
%  memory necessary for the new Image structure and returns a pointer to the
%  new image.
%
%  The format of the ShaveImage method is:
%
%      Image *ShaveImage(const Image *image,const RectangleInfo *shave_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o shave_image: Method ShaveImage returns a pointer to the shaved
%      image.  A null image is returned if there is a memory shortage or
%      if the image width or height is zero.
%
%    o image: The image.
%
%    o shave_info: Specifies a pointer to a RectangleInfo which defines the
%      region of the image to shave.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *ShaveImage(const Image *image,
  const RectangleInfo *shave_info,ExceptionInfo *exception)
{
  RectangleInfo
    geometry;

  if (((2*shave_info->width) >= image->columns) ||
      ((2*shave_info->height) >= image->rows))
    ThrowImageException(OptionError,GeometryDoesNotContainImage,
      MagickMsg(ResourceLimitError,UnableToShaveImage));
  SetGeometry(image,&geometry);
  geometry.width-=2*shave_info->width;
  geometry.height-=2*shave_info->height;
  geometry.x=(long) shave_info->width;
  geometry.y=(long) shave_info->height;
  return(CropImage(image,&geometry,exception));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   T r a n s f o r m I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             % 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  TransformImage() is a convenience method that behaves like ResizeImage() or
%  CropImage() but accepts scaling and/or cropping information as a region
%  geometry specification.  If the operation fails, the original image handle
%  is returned.
%
%  The format of the TransformImage method is:
%
%      void TransformImage(Image **image,const char *crop_geometry,
%        const char *image_geometry)
%
%  A description of each parameter follows:
%
%    o image: The image The transformed image is returned as this parameter.
%
%    o crop_geometry: A crop geometry string.  This geometry defines a
%      subregion of the image to crop.
%
%    o image_geometry: An image geometry string.  This geometry defines the
%      final size of the image.
%
%
*/
MagickExport void TransformImage(Image **image,const char *crop_geometry,
  const char *image_geometry)
{
  Image
    *resize_image,
    *transform_image;

  int
    flags;

  RectangleInfo
    geometry;

  assert(image != (Image **) NULL);
  assert((*image)->signature == MagickSignature);
  transform_image=(*image);
  if (crop_geometry != (const char *) NULL)
    {
      Image
        *crop_image;

      RectangleInfo
        geometry;

      /*
        Crop image to a user specified size.
      */
      crop_image=(Image *) NULL;
      flags=GetImageGeometry(transform_image,crop_geometry,False,&geometry);
      if ((geometry.width == 0) || (geometry.height == 0) ||
          ((flags & XValue) != 0) || ((flags & YValue) != 0) ||
          (flags & PercentValue))
        crop_image=CropImage(transform_image,&geometry,&(*image)->exception);
      else
        if ((transform_image->columns > geometry.width) ||
            (transform_image->rows > geometry.height))
          {
            Image
              *next;

            long
              x,
              y;

            unsigned long
              height,
              width;

            /*
              Crop repeatedly to create uniform subimages.
            */
            width=geometry.width;
            height=geometry.height;
            next=(Image *) NULL;
            for (y=0; y < (long) transform_image->rows; y+=height)
            {
              for (x=0; x < (long) transform_image->columns; x+=width)
              {
                geometry.width=width;
                geometry.height=height;
                geometry.x=x;
                geometry.y=y;
                next=CropImage(transform_image,&geometry,&(*image)->exception);
                if (next == (Image *) NULL)
                  break;
                if (crop_image == (Image *) NULL)
                  crop_image=next;
                else
                  {
                    next->previous=crop_image;
                    crop_image->next=next;
                    crop_image=crop_image->next;
                  }
              }
              if (next == (Image *) NULL)
                break;
            }
          }
      if (crop_image != (Image *) NULL)
        {
          DestroyImage(transform_image);
          while (crop_image->previous != (Image *) NULL)
            crop_image=crop_image->previous;
          transform_image=crop_image;
        }
      *image=transform_image;
    }
  if (image_geometry == (const char *) NULL)
    return;
  /*
    Scale image to a user specified size.
  */
  SetGeometry(transform_image,&geometry);
  flags=GetMagickGeometry(image_geometry,&geometry.x,&geometry.y,
    &geometry.width,&geometry.height);
  if ((transform_image->columns == geometry.width) &&
      (transform_image->rows == geometry.height))
    return;
  /*
    Resize image.
  */
  resize_image=ZoomImage(transform_image,geometry.width,geometry.height,
    &(*image)->exception);
  if (resize_image == (Image *) NULL)
    return;
  DestroyImage(transform_image);
  transform_image=resize_image;
  *image=transform_image;
}
