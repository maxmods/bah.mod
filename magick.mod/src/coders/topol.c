/*
% Copyright (C) 2003 GraphicsMagick Group
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                    TTTTT   OOO    PPPP    OOO    L                          %
%                      T    O   O   P   P  O   O   L                          %
%                      T    O   O   PPPP   O   O   L                          %
%                      T    O   O   P      O   O   L                          %
%                      T     OOO    P       OOO    LLLL                       %
%                                                                             %
%                                                                             %
%                    Read/Write TOPOL X Image Raster Format.                  %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                              Jaroslav Fojtik                                %
%                                 June 2003                                   %
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
#include "magick/error.h"
#include "magick/list.h"
#include "magick/magick.h"
#include "magick/pixel_cache.h"
#include "magick/utility.h"


typedef struct
{
  char Name[20];
  magick_uint16_t Rows;
  magick_uint16_t Cols;
  magick_uint16_t FileType;	/* 0-binary, 1-8 bitu, 2-8 bits+PAL, 3-4 bits,
				   4-4 bits+PAL, 5-24 bits, 6-16 bits, 7-32
				   bits */
  magick_uint32_t Zoom;
  magick_uint16_t Version;
  magick_uint16_t Komprese;		/* 0 - uncompressed (from release 1) */
  magick_uint16_t Stav;
  double xRasMin;
  double yRasMin;
  double xRasMax;
  double yRasMax;
  double Scale;			/* from release 2 */
  magick_uint16_t TileWidth;	/* tile width in pixels */
  magick_uint16_t TileHeight;	/* tile height in pixels */
  magick_uint32_t TileOffsets;	/* offset to array of longints that contains
				   adresses of tiles in the raster (adreses
				   are counted from 0) */
  magick_uint32_t TileByteCounts;/* offset to array of words, that contain amount of bytes stored in
				   tiles. The tile size might vary depending on
				   the value TileCompression */
  magick_uint8_t TileCompression;/* 0 - uncompressed, 1 - variant TIFF
				   Packbits, 2 - CCITT G3 */

  magick_uint8_t Dummy[423];
} RasHeader;

/*
typedef struct			The palette record inside TopoL
{
   BYTE Flag;
   BYTE Red;
   BYTE Green;
   BYTE Blue;
} paletteRAS;*/

