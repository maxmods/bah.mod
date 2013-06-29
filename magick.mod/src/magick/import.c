/*
  Copyright (C) 2003 - 2012 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio

  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  Methods to import image pixels from common representations

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
  Macros
*/

#if defined(WORDS_BIGENDIAN)
#  define MyEndianType MSBEndian
#else
#  define MyEndianType LSBEndian
#endif

#define ImportModulo8Quantum(quantum,quantum_size,p)    \
  {                                                     \
    register unsigned int                               \
      shift;                                            \
                                                        \
    quantum=0;                                          \
    if (LSBEndian != endian)                            \
      {                                                 \
        shift=quantum_size;                             \
        do                                              \
          {                                             \
            shift -= 8U;                                \
            quantum |= (*p++ << shift);                 \
          } while( shift > 0U);                         \
      }                                                 \
    else                                                \
      {                                                 \
        shift=0U;                                       \
        while ( shift < quantum_size )                  \
          {                                             \
            quantum |= (*p++ << shift);                 \
            shift += 8U;                                \
          }                                             \
      }                                                 \
  }
#define ImportUInt8Quantum(quantum,p)           \
  {                                             \
    quantum=*p++;                               \
  }
#define ImportUInt16Quantum(endian,quantum,p)   \
  {                                             \
    if (LSBEndian != endian)                    \
      {                                         \
        quantum=(*p++ << 8);                    \
        quantum|=(*p++);                        \
      }                                         \
    else                                        \
      {                                         \
        quantum=(*p++);                         \
        quantum|=(*p++ << 8);                   \
      }                                         \
  }
/*
  This algorithm has been compared with several others and did best
  overall on SPARC, PowerPC, and Intel Xeon.
*/
#define ImportUInt32Quantum(endian,quantum,p)   \
  {                                             \
    if (LSBEndian != endian)                    \
      {                                         \
        quantum=(*p++ << 24);                   \
        quantum|=(*p++ << 16);                  \
        quantum|=(*p++ << 8);                   \
        quantum|=(*p++);                        \
      }                                         \
    else                                        \
      {                                         \
        quantum=(*p++);                         \
        quantum|=(*p++ << 8);                   \
        quantum|=(*p++ << 16);                  \
        quantum|=(*p++ << 24);                  \
      }                                         \
  }
/*
  Import a 64-bit unsigned value into a 32-bit quantum type.  This
  approach is used since 64-bits is not supported internally and some
  CPUs may perform poorly when using a true 64-bit type.  In this case
  the least significant 32 bits are entirely ignored.
*/
#define ImportUInt64Quantum(endian,quantum,p)   \
  {                                             \
    if (LSBEndian != endian)                    \
      {                                         \
        quantum=(*p++ << 24);                   \
        quantum|=(*p++ << 16);                  \
        quantum|=(*p++ << 8);                   \
        quantum|=(*p++);                        \
        p+=4;                                   \
      }                                         \
    else                                        \
      {                                         \
        p+=4;                                   \
        quantum=(*p++);                         \
        quantum|=(*p++ << 8);                   \
        quantum|=(*p++ << 16);                  \
        quantum|=(*p++ << 24);                  \
      }                                         \
  }
#define ImportFloat16Quantum(endian,value,p)			\
  {								\
    float float_value;						\
    unsigned char c[2];						\
    if (MyEndianType == endian)					\
      {								\
        c[0]=*p++;						\
        c[1]=*p++;						\
      }								\
    else							\
      {								\
        c[1]=*p++;						\
        c[0]=*p++;						\
      }								\
    (void) _Gm_convert_fp16_to_fp32((const fp_16bits *)c,	\
				    &float_value);		\
    value=float_value;						\
  }
#define ImportFloat24Quantum(endian,value,p)			\
  {								\
    float float_value;						\
    unsigned char c[3];						\
    if (MyEndianType == endian)					\
      {								\
        c[0]=*p++;						\
        c[1]=*p++;						\
        c[2]=*p++;						\
      }								\
    else							\
      {								\
        c[2]=*p++;						\
        c[1]=*p++;						\
        c[0]=*p++;						\
      }								\
    (void) _Gm_convert_fp24_to_fp32((const fp_24bits *)c,	\
				    &float_value,		\
				    RANGE_LIMITED);		\
    value=float_value;						\
}
#define ImportFloat32Quantum(endian,value,p)    \
  {                                             \
    if (MyEndianType == endian)                 \
      {                                         \
        value=*((float *) p);                   \
        p += sizeof(float);                     \
      }                                         \
    else                                        \
      {                                         \
        union                                   \
        {                                       \
          float f;                              \
          unsigned char c[4];                   \
        } fu_;                                  \
                                                \
        fu_.c[3]=*p++;                          \
        fu_.c[2]=*p++;                          \
        fu_.c[1]=*p++;                          \
        fu_.c[0]=*p++;                          \
        value=fu_.f;                            \
      }                                         \
  }
#define ImportFloat64Quantum(endian,value,p)    \
  {                                             \
    if (MyEndianType == endian)                 \
      {                                         \
        value=*((double *) p);                  \
        p += sizeof(double);                    \
      }                                         \
    else                                        \
      {                                         \
        union                                   \
        {                                       \
          double d;                             \
          unsigned char c[8];                   \
        } du_;                                  \
                                                \
        du_.c[7]=*p++;                          \
        du_.c[6]=*p++;                          \
        du_.c[5]=*p++;                          \
        du_.c[4]=*p++;                          \
        du_.c[3]=*p++;                          \
        du_.c[2]=*p++;                          \
        du_.c[1]=*p++;                          \
        du_.c[0]=*p++;                          \
        value=du_.d;                            \
      }                                         \
  }

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I m p o r t I m a g e P i x e l A r e a                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ImportImagePixelArea() transfers one or more pixel components from a user
%  supplied buffer into the default image pixel cache view. By default,
%  values are read in network (big-endian) byte/bit order.  By setting the
%  'endian' member of ExportPixelAreaOptions, 16, 32 and 64-bit values may
%  be output as little (LSBEndian), big (MSBEndian), or host native
%  (NativeEndian) endian values.  This function is quite powerful in that
%  besides common native CPU type sizes, it can support any integer bit
%  depth from 1 to 32 (e.g. 13), 64-bits, as well as 32 and 64-bit float.
%
%  MagickPass is returned if the pixels are successfully transferred,
%  otherwise MagickFail.
%
%  The format of the ImportImagePixelArea method is:
%
%      MagickPassFail ImportImagePixelArea(Image *image,
%                                          const QuantumType quantum_type,
%                                          const unsigned int quantum_size,
%                                          const unsigned char *source,
%                                          const ImportPixelAreaOptions *options,
%                                          ImportPixelAreaInfo *import_info)
%
%  A description of each parameter follows:
%
%    o status: Method PushImagePixels returns MagickPass if the pixels are
%      successfully transferred, otherwise MagickFail.
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
%    o source:  The pixel components are transferred from this buffer.
%
%    o options: Additional options specific to quantum_type (may be NULL).
%
%    o import_info : Populated with information regarding the pixels
%               imported (may be NULL)
%
*/
MagickExport MagickPassFail ImportImagePixelArea(Image *image,
  const QuantumType quantum_type,const unsigned int quantum_size,
  const unsigned char *source,const ImportPixelAreaOptions *options,
  ImportPixelAreaInfo *import_info)
{
  return ImportViewPixelArea(AccessDefaultCacheView(image),
                             quantum_type,quantum_size,
                             source,options,import_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I m p o r t V i e w P i x e l A r e a                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ImportViewPixelArea() transfers one or more pixel components from a user
%  supplied buffer into the specified image pixel cache view of an image. By
%  default, values are read in network (big-endian) byte/bit order.  By
%  setting the 'endian' member of ExportPixelAreaOptions, 16, 32 and 64-bit
%  values may be output as little (LSBEndian), big (MSBEndian), or host
%  native (NativeEndian) endian values.  This function is quite powerful in
%  that besides common native CPU type sizes, it can support any integer bit
%  depth from 1 to 32 (e.g. 13), 64-bits, as well as 32 and 64-bit float.
%
%  MagickPass is returned if the pixels are successfully transferred,
%  otherwise MagickFail.
%
%  The format of the ImportViewPixelArea method is:
%
%      MagickPassFail ImportViewPixelArea(ViewInfo *view,
%                                         const QuantumType quantum_type,
%                                         const unsigned int quantum_size,
%                                         const unsigned char *source,
%                                         const ImportPixelAreaOptions *options,
%                                         ImportPixelAreaInfo *import_info)
%
%  A description of each parameter follows:
%
%    o status: Method PushImagePixels returns MagickPass if the pixels are
%      successfully transferred, otherwise MagickFail.
%
%    o view: The pixel view to import pixels into.
%
%    o quantum_type: Declare which pixel components to transfer (AlphaQuantum,
%        BlackQuantum, BlueQuantum, CMYKAQuantum, CMYKQuantum, CyanQuantum,
%        GrayAlphaQuantum, GrayQuantum, GreenQuantum, IndexAlphaQuantum,
%        IndexQuantum, MagentaQuantum, RGBAQuantum, RGBQuantum,
%        RedQuantum, YellowQuantum)
%
%    o quantum_size: Bits per quantum sample (range 1-32, and 64).
%
%    o source:  The pixel components are transferred from this buffer.
%
%    o options: Additional options specific to quantum_type (may be NULL).
%
%    o import_info : Populated with information regarding the pixels
%               imported (may be NULL)
%
*/
static MagickPassFail
ImportIndexQuantumType(const unsigned char *source,
		       PixelPacket* restrict q,
		       IndexPacket * restrict indexes,
		       const unsigned long number_pixels,
		       const unsigned int quantum_size,
		       const QuantumSampleType sample_type,
		       const unsigned int unsigned_maxvalue,
		       const EndianType endian,
		       Image *image,
		       ImportPixelAreaInfo *import_info)
{
  const unsigned char * restrict
    p;

  register unsigned long
    x;

  register unsigned int
    index;

  assert(image->colors <= MaxColormapSize);
  assert(image->colors != 0);
  assert(indexes != (IndexPacket *) NULL);

  p=source;
  if (sample_type == UnsignedQuantumSampleType)
    {
      switch (quantum_size)
	{
	case 1:
	  {
	    /*
	      Special fast support for two colors.
	    */
	    register unsigned int
	      bit = 8U;
                  
	    for (x = number_pixels ; x != 0 ; --x )
	      {
		--bit;
		index=(*p >> bit) & 0x01;
		VerifyColormapIndex(image,index);
		*indexes++=index;
		*q++=image->colormap[index];
		if (bit == 0)
		  {
		    bit=8;
		    p++;
		  }
	      }
	    break;
	  }
	case 4:
	  {
	    /*
	      Special fast support for 16 colors.
	    */
	    register unsigned int
	      state = 0;
                  
	    for (x = number_pixels ; x != 0 ; --x )
	      {
		state ^= 1; /* Produces 1 0 1 0 ... */
		index=(IndexPacket) ((state ? (*p >> 4) : (*p++)) & 0xf);
		VerifyColormapIndex(image,index);
		*indexes++=index;
		*q++=image->colormap[index];
	      }
	    break;
	  }
	case 8:
	  {
	    if (unsigned_maxvalue <= (unsigned int) (image->colors-1))
	      {
		/*
		  Special case for when it is impossible to
		  overflow the colormap range.
		*/
		for (x = number_pixels; x != 0; --x)
		  {
		    ImportUInt8Quantum(index,p);
		    *indexes++=index;
		    *q++=image->colormap[index];
		  }
	      }
	    else
	      {
		for (x = number_pixels; x != 0; --x)
		  {
		    ImportUInt8Quantum(index,p);
		    VerifyColormapIndex(image,index);
		    *indexes++=index;
		    *q++=image->colormap[index];
		  }
	      }
	    break;
	  }
	case 16:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportUInt16Quantum(endian,index,p);
		VerifyColormapIndex(image,index);
		*indexes++=index;
		*q++=image->colormap[index];
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportUInt32Quantum(endian,index,p);
		VerifyColormapIndex(image,index);
		*indexes++=index;
		*q++=image->colormap[index];
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportUInt64Quantum(endian,index,p);
		VerifyColormapIndex(image,index);
		*indexes++=index;
		*q++=image->colormap[index];
	      }
	    break;
	  }
	default:
	  {
	    /*
	      Arbitrary sample size
	    */
	    BitStreamReadHandle
	      stream;
                  
	    MagickBitStreamInitializeRead(&stream,p);
	    for (x = number_pixels; x != 0; --x)
	      {
		index=MagickBitStreamMSBRead(&stream,quantum_size);
		VerifyColormapIndex(image,index);
		*indexes++=index;
		*q++=image->colormap[index];
	      }
	    break;
	  }
	}
    }

  if (import_info)
    {
      import_info->bytes_imported=p-source;
    }

  return MagickPass;
}

