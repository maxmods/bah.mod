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
%                            PPPP   SSSSS  DDDD                               %
%                            P   P  SS     D   D                              %
%                            PPPP    SSS   D   D                              %
%                            P         SS  D   D                              %
%                            P      SSSSS  DDDD                               %
%                                                                             %
%                                                                             %
%                  Read/Write Adobe Photoshop Image Format.                   %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                              Leonard Rosenthol                              %
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
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/profile.h"
#include "magick/utility.h"

/*
  Forward declarations.
*/
static unsigned int
  WritePSDImage(const ImageInfo *,Image *);

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
%  Method DecodeImage uncompresses an image via Macintosh encoding specific to
%  the Adobe Photoshop image format.
%
%  The format of the DecodeImage method is:
%
%      unsigned int DecodeImage(Image *image,const long channel)
%
%  A description of each parameter follows:
%
%    o status: Method DecodeImage return True if the image is
%      decoded.  False is returned if there is an error occurs.
%
%    o image,image: The address of a structure of type Image.
%
%    o channel:  Specifies which channel: red, green, blue, or index to
%      decode the pixel values into.
%
%
*/
static unsigned int DecodeImage(Image *image,const int channel)
{
  int
    count;

  long
    number_pixels;

  register IndexPacket
    *indexes;

  register long
    i,
    x;

  register PixelPacket
    *q;

  unsigned long
    pixel;

  x=0;
  number_pixels=(long) (image->columns*image->rows);
  while (number_pixels > 0)
    {
      count=ReadBlobByte(image);
      if (count >= 128)
        count-=256;
      if (count < 0)
        {
          if (count == -128)
            continue;
          pixel=ReadBlobByte(image);
          for (count=(-count+1); count > 0; count--)
            {
              q=GetImagePixels(image,(long) (x % image->columns),
                               (long) (x/image->columns),1,1);
              if (q == (PixelPacket *) NULL)
                break;
              indexes=AccessMutableIndexes(image);
              switch (channel)
                {
                case -1:
                  {
                    if (image->colorspace == CMYKColorspace)
                      indexes[0]=(Quantum) (MaxRGB-ScaleCharToQuantum(pixel));
                    else
                      q->opacity=(Quantum) (MaxRGB-ScaleCharToQuantum(pixel));
                    break;
                  }
                case 0:
                  {
                    q->red=ScaleCharToQuantum(pixel);
                    if (image->storage_class == PseudoClass)
                      {
                        indexes[0]=(IndexPacket) pixel;
                        *q=image->colormap[pixel];
                      }
                    break;
                  }
                case 1:
                  {
                    if (image->storage_class == PseudoClass)
                      q->opacity=ScaleCharToQuantum(pixel);
                    else
                      q->green=ScaleCharToQuantum(pixel);
                    break;
                  }
                case 2:
                  {
                    q->blue=ScaleCharToQuantum(pixel);
                    break;
                  }
                case 3:
                  {
                    q->opacity=ScaleCharToQuantum(pixel);
                    break;
                  }
                case 4:
                  {
                    if (image->colorspace == CMYKColorspace)
                      indexes[0]=(Quantum) (MaxRGB-ScaleCharToQuantum(pixel));
                    break;
                  }
                default:
                  break;
                }
              if (!SyncImagePixels(image))
                break;
              x++;
              number_pixels--;
            }
          continue;
        }
      count++;
      for (i=count; i > 0; i--)
        {
          pixel=ReadBlobByte(image);
          q=GetImagePixels(image,(long) (x % image->columns),
                           (long) (x/image->columns),1,1);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=AccessMutableIndexes(image);
          switch (channel)
            {
            case -1:
              {
                if (image->colorspace == CMYKColorspace)
                  indexes[0]=(Quantum) (MaxRGB-ScaleCharToQuantum(pixel));
                else
                  q->opacity=(Quantum) (MaxRGB-ScaleCharToQuantum(pixel));
                break;
              }
            case 0:
              {
                q->red=ScaleCharToQuantum(pixel);
                if (image->storage_class == PseudoClass)
                  {
                    indexes[0]=(IndexPacket) pixel;
                    *q=image->colormap[pixel];
                  }
                break;
              }
            case 1:
              {
                if (image->storage_class == PseudoClass)
                  q->opacity=ScaleCharToQuantum(pixel);
                else
                  q->green=ScaleCharToQuantum(pixel);
                break;
              }
            case 2:
              {
                q->blue=ScaleCharToQuantum(pixel);
                break;
              }
            case 3:
              {
                q->opacity=ScaleCharToQuantum(pixel);
                break;
              }
            case 4:
              {
                if (image->colorspace == CMYKColorspace)
                  indexes[0]=(Quantum) (MaxRGB-ScaleCharToQuantum(pixel));
                break;
              }
            default:
              break;
            }
          if (!SyncImagePixels(image))
            break;
          x++;
          number_pixels--;
        }
    }
  /*
    Guarentee the correct number of pixel packets.
  */
  if (number_pixels > 0)
    {
      ThrowBinaryException(CorruptImageError,InsufficientImageDataInFile,
                           image->filename);
    }
  else
    {
      if (number_pixels < 0)
        ThrowBinaryException(CorruptImageError,TooMuchImageDataInFile,
                             image->filename);
    }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s P S D                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsPSD returns True if the image format type, identified by the
%  magick string, is PSD.
%
%  The format of the IsPSD method is:
%
%      unsigned int IsPSD(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsPSD returns True if the image format type is PSD.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsPSD(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(False);
  if (LocaleNCompare((char *) magick,"8BPS",4) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d P S D I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPSDImage reads an Adobe Photoshop image file and returns it.
%  It allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadPSDImage method is:
%
%      image=ReadPSDImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadPSDImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/

static CompositeOperator PSDBlendModeToCompositeOperator(const char *mode)
{
  if (mode == (const char *) NULL)
    return(OverCompositeOp);
  if (LocaleNCompare(mode,"norm",4) == 0)
    return(OverCompositeOp);
  if (LocaleNCompare(mode,"mul ",4) == 0)
    return(MultiplyCompositeOp);
  if (LocaleNCompare(mode,"diss",4) == 0)
    return(DissolveCompositeOp);
  if (LocaleNCompare(mode,"diff",4) == 0)
    return(DifferenceCompositeOp);
  if (LocaleNCompare(mode,"dark",4) == 0)
    return(DarkenCompositeOp);
  if (LocaleNCompare(mode,"lite",4) == 0)
    return(LightenCompositeOp);
  if (LocaleNCompare(mode,"hue ",4) == 0)
    return(HueCompositeOp);
  if (LocaleNCompare(mode,"sat ",4) == 0)
    return(SaturateCompositeOp);
  if (LocaleNCompare(mode,"colr",4) == 0)
    return(ColorizeCompositeOp);
  if (LocaleNCompare(mode,"lum ",4) == 0)
    return(LuminizeCompositeOp);
  if (LocaleNCompare(mode,"scrn",4) == 0)
    return(ScreenCompositeOp);
  if (LocaleNCompare(mode,"over",4) == 0)
    return(OverlayCompositeOp);
  if (LocaleNCompare(mode,"hLit",4) == 0)
    return(OverCompositeOp);
  if (LocaleNCompare(mode,"sLit",4) == 0)
    return(OverCompositeOp);
  if (LocaleNCompare(mode,"smud",4) == 0)
    return(OverCompositeOp);
  if (LocaleNCompare(mode,"div ",4) == 0)
    return(OverCompositeOp);
  if (LocaleNCompare(mode,"idiv",4) == 0)
    return(OverCompositeOp);
  return(OverCompositeOp);
}

static char *CompositeOperatorToPSDBlendMode(CompositeOperator inOp)
{
  char*  outMode = (char *) "norm";

  switch ( inOp )
  {
    case OverCompositeOp:    outMode = (char *) "norm";  break;
    case MultiplyCompositeOp:  outMode = (char *) "mul ";  break;
    case DissolveCompositeOp:  outMode = (char *) "diss";  break;
    case DifferenceCompositeOp:  outMode = (char *) "diff";  break;
    case DarkenCompositeOp:    outMode = (char *) "dark";  break;
    case LightenCompositeOp:  outMode = (char *) "lite";  break;
    case HueCompositeOp:    outMode = (char *) "hue ";  break;
    case SaturateCompositeOp:  outMode = (char *) "sat ";  break;
    case ColorizeCompositeOp:  outMode = (char *) "colr";  break;
    case LuminizeCompositeOp:  outMode = (char *) "lum ";  break;
    case ScreenCompositeOp:    outMode = (char *) "scrn";  break;
    case OverlayCompositeOp:  outMode = (char *) "over";  break;

    default:
      outMode = (char *) "norm";
/*

  if (LocaleNCompare(mode,"hLit",4) == 0)
    return(OverCompositeOp);
  if (LocaleNCompare(mode,"sLit",4) == 0)
    return(OverCompositeOp);
  if (LocaleNCompare(mode,"smud",4) == 0)
    return(OverCompositeOp);
  if (LocaleNCompare(mode,"div ",4) == 0)
    return(OverCompositeOp);
  if (LocaleNCompare(mode,"idiv",4) == 0)
    return(OverCompositeOp);

*/
  }

  return outMode;
}

typedef enum
{
  BitmapMode = 0,
  GrayscaleMode = 1,
  IndexedMode = 2,
  RGBMode = 3,
  CMYKMode = 4,
  MultichannelMode = 7,
  DuotoneMode = 8,
  LabMode = 9
} PSDImageType;

static const char* ModeToString( PSDImageType inType )
{
  switch ( inType )
    {
    case BitmapMode:        return "Bitmap";
    case GrayscaleMode:     return "Grayscale";
    case IndexedMode:       return "Indexed";
    case RGBMode:           return "RGB";
    case CMYKMode:          return "CMYK";
    case MultichannelMode:  return "Multichannel";
    case DuotoneMode:       return "Duotone";
    case LabMode:           return "L*A*B";
    default:                return "unknown";
    }
}


static Image *ReadPSDImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
#define MaxPSDChannels  24
  typedef struct _ChannelInfo
  {
    short int
      type;

    unsigned long
      size;
  } ChannelInfo;

  typedef struct _LayerInfo
  {
    RectangleInfo
      page,
      mask;

    unsigned short
      channels;

    ChannelInfo
      channel_info[MaxPSDChannels];

    char
      blendkey[4];

    Quantum
      opacity;

    unsigned char
      clipping,
      visible,
      flags;

    unsigned long
      offset_x,
      offset_y;

    unsigned char
      name[256];

    Image
      *image;
  } LayerInfo;

  typedef struct _PSDInfo
  {
    char
      signature[4]; /* "8BPS" */

    unsigned short
      channels,     /* 1 to 24 */
      version;      /* 1 */

    unsigned char
      reserved[6];  /* Unused, null */

    unsigned long
      rows,         /* Height, 1 to 30,000 in PS6 */
      columns;      /* Width, 1 to 30,000 in PS6 */

    unsigned short
      depth,        /* Bits per channel. 1, 8, and 16. */
      mode;         /* Mode. Bitmap=0; Grayscale=1; Indexed=2; RGB=3; CMYK=4;
                       Multichannel=7; Duotone=8; Lab=9. */
  } PSDInfo;

  char
    s[MaxTextExtent],
    type[4];

  Image
    *image;

  IndexPacket
    *indexes;

  LayerInfo
    *layer_info;

  long
    j,
    number_layers,
    y;

  PSDInfo
    psd_info;

  register long
    x;

  register PixelPacket
    *q;

  register long
    i;

  size_t
    count,
    length,
    combinedlength,
    size;

  /*
    ExtendedSignedIntegralType
    offset,
    diff_offset; */

  unsigned char
    *data;

  unsigned int
    logging,
    packet_size,
    status;

  unsigned short
    compression;

  unsigned long
    pixel;


  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  logging=LogMagickEvent(CoderEvent,GetMagickModule(),"enter");
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    {
      if(logging)
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),"  OpenBlob failed");
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "return ReadPSDImage()");
        }
      ThrowReaderException(FileOpenError,UnableToOpenFile,image);
    }

  /*
    Read image header.
  */
  count=ReadBlob(image,4,(char *) psd_info.signature);
  psd_info.version=ReadBlobMSBShort(image);
  if ((count != 4) ||
      (LocaleNCompare(psd_info.signature,"8BPS",4) != 0) ||
      (psd_info.version != 1))
    {
      if(logging)
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "  File signature was %.4s instead of '8BPS'",
                                psd_info.signature );
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),"return");
        }
      ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
    }
  (void) ReadBlob(image,6,(char *) psd_info.reserved);
  psd_info.channels=ReadBlobMSBShort(image); /* Number of channels */
  if (psd_info.channels > MaxPSDChannels)
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
  psd_info.rows=ReadBlobMSBLong(image);    /* Height */
  psd_info.columns=ReadBlobMSBLong(image); /* Width */
  psd_info.depth=ReadBlobMSBShort(image);  /* Depth */
  psd_info.mode=ReadBlobMSBShort(image);   /* Color mode */

  if (logging)
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "  Image is %ld x %ld with channels=%d, depth=%d, mode=%s",
                            psd_info.columns, psd_info.rows, psd_info.channels,
                            psd_info.depth, ModeToString((PSDImageType) psd_info.mode));
    }
  /*
    Initialize image.
  */
  image->depth=psd_info.depth <= 8 ? 8 : QuantumDepth;
  image->columns=psd_info.columns;
  image->rows=psd_info.rows;
  (void) SetImage(image,OpaqueOpacity);
  image->matte=psd_info.channels >= 4;
  if (psd_info.mode == CMYKMode)
    {
      image->colorspace=CMYKColorspace;
      image->matte=psd_info.channels >= 5;
    }
  if ((psd_info.mode == BitmapMode) ||
      (psd_info.mode == GrayscaleMode) ||
      (psd_info.mode == DuotoneMode))
    {
      if (!AllocateImageColormap(image,256)) {
        if (logging)
          {
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "  allocation of ImageColorMap failed");
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),"return");
          }
        ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
      }
      image->matte=psd_info.channels >= 2;
      if (logging)
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "  ImageColorMap allocated");
        }
    }
  
  if (logging)
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(), 
                            image->matte ?
                            "  image has matte" : "  image has no matte");
    }
  
  /*
    Read PSD raster colormap only present for indexed and duotone images.
  */
  length=ReadBlobMSBLong(image);
  if (length != 0)
    {
      if (logging)
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "  reading colormap");
        }

      if (psd_info.mode == DuotoneMode)
        {
          /*
            Duotone image data;  the format of this data is undocumented.
          */
          data=MagickAllocateMemory(unsigned char *,length);
          if (data == (unsigned char *) NULL) {
            if(logging)
              {
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                      "  allocation of ImageColorMap failed");
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),"return");
              }
            ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
                                 image);
          }
          (void) ReadBlob(image,length,data);
          MagickFreeMemory(data);
        }
      else
        {
          /*
            Read PSD raster colormap.
          */
          if (!AllocateImageColormap(image,length/3))
            {
              if(logging)
                {
                  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                        "  allocation of ImageColorMap failed");
                  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"return");
                }
              ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
                                   image);
            }
          for (i=0; i < (long) image->colors; i++)
            image->colormap[i].red=ScaleCharToQuantum(ReadBlobByte(image));
          for (i=0; i < (long) image->colors; i++)
            image->colormap[i].green=ScaleCharToQuantum(ReadBlobByte(image));
          for (i=0; i < (long) image->colors; i++)
            image->colormap[i].blue=ScaleCharToQuantum(ReadBlobByte(image));
          image->matte=psd_info.channels >= 2;
        }
    }

  /*
    Image resources.  Currently we simply load this up into the IPTC
    block for access by other methods.  In the future, we may need to
    access parts of it ourselves to support newer features of PSD.
  */
  length=ReadBlobMSBLong(image);
  if (length != 0)
    {
      if (logging)
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "  reading image resources (IPTC) - %ld bytes",
                                (long) length);
        }
      data=MagickAllocateMemory(unsigned char *,length);
      if (data == (unsigned char *) NULL)
        {
          if (logging)
            {
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "  allocation of resources/IPTC failed");
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),"return");
            }
          ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
        }
      count=ReadBlob(image,length,(char *) data);
      if ((count == 0) || (LocaleNCompare((char *) data,"8BIM",4) != 0))
        {
          if (logging)
            {
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "  image resources invalid");
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),"return");
            }
          ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
        }
      (void) SetImageProfile(image,"IPTC",data,length);
      MagickFreeMemory(data)
    }

  /*
    If we are only "pinging" the image, then we're done - so return.
  */
  if (image_info->ping)
    {
      CloseBlob(image);
      
      if (logging)
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "  pinging of image complete");
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),"return");
        }

      return(image);
    }

  /*
    Layer and mask block.
  */
  layer_info=(LayerInfo *) NULL;
  number_layers=0;
  length=ReadBlobMSBLong(image);
  if (length == 8)
    {
      length=ReadBlobMSBLong(image);
      length=ReadBlobMSBLong(image);
    }

  if (length != 0)
    {
      /* offset = TellBlob(image); */
      size=ReadBlobMSBLong(image);
      number_layers=(short) ReadBlobMSBShort(image);
      if (logging)
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "  image contains %ld layers",
                                number_layers);
        }
      if (number_layers < 0)
        {
          /*
            Weird hack in PSD format to ignore first alpha channel.
          */
          number_layers=AbsoluteValue(number_layers);
          if(logging)
            {
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "  negative layer count corrected for");
            }
        }
      layer_info=MagickAllocateMemory(LayerInfo *,number_layers*sizeof(LayerInfo));
      if (layer_info == (LayerInfo *) NULL)
        {
          if (logging)
            {
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "  allocation of LayerInfo failed");
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),"return");
            }
          ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
        }
      (void) memset(layer_info,0,number_layers*sizeof(LayerInfo));
      for (i=0; i < number_layers; i++)
        {
          if (logging)
            {
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "  reading layer #%ld", i+1);
            }
          layer_info[i].page.y=(long) ReadBlobMSBLong(image);
          layer_info[i].page.x=(long) ReadBlobMSBLong(image);
          layer_info[i].page.height=(ReadBlobMSBLong(image)-layer_info[i].page.y);
          layer_info[i].page.width=(ReadBlobMSBLong(image)-layer_info[i].page.x);
          layer_info[i].channels=ReadBlobMSBShort(image);
          if (logging)
            {
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "    offset(%ld,%ld), size(%ld,%ld), channels=%d", 
                                    layer_info[i].page.x,
                                    layer_info[i].page.y, 
                                    layer_info[i].page.height,
                                    layer_info[i].page.width,
                                    layer_info[i].channels);
            }
          for (j=0; j < layer_info[i].channels; j++)
            {
              layer_info[i].channel_info[j].type=ReadBlobMSBShort(image);
              layer_info[i].channel_info[j].size=ReadBlobMSBLong(image);

              if (logging)
                {
                  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                        "    channel[%ld]: type=%d, size=%ld", 
                                        j, layer_info[i].channel_info[j].type, 
                                        (long) layer_info[i].channel_info[j].size);
                }
            }
          count=ReadBlob(image,4,(char *) type);
          if ((count == 0) || (LocaleNCompare(type,"8BIM",4) != 0))
            {
              if (logging)
                {
                  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                        "  layer type was %.4s instead of 8BIM", type);
                  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"return");
                }
              ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
            }
          (void) ReadBlob(image,4,(char *) layer_info[i].blendkey);
          layer_info[i].opacity=(Quantum) (MaxRGB-ScaleCharToQuantum(ReadBlobByte(image)));
          layer_info[i].clipping=ReadBlobByte(image);
          layer_info[i].flags=ReadBlobByte(image);
          layer_info[i].visible=!(layer_info[i].flags & 0x02);
          if (logging)
            {
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "    blend=%.4s, opacity=%d, clipping=%s, flags=%d, visible=%s", 
                                    layer_info[i].blendkey, layer_info[i].opacity,
                                    layer_info[i].clipping ? "true" : "false",
                                    layer_info[i].flags,
                                    layer_info[i].visible ? "true" : "false");
            }

          (void) ReadBlobByte(image);  /* filler */
          combinedlength = 0;
          size=ReadBlobMSBLong(image);
          if (size != 0)
            {
              if (logging)
                {
                  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                        "    layer contains additional info");
                }
              length=ReadBlobMSBLong(image);
              if (length != 0)
                {
                  /*
                    Layer mask info.
                  */
                  layer_info[i].mask.y=(long) ReadBlobMSBLong(image);
                  layer_info[i].mask.x=(long) ReadBlobMSBLong(image);
                  layer_info[i].mask.height=
                    (ReadBlobMSBLong(image)-layer_info[i].mask.y);
                  layer_info[i].mask.width=
                    (ReadBlobMSBLong(image)-layer_info[i].mask.x);

                  if (logging)
                    {
                      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                            "      layer mask: offset(%ld,%ld), size(%ld,%ld), length=%ld",
                                            layer_info[i].mask.x, layer_info[i].mask.y,
                                            layer_info[i].mask.width, layer_info[i].mask.height,
                                            (long) length-16);
                    }
                  /*
                    Skip over the rest of the layer mask information.
                  */
                  for (j=0; j < (long) (length-16); j++)
                    (void) ReadBlobByte(image);
                }
              combinedlength += length + 4;  /* +4 for length */

              length=ReadBlobMSBLong(image);
              if (length != 0)
                {
                  /*
                    Layer blending ranges info.
                  */
                  if (logging)
                    {
                      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                            "      layer blending ranges: length=%ld",
                                            (long) length);
                    }

                  /*
                    we read it, but don't use it...
                  */
                  for (j=0; j < (long) (length); j+=8) {
                    size_t blend_source = ReadBlobMSBLong(image);
                    size_t blend_dest = ReadBlobMSBLong(image);
                    if (logging)
                      {
                        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                              "        source(%x), dest(%x)",
                                              (unsigned int) blend_source,
                                              (unsigned int) blend_dest);
                      }
                  }
                }
              combinedlength += length + 4;  /* +4 for length */

              length=ReadBlobByte(image);
              if (length != 0)
                {
                  /* layer name */
                  for (j=0; j < (long) (length); j++)
                    layer_info[i].name[j] = ReadBlobByte(image);
                  layer_info[i].name[j] = 0;  /* zero term */

                  if (logging)
                    {
                      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                            "      layer name: %s",
                                            layer_info[i].name);
                    }
                }
