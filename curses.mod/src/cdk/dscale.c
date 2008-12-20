#include <cdk_int.h>

/*
 * $Author: tom $
 * $Date: 2006/05/05 00:27:44 $
 * $Revision: 1.19 $
 */

/*
 * Declare file local prototypes.
 */
static void drawCDKDScaleField (CDKDSCALE *widget);

DeclareCDKObjects(DSCALE, DScale, setCdk, Double);

/*
 * This function creates a widget.
 */
CDKDSCALE *newCDKDScale (CDKSCREEN *cdkscreen,
			   int xplace,
			   int yplace,
			   char *title,
			   char *label,
			   chtype fieldAttr,
			   int fieldWidth,
			   double start,
			   double low,
			   double high,
			   double inc,
			   double fastInc,
			   int digits,
			   boolean Box,
			   boolean shadow)
{
   CDKDSCALE *widget	= 0;
   int parentWidth	= getmaxx(cdkscreen->window);
   int parentHeight	= getmaxy(cdkscreen->window);
   int boxHeight;
   int boxWidth;
   int horizontalAdjust, oldWidth;
   int xpos		= xplace;
   int ypos		= yplace;
   int x, junk;

   static const struct { int from; int to; } bindings[] = {
		{ 'u',		KEY_UP },
		{ 'U',		KEY_PPAGE },
		{ CDK_BACKCHAR,	KEY_PPAGE },
		{ CDK_FORCHAR,	KEY_NPAGE },
		{ 'g',		KEY_HOME },
		{ '^',		KEY_HOME },
		{ 'G',		KEY_END },
		{ '$',		KEY_END },
   };

   if ((widget = newCDKObject(CDKDSCALE, &my_funcs)) == 0)
      return (0);

   setCDKDScaleBox (widget, Box);

   boxHeight		= (BorderOf(widget) * 2) + 1;
   boxWidth		= fieldWidth + 2*BorderOf(widget);

   /* Set some basic values of the widget's data field. */
   widget->label	= 0;
   widget->labelLen	= 0;
   widget->labelWin	= 0;

  /*
   * If the fieldWidth is a negative value, the fieldWidth will
   * be COLS-fieldWidth, otherwise, the fieldWidth will be the
   * given width.
   */
   fieldWidth = setWidgetDimension (parentWidth, fieldWidth, 0);
   boxWidth = fieldWidth + 2*BorderOf(widget);

   /* Translate the label char *pointer to a chtype pointer. */
   if (label != 0)
   {
      widget->label	= char2Chtype (label, &widget->labelLen, &junk);
      boxWidth		= widget->labelLen + fieldWidth + 2;
   }

   oldWidth = boxWidth;
   boxWidth = setCdkTitle(ObjOf(widget), title, boxWidth);
   horizontalAdjust = (boxWidth - oldWidth) / 2;

   boxHeight += TitleLinesOf(widget);

  /*
   * Make sure we didn't extend beyond the dimensions of the window.
   */
   boxWidth = (boxWidth > parentWidth ? parentWidth : boxWidth);
   boxHeight = (boxHeight > parentHeight ? parentHeight : boxHeight);
   fieldWidth = (fieldWidth > (boxWidth - widget->labelLen - 2*BorderOf(widget))
   		 ? (boxWidth - widget->labelLen - 2*BorderOf(widget))
		 : fieldWidth);

   /* Rejustify the x and y positions if we need to. */
   alignxy (cdkscreen->window, &xpos, &ypos, boxWidth, boxHeight);

   /* Make the widget's window. */
   widget->win = newwin (boxHeight, boxWidth, ypos, xpos);

   /* Is the main window null??? */
   if (widget->win == 0)
   {
      destroyCDKObject(widget);
      return (0);
   }

   /* Create the widget's label window. */
   if (widget->label != 0)
   {
      widget->labelWin = subwin (widget->win,
				 1, widget->labelLen,
				 ypos + TitleLinesOf(widget) + BorderOf(widget),
				 xpos + horizontalAdjust + BorderOf(widget));
      if (widget->labelWin == 0)
      {
	 destroyCDKObject(widget);
	 return (0);
      }
   }

   /* Create the widget's data field window. */
   widget->fieldWin = subwin (widget->win,
			      1, fieldWidth,
			      ypos + TitleLinesOf(widget) + BorderOf(widget),
			      xpos + widget->labelLen + horizontalAdjust + BorderOf(widget));
   if (widget->fieldWin == 0)
   {
      destroyCDKObject(widget);
      return (0);
   }
   keypad (widget->fieldWin, TRUE);
   keypad (widget->win, TRUE);

   /* Create the widget's data field. */
   ScreenOf(widget)		= cdkscreen;
   widget->parent		= cdkscreen->window;
   widget->shadowWin		= 0;
   widget->boxWidth		= boxWidth;
   widget->boxHeight		= boxHeight;
   widget->fieldWidth		= fieldWidth;
   widget->fieldAttr		= (chtype)fieldAttr;
   widget->current		= low;
   widget->low			= low;
   widget->high			= high;
   widget->current		= start;
   widget->inc			= inc;
   widget->fastinc		= fastInc;
   widget->digits		= digits;
   initExitType(widget);
   ObjOf(widget)->acceptsFocus	= TRUE;
   ObjOf(widget)->inputWindow	= widget->win;
   widget->shadow		= shadow;

   /* Do we want a shadow??? */
   if (shadow)
   {
      widget->shadowWin = newwin (boxHeight, boxWidth, ypos + 1, xpos + 1);
      if (widget->shadowWin == 0)
      {
	 destroyCDKObject(widget);
	 return (0);
      }
   }

   /* Setup the key bindings. */
   for (x = 0; x < (int) SIZEOF(bindings); ++x)
      bindCDKObject (vDSCALE, widget, bindings[x].from, getcCDKBind, (void *)(long)bindings[x].to);

   registerCDKObject (cdkscreen, vDSCALE, widget);

   return (widget);
}

