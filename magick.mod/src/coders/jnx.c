/*
% Copyright (C) 2012 GraphicsMagick Group
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                               J   NN   N  X    X                            %
%                               J   N N  N   X  X                             %
%                               J   N N  N    XX                              %
%                           JJ  J   N  N N   X  X                             %
%                             JJ    N   NN  X    X                            %
%                                                                             %
%                                                                             %
%                   JNX: Garmin proprietary Image Format.                     %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                              Jaroslav Fojtik                                %
%                                  2012 -                                     %
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
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/pixel_cache.h"
#include "magick/utility.h"
#include "magick/tempfile.h"
#include "magick/magic.h"
#include "magick/resource.h"
#include "magick/attribute.h"

typedef struct
{
  int lat, lon;
} TJNXPoint;

typedef struct
{
  TJNXPoint NorthEast, SouthWest;
} TJNXRect;

typedef struct
{
  unsigned Version;
  unsigned DeviceSN;
  TJNXRect MapBounds;
  unsigned Levels;
  unsigned Expiration;
  unsigned ProductID;
  unsigned CRC;
  unsigned SigVersion;
  unsigned SigOffset;
  unsigned ZOrder;
} TJNXHeader;

typedef struct
{
  unsigned TileCount;
  unsigned TilesOffset;
  unsigned Scale;
  char *Copyright;
  unsigned Dummy;
} TJNXLevelInfo;

typedef struct
{
  TJNXRect TileBounds;
  unsigned short PicWidth, PicHeight;
  unsigned PicSize;
  unsigned PicOffset;
} TJNXTileInfo;


static Image *
ExtractTileJPG(Image * image, const ImageInfo * image_info,
               TJNXTileInfo *TileInfo, ExceptionInfo * exception)
{
  size_t
    alloc_size;

  unsigned char
    *blob;

  char img_label_str[MaxTextExtent];

  alloc_size = TileInfo->PicSize + 2;

  if (image->logging)
    (void) LogMagickEvent(CoderEvent, GetMagickModule(),
                          "JNX tile offset %u, size %u bytes",
                          TileInfo->PicOffset, TileInfo->PicSize);

  if ((alloc_size > TileInfo->PicSize) &&
      (blob = MagickAllocateMemory(unsigned char *,alloc_size)) != NULL)
    {
      /* Add missing JPEG header bytes */
      blob[0] = 0xFF;
      blob[1] = 0xD8;

      /* Copy JPG to memory blob */
      if (SeekBlob(image, TileInfo->PicOffset, SEEK_SET) == TileInfo->PicOffset)
        {
          if (ReadBlob(image,TileInfo->PicSize,blob+2) == TileInfo->PicSize)
            {
              Image
                *image2;

              if ((image2 = BlobToImage(image_info,blob,alloc_size,exception))
                  != NULL)
                {
                  /*
                    Replace current image with new image while copying
                    base image attributes.
                  */
                  (void) strlcpy(image2->filename, image->filename,
                                 sizeof(image2->filename));
                  (void) strlcpy(image2->magick_filename, image->magick_filename,
                                 sizeof(image2->magick_filename));
                  (void) strlcpy(image2->magick, image->magick,
                                 sizeof(image2->magick));
                  image2->depth = image->depth;
                  DestroyBlob(image2);
                  image2->blob = ReferenceBlob(image->blob);

                  if ((image->rows == 0) || (image->columns == 0))
                    DeleteImageFromList(&image);

                  AppendImageToList(&image, image2);
                }
            }
          else
            {
              /* Failed to read enough data from input */
              ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,
                             image->filename);
            }
        }
      else
        {
          /* Failed to seek in input */
          ThrowException(exception,BlobError,UnableToSeekToOffset,
                         image->filename);
        }
      MagickFreeMemory(blob);
    }
  else
    {
      /* Failed to allocate memory */
      ThrowException(exception,ResourceLimitError,MemoryAllocationFailed,
                     image->filename);
    }
  
  FormatString(img_label_str,"%.20g,%.20g", 
               (double) TileInfo->TileBounds.NorthEast.lat*180.0/0x7FFFFFFF, 
               (double) TileInfo->TileBounds.NorthEast.lon*180.0/0x7FFFFFFF);
  SetImageAttribute(image,"jnx:northeast",img_label_str);

  FormatString(img_label_str,"%.20g,%.20g",
               (double) TileInfo->TileBounds.SouthWest.lat*180.0/0x7FFFFFFF, 
               (double) TileInfo->TileBounds.SouthWest.lon*180.0/0x7FFFFFFF);
  SetImageAttribute(image,"jnx:southwest",img_label_str);

  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d J N X I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadJNXImage reads an JNX X image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadJNXImage method is:
