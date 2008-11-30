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
%                            PPPP   SSSSS  33333                              %
%                            P   P  SS        33                              %
%                            PPPP    SSS    333                               %
%                            P         SS     33                              %
%                            P      SSSSS  33333                              %
%                                                                             %
%                                                                             %
%                    Write Postscript Level III Format.                       %
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
#include "magick/attribute.h"
#include "magick/blob.h"
#include "magick/pixel_cache.h"
#include "magick/channel.h"
#include "magick/color.h"
#include "magick/compress.h"
#include "magick/constitute.h"
#include "magick/magick.h"
#include "magick/map.h"
#include "magick/monitor.h"
#include "magick/tempfile.h"
#include "magick/utility.h"
#include "magick/version.h"
#if defined(HasTIFF)
#define CCITTParam  "-1"
#else
#define CCITTParam  "0"
#endif

/* For consistent PS and C code usage */
#define PS3_NoCompression "0"
#define PS3_FaxCompression "1"
#define PS3_RLECompression "2"
#define PS3_LZWCompression "3"
#define PS3_ZipCompression "4"
#define PS3_JPEGCompression "5"

#define PS3_RGBColorspace "0"
#define PS3_CMYKColorspace "1"

#define PS3_DirectClass "0"
#define PS3_PseudoClass "1"

/*
  Forward declarations.
*/
static unsigned int
  WritePS3Image(const ImageInfo *,Image *),
  ZLIBEncode2Image(Image *,const size_t,const unsigned long,unsigned char *,
    WriteByteHook,void *);

#if defined(HasTIFF)
#if defined(HAVE_TIFFCONF_H)
#include "tiffconf.h"
#endif
#include "tiffio.h"
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   H u f f m a n 2 D E n c o d e I m a g e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Huffman2DEncodeImage compresses an image via two-dimensional
%  Huffman-coding.
%
%  The format of the Huffman2DEncodeImage method is:
%
%      unsigned int Huffman2DEncodeImage(const ImageInfo *image_info,
%        Image *image)
%
%  A description of each parameter follows:
%
%    o status:  Method Huffman2DEncodeImage returns True if all the pixels are
%      compressed without error, otherwise False.
%
%    o image_info: The image info..
%
%    o image: The image.
%
*/
static unsigned int SerializeHuffman2DImage(const ImageInfo *image_info,
  Image *image, unsigned char **pixels, size_t *length)
{
  char
    filename[MaxTextExtent];

  Image
    *huffman_image;

  ImageInfo
    *clone_info;

  long
    count,
    j;

  register long
    i;

  TIFF
    *tiff;

  uint16
    fillorder;

  unsigned char
    *buffer,
    *p;

  unsigned int
    status;

  unsigned long
    *byte_count,
    strip_size;

  /*
    Write image as CCITTFax4 TIFF image to a temporary file.
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if(!AcquireTemporaryFileName(filename))
    {
      ThrowBinaryException(FileOpenError,UnableToCreateTemporaryFile,
        filename)
    }
  huffman_image=CloneImage(image,0,0,True,&image->exception);
  if (huffman_image == (Image *) NULL)
    return(False);
  /* 
    TODO: If (image, then) huffman_image->compression is JPEG, huffman_image
    is changed to DirectClass in WriteTIFFImage and the huffman_image ends up
    broken. Change WriteTIFFIMage to not alter bilevel image to directclass
    when clone_info is G4 compression. Until then, set
    huffman_image->compression here:
  */
  huffman_image->compression=Group4Compression;

  (void) SetImageType(huffman_image,BilevelType);
  FormatString(huffman_image->filename,"tiff:%s",filename);
  clone_info=CloneImageInfo(image_info);
  clone_info->compression=Group4Compression;
  status=WriteImage(clone_info,huffman_image);
  DestroyImageInfo(clone_info);
  DestroyImage(huffman_image);
  if (status == False)
    {
      (void) LiberateTemporaryFile(filename);
      return(False);
    }
  tiff=TIFFOpen(filename,"rb");
  if (tiff == (TIFF *) NULL)
    {
      (void) LiberateTemporaryFile(filename);
      ThrowBinaryException(FileOpenError,UnableToOpenFile,
        image_info->filename)
    }
  /*
    Allocate raw strip buffer.
  */
  (void) TIFFGetField(tiff,TIFFTAG_STRIPBYTECOUNTS,&byte_count);
  *length=strip_size=byte_count[0];
  for (i=1; i < (long) TIFFNumberOfStrips(tiff); i++)
  {
    if (byte_count[i] > strip_size)
      strip_size=byte_count[i];
    *length+=byte_count[i];
  }
  buffer=MagickAllocateMemory(unsigned char *,strip_size);
  if (buffer == (unsigned char *) NULL)
    {
      TIFFClose(tiff);
      (void) LiberateTemporaryFile(filename);
      ThrowBinaryException(ResourceLimitError,MemoryAllocationFailed,
        (char *) NULL)
    }
  *pixels=MagickAllocateMemory(unsigned char *,*length);
  if (*pixels == (unsigned char *) NULL)
    {
      MagickFreeMemory(buffer);
      TIFFClose(tiff);
      (void) LiberateTemporaryFile(filename);
      ThrowBinaryException(ResourceLimitError,MemoryAllocationFailed,
        (char *) NULL)
    }

  (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_FILLORDER,&fillorder);
  p=(*pixels);
  for (i=0; i < (long) TIFFNumberOfStrips(tiff); i++)
  {
    count=TIFFReadRawStrip(tiff,(uint32) i,buffer,(long) byte_count[i]);
    if (fillorder == FILLORDER_LSB2MSB)
      TIFFReverseBits(buffer,count);
    for (j=0; j < count; j++)
      *p++=buffer[j];
  }
  MagickFreeMemory(buffer);
  TIFFClose(tiff);
  (void) LiberateTemporaryFile(filename);
  return(True);
}

static unsigned int Huffman2DEncodeImage(const ImageInfo *image_info,
  Image *image)
{
  size_t
    length;

  register unsigned long
    i;

  unsigned char
    *pixels;

  unsigned int
    status;

  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  status=SerializeHuffman2DImage(image_info,image,&pixels,&length);
  if (!status)
    return False;

  Ascii85Initialize(image);
  for (i=0; i < length; i++)
    Ascii85Encode(image,(unsigned long) pixels[i]);
  Ascii85Flush(image);
  MagickFreeMemory(pixels);
  return(True);
}

#else

static unsigned int SerializeHuffman2DImage(const ImageInfo *image_info,
  Image *image, unsigned char **pixels, size_t *length)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  ThrowBinaryException(MissingDelegateError,TIFFLibraryIsNotAvailable,image->filename)
}
static unsigned int Huffman2DEncodeImage(const ImageInfo *image_info,
  Image *image)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  ThrowBinaryException(MissingDelegateError,TIFFLibraryIsNotAvailable,image->filename)
}
#endif

