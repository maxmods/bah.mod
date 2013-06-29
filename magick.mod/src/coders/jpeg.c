/*
% Copyright (C) 2003-2011 GraphicsMagick Group
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
%                        JJJJJ  PPPP   EEEEE   GGGG                           %
%                          J    P   P  E      G                               %
%                          J    PPPP   EEE    G  GG                           %
%                        J J    P      E      G   G                           %
%                        JJJ    P      EEEEE   GGG                            %
%                                                                             %
%                                                                             %
%                       Read/Write JPEG Image Format.                         %
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
% This software is based in part on the work of the Independent JPEG Group.
% See ftp://ftp.uu.net/graphics/jpeg/jpegsrc.v6b.tar.gz for copyright and
% licensing restrictions.  Blob support contributed by Glenn Randers-Pehrson.
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/analyze.h"
#include "magick/attribute.h"
#include "magick/blob.h"
#include "magick/colormap.h"
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/pixel_cache.h"
#include "magick/profile.h"
#include "magick/resource.h"
#include "magick/utility.h"

/*
  Forward declarations.
*/
static unsigned int
  WriteJPEGImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s J P E G                                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsJPEG returns True if the image format type, identified by the
%  magick string, is JPEG.
%
%  The format of the IsJPEG  method is:
%
%      unsigned int IsJPEG(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsJPEG returns True if the image format type is JPEG.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsJPEG(const unsigned char *magick,const size_t length)
{
  if (length < 3)
    return(False);
  if (memcmp(magick,"\377\330\377",3) == 0)
    return(True);
  return(False);
}

#if defined(HasJPEG)
#define JPEG_INTERNAL_OPTIONS
/*
  Avoid conflicting typedef for INT32
*/
#if defined(__MINGW32__)
# define XMD_H 1
#endif
/*
  The JPEG headers have the annoying problem that they define
  HAVE_STDLIB_H and we do too.  The define isn't actually used
  so just undef it.
*/
#undef HAVE_STDLIB_H
#include <setjmp.h>
#include "jpeglib.h"
#include "jerror.h"

/*
  Define declarations.
*/
#define ICC_MARKER  (JPEG_APP0+2)
#define IPTC_MARKER  (JPEG_APP0+13)
#define MaxBufferExtent  8192

typedef struct _DestinationManager
{
  struct jpeg_destination_mgr
    manager;

  Image
    *image;

  JOCTET
    *buffer;
} DestinationManager;

typedef struct _ErrorManager
{
  Image
    *image;

  MagickBool
    completed;

  jmp_buf
    error_recovery;

} ErrorManager;

typedef struct _SourceManager
{
  struct jpeg_source_mgr
    manager;

  Image
    *image;

  JOCTET
    *buffer;

  boolean
    start_of_blob;
} SourceManager;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d J P E G I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadJPEGImage reads a JPEG image file and returns it.  It allocates
%  the memory necessary for the new Image structure and returns a pointer to
%  the new image.
%
%  The format of the ReadJPEGImage method is:
%
%      Image *ReadJPEGImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadJPEGImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/

/*
  Format a libjpeg warning or trace event.  Warnings are converted to
  GraphicsMagick warning exceptions while traces are optionally
  logged.
*/
static unsigned int JPEGMessageHandler(j_common_ptr jpeg_info,int msg_level)
{
  char
    message[JMSG_LENGTH_MAX];

  struct jpeg_error_mgr
    *err;

  ErrorManager
    *error_manager;

  Image
    *image;

  message[0]='\0';
  err=jpeg_info->err;
  error_manager=(ErrorManager *) jpeg_info->client_data;
  image=error_manager->image;
  /* msg_level is -1 for warnings, 0 and up for trace messages. */
  if (msg_level < 0)
    {
      /* A warning */
      (err->format_message)(jpeg_info,message);

      if (image->logging)
	{
	  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
				"[%s] JPEG Warning: \"%s\" (code=%d, parms=0x%02x,0x%02x,"
				"0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x)",
				image->filename,message,err->msg_code,
				err->msg_parm.i[0], err->msg_parm.i[1],
				err->msg_parm.i[2], err->msg_parm.i[3],
				err->msg_parm.i[4], err->msg_parm.i[5],
				err->msg_parm.i[6], err->msg_parm.i[7]);
	}
      if ((err->num_warnings == 0) ||
          (err->trace_level >= 3))
	ThrowBinaryException2(CorruptImageWarning,(char *) message,
			      image->filename);
      err->num_warnings++;
    }
  else
    {
      /* A trace message */
      if ((image->logging) && (msg_level >= err->trace_level))
	{
	  (err->format_message)(jpeg_info,message);
	  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
				"[%s] JPEG Trace: \"%s\"",image->filename,message);
	}
    }
  return(True);
}

static boolean FillInputBuffer(j_decompress_ptr cinfo)
{
  SourceManager
    *source;

  source=(SourceManager *) cinfo->src;
  source->manager.bytes_in_buffer=
    ReadBlob(source->image,MaxBufferExtent,(char *) source->buffer);
  if (source->manager.bytes_in_buffer == 0)
    {
      if (source->start_of_blob)
        ERREXIT(cinfo,JERR_INPUT_EMPTY);
      WARNMS(cinfo,JWRN_JPEG_EOF);
      source->buffer[0]=(JOCTET) 0xff;
      source->buffer[1]=(JOCTET) JPEG_EOI;
      source->manager.bytes_in_buffer=2;
    }
  source->manager.next_input_byte=source->buffer;
  source->start_of_blob=FALSE;
  return(TRUE);
}

static unsigned int GetCharacter(j_decompress_ptr jpeg_info)
{
  if (jpeg_info->src->bytes_in_buffer == 0)
    (void) (*jpeg_info->src->fill_input_buffer)(jpeg_info);
  jpeg_info->src->bytes_in_buffer--;
  return(GETJOCTET(*jpeg_info->src->next_input_byte++));
}

static void InitializeSource(j_decompress_ptr cinfo)
{
  SourceManager
    *source;

  source=(SourceManager *) cinfo->src;
  source->start_of_blob=TRUE;
}

/*
  Format and report a libjpeg error event.  Errors are reported via a
  GraphicsMagick error exception. The function terminates with
  longjmp() so it never returns to the caller.
*/
static void JPEGErrorHandler(j_common_ptr jpeg_info) MAGICK_FUNC_NORETURN;

static void JPEGErrorHandler(j_common_ptr jpeg_info)
{
  char
    message[JMSG_LENGTH_MAX];

  struct jpeg_error_mgr
    *err;

  ErrorManager
    *error_manager;

  Image
    *image;

  message[0]='\0';
  err=jpeg_info->err;
  error_manager=(ErrorManager *) jpeg_info->client_data;
  image=error_manager->image;
  (err->format_message)(jpeg_info,message);
  if (image->logging)
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
			    "[%s] JPEG Error: \"%s\" (code=%d, parms=0x%02x,0x%02x,"
			    "0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x)",
			    image->filename,message,err->msg_code,
			    err->msg_parm.i[0], err->msg_parm.i[1],
			    err->msg_parm.i[2], err->msg_parm.i[3],
			    err->msg_parm.i[4], err->msg_parm.i[5],
			    err->msg_parm.i[6], err->msg_parm.i[7]);
    }
  if (error_manager->completed)
    ThrowException2(&image->exception,CoderWarning,(char *) message,image->filename);
  else
    ThrowException2(&image->exception,CoderError,(char *) message,image->filename);
  longjmp(error_manager->error_recovery,1);
}

static boolean ReadComment(j_decompress_ptr jpeg_info)
{
  char
    *comment;

  ErrorManager
    *error_manager;

  Image
    *image;

  size_t
    i,
    length;

  register char
    *p;

  /*
    Determine length of comment.
  */
  error_manager=(ErrorManager *) jpeg_info->client_data;
  image=error_manager->image;
  length=GetCharacter(jpeg_info) << 8;
  length+=GetCharacter(jpeg_info);
  if (length <= 2)
    return(True);
  length-=2;
  comment=MagickAllocateMemory(char *,length+1);
  if (comment == (char *) NULL)
    ThrowBinaryException(ResourceLimitError,MemoryAllocationFailed,
      (char *) NULL);
  /*
    Read comment.
  */
  p=comment;
  for (i=length; i != 0; i--)
    {
      *p=GetCharacter(jpeg_info);
      p++;
    }
  *p='\0';
  (void) SetImageAttribute(image,"comment",comment);
  MagickFreeMemory(comment);
  return(True);
}