/*
 * This allows the person to use the widget's data field.
 */
double activateCDKDScale (CDKDSCALE *widget, chtype *actions)
{
   double ret;

   /* Draw the widget. */
   drawCDKDScale (widget, ObjOf(widget)->box);

   if (actions == 0)
   {
      chtype input = 0;
      boolean functionKey;

      for (;;)
      {
	 input = getchCDKObject (ObjOf(widget), &functionKey);

	 /* Inject the character into the widget. */
	 ret = injectCDKDScale (widget, input);
	 if (widget->exitType != vEARLY_EXIT)
	 {
	    return ret;
	 }
      }
   }
   else
   {
      int length = chlen (actions);
      int x = 0;

      /* Inject each character one at a time. */
      for (x=0; x < length; x++)
      {
	 ret = injectCDKDScale (widget, actions[x]);
	 if (widget->exitType != vEARLY_EXIT)
	 {
	    return ret;
	 }
      }
   }

   /* Set the exit type and return. */
   setExitType(widget, 0);
   return unknownDouble;
}

/*
 * Check if the value lies outside the low/high range.  If so, force it in.
 */
static void limitCurrentValue (CDKDSCALE *widget)
{
   if (widget->current < widget->low)
   {
      widget->current = widget->low;
      Beep();
   }
   else if (widget->current > widget->high)
   {
      widget->current = widget->high;
      Beep();
   }
}

/*
 * Move the cursor to the given edit-position.
 */
static int moveToEditPosition(CDKDSCALE *widget, int newPosition)
{
   return wmove(widget->fieldWin, 0, widget->fieldWidth - newPosition - 1);
}

/*
 * Check if the cursor is on a valid edit-position.  This must be one of
 * the non-blank cells in the field.
 */
static int validEditPosition(CDKDSCALE *widget, int newPosition)
{
   chtype ch;
   if (newPosition <= 0 || newPosition >= widget->fieldWidth)
      return FALSE;
   if (moveToEditPosition(widget, newPosition) == ERR)
      return FALSE;
   ch = winch(widget->fieldWin);
   if (CharOf(ch) != ' ')
      return TRUE;
   if (newPosition > 1)
   {
      /* don't use recursion - only one level is wanted */
      if (moveToEditPosition(widget, newPosition - 1) == ERR)
	 return FALSE;
      ch = winch(widget->fieldWin);
      return CharOf(ch) != ' ';
   }
   return FALSE;
}

