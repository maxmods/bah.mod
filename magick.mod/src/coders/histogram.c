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
%       H   H  IIIII  SSSSS  TTTTT   OOO    GGGG  RRRR    AAA   M   M         %
%       H   H    I    SS       T    O   O  G      R   R  A   A  MM MM         %
%       HHHHH    I     SSS     T    O   O  G  GG  RRRR   AAAAA  M M M         %
%       H   H    I       SS    T    O   O  G   G  R R    A   A  M   M         %
%       H   H  IIIII  SSSSS    T     OOO    GGG   R  R   A   A  M   M         %
%                                                                             %
%                                                                             %
%                          Write A Histogram Image.                           %
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
#include "magick/color.h"
#include "magick/constitute.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/tempfile.h"
#include "magick/utility.h"

/*
  Forward declarations.
*/
static unsigned int
  WriteHISTOGRAMImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r H I S T O G R A M I m a g e                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterHISTOGRAMImage adds attributes for the Histogram image format
%  to the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterHISTOGRAMImage method is:
%
%      RegisterHISTOGRAMImage(void)
%
*/
ModuleExport void RegisterHISTOGRAMImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("HISTOGRAM");
  entry->encoder=(EncoderHandler) WriteHISTOGRAMImage;
  entry->adjoin=False;
  entry->description="Histogram of the image";
  entry->module="HISTOGRAM";
  entry->coder_class=PrimaryCoderClass;
  entry->extension_treatment=IgnoreExtensionTreatment;
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r H I S T O G R A M I m a g e                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterHISTOGRAMImage removes format registrations made by the
%  HISTOGRAM module from the list of supported formats.
%
%  The format of the UnregisterHISTOGRAMImage method is:
%
%      UnregisterHISTOGRAMImage(void)
%
*/
ModuleExport void UnregisterHISTOGRAMImage(void)
{
  (void) UnregisterMagickInfo("HISTOGRAM");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e H I S T O G R A M I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteHISTOGRAMImage writes an image to a file in Histogram format.
%  The image shows a histogram of the color (or gray) values in the image.  The
%  image consists of three overlaid histograms:  a red one for the red channel,
%  a green one for the green channel, and a blue one for the blue channel.  The
%  image comment contains a list of unique pixel values and the number of times
%  each occurs in the image.
%
%  This method is strongly based on a similar one written by
%  muquit@warm.semcor.com which in turn is based on ppmhistmap of netpbm.
%
%  The format of the WriteHISTOGRAMImage method is:
%
%      unsigned int WriteHISTOGRAMImage(const ImageInfo *image_info,
%        Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteHISTOGRAMImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
static unsigned int WriteHISTOGRAMImage(const ImageInfo *image_info,
  Image *image)
{
#define HistogramDensity  "256x200"

  char
    filename[MaxTextExtent];

  double
    scale;

  FILE
    *file;

  Image
    *histogram_image;

  long
    *blue,
    *green,
    maximum,
    *red;

  long
    y;

  RectangleInfo
    geometry;

  register const PixelPacket
    *p;

  register long
    x;

  register PixelPacket
    *q,
    *r;

  unsigned int
    status;

  unsigned long
    length;

  /*
    Allocate histogram image.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  (void) SetImageDepth(image,image->depth);
  SetGeometry(image,&geometry);
  if (image_info->density == (char *) NULL)
    (void) GetMagickGeometry(HistogramDensity,&geometry.x,&geometry.y,
      &geometry.width,&geometry.height);
  else
    (void) GetMagickGeometry(image_info->density,&geometry.x,&geometry.y,
      &geometry.width,&geometry.height);
  histogram_image=CloneImage(image,geometry.width,geometry.height,True,
    &image->exception);
  if (histogram_image == (Image *) NULL)
    ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
  (void) SetImageType(histogram_image,TrueColorType);
  /*
    Allocate histogram count arrays.
  */
  length=Max(ScaleQuantumToChar(MaxRGB)+1,histogram_image->columns);
  red=MagickAllocateMemory(long *,length*sizeof(long));
  green=MagickAllocateMemory(long *,length*sizeof(long));
  blue=MagickAllocateMemory(long *,length*sizeof(long));
  if ((red == (long *) NULL) || (green == (long *) NULL) ||
      (blue == (long *) NULL))
    {
      DestroyImage(histogram_image);
      ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image)
    }
  (void) memset(red,0,length*sizeof(long));
  (void) memset(green,0,length*sizeof(long));
  (void) memset(blue,0,length*sizeof(long));
  /*
    Initialize histogram count arrays.
  */
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      red[ScaleQuantumToChar(p->red)]++;
      green[ScaleQuantumToChar(p->green)]++;
      blue[ScaleQuantumToChar(p->blue)]++;
      p++;
    }
  }
  maximum=0;
  for (x=0; x < (long) histogram_image->columns; x++)
  {
    if (maximum < red[x])
      maximum=red[x];
    if (maximum < green[x])
      maximum=green[x];
    if (maximum < blue[x])
      maximum=blue[x];
  }
  scale=(double) histogram_image->rows/maximum;
  /*
    Initialize histogram image.
  */
  (void) QueryColorDatabase("black",&histogram_image->background_color,
    &image->exception);
  (void) SetImage(histogram_image,OpaqueOpacity);
  for (x=0; x < (long) histogram_image->columns; x++)
  {
    q=GetImagePixels(histogram_image,x,0,1,histogram_image->rows);
    if (q == (PixelPacket *) NULL)
      break;
    y=(long) (histogram_image->rows-(long) (scale*red[x]));
    r=q+y;
    for ( ; y < (long) histogram_image->rows; y++)
    {
      r->red=MaxRGB;
      r++;
    }
    y=(long) (histogram_image->rows-(long) (scale*green[x]));
    r=q+y;
    for ( ; y < (long) histogram_image->rows; y++)
    {
      r->green=MaxRGB;
      r++;
    }
    y=(long) (histogram_image->rows-(long) (scale*blue[x]));
    r=q+y;
    for ( ; y < (long) histogram_image->rows; y++)
    {
      r->blue=MaxRGB;
      r++;
    }
    if (!SyncImagePixels(histogram_image))
      break;
    if (QuantumTick(x,histogram_image->columns))
      if (!MagickMonitorFormatted(x,histogram_image->columns,&image->exception,
                                  SaveImageText,image->filename))
        break;
  }
  /*
    Free memory resources.
  */
  MagickFreeMemory(blue);
  MagickFreeMemory(green);
  MagickFreeMemory(red);
  file=AcquireTemporaryFileStream(filename,BinaryFileIOMode);
  if (file == (FILE *) NULL)
    {
      DestroyImage(histogram_image);
      ThrowWriterTemporaryFileException(filename);
    }
    {
      char
        command[MaxTextExtent];

      /*
        Add a histogram as an image comment.
      */
      (void) GetNumberColors(image,file,&image->exception);
      (void) fclose(file);
      FormatString(command,"@%.1024s",filename);
      (void) SetImageAttribute(histogram_image,"comment",command);
      (void) LiberateTemporaryFile(filename);
    }
  /*
    Write Histogram image as MIFF.
  */
  (void) strlcpy(filename,histogram_image->filename,MaxTextExtent);
  (void) strlcpy(histogram_image->filename,"miff:",MaxTextExtent);
  (void) strlcat(histogram_image->filename,filename,MaxTextExtent);
  status=WriteImage(image_info,histogram_image);
  DestroyImage(histogram_image);
  return(status);
}
