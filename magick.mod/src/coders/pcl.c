/*
% Copyright (C) 2003-2010 GraphicsMagick Group
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
%                            PPPP    CCCC  L                                  %
%                            P   P  C      L                                  %
%                            PPPP   C      L                                  %
%                            P      C      L                                  %
%                            P       CCCC  LLLLL                              %
%                                                                             %
%                                                                             %
%                        Write HP PCL Printer Format.                         %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
%                         Rewritten by John Sergeant                          %
%                                 May 2009                                    %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/
/*
  Tunables
*/
#define NEED_END_OF_ROW_DELTA_OUTPUT
/*#define DISABLE_ZERO_ROW_COMPRESSION */

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/analyze.h"
#include "magick/attribute.h"
#include "magick/blob.h"
#include "magick/pixel_cache.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/utility.h"

/*
  Enum declarations.
*/
typedef enum
{
  PCL_NoCompression,
  PCL_RLECompression,
  PCL_TiffRLECompression,
  PCL_DeltaCompression,
  PCL_ZeroRowCompression,
  PCL_RepeatedRowCompression,
  PCL_UndefinedCompression
} PCL_CompressionType;

/*
  Forward declarations.
*/
static unsigned int
  WritePCLImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s P C L                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsPCL returns True if the image format type, identified by the
