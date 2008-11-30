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
%                            SSSSS   GGGG  IIIII                              %
%                            SS     G        I                                %
%                             SSS   G  GG    I                                %
%                               SS  G   G    I                                %
%                            SSSSS   GGG   IIIII                              %
%                                                                             %
%                                                                             %
%                     Read/Write Irix RGB Image Format.                       %
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
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/utility.h"

/*
  Typedef declaractions.
*/

/*
  The SGI file header is 512 bytes in size.
*/
typedef struct _SGIInfo
{
  unsigned short
    magic;                  /* Identification number (474) */

  magick_int8_t
    storage,                /* Compression flag */
    bytes_per_pixel;        /* Bytes per pixel (per bit plane) */

  magick_uint16_t
    dimension,              /* Number of image dimensions */
    xsize,                  /* Width of image in pixels */
    ysize,                  /* Height of image in pixels */
    zsize;                  /* Number of bit planes */

  magick_uint32_t
    pix_min,                /* Smallest pixel component value */
    pix_max;                /* Largest pixel component value */

  char
    dummy1[4];              /* Not used */

  char
    image_name[80];         /* Name of image (null terminated) */

  magick_uint32_t
    color_map;              /* Format of pixel data */

  char
    dummy2[404];            /* Not used */
} SGIInfo;

