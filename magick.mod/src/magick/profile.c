/*
% Copyright (C) 2003 - 2009 GraphicsMagick Group
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
%              PPPP   RRRR    OOO   FFFFF  IIIII  L      EEEEE                %
%              P   P  R   R  O   O  F        I    L      E                    %
%              PPPP   RRRR   O   O  FFF      I    L      EEE                  %
%              P      R R    O   O  F        I    L      E                    %
%              P      R  R    OOO   F      IIIII  LLLLL  EEEEE                %
%                                                                             %
%                   GraphicsMagick Image Profile Methods                      %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                               Bill Radcliffe                                %
%                              Bob Friesenhahn                                %
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
#include "magick/analyze.h"
#include "magick/color.h"
#include "magick/composite.h"
#include "magick/log.h"
#include "magick/map.h"
#include "magick/monitor.h"
#include "magick/omp_data_view.h"
#include "magick/pixel_iterator.h"
#include "magick/profile.h"
#include "magick/quantize.h"
#include "magick/resize.h"
#include "magick/transform.h"
#include "magick/utility.h"
#if defined(HasLCMS)
#if defined(HAVE_LCMS_LCMS_H)
#include <lcms/lcms.h>
#else
#include "lcms.h"
#endif
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%     A l l o c a t e I m a g e P r o f i l e I t e r a t o r                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AllocateImageProfileIterator allocates an iterator to traverse the
%  image profile list.  It is an error (i.e. will surely crash) to invoke
%  DeleteImageProfile() on the profile that the iterator is currently
%  referencing.  However, it is safe to delete a profile that the iterator
%  is not currently referencing. Inserting additional profiles does not
%  invalidate the current iterator.
%  
%
%  The format of the AllocateImageProfileIterator method is:
%
%      ImageProfileIterator AllocateImageProfileIterator(const Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
*/
MagickExport ImageProfileIterator
AllocateImageProfileIterator(const Image *image)
{
  if (!image->profiles)
    return 0;

  return (ImageProfileIterator) MagickMapAllocateIterator(image->profiles);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%     A p p e n d I m a g e P r o f i l e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AppendImageProfile adds a named profile to the image. If a profile with the
%  same name already exists, then the new profile data is appended to the
%  existing profile. If a null profile address is supplied, then an existing
%  profile is removed. The profile is copied into the image. Note that this
%  function does not execute CMS color profiles. Any existing CMS color
%  profile is simply added/updated. Use the ProfileImage() function in order
%  to execute a CMS color profile.
%
%  The format of the AppendImageProfile method is:
%
%      MaickPassFail AppendImageProfile(Image *image,const char *name,
%                                       const unsigned char *profile_chunk,
%                                       const size_t chunk_length)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o name: Profile name. Valid names are "8BIM", "ICM", "IPTC", XMP, or any
%                          unique text string.
%
%    o profile_chunk: Address of profile chunk to add or append. Pass zero
%               to remove an existing profile.
%
%    o length: The length of the profile chunk to add or append.
%
*/
MagickExport MagickPassFail
AppendImageProfile(Image *image,
		   const char *name,
		   const unsigned char *profile_chunk,
		   const size_t chunk_length)
{
  const unsigned char
    *existing_profile;

  size_t
    existing_length;

  MagickPassFail
    status;

  status=MagickFail;
  existing_length=0;
  existing_profile=(const unsigned char *) NULL;
  if (profile_chunk != (const unsigned char *) NULL)
    existing_profile=GetImageProfile(image,name,&existing_length);

  if ((profile_chunk == (const unsigned char *) NULL) ||
      (existing_profile == (const unsigned char *) NULL))
    {
      status=SetImageProfile(image,name,profile_chunk,chunk_length);
    }
  else
    {
      unsigned char
	*profile;

      size_t
	profile_length;

      profile_length=existing_length+chunk_length;
      if ((profile_length < existing_length) || 
	  ((profile=MagickAllocateMemory(unsigned char *,(size_t) profile_length)) ==
	   (unsigned char *) NULL))
	ThrowBinaryException(ResourceLimitError,MemoryAllocationFailed,
			     (char *) NULL);
      (void) memcpy(profile,existing_profile,existing_length);
      (void) memcpy(profile+existing_length,profile_chunk,chunk_length);
      status=SetImageProfile(image,name,profile,profile_length);
    }

  return status;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%     D e a l l o c a t e I m a g e P r o f i l e I t e r a t o r             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DeallocateImageProfileIterator deallocates an image profile iterator.
%
%  The format of the DeallocateImageProfileIterator method is:
%
%      void DeallocateImageProfileIterator(ImageProfileIterator profile_iterator)
%
%  A description of each parameter follows:
%
%    o profile_iterator: Profile iterator to deallocate.
%
*/
MagickExport void
DeallocateImageProfileIterator(ImageProfileIterator profile_iterator)
{
  if (profile_iterator != 0)
    MagickMapDeallocateIterator((MagickMapIterator) profile_iterator);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%     D e l e t e I m a g e P r o f i l e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DeleteImageProfile removes a named profile from the image.
%
%  The format of the DeleteImageProfile method is:
%
%      unsigned int DeleteImageProfile(Image *image,const char *name)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o name: Profile name. Valid names are "8BIM", "ICM", & "IPTC" or a
%                          generic profile name.
%
*/
MagickExport MagickPassFail
DeleteImageProfile(Image *image,const char *name)
{
  return(SetImageProfile(image,name,0,0));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%     G e t I m a g e P r o f i l e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageProfile returns a pointer to the named image profile if it is
%  present. A null pointer is returned if the named profile is not present.
%
%  Older versions of this function stored profiles named "8BIM" and "IPTC"
%  in the same storage location.  This is no longer the case.  However,
%  GetImageProfile() will try the alternate name if the specifically
%  requested profile name is not available.
%
%  The format of the GetImageProfile method is:
%
%      const unsigned char *GetImageProfile(const Image* image,
%                             const char *name, size_t *length)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o name: Profile name. Valid names are "8BIM", "ICM", "IPTC", "XMP" or any
%                          unique text string.
%
%    o length: Updated with profile length if profile is present.  Set to NULL
%              if length is not needed.
%
*/
MagickExport const unsigned char *
GetImageProfile(const Image* image, const char *name, size_t *length)
{
  const unsigned char
    *profile = 0;

  size_t
    profile_length=0;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(name != NULL);

  if (length)
    *length=0;

  if (!image->profiles)
    return 0;

  profile=MagickMapAccessEntry(image->profiles,name,&profile_length);

  if (!profile)
    {
      /*
        Support common alias names and work-alikes.
      */
      if (LocaleCompare("ICC",name) == 0)
        profile=MagickMapAccessEntry(image->profiles,"ICM",&profile_length);
      else if (LocaleCompare("ICM",name) == 0)
        profile=MagickMapAccessEntry(image->profiles,"ICC",&profile_length);
      else if (LocaleCompare("IPTC",name) == 0)
        profile=MagickMapAccessEntry(image->profiles,"8BIM",&profile_length);
      else if (LocaleCompare("8BIM",name) == 0)
        profile=MagickMapAccessEntry(image->profiles,"IPTC",&profile_length);
    }

  if (length)
    *length=profile_length;

  return profile;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%     N e x t I m a g e P r o f i l e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  NextImageProfile iterates forward to the next image profile. The profile
%  name is returned along with the profile data, and length.  If there are
%  no more entries in the list, then MagickFail is returned.
%
%  The format of the AllocateImageProfileIterator method is:
%
%      MagickPassFail NextImageProfile(ImageProfileIterator profile_iterator,
%                             const char **name, const unsigned char **profile,
%                             size_t *length)
%
%  A description of each parameter follows:
%
%    o profile_iterator: Profile iterator.
%
%    o name: Address of pointer to update with address of name.
%
%    o profile: Address of pointer to update with location of profile data.
%
%    o length: Address of parameter to update with profile length.
%
*/
MagickExport MagickPassFail
NextImageProfile(ImageProfileIterator profile_iterator,
                 const char **name,
                 const unsigned char **profile,
                 size_t *length)
{
  MagickMapIterator
    map_iterator;

  MagickPassFail
    status;

  assert(name != (const char **) NULL);
  assert(length != (size_t *) NULL);

  if (profile_iterator == 0)
    return (MagickFail);

  map_iterator=(MagickMapIterator) profile_iterator;
  status=MagickMapIterateNext(map_iterator,name);
  if (status != MagickFail)
    *profile=MagickMapDereferenceIterator(map_iterator,length);
  return (status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P r o f i l e I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ProfileImage() adds, applies, or removes a ICM, IPTC, or generic profile
%  from an image.  If the profile is NULL, it is removed from the image
%  otherwise added (or applied).  Use a name of '*' and a profile of NULL to
%  remove all profiles from the image. Ownership of the profile is
%  transferred to GraphicsMagick (it should not be altered or deallocated)
%  unless the clone option is set to True.
%
%  ICC ICM profiles are a special case and are handled as follows:
%
%  If there is no ICM profile currently associated with the image, then
%  the profile is simply associated with the image and the image pixels
%  are not altered.
%  
%  If there is already a ICM profile associated with the image, then
%  the colorspace transform described by the existing and new profiles
%  is applied to the image pixels, and the new profile is associated
%  with the image.
%
%  The format of the ProfileImage method is:
%
%      unsigned int ProfileImage(Image *image,const char *name,
%        unsigned char *profile,const size_t length,unsigned int clone)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o name: Name of profile to add or remove: ICM, IPTC, or generic profile.
%
%    o profile: The profile.  Can not be 'const' due to 'clone' option but
%             is treated as 'const' if 'clone' is set to MagickTrue.
%
%    o length: The length of the profile.
%
%    o clone: If set True, then copy the profile rather than taking
%             ownership of it.
%
%
*/
#if defined(HasLCMS)
#if defined(LCMS_VERSION) && (LCMS_VERSION > 1010)
static int
lcmsReplacementErrorHandler(int ErrorCode, const char *ErrorText)
{
  ExceptionType
    type;

  switch(ErrorCode)
  {
    case LCMS_ERRC_ABORTED:
      type=TransformError;
      break;
    default:
    case LCMS_ERRC_RECOVERABLE:
    case LCMS_ERRC_WARNING:
      type=TransformWarning;
      break;
  }
  (void) LogMagickEvent(type,GetMagickModule(),"lcms: #%d, %s",
    ErrorCode,(ErrorText != (char *) NULL) ? ErrorText : "No error text");
  return 1; /* tells lcms that we handled the problem */
}
#endif /* LCMS_VERSION > 1010 */

typedef struct _ProfilePacket
{
  unsigned short
    red,
    green,
    blue,
    black;
} ProfilePacket;

typedef struct _TransformInfo
{
  cmsHPROFILE     source_profile;     /* input profile */
  cmsHPROFILE     target_profile;     /* output profile */
  DWORD           source_type;        /* input pixel format */
  DWORD           target_type;        /* output pixel format */
  int             intent;             /* rendering intent */
  DWORD           flags;              /* create transform flags */
  //cmsHTRANSFORM   transform;          /* LCMS transform */
  ThreadViewDataSet *transform;       /* Thread-specific transforms */
  ColorspaceType  source_colorspace;  /* source image transform colorspace */
  ColorspaceType  target_colorspace;  /* target image transform colorspace */
} TransformInfo;

static MagickPassFail
ProfileImagePixels(void *mutable_data,         /* User provided mutable data */
                   const void *immutable_data, /* User provided immutable data */
                   Image *image,               /* Modify image */
                   PixelPacket *pixels,        /* Pixel row */
                   IndexPacket *indexes,       /* Pixel row indexes */
                   const long npixels,         /* Number of pixels in row */
                   ExceptionInfo *exception)   /* Exception report */
{
  const TransformInfo
    *xform = (const TransformInfo *) immutable_data;

  register long
    i;

  cmsHTRANSFORM
    transform;

  const ColorspaceType
    source_colorspace = xform->source_colorspace;

  const ColorspaceType
    target_colorspace = xform->target_colorspace;

  ProfilePacket
    alpha,
    beta;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(exception);

  transform=(cmsHTRANSFORM) AccessThreadViewData(xform->transform);

  /*
    TODO: This may be optimized to use PixelPackets instead
    of moving PixelPacket components to and from ProfilePackets.
    The transform types below, then, must match #ifdef's in the
    PixelPacket struct definition and should be optimized
    based on Quantum size. Some (if not all?) YCbCr and LUV
    profiles are (TIFF) scanline oriented, so transforming
    one pixel at a time does not work for those profiles.

    Notice that the performance penalty of transforming only
    one pixel at a time is very small and probably not worth
    optimizing.
  */

  for (i=0; i < npixels; i++)
    {
      alpha.red=ScaleQuantumToShort(pixels[i].red);
      if (source_colorspace != GRAYColorspace)
        {
          alpha.green=ScaleQuantumToShort(pixels[i].green);
          alpha.blue=ScaleQuantumToShort(pixels[i].blue);
          if (source_colorspace == CMYKColorspace)
            alpha.black=ScaleQuantumToShort(pixels[i].opacity);
        }
      cmsDoTransform(transform,&alpha,&beta,1);
      pixels[i].red=ScaleShortToQuantum(beta.red);
      if (IsGrayColorspace(target_colorspace))
        {
          pixels[i].green=pixels[i].red;
          pixels[i].blue=pixels[i].red;
        }
      else
        {
          pixels[i].green=ScaleShortToQuantum(beta.green);
          pixels[i].blue=ScaleShortToQuantum(beta.blue);
        }
      if (image->matte)
        {
          if ((source_colorspace == CMYKColorspace) &&
              (target_colorspace != CMYKColorspace))
            pixels[i].opacity=indexes[i];
          else
            if ((source_colorspace != CMYKColorspace) &&
                (target_colorspace == CMYKColorspace))
              indexes[i]=pixels[i].opacity;
        }
      if (target_colorspace == CMYKColorspace)
        pixels[i].opacity=ScaleShortToQuantum(beta.black);
    }

  return MagickPass;
}

static void MagickFreeCMSTransform(void * cmsTransformVoid)
{
  cmsHTRANSFORM
    cmsTransform=(cmsHTRANSFORM) cmsTransformVoid;

  cmsDeleteTransform(cmsTransform);
}

static const char *
PixelTypeToString(int pixel_type)
{
const char *
  result="";

  switch (pixel_type)
    {
    case PT_ANY:    result="ANY"   ; break;
    case PT_GRAY:   result="GRAY"  ; break;
    case PT_RGB:    result="RGB"   ; break;
    case PT_CMY:    result="CMY"   ; break;
    case PT_CMYK:   result="CMYK"  ; break;
    case PT_YCbCr:  result="YCbCr" ; break;
    case PT_YUV:    result="YUV (Lu'v')"; break;
    case PT_XYZ:    result="XYZ"   ; break;
    case PT_Lab:    result="Lab"   ; break;
    case PT_YUVK:   result="YUVK (Lu'v'K)" ; break;
    case PT_HSV:    result="HSV"   ; break;
    case PT_HLS:    result="HLS"   ; break;
    case PT_Yxy:    result="Yxy"   ; break;
    case PT_HiFi:   result="HiFi"  ; break;
    case PT_HiFi7:  result="HiFi7" ; break;
    case PT_HiFi8:  result="HiFi8" ; break;
    case PT_HiFi9:  result="HiFi9" ; break;
    case PT_HiFi10: result="HiFi10"; break;
    case PT_HiFi11: result="HiFi11"; break;
    case PT_HiFi12: result="HiFi12"; break;
    case PT_HiFi13: result="HiFi13"; break;
    case PT_HiFi14: result="HiFi14"; break;
    case PT_HiFi15: result="HiFi15"; break;
    }

  return result;
}

#endif /* defined(HasLCMS) */

#define ProfileImageText "[%s] Color Transform Pixels..."
MagickExport MagickPassFail
ProfileImage(Image *image,const char *name,unsigned char *profile,
             const size_t length,MagickBool clone)
{
  MagickPassFail
    status=MagickPass;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (name == (const char *) NULL)
    ThrowBinaryException3(OptionError,NoProfileNameWasGiven,
                          UnableToAddOrRemoveProfile);
  if ((profile == (const unsigned char *) NULL) || (length == 0))
    {
      /*
        Remove an ICM, IPTC, or generic profile from the image.
      */
      char
        arg_string[MaxTextExtent],
        profile_remove[MaxTextExtent];

      const char
        *profile_name;
      
      size_t
        profile_length;
      
      const unsigned char *
        profile_info;
      
      ImageProfileIterator
        profile_iterator;

      char
        **argv;

      int
        argc;

      long
        i;

      (void) strlcpy(arg_string,name,sizeof(arg_string));
      LocaleUpper(arg_string);
      for (i=0; arg_string[i] != '\0'; i++)
        if (arg_string[i] == ',')
          arg_string[i] = ' ';
      argv=StringToArgv(arg_string,&argc);
      profile_iterator=AllocateImageProfileIterator(image);
      profile_remove[0]=0;
      while(NextImageProfile(profile_iterator,&profile_name,&profile_info,
                             &profile_length) != MagickFail)
        {
          if (strlen(profile_remove))
            {
              (void) DeleteImageProfile(image,profile_remove);
              profile_remove[0]=0;
            }
          for (i=1 ; i < argc ; i++)
            {
              if ((*argv[i] == '!') && (LocaleCompare(profile_name,argv[i]+1) == 0))
                break;                
              if (GlobExpression(profile_name,argv[i]))
                {
                  (void) strlcpy(profile_remove,profile_name,sizeof(profile_remove));
                  break;
                }
            }
        }
      DeallocateImageProfileIterator(profile_iterator);
      if (strlen(profile_remove))
        (void) DeleteImageProfile(image,profile_remove);

      for(i=0; argv[i] != NULL; i++)
        MagickFreeMemory(argv[i]);
      MagickFreeMemory(argv);

      return(MagickPass);
    }
  /*
    Add a ICM, IPTC, or generic profile to the image.
  */
  if ((LocaleCompare("8bim",name) == 0) || (LocaleCompare("iptc",name) == 0))
    {
      if (clone)
        {
          (void) SetImageProfile(image,name,profile,length);
        }
      else
        {
          (void) SetImageProfile(image,name,profile,length);
          MagickFreeMemory(profile);
        }
      return(MagickPass);
    }
  if (LocaleCompare("icm",name) == 0)
    {
      const unsigned char
        *existing_profile;

      size_t
        existing_profile_length=0;

      /* Check for identical input and output profiles. Return on identity. */
      existing_profile=GetImageProfile(image,"ICM",&existing_profile_length);

      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                            "New Profile: %lu bytes, Existing Profile: %lu bytes",
                            (unsigned long) length,
                            (unsigned long) existing_profile_length);

      if ((length != 0) && (length == existing_profile_length) &&
          (memcmp(existing_profile,profile,length) == 0))
        {
          return(MagickPass);
        }

      /* Convert to new colors if we have both an old and a new profile. */
      if ((existing_profile_length != 0) && (length != 0))
        {
#if defined(HasLCMS)

          TransformInfo
            xform;

          MagickBool
            transform_colormap;

          /*
            Transform pixel colors as defined by the color profiles.
          */
#if defined(LCMS_VERSION) && (LCMS_VERSION > 1010)
          cmsSetErrorHandler(lcmsReplacementErrorHandler);
#else
          (void) cmsErrorAction(LCMS_ERROR_SHOW);
#endif
          xform.source_profile=cmsOpenProfileFromMem((unsigned char *) existing_profile,
						     existing_profile_length);
          xform.target_profile=cmsOpenProfileFromMem((unsigned char *) profile,
						     length);
          if ((xform.source_profile == (cmsHPROFILE) NULL) ||
              (xform.target_profile == (cmsHPROFILE) NULL))
            ThrowBinaryException3(ResourceLimitError,UnableToManageColor,
                                  UnableToOpenColorProfile);

          switch (cmsGetColorSpace(xform.source_profile))
            {
            case icSigXYZData:
              {
                xform.source_colorspace=XYZColorspace;
                xform.source_type=TYPE_XYZ_16;
                break;
              }
            case icSigLabData:
              {
                xform.source_colorspace=LABColorspace;
                xform.source_type=TYPE_Lab_16;
                break;
              }
            case icSigCmykData:
              {
                xform.source_colorspace=CMYKColorspace;
                xform.source_type=TYPE_CMYK_16;
                break;
              }
            case icSigYCbCrData:
              {
                xform.source_colorspace=YCbCrColorspace;
                xform.source_type=TYPE_YCbCr_16;
                break;
              }
            case icSigLuvData:
              {
                xform.source_colorspace=YUVColorspace;
                xform.source_type=TYPE_YUV_16;
                break;
              }
            case icSigGrayData:
              {
                xform.source_colorspace=GRAYColorspace;
                xform.source_type=TYPE_GRAY_16;
                break;
              }
            case icSigRgbData:
              {
                xform.source_colorspace=RGBColorspace;
                xform.source_type=TYPE_RGB_16;
                break;
              }
            default:
              {
                xform.source_colorspace=UndefinedColorspace;
                xform.source_type=TYPE_RGB_16;
                break;
              }
            }
          switch (cmsGetColorSpace(xform.target_profile))
            {
            case icSigXYZData:
              {
                xform.target_colorspace=XYZColorspace;
                xform.target_type=TYPE_XYZ_16;
                break;
              }
            case icSigLabData:
              {
                xform.target_colorspace=LABColorspace;
                xform.target_type=TYPE_Lab_16;;
                break;
              }
            case icSigCmykData:
              {
                xform.target_colorspace=CMYKColorspace;
                xform.target_type=TYPE_CMYK_16;
                break;
              }
            case icSigYCbCrData:
              {
                xform.target_colorspace=YCbCrColorspace;
                xform.target_type=TYPE_YCbCr_16;
                break;
              }
            case icSigLuvData:
              {
                xform.target_colorspace=YUVColorspace;
                xform.target_type=TYPE_YUV_16;
                break;
              }
            case icSigGrayData:
              {
                xform.target_colorspace=GRAYColorspace;
                xform.target_type=TYPE_GRAY_16;
                break;
              }
            case icSigRgbData:
              {
                xform.target_colorspace=RGBColorspace;
                xform.target_type=TYPE_RGB_16;
                break;
              }
            default:
              {
                xform.target_colorspace=UndefinedColorspace;
                xform.target_type=TYPE_RGB_16;
                break;
              }
            }

          /* Colorspace undefined */
          if ((xform.source_colorspace == UndefinedColorspace) ||
              (xform.target_colorspace == UndefinedColorspace))
            {
              (void) cmsCloseProfile(xform.source_profile);
              (void) cmsCloseProfile(xform.target_profile);
              ThrowBinaryException3(ImageError,UnableToAssignProfile,
                                    ColorspaceColorProfileMismatch);
            }
          /* Gray colorspace */
          if (IsGrayColorspace(xform.source_colorspace) &&
              !IsGrayImage(image,&image->exception))
            {
              (void) cmsCloseProfile(xform.source_profile);
              (void) cmsCloseProfile(xform.target_profile);
              ThrowBinaryException3(ImageError,UnableToAssignProfile,
                                    ColorspaceColorProfileMismatch);
            }
          /* CMYK colorspace */
          if (IsCMYKColorspace(xform.source_colorspace) &&
              !IsCMYKColorspace(image->colorspace))
            {
              (void) cmsCloseProfile(xform.source_profile);
              (void) cmsCloseProfile(xform.target_profile);
              ThrowBinaryException3(ImageError,UnableToAssignProfile,
                                    ColorspaceColorProfileMismatch);
            }
          /* YCbCr colorspace */
          if (IsYCbCrColorspace(xform.source_colorspace) &&
              !IsYCbCrColorspace(image->colorspace))
            {
              (void) cmsCloseProfile(xform.source_profile);
              (void) cmsCloseProfile(xform.target_profile);
              ThrowBinaryException3(ImageError,UnableToAssignProfile,
                                    ColorspaceColorProfileMismatch);
            }
          /* Verify that source colorspace type is supported */
          if (!IsGrayColorspace(xform.source_colorspace) &&
              !IsCMYKColorspace(xform.source_colorspace) &&
	      !IsLABColorspace(xform.source_colorspace) &&
              !IsYCbCrColorspace(xform.source_colorspace) &&
              !IsRGBColorspace(image->colorspace))
            {
              (void) cmsCloseProfile(xform.source_profile);
              (void) cmsCloseProfile(xform.target_profile);
              ThrowBinaryException3(ImageError,UnableToAssignProfile,
                                    ColorspaceColorProfileMismatch);
            }

          (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                                "Source pixel format: COLORSPACE=%s SWAPFIRST=%d "
				"FLAVOR=%d PLANAR=%d ENDIAN16=%d DOSWAP=%d "
				"EXTRA=%d CHANNELS=%d BYTES=%d",
                                PixelTypeToString((int) T_COLORSPACE(xform.source_type)),
                                (int) T_SWAPFIRST(xform.source_type),
                                (int) T_FLAVOR(xform.source_type),
                                (int) T_PLANAR(xform.source_type),
                                (int) T_ENDIAN16(xform.source_type),
                                (int) T_DOSWAP(xform.source_type),
                                (int) T_EXTRA(xform.source_type),
                                (int) T_CHANNELS(xform.source_type),
                                (int) T_BYTES(xform.source_type));

          (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                                "Target pixel format: COLORSPACE=%s SWAPFIRST=%d "
				"FLAVOR=%d PLANAR=%d ENDIAN16=%d DOSWAP=%d "
				"EXTRA=%d CHANNELS=%d BYTES=%d",
                                PixelTypeToString((int) T_COLORSPACE(xform.target_type)),
                                (int) T_SWAPFIRST(xform.target_type),
                                (int) T_FLAVOR(xform.target_type),
                                (int) T_PLANAR(xform.target_type),
                                (int) T_ENDIAN16(xform.target_type),
                                (int) T_DOSWAP(xform.target_type),
                                (int) T_EXTRA(xform.target_type),
                                (int) T_CHANNELS(xform.target_type),
                                (int) T_BYTES(xform.target_type));

          switch (image->rendering_intent)
            {
            case AbsoluteIntent:
              xform.intent=INTENT_ABSOLUTE_COLORIMETRIC;
              break;
            case PerceptualIntent: 
              xform.intent=INTENT_PERCEPTUAL; 
              break;
            case RelativeIntent: 
              xform.intent=INTENT_RELATIVE_COLORIMETRIC;
              break;
            case SaturationIntent: 
              xform.intent=INTENT_SATURATION; 
              break;
            default: 
              xform.intent=INTENT_PERCEPTUAL; 
              break;
            }

          /*
            Transform just the colormap if the image is colormapped and we're
            not transforming from gray to RGB/CMYK. A gray to RGB/CMYK
            transformation must create a direct class image for the new image
            to match the color profile even if the new image only has gray
            colors. CMYK images are never color mapped.
          */
          transform_colormap=(image->storage_class == PseudoClass) &&
            (xform.target_colorspace != CMYKColorspace) &&
            ((xform.source_colorspace != GRAYColorspace) ||
             (xform.source_colorspace == xform.target_colorspace));

	  /* build pre-computed transforms? */
	  xform.flags=(transform_colormap ? cmsFLAGS_NOTPRECALC : 0);

	  xform.transform=AllocateThreadViewDataSet(MagickFreeCMSTransform,
						    image,&image->exception);
	  if (xform.transform == (ThreadViewDataSet *) NULL)
	    status=MagickFail;
	  if (status != MagickFail)
	    {
	      cmsHTRANSFORM
		transform;

	      unsigned int
		index;

	      for (index=0 ; index < GetThreadViewDataSetAllocatedViews(xform.transform); index++)
		{
		  transform=cmsCreateTransform(xform.source_profile, /* input profile */
					       xform.source_type,    /* input pixel format */
					       xform.target_profile, /* output profile */
					       xform.target_type,    /* output pixel format */
					       xform.intent,         /* rendering intent */
					       xform.flags           /* pre-computed transforms? */
					       );
		  if (transform == (cmsHTRANSFORM) NULL)
		    {
		      status=MagickFail;
		      break;
		    }
		  AssignThreadViewData(xform.transform,index,transform);
		}
	    }
          (void) cmsCloseProfile(xform.source_profile);
          (void) cmsCloseProfile(xform.target_profile);
	  if (status == MagickFail)
            {
	      DestroyThreadViewDataSet(xform.transform);
              ThrowBinaryException3(ResourceLimitError,UnableToManageColor,
                                    UnableToCreateColorTransform);
            }

          if (transform_colormap)
            {
              (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                                    "Performing pseudo class color conversion");

              (void) ProfileImagePixels(NULL,
                                        &xform,
                                        image,
                                        image->colormap,
                                        (IndexPacket *) NULL,
                                        image->colors,
                                        &image->exception);

              (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                                    "Completed pseudo class color conversion");
              status &= SyncImage(image);
            }
          else
            {
              (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                                    "Performing direct class color conversion");
              if (image->storage_class == PseudoClass)
                {
                  status &= SyncImage(image);
                  image->storage_class=DirectClass;
                }
              if (xform.target_colorspace == CMYKColorspace)
                image->colorspace=xform.target_colorspace;

              status=PixelIterateMonoModify(ProfileImagePixels,
                                            NULL,
                                            ProfileImageText,
                                            NULL,&xform,0,0,image->columns,image->rows,
                                            image,&image->exception);

              (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                                    "Completed direct class color conversion");

            }
          image->colorspace=xform.target_colorspace;
          /*
            We can't be sure black and white stays exactly black and white
            and that gray colors transform to gray colors.
          */
          image->is_grayscale=IsGrayColorspace(xform.target_colorspace);
          image->is_monochrome=False;
	  DestroyThreadViewDataSet(xform.transform);

          /*
            Throw away the old profile after conversion before we
            assign a new one.
          */
          DeleteImageProfile(image,"ICM");
#else
          ThrowBinaryException(MissingDelegateError,LCMSLibraryIsNotAvailable,
                               image->filename);
#endif
        }

      /*
        TODO: If the image *did not* already have a color profile,
        verify that the colorspace of the new profile is valid for the
        colorspace of the image. If LCMS is not available we should
        refuse to assign a new profile (just like we're refusing a
        conversion above) as we can't be sure the assigment is valid.
        We might be trying to assign a CMYK profile to an RGB image,
        for instance.
      */
      
      (void) SetImageProfile(image,"ICM",profile,length);
      if (!clone)
        MagickFreeMemory(profile);
      return(status);
    }

  status &= SetImageProfile(image,name,profile,length);
  if (!clone)
    MagickFreeMemory(profile);

  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%     S e t I m a g e P r o f i l e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetImageProfile adds a named profile to the image. If a profile with the
%  same name already exists, then it is replaced. If a null profile address
%  is supplied, then an existing profile is removed. The profile is copied
%  into the image. Note that this function does not execute CMS color
%  profiles. Any existing CMS color profile is simply replaced. Use the
%  ProfileImage() function in order to execute a CMS color profile.
%
%  Older versions of this function stored profiles named "8BIM" and "IPTC"
%  in the same storage location.  This is no longer the case.  However,
%  GetImageProfile() will try the alternate name if the specifically
%  requested profile name is not available.  Note that when trying to remove
%  a profile, it may be necessary to remove both names in order for an
%  "IPTC" profile to no longer be included in output file formats.
%
%  The format of the SetImageProfile method is:
%
%      unsigned int SetImageProfile(Image *image,const char *name,
%                            const unsigned char *profile,const size_t length)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o name: Profile name. Valid names are "8BIM", "ICM", "IPTC", XMP, or any
%                          unique text string.
%
%    o profile: Address of profile to add. Pass zero to remove an existing
%               profile.
%
%    o length: The length of the profile.
%
*/
MagickExport MagickPassFail
SetImageProfile(Image *image,const char *name, const unsigned char *profile,
                const size_t length)
{
  char
    ucase_name[MaxTextExtent];

  unsigned int
    status = MagickPass;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(name != NULL);

  if (strlcpy(ucase_name,name,sizeof(ucase_name)) >= sizeof(ucase_name))
    {
      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                            "Profile name too long! (%s)",name);
      return MagickFail;
    }
  LocaleUpper(ucase_name);

  if ((profile == 0) && (image->profiles != 0))
    {
      /*
        Remove existing entry.
      */
      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                            "Removing %s profile",name);
      status &= MagickMapRemoveEntry(image->profiles,name);
    }
  else
    {
      /*
        Add or replace entry.
      */
      if (image->profiles == 0)
        image->profiles=MagickMapAllocateMap(MagickMapCopyBlob,
                                             MagickMapDeallocateBlob);

      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                            "Adding %s profile with length %ld bytes",name,
			    (unsigned long) length);
      status &= MagickMapAddEntry(image->profiles,name,profile,length,
                                  &image->exception);
    }
  return (status);
}
