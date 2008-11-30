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
%                 RRRR   EEEEE  N   N  DDDD   EEEEE  RRRR                     %
%                 R   R  E      NN  N  D   D  E      R   R                    %
%                 RRRR   EEE    N N N  D   D  EEE    RRRR                     %
%                 R R    E      N  NN  D   D  E      R R                      %
%                 R  R   EEEEE  N   N  DDDD   EEEEE  R  R                     %
%                                                                             %
%                                                                             %
%                   GraphicsMagick Image Drawing Methods                      %
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
% Bill Radcliffe of Corbis (www.corbis.com) contributed the polygon
% rendering code based on Paul Heckbert's "Concave Polygon Scan Conversion",
% Graphics Gems, 1990.  Leonard Rosenthal and David Harr of Appligent
% (www.appligent.com) contributed the dash pattern, linecap stroking
% algorithm, and minor rendering improvements.
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/alpha_composite.h"
#include "magick/attribute.h"
#include "magick/blob.h"
#include "magick/color.h"
#include "magick/constitute.h"
#include "magick/enhance.h"
#include "magick/enum_strings.h"
#include "magick/gem.h"
#include "magick/log.h"
#include "magick/monitor.h"
#include "magick/paint.h"
#include "magick/pixel_cache.h"
#include "magick/render.h"
#include "magick/transform.h"
#include "magick/utility.h"

/*
  Define declarations.
*/
#define BezierQuantum  200

/*
  Typedef declarations.
*/

typedef struct _EdgeInfo
{
  SegmentInfo
    bounds;

  double
    scanline;

  PointInfo
    *points;

  long
    number_points,
    direction,
    ghostline,
    highwater;
} EdgeInfo;

typedef struct _PolygonInfo
{
  EdgeInfo
    *edges;

  long
    number_edges;
} PolygonInfo;

typedef enum
{
  MoveToCode,
  OpenCode,
  GhostlineCode,
  LineToCode,
  EndCode
} PathInfoCode;

typedef struct _PathInfo
{
  PointInfo
    point;

  PathInfoCode
    code;
} PathInfo;

/*
  Forward declarations.
*/
static PrimitiveInfo
  *TraceStrokePolygon(const DrawInfo *,const PrimitiveInfo *);

static MagickPassFail
  DrawPrimitive(Image *,const DrawInfo *,const PrimitiveInfo *),
  DrawStrokePolygon(Image *,const DrawInfo *,const PrimitiveInfo *);

static unsigned long
  TracePath(PrimitiveInfo *,const char *);

static void
#if 0
  DestroyGradientInfo(GradientInfo *),
#endif
  TraceArc(PrimitiveInfo *,const PointInfo,const PointInfo,const PointInfo),
  TraceArcPath(PrimitiveInfo *,const PointInfo,const PointInfo,const PointInfo,
    const double,const unsigned int,const unsigned int),
  TraceBezier(PrimitiveInfo *,const unsigned long),
  TraceCircle(PrimitiveInfo *,const PointInfo,const PointInfo),
  TraceEllipse(PrimitiveInfo *,const PointInfo,const PointInfo,const PointInfo),
  TraceLine(PrimitiveInfo *,const PointInfo,const PointInfo),
  TracePoint(PrimitiveInfo *,const PointInfo),
  TraceRectangle(PrimitiveInfo *,const PointInfo,const PointInfo),
  TraceRoundRectangle(PrimitiveInfo *,const PointInfo,const PointInfo,
    PointInfo),
  TraceSquareLinecap(PrimitiveInfo *,const unsigned long,const double);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e D r a w I n f o                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CloneDrawInfo() makes a copy of the given draw info structure.  If NULL
