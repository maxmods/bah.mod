/*
% Copyright (C) 2003 - 2009 GraphicsMagick Group
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
%                    M   M  EEEEE  M   M   OOO   RRRR   Y   Y                 %
%                    MM MM  E      MM MM  O   O  R   R   Y Y                  %
%                    M M M  EEE    M M M  O   O  RRRR     Y                   %
%                    M   M  E      M   M  O   O  R R      Y                   %
%                    M   M  EEEEE  M   M   OOO   R  R     Y                   %
%                                                                             %
%                                                                             %
%                    GraphicsMagick Memory Allocation Methods                 %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1998                                   %
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
#include "magick/utility.h"

/*
  Static variables.
*/
MagickFreeFunc    FreeFunc    = free;
MagickMallocFunc  MallocFunc  = malloc;
MagickReallocFunc ReallocFunc = realloc;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k A l l o c F u n c t i o n s                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickAllocFunctions() provides a way for the user to supply a preferred
%  free(), malloc(), and realloc() functions.  Otherwise the default system
%  versions are used.
%
%  The format of the  MagickAllocFunctions method is:
%
%      void MagickAllocFunctions(MagickFreeFunc free_func,
%                                MagickMallocFunc malloc_func,
%                                MagickReallocFunc realloc_func)
%
%  A description of each parameter follows:
%
%    o free_func: Function to free memory.
%
%    o malloc_func: Function to allocate memory.
%
%    o realloc_func: Function to reallocate memory.
%
*/
MagickExport void MagickAllocFunctions(MagickFreeFunc free_func,
                                       MagickMallocFunc malloc_func,
                                       MagickReallocFunc realloc_func)
{
  FreeFunc    = free_func;
  MallocFunc  = malloc_func;
  ReallocFunc = realloc_func;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k A r r a y Si z e                                              %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickArraySize() returnes the size of an array given two size_t arguments.
%  Zero is returned if the computed result overflows the size_t type.
%
%  The format of the MagickArraySize method is:
%
%      size_t MagickArraySize(const size_t count, const size_t size);
%
%  A description of each parameter follows:
%
%    o count: The number of elements in the array.
%
%    o size: The size of one array element.
%
*/
MagickExport size_t MagickArraySize(const size_t count, const size_t size)
{
  size_t
    allocation_size;

  allocation_size = size * count;
  if ((count != 0) && (size != allocation_size/count))
    allocation_size = 0;

  return allocation_size;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k M a l l o c                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickMalloc() returns a pointer to a block of memory of at least size
%  bytes suitably aligned for any use.  NULL is returned if insufficient
%  memory is available or the requested size is zero.
%
%  The format of the  MagickMalloc method is:
%
%      void * MagickMalloc(const size_t size)
%
%  A description of each parameter follows:
%
%    o size: The size of the memory in bytes to allocate.
%
%
*/
MagickExport void * MagickMalloc(const size_t size)
{
  if (size == 0)
    return ((void *) NULL);

  return (MallocFunc)(size);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k M a l l o c A r r a y                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickMallocArray() returns a pointer to a block of memory of
%  sufficient size to support an array of elements of a specified size.
%  The returned memory is suitably aligned for any use.  NULL is returned
%  if the required memory exceeds the range of size_t, the specified size
%  is zero, or there is insufficient memory available.
%
%  The format of the MagickMallocArray method is:
%
%      void *MagickMallocArray(const size_t count, const size_t size);
%
%  A description of each parameter follows:
%
%    o count: The number of elements in the array.
%
%    o size: The size of one array element.
%
*/
MagickExport void *MagickMallocArray(const size_t count,const size_t size)
{
  size_t
    allocation_size;

  void
    *allocation;

  allocation = (void *) NULL;
  allocation_size=MagickArraySize(count,size);

  if (allocation_size)
    allocation = (MallocFunc)(allocation_size);
  return allocation;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k M a l l o c C l e a r e d                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickMallocCleared() returns a pointer to a block of memory of at least
%  size bytes suitably aligned for any use.  NULL is returned if insufficient
%  memory is available or the requested size is zero.  This version differs
%  from MagickMalloc in that the allocated bytes are cleared to zero.
%
%  The format of the  MagickMallocCleared method is:
%
%      void * MagickMallocCleared(const size_t size)
%
%  A description of each parameter follows:
%
%    o size: The size of the memory in bytes to allocate.
%
%
*/
MagickExport void * MagickMallocCleared(const size_t size)
{
  void
    *p = (void *) NULL;

  if (size != 0)
    {
      p=(MallocFunc)(size);

      if (p != (void *) NULL)
	(void) memset(p,0,size);
    }

  return p;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k C l o n e M e m o r y                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickCloneMemory() copies size bytes from memory area source to the
%  destination.  Copying between objects that overlap will take place
%  correctly.  It returns destination.
%
%  The format of the MagickCloneMemory method is:
%
%      void *MagickCloneMemory(void *destination,const void *source,
%                              const size_t size)
%
%  A description of each parameter follows:
%
%    o size: The size of the memory in bytes to allocate.
%
%
*/
MagickExport void *MagickCloneMemory(void *destination,const void *source,
  const size_t size)
{
  unsigned char
    *d=(unsigned char*) destination;

  const unsigned char
    *s=(const unsigned char*) source;

  assert(destination != (void *) NULL);
  assert(source != (const void *) NULL);

  if (((d+size) < s) || (d > (s+size)))
    return(memcpy(destination,source,size));

  return(memmove(destination,source,size));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k R e a l l o c                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickRealloc() changes the size of the memory and returns a pointer to
%  the (possibly moved) block.  The contents will be unchanged up to the
%  lesser of the new and old sizes.  If size is zero, then the memory is
%  freed and a NULL value is returned.  If the memory allocation fails, then
%  the existing memory is freed, and a NULL value is returned.
%
%  The format of the MagickRealloc method is:
%
%      void *MagickRealloc(void *memory,const size_t size)
%
%  A description of each parameter follows:
%
%    o memory: A pointer to a memory allocation.
%
%    o size: The new size of the allocated memory.
%
*/
MagickExport void *MagickRealloc(void *memory,const size_t size)
{
  void
    *new_memory = (void *) NULL;

  if ((void *) NULL == memory)
    new_memory = (MallocFunc)(size);
  else
    new_memory = (ReallocFunc)(memory,size);
  if ((new_memory == 0) && (memory != 0) && (size != 0))
    (FreeFunc)(memory);

  return new_memory;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%    M a g i c k F r e e                                                      %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickFree() frees memory that has already been allocated.  A NULL
%  argument is ignored.
%
%  The format of the MagickFree method is:
%
%      void MagickFree(void *memory)
%
%  A description of each parameter follows:
%
%    o memory: A pointer to a block of memory to free for reuse.
%
%
*/
MagickExport void MagickFree(void *memory)
{
  if (memory != (void *) NULL)
    (FreeFunc)(memory);
}