#if 0
              if ( length == 0 )
                padBytes = 3;
              else
                padBytes = (4 - (length % 4));
              if ( padBytes != 0 )
                { /* we need to pad */
                  for ( i=0; i < padBytes; i++)
                    (void) ReadBlobByte(image);
                }
              combinedlength += length + padBytes + 1;  /* +1 for length */
#else
              combinedlength += length + 1;  /* +1 for length */
#endif

#if 0  /* still in development */
              /*
                Adjustment layers and other stuff...
              */
              {
                char  alsig[4],
                  alkey[4];

                count=ReadBlob(image,4,(char *) alsig);
                if ((count == 0) || (LocaleNCompare(alsig,"8BIM",4) != 0))
                  {
                    if (logging)
                      {
                        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                              "  adjustment layer type was %.4s instead of 8BIM",
                                              alsig);
                        (void) LogMagickEvent(CoderEvent,GetMagickModule(),"return");
                      }
                    ThrowReaderException(CorruptImageError,NotAPSDImageFile,image);
                  }
                (void) ReadBlob(image,4,(char *) alkey);
                length=ReadBlobMSBLong(image);
                if (logging)
                  {
                    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                          "      adjustment layer key: %.4s, data length=%ld",
                                          alkey, length);
                  }

                if ( length )
                  {
                    for (j=0; j < (long) (length); j++)
                      (void) ReadBlobByte(image);
                  }

              }
              combinedlength += 12 + length;  /* sig, key, length + the actual length*/
