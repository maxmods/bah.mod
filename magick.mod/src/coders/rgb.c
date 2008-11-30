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
%                            RRRR    GGGG  BBBB                               %
%                            R   R  G      B   B                              %
%                            RRRR   G  GG  BBBB                               %
%                            R R    G   G  B   B                              %
%                            R  R    GGG   BBBB                               %
%                                                                             %
%                                                                             %
%                    Read/Write Raw RGB Image Format.                         %
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
#include "magick/constitute.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/utility.h"

/*
  Forward declarations.
*/
static unsigned int
  WriteRGBImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d R G B I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadRGBImage reads an image of raw red, green, and blue samples and
%  returns it.  It allocates the memory necessary for the new Image structure
%  and returns a pointer to the new image.
%
%  The format of the ReadRGBImage method is:
%
%      Image *ReadRGBImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadRGBImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadRGBImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image
    *image;

  long
    y;

  register long
    i,
    x;

  register PixelPacket
    *q;

  size_t
    count;

  unsigned char
    *scanline;

  unsigned int
    status;

  unsigned int
    packet_size,
    quantum_size;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  if ((image->columns == 0) || (image->rows == 0))
    ThrowReaderException(OptionError,MustSpecifyImageSize,image);
  if (image_info->interlace != PartitionInterlace)
    {
      /*
        Open image file.
      */
      status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
      if (status == False)
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
  if (image->depth <= 8)
    quantum_size=8;
  else if (image->depth <= 16)
    quantum_size=16;
  else
    quantum_size=32;

  packet_size=(quantum_size*3)/8;
  if (LocaleCompare(image_info->magick,"RGBA") == 0)
    {
      image->matte=True;
      packet_size=(quantum_size*4)/8;
    }
  scanline=MagickAllocateMemory(unsigned char *,
    packet_size*image->tile_info.width);
  if (scanline == (unsigned char *) NULL)
    ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
  if (image_info->subrange != 0)
    while (image->scene < image_info->subimage)
    {
      /*
        Skip to next image.
      */
      image->scene++;
      for (y=0; y < (long) image->rows; y++)
        (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
    }
  x=(long) (packet_size*image->tile_info.x);
  do
  {
    /*
      Convert raster image to pixel packets.
    */
    if (image_info->ping && (image_info->subrange != 0))
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    switch (image_info->interlace)
    {
      case NoInterlace:
      default:
      {
        /*
          No interlacing:  RGBRGBRGBRGBRGBRGB...
        */
        for (y=0; y < image->tile_info.y; y++)
          (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
        for (y=0; y < (long) image->rows; y++)
        {
          if ((y > 0) || (image->previous == (Image *) NULL))
            (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          if (!image->matte)
            (void) ImportImagePixelArea(image,RGBQuantum,quantum_size,scanline+x,0,0);
          else
            (void) ImportImagePixelArea(image,RGBAQuantum,quantum_size,scanline+x,0,0);
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitorFormatted(y,image->rows,exception,
                                          LoadImageText,image->filename))
                break;
        }
        count=image->tile_info.height-image->rows-image->tile_info.y;
        for (i=0; i < (long) count; i++)
          (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
        break;
      }
      case LineInterlace:
      {
        /*
          Line interlacing:  RRR...GGG...BBB...RRR...GGG...BBB...
        */
        packet_size=(quantum_size)/8;
        for (y=0; y < image->tile_info.y; y++)
          (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
        for (y=0; y < (long) image->rows; y++)
        {
          if ((y > 0) || (image->previous == (Image *) NULL))
            (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          (void) ImportImagePixelArea(image,RedQuantum,quantum_size,scanline+x,0,0);
          (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
          (void) ImportImagePixelArea(image,GreenQuantum,quantum_size,scanline+x,0,0);
          (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
          (void) ImportImagePixelArea(image,BlueQuantum,quantum_size,scanline+x,0,0);
          if (image->matte)
            {
              (void) ReadBlob(image,packet_size*image->tile_info.width,
                scanline);
              (void) ImportImagePixelArea(image,AlphaQuantum,quantum_size,scanline+x,0,0);
            }
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitorFormatted(y,image->rows,exception,
                                          LoadImageText,image->filename))
                break;
        }
        count=image->tile_info.height-image->rows-image->tile_info.y;
        for (i=0; i < (long) count; i++)
          (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
        break;
      }
      case PlaneInterlace:
      case PartitionInterlace:
      {
        unsigned long
          span;

        /*
          Plane interlacing:  RRRRRR...GGGGGG...BBBBBB...
        */
        if (image_info->interlace == PartitionInterlace)
          {
            AppendImageFormat("R",image->filename);
            status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
            if (status == False)
              ThrowReaderException(FileOpenError,UnableToOpenFile,image);
          }
        packet_size=(quantum_size)/8;
        for (y=0; y < image->tile_info.y; y++)
          (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
        i=0;
        span=image->rows*(image->matte ? 4 : 3);
        for (y=0; y < (long) image->rows; y++)
        {
          if ((y > 0) || (image->previous == (Image *) NULL))
            (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          (void) ImportImagePixelArea(image,RedQuantum,quantum_size,scanline+x,0,0);
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(i,span))
              if (!MagickMonitorFormatted(i,span,&image->exception,
                                          LoadImageText,image->filename))
                break;
          i++;
        }
        count=image->tile_info.height-image->rows-image->tile_info.y;
        for (i=0; i < (long) count; i++)
          (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
        if (image_info->interlace == PartitionInterlace)
          {
            CloseBlob(image);
            AppendImageFormat("G",image->filename);
            status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
            if (status == False)
              ThrowReaderException(FileOpenError,UnableToOpenFile,image);
          }
        for (y=0; y < image->tile_info.y; y++)
          (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
        for (y=0; y < (long) image->rows; y++)
        {
          (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
          q=GetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          (void) ImportImagePixelArea(image,GreenQuantum,quantum_size,scanline+x,0,0);
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(i,span))
              if (!MagickMonitorFormatted(i,span,&image->exception,
                                          LoadImageText,image->filename))
                break;
          i++;
        }
        count=image->tile_info.height-image->rows-image->tile_info.y;
        for (i=0; i < (long) count; i++)
          (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
        if (image_info->interlace == PartitionInterlace)
          {
            CloseBlob(image);
            AppendImageFormat("B",image->filename);
            status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
            if (status == False)
              ThrowReaderException(FileOpenError,UnableToOpenFile,image);
          }
        for (y=0; y < image->tile_info.y; y++)
          (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
        for (y=0; y < (long) image->rows; y++)
        {
          (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
          q=GetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          (void) ImportImagePixelArea(image,BlueQuantum,quantum_size,scanline+x,0,0);
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(i,span))
              if (!MagickMonitorFormatted(i,span,&image->exception,
                                          LoadImageText,image->filename))
                break;
          i++;
        }
        count=image->tile_info.height-image->rows-image->tile_info.y;
        for (i=0; i < (long) count; i++)
          (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
        if (image->matte)
          {
            /*
              Read matte channel.
            */
            if (image_info->interlace == PartitionInterlace)
              {
                CloseBlob(image);
                AppendImageFormat("A",image->filename);
                status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
                if (status == False)
                  ThrowReaderException(FileOpenError,UnableToOpenFile,image);
              }
            for (y=0; y < image->tile_info.y; y++)
              (void) ReadBlob(image,packet_size*image->tile_info.width,
                scanline);
            for (y=0; y < (long) image->rows; y++)
            {
              (void) ReadBlob(image,packet_size*image->tile_info.width,
                scanline);
              q=GetImagePixels(image,0,y,image->columns,1);
              if (q == (PixelPacket *) NULL)
                break;
              (void) ImportImagePixelArea(image,AlphaQuantum,quantum_size,scanline+x,0,0);
              if (!SyncImagePixels(image))
                break;
              if (image->previous == (Image *) NULL)
                if (QuantumTick(i,span))
                  if (!MagickMonitorFormatted(i,span,&image->exception,
                                              LoadImageText,image->filename))
                    break;
              i++;
            }
            count=image->tile_info.height-image->rows-image->tile_info.y;
            for (i=0; i < (long) count; i++)
              (void) ReadBlob(image,packet_size*image->tile_info.width,
                scanline);
          }
        if (image_info->interlace == PartitionInterlace)
          (void) strlcpy(image->filename,image_info->filename,MaxTextExtent);
        break;
      }
    }
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
    if (image_info->interlace == PartitionInterlace)
      break;
    count=ReadBlob(image,packet_size*image->tile_info.width,scanline);
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
%   R e g i s t e r R G B I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterRGBImage adds attributes for the RGB image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterRGBImage method is:
%
%      RegisterRGBImage(void)
%
*/
ModuleExport void RegisterRGBImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("RGB");
  entry->decoder=(DecoderHandler) ReadRGBImage;
  entry->encoder=(EncoderHandler) WriteRGBImage;
  entry->raw=True;
  entry->description="Raw red, green, and blue samples";
  entry->module="RGB";
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("RGBA");
  entry->decoder=(DecoderHandler) ReadRGBImage;
  entry->encoder=(EncoderHandler) WriteRGBImage;
  entry->raw=True;
  entry->description="Raw red, green, blue, and matte samples";
  entry->module="RGB";
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r R G B I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterRGBImage removes format registrations made by the
%  RGB module from the list of supported formats.
%
%  The format of the UnregisterRGBImage method is:
%
%      UnregisterRGBImage(void)
%
*/
ModuleExport void UnregisterRGBImage(void)
{
  (void) UnregisterMagickInfo("RGB");
  (void) UnregisterMagickInfo("RGBA");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e R G B I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteRGBImage writes an image to a file in red, green, and blue
%  rasterfile format.
%
%  The format of the WriteRGBImage method is:
%
%      unsigned int WriteRGBImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteRGBImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
static unsigned int WriteRGBImage(const ImageInfo *image_info,Image *image)
{
  int
    y;

  register const PixelPacket
    *p;

  unsigned char
    *pixels;

  unsigned int
    status;

  unsigned int
    packet_size,
    quantum_size,
    scene;

  /*
    Allocate memory for pixels.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  if (image->depth <= 8)
    quantum_size=8;
  else if (image->depth <= 16)
    quantum_size=16;
  else
    quantum_size=32;

  packet_size=(quantum_size*3)/8;
  if (LocaleCompare(image_info->magick,"RGBA") == 0)
    packet_size=(quantum_size*4)/8;
  pixels=MagickAllocateMemory(unsigned char *,packet_size*image->columns);
  if (pixels == (unsigned char *) NULL)
    ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
  if (image_info->interlace != PartitionInterlace)
    {
      /*
        Open output image file.
      */
      status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
      if (status == False)
        ThrowWriterException(FileOpenError,UnableToOpenFile,image);
    }
  scene=0;
  do
  {
    /*
      Convert MIFF to RGB raster pixels.
    */
    (void) TransformColorspace(image,RGBColorspace);
    if (LocaleCompare(image_info->magick,"RGBA") == 0)
      if (!image->matte)
        SetImageOpacity(image,OpaqueOpacity);
    switch (image_info->interlace)
    {
      case NoInterlace:
      default:
      {
        /*
          No interlacing:  RGBRGBRGBRGBRGBRGB...
        */
        for (y=0; y < (long) image->rows; y++)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          if (LocaleCompare(image_info->magick,"RGBA") != 0)
            {
              (void) ExportImagePixelArea(image,RGBQuantum,quantum_size,pixels,0,0);
              (void) WriteBlob(image,packet_size*image->columns,pixels);
            }
          else
            {
              (void) ExportImagePixelArea(image,RGBAQuantum,quantum_size,pixels,0,0);
              (void) WriteBlob(image,packet_size*image->columns,pixels);
            }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitorFormatted(y,image->rows,&image->exception,
                                          SaveImageText,image->filename))
                break;
        }
        break;
      }
      case LineInterlace:
      {
        /*
          Line interlacing:  RRR...GGG...BBB...RRR...GGG...BBB...
        */
        for (y=0; y < (long) image->rows; y++)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          (void) ExportImagePixelArea(image,RedQuantum,quantum_size,pixels,0,0);
          (void) WriteBlob(image,image->columns,pixels);
          (void) ExportImagePixelArea(image,GreenQuantum,quantum_size,pixels,0,0);
          (void) WriteBlob(image,image->columns,pixels);
          (void) ExportImagePixelArea(image,BlueQuantum,quantum_size,pixels,0,0);
          (void) WriteBlob(image,image->columns,pixels);
          if (LocaleCompare(image_info->magick,"RGBA") == 0)
            {
              (void) ExportImagePixelArea(image,AlphaQuantum,quantum_size,pixels,0,0);
              (void) WriteBlob(image,image->columns,pixels);
            }
          if (QuantumTick(y,image->rows))
            if (!MagickMonitorFormatted(y,image->rows,&image->exception,
                                        SaveImageText,image->filename))
              break;
        }
        break;
      }
      case PlaneInterlace:
      case PartitionInterlace:
      {
        /*
          Plane interlacing:  RRRRRR...GGGGGG...BBBBBB...
        */
        if (image_info->interlace == PartitionInterlace)
          {
            AppendImageFormat("R",image->filename);
            status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
            if (status == False)
              ThrowWriterException(FileOpenError,UnableToOpenFile,image);
          }
        for (y=0; y < (long) image->rows; y++)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          (void) ExportImagePixelArea(image,RedQuantum,quantum_size,pixels,0,0);
          (void) WriteBlob(image,image->columns,pixels);
        }
        if (image_info->interlace == PartitionInterlace)
          {
            CloseBlob(image);
            AppendImageFormat("G",image->filename);
            status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
            if (status == False)
              ThrowWriterException(FileOpenError,UnableToOpenFile,image);
          }
        if (!MagickMonitorFormatted(100,400,&image->exception,SaveImageText,
                                    image->filename))
          break;
        for (y=0; y < (long) image->rows; y++)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          (void) ExportImagePixelArea(image,GreenQuantum,quantum_size,pixels,0,0);
          (void) WriteBlob(image,image->columns,pixels);
        }
        if (image_info->interlace == PartitionInterlace)
          {
            CloseBlob(image);
            AppendImageFormat("B",image->filename);
            status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
            if (status == False)
              ThrowWriterException(FileOpenError,UnableToOpenFile,image);
          }
        if (!MagickMonitorFormatted(200,400,&image->exception,SaveImageText,
                                    image->filename))
          break;
        for (y=0; y < (long) image->rows; y++)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          (void) ExportImagePixelArea(image,BlueQuantum,quantum_size,pixels,0,0);
          (void) WriteBlob(image,image->columns,pixels);
        }
        if (LocaleCompare(image_info->magick,"RGBA") == 0)
          {
            if (!MagickMonitorFormatted(300,400,&image->exception,
                                        SaveImageText,image->filename))
              break;
            if (image_info->interlace == PartitionInterlace)
              {
                CloseBlob(image);
                AppendImageFormat("A",image->filename);
                status=OpenBlob(image_info,image,WriteBinaryBlobMode,
                  &image->exception);
                if (status == False)
                  ThrowWriterException(FileOpenError,UnableToOpenFile,image);
              }
            for (y=0; y < (long) image->rows; y++)
            {
              p=AcquireImagePixels(image,0,y,image->columns,1,
                &image->exception);
              if (p == (const PixelPacket *) NULL)
                break;
              (void) ExportImagePixelArea(image,AlphaQuantum,quantum_size,pixels,0,0);
              (void) WriteBlob(image,image->columns,pixels);
            }
          }
        if (image_info->interlace == PartitionInterlace)
          (void) strlcpy(image->filename,image_info->filename,MaxTextExtent);
        if (!MagickMonitorFormatted(400,400,&image->exception,SaveImageText,
                                    image->filename))
          break;
        break;
      }
    }
    if (image->next == (Image *) NULL)
      break;
    image=SyncNextImageInList(image);
    if (!MagickMonitorFormatted(scene++,GetImageListLength(image),
                                &image->exception,SaveImagesText,
                                image->filename))
      break;
  } while (image_info->adjoin);
  MagickFreeMemory(pixels);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseBlob(image);
  return(True);
}
