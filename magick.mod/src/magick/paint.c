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
%                      PPPP    AAA   IIIII  N   N  TTTTT                      %
%                      P   P  A   A    I    NN  N    T                        %
%                      PPPP   AAAAA    I    N N N    T                        %
%                      P      A   A    I    N  NN    T                        %
%                      P      A   A  IIIII  N   N    T                        %
%                                                                             %
%                                                                             %
%                        Methods to Paint on an Image                         %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1998                                   %
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
#include "magick/alpha_composite.h"
#include "magick/color.h"
#include "magick/monitor.h"
#include "magick/pixel_cache.h"
#include "magick/pixel_iterator.h"
#include "magick/render.h"
#include "magick/utility.h"

/*
  Define declarations.
*/
#define FuzzyOpacityMatch(color,target,fuzz) \
  (((color)->opacity == (target)->opacity) && \
   FuzzyColorMatch(color,target,fuzz))
#define MaxStacksize  (1 << 15)
#define Push(up,left,right,delta) \
  if ((s < (segment_stack+MaxStacksize)) && (((up)+(delta)) >= 0) && \
      (((up)+(delta)) < (long) image->rows)) \
    { \
      s->y1=(up); \
      s->x1=(left); \
      s->x2=(right); \
      s->y2=(delta); \
      s++; \
    }

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o l o r F l o o d f i l l I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ColorFloodfill() changes the color value of any pixel that matches
%  target and is an immediate neighbor.  If the method FillToBorderMethod is
%  specified, the color value is changed for any neighbor pixel that does not
%  match the bordercolor member of image.
%
%  By default target must match a particular pixel color exactly.
%  However, in many cases two colors may differ by a small amount.  The
%  fuzz member of image defines how much tolerance is acceptable to
%  consider two colors as the same.  For example, set fuzz to 10 and the
%  color red at intensities of 100 and 102 respectively are now
%  interpreted as the same color for the purposes of the floodfill.
%
%  The format of the ColorFloodfillImage method is:
%
%      unsigned int ColorFloodfillImage(Image *image,const DrawInfo *draw_info,
%        const PixelPacket target,const long x_offset,const long y_offset,
%        const PaintMethod method)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o draw_info: The draw info.
%
%    o target: The RGB value of the target color.
%
%    o x,y: The starting location of the operation.
%
%    o method: Choose either FloodfillMethod or FillToBorderMethod.
%
%
*/

