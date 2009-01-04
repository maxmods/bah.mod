/*
% Copyright (C) 2003 - 2008 GraphicsMagick Group
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
%     CCCC   OOO   N   N  SSSSS  TTTTT  IIIII  TTTTT  U   U  TTTTT  EEEEE     %
%    C      O   O  NN  N  SS       T      I      T    U   U    T    E         %
%    C      O   O  N N N  ESSS     T      I      T    U   U    T    EEE       %
%    C      O   O  N  NN     SS    T      I      T    U   U    T    E         %
%     CCCC   OOO   N   N  SSSSS    T    IIIII    T     UUU     T    EEEEE     %
%                                                                             %
%                                                                             %
%                      Methods to Constitute an Image                         %
%                                                                             %
%                                                                             %
%                             Software Design                                 %
%                               John Cristy                                   %
%                               October 1998                                  %
%                                                                             %
%                           Re-implemented By                                 %
%                            Bob Friesenhahn                                  %
%                               2003-2008                                     %
%                                                                             %
%                         Small Float Support By                              %
%                             Richard Nolde                                   %
%                             December, 2008                                  %
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
#include "magick/bit_stream.h"
#include "magick/blob.h"
#include "magick/color.h"
#include "magick/constitute.h"
#include "magick/delegate.h"
#include "magick/enum_strings.h"
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/pixel_cache.h"
#include "magick/semaphore.h"
#include "magick/tempfile.h"
#include "magick/utility.h"

/*
  Type definitions
*/
typedef enum
{
  BlueMapQuanum,
  GreenMapQuantum,
  IntensityMapQuantum,
  OpacityInvertedMapQuantum,
  PadMapQuantum,
  RedMapQuantum,
  OpacityMapQuantum
} MapQuantumType;

typedef enum {
  UndefinedDispatchType,
  BGRDispatchType,
  BGRODispatchType,
  BGRPDispatchType,
  RGBDispatchType,
  RGBODispatchType,
  IDispatchType
} DispatchType;

static const PixelPacket BlackPixel = {0, 0, 0, OpaqueOpacity};

static const PixelPacket WhitePixel = {MaxRGB, MaxRGB, MaxRGB, OpaqueOpacity};

#define RANGE_LIMITED 0
#define STRICT_IEEE   1
typedef unsigned char  fp_16bits[2];
typedef unsigned char  fp_24bits[3];

static SemaphoreInfo
  *constitute_semaphore = (SemaphoreInfo *) NULL;

/*
  Forward declarations.
*/
static Image
  *ReadImages(const ImageInfo *,ExceptionInfo *);


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
#define ExportFloat16Quantum(endian,q,quantum)  \
  {                                             \
    float float_val;                            \
    unsigned char c[2];                         \
                                                \
    float_val=(float) (quantum);                \
    (void) convert_fp32_to_fp16(&float_val,     \
                                (fp_16bits *)c, \
                                RANGE_LIMITED); \
    if (MyEndianType == endian)                 \
      {                                         \
        *q++=c[0];                              \
        *q++=c[1];                              \
      }                                         \
    else                                        \
      {                                         \
        *q++=c[1];                              \
        *q++=c[0];                              \
      }                                         \
  }
