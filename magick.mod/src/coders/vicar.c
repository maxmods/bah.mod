/*
% Copyright (C) 2003 GraphicsMagick Group
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
%                     V   V  IIIII   CCCC   AAA   RRRR                        %
%                     V   V    I    C      A   A  R   R                       %
%                     V   V    I    C      AAAAA  RRRR                        %
%                      V V     I    C      A   A  R R                         %
%                       V    IIIII   CCCC  A   A  R  R                        %
%                                                                             %
%                                                                             %
%                   Read/Write VICAR Rasterfile Format.                       %
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
#include "magick/blob.h"
#include "magick/pixel_cache.h"
#include "magick/constitute.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/utility.h"

/*
  Forward declarations.
*/
static unsigned int
  WriteVICARImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s V I C A R                                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsVICAR returns True if the image format type, identified by the
%  magick string, is VICAR.
%
%  The format of the IsVICAR method is:
%
%      unsigned int IsVICAR(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsVICAR returns True if the image format type is VICAR.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsVICAR(const unsigned char *magick,const size_t length)
{
  if (length < 7)
    return(False);
  if (LocaleNCompare((char *) magick,"LBLSIZE",7) == 0)
    return(True);
  if (LocaleNCompare((char *) magick,"NJPL1I",6) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d V I C A R I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadVICARImage reads a VICAR image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadVICARImage method is:
%
%      Image *ReadVICARImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: Method ReadVICARImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadVICARImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  char
    keyword[MaxTextExtent],
    value[MaxTextExtent];

  Image
    *image;

  int
    c,
    y;

  long
    count;

  unsigned char
    *scanline;

  unsigned int
    status,
    value_expected;

  unsigned long
    length;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    ThrowReaderException(FileOpenError,UnableToOpenFile,image);
  /*
    Decode image header.
  */
  c=ReadBlobByte(image);
  count=1;
  if (c == EOF)
    {
      DestroyImage(image);
      return((Image *) NULL);
    }
  length=0;
  image->columns=0;
  image->rows=0;
  while (isgraph(c) && ((image->columns == 0) || (image->rows == 0)))
  {
    if (!isalnum(c))
      {
        c=ReadBlobByte(image);
        count++;
      }
    else
      {
        register char
          *p;

        /*
          Determine a keyword and its value.
        */
        p=keyword;
        do
        {
          if ((p-keyword) < (MaxTextExtent-1))
            *p++=c;
          c=ReadBlobByte(image);
          count++;
        } while (isalnum(c) || (c == '_'));
        *p='\0';
        value_expected=False;
        while (isspace(c) || (c == '='))
        {
          if (c == '=')
            value_expected=True;
          c=ReadBlobByte(image);
          count++;
        }
        if (value_expected == False)
          continue;
        p=value;
        while (isalnum(c))
        {
          if ((p-value) < (MaxTextExtent-1))
            *p++=c;
          c=ReadBlobByte(image);
          count++;
        }
        *p='\0';
        /*
          Assign a value to the specified keyword.
        */
        if (LocaleCompare(keyword,"Label_RECORDS") == 0)
          length=atol(value);
        if (LocaleCompare(keyword,"LBLSIZE") == 0)
          length=atol(value);
        if (LocaleCompare(keyword,"RECORD_BYTES") == 0)
          image->columns= atol(value);
        if (LocaleCompare(keyword,"NS") == 0)
          image->columns= atol(value);
        if (LocaleCompare(keyword,"LINES") == 0)
          image->rows= atol(value);
        if (LocaleCompare(keyword,"NL") == 0)
          image->rows= atol(value);
      }
    while (isspace(c))
    {
      c=ReadBlobByte(image);
      count++;
    }
  }
  while (count < (long) length)
  {
    c=ReadBlobByte(image);
    count++;
  }
  if ((image->columns == 0) || (image->rows == 0))
    ThrowReaderException(CorruptImageError,NegativeOrZeroImageSize,image);
  image->depth=8;
  if (!AllocateImageColormap(image,256))
    ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
  if (image_info->ping)
    {
      CloseBlob(image);
      return(image);
    }
  /*
    Read VICAR pixels.
  */
  scanline=MagickAllocateMemory(unsigned char *,image->columns);
  if (scanline == (unsigned char *) NULL)
    ThrowReaderException(CorruptImageError,UnableToReadImageData,image);
  for (y=0; y < (long) image->rows; y++)
  {
    if (!SetImagePixels(image,0,y,image->columns,1))
      break;
    (void) ReadBlob(image,image->columns,scanline);
    (void) ImportImagePixelArea(image,GrayQuantum,image->depth,scanline,0,0);
    if (!SyncImagePixels(image))
      break;
    if (QuantumTick(y,image->rows))
      if (!MagickMonitorFormatted(y,image->rows,exception,LoadImageText,
                                  image->filename))
        break;
  }
  MagickFreeMemory(scanline);
  if (EOFBlob(image))
    ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,
      image->filename);
  CloseBlob(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r V I C A R I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterVICARImage adds attributes for the VICAR image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterVICARImage method is:
%
%      RegisterVICARImage(void)
%
*/
ModuleExport void RegisterVICARImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("VICAR");
  entry->decoder=(DecoderHandler) ReadVICARImage;
  entry->encoder=(EncoderHandler) WriteVICARImage;
  entry->magick=(MagickHandler) IsVICAR;
  entry->adjoin=False;
  entry->description="VICAR rasterfile format";
  entry->module="VICAR";
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r V I C A R I m a g e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterVICARImage removes format registrations made by the
%  VICAR module from the list of supported formats.
%
%  The format of the UnregisterVICARImage method is:
%
%      UnregisterVICARImage(void)
%
*/
ModuleExport void UnregisterVICARImage(void)
{
  (void) UnregisterMagickInfo("VICAR");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e V I C A R I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteVICARImage writes an image in the VICAR rasterfile format.
%  Vicar files contain a text header, followed by one or more planes of binary
%  grayscale image data.  Vicar files are designed to allow many planes to be
%  stacked together to form image cubes.  This method only writes a single
%  grayscale plane.
%
%  Method WriteVICARImage was written contributed by
%  gorelick@esther.la.asu.edu.
%
%  The format of the WriteVICARImage method is:
%
%      unsigned int WriteVICARImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteVICARImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
static unsigned int WriteVICARImage(const ImageInfo *image_info,Image *image)
{
  char
    header[MaxTextExtent];

  int
    y;

  unsigned char
    *scanline;

  unsigned int
    status;

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
  (void) TransformColorspace(image,RGBColorspace);
  /*
    Write header.
  */
  (void) memset(header,' ',MaxTextExtent);
  FormatString(header,"LBLSIZE=%u FORMAT='BYTE' TYPE='IMAGE' BUFSIZE=20000 "
    "DIM=2 EOL=0 RECSIZE=%lu ORG='BSQ' NL=%lu NS=%lu NB=1 N1=0 N2=0 N3=0 N4=0 "
    "NBB=0 NLB=0 TASK='GraphicsMagick'",MaxTextExtent,image->columns,image->rows,
    image->columns);
  (void) WriteBlob(image,MaxTextExtent,header);
  /*
    Allocate memory for scanline.
  */
  scanline=MagickAllocateMemory(unsigned char *,image->columns);
  if (scanline == (unsigned char *) NULL)
    ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
  /*
    Write VICAR scanline.
  */
  for (y=0; y < (long) image->rows; y++)
  {
    if (!AcquireImagePixels(image,0,y,image->columns,1,&image->exception))
      break;
    (void) ExportImagePixelArea(image,GrayQuantum,8,scanline,0,0);
    (void) WriteBlob(image,image->columns,scanline);
    if (image->previous == (Image *) NULL)
      if (QuantumTick(y,image->rows))
        if (!MagickMonitorFormatted(y,image->rows,&image->exception,
                                    SaveImageText,image->filename))
          break;
  }
  MagickFreeMemory(scanline);
  CloseBlob(image);
  return(True);
}
