/*
  Copyright (C) 2003 - 2012 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio

  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  Methods to export image pixels to common representations

*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/bit_stream.h"
#include "magick/colormap.h"
#include "magick/constitute.h"
#include "magick/floats.h"
#include "magick/magick.h"
#include "magick/pixel_cache.h"

/*
  Type definitions
*/
static const PixelPacket BlackPixel = {0, 0, 0, OpaqueOpacity};

static const PixelPacket WhitePixel = {MaxRGB, MaxRGB, MaxRGB, OpaqueOpacity};

/*
  Forward declarations.
*/


/*
  Macros
*/

#if defined(WORDS_BIGENDIAN)
#  define MyEndianType MSBEndian
#else
#  define MyEndianType LSBEndian
#endif

#if 0
#define ExportModulo8Quantum(q,quantum_size,quantum)    \
  {                                                     \
    register unsigned int                               \
      shift=quantum_size;                               \
                                                        \
    do                                                  \
      {                                                 \
        shift -= 8U;                                    \
        *q++=(unsigned char)                            \
          (((unsigned int) quantum) >> shift);          \
      } while( shift > 0U);                             \
  }
#endif
#define ExportUInt8Quantum(q,quantum)           \
  {                                             \
    *q++=(quantum);                             \
  }
#define ExportUInt16Quantum(endian,q,quantum)   \
  {                                             \
    register unsigned int value_ = (quantum);   \
    if (LSBEndian != endian)                    \
      {                                         \
        *q++=(unsigned char) (value_ >> 8);     \
        *q++=(unsigned char) value_;            \
      }                                         \
    else                                        \
      {                                         \
        *q++=(unsigned char) value_;            \
        *q++=(unsigned char) (value_ >> 8);     \
      }                                         \
}
#define ExportUInt32Quantum(endian,q,quantum)   \
  {                                             \
    register unsigned int value_ = (quantum);   \
    if (LSBEndian != endian)                    \
      {                                         \
        *q++=(unsigned char) (value_ >> 24);    \
        *q++=(unsigned char) (value_ >> 16);    \
        *q++=(unsigned char) (value_ >> 8);     \
        *q++=(unsigned char) value_;            \
      }                                         \
    else                                        \
      {                                         \
        *q++=(unsigned char) value_;            \
        *q++=(unsigned char) (value_ >> 8);     \
        *q++=(unsigned char) (value_ >> 16);    \
        *q++=(unsigned char) (value_ >> 24);    \
      }                                         \
  }
/*
  Export a 32-bit unsigned quantum into a 64-bit storage size.  This
  approach is used since 64-bits is not supported internally and some
  CPUs may perform poorly if the quantum is a 64-bit type.
*/
#define ExportUInt64Quantum(endian,q,quantum)   \
  {                                             \
    register unsigned int value_ = (quantum);   \
    if (LSBEndian != endian)                    \
      {                                         \
        *q++=(unsigned char) (value_ >> 24);    \
        *q++=(unsigned char) (value_ >> 16);    \
        *q++=(unsigned char) (value_ >> 8);     \
        *q++=(unsigned char) value_;            \
        *q++=(unsigned char) (value_ >> 24);    \
        *q++=(unsigned char) (value_ >> 16);    \
        *q++=(unsigned char) (value_ >> 8);     \
        *q++=(unsigned char) value_;            \
      }                                         \
    else                                        \
      {                                         \
        *q++=(unsigned char) value_;            \
        *q++=(unsigned char) (value_ >> 8);     \
        *q++=(unsigned char) (value_ >> 16);    \
        *q++=(unsigned char) (value_ >> 24);    \
        *q++=(unsigned char) value_;            \
        *q++=(unsigned char) (value_ >> 8);     \
        *q++=(unsigned char) (value_ >> 16);    \
        *q++=(unsigned char) (value_ >> 24);    \
      }                                         \
  }
#define ExportFloat16Quantum(endian,q,quantum)	    \
  {						    \
    float float_val;				    \
    unsigned char c[2];				    \
						    \
    float_val=(float) (quantum);		    \
    (void) _Gm_convert_fp32_to_fp16(&float_val,     \
				    (fp_16bits *)c, \
				    RANGE_LIMITED); \
    if (MyEndianType == endian)			    \
      {						    \
        *q++=c[0];				    \
        *q++=c[1];				    \
      }						    \
    else					    \
      {						    \
        *q++=c[1];				    \
        *q++=c[0];				    \
      }						    \
  }
#define ExportFloat24Quantum(endian,q,quantum)		\
  {							\
    float float_val;					\
    unsigned char c[3];					\
							\
    float_val=(float) (quantum);			\
    (void) _Gm_convert_fp32_to_fp24(&float_val,		\
				    (fp_24bits *)c,	\
				    RANGE_LIMITED);	\
    if (MyEndianType == endian)				\
      {							\
        *q++=c[0];					\
        *q++=c[1];					\
        *q++=c[2];					\
      }							\
    else						\
      {							\
        *q++=c[2];					\
        *q++=c[1];					\
        *q++=c[0];					\
      }							\
  }
#define ExportFloat32Quantum(endian,q,quantum)  \
  {                                             \
    if (MyEndianType == endian)                 \
      {                                         \
        *((float *) q) = (float) (quantum);     \
        q += sizeof(float);                     \
      }                                         \
    else                                        \
      {                                         \
        union                                   \
        {                                       \
          float f;                              \
          unsigned char c[4];                   \
        } fu_;                                  \
                                                \
        fu_.f=(float) (quantum);                \
        *q++=fu_.c[3];                          \
        *q++=fu_.c[2];                          \
        *q++=fu_.c[1];                          \
        *q++=fu_.c[0];                          \
      }                                         \
  }
#define ExportFloat64Quantum(endian,q,quantum)  \
  {                                             \
    if (MyEndianType == endian)                 \
      {                                         \
        *((double *) q) = ((double) quantum);   \
        q += sizeof(double);                    \
      }                                         \
    else                                        \
      {                                         \
        union                                   \
        {                                       \
          double d;                             \
          unsigned char c[8];                   \
        } du_;                                  \
                                                \
        du_.d = (double) (quantum);             \
        *q++=du_.c[7];                          \
        *q++=du_.c[6];                          \
        *q++=du_.c[5];                          \
        *q++=du_.c[4];                          \
        *q++=du_.c[3];                          \
        *q++=du_.c[2];                          \
        *q++=du_.c[1];                          \
        *q++=du_.c[0];                          \
      }                                         \
  }

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   E x p o r t I m a g e P i x e l A r e a                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ExportImagePixelArea() transfers one or more pixel components from the
%  default image pixel cache view to a user supplied buffer.  By default,
%  values are written in network (big-endian) byte/bit order.  By setting
%  the 'endian' member of ExportPixelAreaOptions, 16, 32 and 64-bit values
%  may be output as little (LSBEndian), big (MSBEndian), or host native
%  (NativeEndian) endian values.  This function is quite powerful in that
%  besides common native CPU type sizes, it can support any integer bit
%  depth from 1 to 32 (e.g. 13), 64-bits as well as 32 and 64-bit float.
%  
%
%  MagickPass is returned if the pixels are successfully transferred,
%  otherwise MagickFail.
%
%  The format of the ExportImagePixelArea method is:
%
%      MagickPassFail ExportImagePixelArea(const Image *image,
%                                          const QuantumType quantum_type,
%                                          unsigned int quantum_size,
%                                          unsigned char *destination,
%                                          const ExportPixelAreaOptions *options,
%                                          ExportPixelAreaInfo *export_info)
%
%  A description of each parameter follows:
%
%    o status: Returns MagickPass if the pixels are successfully transferred,
%              otherwise MagickFail.
%
%    o image: The image.
%
%    o quantum_type: Declare which pixel components to transfer (AlphaQuantum,
%        BlackQuantum, BlueQuantum, CMYKAQuantum, CMYKQuantum, CyanQuantum,
%        GrayAlphaQuantum, GrayQuantum, GreenQuantum, IndexAlphaQuantum,
%        IndexQuantum, MagentaQuantum, RGBAQuantum, RGBQuantum,
%        RedQuantum, YellowQuantum)
%
%    o quantum_size: Bits per quantum sample (range 1-32, and 64).
%
%    o destination:  The components are transferred to this buffer.  The user
%        is responsible for ensuring that the destination buffer is large
%        enough.
%
%    o options: Additional options specific to quantum_type (may be NULL).
%
%    o export_info : Populated with information regarding the pixels
%        exported (may be NULL)
%
*/
MagickExport MagickPassFail ExportImagePixelArea(const Image *image,
  const QuantumType quantum_type,const unsigned int quantum_size,
  unsigned char *destination,const ExportPixelAreaOptions *options,
  ExportPixelAreaInfo *export_info)
{
  return ExportViewPixelArea(AccessDefaultCacheView(image),
                             quantum_type,quantum_size,
                             destination,options,export_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   E x p o r t V i e w P i x e l A r e a                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ExportViewPixelArea() transfers one or more pixel components from the
%  specified image pixel cache view to a user supplied buffer.  By default,
%  values are written in network (big-endian) byte/bit order.  By setting the
%  'endian' member of ExportPixelAreaOptions, 16, 32 and 64-bit values may be
%  output as little (LSBEndian), big (MSBEndian), or host native
%  (NativeEndian) endian values.  This function is quite powerful in that
%  besides common native CPU type sizes, it can support any integer bit depth
%  from 1 to 32 (e.g. 13), 64-bits as well as 32 and 64-bit float.
%  
%
%  MagickPass is returned if the pixels are successfully transferred,
%  otherwise MagickFail.
%
%  The format of the ExportViewPixelArea method is:
%
%      MagickPassFail ExportViewPixelArea(const ViewInfo *view,
%                                         const QuantumType quantum_type,
%                                         unsigned int quantum_size,
%                                         unsigned char *destination,
%                                         const ExportPixelAreaOptions *options,
%                                         ExportPixelAreaInfo *export_info)
%
%  A description of each parameter follows:
%
%    o status: Returns MagickPass if the pixels are successfully transferred,
%              otherwise MagickFail.
%
%    o view: The image pixel cache view.
%
%    o quantum_type: Declare which pixel components to transfer (AlphaQuantum,
%        BlackQuantum, BlueQuantum, CMYKAQuantum, CMYKQuantum, CyanQuantum,
%        GrayAlphaQuantum, GrayQuantum, GreenQuantum, IndexAlphaQuantum,
%        IndexQuantum, MagentaQuantum, RGBAQuantum, RGBQuantum,
%        RedQuantum, YellowQuantum)
%
%    o quantum_size: Bits per quantum sample (range 1-32, and 64).
%
%    o destination:  The components are transferred to this buffer.  The user
%        is responsible for ensuring that the destination buffer is large
%        enough.
%
%    o options: Additional options specific to quantum_type (may be NULL).
%
%    o export_info : Populated with information regarding the pixels
%        exported (may be NULL)
%
*/
static MagickPassFail
ExportIndexQuantumType(unsigned char * restrict destination,
		       const IndexPacket * restrict indexes,
		       const unsigned long number_pixels,
		       const unsigned int quantum_size,
		       const QuantumSampleType sample_type,
		       const EndianType endian,
		       const Image *image,
		       unsigned long *bytes_exported)
{
  register unsigned long
    x;

  register unsigned char
    *q;

  q=destination;

  assert(indexes != (const IndexPacket *) NULL);
  assert(image->colors <= MaxColormapSize);

  if (sample_type == UnsignedQuantumSampleType)
    {
      switch (quantum_size)
	{
	case 1:
	  {
	    /*
	      Special "fast" support for two-color PsudeoClass.
	    */
	    register unsigned int
	      bit=0,
	      byte=0;
                  
	    for (x = number_pixels; x != 0; --x)
	      {
		byte<<=1;
		byte |= (*indexes++ ? 1U : 0U);
		bit++;
		if (bit == 8)
		  {
		    *q++=byte;
		    bit=0;
		    byte=0;
		  }
	      }
	    if (bit != 0)
	      {
		*q++=byte << (8-bit);
	      }
	    break;
	  }
	case 4:
	  {
	    /*
	      Special "fast" support for four-color PsudeoClass.
	    */
	    register unsigned int
	      state = 0;
                  
	    for (x = number_pixels ; x != 0 ; --x )
	      {
		state ^= 1; /* Produces 1 0 1 0 ... */
		if (state)
		  {
		    *q = ((*indexes & 0xf) << 4);
		  }
		else
		  {
		    *q |= ((*indexes & 0xf));
		    q++;
		  }
		indexes++;
	      }
	    if (1 == state)
	      {
		q++;
	      }
	    break;
	  }
	case 8:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt8Quantum(q,*indexes);
		indexes++;
	      }
	    break;
	  }
	case 16:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt16Quantum(endian,q,*indexes);
		indexes++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt32Quantum(endian,q,*indexes);
		indexes++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt64Quantum(endian,q,*indexes);
		indexes++;
	      }
	    break;
	  }
	default:
	  {
	    /*
	      Arbitrary sample size
	    */
	    BitStreamWriteHandle
	      stream;
                  
	    MagickBitStreamInitializeWrite(&stream,q);
	    for (x = number_pixels; x != 0; --x)
	      {
		MagickBitStreamMSBWrite(&stream,quantum_size,*indexes);
		indexes++;
	      }
	    q=stream.bytes;
	    break;
	  }
	}
    }

  /*
    Collect bytes exported
  */
  *bytes_exported=(q-destination);

  return MagickPass;
}

