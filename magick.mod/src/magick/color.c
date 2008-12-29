/*
% Copyright (C) 2003 GraphicsMagick Group
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
#include "magick/monitor.h"
#include "magick/pixel_cache.h"
#include "magick/quantize.h"
#include "magick/semaphore.h"
#include "magick/utility.h"

/*
  Define declarations.
*/
#define ColorFilename  "colors.mgk"

#define ColorToNodeId(red,green,blue,index) ((unsigned int) \
            (((ScaleQuantumToChar(red) >> index) & 0x01U) << 2U | \
             ((ScaleQuantumToChar(green) >> index) & 0x01U) << 1U | \
             ((ScaleQuantumToChar(blue) >> index) & 0x01U)))

/*
  Declare color map.
*/
static char
  *ColorMap = (char *) 
    "<?xml version=\"1.0\"?>"
    "<colormap>"
    "  <color name=\"none\" red=\"0\" green=\"0\" blue=\"0\" opacity=\"255\" />"
    "  <color name=\"white\" red=\"255\" green=\"255\" blue=\"255\" />"
    "  <color name=\"red\" red=\"255\" green=\"0\" blue=\"0\" />"
    "  <color name=\"green\" red=\"0\" green=\"255\" blue=\"0\" />"
    "  <color name=\"gray\" red=\"190\" green=\"190\" blue=\"190\" />"
    "  <color name=\"blue\" red=\"0\" green=\"0\" blue=\"255\" />"
    "  <color name=\"cyan\" red=\"0\" green=\"255\" blue=\"255\" />"
    "  <color name=\"magenta\" red=\"255\" green=\"0\" blue=\"255\" />"
    "  <color name=\"yellow\" red=\"255\" green=\"255\" blue=\"0\" />"
    "  <color name=\"black\" red=\"0\" green=\"0\" blue=\"0\" />"
    "</colormap>";

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
static SemaphoreInfo
  *color_semaphore = (SemaphoreInfo *) NULL;

static ColorInfo
  *color_list = (ColorInfo *) NULL;

/*
  Forward declarations.
*/
static CubeInfo
  *ComputeCubeInfo(const Image *image,ExceptionInfo *exception),
  *GetCubeInfo(void);

static NodeInfo
  *GetNodeInfo(CubeInfo *,const unsigned int);

static unsigned int
  ReadColorConfigureFile(const char *,const unsigned long,ExceptionInfo *);

static void
  DestroyColorList(NodeInfo *node_info),
  HistogramToFile(const Image *,CubeInfo *,const NodeInfo *,FILE *,ExceptionInfo *);

