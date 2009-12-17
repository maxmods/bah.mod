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
%                         W   W  BBBB   M   M  PPPP                           %
%                         W   W  B   B  MM MM  P   P                          %
%                         W W W  BBBB   M M M  PPPP                           %
%                         WW WW  B   B  M   M  P                              %
%                         W   W  BBBB   M   M  P                              %
%                                                                             %
%                                                                             %
%              Read/Write Wireless Bitmap (level 0) Image Format.             %
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
#include "magick/colormap.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/pixel_cache.h"
#include "magick/utility.h"

/*
  Forward declarations.
*/
static unsigned int
  WriteWBMPImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d W B M P I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadWBMPImage reads a WBMP (level 0) image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  ReadWBMPImage was contributed by Milan Votava <votava@mageo.cz>.
%
%  The format of the ReadWBMPImage method is:
%
%      Image *ReadWBMPImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadWBMPImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/

static unsigned int WBMPReadInteger(Image *image,unsigned long *value)
{
  int
    byte;

  *value=0;
  do
  {
    byte=ReadBlobByte(image);
    if (byte == EOF)
      return(False);
    *value<<=7;
    *value|=(unsigned int) (byte & 0x7f);
  } while (byte & 0x80);
  return(True);
}

static Image *ReadWBMPImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
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

  register long
    i;

  unsigned char
    bit;

  unsigned int
    status;

  unsigned short
    header;

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
  if (!ReadBlob(image,2,(char *) &header)) 
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
  if (header != 0U)
    ThrowReaderException(CoderError,OnlyLevelZerofilesSupported,image);
  /*
    Initialize image structure.
  */
  if (WBMPReadInteger(image,&image->columns) == False) 
    ThrowReaderException(CorruptImageError,CorruptImage,image);
  if (WBMPReadInteger(image,&image->rows) == False) 
    ThrowReaderException(CorruptImageError,CorruptImage,image);
  if ((image->columns == 0) || (image->rows == 0))
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
  for (i=0; i < image->offset; i++)
    {
      if (EOF == ReadBlobByte(image))
        ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,
                       image->filename);
    }
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
      indexes[x]=(byte & (0x01 << (7-bit))) ? 1 : 0;
      bit++;
      if (bit == 8)
        bit=0;
    }
    if (!SyncImagePixels(image))
      break;
    if (QuantumTick(y,image->rows))
      if (!MagickMonitorFormatted(y,image->rows,exception,LoadImageText,
                                  image->filename,
				  image->columns,image->rows))
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
%   R e g i s t e r W B M P I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterWBMPImage adds attributes for the WBMP image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterWBMPImage method is:
%
%      RegisterWBMPImage(void)
%
*/
ModuleExport void RegisterWBMPImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("WBMP");
  entry->decoder=(DecoderHandler) ReadWBMPImage;
  entry->encoder=(EncoderHandler) WriteWBMPImage;
  entry->adjoin=False;
  entry->description="Wireless Bitmap (level 0) image";
  entry->module="WBMP";
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r W B M P I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterWBMPImage removes format registrations made by the
%  WBMP module from the list of supported formats.
%
%  The format of the UnregisterWBMPImage method is:
%
%      UnregisterWBMPImage(void)
%
*/
ModuleExport void UnregisterWBMPImage(void)
{
  (void) UnregisterMagickInfo("WBMP");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e W B M P I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteWBMPImage writes an image to a file in the Wireless Bitmap
%  (level 0) image format.
%
%  WriteWBMPImage was contributed by Milan Votava <votava@mageo.cz>.
%
%  The format of the WriteWBMPImage method is:
%
%      unsigned int WriteWBMPImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteWBMPImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/

static void WBMPWriteInteger(Image *image,const unsigned long value)
{
  int
    bits,
    flag,
    n;

  register long
    i;

  unsigned char
    buffer[5],
    octet;

  n=1;
  bits=28;
  flag=False;
  for(i=4; i >= 0; i--)
  {
    octet=(unsigned char) ((value >> bits) & 0x7f);
    if (!flag && octet)
      {
        flag=True;
        n=i+1;
      }
    buffer[4-i]=octet | (i && (flag || octet))*(0x01 << 7);
    bits-=7;
  }
  (void) WriteBlob(image,n,(char *) buffer+5-n);
}

static unsigned int WriteWBMPImage(const ImageInfo *image_info,Image *image)
{
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
  (void) WriteBlobMSBShort(image,0);
  WBMPWriteInteger(image,image->columns);
  WBMPWriteInteger(image,image->rows);
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
                                  SaveImageText,image->filename,
				  image->columns,image->rows))
        break;
  }
  CloseBlob(image);
  return(True);
}