static MagickPassFail
ExportIndexAlphaQuantumType(unsigned char * restrict destination,
			    const PixelPacket * restrict pixels,
			    const IndexPacket * restrict indexes,
			    const unsigned long number_pixels,
			    const unsigned int quantum_size,
			    const QuantumSampleType sample_type,
			    const EndianType endian,
			    const unsigned int unsigned_scale,
			    const Image *image,
			    unsigned long *bytes_exported)
{

  register unsigned char
    *q = destination;

  register const PixelPacket
    *p = pixels;

  register unsigned long
    x;

  register unsigned int
    unsigned_value;

  assert(image->colors <= MaxColormapSize);
  if (sample_type == UnsignedQuantumSampleType)
    {
      switch (quantum_size)
	{
	case 8:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt8Quantum(q,*indexes);
		ExportUInt8Quantum(q,ScaleQuantumToChar(MaxRGB-GetOpacitySample(p)));
		indexes++;
		p++;
	      }
	    break;
	  }
	case 16:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt16Quantum(endian,q,*indexes);
		ExportUInt16Quantum(endian,q,ScaleQuantumToShort(MaxRGB-GetOpacitySample(p)));
		indexes++;
		p++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt32Quantum(endian,q,*indexes);
		ExportUInt32Quantum(endian,q,ScaleQuantumToLong(MaxRGB-GetOpacitySample(p)));
		indexes++;
		p++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt64Quantum(endian,q,*indexes);
		ExportUInt64Quantum(endian,q,ScaleQuantumToLong(MaxRGB-GetOpacitySample(p)));
		indexes++;
		p++;
	      }
	    break;
	  }
	default:
	  {
	    /*
	      Arbitrary sample size
	    */
	    BitStreamWriteHandle
	      stream;
                  
	    MagickBitStreamInitializeWrite(&stream,q);
	    for (x = number_pixels; x != 0; --x)
	      {
		MagickBitStreamMSBWrite(&stream,quantum_size,*indexes);
		unsigned_value=MaxRGB-GetOpacitySample(p);
		if (QuantumDepth >  quantum_size)
		  unsigned_value /= unsigned_scale;
		else if (QuantumDepth <  quantum_size)
		  unsigned_value *= unsigned_scale;
		MagickBitStreamMSBWrite(&stream,quantum_size,unsigned_value);
		indexes++;
		p++;
	      }
	    q=stream.bytes;
	    break;
	  }
	}
    }

  /*
    Collect bytes exported
  */
  *bytes_exported=(q-destination);

  return MagickPass;
}

static MagickPassFail
ExportGrayQuantumType(unsigned char * restrict destination,
		      const PixelPacket * restrict pixels,
		      const IndexPacket * restrict indexes,
		      const unsigned long number_pixels,
		      const unsigned int quantum_size,
		      const QuantumSampleType sample_type,
		      const EndianType endian,
		      const unsigned int unsigned_scale,
		      const MagickBool grayscale_miniswhite,
		      const unsigned int sample_bits,
		      const double double_minvalue,
		      const double double_scale,
		      const Image *image,
		      unsigned long *bytes_exported)
{

  register unsigned char
    *q = destination;

  register const PixelPacket
    *p = pixels;

  register unsigned long
    x;

  register unsigned int
    unsigned_value;

  if (sample_type == UnsignedQuantumSampleType)
    {
      switch (quantum_size)
	{
	case 1:
	  {
	    if (image->storage_class == PseudoClass)
	      {
		/*
		  Special "fast" support for two-color PsudeoClass.
		*/
		register unsigned int
		  bit=0U,
		  byte=0U,
		  polarity;

		polarity=PixelIntensityToQuantum(&image->colormap[0]) < (MaxRGB/2);
		if (image->colors == 2U)
		  polarity=PixelIntensityToQuantum(&image->colormap[0]) <
		    PixelIntensityToQuantum(&image->colormap[1]);
		if (grayscale_miniswhite)
		  polarity=!polarity;
                      
		for (x = number_pixels; x != 0UL; --x)
		  {
		    byte <<= 1U;
		    if (*indexes++ == polarity)
		      byte |= 1U;
		    bit++;
		    if (bit == 8U)
		      {
			*q++=byte;
			bit=0U;
			byte=0U;
		      }
		  }
		if (bit != 0U)
		  {
		    *q++=byte << (8U-bit);
		  }
	      }
	    else
	      {
		/*
		  Special "fast" support for bi-level gray.
		  Performs 50% thresholding for best appearance.
		*/
		register unsigned int
		  bit=0U,
		  byte=0U,
		  black=0U,
		  white=1U;

		if (grayscale_miniswhite)
		  {
		    black=1U;
		    white=0U;
		  }

		for (x = number_pixels ; x != 0UL ; --x )
		  {
		    if (image->is_grayscale)
		      unsigned_value=GetGraySample(p);
		    else
		      unsigned_value=PixelIntensityToQuantum(p);
		    byte <<= 1;
		    byte |= ((unsigned_value > MaxRGB/2U) ? white : black);
		    bit++;
		    if (bit == 8U)
		      {
			*q++=byte;
			bit=0U;
			byte=0U;
		      }
		    p++;
		  }
		if (bit != 0)
		  {
		    *q++=byte << (8-bit);
		  }
	      }
	    break;
	  }
	case 8:
	  {
	    if (image->is_grayscale)
	      {
		if (grayscale_miniswhite)
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ExportUInt8Quantum(q,ScaleQuantumToChar(MaxRGB-GetGraySample(p)));
			p++;
		      }
		  }
		else
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ExportUInt8Quantum(q,ScaleQuantumToChar(GetGraySample(p)));
			p++;
		      }
		  }
	      }
	    else
	      {
		if (grayscale_miniswhite)
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ExportUInt8Quantum(q,ScaleQuantumToChar(MaxRGB-PixelIntensityToQuantum(p)));
			p++;
		      }
		  }
		else
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ExportUInt8Quantum(q,ScaleQuantumToChar(PixelIntensityToQuantum(p)));
			p++;
		      }
		  }
	      }
	    break;
	  }
	case 16:
	  {
	    if (image->is_grayscale)
	      {
		if (grayscale_miniswhite)
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ExportUInt16Quantum(endian,q,ScaleQuantumToShort(MaxRGB-GetGraySample(p)));
			p++;
		      }
		  }
		else
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ExportUInt16Quantum(endian,q,ScaleQuantumToShort(GetGraySample(p)));
			p++;
		      }
		  }
	      }
	    else
	      {
		if (grayscale_miniswhite)
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ExportUInt16Quantum(endian,q,ScaleQuantumToShort(MaxRGB-PixelIntensityToQuantum(p)));
			p++;
		      }
		  }
		else
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ExportUInt16Quantum(endian,q,ScaleQuantumToShort(PixelIntensityToQuantum(p)));
			p++;
		      }
		  }
	      }
	    break;
	  }
	case 32:
	  {
	    if (image->is_grayscale)
	      {
		if (grayscale_miniswhite)
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ExportUInt32Quantum(endian,q,ScaleQuantumToLong(MaxRGB-GetGraySample(p)));
			p++;
		      }
		  }
		else
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ExportUInt32Quantum(endian,q,ScaleQuantumToLong(GetGraySample(p)));
			p++;
		      }
		  }
	      }
	    else
	      {
		if (grayscale_miniswhite)
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ExportUInt32Quantum(endian,q,ScaleQuantumToLong(MaxRGB-PixelIntensityToQuantum(p)));
			p++;
		      }
		  }
		else
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ExportUInt32Quantum(endian,q,ScaleQuantumToLong(PixelIntensityToQuantum(p)));
			p++;
		      }
		  }
	      }
	    break;
	  }
	case 64:
	  {
	    if (image->is_grayscale)
	      {
		if (grayscale_miniswhite)
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ExportUInt64Quantum(endian,q,ScaleQuantumToLong(MaxRGB-GetGraySample(p)));
			p++;
		      }
		  }
		else
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ExportUInt64Quantum(endian,q,ScaleQuantumToLong(GetGraySample(p)));
			p++;
		      }
		  }
	      }
	    else
	      {
		if (grayscale_miniswhite)
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ExportUInt64Quantum(endian,q,ScaleQuantumToLong(MaxRGB-PixelIntensityToQuantum(p)));
			p++;
		      }
		  }
		else
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ExportUInt64Quantum(endian,q,ScaleQuantumToLong(PixelIntensityToQuantum(p)));
			p++;
		      }
		  }
	      }
	    break;
	  }
	default:
	  {
	    /*
	      Arbitrary sample size
	    */
	    BitStreamWriteHandle
	      stream;

	    MagickBitStreamInitializeWrite(&stream,q);
	    if(QuantumDepth == sample_bits)
	      {
		/* Unity scale */
		for (x = number_pixels; x != 0; --x)
		  {
		    if (image->is_grayscale)
		      unsigned_value=GetGraySample(p);
		    else
		      unsigned_value=PixelIntensityToQuantum(p);
		    if (grayscale_miniswhite)
		      unsigned_value=MaxRGB-unsigned_value;
		    MagickBitStreamMSBWrite(&stream,quantum_size,unsigned_value);
		    p++;
		  }
	      }
	    else if (QuantumDepth >  sample_bits)
	      {
		/* Scale down */
		for (x = number_pixels; x != 0; --x)
		  {
		    if (image->is_grayscale)
		      unsigned_value=GetGraySample(p);
		    else
		      unsigned_value=PixelIntensityToQuantum(p);
		    if (grayscale_miniswhite)
		      unsigned_value=MaxRGB-unsigned_value;
		    unsigned_value /= unsigned_scale;
		    MagickBitStreamMSBWrite(&stream,quantum_size,unsigned_value);
		    p++;
		  }
	      }
	    else
	      {
		/* Scale up */
		for (x = number_pixels; x != 0; --x)
		  {
		    if (image->is_grayscale)
		      unsigned_value=GetGraySample(p);
		    else
		      unsigned_value=PixelIntensityToQuantum(p);
		    if (grayscale_miniswhite)
		      unsigned_value=MaxRGB-unsigned_value;
		    unsigned_value *= unsigned_scale;
		    MagickBitStreamMSBWrite(&stream,quantum_size,unsigned_value);
		    p++;
		  }
	      }
	    q=stream.bytes;
	    break;
	  }
	}
    }
  else if (sample_type == FloatQuantumSampleType)
    {
      switch (quantum_size)
	{
	case 16:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat16Quantum(endian,q,PixelIntensity(p)*double_scale+double_minvalue);
		p++;
	      }
	    break;
	  }
	case 24:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat24Quantum(endian,q,PixelIntensity(p)*double_scale+double_minvalue);
		p++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat32Quantum(endian,q,PixelIntensity(p)*double_scale+double_minvalue);
		p++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat64Quantum(endian,q,PixelIntensity(p)*double_scale+double_minvalue);
		p++;
	      }
	    break;
	  }
	default:
	  break;
	}
    }

  /*
    Collect bytes exported
  */
  *bytes_exported=(q-destination);

  return MagickPass;
}