static void InsertRow(int depth, unsigned char *p, long y, Image * image, unsigned Xoffset, unsigned columns, ImportPixelAreaOptions *import_options)
{
  int
    bit;

  long
    x;

  register PixelPacket
    *q;

  IndexPacket
    index;

  register IndexPacket
    *indexes;

  switch (depth)
    {
    case 1:			/* Convert bitmap scanline. */
      {
	q = SetImagePixels(image, Xoffset, y, columns, 1);
	if (q == (PixelPacket *) NULL)
	  break;
	indexes = AccessMutableIndexes(image);
	for (x = 0; x < ((long)columns - 7); x += 8)
          {
            for (bit = 0; bit < 8; bit++)
              {
                index = ((*p) & (0x80U >> bit) ? 0x01U : 0x00U);
                indexes[x + bit] = index;
                *q++ = image->colormap[index];
              }
            p++;
          }
	if ((columns % 8) != 0)
          {
            for (bit = 0; bit < (long)(columns % 8); bit++)
              {
                index = ((*p) & (0x80 >> bit) ? 0x01 : 0x00);
                indexes[x + bit] = index;
                *q++ = image->colormap[index];
              }
            p++;
          }
	if (!SyncImagePixels(image))
	  break;
	/* if (image->previous == (Image *) NULL)
	   if (QuantumTick(y,image->rows))
	   ProgressMonitor(LoadImageText,image->rows-y-1,image->rows); */
	break;
      }
    case 2:			/* Convert PseudoColor scanline. */
      {
	q = SetImagePixels(image, Xoffset, y, columns, 1);
	if (q == (PixelPacket *) NULL)
	  break;
	indexes = AccessMutableIndexes(image);
	for (x = 0; x < ((long)columns - 1); x += 2)
          {
            index = (IndexPacket) ((*p >> 6) & 0x3);
            VerifyColormapIndex(image, index);
            indexes[x] = index;
            *q++ = image->colormap[index];
            index = (IndexPacket) ((*p >> 4) & 0x3);
            VerifyColormapIndex(image, index);
            indexes[x] = index;
            *q++ = image->colormap[index];
            index = (IndexPacket) ((*p >> 2) & 0x3);
            VerifyColormapIndex(image, index);
            indexes[x] = index;
            *q++ = image->colormap[index];
            index = (IndexPacket) ((*p) & 0x3);
            VerifyColormapIndex(image, index);
            indexes[x + 1] = index;
            *q++ = image->colormap[index];
            p++;
          }
	if ((columns % 4) != 0)
          {
            index = (IndexPacket) ((*p >> 6) & 0x3);
            VerifyColormapIndex(image, index);
            indexes[x] = index;
            *q++ = image->colormap[index];
            if ((columns % 4) >= 1)
              {
                index = (IndexPacket) ((*p >> 4) & 0x3);
                VerifyColormapIndex(image, index);
                indexes[x] = index;
                *q++ = image->colormap[index];
                if((columns % 4) >= 2)
                  {
                    index = (IndexPacket) ((*p >> 2) & 0x3);
                    VerifyColormapIndex(image, index);
                    indexes[x] = index;
                    *q++ = image->colormap[index];
                  }
              }
            p++;
          }
	if (!SyncImagePixels(image))
	  break;
	/*         if (image->previous == (Image *) NULL)
                   if (QuantumTick(y,image->rows))
                   ProgressMonitor(LoadImageText,image->rows-y-1,image->rows); */
	break;
      }

    case 4:			/* Convert PseudoColor scanline. */
      {
	q = SetImagePixels(image, Xoffset, y, columns, 1);
	if (q == (PixelPacket *) NULL)
	  break;
	indexes = AccessMutableIndexes(image);
	for (x = 0; x < ((long)columns - 1); x += 2)
          {
            index = (IndexPacket) ((*p >> 4) & 0xF);	/* Lo nibble */
            VerifyColormapIndex(image, index);
            indexes[x] = index;
            *q++ = image->colormap[index];
            index = (IndexPacket) ((*p) & 0xF);		/* Hi nibble */
            VerifyColormapIndex(image, index);
            indexes[x + 1] = index;
            *q++ = image->colormap[index];
            p++;
          }
	if((columns % 2) != 0)
          {
            index = (IndexPacket) ((*p >> 4) & 0xF);	/* padding nibble */
            VerifyColormapIndex(image, index);
            indexes[x] = index;
            *q++ = image->colormap[index];
            p++;
          }
	if (!SyncImagePixels(image))
	  break;
	/*         if (image->previous == (Image *) NULL)
                   if (QuantumTick(y,image->rows))
                   ProgressMonitor(LoadImageText,image->rows-y-1,image->rows); */
	break;
      }
    case 8:			/* Convert PseudoColor scanline. */
      {
	q = SetImagePixels(image, Xoffset, y, columns, 1);
	if (q == (PixelPacket *) NULL)
	  break;
	indexes = AccessMutableIndexes(image);

	for (x = 0; x < (long)columns; x++)
          {
            index = (IndexPacket) (*p);
            VerifyColormapIndex(image, index);
            indexes[x] = index;
            *q++ = image->colormap[index];
            p++;
          }
	if (!SyncImagePixels(image))
	  break;
	/*           if (image->previous == (Image *) NULL)
                     if (QuantumTick(y,image->rows))
                     ProgressMonitor(LoadImageText,image->rows-y-1,image->rows); */
      }
      break;

    case 16:		/* Convert 16 bit Gray scanline. */
      q = SetImagePixels(image, Xoffset, y, columns, 1);
      if (q == (PixelPacket *) NULL)
	  break;	
      (void)ImportImagePixelArea(image,GrayQuantum,16,p,import_options,0);
      if(!SyncImagePixels(image)) break;
      break;

    case 24:		/* Convert RGB scanline. */
      q = SetImagePixels(image, Xoffset, y, columns, 1);
      if (q == (PixelPacket *) NULL)
	  break;	
      (void)ImportImagePixelArea(image,RGBQuantum,8,p,import_options,0);
      if(!SyncImagePixels(image)) break;
      break;

    case 32:		/* Convert 32 bit Gray scanline. */
      q = SetImagePixels(image, Xoffset, y, columns, 1);
      if (q == (PixelPacket *) NULL)
	  break;	
      (void)ImportImagePixelArea(image,GrayQuantum,32,p,import_options,0);
      if(!SyncImagePixels(image)) break;
      break;
      
    }
}


