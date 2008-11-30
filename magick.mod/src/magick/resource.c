/*
% Copyright (C) 2003, 2008 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%           RRRR    EEEEE   SSSSS   OOO   U   U  RRRR    CCCC  EEEEE          %
%           R   R   E       SS     O   O  U   U  R   R  C      E              %
%           RRRR    EEE      SSS   O   O  U   U  RRRR   C      EEE            %
%           R R     E          SS  O   O  U   U  R R    C      E              %
%           R  R    EEEEE   SSSSS   OOO    UUU   R  R    CCCC  EEEEE          %
%                                                                             %
%                                                                             %
%                        Get/Set ImageMagick Resources.                       %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                               September 2002                                %
%                                                                             %
%                            Completely Re-written                            %
%                               Bob Friesenhahn                               %
%                                 April 2008                                  %
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
#include "magick/log.h"
#include "magick/resource.h"
#include "magick/semaphore.h"
#include "magick/utility.h"

/*
  Define  declarations.
*/
#define ResourceInfinity  (-1)
#define ResourceInfoMaxIndex ((unsigned int) PixelsResource)

/*
  Typedef declarations.
*/
typedef enum
{
  AbsoluteLimit,
  SummationLimit
} LimitType;

/*
  Definition of a resource.  We intentionally use signed types to
  store 'value' and 'limit'.
*/
typedef struct _ResourceInfo
{
  const char
    *name,
    *units;

  magick_int64_t
    value;

  magick_int64_t
    limit;

  LimitType
    limit_type;

} ResourceInfo;

/*
  Global declarations.
*/
static SemaphoreInfo
  *resource_semaphore = (SemaphoreInfo *) NULL;

static ResourceInfo
  resource_info[ResourceInfoMaxIndex+1] =
  {
    { "",       "",  0, ResourceInfinity, SummationLimit },
    { "disk",   "B", 0, ResourceInfinity, SummationLimit },
    { "file",   "",  0, 256,              SummationLimit },
    { "map",    "B", 0, ResourceInfinity, SummationLimit },
    { "memory", "B", 0, ResourceInfinity, SummationLimit },
    { "pixels", "P", 0, ResourceInfinity, AbsoluteLimit  }
  };

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A c q u i r e M a g i c k R e s o u r c e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AcquireMagickResource() acquires resources of the specified type.  True is
%  returned if the specified resource is available otherwise False.
%
%  The format of the AcquireMagickResource() method is:
%
%      MagickPassFail AcquireMagickResource(const ResourceType type,
%        const magick_int64_t size)
%
%  A description of each parameter follows:
%
%    o type: The type of resource.
%
%    o size: The number of bytes needed from for this resource.
%
%
*/
static ResourceInfo *GetResourceInfo(const ResourceType type)
{
  ResourceInfo
    *info;

  unsigned int
    index;

  info = (ResourceInfo *) NULL;
  index = type;
  if (index <= ResourceInfoMaxIndex)
    info = &resource_info[index];

  return info;
}