static MagickPassFail
ImportIndexAlphaQuantumType(const unsigned char *source,
			    PixelPacket* restrict q,
			    IndexPacket * restrict indexes,
			    const unsigned long number_pixels,
			    const unsigned int quantum_size,
			    const QuantumSampleType sample_type,
			    const unsigned int unsigned_scale,
			    const EndianType endian,
			    Image *image,
			    ImportPixelAreaInfo *import_info)
{
  const unsigned char * restrict
    p;

  register unsigned long
    x;

  register unsigned int
    index,
    unsigned_value;

   unsigned int
     sample_bits;

  assert(image->colors <= MaxColormapSize);

  sample_bits=quantum_size;
  p=source;

  if (sample_type == UnsignedQuantumSampleType)
    {
      switch(quantum_size)
	{
	case 8:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportUInt8Quantum(index,p);
		VerifyColormapIndex(image,index);
		*indexes++=index;
		*q=image->colormap[index];
                      
		ImportUInt8Quantum(unsigned_value,p);
		SetOpacitySample(q,MaxRGB-ScaleCharToQuantum(unsigned_value));
		q++;
	      }
	    break;
	  }
	case 16:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportUInt16Quantum(endian,index,p);
		VerifyColormapIndex(image,index);
		*indexes++=index;
		*q=image->colormap[index];
                      
		ImportUInt16Quantum(endian,unsigned_value,p);
		SetOpacitySample(q,MaxRGB-ScaleShortToQuantum(unsigned_value));
		q++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportUInt32Quantum(endian,index,p);
		VerifyColormapIndex(image,index);
		*indexes++=index;
		*q=image->colormap[index];
                      
		ImportUInt32Quantum(endian,unsigned_value,p);
		SetOpacitySample(q,MaxRGB-ScaleLongToQuantum(unsigned_value));
		q++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportUInt64Quantum(endian,index,p);
		VerifyColormapIndex(image,index);
		*indexes++=index;
		*q=image->colormap[index];
                      
		ImportUInt64Quantum(endian,unsigned_value,p);
		SetOpacitySample(q,MaxRGB-ScaleLongToQuantum(unsigned_value));
		q++;
	      }
	    break;
	  }
	default:
	  {
	    /*
	      Arbitrary sample size
	    */
	    BitStreamReadHandle
	      stream;
                  
	    MagickBitStreamInitializeRead(&stream,p);
	    for (x = number_pixels; x != 0; --x)
	      {
		index=MagickBitStreamMSBRead(&stream,quantum_size);
		VerifyColormapIndex(image,index);
		*indexes++=index;
		*q=image->colormap[index];
                      
		unsigned_value=MagickBitStreamMSBRead(&stream,quantum_size);
		if (QuantumDepth >  sample_bits)
		  unsigned_value *= unsigned_scale;
		else if (QuantumDepth <  sample_bits)
		  unsigned_value /= unsigned_scale;
		SetOpacitySample(q,MaxRGB-unsigned_value);
		q++;
	      }
	    break;
	  }
	}
    }

  if (import_info)
    {
      import_info->bytes_imported=p-source;
    }

  return MagickPass;
}

static MagickPassFail
ImportGrayQuantumType(const unsigned char *source,
		      PixelPacket* restrict q,
		      IndexPacket * restrict indexes,
		      const unsigned long number_pixels,
		      const unsigned int quantum_size,
		      const QuantumSampleType sample_type,
		      const unsigned int unsigned_scale,
		      const unsigned int unsigned_maxvalue,
		      const MagickBool grayscale_miniswhite,
		      const double double_minvalue,
		      const double double_scale,
		      const EndianType endian,
		      Image *image,
		      ImportPixelAreaInfo *import_info)
{
  const unsigned char * restrict
    p;

  register unsigned long
    x;

  double
    double_value;

  register unsigned int
    index,
    unsigned_value;

  unsigned int
    sample_bits;

  sample_bits=quantum_size;
  p=source;

  if (sample_type == UnsignedQuantumSampleType)
    {
      if (DirectClass == image->storage_class)
	{
	  /*
	    DirectClass representation.
	  */
	  switch (quantum_size)
	    {
	    case 1:
	      {
		/*
		  Special fast support for bi-level gray.
		*/
		register int
		  bit = 8;
                      
		PixelPacket
		  min_val,
		  max_val;

		if (grayscale_miniswhite)
		  {
		    min_val=WhitePixel;
		    max_val=BlackPixel;
		  }
		else
		  {
		    min_val=BlackPixel;
		    max_val=WhitePixel;
		  }
                      
		for (x = number_pixels ; x != 0 ; --x )
		  {
		    --bit;
		    *q++=(((*p >> bit) & 0x01) ? max_val : min_val);
		    if (bit == 0)
		      {
			bit=8;
			p++;
		      }
		  }
		if (bit != 8)
		  p++;
		break;
	      }
	    case 8:
	      {
		if (grayscale_miniswhite)
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			SetGraySample(q,MaxRGB-ScaleCharToQuantum(*p++));
			SetOpacitySample(q,OpaqueOpacity);
			q++;
		      }
		  }
		else
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			SetGraySample(q,ScaleCharToQuantum(*p++));
			SetOpacitySample(q,OpaqueOpacity);
			q++;
		      }
		  }
		break;
	      }
	    case 16:
	      {
		if (grayscale_miniswhite)
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ImportUInt16Quantum(endian,unsigned_value,p);
			SetGraySample(q,MaxRGB-ScaleShortToQuantum(unsigned_value));
			SetOpacitySample(q,OpaqueOpacity);
			q++;
		      }
		  }
		else
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ImportUInt16Quantum(endian,unsigned_value,p);
			SetGraySample(q,ScaleShortToQuantum(unsigned_value));
			SetOpacitySample(q,OpaqueOpacity);
			q++;
		      }
		  }
		break;
	      }
	    case 32:
	      {
		if (grayscale_miniswhite)
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ImportUInt32Quantum(endian,unsigned_value,p);
			SetGraySample(q,MaxRGB-ScaleLongToQuantum(unsigned_value));
			SetOpacitySample(q,OpaqueOpacity);
			q++;
		      }
		  }
		else
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ImportUInt32Quantum(endian,unsigned_value,p);
			SetGraySample(q,ScaleLongToQuantum(unsigned_value));
			SetOpacitySample(q,OpaqueOpacity);
			q++;
		      }
		  }
		break;
	      }
	    case 64:
	      {
		if (grayscale_miniswhite)
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ImportUInt64Quantum(endian,unsigned_value,p);
			SetGraySample(q,MaxRGB-ScaleLongToQuantum(unsigned_value));
			SetOpacitySample(q,OpaqueOpacity);
			q++;
		      }
		  }
		else
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ImportUInt64Quantum(endian,unsigned_value,p);
			SetGraySample(q,ScaleLongToQuantum(unsigned_value));
			SetOpacitySample(q,OpaqueOpacity);
			q++;
		      }
		  }
		break;
	      }
	    default:
	      {
		BitStreamReadHandle
		  stream;
                      
		MagickBitStreamInitializeRead(&stream,p);
		for (x = number_pixels; x != 0; --x)
		  {
		    unsigned_value=MagickBitStreamMSBRead(&stream,quantum_size);
		    if (QuantumDepth >  sample_bits)
		      unsigned_value *= unsigned_scale;
		    else if (QuantumDepth <  sample_bits)
		      unsigned_value /= unsigned_scale;
		    if (grayscale_miniswhite)
		      unsigned_value = MaxRGB-unsigned_value;
		    SetGraySample(q,unsigned_value);
		    SetOpacitySample(q,OpaqueOpacity);
		    q++;
		  }
		break;
	      }
	    }
	}
      else
	{
	  /*
	    PseudoClass representation.
                  
	    Note that this implementation assumes that the
	    colormap is written in ascending levels of intensity
	    as produced by AllocateImageColormap().  Some old
	    code may assume that 'miniswhite' inverts the
	    colormap order as well.
	  */
	  assert(image->colors <= MaxColormapSize);
                
	  switch (quantum_size)
	    {
	    case 1:
	      {
		/*
		  Special fast support for bi-level gray.
		*/
		register int
		  bit = 8;
                      
		for (x = number_pixels ; x != 0 ; --x )
		  {
		    --bit;
		    index=(*p >> bit) & 0x01;
		    if (grayscale_miniswhite)
		      index ^= 0x01;
		    *indexes++=index;
		    *q++=image->colormap[index];
		    if (bit == 0)
		      {
			bit=8;
			p++;
		      }
		  }
		if (bit != 8)
		  p++;
		break;
	      }
	    case 8:
	      {
		for (x = number_pixels; x != 0; --x)
		  {
		    ImportUInt8Quantum(index,p);
		    VerifyColormapIndex(image,index);
		    if (grayscale_miniswhite)
		      index=(image->colors-1)-index;
		    *indexes++=index;
		    *q++=image->colormap[index];
		  }
		break;
	      }
	    case 16:
	      {
		for (x = number_pixels; x != 0; --x)
		  {
		    ImportUInt16Quantum(endian,index,p);
		    VerifyColormapIndex(image,index);
		    if (grayscale_miniswhite)
		      index=(image->colors-1)-index;
		    *indexes++=index;
		    *q++=image->colormap[index];
		  }
		break;
	      }
	    case 32:
	      {
		for (x = number_pixels; x != 0; --x)
		  {
		    ImportUInt32Quantum(endian,index,p);
		    VerifyColormapIndex(image,index);
		    if (grayscale_miniswhite)
		      index=(image->colors-1)-index;
		    *indexes++=index;
		    *q++=image->colormap[index];
		  }
		break;
	      }
	    case 64:
	      {
		for (x = number_pixels; x != 0; --x)
		  {
		    ImportUInt64Quantum(endian,index,p);
		    VerifyColormapIndex(image,index);
		    if (grayscale_miniswhite)
		      index=(image->colors-1)-index;
		    *indexes++=index;
		    *q++=image->colormap[index];
		  }
		break;
	      }
	    default:
	      {
		/*
		  Arbitrary sample size
		*/
		BitStreamReadHandle
		  stream;

		register unsigned int
		  indexes_scale = 1U;

		if (unsigned_maxvalue > (image->colors-1))
		  indexes_scale=(unsigned_maxvalue/(image->colors-1));
                
		MagickBitStreamInitializeRead(&stream,p);
		for (x = number_pixels; x != 0; --x)
		  {
		    index=MagickBitStreamMSBRead(&stream,quantum_size);
		    index /= indexes_scale;
		    VerifyColormapIndex(image,index);
		    if (grayscale_miniswhite)
		      index=(image->colors-1)-index;
		    *indexes++=index;
		    *q++=image->colormap[index];
		  }
		break;
	      }
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
		ImportFloat16Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetGraySample(q,RoundDoubleToQuantum(double_value));
		q++;
	      }
	    break;
	  }
	case 24:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportFloat24Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetGraySample(q,RoundDoubleToQuantum(double_value));
		q++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportFloat32Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetGraySample(q,RoundDoubleToQuantum(double_value));
		q++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportFloat64Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetGraySample(q,RoundDoubleToQuantum(double_value));
		q++;
	      }
	    break;
	  }
	default:
	  break;
	}
    }

  if (import_info)
    {
      import_info->bytes_imported=p-source;
    }

  return MagickPass;
}