static MagickPassFail
ExportGrayAlphaQuantumType(unsigned char * restrict destination,
			   const PixelPacket * restrict pixels,
			   const unsigned long number_pixels,
			   const unsigned int quantum_size,
			   const QuantumSampleType sample_type,
			   const EndianType endian,
			   const unsigned int unsigned_scale,
			   const MagickBool grayscale_miniswhite,
			   const unsigned int sample_bits,
			   const double double_minvalue,
			   const double double_scale,
			   const Image *image,
			   unsigned long *bytes_exported)
{

  register unsigned char
    *q = destination;

  register const PixelPacket
    *p = pixels;

  register unsigned long
    x;

  register unsigned int
    unsigned_value;

  if (sample_type == UnsignedQuantumSampleType)
    {
      switch (quantum_size)
	{
	case 8:
	  {
	    if (image->is_grayscale)
	      {
		if (grayscale_miniswhite)
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ExportUInt8Quantum(q,ScaleQuantumToChar(MaxRGB-GetRedSample(p)));
			ExportUInt8Quantum(q,ScaleQuantumToChar(MaxRGB-GetOpacitySample(p)));
			p++;
		      }
		  }
		else
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ExportUInt8Quantum(q,ScaleQuantumToChar(GetRedSample(p)));
			ExportUInt8Quantum(q,ScaleQuantumToChar(MaxRGB-GetOpacitySample(p)));
			p++;
		      }
		  }
	      }
	    else
	      {
		if (grayscale_miniswhite)
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ExportUInt8Quantum(q,ScaleQuantumToChar(MaxRGB-PixelIntensityToQuantum(p)));
			ExportUInt8Quantum(q,ScaleQuantumToChar(MaxRGB-GetOpacitySample(p)));
			p++;
		      }
		  }
		else
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ExportUInt8Quantum(q,ScaleQuantumToChar(PixelIntensityToQuantum(p)));
			ExportUInt8Quantum(q,ScaleQuantumToChar(MaxRGB-GetOpacitySample(p)));
			p++;
		      }
		  }
	      }
	    break;
	  }
	case 16:
	  {
	    if (image->is_grayscale)
	      {
		if (grayscale_miniswhite)
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ExportUInt16Quantum(endian,q,ScaleQuantumToShort(MaxRGB-GetRedSample(p)));
			ExportUInt16Quantum(endian,q,ScaleQuantumToShort(MaxRGB-GetOpacitySample(p)));
			p++;
		      }
		  }
		else
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ExportUInt16Quantum(endian,q,ScaleQuantumToShort(GetRedSample(p)));
			ExportUInt16Quantum(endian,q,ScaleQuantumToShort(MaxRGB-GetOpacitySample(p)));
			p++;
		      }
		  }
	      }
	    else
	      {
		if (grayscale_miniswhite)
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ExportUInt16Quantum(endian,q,ScaleQuantumToShort(MaxRGB-PixelIntensityToQuantum(p)));
			ExportUInt16Quantum(endian,q,ScaleQuantumToShort(MaxRGB-GetOpacitySample(p)));
			p++;
		      }
		  }
		else
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ExportUInt16Quantum(endian,q,ScaleQuantumToShort(PixelIntensityToQuantum(p)));
			ExportUInt16Quantum(endian,q,ScaleQuantumToShort(MaxRGB-GetOpacitySample(p)));
			p++;
		      }
		  }
	      }
	    break;
	  }
	case 32:
	  {
	    if (image->is_grayscale)
	      {
		if (grayscale_miniswhite)
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ExportUInt32Quantum(endian,q,ScaleQuantumToLong(MaxRGB-GetRedSample(p)));
			ExportUInt32Quantum(endian,q,ScaleQuantumToLong(MaxRGB-GetOpacitySample(p)));
			p++;
		      }
		  }
		else
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ExportUInt32Quantum(endian,q,ScaleQuantumToLong(GetRedSample(p)));
			ExportUInt32Quantum(endian,q,ScaleQuantumToLong(MaxRGB-GetOpacitySample(p)));
			p++;
		      }
		  }
	      }
	    else
	      {
		if (grayscale_miniswhite)
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ExportUInt32Quantum(endian,q,ScaleQuantumToLong(MaxRGB-PixelIntensityToQuantum(p)));
			ExportUInt32Quantum(endian,q,ScaleQuantumToLong(MaxRGB-GetOpacitySample(p)));
			p++;
		      }
		  }
		else
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ExportUInt32Quantum(endian,q,ScaleQuantumToLong(PixelIntensityToQuantum(p)));
			ExportUInt32Quantum(endian,q,ScaleQuantumToLong(MaxRGB-GetOpacitySample(p)));
			p++;
		      }
		  }
	      }
	    break;
	  }
	case 64:
	  {
	    if (image->is_grayscale)
	      {
		if (grayscale_miniswhite)
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ExportUInt64Quantum(endian,q,ScaleQuantumToLong(MaxRGB-GetRedSample(p)));
			ExportUInt64Quantum(endian,q,ScaleQuantumToLong(MaxRGB-GetOpacitySample(p)));
			p++;
		      }
		  }
		else
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ExportUInt64Quantum(endian,q,ScaleQuantumToLong(GetRedSample(p)));
			ExportUInt64Quantum(endian,q,ScaleQuantumToLong(MaxRGB-GetOpacitySample(p)));
			p++;
		      }
		  }
	      }
	    else
	      {
		if (grayscale_miniswhite)
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ExportUInt64Quantum(endian,q,ScaleQuantumToLong(MaxRGB-PixelIntensityToQuantum(p)));
			ExportUInt64Quantum(endian,q,ScaleQuantumToLong(MaxRGB-GetOpacitySample(p)));
			p++;
		      }
		  }
		else
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ExportUInt64Quantum(endian,q,ScaleQuantumToLong(PixelIntensityToQuantum(p)));
			ExportUInt64Quantum(endian,q,ScaleQuantumToLong(MaxRGB-GetOpacitySample(p)));
			p++;
		      }
		  }
	      }
	    break;
	  }
	default:
	  {
	    /*
	      Arbitrary sample size
	    */
	    BitStreamWriteHandle
	      stream;
                  
	    MagickBitStreamInitializeWrite(&stream,q);
	    if( QuantumDepth == sample_bits)
	      {
		/* Unity scale */
		for (x = number_pixels; x != 0; --x)
		  {
		    if (image->is_grayscale)
		      unsigned_value=GetRedSample(p);
		    else
		      unsigned_value=PixelIntensityToQuantum(p);
		    if (grayscale_miniswhite)
		      unsigned_value=MaxRGB-unsigned_value;
		    MagickBitStreamMSBWrite(&stream,quantum_size,unsigned_value);
		    MagickBitStreamMSBWrite(&stream,quantum_size,MaxRGB-GetOpacitySample(p));
		    p++;
		  }
	      }
	    else if (QuantumDepth >  sample_bits)
	      {
		/* Scale down */
		for (x = number_pixels; x != 0; --x)
		  {
		    if (image->is_grayscale)
		      unsigned_value=GetRedSample(p);
		    else
		      unsigned_value=PixelIntensityToQuantum(p);
		    if (grayscale_miniswhite)
		      unsigned_value=MaxRGB-unsigned_value;
		    unsigned_value /= unsigned_scale;
		    MagickBitStreamMSBWrite(&stream,quantum_size,unsigned_value);
		    MagickBitStreamMSBWrite(&stream,quantum_size,
					    (MaxRGB-GetOpacitySample(p))/unsigned_scale);
		    p++;
		  }
	      }
	    else
	      {
		/* Scale up */
		for (x = number_pixels; x != 0; --x)
		  {
		    if (image->is_grayscale)
		      unsigned_value=GetRedSample(p);
		    else
		      unsigned_value=PixelIntensityToQuantum(p);
		    if (grayscale_miniswhite)
		      unsigned_value=MaxRGB-unsigned_value;
		    unsigned_value *= unsigned_scale;
		    MagickBitStreamMSBWrite(&stream,quantum_size,unsigned_value);
		    MagickBitStreamMSBWrite(&stream,quantum_size,
					    (MaxRGB-GetOpacitySample(p))*unsigned_scale);
		    p++;
		  }
	      }
	    q=stream.bytes;
	    break;
	  }
	}
    }
  else if (sample_type == FloatQuantumSampleType)
    {
      switch (quantum_size)
	{
	case 16:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat16Quantum(endian,q,PixelIntensity(p)*double_scale+double_minvalue);
		ExportFloat16Quantum(endian,q,(((double) MaxRGB-GetOpacitySample(p))*double_scale+double_minvalue));
		p++;
	      }
	    break;
	  }
	case 24:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat24Quantum(endian,q,PixelIntensity(p)*double_scale+double_minvalue);
		ExportFloat24Quantum(endian,q,(((double) MaxRGB-GetOpacitySample(p))*double_scale+double_minvalue));
		p++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat32Quantum(endian,q,PixelIntensity(p)*double_scale+double_minvalue);
		ExportFloat32Quantum(endian,q,(((double) MaxRGB-GetOpacitySample(p))*double_scale+double_minvalue));
		p++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat64Quantum(endian,q,PixelIntensity(p)*double_scale+double_minvalue);
		ExportFloat64Quantum(endian,q,(MaxRGB-GetOpacitySample(p))*double_scale+double_minvalue);
		p++;
	      }
	    break;
	  }
	default:
	  break;
	}
    }

  /*
    Collect bytes exported
  */
  *bytes_exported=(q-destination);

  return MagickPass;
}

