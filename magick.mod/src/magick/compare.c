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
%               CCCC   OOO   M   M  PPPP    AAA   RRRR    EEEEE               %
%              C      O   O  MM MM  P   P  A   A  R   R   E                   %
%              C      O   O  M M M  PPPP   AAAAA  RRRR    EEE                 %
%              C      O   O  M   M  P      A   A  R R     E                   %
%               CCCC   OOO   M   M  P      A   A  R  R    EEEEE               %
%                                                                             %
%                                                                             %
%                    GraphicsMagick Image Compare Methods                     %
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
#include "magick/color.h"
#include "magick/color_lookup.h"
#include "magick/compare.h"
#include "magick/enum_strings.h"
#include "magick/pixel_iterator.h"
#include "magick/utility.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D i f f e r e n c e I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DifferenceImage() returns an annotated difference image based on the
%  the difference between a reference image and a compare image.
%
%  The format of the DifferenceImage method is:
%
%      Image *DifferenceImage(const Image *reference_image,
%                             const Image *compare_image,
%                             const DifferenceImageOptions *difference_options,
%                             ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o reference_image: the reference image.
%
%    o compare_image: the comparison image.
%
%    o difference_options: options to use when differencing.
%
%    o channel: the channel(s) to compare.
%
%    o exception: Return any errors or warnings in this structure.
%
*/
static MagickPassFail
DifferenceImagePixels(void *mutable_data,                  /* User provided mutable data */
                      const void *immutable_data,          /* User provided immutable data */
                      const Image *reference_image,        /* Source 1 image */
                      const PixelPacket *reference_pixels, /* Pixel row in source 1 image */
                      const IndexPacket *reference_indexes,/* Pixel row indexes in source 1 image */
                      const Image *compare_image,          /* Source 2 image */
                      const PixelPacket *compare_pixels,   /* Pixel row in source 2 image */
                      const IndexPacket *compare_indexes,  /* Pixel row indexes in source 2 image */
                      Image *result_image,                 /* Update image */
                      PixelPacket *result_pixels,          /* Pixel row in update image */
                      IndexPacket *result_indexes,         /* Pixel row indexes in update image */
                      const long npixels,                  /* Number of pixels in row */
                      ExceptionInfo *exception             /* Exception report */
                   )
{
  const DifferenceImageOptions
    *difference_options = (const DifferenceImageOptions *) immutable_data;

  register ChannelType
    channels = difference_options->channel;

  register long
    i;

  register MagickBool
    change;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(compare_image);
  ARG_NOT_USED(result_image);
  ARG_NOT_USED(result_indexes);
  ARG_NOT_USED(exception);

  for (i=0; i < npixels; i++)
    {
      change=MagickFalse;

      if (IsCMYKColorspace(reference_image->colorspace))
        {
          if (MagickChannelEnabled(channels,CyanChannel) &&
              (GetCyanSample(&reference_pixels[i]) != GetCyanSample(&compare_pixels[i])))
            change=MagickTrue;
          if (MagickChannelEnabled(channels,MagentaChannel) &&
              (GetMagentaSample(&reference_pixels[i]) != GetMagentaSample(&compare_pixels[i])))
            change=MagickTrue;
          if (MagickChannelEnabled(channels,YellowChannel) &&
              (GetYellowSample(&reference_pixels[i]) != GetYellowSample(&compare_pixels[i])))
            change=MagickTrue;
          if (MagickChannelEnabled(channels,BlackChannel) &&
              (GetBlackSample(&reference_pixels[i]) != GetBlackSample(&compare_pixels[i])))
            change=MagickTrue;
          if (MagickChannelEnabled(channels,OpacityChannel) &&
              (reference_indexes[i] != compare_indexes[i]))
            change=MagickTrue;
        }
      else
        {
          if (MagickChannelEnabled(channels,RedChannel) &&
              (GetRedSample(&reference_pixels[i]) != GetRedSample(&compare_pixels[i])))
            change=MagickTrue;
          if (MagickChannelEnabled(channels,GreenChannel) &&
              (GetGreenSample(&reference_pixels[i]) != GetGreenSample(&compare_pixels[i])))
            change=MagickTrue;
          if (MagickChannelEnabled(channels,BlueChannel) &&
              (GetBlueSample(&reference_pixels[i]) != GetBlueSample(&compare_pixels[i])))
            change=MagickTrue;
          if (MagickChannelEnabled(channels,OpacityChannel) &&
              (GetOpacitySample(&reference_pixels[i]) != GetOpacitySample(&compare_pixels[i])))
            change=MagickTrue;
        }
      /*
        Modify result image to reflect change.
      */
      switch (difference_options->highlight_style)
        {
        case UndefinedHighlightStyle:
          break;
        case AssignHighlightStyle:
          {
            /*
              Changed pixels are assigned the highlight color.
            */
            if (change)
              result_pixels[i]=difference_options->highlight_color;
            else
              result_pixels[i]=compare_pixels[i];
            break;
          }
        case ThresholdHighlightStyle:
          {
            /*
              For changed pixels, compare the pixel intensity.  If the
              pixel intensity in the compare image is higher than the
              reference image, then set the pixel to white, otherwise
              set it to black.
            */
            if (change)
              {
                Quantum
                  compare_intensity,
                  intensity,
                  reference_intensity;

                compare_intensity=PixelIntensity(&compare_pixels[i]);
                reference_intensity=PixelIntensity(&reference_pixels[i]);
                if (compare_intensity > reference_intensity)
                  intensity=MaxRGB;
                else
                  intensity=0U;
                result_pixels[i].red = result_pixels[i].green = result_pixels[i].blue = intensity;
                result_pixels[i].opacity=compare_pixels[i].opacity;
              }
            else
              {
                result_pixels[i]=compare_pixels[i];
              }
            break;
          }
        case TintHighlightStyle:
          {
            /*
              Alpha composite highlight color on top of change pixels.
            */
            if (change)
              AlphaCompositePixel(&result_pixels[i],&difference_options->highlight_color,0.75*MaxRGBDouble,
                                  &compare_pixels[i],compare_pixels[i].opacity);
            else
              result_pixels[i]=compare_pixels[i];
            break;
          }
        case XorHighlightStyle:
          {
            if (change)
              {
                result_pixels[i].red = compare_pixels[i].red ^ difference_options->highlight_color.red;
                result_pixels[i].green = compare_pixels[i].green ^ difference_options->highlight_color.green;
                result_pixels[i].blue = compare_pixels[i].blue ^ difference_options->highlight_color.blue;
                result_pixels[i].opacity = compare_pixels[i].opacity ^ difference_options->highlight_color.opacity;
              }
            else
              {
                result_pixels[i]=compare_pixels[i];
              }
            break;
          }
        }
    }

  return MagickPass;
}

MagickExport Image *
DifferenceImage(const Image *reference_image,const Image *compare_image,
                const DifferenceImageOptions *difference_options,
                ExceptionInfo *exception)
{
  Image
    *difference_image;

  assert(reference_image != (const Image *) NULL);
  assert(reference_image->signature == MagickSignature);
  assert(compare_image != (const Image *) NULL);
  assert(compare_image->signature == MagickSignature);
  assert(difference_options != (const DifferenceImageOptions *) NULL);
  assert(exception != (ExceptionInfo *) NULL);

  difference_image=CloneImage(compare_image,compare_image->columns,
                              compare_image->rows,MagickTrue,exception);
  if (difference_image == (Image *) NULL)
    return ((Image *) NULL);

  /*
    Update "difference" image to mark changes.
  */
  (void) PixelIterateTripleModify(DifferenceImagePixels,
                                  NULL,
                                  "[%s]*[%s]->[%s] Difference image pixels ...",
                                  NULL,difference_options,
                                  reference_image->columns,reference_image->rows,
                                  reference_image, compare_image,0, 0,
                                  difference_image, 0, 0,
                                  exception);
  return difference_image;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e C h a n n e l D i f f e r e n c e                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageChannelDifference() updates a user provided statistics structure
%  with per-channel, and totalized, difference statistics corresponding
%  to a specified comparison metric.
%
%  The format of the GetImageChannelDifference method is:
%
%      MagickPassFail GetImageChannelDifference(const Image *reference_image,
%                                           const Image *compare_image,
%                                           const MetricType metric,
%                                           DifferenceStatistics *statistics,
%                                           ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o reference_image: the reference image.
%
%    o compare_image: the comparison image.
%
%    o metric: metric to use when differencing.
%
%    o statistics: the statistics structure to populate.
%
%    o exception: Return any errors or warnings in this structure.
%
*/
/*
  Compute the total absolute value difference.

  In this case we sum the absolute value difference between channel
  pixel quantums.
*/
static MagickPassFail
ComputeAbsoluteError(void *mutable_data,
                     const void *immutable_data,
                     const Image *first_image,
                     const PixelPacket *first_pixels,
                     const IndexPacket *first_indexes,
                     const Image *second_image,
                     const PixelPacket *second_pixels,
                     const IndexPacket *second_indexes,
                     const long npixels,
                     ExceptionInfo *exception)
{
  DifferenceStatistics
    lstats,
    *stats = (DifferenceStatistics *) mutable_data;

  register long
    i;

  ARG_NOT_USED(immutable_data);
  ARG_NOT_USED(first_image);
  ARG_NOT_USED(first_indexes);
  ARG_NOT_USED(second_image);
  ARG_NOT_USED(second_indexes);
  ARG_NOT_USED(exception);

  InitializeDifferenceStatistics(&lstats,exception);
  for (i=0; i < npixels; i++)
    {
      lstats.red += fabs(first_pixels[i].red-(double) second_pixels[i].red)/MaxRGBDouble;
      lstats.green += fabs(first_pixels[i].green-(double) second_pixels[i].green)/MaxRGBDouble;
      lstats.blue += fabs(first_pixels[i].blue-(double) second_pixels[i].blue)/MaxRGBDouble;
      lstats.opacity += fabs(first_pixels[i].opacity-(double) second_pixels[i].opacity)/MaxRGBDouble;
    }

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_ComputeAbsoluteError)
#endif
  {
    stats->red += lstats.red;
    stats->green += lstats.green;
    stats->blue += lstats.blue;
    stats->opacity += lstats.opacity;
  }

  return (MagickPass);
}

/*
  Compute the peak absolute difference.

  In this case we compute the simple difference between channel pixel
  quantums, obtain the absolute value, and store the value if it is
  greater than the current peak value.
*/
static MagickPassFail
ComputePeakAbsoluteError(void *mutable_data,
                         const void *immutable_data,
                         const Image *first_image,
                         const PixelPacket *first_pixels,
                         const IndexPacket *first_indexes,
                         const Image *second_image,
                         const PixelPacket *second_pixels,
                         const IndexPacket *second_indexes,
                         const long npixels,
                         ExceptionInfo *exception)
{
  DifferenceStatistics
    lstats,
    *stats = (DifferenceStatistics *) mutable_data;

  double
    difference;

  register long
    i;

  ARG_NOT_USED(immutable_data);
  ARG_NOT_USED(first_image);
  ARG_NOT_USED(first_indexes);
  ARG_NOT_USED(second_image);
  ARG_NOT_USED(second_indexes);

  InitializeDifferenceStatistics(&lstats,exception);
  for (i=0; i < npixels; i++)
    {
      difference=fabs(first_pixels[i].red-(double) second_pixels[i].red)/MaxRGBDouble;
      if (difference > lstats.red)
        lstats.red=difference;

      difference=fabs(first_pixels[i].green-(double) second_pixels[i].green)/MaxRGBDouble;
      if (difference > lstats.green)
        lstats.green=difference;

      difference=fabs(first_pixels[i].blue-(double) second_pixels[i].blue)/MaxRGBDouble;
      if (difference > lstats.blue)
        lstats.blue=difference;

      difference=fabs(first_pixels[i].opacity-(double) second_pixels[i].opacity)/MaxRGBDouble;
      if (difference > lstats.opacity)
        lstats.opacity=difference;
    }

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_ComputePeakAbsoluteError)
#endif
  {
    if (lstats.red > stats->red)
      stats->red=lstats.red;
    if (lstats.green > stats->green)
      stats->green=lstats.green;
    if (lstats.blue > stats->blue)
      stats->blue=lstats.blue;
    if (lstats.opacity > stats->opacity)
      stats->opacity=lstats.opacity;
  }

  return (MagickPass);
}

/*
  Compute the squared difference.

  In this case we sum the square of the difference between channel
  pixel quantums.
*/
static MagickPassFail
ComputeSquaredError(void *mutable_data,
                    const void *immutable_data,
                    const Image *first_image,
                    const PixelPacket *first_pixels,
                    const IndexPacket *first_indexes,
                    const Image *second_image,
                    const PixelPacket *second_pixels,
                    const IndexPacket *second_indexes,
                    const long npixels,
                    ExceptionInfo *exception)
{
  DifferenceStatistics
    lstats,
    *stats = (DifferenceStatistics *) mutable_data;

  double
    difference;

  register long
    i;

  ARG_NOT_USED(immutable_data);
  ARG_NOT_USED(first_image);
  ARG_NOT_USED(first_indexes);
  ARG_NOT_USED(second_image);
  ARG_NOT_USED(second_indexes);
  ARG_NOT_USED(exception);

  InitializeDifferenceStatistics(&lstats,exception);
  for (i=0; i < npixels; i++)
    {
      difference=(first_pixels[i].red-(double) second_pixels[i].red)/MaxRGBDouble;
      lstats.red += difference*difference;

      difference=(first_pixels[i].green-(double) second_pixels[i].green)/MaxRGBDouble;
      lstats.green += difference*difference;

      difference=(first_pixels[i].blue-(double) second_pixels[i].blue)/MaxRGBDouble;
      lstats.blue += difference*difference;

      difference=(first_pixels[i].opacity-(double) second_pixels[i].opacity)/MaxRGBDouble;
      lstats.opacity += difference*difference;
    }

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_ComputeSquaredError)
#endif
  {
    stats->red += lstats.red;
    stats->green += lstats.green;
    stats->blue += lstats.blue;
    stats->opacity += lstats.opacity;
  }

  return (MagickPass);
}
MagickExport MagickPassFail
GetImageChannelDifference(const Image *reference_image,
                          const Image *compare_image,
                          const MetricType metric,
                          DifferenceStatistics *statistics,
                          ExceptionInfo *exception)
{
  PixelIteratorDualReadCallback
    call_back = (PixelIteratorDualReadCallback) NULL;

  MagickPassFail
    status = MagickFail;

  assert(reference_image != (const Image *) NULL);
  assert(reference_image->signature == MagickSignature);
  assert(compare_image != (const Image *) NULL);
  assert(compare_image->signature == MagickSignature);
  assert(statistics != (DifferenceStatistics *) NULL);
  assert(exception != (ExceptionInfo *) NULL);

  InitializeDifferenceStatistics(statistics,exception);

  /*
    Select basic differencing function to use.
  */
  switch (metric)
    {
    case UndefinedMetric:
      break;
    case MeanAbsoluteErrorMetric:
      call_back=ComputeAbsoluteError;
      break;
    case MeanSquaredErrorMetric:
      call_back=ComputeSquaredError;
      break;
    case PeakAbsoluteErrorMetric:
      call_back=ComputePeakAbsoluteError;
      break;
    case PeakSignalToNoiseRatioMetric:
      call_back=ComputeSquaredError;
      break;
    case RootMeanSquaredErrorMetric:
      call_back=ComputeSquaredError;
      break;
    }

  if (call_back != (PixelIteratorDualReadCallback) NULL)
    {
      double
        number_channels,
        number_pixels;

      char
        description[MaxTextExtent];
      
      FormatString(description,"[%%s]*[%%s] Compute image difference using %s metric...",
                   MetricTypeToString(metric));

      status=PixelIterateDualRead(call_back,
                                  NULL,
                                  description,
                                  statistics, NULL,
                                  reference_image->columns,reference_image->rows,
                                  reference_image,0,0,
                                  compare_image,0,0,
                                  exception);
      /*
        Post-process statistics (as required)
      */

      number_channels=3.0 + (reference_image->matte ? 1.0 : 0.0);
      number_pixels=(double) reference_image->columns*reference_image->rows;

      if ((MeanAbsoluteErrorMetric == metric) ||
          (MeanSquaredErrorMetric == metric) ||
          (PeakSignalToNoiseRatioMetric == metric)||
          (RootMeanSquaredErrorMetric == metric))
        {
          /*
            Compute mean values.
          */
          statistics->combined=((statistics->red+statistics->green+
                                 statistics->blue+
                                 (reference_image->matte ? statistics->opacity : 0.0))/
                                (number_pixels*number_channels));
          statistics->red /= number_pixels;
          statistics->green /= number_pixels;
          statistics->blue /= number_pixels;
          statistics->opacity /= number_pixels;
        }

      if (PeakAbsoluteErrorMetric == metric)
        {
          /*
            Determine peak channel value
          */
          if (statistics->red > statistics->combined)
            statistics->combined=statistics->red;

          if (statistics->green > statistics->combined)
            statistics->combined=statistics->green;

          if (statistics->blue > statistics->combined)
            statistics->combined=statistics->blue;

          if ((reference_image->matte) && (statistics->opacity > statistics->combined))
            statistics->combined=statistics->opacity;
        }

      if (PeakSignalToNoiseRatioMetric == metric)
        {
          /*
            Compute PSNR.
          */
          statistics->red=(20.0 * log10(1.0/sqrt(statistics->red)));
          statistics->green=(20.0 * log10(1.0/sqrt(statistics->green)));
          statistics->blue=(20.0 * log10(1.0/sqrt(statistics->blue)));
          statistics->opacity=(20.0 * log10(1.0/sqrt(statistics->opacity)));
          statistics->combined=(20.0 * log10(1.0/sqrt(statistics->combined)));
        }

      if (RootMeanSquaredErrorMetric == metric)
        {
          /*
            Compute RMSE.
          */
          statistics->red=sqrt(statistics->red);
          statistics->green=sqrt(statistics->green);
          statistics->blue=sqrt(statistics->blue);
          statistics->opacity=sqrt(statistics->opacity);
          statistics->combined=sqrt(statistics->combined);
        }
    }

  return status;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e C h a n n e l D i s t o r t i o n                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageChannelDistortion() updates a distortion parameter with the
%  distortion (error) computed according to the specified comparison metric.
%  The value returned is only for the channel specified.
%
%  The format of the GetImageChannelDistortion method is:
%
%       MagickPassFail GetImageChannelDistortion(const Image *reference_image,
%                                                const Image *compare_image,
%                                                const ChannelType channel,
%                                                const MetricType metric,
%                                                double *distortion,
%                                                ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o reference_image: the reference image.
%
%    o compare_image: the comparison image.
%
%    o channel: the channel to obtain error data for.
%
%    o metric: metric to use when differencing.
%
%    o distortion: updated with the computed distortion.
%
%    o exception: Return any errors or warnings in this structure.
%
*/
MagickExport MagickPassFail
GetImageChannelDistortion(const Image *reference_image,
                          const Image *compare_image,
                          const ChannelType channel,
                          const MetricType metric,
                          double *distortion,
                          ExceptionInfo *exception)
{
  DifferenceStatistics
    statistics;

  MagickPassFail
    status;

  assert(distortion != (double *) NULL);

  *distortion=1.0;
  status=GetImageChannelDifference(reference_image,compare_image,metric,
                                   &statistics,exception);
  switch (channel)
    {
    case RedChannel:
    case CyanChannel:
      *distortion=statistics.red;
      break;
    case GreenChannel:
    case MagentaChannel:
      *distortion=statistics.green;
      break;
    case BlueChannel:
    case YellowChannel:
      *distortion=statistics.blue;
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      *distortion=statistics.opacity;
      break;
    case UndefinedChannel:
    case AllChannels:
    case GrayChannel:
      *distortion=statistics.combined;
      break;
    }

    return status;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e D i s t o r t i o n                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageDistortion() updates a distortion parameter with the distortion
%  (error) computed according to the specified comparison metric.  The value
%  returned reflects all enabled channels.
%
%  The format of the GetImageDistortion method is:
%
%       MagickPassFail GetImageDistortion(const Image *reference_image,
%                                         const Image *compare_image,
%                                         const MetricType metric,
%                                         double *distortion,
%                                         ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o reference_image: the reference image.
%
%    o compare_image: the comparison image.
%
%    o channel: the channel to obtain error data for.
%
%    o metric: metric to use when differencing.
%
%    o distortion: updated with the computed distortion.
%
%    o exception: Return any errors or warnings in this structure.
%
*/
MagickExport MagickPassFail
GetImageDistortion(const Image *reference_image,
                   const Image *compare_image,
                   const MetricType metric,
                   double *distortion,
                   ExceptionInfo *exception)
{
  return GetImageChannelDistortion(reference_image,compare_image,AllChannels,
                                   metric,distortion,exception);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  I s I m a g e s E q u a l                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IsImagesEqual() measures the difference between colors at each pixel
%  location of two images.  A value other than 0 means the colors match
%  exactly.  Otherwise an error measure is computed by summing over all
%  pixels in an image the distance squared in RGB space between each image
%  pixel and its corresponding pixel in the reference image.  The error
%  measure is assigned to these image members:
%
%    o mean_error_per_pixel:  The mean error for any single pixel in
%      the image.
%
%    o normalized_mean_error:  The normalized mean quantization error for
%      any single pixel in the image.  This distance measure is normalized to
%      a range between 0 and 1.  It is independent of the range of red, green,
%      and blue values in the image.
%
%    o normalized_maximum_error:  The normalized maximum quantization
%      error for any single pixel in the image.  This distance measure is
%      normalized to a range between 0 and 1.  It is independent of the range
%      of red, green, and blue values in your image.
%
%  A small normalized mean square error, accessed as
%  image->normalized_mean_error, suggests the images are very similiar in
%  spatial layout and color.
%
%  The format of the IsImagesEqual method is:
%
%      MagickBool IsImagesEqual(Image *image,const Image *reference)
%
%  A description of each parameter follows.
%
%    o image: The image.
%
%    o reference: The reference image.
%
*/
typedef struct _ErrorStatistics {
  double
    maximum,
    total;
} ErrorStatistics;

static MagickPassFail
ComputePixelError(void *mutable_data,
                  const void *immutable_data,
                  const Image *first_image,
                  const PixelPacket *first_pixels,
                  const IndexPacket *first_indexes,
                  const Image *second_image,
                  const PixelPacket *second_pixels,
                  const IndexPacket *second_indexes,
                  const long npixels,
                  ExceptionInfo *exception)
{
  ErrorStatistics
    *stats = (ErrorStatistics *) mutable_data;

  double
    difference,
    distance,
    distance_squared,
    stats_maximum,
    stats_total;

  register long
    i;

  ARG_NOT_USED(immutable_data);
  ARG_NOT_USED(first_indexes);
  ARG_NOT_USED(second_image);
  ARG_NOT_USED(second_indexes);
  ARG_NOT_USED(exception);

  stats_maximum=0.0;
  stats_total=0.0;

  for (i=0; i < npixels; i++)
    {
      difference=(first_pixels[i].red-(double) second_pixels[i].red)/MaxRGBDouble;
      distance_squared=(difference*difference);

      difference=(first_pixels[i].green-(double) second_pixels[i].green)/MaxRGBDouble;
      distance_squared+=(difference*difference);

      difference=(first_pixels[i].blue-(double) second_pixels[i].blue)/MaxRGBDouble;
      distance_squared+=(difference*difference);

      if (first_image->matte)
        {
          difference=(first_pixels[i].opacity-(double) second_pixels[i].opacity)/MaxRGBDouble;
          distance_squared+=(difference*difference);
        }
      distance=sqrt(distance_squared);

      stats_total+=distance;
      if (distance > stats_maximum)
        stats_maximum=distance;
    }

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_ComputePixelError)
#endif
  {
    stats->total+=stats_total;

    if (stats_maximum > stats->maximum)
      stats->maximum=stats_maximum;
  }
  return (MagickPass);
}

MagickExport MagickBool
IsImagesEqual(Image *image,const Image *reference)
{
  ErrorStatistics
    stats;

  double
    mean_error_per_pixel,
    normalize,
    number_pixels;

  /*
    Initialize measurement.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(reference != (const Image *) NULL);
  assert(reference->signature == MagickSignature);
  (void) memset(&image->error,0,sizeof(ErrorInfo));
  if ((image->rows != reference->rows) ||
      (image->columns != reference->columns))
    ThrowBinaryException3(ImageError,UnableToCompareImages,
      ImageSizeDiffers);
  if ((image->colorspace != reference->colorspace) &&
      (!IsRGBColorspace(image->colorspace) || !IsRGBColorspace(reference->colorspace)))
    ThrowBinaryException3(ImageError,UnableToCompareImages,
      ImageColorspaceDiffers);
  if(image->matte != reference->matte)
    ThrowBinaryException3(ImageError,UnableToCompareImages,
      ImageOpacityDiffers);

  /*
    For each pixel, collect error statistics.
  */
  number_pixels=(double) image->columns*image->rows;

  stats.maximum=0.0;
  stats.total=0.0;

  (void) PixelIterateDualRead(ComputePixelError,
                              NULL,
                              "[%s]*[%s] Compute pixel error ...",
                              &stats, NULL,
                              image->columns,image->rows,
                              image,0,0,
                              reference,0,0,
                              &image->exception);

  /*
    Compute final error statistics.
  */
  
  if (image->matte)
    normalize = sqrt(4.0); /* sqrt(1.0*1.0+1.0*1.0+1.0*1.0+1.0*1.0) */
  else
    normalize = sqrt(3.0); /* sqrt(1.0*1.0+1.0*1.0+1.0*1.0) */
  mean_error_per_pixel=stats.total/number_pixels;
  image->error.mean_error_per_pixel=mean_error_per_pixel*MaxRGBDouble;
  image->error.normalized_mean_error=mean_error_per_pixel/normalize;
  image->error.normalized_maximum_error=stats.maximum/normalize;
  return(image->error.normalized_mean_error == 0.0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I n i t i a l i z e D i f f e r e n c e I m a g e O p t i o n s           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  InitializeDifferenceImageOptions() assigns default options to a user-provided
%  DifferenceImageOptions structure.  This function should always be used
%  to initialize the DifferenceImageOptions structure prior to making any
%  changes to it.
%
%  The format of the InitializeDifferenceImageOptions method is:
%
%      void InitializeDifferenceImageOptions(DifferenceImageOptions *options,
%                                            ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o options: pointer to DifferenceImageOptions structure to initialize.
%
%    o exception: Return any errors or warnings in this structure.
%
*/
MagickExport void
InitializeDifferenceImageOptions(DifferenceImageOptions *options,
                                 ExceptionInfo *exception)
{
  assert(options != (DifferenceImageOptions *) NULL);
  memset(options,0,sizeof(DifferenceImageOptions));
  options->channel=AllChannels;
  options->highlight_style=TintHighlightStyle;
  (void) QueryColorDatabase(HighlightColor,&options->highlight_color,exception);
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I n i t i a l i z e D i f f e r e n c e S t a t i s t i c s               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  InitializeDifferenceStatistics() assigns default options to a user-provided
%  DifferenceStatistics structure.
%
%  The format of the InitializeDifferenceStatistics method is:
%
%      void InitializeDifferenceStatistics(DifferenceStatistics *options,
%                                          ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o options: pointer to DifferenceStatistics structure to initialize.
%
%    o exception: Return any errors or warnings in this structure.
%
*/
MagickExport void
InitializeDifferenceStatistics(DifferenceStatistics *statistics,
                               ExceptionInfo *exception)
{
  ARG_NOT_USED(exception);
  assert(statistics != (DifferenceStatistics *) NULL);
  statistics->red=0.0;
  statistics->green=0.0;
  statistics->blue=0.0;
  statistics->opacity=0.0;
  statistics->combined=0.0;
}
