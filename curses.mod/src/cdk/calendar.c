#include <cdk_int.h>

/*
 * $Author: tom $
 * $Date: 2006/05/05 00:27:45 $
 * $Revision: 1.84 $
 */

#define YEAR2INDEX(year) (((year) >= 1900) ? ((year) - 1900) : (year))

/*
 * Declare file local variables.
 */
static char *monthsOfTheYear[] = {
   "NULL",
   "January",
   "February",
   "March",
   "April",
   "May",
   "June",
   "July",
   "August",
   "September",
   "October",
   "November",
   "December"
};

static int daysOfTheMonth[] = {
   -1,
   31,
   28,
   31,
   30,
   31,
   30,
   31,
   31,
   30,
   31,
   30,
   31
};

/*
 * Declare file local prototypes.
 */
static int getMonthLength (int year, int month);
static int getMonthStartWeekday (int year, int month);
static time_t getCurrentTime (CDKCALENDAR *calendar);
static void verifyCalendarDate (CDKCALENDAR *calendar);
static void incrementCalendarDay (CDKCALENDAR *calendar, int adjust);
static void decrementCalendarDay (CDKCALENDAR *calendar, int adjust);
static void incrementCalendarMonth (CDKCALENDAR *calendar, int adjust);
static void decrementCalendarMonth (CDKCALENDAR *calendar, int adjust);
static void incrementCalendarYear (CDKCALENDAR *calendar, int adjust);
static void decrementCalendarYear (CDKCALENDAR *calendar, int adjust);
static void drawCDKCalendarField (CDKCALENDAR *calendar);

DeclareCDKObjects(CALENDAR, Calendar, setCdk, Int);

/*
 * This creates a calendar widget.
 */
