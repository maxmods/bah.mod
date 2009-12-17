/*
% Copyright (C) 2003 - 2009 GraphicsMagick Group
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
%           CCCC   OOO   M   M  PPPP   RRRR   EEEEE   SSSSS  SSSSS            %
%          C      O   O  MM MM  P   P  R   R  E       SS     SS               %
%          C      O   O  M M M  PPPP   RRRR   EEE      SSS    SSS             %
%          C      O   O  M   M  P      R R    E          SS     SS            %
%           CCCC   OOO   M   M  P      R  R   EEEEE   SSSSS  SSSSS            %
%                                                                             %
%                                                                             %
%                  Image Compression/Decompression Methods                    %
%                                                                             %
%                                                                             %
%                           Software Design                                   %
%                             John Cristy                                     %
%                              May  1993                                      %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/blob.h"
#include "magick/compress.h"
#include "magick/monitor.h"
#include "magick/pixel_cache.h"
#include "magick/utility.h"

/*
  Define declarations.
*/


/*
  Typedef declarations.
*/
typedef struct HuffmanTable
{
  int
    id,
    code,
    length,
    count;
} HuffmanTable;

/*
  Huffman coding declarations.
*/
#define TWId  23
#define MWId  24
#define TBId  25
#define MBId  26
#define EXId  27

static const HuffmanTable
  MBTable[]=
  {
    { MBId, 0x0f, 10, 64 }, { MBId, 0xc8, 12, 128 },
    { MBId, 0xc9, 12, 192 }, { MBId, 0x5b, 12, 256 },
    { MBId, 0x33, 12, 320 }, { MBId, 0x34, 12, 384 },
    { MBId, 0x35, 12, 448 }, { MBId, 0x6c, 13, 512 },
    { MBId, 0x6d, 13, 576 }, { MBId, 0x4a, 13, 640 },
    { MBId, 0x4b, 13, 704 }, { MBId, 0x4c, 13, 768 },
    { MBId, 0x4d, 13, 832 }, { MBId, 0x72, 13, 896 },
    { MBId, 0x73, 13, 960 }, { MBId, 0x74, 13, 1024 },
    { MBId, 0x75, 13, 1088 }, { MBId, 0x76, 13, 1152 },
    { MBId, 0x77, 13, 1216 }, { MBId, 0x52, 13, 1280 },
    { MBId, 0x53, 13, 1344 }, { MBId, 0x54, 13, 1408 },
    { MBId, 0x55, 13, 1472 }, { MBId, 0x5a, 13, 1536 },
    { MBId, 0x5b, 13, 1600 }, { MBId, 0x64, 13, 1664 },
    { MBId, 0x65, 13, 1728 }, { MBId, 0x00, 0, 0 }
  };

static const HuffmanTable
  EXTable[]=
  {
    { EXId, 0x08, 11, 1792 }, { EXId, 0x0c, 11, 1856 },
    { EXId, 0x0d, 11, 1920 }, { EXId, 0x12, 12, 1984 },
    { EXId, 0x13, 12, 2048 }, { EXId, 0x14, 12, 2112 },
    { EXId, 0x15, 12, 2176 }, { EXId, 0x16, 12, 2240 },
    { EXId, 0x17, 12, 2304 }, { EXId, 0x1c, 12, 2368 },
    { EXId, 0x1d, 12, 2432 }, { EXId, 0x1e, 12, 2496 },
    { EXId, 0x1f, 12, 2560 }, { EXId, 0x00, 0, 0 }
  };

static const HuffmanTable
  MWTable[]=
  {
    { MWId, 0x1b, 5, 64 }, { MWId, 0x12, 5, 128 },
    { MWId, 0x17, 6, 192 }, { MWId, 0x37, 7, 256 },
    { MWId, 0x36, 8, 320 }, { MWId, 0x37, 8, 384 },
    { MWId, 0x64, 8, 448 }, { MWId, 0x65, 8, 512 },
    { MWId, 0x68, 8, 576 }, { MWId, 0x67, 8, 640 },
    { MWId, 0xcc, 9, 704 }, { MWId, 0xcd, 9, 768 },
    { MWId, 0xd2, 9, 832 }, { MWId, 0xd3, 9, 896 },
    { MWId, 0xd4, 9, 960 }, { MWId, 0xd5, 9, 1024 },
    { MWId, 0xd6, 9, 1088 }, { MWId, 0xd7, 9, 1152 },
    { MWId, 0xd8, 9, 1216 }, { MWId, 0xd9, 9, 1280 },
    { MWId, 0xda, 9, 1344 }, { MWId, 0xdb, 9, 1408 },
    { MWId, 0x98, 9, 1472 }, { MWId, 0x99, 9, 1536 },
    { MWId, 0x9a, 9, 1600 }, { MWId, 0x18, 6, 1664 },
    { MWId, 0x9b, 9, 1728 }, { MWId, 0x00, 0, 0 }
  };

