/*
% Copyright (C) 2003=2009 GraphicsMagick Group
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
%                            TTTTT  X   X  TTTTT                              %
%                              T     X X     T                                %
%                              T      X      T                                %
%                              T     X X     T                                %
%                              T    X   X    T                                %
%                                                                             %
%                                                                             %
%                      Render Text Onto A Canvas Image.                       %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
%                              Jaroslav Fojtik                                %
%                                   2009                                      %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Patterns accepted:
%     0,0: 129,129,129,255 
%     0,0: 129,129,129
%     0,0: (129,129,129) #818181
%     0,0: (3411594072,2774050136,1883729991) #CB58CB58A558A55870477047
%     0,0: (129,129,129,  0)  #81818100  rgba(129,129,129,0)
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/blob.h"
#include "magick/pixel_cache.h"
#include "magick/color.h"
#include "magick/constitute.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/render.h"
#include "magick/texture.h"
#include "magick/utility.h"

typedef enum
{
  NO_TXT = 0,
  IMAGEMAGICK_TXT = 1,
  TXT_GM8B_HEX,
  TXT_GM8B_PLAIN,
  TXT_GM8B_PLAIN2,
  TXT_GM16B_HEX,
  TXT_GM16B_PLAIN,
  TXT_GM32B_HEX,
  TXT_GM32B_PLAIN,
  IMAGEMAGICK_TXT_Q = 17,
  TXT_GM8B_HEX_Q,
  TXT_GM8B_PLAIN_Q,
  TXT_GM8B_PLAIN2_Q,
  TXT_GM16B_HEX_Q,
  TXT_GM16B_PLAIN_Q,
  TXT_GM32B_HEX_Q,
  TXT_GM32B_PLAIN_Q
} TXT_TYPE;

/*
  Forward declarations.
*/
static unsigned int
  WriteTXTImage(const ImageInfo *,Image *);


/** Reads up to end of line. */
static void readln(Image *image, int *pch)
{
  int ch=0;
  if(pch) ch=*pch;
  else ch=' ';
  while(ch!=10 && ch!=13 && ch!=EOF)
    {
      ch = ReadBlobByte(image);
    }
  if(pch) *pch=ch;
}

