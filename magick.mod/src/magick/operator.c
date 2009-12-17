/*
% Copyright (C) 2004 - 2009 GraphicsMagick Group
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
% Interfaces to support quantum operators.
% Written by Bob Friesenhahn, March 2004.
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/enum_strings.h"
#include "magick/gem.h"
#include "magick/pixel_iterator.h"
#include "magick/random.h"
#include "magick/utility.h"
#include "magick/operator.h"

/*
  Types.
*/
typedef struct _QuantumImmutableContext
{
  ChannelType channel;
  Quantum quantum_value;
  double double_value;
} QuantumImmutableContext;

typedef struct _QuantumMutableContext
{
  Quantum *channel_lut;
} QuantumMutableContext;

typedef struct _ChannelOptions_t
{
  DoublePixelPacket
    values;

  MagickBool
    red_enabled,
    green_enabled,
    blue_enabled,
    opacity_enabled;
} ChannelOptions_t;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   Q u a n t u m O p e r a t o r I m a g e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  QuantumOperatorImage() performs the requested arithmetic,
%  bitwise-logical, or value operation on the selected channels of
%  the entire image.  The AllChannels channel option operates on all
%  color channels whereas the GrayChannel channel option treats the
%  color channels as a grayscale intensity.
%
%  These operations are on the DirectClass pixels of the image and do not
%  update pixel indexes or colormap.
%
%  The format of the QuantumOperatorImage method is:
%
%      MagickPassFail QuantumOperatorImage(Image *image,
%        ChannelType channel, QuantumOperator operator,
%        double rvalue)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o channel: Channel to operate on (RedChannel, CyanChannel,
%        GreenChannel, MagentaChannel, BlueChannel, YellowChannel,
%        OpacityChannel, BlackChannel, MatteChannel, AllChannels,
%        GrayChannel).  The AllChannels type only updates color
%        channels.  The GrayChannel type treats the color channels
%        as if they represent an intensity.
%
%    o quantum_operator: Operator to use (AddQuantumOp,AndQuantumOp,
%        AssignQuantumOp, DepthQuantumOp, DivideQuantumOp, GammaQuantumOp,
%        LShiftQuantumOp, MultiplyQuantumOp,  NegateQuantumOp,
%        NoiseGaussianQuantumOp, NoiseImpulseQuantumOp,
%        NoiseLaplacianQuantumOp, NoiseMultiplicativeQuantumOp,
%        NoisePoissonQuantumOp, NoiseUniformQuantumOp, OrQuantumOp,
%        RShiftQuantumOp, SubtractQuantumOp, ThresholdBlackQuantumOp,
%        ThresholdQuantumOp, ThresholdWhiteQuantumOp, XorQuantumOp).
%
%    o rvalue: Operator argument.
%
%    o exception: Updated with error description.
%
*/
MagickExport MagickPassFail QuantumOperatorImage(Image *image,
  const ChannelType channel,const QuantumOperator quantum_operator,
  const double rvalue,ExceptionInfo *exception)
{
  return QuantumOperatorRegionImage(image,0,0,image->columns,image->rows,
                                    channel,quantum_operator,rvalue,exception);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   Q u a n t u m O p e r a t o r I m a g e M u l t i v a l u e               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  QuantumOperatorImageMultivalue() is a semi-private implementation
%  fuction which accepts a comma delimited string of per-channel values
%  and applies a specified operator to the channels of the image.  The
%  main reason for this function to exist is to support
%  ChannelThresholdPixels(), BlackThresholdImage(), WhiteThresholdImage(),
%  or any other legacy style function which needs to be implemented.
%
%  The format of the QuantumOperatorImageMultivalue method is:
%
%      MagickPassFail QuantumOperatorImageMultivalue(
%                                 Image *image,
%                                 const QuantumOperator quantum_operator,
%                                 const char *values)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o values: define the rvalues, <red>{<green>,<blue>,<opacity>}{%}.
%
*/
MagickExport MagickPassFail
QuantumOperatorImageMultivalue(Image *image,
                               const QuantumOperator quantum_operator,
                               const char *values)
{
  ChannelOptions_t
    options;

  int
    count;

  MagickPassFail
    status;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (values == (const char *) NULL)
    return MagickFail;;

  options.red_enabled        = MagickFalse;
  options.green_enabled      = MagickFalse;
  options.blue_enabled       = MagickFalse;
  options.opacity_enabled    = MagickFalse;

  options.values.red       = -1.0;
  options.values.green     = -1.0;
  options.values.blue      = -1.0;
  options.values.opacity   = -1.0;
  count=sscanf(values,"%lf%*[/,%%]%lf%*[/,%%]%lf%*[/,%%]%lf",
               &options.values.red,
               &options.values.green,
               &options.values.blue,
               &options.values.opacity);
  
  if ((count > 3) && (options.values.opacity >= 0.0))
    options.opacity_enabled = MagickTrue;
  if ((count > 2) && (options.values.blue >= 0.0))
    options.blue_enabled = MagickTrue;
  if ((count > 1) && (options.values.green >= 0.0))
    options.green_enabled = MagickTrue;
  if ((count > 0) && (options.values.red >= 0.0))
    options.red_enabled = MagickTrue;

  if (strchr(values,'%') != (char *) NULL)
    {
      if (options.red_enabled)
        options.values.red     *= MaxRGB/100.0;
      if (options.green_enabled)
        options.values.green   *= MaxRGB/100.0;
      if (options.blue_enabled)
        options.values.blue    *= MaxRGB/100.0;
      if (options.opacity_enabled)
        options.values.opacity *= MaxRGB/100.0;
    }

  status=MagickPass;

  if ((IsRGBColorspace(image->colorspace)) &&
      ((count == 1) ||
       ((options.values.red == options.values.green) &&
        (options.values.green == options.values.blue))))
    {
      /*
        Apply operation to all channels in gray or RGB space.
      */
      if (IsGrayColorspace(image->colorspace))
        status=QuantumOperatorImage(image,GrayChannel,quantum_operator,
                                    options.values.red,&image->exception);
      else
        status=QuantumOperatorImage(image,AllChannels,quantum_operator,
                                    options.values.red,&image->exception);
    }
  else
    {
      /*
        Apply operator to individual RGB(A) channels.
      */
      if ((MagickPass == status) && (options.red_enabled))
        {
          status=QuantumOperatorImage(image,RedChannel,quantum_operator,
                                      options.values.red,&image->exception);
        }

      if ((MagickPass == status) && (options.green_enabled))
        {
          status=QuantumOperatorImage(image,GreenChannel,quantum_operator,
                                      options.values.green,&image->exception);
        }

      if ((MagickPass == status) && (options.blue_enabled))
        {
          status=QuantumOperatorImage(image,BlueChannel,quantum_operator,
                                      options.values.blue,&image->exception);
        }

      if ((MagickPass == status) && (options.opacity_enabled))
        {
          status=QuantumOperatorImage(image,OpacityChannel,quantum_operator,
                                      options.values.opacity,&image->exception);
        }
    }

  if ((MagickPass == status) && (options.opacity_enabled))
    {
      status=QuantumOperatorImage(image,OpacityChannel,quantum_operator,
                                  options.values.opacity,&image->exception);
    }

  return status;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   Q u a n t u m O p e r a t o r R e g i o n I m a g e                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  QuantumOperatorRegionImage() performs the requested arithmetic,
%  bitwise-logical, or value operation on the selected channels of
%  the image over the specified region. The AllChannels channel option
%  operates on all color channels whereas the GrayChannel channel option
%  treats the color channels as a grayscale intensity.
%
%  These operations are on the DirectClass pixels of the image and do not
%  update pixel indexes or colormap.
%
%  The format of the QuantumOperatorRegionImage method is:
%
%      MagickPassFail QuantumOperatorRegionImage(Image *image,
%        long x, long y, unsigned long columns, unsigned long rows,
%        ChannelType channel, QuantumOperator quantum_operator,
%        double rvalue)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o channel: Channel to operate on (RedChannel, CyanChannel,
%        GreenChannel, MagentaChannel, BlueChannel, YellowChannel,
%        OpacityChannel, BlackChannel, MatteChannel, AllChannels,
%        GrayChannel).  The AllChannels type only updates color
%        channels.  The GrayChannel type treats the color channels
%        as if they represent an intensity.
%
%    o x: Ordinate of left row of region.
%
%    o y: Orginate of top column of region.
%
%    o columns: Width of region.
%
%    o rows: Height of region.
%
%    o quantum_operator: Operator to use (AddQuantumOp,AndQuantumOp,
%        AssignQuantumOp, DepthQuantumOp, DivideQuantumOp, GammaQuantumOp,
%        LShiftQuantumOp, MultiplyQuantumOp,  NegateQuantumOp,
%        NoiseGaussianQuantumOp, NoiseImpulseQuantumOp,
%        NoiseLaplacianQuantumOp, NoiseMultiplicativeQuantumOp,
%        NoisePoissonQuantumOp, NoiseUniformQuantumOp, OrQuantumOp,
%        RShiftQuantumOp, SubtractQuantumOp, ThresholdBlackQuantumOp,
%        ThresholdQuantumOp, ThresholdWhiteQuantumOp, XorQuantumOp).
%
%    o rvalue: Operator argument.
%
%    o exception: Updated with error description.
%
*/

#define ApplyArithmeticOperator(lvalue,op,rvalue)       \
{                                                       \
    double                                              \
      result;                                           \
                                                        \
    result=(double) lvalue op (double) rvalue;          \
    lvalue=RoundDoubleToQuantum(result);                \
}

static MagickPassFail
QuantumAddCB(void *mutable_data,
             const void *immutable_data,
             Image *image,
             PixelPacket *pixels,
             IndexPacket *indexes,
             const long npixels,
             ExceptionInfo *exception)
{
  const QuantumImmutableContext
    *context=(const QuantumImmutableContext *) immutable_data;

  register long
    i;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      for (i=0; i < npixels; i++)
        ApplyArithmeticOperator(pixels[i].red,+,context->double_value);
      break;
    case GreenChannel:
    case MagentaChannel:
      for (i=0; i < npixels; i++)
        ApplyArithmeticOperator(pixels[i].green,+,context->double_value);
      break;
    case BlueChannel:
    case YellowChannel:
      for (i=0; i < npixels; i++)
        ApplyArithmeticOperator(pixels[i].blue,+,context->double_value);
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      for (i=0; i < npixels; i++)
        ApplyArithmeticOperator(pixels[i].opacity,+,context->double_value);
      break;
    case UndefinedChannel:
    case AllChannels:
      for (i=0; i < npixels; i++)
        {
          ApplyArithmeticOperator(pixels[i].red,+,context->double_value);
          ApplyArithmeticOperator(pixels[i].green,+,context->double_value);
          ApplyArithmeticOperator(pixels[i].blue,+,context->double_value);
        }
      break;
    case GrayChannel:
      for (i=0; i < npixels; i++)
        {
          Quantum
            intensity;
          
          intensity = PixelIntensity(&pixels[i]);
          ApplyArithmeticOperator(intensity,+,context->double_value);
          pixels[i].red = pixels[i].green = pixels[i].blue = intensity;
        }
      break;
    }

  return (MagickPass);
}
static MagickPassFail
QuantumAndCB(void *mutable_data,
             const void *immutable_data,
             Image *image,
             PixelPacket *pixels,
             IndexPacket *indexes,
             const long npixels,
             ExceptionInfo *exception)
{
  const QuantumImmutableContext
    *context=(const QuantumImmutableContext *) immutable_data;

  register long
    i;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      for (i=0; i < npixels; i++)
        pixels[i].red &= context->quantum_value;
      break;
    case GreenChannel:
    case MagentaChannel:
      for (i=0; i < npixels; i++)
        pixels[i].green &= context->quantum_value;
      break;
    case BlueChannel:
    case YellowChannel:
      for (i=0; i < npixels; i++)
        pixels[i].blue &= context->quantum_value;
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      for (i=0; i < npixels; i++)
        pixels[i].opacity &= context->quantum_value;
      break;
    case UndefinedChannel:
    case AllChannels:
      for (i=0; i < npixels; i++)
        {
          pixels[i].red &= context->quantum_value;
          pixels[i].green &= context->quantum_value;
          pixels[i].blue &= context->quantum_value;
        }
      break;
    case GrayChannel:
      for (i=0; i < npixels; i++)
        {
          Quantum
            intensity;

          intensity = PixelIntensity(&pixels[i]);
          intensity &= context->quantum_value;
          pixels[i].red = pixels[i].green = pixels[i].blue = intensity;
        }
      break;
    }

  return (MagickPass);
}
static MagickPassFail
QuantumAssignCB(void *mutable_data,
                const void *immutable_data,
                Image *image,
                PixelPacket *pixels,
                IndexPacket *indexes,
                const long npixels,
                ExceptionInfo *exception)
{
  const QuantumImmutableContext
    *context=(const QuantumImmutableContext *) immutable_data;

  register long
    i;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      for (i=0; i < npixels; i++)
        pixels[i].red = context->quantum_value;
      break;
    case GreenChannel:
    case MagentaChannel:
      for (i=0; i < npixels; i++)
        pixels[i].green = context->quantum_value;
      break;
    case BlueChannel:
    case YellowChannel:
      for (i=0; i < npixels; i++)
        pixels[i].blue = context->quantum_value;
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      for (i=0; i < npixels; i++)
        pixels[i].opacity = context->quantum_value;
      break;
    case UndefinedChannel:
    case AllChannels:
      for (i=0; i < npixels; i++)
        {
          pixels[i].red = context->quantum_value;
          pixels[i].green = context->quantum_value;
          pixels[i].blue = context->quantum_value;
        }
      break;
    case GrayChannel:
      for (i=0; i < npixels; i++)
        {
          pixels[i].red = pixels[i].green = pixels[i].blue =
            context->quantum_value;
        }
      break;
    }

  return (MagickPass);
}
#define ApplyChannelDepth(parameter)                                    \
  {                                                                     \
    for (i=0; i < npixels; i++)                                         \
      parameter=scale*((parameter)/scale);                              \
  }
