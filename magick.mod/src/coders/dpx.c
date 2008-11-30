/*
% Copyright (C) 2005 GraphicsMagick Group
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            DDDD   PPPP   X   X                              %
%                            D   D  P   P   X X                               %
%                            D   D  PPPP    XXX                               %
%                            D   D  P       X X                               %
%                            DDDD   P      X   X                              %
%                                                                             %
%                                                                             %
%               Read/Write SMTPE 268M-2003 DPX Image Format.                  %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                              Bob Friesenhahn                                %
%                                March 2005                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
% Supported features:
%
%   Anything which can be read, can also be written.
%   All DPX header information is saved as image attributes and saved
%   when the image is written.
%
%   Colorspaces:
%    RGB
%    Log RGB (density range = 2.048)
%    Grayscale (Luma)
%    YCbCr 4:4:4 and 4:2:2 (use -sampling-factor 2x1 for 4:2:2)
%
%   Storage:
%    Bits per sample of 1, 8, 10, 12, and 16.
%    Packed, or fill type A or B for 10/12 bits.
%    All RGB-oriented element types (R, G, B, A, RGB, RGBA, ABGR).
%    All YCbCr-oriented element types.
%    Planar (multi-element) storage fully supported.
%    Alpha (key channel) may be stored in a separate element.
%
% Not currently supported:
%
%   Colorspaces:
%    Composite Video
%
%   Storage:
%    Bits per sample of 32 and 64 (floating point).
%    Depth.
%
% Notes about byte order:
%
%    The DPX specification doesn't say anything about how endian byte
%    order should influence the image pixels.  The 1994 version of the
%    specification suggests very strongly that *all* formats are
%    accessed as an array of 32-bit words (therefore implying 32-bit
%    swapping) but the 2003 specification has ammended this (ambiguously)
%    to imply the mixed use of 16 and 32 bit words.  As we all know,
%    performing endian swapping on two 16-bit values does not have the
%    same effect as performing endian swapping on one 32-bit value (the
%    order of the two 16-bit samples is reversed) so the 2003 specification
%    is not compatible with the 1994 specification.
%
%    Due to the lack of an unambiguous standard, GraphicsMagick is currently
%    using the following endian rules.  These rules may change at any time:
%
%      o 8 bit format is always written in ascending order so endian rules
%        do not apply.
%
%      o Packed 10 and 12-bit formats are based on a 32-bit word, so
%        32-bit words are byte-swapped to native order.
%
%      o 12-bit filled format, and 16 bit format are based on 16-bit words,
%        so 16-bit words are byte-swapped to native order.
%
%      o 10-bit filled format is based on a 32-bit word, so 32-bit words
%        are byte-swapped to native order.
%
%    The default format is big-endian.
%
% Additional notes:
%
%    I have received many 4:2:2 YCbCr files in which the Cb/Cr channels are
%    swapped from my interpretation of the specification. This has caused
%    considerable consternation.  As a result I have decided to observe the
%    "majority rules" rule and follow the apparent direction set by systems
%    set by Quantel iQ, and DVS Clipster.  Therefore, 4:2:2 YCbCr files have
%    Cb/Cr intentionally swapped from the standard.  But 4:4:4 YCbCr files
%    use the ordering suggested by the standard since all such files received
%    to date follow the standard.
%
%     If a YCbCr file reads with vertical or color distortion, try adding
%    "-define dpx:swap-samples=true" when reading the image. The same option
%    may be used to write YCbCr files with swapped samples.
%
%    Note that some YCbCr files are interlaced.  Interlaced files combine
%    two fields into one image.  This means that 1/60th of a second has
%    elapsed between odd and even rows.  Distortion will be evident if
%    the camera or observed object is in motion.  This distortion is not
%    due to a defect in GraphicsMagick.
%
%    The DPX specification does not specify row alignment on a 32-bit
%    word boundary, but unofficial documentation (e.g. the O'Reilly
%    GFF book) and "lore" suggest it.  The GraphicsMagick implementation
%    does pad rows to the next word (16-bit or 32-bit) boundary.  It is not
%    clear if the End-of-line padding field should reflect padding to a
%    word boundary.
%    
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/attribute.h"
#include "magick/blob.h"
#include "magick/bit_stream.h"
#include "magick/pixel_cache.h"
#include "magick/color.h"
#include "magick/constitute.h"
#include "magick/enum_strings.h"
#include "magick/magick_endian.h"
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/omp_data_view.h"
#include "magick/profile.h"
#include "magick/resize.h"
#include "magick/utility.h"
#include "magick/version.h"

/*
  Define STATIC to nothing so that normally static functions are
  externally visible in the symbol table (for profiling).
*/
#undef STATIC
#define STATIC static

/*
  Forward declaractions.
*/
static unsigned int
  WriteDPXImage(const ImageInfo *,Image *);

typedef char ASCII;
typedef magick_uint8_t U8;
typedef magick_uint16_t U16;
typedef magick_uint32_t U32;
typedef float R32;
typedef magick_uint16_t sample_t;

/*
  Union to allow R32 to be accessed as an unsigned U32 type for "unset
  value" access.
*/
typedef union _R32_u
{
  U32   u;
  R32   f;
} R32_u;

#define SET_UNDEFINED_U8(value)  (value=0xFFU)
#define SET_UNDEFINED_U16(value) (value=0xFFFFU)
#define SET_UNDEFINED_U32(value) (value=0xFFFFFFFFU)
#define SET_UNDEFINED_R32(value) (((R32_u*) &value)->u=~0U);
#define SET_UNDEFINED_ASCII(value) ((void) memset(value,0,sizeof(value)))

#define IS_UNDEFINED_U8(value) (value == ((U8) 0xFFU))
#define IS_UNDEFINED_U16(value) (value == ((U16) 0xFFFFU))
#define IS_UNDEFINED_U32(value) (value == ((U32) 0xFFFFFFFFU))
#define IS_UNDEFINED_R32(value) (((R32_u*) &value)->u == ((U32) ~0U))
#define IS_UNDEFINED_ASCII(value) (!(value[0] > 0))

/*
  Round the starting address of pixel data to this offset.  The DPX
  specification recommends rounding up to the next 8K boundary past
  the file header.
*/
#define IMAGE_DATA_ROUNDING 8192

/*
  Image element descriptors.
*/
typedef enum
{
  ImageElementUnspecified=0,
  ImageElementRed=1,
  ImageElementGreen=2,
  ImageElementBlue=3,
  ImageElementAlpha=4,
  ImageElementLuma=6,
  ImageElementColorDifferenceCbCr=7,
  ImageElementDepth=8,
  ImageElementCompositeVideo=9,
  ImageElementRGB=50,              /* BGR order */
  ImageElementRGBA=51,             /* BGRA order */
  ImageElementABGR=52,             /* ARGB order */
  ImageElementCbYCrY422=100,       /* SMPTE 125M, 4:2:2 */
  ImageElementCbYACrYA4224=101,    /* 4:2:2:4 */
  ImageElementCbYCr444=102,        /* 4:4:4 */
  ImageElementCbYCrA4444=103,      /* 4:4:4:4 */
  ImageElementUserDef2Element=150, /* User-defined 2-component element */
  ImageElementUserDef3Element=151, /* User-defined 3-component element */
  ImageElementUserDef4Element=152, /* User-defined 4-component element */
  ImageElementUserDef5Element=153, /* User-defined 5-component element */
  ImageElementUserDef6Element=154, /* User-defined 6-component element */
  ImageElementUserDef7Element=155, /* User-defined 7-component element */
  ImageElementUserDef8Element=156  /* User-defined 8-component element */
} DPXImageElementDescriptor;

/*
  Transfer characteristic enumerations.
*/
typedef enum
{
  TransferCharacteristicUserDefined=0,
  TransferCharacteristicPrintingDensity=1,
  TransferCharacteristicLinear=2,
  TransferCharacteristicLogarithmic=3,
  TransferCharacteristicUnspecifiedVideo=4,
  TransferCharacteristicSMTPE274M=5,     /* 1920x1080 TV */
  TransferCharacteristicITU_R709=6,      /* ITU R709 */
  TransferCharacteristicITU_R601_625L=7, /* 625 Line */
  TransferCharacteristicITU_R601_525L=8, /* 525 Line */
  TransferCharacteristicNTSCCompositeVideo=9,
  TransferCharacteristicPALCompositeVideo=10,
  TransferCharacteristicZDepthLinear=11,
  TransferCharacteristicZDepthHomogeneous=12
} DPXTransferCharacteristic;

/*
  Colorimetric enumerations.
*/
typedef enum
{
  ColorimetricUserDefined=0,       /* User defined */
  ColorimetricPrintingDensity=1,   /* Printing density */
  ColorimetricLinear=2,            /* Linear */
  ColorimetricLogarithmic=3,       /* Logarithmic */
  ColorimetricUnspecifiedVideo=4,
  ColorimetricSMTPE274M=5,         /* 1920x1080 TV */
  ColorimetricITU_R709=6,          /* ITU R709 */
  ColorimetricITU_R601_625L=7,     /* 625 Line ITU R601-5 B & G */
  ColorimetricITU_R601_525L=8,     /* 525 Line ITU R601-5 M */
  ColorimetricNTSCCompositeVideo=9,
  ColorimetricPALCompositeVideo=10,
  ColorimetricZDepthLinear=11,
  ColorimetricZDepthHomogeneous=12
} DPXColorimetric;

/*
  Packing methods for filled words.
*/
typedef enum
{
  PackingMethodPacked=0,           /* Packed with no padding */
  PackingMethodWordsFillLSB=1,     /* Method 'A', padding bits in LSB of 32-bit word */
  PackingMethodWordsFillMSB=2      /* Method 'B', padding bits in MSB of 32-bit word (deprecated) */
}  ImageComponentPackingMethod;

typedef struct _DPXFileInfo
{
  U32   magic;                     /* Magick number (SDPX ASCII) */
  U32   image_data_offset;         /* Offset to image data in bytes */
  ASCII header_format_version[8];  /* Version number of header format */
  U32   file_size;                 /* Total image file size in bytes  */
  U32   ditto_key;                 /* (0 = same as previous frame; 1 = new) */
  U32   generic_section_length;    /* Generic section header length in bytes */
  U32   industry_section_length;   /* Industry specific header length in bytes */
  U32   user_defined_length;       /* User defined header length in bytes */
  ASCII image_filename[100];       /* Image filename */
  ASCII creation_datetime[24];     /* Creation date/time: yyyy:mm:dd:hh:mm:ssLTZ */
  ASCII creator[100];              /* Creator */
  ASCII project_name[200];         /* Project name */
  ASCII copyright[200];            /* Right to use or copyright */
  U32   encryption_key;            /* Encryption key (FFFFFFFF unencrypted ) */
  ASCII reserved[104];             /* Reserved for future use */
} DPXFileInfo;

typedef struct _DPXImageElement
{
  U32   data_sign;                 /* Data sign (0 = unsigned; 1 = signed) */
  U32   reference_low_data_code;   /* Reference low data code value */
  R32   reference_low_quantity;    /* Low quantity represented */
  U32   reference_high_data_code;  /* Reference high data code value */
  R32   reference_high_quantity;   /* Reference high quantity represented */
  U8    descriptor;                /* Descriptor */
  U8    transfer_characteristic;   /* Transfer characteristic */
  U8    colorimetric;              /* Colorimetric specification */
  U8    bits_per_sample;           /* Bit depth */
  U16   packing;                   /* Packing */
  U16   encoding;                  /* Encoding */
  U32   data_offset;               /* Offset to data */
  U32   eol_pad;                   /* End of line padding */
  U32   eoi_pad;                   /* End of image padding */
  ASCII description[32];           /* Description of image element */
} DPXImageElement;

typedef struct _DPXImageInfo
{
  U16   orientation;               /* Image orientation */
  U16   elements;                  /* Number of image elements (1-8) */
  U32   pixels_per_line;           /* Pixels per line (columns) */
  U32   lines_per_image_element;   /* Lines per image element (rows) */
  DPXImageElement element_info[8]; /* Description of elements */
  ASCII reserved[52];              /* Reserved for future use */
} DPXImageInfo;

typedef struct _DPXImageSourceBorderValidity
{
  /*
    Border validity indicates portion of border pixels which have been
    eroded due to processing.
  */
  U16   XL;                        /* Border validity XL border */
  U16   XR;                        /* Border validity XR border */
  U16   YT;                        /* Border validity YT border */
  U16   YB;                        /* Border validity YB border */
} DPXImageSourceBorderValidity;

typedef struct _DPXImageSourcePixelAspectRatio
{
  U32   horizontal;                /* Horizontal */
  U32   vertical;                  /* Vertical */
} DPXImageSourcePixelAspectRatio;

typedef struct _DPXImageSourceInfo
{
  U32   x_offset;                  /* X offset */
  U32   y_offset;                  /* Y offset */
  R32   x_center;                  /* X center */
  R32   y_center;                  /* Y center */
  U32   x_original_size;           /* X original size */
  U32   y_original_size;           /* Y original size */
  ASCII source_image_filename[100];/* Source image filename */
  ASCII source_image_datetime[24]; /* Source image date/time: yyyy:mmm:dd:hh:mm:ssLTZ */
  ASCII input_device_name[32];     /* Input device name */
  ASCII input_device_serialnumber[32]; /* Input device serial number */
  DPXImageSourceBorderValidity border_validity; /* Border validity */
  DPXImageSourcePixelAspectRatio aspect_ratio; /* Aspect ratio */
  R32   x_scanned_size;            /* X scanned size */
  R32   y_scanned_size;            /* Y scanned size */
  ASCII reserved[20];              /* Reserved for future use */
} DPXImageSourceInfo;

typedef struct _DPXMPFilmInfo
{
  ASCII film_mfg_id_code[2];       /* Film mfg. ID code (2 digits from film edge code) */
  ASCII film_type[2];              /* Film type (2 digits from film edge code) */
  ASCII perfs_offset[2];           /* Offset in perfs (2 digits from film edge code) */
  ASCII prefix[6];                 /* Prefix (6 digits from film edge code) */
  ASCII count[4];                  /* Count (4 digits from film edge code) */
  ASCII format[32];                /* Format -- e.g. Academy */
  U32   frame_position;            /* Frame position in sequence */
  U32   sequence_length;           /* Sequence length (frames) */
  U32   held_count;                /* Held count (1 = default) */
  R32   frame_rate;                /* Frame rate of original (frames/s) */
  R32   shutter_angle;             /* Shutter angle of camera in degrees */
  ASCII frame_id[32];              /* Frame identification - e.g. keyframe */
  ASCII slate_info[100];           /* Slate information */
  ASCII reserved[56];              /* Reserved for future use */
} DPXMPFilmInfo;

typedef struct _DPXTVInfo
{
  U32   time_code;                 /* SMPTE time code */
  U32   user_bits;                 /* SMPTE user bits */
  U8    interlace;                 /* Interlace (0 = noninterlaced; 1 = 2:1 interlace */
  U8    field_number;              /* Field number */
  U8    video_signal;              /* Video signal standard */
  U8    zero;                      /* Zero (for byte alignment) */
  R32   horizontal_sample;         /* Horizontal sampling rate */
  R32   vertical_sample;           /* Vertical sampling rate */
  R32   temporal_sample;           /* Temporal sampling rate or frame rate (Hz) */
  R32   sync_time;                 /* Time offset from sync to first pixel (ms) */
  R32   gamma;                     /* Gamma (applied above breakpoint) */
  R32   black_level;               /* Black level code value */
  R32   black_gain;                /* Black gain (linear gain applied below breakpoint) */
  R32   breakpoint;                /* Breakpoint (point above which gamma is applied) */
  R32   white_level;               /* Reference white level code value */
  R32   integration_time;          /* Integration time (s) */
  ASCII reserved[76];              /* Reserved for future use */
} DPXTVInfo;

typedef struct _DPXUserDefinedData
{
  ASCII user_id[32];               /* User identification */
  /* Up to 1MB of user-defined data after this point */
} DPXUserDefinedData;

