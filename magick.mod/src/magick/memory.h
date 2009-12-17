/*
  Copyright (C) 2003 - 2009 GraphicsMagick Group
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  GraphicsMagick Memory Allocation Methods.
*/
#ifndef _MAGICK_MEMORY_H
#define _MAGICK_MEMORY_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

typedef void *(*MagickMallocFunc)(size_t size);
typedef void (*MagickFreeFunc)(void *ptr);
typedef void *(*MagickReallocFunc)(void *ptr, size_t size);

extern MagickExport void
   MagickAllocFunctions(MagickFreeFunc free_func,MagickMallocFunc malloc_func,
                        MagickReallocFunc realloc_func),
  *MagickMalloc(const size_t size) MAGICK_FUNC_MALLOC,
  *MagickMallocCleared(const size_t size) MAGICK_FUNC_MALLOC,
  *MagickCloneMemory(void *destination,const void *source,const size_t size) MAGICK_FUNC_NONNULL,
  *MagickRealloc(void *memory,const size_t size) MAGICK_FUNC_MALLOC,
   MagickFree(void *memory);

#if defined(MAGICK_IMPLEMENTATION)

extern MagickExport void
  *MagickMallocArray(const size_t count,const size_t size) MAGICK_FUNC_MALLOC;

extern MagickExport size_t
  MagickArraySize(const size_t count,const size_t size);

/*
  Allocate memory
*/

/* #define MagickAllocateMemory(type,size) ((type) MagickMalloc((size_t) (size))) */
#define MagickAllocateMemory(type,size) \
  ((((size) != ((size_t) (size))) || (size == 0)) ? ((type) 0) : \
   ((type) MagickMalloc((size_t) (size))))

#define MagickAllocateArray(type,count,size) \
  ( (type) MagickMallocArray(count,size) )

/*
  Free memory and set pointer to NULL
*/
#define MagickFreeMemory(memory) \
{ \
  void *_magick_mp=memory;      \
  MagickFree(_magick_mp);       \
  memory=0;                     \
}

/*
  Reallocate memory using provided pointer.  If reallocation fails
  then free memory, setting pointer to null.  If size is 0 and memory
  is not a null pointer, then free memory.  This interface behaves
  similar to realloc() except that memory is always freed (and pointer
  set to null) if a memory allocation failure occurs.
*/
#define MagickReallocMemory(type,memory,size)   \
{ \
    size_t _new_size = (size_t) (size); \
    void *_magick_mp = MagickRealloc(memory,_new_size); \
    memory=(type) _magick_mp; \
}

#endif /* defined(MAGICK_IMPLEMENTATION) */

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
