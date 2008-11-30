/*
% Copyright (C) 2003, 2005 GraphicsMagick Group
% Copyright (C) 2000-2002, Ghostgum Software Pty Ltd.  All rights reserved.
% Copyright (C) 2002 ImageMagick Studio
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                                 N   N  TTTTT                                %
%                                 NN  N    T                                  %
%                                 N N N    T                                  %
%                                 N  NN    T                                  %
%                                 N   N    T                                  %
%                                                                             %
%                                                                             %
%                Windows NT Utility Methods for GraphicsMagick                %
%                                                                             %
%                                                                             %
%                               Software Design                               %
%                                 John Cristy                                 %
%                                December 1996                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

#include "magick/studio.h"
#if defined(MSWINDOWS)
/*
  Include declarations.
*/
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/utility.h"
#include "magick/version.h"
#if defined(HasLTDL)
#  include "ltdl.h"
#endif /* defined(HasLTDL) */
#include "magick/nt_base.h"

/*
  Static declarations.
*/
#if !defined(HasLTDL)
static char
  *NTslsearchpath = (char *) NULL;
#endif
static void
  *gs_dll_handle = (void *)NULL;
static GhostscriptVectors
    gs_vectors;


/*
  External declarations.
*/
#if !defined(MSWINDOWS)
extern "C" BOOL WINAPI
  DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved);
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T c l o s e d i r                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method NTclosedir closes the named directory stream and frees the DIR
%  structure.
%
%  The format of the NTclosedir method is:
%
%      void NTclosedir(DIR *entry)
%
%  A description of each parameter follows:
%
%    o entry: Specifies a pointer to a DIR structure.
%
%
*/
MagickExport int NTclosedir(DIR *entry)
{
  assert(entry != (DIR *) NULL);
  FindClose(entry->hSearch);
  MagickFreeMemory(entry);
  return 0;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   DllMain                                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DllMain is an entry point to the DLL which is called when processes
%  and threads are initialized and terminated, or upon calls to the
%  Windows LoadLibrary and FreeLibrary functions.
%
%  The function returns TRUE of it succeeds, or FALSE if initialization fails.
%
%  The format of the DllMain method is:
%
%    BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved)
%
%  A description of each parameter follows:
%
%    o hinstDLL: handle to the DLL module
%   
%    o fdwReason: reason for calling function.
%
%          May have values:
%             DLL_PROCESS_ATTACH - DLL is being loaded into virtual address
%                                  space of current process.
%             DLL_THREAD_ATTACH - Indicates that the current process is
%                                 creating a new thread.  Called under the
%                                 context of the new thread.
%             DLL_THREAD_DETACH - Indicates that the thread is exiting.
%                                 Called under the context of the exiting
%                                 thread.
%             DLL_PROCESS_DETACH - Indicates that the DLL is being unloaded
%                                  from the virtual address space of the
%                                  current process.
%
%    o lpvReserved: Used for passing additional info during DLL_PROCESS_ATTACH
%                   and DLL_PROCESS_DETACH.
%
*/
#if defined(_DLL) && defined( ProvideDllMain )
BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved)
{
  switch ( fdwReason )
    {
    case DLL_PROCESS_ATTACH:
      {
#define ENV_VAR_MAX 32767 /* environment variable max is 32,767 bytes */
#define DLL_PATH_MAX 1024

        char
          dll_path[DLL_PATH_MAX],
          current_path[ENV_VAR_MAX];
        
        long count;
        
        count = GetModuleFileName(hinstDLL,dll_path,DLL_PATH_MAX);
        if (count) {
          for ( ; count>0 ; --count)
            {
              if (dll_path[count]=='\\')
                {
                  dll_path[count+1]='\0';
                  break;
                }
            }
          InitializeMagick(dll_path);
          count = GetEnvironmentVariable("PATH",current_path,ENV_VAR_MAX);
          if (strstr(current_path,dll_path) == NULL)
            {
              if (strlen(dll_path)+count+1 < ENV_VAR_MAX-1)
                {
                  char
                    new_path[ENV_VAR_MAX];

                  FormatString(new_path,"%.*s;%.*s",DLL_PATH_MAX,dll_path,
                               ENV_VAR_MAX-DLL_PATH_MAX-1,current_path);
                  SetEnvironmentVariable("PATH",new_path);
                }
            }
          break;
        }
        /* If we get here, then GetModuleFileName must have failed */
        return FALSE;
        break;
      }
    case DLL_PROCESS_DETACH:
      DestroyMagick();
      break;
    default:
      {
        /* Do nothing */
      }
    }
  return TRUE;
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   E x i t                                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Exit calls TerminateProcess for Win95.
%
%  The format of the Exit method is:
%
%      int Exit(int status)
%
%  A description of each parameter follows:
%
%    o status: an integer value representing the status of the terminating
%      process.
%
%
*/
MagickExport int Exit(int status)
{
  if (IsWindows95())
    TerminateProcess(GetCurrentProcess(),(unsigned int) status);
  exit(status);
  return(0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t M M U P a g e S i z e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetMMUPageSize() returns the VM pagesize used by the MMU. The VM
%  pagesize is the number of bytes retrieved due to one page fault.
%
%  The format of the MagickGetMMUPageSize method is:
%
%      long MagickGetMMUPageSize()
%
*/
MagickExport long MagickGetMMUPageSize()
{
  return 4096;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s W i n d o w s 9 5                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsWindows95 returns true if the system is Windows 95.
%
%  The format of the IsWindows95 method is:
%
%      int IsWindows95()
%
%  A description of each parameter follows:
%
%    o status: an integer value representing the status of the terminating
%      process.
%
%
*/
MagickExport MagickBool IsWindows95()
{
  OSVERSIONINFO
    version_info;

  version_info.dwOSVersionInfoSize=sizeof(version_info);
  if (GetVersionEx(&version_info) &&
      (version_info.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS))
    return(MagickTrue);
  return(MagickFalse);
}

#if !defined(HasLTDL)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T d l c l o s e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method NTdlclose unloads the module associated with the passed handle.
%   Zero is returned on success.
%
%  The format of the NTdlclose method is:
%
%      void NTdlclose(void *handle)
%
%  A description of each parameter follows:
%
%    o handle: Specifies a handle to a previously loaded dynamic module.
%
*/
MagickExport int NTdlclose(void *handle)
{
  /* FreeLibrary returns zero for failure */
  return (!(FreeLibrary(handle)));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T d l e r r o r                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method NTdlerror returns a pointer to a string describing the last error
%   associated with a NTdl* function. Note that this function is not thread
%   safe so it should only be used under the protection of a lock.
%
%  The format of the NTdlerror method is:
%
%      const char * NTdlerror(void)
%
%  A description of each parameter follows:
%
*/
MagickExport const char *NTdlerror(void)
{
  static char
    last_error[MaxTextExtent];

  char
    *error;

  last_error[0]='\0';
  error=NTGetLastError();
  if (error)
    strlcpy(last_error,error,MaxTextExtent);
  MagickFreeMemory(error);
  return (last_error);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T d l e x i t                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   NTdlexit() exits the dynamic module loading subsystem.
%
%  The format of the NTdlexit method is:
%
%      int NTdlexit(void)
%
*/
MagickExport int NTdlexit(void)
{
  return(0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T d l i n i t                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method NTdlinit initializes the dynamic module loading subsystem.
%
%  The format of the NTdlinit method is:
%
%      int NTdlinit(void)
%
*/
MagickExport int NTdlinit(void)
{
  return(0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T d l o p e n                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method NTdlopen loads a dynamic module into memory and returns a handle
%   that can be used to access the various procedures in the module.
%
%  The format of the NTdlopen method is:
%
%      void *NTdlopen(const char *filename)
%
%  A description of each parameter follows:
%
%    o path: Specifies a pointer to string representing dynamic module that
%            is to be loaded.
%
*/
MagickExport void *NTdlopen(const char *filename)
{
#define MaxPathElements  31

  char
    buffer[MaxTextExtent];

  int
    index;

  register char
    *p,
    *q;

  register int
    i;

  void
    *handle;

  UINT
    errorMode;

  // Set error mode so that dialog box is not displayed on error.
  errorMode=SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOOPENFILEERRORBOX);

  // Load library via name
  handle=(void *) LoadLibrary(filename);

  // If library failed to load, but a search path is defined, then
  // attempt to load library via search path.
  if ((handle == (void *) NULL) && (NTslsearchpath != NULL))
    {
      p=NTslsearchpath;
      index=0;
      while (index < MaxPathElements)
        {
          q=strchr(p,DirectoryListSeparator);
          if (q == (char *) NULL)
            {
              (void) strlcpy(buffer,p,MaxTextExtent);
              (void) strlcat(buffer,"\\",MaxTextExtent);
              (void) strlcat(buffer,filename,MaxTextExtent);
              handle=(void *) LoadLibrary(buffer);
              break;
            }
          i=q-p;
          (void) strncpy(buffer,p,i);
          buffer[i]='\0';
          (void) strlcat(buffer,"\\",MaxTextExtent);
          (void) strlcat(buffer,filename,MaxTextExtent);
          handle=(void *) LoadLibrary(buffer);
          if (handle)
            break;
          p=q+1;
        }
    }

  // Restore original error handling mode.
  SetErrorMode(errorMode);

  return(handle);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T d l s e t s e a r c h p a t h                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method NTdlsetsearchpath sets the current locations that the subsystem
%   should look at to find dynamically loadable modules.
%
%  The format of the NTdlsetsearchpath method is:
%
%      int NTdlsetsearchpath(char *path)
%
%  A description of each parameter follows:
%
%    o path: Specifies a pointer to string representing the search path
%            for DLL's that can be dynamically loaded.
%
*/
MagickExport int NTdlsetsearchpath(const char *path)
{
  if (NTslsearchpath)
    {
      MagickFreeMemory(NTslsearchpath);
      NTslsearchpath=(char *) NULL;
    }
  if (path != (char *) NULL)
    NTslsearchpath=AllocateString(path);
  return (0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T d l s y m                                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method NTdlsym retrieve the procedure address of the procedure specified
%   by the passed character string.
%
%  The format of the NTdlsym method is:
%
%      void *NTdlsym(void *handle,const char *name)
%
%  A description of each parameter follows:
%
%    o handle: Specifies a handle to the previously loaded dynamic module.
%
%    o name: Specifies the procedure entry point to be returned.
%
*/
MagickExport void *NTdlsym(void *handle,const char *name)
{
  LPFNDLLFUNC1
    lpfnDllFunc1;

  lpfnDllFunc1=(LPFNDLLFUNC1) GetProcAddress(handle,name);
  if (!lpfnDllFunc1)
    return((void *) NULL);
  return((void *) lpfnDllFunc1);
}
#endif /* !defined(HasLTDL) */

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  N T m u n m a p                                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method NTmunmap emulates the POSIX munmap function.
%
%  The format of the NTmunmap method is:
%
%      int NTmunmap(void *map,size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method NTmunmap returns 0 on success; otherwise, it
%      returns -1 and sets errno to indicate the error.
%
%    o map: The address of the binary large object.
%
%    o length: The length of the binary large object.
%
%
*/
MagickExport int NTmunmap(void *map,size_t length)
{
  if (!UnmapViewOfFile(map))
    return(-1);
  return(0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T E l a p s e d T i m e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method NTElapsedTime returns the elapsed time (in seconds) since the last
%  call to StartTimer().
%
%  The format of the ElapsedTime method is:
%
%      double NTElapsedTime(void)
%
%
*/
MagickExport double NTElapsedTime(void)
{
  union
  {
    FILETIME
      filetime;

    __int64
      filetime64;
  } elapsed_time;

  SYSTEMTIME
    system_time;

  GetSystemTime(&system_time);
  SystemTimeToFileTime(&system_time,&elapsed_time.filetime);
  return((double) 1.0e-7*elapsed_time.filetime64);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   N T E r r o r H a n d l e r                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method NTErrorHandler displays an error reason and then terminates
%  the program.
%
%  The format of the NTErrorHandler method is:
%
%      void NTErrorHandler(const ExceptionType error,const char *reason,
%        const char *description)
%
%  A description of each parameter follows:
%
%    o error: Specifies the numeric error category.
%
%    o reason: Specifies the reason to display before terminating the
%      program.
%
%    o description: Specifies any description to the reason.
%
%
*/
MagickExport void NTErrorHandler(const ExceptionType error,const char *reason,
  const char *description)
{
  char
    buffer[3*MaxTextExtent];

  if (reason == (char *) NULL)
    {
      DestroyMagick();
      Exit(0);
    }
  if ((description != (char *) NULL) && errno)
    FormatString(buffer,"%.1024s: %.1024s (%.1024s) [%.1024s].\n",
      GetClientName(),reason,description,strerror(errno));
  else
    if (description != (char *) NULL)
      FormatString(buffer,"%.1024s: %.1024s (%.1024s).\n",
        GetClientName(),reason,description);
    else
      if (errno)
        FormatString(buffer,"%.1024s: %.1024s [%.1024s].\n",
          GetClientName(),reason,strerror(errno));
      else
        FormatString(buffer,"%.1024s: %.1024s.\n",GetClientName(),
          reason);
  (void) MessageBox(NULL,buffer,"GraphicsMagick Exception",MB_OK | MB_TASKMODAL |
    MB_SETFOREGROUND | MB_ICONEXCLAMATION);
  DestroyMagick();
  Exit(0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T G e t E x e c u t i o n P a t h                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method NTGetExecutionPath returns the execution path of a program.
%
%  The format of the GetExecutionPath method is:
%
%      unsigned int NTGetExecutionPath(char *path)
%
%
*/
MagickExport MagickPassFail NTGetExecutionPath(char *path)
{
  GetModuleFileName(0,path,MaxTextExtent);
  return(MagickPass);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T G e t L a s t E r r o r                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method NTGetLastError returns the last error that occurred.
%
%  The format of the NTGetLastError method is:
%
%      char *NTGetLastError(void)
%
%  A description of each parameter follows:
%
*/
char *NTGetLastError(void)
{
  char
    *reason;

  int
    status;

  LPVOID
    buffer;

  status=FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
    FORMAT_MESSAGE_FROM_SYSTEM,NULL,GetLastError(),
    MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),(LPTSTR) &buffer,0,NULL);
  if (!status)
    reason=AllocateString("An unknown error occurred");
  else
    {
      reason=AllocateString((const char *)buffer);
      LocalFree(buffer);
    }
  return(reason);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T G h o s t s c r i p t D L L                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method NTGhostscriptDLL obtains the path to the latest Ghostscript DLL.
%   The method returns False if a value is not obtained.
%
%  The format of the NTGhostscriptDLL method is:
%
%      int NTGhostscriptDLL(char *path, int path_length)
%
%  A description of each parameter follows:
%
%    o path: Pointer to buffer in which to return result.
%
%    o path_length: Length of buffer
%
*/

/* Portions of the following code fall under the following copyright: */

/* Copyright (C) 2000-2002, Ghostgum Software Pty Ltd.  All rights reserved.

  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this file ("Software"), to deal in the Software without
  restriction, including without limitation the rights to use, copy,
  modify, merge, publish, distribute, sublicense, and/or sell copies of
  this Software, and to permit persons to whom this file is furnished to
  do so, subject to the following conditions:

  This Software is distributed with NO WARRANTY OF ANY KIND.  No author
  or distributor accepts any responsibility for the consequences of using it,
  or for whether it serves any particular purpose or works at all, unless he
  or she says so in writing.

  The above copyright notice and this permission notice shall be included
  in all copies or substantial portions of the Software.
*/

#define GS_PRODUCT_AFPL "AFPL Ghostscript"
#define GS_PRODUCT_ALADDIN "Aladdin Ghostscript"
#define GS_PRODUCT_GNU "GNU Ghostscript"
#define GS_PRODUCT_GPL "GPL Ghostscript"
#define GS_MINIMUM_VERSION 550

/* Get Ghostscript versions for given product.
 * Store results starting at pver + 1 + offset.
 * Returns total number of versions in pver.
 */
static int NTGhostscriptProductVersions(int *pver, int offset,
    const char *gs_productfamily)
{
  HKEY
    hkey,
    hkeyroot;

  DWORD
    cbData;

  char
    key[256],
    *p;

  int
    n = 0,
    ver;

  sprintf(key, "Software\\%s", gs_productfamily);
  hkeyroot = HKEY_LOCAL_MACHINE;
  if (RegOpenKeyExA(hkeyroot, key, 0, KEY_READ, &hkey) == ERROR_SUCCESS) {
    /* Now enumerate the keys */
    cbData = sizeof(key) / sizeof(char);
    while (RegEnumKeyA(hkey, n, key, cbData) == ERROR_SUCCESS) {
      n++;
      ver = 0;
      p = key;
      while (*p && (*p!='.')) {
        ver = (ver * 10) + (*p - '0')*100;
        p++;
      }
      if (*p == '.')
        p++;
      if (*p) {
        ver += (*p - '0') * 10;
        p++;
      }
      if (*p)
        ver += (*p - '0');
      if (n + offset < pver[0])
        pver[n+offset] = ver;
    }
  }
  return n+offset;
}

/* Query registry to find which versions of Ghostscript are installed.
 * Return version numbers in an integer array.
 * On entry, the first element in the array must be the array size
 * in elements.
 * If all is well, TRUE is returned.
 * On exit, the first element is set to the number of Ghostscript
 * versions installed, and subsequent elements to the version
 * numbers of Ghostscript.
 * e.g. on entry {5, 0, 0, 0, 0}, on exit {3, 550, 600, 596, 0}
 * Returned version numbers may not be sorted.
 *
 * If Ghostscript is not installed at all, return FALSE
 * and set pver[0] to 0.
 * If the array is not large enough, return FALSE
 * and set pver[0] to the number of Ghostscript versions installed.
 */

static int NTGhostscriptEnumerateVersions(int *pver)
{
  int
    n;

  assert(pver != (int *) NULL);

  n = NTGhostscriptProductVersions(pver, 0, GS_PRODUCT_AFPL);
  n = NTGhostscriptProductVersions(pver, n, GS_PRODUCT_ALADDIN);
  n = NTGhostscriptProductVersions(pver, n, GS_PRODUCT_GNU);
  n = NTGhostscriptProductVersions(pver, n, GS_PRODUCT_GPL);

  if (n >= pver[0]) {
    pver[0] = n;
    return FALSE;  /* too small */
  }

  if (n == 0) {
    pver[0] = 0;
    return FALSE;  /* not installed */
  }
  pver[0] = n;
  return TRUE;
}

/*
 * Get a named registry value.
 * Key = hkeyroot\\key, named value = name.
 * name, ptr, plen and return values are the same as in gp_getenv();
 */

static int  NTGetRegistryValue(HKEY hkeyroot, const char *key, const char *name,
    char *ptr, int *plen)
{
  HKEY
    hkey;

  DWORD
    cbData,
    keytype;

  BYTE
    b,
    *bptr = (BYTE *)ptr;

  LONG
    rc;

  if (RegOpenKeyExA(hkeyroot, key, 0, KEY_READ, &hkey)
      == ERROR_SUCCESS) {
    keytype = REG_SZ;
    cbData = *plen;
    if (bptr == (BYTE *)NULL)
      bptr = &b;  /* Registry API won't return ERROR_MORE_DATA */
    /* if ptr is NULL */
    rc = RegQueryValueExA(hkey, (char *)name, 0, &keytype, bptr, &cbData);
    RegCloseKey(hkey);
    if (rc == ERROR_SUCCESS) {
      *plen = cbData;
      return 0;  /* found environment variable and copied it */
    } else if (rc == ERROR_MORE_DATA) {
      /* buffer wasn't large enough */
      *plen = cbData;
      return -1;
    }
  }
  return 1;  /* not found */
}

static int NTGhostscriptGetProductString(int gs_revision, const char *name,
    char *ptr, int len, const char *gs_productfamily)
{
  /* If using Win32, look in the registry for a value with
   * the given name.  The registry value will be under the key
   * HKEY_CURRENT_USER\Software\AFPL Ghostscript\N.NN
   * or if that fails under the key
   * HKEY_LOCAL_MACHINE\Software\AFPL Ghostscript\N.NN
   * where "AFPL Ghostscript" is actually gs_productfamily
   * and N.NN is obtained from gs_revision.
   */

  int
    code,
    length;

  char
    dotversion[16],
    key[256];

  DWORD version = GetVersion();

  if (((HIWORD(version) & 0x8000) != 0)
      && ((HIWORD(version) & 0x4000) == 0)) {
    /* Win32s */
    return FALSE;
  }

  sprintf(dotversion, "%d.%02d",
          (int)(gs_revision / 100), (int)(gs_revision % 100));
  sprintf(key, "Software\\%s\\%s", gs_productfamily, dotversion);

  length = len;
  code = NTGetRegistryValue(HKEY_CURRENT_USER, key, name, ptr, &length);
  if ( code == 0 )
    return TRUE;  /* found it */

  length = len;
  code = NTGetRegistryValue(HKEY_LOCAL_MACHINE, key, name, ptr, &length);

  if ( code == 0 )
    return TRUE;  /* found it */

  return FALSE;
}

static int NTGhostscriptGetString(int gs_revision, const char *name, char *ptr, int len)
{
  if (NTGhostscriptGetProductString(gs_revision, name, ptr, len, GS_PRODUCT_AFPL))
    return TRUE;
  if (NTGhostscriptGetProductString(gs_revision, name, ptr, len, GS_PRODUCT_ALADDIN))
    return TRUE;
  if (NTGhostscriptGetProductString(gs_revision, name, ptr, len, GS_PRODUCT_GNU))
    return TRUE;
  if (NTGhostscriptGetProductString(gs_revision, name, ptr, len, GS_PRODUCT_GPL))
    return TRUE;
  return FALSE;
}

static int NTGetLatestGhostscript( void )
{
  int
    count,
    i,
    gsver,
    *ver;

  DWORD version = GetVersion();
  if ( ((HIWORD(version) & 0x8000)!=0) && ((HIWORD(version) & 0x4000)==0) )
    return FALSE;  /* win32s */

  count = 1;
  NTGhostscriptEnumerateVersions(&count);
  if (count < 1)
    return FALSE;
  ver = (int *)malloc((count+1)*sizeof(int));
  if (ver == (int *)NULL)
    return FALSE;
  ver[0] = count+1;
  if (!NTGhostscriptEnumerateVersions(ver)) {
    free(ver);
    return FALSE;
  }
  gsver = 0;
  for (i=1; i<=ver[0]; i++) {
    if (ver[i] > gsver)
      gsver = ver[i];
  }
  free(ver);
  return gsver;
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T G h o s t s c r i p t D L L                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method NTGhostscriptDLL obtains the path to the latest Ghostscript DLL.
%   The method returns False if a value is not obtained.
%
%  The format of the NTGhostscriptDLL method is:
%
%      int NTGhostscriptDLL( char *path, int path_length)
%
%  A description of each parameter follows:
%
%    o path: Pointer to path buffer to update
%
%    o path_length: Allocation size of path buffer.
%%
*/
MagickExport int NTGhostscriptDLL(char *path, int path_length)
{
  int
    gsver;

  char
    buf[256];

  *path='\0';
  gsver = NTGetLatestGhostscript();
  if ((gsver == FALSE) || (gsver < GS_MINIMUM_VERSION))
    return FALSE;

  if (!NTGhostscriptGetString(gsver, "GS_DLL", buf, sizeof(buf)))
    return FALSE;

  strlcpy(path, buf, path_length);
  return TRUE;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T G h o s t s c r i p t D L L V e c t o r s                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method NTGhostscriptDLLVectors returns a GhostscriptVectors structure
%   containing function vectors to invoke Ghostscript DLL functions. A null
%   pointer is returned if there is an error with loading the DLL or
%   retrieving the function vectors.
%
%  The format of the NTGhostscriptDLLVectors method is:
%
%      const GhostscriptVectors *NTGhostscriptDLLVectors( void )
%
%%
*/
MagickExport const GhostscriptVectors *NTGhostscriptDLLVectors( void )
{
  if (NTGhostscriptLoadDLL())
    return &gs_vectors;

  return (GhostscriptVectors*) NULL;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T G h o s t s c r i p t E X E                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method NTGhostscriptEXE obtains the path to the latest Ghostscript
%   executable.  The method returns False if a value is not obtained.
%
%  The format of the NTGhostscriptEXE method is:
%
%      int NTGhostscriptEXE(char *path, int path_length)
%
%  A description of each parameter follows:
%
%    o path: Pointer to buffer in which to return result.
%
%    o path_length: Length of buffer
%
*/
MagickExport int NTGhostscriptEXE(char *path, int path_length)
{
  int
    gsver;

  char
    buf[256],
    *p;

  *path='\0';
  gsver = NTGetLatestGhostscript();
  if ((gsver == FALSE) || (gsver < GS_MINIMUM_VERSION))
    return FALSE;

  if (!NTGhostscriptGetString(gsver, "GS_DLL", buf, sizeof(buf)))
    return FALSE;

  p = strrchr(buf, '\\');
  if (p) {
    p++;
    *p = 0;
    strncpy(p, "gswin32c.exe", sizeof(buf)-1-strlen(buf));
    strncpy(path, buf, path_length-1);
    return TRUE;
  }

  return FALSE;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T G h o s t s c r i p t F o n t s                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method NTGhostscriptFonts obtains the path to the Ghostscript fonts.
%   The method returns False if a value is not obtained.
%
%  The format of the NTGhostscriptFonts method is:
%
%      int NTGhostscriptFonts(char *path, int path_length)
%
%  A description of each parameter follows:
%
%    o path: Pointer to buffer in which to return result.
%
%    o path_length: Length of buffer
%
*/
MagickExport int NTGhostscriptFonts(char *path, int path_length)
{
  int
    ghostscript_version;

  char
    gs_lib_path[MaxTextExtent];

  *path='\0';
  ghostscript_version = NTGetLatestGhostscript();
  if ((ghostscript_version == FALSE) || (ghostscript_version < GS_MINIMUM_VERSION))
    return FALSE;

  if (!NTGhostscriptGetString(ghostscript_version, "GS_LIB", gs_lib_path,
                              sizeof(gs_lib_path)))
    return FALSE;

  /*
    The format of the GS_LIB string is a path similar to
    "c:\gs8.14\lib;C:\gs\fonts;C:\gs\gs8.14\Resource". Ghostscript
    7.0X does not include the Resource entry.
  */
  {
    const char
      *end = NULL,
      *start = gs_lib_path;
        
    end=start+strlen(start);
    while ( start < end )
      {
        char
          font_dir[MaxTextExtent],
          font_dir_file[MaxTextExtent];
            
        const char
          *seperator;
            
        int
          length;
            
        seperator = strchr(start,DirectoryListSeparator);
        if (seperator)
          length=seperator-start;
        else
          length=end-start;
        strlcpy(font_dir,start,Min(length+1,MaxTextExtent));
        strlcpy(font_dir_file,font_dir,MaxTextExtent);
        strlcat(font_dir_file,DirectorySeparator,MaxTextExtent);
        strlcat(font_dir_file,"fonts.dir",MaxTextExtent);
        if (IsAccessible(font_dir_file))
          {
            strlcpy(path,font_dir,path_length);
            (void) LogMagickEvent(AnnotateEvent,GetMagickModule(),
                                  "Ghostscript fonts in directory \"%s\"",
                                  path);
            return TRUE;
          }
        start += length+1;
      }
  }

  return FALSE;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T G h o s t s c r i p t L o a d D L L                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method NTGhostscriptLoadDLL attempts to load the Ghostscript DLL
%   and returns True if it succeeds.
%
%  The format of the NTGhostscriptLoadDLL method is:
%
%      int NTGhostscriptLoadDLL(void)
%
%%
*/
MagickExport int NTGhostscriptLoadDLL(void)
{
  char
    dll_path[MaxTextExtent];

  if (gs_dll_handle != (void *) NULL)
    return True;

  if(!NTGhostscriptDLL(dll_path,sizeof(dll_path)))
    return False;

  gs_dll_handle = lt_dlopen(dll_path);
  if (gs_dll_handle == (void *) NULL)
    return False;

  memset((void*)&gs_vectors, 0, sizeof(GhostscriptVectors));

  gs_vectors.exit=(int (MagickDLLCall *)(gs_main_instance*))
    lt_dlsym(gs_dll_handle,"gsapi_exit");
  gs_vectors.init_with_args=(int (MagickDLLCall *)(gs_main_instance *, int, char **))
    (lt_dlsym(gs_dll_handle,"gsapi_init_with_args"));
  gs_vectors.new_instance=(int (MagickDLLCall *)(gs_main_instance **, void *))
    (lt_dlsym(gs_dll_handle,"gsapi_new_instance"));
  gs_vectors.run_string=(int (MagickDLLCall *)(gs_main_instance *, const char *, int, int *))
    (lt_dlsym(gs_dll_handle,"gsapi_run_string"));
  gs_vectors.delete_instance=(void (MagickDLLCall *)(gs_main_instance *))
    (lt_dlsym(gs_dll_handle,"gsapi_delete_instance"));

  if ((gs_vectors.exit==NULL) ||
      (gs_vectors.init_with_args==NULL) ||
      (gs_vectors.new_instance==NULL) ||
      (gs_vectors.run_string==NULL) ||
      (gs_vectors.delete_instance==NULL))
    return False;

  return True;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T G h o s t s c r i p t U n L o a d D L L                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method NTGhostscriptUnLoadDLL unloads the Ghostscript DLL if it is loaded.
%
%  The format of the NTGhostscriptUnLoadDLL method is:
%
%      int NTGhostscriptUnLoadDLL(void)
%
%%
*/
MagickExport int NTGhostscriptUnLoadDLL(void)
{
  if (gs_dll_handle != (void *) NULL)
    {
      lt_dlclose(gs_dll_handle);
      gs_dll_handle=(void *) NULL;
      memset((void*)&gs_vectors, 0, sizeof(GhostscriptVectors));
      return True;
    }

  return False;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T K e r n e l A P I S u p p o r t e d                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method NTKernelAPISupported tests to see if an API symbol is defined in
%  kernel32.dll. If it is defined, then presumably the interface can safely
%  be used without crashing.
%
%  The format of the NTKernelAPISupported method is:
%
%      MagickBool NTKernelAPISupported(const char *name)
%
%  A description of each parameter follows:
%
%    o return: MagickTrue if the symbol is defined, otherwise MagickFalse.
%
%    o name: Symbol name.
%
*/
MagickExport MagickBool NTKernelAPISupported(const char *name)
{
  HMODULE 
    handle = 0;

  MagickBool
    status = MagickFalse;

  // Presumably LoadLibrary is reference counted.
  handle=LoadLibrary("kernel32.dll");
  if (handle == NULL)
    {
      (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
                            "Failed to load \"kernel32.dll\" using LoadLibrary()");
    }
  else
    {
      // If we can resolve the symbol, then it is supported.
      if (GetProcAddress(handle,name))
        status = MagickTrue;

      // FreeLibrary is reference counted.
      FreeLibrary(handle);
    }

  return (status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T R e g i s t r y K e y L o o k u p                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method NTRegistryKeyLookup returns package installation path settings
%  stored in the Windows Registry. Path settings are specific to the
%  installed package version so that multiple package installations may
%  coexist.
%
%  Values are stored in the registry under a base path path similar to
%  "HKEY_LOCAL_MACHINE/SOFTWARE\GraphicsMagick\1.0\Q:8". The provided subkey
%  is appended to this base path to form the full key.
%
%  The format of the NTRegistryKeyLookup method is:
%
%      char *NTRegistryKeyLookup(const char *subkey)
%
%  A description of each parameter follows:
%
%    o return: Returns an allocated string containing the value of the key.
%           This allocation should be freed by the user once it is no longer
%           needed.
%
%    o key: Specifies a string that identifies the registry object.
%           Currently supported sub-keys include: "BinPath", "ConfigurePath",
%           "LibPath", "CoderModulesPath", "FilterModulesPath", "SharePath".
%
*/
MagickExport unsigned char *NTRegistryKeyLookup(const char *subkey)
{
  static HKEY
    reg_key = (HKEY) INVALID_HANDLE_VALUE;

  /*
    Look-up base-key for first access only
  */
  if (reg_key == (HKEY) INVALID_HANDLE_VALUE)
    {
      char
        package_key[MaxTextExtent];

      LONG
        res;
      
      FormatString(package_key,"SOFTWARE\\%s\\%s\\Q:%d", MagickPackageName,
                   MagickLibVersionText,QuantumDepth);

      res = RegOpenKeyExA (HKEY_LOCAL_MACHINE, package_key, 0, KEY_READ,
                           &reg_key);

      if (res != ERROR_SUCCESS)
        {
          reg_key = (HKEY) INVALID_HANDLE_VALUE;
          return 0;
        }
    }

  /*
    Look-up sub-key
  */
  {
    unsigned char
      *dest;
    
    DWORD
      size,
      type;

    LONG
      res;
    
    size = 32;
    dest = MagickAllocateMemory(unsigned char *,size);
    
    res = RegQueryValueExA (reg_key, subkey, 0, &type, dest, &size);
    if (res == ERROR_MORE_DATA && type == REG_SZ)
      {
        MagickReallocMemory(unsigned char *,dest,size);
        res = RegQueryValueExA (reg_key, subkey, 0, &type, dest, &size);
      }
    
    if (type != REG_SZ || res != ERROR_SUCCESS)
      {
        MagickFreeMemory(dest);
      }
    
    return dest;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T R e s o u r c e T o B l o b                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method NTResourceToBlob returns a blob containing the contents of the
%  resource in the current executable specified by the id parameter. This
%  is currently used to retrieve MGK files tha have been embedded into
%  the various command line utilities.
%
%  The format of the telldir method is:
%
%      unsigned char *NTResourceToBlob(const char *id)
%
%  A description of each parameter follows:
%
%    o id: Specifies a string that identifies the resource.
%
%
*/
MagickExport unsigned char *NTResourceToBlob(const char *id)
{
  char
    directory[MaxTextExtent];

  DWORD
    length;

  HGLOBAL
    global;

  HMODULE
    handle;

  HRSRC
    resource;

  unsigned char
    *blob,
    *value;

  assert(id != (const char *) NULL);
  FormatString(directory,"%.1024s%.1024s%.1024s",GetClientPath(),
    DirectorySeparator,GetClientFilename());
  if (IsAccessible(directory))
    handle=GetModuleHandle(directory);
  else
    handle=GetModuleHandle(0);
  if (!handle)
    return((unsigned char *) NULL);
  /*
    Locate a resource matching the specified type and name in the
    specified module.
  */
  resource=FindResource(handle,id,"IMAGEMAGICK");
  if (!resource)
  {
    (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
      "Tried: windows resource \"%.1024s\"",id);
    return((unsigned char *) NULL);
  }
  (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
    "Found: windows resource \"%.1024s\"",id);
  /*
    Load resource into global memory.
  */
  global=LoadResource(handle,resource);
  if (!global)
    return((unsigned char *) NULL);
  /*
    Obtain the size (in bytes) of the specified resource.
  */
  length=SizeofResource(handle,resource);
  /*
    Lock the resource in memory.
  */
  value=(unsigned char *) LockResource(global);
  if (!value)
    {
      FreeResource(global); /* Obsolete 16 bit API */
      return((unsigned char *) NULL);
    }
  blob=MagickAllocateMemory(unsigned char *,length+1);
  if (blob != (unsigned char *) NULL)
    {
      (void) memcpy(blob,value,length);
      blob[length]='\0';
    }
  UnlockResource(global); /* Obsolete 16 bit API with no replacement */
  FreeResource(global); /* Obsolete 16 bit API */
  return(blob);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T S y s t e m C o m m a n d                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method NTSystemComman executes the specified command and waits until it
%   terminates.  The returned value is the exit status of the command.
%
%  The format of the NTSystemComman method is:
%
%      int NTSystemComman(const char *command)
%
%  A description of each parameter follows:
%
%    o command: This string is the command to execute.
%
%
*/
MagickExport int NTSystemComman(const char *command)
{
  char
    local_command[MaxTextExtent];

  DWORD
    child_status;

  int
    status;

  PROCESS_INFORMATION
    process_info;

  STARTUPINFO
    startup_info;

  unsigned int
    background_process;

  if (command == (char *) NULL)
    return(-1);
  GetStartupInfo(&startup_info);
  startup_info.dwFlags=STARTF_USESHOWWINDOW;
  startup_info.wShowWindow=SW_SHOWMINNOACTIVE;
  (void) strlcpy(local_command,command,MaxTextExtent);
  background_process=command[strlen(command)-1] == '&';
  if (background_process)
    local_command[strlen(command)-1]='\0';
  if (command[strlen(command)-1] == '|')
     local_command[strlen(command)-1]='\0';
   else
     startup_info.wShowWindow=SW_SHOWDEFAULT;
  status=CreateProcess((LPCTSTR) NULL,local_command,
    (LPSECURITY_ATTRIBUTES) NULL,(LPSECURITY_ATTRIBUTES) NULL,(BOOL) FALSE,
    (DWORD) NORMAL_PRIORITY_CLASS,(LPVOID) NULL,(LPCSTR) NULL,&startup_info,
    &process_info);
  if (status == 0)
    return(-1);
  if (background_process)
    return(status == 0);

#if 1
  // This code has been used for years, but supposedly may
  // cause problems in GUIs.
  status=WaitForSingleObject(process_info.hProcess,INFINITE);
  if (status != WAIT_OBJECT_0)
    return (status);
#else
  // Following code doesn't work correctly yet
  while(1)
  {
    // Wait for state change or message received
    DWORD result=MsgWaitForMultipleObjects(1, &process_info.hProcess, FALSE,
      INFINITE, QS_ALLEVENTS);
    // Loop if return was due to message received (which we don't care about).
    if (result == (WAIT_OBJECT_0+1))
      continue;
    // If return was due to handle state change, then object is available.
    if ((result - WAIT_OBJECT_0) == 0)
      break;
   }
#endif

  status=GetExitCodeProcess(process_info.hProcess,&child_status);
  if (status == 0)
    return(-1);
  CloseHandle(process_info.hProcess);
  CloseHandle(process_info.hThread);
  return((int) child_status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T U s e r T i m e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method NTUserTime returns the total time the process has been scheduled (i
%  seconds) since the last call to StartTimer().
%
%  The format of the UserTime method is:
%
%      double NTUserTime(void)
%
%
*/
MagickExport double NTUserTime(void)
{
  DWORD
    status;

  FILETIME
    create_time,
    exit_time;

  OSVERSIONINFO
    OsVersionInfo;

  union
  {
    FILETIME
      filetime;

    __int64
      filetime64;
  } kernel_time;

  union
  {
    FILETIME
      filetime;

    __int64
      filetime64;
  } user_time;

  OsVersionInfo.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
  GetVersionEx(&OsVersionInfo);
  if (OsVersionInfo.dwPlatformId != VER_PLATFORM_WIN32_NT)
    return(NTElapsedTime());
  status=GetProcessTimes(GetCurrentProcess(),&create_time,&exit_time,
    &kernel_time.filetime,&user_time.filetime);
  if (status != TRUE)
    return(0.0);
  return((double) 1.0e-7*(kernel_time.filetime64+user_time.filetime64));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T W a r n i n g H a n d l e r                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method NTWarningHandler displays a warning reason.
%
%  The format of the NTWarningHandler method is:
%
%      void NTWarningHandler(const ExceptionType warning,const char *reason,
%        const char *description)
%
%  A description of each parameter follows:
%
%    o warning: Specifies the numeric warning category.
%
%    o reason: Specifies the reason to display before terminating the
%      program.
%
%    o description: Specifies any description to the reason.
%
%
*/
MagickExport void NTWarningHandler(const ExceptionType warning,
  const char *reason,const char *description)
{
  char
    buffer[2*MaxTextExtent];

  if (reason == (char *) NULL)
    return;
  if (description == (char *) NULL)
    FormatString(buffer,"%.1024s: %.1024s.\n",
      GetClientName(),reason);
  else
    FormatString(buffer,"%.1024s: %.1024s (%.1024s).\n",
      GetClientName(),reason,description);
  (void) MessageBox(NULL,buffer,"GraphicsMagick Warning",MB_OK | MB_TASKMODAL |
    MB_SETFOREGROUND | MB_ICONINFORMATION);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T f t r u n c a t e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method NTftruncate truncates a file to the specified size.  If the file is
%  longer than the specified size, it is shortened to the specified size. If
%  the file is shorter than the specified size, it is extended to the
%  specified size by filling with zeros.
%  This is a POSIX compatability function.
%
%  The format of the NTftruncate method is:
%
%      int NTftruncate(int filedes, off_t length)
%
%  A description of each parameter follows:
%
%    o status: Zero is returned on successful completion. Otherwise -1
%        is returned and errno is set to indicate the error.
%
%    o filedes: File descriptor from the _open() call.
%
%    o length: Desired file length.
%
*/
MagickExport int NTftruncate(int filedes, off_t length)
{
  int
    status;

  magick_off_t
    current_pos;

  status=0;
  current_pos=MagickTell(filedes);

  /*
    Truncate file to size, filling any extension with nulls.
    Notice that this interface is limited to 2GB due to its
    use of a 'long' offset. Ftruncate also has this shortcoming
    if off_t is a 'long'.

    A way to support more than 2GB is to use SetFilePointerEx()
    to set the file position followed by SetEndOfFile() to set
    the file EOF to the current file position. This approach does
    not ensure that bytes in the extended portion are null.

    The CreateFileMapping() function may also be used to extend a
    file's length. The filler byte values are not defined in the
    documentation.
  */ 
  status=chsize(filedes,length);

  /*
    It is not documented if _chsize preserves the seek 
    position, so restore the seek position like ftruncate
    does
  */
  if (!status)
    status=MagickSeek(filedes,current_pos,SEEK_SET);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  N T m m a p                                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method NTmmap emulates POSIX mmap. Supports PROT_READ, PROT_WRITE
%  protection options, and MAP_SHARED, MAP_PRIVATE, MAP_ANON flags.
%  Passing a file descriptor of -1 along with the MAP_ANON flag option returns
%  a memory allocation from the system page file with the specified allocated
%  length.
%
%  The format of the NTmmap method is:
%
%    MagickExport void *NTmmap(char *address, size_t length, int protection,
%      int flags, int file, magick_off_t offset)
%
%
*/
MagickExport void *NTmmap(char *address,size_t length,int protection,int flags,
  int file,magick_off_t offset)
{
  void
    *map;

  HANDLE
    file_handle,
    shmem_handle;

  DWORD
    length_low,
    length_high,
    offset_low,
    offset_high;

  DWORD
    access_mode=0,
    protection_mode=0;

  map=(void *) NULL;
  shmem_handle=INVALID_HANDLE_VALUE;
  file_handle=INVALID_HANDLE_VALUE;

  offset_low=(DWORD) (offset & 0xFFFFFFFFUL);
  offset_high=(DWORD) ((offset >> 32) & 0xFFFFFFFFUL);

  length_low=(DWORD) (length & 0xFFFFFFFFUL);
  length_high=(DWORD) ((((magick_off_t) length) >> 32) & 0xFFFFFFFFUL);

  if (protection & PROT_WRITE)
    {
      access_mode=FILE_MAP_WRITE;
      if (flags & MAP_PRIVATE)
        {
          // Copy on write (updates are private)
          access_mode=FILE_MAP_COPY;
          protection_mode=PAGE_WRITECOPY;
        }
      else
        {
          // Updates are shared
          protection_mode=PAGE_READWRITE;
        }
    }
  else if (protection & PROT_READ)
    {
      access_mode=FILE_MAP_READ;
      protection_mode=PAGE_READONLY;
    }

  if ((file == -1) && (flags & MAP_ANON))
    // Similar to using mmap on /dev/zero to allocate memory from paging area.
    file_handle=INVALID_HANDLE_VALUE;
  else
    file_handle=(HANDLE) _get_osfhandle(file);

  shmem_handle=CreateFileMapping(file_handle,0,protection_mode,length_high,
                                 length_low,0);
  if (shmem_handle)
    {
      map=(void *) MapViewOfFile(shmem_handle,access_mode,offset_high,
                                 offset_low,length);
      CloseHandle(shmem_handle);
    }

  if (map == (void *) NULL)
    return((void *) MAP_FAILED);
  return((void *) ((char *) map));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  N T m s y n c                                                              %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method NTmsync emulates the Unix msync function except that the flags
%  argument is ignored. Windows page sync behaves mostly like MS_SYNC
%  except that if the file is accessed over a network, the updates are not
%  fully synchronous unless a special flag is provided when the file is
%  opened.  It is not clear if flushing a range invalidates copy pages
%  like Unix msync does.
%
%  The format of the NTmsync method is:
%
%      int NTmsync(void *addr, size_t len, int flags)
%
%  A description of each parameter follows:
%
%    o status:  Method NTmsync returns 0 on success; otherwise, it
%      returns -1 and sets errno to indicate the error.
%
%    o addr: The address of the binary large object.
%
%    o len: The length of the binary large object.
%
%    o flags: Option flags (ignored for Windows)
%
%
*/
MagickExport int NTmsync(void *addr, size_t len, int flags)
{
  if (!FlushViewOfFile(addr,len))
    return(-1);
  return(0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T o p e n d i r                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method NTopendir opens the directory named by filename and associates
%  a directory stream with it.
%
%  The format of the opendir method is:
%
%      DIR *NTopendir(const char *path)
%
%  A description of each parameter follows:
%
%    o entry: Specifies a pointer to a DIR structure.
%
%
*/
MagickExport DIR *NTopendir(const char *path)
{
  char
    file_specification[MaxTextExtent];

  DIR
    *entry;

  assert(path != (char *) NULL);
  if (strlcpy(file_specification,path,MaxTextExtent) >= MaxTextExtent)
    return (DIR *) NULL;;
  if (strlcat(file_specification,DirectorySeparator,MaxTextExtent) >= MaxTextExtent)
    return (DIR *) NULL;;
  entry=MagickAllocateMemory(DIR *,sizeof(DIR));
  if (entry != (DIR *) NULL)
    {
      entry->firsttime=TRUE;
      entry->hSearch=FindFirstFile(file_specification,&entry->Win32FindData);
    }
  if (entry->hSearch == INVALID_HANDLE_VALUE)
    {
      if (strlcat(file_specification,"\\*.*",MaxTextExtent) >= MaxTextExtent)
        {
          MagickFreeMemory(entry);
          return (DIR *) NULL;
        }
      entry->hSearch=FindFirstFile(file_specification,&entry->Win32FindData);
      if (entry->hSearch == INVALID_HANDLE_VALUE)
        {
          MagickFreeMemory(entry);
          return (DIR *) NULL;
        }
    }
  return(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T r e a d d i r                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method NTreaddir returns a pointer to a structure representing the
%  directory entry at the current position in the directory stream to
%  which entry refers.
%
%  The format of the readdir
%
%      NTreaddir(entry)
%
%  A description of each parameter follows:
%
%    o entry: Specifies a pointer to a DIR structure.
%
%
*/
MagickExport struct dirent *NTreaddir(DIR *entry)
{
  int
    status;

  if (entry == (DIR *) NULL)
    return ((struct dirent *) NULL);
  if (!entry->firsttime)
    {
      status=FindNextFile(entry->hSearch,&entry->Win32FindData);
      if (status == 0)
        return ((struct dirent *) NULL);
    }
  if (strlcpy(entry->file_info.d_name,entry->Win32FindData.cFileName,
              sizeof(entry->file_info.d_name)) >=
      sizeof(entry->file_info.d_name))
    return ((struct dirent *) NULL);
  entry->firsttime=FALSE;
  entry->file_info.d_namlen=strlen(entry->file_info.d_name);
  return (&entry->file_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T s e e k d i r                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method NTseekdir sets the position of the next NTreaddir() operation
%   on the directory stream.
%
%  The format of the NTseekdir method is:
%
%      void NTseekdir(DIR *entry,long position)
%
%  A description of each parameter follows:
%
%    o entry: Specifies a pointer to a DIR structure.
%
%    o position: specifies the position associated with the directory
%      stream.
%
%
%
*/
MagickExport void NTseekdir(DIR *entry,long position)
{
  assert(entry != (DIR *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T t e l l d i r                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method NTtelldir returns the current location associated  with  the
%   named directory stream.
%
%  The format of the NTtelldir method is:
%
%      long NTtelldir(DIR *entry)
%
%  A description of each parameter follows:
%
%    o entry: Specifies a pointer to a DIR structure.
%
%
*/
MagickExport long NTtelldir(DIR *entry)
{
  assert(entry != (DIR *) NULL);
  return(0);
}
#endif
