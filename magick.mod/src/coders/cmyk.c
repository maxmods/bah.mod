/*
% Copyright (C) 2003, 2004 GraphicsMagick Group
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
%                         CCCC  M   M  Y   Y  K   K                           %
%                        C      MM MM   Y Y   K  K                            %
%                        C      M M M    Y    KKK                             %
%                        C      M   M    Y    K  K                            %
%                         CCCC  M   M    Y    K   K                           %
%                                                                             %
%                                                                             %
%                     Read/Write RAW CMYK Image Format.                       %
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
#include "magick/pixel_cache.h"
#include "magick/constitute.h"
#include "magick/blob.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/utility.h"

/*
  Forward declarations.
*/
static unsigned int
  WriteCMYKImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d C M Y K I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadCMYKImage reads an image of raw cyan, magenta, yellow, and black
%  samples and returns it.  It allocates the memory necessary for the new
%  Image structure and returns a pointer to the new image.
%
%  The format of the ReadCMYKImage method is:
%
%      Image *ReadCMYKImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadCMYKImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadCMYKImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
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

  packet_size=(quantum_size*4)/8;
  if (LocaleCompare(image_info->magick,"CMYKA") == 0)
    {
      image->matte=True;
      packet_size=(quantum_size*5)/8;
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
    image->colorspace=CMYKColorspace;
    if (image_info->ping && (image_info->subrange != 0))
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    switch (image_info->interlace)
    {
      case NoInterlace:
      default:
      {
        /*
          No interlacing:  CMYKCMYKCMYKCMYKCMYKCMYK...
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
            (void) ImportImagePixelArea(image,CMYKQuantum,quantum_size,scanline+x,0,0);
          else
            (void) ImportImagePixelArea(image,CMYKAQuantum,quantum_size,scanline+x,0,0);
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
          Line interlacing:  CCC...MMM...YYY...KKK...CCC...MMM...YYY...KKK...
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
          (void) ImportImagePixelArea(image,CyanQuantum,quantum_size,scanline+x,0,0);
          (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
          (void) ImportImagePixelArea(image,MagentaQuantum,quantum_size,scanline+x,0,0);
          (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
          (void) ImportImagePixelArea(image,YellowQuantum,quantum_size,scanline+x,0,0);
          (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
          (void) ImportImagePixelArea(image,BlackQuantum,quantum_size,scanline+x,0,0);
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
          Plane interlacing:  CCCCCC...MMMMMM...YYYYYY...KKKKKK...
        */
        if (image_info->interlace == PartitionInterlace)
          {
            AppendImageFormat("C",image->filename);
            status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
            if (status == False)
              ThrowReaderException(FileOpenError,UnableToOpenFile,image);
          }
        packet_size=(quantum_size)/8;
        for (y=0; y < image->tile_info.y; y++)
          (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
        i=0;
        span=image->rows*(image->matte ? 5 : 4);
        for (y=0; y < (long) image->rows; y++)
        {
          if ((y > 0) || (image->previous == (Image *) NULL))
            (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          (void) ImportImagePixelArea(image,CyanQuantum,quantum_size,scanline+x,0,0);
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
            AppendImageFormat("M",image->filename);
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
          (void) ImportImagePixelArea(image,MagentaQuantum,quantum_size,scanline+x,0,0);
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
            AppendImageFormat("Y",image->filename);
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
          (void) ImportImagePixelArea(image,YellowQuantum,quantum_size,scanline+x,0,0);
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
            AppendImageFormat("K",image->filename);
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
          (void) ImportImagePixelArea(image,BlackQuantum,quantum_size,scanline+x,0,0);
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
        status=MagickMonitorFormatted(TellBlob(image),GetBlobSize(image),
                                      exception,LoadImagesText,
                                      image->filename);
        if (status == False)
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
%   R e g i s t e r C M Y K I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterCMYKImage adds attributes for the CMYK image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterCMYKImage method is:
%
%      RegisterCMYKImage(void)
%
*/
ModuleExport void RegisterCMYKImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("CMYK");
  entry->decoder=(DecoderHandler) ReadCMYKImage;
  entry->encoder=(EncoderHandler) WriteCMYKImage;
  entry->raw=True;
  entry->description="Raw cyan, magenta, yellow, and black samples";
  entry->module="CMYK";
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("CMYKA");
  entry->decoder=(DecoderHandler) ReadCMYKImage;
  entry->encoder=(EncoderHandler) WriteCMYKImage;
  entry->raw=True;
  entry->description="Raw cyan, magenta, yellow, black, and opacity samples";
  entry->module="CMYK";
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r C M Y K I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterCMYKImage removes format registrations made by the
%  CMYK module from the list of supported formats.
%
%  The format of the UnregisterCMYKImage method is:
%
%      UnregisterCMYKImage(void)
%
*/
ModuleExport void UnregisterCMYKImage(void)
{
  (void) UnregisterMagickInfo("CMYK");
  (void) UnregisterMagickInfo("CMYKA");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e C M Y K I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteCMYKImage writes an image to a file in red, green, and blue
%  rasterfile format.
%
%  The format of the WriteCMYKImage method is:
%
%      unsigned int WriteCMYKImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteCMYKImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
static unsigned int WriteCMYKImage(const ImageInfo *image_info,Image *image)
{
  int
    y;

  register const PixelPacket
    *p;

  unsigned char
    *pixels;

  unsigned int
    packet_size,
    quantum_size,
    scene,
    status;

  if (image->depth <= 8)
    quantum_size=8;
  else if (image->depth <= 16)
    quantum_size=16;
  else
    quantum_size=32;

  /*
    Allocate memory for pixels.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  packet_size=(quantum_size*4)/8;
  if (LocaleCompare(image_info->magick,"CMYKA") == 0)
    packet_size=(quantum_size*5)/8;
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
      Convert MIFF to CMYK raster pixels.
    */
    (void) TransformColorspace(image,CMYKColorspace);
    if (LocaleCompare(image_info->magick,"CMYKA") == 0)
      if (!image->matte)
        SetImageOpacity(image,OpaqueOpacity);
    switch (image_info->interlace)
    {
      case NoInterlace:
      default:
      {
        /*
          No interlacing:  CMYKCMYKCMYKCMYKCMYKCMYK...
        */
        for (y=0; y < (long) image->rows; y++)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          if (LocaleCompare(image_info->magick,"CMYKA") != 0)
            {
              (void) ExportImagePixelArea(image,CMYKQuantum,quantum_size,pixels,0,0);
              (void) WriteBlob(image,packet_size*image->columns,pixels);
            }
          else
            {
              (void) ExportImagePixelArea(image,CMYKAQuantum,quantum_size,pixels,0,0);
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
          Line interlacing:  CCC...MMM...YYY...KKK...CCC...MMM...YYY...KKK...
        */
        for (y=0; y < (long) image->rows; y++)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          (void) ExportImagePixelArea(image,CyanQuantum,quantum_size,pixels,0,0);
          (void) WriteBlob(image,image->columns,pixels);
          (void) ExportImagePixelArea(image,MagentaQuantum,quantum_size,pixels,0,0);
          (void) WriteBlob(image,image->columns,pixels);
          (void) ExportImagePixelArea(image,YellowQuantum,quantum_size,pixels,0,0);
          (void) WriteBlob(image,image->columns,pixels);
          (void) ExportImagePixelArea(image,BlackQuantum,quantum_size,pixels,0,0);
          (void) WriteBlob(image,image->columns,pixels);
          if (LocaleCompare(image_info->magick,"CMYKA") == 0)
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
          Plane interlacing:  CCCCCC...MMMMMM...YYYYYY...KKKKKK...
        */
        if (image_info->interlace == PartitionInterlace)
          {
            AppendImageFormat("C",image->filename);
            status=
              OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
            if (status == False)
              ThrowWriterException(FileOpenError,UnableToOpenFile,image);
          }
        for (y=0; y < (long) image->rows; y++)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          (void) ExportImagePixelArea(image,CyanQuantum,quantum_size,pixels,0,0);
          (void) WriteBlob(image,image->columns,pixels);
        }
        if (image_info->interlace == PartitionInterlace)
          {
            CloseBlob(image);
            AppendImageFormat("M",image->filename);
            status=OpenBlob(image_info,image,WriteBinaryBlobMode,
              &image->exception);
            if (status == False)
              ThrowWriterException(FileOpenError,UnableToOpenFile,image);
          }
        if (!MagickMonitorFormatted(100,400,&image->exception,
                                    SaveImageText,image->filename))
          break;
        for (y=0; y < (long) image->rows; y++)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          (void) ExportImagePixelArea(image,MagentaQuantum,quantum_size,pixels,0,0);
          (void) WriteBlob(image,image->columns,pixels);
        }
        if (image_info->interlace == PartitionInterlace)
          {
            CloseBlob(image);
            AppendImageFormat("Y",image->filename);
            status=OpenBlob(image_info,image,WriteBinaryBlobMode,
              &image->exception);
            if (status == False)
              ThrowWriterException(FileOpenError,UnableToOpenFile,image);
          }
        if (!MagickMonitorFormatted(200,400,&image->exception,
                                    SaveImageText,image->filename))
          break;
        for (y=0; y < (long) image->rows; y++)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          (void) ExportImagePixelArea(image,YellowQuantum,quantum_size,pixels,0,0);
          (void) WriteBlob(image,image->columns,pixels);
        }
        if (image_info->interlace == PartitionInterlace)
          {
            CloseBlob(image);
            AppendImageFormat("K",image->filename);
            status=OpenBlob(image_info,image,WriteBinaryBlobMode,
              &image->exception);
            if (status == False)
              ThrowWriterException(FileOpenError,UnableToOpenFile,image);
          }
        if (!MagickMonitorFormatted(200,400,&image->exception,
                                    SaveImageText,image->filename))
          break;
        for (y=0; y < (long) image->rows; y++)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          (void) ExportImagePixelArea(image,BlackQuantum,quantum_size,pixels,0,0);
          (void) WriteBlob(image,image->columns,pixels);
        }
        if (LocaleCompare(image_info->magick,"CMYKA") == 0)
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
        if (!MagickMonitorFormatted(400,400,&image->exception,
                                    SaveImageText,image->filename))
          break;
        break;
      }
    }
    if (image->next == (Image *) NULL)
      break;
    image=SyncNextImageInList(image);
    status=MagickMonitorFormatted(scene++,GetImageListLength(image),
                                  &image->exception,SaveImagesText,
                                  image->filename);
    if (status == False)
      break;
  } while (image_info->adjoin);
  MagickFreeMemory(pixels);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseBlob(image);
  return(True);
}