static MagickPassFail
ImportGrayAlphaQuantumType(const unsigned char *source,
			   PixelPacket* restrict q,
			   IndexPacket * restrict indexes,
			   const unsigned long number_pixels,
			   const unsigned int quantum_size,
			   const QuantumSampleType sample_type,
			   const unsigned int unsigned_scale,
			   const unsigned int unsigned_maxvalue,
			   const MagickBool grayscale_miniswhite,
			   const double double_minvalue,
			   const double double_scale,
			   const EndianType endian,
			   Image *image,
			   ImportPixelAreaInfo *import_info)
{
  const unsigned char * restrict
    p;

  register unsigned long
    x;

  double
    double_value;

  register unsigned int
    index,
    unsigned_value;

  unsigned int
    sample_bits;

  sample_bits=quantum_size;
  p=source;

  if (sample_type == UnsignedQuantumSampleType)
    {
      if (DirectClass == image->storage_class)
	{
	  /*
	    DirectClass representation.
	  */
	  switch (quantum_size)
	    {
	    case 8:
	      {
		if (grayscale_miniswhite)
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ImportUInt8Quantum(unsigned_value,p);
			SetGraySample(q,MaxRGB-ScaleCharToQuantum(unsigned_value));
			ImportUInt8Quantum(unsigned_value,p);
			SetOpacitySample(q,MaxRGB-ScaleCharToQuantum(unsigned_value));
			q++;
		      }
		  }
		else
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ImportUInt8Quantum(unsigned_value,p);
			SetGraySample(q,ScaleCharToQuantum(unsigned_value));
			ImportUInt8Quantum(unsigned_value,p);
			SetOpacitySample(q,MaxRGB-ScaleCharToQuantum(unsigned_value));
			q++;
		      }
		  }
		break;
	      }
	    case 16:
	      {
		if (grayscale_miniswhite)
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ImportUInt16Quantum(endian,unsigned_value,p);
			SetGraySample(q,MaxRGB-ScaleShortToQuantum(unsigned_value));
			ImportUInt16Quantum(endian,unsigned_value,p);
			SetOpacitySample(q,MaxRGB-ScaleShortToQuantum(unsigned_value));
			q++;
		      }
		  }
		else
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ImportUInt16Quantum(endian,unsigned_value,p);
			SetGraySample(q,ScaleShortToQuantum(unsigned_value));
			ImportUInt16Quantum(endian,unsigned_value,p);
			SetOpacitySample(q,MaxRGB-ScaleShortToQuantum(unsigned_value));
			q++;
		      }
		  }
		break;
	      }
	    case 32:
	      {
		if (grayscale_miniswhite)
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ImportUInt32Quantum(endian,unsigned_value,p);
			SetGraySample(q,MaxRGB-ScaleLongToQuantum(unsigned_value));
			ImportUInt32Quantum(endian,unsigned_value,p);
			SetOpacitySample(q,MaxRGB-ScaleLongToQuantum(unsigned_value));
			q++;
		      }
		  }
		else
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ImportUInt32Quantum(endian,unsigned_value,p);
			SetGraySample(q,ScaleLongToQuantum(unsigned_value));
			ImportUInt32Quantum(endian,unsigned_value,p);
			SetOpacitySample(q,MaxRGB-ScaleLongToQuantum(unsigned_value));
			q++;
		      }
		  }
		break;
	      }
	    case 64:
	      {
		if (grayscale_miniswhite)
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ImportUInt64Quantum(endian,unsigned_value,p);
			SetGraySample(q,MaxRGB-ScaleLongToQuantum(unsigned_value));
			ImportUInt64Quantum(endian,unsigned_value,p);
			SetOpacitySample(q,MaxRGB-ScaleLongToQuantum(unsigned_value));
			q++;
		      }
		  }
		else
		  {
		    for (x = number_pixels; x != 0; --x)
		      {
			ImportUInt64Quantum(endian,unsigned_value,p);
			SetGraySample(q,ScaleLongToQuantum(unsigned_value));
			ImportUInt64Quantum(endian,unsigned_value,p);
			SetOpacitySample(q,MaxRGB-ScaleLongToQuantum(unsigned_value));
			q++;
		      }
		  }
		break;
	      }
	    default:
	      {
		/*
		  Arbitrary Depth.
		*/
		BitStreamReadHandle
		  stream;
                      
		MagickBitStreamInitializeRead(&stream,p);
		for (x = number_pixels; x != 0; --x)
		  {
		    unsigned_value=MagickBitStreamMSBRead(&stream,quantum_size);
		    if (QuantumDepth >  sample_bits)
		      unsigned_value *= unsigned_scale;
		    else if (QuantumDepth <  sample_bits)
		      unsigned_value /= unsigned_scale;
		    if (grayscale_miniswhite)
		      unsigned_value = MaxRGB-unsigned_value;
		    SetGraySample(q,unsigned_value);

		    unsigned_value=MagickBitStreamMSBRead(&stream,quantum_size);
		    if (QuantumDepth >  sample_bits)
		      unsigned_value *= unsigned_scale;
		    else if (QuantumDepth <  sample_bits)
		      unsigned_value /= unsigned_scale;
		    SetOpacitySample(q,MaxRGB-unsigned_value);
		    q++;
		  }
		break;
	      }
	    }
	}
      else
	{
	  /*
	    PseudoClass representation.
	  */
	  /*
	    Input is organized as a gray level followed by opacity level
	    Colormap array is pre-stuffed with ascending or descending gray
	    levels according to the gray quantum representation.
	  */
	  register unsigned int
	    indexes_scale = 1U;

	  assert(image->colors <= MaxColormapSize);

	  if (unsigned_maxvalue > (image->colors-1))
	    indexes_scale=(unsigned_maxvalue/(image->colors-1));

	  if ( (quantum_size >= 8) && (quantum_size % 8U == 0U) )
	    {
	      /*
		Modulo-8 sample sizes
	      */
	      if (indexes_scale == 1U)
		{
		  for (x = number_pixels; x != 0; --x)
		    {
		      ImportModulo8Quantum(index,quantum_size,p);
		      VerifyColormapIndex(image,index);
		      if (grayscale_miniswhite)
			index=(image->colors-1)-index;
		      *indexes++=index;
		      *q=image->colormap[index];
                    
		      ImportModulo8Quantum(unsigned_value,quantum_size,p);
		      if (QuantumDepth >  sample_bits)
			unsigned_value *= unsigned_scale;
		      else if (QuantumDepth <  sample_bits)
			unsigned_value /= unsigned_scale;
		      SetOpacitySample(q,MaxRGB-unsigned_value);
		      q++;
		    }
		}
	      else
		{
		  for (x = number_pixels; x != 0; --x)
		    {
		      ImportModulo8Quantum(index,quantum_size,p);
		      index /= indexes_scale;
		      VerifyColormapIndex(image,index);
		      if (grayscale_miniswhite)
			index=(image->colors-1)-index;
		      *indexes++=index;
		      *q=image->colormap[index];
                    
		      ImportModulo8Quantum(unsigned_value,quantum_size,p);
		      if (QuantumDepth >  sample_bits)
			unsigned_value *= unsigned_scale;
		      else if (QuantumDepth <  sample_bits)
			unsigned_value /= unsigned_scale;
		      SetOpacitySample(q,MaxRGB-unsigned_value);
		      q++;
		    }
		}
	    }
	  else
	    {
	      /*
		Arbitrary sample size
	      */
	      BitStreamReadHandle
		stream;
            
	      MagickBitStreamInitializeRead(&stream,p);
	      for (x = number_pixels; x != 0; --x)
		{
		  index=MagickBitStreamMSBRead(&stream,quantum_size);
		  index /= indexes_scale;
		  VerifyColormapIndex(image,index);
		  if (grayscale_miniswhite)
		    index=(image->colors-1)-index;
		  *indexes++=index;
		  *q=image->colormap[index];

		  unsigned_value=MagickBitStreamMSBRead(&stream,quantum_size);
		  if (QuantumDepth >  sample_bits)
		    unsigned_value *= unsigned_scale;
		  else if (QuantumDepth <  sample_bits)
		    unsigned_value /= unsigned_scale;
		  SetOpacitySample(q,MaxRGB-unsigned_value);
		  q++;
		}
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
		ImportFloat16Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetGraySample(q,RoundDoubleToQuantum(double_value));
		ImportFloat16Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetOpacitySample(q,MaxRGB-RoundDoubleToQuantum(double_value));
		q++;
	      }
	    break;
	  }
	case 24:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportFloat24Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetGraySample(q,RoundDoubleToQuantum(double_value));
		ImportFloat24Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetOpacitySample(q,MaxRGB-RoundDoubleToQuantum(double_value));
		q++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportFloat32Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetGraySample(q,RoundDoubleToQuantum(double_value));
		ImportFloat32Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetOpacitySample(q,MaxRGB-RoundDoubleToQuantum(double_value));
		q++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportFloat64Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetGraySample(q,RoundDoubleToQuantum(double_value));
		ImportFloat64Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetOpacitySample(q,MaxRGB-RoundDoubleToQuantum(double_value));
		q++;
	      }
	    break;
	  }
	default:
	  break;
	}
    }

  if (import_info)
    {
      import_info->bytes_imported=p-source;
    }

  return MagickPass;
}