%  magick string, is PCL.
%
%  The format of the IsPCL method is:
%
%      unsigned int IsPCL(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsPCL returns True if the image format type is PCL.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsPCL(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(False);
  if (memcmp(magick,"\033E\033",3) == 0)
    return(True);
  if (memcmp(magick,"\033E\033&",4) == 0)
    return(False);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r P C L I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterPCLImage adds attributes for the PCL image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterPCLImage method is:
%
%      RegisterPCLImage(void)
%
*/
ModuleExport void RegisterPCLImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("PCL");
  entry->encoder=(EncoderHandler) WritePCLImage;
  entry->magick=(MagickHandler) IsPCL;
  entry->adjoin=True;
  entry->description="Page Control Language";
  entry->module="PCL";
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r P C L I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterPCLImage removes format registrations made by the
%  PCL module from the list of supported formats.
%
%  The format of the UnregisterPCLImage method is:
%
%      UnregisterPCLImage(void)
%
*/
ModuleExport void UnregisterPCLImage(void)
{
  (void) UnregisterMagickInfo("PCL");
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P C L _ C h o o s e C o m p r e s s i o n                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method PCL_ChooseCompression chooses a method by which to compress a given
%  row of pixels in the PCL format
%
%  The format of the PCL_ChooseCompression method is:
%
%  PCL_CompressionType PCL_ChooseCompression(unsigned long row_width,
%                                            unsigned char *row,
%                                            unsigned char *last_row)
%
%  A description of each parameter follows.
%
%    o status: Method PCL_ChooseCompression returns a PCL_CompressionType
%      variable suggesting the best form of compression to use for the row
%
%    o row_width: An unsigned long specifying number of bytes in row
%
%    o row: A pointer to the current row of pixels
%
%    o last_row: A pointer to the last row of pixels (for use by delta compression)
%
*/
static PCL_CompressionType PCL_ChooseCompression(unsigned long row_width,
                                                 unsigned char *row,
                                                 unsigned char *last_row)
{
  unsigned long
    x,
    rep,
    unrep,
    RLE_cost,
    TiffRLE_cost,
    delta_cost,
    least_cost;

  unsigned char
    *q,
    *last_row_q;

  unsigned char
    last_char,
    this_char;

  PCL_CompressionType
    compression;

  /*
    Calculate cost to encode via either RLE method
  */
  TiffRLE_cost=0;
  RLE_cost=0;
  x=0;
  q=row;
  this_char=0;
  last_char=!*q;
  rep=0;
  while (x < row_width)
    {
      /*
        Count unrepeated bytes
      */
      unrep=0;
      rep=0;
      while (x < row_width)
        {
          x++;
          this_char=*q++;
          if (this_char == last_char)
            {
              unrep--;
              rep=2;
              break;
            }
          last_char=this_char;
          unrep++;
        }

      /*
        Count repeated bytes
      */
      while (x < row_width)
        {
          this_char=*q;
          if (this_char != last_char)
            break;
          rep++;
          x++;
          q++;
        }
      /*
        Increment costs based on what we found
      */
      RLE_cost += 2*(unrep+((rep+255)/256));
      TiffRLE_cost += unrep + ((unrep+127)/128) + 2*((rep+127)/128);
    } 
  /*
    Special case #1 - row is all zero
  */
#if defined(DISABLE_ZERO_ROW_COMPRESSION)
  if ((rep==row_width) && (!this_char))
    return PCL_ZeroRowCompression;
#endif

  /*
    Calculate cost to encode via delta method
  */
  delta_cost=0;
  x=0;
  q=row;
  last_row_q=last_row;
  while (x < row_width)
    {
      /*
        Count unaltered bytes
      */
      unrep=0;
      rep=0;
      while (x < row_width)
        {
          x++;
          if (*q++ != *last_row_q++)
            {
              unrep=1;
              break;
            }
          rep++;
        }

      /*
        Count altered bytes
      */
      while (x < row_width)
        {
          if (*q == *last_row_q)
            break;
          unrep++;
          x++;
          q++;
          last_row_q++;
        }
      /*
        Increment cost based on what we found
      */
      if (unrep)
        delta_cost += 1 + ((rep+224)/255) + unrep + ((unrep+7)/8);
#if defined(NEED_END_OF_ROW_DELTA_OUTPUT)
      else
        delta_cost += 2 + ((rep+223)/255);
#endif
    }
  /*
    Special case #2 - row is unchanged
  */
  if (rep == row_width)
    return PCL_RepeatedRowCompression;

  /* Choose compression to use, starting with most likely */
  least_cost=delta_cost;
  compression=PCL_DeltaCompression; 
  if (TiffRLE_cost < least_cost)
    {
      least_cost=TiffRLE_cost;
      compression=PCL_TiffRLECompression;
    }
  if (RLE_cost < least_cost)
    {
      least_cost=RLE_cost;
      compression=PCL_RLECompression;
    }
  if (row_width < least_cost)
    {
      least_cost=row_width;
      compression=PCL_NoCompression;
    }
  return compression;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P C L _ D e l t a C o m p r e s s                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method PCL_DeltaCompress compresses a given row of pixels using the PCL delta
%  compress method
%
%  The format of the PCL_DeltaCompress method is:
%
%  unsigned long PCL_DeltaCompress (unsigned long row_width,
%                                   unsigned char *row,
%                                   unsigned char *last_row,
%                                   unsigned char *compressed_row)
%
% A description of each parameter follows.
%
%    o status: Method PCL_DataCompress returns the byte size of the
%      data stored in compressed_row
%
%    o row_width: An unsigned long specifying number of bytes in row
%
%    o row: A pointer to the current row of pixels
%
%    o last_row: A pointer to the last row of pixels
%
%    o compressed_row: A buffer into which the compressed data is to be
%      written.
%
*/
static unsigned long PCL_DeltaCompress(unsigned long row_width,
                                       unsigned char *row,
                                       unsigned char *last_row,
                                       unsigned char *compressed_row)
{
  unsigned long
    x,
    rep,
    unrep,
    rep_this_time,
    unrep_this_time;

  unsigned char
    *q,
    *last_row_q,
    *out;

  x=0;
  q=row;
  last_row_q=last_row;
  out=compressed_row;
  while (x < row_width)
    {
      /*
        Count unaltered bytes
      */
      unrep=0;
      rep=0;
      while (x < row_width)
        {
          x++;
          if (*q++ != *last_row_q++)
            {
              unrep=1;
              break;
            }
          rep++;
        }

      /*
        Count altered bytes
      */
      while (x < row_width)
        {
          if (*q == *last_row_q)
            break;
          unrep++;
          x++;
          q++;
          last_row_q++;
        }

      /*
        Unrep can only be zero if no further changes are required on this row
      */
      if (!unrep)
        break;

      /*
        Output first control byte, including offset
      */
      rep_this_time = (rep >= 31)?31:rep;
      rep -= rep_this_time;
      unrep_this_time = (unrep >= 8)?8:unrep;
      *out++ = (unsigned char)(((unrep_this_time-1) << 5) | rep_this_time);

      if (rep_this_time == 31)
        {
          /*
            Output extra offset bytes plus an extra zero if last was 255
          */
          rep_this_time=255;
          while (rep)
            {
              if (rep_this_time > rep)
                rep_this_time=rep;
              *out++ = (unsigned char)rep_this_time;
              rep -= rep_this_time;
            }
          if (rep_this_time == 255)
            *out++ = (unsigned char)0;
        } 
      /* Now skip back to beginning of unreplicated data and start outputting it */
      q -= unrep; 
      while (1)
        {
          unrep -= unrep_this_time;
          while (unrep_this_time)
            {
              *out++ = *q++;
              unrep_this_time--;
            }
          if (!unrep)
            break;

          /* Output next control byte */
          if (unrep >= 8)
            unrep_this_time = 8;
          else
            unrep_this_time=unrep;
          *out++=(unsigned char)((unrep_this_time-1) << 5);
        }
    }
  return (unsigned long)(out-compressed_row);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P C L _ T i f f R L E C o m p r e s s                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method PCL_TiffRLECompress compresses a given row of pixels using the PCL
%  Tiff RLE compress method
%
%  The format of the PCL_TiffRLECompress method is:
%
%  unsigned long PCL_TiffRLECompress(unsigned long row_width,
%                                    unsigned char *row,
%                                    unsigned char *compressed_row)
%
% A description of each parameter follows.
%
%    o status: Method PCL_TiffRLECompress returns the byte size of the
%      data stored in compressed_row
%
%    o row_width: An unsigned long specifying number of bytes in row
%
%    o row: A pointer to the current row of pixels
%
%    o compressed_row: A buffer into which the compressed data is to be
%      written.
%
*/
static unsigned long PCL_TiffRLECompress(unsigned long row_width,
                                         unsigned char *row,
                                         unsigned char *compressed_row)
{
  unsigned long
    x,
    rep,
    unrep,
    rep_this_time,
    unrep_this_time;

  unsigned char
    *q,
    *out;

  unsigned char
    last_char,
    this_char;

  x=0;
  q=row;
  this_char=0;
  last_char=!*q;
  out=compressed_row;
  while (x < row_width)
    {
      /*
        Count unrepeated bytes
      */
      unrep=0;
      rep=0;
      while (x < row_width)
        {
          x++;
          this_char=*q++;
          if (this_char == last_char)
            {
              unrep--;
              rep=2;
              break;
            }
          last_char=this_char;
          unrep++;
        }

      /*
        Count repeated bytes
      */
      while (x < row_width)
        {
          this_char=*q;
          if (this_char != last_char)
            break;
          rep++;
          x++;
          q++;
        }

      /*
        Output unrepeated bytes
      */
      if (unrep)
        {
          q -= (unrep + rep);
          while (unrep)
            {
              unrep_this_time = (unrep >= 128)?128:unrep;
              *out++=(unsigned char)(unrep_this_time-1);
              unrep -= unrep_this_time;
              while (unrep_this_time)
                {
                  *out++ = *q++;
                  unrep_this_time--;
                }
            }
          q += rep;
        }

      /*
        Output repeated bytes
      */
      rep_this_time=128;
      while (rep)
        {
          if (rep_this_time > rep)
            rep_this_time = rep;
          *out++ = (unsigned char)(257-rep_this_time);
          *out++ = last_char;
          rep -= rep_this_time;
        }
    }
  return (unsigned long)(out-compressed_row);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P C L _ R L E C o m p r e s s                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method PCL_RLECompress compresses a given row of pixels using the PCL RLE
%  compress method
%
%  The format of the PCL_RLECompress method is:
%
%  unsigned long PCL_RLECompress(unsigned long row_width,
%                                unsigned char *row,
%                                unsigned char *compressed_row)
%
% A description of each parameter follows.
%
%    o status: Method PCL_RLECompress returns the byte size of the data
%      stored in compressed_row
%
%    o row_width: An unsigned long specifying number of bytes in row
%
%    o row: A pointer to the current row of pixels
%
%    o compressed_row: A buffer into which the compressed data is to be
%      written.
%
*/
static unsigned long PCL_RLECompress(unsigned long row_width,
                                     unsigned char *row,
                                     unsigned char *compressed_row)
{
  unsigned long
    x,
    rep,
    unrep,
    rep_this_time;

  unsigned char
    *q,
    *out;

  unsigned char
    last_char,
    this_char;

  x=0;
  q=row;
  this_char=0;
  last_char=!*q;
  out=compressed_row;
  while (x < row_width)
    {
      /*
        Count unrepeated bytes
      */
      unrep=0;
      rep=0;
      while (x < row_width)
        {
          x++;
          this_char=*q++;
          if (this_char == last_char)
            {
              unrep--;
              rep=2;
              break;
            }
          last_char=this_char;
          unrep++;
        }

      /*
        Count repeated bytes
      */
      while (x < row_width)
        {
          this_char=*q;
          if (this_char != last_char)
            break;
          rep++;
          x++;
          q++;
        }

      /*
        Output unrepeated bytes
      */
      if (unrep)
        {
          q -= (unrep + rep);
          while (unrep)
            {
              *out++=0;
              *out++=*q++;
              unrep--;
            }
          q += rep;
        }

      /*
        Output repeated bytes
      */
      rep_this_time=256;
      while (rep)
        {
          if (rep_this_time > rep)
            rep_this_time = rep;
          *out++=(unsigned char)(rep_this_time-1);
          *out++ = last_char;
          rep -= rep_this_time;
        }
    }
  return (unsigned long)(out-compressed_row);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P C L I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePCLImage writes an image in the Page Control Language encoded
%  image format.
%
%  The format of the WritePCLImage method is:
%
%      unsigned int WritePCLImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WritePCLImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
%
*/
static unsigned int WritePCLImage(const ImageInfo *image_info,Image *image)
{
  char
    buffer[MaxTextExtent];

  long
    sans,
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
    *pixels,
    *last_row_pixels,
    *output_row;

  unsigned int
    status;

  long
    zero_rows;

  unsigned long
    bytes_to_write,
    scene,
    density,
    bytes_per_line;

  unsigned char
    bits_per_pixel;

  ImageCharacteristics
    characteristics;

  PCL_CompressionType
    compression,
    last_row_compression;

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

  (void) GetGeometry("75x75",&sans,&sans,&density,&density);
  if (image_info->density != (char *) NULL)
    (void) GetGeometry(image_info->density,&sans,&sans,&density,&density);

  scene = 0;
  output_row = (unsigned char *) NULL;
  last_row_pixels = (unsigned char *) NULL;
  do
    {
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
        Initialize the printer
      */
      (void) WriteBlobString(image,"\033E");  /* printer reset */
      (void) WriteBlobString(image,"\033*r3F");  /* set presentation mode */
      /* define columns and rows in image */
      FormatString(buffer,"\033*r%lus%luT",image->columns,image->rows);
      (void) WriteBlobString(image,buffer);
      FormatString(buffer,"\033*t%luR",density);  /* set resolution */
      (void) WriteBlobString(image,buffer);
      (void) WriteBlobString(image,"\033&l0E");  /* top margin 0 */

      /*
        Determine output type and initialize further accordingly
      */
      if (image->storage_class == DirectClass)
        {
          /*
            Full color
          */
          bits_per_pixel=24;
          (void) WriteBlobString(image,"\033*v6W"); /* set color mode... */
          (void) WriteBlobByte(image,0); /* RGB */
          (void) WriteBlobByte(image,3); /* direct by pixel */
          (void) WriteBlobByte(image,0); /* bits per index (ignored) */
          (void) WriteBlobByte(image,8); /* bits per red component */
          (void) WriteBlobByte(image,8); /* bits per green component */
          (void) WriteBlobByte(image,8); /* bits per blue component */
        }
      else
      if (characteristics.monochrome)
        {
          /*
            Use default printer monochrome setup - NB white = 0, black = 1
          */
          bits_per_pixel=1;
        }
      else
        {
          /*
            PseudoClass
          */
          bits_per_pixel=8;
          (void) WriteBlobString(image,"\033*v6W"); /* set color mode... */
          (void) WriteBlobByte(image,0); /* RGB */
          (void) WriteBlobByte(image,1); /* indexed by pixel */
          (void) WriteBlobByte(image,bits_per_pixel); /* bits per index */
          (void) WriteBlobByte(image,8); /* bits per red component (implicit) */
          (void) WriteBlobByte(image,8); /* bits per green component (implicit) */
          (void) WriteBlobByte(image,8); /* bits per blue component (implicit) */

          /*
            Write colormap to file.
          */
          for (i=0; i < (long)(image->colors); i++)
            {
              FormatString(buffer,"\033*v%da%db%dc%ldI",
                           ScaleQuantumToChar(image->colormap[i].red),
                           ScaleQuantumToChar(image->colormap[i].green),
                           ScaleQuantumToChar(image->colormap[i].blue),
                           i);
              WriteBlobString(image,buffer);
            }
          /*
            Initialize rest of palette with empty entries
          */
          for ( ; i < (1L << bits_per_pixel); i++)
            {
              FormatString(buffer,"\033*v%luI",i);
              /* set index to current component values */
              (void) WriteBlobString(image,buffer);
            }
        }

      /*
        Start raster image
      */
      if  ((AccessDefinition(image_info,"pcl","fit-to-page") != NULL) ||
	   (AccessDefinition(image_info,"pcl","fit_to_page") != NULL))
        (void) WriteBlobString(image,"\033*r3A");  /* start raster graphics with scaling */
      else
        (void) WriteBlobString(image,"\033*r1A");  /* start raster graphics */
      (void) WriteBlobString(image,"\033*b0Y");  /* set y offset */

      /*
        Assign row buffer
      */
      bytes_per_line=(image->columns*bits_per_pixel+7)/8;
      pixels=MagickAllocateMemory(unsigned char *,bytes_per_line);
      if (pixels == (unsigned char *) NULL)
        ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);

      /*
        Set up for compression if desired
      */
      last_row_compression = PCL_UndefinedCompression;
      if (image_info->compression != NoCompression)
        {
          MagickFreeMemory(last_row_pixels);
          last_row_pixels=MagickAllocateMemory(unsigned char *,bytes_per_line);
          if (last_row_pixels == (unsigned char *) NULL)
            {
              MagickFreeMemory(pixels);
              ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
            }
          MagickFreeMemory(output_row);
          output_row=MagickAllocateMemory(unsigned char *,bytes_per_line);
          if (output_row == (unsigned char *) NULL)
            {
              MagickFreeMemory(pixels);
              MagickFreeMemory(last_row_pixels);
              ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
            }
            memset(last_row_pixels,0,bytes_per_line);
        }

      /*
        Convert MIFF to PCL raster pixels.
      */
      zero_rows=0;
      for (y=0; y < (long) image->rows; y++)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          q=pixels;

          if (characteristics.monochrome)
            {
              register unsigned char
                bit,
                byte;
              int
                blk_ind;
              /*
                Monochrome row
              */
              blk_ind = ((image->colormap == NULL) || (image->colormap[0].red == 0)) ? 0 : 1;
              indexes=AccessImmutableIndexes(image);
              bit=0;
              byte=0;
              for (x=0; x < (long) image->columns; x++)
                {
                  byte<<=1;
                  if (indexes[x] == blk_ind) byte |= 1;
                  bit++;
                  if (bit == 8)
                    {
                      *q++=byte;
                      bit=0;
                      byte=0;
                    }
                }
              if (bit != 0)
                *q++=byte << (8-bit);
            }
          else
          if (bits_per_pixel == 8)
            {
              /*
                8 bit PseudoClass row
              */
              indexes=AccessImmutableIndexes(image);
              for (x=0; x < (long) image->columns; x++)
                {
                  *q++=indexes[x];
                }
            }
          else
          if ((bits_per_pixel == 24) || (bits_per_pixel == 32))
            {
              /*
                DirectClass/RGB row
              */
              for (x=0; x < (long) image->columns; x++)
                {
                  *q++=ScaleQuantumToChar(p->red);
                  *q++=ScaleQuantumToChar(p->green);
                  *q++=ScaleQuantumToChar(p->blue);
                  p++;
                }
            }

          if (image_info->compression == NoCompression)
            {
              FormatString(buffer,"\033*b%luW",bytes_per_line);  /* send row */
              (void) WriteBlobString(image,buffer);
              (void) WriteBlob(image,bytes_per_line,pixels);
            }
          else
            {
              compression=PCL_ChooseCompression(bytes_per_line,pixels,last_row_pixels);
              if (compression == PCL_ZeroRowCompression)
                {
                  zero_rows++;
                }
              else
                {
                  /*
                    Skip any omitted zero rows now
                  */
                  if (zero_rows > 0)
                    {
                      i = 32767;
                      do
                        {
                          if (zero_rows < i)
                            i=zero_rows;
                          FormatString(buffer,"\033*b%ldY",i); /* Y Offset */
                          (void) WriteBlobString(image,buffer);
                          zero_rows -= i;
                        } while (zero_rows > 0);
                    }

                  switch (compression)
                    {
                      case PCL_DeltaCompression:
                        {
                          if (compression != last_row_compression)
                            {
                              FormatString(buffer,"\033*b3M");  /* delta compression */
                              (void) WriteBlobString(image,buffer);
                              last_row_compression=compression;
                            }
                          bytes_to_write=PCL_DeltaCompress(bytes_per_line,pixels,
                                                           last_row_pixels,output_row);
                          FormatString(buffer,"\033*b%luW",bytes_to_write);
                          (void) WriteBlobString(image,buffer);
                          WriteBlob(image,bytes_to_write,output_row);
                          break;
                        } 
                      case PCL_TiffRLECompression:
                        {
                          if (compression != last_row_compression)
                            {
                              FormatString(buffer,"\033*b2M");  /* Tiff RLE compression */
                              (void) WriteBlobString(image,buffer);
                              last_row_compression=compression;
                            }
                          bytes_to_write=PCL_TiffRLECompress(bytes_per_line,pixels,output_row);
                          FormatString(buffer,"\033*b%luW",bytes_to_write);
                          (void) WriteBlobString(image,buffer);
                          WriteBlob(image,bytes_to_write,output_row);         
                          break;
                        }
                      case PCL_RLECompression:
                        {
                          if (compression != last_row_compression)
                            {
                              FormatString(buffer,"\033*b1M");  /* RLE compression */
                              (void) WriteBlobString(image,buffer);
                              last_row_compression=compression;
                            }
                          bytes_to_write=PCL_RLECompress(bytes_per_line,pixels,output_row);
                          FormatString(buffer,"\033*b%luW",bytes_to_write);
                          (void) WriteBlobString(image,buffer);
                          WriteBlob(image,bytes_to_write,output_row);         
                          break;
                        }
                      case PCL_RepeatedRowCompression:
                        {
                          compression=PCL_DeltaCompression;
                          if (compression != last_row_compression)
                            {
                              FormatString(buffer,"\033*b3M");  /* delta row compression */
                              (void) WriteBlobString(image,buffer);
                              last_row_compression=compression;
                            }
                          FormatString(buffer,"\033*b0W");  /* no data -> replicate row */
                          (void) WriteBlobString(image,buffer);
                          break;
                        } 
                      case PCL_NoCompression:
                        {
                          if (compression != last_row_compression)
                            {
                              FormatString(buffer,"\033*b0M");  /* no compression */
                              (void) WriteBlobString(image,buffer);
                              last_row_compression=compression;
                            }
                          FormatString(buffer,"\033*b%luW",bytes_per_line);  /* send row */
                          (void) WriteBlobString(image,buffer);
                          (void) WriteBlob(image,bytes_per_line,pixels);
                          break;
                        }
                      case PCL_ZeroRowCompression:
                        {
                          break;
                        }
                      case PCL_UndefinedCompression:
                        {
                          break;
                        }
                    }
                }

            /*
              Swap row with last row
            */
            q=last_row_pixels;
            last_row_pixels=pixels;
            pixels=q;
          }

          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitorFormatted(y,image->rows,&image->exception,
                                          SaveImageText,image->filename,
                                          image->columns,image->rows))
                break;
        }

      (void) WriteBlobString(image,"\033*rB");  /* end graphics */
      MagickFreeMemory(pixels);
      MagickFreeMemory(last_row_pixels);
      MagickFreeMemory(output_row);
      if (image->next == (Image *) NULL)
        break;
      image=SyncNextImageInList(image);
      if ((status &= MagickMonitorFormatted(scene++,
                                            GetImageListLength(image),
                                            &image->exception,
                                            SaveImagesText,
                                            image->filename)) == MagickFail)
        break;
    } while (image_info->adjoin);

  (void) WriteBlobString(image,"\033E");
  CloseBlob(image);
  return(True);
}
