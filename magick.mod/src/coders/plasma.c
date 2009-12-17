/*
% Copyright (C) 2003-2009 GraphicsMagick Group
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
%                 PPPP   L       AAA   SSSSS  M   M   AAA                     %
%                 P   P  L      A   A  SS     MM MM  A   A                    %
%                 PPPP   L      AAAAA   SSS   M M M  AAAAA                    %
%                 P      L      A   A     SS  M   M  A   A                    %
%                 P      LLLLL  A   A  SSSSS  M   M  A   A                    %
%                                                                             %
%                                                                             %
%                          Read a Plasma Image.                               %
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
#include "magick/constitute.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/plasma.h"
#include "magick/random.h"
#include "magick/utility.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d P L A S M A I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPlasmaImage creates a plasma fractal image.  The image is
%  initialized to to the X server color as specified by the filename.
%
%  The format of the ReadPlasmaImage method is:
%
%      Image *ReadPlasmaImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadPlasmaImage returns a pointer to the image after
%      creating it. A null image is returned if there is a memory shortage
%      or if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/

static void PlasmaPixel(Image *image,double x,double y)
{
  register PixelPacket
    *q;

  q=GetImagePixels(image,(long) (x+0.5),(long) (y+0.5),1,1);
  if (q == (PixelPacket *) NULL)
    return;
  q->red=(Quantum) (MaxRGBDouble*MagickRandomReal()+0.5);
  q->green=(Quantum) (MaxRGBDouble*MagickRandomReal()+0.5);
  q->blue=(Quantum) (MaxRGBDouble*MagickRandomReal()+0.5);
  (void) SyncImagePixels(image);
}

static Image *ReadPlasmaImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
#define PlasmaImageText  "[%s] Applying image plasma..."

  Image
    *image;

  ImageInfo
    *clone_info;

  long
    y;

  register long
    i,
    x;

  register PixelPacket
    *q;

  SegmentInfo
    segment_info;

  unsigned long
    depth,
    max_depth;

  /*
    Recursively apply plasma to the image.
  */
  clone_info=CloneImageInfo(image_info);
  clone_info->blob=(void *) NULL;
  clone_info->length=0;
  (void) FormatString(clone_info->filename,"gradient:%.1024s",
    image_info->filename);
  image=ReadImage(clone_info,exception);
  DestroyImageInfo(clone_info);
  if (image == (Image *) NULL)
    return(image);
  image->storage_class=DirectClass;
  for (y=0; y < (long) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      q->opacity=(Quantum) (MaxRGB/2);
      q++;
    }
    if (!SyncImagePixels(image))
      break;
  }
  segment_info.x1=0;
  segment_info.y1=0;
  segment_info.x2=image->columns-1;
  segment_info.y2=image->rows-1;
  if (LocaleCompare(image_info->filename,"fractal") == 0)
    {
      /*
        Seed pixels before recursion.
      */
      PlasmaPixel(image,segment_info.x1,segment_info.y1);
      PlasmaPixel(image,segment_info.x1,(segment_info.y1+segment_info.y2)/2);
      PlasmaPixel(image,segment_info.x1,segment_info.y2);
      PlasmaPixel(image,(segment_info.x1+segment_info.x2)/2,segment_info.y1);
      PlasmaPixel(image,(segment_info.x1+segment_info.x2)/2,
        (segment_info.y1+segment_info.y2)/2);
      PlasmaPixel(image,(segment_info.x1+segment_info.x2)/2,segment_info.y2);
      PlasmaPixel(image,segment_info.x2,segment_info.y1);
      PlasmaPixel(image,segment_info.x2,(segment_info.y1+segment_info.y2)/2);
      PlasmaPixel(image,segment_info.x2,segment_info.y2);
    }
  i=(long) (Max(image->columns,image->rows) >> 1);
  for (max_depth=0; i != 0; max_depth++)
    i>>=1;
  for (depth=1; ; depth++)
  {
    if (!MagickMonitorFormatted(depth,max_depth,&image->exception,
                                PlasmaImageText,image->filename))
      break;
    if (PlasmaImage(image,&segment_info,0,depth))
      break;
  }
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r P L A S M A I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterPLASMAImage adds attributes for the Plasma image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterPLASMAImage method is:
%
%      RegisterPLASMAImage(void)
%
*/
ModuleExport void RegisterPLASMAImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("PLASMA");
  entry->decoder=(DecoderHandler) ReadPlasmaImage;
  entry->adjoin=False;
  entry->description="Plasma fractal image";
  entry->module="PLASMA";
  entry->coder_class=PrimaryCoderClass;
  entry->extension_treatment=IgnoreExtensionTreatment;
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("FRACTAL");
  entry->decoder=(DecoderHandler) ReadPlasmaImage;
  entry->adjoin=False;
  entry->description="Plasma fractal image";
  entry->module="PLASMA";
  entry->coder_class=PrimaryCoderClass;
  entry->extension_treatment=IgnoreExtensionTreatment;
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r P L A S M A I m a g e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterPLASMAImage removes format registrations made by the
%  PLASMA module from the list of supported formats.
%
%  The format of the UnregisterPLASMAImage method is:
%
%      UnregisterPLASMAImage(void)
%
*/
ModuleExport void UnregisterPLASMAImage(void)
{
  (void) UnregisterMagickInfo("FRACTAL");
  (void) UnregisterMagickInfo("PLASMA");
}
