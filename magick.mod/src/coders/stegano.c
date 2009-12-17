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
%              SSSSS  TTTTT  EEEEE   GGGG   AAA   N   N   OOO                 %
%              SS       T    E      G      A   A  NN  N  O   O                %
%               SSS     T    EEE    G  GG  AAAAA  N N N  O   O                %
%                 SS    T    E      G   G  A   A  N  NN  O   O                %
%              SSSSS    T    EEEEE   GGG   A   A  N   N   OOO                 %
%                                                                             %
%                                                                             %
%                      Write A Steganographic Image.                          %
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
#include "magick/colormap.h"
#include "magick/constitute.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/pixel_cache.h"
#include "magick/utility.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d S T E G A N O I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadSTEGANOImage reads a steganographic image hidden within another
%  image type.  It allocates the memory necessary for the new Image structure
%  and returns a pointer to the new image.
%
%  The format of the ReadSTEGANOImage method is:
%
%      Image *ReadSTEGANOImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadSTEGANOImage returns a pointer to the image
%      after reading.  A null image is returned if there is a memory shortage
%      of if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadSTEGANOImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
#define GetBit(a,i) (((a) >> (i)) & 0x01)
#define SetBit(a,i,set) \
  a=(Quantum) ((set) ? (a) | (1UL << (i)) : (a) & ~(1UL << (i)))

  Image
    *image,
    *watermark;

  ImageInfo
    *clone_info;

  unsigned long
    y;

  long
    k;

  int
    c,
    i,
    j;

  PixelPacket
    pixel;

  register IndexPacket
    *indexes;

  register unsigned long
    x;

  register PixelPacket
    *q;

  /*
    Initialize Image structure.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  if ((image->columns == 0) || (image->rows == 0))
    ThrowReaderException(OptionError,MustSpecifyImageSize,image);
  clone_info=CloneImageInfo(image_info);
  clone_info->blob=(void *) NULL;
  clone_info->length=0;
  *clone_info->magick='\0';
  watermark=ReadImage(clone_info,exception);
  DestroyImageInfo(clone_info);
  if (watermark == (Image *) NULL)
    return((Image *) NULL);
  watermark->depth=QuantumDepth;
  if (!AllocateImageColormap(image,MaxColormapSize))
    ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
  if (image_info->ping)
    {
      CloseBlob(image);
      return(image);
    }
  /*
    Get hidden watermark from low-order bits of image.
  */
  c=0;
  i=0;
  j=0;
  k=image->offset;
  for (i=QuantumDepth-1; (i >= 0) && (j < QuantumDepth); i--)
  {
    for (y=0; (y < image->rows) && (j < QuantumDepth); y++)
    {
      for (x=0; (x < image->columns) && (j < QuantumDepth); x++)
      {
        (void) AcquireOnePixelByReference(watermark,&pixel,k % watermark->columns,
          k/watermark->columns,exception);
        q=GetImagePixels(image,x,y,1,1);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=AccessMutableIndexes(image);
        switch (c)
        {
          case 0:
          {
            SetBit(*indexes,i,GetBit(pixel.red,j));
            break;
          }
          case 1:
          {
            SetBit(*indexes,i,GetBit(pixel.green,j));
            break;
          }
          case 2:
          {
            SetBit(*indexes,i,GetBit(pixel.blue,j));
            break;
          }
        }
        (void) SyncImage(image);
        c++;
        if (c == 3)
          c=0;
        k++;
        if ((unsigned long) k == watermark->columns*watermark->columns)
          k=0;
        if (k == image->offset)
          j++;
      }
    }
    if (!MagickMonitorFormatted(i,QuantumDepth,&image->exception,
                                LoadImagesText,image->filename))
      break;
  }
  DestroyImage(watermark);
  (void) SyncImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r S T E G A N O I m a g e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterSTEGANOImage adds attributes for the STEGANO image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterSTEGANOImage method is:
%
%      RegisterSTEGANOImage(void)
%
*/
ModuleExport void RegisterSTEGANOImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("STEGANO");
  entry->decoder=(DecoderHandler) ReadSTEGANOImage;
  entry->description="Steganographic image";
  entry->module="STEGANO";
  entry->coder_class=PrimaryCoderClass;
  entry->extension_treatment=IgnoreExtensionTreatment;
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r S T E G A N O I m a g e                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterSTEGANOImage removes format registrations made by the
%  STEGANO module from the list of supported formats.
%
%  The format of the UnregisterSTEGANOImage method is:
%
%      UnregisterSTEGANOImage(void)
%
*/
ModuleExport void UnregisterSTEGANOImage(void)
{
  (void) UnregisterMagickInfo("STEGANO");
}
