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
%                            V   V  IIIII  DDDD                               %
%                            V   V    I    D   D                              %
%                            V   V    I    D   D                              %
%                             V V     I    D   D                              %
%                              V    IIIII  DDDD                               %
%                                                                             %
%                                                                             %
%             Return a Visual Image Directory for matching images.            %
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
#include "magick/constitute.h"
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/montage.h"
#include "magick/resize.h"
#include "magick/utility.h"

/*
  Forward declarations.
*/
static unsigned int
  WriteVIDImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d V I D I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadVIDImage reads one of more images and creates a Visual Image
%  Directory file.  It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.
%
%  The format of the ReadVIDImage method is:
%
%      Image *ReadVIDImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadVIDImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadVIDImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
#define ClientName  "montage"

  char
    **filelist,
    **list;

  Image
    *image,
    *montage_image,
    *next_image,
    *thumbnail_image;

  ImageInfo
    *clone_info;

  int
    number_files;

  MonitorHandler
    handler;

  MontageInfo
    *montage_info;

  RectangleInfo
    geometry;

  register long
    i;

  unsigned int
    status;

  /*
    Expand the filename.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"enter");
  image=AllocateImage(image_info);
  list=MagickAllocateMemory(char **,sizeof(char *));
  if (list == (char **) NULL)
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),"return");
      ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image)
    }
  list[0]=(char *) AllocateString((char *) NULL);
  (void) strlcpy(list[0],image_info->filename,MaxTextExtent);
  number_files=1;
  filelist=list;
  status=ExpandFilenames(&number_files,&filelist);
  if ((status == False) || (number_files == 0))
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),"return");
      ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image)
    }
  DestroyImage(image);
  /*
    Read each image and convert them to a tile.
  */
  image=(Image *) NULL;
  clone_info=CloneImageInfo(image_info);
  clone_info->blob=(void *) NULL;
  clone_info->length=0;
  if (clone_info->size == (char *) NULL)
    (void) CloneString(&clone_info->size,DefaultTileGeometry);
  for (i=0; i < number_files; i++)
  {
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"name: %.1024s",
      filelist[i]);
    handler=SetMonitorHandler((MonitorHandler) NULL);
    (void) strlcpy(clone_info->filename,filelist[i],MaxTextExtent);
    *clone_info->magick='\0';
    next_image=ReadImage(clone_info,exception);
    MagickFreeMemory(filelist[i]);
    if (next_image != (Image *) NULL)
      {
        (void) SetImageAttribute(next_image,"label",DefaultTileLabel);
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),"geometry: %ldx%ld",
          next_image->columns,next_image->rows);
        SetGeometry(next_image,&geometry);
        (void) GetMagickGeometry(clone_info->size,&geometry.x,&geometry.y,
          &geometry.width,&geometry.height);
        thumbnail_image=ZoomImage(next_image,geometry.width,geometry.height,
          exception);
        if (thumbnail_image != (Image *) NULL)
          {
            DestroyImage(next_image);
            next_image=thumbnail_image;
          }
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "thumbnail geometry: %ldx%ld",next_image->columns,next_image->rows);
        if (image == (Image *) NULL)
          image=next_image;
        else
          {
            image->next=next_image;
            image->next->previous=image;
            image=SyncNextImageInList(image);
          }
      }
    (void) SetMonitorHandler(handler);
    if (!MagickMonitorFormatted(i,number_files,&image->exception,
                                LoadImageText,image->filename))
      break;
  }
  DestroyImageInfo(clone_info);
  MagickFreeMemory(filelist);
  if (image == (Image *) NULL)
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),"return");
      ThrowReaderException(CorruptImageError,UnableToReadVIDImage,image)
    }
  while (image->previous != (Image *) NULL)
    image=image->previous;
  /*
    Create the visual image directory.
  */
  montage_info=CloneMontageInfo(image_info,(MontageInfo *) NULL);
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"creating montage");
  montage_image=MontageImages(image,montage_info,exception);
  DestroyMontageInfo(montage_info);
  montage_info=(MontageInfo *) NULL;
  if (montage_image == (Image *) NULL)
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),"return");
      ThrowReaderException(CorruptImageError,UnableToReadVIDImage,image)
    }
  DestroyImageList(image);
  MagickFreeMemory(list[0]);
  MagickFreeMemory(list);
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"return");
  return(montage_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r V I D I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterVIDImage adds attributes for the VID image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterVIDImage method is:
%
%      RegisterVIDImage(void)
%
*/
ModuleExport void RegisterVIDImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("VID");
  entry->decoder=(DecoderHandler) ReadVIDImage;
  entry->encoder=(EncoderHandler) WriteVIDImage;
  entry->description="Visual Image Directory";
  entry->module="VID";
  entry->extension_treatment=IgnoreExtensionTreatment;
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r V I D I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterVIDImage removes format registrations made by the
%  VID module from the list of supported formats.
%
%  The format of the UnregisterVIDImage method is:
%
%      UnregisterVIDImage(void)
%
*/
ModuleExport void UnregisterVIDImage(void)
{
  (void) UnregisterMagickInfo("VID");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e V I D I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteVIDImage writes an image to a file in VID X image format.
%
%  The format of the WriteVIDImage method is:
%
%      unsigned int WriteVIDImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteVIDImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
static unsigned int WriteVIDImage(const ImageInfo *image_info,Image *image)
{
  Image
    *montage_image;

  MontageInfo
    *montage_info;

  register Image
    *p;

  unsigned int
    status;

  /*
    Create the visual image directory.
  */
  for (p=image; p != (Image *) NULL; p=p->next)
    (void) SetImageAttribute(p,"label",DefaultTileLabel);
  montage_info=CloneMontageInfo(image_info,(MontageInfo *) NULL);
  montage_image=MontageImages(image,montage_info,&image->exception);
  DestroyMontageInfo(montage_info);
  montage_info=(MontageInfo *) NULL;
  if (montage_image == (Image *) NULL)
    ThrowWriterException2(CorruptImageError,image->exception.reason,image);
  FormatString(montage_image->filename,"miff:%.1024s",image->filename);
  status=WriteImage(image_info,montage_image);
  DestroyImageList(montage_image);
  return(status);
}
