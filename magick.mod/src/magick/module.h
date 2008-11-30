/*
  Copyright (C) 2003 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  GraphicsMagick Modules Methods.
*/
#ifndef _MAGICK_MODULE_H
#define _MAGICK_MODULE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Module alias list entry
  Maintains modules.mgk path, and the module name corresponding
  to each magick tag.
  Used to support module_list, which is intialized by reading modules.mgk,
*/
typedef struct _ModuleInfo
{
  char
    *path,		/* Path to modules.mgk which created alias */
    *magick,		/* Format name */
    *name;		/* Name of module supporting format. */

  unsigned int
    stealth;		/* If true, hide when printing module list */

  unsigned long
    signature;

  struct _ModuleInfo
    *previous,
    *next;
} ModuleInfo;

typedef enum
{
  MagickCoderModule,
  MagickFilterModule
} MagickModuleType;

/*
  Modules declarations.
*/
extern MagickExport const ModuleInfo
  *GetModuleInfo(const char *,ExceptionInfo *);

extern MagickExport unsigned int
  ExecuteModuleProcess(const char *,Image **,const int,char **),
  ExecuteStaticModuleProcess(const char *,Image **,const int,char **),
  ListModuleInfo(FILE *,ExceptionInfo *),
  OpenModule(const char *,ExceptionInfo *),
  OpenModules(ExceptionInfo *);

extern MagickExport void
  DestroyModuleInfo(void),
  DestroyMagickModules(void),
  InitializeMagickModules(void),
  RegisterStaticModules(void),
  UnregisterStaticModules(void);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
