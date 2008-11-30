/*
% Copyright (C) 2003 - 2008 GraphicsMagick Group
% Copyright (C) 2003 ImageMagick Studio
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
%                     IIIII  M   M   AAA    GGGG  EEEEE                       %
%                       I    MM MM  A   A  G      E                           %
%                       I    M M M  AAAAA  G  GG  EEE                         %
%                       I    M   M  A   A  G   G  E                           %
%                     IIIII  M   M  A   A   GGGG  EEEEE                       %
%                                                                             %
%                                                                             %
%                        GraphicsMagick Image Methods                         %
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
#include "magick/alpha_composite.h"
#include "magick/attribute.h"
#include "magick/blob.h"
#include "magick/channel.h"
#include "magick/color.h"
#include "magick/constitute.h"
#include "magick/composite.h"
#include "magick/compress.h"
#include "magick/effect.h"
#include "magick/enhance.h"
#include "magick/enum_strings.h"
#include "magick/gem.h"
#include "magick/log.h"
#include "magick/magic.h"
#include "magick/magick.h"
#include "magick/map.h"
#include "magick/monitor.h"
#include "magick/module.h"
#include "magick/omp_data_view.h"
#include "magick/operator.h"
#include "magick/pixel_cache.h"
#include "magick/pixel_iterator.h"
#include "magick/quantize.h"
#include "magick/render.h"
#include "magick/profile.h"
#include "magick/semaphore.h"
#include "magick/signature.h"
#include "magick/tempfile.h"
#include "magick/utility.h"
#include "magick/xwindow.h"

/*
  Constant declaration.
*/
const char
  *BackgroundColor = "#ffffffffffff",  /* white */
  *BorderColor = "#dfdfdfdfdfdf",  /* gray */
  *DefaultTileFrame = "15x15+3+3",
  *DefaultTileGeometry = "120x120+4+3>",
  *DefaultTileLabel = "%f\n%wx%h\n%b",
  *ForegroundColor = "#000000000000",  /* black */
  *HighlightColor = "#f1f100001e1e", /* light red */
  *LoadImageText = "[%s] Loading image...  ",
  *LoadImagesText = "Loading images...  ",
  *MatteColor = "#bdbdbdbdbdbd",  /* gray */
  *PSDensityGeometry = "72.0x72.0",
  *PSPageGeometry = "612x792>",
  *SaveImageText = "[%s] Saving image...  ",
  *SaveImagesText = "Saving images...  ";

const unsigned long
  DefaultCompressionQuality = 75;

