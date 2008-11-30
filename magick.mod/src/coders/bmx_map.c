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
%                            M   M   AAA   PPPP                               %
%                            MM MM  A   A  P   P                              %
%                            M M M  AAAAA  PPPP                               %
%                            M   M  A   A  P                                  %
%                            M   M  A   A  P                                  %
%                                                                             %
%                                                                             %
%                 Read/Write Image Colormaps As An Image File                 %
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
#include "magick/magick.h"
#include "magick/utility.h"

/*
  Forward declarations.
*/
static unsigned int
  WriteMAPImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d M A P I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadMAPImage reads an image of raw RGB colormap and colormap index
%  bytes and returns it.  It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.
%
%  The format of the ReadMAPImage method is:
%
%      Image *ReadMAPImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadMAPImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadMAPImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image
    *image;

  IndexPacket
    index;

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

  unsigned char
    *colormap,
    *pixels;

  unsigned int
    packet_size,
    status;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  if ((image->columns == 0) || (image->rows == 0))
    ThrowReaderException(OptionError,MustSpecifyImageSize,image);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    ThrowReaderException(FileOpenError,UnableToOpenFile,image);
  /*
    Initialize image structure.
  */
  image->storage_class=PseudoClass;
  if (!AllocateImageColormap(image,image->offset ? image->offset : 256))
    ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
  packet_size=image->depth > 8 ? 2 : 1;
  pixels=MagickAllocateMemory(unsigned char *,packet_size*image->columns);
  packet_size=image->colors > 256 ? 6 : 3;
  colormap=MagickAllocateMemory(unsigned char *,packet_size*image->colors);
  if ((pixels == (unsigned char *) NULL) ||
      (colormap == (unsigned char *) NULL))
    ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
  /*
    Read image colormap.
  */
  (void) ReadBlob(image,packet_size*image->colors,(char *) colormap);
  p=colormap;
  if (image->colors <= 256)
    for (i=0; i < (long) image->colors; i++)
    {
      image->colormap[i].red=ScaleCharToQuantum(*p++);
      image->colormap[i].green=ScaleCharToQuantum(*p++);
      image->colormap[i].blue=ScaleCharToQuantum(*p++);
    }
  else
    for (i=0; i < (long) image->colors; i++)
    {
      image->colormap[i].red=(*p++ << 8U);
      image->colormap[i].red|=(*p++);
      image->colormap[i].green=(*p++ << 8U);
      image->colormap[i].green|=(*p++);
      image->colormap[i].blue=(*p++ << 8U);
      image->colormap[i].blue|=(*p++);
    }
  MagickFreeMemory(colormap);
  if (image_info->ping)
    {
      CloseBlob(image);
      return(image);
    }
  /*
    Read image pixels.
  */
  packet_size=image->depth > 8 ? 2 : 1;
  for (y=0; y < (long) image->rows; y++)
  {
    p=pixels;
    q=SetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=AccessMutableIndexes(image);
    (void) ReadBlob(image,packet_size*image->columns,(char *) pixels);
    for (x=0; x < (long) image->columns; x++)
    {
      index=(IndexPacket) (*p++);
      VerifyColormapIndex(image,index);
      if (image->colors > 256)
        {
          index=(IndexPacket) ((index << 8)+(*p++));
          VerifyColormapIndex(image,index);
        }
      indexes[x]=index;
      *q++=image->colormap[index];
    }
    if (!SyncImagePixels(image))
      break;
  }
  MagickFreeMemory(pixels);
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
%   R e g i s t e r M A P I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterMAPImage adds attributes for the MAP image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterMAPImage method is:
%
%      RegisterMAPImage(void)
%
*/
ModuleExport void RegisterMAPImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("MAP");
  entry->decoder=(DecoderHandler) ReadMAPImage;
  entry->encoder=(EncoderHandler) WriteMAPImage;
  entry->adjoin=False;
  entry->raw=True;
  entry->description="Colormap intensities and indices";
  entry->module="MAP";
  entry->coder_class=PrimaryCoderClass;
  entry->extension_treatment=ObeyExtensionTreatment;
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r M A P I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterMAPImage removes format registrations made by the
%  MAP module from the list of supported formats.
%
%  The format of the UnregisterMAPImage method is:
%
%      UnregisterMAPImage(void)
%
*/
ModuleExport void UnregisterMAPImage(void)
{
  (void) UnregisterMagickInfo("MAP");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e M A P I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteMAPImage writes an image to a file as red, green, and blue
%  colormap bytes followed by the colormap indexes.
%
%  The format of the WriteMAPImage method is:
%
%      unsigned int WriteMAPImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteMAPImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
static unsigned int WriteMAPImage(const ImageInfo *image_info,Image *image)
{
  long
    y;

  register const IndexPacket
    *indexes;

  register const PixelPacket
    *p;

  register long
    i,
    x;

  register unsigned char
    *q;

  unsigned char
    *colormap,
    *pixels;

  unsigned int
    status;

  unsigned long
    packet_size;

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
  (void) TransformColorspace(image,RGBColorspace);
  /*
    Allocate colormap.
  */
  if (!IsPaletteImage(image,&image->exception))
    (void) SetImageType(image,PaletteType);
  packet_size=image->depth > 8 ? 2 : 1;
  pixels=MagickAllocateMemory(unsigned char *,image->columns*packet_size);
  packet_size=image->colors > 256 ? 6 : 3;
  colormap=MagickAllocateMemory(unsigned char *,packet_size*image->colors);
  if ((pixels == (unsigned char *) NULL) ||
      (colormap == (unsigned char *) NULL))
    ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
  /*
    Write colormap to file.
  */
  q=colormap;
  if (image->colors <= 256)
    for (i=0; i < (long) image->colors; i++)
    {
      *q++=image->colormap[i].red;
      *q++=image->colormap[i].green;
      *q++=image->colormap[i].blue;
    }
#if QuantumDepth > 8
  else
    for (i=0; i < (long) image->colors; i++)
    {
      *q++=image->colormap[i].red >> 8;
      *q++=image->colormap[i].red & 0xff;
      *q++=image->colormap[i].green >> 8;
      *q++=image->colormap[i].green & 0xff;
      *q++=image->colormap[i].blue >> 8;
      *q++=image->colormap[i].blue & 0xff;
    }
#endif /* QuantumDepth > 8 */
  (void) WriteBlob(image,packet_size*image->colors,(char *) colormap);
  MagickFreeMemory(colormap);
  /*
    Write image pixels to file.
  */
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    indexes=AccessImmutableIndexes(image);
    q=pixels;
    for (x=0; x < (long) image->columns; x++)
    {
#if QuantumDepth > 8
      if (image->colors > 256)
        *q++=indexes[x] >> 8;
#endif /* QuantumDepth > 8 */
      *q++=indexes[x];
    }
    (void) WriteBlob(image,q-pixels,(char *) pixels);
  }
  MagickFreeMemory(pixels);
  CloseBlob(image);
  return(status);
}