/*
 * Set the edit position.  Normally the cursor is one cell to the right of
 * the editable field.  Moving it left, over the field allows the user to
 * modify cells by typing in replacement characters for the field's value.
 */
static void setEditPosition(CDKDSCALE *widget, int newPosition)
{
   if (newPosition < 0)
   {
      Beep();
   }
   else if (newPosition == 0)
   {
      widget->fieldEdit = newPosition;
   }
   else if (validEditPosition(widget, newPosition))
   {
      widget->fieldEdit = newPosition;
   }
   else
   {
      Beep();
   }
}

/*
 * Remove the character from the string at the given column, if it is blank.
 * Returns true if a change was made.
 */
static bool removeChar(char *string, int col)
{
   bool result = FALSE;

   if ((col >= 0) && (string[col] != ' '))
   {
      while (string[col] != '\0')
      {
	 string[col] = string[col + 1];
	 ++col;
      }
      result = TRUE;
   }
   return result;
}

/*
 * Perform an editing function for the field.
 */
static bool performEdit(CDKDSCALE *widget, chtype input)
{
   bool result = FALSE;
   bool modify = TRUE;
   int base = 0;
   int need  = widget->fieldWidth;
   char *temp = (char *)malloc(need + 2);
   char test;
   int col = need - widget->fieldEdit - 1;
   double value;
#define SCANF_FMT "%lg%c"

   if (temp != 0)
   {
      wmove(widget->fieldWin, 0, base);
      winnstr(widget->fieldWin, temp, need);
      strcpy(temp + need, " ");
      if (isChar(input))	/* replace the char at the cursor */
      {
	 temp[col] = CharOf(input);
      }
      else if (input == KEY_BACKSPACE)	/* delete the char before the cursor */
      {
	 modify = removeChar(temp, col - 1);
      }
      else if (input == KEY_DC)	/* delete the char at the cursor */
      {
	 modify = removeChar(temp, col);
      }
      else
      {
	 modify = FALSE;
      }
      if (modify
       && sscanf(temp, SCANF_FMT, &value, &test) == 2
       && test == ' '
       && value >= widget->low
       && value <= widget->high)
      {
	 setCDKDScaleValue(widget, value);
	 result = TRUE;
      }
      free(temp);
   }
   return result;
}

#define Decrement(value,by) if (value - by < value) value -= by
#define Increment(value,by) if (value + by > value) value += by

/*
 * This function injects a single character into the widget.
 */
static int _injectCDKDScale (CDKOBJS *object, chtype input)
{
   CDKDSCALE *widget = (CDKDSCALE *)object;
   int ppReturn = 1;
   double ret = unknownDouble;
   bool complete = FALSE;

   /* Set the exit type. */
   setExitType(widget, 0);

   /* Draw the field. */
   drawCDKDScaleField (widget);

   /* Check if there is a pre-process function to be called. */
   if (PreProcessFuncOf(widget) != 0)
   {
      /* Call the pre-process function. */
      ppReturn = PreProcessFuncOf(widget) (vDSCALE, widget, PreProcessDataOf(widget), input);
   }

   /* Should we continue? */
   if (ppReturn != 0)
   {
      /* Check for a key binding. */
      if (checkCDKObjectBind(vDSCALE, widget, input) != 0)
      {
	 checkEarlyExit(widget);
	 complete = TRUE;
      }
      else
      {
	 switch (input)
	 {
	    case KEY_LEFT :
		 setEditPosition(widget, widget->fieldEdit + 1);
		 break;

	    case KEY_RIGHT :
		 setEditPosition(widget, widget->fieldEdit - 1);
		 break;

	    case KEY_DOWN :
		 Decrement(widget->current, widget->inc);
		 break;

	    case KEY_UP :
		 Increment(widget->current, widget->inc);
		 break;

	    case KEY_PPAGE :
		 Increment(widget->current, widget->fastinc);
		 break;

	    case KEY_NPAGE :
		 Decrement(widget->current, widget->fastinc);
		 break;

	    case KEY_HOME :
		 widget->current = widget->low;
		 break;

	    case KEY_END :
		 widget->current = widget->high;
		 break;

	    case KEY_TAB : case KEY_ENTER :
		 setExitType(widget, input);
		 ret = (widget->current);
		 complete = TRUE;
		 break;

	    case KEY_ESC :
		 setExitType(widget, input);
		 complete = TRUE;
		 break;

	    case CDK_REFRESH :
		 eraseCDKScreen (ScreenOf(widget));
		 refreshCDKScreen (ScreenOf(widget));
		 break;

	    default :
		 if (widget->fieldEdit)
		 {
		    if (!performEdit(widget, input))
		       Beep();
		 }
		 else
		 {
		    /*
		     * The cursor is not within the editable text.  Interpret
		     * input as commands.
		     */
		    switch (input)
		    {
		    case 'd':
		    case '-':
		       return _injectCDKDScale(object, KEY_DOWN);
		    case '+':
		       return _injectCDKDScale(object, KEY_UP);
		    case 'D':
		       return _injectCDKDScale(object, KEY_NPAGE);
		    case '0':
		       return _injectCDKDScale(object, KEY_HOME);
		    default:
		       Beep();
		       break;
		    }
		 }
		 break;
	 }
      }
      limitCurrentValue(widget);

      /* Should we call a post-process? */
      if (!complete && (PostProcessFuncOf(widget) != 0))
      {
	 PostProcessFuncOf(widget) (vDSCALE, widget, PostProcessDataOf(widget), input);
      }
   }

   if (!complete) {
      drawCDKDScaleField (widget);
      setExitType(widget, 0);
   }

   ResultOf(widget).valueDouble = ret;
   return (ret != unknownDouble);
}