#if MaxRGB > MaxMap
#  define CrushChannelDepth(parameter) (scale*((parameter)/scale))
#else
#  define CrushChannelDepth(parameter) (mutable_context->channel_lut[ScaleQuantumToMap(parameter)])
#endif
static MagickPassFail
QuantumDepthCB(void *mutable_data,
               const void *immutable_data,
               Image *image,
               PixelPacket *pixels,
               IndexPacket *indexes,
               const long npixels,
               ExceptionInfo *exception)
{
  QuantumMutableContext
    *mutable_context=(QuantumMutableContext *) mutable_data;

  const QuantumImmutableContext
    *immutable_context=(const QuantumImmutableContext *) immutable_data;

  unsigned int
    depth;

  register unsigned int
    scale;

  register long
    i;

  MagickPassFail
    status=MagickPass;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(exception);

  depth=immutable_context->quantum_value;
  if (depth < 1)
    depth=1;
  else if (depth > QuantumDepth)
    depth=QuantumDepth;

  if (depth < QuantumDepth)
    {
      scale=MaxRGB / (MaxRGB >> (QuantumDepth-depth));

      /*
        Build LUT for Q8 and Q16 builds
      */
#if MaxRGB <= MaxMap
#  if defined(HAVE_OPENMP)
#    pragma omp critical (GM_QuantumDepthCB)
#  endif
      if (mutable_context->channel_lut == (Quantum *) NULL)
        {
          mutable_context->channel_lut=MagickAllocateArray(Quantum *, MaxMap+1,sizeof(Quantum));
          if (mutable_context->channel_lut == (Quantum *) NULL)
            status=MagickFail;
          
          if (mutable_context->channel_lut != (Quantum *) NULL)
            {
              for (i=0; i <= (long) MaxMap; i++)
                mutable_context->channel_lut[i] = scale*(i/scale);
            }
        }
#else
      ARG_NOT_USED(*mutable_context);
#endif

      if (MagickFail == status)
        return status;

      switch (immutable_context->channel)
        {
        case RedChannel:
        case CyanChannel:
          for (i=0; i < npixels; i++)
            pixels[i].red=CrushChannelDepth(pixels[i].red);
          break;
        case GreenChannel:
        case MagentaChannel:
          for (i=0; i < npixels; i++)
            pixels[i].green=CrushChannelDepth(pixels[i].green);
          break;
        case BlueChannel:
        case YellowChannel:
          for (i=0; i < npixels; i++)
            pixels[i].blue=CrushChannelDepth(pixels[i].blue);
          break;
        case MatteChannel:
        case OpacityChannel:
          if (image->colorspace == CMYKColorspace)
            for (i=0; i < npixels; i++)
              indexes[i]=CrushChannelDepth(indexes[i]);
          else
            for (i=0; i < npixels; i++)
              pixels[i].opacity=CrushChannelDepth(pixels[i].opacity);
          break;
        case BlackChannel:
          for (i=0; i < npixels; i++)
            pixels[i].opacity=CrushChannelDepth(pixels[i].opacity);
          break;
        case UndefinedChannel:
        case AllChannels:
          for (i=0; i < npixels; i++)
            {
              pixels[i].red=CrushChannelDepth(pixels[i].red);
              pixels[i].green=CrushChannelDepth(pixels[i].green);
              pixels[i].blue=CrushChannelDepth(pixels[i].blue);
              if (image->colorspace == CMYKColorspace)
                pixels[i].opacity=CrushChannelDepth(pixels[i].opacity);
            }
          break;
        case GrayChannel:
          for (i=0; i < npixels; i++)
            {
              Quantum
                intensity;
                
              intensity = PixelIntensity(&pixels[i]);
              intensity=CrushChannelDepth(intensity);
              pixels[i].red = pixels[i].green = pixels[i].blue = intensity;
            }
          break;
        }
    }
  
  return MagickPass;
}
static MagickPassFail
QuantumDivideCB(void *mutable_data,
                const void *immutable_data,
                Image *image,
                PixelPacket *pixels,
                IndexPacket *indexes,
                const long npixels,
                ExceptionInfo *exception)
{
  const QuantumImmutableContext
    *context=(const QuantumImmutableContext *) immutable_data;
  
  register long
    i;
  
  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);
  
  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      for (i=0; i < npixels; i++)
        ApplyArithmeticOperator(pixels[i].red,/,context->double_value);
      break;
    case GreenChannel:
    case MagentaChannel:
      for (i=0; i < npixels; i++)
        ApplyArithmeticOperator(pixels[i].green,/,context->double_value);
      break;
    case BlueChannel:
    case YellowChannel:
      for (i=0; i < npixels; i++)
        ApplyArithmeticOperator(pixels[i].blue,/,context->double_value);
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      for (i=0; i < npixels; i++)
        ApplyArithmeticOperator(pixels[i].opacity,/,context->double_value);
      break;
    case UndefinedChannel:
    case AllChannels:
      for (i=0; i < npixels; i++)
        {
          ApplyArithmeticOperator(pixels[i].red,/,context->double_value);
          ApplyArithmeticOperator(pixels[i].green,/,context->double_value);
          ApplyArithmeticOperator(pixels[i].blue,/,context->double_value);
        }
      break;
    case GrayChannel:
      for (i=0; i < npixels; i++)
        {
          Quantum
            intensity;

          intensity = PixelIntensity(&pixels[i]);
          ApplyArithmeticOperator(intensity,/,context->double_value);
          pixels[i].red = pixels[i].green = pixels[i].blue = intensity;
        }
      break;
    }

  return (MagickPass);
}
#if MaxRGB > MaxMap
#  define GammaAdjustQuantum(quantum) (MaxRGBDouble*pow(quantum/MaxRGBDouble,1.0/immutable_context->double_value)+0.5)
#else
#  define GammaAdjustQuantum(quantum) (mutable_context->channel_lut[ScaleQuantumToMap(quantum)])
#endif
static MagickPassFail
QuantumGammaCB(void *mutable_data,
               const void *immutable_data,
               Image *image,
               PixelPacket *pixels,
               IndexPacket *indexes,
               const long npixels,
               ExceptionInfo *exception)
{
  QuantumMutableContext
    *mutable_context=(QuantumMutableContext *) mutable_data;

  const QuantumImmutableContext
    *immutable_context=(const QuantumImmutableContext *) immutable_data;
  
  register long
    i;

  MagickPassFail
    status=MagickPass;
  
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  /*
    Build LUT for Q8 and Q16 builds
  */
#if MaxRGB <= MaxMap
#  if defined(HAVE_OPENMP)
#    pragma omp critical (GM_QuantumGammaCB)
#  endif
  if (mutable_context->channel_lut == (Quantum *) NULL)
    {
      mutable_context->channel_lut=MagickAllocateArray(Quantum *, MaxMap+1,sizeof(Quantum));
      if (mutable_context->channel_lut == (Quantum *) NULL)
        status=MagickFail;
      
      if (mutable_context->channel_lut != (Quantum *) NULL)
        {
          for (i=0; i <= (long) MaxMap; i++)
            mutable_context->channel_lut[i] =
              ScaleMapToQuantum(MaxMap*pow((double) i/MaxMap,
                                           1.0/immutable_context->double_value));
        }
    }
#else
  ARG_NOT_USED(*mutable_context);
#endif
  if (MagickFail == status)
    return status;

  switch (immutable_context->channel)
    {
    case RedChannel:
    case CyanChannel:
      for (i=0; i < npixels; i++)
        pixels[i].red=GammaAdjustQuantum(pixels[i].red);
      break;
    case GreenChannel:
    case MagentaChannel:
      for (i=0; i < npixels; i++)
        pixels[i].green=GammaAdjustQuantum(pixels[i].green);
      break;
    case BlueChannel:
    case YellowChannel:
      for (i=0; i < npixels; i++)
        pixels[i].blue=GammaAdjustQuantum(pixels[i].blue);
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      for (i=0; i < npixels; i++)
        pixels[i].opacity=GammaAdjustQuantum(pixels[i].opacity);
      break;
    case UndefinedChannel:
    case AllChannels:
      for (i=0; i < npixels; i++)
        {
          pixels[i].red=GammaAdjustQuantum(pixels[i].red);
          pixels[i].green=GammaAdjustQuantum(pixels[i].green);
          pixels[i].blue=GammaAdjustQuantum(pixels[i].blue);
        }
      break;
    case GrayChannel:
      for (i=0; i < npixels; i++)
        {
          Quantum
            intensity;

          intensity = PixelIntensity(&pixels[i]);
          intensity = GammaAdjustQuantum(intensity);
          pixels[i].red = pixels[i].green = pixels[i].blue = intensity;
        }
      break;
    }

  return status;
}
static MagickPassFail
QuantumNegateCB(void *mutable_data,
                const void *immutable_data,
                Image *image,
                PixelPacket *pixels,
                IndexPacket *indexes,
                const long npixels,
                ExceptionInfo *exception)
{
  const QuantumImmutableContext
    *context=(const QuantumImmutableContext *) immutable_data;

  register long
    i;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);
  
  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      for (i=0; i < npixels; i++)
        pixels[i].red=MaxRGB-pixels[i].red;
      break;
    case GreenChannel:
    case MagentaChannel:
      for (i=0; i < npixels; i++)
        pixels[i].green=MaxRGB-pixels[i].green;
      break;
    case BlueChannel:
    case YellowChannel:
      for (i=0; i < npixels; i++)
        pixels[i].blue=MaxRGB-pixels[i].blue;
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      for (i=0; i < npixels; i++)
        pixels[i].opacity=MaxRGB-pixels[i].opacity;
      break;
    case UndefinedChannel:
    case AllChannels:
      for (i=0; i < npixels; i++)
        {
          pixels[i].red=MaxRGB-pixels[i].red;
          pixels[i].green=MaxRGB-pixels[i].green;
          pixels[i].blue=MaxRGB-pixels[i].blue;
        }
      break;
    case GrayChannel:
      for (i=0; i < npixels; i++)
        {
          Quantum
            intensity;

          intensity = PixelIntensity(&pixels[i]);
          intensity = MaxRGB-intensity;
          pixels[i].red = pixels[i].green = pixels[i].blue = intensity;
        }
      break;
    }

  return (MagickPass);
}
/* log(quantum*value+1)/log(value+1) */
#if MaxRGB > MaxMap
#  define LogAdjustQuantum(quantum) \
  ((MaxRGBDouble*log((quantum/MaxRGBDouble)*immutable_context->double_value+1.0)/ \
    log(immutable_context->double_value+1.0))+0.5)
