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
%                                                                             %
%                            EEEEE  M   M  FFFFF                              %
%                            E      MM MM  F                                  %
%                            EEE    M M M  FFF                                %
%                            E      M   M  F                                  %
%                            EEEEE  M   M  F                                  %
%                                                                             %
%                                                                             %
%                  Read Windows Enahanced Metafile Format.                    %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                              Bill Radcliffe                                 %
%                                   2001                                      %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

/*
 * Include declarations.
 */

#include "magick/studio.h"
#include "magick/blob.h"
#include "magick/pixel_cache.h"
#include "magick/magick.h"
#include "magick/utility.h"
#if defined(HasWINGDI32)
#  if defined(__CYGWIN__)
#    include <windows.h>
#  else
     /* All MinGW needs ... */
#    include <wingdi.h>
#  endif
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  R e a d E M F I m a g e                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadEMFImage reads an Microsoft Windows Enhanced MetaFile (EMF) or
%  Windows MetaFile (WMF) file using the Windows API and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadEMFImage method is:
%
%      Image *ReadEMFImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadEMFImage returns a pointer to the image after
%      reading.  A null image is returned if there is a a memory shortage or
%      if the image cannot be read.
%
%    o image_info: The image info..
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

/*
  This method reads either an enhanced metafile, a regular 16bit Windows
  metafile, or an Aldus Placeable metafile and converts it into an enhanced
  metafile.  Width and height are returned in .01mm units.
*/
#if defined(HasWINGDI32)
static HENHMETAFILE ReadEnhMetaFile(const char *szFileName,long *width,
  long *height)
{
#pragma pack( push, 2 )
  typedef struct
  {
    DWORD dwKey;
    WORD hmf;
    SMALL_RECT bbox;
    WORD wInch;
    DWORD dwReserved;
    WORD wCheckSum;
  } APMHEADER, *PAPMHEADER;
#pragma pack( pop )

  DWORD
    dwSize;

  ENHMETAHEADER
    emfh;

  HANDLE
    hFile;

  HDC
    hDC;

  HENHMETAFILE
    hTemp;

  LPBYTE
    pBits;

  METAFILEPICT
    mp;

  HMETAFILE
    hOld;

  *width=512;
  *height=512;
  hTemp=GetEnhMetaFile(szFileName);
  if (hTemp != (HENHMETAFILE) NULL)
    {
      /*
        Enhanced metafile.
      */
      GetEnhMetaFileHeader(hTemp,sizeof(ENHMETAHEADER),&emfh);
      *width=emfh.rclFrame.right-emfh.rclFrame.left;
      *height=emfh.rclFrame.bottom-emfh.rclFrame.top;
      return(hTemp);
    }
  hOld=GetMetaFile(szFileName);
  if (hOld != (HMETAFILE) NULL)
    {
      /*
        16bit windows metafile.
      */
      dwSize=GetMetaFileBitsEx(hOld,0,NULL);
      if (dwSize == 0)
        {
          DeleteMetaFile(hOld);
          return((HENHMETAFILE) NULL);
        }
      pBits=MagickAllocateMemory(LPBYTE,dwSize);
      if (pBits == (LPBYTE) NULL)
        {
          DeleteMetaFile(hOld);
          return((HENHMETAFILE) NULL);
        }
      if (GetMetaFileBitsEx(hOld,dwSize,pBits) == 0)
        {
          MagickFreeMemory(pBits);
          DeleteMetaFile(hOld);
          return((HENHMETAFILE) NULL);
        }
      /*
        Make an enhanced metafile from the windows metafile.
      */
      mp.mm=MM_ANISOTROPIC;
      mp.xExt=1000;
      mp.yExt=1000;
      mp.hMF=NULL;
      hDC=GetDC(NULL);
      hTemp=SetWinMetaFileBits(dwSize,pBits,hDC,&mp);
      ReleaseDC(NULL,hDC);
      DeleteMetaFile(hOld);
      MagickFreeMemory(pBits);
      GetEnhMetaFileHeader(hTemp,sizeof(ENHMETAHEADER),&emfh);
      *width=emfh.rclFrame.right-emfh.rclFrame.left;
      *height=emfh.rclFrame.bottom-emfh.rclFrame.top;
      return(hTemp);
    }
  /*
    Aldus Placeable metafile.
  */
  hFile=CreateFile(szFileName,GENERIC_READ,0,NULL,OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,NULL);
  if (hFile == INVALID_HANDLE_VALUE)
    return(NULL);
  dwSize=GetFileSize(hFile,NULL);
  pBits=MagickAllocateMemory(LPBYTE,dwSize);
  ReadFile(hFile,pBits,dwSize,&dwSize,NULL);
  CloseHandle(hFile);
  if (((PAPMHEADER) pBits)->dwKey != 0x9ac6cdd7l)
    {
      MagickFreeMemory(pBits);
      return((HENHMETAFILE) NULL);
    }
  /*
    Make an enhanced metafile from the placable metafile.
  */
  mp.mm=MM_ANISOTROPIC;
  mp.xExt=((PAPMHEADER) pBits)->bbox.Right-((PAPMHEADER) pBits)->bbox.Left;
  *width=mp.xExt;
  mp.xExt=(mp.xExt*2540l)/(DWORD) (((PAPMHEADER) pBits)->wInch);
  mp.yExt=((PAPMHEADER)pBits)->bbox.Bottom-((PAPMHEADER) pBits)->bbox.Top;
  *height=mp.yExt;
  mp.yExt=(mp.yExt*2540l)/(DWORD) (((PAPMHEADER) pBits)->wInch);
  mp.hMF=NULL;
  hDC=GetDC(NULL);
  hTemp=SetWinMetaFileBits(dwSize,&(pBits[sizeof(APMHEADER)]),hDC,&mp);
  ReleaseDC(NULL,hDC);
  MagickFreeMemory(pBits);
  return(hTemp);
}

