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
%                        DDDD   RRRR    AAA   W   W                           %
%                        D   D  R   R  A   A  W   W                           %
%                        D   D  RRRR   AAAAA  W   W                           %
%                        D   D  R R    A   A  W W W                           %
%                        DDDD   R  R   A   A   W W                            %
%                                                                             %
%                                                                             %
%                 GraphicsMagick Image Vector Drawing Methods                 %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                              Bob Friesenhahn                                %
%                                March 2002                                   %
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
#include "magick/blob.h"
#include "magick/color.h"
#include "magick/color_lookup.h"
#include "magick/draw.h"
#include "magick/enum_strings.h"
#include "magick/gem.h"
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/utility.h"

/*
  Define declarations.
*/
#define DRAW_BINARY_IMPLEMENTATION 0

#define ThrowDrawException(code,reason,description) \
{ \
  if (context->image->exception.severity > (long)code) \
    ThrowException(&context->image->exception,code,reason,description); \
  return; \
}
#define ThrowDrawException3(code,reason,description) \
{ \
  if (context->image->exception.severity > (long)code) \
    ThrowException3(&context->image->exception,code,reason,description); \
  return; \
}

#define CurrentContext (context->graphic_context[context->index])
#define PixelPacketMatch(p,q) (((p)->red == (q)->red) && \
  ((p)->green == (q)->green) && ((p)->blue == (q)->blue) && \
  ((p)->opacity == (q)->opacity))

/*
  Typedef declarations.
*/
typedef enum
{
  PathDefaultOperation,
  PathCloseOperation,                           /* Z|z (none) */
  PathCurveToOperation,                         /* C|c (x1 y1 x2 y2 x y)+ */
  PathCurveToQuadraticBezierOperation,          /* Q|q (x1 y1 x y)+ */
  PathCurveToQuadraticBezierSmoothOperation,    /* T|t (x y)+ */
  PathCurveToSmoothOperation,                   /* S|s (x2 y2 x y)+ */
  PathEllipticArcOperation,                     /* A|a (rx ry x-axis-rotation large-arc-flag sweep-flag x y)+ */
  PathLineToHorizontalOperation,                /* H|h x+ */
  PathLineToOperation,                          /* L|l (x y)+ */
  PathLineToVerticalOperation,                  /* V|v y+ */
  PathMoveToOperation                           /* M|m (x y)+ */
} PathOperation;

typedef enum
{
  DefaultPathMode,
  AbsolutePathMode,
  RelativePathMode
} PathMode;

struct _DrawContext
{
  /* Support structures */
  Image
    *image;

  /* MVG output string and housekeeping */
  char
    *mvg;               /* MVG data */

  size_t
    mvg_alloc,          /* total allocated memory */
    mvg_length;         /* total MVG length */

  unsigned int
    mvg_width;          /* current line length */

  /* Pattern support */
  char
    *pattern_id;

  RectangleInfo
    pattern_bounds;

  size_t
    pattern_offset;

  /* Graphic context */
  unsigned int
    index;              /* array index */

  DrawInfo
    **graphic_context;

  int
    filter_off;         /* true if not filtering attributes */

  /* Pretty-printing depth */
  unsigned int
    indent_depth;       /* number of left-hand pad characters */

  /* Path operation support */
  PathOperation
    path_operation;

  PathMode
    path_mode;

  /* Structure unique signature */
  unsigned long
    signature;
};

/* Vector table for invoking subordinate renderers */
#if 0
struct _DrawVTable
{
  void (*DrawAnnotation)
    (DrawContext context, const double x, const double y,
     const unsigned char *text);
  void (*DrawArc)
    (DrawContext context, const double sx, const double sy,
     const double ex, const double ey, const double sd, const double ed);
  void (*DrawBezier)
    (DrawContext context, const unsigned long num_coords, const PointInfo *coordinates);
  void (*DrawCircle)
    (DrawContext context, const double ox, const double oy,
     const double px, const double py);
  void (*DrawColor)
    (DrawContext context, const double x, const double y,
     const PaintMethod paintMethod);
  void (*DrawComment)
    (DrawContext context,const char* comment);
  void (*DrawDestroyContext)
    (DrawContext context);
  void (*DrawEllipse)
    (DrawContext context, const double ox, const double oy,
     const double rx, const double ry, const double start, const double end);
  void (*DrawComposite)
    (DrawContext context, const CompositeOperator composite_operator,
     const double x, const double y, const double width, const double height,
     const Image * image );
  void (*DrawLine)
    (DrawContext context, const double sx, const double sy,
     const double ex, const double ey);
  void (*DrawMatte)
    (DrawContext context, const double x, const double y,
     const PaintMethod paint_method);
  void (*DrawPathClose)
    (DrawContext context);
  void (*DrawPathCurveToAbsolute)
    (DrawContext context, const double x1, const double y1,
     const double x2, const double y2, const double x, const double y);
  void (*DrawPathCurveToRelative)
    (DrawContext context, const double x1, const double y1,
     const double x2, const double y2, const double x, const double y);
  void (*DrawPathCurveToQuadraticBezierAbsolute)
    (DrawContext context, const double x1, const double y1,
     const double x, const double y);
  void (*DrawPathCurveToQuadraticBezierRelative)
    (DrawContext context, const double x1, const double y1,
     const double x, const double y);
  void (*DrawPathCurveToQuadraticBezierSmoothAbsolute)
    (DrawContext context, const double x, const double y);
  void (*DrawPathCurveToQuadraticBezierSmoothRelative)
    (DrawContext context, const double x, const double y);
  void (*DrawPathCurveToSmoothAbsolute)
    (DrawContext context, const double x2, const double y2,
     const double x, const double y);
  void (*DrawPathCurveToSmoothRelative)
    (DrawContext context, const double x2, const double y2,
     const double x, const double y);
  void (*DrawPathEllipticArcAbsolute)
    (DrawContext context, const double rx, const double ry,
     const double x_axis_rotation, unsigned int large_arc_flag,
     unsigned int sweep_flag, const double x, const double y);
  void (*DrawPathEllipticArcRelative)
    (DrawContext context, const double rx, const double ry,
     const double x_axis_rotation, unsigned int large_arc_flag,
     unsigned int sweep_flag, const double x, const double y);
  void (*DrawPathFinish)
    (DrawContext context);
  void (*DrawPathLineToAbsolute)
    (DrawContext context, const double x, const double y);
  void (*DrawPathLineToRelative)
    (DrawContext context, const double x, const double y);
  void (*DrawPathLineToHorizontalAbsolute)
    (DrawContext context, const double x);
  void (*DrawPathLineToHorizontalRelative)
    (DrawContext context, const double x);
  void (*DrawPathLineToVerticalAbsolute)
    (DrawContext context, const double y);
  void (*DrawPathLineToVerticalRelative)
    (DrawContext context, const double y);
  void (*DrawPathMoveToAbsolute)
    (DrawContext context, const double x, const double y);
  void (*DrawPathMoveToRelative)
    (DrawContext context, const double x, const double y);
  void (*DrawPathStart)
    (DrawContext context);
  void (*DrawPoint)
    (DrawContext context, const double x, const double y);
  void (*DrawPolygon)
    (DrawContext context, const unsigned long num_coords, const PointInfo * coordinates);
  void (*DrawPolyline)
    (DrawContext context, const unsigned long num_coords, const PointInfo * coordinates);
  void (*DrawPopClipPath)
    (DrawContext context);
  void (*DrawPopDefs)
    (DrawContext context);
  void (*DrawPopGraphicContext)
    (DrawContext context);
  void (*DrawPopPattern)
    (DrawContext context);
  void (*DrawPushClipPath)
    (DrawContext context, const char *clip_path_id);
  void (*DrawPushDefs)
    (DrawContext context);
  void (*DrawPushGraphicContext)
    (DrawContext context);
  void (*DrawPushPattern)
    (DrawContext context, const char *pattern_id,
     const double x, const double y, const double width, const double height);
  void (*DrawRectangle)
    (DrawContext context, const double x1, const double y1,
     const double x2, const double y2);
  void (*DrawRoundRectangle)
    (DrawContext context, double x1, double y1,
     double x2, double y2, double rx, double ry);
  void (*DrawAffine)
    (DrawContext context, const AffineMatrix *affine);
  void (*DrawSetClipPath)
    (DrawContext context, const char *clip_path);
  void (*DrawSetClipRule)
    (DrawContext context, const FillRule fill_rule);
  void (*DrawSetClipUnits)
    (DrawContext context, const ClipPathUnits clip_units);
  void (*DrawSetFillColor)
    (DrawContext context, const PixelPacket * fill_color);
  void (*DrawSetFillOpacity)
    (DrawContext context, const double fill_opacity);
  void (*DrawSetFillRule)
    (DrawContext context, const FillRule fill_rule);
  void (*DrawSetFillPatternURL)
    (DrawContext context, const char* fill_url);
  void (*DrawSetFont)
    (DrawContext context, const char *font_name);
  void (*DrawSetFontFamily)
    (DrawContext context, const char *font_family);
  void (*DrawSetFontSize)
    (DrawContext context, const double font_pointsize);
  void (*DrawSetFontStretch)
    (DrawContext context, const StretchType font_stretch);
  void (*DrawSetFontStyle)
    (DrawContext context, const StyleType font_style);
  void (*DrawSetFontWeight)
    (DrawContext context, const unsigned long font_weight);
  void (*DrawSetGravity)
    (DrawContext context, const GravityType gravity);
  void (*DrawRotate)
    (DrawContext context, const double degrees);
  void (*DrawScale)
    (DrawContext context, const double x, const double y);
  void (*DrawSkewX)
    (DrawContext context, const double degrees);
  void (*DrawSkewY)
    (DrawContext context, const double degrees);
/*   void (*DrawSetStopColor) */
/*     (DrawContext context, const PixelPacket * color, const double offset); */
  void (*DrawSetStrokeAntialias)
    (DrawContext context, const unsigned int true_false);
  void (*DrawSetStrokeColor)
    (DrawContext context, const PixelPacket * stroke_color);
  void (*DrawSetStrokeDashArray)
    (DrawContext context,const double *dasharray);
  void (*DrawSetStrokeDashOffset)
    (DrawContext context,const double dashoffset);
  void (*DrawSetStrokeLineCap)
    (DrawContext context, const LineCap linecap);
  void (*DrawSetStrokeLineJoin)
    (DrawContext context, const LineJoin linejoin);
  void (*DrawSetStrokeMiterLimit)
    (DrawContext context,const unsigned long miterlimit);
  void (*DrawSetStrokeOpacity)
    (DrawContext context, const double opacity);
  void (*DrawSetStrokePatternURL)
    (DrawContext context, const char* stroke_url);
  void (*DrawSetStrokeWidth)
    (DrawContext context, const double width);
  void (*DrawSetTextAntialias)
    (DrawContext context, const unsigned int true_false);
  void (*DrawSetTextDecoration)
    (DrawContext context, const DecorationType decoration);
  void (*DrawSetTextUnderColor)
    (DrawContext context, const PixelPacket * color);
  void (*DrawTranslate)
    (DrawContext context, const double x, const double y);
  void (*DrawSetViewbox)
    (DrawContext context, unsigned long x1, unsigned long y1,
     unsigned long x2, unsigned long y2);
};
#endif

/*
  Forward declarations.
*/
static int
  MvgPrintf(DrawContext context, const char *format, ...)
#if defined(__GNUC__)
__attribute__ ((format (printf, 2, 3)))
#endif
,
  MvgAutoWrapPrintf(DrawContext context, const char *format, ...)
#if defined(__GNUC__)
__attribute__ ((format (printf, 2, 3)))
#endif
;
static void
  MvgAppendColor(DrawContext context, const PixelPacket *color);


/* "Printf" for MVG commands */
static int MvgPrintf(DrawContext context, const char *format, ...)
{
  const size_t
    alloc_size = MaxTextExtent * 20; /* 40K */

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  /* Allocate initial memory */
  if (context->mvg == (char*) NULL)
    {
      context->mvg = MagickAllocateMemory(char *,alloc_size);
      if( context->mvg == (char*) NULL )
        {
          ThrowException3(&context->image->exception,ResourceLimitError,
            MemoryAllocationFailed,UnableToDrawOnImage);
          return -1;
        }

      context->mvg_alloc = alloc_size;
      context->mvg_length = 0;
      if (context->mvg == 0)
        {
          ThrowException3(&context->image->exception,ResourceLimitError,
            MemoryAllocationFailed,UnableToDrawOnImage);
          return -1;
        }
    }

  /* Re-allocate additional memory if necessary (ensure 20K unused) */
  if (context->mvg_alloc < (context->mvg_length + MaxTextExtent * 10))
    {
      size_t realloc_size = context->mvg_alloc + alloc_size;

      MagickReallocMemory(char *, context->mvg, realloc_size);
      if (context->mvg == NULL)
        {
          ThrowException3(&context->image->exception,ResourceLimitError,
            MemoryAllocationFailed,UnableToDrawOnImage);
          return -1;
        }
      context->mvg_alloc = realloc_size;
    }

  /* Write to end of existing MVG string */
  {
    int
      formatted_length; /* must be a signed type! */

    va_list
      argp;

    /* Pretty-print indentation */
    while(context->mvg_width < context->indent_depth)
      {
        context->mvg[context->mvg_length] = ' ';
        ++context->mvg_length;
        ++context->mvg_width;
      }
    context->mvg[context->mvg_length] = 0;

    va_start(argp, format);
#if defined(HAVE_VSNPRINTF)
    formatted_length =
      vsnprintf(context->mvg + context->mvg_length,
                context->mvg_alloc - context->mvg_length - 1, format, argp);
#else
#  if defined(HAVE_VSPRINTF)
    formatted_length = vsprintf(context->mvg + context->mvg_length, format, argp);
#  else
#    error Neither vsnprintf or vsprintf is available.
#  endif
#endif
    va_end(argp);

    if (formatted_length < 0)
      {
        ThrowException(&context->image->exception,DrawError,UnableToPrint,
          format);
      }
    else
      {
        context->mvg_length += formatted_length;
        context->mvg_width += formatted_length;
      }
    context->mvg[context->mvg_length] = 0;

    /* Re-evaluate mvg_width */
    if( (context->mvg_length > 1) &&
        (context->mvg[context->mvg_length-1] == '\n') )
      context->mvg_width = 0;

    assert(context->mvg_length + 1 < context->mvg_alloc);

    return formatted_length;
  }
}