static MagickPassFail
ExportRedQuantumType(unsigned char * restrict destination,
		     const PixelPacket * restrict pixels,
		     const unsigned long number_pixels,
		     const unsigned int quantum_size,
		     const QuantumSampleType sample_type,
		     const EndianType endian,
		     const unsigned int unsigned_scale,
		     const unsigned int sample_bits,
		     const double double_minvalue,
		     const double double_scale,
		     unsigned long *bytes_exported)
{

  register unsigned char
    *q = destination;

  register const PixelPacket
    *p = pixels;

  register unsigned long
    x;

  if (sample_type == UnsignedQuantumSampleType)
    {
      /*
	Modulo-8 sample sizes
      */
      switch (quantum_size)
	{
	case 8:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt8Quantum(q,ScaleQuantumToChar(GetRedSample(p)));
		p++;
	      }
	    break;
	  }
	case 16:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt16Quantum(endian,q,ScaleQuantumToShort(GetRedSample(p)));
		p++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt32Quantum(endian,q,ScaleQuantumToLong(GetRedSample(p)));
		p++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt64Quantum(endian,q,ScaleQuantumToLong(GetRedSample(p)));
		p++;
	      }
	    break;
	  }
	default:
	  {
	    /*
	      Arbitrary sample size
	    */
	    BitStreamWriteHandle
	      stream;
                  
	    MagickBitStreamInitializeWrite(&stream,q);
	    if( QuantumDepth == sample_bits)
	      {
		/* Unity scale */
		for (x = number_pixels; x != 0; --x)
		  {
		    MagickBitStreamMSBWrite(&stream,quantum_size,
					    GetRedSample(p));
		    p++;
		  }
	      }
	    else if (QuantumDepth >  sample_bits)
	      {
		/* Scale down */
		for (x = number_pixels; x != 0; --x)
		  {
		    MagickBitStreamMSBWrite(&stream,quantum_size,
					    GetRedSample(p)/unsigned_scale);
		    p++;
		  }
	      }
	    else
	      {
		/* Scale up */
		for (x = number_pixels; x != 0; --x)
		  {
		    MagickBitStreamMSBWrite(&stream,quantum_size,
					    GetRedSample(p)*unsigned_scale);
		    p++;
		  }
	      }
	    q=stream.bytes;
	    break;
	  }
	}
    }
  else if (sample_type == FloatQuantumSampleType)
    {
      switch (quantum_size)
	{
	case 16:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat16Quantum(endian,q,GetRedSample(p)*double_scale+double_minvalue);
		p++;
	      }
	    break;
	  }
	case 24:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat24Quantum(endian,q,GetRedSample(p)*double_scale+double_minvalue);
		p++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat32Quantum(endian,q,GetRedSample(p)*double_scale+double_minvalue);
		p++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat64Quantum(endian,q,GetRedSample(p)*double_scale+double_minvalue);
		p++;
	      }
	    break;
	  }
	default:
	  break;
	}
    }

  /*
    Collect bytes exported
  */
  *bytes_exported=(q-destination);

  return MagickPass;
}

static MagickPassFail
ExportGreenQuantumType(unsigned char * restrict destination,
		       const PixelPacket * restrict pixels,
		       const unsigned long number_pixels,
		       const unsigned int quantum_size,
		       const QuantumSampleType sample_type,
		       const EndianType endian,
		       const unsigned int unsigned_scale,
		       const unsigned int sample_bits,
		       const double double_minvalue,
		       const double double_scale,
		       unsigned long *bytes_exported)
{
  register unsigned char
    *q = destination;

  register const PixelPacket
    *p = pixels;

  register unsigned long
    x;

  if (sample_type == UnsignedQuantumSampleType)
    {
      /*
	Modulo-8 sample sizes
      */
      switch (quantum_size)
	{
	case 8:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt8Quantum(q,ScaleQuantumToChar(GetGreenSample(p)));
		p++;
	      }
	    break;
	  }
	case 16:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt16Quantum(endian,q,ScaleQuantumToShort(GetGreenSample(p)));
		p++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt32Quantum(endian,q,ScaleQuantumToLong(GetGreenSample(p)));
		p++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt64Quantum(endian,q,ScaleQuantumToLong(GetGreenSample(p)));
		p++;
	      }
	    break;
	  }
	default:
	  {
	    /*
	      Arbitrary sample size
	    */
	    BitStreamWriteHandle
	      stream;
                  
	    MagickBitStreamInitializeWrite(&stream,q);
	    if( QuantumDepth == sample_bits)
	      {
		/* Unity scale */
		for (x = number_pixels; x != 0; --x)
		  {
		    MagickBitStreamMSBWrite(&stream,quantum_size,
					    GetGreenSample(p));
		    p++;
		  }
	      }
	    else if (QuantumDepth >  sample_bits)
	      {
		/* Scale down */
		for (x = number_pixels; x != 0; --x)
		  {
		    MagickBitStreamMSBWrite(&stream,quantum_size,
					    GetGreenSample(p)/unsigned_scale);
		    p++;
		  }
	      }
	    else
	      {
		/* Scale up */
		for (x = number_pixels; x != 0; --x)
		  {
		    MagickBitStreamMSBWrite(&stream,quantum_size,
					    GetGreenSample(p)*unsigned_scale);
		    p++;
		  }
	      }
	    q=stream.bytes;
	    break;
	  }
	}
    }
  else if (sample_type == FloatQuantumSampleType)
    {
      switch (quantum_size)
	{
	case 16:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat16Quantum(endian,q,GetGreenSample(p)*double_scale+double_minvalue);
		p++;
	      }
	    break;
	  }
	case 24:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat24Quantum(endian,q,GetGreenSample(p)*double_scale+double_minvalue);
		p++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat32Quantum(endian,q,GetGreenSample(p)*double_scale+double_minvalue);
		p++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat64Quantum(endian,q,GetGreenSample(p)*double_scale+double_minvalue);
		p++;
	      }
	    break;
	  }
	default:
	  break;
	}
    }

  /*
    Collect bytes exported
  */
  *bytes_exported=(q-destination);

  return MagickPass;
}

static MagickPassFail
ExportBlueQuantumType(unsigned char * restrict destination,
		      const PixelPacket * restrict pixels,
		      const unsigned long number_pixels,
		      const unsigned int quantum_size,
		      const QuantumSampleType sample_type,
		      const EndianType endian,
		      const unsigned int unsigned_scale,
		      const unsigned int sample_bits,
		      const double double_minvalue,
		      const double double_scale,
		      unsigned long *bytes_exported)
{
  register unsigned char
    *q = destination;

  register const PixelPacket
    *p = pixels;

  register unsigned long
    x;

  if (sample_type == UnsignedQuantumSampleType)
    {
      /*
	Modulo-8 sample sizes
      */
      switch (quantum_size)
	{
	case 8:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt8Quantum(q,ScaleQuantumToChar(GetBlueSample(p)));
		p++;
	      }
	    break;
	  }
	case 16:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt16Quantum(endian,q,ScaleQuantumToShort(GetBlueSample(p)));
		p++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt32Quantum(endian,q,ScaleQuantumToLong(GetBlueSample(p)));
		p++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt64Quantum(endian,q,ScaleQuantumToLong(GetBlueSample(p)));
		p++;
	      }
	    break;
	  }
	default:
	  {
	    /*
	      Arbitrary sample size
	    */
	    BitStreamWriteHandle
	      stream;
                  
	    MagickBitStreamInitializeWrite(&stream,q);
	    if( QuantumDepth == sample_bits)
	      {
		/* Unity scale */
		for (x = number_pixels; x != 0; --x)
		  {
		    MagickBitStreamMSBWrite(&stream,quantum_size,
					    GetBlueSample(p));
		    p++;
		  }
	      }
	    else if (QuantumDepth >  sample_bits)
	      {
		/* Scale down */
		for (x = number_pixels; x != 0; --x)
		  {
		    MagickBitStreamMSBWrite(&stream,quantum_size,
					    GetBlueSample(p)/unsigned_scale);
		    p++;
		  }
	      }
	    else
	      {
		/* Scale up */
		for (x = number_pixels; x != 0; --x)
		  {
		    MagickBitStreamMSBWrite(&stream,quantum_size,
					    GetBlueSample(p)*unsigned_scale);
		    p++;
		  }
	      }
	    q=stream.bytes;
	    break;
	  }
	}
    }
  else if (sample_type == FloatQuantumSampleType)
    {
      switch (quantum_size)
	{
	case 16:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat16Quantum(endian,q,GetBlueSample(p)*double_scale+double_minvalue);
		p++;
	      }
	    break;
	  }
	case 24:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat24Quantum(endian,q,GetBlueSample(p)*double_scale+double_minvalue);
		p++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat32Quantum(endian,q,GetBlueSample(p)*double_scale+double_minvalue);
		p++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat64Quantum(endian,q,GetBlueSample(p)*double_scale+double_minvalue);
		p++;
	      }
	    break;
	  }
	default:
	  break;
	}
    }

  /*
    Collect bytes exported
  */
  *bytes_exported=(q-destination);

  return MagickPass;
}

