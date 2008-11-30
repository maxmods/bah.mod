/*
% Copyright (C) 2003, 2007 GraphicsMagick Group
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                   CCCC  IIIII  N   N  EEEEE  OOO   N   N                    %
%                  C        I    NN  N  E     O   O  NN  N                    %
%                  C        I    N N N  EEEE  O   O  N N N                    %
%                  C        I    N  NN  E     O   O  N  NN                    %
%                   CCCC  IIIII  N   N  EEEEE  OOO   N   N                    %
%                                                                             %
%                                                                             %
%                    Read/Write Kodak Cineon Image Format.                    %
%                 Cineon Image Format is similar to SMTPE DPX                 %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                              Bob Friesenhahn                                %
%                                 May 2007                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Cineon image file format draft is available at
%  http://www.cineon.com/ff_draft.php.
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/attribute.h"
#include "magick/blob.h"
#include "magick/bit_stream.h"
#include "magick/colorspace.h"
#include "magick/enum_strings.h"
#include "magick/pixel_cache.h"
#include "magick/magick_endian.h"
#include "magick/error.h"
#include "magick/list.h"
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/profile.h"
#include "magick/utility.h"

/*
  Forward declaractions.
*/
static unsigned int
  WriteCINEONImage(const ImageInfo *,Image *);

typedef char ASCII;
typedef magick_uint8_t U8;
typedef magick_uint32_t U32;
typedef magick_int32_t S32;
typedef float R32;

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
#define SET_UNDEFINED_U32(value) (value=0xFFFFFFFFU)
#define SET_UNDEFINED_S32(value) (value=0x80000000)

#define SET_UNDEFINED_R32(value) (((R32_u*) &value)->u=0x7F800000U);
#define SET_UNDEFINED_ASCII(value) ((void) memset(value,0,sizeof(value)))

#define IS_UNDEFINED_U8(value) (value == ((U8) 0xFFU))
#define IS_UNDEFINED_U32(value) (value == ((U32) 0xFFFFFFFFU))
#define IS_UNDEFINED_S32(value) (value == ((S32) 0x80000000))
#define IS_UNDEFINED_R32(value) (((R32_u*) &value)->u == ((U32) 0x7F800000U))
#define IS_UNDEFINED_ASCII(value) (!(value[0] > 0))

typedef struct _CineonFileInfo
{
  U32   magic;                     /* Magick number (0x802A5FD7) */
  U32   image_data_offset;         /* Offset to image data in bytes */
  U32   generic_section_length;    /* Generic section header length in bytes */
  U32   industry_section_length;   /* Industry specific header length in bytes */
  U32   user_defined_length;       /* User defined header length in bytes */
  U32   file_size;                 /* Total image file size in bytes  */
  ASCII header_format_version[8];  /* Version number of header format */
  ASCII image_filename[100];       /* Image filename */
  ASCII creation_date[12];         /* Creation date: yyyy:mm:dd */
  ASCII creation_time[12];         /* Creation time: hh:mm:ssLTZ */
  ASCII reserved[36];              /* Reserved for future use */
} CineonFileInfo;

typedef struct _CineonImageChannel
{
  U8    designator_byte_0;         /* 0 = universal metric */
  U8    designator_byte_1;         /* */
  U8    bits_per_sample;           /* Bit depth */
  U8    unused;
  U32   pixels_per_line;           /* Pixels per line (columns) */
  U32   lines_per_image;           /* Lines per image (rows) */
  R32   reference_low_data_code;   /* Reference low data code value */
  R32   reference_low_quantity;    /* Low quantity represented */
  R32   reference_high_data_code;  /* Reference high data code value */
  R32   reference_high_quantity;   /* Reference high quantity represented */
} DPXImageChannel;

typedef struct _CineonImageInfo
{
  U8    orientation;               /* Image orientation */
  U8    channels;                  /* Number of image channels (1-8) */
  U8    pad[2];                    /* Unused (2 byte space for word allignment */
  DPXImageChannel channel_info[8]; /* Description of channels */
  R32   white_point[2];            /* White point (color temperature) - x,y pair */
  R32   red_primary_chromaticity[2]; /* Red primary chromaticity - x,y pair */
  R32   green_primary_chromaticity[2]; /* Green primary chromaticity - x,y pair */
  R32   blue_primary_chromaticity[2]; /* Blue primary chromaticity - x,y pair */
  ASCII label_text[200];           /* Label text */
  ASCII reserved[28];              /* Reserved for future use */
  /* Image Data Format Information */
  U8    data_interleave;           /* Data interleave */
  U8    packing;                   /* Packing method */
  U8    sign;                      /* Data sign: 0=unsigned, 1=signed */
  U8    sense;                     /* Image sense: 0=positive, 1=negative */
  U32   eol_pad;                   /* End of line padding */
  U32   eoc_pad;                   /* End of channel padding */
  ASCII reserved2[20];             /* Reserved for future use */  
} CineonImageInfo;

typedef struct _CineonImageOriginationInfo
{
  S32   x_offset;                  /* X offset */
  S32   y_offset;                  /* Y offset */
  ASCII source_image_filename[100];/* Source image filename */
  ASCII creation_date[12];         /* Creation date: yyyy:mm:dd */
  ASCII creation_time[12];         /* Creation time: hh:mm:ssLTZ */
  ASCII input_device[64];          /* Input device */
  ASCII input_device_model[32];    /* Input device model number */
  ASCII input_device_serial[32];   /* Input device serial number */
  R32   input_device_pitch_x;      /* Input device pitch for X (samples/mm) */
  R32   input_device_pitch_y;      /* Input device pitch for Y (samples/mm) */
  R32   gamma;                     /* Image gamma of capture device */
  ASCII reserved[40];              /* Reserved for future use */
} CineonImageOriginationInfo;

