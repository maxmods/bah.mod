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
%              AAA   N   N  IIIII  M   M   AAA   TTTTT  EEEEE                 %
%             A   A  NN  N    I    MM MM  A   A    T    E                     %
%             AAAAA  N N N    I    M M M  AAAAA    T    EEE                   %
%             A   A  N  NN    I    M   M  A   A    T    E                     %
%             A   A  N   N  IIIII  M   M  A   A    T    EEEEE                 %
%                                                                             %
%                                                                             %
%            Methods to Interactively Animate an Image Sequence               %
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
#include "magick/color.h"
#include "magick/constitute.h"
#include "magick/delegate.h"
#include "magick/log.h"
#include "magick/monitor.h"
#include "magick/transform.h"
#include "magick/utility.h"
#include "magick/version.h"
#include "magick/xwindow.h"
#if defined(HasX11)
#include "magick/animate.h"

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
%  Method MagickXMagickCommand makes a transform to the image or Image window as
%  specified by a user menu button or keyboard command.
%
%  The format of the MagickXMagickCommand method is:
%
%      Image *MagickXMagickCommand(Display *display,MagickXResourceInfo *resource_info,
%        MagickXWindows *windows,const CommandType command_type,Image **image,
%        unsigned int *state)
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
%    o image: Specifies a pointer to an Image structure;  MagickXMagickCommand
%      may transform the image and return a new image pointer.
%
%    o state: Specifies an unsigned int;  MagickXMagickCommand may return a
%      modified state.
%
%
*/
static Image *MagickXMagickCommand(Display *display,MagickXResourceInfo *resource_info,
  MagickXWindows *windows,const CommandType command_type,Image **image,
  unsigned int *state)
{
  Image
    *nexus;

  int
    status;

  XTextProperty
    window_name;

  /*
    Process user command.
  */
  nexus=(Image *) NULL;
  switch (command_type)
  {
    case OpenCommand:
    {
      char
        **filelist;

      ExceptionInfo
        exception;

      Image
        *image,
        *next;

      ImageInfo
        *clone_info;

      int
        number_files;

      MonitorHandler
        handler;

      register int
        i;

      static char
        filenames[MaxTextExtent] = "*";

      unsigned int
        status;

      if (resource_info->immutable)
        break;
      /*
        Request file name from user.
      */
      MagickXFileBrowserWidget(display,windows,"Animate",filenames);
      if (*filenames == '\0')
        return((Image *) NULL);
      /*
        Expand the filenames.
      */
      filelist=MagickAllocateMemory(char **,sizeof(char *));
      if (filelist == (char **) NULL)
        {
          MagickError(ResourceLimitError,MemoryAllocationFailed,filenames);
          return((Image *) NULL);
        }
      number_files=1;
      filelist[0]=filenames;
      status=ExpandFilenames(&number_files,&filelist);
      if ((status == False) || (number_files == 0))
        {
          if (number_files == 0)
            MagickError(ImageError,NoImagesWereLoaded,filenames);
          else
            MagickError(ResourceLimitError,MemoryAllocationFailed,filenames);
          return((Image *) NULL);
        }
      clone_info=CloneImageInfo(resource_info->image_info);
      if (clone_info == (ImageInfo *) NULL)
        break;
      GetExceptionInfo(&exception);
      image=(Image *) NULL;
      handler=(MonitorHandler) NULL;
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      for (i=0; i < number_files; i++)
      {
        if (number_files > 5)
          handler=SetMonitorHandler((MonitorHandler) NULL);
        (void) strlcpy(clone_info->filename,filelist[i],MaxTextExtent);
        *clone_info->magick='\0';
        next=ReadImage(clone_info,&exception);
        if (exception.severity != UndefinedException)
          CatchException(&exception);
        if (next != (Image *) NULL)
          {
            if (image == (Image *) NULL)
              image=next;
            else
              {
                image->next=next;
                image->next->previous=image;
                image=image->next;
              }
          }
        if (number_files <= 5)
          continue;
        (void) SetMonitorHandler(handler);
        if (!MagickMonitorFormatted(i,number_files,&image->exception,
                                    LoadImagesText,image->filename))
          break;
      }
      DestroyExceptionInfo(&exception);
      DestroyImageInfo(clone_info);
      if (image == (Image *) NULL)
        {
          MagickXSetCursorState(display,windows,False);
          MagickError(ImageError,NoImagesWereLoaded,filenames);
          return((Image *) NULL);
        }
      while (image->previous != (Image *) NULL)
        image=image->previous;
      nexus=image;
      *state|=ExitState;
      break;
    }
    case PlayCommand:
    {
      char
        basename[MaxTextExtent];

      /*
        Window name is the base of the filename.
      */
      *state|=PlayAnimationState;
      *state&=(~AutoReverseAnimationState);
      GetPathComponent((*image)->filename,BasePath,basename);
      FormatString(windows->image.name,"GraphicsMagick: %.1024s",basename);
      if (resource_info->title != (char *) NULL)
        windows->image.name=TranslateText(resource_info->image_info,*image,
          resource_info->title);
      status=XStringListToTextProperty(&windows->image.name,1,&window_name);
      if (status == 0)
        break;
      XSetWMName(display,windows->image.id,&window_name);
      (void) XFree((void *) window_name.value);
      break;
    }
    case StepCommand:
    case StepBackwardCommand:
    case StepForwardCommand:
    {
      *state|=StepAnimationState;
      *state&=(~PlayAnimationState);
      if (command_type == StepBackwardCommand)
        *state&=(~ForwardAnimationState);
      if (command_type == StepForwardCommand)
        *state|=ForwardAnimationState;
      if (resource_info->title != (char *) NULL)
        break;
      break;
    }
    case RepeatCommand:
    {
      *state|=RepeatAnimationState;
      *state&=(~AutoReverseAnimationState);
      *state|=PlayAnimationState;
      break;
    }
    case AutoReverseCommand:
    {
      *state|=AutoReverseAnimationState;
      *state&=(~RepeatAnimationState);
      *state|=PlayAnimationState;
      break;
    }
    case SlowerCommand:
    {
      resource_info->delay++;
      break;
    }
    case FasterCommand:
    {
      if (resource_info->delay == 0)
        break;
      resource_info->delay--;
      break;
    }
    case ForwardCommand:
    {
      *state=ForwardAnimationState;
      *state&=(~AutoReverseAnimationState);
      break;
    }
    case ReverseCommand:
    {
      *state&=(~ForwardAnimationState);
      *state&=(~AutoReverseAnimationState);
      break;
    }
    case InfoCommand:
    {
      MagickXDisplayImageInfo(display,resource_info,windows,(Image *) NULL,*image);
      break;
    }
    case HelpCommand:
    {
      /*
        User requested help.
      */
      MagickXTextViewWidget(display,resource_info,windows,False,
        "Help Viewer - Animate",AnimateHelp);
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
          (void) XChangeProperty(display,mozilla_window,mozilla_atom,
            XA_STRING,8,PropModeReplace,(unsigned char *) command,
            (int) strlen(command));
          MagickXSetCursorState(display,windows,False);
          break;
        }
      MagickXSetCursorState(display,windows,True);
      MagickXCheckRefreshWindows(display,windows);
      status=InvokeDelegate(resource_info->image_info,*image,"browse",
        (char *) NULL,&(*image)->exception);
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
    case QuitCommand:
    {
      /*
        Exit program
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
    default:
      break;
  }
  return(nexus);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k X A n i m a t e B a c k g r o u n d I m a g e                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickXAnimateBackgroundImage() animates an image sequence in the background of
%  a window.
%
%  The format of the MagickXAnimateBackgroundImage method is:
%
%      void MagickXAnimateBackgroundImage(Display *display,
%        MagickXResourceInfo *resource_info,Image *images)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 MagickXResourceInfo structure.
%
%    o images: The image list.
%
%
*/

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

static int SceneCompare(const void *x,const void *y)
{
  Image
    **image_1,
    **image_2;

  image_1=(Image **) x;
  image_2=(Image **) y;
  return((int) ((*image_1)->scene-(*image_2)->scene));
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

MagickExport void MagickXAnimateBackgroundImage(Display *display,
  MagickXResourceInfo *resource_info,Image *images)
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

  Image
    *display_image,
    **image_list;

  int
    scene;

  register long
    i;

  size_t
    number_scenes;

  unsigned int
    coalesce,
    status;

  unsigned int
    height,
    width;

  Window
    root_window;

  XEvent
    event;

  XGCValues
    context_values;

  MagickXPixelInfo
    scene_info;

  MagickXResourceInfo
    resources;

  XWindowAttributes
    window_attributes;

  /*
    Determine target window.
  */
  assert(images != (Image *) NULL);
  assert(images->signature == MagickSignature);
  resources=(*resource_info);
  window_info.id=(Window) NULL;
  root_window=XRootWindow(display,XDefaultScreen(display));
  if (LocaleCompare(resources.window_id,"root") == 0)
    window_info.id=root_window;
  else
    {
      if (isdigit((int) (*resources.window_id)))
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
      return;
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
        MagickFatalError(ResourceLimitFatalError,MemoryAllocationFailed,
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
    MagickXDestroyWindowColors(display,root_window);
  coalesce=False;
  if (images->next != (Image *) NULL)
    {
      Image
        *next;

      /*
        Determine if the sequence of images have identical page info.
      */
      for (next=images; next != (Image *) NULL; )
      {
        if ((images->page.width != 0) && (images->page.height != 0))
          if ((images->columns != next->columns) ||
              (images->rows != next->rows))
            break;
        if ((images->page.x != next->page.x) ||
            (images->page.y != next->page.y))
          break;
        next=next->next;
      }
      coalesce=next != (Image *) NULL;
      if (coalesce)
        {
          Image
            *coalesce_image;

          coalesce_image=CoalesceImages(images,&images->exception);
          if (coalesce_image == (Image *) NULL)
            MagickFatalError2(images->exception.severity,
              images->exception.reason,images->exception.description);
          images=coalesce_image;
        }
    }
  if (resources.map_type == (char *) NULL)
    if ((visual_info->class != TrueColor) &&
        (visual_info->class != DirectColor))
      {
        Image
          *next;

        /*
          Determine if the sequence of images has the identical colormap.
        */
        for (next=images; next != (Image *) NULL; )
        {
          next->matte=False;
          if ((next->storage_class == DirectClass) ||
              (next->colors != images->colors) ||
              (next->colors > (unsigned long) visual_info->colormap_size))
            break;
          for (i=0; i < (long) images->colors; i++)
            if (NotColorMatch(next->colormap+i,images->colormap+i))
              break;
          if (i < (long) images->colors)
            break;
          next=next->next;
        }
        if (next != (Image *) NULL)
          (void) MapImages(images,(Image *) NULL,
            resources.quantize_info->dither);
      }
  /*
    Sort images by increasing scene number.
  */
  number_scenes=GetImageListLength(images);
  image_list=ImageListToArray(images,&images->exception);
  if (image_list == (Image **) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToAnimateImageSequence);
  for (i=0; i < (long) number_scenes; i++)
    if (image_list[i]->scene == 0)
      break;
  if (i == (long) number_scenes)
    qsort((void *) image_list,number_scenes,sizeof(Image *),SceneCompare);
  /*
    Initialize Standard Colormap.
  */
  resources.colormap=SharedColormap;
  display_image=image_list[0];
  for (scene=0; scene < (int) number_scenes; scene++)
  {
    if ((resource_info->map_type != (char *) NULL) ||
        (visual_info->class == TrueColor) ||
        (visual_info->class == DirectColor))
      (void) SetImageType(image_list[scene],TrueColorType);
    if ((display_image->columns < image_list[scene]->columns) &&
        (display_image->rows < image_list[scene]->rows))
      display_image=image_list[scene];
  }
  if ((resource_info->map_type != (char *) NULL) ||
      (visual_info->class == TrueColor) ||
      (visual_info->class == DirectColor))
    (void) SetImageType(display_image,TrueColorType);
  MagickXMakeStandardColormap(display,visual_info,&resources,display_image,map_info,
    &pixel);
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
  window_info.width=(unsigned int) image_list[0]->columns;
  window_info.height=(unsigned int) image_list[0]->rows;
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
  window_info.width=(unsigned int) height;
  window_info.height=(unsigned int) height;
  window_info.x=(int) x;
  window_info.y=(int) y;
  status=MagickXMakeImage(display,&resources,&window_info,image_list[0],
    window_info.width,window_info.height);
  if (status == False)
    MagickFatalError(XServerFatalError,UnableToCreateXImage,(char *) NULL);
  window_info.x=0;
  window_info.y=0;
  if (IsEventLogging())
    {
      (void) LogMagickEvent(X11Event,GetMagickModule(),
        "Image: %.1024s[%lu] %lux%lu ",image_list[0]->filename,
        image_list[0]->scene,image_list[0]->columns,image_list[0]->rows);
      if (image_list[0]->colors != 0)
        (void) LogMagickEvent(X11Event,GetMagickModule(),"%uc ",
          image_list[0]->colors);
      (void) LogMagickEvent(X11Event,GetMagickModule(),"%.1024s",
        image_list[0]->magick);
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
      window_info.x=(window_attributes.width/2)-(window_info.ximage->width/2);
      window_info.y=(window_attributes.height/2)-(window_info.ximage->height/2);
      width=window_attributes.width;
      height=window_attributes.height;
    }
  if (resources.image_geometry != (char *) NULL)
    {
      char
        default_geometry[MaxTextExtent];

      int
        gravity;

      XSizeHints
        *size_hints;

      unsigned int
        flags;

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
    window_info.ximage,0,0,window_info.x,window_info.y,window_info.width,
    window_info.height);
  (void) XSetWindowBackgroundPixmap(display,window_info.id,window_info.pixmap);
  (void) XClearWindow(display,window_info.id);
  /*
    Initialize image pixmaps structure.
  */
  window_info.pixmaps=MagickAllocateMemory(Pixmap *,number_scenes*sizeof(Pixmap));
  window_info.matte_pixmaps=MagickAllocateMemory(Pixmap *,
    number_scenes*sizeof(Pixmap));
  if ((window_info.pixmaps == (Pixmap *) NULL) ||
      (window_info.matte_pixmaps == (Pixmap *) NULL))
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToAnimateImageSequence);
  window_info.pixmaps[0]=window_info.pixmap;
  window_info.matte_pixmaps[0]=window_info.pixmap;
  scene_info.pixels=(unsigned long *) NULL;
  for (scene=1; scene < (int) number_scenes; scene++)
  {
    /*
      Create X image.
    */
    window_info.pixmap=(Pixmap) NULL;
    window_info.matte_pixmap=(Pixmap) NULL;
    if ((resources.map_type != (char *) NULL) ||
        (visual_info->class == TrueColor) ||
        (visual_info->class == DirectColor))
      if (image_list[scene]->storage_class == PseudoClass)
        {
          /*
            Get pixel info for this scene.
          */
          MagickXGetPixelPacket(display,visual_info,map_info,&resources,
            image_list[scene],&scene_info);
          window_info.pixel_info=(&scene_info);
        }
    status=MagickXMakeImage(display,&resources,&window_info,image_list[scene],
      (unsigned int) image_list[scene]->columns,
      (unsigned int) image_list[scene]->rows);
    if (status == False)
      MagickFatalError(XServerFatalError,UnableToCreateXImage,(char *) NULL);
    if (IsEventLogging())
      {
        (void) LogMagickEvent(X11Event,GetMagickModule(),
          "Image: [%lu] %.1024s %lux%lu ",image_list[scene]->scene,
          image_list[scene]->filename,image_list[scene]->columns,
          image_list[scene]->rows);
        if (image_list[scene]->colors != 0)
          (void) LogMagickEvent(X11Event,GetMagickModule(),"%uc ",
            image_list[scene]->colors);
        (void) LogMagickEvent(X11Event,GetMagickModule(),"%.1024s",
          image_list[scene]->magick);
      }
    /*
      Create the X pixmap.
    */
    window_info.pixmap=XCreatePixmap(display,window_info.id,
      (unsigned int) width,(unsigned int) height,window_info.depth);
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
      window_info.ximage,0,0,window_info.x,window_info.y,window_info.width,
      window_info.height);
    (void) XSetWindowBackgroundPixmap(display,window_info.id,
      window_info.pixmap);
    (void) XClearWindow(display,window_info.id);
    window_info.pixmaps[scene]=window_info.pixmap;
    window_info.matte_pixmaps[scene]=window_info.matte_pixmap;
    if (image_list[scene]->matte)
      (void) XClearWindow(display,window_info.id);
    MagickXDelay(display,(unsigned long) resources.delay*10*Max(images->delay,1));
  }
  window_info.pixel_info=(&pixel);
  /*
    Display pixmap on the window.
  */
  (void) XSelectInput(display,window_info.id,SubstructureNotifyMask);
  event.type=Expose;
  do
  {
    for (scene=0; scene < (int) number_scenes; scene++)
    {
      if (XEventsQueued(display,QueuedAfterFlush) > 0)
        {
          (void) XNextEvent(display,&event);
          if (event.type == DestroyNotify)
            break;
        }
      window_info.pixmap=window_info.pixmaps[scene];
      window_info.matte_pixmap=window_info.matte_pixmaps[scene];
      (void) XSetWindowBackgroundPixmap(display,window_info.id,
        window_info.pixmap);
      (void) XClearWindow(display,window_info.id);
      (void) XSync(display,False);
      MagickXDelay(display,(unsigned long) resources.delay*10*
        Max(image_list[scene]->delay,1));
    }
  } while (event.type != DestroyNotify);
  (void) XSync(display,False);
  MagickFreeMemory(image_list);
  if (coalesce)
    DestroyImageList(images);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k X A n i m a t e I m a g e s                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickXAnimateImages() displays an image via X11.
%
%  The format of the MagickXAnimateImages method is:
%
%      Image *MagickXAnimateImages(Display *display,MagickXResourceInfo *resource_info,
%        char **argv,const int argc,Image *images)
%
%  A description of each parameter follows:
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
%    o images: The image list.
%
%
*/
MagickExport Image *
MagickXAnimateImages(Display *display,
		     MagickXResourceInfo *resource_info,
		     char *argv[],
		     const int argc,
		     Image *images)
{
#define MagickMenus  4
#define MaxWindows  8
#define MagickTitle  "Commands"

  static const char
    *CommandMenu[]=
    {
      "Animate",
      "Speed",
      "Direction",
      "Help",
      "Image Info",
      "Quit",
      (char *) NULL
    },
    *AnimateMenu[]=
    {
      "Open",
      "Play",
      "Step",
      "Repeat",
      "Auto Reverse",
      (char *) NULL
    },
    *SpeedMenu[]=
    {
      "Faster",
      "Slower",
      (char *) NULL
    },
    *DirectionMenu[]=
    {
      "Forward",
      "Reverse",
      (char *) NULL
    },
    *HelpMenu[]=
    {
      "Overview",
      "Browse Documentation",
      "About Animate",
      (char *) NULL
    };


  static const char
    **Menus[MagickMenus]=
    {
      AnimateMenu,
      SpeedMenu,
      DirectionMenu,
      HelpMenu
    };

  static const CommandType
    CommandMenus[]=
    {
      NullCommand,
      NullCommand,
      NullCommand,
      NullCommand,
      InfoCommand,
      QuitCommand
    },
    CommandTypes[]=
    {
      OpenCommand,
      PlayCommand,
      StepCommand,
      RepeatCommand,
      AutoReverseCommand
    },
    SpeedCommands[]=
    {
      FasterCommand,
      SlowerCommand
    },
    DirectionCommands[]=
    {
      ForwardCommand,
      ReverseCommand
    },
    HelpCommands[]=
    {
      HelpCommand,
      BrowseDocumentationCommand,
      VersionCommand
    };

  static const CommandType
    *Commands[MagickMenus]=
    {
      CommandTypes,
      SpeedCommands,
      DirectionCommands,
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
    *image,
    **image_list,
    *nexus;

  int
    status;

  unsigned int
    iterations;

  long
    first_scene,
    scene,
    x,
    y;

  KeySym
    key_symbol;

  MonitorHandler
    monitor_handler;

  register char
    *p;

  register long
    i;

  static char
    working_directory[MaxTextExtent];

  static unsigned long
    number_windows;

  static MagickXWindowInfo
    *magick_windows[MaxWindows];

  time_t
    timestamp;

  unsigned int
    context_mask,
    coalesce,
    state;

  unsigned long
    height,
    number_scenes,
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
    *pixel,
    scene_info;

  MagickXResourceInfo
    *icon_resources;

  XStandardColormap
    *icon_map,
    *map_info;

  XTextProperty
    window_name;

  XVisualInfo
    *icon_visual,
    *visual_info;

  MagickXWindows
    *windows;

  XWMHints
    *manager_hints;

  assert(images != (Image *) NULL);
  assert(images->signature == MagickSignature);
  monitor_handler=(MonitorHandler) NULL;
  warning_handler=(WarningHandler) NULL;
  windows=MagickXSetWindows((MagickXWindows *) ~0);
  if (windows != (MagickXWindows *) NULL)
    {
      (void) chdir(working_directory);
      monitor_handler=SetMonitorHandler(MagickXMagickMonitor);
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
      register Image
        *p;

      /*
        Initialize window structure.
      */
      for (p=images; p != (Image *) NULL; p=p->next)
      {
        if (p->storage_class == DirectClass)
          {
            resource_info->colors=0;
            break;
          }
        if (p->colors > resource_info->colors)
          resource_info->colors=p->colors;
      }
      windows=MagickXSetWindows(MagickXInitializeWindows(display,resource_info));
      if (windows == (MagickXWindows *) NULL)
        MagickFatalError(ResourceLimitFatalError,MemoryAllocationFailed,
          MagickMsg(XServerFatalError,UnableToCreateXWindow));
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
      for (i=0; i < (long) number_windows; i++)
        magick_windows[i]->id=(Window) NULL;
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
  coalesce=False;
  if (images->next != (Image *) NULL)
    {
      Image
        *next;

      /*
        Determine if the sequence of images have identical page info.
      */
      for (next=images; next != (Image *) NULL; )
      {
        if ((images->columns != next->columns) ||
            (images->rows != next->rows))
          break;
        if ((images->page.x != next->page.x) ||
            (images->page.y != next->page.y))
          break;
        next=next->next;
      }
      coalesce=next != (Image *) NULL;
      if (coalesce)
        {
          Image
            *coalesce_image;

          coalesce_image=CoalesceImages(images,&images->exception);
          if (coalesce_image == (Image *) NULL)
            MagickFatalError2(images->exception.severity,
              images->exception.reason,images->exception.description);
          images=coalesce_image;
        }
    }
  if (resource_info->map_type == (char *) NULL)
    if ((visual_info->class != TrueColor) &&
        (visual_info->class != DirectColor))
      {
        Image
          *next;

        /*
          Determine if the sequence of images has the identical colormap.
        */
        for (next=images; next != (Image *) NULL; )
        {
          next->matte=False;
          if ((next->storage_class == DirectClass) ||
              (next->colors != images->colors) ||
              (next->colors > (unsigned long) visual_info->colormap_size))
            break;
          for (i=0; i < (long) images->colors; i++)
            if (NotColorMatch(next->colormap+i,images->colormap+i))
              break;
          if (i < (long) images->colors)
            break;
          next=next->next;
        }
        if (next != (Image *) NULL)
          (void) MapImages(images,(Image *) NULL,
            resource_info->quantize_info->dither);
      }
  /*
    Sort images by increasing scene number.
  */
  number_scenes=GetImageListLength(images);
  image_list=ImageListToArray(images,&images->exception);
  if (image_list == (Image **) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToAnimateImageSequence);
  for (scene=0; scene < (long) number_scenes; scene++)
    if (image_list[scene]->scene == 0)
      break;
  if (scene == (long) number_scenes)
    qsort((void *) image_list,number_scenes,sizeof(Image *),SceneCompare);
  /*
    Initialize Standard Colormap.
  */
  nexus=(Image *) NULL;
  display_image=image_list[0];
  (void) TransformColorspace(display_image,RGBColorspace);
  for (scene=0; scene < (long) number_scenes; scene++)
  {
    if ((resource_info->map_type != (char *) NULL) ||
        (visual_info->class == TrueColor) ||
        (visual_info->class == DirectColor))
      (void) SetImageType(image_list[scene],TrueColorType);
    if ((display_image->columns < image_list[scene]->columns) &&
        (display_image->rows < image_list[scene]->rows))
      display_image=image_list[scene];
  }
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
    (void) LogMagickEvent(X11Event,GetMagickModule(),
      "Window id: 0x%lx (context)",windows->context.id);
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
      MagickFreeMemory(windows->image.name);
      MagickFreeMemory(windows->image.icon_name);
    }
  MagickXGetWindowInfo(display,visual_info,map_info,pixel,font_info,
    resource_info,&windows->image);
  windows->image.shape=True;  /* non-rectangular shape hint */
  windows->image.shared_memory&=resource_info->use_shared_memory;
  if (resource_info->title != (char *) NULL)
    {
      windows->image.name=TranslateText(resource_info->image_info,
        display_image,resource_info->title);
      windows->image.icon_name=TranslateText(resource_info->image_info,
        display_image,resource_info->title);
    }
  else
    {
      char
        filename[MaxTextExtent];

      /*
        Window name is the base of the filename.
      */
      windows->image.name=AllocateString((char *) NULL);
      windows->image.icon_name=AllocateString((char *) NULL);
      GetPathComponent(display_image->filename,TailPath,filename);
      FormatString(windows->image.name,"GraphicsMagick: %.1024s[%lu of %lu]",
        filename,display_image->scene,number_scenes);
      (void) strlcpy(windows->image.icon_name,filename,MaxTextExtent);
    }
  if (resource_info->immutable)
    windows->image.immutable=True;
  windows->image.shape=True;
  windows->image.geometry=resource_info->image_geometry;
  FormatString(geometry,"%ux%u+0+0>!",
    90*XDisplayWidth(display,visual_info->screen)/100,
    90*XDisplayHeight(display,visual_info->screen)/100);
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
      manager_hints->flags|=(unsigned int) WindowGroupHint;
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
  MagickXGetWindowInfo(display,visual_info,map_info,pixel,font_info,
    resource_info,&windows->info);
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
  if (IsEventLogging())
    (void) LogMagickEvent(X11Event,GetMagickModule(),"Window id: 0x%lx (command)",
      windows->command.id);
  /*
    Initialize Widget window.
  */
  if (windows->widget.id != (Window) NULL)
    MagickFreeMemory(windows->widget.name);
  MagickXGetWindowInfo(display,visual_info,map_info,pixel,font_info,
    resource_info,&windows->widget);
  FormatString(resource_name,"%.1024s.widget",resource_info->client_name);
  windows->widget.geometry=MagickXGetResourceClass(resource_info->resource_database,
    resource_name,"geometry",(char *) NULL);
  windows->widget.name=AllocateString((char *) NULL);
  *windows->widget.name='\0';
  windows->widget.border_width=0;
  windows->widget.flags|=PPosition;
  windows->widget.attributes.backing_store=WhenMapped;
  windows->widget.attributes.save_under=True;
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
    MagickFreeMemory(windows->popup.name);
  MagickXGetWindowInfo(display,visual_info,map_info,pixel,font_info,
    resource_info,&windows->popup);
  windows->popup.name=AllocateString((char *) NULL);
  *windows->popup.name='\0';
  windows->popup.border_width=0;
  windows->popup.flags|=PPosition;
  windows->popup.attributes.backing_store=WhenMapped;
  windows->popup.attributes.save_under=True;
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
  if (!windows->image.mapped || (windows->backdrop.id != (Window) NULL))
    (void) XMapWindow(display,windows->image.id);
  /*
    Set out progress and warning handlers.
  */
  if (monitor_handler == (MonitorHandler) NULL)
    monitor_handler=SetMonitorHandler(MagickXMagickMonitor);
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
    Initialize X image structure.
  */
  windows->image.x=0;
  windows->image.y=0;
  status=MagickXMakeImage(display,resource_info,&windows->image,display_image,
    (unsigned int) display_image->columns,(unsigned int) display_image->rows);
  if (status == False)
    MagickFatalError(XServerFatalError,UnableToCreateXImage,(char *) NULL);
  if (windows->image.mapped)
    MagickXRefreshWindow(display,&windows->image,(XEvent *) NULL);
  /*
    Initialize image pixmaps structure.
  */
  (void) XMapWindow(display,windows->image.id);
  windows->image.pixmaps=MagickAllocateMemory(Pixmap *,number_scenes*sizeof(Pixmap));
  windows->image.matte_pixmaps=MagickAllocateMemory(Pixmap *,
    number_scenes*sizeof(Pixmap));
  if ((windows->image.pixmaps == (Pixmap *) NULL) ||
      (windows->image.matte_pixmaps == (Pixmap *) NULL))
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToAnimateImageSequence);
  windows->image.pixmaps[0]=windows->image.pixmap;
  windows->image.matte_pixmaps[0]=windows->image.matte_pixmap;
  scene_info.pixels=(unsigned long *) NULL;
  for (scene=1; scene < (long) number_scenes; scene++)
  {
    /*
      Create X image.
    */
    (void) TransformColorspace(image_list[scene],RGBColorspace);
    windows->image.pixmap=(Pixmap) NULL;
    windows->image.matte_pixmap=(Pixmap) NULL;
    if ((resource_info->map_type != (char *) NULL) ||
        (visual_info->class == TrueColor) ||
        (visual_info->class == DirectColor))
      if (image_list[scene]->storage_class == PseudoClass)
        {
          /*
            Get pixel info for this scene.
          */
          MagickXGetPixelPacket(display,visual_info,map_info,resource_info,
                          image_list[scene],&scene_info);
          windows->image.pixel_info=(&scene_info);
        }
    status=MagickXMakeImage(display,resource_info,&windows->image,image_list[scene],
      (unsigned int) image_list[scene]->columns,
      (unsigned int) image_list[scene]->rows);
    if (status == False)
      MagickFatalError(XServerFatalError,UnableToCreateXImage,(char *) NULL);
    if (IsEventLogging())
      {
        (void) LogMagickEvent(X11Event,GetMagickModule(),
          "Image: [%lu] %.1024s %lux%lu ",image_list[scene]->scene,
          image_list[scene]->filename,image_list[scene]->columns,
          image_list[scene]->rows);
        if (image_list[scene]->colors != 0)
          (void) LogMagickEvent(X11Event,GetMagickModule(),"%uc ",
            image_list[scene]->colors);
        (void) LogMagickEvent(X11Event,GetMagickModule(),"%.1024s",
          image_list[scene]->magick);
      }
    /*
      Window name is the base of the filename.
    */
    if (resource_info->title != (char *) NULL)
      windows->image.name=TranslateText(resource_info->image_info,
        image_list[scene],resource_info->title);
    else
      {
        p=image_list[scene]->filename+strlen(image_list[scene]->filename)-1;
        while ((p > image_list[scene]->filename) && (*(p-1) != '/'))
          p--;
        FormatString(windows->image.name,"GraphicsMagick: %.1024s[%lu of %lu]",p,
          scene,number_scenes);
      }
    status=XStringListToTextProperty(&windows->image.name,1,&window_name);
    if (status != 0)
      {
        XSetWMName(display,windows->image.id,&window_name);
        (void) XFree((void *) window_name.value);
      }
    windows->image.pixmaps[scene]=windows->image.pixmap;
    windows->image.matte_pixmaps[scene]=windows->image.matte_pixmap;
    event.xexpose.x=0;
    event.xexpose.y=0;
    event.xexpose.width=(unsigned int) image_list[scene]->columns;
    event.xexpose.height=(unsigned int) image_list[scene]->rows;
    MagickXRefreshWindow(display,&windows->image,&event);
    MagickXDelay(display,(unsigned long) resource_info->delay*10*Max(images->delay,1));
  }
  if (windows->command.mapped)
    (void) XMapRaised(display,windows->command.id);
  /*
    Respond to events.
  */
  nexus=(Image *) NULL;
  scene=0;
  first_scene=0;
  iterations=0;
  image=image_list[0];
  state=ForwardAnimationState | RepeatAnimationState;
  (void) MagickXMagickCommand(display,resource_info,windows,PlayCommand,&images,
    &state);
  do
  {
    if (XEventsQueued(display,QueuedAfterFlush) == 0)
      if ((state & PlayAnimationState) || (state & StepAnimationState))
        {
          if (state & ForwardAnimationState)
            {
              /*
                Forward animation:  increment scene number.
              */
              if (scene < ((long) number_scenes-1))
                scene++;
              else
                {
                  iterations++;
                  if (iterations == image_list[0]->iterations)
                    {
                      iterations=0;
                      state&=(~RepeatAnimationState);
                    }
                  if (state & AutoReverseAnimationState)
                    {
                      state&=(~ForwardAnimationState);
                      scene--;
                    }
                  else
                    {
                      if (!(state & RepeatAnimationState))
                        state&=(~PlayAnimationState);
                      scene=first_scene;
                      (void) sleep(resource_info->pause);
                    }
                }
            }
          else
            {
              /*
                Reverse animation:  decrement scene number.
              */
              if (scene > first_scene)
                scene--;
              else
                {
                  iterations++;
                  if (iterations == image_list[0]->iterations)
                    {
                      iterations=0;
                      state&=(~RepeatAnimationState);
                    }
                  if (state & AutoReverseAnimationState)
                    {
                      state|=ForwardAnimationState;
                      scene=first_scene;
                      (void) sleep(resource_info->pause);
                    }
                  else
                    {
                      if (!(state & RepeatAnimationState))
                        state&=(~PlayAnimationState);
                      scene=(long) number_scenes-1;
                    }
                }
            }
          image=image_list[scene];
          if ((image != (Image *) NULL) && image->start_loop)
            first_scene=scene;
          if ((state & StepAnimationState) ||
              (resource_info->title != (char *) NULL))
            {
              /*
                Update window title.
              */
              p=image_list[scene]->filename+
                strlen(image_list[scene]->filename)-1;
              while ((p > image_list[scene]->filename) && (*(p-1) != '/'))
                p--;
              FormatString(windows->image.name,
                "GraphicsMagick: %.1024s[%lu of %lu]",p,scene,number_scenes);
              if (resource_info->title != (char *) NULL)
                windows->image.name=TranslateText(resource_info->image_info,
                  image,resource_info->title);
              status=
                XStringListToTextProperty(&windows->image.name,1,&window_name);
              if (status != 0)
                {
                  XSetWMName(display,windows->image.id,&window_name);
                  (void) XFree((void *) window_name.value);
                }
            }
          /*
            Copy X pixmap to Image window.
          */
          MagickXGetPixelPacket(display,visual_info,map_info,resource_info,
            image_list[scene],&scene_info); /* FIXME leak here */
          windows->image.pixel_info=(&scene_info);
          windows->image.ximage->width=(unsigned int) image->columns;
          windows->image.ximage->height=(unsigned int) image->rows;
          windows->image.pixmap=windows->image.pixmaps[scene];
          windows->image.matte_pixmap=windows->image.matte_pixmaps[scene];
          event.xexpose.x=0;
          event.xexpose.y=0;
          event.xexpose.width=(unsigned int) image->columns;
          event.xexpose.height=(unsigned int) image->rows;
          MagickXRefreshWindow(display,&windows->image,&event);
          (void) XSync(display,False);
          state&=(~StepAnimationState);
          MagickXDelay(display,(unsigned long) resource_info->delay*10*
            Max(image->delay,1));
          continue;
        }
    /*
      Handle a window event.
    */
    timestamp=time((time_t *) NULL);
    (void) XNextEvent(display,&event);
    if (!windows->image.stasis)
      windows->image.stasis=(time((time_t *) NULL)-timestamp) > 0;
    if (event.xany.window == windows->command.id)
      {
        int
          id;

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
            int
              entry;

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
          nexus=MagickXMagickCommand(display,resource_info,windows,
            command_type,&image,&state);
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
            if (resource_info->immutable)
              {
                state|=ExitState;
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
          }
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
            if (*event.xclient.data.l == (long) windows->im_update_colormap)
              {
                /*
                  Update graphic context and window colormap.
                */
                for (i=0; i < (long) number_windows; i++)
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
                if (windows->backdrop.id != (Window) NULL)
                  (void) XInstallColormap(display,map_info->colormap);
                break;
              }
            if (*event.xclient.data.l == (long) windows->im_exit)
              {
                state|=ExitState;
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
            status=XGetWindowProperty(display,root_window,selection,0L,2047L,
              False,(Atom) AnyPropertyType,&type,&format,&length,&after,&data);
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
                if (LocaleNCompare((char *) data,"file:",5) != 0)
                  {
                    (void) XFree((void *) data);
                    break;
                  }
                (void) strlcpy(resource_info->image_info->filename,
                  ((char *) data)+5,MaxTextExtent);
              }
            nexus=ReadImage(resource_info->image_info,&image->exception);
            if (image->exception.severity != UndefinedException)
              MagickError2(image->exception.severity,image->exception.reason,
                image->exception.description);
            if (nexus != (Image *) NULL)
              state|=ExitState;
            (void) XFree((void *) data);
            break;
          }
        /*
          If client window delete message, exit.
        */
        if (event.xclient.message_type != windows->wm_protocols)
          break;
        if (*event.xclient.data.l == (long) windows->wm_take_focus)
          {
            (void) XSetInputFocus(display,event.xclient.window,RevertToParent,
              event.xclient.data.l[1]);
            break;
          }
        if (*event.xclient.data.l != (long) windows->wm_delete_window)
          break;
        (void) XWithdrawWindow(display,event.xclient.window,
          visual_info->screen);
        if (event.xclient.window == windows->image.id)
          {
            state|=ExitState;
            break;
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
              }
            /*
              Image window has a new configuration.
            */
            windows->image.width=event.xconfigure.width;
            windows->image.height=event.xconfigure.height;
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
            state|=ExitState;
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
          Repaint windows that are now exposed.
        */
        if (event.xexpose.window == windows->image.id)
          {
            windows->image.pixmap=windows->image.pixmaps[scene];
            windows->image.matte_pixmap=windows->image.matte_pixmaps[scene];
            MagickXRefreshWindow(display,&windows->image,&event);
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
        static int
          length;

        /*
          Respond to a user key press.
        */
        length=XLookupString((XKeyEvent *) &event.xkey,command,sizeof(command),
          &key_symbol,(XComposeStatus *) NULL);
        *(command+length)='\0';
        if (IsEventLogging())
          (void) LogMagickEvent(X11Event,GetMagickModule(),"Key press: 0x%lx (%c)",
            key_symbol,*command);
        command_type=NullCommand;
        switch (key_symbol)
        {
          case XK_o:
          {
            if (!(event.xkey.state & ControlMask))
              break;
            command_type=OpenCommand;
            break;
          }
          case XK_BackSpace:
          {
            command_type=StepBackwardCommand;
            break;
          }
          case XK_space:
          {
            command_type=StepForwardCommand;
            break;
          }
          case XK_less:
          {
            command_type=FasterCommand;
            break;
          }
          case XK_greater:
          {
            command_type=SlowerCommand;
            break;
          }
          case XK_F1:
          {
            command_type=HelpCommand;
            break;
          }
          case XK_Find:
          {
            command_type=BrowseDocumentationCommand;
            break;
          }
          case XK_question:
          {
            command_type=InfoCommand;
            break;
          }
          case XK_q:
          case XK_Cancel:
          {
            if (!(event.xkey.state & ControlMask))
              break;
            command_type=QuitCommand;
            break;
          }
          default:
            break;
        }
        if (command_type != NullCommand)
          nexus=MagickXMagickCommand(display,resource_info,windows,
            command_type,&image,&state);
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
            if (LocaleCompare(image_list[0]->magick,"LOGO") == 0)
              {
                if (LocaleCompare(display_image->filename,"Untitled") == 0)
                  nexus=MagickXMagickCommand(display,resource_info,windows,
                    OpenCommand,&image,&state);
                else
                  state|=ExitState;
              }
            windows->image.mapped=True;
            break;
          }
        if (event.xmap.window == windows->info.id)
          {
            windows->info.mapped=True;
            break;
          }
        if (event.xmap.window == windows->icon.id)
          {
            /*
              Create an icon image.
            */
            MagickXMakeStandardColormap(display,icon_visual,icon_resources,
              display_image,icon_map,icon_pixel);
            (void) MagickXMakeImage(display,icon_resources,&windows->icon,
              display_image,windows->icon.width,windows->icon.height);
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
        nexus=ReadImage(resource_info->image_info,&image->exception);
        if (image->exception.severity != UndefinedException)
          MagickError2(image->exception.severity,image->exception.reason,
            image->exception.description);
        if (nexus != (Image *) NULL)
          state|=ExitState;
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
  while (!(state & ExitState));
  MagickFreeMemory(image_list);
  if (coalesce)
    {
      DestroyImageList(images);
      images=(Image *) NULL;
    }
  if ((windows->visual_info->class == GrayScale) ||
      (windows->visual_info->class == PseudoColor) ||
      (windows->visual_info->class == DirectColor))
    {
      /*
        Withdraw windows.
      */
      if (windows->info.mapped)
        (void) XWithdrawWindow(display,windows->info.id,windows->info.screen);
      if (windows->command.mapped)
        (void) XWithdrawWindow(display,windows->command.id,
          windows->command.screen);
    }
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
  MagickXSetCursorState(display,windows,True);
  MagickXCheckRefreshWindows(display,windows);
  for (scene=1; scene < (long) number_scenes; scene++)
  {
    if (windows->image.pixmaps[scene] != (Pixmap) NULL)
      (void) XFreePixmap(display,windows->image.pixmaps[scene]);
    windows->image.pixmaps[scene]=(Pixmap) NULL;

    if (windows->image.matte_pixmaps[scene] != (Pixmap) NULL)
      (void) XFreePixmap(display,windows->image.matte_pixmaps[scene]);
    windows->image.matte_pixmaps[scene]=(Pixmap) NULL;
  }
  MagickFreeMemory(windows->image.pixmaps);
  MagickFreeMemory(windows->image.matte_pixmaps);
  if (nexus == (Image *) NULL)
    {
      /*
        Destroy X windows.
      */
      if (windows->image.mapped)
        (void) XWithdrawWindow(display,windows->image.id,windows->image.screen);
      MagickXDelay(display,SuspendTime);

      /*
        Free Standard Colormap.
      */
      (void) MagickXFreeStandardColormap(display,icon_visual,icon_map,icon_pixel);
      if (resource_info->map_type == (char *) NULL)
        (void) MagickXFreeStandardColormap(display,visual_info,map_info,pixel);
      /*
        Free X resources.
      */
      MagickXDestroyXWindows(windows);
    }
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
  return(nexus);
}
#endif
