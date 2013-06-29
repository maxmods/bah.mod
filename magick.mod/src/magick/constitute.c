/*
% Copyright (C) 2003 - 2012 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%     CCCC   OOO   N   N  SSSSS  TTTTT  IIIII  TTTTT  U   U  TTTTT  EEEEE     %
%    C      O   O  NN  N  SS       T      I      T    U   U    T    E         %
%    C      O   O  N N N  ESSS     T      I      T    U   U    T    EEE       %
%    C      O   O  N  NN     SS    T      I      T    U   U    T    E         %
%     CCCC   OOO   N   N  SSSSS    T    IIIII    T     UUU     T    EEEEE     %
%                                                                             %
%                                                                             %
%                      Methods to Constitute an Image                         %
%                                                                             %
%                                                                             %
%                             Software Design                                 %
%                               John Cristy                                   %
%                               October 1998                                  %
%                                                                             %
%                           Re-implemented By                                 %
%                            Bob Friesenhahn                                  %
%                               2003-2008                                     %
%                                                                             %
%                         Small Float Support By                              %
%                             Richard Nolde                                   %
%                             December, 2008                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/analyze.h"
#include "magick/attribute.h"
#include "magick/blob.h"
#include "magick/colormap.h"
#include "magick/constitute.h"
#include "magick/delegate.h"
#include "magick/describe.h"
#include "magick/enum_strings.h"
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/pixel_cache.h"
#include "magick/semaphore.h"
#include "magick/tempfile.h"
#include "magick/utility.h"

/*
  Type definitions
*/
typedef enum
{
  BlueMapQuanum,
  GreenMapQuantum,
  IntensityMapQuantum,
  TransparencyMapQuantum,
  PadMapQuantum,
  RedMapQuantum,
  OpacityMapQuantum
} MapQuantumType;

typedef enum {
  UndefinedDispatchType,
  BGRDispatchType,
  BGRADispatchType,
  BGRPDispatchType,
  RGBDispatchType,
  RGBADispatchType,
  IDispatchType
} DispatchType;

static SemaphoreInfo
  *constitute_semaphore = (SemaphoreInfo *) NULL;

/*
  Forward declarations.
*/
static Image
  *ReadImages(const ImageInfo *,ExceptionInfo *);


