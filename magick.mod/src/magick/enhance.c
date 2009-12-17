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
%              EEEEE  N   N  H   H   AAA   N   N   CCCC  EEEEE                %
%              E      NN  N  H   H  A   A  NN  N  C      E                    %
%              EEE    N N N  HHHHH  AAAAA  N N N  C      EEE                  %
%              E      N  NN  H   H  A   A  N  NN  C      E                    %
%              EEEEE  N   N  H   H  A   A  N   N   CCCC  EEEEE                %
%                                                                             %
%                                                                             %
%                  GraphicsMagick Image Enhancement Methods                   %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
%                                 Re-Written                                  %
%                              Bob Friesenhahn                                %
%                                  May 2008                                   %
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
#include "magick/enhance.h"
#include "magick/gem.h"
#include "magick/pixel_iterator.h"
#include "magick/monitor.h"
#include "magick/utility.h"

static MagickPassFail
BuildChannelHistogramsCB(void *mutable_data,          /* User provided mutable data */
                         const void *immutable_data,  /* User provided immutable data */
                         const Image *const_image,    /* Input image */
                         const PixelPacket *pixels,   /* Pixel row */
                         const IndexPacket *indexes,  /* Pixel indexes */
                         const long npixels,          /* Number of pixels in row */
                         ExceptionInfo *exception     /* Exception report */
                         )
{
  /*
    Built an image histogram in the supplied table.

    Should be executed by just one thread in order to avoid contention
    for the histogram table.
  */
  DoublePixelPacket
    *histogram = (DoublePixelPacket *) mutable_data;

  register long
    i;

  ARG_NOT_USED(immutable_data);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  for (i=0; i < npixels; i++)
    {
      histogram[ScaleQuantumToMap(pixels[i].red)].red++;
      histogram[ScaleQuantumToMap(pixels[i].green)].green++;
      histogram[ScaleQuantumToMap(pixels[i].blue)].blue++;
      if (const_image->matte)
        histogram[ScaleQuantumToMap(pixels[i].opacity)].opacity++;
    }

  return MagickPass;
}

