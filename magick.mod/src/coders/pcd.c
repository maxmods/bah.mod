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
%                            PPPP    CCCC  DDDD                               %
%                            P   P  C      D   D                              %
%                            PPPP   C      D   D                              %
%                            P      C      D   D                              %
%                            P       CCCC  DDDD                               %
%                                                                             %
%                                                                             %
%                     Read/Write Photo CD Image Format.                       %
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
#include "magick/decorate.h"
#include "magick/gem.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/montage.h"
#include "magick/resize.h"
#include "magick/shear.h"
#include "magick/transform.h"
#include "magick/utility.h"

/*
  Forward declarations.
*/
static unsigned int
  WritePCDImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U p s a m p l e                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Upsample() doubles the size of the image.
%
%  The format of the Upsample method is:
%
%      void Upsample(const unsigned long width,const unsigned long height,
%        const unsigned long scaled_width,unsigned char *pixels)
%
%  A description of each parameter follows:
%
%    o width,height:  Unsigned values representing the width and height of
%      the image pixel array.
%
%    o scaled_width:  Specifies the final width of the upsampled pixel array.
%
%    o pixels:  An unsigned char containing the pixel data.  On output the
%      upsampled pixels are returned here.
%
%
*/
static void Upsample(const unsigned long width,const unsigned long height,
                     const unsigned long scaled_width,unsigned char *pixels)
{
  register long
    x,
    y;

  register unsigned char
    *p,
    *q,
    *r;

  /*
    Create a new image that is a integral size greater than an existing one.
  */
  assert(pixels != (unsigned char *) NULL);
  for (y=0; y < (long) height; y++)
  {
    p=pixels+(height-1-y)*scaled_width+(width-1);
    q=pixels+((height-1-y) << 1)*scaled_width+((width-1) << 1);
    *q=(*p);
    *(q+1)=(*(p));
    for (x=1; x < (long) width; x++)
    {
      p--;
      q-=2;
      *q=(*p);
      *(q+1)=(unsigned char) ((((long) *p)+((long) *(p+1))+1) >> 1);
    }
  }
  for (y=0; y < (long) (height-1); y++)
  {
    p=pixels+(y << 1)*scaled_width;
    q=p+scaled_width;
    r=q+scaled_width;
    for (x=0; x < (long) (width-1); x++)
    {
      *q=(unsigned char) ((((long) *p)+((long) *r)+1) >> 1);
      *(q+1)=(unsigned char)
        ((((long) *p)+((long) *(p+2))+((long) *r)+((long) *(r+2))+2) >> 2);
      q+=2;
      p+=2;
      r+=2;
    }
    *q++=(unsigned char) ((((long) *p++)+((long) *r++)+1) >> 1);
    *q++=(unsigned char) ((((long) *p++)+((long) *r++)+1) >> 1);
  }
  p=pixels+(2*height-2)*scaled_width;
  q=pixels+(2*height-1)*scaled_width;
  (void) memcpy(q,p,2*width);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e c o d e I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DecodeImage recovers the Huffman encoded luminance and chrominance
%  deltas.
%
%  The format of the DecodeImage method is:
%
%      static unsigned int DecodeImage(Image *image,unsigned char *luma,
%        unsigned char *chroma1,unsigned char *chroma2)
%
%  A description of each parameter follows:
%
%    o status:  Method DecodeImage returns True if all the deltas are
%      recovered without error, otherwise False.
%
%    o image: The address of a structure of type Image.
%
%    o luma: The address of a character buffer that contains the
%      luminance information.
%
%    o chroma1: The address of a character buffer that contains the
%      chrominance information.
%
%    o chroma2: The address of a character buffer that contains the
%      chrominance information.
%
%
%
*/
#define IsSync  ((sum & 0xffffff00) == 0xfffffe00)
#define DecodeImageText  "[%s] PCD decode image..."
#define PCDGetBits(n) \
{  \
  sum=(sum << n) & 0xffffffff; \
  bits-=n; \
  while (bits <= 24) \
  { \
    if (p >= (buffer+0x800)) \
      { \
        (void) ReadBlob(image,0x800,(char *) buffer); \
        p=buffer; \
      } \
    sum|=((unsigned int) (*p) << (24-bits)); \
    bits+=8; \
    p++; \
  } \
  if (EOFBlob(image)) \
    break; \
}
static unsigned int DecodeImage(Image *image,unsigned char *luma,
  unsigned char *chroma1,unsigned char *chroma2)
{
  typedef struct PCDTable
  {
    unsigned int
      length,
      sequence;

    unsigned int
      mask;

    unsigned char
      key;
  } PCDTable;

  long
    count,
    quantum;

  PCDTable
    *pcd_table[3];

  register long
    i,
    j;

  register PCDTable
    *r;

  register unsigned char
    *p,
    *q;

  size_t
    length;

  unsigned char
    *buffer;

  unsigned int
    bits,
    plane,
    pcd_length[3],
    row,
    sum;

  /*
    Initialize Huffman tables.
  */
  assert(image != (const Image *) NULL);
  assert(luma != (unsigned char *) NULL);
  assert(chroma1 != (unsigned char *) NULL);
  assert(chroma2 != (unsigned char *) NULL);
  buffer=MagickAllocateMemory(unsigned char *,0x800);
  if (buffer == (unsigned char *) NULL)
    ThrowBinaryException(ResourceLimitError,MemoryAllocationFailed,
      (char *) NULL);
  sum=0;
  bits=32;
  p=buffer+0x800;
  for (i=0; i < (image->columns > 1536 ? 3 : 1); i++)
  {
    PCDGetBits(8);
    length=(sum & 0xff)+1;
    pcd_table[i]=MagickAllocateMemory(PCDTable *,length*sizeof(PCDTable));
    if (pcd_table[i] == (PCDTable *) NULL)
      {
        MagickFreeMemory(buffer);
        ThrowBinaryException(ResourceLimitError,MemoryAllocationFailed,
          (char *) NULL)
      }
    r=pcd_table[i];
    for (j=0; j < (long) length; j++)
    {
      PCDGetBits(8);
      r->length=(sum & 0xff)+1;
      if (r->length > 16)
        {
          MagickFreeMemory(buffer);
          return(False);
        }
      PCDGetBits(16);
      r->sequence=(sum & 0xffff) << 16;
      PCDGetBits(8);
      r->key=(unsigned char) (sum & 0xff);
      r->mask=(~((((unsigned int) 1) << (32-r->length))-1));
      r++;
    }
    pcd_length[i]=length;
  }
  /*
    Search for Sync byte.
  */
  for (i=0; i < 1; i++)
    PCDGetBits(16);
  for (i=0; i < 1; i++)
    PCDGetBits(16);
  while ((sum & 0x00fff000) != 0x00fff000)
    PCDGetBits(8);
  while (!IsSync)
    PCDGetBits(1);
  /*
    Recover the Huffman encoded luminance and chrominance deltas.
  */
  count=0;
  length=0;
  plane=0;
  row=0;
  q=luma;
  for ( ; ; )
  {
    if (IsSync)
      {
        /*
          Determine plane and row number.
        */
        PCDGetBits(16);
        row=((sum >> 9) & 0x1fff);
        if (row == image->rows)
          break;
        PCDGetBits(8);
        plane=sum >> 30;
        PCDGetBits(16);
        switch (plane)
        {
          case 0:
          {
            q=luma+row*image->columns;
            count=(long) image->columns;
            break;
          }
          case 2:
          {
            q=chroma1+(row >> 1)*image->columns;
            count=(long) (image->columns >> 1);
            plane--;
            break;
          }
          case 3:
          {
            q=chroma2+(row >> 1)*image->columns;
            count=(long) (image->columns >> 1);
            plane--;
            break;
          }
          default:
          {
            ThrowBinaryException(CorruptImageError,CorruptImage,
              image->filename)
          }
        }
        length=pcd_length[plane];
        if (QuantumTick(row,image->rows))
          if (!MagickMonitorFormatted(row,image->rows,&image->exception,
                                      DecodeImageText,image->filename))
            break;
        continue;
      }
    /*
      Decode luminance or chrominance deltas.
    */
    r=pcd_table[plane];
    for (i=0; ((i < (long) length) && ((sum & r->mask) != r->sequence)); i++)
      r++;
    if ((row > image->rows) || (r == (PCDTable *) NULL))
      {
        ThrowException(&image->exception,CorruptImageWarning,SkipToSyncByte,
          image->filename);
        while ((sum & 0x00fff000) != 0x00fff000)
          PCDGetBits(8);
        while (!IsSync)
          PCDGetBits(1);
        continue;
      }
    if (r->key < 128U)
      quantum=(long) (*q)+r->key;
    else
      quantum=(long) (*q)+r->key-256;
    *q=(unsigned char) ((quantum < 0L) ? 0U :
                        (quantum > 255L) ? 255U :
                        (unsigned char) quantum);
    q++;
    PCDGetBits(r->length);
    count--;
  }
  /*
    Free memory.
  */
  for (i=0; i < (image->columns > 1536 ? 3 : 1); i++)
    MagickFreeMemory(pcd_table[i]);
  MagickFreeMemory(buffer);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s P C D                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsPCD returns True if the image format type, identified by the
%  magick string, is PCD.
%
%  The format of the IsPCD method is:
%
%      unsigned int IsPCD(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsPCD returns True if the image format type is PCD.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsPCD(const unsigned char *magick,const size_t length)
{
  if (length < 2052)
    return(False);
  if (LocaleNCompare((char *) magick+2048,"PCD_",4) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d P C D I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPCDImage reads a Photo CD image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.  Much of the PCD decoder was derived from
%  the program hpcdtoppm(1) by Hadmut Danisch.
%
%  The format of the ReadPCDImage method is:
%
%      image=ReadPCDImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadPCDImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/

static Image *OverviewImage(const ImageInfo *image_info,Image *image,
  ExceptionInfo *exception)
{
  Image
    *montage_image;

  MontageInfo
    *montage_info;

  /*
    Label image tiles.
  */
  {
    Image
      *label_image;

    for( label_image=GetFirstImageInList(image); label_image != 0;
         label_image=GetNextImageInList(label_image) )
      (void) SetImageAttribute(label_image, "label", DefaultTileLabel);
  }

  /*
    Create the PCD Overview image.
  */
  montage_info=CloneMontageInfo(image_info,(MontageInfo *) NULL);
  (void) strlcpy(montage_info->filename,image_info->filename,MaxTextExtent);
  montage_image=MontageImages(image,montage_info,exception);
  DestroyMontageInfo(montage_info);
  if (montage_image == (Image *) NULL)
    ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
  DestroyImage(image);
  return(montage_image);
}

static Image *ReadPCDImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image
    *image;

  long
    x;

  ExtendedSignedIntegralType
    offset;

  register long
    y;

  register PixelPacket
    *q;

  register long
    i;

  register unsigned char
    *c1,
    *c2,
    *yy;

  size_t
    count;

  unsigned char
    *chroma1,
    *chroma2,
    *header,
    *luma;

  unsigned int
    overview,
    rotate,
    status;

  unsigned long
    height,
    number_images,
    number_pixels,
    subimage,
    width;

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
    Determine if this is a PCD file.
  */
  header=MagickAllocateMemory(unsigned char *,3*0x800);
  if (header == (unsigned char *) NULL)
    ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
  count=ReadBlob(image,3*0x800,(char *) header);
  overview=LocaleNCompare((char *) header,"PCD_OPA",7) == 0;
  if ((count == 0) ||
      ((LocaleNCompare((char *) header+0x800,"PCD",3) != 0) && !overview))
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
  rotate=header[0x0e02] & 0x03;
  number_images=(header[10] << 8) | header[11];
  MagickFreeMemory(header);
  /*
    Determine resolution by subimage specification.
  */
  number_pixels=image->columns*image->rows;
  if (number_pixels == 0)
    subimage=3;
  else
    {
      width=192;
      height=128;
      for (subimage=1; subimage < 6; subimage++)
      {
        if ((width >= image->columns) && (height >= image->rows))
          break;
        width<<=1;
        height<<=1;
      }
    }
  if (image_info->subrange != 0)
    subimage=Min(image_info->subimage,6);
  if (overview)
    subimage=1;
  /*
    Initialize image structure.
  */
  width=192;
  height=128;
  for (i=1; i < (long) Min(subimage,3); i++)
  {
    width<<=1;
    height<<=1;
  }
  image->columns=width;
  image->rows=height;
  image->depth=8;
  for ( ; i < (long) subimage; i++)
  {
    image->columns<<=1;
    image->rows<<=1;
  }
  /*
    Allocate luma and chroma memory.
  */
  number_pixels=image->columns*image->rows;
  chroma1=MagickAllocateMemory(unsigned char *,number_pixels+1);
  chroma2=MagickAllocateMemory(unsigned char *,number_pixels+1);
  luma=MagickAllocateMemory(unsigned char *,number_pixels+1);
  if ((chroma1 == (unsigned char *) NULL) ||
      (chroma2 == (unsigned char *) NULL) || (luma == (unsigned char *) NULL))
    ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
  /*
    Advance to image data.
  */
  offset=93;
  if (overview)
    offset=2;
  else
    if (subimage == 2)
      offset=20;
    else
      if (subimage <= 1)
        offset=1;
  for (i=0; i < (long) (offset*0x800); i++)
    (void) ReadBlobByte(image);
  if (overview)
    {
      Image
        *overview_image;

      MonitorHandler
        handler;

      register long
        j;

      /*
        Read thumbnails from overview image.
      */
      for (j=1; j <= (long) number_images; j++)
      {
        handler=SetMonitorHandler((MonitorHandler) NULL);
        FormatString(image->filename,"images/img%04ld.pcd",j);
        FormatString(image->magick_filename,"images/img%04ld.pcd",j);
        image->scene=j;
        image->columns=width;
        image->rows=height;
        image->depth=8;
        yy=luma;
        c1=chroma1;
        c2=chroma2;
        for (y=0; y < (long) height; y+=2)
        {
          (void) ReadBlob(image,width,(char *) yy);
          yy+=image->columns;
          (void) ReadBlob(image,width,(char *) yy);
          yy+=image->columns;
          (void) ReadBlob(image,width >> 1,(char *) c1);
          c1+=image->columns;
          (void) ReadBlob(image,width >> 1,(char *) c2);
          c2+=image->columns;
        }
        Upsample(image->columns >> 1,image->rows >> 1,image->columns,chroma1);
        Upsample(image->columns >> 1,image->rows >> 1,image->columns,chroma2);
        /*
          Transfer luminance and chrominance channels.
        */
        yy=luma;
        c1=chroma1;
        c2=chroma2;
        for (y=0; y < (long) image->rows; y++)
        {
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          for (x=0; x < (long) image->columns; x++)
          {
            q->red=ScaleCharToQuantum(*yy++);
            q->green=ScaleCharToQuantum(*c1++);
            q->blue=ScaleCharToQuantum(*c2++);
            q++;
          }
          if (!SyncImagePixels(image))
            break;
        }
        if (LocaleCompare(image_info->magick,"PCDS") == 0)
          image->colorspace=sRGBColorspace;
        else
          image->colorspace=YCCColorspace;
        (void) TransformColorspace(image,RGBColorspace);
        if (j < (long) number_images)
          {
            /*
              Allocate next image structure.
            */
            AllocateNextImage(image_info,image);
            if (image->next == (Image *) NULL)
              {
                DestroyImageList(image);
                return((Image *) NULL);
              }
            image=SyncNextImageInList(image);
          }
        (void) SetMonitorHandler(handler);
        if (!MagickMonitorFormatted(j-1,number_images,&image->exception,
                                    LoadImageText,image->filename))
          break;
      }
      MagickFreeMemory(chroma2);
      MagickFreeMemory(chroma1);
      MagickFreeMemory(luma);
      while (image->previous != (Image *) NULL)
        image=image->previous;
      overview_image=OverviewImage(image_info,image,exception);
      return(overview_image);
    }
  /*
    Read interleaved image.
  */
  yy=luma;
  c1=chroma1;
  c2=chroma2;
  for (y=0; y < (long) height; y+=2)
  {
    (void) ReadBlob(image,width,(char *) yy);
    yy+=image->columns;
    (void) ReadBlob(image,width,(char *) yy);
    yy+=image->columns;
    (void) ReadBlob(image,width >> 1,(char *) c1);
    c1+=image->columns;
    (void) ReadBlob(image,width >> 1,(char *) c2);
    c2+=image->columns;
  }
  if (subimage >= 4)
    {
      /*
        Recover luminance deltas for 1536x1024 image.
      */
      Upsample(768,512,image->columns,luma);
      Upsample(384,256,image->columns,chroma1);
      Upsample(384,256,image->columns,chroma2);
      image->rows=1024;
      for (i=0; i < (4*0x800); i++)
        (void) ReadBlobByte(image);
      status=DecodeImage(image,luma,chroma1,chroma2);
      if ((subimage >= 5) && status)
        {
          /*
            Recover luminance deltas for 3072x2048 image.
          */
          Upsample(1536,1024,image->columns,luma);
          Upsample(768,512,image->columns,chroma1);
          Upsample(768,512,image->columns,chroma2);
          image->rows=2048;
          offset=TellBlob(image)/0x800+12;
          (void) SeekBlob(image,offset*0x800,SEEK_SET);
          status=DecodeImage(image,luma,chroma1,chroma2);
          if ((subimage >= 6) && status)
            {
              /*
                Recover luminance deltas for 6144x4096 image (vaporware).
              */
              Upsample(3072,2048,image->columns,luma);
              Upsample(1536,1024,image->columns,chroma1);
              Upsample(1536,1024,image->columns,chroma2);
              image->rows=4096;
            }
        }
    }
  Upsample(image->columns >> 1,image->rows >> 1,image->columns,chroma1);
  Upsample(image->columns >> 1,image->rows >> 1,image->columns,chroma2);
  /*
    Transfer luminance and chrominance channels.
  */
  yy=luma;
  c1=chroma1;
  c2=chroma2;
  for (y=0; y < (long) image->rows; y++)
  {
    q=SetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      q->red=ScaleCharToQuantum(*yy++);
      q->green=ScaleCharToQuantum(*c1++);
      q->blue=ScaleCharToQuantum(*c2++);
      q++;
    }
    if (!SyncImagePixels(image))
      break;
    if (QuantumTick(y,image->rows))
      if (!MagickMonitorFormatted(y,image->rows,exception,LoadImageText,
                                  image->filename))
        break;
  }
  MagickFreeMemory(chroma2);
  MagickFreeMemory(chroma1);
  MagickFreeMemory(luma);
  if (LocaleCompare(image_info->magick,"PCDS") == 0)
    image->colorspace=sRGBColorspace;
  else
    image->colorspace=YCCColorspace;
  /* FIXME: YCCColorspace transform is broken! 1.1 is ok! */
  (void) TransformColorspace(image,RGBColorspace);
  if (EOFBlob(image))
    ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,
      image->filename);
  CloseBlob(image);

  if ((rotate == 1) || (rotate == 3))
    {
      double
        degrees;

      Image
        *rotated_image;

      /*
        Rotate image.
      */
      degrees=rotate == 1 ? -90.0 : 90.0;
      rotated_image=RotateImage(image,degrees,exception);
      if (rotated_image != (Image *) NULL)
        {
          DestroyImage(image);
          image=rotated_image;
        }
    }

  /*
    Set CCIR 709 primaries with a D65 white point.
  */
  image->chromaticity.red_primary.x=0.6400f;
  image->chromaticity.red_primary.y=0.3300f;
  image->chromaticity.green_primary.x=0.3000f;
  image->chromaticity.green_primary.y=0.6000f;
  image->chromaticity.blue_primary.x=0.1500f;
  image->chromaticity.blue_primary.y=0.0600f;
  image->chromaticity.white_point.x=0.3127f;
  image->chromaticity.white_point.y=0.3290f;
  image->gamma=1.000f/2.200f;
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r P C D I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterPCDImage adds attributes for the PCD image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterPCDImage method is:
%
%      RegisterPCDImage(void)
%
*/
ModuleExport void RegisterPCDImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("PCD");
  entry->decoder=(DecoderHandler) ReadPCDImage;
  entry->encoder=(EncoderHandler) WritePCDImage;
  entry->magick=(MagickHandler) IsPCD;
  entry->adjoin=False;
  entry->description="Photo CD";
  entry->module="PCD";
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("PCDS");
  entry->decoder=(DecoderHandler) ReadPCDImage;
  entry->encoder=(EncoderHandler) WritePCDImage;
  entry->adjoin=False;
  entry->description="Photo CD";
  entry->module="PCD";
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r P C D I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterPCDImage removes format registrations made by the
%  PCD module from the list of supported formats.
%
%  The format of the UnregisterPCDImage method is:
%
%      UnregisterPCDImage(void)
%
*/
ModuleExport void UnregisterPCDImage(void)
{
  (void) UnregisterMagickInfo("PCD");
  (void) UnregisterMagickInfo("PCDS");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P C D I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePCDImage writes an image in the Photo CD encoded image
%  format.
%
%  The format of the WritePCDImage method is:
%
%      unsigned int WritePCDImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WritePCDImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/

static unsigned int WritePCDTile(const ImageInfo *ARGUNUSED(image_info),
  Image *image,char *page_geometry,char *tile_geometry)
{
  Image
    *downsample_image,
    *tile_image;

  long
    y;

  RectangleInfo
    geometry;

  register const PixelPacket
    *p,
    *q;

  register long
    i,
    x;

  /*
    Scale image to tile size.
  */
  SetGeometry(image,&geometry);
  (void) GetMagickGeometry(page_geometry,&geometry.x,&geometry.y,
    &geometry.width,&geometry.height);
  if ((geometry.width % 2) != 0)
    geometry.width--;
  if ((geometry.height % 2) != 0)
    geometry.height--;
  tile_image=ResizeImage(image,geometry.width,geometry.height,TriangleFilter,
    1.0,&image->exception);
  if (tile_image == (Image *) NULL)
    return(False);
  (void) sscanf(page_geometry,"%lux%lu",&geometry.width,&geometry.height);
  if ((tile_image->columns != geometry.width) ||
      (tile_image->rows != geometry.height))
    {
      Image
        *bordered_image;

      RectangleInfo
        border_info;

      /*
        Put a border around the image.
      */
      border_info.width=(geometry.width-tile_image->columns+1) >> 1;
      border_info.height=(geometry.height-tile_image->rows+1) >> 1;
      bordered_image=BorderImage(tile_image,&border_info,&image->exception);
      if (bordered_image == (Image *) NULL)
        return(False);
      DestroyImage(tile_image);
      tile_image=bordered_image;
    }
  (void) TransformImage(&tile_image,(char *) NULL,tile_geometry);
  (void) TransformColorspace(tile_image,YCCColorspace);
  downsample_image=ResizeImage(tile_image,tile_image->columns/2,
    tile_image->rows/2,TriangleFilter,1.0,&image->exception);
  if (downsample_image == (Image *) NULL)
    return(False);
  /*
    Write tile to PCD file.
  */
  for (y=0; y < (long) tile_image->rows; y+=2)
  {
    p=AcquireImagePixels(tile_image,0,y,tile_image->columns,2,
      &tile_image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    for (x=0; x < (long) (tile_image->columns << 1); x++)
    {
      (void) WriteBlobByte(image,ScaleQuantumToChar(p->red));
      p++;
    }
    q=AcquireImagePixels(downsample_image,0,y >> 1,downsample_image->columns,
      1,&downsample_image->exception);
    if (q == (const PixelPacket *) NULL)
      break;
    for (x=0; x < (long) downsample_image->columns; x++)
    {
      (void) WriteBlobByte(image,ScaleQuantumToChar(q->green));
      q++;
    }
    q=AcquireImagePixels(downsample_image,0,y >> 1,downsample_image->columns,
      1,&downsample_image->exception);
    if (q == (const PixelPacket *) NULL)
      break;
    for (x=0; x < (long) downsample_image->columns; x++)
    {
      (void) WriteBlobByte(image,ScaleQuantumToChar(q->blue));
      q++;
    }
    if (QuantumTick(y,tile_image->rows))
      if (!MagickMonitorFormatted(y,tile_image->rows,&image->exception,
                                  SaveImageText,image->filename))
        break;
  }
  for (i=0; i < 0x800; i++)
    (void) WriteBlobByte(image,'\0');
  DestroyImage(downsample_image);
  DestroyImage(tile_image);
  return(True);
}

static unsigned int WritePCDImage(const ImageInfo *image_info,Image *image)
{
  Image
    *pcd_image;

  register long
    i;

  unsigned int
    status;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  pcd_image=image;
  if (image->columns < image->rows)
    {
      Image
        *rotated_image;

      /*
        Rotate portrait to landscape.
      */
      rotated_image=RotateImage(image,90.0,&image->exception);
      if (rotated_image == (Image *) NULL)
        return(False);
      pcd_image=rotated_image;
    }
  /*
    Open output image file.
  */
  status=OpenBlob(image_info,pcd_image,WriteBinaryBlobMode,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenError,UnableToOpenFile,pcd_image);
  (void) TransformColorspace(pcd_image,RGBColorspace);
  /*
    Write PCD image header.
  */
  for (i=0; i < 32; i++)
    (void) WriteBlobByte(pcd_image,0xff);
  for (i=0; i < 4; i++)
    (void) WriteBlobByte(pcd_image,0x0e);
  for (i=0; i < 8; i++)
    (void) WriteBlobByte(pcd_image,'\0');
  for (i=0; i < 4; i++)
    (void) WriteBlobByte(pcd_image,0x01);
  for (i=0; i < 4; i++)
    (void) WriteBlobByte(pcd_image,0x05);
  for (i=0; i < 8; i++)
    (void) WriteBlobByte(pcd_image,'\0');
  for (i=0; i < 4; i++)
    (void) WriteBlobByte(pcd_image,0x0A);
  for (i=0; i < 36; i++)
    (void) WriteBlobByte(pcd_image,'\0');
  for (i=0; i < 4; i++)
    (void) WriteBlobByte(pcd_image,0x01);
  for (i=0; i < 1944; i++)
    (void) WriteBlobByte(pcd_image,'\0');
  (void) WriteBlob(pcd_image,7,"PCD_IPI");
  (void) WriteBlobByte(pcd_image,0x06);
  for (i=0; i < 1530; i++)
    (void) WriteBlobByte(pcd_image,'\0');
  if (image->columns < image->rows)
    (void) WriteBlobByte(pcd_image,'\1');
  else
    (void) WriteBlobByte(pcd_image,'\0');
  for (i=0; i < (3*0x800-1539); i++)
    (void) WriteBlobByte(pcd_image,'\0');
  /*
    Write PCD tiles.
  */
  status=WritePCDTile(image_info,pcd_image,(char *) "768x512>",
    (char *) "192x128");
  status|=WritePCDTile(image_info,pcd_image,(char *) "768x512>",
    (char *) "384x256");
  status|=WritePCDTile(image_info,pcd_image,(char *) "768x512>",
    (char *) "768x512");
  CloseBlob(pcd_image);
  if (pcd_image != image)
    DestroyImage(pcd_image);
  return(status);
}
