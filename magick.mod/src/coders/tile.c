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
%                        TTTTT  IIIII  L      EEEEE                           %
%                          T      I    L      E                               %
%                          T      I    L      EEE                             %
%                          T      I    L      E                               %
%                          T    IIIII  LLLLL  EEEEE                           %
%                                                                             %
%                                                                             %
%                    Return A Tiled Image Using Texture.                      %
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
#include "magick/composite.h"
#include "magick/constitute.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/utility.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d T I L E I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadTILEImage tiles a texture on an image.  It allocates the
%  memory necessary for the new Image structure and returns a pointer to the
%  new image.
%
%  The format of the ReadTILEImage method is:
%
%      Image *ReadTILEImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadTILEImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadTILEImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *image,
    *tile_image;

  ImageInfo
    *clone_info;

  RectangleInfo
    geometry;
    

  /*
    Initialize Image structure.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);

  clone_info=CloneImageInfo(image_info);
  clone_info->blob=(void *) NULL;
  clone_info->length=0;
  *clone_info->magick='\0';
  tile_image=ReadImage(clone_info,exception);
  DestroyImageInfo(clone_info);
  if (tile_image == (Image *) NULL)
    return((Image *) NULL);

  /*
    Adapt tile image to desired image type.
  */
  if (image_info->type != UndefinedType)
    (void) SetImageType(tile_image,image_info->type);

  /*
    Create tiled canvas image.
  */
  (void) GetGeometry(image_info->size,&geometry.x,&geometry.y,&geometry.width,
                     &geometry.height);
  image=ConstituteTextureImage(geometry.width,geometry.height,tile_image,exception);

  DestroyImage(tile_image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r T I L E I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterTILEImage adds attributes for the TILE image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterTILEImage method is:
%
%      RegisterTILEImage(void)
%
*/
ModuleExport void RegisterTILEImage(void)
{
  MagickInfo
    *entry;

  static const char
    *TILENote=
    {
      "Use the syntax \"-size WIDTHxHEIGHT TILE:imagename\" to tile the\n"
      "specified tile image over a canvas image of size WIDTHxHEIGHT."
    };

  entry=SetMagickInfo("TILE");
  entry->decoder=(DecoderHandler) ReadTILEImage;
  entry->raw=True;
  entry->description="Tile image with a texture";
  entry->note=TILENote;
  entry->module="TILE";
  entry->coder_class=PrimaryCoderClass;
  entry->extension_treatment=IgnoreExtensionTreatment;
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r T I L E I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterTILEImage removes format registrations made by the
%  TILE module from the list of supported formats.
%
%  The format of the UnregisterTILEImage method is:
%
%      UnregisterTILEImage(void)
%
*/
ModuleExport void UnregisterTILEImage(void)
{
  (void) UnregisterMagickInfo("TILE");
}
