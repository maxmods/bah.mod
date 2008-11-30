/*
% Copyright (C) 2003, 2004 GraphicsMagick Group
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
%                            DDDD   PPPP   SSSSS                              %
%                            D   D  P   P  SS                                 %
%                            D   D  PPPP    SSS                               %
%                            D   D  P         SS                              %
%                            DDDD   P      SSSSS                              %
%                                                                             %
%                                                                             %
%            Read Postscript Using the Display Postscript System.             %
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
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/utility.h"
#include "magick/xwindow.h"
#if defined(HasDPS)
# include <DPS/dpsXclient.h>
# include <DPS/dpsXpreview.h>

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d D P S I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadDPSImage reads a Adobe Postscript image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadDPSImage method is:
%
%      Image *ReadDPSImage(const ImageInfo *image_info,
%                          ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadDPSImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadDPSImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  char
    *client_name;

  Display
    *display;

  float
    pixels_per_point;

  Image
    *image;

  int
    sans,
    status;

  long
    x,
    y;

  Pixmap
    pixmap;

  register IndexPacket
    *indexes;

  register long
    i;

  register PixelPacket
    *q;

  register unsigned long
    pixel;

  Screen
    *screen;

  XColor
    *colors;

  XImage
    *dps_image;

  XRectangle
    page,
    bits_per_pixel;

  MagickXResourceInfo
    resource_info;

  XrmDatabase
    resource_database;

  XStandardColormap
    *map_info;

  XVisualInfo
    *visual_info;

  /*
    Open X server connection.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  display=XOpenDisplay(image_info->server_name);
  if (display == (Display *) NULL)
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "failed to open X11 display!");
      return((Image *) NULL);
    }
  /*
    Set our forgiving error handler.
  */
  (void) XSetErrorHandler(MagickXError);
  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),"failed to open blob!");
      return((Image *) NULL);
    }
  /*
    Get user defaults from X resource database.
  */
  client_name=SetClientName((char *) NULL);
  resource_database=MagickXGetResourceDatabase(display,client_name);
  MagickXGetResourceInfo(resource_database,client_name,&resource_info);
  /*
    Allocate standard colormap.
  */
  map_info=XAllocStandardColormap();
  visual_info=(XVisualInfo *) NULL;
  if (map_info == (XStandardColormap *) NULL)
    ThrowReaderException(XServerError,UnableToCreateStandardColormap,
      image)
  else
    {
      /*
        Initialize visual info.
      */
      (void) CloneString(&resource_info.visual_type,"default");
      visual_info=MagickXBestVisualInfo(display,map_info,&resource_info);
      map_info->colormap=(Colormap) NULL;
    }
  if ((map_info == (XStandardColormap *) NULL) ||
      (visual_info == (XVisualInfo *) NULL))
    {
      DestroyImage(image);
      MagickXFreeResources(display,visual_info,map_info,(MagickXPixelInfo *) NULL,
        (XFontStruct *) NULL,&resource_info,(MagickXWindowInfo *) NULL);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "failed to initialize visual info!");
      return((Image *) NULL);
    }
  /*
    Create a pixmap the appropriate size for the image.
  */
  screen=ScreenOfDisplay(display,visual_info->screen);
  pixels_per_point=XDPSPixelsPerPoint(screen);
  if ((image->x_resolution != 0.0) && (image->y_resolution != 0.0))
    pixels_per_point=Min(image->x_resolution,image->y_resolution)/72.0;
  status=XDPSCreatePixmapForEPSF((DPSContext) NULL,screen,GetBlobFileHandle(image),
    visual_info->depth,pixels_per_point,&pixmap,&bits_per_pixel,&page);
  if ((status == dps_status_failure) || (status == dps_status_no_extension))
    {
      DestroyImage(image);
      MagickXFreeResources(display,visual_info,map_info,(MagickXPixelInfo *) NULL,
        (XFontStruct *) NULL,&resource_info,(MagickXWindowInfo *) NULL);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "failed to create pixmap for image!");
      return((Image *) NULL);
    }
  /*
    Rasterize the file into the pixmap.
  */
  status=XDPSImageFileIntoDrawable((DPSContext) NULL,screen,pixmap,
    GetBlobFileHandle(image),bits_per_pixel.height,visual_info->depth,&page,-page.x,
    -page.y,pixels_per_point,True,False,True,&sans);
  if (status != dps_status_success)
    {
      DestroyImage(image);
      MagickXFreeResources(display,visual_info,map_info,(MagickXPixelInfo *) NULL,
        (XFontStruct *) NULL,&resource_info,(MagickXWindowInfo *) NULL);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "failed to rasterize EPS into pixmap!");
      return((Image *) NULL);
    }
  /*
    Initialize DPS X image.
  */
  dps_image=XGetImage(display,pixmap,0,0,bits_per_pixel.width,
    bits_per_pixel.height,AllPlanes,ZPixmap);
  (void) XFreePixmap(display,pixmap);
  if (dps_image == (XImage *) NULL)
    {
      DestroyImage(image);
      MagickXFreeResources(display,visual_info,map_info,(MagickXPixelInfo *) NULL,
        (XFontStruct *) NULL,&resource_info,(MagickXWindowInfo *) NULL);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "failed initialize DPS X image!");
      return((Image *) NULL);
    }
  /*
    Get the colormap colors.
  */
  colors=MagickAllocateMemory(XColor *,
                              visual_info->colormap_size*sizeof(XColor));
  if (colors == (XColor *) NULL)
    {
      DestroyImage(image);
      XDestroyImage(dps_image);
      MagickXFreeResources(display,visual_info,map_info,(MagickXPixelInfo *) NULL,
        (XFontStruct *) NULL,&resource_info,(MagickXWindowInfo *) NULL);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "failed allocate memory for colormap!");
      return((Image *) NULL);
    }
  if ((visual_info->class != DirectColor) &&
      (visual_info->class != TrueColor))
    for (i=0; i < visual_info->colormap_size; i++)
    {
      colors[i].pixel=i;
      colors[i].pad=0;
    }
  else
    {
      unsigned long
        blue,
        blue_bit,
        green,
        green_bit,
        red,
        red_bit;

      /*
        DirectColor or TrueColor visual.
      */
      red=0;
      green=0;
      blue=0;
      red_bit=visual_info->red_mask & (~(visual_info->red_mask)+1);
      green_bit=visual_info->green_mask & (~(visual_info->green_mask)+1);
      blue_bit=visual_info->blue_mask & (~(visual_info->blue_mask)+1);
      for (i=0; i < visual_info->colormap_size; i++)
      {
        colors[i].pixel=red | green | blue;
        colors[i].pad=0;
        red+=red_bit;
        if (red > visual_info->red_mask)
          red=0;
        green+=green_bit;
        if (green > visual_info->green_mask)
          green=0;
        blue+=blue_bit;
        if (blue > visual_info->blue_mask)
          blue=0;
      }
    }
  (void) XQueryColors(display,XDefaultColormap(display,visual_info->screen),
    colors,visual_info->colormap_size);
  /*
    Convert X image to MIFF format.
  */
  if ((visual_info->class != TrueColor) &&
      (visual_info->class != DirectColor))
    image->storage_class=PseudoClass;
  image->columns=dps_image->width;
  image->rows=dps_image->height;
  if (image_info->ping)
    {
      CloseBlob(image);
      return(image);
    }
  switch (image->storage_class)
  {
    case DirectClass:
    default:
    {
      register unsigned long
        color,
        index;

      unsigned long
        blue_mask,
        blue_shift,
        green_mask,
        green_shift,
        red_mask,
        red_shift;

      /*
        Determine shift and mask for red, green, and blue.
      */
      red_mask=visual_info->red_mask;
      red_shift=0;
      while ((red_mask & 0x01) == 0)
      {
        red_mask>>=1;
        red_shift++;
      }
      green_mask=visual_info->green_mask;
      green_shift=0;
      while ((green_mask & 0x01) == 0)
      {
        green_mask>>=1;
        green_shift++;
      }
      blue_mask=visual_info->blue_mask;
      blue_shift=0;
      while ((blue_mask & 0x01) == 0)
      {
        blue_mask>>=1;
        blue_shift++;
      }
      /*
        Convert X image to DirectClass packets.
      */
      if ((visual_info->colormap_size > 0) &&
          (visual_info->class == DirectColor))
        for (y=0; y < (long) image->rows; y++)
        {
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          for (x=0; x < (long) image->columns; x++)
          {
            pixel=XGetPixel(dps_image,x,y);
            index=(pixel >> red_shift) & red_mask;
            q->red=ScaleShortToQuantum(colors[index].red);
            index=(pixel >> green_shift) & green_mask;
            q->green=ScaleShortToQuantum(colors[index].green);
            index=(pixel >> blue_shift) & blue_mask;
            q->blue=ScaleShortToQuantum(colors[index].blue);
            q++;
          }
          if (!SyncImagePixels(image))
            break;
          if (QuantumTick(y,image->rows))
            if (!MagickMonitorFormatted(y,image->rows,exception,
                                        LoadImageText,image->filename))
              break;
        }
      else
        for (y=0; y < (long) image->rows; y++)
        {
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          for (x=0; x < (long) image->columns; x++)
          {
            pixel=XGetPixel(dps_image,x,y);
            color=(pixel >> red_shift) & red_mask;
            q->red=ScaleShortToQuantum((color*65535L)/red_mask);
            color=(pixel >> green_shift) & green_mask;
            q->green=ScaleShortToQuantum((color*65535L)/green_mask);
            color=(pixel >> blue_shift) & blue_mask;
            q->blue=ScaleShortToQuantum((color*65535L)/blue_mask);
            q++;
          }
          if (!SyncImagePixels(image))
            break;
          if (QuantumTick(y,image->rows))
            if (!MagickMonitorFormatted(y,image->rows,exception,
                                        LoadImageText,image->filename))
              break;
        }
      break;
    }
    case PseudoClass:
    {
      /*
        Create colormap.
      */
      if (!AllocateImageColormap(image,visual_info->colormap_size))
        {
          DestroyImage(image);
          MagickFreeMemory(colors);
          XDestroyImage(dps_image);
          MagickXFreeResources(display,visual_info,map_info,(MagickXPixelInfo *) NULL,
            (XFontStruct *) NULL,&resource_info,(MagickXWindowInfo *) NULL);
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "failed allocate image colormap!");
          return((Image *) NULL);
        }
      for (i=0; i < (long) image->colors; i++)
      {
        image->colormap[colors[i].pixel].red=
          ScaleShortToQuantum(colors[i].red);
        image->colormap[colors[i].pixel].green=
          ScaleShortToQuantum(colors[i].green);
        image->colormap[colors[i].pixel].blue=
          ScaleShortToQuantum(colors[i].blue);
      }
      /*
        Convert X image to PseudoClass packets.
      */
      for (y=0; y < (long) image->rows; y++)
      {
        q=SetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=AccessMutableIndexes(image);
        for (x=0; x < (long) image->columns; x++)
          indexes[x]=(unsigned short) XGetPixel(dps_image,x,y);
        if (!SyncImagePixels(image))
          break;
        if (QuantumTick(y,image->rows))
          if (!MagickMonitorFormatted(y,image->rows,exception,
                                      LoadImageText,image->filename))
            break;
      }
      break;
    }
  }
  MagickFreeMemory(colors);
  XDestroyImage(dps_image);
  if (image->storage_class == PseudoClass)
    (void) SyncImage(image);
  /*
    Rasterize matte image.
  */
  status=XDPSCreatePixmapForEPSF((DPSContext) NULL,screen,GetBlobFileHandle(image),1,
    pixels_per_point,&pixmap,&bits_per_pixel,&page);
  if ((status != dps_status_failure) && (status != dps_status_no_extension))
    {
      status=XDPSImageFileIntoDrawable((DPSContext) NULL,screen,pixmap,
        GetBlobFileHandle(image),bits_per_pixel.height,1,&page,-page.x,
        -page.y,pixels_per_point,True,True,True,&sans);
      if (status == dps_status_success)
        {
          XImage
            *matte_image;

          /*
            Initialize image matte.
          */
          matte_image=XGetImage(display,pixmap,0,0,bits_per_pixel.width,
            bits_per_pixel.height,AllPlanes,ZPixmap);
          (void) XFreePixmap(display,pixmap);
          if (matte_image != (XImage *) NULL)
            {
              image->storage_class=DirectClass;
              image->matte=True;
              for (y=0; y < (long) image->rows; y++)
              {
                q=SetImagePixels(image,0,y,image->columns,1);
                if (q == (PixelPacket *) NULL)
                  break;
                for (x=0; x < (long) image->columns; x++)
                {
                  q->opacity=OpaqueOpacity;
                  if (!XGetPixel(matte_image,x,y))
                    q->opacity=TransparentOpacity;
                  q++;
                }
                if (!SyncImagePixels(image))
                  break;
              }
              XDestroyImage(matte_image);
            }
        }
    }
  /*
    Free resources.
  */
  MagickXFreeResources(display,visual_info,map_info,(MagickXPixelInfo *) NULL,
    (XFontStruct *) NULL,&resource_info,(MagickXWindowInfo *) NULL);
  CloseBlob(image);
  return(image);
}
#else
static Image *ReadDPSImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  ThrowException(exception,MissingDelegateError,DPSLibraryIsNotAvailable,
    image_info->filename);
  return((Image *) NULL);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r D P S I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterDPSImage adds attributes for the Display Postscript image
%  format to the list of supported formats.  The attributes include the image
%  format tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterDPSImage method is:
%
%      RegisterDPSImage(void)
%
*/
ModuleExport void RegisterDPSImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("DPS");
  entry->decoder=(DecoderHandler) ReadDPSImage;
  entry->blob_support=False;
  entry->description="Display Postscript Interpreter";
  entry->module="DPS";
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r D P S I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterDPSImage removes format registrations made by the
%  DPS module from the list of supported formats.
%
%  The format of the UnregisterDPSImage method is:
%
%      UnregisterDPSImage(void)
%
*/
ModuleExport void UnregisterDPSImage(void)
{
  (void) UnregisterMagickInfo("DPS");
}
