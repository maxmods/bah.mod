/*
% Copyright (C) 2003 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                               A N A L I Z E                                 %
%                                                                             %
%               Methods to Compute a Information about an Image               %
%                                                                             %
%                                                                             %
%                             Software Design                                 %
%                               Bill Corbis                                   %
%                              December 1998                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/attribute.h"
#include "magick/gem.h"
#include "magick/pixel_cache.h"
#include "magick/utility.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A n a l y z e I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method AnalyzeImage computes the brightness and saturation mean and
%  standard deviation and stores these values as attributes of the image.
%
%  The format of the AnalyzeImage method is:
%
%      unsigned int AnalyzeImage(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
*/
#define PRECISION "%.0f"
ModuleExport unsigned int AnalyzeImage(Image **image,
  const int ARGUNUSED(argc),char **ARGUNUSED(argv))
{
  double
    bsumX = 0.0,
    bsumX2 = 0.0,
    brightness_mean = 0.0,
    brightness_stdev = 0.0,
    ssumX = 0.0,
    ssumX2 = 0.0,
    saturation_mean = 0.0,
    saturation_stdev = 0.0,
    total_pixels = 0.0;

  double
    brightness,
    hue,
    saturation;

  long
    y;

  register long
    x;

  register PixelPacket
    *p;

  char
    text[MaxTextExtent];

  assert(image != (Image **) NULL);
  assert(*image != (Image *) NULL);
  for (y=0; y < (int) (*image)->rows; y++)
  {
    p=GetImagePixels((*image),0,y,(*image)->columns,1);
    if (p == (PixelPacket *) NULL)
      break;
    if (y == 0)
      {
        FormatString(text,"#%02x%02x%02x",p->red,p->green,p->blue);
        (void) SetImageAttribute((*image),"TopLeftColor",text);
      }
    if (y == (long) ((*image)->rows-1))
      {
        FormatString(text,"#%02x%02x%02x",p->red,p->green,p->blue);
        (void) SetImageAttribute((*image),"BottomLeftColor",text);
      }
    for (x=0; x < (long) (*image)->columns; x++)
    {
      TransformHSL(p->red,p->green,p->blue,&hue,&saturation,&brightness);
      brightness *= MaxRGB;
            bsumX += brightness;
            bsumX2 += brightness * brightness;
      saturation *= MaxRGB;
            ssumX += saturation;
            ssumX2 += saturation * saturation;
      total_pixels++;
      p++;
    }
    p--; /* backup one pixel to allow us to sample */
    if (y == 0)
      {
        FormatString(text,"#%02x%02x%02x",p->red,p->green,p->blue);
        (void) SetImageAttribute((*image),"TopRightColor",text);
      }
    if (y == (long) ((*image)->rows-1))
      {
        FormatString(text,"#%02x%02x%02x",p->red,p->green,p->blue);
        (void) SetImageAttribute((*image),"BottomRightColor",text);
      }
  }
  if (total_pixels > 0.0)
    {
      brightness_mean = bsumX/total_pixels;
      FormatString(text,PRECISION,brightness_mean);
      (void) SetImageAttribute((*image),"BrightnessMean",text);
      /*  This formula gives a slightly biased result */
      brightness_stdev =
          sqrt(bsumX2/total_pixels - (bsumX/total_pixels*bsumX/total_pixels));
      FormatString(text,PRECISION,brightness_stdev);
      (void) SetImageAttribute((*image),"BrightnessStddev",text);
      /* Now the correction for bias. */
      /*  stdev = stdev*sqrt((double)total_pixels/(double)(total_pixels-1)); */
      /* Now calculate the standard deviation of the mean */
      /*  brightness_stdevmean = bstdev/sqrt((double)total_pixels); */

      saturation_mean = ssumX/total_pixels;
      FormatString(text,PRECISION,saturation_mean);
      (void) SetImageAttribute((*image),"SaturationMean",text);
      /* This formula gives a slightly biased result */
      saturation_stdev =
          sqrt(ssumX2/total_pixels - (ssumX/total_pixels*ssumX/total_pixels));
      FormatString(text,PRECISION,saturation_stdev);
      (void) SetImageAttribute((*image),"SaturationStddev",text);
      /* Now the correction for bias. */
      /*  stdev = stdev*sqrt((double)total_pixels/(double)(total_pixels-1)); */
      /* Now calculate the standard deviation of the mean */
      /*  saturation_stdevmean = sstdev/sqrt((double)total_pixels); */
    }
  return(True);
}