typedef struct _DPXHeader
{
  DPXFileInfo file_info;           /* File information header */
  DPXImageInfo image_info;         /* Image information header */
  DPXImageSourceInfo source_info; /* Image source information header */
  DPXMPFilmInfo mp_info;           /* Motion picture film information header */
  DPXTVInfo tv_info;               /* Television information header */
} DPXHeader;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s D P X                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsDPX returns True if the image format type, identified by the
%  magick string, is DPX.
%
%  The format of the IsDPX method is:
%
%      unsigned int IsDPX(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsDPX returns True if the image format type is DPX.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
STATIC unsigned int IsDPX(const unsigned char *magick,const size_t length)
{
  return ((length >= 4) &&
          ((memcmp(magick,"SDPX",4) == 0) || (memcmp(magick,"XPDS",4) == 0)));
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d D P X I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadDPXImage reads an DPX X image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadDPXImage method is:
%
%      Image *ReadDPXImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadDPXImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
#define LogSetImageAttribute(name,value) \
{ \
  (void) LogMagickEvent(CoderEvent,GetMagickModule(), \
                        "Attribute \"%s\" set to \"%s\"", \
                        name,value); \
}

#define StringToAttribute(image,name,member) \
{ \
  char \
    buffer_[MaxTextExtent]; \
\
  if (!IS_UNDEFINED_ASCII(member)) \
    { \
      (void) strlcpy(buffer_,member,Min(sizeof(member)+1,MaxTextExtent)); \
      (void) SetImageAttribute(image,name,buffer_); \
      LogSetImageAttribute(name,buffer_); \
    } \
}
#define U8ToAttribute(image,name,member) \
{ \
  char \
    buffer_[MaxTextExtent]; \
\
  if (!IS_UNDEFINED_U8(member)) \
    { \
      FormatString(buffer_,"%u",(unsigned int) member); \
      (void) SetImageAttribute(image,name,buffer_); \
      LogSetImageAttribute(name,buffer_); \
    } \
}
#define U16ToAttribute(image,name,member) \
{ \
  char \
    buffer_[MaxTextExtent]; \
\
  if (!IS_UNDEFINED_U16(member)) \
    { \
      FormatString(buffer_,"%u",(unsigned int) member); \
      (void) SetImageAttribute(image,name,buffer_); \
      LogSetImageAttribute(name,buffer_); \
    } \
}
#define U32ToAttribute(image,name,member) \
{ \
  char \
    buffer_[MaxTextExtent]; \
\
  if (!IS_UNDEFINED_U32(member)) \
    { \
      FormatString(buffer_,"%u",member); \
      (void) SetImageAttribute(image,name,buffer_); \
      LogSetImageAttribute(name,buffer_); \
    } \
}
#define U32ToBitsAttribute(image,name,member) \
{ \
  char \
    buffer_[MaxTextExtent]; \
\
  if (!IS_UNDEFINED_U32(member)) \
    { \
      SMPTEBitsToString(member,buffer_); \
      (void) SetImageAttribute(image,name,buffer_); \
      LogSetImageAttribute(name,buffer_); \
    } \
}
#define R32ToAttribute(image,name,member) \
{ \
  char \
    buffer_[MaxTextExtent]; \
\
  if (!IS_UNDEFINED_R32(member)) \
    { \
      FormatString(buffer_,"%g",member); \
      (void) SetImageAttribute(image,name,buffer_); \
      LogSetImageAttribute(name,buffer_); \
    } \
}
STATIC void SwabDPXFileInfo(DPXFileInfo *file_info)
{
  MagickSwabUInt32(&file_info->magic);
  MagickSwabUInt32(&file_info->image_data_offset);
  MagickSwabUInt32(&file_info->file_size);
  MagickSwabUInt32(&file_info->ditto_key);
  MagickSwabUInt32(&file_info->generic_section_length);
  MagickSwabUInt32(&file_info->industry_section_length);
  MagickSwabUInt32(&file_info->user_defined_length);
  MagickSwabUInt32(&file_info->encryption_key);
}
STATIC void SwabDPXImageInfo(DPXImageInfo *image_info)
{
  int
    i;

  MagickSwabUInt16(&image_info->orientation);
  MagickSwabUInt16(&image_info->elements);
  MagickSwabUInt32(&image_info->pixels_per_line);
  MagickSwabUInt32(&image_info->lines_per_image_element);
  for (i=0 ; i < 8 ; i++)
    {
      MagickSwabUInt32(&image_info->element_info[i].data_sign);
      MagickSwabUInt32(&image_info->element_info[i].reference_low_data_code);
      MagickSwabFloat(&image_info->element_info[i].reference_low_quantity);
      MagickSwabUInt32(&image_info->element_info[i].reference_high_data_code);
      MagickSwabFloat(&image_info->element_info[i].reference_high_quantity);
      MagickSwabUInt16(&image_info->element_info[i].packing);
      MagickSwabUInt16(&image_info->element_info[i].encoding);
      MagickSwabUInt32(&image_info->element_info[i].data_offset);
      MagickSwabUInt32(&image_info->element_info[i].eol_pad);
      MagickSwabUInt32(&image_info->element_info[i].eoi_pad);
    }
}
STATIC void SwabDPXImageSourceInfo(DPXImageSourceInfo *source_info)
{
  MagickSwabUInt32(&source_info->x_offset);
  MagickSwabUInt32(&source_info->y_offset);
  MagickSwabFloat(&source_info->x_center);
  MagickSwabFloat(&source_info->y_center);
  MagickSwabUInt32(&source_info->x_original_size);
  MagickSwabUInt32(&source_info->y_original_size);
  MagickSwabUInt16(&source_info->border_validity.XL);
  MagickSwabUInt16(&source_info->border_validity.XR);
  MagickSwabUInt16(&source_info->border_validity.YT);
  MagickSwabUInt16(&source_info->border_validity.YB);
  MagickSwabUInt32(&source_info->aspect_ratio.horizontal);
  MagickSwabUInt32(&source_info->aspect_ratio.vertical);
  MagickSwabFloat(&source_info->x_scanned_size);
  MagickSwabFloat(&source_info->y_scanned_size);
}
STATIC void SwabDPXMPFilmInfo(DPXMPFilmInfo *mp_info)
{
  MagickSwabUInt32(&mp_info->frame_position);
  MagickSwabUInt32(&mp_info->sequence_length);
  MagickSwabUInt32(&mp_info->held_count);
  MagickSwabFloat(&mp_info->frame_rate);
  MagickSwabFloat(&mp_info->shutter_angle);
}
STATIC void SwabDPXTVInfo(DPXTVInfo *tv_info)
{
  MagickSwabUInt32(&tv_info->time_code);
  MagickSwabUInt32(&tv_info->user_bits);
  MagickSwabFloat(&tv_info->horizontal_sample);
  MagickSwabFloat(&tv_info->vertical_sample);
  MagickSwabFloat(&tv_info->temporal_sample);
  MagickSwabFloat(&tv_info->sync_time);
  MagickSwabFloat(&tv_info->gamma);
  MagickSwabFloat(&tv_info->black_level);
  MagickSwabFloat(&tv_info->black_gain);
  MagickSwabFloat(&tv_info->breakpoint);
  MagickSwabFloat(&tv_info->white_level);
  MagickSwabFloat(&tv_info->integration_time);
}
STATIC void SMPTEBitsToString(const U32 value, char *str)
{
  unsigned int
    pos,
    shift = 28;

  for (pos=8; pos != 0; pos--, shift -= 4)
    {
      (void) sprintf(str,"%01u",(unsigned int) ((value >> shift) & 0x0fU));
      str += 1;
      if ((pos > 2) && (pos % 2))
        {
          (void) strcat(str,":");
          str++;
        }
    }
  *str='\0';
}
STATIC U32 SMPTEStringToBits(const char *str)
{
  U32
    value=0;

  unsigned int
    pos = 0,
    shift = 28;

  char
    buff[2];

  buff[1]='\0';

  while ((*str != 0) && (pos < 8))
    {
      if (!isdigit((int) *str))
        {
          str++;
          continue;
        }
      buff[0]=*str++;
      value |= (U32) ((strtol(buff,(char **)NULL,10)) << shift);
      shift -= 4;
      pos++;
    }
  return value;
}
/*
  Compute the number of octets required to contain the specified number of
  rows, with specified samples per row, bits per sample, and packing method.
*/
STATIC size_t DPXRowOctets(const unsigned long rows,
                           const unsigned int samples_per_row,
                           const unsigned int bits_per_sample,
                           const ImageComponentPackingMethod packing_method)
{
  size_t
    octets = 0;

  switch(bits_per_sample)
    {
    case 1:
      /* Packed 1-bit samples in 32-bit words. Rows are padded out to 32-bit alignment */
      octets=rows*(((magick_int64_t) samples_per_row*bits_per_sample+31)/32)*sizeof(U32);
      break;
    case 8:
      /* C.1 8-bit samples in a 32-bit word. Rows are padded out to 32-bit alignment */
      octets=rows*(((magick_int64_t) samples_per_row*bits_per_sample+31)/32)*sizeof(U32);
      break;
    case 32:
      /* 32-bit samples in a 32-bit word */
      octets=samples_per_row*sizeof(U32)*rows;
      break;
    case 10:
      if ((packing_method == PackingMethodWordsFillLSB) ||
          (packing_method == PackingMethodWordsFillMSB))
        {
          /* C.3 Three 10-bit samples per 32-bit word */
          octets=(((((magick_int64_t) (rows*samples_per_row+2)/3)*sizeof(U32)*8)+31)/32)*sizeof(U32);
        }
      else
        {
          /* C.2 Packed 10-bit samples in a 32-bit word. */
          octets=rows*(((magick_int64_t) samples_per_row*bits_per_sample+31)/32)*sizeof(U32);
        }
      break;
    case 12:
      if ((packing_method == PackingMethodWordsFillLSB) ||
          (packing_method == PackingMethodWordsFillMSB))
        {
          /* C.5: One 12-bit sample per 16-bit word */
          octets=((((magick_int64_t) rows*samples_per_row*sizeof(U16)*8)+15)/16)*sizeof(U16);
        }
      else
        {
          /* C.4: Packed 12-bit samples in a 32-bit word. */
          octets=rows*(((magick_int64_t) samples_per_row*bits_per_sample+31)/32)*sizeof(U32);
        }
      break;
    case 16:
      /* C.6 16-bit samples in 16-bit words. */
      octets=((((magick_int64_t) rows*samples_per_row*bits_per_sample)+15)/16)*sizeof(U16);
      break;
    case 64:
      /* 64-bit samples in 64-bit words. */
      octets=(magick_int64_t) rows*samples_per_row*8;
      break;
    }

  return octets;
}
/*
  Compute optimum I/O parameters based on all considerations.
*/
#if 0
STATIC size_t DPXIOOctets(const long current_row, /* 0 based */
                          const unsigned long image_rows,
                          const unsigned int samples_per_row,
                          const unsigned int bits_per_sample,
                          const ImageComponentPackingMethod packing_method)
{
  long
    rows_remaining;

  rows_remaining=image_rows-current_row;



}
#endif
STATIC const char *DescribeImageElementDescriptor(const DPXImageElementDescriptor descriptor)
{
  const char *
    description="Unknown";

  static char
    buffer[MaxTextExtent];

  switch(descriptor)
    {
    case ImageElementUnspecified:
      description="Generic 1 Element";
      break;
    case ImageElementRed:
      description="Red";
      break;
    case ImageElementGreen:
      description="Green";
      break;
    case ImageElementBlue:
      description="Blue";
      break;
    case ImageElementAlpha:
      description="Alpha";
      break;
    case ImageElementLuma:
      description="Luma";
      break;
    case ImageElementColorDifferenceCbCr:
      description="CbCr";
      break;
    case ImageElementDepth:
      description="Depth(8)";
      break;
    case ImageElementCompositeVideo:
      description="CompositeVideo";
      break;
    case ImageElementRGB:
      description="RGB";
      break;
    case ImageElementRGBA:
      description="RGBA";
      break;
    case ImageElementABGR:
      description="ABGR";
      break;
    case ImageElementCbYCrY422:
      description="CbYCrY 4:2:2";
      break;
    case ImageElementCbYACrYA4224:
      description="CbYACrYA 4:2:2:4";
      break;
    case ImageElementCbYCr444:
      description="CbYCr 4:4:4";
      break;
    case ImageElementCbYCrA4444:
      description="CbYCrA 4:4:4:4";
      break;
    case ImageElementUserDef2Element:
      description="Generic 2 Element";
      break;
    case ImageElementUserDef3Element:
      description="Generic 3 Element";
      break;
    case ImageElementUserDef4Element:
      description="Generic 4 Element";
      break;
    case ImageElementUserDef5Element:
      description="Generic 5 Element";
      break;
    case ImageElementUserDef6Element:
      description="Generic 6 Element";
      break;
    case ImageElementUserDef7Element:
      description="Generic 7 Element";
      break;
    case ImageElementUserDef8Element:
      description="Generic 8 Element";
      break;
    default:
      {
        FormatString(buffer,"Unknown (%u)",(unsigned int) descriptor);
        description=buffer;
      }
    }

  return description;
}
/*
  Describe the element transfer characteristic.
*/
STATIC const char *DescribeImageTransferCharacteristic(const DPXTransferCharacteristic characteristic)
{
  static char
    buffer[MaxTextExtent];

  const char
    *description=buffer;

  buffer[0]='\0';
  switch(characteristic)
    {
    case TransferCharacteristicUserDefined:
      description="UserDefined";
      break;
    case TransferCharacteristicPrintingDensity:
      description="PrintingDensity";
      break;
    case TransferCharacteristicLinear:
      description="Linear";
      break;
    case TransferCharacteristicLogarithmic:
      description="Logarithmic";
      break;
    case TransferCharacteristicUnspecifiedVideo:
      description="UnspecifiedVideo";
      break;
    case TransferCharacteristicSMTPE274M:
      description="SMTPE274M";
      break;
    case TransferCharacteristicITU_R709:
      description="ITU-R709";
      break;
    case TransferCharacteristicITU_R601_625L:
      description="ITU-R601-625L";
      break;
    case TransferCharacteristicITU_R601_525L:
      description="ITU-R601-525L";
      break;
    case TransferCharacteristicNTSCCompositeVideo:
      description="NTSCCompositeVideo";
      break;
    case TransferCharacteristicPALCompositeVideo:
      description="PALCompositeVideo";
      break;
    case TransferCharacteristicZDepthLinear:
      description="ZDepthLinear";
      break;
    case TransferCharacteristicZDepthHomogeneous:
      description="ZDepthHomogeneous";
      break;
    default:
      {
        FormatString(buffer,"Reserved(%u)",(unsigned int) characteristic);
      }
    }

  return description;
}
/*
  Describe the element colorimetric.
*/
STATIC const char *DescribeImageColorimetric(const DPXColorimetric colorimetric)
{
  static char
    buffer[MaxTextExtent];

  const char
    *description=buffer;

  buffer[0]='\0';
  switch(colorimetric)
    {
    case ColorimetricUserDefined:
      description="UserDefined";
      break;
    case ColorimetricPrintingDensity:
      description="PrintingDensity";
      break;
    case ColorimetricLinear:
      description="NotApplicable";
      break;
    case ColorimetricLogarithmic:
      description="NotApplicable";
      break;
    case ColorimetricUnspecifiedVideo:
      description="UnspecifiedVideo";
      break;
    case ColorimetricSMTPE274M:
      description="SMTPE274M";
      break;
    case ColorimetricITU_R709:
      description="ITU-R709";
      break;
    case ColorimetricITU_R601_625L:
      description="ITU-R601-625L";
      break;
    case ColorimetricITU_R601_525L:
      description="ITU-R601-525L";
      break;
    case ColorimetricNTSCCompositeVideo:
      description="NTSCCompositeVideo";
      break;
    case ColorimetricPALCompositeVideo:
      description="PALCompositeVideo";
      break;
    case ColorimetricZDepthLinear:
      description="NotApplicable";
      break;
    case ColorimetricZDepthHomogeneous:
      description="NotApplicable";
      break;
    default:
      {
        FormatString(buffer,"Reserved(%u)",(unsigned int) colorimetric);
      }
    }

  return description;
}
/*
  Describe the image element.
*/
STATIC void DescribeDPXImageElement(const DPXImageElement *element_info,
                                    const unsigned int element)
{
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "Element %u: data_sign=%s",element, 
                        element_info->data_sign == 0 ?
                        "unsigned(0)" : "signed(1)");
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "Element %u: reference_low_data_code=%u reference_low_quantity=%g",
                        element,
                        element_info->reference_low_data_code,
                        element_info->reference_low_quantity);
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "Element %u: reference_high_data_code=%u reference_high_quantity=%g",
                        element,
                        element_info->reference_high_data_code,
                        element_info->reference_high_quantity);
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "Element %u: descriptor=%s(%u) transfer_characteristic=%s(%u) colorimetric=%s(%u)",
                        element,
                        DescribeImageElementDescriptor((DPXImageElementDescriptor) element_info->descriptor),
                        (unsigned int) element_info->descriptor,
                        DescribeImageTransferCharacteristic((DPXTransferCharacteristic) element_info->transfer_characteristic),
                        (unsigned int) element_info->transfer_characteristic,
                        DescribeImageColorimetric((DPXColorimetric) element_info->colorimetric),
                        (unsigned int) element_info->colorimetric);
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "Element %u: bits-per-sample=%u",
                        element,
                        (unsigned int) element_info->bits_per_sample);
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "Element %u: packing=%s encoding=%s data_offset=%u eol_pad=%u eoi_pad=%u",
                        element,
                        (element_info->packing == 0 ? "Packed(0)" :
                         element_info->packing == 1 ? "PadLSB(1)" :
                         element_info->packing == 2 ? "PadMSB(2)" :
                         "Unknown"),
                        (element_info->encoding == 0 ? "None(0)" :
                         element_info->encoding == 1 ? "RLE(1)" :
                         "Unknown"),
                        (unsigned int) element_info->data_offset,
                        (unsigned int) element_info->eol_pad,
                        (unsigned int) element_info->eoi_pad);
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "Element %u: description=\"%.32s\"",
                        element,
                        element_info->description);
}
/*
  Obtain number of element samples required to support one pixel.  For
  example, RGB requires three samples, but if the image organization
  is planar three elements are required to support RGB, and this
  function will therefore return 1 rather than 3.
*/
STATIC unsigned int  DPXSamplesPerPixel(const DPXImageElementDescriptor element_descriptor)
{
  unsigned int
    samples_per_pixel=0;
  
  switch (element_descriptor)
    {
    case ImageElementUnspecified:
    case ImageElementRed:
    case ImageElementGreen:
    case ImageElementBlue:
    case ImageElementAlpha:
    case ImageElementLuma:
    case ImageElementColorDifferenceCbCr:
      samples_per_pixel=1;
      break;
    case ImageElementRGB:
      samples_per_pixel=3;
      break;
    case ImageElementRGBA:
    case ImageElementABGR:
      samples_per_pixel=4;
      break;
    case ImageElementCbYCrY422:
      /* CbY | CrY | CbY | CrY ..., even number of columns required. */
      samples_per_pixel=2;
      break;
    case ImageElementCbYACrYA4224:
      /* CbYA | CrYA | CbYA | CrYA ..., even number of columns required. */
      samples_per_pixel=3;
      break;
    case ImageElementCbYCr444:
      samples_per_pixel=3;
      break;
    case ImageElementCbYCrA4444:
      samples_per_pixel=4;
      break;
    default:
      samples_per_pixel=0;
      break;
    }

  return samples_per_pixel;
}
/*
  Set the image primary chromaticities based on the colorimetric.
*/
STATIC void DPXSetPrimaryChromaticities(const DPXColorimetric colorimetric,
                                        ChromaticityInfo *chromaticity_info)
{
  switch(colorimetric)
    {
    case ColorimetricSMTPE274M:
    case ColorimetricITU_R709:/* ITU R709 */
      /* ITU-R BT.709-5, D65 */
      chromaticity_info->red_primary.x=0.640;
      chromaticity_info->red_primary.y=0.330;
      chromaticity_info->red_primary.z=0.030;
      chromaticity_info->green_primary.x=0.300;
      chromaticity_info->green_primary.y=0.600;
      chromaticity_info->green_primary.z=0.100;
      chromaticity_info->blue_primary.x=0.150;
      chromaticity_info->blue_primary.y=0.060;
      chromaticity_info->blue_primary.z=0.790;
      chromaticity_info->white_point.x=0.3127;
      chromaticity_info->white_point.y=0.3290;
      chromaticity_info->white_point.z=0.3582;
      break;

    case ColorimetricNTSCCompositeVideo:
      /* Obsolete NTSC primaries, White CIE III. C */
      chromaticity_info->red_primary.x=0.67;
      chromaticity_info->red_primary.y=0.33;
      chromaticity_info->red_primary.z=0.00;
      chromaticity_info->green_primary.x=0.21;
      chromaticity_info->green_primary.y=0.71;
      chromaticity_info->green_primary.z=0.08;
      chromaticity_info->blue_primary.x=0.14;
      chromaticity_info->blue_primary.y=0.08;
      chromaticity_info->blue_primary.z=0.78;
      chromaticity_info->white_point.x=0.310;
      chromaticity_info->white_point.y=0.316;
      chromaticity_info->white_point.z=0.374;
      break;

    case ColorimetricPALCompositeVideo:
      /* EBU Tech. 3213 primaries, D65 */
      chromaticity_info->red_primary.x=0.640;
      chromaticity_info->red_primary.y=0.330;
      chromaticity_info->red_primary.z=0.030;
      chromaticity_info->green_primary.x=0.290;
      chromaticity_info->green_primary.y=0.600;
      chromaticity_info->green_primary.z=0.110;
      chromaticity_info->blue_primary.x=0.150;
      chromaticity_info->blue_primary.y=0.060;
      chromaticity_info->blue_primary.z=0.790;
      chromaticity_info->white_point.x=0.3127;
      chromaticity_info->white_point.y=0.3290;
      chromaticity_info->white_point.z=0.3582;
      break;

#if 0
      /* SMPTE RP 145 / SMPTE 240M primaries (as used for 480i SDTV), D65 */
      chromaticity_info->red_primary.x=0.630;
      chromaticity_info->red_primary.y=0.340;
      chromaticity_info->red_primary.z=0.030;
      chromaticity_info->green_primary.x=0.310;
      chromaticity_info->green_primary.y=0.595;
      chromaticity_info->green_primary.z=0.095;
      chromaticity_info->blue_primary.x=0.155;
      chromaticity_info->blue_primary.y=0.070;
      chromaticity_info->blue_primary.z=0.775;
      chromaticity_info->white_point.x=0.3127;
      chromaticity_info->white_point.y=0.3290;
      chromaticity_info->white_point.z=0.3582;
#endif


    case ColorimetricITU_R601_625L: /* 625 Line ITU R601-5 B & G */
    case ColorimetricITU_R601_525L: /* 525 Line ITU R601-5 M */

    case ColorimetricUserDefined: /* User defined */
    case ColorimetricPrintingDensity: /* Printing density */
    case ColorimetricLinear: /* Linear */
    case ColorimetricLogarithmic: /* Logarithmic */
    case ColorimetricUnspecifiedVideo:
    default:
      {
        break;
      }
    }
}