CDKCALENDAR *newCDKCalendar(CDKSCREEN *cdkscreen, int xplace, int yplace, char *title, int day, int month, int year, chtype dayAttrib, chtype monthAttrib, chtype yearAttrib, chtype highlight, boolean Box, boolean shadow)
{
   /* Maintain the calendar information. */
   CDKCALENDAR *calendar	= 0;
   int parentWidth		= getmaxx(cdkscreen->window);
   int parentHeight		= getmaxy(cdkscreen->window);
   int boxWidth			= 24;
   int boxHeight		= 11;
   int xpos			= xplace;
   int ypos			= yplace;
   int x;
   struct tm *dateInfo;
   time_t clck;
   char *dayname		= "Su Mo Tu We Th Fr Sa";

   static const struct { int from; int to; } bindings[] = {
	    { 'T',		KEY_HOME },
	    { 't',		KEY_HOME },
	    { 'n',		KEY_NPAGE },
	    { CDK_FORCHAR,	KEY_NPAGE },
	    { 'p',		KEY_PPAGE },
	    { CDK_BACKCHAR,	KEY_PPAGE },
   };

   if ((calendar = newCDKObject(CDKCALENDAR, &my_funcs)) == 0)
      return (0);

   setCDKCalendarBox (calendar, Box);

   boxWidth = setCdkTitle(ObjOf(calendar), title, boxWidth);

   boxHeight += TitleLinesOf(calendar);

  /*
   * Make sure we didn't extend beyond the dimensions of the window.
   */
   boxWidth = (boxWidth > parentWidth ? parentWidth : boxWidth);
   boxHeight = (boxHeight > parentHeight ? parentHeight : boxHeight);

   /* Rejustify the x and y positions if we need to. */
   alignxy (cdkscreen->window, &xpos, &ypos, boxWidth, boxHeight);

   /* Create the calendar window. */
   calendar->win = newwin (boxHeight, boxWidth, ypos, xpos);

   /* Is the window null? */
   if (calendar->win == 0)
   {
      destroyCDKObject (calendar);
      return (0);
   }
   keypad (calendar->win, TRUE);

   /* Set some variables. */
   calendar->xOffset = (boxWidth - 20) / 2;
   calendar->fieldWidth = boxWidth - 2 * (1 + BorderOf(calendar));

   /* Set months and days names */
   for (x=0; x < MAX_MONTHS; x++)
   {
      calendar->MonthName[x] = copyChar(monthsOfTheYear[x]);
   }
   calendar->DayName = copyChar(dayname);

   /* Set the rest of the widget values. */
   ScreenOf(calendar)			= cdkscreen;
   calendar->parent			= cdkscreen->window;
   calendar->shadowWin			= 0;
   calendar->xpos			= xpos;
   calendar->ypos			= ypos;
   calendar->boxWidth			= boxWidth;
   calendar->boxHeight			= boxHeight;
   calendar->day			= day;
   calendar->month			= month;
   calendar->year			= year;
   calendar->dayAttrib			= dayAttrib;
   calendar->monthAttrib		= monthAttrib;
   calendar->yearAttrib			= yearAttrib;
   calendar->highlight			= highlight;
   calendar->width			= boxWidth;
   initExitType(calendar);
   ObjOf(calendar)->acceptsFocus	= TRUE;
   ObjOf(calendar)->inputWindow		= calendar->win;
   calendar->shadow			= shadow;

   calendar->labelWin = subwin (calendar->win,
				1, calendar->fieldWidth,
				ypos + TitleLinesOf(calendar) + 1,
				xpos + 1 + BorderOf(calendar));
   if (calendar->labelWin == 0)
   {
      destroyCDKObject (calendar);
      return (0);
   }

   calendar->fieldWin = subwin (calendar->win,
				7, 20,
				ypos + TitleLinesOf(calendar) + 3,
				xpos + calendar->xOffset);
   if (calendar->fieldWin == 0)
   {
      destroyCDKObject (calendar);
      return (0);
   }

   setCDKCalendarBox (calendar, Box);

   calendar->marker = typeCallocN(chtype, CALENDAR_LIMIT);
   if (calendar->marker == 0)
   {
      destroyCDKObject (calendar);
      return (0);
   }

   /* If the day/month/year values were 0, then use today's date. */
   if ((calendar->day == 0) && (calendar->month == 0) && (calendar->year == 0))
   {
      time (&clck);
      dateInfo = localtime (&clck);

      calendar->day	   = dateInfo->tm_mday;
      calendar->month	   = dateInfo->tm_mon + 1;
      calendar->year	   = dateInfo->tm_year + 1900;
   }

   /* Verify the dates provided. */
   verifyCalendarDate (calendar);

   /* Determine which day the month starts on. */
   calendar->weekDay = getMonthStartWeekday (calendar->year, calendar->month);

   /* If a shadow was requested, then create the shadow window. */
   if (shadow)
   {
      calendar->shadowWin = newwin (boxHeight, boxWidth, ypos + 1, xpos + 1);
   }

   /* Setup the key bindings. */
   for (x = 0; x < (int) SIZEOF(bindings); ++x)
      bindCDKObject (vCALENDAR, calendar, bindings[x].from, getcCDKBind, (void *)(long)bindings[x].to);

   registerCDKObject (cdkscreen, vCALENDAR, calendar);

   return (calendar);
}

/*
 * This function lets the user play with this widget.
 */
time_t activateCDKCalendar (CDKCALENDAR *calendar, chtype *actions)
{
   chtype input = 0;
   boolean functionKey;
   time_t ret	= -1;

   /* Draw the widget. */
   drawCDKCalendar (calendar, ObjOf(calendar)->box);

   if (actions == 0)
   {
      for (;;)
      {
	 input = getchCDKObject (ObjOf(calendar), &functionKey);

	 /* Inject the character into the widget. */
	 ret = injectCDKCalendar (calendar, input);
	 if (calendar->exitType != vEARLY_EXIT)
	 {
	    return ret;
	 }
      }
   }
   else
   {
      int length = chlen (actions);
      int x =0;

      /* Inject each character one at a time. */
      for (x=0; x < length; x++)
      {
	 ret = injectCDKCalendar (calendar, actions[x]);
	 if (calendar->exitType != vEARLY_EXIT)
	 {
	    return ret;
	 }
      }
   }
   return ret;
}

