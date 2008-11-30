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
%                  M   M   AAA    GGGG  IIIII   CCCC  K   K                   %
%                  MM MM  A   A  G        I    C      K  K                    %
%                  M M M  AAAAA  G GGG    I    C      KKK                     %
%                  M   M  A   A  G   G    I    C      K  K                    %
%                  M   M  A   A   GGGG  IIIII   CCCC  K   K                   %
%                                                                             %
%                                                                             %
%            Methods to Read or List GraphicsMagick Image formats             %
%                                                                             %
%                                                                             %
%                            Software Design                                  %
%                            Bob Friesenhahn                                  %
%                              John Cristy                                    %
%                             November 1998                                   %
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
#if defined(MSWINDOWS) || defined(__CYGWIN__)
# include "magick/nt_feature.h"
#endif
#include "magick/blob.h"
#include "magick/color.h"
#include "magick/constitute.h"
#include "magick/delegate.h"
#include "magick/log.h"
#include "magick/magic.h"
#include "magick/magick.h"
#include "magick/module.h"
#include "magick/pixel_cache.h"
#include "magick/registry.h"
#include "magick/resource.h"
#include "magick/render.h"
#include "magick/semaphore.h"
#include "magick/tempfile.h"
#include "magick/utility.h"
#include "magick/version.h"
#if defined(HasX11)
#include "magick/xwindow.h"
#endif
/* watch out here - this include code for simple spinlock semaphore */
#include "magick/spinlock.h"

/*
  Global declarations.
*/
typedef enum  {
  InitDefault,
  InitUninitialized,
  InitInitialized
} MagickInitializationState;

static void
  DestroyMagickInfoList(void);

static SemaphoreInfo
  *magick_semaphore = (SemaphoreInfo *) NULL;

static MagickInfo
  *magick_list = (MagickInfo *) NULL;

static MagickInitializationState MagickInitialized = InitDefault;
static CoderClass MinimumCoderClass = UnstableCoderClass;