/*
  Forward declarations.
*/
static unsigned int
  WriteSGIImage(const ImageInfo *,Image *);
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s S G I                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsSGI returns True if the image format type, identified by the
%  magick string, is SGI.
%
%  The format of the IsSGI method is:
%
%      unsigned int IsSGI(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsSGI returns True if the image format type is SGI.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsSGI(const unsigned char *magick,const size_t length)
{
  if (length < 2)
    return(False);
  if (memcmp(magick,"\001\332",2) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d S G I I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadSGIImage reads a SGI RGB image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadSGIImage method is:
%
%      Image *ReadSGIImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadSGIImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/

static int SGIDecode(const unsigned long bytes_per_pixel,
  unsigned char *max_packets,unsigned char *pixels,
  unsigned long npackets,unsigned long npixels)
{
  unsigned long
    count;

  register unsigned char
    *p,
    *q;

  unsigned int
    pixel;

  p=max_packets;
  q=pixels;
  if (bytes_per_pixel == 2)
    {
      for ( ; ; )
      {
         if (npackets-- == 0)
           return -1;
        pixel=(*p++) << 8U;
        pixel|=(*p++);
        count=(pixel & 0x7fU);
        if (count == 0)
          break;
        if (count > npixels)
          return -1;
        npixels -= count;
        if (pixel & 0x80U)
          for ( ; count != 0U; count--)
          {
            if (npackets-- == 0)
              return -1;
            *q=(*p++);
            *(q+1)=(*p++);
            q+=8U;
          }
        else
          {
            if (npackets-- == 0)
              return -1;
            pixel=(*p++) << 8U;
            pixel|=(*p++);
            for ( ; count != 0; count--)
            {
              *q=(unsigned char) (pixel >> 8U);
              *(q+1)=(unsigned char) pixel;
              q+=8U;
            }
          }
      }
      return 0;
    }
  for ( ; ; )
  {
    if (npackets-- == 0)
      return -1;
    pixel=(*p++);
    count= (pixel & 0x7fU);
    if (count == 0)
      break;
    if (count > npixels)
      return -1;
    npixels -= count;
    if (pixel & 0x80)
      for ( ; count != 0; count--)
      {
        if (npackets-- == 0)
          return -1;
        *q=(*p++);
        q+=4;
      }
    else
      {
        if (npackets-- == 0)
          return -1;
        pixel=(*p++);
        for ( ; count != 0; count--)
        {
          *q=(unsigned char) pixel;
          q+=4;
        }
      }
  }
  return 0;
}

static Image *ReadSGIImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image
    *image;

  long
    y,
    z;

  register IndexPacket
    *indexes;

  register long
    i,
    x;

  register PixelPacket
    *q;

  register unsigned char
    *p;

  SGIInfo
    iris_info;

  unsigned char
    *iris_pixels;

  unsigned int
    status;

  unsigned long
    bytes_per_pixel,
    number_pixels;

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
    Read SGI raster header.
  */
  iris_info.magic=ReadBlobMSBShort(image);
  do
  {
    /*
      Verify SGI identifier.
    */
    if (iris_info.magic != 0x01DA)
      ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
    iris_info.storage=ReadBlobByte(image);
    iris_info.bytes_per_pixel=ReadBlobByte(image);
    iris_info.dimension=ReadBlobMSBShort(image);
    iris_info.xsize=ReadBlobMSBShort(image);
    iris_info.ysize=ReadBlobMSBShort(image);
    iris_info.zsize=ReadBlobMSBShort(image);
    iris_info.pix_min=ReadBlobMSBLong(image);
    iris_info.pix_max=ReadBlobMSBLong(image);

    (void) ReadBlob(image,(unsigned int) sizeof(iris_info.dummy1),
                    iris_info.dummy1);
    (void) ReadBlob(image,(unsigned int) sizeof(iris_info.image_name),
                    iris_info.image_name);
    iris_info.image_name[sizeof(iris_info.image_name)-1]=0;
    iris_info.color_map=ReadBlobMSBLong(image);
    (void) ReadBlob(image,(unsigned int) sizeof(iris_info.dummy2),
                    iris_info.dummy2);

    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "    Header: Storage=%u, BPC=%u, Dimension=%u, XSize=%u, YSize=%u, ZSize=%u, PixMin=%u, PixMax=%u, image_name=\"%.79s\", color_map=%u",
                          (unsigned int) iris_info.storage,
                          (unsigned int) iris_info.bytes_per_pixel,
                          (unsigned int) iris_info.dimension,
                          (unsigned int) iris_info.xsize,
                          (unsigned int) iris_info.ysize,
                          (unsigned int) iris_info.zsize,
                          iris_info.pix_min,
                          iris_info.pix_max,
                          iris_info.image_name,
                          iris_info.color_map);

    /*
      Validate image header and set image attributes.
    */
    if (iris_info.storage == 0)
      {
        /* Uncompressed */
        image->compression=NoCompression;
      }
    else if (iris_info.storage == 1)
      {
        /* RLE compressed */
        image->compression=RLECompression;
      }
    else
      {
        /* Error */
        ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
      }

    if (iris_info.color_map != 0)
      {
        /* We only support images with normal (no) colormap */
        ThrowReaderException(CorruptImageError,ImageTypeNotSupported,image);
      }

    if (iris_info.bytes_per_pixel == 1)
      {
        /* 8 bits per sample */
        image->depth=8;
      }
    else if (iris_info.bytes_per_pixel == 2)
      {
        /* 16 bits per sample */
        image->depth=Min(16,QuantumDepth);
      }
    else
      {
        /* Error */
        ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
      }

    if (iris_info.dimension == 1)
      {
        /*
          Image contains one channel and one scanline, with scanline
          length specified by xsize.
        */
        image->columns=iris_info.xsize;
        image->rows=1;
        image->is_grayscale=MagickTrue;
        if (iris_info.bytes_per_pixel == 1)
          {
            /* Use a grayscale colormap */
            image->storage_class=PseudoClass;
            image->colors=256;
          }
      }
    else if (iris_info.dimension == 2)
      {
        /*
          One channel with a number of scan lines.  Width and height
          of image are given by the values of xsize and ysize.
        */
        image->columns=iris_info.xsize;
        image->rows=iris_info.ysize;
        image->is_grayscale=MagickTrue;
        if (iris_info.bytes_per_pixel == 1)
          {
            /* Use a grayscale colormap */
            image->storage_class=PseudoClass;
            image->colors=256;
          }
      }
    else if (iris_info.dimension == 3)
      {
        /*
          A number of channels.  Width and height of image are given
          by the values of xsize and ysize. The number of channels is
          specified by zsize.

          B/W images have a zsize of 1.  RGB images have a zsize of 3.
          RGB images with an alpha channel have a zsize of 4. Images
          may have more than four channels but the meaning of
          additional channels is implementation dependent.
        */

        image->columns=iris_info.xsize;
        image->rows=iris_info.ysize;

        if (iris_info.zsize == 1)
          {
            /* B/W image */
            image->matte = MagickFalse;
            image->is_grayscale=MagickTrue;
            if (iris_info.bytes_per_pixel == 1)
              {
                /* Use a grayscale colormap */
                image->storage_class=PseudoClass;
                image->colors=256;
              }
          }
        else if (iris_info.zsize == 3)
          {
            /* RGB */
            image->matte=MagickFalse;
          }
        else if (iris_info.zsize == 4)
          {
            /* RGBA  */
            image->matte=MagickTrue;
          }
        else
          {
            /* Error */
            ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
          }
      }
    else
      {
        /* Error */
        ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
      }

    if (iris_info.image_name[0])
      (void) SetImageAttribute(image,"comment",iris_info.image_name);

    if (image_info->ping && (image_info->subrange != 0))
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;

    /*
      Allocate SGI pixels.
    */
    bytes_per_pixel=iris_info.bytes_per_pixel;
    number_pixels=iris_info.xsize*iris_info.ysize;
    if (4*bytes_per_pixel*number_pixels < number_pixels) /* Overflow? */
      ThrowReaderException(CorruptImageError,InsufficientImageDataInFile,image);
    iris_pixels=MagickAllocateMemory(unsigned char *,
      4*bytes_per_pixel*number_pixels);
    if (iris_pixels == (unsigned char *) NULL)
      ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
    if (iris_info.storage != 0x01)
      {
        unsigned char
          *scanline;

        /*
          Read standard image format.
        */
        scanline=MagickAllocateMemory(unsigned char *,
          bytes_per_pixel*iris_info.xsize);
        if (scanline == (unsigned char *) NULL)
          ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
            image);
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "   Reading SGI scanlines"); 
        for (z=0; z < (int) iris_info.zsize; z++)
        {
          p=iris_pixels+bytes_per_pixel*z;
          for (y=0; y < (long) iris_info.ysize; y++)
          {
            (void) ReadBlob(image,bytes_per_pixel*iris_info.xsize,
              (char *) scanline);
            if (EOFBlob(image))
              {
                ThrowReaderException(CorruptImageError,
                  UnexpectedEndOfFile, image);
                break;
              }
            if (bytes_per_pixel == 2)
              for (x=0; x < (long) iris_info.xsize; x++)
              {
                *p=scanline[2*x];
                *(p+1)=scanline[2*x+1];
                p+=8;
              }
            else
              for (x=0; x < (long) iris_info.xsize; x++)
              {
                *p=scanline[x];
                p+=4;
              }
          }
          if (EOFBlob(image))
            break;
        }
        MagickFreeMemory(scanline);
      }
    else
      {
        unsigned char
          *max_packets;

        unsigned int
          data_order;

        unsigned long
          offset,
          *offsets,
          *runlength;

        /*
          Read runlength-encoded image format.
        */
        offsets=MagickAllocateMemory(unsigned long *,iris_info.ysize*
          iris_info.zsize*sizeof(unsigned long));
        max_packets=MagickAllocateArray(unsigned char *,iris_info.xsize+10,4);
        runlength=MagickAllocateMemory(unsigned long *,iris_info.ysize*
          iris_info.zsize*sizeof(unsigned long));
        if ((offsets == (unsigned long *) NULL) ||
            (max_packets == (unsigned char *) NULL) ||
            (runlength == (unsigned long *) NULL))
          ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
            image);
        for (i=0; i < (int) (iris_info.ysize*iris_info.zsize); i++)
          offsets[i]=ReadBlobMSBLong(image);
        for (i=0; i < (int) (iris_info.ysize*iris_info.zsize); i++)
        {
          runlength[i]=ReadBlobMSBLong(image);
          if (EOFBlob(image))
            ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
          if (runlength[i] > ((unsigned long) 4*iris_info.xsize+10))
            ThrowReaderException(CorruptImageError,UnableToRunlengthDecodeImage,image);
        }
        /*
          Check data order.
        */
        offset=0;
        data_order=0;
        for (y=0; ((y < (long) iris_info.ysize) && !data_order); y++)
          for (z=0; ((z < (int) iris_info.zsize) && !data_order); z++)
          {
            if (offsets[y+z*iris_info.ysize] < offset)
              data_order=1;
            offset=offsets[y+z*iris_info.ysize];
          }
        offset=TellBlob(image);
        if (data_order == 1)
          {
            for (z=0; z < (int) iris_info.zsize; z++)
            {
              p=iris_pixels;
              for (y=0; y < (long) iris_info.ysize; y++)
              {
                if (offset != offsets[y+z*iris_info.ysize])
                  {
                    offset=offsets[y+z*iris_info.ysize];
                    (void) SeekBlob(image,(long) offset,SEEK_SET);
                  }
                (void) ReadBlob(image,runlength[y+z*iris_info.ysize],
                  (char *) max_packets);
                if (EOFBlob(image))
                  {
                    ThrowReaderException(CorruptImageError,
                      UnexpectedEndOfFile, image);
                    break;
                  }
                offset+=runlength[y+z*iris_info.ysize];
                if (SGIDecode(bytes_per_pixel,max_packets,p+bytes_per_pixel*z,
		              runlength[y+z*iris_info.ysize]/bytes_per_pixel,
                              iris_info.xsize) == -1)
	          ThrowReaderException(CorruptImageError,
                                       UnableToRunlengthDecodeImage,image); 
                p+=(iris_info.xsize*4*bytes_per_pixel);
              }
            }
          }
        else
          {
            p=iris_pixels;
            for (y=0; y < (long) iris_info.ysize; y++)
            {
              for (z=0; z < (int) iris_info.zsize; z++)
              {
                if (offset != offsets[y+z*iris_info.ysize])
                  {
                    offset=offsets[y+z*iris_info.ysize];
                    (void) SeekBlob(image,(long) offset,SEEK_SET);
                  }
                (void) ReadBlob(image,runlength[y+z*iris_info.ysize],
                  (char *) max_packets);
                if (EOFBlob(image))
                  {
                    ThrowReaderException(CorruptImageError,
                      UnexpectedEndOfFile, image);
                    break;
                  }
                offset+=runlength[y+z*iris_info.ysize];
                if (SGIDecode(bytes_per_pixel,max_packets,p+bytes_per_pixel*z,
		              runlength[y+z*iris_info.ysize]/bytes_per_pixel,
                              iris_info.xsize) == -1)
	          ThrowReaderException(CorruptImageError,
                                       UnableToRunlengthDecodeImage,image); 
              }
              p+=(iris_info.xsize*4*bytes_per_pixel);
              if (EOFBlob(image))
                break;
            }
          }
        MagickFreeMemory(runlength);
        MagickFreeMemory(max_packets);
        MagickFreeMemory(offsets);
      }

    /*
      Convert SGI raster image to pixel packets.
    */
    if (image->storage_class == DirectClass)
      {
        /*
          Convert SGI image to DirectClass pixel packets.
        */
        if (bytes_per_pixel == 2)
          {
            for (y=0; y < (long) image->rows; y++)
            {
              p=iris_pixels+(image->rows-y-1)*8*image->columns;
              q=SetImagePixels(image,0,y,image->columns,1);
              if (q == (PixelPacket *) NULL)
                break;
              for (x=0; x < (long) image->columns; x++)
              {
                q->red=ScaleShortToQuantum((*(p+0) << 8) | (*(p+1)));
                q->green=ScaleShortToQuantum((*(p+2) << 8) | (*(p+3)));
                q->blue=ScaleShortToQuantum((*(p+4) << 8) | (*(p+5)));
                if (image->matte)
                  q->opacity=(Quantum)
                    (MaxRGB-ScaleShortToQuantum((*(p+6) << 8) | (*(p+7))));
                else
                  q->opacity=OpaqueOpacity;
                p+=8;
                q++;
              }
              if (!SyncImagePixels(image))
                break;
              if (image->previous == (Image *) NULL)
                if (QuantumTick(y,image->rows))
                  if (!MagickMonitorFormatted(y,image->rows,exception,
                                              LoadImageText,image->filename))
                    break;
            }
          }
        else
          for (y=0; y < (long) image->rows; y++)
          {
            p=iris_pixels+(image->rows-y-1)*4*image->columns;
            q=SetImagePixels(image,0,y,image->columns,1);
            if (q == (PixelPacket *) NULL)
              break;
            for (x=0; x < (long) image->columns; x++)
            {
              q->red=ScaleCharToQuantum(*p);
              q->green=ScaleCharToQuantum(*(p+1));
              q->blue=ScaleCharToQuantum(*(p+2));
              if (image->matte)
                q->opacity=(Quantum) (MaxRGB-ScaleCharToQuantum(*(p+3)));
              else
                q->opacity=OpaqueOpacity;
              p+=4;
              q++;
            }
            if (!SyncImagePixels(image))
              break;
            if (image->previous == (Image *) NULL)
              if (QuantumTick(y,image->rows))
                if (!MagickMonitorFormatted(y,image->rows,exception,
                                            LoadImageText,image->filename))
                  break;
          }
      }
    else
      {
        /*
          Create grayscale map.
        */
        if (!AllocateImageColormap(image,image->colors))
          ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
            image);
        /*
          Convert SGI image to PseudoClass pixel packets.
        */
        if (bytes_per_pixel == 2)
          {
            for (y=0; y < (long) image->rows; y++)
            {
              p=iris_pixels+(image->rows-y-1)*8*image->columns;
              q=SetImagePixels(image,0,y,image->columns,1);
              if (q == (PixelPacket *) NULL)
                break;
              indexes=AccessMutableIndexes(image);
              for (x=0; x < (long) image->columns; x++)
              {
                indexes[x]=(*p << 8);
                indexes[x]|=(*(p+1));
                p+=8;
                q++;
              }
              if (!SyncImagePixels(image))
                break;
              if (image->previous == (Image *) NULL)
                if (QuantumTick(y,image->rows))
                  if (!MagickMonitorFormatted(y,image->rows,exception,
                                              LoadImageText,image->filename))
                    break;
            }
          }
        else
          for (y=0; y < (long) image->rows; y++)
          {
            p=iris_pixels+(image->rows-y-1)*4*image->columns;
            q=SetImagePixels(image,0,y,image->columns,1);
            if (q == (PixelPacket *) NULL)
              break;
            indexes=AccessMutableIndexes(image);
            for (x=0; x < (long) image->columns; x++)
            {
              indexes[x]=(*p);
              p+=4;
              q++;
            }
            if (!SyncImagePixels(image))
              break;
            if (image->previous == (Image *) NULL)
              if (QuantumTick(y,image->rows))
                if (!MagickMonitorFormatted(y,image->rows,exception,
                                            LoadImageText,image->filename))
                  break;
          }
        (void) SyncImage(image);
      }
    MagickFreeMemory(iris_pixels);
    if (EOFBlob(image))
      {
        ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,
          image);
        break;
      }
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    iris_info.magic=ReadBlobMSBShort(image);
    if (iris_info.magic == 0x01DA)
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
  } while (iris_info.magic == 0x01DA);
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
%   R e g i s t e r S G I I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterSGIImage adds attributes for the SGI image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterSGIImage method is:
%
%      RegisterSGIImage(void)
%
*/
ModuleExport void RegisterSGIImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("SGI");
  entry->decoder=(DecoderHandler) ReadSGIImage;
  entry->encoder=(EncoderHandler) WriteSGIImage;
  entry->magick=(MagickHandler) IsSGI;
  entry->description="Irix RGB image";
  entry->module="SGI";
  entry->seekable_stream=True;
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r S G I I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterSGIImage removes format registrations made by the
%  SGI module from the list of supported formats.
%
%  The format of the UnregisterSGIImage method is:
%
%      UnregisterSGIImage(void)
%
*/
ModuleExport void UnregisterSGIImage(void)
{
  (void) UnregisterMagickInfo("SGI");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e S G I I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteSGIImage writes an image in SGI RGB encoded image format.
%
%  The format of the WriteSGIImage method is:
%
%      unsigned int WriteSGIImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteSGIImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/

static size_t SGIEncode(unsigned char *pixels,size_t count,
  unsigned char *packets)
{
  short
    runlength;

  register unsigned char
    *p,
    *q;

  unsigned char
    *limit,
    *mark;

  p=pixels;
  limit=p+count*4;
  q=packets;
  while (p < limit)
  {
    mark=p;
    p+=8;
    while ((p < limit) && ((*(p-8) != *(p-4)) || (*(p-4) != *p)))
      p+=4;
    p-=8;
    count=((p-mark) >> 2);
    while (count)
    {
      runlength=(short) (count > 126 ? 126 : count);
      count-=runlength;
      *q++=0x80 | runlength;
      for ( ; runlength > 0; runlength--)
      {
        *q++=(*mark);
        mark+=4;
      }
    }
    mark=p;
    p+=4;
    while ((p < limit) && (*p == *mark))
      p+=4;
    count=((p-mark) >> 2);
    while (count)
    {
      runlength=(short) (count > 126 ? 126 : count);
      count-=runlength;
      *q++=(unsigned char) runlength;
      *q++=(*mark);
    }
  }
  *q++=0;
  return(q-packets);
}

static unsigned int WriteSGIImage(const ImageInfo *image_info,Image *image)
{
  long
    y,
    z;

  SGIInfo
    iris_info;

  register const PixelPacket
    *p;

  register long
    i,
    x;

  register unsigned char
    *q;

  unsigned char
    *iris_pixels,
    *packets;

  unsigned int
    status;

  unsigned long
    number_pixels,
    scene;

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
  if (status == MagickFail)
    ThrowWriterException(FileOpenError,UnableToOpenFile,image);
  scene=0;
  do
  {
    ImageCharacteristics
      characteristics;

    /*
      Ensure that image is in an RGB space.
    */
    (void) TransformColorspace(image,RGBColorspace);
    /*
      Analyze image to be written.
    */
    (void) GetImageCharacteristics(image,&characteristics,
                                   (OptimizeType == image_info->type),
                                   &image->exception);
    /*
      Initialize SGI raster file header.
    */
    iris_info.magic=0x01DA;
    if (image_info->compression == NoCompression)
      iris_info.storage=0x00;
    else
      iris_info.storage=0x01;
    iris_info.bytes_per_pixel=1;  /* one byte per pixel */
    iris_info.dimension=3;
    iris_info.xsize=(unsigned short) image->columns;
    iris_info.ysize=(unsigned short) image->rows;
    if (image->matte != MagickFalse)
      iris_info.zsize=4;
    else
      {
        if ((image_info->type != TrueColorType) &&
            (characteristics.grayscale))
          {
            iris_info.dimension=2;
            iris_info.zsize=1;
          }
        else
          iris_info.zsize=3;
      }
    iris_info.pix_min=0;
    iris_info.pix_max=ScaleQuantumToChar(MaxRGB);

    (void) memset(iris_info.dummy1,0,sizeof(iris_info.dummy1));
    {
      const ImageAttribute
        *attribute;

      (void) memset(iris_info.image_name,0,sizeof(iris_info.image_name));
      if ((attribute=GetImageAttribute(image,"comment")))
        (void) strlcpy(iris_info.image_name,attribute->value,sizeof(iris_info.image_name));
    }
    iris_info.color_map=0;
    (void) memset(iris_info.dummy2,0,sizeof(iris_info.dummy2));

    /*
      Write SGI header.
    */
    (void) WriteBlobMSBShort(image,iris_info.magic);
    (void) WriteBlobByte(image,iris_info.storage);
    (void) WriteBlobByte(image,iris_info.bytes_per_pixel);
    (void) WriteBlobMSBShort(image,iris_info.dimension);
    (void) WriteBlobMSBShort(image,iris_info.xsize);
    (void) WriteBlobMSBShort(image,iris_info.ysize);
    (void) WriteBlobMSBShort(image,iris_info.zsize);
    (void) WriteBlobMSBLong(image,iris_info.pix_min);
    (void) WriteBlobMSBLong(image,iris_info.pix_max);
    (void) WriteBlob(image,sizeof(iris_info.dummy1),
                     (char *) iris_info.dummy1);
    (void) WriteBlob(image,sizeof(iris_info.image_name),
                     (char *) iris_info.image_name);
    (void) WriteBlobMSBLong(image,iris_info.color_map);
    (void) WriteBlob(image,sizeof(iris_info.dummy2),
      (char *) iris_info.dummy2);
    /*
      Allocate SGI pixels.
    */
    number_pixels=image->columns*image->rows;
    iris_pixels=MagickAllocateMemory(unsigned char *,4*number_pixels);
    if (iris_pixels == (unsigned char *) NULL)
      ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
    /*
      Convert image pixels to uncompressed SGI pixels.
    */
    for (y=0; y < (long) image->rows; y++)
    {
      p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
      if (p == (const PixelPacket *) NULL)
        break;
      q=iris_pixels+((iris_info.ysize-1)-y)*(iris_info.xsize*4);
      for (x=0; x < (long) image->columns; x++)
      {
        *q++=ScaleQuantumToChar(p->red);
        *q++=ScaleQuantumToChar(p->green);
        *q++=ScaleQuantumToChar(p->blue);
        *q++=ScaleQuantumToChar(MaxRGB-p->opacity);
        p++;
      }
      if (image->previous == (Image *) NULL)
        if (QuantumTick(y,image->rows))
          if (!MagickMonitorFormatted(y,image->rows,&image->exception,
                                      SaveImageText,image->filename))
            break;
    }
    if (image_info->compression == NoCompression)
      {
        unsigned char
          *scanline;

        /*
          Write uncompressed SGI pixels.
        */
        scanline=MagickAllocateMemory(unsigned char *,iris_info.xsize);
        if (scanline == (unsigned char *) NULL)
          ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,
            image);
        for (z=0; z < (int) iris_info.zsize; z++)
        {
          q=iris_pixels+z;
          for (y=0; y < (long) iris_info.ysize; y++)
          {
            for (x=0; x < (long) iris_info.xsize; x++)
            {
              scanline[x]=(*q);
              q+=4;
            }
            (void) WriteBlob(image,iris_info.xsize,(char *) scanline);
          }
        }
        MagickFreeMemory(scanline);
      }
    else
      {
        unsigned long
          length,
          number_packets,
          offset,
          *offsets,
          *runlength;

        /*
          Convert SGI uncompressed pixels.
        */
        offsets=MagickAllocateMemory(unsigned long *,iris_info.ysize*
          iris_info.zsize*sizeof(unsigned long));
        packets=MagickAllocateMemory(unsigned char *,
          4*(2*iris_info.xsize+10)*image->rows);
        runlength=MagickAllocateMemory(unsigned long *,iris_info.ysize*
          iris_info.zsize*sizeof(unsigned long));
        if ((offsets == (unsigned long *) NULL) ||
            (packets == (unsigned char *) NULL) ||
            (runlength == (unsigned long *) NULL))
          ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,
            image);
        offset=512+4*2*(iris_info.ysize*iris_info.zsize);
        number_packets=0;
        q=iris_pixels;
        for (y=0; y < (long) iris_info.ysize; y++)
        {
          for (z=0; z < (int) iris_info.zsize; z++)
          {
            length=
              SGIEncode(q+z,(int) iris_info.xsize,packets+number_packets);
            number_packets+=length;
            offsets[y+z*iris_info.ysize]=offset;
            runlength[y+z*iris_info.ysize]=length;
            offset+=length;
          }
          q+=(iris_info.xsize*4);
        }
        /*
          Write out line start and length tables and runlength-encoded pixels.
        */
        for (i=0; i < (int) (iris_info.ysize*iris_info.zsize); i++)
          (void) WriteBlobMSBLong(image,offsets[i]);
        for (i=0; i < (int) (iris_info.ysize*iris_info.zsize); i++)
          (void) WriteBlobMSBLong(image,runlength[i]);
        (void) WriteBlob(image,number_packets,(char *) packets);
        /*
          Free memory.
        */
        MagickFreeMemory(runlength);
        MagickFreeMemory(packets);
        MagickFreeMemory(offsets);
      }
    MagickFreeMemory(iris_pixels);
    if (image->next == (Image *) NULL)
      break;
    image=SyncNextImageInList(image);
    if (!MagickMonitorFormatted(scene++,GetImageListLength(image),
                                &image->exception,SaveImagesText,
                                image->filename))
      break;
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseBlob(image);
  return(True);
}
