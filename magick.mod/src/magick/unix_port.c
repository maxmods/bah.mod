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
#include "magick/confirm_access.h"
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
  static long
    pagesize = -1;

  if (pagesize <= 0)
    {
#if defined(HAVE_SYSCONF) && defined(_SC_PAGE_SIZE)
      pagesize=sysconf(_SC_PAGE_SIZE);
#endif /* defined(HAVE_SYSCONF) && defined(_SC_PAGE_SIZE) */
#if defined(HAVE_GETPAGESIZE)
      if (pagesize <= 0)
	pagesize=getpagesize();
#endif /* defined(HAVE_GETPAGESIZE) */
      if (pagesize <= 0)
	pagesize=16384;
    }

  return pagesize;
}

#endif /* defined(POSIX) */
