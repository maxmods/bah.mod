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
%                            PPPP   W   W  PPPP                               %
%                            P   P  W   W  P   P                              %
%                            PPPP   W   W  PPPP                               %
%                            P      W W W  P                                  %
%                            P       W W   P                                  %
%                                                                             %
%                                                                             %
%                   Read Seattle Film Works Image Format.                     %
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
#include "magick/constitute.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/tempfile.h"
#include "magick/utility.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s P W P                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsPWP returns True if the image format type, identified by the
%  magick string, is PWP.
%
%  The format of the IsPWP method is:
%
%      unsigned int IsPWP(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsPWP returns True if the image format type is PWP.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsPWP(const unsigned char *magick,const size_t length)
{
  if (length < 5)
    return(False);
  if (LocaleNCompare((char *) magick,"SFW95",5) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d P W P I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPWPImage reads a Seattle Film Works multi-image file and returns
%  it.  It allocates the memory necessary for the new Image structure and
%  returns a pointer to the new image.
%
%  The format of the ReadPWPImage method is:
%
%      Image *ReadPWPImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadPWPImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadPWPImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  FILE
    *file;

  Image
    *image,
    *next_image,
    *pwp_image;

  ImageInfo
    *clone_info;

  int
    c;

  MonitorHandler
    handler;

  register Image
    *p;

  register long
    i;

  size_t
    count;

  unsigned char
    magick[MaxTextExtent];

  unsigned int
    status;

  unsigned long
    filesize;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=(Image *) NULL;
  pwp_image=AllocateImage(image_info);
  status=OpenBlob(image_info,pwp_image,ReadBinaryBlobMode,exception);
  if (status == False)
    ThrowReaderException(FileOpenError,UnableToOpenFile,pwp_image);
  count=ReadBlob(pwp_image,5,(char *) magick);
  if ((count == 0) || (LocaleNCompare((char *) magick,"SFW95",5) != 0))
    ThrowReaderException(CorruptImageError,ImproperImageHeader,pwp_image);
  clone_info=CloneImageInfo(image_info);
  clone_info->blob=(void *) NULL;
  clone_info->length=0;
  for ( ; ; )
  {
    for (c=ReadBlobByte(pwp_image); c != EOF; c=ReadBlobByte(pwp_image))
    {
      for (i=0; i < 17; i++)
        magick[i]=magick[i+1];
      magick[17]=(unsigned char) c;
      if (LocaleNCompare((char *) (magick+12),"SFW94A",6) == 0)
        break;
    }
    if (c == EOF)
      break;
    if (LocaleNCompare((char *) (magick+12),"SFW94A",6) != 0)
      {
        ThrowReaderException(CorruptImageError,ImproperImageHeader,pwp_image);
      }
    /*
      Dump SFW image to a temporary file.
    */
    file=AcquireTemporaryFileStream(clone_info->filename,BinaryFileIOMode);
    if (file == (FILE *) NULL)
      {
        char
          filename[MaxTextExtent];

        (void) strcpy(filename,clone_info->filename);
        DestroyImageInfo(clone_info);
        ThrowReaderTemporaryFileException(filename);
      }
    (void) fwrite("SFW94A",1,6,file);
    filesize=65535L*magick[2]+256L*magick[1]+magick[0];
    for (i=0; i < (long) filesize; i++)
    {
      c=ReadBlobByte(pwp_image);
      (void) fputc(c,file);
    }
    (void) fclose(file);
    handler=SetMonitorHandler((MonitorHandler) NULL);
    next_image=ReadImage(clone_info,exception);
    (void) LiberateTemporaryFile(clone_info->filename);
    (void) SetMonitorHandler(handler);
    if (next_image == (Image *) NULL)
      break;
    FormatString(next_image->filename,"slide_%02ld.sfw",next_image->scene);
    if (image == (Image *) NULL)
      image=next_image;
    else
      {
        /*
          Link image into image list.
        */
        for (p=image; p->next != (Image *) NULL; p=p->next);
        next_image->previous=p;
        next_image->scene=p->scene+1;
        p->next=next_image;
      }
    if (image_info->subrange != 0)
      if (next_image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    if (!MagickMonitorFormatted(TellBlob(pwp_image),GetBlobSize(image),
                                &image->exception,LoadImagesText,
                                image->filename))
      break;
  }
  DestroyImageInfo(clone_info);
  CloseBlob(pwp_image);
  DestroyImage(pwp_image);
  if (EOFBlob(image))
    ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,
      image->filename);
  CloseBlob(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r P W P I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterPWPImage adds attributes for the PWP image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterPWPImage method is:
%
%      RegisterPWPImage(void)
%
*/
ModuleExport void RegisterPWPImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("PWP");
  entry->decoder=(DecoderHandler) ReadPWPImage;
  entry->magick=(MagickHandler) IsPWP;
  entry->description="Seattle Film Works";
  entry->module="PWP";
  entry->coder_class=UnstableCoderClass;
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r P W P I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterPWPImage removes format registrations made by the
%  PWP module from the list of supported formats.
%
%  The format of the UnregisterPWPImage method is:
%
%      UnregisterPWPImage(void)
%
*/
ModuleExport void UnregisterPWPImage(void)
{
  (void) UnregisterMagickInfo("PWP");
}
