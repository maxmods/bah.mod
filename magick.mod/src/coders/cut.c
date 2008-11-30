/*
% Copyright (C) 2003 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                              CCC  U   U  TTTTT                              %
%                             C     U   U    T                                %
%                             C     U   U    T                                %
%                             C     U   U    T                                %
%                              CCC   UUU     T                                %
%                                                                             %
%                                                                             %
%                         Read DR Halo Image Format.                          %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                              Jaroslav Fojtik                                %
%                              June 2000 - 2007                               %
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
#include "magick/pixel_cache.h"
#include "magick/color.h"
#include "magick/blob.h"
#include "magick/magick.h"
#include "magick/utility.h"

typedef struct
{
  unsigned Width;
  unsigned Height;
  unsigned Reserved;
}CUTHeader;      /*Dr Halo*/
typedef struct
{
  char FileId[2];
  unsigned Version;
  unsigned Size;
  char FileType;
  char SubType;
  unsigned BoardID;
  unsigned GraphicsMode;
  unsigned MaxIndex;
  unsigned MaxRed;
  unsigned MaxGreen;
  unsigned MaxBlue;
  char PaletteId[20];
}CUTPalHeader;


static void InsertRow(unsigned char *p,long y,Image *image)
{
  int bit; long x;
  register PixelPacket *q;
  IndexPacket index;
  register IndexPacket *indexes;


  switch (image->depth)
    {
    case 1:  /* Convert bitmap scanline. */
      {
        q=SetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=AccessMutableIndexes(image);
        for (x=0; x < ((long) image->columns-7); x+=8)
          {
            for (bit=0; bit < 8; bit++)
              {
                index=((*p) & (0x80U >> bit) ? 0x01U : 0x00U);
                indexes[x+bit]=index;
                *q++=image->colormap[index];
              }
            p++;
          }
        if ((image->columns % 8) != 0)
          {
            for (bit=0; bit < (long) (image->columns % 8); bit++)
              {
                index=((*p) & (0x80 >> bit) ? 0x01U : 0x00U);
                indexes[x+bit]=index;
                *q++=image->colormap[index];
              }
            p++;
          }
        if (!SyncImagePixels(image))
          break;
        /*            if (image->previous == (Image *) NULL)
                      if (QuantumTick(y,image->rows))
                      ProgressMonitor(LoadImageText,image->rows-y-1,image->rows);*/
        break;
      }
    case 2:  /* Convert PseudoColor scanline. */
      {
        q=SetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=AccessMutableIndexes(image);
        for (x=0; x < ((long) image->columns-1); x+=2)
          {
            index=(IndexPacket) ((*p >> 6U) & 0x3U);
            VerifyColormapIndex(image,index);
            indexes[x]=index;
            *q++=image->colormap[index];
            index=(IndexPacket) ((*p >> 4U) & 0x3U);
            VerifyColormapIndex(image,index);
            indexes[x]=index;
            *q++=image->colormap[index];
            index=(IndexPacket) ((*p >> 2U) & 0x3U);
            VerifyColormapIndex(image,index);
            indexes[x]=index;
            *q++=image->colormap[index];
            index=(IndexPacket) ((*p) & 0x3U);
            VerifyColormapIndex(image,index);
            indexes[x+1]=index;
            *q++=image->colormap[index];
            p++;
          }
        if ((image->columns % 4) != 0)
          {
            index=(IndexPacket) ((*p >> 6U) & 0x3U);
            VerifyColormapIndex(image,index);
            indexes[x]=index;
            *q++=image->colormap[index];
            if ((image->columns % 4) >= 1)

              {
                index=(IndexPacket) ((*p >> 4U) & 0x3U);
                VerifyColormapIndex(image,index);
                indexes[x]=index;
                *q++=image->colormap[index];
                if ((image->columns % 4) >= 2U)

                  {
                    index=(IndexPacket) ((*p >> 2U) & 0x3U);
                    VerifyColormapIndex(image,index);
                    indexes[x]=index;
                    *q++=image->colormap[index];
                  }
              }
            p++;
          }
        if (!SyncImagePixels(image))
          break;
        /*         if (image->previous == (Image *) NULL)
                   if (QuantumTick(y,image->rows))
                   ProgressMonitor(LoadImageText,image->rows-y-1,image->rows);*/
        break;
      }
      
    case 4:  /* Convert PseudoColor scanline. */
      {
        q=SetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=AccessMutableIndexes(image);
        for (x=0; x < ((long) image->columns-1); x+=2)
          {
            index=(IndexPacket) ((*p >> 4U) & 0xfU);
            VerifyColormapIndex(image,index);
            indexes[x]=index;
            *q++=image->colormap[index];
            index=(IndexPacket) ((*p) & 0xfU);
            VerifyColormapIndex(image,index);
            indexes[x+1]=index;
            *q++=image->colormap[index];
            p++;
          }
        if ((image->columns % 2) != 0)
          {
            index=(IndexPacket) ((*p >> 4U) & 0xfU);
            VerifyColormapIndex(image,index);
            indexes[x]=index;
            *q++=image->colormap[index];
            p++;
          }
        if (!SyncImagePixels(image))
          break;
        /*         if (image->previous == (Image *) NULL)
                   if (QuantumTick(y,image->rows))
                   ProgressMonitor(LoadImageText,image->rows-y-1,image->rows);*/
        break;
      }
    case 8: /* Convert PseudoColor scanline. */
      {
        q=SetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL) break;
        indexes=AccessMutableIndexes(image);

        for (x=0; x < (long) image->columns; x++)
          {
            index=(IndexPacket) (*p);
            VerifyColormapIndex(image,index);
            indexes[x]=index;
            *q++=image->colormap[index];
            p++;
          }
        if (!SyncImagePixels(image))
          break;
        /*           if (image->previous == (Image *) NULL)
                     if (QuantumTick(y,image->rows))
                     ProgressMonitor(LoadImageText,image->rows-y-1,image->rows);*/
      }
      break;

    }
}

