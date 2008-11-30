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
%                        PPPP   IIIII   CCCC  TTTTT                           %
%                        P   P    I    C        T                             %
%                        PPPP     I    C        T                             %
%                        P        I    C        T                             %
%                        P      IIIII   CCCC    T                             %
%                                                                             %
%                                                                             %
%              Read/Write Apple Macintosh QuickDraw/PICT Format.              %
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
#include "magick/composite.h"
#include "magick/constitute.h"
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/profile.h"
#include "magick/tempfile.h"
#include "magick/transform.h"
#include "magick/utility.h"

/*
  GraphicsMagick Macintosh PICT Methods.
*/
#define ReadPixmap(pixmap) \
{ \
  pixmap.version=ReadBlobMSBShort(image); \
  pixmap.pack_type=ReadBlobMSBShort(image); \
  pixmap.pack_size=ReadBlobMSBLong(image); \
  pixmap.horizontal_resolution=ReadBlobMSBLong(image); \
  pixmap.vertical_resolution=ReadBlobMSBLong(image); \
  pixmap.pixel_type=ReadBlobMSBShort(image); \
  pixmap.bits_per_pixel=ReadBlobMSBShort(image); \
  pixmap.component_count=ReadBlobMSBShort(image); \
  pixmap.component_size=ReadBlobMSBShort(image); \
  pixmap.plane_bytes=ReadBlobMSBLong(image); \
  pixmap.table=ReadBlobMSBLong(image); \
  pixmap.reserved=ReadBlobMSBLong(image); \
  if (pixmap.bits_per_pixel <= 0 || pixmap.bits_per_pixel > 32 || \
      pixmap.component_count <= 0 || pixmap.component_count > 4 || \
      pixmap.component_size <= 0) \
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image); \
}

#define ReadRectangle(rectangle) \
{ \
  rectangle.top=ReadBlobMSBShort(image); \
  rectangle.left=ReadBlobMSBShort(image); \
  rectangle.bottom=ReadBlobMSBShort(image); \
  rectangle.right=ReadBlobMSBShort(image); \
  if (rectangle.top > rectangle.bottom || \
      rectangle.left > rectangle.right) \
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image); \
}

typedef struct _PICTCode
{
  char
    *name;

  long
    length;

  char
    *description;
} PICTCode;

typedef struct _PICTPixmap
{
  short
    version,
    pack_type;

  unsigned long
    pack_size,
    horizontal_resolution,
    vertical_resolution;

  short
    pixel_type,
    bits_per_pixel,
    component_count,
    component_size;

  unsigned long
    plane_bytes,
    table,
    reserved;
} PICTPixmap;

typedef struct _PICTRectangle
{
  short
    top,
    left,
    bottom,
    right;
} PICTRectangle;