#endif

              /*
                Skip the rest of the variable data until we support it.
              */
              if (logging)
                {
                  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                        "      unsupported data: length=%ld",
                                        (long) size-combinedlength);
                }
              for (j=0; j < (long) (size-combinedlength); j++)
                (void) ReadBlobByte(image);
            }
          /*
            Allocate layered image.
          */
          layer_info[i].image=CloneImage(image,layer_info[i].page.width,
                                         layer_info[i].page.height,
                                         True,&image->exception);
          if (layer_info[i].image->blob)
            {
              DestroyBlob(layer_info[i].image);
              layer_info[i].image->blob=0;
            }
          layer_info[i].image->blob=ReferenceBlob(image->blob);
          if (layer_info[i].image == (Image *) NULL)
            {
              for (j=0; j < i; j++)
                DestroyImage(layer_info[j].image);

              if (logging)
                {
                  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                        "  allocation of image for layer %ld failed", i);
                  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"return");
                }

              ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
                                   image);
            }
          if (logging)
            {
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "    setting up new layer image");
            }
          (void) SetImage(layer_info[i].image,OpaqueOpacity);
          layer_info[i].image->compose=
            PSDBlendModeToCompositeOperator(layer_info[i].blendkey);
          if (layer_info[i].visible == False)
            layer_info[i].image->compose=NoCompositeOp;
          if (psd_info.mode == CMYKMode)
            layer_info[i].image->colorspace=CMYKColorspace;
          for (j=0; j < layer_info[i].channels; j++)
            if (layer_info[i].channel_info[j].type == -1)
              layer_info[i].image->matte=True;

          /* set up some hidden attributes for folks that need them */
          (void) sprintf( s, "%ld", layer_info[i].page.x );
          (void) SetImageAttribute(layer_info[i].image,"[layer-xpos]",s);
          (void) sprintf( s, "%ld", layer_info[i].page.y );
          (void) SetImageAttribute(layer_info[i].image,"[layer-ypos]",s);
          (void) sprintf( s, "%d", layer_info[i].opacity );
          (void) SetImageAttribute(layer_info[i].image,"[layer-opacity]",s);
          (void) SetImageAttribute(layer_info[i].image,"[layer-name]",
                                   (char *) layer_info[i].name);
        }

      if (logging)
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "  reading image data for layers");
        }
      /*
        Read pixel data for each layer.
      */
      for (i=0; i < number_layers; i++)
        {
          if (logging)
            {
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "  reading data for layer %ld", i);
            }
          for (j=0; j < layer_info[i].channels; j++)
            {
              if (logging)
                {
                  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                        "    reading data for channel %ld", j);
                }
