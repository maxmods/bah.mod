/*
% Copyright (C) 2003 - 2012 GraphicsMagick Group
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
#include "magick/analyze.h"
#include "magick/attribute.h"
#include "magick/blob.h"
#include "magick/channel.h"
#include "magick/color.h"
#include "magick/color_lookup.h"
#include "magick/colormap.h"
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
#include "magick/random.h"
#include "magick/profile.h"
#include "magick/semaphore.h"
#include "magick/signature.h"
#include "magick/statistics.h"
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
  *MatteColor = "#bdbdbdbdbdbd",  /* gray */
  *PSDensityGeometry = "72.0x72.0",
  *PSPageGeometry = "612x792>";

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
%   A d d D e f i n i t i o n                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AddDefinition() adds a key/value definition to the current map of
%  definitions in ImageInfo. Definitions may be used by coders/decoders
%  that read and write images.
%
%  The format of the AddDefinition method is:
%
%      MagickPassFail AddDefinition(ImageInfo *image_info,const char *magick,
%                                   const char *key, const char *value,
%                                   ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o magick: format/classification identifier
%
%    o key: subidentifier within format/classification
%
%    o value: definition value
%
%    o exception: Errors result in updates to this structure.
%
*/
MagickExport MagickPassFail
AddDefinition(ImageInfo *image_info,const char *magick, const char *key,
	      const char *value, ExceptionInfo *exception)
{
  MagickPassFail
    status = MagickFail;

  char
    search_key[MaxTextExtent];

  if (image_info->definitions == 0)
    image_info->definitions=MagickMapAllocateMap(MagickMapCopyString,
						 MagickMapDeallocateString);
  if (image_info->definitions != 0)
    {
      /*
	Format string like "magick:key"
      */
      FormatString(search_key, "%.60s:%.1024s", magick, key);
      
      /*
	Add entry to map
      */
      status = MagickMapAddEntry((MagickMap) image_info->definitions,search_key,value,0,exception);
    }

  return status;
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
%      MagickPassFail AddDefinitions(ImageInfo *image_info,const char *options)
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

  MagickPassFail
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
  if (image_info->definitions == 0)
    return MagickFail;

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
      (void) GetGeometry(image_info->size,
			 &allocate_image->tile_info.x,
			 &allocate_image->tile_info.y,
			 &allocate_image->columns,
			 &allocate_image->rows);
      allocate_image->offset=allocate_image->tile_info.x;
      allocate_image->tile_info.width=allocate_image->columns;
      allocate_image->tile_info.height=allocate_image->rows;
    }
  if (image_info->tile != (char *) NULL)
    if (!IsSubimage(image_info->tile,False))
      {
        (void) GetGeometry(image_info->tile,
			   &allocate_image->tile_info.x,
			   &allocate_image->tile_info.y,
			   &allocate_image->tile_info.width,
			   &allocate_image->tile_info.height);
	if (0 == allocate_image->columns)
	  allocate_image->columns=allocate_image->tile_info.width;
	if (0 == allocate_image->rows)
	  allocate_image->rows=allocate_image->tile_info.height;
      }
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
  const char
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
  client_name=GetClientName();
  resource_database=MagickXGetResourceDatabase(display,client_name);
  MagickXGetResourceInfo(resource_database,client_name,&resource);
  resource.image_info=CloneImageInfo(image_info);
  resource.immutable=True;
  (void) MagickXAnimateImages(display,&resource,(char **) &client_name,
			      1,image);
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
%                                                                             %
%   A p p e n d I m a g e s                                                   %
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
#define AppendImageText "[%s] Append sequence..."

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
    return CloneImage(image,0,0,MagickTrue,exception);

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
	if (append_image->columns > next->columns)
	  SetImageColorRegion(append_image,next->columns,y,
			      append_image->columns-next->columns,next->rows,
			      &append_image->background_color);
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
    if (append_image->rows > next->rows)
      SetImageColorRegion(append_image,x,next->rows,
			  next->columns,
			  append_image->rows-next->rows,
			  &append_image->background_color);
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

#define ClipPathImageText "[%s] Creating clip mask..."
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
  LockSemaphoreInfo((SemaphoreInfo *) image->semaphore);
  image->reference_count--;
  if (image->reference_count == 0)
    destroy=True;
  UnlockSemaphoreInfo((SemaphoreInfo *) image->semaphore);
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
  image->clip_mask=(Image *) NULL;
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
  const char
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
  client_name=GetClientName();
  resource_database=MagickXGetResourceDatabase(display,client_name);
  MagickXGetResourceInfo(resource_database,client_name,&resource_info);
  if (image_info->page != (char *) NULL)
    resource_info.image_geometry=AcquireString(image_info->page);
  resource_info.immutable=True;
  for (next=image; next; next=next->next)
  {
    state=DefaultState;
    (void) MagickXDisplayImage(display,&resource_info,(char **) &client_name,
			       1,&next,&state);
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
  image_info->adjoin=MagickTrue;
  image_info->depth=QuantumDepth;
  image_info->interlace=UndefinedInterlace;
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
+   I s S u b i m a g e                                                       %
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
%                                                                             %
%   I s T a i n t I m a g e                                                   %
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
  LockSemaphoreInfo((SemaphoreInfo *) (*image)->semaphore);
  if ((*image)->reference_count > 1)
    clone=True;
  UnlockSemaphoreInfo((SemaphoreInfo *) (*image)->semaphore);
  if (!clone)
    return;
  clone_image=CloneImage(*image,0,0,True,exception);
  LockSemaphoreInfo((SemaphoreInfo *) (*image)->semaphore);
  (*image)->reference_count--;
  UnlockSemaphoreInfo((SemaphoreInfo *) (*image)->semaphore);
  *image=clone_image;
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
  LockSemaphoreInfo((SemaphoreInfo *) image->semaphore);
  image->reference_count++;
  UnlockSemaphoreInfo((SemaphoreInfo *) image->semaphore);
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
%                                                                             %
%     R e s e t I m a g e P a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ResetImagePage adjusts the current page canvas and position based on a
%  relative page specification.
%
%  The format of the ResetImagePage method is:
%
%      MagickPassFail ResetImagePage(Image *image,const char *page)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o page: Relative page offset adjustment
%
*/
MagickExport MagickPassFail
ResetImagePage(Image *image,const char *page)
{
  RectangleInfo
    page_geometry;

  int
    flags;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  /* Parse page geometry */
  page_geometry.x=0;
  page_geometry.y=0;
  page_geometry.width=0;
  page_geometry.height=0;
  flags=GetMagickGeometry(page,&page_geometry.x,&page_geometry.y,
			  &page_geometry.width,&page_geometry.height);

  /* If no values were parsed, then return failed status */
  if (NoValue == flags)
    return MagickFail;

  /* If width was provided */
  if (flags & WidthValue)
    {
      /* If height was not provided, then default it to width */
      if (!(flags & HeightValue))
        page_geometry.height=page_geometry.width;
      image->page.width=page_geometry.width;
      image->page.height=page_geometry.height;
    }
  /* If values are absolute, then only adjust the page offset
     values */
  if (flags & AspectValue) /* ! */
    {
      if (flags & XValue)
        image->page.x+=page_geometry.x;
      if (flags & YValue)
        image->page.y+=page_geometry.y;
    }
  else
    {
      /* If values are not absolute, then use offset values, and page
	 width and height based on image width and height plus page
	 offsets */
      if (flags & XValue)
        {
          image->page.x=page_geometry.x;
          if ((image->page.width == 0) && (page_geometry.x > 0))
            image->page.width=image->columns+page_geometry.x;
        }
      if (flags & YValue)
        {
          image->page.y=page_geometry.y;
          if ((image->page.height == 0) && (page_geometry.y > 0))
            image->page.height=image->rows+page_geometry.y;
        }
    }

  return MagickPass;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
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
#define SetImageColorText "[%s] Set color..."
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
%   S e t I m a g e C o l o r                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetImageColor() sets the red, green, blue and opacity components of each
%  pixel to those from a specified pixel value.
%
%  The format of the SetImageColor method is:
%
%      MagickPassFail SetImageColor(Image *image,const PixelPacket *pixel)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o pixel: Set each pixel in the image to this pixel's color and transparency.
%
%
*/
MagickExport MagickPassFail SetImageColor(Image *image,
					  const PixelPacket *pixel)
{
  image->is_grayscale=IsGray(*pixel);
  image->is_monochrome=IsMonochrome(*pixel);
  return SetImageColorRegion(image,0,0,image->columns,image->rows,pixel);
}
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t I m a g e C o l o r R e g i o n                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetImageColorRegion() sets the red, green, blue and opacity components
%  of each pixel in the specified region to those from a specified pixel
%  value.
%
%  The format of the SetImageColorRegion method is:
%
%      MagickPassFail SetImageColorRegion(Image *image,
%                                         long x,
%                                         long y,
%                                         unsigned long width,
%                                         unsigned long height,
%                                         const PixelPacket *pixel)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o pixel: Set each pixel in the image to this pixel's color and transparency.
%
%
*/
MagickExport MagickPassFail
SetImageColorRegion(Image *image,
		    long x,
		    long y,
		    unsigned long width,
		    unsigned long height,
		    const PixelPacket *pixel)
{
  MagickPassFail
    status;

  MagickBool
    is_grayscale;

  MagickBool
    is_monochrome;

  assert(image != (Image *) NULL);
  assert(pixel != (PixelPacket *) NULL);
  assert(image->signature == MagickSignature);
  status=MagickPass;

  is_grayscale=(image->is_grayscale && IsGray(*pixel));
  is_monochrome=(image->is_monochrome && IsMonochrome(*pixel));

  if (pixel->opacity != OpaqueOpacity)
    image->matte=MagickTrue;
  image->colorspace=RGBColorspace;
  image->storage_class=DirectClass;

  status=PixelIterateMonoModify(SetImageColorCallBack,NULL,
				SetImageColorText,
				NULL,pixel,x,y,
				width,height,
				image,&image->exception);

  image->is_grayscale=is_grayscale;
  image->is_monochrome=is_monochrome;
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
%  SetImageInfo() inspects the filename field of the ImageInfo
%  structure.  Based on what it finds, it may update the `affirm',
%  `filename', `magick', `subimage', `subrange', `temporary', and
%  `tile' fields in the ImageInfo structure, and may even allocate a
%  temporary file. This is a powerful, mysterious, and anchient
%  function which supports the many special features associated with
%  input and output file specifications, and is intended for use only
%  within GraphicsMagick code.
%
%  If the file will be read and the file specification includes an
%  index enclosed in brackets after the file name and the file exists,
%  then it is assumed to specify a subimage of a multi-resolution image
%  format like Photo CD (e.g.  img0001.pcd[4]).  The `tile' `subimage',
%  and `subrange' fields will be updated.  The filename specification
%  is then truncated to remove the subimage specification.
%
%  The filename is inspected for an image format prefix. For example,
%  `ps:image' returns PS indicating a Postscript image.  If a format
%  prefix was found, then `filename' is updated to remove it.  The
%  `magick' field is set to the specified format and the `affirm' field
%  is set to indicate an explicit user request for the format (which
%  will not be overridden).
%
%  If the format is not yet known, the filename is inspected for an
%  image format extension.  If format support exists for this
%  extension, then the official format designator for that format is
%  written into the `magick' field. For example, "JPEG" is set in the
%  `magick' field for the filename: `image.jpg'. Some file extensions
%  are intentionally ignored due to potential confusion or security
%  issues.  The file extension is used as a strong hint of the file
%  format but is not authoritative.
%
%  If the file will be read, then its content is inspected for its
%  type.  If the input is not seekable, then its content is copied to a
%  temporary file, `filename` is updated with the name of the temporary
%  file, and `temporary' is set to true so that the temporary file may
%  be automatically deleted later.  The `magick' field is updated if
%  file header matches a known type.
%
%  MagickFail is returned if an error is encountered.
%
%  The format of the SetImageInfo method is:
%
%      MagickPassFail SetImageInfo(ImageInfo *image_info,
%        const unsigned int flags,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info..
%
%    o flags: Flag options based on an OR of SETMAGICK_READ, SETMAGICK_WRITE,
%        and SETMAGICK_RECTIFY. SETMAGICK_READ indicates that the file is to
%        be read, SETMAGICK_WRITE indicates that the the file will be written,
%        and SETMAGICK_RECTIFY indicates that the file specification should be
%        inspected for an embedded scene specification, and adjust the 'ajoin'
%        accordingly.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

static inline unsigned int
IsFrame(const char *point)
{
  char
    *p;

  (void) strtol(point,&p,10);
  return(p != point);
}

static MagickPassFail
ParseSubImageSpecification(char *filename,
			   char **tile_ptr,
			   unsigned long *subimage_ptr,
			   unsigned long *subrange_ptr,
			   ExceptionInfo *exception)
{
  char
    *spec_start,
    *spec_end;

  size_t
    filename_length;

  MagickPassFail
    status=MagickPass;

  assert(filename != (const char *) NULL);
  assert(tile_ptr != (char **) NULL);
  assert(subimage_ptr != (unsigned long *) NULL);
  assert(subrange_ptr != (unsigned long *) NULL);
  assert(exception != (ExceptionInfo*) NULL);

  MagickFreeMemory(*tile_ptr);
  filename_length=strlen(filename);
  if ((filename_length > 2) &&
      (filename_length < MaxTextExtent) &&
      (filename[filename_length-1] == ']') &&
      ((spec_start=strrchr(filename,'[')) != (const char *) NULL))
    {
      const char
	*digits;

      char
	spec[MaxTextExtent],
	*q;

      unsigned long
	subimage=0,
	subrange=0;

      unsigned long
	first,
	last;

      long
	value;

      spec_end=&filename[filename_length-1];
      spec_start++;
      (void) strlcpy(spec,spec_start,sizeof(spec));
      spec[spec_end-spec_start]='\0';

      /*
	Example of supported formats (as per documentation):

	4
	2,7,4
	4-7
	320x256+50+50
      */

      digits=spec;
      q=0;
      value=strtol(digits,&q,10);
      if (q <= digits) /* Parse error */
	goto invalid_subimage_specification;

      subimage=value;
      subrange=subimage;
      (void) CloneString(tile_ptr,spec);

      for (q=spec; *q != '\0'; )
	{
	  while (isspace((int)(unsigned char) *q) || (*q == ','))
	    q++;
	  digits=q;
	  value=strtol(digits,&q,10);
	  if (q <= digits) /* Parse error */
	    break;
	  first=value;
	  last=first;
	  while (isspace((int)(unsigned char) *q))
	    q++;
	  if (*q == '-')
	    {
	      digits=q+1;
	      value=strtol(digits,&q,10);
	      if (q <= digits) /* Parse error */
		break;
	      last=value;
	    }
	  else if ((*q != ',') && (*q != '\0'))
	    {
	      break; /* Parse error */
	    }
	  if (first > last)
	    Swap(first,last);
	  if (first < subimage)
	    subimage=first;
	  if (last > subrange)
	    subrange=last;
	}
      if (*q == '\0')
	{
	  subrange -= subimage-1;
	  *subimage_ptr=subimage;
	  *subrange_ptr=subrange;
	  status=MagickPass;
	}
      else if (IsGeometry(spec))
	{
	  status=MagickPass;
	}
      else
	{
	invalid_subimage_specification:
	  ThrowException(exception,OptionError,
			 InvalidSubimageSpecification,spec);
	  status=MagickFail;
	}
      if (status == MagickPass)
	{
	  /* Truncate filename */
	  *(spec_start-1)='\0';
	}
#if 0
      fprintf(stderr,"subimage=%lu subrange=%lu tile=\"%s\"\n",
	      *subimage_ptr,*subrange_ptr,
	      (*tile_ptr ? *tile_ptr : "(null)"));
#endif
    }

  return status;
}

MagickExport MagickPassFail
SetImageInfo(ImageInfo *image_info,const unsigned int flags,
	     ExceptionInfo *exception)
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

  unsigned int
    lflags;

  unsigned char
    magick[2*MaxTextExtent];

  MagickPassFail
    status=MagickPass;

  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);

  /*
    Ensure backward compatiblity with previous flags which used
    True/False logic.
  */
  lflags=flags;
  if (MagickFalse == flags)
    lflags=SETMAGICK_WRITE;
  else if (MagickTrue == flags)
    lflags=(SETMAGICK_WRITE | SETMAGICK_RECTIFY);

#if 0
  fprintf(stderr,"SetImageInfo \"%s\" Read=%d Write=%d Rectify=%d\n",
	  image_info->filename,
	  ((lflags & SETMAGICK_READ) > 0),
	  ((lflags & SETMAGICK_WRITE) > 0),
	  ((lflags & SETMAGICK_RECTIFY) > 0));
#endif

  *magic='\0';

  if (lflags & SETMAGICK_READ)
    {
      /*
	Look for sub-image specification (e.g. img0001.pcd[4]).
      */
      p=image_info->filename+Max((long) strlen(image_info->filename)-1,0);
      /*
	Sometimes the provided argument is a real file and we need to
	account for that.  If it is not a real file and the argument ends
	with ']' then the trailing part is likely a sub-image or size
	specification.
      */
      if (*p == ']' && !IsAccessibleNoLogging(image_info->filename))
	{
	  (void) ParseSubImageSpecification(image_info->filename,
					    &image_info->tile,
					    &image_info->subimage,
					    &image_info->subrange,
					    exception);
	}
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
      if ((LocaleCompare(p,".gz") == 0) ||
	  (LocaleCompare(p,".Z") == 0) ||
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

  if (image_info->affirm)
    return(MagickPass);
  if (lflags & SETMAGICK_READ)
    {
      /*
	Determine the file format from the first few bytes of the
	file.
      */
      image=AllocateImage(image_info);
      if (image == (Image *) NULL)
	return(MagickFail);
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
	      return(MagickFail);
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
    }

  return(MagickPass);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t I m a g e O p a c i t y                                             %
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
                                    "[%s] Modulate opacity...",
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
                                    "[%s] Set opacity...",
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
%      BilevelType, GrayscaleType, GrayscaleMatteType, PaletteType,
%      PaletteMatteType, TrueColorType, TrueColorMatteType,
%      ColorSeparationType, ColorSeparationMatteType, OptimizeType
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
%     S t r i p I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  StripImage removes all profiles and text attributes from the image.
%
%  The format of the StripImage method is:
%
%      MagickPassFail StripImage(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
*/
MagickExport MagickPassFail
StripImage(Image *image)
{
  /* Text attributes to strip.  List is NULL terminated. */
  static const char *strip_attributes[] =
    {
      "artist",
      "comment",
      "copyright",
      "hostcomputer",
      "label",
      "make",
      "model",
      "timestamp",
      (const char *) NULL
    };

  unsigned int
    i;

  assert(image != (Image *) NULL);

  /* Strip all profiles */
  (void) ProfileImage(image,"*",NULL,0,MagickFalse);

  /* Strip common text attributes */
  for (i=0; strip_attributes[i] != NULL; i++)
    (void) SetImageAttribute(image,strip_attributes[i],(char *) NULL);

  return MagickPass;
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
#define SyncImageText "[%s] Synchronizing pixels..."
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