#define CENTIMETERS_INCH 2.54

static Image *ReadEMFImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  BITMAPINFO
    DIBinfo;

  HBITMAP
    hBitmap,
    hOldBitmap;

  HDC
    hDC;

  HENHMETAFILE
    hemf;

  Image
    *image;

  long
    height,
    width,
    y;

  RECT
    rect;

  register long
    x;

  register PixelPacket
    *q;

  RGBQUAD
    *pBits,
    *ppBits;

  image=AllocateImage(image_info);
  hemf=ReadEnhMetaFile(image_info->filename,&width,&height);
  if (!hemf)
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
  if ((image->columns == 0) || (image->rows == 0))
    {
      double
        y_resolution = 72.0,
        x_resolution = 72.0;

      if (image->y_resolution > 0)
        {
          y_resolution=image->y_resolution;
          if (image->units == PixelsPerCentimeterResolution)
            y_resolution*=CENTIMETERS_INCH;
        }
      if (image->x_resolution > 0)
        {
          x_resolution=image->x_resolution;
          if (image->units == PixelsPerCentimeterResolution)
            x_resolution*=CENTIMETERS_INCH;
        }
      image->rows=ceil((height*CENTIMETERS_INCH)/1000*y_resolution);
      image->columns=ceil((width*CENTIMETERS_INCH)/1000*x_resolution);
    }
  if (image_info->size != (char *) NULL)
    {
      long
        x;

      image->columns=width;
      image->rows=height;
      x=0;
      y=0;
      (void) GetGeometry(image_info->size,&x,&y,&image->columns,&image->rows);
    }
  if (image_info->page != (char *) NULL)
    {
      char
        *geometry;

      long
        sans;

      register char
        *p;

/*       unsigned int */
/*         flags; */

      geometry=GetPageGeometry(image_info->page);
      p=strchr(geometry,'>');
      if (!p)
        {
          /*flags=*/ (void) GetMagickGeometry(geometry,&sans,&sans,&image->columns,
            &image->rows);
          if (image->x_resolution != 0.0)
            image->columns=(unsigned int)
              ((image->columns*image->x_resolution)+0.5);
          if (image->y_resolution != 0.0)
            image->rows=(unsigned int)
              ((image->rows*image->y_resolution)+0.5);
        }
      else
        {
          *p='\0';
          /*flags=*/ (void) GetMagickGeometry(geometry,&sans,&sans,&image->columns,
            &image->rows);
          if (image->x_resolution != 0.0)
            image->columns=(unsigned int)
              (((image->columns*image->x_resolution)/72.0)+0.5);
          if (image->y_resolution != 0.0)
            image->rows=(unsigned int)
              (((image->rows*image->y_resolution)/72.0)+0.5);
        }
      MagickFreeMemory(geometry);
    }
  hDC=GetDC(NULL);
  if (!hDC)
    ThrowReaderException(CoderError,UnableToCreateADC,image);
  /*
    Initialize the bitmap header info.
  */
  (void) memset(&DIBinfo,0,sizeof(BITMAPINFO));
  DIBinfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
  DIBinfo.bmiHeader.biWidth=image->columns;
  DIBinfo.bmiHeader.biHeight=(-1)*image->rows;
  DIBinfo.bmiHeader.biPlanes=1;
  DIBinfo.bmiHeader.biBitCount=32;
  DIBinfo.bmiHeader.biCompression=BI_RGB;
  hBitmap=
    CreateDIBSection(hDC,&DIBinfo,DIB_RGB_COLORS,(void **) &ppBits,NULL,0);
  ReleaseDC(NULL,hDC);
  if (!hBitmap)
    ThrowReaderException(CoderError,UnableToCreateBitmap,image);
  hDC=CreateCompatibleDC(NULL);
  if (!hDC)
    {
      DeleteObject(hBitmap);
      ThrowReaderException(CoderError,UnableToCreateADC,image);
    }
  hOldBitmap=(HBITMAP) SelectObject(hDC,hBitmap);
  if (!hOldBitmap)
    {
      DeleteDC(hDC);
      DeleteObject(hBitmap);
      ThrowReaderException(CoderError,UnableToCreateBitmap,image);
    }
  /*
    Initialize the bitmap to the image background color.
  */
  pBits=ppBits;
  for (y=0; y < (long) image->rows; y++)
  {
    for (x=0; x < (long) image->columns; x++)
    {
      pBits->rgbRed=ScaleQuantumToChar(image->background_color.red);
      pBits->rgbGreen=ScaleQuantumToChar(image->background_color.green);
      pBits->rgbBlue=ScaleQuantumToChar(image->background_color.blue);
      pBits++;
    }
  }
  rect.top=0;
  rect.left=0;
  rect.right=image->columns;
  rect.bottom=image->rows;
  /*
    Convert metafile pixels.
  */
  PlayEnhMetaFile(hDC,hemf,&rect);
  pBits=ppBits;
  for (y=0; y < (long) image->rows; y++)
  {
    q=SetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      q->red=ScaleCharToQuantum(pBits->rgbRed);
      q->green=ScaleCharToQuantum(pBits->rgbGreen);
      q->blue=ScaleCharToQuantum(pBits->rgbBlue);
      q->opacity=OpaqueOpacity;
      pBits++;
      q++;
    }
    if (!SyncImagePixels(image))
      break;
  }
  if (hemf)
    DeleteEnhMetaFile(hemf);
  SelectObject(hDC,hOldBitmap);
  DeleteDC(hDC);
  DeleteObject(hBitmap);
  return(image);
}
#endif /* HasWINGDI32 */

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r E M F I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterEMFImage adds attributes for the EMF image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterEMFImage method is:
%
%      RegisterEMFImage(void)
%
*/
ModuleExport void RegisterEMFImage(void)
{
#if defined(HasWINGDI32)
  MagickInfo
    *entry;

  entry = SetMagickInfo("EMF");
  entry->decoder=ReadEMFImage;
  entry->description="Windows WIN32 API rendered Enhanced Meta File";
  entry->blob_support=False;
  entry->module="WMF";
  (void) RegisterMagickInfo(entry);

  entry = SetMagickInfo("WMFWIN32");
  entry->decoder=ReadEMFImage;
  entry->description="Windows WIN32 API rendered Meta File";
  entry->blob_support=False;
  entry->module="WMFWIN32";
  (void) RegisterMagickInfo(entry);
#endif /* HasWINGDI32 */
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r E M F I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterEMFImage removes format registrations made by the
%  EMF module from the list of supported formats.
%
%  The format of the UnregisterEMFImage method is:
%
%      UnregisterEMFImage(void)
%
*/
ModuleExport void UnregisterEMFImage(void)
{
#if defined(HasWINGDI32)
  (void) UnregisterMagickInfo("EMF");
  (void) UnregisterMagickInfo("WMFWIN32");
#endif /* HasWINGDI32 */
}