#if 0
              if (layer_info[i].channel_info[j].size <= (2*layer_info[i].image->rows))
                {
                  long
                    k;

                  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                        "      layer data is empty");

                  /*
                    A layer without data.
                  */
                  for (k=0; k < (long) layer_info[i].channel_info[j].size; k++)
                    (void) ReadBlobByte(layer_info[i].image);
                  continue;
                }
#endif
              compression=ReadBlobMSBShort(layer_info[i].image);
              if (compression == 1)
                {
                  /*
                    Read RLE compressed data.
                  */
                  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                        "      layer data is RLE compressed");

                  for (y=0; y < (long) layer_info[i].image->rows; y++)
                    (void) ReadBlobMSBShort(layer_info[i].image);
                  (void) DecodeImage(layer_info[i].image,
                                     layer_info[i].channel_info[j].type);
                  continue;
                }

              /*
                Read uncompressed pixel data as separate planes.
              */
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "      layer data is uncompressed");
              packet_size=1;
              if (layer_info[i].image->storage_class == PseudoClass)
                {
                  if (layer_info[i].image->colors > 256)
                    packet_size++;
                }
              else
                if (layer_info[i].image->depth > 8)
                  packet_size++;
              for (y=0; y < (long) layer_info[i].image->rows; y++)
                {
                  q=GetImagePixels(layer_info[i].image,0,y,
                                   layer_info[i].image->columns,1);
                  if (q == (PixelPacket *) NULL)
                    break;
                  indexes=AccessMutableIndexes(layer_info[i].image);
                  for (x=0; x < (long) layer_info[i].image->columns; x++)
                    {
                      if (packet_size == 1)
                        pixel=ScaleCharToQuantum(ReadBlobByte(layer_info[i].image));
                      else
                        pixel=ScaleQuantumToShort(ReadBlobMSBShort(layer_info[i].image));
                      switch (layer_info[i].channel_info[j].type)
                        {
                        case -1:  /* transparency mask */
                          {
                            if (image->colorspace == CMYKColorspace)
                              indexes[x]=(Quantum) (MaxRGB-pixel);
                            else
                              q->opacity=(Quantum) (MaxRGB-pixel);
                            break;
                          }
                        case 0:  /* first component (Red, Cyan, Gray or Index) */
                          {
                            q->red=(Quantum) pixel;
                            if (layer_info[i].image->storage_class == PseudoClass)
                              {
                                indexes[x]=(IndexPacket) ScaleQuantumToChar(MaxRGB - pixel);
                                *q=layer_info[i].image->colormap[indexes[x]];
                              }
                            break;
                          }
                        case 1:  /* second component (Green, Magenta, or opacity) */
                          {
                            if (layer_info[i].image->storage_class == PseudoClass)
                              q->opacity=(Quantum) pixel;
                            else
                              q->green=(Quantum) pixel;
                            break;
                          }
                        case 2:  /* third component (Blue or Yellow) */
                          {
                            q->blue=(Quantum) pixel;
                            break;
                          }
                        case 3:  /* fourth component (Opacity or Black) */
                          {
                            q->opacity=(Quantum) pixel;
                            break;
                          }
                        case 4:  /* fifth component (opacity) */
                          {
                            if (image->colorspace == CMYKColorspace)
                              indexes[x]=(Quantum) (MaxRGB-pixel);
                            break;
                          }
                        default:
                          break;
                        }
                      q++;
                    }
                  if (!SyncImagePixels(layer_info[i].image))
                    break;
                }
            }
          if (layer_info[i].opacity != OpaqueOpacity)
            {
              /*
                Correct for opacity level.
              */
              for (y=0; y < (long) layer_info[i].image->rows; y++)
                {
                  q=GetImagePixels(layer_info[i].image,0,y,
                                   layer_info[i].image->columns,1);
                  if (q == (PixelPacket *) NULL)
                    break;
                  indexes=AccessMutableIndexes(layer_info[i].image);
                  for (x=0; x < (long) layer_info[i].image->columns; x++)
                    {
                      q->opacity=(Quantum) ((unsigned long)
                                            (q->opacity*layer_info[i].opacity)/MaxRGB);
                      if (layer_info[i].image->colorspace == CMYKColorspace)
                        indexes[x]=(IndexPacket) ((unsigned long)
                                                  (indexes[x]*layer_info[i].opacity)/MaxRGB);
                      q++;
                    }
                  if (!SyncImagePixels(layer_info[i].image))
                    break;
                }
            }
          if (layer_info[i].image->colorspace == CMYKColorspace)
            {
              /*
                Correct CMYK levels.
              */
              if(logging)
                {
                  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                        "    correcting CMYK values");
                }
              for (y=0; y < (long) layer_info[i].image->rows; y++)
                {
                  q=GetImagePixels(layer_info[i].image,0,y,
                                   layer_info[i].image->columns,1);
                  if (q == (PixelPacket *) NULL)
                    break;
                  for (x=0; x < (long) layer_info[i].image->columns; x++)
                    {
                      q->red=(Quantum) (MaxRGB-q->red);
                      q->green=(Quantum) (MaxRGB-q->green);
                      q->blue=(Quantum) (MaxRGB-q->blue);
                      q->opacity=(Quantum) (MaxRGB-q->opacity);
                      q++;
                    }
                  if (!SyncImagePixels(layer_info[i].image))
                    break;
                }
            }
        }

      /* diff_offset = TellBlob(image) - offset; */

      (void) ReadBlobMSBLong(image);  /* global mask size: currently ignored */

      if (number_layers > 0)
        {
          Image* returnImage = image;

          if (logging)
            {
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "  putting layers into image list");
            }
          for (i=0; i < number_layers; i++)
            {
              if (i > 0)
                layer_info[i].image->previous=layer_info[i-1].image;
              if (i < (number_layers-1))
                layer_info[i].image->next=layer_info[i+1].image;
              layer_info[i].image->page=layer_info[i].page;
            }