/*
 * This injects a single character into the widget.
 */
static int _injectCDKCalendar (CDKOBJS *object, chtype input)
{
   CDKCALENDAR *calendar = (CDKCALENDAR *)object;
   /* Declare local variables. */
   int ppReturn = 1;
   int ret = unknownInt;
   bool complete = FALSE;

   /* Set the exit type. */
   setExitType(calendar, 0);

   /* Refresh the calendar field. */
   drawCDKCalendarField (calendar);

   /* Check if there is a pre-process function to be called. */
   if (PreProcessFuncOf(calendar) != 0)
   {
      /* Call the pre-process function. */
      ppReturn = PreProcessFuncOf(calendar) (vCALENDAR, calendar, PreProcessDataOf(calendar), input);
   }

   /* Should we continue? */
   if (ppReturn != 0)
   {
      /* Check a predefined binding. */
      if (checkCDKObjectBind (vCALENDAR, calendar, input) != 0)
      {
	 checkEarlyExit(calendar);
	 complete = TRUE;
      }
      else
      {
	 switch (input)
	 {
	    case KEY_UP :
		 decrementCalendarDay (calendar, 7);
		 break;

	    case KEY_DOWN :
		 incrementCalendarDay (calendar, 7);
		 break;

	    case KEY_LEFT :
		 decrementCalendarDay (calendar, 1);
		 break;

	    case KEY_RIGHT :
		 incrementCalendarDay (calendar, 1);
		 break;

	    case KEY_NPAGE :
		 incrementCalendarMonth (calendar, 1);
		 break;

	    case 'N' :
		 incrementCalendarMonth (calendar, 6);
		 break;

	    case KEY_PPAGE :
		 decrementCalendarMonth (calendar, 1);
		 break;

	    case 'P' :
		 decrementCalendarMonth (calendar, 6);
		 break;

	    case '-' :
		 decrementCalendarYear (calendar, 1);
		 break;

	    case '+' :
		 incrementCalendarYear (calendar, 1);
		 break;

	    case KEY_HOME:
		 setCDKCalendarDate (calendar, -1, -1, -1);
		 break;

	    case KEY_ESC :
		 setExitType(calendar, input);
		 complete = TRUE;
		 break;

	    case KEY_TAB : case KEY_ENTER :
		 setExitType(calendar, input);
		 ret = getCurrentTime (calendar);
		 complete = TRUE;
		 break;

	    case CDK_REFRESH :
		 eraseCDKScreen (ScreenOf(calendar));
		 refreshCDKScreen (ScreenOf(calendar));
		 break;
	 }
      }

      /* Should we do a post-process? */
      if (!complete && (PostProcessFuncOf(calendar) != 0))
      {
	 PostProcessFuncOf(calendar) (vCALENDAR, calendar, PostProcessDataOf(calendar), input);
      }
   }

   if (!complete) {
      setExitType(calendar, 0);
   }

   ResultOf(calendar).valueInt = ret;
   return (ret != unknownInt);
}

/*
 * This moves the calendar field to the given location.
 */
