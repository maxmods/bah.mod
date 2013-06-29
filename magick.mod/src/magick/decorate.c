/*
% Copyright (C) 2003 GraphicsMagick Group
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
%          DDDD   EEEEE   CCCC   OOO   RRRR    AAA   TTTTT  EEEEE             %
%          D   D  E      C      O   O  R   R  A   A    T    E                 %
%          D   D  EEE    C      O   O  RRRR   AAAAA    T    EEE               %
%          D   D  E      C      O   O  R R    A   A    T    E                 %
%          DDDD   EEEEE   CCCC   OOO   R  R   A   A    T    EEEEE             %
%                                                                             %
%                                                                             %
%                   GraphicsMagick Image Decoration Methods                   %
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
#include "magick/color.h"
#include "magick/decorate.h"
#include "magick/pixel_cache.h"
#include "magick/monitor.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   B o r d e r I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  BorderImage() surrounds the image with a border of the color defined by
%  the bordercolor member of the image structure.  The width and height
%  of the border are defined by the corresponding members of the border_info
%  structure.
%
%  The format of the BorderImage method is:
%
%      Image *BorderImage(const Image *image,const RectangleInfo *border_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o border_info:  Define the width and height of the border.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *BorderImage(const Image *image,
  const RectangleInfo *border_info,ExceptionInfo *exception)
{
  Image
    *border_image,
    *clone_image;

  FrameInfo
    frame_info;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(border_info != (RectangleInfo *) NULL);
  frame_info.width=image->columns+(border_info->width << 1);
  frame_info.height=image->rows+(border_info->height << 1);
  frame_info.x=(long) border_info->width;
  frame_info.y=(long) border_info->height;
  frame_info.inner_bevel=0;
  frame_info.outer_bevel=0;
  clone_image=CloneImage(image,0,0,True,exception);
  if (clone_image == (Image *) NULL)
    return((Image *) NULL);
  clone_image->matte_color=image->border_color;
  border_image=FrameImage(clone_image,&frame_info,exception);
  DestroyImage(clone_image);
  if (border_image != (Image *) NULL)
    border_image->matte_color=image->matte_color;
  return(border_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   F r a m e I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  FrameImage() adds a simulated three-dimensional border around the image.
%  The color of the border is defined by the matte_color member of image.
%  Members width and height of frame_info specify the border width of the
%  vertical and horizontal sides of the frame.  Members inner and outer
%  indicate the width of the inner and outer shadows of the frame.
%
%  The format of the FrameImage method is:
%
%      Image *FrameImage(const Image *image,const FrameInfo *frame_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o frame_info: Define the width and height of the frame and its bevels.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *FrameImage(const Image *image,const FrameInfo *frame_info,
                               ExceptionInfo *exception)
{
#define FrameImageText "[%s] Frame: %lux%lu%+ld%+ld bevel inner %ld outer %ld..."

  Image
    *frame_image;

  unsigned long
    row_count=0;

  long
    height,
    width,
    y;

  register const PixelPacket
    *p;

  register long
    x;

  register PixelPacket
    *q;

  PixelPacket
    accentuate,
    highlight,
    matte,
    shadow,
    trough;

  unsigned long
    bevel_width;

  unsigned int
    is_grayscale;

  MagickPassFail
    status=MagickPass;

  /*
    Check frame geometry.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(frame_info != (FrameInfo *) NULL);
  is_grayscale=image->is_grayscale;
  if ((frame_info->outer_bevel < 0) || (frame_info->inner_bevel < 0))
    ThrowImageException3(OptionError,UnableToFrameImage,
                         BevelWidthIsNegative);
  bevel_width=frame_info->outer_bevel+frame_info->inner_bevel;
  width=(long) (frame_info->width-frame_info->x-bevel_width);
  height=(long) (frame_info->height-frame_info->y-bevel_width);
  if ((width < (long) image->columns) || (height < (long) image->rows))
    ThrowImageException3(OptionError,UnableToFrameImage,
                         FrameIsLessThanImageSize);
  /*
    Allocate image
  */
  frame_image=
    CloneImage(image,frame_info->width,frame_info->height,True,exception);
  if (frame_image == (Image *) NULL)
    return(False);

  /*
    Set image type.
  */
  (void) SetImageType(frame_image,frame_image->matte_color.opacity !=
                      OpaqueOpacity ? TrueColorMatteType : TrueColorType);

  /*
    Initialize 3D effects color.
  */
  matte=image->matte_color;
  if (!IsGray(image->matte_color))
    is_grayscale=False;
  accentuate.red=(Quantum) ((((double) MaxRGB-AccentuateModulate)*matte.red+
                             ((double) MaxRGB*AccentuateModulate))/MaxRGB+0.5);
  accentuate.green=(Quantum) ((((double) MaxRGB-AccentuateModulate)*matte.green+
                               ((double) MaxRGB*AccentuateModulate))/MaxRGB+0.5);
  accentuate.blue=(Quantum) ((((double) MaxRGB-AccentuateModulate)*matte.blue+
                              ((double) MaxRGB*AccentuateModulate))/MaxRGB+0.5);
  accentuate.opacity=(Quantum) ((((double) MaxRGB-AccentuateModulate)*
                                 matte.opacity+((double) MaxRGB*AccentuateModulate))/MaxRGB+0.5);
  highlight.red=(Quantum) ((((double) MaxRGB-HighlightModulate)*matte.red+
                            ((double) MaxRGB*HighlightModulate))/MaxRGB+0.5);
  highlight.green=(Quantum) ((((double) MaxRGB-HighlightModulate)*matte.green+
                              ((double) MaxRGB*HighlightModulate))/MaxRGB+0.5);
  highlight.blue=(Quantum) ((((double) MaxRGB-HighlightModulate)*matte.blue+
                             ((double) MaxRGB*HighlightModulate))/MaxRGB+0.5);
  highlight.opacity=(Quantum) ((((double) MaxRGB-HighlightModulate)*
                                matte.opacity+((double) MaxRGB*HighlightModulate))/MaxRGB+0.5);
  shadow.red=(Quantum) (((double) matte.red*ShadowModulate)/MaxRGB+0.5);
  shadow.green=(Quantum) (((double) matte.green*ShadowModulate)/MaxRGB+0.5);
  shadow.blue=(Quantum) (((double) matte.blue*ShadowModulate)/MaxRGB+0.5);
  shadow.opacity=(Quantum) (((double) matte.opacity*ShadowModulate)/MaxRGB+0.5);
  trough.red=(Quantum) (((double) matte.red*TroughModulate)/MaxRGB+0.5);
  trough.green=(Quantum) (((double) matte.green*TroughModulate)/MaxRGB+0.5);
  trough.blue=(Quantum) (((double) matte.blue*TroughModulate)/MaxRGB+0.5);
  trough.opacity=(Quantum) (((double) matte.opacity*TroughModulate)/MaxRGB+0.5);
  /*
    Draw top of ornamental border.
  */
  height=(long) (frame_info->outer_bevel+(frame_info->y-bevel_width)+
                 frame_info->inner_bevel);
  q=SetImagePixelsEx(frame_image,0,0,frame_image->columns,height,exception);
  if (q != (PixelPacket *) NULL)
    {
      for (y=0; y < frame_info->outer_bevel; y++)
        {
          for (x=0; x < (long) (frame_image->columns-y); x++)
            if (x < y)
              *q++=highlight;
            else
              *q++=accentuate;
          for ( ; x < (long) frame_image->columns; x++)
            *q++=shadow;
        }
      for (y=0; y < (long) (frame_info->y-bevel_width); y++)
        {
          for (x=0; x < frame_info->outer_bevel; x++)
            *q++=highlight;
          width=(long) (frame_image->columns-2*frame_info->outer_bevel);
          for (x=0; x < (long) width; x++)
            *q++=matte;
          for (x=0; x < frame_info->outer_bevel; x++)
            *q++=shadow;
        }
      for (y=0; y < frame_info->inner_bevel; y++)
        {
          for (x=0; x < frame_info->outer_bevel; x++)
            *q++=highlight;
          for (x=0; x < (long) (frame_info->x-bevel_width); x++)
            *q++=matte;
          width=(long) (image->columns+(frame_info->inner_bevel << 1)-y);
          for (x=0; x < width; x++)
            if (x < y)
              *q++=shadow;
            else
              *q++=trough;
          for ( ; x < (long) (image->columns+(frame_info->inner_bevel << 1)); x++)
            *q++=highlight;
          width=(long)
            (frame_info->width-frame_info->x-image->columns-bevel_width);
          for (x=0; x < width; x++)
            *q++=matte;
          for (x=0; x < frame_info->outer_bevel; x++)
            *q++=shadow;
        }
      (void) SyncImagePixelsEx(frame_image,exception);
    }
  /*
    Draw sides of ornamental border.
  */
