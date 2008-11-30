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
%                            TTTTT  X   X  TTTTT                              %
%                              T     X X     T                                %
%                              T      X      T                                %
%                              T     X X     T                                %
%                              T    X   X    T                                %
%                                                                             %
%                                                                             %
%                      Render Text Onto A Canvas Image.                       %
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
#include "magick/color.h"
#include "magick/constitute.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/render.h"
#include "magick/utility.h"

/*
  Forward declarations.
*/
static unsigned int
  WriteTXTImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s T X T                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsTXT returns True if the image format type, identified by the
%  magick string, is TXT.
%
%  The format of the IsTXT method is:
%
%      unsigned int IsTXT(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsTXT returns True if the image format type is TXT.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsTXT(const unsigned char *magick,const size_t length)
{
  if (length < 22)
    return(False);
  {
    unsigned long
      column,
      row;

    unsigned int
      red,
      green,
      blue,
      opacity,
      hex_red,
      hex_green,
      hex_blue,
      hex_opacity;

    int
      count;

    char
      buffer[MaxTextExtent];

    (void) memset((void *)buffer,0,MaxTextExtent);
    (void) memcpy((void *)buffer,(const void *)magick,Min(MaxTextExtent,length));

    count=sscanf(buffer,"%lu,%lu: (%u, %u, %u) #%02X%02X%02X",
                 &column, &row, &red, &green, &blue, &hex_red, &hex_green,
                 &hex_blue);
    if ((count == 8) && (column == 0) && (row == 0) && (red == hex_red) &&
        (green == hex_green) && (blue == hex_blue))
      return(True);

    count=sscanf(buffer,"%lu,%lu: (%u, %u, %u) #%04X%04X%04X",
                 &column, &row, &red, &green, &blue, &hex_red, &hex_green,
                 &hex_blue);
    if ((count == 8) && (column == 0) && (row == 0) && (red == hex_red) &&
        (green == hex_green) && (blue == hex_blue))
      return(True);

    count=sscanf(buffer,"%lu,%lu: (%u, %u, %u) #%08X%08X%08X",
                 &column, &row, &red, &green, &blue, &hex_red, &hex_green,
                 &hex_blue);
    if ((count == 8) && (column == 0) && (row == 0) && (red == hex_red) &&
        (green == hex_green) && (blue == hex_blue))
      return(True);

    count=sscanf(buffer,"%lu,%lu: (%u, %u, %u, %u) #%02X%02X%02X%02X",
                 &column, &row, &red, &green, &blue, &opacity, &hex_red,
                 &hex_green, &hex_blue, &hex_opacity);
    if ((count == 10) && (column == 0) && (row == 0) && (red == hex_red) &&
        (green == hex_green) && (blue == hex_blue) && (opacity == hex_opacity))
      return(True);

    count=sscanf(buffer,"%lu,%lu: (%u, %u, %u, %u) #%04X%04X%04X%04X",
                 &column, &row, &red, &green, &blue, &opacity, &hex_red,
                 &hex_green, &hex_blue, &hex_opacity);
    if ((count == 10) && (column == 0) && (row == 0) && (red == hex_red) &&
        (green == hex_green) && (blue == hex_blue) && (opacity == hex_opacity))
      return(True);

    count=sscanf(buffer,"%lu,%lu: (%u, %u, %u, %u) #%08X%08X%08X%08X",
                 &column, &row, &red, &green, &blue, &opacity, &hex_red,
                 &hex_green, &hex_blue, &hex_opacity);
    if ((count == 10) && (column == 0) && (row == 0) && (red == hex_red) &&
        (green == hex_green) && (blue == hex_blue) && (opacity == hex_opacity))
      return(True);
  }
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d T X T I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadTXTImage reads a text file and returns it as an image.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadTXTImage method is:
%
%      Image *ReadTXTImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadTXTImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadTXTImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    filename[MaxTextExtent],
    geometry[MaxTextExtent],
    *p,
    text[MaxTextExtent];

  double
    dx_resolution,
    dy_resolution;

  DrawInfo
    *draw_info;

  Image
    *image,
    *texture;

  long
    count,
    offset;

  RectangleInfo
    page;

  TypeMetric
    metrics;

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
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    ThrowReaderException(FileOpenError,UnableToOpenFile,image);
  /*
    Set the page geometry.
  */
  dx_resolution=72.0;
  dy_resolution=72.0;
  if ((image->x_resolution == 0.0) || (image->y_resolution == 0.0))
    {
      char
        density[MaxTextExtent];

      (void) strcpy(density,PSDensityGeometry);
      count=GetMagickDimension(density,&image->x_resolution,
        &image->y_resolution,NULL,NULL);
      if (count != 2)
        image->y_resolution=image->x_resolution;
    }
  SetGeometry(image,&page);
  page.width=612;
  page.height=792;
  (void) GetGeometry("612x792+43+43",&page.x,&page.y,&page.width,&page.height);
  if (image_info->page != (char *) NULL)
    (void) GetGeometry(image_info->page,&page.x,&page.y,&page.width,
      &page.height);
  /*
    Initialize Image structure.
  */
  image->columns=(unsigned long)
    ceil(((page.width*image->x_resolution)/dx_resolution)-0.5);
  image->rows=(unsigned long)
    ceil(((page.height*image->y_resolution)/dy_resolution)-0.5);
  texture=(Image *) NULL;
  if (image_info->texture != (char *) NULL)
    {
      ImageInfo
        *clone_info;

      clone_info=CloneImageInfo(image_info);
      clone_info->blob=(void *) NULL;
      clone_info->length=0;
      (void) strlcpy(clone_info->filename,image_info->texture,MaxTextExtent);
      texture=ReadImage(clone_info,exception);
      DestroyImageInfo(clone_info);
    }
  /*
    Annotate the text image.
  */
  (void) SetImage(image,OpaqueOpacity);
  draw_info=CloneDrawInfo(image_info,(DrawInfo *) NULL);
  draw_info->fill=image_info->pen;
  (void) CloneString(&draw_info->text,image_info->filename);
  FormatString(geometry,"0x0%+ld%+ld",page.x,page.y);
  (void) CloneString(&draw_info->geometry,geometry);
  status=GetTypeMetrics(image,draw_info,&metrics);
  if (status == False)
    ThrowReaderException(TypeError,UnableToGetTypeMetrics,image);
  (void) strlcpy(filename,image_info->filename,MaxTextExtent);
  if (draw_info->text != '\0')
    *draw_info->text='\0';
  p=ReadBlobString(image,text);

  if (IsTXT((unsigned char *)p,strlen(p)))
    {
      ThrowReaderException(CoderError,ImageTypeNotSupported,image);
    }

  for (offset=2*page.y; p != (char *) NULL; )
  {
    /*
      Annotate image with text.
    */
    (void) ConcatenateString(&draw_info->text,text);
    (void) ConcatenateString(&draw_info->text,"\\n");
    offset+=(long) (metrics.ascent-metrics.descent);
    if (image->previous == (Image *) NULL)
      if (QuantumTick(offset,image->rows))
        if (!MagickMonitorFormatted(offset,image->rows,&image->exception,
                                    LoadImageText,image->filename))
          break;
    p=ReadBlobString(image,text);
    if ((offset < (long) image->rows) && (p != (char *) NULL))
      continue;
    if (texture != (Image *) NULL)
      {
        MonitorHandler
          handler;

        handler=SetMonitorHandler((MonitorHandler) NULL);
        (void) TextureImage(image,texture);
        (void) SetMonitorHandler(handler);
      }
    (void) AnnotateImage(image,draw_info);
    if (p == (char *) NULL)
      break;
    /*
      Page is full-- allocate next image structure.
    */
    *draw_info->text='\0';
    offset=2*page.y;
    AllocateNextImage(image_info,image);
    if (image->next == (Image *) NULL)
      {
        DestroyImageList(image);
        return((Image *) NULL);
      }
    image->next->columns=image->columns;
    image->next->rows=image->rows;
    image=SyncNextImageInList(image);
    (void) strlcpy(image->filename,filename,MaxTextExtent);
    (void) SetImage(image,OpaqueOpacity);
    if (!MagickMonitorFormatted(TellBlob(image),GetBlobSize(image),exception,
                                LoadImagesText,image->filename))
      break;
  }
  if (texture != (Image *) NULL)
    {
      MonitorHandler
        handler;

      handler=SetMonitorHandler((MonitorHandler) NULL);
      (void) TextureImage(image,texture);
      (void) SetMonitorHandler(handler);
    }
  (void) AnnotateImage(image,draw_info);
  if (texture != (Image *) NULL)
    DestroyImage(texture);
  DestroyDrawInfo(draw_info);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseBlob(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r T X T I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterTXTImage adds attributes for the TXT image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterTXTImage method is:
%
%      RegisterTXTImage(void)
%
*/
ModuleExport void RegisterTXTImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("TEXT");
  entry->decoder=(DecoderHandler) ReadTXTImage;
  entry->encoder=(EncoderHandler) WriteTXTImage;
  entry->raw=True;
  entry->description="Text";
  entry->module="TXT";
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("TXT");
  entry->decoder=(DecoderHandler) ReadTXTImage;
  entry->encoder=(EncoderHandler) WriteTXTImage;
  entry->description="Text";
  entry->module="TXT";
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r T X T I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterTXTImage removes format registrations made by the
%  TXT module from the list of supported formats.
%
%  The format of the UnregisterTXTImage method is:
%
%      UnregisterTXTImage(void)
%
*/
ModuleExport void UnregisterTXTImage(void)
{
  (void) UnregisterMagickInfo("TEXT");
  (void) UnregisterMagickInfo("TXT");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e T X T I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteTXTImage writes the pixel values as text numbers.
%
%  The format of the WriteTXTImage method is:
%
%      unsigned int WriteTXTImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteTXTImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
static unsigned int WriteTXTImage(const ImageInfo *image_info,Image *image)
{
  char
    buffer[MaxTextExtent],
    tuple[MaxTextExtent];

  long
    y;

  register const PixelPacket
    *p;

  register long
    x;

  unsigned int
    status;

  unsigned long
    scene;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=OpenBlob(image_info,image,WriteBlobMode,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenError,UnableToOpenFile,image);
  scene=0;
  do
  {
    /*
      Convert MIFF to TXT raster pixels.
    */
    (void) TransformColorspace(image,RGBColorspace);
    for (y=0; y < (long) image->rows; y++)
    {
      p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
      if (p == (const PixelPacket *) NULL)
        break;
      for (x=0; x < (long) image->columns; x++)
      {
        FormatString(buffer,"%ld,%ld: ",x,y);
        (void) WriteBlobString(image,buffer);
        GetColorTuple(p,image->depth,image->matte,False,tuple);
        (void) strcat(tuple," ");
        (void) WriteBlobString(image,tuple);
        (void) QueryColorname(image,p,SVGCompliance,tuple,&image->exception);
        (void) WriteBlobString(image,tuple);
        (void) WriteBlobString(image,"\n");
        p++;
      }
    }
    if (image->next == (Image *) NULL)
      break;
    image=SyncNextImageInList(image);
    status=MagickMonitorFormatted(scene++,GetImageListLength(image),
                                  &image->exception,SaveImagesText,
                                  image->filename);
    if (status == False)
      break;
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseBlob(image);
  return(True);
}
