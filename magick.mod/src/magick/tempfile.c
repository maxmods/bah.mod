/*
% Copyright (C) 2003, 2004, 2009 GraphicsMagick Group
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
% Temporary file allocation manager.
%
*/

#include "magick/studio.h"
#include "magick/error.h"
#include "magick/log.h"
#include "magick/random.h"
#include "magick/semaphore.h"
#include "magick/tempfile.h"
#include "magick/utility.h"

/*
  Define declarations.
*/
#if defined(S_IRUSR) && defined(S_IWUSR)
#  define S_MODE     (S_IRUSR | S_IWUSR)
#elif defined (MSWINDOWS)
#  define S_MODE     (_S_IREAD | _S_IWRITE)
#else
# define S_MODE      0600
#endif

#if defined(MSWINDOWS)
# if defined(_P_tmpdir) && !defined(P_tmpdir)
#  define P_tmpdir _P_tmpdir
# endif
#endif

/*
  Type definitions
*/
typedef struct _TempfileInfo
{
  char
    filename[MaxTextExtent];

  struct _TempfileInfo
    *next;
} TempfileInfo;

static TempfileInfo
  *templist = 0;

static SemaphoreInfo
  *templist_semaphore = 0;

/*
  Add a temporary file to list
*/
static void AddTemporaryFileToList(const char *filename)
{
  (void) LogMagickEvent(TemporaryFileEvent,GetMagickModule(),
    "Allocating temporary file \"%s\"",filename);
  LockSemaphoreInfo(templist_semaphore);
  {
    TempfileInfo
      *info;

    info=MagickAllocateMemory(TempfileInfo *,sizeof(TempfileInfo));
    if (info)
      {
        info->next=0;
        (void) strcpy(info->filename,filename);
        if (!templist)
          templist=info;
        else
          {
            info->next=templist;
            templist=info;
          }
      }
  }
  UnlockSemaphoreInfo(templist_semaphore);
}