static MagickPassFail
ImportRedQuantumType(const unsigned char *source,
		     PixelPacket* restrict q,
		     const unsigned long number_pixels,
		     const unsigned int quantum_size,
		     const QuantumSampleType sample_type,
		     const unsigned int unsigned_scale,
		     const double double_minvalue,
		     const double double_scale,
		     const EndianType endian,
		     ImportPixelAreaInfo *import_info)
{
  const unsigned char * restrict
    p;

  register unsigned long
    x;

  double
    double_value;

  register unsigned int
    unsigned_value;

  unsigned int
    sample_bits;

  sample_bits=quantum_size;
  p=source;

  if (sample_type == UnsignedQuantumSampleType)
    {
      switch (quantum_size)
	{
	case 8:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		SetRedSample(q,ScaleCharToQuantum(*p++));
		q++;
	      }
	    break;
	  }
	case 16:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportUInt16Quantum(endian,unsigned_value,p);
		SetRedSample(q,ScaleShortToQuantum(unsigned_value));
		q++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportUInt32Quantum(endian,unsigned_value,p);
		SetRedSample(q,ScaleLongToQuantum(unsigned_value));
		q++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportUInt64Quantum(endian,unsigned_value,p);
		SetRedSample(q,ScaleLongToQuantum(unsigned_value));
		q++;
	      }
	    break;
	  }
	default:
	  {
	    /*
	      Arbitrary sample size
	    */
	    BitStreamReadHandle
	      stream;
                  
	    MagickBitStreamInitializeRead(&stream,p);
	    if (QuantumDepth >=  sample_bits)
	      {
		/* Scale up */
		for (x = number_pixels; x != 0; --x)
		  {
		    SetRedSample(q,MagickBitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
		    q++;
		  }
	      }
	    else
	      {
		/* Scale down */
		for (x = number_pixels; x != 0; --x)
		  {
		    SetRedSample(q,MagickBitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
		    q++;
		  }
	      }
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
		ImportFloat16Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetRedSample(q,RoundDoubleToQuantum(double_value));
		q++;
	      }
	    break;
	  }
	case 24:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportFloat24Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetRedSample(q,RoundDoubleToQuantum(double_value));
		q++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportFloat32Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetRedSample(q,RoundDoubleToQuantum(double_value));
		q++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportFloat64Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetRedSample(q,RoundDoubleToQuantum(double_value));
		q++;
	      }
	    break;
	  }
	default:
	  break;
	}
    }

  if (import_info)
    {
      import_info->bytes_imported=p-source;
    }

  return MagickPass;
}

static MagickPassFail
ImportGreenQuantumType(const unsigned char *source,
		       PixelPacket* restrict q,
		       const unsigned long number_pixels,
		       const unsigned int quantum_size,
		       const QuantumSampleType sample_type,
		       const unsigned int unsigned_scale,
		       const double double_minvalue,
		       const double double_scale,
		       const EndianType endian,
		       ImportPixelAreaInfo *import_info)
{
  const unsigned char * restrict
    p;

  register unsigned long
    x;

  double
    double_value;

  register unsigned int
    unsigned_value;

  unsigned int
    sample_bits;

  sample_bits=quantum_size;
  p=source;

  if (sample_type == UnsignedQuantumSampleType)
    {
      switch (quantum_size)
	{
	case 8:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		SetGreenSample(q,ScaleCharToQuantum(*p++));
		q++;
	      }
	    break;
	  }
	case 16:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportUInt16Quantum(endian,unsigned_value,p);
		SetGreenSample(q,ScaleShortToQuantum(unsigned_value));
		q++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportUInt32Quantum(endian,unsigned_value,p);
		SetGreenSample(q,ScaleLongToQuantum(unsigned_value));
		q++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportUInt64Quantum(endian,unsigned_value,p);
		SetGreenSample(q,ScaleLongToQuantum(unsigned_value));
		q++;
	      }
	    break;
	  }
	default:
	  {
	    /*
	      Arbitrary sample size
	    */
	    BitStreamReadHandle
	      stream;
                  
	    MagickBitStreamInitializeRead(&stream,p);
	    if (QuantumDepth >=  sample_bits)
	      {
		/* Scale up */
		for (x = number_pixels; x != 0; --x)
		  {
		    SetGreenSample(q,MagickBitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
		    q++;
		  }
	      }
	    else
	      {
		/* Scale down */
		for (x = number_pixels; x != 0; --x)
		  {
		    SetGreenSample(q,MagickBitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
		    q++;
		  }
	      }
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
		ImportFloat16Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetGreenSample(q,RoundDoubleToQuantum(double_value));
		q++;
	      }
	    break;
	  }
	case 24:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportFloat24Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetGreenSample(q,RoundDoubleToQuantum(double_value));
		q++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportFloat32Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetGreenSample(q,RoundDoubleToQuantum(double_value));
		q++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportFloat64Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetGreenSample(q,RoundDoubleToQuantum(double_value));
		q++;
	      }
	    break;
	  }
	default:
	  break;
	}
    }

  if (import_info)
    {
      import_info->bytes_imported=p-source;
    }

  return MagickPass;
}

static MagickPassFail
ImportBlueQuantumType(const unsigned char *source,
		      PixelPacket* restrict q,
		      const unsigned long number_pixels,
		      const unsigned int quantum_size,
		      const QuantumSampleType sample_type,
		      const unsigned int unsigned_scale,
		      const double double_minvalue,
		      const double double_scale,
		      const EndianType endian,
		      ImportPixelAreaInfo *import_info)
{
  const unsigned char * restrict
    p;

  register unsigned long
    x;

  double
    double_value;

  register unsigned int
    unsigned_value;

  unsigned int
    sample_bits;

  sample_bits=quantum_size;
  p=source;

  if (sample_type == UnsignedQuantumSampleType)
    {
      switch (quantum_size)
	{
	case 8:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		SetBlueSample(q,ScaleCharToQuantum(*p++));
		q++;
	      }
	    break;
	  }
	case 16:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportUInt16Quantum(endian,unsigned_value,p);
		SetBlueSample(q,ScaleShortToQuantum(unsigned_value));
		q++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportUInt32Quantum(endian,unsigned_value,p);
		SetBlueSample(q,ScaleLongToQuantum(unsigned_value));
		q++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportUInt64Quantum(endian,unsigned_value,p);
		SetBlueSample(q,ScaleLongToQuantum(unsigned_value));
		q++;
	      }
	    break;
	  }
	default:
	  {
	    /*
	      Arbitrary sample size
	    */
	    BitStreamReadHandle
	      stream;
                  
	    MagickBitStreamInitializeRead(&stream,p);
	    if (QuantumDepth >=  sample_bits)
	      {
		/* Scale up */
		for (x = number_pixels; x != 0; --x)
		  {
		    SetBlueSample(q,MagickBitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
		    q++;
		  }
	      }
	    else
	      {
		/* Scale down */
		for (x = number_pixels; x != 0; --x)
		  {
		    SetBlueSample(q,MagickBitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
		    q++;
		  }
	      }
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
		ImportFloat16Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetBlueSample(q,RoundDoubleToQuantum(double_value));
		q++;
	      }
	    break;
	  }
	case 24:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportFloat24Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetBlueSample(q,RoundDoubleToQuantum(double_value));
		q++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportFloat32Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetBlueSample(q,RoundDoubleToQuantum(double_value));
		q++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportFloat64Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetBlueSample(q,RoundDoubleToQuantum(double_value));
		q++;
	      }
	    break;
	  }
	default:
	  break;
	}
    }

  if (import_info)
    {
      import_info->bytes_imported=p-source;
    }

  return MagickPass;
}

