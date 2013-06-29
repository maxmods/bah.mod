/*
% Copyright (C) 2003 - 2010 GraphicsMagick Group
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

static MagickPassFail NTstrerror_r(LONG errnum, char *strerrbuf, size_t  buflen);

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
/*
  Get a named registry value.
  Key = hkeyroot\\key, named value = name.
 */
static int
NTGetRegistryValue(HKEY hkeyroot, const char *key, const char *name,
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
      == ERROR_SUCCESS)
    {
      keytype = REG_SZ;
      cbData = *plen;
      if (bptr == (BYTE *) NULL)
	bptr = &b;  /* Registry API won't return ERROR_MORE_DATA */
      /* if ptr is NULL */
      rc = RegQueryValueExA(hkey, (char *)name, 0, &keytype, bptr, &cbData);
      RegCloseKey(hkey);
      if (rc == ERROR_SUCCESS)
	{
	  *plen = cbData;
	  return 0;  /* found environment variable and copied it */
	}
      else
	if (rc == ERROR_MORE_DATA)
	  {
	    /* buffer wasn't large enough */
	    *plen = cbData;
	    return -1;
	  }
    }
  return 1;  /* not found */
}

/*
  Find the latest version of Ghostscript installed on the system (if
  any).
*/
static MagickPassFail
NTGhostscriptFind(const char **gs_productfamily,
		  int *gs_major_version,
		  int *gs_minor_version)
{
  /*
    These are the Ghostscript product versions we will search for.
  */
  const char *products[4] =
    {
      "GPL Ghostscript",
      "GNU Ghostscript",
      "AFPL Ghostscript",
      "Aladdin Ghostscript" 
    };

  int
    product_index;

  MagickPassFail
    status;

  status=MagickFail;
  *gs_productfamily=NULL;

  (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
			"Searching for Ghostscript...");

  /* Minimum version of Ghostscript is 5.50 */
  *gs_major_version=5;
  *gs_minor_version=49;
  for (product_index=0; product_index < sizeof(products)/sizeof(products[0]);
       ++product_index)
    {
      HKEY
	hkey,
	hkeyroot;

      LONG
	winstatus;

      REGSAM
	open_key_mode;

      char
	key[MaxTextExtent],
	last_error_msg[MaxTextExtent];

      (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
			    "  Searching for %s...",
			    products[product_index]);
      FormatString(key,"SOFTWARE\\%s",products[product_index]);
      hkeyroot = HKEY_LOCAL_MACHINE;
      /*
	long WINAPI RegOpenKeyEx(const HKEY hKey, const LPCTSTR
	lpSubKey, const DWORD ulOptions, const REGSAM samDesired,
	PHKEY phkResult)
      */
      open_key_mode=KEY_READ;
#if defined(KEY_WOW64_32KEY)
      // Access a 32-bit key from either a 32-bit or 64-bit
      // application.  This flag is not supported on Windows 2000.
      // Presumably Ghostscript is registered in the 32-bit registry.
      open_key_mode |= KEY_WOW64_32KEY;
#endif // defined(KEY_WOW64_32KEY)
      if ((winstatus=RegOpenKeyExA(hkeyroot, key, 0, open_key_mode, &hkey))
	  == ERROR_SUCCESS)
	{
	  DWORD
	    cbData;

	  int
	    n;

	  (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
				"    RegOpenKeyExA() opened "
				"\"HKEY_LOCAL_MACHINE\\%s\"",
				key);
	  /* Now enumerate the keys */
	  cbData = sizeof(key) / sizeof(char);
	  n=0;
	  /*
	    LONG WINAPI RegEnumKeyEx(HKEY hKey, DWORD dwIndex, LPTSTR
	    lpName, LPDWORD lpcName, LPDWORD lpReserved, LPTSTR
	    lpClass, LPDWORD lpcClass, PFILETIME lpftLastWriteTime)

	    Enumerates the subkeys of the specified open registry key. 

	    RegEnumKeyA is is provided only for compatibility with
	    16-bit versions of Windows.
	  */
	  while ((winstatus=RegEnumKeyA(hkey, n, key, cbData)) == ERROR_SUCCESS)
	    {
	      int
		major_version,
		minor_version;

	      (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
				    "      RegEnumKeyA enumerated \"%s\"",key);
	      n++;
	      major_version=0;
	      minor_version=0;
	      if (sscanf(key,"%d.%d",&major_version,&minor_version) != 2)
		continue;

	      (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
				    "      Found Ghostscript (%s) version %d.%02d",
				    products[product_index],
				    major_version,
				    minor_version);

	      if ((major_version > *gs_major_version) ||
		  ((major_version == *gs_major_version) &&
		   (minor_version > *gs_minor_version)))
		{
		  *gs_productfamily=products[product_index];
		  *gs_major_version=major_version;
		  *gs_minor_version=minor_version;
		  status=MagickPass;
		}
	    }
	  if (winstatus != ERROR_NO_MORE_ITEMS)
	    {
	      (void) NTstrerror_r(winstatus,last_error_msg,sizeof(last_error_msg));
	      (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
				    "      RegEnumKeyA (%s)",
				    last_error_msg);
	    }
	  /*
	    LONG WINAPI RegCloseKey(HKEY hKey)
	    
	    Close the registry key.
	  */
	  winstatus=RegCloseKey(hkey);
	}
      else
	{
	  /*
	    If the function fails, the return value is a nonzero error
	    code defined in Winerror.h. You can use the FormatMessage
	    function with the FORMAT_MESSAGE_FROM_SYSTEM flag to get a
	    generic description of the error.
	   */
	  (void) NTstrerror_r(winstatus,last_error_msg,sizeof(last_error_msg));
	  (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
				"    RegOpenKeyExA() failed to open "
				"\"HKEY_LOCAL_MACHINE\\%s\" (%s)",
				key,last_error_msg);
	}
    }
  if (status != MagickFail)
    {
      (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
			    "Selected Ghostscript (%s) version %d.%02d",
			    *gs_productfamily,*gs_major_version,
			    *gs_minor_version);
    }
  else
    {
      (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
			    "Failed to find Ghostscript!");
      *gs_major_version=0;
      *gs_minor_version=0;
    }

  return status;
}