#else
#  define LogAdjustQuantum(quantum) (mutable_context->channel_lut[ScaleQuantumToMap(quantum)])
#endif
static MagickPassFail
QuantumLogCB(void *mutable_data,
             const void *immutable_data,
             Image *image,
             PixelPacket *pixels,
             IndexPacket *indexes,
             const long npixels,
             ExceptionInfo *exception)
{
  QuantumMutableContext
    *mutable_context=(QuantumMutableContext *) mutable_data;

  const QuantumImmutableContext
    *immutable_context=(const QuantumImmutableContext *) immutable_data;
  
  register long
    i;

  MagickPassFail
    status=MagickPass;
  
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  /*
    Build LUT for Q8 and Q16 builds
  */
#if MaxRGB <= MaxMap
#  if defined(HAVE_OPENMP)
#    pragma omp critical (GM_QuantumLogCB)
#  endif
  if (mutable_context->channel_lut == (Quantum *) NULL)
    {
      mutable_context->channel_lut=MagickAllocateArray(Quantum *, MaxMap+1,sizeof(Quantum));
      if (mutable_context->channel_lut == (Quantum *) NULL)
        status=MagickFail;
      
      if (mutable_context->channel_lut != (Quantum *) NULL)
        {
          for (i=0; i <= (long) MaxMap; i++)
            {
              double
                value;

              value=MaxRGBDouble*(log((ScaleMapToQuantum(i)/MaxRGBDouble)*
                                     immutable_context->double_value+1.0)/
                                  log(immutable_context->double_value+1.0));
              mutable_context->channel_lut[i] = RoundDoubleToQuantum(value);
            }
        }
    }
#else
  ARG_NOT_USED(*mutable_context);
#endif
  if (MagickFail == status)
    return status;

  switch (immutable_context->channel)
    {
    case RedChannel:
    case CyanChannel:
      for (i=0; i < npixels; i++)
        pixels[i].red=LogAdjustQuantum(pixels[i].red);
      break;
    case GreenChannel:
    case MagentaChannel:
      for (i=0; i < npixels; i++)
        pixels[i].green=LogAdjustQuantum(pixels[i].green);
      break;
    case BlueChannel:
    case YellowChannel:
      for (i=0; i < npixels; i++)
        pixels[i].blue=LogAdjustQuantum(pixels[i].blue);
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      for (i=0; i < npixels; i++)
        pixels[i].opacity=LogAdjustQuantum(pixels[i].opacity);
      break;
    case UndefinedChannel:
    case AllChannels:
      for (i=0; i < npixels; i++)
        {
          pixels[i].red=LogAdjustQuantum(pixels[i].red);
          pixels[i].green=LogAdjustQuantum(pixels[i].green);
          pixels[i].blue=LogAdjustQuantum(pixels[i].blue);
        }
      break;
    case GrayChannel:
      for (i=0; i < npixels; i++)
        {
          Quantum
            intensity;

          intensity = PixelIntensity(&pixels[i]);
          intensity = LogAdjustQuantum(intensity);
          pixels[i].red = pixels[i].green = pixels[i].blue = intensity;
        }
      break;
    }

  return status;
}
static MagickPassFail
QuantumLShiftCB(void *mutable_data,
                const void *immutable_data,
                Image *image,
                PixelPacket *pixels,
                IndexPacket *indexes,
                const long npixels,
                ExceptionInfo *exception)
{
  const QuantumImmutableContext
    *context=(const QuantumImmutableContext *) immutable_data;

  register long
    i;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      for (i=0; i < npixels; i++)
        pixels[i].red <<= context->quantum_value;
      break;
    case GreenChannel:
    case MagentaChannel:
      for (i=0; i < npixels; i++)
        pixels[i].green <<= context->quantum_value;
      break;
    case BlueChannel:
    case YellowChannel:
      for (i=0; i < npixels; i++)
        pixels[i].blue <<= context->quantum_value;
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      for (i=0; i < npixels; i++)
        pixels[i].opacity <<= context->quantum_value;
      break;
    case UndefinedChannel:
    case AllChannels:
      for (i=0; i < npixels; i++)
        {
          pixels[i].red <<= context->quantum_value;
          pixels[i].green <<= context->quantum_value;
          pixels[i].blue <<= context->quantum_value;
        }
      break;
    case GrayChannel:
      for (i=0; i < npixels; i++)
        {
          Quantum
            intensity;

          intensity = PixelIntensity(&pixels[i]);
          intensity <<= context->quantum_value;
          pixels[i].red = pixels[i].green = pixels[i].blue = intensity;
        }
      break;
    }

  return (MagickPass);
}
static MagickPassFail
QuantumMaxCB(void *mutable_data,
             const void *immutable_data,
             Image *image,
             PixelPacket *pixels,
             IndexPacket *indexes,
             const long npixels,
             ExceptionInfo *exception)
{
  const QuantumImmutableContext
    *context=(const QuantumImmutableContext *) immutable_data;

  register long
    i;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      for (i=0; i < npixels; i++)
        if (context->quantum_value > pixels[i].red)
          pixels[i].red = context->quantum_value;
      break;
    case GreenChannel:
    case MagentaChannel:
      for (i=0; i < npixels; i++)
        if (context->quantum_value > pixels[i].green)
          pixels[i].green = context->quantum_value;
      break;
    case BlueChannel:
    case YellowChannel:
      for (i=0; i < npixels; i++)
        if (context->quantum_value > pixels[i].blue)
          pixels[i].blue = context->quantum_value;
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      for (i=0; i < npixels; i++)
        if (context->quantum_value > pixels[i].opacity)
          pixels[i].opacity = context->quantum_value;
      break;
    case UndefinedChannel:
    case AllChannels:
      for (i=0; i < npixels; i++)
        {
          if (context->quantum_value > pixels[i].red)
            pixels[i].red = context->quantum_value;
          if (context->quantum_value > pixels[i].green)
            pixels[i].green = context->quantum_value;
          if (context->quantum_value > pixels[i].blue)
            pixels[i].blue = context->quantum_value;
        }
      break;
    case GrayChannel:
      for (i=0; i < npixels; i++)
        {
          Quantum
            intensity;

          intensity = PixelIntensity(&pixels[i]);
          if (context->quantum_value > intensity)
            intensity = context->quantum_value;
          pixels[i].red = pixels[i].green = pixels[i].blue = intensity;
        }
      break;
    }

  return (MagickPass);
}
static MagickPassFail
QuantumMinCB(void *mutable_data,
             const void *immutable_data,
             Image *image,
             PixelPacket *pixels,
             IndexPacket *indexes,
             const long npixels,
             ExceptionInfo *exception)
{
  const QuantumImmutableContext
    *context=(const QuantumImmutableContext *) immutable_data;

  register long
    i;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      for (i=0; i < npixels; i++)
        if (context->quantum_value < pixels[i].red)
          pixels[i].red = context->quantum_value;
      break;
    case GreenChannel:
    case MagentaChannel:
      for (i=0; i < npixels; i++)
        if (context->quantum_value < pixels[i].green)
          pixels[i].green = context->quantum_value;
      break;
    case BlueChannel:
    case YellowChannel:
      for (i=0; i < npixels; i++)
        if (context->quantum_value < pixels[i].blue)
          pixels[i].blue = context->quantum_value;
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      for (i=0; i < npixels; i++)
        if (context->quantum_value < pixels[i].opacity)
          pixels[i].opacity = context->quantum_value;
      break;
    case UndefinedChannel:
    case AllChannels:
      for (i=0; i < npixels; i++)
        {
          if (context->quantum_value < pixels[i].red)
            pixels[i].red = context->quantum_value;
          if (context->quantum_value < pixels[i].green)
            pixels[i].green = context->quantum_value;
          if (context->quantum_value < pixels[i].blue)
            pixels[i].blue = context->quantum_value;
        }
      break;
    case GrayChannel:
      for (i=0; i < npixels; i++)
        {
          Quantum
            intensity;

          intensity = PixelIntensity(&pixels[i]);
          if (context->quantum_value < intensity)
            intensity = context->quantum_value;
          pixels[i].red = pixels[i].green = pixels[i].blue = intensity;
        }
      break;
    }

  return (MagickPass);
}
static MagickPassFail
QuantumMultiplyCB(void *mutable_data,
                  const void *immutable_data,
                  Image *image,
                  PixelPacket *pixels,
                  IndexPacket *indexes,
                  const long npixels,
                  ExceptionInfo *exception)
{
  const QuantumImmutableContext
    *context=(const QuantumImmutableContext *) immutable_data;

  register long
    i;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      for (i=0; i < npixels; i++)
        ApplyArithmeticOperator(pixels[i].red,*,context->double_value);
      break;
    case GreenChannel:
    case MagentaChannel:
      for (i=0; i < npixels; i++)
        ApplyArithmeticOperator(pixels[i].green,*,context->double_value);
      break;
    case BlueChannel:
    case YellowChannel:
      for (i=0; i < npixels; i++)
        ApplyArithmeticOperator(pixels[i].blue,*,context->double_value);
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      for (i=0; i < npixels; i++)
        ApplyArithmeticOperator(pixels[i].opacity,*,context->double_value);
      break;
    case UndefinedChannel:
    case AllChannels:
      for (i=0; i < npixels; i++)
        {
          ApplyArithmeticOperator(pixels[i].red,*,context->double_value);
          ApplyArithmeticOperator(pixels[i].green,*,context->double_value);
          ApplyArithmeticOperator(pixels[i].blue,*,context->double_value);
        }
      break;
    case GrayChannel:
      for (i=0; i < npixels; i++)
        {
          Quantum
            intensity;

          intensity = PixelIntensity(&pixels[i]);
          ApplyArithmeticOperator(intensity,*,context->double_value);
          pixels[i].red = pixels[i].green = pixels[i].blue = intensity;
        }
      break;
    }

  return (MagickPass);
}

