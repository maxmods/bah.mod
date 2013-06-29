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
%                            M   M   AAA    CCC                               %
%                            MM MM  A   A  C                                  %
%                            M M M  AAAAA  C                                  %
%                            M   M  A   A  C                                  %
%                            M   M  A   A   CCC       			      %                              %
%                                                                             %
%                Read MAC: MacPaint Image Format.                                     %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                              Jaroslav Fojtik                                %
%                                    2010                                     %
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
#include "magick/colormap.h"
#include "magick/constitute.h"
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/pixel_cache.h"
#include "magick/utility.h"


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d M A C I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadMACImage reads an MAC image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadMACImage method is:
%
%      Image *ReadMACImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadMACImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadMACImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image *image;
  unsigned int y;  
  unsigned char x8, rep, b;
  long ldblk;
  unsigned char *BImgBuff = NULL;
  unsigned char *DataPtr;
  unsigned int status;
  const PixelPacket *q;

	/* Open image file. */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image = AllocateImage(image_info);
  status = OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if(status == False)
    ThrowReaderException(FileOpenError,UnableToOpenFile,image);

	/* Read MAC image. */
  ldblk = ReadBlobLSBShort(image);
  if((ldblk & 0xFF)!=0)
	ThrowReaderException(CorruptImageError,ImproperImageHeader,image);

  if(ldblk==0)		/* ???? don't know why */
	SeekBlob(image,0x200,SEEK_SET);
  else
	SeekBlob(image,0x280,SEEK_SET);

  image->columns = 576;
  image->rows = 720;
  image->depth = 1;
  image->colors = 1l << image->depth;

  if (!AllocateImageColormap(image,image->colors)) goto NoMemory;

    /* If ping is true, then only set image size and colors without reading any image data. */
  if (image_info->ping) goto DONE_READING;

  /* ----- Load RLE compressed raster ----- */
  ldblk = (image->depth*image->columns) /8;
  BImgBuff = MagickAllocateMemory(unsigned char *, ((size_t)ldblk));
  if(BImgBuff==NULL)
    NoMemory:
      ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
  
  DataPtr = BImgBuff;  
  x8=0; y=0;

  while(y<image->rows)
  {
    rep = ReadBlobByte(image);
    if(EOFBlob(image)) break;

    if( rep>=128 || rep<=0)
    {
      b = ~ReadBlobByte(image);;

      rep = ~rep + 2;
      while(rep>0)
      {
        *DataPtr++ = b;
        x8++;
        rep--;
        if(x8>=ldblk)
	{
	  x8=0;

          q = SetImagePixels(image,0,y,image->columns,1);
          if(q == (PixelPacket *)NULL) break;
          (void)ImportImagePixelArea(image,GrayQuantum,1,BImgBuff,NULL,0);
          if(!SyncImagePixels(image)) break;

	  DataPtr = BImgBuff;
	  y++;
	  if(y>=image->rows)
	  {
	    break;	    
	  }
	}
      }
    }
    else
    {
      rep++;
      while(rep>0)
      {
        b = ~ReadBlobByte(image);
        *DataPtr++ = b;
        x8++;
        rep--;
        if(x8>=ldblk)
        {
 	  x8=0;

          q = SetImagePixels(image,0,y,image->columns,1);
          if(q == (PixelPacket *)NULL) break;
          (void)ImportImagePixelArea(image,GrayQuantum,1,BImgBuff,NULL,0);
          if (!SyncImagePixels(image)) break;

	  DataPtr = BImgBuff;
	  y++;
	  if(y>=image->rows)
	  {
	    break;
	  }
        }
      }
    }
  }
  if(BImgBuff!=NULL)
    MagickFreeMemory(BImgBuff);
  if(EOFBlob(image))
    ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,image->filename);

DONE_READING:
  CloseBlob(image);
  return(image);
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r M A C I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterMACImage adds attributes for the MAC image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterMACImage method is:
%
%      RegisterMACImage(void)
*/
ModuleExport void RegisterMACImage(void)
{
  MagickInfo *entry;

  entry=SetMagickInfo("MAC");
  entry->decoder = (DecoderHandler)ReadMACImage;
  entry->description="Mac Paint";
  entry->seekable_stream = True;
  entry->module="MAC";
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r M A C I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterMACImage removes format registrations made by the
%  MAC module from the list of supported formats.
%
%  The format of the UnregisterMACImage method is:
%
%      UnregisterMACImage(void)
%
*/
ModuleExport void UnregisterMACImage(void)
{
  (void) UnregisterMagickInfo("MAC");
}
