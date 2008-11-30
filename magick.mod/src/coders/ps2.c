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
%                            PPPP   SSSSS  22222                              %
%                            P   P  SS        22                              %
%                            PPPP    SSS    222                               %
%                            P         SS  22                                 %
%                            P      SSSSS  22222                              %
%                                                                             %
%                                                                             %
%                     Write Postscript Level II Format.                       %
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
#include "magick/attribute.h"
#include "magick/blob.h"
#include "magick/pixel_cache.h"
#include "magick/color.h"
#include "magick/constitute.h"
#include "magick/compress.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/tempfile.h"
#include "magick/utility.h"
#if defined(HasTIFF)
#define CCITTParam  "-1"
#else
#define CCITTParam  "0"
#endif

/*
  Forward declarations.
*/
static unsigned int
  WritePS2Image(const ImageInfo *,Image *);

#if defined(HasTIFF)
#if defined(HAVE_TIFFCONF_H)
#include "tiffconf.h"
#endif
#include "tiffio.h"
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   H u f f m a n 2 D E n c o d e I m a g e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Huffman2DEncodeImage compresses an image via two-dimensional
%  Huffman-coding.
%
%  The format of the Huffman2DEncodeImage method is:
%
%      unsigned int Huffman2DEncodeImage(const ImageInfo *image_info,
%        Image *image)
%
%  A description of each parameter follows:
%
%    o status:  Method Huffman2DEncodeImage returns True if all the pixels are
%      compressed without error, otherwise False.
%
%    o image_info: The image info..
%
%    o image: The image.
%
*/
static unsigned int Huffman2DEncodeImage(const ImageInfo *image_info,
  Image *image)
{
  char
    filename[MaxTextExtent];

  Image
    *huffman_image;

  ImageInfo
    *clone_info;

  long
    count,
    j;

  register long
    i;

  TIFF
    *tiff;

  uint16
    fillorder;

  unsigned char
    *buffer;

  unsigned int
    status;

  unsigned long
    *byte_count,
    strip_size;

  /*
    Write image as CCITTFax4 TIFF image to a temporary file.
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  huffman_image=CloneImage(image,0,0,True,&image->exception);
  if (huffman_image == (Image *) NULL)
    return(False);
  (void) SetImageType(huffman_image,BilevelType);
  if(!AcquireTemporaryFileName(filename))
    {
      DestroyImage(huffman_image);
      ThrowBinaryException(FileOpenError,UnableToCreateTemporaryFile,
        filename);
    }
  FormatString(huffman_image->filename,"tiff:%s",filename);
  clone_info=CloneImageInfo(image_info);
  clone_info->compression=Group4Compression;
  status=WriteImage(clone_info,huffman_image);
  DestroyImageInfo(clone_info);
  DestroyImage(huffman_image);
  if (status == False)
    return(False);
  tiff=TIFFOpen(filename,"rb");
  if (tiff == (TIFF *) NULL)
    {
      (void) LiberateTemporaryFile(filename);
      ThrowBinaryException(FileOpenError,UnableToOpenFile,
        image_info->filename)
    }
  /*
    Allocate raw strip buffer.
  */
  (void) TIFFGetField(tiff,TIFFTAG_STRIPBYTECOUNTS,&byte_count);
  strip_size=byte_count[0];
  for (i=1; i < (long) TIFFNumberOfStrips(tiff); i++)
    if (byte_count[i] > strip_size)
      strip_size=byte_count[i];
  buffer=MagickAllocateMemory(unsigned char *,strip_size);
  if (buffer == (unsigned char *) NULL)
    {
      TIFFClose(tiff);
      (void) LiberateTemporaryFile(filename);
      ThrowBinaryException(ResourceLimitError,MemoryAllocationFailed,
        (char *) NULL)
    }
  /*
    Compress runlength encoded to 2D Huffman pixels.
  */
  (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_FILLORDER,&fillorder);
  for (i=0; i < (long) TIFFNumberOfStrips(tiff); i++)
  {
    Ascii85Initialize(image);
    count=TIFFReadRawStrip(tiff,(uint32) i,buffer,(long) byte_count[i]);
    if (fillorder == FILLORDER_LSB2MSB)
      TIFFReverseBits(buffer,count);
    for (j=0; j < count; j++)
      Ascii85Encode(image,(unsigned long) buffer[j]);
    Ascii85Flush(image);
  }
  MagickFreeMemory(buffer);
  TIFFClose(tiff);
  (void) LiberateTemporaryFile(filename);
  return(True);
}
#else
static unsigned int Huffman2DEncodeImage(const ImageInfo *image_info,
  Image *image)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  ThrowBinaryException(MissingDelegateError,TIFFLibraryIsNotAvailable,image->filename);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r P S 2 I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterPS2Image adds attributes for the PS2 image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterPS2Image method is:
