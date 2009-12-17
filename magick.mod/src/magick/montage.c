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
%              M   M   OOO   N   N  TTTTT   AAA    GGGG  EEEEE                %
%              MM MM  O   O  NN  N    T    A   A  G      E                    %
%              M M M  O   O  N N N    T    AAAAA  G  GG  EEE                  %
%              M   M  O   O  N  NN    T    A   A  G   G  E                    %
%              M   M   OOO   N   N    T    A   A   GGG   EEEEE                %
%                                                                             %
%                                                                             %
%             GraphicsMagick Methods to Create Image Thumbnails               %
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
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/attribute.h"
#include "magick/composite.h"
#include "magick/constitute.h"
#include "magick/decorate.h"
#include "magick/gem.h"
#include "magick/monitor.h"
#include "magick/montage.h"
#include "magick/pixel_cache.h"
#include "magick/render.h"
#include "magick/resize.h"
#include "magick/texture.h"
#include "magick/utility.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e M o n t a g e I n f o                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CloneMontageInfo() makes a copy of the given montage info structure.  If
%  NULL is specified, a new image info structure is created initialized to
%  default values.
%
%  The format of the CloneMontageInfo method is:
%
%      MontageInfo *CloneMontageInfo(const ImageInfo *image_info,
%        const MontageInfo *montage_info)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o montage_info: The montage info.
%
%
*/
MagickExport MontageInfo *CloneMontageInfo(const ImageInfo *image_info,
  const MontageInfo *montage_info)
{
  MontageInfo
    *clone_info;

  clone_info=MagickAllocateMemory(MontageInfo *,sizeof(MontageInfo));
  if (clone_info == (MontageInfo *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToAllocateMontageInfo);
  GetMontageInfo(image_info,clone_info);
  if (montage_info == (MontageInfo *) NULL)
    return(clone_info);
  if (montage_info->geometry != (char *) NULL)
    clone_info->geometry=AllocateString(montage_info->geometry);
  if (montage_info->tile != (char *) NULL)
    clone_info->tile=AllocateString(montage_info->tile);
  if (montage_info->title != (char *) NULL)
    clone_info->title=AllocateString(montage_info->title);
  if (montage_info->frame != (char *) NULL)
    clone_info->frame=AllocateString(montage_info->frame);
  if (montage_info->texture != (char *) NULL)
    clone_info->texture=AllocateString(montage_info->texture);
  if (montage_info->font != (char *) NULL)
    clone_info->font=AllocateString(montage_info->font);
  clone_info->pointsize=montage_info->pointsize;
  clone_info->border_width=montage_info->border_width;
  clone_info->shadow=montage_info->shadow;
  clone_info->fill=montage_info->fill;
  clone_info->stroke=montage_info->stroke;
  clone_info->background_color=montage_info->background_color;
  clone_info->border_color=montage_info->border_color;
  clone_info->matte_color=montage_info->matte_color;
  clone_info->gravity=montage_info->gravity;
  (void) strlcpy(clone_info->filename,montage_info->filename,MaxTextExtent);
  return(clone_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y M o n t a g e I n f o                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyMontageInfo() deallocates memory associated with montage_info.
%
%  The format of the DestroyMontageInfo method is:
%
%      void DestroyMontageInfo(MontageInfo *montage_info)
%
%  A description of each parameter follows:
%
%    o montage_info: Specifies a pointer to an MontageInfo structure.
%
%
*/
MagickExport void DestroyMontageInfo(MontageInfo *montage_info)
{
  assert(montage_info != (MontageInfo *) NULL);
  assert(montage_info->signature == MagickSignature);
  if (montage_info->geometry != (char *) NULL)
    MagickFreeMemory(montage_info->geometry);
  if (montage_info->tile != (char *) NULL)
    MagickFreeMemory(montage_info->tile);
  if (montage_info->title != (char *) NULL)
    MagickFreeMemory(montage_info->title);
  if (montage_info->frame != (char *) NULL)
    MagickFreeMemory(montage_info->frame);
  if (montage_info->texture != (char *) NULL)
    MagickFreeMemory(montage_info->texture);
  if (montage_info->font != (char *) NULL)
    MagickFreeMemory(montage_info->font);
  MagickFreeMemory(montage_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t M o n t a g e I n f o                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetMontageInfo() initializes montage_info to default values.
%
%  The format of the GetMontageInfo method is:
%
%      void GetMontageInfo(const ImageInfo *image_info,
%        MontageInfo *montage_info)
%
%  A description of each parameter follows:
%
%    o image_info: a structure of type ImageInfo.
%
%    o montage_info: Specifies a pointer to a MontageInfo structure.
%
%
*/
MagickExport void GetMontageInfo(const ImageInfo *image_info,
  MontageInfo *montage_info)
{
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(montage_info != (MontageInfo *) NULL);
  (void) memset(montage_info,0,sizeof(MontageInfo));
  (void) strlcpy(montage_info->filename,image_info->filename,MaxTextExtent);
  montage_info->geometry=AllocateString(DefaultTileGeometry);
  montage_info->gravity=CenterGravity;
  montage_info->tile=AllocateString("6x4");
  if (image_info->font != (char *) NULL)
    montage_info->font=AllocateString(image_info->font);
  montage_info->pointsize=image_info->pointsize;
  montage_info->fill.opacity=OpaqueOpacity;
  montage_info->stroke.opacity=TransparentOpacity;
  montage_info->background_color=image_info->background_color;
  montage_info->border_color=image_info->border_color;
  montage_info->matte_color=image_info->matte_color;
  montage_info->signature=MagickSignature;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M o n t a g e I m a g e s                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Montageimages() is a layout manager that lets you tile one or more
%  thumbnails across an image canvas.
%
%  The format of the MontageImages method is:
%
%      Image *MontageImages(const Image *images,const MontageInfo *montage_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o images: Specifies a pointer to an array of Image structures.
%
%    o montage_info: Specifies a pointer to a MontageInfo structure.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif


static int SceneCompare(const void *x,const void *y)
{
  Image
    **image_1,
    **image_2;

  image_1=(Image **) x;
  image_2=(Image **) y;
  return((int) ((*image_1)->scene-(*image_2)->scene));
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

MagickExport Image *MontageImages(const Image *images,
  const MontageInfo *montage_info,ExceptionInfo *exception)
{
#define MontageImageText "[%s] Create visual image directory..."
#define TileImageText  "[%s] Create image tiles..."

  char
    tile_geometry[MaxTextExtent],
    *title;

  const ImageAttribute
    *attribute;

  DrawInfo
    *draw_info;

  FrameInfo
    frame_info;

  Image
    *image,
    **image_list,
    **master_list,
    *montage,
    *texture,
    *tile_image,
    *thumbnail;

  ImageInfo
    *image_info;

  int
    flags;

  long
    x,
    x_offset,
    y,
    y_offset;

  MonitorHandler
    handler;

  register long
    i;

  register PixelPacket
    *q;

  RectangleInfo
    bounds,
    geometry,
    tile_info;

  size_t
    number_images;

  TypeMetric
    metrics;

  unsigned int
    concatenate;

  unsigned long
    bevel_width,
    border_width,
    count,
    height,
    images_per_page,
    max_height,
    number_lines,
    tile,
    tiles,
    tiles_per_column,
    tiles_per_page,
    tiles_per_row,
    title_offset,
    total_tiles,
    width;

  /*
    Create image tiles.
  */
  assert(images != (Image *) NULL);
  assert(images->signature == MagickSignature);
  assert(montage_info != (MontageInfo *) NULL);
  assert(montage_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  number_images=GetImageListLength(images);
  master_list=ImageListToArray(images,exception);
  if (master_list == (Image **) NULL)
    ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
      UnableToCreateImageMontage);
  image_list=master_list;
  thumbnail=(Image *) NULL;
  for (i=0; i < (long) number_images; i++)
  {
    handler=SetMonitorHandler((MonitorHandler) NULL);
    image=image_list[i];
    SetGeometry(image,&geometry);
    flags=GetMagickGeometry(montage_info->geometry,&geometry.x,&geometry.y,
      &geometry.width,&geometry.height);
    /*
      Use ThumbnailImage() rather than ZoomImage() to resize montage
      thumbnails provided that the user has not specified an image
      filter, and the montage thumbnail is smaller than the
      image. This should lead to faster montages for large images.
    */
    if ((image->filter != UndefinedFilter) ||
        (geometry.width>image->columns) || (geometry.height>image->rows))
      thumbnail=ZoomImage(image,geometry.width,geometry.height,exception);
    else
      thumbnail=ThumbnailImage(image,geometry.width,geometry.height,exception);
    if (thumbnail == (Image *) NULL)
      break;
    image_list[i]=thumbnail;
    (void) SetMonitorHandler(handler);
    if (!MagickMonitorFormatted(i,number_images,&image->exception,
                                TileImageText,image->filename))
      break;
  }
  if (i < (long) number_images)
    {
      if (!thumbnail)
        --i;

      for (tile=0; tile <= (unsigned long) i; tile++)
        if (image_list[tile])
          DestroyImage(image_list[tile]);
      MagickFreeMemory(master_list);
      return((Image *) NULL);
    }
  /*
    Sort image list by increasing tile number.
  */
  for (i=0; i < (long) number_images; i++)
    if (image_list[i]->scene == 0)
      break;
  if (i == (long) number_images)
    qsort((void *) image_list,number_images,sizeof(Image *),SceneCompare);
  /*
    Determine tiles per row and column.
  */
  tiles_per_row=1;
  tiles_per_column=1;
  while ((tiles_per_row*tiles_per_column) < number_images)
  {
    tiles_per_row++;
    tiles_per_column++;
  }
  if (montage_info->tile != (char *) NULL)
    {
      x=0;
      y=0;
      tiles_per_column=number_images;
      flags=GetGeometry(montage_info->tile,&x,&y,&tiles_per_row,
        &tiles_per_column);
    }
  /*
    Determine tile sizes.
  */
  concatenate=False;
  SetGeometry(image_list[0],&tile_info);
  tile_info.x=(long) montage_info->border_width;
  tile_info.y=(long) montage_info->border_width;
  if (montage_info->geometry != (char *) NULL)
    {
      /*
        Initialize tile geometry.
      */
      flags=GetGeometry(montage_info->geometry,&tile_info.x,&tile_info.y,
        &tile_info.width,&tile_info.height);
      if ((tile_info.x == 0) && (tile_info.y == 0))
        concatenate=!((flags & WidthValue) || (flags & HeightValue));
    }
  border_width=montage_info->border_width;
  bevel_width=0;
  if (montage_info->frame != (char *) NULL)
    {
      char
        absolute_geometry[MaxTextExtent];

      (void) memset(&frame_info,0,sizeof(FrameInfo));
      frame_info.width=tile_info.width;
      frame_info.height=tile_info.height;
      FormatString(absolute_geometry,"%.1024s!",montage_info->frame);
      flags=GetMagickGeometry(absolute_geometry,&frame_info.outer_bevel,
        &frame_info.inner_bevel,&frame_info.width,&frame_info.height);
      if ((flags & HeightValue) == 0)
        frame_info.height=frame_info.width;
      if ((flags & XValue) == 0)
        frame_info.outer_bevel=(long) (frame_info.width >> 2)+1;
      if ((flags & YValue) == 0)
        frame_info.inner_bevel=frame_info.outer_bevel;
      frame_info.x=(long) frame_info.width;
      frame_info.y=(long) frame_info.height;
      bevel_width=Max(frame_info.inner_bevel,frame_info.outer_bevel);
      border_width=Max(frame_info.width,frame_info.height);
    }
  for (i=1; i < (long) number_images; i++)
  {
    if (image_list[i]->columns > tile_info.width)
      tile_info.width=image_list[i]->columns;
    if (image_list[i]->rows > tile_info.height)
      tile_info.height=image_list[i]->rows;
  }
  /*
    Initialize draw attributes.
  */
  image_info=CloneImageInfo((ImageInfo *) NULL);
  image_info->background_color=montage_info->background_color;
  image_info->border_color=montage_info->border_color;
  draw_info=CloneDrawInfo(image_info,(DrawInfo *) NULL);
  if (montage_info->font != (char *) NULL)
    (void) CloneString(&draw_info->font,montage_info->font);
  draw_info->pointsize=montage_info->pointsize;
  draw_info->gravity=NorthGravity;
  draw_info->stroke=montage_info->stroke;
  draw_info->fill=montage_info->fill;
  draw_info->text=AllocateString("");
  (void) GetTypeMetrics(image_list[0],draw_info,&metrics);
  texture=(Image *) NULL;
  if (montage_info->texture != (char *) NULL)
    {
      (void) strlcpy(image_info->filename,montage_info->texture,
        MaxTextExtent);
      texture=ReadImage(image_info,exception);
    }
  /*
    Determine the number of lines in an next label.
  */
  title=TranslateText(image_info,image_list[0],montage_info->title);
  title_offset=0;
  if (montage_info->title != (char *) NULL)
    title_offset=(unsigned long) (2*(metrics.ascent-metrics.descent)*
      MultilineCensus(title)+2*tile_info.y);
  number_lines=0;
  for (i=0; i < (long) number_images; i++)
  {
    attribute=GetImageAttribute(image_list[i],"label");
    if (attribute == (ImageAttribute *) NULL)
      continue;
    if (MultilineCensus(attribute->value) > number_lines)
      number_lines=MultilineCensus(attribute->value);
  }
  /*
    Allocate next structure.
  */
  tile_image=AllocateImage(NULL);
  montage=AllocateImage(image_info);
  montage->scene=1;
  images_per_page=(number_images-1)/(tiles_per_row*tiles_per_column)+1;
  tiles=0;
  total_tiles=number_images;
  for (i=0; i < (long) images_per_page; i++)
  {
    /*
      Determine bounding box.
    */
    tiles_per_page=Min(number_images,tiles_per_row*tiles_per_column);
    x_offset=0;
    y_offset=(long) title_offset;
    max_height=0;
    bounds.width=0;
    bounds.height=0;
    for (tile=0; tile < tiles_per_page; tile++)
    {
      width=concatenate ? image_list[tile]->columns : tile_info.width;
      x_offset+=width+(tile_info.x+border_width)*2;
      if (x_offset > (long) bounds.width)
        bounds.width=x_offset;
      if (image_list[tile]->rows > max_height)
        max_height=image_list[tile]->rows;
      if (((tile+1) == tiles_per_page) || (((tile+1) % tiles_per_row) == 0))
        {
          x_offset=0;
          height=concatenate ? max_height : tile_info.height;
          y_offset+=(unsigned long) (height+(tile_info.y+border_width)*2+
            (metrics.ascent-metrics.descent+4)*number_lines+
            (montage_info->shadow ? 4 : 0));
          if (y_offset > (long) bounds.height)
            bounds.height=y_offset;
          max_height=0;
        }
    }
    /*
      Initialize montage image.
    */
    (void) strlcpy(montage->filename,montage_info->filename,MaxTextExtent);
    montage->columns=bounds.width;
    montage->rows=bounds.height;
    (void) SetImage(montage,OpaqueOpacity);
    /*
      Set montage geometry.
    */
    montage->montage=AllocateString((char *) NULL);
    count=1;
    for (tile=0; tile < tiles_per_page; tile++)
      count+=strlen(image_list[tile]->filename)+1;
    montage->directory=MagickAllocateMemory(char *,count);
    if ((montage->montage == (char *) NULL) ||
        (montage->directory == (char *) NULL))
      ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
        UnableToCreateImageMontage);
    x_offset=0;
    y_offset=(long) title_offset;
    FormatString(montage->montage,"%ldx%ld%+ld%+ld",
      (long) (tile_info.width+(tile_info.x+border_width)*2),
      (long) (tile_info.height+(tile_info.y+border_width)*2+(metrics.ascent-
      metrics.descent+4)*number_lines+(montage_info->shadow ? 4 : 0)),x_offset,
      y_offset);
    *montage->directory='\0';
    for (tile=0; tile < tiles_per_page; tile++)
    {
      (void) strlcat(montage->directory,image_list[tile]->filename,
        MaxTextExtent);
      (void) strcat(montage->directory,"\n");
    }
    handler=SetMonitorHandler((MonitorHandler) NULL);
    if (texture != (Image *) NULL)
      (void) TextureImage(montage,texture);
    if (montage_info->title != (char *) NULL)
      {
        char
          geometry[MaxTextExtent];

        /*
          Annotate composite image with title.
        */
        FormatString(geometry,"%lux%lu%+ld%+ld",montage->columns,(unsigned long)
          (2*(metrics.ascent-metrics.descent)),0L,(long) ((metrics.ascent-
          metrics.descent)+tile_info.y+4+metrics.ascent));
        (void) CloneString(&draw_info->geometry,geometry);
        (void) CloneString(&draw_info->text,title);
        (void) AnnotateImage(montage,draw_info);
      }
    (void) SetMonitorHandler(handler);
    /*
      Copy tile to the composite.
    */
    x_offset=tile_info.x;
    y_offset=(long) title_offset+tile_info.y;
    max_height=0;
    for (tile=0; tile < tiles_per_page; tile++)
    {
      /*
        Copy this tile to the composite.
      */
      handler=SetMonitorHandler((MonitorHandler) NULL);
      image=CloneImage(image_list[tile],0,0,True,exception);
      width=concatenate ? image->columns : tile_info.width;
      if (image->rows > max_height)
        max_height=image->rows;
      height=concatenate ? max_height : tile_info.height;
      if (border_width != 0)
        {
          Image
            *border_image;

          RectangleInfo
            border_info;

          /*
            Put a border around the image.
          */
          border_info.width=border_width;
          border_info.height=border_width;
          if (montage_info->frame != (char *) NULL)
            {
              border_info.width=(width-image->columns+1)/2;
              border_info.height=(height-image->rows+1)/2;
            }
          border_image=BorderImage(image,&border_info,exception);
          if (border_image != (Image *) NULL)
            {
              DestroyImage(image);
              image=border_image;
            }
        }
      /*
        Gravitate as specified by the tile gravity.
      */
      tile_image->columns=width;
      tile_image->rows=height;
      tile_image->gravity=montage_info->gravity;
      if (image->gravity != ForgetGravity)
        tile_image->gravity=image->gravity;
      FormatString(tile_geometry,"%lux%lu+0+0",image->columns,image->rows);
      flags=GetImageGeometry(tile_image,tile_geometry,False,&geometry);
      x=(long) (geometry.x+border_width);
      y=(long) (geometry.y+border_width);
      if ((montage_info->frame != (char *) NULL) && (bevel_width != 0))
        {
          FrameInfo
            tile_info;

          Image
            *frame_image;

          /*
            Put an ornamental border around this tile.
          */
          tile_info=frame_info;
          tile_info.width=width+2*frame_info.width;
          tile_info.height=height+2*frame_info.height;
          attribute=GetImageAttribute(image,"label");
          if (attribute != (const ImageAttribute *) NULL)
            tile_info.height+=(unsigned long) ((metrics.ascent-metrics.descent+
              4)*MultilineCensus(attribute->value));
          frame_image=FrameImage(image,&tile_info,exception);
          if (frame_image != (Image *) NULL)
            {
              DestroyImage(image);
              image=frame_image;
            }
          x=0;
          y=0;
        }
      if (LocaleCompare(image->magick,"NULL") != 0)
        {
          /*
            Composite background with tile.
          */
          (void) CompositeImage(montage,image->compose,image,x_offset+x,
            y_offset+y);
          if (montage_info->shadow)
            {
              register long
                columns,
                rows;

              /*
                Put a shadow under the tile to show depth.
              */
              for (rows=0; rows < ((long) image->rows-4); rows++)
              {
                q=GetImagePixels(montage,(long) (x+x_offset+image->columns),
                  (long) (y+y_offset+rows+4),Min(tile_info.x,4),1);
                if (q == (PixelPacket *) NULL)
                  break;
                for (columns=0; columns < Min(tile_info.x,4); columns++)
                {
                  Modulate(0.0,0.0,53.0,&q->red,&q->green,&q->blue);
                  q++;
                }
                if (!SyncImagePixels(montage))
                  break;
              }
              for (rows=0; rows < Min(tile_info.y,4); rows++)
              {
                q=GetImagePixels(montage,x+x_offset+4,y+y_offset+
                  (long) image->rows+rows,image->columns,1);
                if (q == (PixelPacket *) NULL)
                  break;
                for (columns=0; columns < (long) image->columns; columns++)
                {
                  Modulate(0.0,0.0,53.0,&q->red,&q->green,&q->blue);
                  q++;
                }
                if (!SyncImagePixels(montage))
                  break;
              }
            }
          attribute=GetImageAttribute(image,"label");
          if (attribute != (const ImageAttribute *) NULL)
            {
              char
                geometry[MaxTextExtent];

              /*
                Annotate composite tile with label.
              */
              FormatString(geometry,"%lux%lu%+ld%+ld",
                (montage_info->frame ? image->columns : width)-2*border_width,
                (unsigned long) (metrics.ascent-metrics.descent+4)*
                MultilineCensus(attribute->value),x_offset+border_width,
                (montage_info->frame ? y_offset+height+border_width+2 :
                y_offset+tile_info.height+border_width+(montage_info->shadow ?
                4 : 0))+(long) (metrics.ascent-metrics.descent));
              (void) CloneString(&draw_info->geometry,geometry);
              (void) CloneString(&draw_info->text,attribute->value);
              (void) AnnotateImage(montage,draw_info);
            }
        }
      x_offset+=width+(tile_info.x+border_width)*2;
      if (((tile+1) == tiles_per_page) || (((tile+1) % tiles_per_row) == 0))
        {
          x_offset=tile_info.x;
          y_offset+=(unsigned long) (height+(tile_info.y+border_width)*2+
            (metrics.ascent-metrics.descent+4)*number_lines+
            (montage_info->shadow ? 4 : 0));
          max_height=0;
        }
      DestroyImage(image_list[tile]);
      image_list[tile]=(Image *) NULL;
      (void) SetMonitorHandler(handler);
      if (!MagickMonitorFormatted(tiles,total_tiles,&image->exception,
                                  MontageImageText,image->filename))
        {
          DestroyImage(image);
          image=(Image *) NULL;
          break;
        }
      DestroyImage(image);
      image=(Image *) NULL;
      tiles++;
    }
    if ((i+1) < (long) images_per_page)
      {
        /*
          Allocate next image structure.
        */
        AllocateNextImage(image_info,montage);
        if (montage->next == (Image *) NULL)
          {
            DestroyImageList(montage);
            montage=(Image *) NULL;
            return((Image *) NULL);
          }
        montage=montage->next;
        image_list+=tiles_per_page;
        number_images-=tiles_per_page;
      }
  }
  DestroyImage(tile_image);
  if (texture != (Image *) NULL)
    DestroyImage(texture);
  MagickFreeMemory(master_list);
  DestroyDrawInfo(draw_info);
  DestroyImageInfo(image_info);
  while (montage->previous != (Image *) NULL)
    montage=montage->previous;
  return(montage);
}