static boolean ReadGenericProfile(j_decompress_ptr jpeg_info)
{
  ErrorManager
    *error_manager;

  Image
    *image;

  size_t
    length;

  register size_t
    i;

  char
    profile_name[MaxTextExtent];

  unsigned char
    *profile;

  int
    marker;

  boolean
    status = True;

  /*
    Determine length of generic profile.
  */
  length=GetCharacter(jpeg_info) << 8;
  length+=GetCharacter(jpeg_info);
  if (length <= 2)
    return(status);
  length-=2;

  marker=jpeg_info->unread_marker-JPEG_APP0;

  /*
    Compute generic profile name.
  */
  FormatString(profile_name,"APP%d",marker);

  /*
    Obtain Image.
  */
  error_manager=(ErrorManager *) jpeg_info->client_data;
  image=error_manager->image;

  /*
    Copy profile from JPEG to allocated memory.
  */
  profile=MagickAllocateMemory(unsigned char *,length);
  if (profile == (unsigned char *) NULL)
    ThrowBinaryException(ResourceLimitError,MemoryAllocationFailed,
                         (char *) NULL);

  for (i=0 ; i < length ; i++)
    profile[i]=GetCharacter(jpeg_info);

  /*
    Detect EXIF and XMP profiles.
  */
  if ((marker==1) && (length > 4) &&
      (strncmp((char *) profile,"Exif",4) == 0))
    FormatString(profile_name,"EXIF");
  else if (((marker==1) && length > 5) &&
	   (strncmp((char *) profile,"http:",5) == 0))
    FormatString((char *) profile,"XMP");

  /*
    Store profile in Image.
  */
  status=AppendImageProfile(image,profile_name,profile,length);
  MagickFreeMemory(profile);

  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Profile: %s, %lu bytes",
                        profile_name,(unsigned long) length);

  return (status);
}

static boolean ReadICCProfile(j_decompress_ptr jpeg_info)
{
  char
    magick[12];

  ErrorManager
    *error_manager;

  Image
    *image;

  long
    length;

  register long
    i;

  unsigned char
    *profile;

  /*
    Determine length of color profile.
  */
  length=(long) GetCharacter(jpeg_info) << 8;
  length+=(long) GetCharacter(jpeg_info);
  length-=2;
  if (length <= 14)
    {
      while (--length >= 0)
        (void) GetCharacter(jpeg_info);
      return(True);
    }
  for (i=0; i < 12; i++)
    magick[i]=GetCharacter(jpeg_info);
  if (LocaleCompare(magick,"ICC_PROFILE") != 0)
    {
      /*
        Not a ICC profile, return.
      */
      for (i=0; i < length-12; i++)
        (void) GetCharacter(jpeg_info);
      return(True);
    }
  (void) GetCharacter(jpeg_info);  /* id */
  (void) GetCharacter(jpeg_info);  /* markers */
  length-=14;
  error_manager=(ErrorManager *) jpeg_info->client_data;
  image=error_manager->image;

  /*
    Read color profile.
  */
  profile=MagickAllocateMemory(unsigned char *,(size_t) length);
  if (profile == (unsigned char *) NULL)
    ThrowBinaryException(ResourceLimitError,MemoryAllocationFailed,
      (char *) NULL);

  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
			"ICC profile chunk: %ld bytes",
    length);

  for (i=0 ; i < length; i++)
   profile[i]=GetCharacter(jpeg_info);

  (void) AppendImageProfile(image,"ICM",profile,length);

  MagickFreeMemory(profile);

  return(True);
}

static boolean ReadIPTCProfile(j_decompress_ptr jpeg_info)
{
  char
    magick[MaxTextExtent];

  ErrorManager
    *error_manager;

  Image
    *image;

  long
    length,
    tag_length;

  unsigned char
    *profile;

  register long
    i;

#ifdef GET_ONLY_IPTC_DATA
  unsigned char
    tag[MaxTextExtent];
#endif

  /*
    Determine length of binary data stored here.
  */
  length=(long) GetCharacter(jpeg_info) << 8;
  length+=(long) GetCharacter(jpeg_info);
  length-=2;
  if (length <= 0)
    return(True);
  tag_length=0;
#ifdef GET_ONLY_IPTC_DATA
  *tag='\0';
#endif
  error_manager=(ErrorManager *) jpeg_info->client_data;
  image=error_manager->image;
#ifdef GET_ONLY_IPTC_DATA
  /*
    Find the beginning of the IPTC portion of the binary data.
  */
  for (*tag='\0'; length > 0; )
    {
      *tag=GetCharacter(jpeg_info);
      *(tag+1)=GetCharacter(jpeg_info);
      length-=2;
      if ((*tag == 0x1c) && (*(tag+1) == 0x02))
	break;
    }
  tag_length=2;
#else
  /*
    Validate that this was written as a Photoshop resource format slug.
  */
  for (i=0; i < 10; i++)
    magick[i]=GetCharacter(jpeg_info);
  magick[10]='\0';
  length-=10;
  if (LocaleCompare(magick,"Photoshop ") != 0)
    {
      /*
	Not a ICC profile, return.
      */
      for (i=0; i < length; i++)
	(void) GetCharacter(jpeg_info);
      return(True);
    }
  /*
    Remove the version number.
  */
  for (i=0; i < 4; i++)
    (void) GetCharacter(jpeg_info);
  length-=4;
  tag_length=0;
#endif
  if (length <= 0)
    return(True);

  profile=MagickAllocateMemory(unsigned char *,(size_t) length+tag_length);
  if (profile == (unsigned char *) NULL)
    ThrowBinaryException(ResourceLimitError,MemoryAllocationFailed,
      (char *) NULL);
  /*
    Read the payload of this binary data.
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
			"Profile: IPTC, %ld bytes",
    length);

  for (i=0; i<length; i++)
    profile[i]=GetCharacter(jpeg_info);

  (void) AppendImageProfile(image,"IPTC",profile,length);

  MagickFreeMemory(profile);
  return(True);
}

static void SkipInputData(j_decompress_ptr cinfo,long number_bytes)
{
  SourceManager
    *source;

  if (number_bytes <= 0)
    return;
  source=(SourceManager *) cinfo->src;
  while (number_bytes > (long) source->manager.bytes_in_buffer)
  {
    number_bytes-=(long) source->manager.bytes_in_buffer;
    (void) FillInputBuffer(cinfo);
  }
  source->manager.next_input_byte+=(size_t) number_bytes;
  source->manager.bytes_in_buffer-=(size_t) number_bytes;
}

static void TerminateSource(j_decompress_ptr cinfo)
{
  (void) cinfo;
}

static void JPEGSourceManager(j_decompress_ptr cinfo,Image *image)
{
  SourceManager
    *source;

  cinfo->src=(struct jpeg_source_mgr *) (*cinfo->mem->alloc_small)
    ((j_common_ptr) cinfo,JPOOL_IMAGE,sizeof(SourceManager));
  source=(SourceManager *) cinfo->src;
  source->buffer=(JOCTET *) (*cinfo->mem->alloc_small)
    ((j_common_ptr) cinfo,JPOOL_IMAGE,MaxBufferExtent*sizeof(JOCTET));
  source=(SourceManager *) cinfo->src;
  source->manager.init_source=InitializeSource;
  source->manager.fill_input_buffer=FillInputBuffer;
  source->manager.skip_input_data=SkipInputData;
  source->manager.resync_to_restart=jpeg_resync_to_restart;
  source->manager.term_source=TerminateSource;
  source->manager.bytes_in_buffer=0;
  source->manager.next_input_byte=NULL;
  source->image=image;
}

/*
  Estimate the IJG quality factor used when saving the file.
*/
static int
EstimateJPEGQuality(const struct jpeg_decompress_struct *jpeg_info,
		    Image *image)
{
  int
    save_quality;

  register long
    i;

  save_quality=0;
#ifdef D_LOSSLESS_SUPPORTED
  if (image->compression==LosslessJPEGCompression)
    {
      save_quality=100;
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
			    "Quality: 100 (lossless)");
    }
  else
#endif

    {
      int
	hashval,
	sum;

      /*
	Log the JPEG quality that was used for compression.
      */
      sum=0;
      for (i=0; i < NUM_QUANT_TBLS; i++)
	{
	  int
	    j;

	  if (jpeg_info->quant_tbl_ptrs[i] != NULL)
	    for (j=0; j < DCTSIZE2; j++)
	      {
		UINT16 *c;
		c=jpeg_info->quant_tbl_ptrs[i]->quantval;
		sum+=c[j];
	      }
	}
      if ((jpeg_info->quant_tbl_ptrs[0] != NULL) &&
	  (jpeg_info->quant_tbl_ptrs[1] != NULL))
	{
	  int
	    hash[] =
	    {
	      1020, 1015,  932,  848,  780,  735,  702,  679,  660,  645,
	      632,  623,  613,  607,  600,  594,  589,  585,  581,  571,
	      555,  542,  529,  514,  494,  474,  457,  439,  424,  410,
	      397,  386,  373,  364,  351,  341,  334,  324,  317,  309,
	      299,  294,  287,  279,  274,  267,  262,  257,  251,  247,
	      243,  237,  232,  227,  222,  217,  213,  207,  202,  198,
	      192,  188,  183,  177,  173,  168,  163,  157,  153,  148,
	      143,  139,  132,  128,  125,  119,  115,  108,  104,   99,
	      94,   90,   84,   79,   74,   70,   64,   59,   55,   49,
	      45,   40,   34,   30,   25,   20,   15,   11,    6,    4,
	      0
	    };

	  int
	    sums[] =
	    {
	      32640,32635,32266,31495,30665,29804,29146,28599,28104,27670,
	      27225,26725,26210,25716,25240,24789,24373,23946,23572,22846,
	      21801,20842,19949,19121,18386,17651,16998,16349,15800,15247,
	      14783,14321,13859,13535,13081,12702,12423,12056,11779,11513,
	      11135,10955,10676,10392,10208, 9928, 9747, 9564, 9369, 9193,
	      9017, 8822, 8639, 8458, 8270, 8084, 7896, 7710, 7527, 7347,
	      7156, 6977, 6788, 6607, 6422, 6236, 6054, 5867, 5684, 5495,
	      5305, 5128, 4945, 4751, 4638, 4442, 4248, 4065, 3888, 3698,
	      3509, 3326, 3139, 2957, 2775, 2586, 2405, 2216, 2037, 1846,
	      1666, 1483, 1297, 1109,  927,  735,  554,  375,  201,  128,
	      0
	    };

	  hashval=(jpeg_info->quant_tbl_ptrs[0]->quantval[2]+
		   jpeg_info->quant_tbl_ptrs[0]->quantval[53]+
		   jpeg_info->quant_tbl_ptrs[1]->quantval[0]+
		   jpeg_info->quant_tbl_ptrs[1]->quantval[DCTSIZE2-1]);
	  for (i=0; i < 100; i++)
	    {
	      if ((hashval >= hash[i]) || (sum >= sums[i]))
		{
		  save_quality=i+1;
		  if (image->logging)
		    {
		      if ((hashval > hash[i]) || (sum > sums[i]))
			(void) LogMagickEvent(CoderEvent,GetMagickModule(),
					      "Quality: %d (approximate)",
					      save_quality);
		      else
			(void) LogMagickEvent(CoderEvent,GetMagickModule(),
					      "Quality: %d",save_quality);
		    }
		  break;
		}
	    }
	}
      else
	if (jpeg_info->quant_tbl_ptrs[0] != NULL)
	  {
	    int
	      bwhash[] =
	      {
		510,  505,  422,  380,  355,  338,  326,  318,  311,  305,
		300,  297,  293,  291,  288,  286,  284,  283,  281,  280,
		279,  278,  277,  273,  262,  251,  243,  233,  225,  218,
		211,  205,  198,  193,  186,  181,  177,  172,  168,  164,
		158,  156,  152,  148,  145,  142,  139,  136,  133,  131,
		129,  126,  123,  120,  118,  115,  113,  110,  107,  105,
		102,  100,   97,   94,   92,   89,   87,   83,   81,   79,
		76,   74,   70,   68,   66,   63,   61,   57,   55,   52,
		50,   48,   44,   42,   39,   37,   34,   31,   29,   26,
		24,   21,   18,   16,   13,   11,    8,    6,    3,    2,
		0
	      };

	    int
	      bwsum[] =
	      {
		16320,16315,15946,15277,14655,14073,13623,13230,12859,12560,
		12240,11861,11456,11081,10714,10360,10027, 9679, 9368, 9056,
		8680, 8331, 7995, 7668, 7376, 7084, 6823, 6562, 6345, 6125,
		5939, 5756, 5571, 5421, 5240, 5086, 4976, 4829, 4719, 4616, 
		4463, 4393, 4280, 4166, 4092, 3980, 3909, 3835, 3755, 3688,
		3621, 3541, 3467, 3396, 3323, 3247, 3170, 3096, 3021, 2952,
		2874, 2804, 2727, 2657, 2583, 2509, 2437, 2362, 2290, 2211,
		2136, 2068, 1996, 1915, 1858, 1773, 1692, 1620, 1552, 1477,
		1398, 1326, 1251, 1179, 1109, 1031,  961,  884,  814,  736,
		667,  592,  518,  441,  369,  292,  221,  151,   86,   64,
		0
	      };

	    hashval=(jpeg_info->quant_tbl_ptrs[0]->quantval[2]+
		     jpeg_info->quant_tbl_ptrs[0]->quantval[53]);
	    for (i=0; i < 100; i++)
	      {
		if ((hashval >= bwhash[i]) || (sum >= bwsum[i]))
		  {
		    save_quality=i+1;
		    if (image->logging)
		      {
			if ((hashval > bwhash[i]) || (sum > bwsum[i]))
			  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
						"Quality: %ld (approximate)",
						i+1);
			else
			  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
						"Quality: %ld",i+1);
		      }
		    break;
		  }
	      }
	  }
    }

  return save_quality;
}