MagickExport MagickPassFail ColorFloodfillImage(Image *image,
  const DrawInfo *draw_info,const PixelPacket target,const long x_offset,
  const long y_offset,const PaintMethod method)
{
  Image
    *pattern;

  int
    skip;

  long
    offset,
    start,
    x1,
    x2,
    y;

  PixelPacket
    color;

  register long
    x;

  register PixelPacket
    *q;

  register SegmentInfo
    *s;

  SegmentInfo
    *segment_stack;

  unsigned char
    *floodplane;

  MagickPassFail
    status=MagickPass;

  /*
    Check boundary conditions.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(draw_info != (DrawInfo *) NULL);
  assert(draw_info->signature == MagickSignature);
  if ((x_offset < 0) || (x_offset >= (long) image->columns))
    return(MagickFail);
  if ((y_offset < 0) || (y_offset >= (long) image->rows))
    return(MagickFail);
  /*
    Set floodfill color.
  */
  if (FuzzyColorMatch(&draw_info->fill,&target,image->fuzz))
    return(MagickFail);
  floodplane=MagickAllocateMemory(unsigned char *,image->columns*image->rows);
  segment_stack=MagickAllocateMemory(SegmentInfo *,MaxStacksize*sizeof(SegmentInfo));
  if ((floodplane== (unsigned char *) NULL) ||
      (segment_stack == (SegmentInfo *) NULL))
    ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
      UnableToFloodfillImage);
  (void) memset(floodplane,False,image->columns*image->rows);
  /*
    Push initial segment on stack.
  */
  image->storage_class=DirectClass;
  x=x_offset;
  y=y_offset;
  start=0;
  s=segment_stack;
  Push(y,x,x,1);
  Push(y+1,x,x,-1);
  while (s > segment_stack)
  {
    /*
      Pop segment off stack.
    */
    s--;
    x1=(long) s->x1;
    x2=(long) s->x2;
    offset=(long) s->y2;
    y=(long) s->y1+offset;
    /*
      Recolor neighboring pixels.
    */
    q=GetImagePixels(image,0,y,x1+1,1);
    if (q == (PixelPacket *) NULL)
      {
        status=MagickFail;
        break;
      }
    q+=x1;
    for (x=x1; x >= 0; x--)
    {
      if (method == FloodfillMethod)
        {
          if (!FuzzyColorMatch(q,&target,image->fuzz))
            break;
        }
      else
        if (FuzzyColorMatch(q,&target,image->fuzz) ||
            FuzzyColorMatch(q,&draw_info->fill,image->fuzz))
          break;
      floodplane[y*image->columns+x]=True;
      *q=draw_info->fill;
      q--;
    }
    if (!SyncImagePixels(image))
      {
        status=MagickFail;
        break;
      }
    skip=x >= x1;
    if (!skip)
      {
        start=x+1;
        if (start < x1)
          Push(y,start,x1-1,-offset);
        x=x1+1;
      }
    do
    {
      if (!skip)
        {
          if (x < (long) image->columns)
            {
              q=GetImagePixels(image,x,y,image->columns-x,1);
              if (q == (PixelPacket *) NULL)
                {
                  status=MagickFail;
                  break;
                }
              for ( ; x < (long) image->columns; x++)
              {
                if (method == FloodfillMethod)
                  {
                    if (!FuzzyColorMatch(q,&target,image->fuzz))
                      break;
                  }
                else
                  if (FuzzyColorMatch(q,&target,image->fuzz) ||
                      FuzzyColorMatch(q,&draw_info->fill,image->fuzz))
                    break;
                floodplane[y*image->columns+x]=True;
                *q=draw_info->fill;
                q++;
              }
              if (!SyncImagePixels(image))
                {
                  status=MagickFail;
                  break;
                }
            }
          Push(y,start,x-1,offset);
          if (x > (x2+1))
            Push(y,x2+1,x-1,-offset);
        }
      skip=False;
      x++;
      if (x <= x2)
        {
          q=GetImagePixels(image,x,y,x2-x+1,1);
          if (q == (PixelPacket *) NULL)
            {
              status=MagickFail;
              break;
            }
          for ( ; x <= x2; x++)
          {
            if (method == FloodfillMethod)
              {
                if (FuzzyColorMatch(q,&target,image->fuzz))
                  break;
              }
            else
              if (!FuzzyColorMatch(q,&target,image->fuzz) &&
                  !FuzzyColorMatch(q,&draw_info->fill,image->fuzz))
                break;
            q++;
          }
        }
      start=x;
    } while (x <= x2);
  }
  pattern=draw_info->fill_pattern;
  if (pattern == (Image *) NULL)
    for (y=0; y < (long) image->rows; y++)
    {
      /*
        Tile fill color onto floodplane.
      */
      q=GetImagePixels(image,0,y,image->columns,1);
      if (q == (PixelPacket *) NULL)
        {
          status=MagickFail;
          break;
        }
      for (x=0; x < (long) image->columns; x++)
      {
        if (floodplane[y*image->columns+x])
          *q=draw_info->fill;
        q++;
      }
      if (!SyncImagePixels(image))
        {
          status=MagickFail;
          break;
        }
    }
  else
    {
      /*
        Tile image onto floodplane.
      */
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          {
            status=MagickFail;
            break;
          }
        for (x=0; x < (long) image->columns; x++)
        {
          if (floodplane[y*image->columns+x])
            {
              (void) AcquireOnePixelByReference(pattern,&color,(long) ((unsigned long)
                (x-pattern->tile_info.x) % pattern->columns),(long)
                ((unsigned long) (y-pattern->tile_info.y) % pattern->rows),
                &image->exception);
              if (!pattern->matte)
                color.opacity=OpaqueOpacity;
              if (color.opacity != TransparentOpacity)
                AlphaCompositePixel(q,&color,color.opacity,q,q->opacity);
            }
          q++;
        }
        if (!SyncImagePixels(image))
          {
            status=MagickFail;
            break;
          }
      }
    }
  MagickFreeMemory(segment_stack);
  MagickFreeMemory(floodplane);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a t t e F l o o d f i l l I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MatteFloodfill() changes the transparency value of any pixel that matches
