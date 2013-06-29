/*
% Copyright (C) 2009 GraphicsMagick Group
% Copyright (C) 2005 Eskil Steenberg.  All rights reserved.
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/hclut.h"
#include "magick/pixel_cache.h"
#include "magick/pixel_iterator.h"
#include "magick/monitor.h"
#include "magick/utility.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%     H a l d C l u t I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  The HaldClutImage() method apply a color lookup table (Hald CLUT) to the
%  image.  The fundamental principle of the Hald CLUT algorithm is that
%  application of an identity CLUT causes no change to the input image,
%  but an identity CLUT image which has had its colors transformed in
%  some way (e.g. in Adobe Photoshop) may be used to implement an identical
%  transform on any other image.
%
%  The minimum CLUT level is 2, and the maximum depends on available memory
%  (largest successfully tested is 24).  A CLUT image is required to have equal
%  width and height. A CLUT of level 8 is an image of dimension 512x512, a CLUT
%  of level 16 is an image of dimension 4096x4096.  Interpolation is used so
%  extremely large CLUT images are not required.
%
%  GraphicsMagick provides an 'identity' coder which may be used to generate
%  identity HLUTs.  For example, reading from "identity:8" creates an identity
%  CLUT of order 8.
%
%  The Hald CLUT algorithm has been developed by Eskil Steenberg as described
%  at http://www.quelsolaar.com/technology/clut.html, and was adapted for
%  GraphicsMagick by Clément Follet with support from Cédric Lejeune of
%  Workflowers.
%
%  The format of the HaldClutImage method is:
%
%      MagickPassFail HaldClutImage(Image *image,const Image *clut)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o clut: The color lookup table image
%
%
*/

typedef struct _HaldClutImageParameters_t
{
  unsigned int 
    level;

  const PixelPacket 
    *ppcl;

} HaldClutImageParameters_t;

static MagickPassFail
HaldClutImagePixels(void *mutable_data,         /* User provided mutable data */
		    const void *immutable_data, /* User provided immutable data */
		    Image *image,               /* Modify image */
		    PixelPacket *pixels,        /* Pixel row */
		    IndexPacket *indexes,       /* Pixel row indexes */
		    const long npixels,         /* Number of pixels in row */
		    ExceptionInfo *exception)   /* Exception report */
{

  const HaldClutImageParameters_t *
    param = (const HaldClutImageParameters_t *) immutable_data;
	
  unsigned int
    level = param->level;
	
  const PixelPacket
    *clut = param->ppcl;

  unsigned int
    color;

  unsigned int
    blueaxis,
    greenaxis,
    redaxis;

  register long
    i,
    k;

  double
    sums[9],
    r,
    g,
    b,
    value;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  level *= level;
	
  for(k = 0; k < npixels ; k++)
    {
      /*
	Calculate the position of each 3D axis pixel level.
      */
      redaxis = (unsigned int) (((double) pixels[k].red/MaxRGBDouble) * (level-1));
      if (redaxis > level - 2)
	redaxis = level - 2;
      greenaxis = (unsigned int) (((double) pixels[k].green/MaxRGBDouble) * (level-1));
      if(greenaxis > level - 2)
	greenaxis = level - 2;
      blueaxis = (unsigned int) (((double) pixels[k].blue/MaxRGBDouble) * (level-1));
      if(blueaxis > level - 2)
	blueaxis = level - 2;

      /*
	Convert between the value and the equivalent value position.
      */
      r = ((double) pixels[k].red/MaxRGBDouble) * (level - 1) - redaxis;
      g = ((double) pixels[k].green/MaxRGBDouble) * (level - 1) - greenaxis;
      b = ((double) pixels[k].blue/MaxRGBDouble) * (level - 1) - blueaxis;

      color = redaxis + greenaxis * level + blueaxis * level * level;

      i = color;
      sums[0] = ((double) clut[i].red) * (1 - r);
      sums[1] = ((double) clut[i].green) * (1 - r);
      sums[2] = ((double) clut[i].blue) * (1 - r);
      i++;
      sums[0] += ((double) clut[i].red) * r;
      sums[1] += ((double) clut[i].green) * r;
      sums[2] += ((double) clut[i].blue) * r;

      i = (color + level);
      sums[3] = ((double) clut[i].red) * (1 - r);
      sums[4] = ((double) clut[i].green) * (1 - r);
      sums[5] = ((double) clut[i].blue) * (1 - r);
      i++;
      sums[3] += ((double) clut[i].red) * r;
      sums[4] += ((double) clut[i].green) * r;
      sums[5] += ((double) clut[i].blue) * r;

      sums[6] = sums[0] * (1 - g) + sums[3] * g;
      sums[7] = sums[1] * (1 - g) + sums[4] * g;
      sums[8] = sums[2] * (1 - g) + sums[5] * g;

      i = (color + level * level);
      sums[0] = ((double) clut[i].red) * (1 - r);
      sums[1] = ((double) clut[i].green) * (1 - r);
      sums[2] = ((double) clut[i].blue) * (1 - r);
      i++;
      sums[0] += ((double) clut[i].red) * r;
      sums[1] += ((double) clut[i].green) * r;
      sums[2] += ((double) clut[i].blue) * r;

      i = (color + level * level + level);
      sums[3] = ((double) clut[i].red) * (1 - r);
      sums[4] = ((double) clut[i].green) * (1 - r);
      sums[5] = ((double) clut[i].blue) * (1 - r);
      i++;
      sums[3] += ((double) clut[i].red) * r;
      sums[4] += ((double) clut[i].green) * r;
      sums[5] += ((double) clut[i].blue) * r;

      sums[0] = sums[0] * (1 - g) + sums[3] * g;
      sums[1] = sums[1] * (1 - g) + sums[4] * g;
      sums[2] = sums[2] * (1 - g) + sums[5] * g;

      value=(sums[6] * (1 - b) + sums[0] * b);
      pixels[k].red = RoundDoubleToQuantum(value);

      value=(sums[7] * (1 - b) + sums[1] * b);
      pixels[k].green = RoundDoubleToQuantum(value);

      value=(sums[8] * (1 - b) + sums[2] * b);
      pixels[k].blue = RoundDoubleToQuantum(value);
    }
	
  return MagickPass;
}


