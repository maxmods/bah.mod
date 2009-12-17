/*
  Copyright (C) 2005 GraphicsMagick Group
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
*/

/*
  Thread specific data support.
*/

#include "magick/studio.h"
#include "magick/utility.h"
#if defined(HAVE_PTHREAD)
#include <pthread.h>
#endif /* defined(HAVE_PTHREAD) */
#if defined(MSWINDOWS)
#  include <windows.h>
#endif /* defined(MSWINDOWS) */
#include "magick/tsd.h"

/*
  Create a thread specific data key.
*/
MagickExport MagickPassFail MagickTsdKeyCreate(MagickTsdKey_t *key)
{
#if defined(HAVE_PTHREAD)
  return ((pthread_key_create(key, 0) == 0) ? MagickPass : MagickFail);
#elif defined(MSWINDOWS)
  /* DWORD WINAPI TlsAlloc(void); */
  *key=TlsAlloc();
  return ((*key != TLS_OUT_OF_INDEXES) ? MagickPass : MagickFail);
#else
  *key=MagickAllocateMemory(unsigned long *,sizeof(unsigned long));
  if (*key != (void *) NULL)
    *key[0]=0;
  return ((*key != (void *) NULL) ? MagickPass : MagickFail);
#endif
}

/*
  Delete a thread-specific data key, invoking the registered
  destructor on any remaining non-NULL key values.
*/
MagickExport MagickPassFail MagickTsdKeyDelete(MagickTsdKey_t key)
{
#if defined(HAVE_PTHREAD)
  return ((pthread_key_delete(key) == 0) ? MagickPass : MagickFail);
#elif defined(MSWINDOWS)
  /* BOOL WINAPI TlsFree(DWORD dwTlsIndex) */
  return ((TlsFree(key) != 0) ? MagickPass : MagickFail);
#else
  MagickFreeMemory(key);
  return MagickPass;
#endif
}

/*
  Set the thread-specific value associated with the key.
*/
MagickExport MagickPassFail MagickTsdSetSpecific(MagickTsdKey_t key, const void *value)
{
#if defined(HAVE_PTHREAD)
  return ((pthread_setspecific(key, value) == 0) ? MagickPass : MagickFail);
#elif defined(MSWINDOWS)
  /* BOOL WINAPI TlsSetValue(DWORD dwTlsIndex,LPVOID lpTlsValue) */
  return ((TlsSetValue(key,(void *) value) != 0) ? MagickPass : MagickFail);
#else
  *key=(unsigned long) value;
  return MagickPass;
#endif
}

/*
  Get the thread-specific value associated with the key.
*/
MagickExport void *MagickTsdGetSpecific(MagickTsdKey_t key)
{
#if defined(HAVE_PTHREAD)
  return (pthread_getspecific(key));
#elif defined(MSWINDOWS)
  /* LPVOID WINAPI TlsGetValue(DWORD dwTlsIndex) */
  return TlsGetValue(key);
#else
  return (void *) (*key);
#endif
}
