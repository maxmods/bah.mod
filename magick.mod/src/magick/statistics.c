/*
% Copyright (C) 2003 - 2009 GraphicsMagick Group
% Copyright (C) 2003 ImageMagick Studio
% Copyright 1991-1999 E. I. du Pont de Nemours and Company
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
% GraphicsMagick Image Statistics Methods.
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/pixel_iterator.h"
#include "magick/statistics.h"
#include "magick/utility.h"

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
#  pragma omp critical (GM_GetImageStatisticsMean)
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
#  pragma omp critical (GM_GetImageStatisticsVariance)
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
#  pragma omp critical (GM_GetImageStatisticsVariance)
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

