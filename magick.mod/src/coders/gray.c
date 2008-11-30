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
%                         GGGG  RRRR    AAA   Y   Y                           %
%                        G      R   R  A   A   Y Y                            %
%                        G  GG  RRRR   AAAAA    Y                             %
%                        G   G  R R    A   A    Y                             %
%                         GGG   R  R   A   A    Y                             %
%                                                                             %
%                                                                             %
%                    Read/Write RAW Gray Image Format.                        %
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
  WriteGRAYImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d G R A Y I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadGRAYImage reads an image of raw grayscale samples and returns
%  it.  It allocates the memory necessary for the new Image structure and
%   returns a pointer to the new image.
%
%  The format of the ReadGRAYImage method is:
%
%      Image *ReadGRAYImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadGRAYImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadGRAYImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *image;

  long
    j,
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
    depth,
    quantum_size,
    packet_size;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  if ((image->columns == 0) || (image->rows == 0))
    ThrowReaderException(OptionError,MustSpecifyImageSize,image);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    ThrowReaderException(FileOpenError,UnableToOpenFile,image);
  for (i=0; i < image->offset; i++)
    {
      if (EOF == ReadBlobByte(image))
        ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,
                       image->filename);
    }
  /*
    Support depth in multiples of 8 bits.
  */
  if (image->depth > 16)
    depth=32;
  else if (image->depth > 8)
    depth=16;
  else
    depth=8;
  /*
    Allocate memory for a scanline.
  */
  if (depth <= 8)
    quantum_size=8;
  else if (depth <= 16)
    quantum_size=16;
  else
    quantum_size=32;

  packet_size=quantum_size/8;
  scanline=MagickAllocateMemory(unsigned char *,packet_size*image->tile_info.width);
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
    for (y=0; y < image->tile_info.y; y++)
      (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
    for (y=0; y < (long) image->rows; y++)
    {
      if ((y > 0) || (image->previous == (Image *) NULL))
        (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
      q=SetImagePixels(image,0,y,image->columns,1);
      if (q == (PixelPacket *) NULL)
        break;
      (void) ImportImagePixelArea(image,GrayQuantum,quantum_size,scanline+x,0,0);
      if (!SyncImagePixels(image))
        break;
      if (image->previous == (Image *) NULL)
        if (QuantumTick(y,image->rows))
          if (!MagickMonitorFormatted(y,image->rows,exception,
                                      LoadImageText,image->filename))
            break;
    }
    image->is_grayscale=MagickTrue;
    count=image->tile_info.height-image->rows-image->tile_info.y;
    for (j=0; j < (long) count; j++)
      (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
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
        if (!MagickMonitorFormatted(TellBlob(image),GetBlobSize(image),exception,
                                    LoadImagesText,image->filename))
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
%   R e g i s t e r G R A Y I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterGRAYImage adds attributes for the GRAY image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterGRAYImage method is:
%
%      RegisterGRAYImage(void)
%
*/
ModuleExport void RegisterGRAYImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("GRAY");
  entry->decoder=(DecoderHandler) ReadGRAYImage;
  entry->encoder=(EncoderHandler) WriteGRAYImage;
  entry->raw=True;
  entry->description="Raw gray samples";
  entry->module="GRAY";
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("R");
  entry->decoder=(DecoderHandler) ReadGRAYImage;
  entry->encoder=(EncoderHandler) WriteGRAYImage;
  entry->stealth=True;
  entry->raw=True;
  entry->description="Raw red samples";
  entry->module="GRAY";
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("C");
  entry->decoder=(DecoderHandler) ReadGRAYImage;
  entry->encoder=(EncoderHandler) WriteGRAYImage;
  entry->stealth=True;
  entry->raw=True;
  entry->description="Raw cyan samples";
  entry->module="GRAY";
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("G");
  entry->decoder=(DecoderHandler) ReadGRAYImage;
  entry->encoder=(EncoderHandler) WriteGRAYImage;
  entry->stealth=True;
  entry->raw=True;
  entry->description="Raw green samples";
  entry->module="GRAY";
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("M");
  entry->decoder=(DecoderHandler) ReadGRAYImage;
  entry->encoder=(EncoderHandler) WriteGRAYImage;
  entry->stealth=True;
  entry->raw=True;
  entry->description="Raw magenta samples";
  entry->module="GRAY";

  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("B");
  entry->decoder=(DecoderHandler) ReadGRAYImage;
  entry->encoder=(EncoderHandler) WriteGRAYImage;
  entry->stealth=True;
  entry->raw=True;
  entry->description="Raw blue samples";
  entry->module="GRAY";
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("Y");
  entry->decoder=(DecoderHandler) ReadGRAYImage;
  entry->encoder=(EncoderHandler) WriteGRAYImage;
  entry->stealth=True;
  entry->raw=True;
  entry->description="Raw yellow samples";
  entry->module="GRAY";
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("O");
  entry->decoder=(DecoderHandler) ReadGRAYImage;
  entry->encoder=(EncoderHandler) WriteGRAYImage;
  entry->stealth=True;
  entry->raw=True;
  entry->description="Raw opacity samples";
  entry->module="GRAY";
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("K");
  entry->decoder=(DecoderHandler) ReadGRAYImage;
  entry->encoder=(EncoderHandler) WriteGRAYImage;
  entry->stealth=True;
  entry->raw=True;
  entry->description="Raw black samples";
  entry->module="GRAY";
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r G R A Y I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterGRAYImage removes format registrations made by the
%  GRAY module from the list of supported formats.
%
%  The format of the UnregisterGRAYImage method is:
%
%      UnregisterGRAYImage(void)
%
*/
ModuleExport void UnregisterGRAYImage(void)
{
  (void) UnregisterMagickInfo("GRAY");
  (void) UnregisterMagickInfo("R");
  (void) UnregisterMagickInfo("C");
  (void) UnregisterMagickInfo("G");
  (void) UnregisterMagickInfo("M");
  (void) UnregisterMagickInfo("B");
  (void) UnregisterMagickInfo("Y");
  (void) UnregisterMagickInfo("O");
  (void) UnregisterMagickInfo("K");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e G R A Y I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteGRAYImage writes an image to a file as gray scale intensity
%  values.
%
%  The format of the WriteGRAYImage method is:
%
%      unsigned int WriteGRAYImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteGRAYImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
static unsigned int WriteGRAYImage(const ImageInfo *image_info,Image *image)
{
  int
    y;

  register const PixelPacket
    *p;

  unsigned char
    *scanline;

  unsigned int
    depth,
    quantum_size,
    packet_size,
    scene,
    status;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenError,UnableToOpenFile,image);
  /*
    Support depth in multiples of 8 bits.
  */
  if (image->depth > 16)
    depth=32;
  else if (image->depth > 8)
    depth=16;
  else
    depth=8;
  /*
    Convert image to gray scale PseudoColor class.
  */
  scene=0;
  do
  {
    /*
      Allocate memory for scanline.
    */
    if (depth <= 8)
      quantum_size=8;
    else if (depth <= 16)
      quantum_size=16;
    else
      quantum_size=32;
    (void) TransformColorspace(image,RGBColorspace);
    packet_size=quantum_size/8;
    scanline=MagickAllocateMemory(unsigned char *,packet_size*image->columns);
    if (scanline == (unsigned char *) NULL)
      ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
    /*
      Convert MIFF to GRAY raster scanline.
    */
    for (y=0; y < (long) image->rows; y++)
    {
      p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
      if (p == (const PixelPacket *) NULL)
        break;
      (void) ExportImagePixelArea(image,GrayQuantum,quantum_size,scanline,0,0);
      (void) WriteBlob(image,packet_size*image->columns,scanline);
      if (image->previous == (Image *) NULL)
        if (QuantumTick(y,image->rows))
          if (!MagickMonitorFormatted(y,image->rows,&image->exception,
                                      SaveImageText,image->filename))
            break;
    }
    MagickFreeMemory(scanline);
    if (image->next == (Image *) NULL)
      break;
    image=SyncNextImageInList(image);
    if (!MagickMonitorFormatted(scene++,GetImageListLength(image),&image->exception,
                                SaveImagesText,image->filename))
      break;
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseBlob(image);
  return(True);
}
