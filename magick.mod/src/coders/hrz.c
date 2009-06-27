/*
% Copyright (C) 2009 GraphicsMagick Group
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                            H   H  RRRR   ZZZZZ                              %
%                            H   H  R   R     ZZ                              %
%                            HHHHH  RRRR     Z                                %
%                            H   H  R R    ZZ                                 %
%                            H   H  R  R   ZZZZZ                              %
%                                                                             %
%                                                                             %
%                              Slow scan TV.                                  %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                              Jaroslav Fojtik                                %
%                                March 2009                                   %
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
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/utility.h"
#include "magick/constitute.h"


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d H R Z I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadHRZImage reads an HRZ X image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadHRZImage method is:
%
%      Image *ReadHRZImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadHRZImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadHRZImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image *image;
  int i;
  unsigned width,height;
  long ldblk, j;
  unsigned char *BImgBuff=NULL;
  unsigned int status;
  const PixelPacket *q;

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
    Read HRZ image.
  */
  width = 256;
  height = 240;

  ldblk = (long)(3*width);

  if(GetBlobSize(image)!=(((long)ldblk)*height))
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);

  image->columns = width;
  image->rows = height;
  image->depth = 8;

  /* printf("HRZ header checked OK %d,%d\n",image->colors,image->depth); */

  /* If ping is true, then only set image size and colors without reading any image data. */
  if (image_info->ping) goto DONE_READING;

  /* ----- Load RLE compressed raster ----- */
  BImgBuff=MagickAllocateMemory(unsigned char *,((size_t) ldblk));  /*Ldblk was set in the check phase*/
  if(BImgBuff==NULL)
    ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);

  for(i=0; i<(int)height; i++)
    {
      (void) ReadBlob(image,(size_t)ldblk,(char *)BImgBuff);

      for(j=0; j<ldblk; j++)
      {
        BImgBuff[j] <<= 2;
      }

      q = SetImagePixels(image,0,i,image->columns,1);
      if(q == (PixelPacket *)NULL) break;
      (void)ImportImagePixelArea(image,RGBQuantum,8,BImgBuff,NULL,0);
      if(!SyncImagePixels(image)) break;
    }

  if(BImgBuff!=NULL)
    MagickFreeMemory(BImgBuff);
  if (EOFBlob(image))
    ThrowException(exception, CorruptImageError, UnexpectedEndOfFile, image->filename);

DONE_READING:
  CloseBlob(image);
  return(image);
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e H R Z I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function WriteHRZImage writes an HRZ image to a file.  
%
%  The format of the WriteHRZImage method is:
%
%      unsigned int WriteHRZImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Function WriteHRZImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
*/
/*static unsigned int WriteHRZImage(const ImageInfo *image_info,Image *image)
{
    if (logging)
    (void)LogMagickEvent(CoderEvent,GetMagickModule(),"return HRZ");
  
  return(status);
} */


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r H R Z I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterHRZImage adds attributes for the HRZ image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterHRZImage method is:
%
%      RegisterHRZImage(void)
%
*/
ModuleExport void RegisterHRZImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("HRZ");
  entry->decoder = (DecoderHandler)ReadHRZImage;
/*entry->encoder = (EncoderHandler)WriteHRZImage; */
  entry->description="HRZ: Slow scan TV";
  entry->module="HRZ";
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r H R Z I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterHRZImage removes format registrations made by the
%  HRZ module from the list of supported formats.
%
%  The format of the UnregisterHRZImage method is:
%
%      UnregisterHRZImage(void)
%
*/
ModuleExport void UnregisterHRZImage(void)
{
  (void) UnregisterMagickInfo("HRZ");
}
