/*
% Copyright (C) 2003 - 2009 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                       CCCC   OOO   L       OOO   RRRR                       %
%                      C      O   O  L      O   O  R   R                      %
%                      C      O   O  L      O   O  RRRR                       %
%                      C      O   O  L      O   O  R R                        %
%                       CCCC   OOO   LLLLL   OOO   R  R                       %
%                                                                             %
%                                                                             %
%                  Methods to Count the Colors in an Image                    %
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
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/blob.h"
#include "magick/color.h"
#include "magick/color_lookup.h"
#include "magick/monitor.h"
#include "magick/pixel_cache.h"
#include "magick/quantize.h"
#include "magick/semaphore.h"
#include "magick/utility.h"

/*
  Define declarations.
*/
#define ColorToNodeId(red,green,blue,index) ((unsigned int) \
            (((ScaleQuantumToChar(red) >> index) & 0x01U) << 2U | \
             ((ScaleQuantumToChar(green) >> index) & 0x01U) << 1U | \
             ((ScaleQuantumToChar(blue) >> index) & 0x01U)))

/*
  Structures.
*/
typedef struct _ColorPacket
{
  PixelPacket
    pixel;

  unsigned short
    index;

  unsigned long
    count;
} ColorPacket;

typedef struct _NodeInfo
{
  struct _NodeInfo
    *child[8];

  ColorPacket
    *list;

  unsigned long
    number_unique;

  unsigned char
    level;
} NodeInfo;

typedef struct _Nodes
{
  NodeInfo
    nodes[NodesInAList];

  struct _Nodes
    *next;
} Nodes;

typedef struct _CubeInfo
{
  NodeInfo
    *root;

  unsigned long
    progress,
    colors,
    free_nodes;

  NodeInfo
    *node_info;

  Nodes
    *node_queue;
} CubeInfo;

/*
  Static declarations.
*/

/*
  Forward declarations.
*/
static CubeInfo
  *ComputeCubeInfo(const Image *image,ExceptionInfo *exception),
  *GetCubeInfo(void);

static NodeInfo
  *GetNodeInfo(CubeInfo *,const unsigned int);

