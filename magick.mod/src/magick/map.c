/*
% Copyright (C) 2003 GraphicsMagick Group
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
% Key,value associative map interface
% Written by Bob Friesenhahn, September 2003
%
*/

#include "magick/studio.h"
#include "magick/map.h"
#include "magick/utility.h"
#include "magick/semaphore.h"

/*
  Structure to represent a contained object
*/
typedef struct _MagickMapObject
{
  /* Object key value */
  char
   *key;

  /* Pointer to stored object */
  void
    *object;

  /* Object size. Optional.  May be set to zero depending on needs of
     object type */
  size_t
    object_size;

  /* Function which clones (copies) contained objects */
  MagickMapObjectClone
    clone_function;

  /* Function which deallocates contained object */
  MagickMapObjectDeallocator
    deallocate_function;

  /* Number of references to this object. */
  long
    reference_count;

  struct _MagickMapObject
    *previous,
    *next;

  unsigned long
    signature;

} MagickMapObject;

/*
  Structure to act as the user handle to a key,value map
*/
typedef struct _MagickMapHandle
{
  /* Function which clones (copies) contained objects */
  MagickMapObjectClone
    clone_function;

  /* Function which deallocates contained object */
  MagickMapObjectDeallocator
    deallocate_function;

  /* Global semaphore for all operations pertaining to this map */
  SemaphoreInfo
    *semaphore;

  /* Number of references to this object. */
  long
    reference_count;

  MagickMapObject
    *list;

  unsigned long
    signature;

} MagickMapHandle;

/*
  Iterator position. Used to manage iterator state.
*/
typedef enum
{
  InListPosition,
  FrontPosition,
  BackPosition
} MagickMapIteratorPosition;