/* Round floating value to an integer */
#define RndToInt(value) ((int)((value)+0.5))

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A c c e s s D e f i n i t i o n                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AccessDefinition() searches the definitions for an entry matching the
%  specified magick and key. NULL is returned if no matching entry is found.
%
%  The format of the AccessDefinition method is:
%
%      const char *AccessDefinition(const ImageInfo *image_info,
%                                    const char *magick, const char *key)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o magick: Format ID. This is usually the same as the coder name.
%
%    o key: Key to search for.
%
*/
MagickExport const char *AccessDefinition(const ImageInfo *image_info,
  const char *magick, const char *key)
{
  const char
    *value = 0;

  char
    search_key[MaxTextExtent];

  if (image_info->definitions)
    {
      FormatString(search_key, "%.60s:%.1024s", magick, key);
      value=(const char*) MagickMapAccessEntry((MagickMap) image_info->definitions,search_key,0);
    }
  return value;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A d d D e f i n i t i o n s                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AddDefinitions() adds definitions from a key/value based string to the current
%  map of definitions in ImageInfo. Definitions may be used by coders/decoders
%  that read and write images.
%
%  The format of the AddDefinitions method is:
%
%      void AddDefinitions(ImageInfo *image_info,const char *options)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o options: List of key/value pairs to put in the definitions map. The
%      format of the string is "key1[=[value1]],key2[=[value2]],...". A missing
%      value argument (with or without the equal sign) inserts an empty, zero
%      length string as value for a key.
%
%    o exception: Errors result in updates to this structure.
%
*/
MagickExport MagickPassFail
AddDefinitions(ImageInfo *image_info,const char *definitions,
  ExceptionInfo *exception)
{
  char
    key[MaxTextExtent],
    value[MaxTextExtent];

  unsigned int
    status;

  unsigned int
    i,
    j;

  size_t
    length;

  status=MagickPass;

  if (image_info->definitions == 0)
    image_info->definitions=MagickMapAllocateMap(MagickMapCopyString,
      MagickMapDeallocateString);

  length=strlen(definitions);
  i=0;
  while (i < length)
  {
    unsigned int
      has_value;

    for (j=0; (i < length) && (definitions[i] != '=') && (definitions[i] != ','); i++,j++)
      key[j]=definitions[i];
    key[j]='\0';
    has_value=(i < length) && (definitions[i] == '='); /* Could be 0-length value */
    i++;

    j=0;
    if (has_value)
      {
        for (; (i < length) && (definitions[i] != ','); i++,j++)
          value[j]=definitions[i];
        i++;
      }
    value[j]='\0';
    if (strlen(key) != 0)
      {
        status &= MagickMapAddEntry((MagickMap) image_info->definitions,key,value,0,exception);
      }
    else
      {
        status=MagickFail;
        break;
      }
  }
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A l l o c a t e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% AllocateImage() returns a pointer to an image structure initialized to
% default values.
%
%  The format of the AllocateImage method is:
%
%      Image *AllocateImage(const ImageInfo *image_info)
%
%  A description of each parameter follows:
%
%    o image_info: Many of the image default values are set from this
%      structure.  For example, filename, compression, depth, background color,
%      and others.
%
%
*/
MagickExport Image *AllocateImage(const ImageInfo *image_info)
{
  Image
    *allocate_image;

  /*
    Allocate image structure.
  */
  allocate_image=MagickAllocateMemory(Image *,sizeof(Image));
  if (allocate_image == (Image *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToAllocateImage);
  (void) memset(allocate_image,0,sizeof(Image));
  /*
    Initialize Image structure.
  */
  (void) strcpy(allocate_image->magick,"MIFF");
  allocate_image->storage_class=DirectClass;
  allocate_image->depth=QuantumDepth;
  allocate_image->interlace=NoInterlace;
  allocate_image->colorspace=RGBColorspace;
  allocate_image->compose=OverCompositeOp;
  allocate_image->blur=1.0;
  GetExceptionInfo(&allocate_image->exception);
  (void) QueryColorDatabase(BackgroundColor,&allocate_image->background_color,
    &allocate_image->exception);
  (void) QueryColorDatabase(BorderColor,&allocate_image->border_color,
    &allocate_image->exception);
  (void) QueryColorDatabase(MatteColor,&allocate_image->matte_color,
    &allocate_image->exception);
  allocate_image->orientation=UndefinedOrientation;
  GetTimerInfo(&allocate_image->timer);
  GetCacheInfo(&allocate_image->cache);
  allocate_image->blob=CloneBlobInfo((BlobInfo *) NULL);
  allocate_image->logging=IsEventLogging();
  allocate_image->is_monochrome=MagickTrue;
  allocate_image->is_grayscale=MagickTrue;
  allocate_image->reference_count=1;
  allocate_image->semaphore=AllocateSemaphoreInfo();
  allocate_image->signature=MagickSignature;
  allocate_image->default_views=AllocateThreadViewSet(allocate_image,
                                                      &allocate_image->exception);
  if (image_info == (ImageInfo *) NULL)
    return(allocate_image);
  /*
    Transfer image info.
  */
  SetBlobClosable(allocate_image,(image_info->file == NULL));
  (void) strlcpy(allocate_image->filename,image_info->filename,MaxTextExtent);
  (void) strlcpy(allocate_image->magick_filename,image_info->filename,
    MaxTextExtent);
  (void) strlcpy(allocate_image->magick,image_info->magick,MaxTextExtent);
  if (image_info->size != (char *) NULL)
    {
      (void) GetGeometry(image_info->size,&allocate_image->tile_info.x,
        &allocate_image->tile_info.y,&allocate_image->columns,
        &allocate_image->rows);
      allocate_image->offset=allocate_image->tile_info.x;
      allocate_image->tile_info.width=allocate_image->columns;
      allocate_image->tile_info.height=allocate_image->rows;
    }
  if (image_info->tile != (char *) NULL)
    if (!IsSubimage(image_info->tile,False))
      (void) GetGeometry(image_info->tile,&allocate_image->tile_info.x,
        &allocate_image->tile_info.y,&allocate_image->columns,
        &allocate_image->rows);
  allocate_image->compression=image_info->compression;
  allocate_image->dither=image_info->dither;
  allocate_image->interlace=image_info->interlace;
  allocate_image->units=image_info->units;
  if (image_info->density != (char *) NULL)
    {
      int
        count;

      count=GetMagickDimension(image_info->density,&allocate_image->x_resolution,
                               &allocate_image->y_resolution,NULL,NULL);
      if (count != 2)
        allocate_image->y_resolution=allocate_image->x_resolution;
    }
  if (image_info->page != (char *) NULL)
    {
      char
        *geometry;

      allocate_image->page=allocate_image->tile_info;
      geometry=GetPageGeometry(image_info->page);
      (void) GetGeometry(geometry,&allocate_image->page.x,
        &allocate_image->page.y,&allocate_image->page.width,
        &allocate_image->page.height);
      MagickFreeMemory(geometry);
    }
  allocate_image->depth=image_info->depth;
  allocate_image->background_color=image_info->background_color;
  allocate_image->border_color=image_info->border_color;
  allocate_image->matte_color=image_info->matte_color;
  allocate_image->client_data=image_info->client_data;
  allocate_image->ping=image_info->ping;

  if (image_info->attributes != (Image *) NULL)
    (void) CloneImageAttributes(allocate_image,image_info->attributes);

  return(allocate_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A l l o c a t e I m a g e C o l o r m a p                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AllocateImageColormap() allocates an image colormap and initializes
%  it to a linear gray colorspace with increasing intensity.  If the image
%  already has a colormap, it is replaced.  AllocateImageColormap() returns
%  True if successful, otherwise False if there is not enough memory.
%
%  The format of the AllocateImageColormap method is:
%
%      unsigned int AllocateImageColormap(Image *image,
%        const unsigned long colors)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o colors: The number of colors in the image colormap.
%
%
*/
MagickExport MagickPassFail AllocateImageColormap(Image *image,
  const unsigned long colors)
{
  register long
    i;

  size_t
    length;

  Quantum
    quantum;

  /*
    Allocate image colormap.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  image->storage_class=PseudoClass;
  image->colors=colors;
  length=image->colors*sizeof(PixelPacket);
  if (image->colormap == (PixelPacket *) NULL)
    image->colormap=MagickAllocateMemory(PixelPacket *,length);
  else
    MagickReallocMemory(PixelPacket *,image->colormap,length);
  if (image->colormap == (PixelPacket *) NULL)
    {
      image->colors=0;
      image->storage_class=DirectClass;
      return(MagickFail);
    }
  for (i=0; i < (long) image->colors; i++)
  {
    quantum=(Quantum) (i*(MaxRGB/Max(colors-1,1)));
    image->colormap[i].red=quantum;
    image->colormap[i].green=quantum;
    image->colormap[i].blue=quantum;
    image->colormap[i].opacity=OpaqueOpacity;
  }
  return(MagickPass);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A l l o c a t e N e x t I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Use AllocateNextImage() to initialize the next image in a sequence to
%  default values.  The next member of image points to the newly allocated
%  image.  If there is a memory shortage, next is assigned NULL.
%
%  The format of the AllocateNextImage method is:
%
%      void AllocateNextImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows:
%
%    o image_info: Many of the image default values are set from this
%      structure.  For example, filename, compression, depth, background color,
%      and others.
%
%    o image: The image.
%
%
*/
MagickExport void AllocateNextImage(const ImageInfo *image_info,Image *image)
{
  /*
    Allocate image structure.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  image->next=AllocateImage(image_info);
  if (image->next == (Image *) NULL)
    return;
  (void) strlcpy(image->next->filename,image->filename,MaxTextExtent);
  if (image_info != (ImageInfo *) NULL)
    (void) strlcpy(image->next->filename,image_info->filename,MaxTextExtent);
  DestroyBlob(image->next);
  image->next->blob=ReferenceBlob(image->blob);
  image->next->scene=image->scene+1;
  image->next->previous=image;
}

#if defined(HasX11)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A n i m a t e I m a g e s                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AnimateImages() repeatedly displays an image sequence to any X window
%  screen.  It returns a value other than 0 if successful.  Check the
%  exception member of image to determine the reason for any failure.
%
%  The format of the AnimateImages method is:
%
%      unsigned int AnimateImages(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o image: The image.
%
%
*/
MagickExport MagickPassFail AnimateImages(const ImageInfo *image_info,
  Image *image)
{
  char
    *client_name;

  Display
    *display;

  XrmDatabase
    resource_database;

  MagickXResourceInfo
    resource;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  display=XOpenDisplay((char *) NULL);
  if (display == (Display *) NULL)
    return(False);
  (void) XSetErrorHandler(MagickXError);
  client_name=SetClientName((char *) NULL);
  resource_database=MagickXGetResourceDatabase(display,client_name);
  MagickXGetResourceInfo(resource_database,client_name,&resource);
  resource.image_info=CloneImageInfo(image_info);
  resource.immutable=True;
  (void) MagickXAnimateImages(display,&resource,&client_name,1,image);
  (void) XCloseDisplay(display);
  DestroyImageInfo(resource.image_info);
  return(image->exception.severity == UndefinedException);
}
#else
MagickExport MagickPassFail AnimateImages(const ImageInfo *image_info,
  Image *image)
{
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  ThrowBinaryException(MissingDelegateError,XWindowLibraryIsNotAvailable,
    image->filename);
  return(False);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     A p p e n d I m a g e s                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  The AppendImages() method takes a set of images and appends them to each
%  other top-to-bottom if the stack parameter is true, otherwise left-to-right.
%
%  The format of the AppendImage method is:
%
%      Image *AppendImages(const Image *image,const unsigned int stack,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image sequence.
%
%    o stack: A value other than 0 stacks the images top-to-bottom.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *AppendImages(const Image *image,const unsigned int stack,
  ExceptionInfo *exception)
{
#define AppendImageText  "[%s]Append image sequence..."

  Image
    *append_image;

  register const Image
    *next;

  register long
    x,
    y;

  unsigned int
    status;

  unsigned long
    height,
    scene,
    width;

  /*
    Ensure the image have the same column width.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if (image->next == (Image *) NULL)
    ThrowImageException3(ImageError, ImageSequenceIsRequired,
      UnableToAppendImage);
  width=image->columns;
  height=image->rows;
  for (next=image->next; next != (Image *) NULL; next=next->next)
  {
    if (stack)
      {
        if (next->columns > width)
          width=next->columns;
        height+=next->rows;
        continue;
      }
    width+=next->columns;
    if (next->rows > height)
      height=next->rows;
  }
  /*
    Initialize append next attributes.
  */
  append_image=CloneImage(image,width,height,True,exception);
  if (append_image == (Image *) NULL)
    return((Image *) NULL);
  (void) SetImage(append_image,OpaqueOpacity);
  scene=0;
  if (stack)
    {
      /*
        Stack top-to-bottom.
      */
      y=0;
      for (next=image; next != (Image *) NULL; next=next->next)
      {
        if (next->storage_class == DirectClass)
          (void) SetImageType(append_image,TrueColorType);
        (void) CompositeImage(append_image,CopyCompositeOp,next,0,y);
        y+=next->rows;
        status=MagickMonitorFormatted(scene,GetImageListLength(image),
                                      exception,AppendImageText,
                                      image->filename);
        if (status == False)
          break;
        scene++;
      }
      return(append_image);
    }
  /*
    Stack left-to-right.
  */
  x=0;
  for (next=image; next != (Image *) NULL; next=next->next)
  {
    if (next->storage_class == DirectClass)
      (void) SetImageType(append_image,TrueColorType);
    (void) CompositeImage(append_image,CopyCompositeOp,next,x,0);
    x+=next->columns;
    status=MagickMonitorFormatted(scene++,GetImageListLength(image),
                                  exception,AppendImageText,
                                  image->filename);
    if (status == False)
      break;
  }
  return(append_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     A v e r a g e I m a g e s                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  The Average() method takes a set of images and averages them together.
%  Each image in the set must have the same width and height.  Average()
%  returns a single image with each corresponding pixel component of
%  each image averaged.   On failure, a NULL image is returned and
%  exception describes the reason for the failure.
%
%  The format of the AverageImage method is:
%
%      Image *AverageImages(Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image sequence.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *AverageImages(const Image *image,ExceptionInfo *exception)
{
  ThreadViewDataSet
    *pixels_sums;

  Image
    *average_image;

  const Image
    *last_image;

  long
    y;

  unsigned long
    row_count=0;

  double
    number_scenes;
    
  unsigned long
    number_pixels;

  MagickPassFail
    status=MagickPass;

  /*
    Ensure the image are the same size.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if (image->next == (Image *) NULL)
    ThrowImageException3(ImageError,ImageSequenceIsRequired,
                         UnableToAverageImage);
  {
    const Image
      *next;
      
    for (next=image; next != (Image *) NULL; next=next->next)
      {
        if ((next->columns != image->columns) || (next->rows != image->rows))
          ThrowImageException3(OptionError,UnableToAverageImageSequence,
                               ImageWidthsOrHeightsDiffer);
      }
  }
  /*
    Allocate sum accumulation buffer.
  */
  number_pixels=image->columns;
  pixels_sums=AllocateThreadViewDataArray(image,exception,number_pixels,
                                          sizeof(DoublePixelPacket));
  if (pixels_sums == (ThreadViewDataSet *) NULL)
    ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
                         UnableToAverageImageSequence);
  /*
    Initialize average next attributes.
  */
  average_image=CloneImage(image,image->columns,image->rows,True,exception);
  if (average_image == (Image *) NULL)
    {
      DestroyThreadViewDataSet(pixels_sums);
      return((Image *) NULL);
    }
  average_image->storage_class=DirectClass;

  number_scenes=(double) GetImageListLength(image);
  last_image=GetLastImageInList(image);
#if defined(HAVE_OPENMP)
#  pragma omp parallel for schedule(dynamic) shared(row_count, status)
#endif
  for (y=0; y < (long) image->rows; y++)
    {
      register DoublePixelPacket
        *pixels_sum;

      const Image
        *next;

      register const PixelPacket
        *p;

      register long
        x;

      MagickBool
        thread_status;

      thread_status=status;
      if (thread_status == MagickFail)
        continue;

      pixels_sum=AccessThreadViewData(pixels_sums);

      /*
        Compute sum over each pixel color component.
      */
      for (next=image; next != (Image *) NULL; next=next->next)
        {
          ViewInfo
            *next_view;

          next_view=OpenCacheView((Image *) next);
          if (next_view == (ViewInfo *) NULL)
            thread_status=MagickFail;
          if (next_view != (ViewInfo *) NULL)
            {
              p=AcquireCacheViewPixels(next_view,0,y,next->columns,1,exception);
              if (p == (const PixelPacket *) NULL)
                thread_status=MagickFail;
              if (p != (const PixelPacket *) NULL)
                {
                  if (next == image)
                    {
                      for (x=0; x < (long) next->columns; x++)
                        {
                          pixels_sum[x].red=p[x].red;
                          pixels_sum[x].green=p[x].green;
                          pixels_sum[x].blue=p[x].blue;
                          pixels_sum[x].opacity=p[x].opacity;
                        }
                    }
                  else
                    {
                      for (x=0; x < (long) next->columns; x++)
                        {
                          pixels_sum[x].red+=p[x].red;
                          pixels_sum[x].green+=p[x].green;
                          pixels_sum[x].blue+=p[x].blue;
                          pixels_sum[x].opacity+=p[x].opacity;
                        }
                    }
                }
              CloseCacheView(next_view);
            }
        }
      /*
        Average next pixels.
      */
      if (thread_status != MagickFail)
        {
          register PixelPacket
            *q;

          q=SetImagePixelsEx(average_image,0,y,average_image->columns,1,exception);
          if (q == (PixelPacket *) NULL)
            thread_status=MagickFail;
          if (q != (PixelPacket *) NULL)
            {
              for (x=0; x < (long) average_image->columns; x++)
                {
                  q[x].red=(Quantum) (pixels_sum[x].red/number_scenes+0.5);
                  q[x].green=(Quantum) (pixels_sum[x].green/number_scenes+0.5);
                  q[x].blue=(Quantum) (pixels_sum[x].blue/number_scenes+0.5);
                  q[x].opacity=(Quantum) (pixels_sum[x].opacity/number_scenes+0.5);
                }
              if (!SyncImagePixelsEx(average_image,exception))
                thread_status=MagickFail;
            }
        }

#if defined(HAVE_OPENMP)
#  pragma omp critical
#endif
      {
        row_count++;
        if (QuantumTick(row_count,average_image->rows))
          if (!MagickMonitorFormatted(row_count,average_image->rows,exception,
                                      "[%s,...,%s] Average image sequence...",
                                      image->filename,last_image->filename))
            thread_status=MagickFail;
      
        if (thread_status == MagickFail)
          status=MagickFail;
      }
    }

  DestroyThreadViewDataSet(pixels_sums);

  if (status == MagickFail)
    {
      DestroyImage(average_image);
      average_image=(Image *) NULL;
    }

  return(average_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C a t c h I m a g e E x c e p t i o n                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CatchImageException() returns if no exceptions are found in the image
%  sequence, otherwise it determines the most severe exception and reports
%  it as a warning or error depending on the severity.
%
%  The format of the CatchImageException method is:
%
%      ExceptionType CatchImageException(Image *image)
%
%  A description of each parameter follows:
%
%    o image: An image sequence.
%
%
*/
MagickExport ExceptionType CatchImageException(Image *image)
{
  ExceptionInfo
    exception;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  GetExceptionInfo(&exception);
  GetImageException(image,&exception);
  CatchException(&exception);
  DestroyExceptionInfo(&exception);
  return(exception.severity);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l i p P a t h I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ClipPathImage() sets the image clip mask based any clipping path information
%  if it exists.
%
%  The format of the ClipPathImage method is:
%
%      unsigned int ClipPathImage(Image *image,const char *pathname,
%        const unsigned int inside)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o pathname: name of clipping path resource. If name is preceded by #, use
%      clipping path numbered by name.
%
%    o inside: if non-zero, later operations take effect inside clipping path.
%      Otherwise later operations take effect outside clipping path.
%
%
*/
MagickExport MagickPassFail ClipImage(Image *image)
{
  return(ClipPathImage(image,"#1",True));
}

#define ClipPathImageText  "[%s] Creating image clip mask..."
static MagickPassFail
ClipPathImageCallBack(void *mutable_data,         /* User provided mutable data */
                      const void *immutable_data, /* User provided immutable data */
                      Image *image,               /* Modify image */
                      PixelPacket *pixels,        /* Pixel row */
                      IndexPacket *indexes,       /* Pixel row indexes */
                      const long npixels,         /* Number of pixels in row */
                      ExceptionInfo *exception)   /* Exception report */
{
  /*
    Force all pixels to be either black or white (opaque or transparent)
    to remove any unintended antialiasing effects created by the SVG
    renderer.
  */
  const MagickBool
    inside = *((MagickBool *) immutable_data);

  register Quantum
    intensity;

  register long
    i;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  for (i=0; i < npixels; i++)
    {
      intensity=PixelIntensityToQuantum(&pixels[i]);
      if (inside)
        intensity=(intensity == TransparentOpacity ? TransparentOpacity :
          OpaqueOpacity);
      else
        intensity=(intensity == TransparentOpacity ? OpaqueOpacity :
          TransparentOpacity);
      pixels[i].red=intensity;
      pixels[i].green=intensity;
      pixels[i].blue=intensity;
      pixels[i].opacity=intensity;
    }

  return MagickPass;
}
MagickExport MagickPassFail ClipPathImage(Image *image,const char *pathname,
  const MagickBool inside)
{

  char
    key[MaxTextExtent];

  const ImageAttribute
    *attribute;

  Image
    *clip_mask;

  ImageInfo
    *image_info;

  MagickPassFail
    status=MagickPass;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(pathname != NULL);
  FormatString(key,"8BIM:1999,2998:%s",pathname);
  attribute=GetImageAttribute(image,key);
  if (attribute == (const ImageAttribute *) NULL)
    return(MagickFail);
  image_info=CloneImageInfo((ImageInfo *) NULL);
  (void) QueryColorDatabase("#ffffffff",&image_info->background_color,
    &image->exception);
  clip_mask=BlobToImage(image_info,attribute->value,strlen(attribute->value),
    &image->exception);
  DestroyImageInfo(image_info);
  if (clip_mask == (Image *) NULL)
    return (MagickFail);
  if (clip_mask->storage_class == PseudoClass)
    {
      if (SyncImage(clip_mask) == MagickFail)
        return (MagickFail);
      clip_mask->storage_class=DirectClass;
    }
  clip_mask->matte=True;
  /*
    Force all pixels to be either black or white (opaque or transparent)
    to remove any unintended antialiasing effects created by the SVG
    renderer.
  */
  status=PixelIterateMonoModify(ClipPathImageCallBack,NULL,
                                ClipPathImageText,
                                NULL,&inside,0,0,clip_mask->columns,clip_mask->rows,
                                clip_mask,&image->exception);
  /*
    Overload magick_filename to keep name of path that created image.
    This is needed so we can get the path as postscript for PS coders
    to create a postscript vector based clipping path.
  */
  FormatString(clip_mask->magick_filename,"8BIM:1999,2998:%s\nPS",pathname);

  clip_mask->is_grayscale=True;
  clip_mask->is_monochrome=True;
  (void) SetImageClipMask(image,clip_mask);
  DestroyImage(clip_mask);
  return status;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CloneImage() copies an image and returns the copy as a new image object.
%  If the specified columns and rows is 0, an exact copy of the image is
%  returned, otherwise the pixel data is undefined and must be initialized
%  with the SetImagePixels() and SyncImagePixels() methods.  On failure,
%  a NULL image is returned and exception describes the reason for the
%  failure.
%
%  The format of the CloneImage method is:
%
%      Image *CloneImage(const Image *image,const unsigned long columns,
%        const unsigned long rows,const unsigned int orphan,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o columns: The number of columns in the cloned image.
%
%    o rows: The number of rows in the cloned image.
%
%    o orphan:  With a value other than 0, the cloned image is an orphan.  An
%      orphan is a stand-alone image that is not assocated with an image list.
%      In effect, the next and previous members of the cloned image is set to
%      NULL.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *CloneImage(const Image *image,const unsigned long columns,
  const unsigned long rows,const unsigned int orphan,ExceptionInfo *exception)
{
  Image
    *clone_image;

  size_t
    length;

  /*
    Clone the image.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  clone_image=MagickAllocateMemory(Image *,sizeof(Image));
  if (clone_image == (Image *) NULL)
    ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
      UnableToCloneImage);
  (void) memset(clone_image,0,sizeof(Image));
  clone_image->storage_class=image->storage_class;
  clone_image->colorspace=image->colorspace;
  clone_image->compression=image->compression;
  clone_image->dither=image->dither;
  clone_image->taint=image->taint;
  clone_image->is_grayscale=image->is_grayscale;
  clone_image->is_monochrome=image->is_monochrome;
  clone_image->matte=image->matte;
  clone_image->columns=image->columns;
  clone_image->rows=image->rows;
  clone_image->depth=image->depth;
  if (image->colormap != (PixelPacket *) NULL)
    {
      /*
        Allocate and copy the image colormap.
      */
      clone_image->colors=image->colors;
      length=image->colors*sizeof(PixelPacket);
      clone_image->colormap=MagickAllocateMemory(PixelPacket *,length);
      if (clone_image->colormap == (PixelPacket *) NULL)
        ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
          UnableToCloneImage);
      length=image->colors*sizeof(PixelPacket);
      (void) memcpy(clone_image->colormap,image->colormap,length);
    }
  clone_image->background_color=image->background_color;
  clone_image->border_color=image->border_color;
  clone_image->matte_color=image->matte_color;
  clone_image->gamma=image->gamma;
  clone_image->chromaticity=image->chromaticity;
  /*
    Clone attached profiles.
  */
  if (image->profiles)
    clone_image->profiles=MagickMapCloneMap(image->profiles,exception);
  clone_image->orientation=image->orientation;
  clone_image->rendering_intent=image->rendering_intent;
  clone_image->units=image->units;
  clone_image->montage=(char *) NULL;
  clone_image->directory=(char *) NULL;
  clone_image->geometry=(char *) NULL;
  clone_image->offset=image->offset;
  clone_image->x_resolution=image->x_resolution;
  clone_image->y_resolution=image->y_resolution;
  clone_image->page=image->page;
  clone_image->tile_info=image->tile_info;
  clone_image->blur=image->blur;
  clone_image->fuzz=image->fuzz;
  clone_image->filter=image->filter;
  clone_image->interlace=image->interlace;
  clone_image->endian=image->endian;
  clone_image->gravity=image->gravity;
  clone_image->compose=image->compose;
  clone_image->signature=MagickSignature;
  (void) CloneImageAttributes(clone_image,image);
  clone_image->scene=image->scene;
  clone_image->dispose=image->dispose;
  clone_image->delay=image->delay;
  clone_image->iterations=image->iterations;
  clone_image->total_colors=image->total_colors;
  clone_image->error=image->error;
  clone_image->semaphore=AllocateSemaphoreInfo();
  clone_image->logging=image->logging;
  clone_image->timer=image->timer;
  GetExceptionInfo(&clone_image->exception);
  CopyException(&clone_image->exception,&image->exception);
  clone_image->client_data=image->client_data;
  clone_image->start_loop=image->start_loop;
  clone_image->ascii85=0; /* Don't copy  ascii85 huffman support structure */
  clone_image->magick_columns=image->magick_columns;
  clone_image->magick_rows=image->magick_rows;
  (void) strlcpy(clone_image->magick_filename,image->magick_filename,
    MaxTextExtent);
  (void) strlcpy(clone_image->magick,image->magick,MaxTextExtent);
  (void) strlcpy(clone_image->filename,image->filename,MaxTextExtent);
  clone_image->reference_count=1;
  clone_image->previous=(Image *) NULL;
  clone_image->list=(Image *) NULL;
  clone_image->next=(Image *) NULL;
  clone_image->clip_mask=(Image *) NULL;
  if (orphan)
    clone_image->blob=CloneBlobInfo((BlobInfo *) NULL);
  else
    {
      clone_image->blob=ReferenceBlob(image->blob);
      if (image->previous != (Image *) NULL)
        clone_image->previous->next=clone_image;
      if (image->next != (Image *) NULL)
        clone_image->next->previous=clone_image;
    }
  if ((columns == 0) && (rows == 0))
    {
      if (image->montage != (char *) NULL)
        (void) CloneString(&clone_image->montage,image->montage);
      if (image->directory != (char *) NULL)
        (void) CloneString(&clone_image->directory,image->directory);
      if (image->clip_mask != (Image *) NULL)
        clone_image->clip_mask=CloneImage(image->clip_mask,0,0,True,exception);
      clone_image->ping=image->ping;
      clone_image->cache=ReferenceCache(image->cache);
      clone_image->default_views=AllocateThreadViewSet(clone_image,exception);
      return(clone_image);
    }
  clone_image->page.width=columns;
  clone_image->page.height=rows;
  clone_image->page.x=(long) columns*image->page.x/(long) clone_image->columns;
  clone_image->page.y=(long) rows*image->page.y/(long) clone_image->rows;
  clone_image->columns=columns;
  clone_image->rows=rows;
  clone_image->ping=image->ping;
  GetCacheInfo(&clone_image->cache);
  clone_image->default_views=AllocateThreadViewSet(clone_image,exception);
  return(clone_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e I m a g e I n f o                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CloneImageInfo() makes a copy of the given image info structure.  If
%  NULL is specified, a new image info structure is created initialized to
%  default values.
%
%  The format of the CloneImageInfo method is:
%
%      ImageInfo *CloneImageInfo(const ImageInfo *image_info)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%
*/
MagickExport ImageInfo *CloneImageInfo(const ImageInfo *image_info)
{
  ImageInfo
    *clone_info;

  clone_info=MagickAllocateMemory(ImageInfo *,sizeof(ImageInfo));
  if (clone_info == (ImageInfo *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToCloneImageInfo);
  GetImageInfo(clone_info);
  if (image_info == (ImageInfo *) NULL)
    return(clone_info);
  clone_info->compression=image_info->compression;
  clone_info->temporary=image_info->temporary;
  clone_info->adjoin=image_info->adjoin;
  clone_info->antialias=image_info->antialias;
  clone_info->subimage=image_info->subimage;
  clone_info->subrange=image_info->subrange;
  clone_info->depth=image_info->depth;
  if (image_info->size != (char *) NULL)
    clone_info->size=AllocateString(image_info->size);
  if (image_info->tile != (char *) NULL)
    clone_info->tile=AllocateString(image_info->tile);
  if (image_info->page != (char *) NULL)
    clone_info->page=AllocateString(image_info->page);
  clone_info->interlace=image_info->interlace;
  clone_info->endian=image_info->endian;
  clone_info->units=image_info->units;
  clone_info->quality=image_info->quality;
  if (image_info->sampling_factor != (char *) NULL)
    clone_info->sampling_factor=AllocateString(image_info->sampling_factor);
  if (image_info->server_name != (char *) NULL)
    clone_info->server_name=AllocateString(image_info->server_name);
  if (image_info->font != (char *) NULL)
    clone_info->font=AllocateString(image_info->font);
  if (image_info->texture != (char *) NULL)
    clone_info->texture=AllocateString(image_info->texture);
  if (image_info->density != (char *) NULL)
    clone_info->density=AllocateString(image_info->density);
  clone_info->pointsize=image_info->pointsize;
  clone_info->fuzz=image_info->fuzz;
  clone_info->pen=image_info->pen;
  clone_info->background_color=image_info->background_color;
  clone_info->border_color=image_info->border_color;
  clone_info->matte_color=image_info->matte_color;
  clone_info->dither=image_info->dither;
  clone_info->progress=image_info->progress;
  clone_info->monochrome=image_info->monochrome;
  clone_info->colorspace=image_info->colorspace;
  clone_info->type=image_info->type;
  clone_info->preview_type=image_info->preview_type;
  clone_info->group=image_info->group;
  clone_info->ping=image_info->ping;
  clone_info->verbose=image_info->verbose;
  if (image_info->view != (char *) NULL)
    clone_info->view=AllocateString(image_info->view);
  if (image_info->authenticate != (char *) NULL)
    clone_info->authenticate=AllocateString(image_info->authenticate);
  if (image_info->attributes != (Image *) NULL)
    clone_info->attributes=CloneImage(image_info->attributes,0,0,True,
      &image_info->attributes->exception);
  if (image_info->definitions != (MagickMap) NULL)
    clone_info->definitions=MagickMapCloneMap((MagickMap) image_info->definitions,0);
  clone_info->client_data=image_info->client_data;
  clone_info->cache=image_info->cache;
  if (image_info->cache != (void *) NULL)
    clone_info->cache=ReferenceCache(image_info->cache);
  clone_info->file=image_info->file;
  clone_info->blob=image_info->blob;
  clone_info->length=image_info->length;
  (void) strlcpy(clone_info->magick,image_info->magick,MaxTextExtent);
  (void) strlcpy(clone_info->unique,image_info->unique,MaxTextExtent);
  (void) strlcpy(clone_info->zero,image_info->zero,MaxTextExtent);
  (void) strlcpy(clone_info->filename,image_info->filename,MaxTextExtent);
  clone_info->signature=image_info->signature;
  return(clone_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     C y c l e C o l o r m a p I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CycleColormap() displaces an image's colormap by a given number of
%  positions.  If you cycle the colormap a number of times you can produce
%  a psychodelic effect.
%
%  The format of the CycleColormapImage method is:
%
%      CycleColormapImage(Image *image,const int amount)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o amount:  Offset the colormap this much.
%
%
*/
#define CycleColormapImageText "[%s] Cycle image colormap..."
static MagickPassFail
CycleColormapCallBack(void *mutable_data,         /* User provided mutable data */
                      const void *immutable_data, /* User provided immutable data */
                      Image *image,               /* Modify image */
                      PixelPacket *pixels,        /* Pixel row */
                      IndexPacket *indexes,       /* Pixel row indexes */
                      const long npixels,         /* Number of pixels in row */
                      ExceptionInfo *exception)   /* Exception report */
{
  /*
    Cycle colormap by amount.
  */
  const int
    amount = *((const int *) immutable_data);


  long
    index;

  register const PixelPacket
    *colormap = image->colormap;

  const unsigned long
    colors = image->colors;

  register long
    i;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  for (i=0; i < npixels; i++)
    {
      index=(long) ((indexes[i]+amount) % colors);
      if (index < 0L)
        index+=colors;
      indexes[i]=(IndexPacket) index;
      pixels[i].red=colormap[index].red;
      pixels[i].green=colormap[index].green;
      pixels[i].blue=colormap[index].blue;
    }

  return MagickPass;
}
MagickExport MagickPassFail  CycleColormapImage(Image *image,const int amount)
{
  unsigned int
    is_grayscale,
    is_monochrome,
    status = MagickPass;

  PixelIteratorOptions
    options;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  is_grayscale=image->is_grayscale;
  is_monochrome=image->is_monochrome;
  if (image->storage_class == DirectClass)
    (void) SetImageType(image,PaletteType);

  InitializePixelIteratorOptions(&options,&image->exception);
#if defined(HAVE_OPENMP) && defined(DisableSlowOpenMP)
  options.max_threads=1;
#endif
  status=PixelIterateMonoModify(CycleColormapCallBack,&options,
                                CycleColormapImageText,
                                NULL,&amount,0,0,image->columns,image->rows,
                                image,&image->exception);
  image->is_grayscale=is_grayscale;
  image->is_monochrome=is_monochrome;
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s c r i b e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DescribeImage() describes an image by printing its attributes to the file.
%  Attributes include the image width, height, size, and others.
%
%  The format of the DescribeImage method is:
%
%      void DescribeImage(Image *image,FILE *file,const unsigned int verbose)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o file: The file, typically stdout.
%
%    o verbose: A value other than zero prints more detailed information
%      about the image. Values greater than one enable counting the number of
%      colors in the image.
%
%
*/
MagickExport MagickPassFail DescribeImage(Image *image,FILE *file,
  const unsigned int verbose)
{
  char
    color[MaxTextExtent],
    format[MaxTextExtent];

  const unsigned char
    *profile;

  size_t
    profile_length;

  const ImageAttribute
    *attribute;

  const MagickInfo
    *magick_info;

  double
    elapsed_time,
    user_time;

  unsigned long
    columns,
    rows;

  magick_int64_t
    pixels_per_second;

  Image
    *p;

  long
    y;

  register long
    i,
    x;

  unsigned long
    count;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(file != (FILE *) NULL);
  elapsed_time=GetElapsedTime(&image->timer);
  user_time=GetUserTime(&image->timer);
  GetTimerInfo(&image->timer);
  if (!verbose)
    {
      /*
        Display summary info about the image.
      */
      if (*image->magick_filename != '\0')
        if (LocaleCompare(image->magick_filename,image->filename) != 0)
          (void) fprintf(file,"%.1024s=>",image->magick_filename);
       if ((image->previous == (Image *) NULL) &&
           (image->next == (Image *) NULL) && (image->scene == 0))
        (void) fprintf(file,"%.1024s ",image->filename);
      else
        (void) fprintf(file,"%.1024s[%lu] ",image->filename,image->scene);
      (void) fprintf(file,"%.1024s ",image->magick);
      columns=image->columns;
      rows=image->rows;
      if ((image->magick_columns != 0) || (image->magick_rows != 0))
        if ((image->magick_columns != image->columns) ||
            (image->magick_rows != image->rows))
          {
            columns=image->magick_columns;
            rows=image->magick_rows;
            (void) fprintf(file,"%lux%lu=>",image->magick_columns,
                           image->magick_rows);
          }
      (void) fprintf(file,"%lux%lu%+ld%+ld ",image->columns,image->rows,
        image->page.x,image->page.y);
      if (image->storage_class == DirectClass)
        {
          (void) fprintf(file,"DirectClass ");
          if (image->total_colors != 0)
            {
              FormatSize(image->total_colors,format);
              (void) fprintf(file,"%.1024s ",format);
            }
        }
      else
        if (image->total_colors <= image->colors)
          (void) fprintf(file,"PseudoClass %uc ",image->colors);
        else
          {
            (void) fprintf(file,"PseudoClass %lu=>%uc ",image->total_colors,
              image->colors);
            (void) fprintf(file,"%ld/%.6f/%.6fe ",
              (long) image->error.mean_error_per_pixel,
              image->error.normalized_mean_error,
              image->error.normalized_maximum_error);
          }
      (void) fprintf(file,"%u-bit ",image->depth);
      if (GetBlobSize(image) != 0)
        {
          FormatSize(GetBlobSize(image),format);
          (void) fprintf(file,"%.1024s ",format);
        }
      (void) fprintf(file,"%0.3fu %ld:%02ld",user_time,
                     (long) (elapsed_time/60.0),
                     (long) ceil(fmod(elapsed_time,60.0)));
      /*
        Only display pixel read rate if the time accumulated is at
        least six times the timer's resolution (typically 0.01 on
        Unix).
      */
      if (elapsed_time >= GetTimerResolution()*6)
        {
          pixels_per_second=(magick_int64_t) ((double) rows*columns/ elapsed_time);
          FormatSize(pixels_per_second,format);
          (void) fprintf(file," (%s pixels/s)",format);
        }
      (void) fprintf(file,"\n");
    
      return (ferror(file) ? MagickFail : MagickPass);
    }
  /*
    Display verbose info about the image.
  */
  (void) SignatureImage(image);
  if (verbose > 1)
    image->total_colors=GetNumberColors(image,(FILE *) NULL,&image->exception);
  (void) fprintf(file,"Image: %.1024s\n",image->filename);
  magick_info=GetMagickInfo(image->magick,&image->exception);
  if ((magick_info == (const MagickInfo *) NULL) ||
      (*magick_info->description == '\0'))
    (void) fprintf(file,"  Format: %.1024s\n",image->magick);
  else
    (void) fprintf(file,"  Format: %.1024s (%.1024s)\n",image->magick,
      magick_info->description);
  (void) fprintf(file,"  Geometry: %lux%lu\n",image->columns,image->rows);
  if (image->storage_class == DirectClass)
    (void) fprintf(file,"  Class: DirectClass\n");
  else
    (void) fprintf(file,"  Class: PseudoClass\n");
  if ((image->magick_columns != 0) || (image->magick_rows != 0))
    if ((image->magick_columns != image->columns) ||
        (image->magick_rows != image->rows))
      (void) fprintf(file,"  Base geometry: %lux%lu\n",image->magick_columns,
        image->magick_rows);
  (void) fprintf(file,"  Type: ");
  switch (GetImageType(image,&image->exception))
  {
    case BilevelType: (void) fprintf(file,"bilevel"); break;
    case GrayscaleType: (void) fprintf(file,"grayscale"); break;
    case GrayscaleMatteType:
      (void) fprintf(file,"grayscale with transparency"); break;
    case PaletteType: (void) fprintf(file,"palette"); break;
    case PaletteMatteType:
      (void) fprintf(file,"palette with transparency"); break;
    case TrueColorType: (void) fprintf(file,"true color"); break;
    case TrueColorMatteType:
      (void) fprintf(file,"true color with transparency"); break;
    case ColorSeparationType: (void) fprintf(file,"color separated"); break;
    case ColorSeparationMatteType:
      (void) fprintf(file,"color separated with transparency"); break;
    default: (void) fprintf(file,"undefined"); break;
  }
  (void) fprintf(file,"\n");
  (void) fprintf(file,"  Depth: %lu bits-per-pixel component\n",
    GetImageDepth(image,&image->exception));
  (void) fprintf(file,"  Channel Depths:\n");
  if (image->colorspace == CMYKColorspace)
    {
      (void) fprintf(file,"    Cyan:     %u bits\n",
                     GetImageChannelDepth(image,CyanChannel,&image->exception));
      (void) fprintf(file,"    Magenta:  %u bits\n",
                     GetImageChannelDepth(image,MagentaChannel,&image->exception));
      (void) fprintf(file,"    Yellow:   %u bits\n",
                     GetImageChannelDepth(image,YellowChannel,&image->exception));
      (void) fprintf(file,"    Black:    %u bits\n",
                     GetImageChannelDepth(image,BlackChannel,&image->exception));
    }
  else if ((IsGrayColorspace(image->colorspace)) ||
           (image->is_grayscale == True))
    {
      (void) fprintf(file,"    Gray:     %u bits\n",
                     GetImageChannelDepth(image,RedChannel,&image->exception));
    }
  else
    {
      (void) fprintf(file,"    Red:      %u bits\n",
                     GetImageChannelDepth(image,RedChannel,&image->exception));
      (void) fprintf(file,"    Green:    %u bits\n",
                     GetImageChannelDepth(image,GreenChannel,&image->exception));
      (void) fprintf(file,"    Blue:     %u bits\n",
                     GetImageChannelDepth(image,BlueChannel,&image->exception));
    }
  if (image->matte)
    (void) fprintf(file,"    Opacity:  %u bits\n",
                   GetImageChannelDepth(image,OpacityChannel,&image->exception));
  (void) fprintf(file,"  Channel Statistics:\n");
  {
    ImageStatistics
      statistics;

    (void) GetImageStatistics(image,&statistics,&image->exception);

    if (image->colorspace == CMYKColorspace)
    {
      (void) fprintf(file,"    Cyan:\n");
      (void) fprintf(file,"      Minimum:            %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.red.minimum,
                     statistics.red.minimum);
      (void) fprintf(file,"      Maximum:            %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.red.maximum,
                     statistics.red.maximum);
      (void) fprintf(file,"      Mean:               %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.red.mean,
                     statistics.red.mean);
      (void) fprintf(file,"      Standard Deviation: %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.red.standard_deviation,
                     statistics.red.standard_deviation);
      (void) fprintf(file,"    Magenta:\n");
      (void) fprintf(file,"      Minimum:            %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.green.minimum,
                     statistics.green.minimum);
      (void) fprintf(file,"      Maximum:            %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.green.maximum,
                     statistics.green.maximum);
      (void) fprintf(file,"      Mean:               %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.green.mean,
                     statistics.green.mean);
      (void) fprintf(file,"      Standard Deviation: %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.green.standard_deviation,
                     statistics.green.standard_deviation);
      (void) fprintf(file,"    Yellow:\n");
      (void) fprintf(file,"      Minimum:            %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.blue.minimum,
                     statistics.blue.minimum);
      (void) fprintf(file,"      Maximum:            %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.blue.maximum,
                     statistics.blue.maximum);
      (void) fprintf(file,"      Mean:               %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.blue.mean,
                     statistics.blue.mean);
      (void) fprintf(file,"      Standard Deviation: %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.blue.standard_deviation,
                     statistics.blue.standard_deviation);
      (void) fprintf(file,"    Black:\n");
      (void) fprintf(file,"      Minimum:            %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.opacity.minimum,
                     statistics.opacity.minimum);
      (void) fprintf(file,"      Maximum:            %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.opacity.maximum,
                     statistics.opacity.maximum);
      (void) fprintf(file,"      Mean:               %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.opacity.mean,
                     statistics.opacity.mean);
      (void) fprintf(file,"      Standard Deviation: %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.opacity.standard_deviation,
                     statistics.opacity.standard_deviation);
      /*
      if (image->matte)
        (void) fprintf(file,"    Opacity:\n");
      */
    }
    else if ((IsGrayColorspace(image->colorspace)) ||
             (image->is_grayscale == True))
      {
      (void) fprintf(file,"    Gray:\n");
      (void) fprintf(file,"      Minimum:            %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.red.minimum,
                     statistics.red.minimum);
      (void) fprintf(file,"      Maximum:            %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.red.maximum,
                     statistics.red.maximum);
      (void) fprintf(file,"      Mean:               %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.red.mean,
                     statistics.red.mean);
      (void) fprintf(file,"      Standard Deviation: %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.red.standard_deviation,
                     statistics.red.standard_deviation);
      if (image->matte)
        {
          (void) fprintf(file,"    Opacity:\n");
          (void) fprintf(file,"      Minimum:            %13.02lf (%1.4f)\n",
                         MaxRGB*statistics.opacity.minimum,
                         statistics.opacity.minimum);
          (void) fprintf(file,"      Maximum:            %13.02lf (%1.4f)\n",
                         MaxRGB*statistics.opacity.maximum,
                         statistics.opacity.maximum);
          (void) fprintf(file,"      Mean:               %13.02lf (%1.4f)\n",
                         MaxRGB*statistics.opacity.mean,
                         statistics.opacity.mean);
          (void) fprintf(file,"      Standard Deviation: %13.02lf (%1.4f)\n",
                         MaxRGB*statistics.opacity.standard_deviation,
                         statistics.opacity.standard_deviation);
        }
      }
    else
    {
      (void) fprintf(file,"    Red:\n");
      (void) fprintf(file,"      Minimum:            %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.red.minimum,
                     statistics.red.minimum);
      (void) fprintf(file,"      Maximum:            %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.red.maximum,
                     statistics.red.maximum);
      (void) fprintf(file,"      Mean:               %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.red.mean,
                     statistics.red.mean);
      (void) fprintf(file,"      Standard Deviation: %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.red.standard_deviation,
                     statistics.red.standard_deviation);
      (void) fprintf(file,"    Green:\n");
      (void) fprintf(file,"      Minimum:            %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.green.minimum,
                     statistics.green.minimum);
      (void) fprintf(file,"      Maximum:            %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.green.maximum,
                     statistics.green.maximum);
      (void) fprintf(file,"      Mean:               %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.green.mean,
                     statistics.green.mean);
      (void) fprintf(file,"      Standard Deviation: %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.green.standard_deviation,
                     statistics.green.standard_deviation);
      (void) fprintf(file,"    Blue:\n");
      (void) fprintf(file,"      Minimum:            %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.blue.minimum,
                     statistics.blue.minimum);
      (void) fprintf(file,"      Maximum:            %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.blue.maximum,
                     statistics.blue.maximum);
      (void) fprintf(file,"      Mean:               %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.blue.mean,
                     statistics.blue.mean);
      (void) fprintf(file,"      Standard Deviation: %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.blue.standard_deviation,
                     statistics.blue.standard_deviation);
      if (image->matte)
        {
          (void) fprintf(file,"    Opacity:\n");
          (void) fprintf(file,"      Minimum:            %13.02lf (%1.4f)\n",
                         MaxRGB*statistics.opacity.minimum,
                         statistics.opacity.minimum);
          (void) fprintf(file,"      Maximum:            %13.02lf (%1.4f)\n",
                         MaxRGB*statistics.opacity.maximum,
                         statistics.opacity.maximum);
          (void) fprintf(file,"      Mean:               %13.02lf (%1.4f)\n",
                         MaxRGB*statistics.opacity.mean,
                         statistics.opacity.mean);
          (void) fprintf(file,"      Standard Deviation: %13.02lf (%1.4f)\n",
                         MaxRGB*statistics.opacity.standard_deviation,
                         statistics.opacity.standard_deviation);
        }
    }
  }
  x=0;
  p=(Image *) NULL;
  if ((image->matte && (strcmp(image->magick,"GIF") != 0)) || image->taint)
    {
      char
        tuple[MaxTextExtent];

      register const PixelPacket
        *p;

      p=(PixelPacket *) NULL;
      for (y=0; y < (long) image->rows; y++)
      {
        p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
        if (p == (const PixelPacket *) NULL)
          break;
        for (x=0; x < (long) image->columns; x++)
        {
          if (p->opacity == TransparentOpacity)
            break;
          p++;
        }
        if (x < (long) image->columns)
          break;
      }
      if ((x < (long) image->columns) || (y < (long) image->rows))
        {
          GetColorTuple(p,image->depth,image->matte,False,tuple);
          (void) fprintf(file,"  Opacity: %.1024s\t",tuple);
          GetColorTuple(p,image->depth,image->matte,True,tuple);
          (void) fprintf(file,"  %.1024s\n",tuple);
        }
    }
  if (image->storage_class == DirectClass)
    {
      if (image->total_colors != 0)
        (void) fprintf(file,"  Colors: %lu\n",image->total_colors);
    }
  else
    {
      if (image->total_colors <= image->colors)
        (void) fprintf(file,"  Colors: %u\n",image->colors);
      else
        (void) fprintf(file,"  Colors: %lu=>%u\n",image->total_colors,
                       image->colors);
    }
  if (image->storage_class == DirectClass)
    {
      if (image->total_colors < 1024)
        if (verbose > 1)
          (void) GetNumberColors(image,file,&image->exception);
    }
  else
    {
      char
        name[MaxTextExtent];

      register PixelPacket
        *p;

      /*
        Display image colormap.
      */
      p=image->colormap;
      for (i=0; i < (long) image->colors; i++)
      {
        char
          tuple[MaxTextExtent];

        GetColorTuple(p,image->depth,image->matte,False,tuple);
        (void) fprintf(file,"    %lu: %.1024s",i,tuple);
        (void) fprintf(file,"\t");
        (void) QueryColorname(image,p,SVGCompliance,name,&image->exception);
        (void) fprintf(file,"  %.1024s",name);
        (void) fprintf(file,"\n");
        p++;
      }
    }
  if (image->error.mean_error_per_pixel != 0.0)
    (void) fprintf(file,"  Mean Exception Per Pixel: %ld\n",
      (long) image->error.mean_error_per_pixel);
  if (image->error.normalized_mean_error != 0.0)
    (void) fprintf(file,"  Normalized Mean Exception: %g\n",
      image->error.normalized_mean_error);
  if (image->error.normalized_maximum_error != 0.0)
    (void) fprintf(file,"  Normalized Maximum Exception: %gn",
      image->error.normalized_maximum_error);
  if (image->rendering_intent == SaturationIntent)
    (void) fprintf(file,"  Rendering-Intent: saturation\n");
  else
    if (image->rendering_intent == PerceptualIntent)
      (void) fprintf(file,"  Rendering-Intent: perceptual\n");
    else
      if (image->rendering_intent == AbsoluteIntent)
        (void) fprintf(file,"  Rendering-Intent: absolute\n");
    else
      if (image->rendering_intent == RelativeIntent)
        (void) fprintf(file,"  Rendering-Intent: relative\n");
  if (image->gamma != 0.0)
    (void) fprintf(file,"  Gamma: %g\n",image->gamma);
  if ((image->chromaticity.red_primary.x != 0.0) ||
      (image->chromaticity.green_primary.x != 0.0) ||
      (image->chromaticity.blue_primary.x != 0.0) ||
      (image->chromaticity.white_point.x != 0.0))
    {
      /*
        Display image chromaticity.
      */
      (void) fprintf(file,"  Chromaticity:\n");
      (void) fprintf(file,"    red primary: (%g,%g)\n",
        image->chromaticity.red_primary.x,image->chromaticity.red_primary.y);
      (void) fprintf(file,"    green primary: (%g,%g)\n",
        image->chromaticity.green_primary.x,
        image->chromaticity.green_primary.y);
      (void) fprintf(file,"    blue primary: (%g,%g)\n",
        image->chromaticity.blue_primary.x,image->chromaticity.blue_primary.y);
      (void) fprintf(file,"    white point: (%g,%g)\n",
        image->chromaticity.white_point.x,image->chromaticity.white_point.y);
    }
  if ((image->tile_info.width*image->tile_info.height) != 0)
    (void) fprintf(file,"  Tile geometry: %lux%lu%+ld%+ld\n",
      image->tile_info.width,image->tile_info.height,image->tile_info.x,
      image->tile_info.y);
  if ((image->x_resolution != 0.0) && (image->y_resolution != 0.0))
    {
      /*
        Display image resolution.
      */
      (void) fprintf(file,"  Resolution: %gx%g",image->x_resolution,
        image->y_resolution);
      if (image->units == UndefinedResolution)
        (void) fprintf(file," pixels\n");
      else
        if (image->units == PixelsPerInchResolution)
          (void) fprintf(file," pixels/inch\n");
        else
          if (image->units == PixelsPerCentimeterResolution)
            (void) fprintf(file," pixels/centimeter\n");
          else
            (void) fprintf(file,"\n");
    }
  FormatSize(GetBlobSize(image),format);
  (void) fprintf(file,"  Filesize: %.1024s\n",format);
  if (image->interlace == NoInterlace)
    (void) fprintf(file,"  Interlace: None\n");
  else
    if (image->interlace == LineInterlace)
      (void) fprintf(file,"  Interlace: Line\n");
    else
      if (image->interlace == PlaneInterlace)
        (void) fprintf(file,"  Interlace: Plane\n");
    else
      if (image->interlace == PartitionInterlace)
        (void) fprintf(file,"  Interlace: Partition\n");
  (void) fprintf(file,"  Orientation: %s\n", OrientationTypeToString(image->orientation));
  (void) QueryColorname(image,&image->background_color,SVGCompliance,color,
    &image->exception);
  (void) fprintf(file,"  Background Color: %.1024s\n",color);
  (void) QueryColorname(image,&image->border_color,SVGCompliance,color,
    &image->exception);
  (void) fprintf(file,"  Border Color: %.1024s\n",color);
  (void) QueryColorname(image,&image->matte_color,SVGCompliance,color,
    &image->exception);
  (void) fprintf(file,"  Matte Color: %.1024s\n",color);
  if ((image->page.width != 0) && (image->page.height != 0))
    (void) fprintf(file,"  Page geometry: %lux%lu%+ld%+ld\n",image->page.width,
      image->page.height,image->page.x,image->page.y);
  (void) fprintf(file,"  Dispose: ");
  switch (image->dispose)
  {
    case UndefinedDispose: (void) fprintf(file,"Undefined\n"); break;
    case NoneDispose: (void) fprintf(file,"None\n"); break;
    case BackgroundDispose: (void) fprintf(file,"Background\n"); break;
    case PreviousDispose: (void) fprintf(file,"Previous\n"); break;
    default: (void) fprintf(file,"\n");  break;
  }
  if (image->delay != 0)
    (void) fprintf(file,"  Delay: %lu\n",image->delay);
  if (image->iterations != 1)
    (void) fprintf(file,"  Iterations: %lu\n",image->iterations);
  p=image;
  while (p->previous != (Image *) NULL)
    p=p->previous;
  for (count=1; p->next != (Image *) NULL; count++)
    p=p->next;
  if (count > 1)
    (void) fprintf(file,"  Scene: %lu of %lu\n",image->scene,count);
  else
    if (image->scene != 0)
      (void) fprintf(file,"  Scene: %lu\n",image->scene);
  (void) fprintf(file,"  Compression: ");
  switch (image->compression)
  {
    case NoCompression: (void) fprintf(file,"None\n"); break;
    case BZipCompression: (void) fprintf(file,"BZip\n"); break;
    case FaxCompression: (void) fprintf(file,"Fax\n"); break;
    case Group4Compression: (void) fprintf(file,"Group 4\n"); break;
    case JPEGCompression: (void) fprintf(file,"JPEG\n"); break;
    case LosslessJPEGCompression: (void) fprintf(file,"Lossless JPEG\n"); break;
    case LZWCompression: (void) fprintf(file,"LZW\n"); break;
    case RLECompression: (void) fprintf(file,"Runlength Encoded\n"); break;
    case ZipCompression: (void) fprintf(file,"Zip\n"); break;
    default: (void) fprintf(file,"\n");  break;
  }
  /*
    Display formatted image attributes. This must happen before we access
    any pseudo attributes like EXIF since doing so causes real attributes
    to be created and we would get duplicates in the output.
  */
  attribute=GetImageAttribute(image,(char *) NULL);
  {
    for ( ; attribute != (const ImageAttribute *) NULL; attribute=attribute->next)
      {
        (void) fprintf(file,"  %c", toupper((int)attribute->key[0]));
        if (strlen(attribute->key) > 1)
          (void) fprintf(file,"%.1024s",attribute->key+1);

        (void) fprintf(file,": ");
        (void) fprintf(file,"%s\n",attribute->value);
      }
  }
  if((profile=GetImageProfile(image,"ICM",&profile_length)) != 0)
    (void) fprintf(file,"  Profile-color: %lu bytes\n",(unsigned long)
      profile_length);
  if((profile=GetImageProfile(image,"IPTC",&profile_length)) != 0)
    {
      char
        *tag,
        *text;

      size_t
        length;

      /*
        Describe IPTC data.
      */
      (void) fprintf(file,"  Profile-iptc: %lu bytes\n",(unsigned long)
        profile_length);
      for (i=0; i < (long) profile_length; )
      {
        if (profile[i] != 0x1c)
          {
            i++;
            continue;
          }
        i++;  /* skip file separator */
        i++;  /* skip record number */
        switch (profile[i])
        {
          case 5: tag=(char *) "Image Name"; break;
          case 7: tag=(char *) "Edit Status"; break;
          case 10: tag=(char *) "Priority"; break;
          case 15: tag=(char *) "Category"; break;
          case 20: tag=(char *) "Supplemental Category"; break;
          case 22: tag=(char *) "Fixture Identifier"; break;
          case 25: tag=(char *) "Keyword"; break;
          case 30: tag=(char *) "Release Date"; break;
          case 35: tag=(char *) "Release Time"; break;
          case 40: tag=(char *) "Special Instructions"; break;
          case 45: tag=(char *) "Reference Service"; break;
          case 47: tag=(char *) "Reference Date"; break;
          case 50: tag=(char *) "Reference Number"; break;
          case 55: tag=(char *) "Created Date"; break;
          case 60: tag=(char *) "Created Time"; break;
          case 65: tag=(char *) "Originating Program"; break;
          case 70: tag=(char *) "Program Version"; break;
          case 75: tag=(char *) "Object Cycle"; break;
          case 80: tag=(char *) "Byline"; break;
          case 85: tag=(char *) "Byline Title"; break;
          case 90: tag=(char *) "City"; break;
          case 95: tag=(char *) "Province State"; break;
          case 100: tag=(char *) "Country Code"; break;
          case 101: tag=(char *) "Country"; break;
          case 103: tag=(char *) "Original Transmission Reference"; break;
          case 105: tag=(char *) "Headline"; break;
          case 110: tag=(char *) "Credit"; break;
          case 115: tag=(char *) "Source"; break;
          case 116: tag=(char *) "Copyright String"; break;
          case 120: tag=(char *) "Caption"; break;
          case 121: tag=(char *) "Local Caption"; break;
          case 122: tag=(char *) "Caption Writer"; break;
          case 200: tag=(char *) "Custom Field 1"; break;
          case 201: tag=(char *) "Custom Field 2"; break;
          case 202: tag=(char *) "Custom Field 3"; break;
          case 203: tag=(char *) "Custom Field 4"; break;
          case 204: tag=(char *) "Custom Field 5"; break;
          case 205: tag=(char *) "Custom Field 6"; break;
          case 206: tag=(char *) "Custom Field 7"; break;
          case 207: tag=(char *) "Custom Field 8"; break;
          case 208: tag=(char *) "Custom Field 9"; break;
          case 209: tag=(char *) "Custom Field 10"; break;
          case 210: tag=(char *) "Custom Field 11"; break;
          case 211: tag=(char *) "Custom Field 12"; break;
          case 212: tag=(char *) "Custom Field 13"; break;
          case 213: tag=(char *) "Custom Field 14"; break;
          case 214: tag=(char *) "Custom Field 15"; break;
          case 215: tag=(char *) "Custom Field 16"; break;
          case 216: tag=(char *) "Custom Field 17"; break;
          case 217: tag=(char *) "Custom Field 18"; break;
          case 218: tag=(char *) "Custom Field 19"; break;
          case 219: tag=(char *) "Custom Field 20"; break;
          default: tag=(char *) "unknown"; break;
        }
        i++;
        (void) fprintf(file,"    %.1024s:\n",tag);
        length=profile[i++] << 8;
        length|=profile[i++];
        text=MagickAllocateMemory(char *,length+1);
        if (text != (char *) NULL)
          {
            char
              **textlist;

            register long
              j;

            (void) strncpy(text,(char *) profile+i,length);
            text[length]='\0';
            textlist=StringToList(text);
            if (textlist != (char **) NULL)
              {
                for (j=0; textlist[j] != (char *) NULL; j++)
                {
                  (void) fprintf(file,"  %s\n",textlist[j]);
                  MagickFreeMemory(textlist[j]);
                }
                MagickFreeMemory(textlist);
              }
            MagickFreeMemory(text);
          }
        i+=length;
      }
    }
  {
    const char
      *profile_name;
    
    size_t
      profile_length;
              
    const unsigned char *
      profile_info;
    
    ImageProfileIterator
      profile_iterator;

    profile_iterator=AllocateImageProfileIterator(image);
    while(NextImageProfile(profile_iterator,&profile_name,&profile_info,
                           &profile_length) != MagickFail)
      {
        if ((LocaleCompare(profile_name,"ICC") == 0) ||
            (LocaleCompare(profile_name,"ICM") == 0) ||
            (LocaleCompare(profile_name,"IPTC") == 0) ||
            (LocaleCompare(profile_name,"8BIM") == 0))
          continue;

        if (profile_length == 0)
          continue;
        (void) fprintf(file,"  Profile-%.1024s: %lu bytes\n",
                       profile_name == (char *) NULL ? "generic" : profile_name,
                       (unsigned long) profile_length);
        if (LocaleCompare(profile_name,"EXIF") == 0)
          {
            attribute=GetImageAttribute(image,"EXIF:*");
            if (attribute != (const ImageAttribute *) NULL)
              {
                char
                  **values;
                
                register char
                  *p;
                
                values=StringToList(attribute->value);
                if (values != (char **) NULL)
                  {
                    for (x=0; values[x] != (char *) NULL; x++)
                      {
                        (void) fprintf(file,"    ");
                        for (p=values[x]; *p != '\0'; p++)
                          {
                            if (p > values[x])
                              if ((isupper((int) ((unsigned char) *p)) != False) &&
                                  (islower((int) ((unsigned char) *(p+1))) != False))
                                (void) fprintf(file," ");
                            if (*p == '=')
                              {
                                (void) fprintf(file,": ");
                                for (p++; *p != '\0'; p++)
                                  (void) fputc(*p,file);
                                break;
                              }
                            (void) fputc(*p,file);
                          }
                        (void) fputc('\n',file);
                        MagickFreeMemory(values[x]);
                      }
                    MagickFreeMemory(values);
                  }
              }
          } /* End of EXIF */
      }
    DeallocateImageProfileIterator(profile_iterator);
  }
  if (image->montage != (char *) NULL)
    (void) fprintf(file,"  Montage: %.1024s\n",image->montage);
  if (image->directory != (char *) NULL)
    {
      Image
        *tile;

      ImageInfo
        *image_info;

      register char
        *p,
        *q;

      WarningHandler
        handler;

      /*
        Display visual image directory.
      */
      image_info=CloneImageInfo((ImageInfo *) NULL);
      (void) CloneString(&image_info->size,"64x64");
      (void) fprintf(file,"  Directory:\n");
      for (p=image->directory; *p != '\0'; p++)
      {
        q=p;
        while ((*q != '\n') && (*q != '\0'))
          q++;
        (void) strncpy(image_info->filename,p,q-p);
        image_info->filename[q-p]='\0';
        p=q;
        (void) fprintf(file,"    %.1024s",image_info->filename);
        handler=SetWarningHandler((WarningHandler) NULL);
        tile=ReadImage(image_info,&image->exception);
        (void) SetWarningHandler(handler);
        if (tile == (Image *) NULL)
          {
            (void) fprintf(file,"\n");
            continue;
          }
        (void) fprintf(file," %lux%lu %.1024s\n",tile->magick_columns,
          tile->magick_rows,tile->magick);
        (void) SignatureImage(tile);
        attribute=GetImageAttribute(tile,(char *) NULL);
        for ( ; attribute != (const ImageAttribute *) NULL; attribute=attribute->next)
        {
          if (*attribute->key == '[')
            continue;
          (void) fprintf(file,"  %.1024s:\n",attribute->key);
          (void) fprintf(file,"%s\n",attribute->value);
        }
        DestroyImage(tile);
      }
      DestroyImageInfo(image_info);
    }
  if (image->taint)
    (void) fprintf(file,"  Tainted: True\n");
  else
    (void) fprintf(file,"  Tainted: False\n");
  /*
    Only display time information if the time accumulated is at least
    the timer's resolution.
  */
  if (user_time >= GetTimerResolution())
    (void) fprintf(file,"  User Time: %0.3fu\n",user_time);
  if (elapsed_time >= GetTimerResolution())
    {
      (void) fprintf(file,"  Elapsed Time: %ld:%02ld\n",
                     (long) (elapsed_time/60.0),
                     (long) ceil(fmod(elapsed_time,60.0)));
      pixels_per_second=(magick_int64_t) ((double) image->rows*
                                          image->columns/
                                          (elapsed_time > GetTimerResolution() ?
                                           elapsed_time : GetTimerResolution()));
      FormatSize(pixels_per_second,format);
      (void) fprintf(file,"  Pixels Per Second: %s\n", format);
    }
  (void) fflush(file);
  return (ferror(file) ? MagickFail : MagickPass);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyImage() dereferences an image, deallocating memory associated with
%  the image if the reference count becomes zero.
%
%  The format of the DestroyImage method is:
%
%      void DestroyImage(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
MagickExport void DestroyImage(Image *image)
{
  int
    destroy;

  /*
    Dereference image.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  destroy=False;
  AcquireSemaphoreInfo((SemaphoreInfo **) &image->semaphore);
  image->reference_count--;
  if (image->reference_count == 0)
    destroy=True;
  LiberateSemaphoreInfo((SemaphoreInfo **) &image->semaphore);
  if (!destroy)
    return;
  /*
    Ensure that this image is not referenced by another image
  */
#if 0
  if (image->previous)
    assert(image->previous->next != image);
  if (image->next)
    assert(image->next->previous != image);
#endif
  /*
    Destroy default views.
  */
  if (image->default_views != (_ThreadViewSetPtr_) NULL)
    DestroyThreadViewSet(image->default_views);
  image->default_views=(_ThreadViewSetPtr_) NULL;
  /*
    Destroy image pixel cache.
  */
  DestroyImagePixels(image);
  if (image->clip_mask != (Image *) NULL)
    DestroyImage(image->clip_mask);
  MagickFreeMemory(image->montage);
  MagickFreeMemory(image->directory);
  MagickFreeMemory(image->colormap);
  if (image->profiles)
    {
      MagickMapDeallocateMap(image->profiles);
      image->profiles=0;
    }
  DestroyImageAttributes(image);
  DestroyExceptionInfo(&image->exception);
  MagickFreeMemory(image->ascii85);
  DestroyBlob(image);
  if (image->semaphore)
    DestroySemaphoreInfo((SemaphoreInfo **) &image->semaphore);
  MagickFreeMemory(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y I m a g e I n f o                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyImageInfo() deallocates memory associated with a ImageInfo
%  structure.
%
%  The format of the DestroyImageInfo method is:
%
%      void DestroyImageInfo(ImageInfo *image_info)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%
*/
MagickExport void DestroyImageInfo(ImageInfo *image_info)
{
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  MagickFreeMemory(image_info->size);
  MagickFreeMemory(image_info->tile);
  MagickFreeMemory(image_info->page);
  MagickFreeMemory(image_info->sampling_factor);
  MagickFreeMemory(image_info->server_name);
  MagickFreeMemory(image_info->font);
  MagickFreeMemory(image_info->texture);
  MagickFreeMemory(image_info->density);
  MagickFreeMemory(image_info->view);
  MagickFreeMemory(image_info->authenticate);
  if (image_info->attributes != (Image *) NULL)
    DestroyImage(image_info->attributes);
  if (image_info->cache != (void *) NULL)
    DestroyCacheInfo(image_info->cache);
  if (image_info->definitions != (MagickMap) NULL)
    MagickMapDeallocateMap((MagickMap) image_info->definitions);
  MagickFreeMemory(image_info);
}

#if defined(HasX11)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D i s p l a y I m a g e s                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DisplayImages() displays an image sequence to any X window screen.  It
%  returns a value other than 0 if successful.  Check the exception member
%  of image to determine the reason for any failure.
%
%  The format of the DisplayImages method is:
%
%      unsigned int DisplayImages(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o image: The image.
%
%
*/
MagickExport MagickPassFail DisplayImages(const ImageInfo *image_info,
  Image *image)
{
  char
    *client_name;

  Display
    *display;

  Image
    *next;

  unsigned long
    state;

  XrmDatabase
    resource_database;

  MagickXResourceInfo
    resource_info;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  display=XOpenDisplay((char *) NULL);
  if (display == (Display *) NULL)
    return(MagickFail);
  (void) XSetErrorHandler(MagickXError);
  client_name=SetClientName((char *) NULL);
  resource_database=MagickXGetResourceDatabase(display,client_name);
  MagickXGetResourceInfo(resource_database,client_name,&resource_info);
  if (image_info->page != (char *) NULL)
    resource_info.image_geometry=AcquireString(image_info->page);
  resource_info.immutable=True;
  for (next=image; next; next=next->next)
  {
    state=DefaultState;
    (void) MagickXDisplayImage(display,&resource_info,&client_name,1,&next,&state);
    if (state & ExitState)
      break;
  }
  if (resource_database != (XrmDatabase) NULL)
    {
      /* It seems that recent X11 libraries (as found in FreeBSD 5.4)
         automatically destroy the resource database associated with
         the display and there are double-frees if we destroy the
         resource database ourselves. */
      /* XrmDestroyDatabase(resource_database); */
      resource_database=(XrmDatabase) NULL;
    }
  MagickXDestroyResourceInfo(&resource_info);
  MagickXDestroyX11Resources();
  (void) XCloseDisplay(display);
  return(image->exception.severity != UndefinedException);
}
#else
MagickExport unsigned int DisplayImages(const ImageInfo *image_info,
  Image *image)
{
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  ThrowBinaryException(MissingDelegateError,XWindowLibraryIsNotAvailable,
    image->filename);
  return(MagickFail);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t I m a g e B o u n d i n g B o x                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetImageBoundingBox returns the bounding box of an image canvas.
%
%  The format of the GetImageBoundingBox method is:
%
%      RectangleInfo GetImageBoundingBox(const Image *image,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o bounds: Method GetImageBoundingBox returns the bounding box of an
%      image canvas.
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
#define GetImageBoundingBoxText "[%s] Get image bounding box..."
MagickExport RectangleInfo GetImageBoundingBox(const Image *image,
                                               ExceptionInfo *exception)
{
  MagickPassFail
    status=MagickPass;

  long
    y;

  unsigned long
    row_count=0;

  PixelPacket
    corners[3];

  RectangleInfo
    bounds;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  bounds.width=0;
  bounds.height=0;
  bounds.x=(long) image->columns;
  bounds.y=(long) image->rows;

  (void) AcquireOnePixelByReference(image,&corners[0],0,0,exception);
  (void) AcquireOnePixelByReference(image,&corners[1],(long) image->columns-1,0,exception);
  (void) AcquireOnePixelByReference(image,&corners[2],0,(long) image->rows-1,exception);
#if defined(HAVE_OPENMP)
#  pragma omp parallel for schedule(dynamic,4) shared(row_count, status)
#endif
  for (y=0; y < (long) image->rows; y++)
    {
      register const PixelPacket
        *p;
    
      register long
        x;

      RectangleInfo
        thread_bounds;

      MagickPassFail
        thread_status;

#if defined(HAVE_OPENMP)
#  pragma omp critical
#endif
      {
        thread_status=status;
        thread_bounds=bounds;
      }
      if (thread_status == MagickFail)
        continue;

      p=AcquireImagePixels(image,0,y,image->columns,1,exception);
      if (p == (const PixelPacket *) NULL)
        thread_status=MagickFail;
      if (thread_status != MagickFail)
        {
          if (image->matte)
            for (x=0; x < (long) image->columns; x++)
              {
                if (p->opacity != corners[0].opacity)
                  if (x < thread_bounds.x)
                    thread_bounds.x=x;
                if (p->opacity != corners[1].opacity)
                  if (x > (long) thread_bounds.width)
                    thread_bounds.width=x;
                if (p->opacity != corners[0].opacity)
                  if (y < thread_bounds.y)
                    thread_bounds.y=y;
                if (p->opacity != corners[2].opacity)
                  if (y > (long) thread_bounds.height)
                    thread_bounds.height=y;
                p++;
              }
          else
            for (x=0; x < (long) image->columns; x++)
              {
                if (!FuzzyColorMatch(p,&corners[0],image->fuzz))
                  if (x < thread_bounds.x)
                    thread_bounds.x=x;
                if (!FuzzyColorMatch(p,&corners[1],image->fuzz))
                  if (x > (long) thread_bounds.width)
                    thread_bounds.width=x;
                if (!FuzzyColorMatch(p,&corners[0],image->fuzz))
                  if (y < thread_bounds.y)
                    thread_bounds.y=y;
                if (!FuzzyColorMatch(p,&corners[2],image->fuzz))
                  if (y > (long) thread_bounds.height)
                    thread_bounds.height=y;
                p++;
              }
        }

#if defined(HAVE_OPENMP)
#  pragma omp critical
#endif
      {
        row_count++;
        if (QuantumTick(row_count,image->rows))
          if (!MagickMonitorFormatted(row_count,image->rows,exception,
                                      GetImageBoundingBoxText,image->filename))
            thread_status=MagickFail;

        if (thread_bounds.x < bounds.x)
          bounds.x=thread_bounds.x;
        if (thread_bounds.y < bounds.y)
          bounds.y=thread_bounds.y;
        if (thread_bounds.width > bounds.width)
          bounds.width=thread_bounds.width;
        if (thread_bounds.height > bounds.height)
          bounds.height=thread_bounds.height;

        if (thread_status == MagickFail)
          status=MagickFail;
      }
    }
  if ((bounds.width != 0) || (bounds.height != 0))
    {
      bounds.width-=(bounds.x-1);
      bounds.height-=(bounds.y-1);
    }
  if (bounds.x < 0)
    bounds.x=0;
  if (bounds.y < 0)
    bounds.y=0;
  return(bounds);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e C l i p M a s k                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageClipMask returns a reference-counted copy of the current image
%  clip mask. This copy must be deallocated using DestroyImage() once it is
%  no longer needed.  If the image does not have an associated clip mask,
%  then NULL is returned.  Use SetImageClipMask() to add a clip mask to an
%  image, or remove a clip mask.
%
%  If a component of the clip mask is set to TransparentOpacity (maximum
%  value) then the corresponding image pixel component will not be updated
%  when SyncImagePixels() is applied. The clip mask may be used to constrain
%  the results of an image processing operation to a region of the image.
%  Regions outside those allowed by the clip mask may be processed, but only
%  pixel quantums allowed by the clip mask will actually be updated.
%
%  The clip mask protects the DirectClass pixels and PseudoClass pixel indexes
%  from modification. The clip mask does *not* protect the image colormap since
%  the image colormap is globally shared by all pixels in a PseudoClass image.
%
%  The format of the GetImageClipMask method is
%
%      Image *GetImageClipMask(const Image *image, ExceptionInfo *exception)
%
%  A descripton of each parameter follows:
%
%    o image: The image.
%
%    o exception: Reason for failure.
%
*/
MagickExport Image *GetImageClipMask(const Image *image, ExceptionInfo *exception)
{
  if (image->clip_mask)
    return CloneImage(image->clip_mask,0,0,True,exception);

  ThrowException3(exception,ImageError,UnableToGetClipMask,NoImagesWereFound);
  return ((Image *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e D e p t h                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageDepth() returns the minimum bit depth of the image required to
%  ensure that data is not lost in the red, green, blue, and opacity, channels.
%  Pixel components are stored in a Quantum, which is 8, 16, or 32 bits
%  depending on the QuantumDepth value set when the software is compiled.
%  GetImageDepth() returns the smallest modulus storage size which supports
%  the scale of the pixel within the range (i.e. no information is lost).
%  As an example, the value one is returned for a black and white image
%  since only one bit of resolution is required to represent a black and white
%  image.
%
%  The format of the GetImageDepth method is:
%
%      unsigned long GetImageDepth(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static inline unsigned char MinimumDepthForValue(const Quantum quantum)
{
  register unsigned int
    depth,
    scale;
  
  for (depth=1 ; depth < MaxRGB; depth++)
    {
      scale=MaxRGB / (MaxRGB >> (QuantumDepth-depth));
      if (quantum == scale*(quantum/scale))
        break;
    }
  
  return depth;
}
#if MaxMap == MaxRGB
static magick_uint8_t* AllocateDepthMap(void)
{
  magick_uint8_t 
    *map;
  
  map = MagickAllocateArray(unsigned char *, MaxRGB, sizeof(magick_uint8_t));
  if (map != (unsigned char *) NULL)
    {
      unsigned int
        i;
      
      for (i=0; i <= MaxRGB; i++)
        map[i] = (magick_uint8_t) MinimumDepthForValue(i);
    }
  return map;
}
#endif /* MaxMap == MaxRGB */
#define GetImageDepthText "[%s] Get image depth..."

static MagickPassFail
GetImageDepthCallBack(void *mutable_data,          /* User provided mutable data */
                      const void *immutable_data,  /* User provided immutable data */
                      const Image *image,          /* Input image */
                      const PixelPacket *pixels,   /* Pixel row */
                      const IndexPacket *indexes,  /* Pixel indexes */
                      const long npixels,          /* Number of pixels in row */
                      ExceptionInfo *exception     /* Exception report */
                      )
{
  unsigned int
    *current_depth=(unsigned int *) mutable_data;

  magick_uint8_t
    *map = (magick_uint8_t *) immutable_data;

  register unsigned int
    depth;

  register long
    i;

  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

#if defined(HAVE_OPENMP)
#  pragma omp critical
#endif
  {
    depth=*current_depth;
  }

#if MaxMap == MaxRGB
  if (map)
    {
      /*
        Use fast table lookups if we can
      */
      for (i=0; i < npixels; i++)
        {
          depth=Max(depth,map[pixels[i].red]);
          depth=Max(depth,map[pixels[i].green]);
          depth=Max(depth,map[pixels[i].blue]);
          if (image->matte)
            depth=Max(depth,map[pixels[i].opacity]);
          if (depth == QuantumDepth)
            break;
        }
    }
#else
    {
      /*
        Use the slow, sure, way (Q32 only)
      */
      register unsigned int
        scale;

      ARG_NOT_USED(map);
      scale=MaxRGB / (MaxRGB >> (QuantumDepth-depth));
      i=0;
      while (i < npixels)
        {
          if ((pixels[i].red != scale*(pixels[i].red/scale)) ||
              (pixels[i].green != scale*(pixels[i].green/scale)) ||
              (pixels[i].blue != scale*(pixels[i].blue/scale)) ||
              (image->matte &&
               (pixels[i].opacity != scale*((pixels[i].opacity/scale)))))
            {
              depth++;
              if (depth == QuantumDepth)
                break;
              scale=MaxRGB / (MaxRGB >> (QuantumDepth-depth));
              continue;
            }
          i++;
        }
    }
#endif

#if defined(HAVE_OPENMP)
#  pragma omp critical
#endif
  {
    if (depth > *current_depth)
      *current_depth=depth;
  }

  return (depth >= QuantumDepth ? MagickFail : MagickPass);
}

MagickExport unsigned long GetImageDepth(const Image *image,
                                         ExceptionInfo *exception)
{
  magick_uint8_t
    *map = (magick_uint8_t *) NULL;

  unsigned int
    depth=1;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  if (image->is_monochrome)
    return depth;

#if MaxMap == MaxRGB
  /*
    Use fast table lookups if we can
  */
  map = AllocateDepthMap();
#endif
  if ((image->storage_class == PseudoClass) && !(image->matte))
    {
      /*
        PseudoClass
      */
      (void) GetImageDepthCallBack(&depth,map,image,
                                   image->colormap,
                                   (IndexPacket *) NULL,
                                   image->colors,
                                   exception);
    }
  else
    {
      /*
        DirectClass.
       
        Notice that all pixels in the image must be inspected if the
        image depth is less than QuantumDepth.
      */
     
      (void) PixelIterateMonoRead(GetImageDepthCallBack,
                                  NULL,
                                  GetImageDepthText,
                                  &depth,map,0,0,image->columns,
                                  image->rows,image,exception);
    }

  return depth;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e E x c e p t i o n                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageException() traverses an image sequence and returns any
%  error more severe than noted by the exception parameter.
%
%  The format of the GetImageException method is:
%
%      void GetImageException(Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: Specifies a pointer to a list of one or more images.
%
%    o exception: return the highest severity exception.
%
%
*/
MagickExport void GetImageException(Image *image,ExceptionInfo *exception)
{
  register Image
    *next;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  for (next=image; next != (Image *) NULL; next=next->next)
  {
    if (next->exception.severity == UndefinedException)
      continue;
    if (next->exception.severity > exception->severity)
      CopyException(exception,&next->exception);
    next->exception.severity=UndefinedException;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e C h a r a c t e r i s t i c s                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageCharacteristics obtains the basic characteristics of the image
%  and stores the characterisistics in the user provided
%  ImageCharacteristics structure.  If optimize is set to MagickTrue, then
%  exhaustive testing of the image pixels is performed (as required).
%  MagickPass is returned if this method executes without error.
%
%  The format of the GetImageCharacteristics method is:
%
%      MagickPassFail GetImageCharacteristics(const Image *image,
%                               ImageCharacteristics *characteristics,
%                               MagickBool optimize,
%                               ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o characteristics: An ImageCharacteristics structure to update.
%
%    o optimize: Inspect image pixels (if required)
%
%    o exception: Any errors are reported here.
%
*/
#define AnalyzeImageText "[%s] Analyze image...  "
MagickExport MagickPassFail GetImageCharacteristics(const Image *image,
                                                    ImageCharacteristics *characteristics,
                                                    const MagickBool optimize,
                                                    ExceptionInfo *exception)
{
  unsigned long
    y;

  register const PixelPacket
    *p;

  register unsigned long
    x;

  MagickBool
    broke_loop = MagickFalse;

  MagickPassFail
    status = MagickPass;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(characteristics != (ImageCharacteristics *) NULL);
  assert(exception != (ExceptionInfo *) NULL);

  characteristics->cmyk = (image->colorspace == CMYKColorspace ? MagickTrue : MagickFalse);
  characteristics->grayscale = (image->is_grayscale ? MagickTrue : MagickFalse);
  characteristics->monochrome = (image->is_monochrome ? MagickTrue : MagickFalse);
  characteristics->opaque = (image->matte ? MagickFalse : MagickTrue);
  characteristics->palette = (image->storage_class == PseudoClass ? MagickTrue : MagickFalse);

  if ((optimize) && (GetPixelCachePresent(image)))
    {
      MagickBool
        grayscale,
        monochrome,
        opaque;

      /* Predicate to test */
      grayscale=(image->is_grayscale ? MagickFalse : MagickTrue);
      monochrome=(image->is_monochrome ? MagickFalse : MagickTrue);
      opaque=(image->matte ? MagickTrue : MagickFalse);
      switch (image->storage_class)
        {
        case DirectClass:
        case UndefinedClass:
          {
            for (y=0; y < image->rows; y++)
              {
                p=AcquireImagePixels(image,0,y,image->columns,1,exception);
                if (p == (const PixelPacket *) NULL)
                  {
                    status = MagickFail;
                    break;
                  }
                for (x=image->columns; x != 0; x--)
                  {
                    grayscale = ((grayscale) &&
                                 (p->red == p->green) && (p->red == p->blue));
                    monochrome = ((monochrome) && (grayscale) &&
                                  ((0 == p->red) || (MaxRGB == p->red)));
                    opaque = ((opaque) &&
                              (p->opacity == OpaqueOpacity));
                    if (!grayscale &&
                        !monochrome &&
                        !opaque)
                      {
                        broke_loop=MagickTrue;
                        break;
                      }
                    p++;
                  }
                if (!grayscale &&
                    !monochrome &&
                    !opaque)
                  break;
                if (QuantumTick(y,image->rows))
                  if (!MagickMonitorFormatted(y,image->rows,exception,
                                              AnalyzeImageText,image->filename))
                    break;
              }
            break;
          }
        case PseudoClass:
          {
            p=image->colormap;
            for (x=image->colors; x != 0; x--)
              {
                grayscale = ((grayscale) &&
                             (p->red == p->green) && (p->red == p->blue));
                monochrome = ((monochrome) && (grayscale) &&
                              ((0 == p->red) || (MaxRGB == p->red)));
                if (!grayscale &&
                    !monochrome)
                  {
                    broke_loop=MagickTrue;
                    break;
                  }
                p++;
              }
            if (!opaque)
              {
                for (y=0; y < image->rows; y++)
                  {
                    p=AcquireImagePixels(image,0,y,image->columns,1,exception);
                    if (p == (const PixelPacket *) NULL)
                      {
                        status = MagickFail;
                        break;
                      }
                    for (x=image->columns; x != 0; x--)
                      {
                        opaque = ((opaque) &&
                                  (p->opacity == OpaqueOpacity));
                        if (!opaque)
                          {
                            broke_loop=MagickTrue;
                            break;
                          }
                        p++;
                      }
                    if (!opaque)
                      break;
                    if (QuantumTick(y,image->rows))
                      if (!MagickMonitorFormatted(y,image->rows,exception,
                                                  AnalyzeImageText,image->filename))
                        break;
                  }
              }
            break;
          }
        }
      if (!characteristics->grayscale)
        {
          characteristics->grayscale=grayscale;
          ((Image *)image)->is_grayscale=grayscale; /* Intentionally ignore const */
        }
      if (!characteristics->monochrome)
        {
          characteristics->monochrome=monochrome;
          ((Image *)image)->is_monochrome=monochrome; /* Intentionally ignore const */
        }
      if (!characteristics->opaque)
        characteristics->opaque=opaque;
    }

  /*
    Force progress indication to 100%
  */
  if (broke_loop)
    (void) MagickMonitorFormatted(image->rows-1,image->rows,exception,
                                  AnalyzeImageText,image->filename);
/*   printf("status=%s, cmyk=%u, grayscale=%u, monochrome=%u, opaque=%u, palette=%u\n", */
/*          (status == MagickFail ? "Fail" : "Pass"),characteristics->cmyk,characteristics->grayscale, */
/*          characteristics->monochrome,characteristics->opaque,characteristics->palette); */

  return status;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e G e o m e t r y                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageGeometry() returns a region as defined by the geometry string with
%  respect to the image and its gravity.
%
%  The format of the GetImageGeometry method is:
%
%      int GetImageGeometry(const Image *image,const char *geometry,
%        const unsigned int size_to_fit,RectangleInfo *region_info)
%
%  A description of each parameter follows:
%
%    o flags:  Method GetImageGeometry returns a bitmask that indicates
%      which of the four values were located in the geometry string.
%
%    o geometry:  The geometry (e.g. 100x100+10+10).
%
%    o size_to_fit:  A value other than 0 means to scale the region so it
%      fits within the specified width and height.
%
%    o region_info: The region as defined by the geometry string with
%      respect to the image and its gravity.
%
%
*/
MagickExport int GetImageGeometry(const Image *image,const char *geometry,
  const unsigned int size_to_fit,RectangleInfo *region_info)
{
  char
    region_geometry[MaxTextExtent];

  int
    flags;

  region_info->width=image->columns;
  region_info->height=image->rows;
  region_info->x=0;
  region_info->y=0;
  (void) strlcpy(region_geometry,geometry,MaxTextExtent);
  if (!size_to_fit)
    (void) strlcat(region_geometry,"!",MaxTextExtent);
  flags=GetMagickGeometry(region_geometry,&region_info->x,&region_info->y,
    &region_info->width,&region_info->height);
  switch (image->gravity)
  {
    case ForgetGravity:
    case NorthWestGravity:
      break;
    case NorthGravity:
    {
      region_info->x+=(long) (image->columns/2-region_info->width/2);
      break;
    }
    case NorthEastGravity:
    {
      region_info->x=(long) (image->columns-region_info->width-region_info->x);
      break;
    }
    case WestGravity:
    {
      region_info->y+=(long) (image->rows/2-region_info->height/2);
      break;
    }
    case StaticGravity:
    case CenterGravity:
    default:
    {
      region_info->x+=(long) (image->columns/2-region_info->width/2);
      region_info->y+=(long) (image->rows/2-region_info->height/2);
      break;
    }
    case EastGravity:
    {
      region_info->x=(long) (image->columns-region_info->width-region_info->x);
      region_info->y+=(long) (image->rows/2-region_info->height/2);
      break;
    }
    case SouthWestGravity:
    {
      region_info->y=(long) (image->rows-region_info->height-region_info->y);
      break;
    }
    case SouthGravity:
    {
      region_info->x+=(long) (image->columns/2-region_info->width/2);
      region_info->y=(long) (image->rows-region_info->height-region_info->y);
      break;
    }
    case SouthEastGravity:
    {
      region_info->x=(long) (image->columns-region_info->width-region_info->x);
      region_info->y=(long) (image->rows-region_info->height-region_info->y);
      break;
    }
  }
  return(flags);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e I n f o                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageInfo() initializes image_info to default values.
%
%  The format of the GetImageInfo method is:
%
%      void GetImageInfo(ImageInfo *image_info)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%
*/
MagickExport void GetImageInfo(ImageInfo *image_info)
{
  ExceptionInfo
    exception;

  /*
    File and image dimension members.
  */
  assert(image_info != (ImageInfo *) NULL);
  (void) memset(image_info,0,sizeof(ImageInfo));
  image_info->adjoin=True;
  image_info->depth=QuantumDepth;
  image_info->interlace=NoInterlace;
  image_info->quality=DefaultCompressionQuality;
  image_info->antialias=True;
  image_info->pointsize=12;
  image_info->dither=True;
  image_info->progress=True;
  GetExceptionInfo(&exception);
  (void) QueryColorDatabase(BackgroundColor,&image_info->background_color,
    &exception);
  (void) QueryColorDatabase(BorderColor,&image_info->border_color,&exception);
  (void) QueryColorDatabase(MatteColor,&image_info->matte_color,&exception);
  DestroyExceptionInfo(&exception);
  image_info->signature=MagickSignature;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e S t a t i s t i c s                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageStatistics computes common statistics (currently maximum, minimum,
%  mean and standard deviation) for the available image channels. The
%  per-channel values are returned in an ImageStatistics structure. Statistics
%  are normalized to the range 0.0 to 1.0.  Multiply values by MaxRGB to obtain
%  the statistics in quantum units. Statistics for non-existent channels are
%  set to zero.
%
%  The format of the GetImageStatistics method is:
%
%      MagickPassFail GetImageStatistics(const Image *image,
%                                        ImageStatistics *statistics
%                                        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o statistics: An ImageStatistics structure to update with statistics.
%
%    o exception: Any errors are reported here.
%
*/
typedef struct _StatisticsContext {
  double samples;
  double variance_divisor;
} StatisticsContext;
static MagickPassFail GetImageStatisticsMean(void *mutable_data,
                                             const void *immutable_data,
                                             const Image *image,
                                             const PixelPacket *pixel,
                                             const IndexPacket *indexes,
                                             const long npixels,
                                             ExceptionInfo *exception)
{
  ImageStatistics
    lstatistics,
    *statistics=(ImageStatistics *) mutable_data;

  const StatisticsContext
    *context=(const StatisticsContext *) immutable_data;

  double
    normalized;

  register long
    i;

  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  (void) memset(&lstatistics, 0, sizeof(ImageStatistics));
  lstatistics.red.minimum=1.0;
  lstatistics.green.minimum=1.0;
  lstatistics.blue.minimum=1.0;
  if (image->matte)
    lstatistics.opacity.minimum=1.0;

  for (i=0; i < npixels; i++)
    {
      normalized=(double) pixel[i].red/MaxRGB;
      lstatistics.red.mean += normalized/context->samples;
      if (normalized > lstatistics.red.maximum)
        lstatistics.red.maximum=normalized;
      if (normalized <  lstatistics.red.minimum)
        lstatistics.red.minimum=normalized;

      normalized=(double) pixel[i].green/MaxRGB;
      lstatistics.green.mean += normalized/context->samples;
      if (normalized > lstatistics.green.maximum)
        lstatistics.green.maximum=normalized;
      if (normalized <  lstatistics.green.minimum)
        lstatistics.green.minimum=normalized;

      normalized=(double) pixel[i].blue/MaxRGB;
      lstatistics.blue.mean += normalized/context->samples;
      if (normalized > lstatistics.blue.maximum)
        lstatistics.blue.maximum=normalized;
      if (normalized <  lstatistics.blue.minimum)
        lstatistics.blue.minimum=normalized;

      if (image->matte)
        {
          normalized=(double) pixel[i].opacity/MaxRGB;
          lstatistics.opacity.mean += normalized/context->samples;
          if (normalized > lstatistics.opacity.maximum)
            lstatistics.opacity.maximum=normalized;
          if (normalized <  lstatistics.opacity.minimum)
            lstatistics.opacity.minimum=normalized;
        }
    }

#if defined(HAVE_OPENMP)
#  pragma omp critical
#endif
  {
    statistics->red.mean += lstatistics.red.mean;
    if (lstatistics.red.maximum > statistics->red.maximum)
      statistics->red.maximum=lstatistics.red.maximum;
    if (lstatistics.red.minimum < statistics->red.minimum)
      statistics->red.minimum=lstatistics.red.minimum;

    statistics->green.mean += lstatistics.green.mean;
    if (lstatistics.green.maximum > statistics->green.maximum)
      statistics->green.maximum=lstatistics.green.maximum;
    if (lstatistics.green.minimum < statistics->green.minimum)
      statistics->green.minimum=lstatistics.green.minimum;

    statistics->blue.mean += lstatistics.blue.mean;
    if (lstatistics.blue.maximum > statistics->blue.maximum)
      statistics->blue.maximum=lstatistics.blue.maximum;
    if (lstatistics.blue.minimum < statistics->blue.minimum)
      statistics->blue.minimum=lstatistics.blue.minimum;

    if (image->matte)
      {
        statistics->opacity.mean += lstatistics.opacity.mean;
        if (lstatistics.opacity.maximum > statistics->opacity.maximum)
          statistics->opacity.maximum=lstatistics.opacity.maximum;
        if (lstatistics.opacity.minimum < statistics->opacity.minimum)
          statistics->opacity.minimum=lstatistics.opacity.minimum;
      }
  }

  return MagickPass;
}
#define Square(x)  ((x)*(x))
static MagickPassFail GetImageStatisticsVariance(void *mutable_data,
                                                 const void *immutable_data,
                                                 const Image *image,
                                                 const PixelPacket *pixel,
                                                 const IndexPacket *indexes,
                                                 const long npixels,
                                                 ExceptionInfo *exception)
{
  ImageStatistics
    lstatistics,
    *statistics=(ImageStatistics *) mutable_data;

  const StatisticsContext
    *context=(const StatisticsContext *) immutable_data;

  double
    normalized;

  register long
    i;

  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  (void) memset(&lstatistics, 0, sizeof(ImageStatistics));
#if defined(HAVE_OPENMP)
#  pragma omp critical
#endif
  {
    lstatistics.red.mean=statistics->red.mean;
    lstatistics.green.mean=statistics->green.mean;
    lstatistics.blue.mean=statistics->blue.mean;
    lstatistics.opacity.mean=statistics->opacity.mean;
  }

  for (i=0; i < npixels; i++)
    {
      normalized=(double) pixel[i].red/MaxRGB;
      lstatistics.red.variance +=
        Square(normalized-lstatistics.red.mean)/context->variance_divisor;
      
      normalized=(double) pixel[i].green/MaxRGB;
      lstatistics.green.variance +=
        Square(normalized-lstatistics.green.mean)/context->variance_divisor;
      
      normalized=(double) pixel[i].blue/MaxRGB;
      lstatistics.blue.variance +=
        Square(normalized-lstatistics.blue.mean)/context->variance_divisor;
      
      if (image->matte)
        {
          normalized=(double) pixel[i].opacity/MaxRGB;
          lstatistics.opacity.variance +=
            Square(normalized-lstatistics.opacity.mean)/context->variance_divisor;
        }
    }

#if defined(HAVE_OPENMP)
#  pragma omp critical
#endif
  {
    statistics->red.variance += lstatistics.red.variance;
    statistics->green.variance += lstatistics.green.variance;
    statistics->blue.variance += lstatistics.blue.variance;
    statistics->opacity.variance += lstatistics.opacity.variance;
  }

  return MagickPass;
}
MagickExport MagickPassFail GetImageStatistics(const Image *image,
                                               ImageStatistics *statistics,
                                               ExceptionInfo *exception)
{
  StatisticsContext
    context;
  
  MagickPassFail
    status=MagickPass;

  double
    samples;
  
  (void) memset((void *) statistics, 0, sizeof(ImageStatistics));
  statistics->red.minimum=1.0;
  statistics->green.minimum=1.0;
  statistics->blue.minimum=1.0;
  if (image->matte)
    statistics->opacity.minimum=1.0;

  samples=(double) image->rows*image->columns;
  context.samples=samples;
  context.variance_divisor=samples-1;
  
  /*
    Compute Mean, Max, and Min
  */
  status = PixelIterateMonoRead(GetImageStatisticsMean,
                                NULL,
                                "[%s] Compute image mean, max, min...",
                                statistics,&context,0,0,image->columns,
                                image->rows,image,exception);
  /*
    Compute Variance
  */
  if (status == MagickPass)
    status = PixelIterateMonoRead(GetImageStatisticsVariance,
                                  NULL,
                                  "[%s] Compute image variance...",
                                  statistics,&context,0,0,image->columns,
                                  image->rows,image,exception);
  /*
    Compute Standard Deviation
  */
  if (status == MagickPass)
    {
      statistics->red.standard_deviation=sqrt(statistics->red.variance);
      statistics->green.standard_deviation=sqrt(statistics->green.variance);
      statistics->blue.standard_deviation=sqrt(statistics->blue.variance);
      if (image->matte)
        statistics->opacity.standard_deviation=sqrt(statistics->opacity.variance);
    }

  return status;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e T y p e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageType() returns the type of image:
%
%        Bilevel        Grayscale       GrayscaleMatte
%        Palette        PaletteMatte    TrueColor
%        TrueColorMatte ColorSeparation ColorSeparationMatte
%
%
%  The format of the GetImageType method is:
%
%      ImageType GetImageType(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

MagickExport ImageType GetImageType(const Image *image,ExceptionInfo *exception)
{
  ImageCharacteristics
    characteristics;

  ImageType
    image_type;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  image_type=UndefinedType;
  if (GetImageCharacteristics(image,&characteristics,MagickTrue,exception))
    {
      if (characteristics.cmyk)
        image_type=(characteristics.opaque ? ColorSeparationType : ColorSeparationMatteType);
      else if (characteristics.monochrome)
        image_type=BilevelType;
      else if (characteristics.grayscale)
        image_type=(characteristics.opaque ? GrayscaleType : GrayscaleMatteType);
      else if (characteristics.palette)
        image_type=(characteristics.opaque ? PaletteType : PaletteMatteType);
      else
        image_type=(characteristics.opaque ? TrueColorType : TrueColorMatteType);
    }
  return image_type;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
+     G r a d i e n t I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GradientImage() applies a continuously smooth color transitions along a
%  vector from one color to another.
%
%  Note, the interface of this method will change in the future to support
%  more than one transistion.
%
%  The format of the GradientImage method is:
%
%      unsigned int GradientImage(Image *image,const PixelPacket *start_color,
%        const PixelPacket *stop_color)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o start_color: The start color.
%
%    o stop_color: The stop color.
%
%
*/

#define GradientImageText "[%s] Gradient image..."
MagickExport MagickPassFail GradientImage(Image *image,
                                          const PixelPacket *start_color,
                                          const PixelPacket *stop_color)
{
  const unsigned long
    image_rows=image->rows,
    image_columns=image->columns;

  long
    y;

  unsigned long
    row_count=0;

  MagickPassFail
    status=MagickPass;

  /*
    Determine (Hue, Saturation, Brightness) gradient.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(start_color != (const PixelPacket *) NULL);
  assert(stop_color != (const PixelPacket *) NULL);

  /*
    Generate gradient pixels.
  */
#if defined(HAVE_OPENMP)
#  pragma omp parallel for shared(row_count, status)
#endif
  for (y=0; y < (long) image->rows; y++)
    {
      MagickPassFail
        thread_status;

      register long
        x;
      
      register PixelPacket
        *q;

      thread_status=status;
      if (thread_status == MagickFail)
        continue;

      q=SetImagePixelsEx(image,0,y,image->columns,1,&image->exception);
      if (q == (PixelPacket *) NULL)
        thread_status=MagickFail;

      if (q != (PixelPacket *) NULL)
        {
          for (x=0; x < (long) image->columns; x++)
            {
              BlendCompositePixel(&q[x],start_color,stop_color,(double)
                                  MaxRGB*(y*image_columns+x)/(image_columns*image_rows));
            }

          if (!SyncImagePixelsEx(image,&image->exception))
            thread_status=MagickFail;
        }

#if defined(HAVE_OPENMP)
#  pragma omp critical
#endif
      {
        row_count++;
        if (QuantumTick(row_count,image->rows))
          if (!MagickMonitorFormatted(row_count,image->rows,&image->exception,
                                      GradientImageText,image->filename))
            thread_status=MagickFail;

        if (thread_status == MagickFail)
          status=MagickFail;
      }
    }
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
+     I s S u b i m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsSubimage returns True if the geometry is a valid subimage
%  specification (e.g. [1], [1-9], [1,7,4]).
%
%  The format of the IsSubimage method is:
%
%      unsigned int IsSubimage(const char *geometry,const unsigned int pedantic)
%
%  A description of each parameter follows:
%
%    o status: Method IsSubimage returns True if the geometry is a valid
%      subimage specification otherwise False is returned.
%
%    o geometry: This string is the geometry specification.
%
%    o pedantic: A value other than 0 invokes a more restriction set of
%      conditions for a valid specification (e.g. [1], [1-4], [4-1]).
%
%
*/
MagickExport unsigned int IsSubimage(const char *geometry,
  const unsigned int pedantic)
{
  long
    x,
    y;

  unsigned int
    flags;

  unsigned long
    height,
    width;

  if (geometry == (const char *) NULL)
    return(False);
  flags=GetGeometry((char *) geometry,&x,&y,&width,&height);
  if (pedantic)
    return((flags != NoValue) && !(flags & HeightValue));
  return(IsGeometry(geometry) && !(flags & HeightValue));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     I s T a i n t I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IsTaintImage() returns a value other than 0 if any pixel in the image
%  has been altered since it was first constituted.
%
%  The format of the IsTaintImage method is:
%
%      unsigned int IsTaintImage(const Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
MagickExport unsigned int IsTaintImage(const Image *image)
{
  char
    magick[MaxTextExtent],
    filename[MaxTextExtent];

  register const Image
    *p;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  (void) strlcpy(magick,image->magick,MaxTextExtent);
  (void) strlcpy(filename,image->filename,MaxTextExtent);
  for (p=image; p != (Image *) NULL; p=p->next)
  {
    if (p->taint)
      return(True);
    if (LocaleCompare(p->magick,magick) != 0)
      return(True);
    if (LocaleCompare(p->filename,filename) != 0)
      return(True);
  }
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M o d i f y I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ModifyImage() ensures that there is only a single reference to the image
%  to be modified, updating the provided image pointer to point to a clone of
%  the original image if necessary.
%
%  The format of the ModifyImage method is:
%
%      ModifyImage(Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport void ModifyImage(Image **image,ExceptionInfo *exception)
{
  Image
    *clone_image;

  unsigned int
    clone;

  assert(image != (Image **) NULL);
  assert(*image != (Image *) NULL);
  assert((*image)->signature == MagickSignature);
  clone=False;
  AcquireSemaphoreInfo((SemaphoreInfo **) &(*image)->semaphore);
  if ((*image)->reference_count > 1)
    clone=True;
  LiberateSemaphoreInfo((SemaphoreInfo **) &(*image)->semaphore);
  if (!clone)
    return;
  clone_image=CloneImage(*image,0,0,True,exception);
  AcquireSemaphoreInfo((SemaphoreInfo **) &(*image)->semaphore);
  (*image)->reference_count--;
  LiberateSemaphoreInfo((SemaphoreInfo **) &(*image)->semaphore);
  *image=clone_image;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     P l a s m a I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PlasmaImage() initializes an image with plasma fractal values.  The image
%  must be initialized with a base color and the random number generator
%  seeded before this method is called.
%
%  The format of the PlasmaImage method is:
%
%      unsigned int PlasmaImage(Image *image,const SegmentInfo *segment,
%        unsigned long attenuate,unsigned long depth)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o segment:   Define the region to apply plasma fractals values.
%
%    o attenuate: Define the plasma attenuation factor.
%
%    o depth: Limit the plasma recursion depth.
%
%
*/

static inline Quantum PlasmaPixel(const double pixel,const double noise)
{
  double
    value;

  value=pixel+noise*rand()/RAND_MAX-noise/2;
  if (value <= 0.0)
    return(0);
  if (value >= MaxRGB)
    return(MaxRGB);
  return((Quantum) (value+0.5));
}

MagickExport MagickPassFail PlasmaImage(Image *image,const SegmentInfo *segment,
  unsigned long attenuate,unsigned long depth)
{
  double
    plasma;

  long
    x,
    x_mid,
    y,
    y_mid;

  PixelPacket
    u,
    v;

  register PixelPacket
    *q;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(segment != (SegmentInfo *) NULL);
  if (depth != 0)
    {
      SegmentInfo
        local_info;

      /*
        Divide the area into quadrants and recurse.
      */
      depth--;
      attenuate++;
      x_mid=(long) (segment->x1+segment->x2+0.5)/2;
      y_mid=(long) (segment->y1+segment->y2+0.5)/2;
      local_info=(*segment);
      local_info.x2=x_mid;
      local_info.y2=y_mid;
      (void) PlasmaImage(image,&local_info,attenuate,depth);
      local_info=(*segment);
      local_info.y1=y_mid;
      local_info.x2=x_mid;
      (void) PlasmaImage(image,&local_info,attenuate,depth);
      local_info=(*segment);
      local_info.x1=x_mid;
      local_info.y2=y_mid;
      (void) PlasmaImage(image,&local_info,attenuate,depth);
      local_info=(*segment);
      local_info.x1=x_mid;
      local_info.y1=y_mid;
      return(PlasmaImage(image,&local_info,attenuate,depth));
    }
  if ((image->colorspace != RGBColorspace) ||
      (image->storage_class != DirectClass))
    {
      (void) SetImageType(image,TrueColorType);
    }
  x_mid=(long) (segment->x1+segment->x2+0.5)/2;
  y_mid=(long) (segment->y1+segment->y2+0.5)/2;
  if ((segment->x1 == x_mid) && (segment->x2 == x_mid) &&
      (segment->y1 == y_mid) && (segment->y2 == y_mid))
    return(False);
  /*
    Average pixels and apply plasma.
  */
  plasma=MaxRGB/(2.0*attenuate);
  if ((segment->x1 != x_mid) || (segment->x2 != x_mid))
    {
      /*
        Left pixel.
      */
      x=(long) (segment->x1+0.5);
      (void) AcquireOnePixelByReference(image,&u,x,(long) (segment->y1+0.5),&image->exception);
      (void) AcquireOnePixelByReference(image,&v,x,(long) (segment->y2+0.5),&image->exception);
      q=SetImagePixels(image,x,y_mid,1,1);
      if (q == (PixelPacket *) NULL)
        return(True);
      q->red=PlasmaPixel(((double) u.red+v.red)/2,plasma);
      q->green=PlasmaPixel(((double) u.green+v.green)/2,plasma);
      q->blue=PlasmaPixel(((double) u.blue+v.blue)/2,plasma);
      (void) SyncImagePixels(image);
      if (segment->x1 != segment->x2)
        {
          /*
            Right pixel.
          */
          x=(long) (segment->x2+0.5);
          (void) AcquireOnePixelByReference(image,&u,x,(long) (segment->y1+0.5),&image->exception);
          (void) AcquireOnePixelByReference(image,&v,x,(long) (segment->y2+0.5),&image->exception);
          q=SetImagePixels(image,x,y_mid,1,1);
          if (q == (PixelPacket *) NULL)
            return(True);
          q->red=PlasmaPixel(((double) u.red+v.red)/2,plasma);
          q->green=PlasmaPixel(((double) u.green+v.green)/2,plasma);
          q->blue=PlasmaPixel(((double) u.blue+v.blue)/2,plasma);
          (void) SyncImagePixels(image);
        }
    }
  if ((segment->y1 != y_mid) || (segment->y2 != y_mid))
    {
      if ((segment->x1 != x_mid) || (segment->y2 != y_mid))
        {
          /*
            Bottom pixel.
          */
          y=(long) (segment->y2+0.5);
          (void) AcquireOnePixelByReference(image,&u,(long) (segment->x1+0.5),y,&image->exception);
          (void) AcquireOnePixelByReference(image,&v,(long) (segment->x2+0.5),y,&image->exception);
          q=SetImagePixels(image,x_mid,y,1,1);
          if (q == (PixelPacket *) NULL)
            return(True);
          q->red=PlasmaPixel(((double) u.red+v.red)/2,plasma);
          q->green=PlasmaPixel(((double) u.green+v.green)/2,plasma);
          q->blue=PlasmaPixel(((double) u.blue+v.blue)/2,plasma);
          (void) SyncImagePixels(image);
        }
      if (segment->y1 != segment->y2)
        {
          /*
            Top pixel.
          */
          y=(long) (segment->y1+0.5);
          (void) AcquireOnePixelByReference(image,&u,(long) (segment->x1+0.5),y,&image->exception);
          (void) AcquireOnePixelByReference(image,&v,(long) (segment->x2+0.5),y,&image->exception);
          q=SetImagePixels(image,x_mid,y,1,1);
          if (q == (PixelPacket *) NULL)
            return(True);
          q->red=PlasmaPixel(((double) u.red+v.red)/2,plasma);
          q->green=PlasmaPixel(((double) u.green+v.green)/2,plasma);
          q->blue=PlasmaPixel(((double) u.blue+v.blue)/2,plasma);
          (void) SyncImagePixels(image);
        }
    }
  if ((segment->x1 != segment->x2) || (segment->y1 != segment->y2))
    {
      /*
        Middle pixel.
      */
      x=(long) (segment->x1+0.5);
      y=(long) (segment->y1+0.5);
      (void) AcquireOnePixelByReference(image,&u,x,y,&image->exception);
      x=(long) (segment->x2+0.5);
      y=(long) (segment->y2+0.5);
      (void) AcquireOnePixelByReference(image,&v,x,y,&image->exception);
      q=SetImagePixels(image,x_mid,y_mid,1,1);
      if (q == (PixelPacket *) NULL)
        return(True);
      q->red=PlasmaPixel(((double) u.red+v.red)/2,plasma);
      q->green=PlasmaPixel(((double) u.green+v.green)/2,plasma);
      q->blue=PlasmaPixel(((double) u.blue+v.blue)/2,plasma);
      (void) SyncImagePixels(image);
    }
  if (((segment->x2-segment->x1) < 3.0) && ((segment->y2-segment->y1) < 3.0))
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e f e r e n c e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReferenceImage() increments the reference count associated with an image
%  returning a pointer to the image.
%
%  The format of the ReferenceImage method is:
%
%      Image *ReferenceImage(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
MagickExport Image *ReferenceImage(Image *image)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  AcquireSemaphoreInfo((SemaphoreInfo **) &image->semaphore);
  image->reference_count++;
  LiberateSemaphoreInfo((SemaphoreInfo **) &image->semaphore);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e m o v e D e f i n i t i o n s                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  RemoveDefinitions() removes definitions from the current map of definitions
%  in ImageInfo. Definitions may be used by coders/decoders that read and
%  write images. RemoveDefinitions() returns true only if the specified keys
%  are present in the map and are actually removed.
%
%  The format of the RemoveDefinitions method is:
%
%      void RemoveDefinitions(ImageInfo *image_info,const char *options)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o keys: List of keys to remove from the definitions map. The
%      format of the string is "key1,key2,...". A special key, '*', removes
%      all the key/value pairs in the definitions map. This key always
%      succeeds.
%
%    o exception: Errors result in updates to this structure.
%
*/
MagickExport MagickPassFail
RemoveDefinitions(const ImageInfo *image_info,const char *keys)
{
  char
    key[MaxTextExtent];

  unsigned int
    status;

  unsigned int
    i,
    j;

  size_t
    length;

  if (image_info->definitions == 0)
    return(False);

  status=MagickPass;

  /*
    TODO: update to accept GlobExpression as argument names list.
    That would require the iterator to know we're manipulating
    the map or a remove method on the map iterator itself.
    Until then we accept a simple "*" to mean clear the whole map.
  */
  length=strlen(keys);
  i=0;
  while (i < length)
  {
    for (j=0; (i < length) && (keys[i] != ','); i++,j++)
      key[j]=keys[i];
    key[j]='\0';
    i++;
    if (strlen(key) != 0)
      {
        if ((key[0] == '*') && (key[1] == '\0'))
          MagickMapClearMap((MagickMap) image_info->definitions);
        else
          status &= MagickMapRemoveEntry((MagickMap) image_info->definitions,key);
      }
    else
      {
        status=MagickFail;
        break;
      }
  }
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%   R e p l a c e I m a g e C o l o r m a p                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReplaceImageColormap() replaces an existing image colormap with a new
%  image colormap. The new image colormap is expected to contain all of the
%  colors from the existing colormap. The existing colormap indexes are
%  adjusted to conform with positions in the new colormap. If the new
%  colormap contains duplicate entries, then the associated colormap index
%  will point to the first entry found in the colormap and other matching
%  entries will not be used. MagickPass is returned if the operation is
%  successful, otherwise MagickFail is returned, and image->exception is
%  updated with the cause of the failure.
%
%  This function is useful in case colormap entries need to match across
%  multiple images or otherwise occupy specific locations.
%
%  The format of the ReplaceImageColormap method is:
%
%      MagickPassFail ReplaceImageColormap(Image *image,
%                                          const PixelPacket *colormap,
%                                          const unsigned int colors)
%
%  A description of each parameter follows:
%
%    o image: image in which to replace colormap.
%
%    o colormap: new colormap.
%
%    o colors: number of colors in new colormap.
%
*/
#define ReplaceImageColormapText "[%s] Replacing image colormap..."
static MagickPassFail
ReplaceImageColormapCallBack(void *mutable_data,         /* User provided mutable data */
                             const void *immutable_data, /* User provided immutable data */
                             Image *image,               /* Modify image */
                             PixelPacket *pixels,        /* Pixel row */
                             IndexPacket *indexes,       /* Pixel row indexes */
                             const long npixels,         /* Number of pixels in row */
                             ExceptionInfo *exception)   /* Exception report */
{
  /*
    Replace image colormap
  */

  const unsigned int
    *colormap_index=(const unsigned int *) immutable_data;

  register long
    i;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(pixels);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  for (i=0; i < npixels; i++)
    indexes[i]=colormap_index[indexes[i]];

  return MagickPass;
}
MagickExport MagickPassFail
ReplaceImageColormap(Image *image,
                     const PixelPacket *colormap,
                     const unsigned int colors)
{
  unsigned int
    *colormap_index=(unsigned int *) NULL;

  register unsigned int
    i,
    j;

  MagickPassFail
    status=MagickPass;

  assert(image != (Image *) NULL);
  assert(colormap != (const PixelPacket *) NULL);
  assert(colors != 0);
  assert(image->storage_class == PseudoClass);

  /*
    Allocate memory for colormap index
  */
  colormap_index=MagickAllocateMemory(unsigned int *,
                                      MaxColormapSize*sizeof(unsigned int));
  if (colormap_index == (unsigned int *) NULL)
    {
      ThrowException3(&image->exception,ResourceLimitError,
                      MemoryAllocationFailed,UnableToAllocateColormap);
      return MagickFail;
    }

  /*
    Build a map between the new colormap and the old colormap.
  */
  (void) memset(colormap_index,0,MaxColormapSize*sizeof(unsigned int));
  for (i=0; i < image->colors ; i++)
    {
      for (j=0; j < colors; j++)
        {
          if (ColorMatch(&colormap[j],&image->colormap[i]))
            {
              colormap_index[i]=j;
              break;
            }
        }
    }

  if (status == MagickPass)
    {
      /*
        Reassign image colormap indexes
      */
      status=PixelIterateMonoModify(ReplaceImageColormapCallBack,NULL,
                                    ReplaceImageColormapText,
                                    NULL,colormap_index,0,0,
                                    image->columns,image->rows,
                                    image,&image->exception);
      /*
        Replace existing colormap.
      */
      if (status == MagickPass)
        {
          MagickReallocMemory(PixelPacket *,image->colormap,sizeof(PixelPacket)*colors);
          if (image->colormap == (PixelPacket *) NULL)
            {
              ThrowException3(&image->exception,ResourceLimitError,
                              MemoryAllocationFailed,UnableToAllocateColormap);
              status=MagickFail;
            }
        }
      if (status == MagickPass)
        (void) memcpy(image->colormap,colormap,sizeof(PixelPacket)*colors);
    }

  MagickFreeMemory(colormap_index);

  image->is_grayscale=IsGrayImage(image,&image->exception);
  image->is_monochrome=IsMonochromeImage(image,&image->exception);
  
  return status;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%   S e t I m a g e                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetImage() sets the red, green, and blue components of each pixel to
%  the image background color and the opacity component to the specified
%  level of transparency.  The background color is defined by the
%  background_color member of the image.
%
%  The format of the SetImage method is:
%
%      void SetImage(Image *image,const Quantum opacity)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o opacity: Set each pixel to this level of transparency.
%
%
*/
#define SetImageColorText "[%s] Set image color..."
static MagickPassFail
SetImageColorCallBack(void *mutable_data,         /* User provided mutable data */
                      const void *immutable_data, /* User provided immutable data */
                      Image *image,               /* Modify image */
                      PixelPacket *pixels,        /* Pixel row */
                      IndexPacket *indexes,       /* Pixel row indexes */
                      const long npixels,         /* Number of pixels in row */
                      ExceptionInfo *exception)   /* Exception report */
{
  /*
    Assign color to pixels.
  */
  const PixelPacket
    background_color = *(const PixelPacket *) immutable_data;
  
  register long
    i;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  /*
    Set DirectClass pixels
  */
  for (i=0; i < npixels; i++)
    pixels[i]=background_color;

  if ((image->storage_class == PseudoClass) ||
      (image->colorspace == CMYKColorspace))
    {
      /*
        Set PseudoClass pixel indexes.
      */
      (void) memset(indexes,0,npixels*sizeof(IndexPacket));
    }

  return MagickPass;
}
MagickExport MagickPassFail SetImage(Image *image,const Quantum opacity)
{
  PixelPacket
    background_color;

  MagickPassFail
    status;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=MagickPass;
  background_color=image->background_color;
  if (opacity != OpaqueOpacity)
    background_color.opacity=opacity;
  if (background_color.opacity != OpaqueOpacity)
    {
      image->matte=True;
      image->colorspace=RGBColorspace;
      image->storage_class=DirectClass;
    }

  status=PixelIterateMonoModify(SetImageColorCallBack,NULL,
                                SetImageColorText,
                                NULL,&background_color,0,0,
                                image->columns,image->rows,
                                image,&image->exception);

  image->is_grayscale=IsGray(image->background_color);
  image->is_monochrome=IsMonochrome(image->background_color);
  return status;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t I m a g e C l i p M a s k                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetImageClipMask() associates a clip mask with the image.  The clip mask
%  must be the same dimensions as the image.
%
%  If a component of the clip mask is set to TransparentOpacity (maximum
%  value) then the corresponding image pixel component will not be updated
%  when SyncImagePixels() is applied. The clip mask may be used to constrain
%  the results of an image processing operation to a region of the image.
%  Regions outside those allowed by the clip mask may be processed, but only
%  pixel quantums allowed by the clip mask will actually be updated.
%
%  The clip mask protects the DirectClass pixels and PseudoClass pixel indexes
%  from modification. The clip mask does *not* protect the image colormap since
%  the image colormap is globally shared by all pixels in a PseudoClass image.
%
%  The format of the SetImageClipMask method is:
%
%      unsigned int SetImageClipMask(Image *image,const Image *clip_mask)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o clip_mask: The image clip mask.
%
%
*/
MagickExport MagickPassFail SetImageClipMask(Image *image,const Image *clip_mask)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (clip_mask != (const Image *) NULL)
    if ((clip_mask->columns != image->columns) ||
        (clip_mask->rows != image->rows))
      ThrowBinaryException3(ImageError,UnableToSetClipMask,ImageSizeDiffers);
  if (image->clip_mask != (Image *) NULL)
    DestroyImage(image->clip_mask);
  image->clip_mask=(Image *) NULL;
  if (clip_mask == (Image *) NULL)
    return(MagickPass);
  image->clip_mask=CloneImage(clip_mask,0,0,True,&image->exception);
  if (image->clip_mask)
    return (MagickPass);
  return (MagickFail);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t I m a g e D e p t h                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetImageDepth() translates the pixel quantums across all of the channels
%  so that if they are later divided to fit within the specified bit
%  depth, that no additional information is lost (i.e. no remainder will
%  result from the division).  Note that any subsequent image processing is
%  likely to increase the effective depth of the image channels. A non-zero
%  value is returned if the operation is successful. Check the exception
%  member of image to determine the cause for any failure.
%
%  The format of the SetImageDepth method is:
%
%      unsigned int SetImageDepth(Image *image,const unsigned long depth)
%
%  A description of each parameter follows:
%
%    o image: The image to update.
%
%    o depth: Desired image depth (range 1 to QuantumDepth)
%
%
*/
MagickExport MagickPassFail SetImageDepth(Image *image,const unsigned long depth)
{
  MagickPassFail
    status;

  status=QuantumOperatorImage(image,AllChannels,DepthQuantumOp,(double) depth,
                              &image->exception);
  if ((image->matte) && (MagickFail != status))
    status=QuantumOperatorImage(image,OpacityChannel,DepthQuantumOp,(double) depth,
                                &image->exception);
  image->depth=Min(depth,QuantumDepth);
  return status;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   S e t I m a g e I n f o                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetImageInfo() initializes the `magick' field of the ImageInfo structure.
%  It is set to a type of image format based on the prefix or suffix of the
%  filename.  For example, `ps:image' returns PS indicating a Postscript image.
%  JPEG is returned for this filename: `image.jpg'.  The filename prefix has
%  precendence over the suffix.  Use an optional index enclosed in brackets
%  after a file name to specify a desired subimage of a multi-resolution image
%  format like Photo CD (e.g. img0001.pcd[4]).  A True (non-zero) return value
%  indicates success.
%
%  The format of the SetImageInfo method is:
%
%      unsigned int SetImageInfo(ImageInfo *image_info,
%        const unsigned int rectify,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info..
%
%    o rectify: an unsigned value other than zero rectifies the attribute for
%      multi-frame support (user may want multi-frame but image format may not
%      support it).
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

static inline unsigned int IsFrame(const char *point)
{
  char
    *p;

  (void) strtol(point,&p,10);
  return(p != point);
}

MagickExport MagickPassFail SetImageInfo(ImageInfo *image_info,
  const MagickBool rectify,ExceptionInfo *exception)
{
  static const char
    *virtual_delegates[] =
    {
      "AUTOTRACE",
      "BROWSE",
      "EDIT",
      "GS-COLOR",
      "GS-COLOR+ALPHA",
      "GS-GRAY",
      "GS-MONO",
      "LAUNCH",
      "MPEG-ENCODE",
      "PRINT",
      "SCAN",
      "SHOW",
      "TMP",
      "WIN",
      NULL
    };

  char
    filename[MaxTextExtent],
    magic[MaxTextExtent],
    *q;

  Image
    *image;

  register char
    *p;

  const MagickInfo
    *magick_info;

  size_t
    magick_length;

  unsigned char
    magick[2*MaxTextExtent];

  unsigned int
    status=MagickPass;

  /*
    Look for 'image.format' in filename.
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  *magic='\0';
  p=image_info->filename+Max((long) strlen(image_info->filename)-1,0);
  if (*p == ']' && !IsAccessibleNoLogging(image_info->filename))
    {
      /*
        Look for sub-image specification (e.g. img0001.pcd[4]).
      */
      for (q=p; (q > image_info->filename) && (*q != '['); q--)
        ;

      if ((q > image_info->filename) && (*q == '[') && IsFrame(q+1))
        {
          unsigned long
            first,
            last;

          (void) CloneString(&image_info->tile,q+1);
          /* Copy image range spec. to tile spec. w/o brackets */
          image_info->tile[p-q-1]='\0';
          /* Cut image range spec. from filename */
          *q='\0';
          image_info->subimage=atol(image_info->tile);
          image_info->subrange=image_info->subimage;

          /* Parse the image range spec. now placed in tile */
          for (q=image_info->tile; *q != '\0'; )
          {
            while (isspace((int)(unsigned char) *q) || (*q == ','))
              q++;
            first=strtol(q,&q,10);
            last=first;
            while (isspace((int)(unsigned char) *q))
              q++;
            if (*q == '-')
              last=strtol(q+1,&q,10);
            if (first > last)
              Swap(first,last);
            if (first < image_info->subimage)
              image_info->subimage=first;
            if (last > image_info->subrange)
              image_info->subrange=last;
          }
          image_info->subrange-=image_info->subimage-1;
        }

      /* Restore p to end of modified filename */
      p=image_info->filename+Max((long) strlen(image_info->filename)-1,0);
    }

  /*
    Look for explicit 'format:image' in filename.
  */
  image_info->affirm=False;
  p=image_info->filename;
  while (isalnum((int) *p))
    p++;
  if ((*p == ':') && ((p-image_info->filename) < (long) sizeof(magic)))
    {
      char
        format[MaxTextExtent];

      /*
        User specified image format.
      */
      (void) strncpy(format,image_info->filename,p-image_info->filename);
      format[p-image_info->filename]='\0';

      /*
        Backward compatability and interoperability namimg
      */
      if (LocaleCompare(format,"GRADATION") == 0)
        (void) strcpy(format,"GRADIENT");

      if (LocaleCompare(format,"MAGICK") == 0)
        (void) strcpy(format,"IMAGE");

      LocaleUpper(format);
      if (!IsMagickConflict(format))
        {
          /*
            Strip off image format prefix.
          */
          char base_filename[MaxTextExtent];
          p++;
          (void) strlcpy(base_filename,p,MaxTextExtent);
          (void) strcpy(image_info->filename,base_filename);
          (void) strlcpy(magic,format,MaxTextExtent);
          (void) strlcpy(image_info->magick,magic,MaxTextExtent);
          if (LocaleCompare(magic,"TMP") != 0)
            image_info->affirm=MagickTrue;
          else
            /* input file will be automatically removed */
            image_info->temporary=MagickTrue;
        }
    }

  /*
    If we have not set the magic yet then set magic based on file
    extension.  Some extensions are intentionally not recognized
    because they are confusing or dangerous.
  */
  if (*magic == '\0')
    {
      /* Restore p to end of modified filename */
      p=image_info->filename+Max((long) strlen(image_info->filename)-1,0);

      while ((*p != '.') && (p > (image_info->filename+1)))
        p--;
      if ((LocaleCompare(p,".gz") == 0) || (LocaleCompare(p,".Z") == 0) ||
          (LocaleCompare(p,".bz2") == 0))
        do
          {
            p--;
          } while ((*p != '.') && (p > (image_info->filename+1)));
      if ((*p == '.') && (strlen(p) < (long) sizeof(magic)))
        {
          /*
            User specified image format.
          */
          unsigned int
            i;
          
          MagickBool
            exclude;
          
          (void) strlcpy(magic,p+1,MaxTextExtent);
          for (q=magic; *q != '\0'; q++)
            if (*q == '.')
              {
                *q='\0';
                break;
              }
          LocaleUpper(magic);

          exclude=MagickFalse;

          /*
            SGI and RGB are ambiguous.
          */
          if ((LocaleNCompare(image_info->magick,"SGI",3) == 0) &&
              (LocaleCompare(magic,"RGB") == 0))
            exclude=MagickTrue;

          /*
            Ignore extensions which match virtual delegates.
          */
          i=0;
          while ((!exclude) && (virtual_delegates[i] != NULL))
            {
              if ((magic[0] == (virtual_delegates[i])[0]) &&
                  (LocaleCompare(magic,virtual_delegates[i]) == 0))
                {
                  exclude=MagickTrue;
                }
              i++;
            }

          /*
            Check to see if there is a coder definition for this
            format and if the extension should be ignored, or is the
            sole authority for this format.
          */
          magick_info=GetMagickInfo(magic,exception);
          if (magick_info != (const MagickInfo *) NULL)
            {
              if (magick_info->extension_treatment == IgnoreExtensionTreatment)
                exclude=MagickTrue;
              else if (magick_info->extension_treatment == ObeyExtensionTreatment)
                image_info->affirm=MagickTrue;
            }

          if ((!exclude) || (image_info->affirm))
            (void) strlcpy(image_info->magick,magic,MaxTextExtent);
        }
    }

  if (rectify)
    {
      /*
        Test for multiple image support in file filename template. In
        this case, the filename contains a printf style string
        containing some variation of %d (e.g. "image%02d.miff");
      */

      if (MagickSceneFileName(filename,image_info->filename,".%lu",MagickFalse,0))
        image_info->adjoin=False;

      magick_info=GetMagickInfo(magic,exception);
      if (magick_info != (const MagickInfo *) NULL)
        image_info->adjoin&=magick_info->adjoin;
      return(MagickTrue);
    }
  if (image_info->affirm)
    return(MagickTrue);
  /*
    Determine the image format from the first few bytes of the file.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return(False);
  (void) strlcpy(image->filename,image_info->filename,MaxTextExtent);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFail)
    {
      DestroyImage(image);
      return(MagickFail);
    }
  if (!BlobIsSeekable(image))
    {
      /*
        Copy standard input or pipe to temporary file.
      */
      if(!AcquireTemporaryFileName(filename))
        {
          CloseBlob(image);
          DestroyImage(image);
          return(False);
        }
      (void) ImageToFile(image,filename,exception);
      CloseBlob(image);
      (void) strcpy(image->filename,filename);
      status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
      if (status == MagickFail)
        {
          DestroyImage(image);
          return(MagickFail);
        }
      (void) strcpy(image_info->filename,filename);
      image_info->temporary=MagickTrue;
    }
  magick[0]='\0';
  magick_length = ReadBlob(image,2*MaxTextExtent,magick);
  CloseBlob(image);
  DestroyImage(image);
  /*
    Check format using magic.mgk configuration file.  Use of an
    external config file is absolutely necessary when using loadable
    modules since otherwise the code necessary to perform the test
    might not be available yet.
  */
  if (GetMagickFileFormat(magick,magick_length,image_info->magick,
                          MaxTextExtent,exception))
    return(MagickPass);

  return(MagickFail);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     S e t I m a g e O p a c i t y                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetImageOpacity() attenuates the opacity channel of an image.  If the
%  image pixels are opaque, they are set to the specified opacity level.
%  Otherwise, the pixel opacity values are blended with the supplied
%  transparency value.
%
%  The format of the SetImageOpacity method is:
%
%      void SetImageOpacity(Image *image,const unsigned int opacity)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o opacity: The level of transparency: 0 is fully opaque and MaxRGB is
%      fully transparent.
%
%
*/
static MagickPassFail
ModulateImageOpacityCallBack(void *mutable_data,         /* User provided mutable data */
                             const void *immutable_data, /* User provided immutable data */
                             Image *image,               /* Modify image */
                             PixelPacket *pixels,        /* Pixel row */
                             IndexPacket *indexes,       /* Pixel row indexes */
                             const long npixels,         /* Number of pixels in row */
                             ExceptionInfo *exception)   /* Exception report */
{
  const magick_uint32_t
    opacity = *((const unsigned int *) immutable_data);

  register long
    i;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);

if (image->colorspace == CMYKColorspace)
  {
    for (i=0; i < npixels; i++)
      indexes[i]=(IndexPacket) BlendQuantumOpacity(indexes[i],opacity);
  }
 else
   {
     for (i=0; i < npixels; i++)
       pixels[i].opacity=(Quantum) BlendQuantumOpacity(pixels[i].opacity,opacity);
   }
  return MagickPass;
}
static MagickPassFail
SetImageOpacityCallBack(void *mutable_data,         /* User provided mutable data */
                        const void *immutable_data, /* User provided immutable data */
                        Image *image,               /* Modify image */
                        PixelPacket *pixels,        /* Pixel row */
                        IndexPacket *indexes,       /* Pixel row indexes */
                        const long npixels,         /* Number of pixels in row */
                        ExceptionInfo *exception)   /* Exception report */
{
  const unsigned int
    opacity = *((const unsigned int *) immutable_data);

  register long
    i;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);

if (image->colorspace == CMYKColorspace)
  {
    for (i=0; i < npixels; i++)
      indexes[i]=opacity;
  }
 else
   {
     for (i=0; i < npixels; i++)
       pixels[i].opacity=opacity;
   }
  return MagickPass;
}
MagickExport void SetImageOpacity(Image *image,const unsigned int opacity_val)
{
  const unsigned int
    opacity = opacity_val;

  MagickBool
    is_grayscale,
    is_monochrome;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  is_grayscale=image->is_grayscale;
  is_monochrome=image->is_monochrome;
  image->storage_class=DirectClass;
  if (image->matte && (opacity != OpaqueOpacity) &&
      (opacity != TransparentOpacity))
    {
      /*
        Attenuate existing opacity channel
      */
      (void) PixelIterateMonoModify(ModulateImageOpacityCallBack,NULL,
                                    "[%s] Modulate image opacity...",
                                    NULL,&opacity,0,0,image->columns,image->rows,
                                    image,&image->exception);
    }
  else
    {
      /*
        Add new opacity channel or make existing opacity channel opaque
      */
      image->matte=True;
      (void) PixelIterateMonoModify(SetImageOpacityCallBack,NULL,
                                    "[%s] Set image opacity...",
                                    NULL,&opacity,0,0,image->columns,image->rows,
                                    image,&image->exception);
    }
  image->is_grayscale=is_grayscale;
  image->is_monochrome=is_monochrome;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t I m a g e T y p e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  (void) SetImageType() sets the type of image.  Choose from these types:
%
%        Bilevel        Grayscale       GrayscaleMatte
%        Palette        PaletteMatte    TrueColor
%        TrueColorMatte ColorSeparation ColorSeparationMatte
%        OptimizeType
%
%  The format of the (void) SetImageType method is:
%
%      (void) SetImageType(Image *image,const ImageType image_type)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o image_type: Image type.
%
%
*/
MagickExport MagickPassFail SetImageType(Image *image,const ImageType image_type)
{
  QuantizeInfo
    quantize_info;

  unsigned int
    status = MagickPass;

  MagickBool
    logging;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  logging=IsEventLogging();
  switch (image_type)
  {
    case BilevelType:
    {
      MagickBool
        is_monochrome;
      
      if (!IsRGBColorspace(image->colorspace))
        {
          if (logging)
            (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                                  "SetImageType(Bilevel) Transforming to RGB colorspace ...");
          (void) TransformColorspace(image,RGBColorspace);
        }

       is_monochrome=image->is_monochrome;
       if (!is_monochrome && image->dither)
         is_monochrome=IsMonochromeImage(image,&image->exception);
      if (is_monochrome || image->is_grayscale)
        image->colorspace=GRAYColorspace;

      if (!image->dither || is_monochrome)
        {
          if (!is_monochrome || (image->storage_class != PseudoClass))
            {
              /*
                Threshold image to bilevel
              */
              if (logging)
                (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                                      "SetImageType(Bilevel) Smashing to bilevel using Threshold method ...");
              (void) ThresholdImage(image,(double)MaxRGB/2);
              (void) AllocateImageColormap(image,2);
            }
        }
      else
        {
          /*
            Dither image to bilevel (very slow!)
          */
          GetQuantizeInfo(&quantize_info);
          quantize_info.colorspace=GRAYColorspace;
          quantize_info.dither=image->dither;
          quantize_info.tree_depth=8;
          quantize_info.number_colors=2;
          if (logging)
            (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                                  "SetImageType(Bilevel) Dithering to bilevel using Quantize method ...");
          (void) QuantizeImage(&quantize_info,image);
        }
      image->colorspace=GRAYColorspace;
      image->is_grayscale=True;
      image->is_monochrome=True;
      break;
    }
    case GrayscaleType:
    {
      if (!IsRGBColorspace(image->colorspace))
        {
          if (logging)
            (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                                  "SetImageType(Grayscale) Transforming to RGB colorspace ...");
          (void) TransformColorspace(image,RGBColorspace);
        }
      if (!image->is_grayscale)
        {
          if (logging)
            (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                                  "SetImageType(Grayscale) Transforming to gray colorspace ...");
          (void) TransformColorspace(image,GRAYColorspace);
        }
      image->colorspace=GRAYColorspace;
      image->is_grayscale=True;
      break;
    }
    case GrayscaleMatteType:
    {
      if (!IsRGBColorspace(image->colorspace))
        {
          if (logging)
            (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                                  "SetImageType(GrayscaleMatte) Transforming to RGB colorspace ...");
          (void) TransformColorspace(image,RGBColorspace);
        }
      if (!image->is_grayscale)
        {
          if (logging)
            (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                                  "SetImageType(GrayscaleMatte) Transforming to gray colorspace ...");
          (void) TransformColorspace(image,GRAYColorspace);
        }
      if (!image->matte)
        {
          if (logging)
            (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                                  "SetImageType(GrayscaleMatte) Adding opaque matte channel ...");
          SetImageOpacity(image,OpaqueOpacity);
        }
      image->colorspace=GRAYColorspace;
      image->is_grayscale=True;
      break;
    }
    case PaletteType:
    {
      if (!IsRGBColorspace(image->colorspace))
        {
          if (logging)
            (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                                  "SetImageType(Palette) Transforming to RGB colorspace ...");
          (void) TransformColorspace(image,RGBColorspace);
        }
      if (image->storage_class != PseudoClass)
        {
          if (logging)
            (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                                  "SetImageType(Palette) Using Quantize method ...");
          GetQuantizeInfo(&quantize_info);
          quantize_info.dither=image->dither;
          (void) QuantizeImage(&quantize_info,image);
        }
      break;
    }
    case PaletteMatteType:
    {
      if (!IsRGBColorspace(image->colorspace))
        {
          if (logging)
            (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                                  "SetImageType(PaletteMatte) Transforming to RGB colorspace ...");
          (void) TransformColorspace(image,RGBColorspace);
        }
      if (!image->matte)
        {
          if (logging)
            (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                                  "SetImageType(PaletteMatte) Adding opaque matte channel ...");
          SetImageOpacity(image,OpaqueOpacity);
        }
      if (image->storage_class != PseudoClass)
        {
          if (logging)
            (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                                  "SetImageType(PaletteMatteType) Using Quantize method ...");
          GetQuantizeInfo(&quantize_info);
          quantize_info.colorspace=TransparentColorspace;
          quantize_info.dither=image->dither;
          (void) QuantizeImage(&quantize_info,image);
        }
      break;
    }
    case TrueColorType:
    {
      if (!IsRGBColorspace(image->colorspace))
        {
          if (logging)
            (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                                  "SetImageType(TrueColor) Transforming to RGB colorspace ...");
          (void) TransformColorspace(image,RGBColorspace);
        }
      image->storage_class=DirectClass;
      break;
    }
    case TrueColorMatteType:
    {

      if (!IsRGBColorspace(image->colorspace))
        {
          if (logging)
            (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                                  "SetImageType(TrueColorMatte) Transforming to RGB colorspace ...");
          (void) TransformColorspace(image,RGBColorspace);
        }
      image->storage_class=DirectClass;
      if (!image->matte)
        {
          if (logging)
            (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                                  "SetImageType(TrueColorMatte) Adding opaque matte channel ...");
          SetImageOpacity(image,OpaqueOpacity);
        }
      break;
    }
    case ColorSeparationType:
    {
      if (image->colorspace != CMYKColorspace)
        {
          if (logging)
            (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                                  "SetImageType(ColorSeparation) Transforming to CMYK colorspace ...");
          (void) TransformColorspace(image,CMYKColorspace);
        }
      break;
    }
    case ColorSeparationMatteType:
    {
      if (image->colorspace != CMYKColorspace)
        {
          if (logging)
            (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                                  "SetImageType(ColorSeparationMatte) Transforming to CMYK colorspace ...");
          (void) TransformColorspace(image,CMYKColorspace);
        }
      if (!image->matte)
        {
          if (logging)
            (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                                  "SetImageType(ColorSeparationMatte) Adding opaque matte channel ...");
          SetImageOpacity(image,OpaqueOpacity);
        }
      break;
    }
    case OptimizeType:
    default:
      break;
  }
  return (status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   S o r t C o l o r m a p B y I n t e n t s i t y                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SortColormapByIntensity() sorts the colormap of a PseudoClass image by
%  decreasing color intensity.
%
%  The format of the SortColormapByIntensity method is:
%
%      unsigned int SortColormapByIntensity(Image *image)
%
%  A description of each parameter follows:
%
%    o image: A pointer to an Image structure.
%
%
*/

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

static int InverseIntensityCompare(const void *x,const void *y)
{
  long
    intensity;

  PixelPacket
    *color_1,
    *color_2;

  color_1=(PixelPacket *) x;
  color_2=(PixelPacket *) y;
  /*
    y - x results in decreasing order
  */
  intensity=PixelIntensityToQuantum(color_2)-
    (long) PixelIntensityToQuantum(color_1);
  return(intensity);
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#define SortColormapByIntensityText "[%s] Sorting colormap by intensity...  "
static MagickPassFail
SortColormapByIntensityCallBack(void *mutable_data,         /* User provided mutable data */
                                const void *immutable_data, /* User provided immutable data */
                                Image *image,               /* Modify image */
                                PixelPacket *pixels,        /* Pixel row */
                                IndexPacket *indexes,       /* Pixel row indexes */
                                const long npixels,         /* Number of pixels in row */
                                ExceptionInfo *exception)   /* Exception report */
{
  /*
    Update image colormap indexes to reflect new ordering indicated by
    new_indexes array.
  */
  const unsigned short
    *new_indexes = (const unsigned short *) immutable_data;

  register long
    i;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(pixels);
  ARG_NOT_USED(exception);
  for (i=0; i < npixels; i++)
    indexes[i]=new_indexes[indexes[i]];

  return MagickPass;
}
MagickExport MagickPassFail SortColormapByIntensity(Image *image)
{
  register long
    i;

  unsigned int
    is_grayscale,
    is_monochrome;

  unsigned short
    *new_indexes;

  MagickPassFail
    status=MagickPass;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->storage_class != PseudoClass)
    return(MagickFail);
  is_grayscale=image->is_grayscale;
  is_monochrome=image->is_monochrome;
  /*
    Allocate memory for pixel indexes.
  */
  new_indexes=MagickAllocateMemory(unsigned short *,image->colors*sizeof(unsigned short));
  if (new_indexes == (unsigned short *) NULL)
    ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
                          UnableToSortImageColormap);
  /*
    Assign index values to colormap entries.
  */
  for (i=0; i < (long) image->colors; i++)
    image->colormap[i].opacity=(unsigned short) i;
  /*
    Sort image colormap by decreasing intensity.
  */
  qsort((void *) image->colormap,image->colors,sizeof(PixelPacket),
        InverseIntensityCompare);
  /*
    Update image colormap indexes to new order.
  */
  for (i=0; i < (long) image->colors; i++)
    new_indexes[image->colormap[i].opacity]=(unsigned short) i;
  status=PixelIterateMonoModify(SortColormapByIntensityCallBack,NULL,
                                SortColormapByIntensityText,
                                NULL,new_indexes,0,0,image->columns,image->rows,
                                image,&image->exception);
  MagickFreeMemory(new_indexes);
  image->is_grayscale=is_grayscale;
  image->is_monochrome=is_monochrome;
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   S y n c I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SyncImage initializes the red, green, and blue intensities of each
%  pixel as defined by the colormap index.
%
%  The format of the SyncImage method is:
%
%      MagickPassFail SyncImage(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
#define SyncImageText "[%s] Synchronizing DirectClass pixels..."
static MagickPassFail
SyncImageCallBack(void *mutable_data,         /* User provided mutable data */
                  const void *immutable_data, /* User provided immutable data */
                  Image *image,               /* Modify image */
                  PixelPacket *pixels,        /* Pixel row */
                  IndexPacket *indexes,       /* Pixel row indexes */
                  const long npixels,         /* Number of pixels in row */
                  ExceptionInfo *exception)   /* Exception report */
{
  /*
    Synchronize DirectClass pixels with Indexes and colormap.
  */
  register long
    i;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(immutable_data);
  ARG_NOT_USED(exception);

  if (image->matte)
    {
      register const PixelPacket
        *p;

      for (i=0; i < npixels; i++)
        {
          VerifyColormapIndex(image,indexes[i]);
          /*
            Explicit member assignment is used in order to support a colormap
            simultaneous with with pixel opacity.
          */
          p=&image->colormap[indexes[i]];
          pixels[i].red=p->red;
          pixels[i].green=p->green;
          pixels[i].blue=p->blue;
        }
    }
  else
    {
      for (i=0; i < npixels; i++)
        {
          VerifyColormapIndex(image,indexes[i]);
          /*
            Use structure assignment for improved performance.
            Trash whatever is in the opacity channel.
          */
          pixels[i]=image->colormap[indexes[i]];
        }
    }

  return MagickPass;
}
MagickExport MagickPassFail SyncImage(Image *image)
{
  MagickBool
    is_grayscale,
    is_monochrome;

  MagickPassFail
    status=MagickPass;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->storage_class == DirectClass)
    return (status);
  assert(image->colormap != (PixelPacket *) NULL);
  is_grayscale=image->is_grayscale;
  is_monochrome=image->is_monochrome;
  status=PixelIterateMonoModify(SyncImageCallBack,NULL,
                                SyncImageText,
                                NULL,NULL,0,0,image->columns,image->rows,
                                image,&image->exception);
  image->is_grayscale=is_grayscale;
  image->is_monochrome=is_monochrome;
  return (status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     T e x t u r e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  TextureImage() repeatedly tiles the texture image across and down the image
%  canvas. If the image canvas includes a matte channel, then the tile is
%  alpha-composited "under" the image. MagickFail is returned if an error is
%  encountered.
%
%  The format of the TextureImage method is:
%
%      unsigned int TextureImage(Image *image,const Image *texture)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o texture: This image is the texture to layer on the background.
%
%
*/

#define TextureImageText  "[%s] Apply image texture..."
MagickExport MagickPassFail TextureImage(Image *image,const Image *texture)
{
  MagickPassFail
    status=MagickPass;

  long
    y;

  unsigned long
    row_count=0;

  MagickBool
    get_pixels,
    is_grayscale;

  /*
    Tile texture onto the image background.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (texture == (const Image *) NULL)
    return MagickFail;

  get_pixels=GetPixelCachePresent(image);
  is_grayscale=image->is_grayscale;
  image->storage_class=DirectClass;
#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  pragma omp parallel for schedule(dynamic,4) shared(row_count, status)
#endif
  for (y=0; y < (long) image->rows; y++)
    {
      const PixelPacket
        *pixels;

      long
        x;

      register long
        z;

      register const PixelPacket
        *p;

      register PixelPacket
        *q;

      unsigned long
        width;

      MagickBool
        thread_status;

      thread_status=status;
      if (thread_status == MagickFail)
        continue;

      p=AcquireImagePixels(texture,0,y % texture->rows,
                           texture->columns,1,&image->exception);
      if (get_pixels)
        q=GetImagePixelsEx(image,0,y,image->columns,1,&image->exception);
      else
        q=SetImagePixelsEx(image,0,y,image->columns,1,&image->exception);

      if ((p == (const PixelPacket *) NULL) ||
          (q == (PixelPacket *) NULL))
        thread_status=MagickFail;

      if (thread_status != MagickFail)
        {
          pixels=p;
          for (x=0; x < (long) image->columns; x+=texture->columns)
            {
              width=texture->columns;
              if ((x+width) > image->columns)
                width=image->columns-x;
              p=pixels;
              if (image->matte)
                {
                  for (z=(long) width; z != 0; z--)
                    {
                      AlphaCompositePixel(q,q,q->opacity,p,texture->matte != MagickFalse ?
                                          p->opacity : OpaqueOpacity);
                      p++;
                      q++;
                    }
                }
              else
                {
                  if (width*sizeof(PixelPacket) < 1024)
                    {
                      for (z=(long) width; z != 0; z--)
                        {
                          *q=(*p);
                          p++;
                          q++;
                        }
                    }
                  else
                    {
                      (void) memcpy(q,p,width*sizeof(PixelPacket));
                      q += width;
                    }
                }
            }
          if (!SyncImagePixelsEx(image,&image->exception))
            thread_status=MagickFail;
        }
#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  pragma omp critical
#endif
      {
        row_count++;
        if (QuantumTick(row_count,image->rows))
          if (!MagickMonitorFormatted(row_count,image->rows,&image->exception,
                                      TextureImageText,image->filename))
            thread_status=MagickFail;

        if (thread_status == MagickFail)
          status=MagickFail;
      }
    }

  if (image->matte)
    image->is_grayscale=(is_grayscale && texture->is_grayscale);
  else
    image->is_grayscale=texture->is_grayscale;
  return (status);
}
