/*
% Copyright (C) 2003 - 2009 GraphicsMagick Group
% Copyright (C) 2003 ImageMagick Studio
% Copyright 1991-1999 E. I. du Pont de Nemours and Company
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
% GraphicsMagick Texture Methods.
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/alpha_composite.h"
#include "magick/monitor.h"
#include "magick/pixel_cache.h"
#include "magick/texture.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     C o n s t i t u t e T e x t u r e I m a g e                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ConstituteTextureImage() returns a new image canvas based on repeatedly
%  tiling the texture image across and down the new image canvas.  The
%  returned image properties are similar to the texture image properties.
%
%  The format of the TextureImage method is:
%
%      Image *ConstituteTextureImage(unsigned long columns,unsigned long rows,
%                              const Image *texture, ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o columns: The number of columns in the new image.
%
%    o rows: The number of rows in the new image.
%
%    o texture: The texture image to layer on the background.
%
%    o exceptions: Any errors are reported here.
%
*/
#define ConstituteTextureImageText "[%s] Generate texture...  "
MagickExport Image *ConstituteTextureImage(const unsigned long columns,
                                           const unsigned long rows,
                                           const Image *texture_image,
                                           ExceptionInfo *exception)
{
  Image
    *canvas_image;

  long
    y;

  unsigned long
    row_count=0;

  MagickPassFail
    status=MagickPass;

  assert(texture_image != (Image *) NULL);
  assert(texture_image->signature == MagickSignature);

  canvas_image=CloneImage(texture_image,columns,rows,MagickTrue,exception);
  if (canvas_image == (Image *) NULL)
    return canvas_image;
#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  if defined(TUNE_OPENMP)
#    pragma omp parallel for schedule(runtime) shared(row_count, status)
#  else
#    pragma omp parallel for schedule(static,16) shared(row_count, status)
#  endif
#endif
  for (y=0; y < (long) canvas_image->rows; y++)
    {
      const PixelPacket
        *texture_pixels;

      PixelPacket
        *canvas_pixels;

      unsigned long
        x;

      MagickBool
        thread_status;

#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  pragma omp critical (GM_ConstituteTextureImage)
#endif
      thread_status=status;
      if (thread_status == MagickFail)
        continue;

      texture_pixels=AcquireImagePixels(texture_image,0,
                                        y % texture_image->rows,
                                        texture_image->columns,1,
                                        exception);
      canvas_pixels=SetImagePixelsEx(canvas_image,0,y,canvas_image->columns,
                                     1,exception);

      if ((texture_pixels == (const PixelPacket *) NULL) ||
          (canvas_pixels == (PixelPacket *) NULL))
        thread_status=MagickFail;

      if (thread_status != MagickFail)
        {
          const IndexPacket
            *texture_indexes=(const IndexPacket *) NULL;
          
          IndexPacket
            *canvas_indexes=(IndexPacket *) NULL;;

          if (texture_image->storage_class == PseudoClass)
            texture_indexes=AccessImmutableIndexes(texture_image);
          if (canvas_image->storage_class == PseudoClass)
            canvas_indexes=AccessMutableIndexes(canvas_image);

          for (x=0; x < canvas_image->columns; x+=texture_image->columns)
            {
              unsigned long
                texture_width;

              texture_width=texture_image->columns;
              if ((x+texture_width) > canvas_image->columns)
                texture_width=canvas_image->columns-x;

              if ((texture_indexes != (const IndexPacket *) NULL) &&
                  (canvas_indexes != (const IndexPacket *) NULL))
                {
                  (void) memcpy(canvas_indexes,texture_indexes,texture_width*sizeof(IndexPacket));
                  canvas_indexes += texture_width;
                }
              (void) memcpy(canvas_pixels,texture_pixels,texture_width*sizeof(PixelPacket));
              canvas_pixels += texture_width;
            }

          if (!SyncImagePixelsEx(canvas_image,exception))
            thread_status=MagickFail;
        }
#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  pragma omp critical (GM_ConstituteTextureImage)
#endif
      {
        row_count++;
        if (QuantumTick(row_count,canvas_image->rows))
          if (!MagickMonitorFormatted(row_count,canvas_image->rows,exception,
                                      ConstituteTextureImageText,
                                      texture_image->filename))
            thread_status=MagickFail;

        if (thread_status == MagickFail)
          status=MagickFail;
      }
    }

  if (status == MagickFail)
    {
      DestroyImage(canvas_image);
      canvas_image=(Image *) NULL;
    }

  if (canvas_image != (Image *) NULL)
    {
      canvas_image->is_monochrome=texture_image->is_monochrome;
      canvas_image->is_grayscale=texture_image->is_grayscale;
    }

  return canvas_image;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     T e x t u r e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  TextureImage() repeatedly tiles the texture image across and down the image
%  canvas. If the image canvas includes a matte channel, then the tile is
%  alpha-composited "under" the image and the matte channel is removed.
%  MagickFail is returned if an error is encountered.
%
%  The format of the TextureImage method is:
%
%      MagickPassFail TextureImage(Image *image,const Image *texture)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o texture: This image is the texture to layer on the background.
%
%
*/

#define TextureImageText  "[%s] Apply texture..."
MagickExport MagickPassFail TextureImage(Image *image,const Image *texture)
{
  MagickPassFail
    status=MagickPass;

  long
    y;

  unsigned long
    row_count=0;

  MagickBool
    get_pixels,
    is_grayscale;

  /*
    Tile texture onto the image background.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (texture == (const Image *) NULL)
    return MagickFail;

  get_pixels=GetPixelCachePresent(image);
  is_grayscale=image->is_grayscale;
  image->storage_class=DirectClass;
#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  if defined(TUNE_OPENMP)
#    pragma omp parallel for schedule(runtime) shared(row_count, status)
#  else
#    pragma omp parallel for schedule(static,4) shared(row_count, status)
#  endif
#endif
  for (y=0; y < (long) image->rows; y++)
    {
      const PixelPacket
        *pixels;

      long
        x;

      register long
        z;

      register const PixelPacket
        *p;

      register PixelPacket
        *q;

      unsigned long
        width;

      MagickBool
        thread_status;

#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  pragma omp critical (GM_TextureImage)
#endif
      thread_status=status;
      if (thread_status == MagickFail)
        continue;

      p=AcquireImagePixels(texture,0,y % texture->rows,
                           texture->columns,1,&image->exception);
      if (get_pixels)
        q=GetImagePixelsEx(image,0,y,image->columns,1,&image->exception);
      else
        q=SetImagePixelsEx(image,0,y,image->columns,1,&image->exception);

      if ((p == (const PixelPacket *) NULL) ||
          (q == (PixelPacket *) NULL))
        thread_status=MagickFail;

      if (thread_status != MagickFail)
        {
          pixels=p;
          for (x=0; x < (long) image->columns; x+=texture->columns)
            {
              width=texture->columns;
              if ((x+width) > image->columns)
                width=image->columns-x;
              p=pixels;
              if (image->matte)
                {
                  for (z=(long) width; z != 0; z--)
                    {
                      AlphaCompositePixel(q,q,q->opacity,p,texture->matte != MagickFalse ?
                                          p->opacity : OpaqueOpacity);
                      p++;
                      q++;
                    }
                }
              else
                {
                  if (width*sizeof(PixelPacket) < 1024)
                    {
                      for (z=(long) width; z != 0; z--)
                        {
                          *q=(*p);
                          p++;
                          q++;
                        }
                    }
                  else
                    {
                      (void) memcpy(q,p,width*sizeof(PixelPacket));
                      q += width;
                    }
                }
            }
          if (!SyncImagePixelsEx(image,&image->exception))
            thread_status=MagickFail;
        }
#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  pragma omp critical (GM_TextureImage)
#endif
      {
        row_count++;
        if (QuantumTick(row_count,image->rows))
          if (!MagickMonitorFormatted(row_count,image->rows,&image->exception,
                                      TextureImageText,image->filename))
            thread_status=MagickFail;

        if (thread_status == MagickFail)
          status=MagickFail;
      }
    }

  if (image->matte)
    image->is_grayscale=(is_grayscale && texture->is_grayscale);
  else
    image->is_grayscale=texture->is_grayscale;
  if (image->matte)
    image->matte=MagickFalse;
  else
    image->matte=texture->matte;
  return (status);
}