static inline Quantum
GenerateQuantumNoise(const Quantum quantum,const NoiseType noise_type,
                     const double factor,MagickRandomKernel *kernel)
{
  double
    value;

  value = (double) quantum+
    factor*GenerateDifferentialNoise((double) quantum,noise_type,kernel);
  return RoundDoubleToQuantum(value);
}

static MagickPassFail
QuantumNoiseCB(void *mutable_data,
               const void *immutable_data,
               Image *image,
               PixelPacket *pixels,
               IndexPacket *indexes,
               const long npixels,
               ExceptionInfo *exception,
               const NoiseType noise_type
               )
{
  const QuantumImmutableContext
    *context=(const QuantumImmutableContext *) immutable_data;

  register long
    i;

  double
    factor;

  MagickRandomKernel
    *kernel;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  kernel=AcquireMagickRandomKernel();
  factor=context->double_value/MaxRGBDouble;

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      for (i=0; i < npixels; i++)
        pixels[i].red = GenerateQuantumNoise(pixels[i].red,noise_type,factor,kernel);
      break;
    case GreenChannel:
    case MagentaChannel:
      for (i=0; i < npixels; i++)
        pixels[i].green = GenerateQuantumNoise(pixels[i].green,noise_type,factor,kernel);
      break;
    case BlueChannel:
    case YellowChannel:
      for (i=0; i < npixels; i++)
        pixels[i].blue = GenerateQuantumNoise(pixels[i].blue,noise_type,factor,kernel);
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      for (i=0; i < npixels; i++)
        pixels[i].opacity = GenerateQuantumNoise(pixels[i].opacity,noise_type,factor,kernel);
      break;
    case UndefinedChannel:
    case AllChannels:
      for (i=0; i < npixels; i++)
        {
          pixels[i].red   = GenerateQuantumNoise(pixels[i].red,noise_type,factor,kernel);
          pixels[i].green = GenerateQuantumNoise(pixels[i].green,noise_type,factor,kernel);
          pixels[i].blue  = GenerateQuantumNoise(pixels[i].blue,noise_type,factor,kernel);
        }
      break;
    case GrayChannel:
      for (i=0; i < npixels; i++)
        {
          Quantum
            intensity;

          intensity = PixelIntensity(&pixels[i]);
          pixels[i].red = pixels[i].green = pixels[i].blue =
            GenerateQuantumNoise(intensity,noise_type,factor,kernel);
        }
      break;
    }

  return (MagickPass);
}
static MagickPassFail
QuantumNoiseGaussianCB(void *mutable_data,
                       const void *immutable_data,
                       Image *image,
                       PixelPacket *pixels,
                       IndexPacket *indexes,
                       const long npixels,
                       ExceptionInfo *exception)
{
  return
    QuantumNoiseCB(mutable_data,immutable_data,image,pixels,indexes,npixels,exception,GaussianNoise);
}
static MagickPassFail
QuantumNoiseImpulseCB(void *mutable_data,
                      const void *immutable_data,
                      Image *image,
                      PixelPacket *pixels,
                      IndexPacket *indexes,
                      const long npixels,
                      ExceptionInfo *exception)
{
  return
    QuantumNoiseCB(mutable_data,immutable_data,image,pixels,indexes,npixels,exception,ImpulseNoise);
}
static MagickPassFail
QuantumNoiseLaplacianCB(void *mutable_data,
                        const void *immutable_data,
                        Image *image,
                        PixelPacket *pixels,
                        IndexPacket *indexes,
                        const long npixels,
                        ExceptionInfo *exception)
{
  return
    QuantumNoiseCB(mutable_data,immutable_data,image,pixels,indexes,npixels,exception,LaplacianNoise);
}
static MagickPassFail
QuantumNoiseMultiplicativeCB(void *mutable_data,
                             const void *immutable_data,
                             Image *image,
                             PixelPacket *pixels,
                             IndexPacket *indexes,
                             const long npixels,
                             ExceptionInfo *exception)
{
  return
    QuantumNoiseCB(mutable_data,immutable_data,image,pixels,indexes,npixels,
                 exception,MultiplicativeGaussianNoise);
}
static MagickPassFail
QuantumNoisePoissonCB(void *mutable_data,
                      const void *immutable_data,
                      Image *image,
                      PixelPacket *pixels,
                      IndexPacket *indexes,
                      const long npixels,
                      ExceptionInfo *exception)
{
  return
    QuantumNoiseCB(mutable_data,immutable_data,image,pixels,indexes,npixels,exception,PoissonNoise);
}
static MagickPassFail
QuantumNoiseUniformCB(void *mutable_data,
                      const void *immutable_data,
                      Image *image,
                      PixelPacket *pixels,
                      IndexPacket *indexes,
                      const long npixels,
                      ExceptionInfo *exception)
{
  return
    QuantumNoiseCB(mutable_data,immutable_data,image,pixels,indexes,npixels,exception,UniformNoise);
}
static MagickPassFail
QuantumOrCB(void *mutable_data,
            const void *immutable_data,
            Image *image,
            PixelPacket *pixels,
            IndexPacket *indexes,
            const long npixels,
            ExceptionInfo *exception)
{
  const QuantumImmutableContext
    *context=(const QuantumImmutableContext *) immutable_data;

  register long
    i;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      for (i=0; i < npixels; i++)
        pixels[i].red |= context->quantum_value;
      break;
    case GreenChannel:
    case MagentaChannel:
      for (i=0; i < npixels; i++)
        pixels[i].green |= context->quantum_value;
      break;
    case BlueChannel:
    case YellowChannel:
      for (i=0; i < npixels; i++)
        pixels[i].blue |= context->quantum_value;
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      for (i=0; i < npixels; i++)
        pixels[i].opacity |= context->quantum_value;
      break;
    case UndefinedChannel:
    case AllChannels:
      for (i=0; i < npixels; i++)
        {
          pixels[i].red |= context->quantum_value;
          pixels[i].green |= context->quantum_value;
          pixels[i].blue |= context->quantum_value;
        }
      break;
    case GrayChannel:
      for (i=0; i < npixels; i++)
        {
          Quantum
            intensity;

          intensity = PixelIntensity(&pixels[i]);
          intensity |= context->quantum_value;
          pixels[i].red = pixels[i].green = pixels[i].blue = intensity;
        }
      break;
    }

  return (MagickPass);
}
#if MaxRGB > MaxMap
#  define PowAdjustQuantum(quantum) (MaxRGBDouble*pow(quantum/MaxRGBDouble,immutable_context->double_value)+0.5)
#else
#  define PowAdjustQuantum(quantum) (mutable_context->channel_lut[ScaleQuantumToMap(quantum)])
#endif
static MagickPassFail
QuantumPowCB(void *mutable_data,
             const void *immutable_data,
             Image *image,
             PixelPacket *pixels,
             IndexPacket *indexes,
             const long npixels,
             ExceptionInfo *exception)
{
  QuantumMutableContext
    *mutable_context=(QuantumMutableContext *) mutable_data;

  const QuantumImmutableContext
    *immutable_context=(const QuantumImmutableContext *) immutable_data;
  
  register long
    i;

  MagickPassFail
    status=MagickPass;
  
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  /*
    Build LUT for Q8 and Q16 builds
  */
#if MaxRGB <= MaxMap
#  if defined(HAVE_OPENMP)
#    pragma omp critical (GM_QuantumPowCB)
#  endif
  if (mutable_context->channel_lut == (Quantum *) NULL)
    {
      mutable_context->channel_lut=MagickAllocateArray(Quantum *, MaxMap+1,sizeof(Quantum));
      if (mutable_context->channel_lut == (Quantum *) NULL)
        status=MagickFail;
      
      if (mutable_context->channel_lut != (Quantum *) NULL)
        {
          for (i=0; i <= (long) MaxMap; i++)
            mutable_context->channel_lut[i] =
              ScaleMapToQuantum(MaxMap*pow((double) i/MaxMap,
                                           immutable_context->double_value));
        }
    }
#else
  ARG_NOT_USED(*mutable_context);
#endif
  if (MagickFail == status)
    return status;

  switch (immutable_context->channel)
    {
    case RedChannel:
    case CyanChannel:
      for (i=0; i < npixels; i++)
        pixels[i].red=PowAdjustQuantum(pixels[i].red);
      break;
    case GreenChannel:
    case MagentaChannel:
      for (i=0; i < npixels; i++)
        pixels[i].green=PowAdjustQuantum(pixels[i].green);
      break;
    case BlueChannel:
    case YellowChannel:
      for (i=0; i < npixels; i++)
        pixels[i].blue=PowAdjustQuantum(pixels[i].blue);
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      for (i=0; i < npixels; i++)
        pixels[i].opacity=PowAdjustQuantum(pixels[i].opacity);
      break;
    case UndefinedChannel:
    case AllChannels:
      for (i=0; i < npixels; i++)
        {
          pixels[i].red=PowAdjustQuantum(pixels[i].red);
          pixels[i].green=PowAdjustQuantum(pixels[i].green);
          pixels[i].blue=PowAdjustQuantum(pixels[i].blue);
        }
      break;
    case GrayChannel:
      for (i=0; i < npixels; i++)
        {
          Quantum
            intensity;

          intensity = PixelIntensity(&pixels[i]);
          intensity = PowAdjustQuantum(intensity);
          pixels[i].red = pixels[i].green = pixels[i].blue = intensity;
        }
      break;
    }

  return status;
}
static MagickPassFail
QuantumRShiftCB(void *mutable_data,
                const void *immutable_data,
                Image *image,
                PixelPacket *pixels,
                IndexPacket *indexes,
                const long npixels,
                ExceptionInfo *exception)
{
  const QuantumImmutableContext
    *context=(const QuantumImmutableContext *) immutable_data;

  register long
    i;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      for (i=0; i < npixels; i++)
        pixels[i].red >>= context->quantum_value;
      break;
    case GreenChannel:
    case MagentaChannel:
      for (i=0; i < npixels; i++)
        pixels[i].green >>= context->quantum_value;
      break;
    case BlueChannel:
    case YellowChannel:
      for (i=0; i < npixels; i++)
        pixels[i].blue >>= context->quantum_value;
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      for (i=0; i < npixels; i++)
        pixels[i].opacity >>= context->quantum_value;
      break;
    case UndefinedChannel:
    case AllChannels:
      for (i=0; i < npixels; i++)
        {
          pixels[i].red >>= context->quantum_value;
          pixels[i].green >>= context->quantum_value;
          pixels[i].blue >>= context->quantum_value;
        }
      break;
    case GrayChannel:
      for (i=0; i < npixels; i++)
        {
          Quantum
            intensity;

          intensity = PixelIntensity(&pixels[i]);
          intensity >>= context->quantum_value;
          pixels[i].red = pixels[i].green = pixels[i].blue = intensity;
        }
      break;
    }

  return (MagickPass);
}
static MagickPassFail
QuantumSubtractCB(void *mutable_data,
                  const void *immutable_data,
                  Image *image,
                  PixelPacket *pixels,
                  IndexPacket *indexes,
                  const long npixels,
                  ExceptionInfo *exception)
{
  const QuantumImmutableContext
    *context=(const QuantumImmutableContext *) immutable_data;

  register long
    i;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      for (i=0; i < npixels; i++)
        ApplyArithmeticOperator(pixels[i].red,-,context->double_value);
      break;
    case GreenChannel:
    case MagentaChannel:
      for (i=0; i < npixels; i++)
        ApplyArithmeticOperator(pixels[i].green,-,context->double_value);
      break;
    case BlueChannel:
    case YellowChannel:
      for (i=0; i < npixels; i++)
        ApplyArithmeticOperator(pixels[i].blue,-,context->double_value);
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      for (i=0; i < npixels; i++)
        ApplyArithmeticOperator(pixels[i].opacity,-,context->double_value);
      break;
    case UndefinedChannel:
    case AllChannels:
      for (i=0; i < npixels; i++)
        {
          ApplyArithmeticOperator(pixels[i].red,-,context->double_value);
          ApplyArithmeticOperator(pixels[i].green,-,context->double_value);
          ApplyArithmeticOperator(pixels[i].blue,-,context->double_value);
        }
      break;
    case GrayChannel:
      for (i=0; i < npixels; i++)
        {
          Quantum
            intensity;

          intensity = PixelIntensity(&pixels[i]);
          ApplyArithmeticOperator(intensity,-,context->double_value);
          pixels[i].red = pixels[i].green = pixels[i].blue = intensity;
        }
      break;
    }

  return (MagickPass);
}