#if defined(HasJPEG)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   J P E G E n c o d e I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method JPEGEncodeImage compresses an image via JPEG compression.
%
%  The format of the JPEGEncodeImage method is:
%
%      unsigned int JPEGEncodeImage(const ImageInfo *image_info,
%        Image *image)
%
%  A description of each parameter follows:
%
%    o status:  Method JPEGEncodeImage returns True if all the pixels are
%      compressed without error, otherwise False.
%
%    o image_info: The image info.
%
%    o image: The image.
%
*/
static unsigned int JPEGEncodeImage(const ImageInfo *image_info,
  Image *image)
{
  Image
    *jpeg_image;

  size_t
    i,
    length;

  register const unsigned char
    *p;

  void
    *blob;

  /*
    Write image as JPEG image to an in-memory BLOB
  */
  jpeg_image=CloneImage(image,0,0,True,&image->exception);
  if (jpeg_image == (Image *) NULL)
    ThrowWriterException2(CoderError,image->exception.reason,image);
  (void) strcpy(jpeg_image->magick,"JPEG");
  blob=ImageToBlob(image_info,jpeg_image,&length,&image->exception);
  if (blob == (void *) NULL)
    ThrowWriterException2(CoderError,image->exception.reason,image);

  Ascii85Initialize(image);
  for (p=(const unsigned char*) blob,i=0; i < length; i++)
    Ascii85Encode(image,(unsigned long) p[i]);
  Ascii85Flush(image);
  DestroyImage(jpeg_image);
  MagickFreeMemory(blob);
  return(True);
}
#else
static unsigned int JPEGEncodeImage(const ImageInfo *image_info,
  Image *image)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  ThrowBinaryException(MissingDelegateError,JPEGLibraryIsNotAvailable,image->filename)
  return(False);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r P S 3 I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterPS3Image adds attributes for the PS3 image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterPS3Image method is:
%
%      RegisterPS3Image(void)
%
*/
ModuleExport void RegisterPS3Image(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("EPS3");
  entry->encoder=(EncoderHandler) WritePS3Image;
  entry->description="Adobe Level III Encapsulated PostScript";
  entry->module="PS3";
  entry->coder_class=PrimaryCoderClass;
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("PS3");
  entry->encoder=(EncoderHandler) WritePS3Image;
  entry->description="Adobe Level III PostScript";
  entry->module="PS3";
  entry->coder_class=PrimaryCoderClass;
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e r i a l i z e P s e u d o C l a s s I m a g e                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Convert the indexes of a color mapped image to a stream of bytes.
%
%  The format of the SerializePseudoClassImage method is:
%
%       unsigned int SerializePseudoClassImage(const ImageInfo *image_info,
%         Image *image, unsigned char **pixels, size_t *length)
%
%  A description of each parameter follows:
%
%    o image_info: Specifies a pointer to a ImageInfo structure. required by
%                  exception handlers.
%
%    o image: The image for which the color map indexes should be serialized.
%
%    o pixels: the serialized indexes.
%
%    o length: the length of the pixels mamory area.
%
*/
static unsigned int SerializePseudoClassImage(const ImageInfo *image_info,
  Image *image, unsigned char **pixels, size_t *length)
{
  long
    x,
    y;

  register unsigned char
    *q;

  const PixelPacket
    *p;

  int
    status;

  register const IndexPacket
    *indexes;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=True;
  *length=image->columns*image->rows;
  *pixels=MagickAllocateMemory(unsigned char *, *length);
  if (*pixels == (unsigned char *) NULL)
    ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
  q=(*pixels);
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    indexes=AccessImmutableIndexes(image);
    for (x=0; x < (long) image->columns; x++)
      *q++=indexes[x];
    if (image->previous == (Image *) NULL)
      if (QuantumTick(y,image->rows))
        {
          status=MagickMonitorFormatted(y,image->rows,&image->exception,
                                        SaveImageText,image->filename);
          if (status == False)
            break;
        }
  }
  if (status == False)
    MagickFreeMemory(*pixels);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e r i a l i z e M u l t i C h a n n e l I m a g e                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Convert three and four channel images to a stream of bytes.
%
%  The format of the SerializeMultiChannelImage method is:
%
%       unsigned int SerializeMultiChannelImage(const ImageInfo *image_info,
%         Image *image, unsigned char **pixels, size_t *length)
%
%  A description of each parameter follows:
%
%    o image_info: Specifies a pointer to a ImageInfo structure. required by
%                  exception handlers.
%
%    o image: The image for which the RGB or CMYK channels should be
%             serialized.
%
%    o pixels: the serialized image channels.
%
%    o length: the length of the pixels mamory area.
%
*/
static unsigned int SerializeMultiChannelImage(const ImageInfo *image_info,
  Image *image, unsigned char **pixels, size_t *length)
{
  long
    x,
    y;

  register unsigned char
    *q;

  const PixelPacket
    *p;

  int
    status;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=True;
  *length=(image->colorspace == CMYKColorspace ? 4 : 3)*image->columns*image->rows;
  *pixels=MagickAllocateMemory(unsigned char *, *length);
  if (*pixels == (unsigned char *) NULL)
    ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);

  q=(*pixels);
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    if (image->colorspace == CMYKColorspace)
      for (x=0; x < (long) image->columns; x++)
      {
        *q++=ScaleQuantumToChar(p->red);
        *q++=ScaleQuantumToChar(p->green);
        *q++=ScaleQuantumToChar(p->blue);
        *q++=ScaleQuantumToChar(p->opacity);
        p++;
      }
    else
      for (x=0; x < (long) image->columns; x++)
      {
        *q++=ScaleQuantumToChar(p->red);
        *q++=ScaleQuantumToChar(p->green);
        *q++=ScaleQuantumToChar(p->blue);
        p++;
      }
    if (image->previous == (Image *) NULL)
      if (QuantumTick(y,image->rows))
        {
          status=MagickMonitorFormatted(y,image->rows,&image->exception,
                                        SaveImageText,image->filename);
          if (status == False)
            break;
        }
  }
  if (status == False)
    MagickFreeMemory(*pixels);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e r i a l i z e S i n g l e C h a n n e l I m a g e                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Convert 1 and 8 bit single channel images to a stream of bytes. Bilevel
