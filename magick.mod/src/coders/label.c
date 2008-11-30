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
%                     L       AAA   BBBB   EEEEE  L                           %
%                     L      A   A  B   B  E      L                           %
%                     L      AAAAA  BBBB   EEE    L                           %
%                     L      A   A  B   B  E      L                           %
%                     LLLLL  A   A  BBBB   EEEEE  LLLLL                       %
%                                                                             %
%                                                                             %
%                      Read ASCII String As An Image                          %
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
#include "magick/render.h"
#include "magick/utility.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d L A B E L I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadLABELImage reads a LABEL image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadLABELImage method is:
%
%      Image *ReadLABELImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadLABELImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadLABELImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  char
    geometry[MaxTextExtent];

  DrawInfo
    *draw_info;

  Image
    *image;

  TypeMetric
    metrics;

  unsigned int
    status;

  unsigned long
    height,
    width;

  /*
    Initialize Image structure.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  draw_info=CloneDrawInfo(image_info,(DrawInfo *) NULL);
  draw_info->fill=image_info->pen;
  draw_info->text=TranslateText(image_info,image,image_info->filename);
  if (draw_info->text == (char *) NULL)
    ThrowReaderException(CoderError,UnableToTranslateText,image);
  if ((image->columns != 0) || (image->rows != 0))
    {
      /*
        Fit label to canvas size.
      */
      for ( ; GetTypeMetrics(image,draw_info,&metrics); draw_info->pointsize*=2)
      {
        width=(unsigned long) floor(metrics.width+metrics.max_advance/2+0.5);
        height=(unsigned long) floor(metrics.ascent-metrics.descent+0.5);
        if (((image->columns != 0) && (width >= image->columns)) ||
            ((image->rows != 0) && (height >= image->rows)))
          break;
      }
      for ( ; GetTypeMetrics(image,draw_info,&metrics); draw_info->pointsize--)
      {
        width=(unsigned long) floor(metrics.width+metrics.max_advance/2+0.5);
        height=(unsigned long) floor(metrics.ascent-metrics.descent+0.5);
        if ((image->columns != 0) && (width <= image->columns) &&
           ((image->rows == 0) || (height <= image->rows)))
          break;
        if ((image->rows != 0) && (height <= image->rows) &&
           ((image->columns == 0) || (width <= image->columns)))
          break;
        if (draw_info->pointsize < 2.0)
          break;
      }
    }
  status=GetTypeMetrics(image,draw_info,&metrics);
  if (status == False)
    ThrowReaderException(TypeError,UnableToGetTypeMetrics,image);
  FormatString(geometry,"+%g+%g",metrics.max_advance/4,metrics.ascent);
  if (image->columns == 0)
    image->columns=(unsigned long)
      floor(metrics.width+metrics.max_advance/2+0.5);
  if (image->rows == 0)
    {
      image->rows=(unsigned long) floor(metrics.ascent-metrics.descent+0.5);
      FormatString(geometry,"+%g+%g",image->columns/2.0-metrics.width/2.0,
        metrics.ascent);
    }
  draw_info->geometry=AllocateString(geometry);
  (void) SetImage(image,OpaqueOpacity);
  (void) AnnotateImage(image,draw_info);
  DestroyDrawInfo(draw_info);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r L A B E L I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterLABELImage adds attributes for the LABEL image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterLABELImage method is:
%
%      RegisterLABELImage(void)
%
*/
ModuleExport void RegisterLABELImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("LABEL");
  entry->decoder=(DecoderHandler) ReadLABELImage;
  entry->adjoin=False;
  entry->description="Image label";
  entry->module="LABEL";
  entry->coder_class=PrimaryCoderClass;
  entry->extension_treatment=IgnoreExtensionTreatment;
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r L A B E L I m a g e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterLABELImage removes format registrations made by the
%  LABEL module from the list of supported formats.
%
%  The format of the UnregisterLABELImage method is:
%
%      UnregisterLABELImage(void)
%
*/
ModuleExport void UnregisterLABELImage(void)
{
  (void) UnregisterMagickInfo("LABEL");
}