/*
  Obtain a string from the installed Ghostscript (if any).
*/
static MagickPassFail
NTGhostscriptGetString(const char *name, char *ptr, const size_t len)
{
  static const char
    *gs_productfamily=NULL;

  static int
    gs_major_version=0,
    gs_minor_version=0;

  struct
  {
    const HKEY hkey;
    const char *name;
  }
  hkeys[2] =
    {
      { HKEY_CURRENT_USER,  "HKEY_CURRENT_USER"  },
      { HKEY_LOCAL_MACHINE, "HKEY_LOCAL_MACHINE" }
    };

  int
    i,
    length;
  
  char
    key[MaxTextExtent];

  ptr[0]='\0';

  if (NULL == gs_productfamily)
    (void) NTGhostscriptFind(&gs_productfamily,&gs_major_version,
			     &gs_minor_version);

  if (NULL == gs_productfamily)
    return MagickFail;

  FormatString(key,"SOFTWARE\\%s\\%d.%02d",gs_productfamily,
	       gs_major_version, gs_minor_version);
  
  for (i=0; i < sizeof(hkeys)/sizeof(hkeys[0]); ++i)
    {
      length = len;
      if (NTGetRegistryValue(hkeys[i].hkey, key, name, ptr, &length) == 0)
	{
	  (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
				"Registry: \"%s\\%s\\%s\"=\"%s\"",
				hkeys[i].name,key,name,ptr);
	  return MagickPass;
	}
      else
	{
	  (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
				"Failed lookup: \"%s\\%s\\%s\"",
				hkeys[i].name,key,name);
	}
    }

  return MagickFail;
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
  static char
    cache[MaxTextExtent],
    *result=NULL;

  path[0]='\0';

  if (NULL == result)
    if (NTGhostscriptGetString("GS_DLL", cache, sizeof(cache)))
      result=cache;

  if (result)
    {
      (void) strlcpy(path, result, path_length);
      return TRUE;
    }

  return FALSE;
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
%   executable.  The method returns TRUE if path is updated, otherwise
%   FALSE.  When the full path value is not obtained, then the value
%   "gswin32c.exe" is used.
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
  static char
    cache[MaxTextExtent],
    *result=NULL;

  char
    *p;

  if (NULL == result)
    {
      /* Ensure a suitable default. */
      (void) strlcpy(cache,"gswin32c.exe",sizeof(cache));

      if (NTGhostscriptDLL(cache,sizeof(cache)))
	{
	  p = strrchr(cache, '\\');
	  if (p) {
	    p++;
	    *p = 0;
 	    (void) strlcat(cache,"gswin32c.exe",sizeof(cache));
	  }
	}
      result=cache;
    }

  if (result)
    {
      (void) strlcpy(path,result, path_length);
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
  char
    gs_lib_path[MaxTextExtent];

  path[0]='\0';

  if (!NTGhostscriptGetString("GS_LIB", gs_lib_path,sizeof(gs_lib_path)))
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
        (void) strlcpy(font_dir,start,Min(length+1,MaxTextExtent));
        (void) strlcpy(font_dir_file,font_dir,MaxTextExtent);
        (void) strlcat(font_dir_file,DirectorySeparator,MaxTextExtent);
        (void) strlcat(font_dir_file,"fonts.dir",MaxTextExtent);
        if (IsAccessible(font_dir_file))
          {
            (void) strlcpy(path,font_dir,path_length);
            (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
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

  if (!NTGhostscriptDLL(dll_path,sizeof(dll_path)))
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
  return (GetProcAddress(GetModuleHandle("kernel32.dll"),name) == NULL ?
          MagickFalse : MagickTrue);
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
+   N T s t r e r r o r _ r                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method NTstrerror_r formats a returned Windows error code into a
%   message string in a thread-safe manner.  MagickFail is returned if a
%   message could not be found corresponding to the error code, otherwise
%   MagickPass is returned.
%
%  The format of the NTstrerror_r method is:
%
%      MagickPassFail NTstrerror_r(LONG errnum, char *strerrbuf, size_t  buflen)
%
%  A description of each parameter follows:
%
%    o errnum: Windows error number
%
%    o strerrbuf: A buffer in which to write the message.
%
%    o buflen: The allocation length of the buffer.
%
*/
static MagickPassFail
NTstrerror_r(LONG errnum, char *strerrbuf, size_t  buflen)
{
  MagickPassFail
    status;

  LPVOID
    buffer;

  status=MagickFail;
  if (buflen > 0)
    strerrbuf[0]='\0';
  if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		    FORMAT_MESSAGE_FROM_SYSTEM,NULL,errnum,
		    MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
		    (LPTSTR) &buffer,0,NULL))
    {
      if (strlcpy(strerrbuf,buffer,buflen) < buflen)
	{
	  size_t
	    index;

	  for (index=0; strerrbuf[index] != 0; index++)
	    if (strerrbuf[index] == '\015')
	      strerrbuf[index]='\0';
	  status=MagickPass;
	}
      LocalFree(buffer);
    }
  return status;
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