/*
  Format JPEG color space to a string.
*/
static void
FormatJPEGColorSpace(const J_COLOR_SPACE colorspace,
		     char *colorspace_name)
{
  const char
    *s = NULL;

  switch (colorspace)
    {
    default:
    case JCS_UNKNOWN:
      s = "UNKNOWN";
      break;
    case JCS_GRAYSCALE:
      s = "GRAYSCALE";
      break;
    case JCS_RGB:
      s = "RGB";
      break;
    case JCS_YCbCr:
      s = "YCbCr";
      break;
    case JCS_CMYK:
      s = "CMYK";
      break;
    case JCS_YCCK:
      s = "YCCK";
      break;
    }
  (void) strlcpy(colorspace_name,s,MaxTextExtent);
}

/*
  Format JPEG sampling factors to a string.
*/
static void
FormatJPEGSamplingFactors(const struct jpeg_decompress_struct *jpeg_info,
			  char *sampling_factors)
{
  switch (jpeg_info->out_color_space)
    {
    case JCS_CMYK:
      {
	(void) FormatString(sampling_factors,"%dx%d,%dx%d,%dx%d,%dx%d",
			    jpeg_info->comp_info[0].h_samp_factor,
			    jpeg_info->comp_info[0].v_samp_factor,
			    jpeg_info->comp_info[1].h_samp_factor,
			    jpeg_info->comp_info[1].v_samp_factor,
			    jpeg_info->comp_info[2].h_samp_factor,
			    jpeg_info->comp_info[2].v_samp_factor,
			    jpeg_info->comp_info[3].h_samp_factor,
			    jpeg_info->comp_info[3].v_samp_factor);
        break;
      }
    case JCS_GRAYSCALE:
      {
	(void) FormatString(sampling_factors,"%dx%d",
			    jpeg_info->comp_info[0].h_samp_factor,
			    jpeg_info->comp_info[0].v_samp_factor);
        break;
      }
    case JCS_RGB:
      {
	(void) FormatString(sampling_factors,"%dx%d,%dx%d,%dx%d",
			    jpeg_info->comp_info[0].h_samp_factor,
			    jpeg_info->comp_info[0].v_samp_factor,
			    jpeg_info->comp_info[1].h_samp_factor,
			    jpeg_info->comp_info[1].v_samp_factor,
			    jpeg_info->comp_info[2].h_samp_factor,
			    jpeg_info->comp_info[2].v_samp_factor);
        break;
      }
    default:
      {
	(void) FormatString(sampling_factors,"%dx%d,%dx%d,%dx%d,%dx%d",
			    jpeg_info->comp_info[0].h_samp_factor,
			    jpeg_info->comp_info[0].v_samp_factor,
			    jpeg_info->comp_info[1].h_samp_factor,
			    jpeg_info->comp_info[1].v_samp_factor,
			    jpeg_info->comp_info[2].h_samp_factor,
			    jpeg_info->comp_info[2].v_samp_factor,
			    jpeg_info->comp_info[3].h_samp_factor,
			    jpeg_info->comp_info[3].v_samp_factor);
	break;
      }
    }
}

static MagickBool
IsITUFax(const Image* image)
{
  size_t
    profile_length;
  
  const unsigned char
    *profile;
  
  MagickBool
    status;
  
  status=MagickFalse;
  if ((profile=GetImageProfile(image,"APP1",&profile_length)) &&
      (profile_length >= 5))
    {
      if (profile[0] == 0x47 &&
	  profile[1] == 0x33 &&
	  profile[2] == 0x46 &&
	  profile[3] == 0x41 &&
	  profile[4] == 0x58)
      status=MagickTrue;
    }

    return status;
}

