/*
% Copyright (C) 2003-2012 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%        SSSSS  EEEEE  M   M   AAA   PPPP   H   H   OOO   RRRR   EEEEE        %
%        SS     E      MM MM  A   A  P   P  H   H  O   O  R   R  E            %
%         SSS   EEE    M M M  AAAAA  PPPP   HHHHH  O   O  RRRR   EEE          %
%           SS  E      M   M  A   A  P      H   H  O   O  R R    E            %
%        SSSSS  EEEEE  M   M  A   A  P      H   H   OOO   R  R   EEEEE        %
%                                                                             %
%                                                                             %
%                     GraphicsMagick Semaphore Methods                        %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                             William Radcliffe                               %
%                                John Cristy                                  %
%                                 June 2000                                   %
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

#if defined(HAVE_PTHREAD)
#  define USE_POSIX_THREADS 1
#elif defined(MSWINDOWS)
#  define USE_WIN32_THREADS 1
#endif

#if defined(USE_POSIX_THREADS)
#  include <pthread.h>
#  define PTHREAD_MUTEX_DESTROY(semaphore_mutex)			\
  {									\
    int									\
      err_status;							\
    									\
    if ((err_status = pthread_mutex_destroy(semaphore_mutex)) != 0)	\
      {									\
        errno=err_status;						\
	MagickFatalError3(ResourceLimitFatalError,SemaporeOperationFailed, \
			  UnableToDestroySemaphore);			\
      }									\
  }
#  define PTHREAD_MUTEXATTR_DESTROY(mutexattr)				\
  {									\
    int									\
      err_status;							\
    									\
    if ((err_status = pthread_mutexattr_destroy(mutexattr)) != 0)	\
      {									\
        errno=err_status;						\
	MagickFatalError3(ResourceLimitFatalError,SemaporeOperationFailed, \
			  UnableToDestroySemaphore);			\
      }									\
  }
#  define PTHREAD_MUTEXATTR_INIT(mutexattr)				\
  {									\
    int									\
      err_status;							\
    									\
    if ((err_status = pthread_mutexattr_init(mutexattr)) != 0)		\
      {									\
        errno=err_status;						\
	MagickFatalError3(ResourceLimitFatalError,SemaporeOperationFailed, \
			  UnableToInitializeSemaphore);			\
      }									\
  }
#  define PTHREAD_MUTEXATTR_SETTYPE(mutexattr,mutexattrtype)		\
  {									\
    int									\
      err_status;							\
    									\
    if ((err_status = pthread_mutexattr_settype(mutexattr,mutexattrtype)) != 0) \
      {									\
        errno=err_status;						\
	MagickFatalError3(ResourceLimitFatalError,SemaporeOperationFailed, \
			  UnableToInitializeSemaphore);			\
      }									\
  }
#  define PTHREAD_MUTEX_INIT(semaphore_mutex,mutexattr)			\
  {									\
    int									\
      err_status;							\
    									\
    if ((err_status = pthread_mutex_init(semaphore_mutex,mutexattr)) != 0) \
      {									\
        errno=err_status;						\
	MagickFatalError3(ResourceLimitFatalError,SemaporeOperationFailed, \
			  UnableToInitializeSemaphore);			\
      }									\
  }
#  define PTHREAD_MUTEX_LOCK(semaphore_mutex)				\
  {									\
    int									\
      err_status;							\
    									\
    if ((err_status = pthread_mutex_lock(semaphore_mutex)) != 0)	\
      {									\
        errno=err_status;						\
	MagickFatalError3(ResourceLimitFatalError,SemaporeOperationFailed, \
			  UnableToLockSemaphore);			\
      }									\
  }
#  define PTHREAD_MUTEX_UNLOCK(semaphore_mutex)				\
  {									\
    int									\
      err_status;							\
    									\
    if ((err_status = pthread_mutex_unlock(semaphore_mutex)) != 0)	\
      {									\
        errno=err_status;						\
	MagickFatalError3(ResourceLimitFatalError,SemaporeOperationFailed, \
			  UnableToUnlockSemaphore);			\
      }									\
  }
#endif

#if defined(USE_WIN32_THREADS)
#  include <windows.h>
#  define USE_SPINLOCKS
#  define SPINLOCK_DELAY_MILLI_SECS 10
#endif

#include "magick/semaphore.h"

/*
  Struct declaractions.
*/
struct _SemaphoreInfo
{
#if defined(USE_POSIX_THREADS)
  pthread_mutex_t
    mutex;		/* POSIX thread mutex */
#endif
#if defined(USE_WIN32_THREADS)
  CRITICAL_SECTION
    mutex;		/* Windows critical section */
#endif

