/*
 * Copyright (C) 2009 GraphicsMagick Group
 *
 * American Society of Cinematographers Color Decision List (ASC-CDL)
 * implementation.
 *
 * Original implementation by Clément Follet.  Additional work by Bob
 * Friesenhahn.
 */

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/cdl.h"
#include "magick/pixel_iterator.h"
#include "magick/utility.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     C d l I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  The CdlImage() method applies ("bakes in") the ASC-CDL which is a format
%  for the exchange of basic primary color grading information between
%  equipment and software from different manufacturers. The format defines
%  the math for three functions: slope, offset and power. Each function uses
%  a number for the red, green, and blue color channels for a total of nine
%  numbers comprising a single color decision. A tenth number for chrominance
%  (saturation) has been proposed but is not yet standardized.
%
%  The cdl argument string is comma delimited and is in the form (but
%  without invervening spaces or line breaks):
%
%    redslope, redoffset, redpower :
%    greenslope, greenoffset, greenpower :
%    blueslope, blueoffset, bluepower :
%    saturation
%
%  with the unity (no change) specification being:
%
%    "1.0,0.0,1.0:1.0,0.0,1.0:1.0,0.0,1.0:0.0"
%
%  See http://en.wikipedia.org/wiki/ASC_CDL for more information.
%
%  The format of the CdlImage method is:
%
%      MagickPassFail CdlImage(Image *image,const char *cdl)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o cdl: Define the coefficients for slope offset and power in the
%      red green and blue channels, plus saturation.
%
*/

typedef struct _CdlImageParameters_t
{
  double
    redslope,
    redoffset,
    redpower,
    greenslope,
    greenoffset,
    greenpower,
    blueslope,
    blueoffset,
    bluepower,
    saturation;

  const PixelPacket
    *lut;
		
} CdlImageParameters_t;

static Quantum
CdlQuantum(const Quantum quantum, const double slope, const double offset,
	   const double power, const double saturation)
{
  double
    v,
    t;

  t=(((double) quantum)/MaxRGBDouble)*slope+offset;
  if (t < 0.0)
    t = 0.0;
  else if (t > 1.0)
    t = 1.0;
  v = (pow(t,power)+saturation)*MaxRGBDouble;
  return RoundDoubleToQuantum(v);
}

static MagickPassFail
CdlImagePixels(void *mutable_data,         /* User provided mutable data */
	       const void *immutable_data, /* User provided immutable data */
	       Image *image,               /* Modify image */
	       PixelPacket *pixels,        /* Pixel row */
	       IndexPacket *indexes,       /* Pixel row indexes */
	       const long npixels,         /* Number of pixels in row */
	       ExceptionInfo *exception)   /* Exception report */
{
  const CdlImageParameters_t
    param = *(const CdlImageParameters_t *) immutable_data;
	
  register long
    i;
	
  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);	

  if (param.lut != (PixelPacket *) NULL)
    {
      for(i = 0; i < npixels; i++)
	{
	  pixels[i].red=param.lut[pixels[i].red].red;
	  pixels[i].green=param.lut[pixels[i].green].green;
	  pixels[i].blue=param.lut[pixels[i].blue].blue;
	}
    }
  else
    {
      for(i = 0; i < npixels; i++)
	{
	  pixels[i].red=CdlQuantum(pixels[i].red,param.redslope,param.redoffset,
				   param.redpower,param.saturation);
	  pixels[i].green=CdlQuantum(pixels[i].green,param.greenslope,param.greenoffset,
				     param.greenpower,param.saturation);
	  pixels[i].blue=CdlQuantum(pixels[i].blue,param.blueslope,param.blueoffset,
				    param.bluepower,param.saturation);
	}
    }
	
  return MagickPass;
}


MagickExport MagickPassFail CdlImage(Image *image,const char *cdl)
{
  char
    progress_message[MaxTextExtent];

  CdlImageParameters_t
    param;

  PixelPacket
    *lut = (PixelPacket *) NULL;

  register long
    i;

  MagickPassFail
    status=MagickPass;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (cdl == (char *) NULL)
    return(MagickFail);

  param.redslope=1.0;
  param.redoffset=0.0;
  param.redpower=1.0;
  param.greenslope=1.0;
  param.greenoffset=0.0;
  param.greenpower=1.0;
  param.blueslope=1.0;
  param.blueoffset=0.0;
  param.bluepower=1.0;
  param.saturation=0.0;
  param.lut=(PixelPacket *) NULL;

  (void) sscanf(cdl,
		"%lf%*[,/]%lf%*[,/]%lf%*[:/]%lf%*[,/]%lf%*[,/]%lf%*"
		"[:/]%lf%*[,/]%lf%*[,/]%lf%*[:/]%lf",
		&param.redslope,&param.redoffset,&param.redpower,
		&param.greenslope,&param.greenoffset,&param.greenpower
		,&param.blueslope,&param.blueoffset,&param.bluepower,
		&param.saturation);

  param.redslope=AbsoluteValue(param.redslope);
  param.redpower=AbsoluteValue(param.redpower);
  param.greenslope=AbsoluteValue(param.greenslope);
  param.greenpower=AbsoluteValue(param.greenpower);
  param.blueslope=AbsoluteValue(param.blueslope);
  param.bluepower=AbsoluteValue(param.bluepower);

  FormatString(progress_message,
	       "[%%s] cdl %g/%g/%g/%g/%g/%g/%g/%g/%g/%g image...",
	       param.redslope,param.redoffset,param.redpower,
	       param.greenslope,param.greenoffset,param.greenpower,
	       param.blueslope,param.blueoffset,param.bluepower,
	       param.saturation);

  if (!IsRGBCompatibleColorspace(image->colorspace))
    TransformColorspace(image,RGBColorspace);

  /*
    Build a LUT if it is beneficial to do so.
  */
  if ((MaxMap == MaxRGB) && (image->columns*image->rows > MaxMap*3))
    {
      lut=MagickAllocateMemory(PixelPacket *,(MaxMap+1)*sizeof(PixelPacket));
      if (lut != (PixelPacket *) NULL)
	{
#if (MaxMap > 256) && defined(HAVE_OPENMP)
#  if defined(USE_STATIC_SCHEDULING_ONLY)
#    pragma omp parallel for schedule(static,4)
#  else
#    pragma omp parallel for schedule(guided)
#  endif
#endif
	  for (i=0; i <= (long) MaxMap; i++)
	    {
	      lut[i].red=CdlQuantum((Quantum) i,param.redslope,param.redoffset,
				       param.redpower,param.saturation);
	      lut[i].green=CdlQuantum((Quantum) i,param.greenslope,param.greenoffset,
					 param.greenpower,param.saturation);
	      lut[i].blue=CdlQuantum((Quantum) i,param.blueslope,param.blueoffset,
					param.bluepower,param.saturation);
	    }
	  param.lut=lut;
	}
    }

  if (image->storage_class == PseudoClass)
    {
      (void) CdlImagePixels(NULL,&param,image,image->colormap,
			    (IndexPacket *) NULL,image->colors,
			    &image->exception);
      status=SyncImage(image);
    }
  else
    {
      status=PixelIterateMonoModify(CdlImagePixels,NULL,progress_message,
				    NULL,&param,0,0,image->columns,image->rows,
				    image,&image->exception);
    }

  MagickFreeMemory(lut);

  return(status);
}

	
