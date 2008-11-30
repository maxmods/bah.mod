/*
% Copyright (C) 2003 GraphicsMagick Group
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
%                    TTTTT  IIIII  M   M  EEEEE  RRRR                         %
%                      T      I    MM MM  E      R   R                        %
%                      T      I    M M M  EEE    RRRR                         %
%                      T      I    M   M  E      R R                          %
%                      T    IIIII  M   M  EEEEE  R  R                         %
%                                                                             %
%                                                                             %
%                      GraphicsMagick Timing Methods                          %
%                                                                             %
%                                                                             %
%                             Software Design                                 %
%                               John Cristy                                   %
%                              January 1993                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Contributed by Bill Radcliffe and Bob Friesenhahn.
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"

/*
  Define declarations.
*/
#if !defined(CLK_TCK)
#define CLK_TCK  sysconf(_SC_CLK_TCK)
#endif

/*
  Forward declarations.
*/
static double
  UserTime(void);

static void
  StartTimer(TimerInfo *,const unsigned int),
  StopTimer(TimerInfo *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o n t i n u e T i m e r                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ContinueTimer resumes a stopped stopwatch. The stopwatch continues
%  counting from the last StartTimer() onwards.
%
%  The format of the ContinueTimer method is:
%
%      unsigned int ContinueTimer(TimerInfo *time_info)
%
%  A description of each parameter follows.
%
%    o  time_info: Time statistics structure.
%
*/
MagickExport unsigned int ContinueTimer(TimerInfo *time_info)
{
  assert(time_info != (TimerInfo *) NULL);
  assert(time_info->signature == MagickSignature);
  if (time_info->state == UndefinedTimerState)
    return(False);
  if (time_info->state == StoppedTimerState)
    {
      time_info->user.total-=time_info->user.stop-time_info->user.start;
      time_info->elapsed.total-=
        time_info->elapsed.stop-time_info->elapsed.start;
    }
  time_info->state=RunningTimerState;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   E l a p s e d T i m e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ElapsedTime returns the elapsed time (in seconds) since the last
%  call to StartTimer().
%
%  The format of the ElapsedTime method is:
%
%      double ElapsedTime()
%
*/
static double ElapsedTime(void)
{
#if defined(HAVE_TIMES)
  struct tms
    timer;

  return((double) times(&timer)/CLK_TCK);
#else
#if defined(MSWINDOWS)
  return(NTElapsedTime());
#else
  return((double) clock()/CLK_TCK);
#endif
#endif
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t E l a p s e d T i m e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetElapsedTime returns the elapsed time (in seconds) passed between
%  the start and stop events. If the stopwatch is still running, it is stopped
%  first.
%
%  The format of the GetElapsedTime method is:
%
%      double GetElapsedTime(TimerInfo *time_info)
%
%  A description of each parameter follows.
%
%    o  time_info: Timer statistics structure.
%
*/
MagickExport double GetElapsedTime(TimerInfo *time_info)
{
  assert(time_info != (TimerInfo *) NULL);
  assert(time_info->signature == MagickSignature);
  if (time_info->state == UndefinedTimerState)
    return(0.0);
  if (time_info->state == RunningTimerState)
    StopTimer(time_info);
  return(time_info->elapsed.total);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t T i m e r I n f o                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetTimerInfo initializes the TimerInfo structure.
%
%  The format of the GetTimerInfo method is:
%
%      void GetTimerInfo(TimerInfo *time_info)
%
%  A description of each parameter follows.
%
%    o  time_info: Timer statistics structure.
%
*/
MagickExport void GetTimerInfo(TimerInfo *time_info)
{
  /*
    Create a stopwatch and start it.
  */
  assert(time_info != (TimerInfo *) NULL);
  (void) memset(time_info,0,sizeof(TimerInfo));
  time_info->state=UndefinedTimerState;
  time_info->signature=MagickSignature;
  StartTimer(time_info,True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t T i m e r R e s o l u t i o n                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Obtain the measurement resolution of the timer.
%
%  The format of the GetTimerResolution method is:
%
%      void GetTimerInfo(TimerInfo *time_info)
%
%  A description of each parameter follows.
%
%    o  time_info: Timer statistics structure.
%
*/
MagickExport double GetTimerResolution(void)
{
#if defined(MSWINDOWS)
  return (0.02);
#else
  return (1.0/CLK_TCK);
#endif
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t U s e r T i m e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetUserTime returns the User time (user and system) by the operating
%  system (in seconds) between the start and stop events. If the stopwatch is
%  still running, it is stopped first.
%
%  The format of the GetUserTime method is:
%
%      double GetUserTime(TimerInfo *time_info)
%
%  A description of each parameter follows.
%
%    o  time_info: Timer statistics structure.
%
*/
MagickExport double GetUserTime(TimerInfo *time_info)
{
  assert(time_info != (TimerInfo *) NULL);
  assert(time_info->signature == MagickSignature);
  if (time_info->state == UndefinedTimerState)
    return(0.0);
  if (time_info->state == RunningTimerState)
    StopTimer(time_info);
  return(time_info->user.total);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e s e t T i m e r                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ResetTimer resets the stopwatch.
%
%  The format of the ResetTimer method is:
%
%      void ResetTimer(TimerInfo *time_info)
%
%  A description of each parameter follows.
%
%    o  time_info: Timer statistics structure.
%
*/
MagickExport void ResetTimer(TimerInfo *time_info)
{
  assert(time_info != (TimerInfo *) NULL);
  assert(time_info->signature == MagickSignature);
  StopTimer(time_info);
  time_info->elapsed.stop=0.0;
  time_info->user.stop=0.0;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   S t a r t T i m e r                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method StartTimer starts the stopwatch.
%
%  The format of the StartTimer method is:
%
%      void StartTimer(TimerInfo *time_info,const unsigned int reset)
%
%  A description of each parameter follows.
%
%    o  time_info: Timer statistics structure.
%
%    o  reset: If reset is True, then the stopwatch is reset prior to starting.
%       If reset is False, then timing is continued without resetting the
%       stopwatch.
%
*/
static void StartTimer(TimerInfo *time_info,const unsigned int reset)
{
  assert(time_info != (TimerInfo *) NULL);
  assert(time_info->signature == MagickSignature);
  if (reset)
    {
      /*
        Reset the stopwatch before starting it.
      */
      time_info->user.total=0.0;
      time_info->elapsed.total=0.0;
    }
  if (time_info->state != RunningTimerState)
    {
      time_info->elapsed.start=ElapsedTime();
      time_info->user.start=UserTime();
    }
  time_info->state=RunningTimerState;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   S t o p T i m e r                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method StopTimer stops the stopwatch.
%
%  The format of the StopTimer method is:
%
%      void StopTimer(TimerInfo *time_info)
%
%  A description of each parameter follows.
%
%    o  time_info: Timer statistics structure.
%
*/
static void StopTimer(TimerInfo *time_info)
{
  assert(time_info != (TimerInfo *) NULL);
  assert(time_info->signature == MagickSignature);
  time_info->elapsed.stop=ElapsedTime();
  time_info->user.stop=UserTime();
  if (time_info->state == RunningTimerState)
    {
      time_info->user.total+=
        time_info->user.stop-time_info->user.start+MagickEpsilon;
      time_info->elapsed.total+=
        time_info->elapsed.stop-time_info->elapsed.start+MagickEpsilon;
    }
  time_info->state=StoppedTimerState;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   U s e r T i m e                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UserTime returns the total time the process has been scheduled (in
%  seconds) since the last call to StartTimer().
%
%  The format of the UserTime method is:
%
%      double UserTime()
%
*/
static double UserTime(void)
{
#if defined(HAVE_TIMES)
  struct tms
    timer;

  (void) times(&timer);
  return((double) (timer.tms_utime+timer.tms_stime)/CLK_TCK);
#else
#if defined(MSWINDOWS)
  return(NTUserTime());
#else
  return((double) clock()/CLK_TCK);
#endif
#endif
}