static inline Quantum ApplyThresholdOperator(const Quantum quantum,
                                             const Quantum threshold)
{
  Quantum
    result;

  if (quantum > threshold)
    result=MaxRGB;
  else
    result=0U;

  return result;
}
static MagickPassFail
QuantumThresholdCB(void *mutable_data,
                   const void *immutable_data,
                   Image *image,
                   PixelPacket *pixels,
                   IndexPacket *indexes,
                   const long npixels,
                   ExceptionInfo *exception)
{
  const QuantumImmutableContext
    *context=(const QuantumImmutableContext *) immutable_data;

  register long
    i;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      for (i=0; i < npixels; i++)
        pixels[i].red = ApplyThresholdOperator(pixels[i].red,context->quantum_value);
      break;
    case GreenChannel:
    case MagentaChannel:
      for (i=0; i < npixels; i++)
        pixels[i].green = ApplyThresholdOperator(pixels[i].green,context->quantum_value);
      break;
    case BlueChannel:
    case YellowChannel:
      for (i=0; i < npixels; i++)
        pixels[i].blue = ApplyThresholdOperator(pixels[i].blue,context->quantum_value);
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      for (i=0; i < npixels; i++)
        pixels[i].opacity = ApplyThresholdOperator(pixels[i].opacity,context->quantum_value);
      break;
    case UndefinedChannel:
    case AllChannels:
    case GrayChannel:
      for (i=0; i < npixels; i++)
        {
          Quantum
            intensity;

          intensity = PixelIntensity(&pixels[i]);
          pixels[i].red = pixels[i].green = pixels[i].blue = 
            ApplyThresholdOperator(intensity,context->quantum_value);
        }
      break;
    }
  return (MagickPass);
}

