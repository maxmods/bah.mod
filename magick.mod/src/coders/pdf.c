/*
% Copyright (C) 2003-2009 GraphicsMagick Group
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
%                            PPPP   DDDD   FFFFF                              %
%                            P   P  D   D  F                                  %
%                            PPPP   D   D  FFF                                %
%                            P      D   D  F                                  %
%                            P      DDDD   F                                  %
%                                                                             %
%                                                                             %
%                  Read/Write Portable Document Format.                       %
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
#include "magick/analyze.h"
#include "magick/attribute.h"
#include "magick/blob.h"
#include "magick/compress.h"
#include "magick/constitute.h"
#include "magick/delegate.h"
#include "magick/enum_strings.h"
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/pixel_cache.h"
#include "magick/resize.h"
#include "magick/shear.h"
#include "magick/tempfile.h"
#include "magick/utility.h"
#include "magick/version.h"

/*
  Forward declarations.
*/
static unsigned int
  WritePDFImage(const ImageInfo *,Image *),
  ZLIBEncodeImage(Image *,const size_t,const unsigned long,unsigned char *);


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s P D F                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsPDF returns True if the image format type, identified by the
%  magick string, is PDF.
%
%  The format of the IsPDF method is:
%
%      unsigned int IsPDF(const unsigned char *magick,const size_t offset)
%
%  A description of each parameter follows:
%
%    o status:  Method IsPDF returns True if the image format type is PDF.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o offset: Specifies the offset of the magick string.
%
%
*/
static unsigned int IsPDF(const unsigned char *magick,const size_t offset)
{
  if (offset < 5)
    return(False);
  if (LocaleNCompare((char *) magick,"%PDF-",5) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d P D F I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPDFImage reads a Portable Document Format image file and
%  returns it.  It allocates the memory necessary for the new Image structure
%  and returns a pointer to the new image.
%
%  The format of the ReadPDFImage method is:
%
%      Image *ReadPDFImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadPDFImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadPDFImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
#define MediaBox  "/MediaBox"
#define RenderPostscriptText  "[%s] Rendering postscript..."

  char
    density[MaxTextExtent],
    command[MaxTextExtent],
    filename[MaxTextExtent],
    geometry[MaxTextExtent],
    postscript_filename[MaxTextExtent];

  const DelegateInfo
    *delegate_info;

  double
    dx_resolution,
    dy_resolution;

  FILE
    *file;

  Image
    *image,
    *next_image;

  ImageInfo
    *clone_info;

  int
    count,
    rotate,
    status;

  unsigned int
    antialias=4;

  RectangleInfo
    box,
    page;

  register char
    *p,
    *q;

  register long
    c;

  SegmentInfo
    bounds;

  unsigned long
    height,
    width;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);

  /*
    Select Postscript delegate driver
  */
  delegate_info=GetPostscriptDelegateInfo(image_info,&antialias,exception);
  if (delegate_info == (const DelegateInfo *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    ThrowReaderException(FileOpenError,UnableToOpenFile,image);
  /*
    Open temporary output file.
  */
  file=AcquireTemporaryFileStream(postscript_filename,BinaryFileIOMode);
  if (file == (FILE *) NULL)
    ThrowReaderTemporaryFileException(postscript_filename);
  /*
    Set the page density.
  */
  dx_resolution=72.0;
  dy_resolution=72.0;
  if ((image->x_resolution == 0.0) || (image->y_resolution == 0.0))
    {
      (void) strcpy(density,PSDensityGeometry);
      count=GetMagickDimension(density,&image->x_resolution,&image->y_resolution,NULL,NULL);
      if (count != 2)
        image->y_resolution=image->x_resolution;
    }
  FormatString(density,"%gx%g",image->x_resolution,image->y_resolution);
  /*
    Determine page geometry from the PDF media box.

    Note that we can use Ghostscript to obtain the bounding box info like

    gs -q -dBATCH -dNOPAUSE -sDEVICE=bbox ENV.003.01.pdf
    %%BoundingBox: 70 61 2089 2954
    %%HiResBoundingBox: 70.737537 61.199998 2088.587889 2953.601629
  */
  rotate=0;
  (void) memset(&page,0,sizeof(RectangleInfo));
  (void) memset(&box,0,sizeof(RectangleInfo));
  for (p=command; ; )
  {
    c=ReadBlobByte(image);
    if (c == EOF)
      break;
    (void) fputc(c,file);
    *p++=(char) c;
    if ((c != '\n') && (c != '\r') && ((p-command) < (MaxTextExtent-1)))
      continue;
    *p='\0';
    p=command;
    /*
      Continue unless this is a MediaBox statement.
    */
    if (LocaleNCompare(command,"/Rotate ",8) == 0)
      {
        count=sscanf(command,"/Rotate %d",&rotate);
        if (count > 0)
          {
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "Rotate by %d degrees",rotate);
          }
      }
    q=strstr(command,MediaBox);
    if (q == (char *) NULL)
      continue;
    count=sscanf(q,"/MediaBox [%lf %lf %lf %lf",&bounds.x1,&bounds.y1,
      &bounds.x2,&bounds.y2);
    if (count != 4)
      count=sscanf(q,"/MediaBox[%lf %lf %lf %lf",&bounds.x1,&bounds.y1,
        &bounds.x2,&bounds.y2);
    if (count == 4)
      {
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "Parsed: MediaBox %lf %lf %lf %lf",
                              bounds.x1,bounds.y1,
                              bounds.x2,bounds.y2);
      }
    if (count != 4)
      continue;
    if ((bounds.x1 > bounds.x2) || (bounds.y1 > bounds.y2))
      continue;
    /*
      Set Postscript render geometry.
    */
    width=(unsigned long) (bounds.x2-bounds.x1+0.5);
    height=(unsigned long) (bounds.y2-bounds.y1+0.5);
    if ((width <= box.width) && (height <= box.height))
      continue;
    page.width=width;
    page.height=height;
    box=page;
  }
  /*
    If page is rotated right or left, then swap width and height values.
  */
  if ((90 == AbsoluteValue(rotate)) || (270 == AbsoluteValue(rotate)))
    {
      double
        value;

      value=page.width;
      page.width=page.height;
      page.height=value;
    }
  if ((page.width == 0) || (page.height == 0))
    {
      SetGeometry(image,&page);
      (void) GetGeometry(PSPageGeometry,&page.x,&page.y,&page.width,
        &page.height);
    }
  if (image_info->page != (char *) NULL)
    (void) GetGeometry(image_info->page,&page.x,&page.y,&page.width,
      &page.height);
  geometry[0]='\0';
  FormatString(geometry,"%lux%lu",
    (unsigned long) ceil(page.width*image->x_resolution/dx_resolution-0.5),
    (unsigned long) ceil(page.height*image->y_resolution/dy_resolution-0.5));
  if (ferror(file))
    {
      (void) fclose(file);
      ThrowReaderException(CorruptImageError,AnErrorHasOccurredWritingToFile,
        image)
    }
  (void) fclose(file);
  CloseBlob(image);
  /*
    Use Ghostscript to convert Postscript image.
  */
  {
    char
      options[MaxTextExtent];

    options[0]='\0';
    /*
      Append subrange.
    */
    if (image_info->subrange != 0)
      FormatString(options,"-dFirstPage=%lu -dLastPage=%lu",
		   image_info->subimage+1,image_info->subimage+image_info->subrange);
    /*
      Append authentication string.
    */
    if (image_info->authenticate != (char *) NULL)
      FormatString(options+strlen(options)," -sPDFPassword=%.1024s",
		   image_info->authenticate);
    (void) strlcpy(filename,image_info->filename,MaxTextExtent);
    clone_info=CloneImageInfo(image_info);
    if (!AcquireTemporaryFileName(clone_info->filename))
      {
	DestroyImageInfo(clone_info);
	ThrowReaderTemporaryFileException(clone_info->filename);
      }
    FormatString(command,delegate_info->commands,antialias,
		 antialias,density,options,clone_info->filename,
		 postscript_filename);
  }
  (void) MagickMonitorFormatted(0,8,&image->exception,RenderPostscriptText,
                                image->filename);
  status=InvokePostscriptDelegate(clone_info->verbose,command,exception);
  (void) MagickMonitorFormatted(7,8,&image->exception,RenderPostscriptText,
                                image->filename);
  DestroyImage(image);
  image=(Image *) NULL;
  if (IsAccessibleAndNotEmpty(clone_info->filename))
    {
      /*
	Read Ghostscript output.
      */
      clone_info->blob=(void *) NULL;
      clone_info->length=0;
      clone_info->magick[0]='\0';
      image=ReadImage(clone_info,exception);
    }
  (void) LiberateTemporaryFile(postscript_filename);
  (void) LiberateTemporaryFile(clone_info->filename);
  DestroyImageInfo(clone_info);
  if (image == (Image *) NULL)
    {
      if (UndefinedException == exception->severity)
	ThrowException(exception,DelegateError,PostscriptDelegateFailed,filename);
    }
  else
    {
      do
	{
	  (void) strcpy(image->magick,"PDF");
	  (void) strlcpy(image->filename,filename,MaxTextExtent);
	  next_image=SyncNextImageInList(image);
	  if (next_image != (Image *) NULL)
	    image=next_image;
	} while (next_image != (Image *) NULL);
      while (image->previous != (Image *) NULL)
	image=image->previous;
    }
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r P D F I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterPDFImage adds attributes for the PDF image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterPDFImage method is:
%
%      RegisterPDFImage(void)
%
*/
ModuleExport void RegisterPDFImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("EPDF");
  entry->decoder=(DecoderHandler) ReadPDFImage;
  entry->encoder=(EncoderHandler) WritePDFImage;
  entry->adjoin=False;
  entry->blob_support=False;
  entry->seekable_stream=True;
  entry->description="Encapsulated Portable Document Format";
  entry->module="PDF";
  entry->coder_class=PrimaryCoderClass;
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("PDF");
  entry->decoder=(DecoderHandler) ReadPDFImage;
  entry->encoder=(EncoderHandler) WritePDFImage;
  entry->magick=(MagickHandler) IsPDF;
  entry->blob_support=False;
  entry->seekable_stream=True;
  entry->description="Portable Document Format";
  entry->module="PDF";
  entry->coder_class=PrimaryCoderClass;
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r P D F I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterPDFImage removes format registrations made by the
%  PDF module from the list of supported formats.
%
%  The format of the UnregisterPDFImage method is:
%
%      UnregisterPDFImage(void)
%
*/
ModuleExport void UnregisterPDFImage(void)
{
  (void) UnregisterMagickInfo("EPDF");
  (void) UnregisterMagickInfo("PDF");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P D F I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePDFImage writes an image in the Portable Document image
%  format.
%
%  The format of the WritePDFImage method is:
%
%      unsigned int WritePDFImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WritePDFImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/

static char *EscapeParenthesis(const char *text)
{
  register char
    *p;

  register long
    i;

  static char
    buffer[MaxTextExtent];

  unsigned long
    escapes;

  escapes=0;
  p=buffer;
  for (i=0; i < (long) Min(strlen(text),(MaxTextExtent-escapes-1)); i++)
  {
    if ((text[i] == '(') || (text[i] == ')'))
      {
        *p++='\\';
        escapes++;
      }
    *p++=text[i];
  }
  *p='\0';
  return(buffer);
}

static unsigned int WritePDFImage(const ImageInfo *image_info,Image *image)
{
#define CFormat  "/Filter [ /%.1024s ]\n"
#define ObjectsPerImage  9

  char
    buffer[MaxTextExtent],
    date[MaxTextExtent],
    density[MaxTextExtent],
    **labels,
    page_geometry[MaxTextExtent];

  unsigned char
    *fax_blob=(unsigned char *) NULL;

  const ImageAttribute
    *attribute;

  double
    dx_resolution,
    dy_resolution,
    x_resolution,
    x_scale,
    y_resolution,
    y_scale;

  ExtendedSignedIntegralType
    offset;

  long
    count,
    y;

  RectangleInfo
    geometry,
    media_info;

  register const PixelPacket
    *p;

  register const IndexPacket
    *indexes;

  register unsigned char
    *q;

  register long
    i,
    x;

  size_t
    fax_blob_length,
    length;

  struct tm
    *time_meridian;

  time_t
    seconds;

  unsigned char
    *pixels;

  MagickPassFail
    status;

  unsigned long
    info_id,
    number_pixels,
    object,
    pages_id,
    root_id,
    scene,
    text_size;

  ExtendedSignedIntegralType
    *xref;

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

  /*
    Allocate X ref memory.
  */
  xref=MagickAllocateMemory(ExtendedSignedIntegralType *,
                            2048*sizeof(ExtendedSignedIntegralType));
  if (xref == (ExtendedSignedIntegralType *) NULL)
    ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
  (void) memset(xref,0,2048*sizeof(ExtendedSignedIntegralType));
  /*
    Write Info object.
  */
  object=0;
  (void) WriteBlobString(image,"%PDF-1.2 \n");
  xref[object++]=TellBlob(image);
  info_id=object;
  FormatString(buffer,"%lu 0 obj\n",object);
  (void) WriteBlobString(image,buffer);
  (void) WriteBlobString(image,"<<\n");
  seconds=time((time_t *) NULL);
  time_meridian=localtime(&seconds);
  FormatString(date,"D:%04d%02d%02d%02d%02d%02d",time_meridian->tm_year+1900,
               time_meridian->tm_mon+1,time_meridian->tm_mday,time_meridian->tm_hour,
               time_meridian->tm_min,time_meridian->tm_sec);
  FormatString(buffer,"/CreationDate (%.1024s)\n",date);
  (void) WriteBlobString(image,buffer);
  FormatString(buffer,"/ModDate (%.1024s)\n",date);
  (void) WriteBlobString(image,buffer);
  FormatString(buffer,"/Producer (%.1024s)\n",
               EscapeParenthesis(GetMagickVersion((unsigned long *) NULL)));
  (void) WriteBlobString(image,buffer);
  (void) WriteBlobString(image,">>\n");
  (void) WriteBlobString(image,"endobj\n");
  /*
    Write Catalog object.
  */
  xref[object++]=TellBlob(image);
  root_id=object;
  FormatString(buffer,"%lu 0 obj\n",object);
  (void) WriteBlobString(image,buffer);
  (void) WriteBlobString(image,"<<\n");
  (void) WriteBlobString(image,"/Type /Catalog\n");
  FormatString(buffer,"/Pages %lu 0 R\n",object+1);
  (void) WriteBlobString(image,buffer);
  (void) WriteBlobString(image,">>\n");
  (void) WriteBlobString(image,"endobj\n");
  /*
    Write Pages object.
  */
  xref[object++]=TellBlob(image);
  pages_id=object;
  FormatString(buffer,"%lu 0 obj\n",object);
  (void) WriteBlobString(image,buffer);
  (void) WriteBlobString(image,"<<\n");
  (void) WriteBlobString(image,"/Type /Pages\n");
  FormatString(buffer,"/Kids [ %lu 0 R ",object+1);
  (void) WriteBlobString(image,buffer);
  count=(long) (pages_id+ObjectsPerImage+1);
  if (image_info->adjoin)
    {
      Image
        *kid_image;

      /*
        Predict page object id's.
      */
      kid_image=image;
      for ( ; kid_image->next != (Image *) NULL; count+=ObjectsPerImage)
        {
          FormatString(buffer,"%ld 0 R ",count);
          (void) WriteBlobString(image,buffer);
          kid_image=kid_image->next;
        }
      MagickReallocMemory(ExtendedSignedIntegralType *,xref,
                          (count+2048)*sizeof(ExtendedSignedIntegralType));
      if (xref == (ExtendedSignedIntegralType *) NULL)
        ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
    }
  (void) WriteBlobString(image,"]\n");
  FormatString(buffer,"/Count %lu\n",(count-pages_id)/ObjectsPerImage);
  (void) WriteBlobString(image,buffer);
  (void) WriteBlobString(image,">>\n");
  (void) WriteBlobString(image,"endobj\n");
  scene=0;
  do
    {
      ImageCharacteristics
	characteristics;

      CompressionType
	compression;

      /*
	Analyze image properties.
      */
      (void) GetImageCharacteristics(image,&characteristics,
				     (OptimizeType == image_info->type),
				     &image->exception);

      compression=image->compression;
      if (image_info->compression != UndefinedCompression)
	{
	  /*
	    ImageInfo compression always prevails if it is set.
	  */
	  compression=image_info->compression;
	}
      else
	{
	  /*
	    Default to Zip compression unless the image was JPEG
	    compressed and is not now monochrome or colormapped.
	  */
	  if ((JPEGCompression != compression) ||
	      (characteristics.monochrome) ||
	      (characteristics.palette))
	    {
#if defined(HasZLIB)
	      compression=ZipCompression;
#else
	      compression=LZWCompression;
#endif
	    }
	}
      
      switch (compression)
	{
#if !defined(HasJPEG)
	case JPEGCompression:
	  {
	    /*
	      If JPEG compression is not supported, then use RLE compression
	      and report a warning to user.
	    */
	    compression=RLECompression;
	    ThrowException(&image->exception,MissingDelegateError,JPEGLibraryIsNotAvailable,image->filename);
	    break;
	  }
#endif
#if !defined(HasZLIB)
	case ZipCompression:
	  {
	    /*
	      If ZIP compression is not supported, then use RLE compression
	      and report a warning to user.
	    */
	    compression=RLECompression;
	    ThrowException(&image->exception,MissingDelegateError,ZipLibraryIsNotAvailable,image->filename);
	    break;
	  }
#endif
	default:
	  break;
	}
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
			    "%s compression.",
			    CompressionTypeToString(compression));

      /*
        Scale image to size of Portable Document page.
      */
      text_size=0;
      attribute=GetImageAttribute(image,"label");
      if (attribute != (const ImageAttribute *) NULL)
        text_size=(unsigned int)
          (MultilineCensus(attribute->value)*image_info->pointsize+12);
      SetGeometry(image,&geometry);
      geometry.y=(long) text_size;
      FormatString(page_geometry,"%lux%lu",image->columns,image->rows);
      if (image_info->page != (char *) NULL)
        (void) strlcpy(page_geometry,image_info->page,MaxTextExtent);
      else
        if ((image->page.width != 0) && (image->page.height != 0))
          (void) FormatString(page_geometry,"%lux%lu%+ld%+ld",image->page.width,
                              image->page.height,image->page.x,image->page.y);
        else
          if (LocaleCompare(image_info->magick,"PDF") == 0)
            (void) strcpy(page_geometry,PSPageGeometry);
      (void) GetMagickGeometry(page_geometry,&geometry.x,&geometry.y,
                               &geometry.width,&geometry.height);
      (void) GetGeometry(page_geometry,&media_info.x,&media_info.y,
                         &media_info.width,&media_info.height);
      /*
        Scale relative to dots-per-inch.
      */
      dx_resolution=72.0;
      dy_resolution=72.0;
      x_resolution=72.0;
      (void) strcpy(density,PSDensityGeometry);
      count=GetMagickDimension(density,&x_resolution,&y_resolution,NULL,NULL);
      if (count != 2)
        y_resolution=x_resolution;
      if (image_info->density != (char *) NULL)
        {
          count=GetMagickDimension(image_info->density,&x_resolution,&y_resolution,NULL,NULL);
          if (count != 2)
            y_resolution=x_resolution;
        }
      x_scale=(geometry.width*dx_resolution)/x_resolution;
      geometry.width=(unsigned long) (x_scale+0.5);
      y_scale=(geometry.height*dy_resolution)/y_resolution;
      geometry.height=(unsigned long) (y_scale+0.5);
      /*
        Write Page object.
      */
      xref[object++]=TellBlob(image);
      FormatString(buffer,"%lu 0 obj\n",object);
      (void) WriteBlobString(image,buffer);
      (void) WriteBlobString(image,"<<\n");
      (void) WriteBlobString(image,"/Type /Page\n");
      FormatString(buffer,"/Parent %lu 0 R\n",pages_id);
      (void) WriteBlobString(image,buffer);
      (void) WriteBlobString(image,"/Resources <<\n");
      FormatString(buffer,"/XObject << /Im%lu %lu 0 R >>\n",image->scene,
                   object+4);
      (void) WriteBlobString(image,buffer);
      FormatString(buffer,"/ProcSet %lu 0 R >>\n",object+3);
      (void) WriteBlobString(image,buffer);
      FormatString(buffer,"/MediaBox [0 0 %ld %ld]\n",
                   media_info.width,media_info.height);
      (void) WriteBlobString(image,buffer);
      FormatString(buffer,"/CropBox [%ld %ld %ld %ld]\n",geometry.x,geometry.y,
                   geometry.x+geometry.width,geometry.y+geometry.height);
      (void) WriteBlobString(image,buffer);
      FormatString(buffer,"/Contents %lu 0 R\n",object+1);
      (void) WriteBlobString(image,buffer);
      (void) WriteBlobString(image,">>\n");
      (void) WriteBlobString(image,"endobj\n");
      /*
        Write Contents object.
      */
      xref[object++]=TellBlob(image);
      FormatString(buffer,"%lu 0 obj\n",object);
      (void) WriteBlobString(image,buffer);
      (void) WriteBlobString(image,"<<\n");
      FormatString(buffer,"/Length %lu 0 R\n",object+1);
      (void) WriteBlobString(image,buffer);
      (void) WriteBlobString(image,">>\n");
      (void) WriteBlobString(image,"stream\n");
      offset=TellBlob(image);
      (void) WriteBlobString(image,"q\n");
      labels=(char **) NULL;
      attribute=GetImageAttribute(image,"label");
      if (attribute != (const ImageAttribute *) NULL)
        labels=StringToList(attribute->value);
      if (labels != (char **) NULL)
        {
          for (i=0; labels[i] != (char *) NULL; i++)
            {
              (void) WriteBlobString(image,"BT\n");
              FormatString(buffer,"/F%lu %g Tf\n",image->scene,
                           image_info->pointsize);
              (void) WriteBlobString(image,buffer);
              FormatString(buffer,"%ld %g Td\n",geometry.x,geometry.y+
                           geometry.height+i*image_info->pointsize+12);
              (void) WriteBlobString(image,buffer);
              FormatString(buffer,"(%.1024s) Tj\n",labels[i]);
              (void) WriteBlobString(image,buffer);
              (void) WriteBlobString(image,"ET\n");
              MagickFreeMemory(labels[i]);
            }
          MagickFreeMemory(labels);
        }
      FormatString(buffer,"%g 0 0 %g %ld %ld cm\n",x_scale,y_scale,geometry.x,
                   geometry.y);
      (void) WriteBlobString(image,buffer);
      FormatString(buffer,"/Im%lu Do\n",image->scene);
      (void) WriteBlobString(image,buffer);
      (void) WriteBlobString(image,"Q\n");
      offset=TellBlob(image)-offset;
      (void) WriteBlobString(image,"endstream\n");
      (void) WriteBlobString(image,"endobj\n");
      /*
        Write Length object.
      */
      xref[object++]=TellBlob(image);
      FormatString(buffer,"%lu 0 obj\n",object);
      (void) WriteBlobString(image,buffer);
      FormatString(buffer,"%lu\n",(unsigned long) offset);
      (void) WriteBlobString(image,buffer);
      (void) WriteBlobString(image,"endobj\n");
      /*
        Write Procset object.
      */
      xref[object++]=TellBlob(image);
      FormatString(buffer,"%lu 0 obj\n",object);
      (void) WriteBlobString(image,buffer);
      if ((image->storage_class == DirectClass) || (image->colors > 256))
        (void) strcpy(buffer,"[ /PDF /Text /ImageC");
      else
        if (compression == FaxCompression)
          (void) strcpy(buffer,"[ /PDF /Text /ImageB");
        else
          (void) strcpy(buffer,"[ /PDF /Text /ImageI");
      (void) WriteBlobString(image,buffer);
      (void) WriteBlobString(image," ]\n");
      (void) WriteBlobString(image,"endobj\n");
      /*
        Write XObject object.
      */
      xref[object++]=TellBlob(image);
      FormatString(buffer,"%lu 0 obj\n",object);
      (void) WriteBlobString(image,buffer);
      (void) WriteBlobString(image,"<<\n");
      (void) WriteBlobString(image,"/Type /XObject\n");
      (void) WriteBlobString(image,"/Subtype /Image\n");
      FormatString(buffer,"/Name /Im%lu\n",image->scene);
      (void) WriteBlobString(image,buffer);
      switch (compression)
        {
        case NoCompression:
          {
            FormatString(buffer,CFormat,"ASCII85Decode");
            break;
          }
        case JPEGCompression: 
          {
            FormatString(buffer,CFormat,"DCTDecode"); 
            if (image->colorspace != CMYKColorspace)
              break;
            (void) WriteBlobString(image,buffer);
            (void) strcpy(buffer,"/Decode [1 0 1 0 1 0 1 0]\n");
            break;
          }
        case LZWCompression:
          {
            FormatString(buffer,CFormat,"LZWDecode");
            break;
          }
        case ZipCompression:
          {
            FormatString(buffer,CFormat,"FlateDecode");
            break;
          }
        case FaxCompression:
          {
	    char
	      CCITTParam[4];

	    ExceptionInfo
	      exception;

	    /*
	      Try compressing page to Group4 to see if it is
	      supported, otherwise we will fall back to Group3.
	    */
	    (void) strlcpy(CCITTParam,"0",sizeof(CCITTParam));
	    GetExceptionInfo(&exception);
	    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
				  "Executing ImageToHuffman2DBlob for CCITT Fax4 ...");
	    fax_blob=ImageToHuffman2DBlob(image,image_info,&fax_blob_length,
					  &exception);
	    if (fax_blob != (unsigned char *) NULL)
	      {
		(void) strlcpy(CCITTParam,"-1",sizeof(CCITTParam));
		(void) LogMagickEvent(CoderEvent,GetMagickModule(),
				      "ImageToHuffman2DBlob reports success!");
	      }
	    DestroyExceptionInfo(&exception);
            (void) strcpy(buffer,"/Filter [ /CCITTFaxDecode ]\n");
            (void) WriteBlobString(image,buffer);
            (void) strcpy(buffer,"/Interpolate false\n");
            (void) WriteBlobString(image,buffer);
            FormatString(buffer,
                         "/DecodeParms [ << /K %.1024s /BlackIs1 false /Columns %ld /Rows %ld >> ]\n",
                         CCITTParam,image->columns,image->rows);
            break;
          }
        default:
          {
            FormatString(buffer,CFormat,"RunLengthDecode");
            break;
          }
        }
      (void) WriteBlobString(image,buffer);
      FormatString(buffer,"/Width %lu\n",image->columns);
      (void) WriteBlobString(image,buffer);
      FormatString(buffer,"/Height %lu\n",image->rows);
      (void) WriteBlobString(image,buffer);
      if (compression == FaxCompression)
        strlcpy(buffer,"/ColorSpace /DeviceGray\n",MaxTextExtent);
      else
        FormatString(buffer,"/ColorSpace %lu 0 R\n",object+2);
      (void) WriteBlobString(image,buffer);
      FormatString(buffer,"/BitsPerComponent %d\n",
                   compression == FaxCompression ? 1 : 8);
      (void) WriteBlobString(image,buffer);
      FormatString(buffer,"/Length %lu 0 R\n",object+1);
      (void) WriteBlobString(image,buffer);
      (void) WriteBlobString(image,">>\n");
      (void) WriteBlobString(image,"stream\n");
      offset=TellBlob(image);
      number_pixels=image->columns*image->rows;
      if ((compression == FaxCompression) ||
          ((image_info->type != TrueColorType) &&
           characteristics.grayscale))
        {
	  /*
	    Write grayscale output.
	  */
          switch (compression)
            {
            case FaxCompression:
              {
		/*
		  Try Group4 first and use Group3 as a fallback.
		*/
		if (fax_blob != (unsigned char *) NULL)
		  {
		    (void) WriteBlob(image,fax_blob_length,fax_blob);
		    MagickFreeMemory(fax_blob);
		  }
                else
                  {
                    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                          "Executing HuffmanEncodeImage for CCITT Fax3 ...");
                    if (!HuffmanEncodeImage(image_info,image))
                      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                            "HuffmanEncodeImage reports failure!");
                  }
                break;
              }
            case JPEGCompression:
              {
                unsigned char
                  *jpeg_blob;

                /*
                  Write image in JPEG format.
                */
		jpeg_blob=ImageToJPEGBlob(image,image_info,&length,&image->exception);
                if (jpeg_blob == (unsigned char *) NULL)
                  ThrowWriterException2(CoderError,image->exception.reason,image);
                (void) WriteBlob(image,length,jpeg_blob);
                MagickFreeMemory(jpeg_blob);
                break;
              }
            case RLECompression:
            default:
              {
                /*
                  Allocate pixel array.
                */
                length=number_pixels;
                pixels=MagickAllocateMemory(unsigned char *,length);
                if (pixels == (unsigned char *) NULL)
                  ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,
                                       image);
                /*
                  Dump Runlength encoded pixels.
                */
                q=pixels;
                for (y=0; y < (long) image->rows; y++)
                  {
                    p=AcquireImagePixels(image,0,y,image->columns,1,
                                         &image->exception);
                    if (p == (const PixelPacket *) NULL)
                      break;
                    for (x=0; x < (long) image->columns; x++)
                      {
                        *q++=ScaleQuantumToChar(PixelIntensityToQuantum(p));
                        p++;
                      }
                    if (image->previous == (Image *) NULL)
                      if (QuantumTick(y,image->rows))
                        {
                          status=MagickMonitorFormatted(y,image->rows,
                                                        &image->exception,
                                                        SaveImageText,
                                                        image->filename,
							image->columns,image->rows);
                          if (status == False)
                            break;
                        }
                  }
#if defined(HasZLIB)
                if (compression == ZipCompression)
                  status=ZLIBEncodeImage(image,length,image_info->quality,pixels);
                else
#endif /* defined(HasZLIB) */
                  if (compression == LZWCompression)
                    status=LZWEncodeImage(image,length,pixels);
                  else
                    status=PackbitsEncodeImage(image,length,pixels);
                MagickFreeMemory(pixels);
                if (!status)
                  {
                    CloseBlob(image);
                    return(False);
                  }
                break;
              }
            case NoCompression:
              {
                /*
                  Dump uncompressed PseudoColor packets.
                */
                Ascii85Initialize(image);
                for (y=0; y < (long) image->rows; y++)
                  {
                    p=AcquireImagePixels(image,0,y,image->columns,1,
                                         &image->exception);
                    if (p == (const PixelPacket *) NULL)
                      break;
                    for (x=0; x < (long) image->columns; x++)
                      {
                        Ascii85Encode(image,
                                      ScaleQuantumToChar(PixelIntensityToQuantum(p)));
                        p++;
                      }
                    if (image->previous == (Image *) NULL)
                      if (QuantumTick(y,image->rows))
                        {
                          status=MagickMonitorFormatted(y,image->rows,
                                                        &image->exception,
                                                        SaveImageText,
                                                        image->filename,
							image->columns,image->rows);
                          if (status == False)
                            break;
                        }
                  }
                Ascii85Flush(image);
                break;
              }
            }
        }
      else
        if ((image->storage_class == DirectClass) || (image->colors > 256) ||
            (compression == JPEGCompression))
          switch (compression)
            {
            case JPEGCompression:
              {
                unsigned char
                  *jpeg_blob;

                /*
                  Write image in JPEG format.
                */
		jpeg_blob=ImageToJPEGBlob(image,image_info,&length,&image->exception);
                if (jpeg_blob == (unsigned char *) NULL)
                  ThrowWriterException2(CoderError,image->exception.reason,image);
                (void) WriteBlob(image,length,jpeg_blob);
                MagickFreeMemory(jpeg_blob);
                break;
              }
            case RLECompression:
            default:
              {
                /*
                  Allocate pixel array.
                */
                length=(image->colorspace == CMYKColorspace ? 4 : 3)*number_pixels;
                pixels=MagickAllocateMemory(unsigned char *,length);
                if (pixels == (unsigned char *) NULL)
                  ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,
                                       image);
                /*
                  Dump runoffset encoded pixels.
                */
                q=pixels;
                for (y=0; y < (long) image->rows; y++)
                  {
                    p=AcquireImagePixels(image,0,y,image->columns,1,
                                         &image->exception);
                    if (p == (const PixelPacket *) NULL)
                      break;
                    for (x=0; x < (long) image->columns; x++)
                      {
                        if (image->matte && (p->opacity == TransparentOpacity))
                          {
                            *q++=ScaleQuantumToChar(MaxRGB);
                            *q++=ScaleQuantumToChar(MaxRGB);
                            *q++=ScaleQuantumToChar(MaxRGB);
                            p++;
                            continue;
                          }
                        *q++=ScaleQuantumToChar(p->red);
                        *q++=ScaleQuantumToChar(p->green);
                        *q++=ScaleQuantumToChar(p->blue);
                        if (image->colorspace == CMYKColorspace)
                          *q++=ScaleQuantumToChar(p->opacity);
                        p++;
                      }
                    if (image->previous == (Image *) NULL)
                      if (QuantumTick(y,image->rows))
                        {
                          status=MagickMonitorFormatted(y,image->rows,
                                                        &image->exception,
                                                        SaveImageText,
                                                        image->filename,
							image->columns,image->rows);
                          if (status == False)
                            break;
                        }
                  }
#if defined(HasZLIB)
                if (compression == ZipCompression)
                  status=ZLIBEncodeImage(image,length,image_info->quality,pixels);
                else
#endif /* defined(HasZLIB) */
                  if (compression == LZWCompression)
                    status=LZWEncodeImage(image,length,pixels);
                  else
                    status=PackbitsEncodeImage(image,length,pixels);
                MagickFreeMemory(pixels);
                if (!status)
                  {
                    CloseBlob(image);
                    return(False);
                  }
                break;
              }
            case NoCompression:
              {
                /*
                  Dump uncompressed DirectColor packets.
                */
                Ascii85Initialize(image);
                for (y=0; y < (long) image->rows; y++)
                  {
                    p=AcquireImagePixels(image,0,y,image->columns,1,
                                         &image->exception);
                    if (p == (const PixelPacket *) NULL)
                      break;
                    for (x=0; x < (long) image->columns; x++)
                      {
                        if (image->matte && (p->opacity == TransparentOpacity))
                          {
                            Ascii85Encode(image,ScaleQuantumToChar(MaxRGB));
                            Ascii85Encode(image,ScaleQuantumToChar(MaxRGB));
                            Ascii85Encode(image,ScaleQuantumToChar(MaxRGB));
                            continue;
                          }
                        Ascii85Encode(image,ScaleQuantumToChar(p->red));
                        Ascii85Encode(image,ScaleQuantumToChar(p->green));
                        Ascii85Encode(image,ScaleQuantumToChar(p->blue));
                        if (image->colorspace == CMYKColorspace)
                          Ascii85Encode(image,ScaleQuantumToChar(p->opacity));
                        p++;
                      }
                    if (image->previous == (Image *) NULL)
                      if (QuantumTick(y,image->rows))
                        {
                          status=MagickMonitorFormatted(y,image->rows,
                                                        &image->exception,
                                                        SaveImageText,
                                                        image->filename,
							image->columns,image->rows);
                          if (status == False)
                            break;
                        }
                  }
                Ascii85Flush(image);
                break;
              }
            }
        else
          {
            /*
              Dump number of colors and colormap.
            */
            switch (compression)
              {
              case RLECompression:
              default:
                {
                  /*
                    Allocate pixel array.
                  */
                  length=number_pixels;
                  pixels=MagickAllocateMemory(unsigned char *,length);
                  if (pixels == (unsigned char *) NULL)
                    ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
                  /*
                    Dump Runlength encoded pixels.
                  */
                  q=pixels;
                  for (y=0; y < (long) image->rows; y++)
                    {
                      p=AcquireImagePixels(image,0,y,image->columns,1,
                                           &image->exception);
                      if (p == (const PixelPacket *) NULL)
                        break;
                      indexes=AccessImmutableIndexes(image);
                      for (x=0; x < (long) image->columns; x++)
                        *q++=indexes[x];
                      if (image->previous == (Image *) NULL)
                        if (QuantumTick(y,image->rows))
                          {
                            status=MagickMonitorFormatted(y,image->rows,
                                                          &image->exception,
                                                          SaveImageText,
                                                          image->filename,
							  image->columns,image->rows);
                            if (status == False)
                              break;
                          }
                    }
#if defined(HasZLIB)
                  if (compression == ZipCompression)
                    status=ZLIBEncodeImage(image,length,image_info->quality,pixels);
                  else
#endif /* defined(HasZLIB) */
                    if (compression == LZWCompression)
                      status=LZWEncodeImage(image,length,pixels);
                    else
                      status=PackbitsEncodeImage(image,length,pixels);
                  MagickFreeMemory(pixels);
                  if (!status)
                    {
                      CloseBlob(image);
                      return(False);
                    }
                  break;
                }
              case NoCompression:
                {
                  /*
                    Dump uncompressed PseudoColor packets.
                  */
                  Ascii85Initialize(image);
                  for (y=0; y < (long) image->rows; y++)
                    {
                      p=AcquireImagePixels(image,0,y,image->columns,1,
                                           &image->exception);
                      if (p == (const PixelPacket *) NULL)
                        break;
                      indexes=AccessImmutableIndexes(image);
                      for (x=0; x < (long) image->columns; x++)
                        Ascii85Encode(image,indexes[x]);
                      if (image->previous == (Image *) NULL)
                        if (QuantumTick(y,image->rows))
                          {
                            status=MagickMonitorFormatted(y,image->rows,
                                                          &image->exception,
                                                          SaveImageText,
                                                          image->filename,
							  image->columns,image->rows);
                            if (status == False)
                              break;
                          }
                    }
                  Ascii85Flush(image);
                  break;
                }
              }
          }
      offset=TellBlob(image)-offset;
      (void) WriteBlobString(image,"\nendstream\n");
      (void) WriteBlobString(image,"endobj\n");
      /*
        Write Length object.
      */
      xref[object++]=TellBlob(image);
      FormatString(buffer,"%lu 0 obj\n",object);
      (void) WriteBlobString(image,buffer);
      FormatString(buffer,"%lu\n",(unsigned long) offset);
      (void) WriteBlobString(image,buffer);
      (void) WriteBlobString(image,"endobj\n");
      /*
        Write Colorspace object.
      */
      xref[object++]=TellBlob(image);
      FormatString(buffer,"%lu 0 obj\n",object);
      (void) WriteBlobString(image,buffer);
      if (image->colorspace == CMYKColorspace)
        {
          (void) strcpy(buffer,"/DeviceCMYK\n");
        }
      else
        {
          if ((compression == FaxCompression) ||
              (compression == Group4Compression) ||
              ((image_info->type != TrueColorType) &&
               (characteristics.grayscale)))
            {
              (void) strcpy(buffer,"/DeviceGray\n");
            }
          else
            {
              if ((image->storage_class == DirectClass) || (image->colors > 256) ||
                  (compression == JPEGCompression))
                (void) strcpy(buffer,"/DeviceRGB\n");
              else
                FormatString(buffer,"[ /Indexed /DeviceRGB %u %lu 0 R ]\n",
                             (unsigned int) image->colors-1,object+1);
            }
        }
      (void) WriteBlobString(image,buffer);
      (void) WriteBlobString(image,"endobj\n");
      /*
        Write Colormap object.
      */
      xref[object++]=TellBlob(image);
      FormatString(buffer,"%lu 0 obj\n",object);
      (void) WriteBlobString(image,buffer);
      (void) WriteBlobString(image,"<<\n");
      if ((image->storage_class != DirectClass) && (image->colors <= 256) &&
          (compression != FaxCompression))
        {
          if (compression == NoCompression)
            (void) WriteBlobString(image,"/Filter [ /ASCII85Decode ]\n");
          FormatString(buffer,"/Length %lu 0 R\n",object+1);
          (void) WriteBlobString(image,buffer);
          (void) WriteBlobString(image,">>\n");
          (void) WriteBlobString(image,"stream\n");
          offset=TellBlob(image);
          if (compression == NoCompression)
            Ascii85Initialize(image);
          for (i=0; i < (long) image->colors; i++)
            {
              if (compression == NoCompression)
                {
                  Ascii85Encode(image,ScaleQuantumToChar(image->colormap[i].red));
                  Ascii85Encode(image,ScaleQuantumToChar(image->colormap[i].green));
                  Ascii85Encode(image,ScaleQuantumToChar(image->colormap[i].blue));
                  continue;
                }
              (void) WriteBlobByte(image,
                                   ScaleQuantumToChar(image->colormap[i].red));
              (void) WriteBlobByte(image,
                                   ScaleQuantumToChar(image->colormap[i].green));
              (void) WriteBlobByte(image,
                                   ScaleQuantumToChar(image->colormap[i].blue));
            }
          if (compression == NoCompression)
            Ascii85Flush(image);
        }
      offset=TellBlob(image)-offset;
      (void) WriteBlobString(image,"\nendstream\n");
      (void) WriteBlobString(image,"endobj\n");
      /*
        Write Length object.
      */
      xref[object++]=TellBlob(image);
      FormatString(buffer,"%lu 0 obj\n",object);
      (void) WriteBlobString(image,buffer);
      FormatString(buffer,"%lu\n",(unsigned long) offset);
      (void) WriteBlobString(image,buffer);
      (void) WriteBlobString(image,"endobj\n");
      if (image->next == (Image *) NULL)
        break;
      image=SyncNextImageInList(image);
      status=MagickMonitorFormatted(scene++,GetImageListLength(image),
                                    &image->exception,SaveImagesText,
                                    image->filename);
      if (status == False)
        break;
    } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  /*
    Write Xref object.
  */
  offset=TellBlob(image)-xref[0]+10;
  (void) WriteBlobString(image,"xref\n");
  FormatString(buffer,"0 %lu\n",object+1);
  (void) WriteBlobString(image,buffer);
  (void) WriteBlobString(image,"0000000000 65535 f \n");
  for (i=0; i < (long) object; i++)
    {
      FormatString(buffer,"%010lu 00000 n \n",(unsigned long) xref[i]);
      (void) WriteBlobString(image,buffer);
    }
  (void) WriteBlobString(image,"trailer\n");
  (void) WriteBlobString(image,"<<\n");
  FormatString(buffer,"/Size %lu\n",object+1);
  (void) WriteBlobString(image,buffer);
  FormatString(buffer,"/Info %lu 0 R\n",info_id);
  (void) WriteBlobString(image,buffer);
  FormatString(buffer,"/Root %lu 0 R\n",root_id);
  (void) WriteBlobString(image,buffer);
  (void) WriteBlobString(image,">>\n");
  (void) WriteBlobString(image,"startxref\n");
  FormatString(buffer,"%lu\n",(unsigned long) offset);
  (void) WriteBlobString(image,buffer);
  (void) WriteBlobString(image,"%%EOF\n");
  MagickFreeMemory(xref);
  CloseBlob(image);
  MagickFreeMemory(fax_blob);
  return(True);
}