/* This function reads one block of unsigned longS */
static void ReadBlobDwordLSB(Image *image, size_t len, magick_uint32_t *data)
{
  while (len >= 4)
    {
      *data++ = ReadBlobLSBLong(image);
      len -= 4;
    }
  if (len > 0)
    (void) SeekBlob(image, len, SEEK_CUR);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d T O P O L I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadTOPOLImage reads an TOPOL X image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadTOPOLImage method is:
%
%      Image *ReadTOPOLImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadTOPOLImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: The image info.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadTOPOLImage(const ImageInfo * image_info, ExceptionInfo * exception)
{
  Image
    *image,
    *palette;

  ImageInfo
    *clone_info;

  RasHeader
    Header;

  int logging;

  int
    depth,    
    status;    

  long
    i,
    j,
    ldblk;

  unsigned char
    *BImgBuff = NULL,
    MEZ[256];
  ImportPixelAreaOptions import_options;


  palette = NULL;
  clone_info = NULL;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);

  logging = LogMagickEvent(CoderEvent,GetMagickModule(),"enter"); 

  image = AllocateImage(image_info);
  status = OpenBlob(image_info, image, ReadBinaryBlobMode, exception);
  if (status == False)
    ThrowReaderException(FileOpenError, UnableToOpenFile, image);

  ImportPixelAreaOptionsInit(&import_options);
  import_options.endian = LSBEndian;
  import_options.sample_type = UnsignedQuantumSampleType;

  /*
    Read TopoL RAS header.
  */
  (void) memset(&Header, 0, sizeof(Header));
  (void) ReadBlob(image, 20, &Header.Name);
  Header.Rows = ReadBlobLSBShort(image);
  Header.Cols = ReadBlobLSBShort(image);
  Header.FileType = ReadBlobLSBShort(image);
  Header.Zoom = ReadBlobLSBLong(image);
  Header.Version = ReadBlobLSBShort(image);
  if (Header.Version >= 1)
    {
      Header.Komprese = ReadBlobLSBShort(image);
      Header.Stav = ReadBlobLSBShort(image);
      Header.xRasMin = ReadBlobLSBDouble(image);
      Header.yRasMin = ReadBlobLSBDouble(image);
      Header.xRasMax = ReadBlobLSBDouble(image);
      Header.yRasMax = ReadBlobLSBDouble(image);
      if (Header.Version >= 2)	/* from release 2 */
        {
          Header.Scale = ReadBlobLSBDouble(image);
          Header.TileWidth = ReadBlobLSBShort(image);
          Header.TileHeight = ReadBlobLSBShort(image);
          Header.TileOffsets = ReadBlobLSBLong(image);
          Header.TileByteCounts = ReadBlobLSBLong(image);
          Header.TileCompression = ReadBlobByte(image);
          /* BYTE Dummy[423]; */
        }
    }

  for (i = 0; i < (long) sizeof(Header.Name); i++)
    {
      if (Header.Name[i] < ' ')
TOPOL_KO:              ThrowReaderException(CorruptImageError,ImproperImageHeader, image);
    }
  if (Header.Komprese != 0 || (Header.Version >= 2 && Header.TileCompression != 0))
    ThrowReaderException(CorruptImageError, UnrecognizedImageCompression, image);
  if (Header.Rows == 0 || Header.Cols == 0)
    goto TOPOL_KO;
  if (Header.Version > 2)
    ThrowReaderException(CorruptImageError, InvalidFileFormatVersion, image); /* unknown version */

  switch(Header.FileType)
    {
    case 0:
      image->colors = 2;
      depth = 1;
      break;
    case 1:
    case 2:
      image->colors = 256;
      depth = 8;
      break;
    case 3:
    case 4:
      image->colors = 16;
      depth = 4;
      break;
    case 5:
      image->colors = 0;
      image->depth = 8;
      depth = 24;
      break;
    case 6:
      image->colors = 0;
      depth = 16;
      break;			/* ????????? 16 bits */
    case 7:
      image->colors = 0;
      depth = 32;
      break;			/* ????????? 32 bits */
    default:
      goto TOPOL_KO;
    }

  image->columns = Header.Cols;
  image->rows = Header.Rows;

  /* If ping is true, then only set image size and colors without reading any image data. */
  if (image_info->ping) goto DONE_READING;

  /* ----- Handle the reindexing mez file ----- */
  j = image->colors;
  if(j<=0 || j>256) j=256;
  for(i=0; i<j; i++)
  {    
    MEZ[i] = (unsigned char)((i*256)/j);
  }
    
  if(Header.FileType>=5) goto NoMEZ;    
    
  if ((clone_info=CloneImageInfo(image_info)) == NULL) goto NoMEZ;
  
  i=(long) strlen(clone_info->filename);
  j=i;
  while(--i>0)
    {
      if(clone_info->filename[i]=='.') 
      {
        break;
      }
      if(clone_info->filename[i]=='/' || clone_info->filename[i]=='\\' || clone_info->filename[i]==':' )
      {
        i=j;
        break;
      }
    }
  
  (void) strcpy(clone_info->filename+i,".MEZ");
  if((clone_info->file=fopen(clone_info->filename,"rb"))==NULL)
    {
      (void) strcpy(clone_info->filename+i,".mez");
      if((clone_info->file=fopen(clone_info->filename,"rb"))==NULL)
        {
          DestroyImageInfo(clone_info);
          clone_info=NULL;
          goto NoMEZ;            
        }
    }

  if( (palette=AllocateImage(clone_info))==NULL ) goto NoMEZ;
  status=OpenBlob(clone_info,palette,ReadBinaryBlobMode,exception);
  if (status == False) goto NoMEZ; 

  ldblk=(long) GetBlobSize(palette);
  if ( ldblk > (long) sizeof(MEZ))
    ldblk=sizeof(MEZ);
  (void) ReadBlob(palette, ldblk, MEZ); 
            
NoMEZ:		/*Clean up palette and clone_info*/
  if (palette != NULL) {DestroyImage(palette);palette=NULL;}
  if (clone_info != NULL) 
    {
      DestroyImageInfo(clone_info);
      clone_info=NULL;
    }

  /* ----- Do something with palette ----- */
  if(Header.FileType==5) goto NoPalette;
  if ((clone_info=CloneImageInfo(image_info)) == NULL) goto NoPalette;
 
  i=(long) strlen(clone_info->filename);
  j=i;
  while(--i>0)
    {
      if(clone_info->filename[i]=='.') 
        {
          break;
        }
      if(clone_info->filename[i]=='/' || clone_info->filename[i]=='\\' ||
         clone_info->filename[i]==':' ) 
        {
          i=j;
          break;
        }
    }
  
  (void) strcpy(clone_info->filename+i,".PAL");
  if((clone_info->file=fopen(clone_info->filename,"rb"))==NULL)
    {
      (void) strcpy(clone_info->filename+i,".pal");
      if((clone_info->file=fopen(clone_info->filename,"rb"))==NULL)
        {
          clone_info->filename[i]=0;
          if((clone_info->file=fopen(clone_info->filename,"rb"))==NULL) 
            {
              DestroyImageInfo(clone_info);
              clone_info=NULL;
              goto NoPalette;
            }
        }
    }

  if( (palette=AllocateImage(clone_info))==NULL ) goto NoPalette;
  status=OpenBlob(clone_info,palette,ReadBinaryBlobMode,exception);
  if (status == False)
    {
    ErasePalette:     
      DestroyImage(palette);
      palette=NULL;
      goto NoPalette;
    }
 
     
  if(palette!=NULL)
    {
      ldblk=ReadBlobByte(palette);		/*size of palette*/
      if(ldblk==EOF) goto ErasePalette;
      image->colors=ldblk+1;    
      if (!AllocateImageColormap(image, image->colors)) goto NoMemory;
    
      for(i=0;i<=ldblk;i++)
      {     
        j = ReadBlobByte(palette);	/* Flag */
        if(j==EOF) break;		/* unexpected end of file */
        if(j<=ldblk)
        {
	  if(j==MEZ[i])
	    j = i; /* MEZ[i];	ignore MEZ!!! proper palete element after reindexing */	
          else
            j = MEZ[i];			/* ?? I do not know, big mess ?? */
	  if(j>=(long) image->colors) j=image->colors-1;
          image->colormap[j].red = ScaleCharToQuantum(ReadBlobByte(palette));
          image->colormap[j].green = ScaleCharToQuantum(ReadBlobByte(palette));
          image->colormap[j].blue = ScaleCharToQuantum(ReadBlobByte(palette));
        }
        else
        {
          (void) SeekBlob(palette, 3, SEEK_CUR);
          (void) fprintf(stderr,"TopoL: Wrong index inside palette %d!",(int)j);
        } 
      }
    }

NoPalette:
  if(palette == NULL && image->colors != 0)
  {
    if(Header.FileType<5)
    {
      if (!AllocateImageColormap(image, image->colors))
      {
        NoMemory:
          ThrowReaderException(ResourceLimitError, MemoryAllocationFailed, image);
      }

      for(i = 0; i < (long) image->colors; i++)
      {
	j = MEZ[i];
        image->colormap[i].red = ScaleCharToQuantum(j);
        image->colormap[i].green = ScaleCharToQuantum(j);
        image->colormap[i].blue = ScaleCharToQuantum(j);
      }
    }
  }

  /* ----- Load TopoL raster ----- */    
  switch(Header.Version)
  {
   case 0:
   case 1:
     ldblk = (long) ((depth * image->columns + 7) / 8);
     BImgBuff = MagickAllocateMemory(unsigned char *,(size_t) ldblk);	/*Ldblk was set in the check phase */
     if (BImgBuff == NULL)
        ThrowReaderException(ResourceLimitError, MemoryAllocationFailed, image);
     (void) SeekBlob(image, 512 /*sizeof(Header)*/, SEEK_SET);
     for (i = 0; i < (int) Header.Rows; i++)
     {
       (void)ReadBlob(image, ldblk, (char *)BImgBuff);       
       InsertRow(depth, BImgBuff, i, image, 0, image->columns, &import_options);
     }
     break;
  case 2:
    {
      magick_uint32_t *Offsets;
      long SkipBlk;
      unsigned TilX, TilY;
      unsigned TilesAcross = (Header.Cols+Header.TileWidth-1) / Header.TileWidth;
      unsigned TilesDown   = (Header.Rows+Header.TileHeight-1) / Header.TileHeight;

      if(Header.TileCompression!=0)
		{
  	        ThrowReaderException(CorruptImageError, UnrecognizedImageCompression, image);
		break;
		}

       ldblk = (long)((depth * Header.TileWidth + 7) / 8);
       BImgBuff = MagickAllocateMemory(unsigned char *,(size_t) ldblk);	/*Ldblk was set in the check phase */

       /* dlazdice.create(Header.TileWidth,Header.TileHeight,p.Planes); */
       Offsets = MagickAllocateMemory(magick_uint32_t *,(size_t)TilesAcross*TilesDown*sizeof(magick_uint32_t));
       if(Offsets==NULL)
         ThrowReaderException(ResourceLimitError, MemoryAllocationFailed, image);         

       (void)SeekBlob(image, Header.TileOffsets, SEEK_SET);
       ReadBlobDwordLSB(image, TilesAcross*TilesDown*4, (magick_uint32_t *)Offsets);

       for(TilY=0;TilY<Header.Rows;TilY+=Header.TileHeight)
	 for(TilX=0;TilX<TilesAcross;TilX++)
	   {
	   ldblk = image->columns - TilX*Header.TileWidth;

	   if(ldblk>Header.TileWidth) ldblk = Header.TileWidth;
	   SkipBlk = ((long)depth * (Header.TileWidth-ldblk)+7) / 8;
	   ldblk = ((long)depth * ldblk+7) / 8;

           (void)SeekBlob(image, Offsets[(TilY/Header.TileHeight)*TilesAcross+TilX], SEEK_SET);	   
	   j = TilX * (ldblk+SkipBlk);
	   for(i=0;i<Header.TileHeight;i++)
	   {
             (void)ReadBlob(image, ldblk, (char *)BImgBuff);
             if(SkipBlk>0)
               SeekBlob(image, SkipBlk, SEEK_CUR);
	     InsertRow(depth, BImgBuff, i+TilY, image, TilX, 
                    (image->columns<Header.TileWidth)?image->columns:Header.TileWidth, &import_options);
          }          
	}

       if(Offsets) {MagickFreeMemory(Offsets);Offsets=NULL;}
       break;
      }
    }


  /* Finish: */
DONE_READING:
  if (BImgBuff != NULL)
    MagickFreeMemory(BImgBuff);
  if (palette != NULL)
    DestroyImage(palette);
  if (clone_info != NULL)
    DestroyImageInfo(clone_info);
  /* if (EOFBlob(image))
     ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image); */
  CloseBlob(image);

  if (logging) (void)LogMagickEvent(CoderEvent,GetMagickModule(),"return");
  return (image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r T O P O L I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterTOPOLImage adds attributes for the TOPOL image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterTOPOLImage method is:
%
%      RegisterTOPOLImage(void)
%
*/
ModuleExport void RegisterTOPOLImage(void)
{
  MagickInfo * entry;

  entry = SetMagickInfo("TOPOL");
  entry->decoder = (DecoderHandler) ReadTOPOLImage;
  entry->seekable_stream = True;
  entry->description = "TOPOL X Image";
  entry->module = "TOPOL";
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r T O P O L I m a g e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterTOPOLImage removes format registrations made by the
%  TOPOL module from the list of supported formats.
%
%  The format of the UnregisterTOPOLImage method is:
%
%      UnregisterTOPOLImage(void)
%
*/
ModuleExport void UnregisterTOPOLImage(void)
{
  (void) UnregisterMagickInfo("TOPOL");
}