#define ExportFloat24Quantum(endian,q,quantum)  \
  {                                             \
    float float_val;                            \
    unsigned char c[3];                         \
                                                \
    float_val=(float) (quantum);                \
    (void) convert_fp32_to_fp24(&float_val,     \
                                (fp_24bits *)c, \
                                RANGE_LIMITED); \
    if (MyEndianType == endian)                 \
      {                                         \
        *q++=c[0];                              \
        *q++=c[1];                              \
        *q++=c[2];                              \
      }                                         \
    else                                        \
      {                                         \
        *q++=c[2];                              \
        *q++=c[1];                              \
        *q++=c[0];                              \
      }                                         \
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
#define ImportFloat16Quantum(endian,value,p)          \
  {                                                   \
    float float_value;                                \
    unsigned char c[2];                               \
    if (MyEndianType == endian)                       \
      {                                               \
        c[0]=*p++;                                    \
        c[1]=*p++;                                    \
      }                                               \
    else                                              \
      {                                               \
        c[1]=*p++;                                    \
        c[0]=*p++;                                    \
      }                                               \
    (void) convert_fp16_to_fp32((const fp_16bits *)c, \
                                &float_value);        \
    value=float_value;                                \
  }
#define ImportFloat24Quantum(endian,value,p)            \
  {                                                     \
    float float_value;                                  \
    unsigned char c[3];                                 \
    if (MyEndianType == endian)                         \
      {                                                 \
        c[0]=*p++;                                      \
        c[1]=*p++;                                      \
        c[2]=*p++;                                      \
      }                                                 \
    else                                                \
      {                                                 \
        c[2]=*p++;                                      \
        c[1]=*p++;                                      \
        c[0]=*p++;                                      \
      }                                                 \
    (void) convert_fp24_to_fp32((const fp_24bits *)c,   \
                                &float_value,           \
                                RANGE_LIMITED);         \
    value=float_value;                                  \
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

static int
convert_fp16_to_fp32 (const fp_16bits *fp16, float *fp32)
{
  unsigned char  sbit=0; /* sign bit */
  unsigned char  expt; /* exponent bits */
  unsigned char  m2, m1;  /* MSB to LSB of mantissa */
  unsigned char  new_expt;
  unsigned char  new_m3, new_m2, new_m1; 
  unsigned int   little_endian = 1;
  const unsigned char *src;
  unsigned char *dst;

#ifdef DEBUG32
  /* Debugging variables */
  int            i, j, bit;
  unsigned int   mant;
  double         test  = 0.0; 
  double         test2 = 0.0; 
  double         accum = 0.0;

  errno = 0;
#endif

  assert (sizeof(int) == 4);
  if ((fp16 == NULL) || (fp32 == NULL))
    {
      (void) fprintf (stderr, "Invalid src or destination pointers\n");
      return (1);
    }
  src = (unsigned char *)fp16;
  dst = (unsigned char *)fp32;
  new_expt = expt = 0;
  new_m3 = new_m2 = new_m1 = m2 = m1 = 0;

  little_endian = *((unsigned char *)&little_endian) & '1';
  if ((int)*fp16 != 0)
    {
      if (little_endian)
        {
          sbit =  *(src + 1) & 0x80;
          expt = (*(src + 1) & 0x7F) >> 2;
          m2 =    *(src + 1) & 0x03;
          m1 = *src;
        }
      else
        {
          sbit =  *src & 0x80;
          expt = (*src & 0x7F) >> 2;
          m2 =    *src & 0x03;
          m1 =  *(src + 1);
        }

      if (expt != 0)
        new_expt = expt - 15 + 127;
      new_m3  =  (m2 << 5) | (m1 & 0xF8) >> 3; 
      new_m2  =  (m1 & 7) << 5;
      new_m1  = 0;
    }
  if (little_endian)
    {
      *dst = new_m1;
      *(dst + 1) = new_m2;
      *(dst + 2) = ((new_expt & 1) << 7) | new_m3;
      *(dst + 3) = sbit | (new_expt >> 1);
    }
  else
    {
      *dst = sbit | (new_expt >> 1);
      *(dst + 1) = ((new_expt & 1) << 7) | new_m3;
      *(dst + 2) = new_m2;
      *(dst + 3) = new_m1;
    }

#ifdef DEBUG32
  /* Debugging code for display only */
  mant = ((unsigned int)new_m3 << 16) | ((unsigned int)new_m2 << 8) | (unsigned int)new_m1; 
  if (new_expt == 0)
    {
      test = 0.0;
      test2 = 0.0;
      accum = 0;
    }
  else
    {
      accum = 0.0;
      for (i = 22, j = 1; i >= 0; i--, j++)
        {
          bit = mant & ((unsigned int)1 << i);
          if (bit)
            accum += (1.0 / ((unsigned int)1 << j));
        }
      accum += 1.0;
      test = powf (2.0, 1.0 * (new_expt - 127));
      switch (errno)
        {
        case 0:     break;
        case EDOM: 
        case ERANGE: 
        default: perror ("Invalid value");
          break;
        }
      test2 = accum * test;
      if (sbit)
        test2 *= -1.0;
    }
  (void) printf ("              Sign bit: %u, Expt biased to %5u,  2^%-5d = %8.1f  *  %10.8f = %16.8f\n",
                 sbit > 0 ? 1 : 0, new_expt, new_expt > 0 ? new_expt - 127 : 0, test, accum, test2);
#endif
  return (0);
} /* end convertfp16_to_fp32 */

static int convert_fp32_to_fp16 (const float *fp32, fp_16bits *fp16, const int mode)
{
  int            i, bit, rbits;
  unsigned char  sbit = 0;   /* sign bit */
  unsigned char  expt = 0;   /* exponent bits */
  unsigned char  m3, m2, m1; /* MSB to LSB of mantissa */
  signed char    new_expt;
  unsigned short new_mant;
  unsigned short mant;
  unsigned int   little_endian = 1;
  unsigned char *src;
  unsigned char *dst;
  unsigned char *mp;

#ifdef DEBUG16
  int     j, k;
  double  test = 0.0; 
  double  test2 = 0.0; 
  double  accum = 0.0;
  double  roundup = 0.0;

  errno = 0;
#endif

  assert (sizeof(int) == 4);
  if ((fp32 == NULL) || (fp16 == NULL))
    {
      fprintf (stderr, "Invalid src or destination pointers\n");
      return (1);
    }
  little_endian = *((unsigned char *)&little_endian) & '1';

  src = (unsigned char *)fp32;
  dst = (unsigned char *)fp16;
  mp  = (unsigned char *)&mant;

  new_expt = expt = 0;
  mant = new_mant = 0;
  m2 = m1 = 0;
  rbits = 0;

  /* For zero, all bits except possibly sbit are zero */
  if (*fp32 == 0)
    *dst = 0;
  else
    {
      if (little_endian)
        {
          sbit =   *(src + 3) & 0x80;
          expt = ((*(src + 3) & 0x7F) << 1) | 
            ((*(src + 2) & 0x80) >> 7);
          m3  = (((*(src + 2) & 0x7F)) << 1) |
            ((*(src + 1) & 0x80) >> 7);
          m2  = (((*(src + 1) & 0x7F)) << 1)  |
            ((*src & 0x80) >> 7);
          m1  =  (*src & 0x7F) << 1;
        }
      else
        {
          sbit =   *src & 0x80;
          expt = ((*src & 0x7F) << 1) | 
            ((*(src + 1) & 0x80) >> 7);
          m3  = (((*(src + 1) & 0x7F)) << 1) |
            ((*(src + 2) & 0x80) >> 7);
          m2  = (((*(src + 2) & 0x7F)) << 1)  |
            ((*(src + 3) & 0x80) >> 7);
          m1  =  (*(src + 3) & 0x7F) << 1;
        }
  
      if (expt != 0)  /* Normal number */
        new_expt = expt - 127 + 15;

      if (new_expt <= 0)
        {
#ifdef DEBUG16
          fprintf (stderr, "Subnormal detected\n");
#endif
          if (mode == STRICT_IEEE)
            {
              /* NaN has all 1s in exponent plus 2 bits in Mantissa */
              new_expt = 31;
              new_mant = 513;
              errno = ERANGE;
              fprintf (stderr, 
                       "Overflow. %12.4f exceeded largest storable value. Result clippped\n",
                       *fp32);
              return (1);
            }
          else
            { /* Clamp to mimimum representable value for fp16 */
              new_expt = 0;
              new_mant = 1;
              mp = (unsigned char *)&new_mant;
            }
        }
      else
        {
          if (new_expt > 30)
            {
              if (mode == STRICT_IEEE)
                {
                  /* NaN has all 1s in exponent plus 2 bits in Mantissa */
                  new_expt = 31;
                  new_mant = 513;
                  errno = ERANGE;
                  fprintf (stderr, 
                           "Overflow. %12.4f exceeded largest storable value. Result clippped\n",
                           *fp32);
                  return (1);
                }
              else
                { /* Clamp to maximum rpresentable value for fp16 */
                  new_expt = 30;
                  new_mant = 1023;
                  mp = (unsigned char *)&new_mant;
                }
            }
          else
            {
              /* Take the MSB from the old mantissa and left justify them */
              mant = (m3 << 8) | m2;

              /* Check bits to the right of Unit in last signficant place.
               * Rounding of least significant retained bit falls to value
               * which will produce a zero in the LSB if the bounding values
               * are equidistant from the unrounded value.
               */
              rbits = mant & 0x3F;
              if (rbits >= 0x20)  /* Greater than or equal to 0.5 times LSB */
                {
                  if (rbits > 0x20) /* Rbits is greater than half of LSB */
                    {
                      /* Round up to next higher value of LSB */
                      for (i = 0; i < 6; i++)
                        {
                          bit = mant & (1 << (i + 6));
                          if (bit == 0)
                            {
                              new_mant = (mant | ((unsigned short)1 << (i + 6))) &
                                (0x3FFFF << (i + 6));
                              mp  = (unsigned char *)&new_mant;
                              break;
                            }
                        }
                    }
                  else    /* Rbits is exactly half of LSB */
                    {
                      if ((mant & 0x40)) /* LSB is one so we round up */
                        {
                          /* Round up to next higher value of LSB */
                          for (i = 0; i < 6; i++)
                            {
                              bit = mant & (1 << (i + 6));
                              if (bit == 0)
                                {
                                  new_mant = (mant | ((unsigned short)1 << (i + 6))) &
                                    (0x3FFFF << (i + 6));
                                  mp  = (unsigned char *)&new_mant;
                                  break;
                                }
                            }
                        }
                    }
                }
            }
        }
 
      /* Extract bits into contiguous positions in bytes */
      if (little_endian)
        {
          m2 =  (*(mp + 1) & 0xC0) >> 6;
          m1 = ((*(mp + 1) & 0x3F) << 2) |
            ((*mp & 0xC0) >> 6);
          *dst = m1;
          *(dst + 1) = sbit | ((new_expt & 0x1F) << 2) | m2;
        }
      else
        {
          m2 = (*mp & 0xC0) >> 6;
          m1 = ((*mp & 0x3F) << 2) |
            ((*(mp + 1) & 0xC0) >> 6);
          *dst = sbit | ((new_expt & 0x1F) << 2) | m2;
          *(dst + 1) = m1;
        }
    }

#ifdef DEBUG16
  /* Debugging code to display the result */
  printf ("%12.2f mant%s ", *fp32, (rbits & 0x20) ? "+" : "-");
  for (j = 0, k = 15; j < 16; j++, k--)
    {
      bit = mant & (1 << k);
      printf ("%d", bit ? 1 : 0);
      if (j == 9)
        printf (" ");
      if (bit && (j > 9))
        roundup += (1.0 / (double)(1 << (j - 9)));
    }
  if (new_mant == 0)
    {
      printf (" Fract: %8.6f  m2m1 ", roundup);
      for (j = 0, k = 1; j < 2; j++, k--)
        {
          bit = m2 & (1 << k);
          printf ("%d", bit ? 1 : 0);
        }
      for (j = 0, k = 7; j < 8; j++, k--)
        {
          bit = m1 & (1 << k);
          printf ("%d", bit ? 1 : 0);
        }
    }
  else
    {
      printf (" Fract: %8.6f  Rbits Mant ", roundup);
      for (j = 0, k = 9; j < 10; j++, k--)
        {
          bit = new_mant & (1 << k);
          printf ("%d", bit ? 1 : 0);
        }
    }
  printf (" Sbit + Exp ");
  if (little_endian)
    {
      for (i = 1; i >= 0; i--)
        {
          for (j = 0, k = 7; j < 8; j++, k--)
            {
              bit = *(dst + i) & (1 << k);
              printf ("%d", bit ? 1 : 0);
              if (i == 1 && j == 5)
                printf ("  Mant: ");
            }
        }
    }
  else  
    {
      for (i = 0; i < 2; i++)
        {
          for (j = 0, k = 7; j < 8; j++, k--)
            {
              bit = *(dst + i) & (1 << k);
              printf ("%d", bit ? 1 : 0);
              if (i == 0 && j == 5)
                printf ("  Mant: ");
            }
        }
    }
  printf ("\n");

  mant = ((unsigned short)m2 << 8) | (unsigned short)m1; 
  if (*fp32 == 0)
    {
      test = 0.0;
      test2 = 0.0;
      accum = 0;
    }
  else
    {
      accum = 0.0;
      for (i = 9, j = 1; i >= 0; i--, j++)
        {
          bit = mant & ((unsigned int)1 << i);
          if (bit)
            accum += (1.0 / ((unsigned int)1 << j));
        }
      if (new_expt != 0)
        accum += 1.0;
      test = pow (2.0, 1.0 * (new_expt - 15));
      switch (errno)
        {
        case 0:     break;
        case EDOM: 
        case ERANGE: 
        default: perror ("Invalid value");
          break;
        }
      test2 = accum * test;
      if (sbit)
        test2 *= -1.0;
    }
  printf ("              Sign bit: %u, Expt biased to %5u,  2^%-5d = %8.1f  *  %10.8f = %16.12f\n\n",
	  sbit > 0 ? 1 : 0, new_expt, new_expt > 0 ? new_expt - 15 : 0, test, accum, test2);
#endif
  return (0);
} /* end convert_fp32_to_fp16 */

static int
convert_fp24_to_fp32 (const fp_24bits *fp24, float *fp32, const int mode)
{
  unsigned char  sbit = 0;           /* sign bit */
  unsigned char  expt = 0, new_expt; /* exponent bits */
  unsigned char  m2, m1;    /* MSB to LSB of mantissa */
  unsigned char  new_m3, new_m2, new_m1;
  unsigned short mant;
  unsigned int   new_mant;
  unsigned int   little_endian = 1;
  unsigned char *mp;
  const unsigned char *src;
  unsigned char *dst;

#ifdef DEBUG32
  int      i, j, k, bit;
  double   test  = 0.0; 
  double   test2 = 0.0; 
  double   accum = 0.0;
  errno = 0;
#endif
  (void) mode;

  assert (sizeof(int) == 4);
  if ((fp24 == NULL) || (fp32 == NULL))
    {
      (void) fprintf (stderr, "Invalid src or destination pointers\n");
      return (1);
    }
  little_endian = *((unsigned char *)&little_endian) & '1';

  src = (const unsigned char *)fp24;
  dst = (unsigned char *)fp32;
  mp  = (unsigned char *)&mant;

  new_expt = expt = 0;
  new_mant = mant = 0;
  new_m3 = new_m2 = new_m1 = m2 = m1 = 0;

  if ((int)*fp24 == 0)
    {
      *dst = 0;
      *(dst + 1) = 0;
      *(dst + 2) = 0;
      *(dst + 3) = 0;
    }
  else
    {
      if (little_endian)
        {
          sbit = *(src + 2) & 0x80;
          expt = *(src + 2) & 0x7F;
          m2  =  *(src + 1);
          m1  =  *src;
        }
      else
        {
          sbit = *src & 0x80;
          expt = *src & 0x7F;
          m2  =  *(src + 1);
          m1  =  *(src + 2);
        }

      if (expt != 0)
        new_expt = expt - 63 + 127;
      mant = (m2 << 8) | m1;

      new_m3  =  (m2 & 0xFE) >> 1;
      new_m2  = ((m2 & 0x01) << 7) | ((m1 & 0xFE) >> 1);
      new_m1  =  (m1 & 0x01) << 7;
    }
  if (little_endian)
    {
      *dst = new_m1;
      *(dst + 1) = new_m2;
      *(dst + 2) = ((new_expt & 1) << 7) | new_m3;
      *(dst + 3) = sbit | (new_expt >> 1);
    }
  else
    {
      *dst = sbit | (new_expt >> 1);
      *(dst + 1) = ((new_expt & 1) << 7) | new_m3;
      *(dst + 2) = new_m2;
      *(dst + 3) = new_m1;
    }

#ifdef DEBUG32
  /* Debugging code for display only */
  new_mant = (new_m3 << 16) | (new_m2 << 8) | new_m1;
  (void) printf ("  mant ");
  for (j = 0, k = 15; j < 16; j++, k--)
    {
      bit = mant & (1 << k);
      if ((j % 8) == 0)
        (void) printf(" ");
      (void) printf ("%d", bit ? 1 : 0);
    }

  (void) printf (" New Mant ");
  for (j = 0, k = 22; j < 23; j++, k--)
    {
      bit = new_mant & (1 << k);
      (void) printf ("%d", bit ? 1 : 0);
      if ((k % 8) == 0)
        (void) printf(" ");
    }

  (void) printf (" Sbit + Exp ");
  if (little_endian)
    {
      for (i = 3; i >= 0; i--)
        {
          for (j = 0, k = 7; j < 8; j++, k--)
            {
              bit = *(dst + i) & (1 << k);
              if (i == 2 && j == 1)
                (void) printf ("  Mant: ");
              (void) printf ("%d", bit ? 1 : 0);
            }
        }
    }
  else  
    {
      for (i = 0; i < 4; i++)
        {
          for (j = 0, k = 7; j < 8; j++, k--)
            {
              bit = *(dst + i) & (1 << k);
              if (i == 1 && j == 1)
                (void) printf ("  Mant: ");
              (void) printf ("%d", bit ? 1 : 0);
            }
        }
    }
  (void) printf ("\n");

  new_mant = ((unsigned int)new_m3 << 16) | ((unsigned int)new_m2 << 8) | new_m1;
  if ((int)*fp24 == 0.0)
    {
      test = 0.0;
      test2 = 0.0;
      accum = 0;
    }
  else
    {
      accum = 0.0;
      for (i = 22, j = 1; i >= 0; i--, j++)
        {
          bit = new_mant & ((unsigned int)1 << i);
          if (bit)
            accum += (1.0 / ((unsigned int)1 << j));
        }
      accum += 1.0;
      test = powf (2.0, 1.0 * (new_expt - 127));
      switch (errno)
        {
        case 0:     break;
        case EDOM: 
        case ERANGE: 
        default: perror ("Invalid value");
          break;
        }
      test2 = accum * test;
      if (sbit)
        test2 *= -1.0;
    }
  (void) printf ("              Sign bit: %u, Expt biased to %5u,  2^%-5d = %8.1f  *  %10.8f = %16.8f\n\n",
                 sbit > 0 ? 1 : 0, new_expt, new_expt > 0 ? new_expt - 127 : 0, test, accum, test2);
#endif

  return (0);
} /* end convertfp24_to_fp32 */

static int convert_fp32_to_fp24 (float *fp32, fp_24bits *fp24, int mode)
{
  int            i = 1;
  int            rbits, bit;
  unsigned char  sbit = 0;           /* sign bit */
  unsigned char  expt = 0; /* exponent bits */
  unsigned char  m3;   /* high order bits of mantissa */
  unsigned char  m2;
  unsigned char  m1;   /* low order bits of mantissa */
  unsigned char  new_m2, new_m1;
  signed   char  new_expt;
  unsigned int   mant, new_mant;
  unsigned int   little_endian = 1;
  unsigned char *mp;
  unsigned char *src;
  unsigned char *dst;

#ifdef DEBUG24
  int      j, k;
  double   test  = 0.0; 
  double   test2 = 0.0; 
  double   accum = 0.0;
  double   roundup = 0.0;
#endif

  errno = 0;
  assert (sizeof(int) == 4);
  if ((fp32 == NULL) || (fp24 == NULL))
    {
      fprintf (stderr, "Invalid src or destination pointers\n");
      return (1);
    }
  little_endian = *((unsigned char *)&little_endian) & '1';

  src = (unsigned char *)fp32;
  dst = (unsigned char *)fp24;
  mp  = (unsigned char *)&mant;

  new_expt = expt = 0;
  mant = new_mant = 0;
  m2 = m1 = 0;
  rbits = 0;

  if (*fp32 != 0)
    {
      if (little_endian)
        {
          sbit =   *(src + 3) & 0x80;
          expt = ((*(src + 3) & 0x7F) << 1) | 
            ((*(src + 2) & 0x80) >> 7);
          m3  = (((*(src + 2) & 0x7F)) << 1) |
            ((*(src + 1) & 0x80) >> 7);
          m2  = (((*(src + 1) & 0x7F)) << 1)  |
            ((*src & 0x80) >> 7);
          m1  =  (*src & 0x7F) << 1;
        }
      else
        {
          sbit =   *src & 0x80;
          expt = ((*src & 0x7F) << 1) | 
            ((*(src + 1) & 0x80) >> 7);
          m3  = (((*(src + 1) & 0x7F)) << 1) |
            ((*(src + 2) & 0x80) >> 7);
          m2  = (((*(src + 2) & 0x7F)) << 1)  |
            ((*(src + 3) & 0x80) >> 7);
          m1  =  (*(src + 3) & 0x7F) << 1;
        }
  
      if (expt != 0)
        new_expt = expt - 127 + 63;

      if (new_expt <= 0)
        {
#ifdef DEBUG24
          fprintf (stderr, "Subnormal detected\n");
#endif
          if (mode == STRICT_IEEE)
            {
              /* NaN has all 1s in exponent plus 2 bits in Mantissa */
              new_expt = 0x7F;
              mant = 0x80010000;
              errno = ERANGE;
              fprintf (stderr, 
                       "Overflow. %12.4f exceeded largest storable value. Result clippped\n",
                       *fp32);
              return (1);
            }
          else
            { /* Clamp to minimum representable value for fp24 */
              new_expt = 0;
              mant = 1;
            }
        }
      else
        {
          if (new_expt > 126)
            {
              if (mode == STRICT_IEEE)
                {
                  /* NaN has all 1s in exponent plus 2 bits in Mantissa */
                  new_expt = 0x7F;
                  mant = 0x80010000;
                  errno = ERANGE;
                  fprintf (stderr, 
                           "Overflow. %12.4f exceeded largest storable value. Result clippped\n",
                           *fp32);
                  return (1);
                }
              else
                { /* Clamp to maximum allowed value for fp24 */
                  new_expt = 126;
                  mant = 0xFFFF0000;
                }
            }
          else
            { /* Remove the bits to the left of the binary point 
               * by shifting the fractional bits into the leftmost position
               */
              mant = (m3 << 24) | (m2 << 16) |( m1 << 8); 

              /* Check bits to the right of Unit in last signficant place.
               * Rounding of least significant retained bit falls to value
               * which will produce a zero in the LSB if the bounding values
               * are equidistant from the unrounded value.
               */

              rbits = mant & 0x0000FFFF;
              if (rbits >= 0x8000)  /* Greater than or equal to 0.5 times LSB */
                {
                  if (rbits > 0x8000) /* Rbits is greater than half of LSB */
                    {
                      /* Round up to next higher value of LSB */
                      for (i = 16; i < 32; i++)
                        {
                          bit = mant & (1 << i);
                          if (bit == 0)
                            { 
                              /* Round up by inserting a 1 at first zero and 
                               * clearing bits to the right
                               */
                              new_mant = (mant | ((unsigned int)1 << i)) &
                                (0xFFFF << i);
                              mp  = (unsigned char *)&new_mant;
                              break;
                            }
                        }
                    }
                  else    /* Rbits is exactly half of LSB */
                    {
                      if ((mant & 0x010000)) /* LSB is one so we round up */
                        {
                          /* Round up to next higher value of LSB */
                          for (i = 16; i < 32; i++)
                            {
                              bit = mant & (1 << i);
                              if (bit == 0)
                                {
                                  new_mant = (mant | ((unsigned int)1 << i)) &
                                    (0xFFFF << i);
                                  mp  = (unsigned char *)&new_mant;
                                  break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
  if (little_endian)
    {
      new_m2 = *(mp + 3);
      new_m1 = *(mp + 2);
      *dst = new_m1;
      *(dst + 1) = new_m2;
      *(dst + 2) = sbit | (new_expt & 0x7F);
    }
  else
    {
      new_m2 = *mp;
      new_m1 = *(mp + 1);
      *dst = sbit | (new_expt & 0x7F);
      *(dst + 1) = new_m2;
      *(dst + 2) = new_m1;
    }

#ifdef DEBUG24
  /* Debugging code for display only */
  printf ("%10.2f  mant%s ", *fp32, rbits ? "+" : "-");
  for (j = 0, k = 31; j < 23; j++, k--)
    {
      bit = mant & (1 << k);
      if ((j % 8) == 0)
        printf(" ");
      printf ("%d", bit ? 1 : 0);
      if (bit && (j > 15))
        roundup += (1.0 / (double)(1 << (j - 15)));
    }

  if (new_mant == 0)
    {
      printf (" Fract: %8.6f  m2m1 ", roundup);
      for (j = 0, k = 7; j < 8; j++, k--)
        {
          bit = new_m2 & (1 << k);
          printf ("%d", bit ? 1 : 0);
        }
      printf(" ");
      for (j = 0, k = 7; j < 8; j++, k--)
        {
          bit = new_m1 & (1 << k);
          printf ("%d", bit ? 1 : 0);
        }
    }
  else
    {
      printf (" Fract: %8.6f  Rbits Mant ", roundup);
      for (j = 0, k = 31; j < 16; j++, k--)
        {
          bit = new_mant & (1 << k);
          if (j == 8)
            printf(" ");
          printf ("%d", bit ? 1 : 0);
        }
    }
  printf (" Sbit + Exp ");
  if (little_endian)
    {
      for (i = 2; i >= 0; i--)
        {
          for (j = 0, k = 7; j < 8; j++, k--)
            {
              bit = *(dst + i) & (1 << k);
              if (i == 1 && j == 0)
                printf ("  Mant: ");
              printf ("%d", bit ? 1 : 0);
            }
        }
    }
  else  
    {
      for (i = 0; i < 3; i++)
        {
          for (j = 0, k = 7; j < 8; j++, k--)
            {
              bit = *(dst + i) & (1 << k);
              if (i == 1 && j == 0)
                printf ("  Mant: ");
              printf ("%d", bit ? 1 : 0);
            }
        }
    }
  printf ("\n");

  mant = ((unsigned int)new_m2 << 8) | (unsigned int)new_m1; 
  if (*fp32 == 0.0)
    {
      test = 0.0;
      test2 = 0.0;
      accum = 0;
    }
  else
    {
      accum = 0.0;
      for (i = 15, j = 1; i >= 0; i--, j++)
        {
          bit = mant & ((unsigned int)1 << i);
          if (bit)
            accum += (1.0 / ((unsigned int)1 << j));
        }
      if (new_expt != 0)
        accum += 1.0;
      test = powf (2.0, 1.0 * (new_expt - 63));
      switch (errno)
        {
        case 0:     break;
        case EDOM: 
        case ERANGE: 
        default: perror ("Invalid value");
          break;
        }
      test2 = accum * test;
      if (sbit)
        test2 *= -1.0;
    }
  printf ("              Sign bit: %u, Expt biased to %5u,  2^%-5d = %8.1f  *  %10.8f = %16.8f\n\n",
	  sbit > 0 ? 1 : 0, new_expt, new_expt > 0 ? new_expt - 63 : 0, test, accum, test2);
#endif

  return (0);
} /* end convertfp32_to_fp24 */


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o n s t i t u t e I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ConstituteImage() returns an Image corresponding to an image stored
%  in a raw memory array format. The pixel data must be in scanline order
%  top-to-bottom. The data can be unsigned char, unsigned short int, unsigned
%  int, unsigned long, float, or double.  Float and double require the pixels
%  to be normalized to the range [0..1], otherwise the range is [0..MaxVal]
%  where MaxVal is the maximum possible value for that type.
%
%  Note that for most 32-bit architectures the size of an unsigned long is
%  the same as unsigned int, but for 64-bit architectures observing the LP64
%  standard, an unsigned long is 64 bits, while an unsigned int remains 32
%  bits. This should be considered when deciding if the data should be
%  described as "Integer" or "Long".
%
%  For example, to create a 640x480 image from unsigned red-green-blue
%  character data, use
%
%      image=ConstituteImage(640,480,"RGB",CharPixel,pixels,&exception);
%
%  The format of the Constitute method is:
%
%      Image *ConstituteImage(const unsigned long width,
%        const unsigned long height,const char *map,const StorageType type,
%        const void *pixels,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o width: width in pixels of the image.
%
%    o height: height in pixels of the image.
%
%    o map: This string reflects the expected ordering of the pixel array.
%      It can be any combination or order of R = red, G = green, B = blue,
%      A = alpha (same as Transparency), O = Opacity, T = Transparency,
%      C = cyan, Y = yellow, M = magenta, K = black, or I = intensity
%      (for grayscale). Specify "P" = pad, to skip over a quantum which is
%      intentionally ignored. Creation of an alpha channel for CMYK images
%      is currently not supported.
%
%    o type: Define the data type of the pixels.  Float and double types are
%      expected to be normalized [0..1] otherwise [0..MaxRGB].  Choose from
%      these types: CharPixel, ShortPixel, IntegerPixel, LongPixel, FloatPixel,
%      or DoublePixel.
%
%    o pixels: This array of values contain the pixel components as defined by
%      map and type.  You must preallocate this array where the expected
%      length varies depending on the values of width, height, map, and type.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

MagickExport Image *ConstituteImage(const unsigned long width,
  const unsigned long height,const char *map,const StorageType type,
  const void *pixels,ExceptionInfo *exception)
{
  Image
    *image;

  long
    y;

  PixelPacket
    *q;

  register Quantum
    quantum;

  MapQuantumType
    switch_map[MaxTextExtent/sizeof(MapQuantumType)];

  register IndexPacket
    *indexes;

  register long
    i,
    x;

  size_t
    length;

  /*
    Allocate image structure.
  */
  assert(pixels != (void *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  SetExceptionInfo(exception,UndefinedException);
  image=AllocateImage((ImageInfo *) NULL);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  if ((width == 0) || (height == 0))
    ThrowBinaryException3(OptionError,UnableToConstituteImage,
      NonzeroWidthAndHeightRequired);
  image->columns=width;
  image->rows=height;

  /*
    Handle a few common special cases in order to improve performance.
  */
  if (type == CharPixel)
    {
      DispatchType
        dispatch_type=UndefinedDispatchType;

      if (LocaleCompare(map,"BGR") == 0)
        dispatch_type=BGRDispatchType;
      else if (LocaleCompare(map,"BGRO") == 0)
        dispatch_type=BGRODispatchType;
      else if (LocaleCompare(map,"BGRP") == 0)
        dispatch_type=BGRPDispatchType;
      else if (LocaleCompare(map,"RGB") == 0)
        dispatch_type=RGBDispatchType;
      else if (LocaleCompare(map,"RGBO") == 0)
        dispatch_type=RGBODispatchType;
      else if (LocaleCompare(map,"I") == 0)
        {
          if (!AllocateImageColormap(image,MaxColormapSize))
            ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
                                 UnableToConstituteImage);
          dispatch_type=IDispatchType;
        }

      if (dispatch_type != UndefinedDispatchType)
        {
          register const unsigned char
            *p = (const unsigned char*) pixels;

          for (y=0; y < (long) image->rows; y++)
            {
              q=SetImagePixels(image,0,y,image->columns,1);
              if (q == (PixelPacket *) NULL)
                break;
              indexes=AccessMutableIndexes(image);

              switch (dispatch_type)
                {
                case BGRDispatchType:
                  {
                    for (x=(long) image->columns; x != 0; x--)
                      {
                        SetBlueSample(q,ScaleCharToQuantum(*p++));
                        SetGreenSample(q,ScaleCharToQuantum(*p++));
                        SetRedSample(q,ScaleCharToQuantum(*p++));
                        SetOpacitySample(q,OpaqueOpacity);
                        q++;
                      }
                    break;
                  }
                case BGRODispatchType:
                  { 
                    for (x=(long) image->columns; x != 0; x--)
                      {
                        SetBlueSample(q,ScaleCharToQuantum(*p++));
                        SetGreenSample(q,ScaleCharToQuantum(*p++));
                        SetRedSample(q,ScaleCharToQuantum(*p++));
                        SetOpacitySample(q,(Quantum) MaxRGB-ScaleCharToQuantum(*p++));
                        q++;
                      }
                    break;
                  }
                case BGRPDispatchType:
                  {
                    for (x=(long) image->columns; x != 0; x--)
                      {
                        SetBlueSample(q,ScaleCharToQuantum(*p++));
                        SetGreenSample(q,ScaleCharToQuantum(*p++));
                        SetRedSample(q,ScaleCharToQuantum(*p++));
                        p++;
                        SetOpacitySample(q,OpaqueOpacity);
                        q++;
                      }
                    break;
                  }
                case RGBDispatchType:
                  {
                    for (x=(long) image->columns; x != 0; x--)
                      {
                        SetRedSample(q,ScaleCharToQuantum(*p++));
                        SetGreenSample(q,ScaleCharToQuantum(*p++));
                        SetBlueSample(q,ScaleCharToQuantum(*p++));
                        SetOpacitySample(q,OpaqueOpacity);
                        q++;
                      }
                    break;
                  }
                case RGBODispatchType:
                  {
                    for (x=(long) image->columns; x != 0; x--)
                      {
                        SetRedSample(q,ScaleCharToQuantum(*p++));
                        SetGreenSample(q,ScaleCharToQuantum(*p++));
                        SetBlueSample(q,ScaleCharToQuantum(*p++));
                        SetOpacitySample(q,MaxRGB-ScaleCharToQuantum(*p++));
                        q++;
                      }
                    break;
                  }
                case IDispatchType:
                  {
                    for (x=(long) image->columns; x != 0; x--)
                      {
                        *indexes=ScaleQuantumToIndex(ScaleCharToQuantum(*p++));
                        SetGraySample(q,image->colormap[*indexes].red);
                        SetOpacitySample(q,OpaqueOpacity);
                        indexes++;
                        q++;
                      }
                    break;
                  }
                case UndefinedDispatchType:
                  {
                    break;
                  }
                } /* end switch */
              if (!SyncImagePixels(image))
                break;
            } /* end for (y=0; y < (long) image->rows; y++) */
          if (dispatch_type == IDispatchType)
            {
              (void) IsMonochromeImage(image,exception);
              image->is_grayscale=True;
            }
          return (image);
        } /* end if (dispatch_type != UndefinedDispatchType) */
    } /* end if (type == CharPixel) */

  /*
    Prepare a validated and more efficient version of the map.
  */
  length=strlen(map);
  length=Min(length,sizeof(switch_map)/sizeof(MapQuantumType));
  for (i=0; i < (long) length; i++)
    {
      switch ((int) toupper(map[i]))
        {
        case 'R':
          {
            switch_map[i]=RedMapQuantum;
            break;
          }
        case 'G':
          {
            switch_map[i]=GreenMapQuantum;
            break;
          }
        case 'B':
          {
            switch_map[i]=BlueMapQuanum;
            break;
          }
        case 'A':
          {
            switch_map[i]=OpacityMapQuantum;
            image->matte=True;
            break;
          }
        case 'C':
          {
            image->colorspace=CMYKColorspace;
            switch_map[i]=RedMapQuantum;
            break;
          }
        case 'M':
          {
            image->colorspace=CMYKColorspace;
            switch_map[i]=GreenMapQuantum;
            break;
          }
        case 'Y':
          {
            image->colorspace=CMYKColorspace;
            switch_map[i]=BlueMapQuanum;
            break;
          }
        case 'K':
          {
            image->colorspace=CMYKColorspace;
            switch_map[i]=OpacityMapQuantum;
            break;
          }
        case 'I':
          {
            if (!AllocateImageColormap(image,MaxColormapSize))
              ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
                UnableToConstituteImage);
            switch_map[i]=IntensityMapQuantum;
            break;
          }
        case 'O':
          {
            switch_map[i]=OpacityInvertedMapQuantum;
            image->matte=True;
            break;
          }
        case 'P':
          {
            switch_map[i]=PadMapQuantum;
            break;
          }
        case 'T':
          {
            switch_map[i]=OpacityMapQuantum;
            image->matte=True;
            break;
          }
        default:
          {
            DestroyImage(image);
            ThrowImageException(OptionError,UnrecognizedPixelMap,map)
          }
        }
    }

  /*
    Transfer the pixels from the pixel data array to the image.
  */
  for (y=0; y < (long) image->rows; y++)
    {
      q=SetImagePixels(image,0,y,image->columns,1);
      if (q == (PixelPacket *) NULL)
        break;
      indexes=AccessMutableIndexes(image);
      for (x=0; x < (long) image->columns; x++)
        {
	  SetRedSample(q,0);
	  SetGreenSample(q,0);
	  SetBlueSample(q,0);
          if (image->colorspace == CMYKColorspace)
            {
              SetBlackSample(q,0);
              *indexes=OpaqueOpacity;
            }
          else
            {
              SetOpacitySample(q,OpaqueOpacity);
            }
          for (i=0; i < (long) length; i++)
            {
              /*
                Input a quantum
              */
              quantum=0U;
              
              switch (type)
                {
                case CharPixel:
                  {
                    register const unsigned char *p = (const unsigned char*) pixels;
                    quantum=ScaleCharToQuantum(*p++);
                    pixels = (const void *) p;
                    break;
                  }
                case ShortPixel:
                  {
                    register const unsigned short *p = (const unsigned short*) pixels;
                    quantum=ScaleShortToQuantum(*p++);
                    pixels = (const void *) p;
                    break;
                  }
                case IntegerPixel:
                  {
                    register const unsigned int *p = (const unsigned int*) pixels;
                    quantum=ScaleLongToQuantum(*p++);
                    pixels = (const void *) p;
                    break;
                  }
                case LongPixel:
                  {
                    register const unsigned long *p = (const unsigned long*) pixels;
                    quantum=ScaleLongToQuantum(*p++);
                    pixels = (const void *) p;
                    break;
                  }
                case FloatPixel:
                  {
                    double quantum_float;
                    register const float *p = (const float*) pixels;
                    quantum_float=(double) MaxRGB*(*p++);
                    quantum=RoundDoubleToQuantum(quantum_float);
                    pixels = (const void *) p;
                    break;
                  }
                case DoublePixel:
                  {
                    double quantum_float;
                    register const double *p = (const double*) pixels;
                    quantum_float=(double) MaxRGB*(*p++);
                    quantum=RoundDoubleToQuantum(quantum_float);
                    pixels = (const void *) p;
                    break;
                  }
                }
              
              /*
                Transfer quantum to image
              */
              switch (switch_map[i])
                {
                case RedMapQuantum:
                  {
                    SetRedSample(q,quantum);
                    break;
                  }
                case GreenMapQuantum:
                  {
                    SetGreenSample(q,quantum);
                    break;
                  }
                case BlueMapQuanum:
                  {
                    SetBlueSample(q,quantum);
                    break;
                  }
                case OpacityMapQuantum:
                  {
                    SetOpacitySample(q,quantum);
                    break;
                  }
                case OpacityInvertedMapQuantum:
                  {
                    SetOpacitySample(q,MaxRGB-quantum);
                    break;
                  }
                case IntensityMapQuantum:
                  {
                    *indexes=ScaleQuantumToIndex(quantum);
                    SetGraySample(q,image->colormap[*indexes].red);
                    break;
                  }
                case PadMapQuantum:
                  {
                    /* Discard quantum */
                    break;
                  }
                }
            }
          indexes++;
          q++;
        }
      if (!SyncImagePixels(image))
        break;
    }
  
  if (image->storage_class == PseudoClass)
    {
      /*
        Check and cache monochrome and grayscale status
      */
      (void) IsMonochromeImage(image,exception);
      if (image->is_monochrome)
        image->is_grayscale=True;
      else
        (void) IsGrayImage(image,exception);
    }
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y C o n s t i t u t e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyConstitute() destroys the constitute environment.
%
%  The format of the DestroyConstitute method is:
%
%      DestroyConstitute(void)
%
%
*/
MagickExport void DestroyConstitute(void)
{
#if defined(JUST_FOR_DOCUMENTATION)
  /* The first two calls should bracket any code that deals with the data
     structurees being released */
  AcquireSemaphoreInfo(&constitute_semaphore);
  LiberateSemaphoreInfo(&constitute_semaphore);
#endif
  /* The final call actually releases the associated mutex used to prevent
     multiple threads from accessing the data */
  DestroySemaphoreInfo(&constitute_semaphore);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     C o n s t i t u t e T e x t u r e I m a g e                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ConstituteTextureImage() returns a new image canvas based on repeatedly
%  tiling the texture image across and down the new image canvas.  The
%  returned image properties are similar to the texture image properties.
%
%  The format of the TextureImage method is:
%
%      Image *ConstituteTextureImage(unsigned long columns,unsigned long rows,
%                              const Image *texture, ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o columns: The number of columns in the new image.
%
%    o rows: The number of rows in the new image.
%
%    o texture: The texture image to layer on the background.
%
%    o exceptions: Any errors are reported here.
%
*/
#define ConstituteTextureImageText  "[%s] Generate image texture...  "
MagickExport Image *ConstituteTextureImage(const unsigned long columns,
                                           const unsigned long rows,
                                           const Image *texture_image,
                                           ExceptionInfo *exception)
{
  Image
    *canvas_image;

  long
    y;

  unsigned long
    row_count=0;

  MagickPassFail
    status=MagickPass;

  assert(texture_image != (Image *) NULL);
  assert(texture_image->signature == MagickSignature);

  canvas_image=CloneImage(texture_image,columns,rows,MagickTrue,exception);
  if (canvas_image == (Image *) NULL)
    return canvas_image;
#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  pragma omp parallel for schedule(dynamic,4) shared(row_count, status)
#endif
  for (y=0; y < (long) canvas_image->rows; y++)
    {
      const PixelPacket
        *texture_pixels;

      PixelPacket
        *canvas_pixels;

      unsigned long
        x;

      MagickBool
        thread_status;

      thread_status=status;
      if (thread_status == MagickFail)
        continue;

      texture_pixels=AcquireImagePixels(texture_image,0,
                                        y % texture_image->rows,
                                        texture_image->columns,1,
                                        exception);
      canvas_pixels=SetImagePixelsEx(canvas_image,0,y,canvas_image->columns,
                                     1,exception);

      if ((texture_pixels == (const PixelPacket *) NULL) ||
          (canvas_pixels == (PixelPacket *) NULL))
        thread_status=MagickFail;

      if (thread_status != MagickFail)
        {
          const IndexPacket
            *texture_indexes=(const IndexPacket *) NULL;
          
          IndexPacket
            *canvas_indexes=(IndexPacket *) NULL;;

          if (texture_image->storage_class == PseudoClass)
            texture_indexes=AccessImmutableIndexes(texture_image);
          if (canvas_image->storage_class == PseudoClass)
            canvas_indexes=AccessMutableIndexes(canvas_image);

          for (x=0; x < canvas_image->columns; x+=texture_image->columns)
            {
              unsigned long
                texture_width;

              texture_width=texture_image->columns;
              if ((x+texture_width) > canvas_image->columns)
                texture_width=canvas_image->columns-x;

              if ((texture_indexes != (const IndexPacket *) NULL) &&
                  (canvas_indexes != (const IndexPacket *) NULL))
                {
                  (void) memcpy(canvas_indexes,texture_indexes,texture_width*sizeof(IndexPacket));
                  canvas_indexes += texture_width;
                }
              (void) memcpy(canvas_pixels,texture_pixels,texture_width*sizeof(PixelPacket));
              canvas_pixels += texture_width;
            }

          if (!SyncImagePixelsEx(canvas_image,exception))
            thread_status=MagickFail;
        }
#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  pragma omp critical
#endif
      {
        row_count++;
        if (QuantumTick(row_count,canvas_image->rows))
          if (!MagickMonitorFormatted(row_count,canvas_image->rows,exception,
                                      ConstituteTextureImageText,
                                      texture_image->filename))
            thread_status=MagickFail;

        if (thread_status == MagickFail)
          status=MagickFail;
      }
    }

  if (status == MagickFail)
    {
      DestroyImage(canvas_image);
      canvas_image=(Image *) NULL;
    }

  if (canvas_image != (Image *) NULL)
    {
      canvas_image->is_monochrome=texture_image->is_monochrome;
      canvas_image->is_grayscale=texture_image->is_grayscale;
    }

  return canvas_image;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D i s p a t c h I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DispatchImage() extracts pixel data from an Image into a raw memory array.
%  The pixel data is written in scanline order top-to-bottom using an 
%  arbitrary quantum order specified by 'map', and with quantum size
%  specified by 'type'.
%
%  The output array data may be unsigned char, unsigned short int, unsigned
%  int, unsigned long, float, or double.  Float and double require the pixels
%  to be normalized to the range [0..1], otherwise the range is [0..MaxVal]
%  where MaxVal is the maximum possible value for that type.
%
%  The method returns MagickPass on success or MagickFail if an error is
%  encountered.
%
%  Suppose we want want to extract the first scanline of a 640x480 image as
%  character data in red-green-blue order:
%
%      DispatchImage(image,0,0,640,1,"RGB",0,pixels,exception);
%
%  The format of the DispatchImage method is:
%
%      unsigned int DispatchImage(const Image *image,const long x_offset,
%        const long y_offset,const unsigned long columns,
%        const unsigned long rows,const char *map,const StorageType type,
%        void *pixels,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o x_offset, y_offset, columns, rows:  These values define the perimeter
%      of a region of pixels you want to extract.
%
%    o map: This string reflects the expected ordering of the pixel array.
%      It can be any combination or order of R = red, G = green, B = blue,
%      A = alpha  (same as Transparency), O = Opacity, T = Transparency,
%      C = cyan, Y = yellow, M = magenta, K = black, I = intensity (for
%      grayscale). Specify "P" = pad, to output a pad quantum. Pad quantums
%      are zero-value.
%
%    o type: Define the data type of the pixels.  Float and double types are
%      expected to be normalized [0..1] otherwise [0..MaxRGB].  Choose from
%      these types: CharPixel, ShortPixel, IntegerPixel, LongPixel, FloatPixel,
%      or DoublePixel.
%
%    o pixels: This array of values contain the pixel components as defined by
%      map and type.  You must preallocate this array where the expected
%      length varies depending on the values of width, height, map, and type.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport MagickPassFail DispatchImage(const Image *image,const long x_offset,
  const long y_offset,const unsigned long columns,const unsigned long rows,
  const char *map,const StorageType type,void *pixels,ExceptionInfo *exception)
{
  long
    y;

  register long
    i,
    x;

  register const PixelPacket
    *p;

  register Quantum
    quantum;

  MapQuantumType
    switch_map[MaxTextExtent/sizeof(MapQuantumType)];

  size_t
    length;

  MagickPassFail
    status=MagickPass;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  /*
    Handle a few common special cases in order to improve performance.
  */
  if (type == CharPixel)
    {
      DispatchType
        dispatch_type=UndefinedDispatchType;

      if (LocaleCompare(map,"BGR") == 0)
        dispatch_type=BGRDispatchType;
      else if (LocaleCompare(map,"BGRO") == 0)
        dispatch_type=BGRODispatchType;
      else if (LocaleCompare(map,"BGRP") == 0)
        dispatch_type=BGRPDispatchType;
      else if (LocaleCompare(map,"RGB") == 0)
        dispatch_type=RGBDispatchType;
      else if (LocaleCompare(map,"RGBO") == 0)
        dispatch_type=RGBODispatchType;
      else if (LocaleCompare(map,"I") == 0)
        dispatch_type=IDispatchType;

      if (dispatch_type != UndefinedDispatchType)
        {
          register unsigned char
            *q = (unsigned char*) pixels;
          
          for (y=0; y < (long) rows; y++)
            {
              p=AcquireImagePixels(image,x_offset,y_offset+y,columns,1,exception);
              if (p == (const PixelPacket *) NULL)
                {
                  status=MagickFail;
                  break;
                }
              
              switch (dispatch_type)
                {
                case BGRDispatchType:
                  {
                    for (x=(long) columns; x != 0; x--)
                      {
                        *q++=ScaleQuantumToChar(GetBlueSample(p));
                        *q++=ScaleQuantumToChar(GetGreenSample(p));
                        *q++=ScaleQuantumToChar(GetRedSample(p));
                        p++;
                      }
                    break;
                  }
                case BGRODispatchType:
                  {
                    for (x=(long) columns; x != 0; x--)
                      {
                        *q++=ScaleQuantumToChar(GetBlueSample(p));
                        *q++=ScaleQuantumToChar(GetGreenSample(p));
                        *q++=ScaleQuantumToChar(GetRedSample(p));
                        *q++=ScaleQuantumToChar(MaxRGB-GetOpacitySample(p));
                        p++;
                      }
                    break;
                  }
                case BGRPDispatchType:
                  {
                    for (x=(long) columns; x != 0; x--)
                      {
                        *q++=ScaleQuantumToChar(GetBlueSample(p));
                        *q++=ScaleQuantumToChar(GetGreenSample(p));
                        *q++=ScaleQuantumToChar(GetRedSample(p));
                        *q++=0;
                        p++;
                      }
                    break;
                  }
                case RGBDispatchType:
                  {
                    for (x=(long) columns; x != 0; x--)
                      {
                        *q++=ScaleQuantumToChar(GetRedSample(p));
                        *q++=ScaleQuantumToChar(GetGreenSample(p));
                        *q++=ScaleQuantumToChar(GetBlueSample(p));
                        p++;
                      }
                    break;
                  }
                case RGBODispatchType:
                  {
                    for (x=(long) columns; x != 0; x--)
                      {
                        *q++=ScaleQuantumToChar(GetRedSample(p));
                        *q++=ScaleQuantumToChar(GetGreenSample(p));
                        *q++=ScaleQuantumToChar(GetBlueSample(p));
                        *q++=ScaleQuantumToChar(MaxRGB-GetOpacitySample(p));
                        p++;
                      }
                    break;
                  }
                case IDispatchType:
                  {
                    if (image->is_grayscale)
                      {
                        for (x=(long) columns; x != 0; x--)
                          {
                            *q++=ScaleQuantumToChar(GetGraySample(p));
                            p++;
                          }
                      }
                    else
                      {
                        for (x=(long) columns; x != 0; x--)
                          {
                            *q++=ScaleQuantumToChar(PixelIntensity(p));
                            p++;
                          }
                      }
                    break;
                  }
                case UndefinedDispatchType:
                  {
                  }
                }
            }
          return (status);
        }
    }

  length=strlen(map);
  length=Min(length,sizeof(switch_map)/sizeof(MapQuantumType));

  /*
    Prepare a validated and more efficient version of the map.
  */
  for (i=0; i < (long) length; i++)
    {
      switch ((int) toupper(map[i]))
        {
        case 'R':
          {
            switch_map[i]=RedMapQuantum;
            break;
          }
        case 'G':
          {
            switch_map[i]=GreenMapQuantum;
            break;
          }
        case 'B':
          {
            switch_map[i]=BlueMapQuanum;
            break;
          }
        case 'A':
        case 'T':
          {
            switch_map[i]=OpacityMapQuantum;
            break;
          }
        case 'C':
          {
            switch_map[i]=RedMapQuantum;
            if (image->colorspace == CMYKColorspace)
              break;
            ThrowException(exception,OptionError,ColorSeparatedImageRequired,map);
            return(MagickFail);
          }
        case 'M':
          {
            switch_map[i]=GreenMapQuantum;
            if (image->colorspace == CMYKColorspace)
              break;
            ThrowException(exception,OptionError,ColorSeparatedImageRequired,map);
            return(MagickFail);
          }
        case 'Y':
          {
            switch_map[i]=BlueMapQuanum;
            if (image->colorspace == CMYKColorspace)
              break;
            ThrowException(exception,OptionError,ColorSeparatedImageRequired,map);
            return(MagickFail);
          }
        case 'K':
          {
            switch_map[i]=OpacityMapQuantum;
            if (image->colorspace == CMYKColorspace)
              break;
            ThrowException(exception,OptionError,ColorSeparatedImageRequired,map);
            return(MagickFail);
          }
        case 'I':
          {
            switch_map[i]=IntensityMapQuantum;
            break;
          }
        case 'O':
          {
            switch_map[i]=OpacityInvertedMapQuantum;
            break;
          }
        case 'P':
          {
            switch_map[i]=PadMapQuantum;
            break;
          }
        default:
          {
            ThrowException(exception,OptionError,UnrecognizedPixelMap,map);
            return(MagickFail);
          }
        }
    }
  
  for (y=0; y < (long) rows; y++)
    {
      p=AcquireImagePixels(image,x_offset,y_offset+y,columns,1,exception);
      if (p == (const PixelPacket *) NULL)
        {
          status=MagickFail;
          break;
        }
      for (x=0; x < (long) columns; x++)
        {
          for (i=0; i < (long) length; i++)
            {
              /*
                Obtain quantum value
              */
              quantum=0U;
              switch (switch_map[i])
                {
                case RedMapQuantum:
                  {
                    quantum=GetRedSample(p);
                    break;
                  }
                case GreenMapQuantum:
                  {
                    quantum=GetGreenSample(p);
                    break;
                  }
                case BlueMapQuanum:
                  {
                    quantum=GetBlueSample(p);
                    break;
                  }
                case IntensityMapQuantum:
                  {
                    if (image->is_grayscale)
                      {
                        quantum=GetRedSample(p);
                      }
                    else
                      {
                        double intensity = PixelIntensity(p);
                        quantum=RoundDoubleToQuantum(intensity);
                      }
                    break;
                  }
                case OpacityInvertedMapQuantum:
                  {
                    if (image->matte)
                      quantum=GetOpacitySample(p);
                    quantum=MaxRGB-quantum;
                    break;
                  }
                case OpacityMapQuantum:
                  {
                    if (image->matte)
                      quantum=GetOpacitySample(p);
                    break;
                  }
                case PadMapQuantum:
                  {
                    /* Zero quantum */
                    break;
                  }
                }

              /*
                Output quantum
              */
              switch (type)
                {
                case CharPixel:
                  {
                    register unsigned char *q = (unsigned char*) pixels;
                    *q++=ScaleQuantumToChar(quantum);
                    pixels=(void *) q;
                    break;
                  }
                case ShortPixel:
                  {
                    register unsigned short *q = (unsigned short*) pixels;
                    *q++=ScaleQuantumToShort(quantum);
                    pixels=(void *) q;
                    break;
                  }
                case IntegerPixel:
                  {
                    register unsigned int *q = (unsigned int*) pixels;
                    *q++=ScaleQuantumToLong(quantum);
                    pixels=(void *) q;
                    break;
                  }
                case LongPixel:
                  {
                    register unsigned long *q = (unsigned long*) pixels;
                    *q++=ScaleQuantumToLong(quantum);
                    pixels=(void *) q;
                    break;
                  }
                case FloatPixel:
                  {
                    register float *q = (float*) pixels;
                    *q++=(float) ((double) quantum/MaxRGB);
                    pixels=(void *) q;
                    break;
                  }
                case DoublePixel:
                  {
                    register double *q = (double*) pixels;
                    *q++=(double) quantum/MaxRGB;
                    pixels=(void *) q;
                    break;
                  }
                }
            }
          p++;
        }
    }
  return(status);
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
MagickExport MagickPassFail ExportViewPixelArea(const ViewInfo *view,
  const QuantumType quantum_type,const unsigned int quantum_size,
  unsigned char *destination,const ExportPixelAreaOptions *options,
  ExportPixelAreaInfo *export_info)
{
  const Image
    *image;

  register const IndexPacket
    *indexes;

  register const PixelPacket
    *p;

  register unsigned char
    *q;

  register unsigned long
    x;

  register unsigned int
    unsigned_value,
    unsigned_scale = 1U;

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

  /* printf("quantum_type=%d  quantum_size=%u\n",(int) quantum_type, quantum_size); */

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
  p=AccessCacheViewPixels(view);
  indexes=AcquireCacheViewIndexes(view);
  q=destination;
  switch (quantum_type)
    {
    case IndexQuantum:
      {
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
                  
                  BitStreamInitializeWrite(&stream,q);
                  for (x = number_pixels; x != 0; --x)
                    {
                      BitStreamMSBWrite(&stream,quantum_size,*indexes);
                      indexes++;
                    }
                  q=stream.bytes;
                  break;
                }
              }
          }
        break;
      }
    case IndexAlphaQuantum:
      {
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
                  
                  BitStreamInitializeWrite(&stream,q);
                  for (x = number_pixels; x != 0; --x)
                    {
                      BitStreamMSBWrite(&stream,quantum_size,*indexes);
                      unsigned_value=MaxRGB-GetOpacitySample(p);
                      if (QuantumDepth >  quantum_size)
                        unsigned_value /= unsigned_scale;
                      else if (QuantumDepth <  quantum_size)
                        unsigned_value *= unsigned_scale;
                      BitStreamMSBWrite(&stream,quantum_size,unsigned_value);
                      indexes++;
                      p++;
                    }
                  q=stream.bytes;
                  break;
                }
              }
          }
        break;
      }
    case GrayQuantum:
      {
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

                  BitStreamInitializeWrite(&stream,q);
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
                          BitStreamMSBWrite(&stream,quantum_size,unsigned_value);
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
                          BitStreamMSBWrite(&stream,quantum_size,unsigned_value);
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
                          BitStreamMSBWrite(&stream,quantum_size,unsigned_value);
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
        break;
      }
    case GrayAlphaQuantum:
      {
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
                  
                  BitStreamInitializeWrite(&stream,q);
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
                          BitStreamMSBWrite(&stream,quantum_size,unsigned_value);
                          BitStreamMSBWrite(&stream,quantum_size,MaxRGB-GetOpacitySample(p));
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
                          BitStreamMSBWrite(&stream,quantum_size,unsigned_value);
                          BitStreamMSBWrite(&stream,quantum_size,
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
                          BitStreamMSBWrite(&stream,quantum_size,unsigned_value);
                          BitStreamMSBWrite(&stream,quantum_size,
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
        break;
      }
    case RedQuantum:
    case CyanQuantum:
      {
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
                  
                  BitStreamInitializeWrite(&stream,q);
                  if( QuantumDepth == sample_bits)
                    {
                      /* Unity scale */
                      for (x = number_pixels; x != 0; --x)
                        {
                          BitStreamMSBWrite(&stream,quantum_size,
                                            GetRedSample(p));
                          p++;
                        }
                    }
                  else if (QuantumDepth >  sample_bits)
                    {
                      /* Scale down */
                      for (x = number_pixels; x != 0; --x)
                        {
                          BitStreamMSBWrite(&stream,quantum_size,
                                            GetRedSample(p)/unsigned_scale);
                          p++;
                        }
                    }
                  else
                    {
                      /* Scale up */
                      for (x = number_pixels; x != 0; --x)
                        {
                          BitStreamMSBWrite(&stream,quantum_size,
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
        break;
      }
    case GreenQuantum:
    case MagentaQuantum:
      {
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
                  
                  BitStreamInitializeWrite(&stream,q);
                  if( QuantumDepth == sample_bits)
                    {
                      /* Unity scale */
                      for (x = number_pixels; x != 0; --x)
                        {
                          BitStreamMSBWrite(&stream,quantum_size,
                                            GetGreenSample(p));
                          p++;
                        }
                    }
                  else if (QuantumDepth >  sample_bits)
                    {
                      /* Scale down */
                      for (x = number_pixels; x != 0; --x)
                        {
                          BitStreamMSBWrite(&stream,quantum_size,
                                            GetGreenSample(p)/unsigned_scale);
                          p++;
                        }
                    }
                  else
                    {
                      /* Scale up */
                      for (x = number_pixels; x != 0; --x)
                        {
                          BitStreamMSBWrite(&stream,quantum_size,
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
        break;
      }
    case BlueQuantum:
    case YellowQuantum:
      {
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
                  
                  BitStreamInitializeWrite(&stream,q);
                  if( QuantumDepth == sample_bits)
                    {
                      /* Unity scale */
                      for (x = number_pixels; x != 0; --x)
                        {
                          BitStreamMSBWrite(&stream,quantum_size,
                                            GetBlueSample(p));
                          p++;
                        }
                    }
                  else if (QuantumDepth >  sample_bits)
                    {
                      /* Scale down */
                      for (x = number_pixels; x != 0; --x)
                        {
                          BitStreamMSBWrite(&stream,quantum_size,
                                            GetBlueSample(p)/unsigned_scale);
                          p++;
                        }
                    }
                  else
                    {
                      /* Scale up */
                      for (x = number_pixels; x != 0; --x)
                        {
                          BitStreamMSBWrite(&stream,quantum_size,
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
        break;
      }
    case AlphaQuantum:
      {
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
                      
                      BitStreamInitializeWrite(&stream,q);
                      if( QuantumDepth == sample_bits)
                        {
                          /* Unity scale */
                          for (x = number_pixels; x != 0; --x)
                            {
                              BitStreamMSBWrite(&stream,quantum_size,
                                                MaxRGB-*indexes);
                              indexes++;
                            }
                        }
                      else if (QuantumDepth >  sample_bits)
                        {
                          /* Scale down */
                          for (x = number_pixels; x != 0; --x)
                            {
                              BitStreamMSBWrite(&stream,quantum_size,
                                                (MaxRGB-*indexes)/unsigned_scale);
                              indexes++;
                            }
                        }
                      else
                        {
                          /* Scale up */
                          for (x = number_pixels; x != 0; --x)
                            {
                              BitStreamMSBWrite(&stream,quantum_size,
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
            break;
          }

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
                  
                  BitStreamInitializeWrite(&stream,q);
                  if( QuantumDepth == sample_bits)
                    {
                      /* Unity scale */
                      for (x = number_pixels; x != 0; --x)
                        {
                          BitStreamMSBWrite(&stream,quantum_size,
                                            MaxRGB-GetOpacitySample(p));
                          p++;
                        }
                    }
                  else if (QuantumDepth >  sample_bits)
                    {
                      /* Scale down */
                      for (x = number_pixels; x != 0; --x)
                        {
                          BitStreamMSBWrite(&stream,quantum_size,
                                            (MaxRGB-GetOpacitySample(p))/unsigned_scale);
                          p++;
                        }
                    }
                  else
                    {
                      /* Scale up */
                      for (x = number_pixels; x != 0; --x)
                        {
                          BitStreamMSBWrite(&stream,quantum_size,
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
        break;
      }
    case BlackQuantum:
      {
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
                  
                  BitStreamInitializeWrite(&stream,q);
                  if( QuantumDepth == sample_bits)
                    {
                      /* Unity scale */
                      for (x = number_pixels; x != 0; --x)
                        {
                          BitStreamMSBWrite(&stream,quantum_size,
                                            GetBlackSample(p));
                          p++;
                        }
                    }
                  else if (QuantumDepth >  sample_bits)
                    {
                      /* Scale down */
                      for (x = number_pixels; x != 0; --x)
                        {
                          BitStreamMSBWrite(&stream,quantum_size,
                                            GetBlackSample(p)/unsigned_scale);
                          p++;
                        }
                    }
                  else
                    {
                      /* Scale up */
                      for (x = number_pixels; x != 0; --x)
                        {
                          BitStreamMSBWrite(&stream,quantum_size,
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
        break;
      }
    case RGBQuantum:
    default:
      {
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
                  
                  BitStreamInitializeWrite(&stream,q);
                  if( QuantumDepth == sample_bits)
                    {
                      /* Unity scale */
                      for (x = number_pixels; x != 0; --x)
                        {
                          BitStreamMSBWrite(&stream,quantum_size,GetRedSample(p));
                          BitStreamMSBWrite(&stream,quantum_size,GetGreenSample(p));
                          BitStreamMSBWrite(&stream,quantum_size,GetBlueSample(p));
                          p++;
                        }
                    }
                  else if (QuantumDepth >  sample_bits)
                    {
                      /* Scale down */
                      for (x = number_pixels; x != 0; --x)
                        {
                          BitStreamMSBWrite(&stream,quantum_size,GetRedSample(p)/unsigned_scale);
                          BitStreamMSBWrite(&stream,quantum_size,GetGreenSample(p)/unsigned_scale);
                          BitStreamMSBWrite(&stream,quantum_size,GetBlueSample(p)/unsigned_scale);
                          p++;
                        }
                    }
                  else
                    {
                      /* Scale up */
                      for (x = number_pixels; x != 0; --x)
                        {
                          BitStreamMSBWrite(&stream,quantum_size,GetRedSample(p)*unsigned_scale);
                          BitStreamMSBWrite(&stream,quantum_size,GetGreenSample(p)*unsigned_scale);
                          BitStreamMSBWrite(&stream,quantum_size,GetBlueSample(p)*unsigned_scale);
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
        break;
      }
    case RGBAQuantum:
      {
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
                  
                  BitStreamInitializeWrite(&stream,q);
                  if(QuantumDepth == sample_bits)
                    {
                      /* Unity scale */
                      for (x = number_pixels; x != 0; --x)
                        {
                          BitStreamMSBWrite(&stream,quantum_size,GetRedSample(p));
                          BitStreamMSBWrite(&stream,quantum_size,GetGreenSample(p));
                          BitStreamMSBWrite(&stream,quantum_size,GetBlueSample(p));
                          BitStreamMSBWrite(&stream,quantum_size,MaxRGB-GetOpacitySample(p));
                          p++;
                        }
                    }
                  else if (QuantumDepth >  sample_bits)
                    {
                      /* Scale down */
                      for (x = number_pixels; x != 0; --x)
                        {
                          BitStreamMSBWrite(&stream,quantum_size,GetRedSample(p)/unsigned_scale);
                          BitStreamMSBWrite(&stream,quantum_size,GetGreenSample(p)/unsigned_scale);
                          BitStreamMSBWrite(&stream,quantum_size,GetBlueSample(p)/unsigned_scale);
                          BitStreamMSBWrite(&stream,quantum_size,
                                            (MaxRGB-GetOpacitySample(p))/unsigned_scale);
                          p++;
                        }
                    }
                  else
                    {
                      /* Scale up */
                      for (x = number_pixels; x != 0; --x)
                        {
                          BitStreamMSBWrite(&stream,quantum_size,GetRedSample(p)*unsigned_scale);
                          BitStreamMSBWrite(&stream,quantum_size,GetGreenSample(p)*unsigned_scale);
                          BitStreamMSBWrite(&stream,quantum_size,GetBlueSample(p)*unsigned_scale);
                          BitStreamMSBWrite(&stream,quantum_size,
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
        break;
      }
    case CMYKQuantum:
      {
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
                  
                  BitStreamInitializeWrite(&stream,q);
                  if(QuantumDepth == sample_bits)
                    {
                      /* Unity scale */
                      for (x = number_pixels; x != 0; --x)
                        {
                          BitStreamMSBWrite(&stream,quantum_size,GetCyanSample(p));
                          BitStreamMSBWrite(&stream,quantum_size,GetMagentaSample(p));
                          BitStreamMSBWrite(&stream,quantum_size,GetYellowSample(p));
                          BitStreamMSBWrite(&stream,quantum_size,GetBlackSample(p));
                          p++;
                        }
                    }
                  else if (QuantumDepth >  sample_bits)
                    {
                      /* Scale down */
                      for (x = number_pixels; x != 0; --x)
                        {
                          BitStreamMSBWrite(&stream,quantum_size,GetCyanSample(p)/unsigned_scale);
                          BitStreamMSBWrite(&stream,quantum_size,GetMagentaSample(p)/unsigned_scale);
                          BitStreamMSBWrite(&stream,quantum_size,GetYellowSample(p)/unsigned_scale);
                          BitStreamMSBWrite(&stream,quantum_size,GetBlackSample(p)/unsigned_scale);
                          p++;
                        }
                    }
                  else
                    {
                      /* Scale up */
                      for (x = number_pixels; x != 0; --x)
                        {
                          BitStreamMSBWrite(&stream,quantum_size,GetCyanSample(p)*unsigned_scale);
                          BitStreamMSBWrite(&stream,quantum_size,GetMagentaSample(p)*unsigned_scale);
                          BitStreamMSBWrite(&stream,quantum_size,GetYellowSample(p)*unsigned_scale);
                          BitStreamMSBWrite(&stream,quantum_size,GetBlackSample(p)*unsigned_scale);
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
        break;
      }
    case CMYKAQuantum:
      {
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
                  
                  BitStreamInitializeWrite(&stream,q);
                  if( QuantumDepth == sample_bits)
                    {
                      /* Unity scale */
                      for (x = number_pixels; x != 0; --x)
                        {
                          BitStreamMSBWrite(&stream,quantum_size,GetCyanSample(p));
                          BitStreamMSBWrite(&stream,quantum_size,GetMagentaSample(p));
                          BitStreamMSBWrite(&stream,quantum_size,GetYellowSample(p));
                          BitStreamMSBWrite(&stream,quantum_size,GetBlackSample(p));
                          BitStreamMSBWrite(&stream,quantum_size,MaxRGB-*indexes);
                          indexes++;
                          p++;
                        }
                    }
                  else if (QuantumDepth >  sample_bits)
                    {
                      /* Scale down */
                      for (x = number_pixels; x != 0; --x)
                        {
                          BitStreamMSBWrite(&stream,quantum_size,GetCyanSample(p)/unsigned_scale);
                          BitStreamMSBWrite(&stream,quantum_size,GetMagentaSample(p)/unsigned_scale);
                          BitStreamMSBWrite(&stream,quantum_size,GetYellowSample(p)/unsigned_scale);
                          BitStreamMSBWrite(&stream,quantum_size,GetBlackSample(p)/unsigned_scale);
                          BitStreamMSBWrite(&stream,quantum_size,
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
                          BitStreamMSBWrite(&stream,quantum_size,GetCyanSample(p)*unsigned_scale);
                          BitStreamMSBWrite(&stream,quantum_size,GetMagentaSample(p)*unsigned_scale);
                          BitStreamMSBWrite(&stream,quantum_size,GetYellowSample(p)*unsigned_scale);
                          BitStreamMSBWrite(&stream,quantum_size,GetBlackSample(p)*unsigned_scale);
                          BitStreamMSBWrite(&stream,quantum_size,
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
        break;
      }
    }
  /*
    Appended any requested padding bytes.
  */
  if (options)
    for (x = options->pad_bytes; x != 0; --x)
      *q++=options->pad_value;

  /*
    Collect export info.
  */
  if (export_info)
    {
      export_info->bytes_exported=(q-destination);
    }

  return(MagickPass);
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
MagickExport MagickPassFail ImportViewPixelArea(ViewInfo *view,
  const QuantumType quantum_type,const unsigned int quantum_size,
  const unsigned char *source,const ImportPixelAreaOptions *options,
  ImportPixelAreaInfo *import_info)
{
  Image
    *image;

  register const unsigned char
    *p;

  register unsigned int
    index,
    unsigned_scale = 1U,
    unsigned_value;

  register IndexPacket
    *indexes;

  register unsigned long
    x;

  register PixelPacket
    *q;

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
    double_scale,
    double_value;

  EndianType
    endian=MSBEndian;

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
  p=source;
  q=AccessCacheViewPixels(view);
  indexes=GetCacheViewIndexes(view);
  switch (quantum_type)
    {
    case IndexQuantum:
      {
        assert(image->colors <= MaxColormapSize);
        assert(image->colors != 0);
        assert(indexes != (IndexPacket *) NULL);
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
                  
                  BitStreamInitializeRead(&stream,p);
                  for (x = number_pixels; x != 0; --x)
                    {
                      index=BitStreamMSBRead(&stream,quantum_size);
                      VerifyColormapIndex(image,index);
                      *indexes++=index;
                      *q++=image->colormap[index];
                    }
                  break;
                }
              }
          }
        break;
      }
    case IndexAlphaQuantum:
      {
        assert(image->colors <= MaxColormapSize);
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
                  
                  BitStreamInitializeRead(&stream,p);
                  for (x = number_pixels; x != 0; --x)
                    {
                      index=BitStreamMSBRead(&stream,quantum_size);
                      VerifyColormapIndex(image,index);
                      *indexes++=index;
                      *q=image->colormap[index];
                      
                      unsigned_value=BitStreamMSBRead(&stream,quantum_size);
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
        break;
      }
    case GrayQuantum:
      {
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
                      
                      BitStreamInitializeRead(&stream,p);
                      for (x = number_pixels; x != 0; --x)
                        {
                          unsigned_value=BitStreamMSBRead(&stream,quantum_size);
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
                
                      BitStreamInitializeRead(&stream,p);
                      for (x = number_pixels; x != 0; --x)
                        {
                          index=BitStreamMSBRead(&stream,quantum_size);
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
        break;
      }
    case GrayAlphaQuantum:
      {
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
                      
                      BitStreamInitializeRead(&stream,p);
                      for (x = number_pixels; x != 0; --x)
                        {
                          unsigned_value=BitStreamMSBRead(&stream,quantum_size);
                          if (QuantumDepth >  sample_bits)
                            unsigned_value *= unsigned_scale;
                          else if (QuantumDepth <  sample_bits)
                            unsigned_value /= unsigned_scale;
                          if (grayscale_miniswhite)
                            unsigned_value = MaxRGB-unsigned_value;
                          SetGraySample(q,unsigned_value);

                          unsigned_value=BitStreamMSBRead(&stream,quantum_size);
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
            
                    BitStreamInitializeRead(&stream,p);
                    for (x = number_pixels; x != 0; --x)
                      {
                        index=BitStreamMSBRead(&stream,quantum_size);
                        index /= indexes_scale;
                        VerifyColormapIndex(image,index);
                        if (grayscale_miniswhite)
                          index=(image->colors-1)-index;
                        *indexes++=index;
                        *q=image->colormap[index];

                        unsigned_value=BitStreamMSBRead(&stream,quantum_size);
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
        break;
      }
    case RedQuantum:
    case CyanQuantum:
      {
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
                  
                  BitStreamInitializeRead(&stream,p);
                  if (QuantumDepth >=  sample_bits)
                    {
                      /* Scale up */
                      for (x = number_pixels; x != 0; --x)
                        {
                          SetRedSample(q,BitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
                          q++;
                        }
                    }
                  else
                    {
                      /* Scale down */
                      for (x = number_pixels; x != 0; --x)
                        {
                          SetRedSample(q,BitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
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
        break;
      }
    case GreenQuantum:
    case MagentaQuantum:
      {
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
                  
                  BitStreamInitializeRead(&stream,p);
                  if (QuantumDepth >=  sample_bits)
                    {
                      /* Scale up */
                      for (x = number_pixels; x != 0; --x)
                        {
                          SetGreenSample(q,BitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
                          q++;
                        }
                    }
                  else
                    {
                      /* Scale down */
                      for (x = number_pixels; x != 0; --x)
                        {
                          SetGreenSample(q,BitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
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
        break;
      }
    case BlueQuantum:
    case YellowQuantum:
      {
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
                  
                  BitStreamInitializeRead(&stream,p);
                  if (QuantumDepth >=  sample_bits)
                    {
                      /* Scale up */
                      for (x = number_pixels; x != 0; --x)
                        {
                          SetBlueSample(q,BitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
                          q++;
                        }
                    }
                  else
                    {
                      /* Scale down */
                      for (x = number_pixels; x != 0; --x)
                        {
                          SetBlueSample(q,BitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
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
        break;
      }
    case AlphaQuantum:
      {
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
                
                    BitStreamInitializeRead(&stream,p);
                    if (QuantumDepth >=  sample_bits)
                      {
                        /* Scale up */
                        for (x = number_pixels; x != 0; --x)
                          {
                            *indexes++=(IndexPacket) MaxRGB-BitStreamMSBRead(&stream,quantum_size)*unsigned_scale;
                          }
                      }
                    else
                      {
                        /* Scale down */
                        for (x = number_pixels; x != 0; --x)
                          {
                            *indexes++=(IndexPacket) MaxRGB-BitStreamMSBRead(&stream,quantum_size)/unsigned_scale;
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
            break;
          }

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

                BitStreamInitializeRead(&stream,p);
                if (QuantumDepth >=  sample_bits)
                  {
                    /* Scale up */
                    for (x = number_pixels; x != 0; --x)
                      {
                        SetOpacitySample(q,MaxRGB-BitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
                        q++;
                      }
                  }
                else
                  {
                    /* Scale down */
                    for (x = number_pixels; x != 0; --x)
                      {
                        SetOpacitySample(q,MaxRGB-BitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
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
        break;
      }
    case BlackQuantum:
      {
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
                  
                  BitStreamInitializeRead(&stream,p);
                  if (QuantumDepth >=  sample_bits)
                    {
                      /* Scale up */
                      for (x = number_pixels; x != 0; --x)
                        {
                          SetBlackSample(q,BitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
                          q++;
                        }
                    }
                  else
                    {
                      /* Scale down */
                      for (x = number_pixels; x != 0; --x)
                        {
                          SetBlackSample(q,BitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
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
        break;
      }
    case RGBQuantum:
    default:
      {
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
                  
                  BitStreamInitializeRead(&stream,p);
                  if (QuantumDepth >=  sample_bits)
                    {
                      /* Scale up */
                      for (x = number_pixels; x != 0; --x)
                        {
                          SetRedSample(q,BitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
                          SetGreenSample(q,BitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
                          SetBlueSample(q,BitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
                          SetOpacitySample(q,OpaqueOpacity);
                          q++;
                        }
                    }
                  else if (QuantumDepth <  sample_bits)
                    {
                      /* Scale down */
                      for (x = number_pixels; x != 0; --x)
                        {
                          SetRedSample(q,BitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
                          SetGreenSample(q,BitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
                          SetBlueSample(q,BitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
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
        break;
      }
    case RGBAQuantum:
      {
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
                  
                  BitStreamInitializeRead(&stream,p);
                  if (QuantumDepth >=  sample_bits)
                    {
                      /* Scale up */
                      for (x = number_pixels; x != 0; --x)
                        {
                          SetRedSample(q,BitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
                          SetGreenSample(q,BitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
                          SetBlueSample(q,BitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
                          SetOpacitySample(q,MaxRGB-BitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
                          q++;
                        }
                    }
                  else if (QuantumDepth <  sample_bits)
                    {
                      /* Scale down */
                      for (x = number_pixels; x != 0; --x)
                        {
                          SetRedSample(q,BitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
                          SetGreenSample(q,BitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
                          SetBlueSample(q,BitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
                          SetOpacitySample(q,MaxRGB-BitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
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
        break;
      }
    case CMYKQuantum:
      {
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
                  
                  BitStreamInitializeRead(&stream,p);
                  if (QuantumDepth >=  sample_bits)
                    {
                      /* Scale up */
                      for (x = number_pixels; x != 0; --x)
                        {
                          SetCyanSample(q,BitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
                          SetMagentaSample(q,BitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
                          SetYellowSample(q,BitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
                          SetBlackSample(q,BitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
                          *indexes++=OpaqueOpacity;
                          q++;
                        }
                    }
                  else if (QuantumDepth <  sample_bits)
                    {
                      /* Scale down */
                      for (x = number_pixels; x != 0; --x)
                        {
                          SetCyanSample(q,BitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
                          SetMagentaSample(q,BitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
                          SetYellowSample(q,BitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
                          SetBlackSample(q,BitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
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
        break;
      }
    case CMYKAQuantum:
      {
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
                  
                  BitStreamInitializeRead(&stream,p);
                  if (QuantumDepth >=  sample_bits)
                    {
                      /* Scale up */
                      for (x = number_pixels; x != 0; --x)
                        {
                          SetCyanSample(q,BitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
                          SetMagentaSample(q,BitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
                          SetYellowSample(q,BitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
                          SetBlackSample(q,BitStreamMSBRead(&stream,quantum_size)*unsigned_scale);
                          *indexes++=(IndexPacket) MaxRGB-BitStreamMSBRead(&stream,quantum_size)*unsigned_scale;
                          q++;
                        }
                    }
                  else if (QuantumDepth <  sample_bits)
                    {
                      /* Scale down */
                      for (x = number_pixels; x != 0; --x)
                        {
                          SetCyanSample(q,BitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
                          SetMagentaSample(q,BitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
                          SetYellowSample(q,BitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
                          SetBlackSample(q,BitStreamMSBRead(&stream,quantum_size)/unsigned_scale);
                          *indexes++=(IndexPacket) MaxRGB-BitStreamMSBRead(&stream,quantum_size)/unsigned_scale;
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
        break;
      }
    case CIEXYZQuantum:
      {
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
        break;
      }
    case CIEYQuantum:
      {
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
        break;
      }
    }

  if (import_info)
    {
      import_info->bytes_imported=p-source;
    }

  return(True);
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

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k F i n d R a w I m a g e M i n M a x                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickFindRawImageMinMax() obtains the minimum and maximum sample values
%  for a raw image.  The image blob must already be open with its current
%  seek offset pointing to the start of the raw data.  The seek offset is
%  restored when this function returns.  File data is processed on a 
%  scanline basis in order to minimize memory consumption.  The purpose of
%  this function is to support pre-scanning a raw image to find its maximum
%  values so that appropriate scaling may be applied when the data is read a
%  second time. MagickFail is returned if a problem occurs while scanning the
%  data.
%
%  The format of the MagickFindRawImageMinMax method is:
%
%      MagickPassFail MagickFindRawImageMinMax(Image *image, EndianType endian,
%                   unsigned long width, unsigned long height,StorageType type,
%                   unsigned scanline_octets, void *scanline_buffer,
%                   double *min, double *max)
%
%  A description of each parameter follows:
%
%    o image: Pointer to an image with an already open blob, with seek
%         offset pointing to raw data.
%
%    o endian: Endian order of raw data (LSBEndian or MSBEndian)
%
%    o width: Number of raw samples in a scanline.
%
%    o height: Number of scanlines to process.
%
%    o type: Raw data storage type.
%
%    o scanline_octets: Number of octets to read per scanline.
%
%    o scanline_buffer: Working buffer for scanlines.  Allocation size
%         must at least be enough to contain scanline octets.
%
%    o min: Pointer to double value to update with minimum value.
%
%    o max: Pointer to double value to update with maximum value.
%
*/
#define MagickFindMinMax(status,image,read_func,basic_type,scanline_octets,scanline_buffer,min,max) \
  {                                                                     \
    unsigned long                                                       \
      y;                                                                \
                                                                        \
    for (y = 0; y < height; y++)                                        \
      {                                                                 \
        unsigned long                                                   \
          x;                                                            \
                                                                        \
        basic_type                                                      \
          *scanline;                                                    \
                                                                        \
        scanline=(basic_type *) scanline_buffer;                        \
        if ((read_func)(image, scanline_octets, scanline) !=            \
            scanline_octets)                                            \
          {                                                             \
            status=MagickFail;                                          \
            break;                                                      \
          }                                                             \
                                                                        \
        if (y == 0)                                                     \
          *min = *max = (double) scanline[0];                           \
                                                                        \
        for (x = 0; x < width; x++)                                     \
          {                                                             \
            if (*min > (double) scanline[x])                            \
              *min = (double) scanline[x];                              \
            if (*max < (double) scanline[x])                            \
              *max = (double) scanline[x];                              \
          }                                                             \
      }                                                                 \
  }

MagickPassFail
MagickFindRawImageMinMax(Image *image, EndianType endian,
                         unsigned long width, unsigned long height,StorageType type,
                         unsigned scanline_octets, void *scanline_buffer,
                         double *min, double *max)
{
  magick_off_t
    filepos;

  MagickPassFail
    status;

  *min=0.0;
  *max=1.0;
  status=MagickFail;

  filepos = TellBlob(image);

  if (filepos >= 0)
    {
      status=MagickPass;
      switch (type)
        {
        case CharPixel:
          {
            size_t (*read_func)(Image * image, size_t octets, void *data);

            read_func = ReadBlob;
            
            MagickFindMinMax(status,image,read_func,char,scanline_octets,
                             scanline_buffer,min,max)
              break;
          }
        case ShortPixel:
          {
            size_t (*read_func)(Image * image, size_t octets, magick_uint16_t *data);

            if (endian == LSBEndian)
              read_func = ReadBlobLSBShorts;
            else
              read_func = ReadBlobMSBShorts;

            MagickFindMinMax(status,image,read_func,magick_uint16_t,scanline_octets,
                             scanline_buffer,min,max);
            break;
          }
        case IntegerPixel:
        case LongPixel:
          {
            size_t (*read_func)(Image * image, size_t octets, magick_uint32_t *data);

            if (endian == LSBEndian)
              read_func = ReadBlobLSBLongs;
            else
              read_func = ReadBlobMSBLongs;

            MagickFindMinMax(status,image,read_func,magick_uint32_t,scanline_octets,
                             scanline_buffer,min,max);
            break;
          }
        case FloatPixel:
          {
            size_t (*read_func)(Image * image, size_t octets, float *data);

            if (endian == LSBEndian)
              read_func = ReadBlobLSBFloats;
            else
              read_func = ReadBlobMSBFloats;

            MagickFindMinMax(status,image,read_func,float,scanline_octets,
                             scanline_buffer,min,max);
            break;
          }
        case DoublePixel:
          {
            size_t (*read_func)(Image * image, size_t octets, double *data);

            if (endian == LSBEndian)
              read_func = ReadBlobLSBDoubles;
            else
              read_func = ReadBlobMSBDoubles;

            MagickFindMinMax(status,image,read_func,double,scanline_octets,
                             scanline_buffer,min,max);
            break;
          }
        }

      (void) SeekBlob(image, filepos, SEEK_SET);
    }

  return status;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i n g I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PingImage() returns all the attributes of an image or image sequence
%  except for the pixels.  It is much faster and consumes far less memory
%  than ReadImage().  On failure, a NULL image is returned and exception
%  describes the reason for the failure.
%
%  The format of the PingImage method is:
%
%      Image *PingImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: Ping the image defined by the file or filename members of
%      this structure.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *PingImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *image;

  ImageInfo
    *clone_info;

  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  SetExceptionInfo(exception,UndefinedException);
  clone_info=CloneImageInfo(image_info);
  clone_info->ping=True;
  image=ReadImage(clone_info,exception);
  DestroyImageInfo(clone_info);
  /*
    Intentionally restart timer if ping is requested since timing ping
    is meaningless and misleading.
  */
  if (image != (Image *) NULL)
    GetTimerInfo(&image->timer);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReadImage() reads an image or image sequence from a file or file handle.
%  The method returns a NULL if there is a memory shortage or if the image
%  cannot be read.  On failure, a NULL image is returned and exception
%  describes the reason for the failure.
%
%  The format of the ReadImage method is:
%
%      Image *ReadImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: Read the image defined by the file or filename members of
%      this structure.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static void RemoveTemporaryInputFile(ImageInfo *image_info)
{
  int
    filename_length;

  /*
    Remove normal file name.
  */
  if(!LiberateTemporaryFile(image_info->filename))
    (void) remove(image_info->filename);

  /*
    Remove a .cache file corresponding to a .mpc file.
    This stupidity is necessary because MPC "files" are comprised of two
    separate files.
  */
  filename_length=strlen(image_info->filename);
  if ((filename_length > 4) &&
      (LocaleCompare(image_info->filename+filename_length-4,".mpc") == 0))
    {
      char remove_name[MaxTextExtent];
      (void) strcpy(remove_name,image_info->filename);
      remove_name[filename_length-4]=0;
      (void) strcat(remove_name,".cache");
      (void) printf("removing %s\n", remove_name);
      (void) remove(remove_name);
    }
  else if (LocaleCompare(image_info->magick,"mpc") == 0)
    {
      char remove_name[MaxTextExtent];
      (void) strcpy(remove_name,image_info->filename);
      (void) strcat(remove_name,".cache");
      (void) printf("removing %s\n", remove_name);
      (void) remove(remove_name);
    }

  errno=0;
}

MagickExport Image *ReadImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  char
    filename[MaxTextExtent],
    magick[MaxTextExtent];

  const DelegateInfo
    *delegate_info;

  const MagickInfo
    *magick_info;

  Image
    *image,
    *next;

  ImageInfo
    *clone_info;

  /*
    Determine image type from filename prefix or suffix (e.g. image.jpg).
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image_info->filename != (char *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  SetExceptionInfo(exception,UndefinedException);
  if (*image_info->filename == '@')
    return(ReadImages(image_info,exception));
  clone_info=CloneImageInfo(image_info);

  /*
    Obtain file magick from filename
  */
  (void) SetImageInfo(clone_info,False,exception);
  (void) LogMagickEvent(BlobEvent,GetMagickModule(),
                        "Magick=%s, Filename=%s", clone_info->magick,clone_info->filename);
  (void) strlcpy(filename,clone_info->filename,MaxTextExtent);
  (void) strlcpy(magick,clone_info->magick,MaxTextExtent);
  /*
    Call appropriate image reader based on image type.
  */
  {
    ExceptionInfo
      module_exception,
      delegate_exception;
    
    GetExceptionInfo(&module_exception);
    GetExceptionInfo(&delegate_exception);
    magick_info=GetMagickInfo(clone_info->magick,&module_exception);
    delegate_info=(const DelegateInfo *) NULL;
    if ((magick_info == (const MagickInfo *) NULL) ||
        (magick_info->decoder == NULL))
      delegate_info=GetDelegateInfo(clone_info->magick,(char *) NULL,&delegate_exception);
    
    if (((magick_info == (const MagickInfo *) NULL) ||
         (magick_info->decoder == NULL)) &&
        ((delegate_info == (const DelegateInfo *) NULL) ||
         (delegate_info->decode == NULL)))
      {
        /*
          Module loader ConfigureError errors are intentionally
          ignored here in order to provide the user with familiar "no
          delegate" error messages.  This may be re-considered later.
        */
        if ((module_exception.severity != UndefinedException) &&
            (module_exception.severity != ConfigureError))
          CopyException(exception,&module_exception);
        else if (delegate_exception.severity != UndefinedException)
          CopyException(exception,&delegate_exception);
        else
          {
            /*
              Try to choose a useful error type
            */
            if (clone_info->filename[0] == 0)
              {
                errno=0;
                ThrowException(exception,MissingDelegateError,
                               NoDecodeDelegateForThisImageFormat,clone_info->magick);
              }
            else if (IsAccessibleAndNotEmpty(clone_info->filename))
              {
                errno=0;
                ThrowException(exception,MissingDelegateError,
                               NoDecodeDelegateForThisImageFormat,clone_info->filename);
              }
            else
              {
              ThrowException(exception,FileOpenError,UnableToOpenFile,
                             clone_info->filename);
              }
          }
        DestroyExceptionInfo(&module_exception);
        DestroyExceptionInfo(&delegate_exception);
        if (clone_info->temporary)
          RemoveTemporaryInputFile(clone_info);
        DestroyImageInfo(clone_info);
        return((Image *) NULL);
      }
    
    DestroyExceptionInfo(&module_exception);
    DestroyExceptionInfo(&delegate_exception);
  }

  if ((magick_info != (const MagickInfo *) NULL) &&
      (magick_info->seekable_stream == MagickTrue))
    {
      unsigned int
        status;

      image=AllocateImage(clone_info);
      if (image == (Image *) NULL)
        return(False);
      (void) strlcpy(image->filename,clone_info->filename,MaxTextExtent);
      status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
      if (status == False)
        {
          DestroyImage(image);
          return(False);
        }
      if (!BlobIsSeekable(image))
        {
          /*
            Coder requires a random access stream.
          */
          if(!AcquireTemporaryFileName(clone_info->filename))
            {
              ThrowException(exception,FileOpenError,
                UnableToCreateTemporaryFile,clone_info->filename);
              CloseBlob(image);
              DestroyImageInfo(clone_info);
              DestroyImage(image);
              return((Image *) NULL);
            }
          (void) ImageToFile(image,clone_info->filename,exception);
          clone_info->temporary=True;
        }
      CloseBlob(image);
      DestroyImage(image);
    }
  image=(Image *) NULL;
  if ((magick_info != (const MagickInfo *) NULL) &&
      (magick_info->decoder != NULL))
    {
      if (!magick_info->thread_support)
        AcquireSemaphoreInfo(&constitute_semaphore);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "Invoking \"%.1024s\" decoder (%.1024s)",magick_info->name,
        magick_info->description);
      image=(magick_info->decoder)(clone_info,exception);
      if (!magick_info->thread_support)
        LiberateSemaphoreInfo(&constitute_semaphore);

      if (image != (Image *) NULL)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "Returned from \"%.1024s\" decoder: monochrome=%s grayscale=%s class=%s colorspace=%s",
                              magick_info->name,
                              MagickBoolToString(image->is_monochrome),
                              MagickBoolToString(image->is_grayscale),
                              ClassTypeToString(image->storage_class),
                              ColorspaceTypeToString(image->colorspace));
      else
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "Returned from \"%.1024s\" decoder, returned image is NULL!",
                              magick_info->name);

      /*
        Deal with errors in the image which were not properly reported
        to exception.  If there is an exception at error level, then
        destroy image so that bad image is not consumed by user.
      */
      if (image != (Image *) NULL)
        {
          GetImageException(image,exception);
          if (exception->severity >= ErrorException)
            {
              DestroyImageList(image);
              image=(Image *) NULL;
            }
        }
    }
  else
    {
      if (delegate_info == (const DelegateInfo *) NULL)
        {
          if (clone_info->temporary)
            RemoveTemporaryInputFile(clone_info);
          DestroyImageInfo(clone_info);
          return((Image *) NULL);
        }
      /*
        Let our decoding delegate process the image.
      */
      image=AllocateImage(clone_info);
      if (image == (Image *) NULL)
        {
          DestroyImageInfo(clone_info);
          return((Image *) NULL);
        }
      (void) strlcpy(image->filename,clone_info->filename,MaxTextExtent);
      if(!AcquireTemporaryFileName(clone_info->filename))
        {
          ThrowException(exception,FileOpenError,UnableToCreateTemporaryFile,
            clone_info->filename);
          DestroyImageInfo(clone_info);
          return((Image *) NULL);
        }
      (void) InvokeDelegate(clone_info,image,clone_info->magick,(char *) NULL,
        exception);
      DestroyImageList(image);
      image=(Image *) NULL;
      clone_info->temporary=True;
      (void) SetImageInfo(clone_info,False,exception);
      magick_info=GetMagickInfo(clone_info->magick,exception);
      /*
        If there is no magick info entry for this format, or there is
        no decoder for the format, or an error is reported, then
        attempt to return a reasonable error report.
      */
      if ((magick_info == (const MagickInfo *) NULL) ||
          (magick_info->decoder == NULL) ||
          (exception->severity != UndefinedException))
        {
          if (exception->severity == UndefinedException)
          {
            if (IsAccessibleAndNotEmpty(clone_info->filename))
              ThrowException(exception,MissingDelegateError,
                NoDecodeDelegateForThisImageFormat,clone_info->filename);
            else
              ThrowException(exception,FileOpenError,UnableToOpenFile,
                clone_info->filename);
          }
          if (clone_info->temporary)
            RemoveTemporaryInputFile(clone_info);
          DestroyImageInfo(clone_info);
          return((Image *) NULL);
        }
      /*
        Invoke decoder for format
      */
      if (!magick_info->thread_support)
        AcquireSemaphoreInfo(&constitute_semaphore);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "Invoking \"%.1024s\" decoder (%.1024s)",magick_info->name,
        magick_info->description);
      image=(magick_info->decoder)(clone_info,exception);
      if (!magick_info->thread_support)
        LiberateSemaphoreInfo(&constitute_semaphore);

      if (image != (Image *) NULL)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "Returned from \"%.1024s\" decoder: monochrome=%s grayscale=%s class=%s colorspace=%s",
                              magick_info->name,
                              MagickBoolToString(image->is_monochrome),
                              MagickBoolToString(image->is_grayscale),
                              ClassTypeToString(image->storage_class),
                              ColorspaceTypeToString(image->colorspace));
      else
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "Returned from \"%.1024s\" decoder: returned image is NULL!",
                              magick_info->name);

      /*
        Deal with errors in the image which were not properly reported
        to exception.  If there is an exception at error level, then
        destroy image so that bad image is not consumed by user.
      */
      if (image != (Image *) NULL)
        {
          GetImageException(image,exception);
          if (exception->severity >= ErrorException)
            {
              DestroyImageList(image);
              image=(Image *) NULL;
            }
        }

      /*
        Restore original input file magick in case read is from a
        temporary file prepared by an external delegate.  The user
        will expect that the format reported is that of the input
        file.
      */
      if (image != (Image *) NULL)
        (void) strlcpy(image->magick,magick,MaxTextExtent);
    }
  if (clone_info->temporary)
    {
      RemoveTemporaryInputFile(clone_info);
      clone_info->temporary=False;
      if (image != (Image *) NULL)
        (void) strlcpy(image->filename,filename,MaxTextExtent);
    }
  if (image == (Image *) NULL)
    {
      DestroyImageInfo(clone_info);
      return(image);
    }
  if (GetBlobTemporary(image))
    RemoveTemporaryInputFile(clone_info);
  if ((image->next != (Image *) NULL) && IsSubimage(clone_info->tile,False))
    {
      char
        *q;

      Image
        *clone_image,
        *subimages;

      long
        quantum;

      register char
        *p;

      register long
        i;

      unsigned long
        first,
        last;

      /*
        User specified subimages (e.g. image.miff[1,3-5,7-6,2]).
      */
      subimages=NewImageList();
      p=clone_info->tile;
      for (q=p; *q != '\0'; p++)
      {
        while (isspace((int) *p) || (*p == ','))
          p++;
        first=strtol(p,&q,10);
        last=first;
        while (isspace((int) *q))
          q++;
        if (*q == '-')
          last=strtol(q+1,&q,10);
        quantum=first > last ? -1 : 1;
        for (p=q; first != (last+quantum); first+=quantum)
        {
          i=0;
          for (next=image; next != (Image *) NULL; next=next->next)
          {
            if (next->scene != 0)
              i=(long) next->scene;
            if (i != (long) first)
              {
                i++;
                continue;
              }
            clone_image=CloneImage(next,0,0,True,exception);
            if (clone_image == (Image *) NULL)
              break;
            AppendImageToList(&subimages,clone_image);
            i++;
          }
        }
      }
      if (subimages == (Image *) NULL)
        ThrowException(exception,OptionError,
                       SubimageSpecificationReturnsNoImages,
                       clone_info->filename);
      else
        {
          while (subimages->previous != (Image *) NULL)
            subimages=subimages->previous;
          DestroyImageList(image);
          image=subimages;
        }
    }
  if (GetBlobStatus(image))
    {
      ThrowException(exception,CorruptImageError,
                     AnErrorHasOccurredReadingFromFile,
                     clone_info->filename);
      DestroyImageInfo(clone_info);
      return((Image *) NULL);
    }
  for (next=image; next; next=next->next)
  {
#if 0
    /*
      Apply user-requested colorspace setting.  This allows the user
      to override the default colorspace for the image type.
      FIXME: Does not work yet.
    */
    {
      const ImageAttribute
        *attribute;
      
/*       if ((image_info->attributes) && */
/*           (attribute=GetImageAttribute(image_info->attributes,"colorspace-override"))) */
      if ((attribute=GetImageAttribute(image,"colorspace-override")))
        {
          next->colorspace = StringToColorspaceType(attribute->value);
          (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                                "Colorspace explicitly set to %s",
                                ColorspaceTypeToString(image->colorspace));
        }
    }
#endif
    if (next->storage_class == PseudoClass)
      {
        /*
          Check and cache monochrome and grayscale status
        */
        (void) IsMonochromeImage(next,exception);
        if (next->is_monochrome)
          next->is_grayscale=True;
        else
          (void) IsGrayImage(next,exception);
      }
    next->taint=False;
    (void) strlcpy(next->magick_filename,filename,MaxTextExtent);
    if (GetBlobTemporary(image))
      (void) strlcpy(next->filename,filename,MaxTextExtent);
    if (next->magick_columns == 0)
      next->magick_columns=next->columns;
    if (next->magick_rows == 0)
      next->magick_rows=next->rows;
  }
  DestroyImageInfo(clone_info);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   R e a d I m a g e s                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReadImages() reads a list of image names from a file and then returns the
%  images as a linked list.
%
%  The format of the ReadImage method is:
%
%      Image *ReadImages(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: Method ReadImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: The list of filenames are defined in the filename member of
%      this structure.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static Image *ReadImages(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    *command,
    **images;

  Image
    *image;

  ImageInfo
    *clone_info;

  int
    number_images;

  register Image
    *next;

  register long
    i;

  size_t
    length;

  /*
    Read image list from a file.
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  command=(char *) FileToBlob(image_info->filename+1,&length,exception);
  if (command == (char *) NULL)
    return((Image *) NULL);
  Strip(command);
  images=StringToArgv(command,&number_images);
  if (images == (char **) NULL)
    return((Image *) NULL);
  MagickFreeMemory(command);
  /*
    Read the images into a linked list.
  */
  image=(Image *) NULL;
  clone_info=CloneImageInfo(image_info);
  for (i=1; i < number_images; i++)
  {
    (void) strlcpy(clone_info->filename,images[i],MaxTextExtent);
    next=ReadImage(clone_info,exception);
    if (next == (Image *) NULL)
      continue;
    if (image == (Image *) NULL)
      image=next;
    else
      {
        register Image
          *q;

        /*
          Link image into image list.
        */
        for (q=image; q->next != (Image *) NULL; q=q->next);
        next->previous=q;
        q->next=next;
      }
  }
  DestroyImageInfo(clone_info);
  for (i=1; i < number_images; i++)
    MagickFreeMemory(images[i]);
  MagickFreeMemory(images);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d I n l i n e I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReadInlineImage() reads a Base64-encoded inline image or image sequence.
%  The method returns a NULL if there is a memory shortage or if the image
%  cannot be read.  On failure, a NULL image is returned and exception
%  describes the reason for the failure.
%
%  The format of the ReadInlineImage method is:
%
%      Image *ReadInlineImage(const ImageInfo *image_info,const char *content,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o content: The image encoded in Base64.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *ReadInlineImage(const ImageInfo *image_info,
  const char *content,ExceptionInfo *exception)
{
  Image
    *image;

  MonitorHandler
    handler;

  unsigned char
    *blob;

  size_t
    length;

  register const char
    *p;

  SetExceptionInfo(exception,UndefinedException);
  image=(Image *) NULL;
  for (p=content; (*p != ',') && (*p != '\0'); p++);
  if (*p == '\0')
    ThrowReaderException(CorruptImageError,CorruptImage,image);
  p++;
  blob=Base64Decode(p,&length);
  if (length == 0)
    ThrowReaderException(CorruptImageError,CorruptImage,image);
  handler=SetMonitorHandler((MonitorHandler) NULL);
  image=BlobToImage(image_info,blob,length,exception);
  (void) SetMonitorHandler(handler);
  MagickFreeMemory(blob);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Use Write() to write an image or an image sequence to a file or
%  filehandle.  If writing to a file on disk, the name is defined by the
%  filename member of the image structure.  Write() returns 0 is there is a
%  memory shortage or if the image cannot be written.  Check the exception
%  member of image to determine the cause for any failure.
%
%  The format of the WriteImage method is:
%
%      unsigned int WriteImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o image: The image.
%
%
*/
MagickExport unsigned int WriteImage(const ImageInfo *image_info,Image *image)
{
  const DelegateInfo
    *delegate_info;

  const MagickInfo
    *magick_info;

  ImageInfo
    *clone_info;

  unsigned int
    status;

  /*
    Determine image type from filename prefix or suffix (e.g. image.jpg).
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image_info->filename != (char *) NULL);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  GetTimerInfo(&image->timer);
  image->logging=IsEventLogging();
  clone_info=CloneImageInfo(image_info);
  (void) strlcpy(clone_info->filename,image->filename,MaxTextExtent);
  (void) strlcpy(clone_info->magick,image->magick,MaxTextExtent);
  (void) SetImageInfo(clone_info,True,&image->exception);
  (void) strlcpy(image->filename,clone_info->filename,MaxTextExtent);
  image->dither=image_info->dither;
#if 0
  /*
    FIXME: What is this chunk of "bi-modal delegate" code for?
  */
  if (((image->next == (Image *) NULL) || clone_info->adjoin) &&
      (image->previous == (Image *) NULL) &&
      (clone_info->page == (char *) NULL) && !IsTaintImage(image))
    {
      delegate_info=GetDelegateInfo(image->magick,clone_info->magick,
				    &image->exception);
      if ((delegate_info != (const DelegateInfo *) NULL) &&
          (delegate_info->mode == 0) && IsAccessible(image->magick_filename))
        {
          /*
            Let our bi-modal delegate process the image.
          */
          (void) strlcpy(image->filename,image->magick_filename,
			 MaxTextExtent);
          status=InvokeDelegate(clone_info,image,image->magick,
				clone_info->magick,&image->exception);
          DestroyImageInfo(clone_info);
          return(!status);
        }
    }
#endif
  /*
    Call appropriate image writer based on image type.
  */
  status=False;
  magick_info=GetMagickInfo(clone_info->magick,&image->exception);
  if ((magick_info != (const MagickInfo *) NULL) &&
      (magick_info->encoder != NULL))
    {
      char
	tempfile[MaxTextExtent];

      tempfile[0]='\0';

      if (magick_info->seekable_stream == MagickTrue)
	{
	  /*
	    Divert output to temporary file if coder requires a
	    seekable stream and output is not seekable.
	  */
	  if (OpenBlob(clone_info,image,WriteBinaryBlobMode,&image->exception))
	    {
	      if (!BlobIsSeekable(image))
		{
		  if(!AcquireTemporaryFileName(tempfile))
		    {
		      ThrowException(&image->exception,FileOpenError,
				     UnableToCreateTemporaryFile,image->filename);
		      DestroyImageInfo(clone_info);
		      return(False);
		    }
		  (void) strlcpy(image->filename,tempfile,sizeof(tempfile));
		}
	      CloseBlob(image);
	    }
	}

      if (!magick_info->thread_support)
        AcquireSemaphoreInfo(&constitute_semaphore);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
			    "Invoking \"%.1024s\" encoder (%.1024s): "
			    "monochrome=%s grayscale=%s class=%s colorspace=%s",
                            magick_info->name,
                            magick_info->description,
                            MagickBoolToString(image->is_monochrome),
                            MagickBoolToString(image->is_grayscale),
                            ClassTypeToString(image->storage_class),
                            ColorspaceTypeToString(image->colorspace));
      status=(magick_info->encoder)(clone_info,image);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
			    "Returned from \"%.1024s\" encoder",magick_info->name);
      if (!magick_info->thread_support)
        LiberateSemaphoreInfo(&constitute_semaphore);

      if (tempfile[0] != '\0')
	{
	  /*
	    Send temporary file to stream.
	  */
	  (void) strlcpy(image->filename,clone_info->filename,MaxTextExtent);	  
	  if ((status &= OpenBlob(clone_info,image,WriteBinaryBlobMode,
				  &image->exception)))
	    {
	      status &= WriteBlobFile(image,tempfile);
	      CloseBlob(image);
	    }
	  LiberateTemporaryFile(tempfile);
	}
    }
  else
    {
      delegate_info=GetDelegateInfo((char *) NULL,clone_info->magick,
				    &image->exception);
      if (delegate_info != (DelegateInfo *) NULL)
        {
          /*
            Let our encoding delegate process the image.
          */
          if(!AcquireTemporaryFileName(image->filename))
            {
              ThrowException(&image->exception,FileOpenError,
			     UnableToCreateTemporaryFile,image->filename);
              DestroyImageInfo(clone_info);
              return(False);
            }
          status=InvokeDelegate(clone_info,image,(char *) NULL,
				clone_info->magick,&image->exception);
          (void) LiberateTemporaryFile(image->filename);
          DestroyImageInfo(clone_info);
          return(!status);
        }
      magick_info=GetMagickInfo(clone_info->magick,&image->exception);
      if (!clone_info->affirm && (magick_info == (const MagickInfo *) NULL))
        magick_info=(MagickInfo *)
          GetMagickInfo(image->magick,&image->exception);
      if ((magick_info == (MagickInfo *) NULL) ||
          (magick_info->encoder == NULL))
        {
          DestroyImageInfo(clone_info);
          ThrowBinaryException(MissingDelegateError,
			       NoEncodeDelegateForThisImageFormat,
			       image->filename)
	    }
      if (!magick_info->thread_support)
        AcquireSemaphoreInfo(&constitute_semaphore);
      status=(magick_info->encoder)(clone_info,image);
      if (!magick_info->thread_support)
        LiberateSemaphoreInfo(&constitute_semaphore);
    }
  (void) strlcpy(image->magick,clone_info->magick,MaxTextExtent);
  DestroyImageInfo(clone_info);
  if (GetBlobStatus(image))
    ThrowBinaryException(CorruptImageError,AnErrorHasOccurredWritingToFile,
			 image->filename);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e I m a g e s                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  WriteImages() writes an image sequence.
%
%  The format of the WriteImages method is:
%
%      unsigned int WriteImages(const ImageInfo *image_info,Image *image,
%        const char *filename,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o images: The image list.
%
%    o filename: The image filename.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport MagickPassFail WriteImages(const ImageInfo *image_info,Image *image,
  const char *filename,ExceptionInfo *exception)
{
  ImageInfo
    *clone_info;

  unsigned int
    status=MagickPass;

  /*
    Write converted images.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  image->logging=IsEventLogging();
  clone_info=CloneImageInfo(image_info);
  if (clone_info)
    {
      register Image
        *p;

      if (filename != (const char *) NULL)
        {
          if (strlcpy(clone_info->filename,filename,MaxTextExtent) >= MaxTextExtent)
            status &= MagickFail;
          /* Set file name in all image frames */
          for (p=image; p != (Image *) NULL; p=p->next)
            if (p->filename != filename)
              if (strlcpy(p->filename,filename,MaxTextExtent) >= MaxTextExtent)
                status &= MagickFail;
        }
      (void) SetImageInfo(clone_info,True,exception);
      for (p=image; p != (Image *) NULL; p=p->next)
        {          
          status &= WriteImage(clone_info,p);
          if(p->exception.severity > exception->severity)
            CopyException(exception,&p->exception);
          GetImageException(p,exception);
          if (clone_info->adjoin)
            break;
        }
      if (clone_info->verbose)
        (void) DescribeImage(image,stdout,False);
      DestroyImageInfo(clone_info);
      clone_info=0;
    }
  return(status);
}
