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
%                               X   X   CCCC                                  %
%                                X X   C                                      %
%                                 X    C                                      %
%                                X X   C                                      %
%                               X   X   CCCC                                  %
%                                                                             %
%                                                                             %
%                        Read Constant Color Image.                           %
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
#include "magick/color.h"
#include "magick/composite.h"
#include "magick/magick.h"
#include "magick/utility.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d X C I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadXCImage creates a constant image and initializes it to the
%  X server color as specified by the filename.  It allocates the memory
%  necessary for the new Image structure and returns a pointer to the new
%  image.
%
%  The format of the ReadXCImage method is:
%
%      Image *ReadXCImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadXCImage returns a pointer to the image after
%      creating it. A null image is returned if there is a memory shortage
%      or if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadXCImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image
    *image;

  MagickPassFail
    status;

  /*
    Initialize Image structure.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  if (image->columns == 0)
    image->columns=1;
  if (image->rows == 0)
    image->rows=1;
  (void) strlcpy(image->filename,image_info->filename,MaxTextExtent);
  status=QueryColorDatabase((char *) image_info->filename,
    &image->background_color,exception);
  if (status == MagickFail)
    {
      DestroyImage(image);
      return ((Image *) NULL);
    }
  /*
    Create a colormap if image is not DirectClass type.
  */
  if ((TrueColorType != image_info->type) &&
      (TrueColorMatteType != image_info->type))
    {
      if (!AllocateImageColormap(image,1))
        ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
      image->colormap[0]=image->background_color;
    }
  /*
    Initialize image pixels to the value of image->background_color
  */
  status=SetImage(image,image->background_color.opacity);
  if (status == MagickFail)
    {
      CopyException(exception,&image->exception);
      DestroyImage(image);
      image=(Image *) NULL;
    }

  return image;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r X C I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterXCImage adds attributes for the XC image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterXCImage method is:
%
%      RegisterXCImage(void)
%
*/
ModuleExport void RegisterXCImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("XC");
  entry->decoder=(DecoderHandler) ReadXCImage;
  entry->adjoin=False;
  entry->raw=True;
  entry->description="Constant image uniform color";
  entry->module="XC";
  entry->coder_class=PrimaryCoderClass;
  entry->extension_treatment=IgnoreExtensionTreatment;
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r X C I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterXCImage removes format registrations made by the
%  XC module from the list of supported formats.
%
%  The format of the UnregisterXCImage method is:
%
%      UnregisterXCImage(void)
%
*/
ModuleExport void UnregisterXCImage(void)
{
  (void) UnregisterMagickInfo("XC");
}
