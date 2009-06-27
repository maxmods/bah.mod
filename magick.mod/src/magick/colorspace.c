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
%                  Methods to transform the image colorspace                  %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
%                              Bob Friesenhahn                                %
%                                 March 2003                                  %
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
#include "magick/attribute.h"
#include "magick/color.h"
#include "magick/colorspace.h"
#include "magick/enum_strings.h"
#include "magick/gem.h"
#include "magick/log.h"
#include "magick/monitor.h"
#include "magick/pixel_iterator.h"
#include "magick/utility.h"

/* Assign value of attribute to double if attribute exists for key */
#define MagickAttributeToDouble(image,key,variable) \
{ \
    const ImageAttribute \
      *attribute; \
\
  if ((attribute=GetImageAttribute(image,key))) \
  { \
    variable=strtod(attribute->value,(char **) NULL); \
  } \
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+     R G B T r a n s f o r m I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RGBTransformImage converts the reference image from RGB to
%  an alternate colorspace.  The transformation matrices are not the standard
%  ones: the weights are rescaled to normalize the range of the transformed
%  values to be [0..MaxRGB].
%
%  The format of the RGBTransformImage method is:
%
%      unsigned int RGBTransformImage(Image *image,
%        const ColorspaceType colorspace)
%
%  A description of each parameter follows:
%
%    o image: the image
%
%    o colorspace: the colorspace to transform the image to.
%
%
*/

static MagickPassFail
RGBToCMYKTransform(void *mutable_data,          /* User provided mutable data */
                   const void *immutable_data,  /* User provided immutable data */
                   Image *image,                /* Modify image */
                   PixelPacket *pixels,         /* Pixel row */
                   IndexPacket *indexes,        /* Pixel row indexes */
                   const long npixels,          /* Number of pixels in row */
                   ExceptionInfo *exception)    /* Exception report */
{
  /*
    Transform RGB to CMYK(A) pixels.
  */
  register long
    i;  
  
  Quantum
    black,
    cyan,
    magenta,
    yellow;
  
  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(immutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);
  
  for (i=0; i < npixels; i++)
    {
      cyan=(Quantum) (MaxRGB-pixels[i].red);
      magenta=(Quantum) (MaxRGB-pixels[i].green);
      yellow=(Quantum) (MaxRGB-pixels[i].blue);
      black=(cyan < magenta ? Min(cyan,yellow) : Min(magenta,yellow));
      pixels[i].red=cyan;
      pixels[i].green=magenta;
      pixels[i].blue=yellow;
      indexes[i]=pixels[i].opacity;
      pixels[i].opacity=black;
    }
  
  return MagickPass;
}

static MagickPassFail
RGBToCineonLogTransform(void *mutable_data,          /* User provided mutable data */
                        const void *immutable_data,  /* User provided immutable data */
                        Image *image,                /* Modify image */
                        PixelPacket *pixels,         /* Pixel row */
                        IndexPacket *indexes,        /* Pixel row indexes */
                        const long npixels,          /* Number of pixels in row */
                        ExceptionInfo *exception)    /* Exception report */
{
  /*
    Transform RGB pixels to CineonLog based on an existing lookup
    table.
  */
  const unsigned int
    *logmap = (const unsigned int *) immutable_data;
  
  register long
    i;  

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);
  
  for (i=0; i < npixels; i++)
    {
      pixels[i].red   = logmap[ScaleQuantumToMap(pixels[i].red)];
      pixels[i].green = logmap[ScaleQuantumToMap(pixels[i].green)];
      pixels[i].blue  = logmap[ScaleQuantumToMap(pixels[i].blue)];
    }
  
  return MagickPass;
}

static MagickPassFail
RGBToHSLTransform(void *mutable_data,          /* User provided mutable data */
                  const void *immutable_data,  /* User provided immutable data */
                  Image *image,                /* Modify image */
                  PixelPacket *pixels,         /* Pixel row */
                  IndexPacket *indexes,        /* Pixel row indexes */
                  const long npixels,          /* Number of pixels in row */
                  ExceptionInfo *exception)    /* Exception report */
{
  /*
    Transform pixels from RGB space to HSL space.
  */
  double
    h,
    s,
    l;
  
  register long
    i;  
  
  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(immutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);
  
  for (i=0; i < npixels; i++)
    {
      TransformHSL(pixels[i].red,pixels[i].green,pixels[i].blue,&h,&s,&l);
      h *= MaxRGB;
      s *= MaxRGB;
      l *= MaxRGB;
      pixels[i].red=RoundDoubleToQuantum(h);
      pixels[i].green=RoundDoubleToQuantum(s);
      pixels[i].blue=RoundDoubleToQuantum(l);
    }

  return MagickPass;
}