static Image *ReadJPEGImage(const ImageInfo *image_info,
			    ExceptionInfo *exception)
{
  ErrorManager
    error_manager;

  Image
    *image;

  IndexPacket
    index;

  long
    y;

  JSAMPLE
    *jpeg_pixels;

  JSAMPROW
    scanline[1];

  register long
    i;

  struct jpeg_decompress_struct
    jpeg_info;

  struct jpeg_error_mgr
    jpeg_error;

  register JSAMPLE
    *p;

  MagickPassFail
    status;

  unsigned long
    number_pixels;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFail)
    ThrowReaderException(FileOpenError,UnableToOpenFile,image);
  /*
    Initialize structures.
  */
  (void) memset(&error_manager,0,sizeof(error_manager));
  (void) memset(&jpeg_info,0,sizeof(jpeg_info));
  (void) memset(&jpeg_error,0,sizeof(jpeg_error));
  jpeg_info.err=jpeg_std_error(&jpeg_error);
  jpeg_info.err->emit_message=(void (*)(j_common_ptr,int)) JPEGMessageHandler;
  jpeg_info.err->error_exit=(void (*)(j_common_ptr)) JPEGErrorHandler;
  jpeg_pixels=(JSAMPLE *) NULL;
  error_manager.image=image;

  /*
    Set initial longjmp based error handler.
  */
  if (setjmp(error_manager.error_recovery))
    {
      jpeg_destroy_decompress(&jpeg_info);
      GetImageException(image,exception);
      CloseBlob(image);
      if (exception->severity < ErrorException)
        return(image);
      DestroyImage(image);
      return((Image *) NULL);
    }
  jpeg_info.client_data=(void *) &error_manager;

  jpeg_create_decompress(&jpeg_info);
  JPEGSourceManager(&jpeg_info,image);
  jpeg_set_marker_processor(&jpeg_info,JPEG_COM,ReadComment);
  jpeg_set_marker_processor(&jpeg_info,ICC_MARKER,ReadICCProfile);
  jpeg_set_marker_processor(&jpeg_info,IPTC_MARKER,ReadIPTCProfile);
  for (i=1; i < 16; i++)
    if ((i != 2) && (i != 13) && (i != 14))
      jpeg_set_marker_processor(&jpeg_info,JPEG_APP0+i,ReadGenericProfile);
  i=jpeg_read_header(&jpeg_info,True);
  if (IsITUFax(image))
    {
      if (image->logging)
	(void) LogMagickEvent(CoderEvent,GetMagickModule(),
			      "Image colorspace set to LAB");
      image->colorspace=LABColorspace;
      jpeg_info.out_color_space = JCS_YCbCr;
    }
  else if (jpeg_info.out_color_space == JCS_CMYK)
    {
      if (image->logging)
	(void) LogMagickEvent(CoderEvent,GetMagickModule(),
			      "Image colorspace set to CMYK");
      image->colorspace=CMYKColorspace;
    }
  if (jpeg_info.saw_JFIF_marker)
    {
      if ((jpeg_info.X_density != 1U) && (jpeg_info.Y_density != 1U))
        {
          /*
            Set image resolution.
          */
          image->x_resolution=jpeg_info.X_density;
          image->y_resolution=jpeg_info.Y_density;
          if (jpeg_info.density_unit == 1)
            image->units=PixelsPerInchResolution;
          if (jpeg_info.density_unit == 2)
            image->units=PixelsPerCentimeterResolution;
        }
    }
  /*
    If the desired image size is pre-set (e.g. by using -size), then
    let the JPEG library subsample for us.
  */
  number_pixels=image->columns*image->rows;
  if (number_pixels != 0)
    {
      double
        scale_factor;


      if (image->logging)
	(void) LogMagickEvent(CoderEvent,GetMagickModule(),
			      "Requested Geometry: %lux%lu",
			      image->columns,image->rows);
      jpeg_calc_output_dimensions(&jpeg_info);
      image->magick_columns=jpeg_info.output_width;
      image->magick_rows=jpeg_info.output_height;
      scale_factor=(double) jpeg_info.output_width/image->columns;
      if (scale_factor > ((double) jpeg_info.output_height/image->rows))
        scale_factor=(double) jpeg_info.output_height/image->rows;
      jpeg_info.scale_denom *=(unsigned int) scale_factor;
      jpeg_calc_output_dimensions(&jpeg_info);
      if (image->logging)
	(void) LogMagickEvent(CoderEvent,GetMagickModule(),
			      "Scale_factor: %ld (scale_num=%d, "
			      "scale_denom=%d)",
			      (long) scale_factor,
			      jpeg_info.scale_num,jpeg_info.scale_denom);
    }
#if 0
  /*
    The subrange parameter is set by the filename array syntax similar
    to the way an image is requested from a list (e.g. myfile.jpg[2]).
    Argument values other than zero are used to scale the image down
    by that factor.  IJG JPEG 62 (6b) supports values of 1,2,4, or 8
    while IJG JPEG 70 supports all values in the range 1-16.  This
    feature is useful in case you want to view all of the images with
    a consistent ratio.  Unfortunately, it uses the same syntax as
    list member access.
  */
  else if (image_info->subrange != 0)
    {
      jpeg_info.scale_denom *=(int) image_info->subrange;
      jpeg_calc_output_dimensions(&jpeg_info);
      if (image->logging)
	(void) LogMagickEvent(CoderEvent,GetMagickModule(),
			      "Requested Scaling Denominator: %d "
			      "(scale_num=%d, scale_denom=%d)",
			      (int) image_info->subrange,
			      jpeg_info.scale_num,jpeg_info.scale_denom);

    }
#endif
#if (JPEG_LIB_VERSION >= 61) && defined(D_PROGRESSIVE_SUPPORTED)
#ifdef D_LOSSLESS_SUPPORTED
  image->interlace=
    jpeg_info.process == JPROC_PROGRESSIVE ? LineInterlace : NoInterlace;
  image->compression=jpeg_info.process == JPROC_LOSSLESS ?
    LosslessJPEGCompression : JPEGCompression;
  if (jpeg_info.data_precision > 8)
    MagickError2(OptionError,
		 "12-bit JPEG not supported. Reducing pixel data to 8 bits",
		 (char *) NULL);
#else
  image->interlace=jpeg_info.progressive_mode ? LineInterlace : NoInterlace;
  image->compression=JPEGCompression;
#endif
#else
  image->compression=JPEGCompression;
  image->interlace=LineInterlace;
#endif
  {
    const char
      *value;

    /*
      Allow the user to enable/disable block smoothing.
    */
    if ((value=AccessDefinition(image_info,"jpeg","block-smoothing")))
      {
	if (LocaleCompare(value,"FALSE") == 0)
          jpeg_info.do_block_smoothing=False;
        else
          jpeg_info.do_block_smoothing=True;
      }

    /*
      Allow the user to select the DCT decoding algorithm.
    */
    if ((value=AccessDefinition(image_info,"jpeg","dct-method")))
      {
        if (LocaleCompare(value,"ISLOW") == 0)
          jpeg_info.dct_method=JDCT_ISLOW;
        else if (LocaleCompare(value,"IFAST") == 0)
          jpeg_info.dct_method=JDCT_IFAST;
        else if (LocaleCompare(value,"FLOAT") == 0)
          jpeg_info.dct_method=JDCT_FLOAT;
        else if (LocaleCompare(value,"DEFAULT") == 0)
          jpeg_info.dct_method=JDCT_DEFAULT;
        else if (LocaleCompare(value,"FASTEST") == 0)
          jpeg_info.dct_method=JDCT_FASTEST;
      }

    /*
      Allow the user to enable/disable fancy upsampling.
    */
    if ((value=AccessDefinition(image_info,"jpeg","fancy-upsampling")))
      {
	if (LocaleCompare(value,"FALSE") == 0)
          jpeg_info.do_fancy_upsampling=False;
        else
          jpeg_info.do_fancy_upsampling=True;
      }
  }
  (void) jpeg_start_decompress(&jpeg_info);
  image->columns=jpeg_info.output_width;
  image->rows=jpeg_info.output_height;
  if (image->logging)
    {
      if (image->interlace == LineInterlace)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
			      "Interlace: progressive");
      else
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
			      "Interlace: nonprogressive");
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Data precision: %d",
			    (int) jpeg_info.data_precision);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Components: %d",
			    (int) jpeg_info.output_components);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Geometry: %dx%d",
			    (int) jpeg_info.output_width,
			    (int) jpeg_info.output_height);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),"DCT Method: %d",
			    jpeg_info.dct_method);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Fancy Upsampling: %s",
			    (jpeg_info.do_fancy_upsampling ? "true" : "false"));
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Block Smoothing: %s",
			    (jpeg_info.do_block_smoothing ? "true" : "false"));
    }

  {
    char
      attribute[MaxTextExtent];

    /*
      Estimate and retain JPEG properties as attributes.
    */
    FormatString(attribute,"%d",EstimateJPEGQuality(&jpeg_info,image));
    (void) SetImageAttribute(image,"JPEG-Quality",attribute);

    FormatString(attribute,"%ld",(long)jpeg_info.out_color_space);
    (void) SetImageAttribute(image,"JPEG-Colorspace",attribute);

    FormatJPEGColorSpace(jpeg_info.out_color_space,attribute);
    (void) SetImageAttribute(image,"JPEG-Colorspace-Name",attribute);
    if (image->logging)
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
			    "Colorspace: %s (%d)", attribute,
			    jpeg_info.out_color_space);

    FormatJPEGSamplingFactors(&jpeg_info,attribute);
    (void) SetImageAttribute(image,"JPEG-Sampling-factors",attribute);
    if (image->logging)
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
			    "Sampling Factors: %s", attribute);
  }
  
  image->depth=Min(jpeg_info.data_precision,QuantumDepth);
  if (jpeg_info.out_color_space == JCS_GRAYSCALE)
    if (!AllocateImageColormap(image,1 << image->depth))
      ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
  if (image_info->ping)
    {
      jpeg_destroy_decompress(&jpeg_info);
      CloseBlob(image);
      return(image);
    }
  jpeg_pixels=MagickAllocateArray(JSAMPLE *,
				  jpeg_info.output_components,
				  image->columns*sizeof(JSAMPLE));
  if (jpeg_pixels == (JSAMPLE *) NULL)
    ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);

  /*
    Extended longjmp-based error handler (with jpeg_pixels)
  */
  if (setjmp(error_manager.error_recovery))
    {
      /* Error handling code executed if longjmp was invoked */
      MagickFreeMemory(jpeg_pixels);
      jpeg_destroy_decompress(&jpeg_info);
      if (image->exception.severity > exception->severity)
        CopyException(exception,&image->exception);
      CloseBlob(image);
      number_pixels=image->columns*image->rows;
      if (number_pixels != 0)
        return(image);
      DestroyImage(image);
      return((Image *) NULL);
    }

  /*
    Convert JPEG pixels to pixel packets.
  */
  scanline[0]=(JSAMPROW) jpeg_pixels;
  for (y=0; y < (long) image->rows; y++)
    {
      register IndexPacket
	*indexes;

      register long
	x;

      register PixelPacket
	*q;

      q=SetImagePixels(image,0,y,image->columns,1);
      if (q == (PixelPacket *) NULL)
	{
	  status=MagickFail;
	  break;
	}
      indexes=AccessMutableIndexes(image);

      if (jpeg_read_scanlines(&jpeg_info,scanline,1) != 1)
	{
	  status=MagickFail;
	  break;
	}

      p=jpeg_pixels;

      if (jpeg_info.output_components == 1)
	{
	  for (x=0; x < (long) image->columns; x++)
	    {
	      index=(IndexPacket) (GETJSAMPLE(*p++));
	      VerifyColormapIndex(image,index);
	      indexes[x]=index;
	      *q++=image->colormap[index];
	    }
	}
      else
	{
	  if (jpeg_info.data_precision > 8)
	    {
	      for (x=0; x < (long) image->columns; x++)
		{
		  q->red=ScaleShortToQuantum(16*GETJSAMPLE(*p++));
		  q->green=ScaleShortToQuantum(16*GETJSAMPLE(*p++));
		  q->blue=ScaleShortToQuantum(16*GETJSAMPLE(*p++));
		  if (jpeg_info.output_components > 3)
		    q->opacity=ScaleShortToQuantum(16*GETJSAMPLE(*p++));
		  else
		    q->opacity=OpaqueOpacity;
		  q++;
		}
	    }
	  else
	    {
	      for (x=0; x < (long) image->columns; x++)
		{
		  q->red=ScaleCharToQuantum(GETJSAMPLE(*p++));
		  q->green=ScaleCharToQuantum(GETJSAMPLE(*p++));
		  q->blue=ScaleCharToQuantum(GETJSAMPLE(*p++));
		  if (jpeg_info.output_components > 3)
		    q->opacity=ScaleCharToQuantum(GETJSAMPLE(*p++));
		  else
		    q->opacity=OpaqueOpacity;
		  q++;
		}
	    }
	  if (image->colorspace == CMYKColorspace)
	    {
	      /*
		CMYK pixels are inverted.
	      */
	      q=AccessMutablePixels(image);
	      for (x=0; x < (long) image->columns; x++)
		{
		  q->red=MaxRGB-q->red;
		  q->green=MaxRGB-q->green;
		  q->blue=MaxRGB-q->blue;
		  q->opacity=MaxRGB-q->opacity;
		  q++;
		}
	    }
	}
      if (!SyncImagePixels(image))
	{
	  status=MagickFail;
	  break;
	}
      if (QuantumTick(y,image->rows))
	if (!MagickMonitorFormatted(y,image->rows,exception,LoadImageText,
				    image->filename,
				    image->columns,image->rows))
	  {
	    status=MagickFail;
	    jpeg_abort_decompress(&jpeg_info);
	    break;
	  }
    }
  /*
    Free jpeg resources.
  */
  if (status == MagickPass)
    {
      /*
	jpeg_finish_decompress() may throw an exception while it is
	finishing the remainder of the JPEG file.  At this point we
	have already decoded the image so we handle exceptions from
	jpeg_finish_decompress() specially, mapping reported
	exceptions as warnings rather than errors.  We try using
	jpeg_finish_decompress() and if it results in a longjmp(),
	then we skip over it again.
      */
      error_manager.completed=MagickTrue;
      if (!setjmp(error_manager.error_recovery))
	(void) jpeg_finish_decompress(&jpeg_info);
    }
  jpeg_destroy_decompress(&jpeg_info);
  MagickFreeMemory(jpeg_pixels);
  CloseBlob(image);

  /*
    Retrieve image orientation from EXIF (if present) and store in
    image.

    EXIF orienation enumerations match TIFF enumerations, which happen
    to match the enumeration values used by GraphicsMagick.
  */
  if (status == MagickPass)
    {
      const ImageAttribute
	*attribute;

      attribute = GetImageAttribute(image,"EXIF:Orientation");
      if ((attribute != (const ImageAttribute *) NULL) &&
	  (attribute->value != (char *) NULL))
	{
	  int
	    orientation;

	  orientation=MagickAtoI(attribute->value);
	  if ((orientation > UndefinedOrientation) &&
	      (orientation <= LeftBottomOrientation))
	    image->orientation=(OrientationType) orientation;
	}
    }
  if (image->logging) 
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"return");
  GetImageException(image,exception);
  return(image);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r J P E G I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterJPEGImage adds attributes for the JPEG image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterJPEGImage method is:
%
%      RegisterJPEGImage(void)
%
*/
ModuleExport void RegisterJPEGImage(void)
{
  static const char
    *description="Joint Photographic Experts Group JFIF format";

  static char
    version[MaxTextExtent];

  MagickInfo
    *entry;

  version[0]='\0';
#if defined(HasJPEG)
  FormatString(version,"IJG JPEG %d",JPEG_LIB_VERSION);
#endif

  entry=SetMagickInfo("JPEG");
  entry->thread_support=False; /* libjpeg is not thread safe */
#if defined(HasJPEG)
  entry->decoder=(DecoderHandler) ReadJPEGImage;
  entry->encoder=(EncoderHandler) WriteJPEGImage;
#endif
  entry->magick=(MagickHandler) IsJPEG;
  entry->adjoin=False;
  entry->description=description;
  if (version[0] != '\0')
    entry->version=version;
  entry->module="JPEG";
  entry->coder_class=PrimaryCoderClass;
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("JPG");
  entry->thread_support=False; /* libjpeg is not thread safe */
#if defined(HasJPEG)
  entry->decoder=(DecoderHandler) ReadJPEGImage;
  entry->encoder=(EncoderHandler) WriteJPEGImage;
#endif
  entry->adjoin=False;
  entry->description=description;
  if (version[0] != '\0')
    entry->version=version;
  entry->module="JPEG";
  entry->coder_class=PrimaryCoderClass;
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r J P E G I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterJPEGImage removes format registrations made by the
%  JPEG module from the list of supported formats.
%
%  The format of the UnregisterJPEGImage method is:
%
%      UnregisterJPEGImage(void)
%
*/
ModuleExport void UnregisterJPEGImage(void)
{
  (void) UnregisterMagickInfo("JPEG");
  (void) UnregisterMagickInfo("JPG");
}

#if defined(HasJPEG)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  W r i t e J P E G I m a g e                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteJPEGImage writes a JPEG image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the WriteJPEGImage method is:
%
%      unsigned int WriteJPEGImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows:
%
%    o status:  Method WriteJPEGImage return True if the image is written.
%      False is returned is there is of a memory shortage or if the image
%      file cannot be opened for writing.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o jpeg_image:  A pointer to an Image structure.
%
%
*/

static boolean EmptyOutputBuffer(j_compress_ptr cinfo)
{
  DestinationManager
    *destination;

  destination=(DestinationManager *) cinfo->dest;
  destination->manager.free_in_buffer=WriteBlob(destination->image,
    MaxBufferExtent,(char *) destination->buffer);
  if (destination->manager.free_in_buffer != MaxBufferExtent)
    ERREXIT(cinfo,JERR_FILE_WRITE);
  destination->manager.next_output_byte=destination->buffer;
  return(TRUE);
}

static void InitializeDestination(j_compress_ptr cinfo)
{
  DestinationManager
    *destination;

  destination=(DestinationManager *) cinfo->dest;
  destination->buffer=(JOCTET *) (*cinfo->mem->alloc_small)
    ((j_common_ptr) cinfo,JPOOL_IMAGE,MaxBufferExtent*sizeof(JOCTET));
  destination->manager.next_output_byte=destination->buffer;
  destination->manager.free_in_buffer=MaxBufferExtent;
}

static void TerminateDestination(j_compress_ptr cinfo)
{
  DestinationManager
    *destination;

  destination=(DestinationManager *) cinfo->dest;
  if ((MaxBufferExtent-(int) destination->manager.free_in_buffer) > 0)
    {
      unsigned long
        number_bytes;

      number_bytes=WriteBlob(destination->image,MaxBufferExtent-
        destination->manager.free_in_buffer,(char *) destination->buffer);
      if (number_bytes != (MaxBufferExtent-destination->manager.free_in_buffer))
        ERREXIT(cinfo,JERR_FILE_WRITE);
    }
}

static void WriteICCProfile(j_compress_ptr jpeg_info,Image *image)
{
  register long
    i,
    j;

  const unsigned char
    *color_profile;

  size_t
    profile_length;

  /*
    Retrieve color profile.
  */
  color_profile=GetImageProfile(image,"ICM",&profile_length);
  if (color_profile == ( const unsigned char *) NULL)
    return;

  /*
    Save color profile as a APP marker.
  */
  for (i=0; i < (long) profile_length; i+=65519)
  {
    unsigned char
      *profile;

    size_t
      length=0;


    length=Min(profile_length-i,65519);
    profile=MagickAllocateMemory(unsigned char *,length+14);
    if (profile == (unsigned char *) NULL)
      break;
    (void) strcpy((char *) profile,"ICC_PROFILE");
    profile[12]=(unsigned char) ((i/65519)+1);
    profile[13]=(profile_length/65519)+1;
    for (j=0; j < (long) length; j++)
      profile[j+14]=color_profile[i+j];
    jpeg_write_marker(jpeg_info,ICC_MARKER,profile,(unsigned int) length+14);
    MagickFreeMemory(profile);
  }
  if (image->logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
			  "ICC profile: %ld bytes",(long) profile_length);
}

static void WriteIPTCProfile(j_compress_ptr jpeg_info,Image *image)
{
  register long
    i;

  unsigned long
    roundup,
    tag_length;

  const unsigned char
    *iptc_profile;

  size_t
    profile_length;

  /*
    Retrieve IPTC profile from Image.
  */
  iptc_profile=GetImageProfile(image,"IPTC",&profile_length);
  if (iptc_profile == ( const unsigned char *) NULL)
    return;

  /*
    Save binary Photoshop resource data using an APP marker.
  */
#ifdef GET_ONLY_IPTC_DATA
  tag_length=26;
#else
  tag_length=14;
#endif
  for (i=0; i < (long) profile_length; i+=65500)
  {
    size_t
      length;
    
    unsigned char
      *profile;

    length=Min(profile_length-i,65500);
    roundup=(length & 0x01); /* round up for Photoshop */
    profile=MagickAllocateMemory(unsigned char *,length+roundup+tag_length);
    if (profile == (unsigned char *) NULL)
      break;
#ifdef GET_ONLY_IPTC_DATA
    (void) memcpy(profile,"Photoshop 3.0 8BIM\04\04\0\0\0\0",24);
    profile[13]=0x00;
    profile[24]=length >> 8;
    profile[25]=length & 0xff;
#else
    (void) memcpy(profile,"Photoshop 3.0 ",14);
    profile[13]=0x00;
#endif
    (void) memcpy(&(profile[tag_length]),&(iptc_profile[i]),length);
    if (roundup)
      profile[length+tag_length]=0;
    jpeg_write_marker(jpeg_info,IPTC_MARKER,profile,(unsigned int)
      (length+roundup+tag_length));
    MagickFreeMemory(profile);
  }

  if (image->logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
			  "IPTC profile: %ld bytes",(long) profile_length);
}

static void JPEGDestinationManager(j_compress_ptr cinfo,Image * image)
{
  DestinationManager
    *destination;

  cinfo->dest=(struct jpeg_destination_mgr *) (*cinfo->mem->alloc_small)
    ((j_common_ptr) cinfo,JPOOL_IMAGE,sizeof(DestinationManager));
  destination=(DestinationManager *) cinfo->dest;
  destination->manager.init_destination=InitializeDestination;
  destination->manager.empty_output_buffer=EmptyOutputBuffer;
  destination->manager.term_destination=TerminateDestination;
  destination->image=image;
}

static MagickPassFail WriteJPEGImage(const ImageInfo *image_info,Image *image)
{
  ErrorManager
    error_manager;

  const ImageAttribute
    *attribute;

  JSAMPLE
    *jpeg_pixels;

  JSAMPROW
    scanline[1];

  char
   *sampling_factors,
   *preserve_settings;

  long
    y;

  register const PixelPacket
    *p;

  register JSAMPLE
    *q;

  register long
    i,
    x;

  struct jpeg_compress_struct
    jpeg_info;

  struct jpeg_error_mgr
    jpeg_error;

  MagickPassFail
    status;

  unsigned long
    input_colorspace;

  unsigned long
    quality;

  magick_int64_t
    huffman_memory;

  ImageCharacteristics
    characteristics;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenError,UnableToOpenFile,image);

  (void) memset(&error_manager,0,sizeof(error_manager));
  (void) memset(&jpeg_info,0,sizeof(jpeg_info));
  (void) memset(&jpeg_error,0,sizeof(jpeg_error));

  /*
    Transform image to user-requested colorspace.
  */
  if (UndefinedColorspace != image_info->colorspace)
    {
      (void) TransformColorspace(image,image_info->colorspace);
    }
  /*
    Convert RGB-compatible colorspaces (e.g. CineonLog) to RGB by
    default.  User can still override it by explicitly specifying the
    desired colorspace.
  */
  else if (IsRGBCompatibleColorspace(image->colorspace) &&
	   !IsRGBColorspace(image->colorspace))
    {
      (void) TransformColorspace(image,RGBColorspace);
    }
  
  /*
    Analyze image to be written.
  */
  if (!GetImageCharacteristics(image,&characteristics,
                               (OptimizeType == image_info->type),
                               &image->exception))
    {
      CloseBlob(image);
      return MagickFail;
    }

  /*
    Set initial longjmp based error handler.
  */
  jpeg_info.client_data=(void *) image;
  jpeg_info.err=jpeg_std_error(&jpeg_error);
  jpeg_info.err->emit_message=(void (*)(j_common_ptr,int)) JPEGMessageHandler;
  jpeg_info.err->error_exit=(void (*)(j_common_ptr)) JPEGErrorHandler;
  error_manager.image=image;
  if (setjmp(error_manager.error_recovery))
    {
      jpeg_destroy_compress(&jpeg_info);
      CloseBlob(image);
      return MagickFail ;
    }
  jpeg_info.client_data=(void *) &error_manager;

  jpeg_create_compress(&jpeg_info);
  JPEGDestinationManager(&jpeg_info,image);
  jpeg_info.image_width=(unsigned int) image->columns;
  jpeg_info.image_height=(unsigned int) image->rows;
  jpeg_info.input_components=3;
  jpeg_info.in_color_space=JCS_RGB;

  /*
    Set JPEG colorspace as per user request.
  */
  {
    MagickBool
      colorspace_set=MagickFalse;
    
    if (IsCMYKColorspace(image_info->colorspace))
      {
	jpeg_info.input_components=4;
	jpeg_info.in_color_space=JCS_CMYK;
	colorspace_set=MagickTrue;
      }
    else if (IsYCbCrColorspace(image_info->colorspace))
      {
	jpeg_info.input_components=3;
	jpeg_info.in_color_space=JCS_YCbCr;
	colorspace_set=MagickTrue;
      }
    else if (IsGrayColorspace(image_info->colorspace))
      {
	jpeg_info.input_components=1;
	jpeg_info.in_color_space=JCS_GRAYSCALE;
	colorspace_set=MagickTrue;
      }

    if (!colorspace_set)
      {
	if (IsCMYKColorspace(image->colorspace))
	  {
	    jpeg_info.input_components=4;
	    jpeg_info.in_color_space=JCS_CMYK;
	  }
	else if (IsYCbCrColorspace(image->colorspace))
	  {
	    jpeg_info.input_components=3;
	    jpeg_info.in_color_space=JCS_YCbCr;
	  }
	else if ((IsGrayColorspace(image->colorspace) ||
		  (characteristics.grayscale)))
	  {
	    jpeg_info.input_components=1;
	    jpeg_info.in_color_space=JCS_GRAYSCALE;
	  }
	else
	  {
	    jpeg_info.input_components=3;
	    jpeg_info.in_color_space=JCS_RGB;
	  }
      }
  }

  input_colorspace=UndefinedColorspace;
  quality=image_info->quality;
  /* Check for -define jpeg:preserve-settings */
  /* ImageMagick:
    GetImageOption();
  */
  /* GraphicsMagick */
  preserve_settings=(char *) AccessDefinition(image_info,"jpeg",
     "preserve-settings");

  sampling_factors=image_info->sampling_factor;

  if (preserve_settings)
    {
      if (image->logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "  JPEG:preserve-settings flag is defined.");

      /* Retrieve input file quality */
      attribute=GetImageAttribute(image,"JPEG-Quality");
      if ((attribute != (const ImageAttribute *) NULL) &&
          (attribute->value != (char *) NULL))
        {
          (void) sscanf(attribute->value,"%lu",&quality);
          if (image->logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                "  Input quality=%lu",quality);
        }

      /* Retrieve input file colorspace */
      attribute=GetImageAttribute(image,"JPEG-Colorspace");
      if ((attribute != (const ImageAttribute *) NULL) &&
          (attribute->value != (char *) NULL))
        {
          (void) sscanf(attribute->value,"%lu",&input_colorspace);
          if (image->logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                "  Input colorspace=%lu",input_colorspace);
        }

      if (input_colorspace == (unsigned long) jpeg_info.in_color_space)
        {
          /* Retrieve input sampling factors */
          attribute=GetImageAttribute(image,"JPEG-Sampling-factors");
          if ((attribute != (const ImageAttribute *) NULL) &&
              (attribute->value != (char *) NULL))
            {
              sampling_factors=attribute->value;
              if (image->logging)
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                    "  Input sampling-factors=%s",sampling_factors);
            }
        }
      else
        {
          if (image->logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                "  Input colorspace (%lu) != Output colorspace (%d)",
                input_colorspace,jpeg_info.in_color_space);
        }
    }

  jpeg_set_defaults(&jpeg_info);

  /*
    Determine bit depth.
  */
  {
    int
      sample_size;
    
    sample_size=sizeof(JSAMPLE)*8;
    if (sample_size > 8)
      sample_size=12;
    if ((jpeg_info.data_precision != 12) &&
        (image->depth <= 8))
      sample_size=8;
    jpeg_info.data_precision=sample_size;
  }
  if ((image->x_resolution == 0) || (image->y_resolution == 0))
    {
      image->x_resolution=72.0;
      image->y_resolution=72.0;
      image->units=PixelsPerInchResolution;
    }
  if (image_info->density != (char *) NULL)
    {
      int
        count;

      /* FIXME: density should not be set via image_info->density
         but removing this support may break some applications. */
      count=GetMagickDimension(image_info->density,&image->x_resolution,
        &image->y_resolution,NULL,NULL);
      if (count == 1 )
        image->y_resolution=image->x_resolution;
    }
  jpeg_info.density_unit=1;  /* default to DPI */
  if (image->logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "Image resolution: %ld,%ld",(long) image->x_resolution,
      (long) image->y_resolution);
  if ((image->x_resolution != 0) && (image->y_resolution != 0))
    {
      /*
        Set image resolution.
      */
      jpeg_info.write_JFIF_header=True;
      jpeg_info.X_density=(short) image->x_resolution;
      jpeg_info.Y_density=(short) image->y_resolution;
      if (image->units == PixelsPerInchResolution)
        jpeg_info.density_unit=1;
      if (image->units == PixelsPerCentimeterResolution)
        jpeg_info.density_unit=2;
    }

  {
    const char
      *value;

    /*
      Allow the user to select the DCT encoding algorithm.
    */
    if ((value=AccessDefinition(image_info,"jpeg","dct-method")))
      {
        if (LocaleCompare(value,"ISLOW") == 0)
          jpeg_info.dct_method=JDCT_ISLOW;
        else if (LocaleCompare(value,"IFAST") == 0)
          jpeg_info.dct_method=JDCT_IFAST;
        else if (LocaleCompare(value,"FLOAT") == 0)
          jpeg_info.dct_method=JDCT_FLOAT;
        else if (LocaleCompare(value,"DEFAULT") == 0)
          jpeg_info.dct_method=JDCT_DEFAULT;
        else if (LocaleCompare(value,"FASTEST") == 0)
          jpeg_info.dct_method=JDCT_FASTEST;
      }
  }