#ifdef use_image
          image->next=layer_info[0].image;
          layer_info[0].image->previous=image;
#else
          DestroyImage(image);
          returnImage = layer_info[0].image;  
#endif
          MagickFreeMemory(layer_info);

          if (logging)
            {
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),"return");
            }

          return(returnImage);
        }
    }
  else
    {
      if (logging)
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "  image has no layers");
        }
    }

  /*
    Read the precombined image, present for PSD < 4 compatibility.
    This is a completely rendered image.
  */
  if (logging)
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "  reading the precombined layer");
    }
  compression=ReadBlobMSBShort(image);
  if (compression == 1)
    {
      /*
        Read Packbit encoded pixel data as separate planes.
      */
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "  packbits compression");
      for (i=0; i < (long) (image->rows*psd_info.channels); i++)
        (void) ReadBlobMSBShort(image);
      for (i=0; i < psd_info.channels; i++)
        (void) DecodeImage(image,(int) i);
    }
  else
    {
      /*
        Read uncompressed pixel data as separate planes.
      */
      packet_size=1;
      if (image->storage_class == PseudoClass)
        {
          if (image->colors > 256)
            packet_size++;
        }
      else
        if (image->depth > 8)
          packet_size++;
      for (i=0; i < psd_info.channels; i++)
        {
          for (y=0; y < (long) image->rows; y++)
            {
              q=GetImagePixels(image,0,y,image->columns,1);
              if (q == (PixelPacket *) NULL)
                break;
              indexes=AccessMutableIndexes(image);
              for (x=0; x < (long) image->columns; x++)
                {
                  if (packet_size == 1)
                    pixel=ScaleCharToQuantum(ReadBlobByte(image));
                  else
                    pixel=ScaleShortToQuantum(ReadBlobMSBShort(image));
                  switch (image->matte ? i-1 : i)
                    {
                    case -1:
                      {
                        q->opacity=(Quantum) (MaxRGB-pixel);
                        break;
                      }
                    case 0:
                      {
/*                         if (x==0 && y==0) */
/*                           printf("red=%lu\n",pixel); */
                        q->red=(Quantum) pixel;
                        if (image->storage_class == PseudoClass)
                          {
                            indexes[x]=(IndexPacket) ScaleQuantumToChar(pixel);
                            *q=image->colormap[indexes[x]];
                          }
                        break;
                      }
                    case 1:
                      {
/*                         if (x==0 && y==0) */
/*                           printf("green=%lu\n",pixel); */
                        if (image->storage_class == PseudoClass)
                          q->opacity=(Quantum) pixel;
                        else
                          q->green=(Quantum) pixel;
                        break;
                      }
                    case 2:
                      {
/*                         if (x==0 && y==0) */
/*                           printf("blue=%lu\n",pixel); */
                        q->blue=(Quantum) pixel;
                        break;
                      }
                    case 3:
                      {
                        q->opacity=(Quantum) pixel;
                        break;
                      }
                    case 4:
                      {
                        if (image->colorspace == CMYKColorspace)
                          indexes[x]=(IndexPacket) pixel;
                        break;
                      }
                    default:
                      break;
                    }
                  q++;
                }
              if (!SyncImagePixels(image))
                break;
            }
        }
    }
  if (image->colorspace == CMYKColorspace)
    {
      /*
        Correct CMYK levels.
      */
      for (y=0; y < (long) image->rows; y++)
        {
          q=GetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          for (x=0; x < (long) image->columns; x++)
            {
              q->red=(Quantum) (MaxRGB-q->red);
              q->green=(Quantum) (MaxRGB-q->green);
              q->blue=(Quantum) (MaxRGB-q->blue);
              q->opacity=(Quantum) (MaxRGB-q->opacity);
              q++;
            }
          if (!SyncImagePixels(image))
            break;
        }
    }
  CloseBlob(image);

  if(logging)
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(), "return");
    }

  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r P S D I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterPSDImage adds attributes for the PSD image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterPSDImage method is:
