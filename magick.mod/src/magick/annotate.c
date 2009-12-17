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
%           AAA   N   N  N   N   OOO   TTTTT   AAA   TTTTT  EEEEE             %
%          A   A  NN  N  NN  N  O   O    T    A   A    T    E                 %
%          AAAAA  N N N  N N N  O   O    T    AAAAA    T    EEE               %
%          A   A  N  NN  N  NN  O   O    T    A   A    T    E                 %
%          A   A  N   N  N   N   OOO     T    A   A    T    EEEEE             %
%                                                                             %
%                                                                             %
%                  GraphicsMagick Image Annotation Methods                    %
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
% Digital Applications (www.digapp.com) contributed the stroked text algorithm.
% It was written by Leonard Rosenthol.
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/alpha_composite.h"
#include "magick/analyze.h"
#include "magick/color.h"
#include "magick/color_lookup.h"
#include "magick/composite.h"
#include "magick/constitute.h"
#include "magick/gem.h"
#include "magick/log.h"
#include "magick/pixel_cache.h"
#include "magick/render.h"
#include "magick/tempfile.h"
#include "magick/transform.h"
#include "magick/utility.h"
#include "magick/xwindow.h"
#if defined(HasTTF)

#if defined(__MINGW32__)
#  undef interface  /* Remove interface define */
#endif

#if defined(HAVE_FT2BUILD_H)
  /*
    Modern libwmf2 installs may require that <ft2build.h> be included
    before including other libwmf headers.  Including <ft2build.h>
    establishes definitions used by other FreeType headers.
  */
#  include <ft2build.h>
#endif /* defined(HAVE_FT2BUILD_H) */

#if defined(FT_FREETYPE_H)
#  include FT_FREETYPE_H
#else
#  include <freetype/freetype.h>
#endif /* defined(FT_FREETYPE_H) */

#if defined(FT_GLYPH_H)
#  include FT_GLYPH_H
#else
#  include <freetype/ftglyph.h>
#endif /* defined(FT_GLYPH_H) */

#if defined(FT_OUTLINE_H)
#  include FT_OUTLINE_H
#else
#  include <freetype/ftoutln.h>
#endif /* defined(FT_OUTLINE_H) */

#if defined(FT_BBOX_H)
#  include FT_BBOX_H
#else
#  include <freetype/ftbbox.h>
#endif /* defined(FT_BBOX_H) */

#endif /* defined(HasTTF) */

