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
%                            Y   Y  U   U  V   V                              %
%                             Y Y   U   U  V   V                              %
%                              Y    U   U  V   V                              %
%                              Y    U   U   V V                               %
%                              Y     UUU     V                                %
%                                                                             %
%                                                                             %
%            Read/Write Raw CCIR 601 4:1:1 or 4:2:2 Image Format.             %
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
#include "magick/pixel_cache.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/resize.h"
#include "magick/utility.h"

/*
  Forward declarations.
*/
static unsigned int
  WriteYUVImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d Y U V I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadYUVImage reads an image with digital YUV (CCIR 601 4:1:1, plane
%  or partition interlaced, or 4:2:2 plane, partition interlaced or
%  noninterlaced) bytes and returns it.  It allocates the memory necessary
%  for the new Image structure and returns a pointer to the new image.
%
%  The format of the ReadYUVImage method is:
%
%      Image *ReadYUVImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadYUVImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadYUVImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image
    *chroma_image,
    *image,
    *resize_image;

  long
    horizontal_factor,
    vertical_factor,
    y;

  register const PixelPacket
    *r;

  register long
    x;

  register PixelPacket
    *q,
    *chroma_pixels;

  register long
    i;

  register unsigned char
    *p;

  size_t
    count;

  unsigned char
    *scanline;

  MagickPassFail
    status;

  InterlaceType
    interlace;

  /*
    Allocate image structure.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  status=MagickPass;
  image=AllocateImage(image_info);
  if ((image->columns == 0) || (image->rows == 0))
    ThrowReaderException(OptionError,MustSpecifyImageSize,image);
  image->depth=8;
  interlace=image_info->interlace;
  horizontal_factor=2;
  vertical_factor=2;
  if (image_info->sampling_factor != (char *) NULL)
    {
      long
        factors;

      factors=sscanf(image_info->sampling_factor,"%ldx%ld",&horizontal_factor,
        &vertical_factor);
      if (factors != 2)
        vertical_factor=horizontal_factor;
      if ((horizontal_factor != 1) && (horizontal_factor != 2) &&
          (vertical_factor != 1) && (vertical_factor != 2))
        ThrowReaderException(OptionError,UnsupportedSamplingFactor,
          image);
    }
  if ((interlace == UndefinedInterlace) ||
      ((interlace == NoInterlace) && (vertical_factor == 2)))
    {
      interlace=NoInterlace;    /* CCIR 4:2:2 */
      if (vertical_factor == 2)
        interlace=PlaneInterlace; /* CCIR 4:1:1 */
    }
  if (interlace != PartitionInterlace)
    {
      /*
        Open image file.
      */
      status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
      if (status == MagickFail)
        ThrowReaderException(FileOpenError,UnableToOpenFile,image);
      for (i=0; i < image->offset; i++)
        {
          if (EOF == ReadBlobByte(image))
            ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,
                           image->filename);
        }
    }
  /*
    Allocate memory for a scanline.
  */
  if (interlace == NoInterlace)
    scanline=MagickAllocateMemory(unsigned char *,2*image->columns+2);
  else
    scanline=MagickAllocateMemory(unsigned char *,image->columns);
  if (scanline == (unsigned char *) NULL)
    ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
  do
  {
    chroma_image=CloneImage(image,image->columns/horizontal_factor,
      image->rows/vertical_factor,True,exception);
    if (chroma_image == (Image *) NULL)
      ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
    /*
      Convert raster image to pixel packets.
    */
    if (image_info->ping && (image_info->subrange != 0))
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    if (interlace == PartitionInterlace)
      {
        AppendImageFormat("Y",image->filename);
        status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
        if (status == False)
          ThrowReaderException(FileOpenError,UnableToOpenFile,image);
      }
    for (y=0; y < (long) image->rows; y++)
    {
      if (interlace == NoInterlace)
        {
          if ((y > 0) || (image->previous == (Image *) NULL))
            (void) ReadBlob(image,2*image->columns,scanline);
          p=scanline;
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            {
              status=MagickFail;
              break;
            }
          chroma_pixels=SetImagePixels(chroma_image,0,y,chroma_image->columns,1);
          if (chroma_pixels == (PixelPacket *) NULL)
            {
              status=MagickFail;
              break;
            }
          for (x=0; x < (long) image->columns; x+=2)
          {
            chroma_pixels->red=0;
            chroma_pixels->green=ScaleCharToQuantum(*p++); /* U (Cb) */
            q->red=ScaleCharToQuantum(*p++); /* Y */
            q->green=0;
            q->blue=0;
            q++;
            q->green=0;
            q->blue=0;
            chroma_pixels->blue=ScaleCharToQuantum(*p++); /* V (Cr) */
            q->red=ScaleCharToQuantum(*p++); /* Y */
            chroma_pixels++;
            q++;
          }
        }
      else
        {
          if ((y > 0) || (image->previous == (Image *) NULL))
            (void) ReadBlob(image,image->columns,scanline);
          p=scanline;
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            {
              break;
              status=MagickFail;
            }
          for (x=0; x < (long) image->columns; x++)
          {
            q->red=ScaleCharToQuantum(*p++);
            q->green=0;
            q->blue=0;
            q++;
          }
        }
      if (!SyncImagePixels(image))
        {
          status=MagickFail;
          break;
        }
      if (interlace == NoInterlace)
        if (!SyncImagePixels(chroma_image))
          {
            status=MagickFail;
            break;
          }
      if (image->previous == (Image *) NULL)
        if (!MagickMonitorFormatted(y,image->rows,exception,LoadImageText,
                                    image->filename))
          {
            status=MagickFail;
            break;
          }
    }
    if (status == MagickFail)
      break;
    if (interlace == PartitionInterlace)
      {
        CloseBlob(image);
        AppendImageFormat("U",image->filename);
        status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
        if (status == False)
          ThrowReaderException(FileOpenError,UnableToOpenFile,image);
      }
    if (interlace != NoInterlace)
      {
        for (y=0; y < (long) chroma_image->rows; y++)
        {
          (void) ReadBlob(image,chroma_image->columns,scanline);
          p=scanline;
          q=SetImagePixels(chroma_image,0,y,chroma_image->columns,1);
          if (q == (PixelPacket *) NULL)
            {
              status=MagickFail;
              break;
            }
          for (x=0; x < (long) chroma_image->columns; x++)
          {
            q->red=0;
            q->green=ScaleCharToQuantum(*p++);
            q->blue=0;
            q++;
          }
          if (!SyncImagePixels(chroma_image))
            {
              status=MagickFail;
              break;
            }
        }
        if (status ==MagickFail)
          break;
      if (interlace == PartitionInterlace)
        {
          CloseBlob(image);
          AppendImageFormat("V",image->filename);
          status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
          if (status == False)
            ThrowReaderException(FileOpenError,UnableToOpenFile,image);
        }
      for (y=0; y < (long) chroma_image->rows; y++)
      {
        (void) ReadBlob(image,chroma_image->columns,scanline);
        p=scanline;
        q=GetImagePixels(chroma_image,0,y,chroma_image->columns,1);
        if (q == (PixelPacket *) NULL)
          {
            status=MagickFail;
            break;
          }
        for (x=0; x < (long) chroma_image->columns; x++)
        {
          q->blue=ScaleCharToQuantum(*p++);
          q++;
        }
        if (!SyncImagePixels(chroma_image))
          {
            status=MagickFail;
            break;
          }
      }
      if (status == MagickFail)
        break;
    }
    /*
      Scale image.
    */
    resize_image=ResizeImage(chroma_image,image->columns,image->rows,
      TriangleFilter,1.0,exception);
    DestroyImage(chroma_image);
    if (resize_image == (Image *) NULL)
      ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
    for (y=0; y < (long) image->rows; y++)
    {
      q=GetImagePixels(image,0,y,image->columns,1);
      r=AcquireImagePixels(resize_image,0,y,resize_image->columns,1,
        &resize_image->exception);
      if ((q == (PixelPacket *) NULL) || (r == (const PixelPacket *) NULL))
        {
          status=MagickFail;
          break;
        }
      for (x=0; x < (long) image->columns; x++)
      {
        q->green=r->green;
        q->blue=r->blue;
        r++;
        q++;
      }
      if (!SyncImagePixels(image))
        {
          status=MagickFail;
          break;
        }
    }
    DestroyImage(resize_image);
    if (status == MagickFail)
      break;
    image->colorspace=YCbCrColorspace;
    (void) TransformColorspace(image,RGBColorspace);
    if (interlace == PartitionInterlace)
      (void) strlcpy(image->filename,image_info->filename,MaxTextExtent);
    if (EOFBlob(image))
      {
        ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,
          image->filename);
        break;
      }
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    if (interlace == NoInterlace)
      count=ReadBlob(image,2*image->columns,(char *) scanline);
    else
      count=ReadBlob(image,image->columns,(char *) scanline);
    if (count != 0)
      {
        /*
          Allocate next image structure.
        */
        AllocateNextImage(image_info,image);
        if (image->next == (Image *) NULL)
          {
            DestroyImageList(image);
            return((Image *) NULL);
          }
        image=SyncNextImageInList(image);
        if (!MagickMonitorFormatted(TellBlob(image),GetBlobSize(image),
                                    exception,LoadImagesText,
                                    image->filename))
          break;
      }
  } while (count != 0);
  MagickFreeMemory(scanline);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseBlob(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r Y U V I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterYUVImage adds attributes for the YUV image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterYUVImage method is:
%
%      RegisterYUVImage(void)
%
*/
ModuleExport void RegisterYUVImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("YUV");
  entry->decoder=(DecoderHandler) ReadYUVImage;
  entry->encoder=(EncoderHandler) WriteYUVImage;
  entry->adjoin=False;
  entry->raw=True;
  entry->description="CCIR 601 4:1:1 or 4:2:2 (8-bit only)";
  entry->module="YUV";
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r Y U V I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterYUVImage removes format registrations made by the
%  YUV module from the list of supported formats.
%
%  The format of the UnregisterYUVImage method is:
%
%      UnregisterYUVImage(void)
%
*/
ModuleExport void UnregisterYUVImage(void)
{
  (void) UnregisterMagickInfo("YUV");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e Y U V I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteYUVImage writes an image to a file in the digital YUV
%  (CCIR 601 4:1:1, plane or partition interlaced, or 4:2:2 plane, partition
%  interlaced or noninterlaced) bytes and returns it.
%
%  The format of the WriteYUVImage method is:
%
%      unsigned int WriteYUVImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteYUVImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
static unsigned int WriteYUVImage(const ImageInfo *image_info,Image *image)
{
  Image
    *chroma_image,
    *yuv_image;

  InterlaceType
    interlace;

  long
    horizontal_factor,
    vertical_factor,
    y;

  register const PixelPacket
    *p,
    *s;

  register long
    x;

  unsigned int
    status;

  unsigned long
    scene,
    height,
    width;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  interlace=image_info->interlace;
  horizontal_factor=2;
  vertical_factor=2;
  if (image_info->sampling_factor != (char *) NULL)
    {
      long
        factors;

      factors=sscanf(image_info->sampling_factor,"%ldx%ld",&horizontal_factor,
        &vertical_factor);
      if (factors != 2)
        vertical_factor=horizontal_factor;
      if ((horizontal_factor != 1) && (horizontal_factor != 2) &&
          (vertical_factor != 1) && (vertical_factor != 2))
        ThrowWriterException(OptionError,UnsupportedSamplingFactor,
          image);
    }
  if ((interlace == UndefinedInterlace) ||
      ((interlace == NoInterlace) && (vertical_factor == 2)))
    {
      interlace=NoInterlace;    /* CCIR 4:2:2 */
      if (vertical_factor == 2)
        interlace=PlaneInterlace; /* CCIR 4:1:1 */
    }
  if (interlace != PartitionInterlace)
    {
      /*
        Open output image file.
      */
      status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
      if (status == False)
        ThrowWriterException(FileOpenError,UnableToOpenFile,image);
    }
  else
    {
      AppendImageFormat("Y",image->filename);
      status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
      if (status == False)
        ThrowWriterException(FileOpenError,UnableToOpenFile,image);
    }
  scene=0;
  do
  {
    /*
      Sample image to an even width and height, if necessary.
    */
    image->depth=8;
    (void) TransformColorspace(image,RGBColorspace);
    width=image->columns+(image->columns & (horizontal_factor-1));
    height=image->rows+(image->rows & (vertical_factor-1));
    yuv_image=ResizeImage(image,width,height,TriangleFilter,1.0,
      &image->exception);
    if (yuv_image == (Image *) NULL)
      ThrowWriterException2(ResourceLimitError,image->exception.reason,image);
    (void) TransformColorspace(yuv_image,YCbCrColorspace);
    /*
      Downsample image.
    */
    chroma_image=ResizeImage(image,width/horizontal_factor,
      height/vertical_factor,TriangleFilter,1.0,&image->exception);
    if (chroma_image == (Image *) NULL)
      ThrowWriterException2(ResourceLimitError,image->exception.reason,image);
    (void) TransformColorspace(chroma_image,YCbCrColorspace);
    if (interlace == NoInterlace)
      {
        /*
          Write noninterlaced YUV.
        */
        for (y=0; y < (long) yuv_image->rows; y++)
        {
          p=AcquireImagePixels(yuv_image,0,y,yuv_image->columns,1,
            &yuv_image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          s=AcquireImagePixels(chroma_image,0,y,chroma_image->columns,1,
            &chroma_image->exception);
          if (s == (const PixelPacket *) NULL)
            break;
          for (x=0; x < (long) yuv_image->columns; x++)
          {
            (void) WriteBlobByte(image,ScaleQuantumToChar(s->green));
            (void) WriteBlobByte(image,ScaleQuantumToChar(p->red));
            p++;
            (void) WriteBlobByte(image,ScaleQuantumToChar(s->blue));
            (void) WriteBlobByte(image,ScaleQuantumToChar(p->red));
            p++;
            s++;
            x++;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitorFormatted(y,image->rows,&image->exception,
                                          SaveImageText,image->filename))
                break;
        }
        DestroyImage(yuv_image);
      }
    else
      {
        /*
          Initialize Y channel.
        */
        for (y=0; y < (long) yuv_image->rows; y++)
        {
          p=AcquireImagePixels(yuv_image,0,y,yuv_image->columns,1,
            &yuv_image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          for (x=0; x < (long) yuv_image->columns; x++)
          {
            (void) WriteBlobByte(image,ScaleQuantumToChar(p->red));
            p++;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitorFormatted(y,image->rows,&image->exception,
                                          SaveImageText,image->filename))
                break;
        }
        DestroyImage(yuv_image);
        /*
          Initialize U channel.
        */
        if (interlace == PartitionInterlace)
          {
            CloseBlob(image);
            AppendImageFormat("U",image->filename);
            status=OpenBlob(image_info,image,WriteBinaryBlobMode,
              &image->exception);
            if (status == False)
              ThrowWriterException(FileOpenError,UnableToOpenFile,image);
          }
        for (y=0; y < (long) chroma_image->rows; y++)
        {
          p=AcquireImagePixels(chroma_image,0,y,chroma_image->columns,1,
            &chroma_image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          for (x=0; x < (long) chroma_image->columns; x++)
          {
            (void) WriteBlobByte(image,ScaleQuantumToChar(p->green));
            p++;
          }
        }
        /*
          Initialize V channel.
        */
        if (interlace == PartitionInterlace)
          {
            CloseBlob(image);
            AppendImageFormat("V",image->filename);
            status=OpenBlob(image_info,image,WriteBinaryBlobMode,
              &image->exception);
            if (status == False)
              ThrowWriterException(FileOpenError,UnableToOpenFile,image);
          }
        for (y=0; y < (long) chroma_image->rows; y++)
        {
          p=AcquireImagePixels(chroma_image,0,y,chroma_image->columns,1,
            &chroma_image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          for (x=0; x < (long) chroma_image->columns; x++)
          {
            (void) WriteBlobByte(image,ScaleQuantumToChar(p->blue));
            p++;
          }
        }
      }
    DestroyImage(chroma_image);
    if (interlace == PartitionInterlace)
      (void) strlcpy(image->filename,image_info->filename,MaxTextExtent);
    if (image->next == (Image *) NULL)
      break;
    image=SyncNextImageInList(image);
    status=MagickMonitorFormatted(scene++,GetImageListLength(image),
                                  &image->exception,SaveImagesText,
                                  image->filename);
    if (status == False)
      break;
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseBlob(image);
  return(True);
}