static void _moveCDKCalendar (CDKOBJS *object, int xplace, int yplace, boolean relative, boolean refresh_flag)
{
   CDKCALENDAR *calendar = (CDKCALENDAR *)object;
   /* Declare local variables. */
   int currentX = getbegx(calendar->win);
   int currentY = getbegy(calendar->win);
   int xpos	= xplace;
   int ypos	= yplace;
   int xdiff	= 0;
   int ydiff	= 0;

   /*
    * If this is a relative move, then we will adjust where we want
    * to move to.
    */
   if (relative)
   {
      xpos = getbegx(calendar->win) + xplace;
      ypos = getbegy(calendar->win) + yplace;
   }

   /* Adjust the window if we need to. */
   alignxy (WindowOf(calendar), &xpos, &ypos, calendar->boxWidth, calendar->boxHeight);

   /* Get the difference. */
   xdiff = currentX - xpos;
   ydiff = currentY - ypos;

   /* Move the window to the new location. */
   moveCursesWindow(calendar->win, -xdiff, -ydiff);
   moveCursesWindow(calendar->fieldWin, -xdiff, -ydiff);
   moveCursesWindow(calendar->labelWin, -xdiff, -ydiff);
   moveCursesWindow(calendar->shadowWin, -xdiff, -ydiff);

   /* Touch the windows so they 'move'. */
   refreshCDKWindow (WindowOf(calendar));

   /* Redraw the window, if they asked for it. */
   if (refresh_flag)
   {
      drawCDKCalendar (calendar, ObjOf(calendar)->box);
   }
}

/*
 * This draws the calendar widget.
 */
static void _drawCDKCalendar (CDKOBJS *object, boolean Box)
{
   CDKCALENDAR *calendar = (CDKCALENDAR *)object;
   int headerLen	= (int)strlen (calendar->DayName);

   /* Is there a shadow? */
   if (calendar->shadowWin != 0)
   {
      drawShadow (calendar->shadowWin);
   }

   /* Box the widget if asked. */
   if (Box)
   {
      drawObjBox (calendar->win, ObjOf(calendar));
   }

   drawCdkTitle (calendar->win, object);

   /* Draw in the day-of-the-week header. */
   writeChar (calendar->win,
	      calendar->xOffset, TitleLinesOf(calendar) + 2,
	      calendar->DayName, HORIZONTAL, 0, headerLen);

   wrefresh (calendar->win);

   drawCDKCalendarField (calendar);
}

/*
 * This draws the month field.
 */
static void drawCDKCalendarField (CDKCALENDAR *calendar)
{
   /* Declare local variables. */
   char *monthName	= calendar->MonthName[calendar->month];
   int monthLength	= getMonthLength (calendar->year, calendar->month);
   int yearIndex	= YEAR2INDEX(calendar->year);
   int yearLen		= 0;
   int day		= 1;
   int x, y;
   int save_y = -1, save_x = -1;
   char temp[10];

   for (x = 1; x <= 6; x++)
   {
      for (y = (x == 1) ? calendar->weekDay : 0; y < 7; y++)
      {
	 if (day <= monthLength)
	 {
	    int ypos = y * 3;
	    int xpos = x;

	    chtype marker = calendar->dayAttrib;

	    sprintf(temp, "%02d", day);

	    if (calendar->day == day)
	    {
	       marker = calendar->highlight;
	       save_y = xpos + getbegy(calendar->fieldWin) - getbegy(InputWindowOf(calendar));
	       save_x = 1;
	    }
	    else
	    {
	       marker |= getCDKCalendarMarker(calendar, day, calendar->month, yearIndex);
	    }
	    writeCharAttrib (calendar->fieldWin, ypos, xpos,
			     temp, marker, HORIZONTAL, 0, 2);
	 }
	 day++;
      }
   }
   wrefresh (calendar->fieldWin);

   /* Draw the month in. */
   if (calendar->labelWin != 0)
   {
      sprintf (temp, "%s %d,", monthName, calendar->day);
      writeChar (calendar->labelWin, 0, 0,
		 temp, HORIZONTAL, 0, strlen(temp));
      wclrtoeol (calendar->labelWin);

      /* Draw the year in. */
      sprintf (temp, "%d", calendar->year);
      yearLen = (int)strlen (temp);
      writeChar (calendar->labelWin,
		 calendar->fieldWidth - yearLen, 0,
		 temp, HORIZONTAL, 0, yearLen);

      wmove (calendar->labelWin, 0, 0);
      wrefresh (calendar->labelWin);
   } else if (save_y >= 0) {
      wmove (InputWindowOf(calendar), save_y, save_x);
      wrefresh (InputWindowOf(calendar));
   }
}