static inline Quantum ApplyThresholdBlackOperator(const Quantum quantum,
                                                  const Quantum threshold)
{
  Quantum
    result;

  if (quantum < threshold)
    result=0U;
  else
    result=quantum;

  return result;
}
static MagickPassFail
QuantumThresholdBlackCB(void *mutable_data,
                        const void *immutable_data,
                        Image *image,
                        PixelPacket *pixels,
                        IndexPacket *indexes,
                        const long npixels,
                        ExceptionInfo *exception)
{
  const QuantumImmutableContext
    *context=(const QuantumImmutableContext *) immutable_data;

  register long
    i;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      for (i=0; i < npixels; i++)
        pixels[i].red = ApplyThresholdBlackOperator(pixels[i].red,context->quantum_value);
      break;
    case GreenChannel:
    case MagentaChannel:
      for (i=0; i < npixels; i++)
        pixels[i].green = ApplyThresholdBlackOperator(pixels[i].green,context->quantum_value);
      break;
    case BlueChannel:
    case YellowChannel:
      for (i=0; i < npixels; i++)
        pixels[i].blue = ApplyThresholdBlackOperator(pixels[i].blue,context->quantum_value);
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      for (i=0; i < npixels; i++)
        pixels[i].opacity = ApplyThresholdBlackOperator(pixels[i].opacity,context->quantum_value);
      break;
    case UndefinedChannel:
    case AllChannels:
      /*
        For the all-channels case we bend the rules a bit and only
        threshold to black if the computed intensity of the color
        channels is less than the threshold.  This allows black
        thresholding to work without causing a color shift.  If
        individual channels need to be thresholded, then per-channel
        thresholding will be required for each channel to be
        thresholded.
      */
      for (i=0; i < npixels; i++)
        {
          Quantum
            intensity;

          intensity = PixelIntensity(&pixels[i]);
          if (0U == ApplyThresholdBlackOperator(intensity,context->quantum_value))
            pixels[i].red=pixels[i].green=pixels[i].blue=0U;
        }
      break;
    case GrayChannel:
      for (i=0; i < npixels; i++)
        {
          Quantum
            intensity;

          intensity = PixelIntensity(&pixels[i]);
          pixels[i].red = pixels[i].green = pixels[i].blue =
            ApplyThresholdBlackOperator(intensity,context->quantum_value);
        }
      break;
    }
  return (MagickPass);
}

