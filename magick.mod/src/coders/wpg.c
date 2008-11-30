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
%                            W   W  PPPP    GGGG                              %
%                            W   W  P   P  G                                  %
%                            W W W  PPPP   G GGG                              %
%                            WW WW  P      G   G                              %
%                            W   W  P       GGG                               %
%                                                                             %
%                                                                             %
%                       Read WordPerfect Image Format.                        %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                              Jaroslav Fojtik                                %
%                              June 2000 - 2007                               %
%                         Rework for GraphicsMagick                           %
%                              Bob Friesenhahn                                %
%                               Feb-May 2003                                  %
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
#include "magick/color.h"
#include "magick/constitute.h"
#include "magick/magic.h"
#include "magick/magick.h"
#include "magick/tempfile.h"
#include "magick/transform.h"
#include "magick/shear.h"
#include "magick/utility.h"


typedef struct
   {
   unsigned char Red;
   unsigned char Blue;
   unsigned char Green;
   } RGB_Record;

/* Default palette for WPG level 1 */
const RGB_Record WPG1_Palette[256]={
{  0,  0,  0},		{  0,  0,168},
{  0,168,  0},		{  0,168,168},
{168,  0,  0},		{168,  0,168},
{168, 84,  0},		{168,168,168},
{ 84, 84, 84},		{ 84, 84,252},
{ 84,252, 84},		{ 84,252,252},
{252, 84, 84},		{252, 84,252},
{252,252, 84},		{252,252,252},  /*16*/
{  0,  0,  0},		{ 20, 20, 20},
{ 32, 32, 32},		{ 44, 44, 44},
{ 56, 56, 56},		{ 68, 68, 68},
{ 80, 80, 80},		{ 96, 96, 96},
{112,112,112},		{128,128,128},
{144,144,144},		{160,160,160},
{180,180,180},		{200,200,200},
{224,224,224},		{252,252,252},  /*32*/
{  0,  0,252},		{ 64,  0,252},
{124,  0,252},		{188,  0,252},
{252,  0,252},		{252,  0,188},
{252,  0,124},		{252,  0, 64},
{252,  0,  0},		{252, 64,  0},
{252,124,  0},		{252,188,  0},
{252,252,  0},		{188,252,  0},
{124,252,  0},		{ 64,252,  0},	/*48*/
{  0,252,  0},		{  0,252, 64},
{  0,252,124},		{  0,252,188},
{  0,252,252},		{  0,188,252},
{  0,124,252},		{  0, 64,252},
{124,124,252},		{156,124,252},
{188,124,252},		{220,124,252},
{252,124,252},		{252,124,220},
{252,124,188},		{252,124,156},	/*64*/
{252,124,124},		{252,156,124},
{252,188,124},		{252,220,124},
{252,252,124},		{220,252,124},
{188,252,124},		{156,252,124},
{124,252,124},		{124,252,156},
{124,252,188},		{124,252,220},
{124,252,252},		{124,220,252},
{124,188,252},		{124,156,252},	/*80*/
{180,180,252},		{196,180,252},
{216,180,252},		{232,180,252},
{252,180,252},		{252,180,232},
{252,180,216},		{252,180,196},
{252,180,180},		{252,196,180},
{252,216,180},		{252,232,180},
{252,252,180},		{232,252,180},
{216,252,180},		{196,252,180},	/*96*/
{180,220,180},		{180,252,196},
{180,252,216},		{180,252,232},
{180,252,252},		{180,232,252},
{180,216,252},		{180,196,252},
{0,0,112},		{28,0,112},
{56,0,112},		{84,0,112},
{112,0,112},		{112,0,84},
{112,0,56},		{112,0,28},	/*112*/
{112,0,0},		{112,28,0},
{112,56,0},		{112,84,0},
{112,112,0},		{84,112,0},
{56,112,0},		{28,112,0},
{0,112,0},		{0,112,28},
{0,112,56},		{0,112,84},
{0,112,112},		{0,84,112},
{0,56,112},		{0,28,112}, 	/*128*/
{56,56,112},		{68,56,112},
{84,56,112},		{96,56,112},
{112,56,112},		{112,56,96},
{112,56,84},		{112,56,68},
{112,56,56},		{112,68,56},
{112,84,56},		{112,96,56},
{112,112,56},		{96,112,56},
{84,112,56},		{68,112,56},	/*144*/
{56,112,56},		{56,112,69},
{56,112,84},		{56,112,96},
{56,112,112},		{56,96,112},
{56,84,112},		{56,68,112},
{80,80,112},		{88,80,112},
{96,80,112},		{104,80,112},
{112,80,112},		{112,80,104},
{112,80,96},		{112,80,88},	/*160*/
{112,80,80},		{112,88,80},
{112,96,80},		{112,104,80},
{112,112,80},		{104,112,80},
{96,112,80},		{88,112,80},
{80,112,80},		{80,112,88},
{80,112,96},		{80,112,104},
{80,112,112},		{80,114,112},
{80,96,112},		{80,88,112},	/*176*/
{0,0,64},		{16,0,64},
{32,0,64},		{48,0,64},
{64,0,64},		{64,0,48},
{64,0,32},		{64,0,16},
{64,0,0},		{64,16,0},
{64,32,0},		{64,48,0},
{64,64,0},		{48,64,0},
{32,64,0},		{16,64,0},	/*192*/
{0,64,0},		{0,64,16},
{0,64,32},		{0,64,48},
{0,64,64},		{0,48,64},
{0,32,64},		{0,16,64},
{32,32,64},		{40,32,64},
{48,32,64},		{56,32,64},
{64,32,64},		{64,32,56},
{64,32,48},		{64,32,40},	/*208*/
{64,32,32},		{64,40,32},
{64,48,32},		{64,56,32},
{64,64,32},		{56,64,32},
{48,64,32},		{40,64,32},
{32,64,32},		{32,64,40},
{32,64,48},		{32,64,56},
{32,64,64},		{32,56,64},
{32,48,64},		{32,40,64},	/*224*/
{44,44,64},		{48,44,64},
{52,44,64},		{60,44,64},
{64,44,64},		{64,44,60},
{64,44,52},		{64,44,48},
{64,44,44},		{64,48,44},
{64,52,44},		{64,60,44},
{64,64,44},		{60,64,44},
{52,64,44},		{48,64,44},	/*240*/
{44,64,44},		{44,64,48},
{44,64,52},		{44,64,60},
{44,64,64},		{44,60,64},
{44,55,64},		{44,48,64},
{0,0,0},		{0,0,0},
{0,0,0},		{0,0,0},
{0,0,0},		{0,0,0},
{0,0,0},		{0,0,0}		/*256*/
};



