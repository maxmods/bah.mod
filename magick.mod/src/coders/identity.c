/*
% Copyright (C) 2009 GraphicsMagick Group
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
#include "magick/blob.h"
#include "magick/pixel_cache.h"
#include "magick/constitute.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/utility.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d I D E N T I T Y I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadIdentityImage creates a Hald CLUT identity image.  The argument
%  to the IDENTITY filename (e.g. "identity:8") specifies the order of the
%  identity image.  The minimum order which may be specified is 2.  Higher
%  order LUTs contain more colors and are therefore more accurate, but consume
%  more memory.  Typical Hald CLUT identity images have an order of between 8
%  (512x512) and 16 (4096x4096).  The default order is 8.
%
%  The format of the ReadIdentityImage method is:
%
%      Image *ReadIdentityImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadIdentityImage returns a pointer to the image after
%      creating it. A null image is returned if there is a memory shortage
%      or if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/

static Image *ReadIdentityImage(const ImageInfo *image_info,
				ExceptionInfo *exception)
{
#define IdentityImageText  "[%s] Generating Hald CLUT identity image..."

  Image
    *image;

  unsigned long
    cube_size;

  long
    order,
    y;

  unsigned long
    row_count=0;

  unsigned int
    status=MagickPass;

  /*
    Initialize Image structure.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);

  image=(Image *) NULL;
  order=8;
  if (image_info->filename[0] != '\0')
    order=MagickAtoL(image_info->filename);
  if (order < 2)
    order=8;
  
  image=AllocateImage(image_info);
  cube_size=order*order;
  image->columns=image->rows=order*order*order;

#if defined(HAVE_OPENMP)
#  if defined(TUNE_OPENMP)
#    pragma omp parallel for schedule(runtime) shared(row_count, status)
#  else
#    pragma omp parallel for shared(row_count, status)
#  endif
#endif
  for (y=0; y < (long) image->rows; y += order)
    {
      MagickPassFail
        thread_status;

      register PixelPacket
        *q;

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_IdentityImage)
#endif
      thread_status=status;
      if (thread_status == MagickFail)
        continue;

      q=SetImagePixelsEx(image,0,y,image->columns,order,&image->exception);
      if (q == (PixelPacket *) NULL)
        thread_status=MagickFail;

      if (q != (PixelPacket *) NULL)
        {
	  double
	    value;

	  unsigned int
	    red,
	    green,
	    blue;

	  blue=y/order;
	  for(green = 0; green < cube_size; green++)
	    {
	      for(red = 0; red < cube_size; red++)
		{
		  value=MaxRGBDouble * (double)red / (double)(cube_size - 1);
		  q->red   = RoundDoubleToQuantum(value);
		  value     = MaxRGBDouble * (double)green / (double)(cube_size - 1);
		  q->green = RoundDoubleToQuantum(value);
		  value    = MaxRGBDouble * (double)blue / (double)(cube_size - 1);
		  q->blue  = RoundDoubleToQuantum(value);
		  q->opacity = OpaqueOpacity;
		  q++;
		}
	    }

          if (!SyncImagePixelsEx(image,&image->exception))
            thread_status=MagickFail;
        }

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_IdentityImage)
#endif
      {
        row_count++;
        if (QuantumTick(row_count,image->rows))
          if (!MagickMonitorFormatted(row_count,image->rows,&image->exception,
                                      IdentityImageText,image->filename))
            thread_status=MagickFail;

        if (thread_status == MagickFail)
          status=MagickFail;
      }
    }

  if (status == MagickFail)
    {
      DestroyImage(image);
      image=(Image *) NULL;
    }

  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r I D E N T I T Y I m a g e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterIDENTITYImage adds attributes for the Identity image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterIDENTITYImage method is:
%
%      RegisterIDENTITYImage(void)
%
*/
ModuleExport void RegisterIDENTITYImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("IDENTITY");
  entry->decoder=(DecoderHandler) ReadIdentityImage;
  entry->adjoin=False;
  entry->description="Hald CLUT identity image";
  entry->module="IDENTITY";
  entry->coder_class=PrimaryCoderClass;
  entry->extension_treatment=IgnoreExtensionTreatment;
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r I D E N T I T Y I m a g e                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterIDENTITYImage removes format registrations made by the
%  IDENTITY module from the list of supported formats.
%
%  The format of the UnregisterIDENTITYImage method is:
%
%      UnregisterIDENTITYImage(void)
%
*/
ModuleExport void UnregisterIDENTITYImage(void)
{
  (void) UnregisterMagickInfo("IDENTITY");
}
