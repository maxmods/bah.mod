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
%                            DDDD   IIIII  BBBB                               %
%                            D   D    I    B   B                              %
%                            D   D    I    BBBB                               %
%                            D   D    I    B   B                              %
%                            DDDD   IIIII  BBBB                               %
%                                                                             %
%                                                                             %
%                   Read/Write Windows DIB Image Format.                      %
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
#include "magick/color.h"
#include "magick/transform.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/render.h"
#include "magick/utility.h"

/*
  Typedef declarations.
*/
typedef struct _DIBInfo
{
  unsigned long
    size;

  long
    width,
    height;

  unsigned short
    planes,
    bits_per_pixel;

  unsigned long
    compression,
    image_size,
    x_pixels,
    y_pixels,
    number_colors,
    colors_important;

  unsigned short
    red_mask,
    green_mask,
    blue_mask,
    alpha_mask;

  long
    colorspace;

  PointInfo
    red_primary,
    green_primary,
    blue_primary,
    gamma_scale;
} DIBInfo;

/*
  Forward declarations.
*/
static unsigned int
  WriteDIBImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e c o d e I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DecodeImage unpacks the packed image pixels into runlength-encoded
%  pixel packets.
%
%  The format of the DecodeImage method is:
%
%      unsigned int DecodeImage(Image *image,const unsigned long compression,
%        unsigned char *pixels)
%
%  A description of each parameter follows:
%
%    o status:  Method DecodeImage returns True if all the pixels are
%      uncompressed without error, otherwise False.
%
%    o image: The address of a structure of type Image.
%
%    o compression:  A value of 1 means the compressed pixels are runlength
%      encoded for a 256-color bitmap.  A value of 2 means a 16-color bitmap.
%
%    o pixels:  The address of a byte (8 bits) array of pixel data created by
%      the decoding process.
%
%
*/
static unsigned int DecodeImage(Image *image,const unsigned long compression,
  unsigned char *pixels)
{
  long
    byte,
    count,
    y;

  register long
    i,
    x;

  register unsigned char
    *q;

  unsigned char
    *end;

  assert(image != (Image *) NULL);
  assert(pixels != (unsigned char *) NULL);
  (void) memset(pixels,0,image->columns*image->rows);
  byte=0;
  x=0;
  q=pixels;
  end=pixels + (size_t) image->columns*image->rows;
  for (y=0; y < (long) image->rows; )
  {
    if (q < pixels || q  >= end)
      break;
    count=ReadBlobByte(image);
    if (count == EOF)
      break;
    if (count != 0)
      {
        count=Min(count, end - q);
        /*
          Encoded mode.
        */
        byte=ReadBlobByte(image);
        for (i=0; i < count; i++)
        {
          if (compression == 1)
            *q++=(unsigned char) byte;
          else
            *q++=(unsigned char)
              ((i & 0x01) ? (byte & 0x0f) : ((byte >> 4) & 0x0f));
          x++;
        }
      }
    else
      {
        /*
          Escape mode.
        */
        count=ReadBlobByte(image);
        if (count == 0x01)
          return(True);
        switch ((int) count)
        {
          case 0x00:
          {
            /*
              End of line.
            */
            x=0;
            y++;
            q=pixels+y*image->columns;
            break;
          }
          case 0x02:
          {
            /*
              Delta mode.
            */
            x+=ReadBlobByte(image);
            y+=ReadBlobByte(image);
            q=pixels+y*image->columns+x;
            break;
          }
          default:
          {
            /*
              Absolute mode.
            */
      	    count=Min(count, end - q);
            for (i=0; i < count; i++)
            {
              if (compression == 1)
                *q++=ReadBlobByte(image);
              else
                {
                  if ((i & 0x01) == 0)
                    byte=ReadBlobByte(image);
                  *q++=(unsigned char)
                    ((i & 0x01) ? (byte & 0x0f) : ((byte >> 4) & 0x0f));
                }
              x++;
            }
            /*
              Read pad byte.
            */
            if (compression == 1)
              {
                if (count & 0x01)
                  (void) ReadBlobByte(image);
              }
            else
              if (((count & 0x03) == 1) || ((count & 0x03) == 2))
                (void) ReadBlobByte(image);
            break;
          }
        }
      }
    if (QuantumTick(y,image->rows))
      if (!MagickMonitorFormatted(y,image->rows,&image->exception,
                                  LoadImageText,image->filename))
        break;
  }
  (void) ReadBlobByte(image);  /* end of line */
  (void) ReadBlobByte(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   E n c o d e I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method EncodeImage compresses pixels using a runlength encoded format.
%
%  The format of the EncodeImage method is:
%
%    static unsigned int EncodeImage(Image *image,
%      const unsigned long bytes_per_line,const unsigned char *pixels,
%      unsigned char *compressed_pixels)
%
%  A description of each parameter follows:
%
%    o status:  Method EncodeImage returns the number of bytes in the
%      runlength encoded compress_pixels array.
%
%    o image:  A pointer to an Image structure.
%
%    o bytes_per_line: The number of bytes in a scanline of compressed pixels
%
%    o pixels:  The address of a byte (8 bits) array of pixel data created by
%      the compression process.
%
%    o compressed_pixels:  The address of a byte (8 bits) array of compressed
%      pixel data.
%
%
*/
static size_t EncodeImage(Image *image,const unsigned long bytes_per_line,
  const unsigned char *pixels,unsigned char *compressed_pixels)
{
  long
    y;

  register const unsigned char
    *p;

  register long
    i,
    x;

  register unsigned char
    *q;

  /*
    Runlength encode pixels.
  */
  assert(image != (Image *) NULL);
  assert(pixels != (const unsigned char *) NULL);
  assert(compressed_pixels != (unsigned char *) NULL);
  p=pixels;
  q=compressed_pixels;
  i=0;
  for (y=0; y < (long) image->rows; y++)
  {
    for (x=0; x < (long) bytes_per_line; x+=i)
    {
      /*
        Determine runlength.
      */
      for (i=1; ((x+i) < (long) bytes_per_line); i++)
        if ((*(p+i) != *p) || (i == 255U))
          break;
      *q++=(unsigned char) i;
      *q++=(*p);
      p+=i;
    }
    /*
      End of line.
    */
    *q++=0x00;
    *q++=0x00;
    if (QuantumTick(y,image->rows))
      if (!MagickMonitorFormatted(y,image->rows,&image->exception,
                                  SaveImageText,image->filename))
        break;
  }
  /*
    End of bitmap.
  */
  *q++=0;
  *q++=0x01;
  return(q-compressed_pixels);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s D I B                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsDIB returns True if the image format type, identified by the
%  magick string, is DIB.
%
%  The format of the IsDIB method is:
%
%      unsigned int IsDIB(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsDIB returns True if the image format type is DIB.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsDIB(const unsigned char *magick,const size_t length)
{
  if (length < 2)
    return(False);
  if( (*magick == 40) && (*(magick+1)==0))
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d D I B I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadDIBImage reads a Microsoft Windows bitmap image file and
%  returns it.  It allocates the memory necessary for the new Image structure
%  and returns a pointer to the new image.
%
%  The format of the ReadDIBImage method is:
%
%      image=ReadDIBImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadDIBImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadDIBImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  DIBInfo
    dib_info;

  Image
    *image;

  IndexPacket
    index;

  long
    bit,
    y;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *q;

  register long
    i;

  register unsigned char
    *p;

  size_t
    length;

  unsigned char
    *pixels;

  unsigned int
    status;

  unsigned long
    bytes_per_line;

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
    Determine if this is a DIB file.
  */
  (void) memset(&dib_info,0,sizeof(DIBInfo));
  dib_info.size=ReadBlobLSBLong(image);
  if (dib_info.size!=40)
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
  /*
    Microsoft Windows 3.X DIB image file.
  */

  /*
    BMP v3 defines width and hight as signed LONG (32 bit) values.  If
    height is a positive number, then the image is a "bottom-up"
    bitmap with origin in the lower-left corner.  If height is a
    negative number, then the image is a "top-down" bitmap with the
    origin in the upper-left corner.  The meaning of negative values
    is not defined for width.
  */
  dib_info.width=(magick_int32_t) ReadBlobLSBLong(image);
  dib_info.height=(magick_int32_t) ReadBlobLSBLong(image);
  dib_info.planes=ReadBlobLSBShort(image);
  dib_info.bits_per_pixel=ReadBlobLSBShort(image);
  dib_info.compression=ReadBlobLSBLong(image);
  dib_info.image_size=ReadBlobLSBLong(image);
  dib_info.x_pixels=ReadBlobLSBLong(image);
  dib_info.y_pixels=ReadBlobLSBLong(image);
  dib_info.number_colors=ReadBlobLSBLong(image);
  dib_info.colors_important=ReadBlobLSBLong(image);
  if ((dib_info.compression == 3) && ((dib_info.bits_per_pixel == 16) ||
      (dib_info.bits_per_pixel == 32)))
    {
      dib_info.red_mask=ReadBlobLSBShort(image);
      dib_info.green_mask=ReadBlobLSBShort(image);
      dib_info.blue_mask=ReadBlobLSBShort(image);
    }
  if (dib_info.width <= 0)
      ThrowReaderException(CorruptImageWarning,NegativeOrZeroImageSize,image);
  if (dib_info.height == 0)
      ThrowReaderException(CorruptImageWarning,NegativeOrZeroImageSize,image);
  image->matte=dib_info.bits_per_pixel == 32;
  image->columns=AbsoluteValue(dib_info.width);
  image->rows=AbsoluteValue(dib_info.height);
  image->depth=8;
  if ((dib_info.number_colors != 0) || (dib_info.bits_per_pixel < 16))
    {
      image->storage_class=PseudoClass;
      image->colors=dib_info.number_colors;
      if (image->colors == 0)
        image->colors=1L << dib_info.bits_per_pixel;
    }
  if(image_info->size)
    {
      int
        flags;

      RectangleInfo
        geometry;

      flags=GetGeometry(image_info->size,&geometry.x,&geometry.y,
        &geometry.width,&geometry.height);
      if ((flags & WidthValue) && (geometry.width != 0)
          && (geometry.width < image->columns))
        image->columns=geometry.width;
      if ((flags & HeightValue) && (geometry.height != 0)
          && (geometry.height < image->rows))
        image->rows=geometry.height;
    }
  if (image->storage_class == PseudoClass)
    {
      unsigned char
        *dib_colormap;

      unsigned int
        packet_size;

      /*
        Read DIB raster colormap.
      */
      if (!AllocateImageColormap(image,image->colors))
        ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
      dib_colormap=MagickAllocateArray(unsigned char *,image->colors,4);
      if (dib_colormap == (unsigned char *) NULL)
        ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
      packet_size=4;
      (void) ReadBlob(image,packet_size*image->colors,(char *) dib_colormap);
      p=dib_colormap;
      for (i=0; i < (long) image->colors; i++)
      {
        image->colormap[i].blue=ScaleCharToQuantum(*p++);
        image->colormap[i].green=ScaleCharToQuantum(*p++);
        image->colormap[i].red=ScaleCharToQuantum(*p++);
        if (packet_size == 4)
          p++;
      }
      MagickFreeMemory(dib_colormap);
    }
  /*
    Read image data.
  */
  if (dib_info.compression == 2)
    dib_info.bits_per_pixel<<=1;
  bytes_per_line=4*((image->columns*dib_info.bits_per_pixel+31)/32);
  length=bytes_per_line*image->rows;
  pixels=MagickAllocateArray(unsigned char *,
                                      image->rows,
                                      Max(bytes_per_line,image->columns+1));
  if (pixels == (unsigned char *) NULL)
    ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
  if ((dib_info.compression == 0) || (dib_info.compression == 3))
    (void) ReadBlob(image,length,(char *) pixels);
  else
    {
      /*
        Convert run-length encoded raster pixels.
        DecodeImage expects that pixels array is rows*columns bytes.
      */
      status=DecodeImage(image,dib_info.compression,pixels);
      if (status == False)
        ThrowReaderException(CorruptImageError,UnableToRunlengthDecodeImage,
          image);
    }
  /*
    Initialize image structure.
  */
  image->units=PixelsPerCentimeterResolution;
  image->x_resolution=dib_info.x_pixels/100.0;
  image->y_resolution=dib_info.y_pixels/100.0;
  /*
    Convert DIB raster image to pixel packets.
  */
  switch (dib_info.bits_per_pixel)
  {
    case 1:
    {
      /*
        Convert bitmap scanline.
      */
      for (y=(long) image->rows-1; y >= 0; y--)
      {
        p=pixels+(image->rows-y-1)*bytes_per_line;
        q=SetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=AccessMutableIndexes(image);
        for (x=0; x < ((long) image->columns-7); x+=8)
        {
          for (bit=0; bit < 8; bit++)
          {
            index=((*p) & (0x80 >> bit) ? 0x01 : 0x00);
            indexes[x+bit]=index;
            *q++=image->colormap[index];
          }
          p++;
        }
        if ((image->columns % 8) != 0)
          {
            for (bit=0; bit < (long) (image->columns % 8); bit++)
            {
              index=((*p) & (0x80 >> bit) ? 0x01 : 0x00);
              indexes[x+bit]=index;
              *q++=image->colormap[index];
            }
            p++;
          }
        if (!SyncImagePixels(image))
          break;
        if (image->previous == (Image *) NULL)
          if (QuantumTick(y,image->rows))
            {
              status=MagickMonitorFormatted(image->rows-y-1,image->rows,
                                            exception,LoadImageText,
                                            image->filename);
              if (status == False)
                break;
            }
      }
      break;
    }
    case 4:
    {
      /*
        Convert PseudoColor scanline.
      */
      for (y=(long) image->rows-1; y >= 0; y--)
      {
        p=pixels+(image->rows-y-1)*bytes_per_line;
        q=SetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=AccessMutableIndexes(image);
        for (x=0; x < ((long) image->columns-1); x+=2)
        {
          index=(IndexPacket) ((*p >> 4) & 0xf);
          VerifyColormapIndex(image,index);
          indexes[x]=index;
          *q++=image->colormap[index];
          index=(IndexPacket) (*p & 0xf);
          VerifyColormapIndex(image,index);
          indexes[x+1]=index;
          *q++=image->colormap[index];
          p++;
        }
        if ((image->columns % 2) != 0)
          {
            index=(IndexPacket) ((*p >> 4) & 0xf);
            VerifyColormapIndex(image,index);
            indexes[x]=index;
            *q++=image->colormap[index];
            p++;
          }
        if (!SyncImagePixels(image))
          break;
        if (image->previous == (Image *) NULL)
          if (QuantumTick(y,image->rows))
            {
              status=MagickMonitorFormatted(image->rows-y-1,image->rows,
                                            exception,LoadImageText,
                                            image->filename);
              if (status == False)
                break;
            }
      }
      break;
    }
    case 8:
    {
      /*
        Convert PseudoColor scanline.
      */
      if ((dib_info.compression == 1) || (dib_info.compression == 2))
        bytes_per_line=image->columns;
      for (y=(long) image->rows-1; y >= 0; y--)
      {
        p=pixels+(image->rows-y-1)*bytes_per_line;
        q=SetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=AccessMutableIndexes(image);
        for (x=0; x < (long) image->columns; x++)
        {
          index=(IndexPacket) (*p);
          VerifyColormapIndex(image,index);
          indexes[x]=index;
          *q=image->colormap[index];
          p++;
          q++;
        }
        if (!SyncImagePixels(image))
          break;
        if (image->previous == (Image *) NULL)
          if (QuantumTick(y,image->rows))
            {
              status=MagickMonitorFormatted(image->rows-y-1,image->rows,
                                            exception,LoadImageText,
                                            image->filename);
              if (status == False)
                break;
            }
      }
      break;
    }
    case 16:
    {
      unsigned short
        word;

      /*
        Convert PseudoColor scanline.
      */
      image->storage_class=DirectClass;
      if (dib_info.compression == 1)
        bytes_per_line=2*image->columns;
      for (y=(long) image->rows-1; y >= 0; y--)
      {
        p=pixels+(image->rows-y-1)*bytes_per_line;
        q=SetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (long) image->columns; x++)
        {
          word=(*p++);
          word|=(*p++ << 8);
          if (dib_info.red_mask == 0)
            {
              q->red=ScaleCharToQuantum(ScaleColor5to8((word >> 10) & 0x1f));
              q->green=ScaleCharToQuantum(ScaleColor5to8((word >> 5) & 0x1f));
              q->blue=ScaleCharToQuantum(ScaleColor5to8(word & 0x1f));
            }
          else
            {
              q->red=ScaleCharToQuantum(ScaleColor5to8((word >> 11) & 0x1f));
              q->green=ScaleCharToQuantum(ScaleColor6to8((word >> 5) & 0x3f));
              q->blue=ScaleCharToQuantum(ScaleColor5to8(word & 0x1f));
            }
          q++;
        }
        if (!SyncImagePixels(image))
          break;
        if (image->previous == (Image *) NULL)
          if (QuantumTick(y,image->rows))
            {
              status=MagickMonitorFormatted(image->rows-y-1,image->rows,
                                            exception,LoadImageText,
                                            image->filename);
              if (status == False)
                break;
            }
      }
      break;
    }
    case 24:
    case 32:
    {
      /*
        Convert DirectColor scanline.
      */
      for (y=(long) image->rows-1; y >= 0; y--)
      {
        p=pixels+(image->rows-y-1)*bytes_per_line;
        q=SetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (long) image->columns; x++)
        {
          q->blue=ScaleCharToQuantum(*p++);
          q->green=ScaleCharToQuantum(*p++);
          q->red=ScaleCharToQuantum(*p++);
          if (image->matte)
            q->opacity=ScaleCharToQuantum(*p++);
          q++;
        }
        if (!SyncImagePixels(image))
          break;
        if (image->previous == (Image *) NULL)
          if (QuantumTick(y,image->rows))
            {
              status=MagickMonitorFormatted(image->rows-y-1,image->rows,
                                            exception,LoadImageText,
                                            image->filename);
              if (status == False)
                break;
            }
      }
      break;
    }
    default:
      ThrowReaderException(CorruptImageError,ImproperImageHeader,image)
  }
  MagickFreeMemory(pixels);
  if (EOFBlob(image))
    ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,
      image->filename);
  if (dib_info.height < 0)
    {
      Image
        *flipped_image;

      /*
        Correct image orientation.
      */
      flipped_image=FlipImage(image,exception);
      if (flipped_image == (Image *) NULL)
        {
          DestroyImageList(image);
          return((Image *) NULL);
        }
      DestroyImage(image);
      image=flipped_image;
    }
  CloseBlob(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r D I B I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterDIBImage adds attributes for the DIB image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterDIBImage method is:
%
%      RegisterDIBImage(void)
%
*/
ModuleExport void RegisterDIBImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("DIB");
  entry->decoder=(DecoderHandler) ReadDIBImage;
  entry->encoder=(EncoderHandler) WriteDIBImage;
  entry->magick=(MagickHandler) IsDIB;
  entry->adjoin=False;
  entry->stealth=True;
  entry->description="Microsoft Windows 3.X Packed Device-Independent Bitmap";
  entry->module="DIB";
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r D I B I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterDIBImage removes format registrations made by the
%  DIB module from the list of supported formats.
%
%  The format of the UnregisterDIBImage method is:
%
%      UnregisterDIBImage(void)
%
*/
ModuleExport void UnregisterDIBImage(void)
{
  (void) UnregisterMagickInfo("DIB");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e D I B I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteDIBImage writes an image in Microsoft Windows bitmap encoded
%  image format.
%
%  The format of the WriteDIBImage method is:
%
%      unsigned int WriteDIBImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteDIBImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
static unsigned int WriteDIBImage(const ImageInfo *image_info,Image *image)
{
  DIBInfo
    dib_info;

  long
    y;

  register const PixelPacket
    *p;

  register const IndexPacket
    *indexes;

  register long
    i,
    x;

  register unsigned char
    *q;

  unsigned char
    *dib_data,
    *pixels;

  unsigned int
    status;

  unsigned long
    bytes_per_line;

  ImageCharacteristics
    characteristics;

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
    Ensure that image is in an RGB space.
  */
  (void) TransformColorspace(image,RGBColorspace);
  /*
    Analyze image to be written.
  */
  if (!GetImageCharacteristics(image,&characteristics,
                               (OptimizeType == image_info->type),
                               &image->exception))
    {
      CloseBlob(image);
      return MagickFail;
    }
  /*
    Initialize DIB raster file header.
  */
  if (image->storage_class == DirectClass)
    {
      /*
        Full color DIB raster.
      */
      dib_info.number_colors=0;
      dib_info.bits_per_pixel=image->matte ? 32 : 24;
    }
  else
    {
      /*
        Colormapped DIB raster.
      */
      dib_info.bits_per_pixel=8;
      if (characteristics.monochrome)
        dib_info.bits_per_pixel=1;
      dib_info.number_colors=1 << dib_info.bits_per_pixel;
    }
  bytes_per_line=4*((image->columns*dib_info.bits_per_pixel+31)/32);
  dib_info.size=40;
  dib_info.width=(long) image->columns;
  dib_info.height=(long) image->rows;
  dib_info.planes=1;
  dib_info.compression=0;
  dib_info.image_size=bytes_per_line*image->rows;
  dib_info.x_pixels=75*39;
  dib_info.y_pixels=75*39;
  if (image->units == PixelsPerInchResolution)
    {
      dib_info.x_pixels=(unsigned long) (100.0*image->x_resolution/2.54);
      dib_info.y_pixels=(unsigned long) (100.0*image->y_resolution/2.54);
    }
  if (image->units == PixelsPerCentimeterResolution)
    {
      dib_info.x_pixels=(unsigned long) (100.0*image->x_resolution);
      dib_info.y_pixels=(unsigned long) (100.0*image->y_resolution);
    }
  dib_info.colors_important=dib_info.number_colors;
  /*
    Convert MIFF to DIB raster pixels.
  */
  pixels=MagickAllocateMemory(unsigned char *,dib_info.image_size);
  if (pixels == (unsigned char *) NULL)
    ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
  switch (dib_info.bits_per_pixel)
  {
    case 1:
    {
      register unsigned char
        bit,
        byte;

      /*
        Convert PseudoClass image to a DIB monochrome image.
      */
      for (y=0; y < (long) image->rows; y++)
      {
        p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
        if (p == (const PixelPacket *) NULL)
          break;
        indexes=AccessImmutableIndexes(image);
        q=pixels+(image->rows-y-1)*bytes_per_line;
        bit=0;
        byte=0;
        for (x=0; x < (long) image->columns; x++)
        {
          byte<<=1;
          byte|=indexes[x] ? 0x01 : 0x00;
          bit++;
          if (bit == 8)
            {
              *q++=byte;
              bit=0;
              byte=0;
            }
           p++;
         }
       if (bit != 0)
         *q++=byte << (8-bit);
       /* initialize padding bytes */
       for (x=(long) (image->columns+7)/8; x < (long) bytes_per_line; x++)
         *q++=0x00;
       if (image->previous == (Image *) NULL)
         if (QuantumTick(y,image->rows))
           if (!MagickMonitorFormatted(y,image->rows,&image->exception,
                                       SaveImageText,image->filename))
             break;
      }
      break;
    }
    case 8:
    {
      /*
        Convert PseudoClass packet to DIB pixel.
      */
      for (y=0; y < (long) image->rows; y++)
      {
        p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
        if (p == (const PixelPacket *) NULL)
          break;
        indexes=AccessImmutableIndexes(image);
        q=pixels+(image->rows-y-1)*bytes_per_line;
        for (x=0; x < (long) image->columns; x++)
        {
          *q++=indexes[x];
          p++;
        }
       /* initialize padding bytes */
       for (; x < (long) bytes_per_line; x++)
         *q++=0x00;
        if (image->previous == (Image *) NULL)
          if (QuantumTick(y,image->rows))
            if (!MagickMonitorFormatted(y,image->rows,&image->exception,
                                        SaveImageText,image->filename))
              break;
      }
      break;
    }
    case 24:
    case 32:
    {
      /*
        Convert DirectClass packet to DIB RGB pixel.
      */
      for (y=0; y < (long) image->rows; y++)
      {
        p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
        if (p == (const PixelPacket *) NULL)
          break;
        q=pixels+(image->rows-y-1)*bytes_per_line;
        for (x=0; x < (long) image->columns; x++)
        {
          *q++=ScaleQuantumToChar(p->blue);
          *q++=ScaleQuantumToChar(p->green);
          *q++=ScaleQuantumToChar(p->red);
          if (image->matte)
            *q++=ScaleQuantumToChar(p->opacity);
          p++;
        }
        /* initialize padding bytes */
        if (dib_info.bits_per_pixel == 24)
          {
            /* initialize padding bytes */
            for (x=3*image->columns; x < (long) bytes_per_line; x++)
              *q++=0x00;
          }
        if (image->previous == (Image *) NULL)
          if (QuantumTick(y,image->rows))
            if (!MagickMonitorFormatted(y,image->rows,&image->exception,
                                        SaveImageText,image->filename))
               break;
      }
      break;
    }
  }
  if (dib_info.bits_per_pixel == 8)
    if (image_info->compression != NoCompression)
      {
        size_t
          length;

        /*
          Convert run-length encoded raster pixels.
        */
        length=2*(bytes_per_line+2)*(image->rows+2)+2;
        dib_data=MagickAllocateMemory(unsigned char *,length);
        if (pixels == (unsigned char *) NULL)
          {
            MagickFreeMemory(pixels);
            ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,
              image)
          }
        dib_info.image_size=EncodeImage(image,bytes_per_line,pixels,dib_data);
        MagickFreeMemory(pixels);
        pixels=dib_data;
        dib_info.compression=1;
      }
  /*
    Write DIB header.
  */
  (void) WriteBlobLSBLong(image,dib_info.size);
  (void) WriteBlobLSBLong(image,dib_info.width);
  (void) WriteBlobLSBLong(image,dib_info.height);
  (void) WriteBlobLSBShort(image,dib_info.planes);
  (void) WriteBlobLSBShort(image,dib_info.bits_per_pixel);
  (void) WriteBlobLSBLong(image,dib_info.compression);
  (void) WriteBlobLSBLong(image,dib_info.image_size);
  (void) WriteBlobLSBLong(image,dib_info.x_pixels);
  (void) WriteBlobLSBLong(image,dib_info.y_pixels);
  (void) WriteBlobLSBLong(image,dib_info.number_colors);
  (void) WriteBlobLSBLong(image,dib_info.colors_important);
  if (image->storage_class == PseudoClass)
    {
      unsigned char
        *dib_colormap;

      /*
        Dump colormap to file.
      */
      dib_colormap=MagickAllocateArray(unsigned char *,
                                       (1 << dib_info.bits_per_pixel),4);
      if (dib_colormap == (unsigned char *) NULL)
        ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
      q=dib_colormap;
      for (i=0; i < (long) Min(image->colors,dib_info.number_colors); i++)
      {
        *q++=ScaleQuantumToChar(image->colormap[i].blue);
        *q++=ScaleQuantumToChar(image->colormap[i].green);
        *q++=ScaleQuantumToChar(image->colormap[i].red);
        *q++=(Quantum) 0x0;
      }
      for ( ; i < (1L << dib_info.bits_per_pixel); i++)
      {
        *q++=(Quantum) 0x0;
        *q++=(Quantum) 0x0;
        *q++=(Quantum) 0x0;
        *q++=(Quantum) 0x0;
      }
      (void) WriteBlob(image,4*(1 << dib_info.bits_per_pixel),
        (char *) dib_colormap);
      MagickFreeMemory(dib_colormap);
    }
  (void) WriteBlob(image,dib_info.image_size,(char *) pixels);
  MagickFreeMemory(pixels);
  CloseBlob(image);
  return(True);
}
