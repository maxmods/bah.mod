/*
  Copyright (C) 2003, 2008 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  Graphic Gems - Graphic Support Methods.
*/
#ifndef _MAGICK_GEM_H
#define _MAGICK_GEM_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Graphic gems define declarations.
*/
extern MagickExport double
  ExpandAffine(const AffineMatrix *),
  GenerateDifferentialNoise(const Quantum pixel,const NoiseType noise_type,
    unsigned int *seed);

extern MagickExport int
  GetOptimalKernelWidth(const double,const double),
  GetOptimalKernelWidth1D(const double,const double),
  GetOptimalKernelWidth2D(const double,const double);

extern MagickExport PixelPacket
  InterpolateColor(const Image *image,const double x_offset,
    const double y_offset,ExceptionInfo *exception);

extern MagickExport Quantum
  GenerateNoise(const Quantum,const NoiseType);

extern MagickExport void
  Contrast(const int,Quantum *,Quantum *,Quantum *),
  HSLTransform(const double,const double,const double,Quantum *,Quantum *,
    Quantum *),
  HWBTransform(const double,const double,const double,Quantum *,Quantum *,
    Quantum *),
  Hull(const long,const long,const unsigned long,const unsigned long,Quantum *,
    Quantum *,const int),
  IdentityAffine(AffineMatrix *),
  InterpolateViewColor(const ViewInfo *view,PixelPacket *color,
    const double x_offset,const double y_offset,ExceptionInfo *exception),
  Modulate(const double,const double,const double,Quantum *,Quantum *,
    Quantum *),
  TransformHSL(const Quantum,const Quantum,const Quantum,double *,double *,
    double *),
  TransformHWB(const Quantum,const Quantum,const Quantum,double *,double *,
    double *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