static const HuffmanTable
  TBTable[]=
  {
    { TBId, 0x37, 10, 0 }, { TBId, 0x02, 3, 1 }, { TBId, 0x03, 2, 2 },
    { TBId, 0x02, 2, 3 }, { TBId, 0x03, 3, 4 }, { TBId, 0x03, 4, 5 },
    { TBId, 0x02, 4, 6 }, { TBId, 0x03, 5, 7 }, { TBId, 0x05, 6, 8 },
    { TBId, 0x04, 6, 9 }, { TBId, 0x04, 7, 10 }, { TBId, 0x05, 7, 11 },
    { TBId, 0x07, 7, 12 }, { TBId, 0x04, 8, 13 }, { TBId, 0x07, 8, 14 },
    { TBId, 0x18, 9, 15 }, { TBId, 0x17, 10, 16 }, { TBId, 0x18, 10, 17 },
    { TBId, 0x08, 10, 18 }, { TBId, 0x67, 11, 19 }, { TBId, 0x68, 11, 20 },
    { TBId, 0x6c, 11, 21 }, { TBId, 0x37, 11, 22 }, { TBId, 0x28, 11, 23 },
    { TBId, 0x17, 11, 24 }, { TBId, 0x18, 11, 25 }, { TBId, 0xca, 12, 26 },
    { TBId, 0xcb, 12, 27 }, { TBId, 0xcc, 12, 28 }, { TBId, 0xcd, 12, 29 },
    { TBId, 0x68, 12, 30 }, { TBId, 0x69, 12, 31 }, { TBId, 0x6a, 12, 32 },
    { TBId, 0x6b, 12, 33 }, { TBId, 0xd2, 12, 34 }, { TBId, 0xd3, 12, 35 },
    { TBId, 0xd4, 12, 36 }, { TBId, 0xd5, 12, 37 }, { TBId, 0xd6, 12, 38 },
    { TBId, 0xd7, 12, 39 }, { TBId, 0x6c, 12, 40 }, { TBId, 0x6d, 12, 41 },
    { TBId, 0xda, 12, 42 }, { TBId, 0xdb, 12, 43 }, { TBId, 0x54, 12, 44 },
    { TBId, 0x55, 12, 45 }, { TBId, 0x56, 12, 46 }, { TBId, 0x57, 12, 47 },
    { TBId, 0x64, 12, 48 }, { TBId, 0x65, 12, 49 }, { TBId, 0x52, 12, 50 },
    { TBId, 0x53, 12, 51 }, { TBId, 0x24, 12, 52 }, { TBId, 0x37, 12, 53 },
    { TBId, 0x38, 12, 54 }, { TBId, 0x27, 12, 55 }, { TBId, 0x28, 12, 56 },
    { TBId, 0x58, 12, 57 }, { TBId, 0x59, 12, 58 }, { TBId, 0x2b, 12, 59 },
    { TBId, 0x2c, 12, 60 }, { TBId, 0x5a, 12, 61 }, { TBId, 0x66, 12, 62 },
    { TBId, 0x67, 12, 63 }, { TBId, 0x00, 0, 0 }
  };

static const HuffmanTable
  TWTable[]=
  {
    { TWId, 0x35, 8, 0 }, { TWId, 0x07, 6, 1 }, { TWId, 0x07, 4, 2 },
    { TWId, 0x08, 4, 3 }, { TWId, 0x0b, 4, 4 }, { TWId, 0x0c, 4, 5 },
    { TWId, 0x0e, 4, 6 }, { TWId, 0x0f, 4, 7 }, { TWId, 0x13, 5, 8 },
    { TWId, 0x14, 5, 9 }, { TWId, 0x07, 5, 10 }, { TWId, 0x08, 5, 11 },
    { TWId, 0x08, 6, 12 }, { TWId, 0x03, 6, 13 }, { TWId, 0x34, 6, 14 },
    { TWId, 0x35, 6, 15 }, { TWId, 0x2a, 6, 16 }, { TWId, 0x2b, 6, 17 },
    { TWId, 0x27, 7, 18 }, { TWId, 0x0c, 7, 19 }, { TWId, 0x08, 7, 20 },
    { TWId, 0x17, 7, 21 }, { TWId, 0x03, 7, 22 }, { TWId, 0x04, 7, 23 },
    { TWId, 0x28, 7, 24 }, { TWId, 0x2b, 7, 25 }, { TWId, 0x13, 7, 26 },
    { TWId, 0x24, 7, 27 }, { TWId, 0x18, 7, 28 }, { TWId, 0x02, 8, 29 },
    { TWId, 0x03, 8, 30 }, { TWId, 0x1a, 8, 31 }, { TWId, 0x1b, 8, 32 },
    { TWId, 0x12, 8, 33 }, { TWId, 0x13, 8, 34 }, { TWId, 0x14, 8, 35 },
    { TWId, 0x15, 8, 36 }, { TWId, 0x16, 8, 37 }, { TWId, 0x17, 8, 38 },
    { TWId, 0x28, 8, 39 }, { TWId, 0x29, 8, 40 }, { TWId, 0x2a, 8, 41 },
    { TWId, 0x2b, 8, 42 }, { TWId, 0x2c, 8, 43 }, { TWId, 0x2d, 8, 44 },
    { TWId, 0x04, 8, 45 }, { TWId, 0x05, 8, 46 }, { TWId, 0x0a, 8, 47 },
    { TWId, 0x0b, 8, 48 }, { TWId, 0x52, 8, 49 }, { TWId, 0x53, 8, 50 },
    { TWId, 0x54, 8, 51 }, { TWId, 0x55, 8, 52 }, { TWId, 0x24, 8, 53 },
    { TWId, 0x25, 8, 54 }, { TWId, 0x58, 8, 55 }, { TWId, 0x59, 8, 56 },
    { TWId, 0x5a, 8, 57 }, { TWId, 0x5b, 8, 58 }, { TWId, 0x4a, 8, 59 },
    { TWId, 0x4b, 8, 60 }, { TWId, 0x32, 8, 61 }, { TWId, 0x33, 8, 62 },
    { TWId, 0x34, 8, 63 }, { TWId, 0x00, 0, 0 }
  };

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A S C I I 8 5 E n c o d e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ASCII85Encode encodes data in ASCII base-85 format.  ASCII base-85
%  encoding produces five ASCII printing characters from every four bytes of
%  binary data.
%
%  The format of the ASCII85Encode method is:
%
%      void Ascii85Encode(Image *image,const unsigned long code)
%
%  A description of each parameter follows:
%
%    o code: a binary unsigned char to encode to ASCII 85.
%
%    o file: write the encoded ASCII character to this file.
%
%
*/
#define MaxLineExtent  36

