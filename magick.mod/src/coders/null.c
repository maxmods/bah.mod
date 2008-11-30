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
%                        N   N  U   U  L      L                               %
%                        NN  N  U   U  L      L                               %
%                        N N N  U   U  L      L                               %
%                        N  NN  U   U  L      L                               %
%                        N   N   UUU   LLLLL  LLLLL                           %
%                                                                             %
%                                                                             %
%                   Read/Write Image Of Uniform Color                         %
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
#include "magick/magick.h"
#include "magick/utility.h"

/*
  Forward declarations.
*/
static unsigned int
  WriteNULLImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d N U L L I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadNULLImage creates a constant image and initializes it to the
%  X server color as specified by the filename.  It allocates the memory
%  necessary for the new Image structure and returns a pointer to the new
%  image.
%
%  The format of the ReadNULLImage method is:
%
%      Image *ReadNULLImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadNULLImage returns a pointer to the image after
%      creating it. A null image is returned if there is a memory shortage
%      or if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadNULLImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *image;

  unsigned int
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
  if (status == False)
    {
      DestroyImage(image);
      return((Image *) NULL);
    }
  if (!AllocateImageColormap(image,1))
    ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
  image->colormap[0]=image->background_color;
  status=SetImage(image,OpaqueOpacity);
  if (status == MagickFail)
    {
      CopyException(exception,&image->exception);
      DestroyImage(image);
      image=(Image *) NULL;
    }

  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r N U L L I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterNULLImage adds attributes for the NULL image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterNULLImage method is:
%
%      RegisterNULLImage(void)
%
*/
ModuleExport void RegisterNULLImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("NULL");
  entry->decoder=(DecoderHandler) ReadNULLImage;
  entry->encoder=(EncoderHandler) WriteNULLImage;
  entry->adjoin=False;
  entry->description="Constant image of uniform color";
  entry->module="NULL";
  entry->coder_class=PrimaryCoderClass;
  entry->extension_treatment=IgnoreExtensionTreatment;
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r N U L L I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterNULLImage removes format registrations made by the
%  NULL module from the list of supported formats.
%
%  The format of the UnregisterNULLImage method is:
%
%      UnregisterNULLImage(void)
%
*/
ModuleExport void UnregisterNULLImage(void)
{
  (void) UnregisterMagickInfo("NULL");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e N U L L I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteNULLImage writes no output at all. It is useful when specified
%  as an output format when profiling.
%
%  The format of the WriteNULLImage method is:
%
%      unsigned int WriteNULLImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Always returns True.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
static unsigned int WriteNULLImage(const ImageInfo *image_info,Image *image)
{
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  return(True);
}
