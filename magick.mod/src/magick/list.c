/*
% Copyright (C) 2003 GraphicsMagick Group
% Copyright (C) 2002, 2003 ImageMagick Studio
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                         L      IIIII  SSSSS  TTTTT                          %
%                         L        I    SS       T                            %
%                         L        I     SSS     T                            %
%                         L        I       SS    T                            %
%                         LLLLL  IIIII  SSSSS    T                            %
%                                                                             %
%                                                                             %
%                     GraphicsMagick Image List Methods                       %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                January 2003                                 %
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
#include "magick/list.h"
#include "magick/blob.h"
#include "magick/utility.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A p p e n d I m a g e T o L i s t                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AppendImageToList() appends an image to the end of the list.
%
%  The format of the AppendImageToList method is:
%
%      AppendImageToList(Image **images,Image *image)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%    o image: The image.
%
%
*/
MagickExport void AppendImageToList(Image **images,Image *image)
{
  register Image
    *p;

  assert(images != (Image **) NULL);
  if (image == (Image *) NULL)
    return;
  assert(image->signature == MagickSignature);
  if ((*images) == (Image *) NULL)
    {
      *images=image;
      return;
    }
  assert((*images)->signature == MagickSignature);
  for (p=(*images); p->next != (Image *) NULL; p=p->next);
  p->next=image;
  image->previous=p;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e I m a g e L i s t                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CloneImageList() returns a duplicate of the image list.
%
%  The format of the CloneImageList method is:
%
%      Image *CloneImageList(const Image *images,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *CloneImageList(const Image *images,ExceptionInfo *exception)
{
  Image
    *clone,
    *image;

  register Image
    *p;

  if (images == (Image *) NULL)
    return((Image *) NULL);
  assert(images->signature == MagickSignature);
  while (images->previous != (Image *) NULL)
    images=images->previous;
  image=(Image *) NULL;
  for (p=(Image *) NULL; images != (Image *) NULL; images=images->next)
  {
    clone=CloneImage(images,0,0,True,exception);
    if (clone == (Image *) NULL)
      {
        if (image != (Image *) NULL)
          DestroyImageList(image);
        return((Image *) NULL);
      }
    if (image == (Image *) NULL)
      {
        image=clone;
        p=image;
        continue;
      }
    p->next=clone;
    clone->previous=p;
    p=p->next;
  }
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e l e t e I m a g e F r o m L i s t                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DeleteImageFromList() deletes an image from the list.
%
%  The format of the DeleteImageFromList method is:
%
%      DeleteImageFromList(Image **images)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%
*/
MagickExport void DeleteImageFromList(Image **images)
{
  register Image
    *p;

  assert(images != (Image **) NULL);
  if ((*images) == (Image *) NULL)
    return;
  assert((*images)->signature == MagickSignature);
  p=(*images);
  if ((p->previous == (Image *) NULL) && (p->next == (Image *) NULL))
    *images=(Image *) NULL;
  else
    {
      if (p->previous != (Image *) NULL)
        {
          p->previous->next=p->next;
          *images=p->previous;
        }
      if (p->next != (Image *) NULL)
        {
          p->next->previous=p->previous;
          *images=p->next;
        }
    }
  DestroyImage(p);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y I m a g e L i s t                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyImageList() destroys an image list.
%
%  The format of the DestroyImageList method is:
%
%      void DestroyImageList(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image sequence.
%
%
*/
MagickExport void DestroyImageList(Image *images)
{
  register Image
    *p;

  if (images == (Image *) NULL)
    return;
  assert(images->signature == MagickSignature);
  for (p=images; p->previous != (Image *) NULL; p=p->previous);
  for (images=p; p != (Image *) NULL; images=p)
  {
    p=p->next;
    images->next=0;
    if(p)
      p->previous=0;
    DestroyImage(images);
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t F i r s t I m a g e I n L i s t                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetFirstImageInList() returns a pointer to the first image in the list
%
%  The format of the GetFirstImageInList method is:
%
%      Image *GetFirstImageInList(const Image *images)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
*/
MagickExport Image *GetFirstImageInList(const Image *images)
{
  register const Image
    *p;

  if (images == (Image *) NULL)
    return((Image *) NULL);
  assert(images->signature == MagickSignature);
  for (p=images; p->previous != (Image *) NULL; p=p->previous);
  return((Image *) p);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e F r o m L i s t                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageFromList() returns an image at the specified offset from the list.
%
%  The format of the GetImageFromList method is:
%
%      Image *GetImageFromList(const Image *images,const long offset)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%    o offset: The position within the list.
%
%
*/
MagickExport Image *GetImageFromList(const Image *images,const long offset)
{
  register const Image
    *p;

  register long
    i;

  if (images == (Image *) NULL)
    return((Image *) NULL);
  assert(images->signature == MagickSignature);
  for (p=images; p->previous != (Image *) NULL; p=p->previous);
  for (i=0; p != (Image *) NULL; p=p->next)
    if (i++ == offset)
      break;
  if (p == (Image *) NULL)
    return((Image *) NULL);
  return((Image *) p);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e I n d e x I n L i s t                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageIndexInList() returns the offset in the list of the specified image.
%
%  The format of the GetImageIndexInList method is:
%
%      long GetImageIndexInList(const Image *images)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%
*/
MagickExport long GetImageIndexInList(const Image *images)
{
  register long
    i;

  if (images == (const Image *) NULL)
    return(-1);
  assert(images->signature == MagickSignature);
  for (i=0; images->previous != (Image *) NULL; i++)
    images=images->previous;
  return(i);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e L i s t L e n g t h                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageListLength() returns the length of the list (the number of images in
%  the list).
%
%  The format of the GetImageListLength method is:
%
%      unsigned long GetImageListLength(const Image *images)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%
*/
MagickExport unsigned long GetImageListLength(const Image *images)
{
  register long
    i;

  if (images == (Image *) NULL)
    return(0);
  assert(images->signature == MagickSignature);
  while (images->previous != (Image *) NULL)
    images=images->previous;
  for (i=0; images != (Image *) NULL; images=images->next)
    i++;
  return(i);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t L a s t I m a g e I n L i s t                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetLastImageInList() returns a pointer to the last image in the list
%
%  The format of the GetLastImageInList method is:
%
%      Image *GetLastImageInList(const Image *images)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
*/
MagickExport Image *GetLastImageInList(const Image *images)
{
  register const Image
    *p;

  if (images == (Image *) NULL)
    return((Image *) NULL);
  assert(images->signature == MagickSignature);
  for (p=images; p->next != (Image *) NULL; p=p->next);
  return((Image *) p);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t N e x t I m a g e I n L i s t                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetNextImageInList() returns the next image in the list.
%
%  The format of the GetNextImageInList method is:
%
%      Image *GetNextImageInList(const Image *images)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%
*/
MagickExport Image *GetNextImageInList(const Image *images)
{
  if (images == (Image *) NULL)
    return((Image *) NULL);
  assert(images->signature == MagickSignature);
  return(images->next);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t P r e v i o u s I m a g e I n L i s t                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetPreviousImageInList() returns the previous image in the list.
%
%  The format of the GetPreviousImageInList method is:
%
%      Image *GetPreviousImageInList(const Image *images)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%
*/
MagickExport Image *GetPreviousImageInList(const Image *images)
{
  if (images == (Image *) NULL)
    return((Image *) NULL);
  assert(images->signature == MagickSignature);
  return(images->previous);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     I m a g e L i s t T o A r r a y                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ImageListToArray() is a convenience method that converts an image list to
%  a sequential array.  For example,
%
%    group = ImageListToArray(images, exception);
%    for (i = 0; i < n; i++)
%      puts(group[i]->filename);
%
%  The format of the ImageListToArray method is:
%
%      Image **ImageListToArray(const Image *images,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image list.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image **ImageListToArray(const Image *images,
  ExceptionInfo *exception)
{
  Image
    **group;

  register long
    i;

  if (images == (Image *) NULL)
    return((Image **) NULL);
  assert(images->signature == MagickSignature);
  group=MagickAllocateMemory(Image **,(GetImageListLength(images)+1)*sizeof(Image *));
  if (group == (Image **) NULL)
    {
      ThrowException3(exception,ResourceLimitError,MemoryAllocationFailed,
        UnableToCreateImageGroup);
      return((Image **) NULL);
    }
  while (images->previous != (Image *) NULL)
    images=images->previous;
  for (i=0; images != (Image *) NULL; images=images->next)
    group[i++]=(Image *) images;
  group[i] = (Image *) NULL;
  return(group);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I n s e r t I m a g e I n L i s t                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  InsertImageInList() inserts an image in the list.
%
%  The format of the InsertImageInList method is:
%
%      InsertImageInList(Image **images,Image *image)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%    o image: The image.
%
%
*/
MagickExport void InsertImageInList(Image **images,Image *image)
{
  Image
    *split;

  assert(images != (Image **) NULL);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if ((*images) == (Image *) NULL)
    return;
  assert((*images)->signature == MagickSignature);
  split=SplitImageList(*images);
  if (split == (Image *) NULL)
    return;
  AppendImageToList(images,image);
  AppendImageToList(images,split);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N e w I m a g e L i s t                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  NewImageList() creates an empty image list.
%
%  The format of the NewImageList method is:
%
%      Image *NewImageList(void)
%
*/
MagickExport Image *NewImageList(void)
{
  return((Image *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P r e p e n d I m a g e T o L i s t                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PrependImageToList() prepends the image to the beginning of the list.
%
%  The format of the PrependImageToList method is:
%
%      PrependImageToList(Image *images,Image *image)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%    o image: The image.
%
%
*/
MagickExport void PrependImageToList(Image **images,Image *image)
{
  AppendImageToList(&image,*images);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e m o v e F i r s t I m a g e F r o m L i s t                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  RemoveFirstImageFromList() removes an image from the beginning of the list.
%
%  The format of the RemoveFirstImageFromList method is:
%
%      Image *RemoveFirstImageFromList(Image **images)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%
*/
MagickExport Image *RemoveFirstImageFromList(Image **images)
{
  Image
    *image;

  assert(images != (Image **) NULL);
  if ((*images) == (Image *) NULL)
    return((Image *) NULL);
  assert((*images)->signature == MagickSignature);
  image=(*images);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  if (image == *images)
    *images=(*images)->next;
  if (image->next != (Image *) NULL)
    {
      image->next->previous=(Image *) NULL;
      image->next=(Image *) NULL;
    }
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e m o v e L a s t I m a g e F r o m L i s t                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  RemoveLastImageFromList() removes the last image from the list.
%
%  The format of the RemoveLastImageFromList method is:
%
%      Image *RemoveLastImageFromList(Image **images)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%
*/
MagickExport Image *RemoveLastImageFromList(Image **images)
{
  Image
    *image;

  assert(images != (Image **) NULL);
  if ((*images) == (Image *) NULL)
    return((Image *) NULL);
  assert((*images)->signature == MagickSignature);
  image=(*images);
  while (image->next != (Image *) NULL)
    image=image->next;
  if (image == *images)
    *images=(*images)->previous;
  if (image->previous != (Image *) NULL)
    {
      image->previous->next=(Image *) NULL;
      image->previous=(Image *) NULL;
    }
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e p l a c e I m a g e I n L i s t                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReplaceImageInList() replaces an image in the list.
%
%  The format of the ReplaceImageInList method is:
%
%      ReplaceImageInList(Image **images,Image *image)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%    o image: The image.
%
%
*/
MagickExport void ReplaceImageInList(Image **images,Image *image)
{
  assert(images != (Image **) NULL);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if ((*images) == (Image *) NULL)
    return;
  assert((*images)->signature == MagickSignature);
  image->next=(*images)->next;
  if (image->next != (Image *) NULL)
    image->next->previous=image;
  image->previous=(*images)->previous;
  if (image->previous != (Image *) NULL)
    image->previous->next=image;
  DestroyImage(*images);
  (*images)=image;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e v e r s e I m a g e L i s t                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReverseImageList() reverses the image list.
%
%  The format of the ReverseImageList method is:
%
%       ReverseImageList(const Image **images)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%
*/
MagickExport void ReverseImageList(Image **images)
{
  Image
    *next;

  register Image
    *p;

  assert(images != (Image **) NULL);
  if ((*images) == (Image *) NULL)
    return;
  assert((*images)->signature == MagickSignature);
  for (p=(*images); p->next != (Image *) NULL; p=p->next);
  *images=p;
  for ( ; p != (Image *) NULL; p=p->next)
  {
    next=p->next;
    p->next=p->previous;
    p->previous=next;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S p l i c e I m a g e I n t o L i s t                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SpliceImageIntoList() removes 'length' images from the list and replaces
%  them with the specified splice.
%
%  The format of the SpliceImageIntoList method is:
%
%      SpliceImageIntoList(Image **images,const unsigned long length,
%        Image *splice)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%    o length: The length of the image list to remove.
%
%    o splice: Replace the removed image list with this list.
%
%
*/
MagickExport void SpliceImageIntoList(Image **images,const unsigned long length,
  Image *splice)
{
  Image
    *split;

  register long
    i;
  
  assert(images != (Image **) NULL);
  assert(splice != (Image *) NULL);
  assert(splice->signature == MagickSignature);
  if ((*images) == (Image *) NULL)
    return;
  assert((*images)->signature == MagickSignature);
  split=SplitImageList(*images);
  if (split == (Image *) NULL)
    return;
  AppendImageToList(images,splice);
  for (i=0; (i < (long) length) && (split != (Image *) NULL); i++)
    (void) DeleteImageFromList(&split);
  AppendImageToList(images,split);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S p l i t I m a g e L i s t                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SplitImageList() splits an image into two lists.
%
%  The format of the SplitImageList method is:
%
%      Image *SplitImageList(Image *images)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%
*/
MagickExport Image *SplitImageList(Image *images)
{
  if ((images == (Image *) NULL) || (images->next == (Image *) NULL))
    return((Image *) NULL);
  images=images->next;
  images->previous->next=(Image *) NULL;
  images->previous=(Image *) NULL;
  return(images);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   S y n c N e x t I m a g e I n L i s t                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SyncNextImageInList() returns the next image in the list after the blob
%  referenced is synchronized with the current image.
%
%  The format of the SyncNextImageInList method is:
%
%      Image *SyncNextImageInList(const Image *images)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%
*/
MagickExport Image *SyncNextImageInList(const Image *images)
{
  if (images == (Image *) NULL)
    return((Image *) NULL);
  assert(images->signature == MagickSignature);
  if (images->next == (Image *) NULL)
    return((Image *) NULL);
  if (images->blob != images->next->blob)
    {
      DestroyBlob(images->next);
      images->next->blob=ReferenceBlob(images->blob);
    }
  return(images->next);
}
