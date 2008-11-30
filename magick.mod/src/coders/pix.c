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
%                            PPPP   IIIII  X   X                              %
%                            P   P    I     X X                               %
%                            PPPP     I      X                                %
%                            P        I     X X                               %
%                            P      IIIII  X   X                              %
%                                                                             %
%                                                                             %
%                   Read Alias/Wavefront RLE Image Format.                    %
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
%   R e a d P I X I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPIXImage reads a Alias/Wavefront RLE image file and returns it.
%  It allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadPIXImage method is:
%
%      Image *ReadPIXImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadPIXImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadPIXImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image
    *image;

  IndexPacket
    index;

  long
    y;

  Quantum
    blue,
    green,
    red;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *q;

  unsigned int
    status;

  unsigned long
    bits_per_pixel,
    height,
    length,
    width;

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
    Read PIX image.
  */
  width=ReadBlobMSBShort(image);
  height=ReadBlobMSBShort(image);
  (void) ReadBlobMSBShort(image);  /* x-offset */
  (void) ReadBlobMSBShort(image);  /* y-offset */
  bits_per_pixel=ReadBlobMSBShort(image);
  if ((EOFBlob(image)) ||
      (width == 0) || (height == 0) ||
       ((bits_per_pixel != 8) && (bits_per_pixel != 24)))
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
  do
  {
    /*
      Initialize image structure.
    */
    image->columns= width;
    image->rows= height;
    if (bits_per_pixel == 8)
      if (!AllocateImageColormap(image,256))
        ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
    if (image_info->ping && (image_info->subrange != 0))
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    /*
      Convert PIX raster image to pixel packets.
    */
    red=0;
    green=0;
    blue=0;
    index=0;
    length=0;
    for (y=0; y < (long) image->rows; y++)
    {
      q=SetImagePixels(image,0,y,image->columns,1);
      if (q == (PixelPacket *) NULL)
        break;
      indexes=AccessMutableIndexes(image);
      for (x=0; x < (long) image->columns; x++)
      {
        if (length == 0)
          {
            length=ReadBlobByte(image);
            if (bits_per_pixel == 8)
              index=ScaleCharToQuantum(ReadBlobByte(image));
            else
              {
                blue=ScaleCharToQuantum(ReadBlobByte(image));
                green=ScaleCharToQuantum(ReadBlobByte(image));
                red=ScaleCharToQuantum(ReadBlobByte(image));
              }
          }
        if (image->storage_class == PseudoClass)
          indexes[x]=index;
        q->blue=blue;
        q->green=green;
        q->red=red;
        length--;
        q++;
      }
      if (!SyncImagePixels(image))
        break;
      if (image->previous == (Image *) NULL)
        if (!MagickMonitorFormatted(y,image->rows,exception,LoadImageText,
                                    image->filename))
          break;
    }
    if (image->storage_class == PseudoClass)
      (void) SyncImage(image);
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
    width=ReadBlobMSBLong(image);
    height=ReadBlobMSBLong(image);
    (void) ReadBlobMSBShort(image);
    (void) ReadBlobMSBShort(image);
    bits_per_pixel=ReadBlobMSBShort(image);
    status=(!EOFBlob(image)) && (width != 0U) && (height != 0U) &&
      ((bits_per_pixel == 8) || (bits_per_pixel == 24));
    if (status == True)
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
%   R e g i s t e r P I X I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterPIXImage adds attributes for the PIX image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterPIXImage method is:
%
%      RegisterPIXImage(void)
%
*/
ModuleExport void RegisterPIXImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("PIX");
  entry->decoder=(DecoderHandler) ReadPIXImage;
  entry->description="Alias/Wavefront RLE image format";
  entry->module="PIX";
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r P I X I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterPIXImage removes format registrations made by the
%  PIX module from the list of supported formats.
%
%  The format of the UnregisterPIXImage method is:
%
%      UnregisterPIXImage(void)
%
*/
ModuleExport void UnregisterPIXImage(void)
{
  (void) UnregisterMagickInfo("PIX");
}
