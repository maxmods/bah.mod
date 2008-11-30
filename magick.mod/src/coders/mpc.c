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
%                            M   M  PPPP    CCCC                              %
%                            MM MM  P   P  C                                  %
%                            M M M  PPPP   C                                  %
%                            M   M  P      C                                  %
%                            M   M  P       CCCC                              %
%                                                                             %
%                                                                             %
%              Read/Write Magick Persistant Cache Image Format.               %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 March 2000                                  %
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
#include "magick/pixel_cache.h"
#include "magick/blob.h"
#include "magick/color.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/profile.h"
#include "magick/utility.h"

/*
  Forward declarations.
*/
static unsigned int
  WriteMPCImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s M P C                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsMPC returns True if the image format type, identified by the
%  magick string, is an Magick Persistent Cache image.
%
%  The format of the IsMPC method is:
%
%      unsigned int IsMPC(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsMPC returns True if the image format type is MPC.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsMPC(const unsigned char *magick,const size_t length)
{
  if (length < 14)
    return(False);
  if (LocaleNCompare((char *) magick,"id=MagickCache",14) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d C A C H E I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadMPCImage reads an Magick Persistent Cache image file and returns
%  it.  It allocates the memory necessary for the new Image structure and
%  returns a pointer to the new image.
%
%  The format of the ReadMPCImage method is:
%
%      Image *ReadMPCImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  Decompression code contributed by Kyle Shorter.
%
%  A description of each parameter follows:
%
%    o image: Method ReadMPCImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadMPCImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    cache_filename[MaxTextExtent],
    id[MaxTextExtent],
    keyword[MaxTextExtent],
    *values;

  ExtendedSignedIntegralType
    offset;

  Image
    *image;

  int
    c;

  register long
    i;

  register unsigned char
    *p;

  size_t
    length;

  unsigned int
    status;

  unsigned long
    quantum_depth;

  ProfileInfo
    *profiles=0;

  unsigned int
    number_of_profiles=0;

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
  (void) strlcpy(cache_filename,image->filename,MaxTextExtent);
  AppendImageFormat("cache",cache_filename);
  c=ReadBlobByte(image);
  if (c == EOF)
    {
      DestroyImage(image);
      return((Image *) NULL);
    }
  *id='\0';
  offset=0;
  do
  {
    /*
      Decode image header;  header terminates one character beyond a ':'.
    */
    length=MaxTextExtent;
    values=AllocateString((char *) NULL);
    quantum_depth=QuantumDepth;
    image->depth=8;
    image->compression=NoCompression;
    while (isgraph(c) && (c != ':'))
    {
      register char
        *p;

      if (c == '{')
        {
          char
            *comment;

          /*
            Read comment-- any text between { }.
          */
          length=MaxTextExtent;
          comment=AllocateString((char *) NULL);
          p=comment;
          for ( ; comment != (char *) NULL; p++)
          {
            c=ReadBlobByte(image);
            if ((c == EOF) || (c == '}'))
              break;
            if ((unsigned long) (p-comment+1) >= length)
              {
                *p='\0';
                length<<=1;
                MagickReallocMemory(char *,comment,length);
                if (comment == (char *) NULL)
                  break;
                p=comment+strlen(comment);
              }
            *p=c;
          }
          if (comment == (char *) NULL)
            ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
              image);
          *p='\0';
          (void) SetImageAttribute(image,"comment",comment);
          MagickFreeMemory(comment);
          c=ReadBlobByte(image);
        }
      else
        if (isalnum(c))
          {
            /*
              Determine a keyword and its value.
            */
            p=keyword;
            do
            {
              if ((p-keyword) < (MaxTextExtent-1))
                *p++=c;
              c=ReadBlobByte(image);
            } while (isalnum(c) || (c == '-'));
            *p='\0';
            while (isspace(c) || (c == '='))
              c=ReadBlobByte(image);
            p=values;
            while ((c != '}') && (c != EOF))
            {
              if ((p-values+1) >= (int) length)
                {
                  *p='\0';
                  length<<=1;
                  MagickReallocMemory(char *,values,length);
                  if (values == (char *) NULL)
                    break;
                  p=values+strlen(values);
                }
              if (values == (char *) NULL)
                ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
              *p++=c;
              c=ReadBlobByte(image);
              if (*values != '{')
                if (isspace(c))
                  break;
            }
            *p='\0';
            /*
              Assign a value to the specified keyword.
            */
            switch (*keyword)
            {
              case 'b':
              case 'B':
              {
                if (LocaleCompare(keyword,"background-color") == 0)
                  {
                    (void) QueryColorDatabase(values,&image->background_color,
                      exception);
                    break;
                  }
                if (LocaleCompare(keyword,"blue-primary") == 0)
                  {
                    (void) sscanf(values,"%lf,%lf",
                      &image->chromaticity.blue_primary.x,
                      &image->chromaticity.blue_primary.y);
                    break;
                  }
                if (LocaleCompare(keyword,"border-color") == 0)
                  {
                    (void) QueryColorDatabase(values,&image->border_color,
                      exception);
                    break;
                  }
                (void) SetImageAttribute(image,keyword,
                  *values == '{' ? values+1 : values);
                break;
              }
              case 'c':
              case 'C':
              {
                if (LocaleCompare(keyword,"class") == 0)
                  {
                    image->storage_class=UndefinedClass;
                    if (LocaleCompare(values,"PseudoClass") == 0)
                      image->storage_class=PseudoClass;
                    else
                      if (LocaleCompare(values,"DirectClass") == 0)
                        image->storage_class=DirectClass;
                    break;
                  }
                if (LocaleCompare(keyword,"colors") == 0)
                  {
                    image->colors=atol(values);
                    break;
                  }
                if (LocaleCompare(keyword,"colorspace") == 0)
                  {
                    image->colorspace=UndefinedColorspace;
                    if (LocaleCompare(values,"CMYK") == 0)
                      image->colorspace=CMYKColorspace;
                    else
                      if (LocaleCompare(values,"RGB") == 0)
                        image->colorspace=RGBColorspace;
                    break;
                  }
                if (LocaleCompare(keyword,"compression") == 0)
                  {
                    CompressionType
                      compression;

                    compression=UndefinedCompression;
                    if (LocaleCompare("None",values) == 0)
                      compression=NoCompression;
                    if (LocaleCompare("BZip",values) == 0)
                      compression=BZipCompression;
                    if (LocaleCompare("Fax",values) == 0)
                      compression=FaxCompression;
                    if (LocaleCompare("Group4",values) == 0)
                      compression=Group4Compression;
                    if (LocaleCompare("JPEG",values) == 0)
                      compression=JPEGCompression;
                    if (LocaleCompare("Lossless",values) == 0)
                      compression=LosslessJPEGCompression;
                    if (LocaleCompare("LZW",values) == 0)
                      compression=LZWCompression;
                    if (LocaleCompare("RLE",values) == 0)
                      compression=RLECompression;
                    if (LocaleCompare("Zip",values) == 0)
                      compression=ZipCompression;
                    image->compression=compression;
                    break;
                  }
                if (LocaleCompare(keyword,"columns") == 0)
                  {
                    image->columns= atol(values);
                    break;
                  }
                (void) SetImageAttribute(image,keyword,
                  *values == '{' ? values+1 : values);
                break;
              }
              case 'd':
              case 'D':
              {
                if (LocaleCompare(keyword,"delay") == 0)
                  {
                    image->delay=atol(values);
                    break;
                  }
                if (LocaleCompare(keyword,"depth") == 0)
                  {
                    image->depth=atol(values);
                    break;
                  }
                if (LocaleCompare(keyword,"dispose") == 0)
                  {
                    image->dispose=(DisposeType) atol(values);
                    if (LocaleCompare(values,"Background") == 0)
                      image->dispose=BackgroundDispose;
                    else
                      if (LocaleCompare(values,"None") == 0)
                        image->dispose=NoneDispose;
                      else
                        if (LocaleCompare(values,"Previous") == 0)
                          image->dispose=PreviousDispose;
                    break;
                  }
                (void) SetImageAttribute(image,keyword,
                  *values == '{' ? values+1 : values);
                break;
              }
              case 'e':
              case 'E':
              {
                if (LocaleCompare(keyword,"error") == 0)
                  {
                    image->error.mean_error_per_pixel=atof(values);
                    break;
                  }
                (void) SetImageAttribute(image,keyword,
                  *values == '{' ? values+1 : values);
                break;
              }
              case 'g':
              case 'G':
              {
                if (LocaleCompare(keyword,"gamma") == 0)
                  {
                    image->gamma=atof(values);
                    break;
                  }
                if (LocaleCompare(keyword,"grayscale") == 0)
                  {
                    image->is_grayscale=(LocaleCompare(values,"True") == 0) ||
                      (LocaleCompare(values,"true") == 0);
                    break;
                  }
                if (LocaleCompare(keyword,"green-primary") == 0)
                  {
                    (void) sscanf(values,"%lf,%lf",
                      &image->chromaticity.green_primary.x,
                      &image->chromaticity.green_primary.y);
                    break;
                  }
                (void) SetImageAttribute(image,keyword,
                  *values == '{' ? values+1 : values);
                break;
              }
              case 'i':
              case 'I':
              {
                if (LocaleCompare(keyword,"id") == 0)
                  {
                    (void) strlcpy(id,values,MaxTextExtent);
                    break;
                  }
                if (LocaleCompare(keyword,"iterations") == 0)
                  {
                    image->iterations=atol(values);
                    break;
                  }
                (void) SetImageAttribute(image,keyword,
                  *values == '{' ? values+1 : values);
                break;
              }
              case 'm':
              case 'M':
              {
                if (LocaleCompare(keyword,"matte") == 0)
                  {
                    image->matte=(LocaleCompare(values,"True") == 0) ||
                      (LocaleCompare(values,"true") == 0);
                    break;
                  }
                if (LocaleCompare(keyword,"matte-color") == 0)
                  {
                    (void) QueryColorDatabase(values,&image->matte_color,
                      exception);
                    break;
                  }
                if (LocaleCompare(keyword,"maximum-error") == 0)
                  {
                    image->error.normalized_maximum_error=atof(values);
                    break;
                  }
                if (LocaleCompare(keyword,"mean-error") == 0)
                  {
                    image->error.normalized_mean_error=atof(values);
                    break;
                  }
                if (LocaleCompare(keyword,"monochrome") == 0)
                  {
                    image->is_monochrome=(LocaleCompare(values,"True") == 0) ||
                      (LocaleCompare(values,"true") == 0);
                    break;
                  }
                if (LocaleCompare(keyword,"montage") == 0)
                  {
                    (void) CloneString(&image->montage,values);
                    break;
                  }
                (void) SetImageAttribute(image,keyword,
                  *values == '{' ? values+1 : values);
                break;
              }
              case 'p':
              case 'P':
              {
                if (LocaleCompare(keyword,"page") == 0)
                  {
                    char
                      *geometry;

                    geometry=GetPageGeometry(values);
                    (void) GetGeometry(geometry,&image->page.x,&image->page.y,
                      &image->page.width,&image->page.height);
                    MagickFreeMemory(geometry);
                    break;
                  }
                if (LocaleNCompare(keyword,"profile-",8) == 0)
                  {
                    i=(long) number_of_profiles;
                    MagickReallocMemory(ProfileInfo *,profiles,(i+1)*sizeof(ProfileInfo));
                    if (profiles == (ProfileInfo *) NULL)
                      ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
                    profiles[i].name=AllocateString(keyword+8);
                    profiles[i].length=atol(values);
                    profiles[i].info=(unsigned char *) NULL;
                    number_of_profiles++;
                    break;
                  }
                (void) SetImageAttribute(image,keyword,
                  *values == '{' ? values+1 : values);
                break;
              }
              case 'o':
              case 'O':
              {
                if (LocaleCompare(keyword,"opaque") == 0)
                  {
                    image->matte=(LocaleCompare(values,"True") == 0) ||
                      (LocaleCompare(values,"true") == 0);
                    break;
                   }
                (void) SetImageAttribute(image,keyword,
                  *values == '{' ? values+1 : values);
                break;
              }
              case 'q':
              case 'Q':
              {
                if (LocaleCompare(keyword,"quantum-depth") == 0)
                  {
                    quantum_depth=atol(values);
                    break;
                  }
                (void) SetImageAttribute(image,keyword,
                  *values == '{' ? values+1 : values);
                break;
              }
              case 'r':
              case 'R':
              {
                if (LocaleCompare(keyword,"red-primary") == 0)
                  {
                    (void) sscanf(values,"%lf,%lf",
                      &image->chromaticity.red_primary.x,
                      &image->chromaticity.red_primary.y);
                    break;
                  }
                if (LocaleCompare(keyword,"rendering-intent") == 0)
                  {
                    image->rendering_intent=UndefinedIntent;
                    if (LocaleCompare(values,"Saturation") == 0)
                      image->rendering_intent=SaturationIntent;
                    else
                      if (LocaleCompare(values,"perceptual") == 0)
                        image->rendering_intent=PerceptualIntent;
                      else
                        if (LocaleCompare(values,"absolute") == 0)
                          image->rendering_intent=AbsoluteIntent;
                        else
                          if (LocaleCompare(values,"relative") == 0)
                            image->rendering_intent=RelativeIntent;
                    break;
                  }
                if (LocaleCompare(keyword,"resolution") == 0)
                  {
                    (void) GetMagickDimension(values,&image->x_resolution,
                                              &image->y_resolution,NULL,NULL);
                    break;
                  }
                if (LocaleCompare(keyword,"rows") == 0)
                  {
                    image->rows=atol(values);
                    break;
                  }
                (void) SetImageAttribute(image,keyword,
                  *values == '{' ? values+1 : values);
                break;
              }
              case 's':
              case 'S':
              {
                if (LocaleCompare(keyword,"scene") == 0)
                  {
                    image->scene=atol(values);
                    break;
                  }
                (void) SetImageAttribute(image,keyword,
                  *values == '{' ? values+1 : values);
                break;
              }
              case 'u':
              case 'U':
              {
                if (LocaleCompare(keyword,"units") == 0)
                  {
                    image->units=UndefinedResolution;
                    if (LocaleCompare(values,"pixels-per-inch") == 0)
                      image->units=PixelsPerInchResolution;
                    else
                      if (LocaleCompare(values,"pixels-per-centimeter") == 0)
                        image->units=PixelsPerCentimeterResolution;
                    break;
                  }
                (void) SetImageAttribute(image,keyword,
                  *values == '{' ? values+1 : values);
                break;
              }
              case 'w':
              case 'W':
              {
                if (LocaleCompare(keyword,"white-point") == 0)
                  {
                    (void) sscanf(values,"%lf,%lf",
                      &image->chromaticity.white_point.x,
                      &image->chromaticity.white_point.y);
                    break;
                  }
                (void) SetImageAttribute(image,keyword,
                  *values == '{' ? values+1 : values);
                break;
              }
              default:
              {
                (void) SetImageAttribute(image,keyword,
                  *values == '{' ? values+1 : values);
                break;
              }
            }
          }
        else
          c=ReadBlobByte(image);
      while (isspace(c))
        c=ReadBlobByte(image);
    }
    MagickFreeMemory(values);
    (void) ReadBlobByte(image);
    /*
      Verify that required image information is defined.
    */
    if ((LocaleCompare(id,"MagickCache") != 0) ||
        (image->storage_class == UndefinedClass) ||
        (image->compression == UndefinedCompression) || (image->columns == 0) ||
        (image->rows == 0))
      ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
    if (quantum_depth != QuantumDepth)
      ThrowReaderException(CacheError,InconsistentPersistentCacheDepth,image);
    if (image->montage != (char *) NULL)
      {
        register char
          *p;

        /*
          Image directory.
        */
        image->directory=AllocateString((char *) NULL);
        if (image->directory == (char *) NULL)
          ThrowReaderException(CorruptImageError,UnableToReadImageData,image);
        p=image->directory;
        do
        {
          *p='\0';
          if (((strlen(image->directory)+1) % MaxTextExtent) == 0)
            {
              /*
                Allocate more memory for the image directory.
              */
              MagickReallocMemory(char *,image->directory,
                (strlen(image->directory)+MaxTextExtent+1));
              if (image->directory == (char *) NULL)
                ThrowReaderException(CorruptImageError,UnableToReadImageData,
                  image);
              p=image->directory+strlen(image->directory);
            }
          c=ReadBlobByte(image);
          *p++=c;
        } while (c != '\0');
      }

    /*
      Attached profiles.
    */
    if (number_of_profiles > 0)
      {
        for (i=0; i < (long) number_of_profiles; i++)
        {
          if (profiles[i].length == 0)
            continue;
          profiles[i].info=MagickAllocateMemory(unsigned char *,profiles[i].length);
          if (profiles[i].info == (unsigned char *) NULL)
            ThrowReaderException(CorruptImageError,UnableToReadGenericProfile,
              image);
          (void) ReadBlob(image,profiles[i].length,profiles[i].info);
          (void) SetImageProfile(image,profiles[i].name,profiles[i].info,
                                 profiles[i].length);
          MagickFreeMemory(profiles[i].name);
          MagickFreeMemory(profiles[i].info);
        }
        MagickFreeMemory(profiles);
        number_of_profiles=0;
      }

    if (image->storage_class == PseudoClass)
      {
        /*
          Create image colormap.
        */
        if (!AllocateImageColormap(image,image->colors))
          ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
            image);
        if (image->colors == 0)
          for (i=0; i < 256; i++)
          {
            image->colormap[i].red=ScaleCharToQuantum(i);
            image->colormap[i].green=ScaleCharToQuantum(i);
            image->colormap[i].blue=ScaleCharToQuantum(i);
            image->colors++;
          }
        else
          {
            unsigned char
              *colormap;

            unsigned int
              packet_size;

            /*
              Read image colormap from file.
            */
            packet_size=image->depth > 8 ? 6 : 3;
            colormap=MagickAllocateMemory(unsigned char *,packet_size*image->colors);
            if (colormap == (unsigned char *) NULL)
              ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
                image);
            (void) ReadBlob(image,packet_size*image->colors,colormap);
            p=colormap;
            if (image->depth <= 8)
              for (i=0; i < (long) image->colors; i++)
              {
                image->colormap[i].red=ScaleCharToQuantum(*p++);
                image->colormap[i].green=ScaleCharToQuantum(*p++);
                image->colormap[i].blue=ScaleCharToQuantum(*p++);
              }
            else
              for (i=0; i < (long) image->colors; i++)
              {
                image->colormap[i].red=(*p++ << 8);
                image->colormap[i].red|=(*p++);
                image->colormap[i].green=(*p++ << 8);
                image->colormap[i].green|=(*p++);
                image->colormap[i].blue=(*p++ << 8);
                image->colormap[i].blue|=(*p++);
              }
            MagickFreeMemory(colormap);
          }
      }
    if (EOFBlob(image))
      {
        ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,
          image->filename);
        break;
      }
    if (image_info->ping && (image_info->subrange != 0))
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    /*
      Attach persistent pixel cache.
    */
    status=PersistCache(image,cache_filename,True,&offset,exception);
    if (status == False)
      ThrowReaderException(CacheError,UnableToPeristPixelCache,image);
    /*
      Proceed to next image.
    */
    do
    {
      c=ReadBlobByte(image);
    } while (!isgraph(c) && (c != EOF));
    if (c != EOF)
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
        status=MagickMonitorFormatted(TellBlob(image),GetBlobSize(image),
                                      exception,LoadImagesText,
                                      image->filename);
        if (status == False)
          break;
      }
  } while (c != EOF);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseBlob(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r M P C I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterMPCImage adds attributes for the Cache image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterMPCImage method is:
%
%      RegisterMPCImage(void)
%
*/
ModuleExport void RegisterMPCImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("CACHE");
  entry->description="Magick Persistent Cache image format";
  entry->module="CACHE";
  entry->coder_class=UnstableCoderClass;
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("MPC");
  entry->decoder=(DecoderHandler) ReadMPCImage;
  entry->encoder=(EncoderHandler) WriteMPCImage;
  entry->magick=(MagickHandler) IsMPC;
  entry->description="Magick Persistent Cache image format";
  entry->module="MPC";
  entry->coder_class=UnstableCoderClass;
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r M P C I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterMPCImage removes format registrations made by the
%  MPC module from the list of supported formats.
%
%  The format of the UnregisterMPCImage method is:
%
%      UnregisterMPCImage(void)
%
*/
ModuleExport void UnregisterMPCImage(void)
{
  (void) UnregisterMagickInfo("CACHE");
  (void) UnregisterMagickInfo("MPC");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e M P C I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteMPCImage writes an Magick Persistent Cache image to a file.
%
%  The format of the WriteMPCImage method is:
%
%      unsigned int WriteMPCImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows:
%
%    o status: Method WriteMPCImage return True if the image is written.
%      False is returned if there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image: A pointer to an Image structure.
%
%
*/
static unsigned int WriteMPCImage(const ImageInfo *image_info,Image *image)
{
  char
    buffer[MaxTextExtent],
    cache_filename[MaxTextExtent];

  const ImageAttribute
    *attribute;

  ExtendedSignedIntegralType
    offset;

  register long
    i;

  unsigned int
    status;

  unsigned long
    scene;

  ImageProfileIterator
    profile_iterator;
  
  const char
    *profile_name;
  
  const unsigned char
    *profile_info;
  
  size_t
    profile_length;

  /*
    Open persistent cache.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenError,UnableToOpenFile,image);
  (void) strlcpy(cache_filename,image->filename,MaxTextExtent);
  AppendImageFormat("cache",cache_filename);
  scene=0;
  offset=0;
  do
  {
    /*
      Write persistent cache meta-information.
    */
    (void) WriteBlobString(image,"id=MagickCache\n");
    FormatString(buffer,"quantum-depth=%d\n",QuantumDepth);
    (void) WriteBlobString(image,buffer);
    if (image->storage_class == PseudoClass)
      FormatString(buffer,"class=PseudoClass  colors=%u  matte=%s\n",
                   image->colors,MagickBoolToString(image->matte));
    else
      if (image->colorspace == CMYKColorspace)
        FormatString(buffer,"class=DirectClass  colorspace=CMYK  matte=%s\n",
                     MagickBoolToString(image->matte));
      else
        FormatString(buffer,"class=DirectClass  matte=%s\n",
                     MagickBoolToString(image->matte));
    (void) WriteBlobString(image,buffer);
    switch (image->compression)
    {
      default:
      case NoCompression:
      {
        (void) WriteBlobString(image,"compression=None\n");
        break;
      }
      case BZipCompression:
      {
        (void) WriteBlobString(image,"compression=BZip\n");
        break;
      }
      case FaxCompression:
      {
        (void) WriteBlobString(image,"compression=Fax\n");
        break;
      }
      case Group4Compression:
      {
        (void) WriteBlobString(image,"compression=Group4\n");
        break;
      }
      case JPEGCompression:
      {
        (void) WriteBlobString(image,"compression=JPEG\n");
        break;
      }
      case LosslessJPEGCompression:
      {
        (void) WriteBlobString(image,"compression=Lossless\n");
        break;
      }
      case LZWCompression:
      {
        (void) WriteBlobString(image,"compression=LZW\n");
        break;
      }
      case RLECompression:
      {
        (void) WriteBlobString(image,"compression=RLE\n");
        break;
      }
      case ZipCompression:
      {
        (void) WriteBlobString(image,"compression=Zip\n");
        break;
      }
    }
    FormatString(buffer,"columns=%lu  rows=%lu  depth=%u\n",image->columns,
      image->rows,image->depth);
    (void) WriteBlobString(image,buffer);
    if (image->is_monochrome != MagickFalse)
      {
        FormatString(buffer,"monochrome=True\n");
        (void) WriteBlobString(image,buffer);
      }
    if (image->is_grayscale != MagickFalse)
      {
        FormatString(buffer,"grayscale=True\n");
        (void) WriteBlobString(image,buffer);
      }
    if ((image->x_resolution != 0) && (image->y_resolution != 0))
      {
        char
          units[MaxTextExtent];

        /*
          Set image resolution.
        */
        (void) strcpy(units,"undefined");
        if (image->units == PixelsPerInchResolution)
          (void) strcpy(units,"pixels-per-inch");
        if (image->units == PixelsPerCentimeterResolution)
          (void) strcpy(units,"pixels-per-centimeter");
        FormatString(buffer,"Resolution=%gx%g  units=%.1024s\n",
          image->x_resolution,image->y_resolution,units);
        (void) WriteBlobString(image,buffer);
      }
    if ((image->page.width != 0) && (image->page.height != 0))
      {
        FormatString(buffer,"page=%lux%lu%+ld%+ld\n",image->page.width,
          image->page.height,image->page.x,image->page.y);
        (void) WriteBlobString(image,buffer);
      }
    if ((image->next != (Image *) NULL) || (image->previous != (Image *) NULL))
      {
        if (image->scene == 0)
          FormatString(buffer,"iterations=%lu  delay=%lu\n",image->iterations,
            image->delay);
        else
          FormatString(buffer,"scene=%lu  iterations=%lu  delay=%lu\n",
            image->scene,image->iterations,image->delay);
        (void) WriteBlobString(image,buffer);
      }
    else
      {
        if (image->scene != 0)
          {
            FormatString(buffer,"scene=%lu\n",image->scene);
            (void) WriteBlobString(image,buffer);
          }
        if (image->iterations != 0)
          {
            FormatString(buffer,"iterations=%lu\n",image->iterations);
            (void) WriteBlobString(image,buffer);
          }
        if (image->delay != 0)
          {
            FormatString(buffer,"delay=%lu\n",image->delay);
            (void) WriteBlobString(image,buffer);
          }
      }
    if (image->dispose != UndefinedDispose)
      {
        if (image->dispose == BackgroundDispose)
          (void) strcpy(buffer,"dispose=background\n");
        else
          if (image->dispose == NoneDispose)
            (void) strcpy(buffer,"dispose=none\n");
          else
            (void) strcpy(buffer,"dispose=previous\n");
        (void) WriteBlobString(image,buffer);
      }
    if (image->error.mean_error_per_pixel != 0.0)
      {
        FormatString(buffer,"error=%g  mean-error=%g  maximum-error=%g\n",
          image->error.mean_error_per_pixel,image->error.normalized_mean_error,
          image->error.normalized_maximum_error);
        (void) WriteBlobString(image,buffer);
      }
    if (image->rendering_intent != UndefinedIntent)
      {
        if (image->rendering_intent == SaturationIntent)
          (void) WriteBlobString(image,"rendering-intent=saturation\n");
        else
          if (image->rendering_intent == PerceptualIntent)
            (void) WriteBlobString(image,"rendering-intent=perceptual\n");
          else
            if (image->rendering_intent == AbsoluteIntent)
              (void) WriteBlobString(image,"rendering-intent=absolute\n");
            else
              (void) WriteBlobString(image,"rendering-intent=relative\n");
      }
    if (image->gamma != 0.0)
      {
        FormatString(buffer,"gamma=%g\n",image->gamma);
        (void) WriteBlobString(image,buffer);
      }
    if (image->chromaticity.white_point.x != 0.0)
      {
        /*
          Note chomaticity points.
        */
        FormatString(buffer,
          "red-primary=%g,%g  green-primary=%g,%g  blue-primary=%g,%g\n",
          image->chromaticity.red_primary.x,image->chromaticity.red_primary.y,
          image->chromaticity.green_primary.x,
          image->chromaticity.green_primary.y,
          image->chromaticity.blue_primary.x,
          image->chromaticity.blue_primary.y);
        (void) WriteBlobString(image,buffer);
        FormatString(buffer,"white-point=%g,%g\n",
          image->chromaticity.white_point.x,image->chromaticity.white_point.y);
        (void) WriteBlobString(image,buffer);
      }
    /*
      Attached profiles.
    */    
    profile_iterator=AllocateImageProfileIterator(image);
    if (profile_iterator)
      {
        while(NextImageProfile(profile_iterator,&profile_name,&profile_info,
                               &profile_length)
              != MagickFail)
          {
            FormatString(buffer,"profile-%.1024s=%lu\n",
                         profile_name == (char *) NULL ? "generic" :
                         profile_name,(unsigned long)
                         profile_length);
            (void) WriteBlobString(image,buffer);
          }
        DeallocateImageProfileIterator(profile_iterator);
      }

    if (image->montage != (char *) NULL)
      {
        FormatString(buffer,"montage=%.1024s\n",image->montage);
        (void) WriteBlobString(image,buffer);
      }
    attribute=GetImageAttribute(image,(char *) NULL);
    for ( ; attribute != (const ImageAttribute *) NULL; attribute=attribute->next)
    {
      if (attribute->value != NULL)
        {
          long
            j;

          FormatString(buffer,"%.1024s=",attribute->key);
          (void) WriteBlobString(image,buffer);
          for (j=0; j < (long) strlen(attribute->value); j++)
            if (isspace((int) attribute->value[j]))
              break;
          if (j < (long) strlen(attribute->value))
            (void) WriteBlobByte(image,'{');
          (void) WriteBlobString(image,attribute->value);
          if (j < (long) strlen(attribute->value))
            (void) WriteBlobByte(image,'}');
          (void) WriteBlobByte(image,'\n');
        }
    }
    (void) WriteBlobString(image,"\f\n:\032");
    if (image->montage != (char *) NULL)
      {
        /*
          Write montage tile directory.
        */
        if (image->directory != (char *) NULL)
          (void) WriteBlobString(image,image->directory);
        (void) WriteBlobByte(image,'\0');
      }

    /*
      Attached profiles.
    */
    profile_iterator=AllocateImageProfileIterator(image);
    if (profile_iterator)
      {
        while(NextImageProfile(profile_iterator,&profile_name,&profile_info,
                               &profile_length) != MagickFail)
          {
            if (profile_length != 0)
              (void) WriteBlob(image,profile_length,profile_info);
          }
        DeallocateImageProfileIterator(profile_iterator);
      }

    if (image->storage_class == PseudoClass)
      {
        register unsigned char
          *q;

        unsigned char
          *colormap;

        unsigned int
          packet_size;

        /*
          Allocate colormap.
        */
        packet_size=image->depth > 8 ? 6 : 3;
        colormap=MagickAllocateMemory(unsigned char *,packet_size*image->colors);
        if (colormap == (unsigned char *) NULL)
          return(False);
        /*
          Write colormap to file.
        */
        q=colormap;
        if (image->depth <= 8)
          for (i=0; i < (long) image->colors; i++)
          {
            *q++=image->colormap[i].red;
            *q++=image->colormap[i].green;
            *q++=image->colormap[i].blue;
          }
#if QuantumDepth > 8
        else
          for (i=0; i < (long) image->colors; i++)
          {
            *q++=image->colormap[i].red >> 8;
            *q++=image->colormap[i].red & 0xff;
            *q++=image->colormap[i].green >> 8;
            *q++=image->colormap[i].green & 0xff;
            *q++=image->colormap[i].blue >> 8;
            *q++=image->colormap[i].blue & 0xff;
          }
#endif /* QuantumDepth > 8 */

        (void) WriteBlob(image,packet_size*image->colors,colormap);
        MagickFreeMemory(colormap);
      }
    /*
      Initialize persistent pixel cache.
    */
    status=PersistCache(image,cache_filename,False,&offset,&image->exception);
    if (status == False)
      ThrowWriterException(CacheError,UnableToPeristPixelCache,image);
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
  return(status);
}