/*
 * This moves the widget's data field to the given location.
 */
static void _moveCDKDScale (CDKOBJS *object, int xplace, int yplace, boolean relative, boolean refresh_flag)
{
   CDKDSCALE *widget = (CDKDSCALE *)object;
   int currentX = getbegx(widget->win);
   int currentY = getbegy(widget->win);
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
      xpos = getbegx(widget->win) + xplace;
      ypos = getbegy(widget->win) + yplace;
   }

   /* Adjust the window if we need to. */
   alignxy (WindowOf(widget), &xpos, &ypos, widget->boxWidth, widget->boxHeight);

   /* Get the difference. */
   xdiff = currentX - xpos;
   ydiff = currentY - ypos;

   /* Move the window to the new location. */
   moveCursesWindow(widget->win, -xdiff, -ydiff);
   moveCursesWindow(widget->labelWin, -xdiff, -ydiff);
   moveCursesWindow(widget->fieldWin, -xdiff, -ydiff);
   moveCursesWindow(widget->shadowWin, -xdiff, -ydiff);

   /* Touch the windows so they 'move'. */
   refreshCDKWindow (WindowOf(widget));

   /* Redraw the window, if they asked for it. */
   if (refresh_flag)
   {
      drawCDKDScale (widget, ObjOf(widget)->box);
   }
}

/*
 * This function draws the widget.
 */
static void _drawCDKDScale (CDKOBJS *object, boolean Box)
{
   CDKDSCALE *widget = (CDKDSCALE *)object;

   /* Draw the shadow. */
   if (widget->shadowWin != 0)
   {
      drawShadow (widget->shadowWin);
   }

   /* Box the widget if asked. */
   if (Box)
   {
      drawObjBox (widget->win, ObjOf(widget));
   }

   drawCdkTitle (widget->win, object);

   /* Draw the label. */
   if (widget->labelWin != 0)
   {
      writeChtype (widget->labelWin, 0, 0,
		   widget->label,
		   HORIZONTAL, 0,
		   widget->labelLen);
      wrefresh (widget->labelWin);
   }
   wrefresh (widget->win);

   /* Draw the field window. */
   drawCDKDScaleField (widget);
}

/*
 * This draws the widget.
 */
static void drawCDKDScaleField (CDKDSCALE *widget)
{
   char temp[256];

   werase (widget->fieldWin);

   /* Draw the value in the field. */
   {
      char format[256];
      int digits = MINIMUM(widget->digits, 30);
      sprintf (format, "%%.%if", digits);
      sprintf (temp, format, widget->current);
   }
   writeCharAttrib (widget->fieldWin,
		    widget->fieldWidth - (int)strlen(temp) - 1,
		    0,
		    temp,
		    widget->fieldAttr,
		    HORIZONTAL,
		    0,
		    (int)strlen(temp));

   moveToEditPosition(widget, widget->fieldEdit);
   wrefresh (widget->fieldWin);
}