/*
  Structure to act as a map iterator
*/
typedef struct _MagickMapIteratorHandle
{
  /* Pointer to base map */
  MagickMap
    map;

  /* Pointer to member of map */
  const MagickMapObject
    *member;

  /* Iterator position */
  MagickMapIteratorPosition
    position;

  unsigned long
    signature;
} MagickMapIteratorHandle;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k M a p A l l o c a t e O b j e c t                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickMapAllocateObject() allocates a new object for inclusion in
%  the map.
%
%  The format of the MagickMapAllocateObject method is:
%
%      MagickMapObject *MagickMapAllocateObject(const char *key,
%        const void *object, const size_t object_size,
%        MagickMapObjectClone clone, MagickMapObjectDeallocator deallocate)
%
%  A description of each parameter follows:
%
%    o key: object unique key.
%
%    o object: Object value
%
%    o object_size: Object size.  Optional. May be set to zero if the
%                       object size is known through some other means.
%
%    o clone: function to create a copy of this object type.
%
%    o deallocate: function to deallocate this object type.
%
*/
static MagickMapObject *
MagickMapAllocateObject(const char *key, const void *object,
  const size_t object_size, MagickMapObjectClone clone,
  MagickMapObjectDeallocator deallocate)
{
  MagickMapObject *
    map_object;

  assert(key != 0);
  assert(object != 0);
  assert(clone != 0);
  assert(deallocate != 0);

  map_object=MagickAllocateMemory(MagickMapObject *,sizeof(MagickMapObject));
  if (map_object)
    {
      map_object->key=AcquireString(key);
      map_object->object=(clone)(object,object_size);
      map_object->object_size=object_size;
      map_object->clone_function=clone;
      map_object->deallocate_function=deallocate;
      map_object->reference_count=1;
      map_object->previous=0;
      map_object->next=0;
      map_object->signature=MagickSignature;
    }

  return map_object;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k M a p D e s t r o y O b j e c t                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickMapDestroyObject deallocates a map object by invoking the
%  object's embedded deallocation function.
%
%  The format of the MagickMapDestroyObject method is:
%
%      void MagickMapDestroyObject(MagickMapObject *object)
%
%  A description of each parameter follows:
%
%    o object: Pointer to object to destroy.
%
*/
static void
MagickMapDestroyObject(MagickMapObject *object)
{
  assert(object != 0);
  assert(object->signature == MagickSignature);

  object->reference_count--;
  assert(object->reference_count == 0);

  MagickFreeMemory(object->key);
  (object->deallocate_function)(object->object);

  (void) memset((void *)object,0xbf,sizeof(MagickMapObject));
  MagickFreeMemory(object);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k M a p A c c e s s E n t r y                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickMapAccessEntry() searches for an object in the map identified
%  by the specified key. If a matching object is found, then a const
%  pointer to the object data is returned, and the object_size
%  argument is updated with the object size. Null is returned if no
%  matching object is found.
%
%  An object is not required to contain a size so object_size may be
%  updated to zero. If the object size is known to not be required by
%  this object type, then a null object_size pointer may be passed.
%
%  The format of the MagickMapAccessEntry method is:
%
%      const void *MagickMapAccessEntry(MagickMap map,const char *key,
%                                       size_t *object_size)
%
%  A description of each parameter follows:
%
%    o map: map context
%
%    o key: unique key to match
%
%    o object_size: Pointer to where object size is to be saved.
%
*/
MagickExport const void *
MagickMapAccessEntry(MagickMap map,const char *key, size_t *object_size)
{
  MagickMapObject
    *p;

  assert(map != 0);
  assert(map->signature == MagickSignature);
  assert(key != 0);

  if (object_size)
    *object_size=0;

  (void) LockSemaphoreInfo(map->semaphore);

  for (p=map->list; p != 0; p=p->next)
    if (LocaleCompare(key,p->key) == 0)
      {
        if (object_size)
          *object_size=p->object_size;
        (void) UnlockSemaphoreInfo(map->semaphore);
        return(p->object);
      }

  (void) UnlockSemaphoreInfo(map->semaphore);

  return 0;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k M a p A d d E n t r y                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickMapAddEntry() adds a new entry to a map, or replaces an existing
%  matching entry.  True is returned on success.  If False is returned, then
%  the exception argument describes the reason for failure.
%
%  The format of the MagickMapAddEntry method is:
%
%      void MagickMapAddEntry(MagickMap map,const char *key,
%                             const void *object, const size_t object_size)
%
%  A description of each parameter follows:
%
%    o map: map context
%
%    o key: unique key
%
%    o object: pointer to object data to copy.
%
%    o object_size: size of object data. If the copy function does not
%        require the object size, then the value zero may be used.
%
%    o exception: check this argument for error information if False is
%                 returned.
%
*/
MagickExport unsigned int
MagickMapAddEntry(MagickMap map,const char *key, const void *object,
  const size_t object_size, ExceptionInfo *exception)
{
  MagickMapObject
    *new_object;

  assert(map != 0);
  assert(map->signature == MagickSignature);
  assert(key != 0);
  assert(object != 0);

  new_object=MagickMapAllocateObject(key, object, object_size,
                                     map->clone_function,
                                     map->deallocate_function);

  if (!new_object)
    {
      if (exception)
        ThrowException(exception,ResourceLimitError,MemoryAllocationFailed,
          NULL);
      return (False);
    }

  (void) LockSemaphoreInfo(map->semaphore);

  if (!map->list)
    {
      /*
        If list is empty, object becomes new list.
      */
      map->list=new_object;
    }
  else
    {
      MagickMapObject
        *last_object=0,
        *p;

      /*
        Search for, and replace any existing object with
        same key.
      */

      for (p=map->list; p != 0; p=p->next)
        {
          last_object=p;
          if (LocaleCompare(key,p->key) == 0)
            {
              /*
                Splice in new object
              */
              new_object->previous=p->previous;
              new_object->next=p->next;
              if (new_object->previous)
                new_object->previous->next=new_object;
              if (new_object->next)
                new_object->next->previous=new_object;
              if (map->list == p)
                map->list=new_object;
              p->previous=0;
              p->next=0;
              
              /*
                Remove old object
              */
              MagickMapDestroyObject(p);
              break;
            }
        }

      if (p == 0)
        {
          /*
            No existing matching object was found.  The last_object
            pointer points to the last object in the list.  Append new
            object to end of list.
          */
          new_object->previous=last_object;
          last_object->next=new_object;
        }
    }

  (void) UnlockSemaphoreInfo(map->semaphore);

  return (True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k M a p A l l o c a t e M a p                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickMapAllocateMap() allocates a new map context.  The address of
%  the functions required to copy and deallocate contained object data
%  are provided when the map context is constructed. Objects contained
%  in a map are all of the same type. A null map context is returned if
%  the map construction fails.
%
%  The format of the MagickMapAllocateMap method is:
%
%      MagickMap MagickMapAllocateMap(MagickMapObjectClone clone,
%                                     MagickMapObjectDeallocator deallocate)
%
%  A description of each parameter follows:
%
%    o clone: Pointer to function which copies contained object data.
%
%    o deallocate: Pointer to function which deallocates contained object
%                  data.
%
*/
MagickExport MagickMap
MagickMapAllocateMap(MagickMapObjectClone clone,
  MagickMapObjectDeallocator deallocate)
{
  MagickMap
    map;

  assert(clone != 0);
  assert(deallocate != 0);

  map=MagickAllocateMemory(MagickMap,sizeof(MagickMapHandle));
  if (map)
    {
      map->clone_function=clone;
      map->deallocate_function=deallocate;
      map->semaphore=AllocateSemaphoreInfo();
      map->reference_count=1;
      map->list=0;
      map->signature=MagickSignature;
    }

  return map;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k M a p C l o n e M a p                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickMapCloneMap() duplicates an existing map.  If the duplication fails,
%  zero is returned and the error argument is updated.
%
%  The format of the MagickMapCloneMap method is:
%
%      MagickMap MagickMapCloneMap(MagickMap map)
%
%  A description of each parameter follows:
%
%    o map: map context to duplicate
%
%    o exception: check this structure for error details if null is returned.
%
*/
MagickExport MagickMap
MagickMapCloneMap(MagickMap map,ExceptionInfo *exception)
{
  MagickMap map_clone;
  MagickMapIterator iterator;
  size_t size;
  const char *key;

  assert(map != 0);
  assert(map->signature == MagickSignature);

  /*
    Don't lock. Iterator locks the map. PTHREADS doesn't
    allow one thread multiple locks on one semaphore.
  */
  /* LockSemaphoreInfo(map->semaphore); */

  map_clone=MagickMapAllocateMap(map->clone_function,map->deallocate_function);
  iterator=MagickMapAllocateIterator(map);
  while(MagickMapIterateNext(iterator,&key))
  {
    const void *object=MagickMapDereferenceIterator(iterator,&size);
    /* Add clones key and object on insertion */
    if (MagickMapAddEntry(map_clone,key,object,size,exception) == False)
      {
        MagickMapDeallocateIterator(iterator);
        MagickMapDeallocateMap(map_clone);
        return 0;
      }
  }
  MagickMapDeallocateIterator(iterator);

  return map_clone;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k M a p C l e a r M a p                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickMapClearMap() removes all entries from a map.  The deallocator
%  function for each contained object is invoked when it is removed.
%
%  The format of the MagickMapClearMap method is:
%
%       void MagickMapClearMap(MagickMap map)
%
%  A description of each parameter follows:
%
%    o map: map context to clear
%
*/
MagickExport void
MagickMapClearMap(MagickMap map)
{
  assert(map != 0);
  assert(map->signature == MagickSignature);

  (void) LockSemaphoreInfo(map->semaphore);
  if (map->list)
  {
    register MagickMapObject
      *current,
      *p;

    for (p=map->list; p != 0; )
      {
        current=p;
        p=p->next;
        MagickMapDestroyObject(current);
      }
    map->list=0;
  }
  (void) UnlockSemaphoreInfo(map->semaphore);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k M a p A l l o c a t e I t e r a t o r                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickMapAllocateIterator allocates an iterator which may be used to
%  traverse all the entries in the map. The iterator initially points to
%  the front of the map.
%
%  The format of the MagickMapAllocateIterator method is:
%
%       MagickMapIterator MagickMapAllocateIterator(MagickMap map)
%
%  A description of each parameter follows:
%
%    o map: map context
%
*/
MagickExport MagickMapIterator
MagickMapAllocateIterator(MagickMap map)
{
  MagickMapIterator
    iterator;

  assert(map != 0);
  assert(map->signature == MagickSignature);

  LockSemaphoreInfo(map->semaphore);

  iterator=MagickAllocateMemory(MagickMapIterator,
                                sizeof(MagickMapIteratorHandle));
  if (iterator)
    {
      iterator->map=map;
      iterator->member=0;
      iterator->position=FrontPosition;
      iterator->map->reference_count++;
      iterator->signature=MagickSignature;
    }

  UnlockSemaphoreInfo(map->semaphore);

  return iterator;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k M a p D e a l l o c a t e M a p                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickMapDeallocateMap() deallocates a map, invoking the map deallocation
%  function on each entry contained in the map.
%
%  The format of the MagickMapDeallocateMap method is:
%
%       void MagickMapDeallocateMap(MagickMap map)
%
%  A description of each parameter follows:
%
%    o map: map context to destroy
%
*/
MagickExport void
MagickMapDeallocateMap(MagickMap map)
{
  assert(map != 0);
  assert(map->signature == MagickSignature);

  (void) LockSemaphoreInfo(map->semaphore);

  map->reference_count--;

  /* For the moment, do not allow outstanding references */
  assert(map->reference_count == 0);

  if (map->list)
  {
    register MagickMapObject
      *current,
      *p;

    for (p=map->list; p != 0; )
      {
        current=p;
        p=p->next;
        MagickMapDestroyObject(current);
      }        
  }

  (void) UnlockSemaphoreInfo(map->semaphore);
  DestroySemaphoreInfo(&map->semaphore);

  (void) memset((void *)map,0xbf,sizeof(MagickMapHandle));
  MagickFreeMemory(map);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k M a p D e a l l o c a t e I t e r a t o r                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickMapDeallocateIterator() deallocates a map iterator
%
%  The format of the MagickMapDeallocateIterator method is:
%
%       void MagickMapDeallocateIterator(MagickMapIterator iterator)
%
%  A description of each parameter follows:
%
%    o iterator: iterator context to destroy
%
*/
MagickExport void
MagickMapDeallocateIterator(MagickMapIterator iterator)
{
  assert(iterator != 0);
  assert(iterator->signature == MagickSignature);

  (void) LockSemaphoreInfo(iterator->map->semaphore);

  iterator->map->reference_count--;

  (void) UnlockSemaphoreInfo(iterator->map->semaphore);

  (void) memset((void *)iterator,0xbf,sizeof(MagickMapIteratorHandle));
  MagickFreeMemory(iterator);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k M a p D e r e f e r e n c e I t e r a t o r                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickMapDereferenceIterator() dereferences a map iterator, providing
%  access to the contained data and its size. The object_size parameter may
%  be passed a null pointer if the object size is not required.
%
%  The format of the MagickMapDereferenceIterator method is:
%
%       const void *MagickMapDereferenceIterator(
%            const MagickMapIterator iterator, size_t *object_size)
%
%  A description of each parameter follows:
%
%    o iterator: iterator context
%
%    o object_size: contained object size (if provided)
%
*/
MagickExport const void *
MagickMapDereferenceIterator(const MagickMapIterator iterator,
    size_t *object_size)
{
  const void
    *value=0;

  assert(iterator != 0);
  assert(iterator->signature == MagickSignature);
  assert(iterator->member != 0);

  if (object_size)
    *object_size=0;

  if (iterator->member)
    {
      value=iterator->member->object;
      if (object_size)
        *object_size=iterator->member->object_size;
    }

  return value;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k M a p I t e r a t e T o B a c k                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickMapIterateToBack() sets the iterator to one position beyond the
%  last element in the map. The MagickMapIteratePrevious() function must
%  be executed once to access the last element in the map.
%
%  The format of the MagickMapIterateToBack method is:
%
%       void MagickMapIterateToBack(MagickMapIterator iterator)
%
%  A description of each parameter follows:
%
%    o iterator: iterator context
%
*/
MagickExport void
MagickMapIterateToBack(MagickMapIterator iterator)
{
  assert(iterator != 0);
  assert(iterator->signature == MagickSignature);

  iterator->member=0;
  iterator->position=BackPosition;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k M a p I t e r a t e T o F r o n t                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickMapIterateToFront() sets the iterator to one position before the
%  first element in the map. The MagickMapIterateNext() function must
%  be executed once to access the first element in the map.
%
%  The format of the MagickMapIterateToFront method is:
%
%       void MagickMapIterateToFront(MagickMapIterator iterator)
%
%  A description of each parameter follows:
%
%    o iterator: iterator context
%
*/
MagickExport void
MagickMapIterateToFront(MagickMapIterator iterator)
{
  assert(iterator != 0);
  assert(iterator->signature == MagickSignature);

  iterator->member=0;
  iterator->position=FrontPosition;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k M a p I t e r a t e N e x t                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickMapIterateNext() advances the iterator by one element. True is
%  returned if there are remaining entries in the map. The key argument
%  is updated to point to the element key.
%
%  The format of the MagickMapIterateNext method is:
%
%       void MagickMapIterateToFront(MagickMapIterator iterator)
%
%  A description of each parameter follows:
%
%    o iterator: iterator context
%
%    o key: pointer to location to store a pointer to the key
%
*/
MagickExport unsigned int
MagickMapIterateNext(MagickMapIterator iterator,const char **key)
{
  assert(iterator != 0);
  assert(iterator->signature == MagickSignature);
  assert(key != 0);

  LockSemaphoreInfo(iterator->map->semaphore);

  switch (iterator->position)
    {
    case FrontPosition:
      iterator->member=iterator->map->list;
      if (iterator->member)
        iterator->position=InListPosition;
      break;
    case InListPosition:
      assert(iterator->member != 0);
      iterator->member=iterator->member->next;
      if (!iterator->member)
        iterator->position=BackPosition;
      break;
    case BackPosition:
      break;
    }

  if (iterator->member)
    *key=iterator->member->key;

  UnlockSemaphoreInfo(iterator->map->semaphore);
  
  return (iterator->member != 0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k M a p I t e r a t e P r e v i o u s                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickMapIteratePrevious() moves the iterator by one element in the
%  reverse direction. True is returned if there are remaining entries
%  in the map prior to the entry. The key argument is updated to point
%  to the element key.
%
%  The format of the MagickMapIteratePrevious method is:
%
%       void MagickMapIterateToPrevious(MagickMapIterator iterator)
%
%  A description of each parameter follows:
%
%    o iterator: iterator context
%
%    o key: pointer to location to store a pointer to the key
%
*/
MagickExport unsigned int
MagickMapIteratePrevious(MagickMapIterator iterator,const char **key)
{
  assert(iterator != 0);
  assert(iterator->signature == MagickSignature);
  assert(key != 0);

  LockSemaphoreInfo(iterator->map->semaphore);

  switch (iterator->position)
    {
    case FrontPosition:
      break;
    case InListPosition:
      assert(iterator->member != 0);
      iterator->member=iterator->member->previous;
      if (!iterator->member)
        iterator->position=FrontPosition;
      break;
    case BackPosition:
      {
        for (iterator->member=iterator->map->list;
             (iterator->member != 0) && (iterator->member->next != 0);
             iterator->member=iterator->member->next);
        if (iterator->member)
          iterator->position=InListPosition;
      }
      break;
    }

  if (iterator->member)
    *key=iterator->member->key;

  UnlockSemaphoreInfo(iterator->map->semaphore);

  return (iterator->member != 0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k M a p R e m o v e E n t r y                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickMapRemoveEntry() removes (destroys) an entry from the map. The
%  deallocate function is invoked on the entry data. False is returned if
%  a matching key doesn't exist.
%
%  The format of the MagickMapRemoveEntry method is:
%
%       unsigned int MagickMapRemoveEntry(MagickMap map,const char *key)
%
%  A description of each parameter follows:
%
%    o map: map context
%
%    o key: key corresponding to the entry to remove
%
*/
MagickExport unsigned int
MagickMapRemoveEntry(MagickMap map,const char *key)
{
  unsigned int
    status = False;

  assert(map != 0);
  assert(map->signature == MagickSignature);
  assert(key != 0);

  LockSemaphoreInfo(map->semaphore);

  if (map->list)
    {
      MagickMapObject
        *p;

      for (p=map->list; p != 0; p=p->next)
        {
          if (LocaleCompare(key,p->key) == 0)
            {
              if (p == map->list)
                {
                  if (!p->next)
                    {
                      map->list=0;
                    }
                  else
                    {
                      map->list=p->next;
                      p->next->previous=0;
                    }
                }
              else
                {
                  if (p->previous)
                    p->previous->next=p->next;
                  
                  if (p->next)
                    p->next->previous=p->previous;
                }

              MagickMapDestroyObject(p);
              status=True;
              break;
            }
        }
    }

  UnlockSemaphoreInfo(map->semaphore);

  return status;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k M a p C o p y S t r i n g                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickMapCopyString() copies a string. It is intended for use as the
%  clone function for strings so that C strings may easily be stored in
%  a map.
%
%  The format of the MagickMapCopyString method is:
%
%       void *MagickMapCopyString(const void *string, const size_t size)
%
%  A description of each parameter follows:
%
%    o string: pointer to string data
%
%    o size: ignored by this method.
%
*/
MagickExport void *
MagickMapCopyString(const void *string, const size_t ARGUNUSED(size))
{
  if (string)
    return (void *) AcquireString((const char *)string);
  return 0;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k M a p D e a l l o c a t e S t r i n g                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickMapDeallocateString() deallocates a string. It is intended for
%  use as the deallocate function for strings so that C strings may easily
%  be stored in a map.
%
%  The format of the MagickMapDeallocateString method is:
%
%       void MagickMapDeallocateString(void *string)
%
%  A description of each parameter follows:
%
%    o string: pointer to string data to deallocate
%
*/
extern MagickExport void
MagickMapDeallocateString(void *string)
{
  MagickFreeMemory(string);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k M a p C o p y B l o b                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickMapCopyBlob() copies a BLOB. It is intended for use as the
%  clone function for BLOBs so that BLOB may easily be stored in
%  a map.
%
%  The format of the MagickMapCopyBlob method is:
%
%       void *MagickMapCopyBlob(const void *blob, const size_t size)
%
%  A description of each parameter follows:
%
%    o blob: pointer to BLOB data
%
%    o size: BLOB size
%
*/
MagickExport void *
MagickMapCopyBlob(const void *blob, const size_t size)
{
  if (blob)
    {
      void
        *memory;

      memory=MagickAllocateMemory(void *,size);
      if (memory)
        (void) memcpy(memory,blob,size);
      return (memory);
    }
  return 0;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k M a p D e a l l o c a t e B l o b                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickMapDeallocateBlob() deallocates a BLOB. It is intended for
%  use as the deallocate function for BLOBs so that BLOBs may easily
%  be stored in a map.
%
%  The format of the MagickMapDeallocateBlob method is:
%
%       void MagickMapDeallocateBlob(void *blob)
%
%  A description of each parameter follows:
%
%    o blob: pointer to BLOB data to deallocate
%
*/
extern MagickExport void
MagickMapDeallocateBlob(void *blob)
{
  MagickFreeMemory(blob);
}