MagickExport MagickPassFail AcquireMagickResource(const ResourceType type,
                                                  const magick_uint64_t size)
{
  magick_uint64_t
    value;

  ResourceInfo
    *info;

  MagickPassFail
    status;

  status=MagickPass;

  AcquireSemaphoreInfo(&resource_semaphore);

  if ((info=GetResourceInfo(type)))
    {
      switch(info->limit_type)
        {
        case AbsoluteLimit:
          {
            /*
              Limit depends only on the currently requested size.
            */
            if ((info->limit != ResourceInfinity) &&
                (size > (magick_uint64_t) info->limit))
              status=MagickFail;
            break;
          }
        case SummationLimit:
          {
            /*
              Limit depends on sum of previous allocations as well as
              the currently requested size.
            */
            value=info->value+size;
            if ((info->limit != ResourceInfinity) &&
                (value > (magick_uint64_t) info->limit))
              status=MagickFail;
            else
              info->value=value;

            break;
          }
        }

      if (IsEventLogging())
        {
          char
            f_limit[MaxTextExtent],
            f_size[MaxTextExtent],
            f_value[MaxTextExtent];

          if (info->limit == ResourceInfinity)
            {
              strlcpy(f_limit,"Unlimited",sizeof(f_limit));
            }
          else
            {
              FormatSize(info->limit, f_limit);
              strlcat(f_limit,info->units,sizeof(f_limit));
            }

          FormatSize(size, f_size);
          strlcat(f_size,info->units,sizeof(f_size));

          if (info->limit_type == AbsoluteLimit)
            {
              strlcpy(f_value,"----",sizeof(f_value));
            }
          else
            {
              FormatSize(info->value, f_value);
              strlcat(f_value,info->units,sizeof(f_value));
            }

          (void) LogMagickEvent(ResourceEvent,GetMagickModule(),
                                "%s %s%s/%s/%s",
                                info->name,
                                (status ? "+" : "!"),
                                f_size,
                                f_value,
                                f_limit);
        }
    }

  LiberateSemaphoreInfo(&resource_semaphore);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y M a g i c k R e s o u r c e s                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyMagickResources() destroys the resource environment.
%
%  The format of the DestroyMagickResources() method is:
%
%      DestroyMagickResources(void)
%
%
*/
MagickExport void DestroyMagickResources(void)
{
#if defined(JUST_FOR_DOCUMENTATION)
  /* The first two calls should bracket any code that deals with the data
     structurees being released */
  AcquireSemaphoreInfo(&resource_semaphore);
  LiberateSemaphoreInfo(&resource_semaphore);
#endif
  /* The final call actually releases the associated mutex used to prevent
     multiple threads from accessing the data */
  DestroySemaphoreInfo(&resource_semaphore);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t M a g i c k R e s o u r c e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetMagickResource() returns the current consumption level for the
%  specified resource type.
%
%  The format of the GetMagickResource() method is:
%
%      magick_uint64_t GetMagickResource(const ResourceType type)
%
%  A description of each parameter follows:
%
%    o type: The type of resource.
%
%
*/
MagickExport magick_int64_t GetMagickResource(const ResourceType type)
{
  ResourceInfo
    *info;

  magick_int64_t
    resource;

  resource=0;
  AcquireSemaphoreInfo(&resource_semaphore);

  if ((info=GetResourceInfo(type)))
    resource=info->value;

  LiberateSemaphoreInfo(&resource_semaphore);
  return(resource);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   I n i t i a l i z e M a g i c k R e s o u r c e s                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  InitializeMagickResources() initializes the resource environment.
%
%  The format of the InitializeMagickResources() method is:
%
%      InitializeMagickResources(void)
%
%
*/
MagickExport void InitializeMagickResources(void)
{
  magick_int64_t
    max_disk=-1,
    max_files=256,
    max_map=4096,
    max_memory=1024,
    max_pixels=-1;

  /*
    Set Magick resource limits.
  */
#if defined(POSIX)
  {
    long
      files=-1;

    unsigned long
      total_memory=0;

#  if defined(HAVE_SYSCONF) && defined(_SC_OPEN_MAX)
    files=sysconf(_SC_OPEN_MAX);
#  endif

#  if  defined(HAVE_SYSCONF) && defined(_SC_PHYS_PAGES)
    {
    long
      pagesize=-1,
      pages=-1;
    /*
      Compute total physical memory based on number of memory pages,
      and page size.
    */
    pages=sysconf(_SC_PHYS_PAGES);
    pagesize = MagickGetMMUPageSize();

    if (pages > 0 && pagesize > 0)
      total_memory=((pages+512)/1024)*((pagesize+512)/1024);
    (void) LogMagickEvent(ResourceEvent,GetMagickModule(),
      "Total physical memory %ld MB (%ld pages and %ld bytes per page)",
        total_memory, pages, pagesize);
    }
#  elif defined(MAGICK_PHYSICAL_MEMORY_COMMAND) && defined(HAVE_POPEN)
    {
      double
        bytes=0;

      FILE
        *command;
      /*
        Execute the external command defined by
        MAGICK_PHYSICAL_MEMORY_COMMAND to obtain the total physical
        memory in bytes.  This external command should be quite speedy
        or else it will impact the time to start GraphicsMagick.
      */
      if ((command = popen(MAGICK_PHYSICAL_MEMORY_COMMAND, "r")) != NULL)
        {
          if (fscanf(command,"%lf",&bytes) == 1)
            total_memory=(bytes/(1024*1024));
          (void) pclose(command);
          (void) LogMagickEvent(ResourceEvent,GetMagickModule(),
            "Total physical memory %ld MB",total_memory);
        }
    }
#  endif

    if (files > 0)
      max_files=files/2;

    if (total_memory > 0)
      {
        /*
          We are not able to use more than size_t worth of address
          space.  On 32 bit systems, we can usually only use half or
          three quarters of it.  Regardless, we need address space
          for more than just image data.
        */
        size_t
          size_t_max = (size_t) ~0UL;

        size_t_max /= (1024U*1024U);
        max_memory = Min(size_t_max/2-100,total_memory);
        max_map = Min(size_t_max/2-100,2*total_memory);
      }
  }
#endif /* defined(POSIX) */

#if defined(MSWINDOWS)
  {
    long
      total_physical_memory=0,
      total_virtual_memory=0;

    /*
      GlobalMemoryStatusEx is necessary to handle results for
      large-memory (>4GB) machines (and to provide accurate results
      for 2 to 4 GB of memory), but it is not available on older
      versions of Windows.  Windows 32-bit applications are still
      usually limited to addressing only 2 GB of memory even if the
      system provides more (except for certain server versions of
      Windows with applications linked with the /LARGEADDRESSAWARE
      option). Test for API existence prior to using it.  MinGW
      headers currently lack support for this API so compilation of
      supportive code is made optional.
    */
#if defined(HAVE_GLOBALMEMORYSTATUSEX)
    if (NTKernelAPISupported("GlobalMemoryStatusEx"))
      {
        MEMORYSTATUSEX
          stat_ex;

        if (GlobalMemoryStatusEx(&stat_ex))
          {
            total_physical_memory=(long)(stat_ex.ullTotalPhys/1048576UL);
            total_virtual_memory=(long)(stat_ex.ullTotalVirtual/1048576UL);
          }
      }
#endif

    if (total_physical_memory == 0)
    {
      MEMORYSTATUS
        stat;

      GlobalMemoryStatus(&stat);
      total_physical_memory=stat.dwTotalPhys/1048576;
      total_virtual_memory=stat.dwTotalVirtual/1048576;
    }

    if (total_virtual_memory > 3*total_physical_memory)
      max_memory=2*total_physical_memory;
    else
      max_memory=(long)(0.7*total_virtual_memory);
    max_map=(long)(8*total_physical_memory);
    (void) LogMagickEvent(ResourceEvent,GetMagickModule(),
      "Total physical memory %ld MB, Total virtual memory %ld MB",
        total_physical_memory, total_virtual_memory);

    /*
      Windows lowio level supports up to 2048 open files.
      Reserve 512 handles for other uses.
    */
    max_files=2048-512;
  }
#endif /* defined(MSWINDOWS) */

  /*
    Compute fundamental limits based on build parameters.  Image size
    is fundamentally limited by how many pixels may reside in one row.
  */
  if (sizeof(size_t) == 4)
    max_pixels=(magick_int64_t) ((unsigned long) ULONG_MAX/(5*sizeof(Quantum)));


  /*
    Disk, map, and memory are in units of MB and need to be scaled up.
  */
  if (max_disk > 0)
    max_disk  *= 1024UL*1024UL;
  if (max_map > 0)
    max_map   *= 1024UL*1024UL;
  if (max_memory > 0)
    max_memory *= 1024UL*1024UL;

  /*
    Support using environment variables to set limits
  */
  {
    const char
      *envp;

    if ((envp=getenv("MAGICK_LIMIT_DISK")))
      max_disk=MagickSizeStrToInt64(envp,1024);

    if ((envp=getenv("MAGICK_LIMIT_FILES")))
      max_files=MagickSizeStrToInt64(envp,1024);

    if ((envp=getenv("MAGICK_LIMIT_MAP")))
      max_map=MagickSizeStrToInt64(envp,1024);

    if ((envp=getenv("MAGICK_LIMIT_MEMORY")))
      max_memory=MagickSizeStrToInt64(envp,1024);

    if ((envp=getenv("MAGICK_LIMIT_PIXELS")))
      max_pixels=MagickSizeStrToInt64(envp,1024);
  }

  if (max_disk >= 0)
    (void) SetMagickResourceLimit(DiskResource,max_disk);
  if (max_files >= 0)
    (void) SetMagickResourceLimit(FileResource,max_files);
  if (max_map >= 0)
    (void) SetMagickResourceLimit(MapResource,max_map);
  if (max_memory >= 0)
    (void) SetMagickResourceLimit(MemoryResource,max_memory);
  if (max_pixels >= 0)
    (void) SetMagickResourceLimit(PixelsResource,max_pixels);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L i b e r a t e M a g i c k R e s o u r c e                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  LiberateMagickResource() liberates resources of the specified type.
%
%  The format of the LiberateMagickResource() method is:
%
%      void LiberateMagickResource(const ResourceType type,
%        const magick_int64_t size)
%
%  A description of each parameter follows:
%
%    o type: The type of resource.
%
%    o size: The size of the resource.
%
%
*/
MagickExport void LiberateMagickResource(const ResourceType type,
  const magick_uint64_t size)
{
  ResourceInfo
    *info;

  AcquireSemaphoreInfo(&resource_semaphore);

  if ((info=GetResourceInfo(type)))
    {
      switch(info->limit_type)
        {
        case AbsoluteLimit:
          {
            /*
              Limit depends only on the currently requested size.
            */
            info->value=0;
            break;
          }
        case SummationLimit:
          {
            /*
              Limit depends on sum of previous allocations as well as
              the currently requested size.
            */
            info->value-=size;
            break;
          }
        }


      if (IsEventLogging())
        {
          char
            f_limit[MaxTextExtent],
            f_size[MaxTextExtent],
            f_value[MaxTextExtent];

          if (info->limit == ResourceInfinity)
            {
              strlcpy(f_limit,"Unlimited",sizeof(f_limit));
            }
          else
            {
              FormatSize(info->limit, f_limit);
              strlcat(f_limit,info->units,sizeof(f_limit));
            }

          FormatSize(size, f_size);
          strlcat(f_size,info->units,sizeof(f_size));

          if (info->limit_type == AbsoluteLimit)
            {
              strlcpy(f_value,"----",sizeof(f_value));
            }
          else
            {
              FormatSize(info->value, f_value);
              strlcat(f_value,info->units,sizeof(f_value));
            }

          (void) LogMagickEvent(ResourceEvent,GetMagickModule(),
                                "%s %s%s/%s/%s",
                                info->name,
                                "-",
                                f_size,
                                f_value,
                                f_limit);
        }
    }

  LiberateSemaphoreInfo(&resource_semaphore);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  L i s t M a g i c k R e s o u r c e I n f o                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ListMagickResourceInfo lists the resource info to a file.
%
%  The format of the ListMagickResourceInfo method is:
%
%      unsigned int ListMagickResourceInfo(FILE *file,ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o file:  An pointer to a FILE.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport MagickPassFail ListMagickResourceInfo(FILE *file,
  ExceptionInfo *ARGUNUSED(exception))
{
  unsigned int
    i,
    index;
  
  AcquireSemaphoreInfo(&resource_semaphore);
  if (file == (const FILE *) NULL)
    file=stdout;

  fprintf(file,"Resource Limits (Q%d, %d bits/pixel, %dbit address)\n",
          QuantumDepth,4*QuantumDepth,
          (sizeof(PixelPacket *) > 4 ? 64 : 32));
  fprintf(file,"----------------------------------------------------\n");
  for (index=1 ; index <= ResourceInfoMaxIndex; index++)
    {
      char
        environment[MaxTextExtent],
        heading[MaxTextExtent],
        limit[MaxTextExtent];

      if (resource_info[index].limit == ResourceInfinity)
        {
          strlcpy(limit,"Unlimited",sizeof(limit));
        }
      else
        {
          FormatSize(resource_info[index].limit,limit);
          strlcat(limit,resource_info[index].units,sizeof(limit));
        }
      FormatString(heading,"%c%s",toupper(resource_info[index].name[0]),
                   resource_info[index].name+1);

      for (i=0; resource_info[index].name[i] != 0; i++)
        environment[i]=toupper(resource_info[index].name[i]);
      environment[i]='\0';

      fprintf(file,"%8s: %10s (MAGICK_LIMIT_%s)\n", heading, limit, environment);
    }
  (void) fflush(file);

  LiberateSemaphoreInfo(&resource_semaphore);
  return(MagickPass);
}
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t M a g i c k R e s o u r c e L i m i t                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetMagickResourceLimit() sets the limit for a particular resource.  The
%  units for resource types are as follows:
%
%    DiskResource    -- Gigabytes
%    FileResource    -- Open files
%    MapResource     -- Megabytes
%    MemoryResource  -- Megabytes
%    PixelsResource  -- Megapixels
%
%  The format of the SetMagickResourceLimit() method is:
%
%      void SetMagickResourceLimit(const ResourceType type,
%        const unsigned long limit)
%
%  A description of each parameter follows:
%
%    o type: The type of resource.
%
%    o limit: The maximum limit for the resource.
%
%
*/
MagickExport MagickPassFail SetMagickResourceLimit(const ResourceType type,
                                                   const magick_int64_t limit)
{
  ResourceInfo
    *info;

  MagickPassFail
    status;

  status=MagickFail;

  AcquireSemaphoreInfo(&resource_semaphore);
  if ((info=GetResourceInfo(type)))
    {
      if (limit >= 0)
        {
          char
            f_limit[MaxTextExtent];

          FormatSize((magick_int64_t) limit, f_limit);
          info->limit = limit;
          (void) LogMagickEvent(ResourceEvent,GetMagickModule(),
                                "Set %s resource limit to %s%s",
                                info->name,f_limit,info->units);
          status=MagickPass;
        }
      else
        {
          (void) LogMagickEvent(ResourceEvent,GetMagickModule(),
                                "Ignored bogus request to set %s resource limit to %ld%s",
                                info->name,(long) limit,info->units);
        }
    }
  LiberateSemaphoreInfo(&resource_semaphore);

  return(status);
}
