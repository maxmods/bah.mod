/*
% Copyright (C) 2003 - 2009 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                 RRRR   EEEEE  SSSSS  IIIII  ZZZZZ  EEEEE                    %
%                 R   R  E      SS       I       ZZ  E                        %
%                 RRRR   EEE     SSS     I     ZZZ   EEE                      %
%                 R R    E         SS    I    ZZ     E                        %
%                 R  R   EEEEE  SSSSS  IIIII  ZZZZZ  EEEEE                    %
%                                                                             %
%                    GraphicsMagick Image Resize Methods                      %
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
#include "magick/enum_strings.h"
#include "magick/log.h"
#include "magick/monitor.h"
#include "magick/omp_data_view.h"
#include "magick/pixel_cache.h"
#include "magick/resize.h"
#include "magick/utility.h"

/*
  Typedef declarations.
*/
typedef struct _ContributionInfo
{
  double
    weight;

  long
    pixel;
} ContributionInfo;

typedef struct _FilterInfo
{
  double
    (*function)(const double,const double),
    support;
} FilterInfo;
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   B e s s e l O r d e r O n e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  BesselOrderOne() computes the Bessel function of x of the first kind of
%  order 0:
%
%    Reduce x to |x| since j1(x)= -j1(-x), and for x in (0,8]
%
%       j1(x) = x*j1(x);
%
%    For x in (8,inf)
%
%       j1(x) = sqrt(2/(pi*x))*(p1(x)*cos(x1)-q1(x)*sin(x1))
%
%    where x1 = x-3*pi/4. Compute sin(x1) and cos(x1) as follow:
%
%       cos(x1) =  cos(x)cos(3pi/4)+sin(x)sin(3pi/4)
%               =  1/sqrt(2) * (sin(x) - cos(x))
%       sin(x1) =  sin(x)cos(3pi/4)-cos(x)sin(3pi/4)
%               = -1/sqrt(2) * (sin(x) + cos(x))
%
%  The format of the BesselOrderOne method is:
%
%      double BesselOrderOne(double x)
%
%  A description of each parameter follows:
%
%    o value: Method BesselOrderOne returns the Bessel function of x of the
%      first kind of orders 1.
%
%    o x: double value.
%
%
*/

static double J1(double x)
{
  double
    p,
    q;

  register long
    i;

  static const double
    Pone[] =
    {
       0.581199354001606143928050809e+21,
      -0.6672106568924916298020941484e+20,
       0.2316433580634002297931815435e+19,
      -0.3588817569910106050743641413e+17,
       0.2908795263834775409737601689e+15,
      -0.1322983480332126453125473247e+13,
       0.3413234182301700539091292655e+10,
      -0.4695753530642995859767162166e+7,
       0.270112271089232341485679099e+4
    },
    Qone[] =
    {
      0.11623987080032122878585294e+22,
      0.1185770712190320999837113348e+20,
      0.6092061398917521746105196863e+17,
      0.2081661221307607351240184229e+15,
      0.5243710262167649715406728642e+12,
      0.1013863514358673989967045588e+10,
      0.1501793594998585505921097578e+7,
      0.1606931573481487801970916749e+4,
      0.1e+1
    };

  p=Pone[8];
  q=Qone[8];
  for (i=7; i >= 0; i--)
  {
    p=p*x*x+Pone[i];
    q=q*x*x+Qone[i];
  }
  return(p/q);
}

static double P1(double x)
{
  double
    p,
    q;

  register long
    i;

  static const double
    Pone[] =
    {
      0.352246649133679798341724373e+5,
      0.62758845247161281269005675e+5,
      0.313539631109159574238669888e+5,
      0.49854832060594338434500455e+4,
      0.2111529182853962382105718e+3,
      0.12571716929145341558495e+1
    },
    Qone[] =
    {
      0.352246649133679798068390431e+5,
      0.626943469593560511888833731e+5,
      0.312404063819041039923015703e+5,
      0.4930396490181088979386097e+4,
      0.2030775189134759322293574e+3,
      0.1e+1
    };

  p=Pone[5];
  q=Qone[5];
  for (i=4; i >= 0; i--)
  {
    p=p*(8.0/x)*(8.0/x)+Pone[i];
    q=q*(8.0/x)*(8.0/x)+Qone[i];
  }
  return(p/q);
}

static double Q1(double x)
{
  double
    p,
    q;

  register long
    i;

  static const double
    Pone[] =
    {
      0.3511751914303552822533318e+3,
      0.7210391804904475039280863e+3,
      0.4259873011654442389886993e+3,
      0.831898957673850827325226e+2,
      0.45681716295512267064405e+1,
      0.3532840052740123642735e-1
    },
    Qone[] =
    {
      0.74917374171809127714519505e+4,
      0.154141773392650970499848051e+5,
      0.91522317015169922705904727e+4,
      0.18111867005523513506724158e+4,
      0.1038187585462133728776636e+3,
      0.1e+1
    };

  p=Pone[5];
  q=Qone[5];
  for (i=4; i >= 0; i--)
  {
    p=p*(8.0/x)*(8.0/x)+Pone[i];
    q=q*(8.0/x)*(8.0/x)+Qone[i];
  }
  return(p/q);
}