static MagickPassFail
ExportAlphaQuantumType(unsigned char * restrict destination,
		       const PixelPacket * restrict pixels,
		       const IndexPacket * restrict indexes,
		       const unsigned long number_pixels,
		       const unsigned int quantum_size,
		       const QuantumSampleType sample_type,
		       const EndianType endian,
		       const unsigned int unsigned_scale,
		       const unsigned int sample_bits,
		       const double double_minvalue,
		       const double double_scale,
		       const Image *image,
		       unsigned long *bytes_exported)
{
  register unsigned char
    *q = destination;

  register const PixelPacket
    *p = pixels;

  register unsigned long
    x;

  if (image->colorspace == CMYKColorspace)
    {
      if (sample_type == UnsignedQuantumSampleType)
	{
	  switch (quantum_size)
	    {
	    case 8:
	      {
		for (x = number_pixels; x != 0; --x)
		  {
		    ExportUInt8Quantum(q,ScaleQuantumToChar(MaxRGB-*indexes));
		    indexes++;
		  }
		break;
	      }
	    case 16:
	      {
		for (x = number_pixels; x != 0; --x)
		  {
		    ExportUInt16Quantum(endian,q,ScaleQuantumToShort(MaxRGB-*indexes));
		    indexes++;
		  }
		break;
	      }
	    case 32:
	      {
		for (x = number_pixels; x != 0; --x)
		  {
		    ExportUInt32Quantum(endian,q,ScaleQuantumToLong(MaxRGB-*indexes));
		    indexes++;
		  }
		break;
	      }
	    case 64:
	      {
		for (x = number_pixels; x != 0; --x)
		  {
		    ExportUInt64Quantum(endian,q,ScaleQuantumToLong(MaxRGB-*indexes));
		    indexes++;
		  }
		break;
	      }
	    default:
	      {
		/*
		  Arbitrary sample size
		*/
		BitStreamWriteHandle
		  stream;
                      
		MagickBitStreamInitializeWrite(&stream,q);
		if( QuantumDepth == sample_bits)
		  {
		    /* Unity scale */
		    for (x = number_pixels; x != 0; --x)
		      {
			MagickBitStreamMSBWrite(&stream,quantum_size,
						MaxRGB-*indexes);
			indexes++;
		      }
		  }
		else if (QuantumDepth >  sample_bits)
		  {
		    /* Scale down */
		    for (x = number_pixels; x != 0; --x)
		      {
			MagickBitStreamMSBWrite(&stream,quantum_size,
						(MaxRGB-*indexes)/unsigned_scale);
			indexes++;
		      }
		  }
		else
		  {
		    /* Scale up */
		    for (x = number_pixels; x != 0; --x)
		      {
			MagickBitStreamMSBWrite(&stream,quantum_size,
						(MaxRGB-*indexes)*unsigned_scale);
			indexes++;
		      }
		  }
		q=stream.bytes;
		break;
	      }
	    }
	}
      else if (sample_type == FloatQuantumSampleType)
	{
	  switch (quantum_size)
	    {
	    case 16:
	      {
		for (x = number_pixels; x != 0; --x)
		  {
		    ExportFloat16Quantum(endian,q,(MaxRGB-*indexes)*double_scale+double_minvalue);
		    p++;
		  }
		break;
	      }
	    case 24:
	      {
		for (x = number_pixels; x != 0; --x)
		  {
		    ExportFloat24Quantum(endian,q,(MaxRGB-*indexes)*double_scale+double_minvalue);
		    p++;
		  }
		break;
	      }
	    case 32:
	      {
		for (x = number_pixels; x != 0; --x)
		  {
		    ExportFloat32Quantum(endian,q,(MaxRGB-*indexes)*double_scale+double_minvalue);
		    p++;
		  }
		break;
	      }
	    case 64:
	      {
		for (x = number_pixels; x != 0; --x)
		  {
		    ExportFloat64Quantum(endian,q,(MaxRGB-*indexes)*double_scale+double_minvalue);
		    p++;
		  }
		break;
	      }
	    default:
	      break;
	    }
	}
      
    }
  else
    { 
      if (sample_type == UnsignedQuantumSampleType)
	{
	  switch (quantum_size)
	    {
	    case 8:
	      {
		for (x = number_pixels; x != 0; --x)
		  {
		    ExportUInt16Quantum(endian,q,ScaleQuantumToChar(MaxRGB-GetOpacitySample(p)));
		    p++;
		  }
		break;
	      }
	    case 16:
	      {
		for (x = number_pixels; x != 0; --x)
		  {
		    ExportUInt16Quantum(endian,q,ScaleQuantumToShort(MaxRGB-GetOpacitySample(p)));
		    p++;
		  }
		break;
	      }
	    case 32:
	      {
		for (x = number_pixels; x != 0; --x)
		  {
		    ExportUInt32Quantum(endian,q,ScaleQuantumToLong(MaxRGB-GetOpacitySample(p)));
		    p++;
		  }
		break;
	      }
	    case 64:
	      {
		for (x = number_pixels; x != 0; --x)
		  {
		    ExportUInt64Quantum(endian,q,ScaleQuantumToLong(MaxRGB-GetOpacitySample(p)));
		    p++;
		  }
		break;
	      }
	    default:
	      {
		/*
		  Arbitrary sample size
		*/
		BitStreamWriteHandle
		  stream;
                  
		MagickBitStreamInitializeWrite(&stream,q);
		if( QuantumDepth == sample_bits)
		  {
		    /* Unity scale */
		    for (x = number_pixels; x != 0; --x)
		      {
			MagickBitStreamMSBWrite(&stream,quantum_size,
						MaxRGB-GetOpacitySample(p));
			p++;
		      }
		  }
		else if (QuantumDepth >  sample_bits)
		  {
		    /* Scale down */
		    for (x = number_pixels; x != 0; --x)
		      {
			MagickBitStreamMSBWrite(&stream,quantum_size,
						(MaxRGB-GetOpacitySample(p))/unsigned_scale);
			p++;
		      }
		  }
		else
		  {
		    /* Scale up */
		    for (x = number_pixels; x != 0; --x)
		      {
			MagickBitStreamMSBWrite(&stream,quantum_size,
						(MaxRGB-GetOpacitySample(p))*unsigned_scale);
			p++;
		      }
		  }
		q=stream.bytes;
		break;
	      }
	    }
	}
      else if (sample_type == FloatQuantumSampleType)
	{
	  switch (quantum_size)
	    {
	    case 16:
	      {
		for (x = number_pixels; x != 0; --x)
		  {
		    ExportFloat16Quantum(endian,q,(MaxRGB-GetOpacitySample(p))*double_scale+double_minvalue);
		    p++;
		  }
		break;
	      }
	    case 24:
	      {
		for (x = number_pixels; x != 0; --x)
		  {
		    ExportFloat24Quantum(endian,q,(MaxRGB-GetOpacitySample(p))*double_scale+double_minvalue);
		    p++;
		  }
		break;
	      }
	    case 32:
	      {
		for (x = number_pixels; x != 0; --x)
		  {
		    ExportFloat32Quantum(endian,q,(MaxRGB-GetOpacitySample(p))*double_scale+double_minvalue);
		    p++;
		  }
		break;
	      }
	    case 64:
	      {
		for (x = number_pixels; x != 0; --x)
		  {
		    ExportFloat64Quantum(endian,q,(MaxRGB-GetOpacitySample(p))*double_scale+double_minvalue);
		    p++;
		  }
		break;
	      }
	    default:
	      break;
	    }
	}
    }

  /*
    Collect bytes exported
  */
  *bytes_exported=(q-destination);

  return MagickPass;
}

static MagickPassFail
ExportBlackQuantumType(unsigned char * restrict destination,
		       const PixelPacket * restrict pixels,
		       const unsigned long number_pixels,
		       const unsigned int quantum_size,
		       const QuantumSampleType sample_type,
		       const EndianType endian,
		       const unsigned int unsigned_scale,
		       const unsigned int sample_bits,
		       const double double_minvalue,
		       const double double_scale,
		       unsigned long *bytes_exported)
{
  register unsigned char
    *q = destination;

  register const PixelPacket
    *p = pixels;

  register unsigned long
    x;

  if (sample_type == UnsignedQuantumSampleType)
    {
      switch (quantum_size)
	{
	case 8:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt8Quantum(q,ScaleQuantumToChar(GetBlackSample(p)));
		p++;
	      }
	    break;
	  }
	case 16:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt16Quantum(endian,q,ScaleQuantumToShort(GetBlackSample(p)));
		p++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt32Quantum(endian,q,ScaleQuantumToLong(GetBlackSample(p)));
		p++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt64Quantum(endian,q,ScaleQuantumToLong(GetBlackSample(p)));
		p++;
	      }
	    break;
	  }
	default:
	  {
	    /*
	      Arbitrary sample size
	    */
	    BitStreamWriteHandle
	      stream;
                  
	    MagickBitStreamInitializeWrite(&stream,q);
	    if( QuantumDepth == sample_bits)
	      {
		/* Unity scale */
		for (x = number_pixels; x != 0; --x)
		  {
		    MagickBitStreamMSBWrite(&stream,quantum_size,
					    GetBlackSample(p));
		    p++;
		  }
	      }
	    else if (QuantumDepth >  sample_bits)
	      {
		/* Scale down */
		for (x = number_pixels; x != 0; --x)
		  {
		    MagickBitStreamMSBWrite(&stream,quantum_size,
					    GetBlackSample(p)/unsigned_scale);
		    p++;
		  }
	      }
	    else
	      {
		/* Scale up */
		for (x = number_pixels; x != 0; --x)
		  {
		    MagickBitStreamMSBWrite(&stream,quantum_size,
					    GetBlackSample(p)*unsigned_scale);
		    p++;
		  }
	      }
	    q=stream.bytes;
	    break;
	  }
	}
    }
  else if (sample_type == FloatQuantumSampleType)
    {
      switch (quantum_size)
	{
	case 16:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat16Quantum(endian,q,GetBlackSample(p)*double_scale+double_minvalue);
		p++;
	      }
	    break;
	  }
	case 24:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat24Quantum(endian,q,GetBlackSample(p)*double_scale+double_minvalue);
		p++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat32Quantum(endian,q,GetBlackSample(p)*double_scale+double_minvalue);
		p++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat64Quantum(endian,q,GetBlackSample(p)*double_scale+double_minvalue);
		p++;
	      }
	    break;
	  }
	default:
	  break;
	}
    }

  /*
    Collect bytes exported
  */
  *bytes_exported=(q-destination);

  return MagickPass;
}

