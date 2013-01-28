#include <cdk_int.h>

/*
 * $Author: tom $
 * $Date: 2006/05/05 00:27:44 $
 * $Revision: 1.19 $
 */

/*
 * Declare file local prototypes.
 */
static void drawCDKSliderField (CDKSLIDER *widget);
static int formattedSize (CDKSLIDER *widget, int value);

DeclareCDKObjects(SLIDER, Slider, setCdk, Int);

/*
 * This function creates a widget.
 */
CDKSLIDER *newCDKSlider (CDKSCREEN *cdkscreen,
			   int xplace,
			   int yplace,
			   char *title,
			   char *label,
			   chtype filler,
			   int fieldWidth,
			   int start,
			   int low,
			   int high,
			   int inc,
			   int fastInc,
			   boolean Box,
			   boolean shadow)
{
   CDKSLIDER *widget	= 0;
   int parentWidth	= getmaxx(cdkscreen->window);
   int parentHeight	= getmaxy(cdkscreen->window);
   int boxHeight;
   int boxWidth		= 0;
   int horizontalAdjust, oldWidth;
   int xpos		= xplace;
   int ypos		= yplace;
   int highValueLen;
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

   if ((widget = newCDKObject(CDKSLIDER, &my_funcs)) == 0)
      return (0);

   setCDKSliderBox (widget, Box);
   boxHeight		= (BorderOf(widget) * 2) + 1;

   /* Set some basic values of the widget's data field. */
   widget->label	= 0;
   widget->labelLen	= 0;
   widget->labelWin	= 0;
   highValueLen	= formattedSize (widget, high);

  /*
   * If the fieldWidth is a negative value, the fieldWidth will
   * be COLS-fieldWidth, otherwise, the fieldWidth will be the
   * given width.
   */
   fieldWidth = setWidgetDimension (parentWidth, fieldWidth, 0);

   /* Translate the label char *pointer to a chtype pointer. */
   if (label != 0)
   {
      widget->label	= char2Chtype (label, &widget->labelLen, &junk);
      boxWidth		= widget->labelLen + fieldWidth + highValueLen + 2 * BorderOf(widget);
   }
   else
   {
      boxWidth = fieldWidth + highValueLen + 2*BorderOf(widget);
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
   fieldWidth = (fieldWidth > (boxWidth - widget->labelLen - highValueLen - 1)
   		 ? (boxWidth - widget->labelLen - highValueLen - 1)
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
			      1, fieldWidth + highValueLen - 1,
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
   widget->fieldWidth		= fieldWidth-1;
   widget->filler		= filler;
   widget->low			= low;
   widget->high			= high;
   widget->current		= start;
   widget->inc			= inc;
   widget->fastinc		= fastInc;
   initExitType(widget);
   ObjOf(widget)->acceptsFocus	= TRUE;
   ObjOf(widget)->inputWindow	= widget->win;
   widget->shadow		= shadow;

   /* Set the start value. */
   if (start < low)
   {
      widget->current = low;
   }

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
      bindCDKObject (vSLIDER, widget, bindings[x].from, getcCDKBind, (void *)(long)bindings[x].to);

   registerCDKObject (cdkscreen, vSLIDER, widget);

   return (widget);
}

/*
 * This allows the person to use the widget's data field.
 */
int activateCDKSlider (CDKSLIDER *widget, chtype *actions)
{
   int ret;

   /* Draw the widget. */
   drawCDKSlider (widget, ObjOf(widget)->box);

   if (actions == 0)
   {
      chtype input = 0;
      boolean functionKey;

      for (;;)
      {
	 input = getchCDKObject (ObjOf(widget), &functionKey);

	 /* Inject the character into the widget. */
	 ret = injectCDKSlider (widget, input);
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
	 ret = injectCDKSlider (widget, actions[x]);
	 if (widget->exitType != vEARLY_EXIT)
	 {
	    return ret;
	 }
      }
   }

   /* Set the exit type and return. */
   setExitType(widget, 0);
   return unknownInt;
}

/*
 * Check if the value lies outside the low/high range.  If so, force it in.
 */
static void limitCurrentValue (CDKSLIDER *widget)
{
   if (widget->current < widget->low)
   {
      widget->current = widget->low;
      BEEP();
   }
   else if (widget->current > widget->high)
   {
      widget->current = widget->high;
      BEEP();
   }
}

/*
 * Move the cursor to the given edit-position.
 */
static int moveToEditPosition(CDKSLIDER *widget, int newPosition)
{
   return wmove(widget->fieldWin,
   		0,
		widget->fieldWidth +
		formattedSize(widget, widget->current) -
		newPosition);
}

/*
 * Check if the cursor is on a valid edit-position.  This must be one of
 * the non-blank cells in the field.
 */
static int validEditPosition(CDKSLIDER *widget, int newPosition)
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
static void setEditPosition(CDKSLIDER *widget, int newPosition)
{
   if (newPosition < 0)
   {
      BEEP();
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
      BEEP();
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
static bool performEdit(CDKSLIDER *widget, chtype input)
{
   bool result = FALSE;
   bool modify = TRUE;
   int base = widget->fieldWidth;
   int need = formattedSize(widget, widget->current);
   char *temp = (char *)malloc(need + 5);
   char *data = temp;
   char test;
   int col = need - widget->fieldEdit;
   int value;
#define SCANF_FMT "%d%c"

   if (temp != 0)
   {
      int adj = (col < 0) ? (-col) : 0;
      if (adj)
      {
	 memset(temp, ' ', adj);
	 temp += adj;
      }
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
	 setCDKSliderValue(widget, value);
	 result = TRUE;
      }
      free(data);
   }
   return result;
}

#define Decrement(value,by) if (value - by < value) value -= by
#define Increment(value,by) if (value + by > value) value += by

/*
 * This function injects a single character into the widget.
 */
static int _injectCDKSlider (CDKOBJS *object, chtype input)
{
   CDKSLIDER *widget = (CDKSLIDER *)object;
   int ppReturn = 1;
   int ret = unknownInt;
   bool complete = FALSE;

   /* Set the exit type. */
   setExitType(widget, 0);

   /* Draw the field. */
   drawCDKSliderField (widget);

   /* Check if there is a pre-process function to be called. */
   if (PreProcessFuncOf(widget) != 0)
   {
      /* Call the pre-process function. */
      ppReturn = PreProcessFuncOf(widget) (vSLIDER, widget, PreProcessDataOf(widget), input);
   }

   /* Should we continue? */
   if (ppReturn != 0)
   {
      /* Check for a key binding. */
      if (checkCDKObjectBind(vSLIDER, widget, input) != 0)
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
		       BEEP();
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
		       return _injectCDKSlider(object, KEY_DOWN);
		    case '+':
		       return _injectCDKSlider(object, KEY_UP);
		    case 'D':
		       return _injectCDKSlider(object, KEY_NPAGE);
		    case '0':
		       return _injectCDKSlider(object, KEY_HOME);
		    default:
		       BEEP();
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
	 PostProcessFuncOf(widget) (vSLIDER, widget, PostProcessDataOf(widget), input);
      }
   }

   if (!complete) {
      drawCDKSliderField (widget);
      setExitType(widget, 0);
   }

   ResultOf(widget).valueInt = ret;
   return (ret != unknownInt);
}

/*
 * This moves the widget's data field to the given location.
 */
static void _moveCDKSlider (CDKOBJS *object, int xplace, int yplace, boolean relative, boolean refresh_flag)
{
   CDKSLIDER *widget = (CDKSLIDER *)object;
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
      drawCDKSlider (widget, ObjOf(widget)->box);
   }
}

/*
 * This function draws the widget.
 */
static void _drawCDKSlider (CDKOBJS *object, boolean Box)
{
   CDKSLIDER *widget = (CDKSLIDER *)object;

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
   drawCDKSliderField (widget);
}

/*
 * This draws the widget.
 */
static void drawCDKSliderField (CDKSLIDER *widget)
{
   int fillerCharacters, x;
   char temp[256];
   float step = ((float)widget->fieldWidth
   		/ (float)(widget->high - widget->low));

   /* Determine how many filler characters need to be drawn. */
   fillerCharacters = (int)((widget->current - widget->low) * step);

   werase (widget->fieldWin);

   /* Add the character to the window. */
   for (x=0; x < fillerCharacters; x++)
   {
      mvwaddch (widget->fieldWin, 0, x, widget->filler);
   }

   /* Draw the value in the field. */
   sprintf (temp, "%d", widget->current);
   writeCharAttrib (widget->fieldWin,
		    widget->fieldWidth,
		    0,
		    temp,
		    A_NORMAL,
		    HORIZONTAL,
		    0,
		    (int)strlen(temp));

   moveToEditPosition(widget, widget->fieldEdit);
   wrefresh (widget->fieldWin);
}

/*
 * This sets the background attribute of the widget.
 */
static void _setBKattrSlider (CDKOBJS *object, chtype attrib)
{
   if (object != 0)
   {
      CDKSLIDER *widget = (CDKSLIDER *)object;

      /* Set the widgets background attribute. */
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
static void _destroyCDKSlider (CDKOBJS *object)
{
   if (object != 0)
   {
      CDKSLIDER *widget = (CDKSLIDER *)object;

      cleanCdkTitle (object);
      freeChtype (widget->label);

      /* Clean up the windows. */
      deleteCursesWindow (widget->fieldWin);
      deleteCursesWindow (widget->labelWin);
      deleteCursesWindow (widget->shadowWin);
      deleteCursesWindow (widget->win);

      /* Clean the key bindings. */
      cleanCDKObjectBindings (vSLIDER, widget);

      /* Unregister this object. */
      unregisterCDKObject (vSLIDER, widget);
   }
}

/*
 * This function erases the widget from the screen.
 */
static void _eraseCDKSlider (CDKOBJS *object)
{
   if (validCDKObject (object))
   {
      CDKSLIDER *widget = (CDKSLIDER *)object;

      eraseCursesWindow (widget->labelWin);
      eraseCursesWindow (widget->fieldWin);
      eraseCursesWindow (widget->win);
      eraseCursesWindow (widget->shadowWin);
   }
}

static int formattedSize (CDKSLIDER *widget, int value)
{
   char temp[256];
   (void) widget;
   sprintf (temp, "%d", value);
   return strlen (temp);
}

/*
 * This function sets the low/high/current values of the widget.
 */
void setCDKSlider (CDKSLIDER *widget, int low, int high, int value, boolean Box)
{
   setCDKSliderLowHigh (widget, low, high);
   setCDKSliderValue (widget, value);
   setCDKSliderBox (widget, Box);
}

/*
 * This sets the digits.
 */

/*
 * This sets the widget's value.
 */
void setCDKSliderValue (CDKSLIDER *widget, int value)
{
   widget->current = value;
   limitCurrentValue(widget);
}
int getCDKSliderValue (CDKSLIDER *widget)
{
   return widget->current;
}

/*
 * This function sets the low/high values of the widget.
 */
void setCDKSliderLowHigh (CDKSLIDER *widget, int low, int high)
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
int getCDKSliderLowValue (CDKSLIDER *widget)
{
   return widget->low;
}
int getCDKSliderHighValue (CDKSLIDER *widget)
{
   return widget->high;
}

/*
 * This sets the widget's box attribute.
 */
void setCDKSliderBox (CDKSLIDER *widget, boolean Box)
{
   ObjOf(widget)->box = Box;
   ObjOf(widget)->borderSize = Box ? 1 : 0;
}
boolean getCDKSliderBox (CDKSLIDER *widget)
{
   return ObjOf(widget)->box;
}

static void _focusCDKSlider(CDKOBJS *object)
{
   CDKSLIDER *widget = (CDKSLIDER *)object;

   drawCDKSlider (widget, ObjOf(widget)->box);
}

static void _unfocusCDKSlider(CDKOBJS *object)
{
   CDKSLIDER *widget = (CDKSLIDER *)object;

   drawCDKSlider (widget, ObjOf(widget)->box);
}

dummyRefreshData(Slider)

dummySaveData(Slider)