/* "Printf" for MVG commands, with autowrap at 78 characters */
static int MvgAutoWrapPrintf(DrawContext context, const char *format, ...)
{
  va_list
    argp;

  int
    formatted_length;

  char
    buffer[MaxTextExtent];

  va_start(argp, format);
#if defined(HAVE_VSNPRINTF)
  formatted_length = vsnprintf(buffer, sizeof(buffer) - 1, format, argp);
#else
#  if defined(HAVE_VSPRINTF)
  formatted_length = vsprintf(buffer, format, argp);
#  else
#    error Neither vsnprintf or vsprintf is available.
#  endif
#endif
  va_end(argp);
  buffer[sizeof(buffer)-1]=0;

  if (formatted_length < 0)
    {
      ThrowException(&context->image->exception,DrawError,UnableToPrint,
        format);
    }
  else
    {
      if( ((context->mvg_width + formatted_length) > 78) &&
          buffer[formatted_length-1] != '\n' )
        (void) MvgPrintf(context, "\n");

      (void) MvgPrintf(context, "%s", buffer);
    }

  return formatted_length;
}

static void MvgAppendColor(DrawContext context, const PixelPacket *color)
{
  if(color->red == 0U && color->green == 0U && color->blue == 0U &&
     color->opacity == TransparentOpacity)
    {
      (void) MvgPrintf(context,"none");
    }
  else
    {
      char
        tuple[MaxTextExtent];

      GetColorTuple(color,context->image->depth,context->image->matte,True,
                    tuple);
      (void) MvgPrintf(context,"%.1024s",tuple);
    }
}

static void MvgAppendPointsCommand(DrawContext context, const char* command,
                                   const unsigned long num_coords,
                                   const PointInfo * coordinates)
{
  const PointInfo
    *coordinate;

  size_t
    i;

  (void) MvgPrintf(context, "%.1024s", command);
  for (i = num_coords, coordinate = coordinates; i; i--)
    {
      (void) MvgAutoWrapPrintf(context," %.4g,%.4g", coordinate->x, coordinate->y);
      ++coordinate;
    }

  (void) MvgPrintf(context, "\n");
}