/*
 * This sets multiple attributes of the widget.
 */
void setCDKCalendar (CDKCALENDAR *calendar, int day, int month, int year, chtype dayAttrib, chtype monthAttrib, chtype yearAttrib, chtype highlight, boolean Box)
{
   setCDKCalendarDate (calendar, day, month, year);
   setCDKCalendarDayAttribute (calendar, dayAttrib);
   setCDKCalendarMonthAttribute (calendar, monthAttrib);
   setCDKCalendarYearAttribute (calendar, yearAttrib);
   setCDKCalendarHighlight (calendar, highlight);
   setCDKCalendarBox (calendar, Box);
}

/*
 * This sets the date and some attributes.
 */
void setCDKCalendarDate (CDKCALENDAR *calendar, int day, int month, int year)
{
   /* Declare local variables. */
   struct tm *dateInfo;
   time_t clck;

   /*
    * Get the current dates and set the default values for
    * the day/month/year values for the calendar.
    */
   time (&clck);
   dateInfo = localtime (&clck);

   /* Set the date elements if we need too. */
   calendar->day	= (day == -1 ? dateInfo->tm_mday : day);
   calendar->month	= (month == -1 ? dateInfo->tm_mon + 1 : month);
   calendar->year	= (year == -1 ? dateInfo->tm_year + 1900 : year);

   /* Verify the date information. */
   verifyCalendarDate (calendar);

   /* Get the start of the current month. */
   calendar->weekDay = getMonthStartWeekday (calendar->year, calendar->month);
}

/*
 * This returns the current date on the calendar.
 */
void getCDKCalendarDate (CDKCALENDAR *calendar, int *day, int *month, int *year)
{
   (*day)	= calendar->day;
   (*month)	= calendar->month;
   (*year)	= calendar->year;
}

/*
 * This sets the attribute of the days in the calendar.
 */
void setCDKCalendarDayAttribute (CDKCALENDAR *calendar, chtype attribute)
{
   calendar->dayAttrib = attribute;
}
chtype getCDKCalendarDayAttribute (CDKCALENDAR *calendar)
{
   return calendar->dayAttrib;
}

/*
 * This sets the attribute of the month names in the calendar.
 */
void setCDKCalendarMonthAttribute (CDKCALENDAR *calendar, chtype attribute)
{
   calendar->monthAttrib = attribute;
}
chtype getCDKCalendarMonthAttribute (CDKCALENDAR *calendar)
{
   return calendar->monthAttrib;
}

/*
 * This sets the attribute of the year in the calendar.
 */
void setCDKCalendarYearAttribute (CDKCALENDAR *calendar, chtype attribute)
{
   calendar->yearAttrib = attribute;
}
chtype getCDKCalendarYearAttribute (CDKCALENDAR *calendar)
{
   return calendar->yearAttrib;
}

/*
 * This sets the attribute of the highlight box.
 */
void setCDKCalendarHighlight (CDKCALENDAR *calendar, chtype highlight)
{
   calendar->highlight = highlight;
}
chtype getCDKCalendarHighlight (CDKCALENDAR *calendar)
{
   return calendar->highlight;
}

/*
  * This sets the box attibute of the widget.
 */
void setCDKCalendarBox (CDKCALENDAR *calendar, boolean Box)
{
   ObjOf(calendar)->box = Box;
   ObjOf(calendar)->borderSize = Box ? 1 : 0;
}
boolean getCDKCalendarBox (CDKCALENDAR *calendar)
{
   return ObjOf(calendar)->box;
}

/*
 * This sets the background attribute of the widget.
 */
static void _setBKattrCalendar (CDKOBJS *object, chtype attrib)
{
   if (object != 0)
   {
      CDKCALENDAR *widget = (CDKCALENDAR *) object;

      wbkgd (widget->win, attrib);
      wbkgd (widget->fieldWin, attrib);
      if (widget->labelWin != 0)
      {
	 wbkgd (widget->labelWin, attrib);
      }
   }
}