static MagickPassFail
ImportAlphaQuantumType(const unsigned char *source,
		       PixelPacket* restrict q,
		       IndexPacket * restrict indexes,
		       const unsigned long number_pixels,
		       const unsigned int quantum_size,
		       const QuantumSampleType sample_type,
		       const unsigned int unsigned_scale,
		       const double double_minvalue,
		       const double double_scale,
		       const EndianType endian,
		       Image *image,
		       ImportPixelAreaInfo *import_info)
{
  const unsigned char * restrict
    p;

  register unsigned long
    x;

  double
    double_value;

  register unsigned int
    unsigned_value;

  unsigned int
    sample_bits;

  sample_bits=quantum_size;
  p=source;

  if (image->colorspace == CMYKColorspace)
    {
      if (sample_type == UnsignedQuantumSampleType)
	{
	  if ( (quantum_size >= 8) && (quantum_size % 8U == 0U) )
	    {
	      /*
		Modulo-8 sample sizes
	      */
	      if( QuantumDepth == sample_bits)
		{
		  /* Unity scale */
		  for (x = number_pixels; x != 0; --x)
		    {
		      ImportModulo8Quantum(unsigned_value,quantum_size,p);
		      *indexes++=(IndexPacket) MaxRGB-unsigned_value;
		    }
		}
	      else if (QuantumDepth >  sample_bits)
		{
		  /* Scale up */
		  for (x = number_pixels; x != 0; --x)
		    {
		      ImportModulo8Quantum(unsigned_value,quantum_size,p);
		      *indexes++=(IndexPacket) MaxRGB-unsigned_value*unsigned_scale;
		    }
		}
	      else
		{
		  /* Scale down */
		  for (x = number_pixels; x != 0; --x)
		    {
		      ImportModulo8Quantum(unsigned_value,quantum_size,p);
		      *indexes++=(IndexPacket) MaxRGB-unsigned_value/unsigned_scale;
		    }
		}
	    }
	  else
	    {
	      /*
		Arbitrary sample size
	      */
	      BitStreamReadHandle
		stream;
                
	      MagickBitStreamInitializeRead(&stream,p);
	      if (QuantumDepth >=  sample_bits)
		{
		  /* Scale up */
		  for (x = number_pixels; x != 0; --x)
		    {
		      *indexes++=(IndexPacket) MaxRGB-MagickBitStreamMSBRead(&stream,quantum_size)*unsigned_scale;
		    }
		}
	      else
		{
		  /* Scale down */
		  for (x = number_pixels; x != 0; --x)
		    {
		      *indexes++=(IndexPacket) MaxRGB-MagickBitStreamMSBRead(&stream,quantum_size)/unsigned_scale;
		    }
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
		    ImportFloat16Quantum(endian,double_value,p);
		    double_value -= double_minvalue;
		    double_value *= double_scale;
		    *indexes++=(IndexPacket) MaxRGB-RoundDoubleToQuantum(double_value);
		  }
		break;
	      }
	    case 24:
	      {
		for (x = number_pixels; x != 0; --x)
		  {
		    ImportFloat24Quantum(endian,double_value,p);
		    double_value -= double_minvalue;
		    double_value *= double_scale;
		    *indexes++=(IndexPacket) MaxRGB-RoundDoubleToQuantum(double_value);
		  }
		break;
	      }
	    case 32:
	      {
		for (x = number_pixels; x != 0; --x)
		  {
		    ImportFloat32Quantum(endian,double_value,p);
		    double_value -= double_minvalue;
		    double_value *= double_scale;
		    *indexes++=(IndexPacket) MaxRGB-RoundDoubleToQuantum(double_value);
		  }
		break;
	      }
	    case 64:
	      {
		for (x = number_pixels; x != 0; --x)
		  {
		    ImportFloat64Quantum(endian,double_value,p);
		    double_value -= double_minvalue;
		    double_value *= double_scale;
		    *indexes++=(IndexPacket) MaxRGB-RoundDoubleToQuantum(double_value);
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
      /* RGB */
      if (sample_type == UnsignedQuantumSampleType)
	{
	  if ( (quantum_size >= 8) && (quantum_size % 8U == 0U) )
	    {
	      /*
		Modulo-8 sample sizes
	      */
	      if(QuantumDepth == sample_bits)
		{
		  /* Unity scale */
		  for (x = number_pixels; x != 0; --x)
		    {
		      ImportModulo8Quantum(unsigned_value,quantum_size,p);
		      SetOpacitySample(q,MaxRGB-unsigned_value);
		      q++;
		    }
		}
	      else if (QuantumDepth >  sample_bits)
		{
		  /* Scale up */
		  for (x = number_pixels; x != 0; --x)
		    {
		      ImportModulo8Quantum(unsigned_value,quantum_size,p);
		      SetOpacitySample(q,MaxRGB-unsigned_value*unsigned_scale);
		      q++;
		    }
		}
	      else
		{
		  /* Scale down */
		  for (x = number_pixels; x != 0; --x)
		    {
		      ImportModulo8Quantum(unsigned_value,quantum_size,p);
		      SetOpacitySample(q,MaxRGB-unsigned_value/unsigned_scale);
		      q++;
		    }
		}
	    }
	  else
	    {
	      /*
		Arbitrary sample size
	      */
	      BitStreamReadHandle
		stream;

	      MagickBitStreamInitializeRead(&stream,p);
	      if (QuantumDepth >=  sample_bits)
		{
		  /* Scale up */
		  for (x = number_pixels; x != 0; --x)
		    {
		      SetOpacitySample(q,MaxRGB-MagickBitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
		      q++;
		    }
		}
	      else
		{
		  /* Scale down */
		  for (x = number_pixels; x != 0; --x)
		    {
		      SetOpacitySample(q,MaxRGB-MagickBitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
		      q++;
		    }
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
		    ImportFloat16Quantum(endian,double_value,p);
		    double_value -= double_minvalue;
		    double_value *= double_scale;
		    SetOpacitySample(q,MaxRGB-RoundDoubleToQuantum(double_value));
		    q++;
		  }
		break;
	      }
	    case 24:
	      {
		for (x = number_pixels; x != 0; --x)
		  {
		    ImportFloat24Quantum(endian,double_value,p);
		    double_value -= double_minvalue;
		    double_value *= double_scale;
		    SetOpacitySample(q,MaxRGB-RoundDoubleToQuantum(double_value));
		    q++;
		  }
		break;
	      }
	    case 32:
	      {
		for (x = number_pixels; x != 0; --x)
		  {
		    ImportFloat32Quantum(endian,double_value,p);
		    double_value -= double_minvalue;
		    double_value *= double_scale;
		    SetOpacitySample(q,MaxRGB-RoundDoubleToQuantum(double_value));
		    q++;
		  }
		break;
	      }
	    case 64:
	      {
		for (x = number_pixels; x != 0; --x)
		  {
		    ImportFloat64Quantum(endian,double_value,p);
		    double_value -= double_minvalue;
		    double_value *= double_scale;
		    SetOpacitySample(q,MaxRGB-RoundDoubleToQuantum(double_value));
		    q++;
		  }
		break;
	      }
	    default:
	      break;
	    }
	}
    }

  if (import_info)
    {
      import_info->bytes_imported=p-source;
    }

  return MagickPass;
}

static MagickPassFail
ImportBlackQuantumType(const unsigned char *source,
		       PixelPacket* restrict q,
		       const unsigned long number_pixels,
		       const unsigned int quantum_size,
		       const QuantumSampleType sample_type,
		       const unsigned int unsigned_scale,
		       const double double_minvalue,
		       const double double_scale,
		       const EndianType endian,
		       ImportPixelAreaInfo *import_info)
{
  const unsigned char * restrict
    p;

  register unsigned long
    x;

  double
    double_value;

  register unsigned int
    unsigned_value;

  unsigned int
    sample_bits;

  sample_bits=quantum_size;
  p=source;

  if (sample_type == UnsignedQuantumSampleType)
    {
      switch (quantum_size)
	{
	case 8:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		SetBlackSample(q,ScaleCharToQuantum(*p++));
		q++;
	      }
	    break;
	  }
	case 16:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportUInt16Quantum(endian,unsigned_value,p);
		SetBlackSample(q,ScaleShortToQuantum(unsigned_value));
		q++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportUInt32Quantum(endian,unsigned_value,p);
		SetBlackSample(q,ScaleLongToQuantum(unsigned_value));
		q++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportUInt64Quantum(endian,unsigned_value,p);
		SetBlackSample(q,ScaleLongToQuantum(unsigned_value));
		q++;
	      }
	    break;
	  }
	default:
	  {
	    /*
	      Arbitrary sample size
	    */
	    BitStreamReadHandle
	      stream;
                  
	    MagickBitStreamInitializeRead(&stream,p);
	    if (QuantumDepth >=  sample_bits)
	      {
		/* Scale up */
		for (x = number_pixels; x != 0; --x)
		  {
		    SetBlackSample(q,MagickBitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
		    q++;
		  }
	      }
	    else
	      {
		/* Scale down */
		for (x = number_pixels; x != 0; --x)
		  {
		    SetBlackSample(q,MagickBitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
		    q++;
		  }
	      }
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
		ImportFloat16Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetBlackSample(q,RoundDoubleToQuantum(double_value));
		q++;
	      }
	    break;
	  }
	case 24:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportFloat24Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetBlackSample(q,RoundDoubleToQuantum(double_value));
		q++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportFloat32Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetBlackSample(q,RoundDoubleToQuantum(double_value));
		q++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportFloat64Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetBlackSample(q,RoundDoubleToQuantum(double_value));
		q++;
	      }
	    break;
	  }
	default:
	  break;
	}
    }

  if (import_info)
    {
      import_info->bytes_imported=p-source;
    }

  return MagickPass;
}

