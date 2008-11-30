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
%                             AAA   RRRR   TTTTT                              %
%                            A   A  R   R    T                                %
%                            AAAAA  RRRR     T                                %
%                            A   A  R R      T                                %
%                            A   A  R  R     T                                %
%                                                                             %
%                                                                             %
%                Read/Write PFS: 1st Publisher Image Format.                  %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                              Jaroslav Fojtik                                %
%                            January 2001 - 2007                              %
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
%   R e a d A R T I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadARTImage reads an ART X image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadARTImage method is:
%
%      Image *ReadARTImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadARTImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadARTImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image *image;
  int i;
  unsigned width,height,dummy;
  long ldblk;
  unsigned char *BImgBuff=NULL;
  unsigned char Padding;
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
    Read ART image.
  */
  dummy=ReadBlobLSBShort(image);
  width=ReadBlobLSBShort(image);
  dummy=ReadBlobLSBShort(image);
  height=ReadBlobLSBShort(image);

  ldblk=(long) ((width+7) / 8);
  Padding=(unsigned char) ((-ldblk) & 0x01);

  if(GetBlobSize(image)!=(8+((long)ldblk+Padding)*height))
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);

  image->columns=width;
  image->rows=height;
  image->depth=1;
  image->colors=1l << image->depth;
 
  /* printf("ART header checked OK %d,%d\n",image->colors,image->depth); */

  if (!AllocateImageColormap(image,image->colors)) goto NoMemory;

  /* If ping is true, then only set image size and colors without reading any image data. */
  if (image_info->ping) goto DONE_READING;

  /* ----- Load RLE compressed raster ----- */
  BImgBuff=MagickAllocateMemory(unsigned char *,((size_t) ldblk));  /*Ldblk was set in the check phase*/
  if(BImgBuff==NULL)
    NoMemory:
  ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);

  for(i=0; i<(int)height; i++)
    {
      (void) ReadBlob(image,(size_t)ldblk,(char *)BImgBuff);
      (void) ReadBlob(image,Padding,(char *)&dummy);      

      q=SetImagePixels(image,0,i,image->columns,1);
      if (q == (PixelPacket *)NULL) break;
      (void)ImportImagePixelArea(image,GrayQuantum,1,BImgBuff,NULL,0);
      if (!SyncImagePixels(image)) break;
    }
  if(BImgBuff!=NULL)
    MagickFreeMemory(BImgBuff);
  if (EOFBlob(image))
    ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,
                   image->filename);

DONE_READING:
  CloseBlob(image);
  return(image);
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e A R T I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function WriteARTImage writes an ART image to a file.  
%
%  The format of the WriteARTImage method is:
%
%      unsigned int WriteARTImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Function WriteARTImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
*/
static unsigned int WriteARTImage(const ImageInfo *image_info,Image *image)
{
  long y;
  unsigned dummy = 0;
  long DataSize;
  const PixelPacket *q;
  unsigned int status;
  unsigned char Padding;
  int logging;
  unsigned char *pixels;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  logging=LogMagickEvent(CoderEvent,GetMagickModule(),"enter ART");
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenError,UnableToOpenFile,image);

  DataSize = (long)((image->columns+7) / 8);
  Padding = (unsigned char)((-DataSize) & 0x01);  

  pixels=MagickAllocateMemory(unsigned char *,(size_t) (DataSize));
  if (pixels == (unsigned char *) NULL)
    ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);

 /*
    Write ART hader.
  */
  (void) WriteBlobLSBShort(image,0);
  (void) WriteBlobLSBShort(image,image->columns);
  (void) WriteBlobLSBShort(image,0);
  (void) WriteBlobLSBShort(image,image->rows);

  /*
    Store image data.
  */
  for(y=0; y<(long)image->rows; y++)
  {
    q = AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    (void)ExportImagePixelArea(image,GrayQuantum,1,pixels,0,0);
    (void)WriteBlob(image,DataSize,pixels);
    (void)WriteBlob(image,Padding,(char *)&dummy);
  }

  status=True;

  CloseBlob(image);
  MagickFreeMemory(pixels);

  if (logging)
    (void)LogMagickEvent(CoderEvent,GetMagickModule(),"return ART");
  
  return(status);
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r A R T I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterARTImage adds attributes for the ART image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterARTImage method is:
%
%      RegisterARTImage(void)
%
*/
ModuleExport void RegisterARTImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("ART");
  entry->decoder = (DecoderHandler)ReadARTImage;
  entry->encoder = (EncoderHandler)WriteARTImage;
  entry->description="PFS: 1st Publisher";
  entry->module="ART";
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r A R T I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterARTImage removes format registrations made by the
%  ART module from the list of supported formats.
%
%  The format of the UnregisterARTImage method is:
%
%      UnregisterARTImage(void)
%
*/
ModuleExport void UnregisterARTImage(void)
{
  (void) UnregisterMagickInfo("ART");
}