/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s W P G                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IsWPG() returns True if the image format type, identified by the magick
%  string, is WPG.
%
%  The format of the IsWPG method is:
%
%      unsigned int IsWPG(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsWPG returns True if the image format type is WPG.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsWPG(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(False);
  if (memcmp(magick,"\377WPC",4) == 0)
    return(True);
  return(False);
}


static void Rd_WP_DWORD(Image *image,unsigned long *d)
{
  unsigned char
    b;

  b=ReadBlobByte(image);
  *d=b;
  if (b < 0xFFU)
    return;
  b=ReadBlobByte(image);
  *d=(unsigned long) b;
  b=ReadBlobByte(image);
  *d+=(unsigned long) b*256l;
  if (*d < 0x8000)
    return;
  *d=(*d & 0x7FFF) << 16;
  b=ReadBlobByte(image);
  *d+=(unsigned long) b;
  b=ReadBlobByte(image);
  *d+=(unsigned long) b*256l;
  return;
}


static MagickPassFail InsertRow(unsigned char *p,long y, Image *image, int bpp)
{
  long
    x;
  register PixelPacket
    *q;
  MagickPassFail RetVal = MagickFail;
  IndexPacket index;
  IndexPacket *indexes;


  q = SetImagePixels(image,0,y,image->columns,1);
  if(q == (PixelPacket *) NULL) return MagickFail;

  switch (bpp)
    {
    case 1:  /* Convert bitmap scanline. WP seems to ignore palette even if it is present. */
        RetVal = ImportImagePixelArea(image,GrayQuantum,bpp,p,NULL,0);
        break; 

    case 4:  /* Convert PseudoColor scanline. */
    case 8:  /* Convert PseudoColor scanline. */
        RetVal = ImportImagePixelArea(image,IndexQuantum,bpp,p,NULL,0);
        break;

    case 2:  /* Convert PseudoColor scanline. */
      {
        indexes=AccessMutableIndexes(image);
        for (x=0; x < ((long) image->columns-1); x+=2)
          {
            index=(IndexPacket) ((*p >> 6) & 0x3);
            VerifyColormapIndex(image,index);
            indexes[x]=index;
            *q++=image->colormap[index];
            index=(IndexPacket) ((*p >> 4) & 0x3);
            VerifyColormapIndex(image,index);
            indexes[x]=index;
            *q++=image->colormap[index];
            index=(IndexPacket) ((*p >> 2) & 0x3);
            VerifyColormapIndex(image,index);
            indexes[x]=index;
            *q++=image->colormap[index];
            index=(IndexPacket) ((*p) & 0x3);
            VerifyColormapIndex(image,index);
            indexes[x+1]=index;
            *q++=image->colormap[index];
            p++;
          }
        if ((image->columns % 4) != 0)
          {
            index=(IndexPacket) ((*p >> 6) & 0x3);
            VerifyColormapIndex(image,index);
            indexes[x]=index;
            *q++=image->colormap[index];
            if ((image->columns % 4) >= 1)

              {
                index=(IndexPacket) ((*p >> 4) & 0x3);
                VerifyColormapIndex(image,index);
                indexes[x]=index;
                *q++=image->colormap[index];
                if ((image->columns % 4) >= 2)

                  {
                    index=(IndexPacket) ((*p >> 2) & 0x3);
                    VerifyColormapIndex(image,index);
                    indexes[x]=index;
                    *q++=image->colormap[index];
                  }
              }
            p++;
          }       
        RetVal = MagickPass;
        break;
      }
          
    case 24:     /*  Convert DirectColor scanline.  */      
      RetVal = ImportImagePixelArea(image,RGBQuantum,8,p,NULL,0);
      break;

    default:
      return MagickFail;  /* emit some error here */
    }


  if(RetVal==MagickFail)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"ImportImagePixelArea failed for row: %ld, bpp: %d", y, bpp); 

  if (!SyncImagePixels(image))
  {
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"SyncImagePixels failed for row: %ld, bpp: %d", y, bpp); 
    RetVal = MagickFail;
  }
          
