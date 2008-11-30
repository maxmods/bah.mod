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
%                            EEEEE  PPPP   TTTTT                              %
%                            E      P   P    T                                %
%                            EEE    PPPP     T                                %
%                            E      P        T                                %
%                            EEEEE  P        T                                %
%                                                                             %
%                                                                             %
%           Read/Write Encapsulated Postscript Format (with preview).         %
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
#include "magick/constitute.h"
#include "magick/delegate.h"
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/render.h"
#include "magick/tempfile.h"
#include "magick/utility.h"

/*
  Forward declarations.
*/
static unsigned int
  WriteEPTImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s E P T                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsEPT returns True if the image format type, identified by the
%  magick string, is EPT.
%
%  The format of the IsEPT method is:
%
%      unsigned int IsEPT(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsEPT returns True if the image format type is EPT.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsEPT(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(False);
  if (memcmp(magick,"\305\320\323\306",4) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d E P T I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadEPTImage reads a binary Adobe Postscript image file and returns
%  it.  It allocates the memory necessary for the new Image structure and
%  returns a pointer to the new image.
%
%  The format of the ReadEPTImage method is:
%
%      Image *ReadEPTImage(const ImageInfo *image_info,
         ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadEPTImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadEPTImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
#define BoundingBox  "%%BoundingBox:"
#define DocumentMedia  "%%DocumentMedia:"
#define PageBoundingBox  "%%PageBoundingBox:"
#define PostscriptLevel  "%!PS-"
#define RenderPostscriptText  "[%s] Rendering postscript..."

  char
    density[MaxTextExtent],
    command[MaxTextExtent],
    filename[MaxTextExtent],
    geometry[MaxTextExtent],
    options[MaxTextExtent],
    postscript_filename[MaxTextExtent],
    translate_geometry[MaxTextExtent];

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
    c,
    status;

  unsigned int
    antialias=4;

  ExtendedSignedIntegralType
    filesize;

  RectangleInfo
    box,
    page;

  register char
    *p;

  register long
    i;

  SegmentInfo
    bounds;

  size_t
    count;

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
    Set the page geometry.
  */
  dx_resolution=72.0;
  dy_resolution=72.0;
  if ((image->x_resolution == 0.0) || (image->y_resolution == 0.0))
    {
      (void) strcpy(density,PSDensityGeometry);
      count=GetMagickDimension(density,&image->x_resolution,
        &image->y_resolution,NULL,NULL);
      if (count != 2)
        image->y_resolution=image->x_resolution;
    }
  FormatString(density,"%gx%g",image->x_resolution,image->y_resolution);
  if (image->logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "Density: %s", density);
  SetGeometry(image,&page);
  page.width=612;
  page.height=792;
  (void) GetGeometry(PSPageGeometry,&page.x,&page.y,&page.width,&page.height);
  /*
    Determine page geometry from the Postscript bounding box.
  */
  (void) ReadBlobLSBLong(image);
  count=ReadBlobLSBLong(image);
  filesize=ReadBlobLSBLong(image);
  if (image->logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "File Size: %lu,  Offset: %lu",
                          (unsigned long) filesize, (unsigned long) count);
  if (EOFBlob(image))
    ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
  for (i=0; i < (long) (count-12); i++)
    if (ReadBlobByte(image) == EOF)
      ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
  /*
    Open temporary output file.
  */
  file=AcquireTemporaryFileStream(postscript_filename,BinaryFileIOMode);
  if (file == (FILE *) NULL)
      ThrowReaderTemporaryFileException(postscript_filename);
  FormatString(translate_geometry,"%g %g translate\n              ",0.0,0.0);
  (void) fputs(translate_geometry,file);
  /*
    Copy Postscript to temporary file.
  */
  box.width=0;
  box.height=0;
  p=command;
  if (image->logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "Copying Postscript to temporary file \"%s\" ...",
                          postscript_filename);
  for (i=0; i < (long) filesize; i++)
  {
    if ((c=ReadBlobByte(image)) == EOF)
      break;
    (void) fputc(c,file);
    *p++=c;
    if ((c != '\n') && (c != '\r') && ((p-command) < (MaxTextExtent-1)))
      continue;
    *p='\0';
    p=command;
    /*
      Parse a bounding box statement.
    */
    count=0;
    if (LocaleNCompare(BoundingBox,command,strlen(BoundingBox)) == 0)
      count=sscanf(command,"%%%%BoundingBox: %lf %lf %lf %lf",&bounds.x1,
        &bounds.y1,&bounds.x2,&bounds.y2);
    if (LocaleNCompare(DocumentMedia,command,strlen(DocumentMedia)) == 0)
      count=sscanf(command,"%%%%DocumentMedia: %*s %lf %lf",&bounds.x2,
        &bounds.y2)+2;
    if (LocaleNCompare(PageBoundingBox,command,strlen(PageBoundingBox)) == 0)
      count=sscanf(command,"%%%%PageBoundingBox: %lf %lf %lf %lf",
        &bounds.x1,&bounds.y1,&bounds.x2,&bounds.y2);
    if (count != 4)
      continue;
    if ((bounds.x1 > bounds.x2) ||
        (bounds.y1 > bounds.y2))
      continue;
    /*
      Set Postscript render geometry.
    */
    FormatString(translate_geometry,"%g %g translate\n",-bounds.x1,-bounds.y1);
    width=(unsigned long) (bounds.x2-bounds.x1+0.5);
    height=(unsigned long) (bounds.y2-bounds.y1+0.5);
    if ((width <= box.width) && (height <= box.height))
      continue;
    page.width=width;
    page.height=height;
    box=page;
  }
  if (image->logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "Done copying.");
  if (image_info->page != (char *) NULL)
    (void) GetGeometry(image_info->page,&page.x,&page.y,&page.width,
      &page.height);
  FormatString(geometry,"%lux%lu",
    (unsigned long) ceil(page.width*image->x_resolution/dx_resolution-0.5),
    (unsigned long) ceil(page.height*image->y_resolution/dy_resolution-0.5));
  if (image->logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "Page geometry: %s",geometry);
  if (ferror(file))
    {
      (void) fclose(file);
      (void) LiberateTemporaryFile(postscript_filename);
      ThrowReaderException(CorruptImageError,AnErrorHasOccurredWritingToFile,
        image)
    }
  (void) rewind(file);
  (void) fputs(translate_geometry,file);
  (void) fclose(file);
  CloseBlob(image);
  filesize=GetBlobSize(image);
  DestroyImage(image);
  /*
    Use Ghostscript to convert Postscript image.
  */
  *options='\0';
  if (image_info->subrange != 0)
    FormatString(options,"-dFirstPage=%lu -dLastPage=%lu",
      image_info->subimage+1,image_info->subimage+image_info->subrange);
  (void) strlcpy(filename,image_info->filename,MaxTextExtent);
  if (image_info->temporary)
    (void) LiberateTemporaryFile((char *) image_info->filename);
  if(!AcquireTemporaryFileName((char *)image_info->filename))
    {
      (void) LiberateTemporaryFile(postscript_filename);
      ThrowReaderTemporaryFileException(image_info->filename);
    }
  FormatString(command,delegate_info->commands,antialias,
    antialias,geometry,density,options,image_info->filename,
    postscript_filename);
  (void) MagickMonitorFormatted(0,8,&image->exception,RenderPostscriptText,
                                image->filename);
  status=InvokePostscriptDelegate(image_info->verbose,command);
  if (!IsAccessibleAndNotEmpty(image_info->filename))
    {
      /*
        Ghostscript requires a showpage operator.
      */
      file=fopen(postscript_filename,"ab");
      if (file == (FILE *) NULL)
        ThrowReaderException(FileOpenError,UnableToWriteFile,image);
      (void) fputs("showpage\n",file);
      (void) fclose(file);
       (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                             "Invoking Postscript delegate ...");
      status=InvokePostscriptDelegate(image_info->verbose,command);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "Returned from Postscript delegate (status=%d) ...", status);
    }
  (void) LiberateTemporaryFile(postscript_filename);
  (void) MagickMonitorFormatted(7,8,&image->exception,RenderPostscriptText,
                                image->filename);
  if (!IsAccessibleAndNotEmpty(image_info->filename))
    {
      /*
        Ghostscript has failed-- try the Display Postscript Extension.
      */
      (void) FormatString((char *) image_info->filename,"dps:%.1024s",filename);
      image=ReadImage(image_info,exception);
      if (image != (Image *) NULL)
        return(image);
      ThrowReaderException(DelegateError,PostscriptDelegateFailed,image)
    }
  clone_info=CloneImageInfo(image_info);
  clone_info->blob=(void *) NULL;
  clone_info->length=0;
  clone_info->magick[0]='\0';
  image=ReadImage(clone_info,exception);
  DestroyImageInfo(clone_info);
  (void) LiberateTemporaryFile((char *) image_info->filename);
  if (image == (Image *) NULL)
    ThrowReaderException(DelegateError,PostscriptDelegateFailed,image);
  do
  {
    (void) strcpy(image->magick,"PS");
    (void) strlcpy(image->filename,filename,MaxTextExtent);
    next_image=SyncNextImageInList(image);
    if (next_image != (Image *) NULL)
      image=next_image;
  } while (next_image != (Image *) NULL);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r E P T I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterEPTImage adds attributes for the EPT image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterEPTImage method is:
%
%      RegisterEPTImage(void)
%
*/
ModuleExport void RegisterEPTImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("EPT");
  entry->decoder=(DecoderHandler) ReadEPTImage;
  entry->encoder=(EncoderHandler) WriteEPTImage;
  entry->magick=(MagickHandler) IsEPT;
  entry->adjoin=False;
  entry->blob_support=False;
  entry->description="Adobe Encapsulated PostScript with MS-DOS TIFF preview";
  entry->module="EPT";
  entry->coder_class=PrimaryCoderClass;
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("EPT2");
  entry->decoder=(DecoderHandler) ReadEPTImage;
  entry->encoder=(EncoderHandler) WriteEPTImage;
  entry->magick=(MagickHandler) IsEPT;
  entry->adjoin=False;
  entry->blob_support=False;
  entry->description="Adobe Level II Encapsulated PostScript with MS-DOS TIFF preview";
  entry->module="EPT";
  entry->coder_class=PrimaryCoderClass;
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("EPT3");
  entry->decoder=(DecoderHandler) ReadEPTImage;
  entry->encoder=(EncoderHandler) WriteEPTImage;
  entry->magick=(MagickHandler) IsEPT;
  entry->adjoin=False;
  entry->blob_support=False;
  entry->description="Adobe Level III Encapsulated PostScript with MS-DOS TIFF preview";
  entry->module="EPT";
  entry->coder_class=PrimaryCoderClass;
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r E P T I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterEPTImage removes format registrations made by the
%  EPT module from the list of supported formats.
%
%  The format of the UnregisterEPTImage method is:
%
%      UnregisterEPTImage(void)
%
*/
ModuleExport void UnregisterEPTImage(void)
{
  (void) UnregisterMagickInfo("EPT");
  (void) UnregisterMagickInfo("EPT2");
  (void) UnregisterMagickInfo("EPT3");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e E P T I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteEPTImage writes an image in the Adobe Encapsulated Postscript
%  format with a TIFF preview.
%
%  The format of the WriteEPTImage method is:
%
%      unsigned int WriteEPTImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteEPTImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
static unsigned int WriteEPTImage(const ImageInfo *image_info,Image *image)
{
  char
    filename[MaxTextExtent],
    ps_filename[MaxTextExtent],
    tiff_filename[MaxTextExtent];

  FILE
    *ps_file,
    *tiff_file;

  unsigned int
    logging,
    status;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  logging=IsEventLogging();

  (void) strlcpy(filename,image->filename,MaxTextExtent);
  (void) strlcpy(ps_filename,image->magick_filename,MaxTextExtent);
  if (LocaleCompare(image_info->magick,"EPS") != 0)
    {
      /*
        Write image as Encapsulated Postscript to a temporary file.
      */
      char
        subformat[MaxTextExtent];

      if(!AcquireTemporaryFileName(ps_filename))
        ThrowWriterTemporaryFileException(ps_filename);

      /* Select desired EPS level */
      (void) strcpy(subformat,"eps");
      if (LocaleCompare(image_info->magick,"EPT2") == 0)
        (void) strcpy(subformat,"eps2");
      else if (LocaleCompare(image_info->magick,"EPT3") == 0)
        (void) strcpy(subformat,"eps3");

      /* JPEG compression requires at least EPS2 */
      if ((image->compression == JPEGCompression) &&
          (LocaleCompare(subformat,"EPS") == 0))
        (void) strcpy(subformat,"eps2");

      FormatString(image->filename,"%s:%.1024s",subformat,ps_filename);
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "Writing temporary EPS file \"%s\"",ps_filename);
      (void) WriteImage(image_info,image);
    }
  /*
    Write image as TIFF to a temporary file.
  */
  if(!AcquireTemporaryFileName(tiff_filename))
    {
      (void) LiberateTemporaryFile(ps_filename);
      ThrowWriterTemporaryFileException(tiff_filename);
    }
  
  FormatString(image->filename,"tiff:%.1024s",tiff_filename);
  image->compression=RLECompression;
  if (logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "Writing temporary TIFF preview file \"%s\"",tiff_filename);
  (void) WriteImage(image_info,image);
  /*
    Write EPT image.
  */
  (void) strlcpy(image->filename,filename,MaxTextExtent);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  ps_file=fopen(ps_filename,"rb");
  status&=ps_file != (FILE *) NULL;
  if (logging && (ps_file == 0))
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
       "Failed to open \"%s\" for read",ps_filename);
  tiff_file=fopen(tiff_filename,"rb");
  if (logging && (tiff_file == 0))
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "Failed to open \"%s\" for read",tiff_filename);
  status&=tiff_file != (FILE *) NULL;
  if (status != False)
    {
      int
        c;

      struct stat
        attributes;

      /*
        Write EPT image.
      */

      /* MS-DOS EPS binary file magic signature */
      (void) WriteBlobLSBLong(image,0xc6d3d0c5ul);
      /* Byte position in file for start of Postscript language code
         section */
      (void) WriteBlobLSBLong(image,30);
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "EPS section offset is %lu bytes",(unsigned long) 30);
      /* Byte length of PostScript language section. */
      attributes.st_size=0;
      (void) fstat(fileno(ps_file),&attributes);
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "EPS section is %lu bytes long",(unsigned long)attributes.st_size);
      (void) WriteBlobLSBLong(image,(unsigned long) attributes.st_size);
      /* Byte position in file for start of Metafile screen
         representation (none provided). */
      (void) WriteBlobLSBLong(image,0);
      /* Byte length of Metafile section (PSize). (none provided) */
      (void) WriteBlobLSBLong(image,0);
      /* Byte position of TIFF representation. */
      (void) WriteBlobLSBLong(image,(unsigned long) attributes.st_size+30);
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "TIFF section offset is %lu bytes",(unsigned long) attributes.st_size+30);
      /* Byte length of TIFF section. */
      (void) fstat(fileno(tiff_file),&attributes);
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "TIFF section is %lu bytes long",(unsigned long) attributes.st_size);
      (void) WriteBlobLSBLong(image,(long) attributes.st_size);
      /* Checksum of header (XOR of bytes 0-27). If Checksum is FFFF
         then ignore it. This is lazy code. */
      (void) WriteBlobLSBShort(image,0xffff);
      /* EPS section */
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "Writing EPS section at offset %ld",(long) TellBlob(image));
      for (c=fgetc(ps_file); c != EOF; c=fgetc(ps_file))
        (void) WriteBlobByte(image,c);
      /* TIFF section */
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "Writing TIFF section at offset %ld",(long) TellBlob(image));
      for (c=fgetc(tiff_file); c != EOF; c=fgetc(tiff_file))
        (void) WriteBlobByte(image,c);
      CloseBlob(image);
    }
  (void) fclose(ps_file);
  (void) fclose(tiff_file);
  if (LocaleCompare(image_info->magick,"EPS") != 0)
    (void) LiberateTemporaryFile(ps_filename);
  (void) LiberateTemporaryFile(tiff_filename);
  if (status == False)
    ThrowWriterException(FileOpenError,UnableToOpenFile,image);
  return(True);
}