static void AdjustAffine(DrawContext context, const AffineMatrix *affine)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if ((affine->sx != 1.0) || (affine->rx != 0.0) || (affine->ry != 0.0) ||
      (affine->sy != 1.0) || (affine->tx != 0.0) || (affine->ty != 0.0))
    {
      AffineMatrix
        current;

      current = CurrentContext->affine;
      CurrentContext->affine.sx=current.sx*affine->sx+current.ry*affine->rx;
      CurrentContext->affine.rx=current.rx*affine->sx+current.sy*affine->rx;
      CurrentContext->affine.ry=current.sx*affine->ry+current.ry*affine->sy;
      CurrentContext->affine.sy=current.rx*affine->ry+current.sy*affine->sy;
      CurrentContext->affine.tx=current.sx*affine->tx+current.ry*affine->ty+current.tx;
      CurrentContext->affine.ty=current.rx*affine->tx+current.sy*affine->ty+current.ty;
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w A n n o t a t i o n                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawAnnotation() draws text on the image.
%
%  The format of the DrawAnnotation method is:
%
%      void DrawAnnotation(DrawContext context,
%                          const double x, const double y,
%                          const unsigned char *text)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o x: x ordinate to left of text
%
%    o y: y ordinate to text baseline
%
%    o text: text to draw
%
*/
MagickExport void DrawAnnotation(DrawContext context,
                                 const double x, const double y,
                                 const unsigned char *text)
{
  char
    *escaped_text;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(text != (const unsigned char *) NULL);

  escaped_text=EscapeString((const char*)text,'\'');
  (void) MvgPrintf(context, "text %.4g,%.4g '%.1024s'\n", x, y, escaped_text);
  MagickFreeMemory(escaped_text);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w A f f i n e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawAffine() adjusts the current affine transformation matrix with
%  the specified affine transformation matrix. Note that the current affine
%  transform is adjusted rather than replaced.
%
%  The format of the DrawAffine method is:
%
%      void DrawAffine(DrawContext context, const AffineMatrix *affine)
%
%  A description of each parameter follows:
%
%    o context: Drawing context
%
%    o affine: Affine matrix parameters
%
*/
MagickExport void DrawAffine(DrawContext context, const AffineMatrix *affine)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(affine != (const AffineMatrix *)NULL);

  AdjustAffine( context, affine );

  (void) MvgPrintf(context, "affine %.6g,%.6g,%.6g,%.6g,%.6g,%.6g\n",
                   affine->sx, affine->rx, affine->ry, affine->sy,
                   affine->tx, affine->ty);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w A l l o c a t e C o n t e x t                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawAllocateContext() allocates an initial drawing context which is an
%  opaque handle required by the remaining drawing methods.
%
%  The format of the DrawAllocateContext method is:
%
%      DrawContext DrawAllocateContext(const DrawInfo *draw_info,
%                                      Image *image)
%
%  A description of each parameter follows:
%
%    o draw_info: Initial drawing defaults. Set to NULL to use
%                 GraphicsMagick defaults.
%
%    o image: The image to draw on.
%
*/
MagickExport DrawContext DrawAllocateContext(const DrawInfo *draw_info,
                                             Image *image)
{
  DrawContext
    context;

  /* Allocate initial drawing context */
  context = MagickAllocateMemory(DrawContext,sizeof(struct _DrawContext));
  if(context == (DrawContext) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToAllocateDrawContext);

  /* Support structures */
  context->image = image;

  /* MVG output string and housekeeping */
  context->mvg = NULL;
  context->mvg_alloc = 0;
  context->mvg_length = 0;
  context->mvg_width = 0;

  /* Pattern support */
  context->pattern_id = NULL;
  context->pattern_offset = 0;

  context->pattern_bounds.x = 0;
  context->pattern_bounds.y = 0;
  context->pattern_bounds.width = 0;
  context->pattern_bounds.height = 0;

  /* Graphic context */
  context->index = 0;
  context->graphic_context=MagickAllocateMemory(DrawInfo **,sizeof(DrawInfo *));
  if(context->graphic_context == (DrawInfo **) NULL)
    {
      ThrowException3(&context->image->exception,ResourceLimitError,
        MemoryAllocationFailed,UnableToDrawOnImage);
      return (DrawContext) NULL;
    }
  CurrentContext=CloneDrawInfo((ImageInfo*)NULL,draw_info);
  if(CurrentContext == (DrawInfo*) NULL)
    {
      ThrowException3(&context->image->exception,ResourceLimitError,
        MemoryAllocationFailed,UnableToDrawOnImage);
      return (DrawContext) NULL;
    }

  context->filter_off = False;

  /* Pretty-printing depth */
  context->indent_depth = 0;

  /* Path operation support */
  context->path_operation = PathDefaultOperation;
  context->path_mode = DefaultPathMode;

  /* Structure unique signature */
  context->signature = MagickSignature;

  return context;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w A r c                                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawArc() draws an arc falling within a specified bounding rectangle on the
%  image.
%
%  The format of the DrawArc method is:
%
%      void DrawArc(DrawContext context,
%                   const double sx, const double sy,
%                   const double ex, const double ey,
%                   const double sd, const double ed)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o sx: starting x ordinate of bounding rectangle
%
%    o sy: starting y ordinate of bounding rectangle
%
%    o ex: ending x ordinate of bounding rectangle
%
%    o ey: ending y ordinate of bounding rectangle
%
%    o sd: starting degrees of rotation
%
%    o ed: ending degrees of rotation
%
*/
MagickExport void DrawArc(DrawContext context,
                          const double sx, const double sy,
                          const double ex, const double ey,
                          const double sd, const double ed)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  (void) MvgPrintf(context, "arc %.4g,%.4g %.4g,%.4g %.4g,%.4g\n",
                   sx, sy, ex, ey, sd, ed);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w B e z i e r                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawBezier() draws a bezier curve through a set of points on the image.
%
%  The format of the DrawBezier method is:
%
%      void DrawBezier(DrawContext context, const unsigned long num_coords,
%                      const PointInfo *coordinates)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o num_coords: number of coordinates
%
%    o coordinates: coordinates
%
*/
MagickExport void DrawBezier(DrawContext context, const unsigned long num_coords,
                             const PointInfo *coordinates)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(coordinates != (const PointInfo *) NULL);

  MvgAppendPointsCommand(context,"bezier",num_coords,coordinates);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w C i r c l e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawCircle() draws a circle on the image.
%
%  The format of the DrawCircle method is:
%
%      void DrawCircle(DrawContext context, const double ox,
%                      const double oy, const double px, const double py)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o ox: origin x ordinate
%
%    o oy: origin y ordinate
%
%    o px: perimeter x ordinate
%
%    o py: perimeter y ordinate
%
*/
MagickExport void DrawCircle(DrawContext context, const double ox,
                             const double oy, const double px, const double py)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  (void) MvgPrintf(context, "circle %.4g,%.4g %.4g,%.4g\n", ox, oy, px, py);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t C l i p P a t h                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetClipPath() obtains the current clipping path ID. The value returned
%  must be deallocated by the user when it is no longer needed.
%
%  The format of the DrawGetClipPath method is:
%
%      char *DrawGetClipPath(DrawContext context)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
*/
MagickExport char *DrawGetClipPath(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if (CurrentContext->clip_path != (char *) NULL)
    return (char *)AllocateString(CurrentContext->clip_path);
  else
    return (char *) NULL;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t C l i p P a t h                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetClipPath() associates a named clipping path with the image.  Only
%  the areas drawn on by the clipping path will be modified as long as it
%  remains in effect.
%
%  The format of the DrawSetClipPath method is:
%
%      void DrawSetClipPath(DrawContext context, const char *clip_path)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o clip_path: name of clipping path to associate with image
%
*/
MagickExport void DrawSetClipPath(DrawContext context, const char *clip_path)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(clip_path != (const char *) NULL);

  if( CurrentContext->clip_path == NULL || context->filter_off ||
      LocaleCompare(CurrentContext->clip_path,clip_path) != 0)
    {
      (void) CloneString(&CurrentContext->clip_path,clip_path);
      if(CurrentContext->clip_path == (char*)NULL)
        ThrowDrawException3(ResourceLimitError,MemoryAllocationFailed,
          UnableToDrawOnImage);

#if DRAW_BINARY_IMPLEMENTATION
      (void) DrawClipPath(context->image,CurrentContext,CurrentContext->clip_path);
#endif

      (void) MvgPrintf(context, "clip-path url(#%s)\n", clip_path);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t C l i p R u l e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetClipRule() returns the current polygon fill rule to be used by the
%  clipping path.
%
%  The format of the DrawGetClipRule method is:
%
%     FillRule DrawGetClipRule(DrawContext context)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
*/
MagickExport FillRule DrawGetClipRule(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  return CurrentContext->fill_rule;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t C l i p R u l e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetClipRule() set the polygon fill rule to be used by the clipping path.
%
%  The format of the DrawSetClipRule method is:
%
%      void DrawSetClipRule(DrawContext context,
%                           const FillRule fill_rule)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o fill_rule: fill rule (EvenOddRule or NonZeroRule)
%
*/
MagickExport void DrawSetClipRule(DrawContext context,
                                  const FillRule fill_rule)
{
  const char
    *p = NULL;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->filter_off || (CurrentContext->fill_rule != fill_rule))
    {
      CurrentContext->fill_rule = fill_rule;

      switch (fill_rule)
        {
        case EvenOddRule:
          p = "evenodd";
          break;
        case NonZeroRule:
          p = "nonzero";
          break;
        default:
          break;
        }

      if (p != NULL)
        (void) MvgPrintf(context, "clip-rule %s\n", p);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t C l i p U n i t s                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetClipUnits() returns the interpretation of clip path units.
%
%  The format of the DrawGetClipUnits method is:
%
%      ClipPathUnits DrawGetClipUnits(DrawContext context)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
*/
MagickExport ClipPathUnits DrawGetClipUnits(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  return CurrentContext->clip_units;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t C l i p U n i t s                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetClipUnits() sets the interpretation of clip path units.
%
%  The format of the DrawSetClipUnits method is:
%
%      void DrawSetClipUnits(DrawContext context,
%                            const ClipPathUnits clip_units)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o clip_units: units to use (UserSpace, UserSpaceOnUse, or ObjectBoundingBox)
%
*/
MagickExport void DrawSetClipUnits(DrawContext context,
                                   const ClipPathUnits clip_units)
{
  const char
    *p = NULL;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->filter_off || (CurrentContext->clip_units != clip_units))
    {
      CurrentContext->clip_units = clip_units;

      if ( clip_units == ObjectBoundingBox )
        {
          AffineMatrix
            affine;

          IdentityAffine(&affine);

          affine.sx=CurrentContext->bounds.x2;
          affine.sy=CurrentContext->bounds.y2;
          affine.tx=CurrentContext->bounds.x1;
          affine.ty=CurrentContext->bounds.y1;

          AdjustAffine( context, &affine );
        }

      switch (clip_units)
        {
        case UserSpace:
          p = "userSpace";
          break;
        case UserSpaceOnUse:
          p = "userSpaceOnUse";
          break;
        case ObjectBoundingBox:
          p = "objectBoundingBox";
          break;
        }

      if (p != NULL)
        (void) MvgPrintf(context, "clip-units %s\n", p);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w C o l o r                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawColor() draws color on image using the current fill color, starting at
%  specified position, and using specified paint method. The available paint
%  methods are:
%
%    PointMethod: Recolors the target pixel
%    ReplaceMethod: Recolor any pixel that matches the target pixel.
%    FloodfillMethod: Recolors target pixels and matching neighbors.
%    FillToBorderMethod: Recolor target pixels and neighbors not matching border color.
%    ResetMethod: Recolor all pixels.
%
%  The format of the DrawColor method is:
%
%      void DrawColor(DrawContext context,
%                     const double x, const double y,
%                     const PaintMethod paintMethod)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o x: x ordinate
%
%    o y: y ordinate
%
%    o paintMethod: paint method
%
*/
MagickExport void DrawColor(DrawContext context,
                            const double x, const double y,
                            const PaintMethod paintMethod)
{
  const char
    *p = NULL;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  switch (paintMethod)
    {
    case PointMethod:
      p = "point";
      break;
    case ReplaceMethod:
      p = "replace";
      break;
    case FloodfillMethod:
      p = "floodfill";
      break;
    case FillToBorderMethod:
      p = "filltoborder";
      break;
    case ResetMethod:
      p = "reset";
      break;
    }

  if (p != NULL)
    (void) MvgPrintf(context, "color %.4g,%.4g %s\n", x, y, p);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w C o m m e n t                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawComment() adds a comment to a vector output stream.
%
%  The format of the DrawComment method is:
%
%      void DrawComment(DrawContext context,const char* comment)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o comment: comment text
%
*/
MagickExport void DrawComment(DrawContext context,const char* comment)
{
  /* FIXME: should handle multi-line comments by inserting # before
     new lines */
  (void) MvgPrintf(context, "#%s\n", comment);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w D e s t r o y C o n t e x t                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawDestroyContext() frees all resources associated with the drawing
%  context. Once the drawing context has been freed, it should not be used
%  any further unless it re-allocated.
%
%  The format of the DrawDestroyContext method is:
%
%      void DrawDestroyContext(DrawContext context)
%
%  A description of each parameter follows:
%
%    o context: drawing context to destroy
%
*/
MagickExport void DrawDestroyContext(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  /* Path operation support */
  context->path_operation = PathDefaultOperation;
  context->path_mode = DefaultPathMode;

  /* Pretty-printing depth */
  context->indent_depth = 0;

  /* Graphic context */
  for ( ; context->index != 0; context->index--)
    {
      DestroyDrawInfo(CurrentContext);
      CurrentContext = (DrawInfo*) NULL;
    }
  DestroyDrawInfo(CurrentContext);
  CurrentContext = (DrawInfo*) NULL;
  MagickFreeMemory(context->graphic_context);

  /* Pattern support */
  MagickFreeMemory(context->pattern_id);
  context->pattern_offset = 0;

  context->pattern_bounds.x = 0;
  context->pattern_bounds.y = 0;
  context->pattern_bounds.width = 0;
  context->pattern_bounds.height = 0;

  /* MVG output string and housekeeping */
  MagickFreeMemory(context->mvg);
  context->mvg_alloc = 0;
  context->mvg_length = 0;

  /* Support structures */
  context->image = (Image*)NULL;

  /* Context itself */
  context->signature = 0;
  MagickFreeMemory(context);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w E l l i p s e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawEllipse() draws an ellipse on the image.
%
%  The format of the DrawEllipse method is:
%
%       void DrawEllipse(DrawContext context,
%                        const double ox, const double oy,
%                        const double rx, const double ry,
%                        const double start, const double end)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o ox: origin x ordinate
%
%    o oy: origin y ordinate
%
%    o rx: radius in x
%
%    o ry: radius in y
%
%    o start: starting rotation in degrees
%
%    o end: ending rotation in degrees
%
*/
MagickExport void DrawEllipse(DrawContext context,
                              const double ox, const double oy,
                              const double rx, const double ry,
                              const double start, const double end)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  (void) MvgPrintf(context, "ellipse %.4g,%.4g %.4g,%.4g %.4g,%.4g\n",
                   ox, oy, rx, ry, start, end);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t F i l l C o l o r                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetFillColor() returns the fill color used for drawing filled objects.
%
%  The format of the DrawGetFillColor method is:
%
%      PixelPacket DrawGetFillColor(DrawContext context)
%
%  A description of each parameter follows:
%
%    o context: drawing context
*/
MagickExport PixelPacket DrawGetFillColor(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  return CurrentContext->fill;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t F i l l C o l o r                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetFillColor() sets the fill color to be used for drawing filled objects.
%
%  The format of the DrawSetFillColor method is:
%
%      void DrawSetFillColor(DrawContext context,
%                            const PixelPacket * fill_color)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o fill_color: fill color
%
*/
MagickExport void DrawSetFillColor(DrawContext context,
                                   const PixelPacket *fill_color)
{
  PixelPacket
    *current_fill,
    new_fill;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(fill_color != (const PixelPacket *) NULL);

  new_fill = *fill_color;

  /* Inherit base opacity */
  if(new_fill.opacity == OpaqueOpacity)
    new_fill.opacity = CurrentContext->opacity;

  current_fill = &CurrentContext->fill;
  if( context->filter_off || !(PixelPacketMatch(current_fill,&new_fill)) )
    {
      CurrentContext->fill = new_fill;

      (void) MvgPrintf(context, "fill '");
      MvgAppendColor(context, fill_color);
      (void) MvgPrintf(context, "'\n");
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t F i l l C o l o r S t r i n g                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetFillColorString() sets the fill color to be used for drawing filled
%  objects.
%
%  The format of the DrawSetFillColorString method is:
%
%      void DrawSetFillColorString(DrawContext context, const char* fill_color)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o fill_color: fill color
%
*/
MagickExport void DrawSetFillColorString(DrawContext context,
                                         const char* fill_color)
{
  PixelPacket
    pixel_packet;

  if(QueryColorDatabase(fill_color,&pixel_packet,&context->image->exception))
    DrawSetFillColor(context,&pixel_packet);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t F i l l P a t t e r n U R L                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetFillPatternURL() sets the URL to use as a fill pattern for filling
%  objects. Only local URLs ("#identifier") are supported at this time. These
%  local URLs are normally created by defining a named fill pattern with
%  DrawPushPattern/DrawPopPattern.
%
%  The format of the DrawSetFillPatternURL method is:
%
%      void DrawSetFillPatternURL(DrawContext context, const char* fill_url)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o fill_url: URL to use to obtain fill pattern.
%
*/
MagickExport void DrawSetFillPatternURL(DrawContext context, const char* fill_url)
{
  char
    pattern[MaxTextExtent];

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(fill_url != NULL);

  if(fill_url[0] != '#')
    ThrowDrawException(DrawWarning,NotARelativeURL, fill_url);

  FormatString(pattern,"[%.1024s]",fill_url+1);

  if (GetImageAttribute(context->image,pattern) == (ImageAttribute *) NULL)
    {
      ThrowDrawException(DrawWarning,URLNotFound, fill_url)
    }
  else
    {
      char
        pattern_spec[MaxTextExtent];

      FormatString(pattern_spec,"url(%.1024s)",fill_url);
#if DRAW_BINARY_IMPLEMENTATION
      DrawPatternPath(context->image,CurrentContext,pattern_spec,&CurrentContext->fill_pattern);
#endif
      /* Inherit base opacity */
      if (CurrentContext->fill.opacity != TransparentOpacity)
        CurrentContext->fill.opacity=CurrentContext->opacity;

      (void) MvgPrintf(context, "fill %s\n",pattern_spec);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t F i l l O p a c i t y                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetFillOpacity() returns the opacity used when drawing using the fill
%  color or fill texture.  Fully opaque is 1.0.
%
%  The format of the DrawGetFillOpacity method is:
%
%      double DrawGetFillOpacity(DrawContext context)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
*/
MagickExport double DrawGetFillOpacity(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  return (((double)(MaxRGB-CurrentContext->fill.opacity))/MaxRGB);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t F i l l O p a c i t y                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetFillOpacity() sets the opacity to use when drawing using the fill
%  color or fill texture.  Fully opaque is 1.0.
%
%  The format of the DrawSetFillOpacity method is:
%
%      void DrawSetFillOpacity(DrawContext context, const double fill_opacity)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o fill_opacity: fill opacity
%
*/
MagickExport void DrawSetFillOpacity(DrawContext context,
                                     const double fill_opacity)
{
  Quantum
    quantum_opacity;

  double
    validated_opacity;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  validated_opacity=(fill_opacity < 0.0 ? 0.0 : (fill_opacity > 1.0 ? 1.0 : fill_opacity));
  quantum_opacity = (Quantum) (((double) MaxRGB*(1.0-validated_opacity))+0.5);

  if (context->filter_off || (CurrentContext->fill.opacity != quantum_opacity))
    {
      CurrentContext->fill.opacity = quantum_opacity;
      (void) (void) MvgPrintf(context, "fill-opacity %.4g\n", validated_opacity);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t F i l l R u l e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetFillRule() returns the fill rule used while drawing polygons.
%
%  The format of the DrawGetFillRule method is:
%
%      FillRule DrawGetFillRule(DrawContext context)
%
%  A description of each parameter follows:
%
%    o context: drawing context
*/
MagickExport FillRule DrawGetFillRule(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  return CurrentContext->fill_rule;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t F i l l R u l e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetFillRule() sets the fill rule to use while drawing polygons.
%
%  The format of the DrawSetFillRule method is:
%
%      void DrawSetFillRule(DrawContext context, const FillRule fill_rule)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o fill_rule: fill rule (EvenOddRule or NonZeroRule)
%
*/
MagickExport void DrawSetFillRule(DrawContext context,
                                  const FillRule fill_rule)
{
  const char
    *p = NULL;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->filter_off || (CurrentContext->fill_rule != fill_rule))
    {
      CurrentContext->fill_rule = fill_rule;

      switch (fill_rule)
        {
        case EvenOddRule:
          p = "evenodd";
          break;
        case NonZeroRule:
          p = "nonzero";
          break;
        default:
          break;
        }

      if (p != NULL)
        (void) MvgPrintf(context, "fill-rule %s\n", p);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t F o n t                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetFont() returns a null-terminaged string specifying the font used
%  when annotating with text. The value returned must be freed by the user
%  when no longer needed.
%
%  The format of the DrawGetFont method is:
%
%      char *DrawGetFont(DrawContext context)
%
%  A description of each parameter follows:
%
%    o context: drawing context
*/
MagickExport char *DrawGetFont(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if (CurrentContext->font != (char *) NULL)
    return AllocateString(CurrentContext->font);
  else
    return (char *) NULL;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t F o n t                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetFont() sets the fully-sepecified font to use when annotating with
%  text.
%
%  The format of the DrawSetFont method is:
%
%      void DrawSetFont(DrawContext context, const char *font_name)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o font_name: font name
%
*/
MagickExport void DrawSetFont(DrawContext context, const char *font_name)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(font_name != (const char *) NULL);

  if(context->filter_off || (CurrentContext->font == NULL) ||
     LocaleCompare(CurrentContext->font,font_name) != 0)
    {
      (void) CloneString(&CurrentContext->font,font_name);
      if(CurrentContext->font == (char*)NULL)
        ThrowDrawException3(ResourceLimitError,MemoryAllocationFailed,
          UnableToDrawOnImage);
      (void) MvgPrintf(context, "font '%s'\n", font_name);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t F o n t F a m i l y                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetFontFamily() returns the font family to use when annotating with text.
%  The value returned must be freed by the user when it is no longer needed.
%
%  The format of the DrawGetFontFamily method is:
%
%      char *DrawGetFontFamily(DrawContext context)
%
%  A description of each parameter follows:
%
%    o context: drawing context
*/
MagickExport char *DrawGetFontFamily(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if (CurrentContext->family != NULL)
    return AllocateString(CurrentContext->family);
  else
    return (char *) NULL;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t F o n t F a m i l y                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetFontFamily() sets the font family to use when annotating with text.
%
%  The format of the DrawSetFontFamily method is:
%
%      void DrawSetFontFamily(DrawContext context, const char *font_family)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o font_family: font family
%
*/
MagickExport void DrawSetFontFamily(DrawContext context,
                                    const char *font_family)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(font_family != (const char *) NULL);

  if(context->filter_off || (CurrentContext->family == NULL) ||
     LocaleCompare(CurrentContext->family,font_family) != 0)
    {
      (void) CloneString(&CurrentContext->family,font_family);
      if(CurrentContext->family == (char*)NULL)
        ThrowDrawException3(ResourceLimitError,MemoryAllocationFailed,
          UnableToDrawOnImage);
      (void) MvgPrintf(context, "font-family '%s'\n", font_family);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t F o n t S i z e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetFontSize() returns the font pointsize used when annotating with text.
%
%  The format of the DrawGetFontSize method is:
%
%      double DrawGetFontSize(DrawContext context)
%
%  A description of each parameter follows:
%
%    o context: drawing context
*/
MagickExport double DrawGetFontSize(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  return CurrentContext->pointsize;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t F o n t S i z e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetFontSize() sets the font pointsize to use when annotating with text.
%
%  The format of the DrawSetFontSize method is:
%
%      void DrawSetFontSize(DrawContext context, const double pointsize)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o pointsize: text pointsize
%
*/
MagickExport void DrawSetFontSize(DrawContext context,
                                  const double pointsize)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->filter_off ||
     (AbsoluteValue(CurrentContext->pointsize-pointsize) > MagickEpsilon))
    {
      CurrentContext->pointsize=pointsize;

      (void) MvgPrintf(context, "font-size %.4g\n", pointsize);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t F o n t S t r e t c h                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetFontStretch() returns the font stretch used when annotating with text.
%
%  The format of the DrawGetFontStretch method is:
%
%      StretchType DrawGetFontStretch(DrawContext context)
%
%  A description of each parameter follows:
%
%    o context: drawing context
*/
MagickExport StretchType DrawGetFontStretch(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  return CurrentContext->stretch;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t F o n t S t r e t c h                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetFontStretch() sets the font stretch to use when annotating with text.
%  The AnyStretch enumeration acts as a wild-card "don't care" option.
%
%  The format of the DrawSetFontStretch method is:
%
%      void DrawSetFontStretch(DrawContext context,
%                              const StretchType font_stretch)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o font_stretch: font stretch (NormalStretch, UltraCondensedStretch,
%                    CondensedStretch, SemiCondensedStretch,
%                    SemiExpandedStretch, ExpandedStretch,
%                    ExtraExpandedStretch, UltraExpandedStretch, AnyStretch)
%
*/
MagickExport void DrawSetFontStretch(DrawContext context,
                                     const StretchType font_stretch)
{
  const char
    *p = NULL;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->filter_off || (CurrentContext->stretch != font_stretch))
    {
      CurrentContext->stretch=font_stretch;

      switch (font_stretch)
        {
        case NormalStretch:
          p = "normal";
          break;
        case UltraCondensedStretch:
          p = "ultra-condensed";
          break;
        case ExtraCondensedStretch:
          p = "extra-condensed";
          break;
        case CondensedStretch:
          p = "condensed";
          break;
        case SemiCondensedStretch:
          p = "semi-condensed";
          break;
        case SemiExpandedStretch:
          p = "semi-expanded";
          break;
        case ExpandedStretch:
          p = "expanded";
          break;
        case ExtraExpandedStretch:
          p = "extra-expanded";
          break;
        case UltraExpandedStretch:
          p = "ultra-expanded";
          break;
        case AnyStretch:
          p = "all";
          break;
        }

      if (p != NULL)
        (void) MvgPrintf(context, "font-stretch '%s'\n", p);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t F o n t S t y l e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetFontStyle() returns the font style used when annotating with text.
%
%  The format of the DrawGetFontStyle method is:
%
%      StyleType DrawGetFontStyle(DrawContext context)
%
%  A description of each parameter follows:
%
%    o context: drawing context
*/
MagickExport StyleType DrawGetFontStyle(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  return CurrentContext->style;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t F o n t S t y l e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetFontStyle() sets the font style to use when annotating with text.
%  The AnyStyle enumeration acts as a wild-card "don't care" option.
%
%  The format of the DrawSetFontStyle method is:
%
%      void DrawSetFontStyle(DrawContext context, const StyleType style)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o style: font style (NormalStyle, ItalicStyle, ObliqueStyle, AnyStyle)
%
*/
MagickExport void DrawSetFontStyle(DrawContext context,
                                   const StyleType style)
{
  const char
    *p = NULL;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->filter_off || (CurrentContext->style != style))
    {
      CurrentContext->style=style;

      switch (style)
        {
        case NormalStyle:
          p = "normal";
          break;
        case ItalicStyle:
          p = "italic";
          break;
        case ObliqueStyle:
          p = "oblique";
          break;
        case AnyStyle:
          p = "all";
          break;
        }

      if (p != NULL)
        (void) MvgPrintf(context, "font-style '%s'\n", p);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t F o n t W e i g h t                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetFontWeight() returns the font weight used when annotating with text.
%
%  The format of the DrawGetFontWeight method is:
%
%      unsigned long DrawGetFontWeight(DrawContext context)
%
%  A description of each parameter follows:
%
%    o context: drawing context
*/
MagickExport unsigned long DrawGetFontWeight(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  return CurrentContext->weight;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t F o n t W e i g h t                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetFontWeight() sets the font weight to use when annotating with text.
%
%  The format of the DrawSetFontWeight method is:
%
%      void DrawSetFontWeight(DrawContext context,
%                             const unsigned long font_weight)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o font_weight: font weight (valid range 100-900)
%
*/
MagickExport void DrawSetFontWeight(DrawContext context,
                                    const unsigned long font_weight)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->filter_off || (CurrentContext->weight != font_weight))
    {
      CurrentContext->weight=font_weight;
      (void) MvgPrintf(context, "font-weight %lu\n", font_weight);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t G r a v i t y                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetGravity() returns the text placement gravity used when annotating
%  with text.
%
%  The format of the DrawGetGravity method is:
%
%      GravityType DrawGetGravity(DrawContext context)
%
%  A description of each parameter follows:
%
%    o context: drawing context
*/
MagickExport GravityType DrawGetGravity(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  return CurrentContext->gravity;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t G r a v i t y                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetGravity() sets the text placement gravity to use when annotating
%  with text.
%
%  The format of the DrawSetGravity method is:
%
%      void DrawSetGravity(DrawContext context, const GravityType gravity)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o gravity: positioning gravity (NorthWestGravity, NorthGravity,
%               NorthEastGravity, WestGravity, CenterGravity,
%               EastGravity, SouthWestGravity, SouthGravity,
%               SouthEastGravity)
%
*/
MagickExport void DrawSetGravity(DrawContext context,
                                 const GravityType gravity)
{
  const char
    *p = NULL;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->filter_off || (CurrentContext->gravity != gravity))
    {
      CurrentContext->gravity=gravity;

      switch (gravity)
        {
        case NorthWestGravity:
          p = "NorthWest";
          break;
        case NorthGravity:
          p = "North";
          break;
        case NorthEastGravity:
          p = "NorthEast";
          break;
        case WestGravity:
          p = "West";
          break;
        case CenterGravity:
          p = "Center";
          break;
        case EastGravity:
          p = "East";
          break;
        case SouthWestGravity:
          p = "SouthWest";
          break;
        case SouthGravity:
          p = "South";
          break;
        case SouthEastGravity:
          p = "SouthEast";
          break;
        case StaticGravity:
        case ForgetGravity:
          {
          }
          break;
        }

      if (p != NULL)
        (void) MvgPrintf(context, "gravity %s\n", p);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w C o m p o s i t e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawComposite() composites an image onto the current image, using the
%  specified composition operator, specified position, and at the specified
%  size.
%
%  The format of the DrawComposite method is:
%
%      void DrawComposite(DrawContext context,
%                         const CompositeOperator composite_operator,
%                         const double x, const double y,
%                         const double width, const double height,
%                         const Image * image )
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o composite_operator: composition operator
%
%    o x: x ordinate of top left corner
%
%    o y: y ordinate of top left corner
%
%    o width: Width to resize image to prior to compositing.  Specify zero to
%             use existing width.
%
%    o height: Height to resize image to prior to compositing.  Specify zero
%             to use existing height.
%
%    o image: Image to composite
%
*/
MagickExport void DrawComposite(DrawContext context,
                                const CompositeOperator composite_operator,
                                const double x, const double y,
                                const double width, const double height,
                                const Image * image )

{
  ImageInfo
    *image_info;

  Image
    *clone_image;

  char
    *media_type = NULL,
    *base64 = NULL;

  const char
    *mode = NULL;

  unsigned char
    *blob = (unsigned char*)NULL;

  size_t
    blob_length = 2048,
    encoded_length = 0;

  MonitorHandler
    handler;

  assert(context != (DrawContext)NULL);
  assert(image != (Image *) NULL);
  assert(width != 0);
  assert(height != 0);
  assert(*image->magick != '\0');
  
/*   LogMagickEvent(CoderEvent,GetMagickModule(),"DrawComposite columns=%ld rows=%ld magick=%s ", */
/*                  image->columns, image->rows, image->magick ); */

  clone_image = CloneImage(image,0,0,True,&context->image->exception);
  if(!clone_image)
    return;

  image_info = CloneImageInfo((ImageInfo*)NULL);
  if(!image_info)
    ThrowDrawException3(ResourceLimitError,MemoryAllocationFailed,
      UnableToDrawOnImage);
  handler=SetMonitorHandler((MonitorHandler) NULL);
  blob = (unsigned char*)ImageToBlob( image_info, clone_image, &blob_length,
                                      &context->image->exception );
  (void) SetMonitorHandler(handler);
  DestroyImageInfo(image_info);
  DestroyImageList(clone_image);
  if(!blob)
    return;

  base64 = Base64Encode(blob,blob_length,&encoded_length);
  MagickFreeMemory(blob);
  if(!base64)
    {
      char
        buffer[MaxTextExtent];

      FormatString(buffer,"%ld bytes", (4L*blob_length/3L+4L));
      ThrowDrawException(ResourceLimitWarning,MemoryAllocationFailed,buffer)
    }

  mode = CompositeOperatorToString(composite_operator);
  media_type = MagickToMime( image->magick );

  if( media_type != NULL )
    {
      char
        *str;

      int
        remaining;

      (void) MvgPrintf(context, "image %s %.4g,%.4g %.4g,%.4g 'data:%s;base64,\n",
                mode, x, y, width, height, media_type);

      remaining = (int)encoded_length;
      str = base64;
      while( remaining > 0 )
        {
          (void) MvgPrintf(context,"%.76s", str);
          remaining -= 76;
          str += 76;
          if(remaining > 0)
            (void) MvgPrintf(context,"\n");
        }

      (void) MvgPrintf(context,"'\n");
    }

  MagickFreeMemory(base64);
  MagickFreeMemory(media_type);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w L i n e                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawLine() draws a line on the image using the current stroke color,
%  stroke opacity, and stroke width.
%
%  The format of the DrawLine method is:
%
%      void DrawLine(DrawContext context,
%                    const double sx, const double sy,
%                    const double ex, const double ey)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o sx: starting x ordinate
%
%    o sy: starting y ordinate
%
%    o ex: ending x ordinate
%
%    o ey: ending y ordinate
%
*/
MagickExport void DrawLine(DrawContext context,
                           const double sx, const double sy,
                           const double ex, const double ey)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  (void) MvgPrintf(context, "line %.4g,%.4g %.4g,%.4g\n", sx, sy, ex, ey);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w M a t t e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawMatte() paints on the image's opacity channel in order to set effected
%  pixels to transparent.  The available paint methods are:
%
%    PointMethod: Select the target pixel
%    ReplaceMethod: Select any pixel that matches the target pixel.
%    FloodfillMethod: Select the target pixel and matching neighbors.
%    FillToBorderMethod: Select the target pixel and neighbors not matching
%                        border color.
%    ResetMethod: Select all pixels.
%
%  The format of the DrawMatte method is:
%
%      void DrawMatte(DrawContext context,
%                     const double x, const double y,
%                     const PaintMethod paint_method)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o x: x ordinate
%
%    o y: y ordinate
%
%    o paint_method:
%
*/
MagickExport void DrawMatte(DrawContext context,
                            const double x, const double y,
                            const PaintMethod paint_method)
{
  const char
    *p = NULL;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  switch (paint_method)
    {
    case PointMethod:
      p = "point";
      break;
    case ReplaceMethod:
      p = "replace";
      break;
    case FloodfillMethod:
      p = "floodfill";
      break;
    case FillToBorderMethod:
      p = "filltoborder";
      break;
    case ResetMethod:
      p = "reset";
      break;
    }

  if (p != NULL)
    (void) MvgPrintf(context, "matte %.4g,%.4g %s\n", x, y, p);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h C l o s e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathClose() adds a path element to the current path which closes the
%  current subpath by drawing a straight line from the current point to the
%  current subpath's most recent starting point (usually, the most recent
%  moveto point).
%
%  The format of the DrawPathClose method is:
%
%      void DrawPathClose(DrawContext context)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
*/
MagickExport void DrawPathClose(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  (void) MvgAutoWrapPrintf(context, "%s", context->path_mode == AbsolutePathMode ? "Z" : "z");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h C u r v e T o A b s o l u t e                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathCurveToAbsolute() draws a cubic Bézier curve from the current
%  point to (x,y) using (x1,y1) as the control point at the beginning of
%  the curve and (x2,y2) as the control point at the end of the curve using
%  absolute coordinates. At the end of the command, the new current point
%  becomes the final (x,y) coordinate pair used in the polybezier.
%
%  The format of the DrawPathCurveToAbsolute method is:
%
%      void DrawPathCurveToAbsolute(DrawContext context,
%                                   const double x1, const double y1,
%                                   const double x2, const double y2,
%                                   const double x, const double y)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o x1: x ordinate of control point for curve beginning
%
%    o y1: y ordinate of control point for curve beginning
%
%    o x2: x ordinate of control point for curve ending
%
%    o y2: y ordinate of control point for curve ending
%
%    o x: x ordinate of the end of the curve
%
%    o y: y ordinate of the end of the curve
%
*/
static void DrawPathCurveTo(DrawContext context,
                            const PathMode mode,
                            const double x1, const double y1,
                            const double x2, const double y2,
                            const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if ((context->path_operation != PathCurveToOperation)
      || (context->path_mode != mode))
    {
      context->path_operation = PathCurveToOperation;
      context->path_mode = mode;
      (void) MvgAutoWrapPrintf(context, "%c%.4g,%.4g %.4g,%.4g %.4g,%.4g",
                        mode == AbsolutePathMode ? 'C' : 'c',
                        x1, y1, x2, y2, x, y);
    }
  else
    (void) MvgAutoWrapPrintf(context, " %.4g,%.4g %.4g,%.4g %.4g,%.4g",
                      x1, y1, x2, y2, x, y);
}
MagickExport void DrawPathCurveToAbsolute(DrawContext context,
                                          const double x1, const double y1,
                                          const double x2, const double y2,
                                          const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathCurveTo(context, AbsolutePathMode, x1, y1, x2, y2, x, y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h C u r v e T o R e l a t i v e                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathCurveToRelative() draws a cubic Bézier curve from the current
%  point to (x,y) using (x1,y1) as the control point at the beginning of
%  the curve and (x2,y2) as the control point at the end of the curve using
%  relative coordinates. At the end of the command, the new current point
%  becomes the final (x,y) coordinate pair used in the polybezier.
%
%  The format of the DrawPathCurveToRelative method is:
%
%      void DrawPathCurveToRelative(DrawContext context,
%                                   const double x1, const double y1,
%                                   const double x2, const double y2,
%                                   const double x, const double y)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o x1: x ordinate of control point for curve beginning
%
%    o y1: y ordinate of control point for curve beginning
%
%    o x2: x ordinate of control point for curve ending
%
%    o y2: y ordinate of control point for curve ending
%
%    o x: x ordinate of the end of the curve
%
%    o y: y ordinate of the end of the curve
%
*/
MagickExport void DrawPathCurveToRelative(DrawContext context,
                                          const double x1, const double y1,
                                          const double x2, const double y2,
                                          const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathCurveTo(context, RelativePathMode, x1, y1, x2, y2, x, y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h C u r v e T o Q u a d r a t i c B e z i e r A b s o l u t e %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathCurveToQuadraticBezierAbsolute() draws a quadratic Bézier curve
%  from the current point to (x,y) using (x1,y1) as the control point using
%  absolute coordinates. At the end of the command, the new current point
%  becomes the final (x,y) coordinate pair used in the polybezier.
%
%  The format of the DrawPathCurveToQuadraticBezierAbsolute method is:
%
%      void DrawPathCurveToQuadraticBezierAbsolute(DrawContext context,
%                                                  const double x1,
%                                                  const double y1,
%                                                  const double x,
%                                                  const double y)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o x1: x ordinate of the control point
%
%    o y1: y ordinate of the control point
%
%    o x: x ordinate of final point
%
%    o y: y ordinate of final point
%
*/
static void DrawPathCurveToQuadraticBezier(DrawContext context,
                                           const PathMode mode,
                                           const double x1, double y1,
                                           const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if ((context->path_operation != PathCurveToQuadraticBezierOperation)
      || (context->path_mode != mode))
    {
      context->path_operation = PathCurveToQuadraticBezierOperation;
      context->path_mode = mode;
      (void) MvgAutoWrapPrintf(context, "%c%.4g,%.4g %.4g,%.4g",
                        mode == AbsolutePathMode ? 'Q' : 'q', x1, y1, x, y);
    }
  else
    (void) MvgAutoWrapPrintf(context, " %.4g,%.4g %.4g,%.4g", x1, y1, x, y);
}
MagickExport void DrawPathCurveToQuadraticBezierAbsolute(DrawContext context,
                                                         const double x1,
                                                         const double y1,
                                                         const double x,
                                                         const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathCurveToQuadraticBezier(context, AbsolutePathMode, x1, y1, x, y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h C u r v e T o Q u a d r a t i c B e z i e r R e l a t i v e %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathCurveToQuadraticBezierRelative() draws a quadratic Bézier curve
%  from the current point to (x,y) using (x1,y1) as the control point using
%  relative coordinates. At the end of the command, the new current point
%  becomes the final (x,y) coordinate pair used in the polybezier.
%
%  The format of the DrawPathCurveToQuadraticBezierRelative method is:
%
%      void DrawPathCurveToQuadraticBezierRelative(DrawContext context,
%                                                  const double x1,
%                                                  const double y1,
%                                                  const double x,
%                                                  const double y)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o x1: x ordinate of the control point
%
%    o y1: y ordinate of the control point
%
%    o x: x ordinate of final point
%
%    o y: y ordinate of final point
%
*/
MagickExport void DrawPathCurveToQuadraticBezierRelative(DrawContext context,
                                                         const double x1,
                                                         const double y1,
                                                         const double x,
                                                         const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathCurveToQuadraticBezier(context, RelativePathMode, x1, y1, x, y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h C u r v e T o Q u a d r a t i c B e z i e r S m o o t h A b s o l u t e %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathCurveToQuadraticBezierSmoothAbsolute() draws a quadratic
%  Bézier curve (using absolute coordinates) from the current point to
%  (x,y). The control point is assumed to be the reflection of the
%  control point on the previous command relative to the current
%  point. (If there is no previous command or if the previous command was
%  not a DrawPathCurveToQuadraticBezierAbsolute,
%  DrawPathCurveToQuadraticBezierRelative,
%  DrawPathCurveToQuadraticBezierSmoothAbsolut or
%  DrawPathCurveToQuadraticBezierSmoothRelative, assume the control point
%  is coincident with the current point.). At the end of the command, the
%  new current point becomes the final (x,y) coordinate pair used in the
%  polybezier.
%
%  The format of the DrawPathCurveToQuadraticBezierSmoothAbsolute method is:
%
%      void DrawPathCurveToQuadraticBezierSmoothAbsolute(DrawContext
%                                                        context,
%                                                        const double x,
%                                                        const double y)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o x: x ordinate of final point
%
%    o y: y ordinate of final point
%
*/
static void DrawPathCurveToQuadraticBezierSmooth(DrawContext context,
                                                 const PathMode mode,
                                                 const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if ((context->path_operation != PathCurveToQuadraticBezierSmoothOperation)
      || (context->path_mode != mode))
    {
      context->path_operation = PathCurveToQuadraticBezierSmoothOperation;
      context->path_mode = mode;
      (void) MvgAutoWrapPrintf(context, "%c%.4g,%.4g",
                        mode == AbsolutePathMode ? 'T' : 't', x, y);
    }
  else
    (void) MvgAutoWrapPrintf(context, " %.4g,%.4g", x, y);
}
MagickExport void DrawPathCurveToQuadraticBezierSmoothAbsolute(DrawContext
                                                               context,
                                                               const double x,
                                                               const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathCurveToQuadraticBezierSmooth(context, AbsolutePathMode, x, y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h C u r v e T o Q u a d r a t i c B e z i e r S m o o t h R e l a t i v e %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathCurveToQuadraticBezierSmoothAbsolute() draws a quadratic
%  Bézier curve (using relative coordinates) from the current point to
%  (x,y). The control point is assumed to be the reflection of the
%  control point on the previous command relative to the current
%  point. (If there is no previous command or if the previous command was
%  not a DrawPathCurveToQuadraticBezierAbsolute,
%  DrawPathCurveToQuadraticBezierRelative,
%  DrawPathCurveToQuadraticBezierSmoothAbsolut or
%  DrawPathCurveToQuadraticBezierSmoothRelative, assume the control point
%  is coincident with the current point.). At the end of the command, the
%  new current point becomes the final (x,y) coordinate pair used in the
%  polybezier.
%
%  The format of the DrawPathCurveToQuadraticBezierSmoothRelative method is:
%
%      void DrawPathCurveToQuadraticBezierSmoothRelative(DrawContext
%                                                        context,
%                                                        const double x,
%                                                        const double y)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o x: x ordinate of final point
%
%    o y: y ordinate of final point
%
%
*/
MagickExport void DrawPathCurveToQuadraticBezierSmoothRelative(DrawContext
                                                               context,
                                                               const double x,
                                                               const double y)
{
  DrawPathCurveToQuadraticBezierSmooth(context, RelativePathMode, x, y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h C u r v e T o S m o o t h A b s o l u t e                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathCurveToSmoothAbsolute() draws a cubic Bézier curve from the
%  current point to (x,y) using absolute coordinates. The first control
%  point is assumed to be the reflection of the second control point on
%  the previous command relative to the current point. (If there is no
%  previous command or if the previous command was not an
%  DrawPathCurveToAbsolute, DrawPathCurveToRelative,
%  DrawPathCurveToSmoothAbsolute or DrawPathCurveToSmoothRelative, assume
%  the first control point is coincident with the current point.) (x2,y2)
%  is the second control point (i.e., the control point at the end of the
%  curve). At the end of the command, the new current point becomes the
%  final (x,y) coordinate pair used in the polybezier.
%
%  The format of the DrawPathCurveToSmoothAbsolute method is:
%
%      void DrawPathCurveToSmoothAbsolute(DrawContext context,
%                                         const double x2, const double y2,
%                                         const double x, const double y)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o x2: x ordinate of second control point
%
%    o y2: y ordinate of second control point
%
%    o x: x ordinate of termination point
%
%    o y: y ordinate of termination point
%
%
*/
static void DrawPathCurveToSmooth(DrawContext context, const PathMode mode,
                                  const double x2, const double y2,
                                  const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if ((context->path_operation != PathCurveToSmoothOperation)
      || (context->path_mode != mode))
    {
      context->path_operation = PathCurveToSmoothOperation;
      context->path_mode = mode;
      (void) MvgAutoWrapPrintf(context, "%c%.4g,%.4g %.4g,%.4g",
                        mode == AbsolutePathMode ? 'S' : 's', x2, y2, x, y);
    }
  else
    (void) MvgAutoWrapPrintf(context, " %.4g,%.4g %.4g,%.4g", x2, y2, x, y);
}
MagickExport void DrawPathCurveToSmoothAbsolute(DrawContext context,
                                                const double x2, const double y2,
                                                const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathCurveToSmooth(context, AbsolutePathMode, x2, y2, x, y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h C u r v e T o S m o o t h R e l a t i v e                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathCurveToSmoothRelative() draws a cubic Bézier curve from the
%  current point to (x,y) using relative coordinates. The first control
%  point is assumed to be the reflection of the second control point on
%  the previous command relative to the current point. (If there is no
%  previous command or if the previous command was not an
%  DrawPathCurveToAbsolute, DrawPathCurveToRelative,
%  DrawPathCurveToSmoothAbsolute or DrawPathCurveToSmoothRelative, assume
%  the first control point is coincident with the current point.) (x2,y2)
%  is the second control point (i.e., the control point at the end of the
%  curve). At the end of the command, the new current point becomes the
%  final (x,y) coordinate pair used in the polybezier.
%
%  The format of the DrawPathCurveToSmoothRelative method is:
%
%      void DrawPathCurveToSmoothRelative(DrawContext context,
%                                         const double x2, const double y2,
%                                         const double x, const double y)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o x2: x ordinate of second control point
%
%    o y2: y ordinate of second control point
%
%    o x: x ordinate of termination point
%
%    o y: y ordinate of termination point
%
%
*/
MagickExport void DrawPathCurveToSmoothRelative(DrawContext context,
                                                const double x2, const double y2,
                                                const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathCurveToSmooth(context, RelativePathMode, x2, y2, x, y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h E l l i p t i c A r c A b s o l u t e                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathEllipticArcAbsolute() draws an elliptical arc from the current
%  point to (x, y) using absolute coordinates. The size and orientation
%  of the ellipse are defined by two radii (rx, ry) and an
%  xAxisRotation, which indicates how the ellipse as a whole is rotated
%  relative to the current coordinate system. The center (cx, cy) of the
%  ellipse is calculated automatically to satisfy the constraints imposed
%  by the other parameters. largeArcFlag and sweepFlag contribute to the
%  automatic calculations and help determine how the arc is drawn. If
%  largeArcFlag is true then draw the larger of the available arcs. If
%  sweepFlag is true, then draw the arc matching a clock-wise rotation.
%
%  The format of the DrawPathEllipticArcAbsolute method is:
%
%      void DrawPathEllipticArcAbsolute(DrawContext context,
%                                       const double rx, const double ry,
%                                       const double x_axis_rotation,
%                                       unsigned int large_arc_flag,
%                                       unsigned int sweep_flag,
%                                       const double x, const double y)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o rx: x radius
%
%    o ry: y radius
%
%    o x_axis_rotation: indicates how the ellipse as a whole is rotated
%                       relative to the current coordinate system
%
%    o large_arc_flag: If non-zero (true) then draw the larger of the
%                      available arcs
%
%    o sweep_flag: If non-zero (true) then draw the arc matching a
%                  clock-wise rotation
%
%
*/
static void DrawPathEllipticArc(DrawContext context, const PathMode mode,
                                const double rx, const double ry,
                                const double x_axis_rotation,
                                unsigned int large_arc_flag,
                                unsigned int sweep_flag,
                                const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if ((context->path_operation != PathEllipticArcOperation)
      || (context->path_mode != mode))
    {
      context->path_operation = PathEllipticArcOperation;
      context->path_mode = mode;
      (void) MvgAutoWrapPrintf(context, "%c%.4g,%.4g %.4g %u %u %.4g,%.4g",
                        mode == AbsolutePathMode ? 'A' : 'a', rx, ry, x_axis_rotation,
                        large_arc_flag, sweep_flag, x, y);
    }
  else
    (void) MvgAutoWrapPrintf(context, " %.4g,%.4g %.4g %u %u %.4g,%.4g", rx, ry,
                      x_axis_rotation, large_arc_flag, sweep_flag, x, y);
}
MagickExport void DrawPathEllipticArcAbsolute(DrawContext context,
                                              const double rx, const double ry,
                                              const double x_axis_rotation,
                                              unsigned int large_arc_flag,
                                              unsigned int sweep_flag,
                                              const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathEllipticArc(context, AbsolutePathMode, rx, ry, x_axis_rotation,
                      large_arc_flag, sweep_flag, x, y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h E l l i p t i c A r c R e l a t i v e                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathEllipticArcRelative() draws an elliptical arc from the current
%  point to (x, y) using relative coordinates. The size and orientation
%  of the ellipse are defined by two radii (rx, ry) and an
%  xAxisRotation, which indicates how the ellipse as a whole is rotated
%  relative to the current coordinate system. The center (cx, cy) of the
%  ellipse is calculated automatically to satisfy the constraints imposed
%  by the other parameters. largeArcFlag and sweepFlag contribute to the
%  automatic calculations and help determine how the arc is drawn. If
%  largeArcFlag is true then draw the larger of the available arcs. If
%  sweepFlag is true, then draw the arc matching a clock-wise rotation.
%
%  The format of the DrawPathEllipticArcRelative method is:
%
%      void DrawPathEllipticArcRelative(DrawContext context,
%                                       const double rx, const double ry,
%                                       const double x_axis_rotation,
%                                       unsigned int large_arc_flag,
%                                       unsigned int sweep_flag,
%                                       const double x, const double y)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o rx: x radius
%
%    o ry: y radius
%
%    o x_axis_rotation: indicates how the ellipse as a whole is rotated
%                       relative to the current coordinate system
%
%    o large_arc_flag: If non-zero (true) then draw the larger of the
%                      available arcs
%
%    o sweep_flag: If non-zero (true) then draw the arc matching a
%                  clock-wise rotation
%
*/
MagickExport void DrawPathEllipticArcRelative(DrawContext context,
                                              const double rx, const double ry,
                                              const double x_axis_rotation,
                                              unsigned int large_arc_flag,
                                              unsigned int sweep_flag,
                                              const double x, const double y)
{
  DrawPathEllipticArc(context, RelativePathMode, rx, ry, x_axis_rotation,
                      large_arc_flag, sweep_flag, x, y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h F i n i s h                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathFinish() terminates the current path.
%
%  The format of the DrawPathFinish method is:
%
%      void DrawPathFinish(DrawContext context)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
*/
MagickExport void DrawPathFinish(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  (void) MvgPrintf(context, "'\n");
  context->path_operation = PathDefaultOperation;
  context->path_mode = DefaultPathMode;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h L i n e T o A b s o l u t e                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathLineToAbsolute() draws a line path from the current point to the
%  given coordinate using absolute coordinates. The coordinate then becomes
%  the new current point.
%
%  The format of the DrawPathLineToAbsolute method is:
%
%      void DrawPathLineToAbsolute(DrawContext context,
%                                  const double x, const double y)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o x: target x ordinate
%
%    o y: target y ordinate
%
*/
static void DrawPathLineTo(DrawContext context,
                           const PathMode mode,
                           const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if ((context->path_operation != PathLineToOperation)
      || (context->path_mode != mode))
    {
      context->path_operation = PathLineToOperation;
      context->path_mode = mode;
      (void) MvgAutoWrapPrintf(context, "%c%.4g,%.4g",
                        mode == AbsolutePathMode ? 'L' : 'l', x, y);
    }
  else
    (void) MvgAutoWrapPrintf(context, " %.4g,%.4g", x, y);
}
MagickExport void DrawPathLineToAbsolute(DrawContext context,
                                         const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathLineTo(context, AbsolutePathMode, x, y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h L i n e T o R e l a t i v e                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathLineToRelative() draws a line path from the current point to the
%  given coordinate using relative coordinates. The coordinate then becomes
%  the new current point.
%
%  The format of the DrawPathLineToRelative method is:
%
%      void DrawPathLineToRelative(DrawContext context,
%                                  const double x, const double y)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o x: target x ordinate
%
%    o y: target y ordinate
%
*/
MagickExport void DrawPathLineToRelative(DrawContext context,
                                         const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathLineTo(context, RelativePathMode, x, y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h L i n e T o H o r i z o n t a l A b s o l u t e           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathLineToHorizontalAbsolute() draws a horizontal line path from the
%  current point to the target point using absolute coordinates.  The target
%  point then becomes the new current point.
%
%  The format of the DrawPathLineToHorizontalAbsolute method is:
%
%      void DrawPathLineToHorizontalAbsolute(DrawContext context,
%                                            const double x)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o x: target x ordinate
%
*/

static void DrawPathLineToHorizontal(DrawContext context,
                                     const PathMode mode, const double x)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if ((context->path_operation != PathLineToHorizontalOperation)
      || (context->path_mode != mode))
    {
      context->path_operation = PathLineToHorizontalOperation;
      context->path_mode = mode;
      (void) MvgAutoWrapPrintf(context, "%c%.4g",
                        mode == AbsolutePathMode ? 'H' : 'h', x);
    }
  else
    (void) MvgAutoWrapPrintf(context, " %.4g", x);
}
MagickExport void DrawPathLineToHorizontalAbsolute(DrawContext context,
                                                   const double x)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathLineToHorizontal(context, AbsolutePathMode, x);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h L i n e T o H o r i z o n t a l R e l a t i v e           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathLineToHorizontalRelative() draws a horizontal line path from the
%  current point to the target point using relative coordinates.  The target
%  point then becomes the new current point.
%
%  The format of the DrawPathLineToHorizontalRelative method is:
%
%      void DrawPathLineToHorizontalRelative(DrawContext context, const double x)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o x: target x ordinate
%
*/
MagickExport void DrawPathLineToHorizontalRelative(DrawContext context,
                                                   const double x)
{
  DrawPathLineToHorizontal(context, RelativePathMode, x);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h L i n e T o V e r t i c a l A b s o l u t e               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathLineToVerticalAbsolute() draws a vertical line path from the
%  current point to the target point using absolute coordinates.  The target
%  point then becomes the new current point.
%
%  The format of the DrawPathLineToVerticalAbsolute method is:
%
%      void DrawPathLineToVerticalAbsolute(DrawContext context,
%                                          const double y)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o y: target y ordinate
%
*/
static void DrawPathLineToVertical(DrawContext context, const PathMode mode,
                                   const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if ((context->path_operation != PathLineToVerticalOperation)
      || (context->path_mode != mode))
    {
      context->path_operation = PathLineToVerticalOperation;
      context->path_mode = mode;
      (void) MvgAutoWrapPrintf(context, "%c%.4g",
                        mode == AbsolutePathMode ? 'V' : 'v', y);
    }
  else
    (void) MvgAutoWrapPrintf(context, " %.4g", y);
}
MagickExport void DrawPathLineToVerticalAbsolute(DrawContext context,
                                                 const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathLineToVertical(context, AbsolutePathMode, y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h L i n e T o V e r t i c a l R e l a t i v e               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathLineToVerticalRelative() draws a vertical line path from the
%  current point to the target point using relative coordinates.  The target
%  point then becomes the new current point.
%
%  The format of the DrawPathLineToVerticalRelative method is:
%
%      void DrawPathLineToVerticalRelative(DrawContext context,
%                                          const double y)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o y: target y ordinate
%
*/
MagickExport void DrawPathLineToVerticalRelative(DrawContext context,
                                                 const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathLineToVertical(context, RelativePathMode, y);
}
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h M o v e T o A b s o l u t e                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathMoveToAbsolute() starts a new sub-path at the given coordinate
%  using absolute coordinates. The current point then becomes the
%  specified coordinate.
%
%  The format of the DrawPathMoveToAbsolute method is:
%
%      void DrawPathMoveToAbsolute(DrawContext context, const double x,
%                                  const double y)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o x: target x ordinate
%
%    o y: target y ordinate
%
*/
 static void DrawPathMoveTo(DrawContext context, const PathMode mode,
                           const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if ((context->path_operation != PathMoveToOperation)
      || (context->path_mode != mode))
    {
      context->path_operation = PathMoveToOperation;
      context->path_mode = mode;
      (void) MvgAutoWrapPrintf(context, "%c%.4g,%.4g",
                        mode == AbsolutePathMode ? 'M' : 'm', x, y);
    }
  else
    (void) MvgAutoWrapPrintf(context, " %.4g,%.4g", x, y);
}

MagickExport void DrawPathMoveToAbsolute(DrawContext context, const double x,
                                         const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathMoveTo(context, AbsolutePathMode, x, y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h M o v e T o R e l a t i v e                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathMoveToRelative() starts a new sub-path at the given coordinate
%  using relative coordinates. The current point then becomes the
%  specified coordinate.
%
%  The format of the DrawPathMoveToRelative method is:
%
%      void DrawPathMoveToRelative(DrawContext context,
%                                  const double x, const double y)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o x: target x ordinate
%
%    o y: target y ordinate
%
*/
MagickExport void DrawPathMoveToRelative(DrawContext context,
                                         const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathMoveTo(context, RelativePathMode, x, y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t h S t a r t                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPathStart() declares the start of a path drawing list which is terminated
%  by a matching DrawPathFinish() command. All other DrawPath commands must
%  be enclosed between a DrawPathStart() and a DrawPathFinish() command. This
%  is because path drawing commands are subordinate commands and they do not
%  function by themselves.
%
%  The format of the DrawPathStart method is:
%
%      void DrawPathStart(DrawContext context)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
*/
MagickExport void DrawPathStart(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  (void) MvgPrintf(context, "path '");
  context->path_operation = PathDefaultOperation;
  context->path_mode = DefaultPathMode;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P e e k G r a p h i c C o n t e x t                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPeekGraphicContext() returns a copy of the the DrawInfo structure at
%  the head of the drawing context stack. The user is responsible for
%  deallocating the returned object using DestroyDrawInfo.
%
%  The format of the DrawPeekGraphicContext method is:
%
%      DrawInfo *DrawPeekGraphicContext(const DrawContext context)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
*/
MagickExport DrawInfo *DrawPeekGraphicContext(const DrawContext context)
{
  DrawInfo
    *draw_info;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  draw_info=CloneDrawInfo((ImageInfo *) NULL,CurrentContext);
  (void) CloneString(&draw_info->primitive,context->mvg);
  CurrentContext->primitive=context->mvg;
  return(draw_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P o i n t                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPoint() draws a point using the current stroke color and stroke
%  thickness at the specified coordinates.
%
%  The format of the DrawPoint method is:
%
%      void DrawPoint(DrawContext context, const double x, const double y)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o x: target x coordinate
%
%    o y: target y coordinate
%
*/
MagickExport void DrawPoint(DrawContext context,
                            const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  (void) MvgPrintf(context, "point %.4g,%.4g\n", x, y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P o l y g o n                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPolygon() draws a polygon using the current stroke, stroke width, and
%  fill color or texture, using the specified array of coordinates.
%
%  The format of the DrawPolygon method is:
%
%      void DrawPolygon(DrawContext context,
%                       const unsigned long num_coords,
%                       const PointInfo * coordinates)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o num_coords: number of coordinates
%
%    o coordinates: coordinate array
%
*/
MagickExport void DrawPolygon(DrawContext context,
                              const unsigned long num_coords,
                              const PointInfo * coordinates)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  MvgAppendPointsCommand(context,"polygon",num_coords,coordinates);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P o l y l i n e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPolyline() draws a polyline using the current stroke, stroke width, and
%  fill color or texture, using the specified array of coordinates.
%
%  The format of the DrawPolyline method is:
%
%      void DrawPolyline(DrawContext context,
%                        const unsigned long num_coords,
%                        const PointInfo * coordinates)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o num_coords: number of coordinates
%
%    o coordinates: coordinate array
%
*/
MagickExport void DrawPolyline(DrawContext context,
                               const unsigned long num_coords,
                               const PointInfo * coordinates)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  MvgAppendPointsCommand(context,"polyline",num_coords,coordinates);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P o p C l i p P a t h                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPopClipPath() terminates a clip path definition.
%
%  The format of the DrawPopClipPath method is:
%
%      void DrawPopClipPath(DrawContext context)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
*/
MagickExport void DrawPopClipPath(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->indent_depth != 0)
    context->indent_depth--;
  (void) MvgPrintf(context, "pop clip-path\n");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P o p D e f s                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPopDefs() terminates a definition list
%
%  The format of the DrawPopDefs method is:
%
%      void DrawPopDefs(DrawContext context)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
*/
MagickExport void DrawPopDefs(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->indent_depth != 0)
    context->indent_depth--;
  (void) MvgPrintf(context, "pop defs\n");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P o p G r a p h i c C o n t e x t                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPopGraphicContext() destroys the current context returning to the
%  previously pushed context. Multiple contexts may exist. It is an error
%  to attempt to pop more contexts than have been pushed, and it is proper
%  form to pop all contexts which have been pushed.
%
%  The format of the DrawPopGraphicContext method is:
%
%      void DrawPopGraphicContext(DrawContext context)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
*/
MagickExport void DrawPopGraphicContext(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->index != 0)
    {
      /* Destroy clip path if not same in preceding context */
#if DRAW_BINARY_IMPLEMENTATION
      if (CurrentContext->clip_path != (char *) NULL)
        if (LocaleCompare(CurrentContext->clip_path,
                          context->graphic_context[context->index-1]->clip_path) != 0)
          (void) SetImageClipMask(context->image,(Image *) NULL);
#endif

      DestroyDrawInfo(CurrentContext);
      CurrentContext=(DrawInfo*)NULL;
      context->index--;

      if(context->indent_depth != 0)
        context->indent_depth--;
      (void) MvgPrintf(context, "pop graphic-context\n");
    }
  else
    {
      ThrowDrawException(DrawError,UnbalancedGraphicContextPushPop,NULL)
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P o p P a t t e r n                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPopPattern() terminates a pattern definition.
%
%  The format of the DrawPopPattern method is:
%
%      void DrawPopPattern(DrawContext context)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
*/
MagickExport void DrawPopPattern(DrawContext context)
{
  char
    geometry[MaxTextExtent],
    key[MaxTextExtent];

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if( context->pattern_id == NULL )
    ThrowDrawException(DrawWarning,NotCurrentlyPushingPatternDefinition,NULL);

  FormatString(key,"[%.1024s]",context->pattern_id);

  (void) SetImageAttribute(context->image,key,context->mvg+context->pattern_offset);
  FormatString(geometry,"%lux%lu%+ld%+ld",
               context->pattern_bounds.width,context->pattern_bounds.height,
               context->pattern_bounds.x,context->pattern_bounds.y);
  (void) SetImageAttribute(context->image,key,geometry);

  MagickFreeMemory(context->pattern_id);
  context->pattern_offset = 0;

  context->pattern_bounds.x = 0;
  context->pattern_bounds.y = 0;
  context->pattern_bounds.width = 0;
  context->pattern_bounds.height = 0;

  context->filter_off = False;

  if(context->indent_depth != 0)
    context->indent_depth--;
  (void) MvgPrintf(context, "pop pattern\n");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P u s h C l i p P a t h                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPushClipPath() starts a clip path definition which is comprized of
%  any number of drawing commands and terminated by a DrawPopClipPath()
%  command.
%
%  The format of the DrawPushClipPath method is:
%
%      void DrawPushClipPath(DrawContext context, const char *clip_path_id)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o clip_path_id: string identifier to associate with the clip path for
%      later use.
%
*/
MagickExport void DrawPushClipPath(DrawContext context,
                                   const char *clip_path_id)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(clip_path_id != (const char *) NULL);

  (void) MvgPrintf(context, "push clip-path %s\n", clip_path_id);
  context->indent_depth++;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P u s h D e f s                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPushDefs() indicates that commands up to a terminating DrawPopDefs()
%  command create named elements (e.g. clip-paths, textures, etc.) which
%  may safely be processed earlier for the sake of efficiency.
%
%  The format of the DrawPushDefs method is:
%
%      void DrawPushDefs(DrawContext context)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
*/
MagickExport void DrawPushDefs(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  (void) MvgPrintf(context, "push defs\n");
  context->indent_depth++;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P u s h G r a p h i c C o n t e x t                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPushGraphicContext() clones the current drawing context to create a
%  new drawing context. The original drawing context(s) may be returned to
%  by invoking DrawPopGraphicContext().  The contexts are stored on a context
%  stack.  For every Pop there must have already been an equivalent Push.
%
%  The format of the DrawPushGraphicContext method is:
%
%      void DrawPushGraphicContext(DrawContext context)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
*/
MagickExport void DrawPushGraphicContext(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  context->index++;
  MagickReallocMemory(DrawInfo **,context->graphic_context,
                  (context->index+1)*sizeof(DrawInfo *));
  if (context->graphic_context == (DrawInfo **) NULL)
    {
      ThrowDrawException3(ResourceLimitError,MemoryAllocationFailed,
        UnableToDrawOnImage)
    }
  CurrentContext=
    CloneDrawInfo((ImageInfo *) NULL,context->graphic_context[context->index-1]);
  (void) MvgPrintf(context, "push graphic-context\n");
  context->indent_depth++;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P u s h P a t t e r n                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPushPattern() indicates that subsequent commands up to a
%  DrawPopPattern() command comprise the definition of a named pattern.
%  The pattern space is assigned top left corner coordinates, a width
%  and height, and becomes its own drawing space.  Anything which can
%  be drawn may be used in a pattern definition.
%  Named patterns may be used as stroke or brush definitions.
%
%  The format of the DrawPushPattern method is:
%
%      void DrawPushPattern(DrawContext context,
%                           const char *pattern_id,
%                           const double x, const double y,
%                           const double width, const double height)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o pattern_id: pattern identification for later reference
%
%    o x: x ordinate of top left corner
%
%    o y: y ordinate of top left corner
%
%    o width: width of pattern space
%
%    o height: height of pattern space
%
*/
MagickExport void DrawPushPattern(DrawContext context,
                                  const char *pattern_id,
                                  const double x, const double y,
                                  const double width, const double height)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(pattern_id != (const char *) NULL);

  if( context->pattern_id != NULL )
    ThrowDrawException(DrawError,AlreadyPushingPatternDefinition,
      context->pattern_id);

  context->filter_off = True;

  (void) MvgPrintf(context, "push pattern %s %.4g,%.4g %.4g,%.4g\n",
            pattern_id, x, y, width, height);
  context->indent_depth++;

  /* Record current pattern ID, bounds, and start position in MVG */
  context->pattern_id = AllocateString(pattern_id);
  context->pattern_bounds.x = (long) ceil(x-0.5);
  context->pattern_bounds.y = (long) ceil(y-0.5);
  context->pattern_bounds.width = (unsigned long) floor(width+0.5);
  context->pattern_bounds.height = (unsigned long) floor(height+0.5);
  context->pattern_offset = context->mvg_length;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w R e c t a n g l e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawRectangle() draws a rectangle given two coordinates and using
%  the current stroke, stroke width, and fill settings.
%
%  The format of the DrawRectangle method is:
%
%      void DrawRectangle(DrawContext context,
%                         const double x1, const double y1,
%                         const double x2, const double y2)
%
%  A description of each parameter follows:
%
%    o x1: x ordinate of first coordinate
%
%    o y1: y ordinate of first coordinate
%
%    o x2: x ordinate of second coordinate
%
%    o y2: y ordinate of second coordinate
%
*/
MagickExport void DrawRectangle(DrawContext context,
                                const double x1, const double y1,
                                const double x2, const double y2)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  (void) MvgPrintf(context, "rectangle %.4g,%.4g %.4g,%.4g\n", x1, y1, x2, y2);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w R e n d e r                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawRender() renders all preceding drawing commands onto the image.
%
%  The format of the DrawRender method is:
%
%      int DrawRender(const DrawContext context)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
*/
MagickExport int DrawRender(const DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  CurrentContext->primitive = context->mvg;
  (void) LogMagickEvent(RenderEvent,GetMagickModule(),"MVG:\n'%s'\n",context->mvg);
  (void) DrawImage(context->image, CurrentContext);
  CurrentContext->primitive = (char *) NULL;

  return True;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w R o t a t e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawRotate() applies the specified rotation to the current coordinate
%  space.
%
%  The format of the DrawRotate method is:
%
%      void DrawRotate(DrawContext context, const double degrees)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o degrees: degrees of rotation
%
*/
MagickExport void DrawRotate(DrawContext context, const double degrees)
{
  AffineMatrix
    affine;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  IdentityAffine(&affine);
  affine.sx=cos(DegreesToRadians(fmod(degrees,360.0)));
  affine.rx=sin(DegreesToRadians(fmod(degrees,360.0)));
  affine.ry=(-sin(DegreesToRadians(fmod(degrees,360.0))));
  affine.sy=cos(DegreesToRadians(fmod(degrees,360.0)));
  AdjustAffine( context, &affine );

  (void) MvgPrintf(context, "rotate %.4g\n", degrees);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w R o u n d R e c t a n g l e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawRoundRectangle() draws a rounted rectangle given two coordinates,
%  x & y corner radiuses and using the current stroke, stroke width,
%  and fill settings.
%
%  The format of the DrawRoundRectangle method is:
%
%      void DrawRoundRectangle(DrawContext context,
%                              double x1, double y1,
%                              double x2, double y2,
%                              double rx, double ry)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o x1: x ordinate of first coordinate
%
%    o y1: y ordinate of first coordinate
%
%    o x2: x ordinate of second coordinate
%
%    o y2: y ordinate of second coordinate
%
%    o rx: radius of corner in horizontal direction
%
%    o ry: radius of corner in vertical direction
%
*/
MagickExport void DrawRoundRectangle(DrawContext context,
                                     double x1, double y1,
                                     double x2, double y2,
                                     double rx, double ry)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  (void) MvgPrintf(context, "roundrectangle %.4g,%.4g %.4g,%.4g %.4g,%.4g\n",
            x1, y1, x2, y2, rx, ry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S c a l e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawScale() adjusts the scaling factor to apply in the horizontal and
%  vertical directions to the current coordinate space.
%
%  The format of the DrawScale method is:
%
%      void DrawScale(DrawContext context, const double x, const double y)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o x: horizontal scale factor
%
%    o y: vertical scale factor
%
*/
MagickExport void DrawScale(DrawContext context,
                               const double x, const double y)
{
  AffineMatrix
    affine;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  IdentityAffine(&affine);
  affine.sx=x;
  affine.sy=y;
  AdjustAffine( context, &affine );

  (void) MvgPrintf(context, "scale %.4g,%.4g\n", x, y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S k e w X                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSkewX() skews the current coordinate system in the horizontal
%  direction.
%
%  The format of the DrawSkewX method is:
%
%      void DrawSkewX(DrawContext context, const double degrees)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o degrees: number of degrees to skew the coordinates
%
*/
MagickExport void DrawSkewX(DrawContext context, const double degrees)
{
  AffineMatrix
    affine;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  IdentityAffine(&affine);
  affine.ry=tan(DegreesToRadians(fmod(degrees,360.0)));
  AdjustAffine(context,&affine);

  (void) MvgPrintf(context, "skewX %.4g\n", degrees);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S k e w Y                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSkewY() skews the current coordinate system in the vertical
%  direction.
%
%  The format of the DrawSkewY method is:
%
%      void DrawSkewY(DrawContext context, const double degrees)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o degrees: number of degrees to skew the coordinates
%
*/
MagickExport void DrawSkewY(DrawContext context, const double degrees)
{
  AffineMatrix
    affine;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  IdentityAffine(&affine);
  affine.rx=tan(DegreesToRadians(fmod(degrees,360.0)));
  DrawAffine(context,&affine);

  (void) MvgPrintf(context, "skewY %.4g\n", degrees);
}

#if 0

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t S t o p C o l o r                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetStopColor() sets the stop color and offset for gradients
%
%  The format of the DrawSetStopColor method is:
%
%      void DrawSetStopColor(DrawContext context,
%                            const PixelPacket * stop_color,
%                            const double offset)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o stop_color:
%
%    o offset:
%
*/
/* This is gradient stuff so it shouldn't be supported yet */
MagickExport void DrawSetStopColor(DrawContext context,
                                   const PixelPacket * stop_color,
                                   const double offset)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(stop_color != (const PixelPacket *) NULL);


  (void) MvgPrintf(context, "stop-color ");
  MvgAppendColor(context, stop_color);
  (void) MvgPrintf(context, "\n");
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t S t r o k e C o l o r                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetStrokeColor() returns the color used for stroking object outlines.
%
%  The format of the DrawGetStrokeColor method is:
%
%      PixelPacket DrawGetStrokeColor(DrawContext context)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
*/
MagickExport PixelPacket DrawGetStrokeColor(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  return CurrentContext->stroke;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t S t r o k e C o l o r                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetStrokeColor() sets the color used for stroking object outlines.
%
%  The format of the DrawSetStrokeColor method is:
%
%      void DrawSetStrokeColor(DrawContext context,
%                              const PixelPacket * stroke_color)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o stroke_color: stroke color
%
*/
MagickExport void DrawSetStrokeColor(DrawContext context,
                                     const PixelPacket * stroke_color)
{
  PixelPacket
    *current_stroke,
    new_stroke;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(stroke_color != (const PixelPacket *) NULL);

  new_stroke = *stroke_color;

  /* Inherit base opacity */
  if(new_stroke.opacity == OpaqueOpacity)
    new_stroke.opacity = CurrentContext->opacity;

  current_stroke = &CurrentContext->stroke;
  if( context->filter_off || !(PixelPacketMatch(current_stroke,&new_stroke)) )
    {
      CurrentContext->stroke = new_stroke;

      (void) MvgPrintf(context, "stroke '");
      MvgAppendColor(context, stroke_color);
      (void) MvgPrintf(context, "'\n");
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t S t r o k e C o l o r S t r i n g                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetStrokeColorString() sets the color used for stroking object outlines.
%
%  The format of the DrawSetStrokeColorString method is:
%
%      void DrawSetStrokeColorString(DrawContext context,
%                                    const char* stroke_color)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o stroke_color: stroke color
%
*/
MagickExport void DrawSetStrokeColorString(DrawContext context,
                                           const char* stroke_color)
{
  PixelPacket
    pixel_packet;

  if(QueryColorDatabase(stroke_color,&pixel_packet,&context->image->exception))
    DrawSetStrokeColor(context,&pixel_packet);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t S t r o k e P a t t e r n U R L                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetStrokePatternURL() sets the pattern used for stroking object outlines.
%
%  The format of the DrawSetStrokePatternURL method is:
%
%      void DrawSetStrokePatternURL(DrawContext context, const char* stroke_url)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o stroke_url: URL specifying pattern ID (e.g. "#pattern_id")
%
*/
MagickExport void DrawSetStrokePatternURL(DrawContext context,
                                          const char* stroke_url)
{
  char
    pattern[MaxTextExtent];

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(stroke_url != NULL);

  if(stroke_url[0] != '#')
    ThrowDrawException(DrawWarning, NotARelativeURL, stroke_url);

  FormatString(pattern,"[%.1024s]",stroke_url+1);

  if (GetImageAttribute(context->image,pattern) == (ImageAttribute *) NULL)
    {
      ThrowDrawException(DrawWarning, URLNotFound, stroke_url)
    }
  else
    {
      char
        pattern_spec[MaxTextExtent];

      FormatString(pattern_spec,"url(%.1024s)",stroke_url);
#if DRAW_BINARY_IMPLEMENTATION
      DrawPatternPath(context->image,CurrentContext,pattern_spec,&CurrentContext->stroke_pattern);
#endif
      /* Inherit base opacity */
      if (CurrentContext->stroke.opacity == OpaqueOpacity)
        CurrentContext->stroke.opacity=CurrentContext->opacity;

      (void) MvgPrintf(context, "stroke %s\n",pattern_spec);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t S t r o k e A n t i a l i a s                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetStrokeAntialias() returns the current stroke antialias setting.
%  Stroked outlines are antialiased by default.  When antialiasing is disabled
%  stroked pixels are thresholded to determine if the stroke color or
%  underlying canvas color should be used.
%
%  The format of the DrawGetStrokeAntialias method is:
%
%      unsigned int DrawGetStrokeAntialias(DrawContext context)
%
%  A description of each parameter follows:
%
%    o context: drawing context
*/
MagickExport unsigned int DrawGetStrokeAntialias(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  return CurrentContext->stroke_antialias;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t S t r o k e A n t i a l i a s                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetStrokeAntialias() controls whether stroked outlines are antialiased.
%  Stroked outlines are antialiased by default.  When antialiasing is disabled
%  stroked pixels are thresholded to determine if the stroke color or
%  underlying canvas color should be used.
%
%  The format of the DrawSetStrokeAntialias method is:
%
%      void DrawSetStrokeAntialias(DrawContext context,
%                                  const unsigned int stroke_antialias)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o stroke_antialias: set to false (zero) to disable antialiasing
%
*/
MagickExport void DrawSetStrokeAntialias(DrawContext context,
                                         const unsigned int stroke_antialias)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->filter_off || (CurrentContext->stroke_antialias != stroke_antialias))
    {
      CurrentContext->stroke_antialias = stroke_antialias;

      (void) MvgPrintf(context, "stroke-antialias %i\n", stroke_antialias ? 1 : 0);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t S t r o k e D a s h A r r a y                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetStrokeDashArray() returns an array representing the pattern of
%  dashes and gaps used to stroke paths (see DrawSetStrokeDashArray). The
%  array must be freed once it is no longer required by the user.
%
%  The format of the DrawGetStrokeDashArray method is:
%
%      double *DrawGetStrokeDashArray(DrawContext context,unsigned long *num_elems)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o num_elems: address to place number of elements in dash array
%
% */
MagickExport double *DrawGetStrokeDashArray(DrawContext context,
                                            unsigned long *num_elems)
{
  register const double
    *p;

  register double
    *q;

  double
    *dasharray;

  unsigned int
    i,
    n = 0;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(num_elems != (unsigned long *)NULL);

  p = CurrentContext->dash_pattern;
  if( p != (const double *) NULL )
    while( *p++ != 0)
      n++;

  *num_elems = n;
  dasharray = (double *)NULL;
  if (n != 0)
    {
      dasharray = MagickAllocateMemory(double *, n*sizeof(double));
      p = CurrentContext->dash_pattern;
      q = dasharray;
      i = n;
      while( i-- )
        *q++ = *p++;
    }
  return dasharray;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t S t r o k e D a s h A r r a y                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetStrokeDashArray() specifies the pattern of dashes and gaps used to
%  stroke paths. The strokeDashArray represents an array of numbers that
%  specify the lengths of alternating dashes and gaps in pixels. If an odd
%  number of values is provided, then the list of values is repeated to yield
%  an even number of values. To remove an existing dash array, pass a zero
%  num_elems argument and null dasharray. A typical stroke dash array might
%  contain the members 5 3 2.
%
%  The format of the DrawSetStrokeDashArray method is:
%
%      void DrawSetStrokeDashArray(DrawContext context,
%                                  const unsigned long num_elems,
%                                  const double *dasharray)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o num_elems: number of elements in dash array
%
%    o dasharray: dash array values
%
% */
MagickExport void DrawSetStrokeDashArray(DrawContext context,
                                         const unsigned long num_elems,
                                         const double *dasharray)
{
  register const double
    *p;

  register double
    *q;

  unsigned int
    i,
    updated = False,
    n_new = num_elems,
    n_old = 0;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  q = CurrentContext->dash_pattern;
  if( q != (const double *) NULL )
    while( *q++ != 0)
      n_old++;

  if( (n_old == 0) && (n_new == 0) )
    {
      updated = False;
    }
  else if( n_old != n_new )
    {
      updated = True;
    }
  else if((CurrentContext->dash_pattern != (double*)NULL)
          && (dasharray != (double*)NULL))
    {
      p = dasharray;
      q = CurrentContext->dash_pattern;
      i = n_new;
      while( i-- )
        {
          if(AbsoluteValue(*p - *q) > MagickEpsilon)
            {
              updated = True;
              break;
            }
          ++p;
          ++q;
        }
    }

  if( context->filter_off || updated )
    {
      if(CurrentContext->dash_pattern != (double*)NULL)
        MagickFreeMemory(CurrentContext->dash_pattern);

      if( n_new != 0)
        {
          CurrentContext->dash_pattern = MagickAllocateMemory(double *,
            (n_new+1)*sizeof(double));
          if(CurrentContext->dash_pattern)
            {
              q=CurrentContext->dash_pattern;
              p=dasharray;
              while( *p )
                *q++=*p++;
              *q=0;
            }
          else
            {
              ThrowDrawException3(ResourceLimitError,MemoryAllocationFailed,
                UnableToDrawOnImage)
            }
        }

      (void) MvgPrintf(context, "stroke-dasharray ");
      if ( n_new == 0 )
        (void) MvgPrintf(context, "none");
      else
        {
          p = dasharray;
          i = n_new;
          (void) MvgPrintf(context, "%.4g", *p++);
          while (i--)
            (void) MvgPrintf(context, ",%.4g", *p++);
        }
      (void) MvgPrintf(context, "0 \n");
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t S t r o k e D a s h O f f s e t                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetStrokeDashOffset() returns the offset into the dash pattern to
%  start the dash.
%
%  The format of the DrawGetStrokeDashOffset method is:
%
%      double DrawGetStrokeDashOffset(DrawContext context)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
*/
MagickExport double DrawGetStrokeDashOffset(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  return CurrentContext->dash_offset;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t S t r o k e D a s h O f f s e t                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetStrokeDashOffset() specifies the offset into the dash pattern to
%  start the dash.
%
%  The format of the DrawSetStrokeDashOffset method is:
%
%      void DrawSetStrokeDashOffset(DrawContext context,
%                                   const double dash_offset)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o dash_offset: dash offset
%
*/
MagickExport void DrawSetStrokeDashOffset(DrawContext context,
                                          const double dash_offset)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->filter_off ||
     (AbsoluteValue(CurrentContext->dash_offset-dash_offset) > MagickEpsilon))
    {
      CurrentContext->dash_offset = dash_offset;

      (void) MvgPrintf(context, "stroke-dashoffset %.4g\n", dash_offset);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t S t r o k e L i n e C a p                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetStrokeLineCap() returns the shape to be used at the end of
%  open subpaths when they are stroked. Values of LineCap are
%  UndefinedCap, ButtCap, RoundCap, and SquareCap.
%
%  The format of the DrawGetStrokeLineCap method is:
%
%      LineCap DrawGetStrokeLineCap(DrawContext context)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
% */
MagickExport LineCap DrawGetStrokeLineCap(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  return CurrentContext->linecap;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t S t r o k e L i n e C a p                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetStrokeLineCap() specifies the shape to be used at the end of
%  open subpaths when they are stroked. Values of LineCap are
%  UndefinedCap, ButtCap, RoundCap, and SquareCap.
%
%  The format of the DrawSetStrokeLineCap method is:
%
%      void DrawSetStrokeLineCap(DrawContext context,
%                                const LineCap linecap)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o linecap: linecap style
%
% */
MagickExport void DrawSetStrokeLineCap(DrawContext context,
                                       const LineCap linecap)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->filter_off || (CurrentContext->linecap != linecap))
    {
      const char
        *p = NULL;

      CurrentContext->linecap = linecap;

      switch (linecap)
        {
        case ButtCap:
          p = "butt";
          break;
        case RoundCap:
          p = "round";
          break;
        case SquareCap:
          p = "square";
          break;
        default:
          break;
        }

      if (p != NULL)
        (void) MvgPrintf(context, "stroke-linecap %s\n", p);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t S t r o k e L i n e J o i n                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetStrokeLineJoin() returns the shape to be used at the
%  corners of paths (or other vector shapes) when they are
%  stroked. Values of LineJoin are UndefinedJoin, MiterJoin, RoundJoin,
%  and BevelJoin.
%
%  The format of the DrawGetStrokeLineJoin method is:
%
%      LineJoin DrawGetStrokeLineJoin(DrawContext context)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
% */
MagickExport LineJoin DrawGetStrokeLineJoin(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  return CurrentContext->linejoin;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t S t r o k e L i n e J o i n                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetStrokeLineJoin() specifies the shape to be used at the
%  corners of paths (or other vector shapes) when they are
%  stroked. Values of LineJoin are UndefinedJoin, MiterJoin, RoundJoin,
%  and BevelJoin.
%
%  The format of the DrawSetStrokeLineJoin method is:
%
%      void DrawSetStrokeLineJoin(DrawContext context, const LineJoin linejoin)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o linejoin: line join style
%
% */
MagickExport void DrawSetStrokeLineJoin(DrawContext context,
                                        const LineJoin linejoin)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->filter_off || (CurrentContext->linejoin != linejoin))
    {
      const char
        *p = NULL;

      CurrentContext->linejoin = linejoin;

      switch (linejoin)
        {
        case MiterJoin:
          p = "miter";
          break;
        case RoundJoin:
          p = "round";
          break;
        case BevelJoin:
          p = "square";
          break;
        default:
          break;
        }

      if (p != NULL)
        (void) MvgPrintf(context, "stroke-linejoin %s\n", p);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t S t r o k e M i t e r L i m i t                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetStrokeMiterLimit() returns the miter limit. When two line
%  segments meet at a sharp angle and miter joins have been specified for
%  'lineJoin', it is possible for the miter to extend far beyond the
%  thickness of the line stroking the path. The miterLimit' imposes a
%  limit on the ratio of the miter length to the 'lineWidth'.
%
%  The format of the DrawGetStrokeMiterLimit method is:
%
%      unsigned long DrawGetStrokeMiterLimit(DrawContext context)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
% */
MagickExport unsigned long DrawGetStrokeMiterLimit(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  return CurrentContext->miterlimit;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t S t r o k e M i t e r L i m i t                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetStrokeMiterLimit() specifies the miter limit. When two line
%  segments meet at a sharp angle and miter joins have been specified for
%  'lineJoin', it is possible for the miter to extend far beyond the
%  thickness of the line stroking the path. The miterLimit' imposes a
%  limit on the ratio of the miter length to the 'lineWidth'.
%
%  The format of the DrawSetStrokeMiterLimit method is:
%
%      void DrawSetStrokeMiterLimit(DrawContext context,
%                                   const unsigned long miterlimit)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o miterlimit: miter limit
%
% */
MagickExport void DrawSetStrokeMiterLimit(DrawContext context,
                                          const unsigned long miterlimit)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(CurrentContext->miterlimit != miterlimit)
    {
      CurrentContext->miterlimit = miterlimit;

      (void) MvgPrintf(context, "stroke-miterlimit %lu\n", miterlimit);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t S t r o k e O p a c i t y                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetStrokeOpacity() returns the opacity of stroked object outlines.
%
%  The format of the DrawGetStrokeOpacity method is:
%
%      double DrawGetStrokeOpacity(DrawContext context)
%
%  A description of each parameter follows:
%
%    o context: drawing context
*/
MagickExport double DrawGetStrokeOpacity(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  return (((double)(MaxRGB-CurrentContext->stroke.opacity))/MaxRGB);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t S t r o k e O p a c i t y                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetStrokeOpacity() specifies the opacity of stroked object outlines.
%
%  The format of the DrawSetStrokeOpacity method is:
%
%      void DrawSetStrokeOpacity(DrawContext context,
%                                const double stroke_opacity)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o stroke_opacity: stroke opacity.  The value 1.0 is opaque.
%
*/
MagickExport void DrawSetStrokeOpacity(DrawContext context,
                                       const double stroke_opacity)
{
  Quantum
    quantum_opacity;

  double
    validated_opacity;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  validated_opacity=(stroke_opacity < 0.0 ? 0.0 : (stroke_opacity > 1.0 ? 1.0 : stroke_opacity));
  quantum_opacity = (Quantum) (((double) MaxRGB*(1.0-validated_opacity))+0.5);

  if (context->filter_off || (CurrentContext->stroke.opacity != quantum_opacity))
    {
      CurrentContext->stroke.opacity = quantum_opacity;
      (void) MvgPrintf(context, "stroke-opacity %.4g\n", validated_opacity);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t S t r o k e W i d t h                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetStrokeWidth() returns the width of the stroke used to draw object
%  outlines.
%
%  The format of the DrawGetStrokeWidth method is:
%
%      double DrawGetStrokeWidth(DrawContext context)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
*/
MagickExport double DrawGetStrokeWidth(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  return CurrentContext->stroke_width;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t S t r o k e W i d t h                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetStrokeWidth() sets the width of the stroke used to draw object
%  outlines.
%
%  The format of the DrawSetStrokeWidth method is:
%
%      void DrawSetStrokeWidth(DrawContext context, const double stroke_width)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o stroke_width: stroke width
%
*/
MagickExport void DrawSetStrokeWidth(DrawContext context,
                                     const double stroke_width)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if (context->filter_off ||
      (AbsoluteValue(CurrentContext->stroke_width-stroke_width) > MagickEpsilon))
    {
      CurrentContext->stroke_width = stroke_width;

      (void) MvgPrintf(context, "stroke-width %.4g\n", stroke_width);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t T e x t A n t i a l i a s                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetTextAntialias() returns the current text antialias setting, which
%  determines whether text is antialiased.  Text is antialiased by default.
%
%  The format of the DrawGetTextAntialias method is:
%
%      unsigned int DrawGetTextAntialias(DrawContext context)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
*/
MagickExport unsigned int DrawGetTextAntialias(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  return CurrentContext->text_antialias;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t T e x t A n t i a l i a s                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetTextAntialias() controls whether text is antialiased.  Text is
%  antialiased by default.
%
%  The format of the DrawSetTextAntialias method is:
%
%      void DrawSetTextAntialias(DrawContext context,
%                                const unsigned int text_antialias)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o text_antialias: antialias boolean. Set to false (0) to disable
%                      antialiasing.
%
*/
MagickExport void DrawSetTextAntialias(DrawContext context,
                                       const unsigned int text_antialias)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if (context->filter_off || (CurrentContext->text_antialias != text_antialias))
    {
      CurrentContext->text_antialias = text_antialias;

      (void) MvgPrintf(context, "text-antialias %i\n", text_antialias ? 1 : 0);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t T e x t D e c o r a t i o n                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetTextDecoration() returns the decoration applied when annotating with
%  text.
%
%  The format of the DrawGetTextDecoration method is:
%
%      DecorationType DrawGetTextDecoration(DrawContext context)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
*/
MagickExport DecorationType DrawGetTextDecoration(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  
  return CurrentContext->decorate;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t T e x t D e c o r a t i o n                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetTextDecoration() specifies a decoration to be applied when
%  annotating with text.
%
%  The format of the DrawSetTextDecoration method is:
%
%      void DrawSetTextDecoration(DrawContext context,
%                                 const DecorationType decoration)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o decoration: text decoration.  One of NoDecoration, UnderlineDecoration,
%                                    OverlineDecoration, or LineThroughDecoration
%
*/
MagickExport void DrawSetTextDecoration(DrawContext context,
                                        const DecorationType decoration)
{
  const char
    *p = NULL;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if (context->filter_off || (CurrentContext->decorate != decoration))
    {
      CurrentContext->decorate = decoration;

      switch (decoration)
        {
        case NoDecoration:
          p = "none";
          break;
        case UnderlineDecoration:
          p = "underline";
          break;
        case OverlineDecoration:
          p = "overline";
          break;
        case LineThroughDecoration:
          p = "line-through";
          break;
        }

      if (p != NULL)
        (void) MvgPrintf(context, "decorate %s\n", p);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t T e x t E n c o d i n g                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetTextEncoding() returns a null-terminated string which specifies the
%  code set used for text annotations. The string must be freed by the user
%  once it is no longer required.
%
%  The format of the DrawGetTextEncoding method is:
%
%      char *DrawGetTextEncoding(DrawContext context)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
% */
MagickExport char *DrawGetTextEncoding(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if (CurrentContext->encoding != (char *)NULL)
    return (char *) AllocateString(CurrentContext->encoding);
  else
    return (char *) NULL;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t T e x t E n c o d i n g                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetTextEncoding() specifies specifies the code set to use for
%  text annotations. The only character encoding which may be specified
%  at this time is "UTF-8" for representing Unicode as a sequence of
%  bytes. Specify an empty string to set text encoding to the system's
%  default. Successful text annotation using Unicode may require fonts
%  designed to support Unicode.
%
%  The format of the DrawSetTextEncoding method is:
%
%      void DrawSetTextEncoding(DrawContext context, const char* encoding)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o encoding: character string specifying text encoding
%
*/
MagickExport void DrawSetTextEncoding(DrawContext context, const char* encoding)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(encoding != (char *) NULL);

  if (context->filter_off || (CurrentContext->encoding == (char *) NULL) ||
      (LocaleCompare(CurrentContext->encoding,encoding) != 0))
    {
        (void) CloneString(&CurrentContext->encoding,encoding);

      (void) MvgPrintf(context, "encoding '%s'\n", encoding);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w G e t T e x t U n d e r C o l o r                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawGetTextUnderColor() returns the color of a background rectangle
%  to place under text annotations.
%
%  The format of the DrawGetTextUnderColor method is:
%
%      PixelPacket DrawGetTextUnderColor(DrawContext context)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
*/
MagickExport PixelPacket DrawGetTextUnderColor(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  return CurrentContext->undercolor;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t T e x t U n d e r C o l o r                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetTextUnderColor() specifies the color of a background rectangle
%  to place under text annotations.
%
%  The format of the DrawSetTextUnderColor method is:
%
%      void DrawSetTextUnderColor(DrawContext context,
%                                 const PixelPacket *under_color)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o under_color: text under color
%
*/
MagickExport void DrawSetTextUnderColor(DrawContext context,
                                        const PixelPacket *under_color)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(under_color != (const PixelPacket *)NULL);

  if (context->filter_off || !(PixelPacketMatch(&CurrentContext->undercolor, under_color)))
    {
      CurrentContext->undercolor = *under_color;
      (void) MvgPrintf(context, "text-undercolor '");
      MvgAppendColor(context, under_color);
      (void) MvgPrintf(context, "'\n");
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t T e x t U n d e r C o l o r S t r i n g                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetTextUnderColorString() specifies the color of a background rectangle
%  to place under text annotations.
%
%  The format of the DrawSetTextUnderColorString method is:
%
%      void DrawSetTextUnderColorString(DrawContext context,
%                                       const char* under_color)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o under_color: text under color
%
*/
MagickExport void DrawSetTextUnderColorString(DrawContext context,
                                              const char* under_color)
{
  PixelPacket
    pixel_packet;

  if(QueryColorDatabase(under_color,&pixel_packet,&context->image->exception))
    DrawSetTextUnderColor(context,&pixel_packet);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w T r a n s l a t e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawTranslate() applies a translation to the current coordinate
%  system which moves the coordinate system origin to the specified
%  coordinate.
%
%  The format of the DrawTranslate method is:
%
%      void DrawTranslate(DrawContext context,
%                            const double x, const double y)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o x: new x ordinate for coordinate system origin
%
%    o y: new y ordinate for coordinate system origin
%
*/
MagickExport void DrawTranslate(DrawContext context,
                                   const double x, const double y)
{
  AffineMatrix
    affine;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  IdentityAffine(&affine);
  affine.tx=x;
  affine.ty=y;
  AdjustAffine( context, &affine );

  (void) MvgPrintf(context, "translate %.4g,%.4g\n", x, y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w S e t V i e w b o x                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawSetViewbox() sets the overall canvas size to be recorded with the
%  drawing vector data.  Usually this will be specified using the same
%  size as the canvas image.  When the vector data is saved to SVG or MVG
%  formats, the viewbox is use to specify the size of the canvas image that
%  a viewer will render the vector data on.
%
%  The format of the DrawSetViewbox method is:
%
%      void DrawSetViewbox(DrawContext context,
%                          unsigned long x1, unsigned long y1,
%                          unsigned long x2, unsigned long y2)
%
%  A description of each parameter follows:
%
%    o context: drawing context
%
%    o x1: left x ordinate
%
%    o y1: top y ordinate
%
%    o x2: right x ordinate
%
%    o y2: bottom y ordinate
%
*/
MagickExport void DrawSetViewbox(DrawContext context,
                                 unsigned long x1, unsigned long y1,
                                 unsigned long x2, unsigned long y2)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  (void) MvgPrintf(context, "viewbox %lu %lu %lu %lu\n", x1, y1, x2, y2);
}
