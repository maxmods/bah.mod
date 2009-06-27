/*
% Copyright (C) 2009 GraphicsMagick Group
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/confirm_access.h"
#include "magick/utility.h"

/*
  Global declarations.
*/
static ConfirmAccessHandler
  confirm_access_handler = (ConfirmAccessHandler) NULL;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k C o n f i r m A c c e s s                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickConfirmAccess() calls the access confirmation handler method with
%  parameters which describe the requested access mode and path/URL, as well
%  as an ExceptionInfo structure to update with any error information.  A
%  user-provided callback (set by MagickSetConfirmAccessHandler()) is
%  invoked.  If the callback returns MagickFail, then this function also
%  returns MagickFail, which is intended to determine if the operation may
%  continue.  The callback is expected to report the reason access is denied
%  by filling out the ExceptionInfo structure.  If the callback fails to do
%  so, then a generic "access denied" error is reported.
%
%  The format of the MagickConfirmAccess method is:
%
%      MagickPassFail MagickConfirmAccess(const ConfirmAccessMode mode,
%                                         const char *path,
%                                         ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o mode: The type of access to be performed.
%
%    o path: The local path or URL requested to be accessed.
%
%    o exception: Return any errors or warnings in this structure.
%
*/
MagickExport MagickPassFail
MagickConfirmAccess(const ConfirmAccessMode mode,
		    const char *path,
		    ExceptionInfo *exception)
{
  MagickPassFail
    status;

  assert(path != (const char *) NULL);
  status=MagickPass;
  if (confirm_access_handler != (ConfirmAccessHandler) NULL)
    status=(*confirm_access_handler)(mode,path,exception);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t C o n f i r m A c c e s s H a n d l e r                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetConfirmAccessHandler() sets the access confirmation handler to
%  the specified method and returns the previous access confirmation handler.
%  This access confirmation handler is used to "approve" access to files and
%  URLs.  If the handler returns MagickFalse, then access is denied.  This
%  mechanism may be used to enforce security policies and/or may be used to
%  monitor file and URL accesses.
%
%  The format of the MagickSetConfirmAccessHandler method is:
%
%      ConfirmAccessHandler MagickSetConfirmAccessHandler(ConfirmAccessHandler handler)
%
%  A description of each parameter follows:
%
%    o handler: Specifies a pointer to a method to handle access confirmation.
%
%
*/
MagickExport ConfirmAccessHandler
MagickSetConfirmAccessHandler(ConfirmAccessHandler handler)
{
  ConfirmAccessHandler
    previous_handler;

  previous_handler=confirm_access_handler;
  confirm_access_handler=handler;
  return(previous_handler);
}
