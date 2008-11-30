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
%                      EEEEE  RRRR   RRRR   OOO   RRRR                        %
%                      E      R   R  R   R O   O  R   R                       %
%                      EEE    RRRR   RRRR  O   O  RRRR                        %
%                      E      R R    R R   O   O  R R                         %
%                      EEEEE  R  R   R  R   OOO   R  R                        %
%                                                                             %
%                                                                             %
%                      GraphicsMagick Exception Methods                       %
%                                                                             %
%                                                                             %
%                             Software Design                                 %
%                               John Cristy                                   %
%                                July 1993                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/magick.h"
#include "magick/utility.h"

/*
  Forward declarations.
*/
#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

static void
  DefaultErrorHandler(const ExceptionType,const char *,const char *),
  DefaultFatalErrorHandler(const ExceptionType,const char *,const char *),
  DefaultWarningHandler(const ExceptionType,const char *,const char *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

/*
  Global declarations.
*/
static ErrorHandler
  error_handler = DefaultErrorHandler;

static FatalErrorHandler
  fatal_error_handler = DefaultFatalErrorHandler;

static WarningHandler
  warning_handler = DefaultWarningHandler;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C a t c h E x c e p t i o n                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CatchException() returns if no exceptions is found otherwise it reports
%  the exception as a warning, error, or fatal depending on the severity.
%
%  The format of the CatchException method is:
%
%      CatchException(const ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o exception: The exception info.
%
%
*/
MagickExport void CatchException(const ExceptionInfo *exception)
{
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if (exception->severity == UndefinedException)
    return;
  errno=exception->error_number; /* Shabby work-around for parameter limits */
  if ((exception->severity >= WarningException) &&
      (exception->severity < ErrorException))
    {
      MagickWarning2(exception->severity,exception->reason,
        exception->description);
      return;
    }
  if ((exception->severity >= ErrorException) &&
      (exception->severity < FatalErrorException))
    {
      MagickError2(exception->severity,exception->reason,exception->description);
      return;
    }
  if (exception->severity >= FatalErrorException)
    {
      MagickFatalError2(exception->severity,exception->reason,
        exception->description);
      return;
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  C o p y E x c e p t i o n                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CopyException() copies exception data from one ExceptionInfo structure
%  to another.
%
%  The format of the CopyException method is:
%
%      void CopyException(ExceptionInfo *copy, const ExceptionInfo *original)
%
%  A description of each parameter follows:
%
%    o copy: The exception to copy to.
%
%    o original: The exception to copy from.
%
*/
MagickExport void CopyException(ExceptionInfo *copy, const ExceptionInfo *original)
{
  assert(copy != (ExceptionInfo *) NULL);
  assert(copy->signature == MagickSignature);
  assert(original != (ExceptionInfo *) NULL);
  assert(original->signature == MagickSignature);
  copy->severity=original->severity;
  MagickFreeMemory(copy->reason);
  if (original->reason)
    copy->reason=AcquireString(original->reason);
  MagickFreeMemory(copy->description);
  if (original->description)
    copy->description=AcquireString(original->description);
  copy->error_number=original->error_number;
  MagickFreeMemory(copy->module);
  if (original->module)
    copy->module=AcquireString(original->module);
  MagickFreeMemory(copy->function);
  if (original->function)
    copy->function=AcquireString(original->function);
  copy->line=original->line;
  return;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e f a u l t E r r o r H a n d l e r                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DefaultErrorHandler displays an error reason.
%
%  The format of the DefaultErrorHandler method is:
%
%      void DefaultMagickError(const ExceptionType severity,
%        const char *reason, const char *description)
%
%  A description of each parameter follows:
%
%    o severity: Specifies the numeric error category.
%
%    o reason: Specifies the reason to display before terminating the
%      program.
%
%    o description: Specifies any description to the reason.
%
%
*/

static const char *GetErrorMessageString(const int error_number)
{
  const char
    *message;

  message=strerror(error_number);
  if (message == (const char *) NULL)
    return("Error number is out of range");
  return(message);
}

static void DefaultErrorHandler(const ExceptionType severity,const char *reason,
  const char *description)
{
  if (reason == (char *) NULL)
    return;

  (void) fprintf(stderr,"%.1024s: ",GetClientName());
  if (strstr(reason,"%s") && description)
    {
      /*
        Reason contains printf specification. %s in reason string
        is substituted with description.
      */
      (void) fprintf(stderr,reason,description);
    }
  else
    {
      (void) fprintf(stderr,"%.1024s",reason);
      if (description != (char *) NULL)
        (void) fprintf(stderr," (%.1024s)",description);
    }
  if ((severity != OptionError) && errno)
    (void) fprintf(stderr," [%.1024s]",GetErrorMessageString(errno));
  (void) fprintf(stderr,".\n");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e f a u l t F a t a l E r r o r H a n d l e r                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DefaultFatalErrorHandler displays an error reason and then terminates
%  the program.
%
%  The format of the DefaultFatalErrorHandler method is:
%
%      void DefaultMagickFatalError(const ExceptionType severity,
%        const char *reason, const char *description)
%
%  A description of each parameter follows:
%
%    o severity: Specifies the numeric error category.
%
%    o reason: Specifies the reason to display before terminating the
%      program.
%
%    o description: Specifies any description to the reason.
%
%
*/
static void DefaultFatalErrorHandler(const ExceptionType severity,
  const char *reason,const char *description)
{
  if (reason == (char *) NULL)
    return;
  (void) fprintf(stderr,"%.1024s: %.1024s",GetClientName(),reason);
  if (description != (char *) NULL)
    (void) fprintf(stderr," (%.1024s)",description);
  if ((severity != OptionError) && errno)
    (void) fprintf(stderr," [%.1024s]",GetErrorMessageString(errno));
  (void) fprintf(stderr,".\n");
  DestroyMagick();
  Exit(severity);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e f a u l t W a r n i n g H a n d l e r                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DefaultWarningHandler displays a warning reason.
%
%  The format of the DefaultWarningHandler method is:
%
%      void DefaultWarningHandler(const ExceptionType warning,
%        const char *reason,const char *description)
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
static void DefaultWarningHandler(const ExceptionType severity,
  const char *reason,const char *description)
{
  if (reason == (char *) NULL)
    return;
  (void) fprintf(stderr,"%.1024s: %.1024s",GetClientName(),reason);
  if (description != (char *) NULL)
    (void) fprintf(stderr," (%.1024s)",description);
  if ((severity != OptionWarning) && errno)
    (void) fprintf(stderr," [%.1024s]",GetErrorMessageString(errno));
  (void) fprintf(stderr,".\n");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y E x c e p t i o n I n f o                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyExceptionInfo() deallocates memory associated with exception.
%
%  The format of the DestroyExceptionInfo method is:
%
%      void DestroyExceptionInfo(ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o exception: The exception info.
%
%
*/
MagickExport void DestroyExceptionInfo(ExceptionInfo *exception)
{
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  exception->severity=UndefinedException;
  MagickFreeMemory(exception->reason);
  MagickFreeMemory(exception->description);
  exception->error_number=0;
  MagickFreeMemory(exception->module);
  MagickFreeMemory(exception->function);
  exception->line=0UL;
  exception->signature=0UL;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t E x c e p t i o n I n f o                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetExceptionInfo() initializes an exception to default values.
%
%  The format of the GetExceptionInfo method is:
%
%      GetExceptionInfo(ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o exception: The exception info.
%
%
*/
MagickExport void GetExceptionInfo(ExceptionInfo *exception)
{
  assert(exception != (ExceptionInfo *) NULL);
  exception->severity=UndefinedException;
  exception->reason=0;
  exception->description=0;
  exception->error_number=0;
  exception->module=0;
  exception->function=0;
  exception->line=0UL;
  exception->signature=MagickSignature;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t L o c a l e E x c e p t i o n M e s s a g e                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetLocaleExceptionMessage() converts a enumerated exception severity and tag
%  to a message in the current locale.
%
%  The format of the GetLocaleExceptionMessage method is:
%
%      const char *GetLocaleExceptionMessage(const ExceptionType severity,
%        const char *tag)
%
%  A description of each parameter follows:
%
%    o severity: the severity of the exception.
%
%    o tag: the message tag.
%
%
%
*/

static const char *ExceptionSeverityToTag(const ExceptionType severity)
{
  switch (severity)
  {
    case UndefinedException: return("Unknown/Error/");
    case ResourceLimitWarning: return("Resource/Limit/Warning/");
    case TypeWarning: return("Type/Warning/");
    case OptionWarning: return("Option/Warning/");
    case DelegateWarning: return("Delegate/Warning/");
    case MissingDelegateWarning: return("Missing/Delegate/Warning/");
    case CorruptImageWarning: return("Corrupt/Image/Warning/");
    case FileOpenWarning: return("File/Open/Warning/");
    case BlobWarning: return("Blob/Warning/");
    case StreamWarning: return("Stream/Warning/");
    case CacheWarning: return("Cache/Warning/");
    case CoderWarning: return("Coder/Warning/");
    case ModuleWarning: return("Module/Warning/");
    case DrawWarning: return("Draw/Warning/");
    case ImageWarning: return("Image/Warning/");
    case WandWarning: return("Wand/Warning/");
    case XServerWarning: return("XServer/Warning/");
    case MonitorWarning: return("Monitor/Warning/");
    case RegistryWarning: return("Registry/Warning/");
    case ConfigureWarning: return("Configure/Warning/");
    case ResourceLimitError: return("Resource/Limit/Error/");
    case TypeError: return("Type/Error/");
    case OptionError: return("Option/Error/");
    case DelegateError: return("Delegate/Error/");
    case MissingDelegateError: return("Missing/Delegate/Error/");
    case CorruptImageError: return("Corrupt/Image/Error/");
    case FileOpenError: return("File/Open/Error/");
    case BlobError: return("Blob/Error/");
    case StreamError: return("Stream/Error/");
    case CacheError: return("Cache/Error/");
    case CoderError: return("Coder/Error/");
    case ModuleError: return("Module/Error/");
    case DrawError: return("Draw/Error/");
    case ImageError: return("Image/Error/");
    case WandError: return("Wand/Error/");
    case XServerError: return("XServer/Error/");
    case MonitorError: return("Monitor/Error/");
    case RegistryError: return("Registry/Error/");
    case ConfigureError: return("Configure/Error/");
    case ResourceLimitFatalError: return("Resource/Limit/FatalError/");
    case TypeFatalError: return("Type/FatalError/");
    case OptionFatalError: return("Option/FatalError/");
    case DelegateFatalError: return("Delegate/FatalError/");
    case MissingDelegateFatalError: return("Missing/Delegate/FatalError/");
    case CorruptImageFatalError: return("Corrupt/Image/FatalError/");
    case FileOpenFatalError: return("File/Open/FatalError/");
    case BlobFatalError: return("Blob/FatalError/");
    case StreamFatalError: return("Stream/FatalError/");
    case CacheFatalError: return("Cache/FatalError/");
    case CoderFatalError: return("Coder/FatalError/");
    case ModuleFatalError: return("Module/FatalError/");
    case DrawFatalError: return("Draw/FatalError/");
    case ImageFatalError: return("Image/FatalError/");
    case WandFatalError: return("Wand/FatalError/");
    case XServerFatalError: return("XServer/FatalError/");
    case MonitorFatalError: return("Monitor/FatalError/");
    case RegistryFatalError: return("Registry/FatalError/");
    case ConfigureFatalError: return("Configure/FatalError/");
    default: break;
  }
  return("");
}

MagickExport const char *GetLocaleExceptionMessage(const ExceptionType severity,
  const char *tag)
{
  char
    message[MaxTextExtent];

  const char
    *locale_message;

  /* protect against NULL lookups */
  if (tag != (char *) NULL)
    {
      /* This is a hack that depends on the fact that tag can never have spaces in
        them. If a space is found then it means we are being asked to translate a
        message that has already been translated. A big waste of time. The reason
        this happens is that messages are translated at the point of an exception
        and then again when the exception is caught and processed via the default
        error and warning handlers
      */
      if (strrchr(tag, ' '))
        return tag;
      FormatString(message,"%.1024s%.1024s",ExceptionSeverityToTag(severity),tag);
      locale_message=GetLocaleMessage(message);
      if (locale_message == message)
        return(tag);
      return(locale_message);
   }
  return(tag);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k E r r o r                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickError calls the error handler methods with an error reason.
%
%  The format of the MagickError method is:
%
%      void MagickError(const ExceptionType error,const char *reason,
%        const char *description)
%
%  A description of each parameter follows:
%
%    o exception: Specifies the numeric error category.
%
%    o reason: Specifies the reason to display before terminating the
%      program.
%
%    o description: Specifies any description to the reason.
%
%
*/
#if defined(MagickError)
MagickExport void _MagickError(const ExceptionType error,const char *reason,
  const char *description)
{
  if (error_handler != (ErrorHandler) NULL)
    (*error_handler)(error,GetLocaleExceptionMessage(error,reason),
      GetLocaleExceptionMessage(error,description));
}
#endif
#undef MagickError
MagickExport void MagickError(const ExceptionType error,const char *reason,
  const char *description)
{
  if (error_handler != (ErrorHandler) NULL)
    (*error_handler)(error,GetLocaleExceptionMessage(error,reason),
      GetLocaleExceptionMessage(error,description));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k F a t a l E r r o r                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickFatalError() calls the fatal error handler methods with an error
%  reason.
%
%  The format of the MagickError method is:
%
%      void MagickFatalError(const ExceptionType error,const char *reason,
%        const char *description)
%
%  A description of each parameter follows:
%
%    o exception: Specifies the numeric error category.
%
%    o reason: Specifies the reason to display before terminating the
%      program.
%
%    o description: Specifies any description to the reason.
%
%
*/
#if defined(MagickFatalError)
MagickExport void _MagickFatalError(const ExceptionType error,const char *reason,
  const char *description)
{
  if (fatal_error_handler != (ErrorHandler) NULL)
    (*fatal_error_handler)(error,GetLocaleExceptionMessage(error,reason),
      GetLocaleExceptionMessage(error,description));
  errno=0;
}
#endif
#undef MagickFatalError
MagickExport void MagickFatalError(const ExceptionType error,const char *reason,
  const char *description)
{
  if (fatal_error_handler != (ErrorHandler) NULL)
    (*fatal_error_handler)(error,GetLocaleExceptionMessage(error,reason),
      GetLocaleExceptionMessage(error,description));
  errno=0;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k W a r n i n g                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickWarning calls the warning handler methods with a warning
%  reason.
%
%  The format of the MagickWarning method is:
%
%      void MagickWarning(const ExceptionType warning,const char *reason,
%        const char *description)
%
%  A description of each parameter follows:
%
%    o warning: The warning severity.
%
%    o reason: Define the reason for the warning.
%
%    o description: Describe the warning.
%
%
*/
#if defined(MagickWarning)
MagickExport void _MagickWarning(const ExceptionType warning,const char *reason,
  const char *description)
{
  if (warning_handler != (WarningHandler) NULL)
    (*warning_handler)(warning,GetLocaleExceptionMessage(warning,reason),
      GetLocaleExceptionMessage(warning,description));
}
#endif
#undef MagickWarning
MagickExport void MagickWarning(const ExceptionType warning,const char *reason,
  const char *description)
{
  if (warning_handler != (WarningHandler) NULL)
    (*warning_handler)(warning,GetLocaleExceptionMessage(warning,reason),
      GetLocaleExceptionMessage(warning,description));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t E r r o r H a n d l e r                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetErrorHandler() sets the error handler to the specified method
%  and returns the previous error handler.
%
%  The format of the SetErrorHandler method is:
%
%      ErrorHandler SetErrorHandler(ErrorHandler handler)
%
%  A description of each parameter follows:
%
%    o handler: The method to handle errors.
%
%
*/
MagickExport ErrorHandler SetErrorHandler(ErrorHandler handler)
{
  ErrorHandler
    previous_handler;

  previous_handler=error_handler;
  error_handler=handler;
  return(previous_handler);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t E x c e p t i o n I n f o                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetExceptionInfo() set the exception severity.
%
%  The format of the SetExceptionInfo method is:
%
%      SetExceptionInfo(ExceptionInfo *exception,ExceptionType severity)
%
%  A description of each parameter follows:
%
%    o exception: The exception info.
%
%    o severity: The exception severity.
%
%
*/
MagickExport void SetExceptionInfo(ExceptionInfo *exception,
  ExceptionType severity)
{
  assert(exception != (ExceptionInfo *) NULL);
  exception->severity=severity;
  errno=0;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t F a t a l E r r o r H a n d l e r                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetFatalErrorHandler() sets the fatal error handler to the specified method
%  and returns the previous fatal error handler.
%
%  The format of the SetFatalErrorHandler method is:
%
%      FatalErrorHandler SetFatalErrorHandler(FatalErrorHandler handler)
%
%  A description of each parameter follows:
%
%    o handler: The method to handle errors.
%
%
*/
MagickExport FatalErrorHandler SetFatalErrorHandler(FatalErrorHandler handler)
{
  FatalErrorHandler
    previous_handler;

  previous_handler=fatal_error_handler;
  fatal_error_handler=handler;
  return(previous_handler);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t W a r n i n g H a n d l e r                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetWarningHandler() sets the warning handler to the specified method
%  and returns the previous warning handler.
%
%  The format of the SetWarningHandler method is:
%
%      ErrorHandler SetWarningHandler(ErrorHandler handler)
%
%  A description of each parameter follows:
%
%    o handler: The method to handle warnings.
%
%
*/
MagickExport WarningHandler SetWarningHandler(WarningHandler handler)
{
  WarningHandler
    previous_handler;

  previous_handler=warning_handler;
  warning_handler=handler;
  return(previous_handler);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   T h r o w E x c e p t i o n                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ThrowException() throws an exception with the specified severity code,
%  reason, and optional description.
%
%  The format of the ThrowException method is:
%
%      void ThrowException(ExceptionInfo *exception,
%        const ExceptionType severity,const char *reason,
%        const char *description)
%
%  A description of each parameter follows:
%
%    o exception: The exception.
%
%    o severity: The severity of the exception.
%
%    o reason: The reason of the exception.
%
%    o description: The exception description.
%
%
*/
#undef ThrowException
MagickExport void ThrowException(ExceptionInfo *exception,
  const ExceptionType severity,const char *reason,const char *description)
{
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  exception->severity=(ExceptionType) severity;
  MagickFreeMemory(exception->reason);
  if (reason)
    exception->reason=
      AcquireString(GetLocaleExceptionMessage(severity,reason));
  MagickFreeMemory(exception->description);
  if (description)
    exception->description=
      AcquireString(GetLocaleExceptionMessage(severity,description));
  exception->error_number=errno;
  MagickFreeMemory(exception->module);
  MagickFreeMemory(exception->function);
  exception->line=0UL;
  exception->signature=0UL;
  return;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   T h r o w L o g g e d E x c e p t i o n                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ThrowLoggedException() throws an exception with the specified severity code,
%  reason, optional description, source filename, function name, and line
%  number. If logging is enabled, the exception is also logged.
%
%  The format of the ThrowLoggedException method is:
%
%      void ThrowLoggedException(ExceptionInfo *exception,
%        const ExceptionType severity,const char *reason,
%        const char *description,const char *module,
%        const char *function,const unsigned long line
%
%  A description of each parameter follows:
%
%    o exception: The exception.
%
%    o severity: The severity of the exception.
%
%    o reason: The reason of the exception.
%
%    o description: The exception description.
%
%    o filename: The source module filename.
%
%    o function: The function name.
%
%    o line: The line number of the source module.
%
%
*/
MagickExport void ThrowLoggedException(ExceptionInfo *exception,
  const ExceptionType severity,const char *reason,const char *description,
  const char *module,const char *function,const unsigned long line)
{
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  exception->severity=(ExceptionType) severity;
  MagickFreeMemory(exception->reason);
  if (reason)
    exception->reason=
      AcquireString(GetLocaleExceptionMessage(severity,reason));
  MagickFreeMemory(exception->description);
  if (description)
    exception->description=
      AcquireString(GetLocaleExceptionMessage(severity,description));
  exception->error_number=errno;
  MagickFreeMemory(exception->module);
  if (module)
    exception->module=AcquireString(module);
  MagickFreeMemory(exception->function);
  if (function)
    exception->function=AcquireString(function);
  exception->line=line;
  if (exception->reason)
    {
      if (exception->description)
        (void) LogMagickEvent(severity,module,function,line,"%.1024s (%.1024s)",
                              exception->reason,exception->description );
      else
        (void) LogMagickEvent(severity,module,function,line,"%.1024s",
                              exception->reason);
    }
  else
    {
      (void) LogMagickEvent(severity,module,function,line,
                            "exception contains no reason!");
    }
  return;
}
