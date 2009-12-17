/*
% Copyright (C) 2009 GraphicsMagick Group
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                          CCCC   AAA   L     SSSSS                           %
%                         C      A   A  L     SS                              %
%                         C      AAAAA  L       SSS                           %
%                         C      A   A  L        SS                           %
%                          CCCC  A   A  LLLLL SSSSS                           %
%                                                                             %
%                                                                             %
%                 Read/Write CALS (CALS type 1) format                        %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                               John Sergeant                                 %
%                                  May 2009                                   %
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
#include "magick/blob.h"
#include "magick/compress.h"
#include "magick/constitute.h"
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/tempfile.h"
#include "magick/utility.h"
/* 
   Write an unsigned long to file with Intel/LSB ordering
*/
static void CALS_WriteIntelULong(FILE *file,unsigned long ul)
{
  fputc((unsigned char)ul,file);
  fputc((unsigned char)(ul >> 8),file);
  fputc((unsigned char)(ul >> 16),file);
  fputc((unsigned char)(ul >> 24),file);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s C A L S                                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsCALS returns MagickTrue if the image format type, identified by the
%  magick string, is CAL.
%
%  The format of the IsCALS method is:
%
%      unsigned int IsCALS(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsCALS returns MagickTrue if the image format type is CAL.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static MagickBool IsCALS(const unsigned char *magick,const size_t length)
{
  if (length < 132)
    return(MagickFalse);
  if (LocaleNCompare((char *) (magick),"version: MIL-STD-1840",21) == 0)
    return(MagickTrue);
  if (LocaleNCompare((char *) (magick),"srcdocid:",9) == 0)
    return(MagickTrue);
  if (LocaleNCompare((char *) (magick),"rorient:",8) == 0)
    return(MagickTrue);
  return(MagickFalse);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d C A L S I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadCALSImage reads a CALS type 1 image file and returns it. It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadCALSImage method is:
%
%      Image *ReadCALSImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadCALSImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadCALSImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image
    *image;

  long
    y;

  char
    record[129];  

  unsigned long
    status,
    width,
    height,
    rtype,
    orient,
    density;

  char
    filename[MaxTextExtent];

  FILE
    *file;

  unsigned long
    byte_count_pos,
    strip_off_pos,
    flen;

  int
    c;

  ImageInfo
    *clone_info;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFail)
    ThrowReaderException(FileOpenError,UnableToOpenFile,image);

  /*
    Scan CAL header, record by record, looking for mandatory fields
  */ 
  rtype = 1;
  width = height = 0;
  orient = 1;
  density = 200;
  record[128]=0;
  for (y = 0; y < 16; y++)
    {
      ReadBlob(image,128,(char *) record);
      if (LocaleNCompare(record,"rtype:",6) == 0)
        { /* rtype */
          sscanf(record+6,"%ld",&rtype);
        }
      else
      if (LocaleNCompare(record,"rorient:",8) == 0)
        { /* rorient */
          unsigned long
            pel_path_rot,
            line_rot;

          pel_path_rot = line_rot = 0;
          sscanf(record+8,"%ld,%ld",&pel_path_rot,&line_rot);
          switch (pel_path_rot)
            {
              case 90:
                orient = 5;
                break;
              case 180:
                orient = 3;
                break;
              case 270:
                orient = 7;
                break;
              default:
                orient = 1;
            }
          if (line_rot == 90) orient++;
        }
      else
      if (LocaleNCompare(record,"rpelcnt:",8) == 0)
        { /* replcnt */
          sscanf(record+8,"%ld,%ld",&width,&height);
        }
     else
     if (LocaleNCompare(record,"rdensty:",8) == 0)
        { /* rdensty */
          sscanf(record+8,"%ld",&density);
          if (!density) density = 200;
        }
    }
  if ((!width) || (!height) || (rtype != 1))
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);

  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
			"Dimensions %lux%lu",width,height);

  /* Create TIFF wrapper to handle file data using TIFF library */
  file=AcquireTemporaryFileStream(filename,BinaryFileIOMode);
  if (file == (FILE *) NULL)
    ThrowReaderTemporaryFileException(filename);

  /* Intel TIFF with IFD at offset 8 - IFD has 14 records */
  fwrite("\111\111\052\000\010\000\000\000\016\000",1,10,file);
  /* New sub image - normal type */
  fwrite("\376\000\003\000\001\000\000\000\000\000\000\000",1,12,file);
  /* Image width */
  fwrite("\000\001\004\000\001\000\000\000",1,8,file);
  CALS_WriteIntelULong(file,width);
  /* Image height */
  fwrite("\001\001\004\000\001\000\000\000",1,8,file);
  CALS_WriteIntelULong(file,height);
  /* 1 bit per sample */
  fwrite("\002\001\003\000\001\000\000\000\001\000\000\000",1,12,file);
  /* CCITT Group 4 compression */
  fwrite("\003\001\003\000\001\000\000\000\004\000\000\000",1,12,file);
  /* Photometric interpretation MAX BLACK */
  fwrite("\006\001\003\000\001\000\000\000\000\000\000\000",1,12,file);
  /* Strip offset */
  fwrite("\021\001\003\000\001\000\000\000",1,8,file);
  strip_off_pos = 10 + (12 * 14) + 4 + 8;
  CALS_WriteIntelULong(file,strip_off_pos);
  /* Orientation */
  fwrite("\022\001\003\000\001\000\000\000",1,8,file);
  CALS_WriteIntelULong(file,orient);
  /* 1 sample per pixel */
  fwrite("\025\001\003\000\001\000\000\000\001\000\000\000",1,12,file);
  /* Rows per strip (same as height) */
  fwrite("\026\001\004\000\001\000\000\000",1,8,file);
  CALS_WriteIntelULong(file,height);
  /* Strip byte count */
  fwrite("\027\001\004\000\001\000\000\000\000\000\000\000",1,12,file);
  byte_count_pos = ftell(file)-4;
  /* X resolution */
  fwrite("\032\001\005\000\001\000\000\000",1,8,file);
  CALS_WriteIntelULong(file,strip_off_pos-8);
  /* Y resolution */
  fwrite("\033\001\005\000\001\000\000\000",1,8,file);
  CALS_WriteIntelULong(file,strip_off_pos-8);
  /* Resolution unit is inch */
  fwrite("\050\001\003\000\001\000\000\000\002\000\000\000",1,12,file);
  /* Offset to next IFD ie end of images */
  fwrite("\000\000\000\000",1,4,file);
  /* Write X/Y resolution as rational data */
  CALS_WriteIntelULong(file,density);
  CALS_WriteIntelULong(file,1);

  /* Copy image stream data */
  flen = 0;
  c=ReadBlobByte(image);
  while (c != EOF)
    {
      flen++;
      (void) fputc(c,file);
      c=ReadBlobByte(image);
    }

  /* Return to correct location and output strip byte count */
  fseek(file,byte_count_pos,SEEK_SET);
  CALS_WriteIntelULong(file,flen);

  (void) fclose(file);
  DestroyImage(image);
  clone_info=CloneImageInfo(image_info);
  clone_info->blob=(void *) NULL;
  clone_info->length=0;
  FormatString(clone_info->filename,"tiff:%.1024s",filename);
  image=ReadImage(clone_info,exception);
  (void) LiberateTemporaryFile(filename);
  DestroyImageInfo(clone_info);
  if (image != (Image *) NULL)
    {
      (void) strlcpy(image->filename,image_info->filename,
		     sizeof(image->filename));
      (void) strlcpy(image->magick_filename,image_info->filename,
		     sizeof(image->magick_filename));
      (void) strlcpy(image->magick,"CALS",sizeof(image->magick));
    }
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e C A L S I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteCALSImage writes an image in the CALS type I image format
%
%  The format of the WriteCALSImage method is:
%
%      unsigned int WriteCALSImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteCALSImage return MagickTrue if the image is written.
%      MagickFail is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
%
*/
static void WriteCALSRecord(Image *image,const char *data)
{
  int
    i;

  const char
    *p;

  char
    pad[128];

  i = 0;
  if (data)
    {
      /* Limit output to 128 chars */
      p=data;
      while ((i < 128) && (p[i])) i++;
      WriteBlob(image,i,data);
    }
  if (i < 128)
    {
      /* Pad record to 128 characters using trailing spaces */
      i=128-i;
      memset(pad,' ',i);
      WriteBlob(image,i,pad);
    }
}

