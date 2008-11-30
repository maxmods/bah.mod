/* 
% Copyright (C) 2003, 2004 GraphicsMagick Group 
% 
% This program is covered by multiple licenses, which are described in 
% Copyright.txt. You should have received a copy of Copyright.txt with this 
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html. 
% 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
%                                                                             % 
%                                                                             % 
%                                                                             % 
%                  L       OOO    CCCC   AAA   L      EEEEE                   % 
%                  L      O   O  C      A   A  L      E                       % 
%                  L      O   O  C      AAAAA  L      EEE                     % 
%                  L      O   O  C      A   A  L      E                       % 
%                  LLLLL   OOO    CCCC  A   A  LLLLL  EEEEE                   % 
%                                                                             % 
%                                                                             % 
%                   GraphicsMagick Locale Message Methods                     % 
%                                                                             % 
%                                                                             % 
%                              Software Design                                % 
%                            William T. Radcliffe                             % 
%                                 July 2003                                   % 
%                                                                             % 
%                                                                             % 
%                                                                             % 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
% 
*/ 
 
/* 
  Include declarations. 
*/ 
#include "studio.h"
#include "utility.h" 
#define _INCLUDE_CATEGORYMAP_TABLE_
#define _INCLUDE_SEVERITYMAP_TABLE_
#define _INCLUDE_TAGMAP_TABLE_
#if defined(MAGICK_WINDOWS_MESSAGE_TABLES)
#  include "spinlock.h"
#else
#  define _INCLUDE_MESSAGE_TABLE_
#endif
#include "locale_c.h"
 
/*
  Static declaractions.
*/
#if defined(MAGICK_WINDOWS_MESSAGE_TABLES)
#define MAX_CACHE_SIZE 32

static char
  cache[MaxTextExtent * MAX_CACHE_SIZE];

static const char *
AllocateManagedString(const char *s)
{
  char
    *cs;

  static int
    index = -1;

  SPINLOCK_WAIT;
  index++;
  if (index >= MAX_CACHE_SIZE)
    index=0;
  cs=&cache[MaxTextExtent * index];
  (void) strlcpy(cs,s,MaxTextExtent);
  SPINLOCK_RELEASE;
  return cs;
}

static const char *
NTFormatMessage(DWORD id, ...)
{
  va_list
    args;

  const char
    *result;

  char
    temp[MaxTextExtent];

  HMODULE
    handle;

  int
    status;

  LPVOID
    buffer;

  va_start( args, id );

  buffer = (LPVOID) NULL; /* stop compiler from complaining */
  FormatString(temp,"%.1024s%.1024s%.1024s",SetClientPath((char *) NULL),
               DirectorySeparator,SetClientFilename((char *) NULL));
  if (IsAccessibleNoLogging(temp))
    handle=GetModuleHandle(temp);
  else
    handle=GetModuleHandle(0);
  if (handle)
    {
      /*
        Sample of how to change threads locale explicitly to English.
        you can use this same API call to switch locales on a thread
        by thread basis and extract the correct localized message
      */
      /*
        SetThreadLocale( MAKELCID( MAKELANGID( 0x0409, SUBLANG_NEUTRAL
        ), SORT_DEFAULT ) );
      */
      status=FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                           FORMAT_MESSAGE_FROM_HMODULE,
                           handle,
                           id,
                           MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
                           (LPTSTR) &buffer,
                           0,
                           &args
                           );
      if (!status)
        {
          FormatString(temp,"Undefined message %ld (0x%08lx)",(long) id,
                       (long) id);
          result=AllocateManagedString(temp);
        }
      else
        {
          char
            *s;

          int
            len;

          s = buffer;
          len = strlen(s);
          if (len > 1)
            {
              /*
                The string that is stored in the resource has a CR LF
                on the end, so we need to zap this off before
                returning it
              */
              s[len-1]='\0';
              s[len-2]='\0';
            }
          result=AllocateManagedString(s);
          LocalFree(buffer);
        }
    }
  else
    {
      FormatString(temp,"Undefined message %ld (0x%08lx)",(long) id, (long) id);
      result=AllocateManagedString(temp);
    }
  return(result);
}
#endif /* defined(MAGICK_WINDOWS_MESSAGE_TABLES) */

static void
ChopLocaleComponents(char *path,const unsigned long components)
{
  long
    count;

  register char
    *p;

  if (*path == '\0')
    return;
  p=path+strlen(path)-1;
  if (*p == '/')
    *p='\0';
  for (count=0; (count < (long) components) && (p > path); p--)
    if (*p == '/')
      {
        *p='\0';
        count++;
      }
}

