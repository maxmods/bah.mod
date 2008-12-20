#include <cdk_int.h>

/*
 * $Author: tom $
 * $Date: 2005/12/27 01:05:44 $
 * $Revision: 1.3 $
 */

/*
 * This allows the user to view a file.
 */
int viewFile (CDKSCREEN *screen, char *title, char *filename, char **buttons, int buttonCount)
{
   int result;
   int lines		= 0;
   char **info		= 0;

   /* Open the file and read the contents. */
   lines = CDKreadFile (filename, &info);

   /* If we couldn't read the file, return an error. */
   if (lines == -1)
   {
      result = lines;
   }
   else
   {

      result = viewInfo(screen, title, info, lines, buttons, buttonCount, TRUE);
      CDKfreeStrings (info);
   }
   return result;
}
