/*
% Copyright (C) 2004 - 2010 GraphicsMagick Group
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%            CCCC  H   H   AAA   N   N  N   N  EEEEE  L                       %
%           C      H   H  A   A  NN  N  NN  N  E      L                       %
%           C      HHHHH  AAAAA  N N N  N N N  EEE    L                       %
%           C      H   H  A   A  N  NN  N  NN  E      L                       %
%            CCCC  H   H  A   A  N   N  N   N  EEEEE  LLLLL                   %
%                                                                             %
%                                                                             %
%                       Image Channel Operations                              %
%                                                                             %
%                                                                             %
%                           Software Design                                   %
%                           Bob Friesenhahn                                   %
%                             July  2004                                      %
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
#include "magick/channel.h"
#include "magick/enum_strings.h"
#include "magick/image.h"
#include "magick/operator.h"
#include "magick/pixel_iterator.h"
#include "magick/utility.h"

/*
    Verify that image colorspace is compatible with with requested
    channel type.  Only check mismatch between RGB and CMYK since user
    might intentionally export some obsure colorspace channel.  We
    don't silently convert between RGB and CMYK since there is no one
    correct transform, and the transform is lossy.
  */
static MagickPassFail ValidateChannelRequest(const ColorspaceType image_colorspace,
					     const ChannelType channel,
					     ExceptionInfo *exception)
{
  MagickPassFail
    status = MagickPass;

  switch(channel)
    {
    case CyanChannel:
    case MagentaChannel:
    case YellowChannel:
    case BlackChannel:
      if (image_colorspace != CMYKColorspace)
	status = MagickFail;
      break;
    case RedChannel:
    case GreenChannel:
    case BlueChannel:
      if (image_colorspace == CMYKColorspace)
	status = MagickFail;;
      break;
    default:
      {
      }
    }

  if (MagickFail == status)
    ThrowException3(exception,ImageError,UnableToHandleImageChannel,ImageColorspaceMismatch);

  return status;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%     C h a n n e l I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Transform an image so that the resulting image is a grayscale image
%  based on a specified image channel. The resulting image is returned in
%  the RGB colorspace. This function does not force or assume an input
%  image colorspace so it may be used to extract channels from images in
%  colorspaces other than RGB or CMYK. For example, if the image is currently
%  transformed to the HWB colorspace, the 'B' channel may be extracted by
%  specifying RedChannel as the ChannelType argument.
%
%  The format of the ChannelImage method is:
%
%      unsigned int ChannelImage(Image *image,const ChannelType channel)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o channel: Identify which channel to extract: Red, Cyan, Green, Magenta,
%    Blue, Yellow, or Opacity.
%
%
*/
static MagickPassFail
ChannelImagePixels(void *mutable_data,            /* User provided mutable data */
                   const void *immutable_data,    /* User provided immutable data */
                   Image *image,                  /* Modify image */
                   PixelPacket * restrict pixels, /* Pixel row */
                   IndexPacket * restrict indexes,/* Pixel row indexes */
                   const long npixels,            /* Number of pixels in row */
                   ExceptionInfo *exception)      /* Exception report */
{
  /*
    Transform image so that it only represents the specified channel.
  */
  ChannelType
    channel = *((const ChannelType *) immutable_data);

  register long
    i;  

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(exception);

  switch (channel)
    {
    case RedChannel:
    case CyanChannel:
      {
        for (i=0; i < npixels; i++)
          {
            pixels[i].green=pixels[i].red;
            pixels[i].blue=pixels[i].red;
            pixels[i].opacity=OpaqueOpacity;
          }
        break;
      }
    case GreenChannel:
    case MagentaChannel:
      {
        for (i=0; i < npixels; i++)
          {
            pixels[i].red=pixels[i].green;
            pixels[i].blue=pixels[i].green;
            pixels[i].opacity=OpaqueOpacity;
          }
        break;
      }
    case BlueChannel:
    case YellowChannel:
      {
        for (i=0; i < npixels; i++)
          {
            pixels[i].red=pixels[i].blue;
            pixels[i].green=pixels[i].blue;
            pixels[i].opacity=OpaqueOpacity;
          }
        break;
      }
    case MatteChannel:
    case OpacityChannel:
      {
        if (image->colorspace == CMYKColorspace)
          {
            for (i=0; i < npixels; i++)
              {
                pixels[i].red=indexes[i];
                pixels[i].green=indexes[i];
                pixels[i].blue=indexes[i];
                pixels[i].opacity=OpaqueOpacity;
              }
          }
        else
          {
            for (i=0; i < npixels; i++)
              {
                pixels[i].red=pixels[i].opacity;
                pixels[i].green=pixels[i].opacity;
                pixels[i].blue=pixels[i].opacity;
                pixels[i].opacity=OpaqueOpacity;
              }
          }
        image->matte=False;
        break;
      }
    case BlackChannel:
      {
        for (i=0; i < npixels; i++)
          {
            pixels[i].red=pixels[i].opacity;
            pixels[i].green=pixels[i].opacity;
            pixels[i].blue=pixels[i].opacity;
            pixels[i].opacity=OpaqueOpacity;
          }
        image->matte=False;
        break;
      }

    case UndefinedChannel:
    case AllChannels:
    case GrayChannel:
      {
        for (i=0; i < npixels; i++)
          {
            pixels[i].red=pixels[i].green=pixels[i].blue=PixelIntensity(&pixels[i]);
            pixels[i].opacity=OpaqueOpacity;
          }
        image->matte=False;
        break;
      }
    }

  return MagickPass;
}
MagickExport MagickPassFail ChannelImage(Image *image,const ChannelType channel)
{
  char
    progress_message[MaxTextExtent];

  ChannelType
    channel_type = channel;

  MagickPassFail
    status=MagickPass;

  /*
    Channel DirectClass packets.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  FormatString(progress_message,"[%%s] Extract %s channel...  ",
	       ChannelTypeToString(channel));

  /*
    Verify that image colorspace is compatible with with requested
    channel type.
  */
  if (ValidateChannelRequest(image->colorspace,channel,&image->exception)
      == MagickFail)
    return MagickFail;

  image->storage_class=DirectClass;
  status=PixelIterateMonoModify(ChannelImagePixels,
                                NULL,
                                progress_message,
                                NULL,&channel_type,0,0,image->columns,image->rows,
                                image,&image->exception);

  image->matte=MagickFalse;
  image->is_grayscale=MagickTrue;
  image->colorspace=RGBColorspace;

  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%     E x p o r t I m a g e C h a n n e l                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ExportImageChannel() exports a specified image channel as a new image.
%
%  The format of the ExportImageChannel method is:
%
%      Image *ExportImageChannel(const Image *image,
%                                const ChannelType channel,
%                                ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The source image.
%
%    o channel: The image channel to export
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
#define EXPORT_CHANNEL(source)                                          \
  do {                                                                  \
    register long                                                       \
      i;                                                                \
                                                                        \
    if (source_image->storage_class == PseudoClass)                     \
      {                                                                 \
        for (i=0; i < npixels; i++)                                     \
          {                                                             \
            new_pixels[i].red=new_pixels[i].green=new_pixels[i].blue=   \
              source_image->colormap[source_indexes[i]].source;         \
            new_pixels[i].opacity=OpaqueOpacity;                        \
          }                                                             \
      }                                                                 \
    else                                                                \
      {                                                                 \
        for (i=0; i < npixels; i++)                                     \
          {                                                             \
            new_pixels[i].red=new_pixels[i].green=new_pixels[i].blue=   \
              source_pixels[i].source;                                  \
            new_pixels[i].opacity=OpaqueOpacity;                        \
          }                                                             \
      }                                                                 \
  } while (0);

static MagickPassFail
ExportImageChannelPixels(void *mutable_data,                /* User provided mutable data */
                         const void *immutable_data,        /* User provided immutable data */
                         const Image *source_image,         /* Source image */
                         const PixelPacket *source_pixels,  /* Pixel row in source image */
                         const IndexPacket *source_indexes, /* Pixel row indexes in source image */
                         Image *new_image,                  /* New image */
                         PixelPacket *new_pixels,           /* Pixel row in new image */
                         IndexPacket *new_indexes,          /* Pixel row indexes in new image */
                         const long npixels,                /* Number of pixels in row */
                         ExceptionInfo *exception           /* Exception report */
                         )
{
  ChannelType
    channel = *((const ChannelType *) immutable_data);

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(new_image);
  ARG_NOT_USED(new_indexes);
  ARG_NOT_USED(exception);

  switch (channel)
    {
    case RedChannel:
    case CyanChannel:
      {
        EXPORT_CHANNEL(red);
        break;
      }
    case GreenChannel:
    case MagentaChannel:
      {
        EXPORT_CHANNEL(green);
        break;
      }
    case BlueChannel:
    case YellowChannel:
      {
        EXPORT_CHANNEL(blue);
        break;
      }
    case MatteChannel:
    case OpacityChannel:
      {
        if (source_image->colorspace == CMYKColorspace)
          {
            register long
              i;
            
            for (i=0; i < npixels; i++)
              {
                new_pixels[i].red=new_pixels[i].green=
                  new_pixels[i].blue=source_indexes[i];
                new_pixels[i].opacity=OpaqueOpacity;
              }
          }
        else
          {
            EXPORT_CHANNEL(opacity);
          }
        break;
      }
    case BlackChannel:
      {
        EXPORT_CHANNEL(opacity);
        break;
      }
    default:
      {
      }
    }
  
  return MagickPass;
}
#define ExportImageChannelText "[%s] Exporting channel...  "
MagickExport Image *ExportImageChannel(const Image *source_image,
                                       const ChannelType channel,
                                       ExceptionInfo *exception)
{
  ChannelType
    channel_type = channel;
  
  Image
    *new_image;

  assert(source_image != (Image *) NULL);
  assert(source_image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);

  /*
    Verify that image colorspace is compatible with with requested
    channel type.
  */
  if (ValidateChannelRequest(source_image->colorspace,channel,exception)
      == MagickFail)
    return (Image *) NULL;

  new_image=CloneImage(source_image,source_image->columns,source_image->rows,
                       True,exception);
  if (new_image == (Image *) NULL)
    return ((Image *) NULL);

  new_image->storage_class=DirectClass;

  (void) PixelIterateDualNew(ExportImageChannelPixels,
                             NULL,
                             ExportImageChannelText,
                             NULL,&channel_type,
                             source_image->columns,source_image->rows,
                             source_image,0,0,
                             new_image,0,0,
                             exception); 

  new_image->is_grayscale=True;
  new_image->is_monochrome=source_image->is_monochrome;
  return new_image;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e C h a n n e l D e p t h                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageChannelDepth() returns the minimum bit depth required to store
%  the specified image channel without actual loss of color resolution.
%  Pixel components are stored in a Quantum, which is 8, 16, or 32 bits
%  depending on the QuantumDepth value set when the software is compiled.
%  GetImageChannelDepth() returns the smallest modulus storage size which
%  supports the scale of the pixel within the range (i.e. no information is
%  lost). As an example, the value one is returned for a bilevel channel
%  since only one bit of resolution is required to represent a bilevel channel.
%
%  The format of the GetImageChannelDepth method is:
%
%      unsigned long GetImageChannelDepth(const Image *image,
%                      const ChannelType channel,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o channel: Channel to test.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
#define ComputeChannelDepthText "[%s] Get channel depth..."

#define CHANNEL_DEPTH(parameter)                                \
  {                                                             \
    register long                                               \
      i;                                                        \
                                                                \
    register unsigned int                                       \
      scale;                                                    \
                                                                \
    if (depth < 1)                                              \
      depth=1;                                                  \
    scale=MaxRGB / (MaxRGB >> (QuantumDepth-depth));            \
    i=0;                                                        \
    while (i < npixels)                                         \
      {                                                         \
        if ((parameter) != scale*((parameter)/scale))           \
          {                                                     \
            depth++;                                            \
            if (depth == QuantumDepth)                          \
              break;                                            \
            scale=MaxRGB / (MaxRGB >> (QuantumDepth-depth));    \
            continue;                                           \
          }                                                     \
        i++;                                                    \
      }                                                         \
  }

static MagickPassFail
GetImageChannelDepthPixels(void *mutable_data,          /* User provided mutable data */
                           const void *immutable_data,  /* User provided immutable data */
                           const Image *image,          /* Input image */
                           const PixelPacket *pixels,   /* Pixel row */
                           const IndexPacket *indexes,  /* Pixel indexes */
                           const long npixels,          /* Number of pixels in row */
                           ExceptionInfo *exception     /* Exception report */
                           )
{
  unsigned int
    *channel_depth=(unsigned int *) mutable_data;

  ChannelType
    channel = *((const ChannelType *) immutable_data);

  register unsigned int
    depth;

  ARG_NOT_USED(exception);

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_GetImageChannelDepthPixels)
#endif
  {
    depth=*channel_depth;
  }

  switch (channel)
    {
    case RedChannel:
    case CyanChannel:
      {
        CHANNEL_DEPTH(pixels[i].red);
        break;
      }
    case GreenChannel:
    case MagentaChannel:
      {
        CHANNEL_DEPTH(pixels[i].green);
        break;
      }
    case BlueChannel:
    case YellowChannel:
      {
        CHANNEL_DEPTH(pixels[i].blue);
        break;
      }
    case MatteChannel:
    case OpacityChannel:
      {
        if (image->colorspace == CMYKColorspace)
          {
            CHANNEL_DEPTH(indexes[i]);
          }
        else
          {
            CHANNEL_DEPTH(pixels[i].opacity);
          }
        break;
      }
    case BlackChannel:
      {
        CHANNEL_DEPTH(pixels[i].opacity);
        break;
      }
    default:
      {
      }
    }

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_GetImageChannelDepthPixels)
#endif
  {
    if (depth > *channel_depth)
      *channel_depth=depth;
  }

  if (depth >= QuantumDepth)
    return MagickFail;

  return MagickPass;
}

MagickExport unsigned int
GetImageChannelDepth(const Image *image,
                     const ChannelType channel,
                     ExceptionInfo *exception)
{
  unsigned int
    depth;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  depth=1;

  (void) PixelIterateMonoRead(GetImageChannelDepthPixels,
                              NULL,
                              ComputeChannelDepthText,
                              &depth,
                              &channel,
                              0,0,image->columns,image->rows,
                              image,exception);
  return depth;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%     I m p o r t I m a g e C h a n n e l                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ImportImageChannel() imports an image into the specified image channel.
%
%  The format of the ImportImageChannel method is:
%
%      MagickPassFail ImportImageChannel(const Image *source_image,
%                                        Image *update_image,
%                                        const ChannelType channel)
%
%  A description of each parameter follows:
%
%    o source_image: The image to use as the replacement image channel.
%
%    o update_image: The image to import the channel into.
%
%    o channel: The image channel to import
%
%
*/
#define IMPORT_CHANNEL(target)                                          \
  do                                                                    \
    {                                                                   \
      register long                                                     \
        i;                                                              \
                                                                        \
      if (source_image->storage_class == PseudoClass)                   \
        {                                                               \
          if (source_image->is_grayscale)                               \
            for (i=0; i < npixels; i++)                                 \
              target=source_image->colormap[source_indexes[i]].red;     \
          else                                                          \
            for (i=0; i < npixels; i++)                                 \
              target=PixelIntensityToQuantum(&source_image->colormap[source_indexes[i]]); \
        }                                                               \
      else                                                              \
        {                                                               \
          if (source_image->is_grayscale)                               \
            for (i=0; i < npixels; i++)                                 \
              target=source_pixels[i].red;                              \
          else                                                          \
            for (i=0; i < npixels; i++)                                 \
              target=PixelIntensityToQuantum(&source_pixels[i]);        \
        }                                                               \
    } while (0);

static MagickPassFail
ImportImageChannelPixels(void *mutable_data,                /* User provided mutable data */
                         const void *immutable_data,        /* User provided immutable data */
                         const Image *source_image,         /* Source image */
                         const PixelPacket *source_pixels,  /* Pixel row in source image */
                         const IndexPacket *source_indexes, /* Pixel row indexes in source image */
                         Image *update_image,               /* Update image */
                         PixelPacket *update_pixels,        /* Pixel row in update image */
                         IndexPacket *update_indexes,       /* Pixel row indexes in update image */
                         const long npixels,                /* Number of pixels in row */
                         ExceptionInfo *exception           /* Exception report */
                         )
{
  ChannelType
    channel = *((const ChannelType *) immutable_data);

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(exception);

  switch (channel)
    {
    case RedChannel:
    case CyanChannel:
      {
        IMPORT_CHANNEL(update_pixels[i].red);
        break;
      }
    case GreenChannel:
    case MagentaChannel:
      {
        IMPORT_CHANNEL(update_pixels[i].green);
        break;
      }
    case BlueChannel:
    case YellowChannel:
      {
        IMPORT_CHANNEL(update_pixels[i].blue);
        break;
      }
    case MatteChannel:
    case OpacityChannel:
      {
        if (update_image->colorspace == CMYKColorspace)
          {
            IMPORT_CHANNEL(update_indexes[i]);
          }
        else
          {
            IMPORT_CHANNEL(update_pixels[i].opacity);
          }
        break;
      }
    case BlackChannel:
      {
        IMPORT_CHANNEL(update_pixels[i].opacity);
        break;
      }
    default:
      {
      }
    }

  return MagickPass;
}

#define ImportImageChannelText  "[%s] Importing channel..."
MagickPassFail ImportImageChannel(const Image *source_image,
                                  Image *update_image,
                                  const ChannelType channel)
{
  ChannelType
    channel_type = channel;

  MagickPassFail
    status=MagickPass;

  assert(update_image != (Image *) NULL);
  assert(update_image->signature == MagickSignature);
  assert(source_image != (Image *) NULL);
  assert(source_image->signature == MagickSignature);

  /*
    Verify that image colorspace is compatible with with requested
    channel type.
  */
  if (ValidateChannelRequest(update_image->colorspace,channel,&update_image->exception)
      == MagickFail)
    return MagickFail;

  update_image->storage_class=DirectClass;
  status=PixelIterateDualModify(ImportImageChannelPixels,
                                NULL,
                                ImportImageChannelText,
                                NULL,&channel_type,
                                source_image->columns,source_image->rows,
                                source_image,0,0,
                                update_image,0,0,
                                &update_image->exception);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%     I m p o r t I m a g e C h a n n e l s M a s k e d                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ImportImageChannelsMasked() imports all the channels from a source
%  image to an update image, except for the channels specified.
%
%  The format of the ImportImageChannelsMasked method is:
%
%      MagickPassFail ImportImageChannelsMasked(const Image *source_image,
%                                        Image *update_image,
%                                        const ChannelType channels)
%
%  A description of each parameter follows:
%
%    o source_image: The image from which to extract the replacement channels.
%
%    o update_image: The image to import the channels into.
%
%    o channel: The image channel to import
%
%
*/
static MagickPassFail
ImportImageChannelsMaskedPixels(void *mutable_data,                /* User provided mutable data */
                                const void *immutable_data,        /* User provided immutable data */
                                const Image *source_image,         /* Source image */
                                const PixelPacket *source_pixels,  /* Pixel row in source image */
                                const IndexPacket *source_indexes, /* Pixel row indexes in source image */
                                Image *update_image,               /* Update image */
                                PixelPacket *update_pixels,        /* Pixel row in update image */
                                IndexPacket *update_indexes,       /* Pixel row indexes in update image */
                                const long npixels,                /* Number of pixels in row */
                                ExceptionInfo *exception           /* Exception report */
                                )
{
  ChannelType
    channels = *((const ChannelType *) immutable_data);

  register long
    i;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(source_image);
  ARG_NOT_USED(exception);

  if (IsCMYKColorspace(update_image->colorspace))
    {
      if (!MagickChannelEnabled(channels,CyanChannel))
        for (i=0 ; i < npixels; i++)
          SetCyanSample(&update_pixels[i],GetCyanSample(&source_pixels[i]));
      if (!MagickChannelEnabled(channels,MagentaChannel))
        for (i=0 ; i < npixels; i++)
          SetMagentaSample(&update_pixels[i],GetMagentaSample(&source_pixels[i]));
      if (!MagickChannelEnabled(channels,YellowChannel))
        for (i=0 ; i < npixels; i++)
          SetYellowSample(&update_pixels[i],GetYellowSample(&source_pixels[i]));
      if (!MagickChannelEnabled(channels,BlackChannel))
        for (i=0 ; i < npixels; i++)
          SetBlackSample(&update_pixels[i],GetBlackSample(&source_pixels[i]));
      if ((update_image->matte) &&
          (!MagickChannelEnabled(channels,OpacityChannel)) &&
          (source_indexes != (const IndexPacket *) NULL) &&
          (update_indexes != (IndexPacket *) NULL))
        (void) memcpy(update_indexes,source_indexes,npixels*sizeof(IndexPacket));
    }
  else
    {
      if (!MagickChannelEnabled(channels,RedChannel))
        for (i=0 ; i < npixels; i++)
          SetRedSample(&update_pixels[i],GetRedSample(&source_pixels[i]));
      if (!MagickChannelEnabled(channels,GreenChannel))
        for (i=0 ; i < npixels; i++)
          SetGreenSample(&update_pixels[i],GetGreenSample(&source_pixels[i]));
      if (!MagickChannelEnabled(channels,BlueChannel))
        for (i=0 ; i < npixels; i++)
          SetBlueSample(&update_pixels[i],GetBlueSample(&source_pixels[i]));
      if (!MagickChannelEnabled(channels,OpacityChannel))
        for (i=0 ; i < npixels; i++)
          SetOpacitySample(&update_pixels[i],GetOpacitySample(&source_pixels[i]));
    }

  return MagickPass;
}

#define ImportImageChannelsMaskedText  "[%s] Importing channels...  "
MagickPassFail ImportImageChannelsMasked(const Image *source_image,
                                         Image *update_image,
                                         const ChannelType channels)
{
  ChannelType
    channel_type = channels;
  
  MagickPassFail
    status=MagickPass;
  
  assert(update_image != (Image *) NULL);
  assert(update_image->signature == MagickSignature);
  assert(source_image != (Image *) NULL);
  assert(source_image->signature == MagickSignature);

  if ((AllChannels != channel_type) || (GrayChannel != channel_type))
    {
      update_image->storage_class=DirectClass;
      status=PixelIterateDualModify(ImportImageChannelsMaskedPixels,
                                    NULL,
                                    ImportImageChannelsMaskedText,
                                    NULL,&channel_type,
                                    source_image->columns,source_image->rows,
                                    source_image,0,0,
                                    update_image,0,0,
                                    &update_image->exception);
    }
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t I m a g e C h a n n e l D e p t h                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetImageChannelDepth() translates the pixel quantums in the specified
%  channel so that if they are later divided to fit within the specified bit
%  depth, that no additional information is lost (i.e. no remainder resulting
%  from the division). Note that any subsequent image processing is likely
%  to increase the effective depth of the image channels. A non-zero
%  value is returned if the operation is successful. Check the exception
%  member of image to determine the cause for any failure.
%
%  The format of the SetImageChannelDepth method is:
%
%      MagickPassFail SetImageChannelDepth(Image *image,
%                                          const ChannelType channel,
%                                          const unsigned int depth)
%
%  A description of each parameter follows:
%
%    o image: The image to update.
%
%    o channel: Channel to modify.
%
%    o depth: Desired channel depth (range 1 to QuantumDepth)
%
%
*/
MagickExport MagickPassFail SetImageChannelDepth(Image *image,
                                                 const ChannelType channel,
                                                 const unsigned int depth)
{
  return QuantumOperatorImage(image,channel,DepthQuantumOp,(double) depth,
                              &image->exception);
}