static const char *
GetLocaleMessageFromTag(const char *tag)
{
#if defined(_INCLUDE_TAGMAP_TABLE_)
  char
    category[MaxTextExtent],
    severity[MaxTextExtent];

  register int
    i,
    j,
    k;

  (void) strlcpy(category,tag,MaxTextExtent);
  ChopLocaleComponents(category,2);
  for (k=0; category_map[k].name != 0; k++)
    {
      if (LocaleCompare(category,category_map[k].name) == 0)
        {
          (void) strlcpy(severity,tag,MaxTextExtent);
          ChopLocaleComponents(severity,1);
          for (j=category_map[k].offset; j < category_map[k+1].offset; j++)
            {
              if (LocaleCompare(severity,severity_map[j].name) == 0)
                {
                  for (i=severity_map[j].offset; i < severity_map[j+1].offset; i++)
                    {
                      int
                        prefix,
                        taglen;

                      prefix=strlen(severity);
                      taglen=strlen(tag);
                      if ((prefix > 0) && (prefix < taglen) &&
                          LocaleCompare(&tag[prefix+1],message_map[i].name) == 0)
                        {
#if defined(_INCLUDE_MESSAGE_TABLE_)
                          return message_dat[message_map[i].messageid];
#else
#  if defined(MAGICK_WINDOWS_MESSAGE_TABLES)
                          return NTFormatMessage(message_map[i].messageid);
#  else
                          return tag;
#  endif /* defined(MAGICK_WINDOWS_MESSAGE_TABLES) */
#endif /* defined(_INCLUDE_MESSAGE_TABLE_) */
                        }
                    }
                }
            }
        }
    }
#endif
  return tag;
}

/* 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
%                                                                             % 
%                                                                             % 
%                                                                             % 
%   G e t L o c a l e M e s s a g e                                           % 
%                                                                             % 
%                                                                             % 
%                                                                             % 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
% 
%  GetLocaleMessage() returns a message in the current locale that matches the 
%  supplied tag. 
% 
%  The format of the GetLocaleMessage method is: 
% 
%      const char *GetLocaleMessage(const char *tag) 
% 
%  A description of each parameter follows: 
% 
%    o tag: Return a message that matches this tag in the current locale. 
% 
% 
*/
MagickExport const char *
GetLocaleMessage(const char *tag)
{
  return GetLocaleMessageFromTag(tag);
}

/*
  This routine translates a severity code to it's string value. It is
  slow but the idea is to eventually move away from using strings to
  lookup any messages and use binary codes instead.
 */
#if 0 /* not currently used */
static const char *
SeverityToTag(const ExceptionType severity)
{
  register int
    i;

  for (i=0; severity_map[i].name != 0; i++)
  {
    if (severity == severity_map[i].severityid)
      return severity_map[i].name;
  }
  return("");
}

/*
  This routine is intended to be a replacement for
  GetLocaleExceptionMessage in the code, but using the data from the
  locale MGK files to do it's job rather then some hard coded case
  statement.
 */
static const char *
GetLocaleMessageFromSeverityAndTag(const ExceptionType severity,
                                   const char *tag)
{
  char
    message[MaxTextExtent];

  const char
    *locale_message;

  /*
    This is a hack that depends on the fact that tag can never have
    spaces in them. If a space is found then it means we are being
    asked to translate a message that has already been translated. A
    big waste of time. The reason this happens is that messages are
    translated at the point of an exception and then again when the
    exception is caught and processed via the default error and
    warning handlers
  */
  if (strrchr(tag, ' '))
    return(tag);
  FormatString(message,"%.1024s%.1024s",SeverityToTag(severity),tag);
  locale_message=GetLocaleMessage(message);
  if (locale_message == message)
    return(tag);
  return(locale_message);
}
#endif /* not currently used */

/*
  This routine is used to lookup a message directly form an id pulled
  from the header file generated by the coders\local.c header file
  coder.
 */
MagickExport const char *
GetLocaleMessageFromID(const int id)
{
  if ((id > 0) && (id <= MAX_LOCALE_MSGS))
    {
#if defined(MAGICK_WINDOWS_MESSAGE_TABLES)
      return NTFormatMessage(id);
#else
      return message_dat[id];
#endif
    }
  return (const char *) NULL;
}
