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
%                                                                             %
%                              OOO   TTTTT  BBBB                              %
%                             O   O    T    B   B                             %
%                             O   O    T    BBBB                              %
%                             O   O    T    B   B                             %
%                              OOO     T    BBBB                              %
%                                                                             %
%                                                                             %
%                     Read/Write On-The-Air Image Format.                     %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                               January 2000                                  %
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
  WriteOTBImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d O T B I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadOTBImage reads a on-the-air (level 0) bitmap and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadOTBImage method is:
%
%      Image *ReadOTBImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadOTBImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadOTBImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
#define GetBit(a,i) (((a) >> (i)) & 1L)

  Image
    *image;

  int
    byte;

  long
    y;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *q;

  unsigned char
    bit,
    info,
    depth;

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
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    ThrowReaderException(FileOpenError,UnableToOpenFile,image);
  /*
    Initialize image structure.
  */
  info=ReadBlobByte(image);
  if (GetBit(info,4) == 0)
    {
      image->columns=ReadBlobByte(image);
      image->rows=ReadBlobByte(image);
    }
  else
    {
      image->columns=ReadBlobMSBShort(image);
      image->rows=ReadBlobMSBShort(image);
    }
  if ((image->columns == 0) || (image->rows == 0))
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
  depth=ReadBlobByte(image);
  if (depth != 1)
    ThrowReaderException(CoderError,OnlyLevelZerofilesSupported,image);
  if (!AllocateImageColormap(image,2))
    ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
  if (image_info->ping)
    {
      CloseBlob(image);
      return(image);
    }
  /*
    Convert bi-level image to pixel packets.
  */
  for (y=0; y < (long) image->rows; y++)
  {
    q=SetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=AccessMutableIndexes(image);
    bit=0;
    byte=0;
    for (x=0; x < (long) image->columns; x++)
    {
      if (bit == 0)
        {
          byte=ReadBlobByte(image);
          if (byte == EOF)
            ThrowReaderException(CorruptImageError,CorruptImage,image);
        }
      indexes[x]=(byte & (0x01 << (7-bit))) ? 0x01 : 0x00;
      bit++;
      if (bit == 8)
        bit=0;
    }
    if (!SyncImagePixels(image))
      break;
    if (QuantumTick(y,image->rows))
      if (!MagickMonitorFormatted(y,image->rows,exception,LoadImageText,
                                  image->filename))
        break;
  }
  (void) SyncImage(image);
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
%   R e g i s t e r O T B I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterOTBImage adds attributes for the OTB image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterOTBImage method is:
%
%      RegisterOTBImage(void)
%
*/
ModuleExport void RegisterOTBImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("OTB");
  entry->decoder=(DecoderHandler) ReadOTBImage;
  entry->encoder=(EncoderHandler) WriteOTBImage;
  entry->adjoin=False;
  entry->description="On-the-air bitmap";
  entry->module="OTB";
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r O T B I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterOTBImage removes format registrations made by the
%  OTB module from the list of supported formats.
%
%  The format of the UnregisterOTBImage method is:
%
%      UnregisterOTBImage(void)
%
*/
ModuleExport void UnregisterOTBImage(void)
{
  (void) UnregisterMagickInfo("OTB");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e O T B I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteOTBImage writes an image to a file in the On-the-air Bitmap
%  (level 0) image format.
%
%  The format of the WriteOTBImage method is:
%
%      unsigned int WriteOTBImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteOTBImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
static unsigned int WriteOTBImage(const ImageInfo *image_info,Image *image)
{
#define SetBit(a,i,set) \
  a=(unsigned char) ((set) ? (a) | (1L << (i)) : (a) & ~(1L << (i)))

  long
    y;

  register const PixelPacket
    *p;

  register const IndexPacket
    *indexes;

  register long
    x;

  unsigned char
    bit,
    byte,
    info,
    polarity;

  unsigned int
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
  (void) TransformColorspace(image,RGBColorspace);
  /*
    Convert image to a bi-level image.
  */
  (void) SetImageType(image,BilevelType);
  polarity=PixelIntensityToQuantum(&image->colormap[0]) < (MaxRGB/2);
  if (image->colors == 2)
    polarity=PixelIntensityToQuantum(&image->colormap[0]) <
      PixelIntensityToQuantum(&image->colormap[1]);
  info=0;
  if ((image->columns >= 256) || (image->rows >= 256))
    SetBit(info,4,1);
  (void) WriteBlobByte(image,info);
  if ((image->columns >= 256) || (image->rows >= 256))
    {
      (void) WriteBlobMSBShort(image,image->columns);
      (void) WriteBlobMSBShort(image,image->rows);
    }
  else
    {
      (void) WriteBlobByte(image,(long) image->columns);
      (void) WriteBlobByte(image,(long) image->rows);
    }
  (void) WriteBlobByte(image,1);  /* depth */
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    indexes=AccessImmutableIndexes(image);
    bit=0;
    byte=0;
    for (x=0; x < (long) image->columns; x++)
    {
      if (indexes[x] == polarity)
        byte|=0x1 << (7-bit);
      bit++;
      if (bit == 8)
        {
          (void) WriteBlobByte(image,byte);
          bit=0;
          byte=0;
        }
    }
    if (bit != 0)
      (void) WriteBlobByte(image,byte);
    if (QuantumTick(y,image->rows))
      if (!MagickMonitorFormatted(y,image->rows,&image->exception,
                                  SaveImageText,image->filename))
        break;
  }
  CloseBlob(image);
  return(True);
}