return RetVal;
}


/* Helper for WPG1 raster reader. */
#define InsertByte(b) \
{ \
  BImgBuff[x]=b; \
  x++; \
  if((long) x>=ldblk) \
  { \
    (void)InsertRow(BImgBuff,(long) y,image,bpp); \
    x=0; \
    y++; \
    } \
}

/* WPG1 raster reader. */
static int UnpackWPGRaster(Image *image,int bpp)
{
  int
    x,
    y,
    i;

  unsigned char
    bbuf,
    *BImgBuff,
    RunCount;

  long
    ldblk;

  x=0;
  y=0;

  ldblk=(long) ((bpp*image->columns+7)/8);
  BImgBuff=MagickAllocateMemory(unsigned char *,(size_t) ldblk);
  if(BImgBuff==NULL) return(-2);

  while(y<(long) image->rows)
    {
      bbuf=ReadBlobByte(image);

      RunCount=bbuf & 0x7F;
      if(bbuf & 0x80)
        {
          if(RunCount)  /* repeat next byte runcount * */
            {
              bbuf=ReadBlobByte(image);
              for(i=0;i<(int) RunCount;i++) InsertByte(bbuf);
            }
          else {  /* read next byte as RunCount; repeat 0xFF runcount* */
            RunCount=ReadBlobByte(image);
            for(i=0;i<(int) RunCount;i++) InsertByte(0xFF);
          }
        }
      else {
        if(RunCount)   /* next runcount byte are readed directly */
          {
            for(i=0;i < (int) RunCount;i++)
              {
                bbuf=ReadBlobByte(image);
                InsertByte(bbuf);
              }
          }
        else {  /* repeat previous line runcount* */
          RunCount=ReadBlobByte(image);
          if(x) {    /* attempt to duplicate row from x position: */
            /* I do not know what to do here */
            MagickFreeMemory(BImgBuff);
            return(-3);
          }
          for(i=0;i < (int) RunCount;i++)
            {
              x=0;
              y++;    /* Here I need to duplicate previous row RUNCOUNT* */
              if(y<2) continue;
              if(y>(long) image->rows)
                {
                  MagickFreeMemory(BImgBuff);
                  return(-4);
                }
              (void) InsertRow(BImgBuff,y-1,image,bpp);
            }
        }
      }
    }
  MagickFreeMemory(BImgBuff);
  return(0);
}


/* Helper for WPG2 reader. */
#define InsertByte6(b) \
{ \
  if(XorMe)\
    BImgBuff[x] = b ^ UpImgBuff[x];\
  else\
    BImgBuff[x] = b;\
  x++; \
  if((long) x >= ldblk) \
  { \
    (void)InsertRow(BImgBuff,(long) y,image,bpp); \
    x=0; \
    y++; \
    XorMe = 0; \
    tmpImgBuff = BImgBuff; \
    BImgBuff = UpImgBuff; \
    UpImgBuff = tmpImgBuff; \
   } \
}

/* WPG2 raster reader. */
static int UnpackWPG2Raster(Image *image,int bpp)
{
  unsigned int
    SampleSize=1;

  unsigned char
    bbuf,
    *BImgBuff,		/* Buffer for a current line. */
    *UpImgBuff,		/* Buffer for previous line. */
    *tmpImgBuff,
    RunCount,
    SampleBuffer[8];

  unsigned long
    x,
    y;

  unsigned int
    i;

  long
    ldblk;

  int XorMe = 0;

  x=0;
  y=0;
  ldblk=(long) ((bpp*image->columns+7)/8);
  BImgBuff=MagickAllocateMemory(unsigned char *,(size_t) ldblk);
  if(BImgBuff==NULL)
    return(-2);
  UpImgBuff=MagickAllocateMemory(unsigned char *,(size_t) ldblk);
  if(UpImgBuff==NULL)
  {
    MagickFreeMemory(BImgBuff);
    return(-2);
  }
  (void) memset(UpImgBuff,0,ldblk);

  while( y< image->rows)
    {
      bbuf=ReadBlobByte(image);

      switch(bbuf)
        {
        case 0x7D:
          SampleSize=ReadBlobByte(image);  /* DSZ */
          if(SampleSize>8)
            return(-2);
          if(SampleSize<1)
            return(-2);
          break;
        case 0x7E:
          if(y==0)			   /* XOR */
	    (void)fprintf(stderr,"\nWPG token XOR on the first line is not supported, please report!");
	  XorMe=!XorMe; /* or XorMe=1 ?? */
          break;
        case 0x7F:
          RunCount=ReadBlobByte(image);   /* BLK */
          for(i=0; i < SampleSize*(RunCount+1); i++)
            {
              InsertByte6(0);
            }
          break;
        case 0xFD:
	  RunCount=ReadBlobByte(image);   /* EXT */
	  for(i=0; i<= RunCount;i++)
            for(bbuf=0; bbuf < SampleSize; bbuf++)
              InsertByte6(SampleBuffer[bbuf]);          
          break;
        case 0xFE:
          RunCount=ReadBlobByte(image);  /* RST */
          if(x!=0)
            {
              (void) fprintf(stderr,
                             "\nUnsupported WPG2 unaligned token RST x=%lu, please report!\n"
                             ,x);
              return(-3);
            }
          {
            /* duplicate the previous row RunCount x */
            for(i=0;i<=RunCount;i++)
              {      
                (void) InsertRow(UpImgBuff,(long) (image->rows >= y ? y : image->rows-1),
                                 image,bpp);
                y++;
              }    
          }
          break;
        case 0xFF:
          RunCount=ReadBlobByte(image);	 /* WHT */
          for(i=0; i < SampleSize*(RunCount+1); i++)
            {
              InsertByte6(0xFF);
            }
          break;
        default:
          RunCount=bbuf & 0x7F;

          if(bbuf & 0x80)		 /* REP */
            {  
              for(i=0; i < SampleSize; i++)
                SampleBuffer[i]=ReadBlobByte(image);
              for(i=0;i<=RunCount;i++)
                for(bbuf=0;bbuf<SampleSize;bbuf++)
                  InsertByte6(SampleBuffer[bbuf]);
            }
          else {			/* NRP */
            for(i=0; i< SampleSize*(RunCount+1);i++)
              {
                bbuf=ReadBlobByte(image);
                InsertByte6(bbuf);
              }
          }
        }
    }
  MagickFreeMemory(BImgBuff);
  MagickFreeMemory(UpImgBuff);
  return(0);
}