static MagickPassFail WriteCALSImage(const ImageInfo *image_info,Image *image)
{
  char
    buffer[MaxTextExtent];

  int
    i;

  long
    sans;

  unsigned long
    density;

  int
    orx,
    ory;

  MagickPassFail
    status=MagickPass;

  /*
    Validate input image
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  /*
    Open output image file.
  */
  if (OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception) == MagickFail)
    ThrowWriterException(FileOpenError,UnableToOpenFile,image);

  /*
    Create standard header
  */
  WriteCALSRecord(image,"srcdocid: NONE");
  WriteCALSRecord(image,"dstdocid: NONE");
  WriteCALSRecord(image,"txtfilid: NONE");
  WriteCALSRecord(image,"figid: NONE");
  WriteCALSRecord(image,"srcgph: NONE");
  WriteCALSRecord(image,"docls: NONE");
  WriteCALSRecord(image,"rtype: 1");
  /* orientation based on input or default of upright */
  switch (image->orientation)
  {
    case TopRightOrientation:
      orx=180;
      ory=270;
      break;
    case BottomRightOrientation:
      orx=180;
      ory=90;
      break;
    case BottomLeftOrientation:
      orx=0;
      ory=90;
      break;
    case LeftTopOrientation:
      orx=270;
      ory=0;
      break;
    case RightTopOrientation:
      orx=270;
      ory=180;
      break;
    case RightBottomOrientation:
      orx=90;
      ory=180;
      break;
    case LeftBottomOrientation:
      orx=90;
      ory=0;
      break;
    default:
      orx=0; 
      ory=270;
  }
  FormatString(buffer,"rorient: %03d,%03d",orx,ory);
  WriteCALSRecord(image,buffer);
  /* pixel counts based on columns/rows of input */
  FormatString(buffer,"rpelcnt: %06ld,%06ld",image->columns,image->rows);
  WriteCALSRecord(image,buffer);  
  /* density based on input density or default of 200 */
  density=200;
  if (image_info->density != (char *) NULL)
    (void) GetGeometry(image_info->density,&sans,&sans,&density,&density);
  FormatString(buffer,"rdensty: %04ld",density);
  WriteCALSRecord(image,buffer);
  WriteCALSRecord(image,"notes: NONE");

  /*
    Pad header to make 16 records / 2048 bytes
  */
  memset(buffer,' ',128);
  for (i = 0; i < 5; i++)
    if (WriteBlob(image,128,buffer) != 128)
      status=MagickFail;

  /*
    Encode data to Group 4
  */
  if (MagickFail != status)
  {
    unsigned char
      *blob;

    size_t
      blob_length;

    blob=ImageToHuffman2DBlob(image,image_info,&blob_length,&image->exception);
    if (blob == (unsigned char *) NULL)
      status=MagickFail;

    if (MagickFail != status)
      {
	if (WriteBlob(image,blob_length,blob) != blob_length)
	  status=MagickFail;
      }
    MagickFreeMemory(blob);
  }

  /*
    Close output file and return image
  */
  CloseBlob(image);
  return status;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r C A L S I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterCALSImage adds attributes for the CALS image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterCALSImage method is:
%
%      RegisterCALSImage(void)
%
*/
ModuleExport void RegisterCALSImage(void)
{
  MagickInfo
    *entry;

  const char
    *description = "Continuous Acquisition and Life-cycle Support Type 1 image",
    *module      = "CALS",
    *note        = "Specified in MIL-R-28002 and MIL-PRF-28002";

  entry=SetMagickInfo("CAL");
  entry->decoder=(DecoderHandler) ReadCALSImage;
  entry->encoder=(EncoderHandler) WriteCALSImage;
  entry->magick=(MagickHandler) IsCALS;
  entry->adjoin=MagickFalse;
  entry->seekable_stream=MagickTrue;
  entry->description=description;
  entry->note=note;
  entry->module=module;
  entry->stealth=MagickTrue;
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("CALS");
  entry->decoder=(DecoderHandler) ReadCALSImage;
  entry->encoder=(EncoderHandler) WriteCALSImage;
  entry->magick=(MagickHandler) IsCALS;
  entry->adjoin=MagickFalse;
  entry->seekable_stream=MagickTrue;
  entry->description=description;
  entry->note=note;
  entry->module=module;
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r C A L S I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterCALSImage removes format registrations made by the
%  CAL module from the list of supported formats.
%
%  The format of the UnregisterCALSImage method is:
%
%      UnregisterCALSImage(void)
%
*/
ModuleExport void UnregisterCALSImage(void)
{
  (void) UnregisterMagickInfo("CAL");
  (void) UnregisterMagickInfo("CALS");
}