static MagickPassFail
ExportRGBQuantumType(unsigned char * restrict destination,
		     const PixelPacket * restrict pixels,
		     const unsigned long number_pixels,
		     const unsigned int quantum_size,
		     const QuantumSampleType sample_type,
		     const EndianType endian,
		     const unsigned int unsigned_scale,
		     const unsigned int sample_bits,
		     const double double_minvalue,
		     const double double_scale,
		     unsigned long *bytes_exported)
{
  register unsigned char
    *q = destination;

  register const PixelPacket
    *p = pixels;

  register unsigned long
    x;

  if (sample_type == UnsignedQuantumSampleType)
    {
      switch (quantum_size)
	{
	case 8:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt8Quantum(q,ScaleQuantumToChar(GetRedSample(p)));
		ExportUInt8Quantum(q,ScaleQuantumToChar(GetGreenSample(p)));
		ExportUInt8Quantum(q,ScaleQuantumToChar(GetBlueSample(p)));
		p++;
	      }
	    break;
	  }
	case 16:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt16Quantum(endian,q,ScaleQuantumToShort(GetRedSample(p)));
		ExportUInt16Quantum(endian,q,ScaleQuantumToShort(GetGreenSample(p)));
		ExportUInt16Quantum(endian,q,ScaleQuantumToShort(GetBlueSample(p)));
		p++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt32Quantum(endian,q,ScaleQuantumToLong(GetRedSample(p)));
		ExportUInt32Quantum(endian,q,ScaleQuantumToLong(GetGreenSample(p)));
		ExportUInt32Quantum(endian,q,ScaleQuantumToLong(GetBlueSample(p)));
		p++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt64Quantum(endian,q,ScaleQuantumToLong(GetRedSample(p)));
		ExportUInt64Quantum(endian,q,ScaleQuantumToLong(GetGreenSample(p)));
		ExportUInt64Quantum(endian,q,ScaleQuantumToLong(GetBlueSample(p)));
		p++;
	      }
	    break;
	  }
	default:
	  {
	    /*
	      Arbitrary sample size
	    */
	    BitStreamWriteHandle
	      stream;
                  
	    MagickBitStreamInitializeWrite(&stream,q);
	    if( QuantumDepth == sample_bits)
	      {
		/* Unity scale */
		for (x = number_pixels; x != 0; --x)
		  {
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetRedSample(p));
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetGreenSample(p));
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetBlueSample(p));
		    p++;
		  }
	      }
	    else if (QuantumDepth >  sample_bits)
	      {
		/* Scale down */
		for (x = number_pixels; x != 0; --x)
		  {
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetRedSample(p)/unsigned_scale);
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetGreenSample(p)/unsigned_scale);
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetBlueSample(p)/unsigned_scale);
		    p++;
		  }
	      }
	    else
	      {
		/* Scale up */
		for (x = number_pixels; x != 0; --x)
		  {
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetRedSample(p)*unsigned_scale);
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetGreenSample(p)*unsigned_scale);
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetBlueSample(p)*unsigned_scale);
		    p++;
		  }
	      }
	    q=stream.bytes;
	    break;
	  }
	}
    }
  else if (sample_type == FloatQuantumSampleType)
    {
      switch (quantum_size)
	{
	case 16:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat16Quantum(endian,q,GetRedSample(p)*double_scale+double_minvalue);
		ExportFloat16Quantum(endian,q,GetGreenSample(p)*double_scale+double_minvalue);
		ExportFloat16Quantum(endian,q,GetBlueSample(p)*double_scale+double_minvalue);
		p++;
	      }
	    break;
	  }
	case 24:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat24Quantum(endian,q,GetRedSample(p)*double_scale+double_minvalue);
		ExportFloat24Quantum(endian,q,GetGreenSample(p)*double_scale+double_minvalue);
		ExportFloat24Quantum(endian,q,GetBlueSample(p)*double_scale+double_minvalue);
		p++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat32Quantum(endian,q,GetRedSample(p)*double_scale+double_minvalue);
		ExportFloat32Quantum(endian,q,GetGreenSample(p)*double_scale+double_minvalue);
		ExportFloat32Quantum(endian,q,GetBlueSample(p)*double_scale+double_minvalue);
		p++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat64Quantum(endian,q,GetRedSample(p)*double_scale+double_minvalue);
		ExportFloat64Quantum(endian,q,GetGreenSample(p)*double_scale+double_minvalue);
		ExportFloat64Quantum(endian,q,GetBlueSample(p)*double_scale+double_minvalue);
		p++;
	      }
	    break;
	  }
	default:
	  break;
	}
    }

  /*
    Collect bytes exported
  */
  *bytes_exported=(q-destination);

  return MagickPass;
}

static MagickPassFail
ExportRGBAQuantumType(unsigned char * restrict destination,
		      const PixelPacket * restrict pixels,
		      const unsigned long number_pixels,
		      const unsigned int quantum_size,
		      const QuantumSampleType sample_type,
		      const EndianType endian,
		      const unsigned int unsigned_scale,
		      const unsigned int sample_bits,
		      const double double_minvalue,
		      const double double_scale,
		      unsigned long *bytes_exported)
{
  register unsigned char
    *q = destination;

  register const PixelPacket
    *p = pixels;

  register unsigned long
    x;

  if (sample_type == UnsignedQuantumSampleType)
    {
      switch (quantum_size)
	{
	case 8:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt8Quantum(q,ScaleQuantumToChar(GetRedSample(p)));
		ExportUInt8Quantum(q,ScaleQuantumToChar(GetGreenSample(p)));
		ExportUInt8Quantum(q,ScaleQuantumToChar(GetBlueSample(p)));
		ExportUInt8Quantum(q,ScaleQuantumToChar(MaxRGB-GetOpacitySample(p)));
		p++;
	      }
	    break;
	  }
	case 16:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt16Quantum(endian,q,ScaleQuantumToShort(GetRedSample(p)));
		ExportUInt16Quantum(endian,q,ScaleQuantumToShort(GetGreenSample(p)));
		ExportUInt16Quantum(endian,q,ScaleQuantumToShort(GetBlueSample(p)));
		ExportUInt16Quantum(endian,q,ScaleQuantumToShort(MaxRGB-GetOpacitySample(p)));
		p++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt32Quantum(endian,q,ScaleQuantumToLong(GetRedSample(p)));
		ExportUInt32Quantum(endian,q,ScaleQuantumToLong(GetGreenSample(p)));
		ExportUInt32Quantum(endian,q,ScaleQuantumToLong(GetBlueSample(p)));
		ExportUInt32Quantum(endian,q,ScaleQuantumToLong(MaxRGB-GetOpacitySample(p)));
		p++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt64Quantum(endian,q,ScaleQuantumToLong(GetRedSample(p)));
		ExportUInt64Quantum(endian,q,ScaleQuantumToLong(GetGreenSample(p)));
		ExportUInt64Quantum(endian,q,ScaleQuantumToLong(GetBlueSample(p)));
		ExportUInt64Quantum(endian,q,ScaleQuantumToLong(MaxRGB-GetOpacitySample(p)));
		p++;
	      }
	    break;
	  }
	default:
	  {
	    /*
	      Arbitrary sample size
	    */
	    BitStreamWriteHandle
	      stream;
                  
	    MagickBitStreamInitializeWrite(&stream,q);
	    if(QuantumDepth == sample_bits)
	      {
		/* Unity scale */
		for (x = number_pixels; x != 0; --x)
		  {
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetRedSample(p));
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetGreenSample(p));
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetBlueSample(p));
		    MagickBitStreamMSBWrite(&stream,quantum_size,MaxRGB-GetOpacitySample(p));
		    p++;
		  }
	      }
	    else if (QuantumDepth >  sample_bits)
	      {
		/* Scale down */
		for (x = number_pixels; x != 0; --x)
		  {
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetRedSample(p)/unsigned_scale);
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetGreenSample(p)/unsigned_scale);
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetBlueSample(p)/unsigned_scale);
		    MagickBitStreamMSBWrite(&stream,quantum_size,
					    (MaxRGB-GetOpacitySample(p))/unsigned_scale);
		    p++;
		  }
	      }
	    else
	      {
		/* Scale up */
		for (x = number_pixels; x != 0; --x)
		  {
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetRedSample(p)*unsigned_scale);
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetGreenSample(p)*unsigned_scale);
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetBlueSample(p)*unsigned_scale);
		    MagickBitStreamMSBWrite(&stream,quantum_size,
					    (MaxRGB-GetOpacitySample(p))*unsigned_scale);
		    p++;
                          
		  }
	      }
	    q=stream.bytes;
	    break;
	  }
	}
    }
  else if (sample_type == FloatQuantumSampleType)
    {
      switch (quantum_size)
	{
	case 16:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat16Quantum(endian,q,GetRedSample(p)*double_scale+double_minvalue);
		ExportFloat16Quantum(endian,q,GetGreenSample(p)*double_scale+double_minvalue);
		ExportFloat16Quantum(endian,q,GetBlueSample(p)*double_scale+double_minvalue);
		ExportFloat16Quantum(endian,q,(MaxRGB-GetOpacitySample(p))*double_scale+double_minvalue);
		p++;
	      }
	    break;
	  }
	case 24:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat24Quantum(endian,q,GetRedSample(p)*double_scale+double_minvalue);
		ExportFloat24Quantum(endian,q,GetGreenSample(p)*double_scale+double_minvalue);
		ExportFloat24Quantum(endian,q,GetBlueSample(p)*double_scale+double_minvalue);
		ExportFloat24Quantum(endian,q,(MaxRGB-GetOpacitySample(p))*double_scale+double_minvalue);
		p++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat32Quantum(endian,q,GetRedSample(p)*double_scale+double_minvalue);
		ExportFloat32Quantum(endian,q,GetGreenSample(p)*double_scale+double_minvalue);
		ExportFloat32Quantum(endian,q,GetBlueSample(p)*double_scale+double_minvalue);
		ExportFloat32Quantum(endian,q,(MaxRGB-GetOpacitySample(p))*double_scale+double_minvalue);
		p++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat64Quantum(endian,q,GetRedSample(p)*double_scale+double_minvalue);
		ExportFloat64Quantum(endian,q,GetGreenSample(p)*double_scale+double_minvalue);
		ExportFloat64Quantum(endian,q,GetBlueSample(p)*double_scale+double_minvalue);
		ExportFloat64Quantum(endian,q,(MaxRGB-GetOpacitySample(p))*double_scale+double_minvalue);
		p++;
	      }
	    break;
	  }
	default:
	  break;
	}
    }

  /*
    Collect bytes exported
  */
  *bytes_exported=(q-destination);

  return MagickPass;
}

