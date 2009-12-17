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
%                     M   M   AAA   TTTTT  TTTTT  EEEEE                       %
%                     MM MM  A   A    T      T    E                           %
%                     M M M  AAAAA    T      T    EEE                         %
%                     M   M  A   A    T      T    E                           %
%                     M   M  A   A    T      T    EEEEE                       %
%                                                                             %
%                                                                             %
%                     Write Matte Channel To MIFF File.                       %
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
#include "magick/constitute.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/utility.h"

/*
  Forward declarations.
*/
static unsigned int
  WriteMATTEImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r M A T T E I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterMATTEImage adds attributes for the MATTE image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterMATTEImage method is:
%
%      RegisterMATTEImage(void)
%
*/
ModuleExport void RegisterMATTEImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("MATTE");
  entry->encoder=(EncoderHandler) WriteMATTEImage;
  entry->raw=True;
  entry->description="MATTE raw opacity format";
  entry->module="MATTE";
  entry->extension_treatment=ObeyExtensionTreatment;
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r M A T T E I m a g e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterMATTEImage removes format registrations made by the
%  MATTE module from the list of supported formats.
%
%  The format of the UnregisterMATTEImage method is:
%
%      UnregisterMATTEImage(void)
%
*/
ModuleExport void UnregisterMATTEImage(void)
{
  (void) UnregisterMagickInfo("MATTE");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e M A T T E I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function WriteMATTEImage writes an image of matte bytes to a file.  It
%  consists of data from the matte component of the image [0..255].
%
%  The format of the WriteMATTEImage method is:
%
%      unsigned int WriteMATTEImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Function WriteMATTEImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
static unsigned int WriteMATTEImage(const ImageInfo *image_info,Image *image)
{
  Image
    *matte_image;

  long
    y;

  register const PixelPacket
    *p;

  register long
    x;

  register PixelPacket
    *q;

  unsigned int
    status;

  if (!image->matte)
    ThrowWriterException(CoderError,ImageDoesNotHaveAMatteChannel,image);
  matte_image=
    CloneImage(image,image->columns,image->rows,True,&image->exception);
  if (matte_image == (Image *) NULL)
    return(False);
  (void) (void) SetImageType(matte_image,TrueColorType);
  /*
    Convert image to matte pixels.
  */
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    q=SetImagePixels(matte_image,0,y,matte_image->columns,1);
    if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      q->red=p->opacity;
      q->green=p->opacity;
      q->blue=p->opacity;
      q->opacity=OpaqueOpacity;
      p++;
      q++;
    }
    if (!SyncImagePixels(matte_image))
      break;
    if (image->previous == (Image *) NULL)
      if (QuantumTick(y,image->rows))
        if (!MagickMonitorFormatted(y,image->rows,&image->exception,
                                    SaveImageText,image->filename,
				    image->columns,image->rows))
          break;
  }
  (void) FormatString(matte_image->filename,"MIFF:%.1024s",image->filename);
  status=WriteImage(image_info,matte_image);
  DestroyImage(matte_image);
  return(status);
}
