/*
% Copyright (C) 2003 - 2009 GraphicsMagick Group
% Copyright (C) 2003 ImageMagick Studio
% Copyright 1991-1999 E. I. du Pont de Nemours and Company
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
% GraphicsMagick Gradient Image Methods.
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/alpha_composite.h"
#include "magick/gradient.h"
#include "magick/monitor.h"
#include "magick/pixel_cache.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
+     G r a d i e n t I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GradientImage() applies a continuously smooth color transitions along a
%  vector from one color to another.
%
%  Note, the interface of this method will change in the future to support
%  more than one transistion.
%
%  The format of the GradientImage method is:
%
%      MagickPassFail GradientImage(Image *image,
%        const PixelPacket *start_color,
%        const PixelPacket *stop_color)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o start_color: The start color.
%
%    o stop_color: The stop color.
%
%
*/

#define GradientImageText "[%s] Gradient..."
MagickExport MagickPassFail GradientImage(Image *image,
                                          const PixelPacket *start_color,
                                          const PixelPacket *stop_color)
{
  const unsigned long
    image_rows=image->rows,
    image_columns=image->columns;

  long
    y;

  unsigned long
    row_count=0;

  MagickPassFail
    status=MagickPass;

  /*
    Determine (Hue, Saturation, Brightness) gradient.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(start_color != (const PixelPacket *) NULL);
  assert(stop_color != (const PixelPacket *) NULL);

  /*
    Generate gradient pixels.
  */
#if defined(HAVE_OPENMP)
#  pragma omp parallel for shared(row_count, status)
#endif
  for (y=0; y < (long) image->rows; y++)
    {
      MagickPassFail
        thread_status;

      register long
        x;
      
      register PixelPacket
        *q;

      thread_status=status;
      if (thread_status == MagickFail)
        continue;

      q=SetImagePixelsEx(image,0,y,image->columns,1,&image->exception);
      if (q == (PixelPacket *) NULL)
        thread_status=MagickFail;

      if (q != (PixelPacket *) NULL)
        {
          for (x=0; x < (long) image->columns; x++)
            {
              BlendCompositePixel(&q[x],start_color,stop_color,(double)
                                  MaxRGB*(y*image_columns+x)/(image_columns*image_rows));
            }

          if (!SyncImagePixelsEx(image,&image->exception))
            thread_status=MagickFail;
        }

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_GradientImage)
#endif
      {
        row_count++;
        if (QuantumTick(row_count,image->rows))
          if (!MagickMonitorFormatted(row_count,image->rows,&image->exception,
                                      GradientImageText,image->filename))
            thread_status=MagickFail;

        if (thread_status == MagickFail)
          status=MagickFail;
      }
    }
  return(status);
}