static MagickPassFail
ExportCMYKQuantumType(unsigned char * restrict destination,
		      const PixelPacket * restrict pixels,
		      const unsigned long number_pixels,
		      const unsigned int quantum_size,
		      const QuantumSampleType sample_type,
		      const EndianType endian,
		      const unsigned int unsigned_scale,
		      const unsigned int sample_bits,
		      const double double_minvalue,
		      const double double_scale,
		      unsigned long *bytes_exported)
{
  register unsigned char
    *q = destination;

  register const PixelPacket
    *p = pixels;

  register unsigned long
    x;

  if (sample_type == UnsignedQuantumSampleType)
    {
      switch (quantum_size)
	{
	case 8:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt8Quantum(q,ScaleQuantumToChar(GetCyanSample(p)));
		ExportUInt8Quantum(q,ScaleQuantumToChar(GetMagentaSample(p)));
		ExportUInt8Quantum(q,ScaleQuantumToChar(GetYellowSample(p)));
		ExportUInt8Quantum(q,ScaleQuantumToChar(GetBlackSample(p)));
		p++;
	      }
	    break;
	  }
	case 16:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt16Quantum(endian,q,ScaleQuantumToShort(GetCyanSample(p)));
		ExportUInt16Quantum(endian,q,ScaleQuantumToShort(GetMagentaSample(p)));
		ExportUInt16Quantum(endian,q,ScaleQuantumToShort(GetYellowSample(p)));
		ExportUInt16Quantum(endian,q,ScaleQuantumToShort(GetBlackSample(p)));
		p++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt32Quantum(endian,q,ScaleQuantumToLong(GetCyanSample(p)));
		ExportUInt32Quantum(endian,q,ScaleQuantumToLong(GetMagentaSample(p)));
		ExportUInt32Quantum(endian,q,ScaleQuantumToLong(GetYellowSample(p)));
		ExportUInt32Quantum(endian,q,ScaleQuantumToLong(GetBlackSample(p)));
		p++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt64Quantum(endian,q,ScaleQuantumToLong(GetCyanSample(p)));
		ExportUInt64Quantum(endian,q,ScaleQuantumToLong(GetMagentaSample(p)));
		ExportUInt64Quantum(endian,q,ScaleQuantumToLong(GetYellowSample(p)));
		ExportUInt64Quantum(endian,q,ScaleQuantumToLong(GetBlackSample(p)));
		p++;
	      }
	    break;
	  }
	default:
	  {
	    /*
	      Arbitrary sample size
	    */
	    BitStreamWriteHandle
	      stream;
                  
	    MagickBitStreamInitializeWrite(&stream,q);
	    if(QuantumDepth == sample_bits)
	      {
		/* Unity scale */
		for (x = number_pixels; x != 0; --x)
		  {
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetCyanSample(p));
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetMagentaSample(p));
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetYellowSample(p));
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetBlackSample(p));
		    p++;
		  }
	      }
	    else if (QuantumDepth >  sample_bits)
	      {
		/* Scale down */
		for (x = number_pixels; x != 0; --x)
		  {
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetCyanSample(p)/unsigned_scale);
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetMagentaSample(p)/unsigned_scale);
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetYellowSample(p)/unsigned_scale);
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetBlackSample(p)/unsigned_scale);
		    p++;
		  }
	      }
	    else
	      {
		/* Scale up */
		for (x = number_pixels; x != 0; --x)
		  {
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetCyanSample(p)*unsigned_scale);
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetMagentaSample(p)*unsigned_scale);
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetYellowSample(p)*unsigned_scale);
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetBlackSample(p)*unsigned_scale);
		    p++;
		  }
	      }
	    q=stream.bytes;
	    break;
	  }
	}
    }
  else if (sample_type == FloatQuantumSampleType)
    {
      switch (quantum_size)
	{
	case 16:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat16Quantum(endian,q,GetCyanSample(p)*double_scale+double_minvalue);
		ExportFloat16Quantum(endian,q,GetMagentaSample(p)*double_scale+double_minvalue);
		ExportFloat16Quantum(endian,q,GetYellowSample(p)*double_scale+double_minvalue);
		ExportFloat16Quantum(endian,q,GetBlackSample(p)*double_scale+double_minvalue);
		p++;
	      }
	    break;
	  }
	case 24:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat24Quantum(endian,q,GetCyanSample(p)*double_scale+double_minvalue);
		ExportFloat24Quantum(endian,q,GetMagentaSample(p)*double_scale+double_minvalue);
		ExportFloat24Quantum(endian,q,GetYellowSample(p)*double_scale+double_minvalue);
		ExportFloat24Quantum(endian,q,GetBlackSample(p)*double_scale+double_minvalue);
		p++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat32Quantum(endian,q,GetCyanSample(p)*double_scale+double_minvalue);
		ExportFloat32Quantum(endian,q,GetMagentaSample(p)*double_scale+double_minvalue);
		ExportFloat32Quantum(endian,q,GetYellowSample(p)*double_scale+double_minvalue);
		ExportFloat32Quantum(endian,q,GetBlackSample(p)*double_scale+double_minvalue);
		p++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat64Quantum(endian,q,GetCyanSample(p)*double_scale+double_minvalue);
		ExportFloat64Quantum(endian,q,GetMagentaSample(p)*double_scale+double_minvalue);
		ExportFloat64Quantum(endian,q,GetYellowSample(p)*double_scale+double_minvalue);
		ExportFloat64Quantum(endian,q,GetBlackSample(p)*double_scale+double_minvalue);
		p++;
	      }
	    break;
	  }
	default:
	  break;
	}
    }

  /*
    Collect bytes exported
  */
  *bytes_exported=(q-destination);

  return MagickPass;
}

static MagickPassFail
ExportCMYKAQuantumType(unsigned char * restrict destination,
		       const PixelPacket * restrict pixels,
		       const IndexPacket * restrict indexes,
		       const unsigned long number_pixels,
		       const unsigned int quantum_size,
		       const QuantumSampleType sample_type,
		       const EndianType endian,
		       const unsigned int unsigned_scale,
		       const unsigned int sample_bits,
		       const double double_minvalue,
		       const double double_scale,
		       unsigned long *bytes_exported)
{
  register unsigned char
    *q = destination;

  register const PixelPacket
    *p = pixels;

  register unsigned long
    x;

  if (sample_type == UnsignedQuantumSampleType)
    {
      switch (quantum_size)
	{
	case 8:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt8Quantum(q,ScaleQuantumToChar(GetCyanSample(p)));
		ExportUInt8Quantum(q,ScaleQuantumToChar(GetMagentaSample(p)));
		ExportUInt8Quantum(q,ScaleQuantumToChar(GetYellowSample(p)));
		ExportUInt8Quantum(q,ScaleQuantumToChar(GetBlackSample(p)));
		ExportUInt8Quantum(q,ScaleQuantumToChar(MaxRGB-*indexes));
		indexes++;
		p++;
	      }
	    break;
	  }
	case 16:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt16Quantum(endian,q,ScaleQuantumToShort(GetCyanSample(p)));
		ExportUInt16Quantum(endian,q,ScaleQuantumToShort(GetMagentaSample(p)));
		ExportUInt16Quantum(endian,q,ScaleQuantumToShort(GetYellowSample(p)));
		ExportUInt16Quantum(endian,q,ScaleQuantumToShort(GetBlackSample(p)));
		ExportUInt16Quantum(endian,q,ScaleQuantumToShort(MaxRGB-*indexes));
		indexes++;
		p++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt32Quantum(endian,q,ScaleQuantumToLong(GetCyanSample(p)));
		ExportUInt32Quantum(endian,q,ScaleQuantumToLong(GetMagentaSample(p)));
		ExportUInt32Quantum(endian,q,ScaleQuantumToLong(GetYellowSample(p)));
		ExportUInt32Quantum(endian,q,ScaleQuantumToLong(GetBlackSample(p)));
		ExportUInt32Quantum(endian,q,ScaleQuantumToLong(MaxRGB-*indexes));
		indexes++;
		p++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportUInt64Quantum(endian,q,ScaleQuantumToLong(GetCyanSample(p)));
		ExportUInt64Quantum(endian,q,ScaleQuantumToLong(GetMagentaSample(p)));
		ExportUInt64Quantum(endian,q,ScaleQuantumToLong(GetYellowSample(p)));
		ExportUInt64Quantum(endian,q,ScaleQuantumToLong(GetBlackSample(p)));
		ExportUInt64Quantum(endian,q,ScaleQuantumToLong(MaxRGB-*indexes));
		indexes++;
		p++;
	      }
	    break;
	  }
	default:
	  {
	    /*
	      Arbitrary sample size
	    */
	    BitStreamWriteHandle
	      stream;
                  
	    MagickBitStreamInitializeWrite(&stream,q);
	    if( QuantumDepth == sample_bits)
	      {
		/* Unity scale */
		for (x = number_pixels; x != 0; --x)
		  {
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetCyanSample(p));
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetMagentaSample(p));
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetYellowSample(p));
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetBlackSample(p));
		    MagickBitStreamMSBWrite(&stream,quantum_size,MaxRGB-*indexes);
		    indexes++;
		    p++;
		  }
	      }
	    else if (QuantumDepth >  sample_bits)
	      {
		/* Scale down */
		for (x = number_pixels; x != 0; --x)
		  {
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetCyanSample(p)/unsigned_scale);
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetMagentaSample(p)/unsigned_scale);
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetYellowSample(p)/unsigned_scale);
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetBlackSample(p)/unsigned_scale);
		    MagickBitStreamMSBWrite(&stream,quantum_size,
					    (MaxRGB-*indexes)/unsigned_scale);
		    indexes++;
		    p++;
		  }
	      }
	    else
	      {
		/* Scale up */
		for (x = number_pixels; x != 0; --x)
		  {
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetCyanSample(p)*unsigned_scale);
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetMagentaSample(p)*unsigned_scale);
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetYellowSample(p)*unsigned_scale);
		    MagickBitStreamMSBWrite(&stream,quantum_size,GetBlackSample(p)*unsigned_scale);
		    MagickBitStreamMSBWrite(&stream,quantum_size,
					    (MaxRGB-*indexes)*unsigned_scale);
		    indexes++;
		    p++;
		  }
	      }
	    q=stream.bytes;
	    break;
	  }
	}
    }
  else if (sample_type == FloatQuantumSampleType)
    {
      switch (quantum_size)
	{
	case 16:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat16Quantum(endian,q,GetCyanSample(p)*double_scale+double_minvalue);
		ExportFloat16Quantum(endian,q,GetMagentaSample(p)*double_scale+double_minvalue);
		ExportFloat16Quantum(endian,q,GetYellowSample(p)*double_scale+double_minvalue);
		ExportFloat16Quantum(endian,q,GetBlackSample(p)*double_scale+double_minvalue);
		ExportFloat16Quantum(endian,q,(MaxRGB-*indexes)*double_scale+double_minvalue);
		p++;
	      }
	    break;
	  }
	case 24:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat24Quantum(endian,q,GetCyanSample(p)*double_scale+double_minvalue);
		ExportFloat24Quantum(endian,q,GetMagentaSample(p)*double_scale+double_minvalue);
		ExportFloat24Quantum(endian,q,GetYellowSample(p)*double_scale+double_minvalue);
		ExportFloat24Quantum(endian,q,GetBlackSample(p)*double_scale+double_minvalue);
		ExportFloat24Quantum(endian,q,(MaxRGB-*indexes)*double_scale+double_minvalue);
		p++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat32Quantum(endian,q,GetCyanSample(p)*double_scale+double_minvalue);
		ExportFloat32Quantum(endian,q,GetMagentaSample(p)*double_scale+double_minvalue);
		ExportFloat32Quantum(endian,q,GetYellowSample(p)*double_scale+double_minvalue);
		ExportFloat32Quantum(endian,q,GetBlackSample(p)*double_scale+double_minvalue);
		ExportFloat32Quantum(endian,q,(MaxRGB-*indexes)*double_scale+double_minvalue);
		p++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ExportFloat64Quantum(endian,q,GetCyanSample(p)*double_scale+double_minvalue);
		ExportFloat64Quantum(endian,q,GetMagentaSample(p)*double_scale+double_minvalue);
		ExportFloat64Quantum(endian,q,GetYellowSample(p)*double_scale+double_minvalue);
		ExportFloat64Quantum(endian,q,GetBlackSample(p)*double_scale+double_minvalue);
		ExportFloat64Quantum(endian,q,(MaxRGB-*indexes)*double_scale+double_minvalue);
		p++;
	      }
	    break;
	  }
	default:
	  break;
	}
    }

  /*
    Collect bytes exported
  */
  *bytes_exported=(q-destination);

  return MagickPass;
}

