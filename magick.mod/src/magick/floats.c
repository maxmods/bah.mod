/*
  Copyright (C) 2008 - 2011 GraphicsMagick Group
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  16/24 bit floating point conversion functions

  Written by Richard Nolde, 2008

*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/floats.h"

#define FP16_MANT_BITS 10
#define FP24_MANT_BITS 16
#define FP32_MANT_BITS 23
#define FP64_MANT_BITS 52

MagickExport
int _Gm_convert_fp16_to_fp32 (const fp_16bits *fp16, float *fp32)
{
  unsigned char  sbit; /* sign bit */
  unsigned char  expt; /* exponent bits */
  unsigned char  m2, m1;  /* MSB to LSB of mantissa */
  unsigned char  new_expt;
  unsigned char  new_m3, new_m2, new_m1; 
  unsigned int   little_endian = 1;
  unsigned char *src;
  unsigned char *dst;

#ifdef DEBUG32
  /* Debugging variables */
  int            i, j, bit;
  unsigned int  mant;
  double         test  = 0.0; 
  double         test2 = 0.0; 
  double         accum = 0.0;

  errno = 0;
#endif

  assert (sizeof(int) == 4);
  if ((fp16 == NULL) || (fp32 == NULL))
    {
      fprintf (stderr, "Invalid src or destination pointers\n");
      return (1);
    }
  sbit=0;
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

  /* Underflow and overflow will not be a problem 
   * since target has more significant bits that
   * the source.
   */
#ifdef DEBUG32
  /* Debugging code for display only */
  mant = ((unsigned int)new_m3 << 16) | ((unsigned int)new_m2 << 8) | (unsigned int)new_m1; 
  if (new_expt == 0)
    {
      test = 0.0;
      test2 = 0.0;
      accum = 0.0;
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
      test = pow (2.0, 1.0 * (new_expt - 127));
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
  printf ("              Sign bit: %u, Expt biased to %5u,  2^%-5d = %8.1f  *  %10.8f = %18.10f\n\n",
	  sbit > 0 ? 1 : 0, new_expt, new_expt > 0 ? new_expt - 127 : 0, test, accum, test2);
#endif
  return (0);
} /* end convertfp16_to_fp32 */