/*
  Forward declarations.
*/
static unsigned int
  RenderType(Image *,const DrawInfo *,const PointInfo *,TypeMetric *),
  RenderPostscript(Image *,const DrawInfo *,const PointInfo *,TypeMetric *),
  RenderFreetype(Image *,const DrawInfo *,const char *,const PointInfo *,
    TypeMetric *),
  RenderX11(Image *,const DrawInfo *,const PointInfo *,TypeMetric *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A n n o t a t e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AnnotateImage() annotates an image with text.  Optionally you can include
%  any of the following bits of information about the image by embedding
%  the appropriate special characters:
%
%    %b   file size in bytes.
%    %c   comment.
%    %d   directory in which the image resides.
%    %e   extension of the image file.
%    %f   original filename of the image.
%    %h   height of image.
%    %i   filename of the image.
%    %k   number of unique colors.
%    %l   image label.
%    %m   image file format.
%    %n   number of images in a image sequence.
%    %o   output image filename.
%    %p   page number of the image.
%    %q   image depth (8 or 16).
%    %p   page number of the image.
%    %q   image depth (8 or 16).
%    %s   image scene number.
%    %t   image filename without any extension.
%    %u   a unique temporary filename.
%    %w   image width.
%    %x   x resolution of the image.
%    %y   y resolution of the image.
%
%  The format of the AnnotateImage method is:
%
%      unsigned int AnnotateImage(Image *image,DrawInfo *draw_info)
%
%  A description of each parameter follows:
%
%    o status: Method AnnotateImage returns True if the image is annotated
%      otherwise False.
%
%    o image: The image.
%
%    o draw_info: The draw info.
%
%
*/
MagickExport MagickPassFail AnnotateImage(Image *image,const DrawInfo *draw_info)
{
  char
    primitive[MaxTextExtent],
    *text,
    **textlist;

  DrawInfo
    *annotate,
    *clone_info;

  PointInfo
    offset;

  RectangleInfo
    geometry;

  register long
    i;

  size_t
    length;

  TypeMetric
    metrics;

  unsigned int
    matte;

  MagickPassFail
    status=MagickPass;

  unsigned long
    height,
    number_lines;

  /*
    Translate any embedded format characters (e.g. %f).
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(draw_info != (DrawInfo *) NULL);
  assert(draw_info->signature == MagickSignature);
  if (draw_info->text == (char *) NULL)
    return(MagickFail);
  if (*draw_info->text == '\0')
    return(MagickFail);
  text=TranslateText((ImageInfo *) NULL,image,draw_info->text);
  if (text == (char *) NULL)
    ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
      UnableToAnnotateImage);
  textlist=StringToList(text);
  MagickFreeMemory(text);
  if (textlist == (char **) NULL)
    return(MagickFail);
  length=strlen(textlist[0]);
  for (i=1; textlist[i] != (char *) NULL; i++)
    if (strlen(textlist[i]) > length)
      length=strlen(textlist[i]);
  number_lines=i;
  text=MagickAllocateMemory(char *,length+MaxTextExtent);
  if (text == (char *) NULL)
    ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
      UnableToAnnotateImage);
  SetGeometry(image,&geometry);
  if (draw_info->geometry != (char *) NULL)
    (void) GetGeometry(draw_info->geometry,&geometry.x,&geometry.y,
      &geometry.width,&geometry.height);
  annotate=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  matte=image->matte;
  status=True;
  for (i=0; textlist[i] != (char *) NULL; i++)
  {
    if (*textlist[i] == '\0')
      continue;
    /*
      Position text relative to image.
    */
    (void) CloneString(&annotate->text,textlist[i]);
    if ((i == 0) || (annotate->gravity != NorthWestGravity))
      (void) GetTypeMetrics(image,annotate,&metrics);
    height=(unsigned long) (metrics.ascent-metrics.descent);
    switch (annotate->gravity)
    {
      case ForgetGravity:
      case NorthWestGravity:
      default:
      {
        offset.x=geometry.x+i*draw_info->affine.ry*height;
        offset.y=geometry.y+i*draw_info->affine.sy*height;
        break;
      }
      case NorthGravity:
      {
        offset.x=geometry.x+geometry.width/2+i*draw_info->affine.ry*height-
          draw_info->affine.sx*metrics.width/2;
        offset.y=geometry.y+i*draw_info->affine.sy*height-draw_info->affine.rx*
          metrics.width/2;
        break;
      }
      case NorthEastGravity:
      {
        offset.x=(geometry.width == 0 ? 1 : -1)*geometry.x+geometry.width+i*
          draw_info->affine.ry*height-draw_info->affine.sx*metrics.width;
        offset.y=geometry.y+i*draw_info->affine.sy*height-draw_info->affine.rx*
          metrics.width;
        break;
      }
      case WestGravity:
      {
        offset.x=geometry.x+i*draw_info->affine.ry*height+draw_info->affine.ry*
          (metrics.ascent+metrics.descent-(number_lines-1)*height)/2;
        offset.y=geometry.y+geometry.height/2+i*draw_info->affine.sy*height+
          draw_info->affine.sy*(metrics.ascent+metrics.descent-(number_lines-1)*
          height)/2;
        break;
      }
      case StaticGravity:
      case CenterGravity:
      {
        offset.x=geometry.x+geometry.width/2+i*draw_info->affine.ry*height-
          draw_info->affine.sx*metrics.width/2+draw_info->affine.ry*
          (metrics.ascent+metrics.descent-(number_lines-1)*height)/2;
        offset.y=geometry.y+geometry.height/2+i*draw_info->affine.sy*height-
          draw_info->affine.rx*metrics.width/2+draw_info->affine.sy*
          (metrics.ascent+metrics.descent-(number_lines-1)*height)/2;
        break;
      }
      case EastGravity:
      {
        offset.x=(geometry.width == 0 ? 1 : -1)*geometry.x+geometry.width+i*
          draw_info->affine.ry*height-draw_info->affine.sx*metrics.width+
          draw_info->affine.ry*(metrics.ascent+metrics.descent-(number_lines-1)*
          height)/2;
        offset.y=geometry.y+geometry.height/2+i*draw_info->affine.sy*height-
          draw_info->affine.rx*metrics.width+draw_info->affine.sy*
          (metrics.ascent+metrics.descent-(number_lines-1)*height)/2;
        break;
      }
      case SouthWestGravity:
      {
        offset.x=geometry.x+i*draw_info->affine.ry*height-draw_info->affine.ry*
          (number_lines-1)*height;
        offset.y=(geometry.height == 0 ? 1 : -1)*geometry.y+geometry.height+i*
          draw_info->affine.sy*height-draw_info->affine.sy*(number_lines-1)*
          height;
        break;
      }
      case SouthGravity:
      {
        offset.x=geometry.x+geometry.width/2+i*draw_info->affine.ry*
          height-draw_info->affine.sx*metrics.width/2-
          draw_info->affine.ry*(number_lines-1)*height;
        offset.y=(geometry.height == 0 ? 1 : -1)*geometry.y+geometry.height+i*
          draw_info->affine.sy*height-draw_info->affine.rx*
          metrics.width/2-draw_info->affine.sy*(number_lines-1)*height;
        break;
      }
      case SouthEastGravity:
      {
        offset.x=(geometry.width == 0 ? 1 : -1)*geometry.x+geometry.width+i*
          draw_info->affine.ry*height-draw_info->affine.sx*metrics.width-
          draw_info->affine.ry*(number_lines-1)*height;
        offset.y=(geometry.height == 0 ? 1 : -1)*geometry.y+geometry.height+i*
          draw_info->affine.sy*height-draw_info->affine.rx*metrics.width-
          draw_info->affine.sy*(number_lines-1)*height;
        break;
      }
    }
    switch (annotate->align)
    {
      case LeftAlign:
      {
        offset.x=geometry.x+i*draw_info->affine.ry*height;
        offset.y=geometry.y+i*draw_info->affine.sy*height;
        break;
      }
      case CenterAlign:
      {
        offset.x=geometry.x+i*draw_info->affine.ry*height-draw_info->affine.sx*
          metrics.width/2;
        offset.y=geometry.y+i*draw_info->affine.sy*height-draw_info->affine.rx*
          metrics.width/2;
        break;
      }
      case RightAlign:
      {
        offset.x=geometry.x+i*draw_info->affine.ry*height-draw_info->affine.sx*
          metrics.width;
        offset.y=geometry.y+i*draw_info->affine.sy*height-draw_info->affine.rx*
          metrics.width;
        break;
      }
      default:
        break;
    }
    if (draw_info->undercolor.opacity != TransparentOpacity)
      {
        /*
          Text box.
        */
        clone_info->fill=draw_info->undercolor;
        clone_info->affine.tx=offset.x-draw_info->affine.ry*(metrics.ascent-
          metrics.max_advance/4);
        clone_info->affine.ty=offset.y-draw_info->affine.sy*metrics.ascent;
        FormatString(primitive,"rectangle 0,0 %g,%ld",metrics.width+
          metrics.max_advance/2.0,height);
        (void) CloneString(&clone_info->primitive,primitive);
        (void) DrawImage(image,clone_info);
      }
    clone_info->affine.tx=offset.x;
    clone_info->affine.ty=offset.y;
    FormatString(primitive,"stroke-width %g line 0,0 %g,0",
      metrics.underline_thickness,metrics.width);
    if (annotate->decorate == OverlineDecoration)
      {
        clone_info->affine.ty-=(draw_info->affine.sy*
          (metrics.ascent+metrics.descent)-metrics.underline_position);
        (void) CloneString(&clone_info->primitive,primitive);
        (void) DrawImage(image,clone_info);
      }
    else
      if (annotate->decorate == UnderlineDecoration)
        {
          clone_info->affine.ty-=metrics.underline_position;
          (void) CloneString(&clone_info->primitive,primitive);
          (void) DrawImage(image,clone_info);
        }
    /*
      Annotate image with text.
    */
    status=RenderType(image,annotate,&offset,&metrics);
    if (status == MagickFail)
      break;
    if (annotate->decorate == LineThroughDecoration)
      {
        clone_info->affine.ty-=(draw_info->affine.sy*height+
          metrics.underline_position)/2.0;
        (void) CloneString(&clone_info->primitive,primitive);
        (void) DrawImage(image,clone_info);
      }
  }
  image->matte=matte;
  /*
    Free resources.
  */
  DestroyDrawInfo(clone_info);
  DestroyDrawInfo(annotate);
  MagickFreeMemory(text);
  for (i=0; textlist[i] != (char *) NULL; i++)
    MagickFreeMemory(textlist[i]);
  MagickFreeMemory(textlist);
  return(status);
}

#if defined(HasTTF)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   E n c o d e S J I S                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  EncodeSJIS() converts an ASCII text string to 2-bytes per character code
%  (like UCS-2).  Returns the translated codes and the character count.
%  Characters under 0x7f are just copied, characters over 0x80 are tied with
%  the next character.
%
%  Katsutoshi Shibuya contributed this method.
%
%  The format of the EncodeSJIS function is:
%
%      encoding=EncodeSJIS(const char *text,size_t count)
%
%  A description of each parameter follows:
%
%    o encoding:  EncodeSJIS() returns a pointer to an unsigned short
%      array representing the encoded version of the ASCII string.
%
%    o text: The text.
%
%    o count: return the number of characters generated by the encoding.
%
%
*/

