/*
% Copyright (C) 2003 - 2008 GraphicsMagick Group
% Copyright (C) 2003 ImageMagick Studio
% Copyright 1991-1999 E. I. du Pont de Nemours and Company
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
% GraphicsMagick Image Averaging Methods.
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/monitor.h"
#include "magick/omp_data_view.h"
#include "magick/pixel_cache.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     A v e r a g e I m a g e s                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  The Average() method takes a set of images and averages them together.
%  Each image in the set must have the same width and height.  Average()
%  returns a single image with each corresponding pixel component of
%  each image averaged.   On failure, a NULL image is returned and
%  exception describes the reason for the failure.
%
%  The format of the AverageImage method is:
%
%      Image *AverageImages(Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image sequence.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *AverageImages(const Image *image,ExceptionInfo *exception)
{
  ThreadViewDataSet
    *pixels_sums;

  Image
    *average_image;

  const Image
    *last_image;

  long
    y;

  unsigned long
    row_count=0;

  double
    number_scenes;
    
  unsigned long
    number_pixels;

  MagickPassFail
    status=MagickPass;

  /*
    Ensure the image are the same size.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if (image->next == (Image *) NULL)
    ThrowImageException3(ImageError,ImageSequenceIsRequired,
                         UnableToAverageImage);
  {
    const Image
      *next;
      
    for (next=image; next != (Image *) NULL; next=next->next)
      {
        if ((next->columns != image->columns) || (next->rows != image->rows))
          ThrowImageException3(OptionError,UnableToAverageImageSequence,
                               ImageWidthsOrHeightsDiffer);
      }
  }
  /*
    Allocate sum accumulation buffer.
  */
  number_pixels=image->columns;
  pixels_sums=AllocateThreadViewDataArray(image,exception,number_pixels,
                                          sizeof(DoublePixelPacket));
  if (pixels_sums == (ThreadViewDataSet *) NULL)
    ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
                         UnableToAverageImageSequence);
  /*
    Initialize average next attributes.
  */
  average_image=CloneImage(image,image->columns,image->rows,True,exception);
  if (average_image == (Image *) NULL)
    {
      DestroyThreadViewDataSet(pixels_sums);
      return((Image *) NULL);
    }
  average_image->storage_class=DirectClass;

  number_scenes=(double) GetImageListLength(image);
  last_image=GetLastImageInList(image);
#if defined(HAVE_OPENMP)
#  pragma omp parallel for schedule(static) shared(row_count, status)
#endif
  for (y=0; y < (long) image->rows; y++)
    {
      register DoublePixelPacket
        *pixels_sum;

      const Image
        *next;

      register const PixelPacket
        *p;

      register long
        x;

      MagickBool
        thread_status;

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_AverageImages)
#endif
      thread_status=status;
      if (thread_status == MagickFail)
        continue;

      pixels_sum=AccessThreadViewData(pixels_sums);

      /*
        Compute sum over each pixel color component.
      */
      for (next=image; next != (Image *) NULL; next=next->next)
        {
          ViewInfo
            *next_view;

          next_view=OpenCacheView((Image *) next);
          if (next_view == (ViewInfo *) NULL)
            thread_status=MagickFail;
          if (next_view != (ViewInfo *) NULL)
            {
              p=AcquireCacheViewPixels(next_view,0,y,next->columns,1,exception);
              if (p == (const PixelPacket *) NULL)
                thread_status=MagickFail;
              if (p != (const PixelPacket *) NULL)
                {
                  if (next == image)
                    {
                      for (x=0; x < (long) next->columns; x++)
                        {
                          pixels_sum[x].red=p[x].red;
                          pixels_sum[x].green=p[x].green;
                          pixels_sum[x].blue=p[x].blue;
                          pixels_sum[x].opacity=p[x].opacity;
                        }
                    }
                  else
                    {
                      for (x=0; x < (long) next->columns; x++)
                        {
                          pixels_sum[x].red+=p[x].red;
                          pixels_sum[x].green+=p[x].green;
                          pixels_sum[x].blue+=p[x].blue;
                          pixels_sum[x].opacity+=p[x].opacity;
                        }
                    }
                }
              CloseCacheView(next_view);
            }
        }
      /*
        Average next pixels.
      */
      if (thread_status != MagickFail)
        {
          register PixelPacket
            *q;

          q=SetImagePixelsEx(average_image,0,y,average_image->columns,1,exception);
          if (q == (PixelPacket *) NULL)
            thread_status=MagickFail;
          if (q != (PixelPacket *) NULL)
            {
              for (x=0; x < (long) average_image->columns; x++)
                {
                  q[x].red=(Quantum) (pixels_sum[x].red/number_scenes+0.5);
                  q[x].green=(Quantum) (pixels_sum[x].green/number_scenes+0.5);
                  q[x].blue=(Quantum) (pixels_sum[x].blue/number_scenes+0.5);
                  q[x].opacity=(Quantum) (pixels_sum[x].opacity/number_scenes+0.5);
                }
              if (!SyncImagePixelsEx(average_image,exception))
                thread_status=MagickFail;
            }
        }

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_AverageImages)
#endif
      {
        row_count++;
        if (QuantumTick(row_count,average_image->rows))
          if (!MagickMonitorFormatted(row_count,average_image->rows,exception,
                                      "[%s,...,%s] Average image sequence...",
                                      image->filename,last_image->filename))
            thread_status=MagickFail;
      
        if (thread_status == MagickFail)
          status=MagickFail;
      }
    }

  DestroyThreadViewDataSet(pixels_sums);

  if (status == MagickFail)
    {
      DestroyImage(average_image);
      average_image=(Image *) NULL;
    }

  return(average_image);
}