static void DestroyMagickInfo(MagickInfo** magick_info);
static void DestroyMagickInfoList(void);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y M a g i c k                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyMagick() destroys the GraphicsMagick environment.
%
%  The format of the DestroyMagick function is:
%
%      DestroyMagick(void)
%
%
*/
MagickExport void DestroyMagick(void)
{
  if (MagickInitialized == InitUninitialized)
    return;

#if defined(HasX11)
  MagickXDestroyX11Resources();
#endif
  DestroyColorInfo();           /* Color database */
  DestroyDelegateInfo();        /* External delegate information */
  DestroyTypeInfo();            /* Font information */
  DestroyMagicInfo();           /* File format detection */
  DestroyMagickInfoList();      /* Coder registrations + modules */
  DestroyConstitute();          /* Constitute semaphore */
  DestroyMagickRegistry();      /* Registered images */
  DestroyMagickResources();     /* Resource semaphore */
  DestroyTemporaryFiles();      /* Temporary files */
#if defined(MSWINDOWS)
  NTGhostscriptUnLoadDLL();     /* Ghostscript DLL */
#endif /* defined(MSWINDOWS) */
  /*
    Destroy logging last since some components log their destruction.
  */
  DestroyLogInfo();             /* Logging configuration */
  DestroySemaphore();           /* Semaphores framework */

  MagickInitialized=InitUninitialized;

}
/*
  Destroy MagickInfo structure.
*/

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y M a g i c k I n f o                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyMagickInfo() destroys a MagickInfo structure.
%
%  The format of the DestroyMagickInfoList method is:
%
%      void DestroyMagickInfo(MagickInfo** magick_info)
%
*/
static void DestroyMagickInfo(MagickInfo** magick_info)
{
  MagickInfo
    *p;

  p=*magick_info;
  if (p)
    {
      p->name=0;
      p->description=0;
      p->version=0;
      p->note=0;
      p->module=0;
      MagickFreeMemory(p);
      *magick_info=p;
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y M a g i c k I n f o L i s t                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyMagickInfo() deallocates memory associated with the MagickInfo list.
%
%  The format of the DestroyMagickInfoList method is:
%
%      void DestroyMagickInfoList(void)
%
*/
static void DestroyMagickInfoList(void)
{
  MagickInfo
    *magick_info;

  register MagickInfo
    *p;

#if defined(SupportMagickModules)
  DestroyMagickModules();
#endif /* defined(SupportMagickModules) */

#if !defined(BuildMagickModules)
  UnregisterStaticModules();
#endif /* !defined(BuildMagickModules) */

  /*
    At this point, the list should be empty, but check for remaining
    entries anyway.
  */
  AcquireSemaphoreInfo(&magick_semaphore);
#if defined(BuildMagickModules)
  if (magick_list != (MagickInfo *) NULL)
    (void) printf("Warning: module registrations are still present!\n");
#endif /* defined(BuildMagickModules) */
  for (p=magick_list; p != (MagickInfo *) NULL; )
  {
    magick_info=p;
    p=p->next;

#if !defined(BuildMagickModules)
    (void) printf("Warning: module registration for \"%s\" from module \"%s\" still present!\n",
                  magick_info->name, magick_info->module);
#endif /* !defined(BuildMagickModules) */
    DestroyMagickInfo(&magick_info);
  }
  magick_list=(MagickInfo *) NULL;
  LiberateSemaphoreInfo(&magick_semaphore);
  DestroySemaphoreInfo(&magick_semaphore);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e M a g i c k                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageMagick() searches for an image format that matches the specified
%  magick string.  If one is found the name is returned otherwise NULL.
%
%  The format of the GetImageMagick method is:
%
%      const char *GetImageMagick(const unsigned char *magick,
%        const size_t length)
%
%  A description of each parameter follows:
%
%    o magick: The image format we are searching for.
%
%    o length: The length of the binary string.
%
%
*/
MagickExport const char *GetImageMagick(const unsigned char *magick,
  const size_t length)
{
  register MagickInfo
    *p;

  assert(magick != (const unsigned char *) NULL);
  AcquireSemaphoreInfo(&magick_semaphore);
  for (p=magick_list; p != (MagickInfo *) NULL; p=p->next)
    if (p->magick && p->magick(magick,length))
      break;
  LiberateSemaphoreInfo(&magick_semaphore);
  if (p != (MagickInfo *) NULL)
    return(p->name);
  return((char *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t M a g i c k I n f o                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetMagickInfo() returns a pointer MagickInfo structure that matches
%  the specified name.  If name is NULL, the head of the image format list
%  is returned. It is not safe to traverse the list by using the previous and
%  next pointers in the MagickInfo structure since the list contents or order
%  may be altered while the list is being traversed. If the list must be
%  traversed, access it via the GetMagickInfoArray function instead.
%
%  The format of the GetMagickInfo method is:
%
%     const MagickInfo *GetMagickInfo(const char *name,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o name: The image format we are looking for.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static MagickInfo *GetMagickInfoEntryLocked(const char *name)
{
  register MagickInfo
    *p;

  AcquireSemaphoreInfo(&magick_semaphore);

  p=magick_list;

  if ((name != (const char *) NULL) && (name[0] != '*'))
    {
      for (p=magick_list; p != (MagickInfo *) NULL; p=p->next)
        if (LocaleCompare(p->name,name) == 0)
          break;
      
      if (p != (MagickInfo *) NULL)
        if (p != magick_list)
          {
            /*
              Self-adjusting list.
            */
            if (p->previous != (MagickInfo *) NULL)
              p->previous->next=p->next;
            if (p->next != (MagickInfo *) NULL)
              p->next->previous=p->previous;
            p->previous=(MagickInfo *) NULL;
            p->next=magick_list;
            magick_list->previous=p;
            magick_list=p;
          }
    }

  LiberateSemaphoreInfo(&magick_semaphore);

  return p;
}
MagickExport const MagickInfo *GetMagickInfo(const char *name,
  ExceptionInfo *ARGUNUSED(exception))
{
#if !defined(BuildMagickModules)
  if ((magick_list == (MagickInfo *) NULL) &&
      (GetMagickInfoEntryLocked(NULL) == (MagickInfo *) NULL))
    {
      /*
        Register all static modules
      */
      RegisterStaticModules();
    }
#endif /* !defined(BuildMagickModules) */

  /*
    If name is NULL, then return head of list (as it currently
    exists).
  */
  if (name == (const char *) NULL)
    return GetMagickInfoEntryLocked(name);

#if defined(SupportMagickModules)
  /*
    Load module aliases (does nothing if already loaded)
  */
  (void) GetModuleInfo((char *) NULL,exception);

  if (name[0] == '*')
    {
      /*
        If all modules are requested, then use OpenModules to load all
        modules.
      */
      (void) OpenModules(exception);
    }
  else
    {
      /*
        Try to load a supporting module.
      */
      (void) OpenModule(name,exception);
    }
#endif /* #if defined(SupportMagickModules) */

  /*
    Return whatever we've got
  */
  return GetMagickInfoEntryLocked(name);
}
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t M a g i c k I n f o A r r a y                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetMagickInfoArray() returns a sorted null-terminated array of MagickInfo
%  pointers corresponding to the available format registrations. If necessarly
%  all modules are loaded in order to return a complete list. This function
%  should be used to access the entire list rather than GetMagickInfo since
%  the list returned by GetMagickInfo may be re-ordered every time it is
%  invoked. Once the returned array is no longer needed, the allocated array
%  should be deallocated. Do not attempt to deallocate the MagickInfo
%  structures based on pointers in the array!
%
%  The format of the GetMagickList method is:
%
%      MagickInfo **GetMagickInfoArray(ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
/*
  Compare two MagickInfo structures based on their name
*/
static int MagickInfoCompare(const void *x, const void *y)
{
  const MagickInfo
    *xx=*((const MagickInfo **) x),
    *yy=*((const MagickInfo **) y);

  return (strcmp(xx->name, yy->name));
}
MagickExport MagickInfo **GetMagickInfoArray(ExceptionInfo *exception)
{
  MagickInfo
    **array;

  MagickInfo
    *p;

  MagickInfo
    *list;

  size_t
    entries=0;

  int
    i;

  /*
    Load all modules and obtain pointer to head of list
  */
  (void) GetMagickInfo("*",exception);
  if (!magick_list)
    return 0;

  AcquireSemaphoreInfo(&magick_semaphore);

  list=magick_list;

  /*
    Count number of list entries
  */
  for (p=list; p != 0; p=p->next)
    entries++;

  /*
    Allocate array memory
  */
  array=MagickAllocateMemory(MagickInfo **,sizeof(MagickInfo *)*(entries+1));
  if (!array)
    {
      ThrowException(exception,ResourceLimitError,MemoryAllocationFailed,0);
      return False;
    }
  (void) memset((void **)array,0,sizeof(MagickInfo *)*(entries+1));

  /*
    Add entries to array
  */
  i=0;
  for (p=list; p != 0; p=p->next)
    array[i++]=p;

  LiberateSemaphoreInfo(&magick_semaphore);

  /*
    Sort array entries
  */
  qsort((void *) array, entries, sizeof(MagickInfo *), MagickInfoCompare);

  return (array);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I n i t i a l i z e M a g i c k                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  InitializeMagick() initializes the GraphicsMagick environment.
%
%  The format of the InitializeMagick function is:
%
%      InitializeMagick(const char *path)
%
%  A description of each parameter follows:
%
%    o path: The execution path of the current GraphicsMagick client.
%
%
*/

/*
  Due to legacy compilers, POSIX.1 actually allows the signal function
  to return 'int' or 'void'. The compiler is not so lenient so we must
  ensure that the return type matches the prototype in the headers.
*/
#if !defined(RETSIGTYPE)
# define RETSIGTYPE void
#endif
typedef RETSIGTYPE Sigfunc(int);

#if !defined(SIG_ERR)
# define SIG_ERR (Sigfunc *)-1
#endif
#if !defined(SIG_DFL)
# define SIG_DFL (Sigfunc *)0
#endif

/*
  Signal function which prevents interrupted system calls from
  automatically being restarted. From W. Richard Stevens "Advanced
  Programming in the UNIX Environment", Chapter 10.14.
*/
static Sigfunc *MagickSignal(int signo, Sigfunc *func)
{
#if defined(HAVE_SIGACTION) && defined(HAVE_SIGEMPTYSET)
  struct sigaction
    act,
    oact;

  act.sa_handler=func;
  (void) sigemptyset(&act.sa_mask);
  act.sa_flags=0;
#  if defined(SA_INTERRUPT)  /* SunOS */
  act.sa_flags |= SA_INTERRUPT;
#  endif
  if (sigaction(signo, &act, &oact) < 0)
    return (SIG_ERR);
  return (oact.sa_handler);
#else
  return signal(signo, func);
#endif
}

/*
  Signal function which only sets the signal handler if it its handling
  is still set to SIG_DFL. This ensures that signal handlers are not
  registered for signals which are being ignored, or that the API
  user has already registered before invoking InitializeMagick.

  If an API user registers its own signal hander, then it is responsible
  for invoking DestroyMagick when a signal is received.
*/
static Sigfunc *MagickCondSignal(int signo, Sigfunc *func)
{
  Sigfunc *
    o_handler;

  if ((o_handler=MagickSignal(signo,func)) != SIG_ERR)
    {
      /*
        If handler is not the default, then restore the previous
        setting.
      */
      if (o_handler != SIG_DFL)
        (void) MagickSignal(signo,o_handler);
      else
        (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
          "Registered signal handler for signal ID %d",signo);
    }
  return (o_handler);
}

/*
  Signal handler which does nothing.
*/
#if 0
static RETSIGTYPE MagickIgnoreSignalHandler(int signo)
{
  fprintf(stderr,"Caught ignored signal %d\n", signo);
}
#endif

/*
  Signal handler to ensure that DestroyMagick is invoked in case the
  user aborts the program.

  The WIN32 documentation says that SIGINT is not supported under any
  version of Windows. It also says that a new thread is created to
  handle the interrupt caused by CNTRL+C. The Windows signal
  documentation also says that it is unsafe to do anything useful from
  within a signal handler.  This means that malloc/free and any system
  call should not be executed. Theoretically DestroyMagick() should
  not be executed since its purpose is to invoke unsafe functions.
  SIGILL, SIGSEGV, and SIGTERM are documented to never be generated by
  Windows NT.

  While support for signal is definitely broken under Windows, the good
  news is that it seems to be unlikely to generate a signal we care about.
*/
static RETSIGTYPE MagickPanicSignalHandler(int signo)
{
  /* fprintf(stderr,"Caught panic signal %d\n", signo); */

  /*
    Restore default handling for the signal
  */
  (void) MagickSignal(signo,SIG_DFL);

  /*
    Release resources
  */
  DestroyTemporaryFiles();

  /*
    Raise signal again to invoke default handler
    This may cause a core dump or immediate exit.
  */
#if defined(HAVE_RAISE)
  (void) fflush(stdout);
  (void) raise(signo);
#endif

  SignalHandlerExit(signo);
}
static RETSIGTYPE MagickSignalHandler(int signo)
{
  /* fprintf(stderr,"Caught signal %d\n", signo); */

  /*
    Restore default handling for the signal
  */
  (void) MagickSignal(signo,SIG_DFL);

  /*
    Release resources
  */
  DestroyMagick();

  /*
    Raise signal again to invoke default handler
    This may cause a core dump or immediate exit.
  */
#if defined(HAVE_RAISE)
  (void) raise(signo);
#endif

  SignalHandlerExit(signo);
}

/*
   The goal of this routine is to determine whether the passed
   string is a valid and complete path to a file within the 
   filesystem
 */
#if !defined(UseInstalledMagick)
static unsigned int IsValidFilesystemPath(const char *path)
{
  if ((path != (const char *) NULL) && (*path != '\0'))
    {
#if defined(POSIX)
      /* For POSIX we check the first character to see of it is a file
          system path seperator. If not then we ignore the passed data
      */
      if ((*path == *DirectorySeparator))
        return IsAccessibleNoLogging(path);
#elif defined(MSWINDOWS)
      /* For Windows we check to see if the path passed seems to be a 
         pathof any kind (contains delimiters) or seem to be either UNC
         path or one with a drive letter spec in it: \\Server\share, C:\
      */
      if (((*path == *DirectorySeparator) && (*(path+1) == *DirectorySeparator)) ||
            (*(path+1) == ':') ||
              (strchr(path,*DirectorySeparator) != (char *) NULL))
        return IsAccessibleNoLogging(path);
#else
      /* In any other case, we just look to see if it has path delimiters */
      if ((strchr(path,*DirectorySeparator) != (char *) NULL))
        return IsAccessibleNoLogging(path);
#endif
    }
  return False;
}
#endif /* !defined(UseInstalledMagick) */

/*
  Try and figure out the path and name of the client
 */
MagickExport void InitializeMagickClientPathAndName(const char *ARGUNUSED(path))
{
#if !defined(UseInstalledMagick)
  const char
    *spath;

  char
    execution_path[MaxTextExtent];

  /* the following is to make the logging more readable later on */
  spath = path;
  if (spath == (char *) NULL)
    spath = "NULL";
  if (*spath == '\0')
    spath = "EMPTY";

  *execution_path='\0';
  if (!IsValidFilesystemPath(path))
    {
      /*
        If the path passed is NULL or invalid then we try to ask the
        OS what the name of the executable is. Callers will often pass
        NULL in order to invoke this functionality.
      */
      if (GetExecutionPath(execution_path))
        {
          /*
            The call to the OS worked so we can do all the settings
          */
          (void) DefineClientPathAndName(execution_path);
          (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
            "Invalid path \"%s\" passed - asking OS worked: \"%s\"",spath,
              execution_path);
        }
      else
        {
          /*
            The call to the OS failed so we try to compute a usable
            path based on the current working directory or by
            searching the executable search path.
          */
          if (path)
            (void) strlcpy(execution_path,path,MaxTextExtent);
          if (GetExecutionPathUsingName(execution_path))
            {
              (void) DefineClientPathAndName(execution_path);
              (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
                "Invalid path \"%s\" passed - asking OS failed, getcwd worked: \"%s\"",spath,
                  execution_path);
            }
          else
            {
              /*
                At this point we are totally hosed and have nothing to
                go on for the path, so all we can do is log the error
              */
              (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
                "Invalid path \"%s\" passed - asking OS failed, getcwd also failed",spath);
            }
        }
    }
  else
    {
      /*
        This is the easy one. The caller gave us the correct and
        working path to the application, so we just use it
      */
      (void) strlcpy(execution_path,path,MaxTextExtent);
      (void) DefineClientPathAndName(execution_path);
      (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),"Valid path \"%s\"",spath);
    }
#endif
}

/*
  Establish signal handlers for common signals
*/
MagickExport void InitializeMagickSignalHandlers(void)
{
#if 0
  /* termination of child process */
#if defined(SIGCHLD)
  (void) MagickCondSignal(SIGCHLD,MagickIgnoreSignalHandler);
#endif
#endif

  /* hangup, default terminate */
#if defined(SIGHUP)
  (void) MagickCondSignal(SIGHUP,MagickSignalHandler);
#endif
  /* interrupt (rubout), default terminate */
#if defined(SIGINT) && !defined(MSWINDOWS)
  (void) MagickCondSignal(SIGINT,MagickSignalHandler);
#endif
  /* quit (ASCII FS), default terminate with core */
#if defined(SIGQUIT)
  (void) MagickCondSignal(SIGQUIT,MagickPanicSignalHandler);
#endif
  /* software-triggered abort, default terminate with core */
#if defined(SIGABRT)
  (void) MagickCondSignal(SIGABRT,MagickPanicSignalHandler);
#endif
  /* floating point exception, default terminate with core */
#if defined(SIGFPE)
  (void) MagickCondSignal(SIGFPE,MagickPanicSignalHandler);
#endif
  /* software termination signal from kill, default terminate */
#if defined(SIGTERM)
  (void) MagickCondSignal(SIGTERM,MagickSignalHandler);
#endif
  /* exceeded cpu limit, default terminate with core */
#if defined(SIGXCPU)
  (void) MagickCondSignal(SIGXCPU,MagickPanicSignalHandler);
#endif
  /* exceeded file size limit, default terminate with core */
#if defined(SIGXFSZ)
  (void) MagickCondSignal(SIGXFSZ,MagickPanicSignalHandler);
#endif
}

MagickExport void InitializeMagick(const char *path)
{
  const char
    *p;

  /* NOTE: This routine sets up the path to the client which needs to
     be determined before almost anything else works right. This also
     includes logging!!! So we can't start logging until the path is
     actually saved. As soon as we know what the path is we make the
     same call to DefineClientSettings to set it up. Please make sure
     that this rule is followed in any future updates the this code!!!
  */
  if (MagickInitialized == InitInitialized)
    return;
  SPINLOCK_WAIT;
  MagickInitialized=InitInitialized;
  SPINLOCK_RELEASE;
  
#if defined(MSWINDOWS)
# if defined(_DEBUG) && !defined(__BORLANDC__)
  {
    int
      debug;
    
    debug=_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    debug|=_CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_DELAY_FREE_MEM_DF |
      _CRTDBG_LEAK_CHECK_DF;
    // debug=_CrtSetDbgFlag(debug);
    // _ASSERTE(_CrtCheckMemory());
  }
# endif /* defined(_DEBUG) */
#endif /* defined(MSWINDOWS) */
  
  (void) setlocale(LC_ALL,"");
  (void) setlocale(LC_NUMERIC,"C");
  
  /* Seed the random number generator */
  srand(time(0));
  
  /* Initialize semaphores */
  InitializeSemaphore();
  
  /*
    Set logging flags using the value of MAGICK_DEBUG if it is set in
    the environment.
  */
  if ((p=getenv("MAGICK_DEBUG")) != (const char *) NULL)
    (void) SetLogEventMask(p);

  /*
    Establish the path, filename, and display name of the client app
  */
  InitializeMagickClientPathAndName(path);
  /*
    If the client name did not get setup for any reason, we take one
    last shot at it using the data the caller passed us.
  */
  if (GetClientName() == (const char *) NULL)
    DefineClientName(path);

  /*
    Set logging flags using the value of MAGICK_DEBUG if it is set in
    the environment.
  */
  if ((p=getenv("MAGICK_DEBUG")) != (const char *) NULL)
    (void) SetLogEventMask(p);

  /*
    Establish signal handlers for common signals
  */
  InitializeMagickSignalHandlers();

  /*
    Compute memory allocation and memory map resource limits based
    on available memory
  */
  /* NOTE: This call does logging, so you can not make it before the path
     to the client is setup */
  InitializeMagickResources();

  /*
    Adjust minimum coder class if requested.
  */
  if ((p=getenv("MAGICK_CODER_STABILITY")) != (const char *) NULL)
    {
      if (LocaleCompare(p,"UNSTABLE") == 0)
        MinimumCoderClass=UnstableCoderClass;
      else if (LocaleCompare(p,"STABLE") == 0)
        MinimumCoderClass=StableCoderClass;
      else if (LocaleCompare(p,"PRIMARY") == 0)
        MinimumCoderClass=PrimaryCoderClass;
    }

  /*
    Initialize module loader
  */
#if defined(SupportMagickModules)
  InitializeMagickModules();
#endif /* defined(SupportMagickModules) */

  /* Let's log the three important setting as we exit this routine */
  (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
    "Path: \"%s\" Name: \"%s\" Filename: \"%s\"",
      GetClientPath(),GetClientName(),GetClientFilename());
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s M a g i c k C o n f l i c t                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsMagickConflict returns true if the image format conflicts with
%  a logical drive (.e.g. X:).
%
%  The format of the IsMagickConflict method is:
%
%      MagickBool IsMagickConflict(const char *magick)
%
%  A description of each parameter follows:
%
%    o status: Method IsMagickConflict returns true if the image format
%      conflicts with a logical drive.
%
%    o magick: Specifies the image format.
%
%
*/
MagickExport MagickBool IsMagickConflict(const char *magick)
{
  assert(magick != (char *) NULL);
#if defined(MSWINDOWS) || defined(__CYGWIN__)
  return(NTIsMagickConflict(magick));
#endif
  return(MagickFalse);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  L i s t M a g i c k I n f o                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ListMagickInfo() lists the image formats to a file.
%
%  The format of the ListMagickInfo method is:
%
%      MagickPassFail ListMagickInfo(FILE *file,ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o file: A file handle.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport MagickPassFail ListMagickInfo(FILE *file,ExceptionInfo *exception)
{

  MagickInfo
    **magick_array;

  int
    i;

  if (file == (FILE *) NULL)
    file=stdout;

  magick_array=GetMagickInfoArray(exception);
  if (!magick_array)
    return False;

  (void) fprintf(file,"   Format L  Mode  Description\n");
  (void) fprintf(file,"--------------------------------------------------------"
    "------------------------\n");
  for (i=0; magick_array[i] != 0; i++)
  {
    if (magick_array[i]->stealth)
      continue;
    (void) fprintf(file,"%9s %c  %c%c%c",
                   magick_array[i]->name ? magick_array[i]->name : "",
                   (magick_array[i]->coder_class == PrimaryCoderClass ? 'P' :
                    (magick_array[i]->coder_class == StableCoderClass ? 'S' :
                     'U')),
                   (magick_array[i]->decoder ? 'r' : '-'),
                   (magick_array[i]->encoder ? 'w' : '-'),
                   ((magick_array[i]->encoder && magick_array[i]->adjoin) ? '+' : '-'));
    if (magick_array[i]->description != (char *) NULL)
      (void) fprintf(file,"  %.1024s",magick_array[i]->description);
    if (magick_array[i]->version != (char *) NULL)
      (void) fprintf(file," (%.1024s)",magick_array[i]->version);
    (void) fprintf(file,"\n");
    if (magick_array[i]->note != (char *) NULL)
      {
        char
          **text;

        text=StringToList(magick_array[i]->note);
        if (text != (char **) NULL)
          {
            register long
              i;

            for (i=0; text[i] != (char *) NULL; i++)
              {
                (void) fprintf(file,"            %.1024s\n",text[i]);
                MagickFreeMemory(text[i]);
              }
            MagickFreeMemory(text);
          }
      }
  }
  (void) fprintf(file,"\n Meaning of 'L': P=Primary, S=Stable, U=Unstable\n");
  (void) fflush(file);
  MagickFreeMemory(magick_array);
  return(MagickPass);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  L i s t M o d u l e M a p                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ListModuleMap lists the module alias info to a file in the XML
%  format used by modules.mgk. True is returned on success.
%
%  The format of the ListModuleMap method is:
%
%      MagickPassFail ListModuleMap(FILE *file,ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o file:  An pointer to a FILE.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport MagickPassFail ListModuleMap(FILE *file,ExceptionInfo *exception)
{
  MagickInfo
    **magick_array;

  int
    i;

  if (file == (const FILE *) NULL)
    file=stdout;

   magick_array=GetMagickInfoArray(exception);
   if ((!magick_array) || (exception->severity > UndefinedException))
     return MagickFail;

   (void) fprintf(file, "<?xml version=\"1.0\"?>\n");
   (void) fprintf(file, "<!-- %s -->\n",GetMagickCopyright());
   (void) fprintf(file, "<!-- Magick Module Alias Map (modules.mgk) -->\n");
   (void) fprintf(file, "<modulemap>\n");

   for (i=0; magick_array[i] != 0; i++)
     {
       if (LocaleCompare(magick_array[i]->name,magick_array[i]->module) != 0)
         {
/*            if (i != 0) */
/*              (void) fprintf(file, "\n"); */
/*            (void) fprintf(file, "  <!-- %s -->\n",magick_array[i]->description); */
           (void) fprintf(file, "  <module magick=\"%s\" name=\"%s\" />\n",
                          magick_array[i]->name, magick_array[i]->module);
         }
     }
   (void) fprintf(file, "</modulemap>\n");
   (void) fflush(file);

   MagickFreeMemory(magick_array);

   return(MagickPass);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  M a g i c k T o M i m e                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickToMime returns the officially registered (or de facto) MIME
%  media-type corresponding to a magick string.  If there is no registered
%  media-type, then the string "image/x-magick" (all lower case) is returned.
%  The returned string must be deallocated by the user.
%
%  The format of the MagickToMime method is:
%
%      char *MagickToMime(const char *magick)
%
%  A description of each parameter follows.
%
%   o  magick:  GraphicsMagick format specification "magick" tag.
%
%
*/
MagickExport char *MagickToMime(const char *magick)
{
  typedef struct _MediaType
  {
    const char
      *magick,
      *media;
  } MediaType;

  char
    media[MaxTextExtent];

  MediaType
    *entry;

  static MediaType
    MediaTypes[] =
    {
      { "avi",   "video/avi" },
      { "cgm",   "image/cgm;Version=4;ProfileId=WebCGM" }, /* W3 WebCGM */
      { "dcm",   "application/dicom" }, /* Incomplete.  See RFC 3240 */
      { "epdf",  "application/pdf" },
      { "epi",   "application/postscript" },
      { "eps",   "application/postscript" },
      { "eps2",  "application/postscript" },
      { "eps3",  "application/postscript" },
      { "epsf",  "application/postscript" },
      { "ept",   "application/postscript" },
      { "fax",   "image/g3fax" },
      { "fpx",   "image/vnd.fpx" },
      { "g3",    "image/g3fax" },
      { "gif",   "image/gif" },
      { "gif87", "image/gif" },
      { "jpeg",  "image/jpeg" },
      { "mng",   "video/x-mng" },
      { "mpeg",  "video/mpeg" },
      { "png",   "image/png" },
      { "pdf",   "application/pdf" },
      { "ps",    "application/postscript" },
      { "ps2",   "application/postscript" },
      { "ps3",   "application/postscript" },
      { "svg",   "image/svg+xml" },
      { "tif",   "image/tiff" },
      { "tiff",  "image/tiff" },
      { "wbmp",  "image/vnd.wap.wbmp" },
      { (char *) NULL, (char *) NULL }
    };

  for (entry=MediaTypes; entry->magick != (char *) NULL; entry++)
    if (LocaleCompare(entry->magick,magick) == 0)
      return(AllocateString(entry->media));
  FormatString(media,"image/x-%.1024s",magick);
  LocaleLower(media+8);
  return(AllocateString(media));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r M a g i c k I n f o                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  RegisterMagickInfo() adds attributes for a particular image format to the
%  list of supported formats.  The attributes include the image format name,
%  a method to read and/or write the format, whether the format supports the
%  saving of more than one frame to the same file or blob, whether the format
%  supports native in-memory I/O, and a brief description of the format.
%
%  The format of the RegisterMagickInfo method is:
%
%      MagickInfo *RegisterMagickInfo(MagickInfo *magick_info)
%
%  A description of each parameter follows:
%
%    o magick_info: The magick info.
%
*/
MagickExport MagickInfo *RegisterMagickInfo(MagickInfo *magick_info)
{
  assert(magick_info != (MagickInfo *) NULL);
  assert(magick_info->signature == MagickSignature);

  /*
    Remove any existing entry.
  */
  (void) UnregisterMagickInfo(magick_info->name);

  /*
    Verify that coder stability level is sufficient.
  */
  if (magick_info->coder_class >= MinimumCoderClass)
    {
      /*
        Add to front of list.
      */
      AcquireSemaphoreInfo(&magick_semaphore);
      magick_info->previous=(MagickInfo *) NULL;
      magick_info->next=magick_list;
      if (magick_info->next != (MagickInfo *) NULL)
        magick_info->next->previous=magick_info;
      magick_list=magick_info;
      LiberateSemaphoreInfo(&magick_semaphore);
      return(magick_info);
    }

  /*
    Discard registration and return NULL.
  */
  DestroyMagickInfo(&magick_info);
  return(magick_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t M a g i c k I n f o                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetMagickInfo allocates a MagickInfo structure and initializes the
%  members to default values.
%
%  The format of the SetMagickInfo method is:
%
%      MagickInfo *SetMagickInfo(const char *name)
%
%  A description of each parameter follows:
%
%    o magick_info: Method SetMagickInfo returns the allocated and initialized
%      MagickInfo structure.
%
%    o name: a character string that represents the image format associated
%      with the MagickInfo structure.
%
%
*/
MagickExport MagickInfo *SetMagickInfo(const char *name)
{
  MagickInfo
    *magick_info;

  assert(name != (const char *) NULL);
  magick_info=MagickAllocateMemory(MagickInfo *,sizeof(MagickInfo));
  if (magick_info == (MagickInfo *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToAllocateMagickInfo);
  (void) memset(magick_info,0,sizeof(MagickInfo));
  magick_info->name=name;
  magick_info->adjoin=MagickTrue;
  magick_info->raw=MagickFalse;
  magick_info->stealth=MagickFalse;
  magick_info->seekable_stream=MagickFalse;
  magick_info->blob_support=MagickTrue;
  magick_info->thread_support=MagickTrue;
  magick_info->coder_class=StableCoderClass;
  magick_info->extension_treatment=HintExtensionTreatment;
  magick_info->signature=MagickSignature;
  return(magick_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r M a g i c k I n f o                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterMagickInfo removes a name from the magick info list.  It
%  returns MagickFail if the name does not exist in the list otherwise
%  MagickPass.
%
%  The format of the UnregisterMagickInfo method is:
%
%      unsigned int UnregisterMagickInfo(const char *name)
%
%  A description of each parameter follows:
%
%    o status: Method UnregisterMagickInfo returns False if the name does not
%      exist in the list otherwise True.
%
%    o name: a character string that represents the image format we are
%      looking for.
%
*/
MagickExport MagickPassFail UnregisterMagickInfo(const char *name)
{
  MagickInfo
    *magick_info;

  register MagickInfo
    *p;

  unsigned int
    status;

  assert(name != (const char *) NULL);
  status=MagickFail;
  AcquireSemaphoreInfo(&magick_semaphore);
  for (p=magick_list; p != (MagickInfo *) NULL; p=p->next)
  {
    if (LocaleCompare(p->name,name) != 0)
      continue;
    if (p->next != (MagickInfo *) NULL)
      p->next->previous=p->previous;
    if (p->previous != (MagickInfo *) NULL)
      p->previous->next=p->next;
    else
      magick_list=p->next;
    magick_info=p;
    DestroyMagickInfo(&magick_info);
    status=MagickPass;
    break;
  }
  LiberateSemaphoreInfo(&magick_semaphore);
  return(status);
}
