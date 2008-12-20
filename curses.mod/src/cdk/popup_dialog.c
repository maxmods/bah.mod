#include <cdk_int.h>

/*
 * $Author: tom $
 * $Date: 2003/11/27 22:16:09 $
 * $Revision: 1.1 $
 */

/*
 * This pops up a dialog box.
 */
int popupDialog (CDKSCREEN *screen, char **mesg, int mesgCount, char **buttons, int buttonCount)
{
   /* Declare local variables. */
   CDKDIALOG *popup	= 0;
   int choice;

   /* Create the dialog box. */
   popup = newCDKDialog (screen, CENTER, CENTER,
				mesg, mesgCount,
				buttons, buttonCount,
				A_REVERSE, TRUE,
				TRUE, FALSE);

   /* Activate the dialog box */
   drawCDKDialog (popup, TRUE);

   /* Get the choice. */
   choice = activateCDKDialog (popup, 0);

   /* Destroy the dialog box. */
   destroyCDKDialog (popup);

   /* Clean the screen. */
   eraseCDKScreen (screen);
   refreshCDKScreen (screen);

   /* Return the choice. */
   return choice;
}