%  target and is an immediate neighbor.  If the method FillToBorderMethod
%  is specified, the transparency value is changed for any neighbor pixel
%  that does not match the bordercolor member of image.
%
%  By default target must match a particular pixel transparency exactly.
%  However, in many cases two transparency values may differ by a
%  small amount.  The fuzz member of image defines how much tolerance is
%  acceptable to consider two transparency values as the same.  For example,
%  set fuzz to 10 and the opacity values of 100 and 102 respectively are
%  now interpreted as the same value for the purposes of the floodfill.
%
%  The format of the MatteFloodfillImage method is:
%
%      unsigned int MatteFloodfillImage(Image *image,const PixelPacket target,
%        const unsigned int opacity,const long x_offset,const long y_offset,
%        const PaintMethod method)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o target: The RGB value of the target color.
%
%    o opacity: The level of transparency: 0 is fully opaque and MaxRGB is
%      fully transparent.
%
%    o x,y: The starting location of the operation.
%
%    o method:  Choose either FloodfillMethod or FillToBorderMethod.
%
%
*/
MagickExport MagickPassFail MatteFloodfillImage(Image *image,
  const PixelPacket target,const unsigned int opacity,const long x_offset,
  const long y_offset,const PaintMethod method)
{
  int
    skip;

  long
    offset,
    start,
    x1,
    x2,
    y;

  register long
    x;

  register PixelPacket
    *q;

  register SegmentInfo
    *s;

  SegmentInfo
    *segment_stack;

  MagickPassFail
    status=MagickPass;

  /*
    Check boundary conditions.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if ((x_offset < 0) || (x_offset >= (long) image->columns))
    return(MagickFail);
  if ((y_offset < 0) || (y_offset >= (long) image->rows))
    return(MagickFail);
  if (target.opacity == opacity)
    return(MagickFail);
  q=GetImagePixels(image,x_offset,y_offset,1,1);
  if (q == (PixelPacket *) NULL)
    return(MagickFail);
  if (q->opacity == opacity)
    return(MagickFail);
  /*
    Allocate segment stack.
  */
  segment_stack=MagickAllocateMemory(SegmentInfo *,MaxStacksize*sizeof(SegmentInfo));
  if (segment_stack == (SegmentInfo *) NULL)
    ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
      UnableToFloodfillImage);
  /*
    Push initial segment on stack.
  */
  (void) SetImageType(image,TrueColorMatteType);
  x=x_offset;
  y=y_offset;
  start=0;
  s=segment_stack;
  Push(y,x,x,1);
  Push(y+1,x,x,-1);
  while (s > segment_stack)
  {
    /*
      Pop segment off stack.
    */
    s--;
    x1=(long) s->x1;
    x2=(long) s->x2;
    offset=(long) s->y2;
    y=(long) s->y1+offset;
    /*
      Recolor neighboring points.
    */
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      {
        status=MagickFail;
        break;
      }
    q+=x1;
    for (x=x1; x >= 0; x--)
    {
      if (method == FloodfillMethod)
        {
          if (!FuzzyOpacityMatch(q,&target,image->fuzz))
            break;
        }
      else
        if (FuzzyOpacityMatch(q,&target,image->fuzz) || (q->opacity == opacity))
          break;
      q->opacity=opacity;
      q--;
    }
    if (!SyncImagePixels(image))
      {
        status=MagickFail;
        break;
      }
    skip=x >= x1;
    if (!skip)
      {
        start=x+1;
        if (start < x1)
          Push(y,start,x1-1,-offset);
        x=x1+1;
      }
    do
    {
      if (!skip)
        {
          q=GetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            {
              status=MagickFail;
              break;
            }
          q+=x;
          for ( ; x < (long) image->columns; x++)
          {
            if (method == FloodfillMethod)
              {
                if (!FuzzyOpacityMatch(q,&target,image->fuzz))
                  break;
              }
            else
              if (FuzzyOpacityMatch(q,&target,image->fuzz) ||
                  (q->opacity == opacity))
                break;
            q->opacity=opacity;
            q++;
          }
          if (!SyncImagePixels(image))
            {
              status=MagickFail;
              break;
            }
          Push(y,start,x-1,offset);
          if (x > (x2+1))
            Push(y,x2+1,x-1,-offset);
        }
      skip=False;
      q=GetImagePixels(image,0,y,image->columns,1);
      if (q == (PixelPacket *) NULL)
        {
          status=MagickFail;
          break;
        }
      q+=x;
      for (x++; x <= x2; x++)
      {
        q++;
        if (method == FloodfillMethod)
          {
            if (FuzzyOpacityMatch(q,&target,image->fuzz))
              break;
          }
        else
          if (!FuzzyOpacityMatch(q,&target,image->fuzz) &&
              (q->opacity != opacity))
            break;
      }
      start=x;
    } while (x <= x2);
  }
  MagickFreeMemory(segment_stack);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     O p a q u e I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  OpaqueImage() changes any pixel that matches color with the color