static double BesselOrderOne(double x)
{
  double
    p,
    q;

  if (x == 0.0)
    return(0.0);
  p=x;
  if (x < 0.0)
    x=(-x);
  if (x < 8.0)
    return(p*J1(x));
  q=sqrt(2.0/(MagickPI*x))*(P1(x)*(1.0/sqrt(2.0)*(sin(x)-cos(x)))-8.0/x*Q1(x)*
    (-1.0/sqrt(2.0)*(sin(x)+cos(x))));
  if (p < 0.0)
    q=(-q);
  return(q);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g n i f y I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagnifyImage() is a convenience method that scales an image proportionally
%  to twice its size.
%
%  The format of the MagnifyImage method is:
%
%      Image *MagnifyImage(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *MagnifyImage(const Image *image,ExceptionInfo *exception)
{
#define MagnifyImageText "[%s] Magnify...  "

  Image
    *magnify_image;

  long
    rows,
    y;

  PixelPacket
    *scanline;

  register long
    x;

  register PixelPacket
    *p,
    *q,
    *r;

  /*
    Initialize magnify image attributes.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  magnify_image=CloneImage(image,2*image->columns,2*image->rows,True,exception);
  if (magnify_image == (Image *) NULL)
    return((Image *) NULL);

  (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                        "Magnifying image of size %lux%lu to %lux%lu",
                        image->columns,image->rows,magnify_image->columns,magnify_image->rows);

  magnify_image->storage_class=DirectClass;
  /*
    Allocate image buffer and scanline buffer for 4 rows of the image.
  */
  scanline=MagickAllocateArray(PixelPacket *,
                               magnify_image->columns,sizeof(PixelPacket));
  if (scanline == (PixelPacket *) NULL)
    {
      DestroyImage(magnify_image);
      ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
                           UnableToMagnifyImage)
        }
  /*
    Initialize magnify image pixels.
  */
  for (y=0; y < (long) image->rows; y++)
    {
      const PixelPacket
        *pixels;

      pixels=AcquireImagePixels(image,0,y,image->columns,1,exception);
      q=SetImagePixels(magnify_image,0,y,magnify_image->columns,1);
      if ((pixels == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
        break;
      (void) memcpy(q,pixels,image->columns*sizeof(PixelPacket));
      if (!SyncImagePixels(magnify_image))
        break;
    }
  /*
    Magnify each row.
  */
  for (y=0; y < (long) image->rows; y++)
    {
      p=GetImagePixels(magnify_image,0,(long) (image->rows-y-1),
                       magnify_image->columns,1);
      if (p == (PixelPacket *) NULL)
        break;
      (void) memcpy(scanline,p,magnify_image->columns*sizeof(PixelPacket));
      q=GetImagePixels(magnify_image,0,(long) (2*(image->rows-y-1)),
                       magnify_image->columns,1);
      if (q == (PixelPacket *) NULL)
        break;
      p=scanline+image->columns-1;
      q+=2*(image->columns-1);
      *q=(*p);
      *(q+1)=(*(p));
      for (x=1; x < (long) image->columns; x++)
        {
          p--;
          q-=2;
          *q=(*p);
          (q+1)->red=(Quantum) (((double) p->red+(double) (p+1)->red)/2.0+0.5);
          (q+1)->green=(Quantum) (((double) p->green+(double) (p+1)->green)/2.0+0.5);
          (q+1)->blue=(Quantum) (((double) p->blue+(double) (p+1)->blue)/2.0+0.5);
          (q+1)->opacity=(Quantum)
            (((double) p->opacity+(double) (p+1)->opacity)/2.0+0.5);
        }
      if (!SyncImagePixels(magnify_image))
        break;
    }

  for (y=0; y < (long) image->rows; y++)
    {
      rows=(long) Min(image->rows-y,3);
      p=GetImagePixels(magnify_image,0,2*y,magnify_image->columns,rows);
      if (p == (PixelPacket *) NULL)
        break;
      q=p;
      if (rows > 1)
        q=p+magnify_image->columns;
      r=p;
      if (rows > 2)
        r=q+magnify_image->columns;
      for (x=0; x < (long) (image->columns-1); x++)
        {
          q->red=(Quantum) (((double) p->red+(double) r->red)/2.0+0.5);
          q->green=(Quantum) (((double) p->green+(double) r->green)/2.0+0.5);
          q->blue=(Quantum) (((double) p->blue+(double) r->blue)/2.0+0.5);
          q->opacity=(Quantum) (((double) p->opacity+(double) r->opacity)/2.0+0.5);
          (q+1)->red=(Quantum) (((double) p->red+(double) (p+2)->red+
                                 (double) r->red+(double) (r+2)->red)/4.0+0.5);
          (q+1)->green=(Quantum) (((double) p->green+(double) (p+2)->green+
                                   (double) r->green+(double) (r+2)->green)/4.0+0.5);
          (q+1)->blue=(Quantum) (((double) p->blue+(double) (p+2)->blue+
                                  (double) r->blue+(double) (r+2)->blue)/4.0+0.5);
          (q+1)->opacity=(Quantum) (((double) p->opacity+(double) (p+2)->opacity+
                                     (double) r->opacity+(double) (r+2)->opacity)/4.0+0.5);
          q+=2;
          p+=2;
          r+=2;
        }
      q->red=(Quantum) (((double) p->red+(double) r->red)/2.0+0.5);
      q->green=(Quantum) (((double) p->green+(double) r->green)/2.0+0.5);
      q->blue=(Quantum) (((double) p->blue+(double) r->blue)/2.0+0.5);
      q->opacity=(Quantum) (((double) p->opacity+(double) r->opacity)/2.0+0.5);
      p++;
      q++;
      r++;
      q->red=(Quantum) (((double) p->red+(double) r->red)/2.0+0.5);
      q->green=(Quantum) (((double) p->green+(double) r->green)/2.0+0.5);
      q->blue=(Quantum) (((double) p->blue+(double) r->blue)/2.0+0.5);
      q->opacity=(Quantum) (((double) p->opacity+(double) r->opacity)/2.0+0.5);
      if (!SyncImagePixels(magnify_image))
        break;
      if (QuantumTick(y,image->rows))
        if (!MagickMonitorFormatted(y,image->rows,exception,
                                    MagnifyImageText,image->filename))
          break;
    }
  p=GetImagePixels(magnify_image,0,(long) (2*image->rows-2),
                   magnify_image->columns,1);
  if (p != (PixelPacket *) NULL)
    (void) memcpy(scanline,p,magnify_image->columns*sizeof(PixelPacket));
  q=GetImagePixels(magnify_image,0,(long) (2*image->rows-1),
                   magnify_image->columns,1);
  if (q != (PixelPacket *) NULL)
    (void) memcpy(q,scanline,magnify_image->columns*sizeof(PixelPacket));
  (void) SyncImagePixels(magnify_image);
  MagickFreeMemory(scanline);
  magnify_image->is_grayscale=image->is_grayscale;
  return(magnify_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M i n i f y I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MinifyImage() is a convenience method that scales an image proportionally
%  to half its size.
%
%  The format of the MinifyImage method is:
%
%      Image *MinifyImage(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *MinifyImage(const Image *image,ExceptionInfo *exception)
{
#define Minify(weight)                          \
  total.red+=(weight)*(r->red);                 \
  total.green+=(weight)*(r->green);             \
  total.blue+=(weight)*(r->blue);               \
  total.opacity+=(weight)*(r->opacity);         \
  r++;
#define MinifyImageText "[%s] Minify..."

  Image
    *minify_image;

  long
    y;
 
  /*
    Initialize minified image.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);

  minify_image=CloneImage(image,Max(image->columns/2,1),Max(image->rows/2,1),
                          True,exception);
  if (minify_image == (Image *) NULL)
    return((Image *) NULL);

  (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                        "Minifying image of size %lux%lu to %lux%lu",
                        image->columns,image->rows,
                        minify_image->columns,minify_image->rows);

  minify_image->storage_class=DirectClass;
  /*
    Reduce each row.
  */
  {
    unsigned long
      row_count=0;

    DoublePixelPacket
      zero;

    MagickPassFail
      status=MagickPass;

    (void) memset(&zero,0,sizeof(DoublePixelPacket));
#if defined(HAVE_OPENMP)
#  pragma omp parallel for schedule(dynamic,4) shared(row_count, status)
#endif
    for (y=0; y < (long) minify_image->rows; y++)
      {
        DoublePixelPacket
          total;

        register const PixelPacket
          *p,
          *r;

        register long
          x;

        register PixelPacket
          *q;

        MagickBool
          thread_status;

        thread_status=status;
        if (thread_status == MagickFail)
          continue;

        p=AcquireImagePixels(image,-2,2*(y-1),image->columns+4,4,exception);
        q=SetImagePixelsEx(minify_image,0,y,minify_image->columns,1,exception);
        if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
          thread_status=MagickFail;
        if (thread_status != MagickFail)
          {
            for (x=0; x < (long) minify_image->columns; x++)
              {
                /*
                  Compute weighted average of target pixel color components.
                */
                total=zero;
                r=p;
                Minify(3.0); Minify(7.0);  Minify(7.0);  Minify(3.0);
                r=p+(image->columns+4);
                Minify(7.0); Minify(15.0); Minify(15.0); Minify(7.0);
                r=p+2*(image->columns+4);
                Minify(7.0); Minify(15.0); Minify(15.0); Minify(7.0);
                r=p+3*(image->columns+4);
                Minify(3.0); Minify(7.0);  Minify(7.0);  Minify(3.0);
                q->red=(Quantum) (total.red/128.0+0.5);
                q->green=(Quantum) (total.green/128.0+0.5);
                q->blue=(Quantum) (total.blue/128.0+0.5);
                q->opacity=(Quantum) (total.opacity/128.0+0.5);
                p+=2;
                q++;
              }
            if (!SyncImagePixelsEx(minify_image,exception))
              thread_status=MagickFail;
          }
#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_MinifyImage)
#endif
        {
          row_count++;
          if (QuantumTick(row_count,image->rows))
            if (!MagickMonitorFormatted(row_count,image->rows,exception,
                                        MinifyImageText,image->filename))
              thread_status=MagickFail;
          
          if (thread_status == MagickFail)
            status=MagickFail;
        }
      }
  }
  minify_image->is_grayscale=image->is_grayscale;
  return(minify_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e s i z e I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ResizeImage() scales an image to the desired dimensions with one of these
%  filters:
%
%    Bessel   Blackman   Box
%    Catrom   Cubic      Gaussian
%    Hanning  Hermite    Lanczos
%    Mitchell Point      Quandratic
%    Sinc     Triangle
%
%  Most of the filters are FIR (finite impulse response), however, Bessel,
%  Gaussian, and Sinc are IIR (infinite impulse response).  Bessel and Sinc
%  are windowed (brought down to zero) with the Blackman filter.
%
%  ResizeImage() was inspired by Paul Heckbert's zoom program.
%
%  The format of the ResizeImage method is:
%
%      Image *ResizeImage(Image *image,const unsigned long columns,
%        const unsigned long rows,const FilterTypes filter,const double blur,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o columns: The number of columns in the scaled image.
%
%    o rows: The number of rows in the scaled image.
%
%    o filter: Image filter to use.
%
%    o blur: The blur factor where > 1 is blurry, < 1 is sharp.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

static double Bessel(const double x,const double ARGUNUSED(support))
{
  if (x == 0.0)
    return(MagickPI/4.0);
  return(BesselOrderOne(MagickPI*x)/(2.0*x));
}

static double Sinc(const double x,const double ARGUNUSED(support))
{
  if (x == 0.0)
    return(1.0);
  return(sin(MagickPI*x)/(MagickPI*x));
}

static double Blackman(const double x,const double ARGUNUSED(support))
{
  return(0.42+0.5*cos(MagickPI*x)+0.08*cos(2*MagickPI*x));
}

static double BlackmanBessel(const double x,const double support)
{
  return(Blackman(x/support,support)*Bessel(x,support));
}

static double BlackmanSinc(const double x,const double support)
{
  return(Blackman(x/support,support)*Sinc(x,support));
}

static double Box(const double x,const double ARGUNUSED(support))
{
  if (x < -0.5)
    return(0.0);
  if (x < 0.5)
    return(1.0);
  return(0.0);
}

static double Catrom(const double x,const double ARGUNUSED(support))
{
  if (x < -2.0)
    return(0.0);
  if (x < -1.0)
    return(0.5*(4.0+x*(8.0+x*(5.0+x))));
  if (x < 0.0)
    return(0.5*(2.0+x*x*(-5.0-3.0*x)));
  if (x < 1.0)
    return(0.5*(2.0+x*x*(-5.0+3.0*x)));
  if (x < 2.0)
    return(0.5*(4.0+x*(-8.0+x*(5.0-x))));
  return(0.0);
}

static double Cubic(const double x,const double ARGUNUSED(support))
{
  if (x < -2.0)
    return(0.0);
  if (x < -1.0)
    return((2.0+x)*(2.0+x)*(2.0+x)/6.0);
  if (x < 0.0)
    return((4.0+x*x*(-6.0-3.0*x))/6.0);
  if (x < 1.0)
    return((4.0+x*x*(-6.0+3.0*x))/6.0);
  if (x < 2.0)
    return((2.0-x)*(2.0-x)*(2.0-x)/6.0);
  return(0.0);
}

static double Gaussian(const double x,const double ARGUNUSED(support))
{
  return(exp(-2.0*x*x)*sqrt(2.0/MagickPI));
}

static double Hanning(const double x,const double ARGUNUSED(support))
{
  return(0.5+0.5*cos(MagickPI*x));
}

static double Hamming(const double x,const double ARGUNUSED(support))
{
  return(0.54+0.46*cos(MagickPI*x));
}

static double Hermite(const double x,const double ARGUNUSED(support))
{
  if (x < -1.0)
    return(0.0);
  if (x < 0.0)
    return((2.0*(-x)-3.0)*(-x)*(-x)+1.0);
  if (x < 1.0)
    return((2.0*x-3.0)*x*x+1.0);
  return(0.0);
}

static double Lanczos(const double x,const double support)
{
  if (x < -3.0)
    return(0.0);
  if (x < 0.0)
    return(Sinc(-x,support)*Sinc(-x/3.0,support));
  if (x < 3.0)
    return(Sinc(x,support)*Sinc(x/3.0,support));
  return(0.0);
}

static double Mitchell(const double x,const double ARGUNUSED(support))
{
#define B   (1.0/3.0)
#define C   (1.0/3.0)
#define P0  ((  6.0- 2.0*B       )/6.0)
#define P2  ((-18.0+12.0*B+ 6.0*C)/6.0)
#define P3  (( 12.0- 9.0*B- 6.0*C)/6.0)
#define Q0  ((       8.0*B+24.0*C)/6.0)
#define Q1  ((     -12.0*B-48.0*C)/6.0)
#define Q2  ((       6.0*B+30.0*C)/6.0)
#define Q3  ((     - 1.0*B- 6.0*C)/6.0)

  if (x < -2.0)
    return(0.0);
  if (x < -1.0)
    return(Q0-x*(Q1-x*(Q2-x*Q3)));
  if (x < 0.0)
    return(P0+x*x*(P2-x*P3));
  if (x < 1.0)
    return(P0+x*x*(P2+x*P3));
  if (x < 2.0)
    return(Q0+x*(Q1+x*(Q2+x*Q3)));
  return(0.0);
}

static double Quadratic(const double x,const double ARGUNUSED(support))
{
  if (x < -1.5)
    return(0.0);
  if (x < -0.5)
    return(0.5*(x+1.5)*(x+1.5));
  if (x < 0.5)
    return(0.75-x*x);
  if (x < 1.5)
    return(0.5*(x-1.5)*(x-1.5));
  return(0.0);
}

static double Triangle(const double x,const double ARGUNUSED(support))
{
  if (x < -1.0)
    return(0.0);
  if (x < 0.0)
    return(1.0+x);
  if (x < 1.0)
    return(1.0-x);
  return(0.0);
}

static MagickPassFail
HorizontalFilter(const Image *source,Image *destination,
                 const double x_factor,const FilterInfo *filter_info,
                 const double blur,ThreadViewDataSet *view_data_set,
                 const size_t span,unsigned long *quantum,
                 ExceptionInfo *exception)
{
#define ResizeImageText "[%s] Resize..."
  
  double
    scale,
    support;

  DoublePixelPacket
    zero;

  long
    x;

  MagickPassFail
    status=MagickPass;

  if (IsEventLogging())
    (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                          "Enter HorizontalFilter() ...");

  scale=blur*Max(1.0/x_factor,1.0);
  support=scale*filter_info->support;
  destination->storage_class=source->storage_class;
  if (support > 0.5)
    destination->storage_class=DirectClass;
  else
    {
      /*
        Reduce to point sampling.
      */
      support=0.5+MagickEpsilon;
      scale=1.0;
    }
  scale=1.0/scale;
  (void) memset(&zero,0,sizeof(DoublePixelPacket));
#if defined(HAVE_OPENMP)
#  pragma omp parallel for shared(status)
#endif
  for (x=0; x < (long) destination->columns; x++)
    {
      double
        center,
        density;

      ContributionInfo
        *contribution;

      register const PixelPacket
        *p;

      register PixelPacket
        *q = (PixelPacket *) NULL;

      const IndexPacket
        *source_indexes;

      IndexPacket
        *indexes;

      long
        n,
        start,
        stop,
        y;

      MagickBool
        thread_status;
    
      thread_status=status;
      if (thread_status == MagickFail)
        continue;

      contribution=AccessThreadViewData(view_data_set);
      /* fprintf(stderr,"Thread %d contribution %p\n",omp_get_thread_num(),contribution); */
      center=(double) (x+0.5)/x_factor;
      start=(long) Max(center-support+0.5,0);
      stop=(long) Min(center+support+0.5,source->columns);
      density=0.0;
      for (n=0; n < (stop-start); n++)
        {
          contribution[n].pixel=start+n;
          contribution[n].weight=
            filter_info->function(scale*(start+n-center+0.5),filter_info->support);
          density+=contribution[n].weight;
        }
      if ((density != 0.0) && (density != 1.0))
        {
          /*
            Normalize.
          */
          long
            i;

          density=1.0/density;
          for (i=0; i < n; i++)
            contribution[i].weight*=density;
        }

      p=AcquireImagePixels(source,contribution[0].pixel,0,
                           contribution[n-1].pixel-contribution[0].pixel+1,
                           source->rows,exception);
      if (p == (const PixelPacket *) NULL)
	thread_status=MagickFail;

      if (thread_status != MagickFail)
	q=SetImagePixelsEx(destination,x,0,1,destination->rows,exception);
      if (q == (PixelPacket *) NULL)
        thread_status=MagickFail;

      if (thread_status != MagickFail)
        {
          source_indexes=AccessImmutableIndexes(source);
          indexes=AccessMutableIndexes(destination);
          for (y=0; y < (long) destination->rows; y++)
            {
              double
                weight;

              DoublePixelPacket
                pixel;

              long
                j;

              register long
                i;

              pixel=zero;
              if ((destination->matte) || (destination->colorspace == CMYKColorspace))
                {
		  double
		    transparency_coeff,
		    normalize;

		  normalize=0.0;
                  for (i=0; i < n; i++)
                    {
                      j=y*(contribution[n-1].pixel-contribution[0].pixel+1)+
                        (contribution[i].pixel-contribution[0].pixel);
                      weight=contribution[i].weight;
                      transparency_coeff = weight * (1 - ((double) p[j].opacity/TransparentOpacity));
                      pixel.red+=transparency_coeff*p[j].red;
                      pixel.green+=transparency_coeff*p[j].green;
                      pixel.blue+=transparency_coeff*p[j].blue;
                      pixel.opacity+=weight*p[j].opacity;
                      normalize += transparency_coeff;
                    }
                  normalize = 1.0 / (AbsoluteValue(normalize) <= MagickEpsilon ? 1.0 : normalize);
		  pixel.red *= normalize;
		  pixel.green *= normalize;
		  pixel.blue *= normalize;
                  q[y].red=RoundDoubleToQuantum(pixel.red);
                  q[y].green=RoundDoubleToQuantum(pixel.green);
                  q[y].blue=RoundDoubleToQuantum(pixel.blue);
                  q[y].opacity=RoundDoubleToQuantum(pixel.opacity);
                }
              else
                {
                  for (i=0; i < n; i++)
                    {
                      j=(long) (y*(contribution[n-1].pixel-contribution[0].pixel+1)+
                                (contribution[i].pixel-contribution[0].pixel));
                      weight=contribution[i].weight;
                      pixel.red+=weight*p[j].red;
                      pixel.green+=weight*p[j].green;
                      pixel.blue+=weight*p[j].blue;
                    }
                  q[y].red=RoundDoubleToQuantum(pixel.red);
                  q[y].green=RoundDoubleToQuantum(pixel.green);
                  q[y].blue=RoundDoubleToQuantum(pixel.blue);
                  q[y].opacity=OpaqueOpacity;
                }

              if ((indexes != (IndexPacket *) NULL) &&
                  (source_indexes != (IndexPacket *) NULL))
                {
                  i=Min(Max((long) (center+0.5),start),stop-1);
                  j=y*(contribution[n-1].pixel-contribution[0].pixel+1)+
                    (contribution[i-start].pixel-contribution[0].pixel);
                  indexes[y]=source_indexes[j];
                }
            }
          if (!SyncImagePixelsEx(destination,exception))
            thread_status=MagickFail;
        }
#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_HorizontalFilter)
#endif
      {
        if (QuantumTick(*quantum,span))
          if (!MagickMonitorFormatted(*quantum,span,exception,
                                      ResizeImageText,source->filename))
            thread_status=MagickFail;

        (*quantum)++;
          
        if (thread_status == MagickFail)
          status=MagickFail;
      }
    }

  if (IsEventLogging())
    (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                          "%s exit HorizontalFilter()",
			  (status == MagickFail ? "Error" : "Normal"));

  return (status);
}

static MagickPassFail
VerticalFilter(const Image *source,Image *destination,
               const double y_factor,const FilterInfo *filter_info,
               const double blur,ThreadViewDataSet *view_data_set,
               const size_t span,unsigned long *quantum,
               ExceptionInfo *exception)
{
  double
    scale,
    support;

  DoublePixelPacket
    zero;

  long
    y;

  MagickPassFail
    status=MagickPass;

  if (IsEventLogging())
    (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                          "Enter VerticalFilter() ...");

  /*
    Apply filter to resize vertically from source to destination.
  */
  scale=blur*Max(1.0/y_factor,1.0);
  support=scale*filter_info->support;
  destination->storage_class=source->storage_class;
  if (support > 0.5)
    destination->storage_class=DirectClass;
  else
    {
      /*
        Reduce to point sampling.
      */
      support=0.5+MagickEpsilon;
      scale=1.0;
    }
  scale=1.0/scale;
  (void) memset(&zero,0,sizeof(DoublePixelPacket));
#if defined(HAVE_OPENMP)
#  pragma omp parallel for shared(status)
#endif
  for (y=0; y < (long) destination->rows; y++)
    {
      double
        center,
        density;

      ContributionInfo
        *contribution;

      register const PixelPacket
        *p;
    
      register PixelPacket
        *q = (PixelPacket *) NULL;

      const IndexPacket
        *source_indexes;

      IndexPacket
        *indexes;

      long
        n,
        start,
        stop,
        x;

      MagickBool
        thread_status;
    
      thread_status=status;
      if (thread_status == MagickFail)
        continue;

      contribution=AccessThreadViewData(view_data_set);
      center=(double) (y+0.5)/y_factor;
      start=(long) Max(center-support+0.5,0);
      stop=(long) Min(center+support+0.5,source->rows);
      density=0.0;
      for (n=0; n < (stop-start); n++)
        {
          contribution[n].pixel=start+n;
          contribution[n].weight=
            filter_info->function(scale*(start+n-center+0.5),filter_info->support);
          density+=contribution[n].weight;
        }
      if ((density != 0.0) && (density != 1.0))
        {
          /*
            Normalize.
          */
          long
            i;

          density=1.0/density;
          for (i=0; i < n; i++)
            contribution[i].weight*=density;
        }

      p=AcquireImagePixels(source,0,contribution[0].pixel,source->columns,
                           contribution[n-1].pixel-contribution[0].pixel+1,
                           exception);
      if (p == (const PixelPacket *) NULL)
	thread_status=MagickFail;

      if (thread_status != MagickFail)
	q=SetImagePixelsEx(destination,0,y,destination->columns,1,exception);
      if (q == (PixelPacket *) NULL)
        thread_status=MagickFail;

      if (thread_status != MagickFail)
        {
          source_indexes=AccessImmutableIndexes(source);
          indexes=AccessMutableIndexes(destination);
          for (x=0; x < (long) destination->columns; x++)
            {
              double
                weight;

              DoublePixelPacket
                pixel;

              long
                j;

              register long
                i;

              pixel=zero;
              if ((source->matte) || (source->colorspace == CMYKColorspace))
                {
		  double
		    transparency_coeff,
		    normalize;

		  normalize=0.0;
                  for (i=0; i < n; i++)
                    {
                      j=(long) ((contribution[i].pixel-contribution[0].pixel)*
                                source->columns+x);
                      weight=contribution[i].weight;
                      transparency_coeff = weight * (1 - ((double) p[j].opacity/TransparentOpacity));
                      pixel.red+=transparency_coeff*p[j].red;
                      pixel.green+=transparency_coeff*p[j].green;
                      pixel.blue+=transparency_coeff*p[j].blue;
                      pixel.opacity+=weight*p[j].opacity;
                      normalize += transparency_coeff;
                    }

                  normalize = 1.0 / (AbsoluteValue(normalize) <= MagickEpsilon ? 1.0 : normalize);
		  pixel.red *= normalize;
		  pixel.green *= normalize;
		  pixel.blue *= normalize;
                  q[x].red=RoundDoubleToQuantum(pixel.red);
                  q[x].green=RoundDoubleToQuantum(pixel.green);
                  q[x].blue=RoundDoubleToQuantum(pixel.blue);
                  q[x].opacity=RoundDoubleToQuantum(pixel.opacity);
                }
              else
                {
                  for (i=0; i < n; i++)
                    {
                      j=(long) ((contribution[i].pixel-contribution[0].pixel)*
                                source->columns+x);
                      weight=contribution[i].weight;
                      pixel.red+=weight*p[j].red;
                      pixel.green+=weight*p[j].green;
                      pixel.blue+=weight*p[j].blue;
                    }
                  q[x].red=RoundDoubleToQuantum(pixel.red);
                  q[x].green=RoundDoubleToQuantum(pixel.green);
                  q[x].blue=RoundDoubleToQuantum(pixel.blue);
                  q[x].opacity=OpaqueOpacity;
                }

              if ((indexes != (IndexPacket *) NULL) &&
                  (source_indexes != (IndexPacket *) NULL))
                {
                  i=Min(Max((long) (center+0.5),start),stop-1);
                  j=(long) ((contribution[i-start].pixel-contribution[0].pixel)*
                            source->columns+x);
                  indexes[x]=source_indexes[j];
                }
            }
          if (!SyncImagePixelsEx(destination,exception))
            thread_status=MagickFail;
        }
#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_VerticalFilter)
#endif
      {
        if (QuantumTick(*quantum,span))
          if (!MagickMonitorFormatted(*quantum,span,exception,
                                      ResizeImageText,source->filename))
            thread_status=MagickFail;
        
        (*quantum)++;
        
        if (thread_status == MagickFail)
          status=MagickFail;
      }
    }

  if (IsEventLogging())
    (void) LogMagickEvent(TransformEvent,GetMagickModule(),
			  "%s exit VerticalFilter()",
			  (status == MagickFail ? "Error" : "Normal"));

  return (status);
}

MagickExport Image *ResizeImage(const Image *image,const unsigned long columns,
                                const unsigned long rows,const FilterTypes filter,
                                const double blur,
                                ExceptionInfo *exception)
{
  ThreadViewDataSet
    *view_data_set;

  double
    support,
    x_factor,
    x_support,
    y_factor,
    y_support;

  Image
    *source_image,
    *resize_image;

  register long
    i;

  static const FilterInfo
    filters[SincFilter+1] =
    {
      { Box, 0.0 },
      { Box, 0.0 },
      { Box, 0.5 },
      { Triangle, 1.0 },
      { Hermite, 1.0 },
      { Hanning, 1.0 },
      { Hamming, 1.0 },
      { Blackman, 1.0 },
      { Gaussian, 1.25 },
      { Quadratic, 1.5 },
      { Cubic, 2.0 },
      { Catrom, 2.0 },
      { Mitchell, 2.0 },
      { Lanczos, 3.0 },
      { BlackmanBessel, 3.2383 },
      { BlackmanSinc, 4.0 }
    };

  size_t
    span;

  MagickPassFail
    status;

  unsigned long
    quantum;

  MagickBool
    order;

  /*
    Initialize resize image attributes.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  assert(((int) filter >= 0) && ((int) filter <= SincFilter));
  if ((columns == 0) || (rows == 0))
    {
      ThrowImageException(ImageError,UnableToResizeImage,
                          MagickMsg(OptionError,NonzeroWidthAndHeightRequired));
    }
  if ((columns == image->columns) && (rows == image->rows) && (blur == 1.0))
    return (CloneImage(image,0,0,True,exception));
  resize_image=CloneImage(image,columns,rows,True,exception);
  if (resize_image == (Image *) NULL)
    return ((Image *) NULL);

  order=(((double) columns*(image->rows+rows)) >
         ((double) rows*(image->columns+columns)));
  if (order)
    source_image=CloneImage(resize_image,columns,image->rows,True,exception);
  else
    source_image=CloneImage(resize_image,image->columns,rows,True,exception);
  if (source_image == (Image *) NULL)
    return ((Image *) NULL);

  /*
    Allocate filter contribution info.
  */
  x_factor=(double) resize_image->columns/image->columns;
  y_factor=(double) resize_image->rows/image->rows;
  i=(long) DefaultResizeFilter;
  if (image->filter != UndefinedFilter)
    i=(long) image->filter;
  else
    if ((image->storage_class == PseudoClass) || image->matte ||
        ((x_factor*y_factor) > 1.0))
      i=(long) MitchellFilter;


  if (IsEventLogging())
    (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                          "Resizing image of size %lux%lu to %lux%lu using %s filter",
                          image->columns,image->rows,columns,rows,
                          ResizeFilterToString((FilterTypes)i));

  x_support=blur*Max(1.0/x_factor,1.0)*filters[i].support;
  y_support=blur*Max(1.0/y_factor,1.0)*filters[i].support;
  support=Max(x_support,y_support);
  if (support < filters[i].support)
    support=filters[i].support;
  /*
    Allocate view data set.
  */
  view_data_set=AllocateThreadViewDataArray(image,exception,
                                            (size_t) (2.0*Max(support,0.5)+3),
                                            sizeof(ContributionInfo));
  if (view_data_set == (ThreadViewDataSet *) NULL)
    {
      DestroyImage(resize_image);
      DestroyImage(source_image);
      ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
                           UnableToResizeImage);
    }
  /*
    Resize image.
  */
  status=MagickPass;
  quantum=0;
  if (order)
    {
      span=source_image->columns+resize_image->rows;
      status=HorizontalFilter(image,source_image,x_factor,&filters[i],blur,
			      view_data_set,span,&quantum,exception);
      if (status != MagickFail)
	status=VerticalFilter(source_image,resize_image,y_factor,&filters[i],
			      blur,view_data_set,span,&quantum,exception);
    }
  else
    {
      span=resize_image->columns+source_image->rows;
      status=VerticalFilter(image,source_image,y_factor,&filters[i],blur,
			    view_data_set,span,&quantum,exception);
      if (status != MagickFail)
	status=HorizontalFilter(source_image,resize_image,x_factor,&filters[i],
				blur,view_data_set,span,&quantum,exception);
    }
  /*
    Free allocated memory.
  */
  DestroyThreadViewDataSet(view_data_set);
  DestroyImage(source_image);
  if (status == MagickFail)
    {
      DestroyImage(resize_image);
      return((Image *) NULL);
    }
  resize_image->is_grayscale=image->is_grayscale;
  return(resize_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S a m p l e I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SampleImage() scales an image to the desired dimensions with pixel
%  sampling.  Unlike other scaling methods, this method does not introduce
%  any additional color into the scaled image.
%
%  The format of the SampleImage method is:
%
%      Image *SampleImage(const Image *image,const unsigned long columns,
%        const unsigned long rows,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o columns: The number of columns in the sampled image.
%
%    o rows: The number of rows in the sampled image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *
SampleImage(const Image *image,const unsigned long columns,
            const unsigned long rows,ExceptionInfo *exception)
{
  double
    *x_offset,
    *y_offset;

  Image
    *sample_image;

  long
    j,
    y;

  PixelPacket
    *pixels;

  /*
    Initialize sampled image attributes.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if ((columns == 0) || (rows == 0))
    ThrowImageException(ImageError,UnableToResizeImage,
                        MagickMsg(CorruptImageError,
                                  NegativeOrZeroImageSize));
  if ((columns == image->columns) && (rows == image->rows))
    return(CloneImage(image,0,0,True,exception));
  sample_image=CloneImage(image,columns,rows,True,exception);
  if (sample_image == (Image *) NULL)
    return((Image *) NULL);

  (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                        "Sampling image of size %lux%lu to %lux%lu",
                        image->columns,image->rows,sample_image->columns,
                        sample_image->rows);
  /*
    Allocate scan line buffer and column offset buffers.
  */
  pixels=MagickAllocateArray(PixelPacket *,image->columns,sizeof(PixelPacket));
  x_offset=MagickAllocateArray(double *,sample_image->columns,sizeof(double));
  y_offset=MagickAllocateArray(double *,sample_image->rows,sizeof(double));
  if ((pixels == (PixelPacket *) NULL) || (x_offset == (double *) NULL) ||
      (y_offset == (double *) NULL))
    {
      DestroyImage(sample_image);
      ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
                           UnableToSampleImage);
    }
  /*
    Initialize pixel offsets.
  */
  {
    long
      x;

    for (x=0; x < (long) sample_image->columns; x++)
      x_offset[x]=(double) x*image->columns/(double) sample_image->columns;
    for (y=0; y < (long) sample_image->rows; y++)
      y_offset[y]=(double) y*image->rows/(double) sample_image->rows;
  }
  /*
    Sample each row.
    This algorithm will not benefit from OpenMP.
  */
  j=(-1);
  for (y=0; y < (long) sample_image->rows; y++)
    {
      register const PixelPacket
        *p;

      register PixelPacket
        *q;

      register const IndexPacket
        *indexes;

      register IndexPacket
        *sample_indexes;

      register long
        x;

      q=SetImagePixels(sample_image,0,y,sample_image->columns,1);
      if (q == (PixelPacket *) NULL)
        break;
      if (j != (long) y_offset[y])
        {
          /*
            Read a scan line.
          */
          j=(long) y_offset[y];
          p=AcquireImagePixels(image,0,j,image->columns,1,exception);
          if (p == (const PixelPacket *) NULL)
            break;
          (void) memcpy(pixels,p,image->columns*sizeof(PixelPacket));
        }
      /*
        Sample each column.
      */
      for (x=0; x < (long) sample_image->columns; x++)
        *q++=pixels[(long) x_offset[x]];
      indexes=AccessImmutableIndexes(image);
      sample_indexes=AccessMutableIndexes(sample_image);
      if ((indexes != (IndexPacket *) NULL) &&
          (sample_indexes != (IndexPacket *) NULL))
        for (x=0; x < (long) sample_image->columns; x++)
          sample_indexes[x]=indexes[(long) x_offset[x]];
      if (!SyncImagePixels(sample_image))
        break;
      if (QuantumTick(y,sample_image->rows))
        if (!MagickMonitorFormatted(y,sample_image->rows,exception,
                                    "[%s] Sample (%lux%lu --> %lux%lu) image...",
                                    image->filename,image->columns,image->rows,
                                    sample_image->columns, sample_image->rows))
          break;
    }
  MagickFreeMemory(y_offset);
  MagickFreeMemory(x_offset);
  MagickFreeMemory(pixels);
  /*
    Sampling does not change the image properties.
  */
  sample_image->is_monochrome=image->is_monochrome;
  sample_image->is_grayscale=image->is_grayscale;
  return(sample_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S c a l e I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ScaleImage() changes the size of an image to the given dimensions.
%
%  The format of the ScaleImage method is:
%
%      Image *ScaleImage(const Image *image,const unsigned long columns,
%        const unsigned long rows,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o columns: The number of columns in the scaled image.
%
%    o rows: The number of rows in the scaled image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *ScaleImage(const Image *image,const unsigned long columns,
                               const unsigned long rows,ExceptionInfo *exception)
{
#define ScaleImageText "[%s] Scale..."

  double
    x_scale,
    x_span,
    y_scale,
    y_span;

  DoublePixelPacket
    pixel,
    *scale_scanline,
    *scanline,
    *x_vector,
    *y_vector,
    zero;

  Image
    *scale_image;

  long
    number_rows,
    y;

  register const PixelPacket
    *p;

  register long
    i,
    x;

  register PixelPacket
    *q;

  register DoublePixelPacket
    *s,
    *t;

  unsigned int
    next_column,
    next_row;

  /*
    Initialize scaled image attributes.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if ((columns == 0) || (rows == 0))
    return((Image *) NULL);
  scale_image=CloneImage(image,columns,rows,True,exception);
  if (scale_image == (Image *) NULL)
    return((Image *) NULL);

  (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                        "Scaling image of size %lux%lu to %lux%lu",
                        image->columns,image->rows,scale_image->columns,
                        scale_image->rows);

  scale_image->storage_class=DirectClass;
  /*
    Allocate memory.
  */
  x_vector=MagickAllocateMemory(DoublePixelPacket *,
                                image->columns*sizeof(DoublePixelPacket));
  scanline=x_vector;
  if (image->rows != scale_image->rows)
    scanline=MagickAllocateMemory(DoublePixelPacket *,
                                  image->columns*sizeof(DoublePixelPacket));
  scale_scanline=MagickAllocateMemory(DoublePixelPacket *,
                                      scale_image->columns*sizeof(DoublePixelPacket));
  y_vector=MagickAllocateMemory(DoublePixelPacket *,
                                image->columns*sizeof(DoublePixelPacket));
  if ((scanline == (DoublePixelPacket *) NULL) ||
      (scale_scanline == (DoublePixelPacket *) NULL) ||
      (x_vector == (DoublePixelPacket *) NULL) ||
      (y_vector == (DoublePixelPacket *) NULL))
    {
      DestroyImage(scale_image);
      ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
                           UnableToScaleImage);
    }
  /*
    Scale image.
  */
  number_rows=0;
  next_row=True;
  y_span=1.0;
  y_scale=(double) scale_image->rows/image->rows;
  (void) memset(y_vector,0,image->columns*sizeof(DoublePixelPacket));
  (void) memset(&zero,0,sizeof(DoublePixelPacket));
  i=0;
  for (y=0; y < (long) scale_image->rows; y++)
    {
      q=SetImagePixels(scale_image,0,y,scale_image->columns,1);
      if (q == (PixelPacket *) NULL)
        break;
      if (scale_image->rows == image->rows)
        {
          /*
            Read a new scanline.
          */
          p=AcquireImagePixels(image,0,i++,image->columns,1,exception);
          if (p == (const PixelPacket *) NULL)
            break;
          for (x=0; x < (long) image->columns; x++)
            {
              x_vector[x].red=p->red;
              x_vector[x].green=p->green;
              x_vector[x].blue=p->blue;
              x_vector[x].opacity=p->opacity;
              p++;
            }
        }
      else
        {
          /*
            Scale Y direction.
          */
          while (y_scale < y_span)
            {
              if (next_row && (number_rows < (long) image->rows))
                {
                  /*
                    Read a new scanline.
                  */
                  p=AcquireImagePixels(image,0,i++,image->columns,1,exception);
                  if (p == (const PixelPacket *) NULL)
                    break;
                  for (x=0; x < (long) image->columns; x++)
                    {
                      x_vector[x].red=p->red;
                      x_vector[x].green=p->green;
                      x_vector[x].blue=p->blue;
                      x_vector[x].opacity=p->opacity;
                      p++;
                    }
                  number_rows++;
                }
              for (x=0; x < (long) image->columns; x++)
                {
                  y_vector[x].red+=y_scale*x_vector[x].red;
                  y_vector[x].green+=y_scale*x_vector[x].green;
                  y_vector[x].blue+=y_scale*x_vector[x].blue;
                  y_vector[x].opacity+=y_scale*x_vector[x].opacity;
                }
              y_span-=y_scale;
              y_scale=(double) scale_image->rows/image->rows;
              next_row=True;
            }
          if (next_row && (number_rows < (long) image->rows))
            {
              /*
                Read a new scanline.
              */
              p=AcquireImagePixels(image,0,i++,image->columns,1,exception);
              if (p == (const PixelPacket *) NULL)
                break;
              for (x=0; x < (long) image->columns; x++)
                {
                  x_vector[x].red=p->red;
                  x_vector[x].green=p->green;
                  x_vector[x].blue=p->blue;
                  x_vector[x].opacity=p->opacity;
                  p++;
                }
              number_rows++;
              next_row=False;
            }
          s=scanline;
          for (x=0; x < (long) image->columns; x++)
            {
              pixel.red=y_vector[x].red+y_span*x_vector[x].red;
              pixel.green=y_vector[x].green+y_span*x_vector[x].green;
              pixel.blue=y_vector[x].blue+y_span*x_vector[x].blue;
              pixel.opacity=y_vector[x].opacity+y_span*x_vector[x].opacity;
              s->red=pixel.red > MaxRGBDouble ? MaxRGBDouble : pixel.red;
              s->green=pixel.green > MaxRGBDouble ? MaxRGBDouble : pixel.green;
              s->blue=pixel.blue > MaxRGBDouble ? MaxRGBDouble : pixel.blue;
              s->opacity=pixel.opacity > MaxRGBDouble ? MaxRGBDouble : pixel.opacity;
              s++;
              y_vector[x].red=0;
              y_vector[x].green=0;
              y_vector[x].blue=0;
              y_vector[x].opacity=0;
            }
          y_scale-=y_span;
          if (y_scale <= 0)
            {
              y_scale=(double) scale_image->rows/image->rows;
              next_row=True;
            }
          y_span=1.0;
        }
      if (scale_image->columns == image->columns)
        {
          /*
            Transfer scanline to scaled image.
          */
          s=scanline;
          for (x=0; x < (long) scale_image->columns; x++)
            {
              q->red=(Quantum) (s->red+0.5);
              q->green=(Quantum) (s->green+0.5);
              q->blue=(Quantum) (s->blue+0.5);
              q->opacity=(Quantum) (s->opacity+0.5);
              q++;
              s++;
            }
        }
      else
        {
          /*
            Scale X direction.
          */
          pixel=zero;
          next_column=False;
          x_span=1.0;
          s=scanline;
          t=scale_scanline;
          for (x=0; x < (long) image->columns; x++)
            {
              x_scale=(double) scale_image->columns/image->columns;
              while (x_scale >= x_span)
                {
                  if (next_column)
                    {
                      pixel=zero;
                      t++;
                    }
                  pixel.red+=x_span*s->red;
                  pixel.green+=x_span*s->green;
                  pixel.blue+=x_span*s->blue;
                  pixel.opacity+=x_span*s->opacity;
                  t->red=pixel.red > MaxRGBDouble ? MaxRGBDouble : pixel.red;
                  t->green=pixel.green > MaxRGBDouble ? MaxRGBDouble : pixel.green;
                  t->blue=pixel.blue > MaxRGBDouble ? MaxRGBDouble : pixel.blue;
                  t->opacity=pixel.opacity > MaxRGBDouble ? MaxRGBDouble : pixel.opacity;
                  x_scale-=x_span;
                  x_span=1.0;
                  next_column=True;
                }
              if (x_scale > 0.0)
                {
                  if (next_column)
                    {
                      pixel=zero;
                      next_column=False;
                      t++;
                    }
                  pixel.red+=x_scale*s->red;
                  pixel.green+=x_scale*s->green;
                  pixel.blue+=x_scale*s->blue;
                  pixel.opacity+=x_scale*s->opacity;
                  x_span-=x_scale;
                }
              s++;
            }
          if (x_span > 0.0)
            {
              s--;
              pixel.red+=x_span*s->red;
              pixel.green+=x_span*s->green;
              pixel.blue+=x_span*s->blue;
              pixel.opacity+=x_span*s->opacity;
            }
          if (!next_column && ((t-scale_scanline) < (long) scale_image->columns))
            {
              t->red=pixel.red > MaxRGBDouble ? MaxRGBDouble : pixel.red;
              t->green=pixel.green > MaxRGBDouble ? MaxRGBDouble : pixel.green;
              t->blue=pixel.blue > MaxRGBDouble ? MaxRGBDouble : pixel.blue;
              t->opacity=pixel.opacity > MaxRGBDouble ? MaxRGBDouble : pixel.opacity;
            }
          /*
            Transfer scanline to scaled image.
          */
          t=scale_scanline;
          for (x=0; x < (long) scale_image->columns; x++)
            {
              q->red=(Quantum) (t->red+0.5);
              q->green=(Quantum) (t->green+0.5);
              q->blue=(Quantum) (t->blue+0.5);
              q->opacity=(Quantum) (t->opacity+0.5);
              q++;
              t++;
            }
        }
      if (!SyncImagePixels(scale_image))
        break;
      if (QuantumTick(y,scale_image->rows))
        if (!MagickMonitorFormatted(y,scale_image->rows,exception,
                                    ScaleImageText,image->filename))
          break;
    }
  /*
    Free allocated memory.
  */
  MagickFreeMemory(y_vector);
  MagickFreeMemory(scale_scanline);
  if (scale_image->rows != image->rows)
    MagickFreeMemory(scanline);
  MagickFreeMemory(x_vector);
  scale_image->is_grayscale=image->is_grayscale;
  return(scale_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   T h u m b n a i l I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ThumbnailImage() changes the size of an image to the given dimensions.
%  This method was designed by Bob Friesenhahn as a low cost thumbnail
%  generator.
%
%  The format of the ThumbnailImage method is:
%
%      Image *ThumbnailImage(const Image *image,const unsigned long columns,
%        const unsigned long rows,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o columns: The number of columns in the scaled image.
%
%    o rows: The number of rows in the scaled image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *ThumbnailImage(const Image *image,
  const unsigned long columns,const unsigned long rows,ExceptionInfo *exception)
{
  double
    x_factor,
    y_factor;

  Image
    *sample_image,
    *thumbnail_image;

  x_factor=(double) columns/image->columns;
  y_factor=(double) rows/image->rows;
  if ((x_factor*y_factor) > 0.1)
    return(ResizeImage(image,columns,rows,BoxFilter,image->blur,exception));
  sample_image=SampleImage(image,5*columns,5*rows,exception);
  if (sample_image == (Image *) NULL)
    return((Image *) NULL);
  thumbnail_image=ResizeImage(sample_image,columns,rows,BoxFilter,
    sample_image->blur,exception);
  DestroyImage(sample_image);
  return(thumbnail_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   Z o o m I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ZoomImage() creates a new image that is a scaled size of an existing one.
%  It allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.  The Point filter gives fast pixel replication,
%  Triangle is equivalent to bi-linear interpolation, and Mitchel giver slower,
%  very high-quality results.  See Graphic Gems III for details on this
%  algorithm.
%
%  The filter member of the Image structure specifies which image filter to
%  use. Blur specifies the blur factor where > 1 is blurry, < 1 is sharp.
%
%  The format of the ZoomImage method is:
%
%      Image *ZoomImage(const Image *image,const unsigned long columns,
%        const unsigned long rows,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o zoom_image: Method ZoomImage returns a pointer to the image after
%      scaling.  A null image is returned if there is a memory shortage.
%
%    o image: The image.
%
%    o columns: An integer that specifies the number of columns in the zoom
%      image.
%
%    o rows: An integer that specifies the number of rows in the scaled
%      image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *ZoomImage(const Image *image,const unsigned long columns,
  const unsigned long rows,ExceptionInfo *exception)
{
  Image
    *zoom_image;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  zoom_image=ResizeImage(image,columns,rows,image->filter,image->blur,
    exception);
  return(zoom_image);
}
