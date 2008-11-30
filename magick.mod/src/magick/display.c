/*
% Copyright (C) 2003, 2004 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
% Copyright 1991-1999 E. I. du Pont de Nemours and Company
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%             DDDD   IIIII  SSSSS  PPPP   L       AAA   Y   Y                 %
%             D   D    I    SS     P   P  L      A   A   Y Y                  %
%             D   D    I     SSS   PPPP   L      AAAAA    Y                   %
%             D   D    I       SS  P      L      A   A    Y                   %
%             DDDD   IIIII  SSSSS  P      LLLLL  A   A    Y                   %
%                                                                             %
%                                                                             %
%            Methods to Interactively Display and Edit an Image               %
%                                                                             %
%                                                                             %
%                           Software Design                                   %
%                             John Cristy                                     %
%                              July 1992                                      %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#if defined(HasX11)
#include "magick/attribute.h"
#include "magick/blob.h"
#include "magick/color.h"
#include "magick/composite.h"
#include "magick/constitute.h"
#include "magick/decorate.h"
#include "magick/delegate.h"
#include "magick/effect.h"
#include "magick/enhance.h"
#include "magick/fx.h"
#include "magick/log.h"
#include "magick/monitor.h"
#include "magick/montage.h"
#include "magick/paint.h"
#include "magick/pixel_cache.h"
#include "magick/quantize.h"
#include "magick/render.h"
#include "magick/resize.h"
#include "magick/shear.h"
#include "magick/tempfile.h"
#include "magick/transform.h"
#include "magick/utility.h"
#include "magick/version.h"
#include "magick/xwindow.h"
#include "magick/display.h"

/*
  Constant declaration.
*/
static const int
  RoiDelta = 8;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k X A n n o t a t e E d i t I m a g e                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickXAnnotateEditImage annotates the image with text.
%
%  The format of the MagickXAnnotateEditImage method is:
%
%      unsigned int MagickXAnnotateEditImage(Display *display,
%        MagickXResourceInfo *resource_info,MagickXWindows *windows,Image *image)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 MagickXResourceInfo structure.
%
%    o windows: Specifies a pointer to a MagickXWindows structure.
%
%    o image: Specifies a pointer to an Image structure; returned from
%      ReadImage.
%
*/
static unsigned int MagickXAnnotateEditImage(Display *display,
  MagickXResourceInfo *resource_info,MagickXWindows *windows,Image *image)
{
  static const char
    *AnnotateMenu[]=
    {
      "Font Name",
      "Font Color",
      "Box Color",
      "Rotate Text",
      "Help",
      "Dismiss",
      (char *) NULL
    },
    *TextMenu[]=
    {
      "Help",
      "Apply",
      (char *) NULL
    };

  static double
    degrees = 0.0;

  static const ModeType
    AnnotateCommands[]=
    {
      AnnotateNameCommand,
      AnnotateFontColorCommand,
      AnnotateBackgroundColorCommand,
      AnnotateRotateCommand,
      AnnotateHelpCommand,
      AnnotateDismissCommand
    },
    TextCommands[]=
    {
      TextHelpCommand,
      TextApplyCommand
    };

  static unsigned int
    box_id = MaxNumberPens-2,
    font_id = 0,
    pen_id = 0,
    transparent_box = True,
    transparent_pen = False;

  char
    *ColorMenu[MaxNumberPens+1],
    command[MaxTextExtent],
    text[MaxTextExtent];

  Cursor
    cursor;

  GC
    annotate_context;

  int
    id,
    pen_number,
    x,
    y;

  KeySym
    key_symbol;

  register char
    *p;

  register long
    i;

  unsigned int
    height,
    status,
    width;

  unsigned long
    state;

  MagickXAnnotateInfo
    *annotate_info,
    *previous_info;

  XColor
    color;

  XFontStruct
    *font_info;

  XEvent
    event,
    text_event;

  /*
    Map Command widget.
  */
  (void) CloneString(&windows->command.name,"Annotate");
  windows->command.data=4;
  (void) MagickXCommandWidget(display,windows,AnnotateMenu,(XEvent *) NULL);
  (void) XMapRaised(display,windows->command.id);
  MagickXClientMessage(display,windows->image.id,windows->im_protocols,
    windows->im_update_widget,CurrentTime);
  /*
    Track pointer until button 1 is pressed.
  */
  MagickXQueryPosition(display,windows->image.id,&x,&y);
  (void) XSelectInput(display,windows->image.id,
    windows->image.attributes.event_mask | PointerMotionMask);
  cursor=XCreateFontCursor(display,XC_left_side);
  (void) XDefineCursor(display,windows->image.id,cursor);
  state=DefaultState;
  do
  {
    if (windows->info.mapped)
      {
        /*
          Display pointer position.
        */
        FormatString(text," %+d%+d ",x+windows->image.x,y+windows->image.y);
        MagickXInfoWidget(display,windows,text);
      }
    /*
      Wait for next event.
    */
    MagickXScreenEvent(display,windows,&event);
    if (event.xany.window == windows->command.id)
      {
        /*
          Select a command from the Command widget.
        */
        id=MagickXCommandWidget(display,windows,AnnotateMenu,&event);
        (void) XDefineCursor(display,windows->image.id,cursor);
        if (id < 0)
          continue;
        switch (AnnotateCommands[id])
        {
          case AnnotateNameCommand:
          {
            char
              *FontMenu[MaxNumberFonts];

            int
              font_number;

            /*
              Initialize menu selections.
            */
            for (i=0; i < MaxNumberFonts; i++)
              FontMenu[i]=resource_info->font_name[i];
            FontMenu[MaxNumberFonts-2]=(char *) "Browser...";
            FontMenu[MaxNumberFonts-1]=(char *) NULL;
            /*
              Select a font name from the pop-up menu.
            */
            font_number=MagickXMenuWidget(display,windows,AnnotateMenu[id],
              (const char **) FontMenu,command);
            if (font_number < 0)
              break;
            if (font_number == (MaxNumberFonts-2))
              {
                static char
                  font_name[MaxTextExtent] = "fixed";

                /*
                  Select a font name from a browser.
                */
                resource_info->font_name[font_number]=font_name;
                MagickXFontBrowserWidget(display,windows,"Select",font_name);
                if (*font_name == '\0')
                  break;
              }
            /*
              Initialize font info.
            */
            font_info=
              XLoadQueryFont(display,resource_info->font_name[font_number]);
            if (font_info == (XFontStruct *) NULL)
              {
                MagickXNoticeWidget(display,windows,"Unable to load font:",
                  resource_info->font_name[font_number]);
                break;
              }
            font_id=font_number;
            (void) XFreeFont(display,font_info);
            break;
          }
          case AnnotateFontColorCommand:
          {
            /*
              Initialize menu selections.
            */
            for (i=0; i < (int) (MaxNumberPens-2); i++)
              ColorMenu[i]=resource_info->pen_colors[i];
            ColorMenu[MaxNumberPens-2]=(char *) "transparent";
            ColorMenu[MaxNumberPens-1]=(char *) "Browser...";
            ColorMenu[MaxNumberPens]=(char *) NULL;
            /*
              Select a pen color from the pop-up menu.
            */
            pen_number=MagickXMenuWidget(display,windows,AnnotateMenu[id],
              (const char **) ColorMenu,command);
            if (pen_number < 0)
              break;
            transparent_pen=pen_number == (MaxNumberPens-2);
            if (transparent_pen)
              break;
            if (pen_number == (MaxNumberPens-1))
              {
                static char
                  color_name[MaxTextExtent] = "gray";

                /*
                  Select a pen color from a dialog.
                */
                resource_info->pen_colors[pen_number]=color_name;
                MagickXColorBrowserWidget(display,windows,"Select",color_name);
                if (*color_name == '\0')
                  break;
              }
            /*
              Set pen color.
            */
            (void) XParseColor(display,windows->map_info->colormap,
              resource_info->pen_colors[pen_number],&color);
            MagickXBestPixel(display,windows->map_info->colormap,(XColor *) NULL,
              (unsigned int) MaxColors,&color);
            windows->pixel_info->pen_colors[pen_number]=color;
            pen_id=pen_number;
            break;
          }
          case AnnotateBackgroundColorCommand:
          {
            /*
              Initialize menu selections.
            */
            for (i=0; i < (int) (MaxNumberPens-2); i++)
              ColorMenu[i]=resource_info->pen_colors[i];
            ColorMenu[MaxNumberPens-2]=(char *) "transparent";
            ColorMenu[MaxNumberPens-1]=(char *) "Browser...";
            ColorMenu[MaxNumberPens]=(char *) NULL;
            /*
              Select a pen color from the pop-up menu.
            */
            pen_number=MagickXMenuWidget(display,windows,AnnotateMenu[id],
              (const char **) ColorMenu,command);
            if (pen_number < 0)
              break;
            transparent_box=pen_number == (MaxNumberPens-2);
            if (transparent_box)
              break;
            if (pen_number == (MaxNumberPens-1))
              {
                static char
                  color_name[MaxTextExtent] = "gray";

                /*
                  Select a pen color from a dialog.
                */
                resource_info->pen_colors[pen_number]=color_name;
                MagickXColorBrowserWidget(display,windows,"Select",color_name);
                if (*color_name == '\0')
                  break;
              }
            /*
              Set pen color.
            */
            (void) XParseColor(display,windows->map_info->colormap,
              resource_info->pen_colors[pen_number],&color);
            MagickXBestPixel(display,windows->map_info->colormap,(XColor *) NULL,
              (unsigned int) MaxColors,&color);
            windows->pixel_info->pen_colors[pen_number]=color;
            box_id=pen_number;
            break;
          }
          case AnnotateRotateCommand:
          {
            int
              entry;

            static char
              angle[MaxTextExtent] = "30.0";

            static const char
              *RotateMenu[]=
              {
                "-90",
                "-45",
                "-30",
                "0",
                "30",
                "45",
                "90",
                "180",
                "Dialog...",
                (char *) NULL,
              };

            /*
              Select a command from the pop-up menu.
            */
            entry=MagickXMenuWidget(display,windows,AnnotateMenu[id],RotateMenu,
              command);
            if (entry < 0)
              break;
            if (entry != 8)
              {
                degrees=atof(RotateMenu[entry]);
                break;
              }
            (void) MagickXDialogWidget(display,windows,"OK","Enter rotation angle:",
              angle);
            if (*angle == '\0')
              break;
            degrees=atof(angle);
            break;
          }
          case AnnotateHelpCommand:
          {
            MagickXTextViewWidget(display,resource_info,windows,False,
              "Help Viewer - Image Annotation",ImageAnnotateHelp);
            break;
          }
          case AnnotateDismissCommand:
          {
            /*
              Prematurely exit.
            */
            state|=EscapeState;
            state|=ExitState;
            break;
          }
          default:
            break;
        }
        continue;
      }
    switch (event.type)
    {
      case ButtonPress:
      {
        if (event.xbutton.button != Button1)
          break;
        if (event.xbutton.window != windows->image.id)
          break;
        /*
          Change to text entering mode.
        */
        x=event.xbutton.x;
        y=event.xbutton.y;
        state|=ExitState;
        break;
      }
      case ButtonRelease:
        break;
      case Expose:
        break;
      case KeyPress:
      {
        if (event.xkey.window != windows->image.id)
          break;
        /*
          Respond to a user key press.
        */
        (void) XLookupString((XKeyEvent *) &event.xkey,command,sizeof(command),
          &key_symbol,(XComposeStatus *) NULL);
        switch ((int) key_symbol)
        {
          case XK_Escape:
          case XK_F20:
          {
            /*
              Prematurely exit.
            */
            state|=EscapeState;
            state|=ExitState;
            break;
          }
          case XK_F1:
          case XK_Help:
          {
            MagickXTextViewWidget(display,resource_info,windows,False,
              "Help Viewer - Image Annotation",ImageAnnotateHelp);
            break;
          }
          default:
          {
            (void) XBell(display,0);
            break;
          }
        }
        break;
      }
      case MotionNotify:
      {
        /*
          Map and unmap Info widget as cursor crosses its boundaries.
        */
        x=event.xmotion.x;
        y=event.xmotion.y;
        if (windows->info.mapped)
          {
            if ((x < (int) (windows->info.x+windows->info.width)) &&
                (y < (int) (windows->info.y+windows->info.height)))
              (void) XWithdrawWindow(display,windows->info.id,
                windows->info.screen);
          }
        else
          if ((x > (int) (windows->info.x+windows->info.width)) ||
              (y > (int) (windows->info.y+windows->info.height)))
            (void) XMapWindow(display,windows->info.id);
        break;
      }
      default:
        break;
    }
  } while (!(state & ExitState));
  (void) XSelectInput(display,windows->image.id,
    windows->image.attributes.event_mask);
  (void) XWithdrawWindow(display,windows->info.id,windows->info.screen);
  if (state & EscapeState)
    return(True);
  /*
    Set font info and check boundary conditions.
  */
  font_info=XLoadQueryFont(display,resource_info->font_name[font_id]);
  if (font_info == (XFontStruct *) NULL)
    {
      MagickXNoticeWidget(display,windows,"Unable to load font:",
        resource_info->font_name[font_id]);
      font_info=windows->font_info;
    }
  if ((x+font_info->max_bounds.width) >= (int) windows->image.width)
    x=windows->image.width-font_info->max_bounds.width;
  if (y < (long) (font_info->ascent+font_info->descent))
    y=font_info->ascent+font_info->descent;
  if ((font_info->max_bounds.width > (int) windows->image.width) ||
      ((font_info->ascent+font_info->descent) >= (int) windows->image.height))
    return(False);
  /*
    Initialize annotate structure.
  */
  annotate_info=MagickAllocateMemory(MagickXAnnotateInfo *,sizeof(MagickXAnnotateInfo));
  if (annotate_info == (MagickXAnnotateInfo *) NULL)
    return(False);
  MagickXGetAnnotateInfo(annotate_info);
  annotate_info->x=x;
  annotate_info->y=y;
  if (!transparent_box && !transparent_pen)
    annotate_info->stencil=OpaqueStencil;
  else
    if (!transparent_box)
      annotate_info->stencil=BackgroundStencil;
    else
      annotate_info->stencil=ForegroundStencil;
  annotate_info->height=font_info->ascent+font_info->descent;
  annotate_info->degrees=degrees;
  annotate_info->font_info=font_info;
  annotate_info->text=MagickAllocateMemory(char *,
    windows->image.width/Max(font_info->min_bounds.width,1)+2);
  if (annotate_info->text == (char *) NULL)
    return(False);
  /*
    Create cursor and set graphic context.
  */
  cursor=XCreateFontCursor(display,XC_pencil);
  (void) XDefineCursor(display,windows->image.id,cursor);
  annotate_context=windows->image.annotate_context;
  (void) XSetFont(display,annotate_context,font_info->fid);
  (void) XSetBackground(display,annotate_context,
    windows->pixel_info->pen_colors[box_id].pixel);
  (void) XSetForeground(display,annotate_context,
    windows->pixel_info->pen_colors[pen_id].pixel);
  /*
    Begin annotating the image with text.
  */
  (void) CloneString(&windows->command.name,"Text");
  windows->command.data=0;
  (void) MagickXCommandWidget(display,windows,TextMenu,(XEvent *) NULL);
  state=DefaultState;
  (void) XDrawString(display,windows->image.id,annotate_context,x,y,"_",1);
  text_event.xexpose.width=(unsigned int) font_info->max_bounds.width;
  text_event.xexpose.height=font_info->max_bounds.ascent+
    font_info->max_bounds.descent;
  p=annotate_info->text;
  do
  {
    /*
      Display text cursor.
    */
    *p='\0';
    (void) XDrawString(display,windows->image.id,annotate_context,x,y,"_",1);
    /*
      Wait for next event.
    */
    MagickXScreenEvent(display,windows,&event);
    if (event.xany.window == windows->command.id)
      {
        /*
          Select a command from the Command widget.
        */
        (void) XSetBackground(display,annotate_context,
          windows->pixel_info->background_color.pixel);
        (void) XSetForeground(display,annotate_context,
          windows->pixel_info->foreground_color.pixel);
        id=MagickXCommandWidget(display,windows,AnnotateMenu,&event);
        (void) XSetBackground(display,annotate_context,
          windows->pixel_info->pen_colors[box_id].pixel);
        (void) XSetForeground(display,annotate_context,
          windows->pixel_info->pen_colors[pen_id].pixel);
        if (id < 0)
          continue;
        switch (TextCommands[id])
        {
          case TextHelpCommand:
          {
            MagickXTextViewWidget(display,resource_info,windows,False,
              "Help Viewer - Image Annotation",ImageAnnotateHelp);
            (void) XDefineCursor(display,windows->image.id,cursor);
            break;
          }
          case TextApplyCommand:
          {
            /*
              Finished annotating.
            */
            annotate_info->width=XTextWidth(font_info,annotate_info->text,
              (int) strlen(annotate_info->text));
            MagickXRefreshWindow(display,&windows->image,&text_event);
            state|=ExitState;
            break;
          }
          default:
            break;
        }
        continue;
      }
    /*
      Erase text cursor.
    */
    text_event.xexpose.x=x;
    text_event.xexpose.y=y-font_info->max_bounds.ascent;
    (void) XClearArea(display,windows->image.id,x,text_event.xexpose.y,
      text_event.xexpose.width,text_event.xexpose.height,False);
    MagickXRefreshWindow(display,&windows->image,&text_event);
    switch (event.type)
    {
      case ButtonPress:
      {
        if (event.xbutton.window != windows->image.id)
          break;
        if (event.xbutton.button == Button2)
          {
            /*
              Request primary selection.
            */
            (void) XConvertSelection(display,XA_PRIMARY,XA_STRING,XA_STRING,
              windows->image.id,CurrentTime);
            break;
          }
        break;
      }
      case Expose:
      {
        if (event.xexpose.count == 0)
          {
            MagickXAnnotateInfo
              *text_info;

            /*
              Refresh Image window.
            */
            MagickXRefreshWindow(display,&windows->image,(XEvent *) NULL);
            text_info=annotate_info;
            while (text_info != (MagickXAnnotateInfo *) NULL)
            {
              if (annotate_info->stencil == ForegroundStencil)
                (void) XDrawString(display,windows->image.id,annotate_context,
                  text_info->x,text_info->y,text_info->text,
                  (int) strlen(text_info->text));
              else
                (void) XDrawImageString(display,windows->image.id,
                  annotate_context,text_info->x,text_info->y,text_info->text,
                  (int) strlen(text_info->text));
              text_info=text_info->previous;
            }
            (void) XDrawString(display,windows->image.id,annotate_context,
              x,y,"_",1);
          }
        break;
      }
      case KeyPress:
      {
        int
          length;

        if (event.xkey.window != windows->image.id)
          break;
        /*
          Respond to a user key press.
        */
        length=XLookupString((XKeyEvent *) &event.xkey,command,sizeof(command),
          &key_symbol,(XComposeStatus *) NULL);
        *(command+length)='\0';
        if ((event.xkey.state & ControlMask) || (event.xkey.state & Mod1Mask))
          state|=ModifierState;
        if (state & ModifierState)
          switch ((int) key_symbol)
          {
            case XK_u:
            case XK_U:
            {
              key_symbol=DeleteCommand;
              break;
            }
            default:
              break;
          }
        switch ((int) key_symbol)
        {
          case XK_BackSpace:
          {
            /*
              Erase one character.
            */
            if (p == annotate_info->text)
              {
                if (annotate_info->previous == (MagickXAnnotateInfo *) NULL)
                  break;
                else
                  {
                    /*
                      Go to end of the previous line of text.
                    */
                    annotate_info=annotate_info->previous;
                    p=annotate_info->text;
                    x=annotate_info->x+annotate_info->width;
                    y=annotate_info->y;
                    if (annotate_info->width != 0)
                      p+=strlen(annotate_info->text);
                    break;
                  }
              }
            p--;
            x-=XTextWidth(font_info,p,1);
            text_event.xexpose.x=x;
            text_event.xexpose.y=y-font_info->max_bounds.ascent;
            MagickXRefreshWindow(display,&windows->image,&text_event);
            break;
          }
          case XK_bracketleft:
          {
            key_symbol=XK_Escape;
            break;
          }
          case DeleteCommand:
          {
            /*
              Erase the entire line of text.
            */
            while (p != annotate_info->text)
            {
              p--;
              x-=XTextWidth(font_info,p,1);
              text_event.xexpose.x=x;
              MagickXRefreshWindow(display,&windows->image,&text_event);
            }
            break;
          }
          case XK_Escape:
          case XK_F20:
          {
            /*
              Finished annotating.
            */
            annotate_info->width=XTextWidth(font_info,annotate_info->text,
              (int) strlen(annotate_info->text));
            MagickXRefreshWindow(display,&windows->image,&text_event);
            state|=ExitState;
            break;
          }
          default:
          {
            /*
              Draw a single character on the Image window.
            */
            if (state & ModifierState)
              break;
            if (*command == '\0')
              break;
            *p=(*command);
            if (annotate_info->stencil == ForegroundStencil)
              (void) XDrawString(display,windows->image.id,annotate_context,
                x,y,p,1);
            else
              (void) XDrawImageString(display,windows->image.id,
                annotate_context,x,y,p,1);
            x+=XTextWidth(font_info,p,1);
            p++;
            if ((x+font_info->max_bounds.width) < (int) windows->image.width)
              break;
          }
          case XK_Return:
          case XK_KP_Enter:
          {
            /*
              Advance to the next line of text.
            */
            *p='\0';
            annotate_info->width=XTextWidth(font_info,annotate_info->text,
              (int) strlen(annotate_info->text));
            if (annotate_info->next != (MagickXAnnotateInfo *) NULL)
              {
                /*
                  Line of text already exists.
                */
                annotate_info=annotate_info->next;
                x=annotate_info->x;
                y=annotate_info->y;
                p=annotate_info->text;
                break;
              }
            annotate_info->next=MagickAllocateMemory(MagickXAnnotateInfo *,
              sizeof(MagickXAnnotateInfo));
            if (annotate_info->next == (MagickXAnnotateInfo *) NULL)
              return(False);
            *annotate_info->next=(*annotate_info);
            annotate_info->next->previous=annotate_info;
            annotate_info=annotate_info->next;
            annotate_info->text=MagickAllocateMemory(char *,windows->image.width/
              Max(font_info->min_bounds.width,1)+2);
            if (annotate_info->text == (char *) NULL)
              return(False);
            annotate_info->y+=annotate_info->height;
            if (annotate_info->y > (int) windows->image.height)
              annotate_info->y=annotate_info->height;
            annotate_info->next=(MagickXAnnotateInfo *) NULL;
            x=annotate_info->x;
            y=annotate_info->y;
            p=annotate_info->text;
            break;
          }
        }
        break;
      }
      case KeyRelease:
      {
        /*
          Respond to a user key release.
        */
        (void) XLookupString((XKeyEvent *) &event.xkey,command,sizeof(command),
          &key_symbol,(XComposeStatus *) NULL);
        state&=(~ModifierState);
        break;
      }
      case SelectionNotify:
      {
        Atom
          type;

        int
          format;

        unsigned char
          *data;

        unsigned long
          after,
          length;

        /*
          Obtain response from primary selection.
        */
        if (event.xselection.property == (Atom) None)
          break;
        status=XGetWindowProperty(display,event.xselection.requestor,
          event.xselection.property,0L,MaxTextExtent-1,True,XA_STRING,&type,
          &format,&length,&after,&data);
        if ((status != Success) || (type != XA_STRING) || (format == 32) ||
            (length == 0))
          break;
        /*
          Annotate Image window with primary selection.
        */
        for (i=0; i < (long) length; i++)
        {
          if ((char) data[i] != '\n')
            {
              /*
                Draw a single character on the Image window.
              */
              *p=data[i];
              (void) XDrawString(display,windows->image.id,annotate_context,
                x,y,p,1);
              x+=XTextWidth(font_info,p,1);
              p++;
              if ((x+font_info->max_bounds.width) < (int) windows->image.width)
                continue;
            }
          /*
            Advance to the next line of text.
          */
          *p='\0';
          annotate_info->width=XTextWidth(font_info,annotate_info->text,
            (int) strlen(annotate_info->text));
          if (annotate_info->next != (MagickXAnnotateInfo *) NULL)
            {
              /*
                Line of text already exists.
              */
              annotate_info=annotate_info->next;
              x=annotate_info->x;
              y=annotate_info->y;
              p=annotate_info->text;
              continue;
            }
          annotate_info->next=MagickAllocateMemory(MagickXAnnotateInfo *,
            sizeof(MagickXAnnotateInfo));
          if (annotate_info->next == (MagickXAnnotateInfo *) NULL)
            return(False);
          *annotate_info->next=(*annotate_info);
          annotate_info->next->previous=annotate_info;
          annotate_info=annotate_info->next;
          annotate_info->text=MagickAllocateMemory(char *,windows->image.width/
            Max(font_info->min_bounds.width,1)+2);
          if (annotate_info->text == (char *) NULL)
            return(False);
          annotate_info->y+=annotate_info->height;
          if (annotate_info->y > (int) windows->image.height)
            annotate_info->y=annotate_info->height;
          annotate_info->next=(MagickXAnnotateInfo *) NULL;
          x=annotate_info->x;
          y=annotate_info->y;
          p=annotate_info->text;
        }
        (void) XFree((void *) data);
        break;
      }
      default:
        break;
    }
  } while (!(state & ExitState));
  (void) XFreeCursor(display,cursor);
  /*
    Annotation is relative to image configuration.
  */
  width=(unsigned int) image->columns;
  height=(unsigned int) image->rows;
  x=0;
  y=0;
  if (windows->image.crop_geometry != (char *) NULL)
    (void) XParseGeometry(windows->image.crop_geometry,&x,&y,&width,&height);
  /*
    Initialize annotated image.
  */
  MagickXSetCursorState(display,windows,True);
  MagickXCheckRefreshWindows(display,windows);
  while (annotate_info != (MagickXAnnotateInfo *) NULL)
  {
    if (annotate_info->width == 0)
      {
        /*
          No text on this line--  go to the next line of text.
        */
        previous_info=annotate_info->previous;
        MagickFreeMemory(annotate_info->text);
        MagickFreeMemory(annotate_info);
        annotate_info=previous_info;
        continue;
      }
    /*
      Determine pixel index for box and pen color.
    */
    windows->pixel_info->box_color=windows->pixel_info->pen_colors[box_id];
    if (windows->pixel_info->colors != 0)
      for (i=0; i < (long) windows->pixel_info->colors; i++)
        if (windows->pixel_info->pixels[i] ==
            windows->pixel_info->pen_colors[box_id].pixel)
          {
            windows->pixel_info->box_index=(unsigned short) i;
            break;
          }
    windows->pixel_info->pen_color=windows->pixel_info->pen_colors[pen_id];
    if (windows->pixel_info->colors != 0)
      for (i=0; i < (long) windows->pixel_info->colors; i++)
        if (windows->pixel_info->pixels[i] ==
            windows->pixel_info->pen_colors[pen_id].pixel)
          {
            windows->pixel_info->pen_index=(unsigned short) i;
            break;
          }
    /*
      Define the annotate geometry string.
    */
    annotate_info->x=
      width*(annotate_info->x+windows->image.x)/windows->image.ximage->width;
    annotate_info->y=height*(annotate_info->y-font_info->ascent+
      windows->image.y)/windows->image.ximage->height;
    FormatString(annotate_info->geometry,"%ux%u%+d%+d",
      width*annotate_info->width/windows->image.ximage->width,
      height*annotate_info->height/windows->image.ximage->height,
      annotate_info->x+x,annotate_info->y+y);
    /*
      Annotate image with text.
    */
    status=MagickXAnnotateImage(display,windows->pixel_info,annotate_info,image);
    if (status == 0)
      return(False);
    /*
      Free up memory.
    */
    previous_info=annotate_info->previous;
    MagickFreeMemory(annotate_info->text);
    MagickFreeMemory(annotate_info);
    annotate_info=previous_info;
  }
  (void) XSetForeground(display,annotate_context,
    windows->pixel_info->foreground_color.pixel);
  (void) XSetBackground(display,annotate_context,
    windows->pixel_info->background_color.pixel);
  (void) XSetFont(display,annotate_context,windows->font_info->fid);
  MagickXSetCursorState(display,windows,False);
  (void) XFreeFont(display,font_info);
  /*
    Update image configuration.
  */
  MagickXConfigureImageColormap(display,resource_info,windows,image);
  (void) MagickXConfigureImage(display,resource_info,windows,image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k X B a c k g r o u n d I m a g e                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickXBackgroundImage displays the image in the background of a window.
%
%  The format of the MagickXBackgroundImage method is:
%
%      unsigned int MagickXBackgroundImage(Display *display,
%        MagickXResourceInfo *resource_info,MagickXWindows *windows,Image **image)
%
%  A description of each parameter follows:
%
%    o status: Method MagickXBackgroundImage return True if the image is
%      printed.  False is returned is there is a memory shortage or if the
%      image fails to print.
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 MagickXResourceInfo structure.
%
%    o windows: Specifies a pointer to a MagickXWindows structure.
%
%    o image: Specifies a pointer to an Image structure;  returned from
%      ReadImage.
%
%
*/
static unsigned int MagickXBackgroundImage(Display *display,
  MagickXResourceInfo *resource_info,MagickXWindows *windows,Image **image)
{
#define BackgroundImageText  "  Background the image...  "

  static char
    window_id[MaxTextExtent] = "root";

  MagickXResourceInfo
    background_resources;

  unsigned int
    status;

  /*
    Put image in background.
  */
  status=MagickXDialogWidget(display,windows,"Background",
    "Enter window id (id 0x00 selects window with pointer):",window_id);
  if (*window_id == '\0')
    return(False);
  (void) MagickXMagickCommand(display,resource_info,windows,ApplyCommand,image);
  MagickXInfoWidget(display,windows,BackgroundImageText);
  /*
    Display hourglass cursor if progress indication enabled.
  */
  if (resource_info->image_info->progress)
    MagickXSetCursorState(display,windows,True);
  MagickXCheckRefreshWindows(display,windows);
  background_resources=(*resource_info);
  background_resources.window_id=window_id;
  background_resources.backdrop=status;
  status=MagickXDisplayBackgroundImage(display,&background_resources,*image);
  if (status)
    MagickXClientMessage(display,windows->image.id,windows->im_protocols,
      windows->im_retain_colors,CurrentTime);
  MagickXSetCursorState(display,windows,False);
  (void) MagickXMagickCommand(display,resource_info,windows,UndoCommand,image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k X C h o p I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickXChopImage chops the X image.
%
%  The format of the MagickXChopImage method is:
%
%    unsigned int MagickXChopImage(Display *display,MagickXResourceInfo *resource_info,
%      MagickXWindows *windows,Image **image)
%
%  A description of each parameter follows:
%
%    o status: Method MagickXChopImage return True if the image is
%      cut.  False is returned is there is a memory shortage or if the
%      image fails to cut.
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 MagickXResourceInfo structure.
%
%    o windows: Specifies a pointer to a MagickXWindows structure.
%
%    o image: Specifies a pointer to an Image structure;  returned from
%      ReadImage.
%
%
*/
static unsigned int MagickXChopImage(Display *display,MagickXResourceInfo *resource_info,
  MagickXWindows *windows,Image **image)
{
  static const char
    *ChopMenu[]=
    {
      "Direction",
      "Help",
      "Dismiss",
      (char *) NULL
    };

  static ModeType
    direction = HorizontalChopCommand;

  static const ModeType
    ChopCommands[]=
    {
      ChopDirectionCommand,
      ChopHelpCommand,
      ChopDismissCommand
    },
    DirectionCommands[]=
    {
      HorizontalChopCommand,
      VerticalChopCommand
    };

  char
    text[MaxTextExtent];

  double
    scale_factor;

  Image
    *chop_image;

  int
    id,
    x,
    y;

  RectangleInfo
    chop_info;

  unsigned int
    distance,
    height,
    width;

  unsigned long
    state;

  XEvent
    event;

  XSegment
    segment_info;

  /*
    Map Command widget.
  */
  (void) CloneString(&windows->command.name,"Chop");
  windows->command.data=1;
  (void) MagickXCommandWidget(display,windows,ChopMenu,(XEvent *) NULL);
  (void) XMapRaised(display,windows->command.id);
  MagickXClientMessage(display,windows->image.id,windows->im_protocols,
    windows->im_update_widget,CurrentTime);
  /*
    Track pointer until button 1 is pressed.
  */
  MagickXQueryPosition(display,windows->image.id,&x,&y);
  (void) XSelectInput(display,windows->image.id,
    windows->image.attributes.event_mask | PointerMotionMask);
  state=DefaultState;
  do
  {
    if (windows->info.mapped)
      {
        /*
          Display pointer position.
        */
        FormatString(text," %+d%+d ",x+windows->image.x,y+windows->image.y);
        MagickXInfoWidget(display,windows,text);
      }
    /*
      Wait for next event.
    */
    MagickXScreenEvent(display,windows,&event);
    if (event.xany.window == windows->command.id)
      {
        /*
          Select a command from the Command widget.
        */
        id=MagickXCommandWidget(display,windows,ChopMenu,&event);
        if (id < 0)
          continue;
        switch (ChopCommands[id])
        {
          case ChopDirectionCommand:
          {
            char
              command[MaxTextExtent];

            static const char
              *Directions[]=
              {
                "horizontal",
                "vertical",
                (char *) NULL,
              };

            /*
              Select a command from the pop-up menu.
            */
            id=
              MagickXMenuWidget(display,windows,ChopMenu[id],Directions,command);
            if (id >= 0)
              direction=DirectionCommands[id];
            break;
          }
          case ChopHelpCommand:
          {
            MagickXTextViewWidget(display,resource_info,windows,False,
              "Help Viewer - Image Chop",ImageChopHelp);
            break;
          }
          case ChopDismissCommand:
          {
            /*
              Prematurely exit.
            */
            state|=EscapeState;
            state|=ExitState;
            break;
          }
          default:
            break;
        }
        continue;
      }
    switch (event.type)
    {
      case ButtonPress:
      {
        if (event.xbutton.button != Button1)
          break;
        if (event.xbutton.window != windows->image.id)
          break;
        /*
          User has committed to start point of chopping line.
        */
        segment_info.x1=event.xbutton.x;
        segment_info.x2=event.xbutton.x;
        segment_info.y1=event.xbutton.y;
        segment_info.y2=event.xbutton.y;
        state|=ExitState;
        break;
      }
      case ButtonRelease:
        break;
      case Expose:
        break;
      case KeyPress:
      {
        char
          command[MaxTextExtent];

        KeySym
          key_symbol;

        if (event.xkey.window != windows->image.id)
          break;
        /*
          Respond to a user key press.
        */
        (void) XLookupString((XKeyEvent *) &event.xkey,command,
          sizeof(command),&key_symbol,(XComposeStatus *) NULL);
        switch ((int) key_symbol)
        {
          case XK_Escape:
          case XK_F20:
          {
            /*
              Prematurely exit.
            */
            state|=EscapeState;
            state|=ExitState;
            break;
          }
          case XK_F1:
          case XK_Help:
          {
            (void) XSetFunction(display,windows->image.highlight_context,
              GXcopy);
            MagickXTextViewWidget(display,resource_info,windows,False,
              "Help Viewer - Image Chop",ImageChopHelp);
            (void) XSetFunction(display,windows->image.highlight_context,
              GXinvert);
            break;
          }
          default:
          {
            (void) XBell(display,0);
            break;
          }
        }
        break;
      }
      case MotionNotify:
      {
        /*
          Map and unmap Info widget as text cursor crosses its boundaries.
        */
        x=event.xmotion.x;
        y=event.xmotion.y;
        if (windows->info.mapped)
          {
            if ((x < (int) (windows->info.x+windows->info.width)) &&
                (y < (int) (windows->info.y+windows->info.height)))
              (void) XWithdrawWindow(display,windows->info.id,
                windows->info.screen);
          }
        else
          if ((x > (int) (windows->info.x+windows->info.width)) ||
              (y > (int) (windows->info.y+windows->info.height)))
            (void) XMapWindow(display,windows->info.id);
      }
    }
  } while (!(state & ExitState));
  (void) XSelectInput(display,windows->image.id,
    windows->image.attributes.event_mask);
  (void) XWithdrawWindow(display,windows->info.id,windows->info.screen);
  if (state & EscapeState)
    return(True);
  /*
    Draw line as pointer moves until the mouse button is released.
  */
  chop_info.width=0;
  chop_info.height=0;
  chop_info.x=0;
  chop_info.y=0;
  distance=0;
  (void) XSetFunction(display,windows->image.highlight_context,GXinvert);
  state=DefaultState;
  do
  {
    if (distance > 9)
      {
        /*
          Display info and draw chopping line.
        */
        if (!windows->info.mapped)
          (void) XMapWindow(display,windows->info.id);
        FormatString(text," %lux%lu%+ld%+ld",chop_info.width,chop_info.height,
          chop_info.x,chop_info.y);
        MagickXInfoWidget(display,windows,text);
        MagickXHighlightLine(display,windows->image.id,
          windows->image.highlight_context,&segment_info);
      }
    else
      if (windows->info.mapped)
        (void) XWithdrawWindow(display,windows->info.id,windows->info.screen);
    /*
      Wait for next event.
    */
    MagickXScreenEvent(display,windows,&event);
    if (distance > 9)
      MagickXHighlightLine(display,windows->image.id,
        windows->image.highlight_context,&segment_info);
    switch (event.type)
    {
      case ButtonPress:
      {
        segment_info.x2=event.xmotion.x;
        segment_info.y2=event.xmotion.y;
        break;
      }
      case ButtonRelease:
      {
        /*
          User has committed to chopping line.
        */
        segment_info.x2=event.xbutton.x;
        segment_info.y2=event.xbutton.y;
        state|=ExitState;
        break;
      }
      case Expose:
        break;
      case MotionNotify:
      {
        segment_info.x2=event.xmotion.x;
        segment_info.y2=event.xmotion.y;
      }
      default:
        break;
    }
    /*
      Check boundary conditions.
    */
    if (segment_info.x2 < 0)
      segment_info.x2=0;
    else
      if (segment_info.x2 > windows->image.ximage->width)
        segment_info.x2=windows->image.ximage->width;
    if (segment_info.y2 < 0)
      segment_info.y2=0;
    else
      if (segment_info.y2 > windows->image.ximage->height)
        segment_info.y2=windows->image.ximage->height;
    distance=
      ((segment_info.x2-segment_info.x1)*(segment_info.x2-segment_info.x1))+
      ((segment_info.y2-segment_info.y1)*(segment_info.y2-segment_info.y1));
    /*
      Compute chopping geometry.
    */
    if (direction == HorizontalChopCommand)
      {
        chop_info.width=segment_info.x2-segment_info.x1+1;
        chop_info.x=windows->image.x+segment_info.x1;
        chop_info.height=0;
        chop_info.y=0;
        if (segment_info.x1 > (int) segment_info.x2)
          {
            chop_info.width=segment_info.x1-segment_info.x2+1;
            chop_info.x=windows->image.x+segment_info.x2;
          }
      }
    else
      {
        chop_info.width=0;
        chop_info.height=segment_info.y2-segment_info.y1+1;
        chop_info.x=0;
        chop_info.y=windows->image.y+segment_info.y1;
        if (segment_info.y1 > segment_info.y2)
          {
            chop_info.height=segment_info.y1-segment_info.y2+1;
            chop_info.y=windows->image.y+segment_info.y2;
          }
      }
  } while (!(state & ExitState));
  (void) XSetFunction(display,windows->image.highlight_context,GXcopy);
  (void) XWithdrawWindow(display,windows->info.id,windows->info.screen);
  if (distance <= 9)
    return(True);
  /*
    Image chopping is relative to image configuration.
  */
  (void) MagickXMagickCommand(display,resource_info,windows,ApplyCommand,image);
  MagickXSetCursorState(display,windows,True);
  MagickXCheckRefreshWindows(display,windows);
  windows->image.window_changes.width=
    windows->image.ximage->width-(unsigned int) chop_info.width;
  windows->image.window_changes.height=
    windows->image.ximage->height-(unsigned int) chop_info.height;
  width=(unsigned int) (*image)->columns;
  height=(unsigned int) (*image)->rows;
  x=0;
  y=0;
  if (windows->image.crop_geometry != (char *) NULL)
    (void) XParseGeometry(windows->image.crop_geometry,&x,&y,&width,&height);
  scale_factor=(double) width/windows->image.ximage->width;
  chop_info.x+=x;
  chop_info.x=(int) (scale_factor*chop_info.x+0.5);
  chop_info.width=(unsigned int) (scale_factor*chop_info.width+0.5);
  scale_factor=(double) height/windows->image.ximage->height;
  chop_info.y+=y;
  chop_info.y=(int) (scale_factor*chop_info.y+0.5);
  chop_info.height=(unsigned int) (scale_factor*chop_info.height+0.5);
  /*
    Chop image.
  */
  chop_image=ChopImage(*image,&chop_info,&(*image)->exception);
  MagickXSetCursorState(display,windows,False);
  if (chop_image == (Image *) NULL)
    return(False);
  DestroyImage(*image);
  *image=chop_image;
  /*
    Update image configuration.
  */
  MagickXConfigureImageColormap(display,resource_info,windows,*image);
  (void) MagickXConfigureImage(display,resource_info,windows,*image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k X C o l o r E d i t I m a g e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickXColorEditImage allows the user to interactively change
%  the color of one pixel for a DirectColor image or one colormap entry for
%  a PseudoClass image.
%
%  The format of the MagickXColorEditImage method is:
%
%      unsigned int MagickXColorEditImage(Display *display,
%        MagickXResourceInfo *resource_info,MagickXWindows *windows,Image **image)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 MagickXResourceInfo structure.
%
%    o windows: Specifies a pointer to a MagickXWindows structure.
%
%    o image: Specifies a pointer to an Image structure; returned from
%      ReadImage.
%
*/
static unsigned int MagickXColorEditImage(Display *display,
  MagickXResourceInfo *resource_info,MagickXWindows *windows,Image **image)
{
  static const char
    *ColorEditMenu[]=
    {
      "Method",
      "Pixel Color",
      "Border Color",
      "Fuzz",
      "Undo",
      "Help",
      "Dismiss",
      (char *) NULL
    };

  static const ModeType
    ColorEditCommands[]=
    {
      ColorEditMethodCommand,
      ColorEditColorCommand,
      ColorEditBorderCommand,
      ColorEditFuzzCommand,
      ColorEditUndoCommand,
      ColorEditHelpCommand,
      ColorEditDismissCommand
    };

  static PaintMethod
    method = PointMethod;

  static unsigned int
    pen_id = 0;

  static XColor
    border_color = { 0, 0, 0, 0, 0, 0 }; /* Also fill 'pad' field */

  char
    command[MaxTextExtent],
    text[MaxTextExtent];

  Cursor
    cursor;

  int
    entry,
    id,
    x,
    x_offset,
    y,
    y_offset;

  register PixelPacket
    *q;

  register long
    i;

  unsigned int
    height,
    width;

  unsigned long
    state;

  XColor
    color;

  XEvent
    event;

  /*
    Map Command widget.
  */
  (void) CloneString(&windows->command.name,"Color Edit");
  windows->command.data=4;
  (void) MagickXCommandWidget(display,windows,ColorEditMenu,(XEvent *) NULL);
  (void) XMapRaised(display,windows->command.id);
  MagickXClientMessage(display,windows->image.id,windows->im_protocols,
    windows->im_update_widget,CurrentTime);
  /*
    Make cursor.
  */
  cursor=MagickXMakeCursor(display,windows->image.id,windows->map_info->colormap,
    resource_info->background_color,resource_info->foreground_color);
  (void) XDefineCursor(display,windows->image.id,cursor);
  /*
    Track pointer until button 1 is pressed.
  */
  MagickXQueryPosition(display,windows->image.id,&x,&y);
  (void) XSelectInput(display,windows->image.id,
    windows->image.attributes.event_mask | PointerMotionMask);
  state=DefaultState;
  do
  {
    if (windows->info.mapped)
      {
        /*
          Display pointer position.
        */
        FormatString(text," %+d%+d ",x+windows->image.x,y+windows->image.y);
        MagickXInfoWidget(display,windows,text);
      }
    /*
      Wait for next event.
    */
    MagickXScreenEvent(display,windows,&event);
    if (event.xany.window == windows->command.id)
      {
        /*
          Select a command from the Command widget.
        */
        id=MagickXCommandWidget(display,windows,ColorEditMenu,&event);
        if (id < 0)
          {
            (void) XDefineCursor(display,windows->image.id,cursor);
            continue;
          }
        switch (ColorEditCommands[id])
        {
          case ColorEditMethodCommand:
          {
            static const char
              *MethodMenu[]=
              {
                "point",
                "replace",
                "floodfill",
                "filltoborder",
                "reset",
                (char *) NULL,
              };

            /*
              Select a method from the pop-up menu.
            */
            entry=
              MagickXMenuWidget(display,windows,ColorEditMenu[id],MethodMenu,command);
            if (entry >= 0)
              method=(PaintMethod) entry;
            break;
          }
          case ColorEditColorCommand:
          {
            char
              *ColorMenu[MaxNumberPens];

            int
              pen_number;

            /*
              Initialize menu selections.
            */
            for (i=0; i < (int) (MaxNumberPens-2); i++)
              ColorMenu[i]=resource_info->pen_colors[i];
            ColorMenu[MaxNumberPens-2]=(char *) "Browser...";
            ColorMenu[MaxNumberPens-1]=(char *) NULL;
            /*
              Select a pen color from the pop-up menu.
            */
            pen_number=MagickXMenuWidget(display,windows,ColorEditMenu[id],
              (const char **) ColorMenu,command);
            if (pen_number < 0)
              break;
            if (pen_number == (MaxNumberPens-2))
              {
                static char
                  color_name[MaxTextExtent] = "gray";

                /*
                  Select a pen color from a dialog.
                */
                resource_info->pen_colors[pen_number]=color_name;
                MagickXColorBrowserWidget(display,windows,"Select",color_name);
                if (*color_name == '\0')
                  break;
              }
            /*
              Set pen color.
            */
            (void) XParseColor(display,windows->map_info->colormap,
              resource_info->pen_colors[pen_number],&color);
            MagickXBestPixel(display,windows->map_info->colormap,(XColor *) NULL,
              (unsigned int) MaxColors,&color);
            windows->pixel_info->pen_colors[pen_number]=color;
            pen_id=pen_number;
            break;
          }
          case ColorEditBorderCommand:
          {
            char
              *ColorMenu[MaxNumberPens];

            int
              pen_number;

            /*
              Initialize menu selections.
            */
            for (i=0; i < (int) (MaxNumberPens-2); i++)
              ColorMenu[i]=resource_info->pen_colors[i];
            ColorMenu[MaxNumberPens-2]=(char *) "Browser...";
            ColorMenu[MaxNumberPens-1]=(char *) NULL;
            /*
              Select a pen color from the pop-up menu.
            */
            pen_number=MagickXMenuWidget(display,windows,ColorEditMenu[id],
              (const char **) ColorMenu,command);
            if (pen_number < 0)
              break;
            if (pen_number == (MaxNumberPens-2))
              {
                static char
                  color_name[MaxTextExtent] = "gray";

                /*
                  Select a pen color from a dialog.
                */
                resource_info->pen_colors[pen_number]=color_name;
                MagickXColorBrowserWidget(display,windows,"Select",color_name);
                if (*color_name == '\0')
                  break;
              }
            /*
              Set border color.
            */
            (void) XParseColor(display,windows->map_info->colormap,
              resource_info->pen_colors[pen_number],&border_color);
            break;
          }
          case ColorEditFuzzCommand:
          {
            static char
              fuzz[MaxTextExtent];

            static const char
              *FuzzMenu[]=
              {
                "0%",
                "2%",
                "5%",
                "10%",
                "15%",
                "Dialog...",
                (char *) NULL,
              };

            /*
              Select a command from the pop-up menu.
            */
            entry=MagickXMenuWidget(display,windows,ColorEditMenu[id],FuzzMenu,
              command);
            if (entry < 0)
              break;
            if (entry != 5)
              {
                (*image)->fuzz=StringToDouble(FuzzMenu[entry],MaxRGB);
                break;
              }
            (void) strcpy(fuzz,"20%");
            (void) MagickXDialogWidget(display,windows,"Ok",
              "Enter fuzz factor (0.0 - 99.9%):",fuzz);
            if (*fuzz == '\0')
              break;
            (void) strcat(fuzz,"%");
            (*image)->fuzz=StringToDouble(fuzz,MaxRGB);
            break;
          }
          case ColorEditUndoCommand:
          {
            (void) MagickXMagickCommand(display,resource_info,windows,UndoCommand,
              image);
            break;
          }
          case ColorEditHelpCommand:
          default:
          {
            MagickXTextViewWidget(display,resource_info,windows,False,
              "Help Viewer - Image Annotation",ImageColorEditHelp);
            break;
          }
          case ColorEditDismissCommand:
          {
            /*
              Prematurely exit.
            */
            state|=EscapeState;
            state|=ExitState;
            break;
          }
        }
        (void) XDefineCursor(display,windows->image.id,cursor);
        continue;
      }
    switch (event.type)
    {
      case ButtonPress:
      {
        if (event.xbutton.button != Button1)
          break;
        if ((event.xbutton.window != windows->image.id) &&
            (event.xbutton.window != windows->magnify.id))
          break;
        /*
          Exit loop.
        */
        x=event.xbutton.x;
        y=event.xbutton.y;
        (void) MagickXMagickCommand(display,resource_info,windows,
          SaveToUndoBufferCommand,image);
        state|=UpdateConfigurationState;
        break;
      }
      case ButtonRelease:
      {
        if (event.xbutton.button != Button1)
          break;
        if ((event.xbutton.window != windows->image.id) &&
            (event.xbutton.window != windows->magnify.id))
          break;
        /*
          Update colormap information.
        */
        x=event.xbutton.x;
        y=event.xbutton.y;
        MagickXConfigureImageColormap(display,resource_info,windows,*image);
        (void) MagickXConfigureImage(display,resource_info,windows,*image);
        MagickXInfoWidget(display,windows,text);
        (void) XDefineCursor(display,windows->image.id,cursor);
        state&=(~UpdateConfigurationState);
        break;
      }
      case Expose:
        break;
      case KeyPress:
      {
        KeySym
          key_symbol;

        if (event.xkey.window == windows->magnify.id)
          {
            Window
              window;

            window=windows->magnify.id;
            while (XCheckWindowEvent(display,window,KeyPressMask,&event));
          }
        if (event.xkey.window != windows->image.id)
          break;
        /*
          Respond to a user key press.
        */
        (void) XLookupString((XKeyEvent *) &event.xkey,command,sizeof(command),
          &key_symbol,(XComposeStatus *) NULL);
        switch ((int) key_symbol)
        {
          case XK_Escape:
          case XK_F20:
          {
            /*
              Prematurely exit.
            */
            state|=ExitState;
            break;
          }
          case XK_F1:
          case XK_Help:
          {
            MagickXTextViewWidget(display,resource_info,windows,False,
              "Help Viewer - Image Annotation",ImageColorEditHelp);
            break;
          }
          default:
          {
            (void) XBell(display,0);
            break;
          }
        }
        break;
      }
      case MotionNotify:
      {
        /*
          Map and unmap Info widget as cursor crosses its boundaries.
        */
        x=event.xmotion.x;
        y=event.xmotion.y;
        if (windows->info.mapped)
          {
            if ((x < (int) (windows->info.x+windows->info.width)) &&
                (y < (int) (windows->info.y+windows->info.height)))
              (void) XWithdrawWindow(display,windows->info.id,
                windows->info.screen);
          }
        else
          if ((x > (int) (windows->info.x+windows->info.width)) ||
              (y > (int) (windows->info.y+windows->info.height)))
            (void) XMapWindow(display,windows->info.id);
        break;
      }
      default:
        break;
    }
    if (event.xany.window == windows->magnify.id)
      {
        x=windows->magnify.x-windows->image.x;
        y=windows->magnify.y-windows->image.y;
      }
    x_offset=x;
    y_offset=y;
    if (state & UpdateConfigurationState)
      {
        int
          x,
          y;

        /*
          Pixel edit is relative to image configuration.
        */
        (void) XClearArea(display,windows->image.id,x_offset,y_offset,1,1,True);
        color=windows->pixel_info->pen_colors[pen_id];
        XPutPixel(windows->image.ximage,x_offset,y_offset,color.pixel);
        width=(unsigned int) (*image)->columns;
        height=(unsigned int) (*image)->rows;
        x=0;
        y=0;
        if (windows->image.crop_geometry != (char *) NULL)
          (void) XParseGeometry(windows->image.crop_geometry,&x,&y,
            &width,&height);
        x_offset=
          width*(windows->image.x+x_offset)/windows->image.ximage->width+x;
        y_offset=
          height*(windows->image.y+y_offset)/windows->image.ximage->height+y;
        if ((x_offset < 0) || (y_offset < 0))
          continue;
        if ((x_offset >= (long) (*image)->columns) ||
            (y_offset >= (long) (*image)->rows))
          continue;
        switch (method)
        {
          case PointMethod:
          default:
          {
            /*
              Update color information using point algorithm.
            */
            (void) SetImageType(*image,TrueColorType);
            q=GetImagePixels(*image,x_offset,y_offset,1,1);
            if (q == (PixelPacket *) NULL)
              break;
            q->red=ScaleShortToQuantum(color.red);
            q->green=ScaleShortToQuantum(color.green);
            q->blue=ScaleShortToQuantum(color.blue);
            (void) SyncImagePixels(*image);
            break;
          }
          case ReplaceMethod:
          {
            PixelPacket
              target;

            /*
              Update color information using replace algorithm.
            */
            (void) AcquireOnePixelByReference(*image,&target,x_offset,y_offset,&((*image)->exception));
            if ((*image)->storage_class == DirectClass)
              {
                for (y=0; y < (long) (*image)->rows; y++)
                {
                  q=GetImagePixels(*image,0,y,(*image)->columns,1);
                  if (q == (PixelPacket *) NULL)
                    break;
                  for (x=0; x < (int) (*image)->columns; x++)
                  {
                    if (FuzzyColorMatch(q,&target,(*image)->fuzz))
                      {
                        q->red=ScaleShortToQuantum(color.red);
                        q->green=ScaleShortToQuantum(color.green);
                        q->blue=ScaleShortToQuantum(color.blue);
                      }
                    q++;
                  }
                  if (!SyncImagePixels(*image))
                    break;
                }
              }
            else
              {
                for (i=0; i < (int) (*image)->colors; i++)
                  if (FuzzyColorMatch((*image)->colormap+i,&target,(*image)->fuzz))
                    {
                      (*image)->colormap[i].red=ScaleShortToQuantum(color.red);
                      (*image)->colormap[i].green=
                        ScaleShortToQuantum(color.green);
                      (*image)->colormap[i].blue=
                        ScaleShortToQuantum(color.blue);
                    }
                (void) SyncImage(*image);
              }
            break;
          }
          case FloodfillMethod:
          case FillToBorderMethod:
          {
            DrawInfo
              *draw_info;

            PixelPacket
              target;

            /*
              Update color information using floodfill algorithm.
            */
            (void) AcquireOnePixelByReference(*image,&target,x_offset,y_offset,&((*image)->exception));
            if (method == FillToBorderMethod)
              {
                target.red=ScaleShortToQuantum(border_color.red);
                target.green=ScaleShortToQuantum(border_color.green);
                target.blue=ScaleShortToQuantum(border_color.blue);
              }
            draw_info=
              CloneDrawInfo(resource_info->image_info,(DrawInfo *) NULL);
            (void) QueryColorDatabase(resource_info->pen_colors[pen_id],
              &draw_info->fill,&(*image)->exception);
            (void) ColorFloodfillImage(*image,draw_info,target,x_offset,
              y_offset,method);
            DestroyDrawInfo(draw_info);
            break;
          }
          case ResetMethod:
          {
            /*
              Update color information using reset algorithm.
            */
            (void) SetImageType(*image,TrueColorType);
            for (y=0; y < (long) (*image)->rows; y++)
            {
              q=SetImagePixels(*image,0,y,(*image)->columns,1);
              if (q == (PixelPacket *) NULL)
                break;
              for (x=0; x < (int) (*image)->columns; x++)
              {
                q->red=ScaleShortToQuantum(color.red);
                q->green=ScaleShortToQuantum(color.green);
                q->blue=ScaleShortToQuantum(color.blue);
                q++;
              }
              if (!SyncImagePixels(*image))
                break;
            }
            break;
          }
        }
        state&=(~UpdateConfigurationState);
      }
  } while (!(state & ExitState));
  (void) XSelectInput(display,windows->image.id,
    windows->image.attributes.event_mask);
  MagickXSetCursorState(display,windows,False);
  (void) XFreeCursor(display,cursor);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k X C o m p o s i t e I m a g e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickXCompositeImage requests an image name from the user, reads
%  the image and composites it with the X window image at a location the user
%  chooses with the pointer.
%
%  The format of the MagickXCompositeImage method is:
%
%      unsigned int MagickXCompositeImage(Display *display,
%        MagickXResourceInfo *resource_info,MagickXWindows *windows,Image *image)
%
%  A description of each parameter follows:
%
%    o status: Method MagickXCompositeImage returns True if the image is
%      composited.  False is returned is there is a memory shortage or if the
%      image fails to be composited.
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 MagickXResourceInfo structure.
%
%    o windows: Specifies a pointer to a MagickXWindows structure.
%
%    o image: Specifies a pointer to an Image structure; returned from
%      ReadImage.
%
*/
static unsigned int MagickXCompositeImage(Display *display,
  MagickXResourceInfo *resource_info,MagickXWindows *windows,Image *image)
{
  static char
    displacement_geometry[MaxTextExtent] = "30x30",
    filename[MaxTextExtent] = "\0";

  static const char
    *CompositeMenu[]=
    {
      "Operators",
      "Dissolve",
      "Displace",
      "Help",
      "Dismiss",
      (char *) NULL
    };

  static CompositeOperator
    compose = CopyCompositeOp;

  static const ModeType
    CompositeCommands[]=
    {
      CompositeOperatorsCommand,
      CompositeDissolveCommand,
      CompositeDisplaceCommand,
      CompositeHelpCommand,
      CompositeDismissCommand
    };

  char
    text[MaxTextExtent];

  Cursor
    cursor;

  double
    blend,
    scale_factor;

  Image
    *composite_image;

  int
    id,
    x,
    y;

  RectangleInfo
    highlight_info,
    composite_info;

  unsigned int
    height,
    width;

  unsigned long
    state;

  XEvent
    event;

  /*
    Request image file name from user.
  */
  MagickXFileBrowserWidget(display,windows,"Composite",filename);
  if (*filename == '\0')
    return(True);
  /*
    Read image.
  */
  MagickXSetCursorState(display,windows,True);
  MagickXCheckRefreshWindows(display,windows);
  (void) strlcpy(resource_info->image_info->filename,filename,MaxTextExtent);
  composite_image=ReadImage(resource_info->image_info,&image->exception);
  if (image->exception.severity != UndefinedException)
    MagickError2(image->exception.severity,image->exception.reason,
      image->exception.description);
  MagickXSetCursorState(display,windows,False);
  if (composite_image == (Image *) NULL)
    return(False);
  if (!composite_image->matte)
    {
      /*
        Request mask image file name from user.
      */
      MagickXNoticeWidget(display,windows,
        "Your image does not have the required matte information.",
        "Press dismiss and choose an image to use as a mask.");
      MagickXFileBrowserWidget(display,windows,"Composite",filename);
      if (*filename != '\0')
        {
          char
            size[MaxTextExtent];

          Image
            *mask_image;

          ImageInfo
            *image_info;

          /*
            Read image.
          */
          MagickXSetCursorState(display,windows,True);
          MagickXCheckRefreshWindows(display,windows);
          image_info=CloneImageInfo((ImageInfo *) NULL);
          (void) strlcpy(image_info->filename,filename,MaxTextExtent);
          (void) CloneString(&image_info->size,size);
          FormatString(image_info->size,"%lux%lu",composite_image->columns,
            composite_image->rows);
          mask_image=ReadImage(image_info,&image->exception);
          if (image->exception.severity != UndefinedException)
            MagickError2(image->exception.severity,image->exception.reason,
              image->exception.description);
          MagickXSetCursorState(display,windows,False);
          if (mask_image == (Image *) NULL)
            return(False);
          (void) CompositeImage(composite_image,CopyOpacityCompositeOp,
            mask_image,0,0);
          DestroyImage(mask_image);
          DestroyImageInfo(image_info);
        }
    }
  /*
    Map Command widget.
  */
  (void) CloneString(&windows->command.name,"Composite");
  windows->command.data=1;
  (void) MagickXCommandWidget(display,windows,CompositeMenu,(XEvent *) NULL);
  (void) XMapRaised(display,windows->command.id);
  MagickXClientMessage(display,windows->image.id,windows->im_protocols,
    windows->im_update_widget,CurrentTime);
  /*
    Track pointer until button 1 is pressed.
  */
  MagickXQueryPosition(display,windows->image.id,&x,&y);
  (void) XSelectInput(display,windows->image.id,
    windows->image.attributes.event_mask | PointerMotionMask);
  composite_info.x=windows->image.x+x;
  composite_info.y=windows->image.y+y;
  composite_info.width=0;
  composite_info.height=0;
  cursor=XCreateFontCursor(display,XC_ul_angle);
  (void) XSetFunction(display,windows->image.highlight_context,GXinvert);
  blend=0.0;
  state=DefaultState;
  do
  {
    if (windows->info.mapped)
      {
        /*
          Display pointer position.
        */
        FormatString(text," %+ld%+ld ",composite_info.x,composite_info.y);
        MagickXInfoWidget(display,windows,text);
      }
    highlight_info=composite_info;
    highlight_info.x=composite_info.x-windows->image.x;
    highlight_info.y=composite_info.y-windows->image.y;
    MagickXHighlightRectangle(display,windows->image.id,
      windows->image.highlight_context,&highlight_info);
    /*
      Wait for next event.
    */
    MagickXScreenEvent(display,windows,&event);
    MagickXHighlightRectangle(display,windows->image.id,
      windows->image.highlight_context,&highlight_info);
    if (event.xany.window == windows->command.id)
      {
        /*
          Select a command from the Command widget.
        */
        id=MagickXCommandWidget(display,windows,CompositeMenu,&event);
        if (id < 0)
          continue;
        switch (CompositeCommands[id])
        {
          case CompositeOperatorsCommand:
          {
            char
              command[MaxTextExtent];

            static const char
              *OperatorMenu[]=
              {
                "Over",
                "In",
                "Out",
                "Atop",
                "Xor",
                "Plus",
                "Minus",
                "Add",
                "Subtract",
                "Difference",
                "Multiply",
                "Bumpmap",
                "Copy",
                "CopyRed",
                "CopyGreen",
                "CopyBlue",
                "CopyOpacity",
                "Clear",
                (char *) NULL,
              };

            /*
              Select a command from the pop-up menu.
            */
            compose=(CompositeOperator) (MagickXMenuWidget(display,windows,
              CompositeMenu[id],OperatorMenu,command)+1);
            break;
          }
          case CompositeDissolveCommand:
          {
            static char
              factor[MaxTextExtent] = "20.0";

            /*
              Dissolve the two images a given percent.
            */
            (void) XSetFunction(display,windows->image.highlight_context,
              GXcopy);
            (void) MagickXDialogWidget(display,windows,"Dissolve",
              "Enter the blend factor (0.0 - 99.9%):",factor);
            (void) XSetFunction(display,windows->image.highlight_context,
              GXinvert);
            if (*factor == '\0')
              break;
            blend=atof(factor);
            compose=DissolveCompositeOp;
            break;
          }
          case CompositeDisplaceCommand:
          {
            /*
              Get horizontal and vertical scale displacement geometry.
            */
            (void) XSetFunction(display,windows->image.highlight_context,
              GXcopy);
            (void) MagickXDialogWidget(display,windows,"Displace",
              "Enter the horizontal and vertical scale:",displacement_geometry);
            (void) XSetFunction(display,windows->image.highlight_context,
              GXinvert);
            if (*displacement_geometry == '\0')
              break;
            compose=DisplaceCompositeOp;
            break;
          }
          case CompositeHelpCommand:
          {
            (void) XSetFunction(display,windows->image.highlight_context,
              GXcopy);
            MagickXTextViewWidget(display,resource_info,windows,False,
              "Help Viewer - Image Composite",ImageCompositeHelp);
            (void) XSetFunction(display,windows->image.highlight_context,
              GXinvert);
            break;
          }
          case CompositeDismissCommand:
          {
            /*
              Prematurely exit.
            */
            state|=EscapeState;
            state|=ExitState;
            break;
          }
          default:
            break;
        }
        continue;
      }
    switch (event.type)
    {
      case ButtonPress:
      {
        if (IsEventLogging())
          (void) LogMagickEvent(X11Event,GetMagickModule(),
            "Button Press: 0x%lx %u +%d+%d",event.xbutton.window,
            event.xbutton.button,event.xbutton.x,event.xbutton.y);
        if (event.xbutton.button != Button1)
          break;
        if (event.xbutton.window != windows->image.id)
          break;
        /*
          Change cursor.
        */
        composite_info.width=composite_image->columns;
        composite_info.height=composite_image->rows;
        (void) XDefineCursor(display,windows->image.id,cursor);
        composite_info.x=windows->image.x+event.xbutton.x;
        composite_info.y=windows->image.y+event.xbutton.y;
        break;
      }
      case ButtonRelease:
      {
        if (IsEventLogging())
          (void) LogMagickEvent(X11Event,GetMagickModule(),
            "Button Release: 0x%lx %u +%d+%d",event.xbutton.window,
            event.xbutton.button,event.xbutton.x,event.xbutton.y);
        if (event.xbutton.button != Button1)
          break;
        if (event.xbutton.window != windows->image.id)
          break;
        if ((composite_info.width != 0) && (composite_info.height != 0))
          {
            /*
              User has selected the location of the composite image.
            */
            composite_info.x=windows->image.x+event.xbutton.x;
            composite_info.y=windows->image.y+event.xbutton.y;
            state|=ExitState;
          }
        break;
      }
      case Expose:
        break;
      case KeyPress:
      {
        char
          command[MaxTextExtent];

        KeySym
          key_symbol;

        int
          length;

        if (event.xkey.window != windows->image.id)
          break;
        /*
          Respond to a user key press.
        */
        length=XLookupString((XKeyEvent *) &event.xkey,command,sizeof(command),
          &key_symbol,(XComposeStatus *) NULL);
        *(command+length)='\0';
        if (IsEventLogging())
          (void) LogMagickEvent(X11Event,GetMagickModule(),
            "Key press: 0x%lx (%.1024s)",key_symbol,command);
        switch ((int) key_symbol)
        {
          case XK_Escape:
          case XK_F20:
          {
            /*
              Prematurely exit.
            */
            DestroyImage(composite_image);
            state|=EscapeState;
            state|=ExitState;
            break;
          }
          case XK_F1:
          case XK_Help:
          {
            (void) XSetFunction(display,windows->image.highlight_context,
              GXcopy);
            MagickXTextViewWidget(display,resource_info,windows,False,
              "Help Viewer - Image Composite",ImageCompositeHelp);
            (void) XSetFunction(display,windows->image.highlight_context,
              GXinvert);
            break;
          }
          default:
          {
            (void) XBell(display,0);
            break;
          }
        }
        break;
      }
      case MotionNotify:
      {
        /*
          Map and unmap Info widget as text cursor crosses its boundaries.
        */
        x=event.xmotion.x;
        y=event.xmotion.y;
        if (windows->info.mapped)
          {
            if ((x < (int) (windows->info.x+windows->info.width)) &&
                (y < (int) (windows->info.y+windows->info.height)))
              (void) XWithdrawWindow(display,windows->info.id,
                windows->info.screen);
          }
        else
          if ((x > (int) (windows->info.x+windows->info.width)) ||
              (y > (int) (windows->info.y+windows->info.height)))
            (void) XMapWindow(display,windows->info.id);
        composite_info.x=windows->image.x+x;
        composite_info.y=windows->image.y+y;
        break;
      }
      default:
      {
        if (IsEventLogging())
          (void) LogMagickEvent(X11Event,GetMagickModule(),"Event type: %d",
            event.type);
        break;
      }
    }
  } while (!(state & ExitState));
  (void) XSelectInput(display,windows->image.id,
    windows->image.attributes.event_mask);
  (void) XSetFunction(display,windows->image.highlight_context,GXcopy);
  MagickXSetCursorState(display,windows,False);
  (void) XFreeCursor(display,cursor);
  if (state & EscapeState)
    return(True);
  /*
    Image compositing is relative to image configuration.
  */
  MagickXSetCursorState(display,windows,True);
  MagickXCheckRefreshWindows(display,windows);
  width=(unsigned int) image->columns;
  height=(unsigned int) image->rows;
  x=0;
  y=0;
  if (windows->image.crop_geometry != (char *) NULL)
    (void) XParseGeometry(windows->image.crop_geometry,&x,&y,&width,&height);
  scale_factor=(double) width/windows->image.ximage->width;
  composite_info.x+=x;
  composite_info.x=(int) (scale_factor*composite_info.x+0.5);
  composite_info.width=(unsigned int) (scale_factor*composite_info.width+0.5);
  scale_factor=(double) height/windows->image.ximage->height;
  composite_info.y+=y;
  composite_info.y=(int) (scale_factor*composite_info.y+0.5);
  composite_info.height=(unsigned int) (scale_factor*composite_info.height+0.5);
  if ((composite_info.width != composite_image->columns) ||
      (composite_info.height != composite_image->rows))
    {
      Image
        *resize_image;

      /*
        Scale composite image.
      */
      resize_image=ZoomImage(composite_image,composite_info.width,
        composite_info.height,&image->exception);
      DestroyImage(composite_image);
      if (resize_image == (Image *) NULL)
        {
          MagickXSetCursorState(display,windows,False);
          return(False);
        }
      composite_image=resize_image;
    }
  if (compose == DisplaceCompositeOp)
    composite_image->geometry=displacement_geometry;
  if (blend != 0.0)
    {
      int
        y;

      Quantum
        opacity;

      register int
        x;

      register PixelPacket
        *q;

      /*
        Create mattes for blending.
      */
      SetImageOpacity(composite_image,OpaqueOpacity);
      opacity=(Quantum) (ScaleQuantumToChar(MaxRGB)-
        ((long) ScaleQuantumToChar(MaxRGB)*blend)/100);
      (void) SetImageType(image,TrueColorMatteType);
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (int) image->columns; x++)
        {
          q->opacity=opacity;
          q++;
        }
        if (!SyncImagePixels(image))
          break;
      }
    }
  /*
    Composite image with X Image window.
  */
  (void) CompositeImage(image,compose,composite_image,composite_info.x,
    composite_info.y);
  DestroyImage(composite_image);
  MagickXSetCursorState(display,windows,False);
  /*
    Update image configuration.
  */
  MagickXConfigureImageColormap(display,resource_info,windows,image);
  (void) MagickXConfigureImage(display,resource_info,windows,image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k X C o n f i g u r e I m a g e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickXConfigureImage creates a new X image.  It also notifies the
%  window manager of the new image size and configures the transient widows.
%
%  The format of the MagickXConfigureImage method is:
%
%      unsigned int MagickXConfigureImage(Display *display,
%        MagickXResourceInfo *resource_info,MagickXWindows *windows,Image *image)
%
%  A description of each parameter follows:
%
%    o status: Method MagickXConfigureImage returns True if the window is
%      resized.  False is returned is there is a memory shortage or if the
%      window fails to resize.
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 MagickXResourceInfo structure.
%
%    o windows: Specifies a pointer to a MagickXWindows structure.
%
%    o image: Specifies a pointer to an Image structure;  returned from
%      ReadImage.
%
%
*/
static unsigned int MagickXConfigureImage(Display *display,
  MagickXResourceInfo *resource_info,MagickXWindows *windows,Image *image)
{
  char
    geometry[MaxTextExtent];

  long
    x,
    y;

  unsigned int
    mask,
    status;

  unsigned long
    height,
    width;

  XSizeHints
    *size_hints;

  XWindowChanges
    window_changes;

  /*
    Dismiss if window dimensions are zero.
  */
  width=windows->image.window_changes.width;
  height=windows->image.window_changes.height;
  if (IsEventLogging())
    (void) LogMagickEvent(X11Event,GetMagickModule(),
      "Configure Image: %dx%d=>%lux%lu",windows->image.ximage->width,
      windows->image.ximage->height,width,height);
  if ((width*height) == 0)
    return(True);
  x=0;
  y=0;
  /*
    Display hourglass cursor if progress indication enabled.
  */
  if (resource_info->image_info->progress)
    MagickXSetCursorState(display,windows,True);
  /*
    Resize image to fit Image window dimensions.
  */
  (void) XFlush(display);
  if (((int) width != windows->image.ximage->width) ||
      ((int) height != windows->image.ximage->height))
    image->taint=True;
  windows->magnify.x=(unsigned int)
    width*windows->magnify.x/windows->image.ximage->width;
  windows->magnify.y=(unsigned int)
    height*windows->magnify.y/windows->image.ximage->height;
  windows->image.x=(int) (width*windows->image.x/windows->image.ximage->width);
  windows->image.y=(int)
    (height*windows->image.y/windows->image.ximage->height);
  status=MagickXMakeImage(display,resource_info,&windows->image,image,
    (unsigned int) width,(unsigned int) height);
  if (status == False)
    MagickXNoticeWidget(display,windows,"Unable to configure X image:",
      windows->image.name);
  /*
    Notify window manager of the new configuration.
  */
  FormatString(geometry,"%ux%u+0+0>!",
    XDisplayWidth(display,windows->image.screen),
    XDisplayHeight(display,windows->image.screen));
  (void) GetMagickGeometry(geometry,&x,&y,&width,&height);
  window_changes.width=(unsigned int) width;
  window_changes.height=(unsigned int) height;
  mask=CWWidth | CWHeight;
  if (resource_info->backdrop)
    {
      mask|=CWX | CWY;
      window_changes.x=(int)
        ((XDisplayWidth(display,windows->image.screen)/2)-(width/2));
      window_changes.y=(int)
        ((XDisplayHeight(display,windows->image.screen)/2)-(height/2));
    }
  (void) XReconfigureWMWindow(display,windows->image.id,windows->image.screen,
    mask,&window_changes);
  (void) XClearWindow(display,windows->image.id);
  MagickXRefreshWindow(display,&windows->image,(XEvent *) NULL);
  /*
    Update Magnify window configuration.
  */
  if (windows->magnify.mapped)
    MagickXMakeMagnifyImage(display,windows);
  /*
    Update pan window configuration.
  */
  windows->pan.crop_geometry=windows->image.crop_geometry;
  MagickXBestIconSize(display,&windows->pan,image);
  while ((windows->pan.width < MaxIconSize) &&
         (windows->pan.height < MaxIconSize))
  {
    windows->pan.width<<=1;
    windows->pan.height<<=1;
  }
  if (windows->pan.geometry != (char *) NULL)
    (void) XParseGeometry(windows->pan.geometry,&windows->pan.x,&windows->pan.y,
      &windows->pan.width,&windows->pan.height);
  window_changes.width=windows->pan.width;
  window_changes.height=windows->pan.height;
  size_hints=XAllocSizeHints();
  if (size_hints != (XSizeHints *) NULL)
    {
      /*
        Set new size hints.
      */
      size_hints->flags=PSize | PMinSize | PMaxSize;
      size_hints->width=window_changes.width;
      size_hints->height=window_changes.height;
      size_hints->min_width=size_hints->width;
      size_hints->min_height=size_hints->height;
      size_hints->max_width=size_hints->width;
      size_hints->max_height=size_hints->height;
      (void) XSetNormalHints(display,windows->pan.id,size_hints);
      (void) XFree((void *) size_hints);
    }
  (void) XReconfigureWMWindow(display,windows->pan.id,windows->pan.screen,
    CWWidth | CWHeight,&window_changes);
  if (windows->pan.mapped)
    MagickXMakePanImage(display,resource_info,windows,image);
  /*
    Update icon window configuration.
  */
  windows->icon.crop_geometry=windows->image.crop_geometry;
  MagickXBestIconSize(display,&windows->icon,image);
  window_changes.width=windows->icon.width;
  window_changes.height=windows->icon.height;
  (void) XReconfigureWMWindow(display,windows->icon.id,windows->icon.screen,
    CWWidth | CWHeight,&window_changes);
  MagickXSetCursorState(display,windows,False);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k X C r o p I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickXCropImage allows the user to select a region of the image and
%  crop, copy, or cut it.  For copy or cut, the image can subsequently be
%  composited onto the image with MagickXPasteImage.
%
%  The format of the MagickXCropImage method is:
%
%      unsigned int MagickXCropImage(Display *display,MagickXResourceInfo *resource_info,
%        MagickXWindows *windows,Image *image,const ClipboardMode mode)
%
%  A description of each parameter follows:
%
%    o status: Method MagickXCropImage returns True if the image is
%      copied.  False is returned is there is a memory shortage or if the
%      image fails to be copied.
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 MagickXResourceInfo structure.
%
%    o windows: Specifies a pointer to a MagickXWindows structure.
%
%    o image: Specifies a pointer to an Image structure; returned from
%      ReadImage.
%
%    o mode: This unsigned value specified whether the image should be
%      cropped, copied, or cut.
%
%
*/
static unsigned int MagickXCropImage(Display *display,MagickXResourceInfo *resource_info,
  MagickXWindows *windows,Image *image,const ClipboardMode mode)
{
  static const char
    *CropModeMenu[]=
    {
      "Help",
      "Dismiss",
      (char *) NULL
    },
    *RectifyModeMenu[]=
    {
      "Crop",
      "Help",
      "Dismiss",
      (char *) NULL
    };

  static const ModeType
    CropCommands[]=
    {
      CropHelpCommand,
      CropDismissCommand
    },
    RectifyCommands[]=
    {
      RectifyCopyCommand,
      RectifyHelpCommand,
      RectifyDismissCommand
    };

  char
    command[MaxTextExtent],
    text[MaxTextExtent];

  Cursor
    cursor;

  double
    scale_factor;

  int
    id,
    x,
    y;

  KeySym
    key_symbol;

  Image
    *crop_image;

  RectangleInfo
    crop_info,
    highlight_info;

  register PixelPacket
    *q;

  unsigned int
    height,
    width;

  unsigned long
    state;

  XEvent
    event;

  /*
    Map Command widget.
  */
  switch (mode)
  {
    case CopyMode:
    {
      (void) CloneString(&windows->command.name,"Copy");
      break;
    }
    case CropMode:
    {
      (void) CloneString(&windows->command.name,"Crop");
      break;
    }
    case CutMode:
    {
      (void) CloneString(&windows->command.name,"Cut");
      break;
    }
  }
  RectifyModeMenu[0]=windows->command.name;
  windows->command.data=0;
  (void) MagickXCommandWidget(display,windows,CropModeMenu,(XEvent *) NULL);
  (void) XMapRaised(display,windows->command.id);
  MagickXClientMessage(display,windows->image.id,windows->im_protocols,
    windows->im_update_widget,CurrentTime);
  /*
    Track pointer until button 1 is pressed.
  */
  MagickXQueryPosition(display,windows->image.id,&x,&y);
  (void) XSelectInput(display,windows->image.id,
    windows->image.attributes.event_mask | PointerMotionMask);
  crop_info.x=windows->image.x+x;
  crop_info.y=windows->image.y+y;
  crop_info.width=0;
  crop_info.height=0;
  cursor=XCreateFontCursor(display,XC_fleur);
  state=DefaultState;
  do
  {
    if (windows->info.mapped)
      {
        /*
          Display pointer position.
        */
        FormatString(text," %+ld%+ld ",crop_info.x,crop_info.y);
        MagickXInfoWidget(display,windows,text);
      }
    /*
      Wait for next event.
    */
    MagickXScreenEvent(display,windows,&event);
    if (event.xany.window == windows->command.id)
      {
        /*
          Select a command from the Command widget.
        */
        id=MagickXCommandWidget(display,windows,CropModeMenu,&event);
        if (id < 0)
          continue;
        switch (CropCommands[id])
        {
          case CropHelpCommand:
          {
            switch (mode)
            {
              case CopyMode:
              {
                MagickXTextViewWidget(display,resource_info,windows,False,
                  "Help Viewer - Image Copy",ImageCopyHelp);
                break;
              }
              case CropMode:
              {
                MagickXTextViewWidget(display,resource_info,windows,False,
                  "Help Viewer - Image Crop",ImageCropHelp);
                break;
              }
              case CutMode:
              {
                MagickXTextViewWidget(display,resource_info,windows,False,
                  "Help Viewer - Image Cut",ImageCutHelp);
                break;
              }
            }
            break;
          }
          case CropDismissCommand:
          {
            /*
              Prematurely exit.
            */
            state|=EscapeState;
            state|=ExitState;
            break;
          }
          default:
            break;
        }
        continue;
      }
    switch (event.type)
    {
      case ButtonPress:
      {
        if (event.xbutton.button != Button1)
          break;
        if (event.xbutton.window != windows->image.id)
          break;
        /*
          Note first corner of cropping rectangle-- exit loop.
        */
        (void) XDefineCursor(display,windows->image.id,cursor);
        crop_info.x=windows->image.x+event.xbutton.x;
        crop_info.y=windows->image.y+event.xbutton.y;
        state|=ExitState;
        break;
      }
      case ButtonRelease:
        break;
      case Expose:
        break;
      case KeyPress:
      {
        if (event.xkey.window != windows->image.id)
          break;
        /*
          Respond to a user key press.
        */
        (void) XLookupString((XKeyEvent *) &event.xkey,command,sizeof(command),
          &key_symbol,(XComposeStatus *) NULL);
        switch ((int) key_symbol)
        {
          case XK_Escape:
          case XK_F20:
          {
            /*
              Prematurely exit.
            */
            state|=EscapeState;
            state|=ExitState;
            break;
          }
          case XK_F1:
          case XK_Help:
          {
            switch (mode)
            {
              case CopyMode:
              {
                MagickXTextViewWidget(display,resource_info,windows,False,
                  "Help Viewer - Image Copy",ImageCopyHelp);
                break;
              }
              case CropMode:
              {
                MagickXTextViewWidget(display,resource_info,windows,False,
                  "Help Viewer - Image Crop",ImageCropHelp);
                break;
              }
              case CutMode:
              {
                MagickXTextViewWidget(display,resource_info,windows,False,
                  "Help Viewer - Image Cut",ImageCutHelp);
                break;
              }
            }
            break;
          }
          default:
          {
            (void) XBell(display,0);
            break;
          }
        }
        break;
      }
      case MotionNotify:
      {
        /*
          Map and unmap Info widget as text cursor crosses its boundaries.
        */
        x=event.xmotion.x;
        y=event.xmotion.y;
        if (windows->info.mapped)
          {
            if ((x < (int) (windows->info.x+windows->info.width)) &&
                (y < (int) (windows->info.y+windows->info.height)))
              (void) XWithdrawWindow(display,windows->info.id,
                windows->info.screen);
          }
        else
          if ((x > (int) (windows->info.x+windows->info.width)) ||
              (y > (int) (windows->info.y+windows->info.height)))
            (void) XMapWindow(display,windows->info.id);
        crop_info.x=windows->image.x+x;
        crop_info.y=windows->image.y+y;
        break;
      }
      default:
        break;
    }
  } while (!(state & ExitState));
  (void) XSelectInput(display,windows->image.id,
    windows->image.attributes.event_mask);
  if (state & EscapeState)
    {
      /*
        User want to exit without cropping.
      */
      (void) XWithdrawWindow(display,windows->info.id,windows->info.screen);
      (void) XFreeCursor(display,cursor);
      return(True);
    }
  (void) XSetFunction(display,windows->image.highlight_context,GXinvert);
  do
  {
    /*
      Size rectangle as pointer moves until the mouse button is released.
    */
    x=(int) crop_info.x;
    y=(int) crop_info.y;
    crop_info.width=0;
    crop_info.height=0;
    state=DefaultState;
    do
    {
      highlight_info=crop_info;
      highlight_info.x=crop_info.x-windows->image.x;
      highlight_info.y=crop_info.y-windows->image.y;
      if ((highlight_info.width > 3) && (highlight_info.height > 3))
        {
          /*
            Display info and draw cropping rectangle.
          */
          if (!windows->info.mapped)
            (void) XMapWindow(display,windows->info.id);
          FormatString(text," %lux%lu%+ld%+ld",crop_info.width,crop_info.height,
            crop_info.x,crop_info.y);
          MagickXInfoWidget(display,windows,text);
          MagickXHighlightRectangle(display,windows->image.id,
            windows->image.highlight_context,&highlight_info);
        }
      else
        if (windows->info.mapped)
          (void) XWithdrawWindow(display,windows->info.id,windows->info.screen);
      /*
        Wait for next event.
      */
      MagickXScreenEvent(display,windows,&event);
      if ((highlight_info.width > 3) && (highlight_info.height > 3))
        MagickXHighlightRectangle(display,windows->image.id,
          windows->image.highlight_context,&highlight_info);
      switch (event.type)
      {
        case ButtonPress:
        {
          crop_info.x=windows->image.x+event.xbutton.x;
          crop_info.y=windows->image.y+event.xbutton.y;
          break;
        }
        case ButtonRelease:
        {
          /*
            User has committed to cropping rectangle.
          */
          crop_info.x=windows->image.x+event.xbutton.x;
          crop_info.y=windows->image.y+event.xbutton.y;
          MagickXSetCursorState(display,windows,False);
          state|=ExitState;
          if (LocaleCompare(windows->command.name,"Rectify") == 0)
            break;
          (void) CloneString(&windows->command.name,"Rectify");
          windows->command.data=0;
          (void) MagickXCommandWidget(display,windows,RectifyModeMenu,
            (XEvent *) NULL);
          break;
        }
        case Expose:
          break;
        case MotionNotify:
        {
          crop_info.x=windows->image.x+event.xmotion.x;
          crop_info.y=windows->image.y+event.xmotion.y;
        }
        default:
          break;
      }
      if ((((int) crop_info.x != x) && ((int) crop_info.y != y)) ||
          (state & ExitState))
        {
          /*
            Check boundary conditions.
          */
          if (crop_info.x < 0)
            crop_info.x=0;
          else
            if (crop_info.x > (int) windows->image.ximage->width)
              crop_info.x=windows->image.ximage->width;
          if ((int) crop_info.x < x)
            crop_info.width=(unsigned int) (x-crop_info.x);
          else
            {
              crop_info.width=(unsigned int) (crop_info.x-x);
              crop_info.x=x;
            }
          if (crop_info.y < 0)
            crop_info.y=0;
          else
            if (crop_info.y > (int) windows->image.ximage->height)
              crop_info.y=windows->image.ximage->height;
          if ((int) crop_info.y < y)
            crop_info.height=(unsigned int) (y-crop_info.y);
          else
            {
              crop_info.height=(unsigned int) (crop_info.y-y);
              crop_info.y=y;
            }
        }
    } while (!(state & ExitState));
    /*
      Wait for user to grab a corner of the rectangle or press return.
    */
    state=DefaultState;
    do
    {
      if (windows->info.mapped)
        {
          /*
            Display pointer position.
          */
          FormatString(text," %lux%lu%+ld%+ld",crop_info.width,crop_info.height,
            crop_info.x,crop_info.y);
          MagickXInfoWidget(display,windows,text);
        }
      highlight_info=crop_info;
      highlight_info.x=crop_info.x-windows->image.x;
      highlight_info.y=crop_info.y-windows->image.y;
      if ((highlight_info.width <= 3) || (highlight_info.height <= 3))
        {
          state|=EscapeState;
          state|=ExitState;
          break;
        }
      MagickXHighlightRectangle(display,windows->image.id,
        windows->image.highlight_context,&highlight_info);
      MagickXScreenEvent(display,windows,&event);
      if (event.xany.window == windows->command.id)
        {
          /*
            Select a command from the Command widget.
          */
          (void) XSetFunction(display,windows->image.highlight_context,GXcopy);
          id=MagickXCommandWidget(display,windows,RectifyModeMenu,&event);
          (void) XSetFunction(display,windows->image.highlight_context,
            GXinvert);
          MagickXHighlightRectangle(display,windows->image.id,
            windows->image.highlight_context,&highlight_info);
          if (id >= 0)
            switch (RectifyCommands[id])
            {
              case RectifyCopyCommand:
              {
                state|=ExitState;
                break;
              }
              case RectifyHelpCommand:
              {
                (void) XSetFunction(display,windows->image.highlight_context,
                  GXcopy);
                switch (mode)
                {
                  case CopyMode:
                  {
                    MagickXTextViewWidget(display,resource_info,windows,False,
                      "Help Viewer - Image Copy",ImageCopyHelp);
                    break;
                  }
                  case CropMode:
                  {
                    MagickXTextViewWidget(display,resource_info,windows,False,
                      "Help Viewer - Image Crop",ImageCropHelp);
                    break;
                  }
                  case CutMode:
                  {
                    MagickXTextViewWidget(display,resource_info,windows,False,
                      "Help Viewer - Image Cut",ImageCutHelp);
                    break;
                  }
                }
                (void) XSetFunction(display,windows->image.highlight_context,
                  GXinvert);
                break;
              }
              case RectifyDismissCommand:
              {
                /*
                  Prematurely exit.
                */
                state|=EscapeState;
                state|=ExitState;
                break;
              }
              default:
                break;
            }
          continue;
        }
      MagickXHighlightRectangle(display,windows->image.id,
        windows->image.highlight_context,&highlight_info);
      switch (event.type)
      {
        case ButtonPress:
        {
          if (event.xbutton.button != Button1)
            break;
          if (event.xbutton.window != windows->image.id)
            break;
          x=windows->image.x+event.xbutton.x;
          y=windows->image.y+event.xbutton.y;
          if ((x < (int) (crop_info.x+RoiDelta)) &&
              (x > (int) (crop_info.x-RoiDelta)) &&
              (y < (int) (crop_info.y+RoiDelta)) &&
              (y > (int) (crop_info.y-RoiDelta)))
            {
              crop_info.x=(long) (crop_info.x+crop_info.width);
              crop_info.y=(long) (crop_info.y+crop_info.height);
              state|=UpdateConfigurationState;
              break;
            }
          if ((x < (int) (crop_info.x+RoiDelta)) &&
              (x > (int) (crop_info.x-RoiDelta)) &&
              (y < (int) (crop_info.y+crop_info.height+RoiDelta)) &&
              (y > (int) (crop_info.y+crop_info.height-RoiDelta)))
            {
              crop_info.x=(long) (crop_info.x+crop_info.width);
              state|=UpdateConfigurationState;
              break;
            }
          if ((x < (int) (crop_info.x+crop_info.width+RoiDelta)) &&
              (x > (int) (crop_info.x+crop_info.width-RoiDelta)) &&
              (y < (int) (crop_info.y+RoiDelta)) &&
              (y > (int) (crop_info.y-RoiDelta)))
            {
              crop_info.y=(long) (crop_info.y+crop_info.height);
              state|=UpdateConfigurationState;
              break;
            }
          if ((x < (int) (crop_info.x+crop_info.width+RoiDelta)) &&
              (x > (int) (crop_info.x+crop_info.width-RoiDelta)) &&
              (y < (int) (crop_info.y+crop_info.height+RoiDelta)) &&
              (y > (int) (crop_info.y+crop_info.height-RoiDelta)))
            {
              state|=UpdateConfigurationState;
              break;
            }
        }
        case ButtonRelease:
        {
          if (event.xbutton.window == windows->pan.id)
            if ((highlight_info.x != crop_info.x-windows->image.x) ||
                (highlight_info.y != crop_info.y-windows->image.y))
              MagickXHighlightRectangle(display,windows->image.id,
                windows->image.highlight_context,&highlight_info);
          break;
        }
        case Expose:
        {
          if (event.xexpose.window == windows->image.id)
            if (event.xexpose.count == 0)
              {
                event.xexpose.x=(int) highlight_info.x;
                event.xexpose.y=(int) highlight_info.y;
                event.xexpose.width=(unsigned int) highlight_info.width;
                event.xexpose.height=(unsigned int) highlight_info.height;
                MagickXRefreshWindow(display,&windows->image,&event);
              }
          if (event.xexpose.window == windows->info.id)
            if (event.xexpose.count == 0)
              MagickXInfoWidget(display,windows,text);
          break;
        }
        case KeyPress:
        {
          if (event.xkey.window != windows->image.id)
            break;
          /*
            Respond to a user key press.
          */
          (void) XLookupString((XKeyEvent *) &event.xkey,command,
            sizeof(command),&key_symbol,(XComposeStatus *) NULL);
          switch ((int) key_symbol)
          {
            case XK_Escape:
            case XK_F20:
              state|=EscapeState;
            case XK_Return:
            {
              state|=ExitState;
              break;
            }
            case XK_F1:
            case XK_Help:
            {
              (void) XSetFunction(display,windows->image.highlight_context,
                GXcopy);
              switch (mode)
              {
                case CopyMode:
                {
                  MagickXTextViewWidget(display,resource_info,windows,False,
                    "Help Viewer - Image Copy",ImageCopyHelp);
                  break;
                }
                case CropMode:
                {
                  MagickXTextViewWidget(display,resource_info,windows,False,
                    "Help Viewer - Image Cropg",ImageCropHelp);
                  break;
                }
                case CutMode:
                {
                  MagickXTextViewWidget(display,resource_info,windows,False,
                    "Help Viewer - Image Cutg",ImageCutHelp);
                  break;
                }
              }
              (void) XSetFunction(display,windows->image.highlight_context,
                GXinvert);
              break;
            }
            default:
            {
              (void) XBell(display,0);
              break;
            }
          }
          break;
        }
        case KeyRelease:
          break;
        case MotionNotify:
        {
          /*
            Map and unmap Info widget as text cursor crosses its boundaries.
          */
          x=event.xmotion.x;
          y=event.xmotion.y;
          if (windows->info.mapped)
            {
              if ((x < (int) (windows->info.x+windows->info.width)) &&
                  (y < (int) (windows->info.y+windows->info.height)))
                (void) XWithdrawWindow(display,windows->info.id,
                  windows->info.screen);
            }
          else
            if ((x > (int) (windows->info.x+windows->info.width)) ||
                (y > (int) (windows->info.y+windows->info.height)))
              (void) XMapWindow(display,windows->info.id);
          break;
        }
        default:
          break;
      }
      if (state & UpdateConfigurationState)
        {
          (void) XPutBackEvent(display,&event);
          (void) XDefineCursor(display,windows->image.id,cursor);
          break;
        }
    } while (!(state & ExitState));
  } while (!(state & ExitState));
  (void) XSetFunction(display,windows->image.highlight_context,GXcopy);
  MagickXSetCursorState(display,windows,False);
  if (state & EscapeState)
    return(True);
  if (mode == CropMode)
    if (((int) crop_info.width != windows->image.ximage->width) ||
        ((int) crop_info.height != windows->image.ximage->height))
      {
        /*
          Reconfigure Image window as defined by cropping rectangle.
        */
        MagickXSetCropGeometry(display,windows,&crop_info,image);
        windows->image.window_changes.width=(unsigned int) crop_info.width;
        windows->image.window_changes.height=(unsigned int) crop_info.height;
        (void) MagickXConfigureImage(display,resource_info,windows,image);
        return(True);
      }
  /*
    Copy image before applying image transforms.
  */
  MagickXSetCursorState(display,windows,True);
  MagickXCheckRefreshWindows(display,windows);
  width=(unsigned int) image->columns;
  height=(unsigned int) image->rows;
  x=0;
  y=0;
  if (windows->image.crop_geometry != (char *) NULL)
    (void) XParseGeometry(windows->image.crop_geometry,&x,&y,&width,&height);
  scale_factor=(double) width/windows->image.ximage->width;
  crop_info.x+=x;
  crop_info.x=(int) (scale_factor*crop_info.x+0.5);
  crop_info.width=(unsigned int) (scale_factor*crop_info.width+0.5);
  scale_factor=(double) height/windows->image.ximage->height;
  crop_info.y+=y;
  crop_info.y=(int) (scale_factor*crop_info.y+0.5);
  crop_info.height=(unsigned int) (scale_factor*crop_info.height+0.5);
  crop_image=CropImage(image,&crop_info,&image->exception);
  MagickXSetCursorState(display,windows,False);
  if (crop_image == (Image *) NULL)
    return(False);
  if (resource_info->copy_image != (Image *) NULL)
    DestroyImage(resource_info->copy_image);
  resource_info->copy_image=crop_image;
  if (mode == CopyMode)
    {
      (void) MagickXConfigureImage(display,resource_info,windows,image);
      return(True);
    }
  /*
    Cut image.
  */
  (void) SetImageType(image,TrueColorMatteType);
  for (y=0; y < (long) crop_info.height; y++)
  {
    q=GetImagePixels(image,crop_info.x,y+crop_info.y,crop_info.width,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (int) crop_info.width; x++)
    {
      q->opacity=TransparentOpacity;
      q++;
    }
    if (!SyncImagePixels(image))
      break;
  }
  /*
    Update image configuration.
  */
  MagickXConfigureImageColormap(display,resource_info,windows,image);
  (void) MagickXConfigureImage(display,resource_info,windows,image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k X D r a w I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickXDrawEditImage draws a graphic element (point, line, rectangle,
%  etc.) on the image.
%
%  The format of the MagickXDrawEditImage method is:
%
%      unsigned int MagickXDrawEditImage(Display *display,
%        MagickXResourceInfo *resource_info,MagickXWindows *windows,Image **image)
%
%  A description of each parameter follows:
%
%    o status: Method MagickXDrawEditImage return True if the image is drawn
%      upon.  False is returned is there is a memory shortage or if the
%      image cannot be drawn on.
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 MagickXResourceInfo structure.
%
%    o windows: Specifies a pointer to a MagickXWindows structure.
%
%    o image: Specifies a pointer to an Image structure;  returned from
%      ReadImage.
%
%
*/
static unsigned int MagickXDrawEditImage(Display *display,
  MagickXResourceInfo *resource_info,MagickXWindows *windows,Image **image)
{
  static const char
    *DrawMenu[]=
    {
      "Element",
      "Color",
      "Stipple",
      "Width",
      "Undo",
      "Help",
      "Dismiss",
      (char *) NULL
    };

  static ElementType
    element = PointElement;

  static const ModeType
    DrawCommands[]=
    {
      DrawElementCommand,
      DrawColorCommand,
      DrawStippleCommand,
      DrawWidthCommand,
      DrawUndoCommand,
      DrawHelpCommand,
      DrawDismissCommand
    };

  static Pixmap
    stipple = (Pixmap) NULL;

  static unsigned int
    pen_id = 0,
    line_width = 1;

  char
    command[MaxTextExtent],
    text[MaxTextExtent];

  Cursor
    cursor;

  double
    degrees;

  int
    entry,
    id,
    number_coordinates,
    x,
    y;

  RectangleInfo
    rectangle_info;

  register int
    i;

  unsigned int
    distance,
    height,
    max_coordinates,
    status,
    width;

  unsigned long
    state;

  Window
    root_window;

  MagickXDrawInfo
    draw_info;

  XEvent
    event;

  XPoint
    *coordinate_info;

  XSegment
    line_info;

  /*
    Allocate polygon info.
  */
  max_coordinates=2048;
  coordinate_info=MagickAllocateMemory(XPoint *,max_coordinates*sizeof(XPoint));
  if (coordinate_info == (XPoint *) NULL)
    {
      MagickError3(ResourceLimitError,MemoryAllocationFailed,
        UnableToDrawOnImage);
      return(False);
    }
  /*
    Map Command widget.
  */
  (void) CloneString(&windows->command.name,"Draw");
  windows->command.data=4;
  (void) MagickXCommandWidget(display,windows,DrawMenu,(XEvent *) NULL);
  (void) XMapRaised(display,windows->command.id);
  MagickXClientMessage(display,windows->image.id,windows->im_protocols,
    windows->im_update_widget,CurrentTime);
  /*
    Wait for first button press.
  */
  root_window=XRootWindow(display,XDefaultScreen(display));
  draw_info.stencil=OpaqueStencil;
  status=True;
  cursor=XCreateFontCursor(display,XC_tcross);
  for ( ; ; )
  {
    MagickXQueryPosition(display,windows->image.id,&x,&y);
    (void) XSelectInput(display,windows->image.id,
      windows->image.attributes.event_mask | PointerMotionMask);
    (void) XDefineCursor(display,windows->image.id,cursor);
    state=DefaultState;
    do
    {
      if (windows->info.mapped)
        {
          /*
            Display pointer position.
          */
          FormatString(text," %+d%+d ",x+windows->image.x,y+windows->image.y);
          MagickXInfoWidget(display,windows,text);
        }
      /*
        Wait for next event.
      */
      MagickXScreenEvent(display,windows,&event);
      if (event.xany.window == windows->command.id)
        {
          /*
            Select a command from the Command widget.
          */
          id=MagickXCommandWidget(display,windows,DrawMenu,&event);
          if (id < 0)
            continue;
          switch (DrawCommands[id])
          {
            case DrawElementCommand:
            {
              static const char
                *Elements[]=
                {
                  "point",
                  "line",
                  "rectangle",
                  "fill rectangle",
                  "circle",
                  "fill circle",
                  "ellipse",
                  "fill ellipse",
                  "polygon",
                  "fill polygon",
                  (char *) NULL,
                };

              /*
                Select a command from the pop-up menu.
              */
              element=(ElementType) (MagickXMenuWidget(display,windows,
                DrawMenu[id],Elements,command)+1);
              break;
            }
            case DrawColorCommand:
            {
              char
                *ColorMenu[MaxNumberPens+1];

              int
                pen_number;

              unsigned int
                transparent;

              XColor
                color;

              /*
                Initialize menu selections.
              */
              for (i=0; i < (int) (MaxNumberPens-2); i++)
                ColorMenu[i]=resource_info->pen_colors[i];
              ColorMenu[MaxNumberPens-2]=(char *) "transparent";
              ColorMenu[MaxNumberPens-1]=(char *) "Browser...";
              ColorMenu[MaxNumberPens]=(char *) NULL;
              /*
                Select a pen color from the pop-up menu.
              */
              pen_number=MagickXMenuWidget(display,windows,DrawMenu[id],
                (const char **) ColorMenu,command);
              if (pen_number < 0)
                break;
              transparent=pen_number == (MaxNumberPens-2);
              if (transparent)
                {
                  draw_info.stencil=TransparentStencil;
                  break;
                }
              if (pen_number == (MaxNumberPens-1))
                {
                  static char
                    color_name[MaxTextExtent] = "gray";

                  /*
                    Select a pen color from a dialog.
                  */
                  resource_info->pen_colors[pen_number]=color_name;
                  MagickXColorBrowserWidget(display,windows,"Select",color_name);
                  if (*color_name == '\0')
                    break;
                }
              /*
                Set pen color.
              */
              (void) XParseColor(display,windows->map_info->colormap,
                resource_info->pen_colors[pen_number],&color);
              MagickXBestPixel(display,windows->map_info->colormap,(XColor *) NULL,
                (unsigned int) MaxColors,&color);
              windows->pixel_info->pen_colors[pen_number]=color;
              pen_id=pen_number;
              draw_info.stencil=OpaqueStencil;
              break;
            }
            case DrawStippleCommand:
            {
              Image
                *stipple_image;

              ImageInfo
                *image_info;

              static char
                filename[MaxTextExtent] = "\0";

              static const char
                *StipplesMenu[]=
                {
                  "Brick",
                  "Diagonal",
                  "Scales",
                  "Vertical",
                  "Wavy",
                  "Translucent",
                  "Opaque",
                  (char *) NULL,
                  (char *) NULL,
                };

              /*
                Select a command from the pop-up menu.
              */
              StipplesMenu[7]=(char *) "Open...";
              entry=MagickXMenuWidget(display,windows,DrawMenu[id],StipplesMenu,
                command);
              if (entry < 0)
                break;
              if (stipple != (Pixmap) NULL)
                (void) XFreePixmap(display,stipple);
              stipple=(Pixmap) NULL;
              if (entry == 6)
                break;
              if (entry != 7)
                {
                  switch (entry)
                  {
                    case 0:
                    {
                      stipple=XCreateBitmapFromData(display,root_window,
                        (char *) BricksBitmap,BricksWidth,BricksHeight);
                      break;
                    }
                    case 1:
                    {
                      stipple=XCreateBitmapFromData(display,root_window,
                        (char *) DiagonalBitmap,DiagonalWidth,DiagonalHeight);
                      break;
                    }
                    case 2:
                    {
                      stipple=XCreateBitmapFromData(display,root_window,
                        (char *) ScalesBitmap,ScalesWidth,ScalesHeight);
                      break;
                    }
                    case 3:
                    {
                      stipple=XCreateBitmapFromData(display,root_window,
                        (char *) VerticalBitmap,VerticalWidth,VerticalHeight);
                      break;
                    }
                    case 4:
                    {
                      stipple=XCreateBitmapFromData(display,root_window,
                        (char *) WavyBitmap,WavyWidth,WavyHeight);
                      break;
                    }
                    case 5:
                    default:
                    {
                      stipple=XCreateBitmapFromData(display,root_window,
                        (char *) HighlightBitmap,HighlightWidth,
                        HighlightHeight);
                      break;
                    }
                  }
                  break;
                }
              MagickXFileBrowserWidget(display,windows,"Stipple",filename);
              if (*filename == '\0')
                break;
              /*
                Read image.
              */
              MagickXSetCursorState(display,windows,True);
              MagickXCheckRefreshWindows(display,windows);
              image_info=CloneImageInfo((ImageInfo *) NULL);
              (void) strlcpy(image_info->filename,filename,MaxTextExtent);
              stipple_image=ReadImage(image_info,&(*image)->exception);
              if ((*image)->exception.severity != UndefinedException)
                MagickError2((*image)->exception.severity,
                  (*image)->exception.reason,(*image)->exception.description);
              MagickXSetCursorState(display,windows,False);
              if (stipple_image == (Image *) NULL)
                break;
              if (!AcquireTemporaryFileName(filename))
                {
                  MagickXNoticeWidget(display,windows,
                    "Unable to open temporary file:",filename);
                  break;
                }
              FormatString(stipple_image->filename,"xbm:%.1024s",filename);
              (void) WriteImage(image_info,stipple_image);
              DestroyImage(stipple_image);
              DestroyImageInfo(image_info);
              status=XReadBitmapFile(display,root_window,filename,&width,
                &height,&stipple,&x,&y);
              (void) LiberateTemporaryFile(filename);
              if (status != BitmapSuccess)
                MagickXNoticeWidget(display,windows,"Unable to read X bitmap image:",
                  filename);
              break;
            }
            case DrawWidthCommand:
            {
              static char
                width[MaxTextExtent] = "0";

              static const char
                *WidthsMenu[]=
                {
                  "1",
                  "2",
                  "4",
                  "8",
                  "16",
                  "Dialog...",
                  (char *) NULL,
                };

              /*
                Select a command from the pop-up menu.
              */
              entry=MagickXMenuWidget(display,windows,DrawMenu[id],WidthsMenu,
                command);
              if (entry < 0)
                break;
              if (entry != 5)
                {
                  line_width=atoi(WidthsMenu[entry]);
                  break;
                }
              (void) MagickXDialogWidget(display,windows,"Ok","Enter line width:",
                width);
              if (*width == '\0')
                break;
              line_width=atoi(width);
              break;
            }
            case DrawUndoCommand:
            {
              (void) MagickXMagickCommand(display,resource_info,windows,UndoCommand,
                image);
              break;
            }
            case DrawHelpCommand:
            {
              MagickXTextViewWidget(display,resource_info,windows,False,
                "Help Viewer - Image Rotation",ImageDrawHelp);
              (void) XDefineCursor(display,windows->image.id,cursor);
              break;
            }
            case DrawDismissCommand:
            {
              /*
                Prematurely exit.
              */
              state|=EscapeState;
              state|=ExitState;
              break;
            }
            default:
              break;
          }
          (void) XDefineCursor(display,windows->image.id,cursor);
          continue;
        }
      switch (event.type)
      {
        case ButtonPress:
        {
          if (event.xbutton.button != Button1)
            break;
          if (event.xbutton.window != windows->image.id)
            break;
          /*
            Exit loop.
          */
          x=event.xbutton.x;
          y=event.xbutton.y;
          state|=ExitState;
          break;
        }
        case ButtonRelease:
          break;
        case Expose:
          break;
        case KeyPress:
        {
          KeySym
            key_symbol;

          if (event.xkey.window != windows->image.id)
            break;
          /*
            Respond to a user key press.
          */
          (void) XLookupString((XKeyEvent *) &event.xkey,command,
            sizeof(command),&key_symbol,(XComposeStatus *) NULL);
          switch ((int) key_symbol)
          {
            case XK_Escape:
            case XK_F20:
            {
              /*
                Prematurely exit.
              */
              state|=EscapeState;
              state|=ExitState;
              break;
            }
            case XK_F1:
            case XK_Help:
            {
              MagickXTextViewWidget(display,resource_info,windows,False,
                "Help Viewer - Image Rotation",ImageDrawHelp);
              break;
            }
            default:
            {
              (void) XBell(display,0);
              break;
            }
          }
          break;
        }
        case MotionNotify:
        {
          /*
            Map and unmap Info widget as text cursor crosses its boundaries.
          */
          x=event.xmotion.x;
          y=event.xmotion.y;
          if (windows->info.mapped)
            {
              if ((x < (int) (windows->info.x+windows->info.width)) &&
                  (y < (int) (windows->info.y+windows->info.height)))
                (void) XWithdrawWindow(display,windows->info.id,
                  windows->info.screen);
            }
          else
            if ((x > (int) (windows->info.x+windows->info.width)) ||
                (y > (int) (windows->info.y+windows->info.height)))
              (void) XMapWindow(display,windows->info.id);
          break;
        }
      }
    } while (!(state & ExitState));
    (void) XSelectInput(display,windows->image.id,
      windows->image.attributes.event_mask);
    (void) XWithdrawWindow(display,windows->info.id,windows->info.screen);
    if (state & EscapeState)
      break;
    /*
      Draw element as pointer moves until the button is released.
    */
    distance=0;
    degrees=0.0;
    line_info.x1=x;
    line_info.y1=y;
    line_info.x2=x;
    line_info.y2=y;
    rectangle_info.x=x;
    rectangle_info.y=y;
    rectangle_info.width=0;
    rectangle_info.height=0;
    number_coordinates=1;
    coordinate_info->x=x;
    coordinate_info->y=y;
    (void) XSetFunction(display,windows->image.highlight_context,GXinvert);
    state=DefaultState;
    do
    {
      switch (element)
      {
        case PointElement:
        default:
        {
          if (number_coordinates > 1)
            {
              (void) XDrawLines(display,windows->image.id,
                windows->image.highlight_context,coordinate_info,
                number_coordinates,CoordModeOrigin);
              FormatString(text," %+d%+d",
                coordinate_info[number_coordinates-1].x,
                coordinate_info[number_coordinates-1].y);
              MagickXInfoWidget(display,windows,text);
            }
          break;
        }
        case LineElement:
        {
          if (distance > 9)
            {
              /*
                Display angle of the line.
              */
              degrees=RadiansToDegrees(-atan2((double) (line_info.y2-
                line_info.y1),(double) (line_info.x2-line_info.x1)));
              FormatString(text," %.2f",degrees);
              MagickXInfoWidget(display,windows,text);
              MagickXHighlightLine(display,windows->image.id,
                windows->image.highlight_context,&line_info);
            }
          else
            if (windows->info.mapped)
              (void) XWithdrawWindow(display,windows->info.id,
                windows->info.screen);
          break;
        }
        case RectangleElement:
        case FillRectangleElement:
        {
          if ((rectangle_info.width > 3) && (rectangle_info.height > 3))
            {
              /*
                Display info and draw drawing rectangle.
              */
              FormatString(text," %lux%lu%+ld%+ld",rectangle_info.width,
                rectangle_info.height,rectangle_info.x,rectangle_info.y);
              MagickXInfoWidget(display,windows,text);
              MagickXHighlightRectangle(display,windows->image.id,
                windows->image.highlight_context,&rectangle_info);
            }
          else
            if (windows->info.mapped)
              (void) XWithdrawWindow(display,windows->info.id,
                windows->info.screen);
          break;
        }
        case CircleElement:
        case FillCircleElement:
        case EllipseElement:
        case FillEllipseElement:
        {
          if ((rectangle_info.width > 3) && (rectangle_info.height > 3))
            {
              /*
                Display info and draw drawing rectangle.
              */
              FormatString(text," %lux%lu%+ld%+ld",rectangle_info.width,
                rectangle_info.height,rectangle_info.x,rectangle_info.y);
              MagickXInfoWidget(display,windows,text);
              MagickXHighlightEllipse(display,windows->image.id,
                windows->image.highlight_context,&rectangle_info);
            }
          else
            if (windows->info.mapped)
              (void) XWithdrawWindow(display,windows->info.id,
                windows->info.screen);
          break;
        }
        case PolygonElement:
        case FillPolygonElement:
        {
          if (number_coordinates > 1)
            (void) XDrawLines(display,windows->image.id,
              windows->image.highlight_context,coordinate_info,
              number_coordinates,CoordModeOrigin);
          if (distance > 9)
            {
              /*
                Display angle of the line.
              */
              degrees=RadiansToDegrees(-atan2((double) (line_info.y2-
                line_info.y1),(double) (line_info.x2-line_info.x1)));
              FormatString(text," %.2f",degrees);
              MagickXInfoWidget(display,windows,text);
              MagickXHighlightLine(display,windows->image.id,
                windows->image.highlight_context,&line_info);
            }
          else
            if (windows->info.mapped)
              (void) XWithdrawWindow(display,windows->info.id,
                windows->info.screen);
          break;
        }
      }
      /*
        Wait for next event.
      */
      MagickXScreenEvent(display,windows,&event);
      switch (element)
      {
        case PointElement:
        default:
        {
          if (number_coordinates > 1)
            (void) XDrawLines(display,windows->image.id,
              windows->image.highlight_context,coordinate_info,
              number_coordinates,CoordModeOrigin);
          break;
        }
        case LineElement:
        {
          if (distance > 9)
            MagickXHighlightLine(display,windows->image.id,
              windows->image.highlight_context,&line_info);
          break;
        }
        case RectangleElement:
        case FillRectangleElement:
        {
          if ((rectangle_info.width > 3) && (rectangle_info.height > 3))
            MagickXHighlightRectangle(display,windows->image.id,
              windows->image.highlight_context,&rectangle_info);
          break;
        }
        case CircleElement:
        case FillCircleElement:
        case EllipseElement:
        case FillEllipseElement:
        {
          if ((rectangle_info.width > 3) && (rectangle_info.height > 3))
            MagickXHighlightEllipse(display,windows->image.id,
              windows->image.highlight_context,&rectangle_info);
          break;
        }
        case PolygonElement:
        case FillPolygonElement:
        {
          if (number_coordinates > 1)
            (void) XDrawLines(display,windows->image.id,
              windows->image.highlight_context,coordinate_info,
              number_coordinates,CoordModeOrigin);
          if (distance > 9)
            MagickXHighlightLine(display,windows->image.id,
              windows->image.highlight_context,&line_info);
          break;
        }
      }
      switch (event.type)
      {
        case ButtonPress:
          break;
        case ButtonRelease:
        {
          /*
            User has committed to element.
          */
          line_info.x2=event.xbutton.x;
          line_info.y2=event.xbutton.y;
          rectangle_info.x=event.xbutton.x;
          rectangle_info.y=event.xbutton.y;
          coordinate_info[number_coordinates].x=event.xbutton.x;
          coordinate_info[number_coordinates].y=event.xbutton.y;
          if (((element != PolygonElement) &&
               (element != FillPolygonElement)) || (distance <= 9))
            {
              state|=ExitState;
              break;
            }
          number_coordinates++;
          if (number_coordinates < (int) max_coordinates)
            {
              line_info.x1=event.xbutton.x;
              line_info.y1=event.xbutton.y;
              break;
            }
          max_coordinates<<=1;
          MagickReallocMemory(XPoint *,coordinate_info,
            max_coordinates*sizeof(XPoint));
          if (coordinate_info == (XPoint *) NULL)
            MagickError3(ResourceLimitError,MemoryAllocationFailed,
              UnableToDrawOnImage);
          break;
        }
        case Expose:
          break;
        case MotionNotify:
        {
          if (event.xmotion.window != windows->image.id)
            break;
          if (element != PointElement)
            {
              line_info.x2=event.xmotion.x;
              line_info.y2=event.xmotion.y;
              rectangle_info.x=event.xmotion.x;
              rectangle_info.y=event.xmotion.y;
              break;
            }
          coordinate_info[number_coordinates].x=event.xbutton.x;
          coordinate_info[number_coordinates].y=event.xbutton.y;
          number_coordinates++;
          if (number_coordinates < (int) max_coordinates)
            break;
          max_coordinates<<=1;
          MagickReallocMemory(XPoint *,coordinate_info,
            max_coordinates*sizeof(XPoint));
          if (coordinate_info == (XPoint *) NULL)
            MagickError3(ResourceLimitError,MemoryAllocationFailed,
              UnableToDrawOnImage);
          break;
        }
        default:
          break;
      }
      /*
        Check boundary conditions.
      */
      if (line_info.x2 < 0)
        line_info.x2=0;
      else
        if (line_info.x2 > (int) windows->image.width)
          line_info.x2=windows->image.width;
      if (line_info.y2 < 0)
        line_info.y2=0;
      else
        if (line_info.y2 > (int) windows->image.height)
          line_info.y2=windows->image.height;
      distance=
        ((line_info.x2-line_info.x1+1)*(line_info.x2-line_info.x1+1))+
        ((line_info.y2-line_info.y1+1)*(line_info.y2-line_info.y1+1));
      if ((((int) rectangle_info.x != x) && ((int) rectangle_info.y != y)) ||
          (state & ExitState))
        {
          if (rectangle_info.x < 0)
            rectangle_info.x=0;
          else
            if (rectangle_info.x > (int) windows->image.width)
              rectangle_info.x=(long) windows->image.width;
          if ((int) rectangle_info.x < x)
            rectangle_info.width=(unsigned int) (x-rectangle_info.x);
          else
            {
              rectangle_info.width=(unsigned int) (rectangle_info.x-x);
              rectangle_info.x=x;
            }
          if (rectangle_info.y < 0)
            rectangle_info.y=0;
          else
            if (rectangle_info.y > (int) windows->image.height)
              rectangle_info.y=(long) windows->image.height;
          if ((int) rectangle_info.y < y)
            rectangle_info.height=(unsigned int) (y-rectangle_info.y);
          else
            {
              rectangle_info.height=(unsigned int) (rectangle_info.y-y);
              rectangle_info.y=y;
            }
        }
    } while (!(state & ExitState));
    (void) XSetFunction(display,windows->image.highlight_context,GXcopy);
    if ((element == PointElement) || (element == PolygonElement) ||
        (element == FillPolygonElement))
      {
        /*
          Determine polygon bounding box.
        */
        rectangle_info.x=coordinate_info->x;
        rectangle_info.y=coordinate_info->y;
        x=coordinate_info->x;
        y=coordinate_info->y;
        for (i=1; i < number_coordinates; i++)
        {
          if (coordinate_info[i].x > x)
            x=coordinate_info[i].x;
          if (coordinate_info[i].y > y)
            y=coordinate_info[i].y;
          if (coordinate_info[i].x < rectangle_info.x)
            rectangle_info.x=Max(coordinate_info[i].x,0);
          if (coordinate_info[i].y < rectangle_info.y)
            rectangle_info.y=Max(coordinate_info[i].y,0);
        }
        rectangle_info.width=x-rectangle_info.x;
        rectangle_info.height=y-rectangle_info.y;
        for (i=0; i < number_coordinates; i++)
        {
          coordinate_info[i].x-=rectangle_info.x;
          coordinate_info[i].y-=rectangle_info.y;
        }
      }
    else
      if (distance <= 9)
        continue;
      else
        if ((element == RectangleElement) ||
            (element == CircleElement) || (element == EllipseElement))
          {
            rectangle_info.width--;
            rectangle_info.height--;
          }
    /*
      Drawing is relative to image configuration.
    */
    draw_info.x=(int) rectangle_info.x;
    draw_info.y=(int) rectangle_info.y;
    (void) MagickXMagickCommand(display,resource_info,windows,SaveToUndoBufferCommand,
      image);
    width=(unsigned int) (*image)->columns;
    height=(unsigned int) (*image)->rows;
    x=0;
    y=0;
    if (windows->image.crop_geometry != (char *) NULL)
      (void) XParseGeometry(windows->image.crop_geometry,&x,&y,&width,&height);
    draw_info.x+=windows->image.x-(line_width/2);
    if (draw_info.x < 0)
      draw_info.x=0;
    draw_info.x=width*draw_info.x/windows->image.ximage->width;
    draw_info.y+=windows->image.y-(line_width/2);
    if (draw_info.y < 0)
      draw_info.y=0;
    draw_info.y=height*draw_info.y/windows->image.ximage->height;
    draw_info.width=(unsigned int) rectangle_info.width+(line_width << 1);
    if (draw_info.width > (unsigned int) (*image)->columns)
      draw_info.width=(unsigned int) (*image)->columns;
    draw_info.height=(unsigned int) rectangle_info.height+(line_width << 1);
    if (draw_info.height > (unsigned int) (*image)->rows)
      draw_info.height=(unsigned int) (*image)->rows;
    FormatString(draw_info.geometry,"%ux%u%+d%+d",
      width*draw_info.width/windows->image.ximage->width,
      height*draw_info.height/windows->image.ximage->height,
      draw_info.x+x,draw_info.y+y);
    /*
      Initialize drawing attributes.
    */
    draw_info.degrees=0.0;
    draw_info.element=element;
    draw_info.stipple=stipple;
    draw_info.line_width=line_width;
    draw_info.line_info=line_info;
    if (line_info.x1 > (int) (line_width/2))
      draw_info.line_info.x1=line_width/2;
    if (line_info.y1 > (int) (line_width/2))
      draw_info.line_info.y1=line_width/2;
    draw_info.line_info.x2=line_info.x2-line_info.x1+(line_width/2);
    draw_info.line_info.y2=line_info.y2-line_info.y1+(line_width/2);
    if ((draw_info.line_info.x2 < 0) && (draw_info.line_info.y2 < 0))
      {
        draw_info.line_info.x2=(-draw_info.line_info.x2);
        draw_info.line_info.y2=(-draw_info.line_info.y2);
      }
    if (draw_info.line_info.x2 < 0)
      {
        draw_info.line_info.x2=(-draw_info.line_info.x2);
        Swap(draw_info.line_info.x1,draw_info.line_info.x2);
      }
    if (draw_info.line_info.y2 < 0)
      {
        draw_info.line_info.y2=(-draw_info.line_info.y2);
        Swap(draw_info.line_info.y1,draw_info.line_info.y2);
      }
    draw_info.rectangle_info=rectangle_info;
    if (draw_info.rectangle_info.x > (int) (line_width/2))
      draw_info.rectangle_info.x=(long) line_width/2;
    if (draw_info.rectangle_info.y > (int) (line_width/2))
      draw_info.rectangle_info.y=(long) line_width/2;
    draw_info.number_coordinates=number_coordinates;
    draw_info.coordinate_info=coordinate_info;
    windows->pixel_info->pen_color=windows->pixel_info->pen_colors[pen_id];
    /*
      Draw element on image.
    */
    MagickXSetCursorState(display,windows,True);
    MagickXCheckRefreshWindows(display,windows);
    status=MagickXDrawImage(display,windows->pixel_info,&draw_info,*image);
    MagickXSetCursorState(display,windows,False);
    /*
      Update image colormap and return to image drawing.
    */
    MagickXConfigureImageColormap(display,resource_info,windows,*image);
    (void) MagickXConfigureImage(display,resource_info,windows,*image);
  }
  MagickXSetCursorState(display,windows,False);
  MagickFreeMemory(coordinate_info);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k X D r a w P a n R e c t a n g l e                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickXDrawPanRectangle draws a rectangle in the pan window.  The pan
%  window displays a zoom image and the rectangle shows which portion of
%  the image is displayed in the Image window.
%
%  The format of the MagickXDrawPanRectangle method is:
%
%      MagickXDrawPanRectangle(Display *display,MagickXWindows *windows)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o windows: Specifies a pointer to a MagickXWindows structure.
%
%
*/
static void MagickXDrawPanRectangle(Display *display,MagickXWindows *windows)
{
  double
    scale_factor;

  RectangleInfo
    highlight_info;

  /*
    Determine dimensions of the panning rectangle.
  */
  scale_factor=(double) windows->pan.width/windows->image.ximage->width;
  highlight_info.x=(int) (scale_factor*windows->image.x+0.5);
  highlight_info.width=(unsigned int) (scale_factor*windows->image.width+0.5);
  scale_factor=(double) windows->pan.height/windows->image.ximage->height;
  highlight_info.y=(int) (scale_factor*windows->image.y+0.5);
  highlight_info.height=(unsigned int) (scale_factor*windows->image.height+0.5);
  /*
    Display the panning rectangle.
  */
  (void) XClearWindow(display,windows->pan.id);
  MagickXHighlightRectangle(display,windows->pan.id,windows->pan.annotate_context,
    &highlight_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k X I m a g e C a c h e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickXImageCache handles the creation, manipulation, and destruction of
%  the image cache (undo and redo buffers).
%
%  The format of the MagickXImageCache method is:
%
%      void MagickXImageCache(Display *display,MagickXResourceInfo *resource_info,
%        MagickXWindows *windows,const CommandType command,Image **image)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 MagickXResourceInfo structure.
%
%    o windows: Specifies a pointer to a MagickXWindows structure.
%
%    o command: Specifies a command to perform.
%
%    o image: Specifies a pointer to an Image structure;  MagickXImageCache
%      may transform the image and return a new image pointer.
%
%
*/
static void MagickXImageCache(Display *display,MagickXResourceInfo *resource_info,
  MagickXWindows *windows,const CommandType command,Image **image)
{
  Image
    *cache_image;

  static Image
    *redo_image = (Image *) NULL,
    *undo_image = (Image *) NULL;

  switch (command)
  {
    case FreeBuffersCommand:
    {
      /*
        Free memory from the undo and redo cache.
      */
      while (undo_image != (Image *) NULL)
      {
        cache_image=undo_image;
        undo_image=undo_image->previous;
        DestroyImage(cache_image->list);
        DestroyImage(cache_image);
      }
      undo_image=(Image *) NULL;
      if (redo_image != (Image *) NULL)
        DestroyImage(redo_image);
      redo_image=(Image *) NULL;
      return;
    }
    case UndoCommand:
    {
      /*
        Undo the last image transformation.
      */
      if (undo_image == (Image *) NULL)
        {
          (void) XBell(display,0);
          return;
        }
      cache_image=undo_image;
      undo_image=undo_image->previous;
      windows->image.window_changes.width=(unsigned int) cache_image->columns;
      windows->image.window_changes.height=(unsigned int) cache_image->rows;
      if (windows->image.crop_geometry != (char *) NULL)
        MagickFreeMemory(windows->image.crop_geometry);
      windows->image.crop_geometry=cache_image->geometry;
      if (redo_image != (Image *) NULL)
        DestroyImage(redo_image);
      redo_image=(*image);
      *image=cache_image->list;
      DestroyImage(cache_image);
      if (windows->image.orphan)
        return;
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      return;
    }
    case CutCommand:
    case PasteCommand:
    case ApplyCommand:
    case HalfSizeCommand:
    case OriginalSizeCommand:
    case DoubleSizeCommand:
    case ResizeCommand:
    case TrimCommand:
    case CropCommand:
    case ChopCommand:
    case FlipCommand:
    case FlopCommand:
    case RotateRightCommand:
    case RotateLeftCommand:
    case RotateCommand:
    case ShearCommand:
    case RollCommand:
    case NegateCommand:
    case EqualizeCommand:
    case NormalizeCommand:
    case HueCommand:
    case SaturationCommand:
    case BrightnessCommand:
    case GammaCommand:
    case SpiffCommand:
    case DullCommand:
    case GrayscaleCommand:
    case MapCommand:
    case QuantizeCommand:
    case DespeckleCommand:
    case EmbossCommand:
    case ReduceNoiseCommand:
    case AddNoiseCommand:
    case SharpenCommand:
    case BlurCommand:
    case ThresholdCommand:
    case EdgeDetectCommand:
    case SpreadCommand:
    case ShadeCommand:
    case RaiseCommand:
    case SegmentCommand:
    case SolarizeCommand:
    case SwirlCommand:
    case ImplodeCommand:
    case WaveCommand:
    case OilPaintCommand:
    case CharcoalDrawCommand:
    case AnnotateCommand:
    case AddBorderCommand:
    case AddFrameCommand:
    case CompositeCommand:
    case CommentCommand:
    case LaunchCommand:
    case RegionofInterestCommand:
    case SaveToUndoBufferCommand:
    case RedoCommand:
    {
      Image
        *previous_image;

      long
        bytes;

      bytes=(long) ((*image)->columns*(*image)->rows*sizeof(PixelPacket));
      if (undo_image != (Image *) NULL)
        {
          /*
            Ensure the undo stash.has enough memory available.
          */
          previous_image=undo_image;
          while (previous_image != (Image *) NULL)
          {
            bytes+=previous_image->list->columns*previous_image->list->rows*
              sizeof(PixelPacket);
            if (bytes <= (long) (resource_info->undo_cache << 20))
              {
                previous_image=previous_image->previous;
                continue;
              }
            bytes-=previous_image->list->columns*previous_image->list->rows*
              sizeof(PixelPacket);
            if (previous_image == undo_image)
              undo_image=(Image *) NULL;
            else
              previous_image->next->previous=(Image *) NULL;
            break;
          }
          while (previous_image != (Image *) NULL)
          {
            /*
              Delete any excess memory from undo cache.
            */
            cache_image=previous_image;
            previous_image=previous_image->previous;
            DestroyImage(cache_image->list);
            DestroyImage(cache_image);
          }
        }
      if (bytes > (long) (resource_info->undo_cache << 20))
        break;
      /*
        Save image before transformations are applied.
      */
      cache_image=AllocateImage((ImageInfo *) NULL);
      if (cache_image == (Image *) NULL)
        break;
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      cache_image->list=CloneImage(*image,0,0,True,&(*image)->exception);
      MagickXSetCursorState(display,windows,False);
      if (cache_image->list == (Image *) NULL)
        {
          DestroyImage(cache_image);
          break;
        }
      cache_image->columns=windows->image.ximage->width;
      cache_image->rows=windows->image.ximage->height;
      cache_image->geometry=windows->image.crop_geometry;
      if (windows->image.crop_geometry != (char *) NULL)
        {
          cache_image->geometry=AllocateString((char *) NULL);
          (void) strlcpy(cache_image->geometry,windows->image.crop_geometry,
            MaxTextExtent);
        }
      if (undo_image == (Image *) NULL)
        {
          undo_image=cache_image;
          break;
        }
      undo_image->next=cache_image;
      undo_image->next->previous=undo_image;
      undo_image=undo_image->next;
      break;
    }
    default:
      break;
  }
  if (command == RedoCommand)
    {
      /*
        Redo the last image transformation.
      */
      if (redo_image == (Image *) NULL)
        {
          (void) XBell(display,0);
          return;
        }
      windows->image.window_changes.width=(unsigned int) redo_image->columns;
      windows->image.window_changes.height=(unsigned int) redo_image->rows;
      if (windows->image.crop_geometry != (char *) NULL)
        MagickFreeMemory(windows->image.crop_geometry);
      windows->image.crop_geometry=redo_image->geometry;
      DestroyImage(*image);
      *image=redo_image;
      redo_image=(Image *) NULL;
      if (windows->image.orphan)
        return;
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      return;
    }
  if (command != InfoCommand)
    return;
  /*
    Display image info.
  */
  MagickXSetCursorState(display,windows,True);
  MagickXCheckRefreshWindows(display,windows);
  MagickXDisplayImageInfo(display,resource_info,windows,undo_image,*image);
  MagickXSetCursorState(display,windows,False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k X I m a g e W i n d o w C o m m a n d                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickXImageWindowCommand makes a transform to the image or Image window
%  as specified by a user menu button or keyboard command.
%
%  The format of the MagickXMagickCommand method is:
%
%      CommandType MagickXImageWindowCommand(Display *display,
%        MagickXResourceInfo *resource_info,MagickXWindows *windows,
%        const unsigned int state,KeySym key_symbol,Image **image)
%
%  A description of each parameter follows:
%
%    o nexus:  Method MagickXImageWindowCommand returns an image when the
%      user chooses 'Open Image' from the command menu.  Otherwise a null
%      image is returned.
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 MagickXResourceInfo structure.
%
%    o windows: Specifies a pointer to a MagickXWindows structure.
%
%    o state: key mask.
%
%    o key_symbol: Specifies a command to perform.
%
%    o image: Specifies a pointer to an Image structure;  XImageWIndowCommand
%      may transform the image and return a new image pointer.
%
%
*/
static CommandType MagickXImageWindowCommand(Display *display,
  MagickXResourceInfo *resource_info,MagickXWindows *windows,const unsigned int state,
  KeySym key_symbol,Image **image)
{
  static char
    delta[MaxTextExtent] = "";

  static const char
    Digits[] = "01234567890";

  static KeySym
    last_symbol = XK_0;

  if ((key_symbol >= XK_0) && (key_symbol <= XK_9))
    {
      if (!((last_symbol >= XK_0) && (last_symbol <= XK_9)))
        {
          *delta='\0';
          resource_info->quantum=1;
        }
      last_symbol=key_symbol;
      delta[strlen(delta)+1]='\0';
      delta[strlen(delta)]=Digits[key_symbol-XK_0];
      resource_info->quantum=atoi(delta);
      return(NullCommand);
    }
  last_symbol=key_symbol;
  if (resource_info->immutable)
    {
      /*
        Immutable image window has a restricted command set.
      */
      switch(key_symbol)
      {
        case XK_question:
          return(InfoCommand);
        case XK_p:
        case XK_Print:
          return(PrintCommand);
        case XK_space:
          return(NextCommand);
        case XK_q:
        {
          if (!(state & ControlMask))
            break;
          return(QuitCommand);
        }
        default:
          break;
      }
      return(NullCommand);
    }
  switch ((int) key_symbol)
  {
    case XK_o:
    {
      if (!(state & ControlMask))
        break;
      return(OpenCommand);
    }
    case XK_space:
      return(NextCommand);
    case XK_BackSpace:
      return(FormerCommand);
    case XK_s:
    {
      if (state & Mod1Mask)
        return(SwirlCommand);
      if (!(state & ControlMask))
        return(ShearCommand);
      return(SaveCommand);
    }
    case XK_p:
    case XK_Print:
    {
      if (state & Mod1Mask)
        return(OilPaintCommand);
      if (state & Mod4Mask)
        return(ColorCommand);
      if (!(state & ControlMask))
        return(NullCommand);
      return(PrintCommand);
    }
    case XK_d:
    {
      if (state & Mod4Mask)
        return(DrawCommand);
      if (!(state & ControlMask))
        return(NullCommand);
      return(DeleteCommand);
    }
    case XK_Select:
    {
      if (!(state & ControlMask))
        return(NullCommand);
      return(SelectCommand);
    }
    case XK_n:
    {
      if (!(state & ControlMask))
        return(NullCommand);
      return(NewCommand);
    }
    case XK_q:
    case XK_Cancel:
    {
      if (!(state & ControlMask))
        return(NullCommand);
      return(QuitCommand);
    }
    case XK_z:
    case XK_Undo:
    {
      if (!(state & ControlMask))
        return(NullCommand);
      return(UndoCommand);
    }
    case XK_r:
    case XK_Redo:
    {
      if (!(state & ControlMask))
        return(RollCommand);
      return(RedoCommand);
    }
    case XK_x:
    {
      if (!(state & ControlMask))
        return(NullCommand);
      return(CutCommand);
    }
    case XK_c:
    {
      if (state & Mod1Mask)
        return(CharcoalDrawCommand);
      if (!(state & ControlMask))
        return(CropCommand);
      return(CopyCommand);
    }
    case XK_v:
    case XK_Insert:
    {
      if (state & Mod4Mask)
        return(CompositeCommand);
      if (!(state & ControlMask))
        return(FlipCommand);
      return(PasteCommand);
    }
    case XK_less:
      return(HalfSizeCommand);
    case XK_minus:
      return(OriginalSizeCommand);
    case XK_greater:
      return(DoubleSizeCommand);
    case XK_percent:
      return(ResizeCommand);
    case XK_at:
      return(RefreshCommand);
    case XK_bracketleft:
      return(ChopCommand);
    case XK_h:
      return(FlopCommand);
    case XK_slash:
      return(RotateRightCommand);
    case XK_backslash:
      return(RotateLeftCommand);
    case XK_asterisk:
      return(RotateCommand);
    case XK_t:
      return(TrimCommand);
    case XK_H:
      return(HueCommand);
    case XK_S:
      return(SaturationCommand);
    case XK_L:
      return(BrightnessCommand);
    case XK_G:
      return(GammaCommand);
    case XK_C:
      return(SpiffCommand);
    case XK_Z:
      return(DullCommand);
    case XK_equal:
      return(EqualizeCommand);
    case XK_N:
      return(NormalizeCommand);
    case XK_asciitilde:
      return(NegateCommand);
    case XK_period:
      return(GrayscaleCommand);
    case XK_numbersign:
      return(QuantizeCommand);
    case XK_F2:
      return(DespeckleCommand);
    case XK_F3:
      return(EmbossCommand);
    case XK_F4:
      return(ReduceNoiseCommand);
    case XK_F5:
      return(AddNoiseCommand);
    case XK_F6:
      return(SharpenCommand);
    case XK_F7:
      return(BlurCommand);
    case XK_F8:
      return(ThresholdCommand);
    case XK_F9:
      return(EdgeDetectCommand);
    case XK_F10:
      return(SpreadCommand);
    case XK_F11:
      return(ShadeCommand);
    case XK_F12:
      return(RaiseCommand);
    case XK_F13:
      return(SegmentCommand);
    case XK_i:
    {
      if (!(state & Mod1Mask))
        return(NullCommand);
      return(ImplodeCommand);
    }
    case XK_w:
    {
      if (!(state & Mod1Mask))
        return(NullCommand);
      return(WaveCommand);
    }
    case XK_m:
    {
      if (!(state & Mod4Mask))
        return(NullCommand);
      return(MatteCommand);
    }
    case XK_b:
    {
      if (!(state & Mod4Mask))
        return(NullCommand);
      return(AddBorderCommand);
    }
    case XK_f:
    {
      if (!(state & Mod4Mask))
        return(NullCommand);
      return(AddFrameCommand);
    }
    case XK_exclam:
    {
      if (!(state & Mod4Mask))
        return(NullCommand);
      return(CommentCommand);
    }
    case XK_a:
    {
      if (state & Mod1Mask)
        return(ApplyCommand);
      if (state & Mod4Mask)
        return(AnnotateCommand);
      if (!(state & ControlMask))
        return(NullCommand);
      return(RegionofInterestCommand);
    }
    case XK_question:
      return(InfoCommand);
    case XK_plus:
      return(ZoomCommand);
    case XK_P:
    {
      if (!(state & ShiftMask))
        return(NullCommand);
      return(ShowPreviewCommand);
    }
    case XK_Execute:
      return(LaunchCommand);
    case XK_F1:
      return(HelpCommand);
    case XK_Find:
      return(BrowseDocumentationCommand);
    case XK_Menu:
    {
      (void) XMapRaised(display,windows->command.id);
      return(NullCommand);
    }
    case XK_Next:
    case XK_Prior:
    case XK_Home:
    case XK_KP_Home:
    {
      MagickXTranslateImage(display,windows,*image,key_symbol);
      return(NullCommand);
    }
    case XK_Up:
    case XK_KP_Up:
    case XK_Down:
    case XK_KP_Down:
    case XK_Left:
    case XK_KP_Left:
    case XK_Right:
    case XK_KP_Right:
    {
      if (state & Mod1Mask)
        {
          RectangleInfo
            crop_info;

          /*
            Trim one pixel from edge of image.
          */
          crop_info.x=0;
          crop_info.y=0;
          crop_info.width=windows->image.ximage->width;
          crop_info.height=windows->image.ximage->height;
          if ((key_symbol == XK_Up) || (key_symbol == XK_KP_Up))
            {
              if (resource_info->quantum >= (int) crop_info.height)
                resource_info->quantum=(unsigned int) crop_info.height-1;
              crop_info.height-=resource_info->quantum;
            }
          if ((key_symbol == XK_Down) || (key_symbol == XK_KP_Down))
            {
              if (resource_info->quantum >= (int) (crop_info.height-crop_info.y))
                resource_info->quantum=(unsigned int)
                  (crop_info.height-crop_info.y-1);
              crop_info.y+=resource_info->quantum;
              crop_info.height-=resource_info->quantum;
            }
          if ((key_symbol == XK_Left) || (key_symbol == XK_KP_Left))
            {
              if (resource_info->quantum >= (int) crop_info.width)
                resource_info->quantum=(int) crop_info.width-1;
              crop_info.width-=resource_info->quantum;
            }
          if ((key_symbol == XK_Right) || (key_symbol == XK_KP_Right))
            {
              if (resource_info->quantum >= (int) (crop_info.width-crop_info.x))
                resource_info->quantum=(unsigned int)
                  (crop_info.width-crop_info.x-1);
              crop_info.x+=resource_info->quantum;
              crop_info.width-=resource_info->quantum;
            }
          if ((int) (windows->image.x+windows->image.width) >
              (int) crop_info.width)
            windows->image.x=(int) (crop_info.width-windows->image.width);
          if ((int) (windows->image.y+windows->image.height) >
              (int) crop_info.height)
            windows->image.y=(int) (crop_info.height-windows->image.height);
          MagickXSetCropGeometry(display,windows,&crop_info,*image);
          windows->image.window_changes.width=(unsigned int) crop_info.width;
          windows->image.window_changes.height=(unsigned int) crop_info.height;
          (void) XSetWindowBackgroundPixmap(display,windows->image.id,None);
          (void) MagickXConfigureImage(display,resource_info,windows,*image);
          return(NullCommand);
        }
      MagickXTranslateImage(display,windows,*image,key_symbol);
      return(NullCommand);
    }
    default:
      return(NullCommand);
  }
  return(NullCommand);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k X M a g i c k C o m m a n d                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickXMagickCommand makes a transform to the image or Image window
%  as specified by a user menu button or keyboard command.
%
%  The format of the MagickXMagickCommand method is:
%
%      Image *MagickXMagickCommand(Display *display,MagickXResourceInfo *resource_info,
%        MagickXWindows *windows,const CommandType command,Image **image)
%
%  A description of each parameter follows:
%
%    o nexus:  Method MagickXMagickCommand returns an image when the
%      user chooses 'Load Image' from the command menu.  Otherwise a null
%      image is returned.
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 MagickXResourceInfo structure.
%
%    o windows: Specifies a pointer to a MagickXWindows structure.
%
%    o command: Specifies a command to perform.
%
%    o image: Specifies a pointer to an Image structure;  MagickXMagickCommand
%      may transform the image and return a new image pointer.
%
%
*/
#define ReplaceImage(oldimage,func) \
{ \
  Image \
    *temporary_image; \
\
  temporary_image=func; \
  if (temporary_image) \
  { \
    DestroyImage(oldimage); \
    oldimage=temporary_image; \
  } \
}
static Image *MagickXMagickCommand(Display *display,MagickXResourceInfo *resource_info,
  MagickXWindows *windows,const CommandType command,Image **image)
{
  char
    /* *argv[10], */
    filename[MaxTextExtent],
    geometry[MaxTextExtent],
    modulate_factors[MaxTextExtent];

  Image
    *nexus;

  ImageInfo
    *image_info;

  RectangleInfo
    rectangle;

  int
    status,
    x,
    y;

  static char
    color[MaxTextExtent] = "gray";

  unsigned int
    height,
    width;

  /*
    Process user command.
  */
  MagickXCheckRefreshWindows(display,windows);
  MagickXImageCache(display,resource_info,windows,command,image);
  /* argv[0]=resource_info->client_name; */
  nexus=(Image *) NULL;
  windows->image.window_changes.width=windows->image.ximage->width;
  windows->image.window_changes.height=windows->image.ximage->height;
  image_info=CloneImageInfo((ImageInfo *) NULL);
  switch (command)
  {
    case OpenCommand:
    {
      /*
        Load image.
      */
      nexus=MagickXOpenImage(display,resource_info,windows,False);
      break;
    }
    case NextCommand:
    {
      /*
        Display next image.
      */
      MagickXClientMessage(display,windows->image.id,windows->im_protocols,
        windows->im_next_image,CurrentTime);
      break;
    }
    case FormerCommand:
    {
      /*
        Display former image.
      */
      MagickXClientMessage(display,windows->image.id,windows->im_protocols,
        windows->im_former_image,CurrentTime);
      break;
    }
    case SelectCommand:
    {
      /*
        Select image.
      */
      (void) chdir(resource_info->home_directory);
      nexus=MagickXOpenImage(display,resource_info,windows,True);
      break;
    }
    case SaveCommand:
    {
      /*
        Save image.
      */
      status=MagickXSaveImage(display,resource_info,windows,*image);
      if (status == False)
        {
          MagickXNoticeWidget(display,windows,"Unable to write X image:",
            (*image)->filename);
          break;
        }
      break;
    }
    case PrintCommand:
    {
      /*
        Print image.
      */
      status=MagickXPrintImage(display,resource_info,windows,*image);
      if (status == False)
        {
          MagickXNoticeWidget(display,windows,"Unable to print X image:",
            (*image)->filename);
          break;
        }
      break;
    }
    case DeleteCommand:
    {
      static char
        filename[MaxTextExtent] = "\0";

      /*
        Delete image file.
      */
      MagickXFileBrowserWidget(display,windows,"Delete",filename);
      if (*filename == '\0')
        break;
      status=remove(filename);
      if (status != False)
        MagickXNoticeWidget(display,windows,"Unable to delete image file:",filename);
      break;
    }
    case NewCommand:
    {
      static char
        *format = (char *) "gradient",
        color[MaxTextExtent] = "gray",
        geometry[MaxTextExtent] = "640x480";

      /*
        Query user for canvas geometry.
      */
      status=MagickXDialogWidget(display,windows,"New","Enter image geometry:",
        geometry);
      if (*geometry == '\0')
        break;
      if (!status)
        format=(char *) "xc";
      MagickXColorBrowserWidget(display,windows,"Select",color);
      if (*color == '\0')
        break;
      /*
        Create canvas.
      */
      FormatString(image_info->filename,"%.1024s:%.1024s",format,color);
      (void) CloneString(&image_info->size,geometry);
      nexus=ReadImage(image_info,&(*image)->exception);
      if ((*image)->exception.severity != UndefinedException)
        MagickError2((*image)->exception.severity,(*image)->exception.reason,
          (*image)->exception.description);
      MagickXClientMessage(display,windows->image.id,windows->im_protocols,
        windows->im_next_image,CurrentTime);
      break;
    }
    case VisualDirectoryCommand:
    {
      /*
        Visual Image directory.
      */
      nexus=MagickXVisualDirectoryImage(display,resource_info,windows);
      break;
    }
    case QuitCommand:
    {
      /*
        Exit program.
      */
      if (!resource_info->confirm_exit)
        MagickXClientMessage(display,windows->image.id,windows->im_protocols,
          windows->im_exit,CurrentTime);
      else
        {
          /*
            Confirm program exit.
          */
          status=MagickXConfirmWidget(display,windows,"Do you really want to exit",
            resource_info->client_name);
          if (status > 0)
            MagickXClientMessage(display,windows->image.id,windows->im_protocols,
              windows->im_exit,CurrentTime);
        }
      break;
    }
    case CutCommand:
    {
      /*
        Cut image.
      */
      (void) MagickXCropImage(display,resource_info,windows,*image,CutMode);
      break;
    }
    case CopyCommand:
    {
      /*
        Copy image.
      */
      (void) MagickXCropImage(display,resource_info,windows,*image,CopyMode);
      break;
    }
    case PasteCommand:
    {
      /*
        Paste image.
      */
      status=MagickXPasteImage(display,resource_info,windows,*image);
      if (status == False)
        {
          MagickXNoticeWidget(display,windows,"Unable to paste X image",
            (*image)->filename);
          break;
        }
      break;
    }
    case HalfSizeCommand:
    {
      /*
        Half image size.
      */
      windows->image.window_changes.width=windows->image.ximage->width/2;
      windows->image.window_changes.height=windows->image.ximage->height/2;
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case OriginalSizeCommand:
    {
      /*
        Original image size.
      */
      windows->image.window_changes.width=(unsigned int) (*image)->columns;
      windows->image.window_changes.height=(unsigned int) (*image)->rows;
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case DoubleSizeCommand:
    {
      /*
        Double the image size.
      */
      windows->image.window_changes.width=windows->image.ximage->width << 1;
      windows->image.window_changes.height=windows->image.ximage->height << 1;
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case ResizeCommand:
    {
      long
        x,
        y;

      unsigned long
        height,
        width;

      /*
        Resize image.
      */
      width=windows->image.ximage->width;
      height=windows->image.ximage->height;
      x=0;
      y=0;
      FormatString(geometry,"%lux%lu+0+0",width,height);
      status=MagickXDialogWidget(display,windows,"Resize",
        "Enter resize geometry (e.g. 640x480, 200%):",geometry);
      if (*geometry == '\0')
        break;
      if (!status)
        (void) strcat(geometry,"!");
      (void) GetMagickGeometry(geometry,&x,&y,&width,&height);
      windows->image.window_changes.width=(unsigned int) width;
      windows->image.window_changes.height=(unsigned int) height;
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case ApplyCommand:
    {
      char
        image_geometry[MaxTextExtent];

      if ((windows->image.crop_geometry == (char *) NULL) &&
          ((int) (*image)->columns == windows->image.ximage->width) &&
          ((int) (*image)->rows == windows->image.ximage->height))
        break;
      /*
        Apply size transforms to image.
      */
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      /*
        Crop and/or scale displayed image.
      */
      FormatString(image_geometry,"%dx%d!",windows->image.ximage->width,
        windows->image.ximage->height);
      TransformImage(image,windows->image.crop_geometry,image_geometry);
      if (windows->image.crop_geometry != (char *) NULL)
        {
          MagickFreeMemory(windows->image.crop_geometry);
          windows->image.crop_geometry=(char *) NULL;
        }
      windows->image.x=0;
      windows->image.y=0;
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case RefreshCommand:
    {
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case RestoreCommand:
    {
      /*
        Restore Image window to its original size.
      */
      if ((windows->image.width == (unsigned int) (*image)->columns) &&
          (windows->image.height == (unsigned int) (*image)->rows) &&
          (windows->image.crop_geometry == (char *) NULL))
        {
          (void) XBell(display,0);
          break;
        }
      windows->image.window_changes.width=(unsigned int) (*image)->columns;
      windows->image.window_changes.height=(unsigned int) (*image)->rows;
      if (windows->image.crop_geometry != (char *) NULL)
        {
          MagickFreeMemory(windows->image.crop_geometry);
          windows->image.crop_geometry=(char *) NULL;
          windows->image.x=0;
          windows->image.y=0;
        }
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case CropCommand:
    {
      /*
        Crop image.
      */
      (void) MagickXCropImage(display,resource_info,windows,*image,CropMode);
      break;
    }
    case ChopCommand:
    {
      /*
        Chop image.
      */
      status=MagickXChopImage(display,resource_info,windows,image);
      if (status == False)
        {
          MagickXNoticeWidget(display,windows,"Unable to cut X image",
            (*image)->filename);
          break;
        }
      break;
    }
    case FlopCommand:
    {
      /*
        Flop image scanlines.
      */
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      ReplaceImage(*image,FlopImage(*image,&(*image)->exception));
      MagickXSetCursorState(display,windows,False);
      if (windows->image.crop_geometry != (char *) NULL)
        {
          /*
            Flop crop geometry.
          */
          width=(unsigned int) (*image)->columns;
          height=(unsigned int) (*image)->rows;
          (void) XParseGeometry(windows->image.crop_geometry,&x,&y,
            &width,&height);
          FormatString(windows->image.crop_geometry,"%ux%u%+d%+d",width,
            height,(int) (*image)->columns-(int) width-x,y);
        }
      if (windows->image.orphan)
        break;
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case FlipCommand:
    {
      /*
        Flip image scanlines.
      */
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      ReplaceImage(*image,FlipImage(*image,&(*image)->exception));
      MagickXSetCursorState(display,windows,False);
      if (windows->image.crop_geometry != (char *) NULL)
        {
          /*
            Flip crop geometry.
          */
          width=(unsigned int) (*image)->columns;
          height=(unsigned int) (*image)->rows;
          (void) XParseGeometry(windows->image.crop_geometry,&x,&y,
            &width,&height);
          FormatString(windows->image.crop_geometry,"%ux%u%+d%+d",width,
            height,x,(int) (*image)->rows-(int) height-y);
        }
      if (windows->image.orphan)
        break;
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case RotateRightCommand:
    {
      /*
        Rotate image 90 degrees clockwise.
      */
      status=MagickXRotateImage(display,resource_info,windows,90.0,image);
      if (status == False)
        {
          MagickXNoticeWidget(display,windows,"Unable to rotate X image",
            (*image)->filename);
          break;
        }
      break;
    }
    case RotateLeftCommand:
    {
      /*
        Rotate image 90 degrees counter-clockwise.
      */
      status=MagickXRotateImage(display,resource_info,windows,-90.0,image);
      if (status == False)
        {
          MagickXNoticeWidget(display,windows,"Unable to rotate X image",
            (*image)->filename);
          break;
        }
      break;
    }
    case RotateCommand:
    {
      /*
        Rotate image.
      */
      status=MagickXRotateImage(display,resource_info,windows,0.0,image);
      if (status == False)
        {
          MagickXNoticeWidget(display,windows,"Unable to rotate X image",
            (*image)->filename);
          break;
        }
      break;
    }
    case ShearCommand:
    {
      static char
        geometry[MaxTextExtent] = "45.0x45.0";

      double
        x_shear,
        y_shear;

      /*
        Query user for shear color and geometry.
      */
      MagickXColorBrowserWidget(display,windows,"Select",color);
      if (*color == '\0')
        break;
      (void) MagickXDialogWidget(display,windows,"Shear","Enter shear geometry:",
        geometry);
      if (*geometry == '\0')
        break;
      /*
        Shear image.
      */
      (void) MagickXMagickCommand(display,resource_info,windows,ApplyCommand,image);
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      (void) QueryColorDatabase(color,&(*image)->background_color,
         &(*image)->exception);
      x_shear=0.0;
      y_shear=0.0;
      (void) GetMagickDimension(geometry,&x_shear,&y_shear,NULL,NULL);
      ReplaceImage(*image,ShearImage(*image,x_shear,y_shear,
        &(*image)->exception));
      MagickXSetCursorState(display,windows,False);
      if (windows->image.orphan)
        break;
      windows->image.window_changes.width=(unsigned int) (*image)->columns;
      windows->image.window_changes.height=(unsigned int) (*image)->rows;
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case RollCommand:
    {
      static char
        geometry[MaxTextExtent] = "+2+2";

      /*
        Query user for the roll geometry.
      */
      (void) MagickXDialogWidget(display,windows,"Roll","Enter roll geometry:",
        geometry);
      if (*geometry == '\0')
        break;
      /*
        Roll image.
      */
      (void) MagickXMagickCommand(display,resource_info,windows,ApplyCommand,image);
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      (void) GetImageGeometry(*image,geometry,False,&rectangle);
      ReplaceImage(*image,RollImage(*image,rectangle.x,rectangle.y,
        &(*image)->exception));
      MagickXSetCursorState(display,windows,False);
      if (windows->image.orphan)
        break;
      windows->image.window_changes.width=(unsigned int) (*image)->columns;
      windows->image.window_changes.height=(unsigned int) (*image)->rows;
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case TrimCommand:
    {
      /*
        Trim image.
      */
      status=MagickXTrimImage(display,resource_info,windows,*image);
      if (status == False)
        {
          MagickXNoticeWidget(display,windows,"Unable to trim X image",
            (*image)->filename);
          break;
        }
      break;
    }
    case HueCommand:
    {
      static char
        hue_percent[MaxTextExtent] = "110";

      /*
        Query user for percent hue change.
      */
      (void) MagickXDialogWidget(display,windows,"Apply",
        "Enter percent change in image hue (0-200):",hue_percent);
      if (*hue_percent == '\0')
        break;
      /*
        Vary the image hue.
      */
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      (void) strlcpy(modulate_factors,"100.0/100.0/",MaxTextExtent);
      (void) strlcat(modulate_factors,hue_percent,MaxTextExtent);
      (void) ModulateImage(*image,modulate_factors);
      MagickXSetCursorState(display,windows,False);
      if (windows->image.orphan)
        break;
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case SaturationCommand:
    {
      static char
        saturation_percent[MaxTextExtent] = "110";

      /*
        Query user for percent saturation change.
      */
      (void) MagickXDialogWidget(display,windows,"Apply",
        "Enter percent change in color saturation (0-200):",saturation_percent);
      if (*saturation_percent == '\0')
        break;
      /*
        Vary color saturation.
      */
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      (void) strlcpy(modulate_factors,"100.0/",MaxTextExtent);
      (void) strlcat(modulate_factors,saturation_percent,MaxTextExtent);
      (void) ModulateImage(*image,modulate_factors);
      MagickXSetCursorState(display,windows,False);
      if (windows->image.orphan)
        break;
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case BrightnessCommand:
    {
      static char
        brightness_percent[MaxTextExtent] = "110";

      /*
        Query user for percent brightness change.
      */
      (void) MagickXDialogWidget(display,windows,"Apply",
        "Enter percent change in color brightness (0-200):",brightness_percent);
      if (*brightness_percent == '\0')
        break;
      /*
        Vary the color brightness.
      */
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      (void) strlcpy(modulate_factors,brightness_percent,MaxTextExtent);
      (void) ModulateImage(*image,modulate_factors);
      MagickXSetCursorState(display,windows,False);
      if (windows->image.orphan)
        break;
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case GammaCommand:
    {
      static char
        factor[MaxTextExtent] = "1.6";

      /*
        Query user for gamma value.
      */
      (void) MagickXDialogWidget(display,windows,"Gamma",
        "Enter gamma value (e.g. 1.0/1.0/1.6):",factor);
      if (*factor == '\0')
        break;
      /*
        Gamma correct image.
      */
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      (void) GammaImage(*image,factor);
      MagickXSetCursorState(display,windows,False);
      if (windows->image.orphan)
        break;
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case SpiffCommand:
    {
      /*
        Sharpen the image contrast.
      */
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      (void) ContrastImage(*image,True);
      MagickXSetCursorState(display,windows,False);
      if (windows->image.orphan)
        break;
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case DullCommand:
    {
      /*
        Dull the image contrast.
      */
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      (void) ContrastImage(*image,False);
      MagickXSetCursorState(display,windows,False);
      if (windows->image.orphan)
        break;
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case EqualizeCommand:
    {
      /*
        Perform histogram equalization on the image.
      */
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      (void) EqualizeImage(*image);
      MagickXSetCursorState(display,windows,False);
      if (windows->image.orphan)
        break;
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case NormalizeCommand:
    {
      /*
        Perform histogram normalization on the image.
      */
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      (void) NormalizeImage(*image);
      MagickXSetCursorState(display,windows,False);
      if (windows->image.orphan)
        break;
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case NegateCommand:
    {
      /*
        Negate colors in image.
      */
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      (void) NegateImage(*image,False);
      MagickXSetCursorState(display,windows,False);
      if (windows->image.orphan)
        break;
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case GrayscaleCommand:
    {
      /*
        Convert image to grayscale.
      */
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      (void) TransformColorspace(*image,GRAYColorspace);
      MagickXSetCursorState(display,windows,False);
      if (windows->image.orphan)
        break;
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case MapCommand:
    {
      static char
        filename[MaxTextExtent] = "\0";

      Image
        *map_image;

      /*
        Request image file name from user.
      */
      MagickXFileBrowserWidget(display,windows,"Map",filename);
      if (*filename == '\0')
        break;
      /*
        Map image.
      */
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      map_image=ReadImage(resource_info->image_info,&(*image)->exception);
      if (map_image != (Image *) NULL)
        {
          (void) MapImage(*image,map_image,True);
          DestroyImage(map_image);
        }
      MagickXSetCursorState(display,windows,False);
      if (windows->image.orphan)
        break;
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case QuantizeCommand:
    {
      static char
        colors[MaxTextExtent] = "256";

      QuantizeInfo
        quantize_info;

      /*
        Query user for maximum number of colors.
      */
      status=MagickXDialogWidget(display,windows,"Quantize",
        "Maximum number of colors:",colors);
      if (*colors == '\0')
        break;
      /*
        Color reduce the image.
      */
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      GetQuantizeInfo(&quantize_info);
      quantize_info.number_colors=atol(colors);
      quantize_info.dither=(status ? False : True);
      quantize_info.colorspace=(*image)->colorspace;
      (void) QuantizeImage(&quantize_info,*image);
      MagickXSetCursorState(display,windows,False);
      if (windows->image.orphan)
        break;
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case DespeckleCommand:
    {
      /*
        Despeckle image.
      */
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      ReplaceImage(*image,DespeckleImage(*image,&(*image)->exception));
      MagickXSetCursorState(display,windows,False);
      if (windows->image.orphan)
        break;
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case EmbossCommand:
    {
      static char
        emboss_argument[MaxTextExtent] = "0.0x1.0";

      double
        radius,
        sigma;

      /*
        Query user for emboss radius.
      */
      (void) MagickXDialogWidget(display,windows,"Emboss",
        "Enter the emboss radius and standard deviation:",emboss_argument);
      if (*emboss_argument == '\0')
        break;
      /*
        Reduce noise in the image.
      */
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      radius=0.0;
      sigma=1.0;
      (void) GetMagickDimension(emboss_argument,&radius,&sigma,NULL,NULL);
      ReplaceImage(*image,EmbossImage(*image,radius,sigma,
        &(*image)->exception));
      MagickXSetCursorState(display,windows,False);
      if (windows->image.orphan)
        break;
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case ReduceNoiseCommand:
    {
      static char
        radius[MaxTextExtent] = "0";

      /*
        Query user for noise radius.
      */
      (void) MagickXDialogWidget(display,windows,"Reduce Noise",
        "Enter the noise radius:",radius);
      if (*radius == '\0')
        break;
      /*
        Reduce noise in the image.
      */
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      ReplaceImage(*image,ReduceNoiseImage(*image,atol(radius),
        &(*image)->exception));
      MagickXSetCursorState(display,windows,False);
      if (windows->image.orphan)
        break;
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case AddNoiseCommand:
    {
      static char
        option[MaxTextExtent] = "Gaussian";

      NoiseType
        noise_type;

      /*
        Add noise to the image.
      */
      MagickXListBrowserWidget(display,windows,&windows->widget,NoiseTypes,
        "Add Noise","Select a type of noise to add to your image:",
        option);
      if (*option == '\0')
        break;
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      noise_type=UniformNoise;
      if (LocaleCompare("Gaussian",option) == 0)
        noise_type=GaussianNoise;
      if (LocaleCompare("multiplicative",option) == 0)
        noise_type=MultiplicativeGaussianNoise;
      if (LocaleCompare("impulse",option) == 0)
        noise_type=ImpulseNoise;
      if (LocaleCompare("laplacian",option) == 0)
        noise_type=LaplacianNoise;
      if (LocaleCompare("Poisson",option) == 0)
        noise_type=PoissonNoise;
      ReplaceImage(*image,AddNoiseImage(*image,noise_type,
        &(*image)->exception));
      MagickXSetCursorState(display,windows,False);
      if (windows->image.orphan)
        break;
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case SharpenCommand:
    {
      static char
        option[MaxTextExtent] = "0.0x1.0";

      double
        radius,
        sigma;

      /*
        Query user for sharpen radius.
      */
      (void) MagickXDialogWidget(display,windows,"Sharpen",
        "Enter the sharpen radius and standard deviation:",option);
      if (*option == '\0')
        break;
      /*
        Sharpen image scanlines.
      */
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      radius=0.0;
      sigma=1.0;
      (void) GetMagickDimension(option,&radius,&sigma,NULL,NULL);
      ReplaceImage(*image,SharpenImage(*image,radius,sigma,
        &(*image)->exception));
      MagickXSetCursorState(display,windows,False);
      if (windows->image.orphan)
        break;
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case BlurCommand:
    {
      static char
        option[MaxTextExtent] = "0.0x1.0";

      double
        radius,
        sigma;

      /*
        Query user for blur radius.
      */
      (void) MagickXDialogWidget(display,windows,"Blur",
        "Enter the blur radius and standard deviation:",option);
      if (*option == '\0')
        break;
      /*
        Blur an image.
      */
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      radius=0.0;
      sigma=1.0;
      (void) GetMagickDimension(option,&radius,&sigma,NULL,NULL);
      ReplaceImage(*image,BlurImage(*image,radius,sigma,
        &(*image)->exception));
      MagickXSetCursorState(display,windows,False);
      if (windows->image.orphan)
        break;
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case ThresholdCommand:
    {
      static char
        factor[MaxTextExtent];

      /*
        Query user for threshold value.
      */
      (void) sprintf(factor,"%lu",(unsigned long)(MaxRGB+1)/2);
      (void) MagickXDialogWidget(display,windows,"Threshold",
        "Enter threshold value:",factor);
      if (*factor == '\0')
        break;
      /*
        Threshold image.
      */
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      (void) ChannelThresholdImage(*image,factor);
      MagickXSetCursorState(display,windows,False);
      if (windows->image.orphan)
        break;
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case EdgeDetectCommand:
    {
      static char
        radius[MaxTextExtent] = "0";

      /*
        Query user for edge factor.
      */
      (void) MagickXDialogWidget(display,windows,"Detect Edges",
        "Enter the edge detect radius:",radius);
      if (*radius == '\0')
        break;
      /*
        Detect edge in image.
      */
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      ReplaceImage(*image,EdgeImage(*image,atof(radius),
        &(*image)->exception));
      MagickXSetCursorState(display,windows,False);
      if (windows->image.orphan)
        break;
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case SpreadCommand:
    {
      static char
        amount[MaxTextExtent] = "2";

      /*
        Query user for spread amount.
      */
      (void) MagickXDialogWidget(display,windows,"Spread",
        "Enter the displacement amount:",amount);
      if (*amount == '\0')
        break;
      /*
        Displace image pixels by a random amount.
      */
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      ReplaceImage(*image,SpreadImage(*image,atoi(amount),
        &(*image)->exception));
      MagickXSetCursorState(display,windows,False);
      if (windows->image.orphan)
        break;
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case ShadeCommand:
    {
      static char
        geometry[MaxTextExtent] = "30x30";

      double
        azimuth,
        elevation;
      
      /*
        Query user for the shade geometry.
      */
      status=MagickXDialogWidget(display,windows,"Shade",
        "Enter the azimuth and elevation of the light source:",geometry);
      if (*geometry == '\0')
        break;
      /*
        Shade image pixels.
      */
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      azimuth=30.0;
      elevation=30.0;
      (void) GetMagickDimension(geometry,&azimuth,&elevation,NULL,NULL);
      ReplaceImage(*image,ShadeImage(*image,(status ? True : False),
        azimuth,elevation,&(*image)->exception));
      MagickXSetCursorState(display,windows,False);
      if (windows->image.orphan)
        break;
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case RaiseCommand:
    {
      static char
        bevel_width[MaxTextExtent] = "10";

      /*
        Query user for bevel width.
      */
      (void) MagickXDialogWidget(display,windows,"Raise","Bevel width:",bevel_width);
      if (*bevel_width == '\0')
        break;
      /*
        Raise an image.
      */
      (void) MagickXMagickCommand(display,resource_info,windows,ApplyCommand,image);
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      (void) GetImageGeometry(*image,bevel_width,False,&rectangle);
      (void) RaiseImage(*image,&rectangle,True);
      MagickXSetCursorState(display,windows,False);
      if (windows->image.orphan)
        break;
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case SegmentCommand:
    {
      static char
        threshold[MaxTextExtent] = "1.0x1.5";

      double
        cluster_threshold,
        smoothing_threshold;

      /*
        Query user for smoothing threshold.
      */
      (void) MagickXDialogWidget(display,windows,"Segment","Smooth threshold:",
        threshold);
      if (*threshold == '\0')
        break;
      /*
        Segment an image.
      */
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      cluster_threshold=1.0;
      smoothing_threshold=1.5;
      (void) GetMagickDimension(threshold,&cluster_threshold,
                                &smoothing_threshold,NULL,NULL);
      (void) SegmentImage(*image,(*image)->colorspace,False,
        cluster_threshold,smoothing_threshold);
      MagickXSetCursorState(display,windows,False);
      if (windows->image.orphan)
        break;
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case SolarizeCommand:
    {
      static char
        factor[MaxTextExtent] = "60";

      /*
        Query user for solarize factor.
      */
      (void) MagickXDialogWidget(display,windows,"Solarize",
        "Enter the solarize factor (0 - 99.9%):",factor);
      if (*factor == '\0')
        break;
      /*
        Solarize image pixels.
      */
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      (void) SolarizeImage(*image,StringToDouble(factor,MaxRGB));
      MagickXSetCursorState(display,windows,False);
      if (windows->image.orphan)
        break;
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case SwirlCommand:
    {
      static char
        degrees[MaxTextExtent] = "60";

      /*
        Query user for swirl angle.
      */
      (void) MagickXDialogWidget(display,windows,"Swirl","Enter the swirl angle:",
        degrees);
      if (*degrees == '\0')
        break;
      /*
        Swirl image pixels about the center.
      */
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      ReplaceImage(*image,SwirlImage(*image,atof(degrees),
        &(*image)->exception));
      MagickXSetCursorState(display,windows,False);
      if (windows->image.orphan)
        break;
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case ImplodeCommand:
    {
      static char
        factor[MaxTextExtent] = "0.3";

      /*
        Query user for implode factor.
      */
      (void) MagickXDialogWidget(display,windows,"Implode",
        "Enter the implosion/explosion factor (-1.0 - 1.0):",factor);
      if (*factor == '\0')
        break;
      /*
        Implode image pixels about the center.
      */
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      ReplaceImage(*image,ImplodeImage(*image,atof(factor),
        &(*image)->exception));
      MagickXSetCursorState(display,windows,False);
      if (windows->image.orphan)
        break;
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case WaveCommand:
    {
      static char
        geometry[MaxTextExtent] = "25x150";

      double
        amplitude,
        wavelength;

      /*
        Query user for the shade geometry.
      */
      (void) MagickXDialogWidget(display,windows,"Wave",
        "Enter the amplitude and length of the wave:",geometry);
      if (*geometry == '\0')
        break;
      /*
        Shade image pixels.
      */
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      amplitude=25.0;
      wavelength=150.0;
      (void) GetMagickDimension(geometry,&amplitude,&wavelength,NULL,NULL);
      ReplaceImage(*image,WaveImage(*image,amplitude,wavelength,
        &(*image)->exception));
      MagickXSetCursorState(display,windows,False);
      if (windows->image.orphan)
        break;
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case OilPaintCommand:
    {
      static char
        radius[MaxTextExtent] = "0";

      /*
        Query user for circular neighborhood radius.
      */
      (void) MagickXDialogWidget(display,windows,"Oil Paint",
        "Enter the mask radius:",radius);
      if (*radius == '\0')
        break;
      /*
        OilPaint image scanlines.
      */
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      ReplaceImage(*image,OilPaintImage(*image,atof(radius),
        &(*image)->exception));
      MagickXSetCursorState(display,windows,False);
      if (windows->image.orphan)
        break;
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case CharcoalDrawCommand:
    {
      static char
        option[MaxTextExtent] = "0.0";

      double
        radius,
        sigma;

      /*
        Query user for charcoal radius.
      */
      (void) MagickXDialogWidget(display,windows,"Charcoal Draw",
        "Enter the charcoal radius:",option);
      if (*option == '\0')
        break;
      /*
        Charcoal the image.
      */
      (void) MagickXMagickCommand(display,resource_info,windows,ApplyCommand,image);
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      radius=0.0;
      sigma=1.0;
      (void) GetMagickDimension(option,&radius,&sigma,NULL,NULL);
      ReplaceImage(*image,CharcoalImage(*image,radius,sigma,
        &(*image)->exception));
      MagickXSetCursorState(display,windows,False);
      if (windows->image.orphan)
        break;
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case AnnotateCommand:
    {
      /*
        Annotate the image with text.
      */
      status=MagickXAnnotateEditImage(display,resource_info,windows,*image);
      if (status == False)
        {
          MagickXNoticeWidget(display,windows,"Unable to annotate X image",
            (*image)->filename);
          break;
        }
      break;
    }
    case DrawCommand:
    {
      /*
        Draw image.
      */
      status=MagickXDrawEditImage(display,resource_info,windows,image);
      if (status == False)
        {
          MagickXNoticeWidget(display,windows,"Unable to draw on the X image",
            (*image)->filename);
          break;
        }
      break;
    }
    case ColorCommand:
    {
      /*
        Color edit.
      */
      status=MagickXColorEditImage(display,resource_info,windows,image);
      if (status == False)
        {
          MagickXNoticeWidget(display,windows,"Unable to pixel edit X image",
            (*image)->filename);
          break;
        }
      break;
    }
    case MatteCommand:
    {
      /*
        Matte edit.
      */
      status=MagickXMatteEditImage(display,resource_info,windows,image);
      if (status == False)
        {
          MagickXNoticeWidget(display,windows,"Unable to matte edit X image",
            (*image)->filename);
          break;
        }
      break;
    }
    case CompositeCommand:
    {
      /*
        Composite image.
      */
      status=MagickXCompositeImage(display,resource_info,windows,*image);
      if (status == False)
        {
          MagickXNoticeWidget(display,windows,"Unable to composite X image",
            (*image)->filename);
          break;
        }
      break;
    }
    case AddBorderCommand:
    {
      static char
        geometry[MaxTextExtent] = "6x6";

      /*
        Query user for border color and geometry.
      */
      MagickXColorBrowserWidget(display,windows,"Select",color);
      if (*color == '\0')
        break;
      (void) MagickXDialogWidget(display,windows,"Add Border",
        "Enter border geometry:",geometry);
      if (*geometry == '\0')
        break;
      /*
        Add a border to the image.
      */
      (void) MagickXMagickCommand(display,resource_info,windows,ApplyCommand,image);
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      (void) QueryColorDatabase(color,&(*image)->border_color,
        &(*image)->exception);
      (void) GetImageGeometry(*image,geometry,False,&rectangle);
      ReplaceImage(*image,BorderImage(*image,&rectangle,
        &(*image)->exception));
      MagickXSetCursorState(display,windows,False);
      if (windows->image.orphan)
        break;
      windows->image.window_changes.width=(unsigned int) (*image)->columns;
      windows->image.window_changes.height=(unsigned int) (*image)->rows;
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case AddFrameCommand:
    {
      static char
        geometry[MaxTextExtent] = "6x6";

      FrameInfo
        frame_info;

      /*
        Query user for frame color and geometry.
      */
      MagickXColorBrowserWidget(display,windows,"Select",color);
      if (*color == '\0')
        break;
      (void) MagickXDialogWidget(display,windows,"Add Frame","Enter frame geometry:",
        geometry);
      if (*geometry == '\0')
        break;
      /*
        Surround image with an ornamental border.
      */
      (void) MagickXMagickCommand(display,resource_info,windows,ApplyCommand,image);
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      (void) QueryColorDatabase(color,&(*image)->matte_color,
        &(*image)->exception);
      (void) GetImageGeometry(*image,geometry,False,&rectangle);
      frame_info.width=rectangle.width;
      frame_info.height=rectangle.height;
      frame_info.outer_bevel=rectangle.x;
      frame_info.inner_bevel=rectangle.y;
      frame_info.x=(long) frame_info.width;
      frame_info.y=(long) frame_info.height;
      frame_info.width=(*image)->columns+2*frame_info.width;
      frame_info.height=(*image)->rows+2*frame_info.height;
      ReplaceImage(*image,FrameImage(*image,&frame_info,&(*image)->exception));
      MagickXSetCursorState(display,windows,False);
      if (windows->image.orphan)
        break;
      windows->image.window_changes.width=(unsigned int) (*image)->columns;
      windows->image.window_changes.height=(unsigned int) (*image)->rows;
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
      (void) MagickXConfigureImage(display,resource_info,windows,*image);
      break;
    }
    case CommentCommand:
    {
      const ImageAttribute
        *attribute;

      FILE
        *file;

      /*
        Edit image comment.
      */
      if (!AcquireTemporaryFileName(image_info->filename))
        {
          MagickXNoticeWidget(display,windows,"Unable to open temporary file:",
            image_info->filename);
          break;
        }
      attribute=GetImageAttribute(*image,"comment");
      if ((attribute != (const ImageAttribute *) NULL) &&
          (attribute->value != (char *) NULL))
        {
          register char
            *p;

          file=fopen(image_info->filename,"w");
          if (file == (FILE *) NULL)
            {
              (void) LiberateTemporaryFile(image_info->filename);
              MagickXNoticeWidget(display,windows,"Unable to edit image comment",
                image_info->filename);
              break;
            }
          for (p=attribute->value; *p != '\0'; p++)
            (void) fputc((int) *p,file);
          (void) fputc('\n',file);
          (void) fclose(file);
        }
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      status=InvokeDelegate(image_info,*image,"edit",(char *) NULL,
        &(*image)->exception);
      if (status == False)
        MagickXNoticeWidget(display,windows,"Unable to edit image comment",
          (char *) NULL);
      else
        {
          char
            command[MaxTextExtent];

          FormatString(command,"@%.1024s",image_info->filename);
          (void) SetImageAttribute(*image,"comment",(char *) NULL);
          (void) SetImageAttribute(*image,"comment",command);
          (*image)->taint=True;
        }
      (void) LiberateTemporaryFile(image_info->filename);
      MagickXSetCursorState(display,windows,False);
      break;
    }
    case LaunchCommand:
    {
      /*
        Launch program.
      */
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      if (!AcquireTemporaryFileName(filename))
        {
          MagickXNoticeWidget(display,windows,"Unable to open temporary file:",
            filename);
          break;
        }
      FormatString((*image)->filename,"launch:%s",filename);
      status=WriteImage(image_info,*image);
      if (status == False)
        {
          (void) LiberateTemporaryFile(filename);
          MagickXNoticeWidget(display,windows,"Unable to launch image editor",
            (char *) NULL);
        }
      else
        {
          nexus=ReadImage(resource_info->image_info,&(*image)->exception);
          if ((*image)->exception.severity != UndefinedException)
            MagickError2((*image)->exception.severity,
              (*image)->exception.reason,(*image)->exception.description);
          MagickXClientMessage(display,windows->image.id,windows->im_protocols,
            windows->im_next_image,CurrentTime);
        }
      (void) LiberateTemporaryFile(filename);
      MagickXSetCursorState(display,windows,False);
      break;
    }
    case RegionofInterestCommand:
    {
      /*
        Apply an image processing technique to a region of interest.
      */
      (void) MagickXROIImage(display,resource_info,windows,image);
      break;
    }
    case InfoCommand:
      break;
    case ZoomCommand:
    {
      /*
        Zoom image.
      */
      if (windows->magnify.mapped)
        (void) XRaiseWindow(display,windows->magnify.id);
      else
        {
          /*
            Make magnify image.
          */
          MagickXSetCursorState(display,windows,True);
          (void) XMapRaised(display,windows->magnify.id);
          MagickXSetCursorState(display,windows,False);
        }
      break;
    }
    case ShowPreviewCommand:
    {
      static char
        preview_type[MaxTextExtent] = "Gamma";

      register int
        i;

      /*
        Select preview type from menu.
      */
      MagickXListBrowserWidget(display,windows,&windows->widget,PreviewTypes,
        "Preview","Select an enhancement, effect, or F/X:",preview_type);
      if (*preview_type == '\0')
        break;
      for (i=0; PreviewTypes[i] != (char *) NULL; i++)
        if (LocaleCompare(PreviewTypes[i],preview_type) == 0)
          break;
      if (PreviewTypes[i] == (char *) NULL)
        {
          MagickXNoticeWidget(display,windows,"unknown preview type",preview_type);
          break;
        }
      /*
        Show image preview.
      */
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      image_info->preview_type=(PreviewType) (i+1);
      image_info->group=(long) windows->image.id;
      (void) SetImageAttribute(*image,"label",(char *) NULL);
      (void) SetImageAttribute(*image,"label","Preview");
      if (!AcquireTemporaryFileName(filename))
        {
          MagickXNoticeWidget(display,windows,"Unable to open temporary file:",
            filename);
          break;
        }
      FormatString((*image)->filename,"preview:%s",filename);
      status=WriteImage(image_info,*image);
      FormatString((*image)->filename,"show:%s",filename);
      status=WriteImage(image_info,*image);
      if (status == False)
        {
          MagickXNoticeWidget(display,windows,"Unable to show image preview",
            (*image)->filename);
        }
      MagickXDelay(display,1500);
      MagickXSetCursorState(display,windows,False);
      break;
    }
    case ShowHistogramCommand:
    {
      /*
        Show image histogram.
      */
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      image_info->group=(long) windows->image.id;
      (void) SetImageAttribute(*image,"label",(char *) NULL);
      (void) SetImageAttribute(*image,"label","Histogram");
      if (!AcquireTemporaryFileName(filename))
        {
          MagickXNoticeWidget(display,windows,"Unable to open temporary file:",
            filename);
          break;
        }
      FormatString((*image)->filename,"histogram:%s",filename);
      status=WriteImage(image_info,*image);
      FormatString((*image)->filename,"show:%s",filename);
      status=WriteImage(image_info,*image);
      if (status == False)
        MagickXNoticeWidget(display,windows,"Unable to show histogram",
          (*image)->filename);
      MagickXDelay(display,1500);
      MagickXSetCursorState(display,windows,False);
      break;
    }
    case ShowMatteCommand:
    {
      if (!(*image)->matte)
        {
          MagickXNoticeWidget(display,windows,
            "Image does not have any matte information",(*image)->filename);
          break;
        }
      /*
        Show image matte.
      */
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      image_info->group=(long) windows->image.id;
      (void) SetImageAttribute(*image,"label",(char *) NULL);
      (void) SetImageAttribute(*image,"label","Matte");
      if (!AcquireTemporaryFileName(filename))
        {
          MagickXNoticeWidget(display,windows,"Unable to open temporary file:",
            filename);
          break;
        }
      FormatString((*image)->filename,"matte:%s",filename);
      status=WriteImage(image_info,*image);
      FormatString((*image)->filename,"show:%s",filename);
      status=WriteImage(image_info,*image);
      if (status == False)
        MagickXNoticeWidget(display,windows,"Unable to show matte",
          (*image)->filename);
      MagickXDelay(display,1500);
      MagickXSetCursorState(display,windows,False);
      break;
    }
    case BackgroundCommand:
    {
      /*
        Background image.
      */
      status=MagickXBackgroundImage(display,resource_info,windows,image);
      if (status == False)
        break;
      nexus=CloneImage(*image,0,0,True,&(*image)->exception);
      if (nexus != (Image *) NULL)
        MagickXClientMessage(display,windows->image.id,windows->im_protocols,
          windows->im_next_image,CurrentTime);
      break;
    }
    case SlideShowCommand:
    {
      static char
        delay[MaxTextExtent] = "5";

      /*
        Display next image after pausing.
      */
      (void) MagickXDialogWidget(display,windows,"Slide Show",
        "Pause how many 1/100ths of a second between images:",delay);
      if (*delay == '\0')
        break;
      resource_info->delay=atoi(delay);
      MagickXClientMessage(display,windows->image.id,windows->im_protocols,
        windows->im_next_image,CurrentTime);
      break;
    }
    case PreferencesCommand:
    {
      /*
        Set user preferences.
      */
      status=MagickXPreferencesWidget(display,resource_info,windows);
      if (status == False)
        break;
      nexus=CloneImage(*image,0,0,True,&(*image)->exception);
      if (nexus != (Image *) NULL)
        MagickXClientMessage(display,windows->image.id,windows->im_protocols,
          windows->im_next_image,CurrentTime);
      break;
    }
    case HelpCommand:
    {
      /*
        User requested help.
      */
      MagickXTextViewWidget(display,resource_info,windows,False,
        "Help Viewer - Display",DisplayHelp);
      break;
    }
    case BrowseDocumentationCommand:
    {
      Atom
        mozilla_atom;

      Window
        mozilla_window,
        root_window;

      /*
        Browse the GraphicsMagick documentation.
      */
      root_window=XRootWindow(display,XDefaultScreen(display));
      mozilla_atom=XInternAtom(display,"_MOZILLA_VERSION",False);
      mozilla_window=MagickXWindowByProperty(display,root_window,mozilla_atom);
      if (mozilla_window != (Window) NULL)
        {
          char
            command[MaxTextExtent];

          /*
            Display documentation using Netscape remote control.
          */
          FormatString(command,"openURL(%.1024s,new-window,noraise)",
            "http://www.graphicsmagick.org/");
          mozilla_atom=XInternAtom(display,"_MOZILLA_COMMAND",False);
          (void) XChangeProperty(display,mozilla_window,mozilla_atom,XA_STRING,
            8,PropModeReplace,(unsigned char *) command,(int) strlen(command));
          MagickXSetCursorState(display,windows,False);
          break;
        }
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      status=InvokeDelegate(image_info,*image,"browse",(char *) NULL,
        &(*image)->exception);
      if (status == False)
        MagickXNoticeWidget(display,windows,"Unable to browse documentation",
          (char *) NULL);
      MagickXDelay(display,1500);
      MagickXSetCursorState(display,windows,False);
      break;
    }
    case VersionCommand:
    {
      MagickXNoticeWidget(display,windows,GetMagickVersion((unsigned long *) NULL),
        GetMagickCopyright());
      break;
    }
    case SaveToUndoBufferCommand:
      break;
    default:
    {
      (void) XBell(display,0);
      break;
    }
  }
  DestroyImageInfo(image_info);
  return(nexus);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k X M a g n i f y I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickXMagnifyImage magnifies portions of the image as indicated
%  by the pointer.  The magnified portion is displayed in a separate window.
%
%  The format of the MagickXMagnifyImage method is:
%
%      void MagickXMagnifyImage(Display *display,MagickXWindows *windows,XEvent *event)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o windows: Specifies a pointer to a MagickXWindows structure.
%
%    o event: Specifies a pointer to a XEvent structure.  If it is NULL,
%      the entire image is refreshed.
%
%
*/
static void MagickXMagnifyImage(Display *display,MagickXWindows *windows,XEvent *event)
{
  char
    text[MaxTextExtent];

  register int
    x,
    y;

  unsigned long
    state;

  /*
    Update magnified image until the mouse button is released.
  */
  (void) XDefineCursor(display,windows->image.id,windows->magnify.cursor);
  state=DefaultState;
  x=event->xbutton.x;
  y=event->xbutton.y;
  windows->magnify.x=windows->image.x+x;
  windows->magnify.y=windows->image.y+y;
  do
  {
    /*
      Map and unmap Info widget as text cursor crosses its boundaries.
    */
    if (windows->info.mapped)
      {
        if ((x < (int) (windows->info.x+windows->info.width)) &&
            (y < (int) (windows->info.y+windows->info.height)))
          (void) XWithdrawWindow(display,windows->info.id,windows->info.screen);
      }
    else
      if ((x > (int) (windows->info.x+windows->info.width)) ||
          (y > (int) (windows->info.y+windows->info.height)))
        (void) XMapWindow(display,windows->info.id);
    if (windows->info.mapped)
      {
        /*
          Display pointer position.
        */
        FormatString(text," %+d%+d ",windows->magnify.x,windows->magnify.y);
        MagickXInfoWidget(display,windows,text);
      }
    /*
      Wait for next event.
    */
    MagickXScreenEvent(display,windows,event);
    switch (event->type)
    {
      case ButtonPress:
        break;
      case ButtonRelease:
      {
        /*
          User has finished magnifying image.
        */
        x=event->xbutton.x;
        y=event->xbutton.y;
        state|=ExitState;
        break;
      }
      case Expose:
        break;
      case MotionNotify:
      {
        x=event->xmotion.x;
        y=event->xmotion.y;
        break;
      }
      default:
        break;
    }
    /*
      Check boundary conditions.
    */
    if (x < 0)
      x=0;
    else
      if (x >= (int) windows->image.width)
        x=windows->image.width-1;
    if (y < 0)
      y=0;
    else
     if (y >= (int) windows->image.height)
       y=windows->image.height-1;
  } while (!(state & ExitState));
  /*
    Display magnified image.
  */
  MagickXSetCursorState(display,windows,False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k X M a g n i f y W i n d o w C o m m a n d                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickXMagnifyWindowCommand moves the image within an Magnify window by
%  one pixel as specified by the key symbol.
%
%  The format of the MagickXMagnifyWindowCommand method is:
%
%      void MagickXMagnifyWindowCommand(Display *display,MagickXWindows *windows,
%        const unsigned int state,const KeySym key_symbol)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o windows: Specifies a pointer to a MagickXWindows structure.
%
%    o state: key mask.
%
%    o key_symbol: Specifies a KeySym which indicates which side of the image
%      to trim.
%
%
*/
static void MagickXMagnifyWindowCommand(Display *display,MagickXWindows *windows,
  const unsigned int state,const KeySym key_symbol)
{
  unsigned int
    quantum;

  /*
    User specified a magnify factor or position.
  */
  quantum=1;
  if (state & Mod1Mask)
    quantum=10;
  switch ((int) key_symbol)
  {
    case QuitCommand:
    {
      (void) XWithdrawWindow(display,windows->magnify.id,
        windows->magnify.screen);
      break;
    }
    case XK_Home:
    case XK_KP_Home:
    {
      windows->magnify.x=windows->image.width/2;
      windows->magnify.y=windows->image.height/2;
      break;
    }
    case XK_Left:
    case XK_KP_Left:
    {
      if (windows->magnify.x > 0)
        windows->magnify.x-=quantum;
      break;
    }
    case XK_Up:
    case XK_KP_Up:
    {
      if (windows->magnify.y > 0)
        windows->magnify.y-=quantum;
      break;
    }
    case XK_Right:
    case XK_KP_Right:
    {
      if (windows->magnify.x < (int) (windows->image.ximage->width-1))
        windows->magnify.x+=quantum;
      break;
    }
    case XK_Down:
    case XK_KP_Down:
    {
      if (windows->magnify.y < (int) (windows->image.ximage->height-1))
        windows->magnify.y+=quantum;
      break;
    }
    case XK_0:
    case XK_1:
    case XK_2:
    case XK_3:
    case XK_4:
    case XK_5:
    case XK_6:
    case XK_7:
    case XK_8:
    case XK_9:
    {
      windows->magnify.data=(key_symbol-XK_0);
      break;
    }
    case XK_KP_0:
    case XK_KP_1:
    case XK_KP_2:
    case XK_KP_3:
    case XK_KP_4:
    case XK_KP_5:
    case XK_KP_6:
    case XK_KP_7:
    case XK_KP_8:
    case XK_KP_9:
    {
      windows->magnify.data=(key_symbol-XK_KP_0);
      break;
    }
    default:
      break;
  }
  MagickXMakeMagnifyImage(display,windows);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k X M a k e P a n I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickXMakePanImage creates a thumbnail of the image and displays it in
%  the Pan icon window.
%
%  The format of the MagickXMakePanImage method is:
%
%        void MagickXMakePanImage(Display *display,MagickXResourceInfo *resource_info,
%          MagickXWindows *windows,Image *image)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 MagickXResourceInfo structure.
%
%    o windows: Specifies a pointer to a MagickXWindows structure.
%
%    o image: Specifies a pointer to an Image structure;  returned from
%      ReadImage.
%
%
*/
static void MagickXMakePanImage(Display *display,MagickXResourceInfo *resource_info,
  MagickXWindows *windows,Image *image)
{
  unsigned int
    status;

  /*
    Display hourglass cursor if progress indication enabled.
  */
  if (resource_info->image_info->progress)
    MagickXSetCursorState(display,windows,True);
  MagickXCheckRefreshWindows(display,windows);
  windows->pan.x=windows->image.x;
  windows->pan.y=windows->image.y;
  /*
    Create and display image for panning icon.
  */
  status=MagickXMakeImage(display,resource_info,&windows->pan,image,
    windows->pan.width,windows->pan.height);
  if (status == False)
    MagickError2(XServerError,image->exception.reason,(char *) NULL);
  (void) XSetWindowBackgroundPixmap(display,windows->pan.id,
    windows->pan.pixmap);
  (void) XClearWindow(display,windows->pan.id);
  MagickXDrawPanRectangle(display,windows);
  MagickXSetCursorState(display,windows,False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k X M a t t a E d i t I m a g e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickXMatteEditImage allows the user to interactively change
%  the Matte channel of an image.  If the image is PseudoClass it is promoted
%  to DirectClass before the matte information is stored.
%
%  The format of the MagickXMatteEditImage method is:
%
%      unsigned int MagickXMatteEditImage(Display *display,
%        MagickXResourceInfo *resource_info,MagickXWindows *windows,Image **image)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 MagickXResourceInfo structure.
%
%    o windows: Specifies a pointer to a MagickXWindows structure.
%
%    o image: Specifies a pointer to an Image structure; returned from
%      ReadImage.
%
*/
static unsigned int MagickXMatteEditImage(Display *display,
  MagickXResourceInfo *resource_info,MagickXWindows *windows,Image **image)
{
  static char
    matte[MaxTextExtent] = "0";

  static const char
    *MatteEditMenu[]=
    {
      "Method",
      "Border Color",
      "Fuzz",
      "Matte Value",
      "Undo",
      "Help",
      "Dismiss",
      (char *) NULL
    };

  static const ModeType
    MatteEditCommands[]=
    {
      MatteEditMethod,
      MatteEditBorderCommand,
      MatteEditFuzzCommand,
      MatteEditValueCommand,
      MatteEditUndoCommand,
      MatteEditHelpCommand,
      MatteEditDismissCommand
    };

  static PaintMethod
    method = PointMethod;

  static XColor
    border_color = { 0, 0, 0, 0, 0, 0 };  /* Also fill 'pad' field */

  char
    command[MaxTextExtent],
    text[MaxTextExtent];

  Cursor
    cursor;

  int
    entry,
    id,
    x,
    x_offset,
    y,
    y_offset;

  register int
    i;

  register PixelPacket
    *q;

  unsigned int
    height,
    width;

  unsigned long
    state;

  XEvent
    event;

  /*
    Map Command widget.
  */
  (void) CloneString(&windows->command.name,"Matte Edit");
  windows->command.data=4;
  (void) MagickXCommandWidget(display,windows,MatteEditMenu,(XEvent *) NULL);
  (void) XMapRaised(display,windows->command.id);
  MagickXClientMessage(display,windows->image.id,windows->im_protocols,
    windows->im_update_widget,CurrentTime);
  /*
    Make cursor.
  */
  cursor=MagickXMakeCursor(display,windows->image.id,windows->map_info->colormap,
    resource_info->background_color,resource_info->foreground_color);
  (void) XDefineCursor(display,windows->image.id,cursor);
  /*
    Track pointer until button 1 is pressed.
  */
  MagickXQueryPosition(display,windows->image.id,&x,&y);
  (void) XSelectInput(display,windows->image.id,
    windows->image.attributes.event_mask | PointerMotionMask);
  state=DefaultState;
  do
  {
    if (windows->info.mapped)
      {
        /*
          Display pointer position.
        */
        FormatString(text," %+d%+d ",x+windows->image.x,y+windows->image.y);
        MagickXInfoWidget(display,windows,text);
      }
    /*
      Wait for next event.
    */
    MagickXScreenEvent(display,windows,&event);
    if (event.xany.window == windows->command.id)
      {
        /*
          Select a command from the Command widget.
        */
        id=MagickXCommandWidget(display,windows,MatteEditMenu,&event);
        if (id < 0)
          {
            (void) XDefineCursor(display,windows->image.id,cursor);
            continue;
          }
        switch (MatteEditCommands[id])
        {
          case MatteEditMethod:
          {
            static const char
              *MethodMenu[]=
              {
                "point",
                "replace",
                "floodfill",
                "filltoborder",
                "reset",
                (char *) NULL,
              };

            /*
              Select a method from the pop-up menu.
            */
            entry=
              MagickXMenuWidget(display,windows,MatteEditMenu[id],MethodMenu,command);
            if (entry >= 0)
              method=(PaintMethod) entry;
            break;
          }
          case MatteEditBorderCommand:
          {
            char
              *ColorMenu[MaxNumberPens];

            int
              pen_number;

            /*
              Initialize menu selections.
            */
            for (i=0; i < (int) (MaxNumberPens-2); i++)
              ColorMenu[i]=resource_info->pen_colors[i];
            ColorMenu[MaxNumberPens-2]=(char *) "Browser...";
            ColorMenu[MaxNumberPens-1]=(char *) NULL;
            /*
              Select a pen color from the pop-up menu.
            */
            pen_number=MagickXMenuWidget(display,windows,MatteEditMenu[id],
              (const char **) ColorMenu,command);
            if (pen_number < 0)
              break;
            if (pen_number == (MaxNumberPens-2))
              {
                static char
                  color_name[MaxTextExtent] = "gray";

                /*
                  Select a pen color from a dialog.
                */
                resource_info->pen_colors[pen_number]=color_name;
                MagickXColorBrowserWidget(display,windows,"Select",color_name);
                if (*color_name == '\0')
                  break;
              }
            /*
              Set border color.
            */
            (void) XParseColor(display,windows->map_info->colormap,
              resource_info->pen_colors[pen_number],&border_color);
            break;
          }
          case MatteEditFuzzCommand:
          {
            static char
              fuzz[MaxTextExtent];

            static const char
              *FuzzMenu[]=
              {
                "0%",
                "2%",
                "5%",
                "10%",
                "15%",
                "Dialog...",
                (char *) NULL,
              };

            /*
              Select a command from the pop-up menu.
            */
            entry=MagickXMenuWidget(display,windows,MatteEditMenu[id],FuzzMenu,
              command);
            if (entry < 0)
              break;
            if (entry != 5)
              {
                (*image)->fuzz=StringToDouble(FuzzMenu[entry],MaxRGB);
                break;
              }
            (void) strcpy(fuzz,"20%");
            (void) MagickXDialogWidget(display,windows,"Ok",
              "Enter fuzz factor (0.0 - 99.9%):",fuzz);
            if (*fuzz == '\0')
              break;
            (void) strcat(fuzz,"%");
            (*image)->fuzz=StringToDouble(fuzz,MaxRGB);
            break;
          }
          case MatteEditValueCommand:
          {
            static char
              message[MaxTextExtent];

            static const char
              *MatteMenu[]=
              {
                "Opaque",
                "Transparent",
                "Dialog...",
                (char *) NULL,
              };

            /*
              Select a command from the pop-up menu.
            */
            entry=MagickXMenuWidget(display,windows,MatteEditMenu[id],MatteMenu,
              command);
            if (entry < 0)
              break;
            if (entry != 2)
              {
                FormatString(matte,"%lu",(unsigned long) OpaqueOpacity);
                if (LocaleCompare(MatteMenu[entry],"Transparent") == 0)
                  FormatString(matte,"%lu",(unsigned long) TransparentOpacity);
                break;
              }
            FormatString(message,"Enter matte value (0 - %lu):",(unsigned long) MaxRGB);
            (void) MagickXDialogWidget(display,windows,"Matte",message,matte);
            if (*matte == '\0')
              break;
            break;
          }
          case MatteEditUndoCommand:
          {
            (void) MagickXMagickCommand(display,resource_info,windows,UndoCommand,
              image);
            break;
          }
          case MatteEditHelpCommand:
          {
            MagickXTextViewWidget(display,resource_info,windows,False,
              "Help Viewer - Matte Edit",ImageMatteEditHelp);
            break;
          }
          case MatteEditDismissCommand:
          {
            /*
              Prematurely exit.
            */
            state|=EscapeState;
            state|=ExitState;
            break;
          }
          default:
            break;
        }
        (void) XDefineCursor(display,windows->image.id,cursor);
        continue;
      }
    switch (event.type)
    {
      case ButtonPress:
      {
        if (event.xbutton.button != Button1)
          break;
        if ((event.xbutton.window != windows->image.id) &&
            (event.xbutton.window != windows->magnify.id))
          break;
        /*
          Update matte data.
        */
        x=event.xbutton.x;
        y=event.xbutton.y;
        (void) MagickXMagickCommand(display,resource_info,windows,
          SaveToUndoBufferCommand,image);
        state|=UpdateConfigurationState;
        break;
      }
      case ButtonRelease:
      {
        if (event.xbutton.button != Button1)
          break;
        if ((event.xbutton.window != windows->image.id) &&
            (event.xbutton.window != windows->magnify.id))
          break;
        /*
          Update colormap information.
        */
        x=event.xbutton.x;
        y=event.xbutton.y;
        MagickXConfigureImageColormap(display,resource_info,windows,*image);
        (void) MagickXConfigureImage(display,resource_info,windows,*image);
        MagickXInfoWidget(display,windows,text);
        (void) XDefineCursor(display,windows->image.id,cursor);
        state&=(~UpdateConfigurationState);
        break;
      }
      case Expose:
        break;
      case KeyPress:
      {
        char
          command[MaxTextExtent];

        KeySym
          key_symbol;

        if (event.xkey.window == windows->magnify.id)
          {
            Window
              window;

            window=windows->magnify.id;
            while (XCheckWindowEvent(display,window,KeyPressMask,&event));
          }
        if (event.xkey.window != windows->image.id)
          break;
        /*
          Respond to a user key press.
        */
        (void) XLookupString((XKeyEvent *) &event.xkey,command,sizeof(command),
          &key_symbol,(XComposeStatus *) NULL);
        switch ((int) key_symbol)
        {
          case XK_Escape:
          case XK_F20:
          {
            /*
              Prematurely exit.
            */
            state|=ExitState;
            break;
          }
          case XK_F1:
          case XK_Help:
          {
            MagickXTextViewWidget(display,resource_info,windows,False,
              "Help Viewer - Matte Edit",ImageMatteEditHelp);
            break;
          }
          default:
          {
            (void) XBell(display,0);
            break;
          }
        }
        break;
      }
      case MotionNotify:
      {
        /*
          Map and unmap Info widget as cursor crosses its boundaries.
        */
        x=event.xmotion.x;
        y=event.xmotion.y;
        if (windows->info.mapped)
          {
            if ((x < (int) (windows->info.x+windows->info.width)) &&
                (y < (int) (windows->info.y+windows->info.height)))
              (void) XWithdrawWindow(display,windows->info.id,
                windows->info.screen);
          }
        else
          if ((x > (int) (windows->info.x+windows->info.width)) ||
              (y > (int) (windows->info.y+windows->info.height)))
            (void) XMapWindow(display,windows->info.id);
        break;
      }
      default:
        break;
    }
    if (event.xany.window == windows->magnify.id)
      {
        x=windows->magnify.x-windows->image.x;
        y=windows->magnify.y-windows->image.y;
      }
    x_offset=x;
    y_offset=y;
    if (state & UpdateConfigurationState)
      {
        int
          x,
          y;

        /*
          Matte edit is relative to image configuration.
        */
        (void) XClearArea(display,windows->image.id,x_offset,y_offset,1,1,True);
        XPutPixel(windows->image.ximage,x_offset,y_offset,
          windows->pixel_info->background_color.pixel);
        width=(unsigned int) (*image)->columns;
        height=(unsigned int) (*image)->rows;
        x=0;
        y=0;
        if (windows->image.crop_geometry != (char *) NULL)
          (void) XParseGeometry(windows->image.crop_geometry,&x,&y,
            &width,&height);
        x_offset=
          width*(windows->image.x+x_offset)/windows->image.ximage->width+x;
        y_offset=
          height*(windows->image.y+y_offset)/windows->image.ximage->height+y;
        if ((x_offset < 0) || (y_offset < 0))
          continue;
        if ((x_offset >= (int) (*image)->columns) ||
            (y_offset >= (int) (*image)->rows))
          continue;
        (void) SetImageType(*image,TrueColorMatteType);
        switch (method)
        {
          case PointMethod:
          default:
          {
            /*
              Update matte information using point algorithm.
            */
            q=GetImagePixels(*image,x_offset,y_offset,1,1);
            if (q == (PixelPacket *) NULL)
              break;
            q->opacity=(Quantum) atol(matte);
            (void) SyncImagePixels(*image);
            break;
          }
          case ReplaceMethod:
          {
            PixelPacket
              target;

            /*
              Update matte information using replace algorithm.
            */
            (void) AcquireOnePixelByReference(*image,&target,x_offset,y_offset,&((*image)->exception));
            for (y=0; y < (long) (*image)->rows; y++)
            {
              q=GetImagePixels(*image,0,y,(*image)->columns,1);
              if (q == (PixelPacket *) NULL)
                break;
              for (x=0; x < (int) (*image)->columns; x++)
              {
                if (FuzzyColorMatch(q,&target,(*image)->fuzz))
                  q->opacity=(Quantum) atol(matte);
                q++;
              }
              if (!SyncImagePixels(*image))
                break;
            }
            break;
          }
          case FloodfillMethod:
          case FillToBorderMethod:
          {
            PixelPacket
              target;

            /*
              Update matte information using floodfill algorithm.
            */
            (void) AcquireOnePixelByReference(*image,&target,x_offset,y_offset,&((*image)->exception));
            if (method == FillToBorderMethod)
              {
                target.red=ScaleShortToQuantum(border_color.red);
                target.green=ScaleShortToQuantum(border_color.green);
                target.blue=ScaleShortToQuantum(border_color.blue);
              }
            (void) MatteFloodfillImage(*image,target,atoi(matte),x_offset,
              y_offset,method);
            break;
          }
          case ResetMethod:
          {
            /*
              Update matte information using reset algorithm.
            */
            (void) SetImageType(*image,TrueColorType);
            for (y=0; y < (long) (*image)->rows; y++)
            {
              q=SetImagePixels(*image,0,y,(*image)->columns,1);
              if (q == (PixelPacket *) NULL)
                break;
              for (x=0; x < (int) (*image)->columns; x++)
              {
                q->opacity=(Quantum) atol(matte);
                q++;
              }
              if (!SyncImagePixels(*image))
                break;
            }
            if (atol(matte) == OpaqueOpacity)
              (*image)->matte=False;
            break;
          }
        }
        state&=(~UpdateConfigurationState);
      }
  } while (!(state & ExitState));
  (void) XSelectInput(display,windows->image.id,
    windows->image.attributes.event_mask);
  MagickXSetCursorState(display,windows,False);
  (void) XFreeCursor(display,cursor);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k X O p e n I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickXOpenImage loads an image from a file.
%
%  The format of the MagickXOpenImage method is:
%
%     Image *MagickXOpenImage(Display *display,MagickXResourceInfo *resource_info,
%       MagickXWindows *windows,const unsigned int command)
%
%  A description of each parameter follows:
%
%    o nexus: Method MagickXOpenImage returns an image if can be loaded
%      successfully.  Otherwise a null image is returned.
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 MagickXResourceInfo structure.
%
%    o windows: Specifies a pointer to a MagickXWindows structure.
%
%    o command: A value other than zero indicates that the file is selected
%      from the command line argument list.
%
%
*/
static Image *MagickXOpenImage(Display *display,MagickXResourceInfo *resource_info,
  MagickXWindows *windows,const unsigned int command)
{
  ExceptionInfo
    exception;

  Image
    *nexus;

  ImageInfo
    *image_info;

  MonitorHandler
    handler;

  static char
    filename[MaxTextExtent] = "\0";

  /*
    Request file name from user.
  */
  if (!command)
    MagickXFileBrowserWidget(display,windows,"Open",filename);
  else
    {
      char
        **filelist,
        **files;

      int
        count,
        status;

      register int
        i,
        j;

      /*
        Select next image from the command line.
      */
      status=XGetCommand(display,windows->image.id,&files,&count);
      if (!status)
        {
          MagickError(XServerError,UnableToGetProperty,
            MagickMsg(ResourceLimitError,UnableToSelectImage));
          return((Image *) NULL);
        }
      filelist=MagickAllocateMemory(char **,count*sizeof(char *));
      if (filelist == (char **) NULL)
        {
          MagickError3(ResourceLimitError,MemoryAllocationFailed,
            UnableToSelectImage);
          (void) XFreeStringList(files);
          return((Image *) NULL);
        }
      j=0;
      for (i=1; i < count; i++)
        if (*files[i] != '-')
          filelist[j++]=files[i];
      filelist[j]=(char *) NULL;
      MagickXListBrowserWidget(display,windows,&windows->widget,
        (const char **) filelist,"Load","Select Image to Load:",filename);
      MagickFreeMemory(filelist);
      (void) XFreeStringList(files);
    }
  if (*filename == '\0')
    return((Image *) NULL);
  image_info=CloneImageInfo(resource_info->image_info);
  (void) strlcpy(image_info->filename,filename,MaxTextExtent);
  GetExceptionInfo(&exception);
  (void) SetImageInfo(image_info,False,&exception);
  if (LocaleCompare(image_info->magick,"X") == 0)
    {
      char
        seconds[MaxTextExtent];

      /*
        User may want to delay the X server screen grab.
      */
      (void) strcpy(seconds,"0");
      (void) MagickXDialogWidget(display,windows,"Grab","Enter any delay in seconds:",
        seconds);
      if (*seconds == '\0')
        return((Image *) NULL);
      MagickXDelay(display,1000*atol(seconds));
    }
  if ((LocaleCompare(image_info->magick,"CMYK") == 0) ||
      (LocaleCompare(image_info->magick,"GRAY") == 0) ||
      (LocaleCompare(image_info->magick,"MAP") == 0) ||
      (LocaleCompare(image_info->magick,"Matte") == 0) ||
      (LocaleCompare(image_info->magick,"RGB") == 0) ||
      (LocaleCompare(image_info->magick,"RGBA") == 0) ||
      (LocaleCompare(image_info->magick,"TEXT") == 0) ||
      (LocaleCompare(image_info->magick,"TILE") == 0) ||
      (LocaleCompare(image_info->magick,"UYVY") == 0) ||
      (LocaleCompare(image_info->magick,"XC") == 0) ||
      (LocaleCompare(image_info->magick,"YUV") == 0))
    {
      char
        geometry[MaxTextExtent];

      /*
        Request image size from the user.
      */
      (void) strcpy(geometry,"512x512");
      if (image_info->size != (char *) NULL)
        (void) strlcpy(geometry,image_info->size,MaxTextExtent);
      (void) MagickXDialogWidget(display,windows,"Load","Enter the image geometry:",
        geometry);
      (void) CloneString(&image_info->size,geometry);
    }
  /*
    Load the image.
  */
  MagickXSetCursorState(display,windows,True);
  MagickXCheckRefreshWindows(display,windows);
  (void) strlcpy(image_info->filename,filename,MaxTextExtent);
  handler=(MonitorHandler) NULL;
  if (LocaleCompare(image_info->magick,"X") == 0)
    handler=SetMonitorHandler((MonitorHandler) NULL);
  nexus=ReadImage(image_info,&exception);
  if (exception.severity != UndefinedException)
    CatchException(&exception);
  if (LocaleCompare(image_info->magick,"X") == 0)
    (void) SetMonitorHandler(handler);
  MagickXSetCursorState(display,windows,False);
  if (nexus != (Image *) NULL)
    MagickXClientMessage(display,windows->image.id,windows->im_protocols,
      windows->im_next_image,CurrentTime);
  else
    {
      char
        *text,
        **textlist;

      size_t
        length;

      /*
        Unknown image format.
      */
      text=(char *) FileToBlob(filename,&length,&exception);
      if (text == (char *) NULL)
        return((Image *) NULL);
      textlist=StringToList(text);
      if (textlist != (char **) NULL)
        {
          char
            title[MaxTextExtent];

          register int
            i;

          FormatString(title,"Unknown format: %.1024s",filename);
          MagickXTextViewWidget(display,resource_info,windows,True,title,
            (const char **) textlist);
          for (i=0; textlist[i] != (char *) NULL; i++)
            MagickFreeMemory(textlist[i]);
          MagickFreeMemory(textlist);
        }
      MagickFreeMemory(text);
    }
  DestroyExceptionInfo(&exception);
  DestroyImageInfo(image_info);
  return(nexus);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k X P a n I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickXPanImage pans the image until the mouse button is released.
%
%  The format of the MagickXPanImage method is:
%
%      void MagickXPanImage(Display *display,MagickXWindows *windows,XEvent *event)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o windows: Specifies a pointer to a MagickXWindows structure.
%
%    o event: Specifies a pointer to a XEvent structure.  If it is NULL,
%      the entire image is refreshed.
%
*/
static void MagickXPanImage(Display *display,MagickXWindows *windows,XEvent *event)
{
  char
    text[MaxTextExtent];

  Cursor
    cursor;

  double
    x_factor,
    y_factor;

  RectangleInfo
    pan_info;

  unsigned long
    state;

  /*
    Define cursor.
  */
  if ((windows->image.ximage->width > (int) windows->image.width) &&
      (windows->image.ximage->height > (int) windows->image.height))
    cursor=XCreateFontCursor(display,XC_fleur);
  else
    if (windows->image.ximage->width > (int) windows->image.width)
      cursor=XCreateFontCursor(display,XC_sb_h_double_arrow);
    else
      if (windows->image.ximage->height > (int) windows->image.height)
        cursor=XCreateFontCursor(display,XC_sb_v_double_arrow);
      else
        cursor=XCreateFontCursor(display,XC_arrow);
  (void) XDefineCursor(display,windows->pan.id,cursor);
  /*
    Pan image as pointer moves until the mouse button is released.
  */
  x_factor=(double) windows->image.ximage->width/windows->pan.width;
  y_factor=(double) windows->image.ximage->height/windows->pan.height;
  pan_info.x=0;
  pan_info.y=0;
  pan_info.width=
    windows->pan.width*windows->image.width/windows->image.ximage->width;
  pan_info.height=
    windows->pan.height*windows->image.height/windows->image.ximage->height;
  state=UpdateConfigurationState;
  do
  {
    switch (event->type)
    {
      case ButtonPress:
      {
        /*
          User choose an initial pan location.
        */
        pan_info.x=event->xbutton.x;
        pan_info.y=event->xbutton.y;
        state|=UpdateConfigurationState;
        break;
      }
      case ButtonRelease:
      {
        /*
          User has finished panning the image.
        */
        pan_info.x=event->xbutton.x;
        pan_info.y=event->xbutton.y;
        state|=UpdateConfigurationState | ExitState;
        break;
      }
      case MotionNotify:
      {
        pan_info.x=event->xmotion.x;
        pan_info.y=event->xmotion.y;
        state|=UpdateConfigurationState;
      }
      default:
        break;
    }
    if (state & UpdateConfigurationState)
      {
        /*
          Check boundary conditions.
        */
        if (pan_info.x < (int) (pan_info.width/2))
          pan_info.x=0;
        else
          pan_info.x=(int) (x_factor*(pan_info.x-(pan_info.width/2)));
        if (pan_info.x < 0)
          pan_info.x=0;
        else
          if ((int) (pan_info.x+windows->image.width) >
              windows->image.ximage->width)
            pan_info.x=(long)
              (windows->image.ximage->width-windows->image.width);
        if (pan_info.y < (long) (pan_info.height/2))
          pan_info.y=0;
        else
          pan_info.y=(long) (y_factor*(pan_info.y-(pan_info.height/2)));
        if (pan_info.y < 0)
          pan_info.y=0;
        else
          if ((int) (pan_info.y+windows->image.height) >
              windows->image.ximage->height)
            pan_info.y=(long)
              (windows->image.ximage->height-windows->image.height);
        if ((windows->image.x != (int) pan_info.x) ||
            (windows->image.y != (int) pan_info.y))
          {
            /*
              Display image pan offset.
            */
            windows->image.x=(int) pan_info.x;
            windows->image.y=(int) pan_info.y;
            FormatString(text," %ux%u%+d%+d ",windows->image.width,
              windows->image.height,windows->image.x,windows->image.y);
            MagickXInfoWidget(display,windows,text);
            /*
              Refresh Image window.
            */
            MagickXDrawPanRectangle(display,windows);
            MagickXRefreshWindow(display,&windows->image,(XEvent *) NULL);
          }
        state&=(~UpdateConfigurationState);
      }
    /*
      Wait for next event.
    */
    if (!(state & ExitState))
      MagickXScreenEvent(display,windows,event);
  } while (!(state & ExitState));
  /*
    Restore cursor.
  */
  (void) XDefineCursor(display,windows->pan.id,windows->pan.cursor);
  (void) XFreeCursor(display,cursor);
  (void) XWithdrawWindow(display,windows->info.id,windows->info.screen);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k X P a s t e I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickXPasteImage pastes an image previously saved with MagickXCropImage
%  in the X window image at a location the user chooses with the pointer.
%
%  The format of the MagickXPasteImage method is:
%
%      unsigned int MagickXPasteImage(Display *display,MagickXResourceInfo *resource_info,
%        MagickXWindows *windows,Image *image)
%
%  A description of each parameter follows:
%
%    o status: Method MagickXPasteImage returns True if the image is
%      pasted.  False is returned is there is a memory shortage or if the
%      image fails to be pasted.
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 MagickXResourceInfo structure.
%
%    o windows: Specifies a pointer to a MagickXWindows structure.
%
%    o image: Specifies a pointer to an Image structure; returned from
%      ReadImage.
%
*/
static unsigned int MagickXPasteImage(Display *display,MagickXResourceInfo *resource_info,
  MagickXWindows *windows,Image *image)
{
  static const char
    *PasteMenu[]=
    {
      "Operator",
      "Help",
      "Dismiss",
      (char *) NULL
    };

  static const ModeType
    PasteCommands[]=
    {
      PasteOperatorsCommand,
      PasteHelpCommand,
      PasteDismissCommand
    };

  static CompositeOperator
    operation = CopyCompositeOp;

  char
    text[MaxTextExtent];

  Cursor
    cursor;

  double
    scale_factor;

  Image
    *paste_image;

  int
    id,
    x,
    y;

  RectangleInfo
    highlight_info,
    paste_info;

  unsigned int
    height,
    width;

  unsigned long
    state;

  XEvent
    event;

  /*
    Copy image.
  */
  if (resource_info->copy_image == (Image *) NULL)
    return(False);
  paste_image=CloneImage(resource_info->copy_image,0,0,True,&image->exception);
  /*
    Map Command widget.
  */
  (void) CloneString(&windows->command.name,"Paste");
  windows->command.data=1;
  (void) MagickXCommandWidget(display,windows,PasteMenu,(XEvent *) NULL);
  (void) XMapRaised(display,windows->command.id);
  MagickXClientMessage(display,windows->image.id,windows->im_protocols,
    windows->im_update_widget,CurrentTime);
  /*
    Track pointer until button 1 is pressed.
  */
  MagickXSetCursorState(display,windows,False);
  MagickXQueryPosition(display,windows->image.id,&x,&y);
  (void) XSelectInput(display,windows->image.id,
    windows->image.attributes.event_mask | PointerMotionMask);
  paste_info.x=windows->image.x+x;
  paste_info.y=windows->image.y+y;
  paste_info.width=0;
  paste_info.height=0;
  cursor=XCreateFontCursor(display,XC_ul_angle);
  (void) XSetFunction(display,windows->image.highlight_context,GXinvert);
  state=DefaultState;
  do
  {
    if (windows->info.mapped)
      {
        /*
          Display pointer position.
        */
        FormatString(text," %+ld%+ld ",paste_info.x,paste_info.y);
        MagickXInfoWidget(display,windows,text);
      }
    highlight_info=paste_info;
    highlight_info.x=paste_info.x-windows->image.x;
    highlight_info.y=paste_info.y-windows->image.y;
    MagickXHighlightRectangle(display,windows->image.id,
      windows->image.highlight_context,&highlight_info);
    /*
      Wait for next event.
    */
    MagickXScreenEvent(display,windows,&event);
    MagickXHighlightRectangle(display,windows->image.id,
      windows->image.highlight_context,&highlight_info);
    if (event.xany.window == windows->command.id)
      {
        /*
          Select a command from the Command widget.
        */
        id=MagickXCommandWidget(display,windows,PasteMenu,&event);
        if (id < 0)
          continue;
        switch (PasteCommands[id])
        {
          case PasteOperatorsCommand:
          {
            char
              command[MaxTextExtent];

            static const char
              *OperatorMenu[]=
              {
                "Over",
                "In",
                "Out",
                "Atop",
                "Xor",
                "Plus",
                "Minus",
                "Add",
                "Subtract",
                "Difference",
                "Multiply",
                "Bumpmap",
                "Copy",
                "CopyRed",
                "CopyGreen",
                "CopyBlue",
                "CopyOpacity",
                "Clear",
                (char *) NULL,
              };

            /*
              Select a command from the pop-up menu.
            */
            operation=(CompositeOperator) (MagickXMenuWidget(display,windows,
              PasteMenu[id],OperatorMenu,command)+1);
            break;
          }
          case PasteHelpCommand:
          {
            MagickXTextViewWidget(display,resource_info,windows,False,
              "Help Viewer - Image Composite",ImagePasteHelp);
            break;
          }
          case PasteDismissCommand:
          {
            /*
              Prematurely exit.
            */
            state|=EscapeState;
            state|=ExitState;
            break;
          }
          default:
            break;
        }
        continue;
      }
    switch (event.type)
    {
      case ButtonPress:
      {
        if (IsEventLogging())
          (void) LogMagickEvent(X11Event,GetMagickModule(),
            "Button Press: 0x%lx %u +%d+%d",event.xbutton.window,
            event.xbutton.button,event.xbutton.x,event.xbutton.y);
        if (event.xbutton.button != Button1)
          break;
        if (event.xbutton.window != windows->image.id)
          break;
        /*
          Paste rectangle is relative to image configuration.
        */
        width=(unsigned int) image->columns;
        height=(unsigned int) image->rows;
        x=0;
        y=0;
        if (windows->image.crop_geometry != (char *) NULL)
          (void) XParseGeometry(windows->image.crop_geometry,&x,&y,
            &width,&height);
        scale_factor=(double) windows->image.ximage->width/width;
        paste_info.width=(unsigned int) (scale_factor*paste_image->columns+0.5);
        scale_factor=(double) windows->image.ximage->height/height;
        paste_info.height=(unsigned int) (scale_factor*paste_image->rows+0.5);
        (void) XDefineCursor(display,windows->image.id,cursor);
        paste_info.x=windows->image.x+event.xbutton.x;
        paste_info.y=windows->image.y+event.xbutton.y;
        break;
      }
      case ButtonRelease:
      {
        if (IsEventLogging())
          (void) LogMagickEvent(X11Event,GetMagickModule(),
            "Button Release: 0x%lx %u +%d+%d",event.xbutton.window,
            event.xbutton.button,event.xbutton.x,event.xbutton.y);
        if (event.xbutton.button != Button1)
          break;
        if (event.xbutton.window != windows->image.id)
          break;
        if ((paste_info.width != 0) && (paste_info.height != 0))
          {
            /*
              User has selected the location of the paste image.
            */
            paste_info.x=windows->image.x+event.xbutton.x;
            paste_info.y=windows->image.y+event.xbutton.y;
            state|=ExitState;
          }
        break;
      }
      case Expose:
        break;
      case KeyPress:
      {
        char
          command[MaxTextExtent];

        KeySym
          key_symbol;

        int
          length;

        if (event.xkey.window != windows->image.id)
          break;
        /*
          Respond to a user key press.
        */
        length=XLookupString((XKeyEvent *) &event.xkey,command,sizeof(command),
          &key_symbol,(XComposeStatus *) NULL);
        *(command+length)='\0';
        if (IsEventLogging())
          (void) LogMagickEvent(X11Event,GetMagickModule(),
            "Key press: 0x%lx (%.1024s)",key_symbol,command);
        switch ((int) key_symbol)
        {
          case XK_Escape:
          case XK_F20:
          {
            /*
              Prematurely exit.
            */
            DestroyImage(paste_image);
            state|=EscapeState;
            state|=ExitState;
            break;
          }
          case XK_F1:
          case XK_Help:
          {
            (void) XSetFunction(display,windows->image.highlight_context,
              GXcopy);
            MagickXTextViewWidget(display,resource_info,windows,False,
              "Help Viewer - Image Composite",ImagePasteHelp);
            (void) XSetFunction(display,windows->image.highlight_context,
              GXinvert);
            break;
          }
          default:
          {
            (void) XBell(display,0);
            break;
          }
        }
        break;
      }
      case MotionNotify:
      {
        /*
          Map and unmap Info widget as text cursor crosses its boundaries.
        */
        x=event.xmotion.x;
        y=event.xmotion.y;
        if (windows->info.mapped)
          {
            if ((x < (int) (windows->info.x+windows->info.width)) &&
                (y < (int) (windows->info.y+windows->info.height)))
              (void) XWithdrawWindow(display,windows->info.id,
                windows->info.screen);
          }
        else
          if ((x > (int) (windows->info.x+windows->info.width)) ||
              (y > (int) (windows->info.y+windows->info.height)))
            (void) XMapWindow(display,windows->info.id);
        paste_info.x=windows->image.x+x;
        paste_info.y=windows->image.y+y;
        break;
      }
      default:
      {
        if (IsEventLogging())
          (void) LogMagickEvent(X11Event,GetMagickModule(),"Event type: %d",
            event.type);
        break;
      }
    }
  } while (!(state & ExitState));
  (void) XSelectInput(display,windows->image.id,
    windows->image.attributes.event_mask);
  (void) XSetFunction(display,windows->image.highlight_context,GXcopy);
  MagickXSetCursorState(display,windows,False);
  (void) XFreeCursor(display,cursor);
  if (state & EscapeState)
    return(True);
  /*
    Image pasting is relative to image configuration.
  */
  MagickXSetCursorState(display,windows,True);
  MagickXCheckRefreshWindows(display,windows);
  width=(unsigned int) image->columns;
  height=(unsigned int) image->rows;
  x=0;
  y=0;
  if (windows->image.crop_geometry != (char *) NULL)
    (void) XParseGeometry(windows->image.crop_geometry,&x,&y,&width,&height);
  scale_factor=(double) width/windows->image.ximage->width;
  paste_info.x+=x;
  paste_info.x=(int) (scale_factor*paste_info.x+0.5);
  paste_info.width=(unsigned int) (scale_factor*paste_info.width+0.5);
  scale_factor=(double) height/windows->image.ximage->height;
  paste_info.y+=y;
  paste_info.y=(int) (scale_factor*paste_info.y*scale_factor+0.5);
  paste_info.height=(unsigned int) (scale_factor*paste_info.height+0.5);
  /*
    Paste image with X Image window.
  */
  (void) CompositeImage(image,operation,paste_image,paste_info.x,paste_info.y);
  DestroyImage(paste_image);
  MagickXSetCursorState(display,windows,False);
  /*
    Update image colormap.
  */
  MagickXConfigureImageColormap(display,resource_info,windows,image);
  (void) MagickXConfigureImage(display,resource_info,windows,image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k X P r i n t I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickXPrintImage prints an image to a Postscript printer.
%
%  The format of the MagickXPrintImage method is:
%
%      unsigned int MagickXPrintImage(Display *display,MagickXResourceInfo *resource_info,
%        MagickXWindows *windows,Image *image)
%
%  A description of each parameter follows:
%
%    o status: Method MagickXPrintImage return True if the image is
%      printed.  False is returned is there is a memory shortage or if the
%      image fails to print.
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 MagickXResourceInfo structure.
%
%    o windows: Specifies a pointer to a MagickXWindows structure.
%
%    o image: Specifies a pointer to an Image structure;  returned from
%      ReadImage.
%
%
*/
static unsigned int MagickXPrintImage(Display *display,MagickXResourceInfo *resource_info,
  MagickXWindows *windows,Image *image)
{
  char
    filename[MaxTextExtent],
    geometry[MaxTextExtent];

  Image
    *print_image;

  ImageInfo
    *image_info;

  unsigned int
    status=True;

  /*
    Request Postscript page geometry from user.
  */
  image_info=CloneImageInfo(resource_info->image_info);
  FormatString(geometry,"Letter");
  if (image_info->page != (char *) NULL)
    (void) strlcpy(geometry,image_info->page,MaxTextExtent);
  MagickXListBrowserWidget(display,windows,&windows->widget,PageSizes,"Select",
    "Select Postscript Page Geometry:",geometry);
  if (*geometry == '\0')
    return(True);
  image_info->page=GetPageGeometry(geometry);
  /*
    Apply image transforms.
  */
  MagickXSetCursorState(display,windows,True);
  MagickXCheckRefreshWindows(display,windows);
  print_image=CloneImage(image,0,0,True,&image->exception);
  if (print_image == (Image *) NULL)
    return(False);
  FormatString(geometry,"%dx%d!",windows->image.ximage->width,
    windows->image.ximage->height);
  TransformImage(&print_image,windows->image.crop_geometry,geometry);
  /*
    Print image.
  */
  if (!AcquireTemporaryFileName(print_image->magick_filename))
    {
      MagickXNoticeWidget(display,windows,"Unable to open temporary file:",
        print_image->magick_filename);
      status=False;
      goto error_return;
    }
  if (!AcquireTemporaryFileName(filename))
    {
      MagickXNoticeWidget(display,windows,"Unable to open temporary file:",
        filename);
      status=False;
      goto error_return;
    }
  FormatString(print_image->filename,"print:%s",filename);
  status=WriteImage(image_info,print_image);
 error_return:
  DestroyImage(print_image);
  DestroyImageInfo(image_info);
  MagickXSetCursorState(display,windows,False);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k X R O I I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickXROIImage applies an image processing technique to a region
%  of interest.
%
%  The format of the MagickXROIImage method is:
%
%      unsigned int MagickXROIImage(Display *display,MagickXResourceInfo *resource_info,
%        MagickXWindows *windows,Image **image)
%
%  A description of each parameter follows:
%
%    o status: Method MagickXROIImage returns True if the image is
%      cropped.  False is returned is there is a memory shortage or if the
%      image fails to be cropped.
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 MagickXResourceInfo structure.
%
%    o windows: Specifies a pointer to a MagickXWindows structure.
%
%    o image: Specifies a pointer to an Image structure; returned from
%      ReadImage.
%
%
*/
static unsigned int MagickXROIImage(Display *display,MagickXResourceInfo *resource_info,
  MagickXWindows *windows,Image **image)
{
#define ApplyMenus  7

  static const char
    *ROIMenu[]=
    {
      "Help",
      "Dismiss",
      (char *) NULL
    },
    *ApplyMenu[]=
    {
      "File",
      "Edit",
      "Transform",
      "Enhance",
      "Effects",
      "F/X",
      "Miscellany",
      "Help",
      "Dismiss",
      (char *) NULL
    },
    *FileMenu[]=
    {
      "Save...",
      "Print...",
      (char *) NULL
    },
    *EditMenu[]=
    {
      "Undo",
      "Redo",
      (char *) NULL
    },
    *TransformMenu[]=
    {
      "Flop",
      "Flip",
      "Rotate Right",
      "Rotate Left",
      (char *) NULL
    },
    *EnhanceMenu[]=
    {
      "Hue...",
      "Saturation...",
      "Brightness...",
      "Gamma...",
      "Spiff",
      "Dull",
      "Equalize",
      "Normalize",
      "Negate",
      "Grayscale",
      "Map...",
      "Quantize...",
      (char *) NULL
    },
    *EffectsMenu[]=
    {
      "Despeckle",
      "Emboss",
      "Reduce Noise",
      "Add Noise",
      "Sharpen...",
      "Blur...",
      "Threshold...",
      "Edge Detect...",
      "Spread...",
      "Shade...",
      "Raise...",
      "Segment...",
      (char *) NULL
    },
    *FXMenu[]=
    {
      "Solarize...",
      "Swirl...",
      "Implode...",
      "Wave...",
      "Oil Paint...",
      "Charcoal Draw...",
      (char *) NULL
    },
    *MiscellanyMenu[]=
    {
      "Image Info",
      "Zoom Image",
      "Show Preview...",
      "Show Histogram",
      "Show Matte",
      (char *) NULL
    };

  static const char
    **Menus[ApplyMenus]=
    {
      FileMenu,
      EditMenu,
      TransformMenu,
      EnhanceMenu,
      EffectsMenu,
      FXMenu,
      MiscellanyMenu
    };

  static const CommandType
    ApplyCommands[]=
    {
      NullCommand,
      NullCommand,
      NullCommand,
      NullCommand,
      NullCommand,
      NullCommand,
      NullCommand,
      HelpCommand,
      QuitCommand
    },
    FileCommands[]=
    {
      SaveCommand,
      PrintCommand
    },
    EditCommands[]=
    {
      UndoCommand,
      RedoCommand
    },
    TransformCommands[]=
    {
      FlopCommand,
      FlipCommand,
      RotateRightCommand,
      RotateLeftCommand
    },
    EnhanceCommands[]=
    {
      HueCommand,
      SaturationCommand,
      BrightnessCommand,
      GammaCommand,
      SpiffCommand,
      DullCommand,
      EqualizeCommand,
      NormalizeCommand,
      NegateCommand,
      GrayscaleCommand,
      MapCommand,
      QuantizeCommand
    },
    EffectsCommands[]=
    {
      DespeckleCommand,
      EmbossCommand,
      ReduceNoiseCommand,
      AddNoiseCommand,
      SharpenCommand,
      BlurCommand,
      EdgeDetectCommand,
      SpreadCommand,
      ShadeCommand,
      RaiseCommand,
      SegmentCommand
    },
    FXCommands[]=
    {
      SolarizeCommand,
      SwirlCommand,
      ImplodeCommand,
      WaveCommand,
      OilPaintCommand,
      CharcoalDrawCommand
    },
    MiscellanyCommands[]=
    {
      InfoCommand,
      ZoomCommand,
      ShowPreviewCommand,
      ShowHistogramCommand,
      ShowMatteCommand
    },
    ROICommands[]=
    {
      ROIHelpCommand,
      ROIDismissCommand
    };

  static const CommandType
    *Commands[ApplyMenus]=
    {
      FileCommands,
      EditCommands,
      TransformCommands,
      EnhanceCommands,
      EffectsCommands,
      FXCommands,
      MiscellanyCommands
    };

  char
    command[MaxTextExtent],
    text[MaxTextExtent];

  CommandType
    command_type;

  Cursor
    cursor;

  double
    scale_factor;

  Image
    *roi_image;

  int
    entry,
    id,
    x,
    y;

  MonitorHandler
    handler;

  RectangleInfo
    crop_info,
    highlight_info,
    roi_info;

  unsigned int
    height,
    width;

  unsigned long
    state;

  XEvent
    event;

  /*
    Map Command widget.
  */
  (void) CloneString(&windows->command.name,"ROI");
  windows->command.data=0;
  (void) MagickXCommandWidget(display,windows,ROIMenu,(XEvent *) NULL);
  (void) XMapRaised(display,windows->command.id);
  MagickXClientMessage(display,windows->image.id,windows->im_protocols,
    windows->im_update_widget,CurrentTime);
  /*
    Track pointer until button 1 is pressed.
  */
  MagickXQueryPosition(display,windows->image.id,&x,&y);
  (void) XSelectInput(display,windows->image.id,
    windows->image.attributes.event_mask | PointerMotionMask);
  roi_info.x=windows->image.x+x;
  roi_info.y=windows->image.y+y;
  roi_info.width=0;
  roi_info.height=0;
  cursor=XCreateFontCursor(display,XC_fleur);
  state=DefaultState;
  do
  {
    if (windows->info.mapped)
      {
        /*
          Display pointer position.
        */
        FormatString(text," %+ld%+ld ",roi_info.x,roi_info.y);
        MagickXInfoWidget(display,windows,text);
      }
    /*
      Wait for next event.
    */
    MagickXScreenEvent(display,windows,&event);
    if (event.xany.window == windows->command.id)
      {
        /*
          Select a command from the Command widget.
        */
        id=MagickXCommandWidget(display,windows,ROIMenu,&event);
        if (id < 0)
          continue;
        switch (ROICommands[id])
        {
          case ROIHelpCommand:
          {
            MagickXTextViewWidget(display,resource_info,windows,False,
              "Help Viewer - Region of Interest",ImageROIHelp);
            break;
          }
          case ROIDismissCommand:
          {
            /*
              Prematurely exit.
            */
            state|=EscapeState;
            state|=ExitState;
            break;
          }
          default:
            break;
        }
        continue;
      }
    switch (event.type)
    {
      case ButtonPress:
      {
        if (event.xbutton.button != Button1)
          break;
        if (event.xbutton.window != windows->image.id)
          break;
        /*
          Note first corner of region of interest rectangle-- exit loop.
        */
        (void) XDefineCursor(display,windows->image.id,cursor);
        roi_info.x=windows->image.x+event.xbutton.x;
        roi_info.y=windows->image.y+event.xbutton.y;
        state|=ExitState;
        break;
      }
      case ButtonRelease:
        break;
      case Expose:
        break;
      case KeyPress:
      {
        KeySym
          key_symbol;

        if (event.xkey.window != windows->image.id)
          break;
        /*
          Respond to a user key press.
        */
        (void) XLookupString((XKeyEvent *) &event.xkey,command,sizeof(command),
          &key_symbol,(XComposeStatus *) NULL);
        switch ((int) key_symbol)
        {
          case XK_Escape:
          case XK_F20:
          {
            /*
              Prematurely exit.
            */
            state|=EscapeState;
            state|=ExitState;
            break;
          }
          case XK_F1:
          case XK_Help:
          {
            MagickXTextViewWidget(display,resource_info,windows,False,
              "Help Viewer - Region of Interest",ImageROIHelp);
            break;
          }
          default:
          {
            (void) XBell(display,0);
            break;
          }
        }
        break;
      }
      case MotionNotify:
      {
        /*
          Map and unmap Info widget as text cursor crosses its boundaries.
        */
        x=event.xmotion.x;
        y=event.xmotion.y;
        if (windows->info.mapped)
          {
            if ((x < (int) (windows->info.x+windows->info.width)) &&
                (y < (int) (windows->info.y+windows->info.height)))
              (void) XWithdrawWindow(display,windows->info.id,
                windows->info.screen);
          }
        else
          if ((x > (int) (windows->info.x+windows->info.width)) ||
              (y > (int) (windows->info.y+windows->info.height)))
            (void) XMapWindow(display,windows->info.id);
        roi_info.x=windows->image.x+x;
        roi_info.y=windows->image.y+y;
        break;
      }
      default:
        break;
    }
  } while (!(state & ExitState));
  (void) XSelectInput(display,windows->image.id,
    windows->image.attributes.event_mask);
  if (state & EscapeState)
    {
      /*
        User want to exit without region of interest.
      */
      (void) XWithdrawWindow(display,windows->info.id,windows->info.screen);
      (void) XFreeCursor(display,cursor);
      return(True);
    }
  (void) XSetFunction(display,windows->image.highlight_context,GXinvert);
  do
  {
    /*
      Size rectangle as pointer moves until the mouse button is released.
    */
    x=(int) roi_info.x;
    y=(int) roi_info.y;
    roi_info.width=0;
    roi_info.height=0;
    state=DefaultState;
    do
    {
      highlight_info=roi_info;
      highlight_info.x=roi_info.x-windows->image.x;
      highlight_info.y=roi_info.y-windows->image.y;
      if ((highlight_info.width > 3) && (highlight_info.height > 3))
        {
          /*
            Display info and draw region of interest rectangle.
          */
          if (!windows->info.mapped)
            (void) XMapWindow(display,windows->info.id);
          FormatString(text," %lux%lu%+ld%+ld",roi_info.width,roi_info.height,
            roi_info.x,roi_info.y);
          MagickXInfoWidget(display,windows,text);
          MagickXHighlightRectangle(display,windows->image.id,
            windows->image.highlight_context,&highlight_info);
        }
      else
        if (windows->info.mapped)
          (void) XWithdrawWindow(display,windows->info.id,windows->info.screen);
      /*
        Wait for next event.
      */
      MagickXScreenEvent(display,windows,&event);
      if ((highlight_info.width > 3) && (highlight_info.height > 3))
        MagickXHighlightRectangle(display,windows->image.id,
          windows->image.highlight_context,&highlight_info);
      switch (event.type)
      {
        case ButtonPress:
        {
          roi_info.x=windows->image.x+event.xbutton.x;
          roi_info.y=windows->image.y+event.xbutton.y;
          break;
        }
        case ButtonRelease:
        {
          /*
            User has committed to region of interest rectangle.
          */
          roi_info.x=windows->image.x+event.xbutton.x;
          roi_info.y=windows->image.y+event.xbutton.y;
          MagickXSetCursorState(display,windows,False);
          state|=ExitState;
          if (LocaleCompare(windows->command.name,"Apply") == 0)
            break;
          (void) CloneString(&windows->command.name,"Apply");
          windows->command.data=ApplyMenus;
          (void) MagickXCommandWidget(display,windows,ApplyMenu,(XEvent *) NULL);
          break;
        }
        case Expose:
          break;
        case MotionNotify:
        {
          roi_info.x=windows->image.x+event.xmotion.x;
          roi_info.y=windows->image.y+event.xmotion.y;
        }
        default:
          break;
      }
      if ((((int) roi_info.x != x) && ((int) roi_info.y != y)) ||
          (state & ExitState))
        {
          /*
            Check boundary conditions.
          */
          if (roi_info.x < 0)
            roi_info.x=0;
          else
            if (roi_info.x > (int) windows->image.ximage->width)
              roi_info.x=windows->image.ximage->width;
          if ((int) roi_info.x < x)
            roi_info.width=(unsigned int) (x-roi_info.x);
          else
            {
              roi_info.width=(unsigned int) (roi_info.x-x);
              roi_info.x=x;
            }
          if (roi_info.y < 0)
            roi_info.y=0;
          else
            if (roi_info.y > (int) windows->image.ximage->height)
              roi_info.y=windows->image.ximage->height;
          if ((int) roi_info.y < y)
            roi_info.height=(unsigned int) (y-roi_info.y);
          else
            {
              roi_info.height=(unsigned int) (roi_info.y-y);
              roi_info.y=y;
            }
        }
    } while (!(state & ExitState));
    /*
      Wait for user to grab a corner of the rectangle or press return.
    */
    state=DefaultState;
    command_type=NullCommand;
    do
    {
      if (windows->info.mapped)
        {
          /*
            Display pointer position.
          */
          FormatString(text," %lux%lu%+ld%+ld",roi_info.width,roi_info.height,
            roi_info.x,roi_info.y);
          MagickXInfoWidget(display,windows,text);
        }
      highlight_info=roi_info;
      highlight_info.x=roi_info.x-windows->image.x;
      highlight_info.y=roi_info.y-windows->image.y;
      if ((highlight_info.width <= 3) || (highlight_info.height <= 3))
        {
          state|=EscapeState;
          state|=ExitState;
          break;
        }
      if (state & UpdateRegionState)
        {
          (void) XSetFunction(display,windows->image.highlight_context,GXcopy);
          switch (command_type)
          {
            case UndoCommand:
            case RedoCommand:
            {
              (void) MagickXMagickCommand(display,resource_info,windows,command_type,
                image);
              break;
            }
            default:
            {
              /*
                Region of interest is relative to image configuration.
              */
              handler=SetMonitorHandler((MonitorHandler) NULL);
              crop_info=roi_info;
              width=(unsigned int) (*image)->columns;
              height=(unsigned int) (*image)->rows;
              x=0;
              y=0;
              if (windows->image.crop_geometry != (char *) NULL)
                (void) XParseGeometry(windows->image.crop_geometry,&x,&y,
                  &width,&height);
              scale_factor=(double) width/windows->image.ximage->width;
              crop_info.x+=x;
              crop_info.x=(int) (scale_factor*crop_info.x+0.5);
              crop_info.width=(unsigned int) (scale_factor*crop_info.width+0.5);
              scale_factor=(double) height/windows->image.ximage->height;
              crop_info.y+=y;
              crop_info.y=(int) (scale_factor*crop_info.y+0.5);
              crop_info.height=(unsigned int)
                (scale_factor*crop_info.height+0.5);
              roi_image=CropImage(*image,&crop_info,&(*image)->exception);
              (void) SetMonitorHandler(handler);
              if (roi_image == (Image *) NULL)
                continue;
              /*
                Apply image processing technique to the region of interest.
              */
              windows->image.orphan=True;
              (void) MagickXMagickCommand(display,resource_info,windows,command_type,
                &roi_image);
              handler=SetMonitorHandler((MonitorHandler) NULL);
              (void) MagickXMagickCommand(display,resource_info,windows,
                SaveToUndoBufferCommand,image);
              windows->image.orphan=False;
              (void) CompositeImage(*image,CopyCompositeOp,roi_image,
                crop_info.x,crop_info.y);
              DestroyImage(roi_image);
              (void) SetMonitorHandler(handler);
              break;
            }
          }
          if (command_type != InfoCommand)
            {
              MagickXConfigureImageColormap(display,resource_info,windows,*image);
              (void) MagickXConfigureImage(display,resource_info,windows,*image);
            }
          MagickXCheckRefreshWindows(display,windows);
          MagickXInfoWidget(display,windows,text);
          (void) XSetFunction(display,windows->image.highlight_context,
            GXinvert);
          state&=(~UpdateRegionState);
        }
      MagickXHighlightRectangle(display,windows->image.id,
        windows->image.highlight_context,&highlight_info);
      MagickXScreenEvent(display,windows,&event);
      if (event.xany.window == windows->command.id)
        {
          /*
            Select a command from the Command widget.
          */
          (void) XSetFunction(display,windows->image.highlight_context,GXcopy);
          command_type=NullCommand;
          id=MagickXCommandWidget(display,windows,ApplyMenu,&event);
          if (id >= 0)
            {
              (void) strlcpy(command,ApplyMenu[id],MaxTextExtent);
              command_type=ApplyCommands[id];
              if (id < ApplyMenus)
                {
                  /*
                    Select a command from a pop-up menu.
                  */
                  entry=MagickXMenuWidget(display,windows,ApplyMenu[id],
                    (const char **) Menus[id],command);
                  if (entry >= 0)
                    {
                      (void) strlcpy(command,Menus[id][entry],MaxTextExtent);
                      command_type=Commands[id][entry];
                    }
                }
            }
          (void) XSetFunction(display,windows->image.highlight_context,
            GXinvert);
          MagickXHighlightRectangle(display,windows->image.id,
            windows->image.highlight_context,&highlight_info);
          if (command_type == HelpCommand)
            {
              (void) XSetFunction(display,windows->image.highlight_context,
                GXcopy);
              MagickXTextViewWidget(display,resource_info,windows,False,
                "Help Viewer - Region of Interest",ImageROIHelp);
              (void) XSetFunction(display,windows->image.highlight_context,
                GXinvert);
              continue;
            }
          if (command_type == QuitCommand)
            {
              /*
                Exit.
              */
              state|=EscapeState;
              state|=ExitState;
              continue;
            }
          if (command_type != NullCommand)
            state|=UpdateRegionState;
          continue;
        }
      MagickXHighlightRectangle(display,windows->image.id,
        windows->image.highlight_context,&highlight_info);
      switch (event.type)
      {
        case ButtonPress:
        {
          x=windows->image.x;
          y=windows->image.y;
          if (event.xbutton.button != Button1)
            break;
          if (event.xbutton.window != windows->image.id)
            break;
          x=windows->image.x+event.xbutton.x;
          y=windows->image.y+event.xbutton.y;
          if ((x < (int) (roi_info.x+RoiDelta)) &&
              (x > (int) (roi_info.x-RoiDelta)) &&
              (y < (int) (roi_info.y+RoiDelta)) &&
              (y > (int) (roi_info.y-RoiDelta)))
            {
              roi_info.x=(long) (roi_info.x+roi_info.width);
              roi_info.y=(long) (roi_info.y+roi_info.height);
              state|=UpdateConfigurationState;
              break;
            }
          if ((x < (int) (roi_info.x+RoiDelta)) &&
              (x > (int) (roi_info.x-RoiDelta)) &&
              (y < (int) (roi_info.y+roi_info.height+RoiDelta)) &&
              (y > (int) (roi_info.y+roi_info.height-RoiDelta)))
            {
              roi_info.x=(long) (roi_info.x+roi_info.width);
              state|=UpdateConfigurationState;
              break;
            }
          if ((x < (int) (roi_info.x+roi_info.width+RoiDelta)) &&
              (x > (int) (roi_info.x+roi_info.width-RoiDelta)) &&
              (y < (int) (roi_info.y+RoiDelta)) &&
              (y > (int) (roi_info.y-RoiDelta)))
            {
              roi_info.y=(long) (roi_info.y+roi_info.height);
              state|=UpdateConfigurationState;
              break;
            }
          if ((x < (int) (roi_info.x+roi_info.width+RoiDelta)) &&
              (x > (int) (roi_info.x+roi_info.width-RoiDelta)) &&
              (y < (int) (roi_info.y+roi_info.height+RoiDelta)) &&
              (y > (int) (roi_info.y+roi_info.height-RoiDelta)))
            {
              state|=UpdateConfigurationState;
              break;
            }
        }
        case ButtonRelease:
        {
          if (event.xbutton.window == windows->pan.id)
            if ((highlight_info.x != crop_info.x-windows->image.x) ||
                (highlight_info.y != crop_info.y-windows->image.y))
              MagickXHighlightRectangle(display,windows->image.id,
                windows->image.highlight_context,&highlight_info);
          break;
        }
        case Expose:
        {
          if (event.xexpose.window == windows->image.id)
            if (event.xexpose.count == 0)
              {
                event.xexpose.x=(int) highlight_info.x;
                event.xexpose.y=(int) highlight_info.y;
                event.xexpose.width=(unsigned int) highlight_info.width;
                event.xexpose.height=(unsigned int) highlight_info.height;
                MagickXRefreshWindow(display,&windows->image,&event);
              }
          if (event.xexpose.window == windows->info.id)
            if (event.xexpose.count == 0)
              MagickXInfoWidget(display,windows,text);
          break;
        }
        case KeyPress:
        {
          KeySym
            key_symbol;

          if (event.xkey.window != windows->image.id)
            break;
          /*
            Respond to a user key press.
          */
          (void) XLookupString((XKeyEvent *) &event.xkey,command,
            sizeof(command),&key_symbol,(XComposeStatus *) NULL);
          switch ((int) key_symbol)
          {
            case XK_Shift_L:
            case XK_Shift_R:
              break;
            case XK_Escape:
            case XK_F20:
              state|=EscapeState;
            case XK_Return:
            {
              state|=ExitState;
              break;
            }
            case XK_F1:
            case XK_Help:
            {
              (void) XSetFunction(display,windows->image.highlight_context,
                GXcopy);
              MagickXTextViewWidget(display,resource_info,windows,False,
                "Help Viewer - Region of Interest",ImageROIHelp);
              (void) XSetFunction(display,windows->image.highlight_context,
                GXinvert);
              break;
            }
            default:
            {
              command_type=MagickXImageWindowCommand(display,resource_info,windows,
                event.xkey.state,key_symbol,image);
              if (command_type != NullCommand)
                state|=UpdateRegionState;
              break;
            }
          }
          break;
        }
        case KeyRelease:
          break;
        case MotionNotify:
        {
          if (event.xbutton.window != windows->image.id)
            break;
          /*
            Map and unmap Info widget as text cursor crosses its boundaries.
          */
          x=event.xmotion.x;
          y=event.xmotion.y;
          if (windows->info.mapped)
            {
              if ((x < (int) (windows->info.x+windows->info.width)) &&
                  (y < (int) (windows->info.y+windows->info.height)))
                (void) XWithdrawWindow(display,windows->info.id,
                  windows->info.screen);
            }
          else
            if ((x > (int) (windows->info.x+windows->info.width)) ||
                (y > (int) (windows->info.y+windows->info.height)))
              (void) XMapWindow(display,windows->info.id);
          break;
        }
        default:
          break;
      }
      if (state & UpdateConfigurationState)
        {
          (void) XPutBackEvent(display,&event);
          (void) XDefineCursor(display,windows->image.id,cursor);
          break;
        }
    } while (!(state & ExitState));
  } while (!(state & ExitState));
  (void) XSetFunction(display,windows->image.highlight_context,GXcopy);
  MagickXSetCursorState(display,windows,False);
  if (state & EscapeState)
    return(True);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k X R o t a t e I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickXRotateImage rotates the X image.  If the degrees parameter
%  if zero, the rotation angle is computed from the slope of a line drawn by
%  the user.
%
%  The format of the MagickXRotateImage method is:
%
%      unsigned int MagickXRotateImage(Display *display,MagickXResourceInfo *resource_info,
%        MagickXWindows *windows,double degrees,Image **image)
%
%  A description of each parameter follows:
%
%    o status: Method MagickXRotateImage return True if the image is
%      rotate.  False is returned is there is a memory shortage or if the
%      image fails to rotate.
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 MagickXResourceInfo structure.
%
%    o windows: Specifies a pointer to a MagickXWindows structure.
%
%    o degrees: Specifies the number of degrees to rotate the image.
%
%    o image: Specifies a pointer to an Image structure;  returned from
%      ReadImage.
%
%
*/
static unsigned int MagickXRotateImage(Display *display,MagickXResourceInfo *resource_info,
  MagickXWindows *windows,double degrees,Image **image)
{
  static const char
    *RotateMenu[]=
    {
      "Pixel Color",
      "Direction",
      "Help",
      "Dismiss",
      (char *) NULL
    };

  static ModeType
    direction = HorizontalRotateCommand;

  static const ModeType
    DirectionCommands[]=
    {
      HorizontalRotateCommand,
      VerticalRotateCommand
    },
    RotateCommands[]=
    {
      RotateColorCommand,
      RotateDirectionCommand,
      RotateHelpCommand,
      RotateDismissCommand
    };

  static unsigned int
    pen_id = 0;

  char
    command[MaxTextExtent],
    text[MaxTextExtent];

  double
    normalized_degrees;

  Image
    *rotate_image;

  int
    id,
    x,
    y;

  register int
    i;

  unsigned int
    height,
    rotations,
    width;

  if (degrees == 0.0)
    {
      unsigned int
        distance;

      unsigned long
        state;

      XEvent
        event;

      XSegment
        rotate_info;

      /*
        Map Command widget.
      */
      (void) CloneString(&windows->command.name,"Rotate");
      windows->command.data=2;
      (void) MagickXCommandWidget(display,windows,RotateMenu,(XEvent *) NULL);
      (void) XMapRaised(display,windows->command.id);
      MagickXClientMessage(display,windows->image.id,windows->im_protocols,
        windows->im_update_widget,CurrentTime);
      /*
        Wait for first button press.
      */
      (void) XSetFunction(display,windows->image.highlight_context,GXinvert);
      MagickXQueryPosition(display,windows->image.id,&x,&y);
      rotate_info.x1=x;
      rotate_info.y1=y;
      rotate_info.x2=x;
      rotate_info.y2=y;
      state=DefaultState;
      do
      {
        MagickXHighlightLine(display,windows->image.id,
          windows->image.highlight_context,&rotate_info);
        /*
          Wait for next event.
        */
        MagickXScreenEvent(display,windows,&event);
        MagickXHighlightLine(display,windows->image.id,
          windows->image.highlight_context,&rotate_info);
        if (event.xany.window == windows->command.id)
          {
            /*
              Select a command from the Command widget.
            */
            id=MagickXCommandWidget(display,windows,RotateMenu,&event);
            if (id < 0)
              continue;
            (void) XSetFunction(display,windows->image.highlight_context,
              GXcopy);
            switch (RotateCommands[id])
            {
              case RotateColorCommand:
              {
                char
                  *ColorMenu[MaxNumberPens];

                int
                  pen_number;

                XColor
                  color;

                /*
                  Initialize menu selections.
                */
                for (i=0; i < (int) (MaxNumberPens-2); i++)
                  ColorMenu[i]=resource_info->pen_colors[i];
                ColorMenu[MaxNumberPens-2]=(char *) "Browser...";
                ColorMenu[MaxNumberPens-1]=(char *) NULL;
                /*
                  Select a pen color from the pop-up menu.
                */
                pen_number=MagickXMenuWidget(display,windows,RotateMenu[id],
                  (const char **) ColorMenu,command);
                if (pen_number < 0)
                  break;
                if (pen_number == (MaxNumberPens-2))
                  {
                    static char
                      color_name[MaxTextExtent] = "gray";

                    /*
                      Select a pen color from a dialog.
                    */
                    resource_info->pen_colors[pen_number]=color_name;
                    MagickXColorBrowserWidget(display,windows,"Select",color_name);
                    if (*color_name == '\0')
                      break;
                  }
                /*
                  Set pen color.
                */
                (void) XParseColor(display,windows->map_info->colormap,
                  resource_info->pen_colors[pen_number],&color);
                MagickXBestPixel(display,windows->map_info->colormap,(XColor *) NULL,
                  (unsigned int) MaxColors,&color);
                windows->pixel_info->pen_colors[pen_number]=color;
                pen_id=pen_number;
                break;
              }
              case RotateDirectionCommand:
              {
                static const char
                  *Directions[]=
                  {
                    "horizontal",
                    "vertical",
                    (char *) NULL,
                  };

                /*
                  Select a command from the pop-up menu.
                */
                id=MagickXMenuWidget(display,windows,RotateMenu[id],
                  Directions,command);
                if (id >= 0)
                  direction=DirectionCommands[id];
                break;
              }
              case RotateHelpCommand:
              {
                MagickXTextViewWidget(display,resource_info,windows,False,
                  "Help Viewer - Image Rotation",ImageRotateHelp);
                break;
              }
              case RotateDismissCommand:
              {
                /*
                  Prematurely exit.
                */
                state|=EscapeState;
                state|=ExitState;
                break;
              }
              default:
                break;
            }
            (void) XSetFunction(display,windows->image.highlight_context,
              GXinvert);
            continue;
          }
        switch (event.type)
        {
          case ButtonPress:
          {
            if (event.xbutton.button != Button1)
              break;
            if (event.xbutton.window != windows->image.id)
              break;
            /*
              Exit loop.
            */
            (void) XSetFunction(display,windows->image.highlight_context,
              GXcopy);
            rotate_info.x1=event.xbutton.x;
            rotate_info.y1=event.xbutton.y;
            state|=ExitState;
            break;
          }
          case ButtonRelease:
            break;
          case Expose:
            break;
          case KeyPress:
          {
            char
              command[MaxTextExtent];

            KeySym
              key_symbol;

            if (event.xkey.window != windows->image.id)
              break;
            /*
              Respond to a user key press.
            */
            (void) XLookupString((XKeyEvent *) &event.xkey,command,
              sizeof(command),&key_symbol,(XComposeStatus *) NULL);
            switch ((int) key_symbol)
            {
              case XK_Escape:
              case XK_F20:
              {
                /*
                  Prematurely exit.
                */
                state|=EscapeState;
                state|=ExitState;
                break;
              }
              case XK_F1:
              case XK_Help:
              {
                (void) XSetFunction(display,windows->image.highlight_context,
                  GXcopy);
                MagickXTextViewWidget(display,resource_info,windows,False,
                  "Help Viewer - Image Rotation",ImageRotateHelp);
                (void) XSetFunction(display,windows->image.highlight_context,
                  GXinvert);
                break;
              }
              default:
              {
                (void) XBell(display,0);
                break;
              }
            }
            break;
          }
          case MotionNotify:
          {
            rotate_info.x1=event.xmotion.x;
            rotate_info.y1=event.xmotion.y;
          }
        }
        rotate_info.x2=rotate_info.x1;
        rotate_info.y2=rotate_info.y1;
        if (direction == HorizontalRotateCommand)
          rotate_info.x2+=32;
        else
          rotate_info.y2-=32;
      } while (!(state & ExitState));
      (void) XSetFunction(display,windows->image.highlight_context,GXcopy);
      (void) XWithdrawWindow(display,windows->info.id,windows->info.screen);
      if (state & EscapeState)
        return(True);
      /*
        Draw line as pointer moves until the mouse button is released.
      */
      distance=0;
      (void) XSetFunction(display,windows->image.highlight_context,GXinvert);
      state=DefaultState;
      do
      {
        if (distance > 9)
          {
            /*
              Display info and draw rotation line.
            */
            if (!windows->info.mapped)
              (void) XMapWindow(display,windows->info.id);
            FormatString(text," %.2f",
              direction == VerticalRotateCommand ? degrees-90.0 : degrees);
            MagickXInfoWidget(display,windows,text);
            MagickXHighlightLine(display,windows->image.id,
              windows->image.highlight_context,&rotate_info);
          }
        else
          if (windows->info.mapped)
            (void) XWithdrawWindow(display,windows->info.id,
              windows->info.screen);
        /*
          Wait for next event.
        */
        MagickXScreenEvent(display,windows,&event);
        if (distance > 9)
          MagickXHighlightLine(display,windows->image.id,
            windows->image.highlight_context,&rotate_info);
        switch (event.type)
        {
          case ButtonPress:
            break;
          case ButtonRelease:
          {
            /*
              User has committed to rotation line.
            */
            rotate_info.x2=event.xbutton.x;
            rotate_info.y2=event.xbutton.y;
            state|=ExitState;
            break;
          }
          case Expose:
            break;
          case MotionNotify:
          {
            rotate_info.x2=event.xmotion.x;
            rotate_info.y2=event.xmotion.y;
          }
          default:
            break;
        }
        /*
          Check boundary conditions.
        */
        if (rotate_info.x2 < 0)
          rotate_info.x2=0;
        else
          if (rotate_info.x2 > (int) windows->image.width)
            rotate_info.x2=windows->image.width;
        if (rotate_info.y2 < 0)
          rotate_info.y2=0;
        else
          if (rotate_info.y2 > (int) windows->image.height)
            rotate_info.y2=windows->image.height;
        /*
          Compute rotation angle from the slope of the line.
        */
        degrees=0.0;
        distance=
          ((rotate_info.x2-rotate_info.x1+1)*(rotate_info.x2-rotate_info.x1+1))+
          ((rotate_info.y2-rotate_info.y1+1)*(rotate_info.y2-rotate_info.y1+1));
        if (distance > 9)
          degrees=RadiansToDegrees(-atan2((double) (rotate_info.y2-
            rotate_info.y1),(double) (rotate_info.x2-rotate_info.x1)));
      } while (!(state & ExitState));
      (void) XSetFunction(display,windows->image.highlight_context,GXcopy);
      (void) XWithdrawWindow(display,windows->info.id,windows->info.screen);
      if (distance <= 9)
        return(True);
    }
  if (direction == VerticalRotateCommand)
    degrees-=90.0;
  if (degrees == 0.0)
    return(True);
  /*
    Rotate image.
  */
  normalized_degrees=degrees;
  while (normalized_degrees < -45.0)
    normalized_degrees+=360.0;
  for (rotations=0; normalized_degrees > 45.0; rotations++)
    normalized_degrees-=90.0;
  if (normalized_degrees != 0.0)
    (void) MagickXMagickCommand(display,resource_info,windows,ApplyCommand,image);
  MagickXSetCursorState(display,windows,True);
  MagickXCheckRefreshWindows(display,windows);
  (*image)->background_color.red=
    ScaleShortToQuantum(windows->pixel_info->pen_colors[pen_id].red);
  (*image)->background_color.green=
    ScaleShortToQuantum(windows->pixel_info->pen_colors[pen_id].green);
  (*image)->background_color.blue=
    ScaleShortToQuantum(windows->pixel_info->pen_colors[pen_id].blue);
  rotate_image=RotateImage(*image,degrees,&(*image)->exception);
  MagickXSetCursorState(display,windows,False);
  if (rotate_image == (Image *) NULL)
    return(False);
  DestroyImage(*image);
  *image=rotate_image;
  if (windows->image.crop_geometry != (char *) NULL)
    {
      /*
        Rotate crop geometry.
      */
      width=(unsigned int) (*image)->columns;
      height=(unsigned int) (*image)->rows;
      (void) XParseGeometry(windows->image.crop_geometry,&x,&y,&width,&height);
      switch (rotations % 4)
      {
        default:
        case 0:
          break;
        case 1:
        {
          /*
            Rotate 90 degrees.
          */
          FormatString(windows->image.crop_geometry,"%ux%u%+d%+d",
            height,width,(int) (*image)->columns-(int) height-y,x);
          break;
        }
        case 2:
        {
          /*
            Rotate 180 degrees.
          */
          FormatString(windows->image.crop_geometry,"%ux%u%+d%+d",
            width,height,(int) width-x,(int) height-y);
          break;
        }
        case 3:
        {
          /*
            Rotate 270 degrees.
          */
          FormatString(windows->image.crop_geometry,"%ux%u%+d%+d",
            height,width,y,(int) (*image)->rows-(int) width-x);
          break;
        }
      }
    }
  if (windows->image.orphan)
    return(True);
  if (normalized_degrees != 0.0)
    {
      /*
        Update image colormap.
      */
      windows->image.window_changes.width=(unsigned int) (*image)->columns;
      windows->image.window_changes.height=(unsigned int) (*image)->rows;
      if (windows->image.crop_geometry != (char *) NULL)
        {
          /*
            Obtain dimensions of image from crop geometry.
          */
          (void) XParseGeometry(windows->image.crop_geometry,&x,&y,
            &width,&height);
          windows->image.window_changes.width=width;
          windows->image.window_changes.height=height;
        }
      MagickXConfigureImageColormap(display,resource_info,windows,*image);
    }
  else
    if (((rotations % 4) == 1) || ((rotations % 4) == 3))
      {
        windows->image.window_changes.width=windows->image.ximage->height;
        windows->image.window_changes.height=windows->image.ximage->width;
      }
  /*
    Update image configuration.
  */
  (void) MagickXConfigureImage(display,resource_info,windows,*image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k X S a v e I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickXSaveImage saves an image to a file.
%
%  The format of the MagickXSaveImage method is:
%
%      unsigned int MagickXSaveImage(Display *display,MagickXResourceInfo *resource_info,
%        MagickXWindows *windows,Image *image)
%
%  A description of each parameter follows:
%
%    o status: Method MagickXSaveImage return True if the image is
%      written.  False is returned is there is a memory shortage or if the
%      image fails to write.
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 MagickXResourceInfo structure.
%
%    o windows: Specifies a pointer to a MagickXWindows structure.
%
%    o image: Specifies a pointer to an Image structure;  returned from
%      ReadImage.
%
%
*/
static unsigned int MagickXSaveImage(Display *display,MagickXResourceInfo *resource_info,
  MagickXWindows *windows,Image *image)
{
  char
    filename[MaxTextExtent],
    geometry[MaxTextExtent];

  Image
    *save_image;

  ImageInfo
    *image_info;

  int
    status;

  /*
    Request file name from user.
  */
  if (resource_info->write_filename != (char *) NULL)
    (void) strlcpy(filename,resource_info->write_filename,MaxTextExtent);
  else
    {
      char
        path[MaxTextExtent];

      GetPathComponent(image->filename,HeadPath,path);
      GetPathComponent(image->filename,TailPath,filename);
      (void) chdir(path);
    }
  MagickXFileBrowserWidget(display,windows,"Save",filename);
  if (*filename == '\0')
    return(True);
  if (IsAccessible(filename))
    {
      /*
        File exists-- seek user's permission before overwriting.
      */
      status=MagickXConfirmWidget(display,windows,"Overwrite",filename);
      if (status <= 0)
        return(True);
    }
  image_info=CloneImageInfo(resource_info->image_info);
  (void) strlcpy(image_info->filename,filename,MaxTextExtent);
  (void) SetImageInfo(image_info,False,&image->exception);
  if ((LocaleCompare(image_info->magick,"JPEG") == 0) ||
      (LocaleCompare(image_info->magick,"JPG") == 0))
    {
      char
        quality[MaxTextExtent];

      /*
        Request JPEG quality from user.
      */
      FormatString(quality,"%lu",image_info->quality);
      status=MagickXDialogWidget(display,windows,"Save","Enter JPEG quality:",
        quality);
      if (*quality == '\0')
        return(True);
      image_info->quality=atol(quality);
      image_info->interlace=status ? NoInterlace : PlaneInterlace;
    }
  if ((LocaleCompare(image_info->magick,"EPS") == 0) ||
      (LocaleCompare(image_info->magick,"PDF") == 0) ||
      (LocaleCompare(image_info->magick,"PS") == 0) ||
      (LocaleCompare(image_info->magick,"PS2") == 0))
    {
      char
        geometry[MaxTextExtent];

      /*
        Request page geometry from user.
      */
      FormatString(geometry,"%.1024s",PSPageGeometry);
      if (LocaleCompare(image_info->magick,"PDF") == 0)
        FormatString(geometry,"%.1024s",PSPageGeometry);
      if (image_info->page != (char *) NULL)
        (void) strlcpy(geometry,image_info->page,MaxTextExtent);
      MagickXListBrowserWidget(display,windows,&windows->widget,PageSizes,"Select",
        "Select page geometry:",geometry);
      if (*geometry != '\0')
        image_info->page=GetPageGeometry(geometry);
    }
  /*
    Apply image transforms.
  */
  MagickXSetCursorState(display,windows,True);
  MagickXCheckRefreshWindows(display,windows);
  save_image=CloneImage(image,0,0,True,&image->exception);
  if (save_image == (Image *) NULL)
    return(False);
  FormatString(geometry,"%dx%d!",windows->image.ximage->width,
    windows->image.ximage->height);
  TransformImage(&save_image,windows->image.crop_geometry,geometry);
  /*
    Write image.
  */
  (void) strlcpy(save_image->filename,filename,MaxTextExtent);
  status=WriteImage(image_info,save_image);
  if (status != False)
    image->taint=False;
  DestroyImage(save_image);
  DestroyImageInfo(image_info);
  MagickXSetCursorState(display,windows,False);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k X S c r e e n E v e n t                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickXScreenEvent handles global events associated with the Pan and
%  Magnify windows.
%
%  The format of the MagickXScreenEvent function is:
%
%      void MagickXScreenEvent(Display *display,MagickXWindows *windows,XEvent *event)
%
%  A description of each parameter follows:
%
%    o display: Specifies a pointer to the Display structure;  returned from
%      XOpenDisplay.
%
%    o windows: Specifies a pointer to a MagickXWindows structure.
%
%    o event: Specifies a pointer to a X11 XEvent structure.
%
%
*/

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

static int MagickXPredicate(Display *ARGUNUSED(display),XEvent *event,char *data)
{
  register MagickXWindows
    *windows;

  windows=(MagickXWindows *) data;
  if ((event->type == ClientMessage) &&
      (event->xclient.window == windows->image.id))
    return(False);
  return(True);
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

static void MagickXScreenEvent(Display *display,MagickXWindows *windows,XEvent *event)
{
  MonitorHandler
    handler;

  register int
    x,
    y;

  (void) XIfEvent(display,event,MagickXPredicate,(char *) windows);
  if (event->xany.window == windows->command.id)
    return;
  switch (event->type)
  {
    case ButtonPress:
    case ButtonRelease:
    {
      if ((event->xbutton.button == Button3) &&
          (event->xbutton.state & Mod1Mask))
        {
          /*
            Convert Alt-Button3 to Button2.
          */
          event->xbutton.button=Button2;
          event->xbutton.state&=(~Mod1Mask);
        }
      if (event->xbutton.window == windows->backdrop.id)
        {
          (void) XSetInputFocus(display,event->xbutton.window,RevertToParent,
            event->xbutton.time);
          break;
        }
      if (event->xbutton.window == windows->pan.id)
        {
          MagickXPanImage(display,windows,event);
          break;
        }
      if (event->xbutton.window == windows->image.id)
        if (event->xbutton.button == Button2)
          {
            /*
              Update magnified image.
            */
            x=event->xbutton.x;
            y=event->xbutton.y;
            if (x < 0)
              x=0;
            else
              if (x >= (int) windows->image.width)
                x=windows->image.width-1;
            windows->magnify.x=windows->image.x+x;
            if (y < 0)
              y=0;
            else
             if (y >= (int) windows->image.height)
               y=windows->image.height-1;
            windows->magnify.y=windows->image.y+y;
            if (!windows->magnify.mapped)
              (void) XMapRaised(display,windows->magnify.id);
            handler=SetMonitorHandler((MonitorHandler) NULL);
            MagickXMakeMagnifyImage(display,windows);
            (void) SetMonitorHandler(handler);
            if (event->type == ButtonRelease)
              (void) XWithdrawWindow(display,windows->info.id,
                windows->info.screen);
            break;
          }
      break;
    }
    case ClientMessage:
    {
      /*
        If client window delete message, exit.
      */
      if (event->xclient.message_type != windows->wm_protocols)
        break;
      if (*event->xclient.data.l != (long) windows->wm_delete_window)
        break;
      if (event->xclient.window == windows->magnify.id)
        {
          (void) XWithdrawWindow(display,windows->magnify.id,
            windows->magnify.screen);
          break;
        }
      break;
    }
    case ConfigureNotify:
    {
      if (event->xconfigure.window == windows->magnify.id)
        {
          unsigned int
            magnify;

          /*
            Magnify window has a new configuration.
          */
          windows->magnify.width=event->xconfigure.width;
          windows->magnify.height=event->xconfigure.height;
          if (!windows->magnify.mapped)
            break;
          magnify=1;
          while ((int) magnify <= event->xconfigure.width)
            magnify<<=1;
          while ((int) magnify <= event->xconfigure.height)
            magnify<<=1;
          magnify>>=1;
          if (((int) magnify != event->xconfigure.width) ||
              ((int) magnify != event->xconfigure.height))
            {
              XWindowChanges
                window_changes;

              window_changes.width=magnify;
              window_changes.height=magnify;
              (void) XReconfigureWMWindow(display,windows->magnify.id,
                windows->magnify.screen,CWWidth | CWHeight,&window_changes);
              break;
            }
          MagickXMakeMagnifyImage(display,windows);
          break;
        }
      break;
    }
    case Expose:
    {
      if (event->xexpose.window == windows->image.id)
        {
          MagickXRefreshWindow(display,&windows->image,event);
          break;
        }
      if (event->xexpose.window == windows->pan.id)
        if (event->xexpose.count == 0)
          {
            MagickXDrawPanRectangle(display,windows);
            break;
          }
      if (event->xexpose.window == windows->magnify.id)
        if (event->xexpose.count == 0)
          {
            MagickXMakeMagnifyImage(display,windows);
            break;
          }
      break;
    }
    case KeyPress:
    {
      char
        command[MaxTextExtent];

      KeySym
        key_symbol;

      if (event->xkey.window != windows->magnify.id)
        break;
      /*
        Respond to a user key press.
      */
      (void) XLookupString((XKeyEvent *) &event->xkey,command,sizeof(command),
        &key_symbol,(XComposeStatus *) NULL);
      MagickXMagnifyWindowCommand(display,windows,event->xkey.state,key_symbol);
      break;
    }
    case MapNotify:
    {
      if (event->xmap.window == windows->magnify.id)
        {
          windows->magnify.mapped=True;
          (void) XWithdrawWindow(display,windows->info.id,windows->info.screen);
          break;
        }
      if (event->xmap.window == windows->info.id)
        {
          windows->info.mapped=True;
          break;
        }
      break;
    }
    case MotionNotify:
    {
      while (XCheckMaskEvent(display,ButtonMotionMask,event));
      if (event->xmotion.window == windows->image.id)
        if (windows->magnify.mapped)
          {
            /*
              Update magnified image.
            */
            x=event->xmotion.x;
            y=event->xmotion.y;
            if (x < 0)
              x=0;
            else
              if (x >= (int) windows->image.width)
                x=windows->image.width-1;
            windows->magnify.x=windows->image.x+x;
            if (y < 0)
              y=0;
            else
             if (y >= (int) windows->image.height)
               y=windows->image.height-1;
            windows->magnify.y=windows->image.y+y;
            MagickXMakeMagnifyImage(display,windows);
          }
      break;
    }
    case UnmapNotify:
    {
      if (event->xunmap.window == windows->magnify.id)
        {
          windows->magnify.mapped=False;
          break;
        }
      if (event->xunmap.window == windows->info.id)
        {
          windows->info.mapped=False;
          break;
        }
      break;
    }
    default:
      break;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k X S e t C r o p G e o m e t r y                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickXSetCropGeometry accepts a cropping geometry relative to the
%  Image window and translates it to a cropping geometry relative to the
%  image.
%
%  The format of the MagickXSetCropGeometry method is:
%
%      void MagickXSetCropGeometry(Display *display,MagickXWindows *windows,
%        RectangleInfo *crop_info,Image *image)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o windows: Specifies a pointer to a MagickXWindows structure.
%
%    o crop_info:  A pointer to a RectangleInfo that defines a region of the
%      Image window to crop.
%
%    o image: Specifies a pointer to an Image structure.
%
%
*/
static void MagickXSetCropGeometry(Display *display,MagickXWindows *windows,
  RectangleInfo *crop_info,Image *image)
{
  char
    text[MaxTextExtent];

  double
    scale_factor;

  int
    x,
    y;

  unsigned int
    height,
    width;

  if (windows->info.mapped)
    {
      /*
        Display info on cropping rectangle.
      */
      FormatString(text," %lux%lu%+ld%+ld",crop_info->width,crop_info->height,
        crop_info->x,crop_info->y);
      MagickXInfoWidget(display,windows,text);
    }
  /*
    Cropping geometry is relative to any previous crop geometry.
  */
  x=0;
  y=0;
  width=(unsigned int) image->columns;
  height=(unsigned int) image->rows;
  if (windows->image.crop_geometry != (char *) NULL)
    (void) XParseGeometry(windows->image.crop_geometry,&x,&y,&width,&height);
  else
    windows->image.crop_geometry=AllocateString((char *) NULL);
  /*
    Define the crop geometry string from the cropping rectangle.
  */
  scale_factor=(double) width/windows->image.ximage->width;
  if (crop_info->x > 0)
    x+=(int) (scale_factor*crop_info->x+0.5);
  width=(unsigned int) (scale_factor*crop_info->width+0.5);
  if (width == 0)
    width=1;
  scale_factor=(double) height/windows->image.ximage->height;
  if (crop_info->y > 0)
    y+=(int) (scale_factor*crop_info->y+0.5);
  height=(unsigned int) (scale_factor*crop_info->height+0.5);
  if (height == 0)
    height=1;
  FormatString(windows->image.crop_geometry,"%ux%u%+d%+d",width,height,x,y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k X T i l e I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickXTileImage loads or deletes a selected tile from a visual
%  image directory.  The load or delete command is chosen from a menu.
%
%  The format of the MagickXTileImage method is:
%
%      Image *MagickXTileImage(Display *display,MagickXResourceInfo *resource_info,
%        MagickXWindows *windows,Image *image,XEvent *event)
%
%  A description of each parameter follows:
%
%    o tile_image:  MagickXTileImage reads or deletes the tile image
%      and returns it.  A null image is returned if an error occurs.
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 MagickXResourceInfo structure.
%
%    o windows: Specifies a pointer to a MagickXWindows structure.
%
%    o image: Specifies a pointer to an Image structure; returned from
%      ReadImage.
%
%    o event: Specifies a pointer to a XEvent structure.  If it is NULL,
%      the entire image is refreshed.
%
%
*/
static Image *MagickXTileImage(Display *display,MagickXResourceInfo *resource_info,
  MagickXWindows *windows,Image *image,XEvent *event)
{
  static const char
    *VerbMenu[]=
    {
      "Load",
      "Next",
      "Former",
      "Delete",
      "Update",
      (char *) NULL,
    };

  static const ModeType
    TileCommands[]=
    {
      TileLoadCommand,
      TileNextCommand,
      TileFormerCommand,
      TileDeleteCommand,
      TileUpdateCommand
    };

  char
    command[MaxTextExtent],
    filename[MaxTextExtent];

  double
    scale_factor;

  Image
    *tile_image;

  int
    id,
    status,
    tile,
    x,
    y;

  register char
    *p,
    *q;

  register int
    i;

  unsigned int
    height,
    width;

  /*
    Tile image is relative to montage image configuration.
  */
  x=0;
  y=0;
  width=(unsigned int) image->columns;
  height=(unsigned int) image->rows;
  if (windows->image.crop_geometry != (char *) NULL)
    (void) XParseGeometry(windows->image.crop_geometry,&x,&y,&width,&height);
  scale_factor=(double) width/windows->image.ximage->width;
  event->xbutton.x+=windows->image.x;
  event->xbutton.x=(int) (scale_factor*event->xbutton.x+x+0.5);
  scale_factor=(double) height/windows->image.ximage->height;
  event->xbutton.y+=windows->image.y;
  event->xbutton.y=(int) (scale_factor*event->xbutton.y+y+0.5);
  /*
    Determine size and location of each tile in the visual image directory.
  */
  width=(unsigned int) image->columns;
  height=(unsigned int) image->rows;
  x=0;
  y=0;
  (void) XParseGeometry(image->montage,&x,&y,&width,&height);
  tile=((event->xbutton.y-y)/height)*(((int) image->columns-x)/width)+
    (event->xbutton.x-x)/width;
  if (tile < 0)
    {
      /*
        Button press is outside any tile.
      */
      (void) XBell(display,0);
      return((Image *) NULL);
    }
  /*
    Determine file name from the tile directory.
  */
  p=image->directory;
  for (i=tile; (i != 0) && (*p != '\0'); )
  {
    if (*p == '\n')
      i--;
    p++;
  }
  if (*p == '\0')
    {
      /*
        Button press is outside any tile.
      */
      (void) XBell(display,0);
      return((Image *) NULL);
    }
  /*
    Select a command from the pop-up menu.
  */
  id=MagickXMenuWidget(display,windows,"Tile Verb",VerbMenu,command);
  if (id < 0)
    return((Image *) NULL);
  q=p;
  while ((*q != '\n') && (*q != '\0'))
    q++;
  (void) strncpy(filename,p,q-p);
  filename[q-p]='\0';
  /*
    Perform command for the selected tile.
  */
  MagickXSetCursorState(display,windows,True);
  MagickXCheckRefreshWindows(display,windows);
  tile_image=(Image *) NULL;
  switch (TileCommands[id])
  {
    case TileLoadCommand:
    {
      /*
        Load tile image.
      */
      MagickXCheckRefreshWindows(display,windows);
      (void) strcpy(resource_info->image_info->magick,"MIFF");
      (void) strlcpy(resource_info->image_info->filename,filename,
                     MaxTextExtent);
      tile_image=ReadImage(resource_info->image_info,&image->exception);
      if (image->exception.severity != UndefinedException)
        MagickError2(image->exception.severity,image->exception.reason,
          image->exception.description);
      (void) XWithdrawWindow(display,windows->info.id,windows->info.screen);
      break;
    }
    case TileNextCommand:
    {
      /*
        Display next image.
      */
      MagickXClientMessage(display,windows->image.id,windows->im_protocols,
        windows->im_next_image,CurrentTime);
      break;
    }
    case TileFormerCommand:
    {
      /*
        Display former image.
      */
      MagickXClientMessage(display,windows->image.id,windows->im_protocols,
        windows->im_former_image,CurrentTime);
      break;
    }
    case TileDeleteCommand:
    {
      /*
        Delete tile image.
      */
      if (!IsAccessible(filename))
        {
          MagickXNoticeWidget(display,windows,"Image file does not exist:",filename);
          break;
        }
      status=MagickXConfirmWidget(display,windows,"Really delete tile",filename);
      if (status <= 0)
        break;
      status=remove(filename);
      if (status != False)
        {
          MagickXNoticeWidget(display,windows,"Unable to delete image file:",
            filename);
          break;
        }
    }
    case TileUpdateCommand:
    {
      int
        x_offset,
        y_offset;

      PixelPacket
        pixel;

      register int
        j;

      register PixelPacket
        *s;

      /*
        Ensure all the images exist.
      */
      tile=0;
      for (p=image->directory; *p != '\0'; p++)
      {
        q=p;
        while ((*q != '\n') && (*q != '\0'))
          q++;
        (void) strncpy(filename,p,q-p);
        filename[q-p]='\0';
        p=q;
        if (IsAccessible(filename))
          {
            tile++;
            continue;
          }
        /*
          Overwrite tile with background color.
        */
        x_offset=width*(tile % (((int) image->columns-x)/width))+x;
        y_offset=height*(tile/(((int) image->columns-x)/width))+y;
        (void) AcquireOnePixelByReference(image,&pixel,0,0,&image->exception);
        for (i=0; i < (int) height; i++)
        {
          s=GetImagePixels(image,x_offset,y_offset+i,width,1);
          if (s == (PixelPacket *) NULL)
            break;
          for (j=0; j < (int) width; j++)
            *s++=pixel;
          if (!SyncImagePixels(image))
            break;
        }
        tile++;
      }
      windows->image.window_changes.width=(unsigned int) image->columns;
      windows->image.window_changes.height=(unsigned int) image->rows;
      MagickXConfigureImageColormap(display,resource_info,windows,image);
      (void) MagickXConfigureImage(display,resource_info,windows,image);
      break;
    }
    default:
      break;
  }
  MagickXSetCursorState(display,windows,False);
  return(tile_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k X T r a n s l a t e I m a g e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickXTranslateImage translates the image within an Image window
%  by one pixel as specified by the key symbol.  If the image has a `montage'
%  string the translation is respect to the width and height contained within
%  the string.
%
%  The format of the MagickXTranslateImage method is:
%
%      void MagickXTranslateImage(Display *display,MagickXWindows *windows,
%        Image *image,const KeySym key_symbol)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o windows: Specifies a pointer to a MagickXWindows structure.
%
%    o image: Specifies a pointer to an Image structure;  returned from
%      ReadImage.
%
%    o key_symbol: Specifies a KeySym which indicates which side of the image
%      to trim.
%
%
*/
static void MagickXTranslateImage(Display *display,MagickXWindows *windows,
  Image *image,const KeySym key_symbol)
{
  char
    text[MaxTextExtent];

  int
    x,
    y;

  unsigned int
    x_offset,
    y_offset;

  /*
    User specified a pan position offset.
  */
  x_offset=windows->image.width;
  y_offset=windows->image.height;
  if (image->montage != (char *) NULL)
    (void) XParseGeometry(image->montage,&x,&y,&x_offset,&y_offset);
  switch ((int) key_symbol)
  {
    case XK_Home:
    case XK_KP_Home:
    {
      windows->image.x=windows->image.width/2;
      windows->image.y=windows->image.height/2;
      break;
    }
    case XK_Left:
    case XK_KP_Left:
    {
      windows->image.x-=x_offset;
      break;
    }
    case XK_Next:
    case XK_Up:
    case XK_KP_Up:
    {
      windows->image.y-=y_offset;
      break;
    }
    case XK_Right:
    case XK_KP_Right:
    {
      windows->image.x+=x_offset;
      break;
    }
    case XK_Prior:
    case XK_Down:
    case XK_KP_Down:
    {
      windows->image.y+=y_offset;
      break;
    }
    default:
      return;
  }
  /*
    Check boundary conditions.
  */
  if (windows->image.x < 0)
    windows->image.x=0;
  else
    if ((int) (windows->image.x+windows->image.width) >
        windows->image.ximage->width)
      windows->image.x=windows->image.ximage->width-windows->image.width;
  if (windows->image.y < 0)
    windows->image.y=0;
  else
    if ((int) (windows->image.y+windows->image.height) >
        windows->image.ximage->height)
      windows->image.y=windows->image.ximage->height-windows->image.height;
  /*
    Refresh Image window.
  */
  FormatString(text," %ux%u%+d%+d ",windows->image.width,
    windows->image.height,windows->image.x,windows->image.y);
  MagickXInfoWidget(display,windows,text);
  MagickXCheckRefreshWindows(display,windows);
  MagickXDrawPanRectangle(display,windows);
  MagickXRefreshWindow(display,&windows->image,(XEvent *) NULL);
  (void) XWithdrawWindow(display,windows->info.id,windows->info.screen);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k X T r i m I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickXTrimImage trims the edges from the Image window.
%
%  The format of the MagickXTrimImage method is:
%
%      unsigned int MagickXTrimImage(Display *display,MagickXResourceInfo *resource_info,
%        MagickXWindows *windows,Image *image)
%
%  A description of each parameter follows:
%
%    o status: Method MagickXTrimImage returns True if the image is
%      cropped.  False is returned is there is a memory shortage or if the
%      image fails to be cropped.
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 MagickXResourceInfo structure.
%
%    o windows: Specifies a pointer to a MagickXWindows structure.
%
%    o image: Specifies a pointer to an Image structure.
%
%
*/
static unsigned int MagickXTrimImage(Display *display,MagickXResourceInfo *resource_info,
  MagickXWindows *windows,Image *image)
{
  RectangleInfo
    trim_info;

  register int
    x,
    y;

  unsigned long
    background,
    pixel;

  /*
    Trim edges from image.
  */
  MagickXSetCursorState(display,windows,True);
  MagickXCheckRefreshWindows(display,windows);
  /*
    Crop the left edge.
  */
  background=XGetPixel(windows->image.ximage,0,0);
  trim_info.width=windows->image.ximage->width;
  for (x=0; x < windows->image.ximage->width; x++)
  {
    for (y=0; y < windows->image.ximage->height; y++)
    {
      pixel=XGetPixel(windows->image.ximage,x,y);
      if (pixel != background)
        break;
    }
    if (y < windows->image.ximage->height)
      break;
  }
  trim_info.x=x;
  if (trim_info.x == (int) windows->image.ximage->width)
    {
      MagickXSetCursorState(display,windows,False);
      return(False);
    }
  /*
    Crop the right edge.
  */
  background=XGetPixel(windows->image.ximage,windows->image.ximage->width-1,0);
  for (x=windows->image.ximage->width-1; x > 0; x--)
  {
    for (y=0; y < windows->image.ximage->height; y++)
    {
      pixel=XGetPixel(windows->image.ximage,x,y);
      if (pixel != background)
        break;
    }
    if (y < windows->image.ximage->height)
      break;
  }
  trim_info.width=x-trim_info.x+1;
  /*
    Crop the top edge.
  */
  background=XGetPixel(windows->image.ximage,0,0);
  trim_info.height=windows->image.ximage->height;
  for (y=0; y < windows->image.ximage->height; y++)
  {
    for (x=0; x < windows->image.ximage->width; x++)
    {
      pixel=XGetPixel(windows->image.ximage,x,y);
      if (pixel != background)
        break;
    }
    if (x < windows->image.ximage->width)
      break;
  }
  trim_info.y=y;
  /*
    Crop the bottom edge.
  */
  background=XGetPixel(windows->image.ximage,0,windows->image.ximage->height-1);
  for (y=windows->image.ximage->height-1; y > 0; y--)
  {
    for (x=0; x < windows->image.ximage->width; x++)
    {
      pixel=XGetPixel(windows->image.ximage,x,y);
      if (pixel != background)
        break;
    }
    if (x < windows->image.ximage->width)
      break;
  }
  trim_info.height=y-trim_info.y+1;
  if (((unsigned int) trim_info.width != windows->image.width) ||
      ((unsigned int) trim_info.height != windows->image.height))
    {
      /*
        Reconfigure Image window as defined by the trimming rectangle.
      */
      MagickXSetCropGeometry(display,windows,&trim_info,image);
      windows->image.window_changes.width=(unsigned int) trim_info.width;
      windows->image.window_changes.height=(unsigned int) trim_info.height;
      (void) MagickXConfigureImage(display,resource_info,windows,image);
    }
  MagickXSetCursorState(display,windows,False);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k X V i s u a l D i r e c t o r y I m a g e                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickXVisualDirectoryImage creates a Visual Image Directory.
%
%  The format of the MagickXVisualDirectoryImage method is:
%
%      Image *MagickXVisualDirectoryImage(Display *display,
%        MagickXResourceInfo *resource_info,MagickXWindows *windows)
%
%  A description of each parameter follows:
%
%    o nexus: Method MagickXVisualDirectoryImage returns a visual image
%      directory if it can be created successfully.  Otherwise a null image
%      is returned.
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 MagickXResourceInfo structure.
%
%    o windows: Specifies a pointer to a MagickXWindows structure.
%
%
*/
static Image *MagickXVisualDirectoryImage(Display *display,
  MagickXResourceInfo *resource_info,MagickXWindows *windows)
{
#define TileImageText  "  Scale image tiles...  "
#define XClientName  "montage"

  char
    **filelist,
    window_id[MaxTextExtent];

  ExceptionInfo
    exception;

  Image
    *image,
    *montage_image,
    *next_image,
    *thumbnail_image;

  ImageInfo
    *clone_info;

  int
    number_files;

  MonitorHandler
    handler;

  MontageInfo
    *montage_info;

  RectangleInfo
    geometry;

  register int
    i;

  static char
    filename[MaxTextExtent] = "\0",
    filenames[MaxTextExtent] = "*";

  unsigned int
    backdrop,
    status;

  MagickXResourceInfo
    background_resources;

  /*
    Request file name from user.
  */
  MagickXFileBrowserWidget(display,windows,"Directory",filenames);
  if (*filenames == '\0')
    return((Image *) NULL);
  /*
    Expand the filenames.
  */
  filelist=MagickAllocateMemory(char **,sizeof(char *));
  if (filelist == (char **) NULL)
    {
      MagickError(ResourceLimitError,MemoryAllocationFailed,(char *) NULL);
      return((Image *) NULL);
    }
  number_files=1;
  filelist[0]=filenames;
  status=ExpandFilenames(&number_files,&filelist);
  if ((status == False) || (number_files == 0))
    {
      if (number_files == 0)
        MagickError(ImageError,NoImagesWereFound,filenames);
      else
        MagickError(ResourceLimitError,MemoryAllocationFailed,filenames);
      return((Image *) NULL);
    }
  /*
    Set image background resources.
  */
  background_resources=(*resource_info);
  background_resources.window_id=window_id;
  FormatString(background_resources.window_id,"0x%lx",windows->image.id);
  background_resources.backdrop=True;
  /*
    Read each image and convert them to a tile.
  */
  backdrop=(windows->visual_info->class == TrueColor) ||
   (windows->visual_info->class == DirectColor);
  clone_info=CloneImageInfo(resource_info->image_info);
  if (clone_info == (ImageInfo *) NULL)
    return((Image *) NULL);
  image=(Image *) NULL;
  GetExceptionInfo(&exception);
  MagickXSetCursorState(display,windows,True);
  MagickXCheckRefreshWindows(display,windows);
  for (i=0; i < number_files; i++)
  {
    handler=SetMonitorHandler((MonitorHandler) NULL);
    (void) strlcpy(clone_info->filename,filelist[i],MaxTextExtent);
    *clone_info->magick='\0';
    (void) CloneString(&clone_info->size,DefaultTileGeometry);
    next_image=ReadImage(clone_info,&exception);
    if (exception.severity != UndefinedException)
      CatchException(&exception);
    if (filelist[i] != filenames)
      MagickFreeMemory(filelist[i]);
    if (next_image != (Image *) NULL)
      {
        (void) SetImageAttribute(next_image,"label",(char *) NULL);
        (void) SetImageAttribute(next_image,"label",DefaultTileLabel);
        SetGeometry(next_image,&geometry);
        (void) GetMagickGeometry(clone_info->size,&geometry.x,&geometry.y,
          &geometry.width,&geometry.height);
        thumbnail_image=ThumbnailImage(next_image,geometry.width,
          geometry.height,&exception);
        if (thumbnail_image != (Image *) NULL)
          {
            DestroyImage(next_image);
            next_image=thumbnail_image;
          }
        if (backdrop)
          {
            (void) MagickXDisplayBackgroundImage(display,&background_resources,
              next_image);
            MagickXSetCursorState(display,windows,True);
          }
        if (image == (Image *) NULL)
          image=next_image;
        else
          {
            image->next=next_image;
            image->next->previous=image;
            image=image->next;
          }
      }
    (void) SetMonitorHandler(handler);
    if (!MagickMonitorFormatted(i,number_files,&image->exception,
                                LoadImageText,image->filename))
      break;
  }
  DestroyImageInfo(clone_info);
  MagickFreeMemory(filelist);
  if (image == (Image *) NULL)
    {
      MagickXSetCursorState(display,windows,False);
      MagickError(ImageError,NoImagesWereLoaded,filenames);
      return((Image *) NULL);
    }
  while (image->previous != (Image *) NULL)
    image=image->previous;
  /*
    Create the Visual Image Directory.
  */
  montage_info=CloneMontageInfo(resource_info->image_info,(MontageInfo *) NULL);
  if (resource_info->font != (char *) NULL)
    (void) CloneString(&montage_info->font,resource_info->font);
  (void) strlcpy(montage_info->filename,filename,MaxTextExtent);
  montage_image=MontageImages(image,montage_info,&image->exception);
  DestroyMontageInfo(montage_info);
  DestroyImageList(image);
  MagickXSetCursorState(display,windows,False);
  if (montage_image == (Image *) NULL)
    return(montage_image);
  MagickXClientMessage(display,windows->image.id,windows->im_protocols,
    windows->im_next_image,CurrentTime);
  return(montage_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k X D i s p l a y B a c k g r o u n d I m a g e                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickXDisplayBackgroundImage() displays an image in the background of a window.
%
%  The format of the MagickXDisplayBackgroundImage method is:
%
%      unsigned int MagickXDisplayBackgroundImage(Display *display,
%        MagickXResourceInfo *resource_info,Image *image)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 MagickXResourceInfo structure.
%
%    o image: Specifies a pointer to an Image structure; returned from
%      ReadImage.
%
%
*/
MagickExport unsigned int MagickXDisplayBackgroundImage(Display *display,
  MagickXResourceInfo *resource_info,Image *image)
{
  char
    geometry[MaxTextExtent],
    visual_type[MaxTextExtent];

  long
    x,
    y;

  static MagickXPixelInfo
    pixel;

  static XStandardColormap
    *map_info;

  static XVisualInfo
    *visual_info = (XVisualInfo *) NULL;

  static MagickXWindowInfo
    window_info;

  unsigned int
    status;

  unsigned int
    height,
    width;

  Window
    root_window;

  XGCValues
    context_values;

  MagickXResourceInfo
    resources;

  XWindowAttributes
    window_attributes;

  /*
    Determine target window.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  resources=(*resource_info);
  window_info.id=(Window) NULL;
  root_window=XRootWindow(display,XDefaultScreen(display));
  if (LocaleCompare(resources.window_id,"root") == 0)
    window_info.id=root_window;
  else
    {
      if (isdigit((int) *resources.window_id))
        window_info.id=MagickXWindowByID(display,root_window,
          (Window) strtol((char *) resources.window_id,(char **) NULL,0));
      if (window_info.id == (Window) NULL)
        window_info.id=
          MagickXWindowByName(display,root_window,resources.window_id);
    }
  if (window_info.id == (Window) NULL)
    {
      MagickError(XServerError,NoWindowWithSpecifiedIDExists,
        resources.window_id);
      return(False);
    }
  /*
    Determine window visual id.
  */
  window_attributes.width=XDisplayWidth(display,XDefaultScreen(display));
  window_attributes.height=XDisplayHeight(display,XDefaultScreen(display));
  (void) strcpy(visual_type,"default");
  status=XGetWindowAttributes(display,window_info.id,&window_attributes);
  if (status != False)
    FormatString(visual_type,"0x%lx",
      XVisualIDFromVisual(window_attributes.visual));
  if (visual_info == (XVisualInfo *) NULL)
    {
      /*
        Allocate standard colormap.
      */
      map_info=XAllocStandardColormap();
      if (map_info == (XStandardColormap *) NULL)
        MagickFatalError(ResourceLimitError,MemoryAllocationFailed,
          MagickMsg(XServerFatalError,UnableToCreateStandardColormap));
      map_info->colormap=(Colormap) NULL;
      pixel.pixels=(unsigned long *) NULL;
      /*
        Initialize visual info.
      */
      resources.map_type=(char *) NULL;
      resources.visual_type=visual_type;
      visual_info=MagickXBestVisualInfo(display,map_info,&resources);
      if (visual_info == (XVisualInfo *) NULL)
        MagickFatalError(XServerFatalError,UnableToGetVisual,
          resources.visual_type);
      /*
        Initialize window info.
      */
      window_info.ximage=(XImage *) NULL;
      window_info.matte_image=(XImage *) NULL;
      window_info.pixmap=(Pixmap) NULL;
      window_info.matte_pixmap=(Pixmap) NULL;
    }
  /*
    Free previous root colors.
  */
  if (window_info.id == root_window)
    (void) MagickXDestroyWindowColors(display,root_window);
  /*
    Initialize Standard Colormap.
  */
  resources.colormap=SharedColormap;
  MagickXMakeStandardColormap(display,visual_info,&resources,image,map_info,&pixel);
  /*
    Graphic context superclass.
  */
  context_values.background=pixel.background_color.pixel;
  context_values.foreground=pixel.foreground_color.pixel;
  pixel.annotate_context=XCreateGC(display,window_info.id,GCBackground |
    GCForeground,&context_values);
  if (pixel.annotate_context == (GC) NULL)
    MagickFatalError(XServerFatalError,UnableToCreateGraphicContext,
      (char *) NULL);
  /*
    Initialize Image window attributes.
  */
  MagickXGetWindowInfo(display,visual_info,map_info,&pixel,(XFontStruct *) NULL,
    &resources,&window_info);
  /*
    Create the X image.
  */
  window_info.width=(unsigned int) image->columns;
  window_info.height=(unsigned int) image->rows;
  FormatString(geometry,"%ux%u+0+0>",window_attributes.width,
    window_attributes.height);
  width=window_info.width;
  height=window_info.height;
  x=window_info.x;
  y=window_info.y;
  {
    unsigned long
      geometry_width=width,
      geometry_height=height;

    (void) GetMagickGeometry(geometry,&x,&y,&geometry_width,&geometry_height);
    width=(unsigned int) geometry_width;
    height=(unsigned int) geometry_height;
  }
  window_info.width=(unsigned int) width;
  window_info.height=(unsigned int) height;
  window_info.x=(int) x;
  window_info.y=(int) y;
  status=MagickXMakeImage(display,&resources,&window_info,image,window_info.width,
    window_info.height);
  if (status == False)
    MagickFatalError(XServerFatalError,UnableToCreateXImage,(char *) NULL);
  window_info.x=0;
  window_info.y=0;
  if (IsEventLogging())
    {
      (void) LogMagickEvent(X11Event,GetMagickModule(),
        "Image: %.1024s[%lu] %lux%lu ",image->filename,image->scene,
        image->columns,image->rows);
      if (image->colors != 0)
        (void) LogMagickEvent(X11Event,GetMagickModule(),"%uc ",image->colors);
      (void) LogMagickEvent(X11Event,GetMagickModule(),"%.1024s",image->magick);
    }
  /*
    Adjust image dimensions as specified by backdrop or geometry options.
  */
  width=window_info.width;
  height=window_info.height;
  if (resources.backdrop)
    {
      /*
        Center image on window.
      */
      window_info.x=(window_attributes.width/2)-
        (window_info.ximage->width/2);
      window_info.y=(window_attributes.height/2)-
        (window_info.ximage->height/2);
      width=window_attributes.width;
      height=window_attributes.height;
    }
  if (resources.image_geometry != (char *) NULL)
    {
      char
        default_geometry[MaxTextExtent];

      int
        flags,
        gravity;

      XSizeHints
        *size_hints;

      /*
        User specified geometry.
      */
      size_hints=XAllocSizeHints();
      if (size_hints == (XSizeHints *) NULL)
        MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
          UnableToDisplayImage);
      size_hints->flags=(long) NULL;
      FormatString(default_geometry,"%ux%u",width,height);
      flags=XWMGeometry(display,visual_info->screen,resources.image_geometry,
        default_geometry,window_info.border_width,size_hints,&window_info.x,
        &window_info.y,(int *) &width,(int *) &height,&gravity);
      if (flags & (XValue | YValue))
        {
          width=window_attributes.width;
          height=window_attributes.height;
        }
      (void) XFree((void *) size_hints);
    }
  /*
    Create the X pixmap.
  */
  window_info.pixmap=XCreatePixmap(display,window_info.id,(unsigned int) width,
    (unsigned int) height,window_info.depth);
  if (window_info.pixmap == (Pixmap) NULL)
    MagickFatalError(XServerFatalError,UnableToCreateXPixmap,(char *) NULL);
  /*
    Display pixmap on the window.
  */
  if (((unsigned int) width > window_info.width) ||
      ((unsigned int) height > window_info.height))
    (void) XFillRectangle(display,window_info.pixmap,
      window_info.annotate_context,0,0,(unsigned int) width,
      (unsigned int) height);
  (void) XPutImage(display,window_info.pixmap,window_info.annotate_context,
    window_info.ximage,0,0,window_info.x,window_info.y,
    (unsigned int) window_info.width,(unsigned int) window_info.height);
  (void) XSetWindowBackgroundPixmap(display,window_info.id,window_info.pixmap);
  (void) XClearWindow(display,window_info.id);
  MagickXDelay(display,10*image->delay);
  (void) XSync(display,False);
  return(window_info.id == root_window);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k X D i s p l a y I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickXDisplayImage() displays an image via X11.  A new image is created and
%  returned if the user interactively transforms the displayed image.
%
%  The format of the MagickXDisplayImage method is:
%
%      Image *MagickXDisplayImage(Display *display,MagickXResourceInfo *resource_info,
%        char **argv,int argc,Image **image,unsigned long *state)
%
%  A description of each parameter follows:
%
%    o nexus:  Method MagickXDisplayImage returns an image when the
%      user chooses 'Open Image' from the command menu or picks a tile
%      from the image directory.  Otherwise a null image is returned.
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 MagickXResourceInfo structure.
%
%    o argv: Specifies the application's argument list.
%
%    o argc: Specifies the number of arguments.
%
%    o image: Specifies an address to an address of an Image structure;
%      returned from ReadImage.
%
%
*/
#define MagnifySize  256  /* must be a power of 2 */
#define MagickMenus  10
#define MagickTitle  "Commands"
#define DestroyWindowsImage(windows,subwindow) \
{ \
  if ((windows->subwindow.image != (Image *) NULL) && \
      (windows->subwindow.destroy == True)) \
    { \
      DestroyImage(windows->subwindow.image); \
      windows->subwindow.image=(Image *) NULL;    \
    } \
}

MagickExport Image *MagickXDisplayImage(Display *display,MagickXResourceInfo *resource_info,
  char **argv,int argc,Image **image,unsigned long *state)
{

  static const char
    *CommandMenu[]=
    {
      "File",
      "Edit",
      "View",
      "Transform",
      "Enhance",
      "Effects",
      "F/X",
      "Image Edit",
      "Miscellany",
      "Help",
      (char *) NULL
    },
    *FileMenu[]=
    {
      "Open...",
      "Next",
      "Former",
      "Select...",
      "Save...",
      "Print...",
      "Delete...",
      "New...",
      "Visual Directory...",
      "Quit",
      (char *) NULL
    },
    *EditMenu[]=
    {
      "Undo",
      "Redo",
      "Cut",
      "Copy",
      "Paste",
      (char *) NULL
    },
    *ViewMenu[]=
    {
      "Half Size",
      "Original Size",
      "Double Size",
      "Resize...",
      "Apply",
      "Refresh",
      "Restore",
      (char *) NULL
    },
    *TransformMenu[]=
    {
      "Crop",
      "Chop",
      "Flop",
      "Flip",
      "Rotate Right",
      "Rotate Left",
      "Rotate...",
      "Shear...",
      "Roll...",
      "Trim Edges",
      (char *) NULL
    },
    *EnhanceMenu[]=
    {
      "Hue...",
      "Saturation...",
      "Brightness...",
      "Gamma...",
      "Spiff",
      "Dull",
      "Equalize",
      "Normalize",
      "Negate",
      "Grayscale",
      "Map...",
      "Quantize...",
      (char *) NULL
    },
    *EffectsMenu[]=
    {
      "Despeckle",
      "Emboss",
      "Reduce Noise",
      "Add Noise...",
      "Sharpen...",
      "Blur...",
      "Threshold...",
      "Edge Detect...",
      "Spread...",
      "Shade...",
      "Raise...",
      "Segment...",
      (char *) NULL
    },
    *FXMenu[]=
    {
      "Solarize...",
      "Swirl...",
      "Implode...",
      "Wave...",
      "Oil Paint...",
      "Charcoal Draw...",
      (char *) NULL
    },
    *ImageEditMenu[]=
    {
      "Annotate...",
      "Draw...",
      "Color...",
      "Matte...",
      "Composite...",
      "Add Border...",
      "Add Frame...",
      "Comment...",
      "Launch...",
      "Region of Interest...",
      (char *) NULL
    },
    *MiscellanyMenu[]=
    {
      "Image Info",
      "Zoom Image",
      "Show Preview...",
      "Show Histogram",
      "Show Matte",
      "Background...",
      "Slide Show...",
      "Preferences...",
      (char *) NULL
    },
    *HelpMenu[]=
    {
      "Overview",
      "Browse Documentation",
      "About Display",
      (char *) NULL
    },
    *ShortCutsMenu[]=
    {
      "Next",
      "Former",
      "Open...",
      "Save...",
      "Print...",
      "Undo",
      "Restore",
      "Image Info",
      "Quit",
      (char *) NULL
    },
    *ImmutableMenu[]=
    {
      "Image Info",
      "Print",
      "Next",
      "Quit",
      (char *) NULL
    };

  static const char
    **Menus[MagickMenus]=
    {
      FileMenu,
      EditMenu,
      ViewMenu,
      TransformMenu,
      EnhanceMenu,
      EffectsMenu,
      FXMenu,
      ImageEditMenu,
      MiscellanyMenu,
      HelpMenu
    };

  static CommandType
    CommandMenus[]=
    {
      NullCommand,
      NullCommand,
      NullCommand,
      NullCommand,
      NullCommand,
      NullCommand,
      NullCommand,
      NullCommand,
      NullCommand,
      NullCommand,
    },
    FileCommands[]=
    {
      OpenCommand,
      NextCommand,
      FormerCommand,
      SelectCommand,
      SaveCommand,
      PrintCommand,
      DeleteCommand,
      NewCommand,
      VisualDirectoryCommand,
      QuitCommand
    },
    EditCommands[]=
    {
      UndoCommand,
      RedoCommand,
      CutCommand,
      CopyCommand,
      PasteCommand
    },
    ViewCommands[]=
    {
      HalfSizeCommand,
      OriginalSizeCommand,
      DoubleSizeCommand,
      ResizeCommand,
      ApplyCommand,
      RefreshCommand,
      RestoreCommand
    },
    TransformCommands[]=
    {
      CropCommand,
      ChopCommand,
      FlopCommand,
      FlipCommand,
      RotateRightCommand,
      RotateLeftCommand,
      RotateCommand,
      ShearCommand,
      RollCommand,
      TrimCommand
    },
    EnhanceCommands[]=
    {
      HueCommand,
      SaturationCommand,
      BrightnessCommand,
      GammaCommand,
      SpiffCommand,
      DullCommand,
      EqualizeCommand,
      NormalizeCommand,
      NegateCommand,
      GrayscaleCommand,
      MapCommand,
      QuantizeCommand
    },
    EffectsCommands[]=
    {
      DespeckleCommand,
      EmbossCommand,
      ReduceNoiseCommand,
      AddNoiseCommand,
      SharpenCommand,
      BlurCommand,
      ThresholdCommand,
      EdgeDetectCommand,
      SpreadCommand,
      ShadeCommand,
      RaiseCommand,
      SegmentCommand
    },
    FXCommands[]=
    {
      SolarizeCommand,
      SwirlCommand,
      ImplodeCommand,
      WaveCommand,
      OilPaintCommand,
      CharcoalDrawCommand
    },
    ImageEditCommands[]=
    {
      AnnotateCommand,
      DrawCommand,
      ColorCommand,
      MatteCommand,
      CompositeCommand,
      AddBorderCommand,
      AddFrameCommand,
      CommentCommand,
      LaunchCommand,
      RegionofInterestCommand
    },
    MiscellanyCommands[]=
    {
      InfoCommand,
      ZoomCommand,
      ShowPreviewCommand,
      ShowHistogramCommand,
      ShowMatteCommand,
      BackgroundCommand,
      SlideShowCommand,
      PreferencesCommand
    },
    HelpCommands[]=
    {
      HelpCommand,
      BrowseDocumentationCommand,
      VersionCommand
    },
    ShortCutsCommands[]=
    {
      NextCommand,
      FormerCommand,
      OpenCommand,
      SaveCommand,
      PrintCommand,
      UndoCommand,
      RestoreCommand,
      InfoCommand,
      QuitCommand
    },
    ImmutableCommands[]=
    {
      InfoCommand,
      PrintCommand,
      NextCommand,
      QuitCommand
    };

  static CommandType
    *Commands[MagickMenus]=
    {
      FileCommands,
      EditCommands,
      ViewCommands,
      TransformCommands,
      EnhanceCommands,
      EffectsCommands,
      FXCommands,
      ImageEditCommands,
      MiscellanyCommands,
      HelpCommands
    };

  char
    command[MaxTextExtent],
    geometry[MaxTextExtent],
    resource_name[MaxTextExtent];

  CommandType
    command_type;

  Image
    *display_image,
    *nexus;

  int
    entry,
    id,
    status;

  long
    x,
    y;

  KeySym
    key_symbol;

  MonitorHandler
    handler,
    monitor_handler;

  register int
    i;

  static char
    working_directory[MaxTextExtent];

  static XPoint
    vid_info;

  static MagickXWindowInfo
    *magick_windows[MaxXWindows];

  static unsigned int
    number_windows;

  struct stat
    file_info;

  time_t
    timer,
    timestamp,
    update_time;

  unsigned int
    context_mask;

  unsigned long
    height,
    width;

  WarningHandler
    warning_handler;

  Window
    root_window;

  XClassHint
    *class_hints;

  XEvent
    event;

  XFontStruct
    *font_info;

  XGCValues
    context_values;

  MagickXPixelInfo
    *icon_pixel,
    *pixel;

  MagickXResourceInfo
    *icon_resources;

  XStandardColormap
    *icon_map,
    *map_info;

  XVisualInfo
    *icon_visual,
    *visual_info;

  XWindowChanges
    window_changes;

  MagickXWindows
    *windows;

  XWMHints
    *manager_hints;

  assert(image != (Image **) NULL);
  assert((*image)->signature == MagickSignature);
  display_image=(*image);
  (void) TransformColorspace(display_image,RGBColorspace);
  monitor_handler=(MonitorHandler) NULL;
  warning_handler=(WarningHandler) NULL;
  windows=MagickXSetWindows((MagickXWindows *) ~0);
  if (windows != (MagickXWindows *) NULL)
    {
      /*
        Change to the working directory.
      */
      (void) chdir(working_directory);
      /*
        Set the progress monitor if progress monitoring is requested.
      */
      if (resource_info->image_info->progress)
        monitor_handler=SetMonitorHandler(MagickXMagickMonitor);
      /*
        Set the warning and signal handlers.
      */
      warning_handler=resource_info->display_warnings ?
        SetErrorHandler(MagickXWarning) : SetErrorHandler((ErrorHandler) NULL);
      warning_handler=resource_info->display_warnings ?
        SetWarningHandler(MagickXWarning) : SetWarningHandler((WarningHandler) NULL);
/*       (void) signal(SIGINT,MagickXSignalHandler); */
/*       (void) signal(SIGSEGV,MagickXSignalHandler); */
/*       (void) signal(SIGTERM,MagickXSignalHandler); */
    }
  else
    {
      /*
        Allocate windows structure.
      */
      resource_info->colors=display_image->colors;
      windows=MagickXSetWindows(MagickXInitializeWindows(display,resource_info));
      if (windows == (MagickXWindows *) NULL)
        MagickFatalError3(ResourceLimitError,MemoryAllocationFailed,
          UnableToCreateXWindow);
      /*
        Initialize window id's.
      */
      number_windows=0;
      magick_windows[number_windows++]=(&windows->icon);
      magick_windows[number_windows++]=(&windows->backdrop);
      magick_windows[number_windows++]=(&windows->image);
      magick_windows[number_windows++]=(&windows->info);
      magick_windows[number_windows++]=(&windows->command);
      magick_windows[number_windows++]=(&windows->widget);
      magick_windows[number_windows++]=(&windows->popup);
      magick_windows[number_windows++]=(&windows->magnify);
      magick_windows[number_windows++]=(&windows->pan);
      magick_windows[number_windows]=(MagickXWindowInfo *) NULL;
      for (i=0; i < (int) number_windows; i++)
        magick_windows[i]->id=(Window) NULL;
      vid_info.x=0;
      vid_info.y=0;
    }
  /*
    Initialize font info.
  */
  if (windows->font_info != (XFontStruct *) NULL)
    (void) XFreeFont(display,windows->font_info);
  windows->font_info=MagickXBestFont(display,resource_info,False);
  if (windows->font_info == (XFontStruct *) NULL)
    MagickFatalError(XServerFatalError,UnableToLoadFont,resource_info->font);
  /*
    Initialize Standard Colormap.
  */
  map_info=windows->map_info;
  icon_map=windows->icon_map;
  visual_info=windows->visual_info;
  icon_visual=windows->icon_visual;
  pixel=windows->pixel_info;
  icon_pixel=windows->icon_pixel;
  font_info=windows->font_info;
  icon_resources=windows->icon_resources;
  class_hints=windows->class_hints;
  manager_hints=windows->manager_hints;
  root_window=XRootWindow(display,visual_info->screen);
  nexus=(Image *) NULL;
  if (IsEventLogging())
    {
      (void) LogMagickEvent(X11Event,GetMagickModule(),
        "Image: %.1024s[%lu] %lux%lu ",display_image->filename,
        display_image->scene,display_image->columns,display_image->rows);
      if (display_image->colors != 0)
        (void) LogMagickEvent(X11Event,GetMagickModule(),"%uc ",
          display_image->colors);
      (void) LogMagickEvent(X11Event,GetMagickModule(),"%.1024s",
        display_image->magick);
    }
  MagickXMakeStandardColormap(display,visual_info,resource_info,display_image,
    map_info,pixel);
  display_image->taint=False;
  /*
    Initialize graphic context.
  */
  windows->context.id=(Window) NULL;
  MagickXGetWindowInfo(display,visual_info,map_info,pixel,font_info,
    resource_info,&windows->context);
  class_hints->res_name=(char *) "superclass";
  class_hints->res_class=(char *) "Display";
  manager_hints->flags=InputHint | StateHint;
  manager_hints->input=False;
  manager_hints->initial_state=WithdrawnState;
  MagickXMakeWindow(display,root_window,argv,argc,class_hints,manager_hints,
    &windows->context);
  if (IsEventLogging())
    (void) LogMagickEvent(X11Event,GetMagickModule(),"Window id: 0x%lx (context)",
      windows->context.id);
  (void) memset(&context_values,0,sizeof(context_values));
  context_values.background=pixel->background_color.pixel;
  context_values.font=font_info->fid;
  context_values.foreground=pixel->foreground_color.pixel;
  context_values.graphics_exposures=False;
  context_mask=GCBackground | GCFont | GCForeground | GCGraphicsExposures;
  if (pixel->annotate_context != (GC) NULL)
    (void) XFreeGC(display,pixel->annotate_context);
  pixel->annotate_context=
    XCreateGC(display,windows->context.id,context_mask,&context_values);
  if (pixel->annotate_context == (GC) NULL)
    MagickFatalError(XServerFatalError,UnableToCreateGraphicContext,
      (char *) NULL);
  context_values.background=pixel->depth_color.pixel;
  if (pixel->widget_context != (GC) NULL)
    (void) XFreeGC(display,pixel->widget_context);
  pixel->widget_context=
    XCreateGC(display,windows->context.id,context_mask,&context_values);
  if (pixel->widget_context == (GC) NULL)
    MagickFatalError(XServerFatalError,UnableToCreateGraphicContext,
      (char *) NULL);
  context_values.background=pixel->foreground_color.pixel;
  context_values.foreground=pixel->background_color.pixel;
  context_values.plane_mask=
    context_values.background ^ context_values.foreground;
  if (pixel->highlight_context != (GC) NULL)
    (void) XFreeGC(display,pixel->highlight_context);
  pixel->highlight_context=XCreateGC(display,windows->context.id,
    context_mask | GCPlaneMask,&context_values);
  if (pixel->highlight_context == (GC) NULL)
    MagickFatalError(XServerFatalError,UnableToCreateGraphicContext,
      (char *) NULL);
  (void) XDestroyWindow(display,windows->context.id);
  /*
    Initialize icon window.
  */
  MagickXGetWindowInfo(display,icon_visual,icon_map,icon_pixel,(XFontStruct *) NULL,
    icon_resources,&windows->icon);
  windows->icon.geometry=resource_info->icon_geometry;
  MagickXBestIconSize(display,&windows->icon,display_image);
  windows->icon.attributes.colormap=
    XDefaultColormap(display,icon_visual->screen);
  windows->icon.attributes.event_mask=ExposureMask | StructureNotifyMask;
  class_hints->res_name=(char *) "icon";
  manager_hints->flags=InputHint | StateHint;
  manager_hints->input=False;
  manager_hints->initial_state=IconicState;
  MagickXMakeWindow(display,root_window,argv,argc,class_hints,manager_hints,
    &windows->icon);
  if (IsEventLogging())
    (void) LogMagickEvent(X11Event,GetMagickModule(),"Window id: 0x%lx (icon)",
      windows->icon.id);
  /*
    Initialize graphic context for icon window.
  */
  if (icon_pixel->annotate_context != (GC) NULL)
    (void) XFreeGC(display,icon_pixel->annotate_context);
  context_values.background=icon_pixel->background_color.pixel;
  context_values.foreground=icon_pixel->foreground_color.pixel;
  icon_pixel->annotate_context=XCreateGC(display,windows->icon.id,
    GCBackground | GCForeground,&context_values);
  if (icon_pixel->annotate_context == (GC) NULL)
    MagickFatalError(XServerFatalError,UnableToCreateGraphicContext,
      (char *) NULL);
  windows->icon.annotate_context=icon_pixel->annotate_context;
  /*
    Initialize Image window.
  */
  if (windows->image.id != (Window) NULL)
    {
      (void) CloneString(&windows->image.name,"");
      (void) CloneString(&windows->image.icon_name,"");
    }
  MagickXGetWindowInfo(display,visual_info,map_info,pixel,font_info,
    resource_info,&windows->image);
  windows->image.shape=True;  /* non-rectangular shape hint */
  windows->image.shared_memory&=resource_info->use_shared_memory;
  if ((resource_info->title != (char *) NULL) && !(*state & MontageImageState))
    {
      MagickFreeMemory(windows->image.name);
      windows->image.name=TranslateText(resource_info->image_info,
        display_image,resource_info->title);
      MagickFreeMemory(windows->image.icon_name);
      windows->image.icon_name=TranslateText(resource_info->image_info,
        display_image,resource_info->title);
    }
  else
    {
      char
        filename[MaxTextExtent];

      register Image
        *q;

      unsigned long
        count;

      /*
        Window name is the base of the filename.
      */
      (void ) CloneString(&windows->image.name,"");
      (void ) CloneString(&windows->image.icon_name,"");
      GetPathComponent(display_image->filename,TailPath,filename);
      FormatString(windows->image.name,"GraphicsMagick: %.1024s[%lu]",filename,
        display_image->scene);
      q=display_image;
      while (q->previous != (Image *) NULL)
        q=q->previous;
      for (count=1; q->next != (Image *) NULL; count++)
        q=q->next;
      FormatString(windows->image.name,"GraphicsMagick: %.1024s[%lu of %lu]",
        filename,display_image->scene,count);
      if ((display_image->previous == (Image *) NULL) &&
          (display_image->next == (Image *) NULL) &&
          (display_image->scene == 0))
        FormatString(windows->image.name,"GraphicsMagick: %.1024s",filename);
      (void) strlcpy(windows->image.icon_name,filename,MaxTextExtent);
    }
  if (resource_info->immutable)
    windows->image.immutable=True;
  windows->image.use_pixmap=resource_info->use_pixmap;
  windows->image.geometry=resource_info->image_geometry;
  FormatString(geometry,"%ux%u+0+0>!",
    XDisplayWidth(display,visual_info->screen),
    XDisplayHeight(display,visual_info->screen));
  width=display_image->columns;
  height=display_image->rows;
  x=0;
  y=0;
  (void) GetMagickGeometry(geometry,&x,&y,&width,&height);
  windows->image.width=(unsigned int) width;
  windows->image.height=(unsigned int) height;
  windows->image.attributes.event_mask=ButtonMotionMask | ButtonPressMask |
    ButtonReleaseMask | EnterWindowMask | ExposureMask | KeyPressMask |
    KeyReleaseMask | LeaveWindowMask | OwnerGrabButtonMask |
    PropertyChangeMask | StructureNotifyMask | SubstructureNotifyMask;
  MagickXGetWindowInfo(display,visual_info,map_info,pixel,font_info,
    resource_info,&windows->backdrop);
  if ((resource_info->backdrop) || (windows->backdrop.id != (Window) NULL))
    {
      /*
        Initialize backdrop window.
      */
      windows->backdrop.x=0;
      windows->backdrop.y=0;
      (void) CloneString(&windows->backdrop.name,"GraphicsMagick Backdrop");
      windows->backdrop.flags=USSize | USPosition;
      windows->backdrop.width=XDisplayWidth(display,visual_info->screen);
      windows->backdrop.height=XDisplayHeight(display,visual_info->screen);
      windows->backdrop.border_width=0;
      windows->backdrop.immutable=True;
      windows->backdrop.attributes.do_not_propagate_mask=ButtonPressMask |
        ButtonReleaseMask;
      windows->backdrop.attributes.event_mask=ButtonPressMask | KeyPressMask |
        StructureNotifyMask;
      windows->backdrop.attributes.override_redirect=True;
      class_hints->res_name=(char *) "backdrop";
      manager_hints->flags=IconWindowHint | InputHint | StateHint;
      manager_hints->icon_window=windows->icon.id;
      manager_hints->input=True;
      manager_hints->initial_state=
        resource_info->iconic ? IconicState : NormalState;
      MagickXMakeWindow(display,root_window,argv,argc,class_hints,manager_hints,
        &windows->backdrop);
      if (IsEventLogging())
        (void) LogMagickEvent(X11Event,GetMagickModule(),
          "Window id: 0x%lx (backdrop)",windows->backdrop.id);
      (void) XMapWindow(display,windows->backdrop.id);
      (void) XClearWindow(display,windows->backdrop.id);
      if (windows->image.id != (Window) NULL)
        {
          (void) XDestroyWindow(display,windows->image.id);
          windows->image.id=(Window) NULL;
        }
      /*
        Position image in the center the backdrop.
      */
      windows->image.flags|=USPosition;
      windows->image.x=(XDisplayWidth(display,visual_info->screen)/2)-
        (windows->image.width/2);
      windows->image.y=(XDisplayHeight(display,visual_info->screen)/2)-
        (windows->image.height/2);
    }
  if (resource_info->name == (char *) NULL)
    class_hints->res_name=resource_info->client_name;
  else
    class_hints->res_name=resource_info->name;
  manager_hints->flags=IconWindowHint | InputHint | StateHint;
  manager_hints->icon_window=windows->icon.id;
  manager_hints->input=True;
  manager_hints->initial_state=
    resource_info->iconic ? IconicState : NormalState;
  if (windows->group_leader.id != (Window) NULL)
    {
      /*
        Follow the leader.
      */
      manager_hints->flags|=WindowGroupHint;
      manager_hints->window_group=windows->group_leader.id;
      (void) XSelectInput(display,windows->group_leader.id,StructureNotifyMask);
      if (IsEventLogging())
        (void) LogMagickEvent(X11Event,GetMagickModule(),
          "Window id: 0x%lx (group leader)",windows->group_leader.id);
    }
  MagickXMakeWindow(display,
    (Window) (resource_info->backdrop ? windows->backdrop.id : root_window),
    argv,argc,class_hints,manager_hints,&windows->image);
  (void) XChangeProperty(display,windows->image.id,windows->im_protocols,
    XA_STRING,8,PropModeReplace,(unsigned char *) NULL,0);
  if (windows->group_leader.id != (Window) NULL)
    (void) XSetTransientForHint(display,windows->image.id,
      windows->group_leader.id);
  if (IsEventLogging())
    (void) LogMagickEvent(X11Event,GetMagickModule(),"Window id: 0x%lx (image)",
      windows->image.id);
  /*
    Initialize Info widget.
  */
  MagickXGetWindowInfo(display,visual_info,map_info,pixel,font_info,resource_info,
    &windows->info);
  (void) CloneString(&windows->info.name,"Info");
  (void) CloneString(&windows->info.icon_name,"Info");
  windows->info.border_width=1;
  windows->info.x=2;
  windows->info.y=2;
  windows->info.flags|=PPosition;
  windows->info.attributes.win_gravity=UnmapGravity;
  windows->info.attributes.event_mask=
    ButtonPressMask | ExposureMask | StructureNotifyMask;
  class_hints->res_name=(char *) "info";
  manager_hints->flags=InputHint | StateHint | WindowGroupHint;
  manager_hints->input=False;
  manager_hints->initial_state=NormalState;
  manager_hints->window_group=windows->image.id;
  MagickXMakeWindow(display,windows->image.id,argv,argc,class_hints,manager_hints,
    &windows->info);
  windows->info.highlight_stipple=XCreateBitmapFromData(display,
    windows->info.id,(char *) HighlightBitmap,HighlightWidth,HighlightHeight);
  windows->info.shadow_stipple=XCreateBitmapFromData(display,
    windows->info.id,(char *) ShadowBitmap,ShadowWidth,ShadowHeight);
  (void) XSetTransientForHint(display,windows->info.id,windows->image.id);
  if (windows->image.mapped)
    (void) XWithdrawWindow(display,windows->info.id,windows->info.screen);
  if (IsEventLogging())
    (void) LogMagickEvent(X11Event,GetMagickModule(),"Window id: 0x%lx (info)",
      windows->info.id);
  /*
    Initialize Command widget.
  */
  MagickXGetWindowInfo(display,visual_info,map_info,pixel,font_info,
    resource_info,&windows->command);
  windows->command.data=MagickMenus;
  (void) MagickXCommandWidget(display,windows,CommandMenu,(XEvent *) NULL);
  FormatString(resource_name,"%.1024s.command",resource_info->client_name);
  windows->command.geometry=MagickXGetResourceClass(resource_info->resource_database,
    resource_name,"geometry",(char *) NULL);
  (void) CloneString(&windows->command.name,MagickTitle);
  windows->command.border_width=0;
  windows->command.flags|=PPosition;
  windows->command.attributes.event_mask=ButtonMotionMask | ButtonPressMask |
    ButtonReleaseMask | EnterWindowMask | ExposureMask | LeaveWindowMask |
    OwnerGrabButtonMask | StructureNotifyMask;
  class_hints->res_name=(char *) "command";
  manager_hints->flags=InputHint | StateHint | WindowGroupHint;
  manager_hints->input=False;
  manager_hints->initial_state=NormalState;
  manager_hints->window_group=windows->image.id;
  MagickXMakeWindow(display,root_window,argv,argc,class_hints,manager_hints,
    &windows->command);
  windows->command.highlight_stipple=windows->info.highlight_stipple;
  windows->command.shadow_stipple=windows->info.shadow_stipple;
  (void) XSetTransientForHint(display,windows->command.id,windows->image.id);
  if (windows->command.mapped)
    (void) XMapRaised(display,windows->command.id);
  if (IsEventLogging())
    (void) LogMagickEvent(X11Event,GetMagickModule(),"Window id: 0x%lx (command)",
      windows->command.id);
  /*
    Initialize Widget window.
  */
  if (windows->widget.id != (Window) NULL)
    (void) CloneString(&windows->widget.name,"");
  MagickXGetWindowInfo(display,visual_info,map_info,pixel,font_info,
    resource_info,&windows->widget);
  FormatString(resource_name,"%.1024s.widget",resource_info->client_name);
  windows->widget.geometry=MagickXGetResourceClass(resource_info->resource_database,
    resource_name,"geometry",(char *) NULL);
  (void ) CloneString(&windows->widget.name,"");
  windows->widget.border_width=0;
  windows->widget.flags|=PPosition;
  windows->widget.attributes.event_mask=ButtonMotionMask | ButtonPressMask |
    ButtonReleaseMask | EnterWindowMask | ExposureMask | KeyPressMask |
    KeyReleaseMask | LeaveWindowMask | OwnerGrabButtonMask |
    StructureNotifyMask;
  class_hints->res_name=(char *) "widget";
  manager_hints->flags=InputHint | StateHint | WindowGroupHint;
  manager_hints->input=True;
  manager_hints->initial_state=NormalState;
  manager_hints->window_group=windows->image.id;
  MagickXMakeWindow(display,root_window,argv,argc,class_hints,manager_hints,
    &windows->widget);
  windows->widget.highlight_stipple=windows->info.highlight_stipple;
  windows->widget.shadow_stipple=windows->info.shadow_stipple;
  (void) XSetTransientForHint(display,windows->widget.id,windows->image.id);
  if (IsEventLogging())
    (void) LogMagickEvent(X11Event,GetMagickModule(),"Window id: 0x%lx (widget)",
      windows->widget.id);
  /*
    Initialize popup window.
  */
  if (windows->popup.id != (Window) NULL)
    (void) CloneString(&windows->popup.name,"");
  MagickXGetWindowInfo(display,visual_info,map_info,pixel,font_info,
    resource_info,&windows->popup);
  (void) CloneString(&windows->popup.name,"");
  windows->popup.border_width=0;
  windows->popup.flags|=PPosition;
  windows->popup.attributes.event_mask=ButtonMotionMask | ButtonPressMask |
    ButtonReleaseMask | EnterWindowMask | ExposureMask | KeyPressMask |
    KeyReleaseMask | LeaveWindowMask | StructureNotifyMask;
  class_hints->res_name=(char *) "popup";
  manager_hints->flags=InputHint | StateHint | WindowGroupHint;
  manager_hints->input=True;
  manager_hints->initial_state=NormalState;
  manager_hints->window_group=windows->image.id;
  MagickXMakeWindow(display,root_window,argv,argc,class_hints,manager_hints,
    &windows->popup);
  windows->popup.highlight_stipple=windows->info.highlight_stipple;
  windows->popup.shadow_stipple=windows->info.shadow_stipple;
  (void) XSetTransientForHint(display,windows->popup.id,windows->image.id);
  if (IsEventLogging())
    (void) LogMagickEvent(X11Event,GetMagickModule(),"Window id: 0x%lx (pop up)",
      windows->popup.id);
  /*
    Initialize Magnify window and cursor.
  */
  if (windows->magnify.id != (Window) NULL)
    (void) CloneString(&windows->magnify.name,"");
  MagickXGetWindowInfo(display,visual_info,map_info,pixel,font_info,
    resource_info,&windows->magnify);
  windows->magnify.shared_memory&=resource_info->use_shared_memory;
  FormatString(resource_name,"%.1024s.magnify",resource_info->client_name);
  windows->magnify.geometry=MagickXGetResourceClass(resource_info->resource_database,
    resource_name,"geometry",(char *) NULL);
  (void) CloneString(&windows->magnify.name,"");
  FormatString(windows->magnify.name,"Magnify %uX",resource_info->magnify);
  windows->magnify.cursor=MagickXMakeCursor(display,windows->image.id,
    map_info->colormap,resource_info->background_color,
    resource_info->foreground_color);
  if (windows->magnify.cursor == (Cursor) NULL)
    MagickFatalError(XServerFatalError,UnableToCreateCursor,(char *) NULL);
  windows->magnify.width=MagnifySize;
  windows->magnify.height=MagnifySize;
  windows->magnify.flags|=PPosition;
  windows->magnify.min_width=MagnifySize;
  windows->magnify.min_height=MagnifySize;
  windows->magnify.width_inc=MagnifySize;
  windows->magnify.height_inc=MagnifySize;
  windows->magnify.data=resource_info->magnify;
  windows->magnify.attributes.cursor=windows->magnify.cursor;
  windows->magnify.attributes.event_mask=ButtonPressMask | ButtonReleaseMask |
    ExposureMask | KeyPressMask | KeyReleaseMask | OwnerGrabButtonMask |
    StructureNotifyMask;
  class_hints->res_name=(char *) "magnify";
  manager_hints->flags=InputHint | StateHint | WindowGroupHint;
  manager_hints->input=True;
  manager_hints->initial_state=NormalState;
  manager_hints->window_group=windows->image.id;
  MagickXMakeWindow(display,root_window,argv,argc,class_hints,manager_hints,
    &windows->magnify);
  if (IsEventLogging())
    (void) LogMagickEvent(X11Event,GetMagickModule(),"Window id: 0x%lx (magnify)",
      windows->magnify.id);
  (void) XSetTransientForHint(display,windows->magnify.id,windows->image.id);
  /*
    Initialize panning window.
  */
  MagickXGetWindowInfo(display,visual_info,map_info,pixel,font_info,
    resource_info,&windows->pan);
  (void) CloneString(&windows->pan.name,"Pan Icon");
  windows->pan.width=windows->icon.width;
  windows->pan.height=windows->icon.height;
  FormatString(resource_name,"%.1024s.pan",resource_info->client_name);
  windows->pan.geometry=MagickXGetResourceClass(resource_info->resource_database,
    resource_name,"geometry",(char *) NULL);
  (void) XParseGeometry(windows->pan.geometry,&windows->pan.x,&windows->pan.y,
    &windows->pan.width,&windows->pan.height);
  windows->pan.flags|=PPosition;
  windows->pan.immutable=True;
  windows->pan.attributes.event_mask=ButtonMotionMask | ButtonPressMask |
    ButtonReleaseMask | ExposureMask | KeyPressMask | KeyReleaseMask |
    StructureNotifyMask;
  class_hints->res_name=(char *) "pan";
  manager_hints->flags=InputHint | StateHint | WindowGroupHint;
  manager_hints->input=False;
  manager_hints->initial_state=NormalState;
  manager_hints->window_group=windows->image.id;
  MagickXMakeWindow(display,root_window,argv,argc,class_hints,manager_hints,
    &windows->pan);
  if (IsEventLogging())
    (void) LogMagickEvent(X11Event,GetMagickModule(),"Window id: 0x%lx (pan)",
      windows->pan.id);
  (void) XSetTransientForHint(display,windows->pan.id,windows->image.id);
  if (windows->info.mapped)
    (void) XWithdrawWindow(display,windows->info.id,windows->info.screen);
  if (!windows->image.mapped || (windows->backdrop.id != (Window) NULL))
    (void) XMapWindow(display,windows->image.id);
  /*
    Set progress monitor if progress monitoring requested.
  */
  if ((resource_info->image_info->progress) &&
      (monitor_handler == (MonitorHandler) NULL))
    monitor_handler=SetMonitorHandler(MagickXMagickMonitor);
  /*
    Set warning and signal handlers.
  */
  if (warning_handler == (WarningHandler) NULL)
    {
      warning_handler=resource_info->display_warnings ?
        SetErrorHandler(MagickXWarning) : SetErrorHandler((ErrorHandler) NULL);
      warning_handler=resource_info->display_warnings ?
        SetWarningHandler(MagickXWarning) : SetWarningHandler((WarningHandler) NULL);
    }
/*   (void) signal(SIGINT,MagickXSignalHandler); */
/*   (void) signal(SIGSEGV,MagickXSignalHandler); */
/*   (void) signal(SIGTERM,MagickXSignalHandler); */
  /*
    Initialize Image and Magnify X images.
  */
  windows->image.x=0;
  windows->image.y=0;
  windows->magnify.shape=False;
  status=MagickXMakeImage(display,resource_info,&windows->image,display_image,
    (unsigned int) display_image->columns,(unsigned int) display_image->rows);
  if (status == False)
    MagickFatalError(XServerFatalError,UnableToCreateXImage,(char *) NULL);
  if (windows->image.mapped)
    MagickXRefreshWindow(display,&windows->image,(XEvent *) NULL);
  handler=SetMonitorHandler((MonitorHandler) NULL);
  status=MagickXMakeImage(display,resource_info,&windows->magnify,(Image *) NULL,
    windows->magnify.width,windows->magnify.height);
  (void) SetMonitorHandler(handler);
  if (status == False)
    MagickFatalError(XServerFatalError,UnableToCreateXImage,(char *) NULL);
  if (windows->magnify.mapped)
    {
      (void) XMapRaised(display,windows->magnify.id);
      MagickXMakeMagnifyImage(display,windows);
    }
  if (windows->image.mapped)
    if (((int) windows->image.width < windows->image.ximage->width) ||
        ((int) windows->image.height < windows->image.ximage->height))
      (void) XMapRaised(display,windows->pan.id);
  (void) XWithdrawWindow(display,windows->info.id,windows->info.screen);
  (void) XSync(display,False);
  /*
    Respond to events.
  */
  if (resource_info->delay > 1)
    display_image->delay=resource_info->delay;
  timer=time((time_t *) NULL)+(long) display_image->delay/100+1;
  update_time=0;
  if (resource_info->update)
    {
      /*
        Determine when file data was last modified.
      */
      status=stat(display_image->filename,&file_info);
      if (status == 0)
        update_time=file_info.st_mtime;
    }
  *state&=(~FormerImageState);
  *state&=(~MontageImageState);
  *state&=(~NextImageState);
  do
  {
    /*
      Handle a window event.
    */
    if (windows->image.mapped && (display_image->delay > 1))
      {
        if (timer < time((time_t *) NULL))
          {
            if (!resource_info->update)
              *state|=NextImageState | ExitState;
            else
              {
                /*
                  Determine if image file was modified.
                */
                status=stat(display_image->filename,&file_info);
                if (status == 0)
                  if (update_time != file_info.st_mtime)
                    {
                      /*
                        Redisplay image.
                      */
                      FormatString(resource_info->image_info->filename,
                        "%.1024s:%.1024s",display_image->magick,
                        display_image->filename);
                      nexus=ReadImage(resource_info->image_info,
                        &display_image->exception);
                      if (display_image->exception.severity !=
                          UndefinedException)
                        MagickError2(display_image->exception.severity,
                          display_image->exception.reason,
                          display_image->exception.description);
                      if (nexus != (Image *) NULL)
                        *state|=NextImageState | ExitState;
                    }
                timer=time((time_t *) NULL)+(long) display_image->delay/100+1;
              }
          }
        if (XEventsQueued(display,QueuedAfterFlush) == 0)
          {
            /*
              Do not block if delay > 0.
            */
            MagickXDelay(display,SuspendTime << 2);
            continue;
          }
      }
    timestamp=time((time_t *) NULL);
    (void) XNextEvent(display,&event);
    if (!windows->image.stasis)
      windows->image.stasis=(time((time_t *) NULL)-timestamp) > 0;
    if (!windows->magnify.stasis)
      windows->magnify.stasis=(time((time_t *) NULL)-timestamp) > 0;
    if (event.xany.window == windows->command.id)
      {
        /*
          Select a command from the Command widget.
        */
        id=MagickXCommandWidget(display,windows,CommandMenu,&event);
        if (id < 0)
          continue;
        (void) strlcpy(command,CommandMenu[id],MaxTextExtent);
        command_type=CommandMenus[id];
        if (id < MagickMenus)
          {
            /*
              Select a command from a pop-up menu.
            */
            entry=MagickXMenuWidget(display,windows,CommandMenu[id],Menus[id],
              command);
            if (entry < 0)
              continue;
            (void) strlcpy(command,Menus[id][entry],MaxTextExtent);
            command_type=Commands[id][entry];
          }
        if (command_type != NullCommand)
          nexus=MagickXMagickCommand(display,resource_info,windows,command_type,
            &display_image);
        continue;
      }
    switch (event.type)
    {
      case ButtonPress:
      {
        if (IsEventLogging())
          (void) LogMagickEvent(X11Event,GetMagickModule(),
            "Button Press: 0x%lx %u +%d+%d",event.xbutton.window,
            event.xbutton.button,event.xbutton.x,event.xbutton.y);
        if ((event.xbutton.button == Button3) &&
            (event.xbutton.state & Mod1Mask))
          {
            /*
              Convert Alt-Button3 to Button2.
            */
            event.xbutton.button=Button2;
            event.xbutton.state&=(~Mod1Mask);
          }
        if (event.xbutton.window == windows->backdrop.id)
          {
            (void) XSetInputFocus(display,event.xbutton.window,RevertToParent,
              event.xbutton.time);
            break;
          }
        if (event.xbutton.window == windows->image.id)
          {
            switch (event.xbutton.button)
            {
              case Button1:
              {
                if (resource_info->immutable)
                  {
                    /*
                      Select a command from the Immutable menu.
                    */
                    entry=MagickXMenuWidget(display,windows,"Commands",ImmutableMenu,
                      command);
                    if (entry >= 0)
                      nexus=MagickXMagickCommand(display,resource_info,windows,
                        ImmutableCommands[entry],&display_image);
                    break;
                  }
                /*
                  Map/unmap Command widget.
                */
                if (windows->command.mapped)
                  (void) XWithdrawWindow(display,windows->command.id,
                    windows->command.screen);
                else
                  {
                    (void) MagickXCommandWidget(display,windows,CommandMenu,
                      (XEvent *) NULL);
                    (void) XMapRaised(display,windows->command.id);
                  }
                break;
              }
              case Button2:
              {
                /*
                  User pressed the image magnify button.
                */
                (void) MagickXMagickCommand(display,resource_info,windows,ZoomCommand,
                  &display_image);
                MagickXMagnifyImage(display,windows,&event);
                break;
              }
              case Button3:
              {
                if (resource_info->immutable)
                  {
                    /*
                      Select a command from the Immutable menu.
                    */
                    entry=MagickXMenuWidget(display,windows,"Commands",ImmutableMenu,
                      command);
                    if (entry >= 0)
                      nexus=MagickXMagickCommand(display,resource_info,windows,
                        ImmutableCommands[entry],&display_image);
                    break;
                  }
                if (display_image->montage != (char *) NULL)
                  {
                    /*
                      Open or delete a tile from a visual image directory.
                    */
                    nexus=MagickXTileImage(display,resource_info,windows,
                      display_image,&event);
                    if (nexus != (Image *) NULL)
                      *state|=MontageImageState | NextImageState | ExitState;
                    vid_info.x=windows->image.x;
                    vid_info.y=windows->image.y;
                    break;
                  }
                /*
                  Select a command from the Short Cuts menu.
                */
                entry=MagickXMenuWidget(display,windows,"Short Cuts",ShortCutsMenu,
                  command);
                if (entry >= 0)
                  nexus=MagickXMagickCommand(display,resource_info,windows,
                    ShortCutsCommands[entry],&display_image);
                break;
              }
              default:
                break;
            }
            break;
          }
        if (event.xbutton.window == windows->magnify.id)
          {
            int
              factor;

            static const char
              *MagnifyMenu[]=
              {
                "2",
                "4",
                "5",
                "6",
                "7",
                "8",
                "9",
                "3",
                (char *) NULL,
              };

            static KeySym
              MagnifyCommands[]=
              {
                XK_2,
                XK_4,
                XK_5,
                XK_6,
                XK_7,
                XK_8,
                XK_9,
                XK_3
              };

            /*
              Select a magnify factor from the pop-up menu.
            */
            factor=MagickXMenuWidget(display,windows,"Magnify",MagnifyMenu,command);
            if (factor >= 0)
              MagickXMagnifyWindowCommand(display,windows,0,MagnifyCommands[factor]);
            break;
          }
        if (event.xbutton.window == windows->pan.id)
          {
            MagickXPanImage(display,windows,&event);
            break;
          }
        timer=time((time_t *) NULL)+(long) display_image->delay/100+1;
        break;
      }
      case ButtonRelease:
      {
        if (IsEventLogging())
          (void) LogMagickEvent(X11Event,GetMagickModule(),
            "Button Release: 0x%lx %u +%d+%d",event.xbutton.window,
            event.xbutton.button,event.xbutton.x,event.xbutton.y);
        break;
      }
      case ClientMessage:
      {
        if (IsEventLogging())
          (void) LogMagickEvent(X11Event,GetMagickModule(),
            "Client Message: 0x%lx 0x%lx %d 0x%lx",event.xclient.window,
            event.xclient.message_type,event.xclient.format,(unsigned long)
            event.xclient.data.l[0]);
        if (event.xclient.message_type == windows->im_protocols)
          {
            if (*event.xclient.data.l == (long) windows->im_update_widget)
              {
                (void) CloneString(&windows->command.name,MagickTitle);
                windows->command.data=MagickMenus;
                (void) MagickXCommandWidget(display,windows,CommandMenu,
                  (XEvent *) NULL);
                break;
              }
            if (*event.xclient.data.l == (long) windows->im_update_colormap)
              {
                /*
                  Update graphic context and window colormap.
                */
                for (i=0; i < (int) number_windows; i++)
                {
                  if (magick_windows[i]->id == windows->icon.id)
                    continue;
                  context_values.background=pixel->background_color.pixel;
                  context_values.foreground=pixel->foreground_color.pixel;
                  (void) XChangeGC(display,magick_windows[i]->annotate_context,
                    context_mask,&context_values);
                  (void) XChangeGC(display,magick_windows[i]->widget_context,
                    context_mask,&context_values);
                  context_values.background=pixel->foreground_color.pixel;
                  context_values.foreground=pixel->background_color.pixel;
                  context_values.plane_mask=
                    context_values.background ^ context_values.foreground;
                  (void) XChangeGC(display,magick_windows[i]->highlight_context,
                    context_mask | GCPlaneMask,&context_values);
                  magick_windows[i]->attributes.background_pixel=
                    pixel->background_color.pixel;
                  magick_windows[i]->attributes.border_pixel=
                    pixel->border_color.pixel;
                  magick_windows[i]->attributes.colormap=map_info->colormap;
                  (void) XChangeWindowAttributes(display,magick_windows[i]->id,
                    magick_windows[i]->mask,&magick_windows[i]->attributes);
                }
                if (windows->pan.mapped)
                  {
                    (void) XSetWindowBackgroundPixmap(display,windows->pan.id,
                      windows->pan.pixmap);
                    (void) XClearWindow(display,windows->pan.id);
                    MagickXDrawPanRectangle(display,windows);
                  }
                if (windows->backdrop.id != (Window) NULL)
                  (void) XInstallColormap(display,map_info->colormap);
                break;
              }
            if (*event.xclient.data.l == (long) windows->im_former_image)
              {
                *state|=FormerImageState | ExitState;
                break;
              }
            if (*event.xclient.data.l == (long) windows->im_next_image)
              {
                *state|=NextImageState | ExitState;
                break;
              }
            if (*event.xclient.data.l == (long) windows->im_retain_colors)
              {
                *state|=RetainColorsState;
                break;
              }
            if (*event.xclient.data.l == (long) windows->im_exit)
              {
                *state|=ExitState;
                break;
              }
            break;
          }
        if (event.xclient.message_type == windows->dnd_protocols)
          {
            Atom
              selection,
              type;

            int
              format;

            unsigned char
              *data;

            unsigned long
              after,
              length;

            /*
              Display image named by the Drag-and-Drop selection.
            */
            if ((*event.xclient.data.l != 2) && (*event.xclient.data.l != 128))
              break;
            selection=XInternAtom(display,"DndSelection",False);
            status=XGetWindowProperty(display,root_window,selection,0L,
              MaxTextExtent-1,False,(Atom) AnyPropertyType,&type,&format,
              &length,&after,&data);
            if ((status != Success) || (length == 0))
              break;
            if (*event.xclient.data.l == 2)
              {
                /*
                  Offix DND.
                */
                (void) strlcpy(resource_info->image_info->filename,
                  (char *) data,MaxTextExtent);
              }
            else
              {
                /*
                  XDND.
                */
                if (strncmp((char *) data, "file:", 5) != 0)
                  {
                    (void) XFree((void *) data);
                    break;
                  }
                (void) strlcpy(resource_info->image_info->filename,
                  ((char *) data)+5,MaxTextExtent);
              }
            nexus=
              ReadImage(resource_info->image_info,&display_image->exception);
            if (display_image->exception.severity != UndefinedException)
              MagickError2(display_image->exception.severity,
                display_image->exception.reason,
                display_image->exception.description);
            if (nexus != (Image *) NULL)
              *state|=NextImageState | ExitState;
            (void) XFree((void *) data);
            break;
          }
        /*
          If client window delete message, exit.
        */
        if (event.xclient.message_type != windows->wm_protocols)
          break;
        if (*event.xclient.data.l != (long) windows->wm_delete_window)
          break;
        (void) XWithdrawWindow(display,event.xclient.window,
          visual_info->screen);
        if (event.xclient.window == windows->image.id)
          {
            *state|=ExitState;
            break;
          }
        if (event.xclient.window == windows->pan.id)
          {
            /*
              Restore original image size when pan window is deleted.
            */
            windows->image.window_changes.width=windows->image.ximage->width;
            windows->image.window_changes.height=windows->image.ximage->height;
            (void) MagickXConfigureImage(display,resource_info,windows,
              display_image);
          }
        break;
      }
      case ConfigureNotify:
      {
        if (IsEventLogging())
          (void) LogMagickEvent(X11Event,GetMagickModule(),
            "Configure Notify: 0x%lx %dx%d+%d+%d %d",event.xconfigure.window,
            event.xconfigure.width,event.xconfigure.height,event.xconfigure.x,
            event.xconfigure.y,event.xconfigure.send_event);
        if (event.xconfigure.window == windows->image.id)
          {
            /*
              Image window has a new configuration.
            */
            if (event.xconfigure.send_event != 0)
              {
                XWindowChanges
                  window_changes;

                /*
                  Position the transient windows relative of the Image window.
                */
                if (windows->command.geometry == (char *) NULL)
                  if (!windows->command.mapped)
                    {
                      windows->command.x=
                        event.xconfigure.x-windows->command.width-25;
                      windows->command.y=event.xconfigure.y;
                      MagickXConstrainWindowPosition(display,&windows->command);
                      window_changes.x=windows->command.x;
                      window_changes.y=windows->command.y;
                      (void) XReconfigureWMWindow(display,windows->command.id,
                        windows->command.screen,CWX | CWY,&window_changes);
                    }
                if (windows->widget.geometry == (char *) NULL)
                  if (!windows->widget.mapped)
                    {
                      windows->widget.x=
                        event.xconfigure.x+event.xconfigure.width/10;
                      windows->widget.y=
                        event.xconfigure.y+event.xconfigure.height/10;
                      MagickXConstrainWindowPosition(display,&windows->widget);
                      window_changes.x=windows->widget.x;
                      window_changes.y=windows->widget.y;
                      (void) XReconfigureWMWindow(display,windows->widget.id,
                        windows->widget.screen,CWX | CWY,&window_changes);
                    }
                if (windows->magnify.geometry == (char *) NULL)
                  if (!windows->magnify.mapped)
                    {
                      windows->magnify.x=
                        event.xconfigure.x+event.xconfigure.width+25;
                      windows->magnify.y=event.xconfigure.y;
                      MagickXConstrainWindowPosition(display,&windows->magnify);
                      window_changes.x=windows->magnify.x;
                      window_changes.y=windows->magnify.y;
                      (void) XReconfigureWMWindow(display,windows->magnify.id,
                        windows->magnify.screen,CWX | CWY,&window_changes);
                    }
                if (windows->pan.geometry == (char *) NULL)
                  if (!windows->pan.mapped)
                    {
                      windows->pan.x=
                        event.xconfigure.x+event.xconfigure.width+25;
                      windows->pan.y=
                        event.xconfigure.y+windows->magnify.height+50;
                      MagickXConstrainWindowPosition(display,&windows->pan);
                      window_changes.x=windows->pan.x;
                      window_changes.y=windows->pan.y;
                      (void) XReconfigureWMWindow(display,windows->pan.id,
                        windows->pan.screen,CWX | CWY,&window_changes);
                    }
              }
            if ((event.xconfigure.width == (long) windows->image.width) &&
                (event.xconfigure.height == (long) windows->image.height))
              break;
            windows->image.width=event.xconfigure.width;
            windows->image.height=event.xconfigure.height;
            windows->image.x=0;
            windows->image.y=0;
            if (display_image->montage != (char *) NULL)
              {
                windows->image.x=vid_info.x;
                windows->image.y=vid_info.y;
              }
            if (windows->image.mapped && windows->image.stasis)
              {
                /*
                  Update Image window configuration.
                */
                windows->image.window_changes.width=event.xconfigure.width;
                windows->image.window_changes.height=event.xconfigure.height;
                (void) MagickXConfigureImage(display,resource_info,windows,
                  display_image);
              }
            if ((event.xconfigure.width < windows->image.ximage->width) ||
                (event.xconfigure.height < windows->image.ximage->height))
              {
                (void) XMapRaised(display,windows->pan.id);
                MagickXDrawPanRectangle(display,windows);
              }
            else
              if (windows->pan.mapped)
                (void) XWithdrawWindow(display,windows->pan.id,
                  windows->pan.screen);
            break;
          }
        if (event.xconfigure.window == windows->magnify.id)
          {
            unsigned int
              magnify;

            /*
              Magnify window has a new configuration.
            */
            windows->magnify.width=event.xconfigure.width;
            windows->magnify.height=event.xconfigure.height;
            if (!windows->magnify.mapped)
              break;
            magnify=1;
            while ((int) magnify <= event.xconfigure.width)
              magnify<<=1;
            while ((int) magnify <= event.xconfigure.height)
              magnify<<=1;
            magnify>>=1;
            if (((int) magnify != event.xconfigure.width) ||
                ((int) magnify != event.xconfigure.height))
              {
                window_changes.width=magnify;
                window_changes.height=magnify;
                (void) XReconfigureWMWindow(display,windows->magnify.id,
                  windows->magnify.screen,CWWidth | CWHeight,&window_changes);
                break;
              }
            if (windows->magnify.mapped && windows->magnify.stasis)
              {
                handler=SetMonitorHandler((MonitorHandler) NULL);
                status=MagickXMakeImage(display,resource_info,&windows->magnify,
                  display_image,windows->magnify.width,windows->magnify.height);
                MagickXMakeMagnifyImage(display,windows);
                (void) SetMonitorHandler(handler);
              }
            break;
          }
        if (event.xconfigure.window == windows->pan.id)
          {
            /*
              Pan icon window has a new configuration.
            */
            if (event.xconfigure.send_event != 0)
              {
                windows->pan.x=event.xconfigure.x;
                windows->pan.y=event.xconfigure.y;
              }
            windows->pan.width=event.xconfigure.width;
            windows->pan.height=event.xconfigure.height;
            break;
          }
        if (event.xconfigure.window == windows->icon.id)
          {
            /*
              Icon window has a new configuration.
            */
            windows->icon.width=event.xconfigure.width;
            windows->icon.height=event.xconfigure.height;
            break;
          }
        break;
      }
      case DestroyNotify:
      {
        /*
          Group leader has exited.
        */
        if (IsEventLogging())
          (void) LogMagickEvent(X11Event,GetMagickModule(),"Destroy Notify: 0x%lx",
            event.xdestroywindow.window);
        if (event.xdestroywindow.window == windows->group_leader.id)
          {
            *state|=ExitState;
            break;
          }
        break;
      }
      case EnterNotify:
      {
        /*
          Selectively install colormap.
        */
        if (map_info->colormap != XDefaultColormap(display,visual_info->screen))
          {
            if (event.xcrossing.mode != NotifyUngrab)
              {
                XInductColormap(display,map_info->colormap);
              }
          }
        break;
      }
      case Expose:
      {
        if (IsEventLogging())
          (void) LogMagickEvent(X11Event,GetMagickModule(),
            "Expose: 0x%lx %dx%d+%d+%d",event.xexpose.window,
            event.xexpose.width,event.xexpose.height,event.xexpose.x,
            event.xexpose.y);
        /*
          Refresh windows that are now exposed.
        */
        if (event.xexpose.window == windows->image.id)
          if (windows->image.mapped)
            {
              MagickXRefreshWindow(display,&windows->image,&event);
              timer=time((time_t *) NULL)+(long) display_image->delay/100+1;
              break;
            }
        if (event.xexpose.window == windows->magnify.id)
          if (event.xexpose.count == 0)
            if (windows->magnify.mapped)
              {
                MagickXMakeMagnifyImage(display,windows);
                break;
              }
        if (event.xexpose.window == windows->pan.id)
          if (event.xexpose.count == 0)
            {
              MagickXDrawPanRectangle(display,windows);
              break;
            }
        if (event.xexpose.window == windows->icon.id)
          if (event.xexpose.count == 0)
            {
              MagickXRefreshWindow(display,&windows->icon,&event);
              break;
            }
        break;
      }
      case KeyPress:
      {
        int
          length;

        /*
          Respond to a user key press.
        */
        length=XLookupString((XKeyEvent *) &event.xkey,command,sizeof(command),
          &key_symbol,(XComposeStatus *) NULL);
        *(command+length)='\0';
        if (IsEventLogging())
          (void) LogMagickEvent(X11Event,GetMagickModule(),
            "Key press: %d 0x%lx (%.1024s)",event.xkey.state,key_symbol,
            command);
        if (event.xkey.window == windows->image.id)
          {
            command_type=MagickXImageWindowCommand(display,resource_info,windows,
              event.xkey.state,key_symbol,&display_image);
            if (command_type != NullCommand)
              nexus=MagickXMagickCommand(display,resource_info,windows,command_type,
                &display_image);
          }
        if (event.xkey.window == windows->magnify.id)
          MagickXMagnifyWindowCommand(display,windows,event.xkey.state,key_symbol);
        if (event.xkey.window == windows->pan.id)
          {
            if (key_symbol == XK_q)
              (void) XWithdrawWindow(display,windows->pan.id,
                windows->pan.screen);
            else
              if ((key_symbol == XK_F1) || (key_symbol == XK_Help))
                MagickXTextViewWidget(display,resource_info,windows,False,
                  "Help Viewer - Image Pan",ImagePanHelp);
              else
                MagickXTranslateImage(display,windows,*image,key_symbol);
          }
        timer=time((time_t *) NULL)+(long) display_image->delay/100+1;
        break;
      }
      case KeyRelease:
      {
        /*
          Respond to a user key release.
        */
        (void) XLookupString((XKeyEvent *) &event.xkey,command,sizeof(command),
          &key_symbol,(XComposeStatus *) NULL);
        if (IsEventLogging())
          (void) LogMagickEvent(X11Event,GetMagickModule(),
            "Key release: 0x%lx (%c)",key_symbol,*command);
        break;
      }
      case LeaveNotify:
      {
        /*
          Selectively uninstall colormap.
        */
        if (map_info->colormap != XDefaultColormap(display,visual_info->screen))
          {
            if (event.xcrossing.mode != NotifyUngrab)
              {
                XUninductColormap(display,map_info->colormap);
              }
          }
        break;
      }
      case MapNotify:
      {
        if (IsEventLogging())
          (void) LogMagickEvent(X11Event,GetMagickModule(),"Map Notify: 0x%lx",
            event.xmap.window);
        if (event.xmap.window == windows->backdrop.id)
          {
            (void) XSetInputFocus(display,event.xmap.window,RevertToParent,
              CurrentTime);
            windows->backdrop.mapped=True;
            break;
          }
        if (event.xmap.window == windows->image.id)
          {
            if (windows->backdrop.id != (Window) NULL)
              (void) XInstallColormap(display,map_info->colormap);
            if (LocaleCompare(display_image->magick,"LOGO") == 0)
              {
                if (LocaleCompare(display_image->filename,"Untitled") == 0)
                  nexus=MagickXOpenImage(display,resource_info,windows,False);
                else
                  *state|=NextImageState | ExitState;
              }
            if (((int) windows->image.width < windows->image.ximage->width) ||
                ((int) windows->image.height < windows->image.ximage->height))
              (void) XMapRaised(display,windows->pan.id);
            windows->image.mapped=True;
            break;
          }
        if (event.xmap.window == windows->magnify.id)
          {
            MagickXMakeMagnifyImage(display,windows);
            windows->magnify.mapped=True;
            (void) XWithdrawWindow(display,windows->info.id,
              windows->info.screen);
            break;
          }
        if (event.xmap.window == windows->pan.id)
          {
            MagickXMakePanImage(display,resource_info,windows,display_image);
            windows->pan.mapped=True;
            break;
          }
        if (event.xmap.window == windows->info.id)
          {
            windows->info.mapped=True;
            break;
          }
        if (event.xmap.window == windows->icon.id)
          {
            unsigned int
              taint;

            /*
              Create an icon image.
            */
            taint=display_image->taint;
            MagickXMakeStandardColormap(display,icon_visual,icon_resources,
              display_image,icon_map,icon_pixel);
            (void) MagickXMakeImage(display,icon_resources,&windows->icon,
              display_image,windows->icon.width,windows->icon.height);
            display_image->taint=taint;
            (void) XSetWindowBackgroundPixmap(display,windows->icon.id,
              windows->icon.pixmap);
            (void) XClearWindow(display,windows->icon.id);
            (void) XWithdrawWindow(display,windows->info.id,
              windows->info.screen);
            windows->icon.mapped=True;
            break;
          }
        if (event.xmap.window == windows->command.id)
          {
            windows->command.mapped=True;
            break;
          }
        if (event.xmap.window == windows->popup.id)
          {
            windows->popup.mapped=True;
            break;
          }
        if (event.xmap.window == windows->widget.id)
          {
            windows->widget.mapped=True;
            break;
          }
        break;
      }
      case MappingNotify:
      {
        (void) XRefreshKeyboardMapping(&event.xmapping);
        break;
      }
      case NoExpose:
        break;
      case PropertyNotify:
      {
        Atom
          type;

        int
          format;

        unsigned char
          *data;

        unsigned long
          after,
          length;

        if (IsEventLogging())
          (void) LogMagickEvent(X11Event,GetMagickModule(),
            "Property Notify: 0x%lx 0x%lx %d",event.xproperty.window,
            event.xproperty.atom,event.xproperty.state);
        if (event.xproperty.atom != windows->im_remote_command)
          break;
        /*
          Display image named by the remote command protocol.
        */
        status=XGetWindowProperty(display,event.xproperty.window,
          event.xproperty.atom,0L,MaxTextExtent-1,False,(Atom) AnyPropertyType,
          &type,&format,&length,&after,&data);
        if ((status != Success) || (length == 0))
          break;
        (void) strlcpy(resource_info->image_info->filename,(char *) data,
          MaxTextExtent);
        nexus=ReadImage(resource_info->image_info,&display_image->exception);
        if (display_image->exception.severity != UndefinedException)
          MagickError2(display_image->exception.severity,
            display_image->exception.reason,
            display_image->exception.description);
        if (nexus != (Image *) NULL)
          *state|=NextImageState | ExitState;
        (void) XFree((void *) data);
        break;
      }
      case ReparentNotify:
      {
        if (IsEventLogging())
          (void) LogMagickEvent(X11Event,GetMagickModule(),
            "Reparent Notify: 0x%lx=>0x%lx",event.xreparent.parent,
            event.xreparent.window);
        break;
      }
      case UnmapNotify:
      {
        if (IsEventLogging())
          (void) LogMagickEvent(X11Event,GetMagickModule(),"Unmap Notify: 0x%lx",
            event.xunmap.window);
        if (event.xunmap.window == windows->backdrop.id)
          {
            windows->backdrop.mapped=False;
            break;
          }
        if (event.xunmap.window == windows->image.id)
          {
            windows->image.mapped=False;
            break;
          }
        if (event.xunmap.window == windows->magnify.id)
          {
            windows->magnify.mapped=False;
            break;
          }
        if (event.xunmap.window == windows->pan.id)
          {
            windows->pan.mapped=False;
            break;
          }
        if (event.xunmap.window == windows->info.id)
          {
            windows->info.mapped=False;
            break;
          }
        if (event.xunmap.window == windows->icon.id)
          {
            if (map_info->colormap == icon_map->colormap)
              MagickXConfigureImageColormap(display,resource_info,windows,
                display_image);
            (void) MagickXFreeStandardColormap(display,icon_visual,icon_map,
              icon_pixel);
            windows->icon.mapped=False;
            break;
          }
        if (event.xunmap.window == windows->command.id)
          {
            windows->command.mapped=False;
            break;
          }
        if (event.xunmap.window == windows->popup.id)
          {
            if (windows->backdrop.id != (Window) NULL)
              (void) XSetInputFocus(display,windows->image.id,RevertToParent,
                CurrentTime);
            windows->popup.mapped=False;
            break;
          }
        if (event.xunmap.window == windows->widget.id)
          {
            if (windows->backdrop.id != (Window) NULL)
              (void) XSetInputFocus(display,windows->image.id,RevertToParent,
                CurrentTime);
            windows->widget.mapped=False;
            break;
          }
        break;
      }
      default:
      {
        if (IsEventLogging())
          (void) LogMagickEvent(X11Event,GetMagickModule(),"Event type: %d",
            event.type);
        break;
      }
    }
  }
  while (!(*state & ExitState));
  if (!(*state & ExitState))
    (void) MagickXMagickCommand(display,resource_info,windows,FreeBuffersCommand,
      &display_image);
  else
    {
      /*
        Query user if image has changed.
      */
      if (!resource_info->immutable && display_image->taint)
        {
          status=MagickXConfirmWidget(display,windows,"Your image changed.",
            "Do you want to save it");
          if (status == 0)
            *state&=(~ExitState);
          else
            if (status > 0)
              (void) MagickXMagickCommand(display,resource_info,windows,SaveCommand,
                &display_image);
        }
    }
  if ((windows->visual_info->class == GrayScale) ||
      (windows->visual_info->class == PseudoColor) ||
      (windows->visual_info->class == DirectColor))
    {
      /*
        Withdraw pan and Magnify window.
      */
      if (windows->info.mapped)
        (void) XWithdrawWindow(display,windows->info.id,windows->info.screen);
      if (windows->magnify.mapped)
        (void) XWithdrawWindow(display,windows->magnify.id,
          windows->magnify.screen);
      if (windows->command.mapped)
        (void) XWithdrawWindow(display,windows->command.id,
          windows->command.screen);
    }
  if (windows->pan.mapped)
    (void) XWithdrawWindow(display,windows->pan.id,windows->pan.screen);
  if (!resource_info->backdrop)
    if (windows->backdrop.mapped)
      {
        (void) XWithdrawWindow(display,windows->backdrop.id,
          windows->backdrop.screen);
        (void) XDestroyWindow(display,windows->backdrop.id);
        windows->backdrop.id=(Window) NULL;
        (void) XWithdrawWindow(display,windows->image.id,windows->image.screen);
        (void) XDestroyWindow(display,windows->image.id);
        windows->image.id=(Window) NULL;
      }
  /*
    Enable application cursor if progress indication enabled.
  */
  if (resource_info->image_info->progress)
    MagickXSetCursorState(display,windows,True);
  MagickXCheckRefreshWindows(display,windows);
  if (!resource_info->immutable || (display_image->next != (Image *) NULL))
    if ((*state & FormerImageState) || (*state & NextImageState))
      *state&=(~ExitState);
  if (*state & ExitState)
    {
      /*
        Free Standard Colormap.
      */
      (void) MagickXFreeStandardColormap(display,icon_visual,icon_map,icon_pixel);
      if (resource_info->map_type == (char *) NULL)
        (void) MagickXFreeStandardColormap(display,visual_info,map_info,pixel);
      /*
        Free X resources.
      */
      if (resource_info->backdrop)
        (void) XFreeCursor(display,windows->backdrop.cursor);
      if (resource_info->copy_image != (Image *) NULL)
        {
          DestroyImage(resource_info->copy_image);
          resource_info->copy_image=(Image *) NULL;
        }
       MagickXDestroyXWindows(windows);
    }

  DestroyWindowsImage(windows,icon);
  DestroyWindowsImage(windows,pan);
  DestroyWindowsImage(windows,magnify);

  (void) XSync(display,False);
  /*
    Restore our progress monitor and warning handlers.
  */
  (void) SetMonitorHandler(monitor_handler);
  (void) SetErrorHandler(warning_handler);
  (void) SetWarningHandler(warning_handler);
  /*
    Change to home directory.
  */
  (void) getcwd(working_directory,MaxTextExtent-1);
  (void) chdir(resource_info->home_directory);
  *image=display_image;
  return(nexus);
}
#endif