static const PICTCode
  codes[] =
  {
    /* 0x00 */ { (char *) "NOP", 0, (char *) "nop" },
    /* 0x01 */ { (char *) "Clip", 0, (char *) "clip" },
    /* 0x02 */ { (char *) "BkPat", 8, (char *) "background pattern" },
    /* 0x03 */ { (char *) "TxFont", 2, (char *) "text font (word)" },
    /* 0x04 */ { (char *) "TxFace", 1, (char *) "text face (byte)" },
    /* 0x05 */ { (char *) "TxMode", 2, (char *) "text mode (word)" },
    /* 0x06 */ { (char *) "SpExtra", 4, (char *) "space extra (fixed point)" },
    /* 0x07 */ { (char *) "PnSize", 4, (char *) "pen size (point)" },
    /* 0x08 */ { (char *) "PnMode", 2, (char *) "pen mode (word)" },
    /* 0x09 */ { (char *) "PnPat", 8, (char *) "pen pattern" },
    /* 0x0a */ { (char *) "FillPat", 8, (char *) "fill pattern" },
    /* 0x0b */ { (char *) "OvSize", 4, (char *) "oval size (point)" },
    /* 0x0c */ { (char *) "Origin", 4, (char *) "dh, dv (word)" },
    /* 0x0d */ { (char *) "TxSize", 2, (char *) "text size (word)" },
    /* 0x0e */ { (char *) "FgColor", 4, (char *) "foreground color (longword)" },
    /* 0x0f */ { (char *) "BkColor", 4, (char *) "background color (longword)" },
    /* 0x10 */ { (char *) "TxRatio", 8, (char *) "numerator (point), denominator (point)" },
    /* 0x11 */ { (char *) "Version", 1, (char *) "version (byte)" },
    /* 0x12 */ { (char *) "BkPixPat", 0, (char *) "color background pattern" },
    /* 0x13 */ { (char *) "PnPixPat", 0, (char *) "color pen pattern" },
    /* 0x14 */ { (char *) "FillPixPat", 0, (char *) "color fill pattern" },
    /* 0x15 */ { (char *) "PnLocHFrac", 2, (char *) "fractional pen position" },
    /* 0x16 */ { (char *) "ChExtra", 2, (char *) "extra for each character" },
    /* 0x17 */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x18 */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x19 */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x1a */ { (char *) "RGBFgCol", 6, (char *) "RGB foreColor" },
    /* 0x1b */ { (char *) "RGBBkCol", 6, (char *) "RGB backColor" },
    /* 0x1c */ { (char *) "HiliteMode", 0, (char *) "hilite mode flag" },
    /* 0x1d */ { (char *) "HiliteColor", 6, (char *) "RGB hilite color" },
    /* 0x1e */ { (char *) "DefHilite", 0, (char *) "Use default hilite color" },
    /* 0x1f */ { (char *) "OpColor", 6, (char *) "RGB OpColor for arithmetic modes" },
    /* 0x20 */ { (char *) "Line", 8, (char *) "pnLoc (point), newPt (point)" },
    /* 0x21 */ { (char *) "LineFrom", 4, (char *) "newPt (point)" },
    /* 0x22 */ { (char *) "ShortLine", 6, (char *) "pnLoc (point, dh, dv (-128 .. 127))" },
    /* 0x23 */ { (char *) "ShortLineFrom", 2, (char *) "dh, dv (-128 .. 127)" },
    /* 0x24 */ { (char *) "reserved", -1, (char *) "reserved for Apple use" },
    /* 0x25 */ { (char *) "reserved", -1, (char *) "reserved for Apple use" },
    /* 0x26 */ { (char *) "reserved", -1, (char *) "reserved for Apple use" },
    /* 0x27 */ { (char *) "reserved", -1, (char *) "reserved for Apple use" },
    /* 0x28 */ { (char *) "LongText", 0, (char *) "txLoc (point), count (0..255), text" },
    /* 0x29 */ { (char *) "DHText", 0, (char *) "dh (0..255), count (0..255), text" },
    /* 0x2a */ { (char *) "DVText", 0, (char *) "dv (0..255), count (0..255), text" },
    /* 0x2b */ { (char *) "DHDVText", 0, (char *) "dh, dv (0..255), count (0..255), text" },
    /* 0x2c */ { (char *) "reserved", -1, (char *) "reserved for Apple use" },
    /* 0x2d */ { (char *) "reserved", -1, (char *) "reserved for Apple use" },
    /* 0x2e */ { (char *) "reserved", -1, (char *) "reserved for Apple use" },
    /* 0x2f */ { (char *) "reserved", -1, (char *) "reserved for Apple use" },
    /* 0x30 */ { (char *) "frameRect", 8, (char *) "rect" },
    /* 0x31 */ { (char *) "paintRect", 8, (char *) "rect" },
    /* 0x32 */ { (char *) "eraseRect", 8, (char *) "rect" },
    /* 0x33 */ { (char *) "invertRect", 8, (char *) "rect" },
    /* 0x34 */ { (char *) "fillRect", 8, (char *) "rect" },
    /* 0x35 */ { (char *) "reserved", 8, (char *) "reserved for Apple use" },
    /* 0x36 */ { (char *) "reserved", 8, (char *) "reserved for Apple use" },
    /* 0x37 */ { (char *) "reserved", 8, (char *) "reserved for Apple use" },
    /* 0x38 */ { (char *) "frameSameRect", 0, (char *) "rect" },
    /* 0x39 */ { (char *) "paintSameRect", 0, (char *) "rect" },
    /* 0x3a */ { (char *) "eraseSameRect", 0, (char *) "rect" },
    /* 0x3b */ { (char *) "invertSameRect", 0, (char *) "rect" },
    /* 0x3c */ { (char *) "fillSameRect", 0, (char *) "rect" },
    /* 0x3d */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x3e */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x3f */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x40 */ { (char *) "frameRRect", 8, (char *) "rect" },
    /* 0x41 */ { (char *) "paintRRect", 8, (char *) "rect" },
    /* 0x42 */ { (char *) "eraseRRect", 8, (char *) "rect" },
    /* 0x43 */ { (char *) "invertRRect", 8, (char *) "rect" },
    /* 0x44 */ { (char *) "fillRRrect", 8, (char *) "rect" },
    /* 0x45 */ { (char *) "reserved", 8, (char *) "reserved for Apple use" },
    /* 0x46 */ { (char *) "reserved", 8, (char *) "reserved for Apple use" },
    /* 0x47 */ { (char *) "reserved", 8, (char *) "reserved for Apple use" },
    /* 0x48 */ { (char *) "frameSameRRect", 0, (char *) "rect" },
    /* 0x49 */ { (char *) "paintSameRRect", 0, (char *) "rect" },
    /* 0x4a */ { (char *) "eraseSameRRect", 0, (char *) "rect" },
    /* 0x4b */ { (char *) "invertSameRRect", 0, (char *) "rect" },
    /* 0x4c */ { (char *) "fillSameRRect", 0, (char *) "rect" },
    /* 0x4d */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x4e */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x4f */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x50 */ { (char *) "frameOval", 8, (char *) "rect" },
    /* 0x51 */ { (char *) "paintOval", 8, (char *) "rect" },
    /* 0x52 */ { (char *) "eraseOval", 8, (char *) "rect" },
    /* 0x53 */ { (char *) "invertOval", 8, (char *) "rect" },
    /* 0x54 */ { (char *) "fillOval", 8, (char *) "rect" },
    /* 0x55 */ { (char *) "reserved", 8, (char *) "reserved for Apple use" },
    /* 0x56 */ { (char *) "reserved", 8, (char *) "reserved for Apple use" },
    /* 0x57 */ { (char *) "reserved", 8, (char *) "reserved for Apple use" },
    /* 0x58 */ { (char *) "frameSameOval", 0, (char *) "rect" },
    /* 0x59 */ { (char *) "paintSameOval", 0, (char *) "rect" },
    /* 0x5a */ { (char *) "eraseSameOval", 0, (char *) "rect" },
    /* 0x5b */ { (char *) "invertSameOval", 0, (char *) "rect" },
    /* 0x5c */ { (char *) "fillSameOval", 0, (char *) "rect" },
    /* 0x5d */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x5e */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x5f */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x60 */ { (char *) "frameArc", 12, (char *) "rect, startAngle, arcAngle" },
    /* 0x61 */ { (char *) "paintArc", 12, (char *) "rect, startAngle, arcAngle" },
    /* 0x62 */ { (char *) "eraseArc", 12, (char *) "rect, startAngle, arcAngle" },
    /* 0x63 */ { (char *) "invertArc", 12, (char *) "rect, startAngle, arcAngle" },
    /* 0x64 */ { (char *) "fillArc", 12, (char *) "rect, startAngle, arcAngle" },
    /* 0x65 */ { (char *) "reserved", 12, (char *) "reserved for Apple use" },
    /* 0x66 */ { (char *) "reserved", 12, (char *) "reserved for Apple use" },
    /* 0x67 */ { (char *) "reserved", 12, (char *) "reserved for Apple use" },
    /* 0x68 */ { (char *) "frameSameArc", 4, (char *) "rect, startAngle, arcAngle" },
    /* 0x69 */ { (char *) "paintSameArc", 4, (char *) "rect, startAngle, arcAngle" },
    /* 0x6a */ { (char *) "eraseSameArc", 4, (char *) "rect, startAngle, arcAngle" },
    /* 0x6b */ { (char *) "invertSameArc", 4, (char *) "rect, startAngle, arcAngle" },
    /* 0x6c */ { (char *) "fillSameArc", 4, (char *) "rect, startAngle, arcAngle" },
    /* 0x6d */ { (char *) "reserved", 4, (char *) "reserved for Apple use" },
    /* 0x6e */ { (char *) "reserved", 4, (char *) "reserved for Apple use" },
    /* 0x6f */ { (char *) "reserved", 4, (char *) "reserved for Apple use" },
    /* 0x70 */ { (char *) "framePoly", 0, (char *) "poly" },
    /* 0x71 */ { (char *) "paintPoly", 0, (char *) "poly" },
    /* 0x72 */ { (char *) "erasePoly", 0, (char *) "poly" },
    /* 0x73 */ { (char *) "invertPoly", 0, (char *) "poly" },
    /* 0x74 */ { (char *) "fillPoly", 0, (char *) "poly" },
    /* 0x75 */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x76 */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x77 */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x78 */ { (char *) "frameSamePoly", 0, (char *) "poly (NYI)" },
    /* 0x79 */ { (char *) "paintSamePoly", 0, (char *) "poly (NYI)" },
    /* 0x7a */ { (char *) "eraseSamePoly", 0, (char *) "poly (NYI)" },
    /* 0x7b */ { (char *) "invertSamePoly", 0, (char *) "poly (NYI)" },
    /* 0x7c */ { (char *) "fillSamePoly", 0, (char *) "poly (NYI)" },
    /* 0x7d */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x7e */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x7f */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x80 */ { (char *) "frameRgn", 0, (char *) "region" },
    /* 0x81 */ { (char *) "paintRgn", 0, (char *) "region" },
    /* 0x82 */ { (char *) "eraseRgn", 0, (char *) "region" },
    /* 0x83 */ { (char *) "invertRgn", 0, (char *) "region" },
    /* 0x84 */ { (char *) "fillRgn", 0, (char *) "region" },
    /* 0x85 */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x86 */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x87 */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x88 */ { (char *) "frameSameRgn", 0, (char *) "region (NYI)" },
    /* 0x89 */ { (char *) "paintSameRgn", 0, (char *) "region (NYI)" },
    /* 0x8a */ { (char *) "eraseSameRgn", 0, (char *) "region (NYI)" },
    /* 0x8b */ { (char *) "invertSameRgn", 0, (char *) "region (NYI)" },
    /* 0x8c */ { (char *) "fillSameRgn", 0, (char *) "region (NYI)" },
    /* 0x8d */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x8e */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x8f */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x90 */ { (char *) "BitsRect", 0, (char *) "copybits, rect clipped" },
    /* 0x91 */ { (char *) "BitsRgn", 0, (char *) "copybits, rgn clipped" },
    /* 0x92 */ { (char *) "reserved", -1, (char *) "reserved for Apple use" },
    /* 0x93 */ { (char *) "reserved", -1, (char *) "reserved for Apple use" },
    /* 0x94 */ { (char *) "reserved", -1, (char *) "reserved for Apple use" },
    /* 0x95 */ { (char *) "reserved", -1, (char *) "reserved for Apple use" },
    /* 0x96 */ { (char *) "reserved", -1, (char *) "reserved for Apple use" },
    /* 0x97 */ { (char *) "reserved", -1, (char *) "reserved for Apple use" },
    /* 0x98 */ { (char *) "PackBitsRect", 0, (char *) "packed copybits, rect clipped" },
    /* 0x99 */ { (char *) "PackBitsRgn", 0, (char *) "packed copybits, rgn clipped" },
    /* 0x9a */ { (char *) "DirectBitsRect", 0, (char *) "PixMap, srcRect, dstRect, mode, PixData" },
    /* 0x9b */ { (char *) "DirectBitsRgn", 0, (char *) "PixMap, srcRect, dstRect, mode, maskRgn, PixData" },
    /* 0x9c */ { (char *) "reserved", -1, (char *) "reserved for Apple use" },
    /* 0x9d */ { (char *) "reserved", -1, (char *) "reserved for Apple use" },
    /* 0x9e */ { (char *) "reserved", -1, (char *) "reserved for Apple use" },
    /* 0x9f */ { (char *) "reserved", -1, (char *) "reserved for Apple use" },
    /* 0xa0 */ { (char *) "ShortComment", 2, (char *) "kind (word)" },
    /* 0xa1 */ { (char *) "LongComment", 0, (char *) "kind (word), size (word), data" }
  };