typedef float tCTM[3][3];

static unsigned LoadWPG2Flags(Image *image,char Precision,float *Angle,tCTM *CTM)
{
const unsigned char TPR=1,TRN=2,SKW=4,SCL=8,ROT=0x10,OID=0x20,LCK=0x80;
long x;
unsigned DenX;
unsigned Flags;

 (void) memset(*CTM,0,sizeof(*CTM));     /*CTM.erase();CTM.resize(3,3);*/
 (*CTM)[0][0]=1;
 (*CTM)[1][1]=1;
 (*CTM)[2][2]=1;

 Flags=ReadBlobLSBShort(image);
 if(Flags & LCK) x=ReadBlobLSBLong(image);	/*Edit lock*/
 if(Flags & OID)
	{
	if(Precision==0)
	  {x=ReadBlobLSBShort(image);}	/*ObjectID*/
	else
	  {x=ReadBlobLSBLong(image);}	/*ObjectID (Double precision)*/
	}
 if(Flags & ROT)
	{
	x=ReadBlobLSBLong(image);	/*Rot Angle*/
	if(Angle) *Angle=x/65536.0;
	}
 if(Flags & (ROT|SCL))
	{
	x=ReadBlobLSBLong(image);	/*Sx*cos()*/
	(*CTM)[0][0] = (float)x/0x10000;
	x=ReadBlobLSBLong(image);	/*Sy*cos()*/
	(*CTM)[1][1] = (float)x/0x10000;
	}
 if(Flags & (ROT|SKW))
	{
	x=ReadBlobLSBLong(image);       /*Kx*sin()*/
	(*CTM)[1][0] = (float)x/0x10000;
	x=ReadBlobLSBLong(image);       /*Ky*sin()*/
	(*CTM)[0][1] = (float)x/0x10000;
	}
 if(Flags & TRN)
	{
	x=ReadBlobLSBLong(image); DenX=ReadBlobLSBShort(image);  /*Tx*/
        if(x>=0) (*CTM)[0][2] = (float)x+(float)DenX/0x10000;
            else (*CTM)[0][2] = (float)x-(float)DenX/0x10000;
	x=ReadBlobLSBLong(image); DenX=ReadBlobLSBShort(image);  /*Ty*/
	(*CTM)[1][2]=(float)x + ((x>=0)?1:-1)*(float)DenX/0x10000;
        if(x>=0) (*CTM)[1][2] = (float)x+(float)DenX/0x10000;
            else (*CTM)[1][2] = (float)x-(float)DenX/0x10000;
	}
 if(Flags & TPR)
	{
	x=ReadBlobLSBShort(image); DenX=ReadBlobLSBShort(image);  /*Px*/
	(*CTM)[2][0] = x + (float)DenX/0x10000;;
	x=ReadBlobLSBShort(image);  DenX=ReadBlobLSBShort(image); /*Py*/
	(*CTM)[2][1] = x + (float)DenX/0x10000;
	}
 return(Flags);
}


