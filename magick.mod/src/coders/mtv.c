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
%                            M   M  TTTTT  V   V                              %
%                            MM MM    T    V   V                              %
%                            M M M    T    V   V                              %
%                            M   M    T     V V                               %
%                            M   M    T      V                                %
%                                                                             %
%                                                                             %
%                   Read/Write MTV Raytracer Image Format.                    %
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
  Forward declarations.
*/
static unsigned int
  WriteMTVImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d M T V I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadMTVImage reads a MTV image file and returns it.  It allocates
%  the memory necessary for the new Image structure and returns a pointer to
%  the new image.
%
%  The format of the ReadMTVImage method is:
%
%      Image *ReadMTVImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadMTVImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadMTVImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    buffer[MaxTextExtent];

  Image
    *image;

  long
    count,
    y;

  register long
    x;

  register PixelPacket
    *q;

  register unsigned char
    *p;

  unsigned char
    *pixels;

  unsigned int
    status;

  unsigned long
    columns,
    rows;

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
    Read MTV image.
  */
  (void) ReadBlobString(image,buffer);
  columns=0;
  rows=0;
  count=sscanf(buffer,"%lu %lu\n",&columns,&rows);
  if (count != 2)
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
  do
  {
    size_t
      row_size;
    
    /*
      Initialize image structure.
    */
    image->columns=columns;
    image->rows=rows;
    image->depth=8;
    if (image_info->ping && (image_info->subrange != 0))
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    /*
      Convert MTV raster image to pixel packets.
    */
    pixels=MagickAllocateArray(unsigned char *,image->columns,3);
    if (pixels == (unsigned char *) NULL)
      ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
    row_size=image->columns*3;
    for (y=0; y < (long) image->rows; y++)
    {
      if (ReadBlob(image,row_size,pixels) != row_size)
        ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
      p=pixels;
      q=SetImagePixels(image,0,y,image->columns,1);
      if (q == (PixelPacket *) NULL)
        break;
      for (x=0; x < (long) image->columns; x++)
      {
        q->red=ScaleCharToQuantum(*p++);
        q->green=ScaleCharToQuantum(*p++);
        q->blue=ScaleCharToQuantum(*p++);
        q++;
      }
      if (!SyncImagePixels(image))
        break;
      if (image->previous == (Image *) NULL)
        if (QuantumTick(y,image->rows))
          if (!MagickMonitorFormatted(y,image->rows,exception,LoadImageText,
                                      image->filename))
            break;
    }
    MagickFreeMemory(pixels);
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
    *buffer='\0';
    (void) ReadBlobString(image,buffer);
    count=sscanf(buffer,"%lu %lu\n",&columns,&rows);
    if (count == 2)
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
  } while (count == 2);
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
%   R e g i s t e r M T V I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterMTVImage adds attributes for the MTV image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterMTVImage method is:
%
%      RegisterMTVImage(void)
%
*/
ModuleExport void RegisterMTVImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("MTV");
  entry->decoder=(DecoderHandler) ReadMTVImage;
  entry->encoder=(EncoderHandler) WriteMTVImage;
  entry->description="MTV Raytracing image format";
  entry->module="MTV";
  entry->coder_class=UnstableCoderClass;
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r M T V I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterMTVImage removes format registrations made by the
%  MTV module from the list of supported formats.
%
%  The format of the UnregisterMTVImage method is:
%
%      UnregisterMTVImage(void)
%
*/
ModuleExport void UnregisterMTVImage(void)
{
  (void) UnregisterMagickInfo("MTV");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e M T V I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteMTVImage writes an image to a file in red, green, and blue
%  MTV rasterfile format.
%
%  The format of the WriteMTVImage method is:
%
%      unsigned int WriteMTVImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteMTVImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
static unsigned int WriteMTVImage(const ImageInfo *image_info,Image *image)
{
  char
    buffer[MaxTextExtent];

  int
    y;

  register const PixelPacket
    *p;

  register long
    x;

  register unsigned char
    *q;

  unsigned char
    *pixels;

  unsigned int
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
    /*
      Allocate memory for pixels.
    */
    (void) TransformColorspace(image,RGBColorspace);
    pixels=MagickAllocateMemory(unsigned char *,
      image->columns*sizeof(PixelPacket));
    if (pixels == (unsigned char *) NULL)
      ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
    /*
      Initialize raster file header.
    */
    FormatString(buffer,"%lu %lu\n",image->columns,image->rows);
    (void) WriteBlobString(image,buffer);
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
      (void) WriteBlob(image,q-pixels,(char *) pixels);
      if (image->previous == (Image *) NULL)
        if (QuantumTick(y,image->rows))
          if (!MagickMonitorFormatted(y,image->rows,&image->exception,
                                      SaveImageText,image->filename))
            break;
    }
    MagickFreeMemory(pixels);
    if (image->next == (Image *) NULL)
      break;
    image=SyncNextImageInList(image);
    status=MagickMonitorFormatted(scene++,GetImageListLength(image),
                                  &image->exception,SaveImagesText,
                                  image->filename);
    if (status == False)
      break;
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseBlob(image);
  return(True);
}