/*
 * This erases the calendar widget.
 */
static void _eraseCDKCalendar (CDKOBJS *object)
{
   if (validCDKObject (object))
   {
      CDKCALENDAR *calendar = (CDKCALENDAR *)object;

      eraseCursesWindow (calendar->labelWin);
      eraseCursesWindow (calendar->fieldWin);
      eraseCursesWindow (calendar->win);
      eraseCursesWindow (calendar->shadowWin);
   }
}

/*
 * This destroys the calendar object pointer.
 */
static void _destroyCDKCalendar (CDKOBJS *object)
{
   if (object != 0)
   {
      CDKCALENDAR *calendar = (CDKCALENDAR *)object;
      int x;

      cleanCdkTitle (object);

      freeChar(calendar->DayName);

      for (x=0; x < MAX_MONTHS; x++)
      {
	 freeChar(calendar->MonthName[x]);
      }

      freeChecked(calendar->marker);

      /* Free up the window pointers. */
      deleteCursesWindow (calendar->labelWin);
      deleteCursesWindow (calendar->fieldWin);
      deleteCursesWindow (calendar->shadowWin);
      deleteCursesWindow (calendar->win);

      /* Clean the key bindings. */
      cleanCDKObjectBindings (vCALENDAR, calendar);

      /* Unregister the object. */
      unregisterCDKObject (vCALENDAR, calendar);
   }
}

/*
 * This sets a marker on the calendar.
 */
void setCDKCalendarMarker (CDKCALENDAR *calendar, int day, int month, int year, chtype marker)
{
   int yearIndex = YEAR2INDEX(year);
   chtype oldmarker = getCDKCalendarMarker(calendar, day, month, year);

   /* Check to see if a marker has not already been set. */
   if (oldmarker != 0)
   {
      CALENDAR_CELL(calendar,day,month,yearIndex) = oldmarker | A_BLINK;
   }
   else
   {
      CALENDAR_CELL(calendar,day,month,yearIndex) = marker;
   }
}
chtype getCDKCalendarMarker (CDKCALENDAR *calendar, int day, int month, int year)
{
   int result = 0;

   year = YEAR2INDEX(year);
   if (calendar->marker != 0)
      result = CALENDAR_CELL(calendar, day, month, year);
   return result;
}

/*
 * This sets a marker on the calendar.
 */
void removeCDKCalendarMarker (CDKCALENDAR *calendar, int day, int month, int year)
{
   int yearIndex = YEAR2INDEX(year);
   CALENDAR_CELL(calendar,day,month,yearIndex) = 0;
}

/*
 * This function sets the month name.
 */
void setCDKCalendarMonthsNames (CDKCALENDAR *calendar, char **months)
{
   int x;

   for (x=1; x < MAX_MONTHS; x++)
   {
      freeChar(calendar->MonthName[x]);
      calendar->MonthName[x] = copyChar(months[x]);
   }
}

/*
 * This function sets the days name.
 */
void setCDKCalendarDaysNames (CDKCALENDAR *calendar, char *days)
{
   freeChar(calendar->DayName);
   calendar->DayName = copyChar(days);
}

/*
 * This makes sure that the dates provided exist.
 */
static void verifyCalendarDate (CDKCALENDAR *calendar)
{
   int monthLength;

   /* Make sure the given year is not less than 1900. */
   if (calendar->year < 1900)
   {
      calendar->year = 1900;
   }

   /* Make sure the month is within range. */
   if (calendar->month > 12)
   {
      calendar->month = 12;
   }
   if (calendar->month < 1)
   {
      calendar->month = 1;
   }

   /* Make sure the day given is within range of the month. */
   monthLength = getMonthLength (calendar->year, calendar->month);
   if (calendar->day < 1)
   {
      calendar->day = 1;
   }
   if (calendar->day > monthLength)
   {
      calendar->day = monthLength;
   }
}