static unsigned int GetCutColors(Image *image)
{
  int
    x,
    y;

  PixelPacket
    *q;

  Quantum
    MaxColor,
    ScaleCharToQuantum16;

  /* Compute the number of colors in Grayed R[i]=G[i]=B[i] image */
  ScaleCharToQuantum16=ScaleCharToQuantum(16);
  MaxColor=0;
  for (y=0; y < (long)image->rows; y++)
    {
      q=SetImagePixels(image,0,y,image->columns,1);
      for (x=(long)image->columns; x > 0; x--)
        {
          if (MaxColor < q->red)
            MaxColor=q->red;
          if (MaxColor >= ScaleCharToQuantum16)
            return(255);
          q++;
        }
    }
  if (MaxColor < ScaleCharToQuantum(2))
    MaxColor=2;
  else if (MaxColor < ScaleCharToQuantum(16))
    MaxColor=16;
  return (MaxColor);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d C U T I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadCUTImage reads an CUT X image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadCUTImage method is:
%
%      Image *ReadCUTImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadCUTImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadCUTImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image *image,*palette;
  ImageInfo *clone_info;
  unsigned int status;
  unsigned long EncodedByte;
  unsigned char RunCount,RunValue,RunCountMasked;
  CUTHeader  Header;
  CUTPalHeader PalHeader;
  long i,j;
  long ldblk;
  unsigned char *BImgBuff=NULL,*ptrB;
  PixelPacket *q;

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
    Read CUT image.
  */
  palette=NULL;
  clone_info=NULL;
  Header.Width=ReadBlobLSBShort(image);
  Header.Height=ReadBlobLSBShort(image);
  Header.Reserved=ReadBlobLSBShort(image);

  if (Header.Width==0 || Header.Height==0 || Header.Reserved!=0)
    CUT_KO:  ThrowReaderException(CorruptImageError,ImproperImageHeader,image);

  /*---This code checks first line of image---*/
  EncodedByte=ReadBlobLSBShort(image);
  RunCount=ReadBlobByte(image);
  RunCountMasked=RunCount & 0x7F;
  ldblk=0;
  while(RunCountMasked!=0)  /*end of line?*/
    {
      i=1;
      if(RunCount<0x80) i=RunCountMasked;
      (void) SeekBlob(image,TellBlob(image)+i,SEEK_SET);
      if(EOFBlob(image)) goto CUT_KO;  /*wrong data*/
      EncodedByte-=i+1;
      ldblk+=RunCountMasked;

      RunCount=ReadBlobByte(image);
      if(EOFBlob(image))  goto CUT_KO;  /*wrong data: unexpected eof in line*/
      RunCountMasked=RunCount & 0x7F;
    }
  if(EncodedByte!=1) goto CUT_KO;  /*wrong data: size incorrect*/
  i=0;        /*guess a number of bit planes*/
  if(ldblk==(int) Header.Width)   i=8;
  if(2*ldblk==(int) Header.Width) i=4;
  if(8*ldblk==(int) Header.Width) i=1;
  if(i==0) goto CUT_KO;    /*wrong data: incorrect bit planes*/

  image->columns=Header.Width;
  image->rows=Header.Height;
  image->depth=i;
  image->colors=1l >> i;

  /* If ping is true, then only set image size and colors without reading any image data. */
  if (image_info->ping) goto Finish;
 
  /* ----- Do something with palette ----- */
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
      (void) ReadBlob(palette,2,PalHeader.FileId);
      if(strncmp(PalHeader.FileId,"AH",2)) goto ErasePalette;
      PalHeader.Version=ReadBlobLSBShort(palette);
      PalHeader.Size=ReadBlobLSBShort(palette);
      PalHeader.FileType=ReadBlobByte(palette);
      PalHeader.SubType=ReadBlobByte(palette);
      PalHeader.BoardID=ReadBlobLSBShort(palette);
      PalHeader.GraphicsMode=ReadBlobLSBShort(palette);
      PalHeader.MaxIndex=ReadBlobLSBShort(palette);
      PalHeader.MaxRed=ReadBlobLSBShort(palette);
      PalHeader.MaxGreen=ReadBlobLSBShort(palette);
      PalHeader.MaxBlue=ReadBlobLSBShort(palette);
      (void) ReadBlob(palette,20,PalHeader.PaletteId);
   
      if(PalHeader.MaxIndex<1) goto ErasePalette;
      image->colors=PalHeader.MaxIndex+1;
      if (!AllocateImageColormap(image,image->colors)) goto NoMemory;
   
      if(PalHeader.MaxRed==0) PalHeader.MaxRed=MaxRGB;  /*avoid division by 0*/
      if(PalHeader.MaxGreen==0) PalHeader.MaxGreen=MaxRGB;
      if(PalHeader.MaxBlue==0) PalHeader.MaxBlue=MaxRGB;
   
      for(i=0;i<=(int) PalHeader.MaxIndex;i++)
        {      /*this may be wrong- I don't know why is palette such strange*/
          j=(long) TellBlob(palette);
          if((j % 512)>512-6)
            {
              j=((j / 512)+1)*512;
              (void) SeekBlob(palette,j,SEEK_SET);
            }
          image->colormap[i].red=ReadBlobLSBShort(palette);
          if(MaxRGB!=PalHeader.MaxRed) 
            {
              image->colormap[i].red=(Quantum)
                (((double)image->colormap[i].red*MaxRGB+(PalHeader.MaxRed>>1))/PalHeader.MaxRed+0.5);
            }
          image->colormap[i].green=ReadBlobLSBShort(palette);
          if(MaxRGB!=PalHeader.MaxGreen) 
            {
              image->colormap[i].green=(Quantum)
                (((double)image->colormap[i].green*MaxRGB+(PalHeader.MaxGreen>>1))/PalHeader.MaxGreen+0.5);
            }
          image->colormap[i].blue=ReadBlobLSBShort(palette);       
          if(MaxRGB!=PalHeader.MaxBlue)  
            {
              image->colormap[i].blue=(Quantum)
                (((double)image->colormap[i].blue*MaxRGB+(PalHeader.MaxBlue>>1))/PalHeader.MaxBlue+0.5);
            }
         
        }
    }

   

 NoPalette:
  if(palette==NULL)
    { 
   
      image->colors=256;
      if (!AllocateImageColormap(image,image->colors))
        {
        NoMemory:
          ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image)
            }     
   
      for (i=0; i < (long)image->colors; i++)
        {
          image->colormap[i].red=ScaleCharToQuantum(i);
          image->colormap[i].green=ScaleCharToQuantum(i);
          image->colormap[i].blue=ScaleCharToQuantum(i);
        }
    }

           
  /* ----- Load RLE compressed raster ----- */
  BImgBuff=MagickAllocateMemory(unsigned char *,(size_t) (ldblk));  /*Ldblk was set in the check phase*/
  if(BImgBuff==NULL) goto NoMemory;

  (void) SeekBlob(image,6 /*sizeof(Header)*/,SEEK_SET);
  for(i=0;i<(int) Header.Height;i++)
    {
      EncodedByte=ReadBlobLSBShort(image);

      ptrB=BImgBuff;
      j=ldblk;

      RunCount=ReadBlobByte(image);
      RunCountMasked=RunCount & 0x7F;

      while(RunCountMasked!=0)    
        {
          if(RunCountMasked>j)
            {    /*Wrong Data*/
              RunCountMasked=(unsigned char) j;
              if(j==0) 
                {
                  break;
                }
            }

          if(RunCount>0x80)
            {
              RunValue=ReadBlobByte(image);
              (void) memset(ptrB,RunValue,RunCountMasked);
            }
          else {
            (void) ReadBlob(image,RunCountMasked,ptrB);
          }
         
          ptrB+=RunCountMasked;
          j-=RunCountMasked;     
    
          if(EOFBlob(image)) goto Finish;  /* wrong data: unexpected eof in line */
          RunCount=ReadBlobByte(image);
          RunCountMasked=RunCount & 0x7F;
        }

      InsertRow(BImgBuff,i,image);
    }


  /*detect monochrome image*/

  if(palette==NULL)
    {    /*attempt to detect binary (black&white) images*/
      if((image->storage_class == PseudoClass) && IsGrayImage(image,&image->exception))
        {
          if(GetCutColors(image)==2)
            {
              for (i=0; i < (long)image->colors; i++)
                {
                  register Quantum
                    sample;
                  sample=ScaleCharToQuantum(i);
                  if(image->colormap[i].red!=sample) goto Finish;
                  if(image->colormap[i].green!=sample) goto Finish;
                  if(image->colormap[i].blue!=sample) goto Finish;
                }
     
              image->colormap[1].red=image->colormap[1].green=image->colormap[1].blue=MaxRGB;
              for (i=0; i < (long)image->rows; i++)  
                {
                  q=SetImagePixels(image,0,i,image->columns,1);  
                  for (j=0; j < (long)image->columns; j++)  
                    {       
                      if(q->red==ScaleCharToQuantum(1))
                        {
                          q->red=q->green=q->blue=MaxRGB;
                        }
                      q++;  
                    }  
                  if (!SyncImagePixels(image)) goto Finish;
                }
            }
        }        
    } 

 Finish:
  if(BImgBuff!=NULL) MagickFreeMemory(BImgBuff);
  if(palette!=NULL) DestroyImage(palette);
  if(clone_info!=NULL) DestroyImageInfo(clone_info);
  if (EOFBlob(image))
    ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
  CloseBlob(image);
  return(image);       
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r C U T I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterCUTImage adds attributes for the CUT image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterCUTImage method is:
%
%      RegisterCUTImage(void)
%
*/
ModuleExport void RegisterCUTImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("CUT");
  entry->decoder=(DecoderHandler) ReadCUTImage;
  entry->seekable_stream=True;
  entry->description="DR Halo";
  entry->module="CUT";
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r C U T I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterCUTImage removes format registrations made by the
%  CUT module from the list of supported formats.
%
%  The format of the UnregisterCUTImage method is:
%
%      UnregisterCUTImage(void)
%
*/
ModuleExport void UnregisterCUTImage(void)
{
  (void) UnregisterMagickInfo("CUT");
}

