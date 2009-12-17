/*
% Copyright (C) 2003 - 2008 GraphicsMagick Group
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
%                        FFFFF  IIIII  TTTTT  SSSSS                           %
%                        F        I      T    SS                              %
%                        FFF      I      T     SSS                            %
%                        F        I      T       SS                           %
%                        F      IIIII    T    SSSSS                           %
%                                                                             %
%                                                                             %
%            Read/Write Flexible Image Transport System Images.               %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
%                              Jaroslav Fojtik                                %
%                                2008 - 2009                                  %
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
#include "magick/colormap.h"
#include "magick/constitute.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/pixel_cache.h"
#include "magick/utility.h"
#include "magick/version.h"

/*
  Forward declarations.
*/
static unsigned int
  WriteFITSImage(const ImageInfo *,Image *);


#define FITS_BLOCK_SIZE 2880
#define FITS_ROW_SIZE 80


/* Convert signed values to unsigned - and reverse. */
static void FixSignedValues(unsigned char *data, int size, unsigned step, unsigned endian)
{
  if(endian != MSBEndian)
  {
    data += step - 1;  /* LSB has most signifficant byte at the end */
  }		       /* MSB has most signifficant byte first */

  while(size-->0)
  {
    *data ^= 0x80;	
    data += step;
  }     
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s F I T S                                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsFITS returns True if the image format type, identified by the
%  magick string, is FITS.
%
%  The format of the IsFITS method is:
%
%      unsigned int IsFITS(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsFITS returns True if the image format type is FITS.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsFITS(const unsigned char *magick,const size_t length)
{
  if (length < 6)
    return(False);
  if (LocaleNCompare((char *) magick,"IT0",3) == 0)
    return(True);
  if (LocaleNCompare((char *) magick,"SIMPLE",6) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d F I T S I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadFITSImage reads a FITS image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadFITSImage method is:
%
%      Image *ReadFITSImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: Method ReadFITSImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o filename: Specifies the name of the image to read.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadFITSImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  typedef struct _FITSInfo
  {
    char extensions_exist;
    int
      simple,
      bits_per_pixel,
      columns,
      rows,
      number_axes,
      number_scenes;

    double
      min_data,
      max_data,
      zero,
      scale;
  } FITSInfo;

  char
    keyword[FITS_ROW_SIZE+1],
    value[FITS_ROW_SIZE+1];  

  FITSInfo
    fits_info;

  Image
    *image; 

  int
    c;   

  int logging;

  long
    packet_size,
    scene,
    y,
    ax_number;

  register PixelPacket
    *q;

  unsigned char
    *fits_pixels;    

  unsigned int
    status,
    value_expected;

  unsigned long
    number_pixels;

  ImportPixelAreaOptions import_options;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);

  logging = LogMagickEvent(CoderEvent,GetMagickModule(),"enter"); 

  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    ThrowReaderException(FileOpenError,UnableToOpenFile,image);

  /*
    Initialize common part of image header.
  */
  fits_info.extensions_exist=0;
  fits_info.simple=False;

  ImportPixelAreaOptionsInit(&import_options);
  import_options.endian = MSBEndian;

  /*
    Decode image header.
  */
  if ((c=ReadBlobByte(image)) == EOF)
    ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);

ReadExtension:
  if (logging) (void)LogMagickEvent(CoderEvent,GetMagickModule(),
          "Reading FITS HDU at position: %Xh", (unsigned)TellBlob(image) );

  /*
    Initialize image header for all subheaders.
  */
  fits_info.bits_per_pixel=8;
  fits_info.columns=1;
  fits_info.rows=1;
  fits_info.number_axes=0;
  fits_info.number_scenes=1;
  fits_info.min_data=0.0;
  fits_info.max_data=0.0;
  fits_info.zero=0.0;
  fits_info.scale=1.0;
  number_pixels = 0;

  for ( ; ; )
  {
    if (!isalnum((int) c))
      c=ReadBlobByte(image);
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
          if ((p-keyword) < (FITS_ROW_SIZE-1))
            *p++=c;
          if ((c=ReadBlobByte(image)) == EOF)
            ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
        } while (isalnum(c) || (c == '_'));
        *p='\0';
        if (LocaleCompare(keyword,"END") == 0)
          break;
        value_expected=False;
        while (isspace(c) || (c == '='))
        {
          if (c == '=')
            value_expected=True;
          if ((c=ReadBlobByte(image)) == EOF)
            ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
        }
        if (value_expected == False)
          continue;
        p=value;
        if (c == '\'')
          c=ReadBlobByte(image);

        while (isalnum(c) || (c == '-') || (c == '+') || (c == '.'))
        {
          if ((p-value) < (FITS_ROW_SIZE-1))
            *p++=c;
          if ((c=ReadBlobByte(image)) == EOF)
            ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
        }
        *p='\0';
        /*
          Assign a value to the specified keyword.
        */
        if (LocaleCompare(keyword,"SIMPLE") == 0)
          fits_info.simple = (*value=='T') || (*value=='t');
        if (LocaleCompare(keyword,"EXTEND") == 0)
          fits_info.extensions_exist = (*value== 'T') || (*value=='t');
        if (LocaleCompare(keyword,"BITPIX") == 0)
        {
          fits_info.bits_per_pixel=atoi(value);
	  if(fits_info.bits_per_pixel>0)
            import_options.sample_type = UnsignedQuantumSampleType;
	  if(fits_info.bits_per_pixel<0)          
            import_options.sample_type = FloatQuantumSampleType;
        }
	if(!LocaleNCompare(keyword,"NAXIS",5))
        {
          if (keyword[5] == 0) ax_number=-1;          
          else
	  {
            if(isdigit(keyword[5]))
              ax_number = atoi(keyword+5);
            else ax_number=-2;			/*unsupported fits keyword*/
          }
	  y=0;
          if(ax_number>=-1)
	    y = atoi(value);
          switch(ax_number)
          {
            case -1:fits_info.number_axes = y; break;
            case 1: fits_info.columns = (y<=0) ? 1 : y; break;
	    case 2: fits_info.rows = (y<=0) ? 1 : y; break;
	    case 3: fits_info.number_scenes = (y<=0) ? 1 : y; break;
          }
          if(ax_number>0)
          {
            if(number_pixels == 0)
              number_pixels = y;
            else
              number_pixels *= (y<=0) ? 1 : y;
          }
        }

        if (LocaleCompare(keyword,"DATAMAX") == 0)
          fits_info.max_data=atof(value);
        if (LocaleCompare(keyword,"DATAMIN") == 0)
          fits_info.min_data=atof(value);
        if (LocaleCompare(keyword,"BZERO") == 0)
          fits_info.zero=atof(value);
        if (LocaleCompare(keyword,"BSCALE") == 0)
          fits_info.scale=atof(value);
        if (LocaleCompare(keyword,"XENDIAN") == 0)
        {
	  if (LocaleCompare(keyword,"BIG") == 0)
	    import_options.endian = MSBEndian;
          else
	    import_options.endian = LSBEndian;          
	}
      }
    while ((TellBlob(image) % 80) != 0)
      if ((c=ReadBlobByte(image)) == EOF)
        ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
    if ((c=ReadBlobByte(image)) == EOF)
      ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
  }

  while ((TellBlob(image) % FITS_BLOCK_SIZE) != 0)		/* Read till the rest of a block. */
    if ((c=ReadBlobByte(image)) == EOF)
      ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);

  /*
    Verify that required image information is defined.
  */  
  if (logging) (void)LogMagickEvent(CoderEvent,GetMagickModule(),
          "HDU read finished at %Xh, number of pixel is: %d (%d*%d*%d)", (unsigned)TellBlob(image), (unsigned)number_pixels,
          (unsigned)fits_info.columns, (unsigned)fits_info.rows, (unsigned)fits_info.number_scenes);

  if ((!fits_info.simple) || (fits_info.number_axes < 1) ||
      (fits_info.number_axes > 4) || (number_pixels == 0))
  {
    if(!fits_info.extensions_exist)	/* when extensions exists, process further */
      ThrowReaderException(CorruptImageError,ImageTypeNotSupported,image);

    number_pixels = (number_pixels*fits_info.bits_per_pixel) / 8;
    number_pixels = ((number_pixels+FITS_BLOCK_SIZE-1) / FITS_BLOCK_SIZE) * FITS_BLOCK_SIZE; /* raw data block size */
    (void) SeekBlob(image,number_pixels,SEEK_CUR);    
  }
  else
  {
   number_pixels = fits_info.columns*fits_info.rows; 
   for (scene=0; scene < (long) fits_info.number_scenes; scene++)
   {
      if(image->rows!=0 && image->columns!=0)
      {
        /*
          Allocate next image structure.
        */
        AllocateNextImage(image_info,image);
        if (image->next == (Image *) NULL)
          {
            DestroyImageList(image);
            return((Image *) NULL);
          }
        image=SyncNextImageInList(image);
        if (!MagickMonitorFormatted(TellBlob(image),GetBlobSize(image),exception,
                                    LoadImagesText,image->filename))
          break;
      }

    /*
      Create linear colormap.
    */
    image->columns = fits_info.columns;
    image->rows = fits_info.rows;
    if(fits_info.bits_per_pixel>=0)
      image->depth = Min(QuantumDepth,fits_info.bits_per_pixel);
    else
      image->depth = QuantumDepth;		/* double type cell */
    /* image->storage_class=PseudoClass; */
    image->storage_class = DirectClass;
    image->scene=scene;
    image->is_grayscale = 1;

    if (image->depth<=8 && fits_info.bits_per_pixel==8)
      if (!AllocateImageColormap(image,1 << image->depth))
        ThrowReaderException(FileOpenError,UnableToOpenFile,image);

    if (image_info->ping && (image_info->subrange != 0))
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;

    /*
      Initialize image structure.
    */
    packet_size=fits_info.bits_per_pixel/8;
    if (packet_size < 0) packet_size = -packet_size;
        
    number_pixels = image->columns*image->rows;
    if ((number_pixels / image->columns) != image->rows)
      ThrowReaderException(CoderError,ImageColumnOrRowSizeIsNotSupported,image);

    fits_pixels=MagickAllocateArray(unsigned char *, image->columns, packet_size);
    if (fits_pixels == (unsigned char *) NULL)
      ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);

    /*
      Recover minimum and maximum from data if not present in HDU
    */     
    if ((fits_info.min_data == 0.0) && (fits_info.max_data == 0.0))
    {  /*  Determine minimum and maximum intensity. */
      if(fits_info.bits_per_pixel==-32)
        (void) MagickFindRawImageMinMax(image, import_options.endian, image->columns,
                                        image->rows, FloatPixel, packet_size*image->columns,
                                        fits_pixels, &import_options.double_minvalue,
                                        &import_options.double_maxvalue);
      if(fits_info.bits_per_pixel==-64)
        (void) MagickFindRawImageMinMax(image, import_options.endian, image->columns,
                                        image->rows, DoublePixel, packet_size*image->columns,
                                        fits_pixels, &import_options.double_minvalue,
                                        &import_options.double_maxvalue);
    }
    else
    {
      import_options.double_maxvalue = fits_info.max_data;
      import_options.double_minvalue = fits_info.min_data;
    }

    /*
      Convert FITS pixels to pixel packets.
    */

    for (y=(long) image->rows-1; y >= 0; y--)
    {
      q=SetImagePixels(image,0,y,image->columns,1);
      if (q == (PixelPacket *) NULL)
        break;

      if(ReadBlob(image, packet_size*image->columns, fits_pixels) != (size_t)packet_size*image->columns)
      {
 	if (logging) (void)LogMagickEvent(CoderEvent,GetMagickModule(),
           "  fits cannot read scanrow %u from a file.", (unsigned)y );
	break; /* goto ExitLoop; */
      }

      switch(fits_info.bits_per_pixel)
      {
        case 16: FixSignedValues(fits_pixels, image->columns, 2, import_options.endian);
		 break;
        case 32: FixSignedValues(fits_pixels, image->columns, 4, import_options.endian);
		 break;
	case 64: FixSignedValues(fits_pixels, image->columns, 8, import_options.endian);
		 break;
      }

      if(ImportImagePixelArea(image, GrayQuantum, packet_size*8, fits_pixels, &import_options,0) == MagickFail)
      {
        if (logging) (void)LogMagickEvent(CoderEvent,GetMagickModule(),
               "  fits failed to ImportImagePixelArea for a row %u", (unsigned)y);
	break;
      }

      if (!SyncImagePixels(image))
        break;
      if (QuantumTick(y,image->rows))
        if (!MagickMonitorFormatted(y,image->rows,exception,
                                    LoadImageText,image->filename,
				    image->columns,image->rows))
          break;
    }
    MagickFreeMemory(fits_pixels);
    if (EOFBlob(image))
      {
        ThrowException(exception,CorruptImageError,UnexpectedEndOfFile, image->filename);
        break;
      }

    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;    
    }
  }

  if(fits_info.extensions_exist)
  {
    while ((TellBlob(image) % FITS_BLOCK_SIZE) != 0) /* Read till the rest of a block. */
      if ((c=ReadBlobByte(image)) == EOF)
        break;
    if(!EOFBlob(image))
    {						/* Try to read a next extension block header. */
      if ((c=ReadBlobByte(image)) != EOF)
	goto ReadExtension;
    }
  }

  CloseBlob(image);

  while (image->previous != (Image *) NULL)
    image=image->previous;  

  if (logging) (void)LogMagickEvent(CoderEvent,GetMagickModule(),"return");  
  if(image==NULL)
    ThrowReaderException(CorruptImageError,ImageFileDoesNotContainAnyImageData,image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r F I T S I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterFITSImage adds attributes for the FITS image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterFITSImage method is:
%
%      RegisterFITSImage(void)
%
*/
ModuleExport void RegisterFITSImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("FITS");
  entry->decoder=(DecoderHandler) ReadFITSImage;
  entry->encoder=(EncoderHandler) WriteFITSImage;
  entry->magick=(MagickHandler) IsFITS;
  entry->adjoin=False;
  entry->description="Flexible Image Transport System";
  entry->module="FITS";
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r F I T S I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterFITSImage removes format registrations made by the
%  FITS module from the list of supported formats.
%
%  The format of the UnregisterFITSImage method is:
%
%      UnregisterFITSImage(void)
%
*/
ModuleExport void UnregisterFITSImage(void)
{
  (void) UnregisterMagickInfo("FITS");
}


/*
  This functions inserts one row into a HDU. Note that according to
  FITS spec a card image contains 80 bytes of ASCII data.
  
  buffer - 2880 byte logical FITS record.
  data   - string data to append
  offset - offset into FITS record to write the data.
*/
int InsertRowHDU(char *buffer, const char *data, int offset)
{
  int
    len;

  if (data == NULL)
    return offset;
  len = strlen(data);
  len = Min(len,80); /* Each card image is 80 bytes max */

  if (len > FITS_BLOCK_SIZE-offset) len = FITS_BLOCK_SIZE-offset;

  (void) strncpy(buffer+offset,data,len);
  return offset +80;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e F I T S I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteFITSImage writes a Flexible Image Transport System image to a
%  file as gray scale intensities [0..255].
%
%  The format of the WriteFITSImage method is:
%
%      unsigned int WriteFITSImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteFITSImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
static unsigned int WriteFITSImage(const ImageInfo *image_info,Image *image)
{
  char
    buffer[FITS_BLOCK_SIZE],
    *fits_info;

  long
    y;

  register const PixelPacket
    *p;  

  unsigned char
    *pixels;

  unsigned int    
    quantum_size,
    status;

  unsigned long
    packet_size;

  ExportPixelAreaOptions export_options;

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

  ExportPixelAreaOptionsInit(&export_options);
  export_options.endian = MSBEndian;
  export_options.sample_type = UnsignedQuantumSampleType;

  if (image->depth <= 8)
  {
    quantum_size=8;    
  }
  else if (image->depth <= 16)
  {    
    quantum_size=16;
  }
  else
  {   
    quantum_size=32;
  }

  /*
    Allocate image memory.
  */  
  packet_size=quantum_size/8;
  fits_info=MagickAllocateMemory(char *,FITS_BLOCK_SIZE);
  pixels=MagickAllocateMemory(unsigned char *,packet_size*image->columns);
  if ((fits_info == (char *) NULL) || (pixels == (unsigned char *) NULL))
    ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);

  /*
    Initialize image header.
  */  
  memset(fits_info,' ',FITS_BLOCK_SIZE);
  y = 0;
  y = InsertRowHDU(fits_info, "SIMPLE  =                    T", y);
  FormatString(buffer,        "BITPIX  =                    %u",quantum_size);
  y = InsertRowHDU(fits_info, buffer, y);
  y = InsertRowHDU(fits_info, "NAXIS   =                    2", y);
  FormatString(buffer,        "NAXIS1  =           %10lu",image->columns);
  y = InsertRowHDU(fits_info, buffer, y);
  FormatString(buffer,        "NAXIS2  =           %10lu",image->rows); 
  y = InsertRowHDU(fits_info, buffer, y);  
  FormatString(buffer,        "DATAMIN =           %10u",0);
  y = InsertRowHDU(fits_info, buffer, y);    
  FormatString(buffer,        "DATAMAX =           %10lu", MaxValueGivenBits(quantum_size));
  y = InsertRowHDU(fits_info, buffer, y);
  if(quantum_size>8)
  {
    FormatString(buffer,      "BZERO   =           %10u", (quantum_size <= 16) ? 32768U : 2147483648U);
    y = InsertRowHDU(fits_info, buffer, y);
  }
  /* Magick version data can only be 60 bytes. */
  (void) FormatString(buffer, "HISTORY Created by %.60s.",MagickPackageName " " MagickLibVersionText);
  y = InsertRowHDU(fits_info, buffer, y);
  y = InsertRowHDU(fits_info, "END", y);        
  (void) WriteBlob(image, FITS_BLOCK_SIZE, (char *)fits_info);
  
  /*
    Convert image to fits scale PseudoColor class.
  */
  for (y=(long) image->rows-1; y >= 0; y--)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    (void) ExportImagePixelArea(image,GrayQuantum,quantum_size,pixels,&export_options,0);
    if(quantum_size==16) FixSignedValues(pixels, image->columns, 2, export_options.endian);
    if(quantum_size==32) FixSignedValues(pixels, image->columns, 4, export_options.endian);
    (void) WriteBlob(image,packet_size*image->columns,pixels);
    if (QuantumTick(image->rows-y-1,image->rows))
      {
        status=MagickMonitorFormatted(image->rows-y-1,image->rows,
                                      &image->exception,SaveImageText,
                                      image->filename,
				      image->columns,image->rows);
        if (status == False)
          break;
      }
  }
  
	/* Calculate of padding */
  y = FITS_BLOCK_SIZE - (image->columns * image->rows * packet_size) % FITS_BLOCK_SIZE;
  if(y>0)
  {
    memset(fits_info, 0, y);    
    (void)WriteBlob(image,y,(char *) fits_info);
  }
  MagickFreeMemory(fits_info);

  MagickFreeMemory(pixels);
  CloseBlob(image);
  return(True);
}
