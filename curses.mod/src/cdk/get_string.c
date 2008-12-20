#include <cdk_int.h>

/*
 * $Author: tom $
 * $Date: 2003/11/27 22:13:28 $
 * $Revision: 1.1 $
 */

/*
 * This gets information from a user.
 */
char *getString (CDKSCREEN *screen, char *title, char *label, char *initValue)
{
   CDKENTRY *widget	= 0;
   char *value		= 0;

   /* Create the widget. */
   widget = newCDKEntry (screen, CENTER, CENTER, title, label,
				A_NORMAL, '.', vMIXED, 40, 0,
				5000, TRUE, FALSE);

   /* Set the default value. */
   setCDKEntryValue (widget, initValue);

   /* Get the string. */
   value = activateCDKEntry (widget, 0);

   /* Make sure they exited normally. */
   if (widget->exitType != vNORMAL)
   {
      destroyCDKEntry (widget);
      return 0;
   }

   /* Return a copy of the string typed in. */
   value = copyChar (getCDKEntryValue (widget));
   destroyCDKEntry (widget);
   return value;
}