static DoublePixelPacket *
BuildChannelHistograms(const Image *image, ExceptionInfo *exception)
{
  MagickPassFail
    status = MagickPass;

  DoublePixelPacket
    *histogram;
  
  histogram=MagickAllocateArray(DoublePixelPacket *,(MaxMap+1),
                                sizeof(DoublePixelPacket));
  if (histogram == (DoublePixelPacket *) NULL)
    {
      ThrowException(exception,ResourceLimitError,MemoryAllocationFailed,
                     image->filename);
      return (DoublePixelPacket *) NULL;
    }

  (void) memset(histogram,0,(MaxMap+1)*sizeof(DoublePixelPacket));

  {
    PixelIteratorOptions
      iterator_options;
    
    InitializePixelIteratorOptions(&iterator_options,exception);
    iterator_options.max_threads=1;
    status=PixelIterateMonoRead(BuildChannelHistogramsCB,
                                &iterator_options,
                                "[%s] Building histogram...",
                                histogram,NULL,
                                0,0,image->columns,image->rows,
                                image,exception);
  }

  if (status == MagickFail)
    MagickFreeMemory(histogram);
  
  return histogram;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     C o n t r a s t I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ContrastImage() enhances the intensity differences between the lighter and
%  darker elements of the image.  Set sharpen to a value other than 0 to
%  increase the image contrast otherwise the contrast is reduced.
%
%  The format of the ContrastImage method is:
%
%      unsigned int ContrastImage(Image *image,const unsigned int sharpen)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o sharpen: Increase or decrease image contrast.
%
%
*/
static MagickPassFail
ContrastImagePixels(void *mutable_data,         /* User provided mutable data */
                    const void *immutable_data, /* User provided immutable data */
                    Image *image,               /* Modify image */
                    PixelPacket *pixels,        /* Pixel row */
                    IndexPacket *indexes,       /* Pixel row indexes */
                    const long npixels,         /* Number of pixels in row */
                    ExceptionInfo *exception)   /* Exception report */
{
  /*
    Modulate pixels contrast.
  */
  const double
    sign = *((const double *) immutable_data);

  static const double
    alpha=0.5+MagickEpsilon;
      
  register long
    i;
  
  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  for (i=0; i < npixels; i++)
    {
      double
        brightness,
        hue,
        saturation;

      TransformHSL(pixels[i].red,pixels[i].green,pixels[i].blue,
                   &hue,&saturation,&brightness);
      brightness+=
        alpha*sign*(alpha*(sin(MagickPI*(brightness-alpha))+1.0)-brightness);
      if (brightness > 1.0)
        brightness=1.0;
      if (brightness < 0.0)
        brightness=0.0;
      HSLTransform(hue,saturation,brightness,&pixels[i].red,
                   &pixels[i].green,&pixels[i].blue);
    }

  return MagickPass;
}
#define DullContrastImageText "[%s] Dulling contrast..."
#define SharpenContrastImageText "[%s] Sharpening contrast..."
MagickExport MagickPassFail ContrastImage(Image *image,const unsigned int sharpen)
{
  double
    sign;

  const char
    *progress_message;

  unsigned int
    is_grayscale;

  MagickPassFail
    status=MagickPass;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  is_grayscale=image->is_grayscale;
  sign=sharpen ? 1.0 : -1.0;
  progress_message=sharpen ? SharpenContrastImageText : DullContrastImageText;
  if (image->storage_class == PseudoClass)
    {
      (void) ContrastImagePixels(NULL,&sign,image,image->colormap,
                                 (IndexPacket *) NULL,image->colors,
                                 &image->exception);
      status=SyncImage(image);
    }
  else
    {
      status=PixelIterateMonoModify(ContrastImagePixels,NULL,
                                    progress_message,
                                    NULL,&sign,0,0,image->columns,image->rows,
                                    image,&image->exception);
    }
  image->is_grayscale=is_grayscale;
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     E q u a l i z e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  EqualizeImage() applies a histogram equalization to the image.
%
%  The format of the EqualizeImage method is:
%
%      unsigned int EqualizeImage(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
*/
typedef struct _ApplyLevelsOptions_t
{
  PixelPacket
    *map;

  MagickBool
    level_red,
    level_green,
    level_blue,
    level_opacity;
} ApplyLevels_t;

static MagickPassFail
ApplyLevels(void *mutable_data,          /* User provided mutable data */
            const void *immutable_data,  /* User provided immutable data */
            Image *image,                /* Modify image */
            PixelPacket *pixels,         /* Pixel row */
            IndexPacket *indexes,        /* Pixel row indexes */
            const long npixels,          /* Number of pixels in row */
            ExceptionInfo *exception)    /* Exception report */
{
  /*
    Apply a levels transformation based on a supplied look-up table.
  */
  const ApplyLevels_t
    *options = (const ApplyLevels_t *) immutable_data;

  register long
    i;

  PixelPacket
    *map=options->map;

  MagickBool
    level_red=options->level_red,
    level_green=options->level_green,
    level_blue=options->level_blue,
    level_opacity=options->level_opacity;
  
  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  for (i=0; i < npixels; i++)
    {
      if (level_red)
        pixels[i].red=map[ScaleQuantumToMap(pixels[i].red)].red;
      if (level_green)
        pixels[i].green=map[ScaleQuantumToMap(pixels[i].green)].green;
      if (level_blue)
        pixels[i].blue=map[ScaleQuantumToMap(pixels[i].blue)].blue;
      if (level_opacity)
        pixels[i].opacity=map[ScaleQuantumToMap(pixels[i].opacity)].opacity;
    }
  return MagickPass;
}

#define EqualizeImageText "Equalize...  "
MagickExport MagickPassFail EqualizeImage(Image *image)
{
  DoublePixelPacket
    high,
    *histogram,
    intensity,
    low,
    *map;

  ApplyLevels_t
    levels;

  register long
    i;

  unsigned int
    is_grayscale;

  MagickPassFail
    status=MagickPass;

  /*
    Allocate and initialize arrays.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  is_grayscale=image->is_grayscale;
  map=MagickAllocateMemory(DoublePixelPacket *,(MaxMap+1)*sizeof(DoublePixelPacket));
  levels.map=MagickAllocateMemory(PixelPacket *,(MaxMap+1)*sizeof(PixelPacket));
  if ((map == (DoublePixelPacket *) NULL) ||
      (levels.map == (PixelPacket *) NULL))
    {
      MagickFreeMemory(map);
      MagickFreeMemory(levels.map);
      ThrowBinaryException(ResourceLimitError,MemoryAllocationFailed,
                           MagickMsg(OptionError,UnableToEqualizeImage));
    }
  /*
    Build histogram.
  */
  histogram=BuildChannelHistograms(image,&image->exception);
  if (histogram == (DoublePixelPacket *) NULL)
    {
      MagickFreeMemory(map);
      MagickFreeMemory(levels.map);
      return MagickFail;
    }
  /*
    Integrate the histogram to get the equalization map.
  */
  (void) memset(&intensity,0,sizeof(DoublePixelPacket));  
  for (i=0; i <= (long) MaxMap; i++)
    {
      intensity.red+=histogram[i].red;
      intensity.green+=histogram[i].green;
      intensity.blue+=histogram[i].blue;
      if (image->matte)
        intensity.opacity+=histogram[i].opacity;
      map[i]=intensity;
    }
  low=map[0];
  high=map[MaxMap];
  (void) memset(levels.map,0,(MaxMap+1)*sizeof(PixelPacket));
  levels.level_red = (low.red != high.red);
  levels.level_green = (low.green != high.green);
  levels.level_blue = (low.blue != high.blue);
  levels.level_opacity= (image->matte && (low.opacity != high.opacity));
  for (i=0; i <= (long) MaxMap; i++)
  {
    if (levels.level_red)
      levels.map[i].red=ScaleMapToQuantum(
        (MaxMap*(map[i].red-low.red))/(high.red-low.red));
    if (levels.level_green)
      levels.map[i].green=ScaleMapToQuantum(
        (MaxMap*(map[i].green-low.green))/(high.green-low.green));
    if (levels.level_blue)
      levels.map[i].blue=ScaleMapToQuantum(
        (MaxMap*(map[i].blue-low.blue))/(high.blue-low.blue));
    if (levels.level_opacity)
      levels.map[i].opacity=ScaleMapToQuantum(
        (MaxMap*(map[i].opacity-low.opacity))/(high.opacity-low.opacity));
  }
  MagickFreeMemory(histogram);
  MagickFreeMemory(map);
  /*
    Stretch the histogram based on the map.
  */
  if (image->storage_class == PseudoClass)
    {
      (void) ApplyLevels(NULL,&levels,image,image->colormap,
                         (IndexPacket *) NULL,image->colors,
                         &image->exception);
      status=SyncImage(image);
    }
  else
    {
      status=PixelIterateMonoModify(ApplyLevels,
                                    NULL,
                                    "[%s] Applying histogram equalization...",
                                    NULL,&levels,
                                    0,0,image->columns,image->rows,
                                    image,
                                    &image->exception);
    }
  MagickFreeMemory(levels.map);
  image->is_grayscale=is_grayscale;
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     G a m m a I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Use GammaImage() to gamma-correct an image.  The same image viewed on
%  different devices will have perceptual differences in the way the
%  image's intensities are represented on the screen.  Specify individual
%  gamma levels for the red, green, and blue channels (e.g. "1.0,2.2,0.45"),
%  or adjust all three with a single gamma parameter.  Values typically range
%  from 0.45 to 2.6.
%
%  You can also reduce the influence of a particular channel with a gamma
%  value of 0.
%
%  The format of the GammaImage method is:
%
%      MagickPassFail GammaImage(Image *image,const char *level)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o level: Define the level of gamma correction.
%
%
*/
#if MaxMap == MaxRGB
typedef struct _ApplyLevelsDiscrete_t
{
  Quantum
    * restrict color,    /* red, green, & blue */
    * restrict red,      /* red */
    * restrict green,    /* green */
    * restrict blue,     /* blue */
    * restrict opacity;  /* Opacity */
} ApplyLevelsDiscrete_t;

static MagickPassFail
ApplyLevelsDiscrete(void *mutable_data,             /* User provided mutable data */
		    const void *immutable_data,     /* User provided immutable data */
		    Image *image,                   /* Modify image */
		    PixelPacket * restrict pixels,  /* Pixel row */
		    IndexPacket * restrict indexes, /* Pixel row indexes */
		    const long npixels,             /* Number of pixels in row */
		    ExceptionInfo *exception)       /* Exception report */
{
  /*
    Apply a levels transformation based on a supplied look-up table.
  */
  const ApplyLevelsDiscrete_t
    levels = *(const ApplyLevelsDiscrete_t *) immutable_data;

  register long
    i;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  for (i=0; i < npixels; i++)
    {
      if (levels.color)
	{
	  pixels[i].red=levels.color[ScaleQuantumToMap(pixels[i].red)];
	  pixels[i].green=levels.color[ScaleQuantumToMap(pixels[i].green)];
	  pixels[i].blue=levels.color[ScaleQuantumToMap(pixels[i].blue)];
	}
      else
	{
	  if (levels.red)
	    pixels[i].red=levels.red[ScaleQuantumToMap(pixels[i].red)];
	  if (levels.green)
	    pixels[i].green=levels.green[ScaleQuantumToMap(pixels[i].green)];
	  if (levels.blue)
	    pixels[i].blue=levels.blue[ScaleQuantumToMap(pixels[i].blue)];
	}
      if (levels.opacity)
        pixels[i].opacity=levels.opacity[ScaleQuantumToMap(pixels[i].opacity)];
    }

  return MagickPass;
}
#endif /* if MaxMap == MaxRGB */

/*
  Gamma correct value in range of 0.0 to 1.0
*/
static inline double
GammaCorrect(const double value, const double gamma)
{
  return pow(value,1.0/gamma);
}

#if MaxMap != MaxRGB
typedef DoublePixelPacket GammaCorrectPixelsOptions_t;

static MagickPassFail
GammaCorrectPixels(void *mutable_data,             /* User provided mutable data */
		   const void *immutable_data,     /* User provided immutable data */
		   Image *image,                   /* Modify image */
		   PixelPacket * restrict pixels,  /* Pixel row */
		   IndexPacket * restrict indexes, /* Pixel row indexes */
		   const long npixels,             /* Number of pixels in row */
		   ExceptionInfo *exception)       /* Exception report */
{
  /*
    Apply a gamma transformation based on slow accurate math.
  */
  const GammaCorrectPixelsOptions_t
    options = *(const GammaCorrectPixelsOptions_t *) immutable_data;

  register long
    i;

  MagickBool
    red_flag,
    green_flag,
    blue_flag;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  red_flag=(options.red != 1.0);
  green_flag=(options.green != 1.0);
  blue_flag=(options.blue != 1.0);

  for (i=0; i < npixels; i++)
    {
      double
	value;

      if (red_flag)
	{
	  value=MaxRGBDouble*GammaCorrect(pixels[i].red/MaxRGBDouble,options.red);
	  pixels[i].red=RoundDoubleToQuantum(value);
	}
      if (green_flag)
	{
	  value=MaxRGBDouble*GammaCorrect(pixels[i].green/MaxRGBDouble,options.green);
	  pixels[i].green=RoundDoubleToQuantum(value);
	}
      if (blue_flag)
	{
	  value=MaxRGBDouble*GammaCorrect(pixels[i].blue/MaxRGBDouble,options.blue);
	  pixels[i].blue=RoundDoubleToQuantum(value);
	}
    }

  return MagickPass;
}
#endif /* MaxMap != MaxRGB */

MagickExport MagickPassFail GammaImage(Image *image,const char *level)
{
  double
    gamma_color=0.0,
    gamma_red=1.0,
    gamma_green=1.0,
    gamma_blue=1.0;

  long
    count;

  unsigned int
    is_grayscale;

  MagickBool
    level_color=MagickFalse,
    level_red=MagickFalse,
    level_green=MagickFalse,
    level_blue=MagickFalse,
    unity_gamma;

  MagickPassFail
    status=MagickPass;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (level == (char *) NULL)
    return(MagickFail);
  count=sscanf(level,"%lf%*[,/]%lf%*[,/]%lf",&gamma_red,&gamma_green,
	       &gamma_blue);
  if (count == 1)
    {
      gamma_green=gamma_red;
      gamma_blue=gamma_red;
    }

  unity_gamma=((gamma_red == gamma_green) && (gamma_green == gamma_blue));

  if ((unity_gamma) && (gamma_red != 1.0))
    {
      gamma_color=gamma_red;
      level_color = MagickTrue;
    }
  else
    {
      level_red = ((gamma_red != 0.0) && (gamma_red != 1.0));
      level_green = ((gamma_green != 0.0) && (gamma_green != 1.0));
      level_blue = ((gamma_blue != 0.0) && (gamma_blue != 1.0));
    }

  is_grayscale=((image->is_grayscale) && (unity_gamma));

  if (!level_color && !level_red && !level_green && !level_blue)
    return(MagickPass);

#if MaxMap == MaxRGB
  {
    ApplyLevelsDiscrete_t
      levels;

    register long
      i;

    /*
      Allocate and initialize gamma maps.
    */
    (void) memset(&levels,0,sizeof(levels));
    if (level_color)
      levels.color=MagickAllocateArray(Quantum *,(MaxMap+1),sizeof(Quantum));
    if (level_red)
      levels.red=MagickAllocateArray(Quantum *,(MaxMap+1),sizeof(Quantum));
    if (level_green)
      levels.green=MagickAllocateArray(Quantum *,(MaxMap+1),sizeof(Quantum));
    if (level_blue)
      levels.blue=MagickAllocateArray(Quantum *,(MaxMap+1),sizeof(Quantum));
    if ((level_color && !levels.color) ||
	(level_red && !levels.red) ||
	(level_green && !levels.green) ||
	(level_blue && !levels.blue))
      {
	MagickFreeMemory(levels.color);
	MagickFreeMemory(levels.red);
	MagickFreeMemory(levels.green);
	MagickFreeMemory(levels.blue);
	ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
			      UnableToGammaCorrectImage);
      }
#if (MaxMap > 256) && defined(HAVE_OPENMP)
#  pragma omp parallel for
#endif
    for (i=0; i <= (long) MaxMap; i++)
      {
	if (levels.color)
	  levels.color[i]=
	    ScaleMapToQuantum(MaxMap*GammaCorrect((double) i/MaxMap,gamma_color));
	if (levels.red)
	  levels.red[i]=
	    ScaleMapToQuantum(MaxMap*GammaCorrect((double) i/MaxMap,gamma_red));
	if (levels.green)
	  levels.green[i]=
	    ScaleMapToQuantum(MaxMap*GammaCorrect((double) i/MaxMap,gamma_green));
	if (levels.blue)
	  levels.blue[i]=
	    ScaleMapToQuantum(MaxMap*GammaCorrect((double) i/MaxMap,gamma_blue));
      }
    /*
      Apply gamma.
    */
    if (image->storage_class == PseudoClass)
      {
	(void) ApplyLevelsDiscrete(NULL,&levels,image,image->colormap,
				   (IndexPacket *) NULL,image->colors,
				   &image->exception);
	status=SyncImage(image);
      }
    else
      {
	status=PixelIterateMonoModify(ApplyLevelsDiscrete,
				      NULL,
				      "[%s] Applying gamma correction...",
				      NULL,&levels,
				      0,0,image->columns,image->rows,
				      image,
				      &image->exception);
      }
    MagickFreeMemory(levels.color);
    MagickFreeMemory(levels.red);
    MagickFreeMemory(levels.green);
    MagickFreeMemory(levels.blue);
  }
#else /* if MaxMap == MaxRGB */
  {
    GammaCorrectPixelsOptions_t
      levels;

    levels.red=gamma_red;
    levels.green=gamma_green;
    levels.blue=gamma_blue;
    levels.opacity=OpaqueOpacity;

    /*
      Apply gamma.
    */
    if (image->storage_class == PseudoClass)
      {
	(void) GammaCorrectPixels(NULL,&levels,image,image->colormap,
				   (IndexPacket *) NULL,image->colors,
				   &image->exception);
	status=SyncImage(image);
      }
    else
      {
	status=PixelIterateMonoModify(GammaCorrectPixels,
				      NULL,
				      "[%s] Applying gamma correction...",
				      NULL,&levels,
				      0,0,image->columns,image->rows,
				      image,
				      &image->exception);
      }
  }
  
#endif/* if MaxMap != MaxRGB */

  if (image->gamma != 0.0)
    image->gamma*=(gamma_red+gamma_green+gamma_blue)/3.0;
  image->is_grayscale=is_grayscale;
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     L e v e l I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  LevelImage() adjusts the levels of an image by scaling the colors falling
%  between specified white and black points to the full available quantum
%  range. The parameters provided represent the black, mid (gamma), and white
%  points. The black point specifies the darkest color in the image. Colors
%  darker than the black point are set to zero. Mid point specifies a gamma
%  correction to apply to the image.  White point specifies the lightest color
%  in the image. Colors brighter than the white point are set to the maximum
%  quantum value.
%
%  The format of the LevelImage method is:
%
%      unsigned int LevelImage(Image *image,const char *level)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o levels: Specify the levels as a string of the form "black/mid/white"
%      (e.g. "10,1.0,65000) where black and white have the range of 0-MaxRGB,
%      and mid has the range 0-10.
%
%
*/
#define LevelImageText "Level...  "
MagickExport MagickPassFail LevelImage(Image *image,const char *levels)
{
  double
    black_point,
    mid_point,
    white_point;
  
  MagickPassFail
    status=MagickPass;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(levels != (char *) NULL);

  {
    /*
      Parse levels argument.
    */
    char
      buffer[MaxTextExtent];

    MagickBool
      percent = MagickFalse;

    int
      count;

    register long
      i;

    const char
      *lp;
    
    char
      *cp;

    black_point=0.0;
    mid_point=1.0;
    white_point=MaxRGB;

    cp=buffer;
    lp=levels;
    for (i=sizeof(buffer)-1 ; (*lp != 0) && (i != 0) ; lp++)
      {
        if (*lp == '%')
          {
            percent = MagickTrue;
          }
        else
          {
            *cp++=*lp;
            i--;
          }
      }
    *cp=0;

    count=sscanf(buffer,"%lf%*[,/]%lf%*[,/]%lf",&black_point,&mid_point,
                 &white_point);
    if (percent)
      {
        if (count > 0)
          black_point*=MaxRGB/100.0;
        if (count > 2)
          white_point*=MaxRGB/100.0;
      }
    black_point=ConstrainToQuantum(black_point);
    white_point=ConstrainToQuantum(white_point);
    if (count == 1)
      white_point=MaxRGB-black_point;
  }

  status=LevelImageChannel(image,AllChannels,black_point,mid_point,white_point);

  return status;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     L e v e l I m a g e C h a n n e l                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  LevelImageChannel() adjusts the levels of one or more channels by
%  scaling the colors falling between specified white and black points to
%  the full available quantum range. The parameters provided represent the
%  black, mid (gamma), and white points.  The black point specifies the
%  darkest color in the image. Colors darker than the black point are set to
%  zero. Mid point specifies a gamma correction to apply to the image.
%  White point specifies the lightest color in the image.  Colors brighter
%  than the white point are set to the maximum quantum value.
%
%  The format of the LevelImage method is:
%
%      unsigned int LevelImageChannel(Image *image,const char *level,
%        const ChannelType channel,const double black_point,
%        const double mid_point,const double white_point)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o channel: Identify which channel to level: Red, Cyan, Green, Magenta,
%      Blue, Yellow, Opacity, or All.
%
%    o black_point, mid_point, white_point: Specify the levels where the black
%      and white points have the range of 0-MaxRGB, and mid has the range 0-10.
%
%
*/
MagickExport MagickPassFail LevelImageChannel(Image *image,
  const ChannelType channel,const double black_point,const double mid_point,
  const double white_point)
{
  double
    black,
    value,
    white;

  ApplyLevels_t
    levels;

  unsigned int
    is_grayscale = MagickFalse;

  register long
    i;

  MagickPassFail
    status=MagickPass;

  /*
    Allocate and initialize levels map.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  levels.map=MagickAllocateArray(PixelPacket *,(MaxMap+1),sizeof(PixelPacket));
  if (levels.map == (PixelPacket *) NULL)
    ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
                          UnableToLevelImage);
  /*
    Determine which channels to operate on.
  */
  levels.level_red=MagickFalse;
  levels.level_green=MagickFalse;
  levels.level_blue=MagickFalse;
  levels.level_opacity=MagickFalse;
  switch (channel)
    {
    case RedChannel:
    case CyanChannel:
      levels.level_red=MagickTrue;
        break;
    case GreenChannel:
    case MagentaChannel:
      levels.level_green=MagickTrue;
      break;
    case BlueChannel:
    case YellowChannel:
      levels.level_blue=MagickTrue;
      break;
    case OpacityChannel:
    case BlackChannel:
      levels.level_opacity=MagickTrue;
      break;
    case AllChannels:
      levels.level_red=MagickTrue;
      levels.level_green=MagickTrue;
      levels.level_blue=MagickTrue;
      is_grayscale=image->is_grayscale;
      break;
    default:
      break;
    }    
  /*
    Build leveling map.
  */
  black=ScaleQuantumToMap(black_point);
  white=ScaleQuantumToMap(white_point);
  for (i=0; i <= (long) MaxMap; i++)
    {
      if (i < black)
        {
          levels.map[i].red=levels.map[i].green=levels.map[i].blue=levels.map[i].opacity=0;
          continue;
        }
      if (i > white)
        {
          levels.map[i].red=levels.map[i].green=levels.map[i].blue=levels.map[i].opacity=MaxRGB;
          continue;
        }
      value=MaxRGB*(pow(((double) i-black)/(white-black),1.0/mid_point));
      levels.map[i].red=levels.map[i].green=levels.map[i].blue=levels.map[i].opacity=
        RoundDoubleToQuantum(value);
    }
  /*
    Apply levels
  */
  if (image->storage_class == PseudoClass)
    {
      (void) ApplyLevels(NULL,&levels,image,image->colormap,
                         (IndexPacket *) NULL,image->colors,
                         &image->exception);
      status=SyncImage(image);
    }
  else
    {
      status=PixelIterateMonoModify(ApplyLevels,
                                    NULL,
                                    "[%s] Leveling channels...",
                                    NULL,&levels,
                                    0,0,image->columns,image->rows,
                                    image,
                                    &image->exception);
    }
  MagickFreeMemory(levels.map);

  image->is_grayscale=is_grayscale;

  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     M o d u l a t e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ModulateImage() lets you control the brightness, saturation, and hue
%  of an image.  Modulate represents the brightness, saturation, and hue
%  as one parameter (e.g. 90,150,100).
%
%  The format of the ModulateImage method is:
%
%      unsigned int ModulateImage(Image *image,const char *modulate)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o modulate: Define the percent change in brightness, saturation, and
%      hue.
%
%
*/
typedef struct _ModulateImageParameters_t
{
  double
    percent_brightness,
    percent_hue,
    percent_saturation;
} ModulateImageParameters_t;

static MagickPassFail
ModulateImagePixels(void *mutable_data,         /* User provided mutable data */
                    const void *immutable_data, /* User provided immutable data */
                    Image *image,               /* Modify image */
                    PixelPacket *pixels,        /* Pixel row */
                    IndexPacket *indexes,       /* Pixel row indexes */
                    const long npixels,         /* Number of pixels in row */
                    ExceptionInfo *exception)   /* Exception report */
{
  /*
    Modulate image pixels according to options.
  */
  const ModulateImageParameters_t
    param = *(const ModulateImageParameters_t *) immutable_data;

  register long
    i;
  
  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  for (i=0; i < npixels; i++)
    {
      double
        brightness,
        hue,
        saturation;

      TransformHSL(pixels[i].red,pixels[i].green,pixels[i].blue,
                   &hue,&saturation,&brightness);
      brightness*=(0.01+MagickEpsilon)*param.percent_brightness;
      if (brightness > 1.0)
        brightness=1.0;
      saturation*=(0.01+MagickEpsilon)*param.percent_saturation;
      if (saturation > 1.0)
        saturation=1.0;
      
      hue += (param.percent_hue/200.0 - 0.5);
      while (hue < 0.0)
        hue += 1.0;
      while (hue > 1.0)
        hue -= 1.0;
      HSLTransform(hue,saturation,brightness,
                   &pixels[i].red,&pixels[i].green,&pixels[i].blue);
    }

  return MagickPass;
}

MagickExport MagickPassFail ModulateImage(Image *image,const char *modulate)
{
  char
    progress_message[MaxTextExtent];

  ModulateImageParameters_t
    param;

  unsigned int
    is_grayscale;

  MagickPassFail
    status=MagickPass;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (modulate == (char *) NULL)
    return(MagickFail);
  is_grayscale=image->is_grayscale;
  param.percent_brightness=100.0;
  param.percent_saturation=100.0;
  param.percent_hue=100.0;
  (void) sscanf(modulate,"%lf%*[,/]%lf%*[,/]%lf",&param.percent_brightness,
    &param.percent_saturation,&param.percent_hue);
  /*
    Ensure that adjustment values are positive so they don't need to
    be checked in Modulate.
  */
  param.percent_brightness=AbsoluteValue(param.percent_brightness);
  param.percent_saturation=AbsoluteValue(param.percent_saturation);
  param.percent_hue=AbsoluteValue(param.percent_hue);

  FormatString(progress_message,"[%%s] Modulate %g/%g/%g...",
               param.percent_brightness,param.percent_saturation,
               param.percent_hue);
  TransformColorspace(image,RGBColorspace);
  if (image->storage_class == PseudoClass)
    {
      (void) ModulateImagePixels(NULL,&param,image,image->colormap,
                                 (IndexPacket *) NULL,image->colors,
                                 &image->exception);
      status=SyncImage(image);
    }
  else
    {
      status=PixelIterateMonoModify(ModulateImagePixels,NULL,progress_message,
                                    NULL,&param,0,0,image->columns,image->rows,
                                    image,&image->exception);
    }

  image->is_grayscale=is_grayscale;
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     N e g a t e I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method NegateImage negates the colors in the reference image.  The
%  Grayscale option means that only grayscale values within the image are
%  negated.
%
%  The format of the NegateImage method is:
%
%      unsigned int NegateImage(Image *image,const unsigned int grayscale)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
static MagickPassFail
NegateImagePixels(void *mutable_data,         /* User provided mutable data */
                  const void *immutable_data, /* User provided immutable data */
                  Image *image,               /* Modify image */
                  PixelPacket *pixels,        /* Pixel row */
                  IndexPacket *indexes,       /* Pixel row indexes */
                  const long npixels,         /* Number of pixels in row */
                  ExceptionInfo *exception)   /* Exception report */
{
  /*
    Negate the pixels.
  */
  const unsigned int
    grayscale = *((const unsigned int *) immutable_data);

  register long
    i;
  
  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  if (grayscale)
    {
      /* Process only the non-gray pixels */
      for (i=0; i < npixels; i++)
        {
          if (!IsGray(pixels[i]))
            continue;
          pixels[i].red=(~pixels[i].red);
          pixels[i].green=(~pixels[i].green);
          pixels[i].blue=(~pixels[i].blue);
          if (image->colorspace == CMYKColorspace)
            pixels[i].opacity=(~pixels[i].opacity);
        }
    }
  else
    {
      /* Process all pixels */
      for (i=0; i < npixels; i++)
        {
          pixels[i].red=(~pixels[i].red);
          pixels[i].green=(~pixels[i].green);
          pixels[i].blue=(~pixels[i].blue);
          if (image->colorspace == CMYKColorspace)
            pixels[i].opacity=(~pixels[i].opacity);
        }
    }

  return MagickPass;
}

#define NegateImageText "[%s] Negate..."
MagickExport MagickPassFail NegateImage(Image *image,const unsigned int grayscale)
{
  unsigned int
    non_gray = grayscale;

  unsigned int
    is_grayscale;

  MagickPassFail
    status=MagickPass;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  is_grayscale=image->is_grayscale;
  if (image->clip_mask)
    image->storage_class=DirectClass;

  if (image->storage_class == PseudoClass)
    {
      (void) NegateImagePixels(NULL,&non_gray,image,image->colormap,
                               (IndexPacket *) NULL,image->colors,
                               &image->exception);
      status=SyncImage(image);
    }
  else
    {
      status=PixelIterateMonoModify(NegateImagePixels,NULL,NegateImageText,
                                    NULL,&non_gray,0,0,image->columns,image->rows,
                                    image,&image->exception);
    }

  image->is_grayscale=is_grayscale;
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     N o r m a l i z e I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  The NormalizeImage() method enhances the contrast of a color image by
%  adjusting the pixels color to span the entire range of colors available.
%
%  The format of the NormalizeImage method is:
%
%      unsigned int NormalizeImage(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
#define MaxRange(color)  ScaleQuantumToMap(color)
MagickExport MagickPassFail NormalizeImage(Image *image)
{
  DoublePixelPacket
    high,
    *histogram,
    intensity,
    low;

  ApplyLevels_t
    levels;

  register long
    i;

  unsigned int
    is_grayscale;

  unsigned long
    threshold_intensity;

  MagickPassFail
    status=MagickPass;

  /*
    Allocate histogram and normalize map.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  is_grayscale=image->is_grayscale;
  levels.map=MagickAllocateMemory(PixelPacket *,(MaxMap+1)*sizeof(PixelPacket));
  if (levels.map == (PixelPacket *) NULL)
    ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
                          UnableToNormalizeImage);
  /*
    Form histogram.
  */
  histogram=BuildChannelHistograms(image,&image->exception);
  if (histogram == (DoublePixelPacket *) NULL)
    {
      MagickFreeMemory(levels.map);
      return MagickFail;
    }
  /*
    Find the histogram boundaries by locating the 0.1 percent levels.
  */
  threshold_intensity=(long) (image->columns*image->rows)/1000;
  (void) memset(&intensity,0,sizeof(DoublePixelPacket));
  for (low.red=0; low.red < MaxMap; low.red++)
    {
      intensity.red+=histogram[(long) low.red].red;
      if (intensity.red > threshold_intensity)
        break;
    }
  (void) memset(&intensity,0,sizeof(DoublePixelPacket));
  for (high.red=MaxMap; high.red != 0; high.red--)
    {
      intensity.red+=histogram[(long) high.red].red;
      if (intensity.red > threshold_intensity)
        break;
    }
  if (low.red == high.red)
    {
      /*
        Unreasonable contrast;  use zero threshold to determine boundaries.
      */
      threshold_intensity=0;
      (void) memset(&intensity,0,sizeof(DoublePixelPacket));
      for (low.red=0; low.red < MaxRange(MaxRGB); low.red++)
        {
          intensity.red+=histogram[(long) low.red].red;
          if (intensity.red > threshold_intensity)
            break;
        }
      (void) memset(&intensity,0,sizeof(DoublePixelPacket));
      for (high.red=MaxRange(MaxRGB); high.red != 0; high.red--)
        {
          intensity.red+=histogram[(long) high.red].red;
          if (intensity.red > threshold_intensity)
            break;
        }
    }
  (void) memset(&intensity,0,sizeof(DoublePixelPacket));
  for (low.green=0; low.green < MaxRange(MaxRGB); low.green++)
    {
      intensity.green+=histogram[(long) low.green].green;
      if (intensity.green > threshold_intensity)
        break;
    }
  (void) memset(&intensity,0,sizeof(DoublePixelPacket));
  for (high.green=MaxRange(MaxRGB); high.green != 0; high.green--)
    {
      intensity.green+=histogram[(long) high.green].green;
      if (intensity.green > threshold_intensity)
        break;
    }
  if (low.green == high.green)
    {
      /*
        Unreasonable contrast;  use zero threshold to determine boundaries.
      */
      threshold_intensity=0;
      (void) memset(&intensity,0,sizeof(DoublePixelPacket));
      for (low.green=0; low.green < MaxRange(MaxRGB); low.green++)
        {
          intensity.green+=histogram[(long) low.green].green;
          if (intensity.green > threshold_intensity)
            break;
        }
      (void) memset(&intensity,0,sizeof(DoublePixelPacket));
      for (high.green=MaxRange(MaxRGB); high.green != 0; high.green--)
        {
          intensity.green+=histogram[(long) high.green].green;
          if (intensity.green > threshold_intensity)
            break;
        }
    }
  (void) memset(&intensity,0,sizeof(DoublePixelPacket));
  for (low.blue=0; low.blue < MaxRange(MaxRGB); low.blue++)
    {
      intensity.blue+=histogram[(long) low.blue].blue;
      if (intensity.blue > threshold_intensity)
        break;
    }
  (void) memset(&intensity,0,sizeof(DoublePixelPacket));
  for (high.blue=MaxRange(MaxRGB); high.blue != 0; high.blue--)
    {
      intensity.blue+=histogram[(long) high.blue].blue;
      if (intensity.blue > threshold_intensity)
        break;
    }
  if (low.blue == high.blue)
    {
      /*
        Unreasonable contrast;  use zero threshold to determine boundaries.
      */
      threshold_intensity=0;
      (void) memset(&intensity,0,sizeof(DoublePixelPacket));
      for (low.blue=0; low.blue < MaxRange(MaxRGB); low.blue++)
        {
          intensity.blue+=histogram[(long) low.blue].blue;
          if (intensity.blue > threshold_intensity)
            break;
        }
      (void) memset(&intensity,0,sizeof(DoublePixelPacket));
      for (high.blue=MaxRange(MaxRGB); high.blue != 0; high.blue--)
        {
          intensity.blue+=histogram[(long) high.blue].blue;
          if (intensity.blue > threshold_intensity)
            break;
        }
    }
  high.opacity=0;
  low.opacity=0;
  if (image->matte)
    {
      (void) memset(&intensity,0,sizeof(DoublePixelPacket));
      for (low.opacity=0; low.opacity < MaxRange(MaxRGB); low.opacity++)
        {
          intensity.opacity+=histogram[(long) low.opacity].opacity;
          if (intensity.opacity > threshold_intensity)
            break;
        }
      (void) memset(&intensity,0,sizeof(DoublePixelPacket));
      for (high.opacity=MaxRange(MaxRGB); high.opacity != 0; high.opacity--)
        {
          intensity.opacity+=histogram[(long) high.opacity].opacity;
          if (intensity.opacity > threshold_intensity)
            break;
        }
      if (low.opacity == high.opacity)
        {
          /*
            Unreasonable contrast;  use zero threshold to determine boundaries.
          */
          threshold_intensity=0;
          (void) memset(&intensity,0,sizeof(DoublePixelPacket));
          for (low.opacity=0; low.opacity < MaxRange(MaxRGB); low.opacity++)
            {
              intensity.opacity+=histogram[(long) low.opacity].opacity;
              if (intensity.opacity > threshold_intensity)
                break;
            }
          (void) memset(&intensity,0,sizeof(DoublePixelPacket));
          for (high.opacity=MaxRange(MaxRGB); high.opacity != 0; high.opacity--)
            {
              intensity.opacity+=histogram[(long) high.opacity].opacity;
              if (intensity.opacity > threshold_intensity)
                break;
            }
        }
    }
  MagickFreeMemory(histogram);
  /*
    Stretch the histogram to create the normalized image mapping.
  */
  (void) memset(levels.map,0,(MaxMap+1)*sizeof(PixelPacket));
  for (i=0; i <= (long) MaxMap; i++)
    {
      if (i < (long) low.red)
        levels.map[i].red=0;
      else
        if (i > (long) high.red)
          levels.map[i].red=MaxRGB;
        else
          if (low.red != high.red)
            levels.map[i].red=
              ScaleMapToQuantum((MaxMap*(i-low.red))/(high.red-low.red));
      if (i < (long) low.green)
        levels.map[i].green=0;
      else
        if (i > (long) high.green)
          levels.map[i].green=MaxRGB;
        else
          if (low.green != high.green)
            levels.map[i].green=ScaleMapToQuantum((MaxMap*(i-low.green))/
                                                  (high.green-low.green));
      if (i < (long) low.blue)
        levels.map[i].blue=0;
      else
        if (i > (long) high.blue)
          levels.map[i].blue=MaxRGB;
        else
          if (low.blue != high.blue)
            levels.map[i].blue=
              ScaleMapToQuantum((MaxMap*(i-low.blue))/(high.blue-low.blue));
      if (image->matte)
        {
          if (i < (long) low.opacity)
            levels.map[i].opacity=0;
          else
            if (i > (long) high.opacity)
              levels.map[i].opacity=MaxRGB;
            else
              if (low.opacity != high.opacity)
                levels.map[i].opacity=ScaleMapToQuantum(
                                                        (MaxMap*(i-low.opacity))/(high.opacity-low.opacity));
        }
    }
  /*
    Normalize the image.
  */
  levels.level_red = (low.red != high.red);
  levels.level_green = (low.green != high.green);
  levels.level_blue = (low.blue != high.blue);
  levels.level_opacity= (image->matte && (low.opacity != high.opacity));
  if (image->storage_class == PseudoClass)
    {
      (void) ApplyLevels(NULL,&levels,image,image->colormap,
                         (IndexPacket *) NULL,image->colors,
                         &image->exception);
      status=SyncImage(image);
    }
  else
    {
      status=PixelIterateMonoModify(ApplyLevels,
                                    NULL,
                                    "[%s] Applying histogram normalization...",
                                    NULL,&levels,
                                    0,0,image->columns,image->rows,
                                    image,
                                    &image->exception);
    }
  MagickFreeMemory(levels.map);
  image->is_grayscale=is_grayscale;
  return(status);
}
