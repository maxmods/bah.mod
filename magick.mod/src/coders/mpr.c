/*
% Copyright (C) 2003-2010 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            M   M  PPPP   RRRR                               %
%                            MM MM  P   P  R   R                              %
%                            M M M  PPPP   RRRR                               %
%                            M   M  P      R R                                %
%                            M   M  P      R  R                               %
%                                                                             %
%                                                                             %
%                  Read/Write the Magick Persistent Registry.                 %
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
#include "magick/magick.h"
#include "magick/registry.h"
#include "magick/utility.h"

/*
  Forward declarations.
*/
static unsigned int
  WriteMPRImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d M P R I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadMPRImage reads a Magick Persistent Registry image as a blob from
%  memory.  It allocates the memory necessary for the new Image structure and
%  returns a pointer to the new image.
%
%  The format of the ReadMPRImage method is:
%
%      Image *ReadMPRImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadMPRImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadMPRImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  char
    *p;

  long
    id;

  RegistryType
    type;

  size_t
    length;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if (LocaleCompare(image_info->magick,"MPRI") != 0)
    return(GetImageFromMagickRegistry(image_info->filename,&id,exception));
  id=strtol(image_info->filename,&p,0);
  return((Image *) GetMagickRegistry(id,&type,&length,exception));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r M P R I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterMPRImage adds attributes for the MPR image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterMPRImage method is:
%
%      RegisterMPRImage(void)
%
*/
ModuleExport void RegisterMPRImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("MPR");
  entry->decoder=(DecoderHandler) ReadMPRImage;
  entry->encoder=(EncoderHandler) WriteMPRImage;
  entry->adjoin=False;
  entry->stealth=True;
  entry->description="Magick Persistent Registry";
  entry->module="MPR";
  entry->coder_class=PrimaryCoderClass;
  entry->extension_treatment=IgnoreExtensionTreatment;
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("MPRI");
  entry->decoder=(DecoderHandler) ReadMPRImage;
  entry->encoder=(EncoderHandler) WriteMPRImage;
  entry->stealth=True;
  entry->adjoin=False;
  entry->stealth=True;
  entry->description="Magick Persistent Registry";
  entry->module="MPRI";
  entry->coder_class=PrimaryCoderClass;
  entry->extension_treatment=IgnoreExtensionTreatment;
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r M P R I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterMPRImage removes format registrations made by the
%  MPR module from the list of supported formats.
%
%  The format of the UnregisterMPRImage method is:
%
%      UnregisterMPRImage(void)
%
*/
ModuleExport void UnregisterMPRImage(void)
{
  (void) UnregisterMagickInfo("MPR");
  (void) UnregisterMagickInfo("MPRI");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e M P R I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  WriteMPRImage() writes an image into the Magick Persistent Registry
%  image as a blob from memory.  It allocates the memory necessary for the
%  new Image structure and returns a pointer to the new image.
%
%  The format of the WriteMPRImage method is:
%
%      unsigned int WriteMPRImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WritePRImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
static unsigned int WriteMPRImage(const ImageInfo *image_info,Image *image)
{
  ExceptionInfo
    exception;

  Image
    *registry_image;

  long
    id;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  GetExceptionInfo(&exception);
  registry_image=GetImageFromMagickRegistry(image->filename,&id,&exception);
  DestroyExceptionInfo(&exception);
  if (registry_image != (Image *) NULL)
    {
      (void) DeleteMagickRegistry(id);
      DestroyImage(registry_image);
    }
  id=SetMagickRegistry(ImageRegistryType,image,sizeof(Image),&image->exception);
  return(id >= 0);
}