%
%      RegisterPSDImage(void)
%
*/
ModuleExport void RegisterPSDImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("PSD");
  entry->decoder=(DecoderHandler) ReadPSDImage;
  entry->encoder=(EncoderHandler) WritePSDImage;
  entry->magick=(MagickHandler) IsPSD;
  entry->description="Adobe Photoshop bitmap";
  entry->module="PSD";
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r P S D I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterPSDImage removes format registrations made by the
%  PSD module from the list of supported formats.
%
%  The format of the UnregisterPSDImage method is:
%
%      UnregisterPSDImage(void)
%
*/
ModuleExport void UnregisterPSDImage(void)
{
  (void) UnregisterMagickInfo("PSD");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P S D I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePSDImage writes an image in the Adobe Photoshop encoded image
%  format.
%
%  The format of the WritePSDImage method is:
%
%      unsigned int WritePSDImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WritePSDImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/

static void WriteOneChannel( Image* image, Image* tmp_image,
               unsigned char *pixels, QuantumType whichQuantum )
{
  int
    y;

  register const PixelPacket
    *p;

  unsigned int
    packet_size,
    quantum_size;

  if (tmp_image->depth <= 8)
    quantum_size=8;
  else
    quantum_size=16;

  if (tmp_image->depth > 16)
    tmp_image->depth=16;

  packet_size=quantum_size/8;

  for (y=0; y < (long) tmp_image->rows; y++)
    {
      p=AcquireImagePixels(tmp_image,0,y,tmp_image->columns,1,&image->exception);
      if (p == (const PixelPacket *) NULL)
        break;
      (void) ExportImagePixelArea(tmp_image,whichQuantum,quantum_size,pixels,0,0);
      (void) WriteBlob(image,packet_size*tmp_image->columns,pixels);
    }
}

static void WriteImageChannels( Image* image, Image* tmp_image, unsigned char *pixels )
{
  /*
    Write uncompressed pixel data as separate planes.
  */
  (void) WriteBlobMSBShort(image,0);  /* compression */
  if (tmp_image->storage_class == PseudoClass)
    {
      if (!tmp_image->matte)
        WriteOneChannel( image, tmp_image, pixels, IndexQuantum );
      else
        WriteOneChannel( image, tmp_image, pixels, IndexAlphaQuantum );
    }
  else
    {
      if (tmp_image->matte)
        WriteOneChannel( image, tmp_image, pixels, AlphaQuantum );

      WriteOneChannel( image, tmp_image, pixels, RedQuantum );
      WriteOneChannel( image, tmp_image, pixels, GreenQuantum );
      WriteOneChannel( image, tmp_image, pixels, BlueQuantum );

      if (tmp_image->colorspace == CMYKColorspace)
        WriteOneChannel( image, tmp_image, pixels, BlackQuantum );
    }
}

/* Write white background, RLE-compressed */

static void WriteWhiteBackground( Image* image )
{
  long
    count,
    w8,
    w;

  char
    *d,
    scanline[256];

  int numChannels = 3, i, bytecount, dim = (int) (image->rows*numChannels);

  (void) WriteBlobMSBShort( image, 1 ); /* RLE compressed */

  w8 = (long) image->columns;

  d = scanline;
  /* Set up scanline */
  for (w=w8; w>128; w-=128)
    {
      *d++ = -127;
      *d++ = (char) 255;
    }

  switch(w)
    {
    case 0:
      break;
    case 1:
      *d++=0;
      *d++ = (char)255;
      break;
    default: *d++=(char) (1-w);     *d++ = (char)255;
      break;
    }

  bytecount = d - scanline;

  /* Scanline counts (rows*channels) */
  for (i=0; i < dim; i++)
    {
      (void) WriteBlobMSBShort( image, bytecount );
    }

  /* RLE compressed data  */
  count = bytecount;
  for(i=0; i < dim; i++)
    {
      (void) WriteBlob( image, count, scanline );
    }
}

static void WritePascalString (Image* inImage, const char *inString, int inPad)
{
  unsigned
    char strLength;

  int
    i;

  /* max length is 255 */

  strLength = ( strlen(inString) > 255 ) ? 255 : (unsigned char) strlen(inString);

  if ( strLength !=  0 )
    {
      (void) WriteBlobByte(inImage, strLength);
      (void) WriteBlob(inImage, strLength, inString);
    }
  else
    (void) WriteBlobByte(inImage, 0);

  strLength ++;

  if ( (strLength % inPad) == 0 )
    return;

  for ( i=0; i < (inPad - (strLength % inPad)); i++)
    (void) WriteBlobByte(inImage, 0);
}

static unsigned int WritePSDImage(const ImageInfo *image_info,Image *image)
{
  int
    layer_count,
    num_channels,
    layer_info_size,
    rounded_layer_info_size,
    channel_size,
    channelLength,
    force_white_background = image->matte,
    invert_layer_count = False;

  register long
    i;

  unsigned char
    *pixels,
    layer_name[4];

  unsigned int
    packet_size,
    status;

  ImageAttribute
    *theAttr;

  Image
    * tmp_image = (Image *) NULL,
    * base_image = force_white_background ? image : image->next;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenError,UnableToOpenFile,image);
  packet_size=image->depth > 8 ? 6 : 3;
  if (image->matte)
    packet_size+=image->depth > 8 ? 2 : 1;
  pixels=MagickAllocateMemory(unsigned char *,
                              packet_size*image->columns*sizeof(PixelPacket));
  if (pixels == (unsigned char *) NULL)
    ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
  (void) WriteBlob(image,4,"8BPS");
  (void) WriteBlobMSBShort(image,1);  /* version */
  for ( i=1; i<=6; i++)
    (void) WriteBlobByte(image, 0);  /* 6 bytes of reserved */
  if ( force_white_background )
    num_channels = 3;
  else
    {
      if (image->storage_class == PseudoClass)
        num_channels = (image->matte ? 2 : 1);
      else
        {
          if (image->colorspace != CMYKColorspace)
            num_channels = (image->matte ? 4 : 3);
          else
            num_channels = (image->matte ? 5 : 4);
        }
    }
  (void) WriteBlobMSBShort(image,num_channels);
  (void) WriteBlobMSBLong(image,image->rows);
  (void) WriteBlobMSBLong(image,image->columns);
  (void) WriteBlobMSBShort(image,
                           (image->storage_class == PseudoClass ? 8 :
                            image->depth > 8 ? 16 : 8));
  if (((image_info->colorspace != UndefinedColorspace) ||
       (image->colorspace != CMYKColorspace)) &&
      (image_info->colorspace != CMYKColorspace))
    {
      (void) TransformColorspace(image,RGBColorspace);
      (void) WriteBlobMSBShort(image,
                               image->storage_class == PseudoClass ? 2 : 3);
    }
  else
    {
      (void) TransformColorspace(image,CMYKColorspace);
      (void) WriteBlobMSBShort(image,4);
    }
  if ((image->storage_class == DirectClass) || (image->colors > 256))
    (void) WriteBlobMSBLong(image,0);
  else
    {
      /*
        Write PSD raster colormap.
      */
      (void) WriteBlobMSBLong(image,768);
      for (i=0; i < (long) image->colors; i++)
        (void) WriteBlobByte(image,ScaleQuantumToChar(image->colormap[i].red));
      for ( ; i < 256; i++)
        (void) WriteBlobByte(image,0);
      for (i=0; i < (long) image->colors; i++)
        (void) WriteBlobByte(image,ScaleQuantumToChar(image->colormap[i].green));
      for ( ; i < 256; i++)
        (void) WriteBlobByte(image,0);
      for (i=0; i < (long) image->colors; i++)
        (void) WriteBlobByte(image,ScaleQuantumToChar(image->colormap[i].blue));
      for ( ; i < 256; i++)
        (void) WriteBlobByte(image,0);
    }

  /* image resource block */
  {
    const unsigned char
      *iptc_profile;
    
    size_t
      iptc_profile_length;
    
    iptc_profile=GetImageProfile(image,"IPTC",&iptc_profile_length);
    
    if ( iptc_profile != 0 )
      {
        (void) WriteBlobMSBLong( image, iptc_profile_length );
        (void) WriteBlob( image, iptc_profile_length, iptc_profile );
      }
    else
      (void) WriteBlobMSBLong(image,0);
  }

 compute_layer_info:
  layer_count = 0;
  layer_info_size = 2;
  tmp_image = base_image;
  while ( tmp_image != NULL ) {
    packet_size=tmp_image->depth > 8 ? 2 : 1;

    if (tmp_image->storage_class == PseudoClass)
      num_channels = (tmp_image->matte ? 2 : 1);
    else
      if (tmp_image->colorspace != CMYKColorspace)
        num_channels = (tmp_image->matte ? 4 : 3);
      else
        num_channels = (tmp_image->matte ? 5 : 4);

    channelLength = (int) (tmp_image->columns * tmp_image->rows * packet_size + 2);
    layer_info_size += (4*4 + 2 + num_channels * 6 + 4 + 4 + 4 * 1 + 4 + 12
                        + num_channels * channelLength);

    layer_count++;
    tmp_image = tmp_image->next;
  }

  /* if the image has a matte, then we need to use layers */
  if ( layer_count == 0 && image->matte == True )
    {
      invert_layer_count = True;
      base_image = image;
      goto compute_layer_info;  /* yes, goto's suck, but it keeps the code cleaner! */
    }

  if ( layer_count == 0 )
    (void) WriteBlobMSBLong(image, 0);
  else
    {
      (void) WriteBlobMSBLong(image,layer_info_size + 4 + 4);
      if( layer_info_size/2 != (layer_info_size+1)/2 ) /* odd */
        rounded_layer_info_size = layer_info_size + 1;
      else
        rounded_layer_info_size = layer_info_size;
      (void) WriteBlobMSBLong(image,rounded_layer_info_size);

      if ( invert_layer_count )
        layer_count *= -1;  /* if we have a matte, then use negative count! */
      (void) WriteBlobMSBShort(image, layer_count);
  
      layer_count = 1;
      tmp_image = base_image;
      while ( tmp_image != NULL ) {
        (void) WriteBlobMSBLong(image,0);
        (void) WriteBlobMSBLong(image,0);
        (void) WriteBlobMSBLong(image,tmp_image->rows);
        (void) WriteBlobMSBLong(image,tmp_image->columns);

        packet_size=tmp_image->depth > 8 ? 2 : 1;
        channel_size = (int) ((packet_size * tmp_image->rows * tmp_image->columns) + 2);
        if (tmp_image->storage_class == PseudoClass) {
          (void) WriteBlobMSBShort(image, tmp_image->matte ? 2 : 1);
          if (tmp_image->matte) {
            (void) WriteBlobMSBShort(image, (magick_uint16_t) -1);
            (void) WriteBlobMSBLong(image, channel_size);
          }
          (void) WriteBlobMSBShort(image, 0);
          (void) WriteBlobMSBLong(image, channel_size);
        } else
          if (tmp_image->colorspace != CMYKColorspace)
            {
              (void) WriteBlobMSBShort(image, tmp_image->matte ? 4 : 3);
              if (tmp_image->matte) {
                (void) WriteBlobMSBShort(image, (magick_uint16_t) -1);
                (void) WriteBlobMSBLong(image, channel_size);
              }
              (void) WriteBlobMSBShort(image, 0);
              (void) WriteBlobMSBLong(image, channel_size);
              (void) WriteBlobMSBShort(image, 1);
              (void) WriteBlobMSBLong(image, channel_size);
              (void) WriteBlobMSBShort(image, 2);
              (void) WriteBlobMSBLong(image, channel_size);
            }
          else
            {
              (void) WriteBlobMSBShort(image, tmp_image->matte ? 5 : 4);
              if (tmp_image->matte) {
                (void) WriteBlobMSBShort(image, (magick_uint16_t) -1);
                (void) WriteBlobMSBLong(image, channel_size);
              }
              (void) WriteBlobMSBShort(image, 0);
              (void) WriteBlobMSBLong(image, channel_size);
              (void) WriteBlobMSBShort(image, 1);
              (void) WriteBlobMSBLong(image, channel_size);
              (void) WriteBlobMSBShort(image, 2);
              (void) WriteBlobMSBLong(image, channel_size);
              (void) WriteBlobMSBShort(image, 3);
              (void) WriteBlobMSBLong(image, channel_size);
            }
 
        (void) WriteBlob(image, 4, "8BIM");
        (void) WriteBlob(image, 4, CompositeOperatorToPSDBlendMode(tmp_image->compose));
        (void) WriteBlobByte(image, 255);    /* BOGUS: layer opacity */
        (void) WriteBlobByte(image, 0);
        (void) WriteBlobByte(image, 1);    /* BOGUS: layer attributes - visible, etc. */
        (void) WriteBlobByte(image, 0);

        (void) WriteBlobMSBLong(image, 12);
        (void) WriteBlobMSBLong(image, 0);
        (void) WriteBlobMSBLong(image, 0);

        theAttr = (ImageAttribute *)GetImageAttribute(tmp_image, "[layer-name]");
        if (theAttr) {
          WritePascalString( image, theAttr->value, 4 );
          /*
            sprintf((char *) &(layer_name[1]), "%4s", theAttr->value );
            (void) WriteBlobByte(image, 3);
            (void) WriteBlob(image, 3, &layer_name[1]);
          */ 
        } else {
          (void) sprintf((char *) layer_name, "L%02d", layer_count++ );
          WritePascalString( image, (char*)layer_name, 4 );
        }
        tmp_image = tmp_image->next;
      };

      /* now the image data! */
      tmp_image = base_image;
      while ( tmp_image != NULL ) {
        WriteImageChannels( image, tmp_image, pixels );

        /* add in the pad! */
        if ( rounded_layer_info_size != layer_info_size )
          (void) WriteBlobByte(image, 0);

        tmp_image = tmp_image->next;
      };

      /* user mask data */
      (void) WriteBlobMSBLong(image, 0);

    }

  /* now the background image data! */
  if ( force_white_background )
    WriteWhiteBackground( image );
  else
    WriteImageChannels( image, image, pixels );

  MagickFreeMemory(pixels);
  CloseBlob(image);
  return(True);
}
