#include <cdk_int.h>

/*
 * Default method-functions for CDK objects.
 *
 * $Author: tom $
 * $Date: 2012/03/22 00:43:33 $
 * $Revision: 1.16 $
 */

/*
 * Set the object's upper-left-corner line-drawing character.
 */
void setCdkULchar (CDKOBJS *obj, chtype ch)
{
   obj->ULChar = ch;
}

/*
 * Set the object's upper-right-corner line-drawing character.
 */
void setCdkURchar (CDKOBJS *obj, chtype ch)
{
   obj->URChar = ch;
}

/*
 * Set the object's lower-left-corner line-drawing character.
 */
void setCdkLLchar (CDKOBJS *obj, chtype ch)
{
   obj->LLChar = ch;
}

/*
 * Set the object's upper-right-corner line-drawing character.
 */
void setCdkLRchar (CDKOBJS *obj, chtype ch)
{
   obj->LRChar = ch;
}

/*
 * Set the object's horizontal line-drawing character.
 */
void setCdkHZchar (CDKOBJS *obj, chtype ch)
{
   obj->HZChar = ch;
}

/*
 * Set the object's vertical line-drawing character.
 */
void setCdkVTchar (CDKOBJS *obj, chtype ch)
{
   obj->VTChar = ch;
}

/*
 * Set the object's box-attributes.
 */
void setCdkBXattr (CDKOBJS *obj, chtype ch)
{
   obj->BXAttr = ch;
}

/*
 * This sets the background color of the widget.
 */
void setCDKObjectBackgroundColor (CDKOBJS *obj, const char *color)
{
   chtype *holder = 0;
   int junk1, junk2;

   /* Make sure the color isn't null. */
   if (color == 0)
   {
      return;
   }

   /* Convert the value of the environment variable to a chtype. */
   holder = char2Chtype (color, &junk1, &junk2);

   /* Set the widget's background color. */
   SetBackAttrObj (obj, holder[0]);

   /* Clean up. */
   freeChtype (holder);
}

/*
 * Set the widget's title.
 */
int setCdkTitle (CDKOBJS *obj, const char *title, int boxWidth)
{
   if (obj != 0)
   {
      cleanCdkTitle (obj);

      if (title != 0)
      {
	 chtype *holder;
	 char **temp = 0;
	 int titleWidth;
	 int x;
	 int len;
	 int align;
	 int maxWidth = 0;

	 /* We need to split the title on \n. */
	 temp = CDKsplitString (title, '\n');
	 obj->titleLines = (int)CDKcountStrings ((CDK_CSTRING2) temp);

	 obj->title = typeCallocN (chtype *, obj->titleLines + 1);
	 obj->titlePos = typeCallocN (int, obj->titleLines + 1);
	 obj->titleLen = typeCallocN (int, obj->titleLines + 1);

	 if (boxWidth >= 0)
	 {
	    /* We need to determine the widest title line. */
	    for (x = 0; x < obj->titleLines; x++)
	    {
	       holder = char2Chtype (temp[x], &len, &align);
	       maxWidth = MAXIMUM (maxWidth, len);
	       freeChtype (holder);
	    }
	    boxWidth = MAXIMUM (boxWidth, maxWidth + 2 * obj->borderSize);
	 }
	 else
	 {
	    boxWidth = -(boxWidth - 1);
	 }

	 /* For each line in the title, convert from char * to chtype * */
	 titleWidth = boxWidth - (2 * obj->borderSize);
	 for (x = 0; x < obj->titleLines; x++)
	 {
	    obj->title[x] = char2Chtype (temp[x], &obj->titleLen[x],
					 &obj->titlePos[x]);
	    obj->titlePos[x] = justifyString (titleWidth, obj->titleLen[x],
					      obj->titlePos[x]);
	 }
	 CDKfreeStrings (temp);
      }
   }
   return boxWidth;
}

/*
 * Draw the widget's title.
 */
void drawCdkTitle (WINDOW *win, CDKOBJS *obj)
{
   if (obj != 0)
   {
      int x;

      for (x = 0; x < obj->titleLines; x++)
      {
	 writeChtype (win,
		      obj->titlePos[x] + obj->borderSize,
		      x + obj->borderSize,
		      obj->title[x],
		      HORIZONTAL, 0,
		      obj->titleLen[x]);
      }
   }
}

/*
 * Remove storage for the widget's title.
 */
void cleanCdkTitle (CDKOBJS *obj)
{
   if (obj != 0)
   {
      CDKfreeChtypes (obj->title);
      obj->title = 0;

      freeAndNull (obj->titlePos);
      freeAndNull (obj->titleLen);

      obj->titleLines = 0;
   }
}

/*
 * Set data for preprocessing.
 */
void setCDKObjectPreProcess (CDKOBJS *obj, PROCESSFN fn, void *data)
{
   obj->preProcessFunction = fn;
   obj->preProcessData = data;
}

/*
 * Set data for postprocessing.
 */
void setCDKObjectPostProcess (CDKOBJS *obj, PROCESSFN fn, void *data)
{
   obj->postProcessFunction = fn;
   obj->postProcessData = data;
}

/*
 * Set the object's exit-type based on the input.
 * The .exitType field should have been part of the CDKOBJS struct, but it
 * is used too pervasively in older applications to move (yet).
 */
void setCdkExitType (CDKOBJS *obj, EExitType *type, chtype ch)
{
   switch (ch)
   {
   case KEY_ERROR:
      *type = vERROR;
      break;
   case KEY_ESC:
      *type = vESCAPE_HIT;
      break;
   case KEY_TAB:
   case KEY_ENTER:
      *type = vNORMAL;
      break;
   case 0:
      *type = vEARLY_EXIT;
      break;
   }
   /* make the result available via ExitTypeOf(obj) */
   obj->exitType = *type;
}