STATIC OrientationType
DPXOrientationToOrientationType(const unsigned int orientation)
{
  OrientationType
    orientation_type = UndefinedOrientation;

  switch (orientation)
    {
    case 0:
      orientation_type=TopLeftOrientation;
      break;
    case 1:
      orientation_type=TopRightOrientation;
      break;
    case 2:
      orientation_type=BottomLeftOrientation;
      break;
    case 3:
      orientation_type=BottomRightOrientation;
      break;
    case 4:
      orientation_type=LeftTopOrientation;
      break;
    case 5:
      orientation_type=RightTopOrientation;
      break;
    case 6:
      orientation_type=LeftBottomOrientation;
      break;
    case 7:
      orientation_type=RightBottomOrientation;
      break;
    }

  return orientation_type;
}

#define LSBOctetsToPackedU32Word(scanline,packed_u32) \
{ \
  packed_u32  = (((magick_uint32_t) *scanline++)); \
  packed_u32 |= (((magick_uint32_t) *scanline++) << 8); \
  packed_u32 |= (((magick_uint32_t) *scanline++) << 16); \
  packed_u32 |= (((magick_uint32_t) *scanline++) << 24); \
}
#define MSBOctetsToPackedU32Word(scanline,packed_u32) \
{ \
  packed_u32  = (((magick_uint32_t) *scanline++) << 24); \
  packed_u32 |= (((magick_uint32_t) *scanline++) << 16); \
  packed_u32 |= (((magick_uint32_t) *scanline++) << 8); \
  packed_u32 |= (((magick_uint32_t) *scanline++)); \
}

/*
  Scale from a video level to a full-range level.
*/
STATIC inline Quantum ScaleFromVideo(const unsigned int sample,
                                     const unsigned int ref_low,
                                     const double upscale)
{
  double
    result = 0.0;

  if (sample > ref_low)
    result = (sample - ref_low)*upscale;
  return RoundDoubleToQuantum(result);
  
}

/*
  WordStreamLSBRead support
*/
typedef struct _ReadWordU32State
{
  const unsigned char *words;
} ReadWordU32State;

STATIC unsigned long ReadWordU32BE (void *state)
{
  magick_uint32_t value;
  ReadWordU32State *read_state=(ReadWordU32State *) state;
  value =  *read_state->words++ << 24;
  value |= *read_state->words++ << 16;
  value |= *read_state->words++ << 8;
  value |= *read_state->words++;
  return value;
}

STATIC unsigned long ReadWordU32LE (void *state)
{
  magick_uint32_t value;
  ReadWordU32State *read_state=(ReadWordU32State *) state;
  value = *read_state->words++;
  value |= *read_state->words++ << 8;
  value |= *read_state->words++ << 16;
  value |=  *read_state->words++ << 24;
  return value;
}

/*
  Decode row samples. Currently just one row but in the future may be
                      multiple rows (e.g. 3).

  scanline         -- Raw input data (may be 8-bit, 16-bit, 32-bit, or 64-bit types)
                      which represents the encoded pixels for one or more scanlines.
                      Underlying input data type is properly aligned for access.
  samples_per_row  -- Number of samples to decode.
  bits_per_sample  -- Number of bits in one decoded sample.
  packing_method   -- Describes the way that samples are packed into enclosing words.
  endian_type      -- The endian order of the enclosing words.
  swap_word_datums -- Use alternate sample order (BGR vs RGB, CbYCr vs CrYCb) for
                      samples filled into 32 bit words.
  samples          -- Decoded samples (currently unsigned 16-bit).
*/
STATIC void ReadRowSamples(const unsigned char *scanline,
                           const unsigned int samples_per_row,
                           const unsigned int bits_per_sample,
                           const ImageComponentPackingMethod packing_method,
                           const EndianType endian_type,
                           const MagickBool swap_word_datums,
                           sample_t *samples)
{
  register unsigned long
    i;

  sample_t
    *sp;

  register unsigned int
    sample;

  sp=samples;
  if ((packing_method != PackingMethodPacked) &&
      ((bits_per_sample == 10) || (bits_per_sample == 12)))
    {
      MagickBool
        word_pad_lsb=MagickFalse,
        word_pad_msb=MagickFalse;

      if (packing_method == PackingMethodWordsFillLSB)
        word_pad_lsb=MagickTrue;
      else if (packing_method == PackingMethodWordsFillMSB)
        word_pad_msb=MagickTrue;

      if (bits_per_sample == 10)
        {
          register magick_uint32_t
            packed_u32;
          
          register unsigned int
            datum;
          
          unsigned int
            shifts[3] = { 0, 0, 0 };

          if (word_pad_lsb)
            {
              /*
                Padding in LSB (Method A)  Standard method.
              */
              if (swap_word_datums == MagickFalse)
                {
                  shifts[0]=2;  /* datum-0 / blue */
                  shifts[1]=12; /* datum-1 / green */
                  shifts[2]=22; /* datum-2 / red */
                }
              else
                {
                  shifts[0]=22; /* datum-2 / red */
                  shifts[1]=12; /* datum-1 / green */
                  shifts[2]=2;  /* datum-0 / blue */
                }
            }
          else if (word_pad_msb)
            {
              /*
                Padding in MSB (Method B)  Deprecated method.
              */
              if (swap_word_datums == MagickFalse)
                {
                  shifts[0]=0;  /* datum-0 / blue */
                  shifts[1]=10; /* datum-1 / green */
                  shifts[2]=20; /* datum-2 / red */
                }
              else
                {
                  shifts[0]=20; /* datum-2 / red */
                  shifts[1]=10; /* datum-1 / green */
                  shifts[2]=0;  /* datum-0 / blue */
                }
            }

          if (endian_type == MSBEndian)
            {
              for (i=samples_per_row/3; i != 0; --i)
                {
                  datum=0;
                  MSBOctetsToPackedU32Word(scanline,packed_u32);
                  *sp++=(packed_u32 >> shifts[datum++]) & 0x3FF;
                  *sp++=(packed_u32 >> shifts[datum++]) & 0x3FF;
                  *sp++=(packed_u32 >> shifts[datum]) & 0x3FF;
                }
              if ((samples_per_row % 3))
                {
                  datum=0;
                  MSBOctetsToPackedU32Word(scanline,packed_u32);
                  for (i=(samples_per_row % 3); i != 0; --i)
                    *sp++=(packed_u32 >> shifts[datum++]) & 0x3FF;
                }
            }
          else if (endian_type == LSBEndian)
            {
              for (i=samples_per_row/3; i != 0; --i)
                {
                  datum=0;
                  LSBOctetsToPackedU32Word(scanline,packed_u32);
                  *sp++=(packed_u32 >> shifts[datum++]) & 0x3FF;
                  *sp++=(packed_u32 >> shifts[datum++]) & 0x3FF;
                  *sp++=(packed_u32 >> shifts[datum]) & 0x3FF;
                }
              if ((samples_per_row % 3))
                {
                  datum=0;
                  LSBOctetsToPackedU32Word(scanline,packed_u32);
                  for (i=(samples_per_row % 3); i != 0; --i)
                    *sp++=(packed_u32 >> shifts[datum++]) & 0x3FF;
                }
            }
          return;
        }
      else if (bits_per_sample == 12)
        {
          if (word_pad_lsb)
            {
              /*
                Padding in LSB (Method A)  Standard method.
              */
              if (endian_type == MSBEndian)
                {
                  for (i=samples_per_row; i != 0; i--)
                    {
                      sample=0;
                      sample |= (*scanline++ << 8);
                      sample |= (*scanline++);
                      sample >>= 4;
                      *sp++=sample;
                    }
                }
              else if (endian_type == LSBEndian)
                {
                  for (i=samples_per_row; i != 0; i--)
                    {
                      sample=0;
                      sample |= (*scanline++);
                      sample |= (*scanline++ << 8);
                      sample >>= 4;
                      *sp++=sample;
                    }
                }
              return;
            }
          else if (word_pad_msb)
            {
              /*
                Padding in MSB (Method B)  Deprecated method.
              */
              if (endian_type == MSBEndian)
                {
                  for (i=samples_per_row; i != 0; i--)
                    {
                      sample=0;
                      sample |= (*scanline++ << 8);
                      sample |= (*scanline++);
                      sample &= 0xFFF;
                      *sp++=sample;
                    }
                }
              else if (endian_type == LSBEndian)
                {
                  for (i=samples_per_row; i != 0; i--)
                    {
                      sample=0;
                      sample |= (*scanline++);
                      sample |= (*scanline++ << 8);
                      sample &= 0xFFF;
                      *sp++=sample;
                    }
                }
              return;
            }
        }
    }

  /*
    Special fast handling for 8-bit images.
  */
  if (bits_per_sample == 8)
    {
      for (i=samples_per_row; i != 0; i--)
        *sp++= (sample_t) *scanline++;
      return;
    }

  /*
    Special fast handling for 16-bit images.
  */
  if (bits_per_sample == 16)
    {
      if (endian_type == MSBEndian)
        {
          for (i=samples_per_row; i != 0; i--)
            {
              sample=0;
              sample |= (*scanline++ << 8);
              sample |= (*scanline++);
              *sp++=sample;
            }
        }
      else if (endian_type == LSBEndian)
        {
          for (i=samples_per_row; i != 0; i--)
            {
              sample=0;
              sample |= (*scanline++);
              sample |= (*scanline++ << 8);
              *sp++=sample;
            }
        }
      return;
    }

#if 0
  /*
    Special fast handling for 32-bit (float) images.
  */
  if (bits_per_sample == 32)
    {
      register magick_uint32_t
        packed_u32;

      if (endian_type == MSBEndian)
        {
          for (i=samples_per_row; i != 0; i--)
            {
              MSBOctetsToPackedU32Word(scanline,packed_u32);
              *sp++=packed_u32;
            }
        }
      else if (endian_type == LSBEndian)
        {
          for (i=samples_per_row; i != 0; i--)
            {
              LSBOctetsToPackedU32Word(scanline,packed_u32);
              *sp++=packed_u32;
            }
        }
      return;
    }
#endif

  /*
    Packed data.
  */
  {
    ReadWordU32State
      read_state;
    
    WordStreamReadHandle
      read_stream;

    WordStreamReadFunc
      read_func=0;

    if (endian_type == MSBEndian)
      read_func=ReadWordU32BE;
    else if (endian_type == LSBEndian)
      read_func=ReadWordU32LE;

    read_state.words=scanline;
    WordStreamInitializeRead(&read_stream,read_func, (void *) &read_state);
      for (i=samples_per_row; i != 0; i--)
        *sp++=WordStreamLSBRead(&read_stream,bits_per_sample);
  }
}

/*
  Apply a simple "Tent" filter to upsample chroma channels.
*/
STATIC void TentUpsampleChroma(PixelPacket *pixels, unsigned long columns)
{
  unsigned long
    column;

  for (column = 1; column < columns-2; column += 2)
    {
#if QuantumDepth < 32
      /*
        Use integer computations if intermediate result will fit.
      */
      pixels->green=((unsigned long) pixels[column-1].green + pixels[column+1].green)/2;
      pixels->blue=((unsigned long) pixels[column-1].blue + pixels[column+1].blue)/2;
#else
      /*
        Use floating point computations.
      */
      double
        result;

      result=((double) pixels[column-1].green + pixels[column+1].green)/2;
      pixels->green=RoundDoubleToQuantum(result);

      result=((double) pixels[column-1].blue + pixels[column+1].blue)/2;
      pixels->blue=RoundDoubleToQuantum(result);
#endif
    }
}

STATIC Image *ReadDPXImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  DPXFileInfo
    dpx_file_info;

  DPXImageInfo
    dpx_image_info;

  DPXImageSourceInfo
    dpx_source_info;

  DPXMPFilmInfo
    dpx_mp_info;

  DPXTVInfo
    dpx_tv_info;

  Image
    *image=0;

  long
    y;

  size_t
    offset,
    row_octets;

  Quantum
    *map_Y,                     /* value translation map (RGB or Y) */
    *map_CbCr;                  /* value translation map (CbCr) */

  ThreadViewDataSet
    *samples_set;

  ThreadViewDataSet
    *scanline_set;

  size_t
    element_size;               /* Number of bytes in an element */

  unsigned int
    bits_per_sample,            /* number of bits per sample */
    element,                    /* current element number */
    max_bits_per_sample,        /* maximum number of bits per sample for any element */
    max_samples_per_pixel,      /* maximum number of samples comprising one pixel for any element */
    samples_per_pixel,          /* number of samples comprising one pixel for this element */
    samples_per_row;            /* number of samples in one row */

  MagickPassFail
    status;

  unsigned long
    i,
    pixels_offset;

  MagickBool
    is_grayscale=MagickFalse,   /* image is grayscale ? */
    is_monochrome=MagickFalse,  /* image is monochrome ? */
    swab=MagickFalse;           /* swap endian order */

  DPXImageElementDescriptor
    element_descriptor;

  DPXTransferCharacteristic
    transfer_characteristic;

  ImageComponentPackingMethod
    packing_method;

  EndianType
    endian_type;

  const char
    *definition_value;

  /*
    Open image file.
  */
  assert(sizeof(DPXHeader) == 2048);
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    ThrowReaderException(FileOpenError,UnableToOpenFile,image);
  /*
    Read DPX image.
  */
  offset=ReadBlob(image,sizeof(dpx_file_info),&dpx_file_info);
  if (offset != sizeof(dpx_file_info) ||
      ((LocaleNCompare((char *) &dpx_file_info.magic,"SDPX",4) != 0) &&
       (LocaleNCompare((char *) &dpx_file_info.magic,"XPDS",4) != 0)))
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
  /*
    Check for swapped endian order.
  */
  if (dpx_file_info.magic != 0x53445058U)
    swab=MagickTrue;

#if defined(WORDS_BIGENDIAN)
  endian_type = (swab ? LSBEndian : MSBEndian);
#else
  endian_type = (swab ? MSBEndian : LSBEndian);