MagickExport MagickPassFail
HaldClutImage(Image *image, const Image *clut)
{	
  unsigned int
    level;

  char
    progress_message[MaxTextExtent];
	
  HaldClutImageParameters_t
    param;
	
  MagickPassFail
    status=MagickPass;
	
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
    
  /*
    Hald CLUT images are square.
  */
  if(clut->rows != clut->columns)
    {
      ThrowBinaryException(OptionError,HaldClutImageDimensionsInvalid,
			   clut->filename);
    }

  /*
    Calculate the level of the Hald CLUT
  */
  for(level = 1; level * level * level < clut->rows; level++);
  if((level * level * level > clut->rows) || (level < 2))
    {
      ThrowBinaryException(OptionError,HaldClutImageDimensionsInvalid,
			   clut->filename);
    }

  param.level = level;

  /*
    We acquire all of the pixels at once, which is the limiting factor
    on maximum Hald CLUT size.
  */
  param.ppcl=AcquireImagePixels(clut,0,0,clut->columns,clut->rows,&image->exception);
  if (param.ppcl == (const PixelPacket *) NULL)
    return MagickFail;
							 
  FormatString(progress_message,
	       "[%%s] Applying Hald CLUT level %u (%lux%lu) ...",
	       param.level,clut->columns,clut->rows);

  if (!IsRGBCompatibleColorspace(image->colorspace))
    TransformColorspace(image,RGBColorspace);
  if (image->storage_class == PseudoClass)
    {
      (void) HaldClutImagePixels(NULL,&param,image,image->colormap,
				 (IndexPacket *) NULL,image->colors,
				 &image->exception);
      status=SyncImage(image);
    }
  else
    {
      status=PixelIterateMonoModify(HaldClutImagePixels,NULL,progress_message,
				    NULL,&param,0,0,image->columns,image->rows,
				    image,&image->exception);
    }
	
  return(status);
}
	