static void
  DestroyColorList(NodeInfo *node_info),
  HistogramToFile(const Image *,CubeInfo *,const NodeInfo *,FILE *,ExceptionInfo *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   C o m p u t e C u b e I n f o                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ComputeCubeInfo builds a populated CubeInfo tree for the specified
%  image.  The returned tree should be deallocated using DestroyCubeInfo()
%  once it is no longer needed.
%
%  The format of the ComputeCubeInfo method is:
%
%      CubeInfo *ComputeCubeInfo(const Image *image,ExceptionInfo *exception)
%
%
*/
static CubeInfo *
ComputeCubeInfo(const Image *image,ExceptionInfo *exception)
{
#define ComputeImageColorsText  "[%s] Compute colors..."

  CubeInfo
    *cube_info;
  
  long
    y;
  
  NodeInfo
    *node_info;
  
  register const PixelPacket
    *p;
  
  register long
    i,
    x;

  register unsigned int
    id,
    index,
    level;

  /*
    Initialize color description tree.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  cube_info=GetCubeInfo();
  if (cube_info == (CubeInfo *) NULL)
    {
      ThrowException3(exception,ResourceLimitError,MemoryAllocationFailed,
                      UnableToDetermineTheNumberOfImageColors);
      return(0);
    }
  for (y=0; y < (long) image->rows; y++)
    {
      p=AcquireImagePixels(image,0,y,image->columns,1,exception);
      if (p == (const PixelPacket *) NULL)
        return(0);
      for (x=0; x < (long) image->columns; x++)
        {
          /*
            Start at the root and proceed level by level.
          */
          node_info=cube_info->root;
          index=MaxTreeDepth-1;
          for (level=1; level <= MaxTreeDepth; level++)
            {
              id=ColorToNodeId(p->red,p->green,p->blue,index);
              if (node_info->child[id] == (NodeInfo *) NULL)
                {
                  node_info->child[id]=GetNodeInfo(cube_info,level);
                  if (node_info->child[id] == (NodeInfo *) NULL)
                    {
                      ThrowException3(exception,ResourceLimitError,
                                      MemoryAllocationFailed,
                                      UnableToDetermineTheNumberOfImageColors);
                      return(0);
                    }
                }
              node_info=node_info->child[id];
              index--;
              if (level != MaxTreeDepth)
                continue;
              for (i=0; i < (long) node_info->number_unique; i++)
                if (ColorMatch(p,&node_info->list[i].pixel))
                  break;
              if (i < (long) node_info->number_unique)
                {
                  node_info->list[i].count++;
                  continue;
                }
              if (node_info->number_unique == 0)
                node_info->list=MagickAllocateMemory(ColorPacket *,
                                                     sizeof(ColorPacket));
              else
                MagickReallocMemory(ColorPacket *,node_info->list,
                                    (i+1)*sizeof(ColorPacket));
              if (node_info->list == (ColorPacket *) NULL)
                {
                  ThrowException3(exception,ResourceLimitError,
                                  MemoryAllocationFailed,
                                  UnableToDetermineTheNumberOfImageColors);
                  return(0);
                }
              node_info->list[i].pixel=(*p);
              node_info->list[i].count=1;
              node_info->number_unique++;
              cube_info->colors++;
            }
          p++;
        }
      if (QuantumTick(y,image->rows))
        if (!MagickMonitorFormatted(y,image->rows,exception,
                                    ComputeImageColorsText,image->filename))
          break;
    }
  return (cube_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y C u b e I n f o                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyCubeInfo() deallocates memory associated with a CubeInfo structure.
%
%  The format of the DestroyCubeInfo method is:
%
%      DestroyCubeInfo(CubeInfo *cube_info)
%
%  A description of each parameter follows:
%
%    o cube_info: The address of a structure of type CubeInfo.
%
%
*/
static void DestroyCubeInfo(CubeInfo *cube_info)
{
  register Nodes
    *nodes;

  /*
    Release color cube tree storage.
  */
  DestroyColorList(cube_info->root);
  do
  {
    nodes=cube_info->node_queue->next;
    MagickFreeMemory(cube_info->node_queue);
    cube_info->node_queue=nodes;
  } while (cube_info->node_queue != (Nodes *) NULL);
  MagickFreeMemory(cube_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  D e s t r o y C o l o r L i s t                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyColorList traverses the color cube tree and frees the list of
%  unique colors.
%
%  The format of the DestroyColorList method is:
%
%      void DestroyColorList(const NodeInfo *node_info)
%
%  A description of each parameter follows.
%
%    o node_info: The address of a structure of type NodeInfo which points to a
%      node in the color cube tree that is to be pruned.
%
%
*/
static void DestroyColorList(NodeInfo *node_info)
{
  register unsigned int
    id;

  /*
    Traverse any children.
  */
  for (id=0; id < 8; id++)
    if (node_info->child[id] != (NodeInfo *) NULL)
      DestroyColorList(node_info->child[id]);
  MagickFreeMemory(node_info->list);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   F u z z y C o l o r M a t c h                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  FuzzyColorMatch() returns true if the distance between two colors is
%  less than the specified distance in a linear three dimensional color space.
%  This function is used by ColorFloodFill and other algorithms which
%  compare two colors.
%
%  The format of the FuzzyColorMatc method is:
%
%      MagickBool FuzzyColorMatch(const PixelPacket *p,
%                                 const PixelPacket *q,const double fuzz)
%
%  A description of each parameter follows:
%
%    o p: Pixel p.
%
%    o q: Pixel q.
%
%    o fuzz:  Define how much difference is acceptable in order to
%      consider two colors to be the same.
%
%
*/
MagickExport MagickBool FuzzyColorMatch(const PixelPacket *p,
  const PixelPacket *q,const double fuzz)
{
  double
    difference,
    distance,
    fuzz_squared;

  if (fuzz <= MagickEpsilon)
    return (ColorMatch(q,p));
  fuzz_squared=fuzz*fuzz;
  difference=p->red-(double) q->red;
  distance=difference*difference;
  if (distance > (fuzz_squared))
    return(MagickFalse);
  difference=p->green-(double) q->green;
  distance+=difference*difference;
  if (distance > (fuzz_squared))
    return(MagickFalse);
  difference=p->blue-(double) q->blue;
  distance+=difference*difference;
  if (distance > (fuzz_squared))
    return(MagickFalse);
  return(MagickTrue);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  G e t C o l o r H i s t o g r a m                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetColorHistogram returns an array of HistogramColorPacket structures
%  which specify the number of times each unique color occurs in the image.
%  The referenced colors parameter is updated with the number of unique colors
%  in the image. The returned array should be deallocated by the user once it
%  is no longer ndded.
%
%  The format of the GetColorHistogram method is:
%
%      HistogramColorPacket *GetColorHistogram(const Image *,
%                          unsigned long *colors, ExceptionInfo *)
%
%  A description of each parameter follows.
%
%    o image: The image.
%
%    o colors:  The referenced value is updated with the with the number of
%               unique colors.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static void HistogramToPacket(const Image *image,CubeInfo *cube_info,
  const NodeInfo *node_info,HistogramColorPacket **histogram_packet,
  ExceptionInfo *exception)
{
  register unsigned int
    id;

  /*
    Traverse any children.
  */
  for (id=0; id < 8; id++)
    if (node_info->child[id] != (NodeInfo *) NULL)
      HistogramToPacket(image,cube_info,node_info->child[id],histogram_packet,
                        exception);
  if (node_info->level == MaxTreeDepth)
    {
      register ColorPacket
        *p;

      register long
        i;

      p=node_info->list;
      for (i=0; i < (long) node_info->number_unique; i++)
        {
          (*histogram_packet)->pixel=p->pixel;
          (*histogram_packet)->count=p->count;
          (*histogram_packet)++;
          p++;
        }
    }
}
MagickExport HistogramColorPacket *GetColorHistogram(const Image *image,
  unsigned long *colors, ExceptionInfo *exception)
{
  CubeInfo
    *cube_info;

  HistogramColorPacket
    *current_packet,
    *histogram;

  unsigned long
    number_colors;

  /*
    Initialize color description tree.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  *colors=0;
  cube_info=ComputeCubeInfo(image,exception);
  if (cube_info == (CubeInfo *) NULL)
    return(0);

  number_colors=cube_info->colors;
  histogram=MagickAllocateMemory(HistogramColorPacket *,
    number_colors*sizeof(HistogramColorPacket));
  if (histogram == 0)
    {
      ThrowException3(exception,ResourceLimitError,MemoryAllocationFailed,
                      UnableToDetermineTheNumberOfImageColors);
      return 0;
    }
  *colors=number_colors;
  current_packet=histogram;
  HistogramToPacket(image,cube_info,cube_info->root,&current_packet,exception);
  DestroyCubeInfo(cube_info);
  return(histogram);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t C o l o r T u p l e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetColorTuple() returns a color as a color tuple string.
%
%  The format of the GetColorTuple method is:
%
%      GetColorTuple(const PixelPacket *color,const unsigned int depth,
%        const unsigned int matte,const unsigned int hex,char *tuple)
%
%  A description of each parameter follows.
%
%    o color: The color.
%
%    o depth: The color depth.
%
%    o matte: A value other than zero returns the opacity in the tuple.
%
%    o hex: A value other than zero returns the tuple in a hexidecimal format.
%
%    o tuple: Return the color tuple as this string.
%
%
*/
MagickExport void GetColorTuple(const PixelPacket *color,
  const unsigned int depth,const unsigned int matte,const unsigned int hex,
  char *tuple)
{
  assert(color != (const PixelPacket *) NULL);
  assert(tuple != (char *) NULL);
  if (matte)
    {
      if (depth <= 8)
        {
          FormatString(tuple,hex ? "#%02X%02X%02X%02X" : "(%3u,%3u,%3u,%3u)",
            ScaleQuantumToChar(color->red),ScaleQuantumToChar(color->green),
            ScaleQuantumToChar(color->blue),ScaleQuantumToChar(color->opacity));
          return;
        }
      if (depth <= 16)
        {
          FormatString(tuple,hex ? "#%04X%04X%04X%04X" : "(%5u,%5u,%5u,%5u)",
            ScaleQuantumToShort(color->red),ScaleQuantumToShort(color->green),
            ScaleQuantumToShort(color->blue),
            ScaleQuantumToShort(color->opacity));
          return;
        }
      FormatString(tuple,
        hex ? "#%08lX%08lX%08lX%08lX" : "(%10lu,%10lu,%10lu,%10lu)",
        ScaleQuantumToLong(color->red),ScaleQuantumToLong(color->green),
        ScaleQuantumToLong(color->blue),ScaleQuantumToLong(color->opacity));
      return;
    }
  if (depth <= 8)
    {
      FormatString(tuple,hex ? "#%02X%02X%02X" : "(%3u,%3u,%3u)",
        ScaleQuantumToChar(color->red),ScaleQuantumToChar(color->green),
        ScaleQuantumToChar(color->blue));
      return;
    }
  if (depth <= 16)
    {
      FormatString(tuple,hex ? "#%04X%04X%04X" : "(%5u,%5u,%5u)",
        ScaleQuantumToShort(color->red),ScaleQuantumToShort(color->green),
        ScaleQuantumToShort(color->blue));
      return;
    }
  FormatString(tuple,hex ? "#%08lX%08lX%08lX" : "(%10lu,%10lu,%10lu)",
    ScaleQuantumToLong(color->red),ScaleQuantumToLong(color->green),
    ScaleQuantumToLong(color->blue));
  return;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t C u b e I n f o                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetCubeInfo initialize the CubeInfo data structure.
%
%  The format of the GetCubeInfo method is:
%
%      cube_info=GetCubeInfo()
%
%  A description of each parameter follows.
%
%    o cube_info: A pointer to the Cube structure.
%
%
*/
static CubeInfo *GetCubeInfo(void)
{
  CubeInfo
    *cube_info;

  /*
    Initialize tree to describe color cube.
  */
  cube_info=MagickAllocateMemory(CubeInfo *,sizeof(CubeInfo));
  if (cube_info == (CubeInfo *) NULL)
    return((CubeInfo *) NULL);
  (void) memset(cube_info,0,sizeof(CubeInfo));
  /*
    Initialize root node.
  */
  cube_info->root=GetNodeInfo(cube_info,0);
  if (cube_info->root == (NodeInfo *) NULL)
    return((CubeInfo *) NULL);
  return(cube_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  G e t N o d e I n f o                                                      %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetNodeInfo allocates memory for a new node in the color cube tree
%  and presets all fields to zero.
%
%  The format of the GetNodeInfo method is:
%
%      node_info=GetNodeInfo(cube_info,level)
%
%  A description of each parameter follows.
%
%    o cube_info: A pointer to the CubeInfo structure.
%
%    o level: Specifies the level in the storage_class the node resides.
%
%
*/
static NodeInfo *GetNodeInfo(CubeInfo *cube_info,const unsigned int level)
{
  NodeInfo
    *node_info;

  if (cube_info->free_nodes == 0)
    {
      Nodes
        *nodes;

      /*
        Allocate a new nodes of nodes.
      */
      nodes=MagickAllocateMemory(Nodes *,sizeof(Nodes));
      if (nodes == (Nodes *) NULL)
        return((NodeInfo *) NULL);
      nodes->next=cube_info->node_queue;
      cube_info->node_queue=nodes;
      cube_info->node_info=nodes->nodes;
      cube_info->free_nodes=NodesInAList;
    }
  cube_info->free_nodes--;
  node_info=cube_info->node_info++;
  (void) memset(node_info,0,sizeof(NodeInfo));
  node_info->level=level;
  return(node_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  G e t N u m b e r C o l o r s                                              %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetNumberColors returns the number of unique colors in an image.
%
%  The format of the GetNumberColors method is:
%
%      unsigned long GetNumberColors(const Image *image,FILE *file,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o image: The image.
%
%    o file:  Write a histogram of the color distribution to this file handle.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned long GetNumberColors(const Image *image,FILE *file,
  ExceptionInfo *exception)
{
  CubeInfo
    *cube_info;

  unsigned long
    number_colors;

  /*
    Initialize color description tree.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  cube_info=ComputeCubeInfo(image,exception);
  if (cube_info == (CubeInfo *) NULL)
    return(0);

  if (file != (FILE *) NULL)
    {
      (void) fprintf(file,"\n");
      HistogramToFile(image,cube_info,cube_info->root,file,exception);
      (void) fflush(file);
    }
  number_colors=cube_info->colors;
  DestroyCubeInfo(cube_info);
  return(number_colors);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  H i s t o g r a m T o F i l e                                              %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method HistogramToFile traverses the color cube tree and produces a list of
%  unique pixel field values and the number of times each occurs in the image.
%
%  The format of the HistogramToFile method is:
%
%      void HistogramToFile(const Image *image,CubeInfo *cube_info,
%        const NodeInfo *node_info,FILE *file,ExceptionInfo *exception
%
%  A description of each parameter follows.
%
%    o cube_info: A pointer to the CubeInfo structure.
%
%    o node_info: The address of a structure of type NodeInfo which points to a
%      node in the color cube tree that is to be pruned.
%
%
*/
static void
HistogramToFile(const Image *image,CubeInfo *cube_info,
                const NodeInfo *node_info,FILE *file,
                ExceptionInfo *exception)
{
#define HistogramToFileImageText "[%s] Compute histogram..."

  register unsigned int
    id;

  /*
    Traverse any children.
  */
  for (id=0; id < 8; id++)
    if (node_info->child[id] != (NodeInfo *) NULL)
      HistogramToFile(image,cube_info,node_info->child[id],file,exception);
  if (node_info->level == MaxTreeDepth)
    {
      char
        name[MaxTextExtent],
        tuple[MaxTextExtent];

      register ColorPacket
        *p;

      register long
        i;

      p=node_info->list;
      for (i=0; i < (long) node_info->number_unique; i++)
        {
          GetColorTuple(&p->pixel,image->depth,image->matte,False,tuple);
          (void) fprintf(file,"%10lu: %.1024s  ",p->count,tuple);
          (void) fprintf(file,"  ");
          (void) QueryColorname(image,&p->pixel,SVGCompliance,name,exception);
          (void) fprintf(file,"%.1024s",name);
          (void) fprintf(file,"\n");
          p++;
        }
      if (QuantumTick(cube_info->progress,cube_info->colors))
        (void) MagickMonitorFormatted(cube_info->progress,
                                      cube_info->colors,exception,
                                      HistogramToFileImageText,
                                      image->filename);
      cube_info->progress++;
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  I s P a l e t t e I m a g e                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsPaletteImage returns True if the image is PseudoClass and has 256
%  unique colors or less.
%
%  The format of the IsPaletteImage method is:
%
%      MagickBool IsPaletteImage(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o status:  Method IsPaletteImage returns True is the image is
%      PseudoClass or has 256 color or less.
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
#define AnalyzePaletteImageText "[%s] Analyze for palette..."
MagickExport MagickBool IsPaletteImage(const Image *image,
  ExceptionInfo *exception)
{
  CubeInfo
    *cube_info;

  long
    y;

  register const PixelPacket
    *p;

  register long
    x;

  register NodeInfo
    *node_info;

  register long
    i;

  unsigned long
    index,
    level;

  unsigned int
    id;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if ((image->storage_class == PseudoClass) && (image->colors <= 256))
    return(MagickTrue);
  if (image->storage_class == PseudoClass)
    return(MagickFalse);
  /*
    Initialize color description tree.
  */
  cube_info=GetCubeInfo();
  if (cube_info == (CubeInfo *) NULL)
    {
      ThrowException3(exception,ResourceLimitError,MemoryAllocationFailed,
        UnableToDetermineImageClass);
      return(MagickFalse);
    }
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,exception);
    if (p == (const PixelPacket *) NULL)
      {
        DestroyCubeInfo(cube_info);
        cube_info=(CubeInfo *) NULL;
        return(MagickFalse);
      }
    for (x=0; x < (long) image->columns; x++)
    {
      /*
        Start at the root and proceed level by level.
      */
      node_info=cube_info->root;
      index=MaxTreeDepth-1;
      for (level=1; level < MaxTreeDepth; level++)
      {
        id=ColorToNodeId(p->red,p->green,p->blue,level);
        if (node_info->child[id] == (NodeInfo *) NULL)
          {
            node_info->child[id]=GetNodeInfo(cube_info,level);
            if (node_info->child[id] == (NodeInfo *) NULL)
              {
                ThrowException3(exception,ResourceLimitError,
                  MemoryAllocationFailed,UnableToDetermineImageClass);
                DestroyCubeInfo(cube_info);
                cube_info=(CubeInfo *) NULL;
                return(MagickFalse);
              }
          }
        node_info=node_info->child[id];
        index--;
      }
      for (i=0; i < (long) node_info->number_unique; i++)
        if (ColorMatch(p,&node_info->list[i].pixel))
          break;
      if (i == (long) node_info->number_unique)
        {
          /*
            Add this unique color to the color list.
          */
          if (node_info->number_unique == 0)
            node_info->list=MagickAllocateMemory(ColorPacket *,
                                                 sizeof(ColorPacket));
          else
            MagickReallocMemory(ColorPacket *,node_info->list,
              (i+1)*sizeof(ColorPacket));
          if (node_info->list == (ColorPacket *) NULL)
            {
              ThrowException3(exception,ResourceLimitError,
                MemoryAllocationFailed,UnableToDetermineImageClass);
              DestroyCubeInfo(cube_info);
                cube_info=(CubeInfo *) NULL;
              return(MagickFalse);
            }
          node_info->list[i].pixel=(*p);
          node_info->list[i].index=(unsigned short) cube_info->colors++;
          node_info->number_unique++;
          if (cube_info->colors > 256)
            {
              DestroyCubeInfo(cube_info);
              cube_info=(CubeInfo *) NULL;
              return(MagickFalse);
            }
        }
      p++;
    }
    if (QuantumTick(y,image->rows))
      if (!MagickMonitorFormatted(y,image->rows,exception,
                                  AnalyzePaletteImageText,
                                  image->filename))
        break;
  }
  DestroyCubeInfo(cube_info);
  cube_info=(CubeInfo *) NULL;
  return(MagickTrue);
}