{
  const char
    *value;
  
  huffman_memory=0;
  if ((value=AccessDefinition(image_info,"jpeg","optimize-coding")))
    {
      if (LocaleCompare(value,"FALSE") == 0)
        jpeg_info.optimize_coding=MagickFalse;
      else
        jpeg_info.optimize_coding=MagickTrue;
    }
  else
    {
      /*
        Huffman optimization requires that the whole image be buffered in
        memory.  Since this is such a large consumer, obtain a memory
        resource for the memory to be consumed.  If the memory resource
        fails to be acquired, then don't enable huffman optimization.
      */
      huffman_memory=(magick_int64_t) jpeg_info.input_components*
        image->columns*image->rows*sizeof(JSAMPLE);
      jpeg_info.optimize_coding=AcquireMagickResource(MemoryResource,huffman_memory);
    }
  if (!jpeg_info.optimize_coding)
    huffman_memory=0;
  if (image->logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "Huffman optimization is %s",
                          (jpeg_info.optimize_coding ? "enabled" : "disabled"));
 }

#if (JPEG_LIB_VERSION >= 61) && defined(C_PROGRESSIVE_SUPPORTED)
 if (image_info->interlace == LineInterlace)
   jpeg_simple_progression(&jpeg_info);
 if (image->logging)
   {
     if (image_info->interlace == LineInterlace)
       (void) LogMagickEvent(CoderEvent,GetMagickModule(),
			     "Interlace: progressive");
     else
       (void) LogMagickEvent(CoderEvent,GetMagickModule(),
			     "Interlace: nonprogressive");
   }
