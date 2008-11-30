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
%                        IIIII   CCCC   OOO   N   N                           %
%                          I    C      O   O  NN  N                           %
%                          I    C      O   O  N N N                           %
%                          I    C      O   O  N  NN                           %
%                        IIIII   CCCC   OOO   N   N                           %
%                                                                             %
%                                                                             %
%                   Read Microsoft Windows Icon Format.                       %
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
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d I C O N I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadIconImage reads a Microsoft icon image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadIconImage method is:
%
%      Image *ReadIconImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadIconImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadIconImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
#define MaxIcons  256

  typedef struct _IconEntry
  {
    unsigned char
      width,
      height,
      colors,
      reserved;

    short int
      planes,
      bits_per_pixel;

    unsigned long
      size,
      offset;
  } IconEntry;

  typedef struct _IconFile
  {
    short
      reserved,
      resource_type,
      count;

    IconEntry
      directory[MaxIcons];
  } IconFile;

  typedef struct _IconInfo
  {
    unsigned long
      size,
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
  } IconInfo;

  IconFile
    icon_file;

  IconInfo
    icon_info;

  Image
    *image;

  int
    bit,
    byte;

  long
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
  icon_file.reserved=ReadBlobLSBShort(image);
  icon_file.resource_type=ReadBlobLSBShort(image);
  icon_file.count=ReadBlobLSBShort(image);
  /*
    Windows ICO and CUR formats are essentially the same except that
    .CUR uses resource_type==1 while .ICO uses resource_type=2.
  */
  if((icon_file.reserved != 0) || ((icon_file.resource_type != 1) &&
     (icon_file.resource_type != 2)) || (icon_file.count > MaxIcons))
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
  for (i=0; i < icon_file.count; i++)
  {
    icon_file.directory[i].width=ReadBlobByte(image);
    icon_file.directory[i].height=ReadBlobByte(image);
    icon_file.directory[i].colors=ReadBlobByte(image);
    icon_file.directory[i].reserved=ReadBlobByte(image);
    icon_file.directory[i].planes=ReadBlobLSBShort(image);
    icon_file.directory[i].bits_per_pixel=ReadBlobLSBShort(image);
    icon_file.directory[i].size=ReadBlobLSBLong(image);
    icon_file.directory[i].offset=ReadBlobLSBLong(image);
  }
  for (i=0; i < icon_file.count; i++)
  {
    /*
      Verify Icon identifier.
    */
    if (SeekBlob(image,icon_file.directory[i].offset,SEEK_SET) == -1)
      ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
    icon_info.size=ReadBlobLSBLong(image);
    icon_info.width=ReadBlobLSBLong(image);
    icon_info.height=ReadBlobLSBLong(image);
    icon_info.planes=ReadBlobLSBShort(image);
    icon_info.bits_per_pixel=ReadBlobLSBShort(image);
    if (icon_info.bits_per_pixel > 32U)
      ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
    icon_info.compression=ReadBlobLSBLong(image);
    icon_info.image_size=ReadBlobLSBLong(image);
    icon_info.x_pixels=ReadBlobLSBLong(image);
    icon_info.y_pixels=ReadBlobLSBLong(image);
    icon_info.number_colors=ReadBlobLSBLong(image);
    icon_info.colors_important=ReadBlobLSBLong(image);
    image->matte=(unsigned int) (icon_info.bits_per_pixel == 32U);
    image->columns=icon_info.width;
    image->rows=icon_info.height;
    image->depth=8;
    if ((icon_info.number_colors != 0) || (icon_info.bits_per_pixel <= 16U))
      {
        image->storage_class=PseudoClass;
        image->colors=icon_info.number_colors;
        if (image->colors == 0)
          image->colors=1 << icon_info.bits_per_pixel;
      }
  if (image->storage_class == PseudoClass)
    {
      register long
        i;

      unsigned char
        *icon_colormap;

      /*
        Read Icon raster colormap.
      */
      if (!AllocateImageColormap(image,1 << icon_info.bits_per_pixel))
        ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
      icon_colormap=MagickAllocateMemory(unsigned char *,4*image->colors);
      if (icon_colormap == (unsigned char *) NULL)
        ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
      (void) ReadBlob(image,4*image->colors,(char *) icon_colormap);
      p=icon_colormap;
      for (i=0; i < (long) image->colors; i++)
      {
        image->colormap[i].blue=(Quantum) ScaleCharToQuantum(*p++);
        image->colormap[i].green=(Quantum) ScaleCharToQuantum(*p++);
        image->colormap[i].red=(Quantum) ScaleCharToQuantum(*p++);
        p++;
      }
      MagickFreeMemory(icon_colormap);
    }
    /*
      Convert Icon raster image to pixel packets.
    */
    image->columns=icon_file.directory[i].width;
    image->rows=icon_file.directory[i].height;
    if (image_info->ping && (image_info->subrange != 0))
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    switch (icon_info.bits_per_pixel)
    {
      case 1:
      {
        /*
          Convert bitmap scanline.
        */
        for (y=(long) image->rows-1; y >= 0; y--)
        {
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=AccessMutableIndexes(image);
          for (x=0; x < ((long) image->columns-7); x+=8)
          {
            byte=ReadBlobByte(image);
            for (bit=0; bit < 8; bit++)
              indexes[x+bit]=(byte & (0x80 >> bit) ? 0x01 : 0x00);
          }
          if ((image->columns % 8) != 0)
            {
              byte=ReadBlobByte(image);
              for (bit=0; bit < (long) (image->columns % 8); bit++)
                indexes[x+bit]=((byte) & (0x80 >> bit) ? 0x01 : 0x00);
            }
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitorFormatted(image->rows-y-1,image->rows,&image->exception,
                                          LoadImageText,image->filename))
                break;
        }
        break;
      }
      case 4:
      {
        /*
          Read 4-bit Icon scanline.
        */
        for (y=(long) image->rows-1; y >= 0; y--)
        {
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=AccessMutableIndexes(image);
          for (x=0; x < ((long) image->columns-1); x+=2)
          {
            byte=ReadBlobByte(image);
            indexes[x]=(byte >> 4) & 0xf;
            indexes[x+1]=(byte) & 0xf;
          }
          if ((image->columns % 2) != 0)
            {
              byte=ReadBlobByte(image);
              indexes[x]=(byte >> 4) & 0xf;
            }
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitorFormatted(image->rows-y-1,image->rows,&image->exception,
                                          LoadImageText,image->filename))
                break;
        }
        break;
      }
      case 8:
      {
        /*
          Convert PseudoColor scanline.
        */
        for (y=(long) image->rows-1; y >= 0; y--)
        {
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=AccessMutableIndexes(image);
          for (x=0; x < (long) image->columns; x++)
          {
            byte=ReadBlobByte(image);
            indexes[x]=(IndexPacket) byte;
          }
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitorFormatted(image->rows-y-1,image->rows,&image->exception,
                                          LoadImageText,image->filename))
                break;
        }
        break;
      }
      case 16:
      {
        /*
          Convert PseudoColor scanline.
        */
        for (y=(long) image->rows-1; y >= 0; y--)
        {
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=AccessMutableIndexes(image);
          for (x=0; x < (long) image->columns; x++)
          {
            byte=ReadBlobByte(image);
            indexes[x]=(IndexPacket) byte;
            byte=ReadBlobByte(image);
            indexes[x]|=byte << 8;
          }
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitorFormatted(image->rows-y-1,image->rows,&image->exception,
                                          LoadImageText,image->filename))
                break;
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
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          for (x=0; x < (long) image->columns; x++)
          {
            q->blue=(Quantum) ScaleCharToQuantum(ReadBlobByte(image));
            q->green=(Quantum) ScaleCharToQuantum(ReadBlobByte(image));
            q->red=(Quantum) ScaleCharToQuantum(ReadBlobByte(image));
            if (image->matte)
              q->opacity=(Quantum) ScaleCharToQuantum(ReadBlobByte(image));
            q++;
          }
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitorFormatted(image->rows-y-1,image->rows,&image->exception,
                                          LoadImageText,image->filename))
                break;
        }
        break;
      }
      default:
        ThrowReaderException(CorruptImageError,ImproperImageHeader,image)
    }
    (void) SyncImage(image);
    /*
      Convert bitmap scanline to pixel packets.
    */
    image->storage_class=DirectClass;
    image->matte=True;
    for (y=(long) image->rows-1; y >= 0; y--)
    {
      q=GetImagePixels(image,0,y,image->columns,1);
      if (q == (PixelPacket *) NULL)
        break;
      for (x=0; x < ((long) image->columns-7); x+=8)
      {
        byte=ReadBlobByte(image);
        for (bit=0; bit < 8; bit++)
          q[x+bit].opacity=(Quantum) 
            (byte & (0x80 >> bit) ? TransparentOpacity : OpaqueOpacity);
      }
      if ((image->columns % 8) != 0)
        {
          byte=ReadBlobByte(image);
          for (bit=0; bit < (long) (image->columns % 8); bit++)
            q[x+bit].opacity=(Quantum) 
              (byte & (0x80 >> bit) ? TransparentOpacity : OpaqueOpacity);
        }
     if (image->columns % 32) 
       for (x=0; x < (long) ((32-(image->columns % 32))/8); x++)
         (void) ReadBlobByte(image);
      if (!SyncImagePixels(image))
        break;
      if (image->previous == (Image *) NULL)
        if (QuantumTick(y,image->rows))
          if (!MagickMonitorFormatted(image->rows-y-1,image->rows,&image->exception,
                                      LoadImageText,image->filename))
            break;
    }
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
    if (i < (long) (icon_file.count-1))
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
  }
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
%   R e g i s t e r I C O N I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterICONImage adds attributes for the Icon image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterICONImage method is:
%
%      RegisterICONImage(void)
%
*/
ModuleExport void RegisterICONImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("CUR");
  entry->decoder=(DecoderHandler) ReadIconImage;
  entry->adjoin=False;
  entry->seekable_stream=True;
  entry->description="Microsoft Cursor Icon";
  entry->module="ICON";
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("ICO");
  entry->decoder=(DecoderHandler) ReadIconImage;
  entry->adjoin=False;
  entry->seekable_stream=True;
  entry->description="Microsoft Icon";
  entry->module="ICON";
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("ICON");
  entry->decoder=(DecoderHandler) ReadIconImage;
  entry->adjoin=False;
  entry->seekable_stream=True;
  entry->description="Microsoft Icon";
  entry->module="ICON";
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r I C O N I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterICONImage removes format registrations made by the
%  ICON module from the list of supported formats.
%
%  The format of the UnregisterICONImage method is:
%
%      UnregisterICONImage(void)
%
*/
ModuleExport void UnregisterICONImage(void)
{
  (void) UnregisterMagickInfo("CUR");
  (void) UnregisterMagickInfo("ICO");
  (void) UnregisterMagickInfo("ICON");
}
