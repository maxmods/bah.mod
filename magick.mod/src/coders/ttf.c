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
%                            TTTTT  TTTTT  FFFFF                              %
%                              T      T    F                                  %
%                              T      T    FFF                                %
%                              T      T    F                                  %
%                              T      T    F                                  %
%                                                                             %
%                                                                             %
%             Return A Preview For A TrueType or Postscript Font              %
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
#include "magick/draw.h"
#include "magick/magick.h"
#include "magick/render.h"
#include "magick/utility.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s T T F                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsTTF returns True if the image format type, identified by the
%  magick string can be handled by this module.
%
%  The format of the IsTTF method is:
%
%      unsigned int IsTTF(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsTTF returns True if the image format type can be
%               handled by this module.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/

static unsigned int IsPFA(const unsigned char *magick,const size_t length)
{
  if (length < 14)
    return(False);
  if (LocaleNCompare((char *) magick,"%!PS-AdobeFont-1.0",14) == 0)
    return(True);
  return(False);
}

static unsigned int IsTTF(const unsigned char *magick,const size_t length)
{
  if (length < 5)
    return(False);
  if ((magick[0] == 0x00U) && (magick[1] == 0x01U) && (magick[2] == 0x00U) &&
      (magick[3] == 0x00U) && (magick[4] == 0x0U))
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d T T F I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadTTFImage reads a TrueType font file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadTTFImage method is:
%
%      Image *ReadTTFImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadTTFImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
#if defined(HasTTF)
static Image *ReadTTFImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    buffer[MaxTextExtent];

  const char
    *Text =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz\n0123456789 "
      "\342\352\356\373\364\344\353\357\366\374\377\340\371\351\350\347\n"
      "&#~\\\"\'(-`_^@)=+\260 $\243^\250*\265\371%!\247:/;.,?<>";

  DrawContext
    context;

  DrawInfo
    *draw_info;

  Image
    *image;

  long
    y;

  PixelPacket
    background_color;

  register long
    i,
    x;

  register PixelPacket
    *q;

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
  image->columns=800;
  image->rows=480;
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    ThrowReaderException(FileOpenError,UnableToOpenFile,image);
  /*
    Color canvas with background color
  */
  background_color=image_info->background_color;
  for (y=0; y < (long) image->rows; y++)
  {
    q=SetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (long) image->columns; x++)
      *q++=background_color;
    if (!SyncImagePixels(image))
      break;
  }
  (void) strlcpy(image->magick,image_info->magick,MaxTextExtent);
  (void) strlcpy(image->filename,image_info->filename,MaxTextExtent);
  /*
    Prepare drawing commands
  */
  y=20;
  draw_info=CloneDrawInfo(image_info,(DrawInfo *) NULL);
  draw_info->font=AllocateString(image->filename);
  draw_info->fill=image_info->pen;
  context=DrawAllocateContext(draw_info,image);
  (void) DrawPushGraphicContext(context);
  (void) DrawSetViewbox(context,0,0,image->columns,image->rows);
  (void) DrawSetFont(context,image_info->filename);
  (void) DrawSetFontSize(context,18);
  (void) DrawAnnotation(context,10,y,(const unsigned char *) Text);
  y+=20*MultilineCensus(Text)+20;
  for (i=12; i <= 72; i+=6)
  {
    y+=i+12;
    (void) DrawSetFontSize(context,18);
    (void) FormatString(buffer,"%ld",i);
    (void) DrawAnnotation(context,10,y,(const unsigned char *) buffer);
    (void) DrawSetFontSize(context,i);
    (void) DrawAnnotation(context,50,y,(const unsigned char *)
      "That which does not destroy me, only makes me stronger.");
    if (i >= 24)
      i+=6;
  }
  (void) DrawPopGraphicContext(context);
  (void) DrawRender(context);
  /*
    Free resources.
  */
  DestroyDrawInfo(draw_info);
  DrawDestroyContext(context);
  CloseBlob(image);
  return(image);
}
#endif /* HasTTF */

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r T T F I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterTTFImage adds attributes for the TTF image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterTTFImage method is:
%
%      RegisterTTFImage(void)
%
*/
ModuleExport void RegisterTTFImage(void)
{
  static char
    version[MaxTextExtent];

  MagickInfo
    *entry;

  *version='\0';
#if defined(FREETYPE_MAJOR) && defined(FREETYPE_MINOR)
  FormatString(version,"%d.%d",FREETYPE_MAJOR,FREETYPE_MINOR);
#endif

  entry=SetMagickInfo("TTF");
#if defined(HasTTF)
  entry->decoder=(DecoderHandler) ReadTTFImage;
#endif
  entry->magick=(MagickHandler) IsTTF;
  entry->adjoin=False;
  entry->description="TrueType font";
  if (*version != '\0')
    entry->version=version;
  entry->module="TTF";
  entry->coder_class=PrimaryCoderClass;
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("PFA");
#if defined(HasTTF)
  entry->decoder=(DecoderHandler) ReadTTFImage;
#endif
  entry->magick=(MagickHandler) IsPFA;
  entry->adjoin=False;
  entry->description="Postscript Type 1 font (ASCII)";
  if (*version != '\0')
    entry->version=version;
  entry->module="TTF";
  entry->coder_class=PrimaryCoderClass;
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("PFB");
#if defined(HasTTF)
  entry->decoder=(DecoderHandler) ReadTTFImage;
#endif
  entry->magick=(MagickHandler) IsPFA;
  entry->adjoin=False;
  entry->description="Postscript Type 1 font (binary)";
  if (*version != '\0')
    entry->version=version;
  entry->module="TTF";
  entry->coder_class=PrimaryCoderClass;
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r T T F I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterTTFImage removes format registrations made by the
%  TTF module from the list of supported formats.
%
%  The format of the UnregisterTTFImage method is:
%
%      UnregisterTTFImage(void)
%
*/
ModuleExport void UnregisterTTFImage(void)
{
  (void) UnregisterMagickInfo("TTF");
  (void) UnregisterMagickInfo("PFA");
  (void) UnregisterMagickInfo("PFB");
}