static MagickPassFail
ImportRGBQuantumType(const unsigned char *source,
		     PixelPacket* restrict q,
		     const unsigned long number_pixels,
		     const unsigned int quantum_size,
		     const QuantumSampleType sample_type,
		     const unsigned int unsigned_scale,
		     const double double_minvalue,
		     const double double_scale,
		     const EndianType endian,
		     ImportPixelAreaInfo *import_info)
{
  const unsigned char * restrict
    p;

  register unsigned long
    x;

  double
    double_value;

  register unsigned int
    unsigned_value;

  unsigned int
    sample_bits;

  sample_bits=quantum_size;
  p=source;

  if (sample_type == UnsignedQuantumSampleType)
    {
      switch (quantum_size)
	{
	case 8:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportUInt8Quantum(unsigned_value,p);
		SetRedSample(q,ScaleCharToQuantum(unsigned_value));
		ImportUInt8Quantum(unsigned_value,p);
		SetGreenSample(q,ScaleCharToQuantum(unsigned_value));
		ImportUInt8Quantum(unsigned_value,p);
		SetBlueSample(q,ScaleCharToQuantum(unsigned_value));
		SetOpacitySample(q,OpaqueOpacity);
		q++;
	      }
	    break;
	  }
	case 16:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportUInt16Quantum(endian,unsigned_value,p);
		SetRedSample(q,ScaleShortToQuantum(unsigned_value));
		ImportUInt16Quantum(endian,unsigned_value,p);
		SetGreenSample(q,ScaleShortToQuantum(unsigned_value));
		ImportUInt16Quantum(endian,unsigned_value,p);
		SetBlueSample(q,ScaleShortToQuantum(unsigned_value));
		SetOpacitySample(q,OpaqueOpacity);
		q++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportUInt32Quantum(endian,unsigned_value,p);
		SetRedSample(q,ScaleLongToQuantum(unsigned_value));
		ImportUInt32Quantum(endian,unsigned_value,p);
		SetGreenSample(q,ScaleLongToQuantum(unsigned_value));
		ImportUInt32Quantum(endian,unsigned_value,p);
		SetBlueSample(q,ScaleLongToQuantum(unsigned_value));
		SetOpacitySample(q,OpaqueOpacity);
		q++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportUInt64Quantum(endian,unsigned_value,p);
		SetRedSample(q,ScaleLongToQuantum(unsigned_value));
		ImportUInt64Quantum(endian,unsigned_value,p);
		SetGreenSample(q,ScaleLongToQuantum(unsigned_value));
		ImportUInt64Quantum(endian,unsigned_value,p);
		SetBlueSample(q,ScaleLongToQuantum(unsigned_value));
		SetOpacitySample(q,OpaqueOpacity);
		q++;
	      }
	    break;
	  }
	default:
	  {
	    /*
	      Arbitrary sample size
	    */
	    BitStreamReadHandle
	      stream;
                  
	    MagickBitStreamInitializeRead(&stream,p);
	    if (QuantumDepth >=  sample_bits)
	      {
		/* Scale up */
		for (x = number_pixels; x != 0; --x)
		  {
		    SetRedSample(q,MagickBitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
		    SetGreenSample(q,MagickBitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
		    SetBlueSample(q,MagickBitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
		    SetOpacitySample(q,OpaqueOpacity);
		    q++;
		  }
	      }
	    else if (QuantumDepth <  sample_bits)
	      {
		/* Scale down */
		for (x = number_pixels; x != 0; --x)
		  {
		    SetRedSample(q,MagickBitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
		    SetGreenSample(q,MagickBitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
		    SetBlueSample(q,MagickBitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
		    SetOpacitySample(q,OpaqueOpacity);
		    q++;
		  }
	      }
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
		ImportFloat16Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetRedSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat16Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetGreenSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat16Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetBlueSample(q,RoundDoubleToQuantum(double_value));
		SetOpacitySample(q,OpaqueOpacity);
		q++;
	      }
	    break;
	  }
	case 24:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportFloat24Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetRedSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat24Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetGreenSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat24Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetBlueSample(q,RoundDoubleToQuantum(double_value));
		SetOpacitySample(q,OpaqueOpacity);
		q++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportFloat32Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetRedSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat32Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetGreenSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat32Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetBlueSample(q,RoundDoubleToQuantum(double_value));
		SetOpacitySample(q,OpaqueOpacity);
		q++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportFloat64Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetRedSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat64Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetGreenSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat64Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetBlueSample(q,RoundDoubleToQuantum(double_value));
		SetOpacitySample(q,OpaqueOpacity);
		q++;
	      }
	    break;
	  }
	default:
	  break;
	}
    }

  if (import_info)
    {
      import_info->bytes_imported=p-source;
    }

  return MagickPass;
}

