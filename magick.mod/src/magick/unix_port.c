/*
% Copyright (C) 2007 GraphicsMagick Group
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                       U   U  N   N  IIIII  X   X                            %
%                       U   U  NN  N    I     X X                             %
%                       U   U  N N N    I      X                              %
%                       U   U  N  NN    I     X X                             %
%                        UUU   N   N  IIIII  X   X                            %
%                                                                             %
%                                                                             %
%                       GraphicsMagick UNIX Methods                           %
%                                                                             %
%                                                                             %
%                             Software Design                                 %
%                             Bob Friesenhahn                                 %
%                                July 2007                                    %
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
#if defined(POSIX)
/* some of these may have already been included by studio.h */
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

#include "magick/utility.h"

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
MagickExport long MagickGetMMUPageSize(void)
{
  long
    pagesize;

  pagesize=-1;

#if defined(HAVE_SYSCONF) && defined(_SC_PAGE_SIZE)
  pagesize=sysconf(_SC_PAGE_SIZE);
#endif /* defined(HAVE_SYSCONF) && defined(_SC_PAGE_SIZE) */
#if defined(HAVE_GETPAGESIZE)
  if (pagesize <= 0)
    pagesize=getpagesize();
#endif /* defined(HAVE_GETPAGESIZE) */
  if (pagesize <= 0)
    pagesize=16384;

  return pagesize;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S p a w n V P                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSpawnVP() executes an external command with arguments provided by
%  an argument vector.  The return status of the executed command is returned
%  if it is executed, or -1 is returned if the command could not be executed.
%  Executed commands will normally return zero if they execute without error.
%
%  The format of the MagickSpawnVP method is:
%
%      int MagickSpawnVP(const char *file, char *const argv[])
%
%  A description of each parameter follows:
%
%    o file:  Name of the command to execute.
%
%    o argv:  Argument vector. First argument in the vector should be
%             the name of the command.  The argument vector is terminated
%             via a NULL pointer.
%
*/
MagickExport int MagickSpawnVP(const unsigned int verbose,const char *file, char *const argv[])
{
  int
    status;

  char
    message[MaxTextExtent];

  pid_t
    child_pid;

  status = -1;
  message[0]='\0';
  errno=0;

  child_pid = fork( );
  if ( (pid_t)-1 == child_pid)
    {
      /* Failed to fork, errno contains reason */
      status = -1;
      FormatString(message,"fork failed: %.1024s", strerror(errno));
    }
  else if ( 0 == child_pid )
    {
      /* We are the child process, exec program with arguments. */
      status = execvp(file, argv);

      /* If we get here, then execvp must have failed. */
      (void) fprintf(stderr, "execvp failed, errno = %d (%s)\n",errno,strerror(errno));

      /* If there is an execvp error, then call _exit() */
      _exit(1);
    }
  else
    {
      /* We are the parent process, wait for child. */
      pid_t waitpid_status;
      int child_status = 0;
      waitpid_status = waitpid(child_pid, &child_status, 0);
      if ( (pid_t)-1 == waitpid_status )
        {
          /* Waitpid error */
          status = -1;
          FormatString(message, "waitpid failed: %.1024s", strerror(errno));
        }
      else if ( waitpid_status == child_pid )
        {
          /* Status is available for child process */
          if ( WIFEXITED( child_status ) )
            {
              status =  WEXITSTATUS( child_status );
            }
          else if ( WIFSIGNALED( child_status ) )
            {
              int sig_num = WTERMSIG( child_status );
              status = -1;
              FormatString(message, "child process quit due to signal %d\n", sig_num);
            }
        }
    }

  /*
    Provide a verbose/dignostic message in a form which is easy for
    the user to understand.
  */
  if (verbose || (status != 0))
  {
    const char
      *message_p = (const char *) NULL;

    char
      *command;

    unsigned int
      i;

    command = AllocateString((const char*) NULL);
    for (i = 0; argv[i] != (const char*) NULL; i++)
      {
        char
          buffer[MaxTextExtent];

        FormatString(buffer,"\"%.1024s\"", argv[i]);

        if (0 != i)
          (void) ConcatenateString(&command," ");

        (void) ConcatenateString(&command,buffer);
      }
    if (message[0] != '\0')
      message_p = message;
    MagickError2(DelegateError,command,message_p);
    MagickFreeMemory(command);
  }

  return status;
}

#endif /* defined(POSIX) */
