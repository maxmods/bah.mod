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
#include "magick/color.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/utility.h"

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
  entry->adjoin=False;
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
    x;

  register unsigned char
    *q;

  unsigned int
    status;

  unsigned long
    density;

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
    Initialize the printer.
  */
  (void) WriteBlobString(image,"\033E");  /* printer reset */
  (void) WriteBlobString(image,"\033&l0E");  /* top margin 0 */
  (void) WriteBlobString(image,buffer);
  (void) GetGeometry("75x75",&sans,&sans,&density,&density);
  if (image_info->density != (char *) NULL)
    (void) GetGeometry(image_info->density,&sans,&sans,&density,&density);
  if (characteristics.monochrome)
    {
      register unsigned char
        bit,
        byte;

      /*
        Write PCL monochrome image.
      */
      FormatString(buffer,"\033*t%ldR",density);  /* set resolution */
      (void) WriteBlobString(image,buffer);
      (void) WriteBlobString(image,"\033*r1A");  /* start graphics */
      (void) WriteBlobString(image,"\033*b0M");  /* no compression */
      FormatString(buffer,"\033*b%luW",(image->columns+7)/8);
      (void) WriteBlobString(image,buffer);
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
          byte<<=1U;
          byte|=indexes[x] ? 0x01U : 0x00U;
          bit++;
          if (bit == 8)
            {
              (void) WriteBlobByte(image,byte);
              bit=0;
              byte=0;
            }
        }
        if (bit != 0)
          (void) WriteBlobByte(image,byte << (8-bit));
        if (y < (long) image->rows)
          {
            FormatString(buffer,"\033*b%luW",(image->columns+7)/8);
            (void) WriteBlobString(image,buffer);
          }
        if (QuantumTick(y,image->rows))
          if (!MagickMonitorFormatted(y,image->rows,&image->exception,
                                      SaveImageText,image->filename))
            break;
      }
      (void) WriteBlobString(image,"\033*rB");  /* end graphics */
    }
  else
    {
      static char
        color_mode[6] = { 0, 3, 0, 8, 8, 8 };

      unsigned char
        *pixels;

      /*
        Allocate pixel buffers.
      */
      pixels=MagickAllocateMemory(unsigned char *,
        3*image->columns*sizeof(unsigned char));
      if (pixels == (unsigned char *) NULL)
        ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image)
      /*
        Write PCL color image.
      */
      (void) WriteBlobString(image,"\033*r3F");  /* set presentation mode */
      FormatString(buffer,"\033*t%luR",density);  /* set resolution */
      (void) WriteBlobString(image,buffer);
      FormatString(buffer,"\033*r%luT",image->rows);
      (void) WriteBlobString(image,buffer);
      FormatString(buffer,"\033*r%luS",image->columns);
      (void) WriteBlobString(image,buffer);
      (void) WriteBlobString(image,"\033*v6W");  /* set color mode */
      (void) WriteBlob(image,6,color_mode);
      (void) WriteBlobString(image,"\033*r1A");  /* start raster graphics */
      (void) WriteBlobString(image,"\033*b0Y");  /* set y offset */
      (void) WriteBlobString(image,"\033*b0M");  /* no compression */
      for (y=0; y < (long) image->rows; y++)
      {
        p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
        if (p == (const PixelPacket *) NULL)
          break;
        q=pixels;
        for (x=0; x < (long) image->columns; x++)
        {
          *q++=ScaleQuantumToChar(p->red);
          *q++=ScaleQuantumToChar(p->green);
          *q++=ScaleQuantumToChar(p->blue);
          p++;
        }
        FormatString(buffer,"\033*b%luW",3*image->columns);  /* send row */
        (void) WriteBlobString(image,buffer);
        (void) WriteBlob(image,3*image->columns,pixels);
        if (QuantumTick(y,image->rows))
          if (!MagickMonitorFormatted(y,image->rows,&image->exception,
                                      SaveImageText,image->filename))
            break;
      }
      (void) WriteBlobString(image,"\033*r0C");  /* end graphics */
      MagickFreeMemory(pixels);
    }
  (void) WriteBlobString(image,"\033E");
  CloseBlob(image);
  return(True);
}