static char *Ascii85Tuple(unsigned char *data)
{
  static char
    tuple[6];

  register long
    i,
    x;

  unsigned long
    code,
    quantum;

  code=((((unsigned long) data[0] << 8) | (unsigned long) data[1]) << 16) |
    ((unsigned long) data[2] << 8) | (unsigned long) data[3];
  if (code == 0L)
    {
      tuple[0]='z';
      tuple[1]='\0';
      return(tuple);
    }
  quantum=85UL*85UL*85UL*85UL;
  for (i=0; i < 4; i++)
  {
    x=(long) (code/quantum);
    code-=quantum*x;
    tuple[i]=(char) (x+(int) '!');
    quantum/=85L;
  }
  tuple[4]=(char) ((code % 85L)+(int) '!');
  tuple[5]='\0';
  return(tuple);
}

MagickExport void Ascii85Initialize(Image *image)
{
  /*
    Allocate image structure.
  */
  if (image->ascii85 == (Ascii85Info *) NULL)
    {
      image->ascii85=MagickAllocateMemory(Ascii85Info *,sizeof(Ascii85Info));
      if (image->ascii85 == (Ascii85Info *) NULL)
        MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
                          UnableToAllocateAscii85Info);
    }
  (void) memset(image->ascii85,0,sizeof(Ascii85Info));
  image->ascii85->line_break=MaxLineExtent << 1;
  image->ascii85->offset=0;
}

MagickExport void Ascii85Flush(Image *image)
{
  register char
    *tuple;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->ascii85 != (Ascii85Info *) NULL);
  if (image->ascii85->offset > 0)
    {
      image->ascii85->buffer[image->ascii85->offset]=0;
      image->ascii85->buffer[image->ascii85->offset+1]=0;
      image->ascii85->buffer[image->ascii85->offset+2]=0;
      tuple=Ascii85Tuple(image->ascii85->buffer);
      (void) WriteBlob(image,image->ascii85->offset+1,
        *tuple == 'z' ? "!!!!" : tuple);
    }
  (void) WriteBlobByte(image,'~');
  (void) WriteBlobByte(image,'>');
  (void) WriteBlobByte(image,'\n');
}

MagickExport void Ascii85Encode(Image *image,const magick_uint8_t code)
{
  long
    n;

  register char
    *q;

  register unsigned char
    *p;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->ascii85 != (Ascii85Info *) NULL);
  image->ascii85->buffer[image->ascii85->offset]=code;
  image->ascii85->offset++;
  if (image->ascii85->offset < 4)
    return;
  p=image->ascii85->buffer;
  for (n=image->ascii85->offset; n >= 4; n-=4)
  {
    for (q=Ascii85Tuple(p); *q; q++)
    {
      image->ascii85->line_break--;
      if ((image->ascii85->line_break < 0) && (*q != '%'))
        {
          (void) WriteBlobByte(image,'\n');
          image->ascii85->line_break=2*MaxLineExtent;
        }
      (void) WriteBlobByte(image,*q);
    }
    p+=8;
  }
  image->ascii85->offset=n;
  p-=4;
  for (n=0; n < 4; n++)
    image->ascii85->buffer[n]=(*p++);
}

MagickExport unsigned int Ascii85WriteByteHook(Image *image, 
  const magick_uint8_t code, void *ARGUNUSED(info))
{
  Ascii85Encode(image,code);
  return(True);
}

