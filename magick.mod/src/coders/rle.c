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
%                            RRRR   L      EEEEE                              %
%                            R   R  L      E                                  %
%                            RRRR   L      EEE                                %
%                            R R    L      E                                  %
%                            R  R   LLLLL  EEEEE                              %
%                                                                             %
%                                                                             %
%                         Read URT RLE Image Format.                          %
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
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/utility.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s R L E                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsRLE returns True if the image format type, identified by the
%  magick string, is RLE.
%
%  The format of the ReadRLEImage method is:
%
%      unsigned int IsRLE(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsRLE returns True if the image format type is RLE.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsRLE(const unsigned char *magick,const size_t length)
{
  if (length < 2)
    return(False);
  if (memcmp(magick,"\122\314",2) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d R L E I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadRLEImage reads a run-length encoded Utah Raster Toolkit
%  image file and returns it.  It allocates the memory necessary for the new
%  Image structure and returns a pointer to the new image.
%
%  The format of the ReadRLEImage method is:
%
%      Image *ReadRLEImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadRLEImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadRLEImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
#define SkipLinesOp  0x01
#define SetColorOp  0x02
#define SkipPixelsOp  0x03
#define ByteDataOp  0x05
#define RunDataOp  0x06
#define EOFOp  0x07

  char
    magick[12];

  Image
    *image;

  int
    opcode,
    operand,
    status;

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

  register unsigned char
    *p;

  size_t
    count;

  unsigned char
    background_color[256],
    *colormap,
    pixel,
    plane,
    *rle_pixels;

  unsigned int
    flags;

  unsigned long
    bits_per_pixel,
    map_length,
    number_colormaps,
    number_pixels,
    number_planes;

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
    Determine if this is a RLE file.
  */
  count=ReadBlob(image,2,(char *) magick);
  if ((count == 0) || (memcmp(magick,"\122\314",2) != 0))
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
  do
  {
    /*
      Read image header.
    */
    (void) ReadBlobLSBShort(image);
    (void) ReadBlobLSBShort(image);
    image->columns=ReadBlobLSBShort(image);
    image->rows=ReadBlobLSBShort(image);
    flags=ReadBlobByte(image);
    image->matte=flags & 0x04;
    number_planes=ReadBlobByte(image);
    bits_per_pixel=ReadBlobByte(image);
    number_colormaps=ReadBlobByte(image);
    map_length=1 << ReadBlobByte(image);
    if ((number_planes == 0) || (number_planes == 2) || (bits_per_pixel != 8) ||
        (image->columns == 0))
      ThrowReaderException(CoderError,DataEncodingSchemeIsNotSupported,image);
    if (flags & 0x02)
      {
        /*
          No background color-- initialize to black.
        */
        for (i=0; i < (long) number_planes; i++)
          background_color[i]=0;
        (void) ReadBlobByte(image);
      }
    else
      {
        /*
          Initialize background color.
        */
        p=background_color;
        for (i=0; i < (long) number_planes; i++)
          *p++=ReadBlobByte(image);
      }
    if ((number_planes & 0x01) == 0)
      (void) ReadBlobByte(image);
    colormap=(unsigned char *) NULL;
    if (number_colormaps != 0)
      {
        /*
          Read image colormaps.
        */
        colormap=MagickAllocateMemory(unsigned char *,number_colormaps*map_length);
        if (colormap == (unsigned char *) NULL)
          ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
            image);
        p=colormap;
        for (i=0; i < (long) number_colormaps; i++)
          for (x=0; x < (long) map_length; x++)
            *p++=ScaleShortToQuantum(ReadBlobLSBShort(image));
      }
    if (flags & 0x08)
      {
        char
          *comment;

        unsigned int
          length;

        /*
          Read image comment.
        */
        length=ReadBlobLSBShort(image);
        comment=MagickAllocateMemory(char *,length);
        if (comment == (char *) NULL)
          ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
            image);
        (void) ReadBlob(image,length-1,comment);
        comment[length-1]='\0';
        (void) SetImageAttribute(image,"comment",comment);
        MagickFreeMemory(comment);
        if ((length & 0x01) == 0)
          (void) ReadBlobByte(image);
      }
    if (image_info->ping && (image_info->subrange != 0))
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    /*
      Allocate RLE pixels.
    */
    if (image->matte)
      number_planes++;
    number_pixels=image->columns*image->rows;
    rle_pixels=MagickAllocateMemory(unsigned char *,number_pixels*number_planes);
    if (rle_pixels == (unsigned char *) NULL)
      ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
    if ((flags & 0x01) && !(flags & 0x02))
      {
        long
          j;

        /*
          Set background color.
        */
        p=rle_pixels;
        for (i=0; i < (long) number_pixels; i++)
        {
          if (!image->matte)
            for (j=0; j < (long) number_planes; j++)
              *p++=background_color[j];
          else
            {
              for (j=0; j < (long) (number_planes-1); j++)
                *p++=background_color[j];
              *p++=0;  /* initialize matte channel */
            }
        }
      }
    /*
      Read runlength-encoded image.
    */
    plane=0;
    x=0;
    y=0;
    opcode=ReadBlobByte(image);
    do
    {
      switch (opcode & 0x3f)
      {
        case SkipLinesOp:
        {
          operand=ReadBlobByte(image);
          if (opcode & 0x40)
            operand=ReadBlobLSBShort(image);
          x=0;
          y+=operand;
          break;
        }
        case SetColorOp:
        {
          operand=ReadBlobByte(image);
          plane=(unsigned char) operand;
          if (plane == 255)
            plane=(unsigned char) (number_planes-1);
          x=0;
          break;
        }
        case SkipPixelsOp:
        {
          operand=ReadBlobByte(image);
          if (opcode & 0x40)
            operand=ReadBlobLSBShort(image);
          x+=operand;
          break;
        }
        case ByteDataOp:
        {
          operand=ReadBlobByte(image);
          if (opcode & 0x40)
            operand=ReadBlobLSBShort(image);
          p=rle_pixels+((image->rows-y-1)*image->columns*number_planes)+
            x*number_planes+plane;
          operand++;
          for (i=0; i < (long) operand; i++)
          {
            pixel=ReadBlobByte(image);
            if ((y < (long) image->rows) && ((x+i) < (long) image->columns))
              *p=pixel;
            p+=number_planes;
          }
          if (operand & 0x01)
            (void) ReadBlobByte(image);
          x+=operand;
          break;
        }
        case RunDataOp:
        {
          operand=ReadBlobByte(image);
          if (opcode & 0x40)
            operand=ReadBlobLSBShort(image);
          pixel=ReadBlobByte(image);
          (void) ReadBlobByte(image);
          operand++;
          p=rle_pixels+((image->rows-y-1)*image->columns*number_planes)+
            x*number_planes+plane;
          for (i=0; i < (long) operand; i++)
          {
            if ((y < (long) image->rows) && ((x+i) < (long) image->columns))
              *p=pixel;
            p+=number_planes;
          }
          x+=operand;
          break;
        }
        default:
          break;
      }
      opcode=ReadBlobByte(image);
    } while (((opcode & 0x3f) != EOFOp) && (opcode != EOF));
    if (number_colormaps != 0)
      {
        unsigned int
          mask;

        /*
          Apply colormap affineation to image.
        */
        mask=(map_length-1);
        p=rle_pixels;
        if (number_colormaps == 1)
          for (i=0; i < (long) number_pixels; i++)
          {
            *p=colormap[*p & mask];
            p++;
          }
        else
          if ((number_planes >= 3) && (number_colormaps >= 3))
            for (i=0; i < (long) number_pixels; i++)
              for (x=0; x < (long) number_planes; x++)
              {
                *p=colormap[x*map_length+(*p & mask)];
                p++;
              }
      }
    /*
      Initialize image structure.
    */
    if (number_planes >= 3)
      {
        /*
          Convert raster image to DirectClass pixel packets.
        */
        p=rle_pixels;
        for (y=0; y < (long) image->rows; y++)
        {
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          for (x=0; x < (long) image->columns; x++)
          {
            q->red=ScaleCharToQuantum(*p++);
            q->green=ScaleCharToQuantum(*p++);
            q->blue=ScaleCharToQuantum(*p++);
            if (image->matte)
              q->opacity=(Quantum) (MaxRGB-ScaleCharToQuantum(*p++));
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
          Create colormap.
        */
        if (number_colormaps == 0)
          map_length=256;
        if (!AllocateImageColormap(image,map_length))
          ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
            image);
        p=colormap;
        if (number_colormaps == 1)
          for (i=0; i < (long) image->colors; i++)
          {
            /*
              Pseudocolor.
            */
            image->colormap[i].red=ScaleCharToQuantum(i);
            image->colormap[i].green=ScaleCharToQuantum(i);
            image->colormap[i].blue=ScaleCharToQuantum(i);
          }
        else
          if (number_colormaps > 1)
            for (i=0; i < (long) image->colors; i++)
            {
              image->colormap[i].red=ScaleCharToQuantum(*p);
              image->colormap[i].green=ScaleCharToQuantum(*(p+map_length));
              image->colormap[i].blue=ScaleCharToQuantum(*(p+map_length*2));
              p++;
            }
        p=rle_pixels;
        if (!image->matte)
          {
            /*
              Convert raster image to PseudoClass pixel packets.
            */
            for (y=0; y < (long) image->rows; y++)
            {
              q=SetImagePixels(image,0,y,image->columns,1);
              if (q == (PixelPacket *) NULL)
                break;
              indexes=AccessMutableIndexes(image);
              for (x=0; x < (long) image->columns; x++)
                indexes[x]=(*p++);
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
        else
          {
            /*
              Image has a matte channel-- promote to DirectClass.
            */
            for (y=0; y < (long) image->rows; y++)
            {
              q=SetImagePixels(image,0,y,image->columns,1);
              if (q == (PixelPacket *) NULL)
                break;
              for (x=0; x < (long) image->columns; x++)
              {
                q->red=image->colormap[*p++].red;
                q->green=image->colormap[*p++].green;
                q->blue=image->colormap[*p++].blue;
                q->opacity=(Quantum) (MaxRGB-ScaleCharToQuantum(*p++));
                q++;
              }
              if (!SyncImagePixels(image))
                break;
              if (image->previous == (Image *) NULL)
                if (QuantumTick(y,image->rows))
                  if (!MagickMonitorFormatted(y,image->rows,exception,
                                              LoadImageText,
                                              image->filename))
                    break;
            }
            MagickFreeMemory(image->colormap);
            image->colormap=(PixelPacket *) NULL;
            image->storage_class=DirectClass;
            image->colors=0;
          }
      }
    if (number_colormaps != 0)
      MagickFreeMemory(colormap);
    MagickFreeMemory(rle_pixels);
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
    (void) ReadBlobByte(image);
    count=ReadBlob(image,2,(char *) magick);
    if ((count != 0) && (memcmp(magick,"\122\314",2) == 0))
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
  } while ((count != 0) && (memcmp(magick,"\122\314",2) == 0));
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
%   R e g i s t e r R L E I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterRLEImage adds attributes for the RLE image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterRLEImage method is:
%
%      RegisterRLEImage(void)
%
*/
ModuleExport void RegisterRLEImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("RLE");
  entry->decoder=(DecoderHandler) ReadRLEImage;
  entry->magick=(MagickHandler) IsRLE;
  entry->adjoin=False;
  entry->description="Utah Run length encoded image";
  entry->module="RLE";
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r R L E I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterRLEImage removes format registrations made by the
%  RLE module from the list of supported formats.
%
%  The format of the UnregisterRLEImage method is:
%
%      UnregisterRLEImage(void)
%
*/
ModuleExport void UnregisterRLEImage(void)
{
  (void) UnregisterMagickInfo("RLE");
}
