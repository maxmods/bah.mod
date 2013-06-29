/*
% Copyright (C) 2003-2011 GraphicsMagick Group
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
%                            PPPP   N   N  M   M                              %
%                            P   P  NN  N  MM MM                              %
%                            PPPP   N N N  M M M                              %
%                            P      N  NN  M   M                              %
%                            P      N   N  M   M                              %
%                                                                             %
%                                                                             %
%              Read/Write PBMPlus Portable Anymap Image Format.               %
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
#include "magick/analyze.h"
#include "magick/attribute.h"
#include "magick/blob.h"
#include "magick/color.h"
#include "magick/colormap.h"
#include "magick/constitute.h"
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/omp_data_view.h"
#include "magick/pixel_cache.h"
#include "magick/utility.h"

/*
  Forward declarations.
*/
static unsigned int
  WritePNMImage(const ImageInfo *,Image *);


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s P N M                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsPNM returns True if the image format type, identified by the
%  magick string, is PNM.
%
%  The format of the IsPNM method is:
%
%      unsigned int IsPNM(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsPNM returns True if the image format type is PNM.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsPNM(const unsigned char *magick,const size_t length)
{
  if (length < 2)
    return(False);
  if ((*magick == 'P') && isdigit((int) magick[1]))
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d P N M I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPNMImage reads a Portable Anymap image file and returns it.
%  It allocates the memory necessary for the new Image structure and returns
%  a pointer to the new image.
%
%  The format of the ReadPNMImage method is:
%
%      Image *ReadPNMImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadPNMImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/

static unsigned int PNMInteger(Image *image,const unsigned int base)
{
#define P7Comment  "END_OF_COMMENTS\n"

  int
    c;

  unsigned long
    value;

  /*
    Skip any leading whitespace.
  */
  do
  {
    c=ReadBlobByte(image);
    if (c == EOF)
      return(0);
    if (c == '#')
      {
        char
          *comment;

        ExtendedSignedIntegralType
          offset;

        register char
          *p,
          *q;

        size_t
          length;

        /*
          Read comment.
        */
        length=MaxTextExtent;
        comment=MagickAllocateMemory(char *,length+strlen(P7Comment)+1);
        p=comment;
        offset=p-comment;
        if (comment != (char *) NULL)
          for ( ; (c != EOF) && (c != '\n'); p++)
          {
            if ((size_t) (p-comment) >= length)
              {
                length<<=1;
                length+=MaxTextExtent;
                MagickReallocMemory(char *,comment,length+strlen(P7Comment)+1);
                if (comment == (char *) NULL)
                  break;
                p=comment+strlen(comment);
              }
            c=ReadBlobByte(image);
            *p=c;
            *(p+1)='\0';
          }
        if (comment == (char *) NULL)
          return(0);
        q=comment+offset;
        if (LocaleCompare(q,P7Comment) == 0)
          *q='\0';
        (void) SetImageAttribute(image,"comment",comment);
        MagickFreeMemory(comment);
        continue;
      }
  } while (!isdigit(c));
  if (base == 2)
    return(c-'0');
  /*
    Evaluate number.
  */
  value=0;
  do
  {
    value*=10;
    value+=c-'0';
    c=ReadBlobByte(image);
    if (c == EOF)
      return(value);
  }
  while (isdigit(c));
  return(value);
}

#define ValidateScalingIndex(image, index, max)				\
  do									\
    {									\
      if (index > max)							\
	ThrowReaderException(CorruptImageError,CorruptImage, image);	\
    } while (0)

#define ValidateScalingPixel(image, pixel, max)		\
  do							\
    {							\
      ValidateScalingIndex(image, pixel.red, max);	\
      ValidateScalingIndex(image, pixel.green, max);	\
      ValidateScalingIndex(image, pixel.blue, max);	\
    } while (0)

typedef enum
  {
    Undefined_PNM_Format,
    PBM_ASCII_Format, /* P1 */
    PGM_ASCII_Format, /* P2 */
    PPM_ASCII_Format, /* P3 */
    PBM_RAW_Format, /* P4 */
    PGM_RAW_Format, /* P5 */
    PPM_RAW_Format, /* P6 */
    PAM_Format, /* P7 */
    XV_332_Format /* P7 332 */
  } PNMSubformat;

static Image *ReadPNMImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    c;

  PNMSubformat
    format;

  Image
    *image;

  long
    y;

  LongPixelPacket
    pixel;

  register IndexPacket
    *indexes;

  register unsigned long
    i;

  size_t
    count,
    number_pixels;

  unsigned int
    index,
    bits_per_sample;

  MagickPassFail
    status;

  unsigned int
    max_value,
    samples_per_pixel;

  Quantum
    *scale;

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
    Read PNM image.
  */
  count=ReadBlob(image,1,(char *) &c);
  do
    {
      /*
        Initialize image structure.
      */
      max_value=0;
      bits_per_sample=0;
      samples_per_pixel=0;

      if (count == 0)
        ThrowReaderException(CorruptImageError,ImproperImageHeader,image);

      if (c != 'P')
	{
	  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Read %c rather than expected 'P'!",c);
	  ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
	}

      c=ReadBlobByte(image);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),"PNM Format Id: P%c",
                            c);

      switch (c)
	{
	case '1': format=PBM_ASCII_Format; break;
	case '2': format=PGM_ASCII_Format; break;
	case '3': format=PPM_ASCII_Format; break;
	case '4': format=PBM_RAW_Format; break;
	case '5': format=PGM_RAW_Format; break;
	case '6': format=PPM_RAW_Format; break;
	case '7':
	  {
	    if ((ReadBlobByte(image) == ' ') && 
		(PNMInteger(image,10) == 332))
	      format=XV_332_Format;
	    else
	      format=PAM_Format;
	    break;
	  }
	default:
	  {
	    format=Undefined_PNM_Format;
	  }
	}

      if (PAM_Format == format)
	{
	  /*
	    PAM header format

	    P7
	    WIDTH 227
	    HEIGHT 149
	    DEPTH 3
	    MAXVAL 255
	    TUPLTYPE RGB
	    ENDHDR
	  */

	  char
	    keyword[MaxTextExtent];

	  register char
	    *p;

	  int
	    c;

	  while (1)
	    {
	      p=keyword;
	      c=ReadBlobByte(image);
	      do
		{
		  if (isalnum(c) || ('#' == c))
		    if ((p-keyword) < (MaxTextExtent-1))
		      *p++=c;
		  c=ReadBlobByte(image);
		} while (isalnum(c) || ('#' == c));
	      *p='\0';

	      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
				    "Keyword \"%s\"",keyword);
	      if ((EOF == c) || (LocaleCompare(keyword,"ENDHDR") == 0))
		{
		  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
					"Exiting header!");
		  break;
		}
	      else if (LocaleCompare(keyword,"HEIGHT") == 0)
		{
		  image->rows=PNMInteger(image,10);
		}
	      else if (LocaleCompare(keyword,"WIDTH") == 0)
		{
		  image->columns=PNMInteger(image,10);
		}
	      else if (LocaleCompare(keyword,"DEPTH") == 0)
		{
		  samples_per_pixel=PNMInteger(image,10);
		}
	      else if (LocaleCompare(keyword,"MAXVAL") == 0)
		{
		  max_value=PNMInteger(image,10);
		}
	      else if (LocaleCompare(keyword,"TUPLTYPE") == 0)
		{
		  /* Skip white space */
		  do
		    {
		      c=ReadBlobByte(image);
		    } while (isspace(c) && (EOF != c));
		  if (EOF == c)
		    break;
		  /* Tupletype argument */
		  p=keyword;
		  do
		    {
		      if ((p-keyword) < (MaxTextExtent-1))
			*p++=c;
		      c=ReadBlobByte(image);
		    } while (('\n' != c) && (EOF != c));
		  *p='\0';
		  if (EOF == c)
		    break;
		  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
					"TUPLTYPE \"%s\"",keyword);
		  if (LocaleNCompare(keyword,"BLACKANDWHITE",13) == 0)
		    {
		      image->colorspace=GRAYColorspace;
		      image->is_monochrome=MagickTrue;
		    }
		  else if (LocaleNCompare(keyword,"CMYK",4) == 0)
		    {
		      image->colorspace=CMYKColorspace;
		    }
		  else if (LocaleNCompare(keyword,"GRAYSCALE",9) == 0)
		    {
		      image->colorspace=GRAYColorspace;
		    }
		  else if (LocaleNCompare(keyword,"RGB",3) == 0)
		    {
		    }

		  /*
		    Check for alpha flag.
		  */
		  count=strlen(keyword);
		  if ((count > 6) && (LocaleNCompare(keyword+count-6,"_ALPHA",6) == 0))
		    {
		      image->matte=MagickTrue;
		    }
		}
	      else if (LocaleNCompare(keyword,"#",1) == 0)
		{
		  /* Skip white space */
		  do
		    {
		      c=ReadBlobByte(image);
		    } while (isspace(c) && (EOF != c));
		  if (EOF == c)
		    break;

		  /* Comment */
		  p=keyword;
		  do
		    {
		      if ((p-keyword) < (MaxTextExtent-1))
			*p++=c;
		      c=ReadBlobByte(image);
		    } while (('\n' != c) && (EOF != c));
		  *p='\0';
		  (void) SetImageAttribute(image,"comment",keyword);
		  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
					"Comment: \"%s\"",keyword);
		}
	      else
		{
		  /* Unknown! */
		  do
		    {
		      c=ReadBlobByte(image);
		    } while (('\n' != c) && (EOF != c));
		  break;
		}
	    }
	}
      else
	{
	  /*
	    PNM header type format

	    P1
	    # feep.pbm
	    24 7

	    P3
	    # feep.ppm
	    4 4
	    15
	  */
	  image->columns=PNMInteger(image,10);
	  image->rows=PNMInteger(image,10);

	  if ((format == PBM_ASCII_Format) || (format == PBM_RAW_Format))
	    max_value=1;  /* bitmap */
	  else
	    max_value=PNMInteger(image,10);

	  switch (format)
	    {
	    case PBM_ASCII_Format:
	    case PBM_RAW_Format:
	    case PGM_ASCII_Format:
	    case PGM_RAW_Format:
	    case XV_332_Format:
	      {
		samples_per_pixel=1;
		break;
	      }
	    case PPM_ASCII_Format:
	    case PPM_RAW_Format:
	      {
		samples_per_pixel=3;
		break;
	      }
	    default:
	      {
	      }
	    }
	}

      if (max_value <= 1)
	bits_per_sample=1;
      else if (max_value <= 255U)
	bits_per_sample=8;
      else if (max_value <= 65535U)
	bits_per_sample=16;
      else if (max_value <= 4294967295U)
	bits_per_sample=32;

      image->depth=Min(bits_per_sample,QuantumDepth);

      (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Max Value: %u",
			    max_value);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Dimensions: %lux%lu",
                            image->columns,image->rows);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Image Depth: %u",
                            image->depth);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Samples Per Pixel: %u",
			    samples_per_pixel);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Bits Per Sample: %u",
			    bits_per_sample);

      if (EOFBlob(image))
	ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,
		       image->filename);

      if ((1 == samples_per_pixel) && (max_value < MaxColormapSize))
        {
          image->storage_class=PseudoClass;
          image->colors=
            max_value >= MaxColormapSize ? MaxColormapSize : max_value+1;
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Colors: %u",
                                image->colors);
        }
      number_pixels=image->columns*image->rows;
      if (number_pixels == 0)
        ThrowReaderException(CorruptImageError,NegativeOrZeroImageSize,image);
      scale=(Quantum *) NULL;
      if (image->storage_class == PseudoClass)
        {
          /*
            Create colormap.
          */
          if (!AllocateImageColormap(image,image->colors))
            ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
                                 image);
          if ((format == XV_332_Format) && (image->colors == 256))
            {
              /*
                Initialize 332 colormap.
              */
              i=0;
              for (pixel.red=0; pixel.red < 8; pixel.red++)
                for (pixel.green=0; pixel.green < 8; pixel.green++)
                  for (pixel.blue=0; pixel.blue < 4; pixel.blue++)
                    {
                      image->colormap[i].red=(Quantum)
                        (((double) MaxRGB*pixel.red)/0x07+0.5);
                      image->colormap[i].green=(Quantum)
                        (((double) MaxRGB*pixel.green)/0x07+0.5);
                      image->colormap[i].blue=(Quantum)
                        (((double) MaxRGB*pixel.blue)/0x03+0.5);
                      i++;
                    }
            }
        }
      if ((image->storage_class != PseudoClass) || (max_value > MaxRGB))
        {
          /*
            Compute pixel scaling table.
          */
          scale=MagickAllocateMemory(Quantum *,
                                     (max_value+1)*sizeof(Quantum));
          if (scale == (Quantum *) NULL)
            ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
                                 image);
          for (i=0; i <= max_value; i++)
            scale[i]=ScaleAnyToQuantum((unsigned long) i, max_value);
        }
      if (image_info->ping && (image_info->subrange != 0))
        if (image->scene >= (image_info->subimage+image_info->subrange-1))
          break;
      /*
        Convert PNM pixels to runlength-encoded MIFF packets.
      */
      switch (format)
        {
        case PBM_ASCII_Format:
          {
            /*
              Convert PBM image to pixel packets.
            */
            register unsigned long
              x;
            
            register PixelPacket
              *q;

            for (y=0; y < (long) image->rows; y++)
              {
                q=SetImagePixels(image,0,y,image->columns,1);
                if (q == (PixelPacket *) NULL)
                  break;
                indexes=AccessMutableIndexes(image);
                for (x=0; x < image->columns; x++)
                  {
                    index=!PNMInteger(image,2);
                    if (EOFBlob(image))
                      break;
                    VerifyColormapIndex(image,index);
                    indexes[x]=index;
                    *q++=image->colormap[index];
                  }
                if (!SyncImagePixels(image))
                  break;
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(y,image->rows))
                    if (!MagickMonitorFormatted(y,image->rows,exception,
                                                LoadImageText,image->filename,
						image->columns,image->rows))
                      break;
                if (EOFBlob(image))
                  break;
              }
            image->is_grayscale=MagickTrue;
            image->is_monochrome=MagickTrue;
            if (EOFBlob(image))
              ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,
                             image->filename);
            break;
          }
        case PGM_ASCII_Format:
          {
            /*
              Convert PGM image to pixel packets.
            */
            register unsigned long
              x;
            
            register PixelPacket
              *q;

            unsigned long
              intensity;

            MagickBool
              is_grayscale,
              is_monochrome;

            is_grayscale=MagickTrue;
            is_monochrome=MagickTrue;
            for (y=0; y < (long) image->rows; y++)
              {
                q=SetImagePixels(image,0,y,image->columns,1);
                if (q == (PixelPacket *) NULL)
                  break;
		if (image->storage_class == PseudoClass)
		  {
		    /*
		      PseudoClass
		    */
		    indexes=AccessMutableIndexes(image);
		    for (x=0; x < image->columns; x++)
		      {
			intensity=PNMInteger(image,10);
			ValidateScalingIndex(image, intensity, max_value);
			if (EOFBlob(image))
			  break;
			if (scale != (Quantum *) NULL)
			  intensity=scale[intensity];
			index=intensity;
			VerifyColormapIndex(image,index);
			indexes[x]=index;
			*q=image->colormap[index];
			is_monochrome &= IsMonochrome(*q);
			q++;
		      }
		  }
		else
		  {
		    /*
		      DirectClass
		    */
		    for (x=0; x < image->columns; x++)
		      {
			intensity=PNMInteger(image,10);
			ValidateScalingIndex(image, intensity, max_value);
			if (EOFBlob(image))
			  break;
			if (scale != (Quantum *) NULL)
			  intensity=scale[intensity];
			q->red=q->green=q->blue=intensity;
			is_monochrome &= IsMonochrome(*q);
			q++;
		      }
		  }
                if (EOFBlob(image))
                  break;
                if (!SyncImagePixels(image))
                  break;
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(y,image->rows))
                    if (!MagickMonitorFormatted(y,image->rows,exception,
                                                LoadImageText,image->filename,
						image->columns,image->rows))
                      break;
              }
            image->is_monochrome=is_monochrome;
            image->is_grayscale=is_grayscale;
            if (EOFBlob(image))
              ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,
                             image->filename);
            break;
          }
        case PPM_ASCII_Format:
          {
            /*
              Convert PNM image to pixel packets.
            */
            register unsigned long
              x;
            
            register PixelPacket
              *q;

            MagickBool
              is_grayscale,
              is_monochrome;

            is_grayscale=MagickTrue;
            is_monochrome=MagickTrue;
            for (y=0; y < (long) image->rows; y++)
              {
                q=SetImagePixels(image,0,y,image->columns,1);
                if (q == (PixelPacket *) NULL)
                  break;
                for (x=0; x < image->columns; x++)
                  {
                    pixel.red=PNMInteger(image,10);
                    pixel.green=PNMInteger(image,10);
                    pixel.blue=PNMInteger(image,10);
                    if (EOFBlob(image))
                      break;
                    ValidateScalingPixel(image, pixel, max_value);
                    if (scale != (Quantum *) NULL)
                      {
                        pixel.red=scale[pixel.red];
                        pixel.green=scale[pixel.green];
                        pixel.blue=scale[pixel.blue];
                      }
                    q->red=(Quantum) pixel.red;
                    q->green=(Quantum) pixel.green;
                    q->blue=(Quantum) pixel.blue;
                    is_monochrome &= IsMonochrome(*q);
                    is_grayscale &= IsGray(*q);
                    q++;
                  }
                if (EOFBlob(image))
                  break;
                if (!SyncImagePixels(image))
                  break;
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(y,image->rows))
                    if (!MagickMonitorFormatted(y,image->rows,exception,
                                                LoadImageText,image->filename,
						image->columns,image->rows))
                      break;
              }
            image->is_monochrome=is_monochrome;
            image->is_grayscale=is_grayscale;
            if (EOFBlob(image))
              ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,
                             image->filename);
            break;
          }
        case PBM_RAW_Format:
        case PGM_RAW_Format:
        case PPM_RAW_Format:
	case PAM_Format:
        case XV_332_Format:
	  {
            /*
              Convert PBM/PGM/PPM/PAM/XV raw raster image to pixel packets.
            */
	    ImportPixelAreaOptions
              import_options;

	    QuantumType
	      quantum_type;

            size_t
              bytes_per_row;
        
            MagickBool
	      check_pixels,
              is_grayscale,
              is_monochrome;
        
            unsigned long
              row_count=0;
        
            ThreadViewDataSet
              *scanline_set;

	    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Reading PAM");

	    ImportPixelAreaOptionsInit(&import_options);
 
	    check_pixels=MagickTrue;
            is_grayscale=MagickTrue;
            is_monochrome=MagickTrue;

	    /*
	      Deduce correct import parameters.
	    */
	    quantum_type=UndefinedQuantum;
	    import_options.grayscale_miniswhite=MagickFalse;
	    bytes_per_row=0;
	    if (1 == samples_per_pixel)
	      {
		if (1 == bits_per_sample)
		  {
		    /* PBM */
		    bytes_per_row=((image->columns+7) >> 3);
		    import_options.grayscale_miniswhite=MagickTrue;
		    quantum_type=GrayQuantum;
		  }
		else
		  {
		    /* PGM & XV_332 */
		    bytes_per_row=((bits_per_sample+7)/8)*image->columns;
		    if (XV_332_Format == format)
		      {
			quantum_type=IndexQuantum;
		      }
		    else
		      {
			quantum_type=GrayQuantum;
		      }
		  }
	      }
	    else
	      {
		bytes_per_row=(((bits_per_sample+7)/8)*samples_per_pixel)*image->columns;
		if (3 == samples_per_pixel)
		  {
		    /* PPM */
		    quantum_type=RGBQuantum;
		  }
		else if (4 == samples_per_pixel)
		  {
		    if (CMYKColorspace == image->colorspace)
		      quantum_type=CMYKQuantum;
		    else
		      quantum_type=RGBAQuantum;
		  }
		else if (5 == samples_per_pixel)
		  {
		    if (CMYKColorspace == image->colorspace)
		      quantum_type=CMYKAQuantum;
		  }
	      }

	    

	    if (1 == samples_per_pixel)
	      {
		check_pixels=MagickFalse;
	      }
	    if (GrayQuantum)
	      {
		if (1 == bits_per_sample)
		  {
		    is_grayscale=MagickTrue;
		    is_monochrome=MagickTrue;
		  }
		else
		  {
		    is_grayscale=MagickTrue;
		    is_monochrome=MagickFalse;
		  }
	      }
        
            scanline_set=AllocateThreadViewDataArray(image,exception,bytes_per_row,1);
            if (scanline_set == (ThreadViewDataSet *) NULL)
              ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
#if 0
#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  if defined(TUNE_OPENMP)
#    pragma omp parallel for schedule(runtime) shared(is_grayscale,is_monochrome,row_count,status)
#  else
#    pragma omp parallel for schedule(static,1) shared(is_grayscale,is_monochrome,row_count,status)
#  endif
#endif
#endif
            for (y=0; y < (long) image->rows; y++)
              {
                register unsigned long
                  x;

                register PixelPacket
                  *q = (PixelPacket *) NULL;

                void
                  *pixels;

                MagickBool
                  thread_status;
            
                MagickBool
                  thread_is_grayscale,
                  thread_is_monochrome;
            
                unsigned long
                  thread_row_count;

		ImportPixelAreaInfo
		  import_info;

#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  pragma omp critical (GM_ReadPNMImage)
#endif
                thread_status=status;
                if (thread_status == MagickFail)
                  continue;
            
                pixels=AccessThreadViewData(scanline_set);
            
#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  pragma omp critical (GM_ReadPNMImage)
#endif
                {
		  thread_is_grayscale=is_grayscale;
		  thread_is_monochrome=is_monochrome;

                  if (ReadBlobZC(image,bytes_per_row,&pixels) != bytes_per_row)
                    thread_status=MagickFail;
              
                  thread_row_count=row_count;
                  row_count++;
              
                  if (image->previous == (Image *) NULL)
                    if (QuantumTick(thread_row_count,image->rows))
                      if (!MagickMonitorFormatted(thread_row_count,image->rows,
                                                  exception,LoadImageText,
                                                  image->filename,
						  image->columns,image->rows))
                        thread_status=MagickFail;
                }

                if (thread_status != MagickFail)
                  if ((q=SetImagePixels(image,0,thread_row_count,image->columns,1)) ==
                      (PixelPacket *) NULL)
                    thread_status=MagickFail;

                if (thread_status != MagickFail)
                  if (!ImportImagePixelArea(image,quantum_type,bits_per_sample,pixels,
					    &import_options,&import_info))
                    thread_status=MagickFail;
                /*
                  For a DirectClass image, check all pixels for
                  gray/monochrome status since this format is often
                  used for input from Ghostscript, which may output
                  bilevel or gray in an RGB format.  It is easier to
                  check now while the pixels are still "hot" in
                  memory.
                */
                if (thread_status != MagickFail)
		  if (check_pixels)
		    if (thread_is_grayscale || thread_is_monochrome)
		      for (x=image->columns; x; x--)
			{
			  thread_is_grayscale = thread_is_grayscale && IsGray(*q);
			  thread_is_monochrome = thread_is_monochrome && IsMonochrome(*q);
			  if (!thread_is_grayscale && !thread_is_monochrome)
			    break;
			  q++;
			}

                if (thread_status != MagickFail)
                  if (!SyncImagePixels(image))
                    thread_status=MagickFail;

#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  pragma omp critical (GM_ReadPNMImage)
#endif
                {
                  if (thread_status == MagickFail)
                    status=MagickFail;

                  if (!thread_is_grayscale)
                    is_grayscale=thread_is_grayscale;

                  if (!thread_is_monochrome)
                    is_monochrome=thread_is_monochrome;
                }
              }
            DestroyThreadViewDataSet(scanline_set);
            image->is_monochrome=is_monochrome;
            image->is_grayscale=is_grayscale;
            if (EOFBlob(image))
              ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,
                             image->filename);
	    break;
	  }
        default:
          ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
        }
      if (scale != (Quantum *) NULL)
        MagickFreeMemory(scale);
      /*
        Proceed to next image.
      */
      if (image_info->subrange != 0)
        if (image->scene >= (image_info->subimage+image_info->subrange-1))
          break;
      if ((format == PBM_ASCII_Format) || (format == PGM_ASCII_Format) || (format == PPM_ASCII_Format))
        do
          {
            /*
              Skip to end of line.
            */
            count=ReadBlob(image,1,&c);
            if (count == 0)
              break;
          } while (c != '\n');
      count=ReadBlob(image,1,&c);
      if ((count != 0) && (c == 'P'))
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
          if (!MagickMonitorFormatted(TellBlob(image),GetBlobSize(image),
                                      exception,LoadImagesText,
                                      image->filename))
            break;
        }
    } while ((count != 0) && (c == 'P'));
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseBlob(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r P N M I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterPNMImage adds attributes for the PNM image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterPNMImage method is:
%
%      RegisterPNMImage(void)
%
*/
ModuleExport void RegisterPNMImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("P7");
  entry->decoder=(DecoderHandler) ReadPNMImage;
  entry->encoder=(EncoderHandler) WritePNMImage;
  entry->description="Xv thumbnail format";
  entry->extension_treatment=IgnoreExtensionTreatment;
  entry->module="PNM";
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("PAM");
  entry->decoder=(DecoderHandler) ReadPNMImage;
  entry->encoder=(EncoderHandler) WritePNMImage;
  entry->description="Portable Arbitrary Map format";
  entry->module="PNM";
  entry->coder_class=PrimaryCoderClass;
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("PBM");
  entry->decoder=(DecoderHandler) ReadPNMImage;
  entry->encoder=(EncoderHandler) WritePNMImage;
  entry->description="Portable bitmap format (black/white)";
  entry->module="PNM";
  entry->coder_class=PrimaryCoderClass;
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("PGM");
  entry->decoder=(DecoderHandler) ReadPNMImage;
  entry->encoder=(EncoderHandler) WritePNMImage;
  entry->description="Portable graymap format (gray scale)";
  entry->module="PNM";
  entry->coder_class=PrimaryCoderClass;
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("PNM");
  entry->decoder=(DecoderHandler) ReadPNMImage;
  entry->encoder=(EncoderHandler) WritePNMImage;
  entry->magick=(MagickHandler) IsPNM;
  entry->description="Portable anymap";
  entry->module="PNM";
  entry->coder_class=PrimaryCoderClass;
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("PPM");
  entry->decoder=(DecoderHandler) ReadPNMImage;
  entry->encoder=(EncoderHandler) WritePNMImage;
  entry->description="Portable pixmap format (color)";
  entry->module="PNM";
  entry->coder_class=PrimaryCoderClass;
  (void) RegisterMagickInfo(entry);

}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r P N M I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterPNMImage removes format registrations made by the
%  PNM module from the list of supported formats.
%
%  The format of the UnregisterPNMImage method is:
%
%      UnregisterPNMImage(void)
%
*/
ModuleExport void UnregisterPNMImage(void)
{
  (void) UnregisterMagickInfo("P7");
  (void) UnregisterMagickInfo("PAM");
  (void) UnregisterMagickInfo("PBM");
  (void) UnregisterMagickInfo("PGM");
  (void) UnregisterMagickInfo("PNM");
  (void) UnregisterMagickInfo("PPM");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P N M I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure WritePNMImage writes an image to a file in the PNM rasterfile
%  format.
%
%  The format of the WritePNMImage method is:
%
%      unsigned int WritePNMImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WritePNMImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
static char *lut_255[] =
{
  "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14",
  "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27",
  "28", "29", "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "40",
  "41", "42", "43", "44", "45", "46", "47", "48", "49", "50", "51", "52", "53",
  "54", "55", "56", "57", "58", "59", "60", "61", "62", "63", "64", "65", "66",
  "67", "68", "69", "70", "71", "72", "73", "74", "75", "76", "77", "78", "79",
  "80", "81", "82", "83", "84", "85", "86", "87", "88", "89", "90", "91", "92",
  "93", "94", "95", "96", "97", "98", "99", "100", "101", "102", "103", "104",
  "105", "106", "107", "108", "109", "110", "111", "112", "113", "114", "115",
  "116", "117", "118", "119", "120", "121", "122", "123", "124", "125", "126",
  "127", "128", "129", "130", "131", "132", "133", "134", "135", "136", "137",
  "138", "139", "140", "141", "142", "143", "144", "145", "146", "147", "148",
  "149", "150", "151", "152", "153", "154", "155", "156", "157", "158", "159",
  "160", "161", "162", "163", "164", "165", "166", "167", "168", "169", "170",
  "171", "172", "173", "174", "175", "176", "177", "178", "179", "180", "181",
  "182", "183", "184", "185", "186", "187", "188", "189", "190", "191", "192",
  "193", "194", "195", "196", "197", "198", "199", "200", "201", "202", "203",
  "204", "205", "206", "207", "208", "209", "210", "211", "212", "213", "214",
  "215", "216", "217", "218", "219", "220", "221", "222", "223", "224", "225",
  "226", "227", "228", "229", "230", "231", "232", "233", "234", "235", "236",
  "237", "238", "239", "240", "241", "242", "243", "244", "245", "246", "247",
  "248", "249", "250", "251", "252", "253", "254", "255"
};

#define AppendUnsignedCharValueToString(j,buffer,value) \
{ \
  const char *lut_entry=lut_255[value]; \
  while(*lut_entry != '\0') \
    { \
      buffer[j++]=*lut_entry; \
      lut_entry++; \
    } \
}

static unsigned int WritePNMImage(const ImageInfo *image_info,Image *image)
{
  char
    buffer[MaxTextExtent];

  const ImageAttribute
    *attribute;

  IndexPacket
    index;

  PNMSubformat
    format;

  unsigned int
    depth;

  unsigned long
    y;

  register const PixelPacket
    *p;

  register const IndexPacket
    *indexes;

  register unsigned long
    i,
    x;

  unsigned int
    scene,
    status;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == MagickFail)
    ThrowWriterException(FileOpenError,UnableToOpenFile,image);
  scene=0;
  do
    {
      depth=(image->depth <= 8 ? 8 : 16);

      /*
	Write PNM file header.
      */
      format=Undefined_PNM_Format;
      if (LocaleCompare(image_info->magick,"P7") == 0)
	{
	  format=XV_332_Format;
	}
      else if (LocaleCompare(image_info->magick,"PPM") == 0)
	{
	  format=PPM_RAW_Format;
	}
      else if (LocaleCompare(image_info->magick,"PGM") == 0)
	{
	  format=PGM_RAW_Format;
	}
      else if (LocaleCompare(image_info->magick,"PBM") == 0)
	{
	  format=PBM_RAW_Format;
	}
      else if (LocaleCompare(image_info->magick,"PAM") == 0)
	{
	  format=PAM_Format;
	}
      else /* PNM auto format */
	{
	  ImageCharacteristics
	    characteristics;

	  /*
	    Make sure that image is in an RGB type space.
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

	  if ((characteristics.monochrome) &&
	      (image_info->type != GrayscaleType) &&
	      (image_info->type != GrayscaleMatteType) &&
	      (image_info->type != TrueColorType) &&
	      (image_info->type != TrueColorMatteType))
	    {
	      /* PBM */
	      format=PBM_RAW_Format;
	    }
	  else if ((characteristics.grayscale) &&
		   (image_info->type != TrueColorType) &&
		   (image_info->type != TrueColorMatteType))
	    {
	      /* PGM */
	      format=PGM_RAW_Format;
	    }
	  else
	    {
	      /* PPM */
	      format=PPM_RAW_Format;
	    }
	}

      /*
	Check if ASCII subformat is requested.
      */
      if ((PBM_RAW_Format == format) || (PGM_RAW_Format == format) | (PPM_RAW_Format == format))
	{
	  MagickBool
	    ascii = MagickFalse;

	  /*
	    If quality is set to zero or "pnm:ascii" is defined, then
	    select an ASCII subformat.
	  */
	  if (image_info->quality == 0)
	    ascii=MagickTrue;
	  else if ((AccessDefinition(image_info,"pnm","ascii")))
	    ascii=MagickTrue;

	  if (ascii)
	    {
	      if (PBM_RAW_Format == format)
		format=PBM_ASCII_Format;
	      else if (PGM_RAW_Format == format)
		format=PGM_ASCII_Format;
	      else if (PPM_RAW_Format == format)
		format=PPM_ASCII_Format;
	    }
	}

      {
	const char *header = "";
	switch (format)
	  {
	  case Undefined_PNM_Format: break;
	  case PBM_ASCII_Format: header="P1"; break;
	  case PGM_ASCII_Format: header="P2"; break;
	  case PPM_ASCII_Format: header="P3"; break;
	  case PBM_RAW_Format:   header="P4"; break;
	  case PGM_RAW_Format:   header="P5"; break;
	  case PPM_RAW_Format:   header="P6"; break;
	  case PAM_Format:       header="P7"; break;
	  case XV_332_Format:    header="P7 332"; break;
	  }
	(void) LogMagickEvent(CoderEvent,GetMagickModule(),"Format Id: %s",
			      header);
	(void) WriteBlobString(image,header);
	(void) WriteBlobByte(image,'\n');
      }

      attribute=GetImageAttribute(image,"comment");
      if (attribute != (const ImageAttribute *) NULL)
	{
	  register char
	    *av;

	  /*
	    Write comments to file.
	  */
	  (void) WriteBlobByte(image,'#');
	  for (av=attribute->value; *av != '\0'; av++)
	    {
	      (void) WriteBlobByte(image,*av);
	      if ((*av == '\n') && (*(av+1) != '\0'))
		(void) WriteBlobByte(image,'#');
	    }
	  (void) WriteBlobByte(image,'\n');
	}
      if ((PAM_Format != format) && (XV_332_Format != format))
	{
	  FormatString(buffer,"%lu %lu\n",image->columns,image->rows);
	  (void) WriteBlobString(image,buffer);
	}
      /*
	Write PNM raster pixels.
      */
      switch (format)
	{
	case PBM_ASCII_Format:
	  {
	    unsigned int
	      polarity;

	    size_t
	      j;

	    /*
	      Convert image to a PBM ASCII image.
	    */
	    (void) SetImageType(image,BilevelType);
	    polarity=PixelIntensityToQuantum(&image->colormap[0]) < (MaxRGB/2);
	    if (image->colors == 2)
	      polarity=PixelIntensityToQuantum(&image->colormap[0]) <
		PixelIntensityToQuantum(&image->colormap[1]);
	    i=0;
	    j=0;
	    for (y=0; y < image->rows; y++)
	      {
		p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
		if (p == (const PixelPacket *) NULL)
		  {
		    status=MagickFail;
		    break;
		  }
		indexes=AccessImmutableIndexes(image);
		for (x=0; x < image->columns; x++)
		  {
		    buffer[j++] = (indexes[x] == polarity ? '0' : '1');
		    buffer[j++] = ' ';
		    i++;
		    if (i == 36)
		      {
			buffer[j++] = '\n';
			i=0;
		      }
		    if (j+4 > sizeof(buffer))
		      {
			status=(WriteBlob(image,j,buffer) == j);
			j=0;
			if (MagickFail == status)
			  break;
		      }
		  }
		if (image->previous == (Image *) NULL)
		  if (QuantumTick(y,image->rows))
		    if (!MagickMonitorFormatted(y,image->rows,&image->exception,
						SaveImageText,image->filename,
						image->columns,image->rows))
		      {
			status=MagickFail;
			break;
		      }
		if (MagickFail == status)
		  break;
	      }
	    if (MagickFail != status)
	      {
		if (i != 0)
		  buffer[j++] = '\n';
		if (j > 0)
		  status=(WriteBlob(image,j,buffer) == j);
	      }
	    break;
	  }
	case PGM_ASCII_Format:
	  {
	    /*
	      Convert image to a PGM ASCII image.
	    */
	    size_t
	      j;

	    unsigned int
	      value;

	    /*
	      Make sure that image is in an RGB type space.
	    */
	    (void) TransformColorspace(image,RGBColorspace);

	    i=0;
	    j=0;
	    
	    if (depth <= 8)
	      value=255;
	    else
	      value=65535;
	    j += sprintf(&buffer[j],"%u\n",value);
	    for (y=0; y < image->rows; y++)
	      {
		p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
		if (p == (const PixelPacket *) NULL)
		  {
		    status=MagickFail;
		    break;
		  }
		for (x=0; x < image->columns; x++)
		  {
		    if (image->is_grayscale)
		      index=p->red;
		    else
		      index=PixelIntensityToQuantum(p);
		    if (depth <= 8)
		      {
			/* Use LUT for speed */
			value=ScaleQuantumToChar(index);
			AppendUnsignedCharValueToString(j,buffer,value);
		      }
		    else
		      {
			value=ScaleQuantumToShort(index);
			j += sprintf(&buffer[j]," %u",value);
		      }
		    i++;
		    if (i == 12)
		      {
			buffer[j++] = '\n';
			i=0;
		      }
		    if (j+8 > sizeof(buffer))
		      {
			status=(WriteBlob(image,j,buffer) == j);
			j=0;
			if (MagickFail == status)
			  break;
		      }
		    p++;
		  }
		if (image->previous == (Image *) NULL)
		  if (QuantumTick(y,image->rows))
		    if (!MagickMonitorFormatted(y,image->rows,&image->exception,
						SaveImageText,image->filename,
						image->columns,image->rows))
		      {
			status=MagickFail;
			break;
		      }
		if (MagickFail == status)
		  break;
	      }
	    if (MagickFail != status)
	      {
		if (i != 0)
		  buffer[j++] = '\n';
		if (j > 0)
		  status=(WriteBlob(image,j,buffer) == j);
	      }
	    break;
	  }
	case PPM_ASCII_Format:
	  {
	    /*
	      Convert image to a PPM ASCII image.
	    */
	    size_t
	      j;

	    unsigned int
	      value;

	    /*
	      Make sure that image is in an RGB type space.
	    */
	    (void) TransformColorspace(image,RGBColorspace);

	    i=0;
	    j=0;

	    if (depth <= 8)
	      value=255;
	    else
	      value=65535;
	    j += sprintf(&buffer[j],"%u\n",value);
	    for (y=0; y < image->rows; y++)
	      {
		p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
		if (p == (const PixelPacket *) NULL)
		  {
		    status=MagickFail;
		    break;
		  }
		for (x=0; x < image->columns; x++)
		  {
		    if (depth <= 8)
		      {
			/* Use LUT for speed */
			value=ScaleQuantumToChar(p->red);
			AppendUnsignedCharValueToString(j,buffer,value);
			buffer[j++] = ' ';
			value=ScaleQuantumToChar(p->green);
			AppendUnsignedCharValueToString(j,buffer,value);
			buffer[j++] = ' ';
			value=ScaleQuantumToChar(p->blue);
			AppendUnsignedCharValueToString(j,buffer,value);
			buffer[j++] = ' ';
		      }
		    else
		      {
			j += sprintf(&buffer[j],"%u %u %u ",ScaleQuantumToShort(p->red),
				     ScaleQuantumToShort(p->green),ScaleQuantumToShort(p->blue));
		      }
		    i++;
		    if (i == 4)
		      {
			buffer[j++] = '\n';
			i=0;
		      }
		    if (j+(8*3) > sizeof(buffer))
		      {
			status=(WriteBlob(image,j,buffer) == j);
			j=0;
			if (MagickFail == status)
			  break;
		      }
		    p++;
		  }
		if (image->previous == (Image *) NULL)
		  if (QuantumTick(y,image->rows))
		    if (!MagickMonitorFormatted(y,image->rows,&image->exception,
						SaveImageText,image->filename,
						image->columns,image->rows))
		      {
			status=MagickFail;
			break;
		      }
		if (MagickFail == status)
		  break;
	      }
	    if (MagickFail != status)
	      {
		if (i != 0)
		  buffer[j++] = '\n';
		if (j > 0)
		  status=(WriteBlob(image,j,buffer) == j);
	      }
	    break;
	  }
	case PBM_RAW_Format:
	case PGM_RAW_Format:
	case PPM_RAW_Format:
	case PAM_Format:
	  {
	    ExportPixelAreaOptions
              export_options;

            size_t
              bytes_per_row;

	    QuantumType
	      quantum_type;
	    
	    unsigned int
	      bits_per_sample,
	      samples_per_pixel;

	    MagickBool
	      grayscale_miniswhite=MagickFalse;

	    unsigned char
	      *pixels;

	    /*
	      Deduce correct export parameters.
	    */
	    bits_per_sample=(depth > 8 ? 16 : 8);
	    quantum_type=RGBQuantum;
	    if (PBM_RAW_Format == format)
	      {
		bits_per_sample=1;
		grayscale_miniswhite=MagickTrue;
		quantum_type=GrayQuantum;
	      }
	    else if (PGM_RAW_Format == format)
	      {
		quantum_type=GrayQuantum;
	      }
	    else if (PPM_RAW_Format == format)
	      {
		quantum_type=RGBQuantum;
	      }
	    else if (PAM_Format == format)
	      {
		ImageCharacteristics
		  characteristics;

		/*
		  Make sure image is of desired type.
		*/
		if (UndefinedType != image_info->type)
		  SetImageType(image,image_info->type);

		/*
		  Analyze the image to get its characteristics.
		*/
		if (!GetImageCharacteristics(image,&characteristics,
					     (OptimizeType == image_info->type),
					     &image->exception))
		  {
		    CloseBlob(image);
		    return MagickFail;
		  }

		/*
		  Choose best encoding based on image characteristics.
		*/
		if (characteristics.cmyk)
		  {
		    if (image->matte)
		      quantum_type=CMYKAQuantum;
		    else
		      quantum_type=CMYKQuantum;
		  }
		else if (characteristics.monochrome)
		  {
		    bits_per_sample=1;
		    grayscale_miniswhite=MagickTrue;

		    if (image->matte)
		      quantum_type=GrayAlphaQuantum;
		    else
		      quantum_type=GrayQuantum;
		  }
		else if (characteristics.grayscale)
		  {
		    if (image->matte)
		      quantum_type=GrayAlphaQuantum;
		    else
		      quantum_type=GrayQuantum;
		  }
		else
		  {
		    if (image->matte)
		      quantum_type=RGBAQuantum;
		    else
		      quantum_type=RGBQuantum;
		  }
	      }

	    samples_per_pixel=MagickGetQuantumSamplesPerPixel(quantum_type);

	    if (1 == bits_per_sample)
	      bytes_per_row=((image->columns+7) >> 3);
	    else
	      bytes_per_row=(((bits_per_sample+7)/8)*samples_per_pixel)*image->columns;

	    ExportPixelAreaOptionsInit(&export_options);
	    export_options.grayscale_miniswhite=grayscale_miniswhite;

	    /*
	      Allocate memory for pixels.
	    */
	    pixels=MagickAllocateMemory(unsigned char *,bytes_per_row);
	    if (pixels == (unsigned char *) NULL)
	      ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,
				   image);

	    /*
	      Output header details
	    */
	    if (PAM_Format == format)
	      {
		/*
		  PAM header
		*/
		const char *tuple_type=NULL;

		if (GrayQuantum == quantum_type)
		  tuple_type="BLACKANDWHITE";
		else if (GrayAlphaQuantum == quantum_type)
		  tuple_type="BLACKANDWHITE_ALPHA";
		else if (RGBQuantum == quantum_type)
		  tuple_type="RGB";
		else if (RGBAQuantum == quantum_type)
		  tuple_type="RGB_ALPHA";
		else if (CMYKQuantum == quantum_type)
		  tuple_type="CMYK";
		else if (CMYKAQuantum == quantum_type)
		  tuple_type="CMYK_ALPHA";

		FormatString(buffer,"WIDTH %lu\nHEIGHT %lu\nDEPTH %u\nMAXVAL %lu\nTUPLTYPE %s\n",
			     image->columns,image->rows,samples_per_pixel,
			     MaxValueGivenBits(bits_per_sample),tuple_type);
		WriteBlobString(image,buffer);

		(void) WriteBlobString(image,"ENDHDR\n");
	      }
	    else if ((PGM_RAW_Format == format) || (PPM_RAW_Format == format))
	      {
		/*
		  PGM, PPM header
		*/
		FormatString(buffer,"%lu\n",MaxValueGivenBits(bits_per_sample));
		WriteBlobString(image,buffer);
	      }

	    /*
	      Output pixels
	    */
	    for (y=0; y < image->rows; y++)
	      {
		p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
		if (p == (const PixelPacket *) NULL)
		  break;
		if (ExportImagePixelArea(image,quantum_type,bits_per_sample,pixels,&export_options,0) == MagickFail)
		  break;
		if (WriteBlob(image,bytes_per_row,(char *) pixels) != bytes_per_row)
		  break;
		if (image->previous == (Image *) NULL)
		  if (QuantumTick(y,image->rows))
		    if (!MagickMonitorFormatted(y,image->rows,&image->exception,
						SaveImageText,image->filename,
						image->columns,image->rows))
		      break;
	      }
	    MagickFreeMemory(pixels);

	    break;
	  }
	case XV_332_Format:
	  {
	    static const short int
	      dither_red[2][16]=
	      {
		{-16,  4, -1, 11,-14,  6, -3,  9,-15,  5, -2, 10,-13,  7, -4,  8},
		{ 15, -5,  0,-12, 13, -7,  2,-10, 14, -6,  1,-11, 12, -8,  3, -9}
	      },
	      dither_green[2][16]=
		{
		  { 11,-15,  7, -3,  8,-14,  4, -2, 10,-16,  6, -4,  9,-13,  5, -1},
		  {-12, 14, -8,  2, -9, 13, -5,  1,-11, 15, -7,  3,-10, 12, -6,  0}
		},
		dither_blue[2][16]=
		  {
		    { -3,  9,-13,  7, -1, 11,-15,  5, -4,  8,-14,  6, -2, 10,-16,  4},
		    {  2,-10, 12, -8,  0,-12, 14, -6,  3, -9, 13, -7,  1,-11, 15, -5}
		  };

		long
		  value;

		Quantum
		  pixel;

		unsigned short
		  *blue_map[2][16],
		  *green_map[2][16],
		  *red_map[2][16];

		unsigned int
		  j;

		/*
		  Allocate and initialize dither maps.
		*/
		for (i=0; i < 2; i++)
		  for (j=0; j < 16; j++)
		    {
		      red_map[i][j]=MagickAllocateMemory(unsigned short *,
							 256*sizeof(unsigned short));
		      green_map[i][j]=MagickAllocateMemory(unsigned short *,
							   256*sizeof(unsigned short));
		      blue_map[i][j]=MagickAllocateMemory(unsigned short *,
							  256*sizeof(unsigned short));
		      if ((red_map[i][j] == (unsigned short *) NULL) ||
			  (green_map[i][j] == (unsigned short *) NULL) ||
			  (blue_map[i][j] == (unsigned short *) NULL))
			ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,
					     image);
		    }
		/*
		  Initialize dither tables.
		*/
		for (i=0; i < 2; i++)
		  for (j=0; j < 16; j++)
		    for (x=0; x < 256; x++)
		      {
			value=x-16;
			if (x < 48)
			  value=x/2+8;
			value+=dither_red[i][j];
			red_map[i][j][x]=(unsigned short)
			  ((value < 0) ? 0 : (value > 255) ? 255 : value);
			value=x-16;
			if (x < 48)
			  value=x/2+8;
			value+=dither_green[i][j];
			green_map[i][j][x]=(unsigned short)
			  ((value < 0) ? 0 : (value > 255) ? 255 : value);
			value=x-32;
			if (x < 112)
			  value=x/2+24;
			value+=2*dither_blue[i][j];
			blue_map[i][j][x]=(unsigned short)
			  ((value < 0) ? 0 : (value > 255) ? 255 : value);
		      }
		/*
		  Write pixels.
		*/
		(void) WriteBlobString(image,"#END_OF_COMMENTS\n");
		FormatString(buffer,"%lu %lu 255\n",image->columns,image->rows);
		(void) WriteBlobString(image,buffer);
		i=0;
		j=0;
		for (y=0; y < image->rows; y++)
		  {
		    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
		    if (p == (const PixelPacket *) NULL)
		      break;
		    for (x=0; x < image->columns; x++)
		      {
			if (!image_info->dither)
			  pixel=(Quantum) ((ScaleQuantumToChar(p->red) & 0xe0) |
					   ((ScaleQuantumToChar(p->green) & 0xe0) >> 3) |
					   ((ScaleQuantumToChar(p->blue) & 0xc0) >> 6));
			else
			  pixel=(Quantum)
			    ((red_map[i][j][ScaleQuantumToChar(p->red)] & 0xe0) |
			     ((green_map[i][j][ScaleQuantumToChar(p->green)] & 0xe0) >> 3) |
			     ((blue_map[i][j][ScaleQuantumToChar(p->blue)] & 0xc0) >> 6));
			(void) WriteBlobByte(image,pixel);
			p++;
			j++;
			if (j == 16)
			  j=0;
		      }
		    i++;
		    if (i == 2)
		      i=0;
		    if (QuantumTick(y,image->rows))
		      if (!MagickMonitorFormatted(y,image->rows,&image->exception,
						  SaveImageText,image->filename,
						  image->columns,image->rows))
			break;
		  }
		/*
		  Free allocated memory.
		*/
		for (i=0; i < 2; i++)
		  for (j=0; j < 16; j++)
		    {
		      MagickFreeMemory(green_map[i][j]);
		      MagickFreeMemory(blue_map[i][j]);
		      MagickFreeMemory(red_map[i][j]);
		    }
		break;
	  }
	case Undefined_PNM_Format:
	  break;
	}
      if (image->next == (Image *) NULL)
	break;
      image=SyncNextImageInList(image);
      status=MagickMonitorFormatted(scene++,GetImageListLength(image),
				    &image->exception,SaveImagesText,
				    image->filename);
      if (status == False)
	break;
    } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseBlob(image);
  return(True);
}