static long ReadInt(Image *image, int *pch)
{
  int ch;
  long n;

  n=0;
  if(pch) ch=*pch;
  else ch=' ';

  while(isspace(ch)||ch==0)
    {
      ch = ReadBlobByte(image);
      if(ch==EOF) return(0);
    }

  while(isdigit(ch))
    {
      n=10*n+(ch-'0');
      ch = ReadBlobByte(image);
      if(ch==EOF) return(n);
    }

  if(pch) *pch=ch;
  /*  else ungetc(ch,F); */

  return(n);
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s T X T                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsTXT returns True if the image format type, identified by the
%  magick string, is TXT.
%
%  The format of the IsTXT method is:
%
%      unsigned int IsTXT(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsTXT returns True if the image format type is TXT.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsTXT(const unsigned char *magick,const size_t length)
{
  if (length < 20)
    return(False);

  {
    unsigned long
      column,
      row;

    unsigned int
      red,
      green,
      blue,
      opacity,
      hex_red,
      hex_green,
      hex_blue,
      hex_opacity;

    int
      count;

    char
      buffer[MaxTextExtent];

    (void) memset((void *)buffer,0,MaxTextExtent);
    (void) memcpy((void *)buffer,(const void *)magick,Min(MaxTextExtent,length));

    if(!strncmp(buffer,"# ImageMagick pixel enumeration:",32))
      return IMAGEMAGICK_TXT;

    count=sscanf(buffer,"%lu,%lu: (%u, %u, %u) #%02X%02X%02X",
                 &column, &row, &red, &green, &blue, &hex_red, &hex_green,
                 &hex_blue);
    if ((count == 8) && (column == 0) && (row == 0) && (red == hex_red) &&
        (green == hex_green) && (blue == hex_blue))
      return(TXT_GM8B_HEX);

    count=sscanf(buffer,"%lu,%lu: (%u, %u, %u) #%04X%04X%04X",
                 &column, &row, &red, &green, &blue, &hex_red, &hex_green,
                 &hex_blue);
    if ((count == 8) && (column == 0) && (row == 0) && (red == hex_red) &&
        (green == hex_green) && (blue == hex_blue))
      return(TXT_GM16B_HEX);

    count=sscanf(buffer,"%lu,%lu: (%u, %u, %u) #%08X%08X%08X",
                 &column, &row, &red, &green, &blue, &hex_red, &hex_green,
                 &hex_blue);
    if ((count == 8) && (column == 0) && (row == 0) && (red == hex_red) &&
        (green == hex_green) && (blue == hex_blue))
      return(TXT_GM32B_HEX);

    count=sscanf(buffer,"%lu,%lu: (%u, %u, %u, %u) #%02X%02X%02X%02X",
                 &column, &row, &red, &green, &blue, &opacity, &hex_red,
                 &hex_green, &hex_blue, &hex_opacity);
    if ((count == 10) && (column == 0) && (row == 0) && (red == hex_red) &&
        (green == hex_green) && (blue == hex_blue) && (opacity == hex_opacity))
      return(TXT_GM8B_HEX_Q);

    count=sscanf(buffer,"%lu,%lu: (%u, %u, %u, %u) #%04X%04X%04X%04X",
                 &column, &row, &red, &green, &blue, &opacity, &hex_red,
                 &hex_green, &hex_blue, &hex_opacity);
    if ((count == 10) && (column == 0) && (row == 0) && (red == hex_red) &&
        (green == hex_green) && (blue == hex_blue) && (opacity == hex_opacity))
      return(TXT_GM16B_HEX_Q);

    count=sscanf(buffer,"%lu,%lu: (%u, %u, %u, %u) #%08X%08X%08X%08X",
                 &column, &row, &red, &green, &blue, &opacity, &hex_red,
                 &hex_green, &hex_blue, &hex_opacity);
    if ((count == 10) && (column == 0) && (row == 0) && (red == hex_red) &&
        (green == hex_green) && (blue == hex_blue) && (opacity == hex_opacity))
      return(TXT_GM32B_HEX_Q);

    count=sscanf(buffer,"%lu,%lu: (%u, %u, %u, %u)",
                 &column, &row, &red, &green, &blue, &opacity);
    if(count==6) return TXT_GM8B_PLAIN_Q;

    count=sscanf(buffer,"%lu,%lu: %u, %u, %u, %u",
                 &column, &row, &red, &green, &blue, &opacity);
    if(count==6) return TXT_GM8B_PLAIN2_Q;

    count=sscanf(buffer,"%lu,%lu: (%u, %u, %u)",
                 &column, &row, &red, &green, &blue);
    if(count==5) return TXT_GM8B_PLAIN;

    count=sscanf(buffer,"%lu,%lu: %u, %u, %u",
                 &column, &row, &red, &green, &blue);
    if(count==5) return TXT_GM8B_PLAIN2;

  }
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d T X T I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadTXTImage reads a text file and returns it as an image.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadTXTImage method is:
%
%      Image *ReadTXTImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadTXTImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadTXTImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    filename[MaxTextExtent],
    geometry[MaxTextExtent],
    *p,
    text[MaxTextExtent];

  double
    dx_resolution,
    dy_resolution;

  DrawInfo
    *draw_info;

  Image
    *image,
    *texture;

  long
    count,
    offset;

  RectangleInfo
    page;

  TypeMetric
    metrics;

  unsigned int
    status;
  int logging;

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

  p = ReadBlobString(image,text);
  status = IsTXT((unsigned char *)p,strlen(p));
  if (logging) (void)LogMagickEvent(CoderEvent,GetMagickModule(),
				    "File type: %d", status);

  if(status)
    {
      unsigned x,y;
      unsigned x_min,x_max,y_curr;
      int ch;
      unsigned long max,i;
      char NumOfPlanes;
      unsigned char *BImgBuff;
      magick_uint16_t *WImgBuff;
      magick_uint32_t *DImgBuff;
      magick_uint32_t R,G,B,A;
      const PixelPacket *q;
      ImportPixelAreaOptions import_options;

      (void) SeekBlob(image,0,SEEK_SET);    

      A=0;
      x=0;
      y=0;
      max=0;

      switch(status)
	{
	case TXT_GM8B_HEX:
	case TXT_GM8B_HEX_Q:  max=255; break;
	case TXT_GM16B_HEX:
	case TXT_GM16B_HEX_Q: max=65535; break;	
	case TXT_GM32B_HEX:	
	case TXT_GM32B_HEX_Q: max=65536; break;
	}

      if(!strncmp(p,"# ImageMagick pixel enumeration:",32))
	{
	  if(sscanf(p+32,"%u,%u,%u",&x_min,&y_curr,&x_max)==3)
	    {
	      if(strstr(p+32,",rgb")!=NULL)
		{
		  x = x_min-1;
		  y = y_curr-1;
		  max = x_max;
		}
	      if(strstr(p+32,",rgba")!=NULL)
		{
		  status = IMAGEMAGICK_TXT_Q;
		}
	    }
	}

      ch=0;
      if(x==0 && y==0) while(!EOFBlob(image))	/* auto detect sizes and num of planes */
	{
	  while(!(ch>='0' && ch<='9'))
	    {             /* go to the begin of number */
	      ch = ReadBlobByte(image);
	      if(ch==EOF) goto EndReading;
	      if(ch=='#') 
		{readln(image,&ch); continue;}
	      if(ch==0 || ch>128 || (ch>='a' && ch<='z') || (ch>='A' && ch<='Z'))
		{
		TXT_FAIL:			/* not a text data */
		  ThrowReaderException(CoderError,ImageTypeNotSupported,image);
		}
	    }
	  /* x,y: (R,G,B) */
	  i = ReadInt(image,&ch);		/* x */
	  if(i>x) x=i;

	  while(ch!=',')
	    {  
	      ch = ReadBlobByte(image);
	      if(ch==EOF) break;
	      if(ch==10 || ch==13) goto TXT_FAIL;
	    }
	  ch=0;
	  i=ReadInt(image,&ch);		/* y */
	  if(i>y) y=i;

	  while(ch!=':')
	    {
	      ch = ReadBlobByte(image);
	      if(ch==10 || ch==13) goto TXT_FAIL;
	      if(ch==EOF) break;
	    }
	  if(status!=TXT_GM8B_PLAIN2_Q)
	    while(ch!='(')
	      {
		ch = ReadBlobByte(image);
		if(ch==10 || ch==13) goto TXT_FAIL;
		if(ch==EOF) break;
	      }
	  ch=0;
	  R = ReadInt(image,&ch);		/* R */
	  if(R>max) max=R;

	  while(ch!=',')
	    { 
	      ch = ReadBlobByte(image);
	      if(ch==10 || ch==13) goto TXT_FAIL;
	      if(ch==EOF) break;
	    }
	  ch=0;
	  G = ReadInt(image,&ch);		/* G */
	  if(G>max) max=G;

	  while(ch!=',')
	    {
	      ch = ReadBlobByte(image);
	      if(ch==10 || ch==13) goto TXT_FAIL;
	      if(ch==EOF) break;
	    }
	  ch=0;
	  B = ReadInt(image,&ch);		/* B */
	  if(B>max) max=B;

	  if(status>16)
	    {
	      while(ch!=',')
		{
		  ch = ReadBlobByte(image);
		  if(ch==10 || ch==13) goto TXT_FAIL;
		  if(ch==EOF) break;
		}
	      ch=0;
	      A = ReadInt(image,&ch);		/* A */
	      if(A>max) max=A;
	    }

	  if(status!=TXT_GM8B_PLAIN2_Q)
	    while(ch!=')')
	      {
		ch = ReadBlobByte(image);
		if(ch==10 || ch==13) goto TXT_FAIL;
		if(ch==EOF) break;
	      }

	  readln(image,&ch);
	}

    EndReading:
      x_min = 1; x_max = 0;
      y_curr = 0;

      NumOfPlanes=8;
      /*   if(max>=    2) i=2; */
      /*   if(max>=    4) i=4; */
      /*   if(max>=   16) i=8; */
      if(max>=  256) NumOfPlanes=16;
      if(max>=65536) NumOfPlanes=32;

      if (logging) (void)LogMagickEvent(CoderEvent,GetMagickModule(),
					"Image detected [%u * %u]: %d", x, y, NumOfPlanes);

      image->depth = Min(QuantumDepth,NumOfPlanes);
      ImportPixelAreaOptionsInit(&import_options);
      import_options.endian = NativeEndian;
  
      BImgBuff = MagickAllocateMemory(unsigned char *,
				      (size_t)(x+1) * ( ((status>16)?4:3) * NumOfPlanes/8));  
      WImgBuff = (magick_uint16_t *)BImgBuff;
      DImgBuff = (magick_uint32_t *)BImgBuff;  
      if(BImgBuff==NULL) 
	ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
  
      image->columns = x+1;
      image->rows = y+1;

      (void) SeekBlob(image,0,SEEK_SET);

      while(!EOFBlob(image)) 	/* load picture data */
	{
	  x=0;
	  while(!(ch >= '0' && ch <= '9'))
	    {		/* move to the beginning of number */
	      if(EOFBlob(image)) goto FINISH;
	      ch = ReadBlobByte(image);
	      if(ch=='#') 
		{readln(image,&ch); continue;}
	    }
    
	  x = ReadInt(image,&ch);		/* x */

	  while(ch!=',')
	    {
	      ch = ReadBlobByte(image);       
	      if(ch==EOF) break;
	    }
	  ch = 0;
	  y = ReadInt(image,&ch);		/* y */

	  while(ch!=':')
	    {
	      ch = ReadBlobByte(image);
	      if(ch==EOF) break;
	    }
	  while(ch!='(')
	    {
	      ch = ReadBlobByte(image);
	      if(ch==EOF) break;
	    }
	  ch=0;
	  R = ReadInt(image,&ch);		/* R */

	  while(ch!=',')
	    {
	      ch = ReadBlobByte(image);
	      if(ch==EOF) break;
	    }
	  ch=0;
	  G = ReadInt(image,&ch);		/* G */

	  while(ch!=',')
	    {
	      ch = ReadBlobByte(image);
	      if(ch==EOF) break;
	    }
	  ch=0;
	  B = ReadInt(image,&ch);		/* B */

	  if(status>16)
	    {
	      while(ch!=',')
		{
		  ch = ReadBlobByte(image);       
		  if(ch==EOF) break;
		}
	      ch=0;
	      A = ReadInt(image,&ch);		/* A */
	      if(A>max) max=A;
	    }

	  while(ch!=')')
	    {
	      ch = ReadBlobByte(image);
	      if(ch==EOF) break;
	    }


	  /* a new line has been detected */
	  if(y!=y_curr)
	    {
	      q = SetImagePixels(image,x_min,y_curr,x_max-x_min+1,1);
	      if (q == (PixelPacket *)NULL) break;	
	      if(status>16)
		(void)ImportImagePixelArea(image,RGBAQuantum,NumOfPlanes,
					   BImgBuff + 4*x_min*(NumOfPlanes/8),&import_options,0);
	      else
		(void)ImportImagePixelArea(image,RGBQuantum,NumOfPlanes,
					   BImgBuff + 3*x_min*(NumOfPlanes/8),&import_options,0);
	      if (!SyncImagePixels(image)) break;

	      x_min = 1; x_max = 0;
	      y_curr=y;
	    }

	  if(x<image->columns)
	    {
	      if(status>16)
		{
		  switch(NumOfPlanes)
		    {
		    case 8: BImgBuff[0+4*x] = R;
		      BImgBuff[1+4*x] = G;
		      BImgBuff[2+4*x] = B;
		      BImgBuff[3+4*x] = A;
		      break;
		    case 16:WImgBuff[0+4*x] = R;
		      WImgBuff[1+4*x] = G;
		      WImgBuff[2+4*x] = B;
		      WImgBuff[3+4*x] = A;
		      break;
		    case 32:DImgBuff[0+4*x] = R;
		      DImgBuff[1+4*x] = G;
		      DImgBuff[2+4*x] = B;
		      DImgBuff[3+4*x] = A;
		      break;
		    }    
		}
	      else
		{
		  switch(NumOfPlanes)
		    {
		    case 8: BImgBuff[0+3*x] = R;
		      BImgBuff[1+3*x] = G;
		      BImgBuff[2+3*x] = B;
		      break;
		    case 16:WImgBuff[0+3*x] = R;
		      WImgBuff[1+3*x] = G;
		      WImgBuff[2+3*x] = B;
		      break;
		    case 32:DImgBuff[0+3*x] = R;
		      DImgBuff[1+3*x] = G;
		      DImgBuff[2+3*x] = B;
		      break;
		    }    
		}
	      if(x_min>x_max) 
		x_max=x_min=x;
	      else
		{
		  if(x<x_min) x_min=x;
		  if(x>x_max) x_max=x;
		}
	    }

	  readln(image,&ch);
	}

    FINISH:
      if(x_min<=x_max)
	{
	  q = SetImagePixels(image,x_min,y_curr,x_max-x_min+1,1);	  
	  if (q != (PixelPacket *)NULL)
	    {
	      if(status>16)        
		(void)ImportImagePixelArea(image, RGBAQuantum, NumOfPlanes,
					   BImgBuff + 4*x_min*(NumOfPlanes/8), &import_options, 0);
	      else
		(void)ImportImagePixelArea(image, RGBQuantum, NumOfPlanes,
					   BImgBuff + 3*x_min*(NumOfPlanes/8), &import_options, 0);
	      if(!SyncImagePixels(image))
		{
		  if (logging) (void)LogMagickEvent(CoderEvent,GetMagickModule(),
						    "  TXT failed to sync image pixels for a row %u", y_curr);
		}

	    }
	}

      MagickFreeMemory(BImgBuff);
      goto TXT_FINISH;    
    }

  /*
    Set the page geometry.
  */
  dx_resolution=72.0;
  dy_resolution=72.0;
  if ((image->x_resolution == 0.0) || (image->y_resolution == 0.0))
    {
      char
        density[MaxTextExtent];

      (void) strcpy(density,PSDensityGeometry);
      count=GetMagickDimension(density,&image->x_resolution,
			       &image->y_resolution,NULL,NULL);
      if (count != 2)
        image->y_resolution=image->x_resolution;
    }
  SetGeometry(image,&page);
  page.width=612;
  page.height=792;
  (void) GetGeometry("612x792+43+43",&page.x,&page.y,&page.width,&page.height);
  if (image_info->page != (char *) NULL)
    (void) GetGeometry(image_info->page,&page.x,&page.y,&page.width,
		       &page.height);
  /*
    Initialize Image structure.
  */
  image->columns=(unsigned long)
    ceil(((page.width*image->x_resolution)/dx_resolution)-0.5);
  image->rows=(unsigned long)
    ceil(((page.height*image->y_resolution)/dy_resolution)-0.5);
  texture=(Image *) NULL;
  if (image_info->texture != (char *) NULL)
    {
      ImageInfo
        *clone_info;

      clone_info=CloneImageInfo(image_info);
      clone_info->blob=(void *) NULL;
      clone_info->length=0;
      (void) strlcpy(clone_info->filename,image_info->texture,MaxTextExtent);
      texture=ReadImage(clone_info,exception);
      DestroyImageInfo(clone_info);
    }
  /*
    Annotate the text image.
  */
  (void) SetImage(image,OpaqueOpacity);
  draw_info=CloneDrawInfo(image_info,(DrawInfo *) NULL);
  draw_info->fill=image_info->pen;
  (void) CloneString(&draw_info->text,image_info->filename);
  FormatString(geometry,"0x0%+ld%+ld",page.x,page.y);
  (void) CloneString(&draw_info->geometry,geometry);
  status=GetTypeMetrics(image,draw_info,&metrics);
  if (status == False)
    ThrowReaderException(TypeError,UnableToGetTypeMetrics,image);
  (void) strlcpy(filename,image_info->filename,MaxTextExtent);
  if (draw_info->text != '\0')
    *draw_info->text='\0';  

  for (offset=2*page.y; p != (char *) NULL; )
    {
      /*
	Annotate image with text.
      */
      (void) ConcatenateString(&draw_info->text,text);
      (void) ConcatenateString(&draw_info->text,"\\n");
      offset+=(long) (metrics.ascent-metrics.descent);
      if (image->previous == (Image *) NULL)
	if (QuantumTick(offset,image->rows))
	  if (!MagickMonitorFormatted(offset,image->rows,&image->exception,
				      LoadImageText,image->filename,
				      image->columns,image->rows))
	    break;
      p=ReadBlobString(image,text);
      if ((offset < (long) image->rows) && (p != (char *) NULL))
	continue;
      if (texture != (Image *) NULL)
	{
	  MonitorHandler
	    handler;

	  handler=SetMonitorHandler((MonitorHandler) NULL);
	  (void) TextureImage(image,texture);
	  (void) SetMonitorHandler(handler);
	}
      (void) AnnotateImage(image,draw_info);
      if (p == (char *) NULL)
	break;
      /*
	Page is full-- allocate next image structure.
      */
      *draw_info->text='\0';
      offset=2*page.y;
      AllocateNextImage(image_info,image);
      if (image->next == (Image *) NULL)
	{
	  DestroyImageList(image);
	  return((Image *) NULL);
	}
      image->next->columns=image->columns;
      image->next->rows=image->rows;
      image=SyncNextImageInList(image);
      (void) strlcpy(image->filename,filename,MaxTextExtent);
      (void) SetImage(image,OpaqueOpacity);
      if (!MagickMonitorFormatted(TellBlob(image),GetBlobSize(image),exception,
				  LoadImagesText,image->filename))
	break;
    }

  if (texture != (Image *) NULL)
    {
      MonitorHandler
        handler;

      handler=SetMonitorHandler((MonitorHandler) NULL);
      (void) TextureImage(image,texture);
      (void) SetMonitorHandler(handler);
    }
  (void) AnnotateImage(image,draw_info);
  if (texture != (Image *) NULL)
    DestroyImage(texture);
  DestroyDrawInfo(draw_info);
  while (image->previous != (Image *) NULL)
    image=image->previous;
 TXT_FINISH:
  CloseBlob(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r T X T I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterTXTImage adds attributes for the TXT image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterTXTImage method is:
%
%      RegisterTXTImage(void)
%
*/
ModuleExport void RegisterTXTImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("TEXT");
  entry->decoder=(DecoderHandler) ReadTXTImage;
  entry->encoder=(EncoderHandler) WriteTXTImage;
  entry->raw=True;
  entry->description="Text";
  entry->module="TXT";
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("TXT");
  entry->decoder=(DecoderHandler) ReadTXTImage;
  entry->encoder=(EncoderHandler) WriteTXTImage;
  entry->description="Text";
  entry->module="TXT";
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r T X T I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterTXTImage removes format registrations made by the
%  TXT module from the list of supported formats.
%
%  The format of the UnregisterTXTImage method is:
%
%      UnregisterTXTImage(void)
%
*/
ModuleExport void UnregisterTXTImage(void)
{
  (void) UnregisterMagickInfo("TEXT");
  (void) UnregisterMagickInfo("TXT");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e T X T I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteTXTImage writes the pixel values as text numbers.
%
%  The format of the WriteTXTImage method is:
%
%      unsigned int WriteTXTImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteTXTImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
static unsigned int WriteTXTImage(const ImageInfo *image_info,Image *image)
{
  char
    buffer[MaxTextExtent],
    tuple[MaxTextExtent];

  long
    y;

  register const PixelPacket
    *p;

  register long
    x;

  unsigned int
    status;

  unsigned long
    scene;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=OpenBlob(image_info,image,WriteBlobMode,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenError,UnableToOpenFile,image);
  scene=0;
  do
    {
      /*
	Convert MIFF to TXT raster pixels.
      */
      unsigned int
	depth;
  	 
      (void) TransformColorspace(image,RGBColorspace);
      if (image->depth <= 8)
	depth=8;
      else if (image->depth <= 16)
	depth=16;
      else
	depth=32;    
      for (y=0; y < (long) image->rows; y++)
	{
	  p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
	  if (p == (const PixelPacket *) NULL)
	    break;
	  for (x=0; x < (long) image->columns; x++)
	    {
	      FormatString(buffer,"%ld,%ld: ",x,y);
	      (void) WriteBlobString(image,buffer);
	      GetColorTuple(p,depth,image->matte,MagickFalse,tuple);
	      (void) strcat(tuple," ");
	      (void) WriteBlobString(image,tuple);
	      /* (void) QueryColorname(image,p,SVGCompliance,tuple,&image->exception); */
	      GetColorTuple(p,depth,image->matte,MagickTrue,tuple);
	      (void) WriteBlobString(image,tuple);
	      (void) WriteBlobString(image,"\n");
	      p++;
	    }
	}
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
  CloseBlob(image);
  return(True);
}
