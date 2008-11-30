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
%        AAA   TTTTT  TTTTT  RRRR   IIIII  BBBB   U   U  TTTTT  EEEEE         %
%       A   A    T      T    R   R    I    B   B  U   U    T    E             %
%       AAAAA    T      T    RRRR     I    BBBB   U   U    T    EEE           %
%       A   A    T      T    R R      I    B   B  U   U    T    E             %
%       A   A    T      T    R  R   IIIII  BBBB    UUU     T    EEEEE         %
%                                                                             %
%                                                                             %
%              Methods to Get/Set/Destroy Image Text Attributes               %
%                                                                             %
%                                                                             %
%                             Software Design                                 %
%                               John Cristy                                   %
%                              February 2000                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  The Attributes methods gets, sets, or destroys attributes associated
%  with a particular image (e.g. comments, copyright, author, etc).
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/attribute.h"
#include "magick/blob.h"
#include "magick/profile.h"
#include "magick/render.h"
#include "magick/tempfile.h"
#include "magick/utility.h"

/*
  Forward declarations.
*/
static void DestroyImageAttribute(ImageAttribute *attribute);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e I m a g e A t t r i b u t e s                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CloneImageAttributes() copies the text attibutes from one image to another.
%  Any text attributes in the destination image are preserved.
%  CloneImageAttributes returns MagickPass if all of the attribututes are
%  successfully cloned or MagickFail if there is a memory allocation error.
%
%  The format of the CloneImageAttributes method is:
%
%      MagickPassFail CloneImageAttributes(Image* clone_image,
%                                          const Image* original_image)
%
%  A description of each parameter follows:
%
%    o clone_image: The destination image.
%
%    o original_image: The source image.
%
%
*/
MagickExport MagickPassFail CloneImageAttributes(Image* clone_image,
  const Image* original_image)
{
  MagickPassFail
    status;

  ImageAttribute
    *cloned_attribute,
    *cloned_attributes;

  const ImageAttribute
    *attribute;

  status = MagickPass;

  /*
    Search for tail of list (if any)
  */
  for(cloned_attributes=clone_image->attributes;
      cloned_attributes != (ImageAttribute *) NULL;
      cloned_attributes=cloned_attributes->next);

  attribute=GetImageAttribute(original_image,(char *) NULL);
  for ( ; attribute != (const ImageAttribute *) NULL; attribute=attribute->next)
    {
      /*
        Construct AttributeInfo to append.
      */
      cloned_attribute=MagickAllocateMemory(ImageAttribute *,sizeof(ImageAttribute));
      if (cloned_attribute == (ImageAttribute *) NULL)
        {
          status = MagickFail;
          break;
        }
      cloned_attribute->key=AcquireString(attribute->key);
      cloned_attribute->length=attribute->length;
      cloned_attribute->value=MagickAllocateMemory(char *,cloned_attribute->length+1);
      cloned_attribute->previous=(ImageAttribute *) NULL;
      cloned_attribute->next=(ImageAttribute *) NULL;
      if ((cloned_attribute->value == (char *) NULL) ||
          (cloned_attribute->key == (char *) NULL))
        {
          DestroyImageAttribute(cloned_attribute);
          status = MagickFail;
          break;
        }
      strcpy(cloned_attribute->value,attribute->value);

      if (cloned_attributes == (ImageAttribute *) NULL)
        {
          /*
            Start list
          */
          cloned_attributes=cloned_attribute;
          clone_image->attributes=cloned_attributes;
        }
      else
        {
          /*
            Append to list
          */
          cloned_attributes->next=cloned_attribute;
          cloned_attribute->previous=cloned_attributes;
          cloned_attributes=cloned_attribute;
        }
    }

  return status;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y I m a g e A t t r i b u t e s                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyImageAttributes() deallocates memory associated with the image
%  attribute list.
%
%  The format of the DestroyImageAttributes method is:
%
%      DestroyImageAttributes(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
static void DestroyImageAttribute(ImageAttribute *attribute)
{
  if (attribute == (ImageAttribute *) NULL)
    return;
  MagickFreeMemory(attribute->value);
  MagickFreeMemory(attribute->key);
  (void) memset(attribute,0xbf,sizeof(ImageAttribute));
  MagickFreeMemory(attribute);
}
MagickExport void DestroyImageAttributes(Image *image)
{
  ImageAttribute
    *attribute;

  register ImageAttribute
    *p;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  for (p=image->attributes; p != (ImageAttribute *) NULL; )
  {
    attribute=p;
    p=p->next;
    DestroyImageAttribute(attribute);
  }
  image->attributes=(ImageAttribute *) NULL;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e A t t r i b u t e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageAttribute() searches the list of image attributes and returns
%  a pointer to the attribute if it exists otherwise NULL.
%
%  The format of the GetImageAttribute method is:
%
%      const ImageAttribute *GetImageAttribute(const Image *image,
%        const char *key)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o key:  These character strings are the name of an image attribute to
%      return.
%
%
*/

static unsigned int GenerateIPTCAttribute(Image *image,const char *key)
{
  char
    *attribute;

  int
    count,
    dataset,
    record;

  register long
    i;

  size_t
    length;

  const unsigned char
    *profile;

  size_t
    profile_length;

  if((profile=GetImageProfile(image,"IPTC",&profile_length)) == 0)
    return(False);
  count=sscanf(key,"IPTC:%d:%d",&dataset,&record);
  if (count != 2)
    return(False);
  for (i=0; i < (long) profile_length; i++)
  {
    if (profile[i] != 0x1cU)
      continue;
    if (profile[i+1] != dataset)
      continue;
    if (profile[i+2] != record)
      continue;
    length=profile[i+3] << 8;
    length|=profile[i+4];
    attribute=MagickAllocateMemory(char *,length+MaxTextExtent);
    if (attribute == (char *) NULL)
      continue;
    (void) strlcpy(attribute,(char *) profile+i+5,length);
    attribute[length]='\0';
    (void) SetImageAttribute(image,key,(const char *) attribute);
    MagickFreeMemory(attribute);
    break;
  }
  return(i < (long) profile_length);
}

static unsigned char ReadByte(unsigned char **p,size_t *length)
{
  unsigned char
    c;

  if (*length < 1)
    return(0xff);
  c=(*(*p)++);
  (*length)--;
  return(c);
}

static long ReadMSBLong(unsigned char **p,size_t *length)
{
  int
    c;

  long
    value;

  register long
    i;

  unsigned char
    buffer[4];

  if (*length < 4)
    return(-1);
  for (i=0; i < 4; i++)
  {
    c=(*(*p)++);
    (*length)--;
    buffer[i]=(unsigned char) c;
  }
  value=buffer[0] << 24;
  value|=buffer[1] << 16;
  value|=buffer[2] << 8;
  value|=buffer[3];
  return(value);
}

static int ReadMSBShort(unsigned char **p,size_t *length)
{
  int
    c,
    value;

  register long
    i;

  unsigned char
    buffer[2];

  if (*length < 2)
    return(-1);
  for (i=0; i < 2; i++)
  {
    c=(*(*p)++);
    (*length)--;
    buffer[i]=(unsigned char) c;
  }
  value=buffer[0] << 8;
  value|=buffer[1];
  return(value);
}

/*
  FIXME: length is defined as type size_t, and then code incorrectly assumes that size_t is
  a signed type
*/
static char *TracePSClippingPath(unsigned char *blob,size_t length,
  unsigned long ARGUNUSED(columns),unsigned long ARGUNUSED(rows))
{
  char
    *path,
    *message;

  int
    knot_count,
    selector;

  long
    x,
    y;

  PointInfo
    first[3],       /* First Bezier knot in sub-path */
    last[3],        /* Last seen Bezier knot in sub-path */
    point[3];       /* Current Bezier knot in sub-path */

  register long
    i;

  unsigned int
    in_subpath;

  path=AllocateString((char *) NULL);
  if (path == (char *) NULL)
    return((char *) NULL);
  message=AllocateString((char *) NULL);

  FormatString(message,"/ClipImage {\n");
  (void) ConcatenateString(&path,message);
  FormatString(message,"/c {curveto} bind def\n");
  (void) ConcatenateString(&path,message);
  FormatString(message,"/l {lineto} bind def\n");
  (void) ConcatenateString(&path,message);
  FormatString(message,"/m {moveto} bind def\n");
  (void) ConcatenateString(&path,message);
  FormatString(message,"/v {currentpoint 6 2 roll curveto} bind def\n");
  (void) ConcatenateString(&path,message);
  FormatString(message,"/y {2 copy curveto} bind def\n");
  (void) ConcatenateString(&path,message);
  FormatString(message,"/z {closepath} bind def\n");
  (void) ConcatenateString(&path,message);
  FormatString(message,"newpath\n");
  (void) ConcatenateString(&path,message);

  knot_count=0;
  in_subpath=False;

  /*
    Open and closed subpaths are all closed in the following
    parser loop as there's no way for the polygon renderer
    to render an open path to a masking image.

    The clipping path format is defined in "Adobe Photoshop File
    Formats Specification" version 6.0 downloadable from adobe.com.
  */
  while (length > 0)
  {
    selector=ReadMSBShort(&blob,&length);
    switch (selector)
    {
      case 0:
      case 3:
      {
        if (knot_count == 0)
          {
            /*
              Expected subpath length record
            */
            knot_count=ReadMSBShort(&blob,&length);
            blob+=22;
            length-=22;
          }
        else
          {
            blob+=24;
            length-=24;
          }	  
        break;
      }
      case 1:
      case 2:
      case 4:
      case 5:
      {
        if (knot_count == 0)
          {
            /*
              Unexpected subpath knot
            */
            blob+=24;
            length-=24;
          }
        else
          {
            /*
              Add sub-path knot
            */
            for (i=0; i < 3; i++)
              {
                y=ReadMSBLong(&blob,&length);
                x=ReadMSBLong(&blob,&length);
                point[i].x=(double) x/4096/4096;
                point[i].y=1.0-(double) y/4096/4096;
              }
            if (!in_subpath)
              {
                FormatString(message,"%.6f %.6f m\n",point[1].x,point[1].y);
                for (i=0; i < 3; i++)
                {
                  first[i]=point[i];
                  last[i]=point[i];
                }
              }
            else
              {
                /*
                  Handle special cases when Bezier curves are used to describe
                  corners and straight lines. This special handling is desirable
                  to bring down the size in bytes of the clipping path data.
                */
                if ((last[1].x == last[2].x) && (last[1].y == last[2].y) &&
                  (point[0].x == point[1].x) && (point[0].y == point[1].y))
                {
                  /*
                    First control point equals first anchor point and last control
                    point equals last anchow point. Straigt line between anchor points.
                  */
                  FormatString(message,"%.6f %.6f l\n",point[1].x,point[1].y);
                }
                else if ((last[1].x == last[2].x) && (last[1].y == last[2].y))
                {
                  /* First control point equals first anchor point */
                  FormatString(message,"%.6f %.6f %.6f %.6f v\n",
                    point[0].x,point[0].y,point[1].x,point[1].y);
                }
                else if ((point[0].x == point[1].x) && (point[0].y == point[1].y))
                {
                  /* Last control point equals last anchow point. */
                  FormatString(message,"%.6f %.6f %.6f %.6f y\n",
                    last[2].x,last[2].y,point[1].x,point[1].y);
                }
                else
                {
                  /* The full monty */
                  FormatString(message,"%.6f %.6f %.6f %.6f %.6f %.6f c\n",
                    last[2].x,last[2].y,point[0].x,point[0].y,point[1].x,
                    point[1].y);
                }
                for (i=0; i < 3; i++)
                  last[i]=point[i];
              }
            (void) ConcatenateString(&path,message);
            in_subpath=True;
            knot_count--;
            /*
              Close the subpath if there are no more knots.
            */
            if (knot_count == 0)
              {
                /*
                  Same special handling as above except we compare to the
                  first point in the path and close the path.
                */
                if ((last[1].x == last[2].x) && (last[1].y == last[2].y) &&
                  (first[0].x == first[1].x) && (first[0].y == first[1].y))
                {
                  FormatString(message,"%.6f %.6f l z\n",first[1].x,first[1].y);
                }
                else if ((last[1].x == last[2].x) && (last[1].y == last[2].y))
                {
                  FormatString(message,"%.6f %.6f %.6f %.6f v z\n",
                    first[0].x,first[0].y,first[1].x,first[1].y);
                }
                else if ((first[0].x == first[1].x) && (first[0].y == first[1].y))
                {
                  FormatString(message,"%.6f %.6f %.6f %.6f y z\n",
                    last[2].x,last[2].y,first[1].x,first[1].y);
                }
                else
                {
                  FormatString(message,"%.6f %.6f %.6f %.6f %.6f %.6f c z\n",
                    last[2].x,last[2].y,first[0].x,first[0].y,first[1].x,
                    first[1].y);
                }
                (void) ConcatenateString(&path,message);
                in_subpath=False;
              }
          }
          break;
      }
      case 6:
      case 7:
      case 8:
      default:
        {
          blob+=24;
          length-=24;
          break;
        }
    }
  }
  /*
    Returns an empty PS path if the path has no knots.
  */
  FormatString(message,"eoclip\n");
  (void) ConcatenateString(&path,message);
  FormatString(message,"} bind def");
  (void) ConcatenateString(&path,message);
  MagickFreeMemory(message);
  return(path);
}

/*
  FIXME: length is defined as type size_t, and then code incorrectly assumes that size_t is
  a signed type.
*/
static char *TraceSVGClippingPath(unsigned char *blob,size_t length,
  unsigned long columns,unsigned long rows)
{
  char
    *path,
    *message;

  int
    knot_count,
    selector;

  long
    x,
    y;

  PointInfo
    first[3],
    last[3],
    point[3];

  register long
    i;

  unsigned int
    in_subpath;

  path=AllocateString((char *) NULL);
  if (path == (char *) NULL)
    return((char *) NULL);
  message=AllocateString((char *) NULL);

  FormatString(message,"<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n");
  (void) ConcatenateString(&path,message);
  FormatString(message,"<svg width=\"%lu\" height=\"%lu\">\n",columns,rows);
  (void) ConcatenateString(&path,message);
  FormatString(message,"<g>\n");
  (void) ConcatenateString(&path,message);
  FormatString(message,"<path style=\"fill:#00000000;stroke:#00000000;");
  (void) ConcatenateString(&path,message);
  FormatString(message,"stroke-width:0;stroke-antialiasing:false\" d=\"\n");
  (void) ConcatenateString(&path,message);

  knot_count=0;
  in_subpath=False;

  /*
    Open and closed subpaths are all closed in the following
    parser loop as there's no way for the polygon renderer
    to render an open path to a masking image.

    The clipping path format is defined in "Adobe Photoshop File
    Formats Specification" version 6.0 downloadable from adobe.com.
  */
  while (length > 0)
  {
    selector=ReadMSBShort(&blob,&length);
    switch (selector)
    {
      case 0:
      case 3:
      {
        if (knot_count == 0)
          {
            /*
              Expected subpath length record
            */
            knot_count=ReadMSBShort(&blob,&length);
            blob+=22;
            length-=22;
          }
        else
          {
            blob+=24;
            length-=24;
          }	  
        break;
      }
      case 1:
      case 2:
      case 4:
      case 5:
      {
        if (knot_count == 0)
          {
            /*
              Unexpected subpath knot
            */
            blob+=24;
            length-=24;
          }
        else
          {
            /*
              Add sub-path knot
            */
            for (i=0; i < 3; i++)
              {
                y=ReadMSBLong(&blob,&length);
                x=ReadMSBLong(&blob,&length);
                point[i].x=(double) x*columns/4096/4096;
                point[i].y=(double) y*rows/4096/4096;
              }
            if (!in_subpath)
              {
                FormatString(message,"M %.6f,%.6f\n",point[1].x,point[1].y);
                for (i=0; i < 3; i++)
                {
                  first[i]=point[i];
                  last[i]=point[i];
                }
              }
            else
              {
                /*
                  Handle special case when Bezier curves are used to describe
                  straight lines.
                */
                if ((last[1].x == last[2].x) && (last[1].y == last[2].y) &&
                  (point[0].x == point[1].x) && (point[0].y == point[1].y))
                {
                  /*
                    First control point equals first anchor point and last control
                    point equals last anchow point. Straigt line between anchor points.
                  */
                  FormatString(message,"L %.6f,%.6f\n",point[1].x,point[1].y);
                }
                else
                {
                  FormatString(message,"C %.6f,%.6f %.6f,%.6f %.6f,%.6f\n",
                    last[2].x,last[2].y,point[0].x,point[0].y,point[1].x,
                    point[1].y);
                }
                for (i=0; i < 3; i++)
                  last[i]=point[i];
              }
            (void) ConcatenateString(&path,message);
            in_subpath=True;
            knot_count--;
            /*
              Close the subpath if there are no more knots.
            */
            if (knot_count == 0)
              {
                /*
                  Same special handling as above except we compare to the
                  first point in the path and close the path.
                */
                if ((last[1].x == last[2].x) && (last[1].y == last[2].y) &&
                  (first[0].x == first[1].x) && (first[0].y == first[1].y))
                {
                  FormatString(message,"L %.6f,%.6f Z\n",first[1].x,first[1].y);
                }
                else
                {
                  FormatString(message,"C %.6f,%.6f %.6f,%.6f %.6f,%.6f Z\n",last[2].x,
                    last[2].y,first[0].x,first[0].y,first[1].x,first[1].y);
                  (void) ConcatenateString(&path,message);
                }
                in_subpath=False;
              }
          }
          break;
      }
      case 6:
      case 7:
      case 8:
      default:
        {
          blob+=24;
          length-=24;
          break;
        }
    }
  }
  /*
    Returns an empty SVG image if the path has no knots.
  */
  FormatString(message,"\"/>\n");
  (void) ConcatenateString(&path,message);
  FormatString(message,"</g>\n");
  (void) ConcatenateString(&path,message);
  FormatString(message,"</svg>\n");
  (void) ConcatenateString(&path,message);
  MagickFreeMemory(message);
  return(path);
}

static int Generate8BIMAttribute(Image *image,const char *key)
{
  char
    *attribute,
    name[MaxTextExtent],
    format[MaxTextExtent],
    *resource;

  int
    id,
    start,
    stop,
    sub_number;

  register long
    i;

  size_t
    length;

  unsigned char
    *info;

  unsigned int
    status;

  unsigned long
    count;

  const unsigned char
    *profile;

  size_t
    profile_length;

  if ((profile=GetImageProfile(image,"IPTC",&profile_length)) == 0)
    return(False);

  /*
    There may be spaces in resource names, but there are no
    newlines, so use a newline as terminater to get the full
    name.
  */
  count=sscanf(key,"8BIM:%d,%d:%[^\n]\n%[^\n]",&start,&stop,name,format);
  if ((count != 2) && (count != 3) && (count != 4))
    return(False);
  if (count < 4)
    (void)strcpy(format,"SVG");
  if (count < 3)
    *name='\0';
  sub_number=1;
  if (*name == '#')
    sub_number=atol(&name[1]);
  sub_number=Max(sub_number,1);
  resource=(char *) NULL;

  status=False;
  length=profile_length;
  /* FIXME: following cast should not be necessary but info can't be
     const due to odd function design. */
  info=(unsigned char *) profile;

  while ((length > 0) && (status == False))
  {
    if (ReadByte(&info,&length) != '8')
      continue;
    if (ReadByte(&info,&length) != 'B')
      continue;
    if (ReadByte(&info,&length) != 'I')
      continue;
    if (ReadByte(&info,&length) != 'M')
      continue;
    id=ReadMSBShort(&info,&length);
    if (id < start)
      continue;
    if (id > stop)
      continue;
    if (resource != (char *)NULL)
        MagickFreeMemory(resource);
    count=ReadByte(&info,&length);
    if ((count != 0) && (count <= length))
      {
        resource=(char *) MagickAllocateMemory(char *,count+MaxTextExtent);
        if (resource != (char *) NULL)
          {
            for (i=0; i < (long) count; i++)
              resource[i]=(char) ReadByte(&info,&length);
            resource[count]='\0';
          }
      }
    if (!(count & 0x01))
      (void) ReadByte(&info,&length);
    count=ReadMSBLong(&info,&length);
    if ((*name != '\0') && (*name != '#'))
      {
        if ((resource == (char *) NULL) || (LocaleCompare(name,resource) != 0))
          {
            /*
              No name match, scroll forward and try next resource.
            */
            info+=count;
            length-=count;
            continue;
          }
      }
    if ((*name == '#') && (sub_number != 1))
      {
        /*
          No numbered match, scroll forward and try next resource.
        */
        sub_number--;
        info+=count;
        length-=count;
        continue;
      }
    /*
      We have the resource of interest.
    */
    attribute=(char *) MagickAllocateMemory(char *,count+MaxTextExtent);
    if (attribute != (char *) NULL)
      {
        (void) memcpy(attribute,(char *) info,count);
        attribute[count]='\0';
        info+=count;
        length-=count;
        if ((id <= 1999) || (id >= 2999))
          {
            (void) SetImageAttribute(image,key,(const char *) attribute);
          }
        else
          {
            char
              *path;
            if (LocaleCompare("SVG",format) == 0)
              path=TraceSVGClippingPath((unsigned char *) attribute,count,
                image->columns,image->rows);
            else
              path=TracePSClippingPath((unsigned char *) attribute,count,
                image->columns,image->rows);
            (void) SetImageAttribute(image,key,(const char *) path);
            MagickFreeMemory(path);
          }
        MagickFreeMemory(attribute);
        status=True;
      }
  }
  if (resource != (char *)NULL)
    MagickFreeMemory(resource);
  return(status);
}

#define DE_STACK_SIZE  16
#define EXIF_DELIMITER  "\n"
#define EXIF_NUM_FORMATS  12
#define EXIF_FMT_BYTE  1
#define EXIF_FMT_STRING  2
#define EXIF_FMT_USHORT  3
#define EXIF_FMT_ULONG  4
#define EXIF_FMT_URATIONAL  5
#define EXIF_FMT_SBYTE  6
#define EXIF_FMT_UNDEFINED  7
#define EXIF_FMT_SSHORT  8
#define EXIF_FMT_SLONG  9
#define EXIF_FMT_SRATIONAL  10
#define EXIF_FMT_SINGLE  11
#define EXIF_FMT_DOUBLE  12
#define TAG_EXIF_OFFSET  0x8769
#define TAG_INTEROP_OFFSET  0xa005

typedef struct _TagInfo
{
  unsigned short
    tag;

  char
    *description;
} TagInfo;

static TagInfo
  tag_table[] =
  {
    {  0x100, (char *) "ImageWidth"},
    {  0x101, (char *) "ImageLength"},
    {  0x102, (char *) "BitsPerSample"},
    {  0x103, (char *) "Compression"},
    {  0x106, (char *) "PhotometricInterpretation"},
    {  0x10A, (char *) "FillOrder"},
    {  0x10D, (char *) "DocumentName"},
    {  0x10E, (char *) "ImageDescription"},
    {  0x10F, (char *) "Make"},
    {  0x110, (char *) "Model"},
    {  0x111, (char *) "StripOffsets"},
    {  0x112, (char *) "Orientation"},
    {  0x115, (char *) "SamplesPerPixel"},
    {  0x116, (char *) "RowsPerStrip"},
    {  0x117, (char *) "StripByteCounts"},
    {  0x118, (char *) "MinSampleValue"},
    {  0x119, (char *) "MaxSampleValue"},
    {  0x11A, (char *) "XResolution"},
    {  0x11B, (char *) "YResolution"},
    {  0x11C, (char *) "PlanarConfiguration"},
    {  0x11D, (char *) "PageName"},
    {  0x11E, (char *) "XPosition"},
    {  0x11F, (char *) "YPosition"},
    {  0x120, (char *) "FreeOffsets"},
    {  0x121, (char *) "FreeByteCounts"},
    {  0x122, (char *) "GrayResponseUnit"},
    {  0x123, (char *) "GrayResponseCurve"},
    {  0x124, (char *) "T4Options"},
    {  0x125, (char *) "T6Options"},
    {  0x128, (char *) "ResolutionUnit"},
    {  0x12D, (char *) "TransferFunction"},
    {  0x131, (char *) "Software"},
    {  0x132, (char *) "DateTime"},
    {  0x13B, (char *) "Artist"},
    {  0x13C, (char *) "HostComputer"},
    {  0x13D, (char *) "Predictor"},
    {  0x13E, (char *) "WhitePoint"},
    {  0x13F, (char *) "PrimaryChromaticities"},
    {  0x140, (char *) "ColorMap"},
    {  0x141, (char *) "HalfToneHints"},
    {  0x142, (char *) "TileWidth"},
    {  0x143, (char *) "TileLength"},
    {  0x144, (char *) "TileOffsets"},
    {  0x145, (char *) "TileByteCounts"},
    {  0x14A, (char *) "SubIFD"},
    {  0x14C, (char *) "InkSet"},
    {  0x14D, (char *) "InkNames"},
    {  0x14E, (char *) "NumberOfInks"},
    {  0x150, (char *) "DotRange"},
    {  0x151, (char *) "TargetPrinter"},
    {  0x152, (char *) "ExtraSample"},
    {  0x153, (char *) "SampleFormat"},
    {  0x154, (char *) "SMinSampleValue"},
    {  0x155, (char *) "SMaxSampleValue"},
    {  0x156, (char *) "TransferRange"},
    {  0x157, (char *) "ClipPath"},
    {  0x158, (char *) "XClipPathUnits"},
    {  0x159, (char *) "YClipPathUnits"},
    {  0x15A, (char *) "Indexed"},
    {  0x15B, (char *) "JPEGTables"},
    {  0x15F, (char *) "OPIProxy"},
    {  0x200, (char *) "JPEGProc"},
    {  0x201, (char *) "JPEGInterchangeFormat"},
    {  0x202, (char *) "JPEGInterchangeFormatLength"},
    {  0x203, (char *) "JPEGRestartInterval"},
    {  0x205, (char *) "JPEGLosslessPredictors"},
    {  0x206, (char *) "JPEGPointTransforms"},
    {  0x207, (char *) "JPEGQTables"},
    {  0x208, (char *) "JPEGDCTables"},
    {  0x209, (char *) "JPEGACTables"},
    {  0x211, (char *) "YCbCrCoefficients"},
    {  0x212, (char *) "YCbCrSubSampling"},
    {  0x213, (char *) "YCbCrPositioning"},
    {  0x214, (char *) "ReferenceBlackWhite"},
    {  0x2BC, (char *) "ExtensibleMetadataPlatform"},
    {  0x301, (char *) "Gamma"},
    {  0x302, (char *) "ICCProfileDescriptor"},
    {  0x303, (char *) "SRGBRenderingIntent"},
    {  0x320, (char *) "ImageTitle"},
    {  0x5001, (char *) "ResolutionXUnit"},
    {  0x5002, (char *) "ResolutionYUnit"},
    {  0x5003, (char *) "ResolutionXLengthUnit"},
    {  0x5004, (char *) "ResolutionYLengthUnit"},
    {  0x5005, (char *) "PrintFlags"},
    {  0x5006, (char *) "PrintFlagsVersion"},
    {  0x5007, (char *) "PrintFlagsCrop"},
    {  0x5008, (char *) "PrintFlagsBleedWidth"},
    {  0x5009, (char *) "PrintFlagsBleedWidthScale"},
    {  0x500A, (char *) "HalftoneLPI"},
    {  0x500B, (char *) "HalftoneLPIUnit"},
    {  0x500C, (char *) "HalftoneDegree"},
    {  0x500D, (char *) "HalftoneShape"},
    {  0x500E, (char *) "HalftoneMisc"},
    {  0x500F, (char *) "HalftoneScreen"},
    {  0x5010, (char *) "JPEGQuality"},
    {  0x5011, (char *) "GridSize"},
    {  0x5012, (char *) "ThumbnailFormat"},
    {  0x5013, (char *) "ThumbnailWidth"},
    {  0x5014, (char *) "ThumbnailHeight"},
    {  0x5015, (char *) "ThumbnailColorDepth"},
    {  0x5016, (char *) "ThumbnailPlanes"},
    {  0x5017, (char *) "ThumbnailRawBytes"},
    {  0x5018, (char *) "ThumbnailSize"},
    {  0x5019, (char *) "ThumbnailCompressedSize"},
    {  0x501A, (char *) "ColorTransferFunction"},
    {  0x501B, (char *) "ThumbnailData"},
    {  0x5020, (char *) "ThumbnailImageWidth"},
    {  0x5021, (char *) "ThumbnailImageHeight"},
    {  0x5022, (char *) "ThumbnailBitsPerSample"},
    {  0x5023, (char *) "ThumbnailCompression"},
    {  0x5024, (char *) "ThumbnailPhotometricInterp"},
    {  0x5025, (char *) "ThumbnailImageDescription"},
    {  0x5026, (char *) "ThumbnailEquipMake"},
    {  0x5027, (char *) "ThumbnailEquipModel"},
    {  0x5028, (char *) "ThumbnailStripOffsets"},
    {  0x5029, (char *) "ThumbnailOrientation"},
    {  0x502A, (char *) "ThumbnailSamplesPerPixel"},
    {  0x502B, (char *) "ThumbnailRowsPerStrip"},
    {  0x502C, (char *) "ThumbnailStripBytesCount"},
    {  0x502D, (char *) "ThumbnailResolutionX"},
    {  0x502E, (char *) "ThumbnailResolutionY"},
    {  0x502F, (char *) "ThumbnailPlanarConfig"},
    {  0x5030, (char *) "ThumbnailResolutionUnit"},
    {  0x5031, (char *) "ThumbnailTransferFunction"},
    {  0x5032, (char *) "ThumbnailSoftwareUsed"},
    {  0x5033, (char *) "ThumbnailDateTime"},
    {  0x5034, (char *) "ThumbnailArtist"},
    {  0x5035, (char *) "ThumbnailWhitePoint"},
    {  0x5036, (char *) "ThumbnailPrimaryChromaticities"},
    {  0x5037, (char *) "ThumbnailYCbCrCoefficients"},
    {  0x5038, (char *) "ThumbnailYCbCrSubsampling"},
    {  0x5039, (char *) "ThumbnailYCbCrPositioning"},
    {  0x503A, (char *) "ThumbnailRefBlackWhite"},
    {  0x503B, (char *) "ThumbnailCopyRight"},
    {  0x5090, (char *) "LuminanceTable"},
    {  0x5091, (char *) "ChrominanceTable"},
    {  0x5100, (char *) "FrameDelay"},
    {  0x5101, (char *) "LoopCount"},
    {  0x5110, (char *) "PixelUnit"},
    {  0x5111, (char *) "PixelPerUnitX"},
    {  0x5112, (char *) "PixelPerUnitY"},
    {  0x5113, (char *) "PaletteHistogram"},
    {  0x1000, (char *) "RelatedImageFileFormat"},
    {  0x800D, (char *) "ImageID"},
    {  0x80E3, (char *) "Matteing"},
    {  0x80E4, (char *) "DataType"},
    {  0x80E5, (char *) "ImageDepth"},
    {  0x80E6, (char *) "TileDepth"},
    {  0x828D, (char *) "CFARepeatPatternDim"},
    {  0x828E, (char *) "CFAPattern"},
    {  0x828F, (char *) "BatteryLevel"},
    {  0x8298, (char *) "Copyright"},
    {  0x829A, (char *) "ExposureTime"},
    {  0x829D, (char *) "FNumber"},
    {  0x83BB, (char *) "IPTC/NAA"},
    {  0x84E3, (char *) "IT8RasterPadding"},
    {  0x84E5, (char *) "IT8ColorTable"},
    {  0x8649, (char *) "ImageResourceInformation"},
    {  0x8769, (char *) "ExifOffset"},
    {  0x8773, (char *) "InterColorProfile"},
    {  0x8822, (char *) "ExposureProgram"},
    {  0x8824, (char *) "SpectralSensitivity"},
    {  0x8825, (char *) "GPSInfo"},
    {  0x8827, (char *) "ISOSpeedRatings"},
    {  0x8828, (char *) "OECF"},
    {  0x9000, (char *) "ExifVersion"},
    {  0x9003, (char *) "DateTimeOriginal"},
    {  0x9004, (char *) "DateTimeDigitized"},
    {  0x9101, (char *) "ComponentsConfiguration"},
    {  0x9102, (char *) "CompressedBitsPerPixel"},
    {  0x9201, (char *) "ShutterSpeedValue"},
    {  0x9202, (char *) "ApertureValue"},
    {  0x9203, (char *) "BrightnessValue"},
    {  0x9204, (char *) "ExposureBiasValue"},
    {  0x9205, (char *) "MaxApertureValue"},
    {  0x9206, (char *) "SubjectDistance"},
    {  0x9207, (char *) "MeteringMode"},
    {  0x9208, (char *) "LightSource"},
    {  0x9209, (char *) "Flash"},
    {  0x920A, (char *) "FocalLength"},
    {  0x927C, (char *) "MakerNote"},
    {  0x9286, (char *) "UserComment"},
    {  0x9290, (char *) "SubSecTime"},
    {  0x9291, (char *) "SubSecTimeOriginal"},
    {  0x9292, (char *) "SubSecTimeDigitized"},
    {  0x9C9B, (char *) "WinXP-Title"},     /* Win XP specific, UTF-16 Unicode */
    {  0x9C9C, (char *) "WinXP-Comments"},  /* Win XP specific, UTF-16 Unicode */
    {  0x9C9D, (char *) "WinXP-Author"},    /* Win XP specific, UTF-16 Unicode */
    {  0x9C9E, (char *) "WinXP-Keywords"},  /* Win XP specific, UTF-16 Unicode */
    {  0x9C9F, (char *) "WinXP-Subject"},   /* Win XP specific, UTF-16 Unicode */
    {  0xA000, (char *) "FlashPixVersion"},
    {  0xA001, (char *) "ColorSpace"},
    {  0xA002, (char *) "ExifImageWidth"},
    {  0xA003, (char *) "ExifImageLength"},
    {  0xA005, (char *) "InteroperabilityOffset"},
    {  0xA20B, (char *) "FlashEnergy"},
    {  0xA20C, (char *) "SpatialFrequencyResponse"},
    {  0xA20D, (char *) "Noise"},
    {  0xA20E, (char *) "FocalPlaneXResolution"},
    {  0xA20F, (char *) "FocalPlaneYResolution"},
    {  0xA210, (char *) "FocalPlaneResolutionUnit"},
    {  0xA211, (char *) "ImageNumber"},
    {  0xA212, (char *) "SecurityClassification"},
    {  0xA213, (char *) "ImageHistory"},
    {  0xA214, (char *) "SubjectLocation"},
    {  0xA215, (char *) "ExposureIndex"},
    {  0xA216, (char *) "TIFF/EPStandardID"},
    {  0xA217, (char *) "SensingMethod"},
    {  0xA300, (char *) "FileSource"},
    {  0xA301, (char *) "SceneType"},
    {  0xA302, (char *) "CFAPattern"},
    {  0xA401, (char *) "CustomRendered"},
    {  0xA402, (char *) "ExposureMode"},
    {  0xA403, (char *) "WhiteBalance"},
    {  0xA404, (char *) "DigitalZoomRatio"},
    {  0xA405, (char *) "FocalLengthIn35mmFilm"},
    {  0xA406, (char *) "SceneCaptureType"},
    {  0xA407, (char *) "GainControl"},
    {  0xA408, (char *) "Contrast"},
    {  0xA409, (char *) "Saturation"},
    {  0xA40A, (char *) "Sharpness"},
    {  0xA40B, (char *) "DeviceSettingDescription"},
    {  0xA40C, (char *) "SubjectDistanceRange"},
    {  0xA420, (char *) "ImageUniqueID"},
    {  0x0000, (char *) NULL}
  };

static int
  format_bytes[] =
  {
    0,
    1, /* BYTE */
    1, /* STRING / ASCII */
    2, /* USHORT */
    4, /* ULONG */
    8, /* URATIONAL */
    1, /* SBYTE */
    1, /* UNDEFINED */
    2, /* SSHORT */
    4, /* SLONG */
    8, /* SRATIONAL */
    4, /* SINGLE / FLOAT */
    8  /* DOUBLE */
  };

static short Read16s(int morder,void *ishort)
{
  short
    value;

  if (morder)
    value=(((unsigned char *) ishort)[0] << 8) | ((unsigned char *) ishort)[1];
  else
    value=(((unsigned char *) ishort)[1] << 8) | ((unsigned char *) ishort)[0];
  return(value);
}

static unsigned short Read16u(int morder,void *ishort)
{
  unsigned short
    value;

  if (morder)
    value=(((unsigned char *) ishort)[0] << 8) | ((unsigned char *) ishort)[1];
  else
    value=(((unsigned char *) ishort)[1] << 8) | ((unsigned char *) ishort)[0];
  return(value);
}

static long Read32s(int morder,void *ilong)
{
  long
    value;

  if (morder)
    value=(((char *) ilong)[0] << 24) | (((unsigned char *) ilong)[1] << 16) |
      (((unsigned char *) ilong)[2] << 8) | (((unsigned char *) ilong)[3]);
  else
    value=(((char *) ilong)[3] << 24) | (((unsigned char *) ilong)[2] << 16) |
      (((unsigned char *) ilong)[1] << 8 ) | (((unsigned char *) ilong)[0]);
  return(value);
}

static unsigned long Read32u(int morder, void *ilong)
{
  return(Read32s(morder,ilong) & 0xffffffffUL);
}

static int GenerateEXIFAttribute(Image *image,const char *specification)
{
  char
    *key,
    *value,
    *final;

  int
    id,
    level,
    morder,
    all;

  long
    tag;

  register long
    i;

  size_t
    length;

  unsigned long
    offset;

  unsigned char
    *tiffp,
    *ifdstack[DE_STACK_SIZE],
    *ifdp,
    *info;

  unsigned int
    de,
    destack[DE_STACK_SIZE],
    nde;

  const unsigned char
    *profile_info;
  
  size_t
    profile_length;

  /*
    Determine if there is any EXIF data available in the image.
  */
  value=(char *) NULL;
  final=AllocateString("");
  profile_info=GetImageProfile(image,"EXIF",&profile_length);
  if (profile_info == 0)
    goto generate_attribute_failure;
  /*
    If EXIF data exists, then try to parse the request for a tag.
  */
  key=(char *) &specification[5];
  if ((key == (char *) NULL) || (*key == '\0'))
    goto generate_attribute_failure;
  while (isspace((int) (*key)))
    key++;
  all=0;
  tag=(-1);
  switch(*key)
  {
    /*
      Caller has asked for all the tags in the EXIF data.
    */
    case '*':
    {
      tag=0;
      all=1; /* return the data in description=value format */
      break;
    }
    case '!':
    {
      tag=0;
      all=2; /* return the data in tageid=value format */
      break;
    }
    /*
      Check for a hex based tag specification first.
    */
    case '#':
    {
      char
        c;

      unsigned long
        n;

      tag=0;
      key++;
      n=strlen(key);
      if (n != 4)
        goto generate_attribute_failure;
      else
        {
          /*
            Parse tag specification as a hex number.
          */
          n/=4;
          do
          {
            for (i=(long) n-1; i >= 0; i--)
            {
              c=(*key++);
              tag<<=4;
              if ((c >= '0') && (c <= '9'))
                tag|=c-'0';
              else
                if ((c >= 'A') && (c <= 'F'))
                  tag|=c-('A'-10);
                else
                  if ((c >= 'a') && (c <= 'f'))
                    tag|=c-('a'-10);
                  else
                    goto generate_attribute_failure;
            }
          } while (*key != '\0');
        }
      break;
    }
    default:
    {
      /*
        Try to match the text with a tag name instead.
      */
      for (i=0; ; i++)
      {
        if (tag_table[i].tag == 0)
          break;
        if (LocaleCompare(tag_table[i].description,key) == 0)
          {
            tag=tag_table[i].tag;
            break;
          }
      }
      break;
    }
  }
  if (tag < 0)
    goto generate_attribute_failure;;
  length=profile_length;
  info=(unsigned char *) profile_info;
  while (length != 0)
  {
    if (ReadByte(&info,&length) != 0x45)
      continue;
    if (ReadByte(&info,&length) != 0x78)
      continue;
    if (ReadByte(&info,&length) != 0x69)
      continue;
    if (ReadByte(&info,&length) != 0x66)
      continue;
    if (ReadByte(&info,&length) != 0x00)
      continue;
    if (ReadByte(&info,&length) != 0x00)
      continue;
    break;
  }
  if (length < 16)
    goto generate_attribute_failure;
  tiffp=info;
  id=Read16u(0,tiffp);
  morder=0;
  if (id == 0x4949) /* LSB */
    morder=0;
  else
    if (id == 0x4D4D) /* MSB */
      morder=1;
    else
      goto generate_attribute_failure;
  if (Read16u(morder,tiffp+2) != 0x002a)
    goto generate_attribute_failure;
  /*
    This is the offset to the first IFD.
  */
  offset=Read32u(morder,tiffp+4);
  if (offset >= length)
    goto generate_attribute_failure;
  /*
    Set the pointer to the first IFD and follow it were it leads.
  */
  ifdp=tiffp+offset;
  level=0;
  de=0;
  do
  {
    /*
      If there is anything on the stack then pop it off.
    */
    if (level > 0)
      {
        level--;
        ifdp=ifdstack[level];
        de=destack[level];
      }
    /*
      Determine how many entries there are in the current IFD.
    */
    nde=Read16u(morder,ifdp);
    for (; de < nde; de++)
    {
      unsigned int
        n;

      int
        t,
        f,
        c;

      char
        *pde,
        *pval;

      pde=(char *) (ifdp+2+(12*de));
      t=Read16u(morder,pde); /* get tag value */
      f=Read16u(morder,pde+2); /* get the format */
      if ((f-1) >= EXIF_NUM_FORMATS)
        break;
      c=(long) Read32u(morder,pde+4); /* get number of components */
      n=c*format_bytes[f];
      if (n <= 4)
        pval=pde+8;
      else
        {
          unsigned long
            oval;

          /*
            The directory entry contains an offset.
          */
          oval=Read32u(morder,pde+8);
          if ((oval+n) > length)
            continue;
          pval=(char *)(tiffp+oval);
        }
      if (all || (tag == t))
        {
          char
            s[MaxTextExtent];

          switch (f)
          {
            case EXIF_FMT_SBYTE:
            {
              /* 8-bit signed integer */
              FormatString(s,"%ld",(long) (*(char *) pval));
              value=AllocateString(s);
              break;
            }
            case EXIF_FMT_BYTE:
            {
              /* 8-bit unsigned integer */
              value=MagickAllocateMemory(char *,n+1);
              if (value != (char *) NULL)
                {
                  unsigned int
                    a;

                  for (a=0; a < n; a++)
                  {
                    value[a]='.';
                    if (isprint((int) pval[a]) /* || (pval[a] == '\0') */)
                      value[a]=pval[a];
                  }
                  value[a]='\0';
                  break;
                }
#if 0
              printf("format %u, length %u\n",f,n);
              FormatString(s,"%ld",(long) (*(unsigned char *) pval));
              value=AllocateString(s);
#endif
              break;
            }
            case EXIF_FMT_SSHORT:
            {
              /* 16-bit signed integer */
              FormatString(s,"%hd",Read16u(morder,pval));
              value=AllocateString(s);
              break;
            }
            case EXIF_FMT_USHORT:
            {
              /* 16-bit unsigned integer */
              FormatString(s,"%hu",Read16s(morder,pval));
              value=AllocateString(s);
              break;
            }
            case EXIF_FMT_ULONG:
            {
              FormatString(s,"%lu",Read32u(morder,pval));
              value=AllocateString(s);
              break;
            }
            case EXIF_FMT_SLONG:
            {
              FormatString(s,"%ld",Read32s(morder,pval));
              value=AllocateString(s);
              break;
            }
            case EXIF_FMT_URATIONAL:
            {
              FormatString(s,"%ld/%ld",Read32u(morder,pval),
                Read32u(morder,4+(char *) pval));
              value=AllocateString(s);
              break;
            }
            case EXIF_FMT_SRATIONAL:
            {
              FormatString(s,"%ld/%ld",Read32s(morder,pval),
                Read32s(morder,4+(char *) pval));
              value=AllocateString(s);
              break;
            }
            case EXIF_FMT_SINGLE:
            {
              FormatString(s,"%f",(double) *(float *) pval);
              value=AllocateString(s);
              break;
            }
            case EXIF_FMT_DOUBLE:
            {
              FormatString(s,"%f",*(double *) pval);
              value=AllocateString(s);
              break;
            }
            default:
            case EXIF_FMT_UNDEFINED:
            case EXIF_FMT_STRING:
            {
              value=MagickAllocateMemory(char *,n+1);
              if (value != (char *) NULL)
                {
                  unsigned int
                    a;

                  for (a=0; a < n; a++)
                  {
                    value[a]='.';
                    if (isprint((int) pval[a]) || (pval[a] == '\0'))
                      value[a]=pval[a];
                  }
                  value[a]='\0';
                  break;
                }
              break;
            }
          }
          if (value != (char *) NULL)
            {
              int
                i;

              char
                *description;

              if (strlen(final) != 0)
                (void) ConcatenateString(&final,EXIF_DELIMITER);
              description=(char *) NULL;
              switch (all)
              {
                case 1:
                {
                  description=(char *) "unknown";
                  for (i=0; ; i++)
                  {
                    if (tag_table[i].tag == 0)
                      break;
                    if (tag_table[i].tag == t)
                      {
                        description=tag_table[i].description;
                        break;
                      }
                  }
                  FormatString(s,"%.1024s=",description);
                  (void) ConcatenateString(&final,s);
                  break;
                }
                case 2:
                {
                  FormatString(s,"#%04x=",t);
                  (void) ConcatenateString(&final,s);
                  break;
                }
              }
              (void) ConcatenateString(&final,value);
              MagickFreeMemory(value);
            }
        }
        if ((t == TAG_EXIF_OFFSET) || (t == TAG_INTEROP_OFFSET))
          {
            size_t
              offset;

            offset=(size_t) Read32u(morder,pval);
            if ((offset < length) && (level < (DE_STACK_SIZE-2)))
              {
                /*
                  Push our current directory state onto the stack.
                */
                ifdstack[level]=ifdp;
                de++; /* bump to the next entry */
                destack[level]=de;
                level++;
                /*
                  Push new state onto of stack to cause a jump.
                */
                ifdstack[level]=tiffp+offset;
                destack[level]=0;
                level++;
              }
            break; /* break out of the for loop */
          }
    }
  } while (level > 0);
  if (strlen(final) == 0)
    (void) ConcatenateString(&final,"unknown");
  (void) SetImageAttribute(image,specification,(const char *) final);
  MagickFreeMemory(final);
  return(True);
 generate_attribute_failure:
  MagickFreeMemory(final);
  return False;
}

/*
  Generate an aggregate attribute result based on a wildcard
  specification like "foo:*".
*/
static int GenerateWildcardAttribute(Image *image,const char *key)
{
  char
    *result=NULL;

  size_t
    key_length=0;

  register ImageAttribute
    *p = (ImageAttribute *) NULL;

  MagickPassFail
    status=MagickFail;

  /*
    Support a full "*" wildcard.
  */
  if (strcmp("*",key) == 0)
    {
      (void) GenerateIPTCAttribute((Image *) image,"IPTC:*");
      (void) Generate8BIMAttribute((Image *) image,"8BIM:*");
      (void) GenerateEXIFAttribute((Image *) image,"EXIF:*");
    }

  key_length=strlen(key)-1;
  for (p=image->attributes; p != (ImageAttribute *) NULL; p=p->next)
    if (LocaleNCompare(key,p->key,key_length) == 0)
      {
        char
          s[MaxTextExtent];

        if (result != NULL)
          (void) ConcatenateString(&result,"\n");
        FormatString(s,"%.512s=%.1024s",p->key,p->value);
        (void) ConcatenateString(&result,s);
      }

  if (result != NULL)
    {
      status=SetImageAttribute(image,key,result);
      MagickFreeMemory(result);
    }
  return status;
}

MagickExport const ImageAttribute *GetImageAttribute(const Image *image,
  const char *key)
{
  register ImageAttribute
    *p = (ImageAttribute *) NULL;

  size_t
    key_length=0;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  /*
    If key is null, then return a pointer to the attribute list.
  */
  if (key == (char *) NULL)
    return(image->attributes);

  key_length=strlen(key);


  for (p=image->attributes; p != (ImageAttribute *) NULL; p=p->next)
    if (LocaleCompare(key,p->key) == 0)
      return(p);

  if (LocaleNCompare("IPTC:",key,5) == 0)
    {
      /*
        Create an attribute named "IPTC:*" with all matching
        key=values and return it.
      */
      if (GenerateIPTCAttribute((Image *) image,key) == True)
        return(GetImageAttribute(image,key));
    }
  else if (LocaleNCompare("8BIM:",key,5) == 0)
    {
      /*
        Create an attribute named "8BIM:*" with all matching
        key=values and return it.
      */
      if (Generate8BIMAttribute((Image *) image,key) == True)
        return(GetImageAttribute(image,key));
    }
  else if (LocaleNCompare("EXIF:",key,5) == 0)
    {
      /*
        Create an attribute named "EXIF:*" with all matching
        key=values and return it.
      */
      if (GenerateEXIFAttribute((Image *) image,key) == True)
        return(GetImageAttribute(image,key));
    }
  else if ((key_length >=2) && (key[key_length-1] == '*'))
    {
      /*
        Create an attribute named "foo:*" with all matching
        key=values and return it.
      */
      if (GenerateWildcardAttribute((Image *) image,key) == True)
        return(GetImageAttribute(image,key));
    }
  else if ((key_length ==1) && (key[0] == '*'))
    {
      /*
        Create an attribute named "*" with all key=values and return
        it.
      */
      if (GenerateWildcardAttribute((Image *) image,key) == True)
        return(GetImageAttribute(image,key));
    }
  return(p);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e C l i p p i n g P a t h A t t r i b u t e                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetImageClippingPathAttribute searches the list of image attributes
%  and returns a pointer to a clipping path if it exists otherwise NULL.
%
%  The format of the GetImageClippingPathAttribute method is:
%
%      const ImageAttribute *GetImageClippingPathAttribute(const Image *image)
%
%  A description of each parameter follows:
%
%    o attribute:  Method GetImageClippingPathAttribute returns the clipping
%      path if it exists otherwise NULL.
%
%    o image: The image.
%
%
*/
MagickExport const ImageAttribute *GetImageClippingPathAttribute(
  const Image *image)
{
  return(GetImageAttribute(image,"8BIM:1999,2998"));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t I m a g e I n f o A t t r i b u t e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageInfoAttribute() returns a "fake" attribute based on data in the
%  image info or image structures.
%
%  The format of the GetImageInfoAttribute method is:
%
%      const ImageAttribute *GetImageAttribute(const ImageInfo *image_info,
%        const Image *image,const char *key)
%
%  A description of each parameter follows:
%
%    o attribute:  Method GetImageInfoAttribute returns the attribute if it
%      exists otherwise NULL.
%
%    o image_info: The imageInfo.
%
%    o image: The image.
%
%    o key:  These character strings are the name of an image attribute to
%      return.
%
*/
MagickExport const ImageAttribute *GetImageInfoAttribute(
  const ImageInfo *image_info,const Image *image,const char *key)
{
  char
    attribute[MaxTextExtent],
    filename[MaxTextExtent];

  attribute[0]='\0';
  switch(*(key))
  {
    case 'b':
    {
      if (LocaleNCompare("base",key,2) == 0)
        {
          GetPathComponent(image->magick_filename,BasePath,filename);
          (void) strlcpy(attribute,filename,MaxTextExtent);
          break;
        }
      break;
    }
    case 'd':
    {
      if (LocaleNCompare("depth",key,2) == 0)
        {
          FormatString(attribute,"%u",image->depth);
          break;
        }
      if (LocaleNCompare("directory",key,2) == 0)
        {
          GetPathComponent(image->magick_filename,HeadPath,filename);
          (void) strlcpy(attribute,filename,MaxTextExtent);
          break;
        }
      break;
    }
    case 'e':
    {
      if (LocaleNCompare("extension",key,2) == 0)
        {
          GetPathComponent(image->magick_filename,ExtensionPath,filename);
          (void) strlcpy(attribute,filename,MaxTextExtent);
          break;
        }
      break;
    }
    case 'g':
    {
      if (LocaleNCompare("group",key,2) == 0)
        {
          FormatString(attribute,"0x%lx",image_info->group);
          break;
        }
      break;
    }
    case 'h':
    {
      if (LocaleNCompare("height",key,2) == 0)
        {
          FormatString(attribute,"%lu",
            image->magick_rows ? image->magick_rows : 256L);
          break;
        }
      break;
    }
    case 'i':
    {
      if (LocaleNCompare("input",key,2) == 0)
        {
          (void) strlcpy(attribute,image->filename,MaxTextExtent);
          break;
        }
      break;
    }
    case 'm':
    {
      if (LocaleNCompare("magick",key,2) == 0)
        {
          (void) strlcpy(attribute,image->magick,MaxTextExtent);
          break;
        }
      break;
    }
    case 'n':
    {
      if (LocaleNCompare("name",key,2) == 0)
        {
          (void) strlcpy(attribute,filename,MaxTextExtent);
          break;
        }
     break;
    }
    case 's':
    {
      if (LocaleNCompare("size",key,2) == 0)
        {
          char
            format[MaxTextExtent];

          FormatSize(GetBlobSize(image),format);
          FormatString(attribute,"%.1024s",format);
          break;
        }
      if (LocaleNCompare("scene",key,2) == 0)
        {
          FormatString(attribute,"%lu",image->scene);
          if (image_info->subrange != 0)
            FormatString(attribute,"%lu",image_info->subimage);
          break;
        }
      if (LocaleNCompare("scenes",key,6) == 0)
        {
          FormatString(attribute,"%lu",(unsigned long) GetImageListLength(image));
          break;
        }
       break;
    }
    case 'o':
    {
      if (LocaleNCompare("output",key,2) == 0)
        {
          (void) strlcpy(attribute,image_info->filename,MaxTextExtent);
          break;
        }
     break;
    }
    case 'p':
    {
      if (LocaleNCompare("page",key,2) == 0)
        {
          register const Image
            *p;

          unsigned int
            page;

          p=image;
          for (page=1; p->previous != (Image *) NULL; page++)
            p=p->previous;
          FormatString(attribute,"%u",page);
          break;
        }
      break;
    }
    case 'u':
    {
      if (LocaleNCompare("unique",key,2) == 0)
        {
          (void) strlcpy(filename,image_info->unique,MaxTextExtent);
          if (*filename == '\0')
            if(!AcquireTemporaryFileName(filename))
              return((ImageAttribute *) NULL);
          (void) strlcpy(attribute,filename,MaxTextExtent);
          break;
        }
      break;
    }
    case 'w':
    {
      if (LocaleNCompare("width",key,2) == 0)
        {
          FormatString(attribute,"%lu",
            image->magick_columns ? image->magick_columns : 256L);
          break;
        }
      break;
    }
    case 'x':
    {
      if (LocaleNCompare("xresolution",key,2) == 0)
        {
          FormatString(attribute,"%g",image->x_resolution);
          break;
        }
      break;
    }
    case 'y':
    {
      if (LocaleNCompare("yresolution",key,2) == 0)
        {
          FormatString(attribute,"%g",image->y_resolution);
          break;
        }
      break;
    }
    case 'z':
    {
      if (LocaleNCompare("zero",key,2) == 0)
        {
          (void) strlcpy(filename,image_info->zero,MaxTextExtent);
          if (*filename == '\0')
            if(!AcquireTemporaryFileName(filename))
              return((ImageAttribute *) NULL);
          (void) strlcpy(attribute,filename,MaxTextExtent);
          break;
        }
      break;
    }
  }
  if (strlen(image->magick_filename) != 0)
    return(GetImageAttribute(image,key));
  return((ImageAttribute *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t I m a g e A t t r i b u t e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetImageAttribute() searches the list of image attributes and replaces the
%  attribute value.  If it is not found in the list, the attribute name
%  and value is added to the list.   If the attribute exists in the list,
%  the value is concatenated to the attribute.  SetImageAttribute returns
%  True if the attribute is successfully concatenated or added to the list,
%  otherwise False.  If the value is NULL, the matching key is deleted
%  from the list.
%
%  The format of the SetImageAttribute method is:
%
%      unsigned int SetImageAttribute(Image *image,const char *key,
%        const char *value)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o key,value:  These character strings are the name and value of an image
%      attribute to replace or add to the list.
%
%
*/
MagickExport MagickPassFail SetImageAttribute(Image *image,const char *key,
  const char *value)
{
  ImageAttribute
    *attribute;

  register ImageAttribute
    *p;

  /*
    Initialize new attribute.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if ((key == (const char *) NULL) || (*key == '\0'))
    return(MagickFail);

  if (value == (const char *) NULL)
    {
      /*
        Delete attribute from the image attributes list.
      */
      for (p=image->attributes; p != (ImageAttribute *) NULL; p=p->next)
        if (LocaleCompare(key,p->key) == 0)
          break;
      if (p == (ImageAttribute *) NULL)
        return(False);
      if (p->previous != (ImageAttribute *) NULL)
        p->previous->next=p->next;
      else
        {
          image->attributes=p->next;
          if (p->next != (ImageAttribute *) NULL)
            p->next->previous=(ImageAttribute *) NULL;
        }
      if (p->next != (ImageAttribute *) NULL)
        p->next->previous=p->previous;
      attribute=p;
      DestroyImageAttribute(attribute);
      return(MagickPass);
    }
  if (*value == '\0')
    return(MagickFail);
  attribute=MagickAllocateMemory(ImageAttribute *,sizeof(ImageAttribute));
  if (attribute == (ImageAttribute *) NULL)
    return(MagickFail);
  attribute->key=AllocateString(key);
  attribute->length=0;
  if ((LocaleNCompare(key,"comment",7) == 0) ||
      (LocaleNCompare(key,"label",5) == 0))
    {
      /*
        Translate format requests in attribute text.

        This is really gross since it is assumed that the attribute is
        supplied by the user and the user intends for translation to
        occur.  However, comment attributes may also come from an
        image file and may contain arbitrary text.  There does not
        seem to be any work-around which preserves the already defined
        interface.
      */
      attribute->value=TranslateText((ImageInfo *) NULL,image,value);
      if (attribute->value != (char *) NULL)
        attribute->length=strlen(attribute->value);
    }
  else
    {
      /*
        Use attribute text as is.
      */
      attribute->length=strlen(value);
      attribute->value=MagickAllocateMemory(char *,attribute->length+1);
      if (attribute->value != (char *) NULL)
        (void) strlcpy(attribute->value,value,attribute->length+1);
    }
  if ((attribute->value == (char *) NULL) ||
      (attribute->key == (char *) NULL))
    {
      DestroyImageAttribute(attribute);
      return(MagickFail);
    }

  attribute->previous=(ImageAttribute *) NULL;
  attribute->next=(ImageAttribute *) NULL;
  if (image->attributes == (ImageAttribute *) NULL)
    {
      image->attributes=attribute;
      return(MagickPass);
    }
  for (p=image->attributes; p != (ImageAttribute *) NULL; p=p->next)
  {
    if (LocaleCompare(attribute->key,p->key) == 0)
      {
        size_t
          min_l,
          realloc_l;

        /*
          Extend existing text string.
        */
        min_l=p->length+attribute->length+1;
        for (realloc_l=2; realloc_l <= min_l; realloc_l *= 2);
        MagickReallocMemory(char *,p->value,realloc_l);
        if (p->value != (char *) NULL)
          (void) strcat(p->value+p->length,attribute->value);
        p->length += attribute->length;
        DestroyImageAttribute(attribute);
        if (p->value != (char *) NULL)
          return(MagickPass);
        (void) SetImageAttribute(image,key,NULL);
        return(MagickFail);
      }
    if (p->next == (ImageAttribute *) NULL)
      break;
  }
  /*
    Place new attribute at the end of the attribute list.
  */
  attribute->previous=p;
  p->next=attribute;
  return(MagickPass);
}
