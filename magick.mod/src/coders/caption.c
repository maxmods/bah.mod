/*
% Copyright (C) 2003 GraphicsMagick Group
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
%               CCCC   AAA   PPPP   TTTTT  IIIII   OOO   N   N                %
%              C      A   A  P   P    T      I    O   O  NN  N                %
%              C      AAAAA  PPPP     T      I    O   O  N N N                %
%              C      A   A  P        T      I    O   O  N  NN                %
%               CCCC  A   A  P        T    IIIII   OOO   N   N                %
%                                                                             %
%                                                                             %
%                             Read Text Caption.                              %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                               February 2002                                 %
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
%   R e a d C A P T I O N I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadCAPTIONImage reads a CAPTION image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadCAPTIONImage method is:
%
%      Image *ReadCAPTIONImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadCAPTIONImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadCAPTIONImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  char
    *caption,
    geometry[MaxTextExtent];

  DrawInfo
    *draw_info;

  Image
    *image;

  register char
    *p,
    *q;

  register long
    i;

  TypeMetric
    metrics;

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
    ThrowReaderException(OptionError,MustSpecifyImageSize,image);
  if (*image_info->filename != '@')
    caption=AllocateString(image_info->filename);
  else
    {
      unsigned long
        length;

      /*
        Read caption from file.
      */
      (void) strlcpy(image->filename,image_info->filename+1,MaxTextExtent);
      status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
      if (status == False)
        ThrowReaderException(FileOpenError,UnableToOpenFile,image);
      length=MaxTextExtent;
      caption=MagickAllocateMemory(char *,length);
      p=caption;
      if (caption != (char *) NULL)
        while (ReadBlobString(image,p) != (char *) NULL)
        {
          p+=strlen(p);
          if ((p-caption+MaxTextExtent+1) < (long) length)
            continue;
          length<<=1;
          MagickReallocMemory(char *,caption,length);
          if (caption == (char *) NULL)
            break;
          p=caption+strlen(caption);
        }
      if (caption == (char *) NULL)
        ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
      CloseBlob(image);
    }
  /*
    Format caption.
  */
  draw_info=CloneDrawInfo(image_info,(DrawInfo *) NULL);
  draw_info->fill=image_info->pen;
  draw_info->text=AllocateString(caption);
  p=caption;
  q=draw_info->text;
  for (i=0; *p != '\0'; p++)
  {
    *q++=(*p);
    *q='\0';
    status=GetTypeMetrics(image,draw_info,&metrics);
    if (status == False)
      ThrowReaderException(TypeError,UnableToGetTypeMetrics,image);
    if ((metrics.width+metrics.max_advance/2) < image->columns)
      continue;
    for (p--; !isspace((int) *p) && (p > caption); p--);
    *p++='\n';
    q=draw_info->text;
    i++;
  }
  if (image->rows == 0)
    image->rows=(unsigned long) ((i+1)*(metrics.ascent-metrics.descent));
  (void) SetImage(image,OpaqueOpacity);
  /*
    Draw caption.
  */
  (void) CloneString(&draw_info->text,caption);
  FormatString(geometry,"+%g+%g",metrics.max_advance/4,metrics.ascent);
  draw_info->geometry=AllocateString(geometry);
  (void) AnnotateImage(image,draw_info);
  DestroyDrawInfo(draw_info);
  MagickFreeMemory(caption);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r C A P T I O N I m a g e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterCAPTIONImage adds attributes for the CAPTION image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterCAPTIONImage method is:
%
%      RegisterCAPTIONImage(void)
%
*/
ModuleExport void RegisterCAPTIONImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("CAPTION");
  entry->decoder=(DecoderHandler) ReadCAPTIONImage;
  entry->adjoin=False;
  entry->description="Image caption";
  entry->module="CAPTION";
  entry->coder_class=PrimaryCoderClass;
  entry->extension_treatment=IgnoreExtensionTreatment;
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r C A P T I O N I m a g e                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterCAPTIONImage removes format registrations made by the
%  CAPTION module from the list of supported formats.
%
%  The format of the UnregisterCAPTIONImage method is:
%
%      UnregisterCAPTIONImage(void)
%
*/
ModuleExport void UnregisterCAPTIONImage(void)
{
  (void) UnregisterMagickInfo("CAPTION");
}