static MagickPassFail
RGBToHWBTransform(void *mutable_data,          /* User provided mutable data */
                  const void *immutable_data,  /* User provided immutable data */
                  Image *image,                /* Modify image */
                  PixelPacket *pixels,         /* Pixel row */
                  IndexPacket *indexes,        /* Pixel row indexes */
                  const long npixels,          /* Number of pixels in row */
                  ExceptionInfo *exception)    /* Exception report */
{
  /*
    Transform pixels from RGB space to HWB space.
  */
  double
    h,
    w,
    b;

  register long
    i;  

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(immutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  for (i=0; i < npixels; i++)
    {
      TransformHWB(pixels[i].red,pixels[i].green,pixels[i].blue,&h,&w,&b);
      h *= MaxRGB;
      w *= MaxRGB;
      b *= MaxRGB;
      pixels[i].red=RoundDoubleToQuantum(h);
      pixels[i].green=RoundDoubleToQuantum(w);
      pixels[i].blue=RoundDoubleToQuantum(b);
    }

  return MagickPass;
}

typedef struct _XYZColorTransformPacket
{
  float
    x,
    y,
    z;
} XYZColorTransformPacket;

typedef struct _XYZColorTransformInfo_t
{
  XYZColorTransformPacket *x;
  XYZColorTransformPacket *y;
  XYZColorTransformPacket *z;
  XYZColorTransformPacket primary_info;
} XYZColorTransformInfo_t;

static const size_t
  XYZMapAllocSize=(MaxMap+1)*sizeof(XYZColorTransformPacket);


static MagickPassFail
XYZTransformPackets(void *mutable_data,          /* User provided mutable data */
                    const void *immutable_data,  /* User provided immutable data */
                    Image *image,                /* Modify image */
                    PixelPacket *pixels,         /* Pixel row */
                    IndexPacket *indexes,        /* Pixel row indexes */
                    const long npixels,          /* Number of pixels in row */
                    ExceptionInfo *exception)    /* Exception report */
{
  /*
    3D transform pixels from RGB to alternate colorspace.
  */
  float
    b,
    g,
    r;

  const XYZColorTransformInfo_t
    *xform = (const XYZColorTransformInfo_t *) immutable_data;

  register long
    i;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  for (i=0; i < npixels; i++)
    {
      register unsigned int
        x_index,
        y_index,
        z_index;

      x_index = ScaleQuantumToMap(pixels[i].red);
      y_index = ScaleQuantumToMap(pixels[i].green);
      z_index = ScaleQuantumToMap(pixels[i].blue);

      r = (xform->x[x_index].x + xform->y[y_index].x + xform->z[z_index].x + xform->primary_info.x);
      g = (xform->x[x_index].y + xform->y[y_index].y + xform->z[z_index].y + xform->primary_info.y);
      b = (xform->x[x_index].z + xform->y[y_index].z + xform->z[z_index].z + xform->primary_info.z);

      r = r < 0.0f ? 0.0f : r > MaxMapFloat ? MaxMapFloat : (r + 0.5f);
      g = g < 0.0f ? 0.0f : g > MaxMapFloat ? MaxMapFloat : (g + 0.5f);
      b = b < 0.0f ? 0.0f : b > MaxMapFloat ? MaxMapFloat : (b + 0.5f);

      pixels[i].red   = ScaleMapToQuantum((Quantum) r);
      pixels[i].green = ScaleMapToQuantum((Quantum) g);
      pixels[i].blue  = ScaleMapToQuantum((Quantum) b);
    }

  return MagickPass;
}

MagickExport MagickPassFail RGBTransformImage(Image *image,
                                              const ColorspaceType colorspace)
{
  char
    progress_message[MaxTextExtent];

  register long
    i;

  MagickPassFail
    status=MagickPass;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  /* Detect bogus request to convert to RGB */
  assert(colorspace != RGBColorspace);
  assert(colorspace != TransparentColorspace);
  assert(colorspace != UndefinedColorspace);

  /*
    Ensure that image is an RGB-compatible colorspace prior to
    transforming to an alternate colorspace.
  */
  if (!IsRGBColorspace(image->colorspace))
    (void) TransformRGBImage(image,image->colorspace);

  /*
    Log colorspace transform event
  */
  (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                        "Transform colorspace from %s to %s",
                        ColorspaceTypeToString(image->colorspace),
                        ColorspaceTypeToString(colorspace));
  FormatString(progress_message,"[%%s] Transform colorspace from %s to %s...",
               ColorspaceTypeToString(image->colorspace),
               ColorspaceTypeToString(colorspace));

  /*
    Store colorspace in image.
  */
  image->colorspace=colorspace;

  if (colorspace == CMYKColorspace)
    {
      /*
        Transform RGB to CMYK(A) pixels.
      */
      if (image->storage_class == PseudoClass)
        {
          
          status=SyncImage(image);
          image->storage_class=DirectClass;
        }
      if (status != MagickFail)
        status=PixelIterateMonoModify(RGBToCMYKTransform,
                                      NULL,
                                      progress_message,
                                      NULL,NULL,
                                      0,0,image->columns,image->rows,
                                      image,
                                      &image->exception);
      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                            "Colorspace transform completed"); 
      return(status);
    }

  if (colorspace == CineonLogRGBColorspace)
    {
      /*
        Transform from linear RGB to Cineon Log RGB.
      */
      double
        DisplayGamma,
        Gain,
        MaxLinearValue,
        NegativeFilmGamma,
        Offset,
        ReferenceBlack,
        ReferenceWhite;

      unsigned int
        *logmap,
        scale_to_short;

      /*
        Establish defaults.
      */
      MaxLinearValue=MaxRGB; /* Maximum linear value output */
      ReferenceWhite=685;    /* 90% white card (default 685) */
      ReferenceBlack=95;     /* 1% black card  (default 95) */
      DisplayGamma=1.7;      /* Typical display gamma (Kodak recommends 1.7) */
      NegativeFilmGamma=0.6; /* Typical gamma for a film negative */

      /*
        Allow image attributes to override defaults.
      */
      MagickAttributeToDouble(image,"reference-white",ReferenceWhite);
      MagickAttributeToDouble(image,"reference-black",ReferenceBlack);
      MagickAttributeToDouble(image,"display-gamma",DisplayGamma);
      MagickAttributeToDouble(image,"film-gamma",NegativeFilmGamma);

      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                            "Log Transform: ReferenceWhite=%g ReferenceBlack=%g DisplayGamma=%g FilmGamma=%g",
                            ReferenceWhite,ReferenceBlack,DisplayGamma,NegativeFilmGamma);

      /*
        FIXME: Math seems to be producing data with gamma 1.0 rather than 1.7.
      */

#if 1
      Gain=MaxLinearValue/(1.0 - pow(pow(10,((ReferenceBlack-ReferenceWhite)
                                             *0.002/NegativeFilmGamma)),
                                     (DisplayGamma/1.7)));
#else
      Gain=MaxLinearValue/(1.0 - pow(pow(10,((ReferenceBlack-ReferenceWhite)
                                             *0.002/NegativeFilmGamma)),
                                     (1.0/DisplayGamma)));
#endif
      Offset=Gain-MaxLinearValue;

      /*
        Build LUT
      */
      logmap=MagickAllocateMemory(unsigned int *,(MaxMap+1)*sizeof(unsigned int));
      if (logmap == 0)
        ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
                              UnableToTransformColorspace);
      scale_to_short=(65535U / (65535U >> (16-10)));
      for (i=0; i <= (long) MaxMap; i++)
        {
          double
            linearval,
            logval;

          linearval=i*(double) MaxRGB/MaxMap;

          /*
            FIXME: Math seems to be expecting data with gamma 1.0 rather than 1.7.
            Also, quantizing to 64K levels does not do justice to the Q32 build at all.
          */
          logval=685+log10(pow((((double) linearval+Offset)/Gain),
                               (1.7/DisplayGamma)))/(0.002/NegativeFilmGamma);

          /*           logval=685+log10(pow((((double) linearval+Offset)/Gain), */
          /*                                (DisplayGamma/1.0)))/(0.002/NegativeFilmGamma); */

          logval *= scale_to_short;
          logmap[i]=ScaleShortToQuantum((unsigned int) (logval + 0.5));
          /* printf("logmap[%u]=%u\n",i,(unsigned int) logmap[i]); */
        }
      /*
        Transform pixels.
      */
      if (image->storage_class == PseudoClass)
        {
          /*
            Convert PseudoClass image colormap.
          */
          (void) RGBToCineonLogTransform(0,
                                         logmap,
                                         image,
                                         image->colormap,
                                         (IndexPacket *) NULL,
                                         image->colors,
                                         &image->exception);
          status=SyncImage(image);
        }
      else
        {
          /*
            Convert DirectClass image.
          */
          status=PixelIterateMonoModify(RGBToCineonLogTransform,
                                        NULL,
                                        progress_message,
                                        NULL,logmap,
                                        0,0,image->columns,image->rows,
                                        image,
                                        &image->exception);
        }

      MagickFreeMemory(logmap);
      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                            "Transform to colorspace %s completed",
                            ColorspaceTypeToString(colorspace));
      return(status);
    }


  if (colorspace == HSLColorspace)
    {
      if (image->storage_class == PseudoClass)
        {
          /*
            Convert PseudoClass image colormap.
          */
          RGBToHSLTransform(NULL,
                            NULL,
                            image,
                            image->colormap,
                            (IndexPacket *) NULL,
                            image->colors,
                            &image->exception);
          status=SyncImage(image);
        }
      else
        {
          /*
            Convert DirectClass image.
          */
          status=PixelIterateMonoModify(RGBToHSLTransform,
                                        NULL,
                                        progress_message,
                                        NULL,NULL,
                                        0,0,image->columns,image->rows,
                                        image,
                                        &image->exception);
        }
      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                            "Colorspace transform completed"); 
      return(status);
    }

  if (colorspace == HWBColorspace)
    {
      if (image->storage_class == PseudoClass)
        {
          /*
            Convert PseudoClass image colormap.
          */
          RGBToHWBTransform(NULL,
                            NULL,
                            image,
                            image->colormap,
                            (IndexPacket *) NULL,
                            image->colors,
                            &image->exception);
          status=SyncImage(image);
        }
      else
        {
          status=PixelIterateMonoModify(RGBToHWBTransform,
                                        NULL,
                                        progress_message,
                                        NULL,NULL,
                                        0,0,image->columns,image->rows,
                                        image,
                                        &image->exception);
        }
      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                            "Colorspace transform completed"); 
      return(status);
    }

  {
    /*
      3D Transform.
    */

    XYZColorTransformInfo_t
      xform;

    /*
      Allocate the tables.
    */
    xform.x=MagickAllocateMemory(XYZColorTransformPacket *,XYZMapAllocSize);
    xform.y=MagickAllocateMemory(XYZColorTransformPacket *,XYZMapAllocSize);
    xform.z=MagickAllocateMemory(XYZColorTransformPacket *,XYZMapAllocSize);
    if ((xform.x == 0) || (xform.y == 0) || (xform.z == 0))
      {
        MagickFreeMemory(xform.x);
        MagickFreeMemory(xform.y);
        MagickFreeMemory(xform.z);
        ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
                              UnableToTransformColorspace);
      }
    xform.primary_info.x=xform.primary_info.y=xform.primary_info.z=0;
    switch (colorspace)
      {
      case GRAYColorspace:
      case Rec601LumaColorspace:
        {
          /*
            Initialize Rec. 601 Luma tables:

            G = 0.29900*R+0.58700*G+0.11400*B
          */
#if MaxMap > 255
#  if defined(HAVE_OPENMP)
#    pragma omp parallel for schedule(static,64)
#  endif
#endif
          for (i=0; i <= (long) MaxMap; i++)
            {
              xform.x[i].x=xform.x[i].y=xform.x[i].z=(0.299f*(float) i);
              xform.y[i].x=xform.y[i].y=xform.y[i].z=(0.587f*(float) i);
              xform.z[i].x=xform.z[i].y=xform.z[i].z=(0.114f*(float) i);
            }
          break;
        }
      case Rec709LumaColorspace:
        {
          /*
            Initialize Rec. 709 Luma tables:

            G = 0.2126*R+0.7152*G+0.0722*B
          */
#if MaxMap > 255
#  if defined(HAVE_OPENMP)
#    pragma omp parallel for schedule(static,64)
#  endif
#endif
          for (i=0; i <= (long) MaxMap; i++)
            {
              xform.x[i].x=xform.x[i].y=xform.x[i].z=(0.2126f*(float) i);
              xform.y[i].x=xform.y[i].y=xform.y[i].z=(0.7152f*(float) i);
              xform.z[i].x=xform.z[i].y=xform.z[i].z=(0.0722f*(float) i);
            }
          break;
        }
      case OHTAColorspace:
        {
          /*
            Initialize OHTA tables:

            I1 = 0.33333*R+0.33334*G+0.33333*B
            I2 = 0.50000*R+0.00000*G-0.50000*B
            I3 =-0.25000*R+0.50000*G-0.25000*B

            I and Q, normally -0.5 through 0.5, are normalized to the range 0
            through MaxRGB.
          */
          xform.primary_info.y=((MaxMap+1)/2);
          xform.primary_info.z=((MaxMap+1)/2);
#if MaxMap > 255
#  if defined(HAVE_OPENMP)
#    pragma omp parallel for schedule(static,64)
#  endif
#endif
          for (i=0; i <= (long) MaxMap; i++)
            {
              xform.x[i].x=(0.33333f*(float) i);
              xform.y[i].x=(0.33334f*(float) i);
              xform.z[i].x=(0.33333f*(float) i);
              xform.x[i].y=(0.5f*(float) i);
              xform.y[i].y=(0.0f);
              xform.z[i].y=((-0.5f)*(float) i);
              xform.x[i].z=((-0.25f)*(float) i);
              xform.y[i].z=(0.5f*(float) i);
              xform.z[i].z=((-0.25f)*(float) i);
            }
          break;
        }
      case sRGBColorspace:
        {
          /*
            Kodak PhotoYCC Color Space

            Initialize sRGB tables:

            Y =  0.29900*R+0.58700*G+0.11400*B
            C1= -0.29900*R-0.58700*G+0.88600*B
            C2=  0.70100*R-0.58700*G-0.11400*B

            sRGB is scaled by 1.3584.  C1 zero is 156 and C2 is at 137.
          */
          /* FIXME! The scaling factors for this transform look bizarre,
             and in fact, the results are not correct. */
          xform.primary_info.y=(ScaleCharToMap(156));
          xform.primary_info.z=(ScaleCharToMap(137));
          for (i=0; i <= (long) (0.018*MaxMap); i++)
            {
              xform.x[i].x=(0.003962014134275617*i);
              xform.y[i].x=(0.007778268551236748*i);
              xform.z[i].x=(0.001510600706713781*i);
              xform.x[i].y=((-0.002426619775463276)*i);
              xform.y[i].y=((-0.004763965913702149)*i);
              xform.z[i].y=(0.007190585689165425*i);
              xform.x[i].z=(0.006927257754597858*i);
              xform.y[i].z=((-0.005800713697502058)*i);
              xform.z[i].z=((-0.0011265440570958)*i);
            }
          for ( ; i <= (long) MaxMap; i++)
            {
              xform.x[i].x=(0.2201118963486454*(1.099*i-0.099));
              xform.y[i].x=(0.4321260306242638*(1.099*i-0.099));
              xform.z[i].x=(0.08392226148409894*(1.099*i-0.099));
              xform.x[i].y=((-0.1348122097479598)*(1.099*i-0.099));
              xform.y[i].y=((-0.2646647729834528)*(1.099*i-0.099));
              xform.z[i].y=(0.3994769827314126*(1.099*i-0.099));
              xform.x[i].z=(0.3848476530332144*(1.099*i-0.099));
              xform.y[i].z=((-0.3222618720834477)*(1.099*i-0.099));
              xform.z[i].z=((-0.06258578094976668)*(1.099*i-0.099));
            }
          break;
        }
      case XYZColorspace:
        {
          /*
            Initialize CIE XYZ tables (from ITU-R 709 RGB):

            X = 0.412453*X+0.357580*Y+0.180423*Z
            Y = 0.212671*X+0.715160*Y+0.072169*Z
            Z = 0.019334*X+0.119193*Y+0.950227*Z
          */
#if MaxMap > 255
#  if defined(HAVE_OPENMP)
#    pragma omp parallel for schedule(static,64)
#  endif
#endif
          for (i=0; i <= (long) MaxMap; i++)
            {
              xform.x[i].x=(0.412453f*(float) i);
              xform.y[i].x=(0.35758f*(float) i);
              xform.z[i].x=(0.180423f*(float) i);
              xform.x[i].y=(0.212671f*(float) i);
              xform.y[i].y=(0.71516f*(float) i);
              xform.z[i].y=(0.072169f*(float) i);
              xform.x[i].z=(0.019334f*(float) i);
              xform.y[i].z=(0.119193f*(float) i);
              xform.z[i].z=(0.950227f*(float) i);
            }
          break;
        }
      case Rec601YCbCrColorspace:
        {
          /*
            Initialize YCbCr tables (using ITU-R BT.601 luma):

            Y =  0.299000*R+0.587000*G+0.114000*B
            Cb= -0.168736*R-0.331264*G+0.500000*B
            Cr=  0.500000*R-0.418688*G-0.081312*B

            Cb and Cr, normally -0.5 through 0.5, are normalized to the range 0
            through MaxRGB.
          */
          xform.primary_info.y=((MaxMap+1)/2);
          xform.primary_info.z=((MaxMap+1)/2);
#if MaxMap > 255
#  if defined(HAVE_OPENMP)
#    pragma omp parallel for schedule(static,64)
#  endif
#endif
          for (i=0; i <= (long) MaxMap; i++)
            {
              /* Red */
              xform.x[i].x=(0.299f*(float) i);
              xform.y[i].x=(0.587f*(float) i);
              xform.z[i].x=(0.114f*(float) i);
              /* Green */
              xform.x[i].y=((-0.16873f)*(float) i);
              xform.y[i].y=((-0.331264f)*(float) i);
              xform.z[i].y=(0.500000f*(float) i);
              /* Blue */
              xform.x[i].z=(0.500000f*(float) i);
              xform.y[i].z=((-0.418688f)*(float) i);
              xform.z[i].z=((-0.081312f)*(float) i);
            }
          break;
        }
      case Rec709YCbCrColorspace:
        {
          /*
            Initialize YCbCr tables (using ITU-R BT.709 luma):

            Y =  0.212600*R+0.715200*G+0.072200*B
            Cb= -0.114572*R-0.385428*G+0.500000*B
            Cr=  0.500000*R-0.454153*G-0.045847*B

            Cb and Cr, normally -0.5 through 0.5, are normalized to the range 0
            through MaxRGB.
          */
          xform.primary_info.y=((MaxMap+1)/2);
          xform.primary_info.z=((MaxMap+1)/2);
#if MaxMap > 255
#  if defined(HAVE_OPENMP)
#    pragma omp parallel for schedule(static,64)
#  endif
#endif
          for (i=0; i <= (long) MaxMap; i++)
            {
              /* Red */
              xform.x[i].x=(0.212600f*(float) i);
              xform.y[i].x=(0.715200f*(float) i);
              xform.z[i].x=(0.072200f*(float) i);
              /* Green */
              xform.x[i].y=((-0.114572f)*(float) i);
              xform.y[i].y=((-0.385428f)*(float) i);
              xform.z[i].y=(0.500000f*(float) i);
              /* Blue */
              xform.x[i].z=(0.500000f*(float) i);
              xform.y[i].z=((-0.454153f)*(float) i);
              xform.z[i].z=((-0.045847f)*(float) i);
            }
          break;
        }
      case YCCColorspace:
        {
          /*
            Kodak PhotoYCC Color Space.

            Initialize YCC tables:

            Y =  0.29900*R+0.58700*G+0.11400*B
            C1= -0.29900*R-0.58700*G+0.88600*B
            C2=  0.70100*R-0.58700*G-0.11400*B

            YCC is scaled by 1.3584.  C1 zero is 156 and C2 is at 137.
          */
          /* FIXME! The scaling factors for this transform look bizarre,
             and in fact, the results are not correct. */
          xform.primary_info.y=(ScaleCharToMap(156));
          xform.primary_info.z=(ScaleCharToMap(137));
          for (i=0; i <= (long) (0.018*MaxMap); i++)
            {
              xform.x[i].x=(0.003962014134275617*i);
              xform.y[i].x=(0.007778268551236748*i);
              xform.z[i].x=(0.001510600706713781*i);
              xform.x[i].y=((-0.002426619775463276)*i);
              xform.y[i].y=((-0.004763965913702149)*i);
              xform.z[i].y=(0.007190585689165425*i);
              xform.x[i].z=(0.006927257754597858*i);
              xform.y[i].z=((-0.005800713697502058)*i);
              xform.z[i].z=((-0.0011265440570958)*i);
            }
          for ( ; i <= (long) MaxMap; i++)
            {
              xform.x[i].x=(0.2201118963486454*(1.099*i-0.099));
              xform.y[i].x=(0.4321260306242638*(1.099*i-0.099));
              xform.z[i].x=(0.08392226148409894*(1.099*i-0.099));
              xform.x[i].y=((-0.1348122097479598)*(1.099*i-0.099));
              xform.y[i].y=((-0.2646647729834528)*(1.099*i-0.099));
              xform.z[i].y=(0.3994769827314126*(1.099*i-0.099));
              xform.x[i].z=(0.3848476530332144*(1.099*i-0.099));
              xform.y[i].z=((-0.3222618720834477)*(1.099*i-0.099));
              xform.z[i].z=((-0.06258578094976668)*(1.099*i-0.099));
            }
          break;
        }
      case YIQColorspace:
        {
          /*
            Initialize YIQ tables:

            Y = 0.29900*R+0.58700*G+0.11400*B
            I = 0.59600*R-0.27400*G-0.32200*B
            Q = 0.21100*R-0.52300*G+0.31200*B

            I and Q, normally -0.5 through 0.5, are normalized to the range 0
            through MaxRGB.
          */
          xform.primary_info.y=((MaxMap+1)/2);
          xform.primary_info.z=((MaxMap+1)/2);
#if MaxMap > 255
#  if defined(HAVE_OPENMP)
#    pragma omp parallel for schedule(static,64)
#  endif
#endif
          for (i=0; i <= (long) MaxMap; i++)
            {
              xform.x[i].x=(0.299f*(float) i);
              xform.y[i].x=(0.587f*(float) i);
              xform.z[i].x=(0.114f*(float) i);
              xform.x[i].y=(0.596f*(float) i);
              xform.y[i].y=((-0.274f)*(float) i);
              xform.z[i].y=((-0.322f)*(float) i);
              xform.x[i].z=(0.211f*(float) i);
              xform.y[i].z=((-0.523f)*(float) i);
              xform.z[i].z=(0.312f*(float) i);
            }
          break;
        }
      case YPbPrColorspace:
        {
          /*
            Initialize YPbPr tables (according to ITU-R BT.601):

            Y =  0.299000*R+0.587000*G+0.114000*B
            Pb= -0.168736*R-0.331264*G+0.500000*B
            Pr=  0.500000*R-0.418688*G-0.081312*B

            Pb and Pr, normally -0.5 through 0.5, are normalized to the range 0
            through MaxRGB.
          */
          xform.primary_info.y=((MaxMap+1)/2);
          xform.primary_info.z=((MaxMap+1)/2);
#if MaxMap > 255
#  if defined(HAVE_OPENMP)
#    pragma omp parallel for schedule(static,64)
#  endif
#endif
          for (i=0; i <= (long) MaxMap; i++)
            {
              xform.x[i].x=(0.299f*(float) i);
              xform.y[i].x=(0.587f*(float) i);
              xform.z[i].x=(0.114f*(float) i);
              xform.x[i].y=((-0.168736f)*(float) i);
              xform.y[i].y=((-0.331264f)*(float) i);
              xform.z[i].y=(0.5f*(float) i);
              xform.x[i].z=(0.5f*(float) i);
              xform.y[i].z=((-0.418688f)*(float) i);
              xform.z[i].z=((-0.081312f)*(float) i);
            }
          break;
        }
      case YUVColorspace:
      default:
        {
          /*
            Initialize YUV tables:

            Y =  0.29900*R+0.58700*G+0.11400*B
            U = -0.14740*R-0.28950*G+0.43690*B
            V =  0.61500*R-0.51500*G-0.10000*B

            U and V, normally -0.5 through 0.5, are normalized to the range 0
            through MaxRGB.  Note that U = 0.493*(B-Y), V = 0.877*(R-Y).
          */
          xform.primary_info.y=((MaxMap+1)/2);
          xform.primary_info.z=((MaxMap+1)/2);
#if MaxMap > 255
#  if defined(HAVE_OPENMP)
#    pragma omp parallel for schedule(static,64)
#  endif
#endif
          for (i=0; i <= (long) MaxMap; i++)
            {
              xform.x[i].x=(0.299f*(float) i);
              xform.y[i].x=(0.587f*(float) i);
              xform.z[i].x=(0.114f*(float) i);
              xform.x[i].y=((-0.1474f)*(float) i);
              xform.y[i].y=((-0.2895f)*(float) i);
              xform.z[i].y=(0.4369f*(float) i);
              xform.x[i].z=(0.615f*(float) i);
              xform.y[i].z=((-0.515f)*(float) i);
              xform.z[i].z=((-0.1f)*(float) i);
            }
          break;
        }
      }