MagickExport
int _Gm_convert_fp32_to_fp16 (const float *fp32, fp_16bits *fp16, const int mode)
{
  int            i, bit, rbits, rshift;
  unsigned char  sbit = 0;   /* sign bit */
  unsigned char  expt = 0;   /* exponent bits */
  unsigned char  m3, m2, m1; /* MSB to LSB of mantissa */
  signed   short new_expt;
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
	  /* Extract mantissa and left align bits */
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
	  /* Extract mantissa and left align bits */
	  m3  = (((*(src + 1) & 0x7F)) << 1) |
	    ((*(src + 2) & 0x80) >> 7);
	  m2  = (((*(src + 2) & 0x7F)) << 1)  |
	    ((*(src + 3) & 0x80) >> 7);
	  m1  =  (*(src + 3) & 0x7F) << 1;
	}

      /* Extract the 16 MSB from the mantissa */  
      mant = (m3 << 8) | m2;
      if (expt != 0)  /* Normal number */
	new_expt = expt - 127 + 15;

      /* Even if the new exponent is too small to represent, 
       * the mantissa could have signficant digits that can
       * be represented in the new value as a subnormal.
       */
      if (new_expt <= 0) /* Underflow */
	{
	  rshift = 1 - new_expt;
	  switch (mode)
	    {
	    case STRICT_IEEE: /* NaN has all 1s in exponent plus 2 bits in Mantissa */
	      if (rshift > FP16_MANT_BITS)
		{
		  new_expt = 31;
		  new_mant = 513;
		  errno = ERANGE;
		  fflush (stdout);
		  fprintf (stderr, "Underflow. Result clipped\n");
		  fflush (stderr);
		  return (1);  /* The number cannot be represented as fp16 */
		}
	      break;
	    case RANGE_LIMITED: /* Clamp to smallest subnormal */
	      new_expt = 0;
	      new_mant = mant >> rshift;;
	      mp = (unsigned char *)&new_mant;
#ifdef DEBUG16
	      if (mant != 0)
		{
		  fflush (stdout);
		  fprintf (stderr, "Underflow. %18.10f Result clippped to subnormal value\n", *fp32);
		  fflush (stderr);
		}
#endif
	      break;        
	    case ZERO_LIMITED:  /* Clamp to zero instead of using a subnormal */
	      new_expt = 0;
	      new_mant = 0;
	      mp = (unsigned char *)&new_mant;
#ifdef DEBUG16
	      if (mant != 0)
		{
		  fflush (stdout);
		  fprintf (stderr, "Underflow. %18.10f Result clippped to zero\n", *fp32);
		  fflush (stderr);
		}
#endif
	      break;        
	    }
	}
      else /* Take the MSB from the old mantissa and left justify them */
	{
	  if (new_expt > 30) /* Overflow */
	    {
	      switch (mode)
		{
		case STRICT_IEEE: /* NaN has all 1s in exponent plus 2 bits in Mantissa */
		  new_expt = 31;
		  new_mant = 513;
		  errno = ERANGE;
		  fflush (stdout);
		  fprintf (stderr, "Overflow. %18.10f Result clipped\n", *fp32);
		  fflush (stderr);
		  return (1);
		case ZERO_LIMITED:
		case RANGE_LIMITED:  /* Clamp to maximum allowed value for fp16 */
		  new_expt = 30;
		  new_mant = 1023;
		  mp = (unsigned char *)&new_mant;
#ifdef DEBUG16
		  fflush (stdout);
		  fprintf (stderr, "Overflow. %18.10f Result clippped\n", *fp32);
		  fflush (stderr);
#endif
		  break;        
		}
	    }
	  else /* Normal value within range of target type */
	    {
	      /* Check bits to the right of unit in last signficant place
	       * for destination, eg first digit that cannot be stored.
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
		      for (i = 6; i < 16; i++)
			{
			  bit = mant & (1 << i);
			  if (bit == 0)
			    {
			      new_mant = (mant | ((unsigned short)1 << i)) & (0xFFFF << i);
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
			  for (i = 6; i < 10; i++)
			    {
			      bit = mant & (1 << i);
			      if (bit == 0)
				{
				  new_mant = (mant | ((unsigned short)1 << i)) & (0xFFFF << i);
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
  new_mant = (m2 << 8) | m1;
  printf ("%10.10f mant%s ", *fp32, (rbits & 0x20) ? "+" : "-");
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
      accum = 0.0;
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
      if (new_expt == 0)
	{
	  accum += 2.0;
	  test = pow (2.0, 1.0 * (-15 - rshift));
	}
      else
	{
	  accum += 1.0;
	  test = pow (2.0, 1.0 * (new_expt - 15));
	}
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
  printf ("              Sign bit: %u, Expt biased to %5u,  2^%-5d = %8.10f  *  %10.8f = %18.10f\n\n",
	  sbit > 0 ? 1 : 0, new_expt, new_expt > 0 ? new_expt - 15 : 0, test, accum, test2);
#endif
  return (0);
} /* end _Gm_convert_fp32_to_fp16 */

MagickExport
int _Gm_convert_fp24_to_fp32 (const fp_24bits *fp24, float *fp32, const int mode)
{
  unsigned char  sbit = 0;           /* sign bit */
  unsigned char  expt = 0, new_expt; /* exponent bits */
  unsigned char  m2, m1;    /* MSB to LSB of mantissa */
  unsigned char  new_m3, new_m2, new_m1;
  /* unsigned short mant; */
  /* unsigned int   new_mant; */
  unsigned int   little_endian = 1;
  /* unsigned char *mp; */
  unsigned char *src;
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
      fprintf (stderr, "Invalid src or destination pointers\n");
      return (1);
    }
  little_endian = *((unsigned char *)&little_endian) & '1';

  src = (unsigned char *)fp24;
  dst = (unsigned char *)fp32;
  /* mp  = (unsigned char *)&mant; */

  new_expt = expt = 0;
  /* new_mant = mant = 0; */
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
      /* mant = (m2 << 8) | m1; */
      new_m3  =  (m2 & 0xFE) >> 1;
      new_m2  = ((m2 & 0x01) << 7) | ((m1 & 0xFE) >> 1);
      new_m1  =  (m1 & 0x01) << 7;
    }
  /* We do not have to worry about underflow or overflow 
   * since the target has more significant bits in the
   * exponent and the significand.
   */
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
  printf ("  mant ");
  for (j = 0, k = 15; j < 16; j++, k--)
    {
      bit = mant & (1 << k);
      if ((j % 8) == 0)
	printf(" ");
      printf ("%d", bit ? 1 : 0);
    }

  printf (" New Mant ");
  for (j = 0, k = 22; j < 23; j++, k--)
    {
      bit = new_mant & (1 << k);
      printf ("%d", bit ? 1 : 0);
      if ((k % 8) == 0)
	printf(" ");
    }

  printf (" Sbit + Exp ");
  if (little_endian)
    {
      for (i = 3; i >= 0; i--)
	{
	  for (j = 0, k = 7; j < 8; j++, k--)
	    {
	      bit = *(dst + i) & (1 << k);
	      if (i == 2 && j == 1)
		printf ("  Mant: ");
	      printf ("%d", bit ? 1 : 0);
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
		printf ("  Mant: ");
	      printf ("%d", bit ? 1 : 0);
	    }
	}
    }
  printf ("\n");

  new_mant = ((unsigned int)new_m3 << 16) | ((unsigned int)new_m2 << 8) | new_m1;
  if ((int)*fp24 == 0.0)
    {
      test = 0.0;
      test2 = 0.0;
      accum = 0.0;
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
      test = pow (2.0, 1.0 * (new_expt - 127));
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
  printf ("              Sign bit: %u, Expt biased to %5u,  2^%-5d = %8.1f  *  %10.8f = %18.10f\n\n",
	  sbit > 0 ? 1 : 0, new_expt, new_expt > 0 ? new_expt - 127 : 0, test, accum, test2);
#endif

  return (0);
} /* end convertfp24_to_fp32 */

MagickExport
int _Gm_convert_fp32_to_fp24 (const float *fp32, fp_24bits *fp24, const int mode)
{
  int            i = 1;
  int            rbits, rshift, bit;
  unsigned char  sbit = 0;           /* sign bit */
  unsigned char  expt = 0; /* exponent bits */
  unsigned char  m3;   /* high order bits of mantissa */
  unsigned char  m2;
  unsigned char  m1;   /* low order bits of mantissa */
  unsigned char  new_m2, new_m1;
  signed   short new_expt;
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
  
      mant = (m3 << 24) | (m2 << 16) |( m1 << 8); 
      if (expt != 0)
	new_expt = expt - 127 + 63;

      /* Even if the new exponent is too small to represent, 
       * the mantissa could have signficant digits that can
       * be represented in the new value as a subnormal.
       */
      if (new_expt <= 0) /* Underflow */
	{
	  rshift = 0 - new_expt;
	  switch (mode)
	    {
	    case STRICT_IEEE: /* NaN has all 1s in exponent plus 2 bits in Mantissa */
	      if (rshift > FP24_MANT_BITS)
		{
		  new_expt = 0x7F;
		  new_mant = 0x80010000;
		  errno = ERANGE;
		  fflush (stdout);
		  fprintf (stderr, "Underflow. %18.10f Result clipped\n", *fp32);
		  fflush (stderr);
		  return (1);  /* The number cannot be represented as fp24 */
		}
	      break;
	    case RANGE_LIMITED: /* Clamp to smallest subnormal */
	      new_expt = 0;
	      new_mant = mant >> rshift;;
	      mp = (unsigned char *)&new_mant;
#ifdef DEBUG24
	      fflush (stdout);
	      fprintf (stderr, "Underflow. %18.10f Result clippped to subnormal value\n", *fp32);
	      fflush (stderr);
#endif
	      break;        
	    case ZERO_LIMITED:  /* Clamp to zero instead of using a subnormal */
	      new_expt = 0;
	      new_mant = 0;
	      mp = (unsigned char *)&new_mant;
#ifdef DEBUG24
	      fflush (stdout);
	      fprintf (stderr, "Underflow. %18.10f Result clippped to zero\n", *fp32);
	      fflush (stderr);
#endif
	      break;        
	    }
	}
      else /* Take the MSB from the old mantissa and left justify them */
	{
	  if (new_expt > 126) /* Overflow */
	    {
	      switch (mode)
		{
		case STRICT_IEEE: /* NaN has all 1s in exponent plus 2 bits in Mantissa */
		  new_expt = 0x7F;
		  mant = 0x80010000;
		  errno = ERANGE;
		  fflush (stdout);
		  fprintf (stderr, "Overflow. Result clipped\n");
		  fflush (stderr);
		  return (1);
		case ZERO_LIMITED:
		case RANGE_LIMITED:  /* Clamp to maximum allowed value for fp24 */
		  new_expt = 126;
		  new_mant = 0xFFFF0000;
		  mp = (unsigned char *)&new_mant;
#ifdef DEBUG24
		  fflush (stdout);
		  fprintf (stderr, "Overflow. Result clippped\n");
		  fflush (stderr);
#endif
		  break;        
		}
	    }
	  else /* Normal value within range of target type */
	    { /* Remove the bits to the left of the binary point 
	       * by shifting the fractional bits into the leftmost position
	       */
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
  printf ("%10.10f mant%s ", *fp32, (rbits & 0x8000) ? "+" : "-");
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
      accum = 0.0;
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
      test = pow (2.0, 1.0 * (new_expt - 63));
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
  printf ("              Sign bit: %u, Expt biased to %5u,  2^%-5d = %8.10f  *  %10.8f = %18.10f\n\n",
	  sbit > 0 ? 1 : 0, new_expt, new_expt > 0 ? new_expt - 63 : 0, test, accum, test2);
#endif

  return (0);
} /* end convertfp32_to_fp24 */
