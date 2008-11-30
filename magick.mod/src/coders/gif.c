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
%                             GGGG  IIIII  FFFFF                              %
%                            G        I    F                                  %
%                            G  GG    I    FFF                                %
%                            G   G    I    F                                  %
%                             GGG   IIIII  F                                  %
%                                                                             %
%                                                                             %
%            Read/Write Compuserv Graphics Interchange Format.                %
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
#include "magick/attribute.h"
#include "magick/blob.h"
#include "magick/pixel_cache.h"
#include "magick/color.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/quantize.h"
#include "magick/utility.h"

/*
  Forward declarations.
*/
static size_t
  ReadBlobBlock(Image *,unsigned char *);

static unsigned int
  WriteGIFImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e c o d e I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DecodeImage uncompresses an image via GIF-coding.
%
%  The format of the DecodeImage method is:
%
%      MagickPassFail DecodeImage(Image *image,const long opacity)
%
%  A description of each parameter follows:
%
%    o status:  Method DecodeImage returns MagickPass if all the pixels are
%      uncompressed without error, otherwise MagickFail.
%
%    o image: The address of a structure of type Image.
%
%    o opacity:  The colormap index associated with the transparent
%      color.
%
%
*/
#define MaxStackSize  4096
#define NullCode  (-1)
static MagickPassFail DecodeImage(Image *image,const long opacity)
{
  int
    bits,
    code_size,
    offset,
    pass;

  long
    available,
    clear,
    code,
    code_mask,
    end_of_information,
    in_code,
    old_code,
    y;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *q;

  register unsigned char
    *c;

  register unsigned long
    datum;

  size_t
    count;

  short
    *prefix;

  unsigned char
    data_size,
    first,
    index,
    *packet,
    *pixel_stack,
    *suffix,
    *top_stack;

  MagickPassFail
    status=MagickPass;

  assert(image != (Image *) NULL);

  data_size=ReadBlobByte(image);
  if (data_size > 8U)
    ThrowBinaryException(CorruptImageError,CorruptImage,image->filename);
  /*
    Allocate decoder tables.
  */
  packet=MagickAllocateMemory(unsigned char *,256);
  prefix=MagickAllocateMemory(short *,MaxStackSize*sizeof(short));
  suffix=MagickAllocateMemory(unsigned char *,MaxStackSize);
  pixel_stack=MagickAllocateMemory(unsigned char *,MaxStackSize+1);
  if ((packet == (unsigned char *) NULL) ||
      (prefix == (short *) NULL) ||
      (suffix == (unsigned char *) NULL) ||
      (pixel_stack == (unsigned char *) NULL))
    {
      MagickFreeMemory(pixel_stack);
      MagickFreeMemory(suffix);
      MagickFreeMemory(prefix);
      MagickFreeMemory(packet);
      ThrowBinaryException(ResourceLimitError,MemoryAllocationFailed,
                           image->filename);
    }
  /*
    Initialize GIF data stream decoder.
  */
  clear=1 << data_size;
  end_of_information=clear+1;
  available=clear+2;
  old_code=NullCode;
  code_size=data_size+1;
  code_mask=(1 << code_size)-1;
  for (code=0; code < clear; code++)
  {
    prefix[code]=0;
    suffix[code]=(unsigned char) code;
  }
  /*
    Decode GIF pixel stream.
  */
  datum=0;
  bits=0;
  c=0;
  count=0;
  first=0;
  offset=0;
  pass=0;
  top_stack=pixel_stack;
  for (y=0; y < (long) image->rows; y++)
  {
    q=SetImagePixels(image,0,offset,image->columns,1);
    if (q == (PixelPacket *) NULL)
      {
        status=MagickFail;
        break;
      }
    indexes=AccessMutableIndexes(image);
    for (x=0; x < (long) image->columns; )
    {
      if (top_stack == pixel_stack)
        {
          if (bits < code_size)
            {
              /*
                Load bytes until there is enough bits for a code.
              */
              if (count == 0)
                {
                  /*
                    Read a new data block.
                  */
                  count=ReadBlobBlock(image,packet);
                  if (count == 0)
                    break;
                  c=packet;
                }
              datum+=(unsigned long) (*c) << bits;
              bits+=8;
              c++;
              count--;
              continue;
            }
          /*
            Get the next code.
          */
          code=(long) (datum & code_mask);
          datum>>=code_size;
          bits-=code_size;
          /*
            Interpret the code
          */
          if (code > available)
            {
              status=MagickFail;
              break;
            }
          if (code == end_of_information)
            break;
          if (code == clear)
            {
              /*
                Reset decoder.
              */
              code_size=data_size+1;
              code_mask=(1 << code_size)-1;
              available=clear+2;
              old_code=NullCode;
              continue;
            }
          if (old_code == NullCode)
            {
              *top_stack++=suffix[code];
              old_code=code;
              first=(unsigned char) code;
              continue;
            }
          in_code=code;
          if (code >= available)
            {
              *top_stack++=first;
              code=old_code;
            }
          while (code >= clear)
          {
            if ((top_stack-pixel_stack) >= MaxStackSize)
              {
                status=MagickFail;
                break;
              }
            *top_stack++=suffix[code];
            code=prefix[code];
          }
          if (status == MagickFail)
            break;
          first=suffix[code];
          /*
            Add a new string to the string table,
          */
          if (available >= MaxStackSize)
            {
              status=MagickFail;
              break;
            }
          *top_stack++=first;
          prefix[available]=(short) old_code;
          suffix[available]=first;
          available++;
          if (((available & code_mask) == 0) && (available < MaxStackSize))
            {
              code_size++;
              code_mask+=available;
            }
          old_code=in_code;
        }
      /*
        Pop a pixel off the pixel stack.
      */
      top_stack--;
      index=(*top_stack);
      VerifyColormapIndex(image,index);
      indexes[x]=index;
      *q=image->colormap[index];
      q->opacity=(Quantum)
        (index == opacity ? TransparentOpacity : OpaqueOpacity);
      x++;
      q++;
    }
    if (image->interlace == NoInterlace)
      offset++;
    else
      switch (pass)
      {
        case 0:
        default:
        {
          offset+=8;
          if (offset >= (long) image->rows)
            {
              pass++;
              offset=4;
            }
          break;
        }
        case 1:
        {
          offset+=8;
          if (offset >= (long) image->rows)
            {
              pass++;
              offset=2;
            }
          break;
        }
        case 2:
        {
          offset+=4;
          if (offset >= (long) image->rows)
            {
              pass++;
              offset=1;
            }
          break;
        }
        case 3:
        {
          offset+=2;
          break;
        }
      }
    if (!SyncImagePixels(image))
      {
        status=MagickFail;
        break;
      }
    if (x < (long) image->columns)
      {
        status=MagickFail;
        break;
      }
    if (image->previous == (Image *) NULL)
      if (QuantumTick(y,image->rows))
        if (!MagickMonitorFormatted(y,image->rows,&image->exception,
                                    LoadImageText,image->filename))
          {
            status=MagickFail;
            break;
          }
  }
  MagickFreeMemory(pixel_stack);
  MagickFreeMemory(suffix);
  MagickFreeMemory(prefix);
  MagickFreeMemory(packet);
  if ((status == MagickFail) || (y < (long) image->rows))
    {
      if (image->exception.severity < ErrorException)
        ThrowException(&image->exception,CorruptImageError,CorruptImage,image->filename);
      return MagickFail;
    }
  return(MagickPass);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   E n c o d e I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method EncodeImage compresses an image via GIF-coding.
%
%  The format of the EncodeImage method is:
%
%      MagickPassFail EncodeImage(const ImageInfo *image_info,Image *image,
%        const unsigned int data_size)
%
%  A description of each parameter follows:
%
%    o status:  Method EncodeImage returns MagickPass if all the pixels are
%      compressed without error, otherwise MagickFail.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image: The address of a structure of type Image.
%
%    o data_size:  The number of bits in the compressed packet.
%
%
*/
#define MaxCode(number_bits)  ((1 << (number_bits))-1)
#define MaxHashTable  5003
#define MaxGIFBits  12
#define MaxGIFTable  (1 << MaxGIFBits)

#define GIFOutputCode(code) \
{ \
  /*  \
    Emit a code. \
  */ \
  if (bits > 0) \
    datum|=((long) code << bits); \
  else \
    datum=(long) code; \
  bits+=number_bits; \
  while (bits >= 8) \
  { \
    /*  \
      Add a character to current packet. \
    */ \
    packet[byte_count++]=(unsigned char) (datum & 0xff); \
    if (byte_count >= 254) \
      { \
        (void) WriteBlobByte(image,byte_count); \
        (void) WriteBlob(image,byte_count,(char *) packet); \
        byte_count=0; \
      } \
    datum>>=8; \
    bits-=8; \
  } \
  if (free_code > max_code)  \
    { \
      number_bits++; \
      if (number_bits == MaxGIFBits) \
        max_code=MaxGIFTable; \
      else \
        max_code=MaxCode(number_bits); \
    } \
}
static MagickPassFail EncodeImage(const ImageInfo *image_info,Image *image,
  const unsigned int data_size)
{

  int
    displacement,
    next_pixel,
    bits,
    byte_count,
    k,
    number_bits,
    offset,
    pass;

  long
    datum,
    y;

  register const PixelPacket
    *p;

  register const IndexPacket
    *indexes;

  register long
    i,
    x;

  short
    clear_code,
    end_of_information_code,
    free_code,
    *hash_code,
    *hash_prefix,
    index,
    max_code,
    waiting_code;

  unsigned char
    *packet,
    *hash_suffix;

  /*
    Allocate encoder tables.
  */
  assert(image != (Image *) NULL);
  packet=MagickAllocateMemory(unsigned char *,256);
  hash_code=MagickAllocateMemory(short *,MaxHashTable*sizeof(short));
  hash_prefix=MagickAllocateMemory(short *,MaxHashTable*sizeof(short));
  hash_suffix=MagickAllocateMemory(unsigned char *,MaxHashTable);
  if ((packet == (unsigned char *) NULL) || (hash_code == (short *) NULL) ||
      (hash_prefix == (short *) NULL) ||
      (hash_suffix == (unsigned char *) NULL))
    {
      MagickFreeMemory(packet);
      MagickFreeMemory(hash_code);
      MagickFreeMemory(hash_prefix);
      MagickFreeMemory(hash_suffix);
      return(MagickFail);
    }
  /*
    Initialize GIF encoder.
  */
  number_bits=data_size;
  max_code=MaxCode(number_bits);
  clear_code=((short) 1 << (data_size-1));
  end_of_information_code=clear_code+1;
  free_code=clear_code+2;
  byte_count=0;
  datum=0;
  bits=0;
  for (i=0; i < MaxHashTable; i++)
    hash_code[i]=0;
  GIFOutputCode(clear_code);
  /*
    Encode pixels.
  */
  offset=0;
  pass=0;
  waiting_code=0;
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,offset,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    indexes=AccessImmutableIndexes(image);
    if (y == 0)
      waiting_code=(*indexes);
    for (x=(y == 0) ? 1 : 0; x < (long) image->columns; x++)
    {
      /*
        Probe hash table.
      */
      index=indexes[x] & 0xff;
      p++;
      k=(int) ((int) index << (MaxGIFBits-8))+waiting_code;
      if (k >= MaxHashTable)
        k-=MaxHashTable;
      next_pixel=False;
      displacement=1;
      if ((image_info->compression != NoCompression) && (hash_code[k] > 0))
        {
          if ((hash_prefix[k] == waiting_code) && (hash_suffix[k] == index))
            {
              waiting_code=hash_code[k];
              continue;
            }
          if (k != 0)
            displacement=MaxHashTable-k;
          for ( ; ; )
          {
            k-=displacement;
            if (k < 0)
              k+=MaxHashTable;
            if (hash_code[k] == 0)
              break;
            if ((hash_prefix[k] == waiting_code) && (hash_suffix[k] == index))
              {
                waiting_code=hash_code[k];
                next_pixel=True;
                break;
              }
          }
          if (next_pixel == True)
            continue;
        }
      GIFOutputCode(waiting_code);
      if (free_code < MaxGIFTable)
        {
          hash_code[k]=free_code++;
          hash_prefix[k]=waiting_code;
          hash_suffix[k]=(unsigned char) index;
        }
      else
        {
          /*
            Fill the hash table with empty entries.
          */
          for (k=0; k < MaxHashTable; k++)
            hash_code[k]=0;
          /*
            Reset compressor and issue a clear code.
          */
          free_code=clear_code+2;
          GIFOutputCode(clear_code);
          number_bits=data_size;
          max_code=MaxCode(number_bits);
        }
      waiting_code=index;
    }
    if (image_info->interlace == NoInterlace)
      offset++;
    else
      switch (pass)
      {
        case 0:
        default:
        {
          offset+=8;
          if (offset >= (long) image->rows)
            {
              pass++;
              offset=4;
            }
          break;
        }
        case 1:
        {
          offset+=8;
          if (offset >= (long) image->rows)
            {
              pass++;
              offset=2;
            }
          break;
        }
        case 2:
        {
          offset+=4;
          if (offset >= (long) image->rows)
            {
              pass++;
              offset=1;
            }
          break;
        }
        case 3:
        {
          offset+=2;
          break;
        }
      }
    if (image->previous == (Image *) NULL)
      if (QuantumTick(y,image->rows))
        if (!MagickMonitorFormatted(y,image->rows,&image->exception,
                                    SaveImageText,image->filename))
          break;
  }
  /*
    Flush out the buffered code.
  */
  GIFOutputCode(waiting_code);
  GIFOutputCode(end_of_information_code);
  if (bits > 0)
    {
      /*
        Add a character to current packet.
      */
      packet[byte_count++]=(unsigned char) (datum & 0xff);
      if (byte_count >= 254)
        {
          (void) WriteBlobByte(image,byte_count);
          (void) WriteBlob(image,byte_count,(char *) packet);
          byte_count=0;
        }
    }
  /*
    Flush accumulated data.
  */
  if (byte_count > 0)
    {
      (void) WriteBlobByte(image,byte_count);
      (void) WriteBlob(image,byte_count,(char *) packet);
    }
  /*
    Free encoder memory.
  */
  MagickFreeMemory(hash_suffix);
  MagickFreeMemory(hash_prefix);
  MagickFreeMemory(hash_code);
  MagickFreeMemory(packet);
  return(MagickPass);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s G I F                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsGIF returns MagickTrue if the image format type, identified by
%  the magick string, is GIF.
%
%  The format of the IsGIF method is:
%
%      MagickBool IsGIF(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsGIF returns True if the image format type is GIF.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static MagickBool IsGIF(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(MagickFalse);
  if (LocaleNCompare((char *) magick,"GIF8",4) == 0)
    return(MagickTrue);
  return(MagickFalse);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  R e a d B l o b B l o c k                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadBlobBlock reads data from the image file and returns it.  The
%  amount of data is determined by first reading a count byte.  The number
%  or bytes read is returned.
%
%  The format of the ReadBlobBlock method is:
%
%      size_t ReadBlobBlock(Image *image,unsigned char *data)
%
%  A description of each parameter follows:
%
%    o count:  Method ReadBlobBlock returns the number of bytes read.
%
%    o image: The image.
%
%    o data:  Specifies an area to place the information requested from
%      the file.
%
%
*/
static size_t ReadBlobBlock(Image *image,unsigned char *data)
{
  size_t
    count;

  unsigned char
    block_count;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(data != (unsigned char *) NULL);
  count=ReadBlob(image,1,&block_count);
  if (count == 0)
    return(0);
  return(ReadBlob(image,(size_t) block_count,data));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d G I F I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadGIFImage reads a Compuserve Graphics image file and returns it.
%  It allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadGIFImage method is:
%
%      Image *ReadGIFImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadGIFImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      an error occurs.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
#define BitSet(byte,bit)  (((byte) & (bit)) == (bit))
#define LSBFirstOrder(x,y)  (((y) << 8) | (x))
static Image *ReadGIFImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image
    *image;

  int
    status;

  long
    opacity;

  RectangleInfo
    page;

  register long
    i;

  register unsigned char
    *p;

  size_t
    count;

  unsigned char
    background,
    c,
    flag,
    *global_colormap,
    header[MaxTextExtent],
    magick[12];

  unsigned long
    delay,
    dispose,
    global_colors,
    image_count,
    iterations;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFail)
    ThrowReaderException(FileOpenError,UnableToOpenFile,image);
  /*
    Determine if this is a GIF file.
  */
  count=ReadBlob(image,6,(char *) magick);
  if ((count == 0) || ((LocaleNCompare((char *) magick,"GIF87",5) != 0) &&
      (LocaleNCompare((char *) magick,"GIF89",5) != 0)))
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
  global_colors=0;
  global_colormap=(unsigned char *) NULL;
  page.width=ReadBlobLSBShort(image);
  page.height=ReadBlobLSBShort(image);
  flag=ReadBlobByte(image);
  background=ReadBlobByte(image);
  c=ReadBlobByte(image);  /* reserved */
  global_colors=1 << ((flag & 0x07)+1);
  global_colormap=MagickAllocateMemory(unsigned char *,3*Max(global_colors,256));
  if (global_colormap == (unsigned char *) NULL)
    ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
  if (BitSet(flag,0x80))
    (void) ReadBlob(image,3*global_colors,(char *) global_colormap);
  delay=0;
  dispose=0;
  iterations=1;
  opacity=(-1);
  image_count=0;
  for ( ; ; )
  {
    count=ReadBlob(image,1,(char *) &c);
    if (count == 0)
      break;
    if (c == ';')
      break;  /* terminator */
    if (c == '!')
      {
        /*
          GIF Extension block.
        */
        count=ReadBlob(image,1,(char *) &c);
        if (count == 0) {
	  MagickFreeMemory(global_colormap);
          ThrowReaderException(CorruptImageError,UnableToReadExtensionBlock,	 
            image);
	}
        switch (c)
        {
          case 0xf9:
          {
            /*
              Read Graphics Control extension.
            */
            while (ReadBlobBlock(image,header) != 0);
            dispose=header[0] >> 2;
            delay=(header[2] << 8) | header[1];
            if ((header[0] & 0x01) == 1)
              opacity=header[3];
            break;
          }
          case 0xfe:
          {
            char
              *comments;

            /*
              Read Comment extension.
            */
            comments=AllocateString((char *) NULL);
            for ( ; ; )
            {
              count=ReadBlobBlock(image,header);
              if (count == 0)
                break;
              header[count]='\0';
              (void) ConcatenateString(&comments,(const char *) header);
            }
            (void) SetImageAttribute(image,"comment",comments);
            MagickFreeMemory(comments);
            break;
          }
          case 0xff:
          {
            int
              loop;

            /*
              Read Netscape Loop extension.
            */
            loop=False;
            if (ReadBlobBlock(image,header) != 0)
              loop=!LocaleNCompare((char *) header,"NETSCAPE2.0",11);
            while (ReadBlobBlock(image,header) != 0)
            if (loop)
              iterations=(header[2] << 8) | header[1];
            break;
          }
          default:
          {
            while (ReadBlobBlock(image,header) != 0);
            break;
          }
        }
      }
    if (c != ',')
      continue;
    if (image_count != 0)
      {
        /*
          Allocate next image structure.
        */
        AllocateNextImage(image_info,image);
        if (image->next == (Image *) NULL)
          {
            DestroyImageList(image);
            MagickFreeMemory(global_colormap);
            return((Image *) NULL);
          }
        image=SyncNextImageInList(image);
        if (!MagickMonitorFormatted(TellBlob(image),GetBlobSize(image),exception,
                                    LoadImagesText,image->filename))
          break;
      }
    image_count++;
    /*
      Read image attributes.
    */
    image->storage_class=PseudoClass;
    image->compression=LZWCompression;
    page.x=ReadBlobLSBShort(image);
    page.y=ReadBlobLSBShort(image);
    image->columns=ReadBlobLSBShort(image);
    image->rows=ReadBlobLSBShort(image);
    image->depth=8;
    flag=ReadBlobByte(image);
    image->interlace=BitSet(flag,0x40) ? PlaneInterlace : NoInterlace;
    image->colors=!BitSet(flag,0x80) ? global_colors : 0x01U << ((flag & 0x07)+1);
    if (opacity >= (long) image->colors)
      image->colors=opacity+1;
    image->page.width=page.width;
    image->page.height=page.height;
    image->page.y=page.y;
    image->page.x=page.x;
    image->delay=delay;
    image->dispose=(DisposeType) dispose;
    image->iterations=iterations;
    image->matte=opacity >= 0;
    delay=0;
    dispose=0;
    iterations=1;
    if ((image->columns == 0) || (image->rows == 0)) {
      MagickFreeMemory(global_colormap);    
      ThrowReaderException(CorruptImageError,NegativeOrZeroImageSize,image);
    }
    /*
      Inititialize colormap.
    */
    if (!AllocateImageColormap(image,image->colors)) {
      MagickFreeMemory(global_colormap);    
      ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
    }
    if (!BitSet(flag,0x80))
      {
        /*
          Use global colormap.
        */
        p=global_colormap;
        for (i=0; i < (long) image->colors; i++)
        {
          image->colormap[i].red=ScaleCharToQuantum(*p++);
          image->colormap[i].green=ScaleCharToQuantum(*p++);
          image->colormap[i].blue=ScaleCharToQuantum(*p++);
        }
        image->background_color=
          image->colormap[Min(background,image->colors-1)];
      }
    else
      {
        unsigned char
          *colormap;

        /*
          Read local colormap.
        */
        colormap=MagickAllocateMemory(unsigned char *,3*image->colors);
        if (colormap == (unsigned char *) NULL)
          {
            MagickFreeMemory(global_colormap);
            ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
                                 image);
          }
        (void) ReadBlob(image,3*image->colors,(char *) colormap);
        p=colormap;
        for (i=0; i < (long) image->colors; i++)
        {
          image->colormap[i].red=ScaleCharToQuantum(*p++);
          image->colormap[i].green=ScaleCharToQuantum(*p++);
          image->colormap[i].blue=ScaleCharToQuantum(*p++);
        }
        MagickFreeMemory(colormap);
      }
    if (image_info->ping && (image_info->subrange != 0))
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    /*
      Decode image.
    */
    status=DecodeImage(image,opacity);
    if (!image_info->ping && (status == False)) {
      MagickFreeMemory(global_colormap);
      GetImageException(image,exception);
      ThrowReaderException(CorruptImageError,CorruptImage,image);
    }
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
  }
  MagickFreeMemory(global_colormap);
  if ((image->columns == 0) || (image->rows == 0))
    ThrowReaderException(CorruptImageError,NegativeOrZeroImageSize,image);
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
%   R e g i s t e r G I F I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterGIFImage adds attributes for the GIF image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterGIFImage method is:
%
%      RegisterGIFImage(void)
%
*/
ModuleExport void RegisterGIFImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("GIF");
  entry->decoder=(DecoderHandler) ReadGIFImage;
  entry->encoder=(EncoderHandler) WriteGIFImage;
  entry->magick=(MagickHandler) IsGIF;
  entry->description="CompuServe graphics interchange format";
  entry->version="version 89a";
  entry->module="GIF";
  entry->coder_class=PrimaryCoderClass;
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("GIF87");
  entry->decoder=(DecoderHandler) ReadGIFImage;
  entry->encoder=(EncoderHandler) WriteGIFImage;
  entry->magick=(MagickHandler) IsGIF;
  entry->adjoin=False;
  entry->description="CompuServe graphics interchange format";
  entry->version="version 87a";
  entry->module="GIF";
  entry->coder_class=PrimaryCoderClass;
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r G I F I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterGIFImage removes format registrations made by the
%  GIF module from the list of supported formats.
%
%  The format of the UnregisterGIFImage method is:
%
%      UnregisterGIFImage(void)
%
*/
ModuleExport void UnregisterGIFImage(void)
{
  (void) UnregisterMagickInfo("GIF");
  (void) UnregisterMagickInfo("GIF87");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e G I F I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteGIFImage writes an image to a file in the Compuserve Graphics
%  image format.
%
%  The format of the WriteGIFImage method is:
%
%      MagickPassFail WriteGIFImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteGIFImage return MagickPass if the image is written.
%      MagickFail is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
static MagickPassFail WriteGIFImage(const ImageInfo *image_info,Image *image)
{
  Image
    *next_image;

  int
    y;

  long
    opacity;

  QuantizeInfo
    quantize_info;

  RectangleInfo
    page;

  register const PixelPacket
    *p;

  register long
    x;

  register long
    i;

  register unsigned char
    *q;

  size_t
    j;

  unsigned char
    bits_per_pixel,
    c,
    *colormap,
    *global_colormap;

  unsigned int
    interlace,
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
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == MagickFail)
    ThrowWriterException(FileOpenError,UnableToOpenFile,image);
  /*
    Determine image bounding box.
  */
  page.width=image->columns;
  page.height=image->rows;
  page.x=0;
  page.y=0;
  for (next_image=image; next_image != (Image *) NULL; )
  {
    page.x=next_image->page.x;
    page.y=next_image->page.y;
    if ((next_image->columns+page.x) > page.width)
      page.width=next_image->columns+page.x;
    if ((next_image->rows+page.y) > page.height)
      page.height=next_image->rows+page.y;
    next_image=next_image->next;
  }
  /*
    Allocate colormap.
  */
  global_colormap=MagickAllocateMemory(unsigned char *,768);
  colormap=MagickAllocateMemory(unsigned char *,768);
  if ((global_colormap == (unsigned char *) NULL) ||
      (colormap == (unsigned char *) NULL))
    ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
  for (i=0; i < 768; i++)
    colormap[i]=0;
  /*
    Write GIF header.
  */
  if ((GetImageAttribute(image,"comment") == (ImageAttribute *) NULL) &&
      !image_info->adjoin && !image->matte)
    (void) WriteBlob(image,6,"GIF87a");
  else
    if (LocaleCompare(image_info->magick,"GIF87") == 0)
      (void) WriteBlob(image,6,"GIF87a");
    else
      (void) WriteBlob(image,6,"GIF89a");
  page.x=image->page.x;
  page.y=image->page.y;
  if ((image->page.width != 0) && (image->page.height != 0))
    page=image->page;
  (void) WriteBlobLSBShort(image,page.width);
  (void) WriteBlobLSBShort(image,page.height);
  /*
    Write images to file.
  */
  interlace=image_info->interlace;
  if (image_info->adjoin && (image->next != (Image *) NULL))
    interlace=NoInterlace;
  opacity=(-1);
  scene=0;
  do
  {
    (void) TransformColorspace(image,RGBColorspace);
    if ((image->storage_class == DirectClass) || (image->colors > 256))
      {
        /*
          GIF requires that the image is colormapped.
        */
        GetQuantizeInfo(&quantize_info);
        quantize_info.dither=image_info->dither;
        quantize_info.number_colors=image->matte ? 255 : 256;
        (void) QuantizeImage(&quantize_info,image);
        if (image->matte)
          {
            /*
              Set transparent pixel.
            */
            opacity=(long) image->colors++;
            MagickReallocMemory(PixelPacket *,image->colormap,
              image->colors*sizeof(PixelPacket));
            if (image->colormap == (PixelPacket *) NULL)
              {
                MagickFreeMemory(global_colormap);
                MagickFreeMemory(colormap);
                ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image)
              }
            image->colormap[opacity]=image->background_color;
            for (y=0; y < (long) image->rows; y++)
            {
              register IndexPacket
                *indexes;

              p=GetImagePixelsEx(image,0,y,image->columns,1,
                &image->exception);
              if (p == (const PixelPacket *) NULL)
                break;
              indexes=AccessMutableIndexes(image);
              for (x=0; x < (long) image->columns; x++)
              {
                if (p->opacity == TransparentOpacity)
                  indexes[x]=(IndexPacket) opacity;
                p++;
              }
              if (!SyncImagePixels(image))
                break;
            }
          }
      }
    else
      if (image->matte)
        {
          /*
            Identify transparent pixel index.
          */
          for (y=0; y < (long) image->rows; y++)
          {
            register const IndexPacket
              *indexes;

            p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
            if (p == (const PixelPacket *) NULL)
              break;
            indexes=AccessImmutableIndexes(image);
            for (x=0; x < (long) image->columns; x++)
            {
              if (p->opacity == TransparentOpacity)
                {
                  opacity=(long) indexes[x];
                  break;
                }
              p++;
            }
            if (x < (long) image->columns)
              break;
          }
        }
    if (image->colormap == (PixelPacket *) NULL)
      break;
    for (bits_per_pixel=1; bits_per_pixel < 8; bits_per_pixel++)
      if ((1UL << bits_per_pixel) >= image->colors)
        break;
    q=colormap;
    for (i=0; i < (long) image->colors; i++)
    {
      *q++=ScaleQuantumToChar(image->colormap[i].red);
      *q++=ScaleQuantumToChar(image->colormap[i].green);
      *q++=ScaleQuantumToChar(image->colormap[i].blue);
    }
    for ( ; i < (1L << bits_per_pixel); i++)
    {
      *q++=(Quantum) 0x0;
      *q++=(Quantum) 0x0;
      *q++=(Quantum) 0x0;
    }
    if ((image->previous == (Image *) NULL) || !image_info->adjoin)
      {
        /*
          Write global colormap.
        */
        c=0x80;
        c|=(8-1) << 4;  /* color resolution */
        c|=(bits_per_pixel-1);   /* size of global colormap */
        (void) WriteBlobByte(image,c);
        for (p=image->colormap, j=0; j < Max(image->colors-1,1); j++, p++)
          if (ColorMatch(&image->background_color,p))
            break;
        (void) WriteBlobByte(image,(long) j);  /* background color */
        (void) WriteBlobByte(image,0x0);  /* reserved */
        (void) WriteBlob(image,3*(1 << bits_per_pixel),(char *) colormap);
        for (j=0; j < 768; j++)
          global_colormap[j]=colormap[j];
      }
    if (LocaleCompare(image_info->magick,"GIF87") != 0)
      {
        /*
          Write Graphics Control extension.
        */
        (void) WriteBlobByte(image,0x21);
        (void) WriteBlobByte(image,0xf9);
        (void) WriteBlobByte(image,0x04);
        c=(unsigned char) ((int) image->dispose << 2);
        if (opacity >= 0)
          c|=0x01;
        (void) WriteBlobByte(image,c);
        (void) WriteBlobLSBShort(image,image->delay);
        (void) WriteBlobByte(image,opacity >= 0 ? opacity : 0);
        (void) WriteBlobByte(image,0x00);
        if (GetImageAttribute(image,"comment") != (ImageAttribute *) NULL)
          {
            const ImageAttribute
              *attribute;

            register char
              *p;

            size_t
              count;

            /*
              Write Comment extension.
            */
            (void) WriteBlobByte(image,0x21);
            (void) WriteBlobByte(image,0xfe);
            attribute=GetImageAttribute(image,"comment");
            p=attribute->value;
            while (strlen(p) != 0)
            {
              count=Min(strlen(p),255);
              (void) WriteBlobByte(image,(long) count);
              for (i=0; i < (long) count; i++)
                (void) WriteBlobByte(image,*p++);
            }
            (void) WriteBlobByte(image,0x0);
          }
        if ((image->previous == (Image *) NULL) &&
            (image->next != (Image *) NULL) && (image->iterations != 1))
          {
            /*
              Write Netscape Loop extension.
            */
            (void) WriteBlobByte(image,0x21);
            (void) WriteBlobByte(image,0xff);
            (void) WriteBlobByte(image,0x0b);
            (void) WriteBlob(image,11,"NETSCAPE2.0");
            (void) WriteBlobByte(image,0x03);
            (void) WriteBlobByte(image,0x01);
            (void) WriteBlobLSBShort(image,image->iterations);
            (void) WriteBlobByte(image,0x00);
          }
      }
    (void) WriteBlobByte(image,',');  /* image separator */
    /*
      Write the image header.
    */
    page.x=image->page.x;
    page.y=image->page.y;
    if ((image->page.width != 0) && (image->page.height != 0))
      page=image->page;
    (void) WriteBlobLSBShort(image,page.x);
    (void) WriteBlobLSBShort(image,page.y);
    (void) WriteBlobLSBShort(image,image->columns);
    (void) WriteBlobLSBShort(image,image->rows);
    c=0x00;
    if (interlace != NoInterlace)
      c|=0x40;  /* pixel data is interlaced */
    for (j=0; j < (3*image->colors); j++)
      if (colormap[j] != global_colormap[j])
        break;
    if (j == (3*image->colors))
      (void) WriteBlobByte(image,c);
    else
      {
        c|=0x80;
        c|=(bits_per_pixel-1);   /* size of local colormap */
        (void) WriteBlobByte(image,c);
        (void) WriteBlob(image,3*(1 << bits_per_pixel),(char *) colormap);
      }
    /*
      Write the image data.
    */
    c=Max(bits_per_pixel,2);
    (void) WriteBlobByte(image,c);
    status=EncodeImage(image_info,image,Max(bits_per_pixel,2)+1);
    if (status == MagickFail)
      {
        MagickFreeMemory(global_colormap);
        MagickFreeMemory(colormap);
        ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image)
      }
    (void) WriteBlobByte(image,0x0);
    if (image->next == (Image *) NULL)
      break;
    image=SyncNextImageInList(image);
    status=MagickMonitorFormatted(scene++,GetImageListLength(image),
                                  &image->exception,SaveImagesText,
                                  image->filename);
    if (status == MagickFail)
      break;
  } while (image_info->adjoin);
  (void) WriteBlobByte(image,';'); /* terminator */
  MagickFreeMemory(global_colormap);
  MagickFreeMemory(colormap);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseBlob(image);
  return(MagickPass);
}
