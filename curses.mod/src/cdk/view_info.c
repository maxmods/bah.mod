#include <cdk_int.h>

/*
 * $Author: tom $
 * $Date: 2003/11/27 22:05:27 $
 * $Revision: 1.1 $
 */

/*
 * This allows the user to view information.
 */
int viewInfo (CDKSCREEN *screen, char *title, char **info, int count, char **buttons, int buttonCount, boolean interpret)
{
   CDKVIEWER *viewer	= 0;
   int selected		= -1;

   /* Create the file viewer to view the file selected.*/
   viewer = newCDKViewer (screen, CENTER, CENTER, -6, -16,
				buttons, buttonCount,
				A_REVERSE, TRUE, TRUE);

   /* Set up the viewer title, and the contents to the widget. */
   setCDKViewer (viewer, title, info, count, A_REVERSE, interpret, TRUE, TRUE);

   /* Activate the viewer widget. */
   selected = activateCDKViewer (viewer, 0);

   /* Make sure they exited normally. */
   if (viewer->exitType != vNORMAL)
   {
      destroyCDKViewer (viewer);
      return (-1);
   }

   /* Clean up and return the button index selected. */
   destroyCDKViewer (viewer);
   return selected;
}
