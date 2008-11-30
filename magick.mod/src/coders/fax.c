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
%                            FFFFF   AAA   X   X                              %
%                            F      A   A   X X                               %
%                            FFF    AAAAA    X                                %
%                            F      A   A   X X                               %
%                            F      A   A  X   X                              %
%                                                                             %
%                                                                             %
%                   Read/Write Group 3 FIX Image Format.                      %
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
#include "magick/compress.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/utility.h"

/*
  Forward declarations.
*/
static unsigned int
  WriteFAXImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s F A X                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsFAX returns True if the image format type, identified by the
%  magick string, is FAX.
%
%  The format of the IsFAX method is:
%
%      unsigned int IsFAX(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsFAX returns True if the image format type is FAX.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsFAX(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(False);
  if (LocaleNCompare((char *) magick,"DFAX",4) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d F A X I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadFAXImage reads a Group 3 FAX image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadFAXImage method is:
%
%      Image *ReadFAXImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadFAXImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadFAXImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image
    *image;

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
  /*
    Initialize image structure.
  */
  image->storage_class=PseudoClass;
  if (image->columns == 0)
    image->columns=2592;
  if (image->rows == 0)
    image->rows=3508;
  image->depth=8;
  if (!AllocateImageColormap(image,2))
    ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
  /*
    Monochrome colormap.
  */
  image->colormap[0].red=MaxRGB;
  image->colormap[0].green=MaxRGB;
  image->colormap[0].blue=MaxRGB;
  image->colormap[1].red=0;
  image->colormap[1].green=0;
  image->colormap[1].blue=0;
  if (image_info->ping)
    {
      CloseBlob(image);
      return(image);
    }
  status=HuffmanDecodeImage(image);
  if (status == False)
    ThrowReaderException(CorruptImageError,UnableToReadImageData,image);
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
%   R e g i s t e r F A X I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterFAXImage adds attributes for the FAX image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterFAXImage method is:
%
%      RegisterFAXImage(void)
%
*/
ModuleExport void RegisterFAXImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("FAX");
  entry->decoder=(DecoderHandler) ReadFAXImage;
  entry->encoder=(EncoderHandler) WriteFAXImage;
  entry->magick=(MagickHandler) IsFAX;
  entry->description="Group 3 FAX (Not TIFF Group3 FAX!)";
  entry->module="FAX";
  entry->coder_class=PrimaryCoderClass;
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("G3");
  entry->decoder=(DecoderHandler) ReadFAXImage;
  entry->encoder=(EncoderHandler) WriteFAXImage;
  entry->magick=(MagickHandler) IsFAX;
  entry->adjoin=False;
  entry->description="Group 3 FAX (same as \"FAX\")";
  entry->stealth=MagickTrue;
  entry->module="FAX";
  entry->coder_class=PrimaryCoderClass;
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r F A X I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterFAXImage removes format registrations made by the
%  FAX module from the list of supported formats.
%
%  The format of the UnregisterFAXImage method is:
%
%      UnregisterFAXImage(void)
%
*/
ModuleExport void UnregisterFAXImage(void)
{
  (void) UnregisterMagickInfo("FAX");
  (void) UnregisterMagickInfo("G3");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e F A X I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure WriteFAXImage writes an image to a file in 1 dimensional Huffman
%  encoded format.
%
%  The format of the WriteFAXImage method is:
%
%      unsigned int WriteFAXImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteFAXImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
static unsigned int WriteFAXImage(const ImageInfo *image_info,Image *image)
{
  ImageInfo
    *clone_info;

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
  clone_info=CloneImageInfo(image_info);
  (void) strcpy(clone_info->magick,"FAX");
  scene=0;
  do
  {
    /*
      Convert MIFF to monochrome.
    */
    (void) TransformColorspace(image,RGBColorspace);
    status=HuffmanEncodeImage(clone_info,image);
    if (image->next == (Image *) NULL)
      break;
    image=SyncNextImageInList(image);
    status=MagickMonitorFormatted(scene++,GetImageListLength(image),
                                  &image->exception,SaveImagesText,
                                  image->filename);
    if (status == False)
      break;
  } while (clone_info->adjoin);
  DestroyImageInfo(clone_info);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseBlob(image);
  return(status);
}