/*
  Suppordinate function for use by VerifyColormapIndex macro.
*/
MagickExport unsigned int
MagickConstrainColormapIndex(Image *image, unsigned int index)
{
  if (index >= image->colors)
    {
      char
        colormapIndexBuffer[MaxTextExtent];
      
      FormatString(colormapIndexBuffer,"index %u >= %u colors, %.1024s",
        (unsigned int) index, image->colors, image->filename);
      errno=0;
      index=0U;
      ThrowException(&image->exception,CorruptImageError,
        InvalidColormapIndex,colormapIndexBuffer);
    }

  return index;
}


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
#define ComputeImageColorsText  "[%s] Compute image colors..."

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
+   D e s t r o y C o l o r I n f o                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyColorInfo deallocates memory associated with the color list.
%
%  The format of the DestroyColorInfo method is:
%
%      DestroyColorInfo(void)
%
%
*/
MagickExport void DestroyColorInfo(void)
{
  ColorInfo
    *color_info;

  register ColorInfo
    *p;

  AcquireSemaphoreInfo(&color_semaphore);
  for (p=color_list; p != (const ColorInfo *) NULL; )
  {
    color_info=p;
    p=p->next;
    MagickFreeMemory(color_info->path);
    MagickFreeMemory(color_info->name);
    MagickFreeMemory(color_info);
  }
  color_list=(ColorInfo *) NULL;
  LiberateSemaphoreInfo(&color_semaphore);
  DestroySemaphoreInfo(&color_semaphore);
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
+   G e t C o l o r I n f o                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetColorInfo searches the color list for the specified name and if
%  found returns attributes for that color.
%
%  The format of the GetColorInfo method is:
%
%      const PixelPacket *GetColorInfo(const char *name,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o color_info: Method GetColorInfo searches the color list for the
%      specified name and if found returns attributes for that color.
%
%    o name: The color name.
%
%    o compliance: Define the required color standard.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport const ColorInfo *GetColorInfo(const char *name,
  ExceptionInfo *exception)
{
  char
    colorname[MaxTextExtent];

  register char
    *q;

  register ColorInfo
    *p;

  if (color_list == (ColorInfo *) NULL)
    {
      AcquireSemaphoreInfo(&color_semaphore);
      if (color_list == (ColorInfo *) NULL)
        (void) ReadColorConfigureFile(ColorFilename,0,exception);
      LiberateSemaphoreInfo(&color_semaphore);
    }
  if ((name == (const char *) NULL) || (LocaleCompare(name,"*") == 0))
    return((const ColorInfo *) color_list);
  /*
    Search for named color.
  */
  (void) strlcpy(colorname,name,MaxTextExtent);
  for (q=colorname; *q != '\0'; q++)
  {
    if (*q != ' ')
      continue;
    (void) strcpy(q,q+1);
    q--;
  }
  AcquireSemaphoreInfo(&color_semaphore);
  for (p=color_list; p != (ColorInfo *) NULL; p=p->next)
    if (LocaleCompare(colorname,p->name) == 0)
      break;
  if (p == (ColorInfo *) NULL)
    ThrowException(exception,OptionWarning,UnrecognizedColor,name);
  else
    if (p != color_list)
      {
        /*
          Self-adjusting list.
        */
        if (p->previous != (ColorInfo *) NULL)
          p->previous->next=p->next;
        if (p->next != (ColorInfo *) NULL)
          p->next->previous=p->previous;
        p->previous=(ColorInfo *) NULL;
        p->next=color_list;
        color_list->previous=p;
        color_list=p;
      }
  LiberateSemaphoreInfo(&color_semaphore);
  return((const ColorInfo *) p);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t C o l o r I n f o A r r a y                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetColorInfoArray() returns a sorted null-terminated array of ColorInfo
%  pointers corresponding to the available color definitions. This function
%  should be used to access the entire list rather than GetColorInfo since
%  the list returned by GetColorInfo may be re-ordered every time it is
%  invoked. GetColorList may be used if only a list of color names is desired.
%  The array should be deallocated by the user once it is no longer needed.
%  Do not attempt to deallocate members of the array.
%
%  The format of the GetMagickList method is:
%
%      ColorInfo **GetColorInfoArray(ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
/*
  Compare two ColorInfo structures based on their name
*/
static int ColorInfoCompare(const void *x, const void *y)
{
  const ColorInfo
    *xx=*((const ColorInfo **) x),
    *yy=*((const ColorInfo **) y);

  return (strcmp(xx->name, yy->name));
}
MagickExport ColorInfo **GetColorInfoArray(ExceptionInfo *exception)
{
  ColorInfo
    **array;

  ColorInfo
    *p;

  ColorInfo
    *list;

  size_t
    entries=0;

  int
    i;

  /*
    Load color list
  */
  (void) GetColorInfo("*",exception);
  if ((!color_list) || (exception->severity > UndefinedException))
    return 0;

  AcquireSemaphoreInfo(&color_semaphore);

  list=color_list;

  /*
    Count number of list entries
  */
  for (p=list; p != 0; p=p->next)
    entries++;

  /*
    Allocate array memory
  */
  array=MagickAllocateMemory(ColorInfo **,sizeof(ColorInfo *)*(entries+1));
  if (!array)
    {
      ThrowException(exception,ResourceLimitError,MemoryAllocationFailed,0);
      return False;
    }
  (void) memset((void **)array,0,sizeof(ColorInfo *)*(entries+1));

  /*
    Add entries to array
  */
  i=0;
  for (p=list; p != 0; p=p->next)
    array[i++]=p;

  LiberateSemaphoreInfo(&color_semaphore);

  /*
    Sort array entries
  */
  qsort((void *) array, entries, sizeof(ColorInfo *), ColorInfoCompare);

  return (array);
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
%   G e t C o l o r L i s t                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetColorList returns any colors that match the specified pattern
%  and color standard.
%
%  The format of the GetColorList function is:
%
%      filelist=GetColorList(const char *pattern,unsigned long *number_colors)
%
%  A description of each parameter follows:
%
%    o filelist: Method GetColorList returns a list of colors that match the
%      specified pattern and color standard.
%
%    o pattern: Specifies a pointer to a text string containing a pattern.
%
%    o number_colors:  This integer returns the number of colors in the list.
%
%
*/
MagickExport char **GetColorList(const char *pattern,
  unsigned long *number_colors)
{
  char
    **colorlist;

  ExceptionInfo
    exception;

  register long
    i;

  register const ColorInfo
    *p;


  assert(pattern != (char *) NULL);
  assert(number_colors != (unsigned long *) NULL);
  *number_colors=0;
  GetExceptionInfo(&exception);
  p=GetColorInfo("*",&exception);
  DestroyExceptionInfo(&exception);
  if (p == (const ColorInfo *) NULL)
    return((char **) NULL);

  /*
    Determine color list size
  */
  AcquireSemaphoreInfo(&color_semaphore);
  i=0;
  for (p=color_list; p != (const ColorInfo *) NULL; p=p->next)
    i++;
  LiberateSemaphoreInfo(&color_semaphore);

  /*
    Allocate color list.
  */
  colorlist=MagickAllocateMemory(char **,i*sizeof(char *));
  if (colorlist == (char **) NULL)
    return((char **) NULL);

  /*
    Add colors matching glob specification to list
  */
  AcquireSemaphoreInfo(&color_semaphore);
  i=0;
  for (p=color_list; p != (const ColorInfo *) NULL; p=p->next)
  {
    if (p->stealth)
      continue;
    if (GlobExpression(p->name,pattern))
      colorlist[i++]=AllocateString(p->name);
  }
  LiberateSemaphoreInfo(&color_semaphore);

  *number_colors=i;
  return(colorlist);
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
#define HistogramToFileImageText  "[%s] Compute image histogram..."

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
%     I s G r a y I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IsGrayImage() returns True if all the pixels in the image have the same
%  red, green, and blue intensities.
%
%  The format of the IsGrayImage method is:
%
%      unsigned int IsGrayImage(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status: Method IsGrayImage returns True if the image is grayscale
%      otherwise False is returned.
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
#define AnalyzeGrayImageText "[%s] Analyze image for gray..."
MagickExport MagickBool IsGrayImage(const Image *image,
  ExceptionInfo *exception)
{
  unsigned long
    y;

  register const PixelPacket
    *p;

  register unsigned long
    x;

  MagickBool
    is_grayscale;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  if (image->colorspace == CMYKColorspace)
    return(MagickFalse);
  if (image->is_grayscale)
    return(MagickTrue);
  is_grayscale=MagickTrue;
  switch (image->storage_class)
  {
    case DirectClass:
    case UndefinedClass:
    {
      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                            "IsGrayImage(): Exhaustive pixel test!");
      for (y=0; y < image->rows; y++)
      {
        p=AcquireImagePixels(image,0,y,image->columns,1,exception);
        if (p == (const PixelPacket *) NULL)
          return(MagickFalse);
        for (x=image->columns; x != 0; x--)
        {
          if ((p->red != p->green) || (p->green != p->blue))
            {
              is_grayscale=MagickFalse;
              break;
            }
          p++;
        }
        if (!is_grayscale)
          break;
        if (QuantumTick(y,image->rows))
          if (!MagickMonitorFormatted(y,image->rows,
                                      exception,AnalyzeGrayImageText,
                                      image->filename))
            break;
      }
      break;
    }
    case PseudoClass:
    {
      p=image->colormap;
      for (x=image->colors; x != 0; x--)
        {
          if ((p->red != p->green) || (p->green != p->blue))
            {
              is_grayscale=MagickFalse;
              break;
            }
          p++;
        }
      break;
    }
  }

  /*
    Force progress indication to 100%
  */
  if (!is_grayscale)
    (void) MagickMonitorFormatted(image->rows-1,image->rows,exception,
                                  AnalyzeGrayImageText,image->filename);

  ((Image *)image)->is_grayscale=is_grayscale;
  return(is_grayscale);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%   I s M o n o c h r o m e I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IsMonochromeImage() returns True if all the pixels in the image have
%  the same red, green, and blue intensities and the intensity is either
%  0 or MaxRGB.
%
%  The format of the IsMonochromeImage method is:
%
%      unsigned int IsMonochromeImage(const Image *image,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
#define AnalyzeBilevelImageText "[%s] Analyze image for bilevel..."
MagickExport MagickBool IsMonochromeImage(const Image *image,
  ExceptionInfo *exception)
{
  unsigned long
    y;

  register const PixelPacket
    *p;

  register unsigned long
    x;

  MagickBool
    is_monochrome;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  if (image->colorspace == CMYKColorspace)
    return(False);
  if (image->is_monochrome)
    return(True); 
  is_monochrome=MagickTrue;
  switch (image->storage_class)
  {
    case DirectClass:
    case UndefinedClass:
    {
      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                            "IsMonochromeImage(): Exhaustive pixel test!");
      for (y=0; y < image->rows; y++)
      {
        p=AcquireImagePixels(image,0,y,image->columns,1,exception);
        if (p == (const PixelPacket *) NULL)
          return(MagickFalse);
        for (x=image->columns; x != 0; x--)
        {
          if ((p->red != p->green) || (p->green != p->blue) ||
              ((p->red != 0) && (p->red != MaxRGB)))
            {
              is_monochrome=MagickFalse;
              break;
            }
          p++;
        }
        if (!is_monochrome)
          break;
        if (QuantumTick(y,image->rows))
          if (!MagickMonitorFormatted(y,image->rows,exception,
                                      AnalyzeBilevelImageText,image->filename))
            break;
      }
      break;
    }
    case PseudoClass:
    {
      p=image->colormap;
      for (x=image->colors; x != 0; x--)
      {
        if ((p->red != p->green) || (p->green != p->blue) ||
            ((p->red != 0) && (p->red != MaxRGB)))
          {
            is_monochrome=MagickFalse;
            break;
          }
        p++;
      }
      break;
    }
  }

  /*
    Force progress indication to 100%
  */
  if (!is_monochrome)
    (void) MagickMonitorFormatted(image->rows-1,image->rows,exception,
                                  AnalyzeBilevelImageText,image->filename);

  ((Image *)image)->is_monochrome=is_monochrome;
  return(is_monochrome);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     I s O p a q u e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IsOpaqueImage() returns True if none of the pixels in the image have an
%  opacity value other than opaque (0).
%
%  The format of the IsOpaqueImage method is:
%
%      unsigned int IsOpaqueImage(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status: Method IsOpaqueImage returns False if the image has one or more
%      pixels that are transparent otherwise True is returned.
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
#define AnalyzeOpaqueImageText "[%s] Analyze image for opacity..."
MagickExport MagickBool IsOpaqueImage(const Image *image,
  ExceptionInfo *exception)
{
  unsigned long
    y;

  register const PixelPacket
    *p;

  register unsigned long
    x;

  MagickBool
    is_opaque;

  /*
    Determine if image is opaque.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (!image->matte)
    return(MagickTrue);
  is_opaque=MagickTrue;
  (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                        "IsOpaqueImage(): Exhaustive pixel test!");
  for (y=0; y < image->rows; y++)
    {
      p=AcquireImagePixels(image,0,y,image->columns,1,exception);
      if (p == (const PixelPacket *) NULL)
        return(MagickFalse);
      for (x=image->columns; x > 0; x--)
        {
          if (p->opacity != OpaqueOpacity)
            {
              is_opaque=MagickFalse;
              break;
            }
          p++;
        }
      if (!is_opaque)
        break;
      if (QuantumTick(y,image->rows))
        if (!MagickMonitorFormatted(y,image->rows,exception,
                                    AnalyzeOpaqueImageText,image->filename))
          break;
    }

  /*
    Force progress indication to 100%
  */
  if (!is_opaque)
    (void) MagickMonitorFormatted(image->rows-1,image->rows,exception,
                                  AnalyzeOpaqueImageText,image->filename);

  return(is_opaque);
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
#define AnalyzePaletteImageText "[%s] Analyze image for palette..."
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

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  L i s t C o l o r I n f o                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ListColorInfo() lists color names to the specified file.  Color names
%  are a convenience.  Rather than defining a color by its red, green, and
%  blue intensities just use a color name such as white, blue, or yellow.
%
%  The format of the ListColorInfo method is:
%
%      unsigned int ListColorInfo(FILE *file,ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o file:  List color names to this file handle.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int ListColorInfo(FILE *file,ExceptionInfo *exception)
{
  register long
    i;

  register const ColorInfo
    *p;

  if (file == (const FILE *) NULL)
    file=stdout;
  (void) GetColorInfo("*",exception);
  AcquireSemaphoreInfo(&color_semaphore);
  for (p=color_list; p != (const ColorInfo *) NULL; p=p->next)
  {
    if ((p->previous == (ColorInfo *) NULL) ||
        (LocaleCompare(p->path,p->previous->path) != 0))
      {
        if (p->previous != (ColorInfo *) NULL)
          (void) fprintf(file,"\n");
        if (p->path != (char *) NULL)
          (void) fprintf(file,"Path: %.1024s\n\n",p->path);
        (void) fprintf(file,
          "Name                   Color                   Compliance\n");
        (void) fprintf(file,"-------------------------------------------------"
          "------------------------------\n");
      }
    if (p->stealth)
      continue;
    (void) fprintf(file,"%.1024s",p->name);
    for (i=(long) strlen(p->name); i <= 22; i++)
      (void) fprintf(file," ");
    if (p->color.opacity == OpaqueOpacity)
      (void) fprintf(file,"%5d,%5d,%5d       ",p->color.red,p->color.green,
        p->color.blue);
    else
      (void) fprintf(file,"%5d,%5d,%5d,%5d ",p->color.red,p->color.green,
        p->color.blue,p->color.opacity);
    if ((unsigned int) p->compliance & (unsigned int) SVGCompliance)
      (void) fprintf(file,"SVG ");
    if ((unsigned int) p->compliance & (unsigned int) X11Compliance)
      (void) fprintf(file,"X11 ");
    if ((unsigned int) p->compliance & (unsigned int) XPMCompliance)
      (void) fprintf(file,"XPM ");
    (void) fprintf(file,"\n");
  }
  (void) fflush(file);
  LiberateSemaphoreInfo(&color_semaphore);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   Q u e r y C o l o r D a t a b a s e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  QueryColorDatabase() returns the red, green, blue, and opacity intensities
%  for a given color name.
%
%  The format of the QueryColorDatabase method is:
%
%      unsigned int QueryColorDatabase(const char *name,PixelPacket *color,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o name: The color name (e.g. white, blue, yellow).
%
%    o color: The red, green, blue, and opacity intensities values of the
%      named color in this structure.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int QueryColorDatabase(const char *name,
  PixelPacket *color,ExceptionInfo *exception)
{
  double
    scale;

  DoublePixelPacket
    pixel;

  register const ColorInfo
    *p;

  register int
    i;

  /*
    Initialize color return value.
  */
  assert(color != (PixelPacket *) NULL);
  (void) memset(color,0,sizeof(PixelPacket));
  color->opacity=TransparentOpacity;
  if ((name == (char *) NULL) || (*name == '\0'))
    name=BackgroundColor;
  while (isspace((int) (*name)))
    name++;
  if (*name == '#')
    {
      char
        c;

      int
        n;

      LongPixelPacket
        pixel;

      (void) memset(&pixel,0,sizeof(LongPixelPacket));
      name++;
      for (n=0; isxdigit((int) name[n]); n++);
      if ((n == 3) || (n == 6) || (n == 9) || (n == 12) || (n == 24))
        {
          /*
            Parse RGB specification.
          */
          n/=3;
          do
          {
            pixel.red=pixel.green;
            pixel.green=pixel.blue;
            pixel.blue=0;
            for (i=n-1; i >= 0; i--)
            {
              c=(*name++);
              pixel.blue<<=4;
              if ((c >= '0') && (c <= '9'))
                pixel.blue|=c-'0';
              else
                if ((c >= 'A') && (c <= 'F'))
                  pixel.blue|=c-('A'-10);
                else
                  if ((c >= 'a') && (c <= 'f'))
                    pixel.blue|=c-('a'-10);
                  else
                    {
                      ThrowException(exception,OptionWarning,UnrecognizedColor,name);
                      return(False);
                    }
            }
          } while (isxdigit((int) *name));
        }
      else
        if ((n != 4) && (n != 8) && (n != 16) && (n != 32))
          {
            ThrowException(exception,OptionWarning,UnrecognizedColor,name);
            return(False);
          }
        else
          {
            /*
              Parse RGBA specification.
            */
            n/=4;
            do
            {
              pixel.red=pixel.green;
              pixel.green=pixel.blue;
              pixel.blue=pixel.opacity;
              pixel.opacity=0;
              for (i=n-1; i >= 0; i--)
              {
                c=(*name++);
                pixel.opacity<<=4;
                if ((c >= '0') && (c <= '9'))
                  pixel.opacity|=c-'0';
                else
                  if ((c >= 'A') && (c <= 'F'))
                    pixel.opacity|=c-('A'-10);
                  else
                    if ((c >= 'a') && (c <= 'f'))
                      pixel.opacity|=c-('a'-10);
                    else
                      {
                        ThrowException(exception,OptionWarning,UnrecognizedColor,name);
                        return(False);
                      }
              }
            } while (isxdigit((int) *name));
          }
      {
        unsigned int
          divisor=1;
        
        n<<=2;
        for( i=n-1; i; i--)
          {
            divisor <<= 1;
            divisor |=1;
          }
        color->red=(Quantum)
          (((double) MaxRGB*pixel.red)/divisor+0.5);
        color->green=(Quantum)
          (((double) MaxRGB*pixel.green)/divisor+0.5);
        color->blue=(Quantum)
          (((double) MaxRGB*pixel.blue)/divisor+0.5);
        color->opacity=OpaqueOpacity;
        if ((n != 3) && (n != 6) && (n != 9) && (n != 12) && (n != 24))
          color->opacity=(Quantum)
            (((double) MaxRGB*pixel.opacity)/divisor+0.5);
      }
      return(True);
    }
  if (LocaleNCompare(name,"rgb(",4) == 0)
    {
      scale=strchr(name,'%') == (char *) NULL ? 1.0 :
        ScaleQuantumToChar(MaxRGB)/100.0;
      (void) sscanf(name,"%*[^(](%lf%*[%,]%lf%*[%,]%lf",
        &pixel.red,&pixel.green,&pixel.blue);
      color->red=ScaleCharToQuantum(scale*pixel.red);
      color->green=ScaleCharToQuantum(scale*pixel.green);
      color->blue=ScaleCharToQuantum(scale*pixel.blue);
      color->opacity=OpaqueOpacity;
      return(True);
    }
  if (LocaleNCompare(name,"rgba(",5) == 0)
    {
      scale=strchr(name,'%') == (char *) NULL ? 1.0 :
        ScaleQuantumToChar(MaxRGB)/100.0;
      (void) sscanf(name,"%*[^(](%lf%*[%,]%lf%*[%,]%lf%*[%,]%lf",
        &pixel.red,&pixel.green,&pixel.blue,&pixel.opacity);
      color->red=ScaleCharToQuantum(scale*pixel.red);
      color->green=ScaleCharToQuantum(scale*pixel.green);
      color->blue=ScaleCharToQuantum(scale*pixel.blue);
      color->opacity=ScaleCharToQuantum(scale*pixel.opacity);
      return(True);
    }
  p=GetColorInfo(name,exception);
  if (p == (const ColorInfo *) NULL)
    return(False);
  if ((LocaleCompare(p->name,"opaque") == 0) ||
      (LocaleCompare(p->name,"transparent") == 0))
    {
      color->opacity=p->color.opacity;
      return(True);
    }
  *color=p->color;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  Q u e r y C o l o r n a m e                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  QueryColorname() returns a named color for the given color intensity.  If
%  an exact match is not found, a hex value is return instead.  For example
%  an intensity of rgb:(0,0,0) returns black whereas rgb:(223,223,223)
%  returns #dfdfdf.
%
%  The format of the QueryColorname method is:
%
%      unsigned int QueryColorname(const Image *image,const PixelPacket *color,
%        const ComplianceType compliance,char *name,ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o image: The image.
%
%    o color: The color intensities.
%
%    o Compliance: Adhere to this color standard: NoCompliance, SVGCompliance,
%                    X11Compliance, XPMCompliance, AllCompliance.
%
%    o name: Return the color name or hex value.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int QueryColorname(const Image *image,
  const PixelPacket *color,const ComplianceType compliance,char *name,
  ExceptionInfo *exception)
{
  register const ColorInfo
    *p;

  *name='\0';
  p=GetColorInfo("*",exception);
  if (p != (const ColorInfo *) NULL)
    {
      for (p=color_list; p != (const ColorInfo *) NULL; p=p->next)
      {
        if (!((unsigned int) p->compliance & (unsigned int) compliance))
          continue;
        if ((p->color.red != color->red) || (p->color.green != color->green) ||
            (p->color.blue != color->blue) ||
            (p->color.opacity != color->opacity))
          continue;
        (void) strlcpy(name,p->name,MaxTextExtent);
        return(True);
      }
    }
  GetColorTuple(color,image->depth,image->matte,True,name);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   R e a d C o n f i g u r e F i l e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadColorConfigureFile reads the color configuration file which maps
%  color strings with a particular image format.
%
%  The format of the ReadColorConfigureFile method is:
%
%      unsigned int ReadColorConfigureFile(const char *basename,
%        const unsigned long depth,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status: Method ReadColorConfigureFile returns True if at least one color
%      is defined otherwise False.
%
%    o basename:  The color configuration filename.
%
%    o depth: depth of <include /> statements.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static unsigned int ReadColorConfigureFile(const char *basename,
  const unsigned long depth,ExceptionInfo *exception)
{
  char
    keyword[MaxTextExtent],
    path[MaxTextExtent],
    *q,
    *token,
    *xml;

  size_t
    length;

  /*
    Read the color configure file.
  */
  (void) strcpy(path,basename);
  if (depth == 0)
    xml=(char *) GetConfigureBlob(basename,path,&length,exception);
  else
    xml=(char *) FileToBlob(basename,&length,exception);
  if (xml == (char *) NULL)
    xml=AllocateString(ColorMap);
  token=AllocateString(xml);
  for (q=xml; *q != '\0'; )
  {
    /*
      Interpret XML.
    */
    GetToken(q,&q,token);
    if (*token == '\0')
      break;
    (void) strlcpy(keyword,token,MaxTextExtent);
    if (LocaleNCompare(keyword,"<!--",4) == 0)
      {
        /*
          Comment element.
        */
        while ((LocaleNCompare(q,"->",2) != 0) && (*q != '\0'))
          GetToken(q,&q,token);
        continue;
      }
    if (LocaleCompare(keyword,"<include") == 0)
      {
        /*
          Include element.
        */
        while ((*token != '>') && (*q != '\0'))
        {
          (void) strlcpy(keyword,token,MaxTextExtent);
          GetToken(q,&q,token);
          if (*token != '=')
            continue;
          GetToken(q,&q,token);
          if (LocaleCompare(keyword,"file") == 0)
            {
              if (depth > 200)
                ThrowException(exception,ConfigureError,IncludeElementNestedTooDeeply,path);
              else
                {
                  char
                    filename[MaxTextExtent];

                  GetPathComponent(path,HeadPath,filename);
                  if (*filename != '\0')
                    (void) strlcat(filename,DirectorySeparator,MaxTextExtent);
                  (void) strlcat(filename,token,MaxTextExtent);
                  (void) ReadColorConfigureFile(filename,depth+1,exception);
                }
              if (color_list != (ColorInfo *) NULL)
                while (color_list->next != (ColorInfo *) NULL)
                  color_list=color_list->next;
            }
        }
        continue;
      }
    if (LocaleCompare(keyword,"<color") == 0)
      {
        ColorInfo
          *color_info;

        /*
          Allocate memory for the color list.
        */
        color_info=MagickAllocateMemory(ColorInfo *,sizeof(ColorInfo));
        if (color_info == (ColorInfo *) NULL)
          MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
            UnableToAllocateColorInfo);
        (void) memset(color_info,0,sizeof(ColorInfo));
        color_info->path=AcquireString(path);
        color_info->signature=MagickSignature;
        if (color_list == (ColorInfo *) NULL)
          {
            color_list=color_info;
            continue;
          }
        color_list->next=color_info;
        color_info->previous=color_list;
        color_list=color_list->next;
        continue;
      }
    if (color_list == (ColorInfo *) NULL)
      continue;
    GetToken(q,(char **) NULL,token);
    if (*token != '=')
      continue;
    GetToken(q,&q,token);
    GetToken(q,&q,token);
    switch (*keyword)
    {
      case 'B':
      case 'b':
      {
        if (LocaleCompare((char *) keyword,"blue") == 0)
          {
            color_list->color.blue=ScaleCharToQuantum(atol(token));
            break;
          }
        break;
      }
      case 'C':
      case 'c':
      {
        if (LocaleCompare((char *) keyword,"compliance") == 0)
          {
            long
              compliance;

            compliance=color_list->compliance;
            if (GlobExpression(token,"*SVG*"))
              compliance|=SVGCompliance;
            if (GlobExpression(token,"*X11*"))
              compliance|=X11Compliance;
            if (GlobExpression(token,"*XPM*"))
              compliance|=XPMCompliance;
            color_list->compliance=(ComplianceType) compliance;
            break;
          }
        break;
      }
      case 'G':
      case 'g':
      {
        if (LocaleCompare((char *) keyword,"green") == 0)
          {
            color_list->color.green=ScaleCharToQuantum(atol(token));
            break;
          }
        break;
      }
      case 'N':
      case 'n':
      {
        if (LocaleCompare((char *) keyword,"name") == 0)
          {
            color_list->name=AcquireString(token);
            break;
          }
        break;
      }
      case 'O':
      case 'o':
      {
        if (LocaleCompare((char *) keyword,"opacity") == 0)
          {
            color_list->color.opacity=ScaleCharToQuantum(atol(token));
            break;
          }
        break;
      }
      case 'R':
      case 'r':
      {
        if (LocaleCompare((char *) keyword,"red") == 0)
          {
            color_list->color.red=ScaleCharToQuantum(atol(token));
            break;
          }
        break;
      }
      case 'S':
      case 's':
      {
        if (LocaleCompare((char *) keyword,"stealth") == 0)
          {
            color_list->stealth=LocaleCompare(token,"True") == 0;
            break;
          }
        break;
      }
      default:
        break;
    }
  }
  MagickFreeMemory(token);
  MagickFreeMemory(xml);
  if (color_list == (ColorInfo *) NULL)
    return(False);
  while (color_list->previous != (ColorInfo *) NULL)
    color_list=color_list->previous;
  return(True);
}