static inline Quantum ApplyThresholdWhiteOperator(const Quantum quantum,
                                                  const Quantum threshold)
{
  Quantum
    result;

  if (quantum > threshold)
    result=MaxRGB;
  else
    result=quantum;

  return result;
}
static MagickPassFail
QuantumThresholdWhiteCB(void *mutable_data,
                        const void *immutable_data,
                        Image *image,
                        PixelPacket *pixels,
                        IndexPacket *indexes,
                        const long npixels,
                        ExceptionInfo *exception)
{
  const QuantumImmutableContext
    *context=(const QuantumImmutableContext *) immutable_data;

  register long
    i;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      for (i=0; i < npixels; i++)
        pixels[i].red = ApplyThresholdWhiteOperator(pixels[i].red,context->quantum_value);
      break;
    case GreenChannel:
    case MagentaChannel:
      for (i=0; i < npixels; i++)
        pixels[i].green = ApplyThresholdWhiteOperator(pixels[i].green,context->quantum_value);
      break;
    case BlueChannel:
    case YellowChannel:
      for (i=0; i < npixels; i++)
        pixels[i].blue = ApplyThresholdWhiteOperator(pixels[i].blue,context->quantum_value);
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      for (i=0; i < npixels; i++)
        pixels[i].opacity = ApplyThresholdWhiteOperator(pixels[i].opacity,context->quantum_value);
      break;
    case UndefinedChannel:
    case AllChannels:
      /*
        For the all-channels case we bend the rules a bit and only
        threshold to white if the computed intensity of the color
        channels exceeds the threshold.  This allows white
        thresholding to work without causing a color shift.  If
        individual channels need to be thresholded, then per-channel
        thresholding will be required for each channel to be
        thresholded.
      */
      for (i=0; i < npixels; i++)
        {
          Quantum
            intensity;

          intensity = PixelIntensity(&pixels[i]);
          if (MaxRGB == ApplyThresholdWhiteOperator(intensity,context->quantum_value))
            pixels[i].red=pixels[i].green=pixels[i].blue=MaxRGB;
        }
      break;
    case GrayChannel:
      for (i=0; i < npixels; i++)
        {
          Quantum
            intensity;

          intensity = PixelIntensity(&pixels[i]);
          pixels[i].red = pixels[i].green = pixels[i].blue =
            ApplyThresholdWhiteOperator(intensity,context->quantum_value);
        }
      break;
    }
  return (MagickPass);
}

