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
%                        H   H  TTTTT  M   M  L                               %
%                        H   H    T    MM MM  L                               %
%                        HHHHH    T    M M M  L                               %
%                        H   H    T    M   M  L                               %
%                        H   H    T    M   M  LLLLL                           %
%                                                                             %
%                                                                             %
%                  Write A Client-Side Image Map Using                        %
%                Image Montage & Directory Information.                       %
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
#include "magick/attribute.h"
#include "magick/blob.h"
#include "magick/pixel_cache.h"
#include "magick/constitute.h"
#include "magick/magick.h"
#include "magick/paint.h"
#include "magick/utility.h"

/*
  Forward declarations.
*/
static unsigned int
  WriteHTMLImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s H T M L                                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsHTML returns True if the image format type, identified by the
%  magick string, is HTML.
%
%  The format of the IsHTML method is:
%
%      unsigned int IsHTML(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsHTML returns True if the image format type is HTML.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsHTML(const unsigned char *magick,const size_t length)
{
  if (length < 5)
    return(False);
  if (LocaleNCompare((char *) magick,"<html",5) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r H T M L I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterHTMLImage adds attributes for the HTML image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterHTMLImage method is:
%
%      RegisterHTMLImage(void)
%
*/
ModuleExport void RegisterHTMLImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("HTM");
  entry->encoder=(EncoderHandler) WriteHTMLImage;
  entry->magick=(MagickHandler) IsHTML;
  entry->adjoin=False;
  entry->description="Hypertext Markup Language and a client-side image map";
  entry->stealth=MagickTrue;
  entry->module="HTML";
  entry->coder_class=PrimaryCoderClass;
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("HTML");
  entry->encoder=(EncoderHandler) WriteHTMLImage;
  entry->magick=(MagickHandler) IsHTML;
  entry->adjoin=False;
  entry->description="Hypertext Markup Language and a client-side image map";
  entry->module="HTML";
  entry->coder_class=PrimaryCoderClass;
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("SHTML");
  entry->encoder=(EncoderHandler) WriteHTMLImage;
  entry->magick=(MagickHandler) IsHTML;
  entry->adjoin=False;
  entry->description="Hypertext Markup Language and a client-side image map";
  entry->module="HTML";
  entry->coder_class=PrimaryCoderClass;
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r H T M L I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterHTMLImage removes format registrations made by the
%  HTML module from the list of supported formats.
%
%  The format of the UnregisterHTMLImage method is:
%
%      UnregisterHTMLImage(void)
%
*/
ModuleExport void UnregisterHTMLImage(void)
{
  (void) UnregisterMagickInfo("HTM");
  (void) UnregisterMagickInfo("HTML");
  (void) UnregisterMagickInfo("SHTML");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e H T M L I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteHTMLImage writes an image in the HTML encoded image format.
%
%  The format of the WriteHTMLImage method is:
%
%      unsigned int WriteHTMLImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteHTMLImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
static unsigned int WriteHTMLImage(const ImageInfo *image_info,Image *image)
{
  char
    basename[MaxTextExtent],
    buffer[MaxTextExtent],
    filename[MaxTextExtent],
    mapname[MaxTextExtent],
    url[MaxTextExtent];

  Image
    *next;

  ImageInfo
    *clone_info;

  RectangleInfo
    geometry;

  register char
    *p;

  unsigned int
    status;

  /*
    Open image.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenError,UnableToOpenFile,image);
  CloseBlob(image);
  (void) TransformColorspace(image,RGBColorspace);
  *url='\0';
  if ((LocaleCompare(image_info->magick,"FTP") == 0) ||
      (LocaleCompare(image_info->magick,"HTTP") == 0))
    {
      /*
        Extract URL base from filename.
      */
      p=strrchr(image->filename,'/');
      if (p)
        {
          p++;
          (void) strlcpy(url,image_info->magick,MaxTextExtent);
          (void) strlcat(url,":",MaxTextExtent);
          url[strlen(url)+p-image->filename]='\0';
          (void) strlcat(url,image->filename,(size_t)(p-image->filename+1));
          (void) strlcpy(image->filename,p,MaxTextExtent);
        }
    }
  /*
    Refer to image map file.
  */
  (void) strlcpy(filename,image->filename,MaxTextExtent);
  AppendImageFormat("map",filename);
  GetPathComponent(filename,BasePath,basename);
  (void) strlcpy(mapname,basename,MaxTextExtent);
  (void) strlcpy(image->filename,image_info->filename,MaxTextExtent);
  (void) strlcpy(filename,image->filename,MaxTextExtent);
  clone_info=CloneImageInfo(image_info);
  clone_info->adjoin=True;
  status=True;
  if (LocaleCompare(image_info->magick,"SHTML") != 0)
    {
      const ImageAttribute
        *attribute;

      /*
        Open output image file.
      */
      status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
      if (status == False)
        ThrowWriterException(FileOpenError,UnableToOpenFile,image);
      /*
        Write the HTML image file.
      */
      (void) WriteBlobString(image,"<html version=\"2.0\">\n");
      (void) WriteBlobString(image,"<head>\n");
      attribute=GetImageAttribute(image,"label");
      if (attribute != (const ImageAttribute *) NULL)
        FormatString(buffer,"<title>%.1024s</title>\n",attribute->value);
      else
        {
          GetPathComponent(filename,BasePath,basename);
          FormatString(buffer,"<title>%.1024s</title>\n",basename);
        }
      (void) WriteBlobString(image,buffer);
      (void) WriteBlobString(image,"</head>\n");
      (void) WriteBlobString(image,"<body>\n");
      (void) WriteBlobString(image,"<center>\n");
      FormatString(buffer,"<h1>%.1024s</h1>\n",image->filename);
      (void) WriteBlobString(image,buffer);
      (void) WriteBlobString(image,"<br><br>\n");
      (void) strlcpy(filename,image->filename,MaxTextExtent);
      AppendImageFormat("gif",filename);
      FormatString(buffer,
        "<img ismap usemap=\"#%.1024s\" src=\"%.1024s\" border=0>\n",
        mapname,filename);
      (void) WriteBlobString(image,buffer);
      /*
        Determine the size and location of each image tile.
      */
      SetGeometry(image,&geometry);
      if (image->montage != (char *) NULL)
        {
          (void) GetGeometry(image->montage,&geometry.x,&geometry.y,
            &geometry.width,&geometry.height);
          (void) GetMagickGeometry(image->montage,&geometry.x,&geometry.y,
            &geometry.width,&geometry.height);
        }
      /*
        Write an image map.
      */
      FormatString(buffer,"<map name=\"%.1024s\">\n",mapname);
      (void) WriteBlobString(image,buffer);
      FormatString(buffer,"  <area href=\"%.1024s",url);
      (void) WriteBlobString(image,buffer);
      if (image->directory == (char *) NULL)
        {
          FormatString(buffer,"%.1024s\" shape=rect coords=\"0,0,%lu,%lu\">\n",
            image->filename,geometry.width-1,geometry.height-1);
          (void) WriteBlobString(image,buffer);
        }
      else
        for (p=image->directory; *p != '\0'; p++)
          if (*p != '\n')
            (void) WriteBlobByte(image,*p);
          else
            {
              FormatString(buffer,"\" shape=rect coords=\"%ld,%ld,%ld,%ld\">\n",
                geometry.x,geometry.y,geometry.x+(long) geometry.width-1,
                geometry.y+(long) geometry.height-1);
              (void) WriteBlobString(image,buffer);
              if (*(p+1) != '\0')
                {
                  FormatString(buffer,"  <area href=%.1024s\"",url);
                  (void) WriteBlobString(image,buffer);
                }
              geometry.x+=geometry.width;
              if (geometry.x >= (long) image->columns)
                {
                  geometry.x=0;
                  geometry.y+=geometry.height;
                }
            }
      (void) WriteBlobString(image,"</map>\n");
      if (image->montage != (char *) NULL)
        {
          PixelPacket
            transparent_color;

          (void) AcquireOnePixelByReference(image,&transparent_color,0,0,&image->exception);
          (void) TransparentImage(image,transparent_color,TransparentOpacity);
        }
      (void) strlcpy(filename,image->filename,MaxTextExtent);
      (void) WriteBlobString(image,"</center>\n");
      (void) WriteBlobString(image,"</body>\n");
      (void) WriteBlobString(image,"</html>\n");
      CloseBlob(image);
      /*
        Write the image as transparent GIF.
      */
      (void) strlcpy(image->filename,filename,MaxTextExtent);
      AppendImageFormat("gif",image->filename);
      next=image->next;
      image->next=(Image *) NULL;
      (void) strcpy(image->magick,"GIF");
      (void) WriteImage(clone_info,image);
      image->next=next;
      /*
        Determine image map filename.
      */
      (void) strlcpy(image->filename,filename,MaxTextExtent);
      for (p=filename+strlen(filename)-1; p > (filename+1); p--)
        if (*p == '.')
          {
            (void) strncpy(image->filename,filename,(size_t) (p-filename));
            image->filename[p-filename]='\0';
            break;
          }
      (void) strcat(image->filename,"_map.shtml");
    }
  /*
    Open image map.
  */
  status=OpenBlob(clone_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenError,UnableToOpenFile,image);
  DestroyImageInfo(clone_info);
  /*
    Determine the size and location of each image tile.
  */
  SetGeometry(image,&geometry);
  if (image->montage != (char *) NULL)
    {
      (void) GetGeometry(image->montage,&geometry.x,&geometry.y,
        &geometry.width,&geometry.height);
      (void) GetMagickGeometry(image->montage,&geometry.x,&geometry.y,
        &geometry.width,&geometry.height);
    }
  /*
    Write an image map.
  */
  FormatString(buffer,"<map name=\"%.1024s\">\n",mapname);
  (void) WriteBlobString(image,buffer);
  FormatString(buffer,"  <area href=\"%.1024s",url);
  (void) WriteBlobString(image,buffer);
  if (image->directory == (char *) NULL)
    {
      FormatString(buffer,"%.1024s\" shape=rect coords=\"0,0,%lu,%lu\">\n",
        image->filename,geometry.width-1,geometry.height-1);
      (void) WriteBlobString(image,buffer);
    }
  else
    for (p=image->directory; *p != '\0'; p++)
      if (*p != '\n')
        (void) WriteBlobByte(image,*p);
      else
        {
          FormatString(buffer,"\" shape=rect coords=\"%ld,%ld,%ld,%ld\">\n",
            geometry.x,geometry.y,geometry.x+(long) geometry.width-1,
            geometry.y+(long) geometry.height-1);
          (void) WriteBlobString(image,buffer);
          if (*(p+1) != '\0')
            {
              FormatString(buffer,"  <area href=%.1024s\"",url);
              (void) WriteBlobString(image,buffer);
            }
          geometry.x+=geometry.width;
          if (geometry.x >= (long) image->columns)
            {
              geometry.x=0;
              geometry.y+=geometry.height;
            }
        }
  (void) WriteBlobString(image,"</map>\n");
  CloseBlob(image);
  (void) strlcpy(image->filename,filename,MaxTextExtent);
  return(status);
}