typedef struct _CineonFilmInfo
{
  U8    film_mfg_id_code;          /* Film mfg. ID code (2 digits from film edge code) */
  U8    film_type;                 /* Film type (2 digits from film edge code) */
  U8    perfs_offset;              /* Offset in perfs (2 digits from film edge code) */
  U8    unused;                    /* Unused, for word allignment */
  U32   prefix;                    /* Prefix (6 digits from film edge code) */
  U32   count;                     /* Count (4 digits from film edge code) */
  ASCII format[32];                /* Format -- e.g. Academy */
  U32   frame_position;            /* Frame position in sequence */
  R32   frame_rate;                /* Frame rate of original (frames/s) */
  ASCII frame_id[32];              /* Frame identification - e.g. keyframe */
  ASCII slate_info[200];           /* Slate information */
  ASCII reserved[740];             /* Reserved for future use */
} CineonFilmInfo;


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s C I N E O N                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IsCINEON() returns True if the image format type, identified by the magick
%  string, is CINEON.
%
%  The format of the IsCINEON method is:
%
%      unsigned int IsCINEON(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsCINEON(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(False);
  if (memcmp(magick,"\200\052\137\327",4) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d C I N E O N I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReadCINEONImage() reads an CINEON X image file and returns it.  It allocates
%  the memory necessary for the new Image structure and returns a point to the
%  new image.
%
%  The format of the ReadCINEONImage method is:
%
%      Image *ReadCINEONImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
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
    buffer[MaxTextExtent]; \
\
  if (!IS_UNDEFINED_ASCII(member)) \
    { \
      (void) strlcpy(buffer,member,Min(sizeof(member)+1,MaxTextExtent)); \
      (void) SetImageAttribute(image,name,buffer); \
      LogSetImageAttribute(name,buffer); \
    } \
}
#define U8ToAttribute(image,name,member) \
{ \
  char \
    buffer[MaxTextExtent]; \
\
  if (!IS_UNDEFINED_U8(member)) \
    { \
      FormatString(buffer,"%u",(unsigned int) member); \
      (void) SetImageAttribute(image,name,buffer); \
      LogSetImageAttribute(name,buffer); \
    } \
}
#define U32ToAttribute(image,name,member) \
{ \
  char \
    buffer[MaxTextExtent]; \
\
  if (!IS_UNDEFINED_U32(member)) \
    { \
      FormatString(buffer,"%u",member); \
      (void) SetImageAttribute(image,name,buffer); \
      LogSetImageAttribute(name,buffer); \
    } \
}
#define R32ToAttribute(image,name,member) \
{ \
  char \
    buffer[MaxTextExtent]; \
\
  if (!IS_UNDEFINED_R32(member)) \
    { \
      FormatString(buffer,"%g",member); \
      (void) SetImageAttribute(image,name,buffer); \
      LogSetImageAttribute(name,buffer); \
    } \
}
#define S32ToAttribute(image,name,member) \
{ \
  char \
    buffer[MaxTextExtent]; \
\
  if (!IS_UNDEFINED_S32(member)) \
    { \
      FormatString(buffer,"%d",member); \
      (void) SetImageAttribute(image,name,buffer); \
      LogSetImageAttribute(name,buffer); \
    } \
}
static void SwabCineonFileInfo(CineonFileInfo *file_info)
{
  MagickSwabUInt32(&file_info->magic);
  MagickSwabUInt32(&file_info->image_data_offset);
  MagickSwabUInt32(&file_info->generic_section_length);
  MagickSwabUInt32(&file_info->industry_section_length);
  MagickSwabUInt32(&file_info->user_defined_length);
  MagickSwabUInt32(&file_info->file_size);
}

static void SwabCineonImageInfo(CineonImageInfo *image_info)
{
  unsigned int
    i;

  for (i=0 ; i < (unsigned int) image_info->channels  ; i++)
    {
      MagickSwabUInt32(&image_info->channel_info[i].pixels_per_line);
      MagickSwabUInt32(&image_info->channel_info[i].lines_per_image);
      MagickSwabFloat(&image_info->channel_info[i].reference_low_data_code);
      MagickSwabFloat(&image_info->channel_info[i].reference_low_quantity);
      MagickSwabFloat(&image_info->channel_info[i].reference_high_data_code);
      MagickSwabFloat(&image_info->channel_info[i].reference_high_quantity);
    }

  MagickSwabFloat(&image_info->white_point[0]);
  MagickSwabFloat(&image_info->white_point[1]);
  MagickSwabFloat(&image_info->red_primary_chromaticity[0]);
  MagickSwabFloat(&image_info->red_primary_chromaticity[1]);
  MagickSwabFloat(&image_info->green_primary_chromaticity[0]);
  MagickSwabFloat(&image_info->green_primary_chromaticity[1]);
  MagickSwabFloat(&image_info->blue_primary_chromaticity[0]);
  MagickSwabFloat(&image_info->blue_primary_chromaticity[1]);
  MagickSwabUInt32(&image_info->eol_pad);
  MagickSwabUInt32(&image_info->eoc_pad);  
}

static void SwabCineonImageOriginationInfo(CineonImageOriginationInfo *image_info)
{
  MagickSwabUInt32((U32 *) &image_info->x_offset);
  MagickSwabUInt32((U32 *) &image_info->y_offset);
  MagickSwabFloat(&image_info->input_device_pitch_x);
  MagickSwabFloat(&image_info->input_device_pitch_y);
  MagickSwabFloat(&image_info->gamma);
}

static void SwabCineonFilmInfo(CineonFilmInfo *mp_info)
{
  MagickSwabUInt32(&mp_info->prefix);
  MagickSwabUInt32(&mp_info->count);
  MagickSwabUInt32(&mp_info->frame_position);
  MagickSwabFloat(&mp_info->frame_rate);
}

static Image *ReadCINEONImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  CineonFileInfo
    cin_file_info;

  CineonImageInfo
    cin_image_info;

  CineonImageOriginationInfo
    cin_source_info;

  CineonFilmInfo
    cin_mp_info;

  Image
    *image;

  long
    y;

  register long
    x;

  register PixelPacket
    *q;

  register long
    i;

  size_t
    offset;    

  unsigned int
    channel,
    max_bits_per_sample,
    max_lines_per_image,
    max_pixels_per_line,
    number_of_channels;    

  MagickPassFail
    status;

  MagickBool
    swab=MagickFalse;

  unsigned long
    pixels_offset;

  unsigned char
    *scandata;
  
  void
    *scanline;

  const char *
    definition_value;

  BitStreamReadHandle
    bit_stream;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
#if !defined(WORDS_BIGENDIAN)
  swab=MagickTrue;
#endif /* !defined(WORDS_BIGENDIAN) */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    {
      DestroyImageList(image);
      return((Image *) NULL);
    }
  /*
    Read CINEON image.
  */
  offset=(ReadBlob(image,sizeof(cin_file_info),&cin_file_info));
  if (offset != sizeof(cin_file_info))
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);

  if (swab)
    SwabCineonFileInfo(&cin_file_info);

  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "File magic 0x%04X",cin_file_info.magic);

  if (cin_file_info.magic != 0x802A5FD7U)
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);

  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "Image offset %u, Generic length %u, Industry length %u, User length %u, File size %u",
                        cin_file_info.image_data_offset,
                        cin_file_info.generic_section_length,
                        cin_file_info.industry_section_length,
                        cin_file_info.user_defined_length,
                        cin_file_info.file_size);
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "Header format version \"%s\"", cin_file_info.header_format_version);
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "Image file name \"%s\"", cin_file_info.image_filename);
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "Creation date \"%s\"", cin_file_info.creation_date);
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "Creation time \"%s\"", cin_file_info.creation_time);

  StringToAttribute(image,"document",cin_file_info.image_filename);
  StringToAttribute(image,"DPX:file.filename",cin_file_info.image_filename);
  {
    char
      creation_datetime[24];

    creation_datetime[0]='\0';
    (void) strlcat(creation_datetime,cin_file_info.creation_date,sizeof(cin_file_info.creation_date)+1);
    if (creation_datetime[0]!='\0')
      (void) strlcat(creation_datetime,":",sizeof(creation_datetime));
    (void) strlcat(creation_datetime,cin_file_info.creation_time,sizeof(creation_datetime));
    StringToAttribute(image,"timestamp",creation_datetime);
    StringToAttribute(image,"DPX:file.creation.datetime",creation_datetime);
  }

  /*
    Obtain offset to pixels.
  */
  pixels_offset=cin_file_info.image_data_offset;
  if (pixels_offset < 712)
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);

  /*
    Read image information header.
  */
  offset += ReadBlob(image,sizeof(cin_image_info),&cin_image_info);
  if (offset != (size_t) 712L)
    ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
  if (swab)
    SwabCineonImageInfo(&cin_image_info);
  number_of_channels=cin_image_info.channels;
  U8ToAttribute(image,"DPX:image.orientation",cin_image_info.orientation);
  max_bits_per_sample=0;
  max_pixels_per_line=0;
  max_lines_per_image=0;
  for (channel=0; channel < number_of_channels; channel++)
    {
      max_bits_per_sample=Max(max_bits_per_sample,
                              cin_image_info.channel_info[channel].bits_per_sample);
      max_pixels_per_line=Max(max_pixels_per_line,
                              cin_image_info.channel_info[channel].pixels_per_line);
      max_lines_per_image=Max(max_lines_per_image,
                              cin_image_info.channel_info[channel].lines_per_image);
    }
  
  image->depth=max_bits_per_sample;
  image->columns=max_pixels_per_line;
  image->rows=max_lines_per_image;
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "Columns %ld, Rows %ld, Channels %u",
                        image->columns, image->rows,
                        (unsigned int) cin_image_info.channels);
  if (!IS_UNDEFINED_R32(cin_image_info.white_point[0]))
    image->chromaticity.white_point.x = cin_image_info.white_point[0];
  if (!IS_UNDEFINED_R32(cin_image_info.white_point[1]))
    image->chromaticity.white_point.y = cin_image_info.white_point[1];
  if (!IS_UNDEFINED_R32(cin_image_info.red_primary_chromaticity[0]))
    image->chromaticity.red_primary.x = cin_image_info.red_primary_chromaticity[0];
  if (!IS_UNDEFINED_R32(cin_image_info.red_primary_chromaticity[1]))
    image->chromaticity.red_primary.y = cin_image_info.red_primary_chromaticity[1];
  if (!IS_UNDEFINED_R32(cin_image_info.green_primary_chromaticity[0]))
    image->chromaticity.green_primary.x = cin_image_info.green_primary_chromaticity[0];
  if (!IS_UNDEFINED_R32(cin_image_info.green_primary_chromaticity[1]))
    image->chromaticity.green_primary.y = cin_image_info.green_primary_chromaticity[1];
  if (!IS_UNDEFINED_R32(cin_image_info.blue_primary_chromaticity[0]))
    image->chromaticity.blue_primary.x = cin_image_info.blue_primary_chromaticity[0];
  if (!IS_UNDEFINED_R32(cin_image_info.blue_primary_chromaticity[1]))
    image->chromaticity.blue_primary.y = cin_image_info.blue_primary_chromaticity[1];
  StringToAttribute(image,"DPX:file.project.name",cin_image_info.label_text);

  /*
    Read image origination header.
  */
  offset += ReadBlob(image,sizeof(cin_source_info),&cin_source_info);
  if (offset != (size_t) 1024L)
    ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
  if (swab)
    SwabCineonImageOriginationInfo(&cin_source_info);
  S32ToAttribute(image,"DPX:source.x-offset",cin_source_info.x_offset);
  S32ToAttribute(image,"DPX:source.y-offset",cin_source_info.y_offset);
  StringToAttribute(image,"DPX:source.filename",cin_source_info.source_image_filename);
  {
    char
      source_creation_datetime[MaxTextExtent];

    source_creation_datetime[0]='\0';
    (void) strlcat(source_creation_datetime,cin_file_info.creation_date,sizeof(cin_file_info.creation_date)+1);
    if (source_creation_datetime[0]!='\0')
      (void) strlcat(source_creation_datetime,":",sizeof(source_creation_datetime));
    (void) strlcat(source_creation_datetime,cin_file_info.creation_time,sizeof(source_creation_datetime));
    StringToAttribute(image,"DPX:source.creation.datetime",source_creation_datetime);
  }
  StringToAttribute(image,"DPX:source.device.name",cin_source_info.input_device);
  StringToAttribute(image,"DPX:source.device.model",cin_source_info.input_device_model);
  StringToAttribute(image,"DPX:source.device.serialnumber",cin_source_info.input_device_serial);
  R32ToAttribute(image,"DPX:source.device.pitch.x",cin_source_info.input_device_pitch_x);
  R32ToAttribute(image,"DPX:source.device.pitch.y",cin_source_info.input_device_pitch_y);
  R32ToAttribute(image,"DPX:source.device.gamma",cin_source_info.gamma);

  if ((pixels_offset >= 1024) && (cin_file_info.industry_section_length > 0))
    {
      /*
        Read Motion-picture film information header.
      */
      offset += ReadBlob(image,sizeof(cin_mp_info),&cin_mp_info);
      if (offset != (size_t) 2048L)
        ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
      if (swab)
        SwabCineonFilmInfo(&cin_mp_info);
      U8ToAttribute(image,"DPX:mp.film.manufacturer.id",cin_mp_info.film_mfg_id_code);
      U8ToAttribute(image,"DPX:mp.film.type",cin_mp_info.film_type);
      U8ToAttribute(image,"DPX:mp.perfs.offset",cin_mp_info.perfs_offset);
      U32ToAttribute(image,"DPX:mp.prefix",cin_mp_info.prefix);
      U32ToAttribute(image,"DPX:mp.count",cin_mp_info.count);
      StringToAttribute(image,"DPX:mp.format",cin_mp_info.format);
      U32ToAttribute(image,"DPX:mp.frame.position",cin_mp_info.frame_position);
      R32ToAttribute(image,"DPX:mp.frame.rate",cin_mp_info.frame_rate);
      StringToAttribute(image,"DPX:mp.frame.id",cin_mp_info.frame_id);
      StringToAttribute(image,"DPX:mp.slate.info",cin_mp_info.slate_info);
    }

  if ((pixels_offset >= 2048) && (cin_file_info.user_defined_length > 0))
    {
      unsigned char
        *user_data;
      
      const size_t
        block_size = 65536UL;
      
      size_t
        read_size,
        user_data_length;
      
      user_data_length=0UL;
      user_data=(unsigned char *) NULL;
      while (user_data_length < cin_file_info.user_defined_length)
        {
          read_size=Min(block_size,cin_file_info.user_defined_length-user_data_length);
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
      if (!SetImageProfile(image,"CINEONUSERDATA",user_data,user_data_length))
        {
          MagickFreeMemory(user_data);
          ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
        }
      MagickFreeMemory(user_data);
    }

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

  if (image->logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "Reading Cineon pixels starting at offset %ld",(long) TellBlob(image));

  /*
    Convert CINEON raster image to pixel packets.
  */
  {
    size_t
      scandata_bytes;

    unsigned int
      bits_per_sample,
      scale_to_short=0;

    /* Assume that all channels are the same depth */
    bits_per_sample = max_bits_per_sample;

    switch (number_of_channels)
      {
      case 1:
        {
          scandata_bytes=4;
          scale_to_short=64;
          scandata=MagickAllocateMemory(unsigned char *,scandata_bytes);
          scanline=scandata;
          BitStreamInitializeRead(&bit_stream,scanline);
          for (y=0; y < (long) image->rows; y++)
            {
              q=SetImagePixels(image,0,y,image->columns,1);
              if (q == (PixelPacket *) NULL)
                break;
              /*
                Packed 10 bit samples with 2 bit pad at end of 32-bit word.
              */
              scanline=scandata;
              i=3;
              for (x=(long) image->columns; x > 0; x--, i++)
                {
                  if (i > 2)
                    {
                      scanline=scandata;
                      if (ReadBlobZC(image,scandata_bytes,&scanline) !=
                          scandata_bytes)
                        break;
                      BitStreamInitializeRead(&bit_stream,scanline);
                      i=0;
                    }
                  q->red=q->green=q->blue=
                    ScaleShortToQuantum(BitStreamMSBRead(&bit_stream,10)*scale_to_short);
                  q->opacity=0U;
                  q++;
                }
              if (!SyncImagePixels(image))
                break;
              if (image->previous == (Image *) NULL)
                if (QuantumTick(y,image->rows))
                  if (!MagickMonitorFormatted(y,image->rows,exception,
                                              LoadImageText,image->filename))
                    break;
            }
          MagickFreeMemory(scandata);
          break;
        }
      case 3:
        {
          scandata_bytes=image->columns*4;
          scale_to_short=64;
          scandata=MagickAllocateMemory(unsigned char *,scandata_bytes);
          for (y=0; y < (long) image->rows; y++)
            {
              magick_uint32_t red;
              magick_uint32_t green;
              magick_uint32_t blue;

              q=SetImagePixels(image,0,y,image->columns,1);
              if (q == (PixelPacket *) NULL)
                break;
              scanline=scandata;
              if (ReadBlobZC(image,scandata_bytes,&scanline) != scandata_bytes)
                break;
              BitStreamInitializeRead(&bit_stream,scanline);
              for (x=0 ; x < (long) image->columns; x++)
                {
                  /*
                    Packed 10 bit samples with 2 bit pad at end of 32-bit word.
                  */
                  red   = BitStreamMSBRead(&bit_stream,10);
                  green = BitStreamMSBRead(&bit_stream,10);
                  blue  = BitStreamMSBRead(&bit_stream,10);
                  (void) BitStreamMSBRead(&bit_stream,2);

                  q->red     = ScaleShortToQuantum(red*scale_to_short);
                  q->green   = ScaleShortToQuantum(green*scale_to_short);
                  q->blue    = ScaleShortToQuantum(blue*scale_to_short);
                  q->opacity = 0U;

/*                   printf("i:%u,%u,%u --> %u,%u,%u\n", red, green, blue, */
/*                          (unsigned int)q->red, (unsigned int)q->green, (unsigned int)q->blue); */
                  q++;
                }
              if (!SyncImagePixels(image))
                break;
              if (image->previous == (Image *) NULL)
                if (QuantumTick(y,image->rows))
                  if (!MagickMonitorFormatted(y,image->rows,exception,
                                              LoadImageText,image->filename))
                    break;
            }
          MagickFreeMemory(scandata);
          break;
        }
      default:
        ThrowReaderException(CorruptImageError,ImageTypeNotSupported,image);
      }
  }
  image->depth=Min(image->depth,QuantumDepth);
  image->colorspace=CineonLogRGBColorspace;

  if ((definition_value=AccessDefinition(image_info,"cineon","colorspace")))
    {
      ColorspaceType
        colorspace;

      colorspace=StringToColorspaceType(definition_value);
      if (colorspace != UndefinedColorspace)
        {
          image->colorspace=colorspace;
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Explicitly set colorspace to %s",
                                ColorspaceTypeToString(image->colorspace));
        }
      else
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Unrecognized source colorspace \"%s\"\n",
                                definition_value);
          ThrowException(&image->exception,OptionError,UnrecognizedColorspace,
                         definition_value);
        }
    }

  if (EOFBlob(image))
    ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,image->filename);
  CloseBlob(image);
  return(GetFirstImageInList(image));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r C I N E O N I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterCINEONImage adds attributes for the CINEON image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterCINEONImage method is:
%
%      RegisterCINEONImage(void)
%
*/
ModuleExport void RegisterCINEONImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("CIN");
  entry->decoder=(DecoderHandler) ReadCINEONImage;
  entry->encoder=(EncoderHandler) WriteCINEONImage;
  entry->magick=(MagickHandler) IsCINEON;
  entry->description="Cineon Image File";
  entry->module="CINEON";
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r C I N E O N I m a g e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterCINEONImage removes format registrations made by the
%  CINEON module from the list of supported formats.
%
%  The format of the UnregisterCINEONImage method is:
%
%      UnregisterCINEONImage(void)
%
*/
ModuleExport void UnregisterCINEONImage(void)
{
  (void) UnregisterMagickInfo("CIN");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e C I N E O N I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  WriteCINEONImage() writes an image in CINEON encoded image format.
%
%  The format of the WriteCINEONImage method is:
%
%      unsigned int WriteCINEONImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o image_info: The image info.
%
%    o image:  The image.
%
%
*/

#define AttributeToU8(image_info,image,key,member) \
{ \
  const ImageAttribute \
    *attribute; \
\
  const char \
    *definition_value; \
\
  if ((definition_value=AccessDefinition(image_info,"dpx",key+4))) \
    member=(U8) strtol(definition_value, (char **) NULL, 10); \
  else if ((attribute=GetImageAttribute(image,key))) \
    member=(U8) strtol(attribute->value, (char **) NULL, 10); \
  else \
    SET_UNDEFINED_U8(member); \
}

#define AttributeToU32(image_info,image,key,member) \
{ \
  const ImageAttribute \
    *attribute; \
\
  const char \
    *definition_value; \
\
  if ((definition_value=AccessDefinition(image_info,"dpx",key+4))) \
    member=(U32) strtol(definition_value, (char **) NULL, 10); \
  else if ((attribute=GetImageAttribute(image,key))) \
    member=(U32) strtol(attribute->value, (char **) NULL, 10); \
  else \
    SET_UNDEFINED_U32(member); \
}

#define AttributeToS32(image_info,image,key,member) \
{ \
  const ImageAttribute \
    *attribute; \
\
  const char \
    *definition_value; \
\
  if ((definition_value=AccessDefinition(image_info,"dpx",key+4))) \
    member=(S32) strtol(definition_value, (char **) NULL, 10); \
  else if ((attribute=GetImageAttribute(image,key))) \
    member=(S32) strtol(attribute->value, (char **) NULL, 10); \
  else \
    SET_UNDEFINED_S32(member); \
}

#define AttributeToR32(image_info,image,key,member) \
{ \
  const ImageAttribute \
    *attribute; \
\
  const char \
    *definition_value; \
\
  if ((definition_value=AccessDefinition(image_info,"dpx",key+4))) \
    member=(R32) strtod(definition_value, (char **) NULL); \
  else if ((attribute=GetImageAttribute(image,key))) \
    member=(R32) strtod(attribute->value, (char **) NULL); \
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
    *attribute; \
\
  const char \
    *definition_value; \
\
  if ((definition_value=AccessDefinition(image_info,"dpx",key+4))) \
    (void) strncpy(member,definition_value,sizeof(member)); \
  else if ((attribute=GetImageAttribute(image,key))) \
    (void) strncpy(member,attribute->value,sizeof(member)); \
  else \
    SET_UNDEFINED_ASCII(member); \
}

static void GenerateCineonTimeStamp(char *date_str, size_t date_str_length, char*time_str, size_t time_str_length)
{
  char timestamp[MaxTextExtent];

  time_t
    current_time;

  const struct tm
    *t;

  current_time=time((time_t *) NULL);
  t=localtime(&current_time);

  (void) memset(timestamp,0,sizeof(timestamp));
  (void) strftime(timestamp,MaxTextExtent,"%Y:%m:%d:%H:%M:%S%Z",t);
  timestamp[MaxTextExtent-1]='\0';
  (void) memset(date_str,0,date_str_length);
  (void) strlcpy(date_str,timestamp,11);
  (void) memset(time_str,0,time_str_length);
  (void) strlcpy(time_str,timestamp+11,15);
}


static unsigned int WriteCINEONImage(const ImageInfo *image_info,Image *image)
{
  CineonFileInfo
    cin_file_info;

  CineonImageInfo
    cin_image_info;

  CineonImageOriginationInfo
    cin_source_info;

  CineonFilmInfo
    cin_mp_info;

  const unsigned char
    *user_data;

  size_t
    user_data_length=0,
    offset=0;

  size_t
    written=0;

  MagickBool
    swab=MagickFalse;

  long
    y;

  register const PixelPacket
    *p;
  
  register long
    x;

  unsigned int
    status;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  /*
    Cineon files are written in Cineon Log color space.
  */
  if (image->colorspace != CineonLogRGBColorspace)
    (void) TransformColorspace(image,CineonLogRGBColorspace);

#if !defined(WORDS_BIGENDIAN)
  swab=MagickTrue;
#endif /* !defined(WORDS_BIGENDIAN) */

  /*
    File information header
  */
  (void) memset(&cin_file_info,0,sizeof(cin_file_info));

  /* Magick number (0x802A5FD7) */
  cin_file_info.magic = 0x802A5FD7;
  /* Generic section header length in bytes */
  cin_file_info.generic_section_length = sizeof(cin_file_info)+sizeof(cin_image_info)+sizeof(cin_source_info);
  /* Industry specific header length in bytes */
  cin_file_info.industry_section_length = sizeof(cin_mp_info);
  /* User defined header length in bytes */
  cin_file_info.user_defined_length = 0;
  user_data=GetImageProfile(image,"CINEONUSERDATA",&user_data_length);
  if (user_data && user_data_length)
    cin_file_info.user_defined_length = user_data_length;
  /* Offset to image data in bytes */
  cin_file_info.image_data_offset =
    cin_file_info.generic_section_length +
    cin_file_info.industry_section_length +
    cin_file_info.user_defined_length;
  /* Total image file size in bytes  */
  cin_file_info.file_size =
    cin_file_info.generic_section_length +
    cin_file_info.industry_section_length +
    cin_file_info.user_defined_length +
    image->rows*image->columns*4;
  /* Version number of header format */
  (void) strlcpy(cin_file_info.header_format_version,"V4.5",sizeof(cin_file_info.header_format_version));
  /* Image filename */
  (void) strlcpy(cin_file_info.image_filename,image->filename,sizeof(cin_file_info.image_filename));
  /* Creation date "yyyy:mm:dd", and time "hh:mm:ssLTZ" */
  GenerateCineonTimeStamp(cin_file_info.creation_date,sizeof(cin_file_info.creation_date),
                          cin_file_info.creation_time,sizeof(cin_file_info.creation_time));

  /*
    Image information header
  */
  (void) memset(&cin_image_info,0,sizeof(cin_image_info));
  /* Image orientation */
  cin_image_info.orientation = 0; /* left to right, top to bottom */
  /* Number of image channels (1-8) */
  cin_image_info.channels = 3; /* RGB */

  /* Channel 0 (Red) */
  cin_image_info.channel_info[0].designator_byte_0 = 0;  /* 0 = universal metric */
  cin_image_info.channel_info[0].designator_byte_1 = 1;  /* Red, Printing Density */
  /* Bit depth */
  cin_image_info.channel_info[0].bits_per_sample = 10;
  /* Pixels per line (columns) */
  cin_image_info.channel_info[0].pixels_per_line = image->columns;
  /* Lines per image (rows) */
  cin_image_info.channel_info[0].lines_per_image = image->rows;
  /* Reference low data code value */
  cin_image_info.channel_info[0].reference_low_data_code = 0;
  /* Low quantity represented */
  cin_image_info.channel_info[0].reference_low_quantity = 0.00;
  /* Reference high data code value */
  cin_image_info.channel_info[0].reference_high_data_code = 1023;
  /* Reference high quantity represented */
  cin_image_info.channel_info[0].reference_high_quantity = 2.048F;

  /* Channel 1 (Green) */
  cin_image_info.channel_info[1] = cin_image_info.channel_info[0];
  cin_image_info.channel_info[1].designator_byte_1 = 2; /* Green, Printing Density */

  /* Channel 2  (Blue) */
  cin_image_info.channel_info[2] = cin_image_info.channel_info[0];
  cin_image_info.channel_info[2].designator_byte_1 = 3; /* Blue, Printing Density */

  /* White point (color temperature) - x,y pair */
  SET_UNDEFINED_R32(cin_image_info.white_point[0]);
  SET_UNDEFINED_R32(cin_image_info.white_point[1]);
  if ( image->chromaticity.white_point.x != 0.0 && image->chromaticity.white_point.y != 0.0 )
    {
      cin_image_info.white_point[0] = image->chromaticity.white_point.x;
      cin_image_info.white_point[1] = image->chromaticity.white_point.y;
    }
  /* Red primary chromaticity - x,y pair */
  SET_UNDEFINED_R32(cin_image_info.red_primary_chromaticity[0]);
  SET_UNDEFINED_R32(cin_image_info.red_primary_chromaticity[1]);
  if ( image->chromaticity.red_primary.x != 0.0 &&  image->chromaticity.red_primary.y != 0.0)
    {
      cin_image_info.red_primary_chromaticity[0] = image->chromaticity.red_primary.x;
      cin_image_info.red_primary_chromaticity[1] = image->chromaticity.red_primary.y;
    }
  /* Green primary chromaticity - x,y pair */
  SET_UNDEFINED_R32(cin_image_info.green_primary_chromaticity[0]);
  SET_UNDEFINED_R32(cin_image_info.green_primary_chromaticity[1]);
  if ( image->chromaticity.green_primary.x != 0.0 && image->chromaticity.green_primary.y != 0.0 )
    {
      cin_image_info.green_primary_chromaticity[0] = image->chromaticity.green_primary.x;
      cin_image_info.green_primary_chromaticity[1] = image->chromaticity.green_primary.y;
    }
  /* Blue primary chromaticity - x,y pair */
  SET_UNDEFINED_R32(cin_image_info.blue_primary_chromaticity[0]);
  SET_UNDEFINED_R32(cin_image_info.blue_primary_chromaticity[1]);
  if ( image->chromaticity.blue_primary.x != 0.0 && image->chromaticity.blue_primary.y != 0.0 )
    {
      cin_image_info.blue_primary_chromaticity[0] = image->chromaticity.blue_primary.x;
      cin_image_info.blue_primary_chromaticity[1] = image->chromaticity.blue_primary.y;
    }
  /* Label text */
  AttributeToString(image_info,image,"DPX:file.project.name",cin_image_info.label_text);
  /* Data interleave */
  cin_image_info.data_interleave = 0; /* rgbrgbrgb ... */
  /* Packing method */
  cin_image_info.packing = 5; /* longword (32-bit) boundaries - left justified */
  /* Data sign: 0=unsigned, 1=signed */
  cin_image_info.sign = 0;
  /* Image sense: 0=positive, 1=negative */
  cin_image_info.sense = 0;
  /* End of line padding */
  cin_image_info.eol_pad = 0;
  /* End of channel padding */
  cin_image_info.eoc_pad = 0;
  
  /*
    Image origination header.
  */
  (void) memset(&cin_source_info,0,sizeof(cin_source_info));
  AttributeToS32(image_info,image,"DPX:source.x-offset",cin_source_info.x_offset); /* X offset */
  AttributeToS32(image_info,image,"DPX:source.y-offset",cin_source_info.y_offset); /* Y offset */
  /* Source image filename */
  AttributeToString(image_info,image,"DPX:source.filename",cin_source_info.source_image_filename);
  {
    /*
      DPX stores creation date and time in the format
      "yyyy:mm:dd:hh:mm:ssLTZ" but Cineon wants date and time to be split into
      yyyy:mm:dd & hh:mm:ssLTZ
    */
    ASCII date_and_time[25];
    AttributeToString(image_info,image,"DPX:source.creation.datetime",date_and_time);
    SET_UNDEFINED_ASCII(cin_source_info.creation_date);
    SET_UNDEFINED_ASCII(cin_source_info.creation_time);

    if (!IS_UNDEFINED_ASCII(date_and_time))
      {
        /* Creation date: yyyy:mm:dd */
        (void) strlcpy(cin_source_info.creation_date,date_and_time,11);
        /* Creation time: hh:mm:ssLTZ */
        (void) strlcpy(cin_source_info.creation_time,date_and_time+11,sizeof(cin_source_info.creation_time));
      }
  }
  /* Input device */
  AttributeToString(image_info,image,"DPX:source.device.name",cin_source_info.input_device);
  /* Input device model number */
  AttributeToString(image_info,image,"DPX:source.device.model",cin_source_info.input_device_model);
  /* Input device serial number */
  AttributeToString(image_info,image,"DPX:source.device.serialnumber",cin_source_info.input_device_serial);
  /* Input device pitch for X (samples/mm) */
  AttributeToR32(image_info,image,"DPX:source.device.pitch.x",cin_source_info.input_device_pitch_x);
  /* Input device pitch for Y (samples/mm) */
  AttributeToR32(image_info,image,"DPX:source.device.pitch.y",cin_source_info.input_device_pitch_y);
  /* Image gamma of capture device */
  AttributeToR32(image_info,image,"DPX:source.device.gamma",cin_source_info.gamma);

  /*
    Film/Frame Information
  */
  (void) memset(&cin_mp_info,0,sizeof(cin_mp_info));
  /* Film mfg. ID code (2 digits from film edge code) */
  AttributeToU8(image_info,image,"DPX:mp.film.manufacturer.id",cin_mp_info.film_mfg_id_code);
  /* Film type (2 digits from film edge code) */
  AttributeToU8(image_info,image,"DPX:mp.film.type",cin_mp_info.film_type);
  /* Offset in perfs (2 digits from film edge code) */
  AttributeToU8(image_info,image,"DPX:mp.perfs.offset",cin_mp_info.perfs_offset);
  /* Prefix (6 digits from film edge code) */
  AttributeToU32(image_info,image,"DPX:mp.prefix",cin_mp_info.prefix);
  /* Count (4 digits from film edge code) */
  AttributeToU32(image_info,image,"DPX:mp.count",cin_mp_info.count);
  /* Format -- e.g. Academy */
  AttributeToString(image_info,image,"DPX:mp.format",cin_mp_info.format);
  /* Frame position in sequence */
  AttributeToU32(image_info,image,"DPX:mp.frame.position",cin_mp_info.frame_position);
  /* Frame rate of original (frames/s) */
  AttributeToR32(image_info,image,"DPX:mp.frame.rate",cin_mp_info.frame_rate);
  /* Frame identification - e.g. keyframe */
  AttributeToString(image_info,image,"DPX:mp.frame.id",cin_mp_info.frame_id);
  /* Slate information */
  AttributeToString(image_info,image,"DPX:mp.slate.info",cin_mp_info.slate_info);
  
  /*
    Open output image file.
  */

  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == MagickFail)
    return (status);

  if (swab)
    {
      /*
        Swap byte order.
      */
      SwabCineonFileInfo(&cin_file_info);
      SwabCineonImageInfo(&cin_image_info);
      SwabCineonImageOriginationInfo(&cin_source_info);
      SwabCineonFilmInfo(&cin_mp_info);
    }

  written = WriteBlob(image,sizeof(cin_file_info),&cin_file_info);
  if (written != sizeof(cin_file_info))
    {
      CloseBlob(image);
      return (MagickFail);
    }
  offset += written;

  written = WriteBlob(image,sizeof(cin_image_info),&cin_image_info);
  if (written != sizeof(cin_image_info))
    {
      CloseBlob(image);
      return (MagickFail);
    }
  offset += written;

  written = WriteBlob(image,sizeof(cin_source_info),&cin_source_info);
  if (written != sizeof(cin_source_info))
    {
      CloseBlob(image);
      return (MagickFail);
    }
  offset += written;

  written = WriteBlob(image,sizeof(cin_mp_info),&cin_mp_info);
  if (written != sizeof(cin_mp_info))
    {
      CloseBlob(image);
      return (MagickFail);
    }
  offset += written;

  if (swab)
    {
      /*
        Swap byte order back to original.
      */
      SwabCineonFileInfo(&cin_file_info);
      SwabCineonImageInfo(&cin_image_info);
      SwabCineonImageOriginationInfo(&cin_source_info);
      SwabCineonFilmInfo(&cin_mp_info);
    }

  if (user_data)
    {
      written = WriteBlob(image,user_data_length,user_data);
      if (written != user_data_length)
        {
          CloseBlob(image);
          return (MagickFail);
        }
      offset += written;
    }

  if (image->logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "Writing Cineon pixels starting at offset %ld",(long) TellBlob(image));

  /*
    Convert pixel packets to CINEON raster image.
  */
  {
    unsigned char
      *scanline;

    size_t
      scanline_bytes;

    BitStreamWriteHandle
      bit_stream;

    unsigned int
      red,
      green,
      blue;

    unsigned int
      bits_per_sample,
      scale_from_short;

    bits_per_sample = cin_image_info.channel_info[0].bits_per_sample;
    scale_from_short=(65535U / (65535U >> (16-bits_per_sample)));

    scanline_bytes=image->columns*4;
    scanline=MagickAllocateMemory(unsigned char *,scanline_bytes);
    if (scanline == (unsigned char *) NULL)
      ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
    (void) memset(scanline,0,scanline_bytes);

    for (y=0; y < (long) image->rows; y++)
      {
        p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
        if (p == (const PixelPacket *) NULL)
          break;
        
        BitStreamInitializeWrite(&bit_stream,scanline);
        
        for (x=0; x < (long) image->columns; x++)
          {
            red   = ScaleQuantumToShort(p->red)/scale_from_short;
            green = ScaleQuantumToShort(p->green)/scale_from_short;
            blue  = ScaleQuantumToShort(p->blue)/scale_from_short;

/*             printf("o:%u,%u,%u --> %u,%u,%u\n",(unsigned int)p->red, (unsigned int) p->green, (unsigned int) p->blue, */
/*                    red, green, blue); */

            BitStreamMSBWrite(&bit_stream,10,red);
            BitStreamMSBWrite(&bit_stream,10,green);
            BitStreamMSBWrite(&bit_stream,10,blue);
            BitStreamMSBWrite(&bit_stream,2,0);
            p++;
          }
        written = WriteBlob(image,scanline_bytes,scanline);
        if (written != scanline_bytes)
          {
            status = MagickFail;
            break;
          }
        offset += written;

        if (image->previous == (Image *) NULL)
          if (QuantumTick(y,image->rows))
            if (!MagickMonitorFormatted(y,image->rows,&image->exception,
                                        SaveImageText,image->filename))
              break;
      }
    MagickFreeMemory(scanline);
  }

  if ((magick_off_t) cin_file_info.file_size != TellBlob(image))
    {
      (void) printf("### File length %u, TellBlob says %u\n",
                    cin_file_info.file_size,
                    (unsigned int) TellBlob(image));
    }

  CloseBlob(image);
  return(status);
}