#endif
  
  if (image->logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "%s endian DPX format",
                          (endian_type == MSBEndian ? "Big" : "Little"));

  if (swab)
    SwabDPXFileInfo(&dpx_file_info);

  if (image->logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "File size: %u", dpx_file_info.file_size);

  StringToAttribute(image,"software",dpx_file_info.creator);
  StringToAttribute(image,"comment",dpx_file_info.project_name);
  StringToAttribute(image,"copyright",dpx_file_info.copyright);
  StringToAttribute(image,"document",dpx_file_info.image_filename);
  /* StringToAttribute(image,"timestamp",dpx_file_info.creation_datetime); */

  StringToAttribute(image,"DPX:file.version",dpx_file_info.header_format_version);
  StringToAttribute(image,"DPX:file.filename",dpx_file_info.image_filename);
  StringToAttribute(image,"DPX:file.creation.datetime",dpx_file_info.creation_datetime);
  StringToAttribute(image,"DPX:file.creator",dpx_file_info.creator);
  StringToAttribute(image,"DPX:file.project.name",dpx_file_info.project_name);
  StringToAttribute(image,"DPX:file.copyright",dpx_file_info.copyright);
  U32ToAttribute(image,"DPX:file.encryption.key",dpx_file_info.encryption_key);

  /*
    Obtain offset to pixels.
  */
  pixels_offset=dpx_file_info.image_data_offset;
  if (image->logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "Image data offset %lu",pixels_offset);
  if (pixels_offset < 1408)
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
  if (image->logging)
    {
      char
        generic_length_str[MaxTextExtent],
        industry_length_str[MaxTextExtent],
        user_length_str[MaxTextExtent];

      if (IS_UNDEFINED_U32(dpx_file_info.generic_section_length))
        (void) strlcpy(generic_length_str,"UNDEFINED",sizeof(generic_length_str));
      else
        FormatString(generic_length_str,"%u",dpx_file_info.generic_section_length);

      if (IS_UNDEFINED_U32(dpx_file_info.industry_section_length))
        (void) strlcpy(industry_length_str,"UNDEFINED",sizeof(industry_length_str));
      else
        FormatString(industry_length_str,"%u",dpx_file_info.industry_section_length);

      if (IS_UNDEFINED_U32(dpx_file_info.user_defined_length))
        (void) strlcpy(user_length_str,"UNDEFINED",sizeof(user_length_str));
      else
        FormatString(user_length_str,"%u",dpx_file_info.user_defined_length);

      if (image->logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "Generic length %s, Industry length %s, User length %s",
                              generic_length_str,industry_length_str,user_length_str);
    }
  /*
    Read image information header.
  */
  offset += ReadBlob(image,sizeof(dpx_image_info),&dpx_image_info);
  if (offset != (size_t) 1408L)
    ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
  if (swab)
    SwabDPXImageInfo(&dpx_image_info);
  image->columns=dpx_image_info.pixels_per_line;
  image->rows=dpx_image_info.lines_per_image_element;
  if (image->logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "Columns %ld, Rows %ld, Elements %u",
                          image->columns, image->rows,
                          (unsigned int) dpx_image_info.elements);

  U16ToAttribute(image,"DPX:image.orientation",dpx_image_info.orientation);
  image->orientation=DPXOrientationToOrientationType(dpx_image_info.orientation);

  if (pixels_offset >= 1664UL)
    {
      /*
        Read Image source information header.
      */
      offset += ReadBlob(image,sizeof(dpx_source_info),&dpx_source_info);
      if (offset != (size_t) 1664L)
        ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
      if (swab)
        SwabDPXImageSourceInfo(&dpx_source_info);

      U32ToAttribute(image,"DPX:source.x-offset",dpx_source_info.x_offset);
      U32ToAttribute(image,"DPX:source.y-offset",dpx_source_info.y_offset);
      R32ToAttribute(image,"DPX:source.x-center",dpx_source_info.x_center);
      R32ToAttribute(image,"DPX:source.y-center",dpx_source_info.y_center);
      U32ToAttribute(image,"DPX:source.x-original-size",dpx_source_info.x_original_size);
      U32ToAttribute(image,"DPX:source.y-original-size",dpx_source_info.y_original_size);
      StringToAttribute(image,"DPX:source.filename",dpx_source_info.source_image_filename);
      StringToAttribute(image,"DPX:source.creation.datetime",dpx_source_info.source_image_datetime);
      StringToAttribute(image,"DPX:source.device.name",dpx_source_info.input_device_name);
      StringToAttribute(image,"DPX:source.device.serialnumber",dpx_source_info.input_device_serialnumber);
      U16ToAttribute(image,"DPX:source.border.validity.left",dpx_source_info.border_validity.XL);
      U16ToAttribute(image,"DPX:source.border.validity.right",dpx_source_info.border_validity.XR);
      U16ToAttribute(image,"DPX:source.border.validity.top",dpx_source_info.border_validity.YT);
      U16ToAttribute(image,"DPX:source.border.validity.bottom",dpx_source_info.border_validity.YB);
      U32ToAttribute(image,"DPX:source.aspect.ratio.horizontal",dpx_source_info.aspect_ratio.horizontal);
      U32ToAttribute(image,"DPX:source.aspect.ratio.vertical",dpx_source_info.aspect_ratio.vertical);
      R32ToAttribute(image,"DPX:source.scanned.size.x",dpx_source_info.x_scanned_size);
      R32ToAttribute(image,"DPX:source.scanned.size.y",dpx_source_info.y_scanned_size);
    }
  if (pixels_offset >= 1920UL)
    {
      /*
        Read Motion-picture film information header.
      */
      offset += ReadBlob(image,sizeof(dpx_mp_info),&dpx_mp_info);
      if (offset != (size_t) 1920L)
        ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
      if (swab)
        SwabDPXMPFilmInfo(&dpx_mp_info);

      if (dpx_file_info.industry_section_length != 0)
        {
          StringToAttribute(image,"DPX:mp.film.manufacturer.id",dpx_mp_info.film_mfg_id_code);
          StringToAttribute(image,"DPX:mp.film.type",dpx_mp_info.film_type);
          StringToAttribute(image,"DPX:mp.perfs.offset",dpx_mp_info.perfs_offset);
          StringToAttribute(image,"DPX:mp.prefix",dpx_mp_info.prefix);
          StringToAttribute(image,"DPX:mp.count",dpx_mp_info.count);
          StringToAttribute(image,"DPX:mp.format",dpx_mp_info.format);
          U32ToAttribute(image,"DPX:mp.frame.position",dpx_mp_info.frame_position);
          U32ToAttribute(image,"DPX:mp.sequence.length",dpx_mp_info.sequence_length);
          U32ToAttribute(image,"DPX:mp.held.count",dpx_mp_info.held_count);
          R32ToAttribute(image,"DPX:mp.frame.rate",dpx_mp_info.frame_rate);
          R32ToAttribute(image,"DPX:mp.shutter.angle",dpx_mp_info.shutter_angle);
          StringToAttribute(image,"DPX:mp.frame.id",dpx_mp_info.frame_id);
          StringToAttribute(image,"DPX:mp.slate.info",dpx_mp_info.slate_info);
        }
    }
  if (pixels_offset >= 2048UL)
    {
      /*
        Read Television information header.
      */
      offset += ReadBlob(image,sizeof(dpx_tv_info),&dpx_tv_info);
      if (offset != (size_t) 2048L)
        ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image); 
      if (swab)
        SwabDPXTVInfo(&dpx_tv_info);

      if (dpx_file_info.industry_section_length != 0)
        {
          U32ToBitsAttribute(image,"DPX:tv.time.code",dpx_tv_info.time_code);
          U32ToBitsAttribute(image,"DPX:tv.user.bits",dpx_tv_info.user_bits);
          U8ToAttribute(image,"DPX:tv.interlace",dpx_tv_info.interlace);
          U8ToAttribute(image,"DPX:tv.field.number",dpx_tv_info.field_number);
          U8ToAttribute(image,"DPX:tv.video.signal",dpx_tv_info.video_signal);
          R32ToAttribute(image,"DPX:tv.horizontal.sampling.rate",dpx_tv_info.horizontal_sample);
          R32ToAttribute(image,"DPX:tv.temporal.sampling.rate",dpx_tv_info.temporal_sample);
          R32ToAttribute(image,"DPX:tv.sync.time",dpx_tv_info.sync_time);
          R32ToAttribute(image,"DPX:tv.gamma",dpx_tv_info.gamma);
          R32ToAttribute(image,"DPX:tv.black.level",dpx_tv_info.black_level);
          R32ToAttribute(image,"DPX:tv.black.gain",dpx_tv_info.black_gain);
          R32ToAttribute(image,"DPX:tv.breakpoint",dpx_tv_info.breakpoint);
          R32ToAttribute(image,"DPX:tv.white.level",dpx_tv_info.white_level);
          R32ToAttribute(image,"DPX:tv.integration.time",dpx_tv_info.integration_time);
        }
    }
  if (pixels_offset >= 2080UL)
    {
      if (!IS_UNDEFINED_U32(dpx_file_info.user_defined_length) &&
          (dpx_file_info.user_defined_length >= sizeof(DPXUserDefinedData)))
        {
          /*
            Read user header.
          */
          unsigned char
            *user_data;
          
          const size_t
            block_size = 65536UL;
          
          size_t
            read_size,
            user_data_length;

          DPXUserDefinedData
            *dpx_user_data;
          
          user_data_length=0UL;
          user_data=(unsigned char *) NULL;

          while (user_data_length < dpx_file_info.user_defined_length)
            {
              read_size=Min(block_size,dpx_file_info.user_defined_length-user_data_length);
              MagickReallocMemory(unsigned char *,user_data,user_data_length+read_size);
              if (user_data == (unsigned char *) NULL)
                ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
              if (ReadBlob(image,read_size,user_data+user_data_length) != read_size)
                {
                  MagickFreeMemory(user_data);
                  ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
                }
              user_data_length += read_size;
              offset += read_size;
            }

          dpx_user_data=(DPXUserDefinedData *) user_data;
          StringToAttribute(image,"DPX:user.data.id",dpx_user_data->user_id);
          if (!SetImageProfile(image,"DPXUSERDATA",user_data,user_data_length))
            {
              MagickFreeMemory(user_data);
              ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
            }
          MagickFreeMemory(user_data);
        }
    }
  /*
    Determine the maximum number of bits per sample, samples per element, and colorspace
  */
  max_bits_per_sample=0;
  max_samples_per_pixel=0;
  {
    MagickBool
      has_cbcr=MagickFalse,
      has_luma=MagickFalse,
      has_matte=MagickFalse,
      has_rgb=MagickFalse;

    DPXColorimetric
      colorimetric=ColorimetricUserDefined;

    transfer_characteristic=TransferCharacteristicUserDefined;
    for (element=0; element < dpx_image_info.elements; element++)
      {
        if (element == 0)
          {
            DPXColorimetric colorimetric=
              (DPXColorimetric) dpx_image_info.element_info[element].colorimetric;
            DPXSetPrimaryChromaticities(colorimetric,&image->chromaticity);
          }
        element_descriptor=(DPXImageElementDescriptor)
          dpx_image_info.element_info[element].descriptor;
        max_bits_per_sample=Max(max_bits_per_sample,
                                dpx_image_info.element_info[element].bits_per_sample);
        max_samples_per_pixel=Max(max_samples_per_pixel,
                                  DPXSamplesPerPixel(element_descriptor));
        /*
          Set image colorspace
        */
        switch (element_descriptor)
          {
          case ImageElementColorDifferenceCbCr:
          case ImageElementCbYCrY422:
          case ImageElementCbYACrYA4224:
          case ImageElementCbYCr444:
          case ImageElementCbYCrA4444:
            {
              has_cbcr=MagickTrue;
              colorimetric=(DPXColorimetric) dpx_image_info.element_info[element].colorimetric;
              transfer_characteristic=(DPXTransferCharacteristic) dpx_image_info.element_info[element].transfer_characteristic;
              break;
            }
          case ImageElementRed:
          case ImageElementGreen:
          case ImageElementBlue:
          case ImageElementRGB:
          case ImageElementRGBA:
          case ImageElementABGR:
            {
              has_rgb=MagickTrue;
              colorimetric=(DPXColorimetric) dpx_image_info.element_info[element].colorimetric;
              transfer_characteristic=(DPXTransferCharacteristic) dpx_image_info.element_info[element].transfer_characteristic;
              break;
            }
          case ImageElementLuma:
            {
              has_luma=MagickTrue;
              colorimetric=(DPXColorimetric) dpx_image_info.element_info[element].colorimetric;
              transfer_characteristic=(DPXTransferCharacteristic) dpx_image_info.element_info[element].transfer_characteristic;
              break;
            }
          default:
            {
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Unhandled element descriptor: %s",
                                    DescribeImageElementDescriptor(element_descriptor));
            }
          }
        
        /*
          Check for a matte channel.
        */
        switch (element_descriptor)
          {
          case ImageElementAlpha:
          case ImageElementRGBA:
          case ImageElementABGR:
          case ImageElementCbYACrYA4224:
          case ImageElementCbYCrA4444:
            has_matte=MagickTrue;
            break;
          default:
            break;
          }
      }
    if (has_cbcr)
      {
        image->colorspace=Rec709YCbCrColorspace;
        if ((transfer_characteristic == TransferCharacteristicITU_R601_625L) ||
            (transfer_characteristic == TransferCharacteristicITU_R601_525L))
          image->colorspace=Rec601YCbCrColorspace;
      }
    else if (has_luma)
      {
        image->colorspace=GRAYColorspace;
        if (transfer_characteristic == TransferCharacteristicITU_R709)
          image->colorspace=Rec709LumaColorspace;
        else if ((transfer_characteristic == TransferCharacteristicITU_R601_625L) ||
                 (transfer_characteristic == TransferCharacteristicITU_R601_525L))
          image->colorspace=Rec601LumaColorspace;
      }
    else if (has_rgb)
      {
        image->colorspace=RGBColorspace;
        if (transfer_characteristic == TransferCharacteristicPrintingDensity)
          image->colorspace=CineonLogRGBColorspace;
      }

    image->matte=has_matte;
  }
  if (image->logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "Image colorspace: %s",
                          ColorspaceTypeToString(image->colorspace));
  /*
    Set image depth to maximum bits per sample encountered in any element.
  */
  image->depth=max_bits_per_sample;
  /*
    Skip reading pixels if ping requested.
  */
  if (image_info->ping)
    {
      CloseBlob(image);
      return(image);
    }
  /*
    Read remainder of header.
  */
  for ( ; offset < pixels_offset ; offset++ )
    if (ReadBlobByte(image) == EOF)
      ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);

  /*
    Allocate sample translation map storage.
  */
  map_Y=MagickAllocateArray(Quantum *,
                            MaxValueGivenBits(max_bits_per_sample)+1,
                            sizeof(Quantum));
  if (map_Y == (Quantum *) NULL)
    {
      ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
    }
  
  map_CbCr=MagickAllocateArray(Quantum *,
                               MaxValueGivenBits(max_bits_per_sample)+1,
                               sizeof(Quantum));
  if (map_CbCr == (Quantum *) NULL)
    {
      MagickFreeMemory(map_Y);
      ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
    }
  /*
    Allocate per-thread-view row samples.
  */
  samples_set=AllocateThreadViewDataArray(image,exception,image->columns,
                                          max_samples_per_pixel*sizeof(sample_t));
  if (samples_set == (ThreadViewDataSet *) NULL)
    {
      MagickFreeMemory(map_CbCr);
      MagickFreeMemory(map_Y);
      ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
    }
  /*
    Allocate per-thread-view scanline storage.
  */
  scanline_set=AllocateThreadViewDataArray(image,exception,image->columns,
                                           max_samples_per_pixel*sizeof(U32));
  if (scanline_set == (ThreadViewDataSet *) NULL)
    {
      DestroyThreadViewDataSet(samples_set);
      MagickFreeMemory(map_CbCr);
      MagickFreeMemory(map_Y);
      ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
    }
  /*
    Allow user to over-ride pixel endianness.
  */
  if ((definition_value=AccessDefinition(image_info,"dpx","pixel-endian")))
    {
      if (LocaleCompare(definition_value,"msb") == 0)
        endian_type=MSBEndian;
      else if (LocaleCompare(definition_value,"lsb") == 0)
        endian_type=LSBEndian;
    }
  /*
    Convert DPX raster image to pixel packets.
  */
  for (element=0; element < dpx_image_info.elements; element++)
    {
      unsigned long
        row_count=0;

      MagickBool
        swap_word_datums = MagickFalse;

      DescribeDPXImageElement(&dpx_image_info.element_info[element],element+1);
      /*
        Data sign, (0 = unsigned; 1 = signed)
      */
      if (dpx_image_info.element_info[element].data_sign != 0)
        continue;
      /*
        Bits per sample must be must be 1 to 16.
      */
      if (dpx_image_info.element_info[element].bits_per_sample > 16)
        continue;
      /*
        Move to element data
      */
      if (!IS_UNDEFINED_U32(dpx_image_info.element_info[element].data_offset) &&
          (dpx_image_info.element_info[element].data_offset != 0U))
        {
          pixels_offset=dpx_image_info.element_info[element].data_offset;
#if 0
          offset=SeekBlob(image,(magick_off_t) pixels_offset,SEEK_SET);
#else
          if (pixels_offset >= offset)
            {
              /* Data is at, or ahead of current position.  Good! */
              for ( ; offset < pixels_offset ; offset++ )
                (void) ReadBlobByte(image);
            }
          else
            {
              /* Data is behind current position.  Bad! */
              offset=SeekBlob(image,(magick_off_t) pixels_offset,SEEK_SET);
            }

          /* Verify that we reached our offset objective */
          if ( pixels_offset != offset)
            ThrowReaderException(BlobError,UnableToSeekToOffset,image);
#endif
        }
      bits_per_sample=dpx_image_info.element_info[element].bits_per_sample;
      element_descriptor=(DPXImageElementDescriptor)
        dpx_image_info.element_info[element].descriptor;
      transfer_characteristic=
        (DPXTransferCharacteristic) dpx_image_info.element_info[element].transfer_characteristic;
      packing_method=(ImageComponentPackingMethod) dpx_image_info.element_info[element].packing;
      /*
        Allow the user to over-ride the packing method specified by the header.
      */
      if ((definition_value=AccessDefinition(image_info,"dpx","packing-method")))
        {
          if (LocaleCompare(definition_value,"packed") == 0)
            {
              packing_method=PackingMethodPacked;
            }
          else if ((bits_per_sample == 10) || (bits_per_sample == 12))
            {
              if ((LocaleCompare(definition_value,"lsbpad") == 0) ||
                  (LocaleCompare(definition_value,"a") == 0))
                packing_method=PackingMethodWordsFillLSB;
              else if ((LocaleCompare(definition_value,"msbpad") == 0) ||
                       (LocaleCompare(definition_value,"b") == 0))
                packing_method=PackingMethodWordsFillMSB;
            }
        }
      /*
        Decide if the image is grayscale and monochrome.
      */
      if (IsGrayColorspace(image->colorspace))
        {
          is_grayscale=MagickTrue;
        }
      if ((is_grayscale) && (bits_per_sample == 1))
        {
          is_monochrome=MagickTrue;
        }
      /*
        Are datums returned in reverse order when extracted from a
        32-bit word?  This is to support Note 2 in Table 1 which
        describes how RGB/RGBA are returned in reversed order for the
        10-bit "filled" format.  Note 3 refers to Note 2 so presumably
        the same applies for ABGR.  The majority of YCbCr 4:2:2 files
        received have been swapped (but not YCbCr 4:4:4 for some
        reason) so swap the samples for YCbCr 4:2:2 as well.
      */
      if ((element_descriptor == ImageElementRGB) ||
          (element_descriptor == ImageElementRGBA) ||
          (element_descriptor == ImageElementABGR) ||
          (element_descriptor == ImageElementCbYCrY422) ||
          (element_descriptor == ImageElementCbYACrYA4224))
        {
          if ((bits_per_sample == 10) && (packing_method != PackingMethodPacked))
            swap_word_datums = MagickTrue;
        }
      if ((definition_value=AccessDefinition(image_info,"dpx","swap-samples")))
        {
          if (LocaleCompare(definition_value,"false") != 0)
            swap_word_datums = swap_word_datums ? MagickFalse : MagickTrue;
        }
      /*
        Determine number of samples per pixel element.
      */
      samples_per_pixel=DPXSamplesPerPixel(element_descriptor);
      if (samples_per_pixel != 0)
        {
          unsigned int
            max_value_given_bits = MaxValueGivenBits(bits_per_sample),
            reference_low = 0,
            reference_high = max_value_given_bits,
            scale_to_short;             /* multiplier to scale to 16-bits */

          scale_to_short=1U;
          if (bits_per_sample < 16U)
            scale_to_short=(65535U / (65535U >> (16-bits_per_sample)));

          /*
            Is this a video type space?
          */
          if (IsYCbCrColorspace(image->colorspace) ||
              (image->colorspace == Rec601LumaColorspace) ||
              (image->colorspace == Rec709LumaColorspace))
            {
              double
                ScaleY = 0.0,
                ScaleCbCr = 0.0;

              /*
                Establish YCbCr video defaults.
                8 bit ==> Luma 16 to 235
                10 bit ==> Luma 64 to 940
              */
              reference_low = (((double) max_value_given_bits+1) * (64.0/1024.0));
              reference_high = (((double) max_value_given_bits+1) * (940.0/1024.0));
              
              if (!IS_UNDEFINED_U32(dpx_image_info.element_info[element].reference_low_data_code))
                reference_low=dpx_image_info.element_info[element].reference_low_data_code;
              if ((definition_value=AccessDefinition(image_info,"dpx","reference-low")))
                reference_low=strtol(definition_value, (char **)NULL, 10);
              
              if (!IS_UNDEFINED_U32(dpx_image_info.element_info[element].reference_high_data_code))
                reference_high=dpx_image_info.element_info[element].reference_high_data_code;
              if ((definition_value=AccessDefinition(image_info,"dpx","reference-high")))
                reference_high=strtol(definition_value, (char **)NULL, 10);
              
              ScaleY = (((double) max_value_given_bits+1.0)/(reference_high-reference_low));
              ScaleCbCr = ScaleY*((940.0-64.0)/(960.0-64.0));
              reference_low=ScaleShortToQuantum(reference_low*scale_to_short);

              for(i=0; i <= max_value_given_bits; i++)
                {
                  map_Y[i] = ScaleFromVideo(ScaleShortToQuantum(i*scale_to_short),reference_low,ScaleY);
                  map_CbCr[i] = ScaleFromVideo(ScaleShortToQuantum(i*scale_to_short),reference_low,ScaleCbCr);
                }
            }
          else
            {
              for(i=0; i <= max_value_given_bits; i++)
                map_Y[i]=ScaleShortToQuantum(i*scale_to_short);
            }

          /*
            Compute samples per row.
          */
          samples_per_row=samples_per_pixel*image->columns;
          /*
            Compute octets per row.
          */
          row_octets=DPXRowOctets(1,samples_per_row,bits_per_sample,packing_method);
          if (image->logging)
            {
              /*
                Compute element size.
              */
              element_size=DPXRowOctets(image->rows,samples_per_row,
                                        bits_per_sample,packing_method);
              
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "Samples per row %u, octets per row %lu, element size %lu",
                                    samples_per_row, (unsigned long) row_octets,
                                    (unsigned long) element_size);
            }
          if (((element_descriptor == ImageElementCbYCrY422) ||
               (element_descriptor == ImageElementCbYACrYA4224) ||
               (element_descriptor == ImageElementColorDifferenceCbCr)))
            {
              /*
                When subsampling, image width must be evenly divisible by two.
              */
              if (image->columns %2)
                ThrowReaderException(CorruptImageError,SubsamplingRequiresEvenWidth,image);
            }
          /*
            Read element data.
          */
#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  pragma omp parallel for schedule(dynamic,1)
#endif
          for (y=0; y < (long) image->rows; y++)
            {
              MagickBool
                thread_status;

              register long
                x;
              
              register PixelPacket
                *q;

              sample_t
                *samples_itr;               /* current sample */

              PixelPacket
                *pixels=(PixelPacket *) NULL;

              sample_t
                *samples;                   /* parsed sample array */
              
              unsigned char
                *scanline;

              unsigned long
                thread_row_count;

              thread_status=status;
              if (thread_status == MagickFail)
                continue;

              samples=AccessThreadViewData(samples_set);
              scanline=AccessThreadViewData(scanline_set);

              /*
                Obtain a row's worth of samples.
              */
              {
                void
                  *scanline_data;
                
                scanline_data=scanline;
#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  pragma omp critical
#endif
                {
                  if (ReadBlobZC(image,row_octets,&scanline_data) != row_octets)
                    thread_status=MagickFail;

                  thread_row_count=row_count;
                  row_count++;
                  if (QuantumTick(thread_row_count,image->rows))
                    if (!MagickMonitorFormatted(thread_row_count,image->rows,exception,
                                                LoadImageText,image->filename))
                      thread_status=MagickFail;
                }
                  
                if (thread_status != MagickFail)
                  ReadRowSamples((const unsigned char*) scanline_data,samples_per_row,bits_per_sample,
                                 packing_method,endian_type,swap_word_datums,samples);
              }

              if (thread_status != MagickFail)
                {
                  if (element == 0)
                    pixels=SetImagePixelsEx(image,0,thread_row_count,image->columns,1,exception);
                  else
                    pixels=GetImagePixelsEx(image,0,thread_row_count,image->columns,1,exception);
                }

              if (pixels == (PixelPacket *) NULL)
                thread_status=MagickFail;

              if (thread_status == MagickFail)
                {
#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  pragma omp critical
#endif
                  status=thread_status;
                  continue;
                }

              /*
                Assign samples to pixels.
              */
              q = pixels;
              samples_itr=samples;
              switch (element_descriptor)
                {
                case ImageElementRed:
                  for (x=image->columns; x != 0; x--)
                    {
                      SetRedSample(q++,map_Y[*samples_itr++]);
                    }
                  break;
                case ImageElementGreen:
                  for (x=image->columns; x != 0; x--)
                    {
                      SetGreenSample(q++,map_Y[*samples_itr++]);
                    }
                  break;
                case ImageElementBlue:
                  for (x=image->columns; x != 0; x--)
                    {
                      SetBlueSample(q++,map_Y[*samples_itr++]);
                    }
                  break;
                case ImageElementAlpha:
                  for (x=image->columns; x != 0; x--)
                    {
                      SetOpacitySample(q++,map_Y[*samples_itr++]);
                    }
                  break;
                case ImageElementUnspecified:
                case ImageElementLuma:
                  if (IsYCbCrColorspace(image->colorspace))
                    {
                      /* Video Luma (planar) */
                      for (x=image->columns; x != 0; x--)
                        {
                          SetRedSample(q,map_Y[*samples_itr++]);
                          q++;
                        }
                    }
                  else
                    {
                      /* Video Luma or Linear Grayscale */
                      for (x=image->columns; x != 0; x--)
                        {
                          SetGraySample(q,map_Y[*samples_itr++]);
                          q++;
                        }
                    }
                  break;
                case ImageElementColorDifferenceCbCr:
                  {
                    /* CbCr 4:2:2 sampling */
                    for (x=image->columns; x > 0; x -= 2)
                      {
                        Quantum
                          Cb,
                          Cr;
                        
                        Cb=map_CbCr[*samples_itr++]; /* Cb */
                        Cr=map_CbCr[*samples_itr++]; /* Cr */
                    
                        SetCbSample(q,Cb);  /* Cb */
                        SetCrSample(q,Cr);  /* Cr */
                        q++;

                        SetCbSample(q,Cb);  /* Cb (false) */
                        SetCrSample(q,Cr);  /* Cr (false) */
                        q++;
                      }
                    TentUpsampleChroma(pixels,image->columns);
                    break;
                  }
                case ImageElementRGB:
                  /* RGB order */
                  for (x=image->columns; x != 0; x--)
                    {
                      SetRedSample(q,map_Y[*samples_itr++]);
                      SetGreenSample(q,map_Y[*samples_itr++]);
                      SetBlueSample(q,map_Y[*samples_itr++]);
                      SetOpacitySample(q,OpaqueOpacity);
                      q++;
                    }
                  break;
                case ImageElementRGBA:
                  /* RGB order */
                  for (x=image->columns; x != 0; x--)
                    {
                      SetRedSample(q,map_Y[*samples_itr++]);
                      SetGreenSample(q,map_Y[*samples_itr++]);
                      SetBlueSample(q,map_Y[*samples_itr++]);
                      SetOpacitySample(q,map_Y[*samples_itr++]);
                      q++;
                    }
                  break;
                case ImageElementABGR:
                  /* ARGB order */
                  for (x=image->columns; x != 0; x--)
                    {
                      SetOpacitySample(q,map_Y[*samples_itr++]);
                      SetRedSample(q,map_Y[*samples_itr++]);
                      SetGreenSample(q,map_Y[*samples_itr++]);
                      SetBlueSample(q,map_Y[*samples_itr++]);
                      q++;
                    }
                  break;
                case ImageElementCbYCrY422:
                  {
                    /* CbY | CrY | CbY | CrY ..., even number of columns required. */
                    for (x=image->columns; x > 0; x -= 2)
                      {
                        Quantum
                          Cb,
                          Cr,
                          Y0,
                          Y1;

                        Cb=map_CbCr[*samples_itr++]; /* Cb */
                        Y0=map_Y[*samples_itr++];    /* Y0 */
                        Cr=map_CbCr[*samples_itr++]; /* Cr */
                        Y1=map_Y[*samples_itr++];    /* Y1 */

                        SetYSample(q,Y0);   /* Y0 */
                        SetCbSample(q,Cb);  /* Cb */
                        SetCrSample(q,Cr);  /* Cr */
                        SetOpacitySample(q,OpaqueOpacity);
                        q++;

                        SetYSample(q,Y1);   /* Y1 */
                        SetCbSample(q,Cb) ; /* Cb (false) */
                        SetCrSample(q,Cr);  /* Cr (false) */
                        q->opacity=OpaqueOpacity;
                        q++;
                      }
                    TentUpsampleChroma(pixels,image->columns);
                    break;
                  }
                case ImageElementCbYACrYA4224:
                  {
                    /* CbYA | CrYA ..., even number of columns required. */
                    for (x=image->columns; x > 0; x -= 2)
                      {
                        Quantum
                          A0,
                          A1,
                          Cb,
                          Cr,
                          Y0,
                          Y1;

                        Cb=map_CbCr[*samples_itr++];
                        Y0=map_Y[*samples_itr++];
                        A0=map_Y[*samples_itr++];

                        Cr=map_CbCr[*samples_itr++];
                        Y1=map_Y[*samples_itr++];
                        A1=map_Y[*samples_itr++];

                        SetYSample(q,Y0);       /* Y0 */
                        SetCbSample(q,Cb);      /* Cb */
                        SetCrSample(q,Cr);      /* Cr */
                        SetOpacitySample(q,A0); /* A0 */
                        q++;

                        SetYSample(q,Y1);       /* Y1 */
                        SetCbSample(q,Cb);      /* Cb (false) */
                        SetCrSample(q,Cr);      /* Cr (false) */
                        SetOpacitySample(q,A1); /* A1 */
                        q++;
                      }
                    TentUpsampleChroma(pixels,image->columns);
                    break;
                  }
                case ImageElementCbYCr444:
                  {
                    /* red,green,blue = Y, Cb, Cr */
                    for (x=image->columns; x != 0; x--)
                      {
                        SetCbSample(q,map_CbCr[*samples_itr++]); /* Cb */
                        SetYSample(q,map_Y[*samples_itr++]);     /* Y */
                        SetCrSample(q,map_CbCr[*samples_itr++]); /* Cr */
                        SetOpacitySample(q,OpaqueOpacity);      /* A */
                        q++;
                      }
                    break;
                  }
                case ImageElementCbYCrA4444:
                  {
                    /* red,green,blue = Y, Cb, Cr */
                    for (x=image->columns; x != 0; x--)
                      {
                        SetCbSample(q,map_CbCr[*samples_itr++]);   /* Cb */
                        SetYSample(q,map_Y[*samples_itr++]);       /* Y */
                        SetCrSample(q,map_CbCr[*samples_itr++]);   /* Cr */
                        SetOpacitySample(q,map_Y[*samples_itr++]); /* A */
                        q++;
                      }
                    break;
                  }
                default:
                  break;
                }

              if (!SyncImagePixelsEx(image,exception))
                thread_status=MagickFail;

              /*
                FIXME: Add support for optional EOL padding.
              */              
              if (thread_status == MagickFail)
#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  pragma omp critical
#endif
                status=MagickFail;
#if 0
              if (BlobIsSeekable(image))
                {
                  magick_off_t reported_file_offset = TellBlob(image);
                  if (EOFBlob(image))
                    {
                      (void) fprintf(stderr,"### File length %u, TellBlob says %ld\n",
                                     dpx_file_info.file_size,
                                     (long) reported_file_offset);
                      break;
                    }
                }
#endif
            }
          /* break; */
        }
      else
        {
          ThrowReaderException(CoderError,ColorTypeNotSupported,image);
        }
    }
    
  if (EOFBlob(image))
    ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,
                   image->filename);

  /*
    Support explicitly overriding the input file's colorspace.  Mostly
    useful for testing.
  */
  if ((definition_value=AccessDefinition(image_info,"dpx","colorspace")))
    {
      ColorspaceType
        colorspace;

      colorspace=StringToColorspaceType(definition_value);
      if (colorspace != UndefinedColorspace)
        {
          image->colorspace=colorspace;
          if (image->logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "Explicitly set colorspace to %s",
                                  ColorspaceTypeToString(image->colorspace));
        }
      else
        {
          if (image->logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "Unrecognized source colorspace \"%s\"\n",
                                  definition_value);
          ThrowException(&image->exception,OptionError,UnrecognizedColorspace,
                         definition_value);
        }
    }

  /*
    If image is YCbCr representing Cineon Log RGB, then return the image as
    RGB in CineonLog colorspace.
  */
  if (IsYCbCrColorspace(image->colorspace) &&
      (transfer_characteristic == TransferCharacteristicPrintingDensity))
    {
      (void) TransformColorspace(image,RGBColorspace);
      image->colorspace=CineonLogRGBColorspace;
    }

  image->is_monochrome=is_monochrome;
  image->is_grayscale=is_grayscale;
  image->depth=Min(QuantumDepth,image->depth);
  MagickFreeMemory(map_CbCr);
  MagickFreeMemory(map_Y);
  DestroyThreadViewDataSet(scanline_set);
  DestroyThreadViewDataSet(samples_set);
  CloseBlob(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r D P X I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterDPXImage adds attributes for the DPX image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterDPXImage method is:
%
%      RegisterDPXImage(void)
%
*/
ModuleExport void RegisterDPXImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("DPX");
  entry->decoder=(DecoderHandler) ReadDPXImage;
  entry->encoder=(EncoderHandler) WriteDPXImage;
  entry->magick=(MagickHandler) IsDPX;
  entry->description="SMPTE 268M-2003 (DPX 2.0)";
  entry->note="See http://www.smtpe.org/ for information on DPX.";
  entry->module="DPX";
  entry->adjoin=MagickFalse; /* Only one frame per file */
  entry->seekable_stream=MagickFalse; /* Does not reqire seek() */
  entry->coder_class=PrimaryCoderClass;
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r D P X I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterDPXImage removes format registrations made by the
%  DPX module from the list of supported formats.
%
%  The format of the UnregisterDPXImage method is:
%
%      UnregisterDPXImage(void)
%
*/
ModuleExport void UnregisterDPXImage(void)
{
  (void) UnregisterMagickInfo("DPX");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e D P X I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteDPXImage writes an image in DPX encoded image format.
%
%  The format of the WriteDPXImage method is:
%
%      unsigned int WriteDPXImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteDPXImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
STATIC void GenerateDPXTimeStamp(char *timestamp, size_t maxsize)
{
  time_t
    current_time;

  const struct tm
    *t;

  char *
    p;

  current_time=time((time_t *) NULL);
  t=localtime(&current_time);

  (void) strftime(timestamp,maxsize,"%Y:%m:%d:%H:%M:%S%Z",t);
  timestamp[maxsize-1]='\0';
  for (p=timestamp ; *p != '\0'; p++)
    if (*p == ' ')
      *p='0';
}

STATIC unsigned int OrientationTypeToDPXOrientation(const OrientationType orientation_type)
{
  unsigned int
    orientation = 0;

  switch (orientation_type)
    {
    case UndefinedOrientation:
    case TopLeftOrientation:
      orientation=0;
      break;
    case TopRightOrientation:
      orientation=1;
      break;
    case BottomLeftOrientation:
      orientation=2;
      break;
    case BottomRightOrientation:
      orientation=3;
      break;
    case LeftTopOrientation:
      orientation=4;
      break;
    case RightTopOrientation:
      orientation=5;
      break;
    case LeftBottomOrientation:
      orientation=6;
      break;
    case RightBottomOrientation:
      orientation=7;
      break;
    }
  return orientation;
}

#define LSBPackedU32WordToOctets(packed_u32,scanline) \
{ \
  *scanline++=(unsigned char) ((packed_u32) & 0xFF); \
  *scanline++=(unsigned char) ((packed_u32 >> 8) & 0xFF); \
  *scanline++=(unsigned char) ((packed_u32 >> 18) & 0xFF); \
  *scanline++=(unsigned char) ((packed_u32 >> 24) & 0xFF); \
}
#define MSBPackedU32WordToOctets(packed_u32,scanline) \
{ \
  *scanline++=(unsigned char) ((packed_u32 >> 24) & 0xFF); \
  *scanline++=(unsigned char) ((packed_u32 >> 16) & 0xFF); \
  *scanline++=(unsigned char) ((packed_u32 >> 8) & 0xFF); \
  *scanline++=(unsigned char) ((packed_u32) & 0xFF); \
}

/*
  WordStreamLSBWrite support
*/
typedef struct _WriteWordU32State
{
  unsigned char *words;
} WriteWordU32State;

STATIC size_t WriteWordU32BE (void *state, const unsigned long value)
{
  WriteWordU32State *write_state=(WriteWordU32State *) state;
  *write_state->words++ = (unsigned char) ((value >> 24) & 0xff);
  *write_state->words++ = (unsigned char) ((value >> 16) & 0xff);
  *write_state->words++ = (unsigned char) ((value >> 8) & 0xff);
  *write_state->words++ = (unsigned char) (value & 0xff);
  return sizeof(magick_uint32_t);
}

STATIC size_t WriteWordU32LE (void *state, const unsigned long value)
{
  WriteWordU32State *write_state=(WriteWordU32State *) state;
  *write_state->words++ = (unsigned char) (value & 0xff);
  *write_state->words++ = (unsigned char) ((value >> 8) & 0xff);
  *write_state->words++ = (unsigned char) ((value >> 16) & 0xff);
  *write_state->words++ = (unsigned char) ((value >> 24) & 0xff);
  return sizeof(magick_uint32_t);
}

/*
  Encode row samples. Currently just one row but in the future may be
                      multiple rows (e.g. 3).

  samples          -- unencoded samples (currently unsigned 16-bit).
  samples_per_row  -- Number of samples to encode.
  bits_per_sample  -- Number of bits in one decoded sample.
  packing_method   -- Describes the way that samples are packed into enclosing words.
  endian_type      -- The endian order of the enclosing words.
  swap_word_datums -- Use alternate sample order (BGR vs RGB, CbYCr vs CrYCb) for
                      samples filled into 32 bit words.
  scanline         -- Raw output data (may be 8-bit, 16-bit, 32-bit, or 64-bit types)
                      which represents the encoded pixels for one or more scanlines.
                      Underlying input data type is properly aligned for access.
*/
STATIC void WriteRowSamples(const sample_t *samples,
                            const unsigned int samples_per_row,
                            const unsigned int bits_per_sample,
                            const ImageComponentPackingMethod packing_method,
                            const EndianType endian_type,
                            const MagickBool swap_word_datums,
                            unsigned char *scanline)
{
  register unsigned int
    i;

  BitStreamWriteHandle
    bit_stream;

  register unsigned char
    *sp;

  register unsigned int
    sample;

  sp=scanline;
  BitStreamInitializeWrite(&bit_stream,scanline);

  if ((packing_method != PackingMethodPacked) &&
      ((bits_per_sample == 10) || (bits_per_sample == 12)))
    {
      MagickBool
        word_pad_lsb=MagickFalse,
        word_pad_msb=MagickFalse;

      if (packing_method == PackingMethodWordsFillLSB)
        word_pad_lsb=MagickTrue;
      else if (packing_method == PackingMethodWordsFillMSB)
        word_pad_msb=MagickTrue;

      if (bits_per_sample == 10)
        {
          register magick_uint32_t
            packed_u32;
          
          register unsigned int
            datum;
          
          unsigned int
            shifts[3] = { 0, 0, 0 };

          if (word_pad_lsb)
            {
              /*
                Padding in LSB (Method A)  Standard method.
              */
              if (swap_word_datums == MagickFalse)
                {
                  shifts[0]=2;  /* datum-0 / blue */
                  shifts[1]=12; /* datum-1 / green */
                  shifts[2]=22; /* datum-2 / red */
                }
              else
                {
                  shifts[0]=22; /* datum-2 / red */
                  shifts[1]=12; /* datum-1 / green */
                  shifts[2]=2;  /* datum-0 / blue */
                }
            }
          else if (word_pad_msb)
            {
              /*
                Padding in MSB (Method B)  Deprecated method.
              */
              if (swap_word_datums == MagickFalse)
                {
                  shifts[0]=0;  /* datum-0 / blue */
                  shifts[1]=10; /* datum-1 / green */
                  shifts[2]=20; /* datum-2 / red */
                }
              else
                {
                  shifts[0]=20; /* datum-2 / red */
                  shifts[1]=10; /* datum-1 / green */
                  shifts[2]=0;  /* datum-0 / blue */
                }
            }

          if (endian_type == MSBEndian)
            {
              /* Standard specified datum order */
              for (i=(samples_per_row/3); i != 0; --i)
                {
                  datum=0;
                  packed_u32=0;
                  packed_u32 |= (*samples++ << shifts[datum++]);
                  packed_u32 |= (*samples++ << shifts[datum++]);
                  packed_u32 |= (*samples++ << shifts[datum]);
                  MSBPackedU32WordToOctets(packed_u32,scanline);
                }
              if ((samples_per_row % 3))
                {
                  datum=0;
                  packed_u32=0;
                  for (i=(samples_per_row % 3); i != 0; --i)
                    packed_u32 |= (*samples++ << shifts[datum++]);
                  MSBPackedU32WordToOctets(packed_u32,scanline);
                }
            }
          else if (endian_type == LSBEndian)
            {
              /* Standard specified datum order */
              for (i=(samples_per_row/3); i != 0; --i)
                {
                  datum=0;
                  packed_u32=0;
                  packed_u32 |= (*samples++ << shifts[datum++]);
                  packed_u32 |= (*samples++ << shifts[datum++]);
                  packed_u32 |= (*samples++ << shifts[datum]);
                  LSBPackedU32WordToOctets(packed_u32,scanline);
                }
              if ((samples_per_row % 3))
                {
                  datum=0;
                  packed_u32=0;
                  for (i=(samples_per_row % 3); i != 0; --i)
                    packed_u32 |= (*samples++ << shifts[datum++]);
                  LSBPackedU32WordToOctets(packed_u32,scanline);
                }
            }
          return;
        }
      else if (bits_per_sample == 12)
        {
          if (word_pad_lsb)
            {
              /*
                Padding in LSB (Method A).
              */
              if (endian_type == MSBEndian)
                {
                  for (i=samples_per_row; i != 0; i--)
                    {
                      sample=*samples++;
                      sample <<= 4;
                      *sp++=(unsigned char) (((unsigned int) sample) >> 8);
                      *sp++=(unsigned char) sample;
                    }
                }
              else if (endian_type == LSBEndian)
                {
                  for (i=samples_per_row; i != 0; i--)
                    {
                      sample=*samples++;
                      sample <<= 4;
                      *sp++=(unsigned char) sample;
                      *sp++=(unsigned char) (((unsigned int) sample) >> 8);
                    }
                }
            }
          else if (word_pad_msb)
            {
              /*
                Padding in MSB (Method B).
              */
              if (endian_type == MSBEndian)
                {
                  for (i=samples_per_row; i != 0; i--)
                    {
                      sample=((*samples++) & 0xFFF);
                      *sp++=(unsigned char) (((unsigned int) sample) >> 8);
                      *sp++=(unsigned char) sample;
                    }
                }
              else if (endian_type == LSBEndian)
                {
                  for (i=samples_per_row; i != 0; i--)
                    {
                      sample=((*samples++) & 0xFFF);
                      *sp++=(unsigned char) sample;
                      *sp++=(unsigned char) (((unsigned int) sample) >> 8);
                    }
                }
            }
          return;
        }
    }

  /*
    Special fast handling for 8-bit images.
  */
  if (bits_per_sample == 8)
    {
      for (i=samples_per_row; i != 0; i--)
        *sp++=(unsigned char) *samples++;
      return;
    }

  /*
    Special fast handling for 16-bit images.
  */
  if (bits_per_sample == 16)
    {
      if (endian_type == MSBEndian)
        {
          for (i=samples_per_row; i != 0; i--)
            {
              *sp++=(unsigned char) (((unsigned int) *samples) >> 8);
              *sp++=(unsigned char) *samples;
              samples++;
            }
        }
      else if (endian_type == LSBEndian)
        {
          for (i=samples_per_row; i != 0; i--)
            {
              *sp++=(unsigned char) *samples;
              *sp++=(unsigned char) (((unsigned int) *samples) >> 8);
              samples++;
            }
        }
      return;
    }

#if 0
  /*
    Special fast handling for 32-bit (float) images.
  */
  if (bits_per_sample == 32)
    {
      register magick_uint32_t
        packed_u32;

      if (endian_type == MSBEndian)
        {
          for (i=samples_per_row; i != 0; i--)
            {
              packed_u32=*samples++;
              MSBPackedU32WordToOctets(packed_u32,scanline);
            }
        }
      else if (endian_type == LSBEndian)
        {
          for (i=samples_per_row; i != 0; i--)
            {
              packed_u32=*samples++;
              LSBPackedU32WordToOctets(packed_u32,scanline);
            }
        }
      return;
    }
#endif

  /*
    Packed data.
  */
  {
      WriteWordU32State
        write_state;
      
      WordStreamWriteHandle
        write_stream;

      WordStreamWriteFunc
        write_func=0;

      if (endian_type == MSBEndian)
        write_func=WriteWordU32BE;
      else if (endian_type == LSBEndian)
        write_func=WriteWordU32LE;

      write_state.words=scanline;
      WordStreamInitializeWrite(&write_stream,write_func, (void *) &write_state);

      for (i=samples_per_row; i != 0; i--)
        WordStreamLSBWrite(&write_stream,bits_per_sample,*samples++);

      WordStreamLSBWriteFlush(&write_stream);
  }
}

#define AttributeToU8(image_info,image,key,member) \
{ \
  const ImageAttribute \
    *attribute_; \
\
  const char \
    *definition_value_; \
\
  if ((definition_value_=AccessDefinition(image_info,"dpx",key+4))) \
    member=(U8) strtol(definition_value_, (char **) NULL, 10); \
  else if ((attribute_=GetImageAttribute(image,key))) \
    member=(U8) strtol(attribute_->value, (char **) NULL, 10); \
  else \
    SET_UNDEFINED_U8(member); \
}

#define AttributeToU16(image_info,image,key,member) \
{ \
  const ImageAttribute \
    *attribute_; \
\
  const char \
    *definition_value_; \
\
  if ((definition_value_=AccessDefinition(image_info,"dpx",key+4))) \
    member=(U16) strtol(definition_value_, (char **) NULL, 10); \
  else if ((attribute_=GetImageAttribute(image,key))) \
    member=(U16) strtol(attribute_->value, (char **) NULL, 10); \
  else \
    SET_UNDEFINED_U16(member); \
}

#define AttributeToU32(image_info,image,key,member) \
{ \
  const ImageAttribute \
    *attribute_; \
\
  const char \
    *definition_value_; \
\
  if ((definition_value_=AccessDefinition(image_info,"dpx",key+4))) \
    member=(U32) strtol(definition_value_, (char **) NULL, 10); \
  else if ((attribute_=GetImageAttribute(image,key))) \
    member=(U32) strtol(attribute_->value, (char **) NULL, 10); \
  else \
    SET_UNDEFINED_U32(member); \
}

#define AttributeBitsToU32(image_info,image,key,member) \
{ \
  const ImageAttribute \
    *attribute_; \
\
  const char \
    *definition_value_; \
\
  if ((definition_value_=AccessDefinition(image_info,"dpx",key+4))) \
    member=SMPTEStringToBits(definition_value_); \
  else if ((attribute_=GetImageAttribute(image,key))) \
    member=SMPTEStringToBits(attribute_->value); \
  else \
    SET_UNDEFINED_U32(member); \
}

#define AttributeToR32(image_info,image,key,member) \
{ \
  const ImageAttribute \
    *attribute_; \
\
  const char \
    *definition_value_; \
\
  if ((definition_value_=AccessDefinition(image_info,"dpx",key+4))) \
    member=(R32) strtod(definition_value_, (char **) NULL); \
  else if ((attribute_=GetImageAttribute(image,key))) \
    member=(R32) strtod(attribute_->value, (char **) NULL); \
  else \
    SET_UNDEFINED_R32(member); \
}

/*
  This macro uses strncpy on purpose.  The string is not required to
  be null terminated, but any unused space should be filled with
  nulls.
*/
#define AttributeToString(image_info,image,key,member) \
{ \
  const ImageAttribute \
    *attribute_; \
\
  const char \
    *definition_value_; \
\
  if ((definition_value_=AccessDefinition(image_info,"dpx",key+4))) \
    (void) strncpy(member,definition_value_,sizeof(member)); \
  else if ((attribute_=GetImageAttribute(image,key))) \
    (void) strncpy(member,attribute_->value,sizeof(member)); \
  else \
    SET_UNDEFINED_ASCII(member); \
}

/*
  Scale a value to video levels.
*/
#define ScaleToVideo(sample,ref_low,dnscale) \
  ((unsigned int) (((double) sample*dnscale)+ref_low+0.5))

/*
  Round an offset up to specified offset boundary.
*/
#define RoundUpToBoundary(offset,boundary) \
  (((offset+boundary-1)/boundary)*boundary);

STATIC unsigned int WriteDPXImage(const ImageInfo *image_info,Image *image)
{
  DPXFileInfo
    dpx_file_info;

  DPXImageInfo
    dpx_image_info;

  DPXImageSourceInfo
    dpx_source_info;

  DPXMPFilmInfo
    dpx_mp_info;

  DPXTVInfo
    dpx_tv_info;

  DPXImageElementDescriptor
    element_descriptor;

  ImageComponentPackingMethod
    packing_method;

  DPXTransferCharacteristic
    transfer_characteristic;

  Image
    *chroma_image=0;

  unsigned long
    y;

  register const PixelPacket
    *p;
  
  register unsigned long
    i,
    x;

  sample_t
    *samples,
    *samples_itr;

  sample_t
    *map_Y,                     /* value translation map (RGB or Y) */
    *map_CbCr;                  /* value translation map (CbCr) */

  unsigned char
    *scanline;

  const unsigned char
    *user_data;

  unsigned int
    bits_per_sample=0,
    element,
    sampling_factor_horizontal,
    sampling_factor_vertical,
    max_samples_per_pixel,
    image_data_offset,
    number_of_elements,
    row_samples,
    samples_per_component,
    samples_per_pixel,
    samples_per_row,
    scale_from_short,
    status;

  MagickBool
    swab;

  size_t
    element_size;

  const char *
    definition_value;

  size_t
    offset=0,
    row_octets,
    user_data_length=0;

  EndianType
    endian_type;

  MagickBool
    is_grayscale;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenError,UnableToOpenFile,image);
  is_grayscale=image->is_grayscale;

  /*
    Support user-selection of big/little endian output.
  */
  endian_type=MSBEndian;
#if defined(WORDS_BIGENDIAN)
  swab=MagickFalse;
  if (image_info->endian == LSBEndian)
    {
      swab=MagickTrue;
      endian_type=LSBEndian;
    }
#else
  swab=MagickTrue;
  if (image_info->endian == LSBEndian)
    {
      swab=MagickFalse;
      endian_type=LSBEndian;
    }
#endif

  /*
    Adjust image colorspace if necessary.
  */
  if ((image_info->colorspace == CineonLogRGBColorspace) &&
      (image->colorspace != CineonLogRGBColorspace))
    (void) TransformColorspace(image,CineonLogRGBColorspace);
  else if ((image_info->colorspace == Rec601LumaColorspace) &&
           (image->colorspace != Rec601LumaColorspace))
    (void) TransformColorspace(image,Rec601LumaColorspace);
  else if ((image_info->colorspace == Rec601YCbCrColorspace) &&
           (image->colorspace != Rec601YCbCrColorspace))
    (void) TransformColorspace(image,Rec601YCbCrColorspace);
  else if ((image_info->colorspace == YCbCrColorspace) &&
           (image->colorspace != Rec601YCbCrColorspace))
    (void) TransformColorspace(image,Rec601YCbCrColorspace);
  else if ((image_info->colorspace == Rec709LumaColorspace) &&
           (image->colorspace != Rec709LumaColorspace))
    (void) TransformColorspace(image,Rec709LumaColorspace);
  else if ((image_info->colorspace == Rec709YCbCrColorspace) &&
           (image->colorspace != Rec709YCbCrColorspace))
    (void) TransformColorspace(image,Rec709YCbCrColorspace);
  else if (IsRGBColorspace(image_info->colorspace) &&
           !IsRGBColorspace(image->colorspace))
    (void) TransformColorspace(image,RGBColorspace);
  else if (!IsRGBColorspace(image->colorspace) &&
           (image->colorspace != CineonLogRGBColorspace) &&
           (image->colorspace != Rec601YCbCrColorspace) &&
           (image->colorspace != Rec709YCbCrColorspace))
    (void) TransformColorspace(image,RGBColorspace);

  /*
    Compute desired/necessary number of bits per sample.
  */
  if ((definition_value=AccessDefinition(image_info,"dpx","bits-per-sample")))
    bits_per_sample=atoi(definition_value);

  if (bits_per_sample == 0)
    {
      if (image->depth > 12 )
        bits_per_sample=16;
      else if (image->depth > 10)
        bits_per_sample=12;
      else if (image->depth > 8)
        bits_per_sample=10;
      else if (image->depth > 1)
        bits_per_sample=8;
      else
        bits_per_sample=1;
    }

  if (image->logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "Bits per sample: %u", bits_per_sample);

  /*
    Obtain requested sampling factors.
  */
  sampling_factor_horizontal=2;
  sampling_factor_vertical=2;
  if (image_info->sampling_factor != (char *) NULL)
    {
      long
        factors;

      factors=sscanf(image_info->sampling_factor,"%ux%u",&sampling_factor_horizontal,
                     &sampling_factor_vertical);
      if (factors != 2)
        sampling_factor_vertical=sampling_factor_horizontal;
      if ((sampling_factor_horizontal != 1) && (sampling_factor_horizontal != 2) &&
          (sampling_factor_vertical != 1) && (sampling_factor_vertical != 2))
        ThrowWriterException(OptionError,UnsupportedSamplingFactor,
                             image);

      /*
        When subsampling, image width must be evenly divisible by two.
      */
      if (((sampling_factor_horizontal / sampling_factor_vertical) == 2) &&
          (image->columns %2))
        ThrowWriterException(CoderError,SubsamplingRequiresEvenWidth,image);
    }

  /*
    Intuit the samples per component and the number of elements.
  */
  if (IsYCbCrColorspace(image->colorspace))
    {
      if ((image_info->interlace == PlaneInterlace) &&
          ((sampling_factor_horizontal / sampling_factor_vertical) == 2))
        {
          /* YCbCr 4:2:2 planar */
          samples_per_component=1;
          number_of_elements=2;
          if (image->matte)
            number_of_elements++;
        }
      else
        {
          if ((sampling_factor_horizontal / sampling_factor_vertical) == 2)
            {
              /* YCbCr 4:2:2 */
              samples_per_component=2;
            }
          else
            {
              /* YCbCr 4:4:4 */
              samples_per_component=3;
            }
          number_of_elements=1;
          if (image->matte)
            samples_per_component++;
        }
    }
  else if (IsGrayColorspace(image->colorspace))
    {
      samples_per_component=1;
      number_of_elements=1;
      if (image->matte)
        number_of_elements++;
    }
  else
    {
      if (image_info->interlace == PlaneInterlace)
        {
          samples_per_component=1;
          number_of_elements=3;
          if (image->matte)
            number_of_elements++;
        }
      else
        {
          samples_per_component=3;
          number_of_elements=1;
          if (image->matte)
            samples_per_component++;
        }
    }

  /*
    Choose the default packing method.
  */
  if ((bits_per_sample == 10) || (bits_per_sample == 12))
    packing_method=PackingMethodWordsFillLSB;
  else
    packing_method=PackingMethodPacked;

  /*
    Allow the user to over-ride the default packing method.
  */
  if ((definition_value=AccessDefinition(image_info,"dpx","packing-method")))
    {
      if (LocaleCompare(definition_value,"packed") == 0)
        {
          packing_method=PackingMethodPacked;
        }
      else if ((bits_per_sample == 10) || (bits_per_sample == 12))
        {
          if ((LocaleCompare(definition_value,"lsbpad") == 0) ||
              (LocaleCompare(definition_value,"a") == 0))
            packing_method=PackingMethodWordsFillLSB;
          else if ((LocaleCompare(definition_value,"msbpad") == 0) ||
                   (LocaleCompare(definition_value,"b") == 0))
            packing_method=PackingMethodWordsFillMSB;
        }
    }

  row_samples=((magick_int64_t) image->columns*samples_per_component);
  row_octets=DPXRowOctets(1,row_samples,bits_per_sample,packing_method);
  element_size=DPXRowOctets(image->rows,row_samples,bits_per_sample,packing_method);

  if (image->logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "Samples per row %u, octets per row %lu, element size %lu",
                          row_samples, (unsigned long) row_octets,
                          (unsigned long) element_size);
  /*
    Obtain pointer to user data and user data length (if available).
  */
  user_data=GetImageProfile(image,"DPXUSERDATA",&user_data_length);

  /*
    Image information header
  */
  (void) memset(&dpx_image_info,0,sizeof(dpx_image_info));
  /* Image orientation */
  dpx_image_info.orientation=OrientationTypeToDPXOrientation(image->orientation);
  /* Number of image elements described. */
  dpx_image_info.elements=number_of_elements;
  /* Number of pixels per line. */
  dpx_image_info.pixels_per_line=image->columns;
  /* Number of lines per image element. */
  dpx_image_info.lines_per_image_element=image->rows;
  /* Image sample sign. */
  dpx_image_info.element_info[0].data_sign=0; /* Unsigned data */

  /* Colorimetic specification. Define the appropriate color reference
     primaries (for additive color systems like television) or color
     responses (for printing density). */
  /* Reference low data code.  For printing density the default is 0
     but for ITU-R 601-5 luma, the default is 16 */ 
  /* Reference low quantity represented. For printing density the
     default is a density of 0.00. For ITU-R 601-5, the luma default
     is 0 mv */
  /* Reference high data code value. Defines maximum expected code
     value for image data. For 10-bit printing density, the default
     code value is 1023. */
  /* Reference high quantity represented. For printing density, the
     default is a density of 2.048. For ITU-R 601-5 luma, the default
     is 700 mv. */
  SET_UNDEFINED_U8(dpx_image_info.element_info[0].transfer_characteristic);
  SET_UNDEFINED_U8(dpx_image_info.element_info[0].colorimetric);
  SET_UNDEFINED_U8(dpx_image_info.element_info[0].reference_low_data_code);
  SET_UNDEFINED_R32(dpx_image_info.element_info[0].reference_low_quantity);
  SET_UNDEFINED_U32(dpx_image_info.element_info[0].reference_high_data_code);
  SET_UNDEFINED_R32(dpx_image_info.element_info[0].reference_high_quantity);

  if (image->colorspace == CineonLogRGBColorspace)
    {
      transfer_characteristic=TransferCharacteristicPrintingDensity;
    }
  else if ((image->colorspace == YCbCrColorspace) ||
           (image->colorspace == Rec601YCbCrColorspace) ||
           (image->colorspace == Rec601LumaColorspace))
    {
      if (image->rows > 525)
        transfer_characteristic=TransferCharacteristicITU_R601_625L;
      else
        transfer_characteristic=TransferCharacteristicITU_R601_525L;
    }
  else if ((image->colorspace == Rec709YCbCrColorspace) ||
           (image->colorspace == Rec709LumaColorspace))
    {
      transfer_characteristic=TransferCharacteristicITU_R709;
    }
  else
    {
      transfer_characteristic=TransferCharacteristicLinear;
    }

  /* Transfer characteristic. Define the amplitude transfer function
     necessary to transform the data to a linear original. */
  dpx_image_info.element_info[0].transfer_characteristic=transfer_characteristic;

  if (transfer_characteristic == TransferCharacteristicPrintingDensity)
    {
      /* Printing density is a log encoding */
      dpx_image_info.element_info[0].colorimetric=ColorimetricPrintingDensity;
      dpx_image_info.element_info[0].reference_low_data_code=0;
      dpx_image_info.element_info[0].reference_high_data_code=
        MaxValueGivenBits(bits_per_sample);
      dpx_image_info.element_info[0].reference_low_quantity=0.00F;
      dpx_image_info.element_info[0].reference_high_quantity=2.047F;
    }
  else if ((transfer_characteristic == TransferCharacteristicUnspecifiedVideo) ||
           (transfer_characteristic == TransferCharacteristicSMTPE274M) ||
           (transfer_characteristic == TransferCharacteristicITU_R709) ||
           (transfer_characteristic == TransferCharacteristicITU_R601_625L) ||
           (transfer_characteristic == TransferCharacteristicITU_R601_525L) ||
           (transfer_characteristic == TransferCharacteristicNTSCCompositeVideo) ||
           (transfer_characteristic == TransferCharacteristicPALCompositeVideo))
    {
      /*
        Some sort of video.
      */
      unsigned int
        max_value_given_bits = MaxValueGivenBits(bits_per_sample);

      switch (transfer_characteristic)
        {
        case TransferCharacteristicSMTPE274M:
          dpx_image_info.element_info[0].colorimetric=ColorimetricSMTPE274M;
          break;
        case TransferCharacteristicITU_R709:
          dpx_image_info.element_info[0].colorimetric=ColorimetricITU_R709;
          break;
        case TransferCharacteristicITU_R601_625L:
          dpx_image_info.element_info[0].colorimetric=ColorimetricITU_R601_625L;
          break;
        case TransferCharacteristicITU_R601_525L:
          dpx_image_info.element_info[0].colorimetric=ColorimetricITU_R601_525L;
          break;
        case TransferCharacteristicNTSCCompositeVideo:
          dpx_image_info.element_info[0].colorimetric=ColorimetricNTSCCompositeVideo;
          break;
        case TransferCharacteristicPALCompositeVideo:
          dpx_image_info.element_info[0].colorimetric=ColorimetricPALCompositeVideo;
          break;
        default:
          dpx_image_info.element_info[0].colorimetric=ColorimetricUserDefined;
          break;
        }

      dpx_image_info.element_info[0].reference_low_data_code=        /* 16 for 8 bits */
        (U32) (max_value_given_bits * (16.0/255.0) + 0.5);
      dpx_image_info.element_info[0].reference_high_data_code=       /* 235 for 8 bits */
        (U32) (max_value_given_bits * (235.0/255.0) + 0.5);
      dpx_image_info.element_info[0].reference_low_quantity=0.00F;   /* 0mv */
      dpx_image_info.element_info[0].reference_high_quantity=0.700F; /* 700mv */
    }
  else if (transfer_characteristic == TransferCharacteristicLinear)
    {
      /* Otherwise we are using linear encoding */
      dpx_image_info.element_info[0].colorimetric=ColorimetricLinear;
      dpx_image_info.element_info[0].reference_low_data_code=0;
      dpx_image_info.element_info[0].reference_high_data_code=
        MaxValueGivenBits(bits_per_sample);
    }

  /*
    Compute image data offset.  Should be rounded up to next 8K block.
  */
  image_data_offset=2048;
  if (user_data)
    image_data_offset += user_data_length;
  image_data_offset=RoundUpToBoundary(image_data_offset,IMAGE_DATA_ROUNDING);

  /* Element Descriptor */
  SET_UNDEFINED_U8(dpx_image_info.element_info[0].descriptor);
  /* Number of bits per sample */
  dpx_image_info.element_info[0].bits_per_sample=bits_per_sample;
  /* Packing method */
  dpx_image_info.element_info[0].packing=0;
  if ((bits_per_sample == 10) || (bits_per_sample == 12))
    dpx_image_info.element_info[0].packing=packing_method;
  /* Encoding.  Unencoded or run length encoded */
  dpx_image_info.element_info[0].encoding=0; /* No encoding */
  /* Offset to element data from beginning of file. */
  dpx_image_info.element_info[0].data_offset=image_data_offset;
  /* Number of padded bytes at the end of each line */
  dpx_image_info.element_info[0].eol_pad=0;
  /* Number of padded bytes at the end of image element. */
  dpx_image_info.element_info[0].eoi_pad=0;
  /* Element description */
  SET_UNDEFINED_ASCII(dpx_image_info.element_info[0].description);

  for (i=1; i < number_of_elements; i++)
    {
      /* Clone settings from preceding element */
      dpx_image_info.element_info[i]=dpx_image_info.element_info[i-1];
      /* Compute offset to data */
      dpx_image_info.element_info[i].data_offset=
        dpx_image_info.element_info[i-1].data_offset+element_size;
    }

  /*
    Fill out element descriptor.
  */
  if (number_of_elements > 1)
    {
      /*
        Planar image configuration.
      */
      if (IsGrayColorspace(image->colorspace))
        {
          /* Luma with alpha channel in second plane */
          dpx_image_info.element_info[0].descriptor=ImageElementLuma;
          dpx_image_info.element_info[1].descriptor=ImageElementAlpha;
        }
      else if (IsRGBColorspace(image->colorspace) ||
               (image->colorspace == CineonLogRGBColorspace))
        {
          /* RGB Planar */
          dpx_image_info.element_info[0].descriptor=ImageElementRed;
          dpx_image_info.element_info[1].descriptor=ImageElementGreen;
          dpx_image_info.element_info[2].descriptor=ImageElementBlue;
          if ((image->matte) && (number_of_elements == 4))
            {
              dpx_image_info.element_info[3].descriptor=ImageElementAlpha;
            }
        }
      else if (IsYCbCrColorspace(image->colorspace))
        {
          /* YCbCr 4:2:2 planar */
          dpx_image_info.element_info[0].descriptor=ImageElementLuma;
          dpx_image_info.element_info[1].descriptor=ImageElementColorDifferenceCbCr;
          if (image->matte)
            dpx_image_info.element_info[2].descriptor=ImageElementAlpha;
        }
    }
  else
    {
      if (IsYCbCrColorspace(image->colorspace))
        {
          /* CbYCr */
          if (samples_per_component == 2)
            {
              /* CbYCr 4:2:2 */
              dpx_image_info.element_info[0].descriptor=image->matte ?
                ImageElementCbYACrYA4224 : ImageElementCbYCrY422;
            }
          else
            {
              /* CbYCr 4:4:4 */
              dpx_image_info.element_info[0].descriptor=image->matte ?
                ImageElementCbYCrA4444 : ImageElementCbYCr444;
            }
        }
      else if (IsGrayColorspace(image->colorspace))
        {
          /* Luma */
          dpx_image_info.element_info[0].descriptor=ImageElementLuma;
        }
      else if (IsRGBColorspace(image->colorspace) ||
               (image->colorspace == CineonLogRGBColorspace))
        {
          /* RGB */
          dpx_image_info.element_info[0].descriptor=image->matte ?
            ImageElementRGBA : ImageElementRGB;
        }
    }

  /*
    Add a textual description for each element.
  */
  for (i=0; i < number_of_elements; i++)
    {
      (void) strlcpy(dpx_image_info.element_info[i].description,
                     DescribeImageElementDescriptor((DPXImageElementDescriptor) dpx_image_info.element_info[i].descriptor),
                     sizeof(dpx_image_info.element_info[0].description));
      (void) strlcat(dpx_image_info.element_info[i].description," / ",
                     sizeof(dpx_image_info.element_info[0].description));
      (void) strlcat(dpx_image_info.element_info[i].description,
                     DescribeImageTransferCharacteristic((DPXTransferCharacteristic) dpx_image_info.element_info[i].transfer_characteristic),
                     sizeof(dpx_image_info.element_info[0].description));
    }

  /*
    File information header.
  */
  (void) memset(&dpx_file_info,0,sizeof(dpx_file_info));
  dpx_file_info.magic=0x53445058U;
  dpx_file_info.image_data_offset=dpx_image_info.element_info[0].data_offset;
  (void) strlcpy(dpx_file_info.header_format_version,"V2.0",
                 sizeof(dpx_file_info.header_format_version));
  dpx_file_info.file_size=
    dpx_file_info.image_data_offset+number_of_elements*element_size;
  if (image->logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "Estimated file length: %u",dpx_file_info.file_size);
  dpx_file_info.ditto_key=1; /* new frame */
  dpx_file_info.generic_section_length=sizeof(dpx_file_info)+
    sizeof(dpx_image_info)+sizeof(dpx_source_info);
  dpx_file_info.industry_section_length=sizeof(dpx_mp_info)+sizeof(dpx_tv_info);
  dpx_file_info.user_defined_length=(user_data ? user_data_length : 0);
  (void) strlcpy(dpx_file_info.image_filename,image->filename,
                 sizeof(dpx_file_info.image_filename));
  GenerateDPXTimeStamp(dpx_file_info.creation_datetime,
                       sizeof(dpx_file_info.creation_datetime));
  (void) strlcpy(dpx_file_info.creator,GetMagickVersion((unsigned long *) NULL),
                 sizeof(dpx_file_info.creator));
  AttributeToString(image_info,image,"DPX:file.project.name",dpx_file_info.project_name);
  AttributeToString(image_info,image,"DPX:file.copyright",dpx_file_info.copyright);
  AttributeToU32(image_info,image,"DPX:file.encryption.key",dpx_file_info.encryption_key);
  /*
    Image source information header
  */
  (void) memset(&dpx_source_info,0,sizeof(dpx_source_info));
  SET_UNDEFINED_U32(dpx_source_info.x_offset);
  SET_UNDEFINED_U32(dpx_source_info.y_offset);
  SET_UNDEFINED_R32(dpx_source_info.x_center);
  SET_UNDEFINED_R32(dpx_source_info.y_center);
  SET_UNDEFINED_U32(dpx_source_info.x_original_size);
  SET_UNDEFINED_U32(dpx_source_info.y_original_size);
  if ((image->rows == image->magick_rows) && (image->columns == image->magick_columns))
    {
      /* If image size has not changed from original (magick_columns
         and magick_rows contain original size), then preserve any
         existing source image dimension and offset information. If
         size has changed, then information may be wrong. */
      AttributeToU32(image_info,image,"DPX:source.x-offset",dpx_source_info.x_offset);
      AttributeToU32(image_info,image,"DPX:source.y-offset",dpx_source_info.y_offset);
      AttributeToR32(image_info,image,"DPX:source.x-center",dpx_source_info.x_center);
      AttributeToR32(image_info,image,"DPX:source.y-center",dpx_source_info.y_center);
      AttributeToU32(image_info,image,"DPX:source.x-original-size",dpx_source_info.x_original_size);
      AttributeToU32(image_info,image,"DPX:source.y-original-size",dpx_source_info.y_original_size);
    }
  AttributeToString(image_info,image,"DPX:source.filename",dpx_source_info.source_image_filename);
  if (IS_UNDEFINED_ASCII(dpx_source_info.source_image_filename))
    (void) strlcpy(dpx_source_info.source_image_filename,image->magick_filename,
                   sizeof(dpx_source_info.source_image_filename));
  AttributeToString(image_info,image,"DPX:source.creation.datetime",dpx_source_info.source_image_datetime);
  AttributeToString(image_info,image,"DPX:source.device.name",dpx_source_info.input_device_name);
  AttributeToString(image_info,image,"DPX:source.device.serialnumber",dpx_source_info.input_device_serialnumber);
  AttributeToU16(image_info,image,"DPX:source.border.validity.left",dpx_source_info.border_validity.XL);
  AttributeToU16(image_info,image,"DPX:source.border.validity.right",dpx_source_info.border_validity.XR);
  AttributeToU16(image_info,image,"DPX:source.border.validity.top",dpx_source_info.border_validity.YT);
  AttributeToU16(image_info,image,"DPX:source.border.validity.bottom",dpx_source_info.border_validity.YB);
  AttributeToU32(image_info,image,"DPX:source.aspect.ratio.horizontal",dpx_source_info.aspect_ratio.horizontal);
  AttributeToU32(image_info,image,"DPX:source.aspect.ratio.vertical",dpx_source_info.aspect_ratio.vertical);
  AttributeToR32(image_info,image,"DPX:source.scanned.size.x",dpx_source_info.x_scanned_size);
  AttributeToR32(image_info,image,"DPX:source.scanned.size.y",dpx_source_info.y_scanned_size);
  /*
    Motion-picture film information header.
  */
  AttributeToString(image_info,image,"DPX:mp.film.manufacturer.id",dpx_mp_info.film_mfg_id_code);
  AttributeToString(image_info,image,"DPX:mp.film.type",dpx_mp_info.film_type);
  AttributeToString(image_info,image,"DPX:mp.perfs.offset",dpx_mp_info.perfs_offset);
  AttributeToString(image_info,image,"DPX:mp.prefix",dpx_mp_info.prefix);
  AttributeToString(image_info,image,"DPX:mp.count",dpx_mp_info.count);
  AttributeToString(image_info,image,"DPX:mp.format",dpx_mp_info.format);
  AttributeToU32(image_info,image,"DPX:mp.frame.position",dpx_mp_info.frame_position);
  AttributeToU32(image_info,image,"DPX:mp.sequence.length",dpx_mp_info.sequence_length);
  AttributeToU32(image_info,image,"DPX:mp.held.count",dpx_mp_info.held_count);
  AttributeToR32(image_info,image,"DPX:mp.frame.rate",dpx_mp_info.frame_rate);
  AttributeToR32(image_info,image,"DPX:mp.shutter.angle",dpx_mp_info.shutter_angle);
  AttributeToString(image_info,image,"DPX:mp.frame.id",dpx_mp_info.frame_id);
  AttributeToString(image_info,image,"DPX:mp.slate.info",dpx_mp_info.slate_info);
  /*
    Television information header.
  */
  AttributeBitsToU32(image_info,image,"DPX:tv.time.code",dpx_tv_info.time_code);
  AttributeBitsToU32(image_info,image,"DPX:tv.user.bits",dpx_tv_info.user_bits);
  AttributeToU8(image_info,image,"DPX:tv.interlace",dpx_tv_info.interlace);
  AttributeToU8(image_info,image,"DPX:tv.field.number",dpx_tv_info.field_number);
  AttributeToU8(image_info,image,"DPX:tv.video.signal",dpx_tv_info.video_signal);
  AttributeToR32(image_info,image,"DPX:tv.horizontal.sampling.rate",dpx_tv_info.horizontal_sample);
  AttributeToR32(image_info,image,"DPX:tv.temporal.sampling.rate",dpx_tv_info.temporal_sample);
  AttributeToR32(image_info,image,"DPX:tv.sync.time",dpx_tv_info.sync_time);
  AttributeToR32(image_info,image,"DPX:tv.gamma",dpx_tv_info.gamma);
  AttributeToR32(image_info,image,"DPX:tv.black.level",dpx_tv_info.black_level);
  AttributeToR32(image_info,image,"DPX:tv.black.gain",dpx_tv_info.black_gain);
  AttributeToR32(image_info,image,"DPX:tv.breakpoint",dpx_tv_info.breakpoint);
  AttributeToR32(image_info,image,"DPX:tv.white.level",dpx_tv_info.white_level);
  AttributeToR32(image_info,image,"DPX:tv.integration.time",dpx_tv_info.integration_time);
  /*
    Determine the maximum number of samples required for any element.
  */
  max_samples_per_pixel=0;
  for (element=0; element < dpx_image_info.elements; element++)
    {
      element_descriptor=(DPXImageElementDescriptor)
        dpx_image_info.element_info[element].descriptor;
      max_samples_per_pixel=Max(max_samples_per_pixel,
                                DPXSamplesPerPixel(element_descriptor));
    }
  /*
    Allocate row samples.
  */
  samples=MagickAllocateArray(sample_t *,image->columns,
                              max_samples_per_pixel*sizeof(sample_t));
  if (samples == (sample_t *) NULL)
    ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
  (void) memset((void *) samples,0,max_samples_per_pixel*image->columns*
                sizeof(sample_t));
  /*
    Allocate row scanline.
  */
  scanline=MagickAllocateMemory(unsigned char *,row_octets);
  if (scanline == (unsigned char *) NULL)
    {
      MagickFreeMemory(samples);
      ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
    }
  (void) memset((void *) scanline,0,row_octets);

  /*
    Allocate sample translation map storage.
  */
  map_Y=MagickAllocateArray(sample_t *,MaxMap+1,sizeof(sample_t));
  if (map_Y == (sample_t *) NULL)
    {
      MagickFreeMemory(samples);
      MagickFreeMemory(scanline);
      ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
    }
  (void) memset((void *) map_Y,0,(MaxMap+1)*sizeof(sample_t));

  map_CbCr=MagickAllocateArray(sample_t *,MaxMap+1,sizeof(sample_t));
  if (map_CbCr == (sample_t *) NULL)
    {
      MagickFreeMemory(samples);
      MagickFreeMemory(scanline);
      MagickFreeMemory(map_Y);
      ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
    }
  (void) memset((void *) map_CbCr,0,(MaxMap+1)*sizeof(sample_t));

  /*
    Reserve file/blob length if requested.
  */
  {
    const char
      *env;

    if (((env=getenv("MAGICK_RESERVE_STORAGE")) != NULL) &&
        (LocaleCompare(env,"TRUE") == 0))
      (void) BlobReserveSize(image,dpx_file_info.file_size);
  }

  /*
    Write file headers.
  */
  if (swab)
    {
      /*
        Swap byte order.
      */
      SwabDPXFileInfo(&dpx_file_info);
      SwabDPXImageInfo(&dpx_image_info);
      SwabDPXImageSourceInfo(&dpx_source_info);
      SwabDPXMPFilmInfo(&dpx_mp_info);
      SwabDPXTVInfo(&dpx_tv_info);
    }
  offset += WriteBlob(image,sizeof(dpx_file_info),&dpx_file_info);
  offset += WriteBlob(image,sizeof(dpx_image_info),&dpx_image_info);
  offset += WriteBlob(image,sizeof(dpx_source_info),&dpx_source_info);
  offset += WriteBlob(image,sizeof(dpx_mp_info),&dpx_mp_info);
  offset += WriteBlob(image,sizeof(dpx_tv_info),&dpx_tv_info);
  if (user_data)
    {
      offset += WriteBlob(image,(size_t) user_data_length,user_data);
    }
  if (swab)
    {
      /*
        Swap byte order back to original.
      */
      SwabDPXFileInfo(&dpx_file_info);
      SwabDPXImageInfo(&dpx_image_info);
      SwabDPXImageSourceInfo(&dpx_source_info);
      SwabDPXMPFilmInfo(&dpx_mp_info);
      SwabDPXTVInfo(&dpx_tv_info);
    }
  /*
    Fill to offset.
  */
  for( ; offset < dpx_image_info.element_info[0].data_offset; offset++)
    (void) WriteBlobByte(image,0x00);
  /*
    Allow user to over-ride pixel endianness.
  */
  if ((definition_value=AccessDefinition(image_info,"dpx","pixel-endian")))
    {
      if (LocaleCompare(definition_value,"msb") == 0)
        endian_type=MSBEndian;
      else if (LocaleCompare(definition_value,"lsb") == 0)
        endian_type=LSBEndian;
    }
  /*
    Write out elements.
  */
  for (element=0; element < dpx_image_info.elements; element++)
    {
      MagickBool
        swap_word_datums = MagickFalse;

      unsigned int
        max_value_given_bits = MaxValueGivenBits(bits_per_sample),
        reference_low = 0,
        reference_high = max_value_given_bits;

      if (BlobIsSeekable(image))
        {
          magick_off_t reported_file_offset = TellBlob(image);
          if ((magick_off_t) dpx_image_info.element_info[element].data_offset !=
              reported_file_offset)
            {
              (void) fprintf(stderr,"### Descriptor %u offset %u, TellBlob says %ld\n",
                             element+1, dpx_image_info.element_info[element].data_offset,
                             (long) reported_file_offset);
            }
        }
      DescribeDPXImageElement(&dpx_image_info.element_info[element],element+1);

      bits_per_sample=dpx_image_info.element_info[element].bits_per_sample;
      transfer_characteristic=(DPXTransferCharacteristic) dpx_image_info.element_info[element].transfer_characteristic;

      scale_from_short=1U;
      if (bits_per_sample < 16U)
        scale_from_short=(65535U / (65535U >> (16-bits_per_sample)));

      if ((transfer_characteristic == TransferCharacteristicITU_R709) ||
          (transfer_characteristic == TransferCharacteristicITU_R601_625L) ||
          (transfer_characteristic == TransferCharacteristicITU_R601_525L))
        {
          double
            ScaleY = 0.0,
            ScaleCbCr = 0.0;

          reference_low = (((double) MaxRGB+1)*(64.0/1024.0));
          reference_high = (((double) MaxRGB+1)*(940.0/1024.0));
          ScaleY = ((double) reference_high-reference_low)/((double) MaxRGB+1);
          ScaleCbCr = ScaleY*((960.0-64.0)/(940.0-64.0));

          for (i=0; i <= MaxMap ; i++)
            {
              map_Y[i]=ScaleQuantumToShort(ScaleToVideo(i,reference_low,ScaleY))/scale_from_short;
              map_CbCr[i]=ScaleQuantumToShort(ScaleToVideo(i,reference_low,ScaleCbCr))/scale_from_short;
            }
        }
      else
        {
          for (i=0; i <= MaxMap ; i++)
            map_Y[i]=ScaleQuantumToShort(i)/scale_from_short;
        }

      element_descriptor=(DPXImageElementDescriptor)
        dpx_image_info.element_info[element].descriptor;

      /*
        Determine component packing method.
      */
      packing_method=(ImageComponentPackingMethod) dpx_image_info.element_info[element].packing;
      samples_per_pixel=DPXSamplesPerPixel(element_descriptor);
      samples_per_row=samples_per_pixel*image->columns;

      /*
        Are datums returned in reverse order when extracted from a
        32-bit word?  This is to support Note 2 in Table 1 which
        describes how RGB/RGBA are returned in reversed order for the
        10-bit "filled" format.  Note 3 refers to Note 2 so presumably
        the same applies for ABGR.  The majority of YCbCr 4:2:2 files
        received have been swapped (but not YCbCr 4:4:4 for some
        reason) so swap the samples for YCbCr 4:2:2 as well.
      */
      if ((element_descriptor == ImageElementRGB) ||
          (element_descriptor == ImageElementRGBA) ||
          (element_descriptor == ImageElementABGR) ||
          (element_descriptor == ImageElementCbYCrY422) ||
          (element_descriptor == ImageElementCbYACrYA4224))
        {
          if ((bits_per_sample == 10) && (packing_method != PackingMethodPacked))
            swap_word_datums = MagickTrue;
        }
      if ((definition_value=AccessDefinition(image_info,"dpx","swap-samples")))
        {
          if (LocaleCompare(definition_value,"false") != 0)
            swap_word_datums = swap_word_datums ? MagickFalse : MagickTrue;
        }

      /*
        Create a chroma image if we are subsampling YCbCr.
      */
      if (((element_descriptor == ImageElementCbYCrY422) ||
           (element_descriptor == ImageElementCbYACrYA4224) ||
           (element_descriptor == ImageElementColorDifferenceCbCr)) &&
          (chroma_image == (Image *) NULL))
        {
          chroma_image=ResizeImage(image,image->columns/2,image->rows,
                                   LanczosFilter,1.0,&image->exception);
          if (chroma_image == (Image *) NULL)
            ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
        }

      for (y=0; y < image->rows; y++)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;

          samples_itr=samples;
          /*
            Prepare row samples.
          */
          switch (element_descriptor)
            {
            case ImageElementRed:
              for (x=image->columns; x != 0; x--)
                {
                  *samples_itr++=map_Y[ScaleQuantumToMap(GetRedSample(p))];
                  p++;
                }
              break;
            case ImageElementGreen:
              for (x=image->columns; x != 0; x--)
                {
                  *samples_itr++=map_Y[ScaleQuantumToMap(GetGreenSample(p))];
                  p++;
                }
              break;
            case ImageElementBlue:
              for (x=image->columns; x != 0; x--)
                {
                  *samples_itr++=map_Y[ScaleQuantumToMap(GetBlueSample(p))];
                  p++;
                }
              break;
            case ImageElementAlpha:
              {
                for (x=image->columns; x != 0; x--)
                  {
                    *samples_itr++=map_Y[ScaleQuantumToMap(GetOpacitySample(p))];
                    p++;
                  }
                break;
              }
            case ImageElementUnspecified:
            case ImageElementLuma:
              {
                if ((transfer_characteristic == TransferCharacteristicITU_R709) ||
                    (transfer_characteristic == TransferCharacteristicITU_R601_625L) ||
                    (transfer_characteristic == TransferCharacteristicITU_R601_525L))
                  {
                    /* Video luma */
                    for (x=image->columns; x != 0; x--)
                      {
                        *samples_itr++=map_Y[ScaleQuantumToMap(GetYSample(p))];
                        p++;
                      }
                  }
                else
                  {
                    /* Linear gray */
                    for (x=image->columns; x != 0; x--)
                      {
                        *samples_itr++=map_Y[ScaleQuantumToMap(GetGraySample(p))];
                        p++;
                      }
                  }
                break;
              }
            case ImageElementColorDifferenceCbCr:
              {
                /* CbCr */
                const PixelPacket
                  *chroma_pixels;
                
                chroma_pixels=AcquireImagePixels(chroma_image,0,y,chroma_image->columns,1,
                                                 &image->exception);
                if (chroma_pixels == (const PixelPacket *) NULL)
                  break;
                
                for (x=image->columns; x != 0; x -= 2)
                  {
                    *samples_itr++=map_CbCr[ScaleQuantumToMap(GetCbSample(chroma_pixels))]; /* Cb */
                    *samples_itr++=map_CbCr[ScaleQuantumToMap(GetCrSample(chroma_pixels))]; /* Cr */
                    chroma_pixels++;
                  }
                break;
              }
            case ImageElementRGB:
              for (x=image->columns; x != 0; x--)
                {
#if 0
                  /* BGR */
                  *samples_itr++=map_Y[ScaleQuantumToMap(GetBlueSample(p))];
                  *samples_itr++=map_Y[ScaleQuantumToMap(GetGreenSample(p))];
                  *samples_itr++=map_Y[ScaleQuantumToMap(GetRedSample(p))];
#else
                  *samples_itr++=map_Y[ScaleQuantumToMap(GetRedSample(p))];
                  *samples_itr++=map_Y[ScaleQuantumToMap(GetGreenSample(p))];
                  *samples_itr++=map_Y[ScaleQuantumToMap(GetBlueSample(p))];
#endif
                  p++;
                }
              break;
            case ImageElementRGBA:
              for (x=image->columns; x != 0; x--)
                {
#if 0
                  /* BGRA */
                  *samples_itr++=map_Y[ScaleQuantumToMap(GetBlueSample(p))];
                  *samples_itr++=map_Y[ScaleQuantumToMap(GetGreenSample(p))];
                  *samples_itr++=map_Y[ScaleQuantumToMap(GetRedSample(p))];
#else
                  *samples_itr++=map_Y[ScaleQuantumToMap(GetRedSample(p))];
                  *samples_itr++=map_Y[ScaleQuantumToMap(GetGreenSample(p))];
                  *samples_itr++=map_Y[ScaleQuantumToMap(GetBlueSample(p))];
#endif
                  *samples_itr++=map_Y[ScaleQuantumToMap(GetOpacitySample(p))];
                  p++;
                }
              break;
            case ImageElementCbYCrY422:
              {
                /* CbY | CrY | CbY | CrY ..., even number of columns required. */
                const PixelPacket
                  *chroma_pixels;

                chroma_pixels=AcquireImagePixels(chroma_image,0,y,chroma_image->columns,1,
                                                 &image->exception);
                if (chroma_pixels == (const PixelPacket *) NULL)
                  break;

                for (x=image->columns; x != 0; x -= 2)
                  {
                    *samples_itr++=map_CbCr[ScaleQuantumToMap(GetCbSample(chroma_pixels))]; /* Cb */
                    *samples_itr++=map_Y[ScaleQuantumToMap(GetYSample(p))];                 /* Y */
                    p++;
                    *samples_itr++=map_CbCr[ScaleQuantumToMap(GetCrSample(chroma_pixels))]; /* Cr */
                    *samples_itr++=map_Y[ScaleQuantumToMap(GetYSample(p))];                 /* Y */
                    p++;
                    chroma_pixels++;
                  }
                break;
              }
            case ImageElementCbYACrYA4224:
              {
                /* CbYA | CrYA | CbYA | CrYA ..., even number of columns required. */
                const PixelPacket
                  *chroma_pixels;

                chroma_pixels=AcquireImagePixels(chroma_image,0,y,chroma_image->columns,1,
                                                 &image->exception);
                if (chroma_pixels == (const PixelPacket *) NULL)
                  break;

                for (x=image->columns; x != 0; x -= 2)
                  {
                    *samples_itr++=map_CbCr[ScaleQuantumToMap(GetCbSample(chroma_pixels))]; /* Cb */
                    *samples_itr++=map_Y[ScaleQuantumToMap(GetYSample(p))];                 /* Y */
                    *samples_itr++=map_Y[ScaleQuantumToMap(GetOpacitySample(p))];           /* A */
                    p++;
                    *samples_itr++=map_CbCr[ScaleQuantumToMap(GetCrSample(chroma_pixels))]; /* Cr */
                    *samples_itr++=map_Y[ScaleQuantumToMap(GetYSample(p))];                 /* Y */
                    *samples_itr++=map_Y[ScaleQuantumToMap(GetOpacitySample(p))];           /* A */
                    p++;
                    chroma_pixels++;
                  }
                break;
              }
            case ImageElementCbYCr444:
              for (x=image->columns; x != 0; x--)
                {
                  *samples_itr++=map_CbCr[ScaleQuantumToMap(GetCbSample(p))]; /* Cb */
                  *samples_itr++=map_Y[ScaleQuantumToMap(GetYSample(p))];     /* Y */
                  *samples_itr++=map_CbCr[ScaleQuantumToMap(GetCrSample(p))]; /* Cr */
                  p++;
                }
              break;
            case ImageElementCbYCrA4444:
              for (x=image->columns; x != 0; x--)
                {
                  *samples_itr++=map_CbCr[ScaleQuantumToMap(GetCbSample(p))];   /* Cb */
                  *samples_itr++=map_Y[ScaleQuantumToMap(GetYSample(p))];       /* Y */
                  *samples_itr++=map_CbCr[ScaleQuantumToMap(GetCrSample(p))];   /* Cr */
                  *samples_itr++=map_Y[ScaleQuantumToMap(GetOpacitySample(p))]; /* A */
                  p++;
                }
              break;

            default:
              break;
            }

#if 0
          /*
            Scale samples.
          */
          samples_itr=samples;
          if (bits_per_sample == 1)
            {
              for (i=samples_per_row; i != 0; i--)
                {
                  *samples_itr=(*samples_itr > MaxRGB/2) ? 1 : 0;
                  samples_itr++;
                }
            }
          else
            {
              for (i=samples_per_row; i != 0; i--)
                {
                  *samples_itr=ScaleQuantumToShort(*samples_itr)/scale_from_short;
                  samples_itr++;
                }
            }
#endif

          /*
            FIXME: RLE samples.
          */

          /*
            Output samples.
          */
          WriteRowSamples(samples, samples_per_row, bits_per_sample,
                          packing_method,endian_type,swap_word_datums,scanline);
          if (WriteBlob(image,row_octets,(void *) scanline) != row_octets)
            {
              status=MagickFail;
              break;
            }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitorFormatted(y,image->rows,&image->exception,
                                          SaveImageText,image->filename))
                break;
        }
    }
  
  if (BlobIsSeekable(image))
    {
      magick_off_t reported_file_offset = TellBlob(image);
      if ((magick_off_t) dpx_file_info.file_size != reported_file_offset)
        {
          (void) fprintf(stderr,"### File length %u, TellBlob says %ld\n",
                         dpx_file_info.file_size,
                         (long) reported_file_offset);
        }
    }
  
  MagickFreeMemory(map_CbCr);
  MagickFreeMemory(map_Y);
  MagickFreeMemory(samples);
  MagickFreeMemory(scanline);
  CloseBlob(image);  
  return(status);
}