static MagickPassFail
ImportRGBAQuantumType(const unsigned char *source,
		      PixelPacket* restrict q,
		      const unsigned long number_pixels,
		      const unsigned int quantum_size,
		      const QuantumSampleType sample_type,
		      const unsigned int unsigned_scale,
		      const double double_minvalue,
		      const double double_scale,
		      const EndianType endian,
		      ImportPixelAreaInfo *import_info)
{
  const unsigned char * restrict
    p;

  register unsigned long
    x;

  double
    double_value;

  register unsigned int
    unsigned_value;

  unsigned int
    sample_bits;

  sample_bits=quantum_size;
  p=source;

  if (sample_type == UnsignedQuantumSampleType)
    {
      switch (quantum_size)
	{
	case 8:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		SetRedSample(q,ScaleCharToQuantum(*p++));
		SetGreenSample(q,ScaleCharToQuantum(*p++));
		SetBlueSample(q,ScaleCharToQuantum(*p++));
		SetOpacitySample(q,MaxRGB-ScaleCharToQuantum(*p++));
		q++;
	      }
	    break;
	  }
	case 16:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportUInt16Quantum(endian,unsigned_value,p);
		SetRedSample(q,ScaleShortToQuantum(unsigned_value));
		ImportUInt16Quantum(endian,unsigned_value,p);
		SetGreenSample(q,ScaleShortToQuantum(unsigned_value));
		ImportUInt16Quantum(endian,unsigned_value,p);
		SetBlueSample(q,ScaleShortToQuantum(unsigned_value));
		ImportUInt16Quantum(endian,unsigned_value,p);
		SetOpacitySample(q,MaxRGB-ScaleShortToQuantum(unsigned_value));
		q++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportUInt32Quantum(endian,unsigned_value,p);
		SetRedSample(q,ScaleLongToQuantum(unsigned_value));
		ImportUInt32Quantum(endian,unsigned_value,p);
		SetGreenSample(q,ScaleLongToQuantum(unsigned_value));
		ImportUInt32Quantum(endian,unsigned_value,p);
		SetBlueSample(q,ScaleLongToQuantum(unsigned_value));
		ImportUInt32Quantum(endian,unsigned_value,p);
		SetOpacitySample(q,MaxRGB-ScaleLongToQuantum(unsigned_value));
		q++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportUInt64Quantum(endian,unsigned_value,p);
		SetRedSample(q,ScaleLongToQuantum(unsigned_value));
		ImportUInt64Quantum(endian,unsigned_value,p);
		SetGreenSample(q,ScaleLongToQuantum(unsigned_value));
		ImportUInt64Quantum(endian,unsigned_value,p);
		SetBlueSample(q,ScaleLongToQuantum(unsigned_value));
		ImportUInt64Quantum(endian,unsigned_value,p);
		SetOpacitySample(q,MaxRGB-ScaleLongToQuantum(unsigned_value));
		q++;
	      }
	    break;
	  }
	default:
	  {
	    /*
	      Arbitrary sample size
	    */
	    BitStreamReadHandle
	      stream;
                  
	    MagickBitStreamInitializeRead(&stream,p);
	    if (QuantumDepth >=  sample_bits)
	      {
		/* Scale up */
		for (x = number_pixels; x != 0; --x)
		  {
		    SetRedSample(q,MagickBitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
		    SetGreenSample(q,MagickBitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
		    SetBlueSample(q,MagickBitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
		    SetOpacitySample(q,MaxRGB-MagickBitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
		    q++;
		  }
	      }
	    else if (QuantumDepth <  sample_bits)
	      {
		/* Scale down */
		for (x = number_pixels; x != 0; --x)
		  {
		    SetRedSample(q,MagickBitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
		    SetGreenSample(q,MagickBitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
		    SetBlueSample(q,MagickBitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
		    SetOpacitySample(q,MaxRGB-MagickBitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
		    q++;
		  }
	      }
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
		ImportFloat16Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetRedSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat16Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetGreenSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat16Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetBlueSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat16Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetOpacitySample(q,MaxRGB-RoundDoubleToQuantum(double_value));
		q++;
	      }
	    break;
	  }
	case 24:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportFloat24Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetRedSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat24Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetGreenSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat24Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetBlueSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat24Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetOpacitySample(q,MaxRGB-RoundDoubleToQuantum(double_value));
		q++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportFloat32Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetRedSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat32Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetGreenSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat32Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetBlueSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat32Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetOpacitySample(q,MaxRGB-RoundDoubleToQuantum(double_value));
		q++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportFloat64Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetRedSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat64Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetGreenSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat64Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetBlueSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat64Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetOpacitySample(q,MaxRGB-RoundDoubleToQuantum(double_value));
		q++;
	      }
	    break;
	  }
	default:
	  break;
	}
    }

  if (import_info)
    {
      import_info->bytes_imported=p-source;
    }

  return MagickPass;
}

static MagickPassFail
ImportCMYKQuantumType(const unsigned char *source,
		      PixelPacket* restrict q,
		      IndexPacket * restrict indexes,
		      const unsigned long number_pixels,
		      const unsigned int quantum_size,
		      const QuantumSampleType sample_type,
		      const unsigned int unsigned_scale,
		      const double double_minvalue,
		      const double double_scale,
		      const EndianType endian,
		      ImportPixelAreaInfo *import_info)
{
  const unsigned char * restrict
    p;

  register unsigned long
    x;

  double
    double_value;

  register unsigned int
    unsigned_value;

  unsigned int
    sample_bits;

  sample_bits=quantum_size;
  p=source;

  if (sample_type == UnsignedQuantumSampleType)
    {
      switch (quantum_size)
	{
	case 8:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		SetCyanSample(q,ScaleCharToQuantum(*p++));
		SetMagentaSample(q,ScaleCharToQuantum(*p++));
		SetYellowSample(q,ScaleCharToQuantum(*p++));
		SetBlackSample(q,ScaleCharToQuantum(*p++));
		q++;
	      }
	    break;
	  }
	case 16:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportUInt16Quantum(endian,unsigned_value,p);
		SetCyanSample(q,ScaleShortToQuantum(unsigned_value));
		ImportUInt16Quantum(endian,unsigned_value,p);
		SetMagentaSample(q,ScaleShortToQuantum(unsigned_value));
		ImportUInt16Quantum(endian,unsigned_value,p);
		SetYellowSample(q,ScaleShortToQuantum(unsigned_value));
		ImportUInt16Quantum(endian,unsigned_value,p);
		SetBlackSample(q,unsigned_value);
		q++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportUInt32Quantum(endian,unsigned_value,p);
		SetCyanSample(q,ScaleLongToQuantum(unsigned_value));
		ImportUInt32Quantum(endian,unsigned_value,p);
		SetMagentaSample(q,ScaleLongToQuantum(unsigned_value));
		ImportUInt32Quantum(endian,unsigned_value,p);
		SetYellowSample(q,ScaleLongToQuantum(unsigned_value));
		ImportUInt32Quantum(endian,unsigned_value,p);
		SetBlackSample(q,unsigned_value);
		q++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportUInt64Quantum(endian,unsigned_value,p);
		SetCyanSample(q,ScaleLongToQuantum(unsigned_value));
		ImportUInt64Quantum(endian,unsigned_value,p);
		SetMagentaSample(q,ScaleLongToQuantum(unsigned_value));
		ImportUInt64Quantum(endian,unsigned_value,p);
		SetYellowSample(q,ScaleLongToQuantum(unsigned_value));
		ImportUInt64Quantum(endian,unsigned_value,p);
		SetBlackSample(q,unsigned_value);
		q++;
	      }
	    break;
	  }
	default:
	  {
	    /*
	      Arbitrary sample size
	    */
	    BitStreamReadHandle
	      stream;
                  
	    MagickBitStreamInitializeRead(&stream,p);
	    if (QuantumDepth >=  sample_bits)
	      {
		/* Scale up */
		for (x = number_pixels; x != 0; --x)
		  {
		    SetCyanSample(q,MagickBitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
		    SetMagentaSample(q,MagickBitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
		    SetYellowSample(q,MagickBitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
		    SetBlackSample(q,MagickBitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
		    *indexes++=OpaqueOpacity;
		    q++;
		  }
	      }
	    else if (QuantumDepth <  sample_bits)
	      {
		/* Scale down */
		for (x = number_pixels; x != 0; --x)
		  {
		    SetCyanSample(q,MagickBitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
		    SetMagentaSample(q,MagickBitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
		    SetYellowSample(q,MagickBitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
		    SetBlackSample(q,MagickBitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
		    *indexes++=OpaqueOpacity;
		    q++;
		  }
	      }
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
		ImportFloat16Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetCyanSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat16Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetMagentaSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat16Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetYellowSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat16Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetBlackSample(q,RoundDoubleToQuantum(double_value));
		*indexes++=OpaqueOpacity;
		q++;
	      }
	    break;
	  }
	case 24:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportFloat24Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetCyanSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat24Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetMagentaSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat24Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetYellowSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat24Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetBlackSample(q,RoundDoubleToQuantum(double_value));
		*indexes++=OpaqueOpacity;
		q++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportFloat32Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetCyanSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat32Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetMagentaSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat32Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetYellowSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat32Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetBlackSample(q,RoundDoubleToQuantum(double_value));
		*indexes++=OpaqueOpacity;
		q++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportFloat64Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetCyanSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat64Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetMagentaSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat64Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetYellowSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat64Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetBlackSample(q,RoundDoubleToQuantum(double_value));
		*indexes++=OpaqueOpacity;
		q++;
	      }
	    break;
	  }
	default:
	  break;
	}
    }

  if (import_info)
    {
      import_info->bytes_imported=p-source;
    }

  return MagickPass;
}

static MagickPassFail
ImportCMYKAQuantumType(const unsigned char *source,
		       PixelPacket* restrict q,
		       IndexPacket * restrict indexes,
		       const unsigned long number_pixels,
		       const unsigned int quantum_size,
		       const QuantumSampleType sample_type,
		       const unsigned int unsigned_scale,
		       const double double_minvalue,
		       const double double_scale,
		       const EndianType endian,
		       ImportPixelAreaInfo *import_info)
{
  const unsigned char * restrict
    p;

  register unsigned long
    x;

  double
    double_value;

  register unsigned int
    unsigned_value;

  unsigned int
    sample_bits;

  sample_bits=quantum_size;
  p=source;

  if (sample_type == UnsignedQuantumSampleType)
    {
      switch (quantum_size)
	{
	case 8:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		SetCyanSample(q,ScaleCharToQuantum(*p++));
		SetMagentaSample(q,ScaleCharToQuantum(*p++));
		SetYellowSample(q,ScaleCharToQuantum(*p++));
		SetBlackSample(q,ScaleCharToQuantum(*p++));
		*indexes++=(IndexPacket) MaxRGB-ScaleCharToQuantum(*p++);
		q++;
	      }
	    break;
	  }
	case 16:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportUInt16Quantum(endian,unsigned_value,p);
		SetCyanSample(q,ScaleShortToQuantum(unsigned_value));
		ImportUInt16Quantum(endian,unsigned_value,p);
		SetMagentaSample(q,ScaleShortToQuantum(unsigned_value));
		ImportUInt16Quantum(endian,unsigned_value,p);
		SetYellowSample(q,ScaleShortToQuantum(unsigned_value));
		ImportUInt16Quantum(endian,unsigned_value,p);
		SetBlackSample(q,ScaleShortToQuantum(unsigned_value));
		ImportUInt16Quantum(endian,unsigned_value,p);
		*indexes++=(IndexPacket) MaxRGB-ScaleShortToQuantum(unsigned_value);
		q++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportUInt32Quantum(endian,unsigned_value,p);
		SetCyanSample(q,ScaleLongToQuantum(unsigned_value));
		ImportUInt32Quantum(endian,unsigned_value,p);
		SetMagentaSample(q,ScaleLongToQuantum(unsigned_value));
		ImportUInt32Quantum(endian,unsigned_value,p);
		SetYellowSample(q,ScaleLongToQuantum(unsigned_value));
		ImportUInt32Quantum(endian,unsigned_value,p);
		SetBlackSample(q,ScaleLongToQuantum(unsigned_value));
		ImportUInt32Quantum(endian,unsigned_value,p);
		*indexes++=(IndexPacket) MaxRGB-ScaleLongToQuantum(unsigned_value);
		q++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportUInt64Quantum(endian,unsigned_value,p);
		SetCyanSample(q,ScaleLongToQuantum(unsigned_value));
		ImportUInt64Quantum(endian,unsigned_value,p);
		SetMagentaSample(q,ScaleLongToQuantum(unsigned_value));
		ImportUInt64Quantum(endian,unsigned_value,p);
		SetYellowSample(q,ScaleLongToQuantum(unsigned_value));
		ImportUInt64Quantum(endian,unsigned_value,p);
		SetBlackSample(q,ScaleLongToQuantum(unsigned_value));
		ImportUInt64Quantum(endian,unsigned_value,p);
		*indexes++=(IndexPacket) MaxRGB-ScaleLongToQuantum(unsigned_value);
		q++;
	      }
	    break;
	  }
	default:
	  {
	    /*
	      Arbitrary sample size
	    */
	    BitStreamReadHandle
	      stream;
                  
	    MagickBitStreamInitializeRead(&stream,p);
	    if (QuantumDepth >=  sample_bits)
	      {
		/* Scale up */
		for (x = number_pixels; x != 0; --x)
		  {
		    SetCyanSample(q,MagickBitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
		    SetMagentaSample(q,MagickBitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
		    SetYellowSample(q,MagickBitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
		    SetBlackSample(q,MagickBitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
		    *indexes++=(IndexPacket) MaxRGB-MagickBitStreamMSBRead(&stream,quantum_size)*unsigned_scale;
		    q++;
		  }
	      }
	    else if (QuantumDepth <  sample_bits)
	      {
		/* Scale down */
		for (x = number_pixels; x != 0; --x)
		  {
		    SetCyanSample(q,MagickBitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
		    SetMagentaSample(q,MagickBitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
		    SetYellowSample(q,MagickBitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
		    SetBlackSample(q,MagickBitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
		    *indexes++=(IndexPacket) MaxRGB-MagickBitStreamMSBRead(&stream,quantum_size)/unsigned_scale;
		    q++;
		  }
	      }
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
		ImportFloat16Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetCyanSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat16Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetMagentaSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat16Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetYellowSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat16Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetBlackSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat16Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		*indexes++=(IndexPacket) MaxRGB-RoundDoubleToQuantum(double_value);
		q++;
	      }
	    break;
	  }
	case 24:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportFloat24Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetCyanSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat24Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetMagentaSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat24Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetYellowSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat24Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetBlackSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat24Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		*indexes++=(IndexPacket) MaxRGB-RoundDoubleToQuantum(double_value);
		q++;
	      }
	    break;
	  }
	case 32:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportFloat32Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetCyanSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat32Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetMagentaSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat32Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetYellowSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat32Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetBlackSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat32Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		*indexes++=(IndexPacket) MaxRGB-RoundDoubleToQuantum(double_value);
		q++;
	      }
	    break;
	  }
	case 64:
	  {
	    for (x = number_pixels; x != 0; --x)
	      {
		ImportFloat64Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetCyanSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat64Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetMagentaSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat64Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetYellowSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat64Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		SetBlackSample(q,RoundDoubleToQuantum(double_value));
		ImportFloat64Quantum(endian,double_value,p);
		double_value -= double_minvalue;
		double_value *= double_scale;
		*indexes++=(IndexPacket) MaxRGB-RoundDoubleToQuantum(double_value);
		q++;
	      }
	    break;
	  }
	default:
	  break;
	}
    }

  if (import_info)
    {
      import_info->bytes_imported=p-source;
    }

  return MagickPass;
}

static MagickPassFail
ImportCIEXYZQuantumType(const unsigned char *source,
			PixelPacket* restrict q,
			const unsigned long number_pixels,
			const unsigned int quantum_size,
			const QuantumSampleType sample_type,
			const EndianType endian,
			ImportPixelAreaInfo *import_info)
{
  const unsigned char * restrict
    p;

  register unsigned long
    x;

  p=source;

  if (sample_type == FloatQuantumSampleType)
    {
      double
	red,
	green,
	blue,
	x_sample,
	y_sample,
	z_sample;

      for (x = number_pixels; x != 0; --x)
	{
	  switch (quantum_size)
	    {
	    default:
	    case 32:
	      {
		ImportFloat32Quantum(endian,x_sample,p);
		ImportFloat32Quantum(endian,y_sample,p);
		ImportFloat32Quantum(endian,z_sample,p);
		break;
	      }
	    case 64:
	      {
		ImportFloat64Quantum(endian,x_sample,p);
		ImportFloat64Quantum(endian,y_sample,p);
		ImportFloat64Quantum(endian,z_sample,p);
		break;
	      }
	    }

	  /* Assume CCIR-709 primaries */
	  red   = 2.690*x_sample  + -1.276*y_sample + -0.414*z_sample;
	  green = -1.022*x_sample +  1.978*y_sample +  0.044*z_sample;
	  blue  = 0.061*x_sample  + -0.224*y_sample +  1.163*z_sample;

	  /* assume 2.0 gamma for speed */
	  SetRedSample(q,(Quantum) ((red <= 0.0) ? 0.0 : (red >= 1.0) ? MaxRGB :
				    ((MaxRGB * sqrt(red))+0.5)));
	  SetGreenSample(q,(Quantum) ((green <= 0.0) ? 0.0 : (green >= 1.0) ? MaxRGB :
				      ((MaxRGB * sqrt(green))+0.5)));
	  SetBlueSample(q,(Quantum) ((blue <= 0.0) ? 0.0 : (blue >= 1.0) ? MaxRGB :
				     ((MaxRGB * sqrt(blue))+0.5)));
	  SetOpacitySample(q,OpaqueOpacity);
	  q++;
	}
    }

  if (import_info)
    {
      import_info->bytes_imported=p-source;
    }

  return MagickPass;
}

static MagickPassFail
ImportCIEYQuantumType(const unsigned char *source,
		      PixelPacket* restrict q,
		      const unsigned long number_pixels,
		      const unsigned int quantum_size,
		      const QuantumSampleType sample_type,
		      const EndianType endian,
		      ImportPixelAreaInfo *import_info)
{
  const unsigned char * restrict
    p;

  register unsigned long
    x;

  double
    double_value;

  p=source;

  if (sample_type == FloatQuantumSampleType)
    {
      for (x = number_pixels; x != 0; --x)
	{
	  switch (quantum_size)
	    {
	    default:
	    case 32:
	      {
		ImportFloat32Quantum(endian,double_value,p);
		break;
	      }
	    case 64:
	      {
		ImportFloat64Quantum(endian,double_value,p);
		break;
	      }
	    }
	  /* assume 2.0 gamma for speed */
	  SetGraySample(q,(Quantum) ((double_value <= 0.0) ? 0.0 :
				     (double_value >= 1.0) ? MaxRGB :
				     ((MaxRGB * sqrt(double_value))+0.5)));
	  q++;
	}
    }

  if (import_info)
    {
      import_info->bytes_imported=p-source;
    }

  return MagickPass;
}

MagickExport MagickPassFail
ImportViewPixelArea(ViewInfo *view,
		    const QuantumType quantum_type,
		    const unsigned int quantum_size,
		    const unsigned char *source,
		    const ImportPixelAreaOptions *options,
		    ImportPixelAreaInfo *import_info)
{
  Image
    *image;

  unsigned int
    unsigned_scale = 1U;

  IndexPacket
    * restrict indexes;

  PixelPacket
    * restrict q;

  MagickBool
    grayscale_miniswhite = MagickFalse;

  QuantumSampleType
    sample_type = UnsignedQuantumSampleType;

  unsigned int
    unsigned_maxvalue=MaxRGB,
    sample_bits;

  unsigned long
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
  assert(source != (const unsigned char *) NULL);
  assert(((quantum_size > 0U) && (quantum_size <= 32U)) || (quantum_size == 64U));
  assert((options == (const ImportPixelAreaOptions *) NULL) ||
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

  if (import_info)
    {
      import_info->bytes_imported=0;
    }

  /* printf("quantum_type=%d  quantum_size=%u\n",(int) quantum_type, quantum_size); */

  /* Maximum value which may be represented by a sample */
  unsigned_maxvalue=MaxValueGivenBits(sample_bits);
  double_scale=(double) MaxRGB/(double_maxvalue-double_minvalue);
  if ((sample_type != FloatQuantumSampleType) && (sample_bits <= 32U))
    {
      if (QuantumDepth == sample_bits)
        {
        }
      else if (QuantumDepth > sample_bits)
        {
          /* Multiply to scale up */
          unsigned_scale=(MaxRGB / (MaxRGB >> (QuantumDepth-sample_bits)));
        }
      else if (QuantumDepth < sample_bits)
        {
          /* Divide to scale down */
          unsigned_scale=(unsigned_maxvalue/MaxRGB);
        }
    }

  image=GetCacheViewImage(view);
  number_pixels=(long) GetCacheViewArea(view);
  q=AccessCacheViewPixels(view);
  indexes=GetCacheViewIndexes(view);
  switch (quantum_type)
    {
    case UndefinedQuantum:
      {
	status=MagickFail;
	break;
      }
    case IndexQuantum:
      {
	status=ImportIndexQuantumType(source,q,indexes,number_pixels,quantum_size,
				      sample_type,unsigned_maxvalue,endian,image,
				      import_info);
        break;
      }
    case IndexAlphaQuantum:
      {
	status=ImportIndexAlphaQuantumType(source,q,indexes,number_pixels,
					   quantum_size,sample_type,unsigned_scale,
					   endian,
					   image,
					   import_info);
        break;
      }
    case GrayQuantum:
      {
	status=ImportGrayQuantumType(source,q,indexes,number_pixels,quantum_size,
				     sample_type,unsigned_scale,unsigned_maxvalue,
				     grayscale_miniswhite,double_minvalue,
				     double_scale,endian,image,import_info);
        break;
      }
    case GrayAlphaQuantum:
      {
	status=ImportGrayAlphaQuantumType(source,q,indexes,number_pixels,quantum_size,
					  sample_type,unsigned_scale,unsigned_maxvalue,
					  grayscale_miniswhite,double_minvalue,double_scale,
					  endian,image,import_info);
        break;
      }
    case RedQuantum:
    case CyanQuantum:
      {
	status=ImportRedQuantumType(source,q,number_pixels,quantum_size,sample_type,
				    unsigned_scale,double_minvalue,double_scale,endian,
				    import_info);

        break;
      }
    case GreenQuantum:
    case MagentaQuantum:
      {
	status=ImportGreenQuantumType(source,q,number_pixels,quantum_size,sample_type,
				      unsigned_scale,double_minvalue,double_scale,endian,
				      import_info);
        break;
      }
    case BlueQuantum:
    case YellowQuantum:
      {
	status=ImportBlueQuantumType(source,q,number_pixels,quantum_size,sample_type,
				     unsigned_scale,double_minvalue,double_scale,endian,
				     import_info);
        break;
      }
    case AlphaQuantum:
      {
	status=ImportAlphaQuantumType(source,q,indexes,number_pixels,quantum_size,
				      sample_type,unsigned_scale,double_minvalue,
				      double_scale,endian,image,import_info);
        break;
      }
    case BlackQuantum:
      {
	status=ImportBlackQuantumType(source,q,number_pixels,quantum_size,sample_type,
				      unsigned_scale,double_minvalue,double_scale,endian,
				      import_info);
        break;
      }
    case RGBQuantum:
      {
	status=ImportRGBQuantumType(source,q,number_pixels,quantum_size,sample_type,
				    unsigned_scale,double_minvalue,double_scale,endian,
				    import_info);
        break;
      }
    case RGBAQuantum:
      {
	status=ImportRGBAQuantumType(source,q,number_pixels,quantum_size,sample_type,
				     unsigned_scale,double_minvalue,double_scale,endian,
				     import_info);
        break;
      }
    case CMYKQuantum:
      {
	status=ImportCMYKQuantumType(source,q,indexes,number_pixels,quantum_size,
				     sample_type,unsigned_scale,double_minvalue,
				     double_scale,endian,import_info);
        break;
      }
    case CMYKAQuantum:
      {
	status=ImportCMYKAQuantumType(source,q,indexes,number_pixels,quantum_size,
				      sample_type,unsigned_scale,double_minvalue,
				      double_scale,endian,import_info);
        break;
      }
    case CIEXYZQuantum:
      {
	status=ImportCIEXYZQuantumType(source,q,number_pixels,quantum_size,sample_type,
				       endian,import_info);
        break;
      }
    case CIEYQuantum:
      {
	status=ImportCIEYQuantumType(source,q,number_pixels,quantum_size,sample_type,
				     endian,import_info);
        break;
      }
    }

  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I m p o r t P i x e l A r e a O p t i o n s I n i t                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ImportPixelAreaOptionsInit() initializes the options structure which is
%  optionally passed to ImportPixelArea()
%
%  The format of the ImportPixelAreaOptionsInit method is:
%
%      void ImportPixelAreaOptionsInit(ImportPixelAreaOptions *options)
%
%  A description of each parameter follows:
%
%    o options: Options structure to initialize.
%
*/
MagickExport void ImportPixelAreaOptionsInit(ImportPixelAreaOptions *options)
{
  assert(options != (ImportPixelAreaOptions *) NULL);
  (void) memset((void *) options, 0, sizeof(ImportPixelAreaOptions));
  options->sample_type=UnsignedQuantumSampleType;
  options->double_minvalue=0.0;
  options->double_maxvalue=1.0;
  options->grayscale_miniswhite=MagickFalse;
  options->endian=MSBEndian;
  options->signature=MagickSignature;
}
