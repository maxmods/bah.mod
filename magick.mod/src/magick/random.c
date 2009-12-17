/*
% Copyright (C) 2009 GraphicsMagick Group
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
% Random number generator.
%
% Currently based on George Marsaglia's multiply-with-carry generator.
% This is a k=2 generator with a period >2^60.
%
*/

#include "magick/studio.h"
#include "magick/random.h"
#include "magick/semaphore.h"
#include "magick/tsd.h"
#include "magick/utility.h"

#if defined(HAVE_PTHREAD)
#  define USE_POSIX_THREADS 1
#elif defined(MSWINDOWS)
#  define USE_WIN32_THREADS 1
#endif

#if defined(USE_POSIX_THREADS)
#  include <pthread.h>
#endif
#if defined(USE_WIN32_THREADS)
#  include <windows.h>
#endif


static MagickBool initialized = MagickFalse;
static SemaphoreInfo *semaphore = (SemaphoreInfo *) 0;
static MagickTsdKey_t kernel_key = (MagickTsdKey_t) 0;

/*
  Acquire the default random number kernel.  Memory is owned by
  library and should not be freed.
*/
MagickExport MagickRandomKernel* AcquireMagickRandomKernel()
{
  MagickRandomKernel
    *kernel;

  if (!initialized)
    InitializeMagickRandomGenerator();

  kernel=(MagickRandomKernel *) MagickTsdGetSpecific(kernel_key);
  if (kernel == (MagickRandomKernel *) NULL)
    {
      kernel=MagickAllocateMemory(MagickRandomKernel *,
				  Max(sizeof(MagickRandomKernel),
				      MAGICK_CACHE_LINE_SIZE));
      if (kernel == (MagickRandomKernel *) NULL)
	MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
			  UnableToAllocateRandomKernel);
      InitializeMagickRandomKernel(kernel);
      MagickTsdSetSpecific(kernel_key,(const void *) kernel);
    }

  return kernel;
}

/*
  Initialize the random number generator system.
*/
void InitializeMagickRandomGenerator()
{
  assert(semaphore == (SemaphoreInfo *) NULL);
  semaphore=AllocateSemaphoreInfo();

  if (!initialized)
    {
      MagickTsdKeyCreate(&kernel_key);
      initialized=MagickTrue;
    }
}

/*
  Destroy the random number generator system.
*/
void DestroyMagickRandomGenerator()
{
  if (initialized)
    {
      MagickRandomKernel
	*kernel;

      /* This only frees memory associated with one thread */
      kernel=(MagickRandomKernel *) MagickTsdGetSpecific(kernel_key);
      MagickFreeMemory(kernel);
      (void) MagickTsdSetSpecific(kernel_key,kernel);
      MagickTsdKeyDelete(kernel_key);
    }
  kernel_key=(MagickTsdKey_t) 0;
  initialized=MagickFalse;
  DestroySemaphoreInfo(&semaphore);
}

/*
  Initialize the random kernel with suitable entropy
*/
MagickExport void
InitializeMagickRandomKernel(MagickRandomKernel *kernel)
{
  kernel->z = 0U;
  kernel->w = 0U;
#if HAVE_DEV_URANDOM
  {
    /*
      It is not known for sure what /dev/urandom on System X will do
      if the kernel runs out of entropy.  If it produces zeros, then
      don't trust the result.  While Linux /dev/urandom seems to use
      the same entropy pool as /dev/random, it apparently uses some
      hashing algorithm on the whole entropy pool (4096 bits) so that
      apparently random values are produced when the system runs out
      of fresh entropy.
    */
    int
      file;

    if ((file=open("/dev/urandom",O_RDONLY | O_BINARY)) != -1)
      {
	if (read(file,(void *) kernel,sizeof(*kernel)) == sizeof(*kernel))
	  {
	    (void) close(file);

	    if ((kernel->z != 0U) || (kernel->w != 0U))
	      return;
	  }
	(void) close(file);
      }
  }
#endif

  /*
    Initial time of day.
  */
  kernel->z ^= (magick_uint32_t) time(0);

  /*
    Let's hash with the address of kernel as well.
  */
  kernel->z ^= (magick_uint32_t) ((unsigned long) kernel & 4294967295UL);

 /*
    Process ID
  */
  kernel->w ^= (magick_uint32_t) getpid();

  /*
    Thread ID
  */
#if defined(USE_POSIX_THREADS)
  {
    union
    {
      pthread_t thread_id;
      magick_uint32_t intval;
    } pthread_union;
    
    pthread_union.intval=0U;
    pthread_union.thread_id=pthread_self();
    kernel->w ^= pthread_union.intval;
  }
#elif defined(USE_WIN32_THREADS)
  kernel->w ^= ((magick_uint32_t) GetCurrentThreadId());
#endif

  /*
    It is quite likely that multiple threads will invoke this function
    during the same second so we also tap into the default random
    number generator to help produce a more random seed.
  */
  kernel->w ^= (magick_uint32_t) rand();
}

/*
  Generate a random integer value
*/
MagickExport magick_uint32_t MagickRandomInteger(void)
{
  MagickRandomKernel
    *kernel;

  kernel=AcquireMagickRandomKernel();
  return MagickRandomIntegerInlined(kernel);
}

/*
  Generate a random double value
*/
MagickExport double MagickRandomReal(void)
{
  MagickRandomKernel
    *kernel;

  kernel=AcquireMagickRandomKernel();
  return MagickRandomRealInlined(kernel);
}
