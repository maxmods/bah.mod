/*
% Copyright (C) 2003-2009 GraphicsMagick Group
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
%                              GGGG  EEEEE  M   M                             %
%                             G      E      MM MM                             %
%                             G GG   EEE    M M M                             %
%                             G   G  E      M   M                             %
%                              GGGG  EEEEE  M   M                             %
%                                                                             %
%                                                                             %
%                    Graphic Gems - Graphic Support Methods                   %
%                                                                             %
%                                                                             %
%                               Software Design                               %
%                                 John Cristy                                 %
%                                 August 1996                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/gem.h"
#include "magick/utility.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o n s t r a s t                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Contrast enhances the intensity differences between the lighter
%  and darker elements of the image.
%
%  The format of the Contrast method is:
%
%      void Contrast(const int sign,Quantum *red,Quantum *green,Quantum *blue)
%
%  A description of each parameter follows:
%
%    o sign: A positive value enhances the contrast otherwise it is reduced.
%
%    o red, green, blue: A pointer to a pixel component of type Quantum.
%
%
*/
MagickExport void Contrast(const int sign,Quantum *red,Quantum *green,
  Quantum *blue)
{
  static const double
    alpha=0.5+MagickEpsilon;

  double
    brightness,
    hue,
    saturation;

  /*
    Enhance contrast: dark color become darker, light color become lighter.
  */
  assert(red != (Quantum *) NULL);
  assert(green != (Quantum *) NULL);
  assert(blue != (Quantum *) NULL);
  TransformHSL(*red,*green,*blue,&hue,&saturation,&brightness);
  brightness+=
    alpha*sign*(alpha*(sin(MagickPI*(brightness-alpha))+1.0)-brightness);
  if (brightness > 1.0)
    brightness=1.0;
  if (brightness < 0.0)
    brightness=0.0;
  HSLTransform(hue,saturation,brightness,red,green,blue);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   E x p a n d A f f i n e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ExpandAffine computes the affine's expansion factor, i.e. the
%  square root of the factor by which the affine transform affects area. In an
%  affine transform composed of scaling, rotation, shearing, and translation,
%  returns the amount of scaling.
%
%  The format of the ExpandAffine method is:
%
%      double ExpandAffine(const AffineMatrix *affine)
%
%  A description of each parameter follows:
%
%    o expansion: Method ExpandAffine returns the affine's expansion factor.
%
%    o affine: A pointer the the affine transform of type AffineMatrix.
%
%
*/
MagickExport double ExpandAffine(const AffineMatrix *affine)
{
  double
    expand;

  assert(affine != (const AffineMatrix *) NULL);
  expand=fabs(affine->sx*affine->sy)-fabs(affine->rx*affine->ry);
  if (fabs(expand) < MagickEpsilon)
    return(1.0);
  return(sqrt(fabs(expand)));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e n e r a t e D i f f e r e n t i a l N o i s e                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GenerateDifferentialNoise generates a differential floating-point
%  noise value which will produce in the final result when added to the
%  original pixel.  The floating point differential value is useful since
%  it allows scaling without loss of precision and avoids clipping.
%
%  The format of the GenerateDifferentialNoise method is:
%
%      double GenerateDifferentialNoise(const Quantum pixel,
%                                       const NoiseType noise_type,
%                                       MagickRandomKernel *kernel)
%
%  A description of each parameter follows:
%
%    o pixel: A structure of type Quantum.
%
%    o noise_type:  The type of noise: Uniform, gaussian,
%      multiplicative Gaussian, impulse, laplacian, or Poisson.
%
%    o kernel: Kernel for random number generator.
%
*/
#define NoiseEpsilon   1.0e-5
#define SigmaUniform   4.0
#define SigmaGaussian  4.0
#define SigmaImpulse   0.10
#define SigmaLaplacian 10.0
#define SigmaMultiplicativeGaussian  0.5
#define SigmaPoisson   0.05
#define TauGaussian    20.0

MagickExport double GenerateDifferentialNoise(const Quantum quantum_pixel,
					      const NoiseType noise_type,
					      MagickRandomKernel *kernel)
{
  double
    alpha,
    beta,
    pixel,
    sigma,
    value;

  pixel=(double) quantum_pixel;

#if QuantumDepth > 8
  pixel /= MaxRGBDouble/255.0;
#endif

  alpha=MagickRandomRealInlined(kernel);
  if (alpha == 0.0)
    alpha=1.0;
  switch (noise_type)
  {
    case UniformNoise:
    default:
    {
      value=SigmaUniform*(alpha-0.5);
      break;
    }
    case GaussianNoise:
    {
      double
        tau;

      beta=MagickRandomRealInlined(kernel);
      sigma=sqrt(-2.0*log(alpha))*cos(2.0*MagickPI*beta);
      tau=sqrt(-2.0*log(alpha))*sin(2.0*MagickPI*beta);
      value=sqrt((double) pixel)*SigmaGaussian*sigma+TauGaussian*tau;
      break;
    }
    case MultiplicativeGaussianNoise:
    {
      if (alpha <= NoiseEpsilon)
        sigma=255.0;
      else
        sigma=sqrt(-2.0*log(alpha));
      beta=MagickRandomRealInlined(kernel);
      value=pixel*SigmaMultiplicativeGaussian*sigma*cos(2.0*MagickPI*beta);
      break;
    }
    case ImpulseNoise:
    {
      if (alpha < (SigmaImpulse/2.0))
        value=-pixel;
       else
         if (alpha >= (1.0-(SigmaImpulse/2.0)))
           value=255.0-pixel;
         else
           value=0.0;
      break;
    }
    case LaplacianNoise:
    {
      if (alpha <= 0.5)
        {
          if (alpha <= NoiseEpsilon)
            value=-255.0;
          else
            value=SigmaLaplacian*log(2.0*alpha);
          break;
        }
      beta=1.0-alpha;
      if (beta <= (0.5*NoiseEpsilon))
        value=255.0;
      else
        value=-(SigmaLaplacian*log(2.0*beta));
      break;
    }
    case PoissonNoise:
    {
      double
        limit;

      register long
        i;

      limit=exp(-SigmaPoisson*(double) pixel);
      for (i=0; alpha > limit; i++)
      {
        beta=MagickRandomRealInlined(kernel);
        alpha=alpha*beta;
      }
      value=pixel-((double) i/SigmaPoisson);
      break;
    }
  }

#if QuantumDepth > 8
  value *= (MaxRGBFloat/255.0);
#endif

  return value;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e n e r a t e N o i s e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GenerateNoise adds noise to a pixel.
%
%  The format of the GenerateNoise method is:
%
%      Quantum GenerateNoise(const Quantum pixel,const NoiseType noise_type)
%
%  A description of each parameter follows:
%
%    o pixel: A structure of type Quantum.
%
%    o noise_type:  The type of noise: Uniform, gaussian,
%      multiplicative Gaussian, impulse, laplacian, or Poisson.
%
%
*/
MagickExport Quantum GenerateNoise(const Quantum pixel,
                                   const NoiseType noise_type)
{
  double
    value;

  value=(double) pixel+GenerateDifferentialNoise(pixel,noise_type,
						 AcquireMagickRandomKernel());
  return (RoundDoubleToQuantum(value));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t O p t i m a l K e r n e l W i d t h                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetOptimalKernelWidth computes the optimal kernel radius for a
%  convolution filter.  Start with the minimum value of 3 pixels and walk out
%  until we drop below the threshold of one pixel numerical accuracy,
%
%  The format of the GetOptimalKernelWidth method is:
%
%      int GetOptimalKernelWidth(const double radius,const double sigma)
%
%  A description of each parameter follows:
%
%    o width: Method GetOptimalKernelWidth returns the optimal width of
%      a convolution kernel.
%
%    o radius: The radius of the Gaussian, in pixels, not counting the center
%      pixel.
%
%    o sigma: The standard deviation of the Gaussian, in pixels.
%
%
*/

MagickExport int GetOptimalKernelWidth1D(const double radius,const double sigma)
{
  double
    epsilon,
    normalize,
    value;

  long
    width;

  register long
    u;

  if (radius > 0.0)
    return((int) (2.0*ceil(radius)+1.0));
  epsilon=1.0/MaxRGBDouble;
  if (epsilon < MagickEpsilon)
    epsilon=MagickEpsilon;
  for (width=5; ;)
  {
    normalize=0.0;
    for (u=(-width/2); u <= (width/2); u++)
      normalize+=exp(-((double) u*u)/(2.0*sigma*sigma))/(MagickSQ2PI*sigma);
    u=width/2;
    value=exp(-((double) u*u)/(2.0*sigma*sigma))/(MagickSQ2PI*sigma)/normalize;
    if (value < epsilon)
      break;
    width+=2;
  }
  return(width-2);
}

MagickExport int GetOptimalKernelWidth2D(const double radius,const double sigma)
{
  double
    alpha,
    epsilon,
    normalize,
    value;

  long
    width;

  register long
    u,
    v;

  if (radius > 0.0)
    return((int) (2.0*ceil(radius)+1.0));
  epsilon=1.0/MaxRGBDouble;
  if (epsilon < MagickEpsilon)
    epsilon=MagickEpsilon;
  for (width=5; ;)
  {
    normalize=0.0;
    for (v=(-width/2); v <= (width/2); v++)
    {
      for (u=(-width/2); u <= (width/2); u++)
      {
        alpha=exp(-((double) u*u+v*v)/(2.0*sigma*sigma));
        normalize+=alpha/(2.0*MagickPI*sigma*sigma);
      }
    }
    v=width/2;
    value=exp(-((double) v*v)/(2.0*sigma*sigma))/(MagickSQ2PI*sigma)/normalize;
    if (value < epsilon)
      break;
    width+=2;
  }
  return(width-2);
}

MagickExport int GetOptimalKernelWidth(const double radius,const double sigma)
{
  return(GetOptimalKernelWidth1D(radius,sigma));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   H S L T r a n s f o r m                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method HSLTransform converts a floating point (hue, saturation,
%  luminosity) with range 0.0 to 1.0 to a (red, green, blue) triple
%  with range 0 to MaxRGB.
%
%  The format of the HSLTransformImage method is:
%
%      void HSLTransform(const double hue,const double saturation,
%        const double luminosity,Quantum *red,Quantum *green,Quantum *blue)
%
%  A description of each parameter follows:
%
%    o hue, saturation, luminosity: A double value representing a
%      component of the HSL color space.
%
%    o red, green, blue: A pointer to a pixel component of type Quantum.
%
%
*/
MagickExport void HSLTransform(const double hue,const double saturation,
  const double luminosity,Quantum *red,Quantum *green,Quantum *blue)
{
  /*
    Convert HSL to RGB colorspace.
  */
  assert(red != (Quantum *) NULL);
  assert(green != (Quantum *) NULL);
  assert(blue != (Quantum *) NULL);
  if (saturation == 0.0)
    {
      double l = MaxRGBDouble*luminosity;
      *red=*green=*blue= RoundDoubleToQuantum(l);
    }
  else
    {
      double
        b,
        g,
        r,
        v,
        x,
        y,
        z,
        hue_times_six,
        hue_fract,
        vsf;

      int
        sextant;

      v=(luminosity <= 0.5) ? (luminosity*(1.0+saturation)) :
        (luminosity+saturation-luminosity*saturation);

      hue_times_six=6.0*hue;
      sextant=(int) hue_times_six;
      hue_fract=hue_times_six-(double) sextant;

      y=luminosity+luminosity-v;
      vsf=(v-y)*hue_fract;
      x=y+vsf;
      z=v-vsf;
      
      switch (sextant)
        {
        case 0: r=v; g=x; b=y; break;
        case 1: r=z; g=v; b=y; break;
        case 2: r=y; g=v; b=x; break;
        case 3: r=y; g=z; b=v; break;
        case 4: r=x; g=y; b=v; break;
        case 5: r=v; g=y; b=z; break;
        default: r=v; g=x; b=y; break;
        }
      r *= MaxRGBDouble;
      *red=RoundDoubleToQuantum(r);
      g *= MaxRGBDouble;
      *green=RoundDoubleToQuantum(g);
      b *= MaxRGBDouble;
      *blue=RoundDoubleToQuantum(b);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   H W B T r a n s f o r m                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method HWBTransform converts a (hue, whiteness, blackness) to a
%  (red, green, blue) triple.
%
%  Algorithm derived from "HWB: A more intuitive hue-based color model."
%  Alvy Ray Smith and Eric Ray Lyons, Journal of Graphics Tools, Volume 1
%  Number 1, 1996.  http://www.acm.org/jgt/papers/SmithLyons96/
%
%  The format of the HWBTransformImage method is:
%
%      void HWBTransform(const double hue,const double whiteness,
%        const double blackness,Quantum *red,Quantum *green,Quantum *blue)
%
%  A description of each parameter follows:
%
%    o hue, whiteness, blackness: A double value representing a
%      component of the HWB color space.
%
%    o red, green, blue: A pointer to a pixel component of type Quantum.
%
%
*/
MagickExport void HWBTransform(const double hue,const double whiteness,
  const double blackness,Quantum *red,Quantum *green,Quantum *blue)
{
  double
    b,
    f,
    g,
    n,
    r,
    v;

  register unsigned int
    i;

  /*
    Convert HWB to RGB colorspace.
  */
  assert(red != (Quantum *) NULL);
  assert(green != (Quantum *) NULL);
  assert(blue != (Quantum *) NULL);
  v=1.0-blackness;
  if (hue == 0.0)
    {
      v *= MaxRGBDouble;
      *red=*green=*blue=RoundDoubleToQuantum(v);
      return;
    }
  i=(unsigned int) (6.0*hue);
  f=6.0*hue-i;
  if (i & 0x01)
    f=1.0-f;
  n=whiteness+f*(v-whiteness);  /* linear interpolation */
  switch (i)
  {
    default:
    case 6:
    case 0: r=v; g=n; b=whiteness; break;
    case 1: r=n; g=v; b=whiteness; break;
    case 2: r=whiteness; g=v; b=n; break;
    case 3: r=whiteness; g=n; b=v; break;
    case 4: r=n; g=whiteness; b=v; break;
    case 5: r=v; g=whiteness; b=n; break;
  }
  r *= MaxRGBDouble;
  g *= MaxRGBDouble;
  b *= MaxRGBDouble;
  *red=RoundDoubleToQuantum(r);
  *green=RoundDoubleToQuantum(g);
  *blue=RoundDoubleToQuantum(b);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   H u l l                                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Hull implements the eight hull algorithm described in Applied
%  Optics, Vol. 24, No. 10, 15 May 1985, "Geometric filter for Speckle
%  Reduction", by Thomas R Crimmins.  Each pixel in the image is replaced by
%  one of its eight of its surrounding pixels using a polarity and negative
%  hull function.
%
%  The format of the Hull method is:
%
%      void Hull(const long x_offset,const long y_offset,
%        const unsigned long columns,const unsigned long rows,Quantum *f,
%        Quantum *g,const int polarity)
%
%  A description of each parameter follows:
%
%    o x_offset, y_offset: An integer value representing the offset of the
%      current pixel within the image.
%
%    o columns, rows: Specifies the number of rows and columns in the image.
%
%    o polarity: An integer value declaring the polarity (+,-).
%
%    o f, g: A pointer to an image pixel and one of it's neighbor.
%
%
*/
MagickExport void Hull(const long x_offset,const long y_offset,
  const unsigned long columns,const unsigned long rows,Quantum *f,Quantum *g,
  const int polarity)
{
  double
    v,
    y;

  register long
    x;

  register Quantum
    *p,
    *q,
    *r,
    *s;

  assert(f != (Quantum *) NULL);
  assert(g != (Quantum *) NULL);
  p=f+(columns+2);
  q=g+(columns+2);
  r=p+(y_offset*((long) columns+2)+x_offset);
  for (y=0; y < (long) rows; y++)
  {
    p++;
    q++;
    r++;
    if (polarity > 0)
      for (x=(long) columns; x > 0; x--)
      {
        v=(*p);
        if (*r >= (v+ScaleCharToQuantum(2)))
          v+=ScaleCharToQuantum(1);
        *q=(Quantum) v;
        p++;
        q++;
        r++;
      }
    else
      for (x=(long) columns; x > 0; x--)
      {
        v=(*p);
        if (*r <= (v-(long) ScaleCharToQuantum(2)))
          v-=(long) ScaleCharToQuantum(1);
        *q=(Quantum) v;
        p++;
        q++;
        r++;
      }
    p++;
    q++;
    r++;
  }
  p=f+(columns+2);
  q=g+(columns+2);
  r=q+(y_offset*((long) columns+2)+x_offset);
  s=q-(y_offset*((long) columns+2)+x_offset);
  for (y=0; y < (long) rows; y++)
  {
    p++;
    q++;
    r++;
    s++;
    if (polarity > 0)
      for (x=(long) columns; x > 0; x--)
      {
        v=(*q);
        if ((*s >= (v+ScaleCharToQuantum(2))) && (*r > v))
          v+=ScaleCharToQuantum(1);
        *p=(Quantum) v;
        p++;
        q++;
        r++;
        s++;
      }
    else
      for (x=(long) columns; x > 0; x--)
      {
        v=(*q);
        if ((*s <= (v-(long) ScaleCharToQuantum(2))) && (*r < v))
          v-=(long) ScaleCharToQuantum(1);
        *p=(Quantum) v;
        p++;
        q++;
        r++;
        s++;
      }
    p++;
    q++;
    r++;
    s++;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I d e n t i t y A f f i n e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IdentityAffine initializes the affine transform to the identity
%  matrix.
%
%  The format of the IdentityAffine method is:
%
%      IdentityAffine(AffineMatrix *affine)
%
%  A description of each parameter follows:
%
%    o affine: A pointer the the affine transform of type AffineMatrix.
%
%
*/
MagickExport void IdentityAffine(AffineMatrix *affine)
{
  assert(affine != (AffineMatrix *) NULL);
  (void) memset(affine,0,sizeof(AffineMatrix));
  affine->sx=1.0;
  affine->sy=1.0;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M o d u l a t e                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Modulate modulates the hue, saturation, and brightness of an
%  image. Brightness and saturation are expressed as a ratio of the
%  existing value. Hue is expressed as a ratio of rotation from the current
%  position in that 1.0 results in the existing position, 0.5 results in a
%  counter-clockwise rotation of 90 degrees, 1.5 results in a clockwise
%  rotation of 90 degrees, and 0 and 2.0 obtain a 180 degree rotation.
%
%  The format of the Modulate method is:
%
%      void Modulate(const double percent_hue,const double percent_saturation,
%        const double percent_brightness,Quantum *red,Quantum *green,
%        Quantum *blue)
%
%  A description of each parameter follows:
%
%    o percent_hue, percent_saturation, percent_brightness: A double value
%      representing the percent change in a component of the HSL color space.
%
%    o red, green, blue: A pointer to a pixel component of type Quantum.
%
%
*/
MagickExport void Modulate(const double percent_hue,
  const double percent_saturation,const double percent_brightness,
  Quantum *red,Quantum *green,Quantum *blue)
{
  double
    brightness,
    hue,
    saturation;

  /*
    Increase or decrease color brightness, saturation, or hue.
  */
  assert(red != (Quantum *) NULL);
  assert(green != (Quantum *) NULL);
  assert(blue != (Quantum *) NULL);
  TransformHSL(*red,*green,*blue,&hue,&saturation,&brightness);
  brightness*=(0.01+MagickEpsilon)*percent_brightness;
  if (brightness > 1.0)
    brightness=1.0;
  saturation*=(0.01+MagickEpsilon)*percent_saturation;
  if (saturation > 1.0)
    saturation=1.0;

  hue += (percent_hue/200.0 - 0.5);
  while (hue < 0.0)
    hue += 1.0;
  while (hue > 1.0)
    hue -= 1.0;
  HSLTransform(hue,saturation,brightness,red,green,blue);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   T r a n s f o r m H S L                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method TransformHSL converts a (red, green, blue) to a (hue, saturation,
%  luminosity) triple with values in range 0.0 to 1.0.
%
%  The format of the TransformHSL method is:
%
%      void TransformHSL(const Quantum red,const Quantum green,
%        const Quantum blue,double *hue,double *saturation,double *luminosity)
%
%  A description of each parameter follows:
%
%    o red, green, blue: A Quantum value representing the red, green, and
%      blue component of a pixel..
%
%    o hue, saturation, luminosity: A pointer to a double value representing a
%      component of the HSL color space.
%
%
*/
MagickExport void TransformHSL(const Quantum red,const Quantum green,
  const Quantum blue,double *hue_result,double *saturation_result,double *luminosity_result)
{
  double
    hue,
    saturation,
    luminosity,
    b,
    delta,
    g,
    max,
    min,
    r;

  /*
    Convert RGB to HSL colorspace.
  */
  assert(hue_result != (double *) NULL);
  assert(saturation_result != (double *) NULL);
  assert(luminosity_result != (double *) NULL);

  r=(double) red/MaxRGBDouble;
  g=(double) green/MaxRGBDouble;
  b=(double) blue/MaxRGBDouble;
  max=Max(r,Max(g,b));
  min=Min(r,Min(g,b));
  hue=0.0;
  saturation=0.0;
  luminosity=(min+max)/2.0;
  delta=max-min;
  if (delta != 0.0)
    {
      saturation=delta/((luminosity <= 0.5) ? (min+max) : (2.0-max-min));
      if (r == max)
        hue=(g == min ? 5.0+(max-b)/delta : 1.0-(max-g)/delta);
      else
        if (g == max)
          hue=(b == min ? 1.0+(max-r)/delta : 3.0-(max-b)/delta);
        else
          hue=(r == min ? 3.0+(max-g)/delta : 5.0-(max-r)/delta);
      hue/=6.0;
    }

  *hue_result=ConstrainToRange(0.0,1.0,hue);
  *saturation_result=ConstrainToRange(0.0,1.0,saturation);
  *luminosity_result=ConstrainToRange(0.0,1.0,luminosity);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   T r a n s f o r m H W B                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method TransformHWB converts a (red, green, blue) to a
%  (hue, whiteness, blackness) triple.
%
%  Algorithm derived from "HWB: A more intuitive hue-based color model."
%  Alvy Ray Smith and Eric Ray Lyons, Journal of Graphics Tools, Volume 1
%  Number 1, 1996.  http://www.acm.org/jgt/papers/SmithLyons96/
%
%  The format of the TransformHWB method is:
%
%      void TransformHWB(const Quantum red,const Quantum green,
%        const Quantum blue,double *hue,double *whiteness,double *blackness)
%
%  A description of each parameter follows:
%
%    o red, green, blue: A Quantum value representing the red, green, and
%      blue component of a pixel.
%
%    o hue, whiteness, blackness: A pointer to a double value representing a
%      component of the HWB color space.
%
%
*/
MagickExport void TransformHWB(const Quantum red,const Quantum green, const Quantum blue,
                               double *hue,double *whiteness,double *blackness)
{
  double
    f,
    v,
    w;

  register long
    i;

  /*
    Convert RGB to HWB colorspace.
  */
  assert(hue != (double *) NULL);
  assert(whiteness != (double *) NULL);
  assert(blackness != (double *) NULL);
  w=(double) Min(red,Min(green,blue));
  v=(double) Max(red,Max(green,blue));
  *blackness=((double) MaxRGBDouble-v)/MaxRGBDouble;
  if (v == w)
    {
      *hue=0.0;
      *whiteness=1.0-(*blackness);
    }
  else
    {
      f=(red == w) ? (double) green-blue :
        ((green == w) ? (double) blue-red :
         (double) red-green);
      i=(red == w) ? 3 : ((green == w) ? 5 : 1);
      *hue=((double) i-f/(v-w))/6.0;
      *whiteness=((double) w/MaxRGBDouble);
    }
}
