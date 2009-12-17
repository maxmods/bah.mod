/*
% Copyright (C) 2003 - 2009 GraphicsMagick Group
% Copyright (C) 2003 ImageMagick Studio
% Copyright 1991-1999 E. I. du Pont de Nemours and Company
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
% GraphicsMagick Describe Methods.
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/analyze.h"
#include "magick/attribute.h"
#include "magick/blob.h"
#include "magick/channel.h"
#include "magick/color.h"
#include "magick/color_lookup.h"
#include "magick/constitute.h"
#include "magick/enum_strings.h"
#include "magick/magick.h"
#include "magick/pixel_cache.h"
#include "magick/profile.h"
#include "magick/signature.h"
#include "magick/statistics.h"
#include "magick/utility.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s c r i b e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DescribeImage() describes an image by printing its attributes to the file.
%  Attributes include the image width, height, size, and others.
%
%  The format of the DescribeImage method is:
%
%      void DescribeImage(Image *image,FILE *file,const MagickBool verbose)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o file: The file, typically stdout.
%
%    o verbose: A value other than zero prints more detailed information
%      about the image. Values greater than one enable counting the number of
%      colors in the image.
%
%
*/
MagickExport MagickPassFail DescribeImage(Image *image,FILE *file,
  const MagickBool verbose)
{
  char
    color[MaxTextExtent],
    format[MaxTextExtent];

  const unsigned char
    *profile;

  size_t
    profile_length;

  const ImageAttribute
    *attribute;

  const MagickInfo
    *magick_info;

  double
    elapsed_time,
    user_time;

  unsigned long
    columns,
    rows;

  magick_int64_t
    pixels_per_second;

  Image
    *p;

  long
    y;

  register long
    i,
    x;

  unsigned long
    count;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(file != (FILE *) NULL);
  elapsed_time=GetElapsedTime(&image->timer);
  user_time=GetUserTime(&image->timer);
  GetTimerInfo(&image->timer);
  if (!verbose)
    {
      /*
        Display summary info about the image.
      */
      if (*image->magick_filename != '\0')
        if (LocaleCompare(image->magick_filename,image->filename) != 0)
          (void) fprintf(file,"%.1024s=>",image->magick_filename);
       if ((image->previous == (Image *) NULL) &&
           (image->next == (Image *) NULL) && (image->scene == 0))
        (void) fprintf(file,"%.1024s ",image->filename);
      else
        (void) fprintf(file,"%.1024s[%lu] ",image->filename,image->scene);
      (void) fprintf(file,"%.1024s ",image->magick);
      columns=image->columns;
      rows=image->rows;
      if ((image->magick_columns != 0) || (image->magick_rows != 0))
        if ((image->magick_columns != image->columns) ||
            (image->magick_rows != image->rows))
          {
            columns=image->magick_columns;
            rows=image->magick_rows;
            (void) fprintf(file,"%lux%lu=>",image->magick_columns,
                           image->magick_rows);
          }
      (void) fprintf(file,"%lux%lu%+ld%+ld ",image->columns,image->rows,
        image->page.x,image->page.y);
      if (image->storage_class == DirectClass)
        {
          (void) fprintf(file,"DirectClass ");
          if (image->total_colors != 0)
            {
              FormatSize(image->total_colors,format);
              (void) fprintf(file,"%.1024s ",format);
            }
        }
      else
        if (image->total_colors <= image->colors)
          (void) fprintf(file,"PseudoClass %uc ",image->colors);
        else
          {
            (void) fprintf(file,"PseudoClass %lu=>%uc ",image->total_colors,
              image->colors);
            (void) fprintf(file,"%ld/%.6f/%.6fe ",
              (long) image->error.mean_error_per_pixel,
              image->error.normalized_mean_error,
              image->error.normalized_maximum_error);
          }
      (void) fprintf(file,"%u-bit ",image->depth);
      if (GetBlobSize(image) != 0)
        {
          FormatSize(GetBlobSize(image),format);
          (void) fprintf(file,"%.1024s ",format);
        }
      (void) fprintf(file,"%0.3fu %ld:%02ld",user_time,
                     (long) (elapsed_time/60.0),
                     (long) ceil(fmod(elapsed_time,60.0)));
      /*
        Only display pixel read rate if the time accumulated is at
        least six times the timer's resolution (typically 0.01 on
        Unix).
      */
      if (elapsed_time >= GetTimerResolution()*6)
        {
          pixels_per_second=(magick_int64_t) ((double) rows*columns/ elapsed_time);
          FormatSize(pixels_per_second,format);
          (void) fprintf(file," (%s pixels/s)",format);
        }
      (void) fprintf(file,"\n");
    
      return (ferror(file) ? MagickFail : MagickPass);
    }
  /*
    Display verbose info about the image.
  */
  (void) SignatureImage(image);
  if (verbose > 1)
    image->total_colors=GetNumberColors(image,(FILE *) NULL,&image->exception);
  (void) fprintf(file,"Image: %.1024s\n",image->filename);
  magick_info=GetMagickInfo(image->magick,&image->exception);
  if ((magick_info == (const MagickInfo *) NULL) ||
      (*magick_info->description == '\0'))
    (void) fprintf(file,"  Format: %.1024s\n",image->magick);
  else
    (void) fprintf(file,"  Format: %.1024s (%.1024s)\n",image->magick,
      magick_info->description);
  (void) fprintf(file,"  Geometry: %lux%lu\n",image->columns,image->rows);
  if (image->storage_class == DirectClass)
    (void) fprintf(file,"  Class: DirectClass\n");
  else
    (void) fprintf(file,"  Class: PseudoClass\n");
  if ((image->magick_columns != 0) || (image->magick_rows != 0))
    if ((image->magick_columns != image->columns) ||
        (image->magick_rows != image->rows))
      (void) fprintf(file,"  Base geometry: %lux%lu\n",image->magick_columns,
        image->magick_rows);
  (void) fprintf(file,"  Type: ");
  switch (GetImageType(image,&image->exception))
  {
    case BilevelType: (void) fprintf(file,"bilevel"); break;
    case GrayscaleType: (void) fprintf(file,"grayscale"); break;
    case GrayscaleMatteType:
      (void) fprintf(file,"grayscale with transparency"); break;
    case PaletteType: (void) fprintf(file,"palette"); break;
    case PaletteMatteType:
      (void) fprintf(file,"palette with transparency"); break;
    case TrueColorType: (void) fprintf(file,"true color"); break;
    case TrueColorMatteType:
      (void) fprintf(file,"true color with transparency"); break;
    case ColorSeparationType: (void) fprintf(file,"color separated"); break;
    case ColorSeparationMatteType:
      (void) fprintf(file,"color separated with transparency"); break;
    default: (void) fprintf(file,"undefined"); break;
  }
  (void) fprintf(file,"\n");
  (void) fprintf(file,"  Depth: %lu bits-per-pixel component\n",
    GetImageDepth(image,&image->exception));
  (void) fprintf(file,"  Channel Depths:\n");
  if (image->colorspace == CMYKColorspace)
    {
      (void) fprintf(file,"    Cyan:     %u bits\n",
                     GetImageChannelDepth(image,CyanChannel,&image->exception));
      (void) fprintf(file,"    Magenta:  %u bits\n",
                     GetImageChannelDepth(image,MagentaChannel,&image->exception));
      (void) fprintf(file,"    Yellow:   %u bits\n",
                     GetImageChannelDepth(image,YellowChannel,&image->exception));
      (void) fprintf(file,"    Black:    %u bits\n",
                     GetImageChannelDepth(image,BlackChannel,&image->exception));
    }
  else if ((IsGrayColorspace(image->colorspace)) ||
           (image->is_grayscale == True))
    {
      (void) fprintf(file,"    Gray:     %u bits\n",
                     GetImageChannelDepth(image,RedChannel,&image->exception));
    }
  else
    {
      (void) fprintf(file,"    Red:      %u bits\n",
                     GetImageChannelDepth(image,RedChannel,&image->exception));
      (void) fprintf(file,"    Green:    %u bits\n",
                     GetImageChannelDepth(image,GreenChannel,&image->exception));
      (void) fprintf(file,"    Blue:     %u bits\n",
                     GetImageChannelDepth(image,BlueChannel,&image->exception));
    }
  if (image->matte)
    (void) fprintf(file,"    Opacity:  %u bits\n",
                   GetImageChannelDepth(image,OpacityChannel,&image->exception));
  (void) fprintf(file,"  Channel Statistics:\n");
  {
    ImageStatistics
      statistics;

    (void) GetImageStatistics(image,&statistics,&image->exception);

    if (image->colorspace == CMYKColorspace)
    {
      (void) fprintf(file,"    Cyan:\n");
      (void) fprintf(file,"      Minimum:            %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.red.minimum,
                     statistics.red.minimum);
      (void) fprintf(file,"      Maximum:            %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.red.maximum,
                     statistics.red.maximum);
      (void) fprintf(file,"      Mean:               %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.red.mean,
                     statistics.red.mean);
      (void) fprintf(file,"      Standard Deviation: %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.red.standard_deviation,
                     statistics.red.standard_deviation);
      (void) fprintf(file,"    Magenta:\n");
      (void) fprintf(file,"      Minimum:            %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.green.minimum,
                     statistics.green.minimum);
      (void) fprintf(file,"      Maximum:            %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.green.maximum,
                     statistics.green.maximum);
      (void) fprintf(file,"      Mean:               %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.green.mean,
                     statistics.green.mean);
      (void) fprintf(file,"      Standard Deviation: %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.green.standard_deviation,
                     statistics.green.standard_deviation);
      (void) fprintf(file,"    Yellow:\n");
      (void) fprintf(file,"      Minimum:            %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.blue.minimum,
                     statistics.blue.minimum);
      (void) fprintf(file,"      Maximum:            %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.blue.maximum,
                     statistics.blue.maximum);
      (void) fprintf(file,"      Mean:               %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.blue.mean,
                     statistics.blue.mean);
      (void) fprintf(file,"      Standard Deviation: %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.blue.standard_deviation,
                     statistics.blue.standard_deviation);
      (void) fprintf(file,"    Black:\n");
      (void) fprintf(file,"      Minimum:            %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.opacity.minimum,
                     statistics.opacity.minimum);
      (void) fprintf(file,"      Maximum:            %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.opacity.maximum,
                     statistics.opacity.maximum);
      (void) fprintf(file,"      Mean:               %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.opacity.mean,
                     statistics.opacity.mean);
      (void) fprintf(file,"      Standard Deviation: %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.opacity.standard_deviation,
                     statistics.opacity.standard_deviation);
      /*
      if (image->matte)
        (void) fprintf(file,"    Opacity:\n");
      */
    }
    else if ((IsGrayColorspace(image->colorspace)) ||
             (image->is_grayscale == True))
      {
      (void) fprintf(file,"    Gray:\n");
      (void) fprintf(file,"      Minimum:            %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.red.minimum,
                     statistics.red.minimum);
      (void) fprintf(file,"      Maximum:            %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.red.maximum,
                     statistics.red.maximum);
      (void) fprintf(file,"      Mean:               %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.red.mean,
                     statistics.red.mean);
      (void) fprintf(file,"      Standard Deviation: %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.red.standard_deviation,
                     statistics.red.standard_deviation);
      if (image->matte)
        {
          (void) fprintf(file,"    Opacity:\n");
          (void) fprintf(file,"      Minimum:            %13.02lf (%1.4f)\n",
                         MaxRGB*statistics.opacity.minimum,
                         statistics.opacity.minimum);
          (void) fprintf(file,"      Maximum:            %13.02lf (%1.4f)\n",
                         MaxRGB*statistics.opacity.maximum,
                         statistics.opacity.maximum);
          (void) fprintf(file,"      Mean:               %13.02lf (%1.4f)\n",
                         MaxRGB*statistics.opacity.mean,
                         statistics.opacity.mean);
          (void) fprintf(file,"      Standard Deviation: %13.02lf (%1.4f)\n",
                         MaxRGB*statistics.opacity.standard_deviation,
                         statistics.opacity.standard_deviation);
        }
      }
    else
    {
      (void) fprintf(file,"    Red:\n");
      (void) fprintf(file,"      Minimum:            %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.red.minimum,
                     statistics.red.minimum);
      (void) fprintf(file,"      Maximum:            %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.red.maximum,
                     statistics.red.maximum);
      (void) fprintf(file,"      Mean:               %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.red.mean,
                     statistics.red.mean);
      (void) fprintf(file,"      Standard Deviation: %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.red.standard_deviation,
                     statistics.red.standard_deviation);
      (void) fprintf(file,"    Green:\n");
      (void) fprintf(file,"      Minimum:            %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.green.minimum,
                     statistics.green.minimum);
      (void) fprintf(file,"      Maximum:            %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.green.maximum,
                     statistics.green.maximum);
      (void) fprintf(file,"      Mean:               %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.green.mean,
                     statistics.green.mean);
      (void) fprintf(file,"      Standard Deviation: %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.green.standard_deviation,
                     statistics.green.standard_deviation);
      (void) fprintf(file,"    Blue:\n");
      (void) fprintf(file,"      Minimum:            %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.blue.minimum,
                     statistics.blue.minimum);
      (void) fprintf(file,"      Maximum:            %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.blue.maximum,
                     statistics.blue.maximum);
      (void) fprintf(file,"      Mean:               %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.blue.mean,
                     statistics.blue.mean);
      (void) fprintf(file,"      Standard Deviation: %13.02lf (%1.4f)\n",
                     MaxRGB*statistics.blue.standard_deviation,
                     statistics.blue.standard_deviation);
      if (image->matte)
        {
          (void) fprintf(file,"    Opacity:\n");
          (void) fprintf(file,"      Minimum:            %13.02lf (%1.4f)\n",
                         MaxRGB*statistics.opacity.minimum,
                         statistics.opacity.minimum);
          (void) fprintf(file,"      Maximum:            %13.02lf (%1.4f)\n",
                         MaxRGB*statistics.opacity.maximum,
                         statistics.opacity.maximum);
          (void) fprintf(file,"      Mean:               %13.02lf (%1.4f)\n",
                         MaxRGB*statistics.opacity.mean,
                         statistics.opacity.mean);
          (void) fprintf(file,"      Standard Deviation: %13.02lf (%1.4f)\n",
                         MaxRGB*statistics.opacity.standard_deviation,
                         statistics.opacity.standard_deviation);
        }
    }
  }
  x=0;
  p=(Image *) NULL;
  if ((image->matte && (strcmp(image->magick,"GIF") != 0)) || image->taint)
    {
      char
        tuple[MaxTextExtent];

      register const PixelPacket
        *p;

      p=(PixelPacket *) NULL;
      for (y=0; y < (long) image->rows; y++)
      {
        p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
        if (p == (const PixelPacket *) NULL)
          break;
        for (x=0; x < (long) image->columns; x++)
        {
          if (p->opacity == TransparentOpacity)
            break;
          p++;
        }
        if (x < (long) image->columns)
          break;
      }
      if ((x < (long) image->columns) || (y < (long) image->rows))
        {
          GetColorTuple(p,image->depth,image->matte,False,tuple);
          (void) fprintf(file,"  Opacity: %.1024s\t",tuple);
          GetColorTuple(p,image->depth,image->matte,True,tuple);
          (void) fprintf(file,"  %.1024s\n",tuple);
        }
    }
  if (image->storage_class == DirectClass)
    {
      if (image->total_colors != 0)
        (void) fprintf(file,"  Colors: %lu\n",image->total_colors);
    }
  else
    {
      if (image->total_colors <= image->colors)
        (void) fprintf(file,"  Colors: %u\n",image->colors);
      else
        (void) fprintf(file,"  Colors: %lu=>%u\n",image->total_colors,
                       image->colors);
    }
  if (image->storage_class == DirectClass)
    {
      if (image->total_colors < 1024)
        if (verbose > 1)
          (void) GetNumberColors(image,file,&image->exception);
    }
  else
    {
      char
        name[MaxTextExtent];

      register PixelPacket
        *p;

      /*
        Display image colormap.
      */
      p=image->colormap;
      for (i=0; i < (long) image->colors; i++)
      {
        char
          tuple[MaxTextExtent];

        GetColorTuple(p,image->depth,image->matte,False,tuple);
        (void) fprintf(file,"    %lu: %.1024s",i,tuple);
        (void) fprintf(file,"\t");
        (void) QueryColorname(image,p,SVGCompliance,name,&image->exception);
        (void) fprintf(file,"  %.1024s",name);
        (void) fprintf(file,"\n");
        p++;
      }
    }
  if (image->error.mean_error_per_pixel != 0.0)
    (void) fprintf(file,"  Mean Exception Per Pixel: %ld\n",
      (long) image->error.mean_error_per_pixel);
  if (image->error.normalized_mean_error != 0.0)
    (void) fprintf(file,"  Normalized Mean Exception: %g\n",
      image->error.normalized_mean_error);
  if (image->error.normalized_maximum_error != 0.0)
    (void) fprintf(file,"  Normalized Maximum Exception: %gn",
      image->error.normalized_maximum_error);
  if (image->rendering_intent == SaturationIntent)
    (void) fprintf(file,"  Rendering-Intent: saturation\n");
  else
    if (image->rendering_intent == PerceptualIntent)
      (void) fprintf(file,"  Rendering-Intent: perceptual\n");
    else
      if (image->rendering_intent == AbsoluteIntent)
        (void) fprintf(file,"  Rendering-Intent: absolute\n");
    else
      if (image->rendering_intent == RelativeIntent)
        (void) fprintf(file,"  Rendering-Intent: relative\n");
  if (image->gamma != 0.0)
    (void) fprintf(file,"  Gamma: %g\n",image->gamma);
  if ((image->chromaticity.red_primary.x != 0.0) ||
      (image->chromaticity.green_primary.x != 0.0) ||
      (image->chromaticity.blue_primary.x != 0.0) ||
      (image->chromaticity.white_point.x != 0.0))
    {
      /*
        Display image chromaticity.
      */
      (void) fprintf(file,"  Chromaticity:\n");
      (void) fprintf(file,"    red primary: (%g,%g)\n",
        image->chromaticity.red_primary.x,image->chromaticity.red_primary.y);
      (void) fprintf(file,"    green primary: (%g,%g)\n",
        image->chromaticity.green_primary.x,
        image->chromaticity.green_primary.y);
      (void) fprintf(file,"    blue primary: (%g,%g)\n",
        image->chromaticity.blue_primary.x,image->chromaticity.blue_primary.y);
      (void) fprintf(file,"    white point: (%g,%g)\n",
        image->chromaticity.white_point.x,image->chromaticity.white_point.y);
    }
  if ((image->tile_info.width*image->tile_info.height) != 0)
    (void) fprintf(file,"  Tile geometry: %lux%lu%+ld%+ld\n",
      image->tile_info.width,image->tile_info.height,image->tile_info.x,
      image->tile_info.y);
  if ((image->x_resolution != 0.0) && (image->y_resolution != 0.0))
    {
      /*
        Display image resolution.
      */
      (void) fprintf(file,"  Resolution: %gx%g",image->x_resolution,
        image->y_resolution);
      if (image->units == UndefinedResolution)
        (void) fprintf(file," pixels\n");
      else
        if (image->units == PixelsPerInchResolution)
          (void) fprintf(file," pixels/inch\n");
        else
          if (image->units == PixelsPerCentimeterResolution)
            (void) fprintf(file," pixels/centimeter\n");
          else
            (void) fprintf(file,"\n");
    }
  FormatSize(GetBlobSize(image),format);
  (void) fprintf(file,"  Filesize: %.1024s\n",format);
  fprintf(file,"  Interlace: %s\n",
	  InterlaceTypeToString(image->interlace == UndefinedInterlace ?
				NoInterlace : image->interlace));
  (void) fprintf(file,"  Orientation: %s\n", OrientationTypeToString(image->orientation));
  (void) QueryColorname(image,&image->background_color,SVGCompliance,color,
    &image->exception);
  (void) fprintf(file,"  Background Color: %.1024s\n",color);
  (void) QueryColorname(image,&image->border_color,SVGCompliance,color,
    &image->exception);
  (void) fprintf(file,"  Border Color: %.1024s\n",color);
  (void) QueryColorname(image,&image->matte_color,SVGCompliance,color,
    &image->exception);
  (void) fprintf(file,"  Matte Color: %.1024s\n",color);
  if ((image->page.width != 0) && (image->page.height != 0))
    (void) fprintf(file,"  Page geometry: %lux%lu%+ld%+ld\n",image->page.width,
      image->page.height,image->page.x,image->page.y);
  (void) fprintf(file,"  Compose: %s\n",
		 CompositeOperatorToString(image->compose));
  (void) fprintf(file,"  Dispose: ");
  switch (image->dispose)
  {
    case UndefinedDispose: (void) fprintf(file,"Undefined\n"); break;
    case NoneDispose: (void) fprintf(file,"None\n"); break;
    case BackgroundDispose: (void) fprintf(file,"Background\n"); break;
    case PreviousDispose: (void) fprintf(file,"Previous\n"); break;
    default: (void) fprintf(file,"\n");  break;
  }
  if (image->delay != 0)
    (void) fprintf(file,"  Delay: %lu\n",image->delay);
  if (image->iterations != 1)
    (void) fprintf(file,"  Iterations: %lu\n",image->iterations);
  p=image;
  while (p->previous != (Image *) NULL)
    p=p->previous;
  for (count=1; p->next != (Image *) NULL; count++)
    p=p->next;
  if (count > 1)
    (void) fprintf(file,"  Scene: %lu of %lu\n",image->scene,count);
  else
    if (image->scene != 0)
      (void) fprintf(file,"  Scene: %lu\n",image->scene);
  (void) fprintf(file,"  Compression: %s\n",
		 CompressionTypeToString(image->compression));
  /*
    Display formatted image attributes. This must happen before we access
    any pseudo attributes like EXIF since doing so causes real attributes
    to be created and we would get duplicates in the output.
  */
  attribute=GetImageAttribute(image,(char *) NULL);
  {
    for ( ; attribute != (const ImageAttribute *) NULL; attribute=attribute->next)
      {
	if (LocaleNCompare("EXIF",attribute->key,4) != 0)
	  {
	    (void) fprintf(file,"  %c", toupper((int)attribute->key[0]));
	    if (strlen(attribute->key) > 1)
	      (void) fprintf(file,"%.1024s",attribute->key+1);
	    
	    (void) fprintf(file,": ");
	    (void) fprintf(file,"%s\n",attribute->value);
	  }
      }
  }
  if((profile=GetImageProfile(image,"ICM",&profile_length)) != 0)
    (void) fprintf(file,"  Profile-color: %lu bytes\n",(unsigned long)
      profile_length);
  if((profile=GetImageProfile(image,"IPTC",&profile_length)) != 0)
    {
      char
        *tag,
        *text;

      size_t
        length;

      /*
        Describe IPTC data.
      */
      (void) fprintf(file,"  Profile-iptc: %lu bytes\n",(unsigned long)
        profile_length);
      for (i=0; i < (long) profile_length; )
      {
        if (profile[i] != 0x1c)
          {
            i++;
            continue;
          }
        i++;  /* skip file separator */
        i++;  /* skip record number */
        switch (profile[i])
        {
          case 5: tag=(char *) "Image Name"; break;
          case 7: tag=(char *) "Edit Status"; break;
          case 10: tag=(char *) "Priority"; break;
          case 15: tag=(char *) "Category"; break;
          case 20: tag=(char *) "Supplemental Category"; break;
          case 22: tag=(char *) "Fixture Identifier"; break;
          case 25: tag=(char *) "Keyword"; break;
          case 30: tag=(char *) "Release Date"; break;
          case 35: tag=(char *) "Release Time"; break;
          case 40: tag=(char *) "Special Instructions"; break;
          case 45: tag=(char *) "Reference Service"; break;
          case 47: tag=(char *) "Reference Date"; break;
          case 50: tag=(char *) "Reference Number"; break;
          case 55: tag=(char *) "Created Date"; break;
          case 60: tag=(char *) "Created Time"; break;
          case 65: tag=(char *) "Originating Program"; break;
          case 70: tag=(char *) "Program Version"; break;
          case 75: tag=(char *) "Object Cycle"; break;
          case 80: tag=(char *) "Byline"; break;
          case 85: tag=(char *) "Byline Title"; break;
          case 90: tag=(char *) "City"; break;
          case 95: tag=(char *) "Province State"; break;
          case 100: tag=(char *) "Country Code"; break;
          case 101: tag=(char *) "Country"; break;
          case 103: tag=(char *) "Original Transmission Reference"; break;
          case 105: tag=(char *) "Headline"; break;
          case 110: tag=(char *) "Credit"; break;
          case 115: tag=(char *) "Source"; break;
          case 116: tag=(char *) "Copyright String"; break;
          case 120: tag=(char *) "Caption"; break;
          case 121: tag=(char *) "Local Caption"; break;
          case 122: tag=(char *) "Caption Writer"; break;
          case 200: tag=(char *) "Custom Field 1"; break;
          case 201: tag=(char *) "Custom Field 2"; break;
          case 202: tag=(char *) "Custom Field 3"; break;
          case 203: tag=(char *) "Custom Field 4"; break;
          case 204: tag=(char *) "Custom Field 5"; break;
          case 205: tag=(char *) "Custom Field 6"; break;
          case 206: tag=(char *) "Custom Field 7"; break;
          case 207: tag=(char *) "Custom Field 8"; break;
          case 208: tag=(char *) "Custom Field 9"; break;
          case 209: tag=(char *) "Custom Field 10"; break;
          case 210: tag=(char *) "Custom Field 11"; break;
          case 211: tag=(char *) "Custom Field 12"; break;
          case 212: tag=(char *) "Custom Field 13"; break;
          case 213: tag=(char *) "Custom Field 14"; break;
          case 214: tag=(char *) "Custom Field 15"; break;
          case 215: tag=(char *) "Custom Field 16"; break;
          case 216: tag=(char *) "Custom Field 17"; break;
          case 217: tag=(char *) "Custom Field 18"; break;
          case 218: tag=(char *) "Custom Field 19"; break;
          case 219: tag=(char *) "Custom Field 20"; break;
          default: tag=(char *) "unknown"; break;
        }
        i++;
        (void) fprintf(file,"    %.1024s:\n",tag);
        length=profile[i++] << 8;
        length|=profile[i++];
        text=MagickAllocateMemory(char *,length+1);
        if (text != (char *) NULL)
          {
            char
              **textlist;

            register long
              j;

            (void) strncpy(text,(char *) profile+i,length);
            text[length]='\0';
            textlist=StringToList(text);
            if (textlist != (char **) NULL)
              {
                for (j=0; textlist[j] != (char *) NULL; j++)
                {
                  (void) fprintf(file,"  %s\n",textlist[j]);
                  MagickFreeMemory(textlist[j]);
                }
                MagickFreeMemory(textlist);
              }
            MagickFreeMemory(text);
          }
        i+=length;
      }
    }
  {
    const char
      *profile_name;
    
    size_t
      profile_length;
              
    const unsigned char *
      profile_info;
    
    ImageProfileIterator
      profile_iterator;

    profile_iterator=AllocateImageProfileIterator(image);
    while(NextImageProfile(profile_iterator,&profile_name,&profile_info,
                           &profile_length) != MagickFail)
      {
        if ((LocaleCompare(profile_name,"ICC") == 0) ||
            (LocaleCompare(profile_name,"ICM") == 0) ||
            (LocaleCompare(profile_name,"IPTC") == 0) ||
            (LocaleCompare(profile_name,"8BIM") == 0))
          continue;

        if (profile_length == 0)
          continue;
        (void) fprintf(file,"  Profile-%.1024s: %lu bytes\n",
                       profile_name == (char *) NULL ? "generic" : profile_name,
                       (unsigned long) profile_length);
        if (LocaleCompare(profile_name,"EXIF") == 0)
          {
            attribute=GetImageAttribute(image,"EXIF:*");
            if (attribute != (const ImageAttribute *) NULL)
              {
                char
                  **values;
                
                register char
                  *p;
                
                values=StringToList(attribute->value);
                if (values != (char **) NULL)
                  {
                    for (x=0; values[x] != (char *) NULL; x++)
                      {
                        (void) fprintf(file,"    ");
                        for (p=values[x]; *p != '\0'; p++)
                          {
                            if (p > values[x])
                              if ((isupper((int) ((unsigned char) *p)) != False) &&
                                  (islower((int) ((unsigned char) *(p+1))) != False))
                                (void) fprintf(file," ");
                            if (*p == '=')
                              {
                                (void) fprintf(file,": ");
                                for (p++; *p != '\0'; p++)
                                  (void) fputc(*p,file);
                                break;
                              }
                            (void) fputc(*p,file);
                          }
                        (void) fputc('\n',file);
                        MagickFreeMemory(values[x]);
                      }
                    MagickFreeMemory(values);
                  }
              }
          } /* End of EXIF */
      }
    DeallocateImageProfileIterator(profile_iterator);
  }
  if (image->montage != (char *) NULL)
    (void) fprintf(file,"  Montage: %.1024s\n",image->montage);
  if (image->directory != (char *) NULL)
    {
      Image
        *tile;

      ImageInfo
        *image_info;

      register char
        *p,
        *q;

      WarningHandler
        handler;

      /*
        Display visual image directory.
      */
      image_info=CloneImageInfo((ImageInfo *) NULL);
      (void) CloneString(&image_info->size,"64x64");
      (void) fprintf(file,"  Directory:\n");
      for (p=image->directory; *p != '\0'; p++)
      {
        q=p;
        while ((*q != '\n') && (*q != '\0'))
          q++;
        (void) strncpy(image_info->filename,p,q-p);
        image_info->filename[q-p]='\0';
        p=q;
        (void) fprintf(file,"    %.1024s",image_info->filename);
        handler=SetWarningHandler((WarningHandler) NULL);
        tile=ReadImage(image_info,&image->exception);
        (void) SetWarningHandler(handler);
        if (tile == (Image *) NULL)
          {
            (void) fprintf(file,"\n");
            continue;
          }
        (void) fprintf(file," %lux%lu %.1024s\n",tile->magick_columns,
          tile->magick_rows,tile->magick);
        (void) SignatureImage(tile);
        attribute=GetImageAttribute(tile,(char *) NULL);
        for ( ; attribute != (const ImageAttribute *) NULL; attribute=attribute->next)
        {
          if (*attribute->key == '[')
            continue;
          (void) fprintf(file,"  %.1024s:\n",attribute->key);
          (void) fprintf(file,"%s\n",attribute->value);
        }
        DestroyImage(tile);
      }
      DestroyImageInfo(image_info);
    }
  if (image->taint)
    (void) fprintf(file,"  Tainted: True\n");
  else
    (void) fprintf(file,"  Tainted: False\n");
  /*
    Only display time information if the time accumulated is at least
    the timer's resolution.
  */
  if (user_time >= GetTimerResolution())
    (void) fprintf(file,"  User Time: %0.3fu\n",user_time);
  if (elapsed_time >= GetTimerResolution())
    {
      (void) fprintf(file,"  Elapsed Time: %ld:%02ld\n",
                     (long) (elapsed_time/60.0),
                     (long) ceil(fmod(elapsed_time,60.0)));
      pixels_per_second=(magick_int64_t) ((double) image->rows*
                                          image->columns/
                                          (elapsed_time > GetTimerResolution() ?
                                           elapsed_time : GetTimerResolution()));
      FormatSize(pixels_per_second,format);
      (void) fprintf(file,"  Pixels Per Second: %s\n", format);
    }
  (void) fflush(file);
  return (ferror(file) ? MagickFail : MagickPass);
}