static Image *ExtractPostscript(Image *image,const ImageInfo *image_info,
                                ExtendedSignedIntegralType PS_Offset,long PS_Size,ExceptionInfo *exception)
{
  char
    postscript_file[MaxTextExtent];

  FILE
    *ps_file;

  ImageInfo
    *clone_info;

  Image
    *image2;
    
  unsigned char
    magick[2*MaxTextExtent];

  size_t
    magick_size;
    

  if ((clone_info=CloneImageInfo(image_info)) == NULL)
    return(image);
  clone_info->blob=(void *) NULL;
  clone_info->length=0;

  /* Obtain temporary file */
  ps_file=AcquireTemporaryFileStream(postscript_file,BinaryFileIOMode);
  if (!ps_file)
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Gannot create file stream for PS image");
      goto FINISH;
    }

  /* Copy postscript to temporary file */
  (void) SeekBlob(image,PS_Offset,SEEK_SET);
  magick_size=ReadBlob(image, sizeof(magick), magick);
  
  (void) SeekBlob(image,PS_Offset,SEEK_SET);
  while(PS_Size-- > 0)
    {
      (void) fputc(ReadBlobByte(image),ps_file);
    }
  (void) fclose(ps_file);
  
  /* Detect file format - Check magic.mgk configuration file. */
  if (GetMagickFileFormat(magick,magick_size,clone_info->magick,
                           MaxTextExtent,exception) == MagickFail)
    goto FINISH_UNL;
  
  /* Read nested image */
  /*FormatString(clone_info->filename,"%s:%.1024s",magic_info->name,postscript_file);*/
  FormatString(clone_info->filename,"%.1024s",postscript_file);
  image2=ReadImage(clone_info,exception);

  if (!image2)
    goto FINISH_UNL;

  /*
    Replace current image with new image while copying base image
    attributes.
  */
  (void) strlcpy(image2->filename,image->filename,MaxTextExtent);
  (void) strlcpy(image2->magick_filename,image->magick_filename,MaxTextExtent);
  (void) strlcpy(image2->magick,image->magick,MaxTextExtent);
  image2->depth=image->depth;
  DestroyBlob(image2);
  image2->blob=ReferenceBlob(image->blob);

  if ((image->rows == 0) || (image->columns == 0))
    DeleteImageFromList(&image);

  AppendImageToList(&image,image2);

 FINISH_UNL:    
  (void) LiberateTemporaryFile(postscript_file);
 FINISH:
  DestroyImageInfo(clone_info);
  return(image);
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d W P G I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadWPGImage reads an WPG X image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadWPGImage method is:
%
%    Image *ReadWPGImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadWPGImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadWPGImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  typedef struct
  {
    unsigned long FileId;
    ExtendedSignedIntegralType DataOffset;
    unsigned int ProductType;
    unsigned int FileType;
    unsigned char MajorVersion;
    unsigned char MinorVersion;
    unsigned int EncryptKey;
    unsigned int Reserved;
  } WPGHeader;

  typedef struct
  {
    unsigned char RecType;
    unsigned long RecordLength;
  } WPGRecord;

  typedef struct
  {
    unsigned char Class;
    unsigned char RecType;
    unsigned long Extension;
    unsigned long RecordLength;
  } WPG2Record;

  typedef struct
  {
    unsigned	HorizontalUnits;
    unsigned	VerticalUnits;
    unsigned char PosSizePrecision;
  } WPG2Start;

  typedef struct
  {
    unsigned int Width;
    unsigned int Heigth;
    unsigned int Depth;
    unsigned int HorzRes;
    unsigned int VertRes;
  } WPGBitmapType1;

  typedef struct
  {
    unsigned int Width;
    unsigned int Heigth;
    unsigned char Depth;
    unsigned char Compression;
  } WPG2BitmapType1;

  typedef struct
  {
    unsigned int RotAngle;
    unsigned int LowLeftX;
    unsigned int LowLeftY;
    unsigned int UpRightX;
    unsigned int UpRightY;
    unsigned int Width;
    unsigned int Heigth;
    unsigned int Depth;
    unsigned int HorzRes;
    unsigned int VertRes;
  } WPGBitmapType2;

  typedef struct
  {
    unsigned int StartIndex;
    unsigned int NumOfEntries;
  } WPGColorMapRec;

  typedef struct {
    unsigned long PS_unknown1;
    unsigned int PS_unknown2;
    unsigned int PS_unknown3;
  } WPGPSl1Record;  

  Image
    *image,
    *rotated_image;

  unsigned int
    status;

  WPGHeader
    Header;

  WPGRecord
    Rec;

  WPG2Record
    Rec2;

  WPG2Start StartWPG;

  WPGBitmapType1
    BitmapHeader1;

  WPG2BitmapType1
    Bitmap2Header1;

  WPGBitmapType2
    BitmapHeader2;

  WPGColorMapRec
    WPG_Palette;

  int
    i,
    bpp,
    WPG2Flags;

  int logging;

  long
    ldblk;

  unsigned char
    *BImgBuff;

  tCTM CTM;         /*current transform matrix*/

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);

  logging = LogMagickEvent(CoderEvent,GetMagickModule(),"enter"); 

  image=AllocateImage(image_info);
  image->depth=8;
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    ThrowReaderException(FileOpenError,UnableToOpenFile,image);

  /*
    Read WPG image.
  */
  Header.FileId=ReadBlobLSBLong(image);
  Header.DataOffset=(ExtendedSignedIntegralType) ReadBlobLSBLong(image);
  Header.ProductType=ReadBlobLSBShort(image);
  Header.FileType=ReadBlobLSBShort(image);
  Header.MajorVersion=ReadBlobByte(image);
  Header.MinorVersion=ReadBlobByte(image);
  Header.EncryptKey=ReadBlobLSBShort(image);
  Header.Reserved=ReadBlobLSBShort(image);

  if (Header.FileId!=0x435057FF || (Header.ProductType>>8)!=0x16)
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
  if (Header.EncryptKey!=0)
    ThrowReaderException(CoderError,EncryptedWPGImageFileNotSupported,image);

  image->colors = 0;
  bpp=0;

  if (logging) (void)LogMagickEvent(CoderEvent,GetMagickModule(),
          "File type: %d", Header.FileType);

  switch(Header.FileType)
    {
    case 1:     /* WPG level 1 */
      BitmapHeader2.RotAngle = 0;

      while(!EOFBlob(image)) /* object parser loop */
        {
          (void) SeekBlob(image,Header.DataOffset,SEEK_SET);
          if(EOFBlob(image))
            break;

          Rec.RecType=(i=ReadBlobByte(image));
          if(i==EOF)
            break;
          Rd_WP_DWORD(image,&Rec.RecordLength);
          if(EOFBlob(image))
            break;

          Header.DataOffset=TellBlob(image)+Rec.RecordLength;

          if (logging) (void)LogMagickEvent(CoderEvent,GetMagickModule(),
            "Parsing object: %X", Rec.RecType);

          switch(Rec.RecType)
            {
            case 0x0B: /* bitmap type 1 */
              BitmapHeader1.Width=ReadBlobLSBShort(image);
              BitmapHeader1.Heigth=ReadBlobLSBShort(image);
              BitmapHeader1.Depth=ReadBlobLSBShort(image);
              BitmapHeader1.HorzRes=ReadBlobLSBShort(image);
              BitmapHeader1.VertRes=ReadBlobLSBShort(image);

              if(BitmapHeader1.HorzRes && BitmapHeader1.VertRes)
                {
                  image->units=PixelsPerCentimeterResolution;
                  image->x_resolution=BitmapHeader1.HorzRes/470.0;
                  image->y_resolution=BitmapHeader1.VertRes/470.0;
                }
              image->columns=BitmapHeader1.Width;
              image->rows=BitmapHeader1.Heigth;
              bpp=BitmapHeader1.Depth;

              goto UnpackRaster;

            case 0x0E:  /*Color palette */
              WPG_Palette.StartIndex=ReadBlobLSBShort(image);
              WPG_Palette.NumOfEntries=ReadBlobLSBShort(image);

              image->colors=WPG_Palette.NumOfEntries;
              if (!AllocateImageColormap(image,image->colors))
                goto NoMemory;
              image->storage_class = PseudoClass;
              for (i=WPG_Palette.StartIndex;
                   i < (int)WPG_Palette.NumOfEntries; i++)
                {
                  image->colormap[i].red=
                    ScaleCharToQuantum(ReadBlobByte(image));
                  image->colormap[i].green=
                    ScaleCharToQuantum(ReadBlobByte(image));
                  image->colormap[i].blue=
                    ScaleCharToQuantum(ReadBlobByte(image));
                }
              break;
     
            case 0x11:  /* Start PS l1 */
              if(Rec.RecordLength > 8)
                image=ExtractPostscript(image,image_info,
                                        TellBlob(image)+8,   /* skip PS header in the wpg */
                                        (long) Rec.RecordLength-8,exception);
              break;     

            case 0x14:  /* bitmap type 2 */
              BitmapHeader2.RotAngle=ReadBlobLSBShort(image);
              BitmapHeader2.LowLeftX=ReadBlobLSBShort(image);
              BitmapHeader2.LowLeftY=ReadBlobLSBShort(image);
              BitmapHeader2.UpRightX=ReadBlobLSBShort(image);
              BitmapHeader2.UpRightY=ReadBlobLSBShort(image);
              BitmapHeader2.Width=ReadBlobLSBShort(image);
              BitmapHeader2.Heigth=ReadBlobLSBShort(image);
              BitmapHeader2.Depth=ReadBlobLSBShort(image);
              BitmapHeader2.HorzRes=ReadBlobLSBShort(image);
              BitmapHeader2.VertRes=ReadBlobLSBShort(image);

              image->units=PixelsPerCentimeterResolution;
              image->page.width=(unsigned int)
                ((BitmapHeader2.LowLeftX-BitmapHeader2.UpRightX)/470.0);
              image->page.height=(unsigned int)
                ((BitmapHeader2.LowLeftX-BitmapHeader2.UpRightY)/470.0);
              image->page.x=(int) (BitmapHeader2.LowLeftX/470.0);
              image->page.y=(int) (BitmapHeader2.LowLeftX/470.0);
              if(BitmapHeader2.HorzRes && BitmapHeader2.VertRes)
                {
                  image->x_resolution=BitmapHeader2.HorzRes/470.0;
                  image->y_resolution=BitmapHeader2.VertRes/470.0;
                }
              image->columns=BitmapHeader2.Width;
              image->rows=BitmapHeader2.Heigth;
              bpp=BitmapHeader2.Depth;

            UnpackRaster:	    
              if ((image->colors == 0) && (bpp != 24))
                {
                  image->colors=1 << bpp;
                  if (!AllocateImageColormap(image,image->colors))
                    {
                    NoMemory:
                      ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
                                           image)
                    }
                  image->storage_class = PseudoClass;
                  /* printf("Load default colormap \n"); */
                  for (i=0; (i < (int) image->colors) && (i < 256); i++)
                    {               
                      image->colormap[i].red=ScaleCharToQuantum(WPG1_Palette[i].Red);
                      image->colormap[i].green=ScaleCharToQuantum(WPG1_Palette[i].Green);
                      image->colormap[i].blue=ScaleCharToQuantum(WPG1_Palette[i].Blue);
                    }
                }
              else
                {
                  if (bpp < 24)
                    if ( (image->colors < (1UL<<bpp)) && (bpp != 24) )
                      MagickReallocMemory(PixelPacket *,image->colormap,
                                          (1<<bpp)*sizeof(PixelPacket));
                }
          
              if (bpp == 1)
                {
                  if(image->colormap[0].red==0 &&
                     image->colormap[0].green==0 &&
                     image->colormap[0].blue==0 &&
                     image->colormap[1].red==0 &&
                     image->colormap[1].green==0 &&
                     image->colormap[1].blue==0)
                    {  /* fix crippled monochrome palette */
                      image->colormap[1].red =
                        image->colormap[1].green =
                        image->colormap[1].blue = MaxRGB;
                    }
                }      

              if(UnpackWPGRaster(image,bpp) < 0)
                /* The raster cannot be unpacked */
                {
                DecompressionFailed:
                  ThrowReaderException(CoderError,UnableToDecompressImage,
                                       image)
                    }

              if(Rec.RecType==0x14 && BitmapHeader2.RotAngle!=0 && !image_info->ping)
                {  
                  /* flop command */
                  if(BitmapHeader2.RotAngle & 0x8000)
                    {
                      rotated_image = FlopImage(image, exception);
                      rotated_image->blob = image->blob;
                      image->blob = NULL;
                      (void) RemoveLastImageFromList(&image);
                      AppendImageToList(&image,rotated_image);
                    }
                  /* flip command */
                  if(BitmapHeader2.RotAngle & 0x2000)
                    {
                      rotated_image = FlipImage(image, exception);
                      rotated_image->blob = image->blob;
                      image->blob = NULL;
                      (void) RemoveLastImageFromList(&image);
                      AppendImageToList(&image,rotated_image);		
                    }
		
		  /* rotate command */
                  if(BitmapHeader2.RotAngle & 0x0FFF)
                    {
                      rotated_image = RotateImage(image, (BitmapHeader2.RotAngle & 0x0FFF), exception);
                      rotated_image->blob = image->blob;
                      image->blob = NULL;
                      (void) RemoveLastImageFromList(&image);
                      AppendImageToList(&image,rotated_image);		
                    }                
                }

              /* Allocate next image structure. */
              AllocateNextImage(image_info,image);
              image->depth=8;
              if (image->next == (Image *) NULL)
                goto Finish;
              image=SyncNextImageInList(image);
              image->columns=image->rows=0;
              image->colors=0;
              break;

            case 0x1B:  /* Postscript l2 */
              if(Rec.RecordLength>0x3C)
                image=ExtractPostscript(image,image_info,
                                        TellBlob(image)+0x3C,   /* skip PS l2 header in the wpg */
                                        (long) Rec.RecordLength-0x3C,exception);
              break;
            }
        }
      break;

    case 2:  /* WPG level 2 */
      (void) memset(CTM,0,sizeof(CTM));
      StartWPG.PosSizePrecision = 0;
      while(!EOFBlob(image)) /* object parser loop */
        {
          (void) SeekBlob(image,Header.DataOffset,SEEK_SET);
          if(EOFBlob(image))
            break;

          Rec2.Class=(i=ReadBlobByte(image));
          if(i==EOF)
            break;
          Rec2.RecType=(i=ReadBlobByte(image));
          if(i==EOF)
            break;
          Rd_WP_DWORD(image,&Rec2.Extension);
          Rd_WP_DWORD(image,&Rec2.RecordLength);
          if(EOFBlob(image))
            break;

          Header.DataOffset=TellBlob(image)+Rec2.RecordLength;

          if (logging) (void)LogMagickEvent(CoderEvent,GetMagickModule(),
            "Parsing object: %X", Rec2.RecType);

          switch(Rec2.RecType)
            {
	    case 1:
              StartWPG.HorizontalUnits=ReadBlobLSBShort(image);
              StartWPG.VerticalUnits=ReadBlobLSBShort(image);
              StartWPG.PosSizePrecision=ReadBlobByte(image);
              break;
            case 0x0C:    /* Color palette */
              WPG_Palette.StartIndex=ReadBlobLSBShort(image);
              WPG_Palette.NumOfEntries=ReadBlobLSBShort(image);

              image->colors=WPG_Palette.NumOfEntries;
              if (!AllocateImageColormap(image,image->colors))
                ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
              for (i=WPG_Palette.StartIndex;
                   i < (int)WPG_Palette.NumOfEntries; i++)
                {
                  image->colormap[i].red=
                    ScaleCharToQuantum(ReadBlobByte(image));
                  image->colormap[i].green=
                    ScaleCharToQuantum(ReadBlobByte(image));
                  image->colormap[i].blue=
                    ScaleCharToQuantum(ReadBlobByte(image));
                  (void) ReadBlobByte(image);   /*Opacity??*/
                }
              break;
            case 0x0E:
              Bitmap2Header1.Width=ReadBlobLSBShort(image);
              Bitmap2Header1.Heigth=ReadBlobLSBShort(image);
              Bitmap2Header1.Depth=ReadBlobByte(image);
              Bitmap2Header1.Compression=ReadBlobByte(image);

              if(Bitmap2Header1.Compression > 1)
                continue; /*Unknown compression method */
              switch(Bitmap2Header1.Depth)
                {
                case 1: bpp=1; 
                  break;
                case 2: bpp=2;
                  break;
                case 3: bpp=4;
                  break;
                case 4: bpp=8;
                  break;
                case 8: bpp=24;
                  break;
                default:
                  continue;  /*Ignore raster with unknown depth*/
                }
              image->columns=Bitmap2Header1.Width;
              image->rows=Bitmap2Header1.Heigth;  

              if ((image->colors == 0) && (bpp != 24))
                {
                  image->colors=1 << bpp;
                  if (!AllocateImageColormap(image,image->colors))
                    goto NoMemory;
                  image->storage_class = PseudoClass;
                }
              else
                {
                  if(bpp < 24)
                    if( image->colors<(1UL<<bpp) && bpp!=24 )
                      MagickReallocMemory(PixelPacket *,image->colormap,
                                          (1<<bpp)*sizeof(PixelPacket));
                }


              switch(Bitmap2Header1.Compression)
                {
                case 0:    /*Uncompressed raster*/
                  {
                    ldblk=(long) ((bpp*image->columns+7)/8);
                    BImgBuff=MagickAllocateMemory(unsigned char *,(size_t) ldblk);
                    if (BImgBuff == (unsigned char *) NULL)
                      goto NoMemory;

                    for(i=0; i< (long) image->rows; i++)
                      {
                        (void) ReadBlob(image,ldblk,(char *) BImgBuff);
                        (void) InsertRow(BImgBuff,i,image,bpp);
                      }

                    if(BImgBuff)
                      MagickFreeMemory(BImgBuff);
                    break;
                  }
                case 1:    /*RLE for WPG2 */
                  {
                    if( UnpackWPG2Raster(image,bpp) < 0)
                      goto DecompressionFailed;
                    break;
                  }   
                }


              if(CTM[0][0]<0 && !image_info->ping)
		{		/*?? RotAngle=360-RotAngle;*/
		  rotated_image = FlopImage(image, exception);
		  rotated_image->blob = image->blob;
		  image->blob = NULL;
		  (void) RemoveLastImageFromList(&image);
		  AppendImageToList(&image,rotated_image);
                  /* Try to change CTM according to Flip - I am not sure, must be checked.		  
                     Tx(0,0)=-1;      Tx(1,0)=0;   Tx(2,0)=0;
                     Tx(0,1)= 0;      Tx(1,1)=1;   Tx(2,1)=0;
                     Tx(0,2)=(WPG._2Rect.X_ur+WPG._2Rect.X_ll);
                     Tx(1,2)=0;   Tx(2,2)=1; */                  
                }
              if(CTM[1][1]<0 && !image_info->ping)
		{		/*?? RotAngle=360-RotAngle;*/
		  rotated_image = FlipImage(image, exception);
		  rotated_image->blob = image->blob;
		  image->blob = NULL;
		  (void) RemoveLastImageFromList(&image);
		  AppendImageToList(&image,rotated_image);
                  /* Try to change CTM according to Flip - I am not sure, must be checked.
                     float_matrix Tx(3,3);
                     Tx(0,0)= 1;   Tx(1,0)= 0;   Tx(2,0)=0;
                     Tx(0,1)= 0;   Tx(1,1)=-1;   Tx(2,1)=0;
                     Tx(0,2)= 0;   Tx(1,2)=(WPG._2Rect.Y_ur+WPG._2Rect.Y_ll);
                     Tx(2,2)=1; */		  
		}		
		

              /* Allocate next image structure. */
              AllocateNextImage(image_info,image);
              image->depth=8;
              if (image->next == (Image *) NULL)
                goto Finish;
              image=SyncNextImageInList(image);
              image->columns=image->rows=0;
              image->colors=0;
              break;

            case 0x12:  /* Postscript WPG2*/
	      i=ReadBlobLSBShort(image);
              if(Rec2.RecordLength > (unsigned int) i)
                image=ExtractPostscript(image,image_info,
                                        TellBlob(image)+i,		/*skip PS header in the wpg2*/
                                        (long) (Rec2.RecordLength-i-2),exception);
              break;

	    case 0x1B:          /*bitmap rectangle*/
              WPG2Flags = LoadWPG2Flags(image,StartWPG.PosSizePrecision,NULL,&CTM);
              break;
            }
        }

      break;

    default:
      {
        ThrowReaderException(CoderError,DataEncodingSchemeIsNotSupported,image)
          }
    }

 Finish:
  CloseBlob(image);

  {
    Image
      *p;
    
    long
      scene=0;
    
    /*
      Rewind list, removing any empty images while rewinding.
    */
    p=image;
    image=NULL;
    while (p != (Image *) NULL)
      {
        Image *tmp=p;
        if ((p->rows == 0) || (p->columns == 0)) {
          p=p->previous;
          DeleteImageFromList(&tmp);
        } else {
          image=p;
          p=p->previous;
        }
      }
    
    /*
      Fix scene numbers
    */
    for (p=image; p != (Image *) NULL; p=p->next)
      p->scene=scene++;
  }

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
%   R e g i s t e r W P G I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterWPGImage adds attributes for the WPG image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterWPGImage method is:
%
%      RegisterWPGImage(void)
%
*/
ModuleExport void RegisterWPGImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("WPG");
  entry->decoder=(DecoderHandler) ReadWPGImage;
  entry->magick=(MagickHandler) IsWPG;
  entry->description="Word Perfect Graphics";
  entry->module="WPG";
  entry->seekable_stream=True;
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r W P G I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterWPGImage removes format registrations made by the
%  WPG module from the list of supported formats.
%
%  The format of the UnregisterWPGImage method is:
%
%      UnregisterWPGImage(void)
%
*/
ModuleExport void UnregisterWPGImage(void)
{
  (void) UnregisterMagickInfo("WPG");
}