#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  if defined(TUNE_OPENMP)
#    pragma omp parallel for schedule(runtime) shared(row_count, status) private (p, q, width, x)
#  else
#    pragma omp parallel for schedule(static,8) shared(row_count, status) private (p, q, width, x)
#  endif
#endif
  for (y=0; y < (long) image->rows; y++)
    {
      MagickBool
        thread_status;

#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  pragma omp critical (GM_FrameImage)
#endif    
      thread_status=status;
      if (thread_status == MagickFail)
        continue;

      /*
        Initialize scanline with border color.
      */
      p=AcquireImagePixels(image,0,y,image->columns,1,exception);
      q=SetImagePixelsEx(frame_image,0,frame_info->y+y,frame_image->columns,1,exception);
      if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
        thread_status=MagickFail;

      if (thread_status != MagickFail)
        {
          for (x=0; x < frame_info->outer_bevel; x++)
            *q++=highlight;
          for (x=0; x < (long) (frame_info->x-bevel_width); x++)
            *q++=matte;
          for (x=0; x < frame_info->inner_bevel; x++)
            *q++=shadow;
          /*
            Transfer scanline.
          */
          (void) memcpy(q,p,image->columns*sizeof(PixelPacket));
          q+=image->columns;
          for (x=0; x < frame_info->inner_bevel; x++)
            *q++=highlight;
          width=(long) (frame_info->width-frame_info->x-image->columns-bevel_width);
          for (x=0; x < width; x++)
            *q++=matte;
          for (x=0; x < frame_info->outer_bevel; x++)
            *q++=shadow;
          if (!SyncImagePixelsEx(frame_image,exception))
            thread_status=MagickFail;
        }
#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  pragma omp critical (GM_FrameImage)
#endif
      {
        row_count++;
        if (QuantumTick(row_count,image->rows))
          if (!MagickMonitorFormatted(row_count,image->rows,exception,
                                      FrameImageText,image->filename,
				      frame_info->width,frame_info->height,
				      frame_info->x,
				      frame_info->y,
				      frame_info->inner_bevel,
				      frame_info->outer_bevel))
            thread_status=MagickFail;
          
        if (thread_status == MagickFail)
          status=MagickFail;
      }
    }
  /*
    Draw bottom of ornamental border.
  */
  height=(long) (frame_info->inner_bevel+frame_info->height-frame_info->y-
                 image->rows-bevel_width+frame_info->outer_bevel);
  q=SetImagePixelsEx(frame_image,0,(long) (frame_image->rows-height),
                        frame_image->columns,height,exception);
  if (q == (PixelPacket *) NULL)
    return(frame_image);
  for (y=frame_info->inner_bevel-1; y >= 0; y--)
    {
      for (x=0; x < frame_info->outer_bevel; x++)
        *q++=highlight;
      for (x=0; x < (long) (frame_info->x-bevel_width); x++)
        *q++=matte;
      for (x=0; x < y; x++)
        *q++=shadow;
      for ( ; x < (long) (image->columns+(frame_info->inner_bevel << 1)); x++)
        if (x >= (long) (image->columns+(frame_info->inner_bevel << 1)-y))
          *q++=highlight;
        else
          *q++=accentuate;
      width=(long) (frame_info->width-frame_info->x-image->columns-bevel_width);
      for (x=0; x < (long) width; x++)
        *q++=matte;
      for (x=0; x < frame_info->outer_bevel; x++)
        *q++=shadow;
    }
  height=(long) (frame_info->height-frame_info->y-image->rows-bevel_width);
  for (y=0; y < height; y++)
    {
      for (x=0; x < frame_info->outer_bevel; x++)
        *q++=highlight;
      for (x=0; x < (long) (frame_image->columns-2*frame_info->outer_bevel); x++)
        *q++=matte;
      for (x=0; x < frame_info->outer_bevel; x++)
        *q++=shadow;
    }
  for (y=frame_info->outer_bevel-1; y >= 0; y--)
    {
      for (x=0; x < y; x++)
        *q++=highlight;
      for ( ; x < (long) frame_image->columns; x++)
        if (x >= (long) (frame_image->columns-y))
          *q++=shadow;
        else
          *q++=trough;
    }
  (void) SyncImagePixelsEx(frame_image,exception);

  frame_image->is_grayscale=is_grayscale;
  return(frame_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R a i s e I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  RaiseImage() creates a simulated three-dimensional button-like effect
%  by lightening and darkening the edges of the image.  Members width and
%  height of raise_info define the width of the vertical and horizontal
%  edge of the effect.
%
%  The format of the RaiseImage method is:
%
%      unsigned int RaiseImage(Image *image,const RectangleInfo *raise_info,
%        const int raise_flag)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o raise_info: Define the width and height of the raise area.
%
%    o raise_flag: A value other than zero creates a 3-D raise effect,
%      otherwise it has a lowered effect.
%
%
*/
#define AccentuateFactor  (double) ScaleCharToQuantum(135)
#define HighlightFactor  (double) ScaleCharToQuantum(190)
#define ShadowFactor  (double) ScaleCharToQuantum(190)
#define RaiseImageText  "[%s] Raise..."
#define TroughFactor  (double) ScaleCharToQuantum(135)

MagickExport MagickPassFail
RaiseImage(Image *image,const RectangleInfo *raise_info,const int raise_flag)
{
  unsigned long
    row_count=0;

  double
    foreground,
    background;

  long
    y;

  register long
    x;

  unsigned int
    is_grayscale;

  register PixelPacket
    *q;

  MagickPassFail
    status=MagickPass;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(raise_info != (RectangleInfo *) NULL);
  is_grayscale=image->is_grayscale;
  if ((image->columns <= (raise_info->width << 1)) ||
      (image->rows <= (raise_info->height << 1)))
    ThrowBinaryException3(OptionError,UnableToRaiseImage,
                          ImageSizeMustExceedBevelWidth);

  foreground=MaxRGBDouble;
  background=0.0;
  if (!raise_flag)
    {
      foreground=0.0;
      background=MaxRGBDouble;
    }
  (void) SetImageType(image,TrueColorType);

#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  if defined(TUNE_OPENMP)
#    pragma omp parallel for schedule(runtime) shared(row_count, status) private (q, x)
#  else
#    pragma omp parallel for schedule(static,4) shared(row_count, status) private (q, x)
#  endif
#endif
  for ( y=0; y < (long) image->rows; y++)
    {
      MagickBool
        thread_status;

#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  pragma omp critical (GM_RaiseImage)
#endif
      thread_status=status;
      if (thread_status == MagickFail)
        continue;

      q=GetImagePixelsEx(image,0,y,image->columns,1,&image->exception);
      if (q == (PixelPacket *) NULL)
        thread_status=MagickFail;

      if (thread_status != MagickFail)
        {
          if (y < (long) raise_info->height)
            {
              for (x=0; x < y; x++)
                {
                  q[x].red=(Quantum) ((q[x].red*HighlightFactor+
                                       foreground*(MaxRGBDouble-HighlightFactor))/MaxRGBDouble);
                  q[x].green=(Quantum) ((q[x].green*HighlightFactor+
                                         foreground*(MaxRGBDouble-HighlightFactor))/MaxRGBDouble);
                  q[x].blue=(Quantum) ((q[x].blue*HighlightFactor+
                                        foreground*(MaxRGBDouble-HighlightFactor))/MaxRGBDouble);
                }
              for ( ; x < (long) (image->columns-y); x++)
                {
                  q[x].red=(Quantum) ((q[x].red*AccentuateFactor+
                                       foreground*(MaxRGBDouble-AccentuateFactor))/MaxRGBDouble);
                  q[x].green=(Quantum) ((q[x].green*AccentuateFactor+
                                         foreground*(MaxRGBDouble-AccentuateFactor))/MaxRGBDouble);
                  q[x].blue=(Quantum) ((q[x].blue*AccentuateFactor+
                                        foreground*(MaxRGBDouble-AccentuateFactor))/MaxRGBDouble);
                }
              for ( ; x < (long) image->columns; x++)
                {
                  q[x].red=(Quantum) ((q[x].red*ShadowFactor+
                                       background*(MaxRGBDouble-ShadowFactor))/MaxRGBDouble);
                  q[x].green=(Quantum) ((q[x].green*ShadowFactor+
                                         background*(MaxRGBDouble-ShadowFactor))/MaxRGBDouble);
                  q[x].blue=(Quantum) ((q[x].blue*ShadowFactor+
                                        background*(MaxRGBDouble-ShadowFactor))/MaxRGBDouble);
                }
            }
          else if (y < (long) (image->rows-raise_info->height))
            {
              for (x=0; x < (long) raise_info->width; x++)
                {
                  q[x].red=(Quantum) ((q[x].red*HighlightFactor+
                                       foreground*(MaxRGBDouble-HighlightFactor))/MaxRGBDouble);
                  q[x].green=(Quantum) ((q[x].green*HighlightFactor+
                                         foreground*(MaxRGBDouble-HighlightFactor))/MaxRGBDouble);
                  q[x].blue=(Quantum) ((q[x].blue*HighlightFactor+
                                        foreground*(MaxRGBDouble-HighlightFactor))/MaxRGBDouble);
                }
              for ( ; x < (long) (image->columns-raise_info->width); x++);
              for ( ; x < (long) image->columns; x++)
                {
                  q[x].red=(Quantum) ((q[x].red*ShadowFactor+
                                       background*(MaxRGBDouble-ShadowFactor))/MaxRGBDouble);
                  q[x].green=(Quantum) ((q[x].green*ShadowFactor+
                                         background*(MaxRGBDouble-ShadowFactor))/MaxRGBDouble);
                  q[x].blue=(Quantum) ((q[x].blue*ShadowFactor+
                                        background*(MaxRGBDouble-ShadowFactor))/MaxRGBDouble);
                }
            }
          else
            {
              for (x=0; x < (long) (image->rows-y); x++)
                {
                  q[x].red=(Quantum) ((q[x].red*HighlightFactor+
                                       foreground*(MaxRGBDouble-HighlightFactor))/MaxRGBDouble+0.5);
                  q[x].green=(Quantum) ((q[x].green*HighlightFactor+
                                         foreground*(MaxRGBDouble-HighlightFactor))/MaxRGBDouble+0.5);
                  q[x].blue=(Quantum) ((q[x].blue*HighlightFactor+
                                        foreground*(MaxRGBDouble-HighlightFactor))/MaxRGBDouble+0.5);
                }
              for ( ; x < (long) (image->columns-(image->rows-y)); x++)
                {
                  q[x].red=(Quantum) ((q[x].red*TroughFactor+
                                       background*(MaxRGBDouble-TroughFactor))/MaxRGBDouble+0.5);
                  q[x].green=(Quantum) ((q[x].green*TroughFactor+
                                         background*(MaxRGBDouble-TroughFactor))/MaxRGBDouble+0.5);
                  q[x].blue=(Quantum) ((q[x].blue*TroughFactor+
                                        background*(MaxRGBDouble-TroughFactor))/MaxRGBDouble+0.5);
                }
              for ( ; x < (long) image->columns; x++)
                {
                  q[x].red=(Quantum) ((q[x].red*ShadowFactor+
                                       background*(MaxRGBDouble-ShadowFactor))/MaxRGBDouble+0.5);
                  q[x].green=(Quantum) ((q[x].green*ShadowFactor+
                                         background*(MaxRGBDouble-ShadowFactor))/MaxRGBDouble+0.5);
                  q[x].blue=(Quantum) ((q[x].blue*ShadowFactor+
                                        background*(MaxRGBDouble-ShadowFactor))/MaxRGBDouble+0.5);
                }
            }
          if (!SyncImagePixelsEx(image,&image->exception))
            thread_status=MagickFail;
        }
#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  pragma omp critical (GM_RaiseImage)
#endif
      {
        row_count++;
        if (QuantumTick(row_count,image->rows))
          if (!MagickMonitorFormatted(row_count,image->rows,&image->exception,
                                      RaiseImageText,image->filename))
            thread_status=MagickFail;
          
        if (thread_status == MagickFail)
          status=MagickFail;
      }
    }

  image->is_grayscale=is_grayscale;
  return(status);
}