%  defined by fill.
%
%  By default color must match a particular pixel color exactly.  However,
%  in many cases two colors may differ by a small amount.  Fuzz defines
%  how much tolerance is acceptable to consider two colors as the same.
%  For example, set fuzz to 10 and the color red at intensities of 100 and
%  102 respectively are now interpreted as the same color.
%
%  The format of the OpaqueImage method is:
%
%      unsigned int OpaqueImage(Image *image,const PixelPacket target,
%        const PixelPacket fill)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o target: The RGB value of the target color.
%
%    o fill: The replacement color.
%
%
*/
typedef struct _OpaqueImageOptions_t
{
  double fuzz;
  PixelPacket fill;
  PixelPacket target;
} OpaqueImageOptions_t;
static MagickPassFail
OpaqueImageCallBack(void *mutable_data,         /* User provided mutable data */
                      const void *immutable_data, /* User provided immutable data */
                      Image *image,               /* Modify image */
                      PixelPacket *pixels,        /* Pixel row */
                      IndexPacket *indexes,       /* Pixel row indexes */
                      const long npixels,         /* Number of pixels in row */
                      ExceptionInfo *exception)   /* Exception report */
{
  const OpaqueImageOptions_t
    options = *((const OpaqueImageOptions_t *) immutable_data);

  register long
    i;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  if (options.fuzz == 0.0)
    {
      for (i=0; i < npixels; i++)
        {
          if (ColorMatch(&pixels[i],&options.target))
            pixels[i]=options.fill;
        }
    }
  else
    {
      for (i=0; i < npixels; i++)
        {
          if (FuzzyColorMatch(&pixels[i],&options.target,options.fuzz))
            pixels[i]=options.fill;
        }
    }

  return MagickPass;
}
MagickExport MagickPassFail
OpaqueImage(Image *image,const PixelPacket target,const PixelPacket fill)
{
#define OpaqueImageText "[%s] Setting opaque color..."

  OpaqueImageOptions_t
    options;

  MagickPassFail
    status=MagickPass;

  /*
    Make image color opaque.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  options.fuzz=image->fuzz;
  options.fill=fill;
  options.target=target;
  if (image->storage_class == PseudoClass)
    {
      assert(image->colormap != (PixelPacket *) NULL);
      (void) OpaqueImageCallBack(0,&options,image,image->colormap,
                                 (IndexPacket *) NULL,image->colors,
                                 &image->exception);
      status &= SyncImage(image);
    }
  else
    {
      status=PixelIterateMonoModify(OpaqueImageCallBack,NULL,
                                    OpaqueImageText,NULL,&options,0,0,
                                    image->columns,image->rows,
                                    image,&image->exception);
    }

  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     T r a n s p a r e n t I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  TransparentImage() changes the opacity value associated with any pixel
%  that matches color to the value defined by opacity.
%
%  By default color must match a particular pixel color exactly.  However,
%  in many cases two colors may differ by a small amount.  Fuzz defines
%  how much tolerance is acceptable to consider two colors as the same.
%  For example, set fuzz to 10 and the color red at intensities of 100 and
%  102 respectively are now interpreted as the same color.
%
%  The format of the TransparentImage method is:
%
%      unsigned int TransparentImage(Image *image,const PixelPacket target,
%        const unsigned int opacity)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o target: The RGB value of the target color.
%
%    o opacity: The replacement opacity value.
%
%
*/
typedef struct _TransparentImageOptions_t
{
  double fuzz;
  PixelPacket target;
  unsigned int opacity;
} TransparentImageOptions_t;
static MagickPassFail
TransparentImageCallBack(void *mutable_data,         /* User provided mutable data */
                         const void *immutable_data, /* User provided immutable data */
                         Image *image,               /* Modify image */
                         PixelPacket *pixels,        /* Pixel row */
                         IndexPacket *indexes,       /* Pixel row indexes */
                         const long npixels,         /* Number of pixels in row */
                         ExceptionInfo *exception)   /* Exception report */
{
  const TransparentImageOptions_t
    options = *((const TransparentImageOptions_t *) immutable_data);

  const MagickBool
    clear_matte = (!image->matte);

  register long
    i;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  if (options.fuzz == 0.0)
    {
      for (i=0; i < npixels; i++)
        {
          if (ColorMatch(&pixels[i],&options.target))
            pixels[i].opacity=options.opacity;
          else if (clear_matte)
            pixels[i].opacity=OpaqueOpacity;
        }
    }
  else
    {
      for (i=0; i < npixels; i++)
        {
          if (FuzzyColorMatch(&pixels[i],&options.target,options.fuzz))
            pixels[i].opacity=options.opacity;
          else if (clear_matte)
            pixels[i].opacity=OpaqueOpacity;
        }
    }

  return MagickPass;
}
MagickExport MagickPassFail
TransparentImage(Image *image,const PixelPacket target,
                 const unsigned int opacity)
{
#define TransparentImageText "[%s] Setting transparent color...  "

  TransparentImageOptions_t
    options;

  MagickPassFail
    status=MagickPass;

  /*
    Make image color transparent.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  options.fuzz=image->fuzz;
  options.opacity=opacity;
  options.target=target;
  if (image->storage_class == PseudoClass)
    {
      assert(image->colormap != (PixelPacket *) NULL);
      (void) TransparentImageCallBack(0,&options,image,image->colormap,
                                 (IndexPacket *) NULL,image->colors,
                                 &image->exception);
      status &= SyncImage(image);
    }
  else
    {
      status=PixelIterateMonoModify(TransparentImageCallBack,NULL,
                                    TransparentImageText,NULL,&options,0,0,
                                    image->columns,image->rows,
                                    image,&image->exception);
    }
  image->matte=MagickTrue;

  return(status);
}