MagickExport MagickPassFail
ExportViewPixelArea(const ViewInfo *view,
		    const QuantumType quantum_type,
		    const unsigned int quantum_size,
		    unsigned char *destination,
		    const ExportPixelAreaOptions *options,
		    ExportPixelAreaInfo *export_info)
{
  const Image
    *image;

  register const IndexPacket
    * restrict indexes;

  register const PixelPacket
    * restrict pixels;

  register unsigned char
    * restrict q;

  register unsigned long
    x;

  register unsigned int
    unsigned_scale = 1U;

  MagickBool
    grayscale_miniswhite = MagickFalse;

  QuantumSampleType
    sample_type = UnsignedQuantumSampleType;

  unsigned int
    unsigned_maxvalue=MaxRGB,
    sample_bits;

  unsigned long
    bytes_exported = 0L,
    number_pixels;

  double
    double_maxvalue=1.0,
    double_minvalue=0.0,
    double_scale;

  EndianType
    endian=MSBEndian;

  MagickPassFail
    status=MagickPass;

  assert(view != (ViewInfo *) NULL);
  assert(destination != (unsigned char *) NULL);
  assert(((quantum_size > 0U) && (quantum_size <= 32U)) || (quantum_size == 64U));
  assert((options == (const ExportPixelAreaOptions *) NULL) ||
         (options->signature == MagickSignature));

  /*
    Transfer any special options.
  */
  sample_bits=quantum_size;
  if (options)
    {
      sample_type=options->sample_type;
      double_minvalue=options->double_minvalue;
      double_maxvalue=options->double_maxvalue;
      grayscale_miniswhite=options->grayscale_miniswhite;

      switch (options->endian)
        {
        case MSBEndian:
        case UndefinedEndian:
          {
            endian=MSBEndian;
            break;
          }
        case LSBEndian:
          {
            endian=LSBEndian;
            break;
          }
        case NativeEndian:
          {
#if defined(WORDS_BIGENDIAN)
            endian=MSBEndian;
#else
            endian=LSBEndian;
#endif
            break;
          }
        }
    }

  if (export_info)
    {
      export_info->bytes_exported=0;
    }

  /*   printf("quantum_type=%d  quantum_size=%u  endian=%s\n",(int) quantum_type, quantum_size, EndianTypeToString(endian)); */

  double_scale=(double) (double_maxvalue-double_minvalue)/MaxRGB;
  if ((sample_type != FloatQuantumSampleType) && (sample_bits <= 32U))
    {
      /* Maximum value which may be represented by a sample */
      unsigned_maxvalue=MaxValueGivenBits(sample_bits);
      
      if (QuantumDepth == sample_bits)
        {
        }
      else if (QuantumDepth > sample_bits)
        {
          /* Divide to scale down */
          unsigned_scale=(MaxRGB / (MaxRGB >> (QuantumDepth-sample_bits)));
        }
      else if (QuantumDepth < sample_bits)
        {
          /* Multiply to scale up */
          unsigned_scale=(unsigned_maxvalue/MaxRGB);
        }
    }

  image=GetCacheViewImage(view);
  number_pixels=(unsigned long) GetCacheViewArea(view);
  pixels=AccessCacheViewPixels(view);
  indexes=AcquireCacheViewIndexes(view);
  q=destination;
  switch (quantum_type)
    {
    case UndefinedQuantum:
      {
	status=MagickFail;
	break;
      }
    case IndexQuantum:
      {
	status=ExportIndexQuantumType(destination,indexes,number_pixels,
				      quantum_size,sample_type,endian,
				      image,&bytes_exported);
        break;
      }
    case IndexAlphaQuantum:
      {
	status=ExportIndexAlphaQuantumType(destination,pixels,indexes,number_pixels,
					   quantum_size,sample_type,endian,unsigned_scale,
					   image,&bytes_exported);
        break;
      }
    case GrayQuantum:
      {
	status=ExportGrayQuantumType(destination,pixels,indexes,number_pixels,quantum_size,
				     sample_type,endian,unsigned_scale,grayscale_miniswhite,
				     sample_bits,double_minvalue,double_scale,image,
				     &bytes_exported);
        break;
      }
    case GrayAlphaQuantum:
      {
	status=ExportGrayAlphaQuantumType(destination,pixels,number_pixels,quantum_size,
					  sample_type,endian,unsigned_scale,grayscale_miniswhite,
					  sample_bits,double_minvalue,double_scale,
					  image,&bytes_exported);
        break;
      }
    case RedQuantum:
    case CyanQuantum:
      {
	status=ExportRedQuantumType(destination,pixels,number_pixels,quantum_size,
				    sample_type,endian,unsigned_scale,sample_bits,
				    double_minvalue,double_scale,&bytes_exported);
        break;
      }
    case GreenQuantum:
    case MagentaQuantum:
      {
	status=ExportGreenQuantumType(destination,pixels,number_pixels,quantum_size,
				      sample_type,endian,unsigned_scale,sample_bits,
				      double_minvalue,double_scale,&bytes_exported);
        break;
      }
    case BlueQuantum:
    case YellowQuantum:
      {
	status=ExportBlueQuantumType(destination,pixels,number_pixels,quantum_size,sample_type,
				     endian,unsigned_scale,sample_bits,double_minvalue,double_scale,
				     &bytes_exported);
        break;
      }
    case AlphaQuantum:
	{
	  status=ExportAlphaQuantumType(destination,pixels,indexes,number_pixels,quantum_size,
					sample_type,endian,unsigned_scale,sample_bits,
					double_minvalue,double_scale,image,&bytes_exported);
	  break;
	}
    case BlackQuantum:
      {
	status=ExportBlackQuantumType(destination,pixels,number_pixels,quantum_size,sample_type,
				      endian,unsigned_scale,sample_bits,double_minvalue,double_scale,
				      &bytes_exported);
        break;
      }
    case RGBQuantum:
      {
	status=ExportRGBQuantumType(destination,pixels,number_pixels,quantum_size,sample_type,
				    endian,unsigned_scale,sample_bits,double_minvalue,double_scale,
				    &bytes_exported);
        break;
      }
    case RGBAQuantum:
      {
	status=ExportRGBAQuantumType(destination,pixels,number_pixels,quantum_size,sample_type,
				     endian,unsigned_scale,sample_bits,double_minvalue,double_scale,
				     &bytes_exported);
        break;
      }
    case CMYKQuantum:
      {
	status=ExportCMYKQuantumType(destination,pixels,number_pixels,quantum_size,sample_type,
				     endian,unsigned_scale,sample_bits,double_minvalue,double_scale,
				     &bytes_exported);
        break;
      }
    case CMYKAQuantum:
      {
	status=ExportCMYKAQuantumType(destination,pixels,indexes,number_pixels,quantum_size,
				      sample_type,endian,unsigned_scale,sample_bits,double_minvalue,
				      double_scale,&bytes_exported);
        break;
      }
    case CIEYQuantum:
    case CIEXYZQuantum:
      {
	status=MagickFail;
	break;
      }
    }

  /*
    Appended any requested padding bytes.
  */
  if ((options) && (options->pad_bytes > 0))
    {
      q=(destination+bytes_exported);
      for (x = options->pad_bytes; x != 0; --x)
	*q++=options->pad_value;
      bytes_exported=(q-destination);
    }

  /*
    Collect export info.
  */
  if (export_info)
    {
      export_info->bytes_exported=bytes_exported;
    }

  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   E x p o r t P i x e l A r e a O p t i o n s I n i t                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ExportPixelAreaOptionsInit() initializes the options structure which is
%  optionally passed to ExportPixelArea()
%
%  The format of the ExportPixelAreaOptionsInit method is:
%
%      void ExportPixelAreaOptionsInit(ExportPixelAreaOptions *options)
%
%  A description of each parameter follows:
%
%    o options: Options structure to initialize.
%
*/
MagickExport void ExportPixelAreaOptionsInit(ExportPixelAreaOptions *options)
{
  assert(options != (ExportPixelAreaOptions *) NULL);
  (void) memset((void *) options, 0, sizeof(ExportPixelAreaOptions));
  options->sample_type=UnsignedQuantumSampleType;
  options->double_minvalue=0.0;
  options->double_maxvalue=1.0;
  options->grayscale_miniswhite=MagickFalse;
  options->pad_bytes=0;
  options->pad_value=0;
  options->endian=MSBEndian;
  options->signature=MagickSignature;
}
