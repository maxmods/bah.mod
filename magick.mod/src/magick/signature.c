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
%        SSSSS  IIIII   GGGG  N   N   AAA   TTTTT  U   U  RRRR   EEEEE        %
%        SS       I    G      NN  N  A   A    T    U   U  R   R  E            %
%         SSS     I    G  GG  N N N  AAAAA    T    U   U  RRRR   EEE          %
%           SS    I    G   G  N  NN  A   A    T    U   U  R R    E            %
%        SSSSS  IIIII   GGG   N   N  A   A    T     UUU   R  R   EEEEE        %
%                                                                             %
%                                                                             %
%             Methods to Compute a Message Digest for an Image                %
%                                                                             %
%                                                                             %
%                             Software Design                                 %
%                               John Cristy                                   %
%                              December 1992                                  %
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
#include "magick/monitor.h"
#include "magick/pixel_cache.h"
#include "magick/signature.h"
#include "magick/utility.h"

/*
  Define declarations.
*/
#define Trunc32(x)  ((x) & 0xffffffffUL)

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   F i n a l i z e S i g n a t u r e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method FinalizeSignature finalizes the SHA message digest computation.
%
%  The format of the FinalizeSignature method is:
%
%      FinalizeSignature(SignatureInfo *signature_info)
%
%  A description of each parameter follows:
%
%    o signature_info: The address of a structure of type SignatureInfo.
%
%
*/
MagickExport void FinalizeSignature(SignatureInfo *signature_info)
{
  long
    count;

  unsigned long
    high_order,
    low_order;

  /*
    Finalize computing the SHA digest.
  */
  low_order=signature_info->low_order;
  high_order=signature_info->high_order;
  count=(long) ((low_order >> 3) & 0x3f);
  signature_info->message[count++]=0x80;
  if (count <= (SignatureSize-8))
    (void) memset(signature_info->message+count,0,SignatureSize-8-count);
  else
    {
      (void) memset(signature_info->message+count,0,SignatureSize-count);
      TransformSignature(signature_info);
      (void) memset(signature_info->message,0,SignatureSize-8);
    }
  signature_info->message[56]=(unsigned char) (high_order >> 24);
  signature_info->message[57]=(unsigned char) (high_order >> 16);
  signature_info->message[58]=(unsigned char) (high_order >> 8);
  signature_info->message[59]=(unsigned char) high_order;
  signature_info->message[60]=(unsigned char) (low_order >> 24);
  signature_info->message[61]=(unsigned char) (low_order >> 16);
  signature_info->message[62]=(unsigned char) (low_order >> 8);
  signature_info->message[63]=(unsigned char) low_order;
  TransformSignature(signature_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t S i g n a t u r e I n f o                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetSignatureInfo initializes the SHA message digest structure.
%
%  The format of the GetSignatureInfo method is:
%
%      GetSignatureInfo(SignatureInfo *signature_info)
%
%  A description of each parameter follows:
%
%    o signature_info: The address of a structure of type SignatureInfo.
%
%
*/
MagickExport void GetSignatureInfo(SignatureInfo *signature_info)
{
  (void) memset(signature_info,0,sizeof(SignatureInfo));
  signature_info->digest[0]=0x6a09e667UL;
  signature_info->digest[1]=0xbb67ae85UL;
  signature_info->digest[2]=0x3c6ef372UL;
  signature_info->digest[3]=0xa54ff53aUL;
  signature_info->digest[4]=0x510e527fUL;
  signature_info->digest[5]=0x9b05688cUL;
  signature_info->digest[6]=0x1f83d9abUL;
  signature_info->digest[7]=0x5be0cd19UL;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S i g n a t u r e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SignatureImage() computes a message digest from an image pixel stream with
%  an implementation of the NIST SHA-256 Message Digest algorithm.  This
%  signature uniquely identifies the image and is convenient for determining
%  if an image has been modified or whether two images are identical.
%
%  The format of the SignatureImage method is:
%
%      unsigned int SignatureImage(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
#define SignatureImageText "[%s] Compute SHA-256 signature..."
MagickExport unsigned int SignatureImage(Image *image)
{
  char
    signature[MaxTextExtent];

  const IndexPacket
    *indexes;

  long
    y;

  register const PixelPacket
    *p;

  register long
    x;

  register unsigned char
    *q;

  SignatureInfo
    signature_info;

  unsigned char
    *message;

  unsigned long
    quantum;

  /*
    Allocate memory for digital signature.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  message=MagickAllocateMemory(unsigned char *,20*image->columns);
  if (message == (unsigned char *) NULL)
    ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
      UnableToComputeImageSignature);
  /*
    Compute image digital signature.
  */
  GetSignatureInfo(&signature_info);
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    indexes=AccessImmutableIndexes(image);
    q=message;
    for (x=0; x < (long) image->columns; x++)
    {
      quantum=ScaleQuantumToLong(p->red);
      *q++=(unsigned char) (quantum >> 24);
      *q++=(unsigned char) (quantum >> 16);
      *q++=(unsigned char) (quantum >> 8);
      *q++=(unsigned char) quantum;
      quantum=ScaleQuantumToLong(p->green);
      *q++=(unsigned char) (quantum >> 24);
      *q++=(unsigned char) (quantum >> 16);
      *q++=(unsigned char) (quantum >> 8);
      *q++=(unsigned char) quantum;
      quantum=ScaleQuantumToLong(p->blue);
      *q++=(unsigned char) (quantum >> 24);
      *q++=(unsigned char) (quantum >> 16);
      *q++=(unsigned char) (quantum >> 8);
      *q++=(unsigned char) quantum;
      if (!image->matte)
        {
          if (image->colorspace == CMYKColorspace)
            {
              quantum=ScaleQuantumToLong(p->opacity);
              *q++=(unsigned char) (quantum >> 24);
              *q++=(unsigned char) (quantum >> 16);
              *q++=(unsigned char) (quantum >> 8);
              *q++=(unsigned char) quantum;
            }
          quantum=ScaleQuantumToLong(OpaqueOpacity);
          *q++=(unsigned char) (quantum >> 24);
          *q++=(unsigned char) (quantum >> 16);
          *q++=(unsigned char) (quantum >> 8);
          *q++=(unsigned char) quantum;
        }
      else
        {
          quantum=ScaleQuantumToLong(p->opacity);
          *q++=(unsigned char) (quantum >> 24);
          *q++=(unsigned char) (quantum >> 16);
          *q++=(unsigned char) (quantum >> 8);
          *q++=(unsigned char) quantum;
          if (image->colorspace == CMYKColorspace)
            {
              quantum=ScaleQuantumToLong(indexes[x]);
              *q++=(unsigned char) (quantum >> 24);
              *q++=(unsigned char) (quantum >> 16);
              *q++=(unsigned char) (quantum >> 8);
              *q++=(unsigned char) quantum;
            }
        }
      p++;
    }
    UpdateSignature(&signature_info,message,q-message);
    if (QuantumTick(y,image->rows))
      if (!MagickMonitorFormatted(y,image->rows,&image->exception,
                                  SignatureImageText,image->filename))
        break;
  }
  FinalizeSignature(&signature_info);
  MagickFreeMemory(message);
  /*
    Convert digital signature to a 64 character hex string.
  */
  FormatString(signature,"%08lx%08lx%08lx%08lx%08lx%08lx%08lx%08lx",
    signature_info.digest[0],signature_info.digest[1],signature_info.digest[2],
    signature_info.digest[3],signature_info.digest[4],signature_info.digest[5],
    signature_info.digest[6],signature_info.digest[7]);
  (void) SetImageAttribute(image,"signature",(char *) NULL);
  (void) SetImageAttribute(image,"signature",signature);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   T r a n s f o r m S i g n a t u r e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  TransformSignature() transforms the SHA message digest.
%
%  The format of the TransformSignature method is:
%
%      TransformSignature(SignatureInfo *signature_info)
%
%  A description of each parameter follows:
%
%    o signature_info: The address of a structure of type SignatureInfo.
%
%
*/
MagickExport void TransformSignature(SignatureInfo *signature_info)
{
#define Ch(x,y,z)  (((x) & (y))^(~(x) & (z)))
#define Maj(x,y,z)  (((x) & (y))^((x) & (z))^((y) & (z)))
#define Rot32(x,n)  Trunc32(((x >> n) | (x << (32-n))))
#define Sigma0(x)  (Rot32(x,7)^Rot32(x,18)^Trunc32(x >> 3))
#define Sigma1(x)  (Rot32(x,17)^Rot32(x,19)^Trunc32(x >> 10))
#define Suma0(x)  (Rot32(x,2)^Rot32(x,13)^Rot32(x,22))
#define Suma1(x)  (Rot32(x,6)^Rot32(x,11)^Rot32(x,25))

  static unsigned long
    K[64] =
    {
      0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL, 0x3956c25bUL,
      0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL, 0xd807aa98UL, 0x12835b01UL,
      0x243185beUL, 0x550c7dc3UL, 0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL,
      0xc19bf174UL, 0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL,
      0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL, 0x983e5152UL,
      0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL, 0xc6e00bf3UL, 0xd5a79147UL,
      0x06ca6351UL, 0x14292967UL, 0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL,
      0x53380d13UL, 0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
      0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL, 0xd192e819UL,
      0xd6990624UL, 0xf40e3585UL, 0x106aa070UL, 0x19a4c116UL, 0x1e376c08UL,
      0x2748774cUL, 0x34b0bcb5UL, 0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL,
      0x682e6ff3UL, 0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL,
      0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL
    };  /* 32-bit fractional part of the cube root of the first 64 primes */

  int
    j,
    shift;

  register long
    i;

  register unsigned char
    *p;

  unsigned long
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    lsb_first,
    T,
    T1,
    T2,
    W[64];

  shift=32;
  p=signature_info->message;
  lsb_first=1;
  if (*(char *) &lsb_first)
    {
      if (sizeof(lsb_first) > 4)
        for (i=0; i < 16; i+=2)
        {
          T=(*((unsigned long *) p));
          p+=8;
          W[i]=((T << 24) & 0xff000000U) | ((T << 8) & 0x00ff0000U) |
            ((T >> 8) & 0x0000ff00U) | ((T >> 24) & 0x000000ffU);
          T>>=shift;
          W[i+1]=((T << 24) & 0xff000000U) | ((T << 8) & 0x00ff0000U) |
            ((T >> 8) & 0x0000ff00U) | ((T >> 24) & 0x000000ffU);
        }
      else
        for (i=0; i < 16; i++)
        {
          T=(*((unsigned long *) p));
          p+=4;
          W[i]=((T << 24) & 0xff000000U) | ((T << 8) & 0x00ff0000U) |
            ((T >> 8) & 0x0000ff00U) | ((T >> 24) & 0x000000ffU);
        }
    }
  else
    {
      if (sizeof(lsb_first) > 4)
        for (i=0; i < 16; i+=2)
        {
          T=(*((unsigned long *) p));
          p+=8;
          W[i]=Trunc32(T >> shift);
          W[i+1]=Trunc32(T);
        }
      else
        for (i=0; i < 16; i++)
        {
          T=(*((unsigned long *) p));
          p+=4;
          W[i]=Trunc32(T);
        }
    }
  A=signature_info->digest[0];
  B=signature_info->digest[1];
  C=signature_info->digest[2];
  D=signature_info->digest[3];
  E=signature_info->digest[4];
  F=signature_info->digest[5];
  G=signature_info->digest[6];
  H=signature_info->digest[7];
  for (i=16; i < 64; i++)
    W[i]=Trunc32(Sigma1(W[i-2])+W[i-7]+Sigma0(W[i-15])+W[i-16]);
  for (j=0; j < 64; j++)
  {
    T1=Trunc32(H+Suma1(E)+Ch(E,F,G)+K[j]+W[j]);
    T2=Trunc32(Suma0(A)+Maj(A,B,C));
    H=G;
    G=F;
    F=E;
    E=Trunc32(D+T1);
    D=C;
    C=B;
    B=A;
    A=Trunc32(T1+T2);
  }
  signature_info->digest[0]=Trunc32(signature_info->digest[0]+A);
  signature_info->digest[1]=Trunc32(signature_info->digest[1]+B);
  signature_info->digest[2]=Trunc32(signature_info->digest[2]+C);
  signature_info->digest[3]=Trunc32(signature_info->digest[3]+D);
  signature_info->digest[4]=Trunc32(signature_info->digest[4]+E);
  signature_info->digest[5]=Trunc32(signature_info->digest[5]+F);
  signature_info->digest[6]=Trunc32(signature_info->digest[6]+G);
  signature_info->digest[7]=Trunc32(signature_info->digest[7]+H);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U p d a t e S i g n a t u r e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  UpdateSignature() updates the SHA message digest.
%
%  The format of the UpdateSignature method is:
%
%      UpdateSignature(SignatureInfo *signature_info,
%        const unsigned char *message,const size_t length)
%
%  A description of each parameter follows:
%
%    o signature_info: The address of a structure of type SignatureInfo.
%
%    o message: the message
%
%    o length: The length of the message.
%
%
*/
MagickExport void UpdateSignature(SignatureInfo *signature_info,
  const unsigned char *message,const size_t length)
{
  register long
    i;

  size_t
    n;

  unsigned long
    count;

  /*
    Update the SHA digest.
  */
  n=length;
  count=Trunc32(signature_info->low_order+((unsigned char) n << 3));
  if (n < signature_info->low_order)
    signature_info->high_order++;
  signature_info->low_order=count;
  signature_info->high_order+=(unsigned char) (n >> 29);
  if (signature_info->offset)
    {
      i=SignatureSize-signature_info->offset;
      if (i > (long) n)
        i=(long) n;
      (void) memcpy(signature_info->message+signature_info->offset,message,i);
      n-=i;
      message+=i;
      signature_info->offset+=i;
      if (signature_info->offset != SignatureSize)
        return;
      TransformSignature(signature_info);
    }
  while (n >= SignatureSize)
  {
    (void) memcpy(signature_info->message,message,SignatureSize);
    message+=SignatureSize;
    n-=SignatureSize;
    TransformSignature(signature_info);
  }
  (void) memcpy(signature_info->message,message,n);
  signature_info->offset=(long) n;
}
