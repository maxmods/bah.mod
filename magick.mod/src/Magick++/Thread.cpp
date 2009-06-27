// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000, 2001, 2002
//
// Implementation of thread support
//

#define MAGICK_IMPLEMENTATION
#define MAGICK_PLUSPLUS_IMPLEMENTATION

#include "Magick++/Thread.h"
#include "Magick++/Exception.h"

#include <string.h>

// Default constructor
Magick::MutexLock::MutexLock(void)
#if defined(HasPTHREADS)
  // POSIX threads
  : _mutex()
{
  ::pthread_mutexattr_t attr;
  int sysError;
  if ( (sysError = ::pthread_mutexattr_init( &attr )) == 0 )
    if ( (sysError = ::pthread_mutex_init( &_mutex, &attr )) == 0 )
      {
        ::pthread_mutexattr_destroy( &attr );
        return;
      }
  throwExceptionExplicit( OptionError, "mutex initialization failed",
                          strerror(sysError) );
}
#else
#if defined(_VISUALC_) && defined(_MT)
// Win32 threads
  : _mutex()
{
  SECURITY_ATTRIBUTES security;

  /* Allow the semaphore to be inherited */
  security.nLength = sizeof(security);
  security.lpSecurityDescriptor = NULL;
  security.bInheritHandle = TRUE;

  /* Create the semaphore, with initial value signaled */
  _mutex.id = ::CreateSemaphore(&security, 1, MAXSEMLEN, NULL);
  if ( _mutex.id != NULL )
    return;
  throwExceptionExplicit( OptionError, "mutex initialization failed" );
}
#else
// Threads not supported
{
}
#endif
#endif

// Destructor
Magick::MutexLock::~MutexLock(void)
{
#if defined(HasPTHREADS)
  int sysError;
  if ( (sysError = ::pthread_mutex_destroy( &_mutex )) == 0 )
    return;
  throwExceptionExplicit( OptionError, "mutex destruction failed",
                          strerror(sysError) );
#endif
#if defined(_MT) && defined(_VISUALC_)
  if ( ::CloseHandle(_mutex.id) != 0 )
    return;
  throwExceptionExplicit( OptionError, "mutex destruction failed" );
#endif
}

// Lock mutex
void Magick::MutexLock::lock(void)
{
#if defined(HasPTHREADS)
  int sysError;
  if ( (sysError = ::pthread_mutex_lock( &_mutex )) == 0)
    return;
  throwExceptionExplicit( OptionError, "mutex lock failed",
                          strerror(sysError));
#endif
#if defined(_MT) && defined(_VISUALC_)
#if 1
  if (WaitForSingleObject(_mutex.id,INFINITE) != WAIT_FAILED)
    return;
  throwExceptionExplicit( OptionError, "mutex lock failed" );
#else
  // Following code  is not known to work correctly yet.
  while(1)
  {
    DWORD status=MsgWaitForMultipleObjects(1, &_mutex.id, FALSE, INFINITE,
      QS_ALLEVENTS);
    // Loop if return was due to message received (which we don't care about).
    if (status == (WAIT_OBJECT_0+1))
      continue;
    // If return was due to handle state change, then object is available.
    if ((status - WAIT_OBJECT_0) == 0)
      return;
   }
  // If we get here, then there was an unexpected return value.
  throwExceptionExplicit( OptionError, "mutex lock failed" );
#endif
#endif
}

// Unlock mutex
void Magick::MutexLock::unlock(void)
{
#if defined(HasPTHREADS)
  int sysError;
  if ( (sysError = ::pthread_mutex_unlock( &_mutex )) == 0)
    return;
  throwExceptionExplicit( OptionError, "mutex unlock failed",
                          strerror(sysError) );
#endif
#if defined(_MT) && defined(_VISUALC_)
  if ( ReleaseSemaphore(_mutex.id, 1, NULL) == TRUE )
    return;
  throwExceptionExplicit( OptionError, "mutex unlock failed" );
#endif
}
