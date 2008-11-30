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
%                                                                             %
%                              JJJ  PPPP    222                               %
%                               J   P   P  2   2                              %
%                               J   PPPP     22                               %
%                            J  J   P       2                                 %
%                             JJ    P      22222                              %
%                                                                             %
%                                                                             %
%                    Read/Write JPEG-2000 Image Format.                       %
%                                                                             %
%                                                                             %
%                                John Cristy                                  %
%                                Nathan Brown                                 %
%                                 June 2001                                   %
%                              Bob Friesenhahn                                %
%                               February 2003                                 %
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
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/profile.h"
#include "magick/utility.h"
#if defined(HasJP2)
#if !defined(uchar)
#define uchar  unsigned char
#endif
#if !defined(ushort)
#define ushort  unsigned short
#endif
#if !defined(uint)
#define uint  unsigned int
#endif
#if !defined(longlong)
#define longlong  long long
#endif
#if !defined(ulonglong)
#define ulonglong  unsigned long long
#endif

#ifdef __VMS
#define JAS_VERSION 1.700.0
#define PACKAGE jasper
#define VERSION 1.700.0
#endif
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#include "jasper/jasper.h"
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#endif

/*
  Forward declarations.
*/
#if defined(HasJP2)
static unsigned int
  WriteJP2Image(const ImageInfo *,Image *);
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s J P 2                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsJP2 returns True if the image format type, identified by the
%  magick string, is JP2.
%
%  The format of the IsJP2 method is:
%
%      unsigned int IsJP2(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsJP2 returns True if the image format type is JP2.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsJP2(const unsigned char *magick,const size_t length)
{
  if (length < 9)
    return(False);
  if (memcmp(magick+4,"\152\120\040\040\015",5) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s J P C                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsJPC returns True if the image format type, identified by the
%  magick string, is JPC.
%
%  The format of the IsJPC method is:
%
%      unsigned int IsJPC(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsJPC returns True if the image format type is JPC.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsJPC(const unsigned char *magick,const size_t length)
{
  if (length < 2)
    return(False);
  if (memcmp(magick,"\377\117",2) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d J P 2 I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadJP2Image reads a JPEG 2000 Image file (JP2) or JPEG 2000
%  codestream (JPC) image file and returns it.  It allocates the memory
%  necessary for the new Image structure and returns a pointer to the new
%  image or set of images.
%
%  JP2 support is originally written by Nathan Brown, nathanbrown@letu.edu.
%
%  The format of the ReadJP2Image method is:
%
%      Image *ReadJP2Image(const ImageInfo *image_info,
%                          ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadJP2Image returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
*/
#if defined(HasJP2)

typedef struct _StreamManager
{
  jas_stream_t
    *stream;

  Image
    *image;
} StreamManager;

static int BlobRead(jas_stream_obj_t *object,char *buffer,const int length)
{
  size_t
    count;

  StreamManager
    *source = (StreamManager *) object;

  count=ReadBlob(source->image,(size_t) length,(void *) buffer);
  return ((int) count);
}

static int BlobWrite(jas_stream_obj_t *object,char *buffer,const int length)
{
  size_t
    count;

  StreamManager
    *source = (StreamManager *) object;

  count=WriteBlob(source->image,(size_t) length,(void *) buffer);
  return((int) count);
}

static long BlobSeek(jas_stream_obj_t *object,long offset,int origin)
{
  StreamManager
    *source = (StreamManager *) object;

  return (SeekBlob(source->image,offset,origin));
}

static int BlobClose(jas_stream_obj_t *object)
{
  StreamManager
    *source = (StreamManager *) object;

  CloseBlob(source->image);
  MagickFreeMemory(source);
  return (0);
}

static jas_stream_ops_t
  StreamOperators =
  {
    BlobRead,
    BlobWrite,
    BlobSeek,
    BlobClose
  };

static jas_stream_t *JP2StreamManager(Image *image)
{
  jas_stream_t 
    *stream;

  StreamManager
    *source;

  stream=MagickAllocateMemory(jas_stream_t *,sizeof(jas_stream_t));
  if (stream == (jas_stream_t *) NULL)
    return((jas_stream_t *) NULL);
  (void) memset(stream,0,sizeof(jas_stream_t));
  stream->rwlimit_=(-1);
  stream->obj_=MagickAllocateMemory(jas_stream_obj_t *,sizeof(StreamManager));
  if (stream->obj_ == (jas_stream_obj_t *) NULL)
    return((jas_stream_t *) NULL);
  stream->ops_=(&StreamOperators);
  stream->openmode_=JAS_STREAM_READ | JAS_STREAM_WRITE | JAS_STREAM_BINARY;
  stream->bufbase_=stream->tinybuf_;
  stream->bufsize_=1;
  stream->bufstart_=(&stream->bufbase_[JAS_STREAM_MAXPUTBACK]);
  stream->ptr_=stream->bufstart_;
  stream->bufmode_|=JAS_STREAM_UNBUF & JAS_STREAM_BUFMODEMASK;
  source=(StreamManager *) stream->obj_;
  source->image=image;
  return(stream);
}

static Image *ReadJP2Image(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *image;

  long
    y;

  jas_image_t
    *jp2_image;

  jas_matrix_t
    *pixels[4];

  jas_stream_t
    *jp2_stream;

  register long
    i,
    x;

  register PixelPacket
    *q;

  int
    components[4];

  unsigned int
    channel_scale[4],
    maximum_component_depth,
    status;

  unsigned long
    number_components;

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
    Obtain a JP2 Stream.
  */
  jp2_stream=JP2StreamManager(image);
  if (jp2_stream == (jas_stream_t *) NULL)
    ThrowReaderException(DelegateError,UnableToManageJP2Stream,image);
  jp2_image=jas_image_decode(jp2_stream,-1,0);
  if (jp2_image == (jas_image_t *) NULL)
    {
      (void) jas_stream_close(jp2_stream);
      ThrowReaderException(DelegateError,UnableToDecodeImageFile,image);
    }

  /*
    Validate that we can handle the image and obtain component
    indexes.
  */
  switch (jas_clrspc_fam(jas_image_clrspc(jp2_image)))
    {
    case JAS_CLRSPC_FAM_RGB:
      {
        if (((components[0]=jas_image_getcmptbytype(jp2_image,
                                                    JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_R))) < 0) ||
            ((components[1]=jas_image_getcmptbytype(jp2_image,
                                                    JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_G))) < 0) ||
            ((components[2]=jas_image_getcmptbytype(jp2_image,
                                                    JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_B))) < 0))
          {
            (void) jas_stream_close(jp2_stream);
            jas_image_destroy(jp2_image);
            ThrowReaderException(CorruptImageError,MissingImageChannel,image);
          }
        number_components=3;
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "Image is in RGB colorspace family");
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "RED is in channel %d, GREEN is in channel %d, BLUE is in channel %d",
                              components[0],components[1],components[2]);

        if((components[3]=jas_image_getcmptbytype(jp2_image,
                                                  JAS_IMAGE_CT_COLOR(JAS_IMAGE_CT_OPACITY))) > 0)
          {
            image->matte=True;
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "OPACITY is in channel %d",components[3]);
            number_components++;
          }
        break;
      }
    case JAS_CLRSPC_FAM_GRAY:
      {
        if ((components[0]=jas_image_getcmptbytype(jp2_image,
                                                   JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_GRAY_Y))) < 0)
          {
            (void) jas_stream_close(jp2_stream);
            jas_image_destroy(jp2_image);
            ThrowReaderException(CorruptImageError,MissingImageChannel,image);
          }
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "Image is in GRAY colorspace family");
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "GRAY is in channel %d",components[0]);
        number_components=1;
        break;
      }
    case JAS_CLRSPC_FAM_YCBCR:
      {
        components[0]=jas_image_getcmptbytype(jp2_image,JAS_IMAGE_CT_YCBCR_Y);
        components[1]=jas_image_getcmptbytype(jp2_image,JAS_IMAGE_CT_YCBCR_CB);
        components[2]=jas_image_getcmptbytype(jp2_image,JAS_IMAGE_CT_YCBCR_CR);
        if ((components[0] < 0) || (components[1] < 0) || (components[2] < 0))
          {
            (void) jas_stream_close(jp2_stream);
            jas_image_destroy(jp2_image);
            ThrowReaderException(CorruptImageError,MissingImageChannel,image);
          }
        number_components=3;
        components[3]=jas_image_getcmptbytype(jp2_image,JAS_IMAGE_CT_OPACITY);
        if (components[3] > 0)
          {
            image->matte=True;
            number_components++;
          }
        image->colorspace=YCbCrColorspace;
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "Image is in YCBCR colorspace family");
        break;
      }
    default:
      {
        (void) jas_stream_close(jp2_stream);
        jas_image_destroy(jp2_image);
        ThrowReaderException(CoderError,ColorspaceModelIsNotSupported,image);
      }
    }
  image->columns=jas_image_width(jp2_image);
  image->rows=jas_image_height(jp2_image);
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "columns=%lu rows=%lu components=%lu",image->columns,image->rows,
                        number_components);
  for (i=0; i < (long) number_components; i++)
    {
      if(((unsigned long) jas_image_cmptwidth(jp2_image,components[i]) != image->columns) ||
         ((unsigned long) jas_image_cmptheight(jp2_image,components[i]) != image->rows) ||
         (jas_image_cmpttlx(jp2_image, components[i]) != 0) ||
         (jas_image_cmpttly(jp2_image, components[i]) != 0) ||
         (jas_image_cmpthstep(jp2_image, components[i]) != 1) ||
         (jas_image_cmptvstep(jp2_image, components[i]) != 1) ||
         (jas_image_cmptsgnd(jp2_image, components[i]) != false))
        {
          (void) jas_stream_close(jp2_stream);
          jas_image_destroy(jp2_image);
          ThrowReaderException(CoderError,IrregularChannelGeometryNotSupported,image);
        }
    }
  /*
    Convert JPEG 2000 pixels.
  */
  if (image_info->ping)
    {
      (void) jas_stream_close(jp2_stream);
      jas_image_destroy(jp2_image);
      return(image);
    }
  image->matte=number_components > 3;
  maximum_component_depth=0;
  for (i=0; i < (long) number_components; i++)
    {
      maximum_component_depth=
        Max((unsigned int) jas_image_cmptprec(jp2_image,components[i]),maximum_component_depth);

      pixels[i]=jas_matrix_create(1,(unsigned int) image->columns);
      if (pixels[i] == (jas_matrix_t *) NULL)
        {
          jas_image_destroy(jp2_image);
          ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image)
            }
    }
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "Maximum component depth is %u",maximum_component_depth);
  /*
    Image depth is limited to maximum component depth rounded up to
    modulo 8 or QuantumDepth, which ever is smaller.
  */
  if (maximum_component_depth <= 8)
    image->depth=Min(8,QuantumDepth);
  else
    image->depth=Min(16,QuantumDepth);
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "Image depth is %u",image->depth);
  /*
    Channel depth may not be the same as image depth, and may not be
    modulo-8, so calculate per-channel scaling factors to normalize
    to a short.  FIXME: should scale to 8 or 16 bits, whichever is
    most efficient for the quantum depth.
  */
  for (i=0; i < (long) number_components; i++)
    {
      channel_scale[i]=1;
      if (jas_image_cmptprec(jp2_image,components[i]) < 16)
        channel_scale[i]=(1 << (16 - jas_image_cmptprec(jp2_image,
                                                        components[i])))+1;
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "Channel %ld scale is %u", i, channel_scale[i]);
    }

  for (y=0; y < (long) image->rows; y++)
    {
      q=GetImagePixels(image,0,y,image->columns,1);
      if (q == (PixelPacket *) NULL)
        break;
      for (i=0; i < (long) number_components; i++)
        (void) jas_image_readcmpt(jp2_image,(short) components[i],0,
                                  (unsigned int) y,(unsigned int) image->columns,1,pixels[i]);

      switch (number_components)
        {
        case 1:
          { /* Grayscale */
            for (x=0; x < (long) image->columns; x++)
              {
                q->red=q->green=q->blue=ScaleShortToQuantum((unsigned short)
                                                            jas_matrix_getv(pixels[0],x)*channel_scale[0]);
                q->opacity=OpaqueOpacity;
                q++;
              }
            break;
          }
        case 3:
          { /* RGB */
            for (x=0; x < (long) image->columns; x++)
              {
                q->red=ScaleShortToQuantum((unsigned short)
                                           jas_matrix_getv(pixels[0],x)*channel_scale[0]);
                q->green=ScaleShortToQuantum((unsigned short)
                                             jas_matrix_getv(pixels[1],x)*channel_scale[1]);
                q->blue=ScaleShortToQuantum((unsigned short)
                                            jas_matrix_getv(pixels[2],x)*channel_scale[2]);
                q->opacity=OpaqueOpacity;
                q++;
              }
            break;
          }
        case 4:
          { /* RGBA */
            for (x=0; x < (long) image->columns; x++)
              {
                q->red=ScaleShortToQuantum((unsigned short)
                                           jas_matrix_getv(pixels[0],x)*channel_scale[0]);
                q->green=ScaleShortToQuantum((unsigned short)
                                             jas_matrix_getv(pixels[1],x)*channel_scale[1]);
                q->blue=ScaleShortToQuantum((unsigned short)
                                            jas_matrix_getv(pixels[2],x)*channel_scale[2]);
                q->opacity=MaxRGB-ScaleShortToQuantum((unsigned short)
                                                      jas_matrix_getv(pixels[3],x)*channel_scale[3]);
                q++;
              }
            break;
          }
        }
      if (!SyncImagePixels(image))
        break;
      if (image->previous == (Image *) NULL)
        if (QuantumTick(y,image->rows))
          if (!MagickMonitorFormatted(y,image->rows,exception,LoadImageText,
                                      image->filename))
            break;
    }
  if (number_components == 1)
    image->is_grayscale=MagickTrue;
  {
    /*
      Obtain ICC ICM color profile
    */
    
    jas_cmprof_t
      *cm_profile;
    
    /* Obtain a pointer to the existing jas_cmprof_t profile handle. */
    cm_profile=jas_image_cmprof(jp2_image);
    if (cm_profile != (jas_cmprof_t *) NULL)
      {
        jas_iccprof_t
          *icc_profile;

        /* Obtain a copy of the jas_iccprof_t ICC profile handle */
        icc_profile=jas_iccprof_createfromcmprof(cm_profile);
        /* or maybe just icc_profile=cm_profile->iccprof */
        if (icc_profile != (jas_iccprof_t *) NULL)
          {
            jas_stream_t
              *icc_stream;

            icc_stream=jas_stream_memopen(NULL,0);
            if ((icc_stream != (jas_stream_t *) NULL) &&
                (jas_iccprof_save(icc_profile,icc_stream) == 0) &&
                (jas_stream_flush(icc_stream) == 0))
              {
                jas_stream_memobj_t
                  *blob;
                
                blob=(jas_stream_memobj_t *) icc_stream->obj_;
                if (image->logging)
                  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                        "ICC profile: %lu bytes",(unsigned long) blob->len_);
                SetImageProfile(image,"ICM",blob->buf_,blob->len_);
                
                (void) jas_stream_close(icc_stream);
                jas_iccprof_destroy(icc_profile);
              }
          }
      }
  }

  for (i=0; i < (long) number_components; i++)
    jas_matrix_destroy(pixels[i]);
  (void) jas_stream_close(jp2_stream);
  jas_image_destroy(jp2_image);
  return(image);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r J P 2 I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterJP2Image adds attributes for the JP2 image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterJP2Image method is:
%
%      RegisterJP2Image(void)
%
*/
ModuleExport void RegisterJP2Image(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("JP2");
  entry->description="JPEG-2000 JP2 File Format Syntax";
  entry->module="JP2";
  entry->magick=(MagickHandler) IsJP2;
  entry->adjoin=False;
  entry->seekable_stream=True;
  entry->thread_support=False;
#if defined(HasJP2)
  entry->decoder=(DecoderHandler) ReadJP2Image;
  entry->encoder=(EncoderHandler) WriteJP2Image;
#endif
  entry->coder_class=StableCoderClass;
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("JPC");
  entry->description="JPEG-2000 Code Stream Syntax";
  entry->module="JPC";
  entry->magick=(MagickHandler) IsJPC;
  entry->adjoin=False;
  entry->seekable_stream=True;
  entry->thread_support=False;
#if defined(HasJP2)
  entry->decoder=(DecoderHandler) ReadJP2Image;
  entry->encoder=(EncoderHandler) WriteJP2Image;
#endif
  entry->coder_class=StableCoderClass;
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("PGX");
  entry->description="JPEG-2000 VM Format";
  entry->module="PGX";
  entry->magick=(MagickHandler) IsJPC;
  entry->adjoin=False;
  entry->seekable_stream=True;
  entry->thread_support=False;
#if defined(HasJP2)
  /* Don't set an encoder since PGX is not a standard format */
  entry->decoder=(DecoderHandler) ReadJP2Image;
#endif
  entry->coder_class=StableCoderClass;
  (void) RegisterMagickInfo(entry);

  /*
    Initialize Jasper
  */
#if defined(HasJP2)
  jas_init();
#endif
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r J P 2 I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterJP2Image removes format registrations made by the
%  PNG module from the list of supported formats.
%
%  The format of the UnregisterJP2Image method is:
%
%      UnregisterJP2Image(void)
%
*/
ModuleExport void UnregisterJP2Image(void)
{
  (void) UnregisterMagickInfo("JP2");
  (void) UnregisterMagickInfo("JPC");
  (void) UnregisterMagickInfo("PGX");

#if defined(HasJP2)
  /*
    Cleanup Jasper
  */
  jas_cleanup();
#endif
}

#if defined(HasJP2)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e J P 2 I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteJP2Image writes an image in the JPEG 2000 image format.
%
%  JP2 support originally written by Nathan Brown, nathanbrown@letu.edu
%
%  The format of the WriteJP2Image method is:
%
%      unsigned int WriteJP2Image(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteJP2Image return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
static unsigned int WriteJP2Image(const ImageInfo *image_info,Image *image)
{
  char
    magick[MaxTextExtent],
    option_keyval[MaxTextExtent],
    *options = NULL;

  int
    format;

  long
    y;

  jas_image_cmptparm_t
    component_info;

  jas_image_t
    *jp2_image;

  jas_matrix_t
    *pixels[4];

  jas_stream_t
    *jp2_stream;

  register const PixelPacket
    *p;

  register int
    i,
    x;

  unsigned int
    rate_specified=False,
    status;

  unsigned int
    number_components;

  ImageCharacteristics
    characteristics;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenError,UnableToOpenFile,image);

  /*
    Ensure that image is in RGB space.
  */
  (void) TransformColorspace(image,RGBColorspace);

  /*
    Analyze image to be written.
  */
  if (!GetImageCharacteristics(image,&characteristics,
                               (OptimizeType == image_info->type),
                               &image->exception))
    {
      CloseBlob(image);
      return MagickFail;
    }

  /*
    Obtain a JP2 stream.
  */
  jp2_stream=JP2StreamManager(image);
  if (jp2_stream == (jas_stream_t *) NULL)
    ThrowWriterException(DelegateError,UnableToManageJP2Stream,image);
  number_components=image->matte ? 4 : 3;
  if ((image_info->type != TrueColorType) &&
      (characteristics.grayscale))
    number_components=1;

  jp2_image=jas_image_create0();
  if (jp2_image == (jas_image_t *) NULL)
    ThrowWriterException(DelegateError,UnableToCreateImage,image);

  for (i=0; i < (long) number_components; i++)
  {
    (void) memset((void *)&component_info,0,sizeof(jas_image_cmptparm_t));
    component_info.tlx=0; /* top left x ordinate */
    component_info.tly=0; /* top left y ordinate */
    component_info.hstep=1; /* horizontal pixels per step */
    component_info.vstep=1; /* vertical pixels per step */
    component_info.width=(unsigned int) image->columns;
    component_info.height=(unsigned int) image->rows;
    component_info.prec=(unsigned int) image->depth <= 8 ? 8 : 16; /* bits in range */
    component_info.sgnd = false;  /* range is signed value? */

    if (jas_image_addcmpt(jp2_image, i,&component_info)) {
      jas_image_destroy(jp2_image);
      ThrowWriterException(DelegateError,UnableToCreateImageComponent,image);
    }
  }

  if (number_components == 1)
    {
      /* FIXME: If image has an attached ICC profile, then the profile
         should be transferred and the image colorspace set to
         JAS_CLRSPC_GENGRAY */
      /* sRGB Grayscale */
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "Setting SGRAY colorspace");
      jas_image_setclrspc(jp2_image, JAS_CLRSPC_SGRAY);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "Setting GRAY channel to channel 0");
      jas_image_setcmpttype(jp2_image,0,
        JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_GRAY_Y));
    }
  else
    {
      /* FIXME: If image has an attached ICC profile, then the profile
         should be transferred and the image colorspace set to
         JAS_CLRSPC_GENRGB */

      /* sRGB */
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "Setting SRGB colorspace");
      jas_image_setclrspc(jp2_image, JAS_CLRSPC_SRGB);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "Setting RED channel to channel 0");
      jas_image_setcmpttype(jp2_image,0,
        JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_R));
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "Setting GREEN channel to channel 1");
      jas_image_setcmpttype(jp2_image,1,
        JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_G));
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "Setting BLUE channel to channel 2");
      jas_image_setcmpttype(jp2_image,2,
        JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_B));
      if (number_components == 4 )
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "Setting OPACITY channel to channel 3");
          jas_image_setcmpttype(jp2_image,3,
            JAS_IMAGE_CT_COLOR(JAS_IMAGE_CT_OPACITY));
        }
    }
  /*
    Convert to JPEG 2000 pixels.
  */
  for (i=0; i < (long) number_components; i++)
  {
    pixels[i]=jas_matrix_create(1,(unsigned int) image->columns);
    if (pixels[i] == (jas_matrix_t *) NULL)
      {
        for (x=0; x < i; x++)
          jas_matrix_destroy(pixels[x]);
        jas_image_destroy(jp2_image);
        ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image)
      }
  }
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    if (image->depth <= 8)
      for (x=0; x < (long) image->columns; x++)
      {
        if (number_components == 1)
          jas_matrix_setv(pixels[0],x,
            ScaleQuantumToChar(PixelIntensityToQuantum(p)));
        else
          {
            jas_matrix_setv(pixels[0],x,ScaleQuantumToChar(p->red));
            jas_matrix_setv(pixels[1],x,ScaleQuantumToChar(p->green));
            jas_matrix_setv(pixels[2],x,ScaleQuantumToChar(p->blue));
            if (number_components > 3)
              jas_matrix_setv(pixels[3],x,ScaleQuantumToChar(MaxRGB-p->opacity));
          }
        p++;
      }
    else
      for (x=0; x < (long) image->columns; x++)
        {
          if (number_components == 1)
            jas_matrix_setv(pixels[0],x,
              ScaleQuantumToShort(PixelIntensityToQuantum(p)));
          else
            {
              jas_matrix_setv(pixels[0],x,ScaleQuantumToShort(p->red));
              jas_matrix_setv(pixels[1],x,ScaleQuantumToShort(p->green));
              jas_matrix_setv(pixels[2],x,ScaleQuantumToShort(p->blue));
              if (number_components > 3)
                jas_matrix_setv(pixels[3],x,
                  ScaleQuantumToShort(MaxRGB-p->opacity));
            }
          p++;
        }
    for (i=0; i < (long) number_components; i++)
      {
        (void) jas_image_writecmpt(jp2_image,(short) i,0,(unsigned int) y,
          (unsigned int) image->columns,1,pixels[i]);
      }
    if (image->previous == (Image *) NULL)
      if (QuantumTick(y,image->rows))
        if (!MagickMonitorFormatted(y,image->rows,&image->exception,
                                    SaveImageText,image->filename))
          break;
  }
  (void) strlcpy(magick,image_info->magick,MaxTextExtent);
  LocaleLower(magick);
  format=jas_image_strtofmt(magick);

  /*
    Support passing Jasper options.
  */
  {
    const char
      **option_name;

    static const char *jasper_options[] =
      {
        "imgareatlx",
        "imgareatly",
        "tilegrdtlx",
        "tilegrdtly",
        "tilewidth",
        "tileheight",
        "prcwidth",
        "prcheight",
        "cblkwidth",
        "cblkheight",
        "mode",
        "ilyrrates",
        "prg",
        "nomct",
        "numrlvls",
        "sop",
        "eph",
        "lazy",
        "rate",
        "termall",
        "segsym",
        "vcausal",
        "pterm",
        "resetprob",
        "numgbits",
        NULL
      };
    for (option_name = jasper_options; *option_name != NULL; option_name++)
      {
        const char
          *value;

        if ((value=AccessDefinition(image_info,"jp2",*option_name)) != NULL)
          {
            if(LocaleCompare(*option_name,"rate") == 0)
              rate_specified=True;
            FormatString(option_keyval,"%s=%.1024s ",*option_name,value);
            ConcatenateString(&options,option_keyval);
          }
      }
  }
  /*
    Provide an emulation of IJG JPEG "quality" by default.
  */
  if (rate_specified == False)
    {
      double
        rate=1.0;
      
      /*
        A rough approximation to JPEG v1 quality using JPEG-2000.
        Default "quality" 75 results in a request for 16:1 compression, which
        results in image sizes approximating that of JPEG v1.
      */
      if ((image_info->quality < 99.5) && (image->rows*image->columns > 2500))
        {
          double
            header_size,
            current_size,
            target_size,
            d;
          
          d=115-image_info->quality;  /* Best number is 110-115 */
          rate=100.0/(d*d);
          header_size=550.0; /* Base file size. */
          header_size+=(number_components-1)*142; /* Additional components */
          /* FIXME: Need to account for any ICC profiles here */
          
          current_size=(double)image->rows*image->columns*(image->depth/8)*
            number_components;
          target_size=(current_size*rate)+header_size;
          rate=target_size/current_size;
        }
      FormatString(option_keyval,"%s=%g ","rate",rate);
      ConcatenateString(&options,option_keyval);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "Compression rate: %g (%3.2f:1)",rate,(double)1/rate);
    }
  if (options)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
       "Jasper options: \"%s\"", options);

  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Encoding image");
  status=jas_image_encode(jp2_image,jp2_stream,format,options);
  (void) jas_stream_close(jp2_stream);
  MagickFreeMemory(options);
  for (i=0; i < (long) number_components; i++)
    jas_matrix_destroy(pixels[i]);
  jas_image_destroy(jp2_image);
  if (status)
    ThrowWriterException(DelegateError,UnableToEncodeImageFile,image);
  return(True);
}
#endif