/*
  Remove a temporary file from the list.
  Return MagickPass if removed or MagickFail if not found
*/
static MagickPassFail RemoveTemporaryFileFromList(const char *filename)
{
  MagickPassFail
    status=MagickFail;

  (void) LogMagickEvent(TemporaryFileEvent,GetMagickModule(),
    "Deallocating temporary file \"%s\"",filename);

  LockSemaphoreInfo(templist_semaphore);
  {
    TempfileInfo
      *current,
      *previous=0;

    for (current=templist; current; current=current->next)
      {
        if (strcmp(current->filename,filename) == 0)
          {
            if (previous)
              previous->next=current->next;
            else
              templist=current->next;
            MagickFreeMemory(current);
            status=MagickPass;
            break;
          }
        previous=current;
      }
  }
  UnlockSemaphoreInfo(templist_semaphore);
  return status;
}
/*
  Compose a temporary file name based a template string provided by
  the user.  Any 'X' characters are replaced with characters randomly
  selected from a "safe" set of characters which are unlikely to
  be interpreted by a shell or program and cause confusion.
*/
static void ComposeTemporaryFileName(char *name)
{
  static const char SafeChars[]
    = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

  char
    *c;

  for (c=name; *c; c++)
    {
      if (*c == 'X')
	*c=SafeChars[MagickRandomInteger() % (sizeof(SafeChars)-1)];
    }
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   A c q u i r e T e m p o r a r y F i l e N a m e                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AcquireTemporaryFileName replaces the contents of the string buffer pointed
%  to by filename with a unique file name.  MagickPass is returned if a file
%  name is allocated, or MagickFail is returned if there is a failure.
%  When the filename is allocated, a temporary file is created on disk with
%  zero length, and read/write permission by the user.
%  The allocated filename should be recovered via the LiberateTemporaryFile()
%  function once it is no longer required.
%
%  The format of the AcquireTemporaryFileName method is:
%
%      MagickPassFail AcquireTemporaryFileName(char *filename,
%                                              ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%   o status: MagickPass if a temporary file name is allocated and successfully
%             created on disk.  MagickFail if the temporary file name or file
%             creation fails.
%
%   o filename: Specifies a pointer to an array of characters.  The unique
%             file name is returned in this array.
%
*/
MagickExport MagickPassFail AcquireTemporaryFileName(char *filename)
{
  int
    fd;

  assert(filename != (char *) NULL);
  if ((fd=AcquireTemporaryFileDescriptor(filename)) != -1)
    {
      (void) close(fd);
      return (MagickPass);
    }
  return (MagickFail);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   A c q u i r e T e m p o r a r y F i l e D e s c r i p t o r               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AcquireTemporaryFileDescriptor replaces the contents of the string buffer
%  pointed to by filename with a unique file name created on disk.  A
%  read/write file descriptor (from open()) is returned on success, or -1 is
%  returned on failure. The temporary file is created on disk with read/write
%  permission by the user. The allocated temporary file should be deallocated
%  via the LiberateTemporaryFile() function once it is no longer required.
%
%  The format of the AcquireTemporaryFileDescriptor method is:
%
%      int AcquireTemporaryFileDescriptor(char *filename)
%
%  A description of each parameter follows.
%
%   o status: The file descriptor for an open file on success, or -1
%             on failure.
%
%   o filename: Specifies a pointer to an array of characters with an
%             allocated length of at least MaxTextExtent.  The unique
%             file name associated with the descriptor is returned in
%             this array.
%
*/
MagickExport int AcquireTemporaryFileDescriptor(char *filename)
{
  const char
    *tempdir=0;

  int
    fd=-1;

  assert(filename != (char *) NULL);
  filename[0]='\0';

  tempdir=getenv("MAGICK_TMPDIR");
#if defined(POSIX)
  if (!tempdir)
    tempdir=getenv("TMPDIR");
#endif /* POSIX */
#if defined(MSWINDOWS)
  if (!tempdir)
    tempdir=getenv("TMP");
  if (!tempdir)
    tempdir=getenv("TEMP");
#endif /* MSWINDOWS */
#if defined(P_tmpdir)
  if (!tempdir)
    tempdir=P_tmpdir;
#endif

  if (tempdir)
    {
      /*
        Use our own temporary filename generator if the temporary
        file directory is known.
      */
      char
        tempname[MaxTextExtent];
      
      int
        tries=0;
      
      for (tries=0; tries < 256; tries++)
        {
          (void) strcpy(tempname,"gmXXXXXX");
          ComposeTemporaryFileName(tempname);
          (void) strcpy(filename,tempdir);
          if (tempdir[strlen(tempdir)-1] != DirectorySeparator[0])
            (void) strcat(filename,DirectorySeparator);
          (void) strcat(filename,tempname);
          fd=open(filename,O_RDWR | O_CREAT | O_BINARY | O_EXCL, S_MODE);
          if (fd != -1)
            {
              AddTemporaryFileToList(filename);
              return (fd);
            }
        }
    }

#if HAVE_MKSTEMP
  /*
    Use mkstemp().
    Mkstemp opens the the temporary file to assure that there is
    no race condition between allocating the name and creating the
    file. This helps improve security.  However, the other cases
    also create the file in advance as well so there is not actually
    much advantage.
  */
  {
    (void) strcpy(filename,"gmXXXXXX");
    fd=mkstemp(filename);
    if (fd != -1)
      {
        AddTemporaryFileToList(filename);
	return (fd);
      }
  }


#elif HAVE_TEMPNAM
  /*
    Use tempnam().
    Windows has _tempnam which works similar to Unix tempnam.
    Note that Windows _tempnam only produces temporary file
    names which are unique to the current process so we compute
    a random part in the name ourselves. Windows _tempnam
    is documented to place sequential numbers in the file
    extension.
  */
  {
    char
      *name;

    strcpy(filename,"gmXXXXXX");
    ComposeTemporaryFileName(filename);
    if ((name=tempnam(tempdir,filename)))
      {
        (void) remove(filename);
        fd=open(name,O_RDWR | O_CREAT | O_BINARY | O_EXCL, S_MODE);
        if (fd != -1)
          {
            (void) strlcpy(filename,name,MaxTextExtent);
            AddTemporaryFileToList(filename);
          }
        else
          {
            char
              path[MaxTextExtent];

            /* Try to report a useful pathname for error reports */
            (void) strlcpy(path,tempdir,MaxTextExtent);
            if (tempdir[strlen(path)-1] != DirectorySeparator[0])
              strlcat(path,DirectorySeparator,MaxTextExtent);
            strlcat(path,filename,MaxTextExtent);
            (void) strlcpy(filename,path,MaxTextExtent);
          }

        MagickFreeMemory(name);
      }
    if (fd != -1)
      return (fd);
  }

#else
  /*
    Use ANSI C standard tmpnam
  */
  {
    if ((tmpnam(filename) == filename))
      {
        (void) remove(filename);
        fd=open(filename,O_RDWR | O_CREAT | O_BINARY | O_EXCL, S_MODE);
        if (fd != -1)
          {
            AddTemporaryFileToList(filename);
	    return (fd);
          }
      }
  }
#endif
  return fd;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   A c q u i r e T e m p o r a r y F i l e S t r e a m                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AcquireTemporaryFileStream replaces the contents of the string buffer
%  pointed to by filename with a unique file name.  A pointer to an open
%  stdio FILE stream is returned on success, or a null pointer is returned
%  on failure. The temporary file is created on disk with read/write
%  permission by the user. The allocated temporary file should be deallocated
%  via the LiberateTemporaryFile() function once it is no longer required.
%
%  The format of the AcquireTemporaryFile method is:
%
%      FILE *AcquireTemporaryFileStream(char *filename,FileIOMode text_mode)
%
%  A description of each parameter follows.
%
%   o status: The file descriptor for an open file on success, or -1
%             on failure.
%
%   o filename: Specifies a pointer to an array of characters.  The unique
%             file name is returned in this array.
%
%   o mode:  Set to TextFileIOMode to open the file in text mode, otherwise
%            the file is opened in binary mode.
%
*/
MagickExport FILE *AcquireTemporaryFileStream(char *filename,
  FileIOMode mode)
{
  int
    fd;

  if ((fd=AcquireTemporaryFileDescriptor(filename)) != -1)
    {
      if (mode == TextFileIOMode)
        return fdopen(fd,"w+");
      return fdopen(fd,"wb+");
    }

  return (FILE *) NULL;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y T e m p o r a r y F i l e s                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyTemporaryFiles reclaims all currently allocated temporary files,
%  removing the files from the filesystem if they still exist.
%
%      void LiberateTemporaryFile(void)
%
*/
MagickExport void DestroyTemporaryFiles(void)
{
  TempfileInfo
    *member,
    *liberate;

  member=templist;
  templist=0;
  while(member)
    {
      liberate=member;
      member=member->next;
      (void) LogMagickEvent(TemporaryFileEvent,GetMagickModule(),
        "Removing leaked temporary file \"%s\"",liberate->filename);
      if ((remove(liberate->filename)) != 0)
        (void) LogMagickEvent(TemporaryFileEvent,GetMagickModule(),
          "Temporary file removal failed \"%s\"",liberate->filename);
      liberate->next=0;
      MagickFreeMemory(liberate);
    }
  DestroySemaphoreInfo(&templist_semaphore);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   I n i t i a l i z e T e m p o r a r y F i l e s                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method InitializeTemporaryFiles initializes the temporary file facility
%
%  The format of the InitializeTemporaryFiles method is:
%
%      MagickPassFail InitializeTemporaryFiles(void)
%
%
*/
MagickPassFail
InitializeTemporaryFiles(void)
{
  assert(templist_semaphore == (SemaphoreInfo *) NULL);
  templist_semaphore=AllocateSemaphoreInfo();
  return MagickPass;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   L i b e r a t e T e m p o r a r y F i l e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  LiberateTemporaryFile deallocates the allocated temporary file, removing
%  the temporary file from the filesystem if it still exists. If the name
%  provided is a valid temporary file name, then the first position in the
%  string buffer is set to null in order to avoid accidental continued use.
%  If the name is not contained in the temporary file list, then it is left
%  unmodified.
%
%      MagickPassFail LiberateTemporaryFile(char *filename)
%
%  A description of each parameter follows.
%
%   o filename: Specifies a pointer to an array of characters representing
%               the temporary file to reclaim.
%
*/
MagickExport MagickPassFail LiberateTemporaryFile(char *filename)
{
  MagickPassFail
    status = MagickFail;

  if (RemoveTemporaryFileFromList(filename))
    {
      if ((remove(filename)) == 0)
        status=MagickPass;
      else
        (void) LogMagickEvent(TemporaryFileEvent,GetMagickModule(),
           "Temporary file removal failed \"%s\"",filename);
      /* Erase name so it is not accidentally used again */
      filename[0]='\0';
    }
  else
    (void) LogMagickEvent(TemporaryFileEvent,GetMagickModule(),
      "Temporary file \"%s\" to be removed not allocated!",filename);
  return (status);
}