#if defined(HasZLIB)
#include "zlib.h"
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   Z L I B E n c o d e I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ZLIBEncodeImage compresses an image via ZLIB-coding specific to
%  Postscript Level II or Portable Document Format.  To ensure portability, the
%  binary ZLIB bytes are encoded as ASCII base-85.
%
%  The format of the ZLIBEncodeImage method is:
%
%      unsigned int ZLIBEncodeImage(Image *image,const size_t length,
%        const unsigned long quality,unsigned char *pixels)
%
%  A description of each parameter follows:
%
%    o status:  Method ZLIBEncodeImage returns True if all the pixels are
%      compressed without error, otherwise False.
%
%    o file: The address of a structure of type FILE.  ZLIB encoded pixels
%      are written to this file.
%
%    o length:  A value that specifies the number of pixels to compress.
%
%    o quality: the compression level (0-100).
%
%    o pixels: The address of an unsigned array of characters containing the
%      pixels to compress.
%
%
*/
static voidpf ZLIBAllocFunc(voidpf opaque, uInt items, uInt size)
{
  ARG_NOT_USED(opaque);
  return MagickMallocCleared((size_t) items*size);
}
static void ZLIBFreeFunc(voidpf opaque, voidpf address)
{
  ARG_NOT_USED(opaque);
  MagickFree(address);
}
static unsigned int ZLIBEncodeImage(Image *image,const size_t length,
  const unsigned long quality,unsigned char *pixels)
{
  int
    status;

  register long
    i;

  unsigned char
    *compressed_pixels;

  unsigned long
    compressed_packets;

  z_stream
    stream;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  compressed_packets=(unsigned long) (1.001*length+12);
  compressed_pixels=MagickAllocateMemory(unsigned char *,compressed_packets);
  if (compressed_pixels == (unsigned char *) NULL)
    ThrowBinaryException(ResourceLimitError,MemoryAllocationFailed,
      (char *) NULL);
  (void) memset(&stream,0,sizeof(stream));
  stream.next_in=pixels;
  stream.avail_in=(unsigned int) length;
  stream.next_out=compressed_pixels;
  stream.avail_out=(unsigned int) compressed_packets;
  stream.zalloc=ZLIBAllocFunc;
  stream.zfree=ZLIBFreeFunc;
  stream.opaque=(voidpf) NULL;
  status=deflateInit(&stream,(int) Min(quality/10,9));
  if (status == Z_OK)
    {
      status=deflate(&stream,Z_FINISH);
      if (status == Z_STREAM_END)
        status=deflateEnd(&stream);
      else
        (void) deflateEnd(&stream);
      compressed_packets=stream.total_out;
    }
  if (status)
    ThrowBinaryException(CoderError,UnableToZipCompressImage,(char *) NULL)
  else
    for (i=0; i < (long) compressed_packets; i++)
      (void) WriteBlobByte(image,compressed_pixels[i]);
  MagickFreeMemory(compressed_pixels);
  return(!status);
}
#else
static unsigned int ZLIBEncodeImage(Image *image,const size_t length,
  const unsigned long quality,unsigned char *pixels)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  ThrowBinaryException(MissingDelegateError,ZipLibraryIsNotAvailable,
    image->filename);
  return(False);
}
#endif
