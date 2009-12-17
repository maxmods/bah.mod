/*
% Copyright (C) 2003 - 2009 GraphicsMagick Group
% Copyright (C) 2003 ImageMagick Studio
% Copyright 1991-1999 E. I. du Pont de Nemours and Company
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
% GraphicsMagick Image Analysis Methods
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/analyze.h"
#include "magick/color.h"
#include "magick/monitor.h"
#include "magick/pixel_cache.h"
#include "magick/pixel_iterator.h"
#include "magick/utility.h"

/*
  Constant declaration.
*/

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
#define GetImageBoundingBoxText "[%s] Get bounding box..."
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
#  pragma omp critical (GM_GetImageBoundingBox)
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
#  pragma omp critical (GM_GetImageBoundingBox)
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
  /*
    If we fail to find smaller bounds, then return original image
    dimensions.
  */
  if ((bounds.width == 0) || (bounds.height == 0))
    {
      bounds.width=image->columns;
      bounds.height=image->rows;
      bounds.x=0;
      bounds.y=0;
    }

  return(bounds);
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
  
  map = MagickAllocateArray(unsigned char *, MaxMap+1, sizeof(magick_uint8_t));
  if (map != (unsigned char *) NULL)
    {
      unsigned int
        i;
      
      for (i=0; i <= MaxMap; i++)
        map[i] = (magick_uint8_t) MinimumDepthForValue(i);
    }
  return map;
}
#endif /* MaxMap == MaxRGB */
#define GetImageDepthText "[%s] Get depth..."

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
#  pragma omp critical (GM_GetImageDepthCallBack)
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
#  pragma omp critical (GM_GetImageDepthCallBack)
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

  MagickFreeMemory(map);

  return depth;
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
#define AnalyzeImageText "[%s] Analyze...  "
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
MagickExport ImageType
GetImageType(const Image *image,ExceptionInfo *exception)
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
%                                                                             %
%     I s G r a y I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IsGrayImage() returns MagickTrue if all the pixels in the image have the same
%  red, green, and blue intensities.
%
%  The format of the IsGrayImage method is:
%
%      MagickBool IsGrayImage(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status: Method IsGrayImage returns MagickTrue if the image is grayscale
%      otherwise MagickFalse is returned.
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
#define AnalyzeGrayImageText "[%s] Analyze for gray..."
MagickExport MagickBool IsGrayImage(const Image *image,
  ExceptionInfo *exception)
{
  unsigned long
    y;

  register const PixelPacket
    *p;

  register unsigned long
    x;

  MagickBool
    is_grayscale;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  if (image->colorspace == CMYKColorspace)
    return(MagickFalse);
  if (image->is_grayscale)
    return(MagickTrue);
  is_grayscale=MagickTrue;
  switch (image->storage_class)
  {
    case DirectClass:
    case UndefinedClass:
    {
      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                            "IsGrayImage(): Exhaustive pixel test!");
      for (y=0; y < image->rows; y++)
      {
        p=AcquireImagePixels(image,0,y,image->columns,1,exception);
        if (p == (const PixelPacket *) NULL)
          return(MagickFalse);
        for (x=image->columns; x != 0; x--)
        {
          if ((p->red != p->green) || (p->green != p->blue))
            {
              is_grayscale=MagickFalse;
              break;
            }
          p++;
        }
        if (!is_grayscale)
          break;
        if (QuantumTick(y,image->rows))
          if (!MagickMonitorFormatted(y,image->rows,
                                      exception,AnalyzeGrayImageText,
                                      image->filename))
            break;
      }
      break;
    }
    case PseudoClass:
    {
      p=image->colormap;
      for (x=image->colors; x != 0; x--)
        {
          if ((p->red != p->green) || (p->green != p->blue))
            {
              is_grayscale=MagickFalse;
              break;
            }
          p++;
        }
      break;
    }
  }

  /*
    Force progress indication to 100%
  */
  if (!is_grayscale)
    (void) MagickMonitorFormatted(image->rows-1,image->rows,exception,
                                  AnalyzeGrayImageText,image->filename);

  ((Image *)image)->is_grayscale=is_grayscale;
  return(is_grayscale);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s M o n o c h r o m e I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IsMonochromeImage() returns MagickTrue if all the pixels in the image have
%  the same red, green, and blue intensities and the intensity is either
%  0 or MaxRGB.
%
%  The format of the IsMonochromeImage method is:
%
%      MagickBool IsMonochromeImage(const Image *image,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
#define AnalyzeBilevelImageText "[%s] Analyze for bilevel..."
MagickExport MagickBool IsMonochromeImage(const Image *image,
  ExceptionInfo *exception)
{
  unsigned long
    y;

  register const PixelPacket
    *p;

  register unsigned long
    x;

  MagickBool
    is_monochrome;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  if (image->colorspace == CMYKColorspace)
    return(MagickFalse);
  if (image->is_monochrome)
    return(MagickTrue); 
  is_monochrome=MagickTrue;
  switch (image->storage_class)
  {
    case DirectClass:
    case UndefinedClass:
    {
      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                            "IsMonochromeImage(): Exhaustive pixel test!");
      for (y=0; y < image->rows; y++)
      {
        p=AcquireImagePixels(image,0,y,image->columns,1,exception);
        if (p == (const PixelPacket *) NULL)
          return(MagickFalse);
        for (x=image->columns; x != 0; x--)
        {
          if ((p->red != p->green) || (p->green != p->blue) ||
              ((p->red != 0) && (p->red != MaxRGB)))
            {
              is_monochrome=MagickFalse;
              break;
            }
          p++;
        }
        if (!is_monochrome)
          break;
        if (QuantumTick(y,image->rows))
          if (!MagickMonitorFormatted(y,image->rows,exception,
                                      AnalyzeBilevelImageText,image->filename))
            break;
      }
      break;
    }
    case PseudoClass:
    {
      p=image->colormap;
      for (x=image->colors; x != 0; x--)
      {
        if ((p->red != p->green) || (p->green != p->blue) ||
            ((p->red != 0) && (p->red != MaxRGB)))
          {
            is_monochrome=MagickFalse;
            break;
          }
        p++;
      }
      break;
    }
  }

  /*
    Force progress indication to 100%
  */
  if (!is_monochrome)
    (void) MagickMonitorFormatted(image->rows-1,image->rows,exception,
                                  AnalyzeBilevelImageText,image->filename);

  ((Image *)image)->is_monochrome=is_monochrome;
  return(is_monochrome);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%     I s O p a q u e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IsOpaqueImage() returns MagickTrue if none of the pixels in the image have an
%  opacity value other than opaque (0).
%
%  The format of the IsOpaqueImage method is:
%
%      MagickBool IsOpaqueImage(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status: Method IsOpaqueImage returns MagickFalse if the image has one or more
%      pixels that are transparent otherwise MagickTrue is returned.
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
#define AnalyzeOpaqueImageText "[%s] Analyze for opacity..."
MagickExport MagickBool IsOpaqueImage(const Image *image,
  ExceptionInfo *exception)
{
  unsigned long
    y;

  register const PixelPacket
    *p;

  register unsigned long
    x;

  MagickBool
    is_opaque;

  /*
    Determine if image is opaque.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (!image->matte)
    return(MagickTrue);
  is_opaque=MagickTrue;
  (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                        "IsOpaqueImage(): Exhaustive pixel test!");
  for (y=0; y < image->rows; y++)
    {
      p=AcquireImagePixels(image,0,y,image->columns,1,exception);
      if (p == (const PixelPacket *) NULL)
        return(MagickFalse);
      for (x=image->columns; x > 0; x--)
        {
          if (p->opacity != OpaqueOpacity)
            {
              is_opaque=MagickFalse;
              break;
            }
          p++;
        }
      if (!is_opaque)
        break;
      if (QuantumTick(y,image->rows))
        if (!MagickMonitorFormatted(y,image->rows,exception,
                                    AnalyzeOpaqueImageText,image->filename))
          break;
    }

  /*
    Force progress indication to 100%
  */
  if (!is_opaque)
    (void) MagickMonitorFormatted(image->rows-1,image->rows,exception,
                                  AnalyzeOpaqueImageText,image->filename);

  return(is_opaque);
}
