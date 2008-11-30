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
%                            TTTTT   GGGG   AAA                               %
%                              T    G      A   A                              %
%                              T    G  GG  AAAAA                              %
%                              T    G   G  A   A                              %
%                              T     GGG   A   A                              %
%                                                                             %
%                                                                             %
%                   Read/Write Truevision Targa Image Format.                 %
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
  Forward declarations.
*/
static unsigned int
  WriteTGAImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d T G A I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadTGAImage reads a Truevision TGA image file and returns it.
%  It allocates the memory necessary for the new Image structure and returns
%  a pointer to the new image.
%
%  The format of the ReadTGAImage method is:
%
%      Image *ReadTGAImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadTGAImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadTGAImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
#define TGAColormap 1          /* Colormapped image data */
#define TGARGB 2               /* Truecolor image data */
#define TGAMonochrome 3        /* Monochrome image data */
#define TGARLEColormap  9      /* Colormapped image data (encoded) */
#define TGARLERGB  10          /* Truecolor image data (encoded) */
#define TGARLEMonochrome  11   /* Monochrome image data (encoded) */

  typedef struct _TGAInfo
  {
    unsigned char
      id_length,       /* Size of Image ID field */
      colormap_type,   /* Color map type */
      image_type;      /* Image type code */

    unsigned short
      colormap_index,  /* Color map origin */
      colormap_length; /* Color map length */

    unsigned char
      colormap_size;   /* Color map entry depth */

    unsigned short
      x_origin,        /* X origin of image */
      y_origin,        /* Y orgin of image */
      width,           /* Width of image */
      height;          /* Height of image */

    unsigned char
      bits_per_pixel,  /* Image pixel size */
      attributes;      /* Image descriptor byte */
  } TGAInfo;

  Image
    *image;

  unsigned int
    index;

  long
    y;

  PixelPacket
    pixel;

  register IndexPacket
    *indexes;

  register long
    i,
    x;

  register PixelPacket
    *q;

  size_t
    count;

  TGAInfo
    tga_info;

  unsigned char
    runlength;

  unsigned int
    alpha_bits,
    status;

  unsigned long
    base,
    flag,
    offset,
    real,
    skip;

  unsigned int
    is_grayscale=False;

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
    Read TGA header information.
  */
  (void) ReadBlob(image,1,(char *) &tga_info.id_length);
  tga_info.colormap_type=ReadBlobByte(image);
  tga_info.image_type=ReadBlobByte(image);
  if (EOFBlob(image))
    ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
  do
    {
      if (((tga_info.image_type != TGAColormap) &&
           (tga_info.image_type != TGARGB) &&
           (tga_info.image_type != TGAMonochrome) &&
           (tga_info.image_type != TGARLEColormap) &&
           (tga_info.image_type != TGARLERGB) &&
           (tga_info.image_type != TGARLEMonochrome)) ||
          (((tga_info.image_type == TGAColormap) ||
            (tga_info.image_type == TGARLEColormap)) &&
           (tga_info.colormap_type == 0)))
        ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
      tga_info.colormap_index=ReadBlobLSBShort(image);
      tga_info.colormap_length=ReadBlobLSBShort(image);
      tga_info.colormap_size=ReadBlobByte(image);
      tga_info.x_origin=ReadBlobLSBShort(image);
      tga_info.y_origin=ReadBlobLSBShort(image);
      tga_info.width=ReadBlobLSBShort(image);
      tga_info.height=ReadBlobLSBShort(image);
      tga_info.bits_per_pixel=ReadBlobByte(image);
      tga_info.attributes=ReadBlobByte(image);
      if (EOFBlob(image))
        ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "ImageType=%s CMapType=%u CMapStart=%u CMapLength=%u CMapDepth=%u\n  XOffset=%u YOffset=%u Width=%u Height=%u PixelDepth=%u Attributes=0x%.2x",
                            ((tga_info.image_type == TGAColormap) ? "Colormapped" :
                             (tga_info.image_type == TGARGB) ? "TrueColor" :
                             (tga_info.image_type == TGAMonochrome) ? "Monochrome" :
                             (tga_info.image_type == TGARLEColormap) ? "Colormapped-RLE" :
                             (tga_info.image_type == TGARLERGB) ? "Truecolor-RLE" :
                             (tga_info.image_type == TGARLEMonochrome) ? "Monochrome-RLE" :
                             "Unknown"),
                            (unsigned int) tga_info.colormap_type,  /* Colormap type */
                            (unsigned int) tga_info.colormap_index, /* Index of first colormap entry to use */
                            (unsigned int) tga_info.colormap_length, /* # of elements in colormap */
                            (unsigned int) tga_info.colormap_size, /* Bits in each palette entry */
                            tga_info.x_origin, tga_info.y_origin,
                            tga_info.width, tga_info.height,
                            (unsigned int) tga_info.bits_per_pixel,
                            tga_info.attributes);

      /*
        Validate depth.
      */
      if (!(((tga_info.bits_per_pixel > 1) && (tga_info.bits_per_pixel < 17)) ||
            (tga_info.bits_per_pixel == 24 ) ||
            (tga_info.bits_per_pixel == 32 )))
        ThrowReaderException(CoderError,DataStorageTypeIsNotSupported,image);

      /*
        Initialize image structure.
      */
      alpha_bits=(tga_info.attributes & 0x0FU);
      image->matte=(alpha_bits > 0);
      image->columns=tga_info.width;
      image->rows=tga_info.height;
      if ((tga_info.image_type != TGAColormap) && (tga_info.image_type != TGARLEColormap))
        {
          /*
            TrueColor Quantum Depth
          */
          image->depth=((tga_info.bits_per_pixel <= 8) ? 8 :
                        (tga_info.bits_per_pixel <= 16) ? 5 :
                        (tga_info.bits_per_pixel == 24) ? 8 :
                        (tga_info.bits_per_pixel == 32) ? 8 : 8);
        }
      else
        {
          /*
            ColorMapped Palette Entry Quantum Depth
          */
          image->depth=((tga_info.colormap_size <= 8) ? 8 :
                        (tga_info.colormap_size <= 16) ? 5 :
                        (tga_info.colormap_size == 24) ? 8 :
                        (tga_info.colormap_size == 32) ? 8 : 8);
        }

      image->storage_class=DirectClass;

      if ((tga_info.image_type == TGAColormap) ||
          (tga_info.image_type == TGAMonochrome) ||
          (tga_info.image_type == TGARLEColormap) ||
          (tga_info.image_type == TGARLEMonochrome))
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Setting PseudoClass");
          image->storage_class=PseudoClass;
        }

      if ((tga_info.image_type == TGARLEColormap) ||
          (tga_info.image_type == TGARLEMonochrome))
        image->compression=RLECompression;

      is_grayscale=((tga_info.image_type == TGAMonochrome) ||
                    (tga_info.image_type == TGARLEMonochrome));

      if (image->storage_class == PseudoClass)
        {
          if (tga_info.colormap_type != 0)
            {
              image->colors=tga_info.colormap_length;
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "Using existing colormap with %u colors.",image->colors);

            }
          else
            {
              /*
                Apply grayscale colormap.
              */
              image->colors=(0x01U << tga_info.bits_per_pixel);
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "Applying grayscale colormap with %u colors.",image->colors);
              if (!AllocateImageColormap(image,image->colors))
                ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
                                     image);
            }
        }

      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "StorageClass=%s Matte=%s Depth=%u Grayscale=%s",
                            ((image->storage_class == DirectClass) ? "DirectClass" : "PseduoClass"),
                            MagickBoolToString(image->matte), image->depth,
                            MagickBoolToString(is_grayscale));
    
      if (tga_info.id_length != 0)
        {
          char
            *comment;

          /*
            TGA image comment.
          */
          comment=MagickAllocateMemory(char *,(size_t) tga_info.id_length+1);
          if (comment == (char *) NULL)
            ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
                                 image);
          if(ReadBlob(image,tga_info.id_length,comment) != tga_info.id_length)
            ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
          comment[tga_info.id_length]='\0';
          (void) SetImageAttribute(image,"comment",comment);
          MagickFreeMemory(comment);
        }
      (void) memset(&pixel,0,sizeof(PixelPacket));
      pixel.opacity=TransparentOpacity;
      if (tga_info.colormap_type != 0)
        {
          /*
            Read TGA raster colormap.
          */
          if (!AllocateImageColormap(image,image->colors))
            ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
                                 image);
          for (i=0; i < (long) image->colors; i++)
            {
              switch (tga_info.colormap_size)
                {
                case 8:
                default:
                  {
                    /*
                      Gray scale.
                    */
                    pixel.red=ScaleCharToQuantum(ReadBlobByte(image));
                    pixel.green=pixel.red;
                    pixel.blue=pixel.red;
                    break;
                  }
                case 15:
                case 16:
                  {
                    /*
                      5 bits each of red green and blue.
                    */
                    unsigned int
                      packet;

                    packet = ReadBlobByte(image);
                    packet |= (((unsigned int) ReadBlobByte(image)) << 8);

                    pixel.red=(packet >> 10) & 0x1f;
                    pixel.red=ScaleCharToQuantum(ScaleColor5to8(pixel.red));
                    pixel.green=(packet >> 5) & 0x1f;
                    pixel.green=ScaleCharToQuantum(ScaleColor5to8(pixel.green));
                    pixel.blue=packet & 0x1f;
                    pixel.blue=ScaleCharToQuantum(ScaleColor5to8(pixel.blue));
                    break;
                  }
                case 24:
                case 32:
                  {
                    /*
                      8 bits each of blue, green and red.
                    */
                    pixel.blue=ScaleCharToQuantum(ReadBlobByte(image));
                    pixel.green=ScaleCharToQuantum(ReadBlobByte(image));
                    pixel.red=ScaleCharToQuantum(ReadBlobByte(image));
                    break;
                  }
                }
              image->colormap[i]=pixel;
            }
        }
      if (EOFBlob(image))
        ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
      if (image_info->ping && (image_info->subrange != 0))
        if (image->scene >= (image_info->subimage+image_info->subrange-1))
          break;
      /*
        Convert TGA pixels to pixel packets.
      */
      base=0;
      flag=0;
      skip=False;
      real=0;
      index=0;
      runlength=0;
      offset=0;
      pixel.opacity=OpaqueOpacity;
      for (y=0; y < (long) image->rows; y++)
        {
          real=offset;
          if (((unsigned char) (tga_info.attributes & 0x20) >> 5) == 0)
            real=image->rows-real-1;
          q=SetImagePixels(image,0,(long) real,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=AccessMutableIndexes(image);
          for (x=0; x < (long) image->columns; x++)
            {
              if ((tga_info.image_type == TGARLEColormap) ||
                  (tga_info.image_type == TGARLERGB) ||
                  (tga_info.image_type == TGARLEMonochrome))
                {
                  if (runlength != 0)
                    {
                      runlength--;
                      skip=flag != 0;
                    }
                  else
                    {
                      if (ReadBlob(image,1,(char *) &runlength) != 1)
                        {
                          status=MagickFail;
                          break;
                        }
                      flag=runlength & 0x80;
                      if (flag != 0)
                        runlength-=128;
                      skip=False;
                    }
                }
              if (!skip)
                switch (tga_info.bits_per_pixel)
                  {
                  case 8:
                  default:
                    {
                      /*
                        Gray scale.
                      */
                      index=ReadBlobByte(image);
                      if (image->storage_class == PseudoClass)
                        {
                          VerifyColormapIndex(image,index);
                          pixel=image->colormap[index];
                        }
                      else
                        {
                          pixel.blue=pixel.green=pixel.red=ScaleCharToQuantum(index);
                        }
                      break;
                    }
                  case 15:
                  case 16:
                    {
                      /*
                        5 bits each of red green and blue.
                      */
                      unsigned int
                        packet;

                      packet = ReadBlobByte(image);
                      packet |= (((unsigned int) ReadBlobByte(image)) << 8);

                      pixel.red=(packet >> 10) & 0x1f;
                      pixel.red=ScaleCharToQuantum(ScaleColor5to8(pixel.red));
                      pixel.green=(packet >> 5) & 0x1f;
                      pixel.green=ScaleCharToQuantum(ScaleColor5to8(pixel.green));
                      pixel.blue=packet & 0x1f;
                      pixel.blue=ScaleCharToQuantum(ScaleColor5to8(pixel.blue));
                      if (image->matte)
                        {
                          if ((packet >> 15) & 0x01)
                            pixel.opacity=OpaqueOpacity;
                          else
                            pixel.opacity=TransparentOpacity;
                        }
                
                      if (image->storage_class == PseudoClass)
                        {
                          index=(IndexPacket) (packet & 0x7fff);
                          VerifyColormapIndex(image,index);
                        }
                      break;
                    }
                  case 24:
                    /*
                      8 bits each of blue green and red.
                    */
                    pixel.blue=ScaleCharToQuantum(ReadBlobByte(image));
                    pixel.green=ScaleCharToQuantum(ReadBlobByte(image));
                    pixel.red=ScaleCharToQuantum(ReadBlobByte(image));
                    break;
                  case 32:
                    {
                      /*
                        8 bits each of blue green and red.
                      */
                      pixel.blue=ScaleCharToQuantum(ReadBlobByte(image));
                      pixel.green=ScaleCharToQuantum(ReadBlobByte(image));
                      pixel.red=ScaleCharToQuantum(ReadBlobByte(image));
                      pixel.opacity=ScaleCharToQuantum(255-ReadBlobByte(image));
                      break;
                    }
                  }
              if (EOFBlob(image))
                status = MagickFail;
              if (status == MagickFail)
                ThrowReaderException(CorruptImageError,UnableToReadImageData,image);
              if (image->storage_class == PseudoClass)
                indexes[x]=index;
              *q++=pixel;
            }
          if (((unsigned char) (tga_info.attributes & 0xc0) >> 6) == 4)
            offset+=4;
          else
            if (((unsigned char) (tga_info.attributes & 0xc0) >> 6) == 2)
              offset+=2;
            else
              offset++;
          if (offset >= image->rows)
            {
              base++;
              offset=base;
            }
          if (!SyncImagePixels(image))
            break;
          image->is_grayscale=is_grayscale;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitorFormatted(y,image->rows,exception,
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
      count=ReadBlob(image,1,(char *) &tga_info.id_length);
      tga_info.colormap_type=ReadBlobByte(image);
      tga_info.image_type=ReadBlobByte(image);
      status=((tga_info.image_type == TGAColormap) ||
              (tga_info.image_type == TGARGB) ||
              (tga_info.image_type == TGAMonochrome) ||
              (tga_info.image_type == TGARLEColormap) ||
              (tga_info.image_type == TGARLERGB) ||
              (tga_info.image_type == TGARLEMonochrome));
      if (!EOFBlob(image) && (status == True))
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
          if (!MagickMonitorFormatted(TellBlob(image),GetBlobSize(image),
                                      exception,LoadImagesText,
                                      image->filename))
            break;
        }
    } while (status == True);
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
%   R e g i s t e r T G A I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterTGAImage adds attributes for the TGA image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterTGAImage method is:
%
%      RegisterTGAImage(void)
%
*/
ModuleExport void RegisterTGAImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("ICB");
  entry->decoder=(DecoderHandler) ReadTGAImage;
  entry->encoder=(EncoderHandler) WriteTGAImage;
  entry->description="Truevision Targa image";
  entry->module="TGA";
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("TGA");
  entry->decoder=(DecoderHandler) ReadTGAImage;
  entry->encoder=(EncoderHandler) WriteTGAImage;
  entry->description="Truevision Targa image";
  entry->module="TGA";
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("VDA");
  entry->decoder=(DecoderHandler) ReadTGAImage;
  entry->encoder=(EncoderHandler) WriteTGAImage;
  entry->description="Truevision Targa image";
  entry->module="TGA";
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("VST");
  entry->decoder=(DecoderHandler) ReadTGAImage;
  entry->encoder=(EncoderHandler) WriteTGAImage;
  entry->description="Truevision Targa image";
  entry->module="TGA";
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r T G A I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterTGAImage removes format registrations made by the
%  TGA module from the list of supported formats.
%
%  The format of the UnregisterTGAImage method is:
%
%      UnregisterTGAImage(void)
%
*/
ModuleExport void UnregisterTGAImage(void)
{
  (void) UnregisterMagickInfo("ICB");
  (void) UnregisterMagickInfo("TGA");
  (void) UnregisterMagickInfo("VDA");
  (void) UnregisterMagickInfo("VST");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e T G A I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteTGAImage writes a image in the Truevision Targa rasterfile
%  format.
%
%  The format of the WriteTGAImage method is:
%
%      unsigned int WriteTGAImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteTGAImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
static unsigned int WriteTGAImage(const ImageInfo *image_info,Image *image)
{
#define TargaColormap 1
#define TargaRGB 2
#define TargaMonochrome 3
#define TargaRLEColormap  9
#define TargaRLERGB  10
#define TargaRLEMonochrome  11

  typedef struct _TargaInfo
  {
    unsigned char
      id_length,
      colormap_type,
      image_type;

    unsigned short
      colormap_index,
      colormap_length;

    unsigned char
      colormap_size;

    unsigned short
      x_origin,
      y_origin,
      width,
      height;

    unsigned char
      bits_per_pixel,
      attributes;
  } TargaInfo;

  const ImageAttribute
    *attribute;

  size_t
    count;

  long
    y;

  register const PixelPacket
    *p;

  register const IndexPacket
    *indexes;

  register long
    x;

  register long
    i;

  register unsigned char
    *q;

  TargaInfo
    targa_info;

  unsigned char
    *targa_pixels;

  unsigned int
    write_grayscale,
    status;

  unsigned long
    scene;

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
  scene=0;
  do
    {
      ImageCharacteristics
        characteristics;        

      write_grayscale=False;

      /*
        If requested output is grayscale, then write grayscale.
      */
      if ((image_info->type == GrayscaleType) ||
          (image_info->type == GrayscaleMatteType))
        write_grayscale=True;

      /*
        Convert colorspace to an RGB-compatible type.
      */
      (void) TransformColorspace(image,RGBColorspace);

      /*
        Analyze image to be written.
      */
      (void) GetImageCharacteristics(image,&characteristics,
                                     (OptimizeType == image_info->type),
                                     &image->exception);

      /*
        If some other type has not been requested and the image is
        grayscale, then write a grayscale image unless the image
        contains an alpha channel.
      */
      if (((image_info->type != TrueColorType) &&
           (image_info->type != TrueColorMatteType) &&
           (image_info->type != PaletteType) &&
           (image->matte == False)) &&
          (characteristics.grayscale))
        write_grayscale=True;

      /*
        If there are too many colors for colormapped output or the
        image contains an alpha channel, then promote to TrueColor.
      */
      if (((write_grayscale == False) &&
           (image->storage_class == PseudoClass) &&
           (image->colors > 256)) ||
          (image->matte == True))
        {
          /* (void) SyncImage(image); */
          image->storage_class=DirectClass;
        }

      /*
        Initialize TGA raster file header.
      */
      targa_info.id_length=0;
      attribute=GetImageAttribute(image,"comment");
      if (attribute != (const ImageAttribute *) NULL)
        targa_info.id_length=Min(strlen(attribute->value),255);
      targa_info.colormap_type=0;
      targa_info.colormap_index=0;
      targa_info.colormap_length=0;
      targa_info.colormap_size=0;
      targa_info.x_origin=0;
      targa_info.y_origin=0;
      targa_info.width=(unsigned short) image->columns;
      targa_info.height=(unsigned short) image->rows;
      targa_info.bits_per_pixel=8;
      targa_info.attributes=0;
      if (write_grayscale == True)
        {
          /*
            Grayscale without Colormap
          */
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Writing Grayscale raster ...");
          targa_info.image_type=TargaMonochrome;
	  targa_info.bits_per_pixel=8;
	  targa_info.colormap_type=0;
          targa_info.colormap_index=0;
          targa_info.colormap_length=0;
          targa_info.colormap_size=0;	
        }
      else if (image->storage_class == DirectClass)
        {
          /*
            Full color TGA raster.
          */
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Writing TrueColor raster ...");

          targa_info.image_type=TargaRGB;
          targa_info.bits_per_pixel=24;
          if (image->matte)
            {
              targa_info.bits_per_pixel=32;
              targa_info.attributes=8;  /* # of alpha bits */
            }
        }
     else
        {
          /*
            Colormapped TGA raster.
          */
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Writing ColorMapped raster ..." );
          targa_info.image_type=TargaColormap;
          targa_info.colormap_type=1;
          targa_info.colormap_index=0;
          targa_info.colormap_length=(unsigned short) image->colors;
          targa_info.colormap_size=24;
        }
      /*
        Write TGA header.
      */
      (void) WriteBlobByte(image,targa_info.id_length);
      (void) WriteBlobByte(image,targa_info.colormap_type);
      (void) WriteBlobByte(image,targa_info.image_type);
      (void) WriteBlobLSBShort(image,targa_info.colormap_index);
      (void) WriteBlobLSBShort(image,targa_info.colormap_length);
      (void) WriteBlobByte(image,targa_info.colormap_size);
      (void) WriteBlobLSBShort(image,targa_info.x_origin);
      (void) WriteBlobLSBShort(image,targa_info.y_origin);
      (void) WriteBlobLSBShort(image,targa_info.width);
      (void) WriteBlobLSBShort(image,targa_info.height);
      (void) WriteBlobByte(image,targa_info.bits_per_pixel);
      (void) WriteBlobByte(image,targa_info.attributes);
      if (targa_info.id_length != 0)
        (void) WriteBlob(image,targa_info.id_length,attribute->value);
      if (targa_info.image_type == TargaColormap)
        {
          unsigned char
            *targa_colormap;

          /*
            Dump colormap to file (blue, green, red byte order).
          */
          targa_colormap=MagickAllocateArray(unsigned char *,
                                             targa_info.colormap_length,3);
          if (targa_colormap == (unsigned char *) NULL)
            ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,
                                 image);
          q=targa_colormap;
          for (i=0; i < (long) image->colors; i++)
            {
              *q++=ScaleQuantumToChar(image->colormap[i].blue);
              *q++=ScaleQuantumToChar(image->colormap[i].green);
              *q++=ScaleQuantumToChar(image->colormap[i].red);
            }
          (void) WriteBlob(image,3*targa_info.colormap_length,
                           (char *) targa_colormap);
          MagickFreeMemory(targa_colormap);
        }
      /*
        Convert MIFF to TGA raster pixels.
      */
      count=(size_t) ((targa_info.bits_per_pixel*targa_info.width) >> 3);
      targa_pixels=MagickAllocateMemory(unsigned char *,count);
      if (targa_pixels == (unsigned char *) NULL)
        ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
      for (y=(long) (image->rows-1); y >= 0; y--)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          q=targa_pixels;
          indexes=AccessImmutableIndexes(image);
          for (x=0; x < (long) image->columns; x++)
            {
              if (targa_info.image_type == TargaColormap)
                {
                  /* Colormapped */
                  *q++=*indexes;
                  indexes++;
                }
              else if (targa_info.image_type == TargaMonochrome)
                {
                  /* Grayscale */
                  if (image->storage_class == PseudoClass)
                    {
                      if (image->is_grayscale)
                        *q++=ScaleQuantumToChar(image->colormap[*indexes].red);
                      else
                        *q++=PixelIntensityToQuantum(&image->colormap[*indexes]);
                      indexes++;
                    }
                  else
                    {
                      if (image->is_grayscale)
                        *q++=ScaleQuantumToChar(p->red);
                      else
                        *q++=PixelIntensityToQuantum(p);
                    }
                }
              else
                {
                  /* TrueColor RGB */
                  *q++=ScaleQuantumToChar(p->blue);
                  *q++=ScaleQuantumToChar(p->green);
                  *q++=ScaleQuantumToChar(p->red);
                  if (image->matte)
                    *q++=ScaleQuantumToChar(MaxRGB-p->opacity);
                }
              p++;
            }
          (void) WriteBlob(image,q-targa_pixels,(char *) targa_pixels);
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitorFormatted(y,image->rows,&image->exception,
                                          SaveImageText,image->filename))
                break;
        }
      MagickFreeMemory(targa_pixels);
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