/*
 * This returns what day of the week the month starts on.
 */
static int getMonthStartWeekday (int year, int month)
{
   struct tm Date;

   /* Set the tm structure correctly. */
   Date.tm_sec		= 0;
   Date.tm_min		= 0;
   Date.tm_hour		= 10;
   Date.tm_mday		= 1;
   Date.tm_mon		= month - 1;
   Date.tm_year		= YEAR2INDEX(year);
   Date.tm_isdst	= 1;

   /* Call the mktime function to fill in the holes. */
   if (mktime (&Date) == (time_t)-1)
   {
      return 0;
   }
   return Date.tm_wday;
}

/*
 * This function returns a 1 if it's a leap year and 0 if it's not.
 */
static int isLeapYear (int year)
{
   if (year % 4 == 0 ||
	year % 4 == 0)
   {
      return 1;
   }
   return 0;
}

/*
 * This increments the current day by the given value.
 */
static void incrementCalendarDay (CDKCALENDAR *calendar, int adjust)
{
   int monthLength = getMonthLength (calendar->year, calendar->month);

   /* Make sure we adjust the day correctly. */
   if (adjust + calendar->day > monthLength)
   {
      /* Have to increment the month by one. */
      calendar->day = calendar->day + adjust - monthLength;
      incrementCalendarMonth (calendar, 1);
   }
   else
   {
      calendar->day += adjust;
      drawCDKCalendarField (calendar);
   }
}

/*
 * This decrments the current day by the given value.
 */
static void decrementCalendarDay (CDKCALENDAR *calendar, int adjust)
{
   int monthLength;

   /* Make sure we adjust the day correctly. */
   if (calendar->day - adjust < 1)
   {
      /* Set the day according to the length of the month. */
      if (calendar->month == 1)
      {
	  /* Make sure we aren't going past the year limit. */
	  if (calendar->year == 1900)
	  {
	     char *mesg[] = {"<C></U>Error", "Can not go past the year 1900"};
	     Beep();
	     popupLabel (ScreenOf(calendar), mesg, 2);
	     return;
	  }
	  monthLength = getMonthLength (calendar->year - 1, 12);
      }
      else
      {
	  monthLength = getMonthLength (calendar->year, calendar->month - 1);
      }

      calendar->day = monthLength - (adjust - calendar->day);

      /* Have to decrement the month by one. */
      decrementCalendarMonth (calendar, 1);
   }
   else
   {
      calendar->day -= adjust;
      drawCDKCalendarField (calendar);
   }
}

/*
 * This increments the current month by the given value.
 */
static void incrementCalendarMonth (CDKCALENDAR *calendar, int adjust)
{
   int monthLength;

   /* Are we at the end of the year. */
   if (calendar->month + adjust > 12)
   {
      calendar->month = (calendar->month + adjust) - 12;
      calendar->year++;
   }
   else
   {
      calendar->month += adjust;
   }

   /* Get the length of the current month. */
   monthLength = getMonthLength (calendar->year, calendar->month);
   if (calendar->day > monthLength)
   {
      calendar->day = monthLength;
   }

   /* Get the start of the current month. */
   calendar->weekDay = getMonthStartWeekday (calendar->year, calendar->month);

   /* Redraw the calendar. */
   eraseCDKCalendar (calendar);
   drawCDKCalendar (calendar, ObjOf(calendar)->box);
}

/*
 * This decrements the current month by the given value.
 */