MagickExport unsigned int BlobWriteByteHook(Image *image,
  const magick_uint8_t code, void *ARGUNUSED(info))
{
  return(WriteBlobByte(image,code));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   H u f f m a n D e c o d e I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method HuffmanDecodeImage uncompresses an image via Huffman-coding.
%
%  The format of the HuffmanDecodeImage method is:
%
%      unsigned int HuffmanDecodeImage(Image *image)
%
%  A description of each parameter follows:
%
%    o status:  Method HuffmanDecodeImage returns True if all the pixels are
%      compressed without error, otherwise False.
%
%    o image: The image.
%
%
*/
#define HashSize  1021
#define MBHashA  293
#define MBHashB  2695
#define MWHashA  3510
#define MWHashB  1178

#define InitializeHashTable(hash,table,a,b) \
{ \
  entry=table; \
  while (entry->code != 0) \
  {  \
    hash[((entry->length+a)*(entry->code+b)) % HashSize]=(HuffmanTable *) entry; \
    entry++; \
  } \
}

#define InputBit(bit)  \
{  \
  if ((mask & 0xffU) == 0)  \
    {  \
      byte=ReadBlobByte(image);  \
      if (byte == EOF)  \
        break;  \
      mask=0x80U;  \
    }  \
  runlength++;  \
  bit=(((unsigned int) byte) & mask) ? 0x01U : 0x00U; \
  mask >>= 1U;  \
  if (bit)  \
    runlength=0;  \
}

MagickExport MagickPassFail HuffmanDecodeImage(Image *image)
{
  const HuffmanTable
    *entry;

  HuffmanTable
    **mb_hash,
    **mw_hash;

  int
    bail,
    byte,
    code,
    color,
    length,
    null_lines,
    runlength;

  unsigned int
    bit,
    index,
    mask;

  long
    count,
    y;

  register IndexPacket
    *indexes;

  register long
    i,
    x;

  register PixelPacket
    *q;

  register unsigned char
    *p;

  unsigned char
    *scanline;

  MagickPassFail
    status=MagickPass;

  /*
    Allocate buffers.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  mb_hash=MagickAllocateMemory(HuffmanTable **,HashSize*sizeof(HuffmanTable *));
  mw_hash=MagickAllocateMemory(HuffmanTable **,HashSize*sizeof(HuffmanTable *));
  scanline=MagickAllocateMemory(unsigned char *,image->columns);
  if ((mb_hash == (HuffmanTable **) NULL) ||
      (mw_hash == (HuffmanTable **) NULL) ||
      (scanline == (unsigned char *) NULL))
    ThrowBinaryException(ResourceLimitError,MemoryAllocationFailed,
      (char *) NULL);
  /*
    Initialize Huffman tables.
  */
  for (i=0; i < HashSize; i++)
  {
    mb_hash[i]=(HuffmanTable *) NULL;
    mw_hash[i]=(HuffmanTable *) NULL;
  }
  InitializeHashTable(mw_hash,TWTable,MWHashA,MWHashB);
  InitializeHashTable(mw_hash,MWTable,MWHashA,MWHashB);
  InitializeHashTable(mw_hash,EXTable,MWHashA,MWHashB);
  InitializeHashTable(mb_hash,TBTable,MBHashA,MBHashB);
  InitializeHashTable(mb_hash,MBTable,MBHashA,MBHashB);
  InitializeHashTable(mb_hash,EXTable,MBHashA,MBHashB);
  /*
    Uncompress 1D Huffman to runlength encoded pixels.
  */
  byte=0;
  mask=0;
  null_lines=0;
  runlength=0;
  while (runlength < 11)
   InputBit(bit);
  do
  {
    InputBit(bit);
  } while (bit == 0);
  image->x_resolution=204.0;
  image->y_resolution=196.0;
  image->units=PixelsPerInchResolution;
  for (y=0; ((y < (long) image->rows) && (null_lines < 3)); )
  {
    /*
      Initialize scanline to white.
    */
    p=scanline;
    for (x=0; x < (long) image->columns; x++)
      *p++=0;
    /*
      Decode Huffman encoded scanline.
    */
    color=True;
    code=0;
    count=0;
    length=0;
    runlength=0;
    x=0;
    for ( ; ; )
    {
      if (byte == EOF)
        break;
      if (x >= (long) image->columns)
        {
          while (runlength < 11)
            InputBit(bit);
          do { InputBit(bit); } while (bit == 0);
          break;
        }
      bail=False;
      do
      {
        if (runlength < 11)
          InputBit(bit)
        else
          {
            InputBit(bit);
            if (bit)
              {
                null_lines++;
                if (x != 0)
                  null_lines=0;
                bail=True;
                break;
              }
          }
        code=(code << 1)+bit;
        length++;
      } while (code <= 0);
      if (bail)
        break;
      if (length > 13)
        {
          while (runlength < 11)
           InputBit(bit);
          do
          {
            InputBit(bit);
          } while (bit == 0);
          break;
        }
      if (color)
        {
          if (length < 4)
            continue;
          entry=mw_hash[((length+MWHashA)*(code+MWHashB)) % HashSize];
        }
      else
        {
          if (length < 2)
            continue;
          entry=mb_hash[((length+MBHashA)*(code+MBHashB)) % HashSize];
        }
      if (!entry)
        continue;
      if ((entry->length != length) || (entry->code != code))
        continue;
      switch (entry->id)
      {
        case TWId:
        case TBId:
        {
          count+=entry->count;
          if ((x+count) > (long) image->columns)
            count=(long) image->columns-x;
          if (count > 0)
            {
              if (color)
                {
                  x+=count;
                  count=0;
                }
              else
                for ( ; count > 0; count--)
                  scanline[x++]=1;
            }
          color=!color;
          break;
        }
        case MWId:
        case MBId:
        case EXId:
        {
          count+=entry->count;
          break;
        }
        default:
          break;
      }
      code=0;
      length=0;
    }
    /*
      Transfer scanline to image pixels.
    */
    p=scanline;
    q=SetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      {
        status=MagickFail;
        break;
      }
    indexes=AccessMutableIndexes(image);
    for (x=0; x < (long) image->columns; x++)
    {
      index=(unsigned int) (*p++);
      indexes[x]=index;
      *q++=image->colormap[index];
    }
    if (!SyncImagePixels(image))
      {
        status=MagickFail;
        break;
      }
    if (QuantumTick(y,image->rows))
      if (!MagickMonitorFormatted(y,image->rows,&image->exception,
                                  "[%s] Huffman decode image...",image->filename))
        {
          status=MagickFail;
          break;
        }
    y++;
  }
  image->rows=Max(y-3,1);
  image->compression=FaxCompression;
  /*
    Free decoder memory.
  */
  MagickFreeMemory(mw_hash);
  MagickFreeMemory(mb_hash);
  MagickFreeMemory(scanline);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   H u f f m a n E n c o d e I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method HuffmanEncodeImage compresses an image via Huffman-coding.
%
%  The format of the HuffmanEncodeImage method is:
%
%      unsigned int HuffmanEncodeImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows:
%
%    o status:  Method HuffmanEncodeImage returns True if all the pixels are
%      compressed without error, otherwise False.
%
%    o image_info: The image info..
%
%    o image: The image.
%
*/
#define HuffmanOutputCode(entry)  \
{  \
  mask=1 << (entry->length-1);  \
  while (mask != 0)  \
  {  \
    OutputBit((entry->code & mask ? 1 : 0));  \
    mask>>=1;  \
  }  \
}

#define OutputBit(count)  \
{  \
  if (count > 0)  \
    byte=byte | bit;  \
  bit >>= 1;  \
  if ((bit & 0xffU) == 0)   \
    {  \
      (void) (*write_byte)(image,(magick_uint8_t) byte,info);  \
      byte=0U;  \
      bit=0x80U;  \
    }  \
}
MagickExport MagickPassFail HuffmanEncode2Image(const ImageInfo *image_info,
  Image *image, WriteByteHook write_byte, void *info)
{
  const HuffmanTable
    *entry;

  int
    k,
    is_fax,
    runlength;

  long
    n,
    y;

  Image
    *huffman_image;

  register const IndexPacket
    *indexes;

  register long
    i,
    x;

  register const PixelPacket
    *p;

  register unsigned char
    *q;

  register unsigned int
    polarity;

  unsigned int
    bit,
    byte;

  unsigned char
    *scanline;

  unsigned long
    mask,
    width;

  MagickPassFail
    status=MagickPass;

  /*
    Allocate scanline buffer.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  is_fax=False;
  if (LocaleCompare(image_info->magick,"FAX") == 0)
    is_fax=True;
  width=image->columns;
  if (is_fax == True)
    width=Max(image->columns,1728);
  scanline=MagickAllocateMemory(unsigned char *,width+1);
  if (scanline == (unsigned char *) NULL)
    ThrowBinaryException(ResourceLimitError,MemoryAllocationFailed,
      (char *) NULL);
  huffman_image=CloneImage(image,0,0,True,&image->exception);
  if (huffman_image == (Image *) NULL)
    return(MagickFail);
  status &= SetImageType(huffman_image,BilevelType);
  byte=0;
  bit=0x80;
  if (is_fax == True)
    {
      /*
        End of line.
      */
      for (k=0; k < 11; k++)
        OutputBit(0);
      OutputBit(1);
    }
  /*
    Compress runlength encoded to 1D Huffman pixels.
  */
  polarity=(PixelIntensity(&huffman_image->colormap[0]) < (MaxRGB/2));
  if (huffman_image->colors == 2)
    polarity=(PixelIntensityToQuantum(&huffman_image->colormap[0]) <
      PixelIntensityToQuantum(&huffman_image->colormap[1]) ? 0x00 : 0x01);
  q=scanline;
  for (i=(long) width; i > 0; i--)
    *q++=(unsigned char) polarity;
  q=scanline;
  for (y=0; y < (long) huffman_image->rows; y++)
  {
    p=AcquireImagePixels(huffman_image,0,y,huffman_image->columns,1,
      &huffman_image->exception);
    if (p == (const PixelPacket *) NULL)
      {
        status=MagickFail;
        break;
      }
    indexes=AccessImmutableIndexes(huffman_image);
    for (x=0; x < (long) huffman_image->columns; x++)
    {
      *q=(unsigned char) (indexes[x] == polarity ? !polarity : polarity);
      q++;
    }
    /*
      Huffman encode scanline.
    */
    q=scanline;
    for (n=(long) width; n > 0; )
    {
      /*
        Output white run.
      */
      for (runlength=0; ((n > 0) && (*q == polarity)); n--)
      {
        q++;
        runlength++;
      }
      if (runlength >= 64)
        {
          if (runlength < 1792)
            entry=MWTable+((runlength/64)-1);
          else
            entry=EXTable+(Min(runlength,2560)-1792)/64;
          runlength-=entry->count;
          HuffmanOutputCode(entry);
        }
      entry=TWTable+Min(runlength,63);
      HuffmanOutputCode(entry);
      if (n != 0)
        {
          /*
            Output black run.
          */
          for (runlength=0; ((*q != polarity) && (n > 0)); n--)
          {
            q++;
            runlength++;
          }
          if (runlength >= 64)
            {
              entry=MBTable+((runlength/64)-1);
              if (runlength >= 1792)
                entry=EXTable+(Min(runlength,2560)-1792)/64;
              runlength-=entry->count;
              HuffmanOutputCode(entry);
            }
          entry=TBTable+Min(runlength,63);
          HuffmanOutputCode(entry);
        }
    }
    /*
      End of line.
    */
    for (k=0; k < 11; k++)
      OutputBit(0);
    OutputBit(1);
    q=scanline;
    if (huffman_image->previous == (Image *) NULL)
      if (QuantumTick(y,huffman_image->rows))
        if (!MagickMonitorFormatted(y,huffman_image->rows,&image->exception,
                                    "[%s] Huffman encode image...",image->filename))
          {
            status=MagickFail;
            break;
          }
  }
  /*
    End of page.
  */
  for (i=0; i < 6; i++)
  {
    for (k=0; k < 11; k++)
      OutputBit(0);
    OutputBit(1);
  }
  /*
    Flush bits.
  */
  if (bit != 0x80U)
    (void) (*write_byte)(image,(magick_uint8_t)byte,info);
  DestroyImage(huffman_image);
  MagickFreeMemory(scanline);
  return(status);
}

MagickExport MagickPassFail HuffmanEncodeImage(const ImageInfo *image_info,
  Image *image)
{
  if (LocaleCompare(image_info->magick,"FAX") == 0)
  {
    return(HuffmanEncode2Image(image_info,image,BlobWriteByteHook,(void *)NULL));
  }
  else
  {
    MagickPassFail
      status;
    Ascii85Initialize(image);
    status=HuffmanEncode2Image(image_info,image,Ascii85WriteByteHook,(void *)NULL);
    Ascii85Flush(image);
    return(status);
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   I m a g e T o H u f f m a n 2 D B l o b                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ImageToHuffman2DBlob compresses an image via two-dimensional
%  Huffman-coding (CCITT Group4 FAX) and returns the compressed data in a
%  heap allocated buffer.  NULL is returned if there is an error.
%
%  Data is output with one strip per page with MSB2LSB fill order.
%
%  The format of the ImageToHuffman2DBlob method is:
%
%      unsigned char *ImageToHuffman2DBlob(const Image *image,
%                                          const ImageInfo *image_info,
%                                          size_t *blob_length,
%                                          ExceptionInfo *exception);
%
%  A description of each parameter follows:
%
%    o image: The image to compress.
%
%    o image_info: Image options
%
%    o blob_length: Updated with the length of the compressed data.
%
%    o exception: Any exception is reported here.
%
*/
MagickExport unsigned char *
ImageToHuffman2DBlob(const Image *image,const ImageInfo *image_info,
		     size_t *blob_length,ExceptionInfo *exception)
{
  unsigned char
    *blob = (unsigned char *) NULL;

  ImageInfo
    *huffman_info;

  ARG_NOT_USED(image_info);
  *blob_length=0;
  huffman_info=CloneImageInfo((const ImageInfo *) NULL);
  if (huffman_info != (ImageInfo *) NULL)
    {
      Image
	*huffman_image;

      huffman_image=CloneImage(image,0,0,MagickTrue,exception);
      if (huffman_image != (Image *) NULL)
	{
	  (void) strlcpy(huffman_image->magick,"GROUP4RAW",sizeof(huffman_image->magick));
	  (void) strlcpy(huffman_image->filename,"",sizeof(huffman_image->filename));
	  blob=ImageToBlob(huffman_info, huffman_image, blob_length, exception);
	  DestroyImage(huffman_image);
	}
      DestroyImageInfo(huffman_info);
    }
  return blob;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   I m a g e T o J P E G B l o b                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ImageToJPEGBlob compresses an image into a JFIF JPEG format and
%  returns the compressed data in a heap allocated buffer.  NULL is
%  returned if there is an error.
%
%  The format of the ImageToJPEGBlob method is:
%
%      unsigned char *ImageToJPEGBlob(const Image *image,
%                                     const ImageInfo *image_info,
%                                     size_t *blob_length,
%                                     ExceptionInfo *exception);
%
%  A description of each parameter follows:
%
%    o image: The image to compress.
%
%    o image_info: Image options
%
%    o blob_length: Updated with the length of the compressed data.
%
%    o exception: Any exception is reported here.
%
*/
MagickExport unsigned char *
ImageToJPEGBlob(const Image *image,const ImageInfo *image_info,
		size_t *blob_length,ExceptionInfo *exception)
{
  unsigned char
    *blob = NULL;

  ImageInfo
    *jpeg_info;

  *blob_length=0;
  jpeg_info=CloneImageInfo(image_info);
  if (jpeg_info != (ImageInfo *) NULL)
    {
      Image
	*jpeg_image;

      /*
	Try to preserve any existing JPEG options but if the user
	applies any override, then existing JPEG options are ignored.
      */
      if ((JPEGCompression == image->compression) &&
	  (DefaultCompressionQuality == image_info->quality) &&
	  ((char *) NULL == jpeg_info->sampling_factor))
	(void) AddDefinitions(jpeg_info,"jpeg:preserve-settings=TRUE",
			      exception);
      jpeg_image=CloneImage(image,0,0,MagickTrue,exception);
      if (jpeg_image != (Image *) NULL)
	{
	  (void) strlcpy(jpeg_image->magick,"JPEG",sizeof(jpeg_image->magick));
	  (void) strlcpy(jpeg_image->filename,"",sizeof(jpeg_image->filename));
	  blob =(unsigned char *) ImageToBlob(jpeg_info, jpeg_image, blob_length, exception);
	  DestroyImage(jpeg_image);
	}
      DestroyImageInfo(jpeg_info);
    }
  return blob;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L Z W E n c o d e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method LZWEncodeImage compresses an image via LZW-coding specific to
%  Postscript Level II or Portable Document Format.  To ensure portability, the
%  binary LZW bytes are encoded as ASCII base-85.
%
%  The format of the LZWEncodeImage method is:
%
%      unsigned int LZWEncodeImage(Image *image,const size_t length,
%        unsigned char *pixels)
%
%  A description of each parameter follows:
%
%    o status:  Method LZWEncodeImage returns True if all the pixels are
%      compressed without error, otherwise False.
%
%    o image: The image.
%
%    o length:  A value that specifies the number of pixels to compress.
%
%    o pixels: The address of an unsigned array of characters containing the
%      pixels to compress.
%
%
*/
#define LZWClr  256  /* Clear Table Marker */
#define LZWEod  257  /* End of Data marker */
#define OutputCode(code) \
{ \
    accumulator+=((long) code) << (32-code_width-number_bits); \
    number_bits+=code_width; \
    while (number_bits >= 8) \
    { \
        (void) (*write_byte)(image,(magick_uint8_t) (accumulator >> 24),info); \
        accumulator=accumulator << 8; \
        number_bits-=8; \
    } \
}

MagickExport MagickPassFail LZWEncode2Image(Image *image,
  const size_t length,magick_uint8_t *pixels,WriteByteHook write_byte,void *info)
{
  typedef struct _TableType
  {
    short
      prefix,
      suffix,
      next;
  } TableType;

  int
    index;

  register long
    i;

  short
    number_bits,
    code_width,
    last_code,
    next_index;

  TableType
    *table;

  unsigned long
    accumulator;

  /*
    Allocate string table.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(pixels != (unsigned char *) NULL);
  table=MagickAllocateMemory(TableType *,(1 << 12)*sizeof(TableType));
  if (table == (TableType *) NULL)
    return(False);
  /*
    Initialize variables.
  */
  accumulator=0;
  code_width=9;
  number_bits=0;
  last_code=0;
  OutputCode(LZWClr);
  for (index=0; index < 256; index++)
  {
    table[index].prefix=(-1);
    table[index].suffix=index;
    table[index].next=(-1);
  }
  next_index=LZWEod+1;
  code_width=9;
  last_code=pixels[0];
  for (i=1; i < (long) length; i++)
  {
    /*
      Find string.
    */
    index=last_code;
    while (index != -1)
      if ((table[index].prefix != last_code) ||
          (table[index].suffix != pixels[i]))
        index=table[index].next;
      else
        {
          last_code=index;
          break;
        }
    if (last_code != index)
      {
        /*
          Add string.
        */
        OutputCode(last_code);
        table[next_index].prefix=last_code;
        table[next_index].suffix=pixels[i];
        table[next_index].next=table[last_code].next;
        table[last_code].next=next_index;
        next_index++;
        /*
          Did we just move up to next bit width?
        */
        if ((next_index >> code_width) != 0)
          {
            code_width++;
            if (code_width > 12)
              {
                /*
                  Did we overflow the max bit width?
                */
                code_width--;
                OutputCode(LZWClr);
                for (index=0; index < 256; index++)
                {
                  table[index].prefix=(-1);
                  table[index].suffix=index;
                  table[index].next=(-1);
                }
                next_index=LZWEod+1;
                code_width=9;
              }
            }
          last_code=pixels[i];
      }
  }
  /*
    Flush tables.
  */
  OutputCode(last_code);
  OutputCode(LZWEod);
  if (number_bits != 0)
    (void) (*write_byte)(image,(magick_uint8_t)(accumulator >> 24),info);
  MagickFreeMemory(table);
  return(MagickPass);
}

MagickExport MagickPassFail LZWEncodeImage(Image *image, const size_t length,
  magick_uint8_t *pixels)
{
  return(LZWEncode2Image(image,length,pixels,BlobWriteByteHook,(void *)NULL));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P a c k b i t s E n c o d e I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method PackbitsEncodeImage compresses an image via Macintosh Packbits
%  encoding specific to Postscript Level II or Portable Document Format.  To
%  ensure portability, the binary Packbits bytes are encoded as ASCII Base-85.
%
%  The format of the PackbitsEncodeImage method is:
%
%      unsigned int PackbitsEncodeImage(Image *image,const size_t length,
%        unsigned char *pixels)
%
%  A description of each parameter follows:
%
%    o status:  Method PackbitsEncodeImage returns True if all the pixels are
%      compressed without error, otherwise False.
%
%    o image: The image.
%
%    o length:  A value that specifies the number of pixels to compress.
%
%    o pixels: The address of an unsigned array of characters containing the
%      pixels to compress.
%
%
*/
MagickExport MagickPassFail PackbitsEncode2Image(Image *image,
  const size_t length,magick_uint8_t *pixels,WriteByteHook write_byte, 
  void *info)
{
  int
    count;

  register int
    i,
    j;

  unsigned char
    *packbits;

  /*
    Compress pixels with Packbits encoding.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(pixels != (unsigned char *) NULL);
  packbits=MagickAllocateMemory(unsigned char *,128);
  if (packbits == (unsigned char *) NULL)
    ThrowBinaryException(ResourceLimitError,MemoryAllocationFailed,
      (char *) NULL);
  i=(long) length;
  while (i != 0)
  {
    switch (i)
    {
      case 1:
      {
        i--;
        (void) (*write_byte)(image,(magick_uint8_t)0,info);
        (void) (*write_byte)(image,(magick_uint8_t)*pixels,info);
        break;
      }
      case 2:
      {
        i-=2;
        (void) (*write_byte)(image,(magick_uint8_t)1,info);
        (void) (*write_byte)(image,(magick_uint8_t)*pixels,info);
        (void) (*write_byte)(image,(magick_uint8_t)pixels[1],info);
        break;
      }
      case 3:
      {
        i-=3;
        if ((*pixels == *(pixels+1)) && (*(pixels+1) == *(pixels+2)))
          {
            (void) (*write_byte)(image,(magick_uint8_t)((256-3)+1),info);
            (void) (*write_byte)(image,(magick_uint8_t)*pixels,info);
            break;
          }
        (void) (*write_byte)(image,(magick_uint8_t)2,info);
        (void) (*write_byte)(image,(magick_uint8_t)*pixels,info);
        (void) (*write_byte)(image,(magick_uint8_t)pixels[1],info);
        (void) (*write_byte)(image,(magick_uint8_t)pixels[2],info);
        break;
      }
      default:
      {
        if ((*pixels == *(pixels+1)) && (*(pixels+1) == *(pixels+2)))
          {
            /*
              Packed run.
            */
            count=3;
            while (((long) count < i) && (*pixels == *(pixels+count)))
            {
              count++;
              if (count >= 127)
                break;
            }
            i-=count;
            (void) (*write_byte)(image,(magick_uint8_t)((256-count)+1),info);
            (void) (*write_byte)(image,(magick_uint8_t)*pixels,info);
            pixels+=count;
            break;
          }
        /*
          Literal run.
        */
        count=0;
        while ((*(pixels+count) != *(pixels+count+1)) ||
               (*(pixels+count+1) != *(pixels+count+2)))
        {
          packbits[count+1]=pixels[count];
          count++;
          if (((long) count >= (i-3)) || (count >= 127))
            break;
        }
        i-=count;
        *packbits=count-1;
        for (j=0; j <= (long) count; j++)
          (void) (*write_byte)(image,(magick_uint8_t)packbits[j],info);
        pixels+=count;
        break;
      }
    }
  }
  (void) (*write_byte)(image,(magick_uint8_t)128,info);  /* EOD marker */
  MagickFreeMemory(packbits);
  return(MagickPass);
}

MagickExport MagickPassFail PackbitsEncodeImage(Image *image,const size_t length,
  magick_uint8_t *pixels)
{
  return(PackbitsEncode2Image(image,length,pixels,BlobWriteByteHook,(void *)NULL));
}