  unsigned long
    signature;		/* Used to validate structure */
};

/*
  Static declaractions.
*/
#if defined(USE_POSIX_THREADS)
static pthread_mutex_t
  semaphore_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

#if defined(USE_WIN32_THREADS)
#if !defined(USE_SPINLOCKS)
static CRITICAL_SECTION
  semaphore_mutex;

static unsigned int
  active_semaphore = MagickFalse;
#else
static LONG
  semaphore_mutex = 0;
/* Wait for spin lock */
static void spinlock_wait (LONG volatile *sl)
{
  /*
    InterlockedCompareExchange performs an atomic comparison of the
    specified 32-bit values and exchanges them, based on the outcome of
    the comparison. Requires Windows XP, Windows 2000 Professional,
    Windows NT Workstation 4.0, Windows Me, or Windows 98.
   */

  while (InterlockedCompareExchange (sl, 1L, 0L) != 0)
  {
    /* slight delay - just in case OS does not giveup CPU */
    Sleep (SPINLOCK_DELAY_MILLI_SECS);
  }
}
/* Release spin lock */
static void spinlock_release (LONG volatile *sl)
{
  /*
    InterlockedExchange atomically exchanges a pair of 32-bit
    values. Requires Windows XP, Windows 2000 Professional, Windows NT
    Workstation 3.5 and later, Windows Me, Windows 98, or Windows 95.
  */
  InterlockedExchange (sl, 0L);
}
#endif
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A c q u i r e S e m a p h o r e I n f o                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AcquireSemaphoreInfo() locks a semaphore, initializing it first if
%  necessary.
%
%  The format of the AcquireSemaphoreInfo method is:
%
%      void AcquireSemaphoreInfo(SemaphoreInfo **semaphore_info)
%
%  A description of each parameter follows:
%
%    o semaphore_info: Specifies a pointer to an SemaphoreInfo structure.
%
%
*/
MagickExport void AcquireSemaphoreInfo(SemaphoreInfo **semaphore_info)
{
  assert(semaphore_info != (SemaphoreInfo **) NULL);
#if defined(USE_POSIX_THREADS)
  PTHREAD_MUTEX_LOCK(&semaphore_mutex);
#endif /* defined(USE_POSIX_THREADS) */
#if defined(USE_WIN32_THREADS)
#if !defined(USE_SPINLOCKS)
  if (!active_semaphore)
    InitializeCriticalSection(&semaphore_mutex);
  active_semaphore=MagickTrue;
  EnterCriticalSection(&semaphore_mutex);
#else
  spinlock_wait(&semaphore_mutex);
#endif
#endif
  if (*semaphore_info == (SemaphoreInfo *) NULL)
    *semaphore_info=AllocateSemaphoreInfo();
#if defined(USE_POSIX_THREADS)
  PTHREAD_MUTEX_UNLOCK(&semaphore_mutex);
#endif
#if defined(USE_WIN32_THREADS)
#if !defined(USE_SPINLOCKS)
  LeaveCriticalSection(&semaphore_mutex);
#else
  spinlock_release(&semaphore_mutex);
#endif
#endif
  (void) LockSemaphoreInfo(*semaphore_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A l l o c a t e S e m a p h o r e I n f o                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method AllocateSemaphoreInfo initializes the SemaphoreInfo structure.
%
%  The format of the AllocateSemaphoreInfo method is:
%
%      SemaphoreInfo *AllocateSemaphoreInfo(void)
%
%  A description of each parameter follows:
%
%    o semaphore_info: Method AllocateSemaphoreInfo returns a pointer to an
%      initialized SemaphoreInfo structure.
%
%
*/
MagickExport SemaphoreInfo *AllocateSemaphoreInfo(void)
{
  SemaphoreInfo
    *semaphore_info;

  /*
    Allocate semaphore.
  */
  semaphore_info=MagickAllocateAlignedMemory(SemaphoreInfo *,
					     MAGICK_CACHE_LINE_SIZE,
					     sizeof(SemaphoreInfo));
  if (semaphore_info == (SemaphoreInfo *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToAllocateSemaphoreInfo);
  (void) memset(semaphore_info,0,sizeof(SemaphoreInfo));
  /*
    Initialize the semaphore.
  */
#if defined(USE_POSIX_THREADS)
  {
    pthread_mutexattr_t 
      mutexattr;

    PTHREAD_MUTEXATTR_INIT(&mutexattr);

    /*
      If MAGICK_DEBUG is defined then enable pthread mutex error
      checks.
    */
#if MAGICK_DEBUG
#if defined(PTHREAD_MUTEX_ERRORCHECK)
    PTHREAD_MUTEXATTR_SETTYPE(&mutexattr, PTHREAD_MUTEX_ERRORCHECK);
#endif /* PTHREAD_MUTEX_ERRORCHECK */
#endif /* MAGICK_DEBUG */

    PTHREAD_MUTEX_INIT(&semaphore_info->mutex,&mutexattr);
    PTHREAD_MUTEXATTR_DESTROY(&mutexattr);
  }
#endif
#if defined(USE_WIN32_THREADS)
  InitializeCriticalSection(&semaphore_info->mutex);
#endif
  
  semaphore_info->signature=MagickSignature;
  return(semaphore_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y S e m a p h o r e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroySemaphore() destroys the semaphore environment.
%
%  The format of the DestroySemaphore method is:
%
%      void DestroySemaphore(void)
%
%
*/
MagickExport void DestroySemaphore(void)
{
#if defined(USE_POSIX_THREADS)
  /*
    We use static pthread mutex initialization with
    PTHREAD_MUTEX_INITIALIZER so semaphore mutex should not be
    destroyed.
  */
  /* PTHREAD_MUTEX_DESTROY(&semaphore_mutex); */
#endif
#if defined(USE_WIN32_THREADS)
#if !defined(USE_SPINLOCKS)
  if (active_semaphore)
    DeleteCriticalSection(&semaphore_mutex);
  active_semaphore=MagickFalse;
#endif
#endif
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y S e m a p h o r e I n f o                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroySemaphoreInfo destroys a semaphore.
%
%  The format of the DestroySemaphoreInfo method is:
%
%      void DestroySemaphoreInfo(SemaphoreInfo **semaphore_info)
%
%  A description of each parameter follows:
%
%    o semaphore_info: Specifies a pointer to an SemaphoreInfo structure.
%
%
*/
MagickExport void DestroySemaphoreInfo(SemaphoreInfo **semaphore_info)
{
  assert(semaphore_info != (SemaphoreInfo **) NULL);
  if (*semaphore_info == (SemaphoreInfo *) NULL)
    return;
  assert((*semaphore_info)->signature == MagickSignature);
#if defined(USE_POSIX_THREADS)
  PTHREAD_MUTEX_LOCK(&semaphore_mutex);
#endif
#if defined(USE_WIN32_THREADS)
#if !defined(USE_SPINLOCKS)
  if (!active_semaphore)
    InitializeCriticalSection(&semaphore_mutex);
  active_semaphore=MagickTrue;
  EnterCriticalSection(&semaphore_mutex);
#else
  spinlock_wait(&semaphore_mutex);
#endif
#endif
#if defined(USE_POSIX_THREADS)
  PTHREAD_MUTEX_DESTROY(&(*semaphore_info)->mutex);
#endif
#if defined(USE_WIN32_THREADS)
  DeleteCriticalSection(&(*semaphore_info)->mutex);
#endif
  (void) memset((void *) *semaphore_info,0xbf,sizeof(SemaphoreInfo));
  MagickFreeAlignedMemory((*semaphore_info));
#if defined(USE_POSIX_THREADS)
  PTHREAD_MUTEX_UNLOCK(&semaphore_mutex);
#endif
#if defined(USE_WIN32_THREADS)
#if !defined(USE_SPINLOCKS)
  LeaveCriticalSection(&semaphore_mutex);
#else
  spinlock_release(&semaphore_mutex);
#endif
#endif
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I n i t i a l i z e S e m a p h o r e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method InitializeSemaphore initializes the semaphore environment.
%
%  The format of the InitializeSemaphore method is:
%
%      void InitializeSemaphore(void)
%
%
*/
MagickExport void InitializeSemaphore(void)
{
#if defined(USE_POSIX_THREADS)
  /*
    We use static pthread mutex initialization with
    PTHREAD_MUTEX_INITIALIZER so explicit runtime initialization is
    not required.
  */
/*   (void) pthread_mutex_init(&semaphore_mutex, */
/*     (const pthread_mutexattr_t *) NULL); */
#endif
#if defined(USE_WIN32_THREADS)
#if !defined(USE_SPINLOCKS)
  if (!active_semaphore)
    InitializeCriticalSection(&semaphore_mutex);
  active_semaphore=MagickTrue;
#endif
#endif
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L i b e r a t e S e m a p h o r e I n f o                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method LiberateSemaphoreInfo unlocks the semaphore if it has been allocated.
%
%  The format of the LiberateSemaphoreInfo method is:
%
%      void LiberateSemaphoreInfo(SemaphoreInfo **semaphore_info)
%
%  A description of each parameter follows:
%
%    o semaphore_info: Specifies a pointer to an SemaphoreInfo structure.
%
%
*/
MagickExport void LiberateSemaphoreInfo(SemaphoreInfo **semaphore_info)
{
  assert(semaphore_info != (SemaphoreInfo **) NULL);
  if (*semaphore_info != (SemaphoreInfo *) NULL)
    {
      assert((*semaphore_info)->signature == MagickSignature);
      (void) UnlockSemaphoreInfo(*semaphore_info);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L o c k S e m a p h o r e I n f o                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method LockSemaphoreInfo locks a semaphore.
%
%  The format of the LockSemaphoreInfo method is:
%
%      void LockSemaphoreInfo(SemaphoreInfo *semaphore_info)
%
%  A description of each parameter follows:
%
%    o semaphore_info: Specifies a pointer to an SemaphoreInfo structure.
%
%
*/
MagickExport void LockSemaphoreInfo(SemaphoreInfo *semaphore_info)
{
  assert(semaphore_info != (SemaphoreInfo *) NULL);
  assert(semaphore_info->signature == MagickSignature);
#if defined(USE_POSIX_THREADS)
  PTHREAD_MUTEX_LOCK(&semaphore_info->mutex);
#endif
#if defined(USE_WIN32_THREADS)
  EnterCriticalSection(&semaphore_info->mutex);
#endif
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n l o c k S e m a p h o r e I n f o                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnlockSemaphoreInfo unlocks a semaphore.
%
%  The format of the LockSemaphoreInfo method is:
%
%      void UnlockSemaphoreInfo(SemaphoreInfo *semaphore_info)
%
%  A description of each parameter follows:
%
%    o semaphore_info: Specifies a pointer to an SemaphoreInfo structure.
%
%
*/
MagickExport void UnlockSemaphoreInfo(SemaphoreInfo *semaphore_info)
{
  assert(semaphore_info != (SemaphoreInfo *) NULL);
  assert(semaphore_info->signature == MagickSignature);

#if defined(USE_POSIX_THREADS)
  PTHREAD_MUTEX_UNLOCK(&semaphore_info->mutex);
#endif
#if defined(USE_WIN32_THREADS)
  LeaveCriticalSection(&semaphore_info->mutex);
#endif
}
