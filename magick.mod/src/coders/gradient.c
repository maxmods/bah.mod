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
%            GGGG  RRRR    AAA   DDDD   IIIII  EEEEE  N   N  TTTTT            %
%           G      R   R  A   A  D   D    I    E      NN  N    T              %
%           G  GG  RRRR   AAAAA  D   D    I    EEE    N N N    T              %
%           G   G  R R    A   A  D   D    I    E      N  NN    T              %
%            GGG   R  R   A   A  DDDD   IIIII  EEEEE  N   N    T              %
%                                                                             %
%                                                                             %
%                 Read An Image Filled Using Gradient.                        %
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
#include "magick/color_lookup.h"
#include "magick/gradient.h"
#include "magick/magick.h"
#include "magick/utility.h"
#include "magick/studio.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d G R A D I E N T I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadGRADIENTImage creates a gradient image and initializes it to
%  the color range as specified by the filename.  It allocates the memory
%  necessary for the new Image structure and returns a pointer to the new
%  image.
%
%  The format of the ReadGRADIENTImage method is:
%
%      Image *ReadGRADIENTImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadGRADIENTImage returns a pointer to the image after
%      creating it. A null image is returned if there is a memory shortage
%      or if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadGRADIENTImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  char
    colorname[MaxTextExtent];

  PixelPacket
    start_color,
    stop_color;

  Image
    *image;

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
  (void) SetImage(image,OpaqueOpacity);
  (void) strlcpy(image->filename,image_info->filename,MaxTextExtent);
  (void) strlcpy(colorname,image_info->filename,MaxTextExtent);
  (void) sscanf(image_info->filename,"%[^-]",colorname);
  if (!QueryColorDatabase(colorname,&start_color,exception))
    {
      DestroyImage(image);
      return((Image *) NULL);
    }
  (void) strcpy(colorname,"white");
  if (PixelIntensityToQuantum(&start_color) > (0.5*MaxRGB))
    (void) strcpy(colorname,"black");
  (void) sscanf(image_info->filename,"%*[^-]-%s",colorname);
  if (!QueryColorDatabase(colorname,&stop_color,exception))
    {
      DestroyImage(image);
      return((Image *) NULL);
    }
  (void) GradientImage(image,&start_color,&stop_color);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r G R A D I E N T I m a g e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterGRADIENTImage adds attributes for the GRADIENT image format
%  to the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterGRADIENTImage method is:
%
%      RegisterGRADIENTImage(void)
%
*/
ModuleExport void RegisterGRADIENTImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("GRADIENT");
  entry->decoder=(DecoderHandler) ReadGRADIENTImage;
  entry->adjoin=False;
  entry->raw=True;
  entry->description="Gradual passing from one shade to another";
  entry->module="GRADIENT";
  entry->coder_class=PrimaryCoderClass;
  entry->extension_treatment=IgnoreExtensionTreatment;
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r G R A D I E N T I m a g e                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterGRADIENTImage removes format registrations made by the
%  GRADIENT module from the list of supported formats.
%
%  The format of the UnregisterGRADIENTImage method is:
%
%      UnregisterGRADIENTImage(void)
%
*/
ModuleExport void UnregisterGRADIENTImage(void)
{
  (void) UnregisterMagickInfo("GRADIENT");
}