%
%      RegisterPS2Image(void)
%
*/
ModuleExport void RegisterPS2Image(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("EPS2");
  entry->encoder=(EncoderHandler) WritePS2Image;
  entry->adjoin=False;
  entry->seekable_stream=True;
  entry->description="Adobe Level II Encapsulated PostScript";
  entry->module="PS2";
  entry->coder_class=PrimaryCoderClass;
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("PS2");
  entry->encoder=(EncoderHandler) WritePS2Image;
  entry->seekable_stream=True;
  entry->description="Adobe Level II PostScript";
  entry->module="PS2";
  entry->coder_class=PrimaryCoderClass;
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r P S 2 I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterPS2Image removes format registrations made by the
%  PS2 module from the list of supported formats.
%
%  The format of the UnregisterPS2Image method is:
%
%      UnregisterPS2Image(void)
%
*/
ModuleExport void UnregisterPS2Image(void)
{
  (void) UnregisterMagickInfo("EPS2");
  (void) UnregisterMagickInfo("PS2");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P S 2 I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePS2Image translates an image to encapsulated Postscript
%  Level II for printing.  If the supplied geometry is null, the image is
%  centered on the Postscript page.  Otherwise, the image is positioned as
%  specified by the geometry.
%
%  The format of the WritePS2Image method is:
%
%      unsigned int WritePS2Image(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows:
%
%    o status: Method WritePS2Image return True if the image is printed.
%      False is returned if the image file cannot be opened for printing.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
static unsigned int WritePS2Image(const ImageInfo *image_info,Image *image)
{
  static const char
    *PostscriptProlog[]=
    {
      "%%%%BeginProlog",
      "%%",
      "%% Display a color image.  The image is displayed in color on",
      "%% Postscript viewers or printers that support color, otherwise",
      "%% it is displayed as grayscale.",
      "%%",
      "/DirectClassImage",
      "{",
      "  %%",
      "  %% Display a DirectClass image.",
      "  %%",
      "  colorspace 0 eq",
      "  {",
      "    /DeviceRGB setcolorspace",
      "    <<",
      "      /ImageType 1",
      "      /Width columns",
      "      /Height rows",
      "      /BitsPerComponent 8",
      "      /Decode [0 1 0 1 0 1]",
      "      /ImageMatrix [columns 0 0 rows neg 0 rows]",
      "      compression 0 gt",
      "      { /DataSource pixel_stream /%.1024s filter }",
      "      { /DataSource pixel_stream /%.1024s filter } ifelse",
      "    >> image",
      "  }",
      "  {",
      "    /DeviceCMYK setcolorspace",
      "    <<",
      "      /ImageType 1",
      "      /Width columns",
      "      /Height rows",
      "      /BitsPerComponent 8",
      "      /Decode [1 0 1 0 1 0 1 0]",
      "      /ImageMatrix [columns 0 0 rows neg 0 rows]",
      "      compression 0 gt",
      "      { /DataSource pixel_stream /%.1024s filter }",
      "      { /DataSource pixel_stream /%.1024s filter } ifelse",
      "    >> image",
      "  } ifelse",
      "} bind def",
      "",
      "/PseudoClassImage",
      "{",
      "  %%",
      "  %% Display a PseudoClass image.",
      "  %%",
      "  %% Parameters:",
      "  %%   colors: number of colors in the colormap.",
      "  %%",
      "  currentfile buffer readline pop",
      "  token pop /colors exch def pop",
      "  colors 0 eq",
      "  {",
      "    %%",
      "    %% Image is grayscale.",
      "    %%",
      "    currentfile buffer readline pop",
      "    token pop /bits exch def pop",
      "    /DeviceGray setcolorspace",
      "    <<",
      "      /ImageType 1",
      "      /Width columns",
      "      /Height rows",
      "      /BitsPerComponent bits",
      "      /Decode [0 1]",
      "      /ImageMatrix [columns 0 0 rows neg 0 rows]",
      "      compression 0 gt",
      "      { /DataSource pixel_stream /%.1024s filter }",
      "      {",
      "        /DataSource pixel_stream /%.1024s filter",
      "        <<",
      "           /K "CCITTParam,
      "           /Columns columns",
      "           /Rows rows",
      "        >> /CCITTFaxDecode filter",
      "      } ifelse",
      "    >> image",
      "  }",
      "  {",
      "    %%",
      "    %% Parameters:",
      "    %%   colormap: red, green, blue color packets.",
      "    %%",
      "    /colormap colors 3 mul string def",
      "    currentfile colormap readhexstring pop pop",
      "    currentfile buffer readline pop",
      "    [ /Indexed /DeviceRGB colors 1 sub colormap ] setcolorspace",
      "    <<",
      "      /ImageType 1",
      "      /Width columns",
      "      /Height rows",
      "      /BitsPerComponent 8",
      "      /Decode [0 255]",
      "      /ImageMatrix [columns 0 0 rows neg 0 rows]",
      "      compression 0 gt",
      "      { /DataSource pixel_stream /%.1024s filter }",
      "      { /DataSource pixel_stream /%.1024s filter } ifelse",
      "    >> image",
      "  } ifelse",
      "} bind def",
      "",
      "/DisplayImage",
      "{",
      "  %%",
      "  %% Display a DirectClass or PseudoClass image.",
      "  %%",
      "  %% Parameters:",
      "  %%   x & y translation.",
      "  %%   x & y scale.",
      "  %%   label pointsize.",
      "  %%   image label.",
      "  %%   image columns & rows.",
      "  %%   class: 0-DirectClass or 1-PseudoClass.",
      "  %%   colorspace: 0-RGB or 1-CMYK.",
      "  %%   compression: 0-RLECompression or 1-NoCompression.",
      "  %%   hex color packets.",
      "  %%",
      "  gsave",
      "  /buffer 512 string def",
      "  /pixel_stream currentfile def",
      "",
      "  currentfile buffer readline pop",
      "  token pop /x exch def",
      "  token pop /y exch def pop",
      "  x y translate",
      "  currentfile buffer readline pop",
      "  token pop /x exch def",
      "  token pop /y exch def pop",
      "  currentfile buffer readline pop",
      "  token pop /pointsize exch def pop",
      "  /Helvetica findfont pointsize scalefont setfont",
      (char *) NULL
    },
    *PostscriptEpilog[]=
    {
      "  x y scale",
      "  currentfile buffer readline pop",
      "  token pop /columns exch def",
      "  token pop /rows exch def pop",
      "  currentfile buffer readline pop",
      "  token pop /class exch def pop",
      "  currentfile buffer readline pop",
      "  token pop /colorspace exch def pop",
      "  currentfile buffer readline pop",
      "  token pop /compression exch def pop",
      "  class 0 gt { PseudoClassImage } { DirectClassImage } ifelse",
      "  grestore",
      (char *) NULL
    };

  char
    buffer[MaxTextExtent],
    date[MaxTextExtent],
    density[MaxTextExtent],
    page_geometry[MaxTextExtent],
    **labels;

  const char
    **q;

  CompressionType
    compression;

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
    start,
    stop;

  Image
    *jpeg_image;

  int
    count,
    status;

  long
    j,
    y;

  RectangleInfo
    geometry;

  register const PixelPacket
    *p;

  register const IndexPacket
    *indexes;

  register long
    x;

  register long
    i;

  SegmentInfo
    bounds={0.0,0.0,0.0,0.0};

  size_t
    length;

  time_t
    timer;

  unsigned char
    *pixels;

  unsigned long
    number_pixels,
    page,
    scene,
    text_size;

  void
    *blob;

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
  compression=image->compression;
  if (image_info->compression != UndefinedCompression)
    compression=image_info->compression;
  switch (compression)
  {
#if !defined(HasJPEG)
    case JPEGCompression:
    {
      compression=RLECompression;
      ThrowException(&image->exception,MissingDelegateError,JPEGLibraryIsNotAvailable,image->filename);
      break;
    }
#endif
#if !defined(HasZLIB)
    case ZipCompression:
    {
      compression=RLECompression;
      ThrowException(&image->exception,MissingDelegateError,ZipLibraryIsNotAvailable,image->filename);
      break;
    }
#endif
    default:
      break;
  }
  page=1;
  scene=0;
  do
  {
    ImageCharacteristics
      characteristics;

    /*
      Scale image to size of Postscript page.
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
        if (LocaleCompare(image_info->magick,"PS2") == 0)
          (void) strcpy(page_geometry,PSPageGeometry);
    (void) GetMagickGeometry(page_geometry,&geometry.x,&geometry.y,
       &geometry.width,&geometry.height);
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
        count=GetMagickDimension(image_info->density,&x_resolution,
          &y_resolution,NULL,NULL);
        if (count != 2)
          y_resolution=x_resolution;
      }
    x_scale=(geometry.width*dx_resolution)/x_resolution;
    geometry.width=(unsigned long) (x_scale+0.5);
    y_scale=(geometry.height*dy_resolution)/y_resolution;
    geometry.height=(unsigned long) (y_scale+0.5);
    if (page == 1)
      {
        /*
          Output Postscript header.
        */
        if (LocaleCompare(image_info->magick,"PS2") == 0)
          (void) strcpy(buffer,"%!PS-Adobe-3.0\n");
        else
          (void) strcpy(buffer,"%!PS-Adobe-3.0 EPSF-3.0\n");
        (void) WriteBlobString(image,buffer);
        (void) WriteBlobString(image,"%%Creator: (GraphicsMagick)\n");
        FormatString(buffer,"%%%%Title: (%.1024s)\n",image->filename);
        (void) WriteBlobString(image,buffer);
        timer=time((time_t *) NULL);
        (void) localtime(&timer);
        (void) strlcpy(date,ctime(&timer),MaxTextExtent);
        date[strlen(date)-1]='\0';
        FormatString(buffer,"%%%%CreationDate: (%.1024s)\n",date);
        (void) WriteBlobString(image,buffer);
        bounds.x1=geometry.x;
        bounds.y1=geometry.y;
        bounds.x2=geometry.x+geometry.width;
        bounds.y2=geometry.y+geometry.height+text_size;
        if (image_info->adjoin && (image->next != (Image *) NULL))
          (void) strcpy(buffer,"%%BoundingBox: (atend)\n");
        else
          FormatString(buffer,"%%%%BoundingBox: %g %g %g %g\n",
            floor(bounds.x1+0.5),floor(bounds.y1+0.5),ceil(bounds.x2-0.5),
            ceil(bounds.y2-0.5));
        (void) WriteBlobString(image,buffer);
        attribute=GetImageAttribute(image,"label");
        if (attribute != (const ImageAttribute *) NULL)
          (void) WriteBlobString(image,
            "%%DocumentNeededResources: font Helvetica\n");
        (void) WriteBlobString(image,"%%LanguageLevel: 2\n");
        if (LocaleCompare(image_info->magick,"PS2") != 0)
          (void) WriteBlobString(image,"%%Pages: 1\n");
        else
          {
            (void) WriteBlobString(image,"%%Orientation: Portrait\n");
            (void) WriteBlobString(image,"%%PageOrder: Ascend\n");
            if (!image_info->adjoin)
              (void) strcpy(buffer,"%%Pages: 1\n");
            else
              FormatString(buffer,"%%%%Pages: %lu\n",(unsigned long)
                GetImageListLength(image));
            (void) WriteBlobString(image,buffer);
          }
        (void) WriteBlobString(image,"%%EndComments\n");
        (void) WriteBlobString(image,"\n%%BeginDefaults\n");
        (void) WriteBlobString(image,"%%EndDefaults\n\n");
        /*
          Output Postscript commands.
        */
        for (q=PostscriptProlog; *q; q++)
        {
          switch (compression)
          {
            case NoCompression: FormatString(buffer,*q,"ASCII85Decode"); break;
            case JPEGCompression: FormatString(buffer,*q,"DCTDecode"); break;
            case LZWCompression: FormatString(buffer,*q,"LZWDecode"); break;
            case FaxCompression: FormatString(buffer,*q,"ASCII85Decode"); break;
            default: FormatString(buffer,*q,"RunLengthDecode"); break;
          }
          (void) WriteBlobString(image,buffer);
          (void) WriteBlobByte(image,'\n');
        }
        attribute=GetImageAttribute(image,"label");
        if (attribute != (const ImageAttribute *) NULL)
          for (j=(long) MultilineCensus(attribute->value)-1; j >= 0; j--)
          {
            (void) WriteBlobString(image,"  /label 512 string def\n");
            (void) WriteBlobString(image,"  currentfile label readline pop\n");
            FormatString(buffer,"  0 y %g add moveto label show pop\n",
              j*image_info->pointsize+12);
            (void) WriteBlobString(image,buffer);
          }
        for (q=PostscriptEpilog; *q; q++)
          {
            FormatString(buffer,"%.1024s\n",*q);
            (void) WriteBlobString(image,buffer);
          }
        if (LocaleCompare(image_info->magick,"PS2") == 0)
          (void) WriteBlobString(image,"  showpage\n");
        (void) WriteBlobString(image,"} bind def\n");
        (void) WriteBlobString(image,"%%EndProlog\n");
      }
    FormatString(buffer,"%%%%Page:  1 %lu\n",page++);
    (void) WriteBlobString(image,buffer);
    FormatString(buffer,"%%%%PageBoundingBox: %ld %ld %ld %ld\n",geometry.x,
      geometry.y,geometry.x+(long) geometry.width,geometry.y+(long)
      (geometry.height+text_size));
    (void) WriteBlobString(image,buffer);
    if (geometry.x < bounds.x1)
      bounds.x1=geometry.x;
    if (geometry.y < bounds.y1)
      bounds.y1=geometry.y;
    if ((geometry.x+(long) geometry.width-1) > bounds.x2)
      bounds.x2=geometry.x+geometry.width-1;
    if ((geometry.y+(long) (geometry.height+text_size)-1) > bounds.y2)
      bounds.y2=geometry.y+(geometry.height+text_size)-1;
    attribute=GetImageAttribute(image,"label");
    if (attribute != (const ImageAttribute *) NULL)
      (void) WriteBlobString(image,"%%PageResources: font Times-Roman\n");
    if (LocaleCompare(image_info->magick,"PS2") != 0)
      (void) WriteBlobString(image,"userdict begin\n");
    start=TellBlob(image);
    FormatString(buffer,"%%%%BeginData:%13ld %s Bytes\n",0L,
      compression == NoCompression ? "ASCII" : "Binary");
    (void) WriteBlobString(image,buffer);
    stop=TellBlob(image);
    (void) WriteBlobString(image,"DisplayImage\n");
    /*
      Output image data.
    */
    FormatString(buffer,"%ld %ld\n%g %g\n%f\n",geometry.x,geometry.y,
      x_scale,y_scale,image_info->pointsize);
    (void) WriteBlobString(image,buffer);
    labels=(char **) NULL;
    attribute=GetImageAttribute(image,"label");
    if (attribute != (const ImageAttribute *) NULL)
      labels=StringToList(attribute->value);
    if (labels != (char **) NULL)
      {
        for (i=0; labels[i] != (char *) NULL; i++)
        {
          FormatString(buffer,"%.1024s \n",labels[i]);
          (void) WriteBlobString(image,buffer);
          MagickFreeMemory(labels[i]);
        }
        MagickFreeMemory(labels);
      }
    number_pixels=image->columns*image->rows;

    /*
      Analyze image to be written.
    */
    (void) GetImageCharacteristics(image,&characteristics,
                                   (OptimizeType == image_info->type),
                                   &image->exception);
    if ((compression == FaxCompression) ||
        ((image_info->type != TrueColorType) &&
         (characteristics.grayscale)))
      {
        FormatString(buffer,"%lu %lu\n1\n%d\n",image->columns,image->rows,
          (int) (image->colorspace == CMYKColorspace));
        (void) WriteBlobString(image,buffer);
        FormatString(buffer,"%d\n",(int) (compression != FaxCompression));
        (void) WriteBlobString(image,buffer);
        (void) WriteBlobString(image,"0\n");
        FormatString(buffer,"%d\n",compression == FaxCompression ? 1 : 8);
        (void) WriteBlobString(image,buffer);
        switch (compression)
        {
          case FaxCompression:
          {
            if (LocaleCompare(CCITTParam,"0") == 0)
              {
                (void) HuffmanEncodeImage(image_info,image);
                break;
              }
            (void) Huffman2DEncodeImage(image_info,image);
            break;
          }
          case JPEGCompression:
          {
            /*
              Write image in JPEG format.
            */
            jpeg_image=CloneImage(image,0,0,True,&image->exception);
            if (jpeg_image == (Image *) NULL)
              ThrowWriterException2(CoderError,image->exception.reason,image);
            (void) strcpy(jpeg_image->magick,"JPEG");
            blob=ImageToBlob(image_info,jpeg_image,&length,&image->exception);
            (void) WriteBlob(image,length,blob);
            DestroyImage(jpeg_image);
            MagickFreeMemory(blob);
            break;
          }
          case RLECompression:
          default:
          {
            register unsigned char
              *q;

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
                                                  image->filename);
                    if (status == False)
                      break;
                  }
            }
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
                                                  image->filename);
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
        {
          FormatString(buffer,"%lu %lu\n0\n%d\n",image->columns,image->rows,
            (int) (image->colorspace == CMYKColorspace));
          (void) WriteBlobString(image,buffer);
          FormatString(buffer,"%d\n",(int) (compression == NoCompression));
          (void) WriteBlobString(image,buffer);
          switch (compression)
          {
            case JPEGCompression:
            {
              /*
                Write image in JPEG format.
              */
              jpeg_image=CloneImage(image,0,0,True,&image->exception);
              if (jpeg_image == (Image *) NULL)
                ThrowWriterException2(CoderError,image->exception.reason,image);
              (void) strcpy(jpeg_image->magick,"JPEG");
              blob=ImageToBlob(image_info,jpeg_image,&length,&image->exception);
              (void) WriteBlob(image,length,blob);
              DestroyImage(jpeg_image);
              MagickFreeMemory(blob);
              break;
            }
            case RLECompression:
            default:
            {
              register unsigned char
                *q;

              /*
                Allocate pixel array.
              */
              length=(image->colorspace == CMYKColorspace ? 4 : 3)*
                number_pixels;
              pixels=MagickAllocateMemory(unsigned char *,length);
              if (pixels == (unsigned char *) NULL)
                ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
              /*
                Dump Packbit encoded pixels.
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
                    }
                  else
                    if (image->colorspace != CMYKColorspace)
                      {
                        *q++=ScaleQuantumToChar(p->red);
                        *q++=ScaleQuantumToChar(p->green);
                        *q++=ScaleQuantumToChar(p->blue);
                      }
                    else
                      {
                        *q++=ScaleQuantumToChar(p->red);
                        *q++=ScaleQuantumToChar(p->green);
                        *q++=ScaleQuantumToChar(p->blue);
                        *q++=ScaleQuantumToChar(p->opacity);
                      }
                  p++;
                }
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(y,image->rows))
                    {
                      status=MagickMonitorFormatted(y,image->rows,
                                                    &image->exception,
                                                    SaveImageText,
                                                    image->filename);
                      if (status == False)
                        break;
                    }
              }
              if (compression == LZWCompression)
                status=LZWEncodeImage(image,length,pixels);
              else
                status=PackbitsEncodeImage(image,length,pixels);
              if (!status)
                {
                  CloseBlob(image);
                  return(False);
                }
              MagickFreeMemory(pixels);
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
                    }
                  else
                    if (image->colorspace != CMYKColorspace)
                      {
                        Ascii85Encode(image,ScaleQuantumToChar(p->red));
                        Ascii85Encode(image,ScaleQuantumToChar(p->green));
                        Ascii85Encode(image,ScaleQuantumToChar(p->blue));
                      }
                    else
                      {
                        Ascii85Encode(image,ScaleQuantumToChar(p->red));
                        Ascii85Encode(image,ScaleQuantumToChar(p->green));
                        Ascii85Encode(image,ScaleQuantumToChar(p->blue));
                        Ascii85Encode(image,ScaleQuantumToChar(p->opacity));
                      }
                  p++;
                }
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(y,image->rows))
                    {
                      status=MagickMonitorFormatted(y,image->rows,
                                                    &image->exception,
                                                    SaveImageText,
                                                    image->filename);
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
        {
          /*
            Dump number of colors and colormap.
          */
          FormatString(buffer,"%lu %lu\n1\n%d\n",image->columns,image->rows,
            (int) (image->colorspace == CMYKColorspace));
          (void) WriteBlobString(image,buffer);
          FormatString(buffer,"%d\n",(int) (compression == NoCompression));
          (void) WriteBlobString(image,buffer);
          FormatString(buffer,"%u\n",image->colors);
          (void) WriteBlobString(image,buffer);
          for (i=0; i < (long) image->colors; i++)
          {
            FormatString(buffer,"%02X%02X%02X\n",
              ScaleQuantumToChar(image->colormap[i].red),
              ScaleQuantumToChar(image->colormap[i].green),
              ScaleQuantumToChar(image->colormap[i].blue));
            (void) WriteBlobString(image,buffer);
          }
          switch (compression)
          {
            case RLECompression:
            default:
            {
              register unsigned char
                *q;

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
                                                    image->filename);
                      if (status == False)
                        break;
                    }
              }
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
                                                    image->filename);
                      if (status == False)
                        break;
                    }
              }
              Ascii85Flush(image);
              break;
            }
          }
        }
    (void) WriteBlobByte(image,'\n');
    length=TellBlob(image)-stop;
    stop=TellBlob(image);
    (void) SeekBlob(image,start,SEEK_SET);
    FormatString(buffer,"%%%%BeginData:%13ld %s Bytes\n",(long) length,
      compression == NoCompression ? "ASCII" : "Binary");
    (void) WriteBlobString(image,buffer);
    (void) SeekBlob(image,stop,SEEK_SET);
    (void) WriteBlobString(image,"%%EndData\n");
    if (LocaleCompare(image_info->magick,"PS2") != 0)
      (void) WriteBlobString(image,"end\n");
    (void) WriteBlobString(image,"%%PageTrailer\n");
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
  (void) WriteBlobString(image,"%%Trailer\n");
  if (page > 1)
    {
      FormatString(buffer,"%%%%BoundingBox: %g %g %g %g\n",floor(bounds.x1+0.5),
        floor(bounds.y1+0.5),ceil(bounds.x2-0.5),ceil(bounds.y2-0.5));
      (void) WriteBlobString(image,buffer);
    }
  (void) WriteBlobString(image,"%%EOF\n");
  CloseBlob(image);
  return(True);
}