#if 0
    /*
      Dump tables
    */
    for (i=0; i <= (long) MaxMap; i++)
      {
        printf("%5ld: xform.x(%g,%g,%g) xform.y(%g,%g,%g) xform.z(%g,%g,%g)\n",
               i,
               ((xform.x[i].x)),
               ((xform.x[i].y)),
               ((xform.x[i].z)),
             
               ((xform.y[i].x)),
               ((xform.y[i].y)),
               ((xform.y[i].z)),
             
               ((xform.z[i].x)),
               ((xform.z[i].y)),
               ((xform.z[i].z)));
      }
#endif

    /*
      Convert from RGB.
    */
    if (image->storage_class == PseudoClass)
      {
        /*
          Convert PseudoClass image colormap.
        */
        (void) XYZTransformPackets(NULL,
                                   &xform,
                                   image,
                                   image->colormap,
                                   (IndexPacket *) NULL,
                                   image->colors,
                                   &image->exception);
        status=SyncImage(image);
      }
    else
      {
        /*
          Convert DirectClass image.
        */
        status=PixelIterateMonoModify(XYZTransformPackets,
                                      NULL,
                                      progress_message,
                                      NULL,&xform,
                                      0,0,image->columns,image->rows,
                                      image,
                                      &image->exception);
      }

    /*
      Free allocated memory.
    */
    MagickFreeMemory(xform.x);
    MagickFreeMemory(xform.y);
    MagickFreeMemory(xform.z);
  }

  image->is_grayscale=IsGrayColorspace(colorspace);
  (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                        "Transform to colorspace %s completed",
                        ColorspaceTypeToString(colorspace)); 
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+     T r a n s f o r m C o l o r s p a c e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method (void) TransformColorspace converts the image to a specified colorspace.
%  If the image is already in the requested colorspace, no work is performed.
%  Note that the current colorspace is stored in the image colorspace member.
%  The transformation matrices are not necessarily the standard ones: the
%  weights are rescaled to normalize the range of the transformed values to
%  be [0..MaxRGB].
%
%  The format of the (void) TransformColorspace method is:
%
%      unsigned int (void) TransformColorspace(Image *image,
%        const ColorspaceType colorspace)
%
%  A description of each parameter follows:
%
%    o image: the image to transform
%
%    o colorspace: the desired colorspace.
%
*/
MagickExport MagickPassFail TransformColorspace(Image *image,
  const ColorspaceType colorspace)
{
  MagickPassFail
    status=MagickPass;

  assert(image != (Image *) NULL);
  assert(colorspace != UndefinedColorspace);
  assert(image->colorspace != UndefinedColorspace);
  /*
    If the image colorspace is the same as requested, do nothing.
  */
  if (image->colorspace == colorspace)
     return (status);

  /*
    If the requested colorspace is RGB or Transparent, then convert
    via TransformRGBImage.
  */
  if ((colorspace == RGBColorspace) ||
      (colorspace == TransparentColorspace))
      {
        status &= TransformRGBImage(image,image->colorspace);
        image->colorspace=colorspace;
        return  (status);
      }

  /*
    If the image is not already in an RGB-compatible colorspace, then
    convert it to RGB via TransformRGBImage, and then to the target
    colorspace via RGBTransformImage, otherwise just convert to the
    target colorspace via RGBTransformImage.
  */
  if (!IsRGBColorspace(image->colorspace))
    status=TransformRGBImage(image,image->colorspace);

  status &= RGBTransformImage(image,colorspace);
  return (status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+     T r a n s f o r m R G B I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method TransformRGBImage converts the reference image from an alternate
%  colorspace to RGB.  The transformation matrices are not the standard ones:
%  the weights are rescaled to normalize the range of the transformed values
%  to be [0..MaxRGB].
%
%  The format of the TransformRGBImage method is:
%
%      unsigned int TransformRGBImage(Image *image,
%        const ColorspaceType colorspace)
%
%  A description of each parameter follows:
%
%    o image: the image
%
%    o colorspace: the colorspace to transform the image to.
%
*/
static MagickPassFail
CMYKToRGBTransform(void *mutable_data,          /* User provided mutable data */
                   const void *immutable_data,  /* User provided immutable data */
                   Image *image,                /* Modify image */
                   PixelPacket *pixels,         /* Pixel row */
                   IndexPacket *indexes,        /* Pixel row indexes */
                   const long npixels,          /* Number of pixels in row */
                   ExceptionInfo *exception)    /* Exception report */
{
  /*
    Transform CMYK(A) pixels to RGB.
  */
  register long
    i;  

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(immutable_data);
  ARG_NOT_USED(exception);

  for (i=0; i < npixels; i++)
    {
      double
	black_factor;

      black_factor=MaxRGBDouble-GetBlackSample(&pixels[i]);
      SetRedSample(&pixels[i],
		   (Quantum) (((MaxRGBDouble-GetCyanSample(&pixels[i]))*
			       black_factor)/MaxRGBDouble+0.5));
      SetGreenSample(&pixels[i],
		     (Quantum) (((MaxRGBDouble-GetMagentaSample(&pixels[i]))*
				 black_factor)/MaxRGBDouble+0.5));
      SetBlueSample(&pixels[i],
		    (Quantum) (((MaxRGBDouble-GetYellowSample(&pixels[i]))*
				black_factor)/ MaxRGBDouble+0.5));
      SetOpacitySample(&pixels[i],(image->matte ? indexes[i] : OpaqueOpacity));
    }

  return MagickPass;
}

static MagickPassFail
CineonLogToRGBTransform(void *mutable_data,         /* User provided mutable data */
                        const void *immutable_data, /* User provided immutable data */
                        Image *image,               /* Modify image */
                        PixelPacket *pixels,        /* Pixel row */
                        IndexPacket *indexes,       /* Pixel row indexes */
                        const long npixels,         /* Number of pixels in row */
                        ExceptionInfo *exception)   /* Exception report */
{
  /*
    Transform CineonLog pixels to RGB based on an existing lookup
    table.
  */
  const Quantum
    *linearmap = (const Quantum *) immutable_data;

  register long
    i;  

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  for (i=0; i < npixels; i++)
    {
      pixels[i].red   = linearmap[ScaleQuantumToShort(pixels[i].red)/64U];
      pixels[i].green = linearmap[ScaleQuantumToShort(pixels[i].green)/64U];
      pixels[i].blue  = linearmap[ScaleQuantumToShort(pixels[i].blue)/64U];
    }

  return MagickPass;
}

static MagickPassFail
HSLToRGBTransform(void *mutable_data,         /* User provided mutable data */
                  const void *immutable_data, /* User provided immutable data */
                  Image *image,               /* Modify image */
                  PixelPacket *pixels,        /* Pixel row */
                  IndexPacket *indexes,       /* Pixel row indexes */
                  const long npixels,         /* Number of pixels in row */
                  ExceptionInfo *exception)   /* Exception report */
{
  /*
    Transform pixels from HSL space to RGB space.
  */
  register long
    i;  

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(immutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  for (i=0; i < npixels; i++)
    {
      HSLTransform((double) pixels[i].red/MaxRGB,(double) pixels[i].green/MaxRGB,
                   (double) pixels[i].blue/MaxRGB,&pixels[i].red,&pixels[i].green,&pixels[i].blue);
    }

  return MagickPass;
}

static MagickPassFail
HWBToRGBTransform(void *mutable_data,         /* User provided mutable data */
                  const void *immutable_data, /* User provided immutable data */
                  Image *image,               /* Modify image */
                  PixelPacket *pixels,        /* Pixel row */
                  IndexPacket *indexes,       /* Pixel row indexes */
                  const long npixels,         /* Number of pixels in row */
                  ExceptionInfo *exception)   /* Exception report */
{
  /*
    Transform pixels from HWB space to RGB space.
  */
  register long
    i;  

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(immutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  for (i=0; i < npixels; i++)
    {
      HWBTransform((double) pixels[i].red/MaxRGB,(double) pixels[i].green/MaxRGB,
                   (double) pixels[i].blue/MaxRGB,&pixels[i].red,&pixels[i].green,&pixels[i].blue);
    }

  return MagickPass;
}

typedef struct _RGBColorTransformPacket
{
  float
    r,
    g,
    b;
} RGBColorTransformPacket;

typedef struct _RGBTransformInfo_t
{
  RGBColorTransformPacket *r;
  RGBColorTransformPacket *g;
  RGBColorTransformPacket *b;
  unsigned int rgb_map_max_index;
  const unsigned char *rgb_map;
} RGBTransformInfo_t;

static MagickPassFail
RGBTransformPackets(void *mutable_data,         /* User provided mutable data */
                    const void *immutable_data, /* User provided immutable data */
                    Image *image,               /* Modify image */
                    PixelPacket *pixels,        /* Pixel row */
                    IndexPacket *indexes,       /* Pixel row indexes */
                    const long npixels,         /* Number of pixels in row */
                    ExceptionInfo *exception)   /* Exception report */
{
  /*
    3D transform pixels to RGB.
  */
  float
    b,
    g,
    r;

  const RGBTransformInfo_t
    *xform = (const RGBTransformInfo_t *) immutable_data;

  register long
    i;

  unsigned int
    r_index,
    g_index,
    b_index;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  for (i=0; i < npixels; i++)
    {
      r_index = ScaleQuantumToMap(pixels[i].red);
      g_index = ScaleQuantumToMap(pixels[i].green);
      b_index = ScaleQuantumToMap(pixels[i].blue);
  
      r = (xform->r[r_index].r + xform->g[g_index].r + xform->b[b_index].r);
      g = (xform->r[r_index].g + xform->g[g_index].g + xform->b[b_index].g);
      b = (xform->r[r_index].b + xform->g[g_index].b + xform->b[b_index].b);
  
      r = r < 0.0f ? 0.0f : r > MaxMapFloat ? MaxMapFloat : (r + 0.5f);
      g = g < 0.0f ? 0.0f : g > MaxMapFloat ? MaxMapFloat : (g + 0.5f);
      b = b < 0.0f ? 0.0f : b > MaxMapFloat ? MaxMapFloat : (b + 0.5f);

      if ( xform->rgb_map != 0 )
        {
          r_index = ScaleMapToChar(r);
          g_index = ScaleMapToChar(g);
          b_index = ScaleMapToChar(b);

          if (r_index > xform->rgb_map_max_index) r_index = xform->rgb_map_max_index;
          if (g_index > xform->rgb_map_max_index) g_index = xform->rgb_map_max_index;
          if (b_index > xform->rgb_map_max_index) b_index = xform->rgb_map_max_index;

          pixels[i].red   = ScaleCharToQuantum(xform->rgb_map[r_index]);
          pixels[i].green = ScaleCharToQuantum(xform->rgb_map[g_index]);
          pixels[i].blue  = ScaleCharToQuantum(xform->rgb_map[b_index]);
        }
      else
        {
          pixels[i].red   = ScaleMapToQuantum(r);
          pixels[i].green = ScaleMapToQuantum(g);
          pixels[i].blue  = ScaleMapToQuantum(b);
        }
    }

  return MagickPass;
}

MagickExport MagickPassFail TransformRGBImage(Image *image,
                                              const ColorspaceType colorspace)
{
  static const unsigned char
    sRGBMap[351] =
    {
      0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
      19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 29, 30, 31, 32, 33,
      34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
      50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 65, 66,
      67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82,
      83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 95, 96, 97, 98, 99,
      100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112,
      114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126,
      127, 128, 129, 130, 131, 132, 133, 135, 136, 137, 138, 139, 140,
      141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153,
      154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166,
      167, 168, 169, 170, 171, 172, 173, 174, 175, 175, 176, 177, 178,
      179, 180, 181, 182, 183, 184, 185, 186, 187, 187, 188, 189, 190,
      191, 192, 193, 194, 194, 195, 196, 197, 198, 199, 199, 200, 201,
      202, 203, 203, 204, 205, 206, 207, 207, 208, 209, 210, 210, 211,
      212, 213, 213, 214, 215, 215, 216, 217, 218, 218, 219, 220, 220,
      221, 222, 222, 223, 223, 224, 225, 225, 226, 227, 227, 228, 228,
      229, 229, 230, 230, 231, 232, 232, 233, 233, 234, 234, 235, 235,
      235, 236, 236, 237, 237, 238, 238, 238, 239, 239, 240, 240, 240,
      241, 241, 242, 242, 242, 243, 243, 243, 243, 244, 244, 244, 245,
      245, 245, 245, 246, 246, 246, 247, 247, 247, 247, 247, 248, 248,
      248, 248, 249, 249, 249, 249, 249, 249, 250, 250, 250, 250, 250,
      250, 251, 251, 251, 251, 251, 251, 252, 252, 252, 252, 252, 252,
      252, 252, 252, 253, 253, 253, 253, 253, 253, 253, 253, 253, 254,
      254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 255, 255,
      255, 255, 255, 255, 255
    };
  
  static const unsigned char
    YCCMap[351] =  /* Photo CD information beyond 100% white, Gamma 2.2 */
    {
      0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
      19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 32, 33, 34, 35,
      36, 37, 38, 39, 40, 41, 42, 43, 45, 46, 47, 48, 49, 50, 51, 52,
      53, 54, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68, 69, 70,
      71, 72, 73, 74, 76, 77, 78, 79, 80, 81, 82, 83, 84, 86, 87, 88,
      89, 90, 91, 92, 93, 94, 95, 97, 98, 99, 100, 101, 102, 103, 104,
      105, 106, 107, 108, 110, 111, 112, 113, 114, 115, 116, 117, 118,
      119, 120, 121, 122, 123, 124, 125, 126, 127, 129, 130, 131, 132,
      133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145,
      146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158,
      159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171,
      172, 173, 174, 175, 176, 176, 177, 178, 179, 180, 181, 182, 183,
      184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 193, 194, 195,
      196, 197, 198, 199, 200, 201, 201, 202, 203, 204, 205, 206, 207,
      207, 208, 209, 210, 211, 211, 212, 213, 214, 215, 215, 216, 217,
      218, 218, 219, 220, 221, 221, 222, 223, 224, 224, 225, 226, 226,
      227, 228, 228, 229, 230, 230, 231, 232, 232, 233, 234, 234, 235,
      236, 236, 237, 237, 238, 238, 239, 240, 240, 241, 241, 242, 242,
      243, 243, 244, 244, 245, 245, 245, 246, 246, 247, 247, 247, 248,
      248, 248, 249, 249, 249, 249, 250, 250, 250, 250, 251, 251, 251,
      251, 251, 252, 252, 252, 252, 252, 253, 253, 253, 253, 253, 253,
      253, 253, 253, 253, 253, 253, 253, 254, 254, 254, 254, 254, 254,
      254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255
    };

  char
    progress_message[MaxTextExtent];

  register long
    i;

  unsigned int
    is_grayscale;

  MagickPassFail
    status=MagickPass;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->colorspace != UndefinedColorspace);

  is_grayscale=((image->is_grayscale) || IsGrayColorspace(image->colorspace));

  /*
    If colorspace is already an RGB type then simply return.
  */
  if (IsRGBColorspace(image->colorspace))
    return(status);

  (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                        "Transform colorspace from %s to RGB",
                        ColorspaceTypeToString(image->colorspace));

  FormatString(progress_message,"[%%s] Transform colorspace from %s to RGB...",
               ColorspaceTypeToString(image->colorspace));

  if (image->colorspace == CMYKColorspace)
    {
      if (image->storage_class == PseudoClass)
        {
          status=SyncImage(image);
          image->storage_class=DirectClass;
        }
      if (status != MagickFail)
        status=PixelIterateMonoModify(CMYKToRGBTransform,
                                      NULL,
                                      progress_message,
                                      NULL,NULL,
                                      0,0,image->columns,image->rows,
                                      image,
                                      &image->exception);
      image->colorspace=RGBColorspace;
      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                            "Colorspace transform completed"); 
      return(status);
    }

  if (colorspace == CineonLogRGBColorspace)
    {
      /*
        Transform from Cineon Log RGB to Linear RGB
      */
      double
        BreakPoint,
        DisplayGamma,
        Gain,
        KneeGain,
        KneeOffset,
        MaxLinearValue,
        NegativeFilmGamma,
        Offset,
        ReferenceBlack,
        ReferenceWhite,
        SoftClip;

      Quantum
        *linearmap;

      /*
        Establish defaults.
      */
      MaxLinearValue=MaxRGB; /* Maximum linear value output */
      ReferenceWhite=685.0;  /* 90% white card (default 685) */
      ReferenceBlack=95.0;   /* 1% black card  (default 95) */
      DisplayGamma=1.7;      /* Typical display gamma (Kodak recommended 1.7) */
      NegativeFilmGamma=0.6; /* Typical gamma for a film negative */
      SoftClip=0.0;          /* Soft clip offset */

      /*
        Allow image attributes to override defaults.
      */
      MagickAttributeToDouble(image,"reference-white",ReferenceWhite);
      MagickAttributeToDouble(image,"reference-black",ReferenceBlack);
      MagickAttributeToDouble(image,"display-gamma",DisplayGamma);
      MagickAttributeToDouble(image,"film-gamma",NegativeFilmGamma);
      MagickAttributeToDouble(image,"soft-clip-offset",SoftClip);

      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                            "Log Transform: ReferenceWhite=%g ReferenceBlack=%g DisplayGamma=%g FilmGamma=%g SoftClip=%g",
                            ReferenceWhite,ReferenceBlack,DisplayGamma,NegativeFilmGamma,SoftClip);

      BreakPoint=ReferenceWhite-SoftClip;
      Gain=MaxLinearValue/(1.0 - pow(pow(10,((ReferenceBlack-ReferenceWhite)
                                             *0.002/NegativeFilmGamma)),
                                     (DisplayGamma/1.7)));
      Offset=Gain-MaxLinearValue;
      KneeOffset=pow(pow(10,((BreakPoint-ReferenceWhite)*0.002/NegativeFilmGamma)),
                     (DisplayGamma/1.7))*Gain-Offset;
      KneeGain=(MaxLinearValue-KneeOffset)/pow((5*SoftClip),(SoftClip/100));


      linearmap=MagickAllocateMemory(Quantum *,1024*sizeof(Quantum));
      if (linearmap == 0)
        ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
                              UnableToTransformColorspace);

      for (i=0; i < 1023; i++)
        {
          double
            linearval,
            logval;

          logval=i;
          if (logval < ReferenceBlack)
            {
              /* Values below reference black are clipped to zero */
              linearval=0.0;
            }
          else if (logval > BreakPoint)
            {
              /* Values above the breakpoint are soft-clipped. */
              linearval=pow((logval-BreakPoint),(SoftClip/100))*KneeGain+KneeOffset;
            }
          else
            {
              /* Otherwise, normal values */
              linearval=pow(pow(10,((logval-ReferenceWhite)*0.002/NegativeFilmGamma)),
                            (DisplayGamma/1.7))*Gain-Offset;
            }

          linearmap[i]=(unsigned int) (linearval + 0.5);
        }
      /*
        Transform pixels.
      */
      if (image->storage_class == PseudoClass)
        {
          /*
            Convert PseudoClass image colormap.
          */
          CineonLogToRGBTransform(NULL,
                                  linearmap,
                                  image,
                                  image->colormap,
                                  (IndexPacket *) NULL,
                                  image->colors,
                                  &image->exception);
          status=SyncImage(image);
        }
      else
        {
          /*
            Convert DirectClass image.
          */
          status=PixelIterateMonoModify(CineonLogToRGBTransform,
                                        NULL,
                                        progress_message,
                                        NULL,linearmap,
                                        0,0,image->columns,image->rows,
                                        image,
                                        &image->exception);
        }
      MagickFreeMemory(linearmap);
      image->colorspace=RGBColorspace;
      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                            "Transform to colorspace %s completed",
                            ColorspaceTypeToString(colorspace));
      return(status);
    }

  if (image->colorspace == HSLColorspace)
    {
      if (image->storage_class == PseudoClass)
        {
          /*
            Convert PseudoClass image colormap.
          */
          HSLToRGBTransform(NULL,
                            NULL,
                            image,
                            image->colormap,
                            (IndexPacket *) NULL,
                            image->colors,
                            &image->exception);
          status=SyncImage(image);
        }
      else
        {
          status=PixelIterateMonoModify(HSLToRGBTransform,
                                        NULL,
                                        progress_message,
                                        NULL,NULL,
                                        0,0,image->columns,image->rows,
                                        image,
                                        &image->exception);
        }
      image->colorspace=RGBColorspace;
      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                            "Colorspace transform completed"); 
      return(status);
    }

  if (image->colorspace == HWBColorspace)
    {
      if (image->storage_class == PseudoClass)
        {
          /*
            Convert PseudoClass image colormap.
          */
          HWBToRGBTransform(NULL,
                            NULL,
                            image,
                            image->colormap,
                            (IndexPacket *) NULL,
                            image->colors,
                            &image->exception);
          status=SyncImage(image);
        }
      else
        {
          status=PixelIterateMonoModify(HWBToRGBTransform,
                                        NULL,
                                        progress_message,
                                        NULL,NULL,
                                        0,0,image->columns,image->rows,
                                        image,
                                        &image->exception);
        }
      image->colorspace=RGBColorspace;
      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                            "Colorspace transform completed"); 
      return(status);
    }

  {
    /*
      3D Transform.
    */

    RGBTransformInfo_t
      xform;

    /*
      Allocate the tables.
    */
    xform.rgb_map = (const unsigned char *) NULL;
    xform.rgb_map_max_index = 0;
    xform.r=MagickAllocateMemory(RGBColorTransformPacket *,
                                 (MaxMap+1)*sizeof(RGBColorTransformPacket));
    xform.g=MagickAllocateMemory(RGBColorTransformPacket *,
                                 (MaxMap+1)*sizeof(RGBColorTransformPacket));
    xform.b=MagickAllocateMemory(RGBColorTransformPacket *,
                                 (MaxMap+1)*sizeof(RGBColorTransformPacket));
    if ((xform.r == (RGBColorTransformPacket *) NULL) ||
        (xform.g == (RGBColorTransformPacket *) NULL) ||
        (xform.b == (RGBColorTransformPacket *) NULL))
      {
        MagickFreeMemory(xform.r);
        MagickFreeMemory(xform.g);
        MagickFreeMemory(xform.b);
        ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
                              UnableToTransformColorspace);
      }

    switch (image->colorspace)
      {
      case OHTAColorspace:
        {
          /*
            Initialize OHTA tables:

            R = I1+1.00000*I2-0.66668*I3
            G = I1+0.00000*I2+1.33333*I3
            B = I1-1.00000*I2-0.66668*I3

            I and Q, normally -0.5 through 0.5, must be normalized to the range 0
            through MaxMap.
          */
#if MaxMap > 255
#  if defined(HAVE_OPENMP)
#    pragma omp parallel for schedule(static,64)
#  endif
#endif
          for (i=0; i <= (long) MaxMap; i++)
            {
              xform.r[i].r=((float) i);
              xform.g[i].r=(0.5f*(2.0f*(float) i-MaxMapFloat));
              xform.b[i].r=((-0.33334f)*(2.0f*(float) i-MaxMapFloat));
              xform.r[i].g=((float) i);
              xform.g[i].g=(0.0f);
              xform.b[i].g=(0.666665f*(2.0f*(float) i-MaxMapFloat));
              xform.r[i].b=((float) i);
              xform.g[i].b=((-0.5f)*(2.0f*(float) i-MaxMapFloat));
              xform.b[i].b=((-0.33334f)*(2.0f*(float) i-MaxMapFloat));
            }
          break;
        }
      case sRGBColorspace:
        {
          /*
            Initialize sRGB tables:

            R = Y            +1.032096*C2
            G = Y-0.326904*C1-0.704445*C2
            B = Y+1.685070*C1

            sRGB is scaled by 1.3584.  C1 zero is 156 and C2 is at 137.
          */
          xform.rgb_map=sRGBMap;
          xform.rgb_map_max_index=350;
#if MaxMap > 255
#  if defined(HAVE_OPENMP)
#    pragma omp parallel for schedule(static,64)
#  endif
#endif
          for (i=0; i <= (long) MaxMap; i++)
            {
              xform.r[i].r=(1.40200f*(float) i);
              xform.g[i].r=(0.0f);
              xform.b[i].r=(1.88000f*((float) i-ScaleCharToMap(137)));
              xform.r[i].g=(1.40200f*(float) i);
              xform.g[i].g=((-0.444066f)*((float) i-ScaleCharToMap(156)));
              xform.b[i].g=((-0.95692f)*((float) i-ScaleCharToMap(137)));
              xform.r[i].b=(1.40200f*(float) i);
              xform.g[i].b=(2.28900f*((float) i-ScaleCharToMap(156)));
              xform.b[i].b=(0.0f);
            }
          break;
        }
      case XYZColorspace:
        {
          /*
            Initialize CIE XYZ tables (to ITU R-709 RGB):

            R =  3.240479*R-1.537150*G-0.498535*B
            G = -0.969256*R+1.875992*G+0.041556*B
            B =  0.055648*R-0.204043*G+1.057311*B
          */
#if MaxMap > 255
#  if defined(HAVE_OPENMP)
#    pragma omp parallel for schedule(static,64)
#  endif
#endif
          for (i=0; i <= (long) MaxMap; i++)
            {
              xform.r[i].r=(3.240479f*(float) i);
              xform.g[i].r=((-1.537150f)*(float) i);
              xform.b[i].r=((-0.498535f)*(float) i);
              xform.r[i].g=((-0.969256f)*(float) i);
              xform.g[i].g=(1.875992f*(float) i);
              xform.b[i].g=(0.041556f*(float) i);
              xform.r[i].b=(0.055648f*(float) i);
              xform.g[i].b=((-0.204043f)*(float) i);
              xform.b[i].b=(1.057311f*(float) i);
            }
          break;
        }
      case Rec601YCbCrColorspace:
        {
          /*
            Y'CbCr based on ITU-R 601 Luma

            Initialize Y'CbCr tables:

            R' = Y'            +1.402000*Cr
            G' = Y'-0.344136*Cb-0.714136*Cr
            B' = Y'+1.772000*Cb

            Cb and Cr, normally -0.5 through 0.5, must be normalized to the range 0
            through MaxMap.
          */
#if MaxMap > 255
#  if defined(HAVE_OPENMP)
#    pragma omp parallel for schedule(static,64)
#  endif
#endif
          for (i=0; i <= (long) MaxMap; i++)
            {
              /* Y */
              xform.r[i].r=((float) i);
              xform.g[i].r=(0.0f);
              xform.b[i].r=((1.402000f*0.5f)*(2.0f*(float) i-MaxMapFloat));
              /* Pb */
              xform.r[i].g=((float) i);
              xform.g[i].g=((-0.344136f*0.5f)*(2.0f*(float) i-MaxMapFloat));
              xform.b[i].g=((-0.714136f*0.5f)*(2.0f*(float) i-MaxMapFloat));
              /* Pr */
              xform.r[i].b=((float) i);
              xform.g[i].b=((1.772000f*0.5f)*(2.0f*(float) i-MaxMapFloat));
              xform.b[i].b=(0.0f);
            }
          break;
        }
      case Rec709YCbCrColorspace:
        {
          /*
            Y'CbCr based on ITU-R 709 Luma.

            R' = Y'            +1.574800*Cr
            G' = Y'-0.187324*Cb-0.468124*Cr
            B' = Y'+1.855600*Cb

            Cb and Cr, normally -0.5 through 0.5, must be normalized to the range 0
            through MaxMap.
          */
#if MaxMap > 255
#  if defined(HAVE_OPENMP)
#    pragma omp parallel for schedule(static,64)
#  endif
#endif
          for (i=0; i <= (long) MaxMap; i++)
            {
              /* Y */
              xform.r[i].r=((float) i);
              xform.g[i].r=(0.0f);
              xform.b[i].r=((1.5748f*0.5f)*(2.0f*(float) i-MaxMapFloat));
              /* Pb */
              xform.r[i].g=((float) i);
              xform.g[i].g=((-0.187324f*0.5f)*(2.0f*(float) i-MaxMapFloat));
              xform.b[i].g=((-0.468124f*0.5f)*(2.0f*(float) i-MaxMapFloat));
              /* Pr */
              xform.r[i].b=((float) i);
              xform.g[i].b=((1.8556f*0.5f)*(2.0f*(float) i- MaxMapFloat));
              xform.b[i].b=(0.0f);
            }
          break;
        }
      case YCCColorspace:
        {
          /*
            Kodak PhotoYCC Color Space.

            Initialize YCC tables:

            R = Y            +1.340762*C2
            G = Y-0.317038*C1-0.682243*C2
            B = Y+1.632639*C1

            YCC is scaled by 1.3584.  C1 zero is 156 and C2 is at 137.
          */
          xform.rgb_map=YCCMap;
          xform.rgb_map_max_index=350;
#if MaxMap > 255
#  if defined(HAVE_OPENMP)
#    pragma omp parallel for schedule(static,64)
#  endif
#endif
          for (i=0; i <= (long) MaxMap; i++)
            {
              xform.r[i].r=(1.3584f*(float) i);
              xform.g[i].r=(0.0f);
              xform.b[i].r=(1.8215f*((float) i-ScaleCharToMap(137)));
              xform.r[i].g=(1.3584f*(float) i);
              xform.g[i].g=((-0.4302726f)*((float) i-ScaleCharToMap(156)));
              xform.b[i].g=((-0.9271435f)*((float) i-ScaleCharToMap(137)));
              xform.r[i].b=(1.3584f*i);
              xform.g[i].b=(2.2179f*((float) i-ScaleCharToMap(156)));
              xform.b[i].b=(0.0f);
            }
          break;
        }
      case YIQColorspace:
        {
          /*
            Initialize YIQ tables:

            R = Y+0.95620*I+0.62140*Q
            G = Y-0.27270*I-0.64680*Q
            B = Y-1.10370*I+1.70060*Q

            I and Q, normally -0.5 through 0.5, must be normalized to the range 0
            through MaxMap.
          */
#if MaxMap > 255
#  if defined(HAVE_OPENMP)
#    pragma omp parallel for schedule(static,64)
#  endif
#endif
          for (i=0; i <= (long) MaxMap; i++)
            {
              xform.r[i].r=((float) i);
              xform.g[i].r=(0.4781f*(2.0f*(float) i-MaxMapFloat));
              xform.b[i].r=(0.3107f*(2.0f*(float) i-MaxMapFloat));
              xform.r[i].g=((float) i);
              xform.g[i].g=((-0.13635f)*(2.0f*(float) i-MaxMapFloat));
              xform.b[i].g=((-0.3234f)*(2.0f*(float) i-MaxMapFloat));
              xform.r[i].b=((float) i);
              xform.g[i].b=((-0.55185f)*(2.0f*(float) i-MaxMapFloat));
              xform.b[i].b=(0.8503f*(2.0f*(float) i-MaxMapFloat));
            }
          break;
        }
      case YPbPrColorspace:
        {
          /*
            Initialize Y'PbPr tables using ITU-R 601 luma:

            R = Y            +1.402000*C2
            G = Y-0.344136*C1+0.714136*C2
            B = Y+1.772000*C1

            Pb and Pr, normally -0.5 through 0.5, must be normalized to the range 0
            through MaxMap.
          */
#if MaxMap > 255
#  if defined(HAVE_OPENMP)
#    pragma omp parallel for schedule(static,64)
#  endif
#endif
          for (i=0; i <= (long) MaxMap; i++)
            {
              xform.r[i].r=((float) i);
              xform.g[i].r=(0.0f);
              xform.b[i].r=(0.701f*(2.0f*(float) i-MaxMapFloat));
              xform.r[i].g=((float) i);
              xform.g[i].g=((-0.172068f)*(2.0f*(float) i-MaxMapFloat));
              xform.b[i].g=(0.357068f*(2.0f*(float) i-MaxMapFloat));
              xform.r[i].b=((float) i);
              xform.g[i].b=(0.886f*(2.0f*(float) i-MaxMapFloat));
              xform.b[i].b=(0.0f);
            }
          break;
        }
      case YUVColorspace:
      default:
        {
          /*
            Initialize YUV tables:

            R = Y          +1.13980*V
            G = Y-0.39380*U-0.58050*V
            B = Y+2.02790*U

            U and V, normally -0.5 through 0.5, must be normalized to the range 0
            through MaxMap.
          */
#if MaxMap > 255
#  if defined(HAVE_OPENMP)
#    pragma omp parallel for schedule(static,64)
#  endif
#endif
          for (i=0; i <= (long) MaxMap; i++)
            {
              xform.r[i].r=((float) i);
              xform.g[i].r=(0.0f);
              xform.b[i].r=(0.5699f*(2.0f*(float) i-MaxMapFloat));
              xform.r[i].g=((float) i);
              xform.g[i].g=((-0.1969f)*(2.0f*(float) i-MaxMapFloat));
              xform.b[i].g=((-0.29025f)*(2.0f*(float) i-MaxMapFloat));
              xform.r[i].b=((float) i);
              xform.g[i].b=(1.01395f*(2.0f*(float) i-MaxMapFloat));
              xform.b[i].b=(0.0f);
            }
          break;
        }
      }