%
%      Image *ReadJNXImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadJNXImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
*/
static Image *
ReadJNXImage(const ImageInfo * image_info, ExceptionInfo * exception)
{
  Image
    *image;

  unsigned int
    i,
    j;

  TJNXHeader
    JNXHeader;

  TJNXLevelInfo
    JNXLevelInfo[20];

  unsigned int
    status;

  TJNXTileInfo
    *PositionList = NULL;

  int
    logging;

  magick_int64_t
    SaveLimit;

  unsigned int
    total_tiles,
    current_tile;

  /* Open image file. */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  logging = LogMagickEvent(CoderEvent, GetMagickModule(), "enter");

  image = AllocateImage(image_info);
  status = OpenBlob(image_info, image, ReadBinaryBlobMode, exception);
  if (status == False)
    ThrowReaderException(FileOpenError, UnableToOpenFile, image);

  memset(JNXLevelInfo, 0, sizeof(JNXLevelInfo));

  /* Read JNX image header. */
  JNXHeader.Version = ReadBlobLSBLong(image);
  if (JNXHeader.Version > 4)
    ThrowReaderException(CorruptImageError, ImproperImageHeader, image);
  JNXHeader.DeviceSN = ReadBlobLSBLong(image);
  JNXHeader.MapBounds.NorthEast.lat = ReadBlobLSBLong(image);
  JNXHeader.MapBounds.NorthEast.lon = ReadBlobLSBLong(image);
  JNXHeader.MapBounds.SouthWest.lat = ReadBlobLSBLong(image);
  JNXHeader.MapBounds.SouthWest.lon = ReadBlobLSBLong(image);
  JNXHeader.Levels = ReadBlobLSBLong(image);
  if (JNXHeader.Levels > 20)
    ThrowReaderException(CorruptImageError, ImproperImageHeader, image);
  JNXHeader.Expiration = ReadBlobLSBLong(image);
  JNXHeader.ProductID = ReadBlobLSBLong(image);
  JNXHeader.CRC = ReadBlobLSBLong(image);
  JNXHeader.SigVersion = ReadBlobLSBLong(image);
  JNXHeader.SigOffset = ReadBlobLSBLong(image);
  if (JNXHeader.Version >= 4)
    JNXHeader.ZOrder = ReadBlobLSBLong(image);

  /* Read JNX image level info. */
  total_tiles = 0;
  current_tile = 0;
  for (i = 0; i < JNXHeader.Levels; i++)
    {
      JNXLevelInfo[i].TileCount = ReadBlobLSBLong(image);
      JNXLevelInfo[i].TilesOffset = ReadBlobLSBLong(image);
      JNXLevelInfo[i].Scale = ReadBlobLSBLong(image);
      total_tiles += JNXLevelInfo[i].TileCount;

      if (JNXHeader.Version >= 4)
        {
          JNXLevelInfo[i].Dummy = ReadBlobLSBLong(image);
          JNXLevelInfo[i].Copyright = NULL;
          while (ReadBlobLSBShort(image) != 0)
            {
              /* char *Copyright; */
            }
        }
      else
        {
          JNXLevelInfo[i].Copyright = NULL;
        }
    }

  /* Get the current limit */
  SaveLimit = GetMagickResourceLimit(MapResource);

  /* Temporarily set the limit to zero */
  SetMagickResourceLimit(MapResource, 0);

  /* Read JNX image data. */
  for (i = 0; i < JNXHeader.Levels; i++)
    {
      PositionList = MagickAllocateArray(TJNXTileInfo *,
                                         JNXLevelInfo[i].TileCount,
                                         sizeof(TJNXTileInfo));
      if (PositionList == NULL)
        continue;

      (void) SeekBlob(image, JNXLevelInfo[i].TilesOffset, SEEK_SET);
      for (j = 0; j < JNXLevelInfo[i].TileCount; j++)
        {
          PositionList[j].TileBounds.NorthEast.lat = ReadBlobLSBLong(image);
          PositionList[j].TileBounds.NorthEast.lon = ReadBlobLSBLong(image);
          PositionList[j].TileBounds.SouthWest.lat = ReadBlobLSBLong(image);
          PositionList[j].TileBounds.SouthWest.lon = ReadBlobLSBLong(image);
          PositionList[j].PicWidth = ReadBlobLSBShort(image);
          PositionList[j].PicHeight = ReadBlobLSBShort(image);
          PositionList[j].PicSize = ReadBlobLSBLong(image);
          PositionList[j].PicOffset = ReadBlobLSBLong(image);
        }

      for (j = 0; j < JNXLevelInfo[i].TileCount; j++)
        {
          MonitorHandler
            previous_handler;

          /* Disable progress monitor while loading individual tiles */
          previous_handler = SetMonitorHandler(0);
          image = ExtractTileJPG(image, image_info, PositionList+j, exception);
          (void) SetMonitorHandler(previous_handler);

          current_tile++;
          if (QuantumTick(current_tile,total_tiles))
            if (!MagickMonitorFormatted(current_tile,total_tiles,exception,
                                        LoadImageText,image->filename,
                                        image->columns,image->rows))
              break;
        }

      MagickFreeMemory(PositionList);
    }

  CloseBlob(image);

  /* Restore the previous limit */
  (void) SetMagickResourceLimit(MapResource, SaveLimit);

  {
    Image *p;
    long scene = 0;

    /* Rewind list, removing any empty images while rewinding. */
    p = image;
    image = NULL;
    while (p != (Image *) NULL)
      {
        Image *tmp = p;
        if ((p->rows == 0) || (p->columns == 0))
          {
            p = p->previous;
            DeleteImageFromList(&tmp);
          }
        else
          {
            image = p;
            p = p->previous;
          }
      }

    /* Fix scene numbers */
    for (p = image; p != (Image *) NULL; p = p->next)
      p->scene = scene++;
  }

  if (logging)
    (void) LogMagickEvent(CoderEvent, GetMagickModule(), "return");
  if (image == NULL)
    ThrowReaderException(CorruptImageError, ImageFileDoesNotContainAnyImageData,
			 image);

  return (image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e J N X I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function WriteJNXImage writes an JNX image to a file.  
%
%  The format of the WriteJNXImage method is:
%
%      unsigned int WriteJNXImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Function WriteJNXImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
*/
/*
static MagickPassFail WriteJNXImage(const ImageInfo *image_info,Image *image)
{
  return(0);
}
*/

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r J N X I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterJNXImage adds attributes for the JNX image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterJNXImage method is:
%
%      RegisterJNXImage(void)
%
*/
ModuleExport void RegisterJNXImage(void)
{
  MagickInfo
    *entry;

  entry = SetMagickInfo("JNX");
  entry->decoder = (DecoderHandler) ReadJNXImage;
  /* entry->encoder = (EncoderHandler)WriteJNXImage; */
  entry->description = "JNX: Garmin tile strorage format";
  entry->module = "JNX";
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r J N X I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterJNXImage removes format registrations made by the
%  JNX module from the list of supported formats.
%
%  The format of the UnregisterJNXImage method is:
%
%      UnregisterJNXImage(void)
%
*/
ModuleExport void UnregisterJNXImage(void)
{
  (void) UnregisterMagickInfo("JNX");
}
