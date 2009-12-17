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
%                        U   U  Y   Y  V   V  Y   Y                           %
%                        U   U   Y Y   V   V   Y Y                            %
%                        U   U    Y    V   V    Y                             %
%                        U   U    Y     V V     Y                             %
%                         UUU     Y      V      Y                             %
%                                                                             %
%                                                                             %
%           Read/Write 16bit/pixel Interleaved YUV Image Format.              %
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
#include "magick/utility.h"

/*
  Forward declarations.
*/
static unsigned int
  WriteUYVYImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d U Y V Y I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadUYVYImage reads an image in the UYVY format and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadUYVYImage method is:
%
%      Image *ReadUYVYImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadUYVYImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadUYVYImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *image;

  long
    y;

  register long
    x;

  register PixelPacket
    *q;

  register long
    i;

  unsigned char
    u,
    v,
    y1,
    y2;

  unsigned int
    status;

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
  (void) strlcpy(image->filename,image_info->filename,MaxTextExtent);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    ThrowReaderException(FileOpenError,UnableToOpenFile,image);
  /*
    When subsampling, image width must be evenly divisible by two.
  */
  if (image->columns %2)
    ThrowReaderException(CorruptImageError,SubsamplingRequiresEvenWidth,image);
  for (i=0; i < image->offset; i++)
    {
      if (EOF == ReadBlobByte(image))
        ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,
                       image->filename);
    }
  image->depth=8;
  if (image_info->ping)
    {
      CloseBlob(image);
      return(image);
    }
  /*
    Accumulate UYVY, then unpack into two pixels.
  */
  for (y=0; y < (long) image->rows; y++)
  {
    q=SetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (long) (image->columns >> 1); x++)
    {
      u=ReadBlobByte(image);
      y1=ReadBlobByte(image);
      v=ReadBlobByte(image);
      y2=ReadBlobByte(image);
      q->red=ScaleCharToQuantum(y1);
      q->green=ScaleCharToQuantum(u);
      q->blue=ScaleCharToQuantum(v);
      q++;
      q->red=ScaleCharToQuantum(y2);
      q->green=ScaleCharToQuantum(u);
      q->blue=ScaleCharToQuantum(v);
      q++;
    }
    if (!SyncImagePixels(image))
      break;
    if (QuantumTick(y,image->rows))
      if (!MagickMonitorFormatted(y,image->rows,exception,LoadImageText,
                                  image->filename,
				  image->columns,image->rows))
        break;
  }
  image->colorspace=YCbCrColorspace;
  (void) TransformColorspace(image,RGBColorspace);
  if (EOFBlob(image))
    ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,
      image->filename);
  CloseBlob(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r U Y V Y I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterUYVYImage adds attributes for the UYVY image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterUYVYImage method is:
%
%      RegisterUYVYImage(void)
%
*/
ModuleExport void RegisterUYVYImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("PAL");
  entry->decoder=(DecoderHandler) ReadUYVYImage;
  entry->encoder=(EncoderHandler) WriteUYVYImage;
  entry->adjoin=False;
  entry->raw=True;
  entry->description="16bit/pixel interleaved YUV";
  entry->module="UYVY";
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("UYVY");
  entry->decoder=(DecoderHandler) ReadUYVYImage;
  entry->encoder=(EncoderHandler) WriteUYVYImage;
  entry->adjoin=False;
  entry->raw=True;
  entry->description="16bit/pixel interleaved YUV";
  entry->module="UYVY";
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r U Y V Y I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterUYVYImage removes format registrations made by the
%  UYVY module from the list of supported formats.
%
%  The format of the UnregisterUYVYImage method is:
%
%      UnregisterUYVYImage(void)
%
*/
ModuleExport void UnregisterUYVYImage(void)
{
  (void) UnregisterMagickInfo("PAL");
  (void) UnregisterMagickInfo("UYVY");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e U Y V Y I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteUYVYImage writes an image to a file in the digital UYVY
%  format.  This format, used by AccomWSD, is not dramatically higher quality
%  than the 12bit/pixel YUV format, but has better locality.
%
%  The format of the WriteUYVYImage method is:
%
%      unsigned int WriteUYVYImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteUYVYImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%      Implicit assumption: number of columns is even.
%
*/
static unsigned int WriteUYVYImage(const ImageInfo *image_info,Image *image)
{
  DoublePixelPacket
    pixel;

  long
    y;

  register const PixelPacket
    *p;

  register long
    x;

  unsigned int
    full,
    status;

  ColorspaceType
    original_colorspace;

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
    When subsampling, image width must be evenly divisible by two.
  */
  if (image->columns %2)
    ThrowWriterException(CoderError,SubsamplingRequiresEvenWidth,image);

  /*
    Convert to YUV, at full resolution.
  */
  original_colorspace=image->colorspace;
  (void) TransformColorspace(image,YCbCrColorspace);
  /*
    Accumulate two pixels, then output.
  */
  full=False;
  (void) memset(&pixel,0,sizeof(DoublePixelPacket));
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      if (full)
        {
          pixel.green=(pixel.green+p->green)/2;
          pixel.blue=(pixel.blue+p->blue)/2;
          (void) WriteBlobByte(image,ScaleQuantumToChar(pixel.green));
          (void) WriteBlobByte(image,ScaleQuantumToChar(pixel.red));
          (void) WriteBlobByte(image,ScaleQuantumToChar(pixel.blue));
          (void) WriteBlobByte(image,ScaleQuantumToChar(p->red));
        }
      pixel.red=p->red;
      pixel.green=p->green;
      pixel.blue=p->blue;
      full=!full;
      p++;
    }
    if (QuantumTick(y,image->rows))
      if (!MagickMonitorFormatted(y,image->rows,&image->exception,
                                  SaveImageText,image->filename,
				  image->columns,image->rows))
        break;
  }
  /*
    Restore colorspace
  */
  (void) TransformColorspace(image,original_colorspace);
  CloseBlob(image);
  return(True);
}