static void decrementCalendarMonth (CDKCALENDAR *calendar, int adjust)
{
   int monthLength;

   /* Are we at the end of the year. */
   if (calendar->month <= adjust)
   {
      if (calendar->year == 1900)
      {
	 char *mesg[] = {"<C></U>Error", "Can not go past the year 1900"};
	 Beep();
	 popupLabel (ScreenOf(calendar), mesg, 2);
	 return;
      }
      else
      {
	 calendar->month = 13 - adjust;
	 calendar->year--;
      }
   }
   else
   {
      calendar->month -= adjust;
   }

   /* Get the length of the current month. */
   monthLength = getMonthLength (calendar->year, calendar->month);
   if (calendar->day > monthLength)
   {
      calendar->day = monthLength;
   }

   /* Get the start of the current month. */
   calendar->weekDay = getMonthStartWeekday (calendar->year, calendar->month);

   /* Redraw the calendar. */
   eraseCDKCalendar (calendar);
   drawCDKCalendar (calendar, ObjOf(calendar)->box);
}

/*
 * This increments the current year by the given value.
 */
static void incrementCalendarYear (CDKCALENDAR *calendar, int adjust)
{
   /* Increment the year. */
   calendar->year += adjust;

   /* If we are in Feb make sure we don't trip into voidness. */
   if (calendar->month == 2)
   {
      int monthLength = getMonthLength (calendar->year, calendar->month);
      if (calendar->day > monthLength)
      {
	 calendar->day = monthLength;
      }
   }

   /* Get the start of the current month. */
   calendar->weekDay = getMonthStartWeekday (calendar->year, calendar->month);

   /* Redraw the calendar. */
   eraseCDKCalendar (calendar);
   drawCDKCalendar (calendar, ObjOf(calendar)->box);
}

/*
 * This decrements the current year by the given value.
 */
static void decrementCalendarYear (CDKCALENDAR *calendar, int adjust)
{
   /* Make sure we don't go out of bounds. */
   if (calendar->year - adjust < 1900)
   {
      char *mesg[] = {"<C></U>Error", "Can not go past the year 1900"};
      Beep();
      popupLabel (ScreenOf(calendar), mesg, 2);
      return;
   }

   /* Decrement the year. */
   calendar->year -= adjust;

   /* If we are in Feb make sure we don't trip into voidness. */
   if (calendar->month == 2)
   {
      int monthLength = getMonthLength (calendar->year, calendar->month);
      if (calendar->day > monthLength)
      {
	 calendar->day = monthLength;
      }
   }

   /* Get the start of the current month. */
   calendar->weekDay = getMonthStartWeekday (calendar->year, calendar->month);

   /* Redraw the calendar. */
   eraseCDKCalendar (calendar);
   drawCDKCalendar (calendar, ObjOf(calendar)->box);
}

/*
 * This returns the length of the current month.
 */
static int getMonthLength (int year, int month)
{
   int monthLength = daysOfTheMonth[month];

   if (month == 2)
   {
      monthLength += isLeapYear (year);
   }
   return monthLength;
}

/*
 * This returns what day of the week the month starts on.
 */
static time_t getCurrentTime (CDKCALENDAR *calendar)
{
   struct tm Date, *dateInfo;
   time_t clck;

   /* Determine the current time and determine if we are in DST. */
   time (&clck);
   dateInfo = localtime (&clck);

   /* Set the tm structure correctly. */
   Date.tm_sec		= 0;
   Date.tm_min		= 0;
   Date.tm_hour		= 0;
   Date.tm_mday		= calendar->day;
   Date.tm_mon		= calendar->month - 1;
   Date.tm_year		= YEAR2INDEX(calendar->year);
   Date.tm_isdst	= dateInfo->tm_isdst;

   /* Call the mktime function to fill in the holes. */
   return mktime (&Date);
}

static void _focusCDKCalendar(CDKOBJS *object)
{
   CDKCALENDAR *widget = (CDKCALENDAR *)object;

   drawCDKFScale (widget, ObjOf(widget)->box);
}

static void _unfocusCDKCalendar(CDKOBJS *object)
{
   CDKCALENDAR *widget = (CDKCALENDAR *)object;

   drawCDKFScale (widget, ObjOf(widget)->box);
}

dummyRefreshData(Calendar)

dummySaveData(Calendar)
