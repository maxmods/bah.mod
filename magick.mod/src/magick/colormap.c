/*
% Copyright (C) 2003 - 2009 GraphicsMagick Group
% Copyright (C) 2003 ImageMagick Studio
% Copyright 1991-1999 E. I. du Pont de Nemours and Company
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
% GraphicsMagick Colormap Methods
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/analyze.h"
#include "magick/color.h"
#include "magick/colormap.h"
#include "magick/pixel_iterator.h"
#include "magick/utility.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A l l o c a t e I m a g e C o l o r m a p                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AllocateImageColormap() allocates an image colormap and initializes
%  it to a linear gray colorspace with increasing intensity.  If the image
%  already has a colormap, it is replaced.  AllocateImageColormap() returns
%  True if successful, otherwise False if there is not enough memory.
%
%  The format of the AllocateImageColormap method is:
%
%      unsigned int AllocateImageColormap(Image *image,
%        const unsigned long colors)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o colors: The number of colors in the image colormap.
%
%
*/
MagickExport MagickPassFail AllocateImageColormap(Image *image,
  const unsigned long colors)
{
  register long
    i;

  size_t
    length;

  Quantum
    quantum;

  /*
    Allocate image colormap.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  image->storage_class=PseudoClass;
  image->colors=colors;
  length=image->colors*sizeof(PixelPacket);
  if (image->colormap == (PixelPacket *) NULL)
    image->colormap=MagickAllocateMemory(PixelPacket *,length);
  else
    MagickReallocMemory(PixelPacket *,image->colormap,length);
  if (image->colormap == (PixelPacket *) NULL)
    {
      image->colors=0;
      image->storage_class=DirectClass;
      return(MagickFail);
    }
  for (i=0; i < (long) image->colors; i++)
  {
    quantum=(Quantum) (i*(MaxRGB/Max(colors-1,1)));
    image->colormap[i].red=quantum;
    image->colormap[i].green=quantum;
    image->colormap[i].blue=quantum;
    image->colormap[i].opacity=OpaqueOpacity;
  }
  return(MagickPass);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     C y c l e C o l o r m a p I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CycleColormap() displaces an image's colormap by a given number of
%  positions.  If you cycle the colormap a number of times you can produce
%  a psychodelic effect.
%
%  The format of the CycleColormapImage method is:
%
%      CycleColormapImage(Image *image,const int amount)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o amount:  Offset the colormap this much.
%
%
*/
#define CycleColormapImageText "[%s] Cycle colormap..."
static MagickPassFail
CycleColormapCallBack(void *mutable_data,         /* User provided mutable data */
                      const void *immutable_data, /* User provided immutable data */
                      Image *image,               /* Modify image */
                      PixelPacket *pixels,        /* Pixel row */
                      IndexPacket *indexes,       /* Pixel row indexes */
                      const long npixels,         /* Number of pixels in row */
                      ExceptionInfo *exception)   /* Exception report */
{
  /*
    Cycle colormap by amount.
  */
  const int
    amount = *((const int *) immutable_data);


  long
    colormap_index;

  register const PixelPacket
    *colormap = image->colormap;

  const unsigned long
    colors = image->colors;

  register long
    i;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  for (i=0; i < npixels; i++)
    {
      colormap_index=(long) ((indexes[i]+amount) % colors);
      if (colormap_index < 0L)
        colormap_index+=colors;
      indexes[i]=(IndexPacket) colormap_index;
      pixels[i].red=colormap[colormap_index].red;
      pixels[i].green=colormap[colormap_index].green;
      pixels[i].blue=colormap[colormap_index].blue;
    }

  return MagickPass;
}
MagickExport MagickPassFail  CycleColormapImage(Image *image,const int amount)
{
  unsigned int
    is_grayscale,
    is_monochrome,
    status = MagickPass;

  PixelIteratorOptions
    options;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  is_grayscale=image->is_grayscale;
  is_monochrome=image->is_monochrome;
  if (image->storage_class == DirectClass)
    (void) SetImageType(image,PaletteType);

  InitializePixelIteratorOptions(&options,&image->exception);
#if defined(HAVE_OPENMP) && defined(DisableSlowOpenMP)
  options.max_threads=1;
#endif
  status=PixelIterateMonoModify(CycleColormapCallBack,&options,
                                CycleColormapImageText,
                                NULL,&amount,0,0,image->columns,image->rows,
                                image,&image->exception);
  image->is_grayscale=is_grayscale;
  image->is_monochrome=is_monochrome;
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k C o n s t r a i n C o l o r m a p I n d e x                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickConstrainColormapIndex() is a subordinate function for use by
%  the VerifyColormapIndex macro.  If the colormap index is outside the
%  bounds of the image colormap then zero is returned and an
%  InvalidColormapIndex exception is thrown, otherwise the colormap
%  index is returned.
%
%  The format of the MagickConstrainColormapIndex method is:
%
%      unsigned int MagickConstrainColormapIndex(Image *image,
%                                                unsigned int index)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o index: The colormap index
%
%
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
%   R e p l a c e I m a g e C o l o r m a p                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReplaceImageColormap() replaces an existing image colormap with a new
%  image colormap. The new image colormap is expected to contain all of the
%  colors from the existing colormap. The existing colormap indexes are
%  adjusted to conform with positions in the new colormap. If the new
%  colormap contains duplicate entries, then the associated colormap index
%  will point to the first entry found in the colormap and other matching
%  entries will not be used. MagickPass is returned if the operation is
%  successful, otherwise MagickFail is returned, and image->exception is
%  updated with the cause of the failure.
%
%  This function is useful in case colormap entries need to match across
%  multiple images or otherwise occupy specific locations.
%
%  The format of the ReplaceImageColormap method is:
%
%      MagickPassFail ReplaceImageColormap(Image *image,
%                                          const PixelPacket *colormap,
%                                          const unsigned int colors)
%
%  A description of each parameter follows:
%
%    o image: image in which to replace colormap.
%
%    o colormap: new colormap.
%
%    o colors: number of colors in new colormap.
%
*/
#define ReplaceImageColormapText "[%s] Replacing image colormap..."
static MagickPassFail
ReplaceImageColormapCallBack(void *mutable_data,         /* User provided mutable data */
                             const void *immutable_data, /* User provided immutable data */
                             Image *image,               /* Modify image */
                             PixelPacket *pixels,        /* Pixel row */
                             IndexPacket *indexes,       /* Pixel row indexes */
                             const long npixels,         /* Number of pixels in row */
                             ExceptionInfo *exception)   /* Exception report */
{
  /*
    Replace image colormap
  */

  const unsigned int
    *colormap_index=(const unsigned int *) immutable_data;

  register long
    i;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(pixels);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  for (i=0; i < npixels; i++)
    indexes[i]=colormap_index[indexes[i]];

  return MagickPass;
}
MagickExport MagickPassFail
ReplaceImageColormap(Image *image,
                     const PixelPacket *colormap,
                     const unsigned int colors)
{
  unsigned int
    *colormap_index=(unsigned int *) NULL;

  register unsigned int
    i,
    j;

  MagickPassFail
    status=MagickPass;

  assert(image != (Image *) NULL);
  assert(colormap != (const PixelPacket *) NULL);
  assert(colors != 0);
  assert(image->storage_class == PseudoClass);

  /*
    Allocate memory for colormap index
  */
  colormap_index=MagickAllocateMemory(unsigned int *,
                                      MaxColormapSize*sizeof(unsigned int));
  if (colormap_index == (unsigned int *) NULL)
    {
      ThrowException3(&image->exception,ResourceLimitError,
                      MemoryAllocationFailed,UnableToAllocateColormap);
      return MagickFail;
    }

  /*
    Build a map between the new colormap and the old colormap.
  */
  (void) memset(colormap_index,0,MaxColormapSize*sizeof(unsigned int));
  for (i=0; i < image->colors ; i++)
    {
      for (j=0; j < colors; j++)
        {
          if (ColorMatch(&colormap[j],&image->colormap[i]))
            {
              colormap_index[i]=j;
              break;
            }
        }
    }

  if (status == MagickPass)
    {
      /*
        Reassign image colormap indexes
      */
      status=PixelIterateMonoModify(ReplaceImageColormapCallBack,NULL,
                                    ReplaceImageColormapText,
                                    NULL,colormap_index,0,0,
                                    image->columns,image->rows,
                                    image,&image->exception);
      /*
        Replace existing colormap.
      */
      if (status == MagickPass)
        {
          MagickReallocMemory(PixelPacket *,image->colormap,sizeof(PixelPacket)*colors);
          if (image->colormap == (PixelPacket *) NULL)
            {
              ThrowException3(&image->exception,ResourceLimitError,
                              MemoryAllocationFailed,UnableToAllocateColormap);
              status=MagickFail;
            }
        }
      if (status == MagickPass)
        (void) memcpy(image->colormap,colormap,sizeof(PixelPacket)*colors);
    }

  MagickFreeMemory(colormap_index);

  image->is_grayscale=IsGrayImage(image,&image->exception);
  image->is_monochrome=IsMonochromeImage(image,&image->exception);
  
  return status;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   S o r t C o l o r m a p B y I n t e n t s i t y                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SortColormapByIntensity() sorts the colormap of a PseudoClass image by
%  decreasing color intensity.
%
%  The format of the SortColormapByIntensity method is:
%
%      unsigned int SortColormapByIntensity(Image *image)
%
%  A description of each parameter follows:
%
%    o image: A pointer to an Image structure.
%
%
*/

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

static int InverseIntensityCompare(const void *x,const void *y)
{
  long
    intensity;

  PixelPacket
    *color_1,
    *color_2;

  color_1=(PixelPacket *) x;
  color_2=(PixelPacket *) y;
  /*
    y - x results in decreasing order
  */
  intensity=PixelIntensityToQuantum(color_2)-
    (long) PixelIntensityToQuantum(color_1);
  return(intensity);
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#define SortColormapByIntensityText "[%s] Sorting colormap by intensity...  "
static MagickPassFail
SortColormapByIntensityCallBack(void *mutable_data,         /* User provided mutable data */
                                const void *immutable_data, /* User provided immutable data */
                                Image *image,               /* Modify image */
                                PixelPacket *pixels,        /* Pixel row */
                                IndexPacket *indexes,       /* Pixel row indexes */
                                const long npixels,         /* Number of pixels in row */
                                ExceptionInfo *exception)   /* Exception report */
{
  /*
    Update image colormap indexes to reflect new ordering indicated by
    new_indexes array.
  */
  const unsigned short
    *new_indexes = (const unsigned short *) immutable_data;

  register long
    i;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(pixels);
  ARG_NOT_USED(exception);
  for (i=0; i < npixels; i++)
    indexes[i]=new_indexes[indexes[i]];

  return MagickPass;
}
MagickExport MagickPassFail SortColormapByIntensity(Image *image)
{
  register long
    i;

  unsigned int
    is_grayscale,
    is_monochrome;

  unsigned short
    *new_indexes;

  MagickPassFail
    status=MagickPass;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->storage_class != PseudoClass)
    return(MagickFail);
  is_grayscale=image->is_grayscale;
  is_monochrome=image->is_monochrome;
  /*
    Allocate memory for pixel indexes.
  */
  new_indexes=MagickAllocateMemory(unsigned short *,image->colors*sizeof(unsigned short));
  if (new_indexes == (unsigned short *) NULL)
    ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
                          UnableToSortImageColormap);
  /*
    Assign index values to colormap entries.
  */
  for (i=0; i < (long) image->colors; i++)
    image->colormap[i].opacity=(unsigned short) i;
  /*
    Sort image colormap by decreasing intensity.
  */
  qsort((void *) image->colormap,image->colors,sizeof(PixelPacket),
        InverseIntensityCompare);
  /*
    Update image colormap indexes to new order.
  */
  for (i=0; i < (long) image->colors; i++)
    new_indexes[image->colormap[i].opacity]=(unsigned short) i;
  status=PixelIterateMonoModify(SortColormapByIntensityCallBack,NULL,
                                SortColormapByIntensityText,
                                NULL,new_indexes,0,0,image->columns,image->rows,
                                image,&image->exception);
  MagickFreeMemory(new_indexes);
  image->is_grayscale=is_grayscale;
  image->is_monochrome=is_monochrome;
  return(status);
}