/*
 * This sets the background attribute of the widget.
 */
static void _setBKattrDScale (CDKOBJS *object, chtype attrib)
{
   if (object != 0)
   {
      CDKDSCALE *widget = (CDKDSCALE *)object;

      wbkgd (widget->win, attrib);
      wbkgd (widget->fieldWin, attrib);
      if (widget->labelWin != 0)
      {
	 wbkgd (widget->labelWin, attrib);
      }
   }
}

/*
 * This function destroys the widget.
 */
static void _destroyCDKDScale (CDKOBJS *object)
{
   if (object != 0)
   {
      CDKDSCALE *widget = (CDKDSCALE *)object;

      cleanCdkTitle (object);
      freeChtype (widget->label);

      /* Clean up the windows. */
      deleteCursesWindow (widget->fieldWin);
      deleteCursesWindow (widget->labelWin);
      deleteCursesWindow (widget->shadowWin);
      deleteCursesWindow (widget->win);

      /* Clean the key bindings. */
      cleanCDKObjectBindings (vDSCALE, widget);

      /* Unregister this object. */
      unregisterCDKObject (vDSCALE, widget);
   }
}

/*
 * This function erases the widget from the screen.
 */
static void _eraseCDKDScale (CDKOBJS *object)
{
   if (validCDKObject (object))
   {
      CDKDSCALE *widget = (CDKDSCALE *)object;

      eraseCursesWindow (widget->labelWin);
      eraseCursesWindow (widget->fieldWin);
      eraseCursesWindow (widget->win);
      eraseCursesWindow (widget->shadowWin);
   }
}

/*
 * This function sets the low/high/current values of the widget.
 */
void setCDKDScale (CDKDSCALE *widget, double low, double high, double value, boolean Box)
{
   setCDKDScaleLowHigh (widget, low, high);
   setCDKDScaleValue (widget, value);
   setCDKDScaleBox (widget, Box);
}

/*
 * This sets the digits.
 */
void setCDKDScaleDigits (CDKDSCALE *widget, int digits)
{
    widget->digits = MAXIMUM (0, digits);
}

int getCDKDScaleDigits (CDKDSCALE *widget)
{
    return widget->digits;
}

/*
 * This sets the widget's value.
 */
void setCDKDScaleValue (CDKDSCALE *widget, double value)
{
   widget->current = value;
   limitCurrentValue(widget);
}
double getCDKDScaleValue (CDKDSCALE *widget)
{
   return widget->current;
}

/*
 * This function sets the low/high values of the widget.
 */
void setCDKDScaleLowHigh (CDKDSCALE *widget, double low, double high)
{
   /* Make sure the values aren't out of bounds. */
   if (low <= high)
   {
      widget->low	= low;
      widget->high	= high;
   }
   else if (low > high)
   {
      widget->low	= high;
      widget->high	= low;
   }

   /* Make sure the user hasn't done something silly. */
   limitCurrentValue(widget);
}
double getCDKDScaleLowValue (CDKDSCALE *widget)
{
   return widget->low;
}
double getCDKDScaleHighValue (CDKDSCALE *widget)
{
   return widget->high;
}

/*
 * This sets the widget's box attribute.
 */
void setCDKDScaleBox (CDKDSCALE *widget, boolean Box)
{
   ObjOf(widget)->box = Box;
   ObjOf(widget)->borderSize = Box ? 1 : 0;
}
boolean getCDKDScaleBox (CDKDSCALE *widget)
{
   return ObjOf(widget)->box;
}

static void _focusCDKDScale(CDKOBJS *object)
{
   CDKDSCALE *widget = (CDKDSCALE *)object;

   drawCDKDScale (widget, ObjOf(widget)->box);
}

static void _unfocusCDKDScale(CDKOBJS *object)
{
   CDKDSCALE *widget = (CDKDSCALE *)object;

   drawCDKDScale (widget, ObjOf(widget)->box);
}

dummyRefreshData(DScale)

dummySaveData(DScale)