static MagickPassFail
QuantumXorCB(void *mutable_data,
             const void *immutable_data,
             Image *image,
             PixelPacket *pixels,
             IndexPacket *indexes,
             const long npixels,
             ExceptionInfo *exception)
{
  const QuantumImmutableContext
    *context=(const QuantumImmutableContext *) immutable_data;

  register long
    i;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      for (i=0; i < npixels; i++)
        pixels[i].red ^= context->quantum_value;
      break;
    case GreenChannel:
    case MagentaChannel:
      for (i=0; i < npixels; i++)
        pixels[i].green ^= context->quantum_value;
      break;
    case BlueChannel:
    case YellowChannel:
      for (i=0; i < npixels; i++)
        pixels[i].blue ^= context->quantum_value;
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      for (i=0; i < npixels; i++)
        pixels[i].opacity ^= context->quantum_value;
      break;
    case UndefinedChannel:
    case AllChannels:
      for (i=0; i < npixels; i++)
        {
          pixels[i].red ^= context->quantum_value;
          pixels[i].green ^= context->quantum_value;
          pixels[i].blue ^= context->quantum_value;
        }
      break;
    case GrayChannel:
      for (i=0; i < npixels; i++)
        {
          Quantum
            intensity;

          intensity = PixelIntensity(&pixels[i]);
          intensity ^= context->quantum_value;
          pixels[i].red = pixels[i].green = pixels[i].blue = intensity;
        }
      break;
    }
  return (MagickPass);
}
MagickExport MagickPassFail
QuantumOperatorRegionImage(Image *image,
                           const long x,const long y,
                           const unsigned long columns,
                           const unsigned long rows,
                           const ChannelType channel,
                           const QuantumOperator quantum_operator,
                           const double rvalue,
                           ExceptionInfo *exception)
{
  char
    description[MaxTextExtent];

  QuantumImmutableContext
    immutable_context;

  QuantumMutableContext
    mutable_context;

  MagickPassFail
    status = MagickFail;

  PixelIteratorMonoModifyCallback
    call_back = 0;

  image->storage_class=DirectClass;

  immutable_context.channel=channel;
  immutable_context.double_value=rvalue;
  immutable_context.quantum_value=RoundDoubleToQuantum(rvalue);

  mutable_context.channel_lut=(Quantum *) NULL;

  switch (quantum_operator)
    {
    case UndefinedQuantumOp:
      break;
    case AddQuantumOp:
      call_back=QuantumAddCB;
      break;
    case AndQuantumOp:
      call_back=QuantumAndCB;
      break;
    case AssignQuantumOp:
      call_back=QuantumAssignCB;
      break;
    case DivideQuantumOp:
      call_back=QuantumDivideCB;
      break;
    case LShiftQuantumOp:
      call_back=QuantumLShiftCB;
      break;
    case MultiplyQuantumOp:
      call_back=QuantumMultiplyCB;
      break;
    case OrQuantumOp:
      call_back=QuantumOrCB;
      break;
    case RShiftQuantumOp:
      call_back=QuantumRShiftCB;
      break;
    case SubtractQuantumOp:
      call_back=QuantumSubtractCB;
      break;
    case ThresholdQuantumOp:
      call_back=QuantumThresholdCB;
      break;
    case ThresholdBlackQuantumOp:
      call_back=QuantumThresholdBlackCB;
      break;
    case ThresholdWhiteQuantumOp:
      call_back=QuantumThresholdWhiteCB;
      break;
    case XorQuantumOp:
      call_back=QuantumXorCB;
      break;
    case NoiseGaussianQuantumOp:
      call_back=QuantumNoiseGaussianCB;
      break;
    case NoiseImpulseQuantumOp:
      call_back=QuantumNoiseImpulseCB;
      break;
    case NoiseLaplacianQuantumOp:
      call_back=QuantumNoiseLaplacianCB;
      break;
    case NoiseMultiplicativeQuantumOp:
      call_back=QuantumNoiseMultiplicativeCB;
      break;
    case NoisePoissonQuantumOp:
      call_back=QuantumNoisePoissonCB;
      break;
    case NoiseUniformQuantumOp:
      call_back=QuantumNoiseUniformCB;
      break;
    case NegateQuantumOp:
      call_back=QuantumNegateCB;
      break;
    case GammaQuantumOp:
      call_back=QuantumGammaCB;
      break;
    case DepthQuantumOp:
      call_back=QuantumDepthCB;
      break;
    case LogQuantumOp:
      call_back=QuantumLogCB;
      break;
    case MaxQuantumOp:
      call_back=QuantumMaxCB;
      break;
    case MinQuantumOp:
      call_back=QuantumMinCB;
      break;
    case PowQuantumOp:
      call_back=QuantumPowCB;
      break;
    }

  if (call_back)
    {
      FormatString(description,"[%%s] Apply operator '%s %g (%g%%%%)' to channel '%s'...",
                   QuantumOperatorToString(quantum_operator),rvalue,
                   ((rvalue/MaxRGBFloat)*100),
                   ChannelTypeToString(channel));
      status=PixelIterateMonoModify(call_back,
                                    NULL,
                                    description,
                                    &mutable_context,&immutable_context,x,y,columns,rows,
                                    image,exception);

      /*
        Free any channel LUT.
      */
      MagickFreeMemory(mutable_context.channel_lut);

      /*
        If we are assigning all the color channels in the entire image
        then set monochrome and grayscale flags.
      */
      if ((quantum_operator == AssignQuantumOp) &&
          (channel == AllChannels) && (x == 0) && (y == 0) &&
          (columns == image->columns) && (rows == image->rows))
        {
          image->is_monochrome=MagickTrue;
          image->is_grayscale=MagickTrue;
        }
    }
  return (status);
}