/*
  Forward declarations.
*/
static unsigned int
  WritePICTImage(const ImageInfo *,Image *);

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
%  Method DecodeImage decompresses an image via Macintosh pack bits
%  decoding for Macintosh PICT images.
%
%  The format of the DecodeImage method is:
%
%      unsigned char* DecodeImage(const ImageInfo *image_info,Image *blob,
%        Image *image,unsigned long bytes_per_line,const int bits_per_pixel)
%
%  A description of each parameter follows:
%
%    o status:  Method DecodeImage returns True if all the pixels are
%      uncompressed without error, otherwise False.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o blob,image: The address of a structure of type Image.
%
%    o bytes_per_line: This integer identifies the number of bytes in a
%      scanline.
%
%    o bits_per_pixel: The number of bits in a pixel.
%
%
*/

static unsigned char *ExpandBuffer(unsigned char *pixels,
  unsigned long *bytes_per_line,const unsigned int bits_per_pixel)
{
  register long
    i;

  register unsigned char
    *p,
    *q;

  static unsigned char
    scanline[8*256];

  p=pixels;
  q=scanline;
  switch (bits_per_pixel)
  {
    case 8:
    case 16:
    case 32:
      return(pixels);
    case 4:
    {
      for (i=0; i < (long) *bytes_per_line; i++)
      {
        *q++=(*p >> 4U) & 0xffU;
        *q++=(*p & 15U);
        p++;
      }
      *bytes_per_line*=2;
      break;
    }
    case 2:
    {
      for (i=0; i < (long) *bytes_per_line; i++)
      {
        *q++=(*p >> 6U) & 0x03U;
        *q++=(*p >> 4U) & 0x03U;
        *q++=(*p >> 2U) & 0x03U;
        *q++=(*p & 3U);
        p++;
      }
      *bytes_per_line*=4;
      break;
    }
    case 1:
    {
      for (i=0; i < (long) *bytes_per_line; i++)
      {
        *q++=(*p >> 7) & 0x01;
        *q++=(*p >> 6) & 0x01;
        *q++=(*p >> 5) & 0x01;
        *q++=(*p >> 4) & 0x01;
        *q++=(*p >> 3) & 0x01;
        *q++=(*p >> 2) & 0x01;
        *q++=(*p >> 1) & 0x01;
        *q++=(*p & 0x01);
        p++;
      }
      *bytes_per_line*=8;
      break;
    }
    default:
      break;
  }
  return(scanline);
}