%  image pixels are packed 8 pixels in a byte.
%
%  The format of the SerializeSingleChannelImage method is:
%
%       unsigned int SerializeSingleChannelImage(const ImageInfo *image_info,
%         Image *image, unsigned char **pixels, size_t *length)
%
%  A description of each parameter follows:
%
%    o image_info: Specifies a pointer to an ImageInfo structure. Required by
%                  exception handlers.
%
%    o image: The image for which the RGB or CMYK channels should be
%             serialized.
%
%    o characteristics: Already populated image characteristics.
%
%    o pixels: the serialized image channels.
%
%    o length: the length of the pixels mamory area.
%
*/
static unsigned int SerializeSingleChannelImage(const ImageInfo *image_info,
  Image *image,unsigned char **pixels, size_t *length)
{
  unsigned long
    x,
    y;

  register unsigned char
    *q;

  const PixelPacket
    *p;

  unsigned long
    pack,
    padded_columns;

  unsigned char
    code,
    bit;

  int
    status;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=True;
  pack=IsMonochromeImage(image,&image->exception) ? 8 : 1;
  /* Padded columns are padded to byte boundary */
  padded_columns=((image->columns+pack-1)/pack)*pack;
  *length=padded_columns*image->rows/pack;
  *pixels=MagickAllocateMemory(unsigned char *, *length);
  if (*pixels == (unsigned char *) NULL)
    ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,
      image);

  q=(*pixels);
  for (y=0; y < image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    if (pack == 1)
    {
      for (x=0; x < image->columns; x++)
      {
        *q++=ScaleQuantumToChar(PixelIntensityToQuantum(p));
        p++;
      }
    }
    else
    {
      code=0;
      for (x=0; x < padded_columns; x++)
      {
        bit=0x00;
        if (x < image->columns)
          bit=PixelIntensityToQuantum(p) == TransparentOpacity ? 0x01 : 0x00;
        code=(code << 1)+bit;
        if ((x+1) % pack == 0)
        {
          *q++=code;
          code=0;
        }
        p++;
      }
    }
    if (image->previous == (Image *) NULL)
      if (QuantumTick(y,image->rows))
        {
          status=MagickMonitorFormatted(y,image->rows,
                                        &image->exception,SaveImageText,
                                        image->filename);
          if (status == False)
            break;
        }
  }
  if (status == False)
    MagickFreeMemory(*pixels);
  return(status);
}
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r P S 3 I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterPS3Image removes format registrations made by the
%  PS3 module from the list of supported formats.
%
%  The format of the UnregisterPS3Image method is:
%
%      UnregisterPS3Image(void)
%
*/
ModuleExport void UnregisterPS3Image(void)
{
  (void) UnregisterMagickInfo("EPS3");
  (void) UnregisterMagickInfo("PS3");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P S 3 M a s k I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePS3ImageMask writes the alpha channel of an image as an
%  Encapsulated Postscript Level III image mask. The mask is written as a
%  bilevel image, converted from the grayscale alpha channel using dithering
%  options supplied in image_info.
%
%  The format of the WritePS3MaskImage method is:
%
%      unsigned int WritePS3MaskImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows:
%
%    o status: Method WritePS3MaskImage return True if the mask is printed.
%      False is returned on errors like out-of-memory.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image: The address of a structure of type Image.
%
%
*/
typedef struct WriteByteHookInfo_
{
  Image
    *image;
} WriteByteHookInfo;

static unsigned int MaskWriteByteHook(Image *ARGUNUSED(mask_image),
  const magick_uint8_t code, void *info)
{
  Ascii85Encode(((WriteByteHookInfo *)info)->image, (unsigned long)code);
  return(True);
}

static unsigned int WritePS3MaskImage(const ImageInfo *image_info,Image *image)
{
  char
    buffer[MaxTextExtent];

  Image
    *mask_image;

  size_t
    i,
    length;

  CompressionType
    compression;

  unsigned char
    *pixels;

  int
    status;

  ExtendedSignedIntegralType
    start,
    stop;

  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->matte);
  status=True;
  compression=image->compression;
  if (image_info->compression != UndefinedCompression)
    compression=image_info->compression;

  /* Keep position of BeginData DSC comment for update later */
  start=TellBlob(image);
  FormatString(buffer,"%%%%BeginData:%13ld ASCII Bytes\n",0L);
  (void) WriteBlobString(image,buffer);
  stop=TellBlob(image);

  /* Only lossless compressions for the mask */
  switch (compression)
  {
    case NoCompression:
      FormatString(buffer,
        "currentfile %lu %lu "PS3_NoCompression" ByteStreamDecodeFilter\n",
        image->columns, image->rows);
    case FaxCompression:
    default:
      FormatString(buffer,
        "currentfile %lu %lu "PS3_FaxCompression" ByteStreamDecodeFilter\n",
        image->columns, image->rows);
      break;
    case RLECompression:
      FormatString(buffer,
        "currentfile %lu %lu "PS3_RLECompression" ByteStreamDecodeFilter\n",
        image->columns, image->rows);
      break;
    case LZWCompression:
      FormatString(buffer,
        "currentfile %lu %lu "PS3_LZWCompression" ByteStreamDecodeFilter\n",
        image->columns, image->rows);
      break;
    case ZipCompression:
      FormatString(buffer,
        "currentfile %lu %lu "PS3_ZipCompression" ByteStreamDecodeFilter\n",
        image->columns, image->rows);
      break;
  }
  (void)WriteBlobString(image, buffer);
  (void)WriteBlobString(image, "/ReusableStreamDecode filter\n");

  mask_image=CloneImage(image,0,0,True,&image->exception);
  if (mask_image == (Image *) NULL)
    ThrowWriterException2(CoderError,image->exception.reason,image);
  status=ChannelImage(mask_image, OpacityChannel);
  if (!status)
  {
    DestroyImage(mask_image);
    return(False);
  }
  (void) SetImageType(mask_image, BilevelType);
  mask_image->matte=False;

  /* Only lossless compressions for the mask */
  switch (compression)
  {
    case NoCompression:
      status=SerializeSingleChannelImage(image_info,mask_image,&pixels,&length);
      if (status)
      {
        Ascii85Initialize(image);
        for (i=0; i < length; i++)
          Ascii85Encode(image, (unsigned long)pixels[i]);
        Ascii85Flush(image);
        MagickFreeMemory(pixels);
      }
      break;
    case FaxCompression:
    default:
      if (LocaleCompare(CCITTParam,"0") == 0)
      {
        WriteByteHookInfo
          info;
        info.image=image;
        /* Read bytes from mask and write them ASCII85 encoded to image */
        Ascii85Initialize(image);
        status=HuffmanEncode2Image(image_info,mask_image,MaskWriteByteHook,(void *)&info);
        Ascii85Flush(image);
      }
      else
      {
        status=SerializeHuffman2DImage(image_info,mask_image,&pixels,&length);
        if (status)
        {
          Ascii85Initialize(image);
          for (i=0; i < length; i++)
            Ascii85Encode(image,(unsigned long) pixels[i]);
          Ascii85Flush(image);
          MagickFreeMemory(pixels);
        }
      }
      break;
    case RLECompression:
      status=SerializeSingleChannelImage(image_info,mask_image,&pixels,&length);
      if (status)
      {
        Ascii85Initialize(image);
        status=PackbitsEncode2Image(image,length,pixels,
          Ascii85WriteByteHook,(void *)NULL);
        Ascii85Flush(image);
        MagickFreeMemory(pixels);
      }
      break;
    case LZWCompression:
      status=SerializeSingleChannelImage(image_info,mask_image,&pixels,&length);
      if (status)
      {
        Ascii85Initialize(image);
        status=LZWEncode2Image(image,length,pixels,Ascii85WriteByteHook,(void*)NULL);
        Ascii85Flush(image);
        MagickFreeMemory(pixels);
      }
      break;
    case ZipCompression:
      status=SerializeSingleChannelImage(image_info,mask_image,&pixels,&length);
      if (status)
      {
        Ascii85Initialize(image);
        status=ZLIBEncode2Image(image,length,image_info->quality,pixels,
          Ascii85WriteByteHook,(void*)NULL);
        Ascii85Flush(image);
        MagickFreeMemory(pixels);
      }
      break;
  }
  DestroyImage(mask_image);
  length=TellBlob(image)-stop;
  stop=TellBlob(image);
  (void) SeekBlob(image,start,SEEK_SET);
  FormatString(buffer,"%%%%BeginData:%13ld ASCII Bytes\n",(long) length);
  (void) WriteBlobString(image,buffer);
  (void) SeekBlob(image,stop,SEEK_SET);
  (void) WriteBlobString(image,"%%EndData\n");
  (void)WriteBlobString(image, "/mask_stream exch def\n");
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P S 3 I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePS3Image translates an image to encapsulated Postscript
%  Level III for printing.  If the supplied geometry is null, the image is
%  centered on the Postscript page.  Otherwise, the image is positioned as
%  specified by the geometry.
%
%  The format of the WritePS3Image method is:
%
%      unsigned int WritePS3Image(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows:
%
%    o status: Method WritePS3Image return True if the image is printed.
%      False is returned if the image file cannot be opened for printing.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
static unsigned int WritePS3Image(const ImageInfo *image_info,Image *image)
{
  static const char
    *PostscriptProlog[]=
    {
      "/ByteStreamDecodeFilter",
      "{",
      "  /z exch def",
      "  /r exch def",
      "  /c exch def",
      "  /ASCII85Decode filter",
      "  z "PS3_FaxCompression" eq",
      "  {",
      "    <<",
      "      /K "CCITTParam,
      "      /Columns c",
      "      /Rows r",
      "    >>",
      "    /CCITTFaxDecode filter",
      "  } if",
      "  z "PS3_RLECompression" eq {/RunLengthDecode filter} if",
      "  z "PS3_LZWCompression" eq {/LZWDecode filter} if",
      "  z "PS3_ZipCompression" eq {/FlateDecode filter} if",
      "  z "PS3_JPEGCompression" eq {/DCTDecode filter} if",
      "} bind def",
      "",
      "/DirectClassImageDict",
      "{",
      "  colorspace "PS3_RGBColorspace" eq",
      "  {",
      "    /DeviceRGB setcolorspace",
      "    <<",
      "      /ImageType 1",
      "      /Width columns",
      "      /Height rows",
      "      /BitsPerComponent 8",
      "      /DataSource pixel_stream",
      "      /MultipleDataSources false",
      "      /ImageMatrix [columns 0 0 rows neg 0 rows]",
      "      /Decode [0 1 0 1 0 1]",
      "    >>",
      "  }",
      "  {",
      "    /DeviceCMYK setcolorspace",
      "    <<",
      "      /ImageType 1",
      "      /Width columns",
      "      /Height rows",
      "      /BitsPerComponent 8",
      "      /DataSource pixel_stream",
      "      /MultipleDataSources false",
      "      /ImageMatrix [columns 0 0 rows neg 0 rows]",
      "      /Decode",
      /*
        JPEG coder is used for JPEG compression. It compensates
        for inverted CMYK, so use an inverted decode matrix for
        JPEG compressed CMYK images.
      */
      "        compression "PS3_JPEGCompression" eq",
      "        {[1 0 1 0 1 0 1 0]}",
      "        {[0 1 0 1 0 1 0 1]}",
      "        ifelse",
      "    >>",
      "  }",
      "  ifelse",
      "} bind def",
      "",
      "/PseudoClassImageDict",
      "{",
      "  % COLORS IN PSEUDO CLASS IMAGE",
      "  currentfile buffer readline pop",
      "  token pop /colors exch def pop",
      "  colors 0 eq",
      "  {",
      "    % DEPTH OF GRAYSCALE",
      "    currentfile buffer readline pop",
      "    token pop /bits exch def pop",
      "    /DeviceGray setcolorspace",
      "    <<",
      "      /ImageType 1",
      "      /Width columns",
      "      /Height rows",
      "      /BitsPerComponent bits",
      "      /Decode [0 1]",
      "      /ImageMatrix [columns 0 0 rows neg 0 rows]",
      "      /DataSource pixel_stream",
      "    >>",
      "  }",
      "  {",
      "    % RGB COLORMAP",
      "    /colormap colors 3 mul string def",
      "    % INDEXES",
      "    currentfile /ASCII85Decode filter colormap readstring pop pop",
      "    [ /Indexed /DeviceRGB colors 1 sub colormap ] setcolorspace",
      "    <<",
      "      /ImageType 1",
      "      /Width columns",
      "      /Height rows",
      "      /BitsPerComponent 8",
      "      /Decode [0 255]",
      "      /ImageMatrix [columns 0 0 rows neg 0 rows]",
      "      /DataSource pixel_stream",
      "    >>",
      "  }",
      "  ifelse",
      "} bind def",
      "",
      "/NonMaskedImageDict",
      "{",
      "  class "PS3_PseudoClass" eq",
      "  { PseudoClassImageDict }",
      "  { DirectClassImageDict }",
      "  ifelse",
      "} bind def",
      "",
      "/MaskedImageDict",
      "{",
      "  <<",
      "    /ImageType 3",
      "    /InterleaveType 3",
      "    /DataDict NonMaskedImageDict",
      "    /MaskDict",
      "    <<",
      "      /ImageType 1",
      "      /Width columns",
      "      /Height rows",
      "      /BitsPerComponent 1",
      "      /DataSource mask_stream",
      "      /MultipleDataSources false",
      "      /ImageMatrix [ columns 0 0 rows neg 0 rows]",
      "      /Decode [ 0 1 ]",
      "    >>",
      "  >>",
      "} bind def",
      "",
      /*
        Default procedure for image clipping does nothing. Image
        will provide overriding ClipPath procdure if relevant.
      */
      "/ClipImage",
      "{} def",
      "",
      "/DisplayImage",
      "{",
      "  /buffer 512 string def",
      "",
      "  % TRANSLATION",
      "  currentfile buffer readline pop",
      "  token pop /x exch def",
      "  token pop /y exch def pop",
      "  x y translate",
      "",
      "  % IMAGE SIZE AND FONT SIZE",
      "  currentfile buffer readline pop",
      "  token pop /x exch def",
      "  token pop /y exch def pop",
      "  currentfile buffer readline pop",
      "  token pop /pointsize exch def pop",
      (char *) NULL
    },
    /*
      This hole in the PS prolog is for labels.
    */
    *PostscriptEpilog[]=
    {
      "  x y scale",
      "",
      "  % CLIPPING PATH",
      "  currentfile buffer readline pop",
      "  token pop /clipped exch def pop",
      "",
      "  % EPS",
      "  currentfile buffer readline pop",
      "  token pop /sp exch def pop",
      "",
      "  % IMAGE PIXEL SIZE",
      "  currentfile buffer readline pop",
      "  token pop /columns exch def",
      "  token pop /rows exch def pop",
      "",
      "  % COLORSPACE (RGB/CMYK)",
      "  currentfile buffer readline pop",
      "  token pop /colorspace exch def pop",
      "",
      "  % TRANSPARENCY",
      "  currentfile buffer readline pop",
      "  token pop /alpha exch def pop",
      "",
      "  % STENCIL MASK?",
      "  currentfile buffer readline pop",
      "  token pop /stencil exch def pop",
      "",
      "  % IMAGE CLASS (DIRECT/PSEUDO)",
      "  currentfile buffer readline pop",
      "  token pop /class exch def pop",
      "",
      "  % COMPRESSION",
      "  currentfile buffer readline pop",
      "  token pop /compression exch def pop",
      "",
      "  % CLIP AND RENDER",
      "  /pixel_stream currentfile columns rows compression",
      "    ByteStreamDecodeFilter def",
      "  clipped {ClipImage} if",
      "  alpha stencil not and",
      "  { MaskedImageDict mask_stream resetfile }",
      "  { NonMaskedImageDict }",
      "  ifelse",
      "  stencil {0 setgray imagemask} {image} ifelse",
      "  grestore",
      "  sp {showpage} if",
      "} bind def",
      (char *) NULL
    };

  char
    buffer[MaxTextExtent],
    date[MaxTextExtent],
    density[MaxTextExtent],
    page_geometry[MaxTextExtent],
    **labels;

  const char
    **q;

  CompressionType
    compression;

  const ImageAttribute
    *attribute;

  double
    dx_resolution,
    dy_resolution,
    x_resolution,
    x_scale,
    y_resolution,
    y_scale;

  ExtendedSignedIntegralType
    start,
    stop;

  int
    count,
    status;

  RectangleInfo
    geometry;

  register unsigned int
    i,
    j;

  SegmentInfo
    bounds={0.0,0.0,0.0,0.0};

  size_t
    length;

  time_t
    timer;

  unsigned char
    *pixels;

  unsigned long
    page,
    scene,
    text_size;

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
  compression=image->compression;
  if (image_info->compression != UndefinedCompression)
    compression=image_info->compression;
  switch (compression)
  {
#if !defined(HasJPEG)
    case JPEGCompression:
    {
      compression=RLECompression;
      ThrowException(&image->exception,MissingDelegateError,JPEGLibraryIsNotAvailable,image->filename);
      break;
    }
#endif
#if !defined(HasZLIB)
    case ZipCompression:
    {
      compression=RLECompression;
      ThrowException(&image->exception,MissingDelegateError,ZipLibraryIsNotAvailable,image->filename);
      break;
    }
#endif
    default:
      break;
  }
  page=0;
  scene=0;
  do
  {
    page++;
    /*
      Scale image to size of Postscript page.
    */
    text_size=0;
    attribute=GetImageAttribute(image,"label");
    if (attribute != (const ImageAttribute *) NULL)
      text_size=(unsigned int)(MultilineCensus(attribute->value)*
        image_info->pointsize+12);
    SetGeometry(image,&geometry);
    geometry.y=(long) text_size;
    FormatString(page_geometry,"%lux%lu",image->columns,image->rows);
    if (image_info->page != (char *) NULL)
      {
        (void) strlcpy(page_geometry,image_info->page,MaxTextExtent);
      }
    else
      {
        if ((image->page.width != 0) && (image->page.height != 0))
          (void) FormatString(page_geometry,"%lux%lu%+ld%+ld",
            image->page.width,image->page.height,image->page.x,
              image->page.y);
        else
          if (LocaleCompare(image_info->magick,"PS3") == 0)
            (void) strcpy(page_geometry,PSPageGeometry);
      }
    (void) GetMagickGeometry(page_geometry,&geometry.x,&geometry.y,
       &geometry.width,&geometry.height);
    /*
      Scale relative to dots-per-inch. First whatever resolution passed
      in image_info. Then resolution from the image. Last default PS
      resolution.
    */
    dx_resolution=72.0;
    dy_resolution=72.0;
    x_resolution=72.0;
    (void) strcpy(density,PSDensityGeometry);
    count=GetMagickDimension(density,&x_resolution,&y_resolution,NULL,NULL);
    if (count != 2)
      y_resolution=x_resolution;
    if (image_info->density != (char *) NULL)
      {
        double 
          unit_conversion;

        unit_conversion=
          image_info->units == PixelsPerCentimeterResolution ? 2.54 : 1.0;
        count=GetMagickDimension(image_info->density,&x_resolution,
          &y_resolution,NULL,NULL);
        x_resolution*=unit_conversion;
        if (count != 2)
          y_resolution=x_resolution;
        else
          y_resolution*=unit_conversion;
      }
    else
      {
        double 
          unit_conversion;

        unit_conversion=
          image->units == PixelsPerCentimeterResolution ? 2.54 : 1.0;
        if (image->x_resolution > 0.0)
          x_resolution=image->x_resolution*unit_conversion;
        if (image->y_resolution > 0.0)
          y_resolution=image->y_resolution*unit_conversion;
      }
    x_scale=(geometry.width*dx_resolution)/x_resolution;
    geometry.width=(unsigned long) (x_scale+0.5);
    y_scale=(geometry.height*dy_resolution)/y_resolution;
    geometry.height=(unsigned long) (y_scale+0.5);

    /*
      Postscript header on the first page
    */
    if (page == 1)
      {
        /* Postscript magic */
        if (LocaleCompare(image_info->magick,"PS3") == 0)
          (void) strcpy(buffer,"%!PS-Adobe-3.0\n");
        else
          (void) strcpy(buffer,"%!PS-Adobe-3.0 EPSF-3.0\n");
        (void) WriteBlobString(image,buffer);

        /* Creator */
        FormatString(buffer,"%%%%Creator: GraphicsMagick %s\n",
          MagickLibVersionText);
        (void) WriteBlobString(image,buffer);

        /* Title */
        FormatString(buffer,"%%%%Title: %.1024s\n",image->filename);
        (void) WriteBlobString(image,buffer);

        /* File creation timestamp */
        timer=time((time_t *) NULL);
        (void) localtime(&timer);
        (void) strlcpy(date,ctime(&timer),MaxTextExtent);
        date[strlen(date)-1]='\0';
        FormatString(buffer,"%%%%CreationDate: %.1024s\n",date);
        (void) WriteBlobString(image,buffer);

        /* Bounding box and process colors if not RGB */
        bounds.x1=geometry.x;
        bounds.y1=geometry.y;
        bounds.x2=geometry.x+x_scale;
        bounds.y2=geometry.y+y_scale+text_size;
        if (image_info->adjoin && (image->next != (Image *) NULL))
          {
            (void) WriteBlobString(image,"%%BoundingBox: (atend)\n");
            (void) WriteBlobString(image,"%%HiResBoundingBox: (atend)\n");
            /* No document process colors if there's more than one page */
          }
        else
          {
            FormatString(buffer,"%%%%BoundingBox: %g %g %g %g\n",
              floor(bounds.x1+0.5),floor(bounds.y1+0.5),ceil(bounds.x2-0.5),
              ceil(bounds.y2-0.5));
            (void) WriteBlobString(image,buffer);
            FormatString(buffer,"%%%%HiResBoundingBox: %.7g %.7g %.7g %.7g\n",
              bounds.x1,bounds.y1,bounds.x2,bounds.y2);
            (void) WriteBlobString(image,buffer);

            if (image->colorspace == CMYKColorspace)
              (void) WriteBlobString(image,
                "%%DocumentProcessColors: Cyan Magenta Yellow Black\n");
            else
              if (IsGrayImage(image,&image->exception))
                (void) WriteBlobString(image,
                  "%%DocumentProcessColors: Black\n");
          }

        /* Font resources */
        attribute=GetImageAttribute(image,"label");
        if (attribute != (const ImageAttribute *) NULL)
          (void) WriteBlobString(image,
            "%%DocumentNeededResources: font Helvetica\n");

        /* Language level */
        (void) WriteBlobString(image,"%%LanguageLevel: 3\n");

        /* Pages, orientation, and page order */
        if (LocaleCompare(image_info->magick,"PS3") != 0)
          {
            (void) WriteBlobString(image,"%%Pages: 1\n");
          }
        else
          {
            (void) WriteBlobString(image,"%%Orientation: Portrait\n");
            (void) WriteBlobString(image,"%%PageOrder: Ascend\n");
            if (!image_info->adjoin)
              (void) strcpy(buffer,"%%Pages: 1\n");
            else
              FormatString(buffer,"%%%%Pages: %lu\n",(unsigned long)
                GetImageListLength(image));
            (void) WriteBlobString(image,buffer);
          }
        (void) WriteBlobString(image,"%%EndComments\n");

        /* The static postscript procedures prolog. */
        (void)WriteBlobString(image,"%%BeginProlog\n");
        for (q=PostscriptProlog; *q; q++)
          {
            (void) WriteBlobString(image,*q);
            (void) WriteBlobByte(image,'\n');
          }

        /* One label line for each line in label string */
        attribute=GetImageAttribute(image,"label");
        if (attribute != (const ImageAttribute *) NULL)
          {
            long
              si;

            (void) WriteBlobString(image,"\n  % LABELS\n  /Helvetica "
              "findfont pointsize scalefont setfont\n");
            for (si=(long) MultilineCensus(attribute->value)-1; si >= 0; si--)
              {
                (void) WriteBlobString(image,
                  "  currentfile buffer readline pop token pop\n");
                FormatString(buffer,"  0 y %g add moveto show pop\n",
                  si*image_info->pointsize+12);
                (void) WriteBlobString(image,buffer);
              }
          }

        /* The static postscript procedures epilog. */
        for (q=PostscriptEpilog; *q; q++)
          {
            (void) WriteBlobString(image,*q);
            (void) WriteBlobByte(image,'\n');
          }
        (void)WriteBlobString(image,"%%EndProlog\n");
      }

    /* Page number */
    FormatString(buffer,"%%%%Page: 1 %lu\n",page);
    (void) WriteBlobString(image,buffer);

    /* Page bounding box */
    FormatString(buffer,"%%%%PageBoundingBox: %ld %ld %ld %ld\n",geometry.x,
      geometry.y,geometry.x+(long) geometry.width,geometry.y+(long)
      (geometry.height+text_size));
    (void) WriteBlobString(image,buffer);

    /* Page process colors if not RGB */
    if (image->colorspace == CMYKColorspace)
      (void) WriteBlobString(image,
        "%%PageProcessColors: Cyan Magenta Yellow Black\n");
    else
      if (IsGrayImage(image,&image->exception))
        (void) WriteBlobString(image,"%%PageProcessColors: Black\n");

    /*
      Adjust document bounding box to bound page bounding
      box if page bounding box is the larger
    */
    if (geometry.x < bounds.x1)
      bounds.x1=geometry.x;
    if (geometry.y < bounds.y1)
      bounds.y1=geometry.y;
    if ((geometry.x+x_scale) > bounds.x2)
      bounds.x2=geometry.x+x_scale;
    if ((geometry.y+y_scale+text_size) > bounds.y2)
      bounds.y2=geometry.y+y_scale+text_size;

    /* Page font resource if there's a label */
    attribute=GetImageAttribute(image,"label");
    if (attribute != (const ImageAttribute *) NULL)
      (void) WriteBlobString(image,"%%PageResources: font Helvetica\n");

    /* PS clipping path from Photoshop clipping path */
    if ((image->clip_mask != (Image *) NULL) && 
      (LocaleNCompare("8BIM:",image->clip_mask->magick_filename,5) == 0))
      {
        const ImageAttribute
          *attribute;

        attribute=GetImageAttribute(image,image->clip_mask->magick_filename);
        if (attribute == (const ImageAttribute *) NULL)
          return(False);
        (void) WriteBlobString(image,attribute->value);
        (void) WriteBlobByte(image,'\n');
      }
    else
      {
        /* Reset definition in case previous image had a clipping path.*/
        (void)WriteBlobString(image,"/ClipImage {} def\n");
      }

    /* Push a dictionary for our own def's if this is an EPS */
    if (LocaleCompare(image_info->magick,"PS3") != 0)
      (void) WriteBlobString(image,"userdict begin\n");

    /* Image mask */
    if (image->matte)
      if (!WritePS3MaskImage(image_info, image))
      {
        CloseBlob(image);
        return(False);
      }

    /*
      Remember position of BeginData comment so we can update
      it when we know how many ascii85 encoded bytes we have
      in the blob.
    */
    start=TellBlob(image);
    FormatString(buffer,"%%%%BeginData:%13ld ASCII Bytes\n",0L);
    (void) WriteBlobString(image,buffer);
    stop=TellBlob(image);

    /* Call to image display procedure */
    (void) WriteBlobString(image,"DisplayImage\n");

    /* Translate, scale, and font point size */
    FormatString(buffer,"%ld %ld\n%.7g %.7g\n%f\n",geometry.x,geometry.y,
      x_scale,y_scale,image_info->pointsize);
    (void) WriteBlobString(image,buffer);

    /* Output labels */
    labels=(char **) NULL;
    attribute=GetImageAttribute(image,"label");
    if (attribute != (const ImageAttribute *) NULL)
      labels=StringToList(attribute->value);
    if (labels != (char **) NULL)
      {
        for (i=0; labels[i] != (char *) NULL; i++)
          {
            Ascii85Initialize(image);
            (void) WriteBlobString(image,"<~");
            for (j=0; labels[i][j] != '\0'; j++)
              Ascii85Encode(image, (unsigned long)labels[i][j]);
            Ascii85Flush(image);
            MagickFreeMemory(labels[i]);
          }
        MagickFreeMemory(labels);
      }

    /* Photoshop clipping path active? */
    if ((image->clip_mask != (Image *) NULL) &&
        (LocaleNCompare("8BIM:",image->clip_mask->magick_filename,5) == 0))
      (void) WriteBlobString(image,"true\n");
    else
      (void) WriteBlobString(image,"false\n");
  
    /* Compression seems to take precedence over anyting */
    if (compression == FaxCompression)
      (void) SetImageType(image, BilevelType);

    /* Showpage for non-EPS. */
    (void) WriteBlobString(image,
      LocaleCompare(image_info->magick,"PS3") == 0 ? "true\n" : "false\n");

    /* Image columns and rows; color space */
    FormatString(buffer,"%lu %lu\n%.1024s\n",
      image->columns,image->rows,image->colorspace == CMYKColorspace ?
        PS3_CMYKColorspace : PS3_RGBColorspace);
    (void) WriteBlobString(image,buffer);

    /* Masked image? */
    (void) WriteBlobString(image,image->matte ? "true\n" : "false\n");

    /* Render with imagemask operator? */
    if (((AccessDefinition(image_info,"ps","imagemask")) != 0) &&
        IsMonochromeImage(image,&image->exception))
      (void) WriteBlobString(image,"true\n");
    else
      (void) WriteBlobString(image,"false\n");

    /*
      Output data in one of three ways:
      1) 1 bit and 8 bit single channel direct color image data.
      2) 3 and 4 channel direct color image data
      3) 8 bit color mapped image data
    */

    /*
      Output 1 bit and 8 bit single channel image data. IsGray and IsMono
      may return true for both direct class RGB and CMYK colors, so we need
      to test that these were not requested explicitly.
    */
    if ((image_info->type != TrueColorType) &&
         (image_info->type != TrueColorMatteType) &&
         (image_info->type != ColorSeparationType) &&
         (image_info->type != ColorSeparationMatteType) &&
         (image->colorspace != CMYKColorspace) &&
         (IsGrayImage(image,&image->exception) ||
          IsMonochromeImage(image,&image->exception)))
    {
      /* Image class */
      (void) WriteBlobString(image,PS3_PseudoClass"\n");

      /* Compression scheme - any */
      switch (compression)
      {
        case NoCompression:
        default:
          (void) WriteBlobString(image,PS3_NoCompression"\n");
          break;
        case FaxCompression:
          (void) WriteBlobString(image,PS3_FaxCompression"\n");
          break;
        case RLECompression:
          (void) WriteBlobString(image,PS3_RLECompression"\n");
          break;
        case LZWCompression:
          (void) WriteBlobString(image,PS3_LZWCompression"\n");
          break;
        case ZipCompression:
          (void) WriteBlobString(image,PS3_ZipCompression"\n");
          break;
        case JPEGCompression:
          (void) WriteBlobString(image,PS3_JPEGCompression"\n");
          break;
      }
      
      /* Number of colors -- 0 for single component non-color mapped data */
      (void) WriteBlobString(image,"0\n");

      /* 1 bit or 8 bit components? */
      FormatString(buffer,"%d\n", 
        IsMonochromeImage(image,&image->exception) ? 1 : 8);
      (void) WriteBlobString(image,buffer);

      /* Image data. Always ASCII85 encoded. */
      if (compression == JPEGCompression)
        {
            status=JPEGEncodeImage(image_info,image);
        }
      else
        if (compression == FaxCompression)
          {
              if (LocaleCompare(CCITTParam,"0") == 0)
                status=HuffmanEncodeImage(image_info,image);
              else
                status=Huffman2DEncodeImage(image_info,image);
          }
        else
          {
            status=SerializeSingleChannelImage(image_info,image,&pixels,
              &length);
            if (!status)
              {
                CloseBlob(image);
                return(False);
              }
            Ascii85Initialize(image);
            switch (compression)
            {
              case NoCompression:
              default:
                for (i=0; i < length; i++)
                  Ascii85Encode(image, (unsigned long)pixels[i]);
                break;
              case RLECompression:
                status=PackbitsEncode2Image(image,length,pixels,
                  Ascii85WriteByteHook,(void*)NULL);
                break;
              case LZWCompression:
                status=LZWEncode2Image(image,length,pixels,
                  Ascii85WriteByteHook,(void*)NULL);
                break;
              case ZipCompression:
                status=ZLIBEncode2Image(image,length,image_info->quality,
                  pixels,Ascii85WriteByteHook,(void*)NULL);
                break;
            }
            Ascii85Flush(image);
            MagickFreeMemory(pixels);
          }
    }
    else
      if ((image->storage_class == DirectClass) || (image->colors > 256) ||
          (compression == JPEGCompression))
        {
          /* Image class */
          (void) WriteBlobString(image,PS3_DirectClass"\n");

          /* Compression scheme - fax is only for bilevel images */
          switch (compression)
          {
            case NoCompression:
            default:
              (void) WriteBlobString(image,PS3_NoCompression"\n");
              break;
            case RLECompression:
              (void) WriteBlobString(image,PS3_RLECompression"\n");
              break;
            case LZWCompression:
              (void) WriteBlobString(image,PS3_LZWCompression"\n");
              break;
            case ZipCompression:
              (void) WriteBlobString(image,PS3_ZipCompression"\n");
              break;
            case JPEGCompression:
              (void) WriteBlobString(image,PS3_JPEGCompression"\n");
              break;
          }

          /* Image data. Always ASCII85 encoded. */
          if (compression == JPEGCompression)
            {
                status=JPEGEncodeImage(image_info,image);
            }
          else
            {
              /* Stream based compressions */
              status=SerializeMultiChannelImage(image_info,image,&pixels,
                &length);
              if (!status)
                {
                  CloseBlob(image);
                  return(False);
                }
              Ascii85Initialize(image);
              switch (compression)
              {
                case NoCompression:
                default:
                  for (i=0; i < length; i++)
                    Ascii85Encode(image, (unsigned long)pixels[i]);
                  status=True;
                  break;
                case RLECompression:
                  status=PackbitsEncode2Image(image,length,pixels,
                    Ascii85WriteByteHook,(void*)NULL);
                  break;
                case LZWCompression:
                  status=LZWEncode2Image(image,length,pixels,
                    Ascii85WriteByteHook,(void*)NULL);
                  break;
                case ZipCompression:
                  status=ZLIBEncode2Image(image,length,image_info->quality,
                    pixels,Ascii85WriteByteHook,(void*)NULL);
                  break;
              }
              Ascii85Flush(image);
              MagickFreeMemory(pixels);
            }
        }
      else
        {
          /*
            Color mapped images.
            
            Image class.
          */
          (void) WriteBlobString(image,PS3_PseudoClass"\n");

          /*
            Compression scheme - fax is only for bilevel images,
            JPEG for true color (single channel) images.
          */
          switch (compression)
          {
            case NoCompression:
            default:
              (void) WriteBlobString(image,PS3_NoCompression"\n");
              break;
            case RLECompression:
              (void) WriteBlobString(image,PS3_RLECompression"\n");
              break;
            case LZWCompression:
              (void) WriteBlobString(image,PS3_LZWCompression"\n");
              break;
            case ZipCompression:
              (void) WriteBlobString(image,PS3_ZipCompression"\n");
              break;
          }
          
          /* Number of colors in color map */
          FormatString(buffer,"%u\n",image->colors);
          (void) WriteBlobString(image,buffer);

          /* Color map - uncompressed, ascii85 encoded */
          Ascii85Initialize(image);
          for (i=0; i < image->colors; i++)
          {
            Ascii85Encode(image, (unsigned long)image->colormap[i].red);
            Ascii85Encode(image, (unsigned long)image->colormap[i].green);
            Ascii85Encode(image, (unsigned long)image->colormap[i].blue);
          }
          Ascii85Flush(image);

          status=SerializePseudoClassImage(image_info,image,&pixels,&length);
          if (!status)
            {
              CloseBlob(image);
              return(False);
            }
          Ascii85Initialize(image);
          switch (compression)
          {
            case NoCompression:
            default:
              for (i=0; i < length; i++)
                Ascii85Encode(image, (unsigned long)pixels[i]);
              status=True;
              break;
            case RLECompression:
              status=PackbitsEncode2Image(image,length,pixels,
                Ascii85WriteByteHook,(void *)NULL);
              break;
            case LZWCompression:
              status=LZWEncode2Image(image,length,pixels,Ascii85WriteByteHook,
                (void *)NULL);
              break;
            case ZipCompression:
              status=ZLIBEncode2Image(image,length,image_info->quality,pixels,
                Ascii85WriteByteHook,(void *)NULL);
              break;
          }
          Ascii85Flush(image);
          MagickFreeMemory(pixels);
        }

    if (!status)
      {
        CloseBlob(image);
        return(False);
      }

    /* Update BeginData now that we know the data size */
    length=TellBlob(image)-stop;
    stop=TellBlob(image);
    (void) SeekBlob(image,start,SEEK_SET);
    FormatString(buffer,"%%%%BeginData:%13ld ASCII Bytes\n",(long) length);
    (void) WriteBlobString(image,buffer);
    (void) SeekBlob(image,stop,SEEK_SET);
    (void) WriteBlobString(image,"%%EndData\n");

    /* End private dictionary if this is an EPS */
    if (LocaleCompare(image_info->magick,"PS3") != 0)
      (void) WriteBlobString(image,"end\n");

    (void) WriteBlobString(image,"%%PageTrailer\n");
    if (image->next == (Image *) NULL)
      break;
    image=SyncNextImageInList(image);
    status=MagickMonitorFormatted(scene,GetImageListLength(image),
                                  &image->exception,SaveImagesText,
                                  image->filename);
    if (status == False)
      break;
    scene++;
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  (void) WriteBlobString(image,"%%Trailer\n");
  if (page > 1)
    {
      FormatString(buffer,"%%%%BoundingBox: %g %g %g %g\n",
        floor(bounds.x1+0.5),floor(bounds.y1+0.5),ceil(bounds.x2-0.5),
        ceil(bounds.y2-0.5));
      (void) WriteBlobString(image,buffer);
      FormatString(buffer,"%%%%HiResBoundingBox: %.7g %.7g %.7g %.7g\n",
        bounds.x1,bounds.y1,bounds.x2,bounds.y2);
      (void) WriteBlobString(image,buffer);
    }
  (void) WriteBlobString(image,"%%EOF\n");
  CloseBlob(image);
  return(True);
}

#if defined(HasZLIB)
#include "zlib.h"
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   Z L I B E n c o d e 2 I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ZLIBEncode2Image compresses an image via ZLIB-coding specific to
%  Postscript Level II or Portable Document Format.  To ensure portability, the
%  binary ZLIB bytes are encoded as ASCII base-85.
%
%  The format of the ZLIBEncode2Image method is:
%
%      unsigned int ZLIBEncode2Image(Image *image,const size_t length,
%        const unsigned long quality,unsigned char *pixels,
%        WriteByteHook write_byte,void *info)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o length:  A value that specifies the number of pixels to compress.
%
%    o quality: the compression level (0-100).
%
%    o pixels: The address of an unsigned array of characters containing the
%      pixels to compress.
%
%    o write_byte: function (hook) to call for writing each byte of compressed
%      data.
%
%    o info: information block to pass along when calling write_byte
%      function.
%
%
*/
static unsigned int ZLIBEncode2Image(Image *image,const size_t length,
  const unsigned long quality,unsigned char *pixels,WriteByteHook write_byte,
  void *info)
{
  int
    status;

  register long
    i;

  unsigned char
    *compressed_pixels;

  unsigned long
    compressed_packets;

  z_stream
    stream;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  compressed_packets=(unsigned long) (1.001*length+12);
  compressed_pixels=MagickAllocateMemory(unsigned char *,compressed_packets);
  if (compressed_pixels == (unsigned char *) NULL)
    ThrowBinaryException(ResourceLimitError,MemoryAllocationFailed,
      (char *) NULL);
  stream.next_in=pixels;
  stream.avail_in=(unsigned int) length;
  stream.next_out=compressed_pixels;
  stream.avail_out=(unsigned int) compressed_packets;
  stream.zalloc=(alloc_func) NULL;
  stream.zfree=(free_func) NULL;
  stream.opaque=(voidpf) NULL;
  status=deflateInit(&stream,(int) Min(quality/10,9));
  if (status == Z_OK)
    {
      status=deflate(&stream,Z_FINISH);
      if (status == Z_STREAM_END)
        status=deflateEnd(&stream);
      else
        (void) deflateEnd(&stream);
      compressed_packets=stream.total_out;
    }
  if (status)
    ThrowBinaryException(CoderError,UnableToZipCompressImage,(char *) NULL)
  else
    for (i=0; i < (long) compressed_packets; i++)
      (void) (*write_byte)(image,(magick_uint8_t)compressed_pixels[i],info);
  MagickFreeMemory(compressed_pixels);
  return(!status);
}
#else
static unsigned int ZLIBEncode2Image(Image *image,const size_t length,
  const unsigned long quality,unsigned char *pixels,WriteByteHook write_byte,
  void *info)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  ThrowBinaryException(MissingDelegateError,ZipLibraryIsNotAvailable,image->filename);
  return(False);
}
#endif