%  is specified, a new image info structure is created initialized to
%  default values.
%
%  The format of the CloneDrawInfo method is:
%
%      DrawInfo *CloneDrawInfo(const ImageInfo *image_info,
%        const DrawInfo *draw_info)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o draw_info: The draw info.
%
%
*/
MagickExport DrawInfo *CloneDrawInfo(const ImageInfo *image_info,
  const DrawInfo *draw_info)
{
  DrawInfo
    *clone_info;

  clone_info=MagickAllocateMemory(DrawInfo *,sizeof(DrawInfo));
  if (clone_info == (DrawInfo *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToAllocateDrawInfo);
  GetDrawInfo(image_info,clone_info);
  if (draw_info == (DrawInfo *) NULL)
    return(clone_info);
  clone_info->primitive=(char *) NULL;
  if (draw_info->geometry != (char *) NULL)
    clone_info->geometry=AllocateString(draw_info->geometry);
  clone_info->affine=draw_info->affine;
  clone_info->gravity=draw_info->gravity;
  clone_info->fill=draw_info->fill;
  clone_info->stroke=draw_info->stroke;
  clone_info->stroke_width=draw_info->stroke_width;
  if (draw_info->fill_pattern != (Image *) NULL)
    clone_info->fill_pattern=CloneImage(draw_info->fill_pattern,0,0,True,
      &draw_info->fill_pattern->exception);
  else
    if (draw_info->tile != (Image *) NULL)
      clone_info->fill_pattern=CloneImage(draw_info->tile,0,0,True,
        &draw_info->tile->exception);
  clone_info->tile=(Image *) NULL;  /* tile is deprecated */
  if (draw_info->stroke_pattern != (Image *) NULL)
    clone_info->stroke_pattern=CloneImage(draw_info->stroke_pattern,0,0,True,
      &draw_info->stroke_pattern->exception);
  clone_info->stroke_antialias=draw_info->stroke_antialias;
  clone_info->text_antialias=draw_info->text_antialias;
  clone_info->fill_rule=draw_info->fill_rule;
  clone_info->linecap=draw_info->linecap;
  clone_info->linejoin=draw_info->linejoin;
  clone_info->miterlimit=draw_info->miterlimit;
  clone_info->dash_offset=draw_info->dash_offset;
  clone_info->decorate=draw_info->decorate;
  clone_info->compose=draw_info->compose;
  if (draw_info->text != (char *) NULL)
    clone_info->text=AllocateString(draw_info->text);
  if (draw_info->font != (char *) NULL)
    (void) CloneString(&clone_info->font,draw_info->font);
  if (draw_info->family != (char *) NULL)
    clone_info->family=AllocateString(draw_info->family);
  clone_info->style=draw_info->style;
  clone_info->stretch=draw_info->stretch;
  clone_info->weight=draw_info->weight;
  if (draw_info->encoding != (char *) NULL)
    clone_info->encoding=AllocateString(draw_info->encoding);
  clone_info->pointsize=draw_info->pointsize;
  if (draw_info->density != (char *) NULL)
    (void) CloneString(&clone_info->density,draw_info->density);
  clone_info->align=draw_info->align;
  clone_info->undercolor=draw_info->undercolor;
  clone_info->border_color=draw_info->border_color;
  if (draw_info->server_name != (char *) NULL)
    (void) CloneString(&clone_info->server_name,draw_info->server_name);
  if (draw_info->dash_pattern != (double *) NULL)
    {
      register long
        x;

      for (x=0; draw_info->dash_pattern[x] != 0.0; x++);
      clone_info->dash_pattern=MagickAllocateMemory(double *,(x+1)*sizeof(double));
      if (clone_info->dash_pattern == (double *) NULL)
        MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
          UnableToAllocateDashPattern);
      (void) memcpy(clone_info->dash_pattern,draw_info->dash_pattern,
        (x+1)*sizeof(double));
    }
  if (draw_info->clip_path != (char *) NULL)
    clone_info->clip_path=AllocateString(draw_info->clip_path);
  clone_info->bounds=draw_info->bounds;
  clone_info->clip_units=draw_info->clip_units;
  clone_info->render=draw_info->render;
  clone_info->opacity=draw_info->opacity;
  clone_info->element_reference=draw_info->element_reference;
  return(clone_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   C o n v e r t P a t h T o P o l y g o n                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ConvertPathToPolygon converts a path to the more efficient sorted
%  rendering form.
%
%  The format of the ConvertPathToPolygon method is:
%
%      PolygonInfo *ConvertPathToPolygon(const DrawInfo *draw_info,
%        const PathInfo *path_info)
%
%  A description of each parameter follows:
%
%    o Method ConvertPathToPolygon returns the path in a more efficient sorted
%      rendering form of type PolygonInfo.
%
%    o draw_info: Specifies a pointer to an DrawInfo structure.
%
%    o path_info: Specifies a pointer to an PathInfo structure.
%
%
*/

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

static int CompareEdges(const void *x,const void *y)
{
  register const EdgeInfo
    *p,
    *q;

  /*
    Compare two edges.
  */
  p=(EdgeInfo *) x;
  q=(EdgeInfo *) y;
  if ((p->points[0].y-MagickEpsilon) > q->points[0].y)
    return(1);
  if ((p->points[0].y+MagickEpsilon) < q->points[0].y)
    return(-1);
  if ((p->points[0].x-MagickEpsilon) > q->points[0].x)
    return(1);
  if ((p->points[0].x+MagickEpsilon) < q->points[0].x)
    return(-1);
  if (((p->points[1].x-p->points[0].x)*(q->points[1].y-q->points[0].y)-
       (p->points[1].y-p->points[0].y)*(q->points[1].x-q->points[0].x)) > 0.0)
    return(1);
  return(-1);
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

static void LogPolygonInfo(const PolygonInfo *polygon_info)
{
  register EdgeInfo
    *p;

  register long
    i,
    j;

  (void) LogMagickEvent(RenderEvent,GetMagickModule(),"    begin active-edge");
  p=polygon_info->edges;
  for (i=0; i < polygon_info->number_edges; i++)
  {
    (void) LogMagickEvent(RenderEvent,GetMagickModule(),"      edge %lu:",i);
    (void) LogMagickEvent(RenderEvent,GetMagickModule(),"      direction: %s",
      p->direction ? "down" : "up");
    (void) LogMagickEvent(RenderEvent,GetMagickModule(),"      ghostline: %s",
      p->ghostline ? "transparent" : "opaque");
    (void) LogMagickEvent(RenderEvent,GetMagickModule(),
      "      bounds: %g,%g - %g,%g",p->bounds.x1,p->bounds.y1,p->bounds.x2,
      p->bounds.y2);
    for (j=0; j < p->number_points; j++)
      (void) LogMagickEvent(RenderEvent,GetMagickModule(),"        %g,%g",
        p->points[j].x,p->points[j].y);
    p++;
  }
  (void) LogMagickEvent(RenderEvent,GetMagickModule(),"    end active-edge");
}

static void ReversePoints(PointInfo *points,const int number_points)
{
  PointInfo
    point;

  register long
    i;

  for (i=0; i < (number_points >> 1); i++)
  {
    point=points[i];
    points[i]=points[number_points-(i+1)];
    points[number_points-(i+1)]=point;
  }
}

static PolygonInfo *ConvertPathToPolygon(const DrawInfo *ARGUNUSED(draw_info),
  const PathInfo *path_info)
{
  long
    direction,
    edge,
    ghostline,
    next_direction,
    number_edges,
    number_points;

  PointInfo
    point,
    *points;

  PolygonInfo
    *polygon_info;

  SegmentInfo
    bounds;

  register long
    i,
    n;

  /*
    Convert a path to the more efficient sorted rendering form.
  */
  polygon_info=MagickAllocateMemory(PolygonInfo *,sizeof(PolygonInfo));
  if (polygon_info == (PolygonInfo *) NULL)
    return((PolygonInfo *) NULL);
  number_edges=16;
  polygon_info->edges=MagickAllocateMemory(EdgeInfo *,number_edges*sizeof(EdgeInfo));
  if (polygon_info->edges == (EdgeInfo *) NULL)
    return((PolygonInfo *) NULL);
  direction=0;
  edge=0;
  ghostline=False;
  n=0;
  number_points=0;
  points=(PointInfo *) NULL;
  (void) memset(&point,0,sizeof(PointInfo));
  (void) memset(&bounds,0,sizeof(SegmentInfo));
  for (i=0; path_info[i].code != EndCode; i++)
  {
    if ((path_info[i].code == MoveToCode) || (path_info[i].code == OpenCode) ||
        (path_info[i].code == GhostlineCode))
      {
        /*
          Move to.
        */
        if ((points != (PointInfo *) NULL) && (n >= 2))
          {
            if (edge == number_edges)
              {
                number_edges<<=1;
                MagickReallocMemory(EdgeInfo *,polygon_info->edges,
                  number_edges*sizeof(EdgeInfo));
                if (polygon_info->edges == (EdgeInfo *) NULL)
                  return((PolygonInfo *) NULL);
              }
            polygon_info->edges[edge].number_points=n;
            polygon_info->edges[edge].scanline=(-1.0);
            polygon_info->edges[edge].highwater=0;
            polygon_info->edges[edge].ghostline=ghostline;
            polygon_info->edges[edge].direction=direction > 0;
            if (direction < 0)
              ReversePoints(points,n);
            polygon_info->edges[edge].points=points;
            polygon_info->edges[edge].bounds=bounds;
            polygon_info->edges[edge].bounds.y1=points[0].y;
            polygon_info->edges[edge].bounds.y2=points[n-1].y;
            points=(PointInfo *) NULL;
            ghostline=False;
            edge++;
          }
        if (points == (PointInfo *) NULL)
          {
            number_points=16;
            points=MagickAllocateMemory(PointInfo *,number_points*sizeof(PointInfo));
            if (points == (PointInfo *) NULL)
              return((PolygonInfo *) NULL);
          }
        ghostline=path_info[i].code == GhostlineCode;
        point=path_info[i].point;
        points[0]=point;
        bounds.x1=point.x;
        bounds.x2=point.x;
        direction=0;
        n=1;
        continue;
      }
    /*
      Line to.
    */
    next_direction=((path_info[i].point.y > point.y) ||
      ((path_info[i].point.y == point.y) &&
       (path_info[i].point.x > point.x))) ? 1 : -1;
    if (direction && (direction != next_direction))
      {
        /*
          New edge.
        */
        point=points[n-1];
        if (edge == number_edges)
          {
            number_edges<<=1;
            MagickReallocMemory(EdgeInfo *,polygon_info->edges,
              number_edges*sizeof(EdgeInfo));
            if (polygon_info->edges == (EdgeInfo *) NULL)
              return((PolygonInfo *) NULL);
          }
        polygon_info->edges[edge].number_points=n;
        polygon_info->edges[edge].scanline=(-1.0);
        polygon_info->edges[edge].highwater=0;
        polygon_info->edges[edge].ghostline=ghostline;
        polygon_info->edges[edge].direction=direction > 0;
        if (direction < 0)
          ReversePoints(points,n);
        polygon_info->edges[edge].points=points;
        polygon_info->edges[edge].bounds=bounds;
        polygon_info->edges[edge].bounds.y1=points[0].y;
        polygon_info->edges[edge].bounds.y2=points[n-1].y;
        number_points=16;
        points=MagickAllocateMemory(PointInfo *,number_points*sizeof(PointInfo));
        if (points == (PointInfo *) NULL)
          return((PolygonInfo *) NULL);
        n=1;
        ghostline=False;
        points[0]=point;
        bounds.x1=point.x;
        bounds.x2=point.x;
        edge++;
      }
    direction=next_direction;
    if (points == (PointInfo *) NULL)
      continue;
    if (n == number_points)
      {
        number_points<<=1;
        MagickReallocMemory(PointInfo *,points,number_points*sizeof(PointInfo));
        if (points == (PointInfo *) NULL)
          return((PolygonInfo *) NULL);
      }
    point=path_info[i].point;
    points[n]=point;
    if (point.x < bounds.x1)
      bounds.x1=point.x;
    if (point.x > bounds.x2)
      bounds.x2=point.x;
    n++;
  }
  if (points != (PointInfo *) NULL)
    {
      if (n < 2)
        {
          MagickFreeMemory(points);
        }
      else
        {
          if (edge == number_edges)
            {
              number_edges<<=1;
              MagickReallocMemory(EdgeInfo *,polygon_info->edges,
                number_edges*sizeof(EdgeInfo));
              if (polygon_info->edges == (EdgeInfo *) NULL)
                return((PolygonInfo *) NULL);
            }
          polygon_info->edges[edge].number_points=n;
          polygon_info->edges[edge].scanline=(-1.0);
          polygon_info->edges[edge].highwater=0;
          polygon_info->edges[edge].ghostline=ghostline;
          polygon_info->edges[edge].direction=direction > 0;
          if (direction < 0)
            ReversePoints(points,n);
          polygon_info->edges[edge].points=points;
          polygon_info->edges[edge].bounds=bounds;
          polygon_info->edges[edge].bounds.y1=points[0].y;
          polygon_info->edges[edge].bounds.y2=points[n-1].y;
          ghostline=False;
          edge++;
        }
    }
  polygon_info->number_edges=edge;
  qsort(polygon_info->edges,polygon_info->number_edges,sizeof(EdgeInfo),
    CompareEdges);
  if (IsEventLogging())
    LogPolygonInfo(polygon_info);
  return(polygon_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   C o n v e r t P r i m i t i v e T o P a t h                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ConvertPrimitiveToPath converts a PrimitiveInfo structure into a
%  vector path structure.
%
%  The format of the ConvertPrimitiveToPath method is:
%
%      PathInfo *ConvertPrimitiveToPath(const DrawInfo *draw_info,
%        const PrimitiveInfo *primitive_info)
%
%  A description of each parameter follows:
%
%    o Method ConvertPrimitiveToPath returns a vector path structure of type
%      PathInfo.
%
%    o draw_info: a structure of type DrawInfo.
%
%    o primitive_info: Specifies a pointer to an PrimitiveInfo structure.
%
%
*/

static void LogPathInfo(const PathInfo *path_info)
{
  register const PathInfo
    *p;

  (void) LogMagickEvent(RenderEvent,GetMagickModule(),"    begin vector-path");
  for (p=path_info; p->code != EndCode; p++)
    (void) LogMagickEvent(RenderEvent,GetMagickModule(),
      "      %g,%g %s",p->point.x,p->point.y,p->code == GhostlineCode ?
      "moveto ghostline" : p->code == OpenCode ? "moveto open" :
      p->code == MoveToCode ? "moveto" : p->code == LineToCode ? "lineto" :
      "?");
  (void) LogMagickEvent(RenderEvent,GetMagickModule(),"    end vector-path");
}

static PathInfo *ConvertPrimitiveToPath(const DrawInfo *ARGUNUSED(draw_info),
  const PrimitiveInfo *primitive_info)
{
  long
    coordinates,
    start;

  PathInfo
    *path_info;

  PathInfoCode
    code;

  PointInfo
    p,
    q;

  register long
    i,
    n;

  /*
    Converts a PrimitiveInfo structure into a vector path structure.
  */
  switch (primitive_info->primitive)
  {
    case PointPrimitive:
    case ColorPrimitive:
    case MattePrimitive:
    case TextPrimitive:
    case ImagePrimitive:
      return((PathInfo *) NULL);
    default:
      break;
  }
  for (i=0; primitive_info[i].primitive != UndefinedPrimitive; i++);
  path_info=MagickAllocateMemory(PathInfo *,(2*i+3)*sizeof(PathInfo));
  if (path_info == (PathInfo *) NULL)
    return((PathInfo *) NULL);
  coordinates=0;
  n=0;
  p.x=(-1);
  p.y=(-1);
  q.x=(-1);
  q.y=(-1);
  start=0;
  for (i=0; primitive_info[i].primitive != UndefinedPrimitive; i++)
  {
    code=LineToCode;
    if (coordinates <= 0)
      {
        coordinates=(long) primitive_info[i].coordinates;
        p=primitive_info[i].point;
        start=n;
        code=MoveToCode;
      }
    coordinates--;
    /*
      Eliminate duplicate points.
    */
    if ((i == 0) || (fabs(q.x-primitive_info[i].point.x) > MagickEpsilon) ||
        (fabs(q.y-primitive_info[i].point.y) > MagickEpsilon))
      {
        path_info[n].code=code;
        path_info[n].point=primitive_info[i].point;
        q=primitive_info[i].point;
        n++;
      }
    if (coordinates > 0)
      continue;
    if ((fabs(p.x-primitive_info[i].point.x) <= MagickEpsilon) &&
        (fabs(p.y-primitive_info[i].point.y) <= MagickEpsilon))
      continue;
    /*
      Mark the p point as open if it does not match the q.
    */
    path_info[start].code=OpenCode;
    path_info[n].code=GhostlineCode;
    path_info[n].point=primitive_info[i].point;
    n++;
    path_info[n].code=LineToCode;
    path_info[n].point=p;
    n++;
  }
  path_info[n].code=EndCode;
  path_info[n].point.x=0.0;
  path_info[n].point.y=0.0;
  if (IsEventLogging())
    LogPathInfo(path_info);
  return(path_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y D r a w I n f o                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyDrawInfo() deallocates memory associated with an DrawInfo
%  structure.
%
%  The format of the DestroyDrawInfo method is:
%
%      void DestroyDrawInfo(DrawInfo *draw_info)
%
%  A description of each parameter follows:
%
%    o draw_info: The draw info.
%
%
*/
MagickExport void DestroyDrawInfo(DrawInfo *draw_info)
{
  assert(draw_info != (DrawInfo *) NULL);
  assert(draw_info->signature == MagickSignature);
  MagickFreeMemory(draw_info->primitive);
  MagickFreeMemory(draw_info->text);
  MagickFreeMemory(draw_info->geometry);
  if (draw_info->tile != 0)
    DestroyImage(draw_info->tile);
  if (draw_info->fill_pattern != (Image *) NULL)
    DestroyImage(draw_info->fill_pattern);
  if (draw_info->stroke_pattern != (Image *) NULL)
    DestroyImage(draw_info->stroke_pattern);
  MagickFreeMemory(draw_info->font);
  MagickFreeMemory(draw_info->family);
  MagickFreeMemory(draw_info->encoding);
  MagickFreeMemory(draw_info->density);
  MagickFreeMemory(draw_info->server_name);
  MagickFreeMemory(draw_info->dash_pattern);
  MagickFreeMemory(draw_info->clip_path);
  (void) memset((void *)draw_info,0xbf,sizeof(DrawInfo));
  MagickFreeMemory(draw_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y E d g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyEdge destroys the specified polygon edge.
%
%  The format of the DestroyEdge method is:
%
%      long DestroyEdge(PolygonInfo *polygon_info,const int edge)
%
%  A description of each parameter follows:
%
%    o polygon_info: Specifies a pointer to an PolygonInfo structure.
%
%    o edge: the polygon edge number to destroy.
%
%
*/
static long DestroyEdge(PolygonInfo *polygon_info,const long edge)
{
  assert(edge >= 0);
  assert(edge < polygon_info->number_edges);
  MagickFreeMemory(polygon_info->edges[edge].points);
  polygon_info->number_edges--;
  if (edge < polygon_info->number_edges)
    (void) memmove(polygon_info->edges+edge,polygon_info->edges+edge+1,
      (polygon_info->number_edges-edge)*sizeof(EdgeInfo));
  return(polygon_info->number_edges);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y G r a d i e n t I n f o                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyGradientInfo() deallocates memory associated with the GradientInfo
%  list.
%
%  The format of the DestroyGradientInfo method is:
%
%      DestroyGradientInfo(GradientInfo *gradient_info)
%
%  A description of each parameter follows:
%
%    o gradient_info: The gradient info.
%
%
*/
#if 0
static void DestroyGradientInfo(GradientInfo *gradient_info)
{
  register GradientInfo
    *p;

  if (gradient_info == (GradientInfo *) NULL)
    return;
  assert(gradient_info->signature == MagickSignature);
  for (p=gradient_info; p->previous != (GradientInfo *) NULL; p=p->previous);
  for (gradient_info=p; p != (GradientInfo *) NULL; gradient_info=p)
  {
    p=p->next;
    MagickFreeMemory(gradient_info);
  }
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y P o l y g o n I n f o                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyPolygonInfo destroys the PolygonInfo data structure.
%
%  The format of the DestroyPolygonInfo method is:
%
%      void DestroyPolygonInfo(PolygonInfo *polygon_info)
%
%  A description of each parameter follows:
%
%    o polygon_info: Specifies a pointer to an PolygonInfo structure.
%
%
*/
static void DestroyPolygonInfo(PolygonInfo *polygon_info)
{
  register long
    i;

  for (i=0; i < polygon_info->number_edges; i++)
    MagickFreeMemory(polygon_info->edges[i].points);
  MagickFreeMemory(polygon_info->edges);
  MagickFreeMemory(polygon_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     D r a w A f f i n e I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawAffineImage() composites the source over the destination image as
%  dictated by the affine transform.
%
%  The format of the DrawAffineImage method is:
%
%      unsigned int DrawAffineImage(Image *image,const Image *composite,
%        const AffineMatrix *affine)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o image: The composite image.
%
%    o affine: The affine transform.
%
%
*/

static SegmentInfo AffineEdge(const Image *image,const AffineMatrix *affine,
                              const long y,const SegmentInfo *edge)
{
  double
    intercept,
    z;

  register long
    x;

  SegmentInfo
    inverse_edge;

  /*
    Determine left and right edges.
  */
  inverse_edge.x1=edge->x1;
  inverse_edge.y1=0;
  inverse_edge.x2=edge->x2;
  inverse_edge.y2=0;
  z=affine->ry*(y+0.5)+affine->tx;
  if (affine->sx > MagickEpsilon)
    {
      intercept=(-z/affine->sx);
      x=(long) ceil(intercept+MagickEpsilon-0.5);
      if (x > inverse_edge.x1)
        inverse_edge.x1=x;
      intercept=(-z+image->columns)/affine->sx;
      x=(long) ceil(intercept-MagickEpsilon-0.5);
      if (x < inverse_edge.x2)
        inverse_edge.x2=x;
    }
  else
    if (affine->sx < -MagickEpsilon)
      {
        intercept=(-z+image->columns)/affine->sx;
        x=(long) ceil(intercept+MagickEpsilon-0.5);
        if (x > inverse_edge.x1)
          inverse_edge.x1=x;
        intercept=(-z/affine->sx);
        x=(long) ceil(intercept-MagickEpsilon-0.5);
        if (x < inverse_edge.x2)
          inverse_edge.x2=x;
      }
    else
      if ((z < 0) || (z >= image->columns))
        {
          inverse_edge.x2=edge->x1;
          return(inverse_edge);
        }
  /*
    Determine top and bottom edges.
  */
  z=affine->sy*(y+0.5)+affine->ty;
  if (affine->rx > MagickEpsilon)
    {
      intercept=(-z /affine->rx);
      x=(long) ceil(intercept+MagickEpsilon-0.5);
      if (x > inverse_edge.x1)
        inverse_edge.x1=x;
      intercept=(-z+image->rows)/affine->rx;
      x=(long) ceil(intercept-MagickEpsilon-0.5);
      if (x < inverse_edge.x2)
        inverse_edge.x2=x;
    }
  else
    if (affine->rx < -MagickEpsilon)
      {
        intercept=(-z+image->rows)/affine->rx;
        x=(long) ceil(intercept+MagickEpsilon-0.5);
        if (x > inverse_edge.x1)
          inverse_edge.x1=x;
        intercept=(-z/affine->rx);
        x=(long) ceil(intercept-MagickEpsilon-0.5);
        if (x < inverse_edge.x2)
          inverse_edge.x2=x;
      }
    else
      if ((z < 0) || (z >= image->rows))
        {
          inverse_edge.x2=edge->x1;
          return(inverse_edge);
        }
  return(inverse_edge);
}

static AffineMatrix InverseAffineMatrix(const AffineMatrix *affine)
{
  AffineMatrix
    inverse_affine;

  double
    determinant;

  determinant=1.0/(affine->sx*affine->sy-affine->rx*affine->ry);
  inverse_affine.sx=determinant*affine->sy;
  inverse_affine.rx=determinant*(-affine->rx);
  inverse_affine.ry=determinant*(-affine->ry);
  inverse_affine.sy=determinant*affine->sx;
  inverse_affine.tx=
    (-affine->tx)*inverse_affine.sx-affine->ty*inverse_affine.ry;
  inverse_affine.ty=
    (-affine->tx)*inverse_affine.rx-affine->ty*inverse_affine.sy;
  return(inverse_affine);
}

#define AffineDrawImageText "[%s] Affine composite image..."
MagickExport MagickPassFail DrawAffineImage(Image *image,const Image *composite,
                                            const AffineMatrix *affine)
{
  MagickPassFail
    status = MagickPass;

  unsigned long
    row_count=0;

  AffineMatrix
    inverse_affine;

  long
    y,
    y_max,
    y_min;

  PointInfo
    extent[4],
    min,
    max;

  register long
    i;

  SegmentInfo
    edge;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(composite != (const Image *) NULL);
  assert(composite->signature == MagickSignature);
  assert(affine != (AffineMatrix *) NULL);

  /*
    Determine bounding box.
  */
  extent[0].x=0;
  extent[0].y=0;
  extent[1].x=composite->columns;
  extent[1].y=0;
  extent[2].x=composite->columns;
  extent[2].y=composite->rows;
  extent[3].x=0;
  extent[3].y=composite->rows;
  for (i=0; i < 4; i++)
    {
      register long
        x;

      x=(long) extent[i].x;
      y=(long) extent[i].y;
      extent[i].x=x*affine->sx+y*affine->ry+affine->tx;
      extent[i].y=x*affine->rx+y*affine->sy+affine->ty;
    }
  min=extent[0];
  max=extent[0];
  for (i=1; i < 4; i++)
    {
      if (min.x > extent[i].x)
        min.x=extent[i].x;
      if (min.y > extent[i].y)
        min.y=extent[i].y;
      if (max.x < extent[i].x)
        max.x=extent[i].x;
      if (max.y < extent[i].y)
        max.y=extent[i].y;
    }
  /*
    Affine transform image.
  */
  (void) SetImageType(image,TrueColorType);
  edge.x1=min.x;
  edge.y1=min.y;
  edge.x2=max.x;
  edge.y2=max.y;
  inverse_affine=InverseAffineMatrix(affine);
  if (edge.y1 < 0)
    edge.y1=0.0;
  if (edge.y2 >= image->rows)
    edge.y2=image->rows-1;
  y_min=(long) ceil(edge.y1-0.5);
  y_max=(long) floor(edge.y2+0.5);
#if defined(HAVE_OPENMP)
#  pragma omp parallel for schedule(dynamic) shared(row_count, status)
#endif
  for (y=y_min; y <= y_max; y++)
    {
      MagickBool
        thread_status;

      long
        start,
        stop;

      SegmentInfo
        inverse_edge;

      register PixelPacket
        *q;

      register long
        x;

      thread_status=status;
      if (thread_status == MagickFail)
        continue;

      inverse_edge=AffineEdge(composite,&inverse_affine,y,&edge);
      if (inverse_edge.x2 < inverse_edge.x1)
        continue;
      if (inverse_edge.x1 < 0)
        inverse_edge.x1=0.0;
      if (inverse_edge.x2 >= image->columns)
        inverse_edge.x2=image->columns-1;
      start=(long) ceil(inverse_edge.x1-0.5);
      stop=(long) floor(inverse_edge.x2+0.5);
      x=start;
      q=GetImagePixelsEx(image,x,y,stop-x+1,1,&image->exception);
      if (q == (PixelPacket *) NULL)
        thread_status=MagickFail;
      if (thread_status != MagickFail)
        {
          for ( ; x <= stop; x++)
            {
              PointInfo
                point;

              PixelPacket
                pixel;

              point.x=x*inverse_affine.sx+y*inverse_affine.ry+inverse_affine.tx;
              point.y=x*inverse_affine.rx+y*inverse_affine.sy+inverse_affine.ty;
              (void) AcquireOnePixelByReference(composite,&pixel,(long) point.x,
                                                (long) point.y,&image->exception);
              if (!composite->matte)
                pixel.opacity=OpaqueOpacity;
              AlphaCompositePixel(q,&pixel,pixel.opacity,q,q->opacity);
              q++;
            }
          if (!SyncImagePixelsEx(image,&image->exception))
            thread_status=MagickFail;
        }
#if defined(HAVE_OPENMP)
#  pragma omp critical
#endif
      {
        row_count++;
        if (QuantumTick(row_count,y_max-y_min+1))
          if (!MagickMonitorFormatted(row_count,y_max-y_min+1,&image->exception,
                                      AffineDrawImageText,image->filename))
            thread_status=MagickFail;

        if (thread_status == MagickFail)
          status=MagickFail;
      }
    }

  return (status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D r a w B o u n d i n g R e c t a n g l e s                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DrawBoundingRectangles draws the bounding rectangles on the image.
%  This is only useful for developers debugging the rendering algorithm.
%
%  The format of the DrawBoundingRectangles method is:
%
%      void DrawBoundingRectangles(Image *image,const DrawInfo *draw_info,
%        PolygonInfo *polygon_info)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o draw_info: The draw info.
%
%    o polygon_info: Specifies a pointer to a PolygonInfo structure.
%
%
*/
static void DrawBoundingRectangles(Image *image,const DrawInfo *draw_info,
  const PolygonInfo *polygon_info)
{
  double
    mid;

  DrawInfo
    *clone_info;

  long
    coordinates;

  PointInfo
    end,
    resolution,
    start;

  PrimitiveInfo
    primitive_info[6];

  register long
    i;

  SegmentInfo
    bounds;

  clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  (void) QueryColorDatabase("#000000ff",&clone_info->fill,&image->exception);
  resolution.x=72.0;
  resolution.y=72.0;
  if (clone_info->density != (char *) NULL)
    {
      int
        count;

      count=GetMagickDimension(clone_info->density,&resolution.x,&resolution.y,NULL,NULL);
      if (count != 2)
        resolution.y=resolution.x;
    }
  mid=(resolution.x/72.0)*ExpandAffine(&clone_info->affine)*
    clone_info->stroke_width/2.0;
  if (polygon_info != (PolygonInfo *) NULL)
    {
      bounds=polygon_info->edges[0].bounds;
      for (i=1; i < polygon_info->number_edges; i++)
      {
        if (polygon_info->edges[i].bounds.x1 < bounds.x1)
          bounds.x1=polygon_info->edges[i].bounds.x1;
        if (polygon_info->edges[i].bounds.y1 < bounds.y1)
          bounds.y1=polygon_info->edges[i].bounds.y1;
        if (polygon_info->edges[i].bounds.x2 > bounds.x2)
          bounds.x2=polygon_info->edges[i].bounds.x2;
        if (polygon_info->edges[i].bounds.y2 > bounds.y2)
          bounds.y2=polygon_info->edges[i].bounds.y2;
      }
      bounds.x1-=mid;
      bounds.x1=bounds.x1 < 0.0 ? 0.0 :
        bounds.x1 >= image->columns ? image->columns-1 : bounds.x1;
      bounds.y1-=mid;
      bounds.y1=bounds.y1 < 0.0 ? 0.0 :
        bounds.y1 >= image->rows ? image->rows-1 : bounds.y1;
      bounds.x2+=mid;
      bounds.x2=bounds.x2 < 0.0 ? 0.0 :
        bounds.x2 >= image->columns ? image->columns-1 : bounds.x2;
      bounds.y2+=mid;
      bounds.y2=bounds.y2 < 0.0 ? 0.0 :
        bounds.y2 >= image->rows ? image->rows-1 : bounds.y2;
      for (i=0; i < polygon_info->number_edges; i++)
      {
        if (polygon_info->edges[i].direction)
          (void) QueryColorDatabase("red",&clone_info->stroke,
            &image->exception);
        else
          (void) QueryColorDatabase("green",&clone_info->stroke,
            &image->exception);
        start.x=polygon_info->edges[i].bounds.x1-mid;
        start.y=polygon_info->edges[i].bounds.y1-mid;
        end.x=polygon_info->edges[i].bounds.x2+mid;
        end.y=polygon_info->edges[i].bounds.y2+mid;
        primitive_info[0].primitive=RectanglePrimitive;
        TraceRectangle(primitive_info,start,end);
        primitive_info[0].method=ReplaceMethod;
        coordinates=(long) primitive_info[0].coordinates;
        primitive_info[coordinates].primitive=UndefinedPrimitive;
        (void) DrawPrimitive(image,clone_info,primitive_info);
      }
    }
  (void) QueryColorDatabase("blue",&clone_info->stroke,&image->exception);
  start.x=bounds.x1-mid;
  start.y=bounds.y1-mid;
  end.x=bounds.x2+mid;
  end.y=bounds.y2+mid;
  primitive_info[0].primitive=RectanglePrimitive;
  TraceRectangle(primitive_info,start,end);
  primitive_info[0].method=ReplaceMethod;
  coordinates=(long) primitive_info[0].coordinates;
  primitive_info[coordinates].primitive=UndefinedPrimitive;
  (void) DrawPrimitive(image,clone_info,primitive_info);
  DestroyDrawInfo(clone_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w C l i p P a t h                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawClipPath() draws the clip path on the image mask.
%
%  The format of the DrawClipPath method is:
%
%      unsigned int DrawClipPath(Image *image,const DrawInfo *draw_info,
%        const char *name)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o draw_info: The draw info.
%
%    o name: The name of the clip path.
%
%
*/
MagickExport unsigned int DrawClipPath(Image *image,const DrawInfo *draw_info,
  const char *name)
{
  char
    clip_path[MaxTextExtent];

  const ImageAttribute
    *attribute;

  DrawInfo
    *clone_info;

  unsigned int
    status;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(draw_info != (const DrawInfo *) NULL);
  FormatString(clip_path,"[%.1024s]",name);
  attribute=GetImageAttribute(image,clip_path);
  if (attribute == (ImageAttribute *) NULL)
    return(False);
  if (image->clip_mask == (Image *) NULL)
    {
      Image
        *clip_mask;

      clip_mask=CloneImage(image,image->columns,image->rows,True,
        &image->exception);
      if (clip_mask == (Image *) NULL)
        return(False);
      (void) SetImageClipMask(image,clip_mask);
      DestroyImage(clip_mask);
    }
  (void) QueryColorDatabase("none",&image->clip_mask->background_color,
    &image->exception);
  (void) SetImage(image->clip_mask,TransparentOpacity);
  (void) LogMagickEvent(RenderEvent,GetMagickModule(),
    "\nbegin clip-path %.1024s",draw_info->clip_path);
  clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  (void) CloneString(&clone_info->primitive,attribute->value);
  (void) QueryColorDatabase("white",&clone_info->fill,&image->exception);
  MagickFreeMemory(clone_info->clip_path);
  status=DrawImage(image->clip_mask,clone_info);
  (void) NegateImage(image->clip_mask,False);
  DestroyDrawInfo(clone_info);
  (void) LogMagickEvent(RenderEvent,GetMagickModule(),"end clip-path");
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D r a w D a s h P o l y g o n                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DrawDashPolygon draws a dashed polygon (line, rectangle, ellipse)
%  on the image while respecting the dash offset and dash pattern attributes.
%
%  The format of the DrawDashPolygon method is:
%
%      unsigned int DrawDashPolygon(const DrawInfo *draw_info,
%        const PrimitiveInfo *primitive_info,Image *image)
%
%  A description of each parameter follows:
%
%    o draw_info: The draw info.
%
%    o primitive_info: Specifies a pointer to a PrimitiveInfo structure.
%
%    o image: The image.
%
%
*/
static unsigned int DrawDashPolygon(const DrawInfo *draw_info,
  const PrimitiveInfo *primitive_info,Image *image)
{
  double
    length,
    maximum_length,
    offset,
    scale,
    total_length;

  DrawInfo
    *clone_info;

  int
    j,
    n;

  PrimitiveInfo
    *dash_polygon;

  register double
    dx,
    dy;

  register long
    i;

  unsigned int
    status;

  unsigned long
    number_vertices;

  assert(draw_info != (const DrawInfo *) NULL);
  (void) LogMagickEvent(RenderEvent,GetMagickModule(),"    begin draw-dash");
  clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  clone_info->miterlimit=0;
  for (i=0; primitive_info[i].primitive != UndefinedPrimitive; i++);
  number_vertices=i;
  dash_polygon=MagickAllocateMemory(PrimitiveInfo *,
    (2*number_vertices+1)*sizeof(PrimitiveInfo));
  if (dash_polygon == (PrimitiveInfo *) NULL)
    return(False);
  dash_polygon[0]=primitive_info[0];
  scale=ExpandAffine(&draw_info->affine);
  length=scale*draw_info->dash_pattern[0];
  offset=draw_info->dash_offset != 0.0 ? scale*draw_info->dash_offset : 0.0;
  j=1;
  for (n=0; offset > 0.0; j=0)
  {
    if (draw_info->dash_pattern[n] <= 0.0)
      break;
    length=scale*draw_info->dash_pattern[n];
    if (offset > length)
      {
        offset-=length;
        n++;
        length=scale*draw_info->dash_pattern[n];
        continue;
      }
    if (offset < length)
      {
        length-=offset;
        offset=0.0;
        break;
      }
    offset=0.0;
    n++;
  }
  status=True;
  for (i=1; i < (long) number_vertices; i++)
  {
    dx=primitive_info[i].point.x-primitive_info[i-1].point.x;
    dy=primitive_info[i].point.y-primitive_info[i-1].point.y;
    maximum_length=sqrt(dx*dx+dy*dy+MagickEpsilon);
    if (length == 0)
      {
        n++;
        if (draw_info->dash_pattern[n] == 0.0)
          n=0;
        length=scale*draw_info->dash_pattern[n];
      }
    for (total_length=0.0; maximum_length >= (length+total_length); )
    {
      total_length+=length;
      if (n & 0x01)
        {
          dash_polygon[0]=primitive_info[0];
          dash_polygon[0].point.x=primitive_info[i-1].point.x+
            dx*total_length/maximum_length;
          dash_polygon[0].point.y=primitive_info[i-1].point.y+
            dy*total_length/maximum_length;
          j=1;
        }
      else
        {
          dash_polygon[j]=primitive_info[i-1];
          dash_polygon[j].point.x=primitive_info[i-1].point.x+
            dx*total_length/maximum_length;
          dash_polygon[j].point.y=primitive_info[i-1].point.y+
            dy*total_length/maximum_length;
          dash_polygon[j].coordinates=1;
          j++;
          dash_polygon[0].coordinates=j;
          dash_polygon[j].primitive=UndefinedPrimitive;
          status|=DrawStrokePolygon(image,clone_info,dash_polygon);
        }
      n++;
      if (draw_info->dash_pattern[n] == 0.0)
        n=0;
      length=scale*draw_info->dash_pattern[n];
    }
    length-=(maximum_length-total_length);
    if (n & 0x01)
      continue;
    dash_polygon[j]=primitive_info[i];
    dash_polygon[j].coordinates=1;
    j++;
  }
  MagickFreeMemory(dash_polygon);
  DestroyDrawInfo(clone_info);
  (void) LogMagickEvent(RenderEvent,GetMagickModule(),"    end draw-dash");
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Use DrawImage() to draw a graphic primitive on your image.  The primitive
%  may be represented as a string or filename.  Precede the filename with an
%  "at" sign (@) and the contents of the file are drawn on the image.  You
%  can affect how text is drawn by setting one or more members of the draw
%  info structure.
%
%  Note that this is a legacy interface. Authors of new code should consider
%  using the Draw* methods defined by magick/draw.h since they are better
%  documented and less error prone.
%
%  The format of the DrawImage method is:
%
%      unsigned int DrawImage(Image *image,const DrawInfo *draw_info)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o draw_info: The draw info.
%
%
*/

static inline unsigned int IsPoint(const char *point)
{
  char
    *p;

  (void) strtol(point,&p,10);
  return(p != point);
}

MagickExport unsigned int DrawImage(Image *image,const DrawInfo *draw_info)
{
#define RenderImageText  "[%s] Render image..."

  AffineMatrix
    affine,
    current;

  char
    key[2*MaxTextExtent],
    keyword[MaxTextExtent],
    geometry[MaxTextExtent],
    name[MaxTextExtent],
    pattern[MaxTextExtent],
    *primitive,
    *q,
    *token;

  double
    alpha,
    beta,
    angle,
    factor,
    primitive_extent,
    radius;

  DrawInfo
    **graphic_context;

  long
    j,
    k,
    n;

  PointInfo
    point;

  PixelPacket
    start_color;

  PrimitiveInfo
    *primitive_info;

  PrimitiveType
    primitive_type;

  register char
    *p;

  register long
    i,
    x;

  SegmentInfo
    bounds;

  size_t
    length;

  unsigned int
    status;

  unsigned long
    number_points;

  /*
    Ensure the annotation info is valid.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(draw_info != (DrawInfo *) NULL);
  assert(draw_info->signature == MagickSignature);
  assert(draw_info->primitive != (char *) NULL);
  if (*draw_info->primitive == '\0')
    return(False);
  (void) LogMagickEvent(RenderEvent,GetMagickModule(),"begin draw-image");
  if (*draw_info->primitive != '@')
    primitive=AllocateString(draw_info->primitive);
  else
    primitive=(char *)
      FileToBlob(draw_info->primitive+1,&length,&image->exception);
  if (primitive == (char *) NULL)
    return(False);
  primitive_extent=strlen(primitive);
  (void) SetImageAttribute(image,"[MVG]",primitive);
  n=0;
  /*
    Allocate primitive info memory.
  */
  graphic_context=MagickAllocateMemory(DrawInfo **,sizeof(DrawInfo *));
  if (graphic_context == (DrawInfo **) NULL)
    {
      MagickFreeMemory(primitive);
      ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
        UnableToDrawOnImage)
    }
  number_points=2047;
  primitive_info=MagickAllocateMemory(PrimitiveInfo *,
    number_points*sizeof(PrimitiveInfo));
  if (primitive_info == (PrimitiveInfo *) NULL)
    {
      MagickFreeMemory(primitive);
      for ( ; n >= 0; n--)
        DestroyDrawInfo(graphic_context[n]);
      MagickFreeMemory(graphic_context);
      ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
        UnableToDrawOnImage)
    }
  graphic_context[n]=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  token=AllocateString(primitive);
  (void) QueryColorDatabase("black",&start_color,&image->exception);
  (void) SetImageType(image,TrueColorType);
  status=True;
  for (q=primitive; *q != '\0'; )
  {
    /*
      Interpret graphic primitive.
    */
    GetToken(q,&q,keyword);
    if (*keyword == '\0')
      break;
    if (*keyword == '#')
      {
        /*
          Comment.
        */
        while ((*q != '\n') && (*q != '\0'))
          q++;
        continue;
      }
    p=q-strlen(keyword);
    primitive_type=UndefinedPrimitive;
    current=graphic_context[n]->affine;
    IdentityAffine(&affine);
    switch (*keyword)
    {
      case ';':
        break;
      case 'a':
      case 'A':
      {
        if (LocaleCompare("affine",keyword) == 0)
          {
            GetToken(q,&q,token);
            affine.sx=atof(token);
            GetToken(q,&q,token);
            if (*token == ',')
              GetToken(q,&q,token);
            affine.rx=atof(token);
            GetToken(q,&q,token);
            if (*token == ',')
              GetToken(q,&q,token);
            affine.ry=atof(token);
            GetToken(q,&q,token);
            if (*token == ',')
              GetToken(q,&q,token);
            affine.sy=atof(token);
            GetToken(q,&q,token);
            if (*token == ',')
              GetToken(q,&q,token);
            affine.tx=atof(token);
            GetToken(q,&q,token);
            if (*token == ',')
              GetToken(q,&q,token);
            affine.ty=atof(token);
            break;
          }
        if (LocaleCompare("arc",keyword) == 0)
          {
            primitive_type=ArcPrimitive;
            break;
          }
        status=False;
        break;
      }
      case 'b':
      case 'B':
      {
        if (LocaleCompare("bezier",keyword) == 0)
          {
            primitive_type=BezierPrimitive;
            break;
          }
        status=False;
        break;
      }
      case 'c':
      case 'C':
      {
        if (LocaleCompare("clip-path",keyword) == 0)
          {
            /*
              Create clip mask.
            */
            GetToken(q,&q,token);
            (void) CloneString(&graphic_context[n]->clip_path,token);
            (void) DrawClipPath(image,graphic_context[n],
              graphic_context[n]->clip_path);
            break;
          }
        if (LocaleCompare("clip-rule",keyword) == 0)
          {
            GetToken(q,&q,token);
            if (LocaleCompare("evenodd",token) == 0)
              {
                graphic_context[n]->fill_rule=EvenOddRule;
                break;
              }
            if (LocaleCompare("nonzero",token) == 0)
              {
                graphic_context[n]->fill_rule=NonZeroRule;
                break;
              }
            status=False;
            break;
          }
        if (LocaleCompare("clip-units",keyword) == 0)
          {
            GetToken(q,&q,token);
            if (LocaleCompare("userSpace",token) == 0)
              {
                graphic_context[n]->clip_units=UserSpace;
                break;
              }
            if (LocaleCompare("userSpaceOnUse",token) == 0)
              {
                graphic_context[n]->clip_units=UserSpaceOnUse;
                break;
              }
            if (LocaleCompare("objectBoundingBox",token) == 0)
              {
                graphic_context[n]->clip_units=ObjectBoundingBox;
                IdentityAffine(&current);
                affine.sx=draw_info->bounds.x2;
                affine.sy=draw_info->bounds.y2;
                affine.tx=draw_info->bounds.x1;
                affine.ty=draw_info->bounds.y1;
                break;
              }
            status=False;
            break;
          }
        if (LocaleCompare("circle",keyword) == 0)
          {
            primitive_type=CirclePrimitive;
            break;
          }
        if (LocaleCompare("color",keyword) == 0)
          {
            primitive_type=ColorPrimitive;
            break;
          }
        status=False;
        break;
      }
      case 'd':
      case 'D':
      {
        if (LocaleCompare("decorate",keyword) == 0)
          {
            GetToken(q,&q,token);
            if (LocaleCompare("underline",token) == 0)
              {
                graphic_context[n]->decorate=UnderlineDecoration;
                break;
              }
            if (LocaleCompare("overline",token) == 0)
              {
                graphic_context[n]->decorate=OverlineDecoration;
                break;
              }
            if (LocaleCompare("line-through",token) == 0)
              {
                graphic_context[n]->decorate=LineThroughDecoration;
                break;
              }
            status=False;
            break;
          }
        status=False;
        break;
      }
      case 'e':
      case 'E':
      {
        if (LocaleCompare("ellipse",keyword) == 0)
          {
            primitive_type=EllipsePrimitive;
            break;
          }
        if (LocaleCompare("encoding",keyword) == 0)
          {
            GetToken(q,&q,token);
            (void) CloneString(&graphic_context[n]->encoding,token);
            break;
          }
        status=False;
        break;
      }
      case 'f':
      case 'F':
      {
        if (LocaleCompare("fill",keyword) == 0)
          {
            GetToken(q,&q,token);
            FormatString(pattern,"[%.1024s]",token);
            if (GetImageAttribute(image,pattern) == (ImageAttribute *) NULL)
              (void) QueryColorDatabase(token,&graphic_context[n]->fill,
                &image->exception);
            else
              (void) DrawPatternPath(image,draw_info,token,
                &graphic_context[n]->fill_pattern);
            if (graphic_context[n]->fill.opacity != TransparentOpacity)
              graphic_context[n]->fill.opacity=graphic_context[n]->opacity;
            break;
          }
        if (LocaleCompare("fill-rule",keyword) == 0)
          {
            GetToken(q,&q,token);
            if (LocaleCompare("evenodd",token) == 0)
              {
                graphic_context[n]->fill_rule=EvenOddRule;
                break;
              }
            if (LocaleCompare("nonzero",token) == 0)
              {
                graphic_context[n]->fill_rule=NonZeroRule;
                break;
              }
            status=False;
            break;
          }
        if (LocaleCompare("fill-opacity",keyword) == 0)
          {
            GetToken(q,&q,token);
            factor=strchr(token,'%') != (char *) NULL ? 0.01 : 1.0;
            if (graphic_context[n]->fill.opacity != TransparentOpacity)
              graphic_context[n]->fill.opacity=(Quantum)
                ((double) MaxRGB*(1.0-factor*atof(token)));
            break;
          }
        if (LocaleCompare("font",keyword) == 0)
          {
            GetToken(q,&q,token);
            (void) CloneString(&graphic_context[n]->font,token);
            if (LocaleCompare("none",token) == 0)
              MagickFreeMemory(graphic_context[n]->font);
            break;
          }
        if (LocaleCompare("font-family",keyword) == 0)
          {
            GetToken(q,&q,token);
            (void) CloneString(&graphic_context[n]->family,token);
            break;
          }
        if (LocaleCompare("font-size",keyword) == 0)
          {
            GetToken(q,&q,token);
            graphic_context[n]->pointsize=atof(token);
            break;
          }
        if (LocaleCompare("font-stretch",keyword) == 0)
          {
            GetToken(q,&q,token);
            if (LocaleCompare(token,"all") == 0)
              graphic_context[n]->stretch=AnyStretch;
            if (LocaleCompare(token,"condensed") == 0)
              graphic_context[n]->stretch=CondensedStretch;
            if (LocaleCompare(token,"expanded") == 0)
              graphic_context[n]->stretch=ExpandedStretch;
            if (LocaleCompare(token,"extra-condensed") == 0)
              graphic_context[n]->stretch=ExtraCondensedStretch;
            if (LocaleCompare(token,"extra-expanded") == 0)
              graphic_context[n]->stretch=ExtraExpandedStretch;
            if (LocaleCompare(token,"normal") == 0)
              graphic_context[n]->stretch=NormalStretch;
            if (LocaleCompare(token,"semi-condensed") == 0)
              graphic_context[n]->stretch=SemiCondensedStretch;
            if (LocaleCompare(token,"semi-expanded") == 0)
              graphic_context[n]->stretch=SemiExpandedStretch;
            if (LocaleCompare(token,"ultra-condensed") == 0)
              graphic_context[n]->stretch=UltraCondensedStretch;
            if (LocaleCompare(token,"ultra-expanded") == 0)
              graphic_context[n]->stretch=UltraExpandedStretch;
            break;
          }
        if (LocaleCompare("font-style",keyword) == 0)
          {
            GetToken(q,&q,token);
            if (LocaleCompare(token,"all") == 0)
              graphic_context[n]->style=AnyStyle;
            if (LocaleCompare(token,"italic") == 0)
              graphic_context[n]->style=ItalicStyle;
            if (LocaleCompare(token,"normal") == 0)
              graphic_context[n]->style=NormalStyle;
            if (LocaleCompare(token,"oblique") == 0)
              graphic_context[n]->style=ObliqueStyle;
            break;
          }
        if (LocaleCompare("font-weight",keyword) == 0)
          {
            GetToken(q,&q,token);
            graphic_context[n]->weight=atol(token);
            if (LocaleCompare(token,"all") == 0)
              graphic_context[n]->weight=0;
            if (LocaleCompare(token,"bold") == 0)
              graphic_context[n]->weight=700;
            if (LocaleCompare(token,"bolder") == 0)
              if (graphic_context[n]->weight <= 800)
                graphic_context[n]->weight+=100;
            if (LocaleCompare(token,"lighter") == 0)
              if (graphic_context[n]->weight >= 100)
                graphic_context[n]->weight-=100;
            if (LocaleCompare(token,"normal") == 0)
              graphic_context[n]->weight=400;
            break;
          }
        status=False;
        break;
      }
      case 'g':
      case 'G':
      {
        if (LocaleCompare("gradient-units",keyword) == 0)
          {
            GetToken(q,&q,token);
            break;
          }
        if (LocaleCompare("gravity",keyword) == 0)
          {
            GetToken(q,&q,token);
            if (LocaleCompare("NorthWest",token) == 0)
              {
                graphic_context[n]->gravity=NorthWestGravity;
                break;
              }
            if (LocaleCompare("North",token) == 0)
              {
                graphic_context[n]->gravity=NorthGravity;
                break;
              }
            if (LocaleCompare("NorthEast",token) == 0)
              {
                graphic_context[n]->gravity=NorthEastGravity;
                break;
              }
            if (LocaleCompare("West",token) == 0)
              {
                graphic_context[n]->gravity=WestGravity;
                break;
              }
            if (LocaleCompare("Center",token) == 0)
              {
                graphic_context[n]->gravity=CenterGravity;
                break;
              }
            if (LocaleCompare("East",token) == 0)
              {
                graphic_context[n]->gravity=EastGravity;
                break;
              }
            if (LocaleCompare("SouthWest",token) == 0)
              {
                graphic_context[n]->gravity=SouthWestGravity;
                break;
              }
            if (LocaleCompare("South",token) == 0)
              {
                graphic_context[n]->gravity=SouthGravity;
                break;
              }
            if (LocaleCompare("SouthEast",token) == 0)
              {
                graphic_context[n]->gravity=SouthEastGravity;
                break;
              }
            status=False;
            break;
          }
        status=False;
        break;
      }
      case 'i':
      case 'I':
      {
        if (LocaleCompare("image",keyword) == 0)
          {
            primitive_type=ImagePrimitive;
            GetToken(q,&q,token);
            graphic_context[n]->compose=StringToCompositeOperator(token);
            if (UndefinedCompositeOp == graphic_context[n]->compose)
              status=False;
            break;
          }
        status=False;
        break;
      }
      case 'l':
      case 'L':
      {
        if (LocaleCompare("line",keyword) == 0)
          {
            primitive_type=LinePrimitive;
            break;
          }
        status=False;
        break;
      }
      case 'm':
      case 'M':
      {
        if (LocaleCompare("matte",keyword) == 0)
          {
            primitive_type=MattePrimitive;
            break;
          }
        status=False;
        break;
      }
      case 'o':
      case 'O':
      {
        if (LocaleCompare("offset",keyword) == 0)
          {
            GetToken(q,&q,token);
            break;
          }
        if (LocaleCompare("opacity",keyword) == 0)
          {
            GetToken(q,&q,token);
            factor=strchr(token,'%') != (char *) NULL ? 0.01 : 1.0;
            graphic_context[n]->opacity=(Quantum) ((double) MaxRGB*(1.0-((1.0-
              graphic_context[n]->opacity/MaxRGB)*factor*atof(token)))+0.5);
            graphic_context[n]->fill.opacity=graphic_context[n]->opacity;
            graphic_context[n]->stroke.opacity=graphic_context[n]->opacity;
            break;
          }
        status=False;
        break;
      }
      case 'p':
      case 'P':
      {
        if (LocaleCompare("path",keyword) == 0)
          {
            primitive_type=PathPrimitive;
            break;
          }
        if (LocaleCompare("point",keyword) == 0)
          {
            primitive_type=PointPrimitive;
            break;
          }
        if (LocaleCompare("polyline",keyword) == 0)
          {
            primitive_type=PolylinePrimitive;
            break;
          }
        if (LocaleCompare("polygon",keyword) == 0)
          {
            primitive_type=PolygonPrimitive;
            break;
          }
        if (LocaleCompare("pop",keyword) == 0)
          {
            GetToken(q,&q,token);
            if (LocaleCompare("clip-path",token) == 0)
              break;
            if (LocaleCompare("defs",token) == 0)
              break;
            if (LocaleCompare("gradient",token) == 0)
              break;
            if (LocaleCompare("graphic-context",token) == 0)
              {
                if (n == 0)
                  {
                    ThrowException(&image->exception,DrawError,UnbalancedGraphicContextPushPop,token);
                    break;
                  }
                if (graphic_context[n]->clip_path != (char *) NULL)
                  if (LocaleCompare(graphic_context[n]->clip_path,
                      graphic_context[n-1]->clip_path) != 0)
                    (void) SetImageClipMask(image,(Image *) NULL);
                DestroyDrawInfo(graphic_context[n]);
                n--;
                break;
              }
            if (LocaleCompare("pattern",token) == 0)
              break;
            status=False;
            break;
          }
        if (LocaleCompare("push",keyword) == 0)
          {
            GetToken(q,&q,token);
            if (LocaleCompare("clip-path",token) == 0)
              {
                char
                  name[MaxTextExtent];

                GetToken(q,&q,token);
                FormatString(name,"[%.1024s]",token);
                for (p=q; *q != '\0'; )
                {
                  GetToken(q,&q,token);
                  if (LocaleCompare(token,"pop") != 0)
                    continue;
                  GetToken(q,(char **) NULL,token);
                  if (LocaleCompare(token,"clip-path") != 0)
                    continue;
                  break;
                }
                (void) strncpy(token,p,q-p-4);
                token[q-p-4]='\0';
                (void) SetImageAttribute(image,name,token);
                GetToken(q,&q,token);
                break;
              }
            if (LocaleCompare("gradient",token) == 0)
              {
                char
                  key[2*MaxTextExtent],
                  name[MaxTextExtent],
                  type[MaxTextExtent];

/*                 ElementInfo */
/*                   element; */

                SegmentInfo
                  segment;

                GetToken(q,&q,token);
                (void) strlcpy(name,token,MaxTextExtent);
                GetToken(q,&q,token);
                (void) strlcpy(type,token,MaxTextExtent);
                GetToken(q,&q,token);
                segment.x1=atof(token);
/*                 element.cx=atof(token); */
                GetToken(q,&q,token);
                if (*token == ',')
                  GetToken(q,&q,token);
                segment.y1=atof(token);
/*                 element.cy=atof(token); */
                GetToken(q,&q,token);
                if (*token == ',')
                  GetToken(q,&q,token);
                segment.x2=atof(token);
/*                 element.major=atof(token); */
                GetToken(q,&q,token);
                if (*token == ',')
                  GetToken(q,&q,token);
                segment.y2=atof(token);
/*                 element.minor=atof(token); */
                if (LocaleCompare(type,"radial") == 0)
                  {
                    GetToken(q,&q,token);
                    if (*token == ',')
                      GetToken(q,&q,token);
/*                     element.angle=atof(token); */
                  }
                for (p=q; *q != '\0'; )
                {
                  GetToken(q,&q,token);
                  if (LocaleCompare(token,"pop") != 0)
                    continue;
                  GetToken(q,(char **) NULL,token);
                  if (LocaleCompare(token,"gradient") != 0)
                    continue;
                  break;
                }
                (void) strncpy(token,p,q-p-4);
                token[q-p-4]='\0';
                bounds.x1=graphic_context[n]->affine.sx*segment.x1+
                  graphic_context[n]->affine.ry*segment.y1+
                  graphic_context[n]->affine.tx;
                bounds.y1=graphic_context[n]->affine.rx*segment.x1+
                  graphic_context[n]->affine.sy*segment.y1+
                  graphic_context[n]->affine.ty;
                bounds.x2=graphic_context[n]->affine.sx*segment.x2+
                  graphic_context[n]->affine.ry*segment.y2+
                  graphic_context[n]->affine.tx;
                bounds.y2=graphic_context[n]->affine.rx*segment.x2+
                  graphic_context[n]->affine.sy*segment.y2+
                  graphic_context[n]->affine.ty;
                FormatString(key,"[%.1024s]",name);
                (void) SetImageAttribute(image,key,token);
                FormatString(key,"[%.1024s-geometry]",name);
                FormatString(geometry,"%gx%g%+g%+g",
                  Max(AbsoluteValue(bounds.x2-bounds.x1+1),1),
                  Max(AbsoluteValue(bounds.y2-bounds.y1+1),1),
                  bounds.x1,bounds.y1);
                (void) SetImageAttribute(image,key,geometry);
                GetToken(q,&q,token);
                break;
              }
            if (LocaleCompare("pattern",token) == 0)
              {
                RectangleInfo
                  bounds;

                GetToken(q,&q,token);
                (void) strlcpy(name,token,MaxTextExtent);
                GetToken(q,&q,token);
                bounds.x=(long) ceil(atof(token)-0.5);
                GetToken(q,&q,token);
                if (*token == ',')
                  GetToken(q,&q,token);
                bounds.y=(long) ceil(atof(token)-0.5);
                GetToken(q,&q,token);
                if (*token == ',')
                  GetToken(q,&q,token);
                bounds.width=(unsigned long) floor(atof(token)+0.5);
                GetToken(q,&q,token);
                if (*token == ',')
                  GetToken(q,&q,token);
                bounds.height=(unsigned long) floor(atof(token)+0.5);
                for (p=q; *q != '\0'; )
                {
                  GetToken(q,&q,token);
                  if (LocaleCompare(token,"pop") != 0)
                    continue;
                  GetToken(q,(char **) NULL,token);
                  if (LocaleCompare(token,"pattern") != 0)
                    continue;
                  break;
                }
                (void) strncpy(token,p,q-p-4);
                token[q-p-4]='\0';
                FormatString(key,"[%.1024s]",name);
                (void) SetImageAttribute(image,key,token);
                FormatString(key,"[%.1024s-geometry]",name);
                FormatString(geometry,"%lux%lu%+ld%+ld",bounds.width,
                  bounds.height,bounds.x,bounds.y);
                (void) SetImageAttribute(image,key,geometry);
                GetToken(q,&q,token);
                break;
              }
            if (LocaleCompare("graphic-context",token) == 0)
              {
                n++;
                MagickReallocMemory(DrawInfo **,graphic_context,
                  (n+1)*sizeof(DrawInfo *));
                if (graphic_context == (DrawInfo **) NULL)
                  {
                    ThrowException3(&image->exception,ResourceLimitError,
                      MemoryAllocationFailed,UnableToDrawOnImage);
                    break;
                  }
                graphic_context[n]=
                  CloneDrawInfo((ImageInfo *) NULL,graphic_context[n-1]);
                break;
              }
            if (LocaleCompare("defs",token) == 0)
              break;
            status=False;
            break;
          }
        status=False;
        break;
      }
      case 'r':
      case 'R':
      {
        if (LocaleCompare("rectangle",keyword) == 0)
          {
            primitive_type=RectanglePrimitive;
            break;
          }
        if (LocaleCompare("rotate",keyword) == 0)
          {
            GetToken(q,&q,token);
            angle=atof(token);
            affine.sx=cos(DegreesToRadians(fmod(angle,360.0)));
            affine.rx=sin(DegreesToRadians(fmod(angle,360.0)));
            affine.ry=(-sin(DegreesToRadians(fmod(angle,360.0))));
            affine.sy=cos(DegreesToRadians(fmod(angle,360.0)));
            break;
          }
        if (LocaleCompare("roundRectangle",keyword) == 0)
          {
            primitive_type=RoundRectanglePrimitive;
            break;
          }
        status=False;
        break;
      }
      case 's':
      case 'S':
      {
        if (LocaleCompare("scale",keyword) == 0)
          {
            GetToken(q,&q,token);
            affine.sx=atof(token);
            GetToken(q,&q,token);
            if (*token == ',')
              GetToken(q,&q,token);
            affine.sy=atof(token);
            break;
          }
        if (LocaleCompare("skewX",keyword) == 0)
          {
            GetToken(q,&q,token);
            angle=atof(token);
            affine.ry=tan(DegreesToRadians(fmod(angle,360.0)));
            break;
          }
        if (LocaleCompare("skewY",keyword) == 0)
          {
            GetToken(q,&q,token);
            angle=atof(token);
            affine.rx=tan(DegreesToRadians(fmod(angle,360.0)));
            break;
          }
        if (LocaleCompare("stop-color",keyword) == 0)
          {
            PixelPacket
              stop_color;

            GetToken(q,&q,token);
            (void) QueryColorDatabase(token,&stop_color,&image->exception);
            (void) GradientImage(image,&start_color,&stop_color);
            start_color=stop_color;
            GetToken(q,&q,token);
            break;
          }
        if (LocaleCompare("stroke",keyword) == 0)
          {
            GetToken(q,&q,token);
            FormatString(pattern,"[%.1024s]",token);
            if (GetImageAttribute(image,pattern) == (ImageAttribute *) NULL)
              (void) QueryColorDatabase(token,&graphic_context[n]->stroke,
                &image->exception);
            else
              (void) DrawPatternPath(image,draw_info,token,
                &graphic_context[n]->stroke_pattern);
            if (graphic_context[n]->stroke.opacity != TransparentOpacity)
              graphic_context[n]->stroke.opacity=graphic_context[n]->opacity;
            break;
          }
        if (LocaleCompare("stroke-antialias",keyword) == 0)
          {
            GetToken(q,&q,token);
            graphic_context[n]->stroke_antialias=atoi(token);
            break;
          }
        if (LocaleCompare("stroke-dasharray",keyword) == 0)
          {
            if (graphic_context[n]->dash_pattern != (double *) NULL)
              MagickFreeMemory(graphic_context[n]->dash_pattern);
            if (IsPoint(q))
              {
                char
                  *p;

                p=q;
                GetToken(p,&p,token);
                if (*token == ',')
                  GetToken(p,&p,token);
                for (x=0; IsPoint(token); x++)
                {
                  GetToken(p,&p,token);
                  if (*token == ',')
                    GetToken(p,&p,token);
                }
                graphic_context[n]->dash_pattern=MagickAllocateMemory(double *,
                  (2*x+1)*sizeof(double));
                if (graphic_context[n]->dash_pattern == (double *) NULL)
                  {
                    ThrowException3(&image->exception,ResourceLimitError,
                      MemoryAllocationFailed,UnableToDrawOnImage);
                    break;
                  }
                for (j=0; j < x; j++)
                {
                  GetToken(q,&q,token);
                  if (*token == ',')
                    GetToken(q,&q,token);
                  graphic_context[n]->dash_pattern[j]=atof(token);
                }
                if (x & 0x01)
                  for ( ; j < (2*x); j++)
                    graphic_context[n]->dash_pattern[j]=
                      graphic_context[n]->dash_pattern[j-x];
                graphic_context[n]->dash_pattern[j]=0.0;
                break;
              }
            GetToken(q,&q,token);
            break;
          }
        if (LocaleCompare("stroke-dashoffset",keyword) == 0)
          {
            GetToken(q,&q,token);
            graphic_context[n]->dash_offset=atof(token);
            break;
          }
        if (LocaleCompare("stroke-linecap",keyword) == 0)
          {
            GetToken(q,&q,token);
            if (LocaleCompare("butt",token) == 0)
              {
                graphic_context[n]->linecap=ButtCap;
                break;
              }
            if (LocaleCompare("round",token) == 0)
              {
                graphic_context[n]->linecap=RoundCap;
                break;
              }
            if (LocaleCompare("square",token) == 0)
              {
                graphic_context[n]->linecap=SquareCap;
                break;
              }
            status=False;
            break;
          }
        if (LocaleCompare("stroke-linejoin",keyword) == 0)
          {
            GetToken(q,&q,token);
            if (LocaleCompare("bevel",token) == 0)
              {
                graphic_context[n]->linejoin=BevelJoin;
                break;
              }
            if (LocaleCompare("miter",token) == 0)
              {
                graphic_context[n]->linejoin=MiterJoin;
                break;
              }
            if (LocaleCompare("round",token) == 0)
              {
                graphic_context[n]->linejoin=RoundJoin;
                break;
              }
            status=False;
            break;
          }
        if (LocaleCompare("stroke-miterlimit",keyword) == 0)
          {
            GetToken(q,&q,token);
            graphic_context[n]->miterlimit=atol(token);
            break;
          }
        if (LocaleCompare("stroke-opacity",keyword) == 0)
          {
            GetToken(q,&q,token);
            factor=strchr(token,'%') != (char *) NULL ? 0.01 : 1.0;
            if (graphic_context[n]->stroke.opacity != TransparentOpacity)
              graphic_context[n]->stroke.opacity=(Quantum)
                ((double) MaxRGB*(1.0-factor*atof(token)));
            break;
          }
        if (LocaleCompare("stroke-width",keyword) == 0)
          {
            GetToken(q,&q,token);
            graphic_context[n]->stroke_width=atof(token);
            break;
          }
        status=False;
        break;
      }
      case 't':
      case 'T':
      {
        if (LocaleCompare("text",keyword) == 0)
          {
            primitive_type=TextPrimitive;
            break;
          }
        if (LocaleCompare("text-align",keyword) == 0)
          {
            GetToken(q,&q,token);
            if (LocaleCompare(token,"left") == 0)
              graphic_context[n]->align=LeftAlign;
            if (LocaleCompare(token,"center") == 0)
              graphic_context[n]->align=CenterAlign;
            if (LocaleCompare(token,"right") == 0)
              graphic_context[n]->align=RightAlign;
            break;
          }
        if (LocaleCompare("text-anchor",keyword) == 0)
          {
            GetToken(q,&q,token);
            if (LocaleCompare(token,"start") == 0)
              graphic_context[n]->align=LeftAlign;
            if (LocaleCompare(token,"middle") == 0)
              graphic_context[n]->align=CenterAlign;
            if (LocaleCompare(token,"end") == 0)
              graphic_context[n]->align=RightAlign;
            break;
          }
        if (LocaleCompare("text-antialias",keyword) == 0)
          {
            GetToken(q,&q,token);
            graphic_context[n]->text_antialias=atoi(token);
            break;
          }
        if (LocaleCompare("text-undercolor",keyword) == 0)
          {
            GetToken(q,&q,token);
            (void) QueryColorDatabase(token,&graphic_context[n]->undercolor,
              &image->exception);
            break;
          }
        if (LocaleCompare("translate",keyword) == 0)
          {
            GetToken(q,&q,token);
            affine.tx=atof(token);
            GetToken(q,&q,token);
            if (*token == ',')
              GetToken(q,&q,token);
            affine.ty=atof(token);
            break;
          }
        status=False;
        break;
      }
      case 'v':
      case 'V':
      {
        if (LocaleCompare("viewbox",keyword) == 0)
          {
            GetToken(q,&q,token);
            GetToken(q,&q,token);
            if (*token == ',')
              GetToken(q,&q,token);
            GetToken(q,&q,token);
            if (*token == ',')
              GetToken(q,&q,token);
            GetToken(q,&q,token);
            if (*token == ',')
              GetToken(q,&q,token);
            break;
          }
        status=False;
        break;
      }
      default:
      {
        status=False;
        break;
      }
    }
    if (status == False)
      break;
    if ((affine.sx != 1.0) || (affine.rx != 0.0) || (affine.ry != 0.0) ||
        (affine.sy != 1.0) || (affine.tx != 0.0) || (affine.ty != 0.0))
      {
        graphic_context[n]->affine.sx=current.sx*affine.sx+current.ry*affine.rx;
        graphic_context[n]->affine.rx=current.rx*affine.sx+current.sy*affine.rx;
        graphic_context[n]->affine.ry=current.sx*affine.ry+current.ry*affine.sy;
        graphic_context[n]->affine.sy=current.rx*affine.ry+current.sy*affine.sy;
        graphic_context[n]->affine.tx=
          current.sx*affine.tx+current.ry*affine.ty+current.tx;
        graphic_context[n]->affine.ty=
          current.rx*affine.tx+current.sy*affine.ty+current.ty;
      }
    if (primitive_type == UndefinedPrimitive)
      {
        (void) LogMagickEvent(RenderEvent,GetMagickModule(),"  %.*s",
          (int) (q-p),p);
        continue;
      }
    /*
      Parse the primitive attributes.
    */
    i=0;
    j=0;
    for (x=0; *q != '\0'; x++)
    {
      /*
        Define points.
      */
      if (!IsPoint(q))
        break;
      GetToken(q,&q,token);
      point.x=atof(token);
      GetToken(q,&q,token);
      if (*token == ',')
        GetToken(q,&q,token);
      point.y=atof(token);
      GetToken(q,(char **) NULL,token);
      if (*token == ',')
        GetToken(q,&q,token);
      primitive_info[i].primitive=primitive_type;
      primitive_info[i].point=point;
      primitive_info[i].coordinates=0;
      primitive_info[i].method=FloodfillMethod;
      i++;
      if (i < (long) number_points)
        continue;
      number_points<<=1;
      MagickReallocMemory(PrimitiveInfo *,primitive_info,
        number_points*sizeof(PrimitiveInfo));
      if (primitive_info == (PrimitiveInfo *) NULL)
        {
          ThrowException3(&image->exception,ResourceLimitError,
            MemoryAllocationFailed,UnableToDrawOnImage);
          break;
        }
    }
    primitive_info[j].primitive=primitive_type;
    primitive_info[j].coordinates=x;
    primitive_info[j].method=FloodfillMethod;
    primitive_info[j].text=(char *) NULL;
    /*
      Circumscribe primitive within a circle.
    */
    bounds.x1=primitive_info[j].point.x;
    bounds.y1=primitive_info[j].point.y;
    bounds.x2=primitive_info[j].point.x;
    bounds.y2=primitive_info[j].point.y;
    for (k=1; k < (long) primitive_info[j].coordinates; k++)
    {
      point=primitive_info[j+k].point;
      if (point.x < bounds.x1)
        bounds.x1=point.x;
      if (point.y < bounds.y1)
        bounds.y1=point.y;
      if (point.x > bounds.x2)
        bounds.x2=point.x;
      if (point.y > bounds.y2)
        bounds.y2=point.y;
    }
    alpha=bounds.x2-bounds.x1;
    beta=bounds.y2-bounds.y1;
    radius=sqrt(alpha*alpha+beta*beta);
    length=(size_t) (2*ceil(MagickPI*radius)+6*BezierQuantum+360);
    if (i >= (long) (number_points-length))
      {
        number_points+=length;
        MagickReallocMemory(PrimitiveInfo *,primitive_info,
          number_points*sizeof(PrimitiveInfo));
        if (primitive_info == (PrimitiveInfo *) NULL)
          {
            ThrowException3(&image->exception,ResourceLimitError,
              MemoryAllocationFailed,UnableToDrawOnImage);
            break;
          }
      }
    switch (primitive_type)
    {
      case PointPrimitive:
      default:
      {
        if (primitive_info[j].coordinates != 1)
          {
            status=False;
            break;
          }
        TracePoint(primitive_info+j,primitive_info[j].point);
        i=(long) (j+primitive_info[j].coordinates);
        break;
      }
      case LinePrimitive:
      {
        if (primitive_info[j].coordinates != 2)
          {
            status=False;
            break;
          }
        TraceLine(primitive_info+j,primitive_info[j].point,
          primitive_info[j+1].point);
        i=(long) (j+primitive_info[j].coordinates);
        break;
      }
      case RectanglePrimitive:
      {
        if (primitive_info[j].coordinates != 2)
          {
            status=False;
            break;
          }
        TraceRectangle(primitive_info+j,primitive_info[j].point,
          primitive_info[j+1].point);
        i=(long) (j+primitive_info[j].coordinates);
        break;
      }
      case RoundRectanglePrimitive:
      {
        if (primitive_info[j].coordinates != 3)
          {
            status=False;
            break;
          }
        TraceRoundRectangle(primitive_info+j,primitive_info[j].point,
          primitive_info[j+1].point,primitive_info[j+2].point);
        i=(long) (j+primitive_info[j].coordinates);
        break;
      }
      case ArcPrimitive:
      {
        if (primitive_info[j].coordinates != 3)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        TraceArc(primitive_info+j,primitive_info[j].point,
          primitive_info[j+1].point,primitive_info[j+2].point);
        i=(long) (j+primitive_info[j].coordinates);
        break;
      }
      case EllipsePrimitive:
      {
        if (primitive_info[j].coordinates != 3)
          {
            status=False;
            break;
          }
        TraceEllipse(primitive_info+j,primitive_info[j].point,
          primitive_info[j+1].point,primitive_info[j+2].point);
        i=(long) (j+primitive_info[j].coordinates);
        break;
      }
      case CirclePrimitive:
      {
        if (primitive_info[j].coordinates != 2)
          {
            status=False;
            break;
          }
        TraceCircle(primitive_info+j,primitive_info[j].point,
          primitive_info[j+1].point);
        i=(long) (j+primitive_info[j].coordinates);
        break;
      }
      case PolylinePrimitive:
      {
        if (primitive_info[j].coordinates < 2)
          {
            status=False;
            break;
          }
        break;
      }
      case PolygonPrimitive:
      {
        if (primitive_info[j].coordinates < 3)
          {
            status=False;
            break;
          }
        primitive_info[i]=primitive_info[j];
        primitive_info[i].coordinates=0;
        primitive_info[j].coordinates++;
        i++;
        break;
      }
      case BezierPrimitive:
      {
        if (primitive_info[j].coordinates < 3)
          {
            status=False;
            break;
          }
        TraceBezier(primitive_info+j,primitive_info[j].coordinates);
        i=(long) (j+primitive_info[j].coordinates);
        break;
      }
      case PathPrimitive:
      {
        char
          *s,
          *t;

        GetToken(q,&q,token);
        length=1;
        t=token;
        for (s=token; *s != '\0'; s=t)
        {
          (void) strtod(s,&t);
          if (s == t)
            {
              t++;
              continue;
            }
          length++;
        }
        if (i > (long) (number_points-6*BezierQuantum*length/3-1))
          {
            number_points+=6*BezierQuantum*length/3;
            MagickReallocMemory(PrimitiveInfo *,primitive_info,
              number_points*sizeof(PrimitiveInfo));
            if (primitive_info == (PrimitiveInfo *) NULL)
              {
                ThrowException3(&image->exception,ResourceLimitError,
                  MemoryAllocationFailed,UnableToDrawOnImage);
                break;
              }
          }
        i=(long) (j+TracePath(primitive_info+j,token));
        break;
      }
      case ColorPrimitive:
      case MattePrimitive:
      {
        if (primitive_info[j].coordinates != 1)
          {
            status=False;
            break;
          }
        GetToken(q,&q,token);
        if (LocaleCompare("point",token) == 0)
          primitive_info[j].method=PointMethod;
        if (LocaleCompare("replace",token) == 0)
          primitive_info[j].method=ReplaceMethod;
        if (LocaleCompare("floodfill",token) == 0)
          primitive_info[j].method=FloodfillMethod;
        if (LocaleCompare("filltoborder",token) == 0)
          primitive_info[j].method=FillToBorderMethod;
        if (LocaleCompare("reset",token) == 0)
          primitive_info[j].method=ResetMethod;
        break;
      }
      case TextPrimitive:
      {
        if (primitive_info[j].coordinates != 1)
          {
            status=False;
            break;
          }
        GetToken(q,&q,token);
        primitive_info[j].text=AllocateString(token);
        break;
      }
      case ImagePrimitive:
      {
        if (primitive_info[j].coordinates != 2)
          {
            status=False;
            break;
          }
        GetToken(q,&q,token);
        primitive_info[j].text=AllocateString(token);
        break;
      }
    }
    if (primitive_info == (PrimitiveInfo *) NULL)
      break;
    (void) LogMagickEvent(RenderEvent,GetMagickModule(),"  %.*s",(int) (q-p),p);
    if (status == False)
      break;
    primitive_info[i].primitive=UndefinedPrimitive;
    if (i == 0)
      continue;
    /*
      Transform points.
    */
    for (i=0; primitive_info[i].primitive != UndefinedPrimitive; i++)
    {
      point=primitive_info[i].point;
      primitive_info[i].point.x=graphic_context[n]->affine.sx*point.x+
        graphic_context[n]->affine.ry*point.y+graphic_context[n]->affine.tx;
      primitive_info[i].point.y=graphic_context[n]->affine.rx*point.x+
        graphic_context[n]->affine.sy*point.y+graphic_context[n]->affine.ty;
      point=primitive_info[i].point;
      if (point.x < graphic_context[n]->bounds.x1)
        graphic_context[n]->bounds.x1=point.x;
      if (point.y < graphic_context[n]->bounds.y1)
        graphic_context[n]->bounds.y1=point.y;
      if (point.x > graphic_context[n]->bounds.x2)
        graphic_context[n]->bounds.x2=point.x;
      if (point.y > graphic_context[n]->bounds.y2)
        graphic_context[n]->bounds.y2=point.y;
      if (primitive_info[i].primitive == ImagePrimitive)
        break;
    }
    if (graphic_context[n]->render)
      {
        if ((n != 0) && (graphic_context[n]->clip_path != (char *) NULL) &&
            (LocaleCompare(graphic_context[n]->clip_path,
             graphic_context[n-1]->clip_path) != 0))
          (void) DrawClipPath(image,graphic_context[n],
            graphic_context[n]->clip_path);
        (void) DrawPrimitive(image,graphic_context[n],primitive_info);
      }
    if (primitive_info->text != (char *) NULL)
      MagickFreeMemory(primitive_info->text);
    status=MagickMonitorFormatted(q-primitive,
                                  (magick_uint64_t) primitive_extent,
                                  &image->exception,
                                  RenderImageText,image->filename);
    if (status == False)
      break;
  }
  (void) LogMagickEvent(RenderEvent,GetMagickModule(),"end draw-image");
  /*
    Free resources.
  */
  MagickFreeMemory(token);
  if (primitive_info != (PrimitiveInfo *) NULL)
    MagickFreeMemory(primitive_info);
  MagickFreeMemory(primitive);
  for ( ; n >= 0; n--)
    DestroyDrawInfo(graphic_context[n]);
  MagickFreeMemory(graphic_context);
  if (status == False)
    ThrowBinaryException(DrawError,NonconformingDrawingPrimitiveDefinition,
      keyword);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P a t t e r n P a t h                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DrawPatternPath() draws a pattern.
%
%  The format of the DrawPatternPath method is:
%
%      unsigned int DrawPatternPath(Image *image,const DrawInfo *draw_info,
%        const char *name,Image **pattern)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o draw_info: The draw info.
%
%    o name: The pattern name.
%
%    o image: The image.
%
%
*/
MagickExport unsigned int DrawPatternPath(Image *image,
  const DrawInfo *draw_info,const char *name,Image **pattern)
{
  char
    attribute[MaxTextExtent];

  const ImageAttribute
    *geometry,
    *path;

  DrawInfo
    *clone_info;

  ImageInfo
    *image_info;

  unsigned int
    status;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(draw_info != (const DrawInfo *) NULL);
  assert(name != (const char *) NULL);
  FormatString(attribute,"[%.1024s]",name);
  path=GetImageAttribute(image,attribute);
  if (path == (ImageAttribute *) NULL)
    return(False);
  FormatString(attribute,"[%.1024s-geometry]",name);
  geometry=GetImageAttribute(image,attribute);
  if (geometry == (ImageAttribute *) NULL)
    return(False);
  if ((*pattern) != (Image *) NULL)
    DestroyImage(*pattern);
  image_info=CloneImageInfo((ImageInfo *) NULL);
  image_info->size=AllocateString(geometry->value);
  *pattern=AllocateImage(image_info);
  DestroyImageInfo(image_info);
  (void) QueryColorDatabase("none",&(*pattern)->background_color,
    &image->exception);
  (void) SetImage(*pattern,OpaqueOpacity);
  (void) LogMagickEvent(RenderEvent,GetMagickModule(),
    "begin pattern-path %.1024s %.1024s",name,geometry->value);
  clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  clone_info->fill_pattern=(Image *) NULL;
  clone_info->stroke_pattern=(Image *) NULL;
  (void) CloneString(&clone_info->primitive,path->value);
  status=DrawImage(*pattern,clone_info);
  DestroyDrawInfo(clone_info);
  (void) LogMagickEvent(RenderEvent,GetMagickModule(),"end pattern-path");
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D r a w P o l y g o n P r i m i t i v e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DrawPolygonPrimitive draws a polygon on the image.
%
%  The format of the DrawPolygonPrimitive method is:
%
%      unsigned int DrawPolygonPrimitive(Image *image,const DrawInfo *draw_info,
%        const PrimitiveInfo *primitive_info)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o draw_info: The draw info.
%
%    o primitive_info: Specifies a pointer to a PrimitiveInfo structure.
%
%
*/

static double GetPixelOpacity(PolygonInfo *polygon_info,const double mid,
  const unsigned int fill,const FillRule fill_rule,const long x,const long y,
  double *stroke_opacity)
{
  double
    alpha,
    beta,
    distance,
    subpath_opacity;

  int
    winding_number;

  register double
    dx,
    dy;

  register EdgeInfo
    *p;

  register const PointInfo
    *q;

  register long
    i;

  long
    j;

  /*
    Compute fill & stroke opacity for this (x,y) point.
  */
  *stroke_opacity=0.0;
  subpath_opacity=0.0;
  p=polygon_info->edges;
  for (j=0; j < polygon_info->number_edges; j++)
  {
    if (y <= (p->bounds.y1-mid-0.5))
      break;
    if (y > (p->bounds.y2+mid+0.5))
      {
        (void) DestroyEdge(polygon_info,j);
        p++;
        continue;
      }
    if (x <= (p->bounds.x1-mid-0.5))
      {
        p++;
        continue;
      }
    if (x > (p->bounds.x2+mid+0.5))
      {
        p++;
        continue;
      }
    for (i=Max(p->highwater,1); i < p->number_points; i++)
    {
      if (y <= (p->points[i-1].y-mid-0.5))
        break;
      if (y > (p->points[i].y+mid+0.5))
        continue;
      if (p->scanline != y)
        {
          p->scanline=y;
          p->highwater=i;
        }
      /*
        Compute distance between a point and an edge.
      */
      q=p->points+i-1;
      dx=(q+1)->x-q->x,
      dy=(q+1)->y-q->y;
      beta=dx*(x-q->x)+dy*(y-q->y);
      if (beta < 0.0)
        {
          dx=x-q->x;
          dy=y-q->y;
          distance=dx*dx+dy*dy;
        }
      else
        {
          alpha=dx*dx+dy*dy;
          if (beta > alpha)
            {
              dx=x-(q+1)->x;
              dy=y-(q+1)->y;
              distance=dx*dx+dy*dy;
            }
          else
            {
              alpha=1.0/alpha;
              beta=dx*(y-q->y)-dy*(x-q->x);
              distance=alpha*beta*beta;
            }
        }
      /*
        Compute stroke & subpath opacity.
      */
      beta=0.0;
      if (!p->ghostline)
        {
          alpha=mid+0.5;
          if ((*stroke_opacity < 1.0) &&
              (distance <= ((alpha+0.25)*(alpha+0.25))))
            {
              alpha=mid-0.5;
              if (distance <= ((alpha+0.25)*(alpha+0.25)))
                *stroke_opacity=1.0;
              else
                {
                  beta=1.0;
                  if (distance != 1.0)
                    beta=sqrt(distance);
                  alpha=beta-mid-0.5;
                  if (*stroke_opacity < ((alpha-0.25)*(alpha-0.25)))
                    *stroke_opacity=(alpha-0.25)*(alpha-0.25);
                }
            }
        }
      if (!fill || (distance > 1.0) || (subpath_opacity >= 1.0))
        continue;
      if (distance <= 0.0)
        {
          subpath_opacity=1.0;
          continue;
        }
      if (distance > 1.0)
        continue;
      if (beta == 0.0)
        {
          beta=1.0;
          if (distance != 1.0)
            beta=sqrt(distance);
        }
      alpha=beta-1.0;
      if (subpath_opacity < (alpha*alpha))
        subpath_opacity=alpha*alpha;
    }
    p++;
  }
  /*
    Compute fill opacity.
  */
  if (!fill)
    return(0.0);
  if (subpath_opacity >= 1.0)
    return(1.0);
  /*
    Determine winding number.
  */
  winding_number=0;
  p=polygon_info->edges;
  for (j=0; j < polygon_info->number_edges; j++)
  {
    if (y <= p->bounds.y1)
      break;
    if (y > p->bounds.y2)
      {
        p++;
        continue;
      }
    if (x <= p->bounds.x1)
      {
        p++;
        continue;
      }
    if (x > p->bounds.x2)
      {
        winding_number+=p->direction ? 1 : -1;
        p++;
        continue;
      }
    for (i=Max(p->highwater,1); i < p->number_points; i++)
      if (y <= p->points[i].y)
        break;
    q=p->points+i-1;
    dx=(q+1)->x-q->x;
    dy=(q+1)->y-q->y;
    if ((dx*(y-q->y)) <= (dy*(x-q->x)))
      winding_number+=p->direction ? 1 : -1;
    p++;
  }
  if (fill_rule != NonZeroRule)
    {
      if (AbsoluteValue(winding_number) & 0x01)
        return(1.0);
    }
  else
    if (AbsoluteValue(winding_number) > 0)
      return(1.0);
  return(subpath_opacity);
}

static unsigned int DrawPolygonPrimitive(Image *image,const DrawInfo *draw_info,
  const PrimitiveInfo *primitive_info)
{
  double
    fill_opacity,
    mid,
    stroke_opacity;

  Image
    *pattern;

  long
    start,
    stop,
    y;

  PathInfo
    *path_info;

  PixelPacket
    fill_color,
    stroke_color;

  PolygonInfo
    *polygon_info;

  register EdgeInfo
    *p;

  register long
    x;

  register PixelPacket
    *q;

  register long
    i;

  SegmentInfo
    bounds;

  unsigned int
    fill;

  /*
    Compute bounding box.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(draw_info != (DrawInfo *) NULL);
  assert(draw_info->signature == MagickSignature);
  assert(primitive_info != (PrimitiveInfo *) NULL);
  if (primitive_info->coordinates == 0)
    return(True);
  path_info=ConvertPrimitiveToPath(draw_info,primitive_info);
  if (path_info == (PathInfo *) NULL)
    return(False);
  polygon_info=ConvertPathToPolygon(draw_info,path_info);
  MagickFreeMemory(path_info);
  if (polygon_info == (PolygonInfo *) NULL)
    return(False);
  if (0) /* FIXME ??? */
    DrawBoundingRectangles(image,draw_info,polygon_info);
  (void) LogMagickEvent(RenderEvent,GetMagickModule(),"    begin draw-polygon");
  fill=(primitive_info->method == FillToBorderMethod) ||
    (primitive_info->method == FloodfillMethod);
  fill_color=draw_info->fill;
  stroke_color=draw_info->stroke;
  mid=ExpandAffine(&draw_info->affine)*draw_info->stroke_width/2.0;
  bounds=polygon_info->edges[0].bounds;
  for (i=1; i < polygon_info->number_edges; i++)
  {
    p=polygon_info->edges+i;
    if (p->bounds.x1 < bounds.x1)
      bounds.x1=p->bounds.x1;
    if (p->bounds.y1 < bounds.y1)
      bounds.y1=p->bounds.y1;
    if (p->bounds.x2 > bounds.x2)
      bounds.x2=p->bounds.x2;
    if (p->bounds.y2 > bounds.y2)
      bounds.y2=p->bounds.y2;
  }
  bounds.x1-=(mid+1.0);
  bounds.x1=bounds.x1 < 0.0 ? 0.0 : bounds.x1 >= image->columns ?
    image->columns-1 : bounds.x1;
  bounds.y1-=(mid+1.0);
  bounds.y1=bounds.y1 < 0.0 ? 0.0 : bounds.y1 >= image->rows ?
    image->rows-1 : bounds.y1;
  bounds.x2+=(mid+1.0);
  bounds.x2=bounds.x2 < 0.0 ? 0.0 : bounds.x2 >= image->columns ?
    image->columns-1 : bounds.x2;
  bounds.y2+=(mid+1.0);
  bounds.y2=bounds.y2 < 0.0 ? 0.0 : bounds.y2 >= image->rows ?
    image->rows-1 : bounds.y2;
  if (primitive_info->coordinates == 1)
    {
      /*
        Draw point.
      */
      start=(long) ceil(bounds.x1-0.5);
      stop=(long) floor(bounds.x2+0.5);
      /* FIXME: OpenMP */
      for (y=(long) ceil(bounds.y1-0.5); y <= (long) floor(bounds.y2+0.5); y++)
      {
        x=start;
        q=GetImagePixels(image,x,y,stop-x+1,1);
        if (q == (PixelPacket *) NULL)
          break;
        for ( ; x <= stop; x++)
        {
          if ((x == (long) ceil(primitive_info->point.x-0.5)) &&
              (y == (long) ceil(primitive_info->point.y-0.5)))
            *q=stroke_color;
          q++;
        }
        if (!SyncImagePixels(image))
          break;
      }
      (void) LogMagickEvent(RenderEvent,GetMagickModule(),
        "    end draw-polygon");
      return(True);
    }
  /*
    Draw polygon or line.
  */
  start=(long) ceil(bounds.x1-0.5);
  stop=(long) floor(bounds.x2+0.5);
  /* FIXME: OpenMP */
  for (y=(long) ceil(bounds.y1-0.5); y <= (long) floor(bounds.y2+0.5); y++)
  {
    x=start;
    q=GetImagePixels(image,x,y,stop-x+1,1);
    if (q == (PixelPacket *) NULL)
      break;
    for ( ; x <= stop; x++)
    {
      /*
        Fill and/or stroke.
      */
      fill_opacity=GetPixelOpacity(polygon_info,mid,fill,draw_info->fill_rule,
        x,y,&stroke_opacity);
      if (!draw_info->stroke_antialias)
        {
          /* When stroke antialiasing is disabled, only draw for
             opacities >= 0.99 in order to ensure that lines are not
             drawn wider than requested. */
          if (fill_opacity < 0.99)
            fill_opacity=0.0;
          if (stroke_opacity < 0.99)
            stroke_opacity=0.0;
        }
      pattern=draw_info->fill_pattern;
      if (pattern != (Image *) NULL)
        (void) AcquireOnePixelByReference(pattern,&fill_color,
          (long) (x-pattern->tile_info.x) % pattern->columns,
          (long) (y-pattern->tile_info.y) % pattern->rows,
          &image->exception);
      fill_opacity=MaxRGB-fill_opacity*(MaxRGB-fill_color.opacity);
      if (fill_opacity != TransparentOpacity)
        AlphaCompositePixel(q,&fill_color,fill_opacity,q,
          (q->opacity == TransparentOpacity) ? OpaqueOpacity : q->opacity);
      pattern=draw_info->stroke_pattern;
      if (pattern != (Image *) NULL)
        (void) AcquireOnePixelByReference(pattern,&stroke_color,
          (long) (x-pattern->tile_info.x) % pattern->columns,
          (long) (y-pattern->tile_info.y) % pattern->rows,&image->exception);
      stroke_opacity=MaxRGB-stroke_opacity*(MaxRGB-stroke_color.opacity);
      if (stroke_opacity != TransparentOpacity)
        AlphaCompositePixel(q,&stroke_color,stroke_opacity,q,
          (q->opacity == TransparentOpacity) ? OpaqueOpacity : q->opacity);
      q++;
    }
    if (!SyncImagePixels(image))
      break;
  }
  (void) LogMagickEvent(RenderEvent,GetMagickModule(),"    end draw-polygon");
  DestroyPolygonInfo(polygon_info);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D r a w P r i m i t i v e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DrawPrimitive draws a primitive (line, rectangle, ellipse) on the
%  image.
%
%  The format of the DrawPrimitive method is:
%
%      void DrawPrimitive(Image *image,const DrawInfo *draw_info,
%        PrimitiveInfo *primitive_info)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o draw_info: The draw info.
%
%    o primitive_info: Specifies a pointer to a PrimitiveInfo structure.
%
%
*/

static void LogPrimitiveInfo(const PrimitiveInfo *primitive_info)
{
  char
    *methods[] =
    {
      (char *) "point",
      (char *) "replace",
      (char *) "floodfill",
      (char *) "filltoborder",
      (char *) "reset",
      (char *) "?"
    };

  long
    coordinates,
    y;

  PointInfo
    p={0,0},
    q,
    point;

  register long
    i,
    x;

  x=(long) ceil(primitive_info->point.x-0.5);
  y=(long) ceil(primitive_info->point.y-0.5);
  switch (primitive_info->primitive)
  {
    case PointPrimitive:
    {
      (void) LogMagickEvent(RenderEvent,GetMagickModule(),
        "PointPrimitive %ld,%ld %s",x,y,methods[primitive_info->method]);
      return;
    }
    case ColorPrimitive:
    {
      (void) LogMagickEvent(RenderEvent,GetMagickModule(),
        "ColorPrimitive %ld,%ld %s",x,y,methods[primitive_info->method]);
      return;
    }
    case MattePrimitive:
    {
      (void) LogMagickEvent(RenderEvent,GetMagickModule(),
        "MattePrimitive %ld,%ld %s",x,y,methods[primitive_info->method]);
      return;
    }
    case TextPrimitive:
    {
      (void) LogMagickEvent(RenderEvent,GetMagickModule(),
        "TextPrimitive %ld,%ld",x,y);
      return;
    }
    case ImagePrimitive:
    {
      (void) LogMagickEvent(RenderEvent,GetMagickModule(),
        "ImagePrimitive %ld,%ld",x,y);
      return;
    }
    default:
      break;
  }
  coordinates=0;
  q.x=(-1.0);
  q.y=(-1.0);
  for (i=0; primitive_info[i].primitive != UndefinedPrimitive; i++)
  {
    point=primitive_info[i].point;
    if (coordinates <= 0)
      {
        coordinates=(long) primitive_info[i].coordinates;
        (void) LogMagickEvent(RenderEvent,GetMagickModule(),
          "    begin open (%ld)",coordinates);
        p=point;
      }
    point=primitive_info[i].point;
    if ((fabs(q.x-point.x) > MagickEpsilon) ||
        (fabs(q.y-point.y) > MagickEpsilon))
      (void) LogMagickEvent(RenderEvent,GetMagickModule(),"      %ld: %g,%g",
        coordinates,point.x,point.y);
    else
      (void) LogMagickEvent(RenderEvent,GetMagickModule(),
        "      %ld: %g,%g (duplicate)",coordinates,point.x,point.y);
    q=point;
    coordinates--;
    if (coordinates > 0)
      continue;
    if ((fabs(p.x-point.x) > MagickEpsilon) ||
        (fabs(p.y-point.y) > MagickEpsilon))
      (void) LogMagickEvent(RenderEvent,GetMagickModule(),"    end last (%ld)",
        coordinates);
    else
      (void) LogMagickEvent(RenderEvent,GetMagickModule(),"    end open (%ld)",
        coordinates);
  }
}

static MagickPassFail DrawPrimitive(Image *image,const DrawInfo *draw_info,
  const PrimitiveInfo *primitive_info)
{
  long
    y;

  MonitorHandler
    handler;

  register long
    i,
    x;

  register PixelPacket
    *q;

  unsigned int
    status;

  (void) LogMagickEvent(RenderEvent,GetMagickModule(),"  begin draw-primitive");
  (void) LogMagickEvent(RenderEvent,GetMagickModule(),
    "    affine: %g,%g,%g,%g,%g,%g",draw_info->affine.sx,draw_info->affine.rx,
    draw_info->affine.ry,draw_info->affine.sy,draw_info->affine.tx,
    draw_info->affine.ty);
  status=True;
  x=(long) ceil(primitive_info->point.x-0.5);
  y=(long) ceil(primitive_info->point.y-0.5);
  switch (primitive_info->primitive)
  {
    case PointPrimitive:
    {
      q=GetImagePixels(image,x,y,1,1);
      if (q == (PixelPacket *) NULL)
        break;
      *q=draw_info->fill;
      (void) SyncImagePixels(image);
      break;
    }
    case ColorPrimitive:
    {
      switch (primitive_info->method)
      {
        case PointMethod:
        default:
        {
          q=GetImagePixels(image,x,y,1,1);
          if (q == (PixelPacket *) NULL)
            break;
          *q=draw_info->fill;
          (void) SyncImagePixels(image);
          break;
        }
        case ReplaceMethod:
        {
          Image
            *pattern;

          PixelPacket
            color,
            target;

          color=draw_info->fill;
          (void) AcquireOnePixelByReference(image,&target,x,y,&image->exception);
          pattern=draw_info->fill_pattern;
          for (y=0; y < (long) image->rows; y++)
          {
            q=GetImagePixels(image,0,y,image->columns,1);
            if (q == (PixelPacket *) NULL)
              break;
            for (x=0; x < (long) image->columns; x++)
            {
              if (!FuzzyColorMatch(q,&target,image->fuzz))
                {
                  q++;
                  continue;
                }
              if (pattern != (Image *) NULL)
                {
                  (void) AcquireOnePixelByReference(pattern,&color,
                    (long) (x-pattern->tile_info.x) % pattern->columns,
                    (long) (y-pattern->tile_info.y) % pattern->rows,
                    &image->exception);
                  if (!pattern->matte)
                    color.opacity=OpaqueOpacity;
                }
              if (color.opacity != TransparentOpacity)
                AlphaCompositePixel(q,&color,color.opacity,q,q->opacity);
              q++;
            }
            if (!SyncImagePixels(image))
              break;
          }
          break;
        }
        case FloodfillMethod:
        case FillToBorderMethod:
        {
          PixelPacket
            border_color,
            target;

          (void) AcquireOnePixelByReference(image,&target,x,y,&image->exception);
          if (primitive_info->method == FillToBorderMethod)
            {
              border_color=draw_info->border_color;
              target=border_color;
            }
          (void) ColorFloodfillImage(image,draw_info,target,x,y,
            primitive_info->method);
          break;
        }
        case ResetMethod:
        {
          for (y=0; y < (long) image->rows; y++)
          {
            q=GetImagePixels(image,0,y,image->columns,1);
            if (q == (PixelPacket *) NULL)
              break;
            for (x=0; x < (long) image->columns; x++)
            {
              *q=draw_info->fill;
              q++;
            }
            if (!SyncImagePixels(image))
              break;
          }
          break;
        }
      }
      break;
    }
    case MattePrimitive:
    {
      if (!image->matte)
        SetImageOpacity(image,OpaqueOpacity);
      switch (primitive_info->method)
      {
        case PointMethod:
        default:
        {
          q=GetImagePixels(image,x,y,1,1);
          if (q == (PixelPacket *) NULL)
            break;
          q->opacity=TransparentOpacity;
          (void) SyncImagePixels(image);
          break;
        }
        case ReplaceMethod:
        {
          PixelPacket
            target;

          (void) AcquireOnePixelByReference(image,&target,x,y,&image->exception);
          (void) TransparentImage(image,target,TransparentOpacity);
          break;
        }
        case FloodfillMethod:
        case FillToBorderMethod:
        {
          PixelPacket
            border_color,
            target;

          (void) AcquireOnePixelByReference(image,&target,x,y,&image->exception);
          if (primitive_info->method == FillToBorderMethod)
            {
              border_color=draw_info->border_color;
              target=border_color;
            }
          (void) MatteFloodfillImage(image,target,TransparentOpacity,x,y,
            primitive_info->method);
          break;
        }
        case ResetMethod:
        {
          for (y=0; y < (long) image->rows; y++)
          {
            q=GetImagePixels(image,0,y,image->columns,1);
            if (q == (PixelPacket *) NULL)
              break;
            for (x=0; x < (long) image->columns; x++)
            {
              q->opacity=draw_info->fill.opacity;
              q++;
            }
            if (!SyncImagePixels(image))
              break;
          }
          break;
        }
      }
      break;
    }
    case TextPrimitive:
    {
      char
        geometry[MaxTextExtent];

      DrawInfo
        *clone_info;

      if (primitive_info->text == (char *) NULL)
        break;
      clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
      (void) CloneString(&clone_info->text,primitive_info->text);
      FormatString(geometry,"%+g%+g",primitive_info->point.x,
        primitive_info->point.y);
      (void) CloneString(&clone_info->geometry,geometry);
      status=AnnotateImage(image,clone_info);
      DestroyDrawInfo(clone_info);
      break;
    }
    case ImagePrimitive:
    {
      AffineMatrix
        affine;

      Image
        *composite_image;

      ImageInfo
        *clone_info;

      if (primitive_info->text == (char *) NULL)
        break;
      clone_info=CloneImageInfo((ImageInfo *) NULL);
      if (LocaleNCompare(primitive_info->text,"data:",5) == 0)
        composite_image=ReadInlineImage(clone_info,primitive_info->text,
          &image->exception);
      else
        {
          (void) strlcpy(clone_info->filename,primitive_info->text,
            MaxTextExtent);
          composite_image=ReadImage(clone_info,&image->exception);
        }
      if (image->exception.severity != UndefinedException)
        MagickError2(image->exception.severity,image->exception.reason,
          image->exception.description);
      DestroyImageInfo(clone_info);
      if (composite_image == (Image *) NULL)
        break;
      if ((primitive_info[1].point.x != composite_image->columns) &&
          (primitive_info[1].point.y != composite_image->rows))
        {
          char
            geometry[MaxTextExtent];

          /*
            Resize image.
          */
          FormatString(geometry,"%gx%g!",primitive_info[1].point.x,
            primitive_info[1].point.y);
          handler=SetMonitorHandler((MonitorHandler) NULL);
          TransformImage(&composite_image,(char *) NULL,geometry);
          (void) SetMonitorHandler(handler);
        }
      if (!composite_image->matte)
        SetImageOpacity(composite_image,OpaqueOpacity);
      if (draw_info->opacity != OpaqueOpacity)
        SetImageOpacity(composite_image,draw_info->opacity);
      affine=draw_info->affine;
      affine.tx=x;
      affine.ty=y;
      (void) DrawAffineImage(image,composite_image,&affine);
      DestroyImage(composite_image);
      break;
    }
    default:
    {
      double
        mid,
        scale;

      DrawInfo
        *clone_info;

      if (IsEventLogging())
        LogPrimitiveInfo(primitive_info);
      scale=ExpandAffine(&draw_info->affine);
      if ((draw_info->dash_pattern != (double *) NULL) &&
          (draw_info->dash_pattern[0] != 0.0) &&
          ((scale*draw_info->stroke_width) > MagickEpsilon) &&
          (draw_info->stroke.opacity != TransparentOpacity))
        {
          /*
            Draw dash polygon.
          */
          clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
          clone_info->stroke_width=0.0;
          clone_info->stroke.opacity=TransparentOpacity;
          status=DrawPolygonPrimitive(image,clone_info,primitive_info);
          DestroyDrawInfo(clone_info);
          (void) DrawDashPolygon(draw_info,primitive_info,image);
          break;
        }
      mid=ExpandAffine(&draw_info->affine)*draw_info->stroke_width/2.0;
      if ((mid > 1.0) && (draw_info->stroke.opacity != TransparentOpacity))
        {
          unsigned int
            closed_path;

          /*
            Draw strokes while respecting line cap/join attributes.
          */
          for (i=0; primitive_info[i].primitive != UndefinedPrimitive; i++);
          closed_path=
            (primitive_info[i-1].point.x == primitive_info[0].point.x) &&
            (primitive_info[i-1].point.y == primitive_info[0].point.y);
          i=(long) primitive_info[0].coordinates;
          if ((((draw_info->linecap == RoundCap) || closed_path) &&
               (draw_info->linejoin == RoundJoin)) ||
               (primitive_info[i].primitive != UndefinedPrimitive))
            {
              (void) DrawPolygonPrimitive(image,draw_info,primitive_info);
              break;
            }
          clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
          clone_info->stroke_width=0.0;
          clone_info->stroke.opacity=TransparentOpacity;
          status=DrawPolygonPrimitive(image,clone_info,primitive_info);
          DestroyDrawInfo(clone_info);
          status|=DrawStrokePolygon(image,draw_info,primitive_info);
          break;
        }
      status=DrawPolygonPrimitive(image,draw_info,primitive_info);
      break;
    }
  }
  (void) LogMagickEvent(RenderEvent,GetMagickModule(),"  end draw-primitive");
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D r a w S t r o k e P o l y g o n                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DrawStrokePolygon draws a stroked polygon (line, rectangle, ellipse)
%  on the image while respecting the line cap and join attributes.
%
%  The format of the DrawStrokePolygon method is:
%
%      unsigned int DrawStrokePolygon(Image *image,const DrawInfo *draw_info,
%        const PrimitiveInfo *primitive_info)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o draw_info: The draw info.
%
%    o primitive_info: Specifies a pointer to a PrimitiveInfo structure.
%
%
*/

static void DrawRoundLinecap(Image *image,const DrawInfo *draw_info,
  const PrimitiveInfo *primitive_info)
{
  PrimitiveInfo
    linecap[5];

  register long
    i;

  for (i=0; i < 4; i++)
    linecap[i]=(*primitive_info);
  linecap[0].coordinates=4;
  linecap[1].point.x+=10.0*MagickEpsilon;
  linecap[2].point.x+=10.0*MagickEpsilon;
  linecap[2].point.y+=10.0*MagickEpsilon;
  linecap[3].point.y+=10.0*MagickEpsilon;
  linecap[4].primitive=UndefinedPrimitive;
  (void) DrawPolygonPrimitive(image,draw_info,linecap);
}

static unsigned int DrawStrokePolygon(Image *image,const DrawInfo *draw_info,
  const PrimitiveInfo *primitive_info)
{
  DrawInfo
    *clone_info;

  PrimitiveInfo
    *stroke_polygon;

  register const PrimitiveInfo
    *p,
    *q;

  unsigned int
    closed_path,
    status;

  /*
    Draw stroked polygon.
  */
  (void) LogMagickEvent(RenderEvent,GetMagickModule(),
    "    begin draw-stroke-polygon");
  clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  clone_info->fill=draw_info->stroke;
  clone_info->stroke.opacity=TransparentOpacity;
  clone_info->stroke_width=0.0;
  clone_info->fill_rule=NonZeroRule;
  status=True;
  for (p=primitive_info; p->primitive != UndefinedPrimitive; p+=p->coordinates)
  {
    stroke_polygon=TraceStrokePolygon(draw_info,p);
    status=DrawPolygonPrimitive(image,clone_info,stroke_polygon);
    MagickFreeMemory(stroke_polygon);
    q=p+p->coordinates-1;
    closed_path=(q->point.x == p->point.x) && (q->point.y == p->point.y);
    if ((draw_info->linecap == RoundCap) && !closed_path)
      {
        DrawRoundLinecap(image,draw_info,p);
        DrawRoundLinecap(image,draw_info,q);
      }
  }
  DestroyDrawInfo(clone_info);
  (void) LogMagickEvent(RenderEvent,GetMagickModule(),
    "    end draw-stroke-polygon");
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t D r a w I n f o                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetDrawInfo initializes draw_info to default values.
%
%  The format of the GetDrawInfo method is:
%
%      void GetDrawInfo(const ImageInfo *image_info,DrawInfo *draw_info)
%
%  A description of each parameter follows:
%
%    o image_info: The image info..
%
%    o draw_info: The draw info.
%
%
*/
MagickExport void GetDrawInfo(const ImageInfo *image_info,DrawInfo *draw_info)
{
  ImageInfo
    *clone_info;

  /*
    Initialize draw attributes.
  */
  assert(draw_info != (DrawInfo *) NULL);
  (void) memset(draw_info,0,sizeof(DrawInfo));
  clone_info=CloneImageInfo(image_info);
  IdentityAffine(&draw_info->affine);
  draw_info->gravity=NorthWestGravity;
  draw_info->opacity=OpaqueOpacity; /* 0UL */
  draw_info->fill.opacity=OpaqueOpacity; /* 0UL */
  draw_info->stroke.opacity=TransparentOpacity; /* MaxRGB */
  draw_info->stroke_antialias=clone_info->antialias;
  draw_info->stroke_width=1.0;
  draw_info->fill_rule=EvenOddRule;
  draw_info->linecap=ButtCap;
  draw_info->linejoin=MiterJoin;
  draw_info->miterlimit=10;
  draw_info->decorate=NoDecoration;
  if (clone_info->font != (char *) NULL)
    draw_info->font=AllocateString(clone_info->font);
  if (clone_info->density != (char *) NULL)
    draw_info->density=AllocateString(clone_info->density);
  draw_info->text_antialias=clone_info->antialias;
  draw_info->pointsize=clone_info->pointsize;
  draw_info->undercolor.opacity=TransparentOpacity;
  draw_info->border_color=clone_info->border_color;
  draw_info->compose=CopyCompositeOp;
  if (clone_info->server_name != (char *) NULL)
    draw_info->server_name=AllocateString(clone_info->server_name);
  draw_info->render=True;
  draw_info->signature=MagickSignature;
  DestroyImageInfo(clone_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   P e r m u t a t e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Permutate()
%
%  The format of the Permutate method is:
%
%      void Permutate(long n,long k)
%
%  A description of each parameter follows:
%
%    o n:
%
%    o k:
%
%
*/
static inline double Permutate(const long n,const long k)
{
  double
    r;

  register long
    i;

  r=1.0;
  for (i=k+1; i <= n; i++)
    r*=i;
  for (i=1; i <= (n-k); i++)
    r/=i;
  return(r);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   T r a c e P r i m i t i v e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  TracePrimitive is a collection of methods for generating graphic
%  primitives such as arcs, ellipses, paths, etc.
%
%
*/

static void TraceArc(PrimitiveInfo *primitive_info,const PointInfo start,
  const PointInfo end,const PointInfo arc)
{
  PointInfo
    center,
    radius;

  center.x=0.5*(end.x+start.x);
  center.y=0.5*(end.y+start.y);
  radius.x=fabs(center.x-start.x);
  radius.y=fabs(center.y-start.y);
  TraceEllipse(primitive_info,center,radius,arc);
}

static void TraceArcPath(PrimitiveInfo *primitive_info,const PointInfo start,
  const PointInfo end,const PointInfo arc,const double angle,
  const unsigned int large_arc,const unsigned int sweep)
{
  double
    alpha,
    beta,
    delta,
    factor,
    gamma,
    theta;

  PointInfo
    center,
    points[3],
    radii;

  register double
    cosine,
    sine;

  register PrimitiveInfo
    *p;

  register long
    i;

  unsigned long
    arc_segments;

  if ((start.x == end.x) && (start.y == end.y))
    return;
  radii.x=fabs(arc.x);
  radii.y=fabs(arc.y);
  if ((radii.x == 0.0) || (radii.y == 0.0))
    {
      TraceLine(primitive_info,start,end);
      return;
    }
  cosine=cos(DegreesToRadians(fmod(angle,360.0)));
  sine=sin(DegreesToRadians(fmod(angle,360.0)));
  center.x=cosine*(end.x-start.x)/2+sine*(end.y-start.y)/2;
  center.y=cosine*(end.y-start.y)/2-sine*(end.x-start.x)/2;
  delta=(center.x*center.x)/(radii.x*radii.x)+
    (center.y*center.y)/(radii.y*radii.y);
  if (delta > 1.0)
    {
      radii.x*=sqrt(delta);
      radii.y*=sqrt(delta);
    }
  points[0].x=cosine*start.x/radii.x+sine*start.y/radii.x;
  points[0].y=cosine*start.y/radii.y-sine*start.x/radii.y;
  points[1].x=cosine*end.x/radii.x+sine*end.y/radii.x;
  points[1].y=cosine*end.y/radii.y-sine*end.x/radii.y;
  alpha=points[1].x-points[0].x;
  beta=points[1].y-points[0].y;
  factor=1.0/(alpha*alpha+beta*beta)-0.25;
  if (factor <= 0.0)
    factor=0.0;
  else
    {
      factor=sqrt(factor);
      if (sweep == large_arc)
        factor=(-factor);
    }
  center.x=(points[0].x+points[1].x)/2-factor*beta;
  center.y=(points[0].y+points[1].y)/2+factor*alpha;
  alpha=atan2(points[0].y-center.y,points[0].x-center.x);
  theta=atan2(points[1].y-center.y,points[1].x-center.x)-alpha;
  if ((theta < 0.0) && sweep)
    theta+=2.0*MagickPI;
  else
    if ((theta > 0.0) && !sweep)
      theta-=2.0*MagickPI;
  arc_segments=(long) ceil(fabs(theta/(0.5*MagickPI+MagickEpsilon)));
  p=primitive_info;
  for (i=0; i < (long) arc_segments; i++)
  {
    beta=0.5*((alpha+(i+1)*theta/arc_segments)-(alpha+i*theta/arc_segments));
    gamma=(8.0/3.0)*sin(fmod(0.5*beta,DegreesToRadians(360.0)))*
      sin(fmod(0.5*beta,DegreesToRadians(360.0)))/
      sin(fmod(beta,DegreesToRadians(360.0)));
    points[0].x=center.x+
      cos(fmod(alpha+i*theta/arc_segments,DegreesToRadians(360.0)))-gamma*
      sin(fmod(alpha+i*theta/arc_segments,DegreesToRadians(360.0)));
    points[0].y=center.y+
      sin(fmod(alpha+i*theta/arc_segments,DegreesToRadians(360.0)))+gamma*
      cos(fmod(alpha+i*theta/arc_segments,DegreesToRadians(360.0)));
    points[2].x=center.x+
      cos(fmod(alpha+(i+1)*theta/arc_segments,DegreesToRadians(360.0)));
    points[2].y=center.y+
      sin(fmod(alpha+(i+1)*theta/arc_segments,DegreesToRadians(360.0)));
    points[1].x=points[2].x+gamma*
      sin(fmod(alpha+(i+1)*theta/arc_segments,DegreesToRadians(360.0)));
    points[1].y=points[2].y-gamma*
      cos(fmod(alpha+(i+1)*theta/arc_segments,DegreesToRadians(360.0)));
    p->point.x=(p == primitive_info) ? start.x : (p-1)->point.x;
    p->point.y=(p == primitive_info) ? start.y : (p-1)->point.y;
    (p+1)->point.x=cosine*radii.x*points[0].x-sine*radii.y*points[0].y;
    (p+1)->point.y=sine*radii.x*points[0].x+cosine*radii.y*points[0].y;
    (p+2)->point.x=cosine*radii.x*points[1].x-sine*radii.y*points[1].y;
    (p+2)->point.y=sine*radii.x*points[1].x+cosine*radii.y*points[1].y;
    (p+3)->point.x=cosine*radii.x*points[2].x-sine*radii.y*points[2].y;
    (p+3)->point.y=sine*radii.x*points[2].x+cosine*radii.y*points[2].y;
    if (i == (long) (arc_segments-1))
      (p+3)->point=end;
    TraceBezier(p,4);
    p+=p->coordinates;
  }
  primitive_info->coordinates=p-primitive_info;
  for (i=0; i < (long) primitive_info->coordinates; i++)
  {
    p->primitive=primitive_info->primitive;
    p--;
  }
}

static void TraceBezier(PrimitiveInfo *primitive_info,
  const unsigned long number_coordinates)
{
  double
    alpha,
    *coefficients,
    weight;

  PointInfo
    end,
    point,
    *points;

  register long
    i,
    j;

  register PrimitiveInfo
    *p;

  unsigned long
    control_points,
    quantum;

  /*
    Allocate coeficients.
  */
  quantum=number_coordinates;
  for (i=0; i < (long) number_coordinates; i++)
  {
    for (j=i+1; j < (long) number_coordinates; j++)
    {
      alpha=fabs(primitive_info[j].point.x-primitive_info[i].point.x);
      if (alpha > quantum)
        quantum=(unsigned long) alpha;
      alpha=fabs(primitive_info[j].point.y-primitive_info[i].point.y);
      if (alpha > quantum)
        quantum=(unsigned long) alpha;
    }
  }
  quantum=Min(quantum/number_coordinates,BezierQuantum);
  control_points=quantum*number_coordinates;
  coefficients=MagickAllocateMemory(double *,number_coordinates*sizeof(double));
  points=MagickAllocateMemory(PointInfo *,control_points*sizeof(PointInfo));
  if ((coefficients == (double *) NULL) || (points == (PointInfo *) NULL))
    MagickFatalError3(ResourceLimitError,MemoryAllocationFailed,
      UnableToDrawOnImage);
  /*
    Compute bezier points.
  */
  end=primitive_info[number_coordinates-1].point;
  weight=0.0;
  for (i=0; i < (long) number_coordinates; i++)
    coefficients[i]=Permutate((long) number_coordinates-1,i);
  for (i=0; i < (long) control_points; i++)
  {
    p=primitive_info;
    point.x=0;
    point.y=0;
    alpha=pow((double) (1.0-weight),(double) number_coordinates-1);
    for (j=0; j < (long) number_coordinates; j++)
    {
      point.x+=alpha*coefficients[j]*p->point.x;
      point.y+=alpha*coefficients[j]*p->point.y;
      alpha*=weight/(1.0-weight);
      p++;
    }
    points[i]=point;
    weight+=1.0/quantum/number_coordinates;
  }
  /*
    Bezier curves are just short segmented polys.
  */
  p=primitive_info;
  for (i=0; i < (long) control_points; i++)
  {
    TracePoint(p,points[i]);
    p+=p->coordinates;
  }
  TracePoint(p,end);
  p+=p->coordinates;
  primitive_info->coordinates=p-primitive_info;
  for (i=0; i < (long) primitive_info->coordinates; i++)
  {
    p->primitive=primitive_info->primitive;
    p--;
  }
  MagickFreeMemory(points);
  MagickFreeMemory(coefficients);
}

static void TraceCircle(PrimitiveInfo *primitive_info,const PointInfo start,
  const PointInfo end)
{
  double
    alpha,
    beta,
    radius;

  PointInfo
    offset,
    degrees;

  alpha=end.x-start.x;
  beta=end.y-start.y;
  radius=sqrt(alpha*alpha+beta*beta);
  offset.x=radius;
  offset.y=radius;
  degrees.x=0.0;
  degrees.y=360.0;
  TraceEllipse(primitive_info,start,offset,degrees);
}

static void TraceEllipse(PrimitiveInfo *primitive_info,const PointInfo start,
  const PointInfo stop,const PointInfo degrees)
{
  double
    delta,
    step,
    y;

  PointInfo
    angle,
    point;

  register PrimitiveInfo
    *p;

  register long
    i;

  /*
    Ellipses are just short segmented polys.
  */
  delta=2.0/Max(stop.x,stop.y);
  step=MagickPI/8.0;
  if (delta < (MagickPI/8.0))
    step=MagickPI/(4*ceil(MagickPI/delta/2));
  angle.x=DegreesToRadians(degrees.x);
  y=degrees.y;
  while (y < degrees.x)
    y+=360.0;
  angle.y=DegreesToRadians(y);
  for (p=primitive_info; angle.x < angle.y; angle.x+=step)
  {
    point.x=cos(fmod(angle.x,DegreesToRadians(360.0)))*stop.x+start.x;
    point.y=sin(fmod(angle.x,DegreesToRadians(360.0)))*stop.y+start.y;
    TracePoint(p,point);
    p+=p->coordinates;
  }
  point.x=cos(fmod(angle.y,DegreesToRadians(360.0)))*stop.x+start.x;
  point.y=sin(fmod(angle.y,DegreesToRadians(360.0)))*stop.y+start.y;
  TracePoint(p,point);
  p+=p->coordinates;
  primitive_info->coordinates=p-primitive_info;
  for (i=0; i < (long) primitive_info->coordinates; i++)
  {
    p->primitive=primitive_info->primitive;
    p--;
  }
}

static void TraceLine(PrimitiveInfo *primitive_info,const PointInfo start,
  const PointInfo end)
{
  TracePoint(primitive_info,start);
  if ((AbsoluteValue(start.x-end.x) <= MagickEpsilon) &&
      (AbsoluteValue(start.y-end.y) <= MagickEpsilon))
    {
      primitive_info->primitive=PointPrimitive;
      primitive_info->coordinates=1;
      return;
    }
  TracePoint(primitive_info+1,end);
  (primitive_info+1)->primitive=primitive_info->primitive;
  primitive_info->coordinates=2;
}

static unsigned long TracePath(PrimitiveInfo *primitive_info,const char *path)
{
  char
    *p,
    token[MaxTextExtent];

  double
    x,
    y;

  int
    attribute,
    last_attribute;

  PointInfo
    end={0,0},
    points[4],
    point,
    start={0,0};

  PrimitiveType
    primitive_type;

  register PrimitiveInfo
    *q;

  register long
    i;

  unsigned long
    number_coordinates,
    z_count;

  attribute=0;
  point.x=0;
  point.y=0;
  number_coordinates=0;
  z_count=0;
  primitive_type=primitive_info->primitive;
  q=primitive_info;
  for (p=(char *) path; *p != '\0'; )
  {
    while (isspace((int) *p))
      p++;
    if (*p == '\0')
      break;
    last_attribute=attribute;
    attribute=(*p++);
    switch (attribute)
    {
      case 'a':
      case 'A':
      {
        double
          angle;

        PointInfo
          arc;

        unsigned int
          large_arc,
          sweep;

        /*
          Compute arc points.
        */
        GetToken(p,&p,token);
        if (*token == ',')
          GetToken(p,&p,token);
        arc.x=atof(token);
        GetToken(p,&p,token);
        if (*token == ',')
          GetToken(p,&p,token);
        arc.y=atof(token);
        GetToken(p,&p,token);
        if (*token == ',')
          GetToken(p,&p,token);
        angle=atof(token);
        GetToken(p,&p,token);
        if (*token == ',')
          GetToken(p,&p,token);
        large_arc=atoi(token);
        GetToken(p,&p,token);
        if (*token == ',')
          GetToken(p,&p,token);
        sweep=atoi(token);
        GetToken(p,&p,token);
        if (*token == ',')
          GetToken(p,&p,token);
        x=atof(token);
        GetToken(p,&p,token);
        if (*token == ',')
          GetToken(p,&p,token);
        y=atof(token);
        end.x=attribute == 'A' ? x : point.x+x;
        end.y=attribute == 'A' ? y : point.y+y;
        TraceArcPath(q,point,end,arc,angle,large_arc,sweep);
        q+=q->coordinates;
        point=end;
        break;
      }
      case 'c':
      case 'C':
      {
        /*
          Compute bezier points.
        */
        do
        {
          points[0]=point;
          for (i=1; i <= 3; i++)
          {
            GetToken(p,&p,token);
            if (*token == ',')
              GetToken(p,&p,token);
            x=atof(token);
            GetToken(p,&p,token);
            if (*token == ',')
              GetToken(p,&p,token);
            y=atof(token);
            end.x=attribute == 'C' ? x : point.x+x;
            end.y=attribute == 'C' ? y : point.y+y;
            points[i]=end;
          }
          for (i=0; i <= 3; i++)
            (q+i)->point=points[i];
          TraceBezier(q,4);
          q+=q->coordinates;
          point=end;
        } while (IsPoint(p));
        break;
      }
      case 'H':
      case 'h':
      {
        do
        {
          GetToken(p,&p,token);
          if (*token == ',')
            GetToken(p,&p,token);
          x=atof(token);
          point.x=attribute == 'H' ? x: point.x+x;
          TracePoint(q,point);
          q+=q->coordinates;
        } while (IsPoint(p));
        break;
      }
      case 'l':
      case 'L':
      {
        do
        {
          GetToken(p,&p,token);
          if (*token == ',')
            GetToken(p,&p,token);
          x=atof(token);
          GetToken(p,&p,token);
          if (*token == ',')
            GetToken(p,&p,token);
          y=atof(token);
          point.x=attribute == 'L' ? x : point.x+x;
          point.y=attribute == 'L' ? y : point.y+y;
          TracePoint(q,point);
          q+=q->coordinates;
        } while (IsPoint(p));
        break;
      }
      case 'M':
      case 'm':
      {
        if (q != primitive_info)
          {
            primitive_info->coordinates=q-primitive_info;
            number_coordinates+=primitive_info->coordinates;
            primitive_info=q;
          }
        do
        {
          GetToken(p,&p,token);
          if (*token == ',')
            GetToken(p,&p,token);
          x=atof(token);
          GetToken(p,&p,token);
          if (*token == ',')
            GetToken(p,&p,token);
          y=atof(token);
          point.x=attribute == 'M' ? x : point.x+x;
          point.y=attribute == 'M' ? y : point.y+y;
          TracePoint(q,point);
          q+=q->coordinates;
        } while (IsPoint(p));
        start=point;
        break;
      }
      case 'q':
      case 'Q':
      {
        /*
          Compute bezier points.
        */
        do
        {
          points[0]=point;
          for (i=1; i < 3; i++)
          {
            GetToken(p,&p,token);
            if (*token == ',')
              GetToken(p,&p,token);
            x=atof(token);
            GetToken(p,&p,token);
            if (*token == ',')
              GetToken(p,&p,token);
            y=atof(token);
            if (*p == ',')
              p++;
            end.x=attribute == 'Q' ? x : point.x+x;
            end.y=attribute == 'Q' ? y : point.y+y;
            points[i]=end;
          }
          for (i=0; i < 3; i++)
            (q+i)->point=points[i];
          TraceBezier(q,3);
          q+=q->coordinates;
          point=end;
        } while (IsPoint(p));
        break;
      }
      case 's':
      case 'S':
      {
        /*
          Compute bezier points.
        */
        do
        {
          points[0]=points[3];
          points[1].x=2.0*points[3].x-points[2].x;
          points[1].y=2.0*points[3].y-points[2].y;
          for (i=2; i <= 3; i++)
          {
            GetToken(p,&p,token);
            if (*token == ',')
              GetToken(p,&p,token);
            x=atof(token);
            GetToken(p,&p,token);
            if (*token == ',')
              GetToken(p,&p,token);
            y=atof(token);
            if (*p == ',')
              p++;
            end.x=attribute == 'S' ? x : point.x+x;
            end.y=attribute == 'S' ? y : point.y+y;
            points[i]=end;
          }
          if (strchr("CcSs",last_attribute) == (char *) NULL)
            {
              points[0]=points[2];
              points[1]=points[3];
            }
          for (i=0; i <= 4; i++)
            (q+i)->point=points[i];
          TraceBezier(q,4);
          q+=q->coordinates;
          point=end;
        } while (IsPoint(p));
        break;
      }
      case 't':
      case 'T':
      {
        /*
          Compute bezier points.
        */
        do
        {
          points[0]=points[2];
          points[1].x=2.0*points[2].x-points[1].x;
          points[1].y=2.0*points[2].y-points[1].y;
          for (i=2; i < 3; i++)
          {
            GetToken(p,&p,token);
            if (*token == ',')
              GetToken(p,&p,token);
            x=atof(token);
            GetToken(p,&p,token);
            if (*token == ',')
              GetToken(p,&p,token);
            y=atof(token);
            end.x=attribute == 'T' ? x : point.x+x;
            end.y=attribute == 'T' ? y : point.y+y;
            points[i]=end;
          }
          if (strchr("QqTt",last_attribute) == (char *) NULL)
            {
              points[0]=points[2];
              points[1]=points[3];
            }
          for (i=0; i < 3; i++)
            (q+i)->point=points[i];
          TraceBezier(q,3);
          q+=q->coordinates;
          point=end;
        } while (IsPoint(p));
        break;
      }
      case 'v':
      case 'V':
      {
        do
        {
          GetToken(p,&p,token);
          if (*token == ',')
            GetToken(p,&p,token);
          y=atof(token);
          point.y=attribute == 'V' ? y : point.y+y;
          TracePoint(q,point);
          q+=q->coordinates;
        } while (IsPoint(p));
        break;
      }
      case 'z':
      case 'Z':
      {
        point=start;
        TracePoint(q,point);
        q+=q->coordinates;
        primitive_info->coordinates=q-primitive_info;
        number_coordinates+=primitive_info->coordinates;
        primitive_info=q;
        z_count++;
        break;
      }
      default:
      {
        if (isalpha((int) attribute))
          (void) fprintf(stderr,"attribute not recognized: %c\n",attribute);
        break;
      }
    }
  }
  primitive_info->coordinates=q-primitive_info;
  number_coordinates+=primitive_info->coordinates;
  for (i=0; i < (long) number_coordinates; i++)
  {
    q--;
    q->primitive=primitive_type;
    if (z_count > 1)
      q->method=FillToBorderMethod;
  }
  q=primitive_info;
  return(number_coordinates);
}

static void TracePoint(PrimitiveInfo *primitive_info,const PointInfo point)
{
  primitive_info->coordinates=1;
  primitive_info->point=point;
}

static void TraceRectangle(PrimitiveInfo *primitive_info,const PointInfo start,
  const PointInfo end)
{
  PointInfo
    point;

  register PrimitiveInfo
    *p;

  register long
    i;

  p=primitive_info;
  TracePoint(p,start);
  p+=p->coordinates;
  point.x=start.x;
  point.y=end.y;
  TracePoint(p,point);
  p+=p->coordinates;
  TracePoint(p,end);
  p+=p->coordinates;
  point.x=end.x;
  point.y=start.y;
  TracePoint(p,point);
  p+=p->coordinates;
  TracePoint(p,start);
  p+=p->coordinates;
  primitive_info->coordinates=p-primitive_info;
  for (i=0; i < (long) primitive_info->coordinates; i++)
  {
    p->primitive=primitive_info->primitive;
    p--;
  }
}

static void TraceRoundRectangle(PrimitiveInfo *primitive_info,
  const PointInfo start,const PointInfo end,PointInfo arc)
{
  PointInfo
    degrees,
    offset,
    point;

  register PrimitiveInfo
    *p;

  register long
    i;

  p=primitive_info;
  offset.x=AbsoluteValue(end.x-start.x);
  offset.y=AbsoluteValue(end.y-start.y);
  if (arc.x > (0.5*offset.x))
    arc.x=0.5*offset.x;
  if (arc.y > (0.5*offset.y))
    arc.y=0.5*offset.y;
  point.x=start.x+offset.x-arc.x;
  point.y=start.y+arc.y;
  degrees.x=270.0;
  degrees.y=360.0;
  TraceEllipse(p,point,arc,degrees);
  p+=p->coordinates;
  point.x=start.x+offset.x-arc.x;
  point.y=start.y+offset.y-arc.y;
  degrees.x=0.0;
  degrees.y=90.0;
  TraceEllipse(p,point,arc,degrees);
  p+=p->coordinates;
  point.x=start.x+arc.x;
  point.y=start.y+offset.y-arc.y;
  degrees.x=90.0;
  degrees.y=180.0;
  TraceEllipse(p,point,arc,degrees);
  p+=p->coordinates;
  point.x=start.x+arc.x;
  point.y=start.y+arc.y;
  degrees.x=180.0;
  degrees.y=270.0;
  TraceEllipse(p,point,arc,degrees);
  p+=p->coordinates;
  TracePoint(p,primitive_info->point);
  p+=p->coordinates;
  primitive_info->coordinates=p-primitive_info;
  for (i=0; i < (long) primitive_info->coordinates; i++)
  {
    p->primitive=primitive_info->primitive;
    p--;
  }
}

static void TraceSquareLinecap(PrimitiveInfo *primitive_info,
  const unsigned long number_vertices,const double offset)
{
  double
    distance;

  long
    j;

  register double
    dx,
    dy;

  register long
    i;

  dx=0.0;
  dy=0.0;
  for (i=1; i < (long) number_vertices; i++)
  {
    dx=primitive_info[0].point.x-primitive_info[i].point.x;
    dy=primitive_info[0].point.y-primitive_info[i].point.y;
    if ((fabs(dx) >= MagickEpsilon) || (fabs(dy) >= MagickEpsilon))
      break;
  }
  distance=sqrt(dx*dx+dy*dy+MagickEpsilon);
  primitive_info[0].point.x=primitive_info[i].point.x+
    dx*(distance+offset)/distance;
  primitive_info[0].point.y=primitive_info[i].point.y+
    dy*(distance+offset)/distance;
  for (j=(long) number_vertices-2; j >= 0;  j--)
  {
    dx=primitive_info[number_vertices-1].point.x-primitive_info[j].point.x;
    dy=primitive_info[number_vertices-1].point.y-primitive_info[j].point.y;
    if ((fabs(dx) >= MagickEpsilon) || (fabs(dy) >= MagickEpsilon))
      break;
  }
  distance=sqrt(dx*dx+dy*dy+MagickEpsilon);
  primitive_info[number_vertices-1].point.x=primitive_info[j].point.x+
    dx*(distance+offset)/distance;
  primitive_info[number_vertices-1].point.y=primitive_info[j].point.y+
    dy*(distance+offset)/distance;
}

static PrimitiveInfo *TraceStrokePolygon(const DrawInfo *draw_info,
  const PrimitiveInfo *primitive_info)
{
  typedef struct _LineSegment
  {
    double
      p,
      q;
  } LineSegment;

  double
    delta_theta,
    dot_product,
    mid,
    miterlimit;

  LineSegment
    dx={0,0},
    dy={0,0},
    inverse_slope={0,0},
    slope={0,0},
    theta={0,0};

  PointInfo
    box_p[5],
    box_q[5],
    center,
    offset,
    *path_p,
    *path_q;

  PrimitiveInfo
    *polygon_primitive,
    *stroke_polygon;

  register long
    i;

  unsigned int
    closed_path;

  unsigned long
    arc_segments,
    j,
    n,
    max_strokes,
    number_vertices,
    p,
    q;

  /*
    Allocate paths.
  */
  number_vertices=primitive_info->coordinates;
  max_strokes=2*number_vertices+6*BezierQuantum+360;
  path_p=MagickAllocateMemory(PointInfo *,max_strokes*sizeof(PointInfo));
  path_q=MagickAllocateMemory(PointInfo *,max_strokes*sizeof(PointInfo));
  polygon_primitive=MagickAllocateMemory(PrimitiveInfo *,
    (number_vertices+2)*sizeof(PrimitiveInfo));
  if ((path_p == (PointInfo *) NULL) || (path_q == (PointInfo *) NULL) ||
      (polygon_primitive == (PrimitiveInfo *) NULL))
    return((PrimitiveInfo *) NULL);
  (void) memcpy(polygon_primitive,primitive_info,number_vertices*
    sizeof(PrimitiveInfo));
  closed_path=
    (primitive_info[number_vertices-1].point.x == primitive_info[0].point.x) &&
    (primitive_info[number_vertices-1].point.y == primitive_info[0].point.y);
  if ((draw_info->linejoin == RoundJoin) ||
      ((draw_info->linejoin == MiterJoin) && closed_path))
    {
      polygon_primitive[number_vertices]=primitive_info[1];
      number_vertices++;
    }
  polygon_primitive[number_vertices].primitive=UndefinedPrimitive;
  /*
    Compute the slope for the first line segment, p.
  */
  for (n=1; n < number_vertices; n++)
  {
    dx.p=polygon_primitive[n].point.x-polygon_primitive[0].point.x;
    dy.p=polygon_primitive[n].point.y-polygon_primitive[0].point.y;
    if ((fabs(dx.p) >= MagickEpsilon) || (fabs(dy.p) >= MagickEpsilon))
      break;
  }
  slope.p=0.0;
  inverse_slope.p=0.0;
  if (fabs(dx.p) <= MagickEpsilon)
    {
      if (dx.p >= 0.0)
        slope.p=dy.p < 0.0 ? -1.0/MagickEpsilon : 1.0/MagickEpsilon;
      else
        slope.p=dy.p < 0.0 ? 1.0/MagickEpsilon : -1.0/MagickEpsilon;
    }
  else
    if (fabs(dy.p) <= MagickEpsilon)
      {
        if (dy.p >= 0.0)
          inverse_slope.p=dx.p < 0.0 ? -1.0/MagickEpsilon : 1.0/MagickEpsilon;
        else
          inverse_slope.p=dx.p < 0.0 ? 1.0/MagickEpsilon : -1.0/MagickEpsilon;
      }
    else
      {
        slope.p=dy.p/dx.p;
        inverse_slope.p=(-1.0/slope.p);
      }
  mid=ExpandAffine(&draw_info->affine)*draw_info->stroke_width/2.0;
  miterlimit=draw_info->miterlimit*draw_info->miterlimit*mid*mid;
  if ((draw_info->linecap == SquareCap) && !closed_path)
    TraceSquareLinecap(polygon_primitive,number_vertices,mid);
  offset.x=sqrt(mid*mid/(inverse_slope.p*inverse_slope.p+1.0));
  offset.y=offset.x*inverse_slope.p;
  if ((dy.p*offset.x-dx.p*offset.y) > 0.0)
    {
      box_p[0].x=polygon_primitive[0].point.x-offset.x;
      box_p[0].y=polygon_primitive[0].point.y-offset.x*inverse_slope.p;
      box_p[1].x=polygon_primitive[n].point.x-offset.x;
      box_p[1].y=polygon_primitive[n].point.y-offset.x*inverse_slope.p;
      box_q[0].x=polygon_primitive[0].point.x+offset.x;
      box_q[0].y=polygon_primitive[0].point.y+offset.x*inverse_slope.p;
      box_q[1].x=polygon_primitive[n].point.x+offset.x;
      box_q[1].y=polygon_primitive[n].point.y+offset.x*inverse_slope.p;
    }
  else
    {
      box_p[0].x=polygon_primitive[0].point.x+offset.x;
      box_p[0].y=polygon_primitive[0].point.y+offset.y;
      box_p[1].x=polygon_primitive[n].point.x+offset.x;
      box_p[1].y=polygon_primitive[n].point.y+offset.y;
      box_q[0].x=polygon_primitive[0].point.x-offset.x;
      box_q[0].y=polygon_primitive[0].point.y-offset.y;
      box_q[1].x=polygon_primitive[n].point.x-offset.x;
      box_q[1].y=polygon_primitive[n].point.y-offset.y;
    }
  /*
    Create strokes for the line join attribute: bevel, miter, round.
  */
  p=0;
  q=0;
  path_q[p++]=box_q[0];
  path_p[q++]=box_p[0];
  for (i=(long) n+1; i < (long) number_vertices; i++)
  {
    /*
      Compute the slope for this line segment, q.
    */
    dx.q=polygon_primitive[i].point.x-polygon_primitive[n].point.x;
    dy.q=polygon_primitive[i].point.y-polygon_primitive[n].point.y;
    dot_product=dx.q*dx.q+dy.q*dy.q;
    if (dot_product < 0.25)
      continue;
    slope.q=0.0;
    inverse_slope.q=0.0;
    if (fabs(dx.q) < MagickEpsilon)
      {
        if (dx.q >= 0.0)
          slope.q=dy.q < 0.0 ? -1.0/MagickEpsilon : 1.0/MagickEpsilon;
        else
          slope.q=dy.q < 0.0 ? 1.0/MagickEpsilon : -1.0/MagickEpsilon;
      }
    else
      if (fabs(dy.q) <= MagickEpsilon)
        {
          if (dy.q >= 0.0)
            inverse_slope.q=dx.q < 0.0 ? -1.0/MagickEpsilon : 1.0/MagickEpsilon;
          else
            inverse_slope.q=dx.q < 0.0 ? 1.0/MagickEpsilon : -1.0/MagickEpsilon;
        }
      else
        {
          slope.q=dy.q/dx.q;
          inverse_slope.q=(-1.0/slope.q);
        }
    offset.x=sqrt(mid*mid/(inverse_slope.q*inverse_slope.q+1.0));
    offset.y=offset.x*inverse_slope.q;
    dot_product=dy.q*offset.x-dx.q*offset.y;
    if (dot_product > 0.0)
      {
        box_p[2].x=polygon_primitive[n].point.x-offset.x;
        box_p[2].y=polygon_primitive[n].point.y-offset.y;
        box_p[3].x=polygon_primitive[i].point.x-offset.x;
        box_p[3].y=polygon_primitive[i].point.y-offset.y;
        box_q[2].x=polygon_primitive[n].point.x+offset.x;
        box_q[2].y=polygon_primitive[n].point.y+offset.y;
        box_q[3].x=polygon_primitive[i].point.x+offset.x;
        box_q[3].y=polygon_primitive[i].point.y+offset.y;
      }
    else
      {
        box_p[2].x=polygon_primitive[n].point.x+offset.x;
        box_p[2].y=polygon_primitive[n].point.y+offset.y;
        box_p[3].x=polygon_primitive[i].point.x+offset.x;
        box_p[3].y=polygon_primitive[i].point.y+offset.y;
        box_q[2].x=polygon_primitive[n].point.x-offset.x;
        box_q[2].y=polygon_primitive[n].point.y-offset.y;
        box_q[3].x=polygon_primitive[i].point.x-offset.x;
        box_q[3].y=polygon_primitive[i].point.y-offset.y;
      }
    if (fabs(slope.p-slope.q) <= MagickEpsilon)
      {
        box_p[4]=box_p[1];
        box_q[4]=box_q[1];
      }
    else
      {
        box_p[4].x=(slope.p*box_p[0].x-box_p[0].y-slope.q*box_p[3].x+
          box_p[3].y)/(slope.p-slope.q);
        box_p[4].y=slope.p*(box_p[4].x-box_p[0].x)+box_p[0].y;
        box_q[4].x=(slope.p*box_q[0].x-box_q[0].y-slope.q*box_q[3].x+
          box_q[3].y)/(slope.p-slope.q);
        box_q[4].y=slope.p*(box_q[4].x-box_q[0].x)+box_q[0].y;
      }
    if (q >= (max_strokes-6*BezierQuantum-360))
      {
         max_strokes+=6*BezierQuantum+360;
         MagickReallocMemory(PointInfo *,path_p,max_strokes*sizeof(PointInfo));
         MagickReallocMemory(PointInfo *,path_q,max_strokes*sizeof(PointInfo));
         if ((path_p == (PointInfo *) NULL) || (path_q == (PointInfo *) NULL))
           {
             MagickFreeMemory(polygon_primitive);
             return((PrimitiveInfo *) NULL);
           }
      }
    dot_product=dx.q*dy.p-dx.p*dy.q;
    if (dot_product <= 0.0)
      switch (draw_info->linejoin)
      {
        case BevelJoin:
        {
          path_q[q++]=box_q[1];
          path_q[q++]=box_q[2];
          dot_product=(box_q[4].x-box_p[4].x)*(box_q[4].x-box_p[4].x)+
            (box_q[4].y-box_p[4].y)*(box_q[4].y-box_p[4].y);
          if (dot_product <= miterlimit)
            path_p[p++]=box_p[4];
          else
            {
              path_p[p++]=box_p[1];
              path_p[p++]=box_p[2];
            }
          break;
        }
        case MiterJoin:
        {
          dot_product=(box_q[4].x-box_p[4].x)*(box_q[4].x-box_p[4].x)+
            (box_q[4].y-box_p[4].y)*(box_q[4].y-box_p[4].y);
          if (dot_product > miterlimit)
            {
              path_q[q++]=box_q[1];
              path_q[q++]=box_q[2];
              path_p[p++]=box_p[1];
              path_p[p++]=box_p[2];
           }
         else
           {
             path_q[q++]=box_q[4];
             path_p[p++]=box_p[4];
           }
          break;
        }
        case RoundJoin:
        {
          dot_product=(box_q[4].x-box_p[4].x)*(box_q[4].x-box_p[4].x)+
            (box_q[4].y-box_p[4].y)*(box_q[4].y-box_p[4].y);
          if (dot_product <= miterlimit)
            path_p[p++]=box_p[4];
          else
            {
              path_p[p++]=box_p[1];
              path_p[p++]=box_p[2];
            }
          center=polygon_primitive[n].point;
          theta.p=atan2(box_q[1].y-center.y,box_q[1].x-center.x);
          theta.q=atan2(box_q[2].y-center.y,box_q[2].x-center.x);
          if (theta.q < theta.p)
            theta.q+=2.0*MagickPI;
          arc_segments=(long) ceil((theta.q-theta.p)/(2.0*sqrt(1.0/mid)));
          path_q[q].x=box_q[1].x;
          path_q[q].y=box_q[1].y;
          q++;
          for (j=1; j < arc_segments; j++)
          {
            delta_theta=j*(theta.q-theta.p)/arc_segments;
            path_q[q].x=center.x+mid*
              cos(fmod(theta.p+delta_theta,DegreesToRadians(360.0)));
            path_q[q].y=center.y+mid*
              sin(fmod(theta.p+delta_theta,DegreesToRadians(360.0)));
            q++;
          }
          path_q[q++]=box_q[2];
          break;
        }
        default:
          break;
      }
    else
      switch (draw_info->linejoin)
      {
        case BevelJoin:
        {
          path_p[p++]=box_p[1];
          path_p[p++]=box_p[2];
          dot_product=(box_q[4].x-box_p[4].x)*(box_q[4].x-box_p[4].x)+
            (box_q[4].y-box_p[4].y)*(box_q[4].y-box_p[4].y);
          if (dot_product <= miterlimit)
            path_q[q++]=box_q[4];
          else
            {
              path_q[q++]=box_q[1];
              path_q[q++]=box_q[2];
            }
          break;
        }
        case MiterJoin:
        {
          dot_product=(box_q[4].x-box_p[4].x)*(box_q[4].x-box_p[4].x)+
            (box_q[4].y-box_p[4].y)*(box_q[4].y-box_p[4].y);
          if (dot_product <= miterlimit)
            {
              path_q[q++]=box_q[4];
              path_p[p++]=box_p[4];
            }
          else
            {
              path_q[q++]=box_q[1];
              path_q[q++]=box_q[2];
              path_p[p++]=box_p[1];
              path_p[p++]=box_p[2];
            }
          break;
        }
        case RoundJoin:
        {
          dot_product=(box_q[4].x-box_p[4].x)*(box_q[4].x-box_p[4].x)+
            (box_q[4].y-box_p[4].y)*(box_q[4].y-box_p[4].y);
          if (dot_product <= miterlimit)
            path_q[q++]=box_q[4];
          else
            {
              path_q[q++]=box_q[1];
              path_q[q++]=box_q[2];
            }
          center=polygon_primitive[n].point;
          theta.p=atan2(box_p[1].y-center.y,box_p[1].x-center.x);
          theta.q=atan2(box_p[2].y-center.y,box_p[2].x-center.x);
          if (theta.p < theta.q)
            theta.p+=2.0*MagickPI;
          arc_segments=(long) ceil((theta.p-theta.q)/(2.0*sqrt(1.0/mid)));
          path_p[p++]=box_p[1];
          for (j=1; j < arc_segments; j++)
          {
            delta_theta=j*(theta.q-theta.p)/arc_segments;
            path_p[p].x=center.x+mid*
              cos(fmod(theta.p+delta_theta,DegreesToRadians(360.0)));
            path_p[p].y=center.y+mid*
              sin(fmod(theta.p+delta_theta,DegreesToRadians(360.0)));
            p++;
          }
          path_p[p++]=box_p[2];
          break;
        }
        default:
          break;
      }
    slope.p=slope.q;
    inverse_slope.p=inverse_slope.q;
    box_p[0]=box_p[2];
    box_p[1]=box_p[3];
    box_q[0]=box_q[2];
    box_q[1]=box_q[3];
    dx.p=dx.q;
    dy.p=dy.q;
    n=i;
  }
  path_p[p++]=box_p[1];
  path_q[q++]=box_q[1];
  /*
    Trace stroked polygon.
  */
  stroke_polygon=MagickAllocateMemory(PrimitiveInfo *,
    (p+q+2*closed_path+2)*sizeof(PrimitiveInfo));
  if (stroke_polygon != (PrimitiveInfo *) NULL)
    {
      for (i=0; i < (long) p; i++)
      {
        stroke_polygon[i]=polygon_primitive[0];
        stroke_polygon[i].point=path_p[i];
      }
      if (closed_path)
        {
          stroke_polygon[i]=polygon_primitive[0];
          stroke_polygon[i].point=stroke_polygon[0].point;
          i++;
        }
      for ( ; i < (long) (p+q+closed_path); i++)
      {
        stroke_polygon[i]=polygon_primitive[0];
        stroke_polygon[i].point=path_q[p+q+closed_path-(i+1)];
      }
      if (closed_path)
        {
          stroke_polygon[i]=polygon_primitive[0];
          stroke_polygon[i].point=stroke_polygon[p+closed_path].point;
          i++;
        }
      stroke_polygon[i]=polygon_primitive[0];
      stroke_polygon[i].point=stroke_polygon[0].point;
      i++;
      stroke_polygon[i].primitive=UndefinedPrimitive;
      stroke_polygon[0].coordinates=p+q+2*closed_path+1;
    }
  MagickFreeMemory(path_p);
  MagickFreeMemory(path_q);
  MagickFreeMemory(polygon_primitive);
  return(stroke_polygon);
}