/*
  Macros
*/

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o n s t i t u t e I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ConstituteImage() returns an Image corresponding to an image stored
%  in a raw memory array format. The pixel data must be in scanline order
%  top-to-bottom. The data can be unsigned char, unsigned short int, unsigned
%  int, unsigned long, float, or double.  Float and double require the pixels
%  to be normalized to the range [0..1], otherwise the range is [0..MaxVal]
%  where MaxVal is the maximum possible value for that type.
%
%  Note that for most 32-bit architectures the size of an unsigned long is
%  the same as unsigned int, but for 64-bit architectures observing the LP64
%  standard, an unsigned long is 64 bits, while an unsigned int remains 32
%  bits. This should be considered when deciding if the data should be
%  described as "Integer" or "Long".
%
%  For example, to create a 640x480 image from unsigned red-green-blue
%  character data, use
%
%      image=ConstituteImage(640,480,"RGB",CharPixel,pixels,&exception);
%
%  The format of the Constitute method is:
%
%      Image *ConstituteImage(const unsigned long width,
%        const unsigned long height,const char *map,const StorageType type,
%        const void *pixels,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o width: width in pixels of the image.
%
%    o height: height in pixels of the image.
%
%    o map: This string reflects the expected ordering of the pixel array.
%      It can be any combination or order of R = red, G = green, B = blue,
%      A = alpha (same as Transparency), O = Opacity, T = Transparency,
%      C = cyan, Y = yellow, M = magenta, K = black, or I = intensity
%      (for grayscale). Specify "P" = pad, to skip over a quantum which is
%      intentionally ignored. Creation of an alpha channel for CMYK images
%      is currently not supported.
%
%    o type: Define the data type of the pixels.  Float and double types are
%      expected to be normalized [0..1] otherwise [0..MaxRGB].  Choose from
%      these types: CharPixel, ShortPixel, IntegerPixel, LongPixel, FloatPixel,
%      or DoublePixel.
%
%    o pixels: This array of values contain the pixel components as defined by
%      map and type.  You must preallocate this array where the expected
%      length varies depending on the values of width, height, map, and type.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

MagickExport Image *ConstituteImage(const unsigned long width,
  const unsigned long height,const char *map,const StorageType type,
  const void *pixels,ExceptionInfo *exception)
{
  Image
    *image;

  long
    y;

  PixelPacket
    * restrict q;

  register Quantum
    quantum;

  MapQuantumType
    switch_map[MaxTextExtent/sizeof(MapQuantumType)];

  register IndexPacket
    * restrict indexes;

  register long
    i,
    x;

  size_t
    length;

  /*
    Allocate image structure.
  */
  assert(pixels != (void *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  /* SetExceptionInfo(exception,UndefinedException); */
  image=AllocateImage((ImageInfo *) NULL);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  if ((width == 0) || (height == 0))
    ThrowBinaryException3(OptionError,UnableToConstituteImage,
      NonzeroWidthAndHeightRequired);
  image->columns=width;
  image->rows=height;

  /*
    Handle a few common special cases in order to improve performance.
  */
  if (type == CharPixel)
    {
      DispatchType
        dispatch_type=UndefinedDispatchType;

      if (LocaleCompare(map,"BGR") == 0)
	dispatch_type=BGRDispatchType;
      else if (LocaleCompare(map,"BGRA") == 0)
	dispatch_type=BGRADispatchType;
      else if (LocaleCompare(map,"BGRP") == 0)
	dispatch_type=BGRPDispatchType;
      else if (LocaleCompare(map,"RGB") == 0)
	dispatch_type=RGBDispatchType;
      else if (LocaleCompare(map,"RGBA") == 0)
	dispatch_type=RGBADispatchType;
      else if (LocaleCompare(map,"I") == 0)
        {
          if (!AllocateImageColormap(image,MaxColormapSize))
            ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
                                 UnableToConstituteImage);
          dispatch_type=IDispatchType;
        }

      if (dispatch_type != UndefinedDispatchType)
        {
          register const unsigned char
            * restrict p = (const unsigned char*) pixels;

          for (y=0; y < (long) image->rows; y++)
            {
              q=SetImagePixels(image,0,y,image->columns,1);
              if (q == (PixelPacket *) NULL)
                break;
              indexes=AccessMutableIndexes(image);

              switch (dispatch_type)
                {
                case BGRDispatchType:
                  {
                    for (x=(long) image->columns; x != 0; x--)
                      {
                        SetBlueSample(q,ScaleCharToQuantum(*p++));
                        SetGreenSample(q,ScaleCharToQuantum(*p++));
                        SetRedSample(q,ScaleCharToQuantum(*p++));
                        SetOpacitySample(q,OpaqueOpacity);
                        q++;
                      }
                    break;
                  }
                case BGRADispatchType:
                  {
		    image->matte=True;
                    for (x=(long) image->columns; x != 0; x--)
                      {
                        SetBlueSample(q,ScaleCharToQuantum(*p++));
                        SetGreenSample(q,ScaleCharToQuantum(*p++));
                        SetRedSample(q,ScaleCharToQuantum(*p++));
                        SetOpacitySample(q,(Quantum) MaxRGB-ScaleCharToQuantum(*p++));
                        q++;
                      }
                    break;
                  }
                case BGRPDispatchType:
                  {
                    for (x=(long) image->columns; x != 0; x--)
                      {
                        SetBlueSample(q,ScaleCharToQuantum(*p++));
                        SetGreenSample(q,ScaleCharToQuantum(*p++));
                        SetRedSample(q,ScaleCharToQuantum(*p++));
                        p++;
                        SetOpacitySample(q,OpaqueOpacity);
                        q++;
                      }
                    break;
                  }
                case RGBDispatchType:
                  {
                    for (x=(long) image->columns; x != 0; x--)
                      {
                        SetRedSample(q,ScaleCharToQuantum(*p++));
                        SetGreenSample(q,ScaleCharToQuantum(*p++));
                        SetBlueSample(q,ScaleCharToQuantum(*p++));
                        SetOpacitySample(q,OpaqueOpacity);
                        q++;
                      }
                    break;
                  }
                case RGBADispatchType:
                  {
		    image->matte=True;
                    for (x=(long) image->columns; x != 0; x--)
                      {
                        SetRedSample(q,ScaleCharToQuantum(*p++));
                        SetGreenSample(q,ScaleCharToQuantum(*p++));
                        SetBlueSample(q,ScaleCharToQuantum(*p++));
                        SetOpacitySample(q,MaxRGB-ScaleCharToQuantum(*p++));
                        q++;
                      }
                    break;
                  }
                case IDispatchType:
                  {
                    for (x=(long) image->columns; x != 0; x--)
                      {
                        *indexes=ScaleQuantumToIndex(ScaleCharToQuantum(*p++));
                        SetGraySample(q,image->colormap[*indexes].red);
                        SetOpacitySample(q,OpaqueOpacity);
                        indexes++;
                        q++;
                      }
                    break;
                  }
                case UndefinedDispatchType:
                  {
                    break;
                  }
                } /* end switch */
              if (!SyncImagePixels(image))
                break;
            } /* end for (y=0; y < (long) image->rows; y++) */
          if (dispatch_type == IDispatchType)
            {
              (void) IsMonochromeImage(image,exception);
              image->is_grayscale=True;
            }
          return (image);
        } /* end if (dispatch_type != UndefinedDispatchType) */
    } /* end if (type == CharPixel) */

  /*
    Prepare a validated and more efficient version of the map.
  */
  length=strlen(map);
  length=Min(length,sizeof(switch_map)/sizeof(MapQuantumType));
  for (i=0; i < (long) length; i++)
    {
      switch ((int) toupper((int) map[i]))
        {
        case 'R':
          {
            switch_map[i]=RedMapQuantum;
            break;
          }
        case 'G':
          {
            switch_map[i]=GreenMapQuantum;
            break;
          }
        case 'B':
          {
            switch_map[i]=BlueMapQuanum;
            break;
          }
        case 'A':
          {
            switch_map[i]=TransparencyMapQuantum;
            image->matte=True;
            break;
          }
        case 'C':
          {
            image->colorspace=CMYKColorspace;
            switch_map[i]=RedMapQuantum;
            break;
          }
        case 'M':
          {
            image->colorspace=CMYKColorspace;
            switch_map[i]=GreenMapQuantum;
            break;
          }
        case 'Y':
          {
            image->colorspace=CMYKColorspace;
            switch_map[i]=BlueMapQuanum;
            break;
          }
        case 'K':
          {
            image->colorspace=CMYKColorspace;
            switch_map[i]=OpacityMapQuantum;
            break;
          }
        case 'I':
          {
            if (!AllocateImageColormap(image,MaxColormapSize))
              ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
                UnableToConstituteImage);
            switch_map[i]=IntensityMapQuantum;
            break;
          }
        case 'O':
          {
            switch_map[i]=OpacityMapQuantum;
            image->matte=True;
            break;
          }
        case 'P':
          {
            switch_map[i]=PadMapQuantum;
            break;
          }
        case 'T':
          {
            switch_map[i]=TransparencyMapQuantum;
            image->matte=True;
            break;
          }
        default:
          {
            DestroyImage(image);
            ThrowImageException(OptionError,UnrecognizedPixelMap,map)
          }
        }
    }

  /*
    Transfer the pixels from the pixel data array to the image.
  */
  for (y=0; y < (long) image->rows; y++)
    {
      q=SetImagePixels(image,0,y,image->columns,1);
      if (q == (PixelPacket *) NULL)
        break;
      indexes=AccessMutableIndexes(image);
      for (x=0; x < (long) image->columns; x++)
        {
	  SetRedSample(q,0);
	  SetGreenSample(q,0);
	  SetBlueSample(q,0);
          if (image->colorspace == CMYKColorspace)
            {
              SetBlackSample(q,0);
              *indexes=OpaqueOpacity;
            }
          else
            {
              SetOpacitySample(q,OpaqueOpacity);
            }
          for (i=0; i < (long) length; i++)
            {
              /*
                Input a quantum
              */
              quantum=0U;
              
              switch (type)
                {
                case CharPixel:
                  {
                    register const unsigned char *p = (const unsigned char*) pixels;
                    quantum=ScaleCharToQuantum(*p++);
                    pixels = (const void *) p;
                    break;
                  }
                case ShortPixel:
                  {
                    register const unsigned short *p = (const unsigned short*) pixels;
                    quantum=ScaleShortToQuantum(*p++);
                    pixels = (const void *) p;
                    break;
                  }
                case IntegerPixel:
                  {
                    register const unsigned int *p = (const unsigned int*) pixels;
                    quantum=ScaleLongToQuantum(*p++);
                    pixels = (const void *) p;
                    break;
                  }
                case LongPixel:
                  {
                    register const unsigned long *p = (const unsigned long*) pixels;
                    quantum=ScaleLongToQuantum(*p++);
                    pixels = (const void *) p;
                    break;
                  }
                case FloatPixel:
                  {
                    double quantum_float;
                    register const float *p = (const float*) pixels;
                    quantum_float=(double) MaxRGB*(*p++);
                    quantum=RoundDoubleToQuantum(quantum_float);
                    pixels = (const void *) p;
                    break;
                  }
                case DoublePixel:
                  {
                    double quantum_float;
                    register const double *p = (const double*) pixels;
                    quantum_float=(double) MaxRGB*(*p++);
                    quantum=RoundDoubleToQuantum(quantum_float);
                    pixels = (const void *) p;
                    break;
                  }
                }
              
              /*
                Transfer quantum to image
              */
              switch (switch_map[i])
                {
                case RedMapQuantum:
                  {
                    SetRedSample(q,quantum);
                    break;
                  }
                case GreenMapQuantum:
                  {
                    SetGreenSample(q,quantum);
                    break;
                  }
                case BlueMapQuanum:
                  {
                    SetBlueSample(q,quantum);
                    break;
                  }
                case OpacityMapQuantum:
                  {
                    SetOpacitySample(q,quantum);
                    break;
                  }
                case TransparencyMapQuantum:
                  {
                    SetOpacitySample(q,MaxRGB-quantum);
                    break;
                  }
                case IntensityMapQuantum:
                  {
                    *indexes=ScaleQuantumToIndex(quantum);
                    SetGraySample(q,image->colormap[*indexes].red);
                    break;
                  }
                case PadMapQuantum:
                  {
                    /* Discard quantum */
                    break;
                  }
                }
            }
          indexes++;
          q++;
        }
      if (!SyncImagePixels(image))
        break;
    }
  
  if (image->storage_class == PseudoClass)
    {
      /*
        Check and cache monochrome and grayscale status
      */
      (void) IsMonochromeImage(image,exception);
      if (image->is_monochrome)
        image->is_grayscale=True;
      else
        (void) IsGrayImage(image,exception);
    }
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y C o n s t i t u t e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyConstitute() destroys the constitute environment.
%
%  The format of the DestroyConstitute method is:
%
%      DestroyConstitute(void)
%
%
*/
MagickExport void DestroyConstitute(void)
{
  DestroySemaphoreInfo(&constitute_semaphore);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D i s p a t c h I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DispatchImage() extracts pixel data from an Image into a raw memory array.
%  The pixel data is written in scanline order top-to-bottom using an 
%  arbitrary quantum order specified by 'map', and with quantum size
%  specified by 'type'.
%
%  The output array data may be unsigned char, unsigned short int, unsigned
%  int, unsigned long, float, or double.  Float and double require the pixels
%  to be normalized to the range [0..1], otherwise the range is [0..MaxVal]
%  where MaxVal is the maximum possible value for that type.
%
%  The method returns MagickPass on success or MagickFail if an error is
%  encountered.
%
%  Suppose we want want to extract the first scanline of a 640x480 image as
%  character data in red-green-blue order:
%
%      DispatchImage(image,0,0,640,1,"RGB",0,pixels,exception);
%
%  The format of the DispatchImage method is:
%
%      MagickPassFail DispatchImage(const Image *image,const long x_offset,
%        const long y_offset,const unsigned long columns,
%        const unsigned long rows,const char *map,const StorageType type,
%        void *pixels,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o x_offset, y_offset, columns, rows:  These values define the perimeter
%      of a region of pixels you want to extract.
%
%    o map: This string reflects the expected ordering of the pixel array.
%      It can be any combination or order of R = red, G = green, B = blue,
%      A = alpha  (same as Transparency), O = Opacity, T = Transparency,
%      C = cyan, Y = yellow, M = magenta, K = black, I = intensity (for
%      grayscale). Specify "P" = pad, to output a pad quantum. Pad quantums
%      are zero-value.
%
%    o type: Define the data type of the pixels.  Float and double types are
%      expected to be normalized [0..1] otherwise [0..MaxRGB].  Choose from
%      these types: CharPixel, ShortPixel, IntegerPixel, LongPixel, FloatPixel,
%      or DoublePixel.
%
%    o pixels: This array of values contain the pixel components as defined by
%      map and type.  You must preallocate this array where the expected
%      length varies depending on the values of width, height, map, and type.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport MagickPassFail DispatchImage(const Image *image,const long x_offset,
  const long y_offset,const unsigned long columns,const unsigned long rows,
  const char *map,const StorageType type,void *pixels,ExceptionInfo *exception)
{
  long
    y;

  register long
    i,
    x;

  register const PixelPacket
    * restrict p;

  register Quantum
    quantum;

  MapQuantumType
    switch_map[MaxTextExtent/sizeof(MapQuantumType)];

  size_t
    length;

  MagickPassFail
    status=MagickPass;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  /*
    Handle a few common special cases in order to improve performance.
  */
  if (type == CharPixel)
    {
      DispatchType
        dispatch_type=UndefinedDispatchType;

      if (LocaleCompare(map,"BGR") == 0)
        dispatch_type=BGRDispatchType;
      else if (LocaleCompare(map,"BGRA") == 0)
        dispatch_type=BGRADispatchType;
      else if (LocaleCompare(map,"BGRP") == 0)
        dispatch_type=BGRPDispatchType;
      else if (LocaleCompare(map,"RGB") == 0)
        dispatch_type=RGBDispatchType;
      else if (LocaleCompare(map,"RGBA") == 0)
        dispatch_type=RGBADispatchType;
      else if (LocaleCompare(map,"I") == 0)
        dispatch_type=IDispatchType;

      if (dispatch_type != UndefinedDispatchType)
        {
          register unsigned char
            * restrict q = (unsigned char*) pixels;
          
          for (y=0; y < (long) rows; y++)
            {
              p=AcquireImagePixels(image,x_offset,y_offset+y,columns,1,exception);
              if (p == (const PixelPacket *) NULL)
                {
                  status=MagickFail;
                  break;
                }
              
              switch (dispatch_type)
                {
                case BGRDispatchType:
                  {
                    for (x=(long) columns; x != 0; x--)
                      {
                        *q++=ScaleQuantumToChar(GetBlueSample(p));
                        *q++=ScaleQuantumToChar(GetGreenSample(p));
                        *q++=ScaleQuantumToChar(GetRedSample(p));
                        p++;
                      }
                    break;
                  }
                case BGRADispatchType:
                  {
                    for (x=(long) columns; x != 0; x--)
                      {
                        *q++=ScaleQuantumToChar(GetBlueSample(p));
                        *q++=ScaleQuantumToChar(GetGreenSample(p));
                        *q++=ScaleQuantumToChar(GetRedSample(p));
                        *q++=ScaleQuantumToChar(MaxRGB-GetOpacitySample(p));
                        p++;
                      }
                    break;
                  }
                case BGRPDispatchType:
                  {
                    for (x=(long) columns; x != 0; x--)
                      {
                        *q++=ScaleQuantumToChar(GetBlueSample(p));
                        *q++=ScaleQuantumToChar(GetGreenSample(p));
                        *q++=ScaleQuantumToChar(GetRedSample(p));
                        *q++=0;
                        p++;
                      }
                    break;
                  }
                case RGBDispatchType:
                  {
                    for (x=(long) columns; x != 0; x--)
                      {
                        *q++=ScaleQuantumToChar(GetRedSample(p));
                        *q++=ScaleQuantumToChar(GetGreenSample(p));
                        *q++=ScaleQuantumToChar(GetBlueSample(p));
                        p++;
                      }
                    break;
                  }
                case RGBADispatchType:
                  {
                    for (x=(long) columns; x != 0; x--)
                      {
                        *q++=ScaleQuantumToChar(GetRedSample(p));
                        *q++=ScaleQuantumToChar(GetGreenSample(p));
                        *q++=ScaleQuantumToChar(GetBlueSample(p));
                        *q++=ScaleQuantumToChar(MaxRGB-GetOpacitySample(p));
                        p++;
                      }
                    break;
                  }
                case IDispatchType:
                  {
                    if (image->is_grayscale)
                      {
                        for (x=(long) columns; x != 0; x--)
                          {
                            *q++=ScaleQuantumToChar(GetGraySample(p));
                            p++;
                          }
                      }
                    else
                      {
                        for (x=(long) columns; x != 0; x--)
                          {
                            *q++=ScaleQuantumToChar(PixelIntensity(p));
                            p++;
                          }
                      }
                    break;
                  }
                case UndefinedDispatchType:
                  {
                  }
                }
            }
          return (status);
        }
    }

  length=strlen(map);
  length=Min(length,sizeof(switch_map)/sizeof(MapQuantumType));

  /*
    Prepare a validated and more efficient version of the map.
  */
  for (i=0; i < (long) length; i++)
    {
      switch ((int) toupper((int) map[i]))
        {
        case 'R':
          {
            switch_map[i]=RedMapQuantum;
            break;
          }
        case 'G':
          {
            switch_map[i]=GreenMapQuantum;
            break;
          }
        case 'B':
          {
            switch_map[i]=BlueMapQuanum;
            break;
          }
        case 'A':
        case 'T':
          {
            switch_map[i]=TransparencyMapQuantum;
            break;
          }
        case 'C':
          {
            switch_map[i]=RedMapQuantum;
            if (image->colorspace == CMYKColorspace)
              break;
            ThrowException(exception,OptionError,ColorSeparatedImageRequired,map);
            return(MagickFail);
          }
        case 'M':
          {
            switch_map[i]=GreenMapQuantum;
            if (image->colorspace == CMYKColorspace)
              break;
            ThrowException(exception,OptionError,ColorSeparatedImageRequired,map);
            return(MagickFail);
          }
        case 'Y':
          {
            switch_map[i]=BlueMapQuanum;
            if (image->colorspace == CMYKColorspace)
              break;
            ThrowException(exception,OptionError,ColorSeparatedImageRequired,map);
            return(MagickFail);
          }
        case 'K':
          {
            switch_map[i]=OpacityMapQuantum;
            if (image->colorspace == CMYKColorspace)
              break;
            ThrowException(exception,OptionError,ColorSeparatedImageRequired,map);
            return(MagickFail);
          }
        case 'I':
          {
            switch_map[i]=IntensityMapQuantum;
            break;
          }
        case 'O':
          {
            switch_map[i]=OpacityMapQuantum;
            break;
          }
        case 'P':
          {
            switch_map[i]=PadMapQuantum;
            break;
          }
        default:
          {
            ThrowException(exception,OptionError,UnrecognizedPixelMap,map);
            return(MagickFail);
          }
        }
    }
  
  for (y=0; y < (long) rows; y++)
    {
      p=AcquireImagePixels(image,x_offset,y_offset+y,columns,1,exception);
      if (p == (const PixelPacket *) NULL)
        {
          status=MagickFail;
          break;
        }
      for (x=0; x < (long) columns; x++)
        {
          for (i=0; i < (long) length; i++)
            {
              /*
                Obtain quantum value
              */
              quantum=0U;
              switch (switch_map[i])
                {
                case RedMapQuantum:
                  {
                    quantum=GetRedSample(p);
                    break;
                  }
                case GreenMapQuantum:
                  {
                    quantum=GetGreenSample(p);
                    break;
                  }
                case BlueMapQuanum:
                  {
                    quantum=GetBlueSample(p);
                    break;
                  }
                case IntensityMapQuantum:
                  {
                    if (image->is_grayscale)
                      {
                        quantum=GetRedSample(p);
                      }
                    else
                      {
                        double intensity = PixelIntensity(p);
                        quantum=RoundDoubleToQuantum(intensity);
                      }
                    break;
                  }
                case TransparencyMapQuantum:
                  {
                    if (image->matte)
                      quantum=GetOpacitySample(p);
                    quantum=MaxRGB-quantum;
                    break;
                  }
                case OpacityMapQuantum:
                  {
                    if ((image->matte) ||
			(image->colorspace == CMYKColorspace))
                      quantum=GetOpacitySample(p);
                    break;
                  }
                case PadMapQuantum:
                  {
                    /* Zero quantum */
                    break;
                  }
                }

              /*
                Output quantum
              */
              switch (type)
                {
                case CharPixel:
                  {
                    register unsigned char *q = (unsigned char*) pixels;
                    *q++=ScaleQuantumToChar(quantum);
                    pixels=(void *) q;
                    break;
                  }
                case ShortPixel:
                  {
                    register unsigned short *q = (unsigned short*) pixels;
                    *q++=ScaleQuantumToShort(quantum);
                    pixels=(void *) q;
                    break;
                  }
                case IntegerPixel:
                  {
                    register unsigned int *q = (unsigned int*) pixels;
                    *q++=ScaleQuantumToLong(quantum);
                    pixels=(void *) q;
                    break;
                  }
                case LongPixel:
                  {
                    register unsigned long *q = (unsigned long*) pixels;
                    *q++=ScaleQuantumToLong(quantum);
                    pixels=(void *) q;
                    break;
                  }
                case FloatPixel:
                  {
                    register float *q = (float*) pixels;
                    *q++=(float) ((double) quantum/MaxRGB);
                    pixels=(void *) q;
                    break;
                  }
                case DoublePixel:
                  {
                    register double *q = (double*) pixels;
                    *q++=(double) quantum/MaxRGB;
                    pixels=(void *) q;
                    break;
                  }
                }
            }
          p++;
        }
    }
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k F i n d R a w I m a g e M i n M a x                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickFindRawImageMinMax() obtains the minimum and maximum sample values
%  for a raw image.  The image blob must already be open with its current
%  seek offset pointing to the start of the raw data.  The seek offset is
%  restored when this function returns.  File data is processed on a 
%  scanline basis in order to minimize memory consumption.  The purpose of
%  this function is to support pre-scanning a raw image to find its maximum
%  values so that appropriate scaling may be applied when the data is read a
%  second time. MagickFail is returned if a problem occurs while scanning the
%  data.
%
%  The format of the MagickFindRawImageMinMax method is:
%
%      MagickPassFail MagickFindRawImageMinMax(Image *image, EndianType endian,
%                   unsigned long width, unsigned long height,StorageType type,
%                   unsigned scanline_octets, void *scanline_buffer,
%                   double *min, double *max)
%
%  A description of each parameter follows:
%
%    o image: Pointer to an image with an already open blob, with seek
%         offset pointing to raw data.
%
%    o endian: Endian order of raw data (LSBEndian or MSBEndian)
%
%    o width: Number of raw samples in a scanline.
%
%    o height: Number of scanlines to process.
%
%    o type: Raw data storage type.
%
%    o scanline_octets: Number of octets to read per scanline.
%
%    o scanline_buffer: Working buffer for scanlines.  Allocation size
%         must at least be enough to contain scanline octets.
%
%    o min: Pointer to double value to update with minimum value.
%
%    o max: Pointer to double value to update with maximum value.
%
*/
#define MagickFindMinMax(status,image,read_func,basic_type,scanline_octets,scanline_buffer,min,max) \
  {                                                                     \
    unsigned long                                                       \
      y;                                                                \
                                                                        \
    for (y = 0; y < height; y++)                                        \
      {                                                                 \
        unsigned long                                                   \
          x;                                                            \
                                                                        \
        basic_type                                                      \
          * restrict scanline;						\
                                                                        \
        scanline=(basic_type *) scanline_buffer;                        \
        if ((read_func)(image, scanline_octets, scanline) !=            \
            scanline_octets)                                            \
          {                                                             \
            status=MagickFail;                                          \
            break;                                                      \
          }                                                             \
                                                                        \
        if (y == 0)                                                     \
          *min = *max = (double) scanline[0];                           \
                                                                        \
        for (x = 0; x < width; x++)                                     \
          {                                                             \
            if (*min > (double) scanline[x])                            \
              *min = (double) scanline[x];                              \
            if (*max < (double) scanline[x])                            \
              *max = (double) scanline[x];                              \
          }                                                             \
      }                                                                 \
  }

MagickPassFail
MagickFindRawImageMinMax(Image *image, EndianType endian,
                         unsigned long width, unsigned long height,StorageType type,
                         unsigned scanline_octets, void *scanline_buffer,
                         double *min, double *max)
{
  magick_off_t
    filepos;

  MagickPassFail
    status;

  *min=0.0;
  *max=1.0;
  status=MagickFail;

  filepos = TellBlob(image);

  if (filepos >= 0)
    {
      status=MagickPass;
      switch (type)
        {
        case CharPixel:
          {
            size_t (*read_func)(Image * image, size_t octets, void *data);

            read_func = ReadBlob;
            
            MagickFindMinMax(status,image,read_func,char,scanline_octets,
                             scanline_buffer,min,max)
              break;
          }
        case ShortPixel:
          {
            size_t (*read_func)(Image * image, size_t octets, magick_uint16_t *data);

            if (endian == LSBEndian)
              read_func = ReadBlobLSBShorts;
            else
              read_func = ReadBlobMSBShorts;

            MagickFindMinMax(status,image,read_func,magick_uint16_t,scanline_octets,
                             scanline_buffer,min,max);
            break;
          }
        case IntegerPixel:
        case LongPixel:
          {
            size_t (*read_func)(Image * image, size_t octets, magick_uint32_t *data);

            if (endian == LSBEndian)
              read_func = ReadBlobLSBLongs;
            else
              read_func = ReadBlobMSBLongs;

            MagickFindMinMax(status,image,read_func,magick_uint32_t,scanline_octets,
                             scanline_buffer,min,max);
            break;
          }
        case FloatPixel:
          {
            size_t (*read_func)(Image * image, size_t octets, float *data);

            if (endian == LSBEndian)
              read_func = ReadBlobLSBFloats;
            else
              read_func = ReadBlobMSBFloats;

            MagickFindMinMax(status,image,read_func,float,scanline_octets,
                             scanline_buffer,min,max);
            break;
          }
        case DoublePixel:
          {
            size_t (*read_func)(Image * image, size_t octets, double *data);

            if (endian == LSBEndian)
              read_func = ReadBlobLSBDoubles;
            else
              read_func = ReadBlobMSBDoubles;

            MagickFindMinMax(status,image,read_func,double,scanline_octets,
                             scanline_buffer,min,max);
            break;
          }
        }

      (void) SeekBlob(image, filepos, SEEK_SET);
    }

  return status;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k G e t Q u a n t u m S a m p l e s P e r P i x e l             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetQuantumSamplesPerPixel() returns the number of discrete samples
%  which will be encoded/decoded per-pixel for the specified QuantumType.
%
%  The format of the MagickGetQuantumSamplesPerPixel method is:
%
%      unsigned int MagickGetQuantumSamplesPerPixel(const QuantumType quantum_type)
%
%  A description of each parameter follows:
%
%    o quantum_type: Quantum type
%
%
*/
MagickExport unsigned int
MagickGetQuantumSamplesPerPixel(const QuantumType quantum_type)
{
  unsigned int
    samples=0;

  switch (quantum_type)
    {
    case UndefinedQuantum:
      samples=0;
      break;
    case IndexQuantum:
      samples=1;
      break;
    case GrayQuantum:
      samples=1;
      break;
    case IndexAlphaQuantum:
      samples=2;
      break;
    case GrayAlphaQuantum:
      samples=2;
      break;
    case RedQuantum:
      samples=1;
      break;
    case CyanQuantum:
      samples=1;
      break;
    case GreenQuantum:
      samples=1;
      break;
    case YellowQuantum:
      samples=1;
      break;
    case BlueQuantum:
      samples=1;
      break;
    case MagentaQuantum:
      samples=1;
      break;
    case AlphaQuantum:
      samples=1;
      break;
    case BlackQuantum:
      samples=1;
      break;
    case RGBQuantum:
      samples=3;
      break;
    case RGBAQuantum:
      samples=4;
      break;
    case CMYKQuantum:
      samples=4;
      break;
    case CMYKAQuantum:
      samples=5;
      break;
    case CIEYQuantum:
      samples=1;
      break;
    case CIEXYZQuantum:
      samples=3;
      break;
    }

  return samples;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   I n i t i a l i z e C o n s t i t u t e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method InitializeConstitute initializes the constitute facility
%
%  The format of the InitializeConstitute method is:
%
%      MagickPassFail InitializeConstitute(void)
%
%
*/
MagickPassFail
InitializeConstitute(void)
{
  assert(constitute_semaphore == (SemaphoreInfo *) NULL);
  constitute_semaphore=AllocateSemaphoreInfo();
  return MagickPass;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i n g I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PingImage() returns all the attributes of an image or image sequence
%  except for the pixels.  It is much faster and consumes far less memory
%  than ReadImage().  On failure, a NULL image is returned and exception
%  describes the reason for the failure.
%
%  The format of the PingImage method is:
%
%      Image *PingImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: Ping the image defined by the file or filename members of
%      this structure.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *PingImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *image;

  ImageInfo
    *clone_info;

  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  /* SetExceptionInfo(exception,UndefinedException); */
  clone_info=CloneImageInfo(image_info);
  clone_info->ping=True;
  image=ReadImage(clone_info,exception);
  DestroyImageInfo(clone_info);
  /*
    Intentionally restart timer if ping is requested since timing ping
    is meaningless and misleading.
  */
  if (image != (Image *) NULL)
    GetTimerInfo(&image->timer);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReadImage() reads an image or image sequence from a file or file handle.
%  The method returns a NULL if there is a memory shortage or if the image
%  cannot be read.  On failure, a NULL image is returned and exception
%  describes the reason for the failure.
%
%  The format of the ReadImage method is:
%
%      Image *ReadImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: Read the image defined by the file or filename members of
%      this structure.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static void RemoveTemporaryInputFile(ImageInfo *image_info)
{
  int
    filename_length;

  /*
    Remove normal file name.
  */
  if(!LiberateTemporaryFile(image_info->filename))
    (void) remove(image_info->filename);

  /*
    Remove a .cache file corresponding to a .mpc file.
    This stupidity is necessary because MPC "files" are comprised of two
    separate files.
  */
  filename_length=strlen(image_info->filename);
  if ((filename_length > 4) &&
      (LocaleCompare(image_info->filename+filename_length-4,".mpc") == 0))
    {
      char remove_name[MaxTextExtent];
      (void) strcpy(remove_name,image_info->filename);
      remove_name[filename_length-4]=0;
      (void) strcat(remove_name,".cache");
      (void) printf("removing %s\n", remove_name);
      (void) remove(remove_name);
    }
  else if (LocaleCompare(image_info->magick,"mpc") == 0)
    {
      char remove_name[MaxTextExtent];
      (void) strcpy(remove_name,image_info->filename);
      (void) strcat(remove_name,".cache");
      (void) printf("removing %s\n", remove_name);
      (void) remove(remove_name);
    }

  errno=0;
}

MagickExport Image *ReadImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  char
    filename[MaxTextExtent],
    magick[MaxTextExtent];

  const DelegateInfo
    *delegate_info;

  const MagickInfo
    *magick_info;

  Image
    *image,
    *next;

  ImageInfo
    *clone_info;

  /*
    Determine image type from filename prefix or suffix (e.g. image.jpg).
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image_info->filename != (char *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  /* SetExceptionInfo(exception,UndefinedException); */
  if (*image_info->filename == '@')
    return(ReadImages(image_info,exception));
  clone_info=CloneImageInfo(image_info);

  /*
    Obtain file magick from filename
  */
  (void) SetImageInfo(clone_info,SETMAGICK_READ,exception);
  (void) LogMagickEvent(BlobEvent,GetMagickModule(),
                        "Magick=%s, Filename=%s", clone_info->magick,clone_info->filename);
  (void) strlcpy(filename,clone_info->filename,MaxTextExtent);
  (void) strlcpy(magick,clone_info->magick,MaxTextExtent);
  /*
    Call appropriate image reader based on image type.
  */
  {
    ExceptionInfo
      module_exception,
      delegate_exception;
    
    GetExceptionInfo(&module_exception);
    GetExceptionInfo(&delegate_exception);
    magick_info=GetMagickInfo(clone_info->magick,&module_exception);
    delegate_info=(const DelegateInfo *) NULL;
    if ((magick_info == (const MagickInfo *) NULL) ||
        (magick_info->decoder == NULL))
      delegate_info=GetDelegateInfo(clone_info->magick,(char *) NULL,&delegate_exception);
    
    if (((magick_info == (const MagickInfo *) NULL) ||
         (magick_info->decoder == NULL)) &&
        ((delegate_info == (const DelegateInfo *) NULL) ||
         (delegate_info->decode == NULL)))
      {
        /*
          Module loader ConfigureError errors are intentionally
          ignored here in order to provide the user with familiar "no
          delegate" error messages.  This may be re-considered later.
        */
        if ((module_exception.severity != UndefinedException) &&
            (module_exception.severity != ConfigureError))
          CopyException(exception,&module_exception);
        else if (delegate_exception.severity != UndefinedException)
          CopyException(exception,&delegate_exception);
        else
          {
            /*
              Try to choose a useful error type
            */
            if (clone_info->filename[0] == 0)
              {
                errno=0;
                ThrowException(exception,MissingDelegateError,
                               NoDecodeDelegateForThisImageFormat,clone_info->magick);
              }
            else if (IsAccessibleAndNotEmpty(clone_info->filename))
              {
                errno=0;
                ThrowException(exception,MissingDelegateError,
                               NoDecodeDelegateForThisImageFormat,clone_info->filename);
              }
            else
              {
              ThrowException(exception,FileOpenError,UnableToOpenFile,
                             clone_info->filename);
              }
          }
        DestroyExceptionInfo(&module_exception);
        DestroyExceptionInfo(&delegate_exception);
        if (clone_info->temporary)
          RemoveTemporaryInputFile(clone_info);
        DestroyImageInfo(clone_info);
        return((Image *) NULL);
      }
    
    DestroyExceptionInfo(&module_exception);
    DestroyExceptionInfo(&delegate_exception);
  }

  if ((magick_info != (const MagickInfo *) NULL) &&
      (magick_info->seekable_stream == MagickTrue))
    {
      unsigned int
        status;

      image=AllocateImage(clone_info);
      if (image == (Image *) NULL)
        {
          DestroyImageInfo(clone_info);
          return((Image *) NULL);
        }
      (void) strlcpy(image->filename,clone_info->filename,MaxTextExtent);
      status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
      if (status == False)
        {
          DestroyImageInfo(clone_info);
          DestroyImage(image);
          return((Image *) NULL);
        }
      if (!BlobIsSeekable(image))
        {
          /*
            Coder requires a random access stream.
          */
          if(!AcquireTemporaryFileName(clone_info->filename))
            {
              ThrowException(exception,FileOpenError,
                UnableToCreateTemporaryFile,clone_info->filename);
              CloseBlob(image);
              DestroyImageInfo(clone_info);
              DestroyImage(image);
              return((Image *) NULL);
            }
          (void) ImageToFile(image,clone_info->filename,exception);
          clone_info->temporary=True;
        }
      CloseBlob(image);
      DestroyImage(image);
    }
  image=(Image *) NULL;
  if ((magick_info != (const MagickInfo *) NULL) &&
      (magick_info->decoder != NULL))
    {
      if (!magick_info->thread_support)
        LockSemaphoreInfo(constitute_semaphore);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "Invoking \"%.1024s\" decoder (%.1024s) subimage=%lu subrange=%lu",
			    magick_info->name,
			    magick_info->description,
			    clone_info->subimage,
			    clone_info->subrange);
      image=(magick_info->decoder)(clone_info,exception);
      if (!magick_info->thread_support)
        UnlockSemaphoreInfo(constitute_semaphore);

      if (image != (Image *) NULL)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "Returned from \"%.1024s\" decoder: cache=%s monochrome=%s grayscale=%s class=%s colorspace=%s",
                              magick_info->name,
			      (GetPixelCachePresent(image) ? "present" : "missing"),
                              MagickBoolToString(image->is_monochrome),
                              MagickBoolToString(image->is_grayscale),
                              ClassTypeToString(image->storage_class),
                              ColorspaceTypeToString(image->colorspace));
      else
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "Returned from \"%.1024s\" decoder, returned image is NULL!",
                              magick_info->name);

      /*
        Deal with errors in the image which were not properly reported
        to exception.  If there is an exception at error level, then
        destroy image so that bad image is not consumed by user.
      */
      if (image != (Image *) NULL)
        {
          GetImageException(image,exception);
          if (exception->severity >= ErrorException)
            {
              DestroyImageList(image);
              image=(Image *) NULL;
            }
        }
    }
  else
    {
      if (delegate_info == (const DelegateInfo *) NULL)
        {
          if (clone_info->temporary)
            RemoveTemporaryInputFile(clone_info);
          DestroyImageInfo(clone_info);
          return((Image *) NULL);
        }
      /*
        Let our decoding delegate process the image.
      */
      image=AllocateImage(clone_info);
      if (image == (Image *) NULL)
        {
          DestroyImageInfo(clone_info);
          return((Image *) NULL);
        }
      (void) strlcpy(image->filename,clone_info->filename,MaxTextExtent);
      if(!AcquireTemporaryFileName(clone_info->filename))
        {
          ThrowException(exception,FileOpenError,UnableToCreateTemporaryFile,
            clone_info->filename);
          DestroyImageInfo(clone_info);
          return((Image *) NULL);
        }
      (void) InvokeDelegate(clone_info,image,clone_info->magick,(char *) NULL,
        exception);
      DestroyImageList(image);
      image=(Image *) NULL;
      clone_info->temporary=True;
      (void) SetImageInfo(clone_info,SETMAGICK_READ,exception);
      magick_info=GetMagickInfo(clone_info->magick,exception);
      /*
        If there is no magick info entry for this format, or there is
        no decoder for the format, or an error is reported, then
        attempt to return a reasonable error report.
      */
      if ((magick_info == (const MagickInfo *) NULL) ||
          (magick_info->decoder == NULL) ||
          (exception->severity != UndefinedException))
        {
          if (exception->severity == UndefinedException)
          {
            if (IsAccessibleAndNotEmpty(clone_info->filename))
              ThrowException(exception,MissingDelegateError,
                NoDecodeDelegateForThisImageFormat,clone_info->filename);
            else
              ThrowException(exception,FileOpenError,UnableToOpenFile,
                clone_info->filename);
          }
          if (clone_info->temporary)
            RemoveTemporaryInputFile(clone_info);
          DestroyImageInfo(clone_info);
          return((Image *) NULL);
        }
      /*
        Invoke decoder for format
      */
      if (!magick_info->thread_support)
        LockSemaphoreInfo(constitute_semaphore);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "Invoking \"%.1024s\" decoder (%.1024s) subimage=%lu subrange=%lu",
			    magick_info->name,
			    magick_info->description,
			    clone_info->subimage,
			    clone_info->subrange);
      image=(magick_info->decoder)(clone_info,exception);
      if (!magick_info->thread_support)
        UnlockSemaphoreInfo(constitute_semaphore);

      if (image != (Image *) NULL)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "Returned from \"%.1024s\" decoder: cache=%s monochrome=%s grayscale=%s class=%s colorspace=%s",
                              magick_info->name,
			      (GetPixelCachePresent(image) ? "present" : "missing"),
                              MagickBoolToString(image->is_monochrome),
                              MagickBoolToString(image->is_grayscale),
                              ClassTypeToString(image->storage_class),
                              ColorspaceTypeToString(image->colorspace));
      else
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "Returned from \"%.1024s\" decoder: returned image is NULL!",
                              magick_info->name);

      /*
        Deal with errors in the image which were not properly reported
        to exception.  If there is an exception at error level, then
        destroy image so that bad image is not consumed by user.
      */
      if (image != (Image *) NULL)
        {
          GetImageException(image,exception);
          if (exception->severity >= ErrorException)
            {
              DestroyImageList(image);
              image=(Image *) NULL;
            }
        }

      /*
        Restore original input file magick in case read is from a
        temporary file prepared by an external delegate.  The user
        will expect that the format reported is that of the input
        file.
      */
      if (image != (Image *) NULL)
        (void) strlcpy(image->magick,magick,MaxTextExtent);
    }
  if (clone_info->temporary)
    {
      RemoveTemporaryInputFile(clone_info);
      clone_info->temporary=False;
      if (image != (Image *) NULL)
        (void) strlcpy(image->filename,filename,MaxTextExtent);
    }
  if (image == (Image *) NULL)
    {
      DestroyImageInfo(clone_info);
      return(image);
    }
  if (GetBlobTemporary(image))
    RemoveTemporaryInputFile(clone_info);
  if ((image->next != (Image *) NULL) && IsSubimage(clone_info->tile,False))
    {
      char
        *q;

      Image
        *clone_image,
        *subimages;

      long
        quantum;

      register char
        *p;

      register long
        i;

      unsigned long
        first,
        last;

      /*
        User specified subimages (e.g. image.miff[1,3-5,7-6,2]).
      */
      subimages=NewImageList();
      p=clone_info->tile;
      for (q=p; *q != '\0'; p++)
      {
        while (isspace((int) *p) || (*p == ','))
          p++;
        first=strtol(p,&q,10);
        last=first;
        while (isspace((int) *q))
          q++;
        if (*q == '-')
          last=strtol(q+1,&q,10);
        quantum=first > last ? -1 : 1;
        for (p=q; first != (last+quantum); first+=quantum)
        {
          i=0;
          for (next=image; next != (Image *) NULL; next=next->next)
          {
            if (next->scene != 0)
              i=(long) next->scene;
            if (i != (long) first)
              {
                i++;
                continue;
              }
            clone_image=CloneImage(next,0,0,True,exception);
            if (clone_image == (Image *) NULL)
              break;
            AppendImageToList(&subimages,clone_image);
            i++;
          }
        }
      }
      if (subimages == (Image *) NULL)
        ThrowException(exception,OptionError,
                       SubimageSpecificationReturnsNoImages,
                       clone_info->filename);
      else
        {
          while (subimages->previous != (Image *) NULL)
            subimages=subimages->previous;
          DestroyImageList(image);
          image=subimages;
        }
    }
#if 0
  /*
    Following error-check seems bogus.  Coder should not return an
    image if it was not read correctly.
  */
  if (GetBlobStatus(image))
    {
      ThrowException(exception,CorruptImageError,
                     AnErrorHasOccurredReadingFromFile,
                     clone_info->filename);
      DestroyImageList(image);
      DestroyImageInfo(clone_info);
      return((Image *) NULL);
    }
#endif
  for (next=image; next; next=next->next)
  {
#if 0
    /*
      Apply user-requested colorspace setting.  This allows the user
      to override the default colorspace for the image type.
      FIXME: Does not work yet.
    */
    {
      const ImageAttribute
        *attribute;
      
/*       if ((image_info->attributes) && */
/*           (attribute=GetImageAttribute(image_info->attributes,"colorspace-override"))) */
      if ((attribute=GetImageAttribute(image,"colorspace-override")))
        {
          next->colorspace = StringToColorspaceType(attribute->value);
          (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                                "Colorspace explicitly set to %s",
                                ColorspaceTypeToString(image->colorspace));
        }
    }
#endif
    if (next->storage_class == PseudoClass)
      {
        /*
          Check and cache monochrome and grayscale status
        */
        (void) IsMonochromeImage(next,exception);
        if (next->is_monochrome)
          next->is_grayscale=True;
        else
          (void) IsGrayImage(next,exception);
      }
    next->taint=False;
    (void) strlcpy(next->magick_filename,filename,MaxTextExtent);
    if (GetBlobTemporary(image))
      (void) strlcpy(next->filename,filename,MaxTextExtent);
    if (next->magick_columns == 0)
      next->magick_columns=next->columns;
    if (next->magick_rows == 0)
      next->magick_rows=next->rows;
  }
  DestroyImageInfo(clone_info);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   R e a d I m a g e s                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReadImages() reads a list of image names from a file and then returns the
%  images as a linked list.
%
%  The format of the ReadImage method is:
%
%      Image *ReadImages(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: Method ReadImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: The list of filenames are defined in the filename member of
%      this structure.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static Image *ReadImages(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    *command,
    **images;

  Image
    *image;

  ImageInfo
    *clone_info;

  int
    number_images;

  register Image
    *next;

  register long
    i;

  size_t
    length;

  /*
    Read image list from a file.
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  command=(char *) FileToBlob(image_info->filename+1,&length,exception);
  if (command == (char *) NULL)
    return((Image *) NULL);
  Strip(command);
  images=StringToArgv(command,&number_images);
  if (images == (char **) NULL)
    return((Image *) NULL);
  MagickFreeMemory(command);
  /*
    Read the images into a linked list.
  */
  image=(Image *) NULL;
  clone_info=CloneImageInfo(image_info);
  for (i=1; i < number_images; i++)
  {
    (void) strlcpy(clone_info->filename,images[i],MaxTextExtent);
    next=ReadImage(clone_info,exception);
    if (next == (Image *) NULL)
      continue;
    if (image == (Image *) NULL)
      image=next;
    else
      {
        register Image
          *q;

        /*
          Link image into image list.
        */
        for (q=image; q->next != (Image *) NULL; q=q->next);
        next->previous=q;
        q->next=next;
      }
  }
  DestroyImageInfo(clone_info);
  for (i=1; i < number_images; i++)
    MagickFreeMemory(images[i]);
  MagickFreeMemory(images);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d I n l i n e I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReadInlineImage() reads a Base64-encoded inline image or image sequence.
%  The method returns a NULL if there is a memory shortage or if the image
%  cannot be read.  On failure, a NULL image is returned and exception
%  describes the reason for the failure.
%
%  The format of the ReadInlineImage method is:
%
%      Image *ReadInlineImage(const ImageInfo *image_info,const char *content,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o content: The image encoded in Base64.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *ReadInlineImage(const ImageInfo *image_info,
  const char *content,ExceptionInfo *exception)
{
  Image
    *image;

  MonitorHandler
    handler;

  unsigned char
    *blob;

  size_t
    length;

  register const char
    *p;

  /* SetExceptionInfo(exception,UndefinedException); */
  image=(Image *) NULL;
  for (p=content; (*p != ',') && (*p != '\0'); p++);
  if (*p == '\0')
    ThrowReaderException(CorruptImageError,CorruptImage,image);
  p++;
  blob=Base64Decode(p,&length);
  if (length == 0)
    ThrowReaderException(CorruptImageError,CorruptImage,image);
  handler=SetMonitorHandler((MonitorHandler) NULL);
  image=BlobToImage(image_info,blob,length,exception);
  (void) SetMonitorHandler(handler);
  MagickFreeMemory(blob);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Use WriteImage() to write an image or an image sequence to a file or
%  filehandle.  If writing to a file on disk, the name is defined by the
%  filename member of the image structure.  Write() returns 0 is there is a
%  memory shortage or if the image cannot be written.  Check the exception
%  member of image to determine the cause for any failure.
%
%  The format of the WriteImage method is:
%
%      unsigned int WriteImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o image: The image.
%
%
*/
MagickExport unsigned int WriteImage(const ImageInfo *image_info,Image *image)
{
  const DelegateInfo
    *delegate_info;

  const MagickInfo
    *magick_info;

  ImageInfo
    *clone_info;

  unsigned int
    status;

  /*
    Determine image type from filename prefix or suffix (e.g. image.jpg).
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image_info->filename != (char *) NULL);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  GetTimerInfo(&image->timer);
  image->logging=IsEventLogging();
  clone_info=CloneImageInfo(image_info);
  (void) strlcpy(clone_info->filename,image->filename,MaxTextExtent);
  (void) strlcpy(clone_info->magick,image->magick,MaxTextExtent);
  (void) SetImageInfo(clone_info,SETMAGICK_WRITE,&image->exception);
  (void) strlcpy(image->filename,clone_info->filename,MaxTextExtent);
  image->dither=image_info->dither;

#if 0
  /*
    This bi-modal delegate code allows short-circuiting GraphicsMagick
    in case the delegates support a direct translation.  For example,
    PDF to PS using Ghostscript.

    This is currently disabled due to potential side-effects.
  */
  if (((image->next == (Image *) NULL) || clone_info->adjoin) &&
      (image->previous == (Image *) NULL) &&
      (clone_info->page == (char *) NULL) && !IsTaintImage(image))
    {
      delegate_info=GetDelegateInfo(image->magick,clone_info->magick,
				    &image->exception);
      if ((delegate_info != (const DelegateInfo *) NULL) &&
          (delegate_info->mode == 0) && IsAccessible(image->magick_filename))
        {
          /*
            Let our bi-modal delegate process the image.
          */
          (void) strlcpy(image->filename,image->magick_filename,
			 MaxTextExtent);
          status=InvokeDelegate(clone_info,image,image->magick,
				clone_info->magick,&image->exception);
          DestroyImageInfo(clone_info);
          return(!status);
        }
    }
#endif

  /*
    Call appropriate image writer based on image type.
  */
  status=False;
  magick_info=GetMagickInfo(clone_info->magick,&image->exception);
  if ((magick_info != (const MagickInfo *) NULL) &&
      (magick_info->encoder != NULL))
    {
      char
	tempfile[MaxTextExtent];

      tempfile[0]='\0';

      if (magick_info->seekable_stream == MagickTrue)
	{
	  /*
	    Divert output to temporary file if coder requires a
	    seekable stream and output is not seekable.
	  */
	  if (OpenBlob(clone_info,image,WriteBinaryBlobMode,&image->exception))
	    {
	      if (!BlobIsSeekable(image))
		{
		  if(!AcquireTemporaryFileName(tempfile))
		    {
		      ThrowException(&image->exception,FileOpenError,
				     UnableToCreateTemporaryFile,image->filename);
		      DestroyImageInfo(clone_info);
		      return(False);
		    }
		  (void) strlcpy(image->filename,tempfile,sizeof(image->filename));
		}
	      else
		{
		  /*
		    OpenBlob may expand image->filename so we need to restore it.
		  */
		  (void) strlcpy(image->filename,clone_info->filename,sizeof(image->filename));
		}
	      CloseBlob(image);
	    }
	}

      if (!magick_info->thread_support)
        LockSemaphoreInfo(constitute_semaphore);
      if (image->logging)
	(void) LogMagickEvent(CoderEvent,GetMagickModule(),
			      "Invoking \"%.1024s\" encoder (%.1024s): "
			      "cache=%s "
			      "adjoin=%s type=%s monochrome=%s grayscale=%s "
			      "class=%s colorspace=%s",
			      magick_info->name,
			      magick_info->description,
			      (GetPixelCachePresent(image) ? "present" : "missing"),
			      MagickBoolToString(clone_info->adjoin),
			      ImageTypeToString(clone_info->type),
			      MagickBoolToString(image->is_monochrome),
			      MagickBoolToString(image->is_grayscale),
			      ClassTypeToString(image->storage_class),
			      ColorspaceTypeToString(image->colorspace));
      status=(magick_info->encoder)(clone_info,image);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
			    "Returned from \"%.1024s\" encoder",magick_info->name);
      if (!magick_info->thread_support)
        UnlockSemaphoreInfo(constitute_semaphore);

      if (tempfile[0] != '\0')
	{
	  /*
	    Send temporary file to stream.
	  */
	  (void) strlcpy(image->filename,clone_info->filename,MaxTextExtent);	  
	  if ((status &= OpenBlob(clone_info,image,WriteBinaryBlobMode,
				  &image->exception)))
	    {
	      status &= WriteBlobFile(image,tempfile);
	      CloseBlob(image);
	    }
	  LiberateTemporaryFile(tempfile);
	}
    }
  else
    {
      delegate_info=GetDelegateInfo((char *) NULL,clone_info->magick,
				    &image->exception);
      if (delegate_info != (DelegateInfo *) NULL)
        {
          /*
            Let our encoding delegate process the image.
          */
          if(!AcquireTemporaryFileName(image->filename))
            {
              ThrowException(&image->exception,FileOpenError,
			     UnableToCreateTemporaryFile,image->filename);
              DestroyImageInfo(clone_info);
              return(False);
            }
          status=InvokeDelegate(clone_info,image,(char *) NULL,
				clone_info->magick,&image->exception);
          (void) LiberateTemporaryFile(image->filename);
          DestroyImageInfo(clone_info);
          return(!status);
        }
      magick_info=GetMagickInfo(clone_info->magick,&image->exception);
      if (!clone_info->affirm && (magick_info == (const MagickInfo *) NULL))
        magick_info=(MagickInfo *)
          GetMagickInfo(image->magick,&image->exception);
      if ((magick_info == (MagickInfo *) NULL) ||
          (magick_info->encoder == NULL))
        {
          DestroyImageInfo(clone_info);
          ThrowBinaryException(MissingDelegateError,
			       NoEncodeDelegateForThisImageFormat,
			       image->filename)
	    }
      if (!magick_info->thread_support)
        LockSemaphoreInfo(constitute_semaphore);
      status=(magick_info->encoder)(clone_info,image);
      if (!magick_info->thread_support)
        UnlockSemaphoreInfo(constitute_semaphore);
    }
  (void) strlcpy(image->magick,clone_info->magick,MaxTextExtent);
  DestroyImageInfo(clone_info);
  if (GetBlobStatus(image))
    ThrowBinaryException(CorruptImageError,AnErrorHasOccurredWritingToFile,
			 image->filename);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e I m a g e s                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  WriteImages() writes an image sequence into one or more files.  While
%  WriteImage() will also write an image sequence, it is limited to writing
%  the sequence into a single file using a format which supports multiple
%  frames.  WriteImages() does not have that limitation since it will
%  generate multiple output files if necessary (or when requested).  When
%  ImageInfo's adjoin flag is set to MagickFalse, the file name is expected
%  to include a printf-style formatting string for the frame number (e.g.
%  "image%02d.miff") so that the frames may be written.
%
%  The format of the WriteImages method is:
%
%      unsigned int WriteImages(const ImageInfo *image_info,Image *image,
%                               const char *filename,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o images: The image list.
%
%    o filename: The image filename.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport MagickPassFail WriteImages(const ImageInfo *image_info,Image *image,
					const char *filename,ExceptionInfo *exception)
{
  ImageInfo
    *clone_info;

  unsigned int
    status=MagickPass;

  /*
    Write converted images.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  image->logging=IsEventLogging();
  clone_info=CloneImageInfo(image_info);
  if (clone_info)
    {
      register Image
        *p;

      if (filename != (const char *) NULL)
        {
          if (strlcpy(clone_info->filename,filename,MaxTextExtent) >= MaxTextExtent)
            status &= MagickFail;
          /* Set file name in all image frames */
          for (p=image; p != (Image *) NULL; p=p->next)
            if (p->filename != filename)
              if (strlcpy(p->filename,filename,MaxTextExtent) >= MaxTextExtent)
                status &= MagickFail;
        }
      (void) SetImageInfo(clone_info,
			  (SETMAGICK_WRITE |
			   (!clone_info->adjoin ? SETMAGICK_RECTIFY : 0U)),
			  exception);
      for (p=image; p != (Image *) NULL; p=p->next)
        {
          status &= WriteImage(clone_info,p);
          if (p->exception.severity > exception->severity)
            CopyException(exception,&p->exception);
          GetImageException(p,exception);
          if (clone_info->adjoin)
            break;
        }
      if (clone_info->verbose)
        (void) DescribeImage(image,stderr,False);
      DestroyImageInfo(clone_info);
      clone_info=0;
    }
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e I m a g e s F i l e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  WriteImagesFile() writes an image or image sequence to a stdio
%  FILE handle.  This may be used to append an encoded image to an already
%  existing appended image sequence if the file seek position is at the end
%  of an existing file.
%
%  The format of the WriteImagesFile method is:
%
%      unsigned int WriteImagesFile(const ImageInfo *image_info,Image *image,
%                                   FILE *file,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o images: The image list.
%
%    o file: The open (and positioned) file handle.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport MagickPassFail WriteImagesFile(const ImageInfo *image_info,Image *image,
					    FILE * file ,ExceptionInfo *exception)
{
  ImageInfo
    *clone_info;

  unsigned int
    status=MagickPass;

  /*
    Write converted images.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  image->logging=IsEventLogging();
  clone_info=CloneImageInfo(image_info);
  if (clone_info)
    {
      register Image
        *p;

      clone_info->file=file;
      (void) SetImageInfo(clone_info,
			  (SETMAGICK_WRITE |
			   (!clone_info->adjoin ? SETMAGICK_RECTIFY : 0U)),
			  exception);
      for (p=image; p != (Image *) NULL; p=p->next)
        {
          status &= WriteImage(clone_info,p);
          if(p->exception.severity > exception->severity)
            CopyException(exception,&p->exception);
          GetImageException(p,exception);
          if (clone_info->adjoin)
            break;
        }
      if (clone_info->verbose)
        (void) DescribeImage(image,stderr,False);
      clone_info->file=0;
      DestroyImageInfo(clone_info);
      clone_info=0;
    }
  return(status);
}
