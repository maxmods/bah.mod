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
%              PPPP   RRRR   EEEEE  V   V  IIIII  EEEEE  W   W                %
%              P   P  R   R  E      V   V    I    E      W   W                %
%              PPPP   RRRR   EEE    V   V    I    EEE    W   W                %
%              P      R R    E       V V     I    E      W W W                %
%              P      R  R   EEEEE    V    IIIII  EEEEE   W W                 %
%                                                                             %
%                                                                             %
%                           Write A Preview Image.                            %
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
#include "magick/color.h"
#include "magick/constitute.h"
#include "magick/decorate.h"
#include "magick/effect.h"
#include "magick/enhance.h"
#include "magick/fx.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/montage.h"
#include "magick/quantize.h"
#include "magick/resize.h"
#include "magick/shear.h"
#include "magick/tempfile.h"
#include "magick/transform.h"
#include "magick/utility.h"

/*
  Constant declarations.
*/
static const char
  *DefaultPreviewGeometry = "204x204+10+10";

/*
  Forward declarations.
*/
static unsigned int
  WritePreviewImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r P R E V I E W I m a g e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterPREVIEWImage adds attributes for the Preview image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterPREVIEWImage method is:
%
%      RegisterPREVIEWImage(void)
%
*/
ModuleExport void RegisterPREVIEWImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("PREVIEW");
  entry->encoder=(EncoderHandler) WritePreviewImage;
  entry->adjoin=False;
  entry->description="Show a preview an image enhancement, effect, or f/x";
  entry->module="PREVIEW";
  entry->extension_treatment=IgnoreExtensionTreatment;
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r P R E V I E W I m a g e                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterPREVIEWImage removes format registrations made by the
%  PREVIEW module from the list of supported formats.
%
%  The format of the UnregisterPREVIEWImage method is:
%
%      UnregisterPREVIEWImage(void)
%
*/
ModuleExport void UnregisterPREVIEWImage(void)
{
  (void) UnregisterMagickInfo("PREVIEW");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P R E V I E W I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePreviewImage creates several tiles each with a varying
%  stength of an image enhancement function (e.g. gamma).  The image is written
%  in the MIFF format.
%
%  The format of the WritePreviewImage method is:
%
%      unsigned int WritePreviewImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WritePreviewImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
#define ReplaceImage(oldimage,func) \
{ \
  Image \
    *temporary_image; \
\
  temporary_image=func; \
  if (temporary_image) \
  { \
    DestroyImage(oldimage); \
    oldimage=temporary_image; \
  } \
}
static unsigned int WritePreviewImage(const ImageInfo *image_info,Image *image)
{
#define NumberTiles  9
#define PreviewImageText  "[%s] Creating image preview..."

  char
    factor[MaxTextExtent],
    label[MaxTextExtent];

  float
    degrees,
    gamma,
    percentage,
    radius,
    sigma,
    threshold;

  Image
    *images,
    *montage_image,
    *preview_image,
    *master_image;

  ImageInfo
    *clone_info;

  long
    y;

  MonitorHandler
    handler;

  MontageInfo
    *montage_info;

  RectangleInfo
    geometry;

  register long
    i,
    x;

  unsigned int
    status;

  unsigned long
    colors;

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
  CloseBlob(image);
  (void) TransformColorspace(image,RGBColorspace);
  clone_info=CloneImageInfo(image_info);
  clone_info->quality=0;
  colors=2;
  degrees=0;
  gamma=(-0.2f);
  SetGeometry(image,&geometry);
  (void) GetMagickGeometry(DefaultPreviewGeometry,&geometry.x,&geometry.y,
    &geometry.width,&geometry.height);
  images=NewImageList();
  percentage=12.5;
  radius=0.0;
  sigma=1.0;
  threshold=0.0;
  x=0;
  y=0;
  master_image=ThumbnailImage(image,geometry.width,geometry.height,
                              &image->exception);
  if (master_image == (Image *) NULL)
    return False;
  for (i=0; i < NumberTiles; i++)
  {
    preview_image=CloneImage(master_image,0,0,True,&image->exception);
    if (preview_image == (Image *) NULL)
      break;
    (void) SetImageAttribute(preview_image,"label",DefaultTileLabel);
    if (i == (NumberTiles >> 1))
      {
        (void) QueryColorDatabase("#dfdfdf",&preview_image->matte_color,
          &image->exception);
        AppendImageToList(&images,preview_image);
        continue;
      }
    handler=SetMonitorHandler((MonitorHandler) NULL);
    switch (image_info->preview_type)
    {
      case RotatePreview:
      {
        FormatString(factor,"%.1f",degrees+=45.0);
        FormatString(label,"rotate %.1024s",factor);
        ReplaceImage(preview_image,RotateImage(preview_image,degrees,
          &image->exception));
        break;
      }
      case ShearPreview:
      {
        degrees+=10.0;
        FormatString(factor,"%.1fx%.1f",degrees,2.0*degrees);
        FormatString(label,"shear %.1024s",factor);
        ReplaceImage(preview_image,ShearImage(preview_image,degrees,
          2.0*degrees,&image->exception));
        break;
      }
      case RollPreview:
      {
        x=(long) ((i+1)*preview_image->columns)/NumberTiles;
        y=(long) ((i+1)*preview_image->rows)/NumberTiles;
        FormatString(factor,"%+ld%+ld",x,y);
        FormatString(label,"roll %.1024s",factor);
        ReplaceImage(preview_image,RollImage(preview_image,x,y,
          &image->exception));
        break;
      }
      case HuePreview:
      {
        FormatString(factor,"100/100/%g",2.0*percentage);
        FormatString(label,"modulate %.1024s",factor);
        (void) ModulateImage(preview_image,factor);
        break;
      }
      case SaturationPreview:
      {
        FormatString(factor,"100/%g",2.0*percentage);
        FormatString(label,"modulate %.1024s",factor);
        (void) ModulateImage(preview_image,factor);
        break;
      }
      case BrightnessPreview:
      {
        FormatString(factor,"%g",2.0*percentage);
        FormatString(label,"modulate %.1024s",factor);
        (void) ModulateImage(preview_image,factor);
        break;
      }
      case GammaPreview:
      default:
      {
        FormatString(factor,"%g",gamma+=0.4f);
        FormatString(label,"gamma %.1024s",factor);
        (void) GammaImage(preview_image,factor);
        break;
      }
      case SpiffPreview:
      {
        for (x=0; x < i; x++)
          (void) ContrastImage(preview_image,True);
        FormatString(label,"-contrast %ld",i+1);
        break;
      }
      case DullPreview:
      {
        for (x=0; x < i; x++)
          (void) ContrastImage(preview_image,False);
        FormatString(label,"+contrast %ld",i+1);
        break;
      }
      case GrayscalePreview:
      {
        QuantizeInfo
          quantize_info;

        GetQuantizeInfo(&quantize_info);
        FormatString(factor,"%lu",colors);
        FormatString(label,"colors %.1024s",factor);
        (void) TransformColorspace(preview_image,GRAYColorspace);
        quantize_info.number_colors=colors;
        quantize_info.colorspace=GRAYColorspace;
        quantize_info.dither=image_info->dither;
        quantize_info.tree_depth=8;
        (void) QuantizeImage(&quantize_info,preview_image);
        colors<<=1;
        break;
      }
      case QuantizePreview:
      {
        QuantizeInfo
          quantize_info;

        GetQuantizeInfo(&quantize_info);
        FormatString(factor,"%lu",colors);
        FormatString(label,"colors %.1024s",factor);
        quantize_info.number_colors=colors;
        quantize_info.colorspace=preview_image->colorspace;
        quantize_info.dither=image_info->dither;
        /* quantize_info.tree_depth=8; */
        (void) QuantizeImage(&quantize_info,preview_image);
        colors<<=1;
        break;
      }
      case DespecklePreview:
      {
        for (x=0; x < i; x++)
          {
            ReplaceImage(preview_image,DespeckleImage(preview_image,
              &image->exception));
          }
        FormatString(label,"despeckle %ld",i+1);
        break;
      }
      case ReduceNoisePreview:
      {
        FormatString(factor,"%gx%g",radius,sigma);
        FormatString(label,"noise %.1024s",factor);
        ReplaceImage(preview_image,ReduceNoiseImage(preview_image,radius,
          &image->exception));
        break;
      }
      case AddNoisePreview:
      {
        NoiseType
          noise;

        switch ((int) x)
        {
          case 0:
            (void) strcpy(factor,"uniform");
            noise=UniformNoise;
            break;
          case 1:
            (void) strcpy(factor,"Gaussian");
            noise=GaussianNoise;
            break;
          case 2:
            (void) strcpy(factor,"multiplicative");
            noise=MultiplicativeGaussianNoise;
            break;
          case 3:
            (void) strcpy(factor,"impulse");
            noise=ImpulseNoise;
            break;
          case 4:
            (void) strcpy(factor,"laplacian");
            noise=LaplacianNoise;
            break;
          case 5:
            (void) strcpy(factor,"Poisson");
            noise=PoissonNoise;
            break;
          default:
            (void) strcpy(preview_image->magick,"NULL");
            noise=UniformNoise;
            break;
        }
        x++;
        FormatString(label,"+noise %.1024s",factor);
        ReplaceImage(preview_image,AddNoiseImage(preview_image,noise,
          &image->exception));
        break;
      }
      case SharpenPreview:
      {
        FormatString(factor,"%gx%g",radius,sigma);
        FormatString(label,"sharpen %.1024s",factor);
        ReplaceImage(preview_image,SharpenImage(preview_image,radius,sigma,
          &image->exception));
        break;
      }
      case BlurPreview:
      {
        FormatString(factor,"%gx%g",radius,sigma);
        FormatString(label,"-blur %.1024s",factor);
        ReplaceImage(preview_image,BlurImage(preview_image,radius,sigma,
          &image->exception));
        break;
      }
      case ThresholdPreview:
      {
        FormatString(factor,"%lu",(unsigned long)
          ((percentage*((double) MaxRGB+1.0))/100));
        FormatString(label,"threshold %.1024s",factor);
        (void ) ThresholdImage(preview_image,
          (percentage*((double) MaxRGB+1.0))/100);
        break;
      }
      case EdgeDetectPreview:
      {
        FormatString(factor,"%gx%g",radius,sigma);
        FormatString(label,"edge %.1024s",factor);
        ReplaceImage(preview_image,EdgeImage(preview_image,radius,
          &image->exception));
        break;
      }
      case SpreadPreview:
      {
        FormatString(factor,"%ld",i+1);
        FormatString(label,"spread %.1024s",factor);
        ReplaceImage(preview_image,SpreadImage(preview_image,i+1,
          &image->exception));
        break;
      }
      case SolarizePreview:
      {
        FormatString(factor,"%g",percentage);
        FormatString(label,"solarize %.1024s",factor);
        (void) SolarizeImage(preview_image,
                             (percentage*((double) MaxRGB+1.0))/100);
        break;
      }
      case ShadePreview:
      {
        if (i == 0)
          {
            FormatString(factor,"30x30");
            FormatString(label,"+shade %.1024s",factor);
            ReplaceImage(preview_image,ShadeImage(preview_image,False,30,30,
              &image->exception));
            break;
          }
        degrees+=10.0;
        FormatString(factor,"%gx%g",degrees,degrees);
        FormatString(label,"shade %.1024s",factor);
        ReplaceImage(preview_image,ShadeImage(preview_image,True,degrees,
          degrees,&image->exception));
        break;
      }
      case RaisePreview:
      {
        RectangleInfo
          raise_info;

        raise_info.width=2*i+2;
        raise_info.height=2*i+2;
        raise_info.x=0;
        raise_info.y=0;
        FormatString(factor,"%ldx%ld",2*i+2,2*i+2);
        FormatString(label,"raise %.1024s",factor);
        (void) RaiseImage(preview_image,&raise_info,True);
        break;
      }
      case SegmentPreview:
      {
        threshold+=0.4f;
        FormatString(factor,"%.1fx%.1f",threshold,threshold);
        FormatString(label,"segment %.1024s",factor);
        (void) SegmentImage(preview_image,preview_image->colorspace,False,
                            threshold,threshold);
        break;
      }
      case SwirlPreview:
      {
        FormatString(factor,"%.1f",degrees);
        FormatString(label,"swirl %.1024s",factor);
        ReplaceImage(preview_image,SwirlImage(preview_image,degrees,
          &image->exception));
        degrees+=45.0;
        break;
      }
      case ImplodePreview:
      {
        FormatString(factor,"%.1f",percentage/100.0);
        FormatString(label,"implode %.1024s",factor);
        ReplaceImage(preview_image,ImplodeImage(preview_image,
          percentage/100.0,&image->exception));
        break;
      }
      case WavePreview:
      {
        degrees+=5.0;
        FormatString(factor,"%.1fx%.1f",0.5*degrees,2.0*degrees);
        FormatString(label,"wave %.1024s",factor);
        ReplaceImage(preview_image,WaveImage(preview_image,0.5*degrees,
          2.0*degrees,&image->exception));
        break;
      }
      case OilPaintPreview:
      {
        FormatString(factor,"%g",0.5*(i+1));
        FormatString(label,"paint %.1024s",factor);
        ReplaceImage(preview_image,OilPaintImage(preview_image,0.5*(i+1),
          &image->exception));
        break;
      }
      case CharcoalDrawingPreview:
      {
        FormatString(factor,"%gx%g",radius,sigma);
        FormatString(label,"charcoal %.1024s",factor);
        ReplaceImage(preview_image,CharcoalImage(preview_image,radius,sigma,
          &image->exception));
        break;
      }
      case JPEGPreview:
      {
        char
          filename[MaxTextExtent];

        clone_info->quality=(unsigned int) (percentage+13.0);
        FormatString(factor,"%lu",clone_info->quality);
        if(!AcquireTemporaryFileName(filename))
          {
            DestroyImage(master_image);
            DestroyImageInfo(clone_info);
            ThrowWriterTemporaryFileException(filename);
          }
        FormatString(preview_image->filename,"jpeg:%.1024s",filename);
        status=WriteImage(clone_info,preview_image);
        if (status != False)
          {
            Image
              *quality_image;

            (void) strlcpy(clone_info->filename,preview_image->filename,
              MaxTextExtent);
            quality_image=ReadImage(clone_info,&image->exception);
            if (quality_image != (Image *) NULL)
              {
                DestroyImage(preview_image);
                preview_image=quality_image;
              }
          }
        (void) LiberateTemporaryFile(filename);
        if ((GetBlobSize(preview_image)/1024) >= 1024)
          FormatString(label,"quality %.1024s\n%gmb ",factor,
            (double) GetBlobSize(preview_image)/1024.0/1024.0);
        else
          if (GetBlobSize(preview_image) >= 1024)
            FormatString(label,"quality %.1024s\n%gkb ",factor,
              (double) GetBlobSize(preview_image)/1024.0);
          else
            FormatString(label,"quality %.1024s\n%lub ",factor,
              (unsigned long) GetBlobSize(preview_image));
        break;
      }
    }
    percentage+=12.5;
    radius+=0.5;
    sigma+=0.25;
    (void) SetImageAttribute(preview_image,"label",(char *) NULL);
    (void) SetImageAttribute(preview_image,"label",label);
    (void) SetMonitorHandler(handler);
    AppendImageToList(&images,preview_image);
    if (!MagickMonitorFormatted(i,NumberTiles,&image->exception,
                                PreviewImageText,image->filename))
      break;
  }
  DestroyImage(master_image);
  DestroyImageInfo(clone_info);
  if (images == (Image *) NULL)
    return(False);
  /*
    Create the montage.
  */
  montage_info=CloneMontageInfo(image_info,(MontageInfo *) NULL);
  (void) strlcpy(montage_info->filename,image->filename,MaxTextExtent);
  montage_info->shadow=True;
  (void) CloneString(&montage_info->tile,"3x3");
  (void) CloneString(&montage_info->geometry,DefaultPreviewGeometry);
  (void) CloneString(&montage_info->frame,DefaultTileFrame);
  montage_image=MontageImages(images,montage_info,&image->exception);
  DestroyMontageInfo(montage_info);
  DestroyImageList(images);
  if (montage_image == (Image *) NULL)
    ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
  if (montage_image->montage != (char *) NULL)
    {
      /*
        Free image directory.
      */
      MagickFreeMemory(montage_image->montage);
      montage_image->montage=(char *) NULL;
      if (image->directory != (char *) NULL)
        {
          MagickFreeMemory(montage_image->directory);
          montage_image->directory=(char *) NULL;
        }
    }
  FormatString(montage_image->filename,"miff:%.1024s",image_info->filename);
  status=WriteImage(image_info,montage_image);
  DestroyImage(montage_image);
  return(status);
}