static int GetOneCharacter(const unsigned char *text,size_t *length)
{
  unsigned int
    c;

  if (*length < 1)
    return(-1);
  c=text[0];
  if (!(c & 0x80))
    {
      *length=1;
      return((int) c);
    }
  if (*length < 2)
    {
      *length=0;
      return(-1);
    }
  *length=2;
  c=((int) (text[0]) << 8);
  c|=text[1];
  return((int) c);
}

static unsigned short *EncodeSJIS(const char *text,size_t *count)
{
  int
    c;

  register const char
    *p;

  register unsigned short
    *q;

  size_t
    length;

  unsigned short
    *encoding;

  *count=0;
  if ((text == (char *) NULL) || (*text == '\0'))
    return((unsigned short *) NULL);
  encoding=MagickAllocateMemory(unsigned short *,
    (strlen(text)+MaxTextExtent)*sizeof(unsigned short));
  if (encoding == (unsigned short *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToConvertText);
  q=encoding;
  for (p=text; *p != '\0'; p+=length)
  {
    length=strlen(p);
    c=GetOneCharacter((const unsigned char *) p,&length);
    if (c < 0)
      {
        q=encoding;
        for (p=text; *p != '\0'; p++)
          *q++=(unsigned char) *p;
        break;
      }
    *q=(unsigned short) c;
    q++;
  }
  *count=q-encoding;
  return(encoding);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   E n c o d e T e x t                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  EncodeText() converts an ASCII text string to wide text and returns the
%  translation and the character count.
%
%  The format of the EncodeText function is:
%
%      encoding=EncodeText(const char *text,size_t count)
%
%  A description of each parameter follows:
%
%    o encoding:  EncodeText() returns a pointer to an unsigned short array
%      array representing the encoded version of the ASCII string.
%
%    o text: The text.
%
%    o count: return the number of characters generated by the encoding.
%
%
*/
static unsigned short *EncodeText(const char *text,size_t *count)
{
  register const char
    *p;

  register unsigned short
    *q;

  unsigned short
    *encoding;

  *count=0;
  if ((text == (char *) NULL) || (*text == '\0'))
    return((unsigned short *) NULL);
  encoding=MagickAllocateMemory(unsigned short *,
    (strlen(text)+MaxTextExtent)*sizeof(unsigned short));
  if (encoding == (unsigned short *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToConvertText);
  q=encoding;
  for (p=text; *p != '\0'; p++)
    *q++=(unsigned char) *p;
  *count=q-encoding;
  return(encoding);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   E n c o d e U n i c o d e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  EncodeUnicode() converts an ASCII text string to Unicode and returns the
%  Unicode translation and the character count.  Characters under 0x7f are
%  just copied, characters over 0x80 are tied with the next character.
%
%  The format of the EncodeUnicode function is:
%
%      unicode=EncodeUnicode(const unsigned char *text,size_t count)
%
%  A description of each parameter follows:
%
%    o unicode:  EncodeUnicode() returns a pointer to an unsigned short array
%      array representing the encoded version of the ASCII string.
%
%    o text: The text.
%
%    o count: return the number of characters generated by the encoding.
%
%
*/

static long GetUnicodeCharacter(const unsigned char *text,size_t *length)
{
  unsigned long
    c;

  if (*length < 1)
    return(-1);
  c=text[0];
  if (!(c & 0x80))
    {
      *length=1;
      return((long) c);
    }
  if ((*length < 2) || ((text[1] & 0xc0) != 0x80))
    {
      *length=0;
      return(-1);
    }
  if ((c & 0xe0) != 0xe0)
    {
      *length=2;
      c=(text[0] & 0x1f) << 6;
      c|=text[1] & 0x3f;
      return((long) c);
    }
  if ((*length < 3) || ((text[2] & 0xc0) != 0x80))
    {
      *length=0;
      return(-1);
    }
  if ((c & 0xf0) != 0xf0)
    {
      *length=3;
      c=(text[0] & 0xf) << 12;
      c|=(text[1] & 0x3f) << 6;
      c|=text[2] & 0x3f;
      return((long) c);
    }
  if ((*length < 4) || ((c & 0xf8) != 0xf0) || ((text[3] & 0xc0) != 0x80))
    {
      *length=0;
      return(-1);
    }
  *length=4;
  c=(text[0] & 0x7) << 18;
  c|=(text[1] & 0x3f) << 12;
  c|=(text[2] & 0x3f) << 6;
  c|=text[3] & 0x3f;
  return((long) c);
}

static unsigned short *EncodeUnicode(const char *text,size_t *count)
{
  int
    c;

  register const char
    *p;

  register unsigned short
    *q;

  size_t
    length;

  unsigned short
    *unicode;

  *count=0;
  if ((text == (char *) NULL) || (*text == '\0'))
    return((unsigned short *) NULL);
  unicode=MagickAllocateMemory(unsigned short *,
    (strlen(text)+MaxTextExtent)*sizeof(unsigned short));
  if (unicode == (unsigned short *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToConvertText);
  q=unicode;
  for (p=text; *p != '\0'; p+=length)
  {
    length=strlen(p);
    c=GetUnicodeCharacter((const unsigned char *) p,&length);
    if (c < 0)
      {
        q=unicode;
        for (p=text; *p != '\0'; p++)
          *q++=(unsigned char) *p;
        break;
      }
    *q=(unsigned short) c;
    q++;
  }
  *count=q-unicode;
  return(unicode);
}

#endif /* HasTTF */
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t T y p e M e t r i c s                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetTypeMetrics() returns the following information for the specified font
%  and text:
%
%    o character width
%    o character height
%    o ascent
%    o descent
%    o text width
%    o text height
%    o maximum horizontal advance
%    o underline position
%    o underline thickness
%
%  The format of the GetTypeMetrics method is:
%
%      unsigned int GetTypeMetrics(Image *image,const DrawInfo *draw_info,
%        TypeMetric *metrics)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o draw_info: The draw info.
%
%    o metrics: Return the font metrics in this structure.
%
%
*/
MagickExport MagickPassFail GetTypeMetrics(Image *image,const DrawInfo *draw_info,
  TypeMetric *metrics)
{
  DrawInfo
    *clone_info;

  PointInfo
    offset;

  MagickPassFail
    status;

  assert(draw_info != (DrawInfo *) NULL);
  assert(draw_info->text != (char *) NULL);
  assert(draw_info->signature == MagickSignature);
  clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  clone_info->render=False;
  (void) memset(metrics,0,sizeof(TypeMetric));
  offset.x=0.0;
  offset.y=0.0;
  status=RenderType(image,clone_info,&offset,metrics);
  DestroyDrawInfo(clone_info);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   R e n d e r T y p e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RenderType renders text on the image.  It also returns the bounding
%  box of the text relative to the image.
%
%  The format of the RenderType method is:
%
%      unsigned int RenderType(Image *image,DrawInfo *draw_info,
%        const PointInfo *offset,TypeMetric *metrics)
%
%  A description of each parameter follows:
%
%    o status: Method RenderType returns True if the text is rendered on the
%      image, otherwise False.
%
%    o image: The image.
%
%    o draw_info: The draw info.
%
%    o offset: (x,y) location of text relative to image.
%
%    o metrics: bounding box of text.
%
%
*/
static MagickPassFail RenderType(Image *image,const DrawInfo *draw_info,
  const PointInfo *offset,TypeMetric *metrics)
{
  const TypeInfo
    *type_info;

  DrawInfo
   *clone_info;

  MagickPassFail
    status;

  type_info=(const TypeInfo *) NULL;
  if (draw_info->font != (char *) NULL)
    {
      if (*draw_info->font == '@')
        return(RenderFreetype(image,draw_info,(char *) NULL,offset,metrics));
      if (*draw_info->font == '-')
        return(RenderX11(image,draw_info,offset,metrics));
      type_info=GetTypeInfo(draw_info->font,&image->exception);
      if (type_info == (const TypeInfo *) NULL)
        if (IsAccessible(draw_info->font))
          return(RenderFreetype(image,draw_info,(char *) NULL,offset,metrics));
    }
  if (type_info == (const TypeInfo *) NULL)
    type_info=GetTypeInfoByFamily(draw_info->family,draw_info->style,
      draw_info->stretch,draw_info->weight,&image->exception);
  if (type_info == (const TypeInfo *) NULL)
    return(RenderPostscript(image,draw_info,offset,metrics));
  clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  if (type_info->glyphs != (char *) NULL)
    (void) CloneString(&clone_info->font,type_info->glyphs);
  status=RenderFreetype(image,clone_info,type_info->encoding,offset,metrics);
  DestroyDrawInfo(clone_info);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   R e n d e r F r e e t y p e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RenderFreetype renders text on the image with a Truetype font.  It
%  also returns the bounding box of the text relative to the image.
%
%  The format of the RenderFreetype method is:
%
%      unsigned int RenderFreetype(Image *image,DrawInfo *draw_info,
%        const char *encoding,const PointInfo *offset,TypeMetric *metrics)
%
%  A description of each parameter follows:
%
%    o status: Method RenderFreetype returns True if the text is rendered on the
%      image, otherwise False.
%
%    o image: The image.
%
%    o draw_info: The draw info.
%
%    o encoding: The font encoding.
%
%    o offset: (x,y) location of text relative to image.
%
%    o metrics: bounding box of text.
%
%
*/

#if defined(HasTTF)
static int TraceCubicBezier(FT_Vector *p,FT_Vector *q,FT_Vector *to,
  DrawInfo *draw_info)
{
  AffineMatrix
    affine;

  char
    path[MaxTextExtent];

  affine=draw_info->affine;
  FormatString(path,"C%g,%g %g,%g %g,%g",affine.tx+p->x/64.0,
    affine.ty-p->y/64.0,affine.tx+q->x/64.0,affine.ty-q->y/64.0,
    affine.tx+to->x/64.0,affine.ty-to->y/64.0);
  (void) ConcatenateString(&draw_info->primitive,path);
  return(0);
}

static int TraceLineTo(FT_Vector *to,DrawInfo *draw_info)
{
  AffineMatrix
    affine;

  char
    path[MaxTextExtent];

  affine=draw_info->affine;
  FormatString(path,"L%g,%g",affine.tx+to->x/64.0,affine.ty-to->y/64.0);
  (void) ConcatenateString(&draw_info->primitive,path);
  return(0);
}

static int TraceMoveTo(FT_Vector *to,DrawInfo *draw_info)
{
  AffineMatrix
    affine;

  char
    path[MaxTextExtent];

  affine=draw_info->affine;
  FormatString(path,"M%g,%g",affine.tx+to->x/64.0,affine.ty-to->y/64.0);
  (void) ConcatenateString(&draw_info->primitive,path);
  return(0);
}

static int TraceQuadraticBezier(FT_Vector *control,FT_Vector *to,
  DrawInfo *draw_info)
{
  AffineMatrix
    affine;

  char
    path[MaxTextExtent];

  affine=draw_info->affine;
  FormatString(path,"Q%g,%g %g,%g",affine.tx+control->x/64.0,
    affine.ty-control->y/64.0,affine.tx+to->x/64.0,affine.ty-to->y/64.0);
  (void) ConcatenateString(&draw_info->primitive,path);
  return(0);
}

static MagickPassFail RenderFreetype(Image *image,const DrawInfo *draw_info,
  const char *encoding,const PointInfo *offset,TypeMetric *metrics)
{
  typedef struct _GlyphInfo
  {
    FT_UInt
      id;

    FT_Vector
      origin;

    FT_Glyph
      image;
  } GlyphInfo;

  double
    opacity;

  DrawInfo
    *clone_info;

  FT_BBox
    bounds;

  FT_BitmapGlyph
    bitmap;

  FT_Encoding
    encoding_type;

  FT_Error
    status;

  FT_Face
    face;

  FT_Library
    library;

  FT_Matrix
    affine;

  FT_Vector
    origin;

  GlyphInfo
    glyph,
    last_glyph;

  Image
    *pattern;

  long
    y;

  PixelPacket
    fill_color;

  PointInfo
    point,
    resolution;

  register long
    i,
    x;

  register PixelPacket
    *q;

  register unsigned char
    *p;

  size_t
    length;

  static FT_Outline_Funcs
    OutlineMethods =
    {
      (FT_Outline_MoveTo_Func) TraceMoveTo,
      (FT_Outline_LineTo_Func) TraceLineTo,
      (FT_Outline_ConicTo_Func) TraceQuadraticBezier,
      (FT_Outline_CubicTo_Func) TraceCubicBezier,
      0, 0
    };

  unsigned int
    active;

  unsigned short
    *text;

  glyph.image=(FT_Glyph) 0;
  last_glyph.image=(FT_Glyph) 0;

  /*
    Initialize Truetype library.
  */
  status=FT_Init_FreeType(&library);
  if (status)
    ThrowBinaryException(TypeError,UnableToInitializeFreetypeLibrary,
      draw_info->font);
  if (*draw_info->font != '@')
    status=FT_New_Face(library,draw_info->font,0,&face);
  else
    status=FT_New_Face(library,draw_info->font+1,0,&face);
  if (status != 0)
    {
      (void) FT_Done_FreeType(library);
      ThrowBinaryException(TypeError,UnableToReadFont,draw_info->font)
    }
  if (face->num_charmaps != 0)
    status=FT_Set_Charmap(face,face->charmaps[0]);
  encoding_type=ft_encoding_unicode;
  status=FT_Select_Charmap(face,encoding_type);
  if (status != 0)
    {
      encoding_type=ft_encoding_none;
      status=FT_Select_Charmap(face,encoding_type);
    }
  if (encoding != (char *) NULL)
    {
      if (LocaleCompare(encoding,"AdobeCustom") == 0)
        encoding_type=ft_encoding_adobe_custom;
      if (LocaleCompare(encoding,"AdobeExpert") == 0)
        encoding_type=ft_encoding_adobe_expert;
      if (LocaleCompare(encoding,"AdobeStandard") == 0)
        encoding_type=ft_encoding_adobe_standard;
      if (LocaleCompare(encoding,"AppleRoman") == 0)
        encoding_type=ft_encoding_apple_roman;
      if (LocaleCompare(encoding,"BIG5") == 0)
        encoding_type=ft_encoding_big5;
      if (LocaleCompare(encoding,"GB2312") == 0)
        encoding_type=ft_encoding_gb2312;
      if (LocaleCompare(encoding,"None") == 0)
        encoding_type=ft_encoding_none;
      if (LocaleCompare(encoding,"SJIScode") == 0)
        encoding_type=ft_encoding_sjis;
      if (LocaleCompare(encoding,"Symbol") == 0)
        encoding_type=ft_encoding_symbol;
      if (LocaleCompare(encoding,"Unicode") == 0)
        encoding_type=ft_encoding_unicode;
      if (LocaleCompare(encoding,"Wansung") == 0)
        encoding_type=ft_encoding_wansung;
      status=FT_Select_Charmap(face,encoding_type);
      if (status != 0)
        ThrowBinaryException(TypeError,UnrecognizedFontEncoding,encoding);
    }
  /*
    Set text size.
  */
  resolution.x=72.0;
  resolution.y=72.0;
  if (draw_info->density != (char *) NULL)
    {
      i=GetMagickDimension(draw_info->density,&resolution.x,&resolution.y,NULL,NULL);
      if (i != 2)
        resolution.y=resolution.x;
    }
  (void) FT_Set_Char_Size(face,(FT_F26Dot6) (64.0*draw_info->pointsize),
    (FT_F26Dot6) (64.0*draw_info->pointsize),(FT_UInt) resolution.x,
    (FT_UInt) resolution.y);
  metrics->pixels_per_em.x=face->size->metrics.x_ppem;
  metrics->pixels_per_em.y=face->size->metrics.y_ppem;
  metrics->ascent=(double) face->size->metrics.ascender/64.0;
  metrics->descent=(double) face->size->metrics.descender/64.0;
  metrics->width=0;
  metrics->height=(double) face->size->metrics.height/64.0;
  metrics->max_advance=(double) face->size->metrics.max_advance/64.0;
  metrics->bounds.x1=0.0;
  metrics->bounds.y1=metrics->descent;
  metrics->bounds.x2=metrics->ascent+metrics->descent;
  metrics->bounds.y2=metrics->ascent+metrics->descent;
  metrics->underline_position=face->underline_position/64.0;
  metrics->underline_thickness=face->underline_thickness/64.0;
  /*
    Convert text to 2-byte format as prescribed by the encoding.
  */
  switch (encoding_type)
  {
    case ft_encoding_sjis:
    {
      text=EncodeSJIS(draw_info->text,&length);
      break;
    }
    case ft_encoding_unicode:
    {
      text=EncodeUnicode(draw_info->text,&length);
      break;
    }
    default:
    {
      if (draw_info->encoding != (char *) NULL)
        {
          if (LocaleCompare(draw_info->encoding,"SJIS") == 0)
            {
              text=EncodeSJIS(draw_info->text,&length);
              break;
            }
          if ((LocaleCompare(draw_info->encoding,"UTF-8") == 0) ||
              (encoding_type != ft_encoding_none))
            {
              text=EncodeUnicode(draw_info->text,&length);
              break;
            }
        }
      text=EncodeText(draw_info->text,&length);
      break;
    }
  }
  if (text == (unsigned short *) NULL)
    {
      (void) FT_Done_Face(face);
      (void) FT_Done_FreeType(library);
      ThrowBinaryException(ResourceLimitError,MemoryAllocationFailed,
        draw_info->font)
    }
  /*
    Compute bounding box.
  */
  (void) LogMagickEvent(AnnotateEvent,GetMagickModule(),
    "Font %.1024s; font-encoding %.1024s; text-encoding %.1024s; pointsize %g",
    draw_info->font != (char *) NULL ? draw_info->font : "none",
    encoding != (char *) NULL ? encoding : "none",
    draw_info->encoding != (char *) NULL ? draw_info->encoding : "none",
    draw_info->pointsize);
  glyph.id=0;
  last_glyph.id=0;
  origin.x=0;
  origin.y=0;
  affine.xx=(FT_Fixed) (65536L*draw_info->affine.sx+0.5);
  affine.yx=(FT_Fixed) (-65536L*draw_info->affine.rx+0.5);
  affine.xy=(FT_Fixed) (-65536L*draw_info->affine.ry+0.5);
  affine.yy=(FT_Fixed) (65536L*draw_info->affine.sy+0.5);
  clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  (void) QueryColorDatabase("#000000ff",&clone_info->fill,&image->exception);
  (void) CloneString(&clone_info->primitive,"path '");
  pattern=draw_info->fill_pattern;
  for (i=0; i < (long) length; i++)
  {
    glyph.id=FT_Get_Char_Index(face,text[i]);
    if ((glyph.id != 0) && (last_glyph.id != 0) && FT_HAS_KERNING(face))
      {
        FT_Vector
          kerning;

        (void) FT_Get_Kerning(face,last_glyph.id,glyph.id,ft_kerning_default,
          &kerning);
        origin.x+=kerning.x;
      }
    glyph.origin=origin;
    glyph.image=0;
    status=FT_Load_Glyph(face,glyph.id,FT_LOAD_DEFAULT);
    if (status != False)
      continue;
    status=FT_Get_Glyph(face->glyph,&glyph.image);
    if (status != False)
      continue;
#if 0
    /*
      Obtain glyph's control box. Usually faster than computing the
      exact bounding box but may be slightly larger in some
      situations.
    */
    (void) FT_Glyph_Get_CBox(glyph.image,FT_GLYPH_BBOX_SUBPIXELS,&bounds);
#else
    /*
      Compute exact bounding box for scaled outline. If necessary, the
      outline Bezier arcs are walked over to extract their extrema.
    */
    (void) FT_Outline_Get_BBox(&((FT_OutlineGlyph) glyph.image)->outline,&bounds);
#endif
    if ((i == 0) || (bounds.xMin < metrics->bounds.x1))
      metrics->bounds.x1=bounds.xMin;
    if ((i == 0) || (bounds.yMin < metrics->bounds.y1))
      metrics->bounds.y1=bounds.yMin;
    if ((i == 0) || (bounds.xMax > metrics->bounds.x2))
      metrics->bounds.x2=bounds.xMax;
    if ((i == 0) || (bounds.yMax > metrics->bounds.y2))
      metrics->bounds.y2=bounds.yMax;
    if (draw_info->render)
      if ((draw_info->stroke.opacity != TransparentOpacity) ||
          (draw_info->stroke_pattern != (Image *) NULL))
        {
          /*
            Trace the glyph.
          */
          clone_info->affine.tx=glyph.origin.x/64.0;
          clone_info->affine.ty=glyph.origin.y/64.0;
          (void) FT_Outline_Decompose(&((FT_OutlineGlyph) glyph.image)->outline,
            &OutlineMethods,clone_info);
        }
    FT_Vector_Transform(&glyph.origin,&affine);
    (void) FT_Glyph_Transform(glyph.image,&affine,&glyph.origin);
    if (draw_info->render)
      {
        if ((draw_info->fill.opacity != TransparentOpacity) ||
            (pattern != (Image *) NULL))
          {
            /*
              Rasterize the glyph.
            */
            status=FT_Glyph_To_Bitmap(&glyph.image,ft_render_mode_normal,
              (FT_Vector *) NULL,True);
            if (status != False)
              continue;
            bitmap=(FT_BitmapGlyph) glyph.image;
            image->storage_class=DirectClass;
            point.x=offset->x+bitmap->left;
            point.y=offset->y-bitmap->top;
            p=bitmap->bitmap.buffer;
            /* FIXME: OpenMP */
            for (y=0; y < (long) bitmap->bitmap.rows; y++)
            {
              if ((ceil(point.y+y-0.5) < 0) ||
                  (ceil(point.y+y-0.5) >= image->rows))
                {
                  p+=bitmap->bitmap.width;
                  continue;
                }
              q=GetImagePixels(image,(long) ceil(point.x-0.5),
                (long) ceil(point.y+y-0.5),bitmap->bitmap.width,1);
              active=q != (PixelPacket *) NULL;
              for (x=0; x < (long) bitmap->bitmap.width; x++)
              {
                if (((long) ceil(point.x+x-0.5) < 0) || (*p == 0) ||
                    ((unsigned long) ceil(point.x+x-0.5) >= image->columns))
                  {
                    p++;
                    q++;
                    continue;
                  }
                if (draw_info->text_antialias)
                  opacity=ScaleCharToQuantum(*p);
                else
                  opacity=((*p) < 127 ? OpaqueOpacity : TransparentOpacity);
                fill_color=draw_info->fill;
                if (pattern != (Image *) NULL)
                  (void) AcquireOnePixelByReference(pattern,&fill_color,
                    (long) (point.x+x-pattern->tile_info.x) % pattern->columns,
                    (long) (point.y+y-pattern->tile_info.y) % pattern->rows,
                            &image->exception);
                opacity=((MaxRGB-opacity)*(MaxRGB-fill_color.opacity))/MaxRGB;
                if (!active)
                  q=GetImagePixels(image,(long) ceil(point.x+x-0.5),
                    (long) ceil(point.y+y-0.5),1,1);
                if (q == (PixelPacket *) NULL)
                  {
                    p++;
                    q++;
                    continue;
                  }
                AlphaCompositePixel(q,&fill_color,opacity,q,
                                    image->matte ? q->opacity : OpaqueOpacity);
                if (!active)
                  (void) SyncImagePixels(image);
                p++;
                q++;
              }
              (void) SyncImagePixels(image);
            }
          }
      }
    origin.x+=face->glyph->advance.x;
    if (origin.x > metrics->width)
      metrics->width=origin.x;
    if (last_glyph.id != 0)
      FT_Done_Glyph(last_glyph.image);
    last_glyph=glyph;
  }
  metrics->width/=64.0;
  metrics->bounds.x1/=64.0;
  metrics->bounds.y1/=64.0;
  metrics->bounds.x2/=64.0;
  metrics->bounds.y2/=64.0;
  if (draw_info->render)
    if ((draw_info->stroke.opacity != TransparentOpacity) ||
        (draw_info->stroke_pattern != (Image *) NULL))
      {
        /*
          Draw text stroke.
        */
        clone_info->affine.tx=offset->x;
        clone_info->affine.ty=offset->y;
        (void) ConcatenateString(&clone_info->primitive,"'");
        (void) DrawImage(image,clone_info);
      }
  if (glyph.id != 0)
    FT_Done_Glyph(glyph.image);
  /*
    Free resources.
  */
  MagickFreeMemory(text);
  DestroyDrawInfo(clone_info);
  (void) FT_Done_Face(face);
  (void) FT_Done_FreeType(library);
  return(MagickPass);
}
#else
static unsigned int RenderFreetype(Image *image,const DrawInfo *draw_info,
  const char * ARGUNUSED(encoding),const PointInfo * ARGUNUSED(offset),
  TypeMetric * ARGUNUSED(metrics))
{
  ThrowBinaryException(MissingDelegateError,FreeTypeLibraryIsNotAvailable,
    draw_info->font)
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   R e n d e r P o s t s c r i p t                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RenderPostscript renders text on the image with a Postscript font.
%  It also returns the bounding box of the text relative to the image.
%
%  The format of the RenderPostscript method is:
%
%      unsigned int RenderPostscript(Image *image,DrawInfo *draw_info,
%        const PointInfo *offset,TypeMetric *metrics)
%
%  A description of each parameter follows:
%
%    o status: Method RenderPostscript returns True if the text is rendered on
%      the image, otherwise False.
%
%    o image: The image.
%
%    o draw_info: The draw info.
%
%    o offset: (x,y) location of text relative to image.
%
%    o metrics: bounding box of text.
%
%
*/

static char *EscapeParenthesis(const char *text)
{
  char
    *buffer;

  register char
    *p;

  register long
    i;

  unsigned long
    escapes;

  escapes=0;
  buffer=AllocateString(text);
  p=buffer;
  for (i=0; i < (long) Min(strlen(text),(MaxTextExtent-escapes-1)); i++)
  {
    if ((text[i] == '(') || (text[i] == ')'))
      {
        *p++='\\';
        escapes++;
      }
    *p++=text[i];
  }
  *p='\0';
  return(buffer);
}

static MagickPassFail RenderPostscript(Image *image,const DrawInfo *draw_info,
  const PointInfo *offset,TypeMetric *metrics)
{
  char
    filename[MaxTextExtent],
    geometry[MaxTextExtent],
    *text;

  FILE
    *file;

  Image
    *annotate_image,
    *pattern;

  ImageInfo
    *clone_info;

  long
    y;

  PointInfo
    extent,
    point,
    resolution;

  register long
    i,
    x;

  register PixelPacket
    *q;

  unsigned int
    identity;

  /*
    Render label with a Postscript font.
  */
  (void) LogMagickEvent(AnnotateEvent,GetMagickModule(),
    "Font %.1024s; pointsize %g",draw_info->font != (char *) NULL ?
    draw_info->font : "none",draw_info->pointsize);
  file=AcquireTemporaryFileStream(filename,BinaryFileIOMode);
  if (file == (FILE *) NULL)
    ThrowBinaryException(FileOpenError,UnableToCreateTemporaryFile,filename);
  (void) fprintf(file,"%%!PS-Adobe-3.0\n");
  (void) fprintf(file,"/ReencodeType\n");
  (void) fprintf(file,"{\n");
  (void) fprintf(file,"  findfont dup length\n");
  (void) fprintf(file,
    "  dict begin { 1 index /FID ne {def} {pop pop} ifelse } forall\n");
  (void) fprintf(file,
    "  /Encoding ISOLatin1Encoding def currentdict end definefont pop\n");
  (void) fprintf(file,"} bind def\n");
  /*
    Sample to compute bounding box.
  */
  identity=(draw_info->affine.sx == draw_info->affine.sy) &&
    (draw_info->affine.rx == 0.0) && (draw_info->affine.ry == 0.0);
  extent.x=0.0;
  extent.y=0.0;
  for (i=0; i <= (long) (strlen(draw_info->text)+2); i++)
  {
    point.x=fabs(draw_info->affine.sx*i*draw_info->pointsize+
      draw_info->affine.ry*2.0*draw_info->pointsize);
    point.y=fabs(draw_info->affine.rx*i*draw_info->pointsize+
      draw_info->affine.sy*2.0*draw_info->pointsize);
    if (point.x > extent.x)
      extent.x=point.x;
    if (point.y > extent.y)
      extent.y=point.y;
  }
  (void) fprintf(file,"%g %g moveto\n",identity ? 0.0 : extent.x/2.0,
    extent.y/2.0);
  (void) fprintf(file,"%g %g scale\n",draw_info->pointsize,
    draw_info->pointsize);
  if ((draw_info->font == (char *) NULL) || (*draw_info->font == '\0'))
    (void) fprintf(file,
      "/Times-Roman-ISO dup /Times-Roman ReencodeType findfont setfont\n");
  else
    (void) fprintf(file,
      "/%.1024s-ISO dup /%.1024s ReencodeType findfont setfont\n",
      draw_info->font,draw_info->font);
  (void) fprintf(file,"[%g %g %g %g 0 0] concat\n",draw_info->affine.sx,
    -draw_info->affine.rx,-draw_info->affine.ry,draw_info->affine.sy);
  text=EscapeParenthesis(draw_info->text);
  if (!identity)
    (void) fprintf(file,"(%.1024s) stringwidth pop -0.5 mul -0.5 rmoveto\n",
      text);
  (void) fprintf(file,"(%.1024s) show\n",text);
  MagickFreeMemory(text);
  (void) fprintf(file,"showpage\n");
  (void) fclose(file);
  FormatString(geometry,"%ldx%ld+0+0!",(long) ceil(extent.x-0.5),
    (long) ceil(extent.y-0.5));
  clone_info=CloneImageInfo((ImageInfo *) NULL);
  (void) FormatString(clone_info->filename,"ps:%.1024s",filename);
  (void) CloneString(&clone_info->page,geometry);
  if (draw_info->density != (char *) NULL)
    (void) CloneString(&clone_info->density,draw_info->density);
  clone_info->antialias=draw_info->text_antialias;
  annotate_image=ReadImage(clone_info,&image->exception);
  if (image->exception.severity != UndefinedException)
    MagickError2(image->exception.severity,image->exception.reason,
      image->exception.description);
  DestroyImageInfo(clone_info);
  (void) LiberateTemporaryFile(filename);
  if (annotate_image == (Image *) NULL)
    return(False);
  resolution.x=72.0;
  resolution.y=72.0;
  if (draw_info->density != (char *) NULL)
    {
      int
        count;

      count=GetMagickDimension(draw_info->density,&resolution.x,&resolution.y,NULL,NULL);
      if (count != 2)
        resolution.y=resolution.x;
    }
  if (!identity)
    TransformImage(&annotate_image,"0x0",(char *) NULL);
  else
    {
      RectangleInfo
        crop_info;

      crop_info=GetImageBoundingBox(annotate_image,&annotate_image->exception);
      crop_info.height=(unsigned long) ceil((resolution.y/72.0)*
        ExpandAffine(&draw_info->affine)*draw_info->pointsize-0.5);
      crop_info.y=(long) ceil((resolution.y/72.0)*extent.y/8.0-0.5);
      (void) FormatString(geometry,"%lux%lu%+ld%+ld",crop_info.width,
        crop_info.height,crop_info.x,crop_info.y);
      TransformImage(&annotate_image,geometry,(char *) NULL);
    }
  metrics->pixels_per_em.x=(resolution.y/72.0)*
    ExpandAffine(&draw_info->affine)*draw_info->pointsize;
  metrics->pixels_per_em.y=metrics->pixels_per_em.x;
  metrics->ascent=metrics->pixels_per_em.x;
  metrics->descent=metrics->pixels_per_em.y/-5.0;
  metrics->width=annotate_image->columns/ExpandAffine(&draw_info->affine);
  metrics->height=1.152*metrics->pixels_per_em.x;
  metrics->max_advance=metrics->pixels_per_em.x;
  metrics->bounds.x1=0.0;
  metrics->bounds.y1=metrics->descent;
  metrics->bounds.x2=metrics->ascent+metrics->descent;
  metrics->bounds.y2=metrics->ascent+metrics->descent;
  metrics->underline_position=(-2.0);
  metrics->underline_thickness=1.0;
  if (!draw_info->render)
    {
      DestroyImage(annotate_image);
      return(True);
    }
  if (draw_info->fill.opacity != TransparentOpacity)
    {
      PixelPacket
        fill_color;

      /*
        Render fill color.
      */
      (void) SetImageType(annotate_image,TrueColorMatteType);
      fill_color=draw_info->fill;
      pattern=draw_info->fill_pattern;
      for (y=0; y < (long) annotate_image->rows; y++)
      {
        q=GetImagePixels(annotate_image,0,y,annotate_image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (long) annotate_image->columns; x++)
        {
          if (pattern != (Image *) NULL)
            (void) AcquireOnePixelByReference(pattern,&fill_color,
              (long) (x-pattern->tile_info.x) % pattern->columns,
              (long) (y-pattern->tile_info.y) % pattern->rows,
              &image->exception);
          q->opacity=(Quantum) (MaxRGB-(((MaxRGB-(double)
            PixelIntensityToQuantum(q))*(MaxRGB-fill_color.opacity))/
            MaxRGB)+0.5);
          q->red=fill_color.red;
          q->green=fill_color.green;
          q->blue=fill_color.blue;
          q++;
        }
        if (!SyncImagePixels(annotate_image))
          break;
      }
      (void) CompositeImage(image,OverCompositeOp,annotate_image,(long)
        ceil(offset->x-0.5),(long) ceil(offset->y-(metrics->ascent+
        metrics->descent)-0.5));
    }
  DestroyImage(annotate_image);
  return(MagickPass);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   R e n d e r X 1 1                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RenderX11 renders text on the image with an X11 font.  It also
%  returns the bounding box of the text relative to the image.
%
%  The format of the RenderX11 method is:
%
%      unsigned int RenderX11(Image *image,DrawInfo *draw_info,
%        const PointInfo *offset,TypeMetric *metrics)
%
%  A description of each parameter follows:
%
%    o status: Method RenderX11 returns True if the text is rendered on the
%      image, otherwise False.
%
%    o image: The image.
%
%    o draw_info: The draw info.
%
%    o offset: (x,y) location of text relative to image.
%
%    o metrics: bounding box of text.
%
%
*/
#if defined(HasX11)
static MagickPassFail RenderX11(Image *image,const DrawInfo *draw_info,
  const PointInfo *offset,TypeMetric *metrics)
{
  static DrawInfo
    cache_info;

  static Display
    *display = (Display *) NULL;

  static MagickXAnnotateInfo
    annotate_info;

  static XFontStruct
    *font_info;

  static MagickXPixelInfo
    pixel;

  static MagickXResourceInfo
    resource_info;

  static XrmDatabase
    resource_database;

  static XStandardColormap
    *map_info;

  static XVisualInfo
    *visual_info;

  MagickPassFail
    status;

  unsigned long
    height,
    width;

  if (display == (Display *) NULL)
    {
      const char
        *client_name;

      /*
        Open X server connection.
      */
      display=XOpenDisplay(draw_info->server_name);
      if (display == (Display *) NULL)
        ThrowBinaryException(XServerError,UnableToOpenXServer,
          draw_info->server_name);
      /*
        Get user defaults from X resource database.
      */
      (void) XSetErrorHandler(MagickXError);
      client_name=GetClientName();
      resource_database=MagickXGetResourceDatabase(display,client_name);
      MagickXGetResourceInfo(resource_database,client_name,&resource_info);
      resource_info.close_server=False;
      resource_info.colormap=PrivateColormap;
      resource_info.font=AllocateString(draw_info->font);
      resource_info.background_color=AllocateString("#ffffffffffff");
      resource_info.foreground_color=AllocateString("#000000000000");
      map_info=XAllocStandardColormap();
      if (map_info == (XStandardColormap *) NULL)
        ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
          UnableToAllocateColormap);
      /*
        Initialize visual info.
      */
      visual_info=MagickXBestVisualInfo(display,map_info,&resource_info);
      if (visual_info == (XVisualInfo *) NULL)
        ThrowBinaryException(XServerError,UnableToGetVisual,
          draw_info->server_name);
      map_info->colormap=(Colormap) NULL;
      pixel.pixels=(unsigned long *) NULL;
      /*
        Initialize Standard Colormap info.
      */
      MagickXGetMapInfo(visual_info,XDefaultColormap(display,visual_info->screen),
        map_info);
      MagickXGetPixelPacket(display,visual_info,map_info,&resource_info,
        (Image *) NULL,&pixel);
      pixel.annotate_context=XDefaultGC(display,visual_info->screen);
      /*
        Initialize font info.
      */
      font_info=MagickXBestFont(display,&resource_info,False);
      if (font_info == (XFontStruct *) NULL)
        ThrowBinaryException(XServerError,UnableToLoadFont,draw_info->font);
      if ((map_info == (XStandardColormap *) NULL) ||
          (visual_info == (XVisualInfo *) NULL) ||
          (font_info == (XFontStruct *) NULL))
        {
          MagickXFreeResources(display,visual_info,map_info,&pixel,font_info,
            &resource_info,(MagickXWindowInfo *) NULL);
          ThrowBinaryException(XServerError,UnableToLoadFont,
            draw_info->server_name)
        }
      cache_info=(*draw_info);
    }
  /*
    Initialize annotate info.
  */
  MagickXGetAnnotateInfo(&annotate_info);
  annotate_info.stencil=ForegroundStencil;
  if (cache_info.font != draw_info->font)
    {
      /*
        Type name has changed.
      */
      (void) XFreeFont(display,font_info);
      (void) CloneString(&resource_info.font,draw_info->font);
      font_info=MagickXBestFont(display,&resource_info,False);
      if (font_info == (XFontStruct *) NULL)
        ThrowBinaryException(XServerError,UnableToLoadFont,draw_info->font);
    }
  (void) LogMagickEvent(AnnotateEvent,GetMagickModule(),
    "Font %.1024s; pointsize %g",draw_info->font != (char *) NULL ?
    draw_info->font : "none",draw_info->pointsize);
  cache_info=(*draw_info);
  annotate_info.font_info=font_info;
  annotate_info.text=(char *) draw_info->text;
  annotate_info.width=XTextWidth(font_info,draw_info->text,
    (int) strlen(draw_info->text));
  annotate_info.height=font_info->ascent+font_info->descent;
  metrics->pixels_per_em.x=font_info->max_bounds.width;
  metrics->pixels_per_em.y=font_info->max_bounds.width;
  metrics->ascent=font_info->ascent;
  metrics->descent=(-font_info->descent);
  metrics->width=annotate_info.width/ExpandAffine(&draw_info->affine);
  metrics->height=metrics->pixels_per_em.x+4;
  metrics->max_advance=font_info->max_bounds.width;
  metrics->bounds.x1=0.0;
  metrics->bounds.y1=metrics->descent;
  metrics->bounds.x2=metrics->ascent+metrics->descent;
  metrics->bounds.y2=metrics->ascent+metrics->descent;
  metrics->underline_position=(-2.0);
  metrics->underline_thickness=1.0;
  if (!draw_info->render)
    return(MagickPass);
  if (draw_info->fill.opacity == TransparentOpacity)
    return(MagickPass);
  /*
    Render fill color.
  */
  width=annotate_info.width;
  height=annotate_info.height;
  if ((draw_info->affine.rx != 0.0) || (draw_info->affine.ry != 0.0))
    {
      if (((draw_info->affine.sx-draw_info->affine.sy) == 0.0) &&
          ((draw_info->affine.rx+draw_info->affine.ry) == 0.0))
        annotate_info.degrees=(180.0/MagickPI)*
          atan2(draw_info->affine.rx,draw_info->affine.sx);
    }
  FormatString(annotate_info.geometry,"%lux%lu+%ld+%ld",width,height,
    (long) ceil(offset->x-0.5),
    (long) ceil(offset->y-metrics->ascent-metrics->descent-0.5));
  pixel.pen_color.red=ScaleQuantumToShort(draw_info->fill.red);
  pixel.pen_color.green=ScaleQuantumToShort(draw_info->fill.green);
  pixel.pen_color.blue=ScaleQuantumToShort(draw_info->fill.blue);
  status=MagickXAnnotateImage(display,&pixel,&annotate_info,image);
  if (status == 0)
    ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
      UnableToAnnotateImage);
  return(MagickPass);
}
#else
static MagickPassFail RenderX11(Image *image,const DrawInfo *draw_info,
  const PointInfo *ARGUNUSED(offset),TypeMetric *ARGUNUSED(metrics))
{
  ThrowBinaryException(MissingDelegateError,XWindowLibraryIsNotAvailable,
    draw_info->font);
}
#endif