#if 0
    /*
      Dump tables
    */
    for (i=0; i <= (long) MaxMap; i++)
      {
        printf("%5ld: xform.r(%g,%g,%g) xform.g(%g,%g,%g) xform.b(%g,%g,%g)\n",
               i,
               ((xform.r[i].r)),
               ((xform.r[i].g)),
               ((xform.r[i].b)),
             
               ((xform.g[i].r)),
               ((xform.g[i].g)),
               ((xform.g[i].b)),
             
               ((xform.b[i].r)),
               ((xform.b[i].g)),
               ((xform.b[i].b)));
      }
#endif

    /*
      Convert to RGB.
    */
    if (image->storage_class == PseudoClass)
      {
        /*
          Convert PseudoClass image colormap.
        */
        (void) RGBTransformPackets(NULL,
                                   &xform,
                                   image,
                                   image->colormap,
                                   (IndexPacket *) NULL,
                                   image->colors,
                                   &image->exception);
        status=SyncImage(image);
      }
    else
      {
        /*
          Convert DirectClass image.
        */
        status=PixelIterateMonoModify(RGBTransformPackets,
                                      NULL,
                                      progress_message,
                                      NULL,&xform,
                                      0,0,image->columns,image->rows,
                                      image,
                                      &image->exception);
      }
    

    /*
      Free allocated memory.
    */
    MagickFreeMemory(xform.b);
    MagickFreeMemory(xform.g);
    MagickFreeMemory(xform.r);
  }
  image->is_grayscale=is_grayscale;
  image->colorspace=RGBColorspace;
  (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                        "Colorspace transform completed"); 
  return(status);
}