static unsigned char *DecodeImage(const ImageInfo *ARGUNUSED(image_info),
  Image *blob,Image *image,unsigned long bytes_per_line,
  const unsigned int bits_per_pixel)
{
  long
    j,
    y;

  register long
    i;

  register unsigned char
    *p,
    *q;

  size_t
    allocated_pixels,
    length,
    row_bytes;

  unsigned char
    *pixels,
    *scanline;

  unsigned long
    bytes_per_pixel,
    number_pixels,
    scanline_length,
    width;

  /*
    Determine pixel buffer size.
  */
  if (bits_per_pixel <= 8)
    bytes_per_line&=0x7fff;
  width=image->columns;
  bytes_per_pixel=1;
  if (bits_per_pixel == 16)
    {
      bytes_per_pixel=2;
      width*=2;
    }
  else
    if (bits_per_pixel == 32)
      width*=image->matte ? 4 : 3;
  if (bytes_per_line == 0)
    bytes_per_line=width;
  row_bytes=(size_t) (image->columns | 0x8000);
  if (image->storage_class == DirectClass)
    row_bytes=(size_t) ((4*image->columns) | 0x8000);
  /*
    Allocate pixel and scanline buffer.
  */
  pixels=MagickAllocateArray(unsigned char *,image->rows,row_bytes);
  if (pixels == (unsigned char *) NULL)
    return((unsigned char *) NULL);
  allocated_pixels=image->rows*row_bytes;
  (void) memset(pixels,0,allocated_pixels);
  scanline=MagickAllocateMemory(unsigned char *,row_bytes);
  if (scanline == (unsigned char *) NULL)
    return((unsigned char *) NULL);
  if (bytes_per_line < 8)
    {
      /*
        Pixels are already uncompressed.
      */
      for (y=0; y < (long) image->rows; y++)
      {
        q=pixels+y*width;
        number_pixels=bytes_per_line;
        (void) ReadBlob(blob,number_pixels,(char *) scanline);
        p=ExpandBuffer(scanline,&number_pixels,bits_per_pixel);
        (void) memcpy(q,p,number_pixels);
      }
      MagickFreeMemory(scanline);
      return(pixels);
    }
  /*
    Uncompress RLE pixels into uncompressed pixel buffer.
  */
  for (y=0; y < (long) image->rows; y++)
  {
    q=pixels+y*width;
    if (bytes_per_line > 200)
      scanline_length=ReadBlobMSBShort(blob);
    else
      scanline_length=ReadBlobByte(blob);
    if (scanline_length >= row_bytes)
      {
        ThrowException(&image->exception,CorruptImageError,UnableToUncompressImage,
                       "scanline length exceeds row bytes");
        break;
      }
    (void) ReadBlob(blob,scanline_length,(char *) scanline);
    for (j=0; j < (long) scanline_length; )
      if ((scanline[j] & 0x80) == 0)
        {
          length=(scanline[j] & 0xff)+1;
          number_pixels=length*bytes_per_pixel;
          p=ExpandBuffer(scanline+j+1,&number_pixels,bits_per_pixel);
          if ((q+number_pixels > pixels+allocated_pixels))
            {
              ThrowException(&image->exception,CorruptImageError,UnableToUncompressImage,
                             "Decoded RLE pixels exceeds allocation!");
              break;
            }

          (void) memcpy(q,p,number_pixels);
          q+=number_pixels;
          j+=length*bytes_per_pixel+1;
        }
      else
        {
          length=((scanline[j]^0xff) & 0xff)+2;
          number_pixels=bytes_per_pixel;
          p=ExpandBuffer(scanline+j+1,&number_pixels,bits_per_pixel);
          for (i=0; i < (long) length; i++)
          {
          if ((q+number_pixels > pixels+allocated_pixels))
            {
              ThrowException(&image->exception,CorruptImageError,UnableToUncompressImage,
                             "Decoded RLE pixels exceeds allocation!");
              break;
            }
            (void) memcpy(q,p,number_pixels);
            q+=number_pixels;
          }
          j+=bytes_per_pixel+1;
        }
  }
  MagickFreeMemory(scanline);
  return(pixels);
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
%  Method EncodeImage compresses an image via Macintosh pack bits encoding
%  for Macintosh PICT images.
%
%  The format of the EncodeImage method is:
%
%      size_t EncodeImage(Image *image,const unsigned char *scanline,
%        const unsigned long bytes_per_line,unsigned char *pixels)
%
%  A description of each parameter follows:
%
%    o status:  Method EncodeImage returns the number of encoded pixels.
%
%    o image: The address of a structure of type Image.
%
%    o scanline: A pointer to an array of characters to pack.
%
%    o bytes_per_line: The number of bytes in a scanline.
%
%    o pixels: A pointer to an array of characters where the packed
%      characters are stored.
%
%
*/
static size_t EncodeImage(Image *image,const unsigned char *scanline,
  const unsigned long bytes_per_line,unsigned char *pixels)
{
#define MaxCount  128U
#define MaxPackbitsRunlength  128

  long
    count,
    repeat_count,
    runlength;

  register const unsigned char
    *p;

  register long
    i;

  register unsigned char
    *q;

  size_t
    length;

  unsigned char
    index;

  /*
    Pack scanline.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(scanline != (unsigned char *) NULL);
  assert(pixels != (unsigned char *) NULL);
  count=0;
  runlength=0;
  p=scanline+(bytes_per_line-1);
  q=pixels;
  index=(*p);
  for (i=(long) bytes_per_line-1; i >= 0; i--)
  {
    if (index == *p)
      runlength++;
    else
      {
        if (runlength < 3)
          while (runlength > 0)
          {
            *q++=(unsigned char) index;
            runlength--;
            count++;
            if (count == MaxCount)
              {
                *q++=(unsigned char) (MaxCount-1);
                count-=MaxCount;
              }
          }
        else
          {
            if (count > 0)
              *q++=(unsigned char) (count-1);
            count=0;
            while (runlength > 0)
            {
              repeat_count=runlength;
              if (repeat_count > MaxPackbitsRunlength)
                repeat_count=MaxPackbitsRunlength;
              *q++=(unsigned char) index;
              *q++=(unsigned char) (257-repeat_count);
              runlength-=repeat_count;
            }
          }
        runlength=1;
      }
    index=(*p);
    p--;
  }
  if (runlength < 3)
    while (runlength > 0)
    {
      *q++=(unsigned char) index;
      runlength--;
      count++;
      if (count == MaxCount)
        {
          *q++=(unsigned char) (MaxCount-1);
          count-=MaxCount;
        }
    }
  else
    {
      if (count > 0)
        *q++=(unsigned char) (count-1);
      count=0;
      while (runlength > 0)
      {
        repeat_count=runlength;
        if (repeat_count > MaxPackbitsRunlength)
          repeat_count=MaxPackbitsRunlength;
        *q++=(unsigned char) index;
        *q++=(unsigned char) (257-repeat_count);
        runlength-=repeat_count;
      }
    }
  if (count > 0)
    *q++= (unsigned char) (count-1);
  /*
    Write the number of and the packed length.
  */
  length=(q-pixels);
  if (bytes_per_line > 200)
    {
      (void) WriteBlobMSBShort(image,length);
      length+=2;
    }
  else
    {
      (void) WriteBlobByte(image,(long) length);
      length++;
    }
  while (q != pixels)
  {
    q--;
    (void) WriteBlobByte(image,*q);
  }
  return(length);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d P I C T I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPICTImage reads an Apple Macintosh QuickDraw/PICT image file
%  and returns it.  It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.
%
%  The format of the ReadPICTImage method is:
%
%      Image *ReadPICTImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadPICTImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadPICTImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  char
    geometry[MaxTextExtent];

  Image
    *image;

  IndexPacket
    index;

  int
    c,
    code;

  long
    flags,
    j,
    version,
    y;

  PICTRectangle
    frame;

  PICTPixmap
    pixmap;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *q;

  register long
    i;

  size_t
    length;

  unsigned int
    jpeg,
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
  pixmap.bits_per_pixel=0;
  pixmap.component_count=0;
  /*
    Read PICT header.
  */
  pixmap.bits_per_pixel=0;
  pixmap.component_count=0;
  for (i=0; i < 512; i++)
    (void) ReadBlobByte(image);  /* skip header */
  (void) ReadBlobMSBShort(image);  /* skip picture size */
  ReadRectangle(frame);
  while ((c=ReadBlobByte(image)) == 0);
  if (c != 0x11)
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
  version=ReadBlobByte(image);
  if (version == 2)
    {
      c=ReadBlobByte(image);
      if (c != 0xff)
        ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
    }
  else
    if (version != 1)
      ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
  /*
    Validate dimensions
  */
  if ((frame.left < 0) || (frame.right < 0) || (frame.top < 0) || (frame.bottom < 0) ||
      (frame.left >= frame.right) || (frame.top >= frame.bottom))
    {
      ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
    }

  /*
    Create black canvas.
  */
  flags=0;
  image->columns=frame.right-frame.left;
  image->rows=frame.bottom-frame.top;

  if (IsEventLogging())
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "Dimensions: %lux%lu",image->columns,image->rows);

  /*
    Interpret PICT opcodes.
  */
  jpeg=False;
  for (code=0; EOFBlob(image) == False; )
  {
    if (image_info->ping && (image_info->subrange != 0))
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    if ((version == 1) || ((TellBlob(image) % 2) != 0))
      code=ReadBlobByte(image);
    if (version == 2)
      code=ReadBlobMSBShort(image);
    if (code > 0xa1)
      {
        if (IsEventLogging())
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Code %04X:",code);
      }
    else
      {
        if (IsEventLogging())
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Code  %04X %.1024s: %.1024s",code,
            codes[code].name,codes[code].description);
        switch (code)
        {
          case 0x01:
          {
            /*
              Clipping rectangle.
            */
            length=ReadBlobMSBShort(image);
            if (length != 0x000a)
              {
                for (i=0; i < (long) (length-2); i++)
                  (void) ReadBlobByte(image);
                break;
              }
            ReadRectangle(frame);
            if ((frame.left & 0x8000) || (frame.top & 0x8000))
              break;
            image->columns=frame.right-frame.left;
            image->rows=frame.bottom-frame.top;
            (void) SetImage(image,OpaqueOpacity);
            break;
          }
          case 0x12:
          case 0x13:
          case 0x14:
          {
            long
              pattern;

            unsigned long
              height,
              width;

            /*
              Skip pattern definition.
            */
            pattern=ReadBlobMSBShort(image);
            for (i=0; i < 8; i++)
              (void) ReadBlobByte(image);
            if (pattern == 2)
              {
                for (i=0; i < 5; i++)
                  (void) ReadBlobByte(image);
                break;
              }
            if (pattern != 1)
              ThrowReaderException(CorruptImageError,UnknownPatternType,
                image);
            length=ReadBlobMSBShort(image);
            ReadRectangle(frame);
            ReadPixmap(pixmap);
            (void) ReadBlobMSBLong(image);
            flags=ReadBlobMSBShort(image);
            length=ReadBlobMSBShort(image);
            for (i=0; i <= (long) length; i++)
              (void) ReadBlobMSBLong(image);
            width=frame.bottom-frame.top;
            height=frame.right-frame.left;
            image->depth=pixmap.bits_per_pixel <= 8 ? 8 : QuantumDepth;
            if (pixmap.bits_per_pixel < 8)
              image->depth=8;
            if (pixmap.bits_per_pixel <= 8)
              length&=0x7fff;
            if (pixmap.bits_per_pixel == 16)
              width<<=1;
            if (length == 0)
              length=width;
            if (length < 8)
              {
                for (i=0; i < (long) (length*height); i++)
                  (void) ReadBlobByte(image);
              }
            else
              for (j=0; j < (int) height; j++)
                if (length > 200)
                  for (j=0; j < ReadBlobMSBShort(image); j++)
                    (void) ReadBlobByte(image);
                else
                  for (j=0; j < ReadBlobByte(image); j++)
                    (void) ReadBlobByte(image);
            break;
          }
          case 0x1b:
          {
            /*
              Initialize image background color.
            */
            image->background_color.red=(Quantum)
              ScaleShortToQuantum(ReadBlobMSBShort(image));
            image->background_color.green=(Quantum)
              ScaleShortToQuantum(ReadBlobMSBShort(image));
            image->background_color.blue=(Quantum)
              ScaleShortToQuantum(ReadBlobMSBShort(image));
            break;
          }
          case 0x70:
          case 0x71:
          case 0x72:
          case 0x73:
          case 0x74:
          case 0x75:
          case 0x76:
          case 0x77:
          {
            /*
              Skip polygon or region.
            */
            length=ReadBlobMSBShort(image);
            for (i=0; i < (long) (length-2); i++)
              (void) ReadBlobByte(image);
            break;
          }
          case 0x90:
          case 0x91:
          case 0x98:
          case 0x99:
          case 0x9a:
          case 0x9b:
          {
            long
              bytes_per_line;

            PICTRectangle
              source,
              destination;

            register unsigned char
              *p;

            size_t
              j;

            unsigned char
              *pixels;

            Image
              *tile_image;

            /*
              Pixmap clipped by a rectangle.
            */
            bytes_per_line=0;
            if ((code != 0x9a) && (code != 0x9b))
              bytes_per_line=ReadBlobMSBShort(image);
            else
              {
                (void) ReadBlobMSBShort(image);
                (void) ReadBlobMSBShort(image);
                (void) ReadBlobMSBShort(image);
              }
            ReadRectangle(frame);
            /*
              Initialize tile image.
            */
            tile_image=CloneImage(image,frame.right-frame.left,
              frame.bottom-frame.top,True,exception);
            if (tile_image == (Image *) NULL)
              return((Image *) NULL);
            DestroyBlob(tile_image);
            tile_image->blob=CloneBlobInfo((BlobInfo *) NULL);
            if ((code == 0x9a) || (code == 0x9b) || (bytes_per_line & 0x8000))
              {
                ReadPixmap(pixmap);
                tile_image->matte=pixmap.component_count == 4;
              }
            if ((code != 0x9a) && (code != 0x9b))
              {
                /*
                  Initialize colormap.
                */
                tile_image->colors=2;
                if (bytes_per_line & 0x8000)
                  {
                    (void) ReadBlobMSBLong(image);
                    flags=ReadBlobMSBShort(image);
                    tile_image->colors=ReadBlobMSBShort(image)+1;
                  }
                if (!AllocateImageColormap(tile_image,tile_image->colors))
                  {
                    DestroyImage(tile_image);
                    ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image)
                  }
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                  "Allocated tile image colormap with %u colors",tile_image->colors);
                if (bytes_per_line & 0x8000)
                  {
                    for (i=0; i < (long) tile_image->colors; i++)
                    {
                      j=ReadBlobMSBShort(image) % tile_image->colors;
                      if (flags & 0x8000)
                        j=i;
                      tile_image->colormap[j].red=(Quantum)
                        ScaleShortToQuantum(ReadBlobMSBShort(image));
                      tile_image->colormap[j].green=(Quantum)
                        ScaleShortToQuantum(ReadBlobMSBShort(image));
                      tile_image->colormap[j].blue=(Quantum)
                        ScaleShortToQuantum(ReadBlobMSBShort(image));
                    }
                  }
                else
                  {
                    for (i=0; i < (long) tile_image->colors; i++)
                    {
                      tile_image->colormap[i].red=(Quantum) (MaxRGB-
                        tile_image->colormap[i].red);
                      tile_image->colormap[i].green=(Quantum) (MaxRGB-
                        tile_image->colormap[i].green);
                      tile_image->colormap[i].blue=(Quantum) (MaxRGB-
                        tile_image->colormap[i].blue);
                    }
                  }
              }
            ReadRectangle(source);
            ReadRectangle(destination);
            (void) ReadBlobMSBShort(image);
            if ((code == 0x91) || (code == 0x99) || (code == 0x9b))
              {
                /*
                  Skip region.
                */
                length=ReadBlobMSBShort(image);
                for (i=0; i <= (long) (length-2); i++)
                  (void) ReadBlobByte(image);
              }
            if ((code != 0x9a) && (code != 0x9b) &&
                (bytes_per_line & 0x8000) == 0)
              pixels=DecodeImage(image_info,image,tile_image,bytes_per_line,1);
            else
              pixels=DecodeImage(image_info,image,tile_image,bytes_per_line,
                pixmap.bits_per_pixel);
            if (pixels == (unsigned char *) NULL)
              {
                DestroyImage(tile_image);
                ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image)
              }
            /*
              Convert PICT tile image to pixel packets.
            */
            p=pixels;
            for (y=0; y < (long) tile_image->rows; y++)
            {
              q=SetImagePixels(tile_image,0,y,tile_image->columns,1);
              if (q == (PixelPacket *) NULL)
                break;
              indexes=AccessMutableIndexes(tile_image);
              for (x=0; x < (long) tile_image->columns; x++)
              {
                if (tile_image->storage_class == PseudoClass)
                  {
                    index=(IndexPacket) (*p);
                    VerifyColormapIndex(tile_image,index);
                    indexes[x]=index;
                    q->red=tile_image->colormap[index].red;
                    q->green=tile_image->colormap[index].green;
                    q->blue=tile_image->colormap[index].blue;
                  }
                else
                  {
                    if (pixmap.bits_per_pixel == 16)
                      {
                        i=(*p++);
                        j=(*p);
                        q->red=ScaleCharToQuantum((i & 0x7c) << 1);
                        q->green=ScaleCharToQuantum(((i & 0x03) << 6) |
                          ((j & 0xe0) >> 2));
                        q->blue=ScaleCharToQuantum((j & 0x1f) << 3);
                      }
                    else
                      if (!tile_image->matte)
                        {
                          q->red=ScaleCharToQuantum(*p);
                          q->green=
                            ScaleCharToQuantum(*(p+tile_image->columns));
                          q->blue=ScaleCharToQuantum(*(p+2*tile_image->columns));
                        }
                      else
                        {
                          q->opacity=(Quantum) (MaxRGB-ScaleCharToQuantum(*p));
                          q->red=ScaleCharToQuantum(*(p+tile_image->columns));
                          q->green=(Quantum)
                            ScaleCharToQuantum(*(p+2*tile_image->columns));
                          q->blue=
                           ScaleCharToQuantum(*(p+3*tile_image->columns));
                        }
                  }
                p++;
                q++;
              }
              if (!SyncImagePixels(tile_image))
                break;
              if ((tile_image->storage_class == DirectClass) &&
                  (pixmap.bits_per_pixel != 16))
                p+=(pixmap.component_count-1)*tile_image->columns;
              if (destination.bottom == (long) image->rows)
                if (QuantumTick(y,tile_image->rows))
                  if (!MagickMonitorFormatted(y,tile_image->rows,&image->exception,
                                              LoadImageText,image->filename))
                    break;
            }
            MagickFreeMemory(pixels);
            if (jpeg == False)
              if ((code == 0x9a) || (code == 0x9b) ||
                  (bytes_per_line & 0x8000))
                (void) CompositeImage(image,CopyCompositeOp,tile_image,
                   destination.left,destination.top);
                DestroyImage(tile_image);
            if (destination.bottom != (long) image->rows)
              if (!MagickMonitorFormatted(destination.bottom,image->rows,&image->exception,
                                          LoadImageText,image->filename))
                break;
            break;
          }
          case 0xa1:
          {
            unsigned char
              *info;

            unsigned int
              type;

            /*
              Comment.
            */
            type=ReadBlobMSBShort(image);
            length=ReadBlobMSBShort(image);
            if (length == 0)
              break;
            (void) ReadBlobMSBLong(image);
            length-=4;
            if (length == 0)
              break;
            info=MagickAllocateMemory(unsigned char *,length);
            if (info == (unsigned char *) NULL)
              break;
            (void) ReadBlob(image,length,info);
            switch (type)
            {
              case 0xe0:
              {
                if (length == 0)
                  break;
                if (SetImageProfile(image,"ICM",info,length) == MagickFail)
                  ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
                MagickFreeMemory(info);
                break;
              }
              case 0x1f2:
              {
                if (length == 0)
                  break;
                if (SetImageProfile(image,"IPTC",info,length) == MagickFail)
                  ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
                MagickFreeMemory(info);
                break;
              }
              default:
                break;
            }
            MagickFreeMemory(info);
            break;
          }
          default:
          {
            /*
              Skip to next op code.
            */
            if (codes[code].length == -1)
              (void) ReadBlobMSBShort(image);
            else
              for (i=0; i < (long) codes[code].length; i++)
                (void) ReadBlobByte(image);
          }
        }
      }
    if (code == 0xc00)
      {
        /*
          Skip header.
        */
        for (i=0; i < 24; i++)
          (void) ReadBlobByte(image);
        continue;
      }
    if (((code >= 0xb0) && (code <= 0xcf)) ||
        ((code >= 0x8000) && (code <= 0x80ff)))
      continue;
    if (code == 0x8200)
      {
        FILE
          *file;

        Image
          *tile_image;

        ImageInfo
          *clone_info;

        /*
          Embedded JPEG.
        */
        jpeg=True;
        clone_info=CloneImageInfo(image_info);
        clone_info->blob=(void *) NULL;
        clone_info->length=0;
        file=AcquireTemporaryFileStream(clone_info->filename,BinaryFileIOMode);
        if (file == (FILE *) NULL)
          {
            DestroyImageInfo(clone_info);
            ThrowReaderTemporaryFileException(clone_info->filename);
          }
        length=ReadBlobMSBLong(image);
        for (i=0; i < 6; i++)
          (void) ReadBlobMSBLong(image);
        ReadRectangle(frame);
        for (i=0; i < 122; i++)
          (void) ReadBlobByte(image);
        for (i=0; i < (long) (length-154); i++)
        {
          c=ReadBlobByte(image);
          (void) fputc(c,file);
        }
        (void) fclose(file);
        tile_image=ReadImage(clone_info,exception);
        (void) LiberateTemporaryFile(clone_info->filename);
        DestroyImageInfo(clone_info);
        if (tile_image == (Image *) NULL)
          continue;
        FormatString(geometry,"%lux%lu",Max(image->columns,tile_image->columns),
          Max(image->rows,tile_image->rows));
        (void) TransformImage(&image,(char *) NULL,geometry);
        (void) CompositeImage(image,CopyCompositeOp,tile_image,frame.left,
          frame.right);
        image->compression=tile_image->compression;
        DestroyImage(tile_image);
        continue;
      }
    if ((code == 0xff) || (code == 0xffff))
      break;
    if (((code >= 0xd0) && (code <= 0xfe)) ||
        ((code >= 0x8100) && (code <= 0xffff)))
      {
        /*
          Skip reserved.
        */
        length=ReadBlobMSBShort(image);
        for (i=0; i < (long) length; i++)
          (void) ReadBlobByte(image);
        continue;
      }
    if ((code >= 0x100) && (code <= 0x7fff))
      {
        /*
          Skip reserved.
        */
        length=(code >> 7) & 0xff;
        for (i=0; i < (long) length; i++)
          (void) ReadBlobByte(image);
        continue;
      }
  }
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
%   R e g i s t e r P I C T I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterPICTImage adds attributes for the PICT image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterPICTImage method is:
%
%      RegisterPICTImage(void)
%
*/
ModuleExport void RegisterPICTImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("PCT");
  entry->decoder=(DecoderHandler) ReadPICTImage;
  entry->encoder=(EncoderHandler) WritePICTImage;
  entry->adjoin=False;
  entry->description="Apple Macintosh QuickDraw/PICT";
  entry->module="PICT";
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("PICT");
  entry->decoder=(DecoderHandler) ReadPICTImage;
  entry->encoder=(EncoderHandler) WritePICTImage;
  entry->adjoin=False;
  entry->description="Apple Macintosh QuickDraw/PICT";
  entry->module="PICT";
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r P I C T I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterPICTImage removes format registrations made by the
%  PICT module from the list of supported formats.
%
%  The format of the UnregisterPICTImage method is:
%
%      UnregisterPICTImage(void)
%
*/
ModuleExport void UnregisterPICTImage(void)
{
  (void) UnregisterMagickInfo("PCT");
  (void) UnregisterMagickInfo("PICT");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P I C T I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePICTImage writes an image to a file in the Apple Macintosh
%  QuickDraw/PICT image format.
%
%  The format of the WritePICTImage method is:
%
%      unsigned int WritePICTImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WritePICTImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
static unsigned int WritePICTImage(const ImageInfo *image_info,Image *image)
{
#define MaxCount  128U
#define PictCropRegionOp  0x01
#define PictEndOfPictureOp  0xff
#define PictJPEGOp  0x8200
#define PictInfoOp  0x0C00
#define PictInfoSize  512
#define PictPixmapOp  0x9A
#define PictPICTOp  0x98
#define PictVersion  0x11

  double
    x_resolution,
    y_resolution;

  long
    y;

  ExtendedSignedIntegralType
    offset;

  PICTPixmap
    pixmap;

  PICTRectangle
    bounds,
    crop_rectangle,
    destination_rectangle,
    frame_rectangle,
    size_rectangle,
    source_rectangle;

  const unsigned char
    *profile_info;
  
  size_t
    profile_length;

  register const PixelPacket
    *p;

  register const IndexPacket
    *indexes;

  register long
    i,
    x;

  size_t
    count;

  unsigned char
    *buffer,
    *packed_scanline,
    *scanline;

  unsigned int
    status;

  unsigned long
    bytes_per_line,
    storage_class;

  unsigned short
    base_address,
    row_bytes,
    transfer_mode;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if ((image->columns > 65535L) || (image->rows > 65535L))
    ThrowWriterException(ImageError,WidthOrHeightExceedsLimit,image);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenError,UnableToOpenFile,image);
  (void) TransformColorspace(image,RGBColorspace);
  /*
    Initialize image info.
  */
  size_rectangle.top=0;
  size_rectangle.left=0;
  size_rectangle.bottom=(short) image->rows;
  size_rectangle.right=(short) image->columns;
  frame_rectangle=size_rectangle;
  crop_rectangle=size_rectangle;
  source_rectangle=size_rectangle;
  destination_rectangle=size_rectangle;
  base_address=0xff;
  row_bytes=(unsigned short) (image->columns | 0x8000);
  bounds.top=0;
  bounds.left=0;
  bounds.bottom=(short) image->rows;
  bounds.right=(short) image->columns;
  pixmap.version=0;
  pixmap.pack_type=0;
  pixmap.pack_size=0;
  pixmap.pixel_type=0;
  pixmap.bits_per_pixel=8;
  pixmap.component_count=1;
  pixmap.component_size=8;
  pixmap.plane_bytes=0;
  pixmap.table=0;
  pixmap.reserved=0;
  transfer_mode=0;
  x_resolution=image->x_resolution ? image->x_resolution : 72.0;
  y_resolution=image->y_resolution ? image->y_resolution : 72.0;
  storage_class=image->storage_class;
  if (image->compression == JPEGCompression)
    storage_class=DirectClass;
  if (storage_class == DirectClass)
    {
      pixmap.component_count=image->matte ? 4 : 3;
      pixmap.pixel_type=16;
      pixmap.bits_per_pixel=32;
      pixmap.pack_type=0x04;
      transfer_mode=0x40;
      row_bytes=(unsigned short) ((4*image->columns) | 0x8000);
    }
  /*
    Allocate memory.
  */
  bytes_per_line=image->columns;
  if (storage_class == DirectClass)
    bytes_per_line*=image->matte ? 4 : 3;
  buffer=MagickAllocateMemory(unsigned char *,PictInfoSize);
  packed_scanline=MagickAllocateMemory(unsigned char *,row_bytes+MaxCount);
  scanline=MagickAllocateMemory(unsigned char *,row_bytes);
  if ((buffer == (unsigned char *) NULL) ||
      (packed_scanline == (unsigned char *) NULL) ||
      (scanline == (unsigned char *) NULL))
    ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
  /*
    Write header, header size, size bounding box, version, and reserved.
  */
  (void) memset(buffer,0,PictInfoSize);
  (void) WriteBlob(image,PictInfoSize,buffer);
  (void) WriteBlobMSBShort(image,0);
  (void) WriteBlobMSBShort(image,size_rectangle.top);
  (void) WriteBlobMSBShort(image,size_rectangle.left);
  (void) WriteBlobMSBShort(image,size_rectangle.bottom);
  (void) WriteBlobMSBShort(image,size_rectangle.right);
  (void) WriteBlobMSBShort(image,PictVersion);
  (void) WriteBlobMSBShort(image,0x02ff);  /* version #2 */
  (void) WriteBlobMSBShort(image,PictInfoOp);
  (void) WriteBlobMSBLong(image,0xFFFE0000UL);
  /*
    Write full size of the file, resolution, frame bounding box, and reserved.
  */
  (void) WriteBlobMSBShort(image,(unsigned short) x_resolution);
  (void) WriteBlobMSBShort(image,0x0000);
  (void) WriteBlobMSBShort(image,(unsigned short) y_resolution);
  (void) WriteBlobMSBShort(image,0x0000);
  (void) WriteBlobMSBShort(image,frame_rectangle.top);
  (void) WriteBlobMSBShort(image,frame_rectangle.left);
  (void) WriteBlobMSBShort(image,frame_rectangle.bottom);
  (void) WriteBlobMSBShort(image,frame_rectangle.right);
  (void) WriteBlobMSBLong(image,0x00000000L);
  /*
    Output 8BIM profile.
  */
  profile_info=GetImageProfile(image,"8BIM",&profile_length);
  if (profile_info != (unsigned char *) NULL)
    {
      (void) WriteBlobMSBShort(image,0xa1);
      (void) WriteBlobMSBShort(image,0x1f2);
      (void) WriteBlobMSBShort(image,profile_length+4);
      (void) WriteBlobString(image,"8BIM");
      (void) WriteBlob(image,profile_length,
                       profile_info);
    }
  /*
    Ouput ICM profile.
  */
  profile_info=GetImageProfile(image,"ICM",&profile_length);
  if (profile_info != (unsigned char *) NULL)
    {
      (void) WriteBlobMSBShort(image,0xa1);
      (void) WriteBlobMSBShort(image,0xe0);
      (void) WriteBlobMSBShort(image,profile_length+4);
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlob(image,profile_length,
                       profile_info);
      (void) WriteBlobMSBShort(image,0xa1);
      (void) WriteBlobMSBShort(image,0xe0);
      (void) WriteBlobMSBShort(image,4);
      (void) WriteBlobMSBLong(image,0x00000002UL);
    }
  /*
    Write crop region opcode and crop bounding box.
  */
  (void) WriteBlobMSBShort(image,PictCropRegionOp);
  (void) WriteBlobMSBShort(image,0xa);
  (void) WriteBlobMSBShort(image,crop_rectangle.top);
  (void) WriteBlobMSBShort(image,crop_rectangle.left);
  (void) WriteBlobMSBShort(image,crop_rectangle.bottom);
  (void) WriteBlobMSBShort(image,crop_rectangle.right);
  if (image->compression == JPEGCompression)
    {
      Image
        *jpeg_image;

      size_t
        length;

      unsigned char
        *blob;

      jpeg_image=CloneImage(image,0,0,True,&image->exception);
      if (jpeg_image == (Image *) NULL)
        {
          CloseBlob(image);
          return (False);
        }
      DestroyBlob(jpeg_image);
      jpeg_image->blob=CloneBlobInfo((BlobInfo *) NULL);
      (void) strcpy(jpeg_image->magick,"JPEG");
      blob=(unsigned char *) ImageToBlob(image_info,jpeg_image,&length,
        &image->exception);
      DestroyImage(jpeg_image);
      if (blob == (unsigned char *) NULL)
        return(False);
      (void) WriteBlobMSBShort(image,PictJPEGOp);
      (void) WriteBlobMSBLong(image,length+154);
      (void) WriteBlobMSBShort(image,0x0000);
      (void) WriteBlobMSBLong(image,0x00010000UL);
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlobMSBLong(image,0x00010000UL);
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlobMSBLong(image,0x40000000UL);
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlobMSBLong(image,0x00400000UL);
      (void) WriteBlobMSBShort(image,0x0000);
      (void) WriteBlobMSBShort(image,image->rows);
      (void) WriteBlobMSBShort(image,image->columns);
      (void) WriteBlobMSBShort(image,0x0000);
      (void) WriteBlobMSBShort(image,768);
      (void) WriteBlobMSBShort(image,0x0000);
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlobMSBLong(image,0x00566A70UL);
      (void) WriteBlobMSBLong(image,0x65670000UL);
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlobMSBLong(image,0x00000001UL);
      (void) WriteBlobMSBLong(image,0x00016170UL);
      (void) WriteBlobMSBLong(image,0x706C0000UL);
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlobMSBShort(image,768);
      (void) WriteBlobMSBShort(image,image->columns);
      (void) WriteBlobMSBShort(image,image->rows);
      (void) WriteBlobMSBShort(image,(unsigned short) x_resolution);
      (void) WriteBlobMSBShort(image,0x0000);
      (void) WriteBlobMSBShort(image,(unsigned short) y_resolution);
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlobMSBLong(image,0x87AC0001UL);
      (void) WriteBlobMSBLong(image,0x0B466F74UL);
      (void) WriteBlobMSBLong(image,0x6F202D20UL);
      (void) WriteBlobMSBLong(image,0x4A504547UL);
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlobMSBLong(image,0x0018FFFFUL);
      (void) WriteBlob(image,length,blob);
      if (length & 0x01)
        (void) WriteBlobByte(image,'\0');
      MagickFreeMemory(blob);
    }
  /*
    Write picture opcode, row bytes, and picture bounding box, and version.
  */
  if (storage_class == PseudoClass)
    (void) WriteBlobMSBShort(image,PictPICTOp);
  else
    {
      (void) WriteBlobMSBShort(image,PictPixmapOp);
      (void) WriteBlobMSBLong(image,(unsigned long) base_address);
    }
  (void) WriteBlobMSBShort(image,row_bytes | 0x8000);
  (void) WriteBlobMSBShort(image,bounds.top);
  (void) WriteBlobMSBShort(image,bounds.left);
  (void) WriteBlobMSBShort(image,bounds.bottom);
  (void) WriteBlobMSBShort(image,bounds.right);
  /*
    Write pack type, pack size, resolution, pixel type, and pixel size.
  */
  (void) WriteBlobMSBShort(image,pixmap.version);
  (void) WriteBlobMSBShort(image,pixmap.pack_type);
  (void) WriteBlobMSBLong(image,pixmap.pack_size);
  (void) WriteBlobMSBShort(image,(unsigned short) x_resolution);
  (void) WriteBlobMSBShort(image,0x0000);
  (void) WriteBlobMSBShort(image,(unsigned short) y_resolution);
  (void) WriteBlobMSBShort(image,0x0000);
  (void) WriteBlobMSBShort(image,pixmap.pixel_type);
  (void) WriteBlobMSBShort(image,pixmap.bits_per_pixel);
  /*
    Write component count, size, plane bytes, table size, and reserved.
  */
  (void) WriteBlobMSBShort(image,pixmap.component_count);
  (void) WriteBlobMSBShort(image,pixmap.component_size);
  (void) WriteBlobMSBLong(image,(unsigned long) pixmap.plane_bytes);
  (void) WriteBlobMSBLong(image,(unsigned long) pixmap.table);
  (void) WriteBlobMSBLong(image,(unsigned long) pixmap.reserved);
  if (storage_class == PseudoClass)
    {
      /*
        Write image colormap.
      */
      (void) WriteBlobMSBLong(image,0x00000000L);  /* color seed */
      (void) WriteBlobMSBShort(image,0L);  /* color flags */
      (void) WriteBlobMSBShort(image,(unsigned short) (image->colors-1));
      for (i=0; i < (long) image->colors; i++)
      {
        (void) WriteBlobMSBShort(image,i);
        (void) WriteBlobMSBShort(image,
          ScaleQuantumToShort(image->colormap[i].red));
        (void) WriteBlobMSBShort(image,
          ScaleQuantumToShort(image->colormap[i].green));
        (void) WriteBlobMSBShort(image,
          ScaleQuantumToShort(image->colormap[i].blue));
      }
    }
  /*
    Write source and destination rectangle.
  */
  (void) WriteBlobMSBShort(image,source_rectangle.top);
  (void) WriteBlobMSBShort(image,source_rectangle.left);
  (void) WriteBlobMSBShort(image,source_rectangle.bottom);
  (void) WriteBlobMSBShort(image,source_rectangle.right);
  (void) WriteBlobMSBShort(image,destination_rectangle.top);
  (void) WriteBlobMSBShort(image,destination_rectangle.left);
  (void) WriteBlobMSBShort(image,destination_rectangle.bottom);
  (void) WriteBlobMSBShort(image,destination_rectangle.right);
  (void) WriteBlobMSBShort(image,transfer_mode);
  /*
    Write picture data.
  */
  count=0;
  if (storage_class == PseudoClass)
    for (y=0; y < (long) image->rows; y++)
    {
      p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
      if (p == (const PixelPacket *) NULL)
        break;
      indexes=AccessImmutableIndexes(image);
      for (x=0; x < (long) image->columns; x++)
        scanline[x]=indexes[x];
      count+=EncodeImage(image,scanline,row_bytes & 0x7FFF,packed_scanline);
      if (QuantumTick(y,image->rows))
        if (!MagickMonitorFormatted(y,image->rows,&image->exception,
                                    SaveImageText,image->filename))
          break;
    }
  else
    if (image->compression == JPEGCompression)
      {
        (void) memset(scanline,0,row_bytes);
        for (y=0; y < (long) image->rows; y++)
          count+=EncodeImage(image,scanline,row_bytes & 0x7FFF,packed_scanline);
      }
    else
      {
        register unsigned char
          *blue,
          *green,
          *opacity,
          *red;

        red=scanline;
        green=scanline+image->columns;
        blue=scanline+2*image->columns;
        opacity=scanline+3*image->columns;
        for (y=0; y < (long) image->rows; y++)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          red=scanline;
          green=scanline+image->columns;
          blue=scanline+2*image->columns;
          if (image->matte)
            {
              opacity=scanline;
              red=scanline+image->columns;
              green=scanline+2*image->columns;
              blue=scanline+3*image->columns;
            }
          for (x=0; x < (long) image->columns; x++)
          {
            *red++=ScaleQuantumToChar(p->red);
            *green++=ScaleQuantumToChar(p->green);
            *blue++=ScaleQuantumToChar(p->blue);
            if (image->matte)
              *opacity++=ScaleQuantumToChar(MaxRGB-p->opacity);
            p++;
          }
          count+=EncodeImage(image,scanline,bytes_per_line & 0x7FFF,packed_scanline);
          if (QuantumTick(y,image->rows))
            if (!MagickMonitorFormatted(y,image->rows,&image->exception,
                                        SaveImageText,image->filename))
              break;
        }
      }
  if (count & 0x1)
    (void) WriteBlobByte(image,'\0');
  (void) WriteBlobMSBShort(image,PictEndOfPictureOp);
  offset=TellBlob(image);
  (void) SeekBlob(image,512,SEEK_SET);
  (void) WriteBlobMSBShort(image,(unsigned long) offset);
  MagickFreeMemory(scanline);
  MagickFreeMemory(packed_scanline);
  MagickFreeMemory(buffer);
  CloseBlob(image);
  return(True);
}