#else
  if (image->logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "Interlace:  nonprogressive");
#endif
  if ((image->compression == LosslessJPEGCompression) ||
      (quality > 100))
    {
#if defined(C_LOSSLESS_SUPPORTED)
      if (quality < 100)
        ThrowException(&image->exception,CoderWarning,LosslessToLossyJPEGConversion,(char *) NULL);
      else
        {
          int
            point_transform,
            predictor;

          predictor=quality/100;  /* range 1-7 */
          point_transform=quality % 20;  /* range 0-15 */
          jpeg_simple_lossless(&jpeg_info,predictor,point_transform);
          if (image->logging)
            {
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                "Compression: lossless");
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                "Predictor: %d",predictor);
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                "Point Transform: %d",point_transform);
            }
        }
#else
        {
          jpeg_set_quality(&jpeg_info,100,True);
          if (image->logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Quality: 100");
        }
#endif
    }
  else
    {
      jpeg_set_quality(&jpeg_info,(int) quality,True);
      if (image->logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Quality: %lu",
          quality);
    }
  if (sampling_factors != (char *) NULL)
    {
      double
        hs[4]={1.0, 1.0, 1.0, 1.0}, 
        vs[4]={1.0, 1.0, 1.0, 1.0};

      long
        count;

      /*
        Set sampling factors.
      */
      count=sscanf(sampling_factors,"%lfx%lf,%lfx%lf,%lfx%lf,%lfx%lf",
        &hs[0],&vs[0],&hs[1],&vs[1],&hs[2],&vs[2],&hs[3],&vs[3]);

      if (count%2 == 1)
        vs[count/2]=hs[count/2];

      for (i=0; i < 4; i++)
      {
        jpeg_info.comp_info[i].h_samp_factor=(int) hs[i];
        jpeg_info.comp_info[i].v_samp_factor=(int) vs[i];
      }
      for (; i < MAX_COMPONENTS; i++)
      {
        jpeg_info.comp_info[i].h_samp_factor=1;
        jpeg_info.comp_info[i].v_samp_factor=1;
      }
    }
   else
    {
      if (quality >= 90)
        for (i=0; i < MAX_COMPONENTS; i++)
        {
          jpeg_info.comp_info[i].h_samp_factor=1;
          jpeg_info.comp_info[i].v_samp_factor=1;
        }
    }

  if (image->logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Starting JPEG compression");
  jpeg_start_compress(&jpeg_info,True);
  if (image->logging)
    {
      if (image->storage_class == PseudoClass)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "Storage class: PseudoClass");
      else
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "Storage class: DirectClass");
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Depth: %u",
        image->depth);
      if (image->colors)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "Number of colors: %u",image->colors);
      else
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "Number of colors: unspecified");
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "JPEG data precision: %d",(int) jpeg_info.data_precision);
      if (IsCMYKColorspace(image_info->colorspace))
	{
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
				"Storage class: DirectClass");
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
				"Colorspace: CMYK");
	}
      else if (IsYCbCrColorspace(image_info->colorspace))
	{
	  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
				"Colorspace: YCbCr");
	}
      if (IsCMYKColorspace(image->colorspace))
	{
	  /* A CMYK space */
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
				"Colorspace: CMYK");
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
				"Sampling factors: %dx%d,%dx%d,%dx%d,%dx%d",
				jpeg_info.comp_info[0].h_samp_factor,
				jpeg_info.comp_info[0].v_samp_factor,
				jpeg_info.comp_info[1].h_samp_factor,
				jpeg_info.comp_info[1].v_samp_factor,
				jpeg_info.comp_info[2].h_samp_factor,
				jpeg_info.comp_info[2].v_samp_factor,
				jpeg_info.comp_info[3].h_samp_factor,
				jpeg_info.comp_info[3].v_samp_factor);
	}
      else if (IsGrayColorspace(image->colorspace))
	{
	  /* A gray space */
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
				"Colorspace: GRAY");
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
				"Sampling factors: %dx%d",jpeg_info.comp_info[0].h_samp_factor,
				jpeg_info.comp_info[0].v_samp_factor);
	}
      else if (IsRGBCompatibleColorspace(image->colorspace))
	{
	  /* An RGB space */
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
				" Image colorspace is RGB");
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
				"Sampling factors: %dx%d,%dx%d,%dx%d",
				jpeg_info.comp_info[0].h_samp_factor,
				jpeg_info.comp_info[0].v_samp_factor,
				jpeg_info.comp_info[1].h_samp_factor,
				jpeg_info.comp_info[1].v_samp_factor,
				jpeg_info.comp_info[2].h_samp_factor,
				jpeg_info.comp_info[2].v_samp_factor);
	}
      else if (IsYCbCrColorspace(image->colorspace))
	{
	  /* A YCbCr space */
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
				"Colorspace: YCbCr");
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
				"Sampling factors: %dx%d,%dx%d,%dx%d",
				jpeg_info.comp_info[0].h_samp_factor,
				jpeg_info.comp_info[0].v_samp_factor,
				jpeg_info.comp_info[1].h_samp_factor,
				jpeg_info.comp_info[1].v_samp_factor,
				jpeg_info.comp_info[2].h_samp_factor,
				jpeg_info.comp_info[2].v_samp_factor);
	}
      else
	{
	  /* Some other color space */
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Colorspace: %d",
				image->colorspace);
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
				"Sampling factors: %dx%d,%dx%d,%dx%d,%dx%d",
				jpeg_info.comp_info[0].h_samp_factor,
				jpeg_info.comp_info[0].v_samp_factor,
				jpeg_info.comp_info[1].h_samp_factor,
				jpeg_info.comp_info[1].v_samp_factor,
				jpeg_info.comp_info[2].h_samp_factor,
				jpeg_info.comp_info[2].v_samp_factor,
				jpeg_info.comp_info[3].h_samp_factor,
				jpeg_info.comp_info[3].v_samp_factor);
	}
    }
  /*
    Write JPEG profiles.
  */
  attribute=GetImageAttribute(image,"comment");
  if ((attribute != (const ImageAttribute *) NULL) &&
    (attribute->value != (char *) NULL))
    for (i=0; i < (long) strlen(attribute->value); i+=65533L)
      jpeg_write_marker(&jpeg_info,JPEG_COM,(unsigned char *) attribute->value+
        i,(int) Min(strlen(attribute->value+i),65533L));
  WriteICCProfile(&jpeg_info,image);
  WriteIPTCProfile(&jpeg_info,image);
  {
    const char
      *profile_name;
    
    size_t
      profile_length;
    
    const unsigned char *
      profile_info;
    
    ImageProfileIterator
      profile_iterator;

    profile_iterator=AllocateImageProfileIterator(image);
    while(NextImageProfile(profile_iterator,&profile_name,&profile_info,
                           &profile_length) != MagickFail)
      {
        register long
          j;

        if (LocaleNCompare(profile_name,"APP",3) != 0 &&
            LocaleNCompare(profile_name,"Exif",4) != 0)
          continue;
        x=1;
        if (LocaleNCompare(profile_name,"APP",3) == 0)
          x=MagickAtoL(profile_name+3);
        if (image->logging)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Profile: %s, %lu bytes",profile_name,
                                (unsigned long) profile_length);
        for (j=0; j < (long) profile_length; j+=65533L)
          jpeg_write_marker(&jpeg_info,JPEG_APP0+(int) x,
                            profile_info+j,(int)
                            Min(profile_length-j,65533L));
      }
    DeallocateImageProfileIterator(profile_iterator);
  }
  /*
    Convert MIFF to JPEG raster pixels.
  */
  jpeg_pixels=MagickAllocateArray(JSAMPLE *,
    jpeg_info.input_components*image->columns,sizeof(JSAMPLE));
  if (jpeg_pixels == (JSAMPLE *) NULL)
    {
      if (huffman_memory)
        LiberateMagickResource(MemoryResource,huffman_memory);
      ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
    }
  scanline[0]=(JSAMPROW) jpeg_pixels;
  if (jpeg_info.data_precision > 8)
    {
      if (jpeg_info.in_color_space == JCS_GRAYSCALE)
        {
	  if (image->logging)
	    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
				  "Writing %d bit JCS_GRAYSCALE samples",
				  jpeg_info.data_precision);
          for (y=0; y < (long) image->rows; y++)
            {
              p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
              if (p == (const PixelPacket *) NULL)
                break;
              q=jpeg_pixels;
              if (image->is_grayscale)
                {
                  for (x=0; x < (long) image->columns; x++)
                    {
                      *q++=(JSAMPLE) (ScaleQuantumToShort(GetGraySample(p))/16U);
                      p++;
                    }
                }
              else
                {
                  for (x=0; x < (long) image->columns; x++)
                    {
                      *q++=(JSAMPLE) (ScaleQuantumToShort(PixelIntensityToQuantum(p))/16U);
                      p++;
                    }
                }
              (void) jpeg_write_scanlines(&jpeg_info,scanline,1);
              if (QuantumTick(y,image->rows))
                if (!MagickMonitorFormatted(y,image->rows,&image->exception,
                                            SaveImageText,image->filename,
					    image->columns,image->rows))
                  break;
            }
        }
      else
        if ((jpeg_info.in_color_space == JCS_RGB) ||
            (jpeg_info.in_color_space == JCS_YCbCr))
          {
	    if (image->logging)
	      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
				    "Writing %d bit JCS_RGB or JCS_YCbCr samples",
				    jpeg_info.data_precision);
            for (y=0; y < (long) image->rows; y++)
              {
                p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
                if (p == (const PixelPacket *) NULL)
                  break;
                q=jpeg_pixels;
                for (x=0; x < (long) image->columns; x++)
                  {
                    *q++=(JSAMPLE) (ScaleQuantumToShort(p->red)/16);
                    *q++=(JSAMPLE) (ScaleQuantumToShort(p->green)/16);
                    *q++=(JSAMPLE) (ScaleQuantumToShort(p->blue)/16);
                    p++;
                  }
                (void) jpeg_write_scanlines(&jpeg_info,scanline,1);
                if (QuantumTick(y,image->rows))
                  if (!MagickMonitorFormatted(y,image->rows,&image->exception,
                                              SaveImageText,image->filename,
					      image->columns,image->rows))
                    break;
              }
          }
        else
          {
	    if (image->logging)
	      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
				    "Writing %d bit JCS_CMYK samples",
				    jpeg_info.data_precision);
            for (y=0; y < (long) image->rows; y++)
              {
                p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
                if (p == (const PixelPacket *) NULL)
                  break;
                q=jpeg_pixels;
                for (x=0; x < (long) image->columns; x++)
                  {
                    /*
                      Convert DirectClass packets to contiguous CMYK scanlines.
                    */
                    *q++=(JSAMPLE) (4095-ScaleQuantumToShort(p->red)/16);
                    *q++=(JSAMPLE) (4095-ScaleQuantumToShort(p->green)/16);
                    *q++=(JSAMPLE) (4095-ScaleQuantumToShort(p->blue)/16);
                    *q++=(JSAMPLE) (4095-ScaleQuantumToShort(p->opacity)/16);
                    p++;
                  }
                (void) jpeg_write_scanlines(&jpeg_info,scanline,1);
                if (QuantumTick(y,image->rows))
                  if (!MagickMonitorFormatted(y,image->rows,&image->exception,
                                              SaveImageText,image->filename,
					      image->columns,image->rows))
                    break;
              }
          }
    }
  else
    if (jpeg_info.in_color_space == JCS_GRAYSCALE)
      {
	if (image->logging)
	  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
				"Writing %d bit JCS_GRAYSCALE samples",
				jpeg_info.data_precision);
        for (y=0; y < (long) image->rows; y++)
          {
            p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
            if (p == (const PixelPacket *) NULL)
              break;
            q=jpeg_pixels;
            if (image->is_grayscale)
                {
                  for (x=0; x < (long) image->columns; x++)
                  {
                    *q++=(JSAMPLE) ScaleQuantumToChar(GetGraySample(p));
                    p++;
                  }
                }
            else
              {
                for (x=0; x < (long) image->columns; x++)
                  {
                    *q++=(JSAMPLE) ScaleQuantumToChar(PixelIntensityToQuantum(p));
                    p++;
                  }
              }
            (void) jpeg_write_scanlines(&jpeg_info,scanline,1);
            if (QuantumTick(y,image->rows))
              if (!MagickMonitorFormatted(y,image->rows,&image->exception,
                                          SaveImageText,image->filename,
					  image->columns,image->rows))
                break;
          }
      }
    else
      if ((jpeg_info.in_color_space == JCS_RGB) ||
          (jpeg_info.in_color_space == JCS_YCbCr))
        {
	  if (image->logging)
	    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
				  "Writing %d bit JCS_RGB or JCS_YCbCr samples",
				  jpeg_info.data_precision);
          for (y=0; y < (long) image->rows; y++)
            {
              p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
              if (p == (const PixelPacket *) NULL)
                break;
              q=jpeg_pixels;
              for (x=0; x < (long) image->columns; x++)
                {
                  *q++=(JSAMPLE) ScaleQuantumToChar(p->red);
                  *q++=(JSAMPLE) ScaleQuantumToChar(p->green);
                  *q++=(JSAMPLE) ScaleQuantumToChar(p->blue);
                  p++;
                }
              (void) jpeg_write_scanlines(&jpeg_info,scanline,1);
              if (QuantumTick(y,image->rows))
                if (!MagickMonitorFormatted(y,image->rows,&image->exception,
                                            SaveImageText,image->filename,
					    image->columns,image->rows))
                  break;
            }
        }
      else
        {
	  if (image->logging)
	    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
				  "Writing %d bit JCS_CMYK samples",
				  jpeg_info.data_precision);
          for (y=0; y < (long) image->rows; y++)
            {
              p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
              if (p == (const PixelPacket *) NULL)
                break;
              q=jpeg_pixels;
              for (x=0; x < (long) image->columns; x++)
                {
                  /*
                    Convert DirectClass packets to contiguous CMYK scanlines.
                  */
                  *q++=(JSAMPLE) (ScaleQuantumToChar(MaxRGB-p->red));
                  *q++=(JSAMPLE) (ScaleQuantumToChar(MaxRGB-p->green));
                  *q++=(JSAMPLE) (ScaleQuantumToChar(MaxRGB-p->blue));
                  *q++=(JSAMPLE) (ScaleQuantumToChar(MaxRGB-p->opacity));
                  p++;
                }
              (void) jpeg_write_scanlines(&jpeg_info,scanline,1);
              if (QuantumTick(y,image->rows))
                if (!MagickMonitorFormatted(y,image->rows,&image->exception,
                                            SaveImageText,image->filename,
					    image->columns,image->rows))
                  break;
            }
        }
  if (image->logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Finishing JPEG compression");
  jpeg_finish_compress(&jpeg_info);
  /*
    Free memory.
  */
  if (huffman_memory)
    LiberateMagickResource(MemoryResource,huffman_memory);
  jpeg_destroy_compress(&jpeg_info);
  MagickFreeMemory(jpeg_pixels);
  CloseBlob(image);
  return(True);
}
#endif
